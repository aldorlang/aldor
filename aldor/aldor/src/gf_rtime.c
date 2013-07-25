/*****************************************************************************
 *
 * gf_rtime.c: Foam generation for runtime objects
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "gf_prog.h"
#include "gf_rtime.h"
#include "gf_util.h"
#include "of_inlin.h"
#include "store.h"
#include "strops.h"
/* ToDo:
 * Allow 'runtime' calls that are not in runtime.as
 */

/*
 * Runtime call information
 */

struct runtimeCallInfo runtimeCallInfoTable[] = {
	{ "domainMake",			false,	1, 0 },
	{ "domainAddParents!", 		true,	2, 0 },
	{ "domainAddNameFn!", 		true,	2, 0 },
	{ "domainAddHash!", 		true,	2, 0 },
	{ "domainAddExports!", 		true,	4, 0 },
	{ "domainAddDefaults!",		true,	2, 0 },
	{ "domainMakeDispatch", 	false,	1, 0 },
	{ "domainHash!",		false,	1, RT_DOM_RTHASH },
	{ "domainGetExport!",		false,	3, 0 },
	{ "domainTestExport!",  	false,	3, 0 },
	{ "domainName",			false, 	1, 0 },
	{ "extendMake", 		false,	1, 0 },
	{ "domainMakeDummy",		true,   0, 0},
	{ "domainFill!",		false,  0, 0},
	{ "extendFill!", 		true,	2, 0 },
	{ "categoryMake",		false,	2, 0 },
	{ "categoryAddParents", 	true,	2, 0 },
	{ "categoryAddHash!",		true,	2, 0 },
	{ "categoryAddNameFn!",		true,	2, 0 },
	{ "categoryAddExports!",	true,	4, 0 },
	{ "categoryAddParents!",	true,	2, 0 },
	{ "combineHash",		false,	1, 0 },
	{ "stringHash",			false,	1, 0 },
	{ "categoryMakeDummy",		false,  0, 0},
	{ "categoryFill!",		true,   0, 0},
	{ "categoryName",		false,  1, 0 },
	{ "rtDelayedGetExport!",	false,  3, 0 },
	{ "rtDelayedInit!", 		false,  2, 0 },
	{ "rtLazyDomFrInit", 		false,  2, 0 },
	{ "rtLazyCatFrInit", 		false,  2, 0 },
	{ "namePartConcat", 		false,  2, 0 },
	{ "namePartFrString", 		false,  1, 0 },
	{ "namePartFrOther", 		false,  1, 0 },
	{ "noOperation", 		false,  0, 0 },
	{ "rtConstSIntFn",		false,	1, 0 },
	{ "rtAddStrings", 		true,  	1, 0 },
	{ "lazyGetExport!",		false,  3, 0 },
	{ "lazyForceImport", 		false,  1, RT_FORCER },
	{ "rtCacheMake",		true,	0, 0},
	{ "rtCacheExplicitMake",	true,	1, 0},
	{ "rtCacheCheck",		true,	2, 0},
	{ "rtCacheAdd", 		true,	3, 0},
	{ "rtConstNameFn",		false,  1, 0},
	{ "rtSingleParamNameFn",	false,  2, 0},
	{ "rtDoDebug",			true,   6, 0},
	{ "rtDebugEnter",		false,  6, 0},
	{ "rtDebugInit",		false,  6, 0},
	{ "rtDebugInside",		false,  1, 0},
	{ "rtDebugReturn",		false,  5, 0},
	{ "rtDebugExit",		false,  3, 0},
	{ "rtDebugAssign",		false,  8, 0},
	{ "rtDebugThrow",		false,  4, 0},
	{ "rtDebugCatch",		false,  4, 0},
	{ "rtDebugCall",		false,  7, 0},
	{ "rtDebugStep",		false,  3, 0},
	{ "rtDebugPaused!",		true,	1, 0},
	{ "rtDebugPaused?",		false,	0, 0},
	{ "rtAssertMessage",		true,   3, 0},
	{ "rtCacheDynamicMake",		true,	0, 0},
	{ "rtCacheDynamicCheck",	true,	2, 0},
	{ "rtCacheDynamicAdd", 		true,	3, 0},
};

