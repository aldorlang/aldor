/*****************************************************************************
 *
 * gf_imps.c: Constructing lazy getters for imports
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
/*
 * To Do:
 *	saving and restoring where information in gen0{x}State
 * 	More efficient mechanism for constants
 */
#include "gf_add.h"
#include "gf_imps.h"
#include "gf_prog.h"
#include "gf_rtime.h"
#include "gf_util.h"
#include "of_inlin.h"
#include "store.h"
#include "sefo.h"
#include "lib.h"
#include "tfsat.h"
#include "tform.h"
#include "symbol.h"
#include "strops.h"
#include "table.h"

/*****************************************************************************
 *
 * :: Local function declarations.
 *
 ****************************************************************************/

local Foam	   gen0GetLibImport	  	(Syme, TForm);
local void 	   gen0RestorePreviousState 	(GenFoamState);
local GenFoamState gen0UsePreviousState     	(AInt);
local void	gen0SetSymeInit	(Syme, AInt);
local Foam	gen0LazyFunGet		(TForm, Foam);
local Foam	gen0LazyFunGetByArgs	(Length, Length, Foam);
local Foam	gen0LazyConstGet	(Syme, Foam, Foam, Foam);
local Foam	gen0FindLazySig		(AIntList, FoamTag, AIntList, Foam);
local Bool	gen0AIntEqual		(AInt, AInt);
local Bool	gen0AIntHash		(AInt);
local Foam	gen0MakeGetExport	(Foam, Foam, Foam);
local Foam	gen0BuildLazyFun0	(FoamSig);
local Foam	gen0BuildLazyFun1	(FoamSig);
local Foam	gen0BuildLazyFun2	(void);
local int 	gen0GetLazyFunFormat	(FoamSig);

local Foam	gen0GVectFn		(Lib);
local int	gen0GVectIdx		(Lib, Syme);
local Foam	gen0GVectFnByName	(String);
local int	gen0GVectIdxByName	(String, AInt);
local Foam	gen0MakeLazyGloFn	(TForm, Foam, int);
local Foam	gen0MakeLazyGloDom	(Foam, int);
local Foam	gen0MakeLazyGloCat	(Foam, int);

local Foam	gen0DelayedInit		(Foam, int);
local Foam	gen0DelayedGetExport	(Foam, Foam, Foam);
local Foam 	gen0GetBuiltin		(String, AInt, Length, Length);

local Foam	gen0StdLazyRef		(FoamSig sig);
local Bool	gen0IsStdLazySig	(FoamSig sig);

/*****************************************************************************
 *
 * :: Generate initialization code for an imported symbol
 *
 ****************************************************************************/

void 
gen0InitImport(Syme syme)
{
	GenSaveState ss;
	TForm exporter;
	Foam foam, call;
	AInt symeLexLevel = gen0GetImportLexLevel(syme);
	int  idx;

 	exporter = symeExporter(syme);
	syme = symeExportingSyme(syme);

	idx = gen0MoveToImportPlace(&ss, symeLexLevel);

	gen0SetSymeInit(syme, int0);

	if (tfIsArchive(exporter))
		exporter = tfArchiveLib(exporter, syme);

	if (tfIsLibrary(exporter)) 
		call = gen0GetLibImport(syme, exporter);
	else
		call = gen0GetDomImport(syme, gen0GetDomain(exporter, idx));

	foam = gen0NewLex(idx, gen0VarIndex(syme));

	gen0UseStackedFormat(int0);
	gen0AddStmt(foamNewDef(foam, call), NULL);

	gen0RestoreFromImportPlace(&ss);
}

Foam
gen0GetDomainLex(TForm tf)
{
	GenSaveState ss;
	Foam	     foam;
	AInt	     lexLevel;
	int	     level, idx;

	assert(!tfIsLibrary(tf));
	lexLevel = (AInt) gen0GetSefoLexLevel(tfExpr(tf));
	level    = gen0FoamImportLevel(lexLevel);

	idx = gen0MoveToImportPlace(&ss, lexLevel);

	foam = gen0GetDomain(tf, idx);
	
	gen0AddLexLevels(foam, level);
	gen0RestoreFromImportPlace(&ss);

	if (foamTag(foam) == FOAM_Lex)
		gen0UseStackedFormat(foam->foamLex.level);

	return foam;
}

Foam
gen0GetLazyBuiltin(String libName, AInt gloId, Length nIn, Length nOut)
{
	GenSaveState ss;
	Foam	     foam;
	int	     level;

	level    = gen0RootEnv();

	(void) gen0MoveToImportPlace(&ss, 1);

	foam = gen0GetBuiltin(libName, gloId, nIn, nOut);
	
	gen0AddLexLevels(foam, level);
	gen0RestoreFromImportPlace(&ss);

	if (foamTag(foam) == FOAM_Lex)
		gen0UseStackedFormat(foam->foamLex.level);

	return foam;

}


local GenFoamState 
gen0UsePreviousState(AInt level)
{
	GenFoamState tmp = gen0State;
	gen0State = gen0NthState(level);

	return tmp;
}

