/*****************************************************************************
 *
 * sexpr.c: Lisp-style S expressions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file provides functions for manipulating expression trees
 * of a form which can be communicated to Lisp via text files.
 *
 * Names of the form sxAaaa operate on and produce values of type SExpr
 * and usually have a meaning compatible with the like-named objects in
 * Common Lisp.
 *
 * The following transliterations are used:
 *	aaa		sxAaa
 *	aaa-bbb		sxAaaBbb
 *	*aaa*		sxSTAR_Aaa_STAR
 *	aaa=		sxAaa_EQ
 *
 * Some names have more than one letter capitalized: NReverse ConsP etc.
 * Names of the form sxiAaaa operate on or produce values of other C types.
 * Names of the form sx0Aaaa are values local to this code.
 * Names of the form sxFunAaaa are SExprFun values local to this code.
 *
 *
 * TABLE OF CONTENTS
 *
 * :: Implementation parameters
 *
 * :: Initialization
 *
 * :: Symbols
 * :: Packages
 * :: Numbers
 * :: Characters
 * :: Strings
 * :: Vectors
 * :: Conses
 * :: Lists
 * :: Sequences
 *
 * :: Feature Forms
 *
 * :: I/O Syntax Table
 * :: I/O Buffer
 * :: Share Buffer
 * :: S-Expression Reader
 * :: S-Expression Writer
 *
 * :: Eval
 *
 * :: General operations
 * :: Error handling
 *
 *
 * To Do:
 *	   Use #nn= for uninterned symbols.
 *
 *	   complete #nn= to allow cyclic structures, rather than just DAGs.
 *	   copy symbols from "uses" list on package creation
 *	   sxSvref macros
 *	   calculate only once the info for output width of symbols
 *	   have reading and peeking macros use local var rather than global.
 *	   free partially created value when error in read
 *	   clearing shared values (e.g. symbols)
 *	   _ word separation rather than bicaps
 */

#include "axlgen.h"
#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "sexpr.h"
#include "store.h"
#include "util.h"

Bool sexprDebug = false;
#define sexprDEBUG	DEBUG_IF(sexprDebug)

#define RUBOUT		0177

local int sxiIoIsNeedingEscape(String);

local void sxiRdError(int errnum, ...);
local SExpr sxiUseError(int errnum, ...);
local SExpr sxiDefaultHandler(SrcPos, int errnum, va_list argp);

static SExprErrorFun sxiError = sxiDefaultHandler;

/*****************************************************************************
 *
 * :: Implementation parameters
 *
 ****************************************************************************/

#define sxiHowManySmallInts	256

#define sizeofSExprHead \
	(sizeof(struct SExprStruct) - sizeof(union SExprUnion))

/*****************************************************************************
 *
 * :: Initialization
 *
 ****************************************************************************/

static Buffer sxiIoBuf;
static Buffer sxiCommentBuf;

static SrcPos sxiCurrentPos;
static Length sxiCurrentLineNo;
static FileName sxiCurrentFName;

String sxiLispFileType = "lsp";

SExpr sxiSmallInts[sxiHowManySmallInts];

SExpr sxSTAR_Package_STAR;
SExpr sxSTAR_Features_STAR;

FILE * sxiSTAR_StandardInput_STAR;
FILE * sxiSTAR_StandardOutput_STAR;

SExpr sxT, sxNil, sx0Quote, sx0Function, sxNot, sxAnd, sxOr;

SExpr sx0PackageList;
SExpr sx0KeywordPackage, sx0LispPackage, sx0SystemPackage, sx0UserPackage;
SExpr sx0BackQuote, sx0BackQuoteComma, sx0BackQuoteCommaAt,
		sx0BackQuoteCommaDot, sx0ReadTimeEval, sx0LoadTimeEval;

/* Markers to ease recursive reading of lists */
static SExpr sxiRd_ADot, sxiRd_AParen;
/* Forward declarations */
local SExpr sxiNew(SExprTag, Length size);
local SExpr sxiNewInteger(long n);
local SExpr sxiNewNil(void);
local void sxiIoTableInit(void);

local int       sxFormatter     (OStream stream, Pointer p);

void sxiInit(void)
{
	int i;
	static Bool isInit = false;

	if (isInit)
		return;

	/*
	 * Initialize IO
	 */
	sposInit();
	sxiIoBuf = bufNew();
	sxiCommentBuf = bufNew();
	sxiIoTableInit();

	/*
	 * Initialize Nil
	 */
	sxNil = sxiNewNil();
	sxNil->sxHdr.isShared = true;

	/*
	 * Initialize small integers
	 */
	for (i = 0; i < sxiHowManySmallInts; i++) {
		sxiSmallInts[i] = sxiNewInteger(i);
		sxiSmallInts[i]->sxHdr.isShared = true;
	}

	/*
	 * Set for symbol initialization
	 */
	sx0KeywordPackage = sxNil;
	sx0LispPackage = sxNil;

	/*
	 * Initialize symbols
	 */
	sx0PackageList = sxNil;
	sxSTAR_Features_STAR = sxNil;

	sx0KeywordPackage = sxMakePackage(sxiFrString("KEYWORD"));
	sx0LispPackage = sxMakePackage(sxiFrString("LISP"));
	sx0SystemPackage = sxMakePackage(sxiFrString("SYSTEM"));
	sx0UserPackage = sxMakePackage(sxiFrString("USER"));

	sxSTAR_Package_STAR = sx0LispPackage;

	sxT = sxIntern(sxiFrString("T"));
	sx0Quote = sxIntern(sxiFrString("QUOTE"));
	sx0Function = sxIntern(sxiFrString("FUNCTION"));
	sxAnd = sxIntern(sxiFrString("AND"));
	sxOr = sxIntern(sxiFrString("OR"));
	sxNot = sxIntern(sxiFrString("NOT"));

	sxSTAR_Package_STAR = sx0SystemPackage;

	sx0BackQuote = sxIntern(sxiFrString("BACK-QUOTE"));
	sx0BackQuoteComma = sxIntern(sxiFrString("BACK-QUOTE-COMMA"));
	sx0BackQuoteCommaAt = sxIntern(sxiFrString("BACK-QUOTE-COMMA-AT"));
	sx0BackQuoteCommaDot = sxIntern(sxiFrString("BACK-QUOTE-COMMA-DOT"));

	sx0ReadTimeEval = sxIntern(sxiFrString("READ-TIME-EVAL"));
	sx0LoadTimeEval = sxIntern(sxiFrString("LOAD-TIME-EVAL"));

	sxiRd_ADot = sxMakeSymbol(sxiFrString("Read-a-."));
	sxiRd_AParen = sxMakeSymbol(sxiFrString("Read-a-)"));

	sxSTAR_Package_STAR = sx0UserPackage;

	fmtRegister("SExpr", sxFormatter);

	isInit = true;
}

local int
sxFormatter(OStream ostream, Pointer p)
{
	SExpr sx = (SExpr) p;
	Buffer b = bufNew();
	int c;
	sxiToBufferFormatted(b, sx, SXRW_MixedCase);
	c = ostreamWrite(ostream, bufLiberate(b), -1);
	
	return c;
}

/*****************************************************************************
 *
 * :: Nil
 *
 ****************************************************************************/

local SExpr sxiNewNil(void)
{
	SExpr sx;
	sx = sxiNew(SX_Nil, sizeof(sx->sxNil));
	sx->sxNil.sxCarField = sx;
	sx->sxNil.sxCdrField = sx;

	return sx;
}

/*****************************************************************************
 *
 * :: Symbols
 *
 ****************************************************************************/

local SExpr sx0MakeSymbol(Symbol cymbal, SExpr pkg)
{
	SExpr sym;

	sym = sxiNew(SX_Symbol, sizeof(sym->sxSymbol));
	sym->sxSymbol.homePkg = pkg;
	sym->sxSymbol.sym = cymbal;
	sym->sxSymbol.wrInfo = sxiIoIsNeedingEscape(symString(cymbal));

	return sym;
}

SExpr sxMakeSymbol(SExpr name)
{
	if (!sxiStringP(name))
		return sxiUseError(SX_ErrInternNeeds);
	return sx0InternInFrString(sxiToTheString(name), sxNil);
}

SExpr sxiFrSymbol(Symbol sym)
{
	return sx0InternInFrSymbol(sym, sxSTAR_Package_STAR);
}

/*****************************************************************************
 *
 * :: Packages
 *
 ****************************************************************************/

#define sx0PackageGet(p,s) \
	((SExpr) tblElt	  ((p)->sxPackage.symbolTable, (TblKey)(s),(TblElt) 0))
#define sx0PackagePut(p,s,x) \
	((SExpr) tblSetElt((p)->sxPackage.symbolTable, (TblKey)(s),(TblElt)(x)))

SExpr sxiFindPackage(String name)
{
	SExpr l;

	for (l = sx0PackageList; sxiConsP(l); l = sxCdr(l)) {
		SExpr sym = sxCar(l)->sxPackage.nameSymbol;
		if (!strcmp(name, symString(sxiToSymbol(sym))))
			return sxCar(l);
	}
	return sxNil;
}

SExpr sxFindPackage(SExpr name)
{
	return sxiFindPackage(sxiToTheString(sxString(name)));
}

SExpr sxMakePackage(SExpr name)
{
	SExpr p;

	name = sxString(name);

	if (sxiPackageP(sxFindPackage(name)))
		return sxiUseError(SX_ErrPackageExists, sxiToTheString(name));

	p = sxiNew(SX_Package, sizeof(p->sxPackage));
	p->sxHdr.isShared = true;

	p->sxPackage.nameSymbol = sxMakeSymbol(name);
	p->sxPackage.symbolTable = tblNew((TblHashFun) 0, (TblEqFun) 0);

	sx0PackageList = sxCons(p, sx0PackageList);

	/*!! Should copy here rather than probe twice in Intern */
	return p;
}

SExpr sxIntern(SExpr name)
{
	return sxIntern_In(name, sxSTAR_Package_STAR);
}

SExpr sxIntern_In(SExpr name, SExpr pkg)
{
	if (!sxiStringP(name))
		return sxiUseError(SX_ErrInternNeeds);
	return sx0InternInFrString(sxiToTheString(name), pkg);
}

SExpr sx0InternInFrString(String str, SExpr pkg)
{
	return sx0InternInFrSymbol(symIntern(str), pkg);
}

SExpr sx0InternInFrSymbol(Symbol cymbal, SExpr pkg)
{
	SExpr sym;

	if (!sxiNull(pkg)) {
		sym = sx0PackageGet(pkg, cymbal);
		if (sym)
			return sym;
	}
	if (!sxiNull(sx0LispPackage)) {
		sym = sx0PackageGet(sx0LispPackage, cymbal);
		if (sym)
			return sym;
	}

	sym = sx0MakeSymbol(cymbal, pkg);

	if (!sxiNull(pkg)) {
		(void) sx0PackagePut(pkg, cymbal, sym);
		sym->sxHdr.isShared = true;
	}

	return sym;
}

/*****************************************************************************
 *
 * :: Numbers
 *
 ****************************************************************************/

local SExpr sxiNewInteger(long n)
{
	SExpr s;
	s = sxiNew(SX_Integer, sizeof(s->sxInteger));
	s->sxInteger.val = bintNew(n);
	return s;
}

SExpr sxiFrInteger(long n)
{
	if (0 <= n && n < sxiHowManySmallInts)
		return sxiSmallInts[n];
	else
		return sxiNewInteger(n);
}

SExpr sxiFrBigInteger(BInt b)
{
	SExpr s;
	s = sxiNew(SX_Integer, sizeof(s->sxInteger));
	s->sxInteger.val = bintCopy(b);
	return s;
}

