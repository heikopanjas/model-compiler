%{
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
%}

%option noyywrap
%option yylineno
%option case-insensitive
%option nounput
%option noinput

DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
INTEGER     {DIGIT}+
WHITESPACE  [ \t\r]+

%%

"fabric"        { return FABRIC; }
"enum"          { return ENUM; }
"static"        { return STATIC; }
"unique"        { return UNIQUE; }

{ID}            {
    // Check for type keywords (case-insensitive)
    if (strcasecmp(yytext, "String") == 0) return STRING_TYPE;
    if (strcasecmp(yytext, "Int") == 0) return INT_TYPE;
    if (strcasecmp(yytext, "Real") == 0) return REAL_TYPE;
    if (strcasecmp(yytext, "Timestamp") == 0) return TIMESTAMP_TYPE;
    if (strcasecmp(yytext, "Timespan") == 0) return TIMESPAN_TYPE;
    if (strcasecmp(yytext, "Date") == 0) return DATE_TYPE;
    if (strcasecmp(yytext, "UUID") == 0) return UUID_TYPE;
    if (strcasecmp(yytext, "Guid") == 0) return GUID_TYPE;
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}
{INTEGER}       { yylval.integer = atoi(yytext); return INTEGER_LITERAL; }

"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
"("             { return LPAREN; }
")"             { return RPAREN; }
";"             { return SEMICOLON; }
":"             { return COLON; }
","             { return COMMA; }
"."             { return DOT; }
".."            { return DOTDOT; }
"*"             { return ASTERISK; }

"//".* { /* single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* multi-line comment */ }

{WHITESPACE}    { /* ignore whitespace */ }
\n              { /* ignore newlines */ }

.               { fprintf(stderr, "Unknown character: %s\n", yytext); }

%%
