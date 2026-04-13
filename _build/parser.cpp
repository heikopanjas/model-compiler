/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "Z:/_me/model-compiler/src/model-compiler.y"

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
std::unique_ptr<bbfm::AST> g_ast;

// Global filename for error reporting
std::string g_current_filename;

// Cache of source file lines for error reporting
std::vector<std::string> g_source_lines;

#line 118 "Z:/_me/model-compiler/_build/parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_CLASS = 3,                      /* CLASS  */
  YYSYMBOL_INHERITS = 4,                   /* INHERITS  */
  YYSYMBOL_ENUM = 5,                       /* ENUM  */
  YYSYMBOL_FEATURE = 6,                    /* FEATURE  */
  YYSYMBOL_ALIAS = 7,                      /* ALIAS  */
  YYSYMBOL_INVARIANT = 8,                  /* INVARIANT  */
  YYSYMBOL_OPTIONAL = 9,                   /* OPTIONAL  */
  YYSYMBOL_UNIQUE = 10,                    /* UNIQUE  */
  YYSYMBOL_NAMESPACE = 11,                 /* NAMESPACE  */
  YYSYMBOL_STRING_TYPE = 12,               /* STRING_TYPE  */
  YYSYMBOL_INT_TYPE = 13,                  /* INT_TYPE  */
  YYSYMBOL_REAL_TYPE = 14,                 /* REAL_TYPE  */
  YYSYMBOL_BOOL_TYPE = 15,                 /* BOOL_TYPE  */
  YYSYMBOL_TIMESTAMP_TYPE = 16,            /* TIMESTAMP_TYPE  */
  YYSYMBOL_TIMESPAN_TYPE = 17,             /* TIMESPAN_TYPE  */
  YYSYMBOL_DATE_TYPE = 18,                 /* DATE_TYPE  */
  YYSYMBOL_GUID_TYPE = 19,                 /* GUID_TYPE  */
  YYSYMBOL_LBRACE = 20,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 21,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 22,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 23,                  /* RBRACKET  */
  YYSYMBOL_LPAREN = 24,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 25,                    /* RPAREN  */
  YYSYMBOL_SEMICOLON = 26,                 /* SEMICOLON  */
  YYSYMBOL_COLON = 27,                     /* COLON  */
  YYSYMBOL_COMMA = 28,                     /* COMMA  */
  YYSYMBOL_EQUALS = 29,                    /* EQUALS  */
  YYSYMBOL_DOT = 30,                       /* DOT  */
  YYSYMBOL_DOTDOT = 31,                    /* DOTDOT  */
  YYSYMBOL_ASTERISK = 32,                  /* ASTERISK  */
  YYSYMBOL_PLUS = 33,                      /* PLUS  */
  YYSYMBOL_MINUS = 34,                     /* MINUS  */
  YYSYMBOL_SLASH = 35,                     /* SLASH  */
  YYSYMBOL_PERCENT = 36,                   /* PERCENT  */
  YYSYMBOL_LE = 37,                        /* LE  */
  YYSYMBOL_GE = 38,                        /* GE  */
  YYSYMBOL_EQ = 39,                        /* EQ  */
  YYSYMBOL_NE = 40,                        /* NE  */
  YYSYMBOL_LT = 41,                        /* LT  */
  YYSYMBOL_GT = 42,                        /* GT  */
  YYSYMBOL_AND = 43,                       /* AND  */
  YYSYMBOL_OR = 44,                        /* OR  */
  YYSYMBOL_NOT = 45,                       /* NOT  */
  YYSYMBOL_IDENTIFIER = 46,                /* IDENTIFIER  */
  YYSYMBOL_INTEGER_LITERAL = 47,           /* INTEGER_LITERAL  */
  YYSYMBOL_REAL_LITERAL = 48,              /* REAL_LITERAL  */
  YYSYMBOL_STRING_LITERAL = 49,            /* STRING_LITERAL  */
  YYSYMBOL_BOOL_LITERAL = 50,              /* BOOL_LITERAL  */
  YYSYMBOL_UNARY_MINUS = 51,               /* UNARY_MINUS  */
  YYSYMBOL_YYACCEPT = 52,                  /* $accept  */
  YYSYMBOL_program = 53,                   /* program  */
  YYSYMBOL_namespace_declaration = 54,     /* namespace_declaration  */
  YYSYMBOL_declaration_list = 55,          /* declaration_list  */
  YYSYMBOL_declaration = 56,               /* declaration  */
  YYSYMBOL_enum_declaration = 57,          /* enum_declaration  */
  YYSYMBOL_enum_value_list = 58,           /* enum_value_list  */
  YYSYMBOL_class_declaration = 59,         /* class_declaration  */
  YYSYMBOL_field_list = 60,                /* field_list  */
  YYSYMBOL_invariant_list = 61,            /* invariant_list  */
  YYSYMBOL_attribute_name = 62,            /* attribute_name  */
  YYSYMBOL_invariant = 63,                 /* invariant  */
  YYSYMBOL_field_name = 64,                /* field_name  */
  YYSYMBOL_field = 65,                     /* field  */
  YYSYMBOL_type_spec = 66,                 /* type_spec  */
  YYSYMBOL_modifier_spec = 67,             /* modifier_spec  */
  YYSYMBOL_modifier_list = 68,             /* modifier_list  */
  YYSYMBOL_modifier = 69,                  /* modifier  */
  YYSYMBOL_expression = 70,                /* expression  */
  YYSYMBOL_primary_expression = 71         /* primary_expression  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   215

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  82
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  138

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   306


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   133,   133,   138,   148,   159,   167,   173,   182,   184,
     189,   199,   206,   216,   225,   239,   240,   250,   251,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   272,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   293,   306,
     321,   337,   354,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   388,   393,   399,   408,   410,   412,   414,   416,
     422,   428,   434,   440,   446,   452,   458,   464,   470,   476,
     482,   488,   494,   500,   505,   510,   514,   521,   526,   528,
     534,   539,   545
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "CLASS", "INHERITS",
  "ENUM", "FEATURE", "ALIAS", "INVARIANT", "OPTIONAL", "UNIQUE",
  "NAMESPACE", "STRING_TYPE", "INT_TYPE", "REAL_TYPE", "BOOL_TYPE",
  "TIMESTAMP_TYPE", "TIMESPAN_TYPE", "DATE_TYPE", "GUID_TYPE", "LBRACE",
  "RBRACE", "LBRACKET", "RBRACKET", "LPAREN", "RPAREN", "SEMICOLON",
  "COLON", "COMMA", "EQUALS", "DOT", "DOTDOT", "ASTERISK", "PLUS", "MINUS",
  "SLASH", "PERCENT", "LE", "GE", "EQ", "NE", "LT", "GT", "AND", "OR",
  "NOT", "IDENTIFIER", "INTEGER_LITERAL", "REAL_LITERAL", "STRING_LITERAL",
  "BOOL_LITERAL", "UNARY_MINUS", "$accept", "program",
  "namespace_declaration", "declaration_list", "declaration",
  "enum_declaration", "enum_value_list", "class_declaration", "field_list",
  "invariant_list", "attribute_name", "invariant", "field_name", "field",
  "type_spec", "modifier_spec", "modifier_list", "modifier", "expression",
  "primary_expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-67)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       3,   -39,   -31,   -19,    34,    27,    27,   -67,   -67,   -67,
       0,    39,    43,   -67,    27,   -67,    33,   -67,    42,   -67,
      60,    51,   -67,     5,   -67,    49,    49,    -5,   -67,   -67,
      54,    51,   -67,   -67,   -67,   -67,   -67,   -67,   -67,   -67,
     -67,    76,    91,    75,   -67,   -67,   -67,     1,    59,    49,
      95,   -67,   -67,   -67,   -67,   -67,   -67,   -67,   -67,   -67,
     -67,    -1,    98,    36,     9,   -67,    36,    68,   -67,   -67,
     -67,   -67,   -67,   -67,   -67,   -67,   -67,    36,    36,    36,
     -67,   -67,   -67,   -67,   -67,   -67,    93,   -67,   -67,   -67,
     108,   -18,   -67,   112,   -67,    36,    74,   110,   110,   -67,
      97,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    36,    36,    36,   -30,   -67,     9,   -67,   131,   -67,
     -67,   110,    66,    66,   110,   110,    57,    57,   173,   173,
      57,    57,   160,   146,   -67,   -67,   -67,   -67
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     0,     0,     0,     0,     4,     6,     8,     9,
       0,     0,     0,     1,     3,     7,     0,    15,     0,     5,
       0,    17,    11,     0,    15,     0,     0,     0,    16,    10,
       0,    17,    30,    31,    32,    33,    34,    35,    36,    37,
      29,     0,     0,     0,    13,    18,    12,     0,     0,     0,
       0,    14,    43,    44,    45,    46,    47,    48,    49,    50,
      51,     0,     0,     0,     0,    39,     0,     0,    42,    20,
      21,    22,    23,    24,    25,    26,    27,     0,     0,     0,
      19,    78,    79,    80,    81,    82,     0,    77,    58,    59,
      55,     0,    53,     0,    38,     0,     0,    73,    74,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    52,     0,    41,     0,    75,
      76,    62,    60,    61,    63,    64,    67,    68,    69,    70,
      65,    66,    71,    72,    57,    56,    54,    40
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -67,   -67,   -67,   136,    17,   -67,   -67,   -67,   134,   128,
     -67,   -67,   -25,   -67,   -67,   -67,   -67,    44,   -66,   -67
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,     7,     8,    23,     9,    21,    27,
      85,    45,    41,    28,    61,    67,    91,    92,    86,    87
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      93,    42,   134,    43,    16,   115,     1,    10,     2,    43,
     116,    96,    97,    98,     3,    11,    44,   135,    88,    89,
      17,    64,    51,    15,    62,    65,    29,    12,    66,   118,
       1,    15,     2,    30,    13,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    69,    70,
      71,    72,    73,    74,    75,    76,    90,    25,    26,    18,
      77,    32,    33,    34,    35,    36,    37,    38,    39,    19,
      78,    52,    53,    54,    55,    56,    57,    58,    59,    20,
      24,    79,    80,    81,    82,    83,    84,   100,    22,   101,
     102,   103,   104,   105,    94,    40,   100,    95,   101,   119,
      46,   104,   105,    48,   100,    60,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,    99,
      49,    50,    63,   100,    68,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   117,   114,
     100,    14,   100,   120,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   137,    31,    47,
     136,   100,     0,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   100,     0,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     100,     0,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   100,     0,   101,   102,   103,   104,   105,
     106,   107,     0,     0,   110,   111
};

static const yytype_int8 yycheck[] =
{
      66,    26,    32,     8,     4,    23,     3,    46,     5,     8,
      28,    77,    78,    79,    11,    46,    21,    47,     9,    10,
      20,    22,    21,     6,    49,    26,    21,    46,    29,    95,
       3,    14,     5,    28,     0,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,    12,    13,
      14,    15,    16,    17,    18,    19,    47,     6,     7,    20,
      24,    12,    13,    14,    15,    16,    17,    18,    19,    26,
      34,    12,    13,    14,    15,    16,    17,    18,    19,    46,
      20,    45,    46,    47,    48,    49,    50,    30,    46,    32,
      33,    34,    35,    36,    26,    46,    30,    29,    32,    25,
      46,    35,    36,    27,    30,    46,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    26,
      29,    46,    27,    30,    26,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    26,    31,
      30,     5,    30,    46,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    26,    24,    31,
     116,    30,    -1,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    30,    -1,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      30,    -1,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    30,    -1,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    41,    42
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,    11,    53,    54,    55,    56,    57,    59,
      46,    46,    46,     0,    55,    56,     4,    20,    20,    26,
      46,    60,    46,    58,    20,     6,     7,    61,    65,    21,
      28,    60,    12,    13,    14,    15,    16,    17,    18,    19,
      46,    64,    64,     8,    21,    63,    46,    61,    27,    29,
      46,    21,    12,    13,    14,    15,    16,    17,    18,    19,
      46,    66,    64,    27,    22,    26,    29,    67,    26,    12,
      13,    14,    15,    16,    17,    18,    19,    24,    34,    45,
      46,    47,    48,    49,    50,    62,    70,    71,     9,    10,
      47,    68,    69,    70,    26,    29,    70,    70,    70,    26,
      30,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    31,    23,    28,    26,    70,    25,
      46,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    32,    47,    69,    26
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    52,    53,    53,    53,    54,    55,    55,    56,    56,
      57,    58,    58,    59,    59,    60,    60,    61,    61,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    63,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    65,    65,
      65,    65,    65,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    67,    68,    68,    69,    69,    69,    69,    69,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    70,    71,    71,
      71,    71,    71
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     3,     1,     2,     1,     1,
       5,     1,     3,     6,     8,     0,     2,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     5,
       8,     7,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     3,     3,     1,     1,     1,
       1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: %empty  */