SExpr sxiMakeRatio(int n, int d)
{
	SExpr s;
	s = sxiNew(SX_Ratio, sizeof(s->sxRatio));
	s->sxRatio.num = sxiFrInteger(n);
	s->sxRatio.den = sxiFrInteger(d);
	return s;
}

SExpr sxNumerator(SExpr s)
{
	if (sxiRatioP(s))
		return s->sxRatio.num;
	if (sxiIntegerP(s))
		return s;
	return sxiUseError(SX_ErrNumDenNeeds, "Numerator");
}

SExpr sxDenominator(SExpr s)
{
	if (sxiRatioP(s))
		return s->sxRatio.den;
	if (sxiIntegerP(s))
		return sxiFrInteger(1);
	return sxiUseError(SX_ErrNumDenNeeds, "Denominator");
}

SExpr sxComplex(SExpr r, SExpr i)
{
	SExpr s;
	s = sxiNew(SX_Complex, sizeof(s->sxComplex));
	s->sxComplex.real = r;
	s->sxComplex.imag = i;

	return s;
}

SExpr sxRealpart(SExpr c)
{
	if (sxiComplexP(c))
		return c->sxComplex.real;
	else
		return c;
}

SExpr sxImagpart(SExpr c)
{
	if (sxiComplexP(c))
		return c->sxComplex.imag;
	else
		return sxiFrInteger(int0);
}

SExpr sxiFrFloat(DFloat f, int marker)
{
	SExpr s;

	s = sxiNew(SX_Float, sizeof(s->sxFloat));
	s->sxFloat.val = f;
	s->sxFloat.marker = marker;

	return s;
}

/*****************************************************************************
 *
 * :: Characters
 *
 ****************************************************************************/

SExpr sxiFrChar(int c)
{
	SExpr s;

	s = sxiNew(SX_Char, sizeof(s->sxChar));
	s->sxChar.val = c;

	return s;
}

/*****************************************************************************
 *
 * :: Strings
 *
 ****************************************************************************/

SExpr sxiFrString(String str)
{
	SExpr s;

	s = sxiNew(SX_String, sizeof(s->sxString));
	s->sxString.val = strCopy(str);

	return s;
}

SExpr sxString(SExpr s)
{
	if (sxiStringP(s))
		return s;
	if (sxiSymbolP(s))
		return sxSymbolName(s);

	return sxiUseError(SX_ErrBadArgumentTo, "String");
}

SExpr sxString_EQ(SExpr s1, SExpr s2)
{
	return strcmp(sxiToTheString(s1), sxiToTheString(s2)) ? sxNil : sxT;
}

/*****************************************************************************
 *
 * :: Vectors
 *
 ****************************************************************************/

#define sx0Size(s)	((s)->sxVector.argc)
#define sx0Elt(s,i)	((s)->sxVector.argv[i])

SExpr sxiMakeVector(Length n)
{
	SExpr s;
	Length i;

	s = sxiNew(SX_Vector, fullsizeof(s->sxVector, n, SExpr));
	s->sxVector.argc = n;
	for (i = 0; i < n; i++)
		s->sxVector.argv[i] = sxNil;

	return s;
}

SExpr sxiVector(Length n, ...)
{
	SExpr s;
	Length i;
	va_list argp;

	s = sxiNew(SX_Vector, fullsizeof(s->sxVector, n, SExpr));
	s->sxVector.argc = n;

	va_start(argp, n);
	for (i = 0; i < n; i++)
		s->sxVector.argv[i] = va_arg(argp, SExpr);
	va_end(argp);

	return s;
}

/*****************************************************************************
 *
 * :: Conses
 *
 ****************************************************************************/

SExpr sxCons(SExpr a, SExpr d)
{
	SExpr s;

	s = sxiNew(SX_Cons, sizeof(s->sxCons));
	s->sxCons.sxCarField = a;
	s->sxCons.sxCdrField = d;

	return s;
}

/*****************************************************************************
 *
 * :: Lists
 *
 ****************************************************************************/

SExpr sxiList(Length n, ...)
{
	SExpr sx;
	va_list argp;
	Length i;

	va_start(argp, n);
	sx = sxNil;
	for (i = 0; i < n; i++)
		sx = sxCons(va_arg(argp, SExpr), sx);
	sx = sxNReverse(sx);
	va_end(argp);

	return sx;
}

SExpr sxiNthCdr(Length n, SExpr sx)
{
	while (n-- > 0)
		sx = sxCdr(sx);
	return sx;
}

/*****************************************************************************
 *
 * :: Sequences
 *
 ****************************************************************************/

SExpr sxNReverse(SExpr sx)
{
	if (sxiNull(sx))
		return sx;
	if (sxiConsP(sx)) {
		SExpr r, t;
		r = sxNil;
		while (sxiConsP(sx)) {
			t = sxCdr(sx);
			sxCdr(sx) = r;
			r = sx;
			sx = t;
		}
		if (!sxiNull(sx))
			return sxiUseError(SX_ErrNReverseNeeds);
		sx = r;
	} else if (sxiVectorP(sx)) {
		Length s, e, n;
		SExpr t;
		n = sx->sxVector.argc;
		for (s = 0, e = n - 1; s <= e; s++, e--) {
			t = sx->sxVector.argv[s];
			sx->sxVector.argv[s] = sx->sxVector.argv[e];
			sx->sxVector.argv[e] = t;
		}
	} else if (sxiStringP(sx)) {
		Length s, e, n;
		int t;
		n = strlen(sx->sxString.val);
		for (s = 0, e = n - 1; s <= e; s++, e--) {
			t = sx->sxString.val[s];
			sx->sxString.val[s] = sx->sxString.val[e];
			sx->sxString.val[e] = t;
		}
	} else
		sx = sxiUseError(SX_ErrBadArgumentTo, "NReverse");
	return sx;
}

Length sxiLength(SExpr sx)
{
	Length n;

	if (sxiNull(sx))
		n = 0;
	else if (sxiConsP(sx))
		for (n = 0; sxiConsP(sx); n++)
			sx = sxCdr(sx);
	else if (sxiVectorP(sx))
		n = sx->sxVector.argc;
	else if (sxiStringP(sx))
		n = strlen(sx->sxString.val);
	else {
		sxiUseError(SX_ErrBadArgumentTo, "Length");
		n = 0;
	}
	return n;
}

SExpr sxLength(SExpr sx)
{
	return sxiFrInteger(sxiLength(sx));
}

SExpr sxNConc(SExpr sx1, SExpr sx2)
{
	SExpr sx = sx1;
	if (sxiNull(sx1))
		return sx2;
	assert(sxiConsP(sx));
	while (!sxiNull(sxCdr(sx)))
		sx = sxCdr(sx);
	sxCdr(sx) = sx2;
	return sx1;
}

/*****************************************************************************
 *
 * :: Feature Forms
 *
 ****************************************************************************/

SExpr sx0Memq(SExpr ob, SExpr l)
{
	for (; sxiConsP(l); l = sxCdr(l))
		if (ob == sxCar(l))
			return sxT;
	return sxNil;
}

SExpr sx0EvalFeatureForm(SExpr f, SExpr errval)
{
	if (sxiSymbolP(f))
		return sx0Memq(f, sxSTAR_Features_STAR);

	if (sxiConsP(f)) {
		if (sxCar(f) == sxNot) {
			f = sxCdr(f);
			if (!sxiConsP(f) || sxiConsP(sxCdr(f)))
				return errval;

			if (sx0EvalFeatureForm(sxCar(f), errval))
				return sxNil;
			return sxT;
		} else if (sxCar(f) == sxAnd) {
			for (f = sxCdr(f); sxiConsP(f); f = sxCdr(f))
				if (!sx0EvalFeatureForm(sxCar(f), errval))
					return sxNil;
			return sxT;
		} else if (sxCar(f) == sxOr) {
			for (f = sxCdr(f); sxiConsP(f); f = sxCdr(f))
				if (sx0EvalFeatureForm(sxCar(f), errval))
					return sxT;
			return sxNil;
		}
	}
	return errval;
}

/*****************************************************************************
 *
 * :: I/O Syntax Table
 *
 ****************************************************************************/

/*
 * Common Lisp syntax.
 */

int sxiIoMixedCase = 0; /* This is settable */
int sxiIoKeepSrcPos = 0; /* This is settable */
int sxiIoOutPackages = 0; /* This is settable */
/*
 * One byte of info is stored for each character.
 * The low 3 bits indicate the character kind and the 4th bit indicates
 * whether the character is an exponent marker.	  Bits 5-8 are unused.
 */

unsigned char sxiIoTable[MAX_BYTE + 1];

enum sxiIoKindEnum {
	SX_IoIllegal = 0, /* Illegal */
	SX_IoSpace = 1, /* White Space */
	SX_IoConstit = 2, /* Constituent */
	SX_IoSingEsc = 3, /* Single Escape */
	SX_IoMultEsc = 4, /* Multiple Escape */
	SX_IoTermMac = 5, /* Terminating Macro */
	SX_IoNonTMac = 6
/* Non-terminating Macro */
};

#define sxiIoExptMarker 0x08

#define sxiIoKind(c)		(sxiIoTable[c] & 0x07)
#define sxiIoIsExptMarker(c)	(sxiIoTable[c] & sxiIoExptMarker)

local void sxiIoTableInit(void)
{
	static int isInitialized = 0;
	int c;
	UByte *s;

	if (isInitialized)
		return;

	for (c = 0; c <= MAX_BYTE; c++)
		sxiIoTable[c] = SX_IoIllegal;

	s = (UByte *) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	while (*s)
		sxiIoTable[*s++] = SX_IoConstit;

	s = (UByte *) "0123456789!$%&*+-./:<=>?@[]^_{}~";
	while (*s)
		sxiIoTable[*s++] = SX_IoConstit;
	sxiIoTable['\b'] = SX_IoConstit;
	sxiIoTable[RUBOUT] = SX_IoConstit;

	s = (UByte *) " \t\f\n\r";
	while (*s)
		sxiIoTable[*s++] = SX_IoSpace;

	s = (UByte *) "`\"'(),;";
	while (*s)
		sxiIoTable[*s++] = SX_IoTermMac;

	sxiIoTable['#'] = SX_IoNonTMac;
	sxiIoTable['\\'] = SX_IoSingEsc;
	sxiIoTable['|'] = SX_IoMultEsc;

	s = (UByte *) "esfdlESFDL";
	while (*s)
		sxiIoTable[*s++] |= sxiIoExptMarker;

	isInitialized = 1;
}

/*
 * Values returned by sxiIoIsPotentialNumber.
 */
enum {
	SX_IoNumNot = 0, /* 0 here allows use in boolean context */
	SX_IoNumInteger = 1,
	SX_IoNumRatio = 2,
	SX_IoNumFloat = 3,
	SX_IoNumPotential = 4
};

