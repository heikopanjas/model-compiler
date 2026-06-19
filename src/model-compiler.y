%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include "runtime/Array.h"
#include "runtime/String.h"
#include <fstream>
#include <sstream>
#include "runtime/AST.h"
#include "Console.h"

// Helper function to create heap-allocated C string from std::string
// This is needed because Bison %union only supports POD types
static char* MakeString(const std::string& str)
{
    char* result = new char[str.length() + 1];
    std::memcpy(result, str.c_str(), str.length() + 1);
    return result;
}

// Case-insensitive string comparison (cross-platform)
static int StringCompareIgnoreCase(const char* a, const char* b)
{
#ifdef _WIN32
    return _stricmp(a, b);
#else
    return strcasecmp(a, b);
#endif
}

// External functions from Flex
extern int yylex(void);
extern int yylineno;
extern FILE *yyin;
extern "C" void yyerror(const char *s);

// Global AST root
std::unique_ptr<runtime::AST> g_ast;

// Global filename for error reporting
runtime::String g_current_filename;

// Cache of source file lines for error reporting
runtime::Array<runtime::String> g_source_lines;
%}

%define api.prefix {yy}
%define api.pure false

%locations

%code requires {
    // No special declarations needed
}

// Remove the provides block that was forcing C linkage


// NOTE: This union uses void* (raw pointers) instead of smart pointers
// because Bison's %union only supports POD (Plain Old Data) types.
// std::unique_ptr and std::shared_ptr cannot be stored in a union.
// Raw pointers are created with 'new' and immediately wrapped in
// std::unique_ptr in the grammar actions, ensuring safe ownership transfer.
// This is the standard pattern for Bison parsers in C++.
%union {
    int integer;
    char *string;
    void *ast;
    void *declaration;
    void *enumDecl;
    void *classDecl;
    void *field;
    void *invariant;
    void *typeSpec;
    void *modifier;
    void *expression;
    void *declarationList;
    void *stringList;
    void *fieldList;
    void *invariantList;
    void *modifierList;
    void *namespaceDecl;
}

/* Token declarations */
%token CLASS INHERITS ENUM FEATURE ALIAS INVARIANT OPTIONAL UNIQUE NAMESPACE
%token STRING_TYPE INT_TYPE REAL_TYPE BOOL_TYPE TIMESTAMP_TYPE TIMESPAN_TYPE DATE_TYPE GUID_TYPE
%token LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN
%token SEMICOLON COLON COMMA EQUALS DOT DOTDOT ASTERISK
%token PLUS MINUS SLASH PERCENT
%token LE GE EQ NE LT GT
%token AND OR NOT
%token <string> IDENTIFIER
%token <integer> INTEGER_LITERAL
%token <string> REAL_LITERAL
%token <string> STRING_LITERAL
%token <string> BOOL_LITERAL

%type <ast> program
%type <namespaceDecl> namespace_declaration
%type <declarationList> declaration_list
%type <declaration> declaration
%type <enumDecl> enum_declaration
%type <classDecl> class_declaration
%type <stringList> enum_value_list
%type <fieldList> field_list
%type <invariantList> invariant_list
%type <field> field
%type <invariant> invariant
%type <typeSpec> type_spec
%type <modifierList> modifier_spec modifier_list
%type <modifier> modifier
%type <string> field_name
%type <string> attribute_name
%type <expression> expression primary_expression

/* Operator precedence (lowest to highest) */
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left ASTERISK SLASH PERCENT
%right NOT
%right UNARY_MINUS
%left DOT

%%

program:
    /* empty */
    {
        auto* ast = new runtime::AST("", runtime::Array<std::unique_ptr<runtime::Declaration>>());
        $$ = ast;
        g_ast = std::unique_ptr<runtime::AST>(ast);
    }
    | namespace_declaration declaration_list
    {
        auto* ns = static_cast<runtime::String*>($1);
        auto* list = static_cast<runtime::Array<std::unique_ptr<runtime::Declaration>>*>($2);
        auto* ast = new runtime::AST(*ns, std::move(*list));
        delete ns;
        delete list;
        $$ = ast;
        g_ast = std::unique_ptr<runtime::AST>(ast);
    }
    | declaration_list
    {
        auto* list = static_cast<runtime::Array<std::unique_ptr<runtime::Declaration>>*>($1);
        auto* ast = new runtime::AST("", std::move(*list));
        delete list;
        $$ = ast;
        g_ast = std::unique_ptr<runtime::AST>(ast);
    }
    ;

