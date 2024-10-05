/*****************************************************************************
 *
 * gf_gener.c: Foam code generation for generators.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "fluid.h"
#include "genfoam.h"
#include "gf_util.h"
#include "gf_prog.h"
#include "gf_gener.h"
#include "of_inlin.h"
#include "of_util.h"
#include "store.h"
#include "tform.h"
#include "comsg.h"
#include "util.h"

#define GenerBetterGuesses

typedef struct GenBoundCalc *GenBoundCalc;
typedef struct GenerGenInfo *GenerGenInfo;

local Foam         gen0GenerBodyFun        (AbSyn, TForm);
local Foam         gen0GenerSelect         (int);
local Foam	   gen0GenerDoneFun        (void);
local Foam         gen0GenerStepFun	   (AbSyn, TForm);
local Foam	   gen0GenerValueFun  	   (FoamTag, TForm);
local Foam	   gen0GenerBoundFun  	   (AbSyn, GenBoundCalc);
local Foam	   gen0MakeGenerVars	   (TForm);
local Foam	   gen0GetGenerVar	   (Foam env, int id);

#ifdef GenerBetterGuesses
local GenBoundCalc gen0MakeBoundInit(AbSyn);
local Foam 	   gen0ComputeBoundFunction(GenBoundCalc);
local void	   gen0ComputeGeners(void);
#endif

static Foam gen0RetFormatDDeclValue;
static Foam 		gen0GenVars;
static GenerGenInfo gen0GenInfo;

#define 	gen0RetFormatSize 4
static String 	gen0RetFormatNames[] = { "done?", "step!", "value", "bound" };
static FoamTag	gen0RetFormatTypes[] = { FOAM_Clos, FOAM_Clos, FOAM_Clos, FOAM_Clos };
static AInt	gen0RetFormatFmts[]  = { emptyFormatSlot, emptyFormatSlot,
					 emptyFormatSlot, emptyFormatSlot};

#define yieldDoneLoc    0
#define yieldPlaceLoc   1
#define yieldValueLoc	2

#define yieldDoneVar    gen0GetGenerVar(gen0GenVars, yieldDoneLoc)
#define yieldPlaceVar   gen0GetGenerVar(gen0GenVars, yieldPlaceLoc)
#define yieldValVar	gen0GetGenerVar(gen0GenVars, yieldValueLoc)

#define stepperFName()	("generStepper")
#define doneFName()	("generDone?")
#define boundFName()	("generBound")
#define valueFName()	("generValue")

#define boundPlace	3

/*
 * Generate code for a yield.
 */

Foam
genYield(AbSyn absyn)
{
        /* set the place variable */
        gen0AddStmt(foamNewSet(yieldPlaceVar,
			       foamNewSInt(++gen0State->yieldCount)), absyn);
        gen0AddStmt(foamNewSet(foamCopy(gen0State->yieldValueVar),
                               genFoamVal(absyn->abYield.value)), absyn);
        gen0AddStmt(foamNewGoto(gen0State->yieldPlace), absyn);

        gen0AddStmt(foamNewLabel(gen0State->labelNo), absyn);
        gen0State->yieldLabels = listCons(AInt)(gen0State->labelNo++,
                                               gen0State->yieldLabels);
        return 0;
}

/*
 * Generate a generator body (a body containing yields)
 */

Foam
genGenerate(AbSyn iter)
{
        foamProgUnsetLeaf(gen0State->program);
        return gen0GenerBodyFun(iter, tfGeneratorArg(gen0AbType(iter)));
}

/*
 * generate the code for the first level of a generator function.
 */