int sxiIoIsPotentialNumber(String s)
{
	UByte *t;

	int n; /* n should equal to the sum of the nXxxx below. */
	int nDigits = 0, nSigns = 0, nDecimals = 0, nRatios = 0, nExtensions =
			0, nMarkers = 0, nOthers = 0;

	for (n = 0, t = (UByte *) s; *t; n++, t++) {
		int c = *t;
		if (isdigit(c))
			nDigits++;
		else if (c == '+' || c == '-')
			nSigns++;
		else if (c == '.')
			nDecimals++;
		else if (c == '/')
			nRatios++;
		else if (c == '^' || c == '_')
			nExtensions++;
		else if (isalpha(c)) {
			if (!isalpha(t[1]) && (n == 0 || !isalpha(t[-1])))
				nMarkers++;
			else
				nOthers++;
		} else
			nOthers++;

		/* Don't need to finish */
		if (nOthers > 0)
			return SX_IoNumNot;
	}

	/*
	 * Apply rules for "potential numbers".
	 */
	if (nDigits == 0)
		return SX_IoNumNot;
	if (isalpha(s[0]) || s[0] == '/' || s[n - 1] == '-' || s[n - 1] == '+')
		return SX_IoNumNot;

	/*
	 * Test for integer.  Common Lisp allows final decimal pt!
	 *
	 * [sign] {digit}+ [decimal-point]
	 */
	if (nSigns <= 1 && nDecimals <= 1 && nDigits + nSigns + nDecimals == n) {
		if ((nSigns == 0 || s[0] == '-' || s[0] == '+') && (nDecimals
				== 0 || s[n - 1] == '.')) {
			return SX_IoNumInteger;
		}
	}

	/*
	 * Test for ratio.
	 *
	 * [sign] {digit}+ / {digit}+
	 */
	if (nSigns <= 1 && nRatios == 1 && nDigits + nSigns + nRatios == n) {
		if (nSigns == 0) {
			if (isdigit(s[0]) && isdigit(s[n - 1]))
				return SX_IoNumRatio;
		} else {
			if ((s[0] == '-' || s[0] == '+') && isdigit(s[1])
					&& isdigit(s[n - 1]))
				return SX_IoNumRatio;
		}
	}

	/*
	 * Test for float.
	 *
	 * [sign] {digit}*  decimal-point {digit}+ [exponent] |
	 * [sign] {digit}+ [decimal-point {digit}*] exponent
	 *
	 * where exponent ::= exponent-marker [sign] {digit}+
	 */
	if (nSigns <= 2 && nDecimals <= 1 && nMarkers <= 1 && nSigns
			+ nDecimals + nMarkers + nDigits == n) {
		/* Constraints seem to be insufficient here. Have to scan. */
		UByte *t = (UByte *) s;
		if (t[0] == '-' || t[0] == '+') {
			nSigns--;
			t++;
		}
		if (nDecimals == 1) {
			while (isdigit(*t))
				t++;
			if (*t != '.')
				goto notFloat;
			t++;
			if (!isdigit(*t))
				goto notFloat;
			while (isdigit(*t))
				t++;
			/* At exponent marker or End */
		} else {
			if (!isdigit(*t))
				goto notFloat;
			while (isdigit(*t))
				t++;
			if (!sxiIoIsExptMarker(*t))
				goto notFloat;
			/* At exponent marker */
		}
		/* At exponent marker or End */
		if (*t && !sxiIoIsExptMarker(*t))
			return SX_IoNumPotential;
		if (*t) {
			t++;
			if (*t == '-' || *t == '+')
				t++;
			if (!isdigit(*t))
				goto notFloat;
			while (isdigit(*t))
				t++;
			if (*t != 0)
				goto notFloat;
		}
		return SX_IoNumFloat;
	}

	notFloat: return SX_IoNumPotential;
}

int sxiExponentCharacter(String s)
{
	int i, c = 'e';
	for (i = 0; s[i]; i++)
		if (isalpha(s[i])) {
			c = s[i];
			s[i] = 'e';
			break;
		}
	return c;
}

/*
 * Values returned by sxiIoIsNeedingEscape.
 */
enum {
	SX_IoEscDont = 0, /* 0 here allows enum in boolean contexts */
	SX_IoEscForSpecials = 1,
	SX_IoEscForDoubleColon = 2,
	SX_IoEscForNumber = 3,
	SX_IoEscForLower = 4,
	SX_IoEscForLowerNumber = 5
};

local int sxiIoIsNeedingEscape(String s)
{
	int k, hasLower = 0, hasOnlyDots = 1;
	UByte *t = (UByte *) s;

	if (sxiIoKind(*t) == SX_IoNonTMac)
		return SX_IoEscForSpecials;

	for (; *t; t++) {
		if (*t == ':' && *(t + 1) == ':')
			return SX_IoEscForDoubleColon;
		if ((k = sxiIoKind(*t)) != SX_IoConstit && k != SX_IoNonTMac)
			return SX_IoEscForSpecials;
		if (islower(*t))
			hasLower = 1;
		if (*t != '.')
			hasOnlyDots = 0;
	}
	if (hasOnlyDots)
		return SX_IoEscForSpecials;
	if (sxiIoIsPotentialNumber(s))
		return hasLower ? SX_IoEscForLowerNumber : SX_IoEscForNumber;
	if (hasLower)
		return SX_IoEscForLower;

	return SX_IoEscDont;
}

/*****************************************************************************
 *
 * :: I/O Buffer
 *
 ****************************************************************************/

#define sxiIoBufStart()	     BUF_START(sxiIoBuf)
#define sxiIoBufEnd()	     BUF_ADD1(sxiIoBuf, 0)
#define sxiIoBufAdd(c)	     BUF_ADD1(sxiIoBuf, (c))
#define sxiIoBufPuts(s)	     bufPuts(sxiIoBuf, (s))
#define sxiIoBufChars()	     bufChars(sxiIoBuf)

#define sxiCommentBufStart() BUF_START(sxiCommentBuf)
#define sxiCommentBufEnd()   BUF_ADD1(sxiCommentBuf, 0)
#define sxiCommentBufAdd(c)  BUF_ADD1(sxiCommentBuf, (c))
#define sxiCommentBufPuts(s) bufPuts(sxiCommentBuf, (s))
#define sxiCommentBufChars() bufChars(sxiCommentBuf)

/*****************************************************************************
 *
 * :: Share Buffer
 *
 ****************************************************************************/

static SExpr *sxiShareV = 0;
static int sxiShareVSize = 0;
static int sxiShareVIx;

#define sxiShareVUninit 0

local void sxiShareVStart(void)
{
	int i;
	sxiShareVIx = 0;
	for (i = 0; i < sxiShareVSize; i++)
		sxiShareV[i] = sxiShareVUninit;
}

local SExpr sxiShareVAdd(int n, SExpr s)
{
	if (n == -1) {
		n = ++sxiShareVIx;
	}
	if (n >= sxiShareVSize) {
		SExpr *oldV = sxiShareV;
		int oldVSize = sxiShareVSize;
		int i;

		sxiShareVSize = n + 200;
		sxiShareV = (SExpr *) stoAlloc(OB_SExpr, sxiShareVSize
				* sizeof(SExpr));
		memcpy(sxiShareV, oldV, oldVSize * sizeof(SExpr));

		for (i = oldVSize; i < sxiShareVSize; i++)
			sxiShareV[i] = sxiShareVUninit;
	}
	sxiShareV[n] = s;
	if (!sxiNull(s))
		s->sxHdr.isShared = true;
	return s;
}

local int sxiShareVFind(SExpr s)
{
	int i;
	for (i = 0; i < sxiShareVSize; i++)
		if (s == sxiShareV[i])
			return i;
	return -1;
}

local SExpr sxiShareVGet(int n)
{
	return (n >= sxiShareVSize) ? sxiShareVUninit : sxiShareV[n];
}
/*****************************************************************************
 *
 * :: S-Expression Reader
 *
 ****************************************************************************/
local SExpr sxiRd(FILE *, int context);
local SExpr sxiRd0(FILE *, int context);
local SExpr sxiRdScanToken(FILE *, int context, int firstChar);

#define sxiRd_PackageMarker	'\01'

/* Contexts determine what is legal.  E.g. eof, parens, dots .... */
enum sxiRdContext {
	sxiRd_Top,
	sxiRd_Neutral,
	sxiRd_InList,
	sxiRd_InVector,
	sxiRd_UninternedSymbol
};

/* Reader state is the sxiIoBuf, sxiShareV, plus: */
static FileName sxiRdFName = NULL; /* Name of current file */
static int sxiRdLineNo; /* Line number in file  */
static int sxiRdGLineNo; /* Global line number in file  */
static int sxiRdCharNo; /* Character position   */
static int sxiRdLastCharNo; /* Position before \n 	*/
static SrcPos sxiRdPos; /* Source position      */

static jmp_buf sxiRdCatch;
static int sxiRdBackQuoteDepth;
static SExpr sxiRdEofVal;
static SExpr sxiRdErrVal;

/* Macros for getting characters */
#define sxiRdGetc(pc, inf)  { 					\
	if ((*(pc) = getc(inf)) == '\n') {			\
/*		sxiRdLineNo++; 				*/	\
		sxiRdGLineNo++;					\
		sxiRdLastCharNo = sxiRdCharNo;			\
		sxiRdCharNo = 1;				\
		sxiRdPos = sposNew(sxiRdFName,  sxiRdLineNo,	\
				   sxiRdGLineNo, sxiRdCharNo); 	\
	}							\
	else							\
		sxiRdCharNo++;					\
}
#define sxiRdUngetc(c, inf) { 					\
	if (ungetc(c, inf) == '\n') { 				\
/*		sxiRdLineNo--;				*/	\
		sxiRdGLineNo--;					\
		sxiRdCharNo = sxiRdLastCharNo;			\
		sxiRdPos = sposNew(sxiRdFName,  sxiRdLineNo,	\
				   sxiRdGLineNo, sxiRdCharNo); 	\
	}							\
	else							\
		sxiRdCharNo--;					\
}
#define sxiRdPeekc(pc, inf) ungetc(getc(inf), inf)

#define sxiRdGetcOrElse(pc,in) {				\
	int	_c;						\
	sxiRdGetc(&_c, in);					\
	if (_c == EOF) sxiRdError(SX_ErrReadEOF);		\
	*(pc) = _c;						\
}

#define sxiRdGetcOrGoto(pc,in,lab) {				\
	int	_c;						\
	sxiRdGetc(&_c, in);					\
	if (_c == EOF) goto lab;				\
	*(pc) = _c;						\
}

/*
 * Read an expression from the input stream.  Return sxNil if not possible.
 */
SExpr sxiRead(FILE *inf, FileName *pfname, int *plineno, SExpr eofval,
		ULong iomode)
{
	Scope("sxiRead");

	SExpr sx;
	FileName fn = 0;
	int fluid(sxiIoMixedCase), fluid(sxiIoKeepSrcPos);

	if (iomode & SXRW_MixedCase)
		sxiIoMixedCase = 1;
	if (iomode & SXRW_FoldCase)
		sxiIoMixedCase = 0;
	if (iomode & SXRW_SrcPos)
		sxiIoKeepSrcPos = 1;
	if (iomode & SXRW_NoSrcPos)
		sxiIoKeepSrcPos = 0;
	if (iomode & SXRW_Packages)
		bug("package input not supported");

	sxiRdFName = pfname ? *pfname : NULL;
	sxiRdLineNo = plineno ? *plineno : 1;
	sxiRdGLineNo = 1; /* plineno ? *plineno : 1; */
	sxiRdCharNo = 1;
	sxiRdPos = sposNew(sxiRdFName, sxiRdLineNo, sxiRdGLineNo, 1);

	sxiRdBackQuoteDepth = 0;
	sxiRdEofVal = eofval;

	sxiIoTableInit();
	sxiShareVStart();

	if (setjmp(sxiRdCatch) == 0)
		sx = sxiRd(inf, sxiRd_Top);
	else
		sx = sxiRdErrVal;

	sxiRdEofVal = 0;
	sxiRdErrVal = 0;

	if (plineno)
		*plineno = sxiRdLineNo;
	if (pfname)
		*pfname = sxiRdFName;
	if (fn)
		fnameFree(fn);

	Return(sx)
;}

