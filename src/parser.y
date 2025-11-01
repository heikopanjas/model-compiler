%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>
#include "AST.h"

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
std::unique_ptr<p3::Program> g_ast;
%}

%code requires {
    #ifdef __cplusplus
    #define YYPARSE_DECL extern "C" int yyparse (void)
    #endif
}

%union {
    int integer;
    char *string;
    void *program;
    void *declaration;
    void *enumDecl;
    void *fabricDecl;
    void *field;
    void *typeSpec;
    void *modifier;
    void *declarationList;
    void *stringList;
    void *fieldList;
    void *modifierList;
}

/* Token declarations */
%token FABRIC ENUM STATIC UNIQUE
%token STRING_TYPE INT_TYPE REAL_TYPE TIMESTAMP_TYPE TIMESPAN_TYPE DATE_TYPE GUID_TYPE
%token LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN
%token SEMICOLON COLON COMMA DOT DOTDOT ASTERISK QUESTION
%token <string> IDENTIFIER
%token <integer> INTEGER_LITERAL

%type <program> program
%type <declarationList> declaration_list
%type <declaration> declaration
%type <enumDecl> enum_declaration
%type <fabricDecl> fabric_declaration
%type <stringList> enum_value_list
%type <fieldList> field_list
%type <field> field
%type <typeSpec> type_spec
%type <modifierList> modifier_spec modifier_list
%type <modifier> modifier
%type <integer> field_modifiers

%%

program:
    /* empty */
    {
        auto* prog = new p3::Program(std::vector<std::unique_ptr<p3::Declaration>>());
        $$ = prog;
        g_ast = std::unique_ptr<p3::Program>(prog);
    }
    | declaration_list
    {
        auto* list = static_cast<std::vector<std::unique_ptr<p3::Declaration>>*>($1);
        auto* prog = new p3::Program(std::move(*list));
        delete list;
        $$ = prog;
        g_ast = std::unique_ptr<p3::Program>(prog);
    }
    ;

declaration_list:
    declaration
    {
        auto* list = new std::vector<std::unique_ptr<p3::Declaration>>();
        list->push_back(std::unique_ptr<p3::Declaration>(static_cast<p3::Declaration*>($1)));
        $$ = list;
    }
    | declaration_list declaration
    {
        auto* list = static_cast<std::vector<std::unique_ptr<p3::Declaration>>*>($1);
        list->push_back(std::unique_ptr<p3::Declaration>(static_cast<p3::Declaration*>($2)));
        $$ = list;
    }
    ;

declaration:
    enum_declaration
    { $$ = new p3::Declaration(std::unique_ptr<p3::EnumDeclaration>(static_cast<p3::EnumDeclaration*>($1))); }
    | fabric_declaration
    { $$ = new p3::Declaration(std::unique_ptr<p3::FabricDeclaration>(static_cast<p3::FabricDeclaration*>($1))); }
    ;

enum_declaration:
    ENUM IDENTIFIER LBRACE enum_value_list RBRACE
    {
        auto* values = static_cast<std::vector<std::string>*>($4);
        $$ = new p3::EnumDeclaration($2, std::move(*values));
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

fabric_declaration:
    FABRIC IDENTIFIER LBRACE field_list RBRACE
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<p3::Field>>*>($4);
        $$ = new p3::FabricDeclaration($2, "", std::move(*fields));
        free($2);
        delete fields;
    }
    | FABRIC IDENTIFIER COLON IDENTIFIER LBRACE field_list RBRACE
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<p3::Field>>*>($6);
        $$ = new p3::FabricDeclaration($2, $4, std::move(*fields));
        free($2);
        free($4);
        delete fields;
    }
    ;

field_list:
    /* empty */
    { $$ = new std::vector<std::unique_ptr<p3::Field>>(); }
    | field_list field
    {
        auto* list = static_cast<std::vector<std::unique_ptr<p3::Field>>*>($1);
        list->push_back(std::unique_ptr<p3::Field>(static_cast<p3::Field*>($2)));
        $$ = list;
    }
    ;