local void
gen0RestorePreviousState(GenFoamState state)
{
	gen0State = state;
}

int
gen0MoveToImportPlace(GenSaveState *ss, AInt lexLevel)
{
	FoamList savedLines;
	FoamList *savedPlace, *wherePlace = NULL;
	AInt level;
	Bool deep;
	int  idx;

	level     = gen0FoamImportLevel(lexLevel);
	ss->state = gen0UsePreviousState(level);

	if (gen0InDeep(gen0State->progType)) {
		deep = true;
		gen0ProgUseBaseState();
	} else 
		deep = false;
	
	if (gen0State->whereNest)
		idx = gen0State->whereNest - 
				(lexLevel - gen0State->stabLevel);
	else
		idx = 0;

	if (lexLevel < gen0State->stabLevel)
		idx = gen0State->whereNest;

	if (gen0State->whereNest /*&& gen0State->importPlace*/) {
		wherePlace = gen0State->importPlace;
		if (idx)
			gen0State->importPlace = (FoamList *)
				listElt(AInt)(gen0State->importPlacePrev, 
					      idx - 1);
	} 

	if (gen0State->importPlace != NULL) {
		savedLines = gen0State->lines;
		savedPlace = gen0State->importPlace;
		gen0State->lines = *gen0State->importPlace;
		gen0State->importPlace = NULL;
	}
	else {
		savedLines = NULL;
		savedPlace = NULL;
	}
	ss->savedLines = savedLines;
	ss->savedPlace = savedPlace;
	ss->wherePlace = wherePlace;
	ss->deep       = deep;
	ss->idx        = idx;

	return idx;
}

void
gen0RestoreFromImportPlace(GenSaveState *ss)
{
	FoamList savedLines  = ss->savedLines;
	FoamList *savedPlace = ss->savedPlace;
        FoamList *wherePlace = ss->wherePlace;
	Bool deep	     = ss->deep;

	if (savedPlace != NULL) {
		gen0State->importPlace  = savedPlace;
		*gen0State->importPlace = gen0State->lines;
		gen0State->lines = savedLines;
	}
	
	if (gen0State->whereNest && savedPlace != NULL) 
		gen0State->importPlace = wherePlace;

	gen0RestorePreviousState(ss->state);
	if (deep)
		gen0ProgUseUpperState();
}


Bool
gen0AddImportPlace(FoamList *var)
{	
	/* Don't bother if there is already a place */
	if (gen0State->importPlace == NULL) {
		*var = gen0State->lines;
		gen0State->importPlace = var;
		gen0State->lines = listNil(Foam);
		return true;
	}
	else
		return false;
}

void
gen0ResetImportPlace(FoamList var)
{
	gen0State->lines = listNConcat(Foam)(gen0State->lines, var);
	gen0State->importPlace = NULL;
}


local Foam
gen0GetLibImport(Syme syme, TForm exporter)
{
	TForm tf = symeType(syme);
	Foam  initfn, foam;
	int   idx;

	initfn  = gen0GVectFn(tfLibraryLib(exporter));
	idx     = gen0GVectIdx(tfLibraryLib(exporter), syme);

	if (tfSatDom(tf))
		foam = gen0MakeLazyGloDom(initfn, idx);
	else if (tfSatCat(tf))
		foam = gen0MakeLazyGloCat(initfn, idx);
	else if (tfIsAnyMap(tf))
		foam = gen0MakeLazyGloFn(tf, initfn, idx);
	else {
		foam = foamNew(FOAM_CCall, 3,
			       FOAM_Word, initfn, foamNewSInt(idx));
		foamPure(foam) = true;
	}
	return foam;
}

local void
gen0SetSymeInit(Syme syme, AInt level)
{
	SymeList	l = gen0NthState(level)->funImportList;
	for(; l != 0; l = cdr(l))
		if (symeEqual(syme, car(l))) {
			symeSetImportInit(car(l));
			return;
		}
	return;
}

/*****************************************************************************
 *
 * :: decls for lazy environments
 *
 ****************************************************************************/

#define     gen0LazyEnvSize    3
static String  gen0LazyEnvNames[] = { "getter", "flag", "self" };
static FoamTag gen0LazyEnvTypes[] = { FOAM_Clos, FOAM_Bool, FOAM_Clos };
static AInt    gen0LazyEnvFmts [] = { emptyFormatSlot, emptyFormatSlot, 
				      emptyFormatSlot };

static Foam gen0LazyFun2Const;

/*****************************************************************************
 *
 * :: Lazy-import getting operations
 *
 ****************************************************************************/

Foam
gen0GetDomImport(Syme syme, Foam dom)
{ 
	String	str = symeString(syme);
	TForm	tf  = symeType(syme);
	TForm	otf = symeType(symeOriginal(syme));
	Foam	call, name, type;
	FoamTag	fmType = gen0Type(tf, NULL);

	tfFollow(tf);
	name = foamNewSInt(gen0StrHash(str));
	type = gen0TypeHash(tf, otf, str);

#if 0
	genfExportDEBUG(dbOut, "---LazGet:      %9d (%s from %s)\n",
			type, str, "???");
#endif

	if (tfIsAnyMap(tf)) {
		/*
		 * NB: When calling this function, we will have to
		 * cast the arguments and return types if necessary.
		 */
		call = gen0LazyFunGet(otf, gen0DelayedGetExport(dom, name, type));
	}
	else if (tfSatType(tf)) {
		/* Don't use lazy gets for imported types. */
		call = gen0MakeGetExport(dom, name, type);
		call = foamNewCast(fmType, call);
	}
	else 
		call = gen0LazyConstGet(syme, dom, name, type);

	return call;
}

