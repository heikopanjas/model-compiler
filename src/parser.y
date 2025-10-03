%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex(void);
extern int yylineno;
extern FILE *yyin;
void yyerror(const char *s);
%}

%union {
    int integer;
    char *string;
}

/* Token declarations */
%token FABRIC ENUM STATIC UNIQUE
%token STRING_TYPE INT_TYPE REAL_TYPE TIMESTAMP_TYPE TIMESPAN_TYPE DATE_TYPE GUID_TYPE
%token LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN
%token SEMICOLON COLON COMMA DOT DOTDOT ASTERISK
%token <string> IDENTIFIER
%token <integer> INTEGER_LITERAL

%type <string> type_spec

%%

program:
    /* empty */
    | declaration_list
    { printf("P3 program parsed successfully!\n"); }
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
    { printf("Enum: %s\n", $2); free($2); }
    ;

enum_value_list:
    IDENTIFIER
    { printf("  - %s\n", $1); free($1); }
    | enum_value_list COMMA IDENTIFIER
    { printf("  - %s\n", $3); free($3); }
    ;

fabric_declaration:
    FABRIC IDENTIFIER LBRACE field_list RBRACE
    { printf("Fabric: %s\n", $2); free($2); }
    | FABRIC IDENTIFIER COLON IDENTIFIER LBRACE field_list RBRACE
    { printf("Fabric: %s : %s\n", $2, $4); free($2); free($4); }
    ;

field_list:
    /* empty */
    | field_list field
    ;

field:
    field_modifiers type_spec IDENTIFIER modifier_spec SEMICOLON
    { printf("  Field: %s %s\n", $2, $3); free($2); free($3); }
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
    { printf("[%d]", $1); }
    | INTEGER_LITERAL DOTDOT INTEGER_LITERAL
    { printf("[%d..%d]", $1, $3); }
    | INTEGER_LITERAL DOTDOT ASTERISK
    { printf("[%d..*]", $1); }
    | UNIQUE
    { printf("[unique]"); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}