#line 133 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* ast = new bbfm::AST("", std::vector<std::unique_ptr<bbfm::Declaration>>());
        (yyval.ast) = ast;
        g_ast = std::unique_ptr<bbfm::AST>(ast);
    }
#line 1406 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 3: /* program: namespace_declaration declaration_list  */
#line 139 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* ns = static_cast<std::string*>((yyvsp[-1].namespaceDecl));
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Declaration>>*>((yyvsp[0].declarationList));
        auto* ast = new bbfm::AST(*ns, std::move(*list));
        delete ns;
        delete list;
        (yyval.ast) = ast;
        g_ast = std::unique_ptr<bbfm::AST>(ast);
    }
#line 1420 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 4: /* program: declaration_list  */
#line 149 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Declaration>>*>((yyvsp[0].declarationList));
        auto* ast = new bbfm::AST("", std::move(*list));
        delete list;
        (yyval.ast) = ast;
        g_ast = std::unique_ptr<bbfm::AST>(ast);
    }
#line 1432 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 5: /* namespace_declaration: NAMESPACE IDENTIFIER SEMICOLON  */
#line 160 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        (yyval.namespaceDecl) = new std::string((yyvsp[-1].string));
        free((yyvsp[-1].string));
    }
#line 1441 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 6: /* declaration_list: declaration  */
#line 168 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = new std::vector<std::unique_ptr<bbfm::Declaration>>();
        list->push_back(std::unique_ptr<bbfm::Declaration>(static_cast<bbfm::Declaration*>((yyvsp[0].declaration))));
        (yyval.declarationList) = list;
    }