/*****************************************************************************
 *
 * :: Internal helper routines
 *
 ****************************************************************************/

local Foam	gen0LazySigCall		(FoamSig, Foam);
local Foam	gen0StdLazySigCall	(FoamSig, Foam);
local FoamSig	gen0AddLazySig		(FoamSig);

local Foam
gen0LazyFunGet(TForm tf, Foam forcingFn)
{
	TForm		tfret = tfMapRet(tf);
	Foam		val;
        AIntList	inArgs;
	FoamTag		retType;
	AIntList	retVals;
	AInt		retfmt;
	Length		i;

	inArgs = listNil(AInt);
	for (i = 0; i < tfMapArgc(tf); i += 1) {
		AInt type, fmt;
		type = gen0Type(tfMapArgN(tf, i), &fmt);
		type = (fmt << 8) + type;
		inArgs = listCons(AInt)(type, inArgs);
	}
	inArgs  = listNReverse(AInt)(inArgs);

	tfFollow(tfret);
	retType = gen0Type(tfret, &retfmt);
	retType = retType + (retfmt << 8);
	retVals = listNil(AInt);

	if (tfIsMulti(tfret) && !tfIsNone(tfret)) {
		for (i=0 ; i < tfMultiArgc(tfret); i++)
			retVals = listCons(AInt)(gen0Type(tfMultiArgN(tfret, i), NULL),
						 retVals);
		retVals = listNReverse(AInt)(retVals);
	}

	val = gen0FindLazySig(inArgs, retType, retVals, forcingFn);
	
	listFree(AInt)(inArgs);
	listFree(AInt)(retVals);
	
	return val;
}

/* 
 * Get a lazy function which takes 'inArgs' FOAM_Word arguments, and
 * returns 'outArgs'
 */

local Foam
gen0LazyFunGetByArgs(Length nInArgs, Length nOutArgs, Foam forcingFn)
{
	Foam val;
	AIntList inSig = listNil(AInt);
	FoamTag  retType;
	int i;

	/* !! Could build a ddecl if nOutArgs >= 2 */
	assert(nOutArgs < 2);
	
	for (i=0; i < nInArgs; i++) {
		inSig = listCons(AInt)((AInt) FOAM_Word, inSig);
	}
	
	if (nOutArgs == 0)
		retType = FOAM_NOp;
	else
		retType = FOAM_Word;

	val = gen0FindLazySig(inSig, retType, listNil(AInt), forcingFn);
	
	return val;
}

local Foam
gen0LazyConstGet(Syme syme, Foam dom, Foam name, Foam type)
{
	Foam call;

	call = gen0BuiltinCCall(FOAM_Word, "lazyGetExport!", "runtime", 3,
				dom, name, type);
	foamPure(call) = true;

	return call;
}
	
Foam
gen0LazyValue(Foam var, Syme syme)
{
	Foam 	foam;
	FoamTag type = gen0Type(symeType(syme), NULL);

	if (symeIsImport(syme) && tfIsLibrary(symeExporter(syme)))
		return foamNewCast(type, var);

	foam = gen0BuiltinCCall(FOAM_Word, 
				"lazyForceImport",
				"runtime", 1, var);
	foamSyme(var) = syme;
	foamPure(foam) = true;
	return foamNewCast(type, foam);
}

local Foam
gen0FindLazySig(AIntList inArgs, FoamTag retType, AIntList outVals,
		Foam forcingFn)
{
	FoamSig 	sig, realsig = NULL;
	Foam 		foam;
	FoamTag        *rets;
	AIntList	l;
	int 		i, nRets;
	
	l     = outVals;
	rets  = NULL;
	nRets = listLength(AInt)(outVals);
	while (l != listNil(AInt)) {
		if (car(l) != FOAM_Word) 
			break;
		l = cdr(l);
	}

	if (l != listNil(AInt)) {
		l = outVals;
		rets = (FoamTag*) stoAlloc(OB_Other, nRets*sizeof(FoamTag));
		for (i=0; i < nRets; i++) {
			rets[i] = car(l);
			l = cdr(l);
		}
	}
	
	sig = foamSigNew(inArgs, retType, nRets, rets);

	if (gen0IsStdLazySig(sig))
		foam = gen0StdLazySigCall(sig, forcingFn);
	else {
		FoamSigList 	siglst;
		siglst = gen0LazySigList;
		while (siglst != listNil(FoamSig)) {
			if (foamSigEqual(car(siglst), sig))
				break;
			siglst = cdr(siglst);
		}
		
		if (siglst == listNil(FoamSig))
			realsig = gen0AddLazySig(sig);
		else
			realsig = car(siglst);

		foam = gen0LazySigCall(realsig, forcingFn);
	}

	if (sig != realsig) foamSigFree(sig);
	return foam;
}