namespace_declaration:
    NAMESPACE IDENTIFIER SEMICOLON
    {
        $$ = new runtime::String($2);
        free($2);
    }
    ;

declaration_list:
    declaration
    {
        auto* list = new runtime::Array<std::unique_ptr<runtime::Declaration>>();
        list->AddValue(std::unique_ptr<runtime::Declaration>(static_cast<runtime::Declaration*>($1)));
        $$ = list;
    }
    | declaration_list declaration
    {
        auto* list = static_cast<runtime::Array<std::unique_ptr<runtime::Declaration>>*>($1);
        list->AddValue(std::unique_ptr<runtime::Declaration>(static_cast<runtime::Declaration*>($2)));
        $$ = list;
    }
    ;

declaration:
    enum_declaration
    { $$ = new runtime::Declaration(std::unique_ptr<runtime::EnumDeclaration>(static_cast<runtime::EnumDeclaration*>($1))); }
    | class_declaration
    { $$ = new runtime::Declaration(std::unique_ptr<runtime::ClassDeclaration>(static_cast<runtime::ClassDeclaration*>($1))); }
    ;

enum_declaration:
    ENUM IDENTIFIER LBRACE enum_value_list RBRACE
    {
        auto* values = static_cast<runtime::Array<runtime::String>*>($4);
        $$ = new runtime::EnumDeclaration($2, std::move(*values));
        free($2);
        delete values;
    }
    ;

enum_value_list:
    IDENTIFIER
    {
        auto* list = new runtime::Array<runtime::String>();
        list->AddValue($1);
        free($1);
        $$ = list;
    }
    | enum_value_list COMMA IDENTIFIER
    {
        auto* list = static_cast<runtime::Array<runtime::String>*>($1);
        list->AddValue($3);
        free($3);
        $$ = list;
    }
    ;

class_declaration:
    CLASS IDENTIFIER LBRACE field_list invariant_list RBRACE
    {
        auto* fields = static_cast<runtime::Array<std::unique_ptr<runtime::Field>>*>($4);
        auto* invariants = static_cast<runtime::Array<std::unique_ptr<runtime::Invariant>>*>($5);
        $$ = new runtime::ClassDeclaration(runtime::String($2), runtime::String(""), std::move(*fields), std::move(*invariants));
        free($2);
        delete fields;
        delete invariants;
    }
    | CLASS IDENTIFIER INHERITS IDENTIFIER LBRACE field_list invariant_list RBRACE
    {
        auto* fields = static_cast<runtime::Array<std::unique_ptr<runtime::Field>>*>($6);
        auto* invariants = static_cast<runtime::Array<std::unique_ptr<runtime::Invariant>>*>($7);
        $$ = new runtime::ClassDeclaration(runtime::String($2), runtime::String($4), std::move(*fields), std::move(*invariants));
        free($2);
        free($4);
        delete fields;
        delete invariants;
    }
    ;

field_list:
    /* empty */
    { $$ = new runtime::Array<std::unique_ptr<runtime::Field>>(); }
    | field_list field
    {
        auto* list = static_cast<runtime::Array<std::unique_ptr<runtime::Field>>*>($1);
        list->AddValue(std::unique_ptr<runtime::Field>(static_cast<runtime::Field*>($2)));
        $$ = list;
    }
    ;

invariant_list:
    /* empty */
    { $$ = new runtime::Array<std::unique_ptr<runtime::Invariant>>(); }
    | invariant_list invariant
    {
        auto* list = static_cast<runtime::Array<std::unique_ptr<runtime::Invariant>>*>($1);
        list->AddValue(std::unique_ptr<runtime::Invariant>(static_cast<runtime::Invariant*>($2)));
        $$ = list;
    }
    ;

attribute_name:
    IDENTIFIER      { $$ = $1; }
    | STRING_TYPE   { $$ = MakeString("string"); }
    | INT_TYPE      { $$ = MakeString("int"); }
    | REAL_TYPE     { $$ = MakeString("real"); }
    | BOOL_TYPE     { $$ = MakeString("bool"); }
    | TIMESTAMP_TYPE { $$ = MakeString("timestamp"); }
    | TIMESPAN_TYPE { $$ = MakeString("timespan"); }
    | DATE_TYPE     { $$ = MakeString("date"); }
    | GUID_TYPE     { $$ = MakeString("guid"); }
    ;

