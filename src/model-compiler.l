%{
#ifdef __cplusplus
extern "C" {
#endif
#include "parser.h"
#ifdef __cplusplus
}
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <iostream>

/* Define yylex with C linkage */
#ifdef __cplusplus
#define YY_DECL extern "C" int yylex (void)
#endif
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

"class"         { return CLASS; }
"inherits"      { return INHERITS; }
"enum"          { return ENUM; }
"feature"       { return FEATURE; }
"invariant"     { return INVARIANT; }
"optional"      { return OPTIONAL; }
"unique"        { return UNIQUE; }

{ID}            {
    // Check for type keywords (case-insensitive)
    if (strcasecmp(yytext, "String") == 0) return STRING_TYPE;
    if (strcasecmp(yytext, "Int") == 0) return INT_TYPE;
    if (strcasecmp(yytext, "Real") == 0) return REAL_TYPE;
    if (strcasecmp(yytext, "Bool") == 0) return BOOL_TYPE;
    if (strcasecmp(yytext, "Timestamp") == 0) return TIMESTAMP_TYPE;
    if (strcasecmp(yytext, "Timespan") == 0) return TIMESPAN_TYPE;
    if (strcasecmp(yytext, "Date") == 0) return DATE_TYPE;
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
"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE; }
"<"             { return LT; }
">"             { return GT; }

"//".* { /* single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* multi-line comment */ }

{WHITESPACE}    { /* ignore whitespace */ }
\n              { /* ignore newlines */ }

.               { std::cerr << "Unknown character: " << yytext << "\n"; }

%%
