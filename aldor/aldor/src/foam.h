/*****************************************************************************
 *
 * foam.h: First Order Abstract Machine -- FOAM code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FOAM_H_
#define _FOAM_H_

#include "axlobs.h"
#include "fex.h"

/*****************************************************************************
 *
 * :: Tags used in linear and tree forms
 *
 ****************************************************************************/

enum foamTag {
	FOAM_START,
		/*
		 * If FOAM_DATA_START is ever changed so that it is not
		 * equal to zero then otIsFoamConst() MUST be updated.
		 */
		FOAM_DATA_START = FOAM_START,

			FOAM_Nil = FOAM_DATA_START,
			FOAM_Char,
			FOAM_Bool,
			FOAM_Byte,
			FOAM_HInt,
			FOAM_SInt,
			FOAM_SFlo,
			FOAM_DFlo,
			FOAM_Word,
			FOAM_Arb,

			FOAM_Int8,
			FOAM_Int16,
			FOAM_Int32,
			FOAM_Int64,
			FOAM_Int128,

		FOAM_DATA_LIMIT,
		FOAM_CONTROL_START = FOAM_DATA_LIMIT,

			FOAM_NOp = FOAM_CONTROL_START,    /* No operation */
			FOAM_BVal,       /* Builtin value        */
			FOAM_Ptr,
			FOAM_CProg,
			FOAM_CEnv,
			FOAM_Loose,
			FOAM_EEnsure,    /* Turn a lazy env into a real one */
			FOAM_EInfo,	 /* Information from environment */
			FOAM_Kill,       /* Crush a pointer reference  */
			FOAM_Free,       /* Free memory and pointer    */
			FOAM_Return,	 /* Return a value from a Prog */
			FOAM_Cast,
			FOAM_ANew,
			FOAM_RRNew,	 /* New raw record */
			FOAM_RRec,	 /* Raw record literal */
			FOAM_Clos,
			FOAM_Set,        /* Update value         */
			FOAM_Def,        /* Constant defn        */
			FOAM_AElt,       /* Array element        */
			FOAM_If,         /* If then else         */
			FOAM_Goto,
			FOAM_Throw,	
			FOAM_Catch,
			FOAM_Protect,
			FOAM_Unit,       /* A top-level piece of code */
			FOAM_PushEnv,
			FOAM_PopEnv,
			FOAM_MFmt,	 /* Indicate multiple values */
			FOAM_RRFmt,	 /* Raw record (dynamic) format */
	                FOAM_JavaObj,    /* Java things */
	                FOAM_CObj,      /* C things */

		FOAM_CONTROL_LIMIT,

/* ===========> FFO_ORIGIN (start of multi-format instructions) <=========== */

		FOAM_VECTOR_START = FOAM_CONTROL_LIMIT,

			FOAM_Unimp = FOAM_VECTOR_START,
			FOAM_GDecl,
			FOAM_Decl,
			FOAM_BInt,

		FOAM_VECTOR_LIMIT,
		FOAM_INDEX_START	= FOAM_VECTOR_LIMIT,

			FOAM_Par = FOAM_INDEX_START,
			FOAM_Loc,	 /* Local reference      */
			FOAM_Glo,	 /* Global reference     */
			FOAM_Fluid,	 /* Fluid reference      */
			FOAM_Const,	 /* Constant reference   */
			FOAM_Env,	 /* Lexical environment  */
			FOAM_EEnv,
			FOAM_RNew,
			FOAM_PRef,
			FOAM_TRNew,	 /* Records with trailing arrays */
			FOAM_RRElt,	 /* Raw record element */
			FOAM_Label,

		FOAM_INDEX_LIMIT,
		FOAM_MULTINT_START = FOAM_INDEX_LIMIT,

			FOAM_Lex = FOAM_MULTINT_START,/* Lexical reference   */
			FOAM_RElt,	 /* Record element      */
			FOAM_IRElt,	 /* Initial recorld elt */
			FOAM_TRElt,	 /* Trailing RElt */
			FOAM_EElt,	 /* Environment element */
			FOAM_CFCall,	 /* Closed Fun Call */
			FOAM_OFCall,	 /* Open Fun Call */

		FOAM_MULTINT_LIMIT,
		FOAM_NARY_START = FOAM_MULTINT_LIMIT,

			FOAM_DDecl = FOAM_NARY_START,
			FOAM_DFluid,
			FOAM_DEnv,
			FOAM_DDef,
			FOAM_DFmt,
			FOAM_Rec,
			FOAM_Arr,
			FOAM_TR, 
			FOAM_Select,
			FOAM_PCall,	 /* Call	by protocol  */
			FOAM_BCall,	 /* Builtin call      */
			FOAM_CCall,	 /* Closure call      */
			FOAM_OCall,	 /* Open    call      */
			FOAM_Seq,	 /* Sequence of computations */
			FOAM_Values,	 /* Multiple values   */
			FOAM_Prog,
			
		FOAM_NARY_LIMIT,

	FOAM_LIMIT = FOAM_NARY_LIMIT
};

typedef Enum(foamTag)    	FoamTag;


enum foamBValTag {
	FOAM_BVAL_START,
		FOAM_BVal_BoolFalse = FOAM_BVAL_START,
		FOAM_BVal_BoolTrue,
		FOAM_BVal_BoolNot,
		FOAM_BVal_BoolAnd,
		FOAM_BVal_BoolOr,
		FOAM_BVal_BoolEQ,
		FOAM_BVal_BoolNE,
		
		FOAM_BVal_CharSpace,
		FOAM_BVal_CharNewline,
		FOAM_BVal_CharTab,
		FOAM_BVal_CharMin,
		FOAM_BVal_CharMax,
		FOAM_BVal_CharIsDigit,
		FOAM_BVal_CharIsLetter,
		FOAM_BVal_CharEQ,
		FOAM_BVal_CharNE,
		FOAM_BVal_CharLT,
		FOAM_BVal_CharLE,
		FOAM_BVal_CharLower,
		FOAM_BVal_CharUpper,
		FOAM_BVal_CharOrd,
		FOAM_BVal_CharNum,

		FOAM_BVal_SFlo0,
		FOAM_BVal_SFlo1,
		FOAM_BVal_SFloMin,
		FOAM_BVal_SFloMax,
		FOAM_BVal_SFloEpsilon,
                FOAM_BVal_SFloIsZero,
                FOAM_BVal_SFloIsNeg,
                FOAM_BVal_SFloIsPos,
		FOAM_BVal_SFloEQ,
		FOAM_BVal_SFloNE,
		FOAM_BVal_SFloLT,
		FOAM_BVal_SFloLE,
		FOAM_BVal_SFloNegate,
		FOAM_BVal_SFloPrev,
		FOAM_BVal_SFloNext,
		FOAM_BVal_SFloPlus,
		FOAM_BVal_SFloMinus,
		FOAM_BVal_SFloTimes,
		FOAM_BVal_SFloTimesPlus,
		FOAM_BVal_SFloDivide,
		FOAM_BVal_SFloRPlus,
		FOAM_BVal_SFloRMinus,
		FOAM_BVal_SFloRTimes,
		FOAM_BVal_SFloRTimesPlus,
		FOAM_BVal_SFloRDivide,
		FOAM_BVal_SFloDissemble,
		FOAM_BVal_SFloAssemble,
		