#line 1451 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 7: /* declaration_list: declaration_list declaration  */
#line 174 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Declaration>>*>((yyvsp[-1].declarationList));
        list->push_back(std::unique_ptr<bbfm::Declaration>(static_cast<bbfm::Declaration*>((yyvsp[0].declaration))));
        (yyval.declarationList) = list;
    }
#line 1461 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 8: /* declaration: enum_declaration  */
#line 183 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.declaration) = new bbfm::Declaration(std::unique_ptr<bbfm::EnumDeclaration>(static_cast<bbfm::EnumDeclaration*>((yyvsp[0].enumDecl)))); }
#line 1467 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 9: /* declaration: class_declaration  */
#line 185 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.declaration) = new bbfm::Declaration(std::unique_ptr<bbfm::ClassDeclaration>(static_cast<bbfm::ClassDeclaration*>((yyvsp[0].classDecl)))); }
#line 1473 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 10: /* enum_declaration: ENUM IDENTIFIER LBRACE enum_value_list RBRACE  */
#line 190 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* values = static_cast<std::vector<std::string>*>((yyvsp[-1].stringList));
        (yyval.enumDecl) = new bbfm::EnumDeclaration((yyvsp[-3].string), std::move(*values));
        free((yyvsp[-3].string));
        delete values;
    }
