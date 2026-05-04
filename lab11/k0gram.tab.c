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
#line 6 "k0gram.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "token.h"

#define YYDEBUG 1
int  yylex(void);
void yyerror(const char *s);

extern int   lineno;
extern char *current_filename;

tree_t *g_root         = NULL;
extern int g_lex_errors;
int     g_syntax_errors = 0;

#line 90 "k0gram.tab.c"

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

#include "k0gram.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_FUN = 3,                        /* FUN  */
  YYSYMBOL_VAL = 4,                        /* VAL  */
  YYSYMBOL_VAR = 5,                        /* VAR  */
  YYSYMBOL_IF = 6,                         /* IF  */
  YYSYMBOL_ELSE = 7,                       /* ELSE  */
  YYSYMBOL_WHILE = 8,                      /* WHILE  */
  YYSYMBOL_FOR = 9,                        /* FOR  */
  YYSYMBOL_RETURN = 10,                    /* RETURN  */
  YYSYMBOL_BREAK = 11,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 12,                  /* CONTINUE  */
  YYSYMBOL_WHEN = 13,                      /* WHEN  */
  YYSYMBOL_IN = 14,                        /* IN  */
  YYSYMBOL_DO = 15,                        /* DO  */
  YYSYMBOL_IMPORT = 16,                    /* IMPORT  */
  YYSYMBOL_CONST = 17,                     /* CONST  */
  YYSYMBOL_INTEGERLITERAL = 18,            /* INTEGERLITERAL  */
  YYSYMBOL_LONGLITERAL = 19,               /* LONGLITERAL  */
  YYSYMBOL_REALLITERAL = 20,               /* REALLITERAL  */
  YYSYMBOL_DOUBLELITERAL = 21,             /* DOUBLELITERAL  */
  YYSYMBOL_BOOLEANLITERAL = 22,            /* BOOLEANLITERAL  */
  YYSYMBOL_CHARACTERLITERAL = 23,          /* CHARACTERLITERAL  */
  YYSYMBOL_STRINGLITERAL = 24,             /* STRINGLITERAL  */
  YYSYMBOL_NULLLITERAL = 25,               /* NULLLITERAL  */
  YYSYMBOL_IDENT = 26,                     /* IDENT  */
  YYSYMBOL_ASSIGNMENT = 27,                /* ASSIGNMENT  */
  YYSYMBOL_ADD_ASSIGN = 28,                /* ADD_ASSIGN  */
  YYSYMBOL_SUB_ASSIGN = 29,                /* SUB_ASSIGN  */
  YYSYMBOL_MUL_ASSIGN = 30,                /* MUL_ASSIGN  */
  YYSYMBOL_DIV_ASSIGN = 31,                /* DIV_ASSIGN  */
  YYSYMBOL_MOD_ASSIGN = 32,                /* MOD_ASSIGN  */
  YYSYMBOL_ADD = 33,                       /* ADD  */
  YYSYMBOL_SUB = 34,                       /* SUB  */
  YYSYMBOL_MUL = 35,                       /* MUL  */
  YYSYMBOL_DIV = 36,                       /* DIV  */
  YYSYMBOL_MOD = 37,                       /* MOD  */
  YYSYMBOL_INCR = 38,                      /* INCR  */
  YYSYMBOL_DECR = 39,                      /* DECR  */
  YYSYMBOL_EQ = 40,                        /* EQ  */
  YYSYMBOL_NEQ = 41,                       /* NEQ  */
  YYSYMBOL_GTE = 42,                       /* GTE  */
  YYSYMBOL_LTE = 43,                       /* LTE  */
  YYSYMBOL_REF_EQ = 44,                    /* REF_EQ  */
  YYSYMBOL_REF_NEQ = 45,                   /* REF_NEQ  */
  YYSYMBOL_AND = 46,                       /* AND  */
  YYSYMBOL_OR = 47,                        /* OR  */
  YYSYMBOL_NOT = 48,                       /* NOT  */
  YYSYMBOL_SAFE_CALL = 49,                 /* SAFE_CALL  */
  YYSYMBOL_ELVIS = 50,                     /* ELVIS  */
  YYSYMBOL_QUEST = 51,                     /* QUEST  */
  YYSYMBOL_RANGE = 52,                     /* RANGE  */
  YYSYMBOL_RANGE_UNTIL = 53,               /* RANGE_UNTIL  */
  YYSYMBOL_AS = 54,                        /* AS  */
  YYSYMBOL_LPAREN = 55,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 56,                    /* RPAREN  */
  YYSYMBOL_LCURL = 57,                     /* LCURL  */
  YYSYMBOL_RCURL = 58,                     /* RCURL  */
  YYSYMBOL_LSQUARE = 59,                   /* LSQUARE  */
  YYSYMBOL_RSQUARE = 60,                   /* RSQUARE  */
  YYSYMBOL_LANGLE = 61,                    /* LANGLE  */
  YYSYMBOL_RANGLE = 62,                    /* RANGLE  */
  YYSYMBOL_COMMA = 63,                     /* COMMA  */
  YYSYMBOL_DOT = 64,                       /* DOT  */
  YYSYMBOL_COLON = 65,                     /* COLON  */
  YYSYMBOL_SEMI = 66,                      /* SEMI  */
  YYSYMBOL_ARROW = 67,                     /* ARROW  */
  YYSYMBOL_DOUBLE_COLON = 68,              /* DOUBLE_COLON  */
  YYSYMBOL_YYACCEPT = 69,                  /* $accept  */
  YYSYMBOL_program = 70,                   /* program  */
  YYSYMBOL_topLevelObjects = 71,           /* topLevelObjects  */
  YYSYMBOL_topLevelObject = 72,            /* topLevelObject  */
  YYSYMBOL_functionDeclaration = 73,       /* functionDeclaration  */
  YYSYMBOL_functionValueParameters = 74,   /* functionValueParameters  */
  YYSYMBOL_functionValueParameterList = 75, /* functionValueParameterList  */
  YYSYMBOL_functionValueParameter = 76,    /* functionValueParameter  */
  YYSYMBOL_type = 77,                      /* type  */
  YYSYMBOL_typeArgumentList = 78,          /* typeArgumentList  */
  YYSYMBOL_block = 79,                     /* block  */
  YYSYMBOL_statements = 80,                /* statements  */
  YYSYMBOL_statement = 81,                 /* statement  */
  YYSYMBOL_varDeclaration = 82,            /* varDeclaration  */
  YYSYMBOL_assignment = 83,                /* assignment  */
  YYSYMBOL_ifStatement = 84,               /* ifStatement  */
  YYSYMBOL_whileStatement = 85,            /* whileStatement  */
  YYSYMBOL_forStatement = 86,              /* forStatement  */
  YYSYMBOL_jumpStatement = 87,             /* jumpStatement  */
  YYSYMBOL_controlStructureBody = 88,      /* controlStructureBody  */
  YYSYMBOL_expression = 89,                /* expression  */
  YYSYMBOL_argumentList = 90,              /* argumentList  */
  YYSYMBOL_literal = 91                    /* literal  */
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
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   634

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  176

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   323


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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   153,   153,   161,   165,   172,   181,   188,   196,   204,
     216,   217,   222,   224,   232,   237,   246,   248,   250,   256,
     265,   267,   277,   288,   289,   300,   301,   302,   303,   304,
     305,   306,   307,   309,   316,   322,   329,   335,   342,   353,
     358,   363,   368,   373,   378,   388,   394,   404,   413,   423,
     425,   427,   429,   434,   435,   441,   443,   445,   450,   455,
     460,   465,   470,   475,   477,   479,   481,   483,   485,   487,
     489,   491,   493,   495,   497,   499,   501,   503,   505,   507,
     509,   511,   513,   515,   517,   519,   524,   526,   536,   537,
     538,   539,   540,   541,   542,   543
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
  "\"end of file\"", "error", "\"invalid token\"", "FUN", "VAL", "VAR",
  "IF", "ELSE", "WHILE", "FOR", "RETURN", "BREAK", "CONTINUE", "WHEN",
  "IN", "DO", "IMPORT", "CONST", "INTEGERLITERAL", "LONGLITERAL",
  "REALLITERAL", "DOUBLELITERAL", "BOOLEANLITERAL", "CHARACTERLITERAL",
  "STRINGLITERAL", "NULLLITERAL", "IDENT", "ASSIGNMENT", "ADD_ASSIGN",
  "SUB_ASSIGN", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN", "ADD", "SUB",
  "MUL", "DIV", "MOD", "INCR", "DECR", "EQ", "NEQ", "GTE", "LTE", "REF_EQ",
  "REF_NEQ", "AND", "OR", "NOT", "SAFE_CALL", "ELVIS", "QUEST", "RANGE",
  "RANGE_UNTIL", "AS", "LPAREN", "RPAREN", "LCURL", "RCURL", "LSQUARE",
  "RSQUARE", "LANGLE", "RANGLE", "COMMA", "DOT", "COLON", "SEMI", "ARROW",
  "DOUBLE_COLON", "$accept", "program", "topLevelObjects",
  "topLevelObject", "functionDeclaration", "functionValueParameters",
  "functionValueParameterList", "functionValueParameter", "type",
  "typeArgumentList", "block", "statements", "statement", "varDeclaration",
  "assignment", "ifStatement", "whileStatement", "forStatement",
  "jumpStatement", "controlStructureBody", "expression", "argumentList",
  "literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-160)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       1,     3,    15,     1,  -160,  -160,   -10,  -160,  -160,    10,
       5,    16,    -4,  -160,    45,    22,    10,   -40,    46,   188,
    -160,    45,  -160,  -160,  -160,    45,   188,  -160,  -160,  -160,
    -160,  -160,  -160,  -160,  -160,  -160,   188,   188,   188,   405,
    -160,   130,   -22,  -160,    -6,   405,   570,   108,   245,   188,
     188,   188,   188,   188,  -160,  -160,   188,   188,   188,   188,
     188,   188,   188,   188,    49,   188,   188,   188,    45,    43,
     188,   188,   188,    50,    52,    54,    26,    27,    29,   188,
    -160,  -160,    -5,  -160,  -160,  -160,  -160,  -160,  -160,  -160,
    -160,  -160,   211,   188,  -160,    34,    45,  -160,   570,   570,
     108,   108,   108,   501,   501,   533,   533,   501,   501,   469,
     437,  -160,   405,   540,   540,  -160,  -160,   405,   -53,   277,
     533,   533,  -160,   -11,    -7,   188,   188,    60,   405,   188,
     188,   188,   188,   188,   188,  -160,   405,  -160,  -160,  -160,
     188,  -160,   188,    45,   188,    45,   309,   341,    74,   405,
     405,   405,   405,   405,   405,   405,   405,    62,   405,    63,
     171,   171,   188,   188,   188,  -160,  -160,    85,  -160,   373,
     405,   405,   171,   171,  -160,  -160
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     3,     5,     0,     1,     4,    10,
       0,     0,    11,    12,     0,     0,     0,    16,    14,     0,
      23,     0,     6,    13,    17,     0,     0,    88,    89,    90,
      91,    92,    93,    94,    95,    56,     0,     0,     0,     8,
      55,     0,     0,    20,     0,    15,    68,    69,     0,     0,
       0,     0,     0,     0,    70,    71,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    49,
      51,    52,    56,    22,    26,    24,    25,    31,    27,    28,
      29,    30,    33,     0,     7,    18,     0,    85,    63,    64,
      65,    66,    67,    72,    73,    77,    76,    78,    79,    80,
      81,    61,    84,    82,    83,    62,    57,    86,     0,     0,
      74,    75,    60,     0,     0,     0,     0,     0,    50,     0,
       0,     0,     0,     0,     0,    32,     9,    19,    21,    58,
       0,    59,     0,     0,     0,     0,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    87,    34,     0,    36,    38,
       0,     0,     0,     0,     0,    53,    54,    45,    47,     0,
      35,    37,     0,     0,    46,    48
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -160,  -160,  -160,    90,     6,  -160,  -160,    78,   -13,  -160,
     -14,  -160,    55,  -160,  -160,  -160,  -160,  -160,  -160,  -159,
     -19,  -160,  -160
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,     4,    84,    11,    12,    13,    18,    44,
     165,    41,   166,    86,    87,    88,    89,    90,    91,   167,
      92,   118,    40
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      39,    22,   168,   139,     1,    93,     5,    45,    42,     5,
     140,    24,    43,   174,   175,     7,   142,    46,    47,    48,
     144,    25,   129,   130,   131,   132,   133,   134,    94,     6,
      98,    99,   100,   101,   102,    20,    10,   103,   104,   105,
     106,   107,   108,   109,   110,     9,   112,   113,   114,    19,
     117,   119,   120,   121,   143,   115,    95,    96,   145,    16,
     128,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      14,    17,    15,    26,   136,   111,   122,    36,   123,    20,
     124,   125,   126,   138,   127,   137,   148,    21,   162,   163,
     164,    37,   172,     8,    23,     0,    85,     0,    38,   116,
       0,     0,     0,     0,     0,     0,   146,   147,     0,     0,
     149,   150,   151,   152,   153,   154,     0,     0,     0,     0,
       0,   155,     0,   156,     0,   158,     0,     0,     0,     0,
     157,     0,   159,     1,    74,    75,    76,     0,    77,    78,
      79,    80,    81,   169,   170,   171,    54,    55,    27,    28,
      29,    30,    31,    32,    33,    34,    82,    64,    65,     0,
       0,     0,    68,    69,    36,     0,     0,    70,     0,     0,
       0,     0,    73,     0,     1,    74,    75,    76,    37,    77,
      78,    79,    80,    81,     0,    38,     0,     0,    83,    27,
      28,    29,    30,    31,    32,    33,    34,    82,     0,     0,
       0,     0,     0,     0,     0,    36,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,     0,     0,    37,
       0,     0,    36,     0,     0,     0,    38,     0,    20,     0,
       0,     0,     0,     0,     0,     0,    37,     0,     0,     0,
       0,     0,     0,    38,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,     0,
      64,    65,     0,    66,    67,    68,    69,     0,     0,     0,
      70,     0,    71,    72,     0,    73,     0,   135,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,     0,    64,    65,     0,    66,    67,    68,
      69,    97,     0,     0,    70,     0,    71,    72,     0,    73,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,     0,    64,    65,     0,    66,
      67,    68,    69,     0,     0,     0,    70,   141,    71,    72,
       0,    73,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,     0,    64,    65,
       0,    66,    67,    68,    69,   160,     0,     0,    70,     0,
      71,    72,     0,    73,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,     0,
      64,    65,     0,    66,    67,    68,    69,   161,     0,     0,
      70,     0,    71,    72,     0,    73,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,     0,    64,    65,     0,    66,    67,    68,    69,   173,
       0,     0,    70,     0,    71,    72,     0,    73,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,     0,    64,    65,     0,    66,    67,    68,
      69,     0,     0,     0,    70,     0,    71,    72,     0,    73,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,     0,     0,    64,    65,     0,    66,
      67,    68,    69,     0,     0,     0,    70,     0,    71,    72,
       0,    73,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,     0,     0,     0,    64,    65,
       0,    66,    67,    68,    69,     0,     0,     0,    70,     0,
      71,    72,     0,    73,    49,    50,    51,    52,    53,    54,
      55,     0,     0,    58,    59,     0,     0,     0,     0,     0,
      64,    65,     0,    66,    67,    68,    69,     0,     0,     0,
      70,     0,    71,    72,     0,    73,    49,    50,    51,    52,
      53,    54,    55,    49,    50,    51,    52,    53,    54,    55,
       0,     0,    64,    65,     0,    66,    67,    68,    69,    64,
      65,     0,    70,     0,    68,    69,     0,    73,     0,    70,
       0,     0,     0,     0,    73,    51,    52,    53,    54,    55,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    64,
      65,     0,     0,     0,    68,    69,     0,     0,     0,    70,
       0,     0,     0,     0,    73
};

