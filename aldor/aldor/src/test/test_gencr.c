#include "axlobs.h"
#include "gencr.h"
#include "yldlocs.h"
#include "optinfo.h"
#include "strops.h"
#include "testlib.h"

local void testGenCR_Simple(void);
local void testGenCR_OneVarOut(void);
local void testGenCR_OneVarIn(void);
local void testGenCR_OneVarOutOneVarLocal(void);
local void testGenCR_OneVarIn2(void);
local void testGenCR_UseInYield(void);
local void testGenCR_UseOverLabel(void);
local void testGenCR_SimpleUnit(void);
local void testGenCR_OneLocalUnit(void);
local void testGenCR_OneLocalMultiSetUnit(void);
local void testGenCR_DefAndYield(void);

extern Bool ylDebug;

void
gencrTest()
{
	init();
	ylDebug = true;

	TEST(testGenCR_Simple);
	TEST(testGenCR_OneVarOut);
	TEST(testGenCR_OneVarIn);
	TEST(testGenCR_OneVarOutOneVarLocal);
	TEST(testGenCR_OneVarIn2);
	TEST(testGenCR_UseInYield);
	TEST(testGenCR_UseOverLabel);
	TEST(testGenCR_SimpleUnit);
	TEST(testGenCR_OneLocalUnit);
	TEST(testGenCR_OneLocalMultiSetUnit);
	TEST(testGenCR_DefAndYield);

	ylDebug = false;
}

local void
testGenCR_Simple()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 0;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
					     NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewYield(foamNewSInt(22)),
			   foamNewReturn(foamNewNil()),
			   NULL);

	YldLocResult res = ylProg(coroutine);
	testIsNull("", res->locs);
	testIsNull("", res->reclocs);
	ylLocResultFree(res);

	ylProg(coroutine);
}

local void
testGenCR_OneVarOut()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 0;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("foo"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(1)),
			   foamNewYield(foamNewSInt(22)),
			   foamNewCCall(FOAM_Word, foamNewGlo(int0), foamNewLoc(0), NULL),
			   NULL);

	YldLocResult res = ylProg(coroutine);
	testIsNull("", res->locs);
	testIntEqual("", 1, listLength(AInt)(res->reclocs));
	testIntEqual("", 0, car(res->reclocs));
	ylLocResultFree(res);
}

local void
testGenCR_OneVarIn()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 0;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("foo"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(1)),
			   foamNewReturn(foamNewLoc(0)),
			   NULL);

	YldLocResult res = ylProg(coroutine);
	testIntEqual("", 1, listLength(AInt)(res->locs));
	testIntEqual("", 0, car(res->locs));
	testIsNull("", res->reclocs);
	ylLocResultFree(res);
}

local void
testGenCR_OneVarOutOneVarLocal()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 0;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("ref"), 0),
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewSet(foamNewLoc(1), foamNewSInt(1)),
			   foamNewCCall(FOAM_Word, foamNewGlo(int0), foamNewLoc(1), NULL),
			   foamNewYield(foamNewSInt(22)),
			   foamNewCCall(FOAM_Word, foamNewGlo(int0), foamNewLoc(0), NULL),
			   NULL);
	YldLocResult res = ylProg(coroutine);
	testIntEqual("", 1, listLength(AInt)(res->reclocs));
	testIntEqual("", 0, car(res->reclocs));
	testIntEqual("", 1, listLength(AInt)(res->locs));
	testIntEqual("", 1, car(res->locs));
	ylLocResultFree(res);
}

local void
testGenCR_OneVarIn2()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 0;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewCCall(FOAM_Word, foamNewGlo(int0), foamNewLoc(0), NULL),
			   foamNewYield(foamNewSInt(22)),
			   foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewCCall(FOAM_Word, foamNewGlo(int0), foamNewLoc(0), NULL),
			   NULL);
	YldLocResult res = ylProg(coroutine);
	testIntEqual("", 0, listLength(AInt)(res->reclocs));
	testIntEqual("", 1, listLength(AInt)(res->locs));
	ylLocResultFree(res);
}


local void
testGenCR_UseInYield()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 0;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewYield(foamNewLoc(0)),
			   foamNewReturn(foamNewNil()),
			   NULL);
	YldLocResult res = ylProg(coroutine);
	testIntEqual("", 0, listLength(AInt)(res->reclocs));
	testIntEqual("", 1, listLength(AInt)(res->locs));
	ylLocResultFree(res);
}

local void
testGenCR_UseOverLabel()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 1;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewIf(foamNewCCall(FOAM_Bool, foamNewGlo(1)), 0),
			   foamNewYield(foamNewLoc(0)),
			   foamNewLabel(0),
			   foamNewReturn(foamNewLoc(0)),
			   NULL);

	YldLocResult res = ylProg(coroutine);
	testIntEqual("", 1, listLength(AInt)(res->reclocs));
	testIntEqual("", 0, listLength(AInt)(res->locs));
	ylLocResultFree(res);
}