#line 1484 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 11: /* enum_value_list: IDENTIFIER  */
#line 200 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = new std::vector<std::string>();
        list->push_back((yyvsp[0].string));
        free((yyvsp[0].string));
        (yyval.stringList) = list;
    }
#line 1495 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 12: /* enum_value_list: enum_value_list COMMA IDENTIFIER  */
#line 207 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = static_cast<std::vector<std::string>*>((yyvsp[-2].stringList));
        list->push_back((yyvsp[0].string));
        free((yyvsp[0].string));
        (yyval.stringList) = list;
    }
#line 1506 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 13: /* class_declaration: CLASS IDENTIFIER LBRACE field_list invariant_list RBRACE  */
#line 217 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>((yyvsp[-2].fieldList));
        auto* invariants = static_cast<std::vector<std::unique_ptr<bbfm::Invariant>>*>((yyvsp[-1].invariantList));
        (yyval.classDecl) = new bbfm::ClassDeclaration((yyvsp[-4].string), "", std::move(*fields), std::move(*invariants));
        free((yyvsp[-4].string));
        delete fields;
        delete invariants;
    }
#line 1519 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 14: /* class_declaration: CLASS IDENTIFIER INHERITS IDENTIFIER LBRACE field_list invariant_list RBRACE  */
#line 226 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* fields = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>((yyvsp[-2].fieldList));
        auto* invariants = static_cast<std::vector<std::unique_ptr<bbfm::Invariant>>*>((yyvsp[-1].invariantList));
        (yyval.classDecl) = new bbfm::ClassDeclaration((yyvsp[-6].string), (yyvsp[-4].string), std::move(*fields), std::move(*invariants));
        free((yyvsp[-6].string));
        free((yyvsp[-4].string));
        delete fields;
        delete invariants;
    }
