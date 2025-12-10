/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_USERS_HEIKO_RANDD_ME_MODEL_COMPILER_BUILD_PARSER_H_INCLUDED
# define YY_YY_USERS_HEIKO_RANDD_ME_MODEL_COMPILER_BUILD_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 36 "/Users/heiko/RandD/_me/model-compiler/src/model-compiler.y"

    #ifdef __cplusplus
    #define YYPARSE_DECL extern "C" int yyparse (void)
    #endif

#line 55 "/Users/heiko/RandD/_me/model-compiler/_build/parser.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    CLASS = 258,                   /* CLASS  */
    INHERITS = 259,                /* INHERITS  */
    ENUM = 260,                    /* ENUM  */
    FEATURE = 261,                 /* FEATURE  */
    INVARIANT = 262,               /* INVARIANT  */
    OPTIONAL = 263,                /* OPTIONAL  */
    UNIQUE = 264,                  /* UNIQUE  */
    NAMESPACE = 265,               /* NAMESPACE  */
    STRING_TYPE = 266,             /* STRING_TYPE  */
    INT_TYPE = 267,                /* INT_TYPE  */
    REAL_TYPE = 268,               /* REAL_TYPE  */
    BOOL_TYPE = 269,               /* BOOL_TYPE  */
    TIMESTAMP_TYPE = 270,          /* TIMESTAMP_TYPE  */
    TIMESPAN_TYPE = 271,           /* TIMESPAN_TYPE  */
    DATE_TYPE = 272,               /* DATE_TYPE  */
    GUID_TYPE = 273,               /* GUID_TYPE  */
    LBRACE = 274,                  /* LBRACE  */
    RBRACE = 275,                  /* RBRACE  */
    LBRACKET = 276,                /* LBRACKET  */
    RBRACKET = 277,                /* RBRACKET  */
    LPAREN = 278,                  /* LPAREN  */
    RPAREN = 279,                  /* RPAREN  */
    SEMICOLON = 280,               /* SEMICOLON  */
    COLON = 281,                   /* COLON  */
    COMMA = 282,                   /* COMMA  */
    EQUALS = 283,                  /* EQUALS  */
    DOT = 284,                     /* DOT  */
    DOTDOT = 285,                  /* DOTDOT  */
    ASTERISK = 286,                /* ASTERISK  */
    PLUS = 287,                    /* PLUS  */
    MINUS = 288,                   /* MINUS  */
    SLASH = 289,                   /* SLASH  */
    PERCENT = 290,                 /* PERCENT  */
    LE = 291,                      /* LE  */
    GE = 292,                      /* GE  */
    EQ = 293,                      /* EQ  */
    NE = 294,                      /* NE  */
    LT = 295,                      /* LT  */
    GT = 296,                      /* GT  */
    AND = 297,                     /* AND  */
    OR = 298,                      /* OR  */
    NOT = 299,                     /* NOT  */
    IDENTIFIER = 300,              /* IDENTIFIER  */
    INTEGER_LITERAL = 301,         /* INTEGER_LITERAL  */
    REAL_LITERAL = 302,            /* REAL_LITERAL  */
    STRING_LITERAL = 303,          /* STRING_LITERAL  */
    BOOL_LITERAL = 304,            /* BOOL_LITERAL  */
    UNARY_MINUS = 305              /* UNARY_MINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 48 "/Users/heiko/RandD/_me/model-compiler/src/model-compiler.y"

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

#line 142 "/Users/heiko/RandD/_me/model-compiler/_build/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_USERS_HEIKO_RANDD_ME_MODEL_COMPILER_BUILD_PARSER_H_INCLUDED  */
