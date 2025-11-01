%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include "AST.h"
#include "Console.h"

#ifdef __cplusplus
extern "C" {
#endif
extern int yylex(void);
extern int yylineno;
extern FILE *yyin;
void yyerror(const char *s);
#ifdef __cplusplus
}
#endif

// Global AST root
std::unique_ptr<bbfm::AST> g_ast;

// Global filename for error reporting
std::string g_current_filename;

// Cache of source file lines for error reporting
std::vector<std::string> g_source_lines;
%}

%locations

%code requires {
    #ifdef __cplusplus
    #define YYPARSE_DECL extern "C" int yyparse (void)
    #endif
}

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
}

/* Token declarations */
%token CLASS INHERITS ENUM FEATURE INVARIANT OPTIONAL UNIQUE
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
%type <string> literal_value
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
        auto* ast = new bbfm::AST(std::vector<std::unique_ptr<bbfm::Declaration>>());
        $$ = ast;
        g_ast = std::unique_ptr<bbfm::AST>(ast);
    }
    | declaration_list
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Declaration>>*>($1);
        auto* ast = new bbfm::AST(std::move(*list));
        delete list;
        $$ = ast;
        g_ast = std::unique_ptr<bbfm::AST>(ast);
    }
    ;

declaration_list:
    declaration
    {
        auto* list = new std::vector<std::unique_ptr<bbfm::Declaration>>();
        list->push_back(std::unique_ptr<bbfm::Declaration>(static_cast<bbfm::Declaration*>($1)));
        $$ = list;
    }
    | declaration_list declaration
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Declaration>>*>($1);
        list->push_back(std::unique_ptr<bbfm::Declaration>(static_cast<bbfm::Declaration*>($2)));
        $$ = list;
    }
    ;

declaration:
    enum_declaration
    { $$ = new bbfm::Declaration(std::unique_ptr<bbfm::EnumDeclaration>(static_cast<bbfm::EnumDeclaration*>($1))); }
    | class_declaration
    { $$ = new bbfm::Declaration(std::unique_ptr<bbfm::ClassDeclaration>(static_cast<bbfm::ClassDeclaration*>($1))); }
    ;

enum_declaration:
    ENUM IDENTIFIER LBRACE enum_value_list RBRACE
    {
        auto* values = static_cast<std::vector<std::string>*>($4);
        $$ = new bbfm::EnumDeclaration($2, std::move(*values));
        free($2);
        delete values;
    }
    ;

enum_value_list:
    IDENTIFIER
    {
        auto* list = new std::vector<std::string>();
        list->push_back($1);
        free($1);
        $$ = list;
    }
    | enum_value_list COMMA IDENTIFIER
    {
        auto* list = static_cast<std::vector<std::string>*>($1);
        list->push_back($3);
        free($3);
        $$ = list;
    }
    ;

class_declaration:
    CLASS IDENTIFIER LBRACE field_list invariant_list RBRACE
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>($4);
        auto* invariants = static_cast<std::vector<std::unique_ptr<bbfm::Invariant>>*>($5);
        $$ = new bbfm::ClassDeclaration($2, "", std::move(*fields), std::move(*invariants));
        free($2);
        delete fields;
        delete invariants;
    }
    | CLASS IDENTIFIER INHERITS IDENTIFIER LBRACE field_list invariant_list RBRACE
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>($6);
        auto* invariants = static_cast<std::vector<std::unique_ptr<bbfm::Invariant>>*>($7);
        $$ = new bbfm::ClassDeclaration($2, $4, std::move(*fields), std::move(*invariants));
        free($2);
        free($4);
        delete fields;
        delete invariants;
    }
    ;

field_list:
    /* empty */
    { $$ = new std::vector<std::unique_ptr<bbfm::Field>>(); }
    | field_list field
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>($1);
        list->push_back(std::unique_ptr<bbfm::Field>(static_cast<bbfm::Field*>($2)));
        $$ = list;
    }
    ;

invariant_list:
    /* empty */
    { $$ = new std::vector<std::unique_ptr<bbfm::Invariant>>(); }
    | invariant_list invariant
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Invariant>>*>($1);
        list->push_back(std::unique_ptr<bbfm::Invariant>(static_cast<bbfm::Invariant*>($2)));
        $$ = list;
    }
    ;