local Foam
gen0GenerBodyFun(AbSyn iter, TForm tf)
{
	Scope("genBody0");
	FoamList topLines;
	Bool 	 flag;
	GenerGenInfo	fluid(gen0GenInfo);
	GenBoundCalc	calc = NULL;
	AbSyn		body  = iter->abGenerate.body;
	AbSyn		bound = iter->abGenerate.count;
	FoamTag		retType = gen0Type(tf, NULL);
	Foam	fluid(gen0GenVars);

	GenFoamState	saved;
	Foam		foam, clos, stmt;
	Foam		done, step, bnd, value;

	gen0GenInfo = NULL;
#ifdef GenerBetterGuesses 
	if (!bound || abTag(bound) == AB_Nothing) {
		calc = gen0MakeBoundInit(body);
		gen0ComputeGeners();
	}
#endif
	flag = gen0AddImportPlace(&topLines);

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty("generBaseFn", body);

	saved = gen0ProgSaveState(PT_Gener);

	gen0GenVars = gen0MakeGenerVars(tf);

	step   = gen0GenerStepFun(body, tf);
 	done   = gen0GenerDoneFun();
	value  = gen0GenerValueFun(retType, tf);
	bnd    = gen0GenerBoundFun(bound, calc);

	stmt = foamNewReturn(foamNew(FOAM_Values, 4, done, step, value, bnd));
	gen0AddStmt(stmt, body);

	gen0UseStackedFormat(int0);
	gen0ProgPushFormat(int0);
	gen0ProgFiniEmpty(foam, FOAM_NOp, int0);
	foam->foamProg.format = gen0MakeGenerRetFormat();

	gen0AddLexLevels(foam, 1);

        foam->foamProg.infoBits = IB_SIDE | IB_INLINEME;
        foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	if (gen0GenInfo) stoFree(gen0GenInfo);

	gen0ProgRestoreState(saved);

	if (flag) gen0ResetImportPlace(topLines);

	stmt = foamNewSet(yieldPlaceVar, foamNewSInt(int0));
	gen0AddStmt(stmt, iter);

	foamFree(gen0GenVars);
	gen0GenVars = NULL;

        Return(clos);
}

/*
 * Generate the code for the second level of a generator function.
 */

local Foam
gen0GenerStepFun(AbSyn body, TForm tf)
{
	GenFoamState	saved;
	Foam		foam, clos, tmp;
        FoamTag         yieldType;
	AInt		startLabel;
	AInt		fmtSlot;
        clos = foamNewClos(foamNewEnv(-1), foamNewConst(gen0NumProgs));
        foam = gen0ProgInitEmpty(stepperFName(), body);

        saved = gen0ProgSaveState(PT_Gener);

        startLabel = gen0State->labelNo++;
        gen0State->yieldPlace = gen0State->labelNo++;

	tf = tfDefineeType(tf);
        yieldType = gen0Type(tf, &fmtSlot);

        gen0State->yieldValueVar = gen0TempLocal0(yieldType, fmtSlot);
	
	tmp = foamNewNOp();
	gen0AddStmt(tmp, NULL);	/* filled by gen0GenerSelect */
	gen0AddStmt(foamNewLabel(startLabel), NULL);
	gen0AddStmt(foamNewSet(yieldDoneVar, foamNewBool(int0)), NULL);
        genFoamStmt(body);
	gen0AddStmt(foamNewSet(yieldDoneVar, foamNewBool(1)), NULL);
	gen0AddStmt(foamNewReturn(foamNew(FOAM_Values, int0)), NULL);
	gen0AddStmt(foamNewLabel(gen0State->yieldPlace), NULL);
	gen0AddStmt(foamNewSet(yieldValVar, gen0State->yieldValueVar), NULL);
	gen0AddStmt(foamNewReturn(foamNew(FOAM_Values, int0)), NULL);

	gen0UseStackedFormat(int0);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, FOAM_NOp, int0);

	/* fill in the select statement */
	foam->foamProg.body->foamSeq.argv[0] = gen0GenerSelect(startLabel);
	foamFree(tmp);
        gen0AddLexLevels(foam, 2);

        foam->foamProg.infoBits = IB_SIDE | IB_INLINEME;
        foamOptInfo(foam) = inlInfoNew(gen0State->stab, foam, NULL, true);

	gen0ProgRestoreState(saved);
        return clos;
}

/*
 * Generate the select statement for generator body dispatching.
 */