local Foam
gen0LazySigCall(FoamSig sig, Foam forcingFn)
{
	Foam call, env;
	
	env  = foamNewEnv(gen0RootEnv());
	call = foamNew(FOAM_OCall, 4, FOAM_Clos, 
		       foamCopy(sig->ref), env,
		       forcingFn);
	gen0UseStackedFormat(env->foamEnv.level);
	foamPure(call) = true;

	return call;

}

local Foam
gen0StdLazySigCall(FoamSig sig, Foam forcingFn)
{
	Foam call;

	call = foamNew(FOAM_CCall, 3, FOAM_Clos, 
		       gen0StdLazyRef(sig),
		       forcingFn);

	foamPure(call) = true;
	return call;
}

local FoamSig
gen0AddLazySig(FoamSig sig)
{
	sig->constNum = gen0FwdProgNum--;
	sig->ref      = foamNewConst(sig->constNum);

	gen0LazySigList = listCons(FoamSig)(sig, gen0LazySigList);
	return sig;
}



local Bool
gen0AIntEqual(AInt x, AInt y)
{
	return x == y;
}

local Bool
gen0AIntHash(AInt x)
{
	return x;
}

local Foam
gen0MakeGetExport(Foam dom, Foam name, Foam type)
{
	Foam call;
	call = gen0BuiltinCCall(FOAM_Word, "domainGetExport!",
				"runtime", 3,
				dom, name, type);
	foamPure(call) = true;
	return call;
}

void
gen0IssueLazyFunctions()
{
	FoamSigList funs = gen0LazySigList;

	while (funs) {
		gen0AddConst(car(funs)->constNum, gen0NumProgs);
		gen0BuildLazyFun0(car(funs));
		funs = cdr(funs);
	}

	foamFree(gen0LazyFun2Const);
	gen0LazyFun2Const = NULL;
	
	/* global lists freed in gen0GenFoamFini */
}

local Foam
gen0BuildLazyFun0(FoamSig fun)
{
	GenFoamState saved;
	Foam 	     foam, clos, envInfo, decl;

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(strCopy("lazyFnGetter"), NULL);

	saved = gen0ProgSaveState(PT_ExFn);

	gen0ProgPushFormat(gen0GetLazyFunFormat(fun));

	decl = foamNewDecl(FOAM_Word, strCopy("init"), 
			   emptyFormatSlot);
	gen0AddParam(decl);
	
	gen0AddStmt(foamNewDef(foamNewLex(int0, int0),
			       foamNewCast(FOAM_Clos, foamNewPar(int0))), NULL);

	gen0AddStmt(foamNewSet(foamNewLex(int0, 1),
			       foamNewBool(false)), NULL);
	gen0AddStmt(foamNewSet(foamNewLex(int0, 2),
			       gen0BuildLazyFun1(fun)), NULL);
	envInfo = foamNewEInfo(foamNewEnv(int0));
	foamLazy(envInfo) = true;
	gen0AddStmt(foamNewSet(envInfo,
			       foamNewCast(FOAM_Word,
					   gen0BuildLazyFun2())),
			       NULL);
	gen0AddStmt(foamNewReturn(foamNewLex(int0, 2)), NULL);

	gen0ProgFiniEmpty(foam, FOAM_Clos, int0);

	gen0ProgRestoreState(saved);
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);
	foamProgSetGetter(foam);
	return clos;
}

