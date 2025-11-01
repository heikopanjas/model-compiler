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
%option nounput
%option noinput

DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
INTEGER     {DIGIT}+
REAL        {DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?
WHITESPACE  [ \t\r]+

%%

"class"         { return CLASS; }
"inherits"      { return INHERITS; }
"enum"          { return ENUM; }
"feature"       { return FEATURE; }
"invariant"     { return INVARIANT; }
"optional"      { return OPTIONAL; }
"unique"        { return UNIQUE; }
"String"        { return STRING_TYPE; }
"Int"           { return INT_TYPE; }
"Real"          { return REAL_TYPE; }
"Bool"          { return BOOL_TYPE; }
"Timestamp"     { return TIMESTAMP_TYPE; }
"Timespan"      { return TIMESPAN_TYPE; }
"Date"          { return DATE_TYPE; }
"Guid"          { return GUID_TYPE; }
"true"          { yylval.string = strdup("true"); return BOOL_LITERAL; }
"false"         { yylval.string = strdup("false"); return BOOL_LITERAL; }

{ID}            {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}
{REAL}          { yylval.string = strdup(yytext); return REAL_LITERAL; }
{INTEGER}       { yylval.integer = atoi(yytext); return INTEGER_LITERAL; }
\"([^\"\\]|\\.)*\"  {
    // String literal with escape sequences
    yylval.string = strdup(yytext);
    return STRING_LITERAL;
}

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