void sxiCommentChk(void)
{
	int i, lflag = 0, glflag = 0, fnflag = 0;
	String str, name;
	Buffer blno, bglno, bfn;
	Length lno, glno;
	FileName fn;

	sxiCommentBufStart();
	str = sxiCommentBufChars();
	if (!strIsPrefix("line", str))
		return; sexprDEBUG {
				if (strIsPrefix("line", str))
				fprintf(dbOut, "!!! %s\n", str);
			}
	blno = bufNew();
	bglno = bufNew();
	bfn = bufNew();
	BUF_START(blno);
	BUF_START(bglno);
	BUF_START(bfn);
	for (i = 0; i < bufSize(sxiCommentBuf); i++) {
		if (str[i] == ' ') {
			lflag = !lflag;
			i++;
		}
		if (str[i] == '[') {
			glflag = !glflag;
			i++;
		}
		if (str[i] == ']') {
			glflag = !glflag;
			i++;
		}
		if (str[i] == '"') {
			fnflag = !fnflag;
			i++;
		}
		if (lflag)
			BUF_ADD1(blno, str[i]);
		if (glflag)
			BUF_ADD1(bglno, str[i]);
		if (fnflag && str[i] != '\n')
			BUF_ADD1(bfn, str[i]);
	}
	BUF_ADD1(blno, char0);
	BUF_ADD1(bglno, char0);
	BUF_ADD1(bfn, char0);
	sexprDEBUG {
		fprintf(dbOut, "\n!!! %d, %d ", sxiRdLineNo, sxiRdGLineNo);
		if (sxiRdFName)
			fprintf(dbOut, "%s\n", fnameUnparseStatic(sxiRdFName));
	}
	lno = atol(bufChars(blno));
	glno = atol(bufChars(bglno));
	sxiRdGLineNo = glno;
	name = bufChars(bfn);
	if (lno != sxiRdLineNo)
		sxiRdLineNo = lno;
	if (name && strlen(name) > 1) {
		fn = fnameParseStatic(name);
		if (!sxiRdFName || !fnameEqual(fn, sxiRdFName))
			sxiRdFName = fnameCopy(fn);
	}
	sxiRdPos = sposNew(sxiRdFName, sxiRdLineNo, sxiRdGLineNo, sxiRdCharNo);
	sexprDEBUG {
		fprintf(dbOut, "\n!!! %d, %d ", sxiRdLineNo, sxiRdGLineNo);
		fprintf(dbOut, "%s\n", fnameUnparseStatic(sxiRdFName));
		fprintf(dbOut, "%ld\n", sxiRdPos);
	}
	bufFree(blno);
	bufFree(bglno);
	bufFree(bfn);
}

local SExpr sxiRdSlurpSpaces(FILE *inf, int context)
{
	int c;

	for (;;) {
		sxiRdGetc(&c, inf);
		if (c == EOF) {
			if (context == sxiRd_Top)
				return sxiRdEofVal;
			else
				sxiRdError(SX_ErrReadEOF);
		}
		if (sxiIoKind(c) == SX_IoSpace)
			continue;

		if (c == ';') {
			sxiCommentBufStart();
			do {
				sxiRdGetc(&c, inf);
				sxiCommentBufAdd(c);
			} while (c != '\n' && c != EOF);
			sxiCommentBufEnd();
			sxiCommentChk();
			continue;
		}

		sxiRdUngetc(c, inf);
		break;
	}
	return 0;
}

local SExpr sxiRd(FILE *inf, int context)
{
	SrcPos pos0;
	Length off0;
	SExpr sx;

	sx = sxiRdSlurpSpaces(inf, context);
	if (sx)
		return sx;

	pos0 = sxiRdPos;
	off0 = sxiRdCharNo - 1;
	sx = sxiRd0(inf, context);
	if (sxiIoKeepSrcPos)
		sx = sxiRepos(sposOffset(pos0, off0), sx);
	return sx;
}

local SExpr sxiRd0(FILE *inf, int context)
{
	int c;
	SExpr s;

	start:
	sxiRdGetc(&c, inf);
	if (c == EOF) {
		if (context == sxiRd_Top)
			return sxiRdEofVal;
		else
			sxiRdError(SX_ErrReadEOF);
	}

	/*
	 * Dispatch on first character to determine appropriate read action.
	 */
	switch (sxiIoKind(c)) {
	case SX_IoSpace:
		bug("This should never happen -- space in sxRd0\n");
		break;

	case SX_IoSingEsc:
	case SX_IoMultEsc:
	case SX_IoConstit:
		return sxiRdScanToken(inf, context, c);

	case SX_IoTermMac:
		switch (c) {
		case ';':
			bug("This should never happen -- comment in sxRd0\n");
			break;
		case '"':
			sxiIoBufStart();
			for (;;) {
				sxiRdGetcOrElse(&c, inf);
				if (c == '"')
					break;
				if (c == '\\')
					sxiRdGetcOrElse(&c, inf);
				sxiIoBufAdd(c);
			}
			sxiIoBufEnd();
			return sxiFrString(sxiIoBufChars());
		case '\'':
			s = sxiRd(inf, sxiRd_Neutral);
			s = sxCons(sx0Quote, sxCons(s, sxNil));
			return s;
		case '(': {
			SExpr s0, t;
			s = sxiRd(inf, sxiRd_InList);
			if (s == sxiRd_AParen)
				return sxNil;
			if (s == sxiRd_ADot)
				sxiRdError(SX_ErrBadPunct, ".");

			s0 = t = sxCons(s, sxNil);
			for (;;) {
				s = sxiRd(inf, sxiRd_InList);
				if (s == sxiRd_ADot) {
					sxCdr(t) = sxiRd(inf,
							sxiRd_Neutral);
					s = sxiRd(inf, sxiRd_InList);
					if (s != sxiRd_AParen)
						sxiRdError(SX_ErrBadPunct, ".");
					return s0;
				}
				if (s == sxiRd_AParen)
					return s0;
				sxCdr(t) = sxCons(s, sxNil);
				t = sxCdr(t);
			}
		}
		case ')':
			if (context != sxiRd_InList && context
					!= sxiRd_InVector)
				sxiRdError(SX_ErrBadPunct, ")");
			return sxiRd_AParen;
		case '`':
			sxiRdBackQuoteDepth++;
			s = sxiRd(inf, sxiRd_Neutral);
			s = sxCons(sx0BackQuote, sxCons(s, sxNil));
			sxiRdBackQuoteDepth--;
			return s;
		case ',': {
			SExpr op;

			if (sxiRdBackQuoteDepth <= 0)
				sxiRdError(SX_ErrBadPunct, ",");

			sxiRdGetcOrElse(&c, inf);

			switch (c) {
			case '.':
				op = sx0BackQuoteCommaDot;
				break;
			case '@':
				op = sx0BackQuoteCommaAt;
				break;
			default:
				sxiRdUngetc(c, inf)
				;
				op = sx0BackQuoteComma;
			}
			s = sxiRd(inf, sxiRd_Neutral);
			return sxCons(op, sxCons(s, sxNil));
		}
		default:
			bugBadCase(c);
		}

	case SX_IoNonTMac: {
		int numericArg = -1; /* E.g.	 #16Rxxx */

		M_More:
		sxiRdGetcOrElse(&c, inf);
		if (sxiIoKind(c) == SX_IoIllegal)
			sxiRdError(SX_ErrMacroIlleg, c);

		switch (c) {

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (numericArg == -1)
				numericArg = c - '0';
			else
				numericArg = 10 * numericArg + (c - '0');
			goto M_More;

		case ')':
		case '<':
		case ' ':
		case '\t':
		case '\n':
		case '\f':
		case '\r':
		case '\b':
			sxiRdError(SX_ErrMacroIlleg, c);

		case '\'':
			if (numericArg != -1)
				sxiRdError(SX_ErrCantMacroArg, c);
			s = sxiRd(inf, sxiRd_Neutral);
			s = sxCons(sx0Function, sxCons(s, sxNil));
			return s;

		case '\\': {
			char *str;
			if (numericArg != -1)
				sxiRdError(SX_ErrCantMacroArg, c);
			sxiRdUngetc(c, inf); /* Put \ back */
			s = sxiRd(inf, sxiRd_Neutral);
			if (!sxiSymbolP(s))
				sxiRdError(SX_ErrBadCharName);

			str = strUpper(sxiToTheString(sxSymbolName(s)));

			if (str[0] && !str[1])
				return sxiFrChar(str[0]);
			if (!strcmp(str, "SPACE"))
				return sxiFrChar(' ');
			if (!strcmp(str, "NEWLINE"))
				return sxiFrChar('\n');
			if (!strcmp(str, "LINEFEED"))
				return sxiFrChar('\n');
			if (!strcmp(str, "PAGE"))
				return sxiFrChar('\f');
			if (!strcmp(str, "TAB"))
				return sxiFrChar('\t');
			if (!strcmp(str, "BACKSPACE"))
				return sxiFrChar('\b');
			if (!strcmp(str, "RETURN"))
				return sxiFrChar('\r');
			if (!strcmp(str, "RUBOUT"))
				return sxiFrChar(RUBOUT);

			sxiRdError(SX_ErrBadCharName);
		}
		case ':':
			if (numericArg != -1)
				sxiRdError(SX_ErrCantMacroArg, c);
			sxiRdGetcOrElse(&c, inf)
			;
			return sxiRdScanToken(inf, sxiRd_UninternedSymbol, c);

		case '+':
		case '-': {
			SExpr f, b;
			if (numericArg != -1)
				sxiRdError(SX_ErrCantMacroArg, c);
			f = sxiRd(inf, sxiRd_Neutral);
			s = sxiRd(inf, sxiRd_Neutral);

			b = sx0EvalFeatureForm(f, sx0Quote);
			if (b == sx0Quote)
				sxiRdError(SX_ErrBadFeatureForm);

			if ((c == '+' && b) || (c == '-' && !b))
				return s;
			else
				goto start;
		}
		case '=':
			if (numericArg == -1)
				sxiRdError(SX_ErrMustMacroArg, c);
			s = sxiShareVGet(numericArg);
			if (s != sxiShareVUninit)
				sxiRdError(SX_ErrAlreadyShare);
			s = sxiRd(inf, sxiRd_Neutral);
			sxiShareVAdd(numericArg, s);
			return s;

		case '#':
			if (numericArg == -1)
				sxiRdError(SX_ErrMustMacroArg, c);
			s = sxiShareVGet(numericArg);
			if (s == sxiShareVUninit)
				sxiRdError(SX_ErrCantShare);

			return s;

		case '(': {
			SExpr s0;
			int i, n;

			s0 = sxNil;
			n = 0;

			while ((s = sxiRd(inf, sxiRd_InVector)) != sxiRd_AParen) {
				s0 = sxCons(s, s0);
				n++;
			}

			if (numericArg == -1)
				numericArg = n;
			if (numericArg < n)
				sxiRdError(SX_ErrTooManyElts);

			s = sxiMakeVector(numericArg);

			for (i = 0; i < n; i++, s0 = sxCdr(s0))
				s->sxVector.argv[n - i - 1] = sxCar(s0);

			for (i = n; i < numericArg; i++)
				s->sxVector.argv[i] = s->sxVector.argv[n - 1];

			return s;
		}
		case '|': {
			/*  Balanced comments: #| ... |# */
			int depth = 1;

			while (depth > 0) {
				sxiRdGetcOrElse(&c, inf);
				if (c == '|') {
					sxiRdGetcOrElse(&c, inf);
					if (c == '#')
						--depth;
					continue;
				}
				if (c == '#') {
					sxiRdGetcOrElse(&c, inf);
					if (c == '|')
						++depth;
					continue;
				}
			}
			goto start;
		}

		case 'C':
		case 'c':
			s = sxiRd(inf, sxiRd_Neutral);
			if (!sxiConsP(s) || !sxiConsP(sxCdr(s))
					||
					!sxiNull(sxCddr(s)))
				sxiRdError(SX_ErrBadComplexNum);
			return sxComplex(sxCar(s), sxCadr(s));

		case ',':
			s = sxiRd(inf, sxiRd_Neutral);
			return sxCons(sx0LoadTimeEval, sxCons(s, sxNil));
		case '.':
			s = sxiRd(inf, sxiRd_Neutral);
			return sxCons(sx0ReadTimeEval, sxCons(s, sxNil));

		case '*':
		case 'A':
		case 'B':
		case 'O':
		case 'R':
		case 'S':
		case 'X':
		case 'a':
		case 'b':
		case 'o':
		case 'r':
		case 's':
		case 'x':
			sxiRdError(SX_ErrMacroUnimp, c);
		default:
			sxiRdError(SX_ErrMacroUndef, c);
		}
	}

	case SX_IoIllegal:
		sxiRdError(SX_ErrBadChar, c);

	default:
		bugBadCase(c);
	}
NotReached(return sxNil);
}