#line 1533 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 15: /* field_list: %empty  */
#line 239 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.fieldList) = new std::vector<std::unique_ptr<bbfm::Field>>(); }
#line 1539 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 16: /* field_list: field_list field  */
#line 241 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Field>>*>((yyvsp[-1].fieldList));
        list->push_back(std::unique_ptr<bbfm::Field>(static_cast<bbfm::Field*>((yyvsp[0].field))));
        (yyval.fieldList) = list;
    }
#line 1549 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 17: /* invariant_list: %empty  */
#line 250 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.invariantList) = new std::vector<std::unique_ptr<bbfm::Invariant>>(); }
#line 1555 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 18: /* invariant_list: invariant_list invariant  */
#line 252 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Invariant>>*>((yyvsp[-1].invariantList));
        list->push_back(std::unique_ptr<bbfm::Invariant>(static_cast<bbfm::Invariant*>((yyvsp[0].invariant))));
        (yyval.invariantList) = list;
    }
#line 1565 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 19: /* attribute_name: IDENTIFIER  */
#line 260 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = (yyvsp[0].string); }
#line 1571 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 20: /* attribute_name: STRING_TYPE  */
#line 261 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("string"); }
#line 1577 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 21: /* attribute_name: INT_TYPE  */
#line 262 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("int"); }
#line 1583 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 22: /* attribute_name: REAL_TYPE  */
#line 263 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("real"); }
#line 1589 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 23: /* attribute_name: BOOL_TYPE  */
#line 264 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("bool"); }
#line 1595 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 24: /* attribute_name: TIMESTAMP_TYPE  */
#line 265 "Z:/_me/model-compiler/src/model-compiler.y"
                     { (yyval.string) = MakeString("timestamp"); }
#line 1601 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 25: /* attribute_name: TIMESPAN_TYPE  */
#line 266 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("timespan"); }
#line 1607 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 26: /* attribute_name: DATE_TYPE  */
#line 267 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("date"); }
#line 1613 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 27: /* attribute_name: GUID_TYPE  */
#line 268 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("guid"); }
#line 1619 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 28: /* invariant: INVARIANT IDENTIFIER COLON expression SEMICOLON  */
#line 273 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        (yyval.invariant) = new bbfm::Invariant((yyvsp[-3].string),
            std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-1].expression))));
        free((yyvsp[-3].string));
    }