local void
testGenCR_DefAndYield()
{
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 1;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  foamNewDecl(FOAM_SInt, strCopy("rec"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewSet(foamNewLoc(1), foamNewSInt(1)),
			   foamNewYield(foamNewLoc(0)),
			   foamNewGoto(0),
			   foamNewLabel(0),
			   foamNewCCall(FOAM_Word, foamNewGlo(int0), foamNewLoc(1), NULL),
			   foamNewReturn(foamNewNil()),
			   NULL);

	YldLocResult res = ylProg(coroutine);
	testIntEqual("", 1, listLength(AInt)(res->reclocs));
	testIntEqual("", 1, listLength(AInt)(res->locs));
	ylLocResultFree(res);
}

local void
testGenCR_SimpleUnit()
{
	Foam unit, newUnit;
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 1;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewIf(foamNewCCall(FOAM_Bool, foamNewGlo(0)), 0),
			   foamNewYield(foamNewLoc(0)),
			   foamNewLabel(0),
			   foamNewReturn(foamNewLoc(0)),
			   NULL);

	unit = foamNewUnit(foamNewDFmt(foamNewDDecl(FOAM_DDecl_Global,
						    foamNewDecl(FOAM_Clos, strCopy("fn0"), -1),
						    NULL),
				       foamNewDDecl(FOAM_DDecl_Consts,
						    foamNewDecl(FOAM_Prog, strCopy("cr0"), -1),
						    NULL),
				       foamNewDDeclEmpty(0, FOAM_DDecl_LocalEnv),
				       foamNewDDeclEmpty(0, FOAM_DDecl_Fluid),
				       NULL),
			   foamNewDDef(foamNewDef(foamNewConst(0),
						  coroutine),
				       NULL));

	testTrue("isCoroutine", foamUnitHasCoroutine(unit));
	newUnit = gcrRewriteUnit(unit);
	foamAudit(newUnit);
	foamPrintDb(newUnit);
}



local void
testGenCR_OneLocalUnit()
{
	Foam unit, newUnit;
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 1;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("rec"), 0),
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewLoc(0), foamNewSInt(0)),
			   foamNewSet(foamNewLoc(1), foamNewSInt(1)),
			   foamNewIf(foamNewCCall(FOAM_Bool, foamNewGlo(0), NULL), 0),
			   foamNewYield(foamNewLoc(1)),
			   foamNewLabel(0),
			   foamNewCCall(FOAM_Bool, foamNewGlo(0), foamNewLoc(0), NULL, 0),
			   foamNewReturn(foamNewNil()),
			   NULL);

	unit = foamNewUnit(foamNewDFmt(foamNewDDecl(FOAM_DDecl_Global,
						    foamNewDecl(FOAM_Clos, strCopy("fn0"), -1),
						    NULL),
				       foamNewDDecl(FOAM_DDecl_Consts,
						    foamNewDecl(FOAM_Prog, strCopy("cr0"), -1),
						    NULL),
				       foamNewDDeclEmpty(0, FOAM_DDecl_LocalEnv),
				       foamNewDDeclEmpty(0, FOAM_DDecl_Fluid),
				       NULL),
			   foamNewDDef(foamNewDef(foamNewConst(0),
						  coroutine),
				       NULL));

	testTrue("isCoroutine", foamUnitHasCoroutine(unit));
	newUnit = gcrRewriteUnit(unit);
	foamAudit(newUnit);
	Foam newCoroutine = newUnit->foamUnit.defs->foamDDef.argv[0]->foamDef.rhs->foamProg.locals;
	testAIntEqual("locs", 1, foamDDeclArgc(newCoroutine));
	foamPrintDb(newUnit);
}

local void
testGenCR_OneLocalMultiSetUnit()
{
	Foam unit, newUnit;
	Foam coroutine = foamNewProgEmpty();

	// Create a coroutine
	foamProgSetCoroutine(coroutine);
	coroutine->foamProg.nLabels = 1;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
						  foamNewDecl(FOAM_SInt, strCopy("rec"), 0),
						  foamNewDecl(FOAM_SInt, strCopy("local"), 0),
						  NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body =
		foamNewSeq(foamNewSet(foamNewValues(foamNewLoc(0), foamNewLoc(1), NULL),
				      foamNewValues(foamNewSInt(0), foamNewSInt(1), NULL)),
			   foamNewIf(foamNewCCall(FOAM_Bool, foamNewGlo(0), NULL), 0),
			   foamNewYield(foamNewLoc(1)),
			   foamNewLabel(0),
			   foamNewCCall(FOAM_Bool, foamNewGlo(0), foamNewLoc(0), NULL, 0),
			   foamNewReturn(foamNewNil()),
			   NULL);

	unit = foamNewUnit(foamNewDFmt(foamNewDDecl(FOAM_DDecl_Global,
						    foamNewDecl(FOAM_Clos, strCopy("fn0"), -1),
						    NULL),
				       foamNewDDecl(FOAM_DDecl_Consts,
						    foamNewDecl(FOAM_Prog, strCopy("cr0"), -1),
						    NULL),
				       foamNewDDeclEmpty(0, FOAM_DDecl_LocalEnv),
				       foamNewDDeclEmpty(0, FOAM_DDecl_Fluid),
				       NULL),
			   foamNewDDef(foamNewDef(foamNewConst(0),
						  coroutine),
				       NULL));

	testTrue("isCoroutine", foamUnitHasCoroutine(unit));
	newUnit = gcrRewriteUnit(unit);
	foamAudit(newUnit);
	Foam newCoroutine = newUnit->foamUnit.defs->foamDDef.argv[0]->foamDef.rhs;
	Foam newLocals    = newCoroutine->foamProg.locals;

	testAIntEqual("locs", 2, foamDDeclArgc(newLocals));
	testStringEqual("locs", "rec_tmp", newLocals->foamDDecl.argv[1]->foamDecl.id);
	foamPrintDb(newUnit);
}