local SExpr sxiRdScanToken(FILE *inf /* rest */, int context, int c /* first */)
{
	/*
	 * Finite state machine to collect token
	 */
	int anyEscaped = 0;
	int anyPkgMark = 0;
	int i;
	int numKind;
	char *str;

	sxiIoBufStart();

	T_norm: /* Not in multi-escape collection. Dispatch on char. */
	if (c == EOF)
		goto T_done;

	switch (sxiIoKind(c)) {
	case SX_IoSpace:
	case SX_IoTermMac:
		sxiRdUngetc(c, inf)
		;
		goto T_done;
	case SX_IoNonTMac:
	case SX_IoConstit:
		if (!sxiIoMixedCase)
			c = toupper(c);
		if (c == ':') {
			c = sxiRd_PackageMarker;
			anyPkgMark = 1;
		}
		sxiIoBufAdd(c);
		sxiRdGetcOrGoto(&c, inf, T_done)
		;
		goto T_norm;
	case SX_IoSingEsc:
		sxiRdGetcOrElse(&c, inf)
		;
		sxiIoBufAdd(c);
		sxiRdGetcOrGoto(&c, inf, T_done)
		;
		anyEscaped = 1;
		goto T_norm;
	case SX_IoMultEsc:
		sxiRdGetcOrElse(&c, inf)
		;
		goto T_multi;
	case SX_IoIllegal:
		sxiRdError(SX_ErrBadChar, c);
	default:
		bugBadCase(c);
	}

	T_multi: /* In multi-escape collection. Dispatch on char. */
	if (c == EOF)
		sxiRdError(SX_ErrReadEOF); /* redundant */
	anyEscaped = 1; /* considered escaped even if empty */

	switch (sxiIoKind(c)) {
	case SX_IoSpace:
	case SX_IoTermMac:
	case SX_IoNonTMac:
	case SX_IoConstit:
		sxiIoBufAdd(c);
		sxiRdGetcOrElse(&c, inf)
		;
		goto T_multi;
	case SX_IoSingEsc:
		sxiRdGetcOrElse(&c, inf)
		;
		sxiIoBufAdd(c);
		sxiRdGetcOrElse(&c, inf)
		;
		goto T_multi;
	case SX_IoMultEsc:
		sxiRdGetcOrGoto(&c, inf, T_done)
		;
		goto T_norm;
	case SX_IoIllegal:
		sxiRdError(SX_ErrBadChar, c);
	default:
		bugBadCase(c);
	}

	T_done: sxiIoBufEnd();

	/*
	 * Now decide how to convert token to S-expression.
	 */

	str = sxiIoBufChars();

	if (context == sxiRd_UninternedSymbol) {
		if (anyPkgMark)
			sxiRdError(SX_ErrBadUninterned);
		return sx0InternInFrString(str, sxNil);
	}
	if (strEqual(str, "NIL"))
		return sxNil;

	if (anyEscaped && !anyPkgMark)
		return sxIntern(sxiFrString(str));

	if (str[0] == '.' && str[1] == 0 && context == sxiRd_InList)
		return sxiRd_ADot;

	numKind = sxiIoIsPotentialNumber(str);
	switch (numKind) {

	case SX_IoNumInteger: {
		SExpr s = sxiFrBigInteger(bintFrString(str));
		s->sxHdr.wrWidth = strlen(str);
		return s;
	}
	case SX_IoNumRatio: {
		char *s = str, *t;
		long n, d;

		t = strchr(s, '/');
		if (s == t) {
			d = 1;
		} else {
			*t++ = 0;
			d = atol(t);
		}
		n = atol(s);
		return sxiMakeRatio(n, d);
	}
	case SX_IoNumFloat: {
		char *s = str, exp;
		/* Convert exponent marker to an 'e' then use C fn. */
		exp = sxiExponentCharacter(s);
		return sxiFrFloat(DFloatScan(s), exp);
	}
	case SX_IoNumNot: {
		/* It must be a symbol! */
		/* Check for all dots  and package markers */
		char *s = str, *t;
		int nDots = 0;
		int nPkgs = 0;
		SExpr pkg;

		for (i = 0; s[i] != 0; i++) {
			if (s[i] == '.')
				nDots++;
			if (s[i] == sxiRd_PackageMarker)
				nPkgs++;
		}
		if (nDots == i)
			sxiRdError(SX_ErrBadToken);
		if (nPkgs > 2)
			sxiRdError(SX_ErrBadToken);
		if (nPkgs == 0)
			return sxIntern(sxiFrString(s));

		/* Split into package and name */
		t = strchr(s, sxiRd_PackageMarker);
		if (nPkgs == 2 && t[1] != sxiRd_PackageMarker)
			sxiRdError(SX_ErrBadToken);

		if (s == t)
			pkg = sx0KeywordPackage;
		else {
			*t = 0;
			pkg = sxiFindPackage(s);
			if (sxiNull(pkg))
				pkg = sxMakePackage(sxiFrString(s));
		}
		t += nPkgs;

		return sx0InternInFrString(t, pkg);
	}
	case SX_IoNumPotential:
		sxiRdError(SX_ErrBadPotNum, str);
	default:
		bugBadCase(numKind);
	}

NotReached(return sxNil);
}

/*****************************************************************************
 *
 * :: S-Expression Writer
 *
 ****************************************************************************/

int sxiWrite0(FILE *, SExpr);
int sxiWrWidth(SExpr);
int sxiWrUnscanToken(SExpr); /* To sxiIoBuf */
void sxiWrUnscan0SymbolName(SExpr); /* To sxiIoBuf */
void sxiWrUnscan0DoubleColonName(String);
int sxiWrNewline(FILE *);

int sxiWrLead; /* Current leading spaces */
int sxiWrIndent = 2; /* Per-level indentation */
int sxiWrMaxText = 70; /* Maximum text on a line */

int sxiWrite(FILE *outf, SExpr s, ULong iomode)
{
	Scope("sxiWrite");
	int cc = 0;
	int fluid(sxiIoMixedCase), fluid(sxiIoKeepSrcPos);
	int fluid(sxiIoOutPackages);

	if (iomode & SXRW_MixedCase)
		sxiIoMixedCase = 1;
	if (iomode & SXRW_FoldCase)
		sxiIoMixedCase = 0;
	if (iomode & SXRW_SrcPos)
		sxiIoKeepSrcPos = 1;
	if (iomode & SXRW_NoSrcPos)
		sxiIoKeepSrcPos = 0;
	if (iomode & SXRW_Packages)
		sxiIoOutPackages = 1;

	sxiWrLead = 0;
	sxiShareVStart();
	sxiWrWidth(s);
	sxiShareVStart();

	if (sxiIoKeepSrcPos) {
		sxiCurrentLineNo = 1;
		if (sxiRdFName) {
			sxiCurrentPos = sxiPos(s);
			sxiCurrentFName = sposFile(sxiCurrentPos);
			cc += fprintf(outf, ";line %d [%d]",
					(int) sxiCurrentLineNo,
					(int) sposGlobalLine(sxiCurrentPos));
			cc += fprintf(outf, " \"%s\"\n", fnameUnparseStatic(
					sxiCurrentFName));
		} else {
			sxiCurrentPos = sposNone;
			sxiCurrentFName = sposFile(sxiCurrentPos);
		}
	}
	cc += sxiWrite0(outf, s);
	cc += fprintf(outf, "\n");

	Return(cc)
;}

int sxiWrite0(FILE *outf, SExpr s)
{
	int cc = 0;

#if 0 /* This breaks the format of sexprs. */
	sexprDEBUG {
		SrcPos spos = sxiPos(s);
		fprintf(outf,"{%d.%d}", (int) sposLine(spos), (int) sposChar(spos));
	}
#endif

	/*
	 * Things which are not displayed using #nn#, even if shared.
	 */
	switch (sxiTag(s)) {
	case SX_Nil:
		cc += fprintf(outf, "()");
		return cc;
	case SX_Symbol:
	case SX_Integer:
	case SX_Float:
	case SX_Char:
		sxiWrUnscanToken(s);
		cc += fprintf(outf, "%s", sxiIoBufChars());
		return cc;
	default:
		break;
	}

	if (sxiIsShared(s)) {
		int f = sxiShareVFind(s);
		sxiCurrentPos = sxiPos(s);
		if (f == -1) {
			sxiShareVAdd(-1, s);
			cc += fprintf(outf, "#%d=", sxiShareVIx);
			/* Continue */
		} else {
			cc += fprintf(outf, "#%d#", f);
			return cc;
		}
	}

	switch (sxiTag(s)) {
	case SX_String:
		sxiWrUnscanToken(s);
		cc += fprintf(outf, "%s", sxiIoBufChars());
		break;
	case SX_Ratio:
		cc += sxiWrite0(outf, sxNumerator(s));
		cc += fprintf(outf, "/");
		cc += sxiWrite0(outf, sxDenominator(s));
		break;
	case SX_Complex:
		cc += fprintf(outf, "#C(");
		cc += sxiWrite0(outf, sxRealpart(s));
		cc += fprintf(outf, " ");
		cc += sxiWrite0(outf, sxImagpart(s));
		cc += fprintf(outf, ")");
		break;
	case SX_Vector: {
		int newl = s->sxHdr.wrWidth > sxiWrMaxText;
		int i;

		sxiWrLead += sxiWrIndent;
		cc += fprintf(outf, "#(");

		sxiCurrentPos = sxiPos(s);

		for (i = 0; i < sx0Size(s); i++) {
			if (i > 0) {
				if (newl)
					cc += fprintf(outf, "\n%*s", sxiWrLead,
							"");
				else
					cc += fprintf(outf, " ");
			}
			cc += sxiWrite0(outf, sx0Elt(s, i));
		}
		cc += fprintf(outf, ")");
		sxiWrLead -= sxiWrIndent;
		break;
	}
	case SX_Cons: {
		SExpr kar = sxCar(s);
		SExpr kdr = sxCdr(s);
		char *str = 0;
		int newl = s->sxHdr.wrWidth > sxiWrMaxText;

		sxiWrLead += sxiWrIndent;

		if (sxiConsP(kdr) && sxiNull(sxCdr(kdr))) {
			if (kar == sx0Quote)
				str = "'";
			else if (kar == sx0Function)
				str = "#'";
			else if (kar == sx0BackQuote)
				str = "`";
			else if (kar == sx0BackQuoteComma)
				str = ",";
			else if (kar == sx0BackQuoteCommaAt)
				str = ",@";
			else if (kar == sx0BackQuoteCommaDot)
				str = ",.";
			else if (kar == sx0LoadTimeEval)
				str = "#,";
			else if (kar == sx0ReadTimeEval)
				str = "#.";
		}
		if (str) {
			cc += fprintf(outf, "%s", str);
			cc += sxiWrite0(outf, sxCar(kdr));
		} else {
			sxiCurrentPos = sxiPos(s);
			if (sxiIoKeepSrcPos)
				cc += sxiWrNewline(outf);
			cc += fprintf(outf, "(");
			cc += sxiWrite0(outf, sxCar(s));
			s = sxCdr(s);
			while (sxiConsP(s)) {
				if (newl) {
					if (sxiIoKeepSrcPos)
						cc += sxiWrNewline(outf);
					else
						cc += fprintf(outf, "\n%*s",
								sxiWrLead, "");
				} else
					cc += fprintf(outf, " ");
				cc += sxiWrite0(outf, sxCar(s));
				s = sxCdr(s);
			}
			if (!sxiNull(s)) {
				if (newl)
					cc += fprintf(outf, " .\n%*s",
							sxiWrLead, "");
				else
					cc += fprintf(outf, " . ");
				cc += sxiWrite0(outf, s);
			}
			cc += fprintf(outf, ")");
		}
		sxiWrLead -= sxiWrIndent;
		break;
	}
	default:
		cc += fprintf(outf, "#<Unknown>");
	}

	return cc;
}