		FOAM_BVal_DFlo0,
		FOAM_BVal_DFlo1,
		FOAM_BVal_DFloMin,
		FOAM_BVal_DFloMax,
		FOAM_BVal_DFloEpsilon,
                FOAM_BVal_DFloIsZero,
                FOAM_BVal_DFloIsNeg,
                FOAM_BVal_DFloIsPos,
		FOAM_BVal_DFloEQ,
		FOAM_BVal_DFloNE,
		FOAM_BVal_DFloLT,
		FOAM_BVal_DFloLE,
		FOAM_BVal_DFloNegate,
		FOAM_BVal_DFloPrev,
		FOAM_BVal_DFloNext,
		FOAM_BVal_DFloPlus,
		FOAM_BVal_DFloMinus,
		FOAM_BVal_DFloTimes,
		FOAM_BVal_DFloTimesPlus,
		FOAM_BVal_DFloDivide,
		FOAM_BVal_DFloRPlus,
		FOAM_BVal_DFloRMinus,
		FOAM_BVal_DFloRTimes,
		FOAM_BVal_DFloRTimesPlus,
		FOAM_BVal_DFloRDivide,
		FOAM_BVal_DFloDissemble,
		FOAM_BVal_DFloAssemble,
		
		FOAM_BVal_Byte0,
		FOAM_BVal_Byte1,
		FOAM_BVal_ByteMin,
		FOAM_BVal_ByteMax,

		FOAM_BVal_HInt0,
		FOAM_BVal_HInt1,
		FOAM_BVal_HIntMin,
		FOAM_BVal_HIntMax,

		FOAM_BVal_SInt0,
		FOAM_BVal_SInt1,
		FOAM_BVal_SIntMin,
		FOAM_BVal_SIntMax,
                FOAM_BVal_SIntIsZero,
                FOAM_BVal_SIntIsNeg,
                FOAM_BVal_SIntIsPos,
		FOAM_BVal_SIntIsEven,
		FOAM_BVal_SIntIsOdd,
		FOAM_BVal_SIntEQ,
		FOAM_BVal_SIntNE,
		FOAM_BVal_SIntLT,
		FOAM_BVal_SIntLE,
		FOAM_BVal_SIntNegate,
		FOAM_BVal_SIntPrev,
		FOAM_BVal_SIntNext,
		FOAM_BVal_SIntPlus,
		FOAM_BVal_SIntMinus,
		FOAM_BVal_SIntTimes,
		FOAM_BVal_SIntTimesPlus,
		FOAM_BVal_SIntMod,
		FOAM_BVal_SIntQuo,
		FOAM_BVal_SIntRem,
		FOAM_BVal_SIntDivide,
		FOAM_BVal_SIntGcd,
		FOAM_BVal_SIntPlusMod,
		FOAM_BVal_SIntMinusMod,
		FOAM_BVal_SIntTimesMod,
		FOAM_BVal_SIntTimesModInv,
		FOAM_BVal_SIntLength,
		FOAM_BVal_SIntShiftUp,
		FOAM_BVal_SIntShiftDn,
		FOAM_BVal_SIntBit,
		FOAM_BVal_SIntNot,
		FOAM_BVal_SIntAnd,
		FOAM_BVal_SIntOr,
		FOAM_BVal_SIntXOr,
		FOAM_BVal_SIntHashCombine,

		FOAM_BVal_WordTimesDouble,
		FOAM_BVal_WordDivideDouble,
		FOAM_BVal_WordPlusStep,
		FOAM_BVal_WordTimesStep,

		FOAM_BVal_BInt0,
		FOAM_BVal_BInt1,
                FOAM_BVal_BIntIsZero,
                FOAM_BVal_BIntIsNeg,
                FOAM_BVal_BIntIsPos,
		FOAM_BVal_BIntIsEven,
		FOAM_BVal_BIntIsOdd,
		FOAM_BVal_BIntIsSingle,
		FOAM_BVal_BIntEQ,
		FOAM_BVal_BIntNE,
		FOAM_BVal_BIntLT,
		FOAM_BVal_BIntLE,
		FOAM_BVal_BIntNegate,
		FOAM_BVal_BIntPrev,
		FOAM_BVal_BIntNext,
		FOAM_BVal_BIntPlus,
		FOAM_BVal_BIntMinus,
		FOAM_BVal_BIntTimes,
		FOAM_BVal_BIntTimesPlus,
		FOAM_BVal_BIntMod,
		FOAM_BVal_BIntQuo,
		FOAM_BVal_BIntRem,
		FOAM_BVal_BIntDivide,
		FOAM_BVal_BIntGcd,
		FOAM_BVal_BIntSIPower,
		FOAM_BVal_BIntBIPower,
	        FOAM_BVal_BIntPowerMod,
		FOAM_BVal_BIntLength,
		FOAM_BVal_BIntShiftUp,
		FOAM_BVal_BIntShiftDn,
		FOAM_BVal_BIntShiftRem,
		FOAM_BVal_BIntBit,

		FOAM_BVal_PtrNil,
		FOAM_BVal_PtrIsNil,
		FOAM_BVal_PtrMagicEQ,
		FOAM_BVal_PtrEQ,
		FOAM_BVal_PtrNE,

		FOAM_BVal_FormatSFlo,
		FOAM_BVal_FormatDFlo,
		FOAM_BVal_FormatSInt,
		FOAM_BVal_FormatBInt,

		FOAM_BVal_ScanSFlo,
		FOAM_BVal_ScanDFlo,
		FOAM_BVal_ScanSInt,
		FOAM_BVal_ScanBInt,

		FOAM_BVal_SFloToDFlo,
		FOAM_BVal_DFloToSFlo,
		FOAM_BVal_ByteToSInt,
		FOAM_BVal_SIntToByte,
		FOAM_BVal_HIntToSInt,
		FOAM_BVal_SIntToHInt,
		FOAM_BVal_SIntToBInt,
		FOAM_BVal_BIntToSInt,
		FOAM_BVal_SIntToSFlo,
		FOAM_BVal_SIntToDFlo,
		FOAM_BVal_BIntToSFlo,
		FOAM_BVal_BIntToDFlo,
		FOAM_BVal_PtrToSInt,
		FOAM_BVal_SIntToPtr,

		FOAM_BVal_ArrToSFlo,
		FOAM_BVal_ArrToDFlo,
		FOAM_BVal_ArrToSInt,
		FOAM_BVal_ArrToBInt,

		FOAM_BVal_PlatformRTE,
		FOAM_BVal_PlatformOS,
		FOAM_BVal_Halt,

		FOAM_BVal_RoundZero,
		FOAM_BVal_RoundNearest,
		FOAM_BVal_RoundUp,
		FOAM_BVal_RoundDown,
		FOAM_BVal_RoundDontCare,

		FOAM_BVal_SFloTruncate,
		FOAM_BVal_SFloFraction,
		FOAM_BVal_SFloRound,

		FOAM_BVal_DFloTruncate,
		FOAM_BVal_DFloFraction,
		FOAM_BVal_DFloRound,

		FOAM_BVal_StoForceGC,
		FOAM_BVal_StoInHeap,
		FOAM_BVal_StoIsWritable,
		FOAM_BVal_StoMarkObject,
		FOAM_BVal_StoRecode,
		FOAM_BVal_StoNewObject,
		FOAM_BVal_StoATracer,
		FOAM_BVal_StoCTracer,
		FOAM_BVal_StoShow,
		FOAM_BVal_StoShowArgs,

