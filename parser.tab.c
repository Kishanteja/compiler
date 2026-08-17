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
#line 6 "parser.y"

#include "symtab.hpp"
#include "tac.hpp"

/* ── global definitions (one translation unit only) ── */
ASTNode    *root            = NULL;
Symbol      sym_table[MAX_SYMS];
int         sym_count       = 0;
Symbol      local_table[MAX_SYMS];
int         local_count     = 0;
int         in_function     = 0;
const char *current_decl_type = "int";

/* ── function table globals ── */
FuncSymbol  func_table[MAX_FUNCS];
int         func_count      = 0;
const char *current_function = NULL;
static char saved_func_name[64];
static int  params_already_set = 0;
static int  param_check_idx    = 0;
static int  has_return_stmt    = 0;

int yylex(void);
int yyerror(const char *s);

#line 97 "parser.tab.c"

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

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INTEGER = 3,                    /* INTEGER  */
  YYSYMBOL_FLOAT = 4,                      /* FLOAT  */
  YYSYMBOL_STRING = 5,                     /* STRING  */
  YYSYMBOL_VOID = 6,                       /* VOID  */
  YYSYMBOL_BOOL = 7,                       /* BOOL  */
  YYSYMBOL_NAME = 8,                       /* NAME  */
  YYSYMBOL_INT_NUM = 9,                    /* INT_NUM  */
  YYSYMBOL_FLOAT_NUM = 10,                 /* FLOAT_NUM  */
  YYSYMBOL_STR_CONST = 11,                 /* STR_CONST  */
  YYSYMBOL_BOOL_CONST = 12,                /* BOOL_CONST  */
  YYSYMBOL_READ = 13,                      /* READ  */
  YYSYMBOL_WRITE = 14,                     /* WRITE  */
  YYSYMBOL_RETURN = 15,                    /* RETURN  */
  YYSYMBOL_ASSIGN_OP = 16,                 /* ASSIGN_OP  */
  YYSYMBOL_PLUS = 17,                      /* PLUS  */
  YYSYMBOL_MINUS = 18,                     /* MINUS  */
  YYSYMBOL_MUL = 19,                       /* MUL  */
  YYSYMBOL_DIV = 20,                       /* DIV  */
  YYSYMBOL_OR = 21,                        /* OR  */
  YYSYMBOL_AND = 22,                       /* AND  */
  YYSYMBOL_NOT = 23,                       /* NOT  */
  YYSYMBOL_EQUAL = 24,                     /* EQUAL  */
  YYSYMBOL_NOT_EQUAL = 25,                 /* NOT_EQUAL  */
  YYSYMBOL_GREATER = 26,                   /* GREATER  */
  YYSYMBOL_GREATER_EQUAL = 27,             /* GREATER_EQUAL  */
  YYSYMBOL_LESS = 28,                      /* LESS  */
  YYSYMBOL_LESS_EQUAL = 29,                /* LESS_EQUAL  */
  YYSYMBOL_QUESTION_MARK = 30,             /* QUESTION_MARK  */
  YYSYMBOL_COLON = 31,                     /* COLON  */
  YYSYMBOL_SEMICOLON = 32,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 33,                     /* COMMA  */
  YYSYMBOL_LEFT_ROUND_BRACKET = 34,        /* LEFT_ROUND_BRACKET  */
  YYSYMBOL_RIGHT_ROUND_BRACKET = 35,       /* RIGHT_ROUND_BRACKET  */
  YYSYMBOL_LEFT_CURLY_BRACKET = 36,        /* LEFT_CURLY_BRACKET  */
  YYSYMBOL_RIGHT_CURLY_BRACKET = 37,       /* RIGHT_CURLY_BRACKET  */
  YYSYMBOL_IF = 38,                        /* IF  */
  YYSYMBOL_ELSE = 39,                      /* ELSE  */
  YYSYMBOL_WHILE = 40,                     /* WHILE  */
  YYSYMBOL_DO = 41,                        /* DO  */
  YYSYMBOL_UMINUS = 42,                    /* UMINUS  */
  YYSYMBOL_LOWER_THAN_ELSE = 43,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_YYACCEPT = 44,                  /* $accept  */
  YYSYMBOL_program = 45,                   /* program  */
  YYSYMBOL_global_decl_statement_list = 46, /* global_decl_statement_list  */
  YYSYMBOL_func_def_list = 47,             /* func_def_list  */
  YYSYMBOL_func_header = 48,               /* func_header  */
  YYSYMBOL_func_decl = 49,                 /* func_decl  */
  YYSYMBOL_func_def = 50,                  /* func_def  */
  YYSYMBOL_formal_param_list = 51,         /* formal_param_list  */
  YYSYMBOL_formal_param = 52,              /* formal_param  */
  YYSYMBOL_param_type = 53,                /* param_type  */
  YYSYMBOL_var_decl_stmt = 54,             /* var_decl_stmt  */
  YYSYMBOL_var_decl_item_list = 55,        /* var_decl_item_list  */
  YYSYMBOL_named_type = 56,                /* named_type  */
  YYSYMBOL_compound_statement = 57,        /* compound_statement  */
  YYSYMBOL_block_items = 58,               /* block_items  */
  YYSYMBOL_block_decls = 59,               /* block_decls  */
  YYSYMBOL_block_stmts = 60,               /* block_stmts  */
  YYSYMBOL_stmt_block = 61,                /* stmt_block  */
  YYSYMBOL_statement = 62,                 /* statement  */
  YYSYMBOL_func_call = 63,                 /* func_call  */
  YYSYMBOL_actual_arg_list = 64,           /* actual_arg_list  */
  YYSYMBOL_non_empty_arg_list = 65,        /* non_empty_arg_list  */
  YYSYMBOL_call_statement = 66,            /* call_statement  */
  YYSYMBOL_return_statement = 67,          /* return_statement  */
  YYSYMBOL_read_statement = 68,            /* read_statement  */
  YYSYMBOL_print_statement = 69,           /* print_statement  */
  YYSYMBOL_assignment_statement = 70,      /* assignment_statement  */
  YYSYMBOL_if_statement = 71,              /* if_statement  */
  YYSYMBOL_while_statement = 72,           /* while_statement  */
  YYSYMBOL_do_while_statement = 73,        /* do_while_statement  */
  YYSYMBOL_expression = 74,                /* expression  */
  YYSYMBOL_conditional_expression = 75,    /* conditional_expression  */
  YYSYMBOL_logical_or_expression = 76,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 77,    /* logical_and_expression  */
  YYSYMBOL_not_expression = 78,            /* not_expression  */
  YYSYMBOL_equality_expression = 79,       /* equality_expression  */
  YYSYMBOL_relational_expression = 80,     /* relational_expression  */
  YYSYMBOL_additive_expression = 81,       /* additive_expression  */
  YYSYMBOL_term = 82,                      /* term  */
  YYSYMBOL_factor = 83                     /* factor  */
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
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   200

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  165

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   298


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
      35,    36,    37,    38,    39,    40,    41,    42,    43
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    97,    97,   103,   113,   114,   115,   116,   121,   122,
     131,   151,   157,   170,   184,   204,   205,   209,   233,   234,
     235,   236,   243,   247,   249,   254,   255,   256,   257,   258,
     265,   269,   270,   271,   272,   276,   277,   281,   282,   286,
     287,   294,   295,   296,   297,   298,   299,   300,   301,   302,
     310,   351,   352,   356,   357,   362,   384,   407,   425,   440,
     455,   478,   487,   499,   511,   526,   530,   531,   546,   554,
     558,   566,   570,   578,   582,   591,   600,   604,   613,   622,
     631,   640,   644,   650,   656,   660,   666,   672,   684,   686,
     692,   697,   702,   707,   712,   714
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
  "\"end of file\"", "error", "\"invalid token\"", "INTEGER", "FLOAT",
  "STRING", "VOID", "BOOL", "NAME", "INT_NUM", "FLOAT_NUM", "STR_CONST",
  "BOOL_CONST", "READ", "WRITE", "RETURN", "ASSIGN_OP", "PLUS", "MINUS",
  "MUL", "DIV", "OR", "AND", "NOT", "EQUAL", "NOT_EQUAL", "GREATER",
  "GREATER_EQUAL", "LESS", "LESS_EQUAL", "QUESTION_MARK", "COLON",
  "SEMICOLON", "COMMA", "LEFT_ROUND_BRACKET", "RIGHT_ROUND_BRACKET",
  "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", "IF", "ELSE", "WHILE", "DO",
  "UMINUS", "LOWER_THAN_ELSE", "$accept", "program",
  "global_decl_statement_list", "func_def_list", "func_header",
  "func_decl", "func_def", "formal_param_list", "formal_param",
  "param_type", "var_decl_stmt", "var_decl_item_list", "named_type",
  "compound_statement", "block_items", "block_decls", "block_stmts",
  "stmt_block", "statement", "func_call", "actual_arg_list",
  "non_empty_arg_list", "call_statement", "return_statement",
  "read_statement", "print_statement", "assignment_statement",
  "if_statement", "while_statement", "do_while_statement", "expression",
  "conditional_expression", "logical_or_expression",
  "logical_and_expression", "not_expression", "equality_expression",
  "relational_expression", "additive_expression", "term", "factor", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-79)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-11)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     129,   -79,   -79,   -79,   -79,   -79,    73,   129,   129,   -22,
     -79,   -79,   -79,    68,   -79,   129,   -79,   -79,    -6,   -79,
      71,    28,    51,    93,    57,   -79,   -79,   -79,   -79,   -79,
       5,     3,   -79,    94,   -79,    98,    76,    32,   -79,    83,
     -79,    20,    63,   -79,   -79,    76,    -8,   107,    60,   119,
      67,    86,   104,     7,   -79,   142,   115,    83,     7,   -79,
     -79,   122,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,
     -79,   -79,   -79,    60,    60,   136,   143,   -79,   -79,   -79,
     -79,   137,    60,    60,   -79,   144,   -79,   -17,   156,   -79,
     140,   130,   149,   150,   -79,   -79,   147,   -79,   103,    60,
      60,   141,   -79,   -79,   -79,     7,   -79,   -79,   148,   151,
     152,   -79,   -79,   -79,   -79,   153,   -79,    60,    60,    60,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     -79,   -79,   154,   155,   157,   -79,   -79,    60,   -79,   156,
     161,   -79,   130,   130,   149,   149,   149,   149,   150,   150,
     -79,   -79,     7,     7,    60,   -79,    60,   145,   -79,   158,
     -79,     7,   162,   -79,   -79
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    25,    26,    27,    29,    28,     0,     0,     3,     0,
       7,     9,     6,     0,     1,     2,     4,     5,     0,     8,
       0,     0,    24,     0,     0,    10,    18,    19,    20,    21,
       0,     0,    16,     0,    22,     0,     0,     0,    12,    34,
      14,     0,     0,    17,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,    32,    33,    49,
      38,     0,    48,    47,    42,    43,    41,    44,    45,    46,
      15,    11,    13,     0,    52,     0,    89,    90,    91,    92,
      93,     0,     0,     0,    88,     0,    65,    66,    69,    71,
      73,    76,    81,    84,    87,    57,     0,    40,     0,     0,
       0,     0,    24,    30,    35,    31,    37,    55,     0,     0,
      51,    54,    58,    95,    72,     0,    59,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      56,    39,     0,     0,     0,    60,    50,     0,    94,    68,
       0,    70,    74,    75,    78,    79,    77,    80,    82,    83,
      85,    86,     0,     0,     0,    53,     0,    61,    63,     0,
      67,     0,     0,    62,    64
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -79,   -79,   -79,   175,     2,   176,    34,   163,   159,   -79,
      -5,   -79,     1,    12,   -79,   -79,    27,   -79,   -52,   -39,
     -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,
     -44,    39,   -79,    79,   -75,   -79,    52,    38,    48,   -78
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     6,     7,     8,     9,    10,    11,    31,    32,    33,
      12,    23,    13,    40,    56,    57,    58,    59,    60,    84,
     109,   110,    62,    63,    64,    65,    66,    67,    68,    69,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      61,   101,    17,   113,   117,    96,   106,   114,    73,    20,
      18,    61,    21,   118,    61,    46,    20,    18,    61,    61,
      47,    48,    49,    26,    27,    28,    74,    29,    24,   108,
     111,    26,    27,    28,    54,    29,    41,    38,    42,   115,
      55,    39,    19,    50,   141,    51,   106,    52,    53,    19,
     150,   151,   104,   106,    72,   132,   133,    72,    55,    61,
      26,    27,    28,    30,    29,    41,    61,    45,    76,    77,
      78,    79,    80,    14,   140,    46,    22,    98,    81,    25,
      47,    48,    49,    82,   105,   -10,     1,     2,     3,     4,
       5,    46,    36,   155,    83,    71,    47,    48,    49,    39,
     157,   158,    43,    50,    97,    51,    44,    52,    53,   163,
     159,    46,    39,    61,    61,    75,    47,    48,    49,    50,
      99,    51,    61,    52,    53,    34,    35,    76,    77,    78,
      79,    80,     1,     2,     3,     4,     5,    81,   100,    50,
     131,    51,    82,    52,    53,    76,    77,    78,    79,    80,
     102,    95,   103,    83,   107,    81,   122,   123,   124,   125,
     144,   145,   146,   147,   120,   121,   126,   127,   112,   128,
     129,    83,   142,   143,   148,   149,   116,    74,   119,   130,
     135,   134,    15,    16,   161,   137,   136,    37,   138,   152,
     153,   154,   156,   162,   164,   160,   139,     0,     0,     0,
      70
};