#define	NRuntimeCalls \
	(sizeof(runtimeCallInfoTable)/sizeof(runtimeCallInfoTable[0]))


struct runtimeCallInfo runtimeStdLazyGetterPrefix = { 
 "", false, 1, 0
};


RuntimeCallInfo
gen0GetRuntimeCallInfo(Foam decl)
{	/* !!should cache values */
	int i;
	String name;
	assert(foamTag(decl) == FOAM_GDecl);
	
	name = decl->foamGDecl.id;
	if (decl->foamGDecl.protocol != FOAM_Proto_Foam)
		return NULL;

	for (i = 0; i< NRuntimeCalls; i++) {
		RuntimeCallInfo info = &runtimeCallInfoTable[i];
		if (info->name[0] == name[0] && strEqual(info->name, name))
			return info;
	}
	
	if (strIsPrefix(gen0StdLazyNamePrefix(), name))
		return &runtimeStdLazyGetterPrefix;

	return NULL;
}

/*
 * Runtime cache-size tweaking.
 */


/* See runtime.as for the default cache size */
static AInt gen0RtCacheSize = 15;


/*
 * This ought to only ever be called as a result of the
 * user specifying the -Wrtcache=NNN flag. Use size
 * 0 to get the default cache and a negative value to
 * get a dynamically sized cache.
 */
void
gen0SetRtCacheSize(AInt size)
{
	gen0RtCacheSize = size;
}


AInt
gen0GetRtCacheSize(void)
{
	return gen0RtCacheSize;
}


/*
 * Tuples
 */

#define 	gen0TupleSize 2
static String   gen0TupleNames[] = { "size", "values" };
static FoamTag  gen0TupleTypes[] = { FOAM_SInt, FOAM_Arr };
static AInt	gen0TupleFmts[]  = { emptyFormatSlot, FOAM_Word };

AInt		gen0TupleFormat;

Foam
gen0TupleElt(Foam tuple, int index)
{
	return foamNewAElt(FOAM_Word,
			   foamNewSInt(index),
			   gen0NewTupleValsRef(tuple));
}

AInt
gen0MakeTupleFormat()
{
	if (gen0TupleFormat)
		return gen0TupleFormat;

	gen0TupleFormat = gen0StdDeclFormat(gen0TupleSize, gen0TupleNames, 
					    gen0TupleTypes, gen0TupleFmts);
	return gen0TupleFormat;
}


void
gen0MakeEmptyTuple(Foam argc, Foam *vals, AbSyn loc)
{
	int	format;
	Foam	t1, t2;

	/*!! This should be a callout to tuple$Tuple(S) */
	t1 = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	gen0AddStmt(foamNewSet(foamCopy(t1), 
			       foamNewANew(FOAM_Word, argc)), loc);

	format = gen0MakeTupleFormat();
	t2 = gen0TempLocal0(FOAM_Rec, format);
	gen0AddStmt(gen0RNew(t2, format), loc);
	gen0AddStmt(gen0RSet(t2, format, (AInt) 0, 
			     foamCopy(argc)), loc);
	gen0AddStmt(gen0RSet(t2, format, (AInt) 1, t1), loc);

	vals[0] = foamNewCast(FOAM_Word, t2);
	vals[1] = t1;
}

Foam 
gen0MakeTupleFromFoam(int argc, Foam *argv)
{
	Foam t1, t2;
	int  i,  format;
	t1 = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	gen0AddStmt(gen0ANew(t1, FOAM_Word, argc), NULL);
	
	for (i=0; i < argc; i++) {
		gen0AddStmt(gen0ASet(t1, (AInt) i, FOAM_Word, argv[i]), NULL);
	}

	/*!! This should be a callout to tuple$Tuple(S) */
	format = gen0MakeTupleFormat();
	t2 = gen0TempLocal0(FOAM_Rec, format);
	gen0AddStmt(gen0RNew(t2, format), NULL);
	gen0AddStmt(gen0RSet(t2, format, (AInt) 0, foamNewSInt(argc)), NULL);
	gen0AddStmt(gen0RSet(t2, format, (AInt) 1, foamCopy(t1)), NULL);
	return t2;
}