		FOAM_BVal_TypeInt8,
		FOAM_BVal_TypeInt16,
		FOAM_BVal_TypeInt32,
		FOAM_BVal_TypeInt64,
		FOAM_BVal_TypeInt128,

		FOAM_BVal_TypeNil,
		FOAM_BVal_TypeChar,
		FOAM_BVal_TypeBool,
		FOAM_BVal_TypeByte,
		FOAM_BVal_TypeHInt,
		FOAM_BVal_TypeSInt,
		FOAM_BVal_TypeBInt,
		FOAM_BVal_TypeSFlo,
		FOAM_BVal_TypeDFlo,
		FOAM_BVal_TypeWord,
		FOAM_BVal_TypeClos,
		FOAM_BVal_TypePtr,
		FOAM_BVal_TypeRec,
		FOAM_BVal_TypeArr,
		FOAM_BVal_TypeTR,

		FOAM_BVal_RawRepSize,

		FOAM_BVal_SizeOfInt8,
		FOAM_BVal_SizeOfInt16,
		FOAM_BVal_SizeOfInt32,
		FOAM_BVal_SizeOfInt64,
		FOAM_BVal_SizeOfInt128,

		FOAM_BVal_SizeOfNil,
		FOAM_BVal_SizeOfChar,
		FOAM_BVal_SizeOfBool,
		FOAM_BVal_SizeOfByte,
		FOAM_BVal_SizeOfHInt,
		FOAM_BVal_SizeOfSInt,
		FOAM_BVal_SizeOfBInt,
		FOAM_BVal_SizeOfSFlo,
		FOAM_BVal_SizeOfDFlo,
		FOAM_BVal_SizeOfWord,
		FOAM_BVal_SizeOfClos,
		FOAM_BVal_SizeOfPtr,
		FOAM_BVal_SizeOfRec,
		FOAM_BVal_SizeOfArr,
		FOAM_BVal_SizeOfTR,

		FOAM_BVal_ListNil,
		FOAM_BVal_ListEmptyP,
		FOAM_BVal_ListHead,
		FOAM_BVal_ListTail,
		FOAM_BVal_ListCons,

		FOAM_BVal_NewExportTable,
		FOAM_BVal_AddToExportTable,
		FOAM_BVal_FreeExportTable,
#if EDIT_1_0_n1_AB
		FOAM_BVal_ssaPhi,
#endif
	FOAM_BVAL_LIMIT
};


enum foamProtoTag {
   FOAM_PROTO_START,
	FOAM_Proto_Foam = FOAM_PROTO_START,
	FOAM_Proto_Fortran,
	FOAM_Proto_C,
	FOAM_Proto_Java,
	FOAM_Proto_JavaConstructor,
	FOAM_Proto_JavaMethod,
	FOAM_Proto_Lisp,
	FOAM_Proto_Init,
	FOAM_Proto_Include,
	FOAM_Proto_Other,
   FOAM_PROTO_LIMIT
};

enum foamDDeclTag {
	FOAM_DDecl_LocalEnv,
	FOAM_DDecl_NonLocalEnv,
	FOAM_DDecl_Param,
	FOAM_DDecl_Local,
	FOAM_DDecl_Fluid,
	FOAM_DDecl_Multi,
	FOAM_DDecl_Union,
	FOAM_DDecl_Record,
	FOAM_DDecl_TrailingArray,
	FOAM_DDecl_Consts,
	FOAM_DDecl_Global,
	FOAM_DDecl_FortranSig,
	FOAM_DDecl_CSig,
	FOAM_DDecl_CType,
	FOAM_DDecl_JavaSig,
	FOAM_DDecl_JavaClass,
   FOAM_DDECL_LIMIT
};
	
enum foamGDeclDirTag {
	FOAM_GDecl_Export,
	FOAM_GDecl_Import
};
	
typedef Enum(foamBValTag)       FoamBValTag;
typedef Enum(foamProtoTag)	FoamProtoTag;
typedef Enum(foamDDeclTag)	FoamDDeclTag;
typedef Enum(foamGDeclDirTag)	FoamGDeclDirTag;

enum foamHaltCode {
	FOAM_Halt_BadDependentType   	= 101,
	FOAM_Halt_NeverReached		= 102,
	FOAM_Halt_BadUnionCase	 	= 103,
	FOAM_Halt_AssertFailed	 	= 104,
	FOAM_Halt_BadFortranRecursion   = 105,
	FOAM_Halt_BadPointerWrite	= 106
};

/******************************************************************************
 *
 * :: Structures for interpreting FOAM tree forms.
 *
 ******************************************************************************/

/*
 * Generic views.
 */

struct foamHdr {
	BPack(FoamTag)		tag;
	BPack(UByte)		mark;	 /* Foam Audit tag */
	BPack(UByte)		dvMark;	 /* deadvar elim tag */
	SrcPos			pos;	 /* src pos of FOAM_Seq elements */
	union {
		OptInfo		opt;	 /* info for optimizer */
		Bool		pure;	 /* true for pure calls */
		Bool		fixed;	 /* true for fixed decls */
		Symbol		sym;	 /* ... to be used in genc */
		Bool		lazy;    /* EInfo used inside lazy getter */
 		int		defNo;   /* Used by of_copyp.c (see file) */
 		UdInfoList	defList; /* Reaching definitions (of_copyp.c)*/
		ExpInfo		expInfo; /* for common subexpr elimination */
                InvInfo         invInfo; /* for loop optimization */
#if EDIT_1_0_n1_AB
		FoamUses	fuses;   /* foamLoc pointer to SSA use list */
#endif
	} info;
	int			defnId;	 /* Implementation info (for defns) */
	Syme	 		syme;	 /* syme for imports */
	Length          	argc;
};

struct foamGen {
	struct foamHdr  hdr;
	union  {
		Foam    code;
		AInt    data;
		String  str;
		BInt	bint;
		SFloat  sfloat;
	} argv[NARY];
};


# define foamNewNil()           foamNew(FOAM_Nil, (int) 0)

struct foamNil {
	struct foamHdr          hdr;
};


# define foamNewChar(c)         foamNew(FOAM_Char, 1, (AInt)(c))

struct foamChar {
	struct foamHdr          hdr;
	AInt                    CharData;
};


# define foamNewBool(b)          foamNew(FOAM_Bool, 1, (AInt)(b))

struct foamBool {
	struct foamHdr          hdr;
	AInt                    BoolData;
};


# define foamNewByte(b)         foamNew(FOAM_Byte, 1, (AInt)(b))

struct foamByte {
	struct foamHdr          hdr;
	AInt                    ByteData;
};


# define foamNewHInt(h)         foamNew(FOAM_HInt, 1, (AInt)(h))

struct foamHInt {
	struct foamHdr          hdr;
	AInt                    HIntData;
};


# define foamNewSInt(i)         foamNew(FOAM_SInt, 1, (AInt)(i))

struct foamSInt {
	struct foamHdr          hdr;
	AInt                    SIntData;
};
extern Foam foamSIntReduce(Foam foam);

# define foamNewBInt(b)		foamNew(FOAM_BInt, 1, (b))