literal_value:
    INTEGER_LITERAL { $$ = strdup(std::to_string($1).c_str()); }
    | REAL_LITERAL  { $$ = $1; }
    | STRING_LITERAL { $$ = $1; }
    | BOOL_LITERAL  { $$ = $1; }
    ;

attribute_name:
    IDENTIFIER      { $$ = $1; }
    | STRING_TYPE   { $$ = strdup("string"); }
    | INT_TYPE      { $$ = strdup("int"); }
    | REAL_TYPE     { $$ = strdup("real"); }
    | BOOL_TYPE     { $$ = strdup("bool"); }
    | TIMESTAMP_TYPE { $$ = strdup("timestamp"); }
    | TIMESPAN_TYPE { $$ = strdup("timespan"); }
    | DATE_TYPE     { $$ = strdup("date"); }
    | GUID_TYPE     { $$ = strdup("guid"); }
    ;

invariant:
    INVARIANT IDENTIFIER COLON expression SEMICOLON
    {
        $$ = new bbfm::Invariant($2,
            std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($4)));
        free($2);
    }
    ;

field_name:
    IDENTIFIER      { $$ = $1; }
    | STRING_TYPE   { $$ = strdup("string"); }
    | INT_TYPE      { $$ = strdup("int"); }
    | REAL_TYPE     { $$ = strdup("real"); }
    | BOOL_TYPE     { $$ = strdup("bool"); }
    | TIMESTAMP_TYPE { $$ = strdup("timestamp"); }
    | TIMESPAN_TYPE { $$ = strdup("timespan"); }
    | DATE_TYPE     { $$ = strdup("date"); }
    | GUID_TYPE     { $$ = strdup("guid"); }
    ;

field:
    FEATURE field_name COLON type_spec modifier_spec SEMICOLON
    {
        auto* type = static_cast<bbfm::TypeSpec*>($4);
        auto* modifiers = static_cast<std::vector<std::unique_ptr<bbfm::Modifier>>*>($5);
        $$ = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            $2,
            std::move(*modifiers),
            false
        );
        free($2);
        delete modifiers;
    }
    | FEATURE field_name COLON type_spec SEMICOLON
    {
        // Default modifier: [1] (mandatory single value)
        auto* type = static_cast<bbfm::TypeSpec*>($4);
        auto modifiers = std::vector<std::unique_ptr<bbfm::Modifier>>();
        modifiers.push_back(std::make_unique<bbfm::CardinalityModifier>(1, 1));

        $$ = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            $2,
            std::move(modifiers),
            false
        );
        free($2);
    }
    | FEATURE field_name COLON type_spec modifier_spec EQUALS expression SEMICOLON
    {
        // Computed feature with modifiers
        auto* type = static_cast<bbfm::TypeSpec*>($4);
        auto* modifiers = static_cast<std::vector<std::unique_ptr<bbfm::Modifier>>*>($5);
        auto* expr = static_cast<bbfm::Expression*>($7);
        $$ = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            $2,
            std::move(*modifiers),
            false,
            std::unique_ptr<bbfm::Expression>(expr)
        );
        free($2);
        delete modifiers;
    }
    | FEATURE field_name COLON type_spec EQUALS expression SEMICOLON
    {
        // Computed feature with default modifier [1]
        auto* type = static_cast<bbfm::TypeSpec*>($4);
        auto* expr = static_cast<bbfm::Expression*>($6);
        auto modifiers = std::vector<std::unique_ptr<bbfm::Modifier>>();
        modifiers.push_back(std::make_unique<bbfm::CardinalityModifier>(1, 1));

        $$ = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            $2,
            std::move(modifiers),
            false,
            std::unique_ptr<bbfm::Expression>(expr)
        );
        free($2);
    }
    ;

type_spec:
    STRING_TYPE     { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::STRING); }
    | INT_TYPE      { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::INT); }
    | REAL_TYPE     { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::REAL); }
    | BOOL_TYPE     { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::BOOL); }
    | TIMESTAMP_TYPE { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::TIMESTAMP); }
    | TIMESPAN_TYPE { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::TIMESPAN); }
    | DATE_TYPE     { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::DATE); }
    | GUID_TYPE     { $$ = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::GUID); }
    | IDENTIFIER    { $$ = new bbfm::UserDefinedTypeSpec($1); free($1); }
    ;