invariant:
    INVARIANT IDENTIFIER COLON expression SEMICOLON
    {
        $$ = new runtime::Invariant(runtime::String($2),
            std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($4)));
        free($2);
    }
    ;

field_name:
    IDENTIFIER      { $$ = $1; }
    | STRING_TYPE   { $$ = MakeString("string"); }
    | INT_TYPE      { $$ = MakeString("int"); }
    | REAL_TYPE     { $$ = MakeString("real"); }
    | BOOL_TYPE     { $$ = MakeString("bool"); }
    | TIMESTAMP_TYPE { $$ = MakeString("timestamp"); }
    | TIMESPAN_TYPE { $$ = MakeString("timespan"); }
    | DATE_TYPE     { $$ = MakeString("date"); }
    | GUID_TYPE     { $$ = MakeString("guid"); }
    ;

field:
    FEATURE field_name COLON type_spec modifier_spec SEMICOLON
    {
        auto* type = static_cast<runtime::TypeSpec*>($4);
        auto* modifiers = static_cast<runtime::Array<std::unique_ptr<runtime::Modifier>>*>($5);
        $$ = new runtime::Field(
            std::unique_ptr<runtime::TypeSpec>(type),
            runtime::String($2),
            std::move(*modifiers),
            false
        );
        free($2);
        delete modifiers;
    }
    | FEATURE field_name COLON type_spec SEMICOLON
    {
        // Default modifier: [1] (mandatory single value)
        auto* type = static_cast<runtime::TypeSpec*>($4);
        auto modifiers = runtime::Array<std::unique_ptr<runtime::Modifier>>();
        modifiers.AddValue(std::make_unique<runtime::CardinalityModifier>(1, 1));

        $$ = new runtime::Field(
            std::unique_ptr<runtime::TypeSpec>(type),
            runtime::String($2),
            std::move(modifiers),
            false
        );
        free($2);
    }
    | FEATURE field_name COLON type_spec modifier_spec EQUALS expression SEMICOLON
    {
        // Computed feature with modifiers
        auto* type = static_cast<runtime::TypeSpec*>($4);
        auto* modifiers = static_cast<runtime::Array<std::unique_ptr<runtime::Modifier>>*>($5);
        auto* expr = static_cast<runtime::Expression*>($7);
        $$ = new runtime::Field(
            std::unique_ptr<runtime::TypeSpec>(type),
            runtime::String($2),
            std::move(*modifiers),
            false,
            std::unique_ptr<runtime::Expression>(expr)
        );
        free($2);
        delete modifiers;
    }
    | FEATURE field_name COLON type_spec EQUALS expression SEMICOLON
    {
        // Computed feature with default modifier [1]
        auto* type = static_cast<runtime::TypeSpec*>($4);
        auto* expr = static_cast<runtime::Expression*>($6);
        auto modifiers = runtime::Array<std::unique_ptr<runtime::Modifier>>();
        modifiers.AddValue(std::make_unique<runtime::CardinalityModifier>(1, 1));

        $$ = new runtime::Field(
            std::unique_ptr<runtime::TypeSpec>(type),
            runtime::String($2),
            std::move(modifiers),
            false,
            std::unique_ptr<runtime::Expression>(expr)
        );
        free($2);
    }
    | ALIAS field_name EQUALS field_name SEMICOLON
    {
        // Alias field: alias startTime = timestamp;
        // We mark this by using a FieldReference expression as initializer and setting isAlias flag
        auto* targetExpr = new runtime::FieldReference(runtime::String($4));
        auto modifiers = runtime::Array<std::unique_ptr<runtime::Modifier>>();
        modifiers.AddValue(std::make_unique<runtime::CardinalityModifier>(1, 1));

        $$ = new runtime::Field(
            nullptr, // Type will be inferred from target field
            $2,
            std::move(modifiers),
            false,
            std::unique_ptr<runtime::Expression>(targetExpr),
            true  // isAlias = true
        );
        free($2);
        free($4);
    }
    ;

type_spec:
    STRING_TYPE     { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::STRING); }
    | INT_TYPE      { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::INT); }
    | REAL_TYPE     { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::REAL); }
    | BOOL_TYPE     { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::BOOL); }
    | TIMESTAMP_TYPE { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::TIMESTAMP); }
    | TIMESPAN_TYPE { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::TIMESPAN); }
    | DATE_TYPE     { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::DATE); }
    | GUID_TYPE     { $$ = new runtime::PrimitiveTypeSpec(runtime::PrimitiveType::GUID); }
    | IDENTIFIER    { $$ = new runtime::UserDefinedTypeSpec(runtime::String($1)); free($1); }
    ;