/* 
 * Unions
 */

#define 	gen0UnionSize 2
static String   gen0UnionNames[] = { "tag", "value" };
static FoamTag	gen0UnionTypes[] = { FOAM_SInt, FOAM_Word };
static AInt	gen0UnionFmts[]  = { emptyFormatSlot, emptyFormatSlot };

int	gen0UnionFormat;

int
gen0MakeUnionFormat()
{
	if (gen0UnionFormat)
		return gen0UnionFormat;

	gen0UnionFormat = gen0StdDeclFormat(gen0UnionSize, gen0UnionNames,
					    gen0UnionTypes, gen0UnionFmts);
	return gen0UnionFormat;
}

/* 
 * Arrays
 */

int	gen0ArrayFormat;

#define 	gen0ArraySize 3
static String   gen0ArrayNames[] = { "size", "nsize", "values"};
static FoamTag  gen0ArrayTypes[] = { FOAM_SInt, FOAM_SInt, FOAM_Word };
static AInt 	gen0ArrayFmts[]  = { emptyFormatSlot, emptyFormatSlot, 
				     emptyFormatSlot };

int
gen0MakeArrayFormat()
{
        if (gen0ArrayFormat)
                return gen0ArrayFormat;

	gen0ArrayFormat = gen0StdDeclFormat(gen0ArraySize, gen0ArrayNames, 
					    gen0ArrayTypes, gen0ArrayFmts);
        return gen0ArrayFormat;
}

Foam
gen0MakeArray(Foam size, Foam data, AbSyn absyn)
{
        /*!! Should be a callback */
        int     format = gen0MakeArrayFormat();
        Foam    t = gen0TempLocal0(FOAM_Rec, format);

        gen0AddStmt(gen0RNew(t, format), absyn);
        gen0AddStmt(gen0RSet(t, format, (AInt) 0, foamCopy(size)), absyn);
        gen0AddStmt(gen0RSet(t, format, (AInt) 1, foamCopy(size)), absyn);
        gen0AddStmt(gen0RSet(t, format, (AInt) 2, 
			     foamNewCast(FOAM_Word, foamCopy(data))),
		    absyn);
	
        return t;
}

/*
 * DoubleFloats.  This is some gunk to convert rec(dflo) to dflo.
 * Once `raw' types work, this won't be necessary. Then we can move
 * it into the foreign import/export interface.
 */

#define 	gen0DoubleSize 1
static String   gen0DoubleNames[] = { "double" };
static FoamTag  gen0DoubleTypes[] = { FOAM_DFlo };
static AInt	gen0DoubleFmts[]  = { emptyFormatSlot };

AInt		gen0DoubleFormat;

Foam
gen0DoubleValue(Foam rec)
{
	return foamNewRElt(gen0MakeDoubleFormat(), rec, (AInt) 0);
}

AInt
gen0MakeDoubleFormat()
{
	if (gen0DoubleFormat)
		return gen0DoubleFormat;

	gen0DoubleFormat = gen0StdDeclFormat(gen0DoubleSize, gen0DoubleNames, 
					     gen0DoubleTypes, gen0DoubleFmts);
	return gen0DoubleFormat;
}


Foam 
gen0MakeDouble(Foam foam)
{
	Foam t1;
	int  format;

	format = gen0MakeDoubleFormat();
	t1 = gen0TempLocal0(FOAM_Rec, format);

	gen0AddStmt(gen0RNew(t1, format), NULL);
	gen0AddStmt(gen0RSet(t1, format, (AInt) 0, foam), NULL);

	return foamNewCast(FOAM_Word, t1);
}

Foam
gen0MakeDoubleCode(Foam foam, FoamList *plst)
{
	FoamList lst;
	Foam t1;
	int  format;

	format = gen0MakeDoubleFormat();
	t1 = gen0TempLocal0(FOAM_Rec, format);

	lst = listNil(Foam);
	lst = listCons(Foam)(gen0RNew(t1, format), lst);
	lst = listCons(Foam)(gen0RSet(t1, format, (AInt) 0, foam), lst);

	*plst = lst;
	return foamNewCast(FOAM_Word, t1);
}