modifier_spec:
    LBRACKET modifier_list RBRACKET
    { $$ = $2; }
    ;

modifier_list:
    modifier
    {
        auto* list = new std::vector<std::unique_ptr<bbfm::Modifier>>();
        list->push_back(std::unique_ptr<bbfm::Modifier>(static_cast<bbfm::Modifier*>($1)));
        $$ = list;
    }
    | modifier_list COMMA modifier
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Modifier>>*>($1);
        list->push_back(std::unique_ptr<bbfm::Modifier>(static_cast<bbfm::Modifier*>($3)));
        $$ = list;
    }
    ;

modifier:
    INTEGER_LITERAL
    { $$ = new bbfm::CardinalityModifier($1, $1); }
    | INTEGER_LITERAL DOTDOT INTEGER_LITERAL
    { $$ = new bbfm::CardinalityModifier($1, $3); }
    | INTEGER_LITERAL DOTDOT ASTERISK
    { $$ = new bbfm::CardinalityModifier($1, -1); }
    | OPTIONAL
    { $$ = new bbfm::CardinalityModifier(0, 1); }  // optional is equivalent to [0..1]
    | UNIQUE
    { $$ = new bbfm::UniqueModifier(); }
    ;

/* Expression grammar with operator precedence */
expression:
    expression PLUS expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::ADD,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression MINUS expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::SUB,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression ASTERISK expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::MUL,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression SLASH expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::DIV,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression PERCENT expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::MOD,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression LT expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::LT,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression GT expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::GT,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression LE expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::LE,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression GE expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::GE,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression EQ expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::EQ,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression NE expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::NE,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression AND expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::AND,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | expression OR expression
    { $$ = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        bbfm::BinaryExpression::Op::OR,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($3))
    ); }
    | MINUS expression %prec UNARY_MINUS
    { $$ = new bbfm::UnaryExpression(
        bbfm::UnaryExpression::Op::NEG,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($2))
    ); }
    | NOT expression
    { $$ = new bbfm::UnaryExpression(
        bbfm::UnaryExpression::Op::NOT,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($2))
    ); }
    | LPAREN expression RPAREN
    { $$ = new bbfm::ParenthesizedExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($2))
    ); }
    | expression DOT IDENTIFIER
    { $$ = new bbfm::MemberAccessExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>($1)),
        $3
    );
      free($3);
    }
    | primary_expression
    { $$ = $1; }
    ;

primary_expression:
    INTEGER_LITERAL
    { $$ = new bbfm::LiteralExpression(static_cast<int64_t>($1)); }
    | REAL_LITERAL
    {
        double val = atof($1);
        $$ = new bbfm::LiteralExpression(val);
        free($1);
    }
    | STRING_LITERAL
    {
        $$ = new bbfm::LiteralExpression(std::string($1));
        free($1);
    }
    | BOOL_LITERAL
    {
        bool val = (0 == strcasecmp($1, "true"));
        $$ = new bbfm::LiteralExpression(val);
        free($1);
    }
    | attribute_name
    {
        $$ = new bbfm::FieldReference($1);
        free($1);
    }
    ;

%%

/* Ensure C linkage when compiled as C++ */
#ifdef __cplusplus
extern "C" {
#endif

void yyerror(const char *s) {
    std::ostringstream errorMsg;
    errorMsg << g_current_filename << ":" << yylloc.first_line
             << ":" << yylloc.first_column
             << ": error: " << s;

    bbfm::Console::ReportError(errorMsg.str());

    // Show the source line if available
    if (yylloc.first_line > 0 && yylloc.first_line <= static_cast<int>(g_source_lines.size())) {
        const std::string& line = g_source_lines[yylloc.first_line - 1];
        bbfm::Console::ReportError(line);

        // Show a caret pointing to the error column
        std::ostringstream caretLine;
        for (int i = 1; i < yylloc.first_column; ++i) {
            caretLine << " ";
        }
        caretLine << "^";
        bbfm::Console::ReportError(caretLine.str());
    }
}

#ifdef __cplusplus
}
#endif