struct foamBInt {
	struct foamHdr          hdr;
	BInt			BIntData;
};

extern Foam	foamNewSFlo(SFloat);
# define foamToSFlo(foam)	((foam)->foamSFlo.SFloData)

struct foamSFlo {
	struct foamHdr          hdr;
	SFloat                  SFloData;
};

extern Foam	foamNewDFlo(DFloat);
# define foamToDFlo(foam)	((foam)->foamDFlo.DFloData)

struct foamDFlo {
	struct foamHdr          hdr;
	DFloat                  DFloData;
};

struct foamWord {
	struct foamHdr          hdr;
	AInt                    data;
};

struct foamArb {
	struct foamHdr          hdr;
	AInt                    data[2];
};

struct foamArr {
	struct foamHdr          hdr;
	AInt                    baseType;
	AInt                    eltv[NARY];
};

extern String foamArrToString(Foam);

struct foamRec {
	struct foamHdr          hdr;
	AInt                    format;
	Foam                    eltv[NARY];
};

struct foamRRec {
	struct foamHdr		hdr;
	AInt			nargs;
	Foam			fmt; /* FOAM_Values holding raw sizes */
	Foam			values; /* FOAM_Values holding raw values */
};

#ifdef NEW_FORMATS
#define foamNewProg(x,m,t,f,ib,p,l,fl,le,b) \
	foamNew(FOAM_Prog, 13, (AInt)(x),(AInt)(m),(AInt)(t),(AInt)(f),\
		(AInt)(ib), (AInt)0, (AInt)0, (AInt)0, (AInt)(p), l, fl, le, b)
#else
#define foamNewProg(x,m,t,f,ib,p,l,fl,le,b) \
	foamNew(FOAM_Prog, 13, (AInt)(x),(AInt)(m),(AInt)(t),(AInt)(f),\
		(AInt)(ib), (AInt)0, (AInt)0, (AInt)0 , p, l, fl, le, b)
#endif

extern Foam foamNewProgEmpty(void);

struct foamProg {
	struct foamHdr          hdr;
	AInt                    endOffset;
	AInt                    nLabels;    
	AInt                    retType;
	AInt			format;	
	AInt                    infoBits;

	AInt			size;
	AInt			time;	    /* estimated exec. time	  */
	AInt			auxInfo;

#ifdef NEW_FORMATS
	AInt                    params;
#else
	Foam                    params;
#endif
	Foam                    locals;
	Foam                    fluids; 
	Foam                    levels;
	Foam                    body;
};

#define foamNewClos(env, prog)   foamNew(FOAM_Clos,2, env, prog)

struct foamClos {
	struct foamHdr          hdr;
	Foam                    env;
	Foam                    prog;
};

#define foamNewGDecl(ty,id,rt,f,pr,dir) foamNew(FOAM_GDecl,6,(AInt)(ty),id, \
					     rt,f, \
					     (AInt)(pr),(AInt)(dir))
extern Bool foamGDeclIsImport(Foam);
extern Bool foamGDeclIsExport(Foam);
extern Bool foamGDeclIsExportOf(AInt, Foam);

struct foamGDecl {
	struct foamHdr          hdr;
	AInt                    type;
	String                  id;
	AInt			rtype;
	AInt			format;		/* for rec-valued vars */
	AInt			dir;
	AInt			protocol;
};

#define foamNewDecl(ty,id,f) foamNew(FOAM_Decl,4,(AInt)(ty),id, \
				     (AInt) SYME_NUMBER_UNASSIGNED, f)

struct foamDecl {
	struct foamHdr          hdr;
	AInt                    type;
	String                  id;
	AInt			symeIndex;
	AInt			format;		/* for rec-valued vars */
};

#define foamNewEmptyDDecl(u) foamNew(FOAM_DDecl, 1, (AInt) u)
extern Foam foamNewDDecl(AInt usage, ...);
extern Foam foamNewDDeclOfList(AInt usage, FoamList args);

struct foamDDecl {
	struct foamHdr          hdr;
	AInt			usage;
	Foam                    argv[NARY];
};

#define foamDDeclSlotc (1)
#define foamDDeclArgc(f) (foamArgc(f) - foamDDeclSlotc)


struct foamDFluid {
	struct foamHdr          hdr;
	AInt                    argv[NARY];
};

#define foamNewEmptyDEnv() foamNew(FOAM_DEnv, 0)

struct foamDEnv {
	struct foamHdr          hdr;
	AInt                    argv[NARY];
};

#define foamDEnvArgc(foam) foamArgc(foam)

struct foamDFmt {
	struct foamHdr          hdr;
	Foam                    argv[NARY];
};


#define foamNewDef(l,r)         foamNew(FOAM_Def, 2, l, r)

struct foamDef {
	struct foamHdr          hdr;
	Foam                    lhs;
	Foam                    rhs;
};

struct foamDDef {
	struct foamHdr          hdr;
	Foam                    argv[NARY];
};

#define foamNewPar(i)           foamNew(FOAM_Par, 1, (AInt)(i))

struct foamPar {
	struct foamHdr          hdr;
	AInt                    index;
};


#define foamNewLoc(i)           foamNew(FOAM_Loc, 1, (AInt)(i))
				 

struct foamLoc {
	struct foamHdr          hdr;
	AInt                    index;
};


#define foamNewLex(l,i)         foamNew(FOAM_Lex, 2, (AInt)(l), (AInt)(i))

struct foamLex {
	struct foamHdr          hdr;
	AInt                    level;
	AInt                    index;
};

#define foamNewGlo(i)           foamNew(FOAM_Glo, 1, (AInt)(i))

struct foamGlo {
	struct foamHdr          hdr;
	AInt                    index;
};

#define foamNewConst(i)         foamNew(FOAM_Const, 1, (AInt)(i))

struct foamConst {
	struct foamHdr          hdr;
	AInt                    index;
};

#define foamNewFluid(i)         foamNew(FOAM_Fluid, 1, (AInt)(i))

struct foamFluid {
	struct foamHdr          hdr;
	AInt                    index;
};

#define foamNewEnv(l)           foamNew(FOAM_Env, 1, (AInt)(l))

struct foamEnv {
	struct foamHdr          hdr;
	AInt                    level;
};

#define foamNewEEnv(l,e)          foamNew(FOAM_EEnv, 2, (AInt)(l), e)

struct foamEEnv {
	struct foamHdr          hdr;
	AInt                    level;
	Foam			env;
};

#define foamNewPRef(idx, prog)	foamNew(FOAM_PRef, 2, idx, prog)

struct foamPRef {
	struct foamHdr		hdr;
	AInt			idx;
	Foam			prog;
};

#define foamNewLabel(l)		foamNew(FOAM_Label, 1, (AInt)(l))

struct foamLabel {
	struct foamHdr          hdr;
	AInt                    label;
};

#define foamNewPtr(v)		foamNew(FOAM_Ptr, 1, v)

struct foamPtr {
	struct foamHdr          hdr;
	Foam			val;
};

#define foamNewCProg(v)		foamNew(FOAM_CProg, 1, v)

struct foamCProg {
	struct foamHdr          hdr;
	Foam			prog;
};

#define foamNewCEnv(v)		foamNew(FOAM_CEnv, 1, v)

struct foamCEnv {
	struct foamHdr          hdr;
	Foam			env;
};