local Foam
gen0GenerSelect(int startLabel)
{
        Foam    select = foamNewEmpty(FOAM_Select, gen0State->yieldCount+2);
        int     i;

        select->foamSelect.op = yieldPlaceVar;
        gen0State->yieldLabels = listNReverse(AInt)(gen0State->yieldLabels);
        select->foamSelect.argv[0] = startLabel;
        for(i=1;
            i <= gen0State->yieldCount;
            i++, gen0State->yieldLabels = cdr(gen0State->yieldLabels))
                select->foamSelect.argv[i] = car(gen0State->yieldLabels);
        return select;
}

/*
 * Make the lexical environment for a generator.
 */

local Foam
gen0MakeGenerVars(TForm retType)
{
	Foam   	done, place, value; 
	FoamTag type;
	AInt    fmt;

	done  = gen0Temp(FOAM_Bool);
	place = gen0Temp(FOAM_SInt);
	/* multi-valued gens not supported yet */
	type = gen0Type(retType, &fmt);
	assert(!tfIsMulti(retType)); 
	value = gen0Temp0(type, fmt);

	return foamNew(FOAM_Values, 3, done, place, value);
}

local Foam
gen0GetGenerVar(Foam env, int id)
{
	return foamCopy(env->foamValues.argv[id]);
}

AInt
gen0MakeGenerRetFormat()
{
	if (gen0GenerRetFormat) return gen0GenerRetFormat;

	gen0GenerRetFormat = gen0StdDeclFormat(gen0RetFormatSize, gen0RetFormatNames,
					       gen0RetFormatTypes, gen0RetFormatFmts);
	gen0SetDDeclUsage(gen0GenerRetFormat, FOAM_DDecl_Multi);
	return gen0GenerRetFormat;
}

Foam 
gen0RetFormatDDecl()
{
	return gen0RetFormatDDeclValue;
}

local Foam
gen0GenerDoneFun()
{
	return gen0BuildFunFromFoam(doneFName(), FOAM_Bool, yieldDoneVar);
}

local Foam 
gen0GenerBoundFun(AbSyn bound, GenBoundCalc calc)
{	
	if (calc)
		return gen0ComputeBoundFunction(calc);
	else if (!bound || abTag(bound) == AB_Nothing)
		return gen0BuildFunFromFoam(boundFName(), FOAM_SInt, 
					    foamNewSInt(-1));
	else {
		GenFoamState	saved;
		Foam		foam, clos, ret;

		clos = foamNewClos(foamNewEnv(-1), foamNewConst(gen0NumProgs));
		foam = gen0ProgInitEmpty(boundFName(), bound);

		saved = gen0ProgSaveState(PT_ExFn);

		ret = genFoamVal(bound);
		if (ret) gen0AddStmt(foamNewReturn(ret), bound);

		gen0UseStackedFormat(int0);
		gen0ProgPushFormat(emptyFormatSlot);
		gen0ProgPushFormat(emptyFormatSlot);
		gen0ProgFiniEmpty(foam, gen0Type(gen0AbType(bound), NULL), int0);

		gen0AddLexLevels(foam, 2);

	        foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

		foam->foamProg.infoBits |= IB_INLINEME;

		gen0ProgRestoreState(saved);
		return clos;		
	}
}

local Foam
gen0GenerValueFun(FoamTag retType, TForm tf)
{
	GenFoamState	saved;
	Foam		foam, clos;
	AInt		fmt;
	FoamTag		type;

	type = gen0Type(tf, &fmt);
	clos = foamNewClos(foamNewEnv(-1), foamNewConst(gen0NumProgs));
	foam = gen0ProgInitEmpty(valueFName(), NULL);

	saved = gen0ProgSaveState(PT_Gener);

	gen0AddStmt(foamNewReturn(foamNewCast(FOAM_Word, yieldValVar)), NULL);

	gen0UseStackedFormat(int0);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, FOAM_Word, emptyFormatSlot);

	gen0AddLexLevels(foam, 2);

	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	gen0ProgRestoreState(saved);
	return clos;
}

