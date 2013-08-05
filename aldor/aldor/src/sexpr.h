/*****************************************************************************
 *
 * sexpr.h: Lisp-style S expressions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * sxAaaaBbbb  are Common Lisp objects. 
 *   Functions take and return SExpr values.
 *   See Steele "Common Lisp" (Digital Press) for definitions.
 *
 * sxAaaaBbbb_Option  are Common Lisp functions with combinations
 *   of the optional arguments or use more than one return result.
 *
 * sxiAaaaBbbb  take or return C and SExpr values and
 *   may correrspond to Common Lisp entities.
 *
 * sxFunAaaa  are SExprFun function objects.
 *   Functions take an SExpr form (fun and args) as a single SExpr argument,
 *     and pass the real arguments on to the sxAaaa version of the function.
 */

#ifndef _SEXPR_H_
#define _SEXPR_H_

#include "axlgen.h"

#include "bigint.h"
#include "strops.h"
#include "symbol.h"
#include "table.h"
#include "srcpos.h"

typedef enum {
	SX_Nil,
	SX_Symbol,
	SX_Package,
	SX_Integer, 
	SX_Ratio,
	SX_Float,
	SX_Complex,
	SX_Char,
	SX_String,
	SX_Vector,
	SX_Cons
} SExprTag;


struct sxHdr {
	BPack(SExprTag)	tag;
	BPack(Bool)	isShared;
	UShort		wrWidth;
	SrcPos		pos;
};


struct sxNil {
	struct sxHdr	hdr;
	SExpr		sxCarField;
	SExpr		sxCdrField;
};

struct sxSymbol {
	struct sxHdr	hdr;
	SExpr		homePkg;
	Symbol		sym;
	int		wrInfo;
};

struct sxPackage {
	struct sxHdr	hdr;
	SExpr		nameSymbol;
	Table		symbolTable;
};

struct sxInteger {
	struct sxHdr	hdr;
	BInt		val;
};

struct sxRatio {
	struct sxHdr	hdr;
	SExpr		num;
	SExpr		den;
};

struct sxFloat {
	struct sxHdr	hdr;
	BPack(char)     marker;
	DFloat		val;
};

struct sxComplex {
	struct sxHdr	hdr;
	SExpr		real;
	SExpr		imag;
};

struct sxChar {
	struct sxHdr	hdr;
	int		val;
};

struct sxString {
	struct sxHdr	hdr;
	String		val;
};

struct sxCons {
	struct sxHdr	hdr;
	SExpr		sxCarField;
	SExpr		sxCdrField;
};

struct sxVector {
	struct sxHdr	hdr;
	Length		argc;
	SExpr		argv[NARY];
};

union  SExprUnion  {
	struct sxHdr		sxHdr;
	struct sxNil		sxNil;
	struct sxSymbol 	sxSymbol;
	struct sxPackage	sxPackage;
	struct sxInteger	sxInteger;
	struct sxRatio		sxRatio;
	struct sxFloat		sxFloat;
	struct sxComplex	sxComplex;
	struct sxChar		sxChar;
	struct sxString 	sxString;
	struct sxCons		sxCons;
	struct sxVector 	sxVector;
};


extern void	sxiInit			(void);
extern String	sxiLispFileType;

extern SExpr	sxSTAR_Package_STAR;
extern SExpr	sxSTAR_Features_STAR;

extern FILE *	sxiSTAR_StandardInput_STAR;
extern FILE *	sxiSTAR_StandardOutput_STAR;

extern SExpr	sxT, sxNil, sxAnd, sxOr, sxNot;

extern SExpr	sx0KeywordPackage, sx0LispPackage,
		sx0SystemPackage,  sx0UserPackage;

#define		sxiTag(s)		((s)->sxHdr.tag)
#define		sxiIsShared(s)		((s)->sxHdr.isShared)
#define 	sxiPos(s)		((s)->sxHdr.pos)
extern SExpr	sxiRepos		(SrcPos, SExpr);

extern Bool	sxiEq			(SExpr, SExpr);

#define		sxiNull(s)		(sxiTag(s) == SX_Nil)
#define		sxNull(s)		(sxiNull(s) ? sxT : sxNil)