Foam
gen0WriteDoubleValue(Foam dst, Foam src)
{
	return gen0RSet(dst, gen0MakeDoubleFormat(), (AInt)0, src);
}

/*
 * Characters - used for reading/writing pointers to Char values
 * to help with the foreign Fortran interface.
 */

#define         gen0CharSize 1
static String   gen0CharNames[] = { "char" };
static FoamTag  gen0CharTypes[] = { FOAM_Char };
static AInt     gen0CharFmts[]  = { emptyFormatSlot };

AInt gen0CharFormat;

AInt
gen0MakeCharFormat()
{
	if (gen0CharFormat)
		return gen0CharFormat;

	gen0CharFormat = gen0StdDeclFormat(
				gen0CharSize, gen0CharNames,
				gen0CharTypes, gen0CharFmts);
	return gen0CharFormat;
}

Foam
gen0WriteCharRecValue(Foam dst, Foam src)
{
	return gen0RSet(dst, gen0MakeCharFormat(), (AInt)0, src);
}

Foam
gen0ReadCharRecValue(Foam foam)
{
	Foam tmpfoam = foamCopy(foam);
	return foamNewRElt(gen0MakeCharFormat(), tmpfoam, (AInt)0);
}

Foam
gen0MakeCharRecValue(Foam foam, FoamList *plst)
{
	FoamList lst;
	Foam t1, t2;
	int  format;

	format = gen0MakeCharFormat();
	t1 = gen0TempLocal0(FOAM_Rec, format);
	t2 = foamCopy(foam);

	lst = listNil(Foam);
	lst = listCons(Foam)(gen0RNew(t1, format), lst);
	lst = listCons(Foam)(gen0RSet(t1, format, (AInt) 0, t2), lst);

	*plst = lst;
	return foamNewCast(FOAM_Word, t1);
}

/*
 * Integers - used for reading/writing pointers to SInt values
 * to help with the foreign Fortran interface
 */

#define         gen0IntSize 1
static String   gen0IntNames[] = { "int" };
static FoamTag  gen0IntTypes[] = { FOAM_SInt };
static AInt     gen0IntFmts[]  = { emptyFormatSlot };

AInt gen0IntFormat;

AInt
gen0MakeIntFormat()
{
	if (gen0IntFormat)
		return gen0IntFormat;

	gen0IntFormat = gen0StdDeclFormat(
				gen0IntSize, gen0IntNames,
				gen0IntTypes, gen0IntFmts);
	return gen0IntFormat;
}

Foam
gen0WriteIntRecValue(Foam dst, Foam src)
{
	return gen0RSet(dst, gen0MakeIntFormat(), (AInt)0, src);
}

Foam
gen0ReadIntRecValue(Foam foam)
{
	Foam tmpfoam = foamCopy(foam);
	return foamNewRElt(gen0MakeIntFormat(), tmpfoam, (AInt)0);
}

Foam
gen0MakeIntRecValue(Foam foam, FoamList *plst)
{
	FoamList lst;
	Foam t1, t2;
	int  format;

	format = gen0MakeIntFormat();
	t1 = gen0TempLocal0(FOAM_Rec, format);
	t2 = foamCopy(foam);

	lst = listNil(Foam);
	lst = listCons(Foam)(gen0RNew(t1, format), lst);
	lst = listCons(Foam)(gen0RSet(t1, format, (AInt) 0, t2), lst);

	*plst = lst;
	return foamNewCast(FOAM_Word, t1);
}

/*
 * Floats - used for reading/writing pointers to SFlo values
 * to help with the foreign Fortran interface
 */

#define         gen0FloatSize 1
static String   gen0FloatNames[] = { "float" };
static FoamTag  gen0FloatTypes[] = { FOAM_SFlo };
static AInt     gen0FloatFmts[]  = { emptyFormatSlot };

AInt gen0FloatFormat;