local Foam 
gen0BuildFunFromFoam(String name, FoamTag retType, Foam body)
{
	GenFoamState	saved;
	Foam		foam, clos;

	clos = foamNewClos(foamNewEnv(-1), foamNewConst(gen0NumProgs));
	foam = gen0ProgInitEmpty(name, NULL);

	saved = gen0ProgSaveState(PT_Gener);

	gen0AddStmt(foamNewReturn(body), NULL);

	gen0UseStackedFormat(int0);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, retType, int0);

	gen0AddLexLevels(foam, 2);

        foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	gen0ProgRestoreState(saved);
	return clos;
}

/******************************************************************************
 *
 * :: Lifting free generators, and computing bounds
 *
 *****************************************************************************/

DECLARE_LIST(GenBoundCalc);
typedef enum { GB_Min, GB_Mult, GB_Sum, GB_One, GB_None, GB_Ask } GenBoundTag;

struct GenBoundCalc {
	GenBoundTag tag;
	AbSyn code;
	GenBoundCalcList subs;
};

local GenBoundCalc	gen0MakeBoundCalc	(AbSyn, Bool);
local GenBoundCalc	gen0NewBoundCalc	(GenBoundTag, AbSyn, GenBoundCalcList);
local GenBoundCalc	gen0MakeYieldBound	(AbSyn);
local GenBoundCalc	gen0MakeRepeatBound	(AbSyn, Bool);
local GenBoundCalc	gen0MakeIterBound	(AbSyn, Bool);
local GenBoundCalc	gen0MakeForBound	(AbSyn, Bool);
local GenBoundCalc	gen0MakeSeqBound	(AbSyn, Bool);
local Bool		gen0VerifyExprBoundSafe	(AbSyn, Bool);
local AbSynList		gen0FindIterVars	(AbSyn);

#ifdef GenerBetterGuesses
local Foam		gen0ComputeBoundExpr	(GenBoundCalc, int);
local Foam		gen0ComputeBoundGener	(AbSyn, int);
local Foam		gen0ComputeBCall	(FoamBValTag, FoamList);
local FoamBValTag	gen0CalcToBuiltin	(GenBoundTag);
local Foam		gen0ComputeMin		(FoamList);
#endif

/*  I/face to forIter from bound calc */

struct GenerGenInfo {
	AbSynList geners;
	FoamList  foamVars;
	FoamList  nestChecks;
	AbSynList ids;
};

/* Same order as GenCalcTags */
FoamBValTag gen0CalcOps[] = {
	FOAM_BVal_SIntMin,
	FOAM_BVal_SIntTimes,
	FOAM_BVal_SIntPlus,
	0,
	0,
	0,
};

CREATE_LIST(GenBoundCalc);

local GenBoundCalc
gen0NewBoundCalc(GenBoundTag tag, AbSyn absyn, GenBoundCalcList subs)
{
	GenBoundCalc new;
	new = (GenBoundCalc) stoAlloc(OB_Other, sizeof(*new));
	new->tag      = tag;
	new->code = absyn;
	new->subs = subs;

	return new;
}

#ifdef GenerBetterGuesses
local GenBoundCalc
gen0MakeBoundInit(AbSyn absyn)
{
	GenBoundCalc calc;

	gen0GenInfo = (GenerGenInfo) stoAlloc(OB_Other, sizeof(*gen0GenInfo));
	gen0GenInfo->ids    = listNil(AbSyn);
	gen0GenInfo->geners = listNil(AbSyn);
	gen0GenInfo->nestChecks = listNil(Foam);
	gen0GenInfo->foamVars = listNil(Foam);
	
	calc = gen0MakeBoundCalc(absyn, false);
	
	if (!calc) {
		listFree(AbSyn)(gen0GenInfo->ids);
		listFree(AbSyn)(gen0GenInfo->geners);
		listFree(Foam)(gen0GenInfo->nestChecks);
		listFree(Foam)(gen0GenInfo->foamVars);
		gen0GenInfo->ids    = listNil(AbSyn);
		gen0GenInfo->geners = listNil(AbSyn);
		gen0GenInfo->nestChecks = listNil(Foam);
		gen0GenInfo->foamVars = listNil(Foam);
	}
	return calc;
}