field:
    field_modifiers type_spec IDENTIFIER modifier_spec SEMICOLON
    {
        auto* type = static_cast<p3::TypeSpec*>($2);
        auto* modifiers = static_cast<std::vector<std::unique_ptr<p3::Modifier>>*>($4);
        $$ = new p3::Field(
            std::unique_ptr<p3::TypeSpec>(type),
            $3,
            std::move(*modifiers),
            $1 != 0
        );
        free($3);
        delete modifiers;
    }
    | field_modifiers type_spec IDENTIFIER SEMICOLON
    {
        // Default modifier: [1] (mandatory single value)
        auto* type = static_cast<p3::TypeSpec*>($2);
        auto modifiers = std::vector<std::unique_ptr<p3::Modifier>>();
        modifiers.push_back(std::make_unique<p3::CardinalityModifier>(1, 1));

        $$ = new p3::Field(
            std::unique_ptr<p3::TypeSpec>(type),
            $3,
            std::move(modifiers),
            $1 != 0
        );
        free($3);
    }
    | field_modifiers type_spec IDENTIFIER QUESTION SEMICOLON
    {
        // Optional shorthand: [0..1]
        auto* type = static_cast<p3::TypeSpec*>($2);
        auto modifiers = std::vector<std::unique_ptr<p3::Modifier>>();
        modifiers.push_back(std::make_unique<p3::CardinalityModifier>(0, 1));

        $$ = new p3::Field(
            std::unique_ptr<p3::TypeSpec>(type),
            $3,
            std::move(modifiers),
            $1 != 0
        );
        free($3);
    }
    ;

field_modifiers:
    /* empty */
    { $$ = 0; }
    | STATIC
    { $$ = 1; }
    ;

type_spec:
    STRING_TYPE     { $$ = new p3::PrimitiveTypeSpec(p3::PrimitiveType::STRING); }
    | INT_TYPE      { $$ = new p3::PrimitiveTypeSpec(p3::PrimitiveType::INT); }
    | REAL_TYPE     { $$ = new p3::PrimitiveTypeSpec(p3::PrimitiveType::REAL); }
    | TIMESTAMP_TYPE { $$ = new p3::PrimitiveTypeSpec(p3::PrimitiveType::TIMESTAMP); }
    | TIMESPAN_TYPE { $$ = new p3::PrimitiveTypeSpec(p3::PrimitiveType::TIMESPAN); }
    | DATE_TYPE     { $$ = new p3::PrimitiveTypeSpec(p3::PrimitiveType::DATE); }
    | GUID_TYPE     { $$ = new p3::PrimitiveTypeSpec(p3::PrimitiveType::GUID); }
    | IDENTIFIER    { $$ = new p3::UserDefinedTypeSpec($1); free($1); }
    ;

modifier_spec:
    LBRACKET modifier_list RBRACKET
    { $$ = $2; }
    ;

modifier_list:
    modifier
    {
        auto* list = new std::vector<std::unique_ptr<p3::Modifier>>();
        list->push_back(std::unique_ptr<p3::Modifier>(static_cast<p3::Modifier*>($1)));
        $$ = list;
    }
    | modifier_list COMMA modifier
    {
        auto* list = static_cast<std::vector<std::unique_ptr<p3::Modifier>>*>($1);
        list->push_back(std::unique_ptr<p3::Modifier>(static_cast<p3::Modifier*>($3)));
        $$ = list;
    }
    ;

modifier:
    INTEGER_LITERAL
    { $$ = new p3::CardinalityModifier($1, $1); }
    | INTEGER_LITERAL DOTDOT INTEGER_LITERAL
    { $$ = new p3::CardinalityModifier($1, $3); }
    | INTEGER_LITERAL DOTDOT ASTERISK
    { $$ = new p3::CardinalityModifier($1, -1); }
    | UNIQUE
    { $$ = new p3::UniqueModifier(); }
    ;

%%

/* Ensure C linkage when compiled as C++ */
#ifdef __cplusplus
extern "C" {
#endif

void yyerror(const char *s) {
    std::cerr << "Error at line " << yylineno << ": " << s << "\n";
}

#ifdef __cplusplus
}
#endif