#line 1629 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 29: /* field_name: IDENTIFIER  */
#line 281 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = (yyvsp[0].string); }
#line 1635 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 30: /* field_name: STRING_TYPE  */
#line 282 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("string"); }
#line 1641 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 31: /* field_name: INT_TYPE  */
#line 283 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("int"); }
#line 1647 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 32: /* field_name: REAL_TYPE  */
#line 284 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("real"); }
#line 1653 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 33: /* field_name: BOOL_TYPE  */
#line 285 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("bool"); }
#line 1659 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 34: /* field_name: TIMESTAMP_TYPE  */
#line 286 "Z:/_me/model-compiler/src/model-compiler.y"
                     { (yyval.string) = MakeString("timestamp"); }
#line 1665 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 35: /* field_name: TIMESPAN_TYPE  */
#line 287 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("timespan"); }
#line 1671 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 36: /* field_name: DATE_TYPE  */
#line 288 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("date"); }
#line 1677 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 37: /* field_name: GUID_TYPE  */
#line 289 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.string) = MakeString("guid"); }
#line 1683 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 38: /* field: FEATURE field_name COLON type_spec modifier_spec SEMICOLON  */
#line 294 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* type = static_cast<bbfm::TypeSpec*>((yyvsp[-2].typeSpec));
        auto* modifiers = static_cast<std::vector<std::unique_ptr<bbfm::Modifier>>*>((yyvsp[-1].modifierList));
        (yyval.field) = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            (yyvsp[-4].string),
            std::move(*modifiers),
            false
        );
        free((yyvsp[-4].string));
        delete modifiers;
    }
#line 1700 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 39: /* field: FEATURE field_name COLON type_spec SEMICOLON  */
#line 307 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        // Default modifier: [1] (mandatory single value)
        auto* type = static_cast<bbfm::TypeSpec*>((yyvsp[-1].typeSpec));
        auto modifiers = std::vector<std::unique_ptr<bbfm::Modifier>>();
        modifiers.push_back(std::make_unique<bbfm::CardinalityModifier>(1, 1));

        (yyval.field) = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            (yyvsp[-3].string),
            std::move(modifiers),
            false
        );
        free((yyvsp[-3].string));
    }
#line 1719 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 40: /* field: FEATURE field_name COLON type_spec modifier_spec EQUALS expression SEMICOLON  */
#line 322 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        // Computed feature with modifiers
        auto* type = static_cast<bbfm::TypeSpec*>((yyvsp[-4].typeSpec));
        auto* modifiers = static_cast<std::vector<std::unique_ptr<bbfm::Modifier>>*>((yyvsp[-3].modifierList));
        auto* expr = static_cast<bbfm::Expression*>((yyvsp[-1].expression));
        (yyval.field) = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            (yyvsp[-6].string),
            std::move(*modifiers),
            false,
            std::unique_ptr<bbfm::Expression>(expr)
        );
        free((yyvsp[-6].string));
        delete modifiers;
    }
#line 1739 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 41: /* field: FEATURE field_name COLON type_spec EQUALS expression SEMICOLON  */
#line 338 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        // Computed feature with default modifier [1]
        auto* type = static_cast<bbfm::TypeSpec*>((yyvsp[-3].typeSpec));
        auto* expr = static_cast<bbfm::Expression*>((yyvsp[-1].expression));
        auto modifiers = std::vector<std::unique_ptr<bbfm::Modifier>>();
        modifiers.push_back(std::make_unique<bbfm::CardinalityModifier>(1, 1));

        (yyval.field) = new bbfm::Field(
            std::unique_ptr<bbfm::TypeSpec>(type),
            (yyvsp[-5].string),
            std::move(modifiers),
            false,
            std::unique_ptr<bbfm::Expression>(expr)
        );
        free((yyvsp[-5].string));
    }