int sxiWrNewline(FILE *outf)
{
	int cc = 0;
	FileName fn;
	Length ln;

	fn = sposFile(sxiCurrentPos);
	ln = sposLine(sxiCurrentPos);
	if (ln < 1)
		ln = 1;
	/* If compiled with -g, write out ;line information. */
	if (sxiIoKeepSrcPos && fn) {
		if (!sxiCurrentFName)
			sxiCurrentFName = fnameCopy(fn);
		if (ln != sxiCurrentLineNo)
			sxiCurrentLineNo = ln;
		cc += fprintf(outf, "\n;line %d [%d] ", (int) sxiCurrentLineNo,
				(int) sposGlobalLine(sxiCurrentPos));
		if (!fnameEqual(sxiCurrentFName, fn)) {
			sxiCurrentFName = fnameCopy(fn);
			cc += fprintf(outf, " \"%s\"", fnameUnparseStatic(
					sxiCurrentFName));
		}
	}
	cc += fprintf(outf, "\n%*s", sxiWrLead, "");
	return cc;
}

/*
 * Compute the print width of an S-expression.
 * Short subexpressions have their widths cached in the "reserved" field.
 */

#define SX_PrWIDE	((MAX_BYTE + 1)/2 - 1)		/* BYTE_BITS-1 bits */

int sxiWrWidth(SExpr s)
{
	int n, nmore = 0;

	if (sxiIsShared(s) && sxiTag(s) != SX_Symbol) {
		char buf[25];
		int f = sxiShareVFind(s);

		if ((f -= -1) != 0) {
			sxiShareVAdd(-1, s);
			sprintf(buf, "#%d=", sxiShareVIx);
			nmore = strlen(buf);
		} else {
			sprintf(buf, "#%d#", f);
			return strlen(buf);
		}
	}

	switch (sxiTag(s)) {
	case SX_Nil:
		n = 2; /* () */
		break;
	case SX_Symbol:
		n = sxiWrUnscanToken(s);
		break;
	case SX_Integer:
		n = s->sxHdr.wrWidth;
		if (n != 0)
			break;
		n = sxiWrUnscanToken(s);
		break;
	case SX_Float:
	case SX_Char:
	case SX_String:
		n = sxiWrUnscanToken(s);
		break;
	case SX_Ratio:
		n = 1 + sxiWrWidth(sxNumerator(s)) + sxiWrWidth(
				sxDenominator(s));
		break;
	case SX_Complex:
		n = 5 + sxiWrWidth(sxRealpart(s)) + sxiWrWidth(sxImagpart(s));
		break;
	case SX_Vector: {
		int i;

		if (sx0Size(s) > 1)
			n = 3 + sx0Size(s) - 1; /* #() plus blank seps */
		else
			n = 3; /* #() */

		for (i = 0; i < sx0Size(s); i++)
			n += sxiWrWidth(sx0Elt(s, i));
		break;
	}
	case SX_Cons: {
		char *str = 0;
		SExpr kar = sxCar(s);
		SExpr kdr = sxCdr(s);
		int karWidth, kdrWidth;

		if (sxiConsP(kdr) && sxiNull(sxCdr(kdr))) {
			if (kar == sx0Quote)
				str = "'";
			else if (kar == sx0Function)
				str = "#'";
			else if (kar == sx0BackQuote)
				str = "`";
			else if (kar == sx0BackQuoteComma)
				str = ",";
			else if (kar == sx0BackQuoteCommaAt)
				str = ",@";
			else if (kar == sx0BackQuoteCommaDot)
				str = ",.";
		}
		if (str) {
			n = strlen(str) + sxiWrWidth(sxSecond(s));
		} else {
			karWidth = sxiWrWidth(kar);
			kdrWidth = sxiWrWidth(kdr);

			if (sxiNull(kdr)) /* (E) */
				n = 1 + karWidth + 1;
			else if (sxiConsP(kdr)) /* (A d) where D = (d) */
				n = 1 + karWidth + 1 + kdrWidth + 1 - 2;
			else
				/* (A . D) */
				n = 1 + karWidth + 3 + kdrWidth + 1;
		}
		break;
	}
	default:
		n = 10;
	}

	s->sxHdr.wrWidth = (n > SX_PrWIDE) ? SX_PrWIDE : n;
	return n + nmore;
}

int sxiWrUnscanToken(SExpr s)
{
	char *str;

	sxiIoBufStart();

	switch (sxiTag(s)) {
	case SX_Integer: {
		BInt b;

		b = sxiToTheBigInteger(s);

		if (bintIsSmall(b)) {
			char buf[50];
			sprintf(buf, "%ld", (long) bintSmall(b));
			sxiIoBufPuts(buf);
		} else {
			char *str0 = bintToString(b);
			sxiIoBufPuts(str0);
			strFree(str0);
		}
		break;
	}
	case SX_Float: {
		char buf[MAX_FLOAT_SIZE];
		char *c;

		DFloatSprint(buf, sxiToFloat(s));

		for (c = buf; *c; c++)
			if (isalpha(*c)) {
				*c = s->sxFloat.marker;
				break;
			}
		if (!*c) {
			/* if no exponent marker, add one */
			*c++ = s->sxFloat.marker;
			*c++ = '0';
			*c = '\0';
		}

		sxiIoBufPuts(buf);
		break;
	}
	case SX_Char: {
		char buf[25];
		int c = sxiToChar(s);

		switch (c) {
		case ' ':
			sprintf(buf, "#\\Space");
			break;
		case '\n':
			sprintf(buf, "#\\Newline");
			break;
		case '\f':
			sprintf(buf, "#\\Page");
			break;
		case '\t':
			sprintf(buf, "#\\Tab");
			break;
		case '\b':
			sprintf(buf, "#\\Backspace");
			break;
		case '\r':
			sprintf(buf, "#\\Return");
			break;
		case RUBOUT:
			sprintf(buf, "#\\Rubout");
			break;
		case 0:
			sprintf(buf, "#.(code-char 0)");
			break;
		default:
			sprintf(buf, "#\\%c", c);
			break;
		}
		for (str = buf; *str; str++)
			sxiIoBufAdd(*str);
		break;
	}
	case SX_String:
		sxiIoBufAdd('"');
		for (str = sxiToTheString(s); *str; str++) {
			if (*str == '"' || *str == '\\')
				sxiIoBufAdd('\\');
			sxiIoBufAdd(*str);
		}
		sxiIoBufAdd('"');
		break;
	case SX_Symbol: {
		SExpr pkg = sxSymbolPackage(s);

		/* Package portion */
		if (sxiNull(pkg)) {
			sxiIoBufAdd('#');
			sxiIoBufAdd(':');
		} else if (pkg == sx0KeywordPackage) {
			sxiIoBufAdd(':');
		} else if (pkg == sxSTAR_Package_STAR || pkg == sx0LispPackage) {
			/* Nothing */
			/*!! Assume everyone inherits Lisp */
		} else {
			sxiWrUnscan0SymbolName(pkg->sxPackage.nameSymbol);
			sxiIoBufAdd(':');
			sxiIoBufAdd(':');
		}

		/* Name portion */
		sxiWrUnscan0SymbolName(s);
		break;
	}
	default:
		break;
	}
	sxiIoBufEnd();

	return strlen(sxiIoBufChars());
}

void sxiWrUnscan0SymbolName(SExpr sym)
{
	char *str = sxiToTheString(sxSymbolName(sym));
	int nesc = (sym)->sxSymbol.wrInfo;

	if (sxiIoMixedCase)
		switch (nesc) {
		case SX_IoEscDont:
		case SX_IoEscForLower:
			sxiIoBufPuts(str);
			break;
		case SX_IoEscForNumber:
		case SX_IoEscForLowerNumber:
			sxiIoBufAdd('\\');
			sxiIoBufPuts(str);
			break;
		case SX_IoEscForDoubleColon:
		case SX_IoEscForSpecials:
			sxiIoBufAdd('|');
			for (; *str; str++) {
				if (*str == '|' || *str == '\\')
					sxiIoBufAdd('\\');
				sxiIoBufAdd(*str);
			}
			sxiIoBufAdd('|');
			break;
		}
	else
		switch (nesc) {
		case SX_IoEscDont:
			for (; *str; str++)
				sxiIoBufAdd(tolower(*str));
			break;
		case SX_IoEscForNumber:
			sxiIoBufAdd('\\');
			sxiIoBufPuts(str);
			break;
		case SX_IoEscForDoubleColon:
			sxiWrUnscan0DoubleColonName(str);
			break;
		case SX_IoEscForSpecials:
		case SX_IoEscForLower:
		case SX_IoEscForLowerNumber:
			sxiIoBufAdd('|');
			for (; *str; str++) {
				if (*str == '|' || *str == '\\')
					sxiIoBufAdd('\\');
				sxiIoBufAdd(*str);
			}
			sxiIoBufAdd('|');
			break;
		}
}

/* 
 * A bit tacky, but means that boot::|DoSillyThing| can be
 * generated.
 */

void sxiWrUnscan0DoubleColonName(String str)
{
	int nesc;

	for (; *str != ':'; str++)
		sxiIoBufAdd(*str);
	sxiIoBufPuts("::");
	str += 2;
	nesc = sxiIoIsNeedingEscape(str);

	switch (nesc) {
	case SX_IoEscDont:
		for (; *str; str++)
			sxiIoBufAdd(tolower(*str));
		break;
	case SX_IoEscForNumber:
		sxiIoBufAdd('\\');
		sxiIoBufPuts(str);
		break;
	case SX_IoEscForDoubleColon:
	case SX_IoEscForSpecials:
	case SX_IoEscForLower:
	case SX_IoEscForLowerNumber:
		sxiIoBufAdd('|');
		for (; *str; str++) {
			if (*str == '|' || *str == '\\')
				sxiIoBufAdd('\\');
			sxiIoBufAdd(*str);
		}
		sxiIoBufAdd('|');
		break;
	}

}

/*****************************************************************************
 *
 * :: Other operations
 *
 ****************************************************************************/

Bool sxiEq(SExpr a, SExpr b)
{
	if (sxiTag(a) != sxiTag(b))
		return false;

	switch (sxiTag(a)) {
	case SX_Nil:
		return true;
	case SX_Symbol:
		return a->sxSymbol.sym == b->sxSymbol.sym && sxSymbolPackage(a)
				== sxSymbolPackage(b);
	default:
		return a == b;
	}
}

local SExpr sxiNew(SExprTag tag, Length size)
{
	SExpr s;

	s = (SExpr) stoAlloc(OB_SExpr, size);

	s->sxHdr.tag = tag;
	s->sxHdr.isShared = false;
	s->sxHdr.pos = sposNone;
	s->sxHdr.wrWidth = 0;

	return s;
}