#define		sxiSymbolP(s)		(sxiTag(s) == SX_Symbol)
#define 	sxiToSymbol(s)		((s)->sxSymbol.sym)
extern SExpr	sxiFrSymbol		(Symbol sym);
#define		sxSymbolP(s)		(sxiSymbolP(s) ? sxT : sxNil)
extern SExpr	sxMakeSymbol		(SExpr str);
#define		sxSymbolPackage(s)	((s)->sxSymbol.homePkg)
#define         sxSymbolName(s)		sxiFrString(symString(sxiToSymbol(s)))


#define		sxiPackageP(s)		(sxiTag(s) == SX_Package)
#define		sxPackageP(s)		(sxiPackageP(s) ? sxT : sxNil)
extern SExpr	sxMakePackage		(SExpr  name);
extern SExpr	sxFindPackage		(SExpr  nameString);
#define		sxPackageName(s)	sxSymbolName((s)->sxPackage.nameSymbol)
extern SExpr	sxIntern		(SExpr  name);
extern SExpr	sxIntern_In		(SExpr  name, SExpr pkg);
extern SExpr	sx0InternInFrString	(String name, SExpr pkg);
extern SExpr	sx0InternInFrSymbol	(Symbol name, SExpr pkg);

#define		sxiIntegerP(s)		(sxiTag(s) == SX_Integer)
extern SExpr	sxiFrInteger		(long n);
#define		sxiToInteger(s)		bintSmall((s)->sxInteger.val)
extern SExpr	sxiFrBigInteger		(BInt n);
#define		sxiToBigInteger(s)	bintCopy(sxiToTheBigInteger(s))
#define		sxiToTheBigInteger(s)	((s)->sxInteger.val)
#define		sxIntegerP(s)		(sxiIntegerP(s) ? sxT : sxNil)

#define		sxiRatioP(s)		(sxiTag(s) == SX_Ratio)
extern SExpr	sxiMakeRatio		(int n, int d);
#define		sxRatioP(s)		(sxiRatioP(s) ? sxT : sxNil)
extern SExpr	sxNumerator		(SExpr);
extern SExpr	sxDenominator		(SExpr);

#define		sxiFloatP(s)		(sxiTag(s) == SX_Float)
extern SExpr	sxiFrFloat		(DFloat f, int marker);
#define		sxiToFloat(s)		((s)->sxFloat.val)
#define		sxFloatP(s)		(sxiFloatP(s) ? sxT : sxNil)

#define		sxiSFloatP(s)		sxiFloatP(s)
#define		sxiDFloatP(s)		sxiFloatP(s)
#define		sxiFrSFloat(ss)		sxiFrFloat(ss, 's')
#define		sxiFrDFloat(s)		sxiFrFloat(s, 'e')
#define		sxiToSFloat(s)		sxiToFloat(s)
#define		sxiToDFloat(s)		sxiToFloat(s)

#define		sxiComplexP(s)		(sxiTag(s) == SX_Complex)
#define		sxComplexP(s)		(sxiComplexP(s) ? sxT : sxNil)
extern SExpr	sxComplex		(SExpr, SExpr);
extern SExpr	sxRealpart		(SExpr);
extern SExpr	sxImagpart		(SExpr);

#define		sxiCharP(s)		(sxiTag(s) == SX_Char)
extern SExpr	sxiFrChar		(int c);
#define		sxiToChar(s)		((s)->sxChar.val)
#define		sxCharP(s)		(sxiCharP(s) ? sxT : sxNil)

#define		sxiStringP(s)		(sxiTag(s) == SX_String)
extern SExpr	sxiFrString		(char *s);
#define		sxiToString(s)		strCopy(sxiToTheString(s))
#define		sxiToTheString(s)	((s)->sxString.val)
#define		sxStringP(s)		(sxiStringP(s) ? sxT : sxNil)
extern SExpr	sxString_EQ		(SExpr, SExpr);
extern SExpr	sxString		(SExpr);

#define		sxiVectorP(s)		(sxiTag(s) == SX_Vector)
extern SExpr	sxiMakeVector		(Length n);
extern SExpr	sxiVector		(Length n, ...);
#define		sxVectorP(s)		(sxiVectorP(s) ? sxT : sxNil)