local Foam
gen0BuildLazyFun1(FoamSig sig)
{
	GenFoamState saved;
	Foam 	     result, foam, clos, ccall, tmp;
	AIntList     l;
	AInt	     retType, retFmt;
	static const char *letters = "abcdefghijklmnopqrstuvwxyz";
	char x[2];
	int i, inArgc, callLabel;

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(strCopy("lazyGetter"), NULL);

	saved = gen0ProgSaveState(PT_ExFn);

	assert(sig->nRets != 1);
	if (sig->nRets != 0)
		retFmt = gen0MFmtNumberForSig(sig->nRets, sig->rets);
	else 
		retFmt = sig->retType >> 8;

	x[1] = 0;
	i    = 0;
	for (l = sig->inArgs; l ; l = cdr(l)) {
		AInt type, fmt;
		type = car(l) & 0xFF;
		fmt  = car(l) >> 8;
		x[0] = letters[i];
		gen0AddParam(foamNewDecl(type, strCopy(x), fmt));
		if (i<26) i++; else i=0;
	}
	inArgc = listLength(AInt)(sig->inArgs);

	tmp = gen0TempLocal(FOAM_Clos);
	callLabel = gen0State->labelNo++;
	gen0AddStmt(foamNewIf(foamNewLex(1, 1), callLabel), NULL);
	gen0AddStmt(foamNewSet(foamCopy(tmp),
			       foamNewCast(FOAM_Clos,
					   foamNew(FOAM_CCall, 2,
						   FOAM_Word,
						   foamNewLex(1, int0)))),
		    NULL);
	/* Zap this function */
	gen0AddStmt(foamNewSet(foamNewCEnv(foamNewLex(1, 2)),
			       foamNewCEnv(foamCopy(tmp))), NULL);
	gen0AddStmt(foamNewSet(foamNewCProg(foamNewLex(1, 2)),
			       foamNewCProg(foamCopy(tmp))), NULL);
	/* Now tell the world */
	gen0AddStmt(foamNewSet(foamNewLex(1, 1), foamNewBool(true)), NULL);

	gen0AddStmt(foamNewLabel(callLabel), NULL);
	/* Call the retrieved function */
	ccall = foamNewEmpty(FOAM_CCall, inArgc + 2);
	
	retType = sig->retType & 0xFF;
	ccall->foamCCall.type = retType;
	
	ccall->foamCCall.op = foamNewLex(1, 2);
	for (i=0; i<inArgc; i++)
		ccall->foamCCall.argv[i] = foamNewPar(i);

	if (retType != FOAM_NOp) 
		result = ccall;
	else if (retFmt == emptyFormatSlot) {
		gen0AddStmt(ccall, NULL);
		result = foamNew(FOAM_Values, int0);
	} else {
		assert(retFmt != 0);
		tmp = gen0TempFrDDecl(retFmt, true);
	        gen0AddStmt(foamNewSet(tmp, foamNewMFmt(retFmt, ccall)),
			    NULL);
		result = foamCopy(tmp);
	}

	gen0AddStmt(foamNewReturn(result), NULL);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, retType, retFmt);
	foam->foamProg.format = retFmt;
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);
	gen0ProgRestoreState(saved);
	return clos;
}

/**!! We only need one of these per file */
local Foam
gen0BuildLazyFun2()
{
	GenFoamState saved;
	Foam 	     foam, clos, tmp, envInfo;
	int testLabel;

	if (gen0LazyFun2Const != NULL) 
		return foamCopy(gen0LazyFun2Const);

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(strCopy("lazyGetter2"), NULL);

	saved = gen0ProgSaveState(PT_ExFn);
	testLabel = gen0State->labelNo++;
	tmp = gen0TempLocal(FOAM_Clos);

	gen0AddStmt(foamNewIf(foamNewLex(1,1), testLabel), NULL);
	gen0AddStmt(foamNewSet(tmp, 
			       foamNewCast(FOAM_Clos,
					   foamNew(FOAM_CCall, 2,
						   FOAM_Word,
						   foamNewLex(1, int0)))),
		    NULL);
	gen0AddStmt(foamNewSet(foamNewLex(1, 1), foamNewBool(true)), NULL);

	/* Zap this function */
	gen0AddStmt(foamNewSet(foamNewCEnv(foamNewLex(1, 2)),
			       foamNewCEnv(foamCopy(tmp))), NULL);
	gen0AddStmt(foamNewSet(foamNewCProg(foamNewLex(1, 2)),
			       foamNewCProg(foamCopy(tmp))), NULL);
	envInfo = foamNewEInfo(foamNewEnv(1));
	foamLazy(envInfo) = true;
	gen0AddStmt(foamNewSet(envInfo,
			       foamNewCast(FOAM_Word,
					   foamNewNil())),
		    NULL);
	gen0AddStmt(foamNewLabel(testLabel), NULL);
	gen0AddStmt(foamNewReturn(foamNew(FOAM_Values, int0)), NULL);
	
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, FOAM_NOp, int0);
	foam->foamProg.format = 0;
	gen0ProgRestoreState(saved);
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);
	foamProgSetGetter(foam);
	
	gen0LazyFun2Const = clos;
	return foamCopy(clos);
	
}

local int 
gen0GetLazyFunFormat(FoamSig sig)
{
	if (gen0LazyFunFormat) 
		return gen0LazyFunFormat;

	gen0LazyFunFormat = 
		gen0StdDeclFormat(gen0LazyEnvSize, gen0LazyEnvNames, 
				  gen0LazyEnvTypes, gen0LazyEnvFmts);
	
	return gen0LazyFunFormat;
}

/*****************************************************************************
 *
 * :: Standard Signatures
 *
 ****************************************************************************/

local void	gen0StdGetsCreate0	(AIntList);
local void	gen0StdGetsCreate1	(AIntList, FoamTag, int);
local String	gen0StdLazyName		(AIntList, AInt, int);

/*
 * Because practically all files call a function of none, two and
 * three args, returning any of no values, one value, two values
 * or a closure, we instantiate these exactly once (in runtime.as).
 */


local Foam
gen0StdLazyRef(FoamSig sig)
{
	Foam decl;
	
	decl = foamNewGDecl(FOAM_Clos, 
			    gen0StdLazyName(sig->inArgs, sig->retType, sig->nRets),
			    emptyFormatSlot,
			    FOAM_GDecl_Import, FOAM_Proto_Foam);
	return foamNewGlo(gen0AddGlobal(decl));
}