void sxiFree(SExpr s)
{
	Length i;
	SExpr s0;

	if (sxiIsShared(s))
		return;

	switch (sxiTag(s)) {
	case SX_String:
		strFree(s->sxString.val);
		stoFree((Pointer) s);
		break;
	case SX_Integer:
		bintFree(s->sxInteger.val);
		stoFree((Pointer) s);
		break;
	case SX_Ratio:
		sxiFree(s->sxRatio.num);
		sxiFree(s->sxRatio.den);
		stoFree((Pointer) s);
		break;
	case SX_Complex:
		sxiFree(s->sxComplex.real);
		sxiFree(s->sxComplex.imag);
		stoFree((Pointer) s);
		break;
	case SX_Vector:
		for (i = 0; i < sx0Size(s); i++)
			sxiFree(sx0Elt(s,i));
		stoFree((Pointer) s);
		break;
	case SX_Cons:
		while (sxiConsP(s)) {
			s0 = s;
			s = sxCdr(s);
			sxiFree(sxCar(s0));
			stoFree((Pointer) s0);
		}
		if (!sxiNull(s))
			sxiFree(s);
		break;
	default:
		break;
	}
}

void sxiFreeList(SExpr s)
{
	SExpr s0;
	while (sxiConsP(s)) {
		s0 = s;
		s = sxCdr(s);
		stoFree((Pointer) s0);
	}
}

SExpr sxiRepos(SrcPos pos, SExpr sx)
{
	if (sx->sxHdr.isShared) {
		switch (sxiTag(sx)) {
		case SX_Nil:
			sx = sxiNewNil();
			break;
		case SX_Integer:
			sx = sxiFrBigInteger(sxiToTheBigInteger(sx));
			break;
		case SX_Symbol:
			sx = sx0MakeSymbol(sx->sxSymbol.sym,
					sxSymbolPackage(sx));
			break;
		default:
			bug("Inappropriate shared structure for reposition");
		}
	}
	sxiPos(sx) = pos;
	return sx;
}

void sxiReadPrintLoop(FILE *inf, FILE *outf, ULong iomode)
{
	int lineno = 1;
	SExpr sx, sxeof;
	SExprErrorFun errfun = sxiSetHandler((SExprErrorFun) 0);

	sxiSTAR_StandardInput_STAR = inf;
	sxiSTAR_StandardOutput_STAR = outf;

	sxeof = sxMakeSymbol(sxiFrString("End of File")); /* unique */

	for (;;) {
		prompt(inf, outf, "> ");
		sx = sxiRead(inf, NULL, &lineno, sxeof, iomode);
		if (sx == sxeof)
			break;
		sxiWrite(outf, sx, iomode);
	}

	sxiSetHandler(errfun);
}

void sxiReadEvalPrintLoop(FILE *inf, FILE *outf, ULong iomode)
{
	int lineno = 1;
	SExpr sx, sxeof;
	SExprErrorFun errfun = sxiSetHandler((SExprErrorFun) 0);

	sxiSTAR_StandardInput_STAR = inf;
	sxiSTAR_StandardOutput_STAR = outf;

	sxeof = sxMakeSymbol(sxiFrString("End of File")); /* unique */

	for (;;) {
		prompt(inf, outf, "> ");
		sx = sxiRead(inf, NULL, &lineno, sxeof, iomode);
		if (sx == sxeof)
			break;
		sx = sxEval(sx);
		sxiWrite(outf, sx, iomode);
	}

	sxiSetHandler(errfun);
}

/******************************************************************************
 *
 * :: sxiValueTable
 * :: sxiFunctionTable
 *
 *****************************************************************************/

Bool sxiTableIsInitialized = false;
Table sxiValueTable;
Table sxiFunctionTable;

/*
 * We box functions since we cannot legally cast function to void *.
 */
typedef struct funbox {
	SExprFun fun;
}*SExprFunBox;

local SExprFunBox sxiNewFunBox(SExprFun fun)
{
	SExprFunBox fbox = (SExprFunBox) stoAlloc(OB_Other, sizeof(*fbox));
	fbox->fun = fun;
	return fbox;
}

#define	sxSet(sy,rx)					\
	((SExpr) tblSetElt(sxiValueTable,		\
			   (TblKey) sxiToSymbol(sy),	\
			   (TblElt) rx))

#define	sxiSetFunction(sy, fn)				\
	((void)  tblSetElt(sxiFunctionTable,		\
			  (TblKey) sxiToSymbol(sy),	\
			  (TblElt) sxiNewFunBox(fn)))

/* 12.2 */
SExpr sx0ZeroP, sx0PlusP, sx0MinusP, sx0OddP, sx0EvenP;

/* 12.3 */
SExpr sx0EQ /* =  */, sx0LT /* < */, sx0LE /* <= */, sx0NE /* /= */,
		sx0GT /* > */, sx0GE /* >= */;
SExpr sx0Min, sx0Max;

/* 12.4 -- omitted: 1+, 1-, incf, decf, conjugate */
SExpr sx0PLUS /* + */, sx0MINUS /* - */, sx0TIMES /* * */, sx0DIVIDE /* / */;
SExpr sx0Gcd, sx0Lcm;

/* 22.1 */
SExpr sx0Undef, sx0Apply, sx0Bye, sx0Eval, sx0Set, sx0Setq;
local SExpr sxFunUndef(SExpr);
local SExpr sxFunApply(SExpr);
local SExpr sxFunBye(SExpr);
local SExpr sxFunEval(SExpr);
local SExpr sxFunSet(SExpr);
local SExpr sxFunPLUS(SExpr);
local SExpr sxFunMINUS(SExpr);
local SExpr sxFunTIMES(SExpr);
local SExpr sxFunZeroP(SExpr);
local SExpr sxFunPlusP(SExpr);
local SExpr sxFunMinusP(SExpr);

void sxiTableInit(void)
{
	sx0ZeroP = sxIntern(sxiFrString("ZEROP"));
	sx0PlusP = sxIntern(sxiFrString("PLUSP"));
	sx0MinusP = sxIntern(sxiFrString("MINUSP"));
	sx0OddP = sxIntern(sxiFrString("ODDP"));
	sx0EvenP = sxIntern(sxiFrString("EVENP"));

	sx0EQ = sxIntern(sxiFrString("="));
	sx0LT = sxIntern(sxiFrString("<"));
	sx0LE = sxIntern(sxiFrString("<="));
	sx0NE = sxIntern(sxiFrString("/="));
	sx0GT = sxIntern(sxiFrString(">"));
	sx0GE = sxIntern(sxiFrString(">="));
	sx0Min = sxIntern(sxiFrString("MIN"));
	sx0Max = sxIntern(sxiFrString("MAX"));

	sx0PLUS = sxIntern(sxiFrString("+"));
	sx0MINUS = sxIntern(sxiFrString("-"));
	sx0TIMES = sxIntern(sxiFrString("*"));
	sx0DIVIDE = sxIntern(sxiFrString("/"));
	sx0Gcd = sxIntern(sxiFrString("GCD"));
	sx0Lcm = sxIntern(sxiFrString("LCM"));

	sx0Undef = sxIntern(sxiFrString("*UNDEFINED*"));
	sx0Apply = sxIntern(sxiFrString("APPLY"));
	sx0Bye = sxIntern(sxiFrString("BYE"));
	sx0Eval = sxIntern(sxiFrString("EVAL"));
	sx0Set = sxIntern(sxiFrString("SET"));
	sx0Setq = sxIntern(sxiFrString("SETQ"));

	sxiValueTable = tblNew((TblHashFun) 0, (TblEqFun) 0); /* == hash */
	sxiFunctionTable = tblNew((TblHashFun) 0, (TblEqFun) 0); /* == hash */

	sxiSetFunction(sx0Undef, sxFunUndef);
	sxiSetFunction(sx0Apply, sxFunApply);
	sxiSetFunction(sx0Bye, sxFunBye);
	sxiSetFunction(sx0Eval, sxFunEval);
	sxiSetFunction(sx0Set, sxFunSet);
	sxiSetFunction(sx0PLUS, sxFunPLUS);
	sxiSetFunction(sx0MINUS, sxFunMINUS);
	sxiSetFunction(sx0TIMES, sxFunTIMES);
	sxiSetFunction(sx0ZeroP, sxFunZeroP);
	sxiSetFunction(sx0PlusP, sxFunPlusP);
	sxiSetFunction(sx0MinusP, sxFunMinusP);

	sxiTableIsInitialized = true;
}

SExpr sxSymbolValue(SExpr sy)
{
	SExpr rx = sx0Undef;

	if (!sxiTableIsInitialized)
		sxiTableInit();

	if (sxiSymbolP(sy))
		rx = (SExpr) tblElt(sxiValueTable, (TblKey) sxiToSymbol(sy),
				(TblElt) sx0Undef);

	return rx;
}

SExprFun sxiSymbolFunction(SExpr sy)
{
	SExprFunBox fn = 0;

	if (!sxiTableIsInitialized)
		sxiTableInit();

	if (sxiSymbolP(sy))
		fn = (SExprFunBox) tblElt(sxiFunctionTable,
				(TblKey) sxiToSymbol(sy), (TblElt) 0);

	return fn ? fn->fun : sxFunUndef;
}
/******************************************************************************
 *
 * :: SExprFun implementations.
 *
 *****************************************************************************/
local Bool sxiIsQuoteForm(SExpr);
local Bool sxiIsSetqForm(SExpr);
local SExpr sxiEvalArgs(SExpr);
local SExpr sxiReduceBigint(BInt(*)(BInt, BInt), BInt, SExpr);
local SExpr sxiTest1Bigint(Bool(*)(BInt), SExpr);

/*
 * sxUndef
 */

local SExpr sxFunUndef(SExpr form)
{
	return sx0Undef;
}

/*
 * sxApply
 */

local SExpr sxFunApply(SExpr form)
{
	SExpr args = sxRest(form);
	return sxApply(sxFirst(args), sxSecond(args));
}

SExpr sxApply(SExpr sy, SExpr args)
{
	return (*(sxiSymbolFunction(sy)))(sxCons(sy, args));
}

/*
 * sxBye
 */

local SExpr sxFunBye(SExpr form)
{
	return sxBye();
}

SExpr sxBye(void)
{
	exitSuccess();
NotReached(return sx0Undef);
}

/*
 * sxEval
 */

local SExpr sxFunEval(SExpr form)
{
	SExpr args = sxRest(form);
	return sxEval(sxFirst(args));
}

SExpr sxEval(SExpr sx)
{
	SExpr rx;

	if (!sxiTableIsInitialized)
		sxiTableInit();

	if (sxiNull(sx) || sxiPackageP(sx) || sxiIntegerP(sx)
			|| sxiRatioP(sx) || sxiFloatP(sx) || sxiComplexP(sx)
			|| sxiCharP(sx) || sxiStringP(sx) || sxiVectorP(sx))
		rx = sx;

	else if (sxiSymbolP(sx))
		rx = sxSymbolValue(sx);

	else if (sxiIsQuoteForm(sx))
		rx = sxSecond(sx);

	else if (sxiIsSetqForm(sx))
		rx = sxSet(sxSecond(sx), sxEval(sxThird(sx)));

	else if (sxiConsP(sx)) {
		SExpr args = sxiEvalArgs(sxCdr(sx));
		rx = sxApply(sxCar(sx), args);
		sxiFreeList(args);
	}

	else
		rx = sx0Undef;

	return rx;
}

local SExpr sxiEvalArgs(SExpr args)
{
	if (sxiNull(args))
		return sxNil;
	else
		return sxCons(sxEval(sxCar(args)), sxiEvalArgs(sxCdr(args)));
}

local Bool sxiIsQuoteForm(SExpr sx)
{
	return sxConsP(sx) && sxSymbolP(sxCar(sx)) && sxiToSymbol(sxCar(sx))
			== sxiToSymbol(sx0Quote) && sxConsP(sxCdr(sx))
			&& sxNull(sxCddr(sx));
}