#define foamNewLoose(v)		foamNew(FOAM_Loose, 1, v)

struct foamLoose {
	struct foamHdr          hdr;
	Foam			loc;
};

#define foamNewEEnsure(e)	foamNew(FOAM_EEnsure, 1, e)

struct foamEEnsure {
	struct foamHdr          hdr;
	Foam			env;
};

#define foamNewEInfo(env)		foamNew(FOAM_EInfo, 1, env)

struct foamEInfo {
	struct foamHdr 		hdr;
	Foam 			env;
};

#define foamNewAElt(t,a,n)      foamNew(FOAM_AElt,3,(AInt)(t),a,n)

struct foamAElt {
	struct foamHdr          hdr;
	AInt                    baseType;
	Foam                    index;
	Foam                    expr;
};

#define foamNewRRNew(f,n)       foamNew(FOAM_RRNew, 2, (AInt)n, f)

struct foamRRNew {
	struct foamHdr			hdr;
	AInt			argc;
	Foam			fmt;
};


#define foamNewRRElt(r,n,f)     foamNew(FOAM_RRElt,3,(AInt)(n),f,r)

struct foamRRElt {
	struct foamHdr		hdr;
	AInt			field;
	Foam			fmt;
	Foam			data;
};


#define foamNewRRFmt(f)         foamNew(FOAM_RRFmt,1,f)

struct foamRRFmt {
	struct foamHdr		 hdr;
	Foam			 fmt; /* FOAM values holding raw sizes */
};


#define foamNewRElt(f,r,n)      foamNew(FOAM_RElt,3,(AInt)(f),r,(AInt)(n))

struct foamRElt {
	struct foamHdr          hdr;
	AInt                    format;
	Foam                    expr;
	AInt                    field;
};

#define foamNewIRElt(f,r,n)     foamNew(FOAM_IRElt,3,(AInt)(f),r,(AInt)(n))

struct foamIRElt {
	struct foamHdr          hdr;
	AInt                    format;
	Foam                    expr;
	AInt                    field;
};

#define foamNewTRElt(i,r,x,n)     foamNew(FOAM_TRElt,4,i,r,x,n)

struct foamTRElt {
	struct foamHdr          hdr;
	AInt                    format;
	Foam                    expr;
	Foam			index;
	AInt                    field;
};

#define foamNewEElt(f,e,l,n)    foamNew(FOAM_EElt,4,(AInt)(f),e,(AInt)(l),(AInt)(n))

struct foamEElt {
	struct foamHdr          hdr;
	AInt                    env;
	Foam                    ref;
	AInt                    level;
	AInt                    lex;
};


#define foamNewBVal(o)          foamNew(FOAM_BVal, 1, (AInt)(o))

struct foamBVal {
	struct foamHdr          hdr;
	AInt                    builtinTag;
};


#define foamNewUnimp(s)         foamNew(FOAM_Unimp, 1, s)

struct foamUnimp {
	struct foamHdr          hdr;
	String                  str;
};


#define foamNewNOp()            foamNew(FOAM_NOp, (int) 0)

struct foamNOp {
	struct foamHdr          hdr;
};

#define foamNewSet(l,r)         foamNew(FOAM_Set, 2, l, r)

struct foamSet {
	struct foamHdr          hdr;
	Foam                    lhs;
	Foam                    rhs;
};

#define foamNewIf(c,l)	       foamNew(FOAM_If, 2, c, l)

struct foamIf {
	struct foamHdr          hdr;
	Foam                    test;
	AInt                    label;
};

extern Foam foamNewSeq(Foam arg0, ...);

struct foamSeq {
	struct foamHdr          hdr;
	Foam                    argv[NARY];
};

#define foamNewANew(t,s)	foamNew(FOAM_ANew, 2, t, s)

struct foamANew {
	struct foamHdr		hdr;
	AInt			eltType;
	Foam			size;
};

#define foamNewRNew(f)		foamNew(FOAM_RNew, 1, f)

struct foamRNew {
	struct foamHdr		hdr;
	AInt			format;
};

#define	foamNewTRNew(f,s)	foamNew(FOAM_TRNew, 2, f, s)

struct foamTRNew {
	struct foamHdr		hdr;
	AInt			format;
	Foam			size;
};

#define foamNewCast(t, e)	foamNew(FOAM_Cast, 2, t, e)

struct foamCast {
	struct foamHdr          hdr;
	AInt                    type;
	Foam                    expr;
};

extern Foam foamNewPCall(AInt protocol, AInt type, Foam op, ...);
extern Foam foamNewPCallOfList(AInt protocol, AInt type, Foam op, FoamList args);

struct foamPCall {
	struct foamHdr		hdr;
	AInt			protocol;
	AInt			type;
	Foam			op;
	Foam			argv[NARY];
};

#define foamPCallSlotc (3)
#define foamPCallArgc(foam) (foamArgc(foam) - foamPCallSlotc)

extern Foam foamNewBCall(AInt op, ...);
#define foamNewBCall0(op) foamNewBCall(op, NULL)
#define foamNewBCall1(op, arg1) foamNewBCall(op, arg1, NULL)
#define foamNewBCall2(op, arg1, arg2) foamNewBCall(op, arg1, arg2, NULL)

struct foamBCall {
	struct foamHdr          hdr;
	AInt                    op;
	Foam                    argv[NARY];
};

#define foamBCallSlotc (1)
#define foamBCallArgc(foam) (foamArgc(foam) - foamBCallSlotc)

struct foamCCall {
	struct foamHdr          hdr;
	AInt                    type;
	Foam                    op;
	Foam                    argv[NARY];
};

#define foamCCallSlotc (2)
#define foamCCallArgc(foam) (foamArgc(foam) - foamCCallSlotc )
extern Foam foamNewCCall(AInt type, Foam foam, ...);
extern Foam foamNewCCallOfList(AInt type, Foam op, FoamList args);

struct foamOCall {
	struct foamHdr          hdr;
	AInt                    type;
	Foam                    op;
	Foam                    env;
	Foam                    argv[NARY];
};

#define foamOCallSlotc (3)
#define foamOCallArgc(foam) (foamArgc(foam) - foamOCallSlotc)

struct foamCFCall {
	struct foamHdr          hdr;
	Foam                    clos;
	AInt                    fmt;
	AInt                    retFmt;
	Foam                    argsPtr;
};

#define foamCFCallSlotc (3)
#define foamCFCallArgc(foam) (foamArgc(foam) - foamCFCallSlotc);

struct foamOFCall {
	struct foamHdr          hdr;
	Foam                    prog;
	Foam                    env;
	AInt                    fmt;
	AInt                    retFmt;
	Foam                    argsPtr;
};

#define foamOFCallSlotc (3)
#define foamOFCallArgc(foam) (foamArgc(foam) - foamOFCallSlotc);

struct foamSelect {
	struct foamHdr          hdr;
	Foam                    op;
	AInt                    argv[NARY];	/* labels */
};

#define foamNewUnit(f,b)  foamNew(FOAM_Unit,2,f,b)

struct foamUnit {
	struct foamHdr          hdr;
	Foam                    formats;
	Foam                    defs;           
};

#define foamNewKill(v)          foamNew(FOAM_Kill, 1, v)

#define foamNewPushEnv(f,p) foamNew(FOAM_PushEnv,2,f,p)