local Foam 
gen0ComputeBoundFunction(GenBoundCalc calc)
{	
	GenFoamState saved;
	Foam clos, foam, ret;
	int exitLabel, retLabel;

	clos = foamNewClos(foamNewEnv(-1), foamNewConst(gen0NumProgs));
	foam = gen0ProgInitEmpty(boundFName(), NULL);
	saved = gen0ProgSaveState(PT_Std);

	exitLabel = gen0State->labelNo++;
	retLabel  = gen0State->labelNo++;

	ret = gen0TempLocal(FOAM_SInt);
	gen0AddStmt(foamNewSet(ret, gen0ComputeBoundExpr(calc, exitLabel)), NULL);
	gen0AddStmt(foamNewGoto(retLabel), NULL);
	gen0AddStmt(foamNewLabel(exitLabel), NULL);
	gen0AddStmt(foamNewSet(foamCopy(ret), foamNewSInt(-1)), NULL);
	gen0AddStmt(foamNewLabel(retLabel), NULL);
	gen0AddStmt(foamNewReturn(foamCopy(ret)), NULL);

	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, true);

	foam->foamProg.infoBits = IB_SIDE | IB_INLINEME;

	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, FOAM_SInt, int0);
	gen0AddLexLevels(foam, 2);

	gen0ProgRestoreState(saved);
	return clos;
}
#endif

Foam
gen0GenLiftedGener(AbSyn forIter, AbSyn gener)
{
#ifndef  GenerBetterGuesses
	return genImplicit(forIter, gener, FOAM_Clos);
#else
	Foam	check, vars, call;	
	int	place, i;

	if (gen0GenInfo)
		place = listPosq(AbSyn)(gen0GenInfo->geners, forIter);
	else
		place = -1;

	if (place < 0) {
		Foam vals = gen0TempFrDDecl(gen0MakeGenerRetFormat(), true);
		Foam doneFun = gen0Temp(FOAM_Clos);
		Foam stepFun = gen0Temp(FOAM_Clos);
		Foam valueFun = gen0Temp(FOAM_Clos);
		Foam boundFun = gen0Temp(FOAM_Clos);

		call = foamNewEmpty(FOAM_CCall, 2);
		call->foamCCall.type = FOAM_NOp;
		call->foamCCall.op = genImplicit(forIter, gener, FOAM_Clos);
		call = foamNewMFmt(gen0MakeGenerRetFormat(), call);
		gen0AddStmt(foamNewDef(foamCopy(vals), call), forIter);
		gen0AddStmt(foamNewDef(doneFun, vals->foamValues.argv[0]),
			    	       forIter);
		gen0AddStmt(foamNewDef(stepFun, vals->foamValues.argv[1]), 
			    forIter);
		gen0AddStmt(foamNewDef(valueFun, vals->foamValues.argv[2]), 
			    forIter);
		gen0AddStmt(foamNewDef(boundFun, vals->foamValues.argv[3]), 
			    forIter);
		return foamNew(FOAM_Values, 4, doneFun, stepFun,
			       valueFun, boundFun);
	}
	check = listElt(Foam)(gen0GenInfo->nestChecks, place);
	vars   = listElt(Foam)(gen0GenInfo->foamVars, place);
	if (check) {
		Foam vals = gen0TempFrDDecl(gen0MakeGenerRetFormat(), true);

		call = foamNewEmpty(FOAM_CCall, 2);
		call->foamCCall.type = FOAM_NOp;
		call->foamCCall.op = genImplicit(forIter, 
						 forIter->abFor.whole, 
						 FOAM_Clos);
		gen0AddStmt(foamNewDef(foamCopy(vals),
				       foamNewMFmt(gen0MakeGenerRetFormat(),
						   call)), forIter);
		gen0AddStmt(foamNewDef(foamCopy(check), foamNewBool(true)),
			    forIter);
		for (i=0; i < 4; i++)
			gen0AddStmt(foamNewDef(foamCopy(vars
							->foamValues.argv[i]),
				    	       foamCopy(vals
							->foamValues.argv[i])),
			 	    forIter);
		return foamCopy(vars);
	}
	return foamCopy(vars);
#endif
}