AInt
gen0MakeFloatFormat()
{
	if (!gen0FloatFormat)
	{
		gen0FloatFormat = gen0StdDeclFormat(
				gen0FloatSize, gen0FloatNames,
				gen0FloatTypes, gen0FloatFmts);
	}
	return gen0FloatFormat;
}

Foam
gen0MakeFloatRecValue(Foam foam, FoamList *plst)
{
	FoamList lst;
	Foam t1, t2;
	int  format;

	format = gen0MakeFloatFormat();
	t1 = gen0TempLocal0(FOAM_Rec, format);
	t2 = foamCopy(foam);

	lst = listNil(Foam);
	lst = listCons(Foam)(gen0RNew(t1, format), lst);
	lst = listCons(Foam)(gen0RSet(t1, format, (AInt) 0, t2), lst);

	*plst = lst;
	return foamNewCast(FOAM_Word, t1);
}

Foam
gen0ReadFloatRecValue(Foam foam)
{
	Foam tmpfoam = foamCopy(foam);
	return foamNewRElt(gen0MakeFloatFormat(), tmpfoam, (AInt)0);
}

Foam
gen0WriteFloatRecValue(Foam dst, Foam src)
{
	return gen0RSet(dst, gen0MakeFloatFormat(), (AInt)0, src);
}


/*
 * Destructive copy of any complex number from the foam value `src'
 * represented as a record with format `sfmt', into `dst' represented
 * as a record with format `dfmt'. All the field types of `sfmt' and
 * `dfmt' must be compatible. This means that this function may be used
 * copy from a Fortran COMPLEX REAL into an Aldor Complex SF and vice
 * versa. Similarly for COMPLEX DOUBLE/Complex DF. It must not be used
 * to copy Complex SF to/from Complex DF or vice versa.
 *
 * IMPORTANT: this function adds statements to `*lst' which must be
 *            a valid list on entry.
 */
void
gen0CopyComplex(Foam dst, Foam src, AInt dfmt, AInt sfmt, FoamList *lst)
{
	Foam		tmp;
	FoamList	code = *lst;

	/* Start with the imaginary part */
	tmp  = foamNewRElt(sfmt, foamCopy(src), (AInt)1);
	tmp  = gen0RSet(foamCopy(dst), dfmt, (AInt)1, tmp);
	code = listCons(Foam)(tmp, code);

	/* Finish with the real part */
	tmp  = foamNewRElt(sfmt, foamCopy(src), (AInt)0);
	tmp  = gen0RSet(foamCopy(dst), dfmt, (AInt)0, tmp);
	code = listCons(Foam)(tmp, code);

	/* No need to reverse the code list */
	*lst = code;
}


/*
 * AComplex - generic Aldor complex numbers
 */
#define         gen0ACpxSize 2
static String   gen0ACpxNames[] = { "real", "imag" };
static FoamTag  gen0ACpxTypes[] = { FOAM_Word, FOAM_Word };
static AInt     gen0ACpxFmts[]  = { emptyFormatSlot, emptyFormatSlot };

AInt gen0ACpxFormat;

AInt
gen0AldorCpxFormat()
{
	if (!gen0ACpxFormat) {
		gen0ACpxFormat = gen0StdDeclFormat(
					gen0ACpxSize,  gen0ACpxNames,
					gen0ACpxTypes, gen0ACpxFmts);
	}

	return gen0ACpxFormat;
}


/*
 * FSComplex - single precision Fortran COMPLEX
 */
#define         gen0SCpxSize 2
static String   gen0SCpxNames[] = { "real", "imag" };
static FoamTag  gen0SCpxTypes[] = { FOAM_SFlo, FOAM_SFlo };
static AInt     gen0SCpxFmts[]  = { emptyFormatSlot, emptyFormatSlot };

AInt gen0SCpxFormat;

AInt
gen0SingleCpxFormat()
{
	if (!gen0SCpxFormat) {
		gen0SCpxFormat = gen0StdDeclFormat(
					gen0SCpxSize,  gen0SCpxNames,
					gen0SCpxTypes, gen0SCpxFmts);
	}

	return gen0SCpxFormat;
}


/*
 * FDComplex - double precision Fortran COMPLEX
 */
