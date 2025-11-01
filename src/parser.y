%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

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
%}

%code requires {
    #ifdef __cplusplus
    #define YYPARSE_DECL extern "C" int yyparse (void)
    #endif
}

%union {
    int integer;
    char *string;
}

/* Token declarations */
%token FABRIC ENUM STATIC UNIQUE
%token STRING_TYPE INT_TYPE REAL_TYPE TIMESTAMP_TYPE TIMESPAN_TYPE DATE_TYPE GUID_TYPE
%token LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN
%token SEMICOLON COLON COMMA DOT DOTDOT ASTERISK QUESTION
%token <string> IDENTIFIER
%token <integer> INTEGER_LITERAL

%type <string> type_spec

%%

program:
    /* empty */
    | declaration_list
    { std::cout << "P3 program parsed successfully!\n"; }
    ;

declaration_list:
    declaration
    | declaration_list declaration
    ;

declaration:
    enum_declaration
    | fabric_declaration
    ;

enum_declaration:
    ENUM IDENTIFIER LBRACE enum_value_list RBRACE
    { std::cout << "Enum: " << $2 << "\n"; free($2); }
    ;

enum_value_list:
    IDENTIFIER
    { std::cout << "  - " << $1 << "\n"; free($1); }
    | enum_value_list COMMA IDENTIFIER
    { std::cout << "  - " << $3 << "\n"; free($3); }
    ;

fabric_declaration:
    FABRIC IDENTIFIER LBRACE field_list RBRACE
    { std::cout << "Fabric: " << $2 << "\n"; free($2); }
    | FABRIC IDENTIFIER COLON IDENTIFIER LBRACE field_list RBRACE
    { std::cout << "Fabric: " << $2 << " : " << $4 << "\n"; free($2); free($4); }
    ;

field_list:
    /* empty */
    | field_list field
    ;

field:
    field_modifiers type_spec IDENTIFIER modifier_spec SEMICOLON
    { std::cout << "  Field: " << $2 << " " << $3 << "\n"; free($2); free($3); }
    | field_modifiers type_spec IDENTIFIER SEMICOLON
    { std::cout << "  Field: " << $2 << " " << $3 << " [1 - default]\n"; free($2); free($3); }
    | field_modifiers type_spec IDENTIFIER QUESTION SEMICOLON
    { std::cout << "  Field: " << $2 << " " << $3 << " [0..1 - optional]\n"; free($2); free($3); }
    ;

field_modifiers:
    /* empty */
    | STATIC
    ;

type_spec:
    STRING_TYPE     { $$ = strdup("String"); }
    | INT_TYPE      { $$ = strdup("Int"); }
    | REAL_TYPE     { $$ = strdup("Real"); }
    | TIMESTAMP_TYPE { $$ = strdup("Timestamp"); }
    | TIMESPAN_TYPE { $$ = strdup("Timespan"); }
    | DATE_TYPE     { $$ = strdup("Date"); }
    | GUID_TYPE     { $$ = strdup("Guid"); }
    | IDENTIFIER    { $$ = $1; }
    ;

modifier_spec:
    LBRACKET modifier_list RBRACKET
    ;

modifier_list:
    modifier
    | modifier_list COMMA modifier
    ;

modifier:
    INTEGER_LITERAL
    { std::cout << "[" << $1 << "]"; }
    | INTEGER_LITERAL DOTDOT INTEGER_LITERAL
    { std::cout << "[" << $1 << ".." << $3 << "]"; }
    | INTEGER_LITERAL DOTDOT ASTERISK
    { std::cout << "[" << $1 << "..*]"; }
    | UNIQUE
    { std::cout << "[unique]"; }
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