struct foamPushEnv {
	struct foamHdr		hdr;
	AInt			format;
	Foam			parent;
};

#define foamNewPopEnv() foamNew(FOAM_PopEnv, 0)

struct foamPopEnv {
	struct foamHdr		hdr;
};

#define foamNewMFmt(f,v)	foamNew(FOAM_MFmt, 2, f, v)

struct foamMFmt {
	struct foamHdr		hdr;
	AInt			format;
	Foam			value;
};

#define foamNewEmptyValues()    foamNew(FOAM_Values, 0)
struct foamValues {
	struct foamHdr		hdr;
	Foam			argv[NARY];
};

struct foamKill {
	struct foamHdr          hdr;
	Foam                    place;
};

#define foamNewFree(v)          foamNew(FOAM_Free, 1, v)

struct foamFree {
	struct foamHdr          hdr;
	Foam                    place;
};

#define foamNewGoto(l)		foamNew(FOAM_Goto, 1, (AInt)(l))

struct foamGoto {
	struct foamHdr          hdr;
	AInt                    label;
};

#define foamNewThrow(t,v)	foamNew(FOAM_Throw, 2, t, v)

struct foamThrow {
	struct foamHdr		hdr;
	Foam			tag;
	Foam			val;
};

#define foamNewCatch(r, v)	foamNew(FOAM_Catch, 2, r, v)

struct foamCatch {
	struct foamHdr		hdr;
	Foam			ref;
	Foam			expr;
};

#define foamNewProtect(v, e, a)	foamNew(FOAM_Protect, 3, v, e, a)

struct foamProtect {
	struct foamHdr		hdr;
	Foam			val;
	Foam			expr;
	Foam			after;
};

#define foamNewReturn(v)      foamNew(FOAM_Return, 1, v)

struct foamReturn {
	struct foamHdr          hdr;
	Foam                    value;
};


union foam {
	struct foamHdr          hdr;
	struct foamGen          foamGen;
	
	struct foamNil          foamNil;
	struct foamChar         foamChar;
	struct foamBool         foamBool;
	struct foamByte         foamByte;
	struct foamHInt         foamHInt;
	struct foamSInt         foamSInt;
	struct foamBInt         foamBInt;
	struct foamSFlo         foamSFlo;
	struct foamDFlo         foamDFlo;
	struct foamWord         foamWord;
	struct foamArb		foamArb;
	struct foamArr          foamArr;
	struct foamRec          foamRec;
	struct foamRRec         foamRRec;
	struct foamProg         foamProg;
	struct foamClos         foamClos;

	struct foamDecl         foamDecl;
	struct foamGDecl        foamGDecl;
	struct foamDDecl        foamDDecl;
	struct foamDFluid       foamDFluid;
	struct foamDEnv         foamDEnv;
	struct foamDFmt         foamDFmt;
	struct foamDef          foamDef;
	struct foamDDef         foamDDef;

	struct foamPar          foamPar;
	struct foamLoc          foamLoc;
	struct foamLex          foamLex;
	struct foamGlo          foamGlo;
	struct foamFluid	foamFluid;
	struct foamConst	foamConst;
	struct foamEnv          foamEnv;
	struct foamEEnv         foamEEnv;
	struct foamPRef		foamPRef;
	struct foamLabel        foamLabel;
	struct foamPtr		foamPtr;
	struct foamCProg	foamCProg;
	struct foamCEnv		foamCEnv;
	struct foamLoose	foamLoose;
	struct foamEEnsure	foamEEnsure;
	struct foamEInfo	foamEInfo;
	struct foamAElt         foamAElt;
	struct foamRElt         foamRElt;
	struct foamRRElt        foamRRElt;
	struct foamIRElt        foamIRElt;
	struct foamTRElt        foamTRElt;
	struct foamEElt         foamEElt;
	struct foamBVal         foamBVal;

	struct foamUnimp        foamUnimp;
	struct foamNOp          foamNOp;
	struct foamSet          foamSet;
	struct foamIf           foamIf;
	struct foamSeq          foamSeq;
	struct foamSelect       foamSelect;
	struct foamANew		foamANew;
	struct foamRNew		foamRNew;
	struct foamRRNew	foamRRNew;
	struct foamTRNew	foamTRNew;
	struct foamCast         foamCast;
	struct foamPCall	foamPCall;
	struct foamBCall        foamBCall;
	struct foamCCall        foamCCall;
	struct foamOCall        foamOCall;
	struct foamCFCall       foamCFCall;
	struct foamOFCall       foamOFCall;
	struct foamPushEnv	foamPushEnv;
	struct foamPopEnv	foamPopEnv;
	struct foamMFmt		foamMFmt;
	struct foamRRFmt	foamRRFmt;
	struct foamValues	foamValues;

	struct foamUnit         foamUnit;

	struct foamKill         foamKill;
	struct foamFree         foamFree;
	struct foamGoto         foamGoto;
	struct foamThrow	foamThrow;
	struct foamCatch	foamCatch;
	struct foamProtect	foamProtect;
	struct foamReturn       foamReturn;
};


/******************************************************************************
 *
 * :: Tables of information about instructions, operations and protocols
 *
 ******************************************************************************/

#define FOAMP_SeqExit (1<<0)


struct foam_info {
	FoamTag                 tag;
	SExpr                   sxsym;
	String                  str;            
	short                   argc;      /* -1 => N-ary */
	String                  argf;
	int			properties;
};

#define FOAM_BVAL_MAX_ARGC    5
#define FOAM_BVAL_MAX_RETC    4

struct foamBVal_info {
	FoamBValTag           	tag;
	SExpr                   sxsym;
	String                  str;
	BPack(Bool)             hasSideFx;
	BPack(unsigned)         argCount;
	BPack(FoamTag)          argTypes[FOAM_BVAL_MAX_ARGC];
	BPack(FoamTag)          retType;
	BPack(unsigned)		retCount;
	BPack(FoamTag)		retTypes[FOAM_BVAL_MAX_RETC];
}; 

struct foamProto_info {
	FoamProtoTag		tag;
	SExpr			sxsym;
	String			str;
	FoamProtoTag            base;
};

struct foamDDecl_info {
	FoamDDeclTag	tag;
	SExpr		sxsym;
	String 		str;
};

extern struct foam_info      foamInfoTable[];
extern struct foamBVal_info  foamBValInfoTable[];
extern struct foamProto_info foamProtoInfoTable[];
extern struct foamDDecl_info foamDDeclInfoTable[];

#define FOAM_NARY	(-1)	/* Identifies tags with N-ary data argument. */


#define foamInfo(tag)       (foamInfoTable    [(int)(tag)-(int)FOAM_START])
#define foamBValInfo(tag)   (foamBValInfoTable[(int)(tag)-(int)FOAM_BVAL_START])
#define foamProtoInfo(tag)  (foamProtoInfoTable[(int)(tag)-(int)FOAM_PROTO_START])
#define foamDDeclInfo(tag)  (foamDDeclInfoTable[(int)(tag)])

#define foamStr(tag)        (foamInfo(tag).str)
#define foamBValStr(tag)    (foamBValInfo(tag).str)
#define foamBValRetType(tag)(foamBValInfo(tag).retType)
#define foamProtoStr(tag)   (foamProtoInfo(tag).str)
#define foamProtoBase(tag)  (foamProtoInfo(tag).base)

