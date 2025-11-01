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
std::unique_ptr<bbfm::AST> g_ast;
%}

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
%token CLASS INHERITS ENUM OPTIONAL UNIQUE
%token STRING_TYPE INT_TYPE REAL_TYPE BOOL_TYPE TIMESTAMP_TYPE TIMESPAN_TYPE DATE_TYPE GUID_TYPE
%token LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN
%token SEMICOLON COLON COMMA DOT DOTDOT ASTERISK
%token <string> IDENTIFIER
%token <integer> INTEGER_LITERAL

%type <ast> program
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
    | fabric_declaration
    { $$ = new bbfm::Declaration(std::unique_ptr<bbfm::FabricDeclaration>(static_cast<bbfm::FabricDeclaration*>($1))); }
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

fabric_declaration:
    CLASS IDENTIFIER LBRACE field_list RBRACE
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>($4);
        $$ = new bbfm::FabricDeclaration($2, "", std::move(*fields));
        free($2);
        delete fields;
    }
    | CLASS IDENTIFIER INHERITS IDENTIFIER LBRACE field_list RBRACE
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>($6);
        $$ = new bbfm::FabricDeclaration($2, $4, std::move(*fields));
        free($2);
        free($4);
        delete fields;
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

field:
    field_modifiers type_spec IDENTIFIER modifier_spec SEMICOLON
    {
        auto* type = static_cast<bbfm::TypeSpec*>($2);
        auto* modifiers = static_cast<std::vector<std::unique_ptr<bbfm::Modifier>>*>($4);
        $$ = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
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
        auto* type = static_cast<bbfm::TypeSpec*>($2);
        auto modifiers = std::vector<std::unique_ptr<bbfm::Modifier>>();
        modifiers.push_back(std::make_unique<bbfm::CardinalityModifier>(1, 1));

        $$ = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
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