local Bool sxiIsSetqForm(SExpr sx)
{
	return sxConsP(sx) && sxSymbolP(sxCar(sx)) && sxiToSymbol(sxCar(sx))
			== sxiToSymbol(sx0Setq) && sxConsP(sxCdr(sx))
			&& sxSymbolP(sxCadr(sx))
			&& sxConsP(sxCddr(sx))
			&& sxNull(sxCdr(sxCddr(sx)));
}

/*
 * sxSet
 */

local SExpr sxFunSet(SExpr form)
{
	SExpr args = sxRest(form);
	return sxSet(sxFirst(args), sxSecond(args));
}

/*
 * sxPLUS, sxMINUS, sxTIMES
 */

local SExpr sxFunPLUS(SExpr form)
{
	return sxPLUS(sxRest(form));
}

SExpr sxPLUS(SExpr args)
{
	return sxiReduceBigint(bintPlus, bint0, args);
}

local SExpr sxFunMINUS(SExpr form)
{
	return sxMINUS(sxRest(form));
}

SExpr sxMINUS(SExpr args)
{
	return sxiReduceBigint(bintMinus, bint0, args);
}

local SExpr sxFunTIMES(SExpr form)
{
	return sxTIMES(sxRest(form));
}

SExpr sxTIMES(SExpr args)
{
	return sxiReduceBigint(bintTimes, bint1, args);
}

/*
 * sxZeroP, sxPlusP, sxMinusP
 */
local SExpr sxFunZeroP(SExpr form)
{
	return sxZeroP(sxRest(form));
}

SExpr sxZeroP(SExpr form)
{
	return sxiTest1Bigint(bintIsZero, sxRest(form));
}

local SExpr sxFunPlusP(SExpr form)
{
	return sxPlusP(sxRest(form));
}

SExpr sxPlusP(SExpr form)
{
	return sxiTest1Bigint(bintIsPos, sxRest(form));
}

local SExpr sxFunMinusP(SExpr form)
{
	return sxMinusP(sxRest(form));
}

SExpr sxMinusP(SExpr form)
{
	return sxiTest1Bigint(bintIsNeg, sxRest(form));
}

/*
 * The following rules let this function be used for - as well as +, * ....
 * []	   ==> x0
 * [a]	   ==> x0 op a
 * [a,b]   ==> a op b
 * [a,b,c] ==> a op b op c
 */

local SExpr sxiReduceBigint(BInt(*op)(BInt, BInt), BInt x0, SExpr args)
{
	SExpr sx;
	BInt x1;

	/* 0 args, return x0 */
	if (!sxiConsP(args))
		return sxiFrBigInteger(x0);

	sx = sxEval(sxCar(args));
	if (!sxiIntegerP(sx))
		return sx0Undef;
	x1 = sxiToBigInteger(sx);
	args = sxCdr(args);

	/* 1 arg, return x0 op x1 */
	if (!sxConsP(args))
		return sxiFrBigInteger((*op)(x0, x1));

	/* 2 or more args */
	x0 = x1;
	while (sxiConsP(args)) {
		sx = sxEval(sxCar(args));
		if (!sxiIntegerP(sx))
			return sx0Undef;

		x0 = (*op)(x0, sxiToBigInteger(sx));
		args = sxCdr(args);
	}
	if (!sxiNull(args))
		return sx0Undef;
	else
		return sxiFrBigInteger(x0);
}

local SExpr sxiTest1Bigint(Bool(*op)(BInt), SExpr args)
{
	SExpr sx;
	BInt x0;
	Bool b;

	if (!sxiConsP(args))
		return sx0Undef;

	sx = sxEval(sxCar(args));
	if (!sxiIntegerP(sx))
		return sx0Undef;
	x0 = sxiToBigInteger(sx);

	b = (*op)(x0);
	return b ? sxT : sxNil;
}

/*****************************************************************************
 *
 * :: Error handling
 *
 ****************************************************************************/

local void sxiRdError(int errnum, ...)
{
	va_list argp;
	SrcPos spos = sposOffset(sxiRdPos, sxiRdCharNo);

	va_start(argp, errnum);
	sxiRdErrVal = (*sxiError)(spos, errnum, argp);
	va_end(argp);

	longjmp(sxiRdCatch, errnum);
}

local SExpr sxiUseError(int errnum, ...)
{
	SExpr sx;
	va_list argp;

	va_start(argp, errnum);
	sx = (*sxiError)(sposNone, errnum, argp);
	va_end(argp);

	return sx;
}

local SExpr sxiDefaultHandler(SrcPos spos, int errnum, va_list argp)
{
	char *s;
	switch (errnum) {
	case SX_ErrPackageExists:
		s = "A package with the name %s already exists.";
		break;
	case SX_ErrInternNeeds:
		s = "Intern requires a string.";
		break;
	case SX_ErrNumDenNeeds:
		s = "%s requires an integer or ratio.";
		break;
	case SX_ErrNReverseNeeds:
		s = "NReverse requires the last cdr of a list to be nil.";
		break;
	case SX_ErrBadArgumentTo:
		s = "Inappropriate argument to function `%s'.";
		break;
	case SX_ErrReadEOF:
		s = "End of file during read.";
		break;
	case SX_ErrBadPunct:
		s = "Misplaced '%s'.";
		break;
	case SX_ErrBadChar:
		s = "Illegal character 0x%x.";
		break;
	case SX_ErrMacroIlleg:
		s = "Illegal macro character '#%c'.";
		break;
	case SX_ErrMacroUndef:
		s = "Undefined macro character '#%c'.";
		break;
	case SX_ErrMacroUnimp:
		s = "Unimplemented macro character '#%c'.";
		break;
	case SX_ErrCantMacroArg:
		s = "Macro #%c does not take a numeric argument.";
		break;
	case SX_ErrMustMacroArg:
		s = "Macro #n%c requires a numeric argument.";
		break;
	case SX_ErrBadFeatureForm:
		s = "Improper feature form following #+ or #-.";
		break;
	case SX_ErrTooManyElts:
		s = "Number of elements greater than given size.";
		break;
	case SX_ErrCantShare:
		s = "Share label #nn= not previously defined.";
		break;
	case SX_ErrAlreadyShare:
		s = "Share label #nn= already defined.";
		break;
	case SX_ErrBadCharName:
		s = "Improper character name after #\\.";
		break;
	case SX_ErrBadComplexNum:
		s = "Improper complex number #C....";
		break;
	case SX_ErrBadToken:
		s = "Missing escape in token.";
		break;
	case SX_ErrBadPotNum:
		s = "Meaningless potential number '%s'.";
		break;
	case SX_ErrBadUninterned:
		s = "Package given with '#:'";
		break;
	default:
		bugBadCase(errnum);
		NotReached(s = 0);
	}

	if (spos != sposNone) {
		sposPrint(stderr, spos);
		fprintf(stderr, ": ");
	}
	vfprintf(stderr, s, argp);
	fprintf(stderr, "\n");

	return sxNil;
}

SExprErrorFun sxiSetHandler(SExprErrorFun newFun)
{
	SExprErrorFun oldFun = sxiError;
	sxiError = newFun ? newFun : (SExprErrorFun) sxiDefaultHandler;
	return oldFun;
}

String
sxiFormat(SExpr sx)
{

	Buffer buf = bufNew();
	sxiToBuffer(buf, sx);
	bufAdd1(buf, '\0');
	return bufLiberate(buf);
}

void 
sxiToBufferFormatted(Buffer buf, SExpr s, ULong iomode)
{
	Scope("sxiToBufferFormatted");
	int fluid(sxiIoMixedCase), fluid(sxiIoKeepSrcPos);
	int fluid(sxiIoOutPackages);

	if (iomode & SXRW_MixedCase)
		sxiIoMixedCase = 1;
	if (iomode & SXRW_FoldCase)
		sxiIoMixedCase = 0;
	if (iomode & SXRW_SrcPos)
		sxiIoKeepSrcPos = 1;
	if (iomode & SXRW_NoSrcPos)
		sxiIoKeepSrcPos = 0;
	if (iomode & SXRW_Packages)
		sxiIoOutPackages = 1;

	sxiToBuffer(buf, s);

	Return(Nothing);
}

void 
sxiToBuffer(Buffer buf, SExpr s)
{
	sxiWrWidth(s);

	switch (sxiTag(s)) {
	case SX_Nil:
		bufPrintf(buf, "()");
		return;
	case SX_Symbol:
	case SX_Integer:
	case SX_Float:
	case SX_Char:
		sxiWrUnscanToken(s);
		bufPrintf(buf, "%s", sxiIoBufChars());
		return;
	case SX_String:
		sxiWrUnscanToken(s);
		bufPrintf(buf, "%s", sxiIoBufChars());
		break;
	case SX_Ratio:
		sxiToBuffer(buf, sxNumerator(s));
		bufPrintf(buf, "/");
		sxiToBuffer(buf, sxDenominator(s));
		break;
	case SX_Complex:
		bufPrintf(buf, "#C(");
		sxiToBuffer(buf, sxRealpart(s));
		bufPrintf(buf, " ");
		sxiToBuffer(buf, sxImagpart(s));
		bufPrintf(buf, ")");
		break;
	case SX_Vector: {
		int newl = s->sxHdr.wrWidth > sxiWrMaxText;
		int i;

		sxiWrLead += sxiWrIndent;
		bufPrintf(buf, "#(");

		sxiCurrentPos = sxiPos(s);

		for (i = 0; i < sx0Size(s); i++) {
			if (i > 0) {
				if (newl)
					bufPrintf(buf, "\n%*s", sxiWrLead,
							"");
				else
					bufPrintf(buf, " ");
			}
			sxiToBuffer(buf, sx0Elt(s, i));
		}
		bufPrintf(buf, ")");
		sxiWrLead -= sxiWrIndent;
		break;
	}
	case SX_Cons: {
		SExpr kar = sxCar(s);
		SExpr kdr = sxCdr(s);
		char *str = 0;
		int newl = s->sxHdr.wrWidth > sxiWrMaxText;

		sxiWrLead += sxiWrIndent;

		if (sxiConsP(kdr) && sxiNull(sxCdr(kdr))) {
			if (kar == sx0Quote)
				str = "'";
			else if (kar == sx0Function)
				str = "#'";
			else if (kar == sx0BackQuote)
				str = "`";
			else if (kar == sx0BackQuoteComma)
				str = ",";
			else if (kar == sx0BackQuoteCommaAt)
				str = ",@";
			else if (kar == sx0BackQuoteCommaDot)
				str = ",.";
			else if (kar == sx0LoadTimeEval)
				str = "#,";
			else if (kar == sx0ReadTimeEval)
				str = "#.";
		}
		if (str) {
			bufPrintf(buf, "%s", str);
			sxiToBuffer(buf, sxCar(kdr));
		} else {
			sxiCurrentPos = sxiPos(s);
			bufPrintf(buf, "(");
			sxiToBuffer(buf, sxCar(s));
			s = sxCdr(s);
			while (sxiConsP(s)) {
				if (newl) {
					bufPrintf(buf, "\n%*s",sxiWrLead, " ");
				} else
					bufPrintf(buf, " ");
				sxiToBuffer(buf, sxCar(s));
				s = sxCdr(s);
			}
			if (!sxiNull(s)) {
				if (newl)
					bufPrintf(buf, " .\n%*s",
							sxiWrLead, "");
				else
					bufPrintf(buf, " . ");
				sxiToBuffer(buf, s);
			}
			bufPrintf(buf, ")");
		}
		sxiWrLead -= sxiWrIndent;
		break;
	}
	default:
		bufPrintf(buf, "#<Unknown>");
	}

}