#define foamSExpr(tag)      (foamInfo(tag).sxsym)
#define foamBValSExpr(tag)  (foamBValInfo(tag).sxsym) 
#define foamProtoSExpr(tag) (foamProtoInfo(tag).sxsym)
#define foamDDeclSExpr(tag) (foamDDeclInfo(tag).sxsym)

/* To use these, include symcoinfo.h as well */
#define foamIdTag(sym)      ((FoamTag)      symCoInfo(sym)->foamTagVal)
#define foamBValIdTag(sym)  ((FoamBValTag)  symCoInfo(sym)->foamTagVal)
#define foamProtoIdTag(sym) ((FoamProtoTag) symCoInfo(sym)->foamTagVal)

#define foamProgIndex(foam) \
 	((foam)->foamProg.levels->foamDEnv.argv[0])


/******************************************************************************
 *
 * :: Basic operations
 *
 ******************************************************************************/

/* Foam field access */
#define foamTag(foam)        ((foam)->hdr.tag)
#define foamMark(foam)       ((foam)->hdr.mark)
#define foamDvMark(foam)     ((foam)->hdr.dvMark)
#define foamPos(foam)	     ((foam)->hdr.pos)
#define foamOptInfo(foam)    ((foam)->hdr.info.opt)
#define foamPure(foam)       ((foam)->hdr.info.pure)
#define foamFixed(foam)      ((foam)->hdr.info.fixed)
#define foamLazy(foam)       ((foam)->hdr.info.lazy)
#define foamSyme(foam)	     ((foam)->hdr.syme)
#define foamStab(foam)	     ((foam)->hdr.info.stab)

/******************************************************************************
 *
 * :: Info Bits
 *
 *****************************************************************************/

#define IB_SIDE         (1 << 0)	/* has side effects */
#define IB_LEAF		(1 << 1)	/* prog is a leaf */
#define IB_GENERATOR	(1 << 2)	/* prog is a generator */
#define IB_GETTER	(1 << 3)	/* prog is a getter function */
#define IB_INLINEME	(1 << 4)	/* prog should be inlined */
#define IB_USESFLUIDS   (1 << 5)	/* Prog uses fluid variables */
#define IB_FORCER	(1 << 6)	/* Prog is a forcer for consts */

#define IB_INLINEINFO	 (1 << 7)	/* Remaining info bits availables? */
#define IB_DONTINLINEME	 (1 << 8)	/* prog should never be inlined  */
#define IB_HASCONSTS	 (1 << 9)	/* prog contains some (Const ..) */
#define IB_NOOCALLS	 (1 << 10)	/* prog contains no OCalls	 */
#define IB_SINGLESTMT	 (1 << 11)	/* prog has a single statement	 */
#define IB_CALLEDONCE	 (1 << 12)	/* prog is called once.		 */
#define IB_NOENVUSE	 (1 << 13)	/* Prog does not refer to its environment */

/* Foam prog information bits */
#define foamProgIsSidingEffect(x) ((x)->foamProg.infoBits & IB_SIDE)
#define foamProgIsLeaf(x)         ((x)->foamProg.infoBits & IB_LEAF)
#define foamProgIsGenerator(x)    ((x)->foamProg.infoBits & IB_GENERATOR)
#define foamProgIsGetter(x)       ((x)->foamProg.infoBits & IB_GETTER)
#define foamProgInlineMe(x)       ((x)->foamProg.infoBits & IB_INLINEME)
#define foamProgUsesFluids(x)     ((x)->foamProg.infoBits & IB_USESFLUIDS)
#define foamProgIsForcer(x)       ((x)->foamProg.infoBits & IB_FORCER)

#define foamProgHasInlineInfo(x) ((x)->foamProg.infoBits & IB_INLINEINFO)
#define foamProgDontInlineMe(x)  ((x)->foamProg.infoBits & IB_DONTINLINEME)
#define foamProgHasConsts(x)     ((x)->foamProg.infoBits & IB_HASCONSTS)
#define foamProgHasNoOCalls(x)   ((x)->foamProg.infoBits & IB_NOOCALLS)
#define foamProgHasSingleStmt(x) ((x)->foamProg.infoBits & IB_SINGLESTMT)
#define foamProgIsCalledOnce(x)  ((x)->foamProg.infoBits & IB_CALLEDONCE)
#define foamProgHasNoEnvUse(x)   ((x)->foamProg.infoBits & IB_NOENVUSE)


#define foamProgSetHasInlineInfo(x)  ((x)->foamProg.infoBits |= IB_INLINEINFO)
#define foamProgSetLeaf(x)           ((x)->foamProg.infoBits |= IB_LEAF)
#define foamProgSetGenerator(x)      ((x)->foamProg.infoBits |= IB_GENERATOR)
#define foamProgSetGetter(x)         ((x)->foamProg.infoBits |= IB_GETTER)
#define foamProgSetInlineMe(x) 	     ((x)->foamProg.infoBits |= IB_INLINEME)
#define foamProgSetSide(x)     	     ((x)->foamProg.infoBits |= IB_SIDE)
#define foamProgSetUsesFluid(x)      ((x)->foamProg.infoBits |= IB_USESFLUIDS)
#define foamProgSetForcer(x) 	     ((x)->foamProg.infoBits |= IB_FORCER)

#define foamProgSetDontInlineMe(x)   ((x)->foamProg.infoBits |= IB_DONTINLINEME)
#define foamProgSetHasConsts(x)      ((x)->foamProg.infoBits |= IB_HASCONSTS)
#define foamProgSetHasNoOCalls(x)    ((x)->foamProg.infoBits |= IB_NOOCALLS)
#define foamProgSetHasSingleStmt(x)  ((x)->foamProg.infoBits |= IB_SINGLESTMT)
#define foamProgSetIsCalledOnce(x)   ((x)->foamProg.infoBits |= IB_CALLEDONCE)
#define foamProgSetNoEnvUse(x)       ((x)->foamProg.infoBits |= IB_NOENVUSE)

#define foamProgUnsetSide(x)         ((x)->foamProg.infoBits &= ~IB_SIDE)
#define foamProgUnsetLeaf(x)         ((x)->foamProg.infoBits &= ~IB_LEAF)

#define foamProgUnsetHasConsts(x)     ((x)->foamProg.infoBits &= ~IB_HASCONSTS)
#define foamProgUnsetHasNoOCalls(x)   ((x)->foamProg.infoBits &= ~IB_NOOCALLS)
#define foamProgUnsetHasSingleStmt(x) ((x)->foamProg.infoBits &= ~IB_SINGLESTMT)
#define foamProgUnsetIsCalledOnce(x)  ((x)->foamProg.infoBits &= ~IB_CALLEDONCE)
#define foamProgUnsetNoEnvUse(x)      ((x)->foamProg.infoBits &= ~IB_NOENVUSE)

#define foamArgc(foam)         ((foam)->hdr.argc)
#define foamArgv(foam)         ((foam)->foamGen.argv)

/* Predefined slots in the format section of a unit. */
#define globalsSlot 	0
#define constsSlot  	1
#define lexesSlot   	2
#define fluidsSlot   	3
#ifdef NEW_FORMATS
#define paramsSlot	4
#define emptyFormatSlot 5
#define envUsedSlot 	0
#define FOAM_FORMAT_START 6
#else
#define emptyFormatSlot 4
#define envUsedSlot 	0
#define FOAM_FORMAT_START 5
#endif