#define         gen0DCpxSize 2
static String   gen0DCpxNames[] = { "real", "imag" };
static FoamTag  gen0DCpxTypes[] = { FOAM_DFlo, FOAM_DFlo };
static AInt     gen0DCpxFmts[]  = { emptyFormatSlot, emptyFormatSlot };

AInt gen0DCpxFormat;

AInt
gen0DoubleCpxFormat()
{
	if (!gen0DCpxFormat) {
		gen0DCpxFormat = gen0StdDeclFormat(
					gen0DCpxSize,  gen0DCpxNames,
					gen0DCpxTypes, gen0DCpxFmts);
	}

	return gen0DCpxFormat;
}



/*
 * Caches
 */

/*
 * Calling sequence is expected to be:
 *  cache := gen0CacheMakeEmpty()
 *  gen0Add{Stmt,Init}(cache->init, cache->ab)
 *  ...
 *  gen0CheckCache(cache, types)
 *  val := generate(body)
 *  gen0FillCache(cache, types, val)
 * Possibly should pass in an equality function
 */

local Foam	gen0BuiltinCacheMake	(void);
local Foam	gen0BuiltinCacheCheck	(Foam, Foam);
local Foam	gen0BuiltinCacheAdd	(Foam, Foam, Foam);
local int	gen0MakeCCheckFormat	(void);

#define 	gen0CCheckSize 2
static String   gen0CCheckNames[] = { "found", "value"};
static FoamTag  gen0CCheckTypes[] = { FOAM_Word, FOAM_Word };
static AInt 	gen0CCheckFmts[]  = { emptyFormatSlot, emptyFormatSlot };

int gen0CCheckFormat;


Foam
gen0CacheClos(AbSyn ab, int nargs, Foam fnClos)
{
	/* ab is just for position... */
	Foam   *paramv;
	RTCacheInfo cache;
	Foam 	var, clos, fnVar, foam;
	Foam	call;
	FoamTag retType = FOAM_Word;
	AInt    index;
	int     i;

	cache = gen0CacheMakeEmpty(ab);
	fnVar = gen0TempLex(FOAM_Word);
	paramv = (Foam*) stoAlloc(OB_Other, nargs* sizeof(Foam));
	gen0AddStmt(cache->init, cache->ab);
	gen0AddStmt(foamNewSet(fnVar, fnClos), ab);
	gen0AddLexLevels(fnVar, 1);

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(gen0ProgName, NULL);
	
	index = gen0FormatNum;
	gen0ProgPushState(NULL, GF_Lambda);

	gen0State->type = NULL;
	gen0State->program = foam;
	
	gen0PushFormat(index);

	call = foamNewEmpty(FOAM_CCall, nargs+2);
	call->foamCCall.op   = foamCopy(fnVar);
	call->foamCCall.type = FOAM_Word;

	for (i=0; i < nargs; i++) {
		paramv[i] = foamNewPar(i);
		call->foamCCall.argv[i] = foamNewPar(i);
	}
	gen0CacheCheck(cache, nargs, paramv);
	var = gen0CacheReturn(cache, call);
	gen0AddStmt(foamNewReturn(var), ab);

	gen0ProgAddFormat(index);
	gen0ProgFiniEmpty(foam, retType, int0);

        foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	gen0ProgPopState();
	stoFree(paramv);
	gen0CacheKill(cache);

	return clos;
}


RTCacheInfo
gen0CacheMakeEmpty(AbSyn ab)
{
	RTCacheInfo new;

	new = (RTCacheInfo) stoAlloc(OB_Other, sizeof(*new));

	new->cache = gen0TempLex(FOAM_Word);
	new->ab	   = ab;
	new->init  = foamNewSet(foamCopy(new->cache), 
				foamNewCast(FOAM_Word, foamNewNil()));
	gen0AddLexLevels(new->cache,1);
	return new;
}