/* Internal bound calculation routines */
local GenBoundCalc
gen0MakeBoundCalc(AbSyn absyn, Bool nestCheck)
{
	switch (abTag(absyn)) {
	  case AB_Yield:
		return gen0MakeYieldBound(absyn);
	  case AB_Sequence:
		return gen0MakeSeqBound(absyn, nestCheck);
	  case AB_Repeat:
		return gen0MakeRepeatBound(absyn, nestCheck);
	  /* Nasty problems with side-effects if we allow apply, set, etc. */
	  default:
		return NULL;
	}
}

local GenBoundCalc
gen0MakeYieldBound(AbSyn absyn)
{
	if (gen0VerifyExprBoundSafe(absyn->abYield.value, false))
		return gen0NewBoundCalc(GB_One, NULL, NULL);
	else
		return NULL;
}

local GenBoundCalc
gen0MakeRepeatBound(AbSyn absyn, Bool nestCheck)
{
	GenBoundCalc     inner, loop;
	GenBoundCalcList lst;
	
	if (nestCheck) return NULL;
	/* calcs ids too.. */
	loop = gen0MakeIterBound(absyn, nestCheck);
	if (!loop) 
		return NULL;
	inner = gen0MakeBoundCalc(absyn->abRepeat.body, true);
	if (!inner)
		return NULL;
	lst = listCons(GenBoundCalc)(loop, 
			listCons(GenBoundCalc)(inner, listNil(GenBoundCalc)));
	return gen0NewBoundCalc(GB_Mult, NULL, lst);
				
}

local GenBoundCalc
gen0MakeIterBound(AbSyn ab, Bool nestCheck)
{
	GenBoundCalcList lst = listNil(GenBoundCalc);
	GenBoundCalc     new;
	AbSyn 		 iter;
	int		i;
	String		msg;
	
	if (abRepeatIterc(ab) == 0) {
		return NULL; 
	}

	if (abRepeatIterc(ab) == 1) {
		iter = ab->abRepeat.iterv[0];
	 	switch (abTag(iter)) {
		  case AB_While:
			new = NULL;
			break;
		  case AB_For:
			new = gen0MakeForBound(iter, nestCheck);
			break;
		  default:
			msg = "bad iterator passed to gen0MakeIterBound";
			comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
			bug("not an iterate");
			new = NULL;
#endif
			NotReached(new = NULL);
		}
		return new;			
	}
	for (i=0; i<abRepeatIterc(ab); i++) {
		iter = ab->abRepeat.iterv[i];
		switch(abTag(iter)) {
		  case AB_For:
			new = gen0MakeForBound(iter, nestCheck);
			if (!new) {
				listFree(GenBoundCalc)(lst);
				return NULL;
			}
			lst = listCons(GenBoundCalc)(new, lst);
			break;
		  case AB_While:
			if (!gen0VerifyExprBoundSafe(iter->abWhile.test, false))
				return NULL;
			break;
		  default: 
			msg = "bad iterator passed to gen0MakeIterBound";
			comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
			bug("not an iterator");
			return NULL;
#endif
		}
	}
	return gen0NewBoundCalc(GB_Min, NULL, lst);
}

local GenBoundCalc
gen0MakeForBound(AbSyn absyn, Bool nestCheck)
{
	if (!gen0VerifyExprBoundSafe(absyn->abFor.lhs, true) ||
	    !gen0VerifyExprBoundSafe(absyn->abFor.test, false))    
		return NULL;

	gen0GenInfo->ids    = listNConcat(AbSyn)(gen0FindIterVars(absyn->abFor.lhs),
						 gen0GenInfo->ids);
	gen0GenInfo->geners = listCons(AbSyn)(absyn, gen0GenInfo->geners);
	/* should check nestCheck if we want to un-nest gtor */
	gen0GenInfo->nestChecks = listCons(Foam)(gen0TempLex(FOAM_Bool),
					 	 gen0GenInfo->nestChecks);
	return gen0NewBoundCalc(GB_Ask, absyn, NULL);
}