void
gen0StdLazyGetsCreate()
{
	AIntList args;

	/* No arguments */
	args = listNil(AInt);
	gen0StdGetsCreate0(args);

	/* One (closure) argument */
	args = listCons(AInt)(FOAM_Clos, args);
	gen0StdGetsCreate0(args);

	/* One (word) argument */
	car(args) = FOAM_Word;
	gen0StdGetsCreate0(args);

	/* One (word) argument */
	args = listCons(AInt)(FOAM_Word, args);
	gen0StdGetsCreate0(args);

	/* One (word) argument */
	args = listCons(AInt)(FOAM_Word, args);
	gen0StdGetsCreate0(args);
}

local void
gen0StdGetsCreate0(AIntList args)
{
	FoamTag  retType;
	int	 nRets;

	/* Returning a closure */
	retType = FOAM_Clos; 
	nRets  = 0;
	gen0StdGetsCreate1(args, retType, nRets);


	/* Returning nothing */
	retType = FOAM_NOp;
	nRets  = 0;
	gen0StdGetsCreate1(args, retType, nRets);


	/* Returning a word */
	retType = FOAM_Word;
  	nRets   = 0;
	gen0StdGetsCreate1(args, retType, nRets);


	/* Returning two words */
	retType = FOAM_NOp;
	nRets  = 2;
	gen0StdGetsCreate1(args, retType, nRets);
}

local void
gen0StdGetsCreate1(AIntList args, FoamTag retType, int nRets)
{
	Foam     foam, decl;
	FoamSig  sig;
	String   name;
	AInt	 idx;

	retType = retType + (emptyFormatSlot << 8);

	sig = foamSigNew(args, retType, nRets, NULL);
	gen0AddLazySig(sig);
	foam = foamNewClos(foamNewEnv(int0), foamCopy(sig->ref));
	name = gen0StdLazyName(args, retType, nRets);
	decl = foamNewGDecl(FOAM_Clos, name, emptyFormatSlot,
			    FOAM_GDecl_Export, FOAM_Proto_Foam);
	idx = gen0AddGlobal(decl);
	gen0AddStmt(foamNewSet(foamNewGlo(idx), foam), NULL);
	gen0BuiltinExports = listCons(AInt)(idx, gen0BuiltinExports);
	gen0BuiltinExports = listCons(AInt)(int0, gen0BuiltinExports);
}


local String
gen0StdLazyName(AIntList args, AInt retType, int nRets)
{	
	String name, tmp;
	retType = retType & 0xFF;
	
	name = strCopy(gen0StdLazyNamePrefix());
	while (args != listNil(AInt)) {
		tmp = name;
		name = strConcat(name, foamInfo(car(args) & 0xFF).str);
		strFree(tmp);
		args = cdr(args);
	}
	
	tmp  = name;
	name = strPrintf("%sRet%s%d", name, foamInfo(retType).str, nRets);
	strFree(tmp);

	return name;
}


local Bool
gen0IsStdLazySig(FoamSig sig)
{
	AIntList lst;
	AIntList inArgs  = sig->inArgs;
	FoamTag  retType = sig->retType & 0xFF;
	int 	 nRets   = sig->nRets;
	FoamTag *rets	 = sig->rets;
	Bool     ok;

	lst = inArgs;

	if (listLength(AInt)(inArgs) > 3)
		return false;

	if (listLength(AInt)(inArgs) == 1) 
		ok = ((car(inArgs) & 0xFF) == FOAM_Word 
		      || (car(inArgs) & 0xFF) == FOAM_Clos);
	else {
		ok = true;
		while (ok && inArgs != listNil(AInt)) {
			ok = (car(inArgs) & 0xFF) == FOAM_Word;
			inArgs = cdr(inArgs);
		}
	}
	if (!ok)
		return false;

	if (retType != FOAM_NOp && retType != FOAM_Word)
		return false;

	if (nRets > 2)
		return false;
	
	ok = true;
	if (rets) {
		int i;
		for (i=0; i<nRets; i++)
			ok = rets[i] && ok;
	}

	if (!ok)
		return false;

	return true;
}

/*****************************************************************************
 *
 * :: Global getters
 *
 ****************************************************************************/

/* 
 * To avoid instantiating an entire library at initialisation time,
 * we use Global Vector functions that simply return a vector containing 
 * the globals from a particular module, instantiating the module if
 * necessary. As a further enhancement, newly created domains are 
 * pointers to the real domain (this is controlled by the
 * "runtimeMakeFakeDomain" which ensures that a domain is initialised 
 * iff an export/hashcode is needed.
 *
 * Similarly, functions are not completely instantiated until they are 
 * called, and type returning functions do not cause instantiation until
 * an export or hashcode is needed. 
 * 
 */
local AIntList  gen0GVectFindExports	(String);
local void	gen0GVectPutExports	(String, AIntList);
local void 	gen0GVectIssueFn	(String, AIntList);
local AInt	gen0GlobalRef		(Lib, Syme);

static Table	gen0GVectTable;
static Table	gen0GVectFnTable;



void 
gen0InitGVectTable()
{
	gen0GVectTable   = tblNew((TblHashFun)strHash, 
				  (TblEqFun)strEqual);
	gen0GVectFnTable = tblNew((TblHashFun)strHash,
				  (TblEqFun)strEqual);
}