#define	foamUnitFormats(foam)	((foam)->foamUnit.formats)
#define foamUnitGlobals(foam)	\
 	(foamArgv((foam)->foamUnit.formats)[globalsSlot].code)
#define foamUnitConstants(foam)	\
 	(foamArgv((foam)->foamUnit.formats)[constsSlot].code)
#define foamUnitLexicals(foam)	\
 	(foamArgv((foam)->foamUnit.formats)[lexesSlot].code)
#define foamUnitFluids(foam)	\
 	(foamArgv((foam)->foamUnit.formats)[fluidsSlot].code)
#ifdef NEW_FORMATS
#define foamUnitParams(foam)	\
 	(foamArgv((foam)->foamUnit.formats)[paramsSlot].code)
#endif

/* map a function over a foam tree */
#define foamIter(foam, arg, argumentAction)				\
Statement( {								\
	String		argf     = foamInfo(foamTag(foam)).argf;	\
	Length		_i;						\
									\
	for (_i = 0; _i < foamArgc(foam); _i++, argf++) {		\
		if (*argf == '*') argf--;	 			\
		if (*argf == 'C') {					\
			  Foam	 *arg = (Foam *) foamArgv(foam)+_i;	\
			  Statement(argumentAction);			\
		 }							\
	}								\
})


extern void		 foamInit       (void);
extern void		 foamEnsureInit (void);

extern Foam              foamNewEmpty   (FoamTag t, Length argc);
extern Foam              foamNew        (FoamTag t, Length argc, ...);
extern Foam              foamNewOfList  (FoamTag t, FoamList);
extern Foam              foamNewOfList1  	(FoamTag t, AInt, FoamList);

extern SrcPos		 foamDefaultPosition;	/* Used by foamNewXxx */

extern void		 foamFree       (Foam);
extern Foam		 foamCopy       (Foam);
extern Length		 foamNodeCount	(Foam);

extern int		 foamNaryStart  (FoamTag);

extern Bool		 foamEqual	(Foam, Foam);
extern Bool		 foamEqualModBuffer(Foam, Foam);
extern Hash		 foamHash	(Foam);

extern void              foamFreeNode   (Foam);
extern Foam              foamCopyNode   (Foam);

extern Bool		 foamAuditAll	(Foam, UShort);
extern Bool		 foamAudit	(Foam);
extern void              foamAuditDecl	(Foam);
extern void		 foamAuditSetAll(void);
extern int		 foamTagLimit   (void);
extern int		 foamTagSpanLength(void);

extern Bool		 foamIsRef	(Foam);

extern int               foamPrint      (FILE *, Foam);
extern int		 foamPrintDb	(Foam);
extern void		 foamDumpToFile	(Foam, String);

/*
 * Conversion and basic I/O.
 */
extern int		 foamToBuffer   (Buffer, Foam);
extern Foam		 foamFrBuffer   (Buffer);
extern Bool              foamVerifyBuffer(Buffer, Foam);

extern void		 foamPosToBuffer   (Buffer, Foam);
extern void		 foamPosFrBuffer   (Buffer, Foam);

extern void		 foamPosBufPrint   (FILE *, Buffer);

extern Foam		 foamFormatsFrBuffer	(Buffer);
extern Foam		 foamConstFrBuffer	(Buffer, int);
extern Length		 foamConstcFrBuffer	(Buffer);
extern void		 foamConstvFrBuffer	(Buffer, Length, int *);
extern void		 foamConstvFrFoam	(Foam, Length, Foam *);

extern Foam		 foamGetProgHdrFrBuffer	(Buffer, int);

/*
 * General utilities.
 */

extern FoamTag		 foamExprType	(Foam, Foam, Foam, FoamBox, FoamBox,
					 AInt *);
extern FoamTag		 foamExprType0	(Foam expr, Foam prog, Foam formats, FoamBox locals,
					 FoamBox formatBox, AInt *extra);
extern FoamTag		 foamExprTypeG0	(Foam, Foam, Foam, FoamBox, FoamBox,
					 FoamBox, AInt *);
typedef Foam (*FoamExprTypeCallback)(void *, Foam);
extern FoamTag foamExprTypeCB(Foam expr, AInt *extra, FoamExprTypeCallback callback, void *arg);

extern int 		 foamCountSubtreesOfKind(Foam foam, FoamTag kind);

extern Bool foamTypeIsVoid(Foam fmts, FoamTag type, AInt fmt);
extern Bool foamTypeIsMulti(Foam fmts, FoamTag type, AInt fmt);
extern Bool foamTypeIsValue(Foam fmts, FoamTag type, AInt fmt);

extern Foam foamNeutralValue(FoamTag foam);

#define foamIsBCallOf(fm,bv)	\
	((fm) && foamTag(fm) == FOAM_BCall && (fm)->foamBCall.op == (bv))
#define foamLocNo(fm,n) \
	(((fm) && foamTag(fm) == FOAM_Loc) ? (fm)->foamLoc.index : -1)
#define foamParNo(fm,n) \
	(((fm) && foamTag(fm) == FOAM_Par) ? (fm)->foamPar.index : -1)

#define	foamIsDecl(foam)	(foamTag(foam) == FOAM_Decl)

#define foamTRDDeclIDeclN(ddecl)   ((ddecl)->foamDDecl.argv[0]->foamDecl.format)
#define foamTRDDeclTDeclN(ddecl)   (foamDDeclArgc(ddecl) - (1+foamTRDDeclIDeclN(ddecl)))

#define foamTRDDeclIDecl(ddecl, n) ((ddecl)->foamDDecl.argv[1+(n)])
#define foamTRDDeclTDecl(ddecl, n) ((ddecl)->foamDDecl.argv	\
				    [1+ foamTRDDeclIDeclN(ddecl) + (n)])

#define foamSelectArgc(foam)    (foamArgc(foam) - 1)

extern Bool foamProgHasMultiAssign(Foam prog);
extern Bool foamIsMultiAssign(Foam prog);

extern Bool foamDeclEqual(Foam, Foam);

extern int foamSeqNextReachable(Foam seq, int index);

/*
 * This macro can be used to strip multiple casts from an expression. Be
 * careful where you use it otherwise you may introduce bad foam sharing.
 */
#define foamDereferenceCast(foam)  \
          while (foamTag(foam) == FOAM_Cast) \
                (foam) = (foam)->foamCast.expr;



/* May free substructure. */
extern Foam		 foamNotThis		(Foam);
extern Bool		 foamIsData		(Foam);
extern Bool		 foamHasSideEffect	(Foam);
extern Bool		 foamIsControlFlow	(Foam);

typedef Bool (*FoamTestFn)(Foam f);
extern Foam foamFindFirst(FoamTestFn testFn, Foam foam);

typedef Bool (*FoamTestEnvFn)(Foam f, AInt env);
extern Foam foamFindFirstEnv(FoamTestEnvFn testFn, Foam foam, AInt env);

extern Foam foamFindFirstTag(FoamTag tag, Foam foam);

Foam foamCastIfNeeded(FoamTag wanted, FoamTag actual, Foam foam);


#endif /* !_FOAM_H_ */