#line 1760 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 42: /* field: ALIAS field_name EQUALS field_name SEMICOLON  */
#line 355 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        // Alias field: alias startTime = timestamp;
        // We mark this by using a FieldReference expression as initializer and setting isAlias flag
        auto* targetExpr = new bbfm::FieldReference((yyvsp[-1].string));
        auto modifiers = std::vector<std::unique_ptr<bbfm::Modifier>>();
        modifiers.push_back(std::make_unique<bbfm::CardinalityModifier>(1, 1));

        (yyval.field) = new bbfm::Field(
            nullptr, // Type will be inferred from target field
            (yyvsp[-3].string),
            std::move(modifiers),
            false,
            std::unique_ptr<bbfm::Expression>(targetExpr),
            true  // isAlias = true
        );
        free((yyvsp[-3].string));
        free((yyvsp[-1].string));
    }
#line 1783 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 43: /* type_spec: STRING_TYPE  */
#line 376 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::STRING); }
#line 1789 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 44: /* type_spec: INT_TYPE  */
#line 377 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::INT); }
#line 1795 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 45: /* type_spec: REAL_TYPE  */
#line 378 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::REAL); }
#line 1801 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 46: /* type_spec: BOOL_TYPE  */
#line 379 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::BOOL); }
#line 1807 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 47: /* type_spec: TIMESTAMP_TYPE  */
#line 380 "Z:/_me/model-compiler/src/model-compiler.y"
                     { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::TIMESTAMP); }
#line 1813 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 48: /* type_spec: TIMESPAN_TYPE  */
#line 381 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::TIMESPAN); }
#line 1819 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 49: /* type_spec: DATE_TYPE  */
#line 382 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::DATE); }
#line 1825 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 50: /* type_spec: GUID_TYPE  */
#line 383 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::PrimitiveTypeSpec(bbfm::PrimitiveType::GUID); }
#line 1831 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 51: /* type_spec: IDENTIFIER  */
#line 384 "Z:/_me/model-compiler/src/model-compiler.y"
                    { (yyval.typeSpec) = new bbfm::UserDefinedTypeSpec((yyvsp[0].string)); free((yyvsp[0].string)); }
#line 1837 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 52: /* modifier_spec: LBRACKET modifier_list RBRACKET  */
#line 389 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.modifierList) = (yyvsp[-1].modifierList); }
#line 1843 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 53: /* modifier_list: modifier  */
#line 394 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = new std::vector<std::unique_ptr<bbfm::Modifier>>();
        list->push_back(std::unique_ptr<bbfm::Modifier>(static_cast<bbfm::Modifier*>((yyvsp[0].modifier))));
        (yyval.modifierList) = list;
    }
#line 1853 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 54: /* modifier_list: modifier_list COMMA modifier  */
#line 400 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        auto* list = static_cast<std::vector<std::unique_ptr<bbfm::Modifier>>*>((yyvsp[-2].modifierList));
        list->push_back(std::unique_ptr<bbfm::Modifier>(static_cast<bbfm::Modifier*>((yyvsp[0].modifier))));
        (yyval.modifierList) = list;
    }
