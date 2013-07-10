/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 7 "aldor/aldor/src/axl.y"

# include "axlphase.h"

# define yyerror(s)	yyerrorfn(s)

#ifdef NDEBUG
#  undef  YYDEBUG
#else
#  define YYDEBUG 1
#endif
int yydebug;

# define TPOS(t)((t)->pos)
# define TEND(t)((t)->end-1)
# define APOS(a) abPos(a)
# define TEST(a) abNewTest(abPos(a),(a))
# define abZip	 abNewNothing(sposNone)

/* Line 371 of yacc.c  */
#line 87 "y.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _YY_TK_Id = 1,
     _YY_TK_Blank = 2,
     _YY_TK_Int = 3,
     _YY_TK_Float = 4,
     _YY_TK_String = 5,
     _YY_TK_PreDoc = 6,
     _YY_TK_PostDoc = 7,
     _YY_TK_Comment = 8,
     _YY_TK_SysCmd = 9,
     _YY_TK_Error = 10,
     _YY_KW_Add = 11,
     _YY_KW_And = 12,
     _YY_KW_Always = 13,
     _YY_KW_Assert = 14,
     _YY_KW_Break = 15,
     _YY_KW_But = 16,
     _YY_KW_By = 17,
     _YY_KW_Case = 18,
     _YY_KW_Catch = 19,
     _YY_KW_Default = 20,
     _YY_KW_Define = 21,
     _YY_KW_Delay = 22,
     _YY_KW_Do = 23,
     _YY_KW_Else = 24,
     _YY_KW_Except = 25,
     _YY_KW_Export = 26,
     _YY_KW_Extend = 27,
     _YY_KW_Finally = 28,
     _YY_KW_Fix = 29,
     _YY_KW_For = 30,
     _YY_KW_Fluid = 31,
     _YY_KW_Free = 32,
     _YY_KW_From = 33,
     _YY_KW_Generate = 34,
     _YY_KW_Goto = 35,
     _YY_KW_Has = 36,
     _YY_KW_If = 37,
     _YY_KW_Import = 38,
     _YY_KW_In = 39,
     _YY_KW_Inline = 40,
     _YY_KW_Is = 41,
     _YY_KW_Isnt = 42,
     _YY_KW_Iterate = 43,
     _YY_KW_Let = 44,
     _YY_KW_Local = 45,
     _YY_KW_Macro = 46,
     _YY_KW_Mod = 47,
     _YY_KW_Never = 48,
     _YY_KW_Not = 49,
     _YY_KW_Of = 50,
     _YY_KW_Or = 51,
     _YY_KW_Pretend = 52,
     _YY_KW_Quo = 53,
     _YY_KW_Reference = 54,
     _YY_KW_Rem = 55,
     _YY_KW_Repeat = 56,
     _YY_KW_Return = 57,
     _YY_KW_Rule = 58,
     _YY_KW_Select = 59,
     _YY_KW_Then = 60,
     _YY_KW_Throw = 61,
     _YY_KW_To = 62,
     _YY_KW_Try = 63,
     _YY_KW_Where = 64,
     _YY_KW_While = 65,
     _YY_KW_With = 66,
     _YY_KW_Yield = 67,
     _YY_KW_Quote = 68,
     _YY_KW_Grave = 69,
     _YY_KW_Ampersand = 70,
     _YY_KW_Comma = 71,
     _YY_KW_Semicolon = 72,
     _YY_KW_Dollar = 73,
     _YY_KW_Sharp = 74,
     _YY_KW_At = 75,
     _YY_KW_Assign = 76,
     _YY_KW_Colon = 77,
     _YY_KW_ColonStar = 78,
     _YY_KW_2Colon = 79,
     _YY_KW_Star = 80,
     _YY_KW_2Star = 81,
     _YY_KW_Dot = 82,
     _YY_KW_2Dot = 83,
     _YY_KW_EQ = 84,
     _YY_KW_2EQ = 85,
     _YY_KW_MArrow = 86,
     _YY_KW_Implies = 87,
     _YY_KW_GT = 88,
     _YY_KW_2GT = 89,
     _YY_KW_GE = 90,
     _YY_KW_LT = 91,
     _YY_KW_2LT = 92,
     _YY_KW_LE = 93,
     _YY_KW_LArrow = 94,
     _YY_KW_Hat = 95,
     _YY_KW_HatE = 96,
     _YY_KW_Tilde = 97,
     _YY_KW_TildeE = 98,
     _YY_KW_Plus = 99,
     _YY_KW_PlusMinus = 100,
     _YY_KW_MapsTo = 101,
     _YY_KW_MapsToStar = 102,
     _YY_KW_Minus = 103,
     _YY_KW_RArrow = 104,
     _YY_KW_MapStar = 105,
     _YY_KW_Slash = 106,
     _YY_KW_Wedge = 107,
     _YY_KW_Backslash = 108,
     _YY_KW_Vee = 109,
     _YY_KW_OBrack = 110,
     _YY_KW_OBBrack = 111,
     _YY_KW_OCurly = 112,
     _YY_KW_OBCurly = 113,
     _YY_KW_OParen = 114,
     _YY_KW_OBParen = 115,
     _YY_KW_CBrack = 116,
     _YY_KW_CCurly = 117,
     _YY_KW_CParen = 118,
     _YY_KW_Bar = 119,
     _YY_KW_CBBrack = 120,
     _YY_KW_CBCurly = 121,
     _YY_KW_CBParen = 122,
     _YY_KW_2Bar = 123,
     _YY_KW_NewLine = 124,
     _YY_KW_StartPile = 125,
     _YY_KW_EndPile = 126,
     _YY_KW_SetTab = 127,
     _YY_KW_BackSet = 128,
     _YY_KW_BackTab = 129,
     _YY_KW_Juxtapose = 130
   };
#endif
/* Tokens.  */
#define _YY_TK_Id 1
#define _YY_TK_Blank 2
#define _YY_TK_Int 3
#define _YY_TK_Float 4
#define _YY_TK_String 5
#define _YY_TK_PreDoc 6
#define _YY_TK_PostDoc 7
#define _YY_TK_Comment 8
#define _YY_TK_SysCmd 9
#define _YY_TK_Error 10
#define _YY_KW_Add 11
#define _YY_KW_And 12
#define _YY_KW_Always 13
#define _YY_KW_Assert 14
#define _YY_KW_Break 15
#define _YY_KW_But 16
#define _YY_KW_By 17
#define _YY_KW_Case 18
#define _YY_KW_Catch 19
#define _YY_KW_Default 20
#define _YY_KW_Define 21
#define _YY_KW_Delay 22
#define _YY_KW_Do 23
#define _YY_KW_Else 24
#define _YY_KW_Except 25
#define _YY_KW_Export 26
#define _YY_KW_Extend 27
#define _YY_KW_Finally 28
#define _YY_KW_Fix 29
#define _YY_KW_For 30
#define _YY_KW_Fluid 31
#define _YY_KW_Free 32
#define _YY_KW_From 33
#define _YY_KW_Generate 34
#define _YY_KW_Goto 35
#define _YY_KW_Has 36
#define _YY_KW_If 37
#define _YY_KW_Import 38
#define _YY_KW_In 39
#define _YY_KW_Inline 40
#define _YY_KW_Is 41
#define _YY_KW_Isnt 42
#define _YY_KW_Iterate 43
#define _YY_KW_Let 44
#define _YY_KW_Local 45
#define _YY_KW_Macro 46
#define _YY_KW_Mod 47
#define _YY_KW_Never 48
#define _YY_KW_Not 49
#define _YY_KW_Of 50
#define _YY_KW_Or 51
#define _YY_KW_Pretend 52
#define _YY_KW_Quo 53
#define _YY_KW_Reference 54
#define _YY_KW_Rem 55
#define _YY_KW_Repeat 56
#define _YY_KW_Return 57
#define _YY_KW_Rule 58
#define _YY_KW_Select 59
#define _YY_KW_Then 60
#define _YY_KW_Throw 61
#define _YY_KW_To 62
#define _YY_KW_Try 63
#define _YY_KW_Where 64
#define _YY_KW_While 65
#define _YY_KW_With 66
#define _YY_KW_Yield 67
#define _YY_KW_Quote 68
#define _YY_KW_Grave 69
#define _YY_KW_Ampersand 70
#define _YY_KW_Comma 71
#define _YY_KW_Semicolon 72
#define _YY_KW_Dollar 73
#define _YY_KW_Sharp 74
#define _YY_KW_At 75
#define _YY_KW_Assign 76
#define _YY_KW_Colon 77
#define _YY_KW_ColonStar 78
#define _YY_KW_2Colon 79
#define _YY_KW_Star 80
#define _YY_KW_2Star 81
#define _YY_KW_Dot 82
#define _YY_KW_2Dot 83
#define _YY_KW_EQ 84
#define _YY_KW_2EQ 85
#define _YY_KW_MArrow 86
#define _YY_KW_Implies 87
#define _YY_KW_GT 88
#define _YY_KW_2GT 89
#define _YY_KW_GE 90
#define _YY_KW_LT 91
#define _YY_KW_2LT 92
#define _YY_KW_LE 93
#define _YY_KW_LArrow 94
#define _YY_KW_Hat 95
#define _YY_KW_HatE 96
#define _YY_KW_Tilde 97
#define _YY_KW_TildeE 98
#define _YY_KW_Plus 99
#define _YY_KW_PlusMinus 100
#define _YY_KW_MapsTo 101
#define _YY_KW_MapsToStar 102
#define _YY_KW_Minus 103
#define _YY_KW_RArrow 104
#define _YY_KW_MapStar 105
#define _YY_KW_Slash 106
#define _YY_KW_Wedge 107
#define _YY_KW_Backslash 108
#define _YY_KW_Vee 109
#define _YY_KW_OBrack 110
#define _YY_KW_OBBrack 111
#define _YY_KW_OCurly 112
#define _YY_KW_OBCurly 113
#define _YY_KW_OParen 114
#define _YY_KW_OBParen 115
#define _YY_KW_CBrack 116
#define _YY_KW_CCurly 117
#define _YY_KW_CParen 118
#define _YY_KW_Bar 119
#define _YY_KW_CBBrack 120
#define _YY_KW_CBCurly 121
#define _YY_KW_CBParen 122
#define _YY_KW_2Bar 123
#define _YY_KW_NewLine 124
#define _YY_KW_StartPile 125
#define _YY_KW_EndPile 126
#define _YY_KW_SetTab 127
#define _YY_KW_BackSet 128
#define _YY_KW_BackTab 129
#define _YY_KW_Juxtapose 130



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 165 "aldor/aldor/src/axl.y"

	Token           tok ;
	TokenList	toklist ;
	AbSyn           ab ;
	AbSynList       ablist ;