modifier_spec:
    LBRACKET modifier_list RBRACKET
    { $$ = $2; }
    ;

modifier_list:
    modifier
    {
        auto* list = new runtime::Array<std::unique_ptr<runtime::Modifier>>();
        list->AddValue(std::unique_ptr<runtime::Modifier>(static_cast<runtime::Modifier*>($1)));
        $$ = list;
    }
    | modifier_list COMMA modifier
    {
        auto* list = static_cast<runtime::Array<std::unique_ptr<runtime::Modifier>>*>($1);
        list->AddValue(std::unique_ptr<runtime::Modifier>(static_cast<runtime::Modifier*>($3)));
        $$ = list;
    }
    ;

modifier:
    INTEGER_LITERAL
    { $$ = new runtime::CardinalityModifier($1, $1); }
    | INTEGER_LITERAL DOTDOT INTEGER_LITERAL
    { $$ = new runtime::CardinalityModifier($1, $3); }
    | INTEGER_LITERAL DOTDOT ASTERISK
    { $$ = new runtime::CardinalityModifier($1, -1); }
    | OPTIONAL
    { $$ = new runtime::CardinalityModifier(0, 1); }  // optional is equivalent to [0..1]
    | UNIQUE
    { $$ = new runtime::UniqueModifier(); }
    ;

/* Expression grammar with operator precedence */
expression:
    expression PLUS expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::ADD,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression MINUS expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::SUB,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression ASTERISK expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::MUL,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression SLASH expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::DIV,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression PERCENT expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::MOD,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression LT expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::LT,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression GT expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::GT,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression LE expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::LE,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression GE expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::GE,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression EQ expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::EQ,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression NE expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::NE,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression AND expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::AND,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | expression OR expression
    { $$ = new runtime::BinaryExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        runtime::BinaryExpression::Op::OR,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($3))
    ); }
    | MINUS expression %prec UNARY_MINUS
    { $$ = new runtime::UnaryExpression(
        runtime::UnaryExpression::Op::NEG,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($2))
    ); }
    | NOT expression
    { $$ = new runtime::UnaryExpression(
        runtime::UnaryExpression::Op::NOT,
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($2))
    ); }
    | LPAREN expression RPAREN
    { $$ = new runtime::ParenthesizedExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($2))
    ); }
    | expression DOT IDENTIFIER
    { $$ = new runtime::MemberAccessExpression(
        std::unique_ptr<runtime::Expression>(static_cast<runtime::Expression*>($1)),
        $3
    );
      free($3);
    }
    | primary_expression
    { $$ = $1; }
    ;

primary_expression:
    INTEGER_LITERAL
    { $$ = new runtime::LiteralExpression(static_cast<int64_t>($1)); }
    | REAL_LITERAL
    {
        double val = atof($1);
        $$ = new runtime::LiteralExpression(val);
        free($1);
    }
    | STRING_LITERAL
    {
        $$ = new runtime::LiteralExpression(runtime::String($1));
        free($1);
    }
    | BOOL_LITERAL
    {
        bool val = (0 == StringCompareIgnoreCase($1, "true"));
        $$ = new runtime::LiteralExpression(val);
        free($1);
    }
    | attribute_name
    {
        $$ = new runtime::FieldReference(runtime::String($1));
        free($1);
    }
    ;

%%

/* Ensure C linkage when compiled as C++ */
#ifdef __cplusplus
extern "C" {
#endif

extern "C" void yyerror(const char *s) {
    std::ostringstream errorMsg;
    errorMsg << g_current_filename << ":" << yylloc.first_line
             << ":" << yylloc.first_column
             << ": error: " << s;

    Console::ReportError(errorMsg.str());

    // Show the source line if available
    if (yylloc.first_line > 0 && yylloc.first_line <= static_cast<int>(g_source_lines.GetCount())) {
        const runtime::String& line = g_source_lines.At(static_cast<size_t>(yylloc.first_line - 1));
        Console::ReportError(line);

        // Show a caret pointing to the error column
        std::ostringstream caretLine;
        for (int i = 1; i < yylloc.first_column; ++i) {
            caretLine << " ";
        }
        caretLine << "^";
        Console::ReportError(caretLine.str());
    }
}

#ifdef __cplusplus
}
#endif