#line 1863 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 55: /* modifier: INTEGER_LITERAL  */
#line 409 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.modifier) = new bbfm::CardinalityModifier((yyvsp[0].integer), (yyvsp[0].integer)); }
#line 1869 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 56: /* modifier: INTEGER_LITERAL DOTDOT INTEGER_LITERAL  */
#line 411 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.modifier) = new bbfm::CardinalityModifier((yyvsp[-2].integer), (yyvsp[0].integer)); }
#line 1875 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 57: /* modifier: INTEGER_LITERAL DOTDOT ASTERISK  */
#line 413 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.modifier) = new bbfm::CardinalityModifier((yyvsp[-2].integer), -1); }
#line 1881 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 58: /* modifier: OPTIONAL  */
#line 415 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.modifier) = new bbfm::CardinalityModifier(0, 1); }
#line 1887 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 59: /* modifier: UNIQUE  */
#line 417 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.modifier) = new bbfm::UniqueModifier(); }
#line 1893 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 60: /* expression: expression PLUS expression  */
#line 423 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::ADD,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1903 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 61: /* expression: expression MINUS expression  */
#line 429 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::SUB,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1913 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 62: /* expression: expression ASTERISK expression  */
#line 435 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::MUL,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1923 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 63: /* expression: expression SLASH expression  */
#line 441 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::DIV,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1933 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 64: /* expression: expression PERCENT expression  */
#line 447 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::MOD,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1943 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 65: /* expression: expression LT expression  */
#line 453 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::LT,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1953 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 66: /* expression: expression GT expression  */
#line 459 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::GT,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1963 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 67: /* expression: expression LE expression  */
#line 465 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::LE,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1973 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 68: /* expression: expression GE expression  */
#line 471 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::GE,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1983 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 69: /* expression: expression EQ expression  */
#line 477 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::EQ,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 1993 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 70: /* expression: expression NE expression  */
#line 483 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::NE,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 2003 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 71: /* expression: expression AND expression  */
#line 489 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::AND,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 2013 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 72: /* expression: expression OR expression  */
#line 495 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::BinaryExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        bbfm::BinaryExpression::Op::OR,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 2023 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 73: /* expression: MINUS expression  */
#line 501 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::UnaryExpression(
        bbfm::UnaryExpression::Op::NEG,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 2032 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 74: /* expression: NOT expression  */
#line 506 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::UnaryExpression(
        bbfm::UnaryExpression::Op::NOT,
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[0].expression)))
    ); }
#line 2041 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 75: /* expression: LPAREN expression RPAREN  */
#line 511 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::ParenthesizedExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-1].expression)))
    ); }
#line 2049 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 76: /* expression: expression DOT IDENTIFIER  */
#line 515 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::MemberAccessExpression(
        std::unique_ptr<bbfm::Expression>(static_cast<bbfm::Expression*>((yyvsp[-2].expression))),
        (yyvsp[0].string)
    );
      free((yyvsp[0].string));
    }
#line 2060 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 77: /* expression: primary_expression  */
#line 522 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = (yyvsp[0].expression); }
#line 2066 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 78: /* primary_expression: INTEGER_LITERAL  */
#line 527 "Z:/_me/model-compiler/src/model-compiler.y"
    { (yyval.expression) = new bbfm::LiteralExpression(static_cast<int64_t>((yyvsp[0].integer))); }
#line 2072 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 79: /* primary_expression: REAL_LITERAL  */
#line 529 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        double val = atof((yyvsp[0].string));
        (yyval.expression) = new bbfm::LiteralExpression(val);
        free((yyvsp[0].string));
    }
#line 2082 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 80: /* primary_expression: STRING_LITERAL  */
#line 535 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        (yyval.expression) = new bbfm::LiteralExpression(std::string((yyvsp[0].string)));
        free((yyvsp[0].string));
    }
#line 2091 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 81: /* primary_expression: BOOL_LITERAL  */
#line 540 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        bool val = (0 == StringCompareIgnoreCase((yyvsp[0].string), "true"));
        (yyval.expression) = new bbfm::LiteralExpression(val);
        free((yyvsp[0].string));
    }
#line 2101 "Z:/_me/model-compiler/_build/parser.cpp"
    break;

  case 82: /* primary_expression: attribute_name  */
#line 546 "Z:/_me/model-compiler/src/model-compiler.y"
    {
        (yyval.expression) = new bbfm::FieldReference((yyvsp[0].string));
        free((yyvsp[0].string));
    }
#line 2110 "Z:/_me/model-compiler/_build/parser.cpp"
    break;


#line 2114 "Z:/_me/model-compiler/_build/parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 552 "Z:/_me/model-compiler/src/model-compiler.y"


/* Ensure C linkage when compiled as C++ */
#ifdef __cplusplus
extern "C" {
#endif

extern "C" void yyerror(const char *s) {
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