/* Line 387 of yacc.c  */
#line 399 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 427 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2514

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  133
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  143
/* YYNRULES -- Number of rules.  */
#define YYNRULES  358
/* YYNRULES -- Number of states.  */
#define YYNSTATES  547

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   257

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    15,    18,    21,
      24,    27,    30,    33,    36,    39,    43,    47,    49,    52,
      56,    60,    63,    66,    68,    70,    74,    76,    78,    81,
      84,    86,    88,    92,    94,    96,    99,   101,   103,   105,
     107,   109,   113,   117,   121,   125,   129,   131,   135,   139,
     143,   147,   151,   153,   157,   161,   165,   169,   173,   175,
     179,   183,   187,   191,   195,   200,   202,   204,   206,   213,
     217,   221,   224,   230,   236,   241,   244,   247,   250,   254,
     257,   260,   263,   266,   269,   272,   275,   278,   280,   282,
     289,   293,   297,   300,   306,   312,   317,   320,   323,   326,
     330,   333,   336,   339,   342,   345,   348,   351,   354,   356,
     358,   362,   364,   368,   370,   373,   376,   378,   381,   384,
     386,   388,   391,   393,   395,   398,   404,   407,   409,   412,
     415,   418,   421,   423,   426,   428,   430,   432,   434,   438,
     442,   446,   448,   452,   456,   459,   461,   464,   468,   470,
     474,   477,   479,   483,   485,   489,   491,   495,   497,   501,
     505,   509,   511,   515,   519,   523,   525,   527,   531,   533,
     537,   539,   543,   545,   549,   551,   555,   559,   563,   567,
     569,   571,   573,   575,   577,   579,   581,   583,   585,   587,
     589,   591,   593,   595,   597,   599,   601,   603,   605,   607,
     609,   611,   613,   615,   617,   619,   621,   623,   625,   627,
     629,   631,   633,   635,   637,   639,   641,   643,   645,   647,
     649,   651,   653,   655,   657,   659,   661,   663,   665,   667,
     669,   671,   673,   675,   677,   679,   681,   683,   685,   688,
     691,   693,   696,   699,   701,   704,   707,   711,   713,   716,
     719,   723,   725,   727,   729,   731,   733,   735,   737,   739,
     741,   743,   745,   747,   749,   751,   754,   758,   761,   765,
     768,   772,   774,   776,   778,   780,   782,   784,   786,   788,
     790,   792,   794,   795,   797,   799,   801,   803,   805,   807,
     809,   811,   813,   817,   819,   823,   825,   829,   831,   835,
     837,   841,   843,   847,   849,   853,   855,   859,   861,   863,
     865,   867,   869,   871,   873,   875,   877,   879,   881,   883,
     885,   887,   889,   891,   895,   897,   899,   901,   904,   906,
     909,   911,   913,   915,   917,   919,   923,   925,   929,   931,
     935,   937,   941,   943,   945,   949,   952,   956,   960,   962,
     966,   970,   972,   974,   977,   979,   982,   986,   990
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     134,     0,    -1,   271,    -1,   266,    -1,   144,    -1,   137,
      -1,    77,   224,   252,    -1,    48,   141,    -1,    29,   142,
      -1,    47,   142,    -1,    34,   142,    -1,    33,   142,    -1,
      22,   142,    -1,    23,   142,    -1,    31,   142,    -1,    42,
     251,   250,    -1,    40,   251,   250,    -1,   138,    -1,    28,
     251,    -1,    28,   251,   139,    -1,    28,   251,   140,    -1,
      64,   171,    -1,    35,   261,    -1,   142,    -1,   138,    -1,
      40,   142,   250,    -1,   146,    -1,   219,    -1,    79,   182,
      -1,    80,   182,    -1,   260,    -1,   151,    -1,   151,    66,
     145,    -1,   155,    -1,   148,    -1,   148,   143,    -1,   259,
      -1,   152,    -1,   153,    -1,   154,    -1,   165,    -1,   171,
      78,   152,    -1,   171,    87,   152,    -1,   171,    88,   152,
      -1,   171,   103,   152,    -1,   171,   104,   152,    -1,   157,
      -1,   171,    78,   153,    -1,   171,    87,   153,    -1,   171,
      88,   153,    -1,   171,   103,   153,    -1,   171,   104,   153,
      -1,   156,    -1,   171,    78,   154,    -1,   171,    87,   154,
      -1,   171,    88,   154,    -1,   171,   103,   154,    -1,   171,
     104,   154,    -1,   147,    -1,   147,    78,   151,    -1,   147,
      87,   151,    -1,   147,    88,   151,    -1,   147,   103,   151,
      -1,   147,   104,   151,    -1,    39,   145,    62,   151,    -1,
     159,    -1,   158,    -1,   165,    -1,    39,   145,    62,   150,
      26,   150,    -1,   165,    89,   150,    -1,   166,    58,   150,
      -1,    58,   150,    -1,    65,   151,    18,   161,   163,    -1,
      65,   151,    21,   161,   163,    -1,    61,   151,    41,   162,
      -1,    25,   150,    -1,    24,   150,    -1,    56,   150,    -1,
      36,   160,   150,    -1,    16,   150,    -1,    45,   249,    -1,
      17,   249,    -1,    59,   248,    -1,    69,   150,    -1,    27,
     150,    -1,    63,   150,    -1,    37,   226,    -1,    50,    -1,
     165,    -1,    39,   145,    62,   150,    26,   151,    -1,   165,
      89,   151,    -1,   166,    58,   151,    -1,    58,   151,    -1,
      65,   151,    18,   161,   164,    -1,    65,   151,    21,   161,
     164,    -1,    61,   151,    41,   162,    -1,    25,   151,    -1,
      24,   151,    -1,    56,   151,    -1,    36,   160,   151,    -1,
      16,   151,    -1,    45,   249,    -1,    17,   249,    -1,    59,
     248,    -1,    69,   151,    -1,    27,   151,    -1,    63,   151,
      -1,    37,   226,    -1,    50,    -1,   228,    -1,    64,   145,
      52,    -1,   228,    -1,   146,    41,   162,    -1,   149,    -1,
      15,   150,    -1,    30,   150,    -1,   228,    -1,    15,   151,
      -1,    30,   151,    -1,   228,    -1,   171,    -1,   171,   166,
      -1,   167,    -1,   168,    -1,   167,   168,    -1,    32,   169,
      41,   171,   247,    -1,    67,   171,    -1,   171,    -1,    34,
     171,    -1,    47,   171,    -1,    33,   171,    -1,   121,   171,
      -1,   172,    -1,   172,   143,    -1,   219,    -1,   181,    -1,
     173,    -1,   174,    -1,   173,    14,   174,    -1,   173,    53,
     174,    -1,   173,   192,   174,    -1,   175,    -1,   174,    38,
     175,    -1,   174,   193,   175,    -1,   193,   175,    -1,   176,
      -1,   175,   194,    -1,   175,   194,   176,    -1,   177,    -1,
     176,   195,   177,    -1,   195,   177,    -1,   178,    -1,   177,
     196,   178,    -1,   179,    -1,   178,   197,   179,    -1,   180,
      -1,   180,   198,   179,    -1,   183,    -1,   180,    81,   183,
      -1,   180,    77,   183,    -1,   180,    54,   183,    -1,   189,
      -1,   181,    81,   183,    -1,   181,    77,   183,    -1,   181,
      54,   183,    -1,   180,    -1,   184,    -1,   184,   191,   183,
      -1,   187,    -1,   187,    75,   185,    -1,   209,    -1,   209,
      75,   185,    -1,   214,    -1,   214,    75,   186,    -1,   188,
      -1,   246,    68,   216,    -1,   246,    13,   216,    -1,   187,
      27,   188,    -1,   187,    63,   188,    -1,   207,    -1,   191,
      -1,   192,    -1,   193,    -1,   194,    -1,   195,    -1,   196,
      -1,   197,    -1,   198,    -1,   236,    -1,   235,    -1,   234,
      -1,   233,    -1,   232,    -1,   231,    -1,   230,    -1,   229,
      -1,   244,    -1,   243,    -1,   242,    -1,   241,    -1,   240,
      -1,   239,    -1,   238,    -1,   237,    -1,   106,    -1,    96,
      -1,   107,    -1,   111,    -1,   109,    -1,    86,    -1,   100,
      -1,    98,    -1,    92,    -1,    90,    -1,    91,    -1,    95,
      -1,    93,    -1,    94,    -1,    43,    -1,    44,    -1,    20,
      -1,    85,    -1,    19,    -1,   101,    -1,   105,    -1,   102,
      -1,    49,    -1,    55,    -1,    57,    -1,    82,    -1,   108,
      -1,   110,    -1,    83,    -1,    97,    -1,   208,    -1,   211,
      -1,   213,    -1,   212,    -1,   212,   210,    -1,    51,   210,
      -1,   213,    -1,   213,   210,    -1,    51,   210,    -1,   224,
      -1,    51,   218,    -1,   212,   218,    -1,   212,    84,   217,
      -1,   214,    -1,    51,   218,    -1,   213,   218,    -1,   213,
      84,   217,    -1,   224,    -1,   215,    -1,   220,    -1,   221,
      -1,   222,    -1,   245,    -1,   219,    -1,   224,    -1,   215,
      -1,   219,    -1,   215,    -1,   219,    -1,   268,    -1,   269,
      -1,   116,   120,    -1,   116,   135,   120,    -1,   112,   118,
      -1,   112,   135,   118,    -1,    70,    70,    -1,    70,   223,
      70,    -1,   258,    -1,   226,    -1,   227,    -1,   226,    -1,
     190,    -1,     3,    -1,    76,    -1,    99,    -1,     5,    -1,
       6,    -1,     7,    -1,    -1,   206,    -1,   205,    -1,   204,
      -1,   203,    -1,   202,    -1,   201,    -1,   200,    -1,   199,
      -1,   206,    -1,   206,    75,   186,    -1,   205,    -1,   205,
      75,   186,    -1,   204,    -1,   204,    75,   186,    -1,   203,
      -1,   203,    75,   186,    -1,   202,    -1,   202,    75,   186,
      -1,   201,    -1,   201,    75,   186,    -1,   200,    -1,   200,
      75,   186,    -1,   199,    -1,   199,    75,   186,    -1,   228,
      -1,   207,    -1,   228,    -1,   187,    -1,   228,    -1,   170,
      -1,   228,    -1,   165,    -1,   228,    -1,   225,    -1,   228,
      -1,   140,    -1,   228,    -1,   142,    -1,   228,    -1,   136,
      -1,   254,   135,   255,    -1,   256,    -1,   257,    -1,   228,
      -1,     8,   256,    -1,   228,    -1,     9,   257,    -1,   262,
      -1,   263,    -1,   264,    -1,   265,    -1,   225,    -1,   262,
      73,   225,    -1,   172,    -1,   263,    73,   172,    -1,   145,
      -1,   264,    73,   145,    -1,   171,    -1,   265,    73,   171,
      -1,   267,    -1,   136,    -1,   267,    74,   136,    -1,   267,
      74,    -1,   129,   270,   131,    -1,   114,   271,   119,    -1,
     253,    -1,   270,   130,   253,    -1,     1,   130,   253,    -1,
     272,    -1,   273,    -1,   273,   275,    -1,   228,    -1,   273,
     274,    -1,   275,    74,   255,    -1,     1,    74,   255,    -1,
     254,   136,   255,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   436,   436,   441,   445,   446,   447,   452,   454,   456,
     458,   460,   462,   464,   466,   468,   470,   472,   476,   478,
     480,   485,   490,   495,   496,   497,   502,   503,   507,   509,
     515,   519,   520,   526,   530,   531,   536,   543,   546,   549,
     554,   555,   557,   559,   561,   563,   567,   568,   570,   572,
     574,   576,   580,   581,   583,   585,   587,   589,   595,   596,
     598,   600,   602,   604,   610,   612,   616,   621,   622,   624,
     626,   628,   630,   632,   634,   636,   638,   640,   642,   644,
     646,   648,   650,   652,   654,   656,   658,   660,   664,   665,
     667,   669,   671,   673,   675,   677,   679,   681,   683,   685,
     687,   689,   691,   693,   695,   697,   699,   701,   703,   708,
     709,   714,   716,   721,   726,   728,   730,   733,   735,   737,
     742,   743,   748,   753,   755,   760,   762,   767,   768,   770,
     772,   777,   784,   785,   787,   791,   792,   797,   798,   800,
     802,   807,   808,   810,   812,   817,   818,   820,   825,   826,
     828,   833,   834,   839,   840,   845,   846,   852,   853,   855,
     857,   861,   862,   864,   866,   871,   875,   876,   881,   882,
     887,   888,   893,   894,   899,   900,   902,   904,   906,   911,
     917,   918,   919,   920,   921,   922,   923,   924,   928,   929,
     930,   931,   932,   933,   934,   935,   939,   940,   941,   942,
     943,   944,   945,   946,   948,   948,   948,   949,   949,   950,
     950,   950,   951,   951,   951,   952,   952,   952,   953,   953,
     953,   954,   954,   955,   955,   955,   956,   956,   956,   957,
     957,   957,   958,   958,   963,   967,   971,   976,   977,   979,
     983,   984,   986,   992,   993,   995,   997,  1001,  1002,  1004,
    1006,  1012,  1013,  1017,  1018,  1019,  1023,  1024,  1028,  1029,
    1030,  1034,  1035,  1039,  1040,  1044,  1049,  1054,  1061,  1071,
    1078,  1087,  1092,  1093,  1097,  1098,  1102,  1104,  1106,  1112,
    1114,  1116,  1124,  1129,  1133,  1137,  1141,  1145,  1149,  1153,
    1157,  1163,  1165,  1169,  1171,  1175,  1177,  1181,  1183,  1187,
    1189,  1193,  1195,  1199,  1201,  1205,  1207,  1213,  1214,  1217,
    1218,  1221,  1222,  1225,  1226,  1229,  1230,  1233,  1234,  1237,
    1238,  1241,  1242,  1249,  1258,  1263,  1268,  1270,  1275,  1277,
    1284,  1289,  1294,  1299,  1308,  1310,  1314,  1316,  1320,  1322,
    1326,  1328,  1336,  1342,  1344,  1346,  1353,  1359,  1365,  1367,
    1369,  1375,  1381,  1382,  1388,  1390,  1396,  1398,  1404
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "_YY_TK_Id", "_YY_TK_Blank",
  "_YY_TK_Int", "_YY_TK_Float", "_YY_TK_String", "_YY_TK_PreDoc",
  "_YY_TK_PostDoc", "_YY_TK_Comment", "_YY_TK_SysCmd", "_YY_TK_Error",
  "_YY_KW_Add", "_YY_KW_And", "_YY_KW_Always", "_YY_KW_Assert",
  "_YY_KW_Break", "_YY_KW_But", "_YY_KW_By", "_YY_KW_Case", "_YY_KW_Catch",
  "_YY_KW_Default", "_YY_KW_Define", "_YY_KW_Delay", "_YY_KW_Do",
  "_YY_KW_Else", "_YY_KW_Except", "_YY_KW_Export", "_YY_KW_Extend",
  "_YY_KW_Finally", "_YY_KW_Fix", "_YY_KW_For", "_YY_KW_Fluid",
  "_YY_KW_Free", "_YY_KW_From", "_YY_KW_Generate", "_YY_KW_Goto",
  "_YY_KW_Has", "_YY_KW_If", "_YY_KW_Import", "_YY_KW_In", "_YY_KW_Inline",
  "_YY_KW_Is", "_YY_KW_Isnt", "_YY_KW_Iterate", "_YY_KW_Let",
  "_YY_KW_Local", "_YY_KW_Macro", "_YY_KW_Mod", "_YY_KW_Never",
  "_YY_KW_Not", "_YY_KW_Of", "_YY_KW_Or", "_YY_KW_Pretend", "_YY_KW_Quo",
  "_YY_KW_Reference", "_YY_KW_Rem", "_YY_KW_Repeat", "_YY_KW_Return",
  "_YY_KW_Rule", "_YY_KW_Select", "_YY_KW_Then", "_YY_KW_Throw",
  "_YY_KW_To", "_YY_KW_Try", "_YY_KW_Where", "_YY_KW_While", "_YY_KW_With",
  "_YY_KW_Yield", "_YY_KW_Quote", "_YY_KW_Grave", "_YY_KW_Ampersand",
  "_YY_KW_Comma", "_YY_KW_Semicolon", "_YY_KW_Dollar", "_YY_KW_Sharp",
  "_YY_KW_At", "_YY_KW_Assign", "_YY_KW_Colon", "_YY_KW_ColonStar",
  "_YY_KW_2Colon", "_YY_KW_Star", "_YY_KW_2Star", "_YY_KW_Dot",
  "_YY_KW_2Dot", "_YY_KW_EQ", "_YY_KW_2EQ", "_YY_KW_MArrow",
  "_YY_KW_Implies", "_YY_KW_GT", "_YY_KW_2GT", "_YY_KW_GE", "_YY_KW_LT",
  "_YY_KW_2LT", "_YY_KW_LE", "_YY_KW_LArrow", "_YY_KW_Hat", "_YY_KW_HatE",
  "_YY_KW_Tilde", "_YY_KW_TildeE", "_YY_KW_Plus", "_YY_KW_PlusMinus",
  "_YY_KW_MapsTo", "_YY_KW_MapsToStar", "_YY_KW_Minus", "_YY_KW_RArrow",
  "_YY_KW_MapStar", "_YY_KW_Slash", "_YY_KW_Wedge", "_YY_KW_Backslash",
  "_YY_KW_Vee", "_YY_KW_OBrack", "_YY_KW_OBBrack", "_YY_KW_OCurly",
  "_YY_KW_OBCurly", "_YY_KW_OParen", "_YY_KW_OBParen", "_YY_KW_CBrack",
  "_YY_KW_CCurly", "_YY_KW_CParen", "_YY_KW_Bar", "_YY_KW_CBBrack",
  "_YY_KW_CBCurly", "_YY_KW_CBParen", "_YY_KW_2Bar", "_YY_KW_NewLine",
  "_YY_KW_StartPile", "_YY_KW_EndPile", "_YY_KW_SetTab", "_YY_KW_BackSet",
  "_YY_KW_BackTab", "_YY_KW_Juxtapose", "$accept", "Goal", "Expression",
  "Labeled", "Declaration", "ExportDecl", "ToPart", "FromPart",
  "MacroBody", "Sig", "DeclPart", "Comma", "CommaItem", "DeclBinding",
  "InfixedExprsDecl", "InfixedExprs", "Binding_Collection_",
  "Binding_BalStatement_", "Binding_AnyStatement_",
  "BindingL_Infixed_Collection_", "BindingL_Infixed_BalStatement_",
  "BindingL_Infixed_AnyStatement_",
  "BindingR_InfixedExprsDecl_AnyStatement_", "AnyStatement",
  "BalStatement", "Flow_BalStatement_", "Flow_AnyStatement_", "GenBound",
  "ButExpr", "Cases", "AlwaysPart_BalStatement_",
  "AlwaysPart_AnyStatement_", "Collection", "Iterators", "Iterators1",
  "Iterator", "ForLhs", "SuchthatPart", "Infixed", "InfixedExpr", "E3",
  "E4", "E5", "E6", "E7", "E8", "E9", "E11_E12_", "E11_Op_", "Type", "E12",
  "E13", "QualTail", "OpQualTail", "E14", "E15", "Op", "NakedOp",
  "ArrowOp", "LatticeOp", "RelationOp", "SegOp", "PlusOp", "QuotientOp",
  "TimesOp", "PowerOp", "ArrowTok", "LatticeTok", "RelationTok", "SegTok",
  "PlusTok", "QuotientTok", "TimesTok", "PowerTok", "Application",
  "RightJuxtaposed", "LeftJuxtaposed", "Jright_Atom_", "Jright_Molecule_",
  "Jleft_Atom_", "Jleft_Molecule_", "Molecule", "Enclosure",
  "DeclMolecule", "BlockMolecule", "BlockEnclosure", "Block", "Parened",
  "Bracketed", "QuotedIds", "Names", "Atom", "Name", "Id", "Literal",
  "Nothing", "UnqualOp_PowerTok_", "UnqualOp_TimesTok_",
  "UnqualOp_QuotientTok_", "UnqualOp_PlusTok_", "UnqualOp_SegTok_",
  "UnqualOp_RelationTok_", "UnqualOp_LatticeTok_", "UnqualOp_ArrowTok_",
  "QualOp_PowerTok_", "QualOp_TimesTok_", "QualOp_QuotientTok_",
  "QualOp_PlusTok_", "QualOp_SegTok_", "QualOp_RelationTok_",
  "QualOp_LatticeTok_", "QualOp_ArrowTok_", "opt_Application_", "opt_E14_",
  "opt_SuchthatPart_", "opt_Collection_", "opt_Name_", "opt_FromPart_",
  "opt_Sig_", "opt_Labeled_", "Doc_Expression_", "PreDocument",
  "PostDocument", "PreDocumentList", "PostDocumentList",
  "enlist1_Name__YY_KW_Comma_AB_Comma_",
  "enlist1_InfixedExpr__YY_KW_Comma_AB_Comma_",
  "enlist1_CommaItem__YY_KW_Comma_AB_Comma_",
  "enlist1_Infixed__YY_KW_Comma_AB_Comma_", "enlister1_Name__YY_KW_Comma_",
  "enlister1_InfixedExpr__YY_KW_Comma_",
  "enlister1_CommaItem__YY_KW_Comma_", "enlister1_Infixed__YY_KW_Comma_",
  "enlist1a_Labeled__YY_KW_Semicolon_AB_Sequence_",
  "enlister1a_Labeled__YY_KW_Semicolon_", "Piled_Expression_",
  "Curly_Labeled_", "PileContents_Expression_", "CurlyContents_Labeled_",
  "CurlyContentsList_Labeled_", "CurlyContent1_Labeled_",
  "CurlyContentA_Labeled_", "CurlyContentB_Labeled_", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   133,   134,   135,   136,   136,   136,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   138,   138,
     138,   139,   140,   141,   141,   141,   142,   142,   143,   143,
     144,   145,   145,   146,   147,   147,   148,   149,   150,   151,
     152,   152,   152,   152,   152,   152,   153,   153,   153,   153,
     153,   153,   154,   154,   154,   154,   154,   154,   155,   155,
     155,   155,   155,   155,   156,   156,   157,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   160,
     160,   161,   161,   162,   163,   163,   163,   164,   164,   164,
     165,   165,   166,   167,   167,   168,   168,   169,   169,   169,
     169,   170,   171,   171,   171,   172,   172,   173,   173,   173,
     173,   174,   174,   174,   174,   175,   175,   175,   176,   176,
     176,   177,   177,   178,   178,   179,   179,   180,   180,   180,
     180,   181,   181,   181,   181,   182,   183,   183,   184,   184,
     185,   185,   186,   186,   187,   187,   187,   187,   187,   188,
     189,   189,   189,   189,   189,   189,   189,   189,   190,   190,
     190,   190,   190,   190,   190,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   199,   199,   200,   200,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   202,   202,   203,   203,   203,   204,   204,   204,   205,
     205,   205,   206,   206,   207,   208,   209,   210,   210,   210,
     211,   211,   211,   212,   212,   212,   212,   213,   213,   213,
     213,   214,   214,   215,   215,   215,   216,   216,   217,   217,
     217,   218,   218,   219,   219,   220,   220,   221,   221,   222,
     222,   223,   224,   224,   225,   225,   226,   226,   226,   227,
     227,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   237,   238,   238,   239,   239,   240,   240,   241,
     241,   242,   242,   243,   243,   244,   244,   245,   245,   246,
     246,   247,   247,   248,   248,   249,   249,   250,   250,   251,
     251,   252,   252,   253,   254,   255,   256,   256,   257,   257,
     258,   259,   260,   261,   262,   262,   263,   263,   264,   264,
     265,   265,   266,   267,   267,   267,   268,   269,   270,   270,
     270,   271,   272,   272,   273,   273,   274,   274,   275
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     3,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     1,     2,     3,
       3,     2,     2,     1,     1,     3,     1,     1,     2,     2,
       1,     1,     3,     1,     1,     2,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     1,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     3,     4,     1,     1,     1,     6,     3,
       3,     2,     5,     5,     4,     2,     2,     2,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     1,     6,
       3,     3,     2,     5,     5,     4,     2,     2,     2,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       3,     1,     3,     1,     2,     2,     1,     2,     2,     1,
       1,     2,     1,     1,     2,     5,     2,     1,     2,     2,
       2,     2,     1,     2,     1,     1,     1,     1,     3,     3,
       3,     1,     3,     3,     2,     1,     2,     3,     1,     3,
       2,     1,     3,     1,     3,     1,     3,     1,     3,     3,
       3,     1,     3,     3,     3,     1,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     2,     1,     2,     2,     3,     1,     2,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     2,     3,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     1,     3,     2,     3,     3,     1,     3,
       3,     1,     1,     2,     1,     2,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     282,     0,   354,     2,   351,     0,     1,     0,   282,   326,
     282,   324,   355,   353,   282,   327,   276,   279,   280,   281,
     282,   282,   222,   220,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,     0,   282,   282,   282,
     218,   219,   282,   282,   282,   226,   108,     0,   227,   282,
     228,   282,   282,   282,   282,   282,   282,   282,     0,   277,
       0,   229,   232,   221,   209,   213,   214,   212,   216,   217,
     215,   205,   233,   211,   278,   210,   223,   225,   224,   204,
     206,   230,   208,   231,   207,   282,   282,   282,     0,   282,
       5,    17,     4,   338,    31,    39,    52,    65,    88,     0,
     122,   123,   120,   132,   136,   137,   141,   145,   148,   151,
     153,   155,   135,   157,   166,   168,   174,   161,   180,   181,
     182,   183,   184,   185,   186,   187,   305,   303,   301,   299,
     297,   295,   293,   291,   179,   234,   235,   240,   247,   252,
     134,   253,   254,   255,   251,   272,   273,   309,   203,   202,
     201,   200,   199,   198,   197,   196,     0,    30,   332,   263,
     264,   282,   282,   328,   357,   325,   100,   275,   290,   289,
     288,   287,   286,   285,   284,   283,   316,   274,   315,   195,
     194,   193,   192,   191,   190,   189,   188,   102,    12,    26,
      58,    34,    33,   336,    27,    36,   331,    13,    97,    96,
     105,   320,   319,    18,     8,    14,   282,   282,   282,     0,
     127,    11,    10,   282,   282,   109,   107,     0,   282,   282,
     101,     9,   282,    24,     7,    23,     0,   242,   237,   261,
     248,   262,   243,    98,    92,   314,   120,   313,   103,     0,
     106,     0,   126,   104,   269,     0,   334,   271,   330,   282,
     267,     0,   343,     3,   342,     0,   265,     0,     0,   348,
     282,     0,   358,   282,   282,   282,   124,   282,   282,   282,
     282,   282,   121,   282,   282,   133,   282,   282,   282,   282,
     282,   146,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,     0,     0,     0,   144,   282,   150,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   241,   249,
     282,   282,   282,   356,   329,   282,   282,   282,   282,   282,
      35,   282,   282,   282,    19,    20,   130,   128,   129,   282,
       0,    99,   282,   318,   317,    16,    15,   282,   239,   244,
       0,   238,   245,   282,   282,   282,   270,     0,   322,   321,
       6,   268,   345,   347,   266,   282,   282,   282,   346,    32,
      90,    91,    53,    54,    55,    56,    57,   165,    28,    29,
     138,   282,   139,   140,   142,   143,   147,   149,   152,   154,
     160,   159,   158,   156,   164,   163,   162,   167,   177,   178,
       0,   169,   170,   236,   306,   172,   304,   302,   300,   298,
     296,   294,   292,   259,   250,   260,   258,   308,   176,   257,
     307,   256,   175,   339,    59,    60,    61,    62,    63,   337,
     340,    22,   333,    21,   282,   110,   282,   282,   282,   282,
     282,   282,     0,   282,   282,   108,   282,   282,   282,   282,
     282,   282,   282,     0,    64,    38,    46,    66,    88,     0,
     120,    25,   246,   113,    37,    95,    40,   120,     0,   282,
     111,   282,   335,   344,   350,   323,   349,     0,     0,   282,
     282,   312,   311,   125,    79,   102,    76,    75,    84,   282,
     107,     0,   101,    77,    71,   103,     0,    85,     0,    83,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,    93,   119,    94,   171,
     173,   341,   131,    78,   282,   282,   282,   282,    89,    69,
      70,    47,    48,    49,    50,    51,    41,    42,    43,    44,
      45,   112,   117,   118,     0,    95,   282,   282,   282,   282,
     282,    72,   119,    73,    68,   114,   115
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,   251,   252,    90,    91,   324,   333,   224,   201,
     275,    92,    93,   189,   190,   191,   453,   443,    94,   454,
     445,    95,   192,    96,   446,   447,    97,   214,   459,   455,
     541,   506,    98,    99,   100,   101,   209,   471,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   368,
     113,   114,   391,   394,   115,   116,   117,   167,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   392,   227,   136,   228,
     137,   138,   139,   408,   404,   230,   140,   141,   142,   143,
     245,   144,   176,   145,   146,   147,   179,   180,   181,   182,
     183,   184,   185,   186,   148,   149,   150,   151,   152,   153,
     154,   155,   411,   156,   473,   238,   187,   335,   203,   350,
     259,   260,   164,    11,   165,   247,   195,   157,   421,   248,
     196,   158,   422,   253,   254,   159,   160,   261,     3,     4,
       5,    12,    13
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -407
static const yytype_int16 yypact[] =
{
    -407,    59,  -407,  -407,  -407,   734,  -407,    44,    77,  -407,
    1377,  -407,  -407,    60,   143,  -407,  -407,  -407,  -407,  -407,
    1491,  2403,  -407,  -407,  1941,  1941,  1491,  1491,  1491,  1941,
    1941,  1941,  1717,  1941,  1941,   100,    64,  1491,  1941,  1941,
    -407,  -407,  2403,  1941,  1829,  -407,  -407,  2058,  -407,  1491,
    -407,  1491,  1941,  1491,  1491,  1491,  1941,  1491,  2334,  -407,
     170,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  1147,  -407,  1261,   914,   143,
    -407,  -407,  -407,  -407,   115,  -407,  -407,  -407,   109,   142,
      72,  -407,   128,    11,    42,  2198,    61,   134,   117,    83,
    -407,   239,   136,  -407,     5,   224,  -407,  -407,  -407,  -407,
     525,  -407,   443,  -407,  -407,  -407,   130,   139,   149,   151,
     153,   155,   179,   181,  -407,  -407,  -407,  1946,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,    51,  -407,   172,  -407,
    -407,   143,   143,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
     156,    11,  -407,  -407,  -407,  -407,   185,  -407,  -407,  -407,
    -407,  -407,    68,    23,  -407,  -407,  1941,  1941,  1941,   240,
    -407,  -407,  -407,  1491,  1491,  -407,  -407,   221,   176,   176,
    -407,  -407,  1941,  -407,  -407,  -407,  2058,  -407,  2003,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,    72,    68,  -407,   248,
    -407,   123,  -407,  -407,  -407,   227,  -407,  -407,   218,  1377,
    -407,   180,  -407,  -407,   229,   190,  -407,   194,   182,  -407,
    1377,   122,  -407,  1491,  1491,  1491,  -407,  1491,  1491,  1491,
    1491,  1491,  -407,  1903,  1903,  -407,  2274,  2274,  2274,  1791,
    1791,   525,  1903,  1903,  1903,  1903,  1903,  1903,  1903,  1903,
    1903,  1903,  1903,  1903,  1903,  2115,    61,  1903,   117,   802,
     802,   802,   802,   802,   802,   802,   802,  1775,  -407,  -407,
    2078,  2078,  1491,  -407,  -407,  1491,  1491,  1491,  1491,  1491,
    -407,  2162,  1941,  1941,  -407,  -407,  -407,  -407,  -407,  1941,
     263,  -407,  1605,  -407,  -407,  -407,  -407,   176,  -407,  -407,
    1775,  -407,  -407,  1941,  2162,  2162,  -407,  2403,  -407,    68,
    -407,  -407,  1033,  -407,  -407,    77,   143,    77,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,   148,  -407,  -407,
    2198,  1791,  2198,  2198,    61,    61,   134,   117,    83,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
     126,  -407,   242,   566,  -407,   243,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,   246,  -407,   207,  -407,  1605,  2403,  1605,  1605,
    1605,   100,    64,  1491,  2403,   305,  1605,  1605,  1941,  1491,
    1605,  1491,  1605,   307,  -407,  -407,  -407,  -407,    31,   289,
     302,  -407,  -407,  -407,  -407,  -407,  -407,   780,   309,    78,
      68,    78,  -407,  -407,  -407,  -407,  -407,  2115,   802,  1941,
    1941,  -407,  -407,  -407,  -407,   322,  -407,  -407,  -407,  1605,
     325,   290,   328,  -407,  -407,   338,   324,  -407,   138,  -407,
    1491,  1605,  1605,  1605,  1605,  1605,  1605,  1605,  1941,  1941,
    1941,  1941,  1941,  1941,  1491,  1491,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  1605,  1941,  2162,  2162,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,   341,   342,   120,   120,  1605,  1605,
    1605,  -407,   344,  -407,  -407,  -407,  -407
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -407,  -407,   -33,     1,  -407,   330,  -407,   169,  -407,   124,
     195,  -407,   -30,  -294,  -407,  -407,  -407,  1664,    43,  -175,
     -31,  -154,  -407,  -407,  -407,  -407,  -407,   -46,  -313,  -394,
    -161,  -406,   160,   192,  -407,   291,  -407,  -407,    70,    99,
    -407,   -98,  -110,   114,  -100,   113,  -223,   -26,  -407,   127,
      92,  -407,   -70,   -38,  -407,   -23,  -407,  -407,   284,   296,
     -68,   -89,   -92,   -77,   -90,   293,   -17,   -15,   -12,    -9,
       2,    24,    26,    41,   -36,  -407,  -407,  -101,  -407,  -407,
    -247,  -227,   -27,    96,    71,  -121,   678,  -407,  -407,  -407,
    -407,   -34,   -57,   -18,  -407,     0,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,    -6,   -40,  -166,   257,  -407,
    -210,   403,   -55,   413,   261,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,   340,  -407,
    -407,  -407,  -407
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -353
static const yytype_int16 yytable[] =
{
       2,   246,   220,   177,   168,     9,   169,   217,     9,   170,
     296,    89,   171,   232,   163,   282,   309,   281,   216,   284,
     229,   178,   298,   172,   177,   168,   249,   169,   297,   202,
     170,   283,   461,   171,   262,   215,   308,   280,   202,   202,
     177,   168,   178,   169,   172,   173,   170,   174,   393,   171,
     458,   458,   237,   336,   257,   508,   276,   -67,   322,     6,
     172,   379,   175,   166,   310,   383,   173,    16,   174,   198,
     199,   200,   395,   395,   395,   395,   395,   395,   395,   395,
      22,  -309,   173,   175,   174,     8,     2,   323,     9,   163,
     273,   274,   233,   504,   234,   277,   239,   240,   241,   175,
     243,    71,   210,   232,    32,   339,   313,   342,   505,   531,
     229,    79,    80,   362,   363,   364,   365,   366,    14,   311,
     491,   535,   236,   193,   193,   338,   242,   341,   193,   193,
     193,   508,   193,   193,   161,   539,  -309,   193,   193,    56,
      59,   344,   193,   193,   345,   464,    63,   466,   188,   197,
     540,    82,   162,    84,   204,   205,   516,   211,   212,   517,
      32,   163,   163,    74,   213,    61,    45,   221,   225,   374,
     375,   451,    48,    16,    50,    17,    18,    19,   370,   372,
     373,   263,   377,   330,   297,   297,   297,   297,   297,   297,
     289,    81,   232,    83,   232,    56,    58,   298,   264,   229,
     265,   229,   285,   536,   537,   299,   267,   281,   371,   371,
     371,   322,   235,   290,   300,   268,   269,   291,   334,   334,
     393,   283,   458,   458,   301,   286,   302,   356,   303,   287,
     304,   270,   271,   359,   315,    76,    77,  -310,    85,    78,
      86,   395,    87,   316,   317,   312,    59,   367,   367,   349,
     348,   293,   357,   358,   305,    88,   306,   331,   321,   318,
     319,   296,   396,   397,   398,   399,   400,   401,   402,    74,
     388,   389,   309,   406,   407,   407,   326,   327,   328,   297,
     403,   329,   413,   332,   282,   281,   281,   294,   284,   343,
     462,   347,  -310,   285,   272,   218,   219,   346,   351,   295,
     283,   465,   280,   352,   280,   280,   406,   360,   361,   353,
     410,   410,   355,   403,   354,   425,   286,   467,   468,   469,
     287,   193,    62,   526,   527,   528,   529,   530,   470,   177,
     168,   -87,   169,   490,    32,   170,    72,   334,   171,   362,
     363,   364,   365,   366,   460,   460,   337,   492,   -81,   172,
     503,   -86,   514,   463,   -80,     9,   163,     9,   414,   415,
     416,   417,   418,   229,   -82,   515,   229,   538,   -74,    56,
    -116,   173,   325,   174,   223,   444,   543,   380,   381,   382,
     493,   384,   385,   386,   387,   479,   320,   475,   175,   494,
     495,   266,   420,   423,   482,   376,   378,   509,   292,   424,
     278,   369,   450,   481,   288,   496,   497,   412,    10,   177,
     168,   452,   169,   457,   480,   170,   177,   168,   171,   169,
     419,    15,   170,   314,   472,   171,   255,   178,   272,   172,
     510,   215,   485,     0,   178,     0,   172,     0,   237,     0,
       0,     0,     0,   193,   193,     0,    16,     0,    17,    18,
      19,   173,     0,   174,     0,     0,  -282,     0,   173,   507,
     174,   507,   521,   522,   523,   524,   525,     0,   175,   166,
       0,   198,   199,   200,     0,   175,     0,     0,     0,   233,
     234,     0,   486,   240,   488,   243,     0,     0,     0,     0,
       0,     0,   448,     0,    47,     0,   450,     0,   450,   450,
     450,     0,     0,   456,     0,     0,   450,   450,   236,     0,
     450,  -282,   450,    58,     0,     0,   460,   460,     0,    59,
       0,     0,   331,     0,   449,     0,     0,     0,    16,     0,
      17,    18,    19,   518,   360,   361,   542,   542,  -282,   511,
     512,     0,    74,     0,     0,     0,     0,   532,   533,   450,
       0,     0,     0,     0,     0,    85,     0,   444,     0,    87,
       0,   450,   450,   450,   450,   450,   450,   450,   457,   457,
     457,   457,   457,   457,     0,     0,    47,     0,     0,     0,
       0,   518,   532,   533,   450,   457,   448,     0,   448,   448,
     448,     0,     0,  -282,     0,    58,   448,   448,   235,     0,
     448,    59,   448,     0,     0,     0,     0,     0,   450,   450,
     450,     0,     0,     0,     0,   193,   193,     0,   449,     0,
     449,   449,   449,     0,    74,     0,    76,    77,   449,   449,
      78,     0,   449,     0,   449,     0,    58,    85,     0,   448,
       0,    87,   272,     0,     0,     0,     0,     0,     0,   272,
     307,   448,   448,   448,   448,   448,   448,   448,   456,   456,
     456,   456,   456,   456,     0,     0,     0,     0,     0,     0,
       0,   449,     0,     0,   448,   456,     0,     0,    85,     0,
      86,     0,    87,   449,   449,   449,   449,   449,   449,   449,
       0,     0,     0,     0,     0,    88,     0,     0,   448,   448,
     448,     0,   194,   194,     0,     0,   449,   194,   194,   194,
       0,   194,   194,     0,     0,     0,   194,   194,     0,     0,
       0,   194,   194,     0,     0,   231,     0,     0,     0,     0,
     449,   449,   449,     0,  -352,     7,     0,  -282,     0,  -282,
    -282,  -282,     8,     0,     0,     0,     0,  -282,     0,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,     0,  -282,  -282,  -282,  -282,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,  -282,  -282,     0,     0,     0,  -282,
    -282,  -282,  -282,  -282,     0,  -282,     0,  -282,     0,  -282,
       0,  -282,  -282,  -282,  -282,    16,     0,    17,    18,    19,
    -282,  -282,    32,     0,     0,   231,  -282,  -282,     0,  -282,
    -282,     0,     0,     0,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,     0,     0,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,    56,  -282,     0,
    -282,     0,     0,  -352,     0,     0,     0,     0,   498,     0,
       0,     0,     0,  -282,     0,     0,     0,   499,   500,     0,
       0,     0,    58,     0,     0,     0,     0,     0,    59,     0,
       0,     0,     0,   501,   502,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,    74,     0,     0,   231,     0,   231,     0,     0,     0,
       0,     0,     0,     0,    85,   258,     0,  -282,    87,  -282,
    -282,  -282,     8,     0,     0,     0,     0,  -282,     0,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,     0,  -282,  -282,  -282,  -282,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,  -282,  -282,     0,     0,     0,  -282,
    -282,  -282,  -282,  -282,     0,  -282,     0,  -282,     0,  -282,
       0,  -282,  -282,  -282,  -282,   405,     0,     0,   409,   409,
    -282,  -282,     0,     0,     0,     0,  -282,  -282,     0,  -282,
    -282,     0,     0,     0,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,     0,   405,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,     0,  -282,     0,
    -282,     0,     0,     0,     0,     0,    16,     0,    17,    18,
      19,     0,     0,  -282,     0,     0,  -282,     0,     0,    20,
      21,     0,    22,    23,     0,    24,    25,    26,    27,     0,
      28,    29,    30,     0,    31,    32,    33,    34,   231,    35,
      36,   231,    37,    38,     0,    39,    40,    41,    42,     0,
      43,    44,    45,    46,    47,     0,     0,     0,    48,    49,
      50,    51,    52,     0,    53,     0,    54,     0,    55,     0,
      56,  -282,    57,    58,     0,     0,     0,     0,     0,    59,
      60,     0,     0,     0,     0,    61,    62,     0,    63,    64,
       0,     0,     0,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,     0,    78,    79,
      80,    81,    82,    83,    84,    85,     0,    86,     0,    87,
      16,     0,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,    88,    20,    21,     0,    22,    23,     0,    24,
      25,    26,    27,     0,    28,    29,    30,     0,    31,    32,
      33,    34,     0,    35,    36,     0,    37,    38,     0,    39,
      40,    41,    42,     0,    43,    44,    45,    46,    47,     0,
       0,     0,    48,    49,    50,    51,    52,     0,    53,     0,
      54,     0,    55,     0,    56,     0,    57,    58,     0,     0,
       0,     0,     0,    59,    60,     0,     0,     0,     0,    61,
      62,     0,    63,    64,     0,     0,     0,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
       0,     0,    78,    79,    80,    81,    82,    83,    84,    85,
       0,    86,     0,    87,    16,   250,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,    88,    20,    21,     0,
      22,    23,     0,    24,    25,    26,    27,     0,    28,    29,
      30,     0,    31,    32,    33,    34,     0,    35,    36,     0,
      37,    38,     0,    39,    40,    41,    42,     0,    43,    44,
      45,    46,    47,     0,     0,     0,    48,    49,    50,    51,
      52,     0,    53,     0,    54,     0,    55,     0,    56,     0,
      57,    58,     0,     0,     0,     0,     0,    59,    60,     0,
       0,     0,     0,    61,    62,     0,    63,    64,     0,     0,
       0,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,     0,    78,    79,    80,    81,
      82,    83,    84,    85,     0,    86,     0,    87,     0,     0,
      16,   256,    17,    18,    19,     0,     0,     0,     0,     0,
      88,     0,     0,    20,    21,     0,    22,    23,     0,    24,
      25,    26,    27,     0,    28,    29,    30,     0,    31,    32,
      33,    34,     0,    35,    36,     0,    37,    38,     0,    39,
      40,    41,    42,     0,    43,    44,    45,    46,    47,     0,
       0,     0,    48,    49,    50,    51,    52,     0,    53,     0,
      54,     0,    55,     0,    56,     0,    57,    58,     0,     0,
       0,     0,     0,    59,    60,     0,     0,     0,     0,    61,
      62,     0,    63,    64,     0,     0,     0,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
       0,     0,    78,    79,    80,    81,    82,    83,    84,    85,
       0,    86,     0,    87,    16,     0,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,    88,    20,    21,     0,
      22,    23,     0,     0,     0,    26,    27,     0,    28,     0,
       0,     0,     0,    32,     0,     0,     0,    35,    36,     0,
      37,     0,     0,     0,    40,    41,    42,     0,     0,     0,
      45,    46,    47,     0,     0,     0,    48,    49,    50,    51,
      52,     0,    53,     0,    54,     0,    55,     0,    56,     0,
      57,    58,     0,     0,     0,     0,     0,    59,     0,     0,
       0,     0,     0,    61,    62,     0,    63,    64,     0,     0,
       0,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,     0,    78,    79,    80,    81,
      82,    83,    84,    85,     0,    86,     0,    87,    16,     0,
      17,    18,    19,     0,     0,     0,     0,     0,     0,     0,
      88,   426,   427,     0,    22,    23,     0,     0,     0,   428,
     429,     0,   430,     0,     0,     0,     0,    32,     0,     0,
       0,   431,   432,     0,   433,     0,     0,     0,    40,    41,
     434,     0,     0,     0,    45,   435,    47,     0,     0,     0,
      48,   436,    50,   437,   438,     0,   439,     0,   440,     0,
     441,     0,    56,     0,   442,    58,     0,     0,     0,     0,
       0,    59,     0,     0,     0,     0,     0,    61,    62,     0,
      63,    64,     0,     0,     0,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,     0,     0,
      78,    79,    80,    81,    82,    83,    84,    85,     0,    86,
      16,    87,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    88,     0,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     206,   207,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,     0,     0,   208,     0,    45,     0,    47,     0,
       0,     0,    48,     0,    50,     0,     0,     0,    16,     0,
      17,    18,    19,     0,     0,     0,     0,    58,     0,     0,
       0,     0,     0,    59,    16,     0,    17,    18,    19,    61,
      62,     0,    63,    64,     0,     0,     0,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
       0,     0,    78,    79,    80,    81,    82,    83,    84,    85,
       0,    86,    16,    87,    17,    18,    19,     0,     0,     0,
       0,     0,    47,     0,     0,    58,    88,     0,    22,    23,
       0,    59,     0,     0,     0,     0,     0,    29,     0,     0,
       0,    58,     0,     0,     0,     0,     0,    59,     0,   222,
       0,     0,    40,    41,    74,     0,     0,     0,    45,     0,
      47,     0,     0,     0,    48,     0,    50,    85,     0,    86,
      74,    87,    76,    77,     0,     0,    78,     0,     0,    58,
       0,     0,     0,    85,    88,    59,    16,    87,    17,    18,
      19,    61,    62,     0,    63,    64,     0,     0,     0,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,     0,     0,    78,    79,    80,    81,    82,    83,
      84,    85,     0,    86,    16,    87,    17,    18,    19,    16,
       0,    17,    18,    19,    47,     0,     0,     0,    88,     0,
      22,    23,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    58,     0,     0,     0,     0,     0,    59,
       0,     0,     0,     0,    40,    41,     0,     0,     0,     0,
      45,     0,    47,     0,     0,     0,    48,   226,    50,     0,
       0,     0,    74,     0,     0,     0,    16,     0,    17,    18,
      19,    58,     0,     0,     0,    85,    58,    59,     0,    87,
       0,     0,    59,    61,    62,     0,    63,    64,     0,     0,
     307,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,    74,    78,    79,    80,    81,
      82,    83,    84,    85,   226,    86,     0,    87,    85,     0,
      86,    16,    87,    17,    18,    19,     0,     0,     0,     0,
      88,     0,     0,    58,     0,    88,     0,     0,     0,    59,
       0,    16,     0,    17,    18,    19,     0,   340,     0,     0,
     474,     0,   476,   477,   478,     0,     0,     0,     0,     0,
     483,   484,    74,     0,   487,     0,   489,     0,     0,   226,
       0,     0,     0,     0,     0,    85,     0,    86,    16,    87,
      17,    18,    19,     0,     0,     0,     0,     0,    58,    47,
       0,     0,    88,     0,    59,     0,     0,     0,     0,     0,
       0,     0,     0,   513,     0,     0,     0,     0,    58,     0,
       0,     0,     0,     0,    59,   519,   520,    74,     0,     0,
       0,     0,     0,     0,     0,    16,   390,    17,    18,    19,
      85,     0,    86,     0,    87,     0,     0,    74,   534,     0,
       0,    22,    23,     0,     0,    58,     0,    88,     0,     0,
      85,    59,    86,     0,    87,     0,     0,     0,     0,     0,
       0,     0,   544,   545,   546,    40,    41,    88,     0,     0,
       0,    45,     0,    47,    74,     0,     0,    48,    23,    50,
       0,     0,     0,     0,     0,     0,     0,    85,     0,     0,
       0,    87,    58,     0,     0,     0,   279,     0,    59,     0,
       0,    40,    41,     0,    61,    62,     0,    63,    64,     0,
       0,     0,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,     0,     0,    78,    79,    80,
      81,    82,    83,    84,    85,     0,     0,    16,    87,    17,
      18,    19,     0,     0,    64,     0,     0,     0,    65,    66,
      67,    68,    69,    70,    23,     0,    73,     0,    75,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,    41,     0,
       0,     0,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    16,     0,     0,
       0,     0,     0,     0,    58,     0,     0,     0,     0,     0,
      59,     0,     0,    22,    23,     0,     0,     0,     0,     0,
      64,     0,     0,     0,    65,    66,    67,    68,    69,    70,
       0,     0,    73,    74,    75,    76,    77,    40,    41,    78,
       0,     0,     0,    45,     0,     0,    85,     0,     0,    48,
      87,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   244,     0,    16,     0,     0,     0,
      59,     0,     0,     0,     0,     0,    61,    62,     0,    63,
      64,     0,    22,    23,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,     0,     0,    78,
      79,    80,    81,    82,    83,    84,    40,    41,     0,     0,
       0,     0,    45,     0,     0,     0,     0,     0,    48,     0,
      50,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
       0,     0,     0,     0,     0,    61,    62,     0,    63,    64,
       0,     0,     0,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,     0,    78,    79,
      80,    81,    82,    83,    84
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-407)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,    58,    42,    21,    21,     5,    21,    37,     8,    21,
     120,    10,    21,    47,    14,   107,   137,   106,    36,   109,
      47,    21,   122,    21,    42,    42,    60,    42,   120,    29,
      42,   108,   345,    42,    89,    35,   137,   105,    38,    39,
      58,    58,    42,    58,    42,    21,    58,    21,   295,    58,
     344,   345,    52,   219,    87,   461,    14,    26,    35,     0,
      58,   284,    21,    20,    13,   288,    42,     3,    42,    26,
      27,    28,   299,   300,   301,   302,   303,   304,   305,   306,
      19,    13,    58,    42,    58,     8,    86,    64,    88,    89,
      79,    80,    49,    15,    51,    53,    53,    54,    55,    58,
      57,    96,    32,   137,    32,   226,   161,   228,    30,   503,
     137,   106,   107,   267,   268,   269,   270,   271,    74,    68,
      89,   515,    52,    24,    25,   226,    56,   228,    29,    30,
      31,   537,    33,    34,    74,    15,    68,    38,    39,    67,
      76,    18,    43,    44,    21,   355,    85,   357,    24,    25,
      30,   109,     9,   111,    30,    31,    18,    33,    34,    21,
      32,   161,   162,    99,    64,    82,    49,    43,    44,   279,
     280,   337,    55,     3,    57,     5,     6,     7,   276,   277,
     278,    66,   282,   213,   276,   277,   278,   279,   280,   281,
      54,   108,   226,   110,   228,    67,    70,   297,    89,   226,
      58,   228,    54,   516,   517,    75,    78,   296,   276,   277,
     278,    35,    52,    77,    75,    87,    88,    81,   218,   219,
     467,   298,   516,   517,    75,    77,    75,   260,    75,    81,
      75,   103,   104,   263,    78,   101,   102,    13,   112,   105,
     114,   468,   116,    87,    88,    73,    76,   273,   274,   249,
     249,    27,   130,   131,    75,   129,    75,   214,    73,   103,
     104,   371,   300,   301,   302,   303,   304,   305,   306,    99,
     293,   294,   393,   307,   310,   311,   206,   207,   208,   371,
     307,    41,   312,    62,   376,   374,   375,    63,   378,    41,
     347,    73,    68,    54,   102,    38,    39,    70,   118,    75,
     377,   356,   370,    74,   372,   373,   340,   264,   265,   119,
     310,   311,   130,   340,   120,    52,    77,    75,    75,    73,
      81,   222,    83,   498,   499,   500,   501,   502,   121,   347,
     347,    26,   347,    26,    32,   347,    97,   337,   347,   493,
     494,   495,   496,   497,   344,   345,   222,    58,    26,   347,
      41,    26,    62,   352,    26,   355,   356,   357,   315,   316,
     317,   318,   319,   390,    26,    41,   393,    26,    26,    67,
      26,   347,   203,   347,    44,   332,   537,   285,   286,   287,
      78,   289,   290,   291,   292,   431,   191,   427,   347,    87,
      88,   100,   322,   323,   434,   281,   283,   467,   114,   329,
     104,   274,   332,   433,   111,   103,   104,   311,     5,   427,
     427,   340,   427,   343,   432,   427,   434,   434,   427,   434,
     321,     8,   434,   162,   424,   434,    86,   427,   236,   427,
     468,   431,   438,    -1,   434,    -1,   434,    -1,   438,    -1,
      -1,    -1,    -1,   344,   345,    -1,     3,    -1,     5,     6,
       7,   427,    -1,   427,    -1,    -1,    13,    -1,   434,   459,
     434,   461,   493,   494,   495,   496,   497,    -1,   427,   426,
      -1,   428,   429,   430,    -1,   434,    -1,    -1,    -1,   436,
     437,    -1,   439,   440,   441,   442,    -1,    -1,    -1,    -1,
      -1,    -1,   332,    -1,    51,    -1,   426,    -1,   428,   429,
     430,    -1,    -1,   343,    -1,    -1,   436,   437,   438,    -1,
     440,    68,   442,    70,    -1,    -1,   516,   517,    -1,    76,
      -1,    -1,   479,    -1,   332,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,   490,   491,   492,   536,   537,    13,   469,
     470,    -1,    99,    -1,    -1,    -1,    -1,   504,   505,   479,
      -1,    -1,    -1,    -1,    -1,   112,    -1,   514,    -1,   116,
      -1,   491,   492,   493,   494,   495,   496,   497,   498,   499,
     500,   501,   502,   503,    -1,    -1,    51,    -1,    -1,    -1,
      -1,   538,   539,   540,   514,   515,   426,    -1,   428,   429,
     430,    -1,    -1,    68,    -1,    70,   436,   437,   438,    -1,
     440,    76,   442,    -1,    -1,    -1,    -1,    -1,   538,   539,
     540,    -1,    -1,    -1,    -1,   516,   517,    -1,   426,    -1,
     428,   429,   430,    -1,    99,    -1,   101,   102,   436,   437,
     105,    -1,   440,    -1,   442,    -1,    70,   112,    -1,   479,
      -1,   116,   450,    -1,    -1,    -1,    -1,    -1,    -1,   457,
      84,   491,   492,   493,   494,   495,   496,   497,   498,   499,
     500,   501,   502,   503,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   479,    -1,    -1,   514,   515,    -1,    -1,   112,    -1,
     114,    -1,   116,   491,   492,   493,   494,   495,   496,   497,
      -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,   538,   539,
     540,    -1,    24,    25,    -1,    -1,   514,    29,    30,    31,
      -1,    33,    34,    -1,    -1,    -1,    38,    39,    -1,    -1,
      -1,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,    -1,
     538,   539,   540,    -1,     0,     1,    -1,     3,    -1,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    -1,    19,    20,    -1,    22,    23,    24,    25,
      -1,    27,    28,    29,    -1,    31,    32,    33,    34,    -1,
      36,    37,    -1,    39,    40,    -1,    42,    43,    44,    45,
      -1,    47,    48,    49,    50,    51,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    -1,    61,    -1,    63,    -1,    65,
      -1,    67,    68,    69,    70,     3,    -1,     5,     6,     7,
      76,    77,    32,    -1,    -1,   137,    82,    83,    -1,    85,
      86,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,    -1,    -1,   105,
     106,   107,   108,   109,   110,   111,   112,    67,   114,    -1,
     116,    -1,    -1,   119,    -1,    -1,    -1,    -1,    78,    -1,
      -1,    -1,    -1,   129,    -1,    -1,    -1,    87,    88,    -1,
      -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,
      -1,    -1,    -1,   103,   104,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     222,    99,    -1,    -1,   226,    -1,   228,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,     1,    -1,     3,   116,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    -1,    19,    20,    -1,    22,    23,    24,    25,
      -1,    27,    28,    29,    -1,    31,    32,    33,    34,    -1,
      36,    37,    -1,    39,    40,    -1,    42,    43,    44,    45,
      -1,    47,    48,    49,    50,    51,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    -1,    61,    -1,    63,    -1,    65,
      -1,    67,    68,    69,    70,   307,    -1,    -1,   310,   311,
      76,    77,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,
      86,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,    -1,   340,   105,
     106,   107,   108,   109,   110,   111,   112,    -1,   114,    -1,
     116,    -1,    -1,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,    -1,    -1,   129,    -1,    -1,    13,    -1,    -1,    16,
      17,    -1,    19,    20,    -1,    22,    23,    24,    25,    -1,
      27,    28,    29,    -1,    31,    32,    33,    34,   390,    36,
      37,   393,    39,    40,    -1,    42,    43,    44,    45,    -1,
      47,    48,    49,    50,    51,    -1,    -1,    -1,    55,    56,
      57,    58,    59,    -1,    61,    -1,    63,    -1,    65,    -1,
      67,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    76,
      77,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,    86,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,    -1,    -1,   105,   106,
     107,   108,   109,   110,   111,   112,    -1,   114,    -1,   116,
       3,    -1,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,    16,    17,    -1,    19,    20,    -1,    22,
      23,    24,    25,    -1,    27,    28,    29,    -1,    31,    32,
      33,    34,    -1,    36,    37,    -1,    39,    40,    -1,    42,
      43,    44,    45,    -1,    47,    48,    49,    50,    51,    -1,
      -1,    -1,    55,    56,    57,    58,    59,    -1,    61,    -1,
      63,    -1,    65,    -1,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,    82,
      83,    -1,    85,    86,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,   112,
      -1,   114,    -1,   116,     3,   118,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    16,    17,    -1,
      19,    20,    -1,    22,    23,    24,    25,    -1,    27,    28,
      29,    -1,    31,    32,    33,    34,    -1,    36,    37,    -1,
      39,    40,    -1,    42,    43,    44,    45,    -1,    47,    48,
      49,    50,    51,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    -1,    61,    -1,    63,    -1,    65,    -1,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,
      -1,    -1,    -1,    82,    83,    -1,    85,    86,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    -1,    -1,   105,   106,   107,   108,
     109,   110,   111,   112,    -1,   114,    -1,   116,    -1,    -1,
       3,   120,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    16,    17,    -1,    19,    20,    -1,    22,
      23,    24,    25,    -1,    27,    28,    29,    -1,    31,    32,
      33,    34,    -1,    36,    37,    -1,    39,    40,    -1,    42,
      43,    44,    45,    -1,    47,    48,    49,    50,    51,    -1,
      -1,    -1,    55,    56,    57,    58,    59,    -1,    61,    -1,
      63,    -1,    65,    -1,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,    82,
      83,    -1,    85,    86,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,   112,
      -1,   114,    -1,   116,     3,    -1,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    16,    17,    -1,
      19,    20,    -1,    -1,    -1,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    36,    37,    -1,
      39,    -1,    -1,    -1,    43,    44,    45,    -1,    -1,    -1,
      49,    50,    51,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    -1,    61,    -1,    63,    -1,    65,    -1,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    82,    83,    -1,    85,    86,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    -1,    -1,   105,   106,   107,   108,
     109,   110,   111,   112,    -1,   114,    -1,   116,     3,    -1,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     129,    16,    17,    -1,    19,    20,    -1,    -1,    -1,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    32,    -1,    -1,
      -1,    36,    37,    -1,    39,    -1,    -1,    -1,    43,    44,
      45,    -1,    -1,    -1,    49,    50,    51,    -1,    -1,    -1,
      55,    56,    57,    58,    59,    -1,    61,    -1,    63,    -1,
      65,    -1,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,
      85,    86,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,    -1,    -1,
     105,   106,   107,   108,   109,   110,   111,   112,    -1,   114,
       3,   116,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    -1,    19,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    -1,    -1,    47,    -1,    49,    -1,    51,    -1,
      -1,    -1,    55,    -1,    57,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,    -1,    -1,    -1,    -1,    70,    -1,    -1,
      -1,    -1,    -1,    76,     3,    -1,     5,     6,     7,    82,
      83,    -1,    85,    86,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,   112,
      -1,   114,     3,   116,     5,     6,     7,    -1,    -1,    -1,
      -1,    -1,    51,    -1,    -1,    70,   129,    -1,    19,    20,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    28,    -1,    -1,
      -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,    40,
      -1,    -1,    43,    44,    99,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    55,    -1,    57,   112,    -1,   114,
      99,   116,   101,   102,    -1,    -1,   105,    -1,    -1,    70,
      -1,    -1,    -1,   112,   129,    76,     3,   116,     5,     6,
       7,    82,    83,    -1,    85,    86,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,    -1,    -1,   105,   106,   107,   108,   109,   110,
     111,   112,    -1,   114,     3,   116,     5,     6,     7,     3,
      -1,     5,     6,     7,    51,    -1,    -1,    -1,   129,    -1,
      19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,
      49,    -1,    51,    -1,    -1,    -1,    55,    51,    57,    -1,
      -1,    -1,    99,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,    70,    -1,    -1,    -1,   112,    70,    76,    -1,   116,
      -1,    -1,    76,    82,    83,    -1,    85,    86,    -1,    -1,
      84,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    -1,    99,   105,   106,   107,   108,
     109,   110,   111,   112,    51,   114,    -1,   116,   112,    -1,
     114,     3,   116,     5,     6,     7,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    70,    -1,   129,    -1,    -1,    -1,    76,
      -1,     3,    -1,     5,     6,     7,    -1,    84,    -1,    -1,
     426,    -1,   428,   429,   430,    -1,    -1,    -1,    -1,    -1,
     436,   437,    99,    -1,   440,    -1,   442,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,   112,    -1,   114,     3,   116,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    70,    51,
      -1,    -1,   129,    -1,    76,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   479,    -1,    -1,    -1,    -1,    70,    -1,
      -1,    -1,    -1,    -1,    76,   491,   492,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    51,     5,     6,     7,
     112,    -1,   114,    -1,   116,    -1,    -1,    99,   514,    -1,
      -1,    19,    20,    -1,    -1,    70,    -1,   129,    -1,    -1,
     112,    76,   114,    -1,   116,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   538,   539,   540,    43,    44,   129,    -1,    -1,
      -1,    49,    -1,    51,    99,    -1,    -1,    55,    20,    57,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,    -1,    -1,
      -1,   116,    70,    -1,    -1,    -1,    38,    -1,    76,    -1,
      -1,    43,    44,    -1,    82,    83,    -1,    85,    86,    -1,
      -1,    -1,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,    -1,    -1,   105,   106,   107,
     108,   109,   110,   111,   112,    -1,    -1,     3,   116,     5,
       6,     7,    -1,    -1,    86,    -1,    -1,    -1,    90,    91,
      92,    93,    94,    95,    20,    -1,    98,    -1,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      -1,    -1,    98,    99,   100,   101,   102,    43,    44,   105,
      -1,    -1,    -1,    49,    -1,    -1,   112,    -1,    -1,    55,
     116,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,     3,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,
      86,    -1,    19,    20,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,    -1,    -1,   105,
     106,   107,   108,   109,   110,   111,    43,    44,    -1,    -1,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,    86,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,    -1,    -1,   105,   106,
     107,   108,   109,   110,   111
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   134,   228,   271,   272,   273,     0,     1,     8,   228,
     254,   256,   274,   275,    74,   256,     3,     5,     6,     7,
      16,    17,    19,    20,    22,    23,    24,    25,    27,    28,
      29,    31,    32,    33,    34,    36,    37,    39,    40,    42,
      43,    44,    45,    47,    48,    49,    50,    51,    55,    56,
      57,    58,    59,    61,    63,    65,    67,    69,    70,    76,
      77,    82,    83,    85,    86,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   105,   106,
     107,   108,   109,   110,   111,   112,   114,   116,   129,   136,
     137,   138,   144,   145,   151,   154,   156,   159,   165,   166,
     167,   168,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   183,   184,   187,   188,   189,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   211,   213,   214,   215,
     219,   220,   221,   222,   224,   226,   227,   228,   237,   238,
     239,   240,   241,   242,   243,   244,   246,   260,   264,   268,
     269,    74,     9,   228,   255,   257,   151,   190,   199,   200,
     201,   202,   203,   204,   205,   206,   225,   226,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   249,   142,   146,
     147,   148,   155,   172,   219,   259,   263,   142,   151,   151,
     151,   142,   228,   251,   142,   142,    33,    34,    47,   169,
     171,   142,   142,    64,   160,   228,   226,   145,   251,   251,
     249,   142,    40,   138,   141,   142,    51,   210,   212,   215,
     218,   219,   224,   151,   151,   165,   171,   228,   248,   151,
     151,   151,   171,   151,    70,   223,   225,   258,   262,   224,
     118,   135,   136,   266,   267,   271,   120,   135,     1,   253,
     254,   270,   255,    66,    89,    58,   168,    78,    87,    88,
     103,   104,   166,    79,    80,   143,    14,    53,   192,    38,
     193,   194,   195,   196,   197,    54,    77,    81,   198,    54,
      77,    81,   191,    27,    63,    75,   175,   195,   177,    75,
      75,    75,    75,    75,    75,    75,    75,    84,   210,   218,
      13,    68,    73,   255,   257,    78,    87,    88,   103,   104,
     143,    73,    35,    64,   139,   140,   171,   171,   171,    41,
     145,   151,    62,   140,   228,   250,   250,   142,   210,   218,
      84,   210,   218,    41,    18,    21,    70,    73,   136,   228,
     252,   118,    74,   119,   120,   130,   135,   130,   131,   145,
     151,   151,   154,   154,   154,   154,   154,   180,   182,   182,
     174,   193,   174,   174,   175,   175,   176,   177,   178,   179,
     183,   183,   183,   179,   183,   183,   183,   183,   188,   188,
      51,   185,   209,   213,   186,   214,   186,   186,   186,   186,
     186,   186,   186,   215,   217,   219,   224,   207,   216,   219,
     228,   245,   216,   145,   151,   151,   151,   151,   151,   172,
     171,   261,   265,   171,   171,    52,    16,    17,    24,    25,
      27,    36,    37,    39,    45,    50,    56,    58,    59,    61,
      63,    65,    69,   150,   151,   153,   157,   158,   165,   166,
     171,   250,   217,   149,   152,   162,   165,   171,   146,   161,
     228,   161,   225,   136,   253,   255,   253,    75,    75,    73,
     121,   170,   228,   247,   150,   249,   150,   150,   150,   160,
     226,   145,   249,   150,   150,   248,   151,   150,   151,   150,
      26,    89,    58,    78,    87,    88,   103,   104,    78,    87,
      88,   103,   104,    41,    15,    30,   164,   228,   164,   185,
     186,   171,   171,   150,    62,    41,    18,    21,   151,   150,
     150,   153,   153,   153,   153,   153,   152,   152,   152,   152,
     152,   162,   151,   151,   150,   162,   161,   161,    26,    15,
      30,   163,   228,   163,   150,   150,   150
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YYUSE (yytype);
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1787 of yacc.c  */
#line 437 "aldor/aldor/src/axl.y"
    { yypval = (yyval.ab) = (yyvsp[(1) - (1)].ab); }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 448 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewLabel(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 453 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewMacro(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 455 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewExtend(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 9:
/* Line 1787 of yacc.c  */
#line 457 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewLocal(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 459 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewFree(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 461 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewFluid(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 463 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDefault(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 465 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDDefine(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 467 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewFix(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 15:
/* Line 1787 of yacc.c  */
#line 469 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInline(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 16:
/* Line 1787 of yacc.c  */
#line 471 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewImport(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 477 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewExport(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab), abZip,abZip); }
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 479 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewExport(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 20:
/* Line 1787 of yacc.c  */
#line 481 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewExport(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),(yyvsp[(3) - (3)].ab),abZip); }
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 486 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(2) - (2)].ab); }
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 491 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(2) - (2)].ab); }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 498 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewImport(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 508 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(2) - (2)].ab); }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 510 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewHide(TPOS((yyvsp[(1) - (2)].tok)), (yyvsp[(2) - (2)].ab)); }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 521 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewWhere(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(3) - (3)].ab),(yyvsp[(1) - (3)].ab)); }
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 532 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDeclare(APOS((yyvsp[(1) - (2)].ab)),(yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 556 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAssign(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 558 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 560 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewMDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 562 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 564 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 569 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAssign(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 571 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 49:
/* Line 1787 of yacc.c  */
#line 573 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewMDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 50:
/* Line 1787 of yacc.c  */
#line 575 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 51:
/* Line 1787 of yacc.c  */
#line 577 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 582 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAssign(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 54:
/* Line 1787 of yacc.c  */
#line 584 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 586 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewMDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 588 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 590 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 59:
/* Line 1787 of yacc.c  */
#line 597 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAssign(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 599 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 601 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewMDefine(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 603 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 605 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPLambda(APOS((yyvsp[(1) - (3)].ab)),(yyvsp[(1) - (3)].ab),abZip,(yyvsp[(3) - (3)].ab)); }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 611 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewIf(TPOS((yyvsp[(1) - (4)].tok)), TEST((yyvsp[(2) - (4)].ab)),(yyvsp[(4) - (4)].ab),abZip); }
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 623 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewIf(TPOS((yyvsp[(1) - (6)].tok)), TEST((yyvsp[(2) - (6)].ab)),(yyvsp[(4) - (6)].ab),(yyvsp[(6) - (6)].ab)); }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 625 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewExit(TPOS((yyvsp[(2) - (3)].tok)), TEST((yyvsp[(1) - (3)].ab)),(yyvsp[(3) - (3)].ab)); }
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 627 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRepeatOL(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(3) - (3)].ab),(yyvsp[(1) - (3)].ablist)); listFree(AbSyn)((yyvsp[(1) - (3)].ablist)); }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 629 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRepeat0(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 631 "aldor/aldor/src/axl.y"
    { (void)parseDeprecated(_YY_KW_But, abNewNothing(TPOS((yyvsp[(3) - (5)].tok)))); (yyval.ab) = abNewTry(TPOS((yyvsp[(1) - (5)].tok)),(yyvsp[(2) - (5)].ab),(yyvsp[(4) - (5)].ab)->abSequence.argv[0], (yyvsp[(4) - (5)].ab)->abSequence.argv[1],(yyvsp[(5) - (5)].ab)); }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 633 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewTry(TPOS((yyvsp[(1) - (5)].tok)),(yyvsp[(2) - (5)].ab),(yyvsp[(4) - (5)].ab)->abSequence.argv[0], (yyvsp[(4) - (5)].ab)->abSequence.argv[1],(yyvsp[(5) - (5)].ab)); }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 635 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewSelect(TPOS((yyvsp[(1) - (4)].tok)),(yyvsp[(2) - (4)].ab),(yyvsp[(4) - (4)].ab)); }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 637 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDo(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 639 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDelay(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 641 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewReference(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 643 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewGenerate(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 645 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAssert(TPOS((yyvsp[(1) - (2)].tok)),TEST((yyvsp[(2) - (2)].ab))); }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 647 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewIterate(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 649 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewBreak(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 651 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewReturn(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 653 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewYield(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 655 "aldor/aldor/src/axl.y"
    { (yyval.ab) = parseDeprecated(_YY_KW_Except, abNewRaise(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab))); }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 657 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRaise(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 659 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewGoto(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 661 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewNever(TPOS((yyvsp[(1) - (1)].tok))); }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 666 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewIf(TPOS((yyvsp[(1) - (6)].tok)), TEST((yyvsp[(2) - (6)].ab)),(yyvsp[(4) - (6)].ab),(yyvsp[(6) - (6)].ab)); }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 668 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewExit(TPOS((yyvsp[(2) - (3)].tok)), TEST((yyvsp[(1) - (3)].ab)),(yyvsp[(3) - (3)].ab)); }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 670 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRepeatOL(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(3) - (3)].ab),(yyvsp[(1) - (3)].ablist)); listFree(AbSyn)((yyvsp[(1) - (3)].ablist)); }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 672 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRepeat0(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 674 "aldor/aldor/src/axl.y"
    { (void)parseDeprecated(_YY_KW_But, abNewNothing(TPOS((yyvsp[(3) - (5)].tok)))); (yyval.ab) = abNewTry(TPOS((yyvsp[(1) - (5)].tok)),(yyvsp[(2) - (5)].ab),(yyvsp[(4) - (5)].ab)->abSequence.argv[0], (yyvsp[(4) - (5)].ab)->abSequence.argv[1],(yyvsp[(5) - (5)].ab)); }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 676 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewTry(TPOS((yyvsp[(1) - (5)].tok)),(yyvsp[(2) - (5)].ab),(yyvsp[(4) - (5)].ab)->abSequence.argv[0], (yyvsp[(4) - (5)].ab)->abSequence.argv[1],(yyvsp[(5) - (5)].ab)); }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 678 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewSelect(TPOS((yyvsp[(1) - (4)].tok)),(yyvsp[(2) - (4)].ab),(yyvsp[(4) - (4)].ab)); }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 680 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDo(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 682 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDelay(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 684 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewReference(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 686 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewGenerate(TPOS((yyvsp[(1) - (3)].tok)),(yyvsp[(2) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 688 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAssert(TPOS((yyvsp[(1) - (2)].tok)),TEST((yyvsp[(2) - (2)].ab))); }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 690 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewIterate(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 692 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewBreak(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 694 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewReturn(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 696 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewYield(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 698 "aldor/aldor/src/axl.y"
    { (yyval.ab) = parseDeprecated(_YY_KW_Except, abNewRaise(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab))); }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 700 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRaise(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 702 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewGoto(TPOS((yyvsp[(1) - (2)].tok)),(yyvsp[(2) - (2)].ab)); }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 704 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewNever(TPOS((yyvsp[(1) - (1)].tok))); }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 710 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(2) - (3)].ab); }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 715 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNew(AB_Sequence, APOS((yyvsp[(1) - (1)].ab)), 2, abZip, abZip); }
    break;

  case 112:
/* Line 1787 of yacc.c  */
#line 717 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNew(AB_Sequence, APOS((yyvsp[(1) - (3)].ab)), 2, (yyvsp[(1) - (3)].ab), (yyvsp[(3) - (3)].ab)); }
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 727 "aldor/aldor/src/axl.y"
    { (void)parseDeprecated(_YY_KW_Always, abNewNothing(TPOS((yyvsp[(1) - (2)].tok)))); (yyval.ab) = (yyvsp[(2) - (2)].ab); }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 729 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(2) - (2)].ab); }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 734 "aldor/aldor/src/axl.y"
    { (void)parseDeprecated(_YY_KW_Always, abNewNothing(TPOS((yyvsp[(1) - (2)].tok)))); (yyval.ab) = (yyvsp[(2) - (2)].ab); }
    break;

  case 118:
/* Line 1787 of yacc.c  */
#line 736 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(2) - (2)].ab); }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 744 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewCollectOL(APOS((yyvsp[(1) - (2)].ab)),(yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ablist)); listFree(AbSyn)((yyvsp[(2) - (2)].ablist)); }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 749 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listNReverse(AbSyn)((yyvsp[(1) - (1)].ablist)); }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 754 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(1) - (1)].ab), NULL); }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 756 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(2) - (2)].ab), (yyvsp[(1) - (2)].ablist)); }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 761 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewFor(TPOS((yyvsp[(1) - (5)].tok)),(yyvsp[(2) - (5)].ab),(yyvsp[(4) - (5)].ab),(yyvsp[(5) - (5)].ab)); }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 763 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewWhile(TPOS((yyvsp[(1) - (2)].tok)),TEST((yyvsp[(2) - (2)].ab))); }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 769 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewFree(TPOS((yyvsp[(1) - (2)].tok)), (yyvsp[(2) - (2)].ab)); }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 771 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewLocal(TPOS((yyvsp[(1) - (2)].tok)), (yyvsp[(2) - (2)].ab)); }
    break;

  case 130:
/* Line 1787 of yacc.c  */
#line 773 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewFluid(TPOS((yyvsp[(1) - (2)].tok)), (yyvsp[(2) - (2)].ab)); }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 778 "aldor/aldor/src/axl.y"
    { (yyval.ab) = TEST((yyvsp[(2) - (2)].ab)); }
    break;

  case 133:
/* Line 1787 of yacc.c  */
#line 786 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDeclare(APOS((yyvsp[(1) - (2)].ab)),(yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 799 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAnd(TPOS((yyvsp[(2) - (3)].tok)), TEST((yyvsp[(1) - (3)].ab)),TEST((yyvsp[(3) - (3)].ab))); }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 801 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOr(TPOS((yyvsp[(2) - (3)].tok)), TEST((yyvsp[(1) - (3)].ab)),TEST((yyvsp[(3) - (3)].ab))); }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 803 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 809 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewHas(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 811 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 813 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPrefix(APOS((yyvsp[(1) - (2)].ab)),(yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 146:
/* Line 1787 of yacc.c  */
#line 819 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPostfix(APOS((yyvsp[(2) - (2)].ab)),(yyvsp[(2) - (2)].ab),(yyvsp[(1) - (2)].ab)); }
    break;

  case 147:
/* Line 1787 of yacc.c  */
#line 821 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 149:
/* Line 1787 of yacc.c  */
#line 827 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 150:
/* Line 1787 of yacc.c  */
#line 829 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPrefix(APOS((yyvsp[(1) - (2)].ab)),(yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 152:
/* Line 1787 of yacc.c  */
#line 835 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 154:
/* Line 1787 of yacc.c  */
#line 841 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 156:
/* Line 1787 of yacc.c  */
#line 847 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 158:
/* Line 1787 of yacc.c  */
#line 854 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewCoerceTo(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 159:
/* Line 1787 of yacc.c  */
#line 856 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRestrictTo(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 160:
/* Line 1787 of yacc.c  */
#line 858 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPretendTo(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 863 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewCoerceTo(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 865 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewRestrictTo(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 164:
/* Line 1787 of yacc.c  */
#line 867 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPretendTo(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 167:
/* Line 1787 of yacc.c  */
#line 877 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewInfix(APOS((yyvsp[(2) - (3)].ab)),(yyvsp[(2) - (3)].ab),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 883 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 171:
/* Line 1787 of yacc.c  */
#line 889 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 895 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 175:
/* Line 1787 of yacc.c  */
#line 901 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewWith(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 903 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewAdd(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 177:
/* Line 1787 of yacc.c  */
#line 905 "aldor/aldor/src/axl.y"
    { (yyval.ab) = parseDeprecated(_YY_KW_Except, abNewExcept(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab))); }
    break;

  case 178:
/* Line 1787 of yacc.c  */
#line 907 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewExcept(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 238:
/* Line 1787 of yacc.c  */
#line 978 "aldor/aldor/src/axl.y"
    { (yyval.ab) = parseMakeJuxtapose((yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 239:
/* Line 1787 of yacc.c  */
#line 980 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewNot(TPOS((yyvsp[(1) - (2)].tok)),TEST((yyvsp[(2) - (2)].ab))); }
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 985 "aldor/aldor/src/axl.y"
    { (yyval.ab) = parseMakeJuxtapose((yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 242:
/* Line 1787 of yacc.c  */
#line 987 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewNot(TPOS((yyvsp[(1) - (2)].tok)),TEST((yyvsp[(2) - (2)].ab))); }
    break;

  case 244:
/* Line 1787 of yacc.c  */
#line 994 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewNot(TPOS((yyvsp[(1) - (2)].tok)),TEST((yyvsp[(2) - (2)].ab))); }
    break;

  case 245:
/* Line 1787 of yacc.c  */
#line 996 "aldor/aldor/src/axl.y"
    { (yyval.ab) = parseMakeJuxtapose((yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 246:
/* Line 1787 of yacc.c  */
#line 998 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPrefix(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 248:
/* Line 1787 of yacc.c  */
#line 1003 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewNot(TPOS((yyvsp[(1) - (2)].tok)),TEST((yyvsp[(2) - (2)].ab))); }
    break;

  case 249:
/* Line 1787 of yacc.c  */
#line 1005 "aldor/aldor/src/axl.y"
    { (yyval.ab) = parseMakeJuxtapose((yyvsp[(1) - (2)].ab),(yyvsp[(2) - (2)].ab)); }
    break;

  case 250:
/* Line 1787 of yacc.c  */
#line 1007 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewPrefix(TPOS((yyvsp[(2) - (3)].tok)),(yyvsp[(1) - (3)].ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 265:
/* Line 1787 of yacc.c  */
#line 1045 "aldor/aldor/src/axl.y"
    {
	  (yyval.ab) = abNewParen(TPOS((yyvsp[(1) - (2)].tok)), abNewComma0(TPOS((yyvsp[(1) - (2)].tok))));
	  abSetEnd((yyval.ab), TEND((yyvsp[(2) - (2)].tok)));
	}
    break;

  case 266:
/* Line 1787 of yacc.c  */
#line 1050 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewParen(TPOS((yyvsp[(1) - (3)].tok)), (yyvsp[(2) - (3)].ab)); abSetEnd((yyval.ab), TEND((yyvsp[(3) - (3)].tok))); }
    break;

  case 267:
/* Line 1787 of yacc.c  */
#line 1055 "aldor/aldor/src/axl.y"
    {
	  (yyval.ab) = abNewMatchfix(TPOS((yyvsp[(1) - (2)].tok)),
			     abNewId(TPOS((yyvsp[(1) - (2)].tok)),ssymBracket),
			     abNewComma0(TPOS((yyvsp[(1) - (2)].tok))));
	  abSetEnd((yyval.ab), TEND((yyvsp[(2) - (2)].tok)));
	}
    break;

  case 268:
/* Line 1787 of yacc.c  */
#line 1062 "aldor/aldor/src/axl.y"
    {
	  (yyval.ab) = abNewMatchfix(TPOS((yyvsp[(1) - (3)].tok)),
			     abNewId(TPOS((yyvsp[(1) - (3)].tok)),ssymBracket),
			     (yyvsp[(2) - (3)].ab));
	  abSetEnd((yyval.ab), TEND((yyvsp[(3) - (3)].tok)));
	}
    break;

  case 269:
/* Line 1787 of yacc.c  */
#line 1072 "aldor/aldor/src/axl.y"
    {
	  (yyval.ab) = abNewMatchfix(TPOS((yyvsp[(1) - (2)].tok)),
			     abNewId(TPOS((yyvsp[(1) - (2)].tok)),ssymEnum),
			     abNewComma0(TPOS((yyvsp[(1) - (2)].tok))));
	  abSetEnd((yyval.ab), TEND((yyvsp[(2) - (2)].tok)));
	}
    break;

  case 270:
/* Line 1787 of yacc.c  */
#line 1079 "aldor/aldor/src/axl.y"
    {
	  (yyval.ab) = abNewMatchfix(TPOS((yyvsp[(1) - (3)].tok)),
			     abNewId(TPOS((yyvsp[(1) - (3)].tok)),ssymEnum),
			     (yyvsp[(2) - (3)].ab));
	  abSetEnd((yyval.ab), TEND((yyvsp[(3) - (3)].tok)));
	}
    break;

  case 276:
/* Line 1787 of yacc.c  */
#line 1103 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id,    (yyvsp[(1) - (1)].tok)); }
    break;

  case 277:
/* Line 1787 of yacc.c  */
#line 1105 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id,    (yyvsp[(1) - (1)].tok)); }
    break;

  case 278:
/* Line 1787 of yacc.c  */
#line 1107 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id,    (yyvsp[(1) - (1)].tok)); }
    break;

  case 279:
/* Line 1787 of yacc.c  */
#line 1113 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_LitInteger, (yyvsp[(1) - (1)].tok)); }
    break;

  case 280:
/* Line 1787 of yacc.c  */
#line 1115 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_LitFloat,   (yyvsp[(1) - (1)].tok)); }
    break;

  case 281:
/* Line 1787 of yacc.c  */
#line 1117 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_LitString,  (yyvsp[(1) - (1)].tok)); }
    break;

  case 282:
/* Line 1787 of yacc.c  */
#line 1124 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abZip; }
    break;

  case 283:
/* Line 1787 of yacc.c  */
#line 1130 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 284:
/* Line 1787 of yacc.c  */
#line 1134 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 285:
/* Line 1787 of yacc.c  */
#line 1138 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 286:
/* Line 1787 of yacc.c  */
#line 1142 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 287:
/* Line 1787 of yacc.c  */
#line 1146 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 288:
/* Line 1787 of yacc.c  */
#line 1150 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 289:
/* Line 1787 of yacc.c  */
#line 1154 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 290:
/* Line 1787 of yacc.c  */
#line 1158 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 291:
/* Line 1787 of yacc.c  */
#line 1164 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 292:
/* Line 1787 of yacc.c  */
#line 1166 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 293:
/* Line 1787 of yacc.c  */
#line 1170 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 294:
/* Line 1787 of yacc.c  */
#line 1172 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 295:
/* Line 1787 of yacc.c  */
#line 1176 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 296:
/* Line 1787 of yacc.c  */
#line 1178 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 297:
/* Line 1787 of yacc.c  */
#line 1182 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 298:
/* Line 1787 of yacc.c  */
#line 1184 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 299:
/* Line 1787 of yacc.c  */
#line 1188 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 300:
/* Line 1787 of yacc.c  */
#line 1190 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 301:
/* Line 1787 of yacc.c  */
#line 1194 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 302:
/* Line 1787 of yacc.c  */
#line 1196 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 303:
/* Line 1787 of yacc.c  */
#line 1200 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 304:
/* Line 1787 of yacc.c  */
#line 1202 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 305:
/* Line 1787 of yacc.c  */
#line 1206 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[(1) - (1)].tok)); }
    break;

  case 306:
/* Line 1787 of yacc.c  */
#line 1208 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewQualify(TPOS((yyvsp[(1) - (3)].tok)), abNewOfToken(AB_Id, (yyvsp[(1) - (3)].tok)), (yyvsp[(3) - (3)].ab)); }
    break;

  case 323:
/* Line 1787 of yacc.c  */
#line 1250 "aldor/aldor/src/axl.y"
    {
	  (yyval.ab) = (yyvsp[(2) - (3)].ab);
	  if((yyvsp[(3) - (3)].ab)) (yyval.ab) = abNewDocumented(APOS((yyvsp[(3) - (3)].ab)),(yyval.ab),(yyvsp[(3) - (3)].ab));
	  if((yyvsp[(1) - (3)].ab)) (yyval.ab) = abNewDocumented(APOS((yyval.ab)),(yyval.ab),(yyvsp[(1) - (3)].ab));
	}
    break;

  case 324:
/* Line 1787 of yacc.c  */
#line 1259 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDocTextOfList((yyvsp[(1) - (1)].toklist)); listFree(Token)((yyvsp[(1) - (1)].toklist)); }
    break;

  case 325:
/* Line 1787 of yacc.c  */
#line 1264 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abNewDocTextOfList((yyvsp[(1) - (1)].toklist)); listFree(Token)((yyvsp[(1) - (1)].toklist)); }
    break;

  case 326:
/* Line 1787 of yacc.c  */
#line 1269 "aldor/aldor/src/axl.y"
    { (yyval.toklist) = listNil(Token); }
    break;

  case 327:
/* Line 1787 of yacc.c  */
#line 1271 "aldor/aldor/src/axl.y"
    { (yyval.toklist) = listCons(Token)((yyvsp[(1) - (2)].tok), (yyvsp[(2) - (2)].toklist)); }
    break;

  case 328:
/* Line 1787 of yacc.c  */
#line 1276 "aldor/aldor/src/axl.y"
    { (yyval.toklist) = listNil(Token); }
    break;

  case 329:
/* Line 1787 of yacc.c  */
#line 1278 "aldor/aldor/src/axl.y"
    { (yyval.toklist) = listCons(Token)((yyvsp[(1) - (2)].tok), (yyvsp[(2) - (2)].toklist)); }
    break;

  case 330:
/* Line 1787 of yacc.c  */
#line 1286 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[(1) - (1)].ablist)); listFree(AbSyn)((yyvsp[(1) - (1)].ablist)); }
    break;

  case 331:
/* Line 1787 of yacc.c  */
#line 1291 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[(1) - (1)].ablist)); listFree(AbSyn)((yyvsp[(1) - (1)].ablist)); }
    break;

  case 332:
/* Line 1787 of yacc.c  */
#line 1296 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[(1) - (1)].ablist)); listFree(AbSyn)((yyvsp[(1) - (1)].ablist)); }
    break;

  case 333:
/* Line 1787 of yacc.c  */
#line 1301 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[(1) - (1)].ablist)); listFree(AbSyn)((yyvsp[(1) - (1)].ablist)); }
    break;

  case 334:
/* Line 1787 of yacc.c  */
#line 1309 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(1) - (1)].ab), listNil(AbSyn) ); }
    break;

  case 335:
/* Line 1787 of yacc.c  */
#line 1311 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(3) - (3)].ab), (yyvsp[(1) - (3)].ablist)); }
    break;

  case 336:
/* Line 1787 of yacc.c  */
#line 1315 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(1) - (1)].ab), listNil(AbSyn) ); }
    break;

  case 337:
/* Line 1787 of yacc.c  */
#line 1317 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(3) - (3)].ab), (yyvsp[(1) - (3)].ablist)); }
    break;

  case 338:
/* Line 1787 of yacc.c  */
#line 1321 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(1) - (1)].ab), listNil(AbSyn) ); }
    break;

  case 339:
/* Line 1787 of yacc.c  */
#line 1323 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(3) - (3)].ab), (yyvsp[(1) - (3)].ablist)); }
    break;

  case 340:
/* Line 1787 of yacc.c  */
#line 1327 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(1) - (1)].ab), listNil(AbSyn) ); }
    break;

  case 341:
/* Line 1787 of yacc.c  */
#line 1329 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(3) - (3)].ab), (yyvsp[(1) - (3)].ablist)); }
    break;

  case 342:
/* Line 1787 of yacc.c  */
#line 1337 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abOneOrNewOfList(AB_Sequence, (yyvsp[(1) - (1)].ablist)); listFree(AbSyn)((yyvsp[(1) - (1)].ablist)); }
    break;

  case 343:
/* Line 1787 of yacc.c  */
#line 1343 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(1) - (1)].ab), listNil(AbSyn) ); }
    break;

  case 344:
/* Line 1787 of yacc.c  */
#line 1345 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(3) - (3)].ab), (yyvsp[(1) - (3)].ablist)); }
    break;

  case 345:
/* Line 1787 of yacc.c  */
#line 1347 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = (yyvsp[(1) - (2)].ablist); }
    break;

  case 346:
/* Line 1787 of yacc.c  */
#line 1354 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abOneOrNewOfList(AB_Sequence,(yyvsp[(2) - (3)].ablist)); listFree(AbSyn)((yyvsp[(2) - (3)].ablist)); }
    break;

  case 347:
/* Line 1787 of yacc.c  */
#line 1360 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(2) - (3)].ab); abSetPos((yyval.ab), TPOS((yyvsp[(1) - (3)].tok))); abSetEnd((yyval.ab), TEND((yyvsp[(3) - (3)].tok))); }
    break;

  case 348:
/* Line 1787 of yacc.c  */
#line 1366 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(1) - (1)].ab), listNil(AbSyn)); }
    break;

  case 349:
/* Line 1787 of yacc.c  */
#line 1368 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(3) - (3)].ab), (yyvsp[(1) - (3)].ablist)); }
    break;

  case 350:
/* Line 1787 of yacc.c  */
#line 1370 "aldor/aldor/src/axl.y"
    { yyerrok; (yyval.ablist) = listCons(AbSyn)((yyvsp[(3) - (3)].ab), listNil(AbSyn)); }
    break;

  case 351:
/* Line 1787 of yacc.c  */
#line 1376 "aldor/aldor/src/axl.y"
    { (yyval.ab) = abOneOrNewOfList(AB_Sequence,(yyvsp[(1) - (1)].ablist)); listFree(AbSyn)((yyvsp[(1) - (1)].ablist)); }
    break;

  case 353:
/* Line 1787 of yacc.c  */
#line 1383 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(2) - (2)].ab), (yyvsp[(1) - (2)].ablist)); }
    break;

  case 354:
/* Line 1787 of yacc.c  */
#line 1389 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listNil(AbSyn); }
    break;

  case 355:
/* Line 1787 of yacc.c  */
#line 1391 "aldor/aldor/src/axl.y"
    { (yyval.ablist) = listCons(AbSyn)((yyvsp[(2) - (2)].ab), (yyvsp[(1) - (2)].ablist)); }
    break;

  case 356:
/* Line 1787 of yacc.c  */
#line 1397 "aldor/aldor/src/axl.y"
    { (yyval.ab) = (yyvsp[(1) - (3)].ab); if((yyvsp[(3) - (3)].ab)) (yyval.ab) = abNewDocumented(APOS((yyvsp[(3) - (3)].ab)),(yyval.ab),(yyvsp[(3) - (3)].ab)); }
    break;

  case 357:
/* Line 1787 of yacc.c  */
#line 1399 "aldor/aldor/src/axl.y"
    { yyerrok; (yyval.ab) = abNewNothing(TPOS((yyvsp[(2) - (3)].tok))); }
    break;

  case 358:
/* Line 1787 of yacc.c  */
#line 1405 "aldor/aldor/src/axl.y"
    {
	  (yyval.ab) = (yyvsp[(2) - (3)].ab);
	  if((yyvsp[(1) - (3)].ab)) (yyval.ab) = abNewDocumented(APOS((yyvsp[(1) - (3)].ab)),(yyval.ab),(yyvsp[(1) - (3)].ab));
	  if((yyvsp[(3) - (3)].ab)) (yyval.ab) = abNewDocumented(APOS((yyval.ab)),(yyval.ab),(yyvsp[(3) - (3)].ab));
	}
    break;


/* Line 1787 of yacc.c  */
#line 3907 "y.tab.c"
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2050 of yacc.c  */
#line 1411 "aldor/aldor/src/axl.y"