static const yytype_int16 yycheck[] =
{
      39,    53,     7,    81,    21,    49,    58,    82,    16,     8,
       8,    50,    34,    30,    53,     8,    15,    15,    57,    58,
      13,    14,    15,     3,     4,     5,    34,     7,    34,    73,
      74,     3,     4,     5,    39,     7,    33,    32,    35,    83,
      39,    36,     8,    36,   119,    38,    98,    40,    41,    15,
     128,   129,    57,   105,    42,    99,   100,    45,    57,    98,
       3,     4,     5,    35,     7,    33,   105,    35,     8,     9,
      10,    11,    12,     0,   118,     8,     8,    50,    18,     8,
      13,    14,    15,    23,    57,    34,     3,     4,     5,     6,
       7,     8,    35,   137,    34,    32,    13,    14,    15,    36,
     152,   153,     8,    36,    37,    38,     8,    40,    41,   161,
     154,     8,    36,   152,   153,     8,    13,    14,    15,    36,
      34,    38,   161,    40,    41,    32,    33,     8,     9,    10,
      11,    12,     3,     4,     5,     6,     7,    18,    34,    36,
      37,    38,    23,    40,    41,     8,     9,    10,    11,    12,
       8,    32,    37,    34,    32,    18,    26,    27,    28,    29,
     122,   123,   124,   125,    24,    25,    17,    18,    32,    19,
      20,    34,   120,   121,   126,   127,    32,    34,    22,    32,
      32,    40,     7,     7,    39,    33,    35,    24,    35,    35,
      35,    34,    31,    35,    32,   156,   117,    -1,    -1,    -1,
      41
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,    45,    46,    47,    48,
      49,    50,    54,    56,     0,    47,    49,    54,    48,    50,
      56,    34,     8,    55,    34,     8,     3,     4,     5,     7,
      35,    51,    52,    53,    32,    33,    35,    51,    32,    36,
      57,    33,    35,     8,     8,    35,     8,    13,    14,    15,
      36,    38,    40,    41,    54,    56,    58,    59,    60,    61,
      62,    63,    66,    67,    68,    69,    70,    71,    72,    73,
      52,    32,    57,    16,    34,     8,     8,     9,    10,    11,
      12,    18,    23,    34,    63,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    32,    74,    37,    60,    34,
      34,    62,     8,    37,    54,    60,    62,    32,    74,    64,
      65,    74,    32,    83,    78,    74,    32,    21,    30,    22,
      24,    25,    26,    27,    28,    29,    17,    18,    19,    20,
      32,    37,    74,    74,    40,    32,    35,    33,    35,    77,
      74,    78,    80,    80,    81,    81,    81,    81,    82,    82,
      83,    83,    35,    35,    34,    74,    31,    62,    62,    74,
      75,    39,    35,    62,    32
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    44,    45,    45,    46,    46,    46,    46,    47,    47,
      48,    49,    49,    50,    50,    51,    51,    52,    53,    53,
      53,    53,    54,    55,    55,    56,    56,    56,    56,    56,
      57,    58,    58,    58,    58,    59,    59,    60,    60,    61,
      61,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      63,    64,    64,    65,    65,    66,    67,    67,    68,    69,
      70,    71,    71,    72,    73,    74,    75,    75,    76,    76,
      77,    77,    78,    78,    79,    79,    79,    80,    80,    80,
      80,    80,    81,    81,    81,    82,    82,    82,    83,    83,
      83,    83,    83,    83,    83,    83
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     2,     2,     1,     1,     2,     1,
       2,     5,     4,     5,     4,     3,     1,     2,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       3,     2,     1,     1,     0,     2,     1,     2,     1,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     1,     0,     3,     1,     2,     3,     2,     3,     3,
       4,     5,     7,     5,     7,     1,     1,     5,     3,     1,
       3,     1,     2,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     3,     2
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
  case 2: /* program: global_decl_statement_list func_def_list  */
#line 98 "parser.y"
      {
          int mi = func_index("main");
          root = (mi >= 0) ? func_table[mi].body : NULL;
          (yyval.node) = (yyvsp[0].node);
      }
#line 1299 "parser.tab.c"
    break;

  case 3: /* program: func_def_list  */
#line 104 "parser.y"
      {
          int mi = func_index("main");
          root = (mi >= 0) ? func_table[mi].body : NULL;
          (yyval.node) = (yyvsp[0].node);
      }
#line 1309 "parser.tab.c"
    break;

  case 8: /* func_def_list: func_def_list func_def  */
#line 121 "parser.y"
                              { (yyval.node) = NULL; }
#line 1315 "parser.tab.c"
    break;

  case 9: /* func_def_list: func_def  */
#line 122 "parser.y"
                              { (yyval.node) = NULL; }
#line 1321 "parser.tab.c"
    break;

  case 10: /* func_header: named_type NAME  */
#line 132 "parser.y"
      {
          params_already_set = (func_lookup((yyvsp[0].str)) != NULL);
          func_insert((yyvsp[0].str), current_decl_type);
          strcpy(saved_func_name, (yyvsp[0].str));
          current_function = saved_func_name;
          in_function  = 1;
          local_count  = 0;
          param_check_idx = 0;
          has_return_stmt = 0;
          free((yyvsp[0].str));
          (yyval.node) = NULL;
      }
#line 1338 "parser.tab.c"
    break;

  case 11: /* func_decl: func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET SEMICOLON  */
#line 152 "parser.y"
      {
          in_function      = 0;
          current_function = NULL;
          local_count      = 0;
      }
#line 1348 "parser.tab.c"
    break;

  case 12: /* func_decl: func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON  */
#line 158 "parser.y"
      {
          in_function      = 0;
          current_function = NULL;
          local_count      = 0;
      }
#line 1358 "parser.tab.c"
    break;

  case 13: /* func_def: func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET compound_statement  */
#line 171 "parser.y"
      {
          const char *ret = func_lookup(saved_func_name);
          if (ret && strcmp(ret, "void") != 0 && !has_return_stmt) {
              fprintf(stderr,
                  "Error: non-void function '%s' must have a return statement\n",
                  saved_func_name);
              exit(1);
          }
          func_set_body(saved_func_name, (yyvsp[0].node));
          in_function      = 0;
          current_function = NULL;
          (yyval.node) = (yyvsp[0].node);
      }
#line 1376 "parser.tab.c"
    break;

  case 14: /* func_def: func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET compound_statement  */
#line 185 "parser.y"
      {
          const char *ret = func_lookup(saved_func_name);
          if (ret && strcmp(ret, "void") != 0 && !has_return_stmt) {
              fprintf(stderr,
                  "Error: non-void function '%s' must have a return statement\n",
                  saved_func_name);
              exit(1);
          }
          func_set_body(saved_func_name, (yyvsp[0].node));
          in_function      = 0;
          current_function = NULL;
          (yyval.node) = (yyvsp[0].node);
      }
#line 1394 "parser.tab.c"
    break;

  case 17: /* formal_param: param_type NAME  */
#line 210 "parser.y"
      {
          /* always add to local scope (for function body) */
          sym_insert((yyvsp[0].str), current_decl_type);
          /* if a prior declaration exists, cross-check param types */
          if (params_already_set) {
              const char *decl_type = func_param_type(saved_func_name, param_check_idx);
              if (decl_type && strcmp(decl_type, current_decl_type) != 0) {
                  fprintf(stderr,
                      "Error: parameter %d of '%s' declared as <%s> but defined as <%s>\n",
                      param_check_idx + 1, saved_func_name, decl_type, current_decl_type);
                  exit(1);
              }
              func_update_param_name(saved_func_name, param_check_idx, (yyvsp[0].str));
          } else {
              func_add_param(saved_func_name, (yyvsp[0].str), current_decl_type);
          }
          param_check_idx++;
          free((yyvsp[0].str));
      }
#line 1418 "parser.tab.c"
    break;

  case 18: /* param_type: INTEGER  */
#line 233 "parser.y"
               { current_decl_type = "int";    }
#line 1424 "parser.tab.c"
    break;

  case 19: /* param_type: FLOAT  */
#line 234 "parser.y"
               { current_decl_type = "float";  }
#line 1430 "parser.tab.c"
    break;

  case 20: /* param_type: STRING  */
#line 235 "parser.y"
               { current_decl_type = "string"; }
#line 1436 "parser.tab.c"
    break;

  case 21: /* param_type: BOOL  */
#line 236 "parser.y"
               { current_decl_type = "bool";   }
#line 1442 "parser.tab.c"
    break;

  case 23: /* var_decl_item_list: var_decl_item_list COMMA NAME  */
#line 248 "parser.y"
      { sym_insert((yyvsp[0].str), current_decl_type); free((yyvsp[0].str)); }
#line 1448 "parser.tab.c"
    break;

  case 24: /* var_decl_item_list: NAME  */
#line 250 "parser.y"
      { sym_insert((yyvsp[0].str), current_decl_type); free((yyvsp[0].str)); }
#line 1454 "parser.tab.c"
    break;

  case 25: /* named_type: INTEGER  */
#line 254 "parser.y"
               { current_decl_type = "int";    }
#line 1460 "parser.tab.c"
    break;

  case 26: /* named_type: FLOAT  */
#line 255 "parser.y"
               { current_decl_type = "float";  }
#line 1466 "parser.tab.c"
    break;

  case 27: /* named_type: STRING  */
#line 256 "parser.y"
               { current_decl_type = "string"; }
#line 1472 "parser.tab.c"
    break;

  case 28: /* named_type: BOOL  */
#line 257 "parser.y"
               { current_decl_type = "bool";   }
#line 1478 "parser.tab.c"
    break;

  case 29: /* named_type: VOID  */
#line 258 "parser.y"
               { current_decl_type = "void";   }
#line 1484 "parser.tab.c"
    break;

  case 30: /* compound_statement: LEFT_CURLY_BRACKET block_items RIGHT_CURLY_BRACKET  */
#line 265 "parser.y"
                                                          { (yyval.node) = (yyvsp[-1].node); }
#line 1490 "parser.tab.c"
    break;

  case 31: /* block_items: block_decls block_stmts  */
#line 269 "parser.y"
                               { (yyval.node) = (yyvsp[0].node); }
#line 1496 "parser.tab.c"
    break;

  case 32: /* block_items: block_decls  */
#line 270 "parser.y"
                               { (yyval.node) = NULL; }
#line 1502 "parser.tab.c"
    break;

  case 33: /* block_items: block_stmts  */
#line 271 "parser.y"
                               { (yyval.node) = (yyvsp[0].node); }
#line 1508 "parser.tab.c"
    break;

  case 34: /* block_items: %empty  */
#line 272 "parser.y"
                               { (yyval.node) = NULL; }
#line 1514 "parser.tab.c"
    break;

  case 37: /* block_stmts: block_stmts statement  */
#line 281 "parser.y"
                             { (yyval.node) = connect_nodes((yyvsp[-1].node), (yyvsp[0].node)); }
#line 1520 "parser.tab.c"
    break;

  case 38: /* block_stmts: statement  */
#line 282 "parser.y"
                             { (yyval.node) = (yyvsp[0].node); }
#line 1526 "parser.tab.c"
    break;

  case 39: /* stmt_block: LEFT_CURLY_BRACKET block_stmts RIGHT_CURLY_BRACKET  */
#line 286 "parser.y"
                                                          { (yyval.node) = (yyvsp[-1].node); }
#line 1532 "parser.tab.c"
    break;

  case 40: /* stmt_block: LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET  */
#line 287 "parser.y"
                                                          { (yyval.node) = NULL; }
#line 1538 "parser.tab.c"
    break;

  case 41: /* statement: assignment_statement  */
#line 294 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1544 "parser.tab.c"
    break;

  case 42: /* statement: read_statement  */
#line 295 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1550 "parser.tab.c"
    break;

  case 43: /* statement: print_statement  */
#line 296 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1556 "parser.tab.c"
    break;

  case 44: /* statement: if_statement  */
#line 297 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1562 "parser.tab.c"
    break;

  case 45: /* statement: while_statement  */
#line 298 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1568 "parser.tab.c"
    break;

  case 46: /* statement: do_while_statement  */
#line 299 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1574 "parser.tab.c"
    break;

  case 47: /* statement: return_statement  */
#line 300 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1580 "parser.tab.c"
    break;

  case 48: /* statement: call_statement  */
#line 301 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1586 "parser.tab.c"
    break;

  case 49: /* statement: stmt_block  */
#line 302 "parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1592 "parser.tab.c"
    break;

  case 50: /* func_call: NAME LEFT_ROUND_BRACKET actual_arg_list RIGHT_ROUND_BRACKET  */
#line 311 "parser.y"
      {
          const char *ret = func_lookup((yyvsp[-3].str));
          if (!ret) {
              fprintf(stderr, "Error: call to undeclared function '%s'\n", (yyvsp[-3].str));
              exit(1);
          }
          /* check argument count */
          int expected = func_param_count((yyvsp[-3].str));
          int actual = 0;
          ASTNode *a = (yyvsp[-1].node);
          while (a) { actual++; a = a->next; }
          if (actual != expected) {
              fprintf(stderr,
                  "Error: '%s' expects %d args, got %d\n",
                  (yyvsp[-3].str), expected, actual);
              exit(1);
          }
          /* check argument types */
          a = (yyvsp[-1].node);
          for (int i = 0; i < expected; i++) {
              const char *pt = func_param_type((yyvsp[-3].str), i);
              const char *at = node_type(a);
              if (strcmp(pt, at) != 0) {
                  fprintf(stderr,
                      "Type error: arg %d of '%s' expects <%s>, got <%s>\n",
                      i+1, (yyvsp[-3].str), pt, at);
                  exit(1);
              }
              a = a->next;
          }
          char buf[128];
          sprintf(buf, "FuncCall: %s<%s>", (yyvsp[-3].str), ret);
          ASTNode *n = create_node(buf);
          n->child1 = (yyvsp[-1].node);
          free((yyvsp[-3].str));
          (yyval.node) = n;
      }
#line 1634 "parser.tab.c"
    break;

  case 51: /* actual_arg_list: non_empty_arg_list  */
#line 351 "parser.y"
                          { (yyval.node) = (yyvsp[0].node);   }
#line 1640 "parser.tab.c"
    break;

  case 52: /* actual_arg_list: %empty  */
#line 352 "parser.y"
                          { (yyval.node) = NULL; }
#line 1646 "parser.tab.c"
    break;

  case 53: /* non_empty_arg_list: non_empty_arg_list COMMA expression  */
#line 356 "parser.y"
                                           { (yyval.node) = connect_nodes((yyvsp[-2].node), (yyvsp[0].node)); }
#line 1652 "parser.tab.c"
    break;

  case 54: /* non_empty_arg_list: expression  */
#line 357 "parser.y"
                                           { (yyval.node) = (yyvsp[0].node); }
#line 1658 "parser.tab.c"
    break;

  case 55: /* call_statement: func_call SEMICOLON  */
#line 363 "parser.y"
      {
          const char *caller_ret = func_lookup(saved_func_name);
          if (caller_ret && strcmp(caller_ret, "void") != 0) {
              char fname[64];
              const char *p = (yyvsp[-1].node)->label + 10;
              const char *cut = strchr(p, '<');
              if (cut) { int n=cut-p; strncpy(fname,p,n); fname[n]='\0'; }
              else strcpy(fname, p);
              const char *callee_ret = func_lookup(fname);
              if (callee_ret && strcmp(callee_ret, "void") != 0) {
                  fprintf(stderr,
                      "Error: return value of function '%s' is ignored\n", fname);
                  exit(1);
              }
          }
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1680 "parser.tab.c"
    break;

  case 56: /* return_statement: RETURN expression SEMICOLON  */
#line 385 "parser.y"
      {
          const char *fn_ret  = func_lookup(saved_func_name);
          const char *ex_type = node_type((yyvsp[-1].node));
          if (fn_ret && strcmp(fn_ret, "void") == 0) {
              fprintf(stderr,
                  "Type error: void function '%s' cannot return a value\n",
                  saved_func_name);
              exit(1);
          }
          if (fn_ret && strcmp(fn_ret, ex_type) != 0) {
              fprintf(stderr,
                  "Type error: function '%s' returns <%s>, got <%s>\n",
                  saved_func_name, fn_ret, ex_type);
              exit(1);
          }
          char buf[128];
          sprintf(buf, "Return:<%s>", ex_type);
          ASTNode *n = create_node(buf);
          n->child1 = (yyvsp[-1].node);
          has_return_stmt = 1;
          (yyval.node) = n;
      }
#line 1707 "parser.tab.c"
    break;

  case 57: /* return_statement: RETURN SEMICOLON  */
#line 408 "parser.y"
      {
          const char *fn_ret = func_lookup(saved_func_name);
          if (fn_ret && strcmp(fn_ret, "void") != 0) {
              fprintf(stderr,
                  "Type error: non-void function '%s' must return a value\n",
                  saved_func_name);
              exit(1);
          }
          has_return_stmt = 1;
          (yyval.node) = create_node("Return:<void>");
      }
#line 1723 "parser.tab.c"
    break;

  case 58: /* read_statement: READ NAME SEMICOLON  */
#line 426 "parser.y"
      {
          const char *t = sym_lookup((yyvsp[-1].str));
          if (strcmp(t, "int") != 0 && strcmp(t, "float") != 0) {
              fprintf(stderr, "Type error: read only valid for int/float\n");
              exit(1);
          }
          char buf[256];
          sprintf(buf, "Read: Name : %s_<%s>", (yyvsp[-1].str), t);
          free((yyvsp[-1].str));
          (yyval.node) = create_node(buf);
      }
#line 1739 "parser.tab.c"
    break;

  case 59: /* print_statement: WRITE expression SEMICOLON  */
#line 441 "parser.y"
      {
          if ((yyvsp[-1].node) && !(yyvsp[-1].node)->child1 && !(yyvsp[-1].node)->child2 && !(yyvsp[-1].node)->child3) {
              char buf[256];
              sprintf(buf, "Write: %s", (yyvsp[-1].node)->label);
              (yyval.node) = create_node(buf);
          } else {
              ASTNode *n = create_node("Write:");
              n->child1 = (yyvsp[-1].node);
              (yyval.node) = n;
          }
      }
#line 1755 "parser.tab.c"
    break;

  case 60: /* assignment_statement: NAME ASSIGN_OP expression SEMICOLON  */
#line 456 "parser.y"
      {
          const char *lhs_type = sym_lookup((yyvsp[-3].str));
          const char *rhs_type = node_type((yyvsp[-1].node));
          if (strcmp(lhs_type, rhs_type) != 0) {
              fprintf(stderr, "Type error: cannot assign <%s> to <%s> variable '%s'\n",
                      rhs_type, lhs_type, (yyvsp[-3].str));
              exit(1);
          }
          ASTNode *assign = create_node("Asgn:");
          char buf[256];
          sprintf(buf, "Name: %s_<%s>", (yyvsp[-3].str), lhs_type);
          free((yyvsp[-3].str));
          assign->child1 = create_node(buf);
          assign->child2 = (yyvsp[-1].node);
          (yyval.node) = assign;
      }
#line 1776 "parser.tab.c"
    break;

  case 61: /* if_statement: IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement  */
#line 479 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)), "bool") != 0) {
              fprintf(stderr, "Type error: if condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("If:");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); n->child3 = NULL;
          (yyval.node) = n;
      }
#line 1789 "parser.tab.c"
    break;

  case 62: /* if_statement: IF LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement ELSE statement  */
#line 488 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-4].node)), "bool") != 0) {
              fprintf(stderr, "Type error: if condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("If:");
          n->child1 = (yyvsp[-4].node); n->child2 = (yyvsp[-2].node); n->child3 = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1802 "parser.tab.c"
    break;

  case 63: /* while_statement: WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement  */
#line 500 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)), "bool") != 0) {
              fprintf(stderr, "Type error: while condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("While:");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1815 "parser.tab.c"
    break;

  case 64: /* do_while_statement: DO statement WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET SEMICOLON  */
#line 512 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)), "bool") != 0) {
              fprintf(stderr, "Type error: do-while condition must be bool\n"); exit(1);
          }
          ASTNode *n = create_node("Do:");
          n->child1 = (yyvsp[-5].node); n->child2 = (yyvsp[-2].node);
          (yyval.node) = n;
      }
#line 1828 "parser.tab.c"
    break;

  case 65: /* expression: conditional_expression  */
#line 526 "parser.y"
                              { (yyval.node) = (yyvsp[0].node); }
#line 1834 "parser.tab.c"
    break;

  case 66: /* conditional_expression: logical_or_expression  */
#line 530 "parser.y"
                             { (yyval.node) = (yyvsp[0].node); }
#line 1840 "parser.tab.c"
    break;

  case 67: /* conditional_expression: logical_or_expression QUESTION_MARK expression COLON conditional_expression  */
#line 532 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-4].node)), "bool") != 0) {
              fprintf(stderr, "Type error: ternary condition must be bool\n"); exit(1);
          }
          if (strcmp(node_type((yyvsp[-2].node)), node_type((yyvsp[0].node))) != 0) {
              fprintf(stderr, "Type error: ternary branches must have same type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: ?:");
          n->child1 = (yyvsp[-4].node); n->child2 = (yyvsp[-2].node); n->child3 = (yyvsp[0].node);
          (yyval.node) = n;
      }
#line 1856 "parser.tab.c"
    break;

  case 68: /* logical_or_expression: logical_or_expression OR logical_and_expression  */
#line 547 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)),"bool")!=0 || strcmp(node_type((yyvsp[0].node)),"bool")!=0) {
              fprintf(stderr, "Type error: || requires bool operands\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: OR<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1868 "parser.tab.c"
    break;

  case 69: /* logical_or_expression: logical_and_expression  */
#line 554 "parser.y"
                              { (yyval.node) = (yyvsp[0].node); }
#line 1874 "parser.tab.c"
    break;

  case 70: /* logical_and_expression: logical_and_expression AND not_expression  */
#line 559 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)),"bool")!=0 || strcmp(node_type((yyvsp[0].node)),"bool")!=0) {
              fprintf(stderr, "Type error: && requires bool operands\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: AND<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1886 "parser.tab.c"
    break;

  case 71: /* logical_and_expression: not_expression  */
#line 566 "parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1892 "parser.tab.c"
    break;

  case 72: /* not_expression: NOT not_expression  */
#line 571 "parser.y"
      {
          if (strcmp(node_type((yyvsp[0].node)), "bool") != 0) {
              fprintf(stderr, "Type error: ! requires bool operand\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: NOT<bool>");
          n->child1 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1904 "parser.tab.c"
    break;

  case 73: /* not_expression: equality_expression  */
#line 578 "parser.y"
                           { (yyval.node) = (yyvsp[0].node); }
#line 1910 "parser.tab.c"
    break;

  case 74: /* equality_expression: equality_expression EQUAL relational_expression  */
#line 583 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)), node_type((yyvsp[0].node))) != 0) {
              fprintf(stderr, "Type error: cannot compare <%s> with <%s>\n",
                      node_type((yyvsp[-2].node)), node_type((yyvsp[0].node))); exit(1);
          }
          ASTNode *n = create_node("Condition: EQ<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1923 "parser.tab.c"
    break;

  case 75: /* equality_expression: equality_expression NOT_EQUAL relational_expression  */
#line 592 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)), node_type((yyvsp[0].node))) != 0) {
              fprintf(stderr, "Type error: cannot compare <%s> with <%s>\n",
                      node_type((yyvsp[-2].node)), node_type((yyvsp[0].node))); exit(1);
          }
          ASTNode *n = create_node("Condition: NE<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1936 "parser.tab.c"
    break;

  case 76: /* equality_expression: relational_expression  */
#line 600 "parser.y"
                             { (yyval.node) = (yyvsp[0].node); }
#line 1942 "parser.tab.c"
    break;

  case 77: /* relational_expression: relational_expression LESS additive_expression  */
#line 605 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)),"string")==0||strcmp(node_type((yyvsp[-2].node)),"bool")==0||
              strcmp(node_type((yyvsp[0].node)),"string")==0||strcmp(node_type((yyvsp[0].node)),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: LT<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1955 "parser.tab.c"
    break;

  case 78: /* relational_expression: relational_expression GREATER additive_expression  */
#line 614 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)),"string")==0||strcmp(node_type((yyvsp[-2].node)),"bool")==0||
              strcmp(node_type((yyvsp[0].node)),"string")==0||strcmp(node_type((yyvsp[0].node)),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: GT<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1968 "parser.tab.c"
    break;

  case 79: /* relational_expression: relational_expression GREATER_EQUAL additive_expression  */
#line 623 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)),"string")==0||strcmp(node_type((yyvsp[-2].node)),"bool")==0||
              strcmp(node_type((yyvsp[0].node)),"string")==0||strcmp(node_type((yyvsp[0].node)),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: GE<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1981 "parser.tab.c"
    break;

  case 80: /* relational_expression: relational_expression LESS_EQUAL additive_expression  */
#line 632 "parser.y"
      {
          if (strcmp(node_type((yyvsp[-2].node)),"string")==0||strcmp(node_type((yyvsp[-2].node)),"bool")==0||
              strcmp(node_type((yyvsp[0].node)),"string")==0||strcmp(node_type((yyvsp[0].node)),"bool")==0) {
              fprintf(stderr, "Type error: relational operator on non-numeric type\n"); exit(1);
          }
          ASTNode *n = create_node("Condition: LE<bool>");
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 1994 "parser.tab.c"
    break;

  case 81: /* relational_expression: additive_expression  */
#line 640 "parser.y"
                           { (yyval.node) = (yyvsp[0].node); }
#line 2000 "parser.tab.c"
    break;

  case 82: /* additive_expression: additive_expression PLUS term  */
#line 645 "parser.y"
      {
          char buf[64]; sprintf(buf, "Arith: Plus<%s>", arith_type((yyvsp[-2].node),(yyvsp[0].node)));
          ASTNode *n = create_node(buf);
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 2010 "parser.tab.c"
    break;

  case 83: /* additive_expression: additive_expression MINUS term  */
#line 651 "parser.y"
      {
          char buf[64]; sprintf(buf, "Arith: Minus<%s>", arith_type((yyvsp[-2].node),(yyvsp[0].node)));
          ASTNode *n = create_node(buf);
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 2020 "parser.tab.c"
    break;

  case 84: /* additive_expression: term  */
#line 656 "parser.y"
            { (yyval.node) = (yyvsp[0].node); }
#line 2026 "parser.tab.c"
    break;

  case 85: /* term: term MUL factor  */
#line 661 "parser.y"
      {
          char buf[64]; sprintf(buf, "Arith: Mult<%s>", arith_type((yyvsp[-2].node),(yyvsp[0].node)));
          ASTNode *n = create_node(buf);
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 2036 "parser.tab.c"
    break;

  case 86: /* term: term DIV factor  */
#line 667 "parser.y"
      {
          char buf[64]; sprintf(buf, "Arith: Div<%s>", arith_type((yyvsp[-2].node),(yyvsp[0].node)));
          ASTNode *n = create_node(buf);
          n->child1 = (yyvsp[-2].node); n->child2 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 2046 "parser.tab.c"
    break;

  case 87: /* term: factor  */
#line 672 "parser.y"
              { (yyval.node) = (yyvsp[0].node); }
#line 2052 "parser.tab.c"
    break;

  case 88: /* factor: func_call  */
#line 685 "parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 2058 "parser.tab.c"
    break;

  case 89: /* factor: NAME  */
#line 687 "parser.y"
      {
          char buf[256];
          sprintf(buf, "Name: %s_<%s>", (yyvsp[0].str), sym_lookup((yyvsp[0].str)));
          free((yyvsp[0].str)); (yyval.node) = create_node(buf);
      }
#line 2068 "parser.tab.c"
    break;

  case 90: /* factor: INT_NUM  */
#line 693 "parser.y"
      {
          char buf[128]; sprintf(buf, "Num: %d<int>", atoi((yyvsp[0].str)));
          (yyval.node) = create_node(buf); free((yyvsp[0].str));
      }
#line 2077 "parser.tab.c"
    break;

  case 91: /* factor: FLOAT_NUM  */
#line 698 "parser.y"
      {
          char buf[128]; sprintf(buf, "Num: %.2f<float>", atof((yyvsp[0].str)));
          (yyval.node) = create_node(buf); free((yyvsp[0].str));
      }
#line 2086 "parser.tab.c"
    break;

  case 92: /* factor: STR_CONST  */
#line 703 "parser.y"
      {
          char buf[128]; sprintf(buf, "Str: %s<string>", (yyvsp[0].str));
          (yyval.node) = create_node(buf); free((yyvsp[0].str));
      }
#line 2095 "parser.tab.c"
    break;

  case 93: /* factor: BOOL_CONST  */
#line 708 "parser.y"
      {
          char buf[128]; sprintf(buf, "Bool: %s<bool>", (yyvsp[0].str));
          (yyval.node) = create_node(buf); free((yyvsp[0].str));
      }
#line 2104 "parser.tab.c"
    break;

  case 94: /* factor: LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET  */
#line 713 "parser.y"
      { (yyval.node) = (yyvsp[-1].node); }
#line 2110 "parser.tab.c"
    break;

  case 95: /* factor: MINUS factor  */
#line 715 "parser.y"
      {
          if (strcmp(node_type((yyvsp[0].node)),"string")==0||strcmp(node_type((yyvsp[0].node)),"bool")==0) {
              fprintf(stderr, "Type error: unary minus on non-numeric type\n"); exit(1);
          }
          char buf[64]; sprintf(buf, "Arith: Uminus<%s>", node_type((yyvsp[0].node)));
          ASTNode *n = create_node(buf);
          n->child1 = (yyvsp[0].node); (yyval.node) = n;
      }
#line 2123 "parser.tab.c"
    break;


#line 2127 "parser.tab.c"

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

#line 725 "parser.y"


int yyerror(const char *s)
{
    fprintf(stderr, "Parser error\n");
    exit(1);
}