void 
gen0FiniGVectTable()
{
	tblFree(gen0GVectTable);
	tblFree(gen0GVectFnTable);
	gen0GVectTable   = NULL;
	gen0GVectFnTable = NULL;
}

local Foam
gen0GVectFn(Lib lib)
{
	String libName = libGetFileId(lib);

	return gen0GVectFnByName(libName);
}

local Foam
gen0GVectFnByName(String libName)
{
	Foam res, var;
	int  num;
	res = (Foam) tblElt(gen0GVectFnTable, (TblKey) libName, (TblElt) NULL);
		
	if (res) return foamCopy(res->foamDef.lhs);

	num = gen0FwdProgNum--;
	
	res = gen0Temp(FOAM_Clos);
	var = foamNewDef(res,
			 foamNewClos(foamNewEnv(int0), 
				     foamNewConst(num)));
	gen0AddInit(var);
	tblSetElt(gen0GVectFnTable, (TblKey) libName, (TblElt) var);

	return foamCopy(res);
}

local int
gen0GVectIdx(Lib lib, Syme syme)
{
	AInt globNo = gen0GlobalRef(lib, syme);
	String name = libGetFileId(lib);

	return gen0GVectIdxByName(name, globNo);
	
}

local int
gen0GVectIdxByName(String libName, AInt globNo)
{
	AIntList lst;
	int 	 len;

	lst = gen0GVectFindExports(libName);

	len = listLength(AInt)(lst);
	
	gen0GVectPutExports(libName, listCons(AInt)(globNo, lst));
	
	return (AInt) len;
}

void 
gen0IssueGVectFns()
{
	TableIterator it;

	for (tblITER(it, gen0GVectTable); tblMORE(it); tblSTEP(it)) {
		AIntList globs;
		String	 libName;
		Foam 	 defn;

		libName = (String)   tblKEY(it);
		globs   = (AIntList) tblELT(it);
		defn = (Foam) tblElt(gen0GVectFnTable, 
				     (TblKey) libName, (TblElt) NULL);
		assert(defn);
		gen0AddConst(defn->foamDef.rhs->foamClos.prog->foamConst.index,
			     gen0NumProgs);
		gen0GVectIssueFn(libName, globs);
	}

}

local AIntList
gen0GVectFindExports(String libName)
{
	return (AIntList) tblElt(gen0GVectTable, 
				 (TblKey) libName,
				 (TblElt) listNil(AInt));
}

local void
gen0GVectPutExports(String libName, AIntList globs)
{
	tblSetElt(gen0GVectTable, (TblKey) libName, (TblElt) globs);
}

/*
 * Produce the following code:
 * xxxArr = NULL;
 * ...
 * xxxInit(int n) 
 * {
 *    if (xxxArr) goto l1;
 *    xxxArr = arrNew(n);
 *    xxxArr[0] = Global<xxx>;
 *    ...
 *    l1:
 *    return xxxArr[n];
 * }
 */
local void 
gen0GVectIssueFn(String libName, AIntList globs)
{
	GenFoamState saved;
	Foam   	     foam, clos, decl, ini;
	String 	     fnName;
	int	     initLabel;
	Foam	     var, flag, ret;
	int	     i, len = listLength(AInt)(globs);

	fnName  = strPrintf("%s-init", libName);
	var  = gen0TempLex(FOAM_Word);
	flag = gen0TempLex(FOAM_Bool);

	gen0AddInit(foamNewSet(foamCopy(flag), foamNewBool(int0)));
		    
	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(fnName, NULL);

	saved = gen0ProgSaveState(PT_ExFn);

	gen0IncLexLevels(flag, 1);

	decl = foamNewDecl(FOAM_SInt, strCopy("idx"), emptyFormatSlot);
	gen0AddParam(decl);

	initLabel = gen0State->labelNo++;
	gen0AddStmt(foamNewIf(foamCopy(flag), initLabel), NULL);
	gen0AddStmt(foamNewSet(foamCopy(flag), foamNewBool(1)), NULL);

	/* Make a call to the initialiser */
	decl = foamNewGDecl(FOAM_Clos, 
			    gen0InitialiserName(libName),
			    emptyFormatSlot,
			    FOAM_GDecl_Import, FOAM_Proto_Init);
	ini = foamNewEmpty(FOAM_CCall, 2);
	ini->foamCCall.type = FOAM_NOp;
	ini->foamCCall.op = foamNewGlo(gen0AddGlobal(decl));
	gen0AddStmt(ini, NULL);

	gen0AddStmt(foamNewLabel(initLabel), NULL);

	for (i = len - 1; i > 0; globs = cdr(globs), i -= 1) {
		Foam	glo = foamNewGlo(car(globs));
		int	nextLabel = gen0State->labelNo++;
		gen0AddStmt(foamNewIf(foamNew(FOAM_BCall, 3, FOAM_BVal_SIntNE,
					      foamNewPar(int0),
					      foamNewSInt(i)),
				      nextLabel), NULL);
		gen0AddStmt(foamNewReturn(foamNewCast(FOAM_Word, glo)), NULL);
		gen0AddStmt(foamNewLabel(nextLabel), NULL);
	}

	ret = foamNewCast(FOAM_Word, foamNewGlo(car(globs)));

	gen0AddStmt(foamNewReturn(ret), NULL);

	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, FOAM_Word, int0);
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);
	gen0ProgRestoreState(saved);

	foamFree(flag);
}