local AbSynList
gen0FindIterVars(AbSyn ab)
{
	AbSynList	lst = listNil(AbSyn);
	int		i;
	String		msg;

	switch (abTag(ab)) {
	case AB_Id:
		lst = listCons(AbSyn)(ab, listNil(AbSyn));
		break;
	case AB_Declare:
		lst = gen0FindIterVars(ab->abDeclare.id);
		break;
        case AB_Free:
	case AB_Fluid:
		lst = listNil(AbSyn);
		for (i=0; i < abArgc(ab); i++)
			lst = listNConcat(AbSyn)(gen0FindIterVars(abArgv(ab)[i]), lst);
		break;
	case AB_Comma:
		lst = listNil(AbSyn);
		for (i=0; i < abArgc(ab); i++)
			lst = listNConcat(AbSyn)(gen0FindIterVars(abArgv(ab)[i]), lst);
		break;
	default:
		msg = "bad iterator passed to gen0FindIterVars";
		comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
		abPrint(dbOut, ab);
		bug("unhandled iteration form");
		NotReached(lst = listNil(AbSyn));
		break;
#endif
	}
	return lst;
}

local GenBoundCalc
gen0MakeSeqBound(AbSyn absyn, Bool nestCheck)
{
	GenBoundCalcList lst = listNil(GenBoundCalc);
	GenBoundCalc     val;
	int 	i;

	if (abArgc(absyn) == 0)
		return gen0NewBoundCalc(GB_None, NULL, NULL);
	for (i=0; i < abArgc(absyn); i++) {
		val = gen0MakeBoundCalc(absyn->abSequence.argv[i], nestCheck);
		if (!val) {
			listFree(GenBoundCalc)(lst);
			return NULL;
		}
		
		lst = listCons(GenBoundCalc)(val, lst);
	}
	
	if (!lst) {
		return NULL;
	}
	return gen0NewBoundCalc(GB_Sum, NULL, lst);
}

local Bool
gen0VerifyExprBoundSafe(AbSyn ab, Bool iterIds)
{
	int i;
	switch(abTag(ab)) {
	  case AB_Id:
		if (iterIds && listMember(AbSyn)(gen0GenInfo->ids, ab, abEqual)) {
			return false;
		}
		return true;
	  case AB_Yield:
	  case AB_Goto:
	  case AB_Sequence:
		return false;
	  case AB_LitInteger:
	  case AB_LitFloat:
	  case AB_LitString:
		return true;
	  default: 
		for (i=0; i<abArgc(ab); i++)
			if (!gen0VerifyExprBoundSafe(abArgv(ab)[i], iterIds)) {
				return false;
			}
		return true;
	}
}

#ifdef GenerBetterGuesses

/* Added at generator base */
local void 
gen0ComputeGeners()
{
	AbSynList idlst, glst;
	FoamList  foamVars, nestlst;
	Foam      tmpVar, genVals;

	Foam vals = gen0TempFrDDecl(gen0MakeGenerRetFormat(), true);
	int i;

	idlst   = listNReverse(AbSyn)(gen0GenInfo->ids);
	glst    = listNReverse(AbSyn)(gen0GenInfo->geners);
	nestlst = listNReverse(Foam)(gen0GenInfo->nestChecks);
	gen0GenInfo->geners = glst;
	gen0GenInfo->ids    = idlst;
	gen0GenInfo->nestChecks = nestlst;

	foamVars = listNil(Foam);
	while (glst) {
		Foam doneFun = gen0TempLex(FOAM_Clos);
		Foam stepFun = gen0TempLex(FOAM_Clos);
		Foam valueFun = gen0TempLex(FOAM_Clos);
		Foam boundFun = gen0TempLex(FOAM_Clos);
		FoamTag type;
		AInt    fmt;

		type   = gen0Type(gen0AbType(car(idlst)), &fmt);
		tmpVar = gen0TempLocal0(type, fmt);
		foamVars = listCons(Foam)(foamNew(FOAM_Values, 4, doneFun, stepFun, 
						  valueFun, boundFun), foamVars);
		if (car(nestlst)) 
			gen0AddStmt(foamNewSet(car(nestlst), foamNewBool(false)), NULL);
		else {
			genVals = genImplicit(car(glst), car(glst)->abFor.whole, FOAM_Clos);
			gen0AddStmt(foamNewSet(tmpVar, genVals), NULL);
			genVals = foamNewMFmt(gen0MakeGenerRetFormat(), 
					      foamNew(FOAM_CCall, 2, 
						      FOAM_NOp, foamCopy(tmpVar)));
			gen0AddStmt(foamNewSet(foamCopy(vals), genVals), NULL);
			for (i=0; i<foamArgc(vals); i++)
				gen0AddStmt(foamNewSet(foamCopy(car(foamVars)->foamValues.argv[i]),
						       foamCopy(vals->foamValues.argv[i])), NULL);
		}
		idlst = cdr(idlst);
		glst  = cdr(glst);
		nestlst = cdr(nestlst);
	}
	foamFree(vals);
	gen0GenInfo->foamVars = listNReverse(Foam)(foamVars);
}