#define		sxiConsP(s)		(sxiTag(s) == SX_Cons)
extern SExpr	sxCons			(SExpr a, SExpr d);
#define		sxConsP(s)		(sxiConsP(s) ? sxT : sxNil)
#define		sxCar(s)		((s)->sxCons.sxCarField)
#define		sxCdr(s)		((s)->sxCons.sxCdrField)
#define		sxCadr(s)		sxCar(sxCdr(s))
#define		sxCddr(s)		sxCdr(sxCdr(s))

#define		sxFirst(s)		sxCar(s)
#define		sxSecond(s)		sxCadr(s)
#define		sxThird(s)		sxCar(sxCddr(s))
#define		sxFourth(s)		sxCadr(sxCddr(s))
#define		sxRest(s)		sxCdr(s)

#define		sxiNth(n,s)		sxCar(sxiNthCdr(n,s))
extern SExpr	sxiNthCdr		(Length n, SExpr sx);

#define		sxiListP(s)		(sxiNull(s) || sxiConsP(s))
extern SExpr	sxiList			(Length n, ...);

extern SExpr	sxNReverse		(SExpr sx);
extern Length	sxiLength		(SExpr sx);
extern SExpr	sxLength		(SExpr sx);
extern SExpr	sxNConc			(SExpr sx1, SExpr sx2);

/*
 * Other operations involving SExpr values.
 */
extern void	sxiInit			(void);

#define SXRW_MixedCase	(1L<<0)
#define SXRW_FoldCase	(1L<<1)
#define SXRW_SrcPos	(1L<<2)
#define SXRW_NoSrcPos	(1L<<3)
#define SXRW_Packages	(1L<<4)

#define SXRW_AsIs	(0L)
#define SXRW_Default	(SXRW_FoldCase | SXRW_NoSrcPos)

extern SExpr	sxiRead			(FILE *,FileName *,int *,SExpr,ULong);
extern int 	sxiWrite		(FILE *, SExpr, ULong);
extern String   sxiFormat		(SExpr sx);
extern void     sxiToBuffer             (Buffer buf, SExpr sx);
extern void sxiToBufferFormatted(Buffer buf, SExpr s, ULong flags);

extern void	sxiFree			(SExpr);
extern void	sxiFreeList		(SExpr);

extern void	sxiReadPrintLoop    	(FILE *fin, FILE *fout, ULong);
extern void	sxiReadEvalPrintLoop	(FILE *fin, FILE *fout, ULong);

extern SExpr	sxApply			(SExpr, SExpr);
extern SExpr	sxBye			(void);
extern SExpr	sxEval			(SExpr);
extern SExpr	sxPLUS			(SExpr);
extern SExpr	sxMINUS			(SExpr);
extern SExpr	sxTIMES			(SExpr);
extern SExpr	sxZeroP			(SExpr);
extern SExpr	sxPlusP			(SExpr);
extern SExpr	sxMinusP		(SExpr);

/*
 * Value/Function bindings.
 */

typedef	SExpr		(*SExprFun)		(SExpr);

extern SExpr		sxSymbolValue		(SExpr);
extern SExprFun		sxiSymbolFunction	(SExpr);

/*
 * Error handling.
 */
#define SX_ErrPackageExists   1
#define SX_ErrInternNeeds     2
#define SX_ErrNumDenNeeds     3
#define SX_ErrNReverseNeeds   4
#define SX_ErrBadArgumentTo   5
#define SX_ErrReadEOF         6
#define SX_ErrBadPunct        7
#define SX_ErrBadChar         8
#define SX_ErrMacroIlleg      9
#define SX_ErrMacroUndef     10
#define SX_ErrMacroUnimp     11
#define SX_ErrCantMacroArg   12
#define SX_ErrMustMacroArg   13
#define SX_ErrBadFeatureForm 14
#define SX_ErrTooManyElts    15
#define SX_ErrCantShare      16
#define SX_ErrAlreadyShare   17
#define SX_ErrBadCharName    18
#define SX_ErrBadComplexNum  19
#define SX_ErrBadToken       20
#define SX_ErrBadUninterned  21
#define SX_ErrBadPotNum      22
#define SX_ErrBugBadCharKind 23

typedef SExpr		(*SExprErrorFun)	(SrcPos, int errnum, va_list);

extern SExprErrorFun	sxiSetHandler	(SExprErrorFun);
	/* Install new handler and return old. 0 => default handler. */

#endif /* !_SEXPR_H_ */