local AInt
gen0GlobalRef(Lib lib, Syme syme)
{
	TForm exporter;
	Foam  decl;
	AInt  globNo;

	exporter = symeExporter(syme);
	if (tfIsArchive(exporter))
		exporter = tfArchiveLib(exporter, syme);
	
	/* Pull the binding */
	decl = foamNewGDecl(gen0Type(symeType(syme), NULL),
			    gen0GlobalName(libGetFileId(lib),
					   syme),
			    emptyFormatSlot,
			    FOAM_GDecl_Import, FOAM_Proto_Foam);
	globNo = gen0AddGlobal(decl);
	
	return globNo;
}

/*
 * Map-Valued globals:
 *   [args]->[res]
 * Issue the following code:
 * 
 * init:
 *   (Set fn (Call MakeFnInitter-<n> initfn idx))
 *   
 * (Def (Const MakeFnInitter-<n>)
 *    ..
 *    (CCall (CCall initfn idx) args))
 *
 * This is precisely what we do for imports in any case.
 */

local Foam
gen0MakeLazyGloFn(TForm tf, Foam initfn, int idx)
{
	Foam call;

	call = gen0LazyFunGet(tf, gen0DelayedInit(initfn, idx));

	return call;
}

/*
 * DomainMap Valued Globals
 * We could make these extra-lazy by making appropriate wrappers that
 * terminate in a call to makeLazyDomFrInit. However, 
 * this may incur too much static cost, so it has been left.
 * Note that the method used loses PRef information
 *    --- To fix this would require that the 'PRef 0' slot be
 *	  made into a function, and thus changes to gf_add, of_hfold.
 */

/*
 * Domain Valued Globals
 *
 * (Call MkFakeDomain (mklambda initfn idx))
 *
 * Ditto Cat Globals
 */

local Foam
gen0MakeLazyGloDom(Foam initfn, int idx)
{
	Foam call;

	call = gen0BuiltinCCall(FOAM_Word, "rtLazyDomFrInit", 
				"runtime", 2, initfn, foamNewSInt(idx));

	foamPure(call) = true;
	return call;
}

local Foam
gen0MakeLazyGloCat(Foam initfn, int idx)
{
	Foam call;

	call = gen0BuiltinCCall(FOAM_Word, "rtLazyCatFrInit", 
				"runtime", 2, initfn, foamNewSInt(idx));

	foamPure(call) = true;
	return call;
}

/*
 * Globals: helper fns
 */
local Foam
gen0DelayedInit(Foam initfn, int idx)
{
	Foam foam;

	foam = gen0BuiltinCCall(FOAM_Word, "rtDelayedInit!", 
				"runtime", 2, initfn, foamNewSInt(idx));

	foamPure(foam) = true;
	return foam;
}

local Foam
gen0DelayedGetExport(Foam dom, Foam name, Foam type)
{
	Foam foam;

	foam = gen0BuiltinCCall(FOAM_Word, "rtDelayedGetExport!",
				"runtime", 3, dom, name, type);

	foamPure(foam) = true;
	return foam;
}

				
/*****************************************************************************
 *
 * :: Builtin getters
 *
 ****************************************************************************/

static Table    gen0BuiltinTable;

/*
 * For the moment assume all builtin functions
 * take FOAM_Word arguments and return (possibly 0) FOAM_Words
 */

void
gen0InitBuiltinTable()
{
	gen0BuiltinTable = tblNew((TblHashFun) gen0AIntHash,
				  (TblEqFun) gen0AIntEqual);
}

void
gen0FiniBuiltinTable()
{
	tblFree(gen0BuiltinTable);
	gen0BuiltinTable = NULL;
}


local Foam 
gen0GetBuiltin(String libName, AInt glNo,
	       Length inArgs, Length outArgs)
{
	Foam call, env, initfn, tmp;
	int idx;

	if (gen0BuiltinTable == NULL) gen0InitBuiltinTable();

	tmp = (Foam) tblElt(gen0BuiltinTable, (TblElt) glNo, (TblKey) NULL);

	if (tmp != NULL)
		return foamCopy(tmp);

	tmp = gen0TempLex(FOAM_Clos);

	initfn = gen0GVectFnByName(libName);
	idx    = gen0GVectIdxByName(libName, glNo);

	env  = foamNewEnv(gen0RootEnv());
	call = gen0LazyFunGetByArgs(inArgs, outArgs, 
				    gen0DelayedInit(initfn, idx));

	gen0AddStmt(foamNewDef(tmp, call), NULL);

	tblSetElt(gen0BuiltinTable, (TblKey) glNo, (TblElt) tmp);

	return foamCopy(tmp);
	
}