static const yytype_int16 yycheck[] =
{
      19,    15,   161,    56,     3,    27,     0,    26,    21,     3,
      63,    51,    25,   172,   173,     0,    27,    36,    37,    38,
      27,    61,    27,    28,    29,    30,    31,    32,    42,    26,
      49,    50,    51,    52,    53,    57,    26,    56,    57,    58,
      59,    60,    61,    62,    63,    55,    65,    66,    67,    27,
      69,    70,    71,    72,    65,    68,    62,    63,    65,    63,
      79,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      65,    26,    56,    27,    93,    26,    26,    34,    26,    57,
      26,    55,    55,    96,    55,    51,    26,    65,    14,    27,
      27,    48,     7,     3,    16,    -1,    41,    -1,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,   125,   126,    -1,    -1,
     129,   130,   131,   132,   133,   134,    -1,    -1,    -1,    -1,
      -1,   140,    -1,   142,    -1,   144,    -1,    -1,    -1,    -1,
     143,    -1,   145,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    12,   162,   163,   164,    38,    39,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    49,    50,    -1,
      -1,    -1,    54,    55,    34,    -1,    -1,    59,    -1,    -1,
      -1,    -1,    64,    -1,     3,     4,     5,     6,    48,     8,
       9,    10,    11,    12,    -1,    55,    -1,    -1,    58,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    34,    -1,    -1,    -1,    55,    -1,    57,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      49,    50,    -1,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    -1,    61,    62,    -1,    64,    -1,    66,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    49,    50,    -1,    52,    53,    54,
      55,    56,    -1,    -1,    59,    -1,    61,    62,    -1,    64,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    -1,    49,    50,    -1,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    60,    61,    62,
      -1,    64,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    -1,    49,    50,
      -1,    52,    53,    54,    55,    56,    -1,    -1,    59,    -1,
      61,    62,    -1,    64,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
      49,    50,    -1,    52,    53,    54,    55,    56,    -1,    -1,
      59,    -1,    61,    62,    -1,    64,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    -1,    49,    50,    -1,    52,    53,    54,    55,    56,
      -1,    -1,    59,    -1,    61,    62,    -1,    64,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    49,    50,    -1,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    -1,    61,    62,    -1,    64,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    -1,    -1,    49,    50,    -1,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    -1,    61,    62,
      -1,    64,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    -1,    -1,    -1,    49,    50,
      -1,    52,    53,    54,    55,    -1,    -1,    -1,    59,    -1,
      61,    62,    -1,    64,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    -1,
      49,    50,    -1,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    -1,    61,    62,    -1,    64,    33,    34,    35,    36,
      37,    38,    39,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    49,    50,    -1,    52,    53,    54,    55,    49,
      50,    -1,    59,    -1,    54,    55,    -1,    64,    -1,    59,
      -1,    -1,    -1,    -1,    64,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    59,
      -1,    -1,    -1,    -1,    64
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    70,    71,    72,    73,    26,     0,    72,    55,
      26,    74,    75,    76,    65,    56,    63,    26,    77,    27,
      57,    65,    79,    76,    51,    61,    27,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    34,    48,    55,    89,
      91,    80,    77,    77,    78,    89,    89,    89,    89,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    49,    50,    52,    53,    54,    55,
      59,    61,    62,    64,     4,     5,     6,     8,     9,    10,
      11,    12,    26,    58,    73,    81,    82,    83,    84,    85,
      86,    87,    89,    27,    79,    62,    63,    56,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    26,    89,    89,    89,    77,    56,    89,    90,    89,
      89,    89,    26,    26,    26,    55,    55,    55,    89,    27,
      28,    29,    30,    31,    32,    66,    89,    51,    77,    56,
      63,    60,    27,    65,    27,    65,    89,    89,    26,    89,
      89,    89,    89,    89,    89,    89,    89,    77,    89,    77,
      56,    56,    14,    27,    27,    79,    81,    88,    88,    89,
      89,    89,     7,    56,    88,    88
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    69,    70,    71,    71,    72,    73,    73,    73,    73,
      74,    74,    75,    75,    76,    76,    77,    77,    77,    77,
      78,    78,    79,    80,    80,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    82,    82,    82,    82,    82,    83,
      83,    83,    83,    83,    83,    84,    84,    85,    86,    87,
      87,    87,    87,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    90,    90,    91,    91,
      91,    91,    91,    91,    91,    91
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     6,     8,     7,     9,
       0,     1,     1,     3,     3,     5,     1,     2,     4,     5,
       1,     3,     3,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     4,     6,     4,     6,     4,     3,
       3,     3,     3,     3,     3,     5,     7,     5,     7,     1,
       2,     1,     1,     1,     1,     1,     1,     3,     4,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1
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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
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
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
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

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: topLevelObjects  */
#line 154 "k0gram.y"
        {
            (yyval.tree) = tree_node("program", 1, 1, (yyvsp[0].tree));
            g_root = (yyval.tree);
        }
#line 1387 "k0gram.tab.c"
    break;

  case 3: /* topLevelObjects: topLevelObject  */
#line 162 "k0gram.y"
        {
            (yyval.tree) = (yyvsp[0].tree);
        }
#line 1395 "k0gram.tab.c"
    break;

  case 4: /* topLevelObjects: topLevelObjects topLevelObject  */
#line 166 "k0gram.y"
        {
            (yyval.tree) = tree_node("topLevelObjects", 1, 2, (yyvsp[-1].tree), (yyvsp[0].tree));
        }
#line 1403 "k0gram.tab.c"
    break;

  case 5: /* topLevelObject: functionDeclaration  */
#line 173 "k0gram.y"
        {
            (yyval.tree) = tree_node("topLevelObject", 1, 1, (yyvsp[0].tree));
        }
#line 1411 "k0gram.tab.c"
    break;

  case 6: /* functionDeclaration: FUN IDENT LPAREN functionValueParameters RPAREN block  */
#line 182 "k0gram.y"
        {
            (yyval.tree) = tree_node("functionDeclaration", 1, 6,
                           tree_leaf((yyvsp[-5].tok)), tree_leaf((yyvsp[-4].tok)),
                           tree_leaf((yyvsp[-3].tok)), (yyvsp[-2].tree),
                           tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1422 "k0gram.tab.c"
    break;

  case 7: /* functionDeclaration: FUN IDENT LPAREN functionValueParameters RPAREN COLON type block  */
#line 189 "k0gram.y"
        {
            (yyval.tree) = tree_node("functionDeclaration", 2, 8,
                           tree_leaf((yyvsp[-7].tok)), tree_leaf((yyvsp[-6].tok)),
                           tree_leaf((yyvsp[-5].tok)), (yyvsp[-4].tree),
                           tree_leaf((yyvsp[-3].tok)), tree_leaf((yyvsp[-2].tok)),
                           (yyvsp[-1].tree), (yyvsp[0].tree));
        }
#line 1434 "k0gram.tab.c"
    break;

  case 8: /* functionDeclaration: FUN IDENT LPAREN functionValueParameters RPAREN ASSIGNMENT expression  */
#line 197 "k0gram.y"
        {
            (yyval.tree) = tree_node("functionDeclaration", 3, 7,
                           tree_leaf((yyvsp[-6].tok)), tree_leaf((yyvsp[-5].tok)),
                           tree_leaf((yyvsp[-4].tok)), (yyvsp[-3].tree),
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)),
                           (yyvsp[0].tree));
        }
#line 1446 "k0gram.tab.c"
    break;

  case 9: /* functionDeclaration: FUN IDENT LPAREN functionValueParameters RPAREN COLON type ASSIGNMENT expression  */
#line 205 "k0gram.y"
        {
            (yyval.tree) = tree_node("functionDeclaration", 4, 9,
                           tree_leaf((yyvsp[-8].tok)), tree_leaf((yyvsp[-7].tok)),
                           tree_leaf((yyvsp[-6].tok)), (yyvsp[-5].tree),
                           tree_leaf((yyvsp[-4].tok)), tree_leaf((yyvsp[-3].tok)),
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1458 "k0gram.tab.c"
    break;

  case 10: /* functionValueParameters: %empty  */
#line 216 "k0gram.y"
        { (yyval.tree) = NULL; }
#line 1464 "k0gram.tab.c"
    break;

  case 11: /* functionValueParameters: functionValueParameterList  */
#line 218 "k0gram.y"
        { (yyval.tree) = (yyvsp[0].tree); }
#line 1470 "k0gram.tab.c"
    break;

  case 12: /* functionValueParameterList: functionValueParameter  */
#line 223 "k0gram.y"
        { (yyval.tree) = (yyvsp[0].tree); }
#line 1476 "k0gram.tab.c"
    break;

  case 13: /* functionValueParameterList: functionValueParameterList COMMA functionValueParameter  */
#line 225 "k0gram.y"
        {
            (yyval.tree) = tree_node("functionValueParameterList", 1, 3,
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1485 "k0gram.tab.c"
    break;

  case 14: /* functionValueParameter: IDENT COLON type  */
#line 233 "k0gram.y"
        {
            (yyval.tree) = tree_node("functionValueParameter", 1, 3,
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1494 "k0gram.tab.c"
    break;

  case 15: /* functionValueParameter: IDENT COLON type ASSIGNMENT expression  */
#line 238 "k0gram.y"
        {
            (yyval.tree) = tree_node("functionValueParameter", 2, 5,
                           tree_leaf((yyvsp[-4].tok)), tree_leaf((yyvsp[-3].tok)),
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1504 "k0gram.tab.c"
    break;

  case 16: /* type: IDENT  */
#line 247 "k0gram.y"
        { (yyval.tree) = tree_node("type", 1, 1, tree_leaf((yyvsp[0].tok))); }
#line 1510 "k0gram.tab.c"
    break;

  case 17: /* type: IDENT QUEST  */
#line 249 "k0gram.y"
        { (yyval.tree) = tree_node("type", 2, 2, tree_leaf((yyvsp[-1].tok)), tree_leaf((yyvsp[0].tok))); }
#line 1516 "k0gram.tab.c"
    break;

  case 18: /* type: IDENT LANGLE typeArgumentList RANGLE  */
#line 251 "k0gram.y"
        {
            (yyval.tree) = tree_node("type", 3, 4,
                           tree_leaf((yyvsp[-3].tok)), tree_leaf((yyvsp[-2].tok)),
                           (yyvsp[-1].tree), tree_leaf((yyvsp[0].tok)));
        }
#line 1526 "k0gram.tab.c"
    break;

  case 19: /* type: IDENT LANGLE typeArgumentList RANGLE QUEST  */
#line 257 "k0gram.y"
        {
            (yyval.tree) = tree_node("type", 4, 5,
                           tree_leaf((yyvsp[-4].tok)), tree_leaf((yyvsp[-3].tok)),
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), tree_leaf((yyvsp[0].tok)));
        }
#line 1536 "k0gram.tab.c"
    break;

  case 20: /* typeArgumentList: type  */
#line 266 "k0gram.y"
        { (yyval.tree) = (yyvsp[0].tree); }
#line 1542 "k0gram.tab.c"
    break;

  case 21: /* typeArgumentList: typeArgumentList COMMA type  */
#line 268 "k0gram.y"
        {
            (yyval.tree) = tree_node("typeArgumentList", 1, 3,
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1551 "k0gram.tab.c"
    break;

  case 22: /* block: LCURL statements RCURL  */
#line 278 "k0gram.y"
        {
            (yyval.tree) = tree_node("block", 1, 3,
                           tree_leaf((yyvsp[-2].tok)), (yyvsp[-1].tree), tree_leaf((yyvsp[0].tok)));
        }
#line 1560 "k0gram.tab.c"
    break;

  case 23: /* statements: %empty  */
#line 288 "k0gram.y"
        { (yyval.tree) = NULL; }
#line 1566 "k0gram.tab.c"
    break;

  case 24: /* statements: statements statement  */
#line 290 "k0gram.y"
        {
            if ((yyvsp[-1].tree) == NULL)
                (yyval.tree) = (yyvsp[0].tree);
            else
                (yyval.tree) = tree_node("statements", 1, 2, (yyvsp[-1].tree), (yyvsp[0].tree));
        }
#line 1577 "k0gram.tab.c"
    break;

  case 25: /* statement: varDeclaration  */
#line 300 "k0gram.y"
                            { (yyval.tree) = (yyvsp[0].tree); }
#line 1583 "k0gram.tab.c"
    break;

  case 26: /* statement: functionDeclaration  */
#line 301 "k0gram.y"
                            { (yyval.tree) = (yyvsp[0].tree); }
#line 1589 "k0gram.tab.c"
    break;

  case 27: /* statement: ifStatement  */
#line 302 "k0gram.y"
                            { (yyval.tree) = (yyvsp[0].tree); }
#line 1595 "k0gram.tab.c"
    break;

  case 28: /* statement: whileStatement  */
#line 303 "k0gram.y"
                            { (yyval.tree) = (yyvsp[0].tree); }
#line 1601 "k0gram.tab.c"
    break;

  case 29: /* statement: forStatement  */
#line 304 "k0gram.y"
                            { (yyval.tree) = (yyvsp[0].tree); }
#line 1607 "k0gram.tab.c"
    break;

  case 30: /* statement: jumpStatement  */
#line 305 "k0gram.y"
                            { (yyval.tree) = (yyvsp[0].tree); }
#line 1613 "k0gram.tab.c"
    break;

  case 31: /* statement: assignment  */
#line 306 "k0gram.y"
                            { (yyval.tree) = (yyvsp[0].tree); }
#line 1619 "k0gram.tab.c"
    break;

  case 32: /* statement: expression SEMI  */
#line 308 "k0gram.y"
        { (yyval.tree) = tree_node("exprStatement", 1, 2, (yyvsp[-1].tree), tree_leaf((yyvsp[0].tok))); }
#line 1625 "k0gram.tab.c"
    break;

  case 33: /* statement: expression  */
#line 310 "k0gram.y"
        { (yyval.tree) = (yyvsp[0].tree); }
#line 1631 "k0gram.tab.c"
    break;

  case 34: /* varDeclaration: VAL IDENT ASSIGNMENT expression  */
#line 317 "k0gram.y"
        {
            (yyval.tree) = tree_node("varDeclaration", 1, 4,
                           tree_leaf((yyvsp[-3].tok)), tree_leaf((yyvsp[-2].tok)),
                           tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1641 "k0gram.tab.c"
    break;

  case 35: /* varDeclaration: VAL IDENT COLON type ASSIGNMENT expression  */
#line 323 "k0gram.y"
        {
            (yyval.tree) = tree_node("varDeclaration", 2, 6,
                           tree_leaf((yyvsp[-5].tok)), tree_leaf((yyvsp[-4].tok)),
                           tree_leaf((yyvsp[-3].tok)), (yyvsp[-2].tree),
                           tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1652 "k0gram.tab.c"
    break;

  case 36: /* varDeclaration: VAR IDENT ASSIGNMENT expression  */
#line 330 "k0gram.y"
        {
            (yyval.tree) = tree_node("varDeclaration", 3, 4,
                           tree_leaf((yyvsp[-3].tok)), tree_leaf((yyvsp[-2].tok)),
                           tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1662 "k0gram.tab.c"
    break;

  case 37: /* varDeclaration: VAR IDENT COLON type ASSIGNMENT expression  */
#line 336 "k0gram.y"
        {
            (yyval.tree) = tree_node("varDeclaration", 4, 6,
                           tree_leaf((yyvsp[-5].tok)), tree_leaf((yyvsp[-4].tok)),
                           tree_leaf((yyvsp[-3].tok)), (yyvsp[-2].tree),
                           tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1673 "k0gram.tab.c"
    break;

  case 38: /* varDeclaration: VAR IDENT COLON type  */
#line 343 "k0gram.y"
        {
            (yyval.tree) = tree_node("varDeclaration", 5, 4,
                           tree_leaf((yyvsp[-3].tok)), tree_leaf((yyvsp[-2].tok)),
                           tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1683 "k0gram.tab.c"
    break;

  case 39: /* assignment: IDENT ASSIGNMENT expression  */
#line 354 "k0gram.y"
        {
            (yyval.tree) = tree_node("assignment", 1, 3,
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1692 "k0gram.tab.c"
    break;

  case 40: /* assignment: IDENT ADD_ASSIGN expression  */
#line 359 "k0gram.y"
        {
            (yyval.tree) = tree_node("assignment", 2, 3,
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1701 "k0gram.tab.c"
    break;

  case 41: /* assignment: IDENT SUB_ASSIGN expression  */
#line 364 "k0gram.y"
        {
            (yyval.tree) = tree_node("assignment", 3, 3,
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1710 "k0gram.tab.c"
    break;

  case 42: /* assignment: IDENT MUL_ASSIGN expression  */
#line 369 "k0gram.y"
        {
            (yyval.tree) = tree_node("assignment", 4, 3,
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1719 "k0gram.tab.c"
    break;

  case 43: /* assignment: IDENT DIV_ASSIGN expression  */
#line 374 "k0gram.y"
        {
            (yyval.tree) = tree_node("assignment", 5, 3,
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1728 "k0gram.tab.c"
    break;

  case 44: /* assignment: IDENT MOD_ASSIGN expression  */
#line 379 "k0gram.y"
        {
            (yyval.tree) = tree_node("assignment", 6, 3,
                           tree_leaf((yyvsp[-2].tok)), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1737 "k0gram.tab.c"
    break;

  case 45: /* ifStatement: IF LPAREN expression RPAREN controlStructureBody  */
#line 389 "k0gram.y"
        {
            (yyval.tree) = tree_node("ifStatement", 1, 5,
                           tree_leaf((yyvsp[-4].tok)), tree_leaf((yyvsp[-3].tok)),
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1747 "k0gram.tab.c"
    break;

  case 46: /* ifStatement: IF LPAREN expression RPAREN controlStructureBody ELSE controlStructureBody  */
#line 395 "k0gram.y"
        {
            (yyval.tree) = tree_node("ifStatement", 2, 7,
                           tree_leaf((yyvsp[-6].tok)), tree_leaf((yyvsp[-5].tok)),
                           (yyvsp[-4].tree), tree_leaf((yyvsp[-3].tok)),
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1758 "k0gram.tab.c"
    break;

  case 47: /* whileStatement: WHILE LPAREN expression RPAREN controlStructureBody  */
#line 405 "k0gram.y"
        {
            (yyval.tree) = tree_node("whileStatement", 1, 5,
                           tree_leaf((yyvsp[-4].tok)), tree_leaf((yyvsp[-3].tok)),
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1768 "k0gram.tab.c"
    break;

  case 48: /* forStatement: FOR LPAREN IDENT IN expression RPAREN controlStructureBody  */
#line 414 "k0gram.y"
        {
            (yyval.tree) = tree_node("forStatement", 1, 7,
                           tree_leaf((yyvsp[-6].tok)), tree_leaf((yyvsp[-5].tok)),
                           tree_leaf((yyvsp[-4].tok)), tree_leaf((yyvsp[-3].tok)),
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1779 "k0gram.tab.c"
    break;

  case 49: /* jumpStatement: RETURN  */
#line 424 "k0gram.y"
        { (yyval.tree) = tree_node("jumpStatement", 1, 1, tree_leaf((yyvsp[0].tok))); }
#line 1785 "k0gram.tab.c"
    break;

  case 50: /* jumpStatement: RETURN expression  */
#line 426 "k0gram.y"
        { (yyval.tree) = tree_node("jumpStatement", 2, 2, tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1791 "k0gram.tab.c"
    break;

  case 51: /* jumpStatement: BREAK  */
#line 428 "k0gram.y"
        { (yyval.tree) = tree_node("jumpStatement", 3, 1, tree_leaf((yyvsp[0].tok))); }
#line 1797 "k0gram.tab.c"
    break;

  case 52: /* jumpStatement: CONTINUE  */
#line 430 "k0gram.y"
        { (yyval.tree) = tree_node("jumpStatement", 4, 1, tree_leaf((yyvsp[0].tok))); }
#line 1803 "k0gram.tab.c"
    break;

  case 53: /* controlStructureBody: block  */
#line 434 "k0gram.y"
                { (yyval.tree) = (yyvsp[0].tree); }
#line 1809 "k0gram.tab.c"
    break;

  case 54: /* controlStructureBody: statement  */
#line 435 "k0gram.y"
                { (yyval.tree) = (yyvsp[0].tree); }
#line 1815 "k0gram.tab.c"
    break;

  case 55: /* expression: literal  */
#line 442 "k0gram.y"
        { (yyval.tree) = (yyvsp[0].tree); }
#line 1821 "k0gram.tab.c"
    break;

  case 56: /* expression: IDENT  */
#line 444 "k0gram.y"
        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 1827 "k0gram.tab.c"
    break;

  case 57: /* expression: expression LPAREN RPAREN  */
#line 446 "k0gram.y"
        {
            (yyval.tree) = tree_node("callExpr", 1, 3,
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), tree_leaf((yyvsp[0].tok)));
        }
#line 1836 "k0gram.tab.c"
    break;

  case 58: /* expression: expression LPAREN argumentList RPAREN  */
#line 451 "k0gram.y"
        {
            (yyval.tree) = tree_node("callExpr", 2, 4,
                           (yyvsp[-3].tree), tree_leaf((yyvsp[-2].tok)), (yyvsp[-1].tree), tree_leaf((yyvsp[0].tok)));
        }
#line 1845 "k0gram.tab.c"
    break;

  case 59: /* expression: expression LSQUARE expression RSQUARE  */
#line 456 "k0gram.y"
        {
            (yyval.tree) = tree_node("indexExpr", 1, 4,
                           (yyvsp[-3].tree), tree_leaf((yyvsp[-2].tok)), (yyvsp[-1].tree), tree_leaf((yyvsp[0].tok)));
        }
#line 1854 "k0gram.tab.c"
    break;

  case 60: /* expression: expression DOT IDENT  */
#line 461 "k0gram.y"
        {
            (yyval.tree) = tree_node("memberExpr", 1, 3,
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), tree_leaf((yyvsp[0].tok)));
        }
#line 1863 "k0gram.tab.c"
    break;

  case 61: /* expression: expression SAFE_CALL IDENT  */
#line 466 "k0gram.y"
        {
            (yyval.tree) = tree_node("safeMemberExpr", 1, 3,
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), tree_leaf((yyvsp[0].tok)));
        }
#line 1872 "k0gram.tab.c"
    break;

  case 62: /* expression: expression AS type  */
#line 471 "k0gram.y"
        {
            (yyval.tree) = tree_node("castExpr", 1, 3,
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 1881 "k0gram.tab.c"
    break;

  case 63: /* expression: expression ADD expression  */
#line 476 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 1,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1887 "k0gram.tab.c"
    break;

  case 64: /* expression: expression SUB expression  */
#line 478 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 2,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1893 "k0gram.tab.c"
    break;

  case 65: /* expression: expression MUL expression  */
#line 480 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 3,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1899 "k0gram.tab.c"
    break;

  case 66: /* expression: expression DIV expression  */
#line 482 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 4,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1905 "k0gram.tab.c"
    break;

  case 67: /* expression: expression MOD expression  */
#line 484 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 5,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1911 "k0gram.tab.c"
    break;

  case 68: /* expression: SUB expression  */
#line 486 "k0gram.y"
        { (yyval.tree) = tree_node("unaryExpr",  1,  2, tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1917 "k0gram.tab.c"
    break;

  case 69: /* expression: NOT expression  */
#line 488 "k0gram.y"
        { (yyval.tree) = tree_node("unaryExpr",  2,  2, tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1923 "k0gram.tab.c"
    break;

  case 70: /* expression: expression INCR  */
#line 490 "k0gram.y"
        { (yyval.tree) = tree_node("postfixExpr", 1, 2, (yyvsp[-1].tree), tree_leaf((yyvsp[0].tok))); }
#line 1929 "k0gram.tab.c"
    break;

  case 71: /* expression: expression DECR  */
#line 492 "k0gram.y"
        { (yyval.tree) = tree_node("postfixExpr", 2, 2, (yyvsp[-1].tree), tree_leaf((yyvsp[0].tok))); }
#line 1935 "k0gram.tab.c"
    break;

  case 72: /* expression: expression EQ expression  */
#line 494 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 6,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1941 "k0gram.tab.c"
    break;

  case 73: /* expression: expression NEQ expression  */
#line 496 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 7,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1947 "k0gram.tab.c"
    break;

  case 74: /* expression: expression LANGLE expression  */
#line 498 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 8,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1953 "k0gram.tab.c"
    break;

  case 75: /* expression: expression RANGLE expression  */
#line 500 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 9,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1959 "k0gram.tab.c"
    break;

  case 76: /* expression: expression LTE expression  */
#line 502 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 10, 3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1965 "k0gram.tab.c"
    break;

  case 77: /* expression: expression GTE expression  */
#line 504 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 11, 3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1971 "k0gram.tab.c"
    break;

  case 78: /* expression: expression REF_EQ expression  */
#line 506 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 12, 3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1977 "k0gram.tab.c"
    break;

  case 79: /* expression: expression REF_NEQ expression  */
#line 508 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 13, 3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1983 "k0gram.tab.c"
    break;

  case 80: /* expression: expression AND expression  */
#line 510 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 14, 3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1989 "k0gram.tab.c"
    break;

  case 81: /* expression: expression OR expression  */
#line 512 "k0gram.y"
        { (yyval.tree) = tree_node("binaryExpr", 15, 3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 1995 "k0gram.tab.c"
    break;

  case 82: /* expression: expression RANGE expression  */
#line 514 "k0gram.y"
        { (yyval.tree) = tree_node("rangeExpr",  1,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 2001 "k0gram.tab.c"
    break;

  case 83: /* expression: expression RANGE_UNTIL expression  */
#line 516 "k0gram.y"
        { (yyval.tree) = tree_node("rangeExpr",  2,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 2007 "k0gram.tab.c"
    break;

  case 84: /* expression: expression ELVIS expression  */
#line 518 "k0gram.y"
        { (yyval.tree) = tree_node("elvisExpr",  1,  3, (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree)); }
#line 2013 "k0gram.tab.c"
    break;

  case 85: /* expression: LPAREN expression RPAREN  */
#line 520 "k0gram.y"
        { (yyval.tree) = (yyvsp[-1].tree); }
#line 2019 "k0gram.tab.c"
    break;

  case 86: /* argumentList: expression  */
#line 525 "k0gram.y"
        { (yyval.tree) = (yyvsp[0].tree); }
#line 2025 "k0gram.tab.c"
    break;

  case 87: /* argumentList: argumentList COMMA expression  */
#line 527 "k0gram.y"
        {
            (yyval.tree) = tree_node("argumentList", 1, 3,
                           (yyvsp[-2].tree), tree_leaf((yyvsp[-1].tok)), (yyvsp[0].tree));
        }
#line 2034 "k0gram.tab.c"
    break;

  case 88: /* literal: INTEGERLITERAL  */
#line 536 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2040 "k0gram.tab.c"
    break;

  case 89: /* literal: LONGLITERAL  */
#line 537 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2046 "k0gram.tab.c"
    break;

  case 90: /* literal: REALLITERAL  */
#line 538 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2052 "k0gram.tab.c"
    break;

  case 91: /* literal: DOUBLELITERAL  */
#line 539 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2058 "k0gram.tab.c"
    break;

  case 92: /* literal: BOOLEANLITERAL  */
#line 540 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2064 "k0gram.tab.c"
    break;

  case 93: /* literal: CHARACTERLITERAL  */
#line 541 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2070 "k0gram.tab.c"
    break;

  case 94: /* literal: STRINGLITERAL  */
#line 542 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2076 "k0gram.tab.c"
    break;

  case 95: /* literal: NULLLITERAL  */
#line 543 "k0gram.y"
                        { (yyval.tree) = tree_leaf((yyvsp[0].tok)); }
#line 2082 "k0gram.tab.c"
    break;


#line 2086 "k0gram.tab.c"

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
                      yytoken, &yylval);
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


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 546 "k0gram.y"


void yyerror(const char *s) {
    g_syntax_errors++;
    fprintf(stderr, "%s:%d: syntax error: %s\n",
            current_filename ? current_filename : "<stdin>",
            lineno, s);
}

const char *yyname(int sym) {
    /* yytname starts at index 3 for user tokens: 0=$end,1=error,2=$undefined */
    /* tokens start at 258; FUN is the first %token (index 3 in yytname)      */
    int idx = sym - FUN + 3;
    if (idx < 0 || idx >= (int)(sizeof(yytname)/sizeof(yytname[0])))
        return "UNKNOWN";
    return yytname[idx];
}
