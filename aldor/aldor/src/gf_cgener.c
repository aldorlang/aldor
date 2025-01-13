/*****************************************************************************
 *
 * gf_cgener.c: Foam code generation for generators (new style).
 *
 ****************************************************************************/

#include "comsg.h"
#include "comsgdb.h"
#include "fluid.h"
#include "format.h"
#include "fbox.h"
#include "gf_prog.h"
#include "gf_cgener.h"
#include "optinfo.h"
#include "tform.h"
#include "util.h"

local Foam   gen0CGenCoroutine		(AbSyn gen);
local String gen0CGenCoroutineFnName	(void);

/*
#define 	gen0XRetFmtSize 2
static AInt     gen0XGenRetFmt;
static String 	gen0XRetFmtNames[] = { "step!", "bound" };
static FoamTag	gen0XRetFmtTypes[] = { FOAM_Clos, FOAM_Clos };
static AInt	gen0XRetFmtFmts[]  = { emptyFormatSlot, emptyFormatSlot };
*/
#define gen0XGenStepFnFmtSize 2
static AInt    gen0XGenStepFnFmt;
static String  gen0XGenStepFnFmtNames[] = { "hasNext?", "next" };
static FoamTag gen0XGenStepFnFmtTypes[] = { FOAM_Bool, FOAM_Word };
static AInt    gen0XGenStepFnFmtFmts[]  = { emptyFormatSlot, emptyFormatSlot };

#define coroutineName()	("coroutine")
#define boundFName()	("generBound")

Foam
gen0CGenerate(AbSyn gen)
{
	Scope("gen0XGenerate");
	FoamList topLines;
	Foam cgener;
	Foam val;
	Bool flag;

	foamProgUnsetLeaf(gen0State->program);

	flag = gen0AddImportPlace(&topLines);
	gen0UseStackedFormat(int0);

	cgener = gen0CGenCoroutine(gen);

	if (flag) gen0ResetImportPlace(topLines);

	Return(foamNewCast(FOAM_Word, cgener));
}

Foam
gen0CYield(AbSyn absyn)
{
	FoamTag tag;
	Foam value;
	value = genFoamVal(absyn->abYield.value);
	tag = gen0Type(gen0AbContextType(absyn->abYield.value), NULL);
	gen0AddStmt(foamNewYield(foamCastIfNeeded(FOAM_Word, tag, value)), absyn);

        return 0;
}

local Foam
gen0CGenCoroutine(AbSyn gen)
{
	AbSyn           body = gen->abGenerate.body;
	Foam		foam, gener;
	AInt		fmtSlot, index;

        gener = foamNewGener(int0, gen0ProgEnv0(), foamNewConst(gen0NumProgs));

        foam = gen0ProgInitEmpty(gen0CGenCoroutineFnName(), body);

	index = gen0FormatNum;
	gen0ProgPushState(abStab(gen), GF_Lambda);

	gen0State->type = tfNone();
	gen0State->params = fboxNew(foamNewEmptyDDecl(FOAM_DDecl_Param));
	gen0State->program = foam;
	gen0PushFormat(index);

	gen0Vars(abStab(gen));

        genFoamStmt(body);
	gen0AddStmt(foamNewReturn(foamNew(FOAM_Values, int0)), NULL);

	gen0ProgAddStateFormat(index);
	gen0ProgFiniEmpty(foam, FOAM_NOp, int0);

        foam->foamProg.infoBits = IB_SIDE | IB_INLINEME | IB_COROUTINE;
        foamOptInfo(foam) = optInfoNew(gen0State->stab, foam, NULL, false);

	assert(foamProgIsCoroutine(foam));

	gen0ComputeSideEffects(foam);
	gen0ProgPopState();
	return gener;
}

Foam
gen0CGenCall(AbSyn forIter)
{
	Foam gen, whole, ret, stepFn;
	AInt fmt;

	gen = genImplicit(forIter, forIter->abFor.whole, FOAM_Clos);
	stepFn = gen0Temp(FOAM_Gener);

	gen0AddStmt(foamNewDef(foamCopy(stepFn), gen), forIter);

	return stepFn;
}

local String
gen0CGenCoroutineFnName()
{
	return aStrPrintf("%s_%s", gen0ProgName, coroutineName());
}