/* !!We could simplify if argc == 0 */
void
gen0CacheCheck(RTCacheInfo cache, int argc, Foam *argv)
{
	AbSyn	posn = cache->ab;
	Foam 	vars[2], tupl, elts, elt;
	Foam	flag, test, values, chk;
	Length	i, testLabel;

	gen0MakeEmptyTuple(foamNewSInt(argc), vars, cache->ab);
	tupl = vars[0];
	elts = vars[1];

	cache->tuple 	 = tupl;
	cache->exitLabel = gen0State->labelNo++;
	cache->value 	 = gen0Temp(FOAM_Word);
	flag		 = gen0Temp(FOAM_Word);
	testLabel	 = gen0State->labelNo++;

	/* distance should be held in the cache */
	gen0UseStackedFormat(1);

	test = foamNew(FOAM_BCall, 2, FOAM_BVal_PtrIsNil,
		       foamNewCast(FOAM_Ptr, foamCopy(cache->cache)));
	gen0AddStmt(foamNewIf(foamNotThis(test), testLabel), posn);
	gen0AddStmt(foamNewSet(foamCopy(cache->cache), gen0BuiltinCacheMake()),
		    posn);
	gen0AddStmt(foamNewLabel(testLabel), posn);

	for (i = 0; i < argc ; i += 1) {
		elt = argv[i];
		gen0AddStmt(gen0ASet(elts, (AInt) i, FOAM_Word, elt), posn);
	}

	values = foamNew(FOAM_Values, 2, cache->value, flag);
	chk = gen0BuiltinCacheCheck(foamCopy(cache->cache),
				    foamCopy(cache->tuple));
	gen0AddStmt(foamNewSet(values, chk), posn);
	gen0AddStmt(foamNewIf(foamNewCast(FOAM_Bool, foamCopy(flag)), cache->exitLabel), posn);
}

Foam
gen0CacheReturn(RTCacheInfo cache, Foam value)
{
	AbSyn pos = cache->ab;
	Foam  final = cache->value;

	gen0AddStmt(foamNewSet(foamCopy(final), value), pos);
	gen0AddStmt(foamNewSet(foamCopy(final),
			       gen0BuiltinCacheAdd(foamCopy(cache->cache), 
						   foamCopy(cache->tuple),
						   foamCopy(final))),
		    pos);
	gen0AddStmt(foamNewLabel(cache->exitLabel), pos);
	return foamCopy(final);
}	

void
gen0CacheKill(RTCacheInfo cache)
{
	stoFree(cache);
}

local Foam
gen0BuiltinCacheMake()
{
	AInt	size = gen0GetRtCacheSize();
	String	rt0 = "runtime";
	String	rt1 = "rtCacheMake";
	String	rt2 = "rtCacheDynamicMake";
	String	rt3 = "rtCacheExplicitMake";


	/* Be careful - don't want a duff cache */
	if (size > 0)
	{
		Foam	fm = foamNewSInt(size);
		return gen0BuiltinCCall(FOAM_Word, rt3, rt0, 1, fm);
	}
	else if (size < 0)
		return gen0BuiltinCCall(FOAM_Word, rt2, rt0, int0);
	else /* (size == 0) */
		return gen0BuiltinCCall(FOAM_Word, rt1, rt0, int0);
}

local Foam
gen0BuiltinCacheCheck(Foam cache, Foam value)
{
	AInt	size = gen0GetRtCacheSize();
	String	rt0 = "runtime";
	String	rt1 = "rtCacheCheck";
	String	rt2 = "rtCacheDynamicCheck";
	Foam		call;

	/* Static or dynamic cache? */
	if (size >= 0)
		call = gen0BuiltinCCall(FOAM_Word, rt1, rt0, 2, cache, value);
	else
		call = gen0BuiltinCCall(FOAM_Word, rt2, rt0, 2, cache, value);

	return foamNewMFmt(gen0MakeCCheckFormat(), call);
}

local Foam
gen0BuiltinCacheAdd(Foam cache, Foam key, Foam value)
{
	AInt	size = gen0GetRtCacheSize();
	String	rt0 = "runtime";
	String	rt1 = "rtCacheAdd";
	String	rt2 = "rtCacheDynamicAdd";

	/* Static or dynamic cache? */
	if (size >= 0)
		return gen0BuiltinCCall(FOAM_Word, rt1, rt0, 3, cache, key, value);
	else
		return gen0BuiltinCCall(FOAM_Word, rt2, rt0, 3, cache, key, value);
}