local Foam
gen0ComputeBoundExpr(GenBoundCalc calc, int exitLabel)
{
	FoamList lst;
	Foam val;
	GenBoundCalcList clst;

	switch(calc->tag) {
	  case GB_One:
		val = foamNewSInt(1);
		break;
	  case GB_None:
		val = foamNewSInt(int0);
		break;
	  case GB_Ask: /* Look in list for foam */
		val = gen0ComputeBoundGener(calc->code, exitLabel);
		break;
	  default:
		lst = listNil(Foam);
		clst = calc->subs;
		while (clst) {
			lst = listCons(Foam)
				      (gen0ComputeBoundExpr(car(clst), exitLabel),
				       lst);				
			clst = cdr(clst);
		}
		if (calc->tag == GB_Min)
			val = gen0ComputeMin(lst);
		else
			val = gen0ComputeBCall(gen0CalcToBuiltin(calc->tag), lst);
	}
	return val;
}

local Foam 
gen0ComputeBoundGener(AbSyn code, int exitLabel)
{
	Foam tmp = gen0TempLocal(FOAM_SInt);
	Foam vals, nestCheck, call;
	int place;

	place = listPosq(AbSyn)(gen0GenInfo->geners, code);
	vals      = listElt(Foam)(gen0GenInfo->foamVars, place);
	nestCheck = listElt(Foam)(gen0GenInfo->nestChecks, place);

	if (nestCheck) {
		gen0AddStmt(foamNewIf(foamNotThis(foamCopy(nestCheck)),
				      exitLabel), NULL);
	}
	call = foamNewEmpty(FOAM_CCall, 2);
	call->foamCCall.type = FOAM_SInt;
	call->foamCCall.op = foamCopy(vals->foamValues.argv[boundPlace]);

	gen0AddStmt(foamNewSet(tmp, call), NULL);
	gen0AddStmt(foamNewIf(foamNew(FOAM_BCall, 3, (AInt)FOAM_BVal_SIntLT, 
				      foamCopy(tmp), foamNewSInt(int0)), 
			      exitLabel), NULL);
	gen0UseStackedFormat(int0);
	return foamCopy(tmp);
}

local Foam
gen0ComputeMin(FoamList lst)
{
	Foam acc = gen0TempLocal(FOAM_SInt);

	gen0AddStmt(foamNewSet(acc, car(lst)), NULL);
	lst = cdr(lst);

	while (lst) {
		int label = gen0State->labelNo++;
		gen0AddStmt(foamNewIf(foamNew(FOAM_BCall, 3, FOAM_BVal_SIntLT, 
					      foamCopy(acc), car(lst)), label),
			    NULL);
		gen0AddStmt(foamNewSet(foamCopy(acc), foamCopy(car(lst))), NULL);
		gen0AddStmt(foamNewLabel(label), NULL);
		lst = cdr(lst);
	}
	return foamCopy(acc);
}

local Foam
gen0ComputeBCall(FoamBValTag fn, FoamList lst)
{
	Foam acc;
	if (cdr(lst)==0) return car(lst);
	acc = foamNew(FOAM_BCall, 3, fn, car(lst), gen0ComputeBCall(fn, cdr(lst)));
	return acc;
}

local FoamBValTag
gen0CalcToBuiltin(GenBoundTag tag)
{
	assert(gen0CalcOps[tag]);
	return gen0CalcOps[tag];
}

#endif