local int
gen0MakeCCheckFormat()
{
        if (gen0CCheckFormat)
                return gen0CCheckFormat;

	gen0CCheckFormat = gen0StdDeclFormat(gen0CCheckSize, gen0CCheckNames, 
					    gen0CCheckTypes, gen0CCheckFmts);
        return gen0CCheckFormat;
}

/*
 * Debugging (old style via -Wdebug)
 */

void
gen0DebugIssueStmt(GenDebugKind kind, String name, 
		   int line, Foam type, Foam value, Foam v2)
{	
	Foam foam;

	foam = gen0BuiltinCCall(FOAM_Word, "rtDoDebug", "runtime", 6,
				foamNewCast(FOAM_Word, foamNewSInt(kind)),
				foamNewCast(FOAM_Word, gen0CharArray(name)),
				foamNewCast(FOAM_Word, foamNewSInt(line)),
				type, value, v2);
				
	gen0AddStmt(foam, NULL);
}


/*
 * Debugging (new style via -Wdebugger)
 */

Foam
gen0DbgGetPause(void)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugPaused?",
		"runtime",
		(Length)0
	);
}

Foam
gen0DbgSetPause(Foam value)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugPaused!",
		"runtime",
		1,
		foamCopy(value)
	);
}

Foam
gen0DbgEnter(String fil, AInt lin, Foam exp, String nam, Foam typ, AInt argc)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugEnter",
		"runtime",
		6,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(exp),
		foamNewCast(FOAM_Word, gen0CharArray(nam)),
		foamCopy(typ),
		foamNewSInt(argc)
	);
}

Foam
gen0DbgInit(String fil, AInt lin, Foam exp, String nam, Foam typ, AInt argc)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugInit",
		"runtime",
		6,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(exp),
		foamNewCast(FOAM_Word, gen0CharArray(nam)),
		foamCopy(typ),
		foamNewSInt(argc)
	);
}

Foam
gen0DbgInside(Foam context)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugInside",
		"runtime",
		1,
		foamCopy(context)
	);
}

Foam
gen0DbgReturn(String fil, AInt lin, Foam context, Foam typ, Foam val)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugReturn",
		"runtime",
		5,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(context),
		foamCopy(typ),
		foamCopy(val)
	);
}

Foam
gen0DbgExit(String fil, AInt lin, Foam context)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugExit",
		"runtime",
		3,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(context)
	);
}

Foam
gen0DbgAssign(String fil, AInt lin, Foam context, String var, Foam typ,
	Foam val, AInt depth, AInt vno)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugAssign",
		"runtime",
		8,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(context),
		foamNewCast(FOAM_Word, gen0CharArray(var)),
		foamCopy(typ),
		foamCopy(val),
		foamNewSInt(depth),
		foamNewSInt(vno+1)	/* Aldor uses 1-based arrays */
	);
}

Foam
gen0DbgThrow(String fil, AInt lin, Foam context, Foam exn)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugThrow",
		"runtime",
		4,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(context),
		foamCopy(exn)
	);
}

Foam
gen0DbgCatch(String fil, AInt lin, Foam context, Foam exn)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugCatch",
		"runtime",
		4,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(context),
		foamCopy(exn)
	);
}

Foam
gen0DbgCall(String fil, AInt lin, Foam context, String fun, Foam exporter,
	Foam typ, AInt argc)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugCall",
		"runtime",
		7,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(context),
		foamNewCast(FOAM_Word, gen0CharArray(fun)),
		foamCopy(exporter),
		foamCopy(typ),
		foamNewSInt(argc)
	);
}

Foam
gen0DbgStep(String fil, AInt lin, Foam context)
{
	return gen0BuiltinCCall(
		FOAM_Word,
		"rtDebugStep",
		"runtime",
		3,
		foamNewCast(FOAM_Word, gen0CharArray(fil)),
		foamNewSInt(lin),
		foamCopy(context)
	);
}

