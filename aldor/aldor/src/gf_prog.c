/*****************************************************************************
 *
 * gf_prog.c: Common declarations and macros for foam prog generation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "gf_prog.h"
#include "gf_util.h"
#include "of_inlin.h"
#include "store.h"
#include "syme.h"
#include "strops.h"
#include "fbox.h"
#include "tform.h"

Bool	genfEnvDebug	= false;

/*****************************************************************************
 *
 * :: Local function declarations.
 *
 ****************************************************************************/
local void	gen0ProgTransferState(GenFoamState, GenFoamState);

local Foam		gen0IssueStmts		(void);
#ifdef NEW_FORMATS
local AInt		gen0IssueParams		(void);
#else
local Foam		gen0IssueParams		(void);
#endif
local Foam		gen0IssueLocals		(void);
local Foam		gen0IssueFluids		(void);
local Foam		gen0IssueLevels		(void);

/*****************************************************************************
 *
 * :: Foam prog construction operations.
 *
 ****************************************************************************/

/*
 * Build a foam prog from a piece of abstract syntax.
 */
Foam 
gen0BuildFunction(ProgType pt, String name, AbSyn expr)
{
	GenFoamState	saved;
	Foam		foam, clos, ret;
	AInt 		fmt;
	FoamTag         tag;
	extern TForm	gen0AbContextType(AbSyn);

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(name, expr);

	saved = gen0ProgSaveState(pt);

	if (tfIsNone(gen0AbContextType(expr))) {
		genFoamStmt(expr);
		gen0AddStmt(foamNewReturn(foamNew(FOAM_Values, (Length) 0)), expr);
	}
	else {
		ret = genFoamValAs(gen0AbContextType(expr), expr);
		if (ret) gen0AddStmt(foamNewReturn(ret), expr);
	}
	gen0ProgPushFormat(emptyFormatSlot);
	gen0IssueDCache();

	tag = gen0Type(gen0AbContextType(expr), &fmt);

	gen0ProgFiniEmpty(foam, tag, fmt);

	gen0AddLexLevels(foam, 1);

        foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	gen0ProgRestoreState(saved);
	return clos;
}

/*
 * Push a new prog state.
 */

void
gen0ProgPushState(Stab stab, GenFoamTag tag)
{
	AInt		index = gen0FormatNum--;
	GenFoamState	state = gen0NewState(stab, index, tag);
	SlotUsageList   fu;
	AIntList	fs, ef;
	Length		i, argc;

	assert(state && state->parent);
	fu = state->parent->formatUsage;
	fs = state->parent->formatStack;
	ef = state->parent->envFormatStack;

	argc = listLength(AInt)(ef);
	for (i = 0; i < argc - 1; i += 1)
		fu = listCons(SlotUsage)(suFrFormat(emptyFormatSlot), fu);

	if (ef) ef = listNReverse(AInt)(cdr(listReverse(AInt)(ef)));
	fs = listNConcat(AInt)(ef, fs);

	state->formatUsage = fu;
	state->formatStack = fs;

	gen0State = state;
}

/*
 * Pop a prog state.
 */

void
gen0ProgPopState()
{
	GenFoamState	state = gen0State;
	SlotUsageList fu, ofu;
	int i;
	gen0State = gen0State->parent;
	
	if (genfEnvDebug) {
		afprintf(dbOut, "Pop state - pre formatUsage parent %pSlotUsageList\n", state->parent->formatUsage);
		afprintf(dbOut, "Pop state - pre formatUsage current %pSlotUsageList\n", state->formatUsage);
	}

	ofu = state->parent->formatUsage;
	fu  = cdr(state->formatUsage);
	/* Copy usage information downwards */
	for (i=0; i<state->parent->whereNest; i++)
		fu = cdr(fu);
	while (fu != listNil(SlotUsage)) {
		if (suVal(car(ofu)) == emptyFormatSlot)
			car(ofu) = car(fu);
		ofu = cdr(ofu);
		fu  = cdr(fu);
	}
	vpFree(state->localPool);
	vpFree(state->lexPool);
	listFree(Foam)(state->inits);
	listFree(Foam)(state->lines);
	listFree(Syme)(state->funImportList);
	listFree(TForm)(state->domImportList);
	listFree(Foam)(state->domList);

	if (genfEnvDebug) {
		afprintf(dbOut, "Pop state - formatUsage %pAIntList\n", state->formatUsage);
		afprintf(dbOut, "Pop state - formatUsage %pAIntList\n", state->parent->formatUsage);
	}
	stoFree(state);
}

/*
 * Save the state for a prog which doesn't need its own.
 */

GenFoamState
gen0ProgSaveState(ProgType pt)
{
	GenFoamState	state = gen0State;
	GenFoamState	saved = gen0NewState(NULL, emptyFormatSlot, GF_Saved);

	if (genfEnvDebug) {
		afprintf(dbOut, "Save state %pSlotUsageList\n", state->formatUsage);
	}
	gen0ProgTransferState(saved, state);

	if (!gen0InDeep(state->progType))
		state->base = saved;

	state->formatUsage	= state->formatUsage;
	state->params		= fboxNew(foamNewEmptyDDecl(FOAM_DDecl_Param));
	state->localPool	= vpNew(fboxNew(foamNewEmptyDDecl(FOAM_DDecl_Local)));
	state->fluidsUsed	= listNil(AInt);
	state->labelNo		= 0;
	state->yieldCount	= 0;
	state->yieldLabels	= listNil(AInt);
	state->lines		= listNil(Foam);
	state->inits		= listNil(Foam);
	state->progType		= pt;
	state->domCache		= NULL;
	state->dbgContext	= (Foam)NULL;

	return saved;
}

void
gen0ProgPushFormat(AInt index)
{
	GenFoamState	state = gen0State;
	SlotUsageList   fu;
	AIntList	ef;

	fu = state->formatUsage;
	ef = state->envFormatStack;

	assert(fu);
	if (ef && cdr(ef)) {
		//fu = listConcat(SlotUsage)((SlotUsageList) ef, cdr(fu));
		fu = cdr(fu);
		AIntList l2 = listReverse(AInt)(ef);
		while (l2 != listNil(AInt)) {
			fu = listCons(SlotUsage)(suFrFormat(car(l2)), fu);
			l2 = listFreeCons(AInt)(l2);
		}
	}
	fu = listCons(SlotUsage)(suFrFormat(index), fu);

	state->formatUsage	= fu;
	state->envFormatStack	= listNil(AInt);
}

/*
 * Restore the state for a prog which doesn't need its own.
 */

void
gen0ProgRestoreState(GenFoamState saved)
{
	GenFoamState	state = gen0State;

	gen0ProgTransferState(state, saved);

	if (!gen0InDeep(gen0State->progType))
		state->base = NULL;

	if (genfEnvDebug) {
		afprintf(dbOut, "Restore state %pSlotUsageList\n", state->formatUsage);
	}
	stoFree(saved);
}

void
gen0ProgUseBaseState()
{
	struct gfs tmp;
	gen0ProgTransferState(&tmp, gen0State);
	gen0ProgTransferState(gen0State, gen0State->base);
	gen0ProgTransferState(gen0State->base, &tmp);
}

void 
gen0ProgUseUpperState()
{
	struct gfs tmp;
	gen0ProgTransferState(&tmp, gen0State);
	gen0ProgTransferState(gen0State, gen0State->base);
	gen0ProgTransferState(gen0State->base, &tmp);
}

local void 
gen0ProgTransferState(GenFoamState to, GenFoamState from)
{
	to->formatUsage	  = from->formatUsage;
	to->formatStack	  = from->formatStack;
	to->envFormatStack= from->envFormatStack;
	to->params	  = from->params;
	to->localPool	  = from->localPool;
	to->fluidsUsed	  = from->fluidsUsed;
	to->labelNo	  = from->labelNo;
	to->yieldCount	  = from->yieldCount;
	to->yieldLabels	  = from->yieldLabels;
	to->yieldValueVar = from->yieldValueVar;
	to->lines	  = from->lines;
	to->inits	  = from->inits;
	to->progType	  = from->progType;
	to->domCache      = from->domCache;
	to->dbgContext    = from->dbgContext;
}


/*
 * Create an empty prog.
 */
Foam
gen0ProgInitEmpty(String name, AbSyn absyn)
{
	Foam	foam, decl;
	int 	idx;

	if (genfEnvDebug) {
		idx = gen0NumProgs;
		afprintf(dbOut, "(Creating function %d\n", idx);
	}
#ifdef NEW_FORMATS
	foam = foamNewProg(int0,int0,int0,int0,int0,emptyFormatSlot,NULL,NULL,NULL,NULL);
#else
	foam = foamNewProg(int0,int0,int0,int0,int0,NULL,NULL,NULL,NULL,NULL);
#endif

#ifdef MODS
	name = gen0InitialiserName(name);
#else
	name = strCopy(name);
#endif
	decl = foamNewDecl(FOAM_Prog, name, emptyFormatSlot);
	gen0ConstAdd(decl, foam);

	if (absyn) foamPos(foam) = abPos(absyn);

	if (genfEnvDebug) {
		afprintf(dbOut, "..created function %d - %s)\n", idx, name);
	}

	return foam;
}

void
gen0ConstAdd(Foam decl, Foam prog)
{
	/** FIXME: Better names needed here */
	gen0ProgList = listCons(Foam)(prog, gen0ProgList);
	gen0DeclList = listCons(Foam)(decl, gen0DeclList);
	gen0NumProgs += 1;
}


/*
 * Collect the fields of a prog.
 */
void
gen0ProgFiniEmpty(Foam prog, AInt retType, AInt format)
{
	assert(foamTag(prog) == FOAM_Prog);

	prog->foamProg.nLabels	= gen0NLabels();
	prog->foamProg.retType	= retType;
	prog->foamProg.format	= format;
	prog->foamProg.params	= gen0IssueParams();
	prog->foamProg.locals	= gen0IssueLocals();
	prog->foamProg.fluids	= gen0IssueFluids();
	prog->foamProg.levels	= gen0IssueLevels();
	prog->foamProg.body	= gen0IssueStmts();

	if (genfEnvDebug) {
		afprintf(dbOut, "Fini - fmtStack: %pAIntList, usage: %pSlotUsageList)\n",
			 gen0State->formatStack, gen0State->formatUsage);
	}
}

Foam
gen0ProgClosEmpty(void)
{
	GenFoamState	state = gen0State;
	Foam		env;

	if (genfEnvDebug && gen0State) {
		afprintf(dbOut, "ClosEmpty - fmtStack: %pAIntList, usage: %pSlotUsageList\n",
			 gen0State->formatStack, gen0State->formatUsage);
	}

	env = state ? foamCopy(car(state->envVarStack)) : foamNewEnv(int0);
	if (state && suVal(car(state->formatUsage)) == emptyFormatSlot)
		car(state->formatUsage) = suFrFormat(envUsedSlot);
	return foamNewClos(env, foamNewConst(gen0NumProgs));
}

/*
 * Generate decls for variables needed by the prog.
 */
void
gen0ProgAddParams(Length argc, String *argv)
{
	Length		i;
	String		name;
	Foam		decl;

	for (i = 0; i < argc; i += 1) {
		name = strCopy(argv[i]);
		decl = foamNewDecl(FOAM_Word, name, emptyFormatSlot);
		gen0AddParam(decl);
	}
}

void
gen0ProgAddStateFormat(AInt index)
{
	Foam	format = fboxMake(gen0State->lexPool->fbox);
	gen0ProgAddFormat(index, format);
}

void
gen0ProgAddFormat(AInt index, Foam format)
{
	AInt	nindex;

	if (genfEnvDebug) {
		afprintf(dbOut, "ProgAddFormat format %d %pFoam\n", index, format);
		afprintf(dbOut, "ProgAddFormat stack %pAIntList\n", gen0State->formatStack);
		afprintf(dbOut, "ProgAddFormat usage %pSlotUsageList\n", gen0State->formatUsage);
		afprintf(dbOut, "ProgAddFormat ddecl %pAInt Used %oBool\n", foamDDeclArgc(format), suIsUsed(car(gen0State->formatUsage)));
	}

	if (foamDDeclArgc(format) == 0 && !suIsUsed(car(gen0State->formatUsage))) {
		nindex = emptyFormatSlot;
	}
	else {
		// Not sure why this is happening here; looks like it should be at a
		// higher level
		if (gen0State->hasTemps) setcar(gen0State->formatUsage, suFrFormat(index));
		gen0FormatList = listCons(Foam)(format, gen0FormatList);
		nindex = gen0RealFormatNum++;
	}

	gen0AddFormat(index, nindex);
}


/*****************************************************************************
 *
 * :: Functions for updating the exporter with parameter lists.
 *
 ****************************************************************************/

AbSyn
gen0ProgGetExporter()
{
	GenFoamState	state;

	for (state = gen0State; state; state = state->parent)
		if (state->exporter) return state->exporter;

	return NULL;
}

AbSyn
gen0ProgPushExporter(AbSyn lhs)
{
	AbSyn old;
	old = gen0State->exporter;
	gen0State->exporter = lhs;
	return old;
}

void
gen0ProgPopExporter(AbSyn old)
{
	gen0State->exporter = old;
}

void
gen0ProgAddExporterArgs(AbSyn param)
{
	AbSyn	*argv, exporter;
	Length	i, argc;

	assert(gen0ProgGetExporter());

	switch (abTag(param)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Comma:
		argc = abArgc(param);
		argv = abArgv(param);
		break;
	default:
		argc = 1;
		argv = &param;
		break;
	}

	exporter = abNewEmpty(AB_Apply, 1 + argc);
	exporter->abApply.op = gen0ProgGetExporter();
	for (i = 0; i < argc; i += 1)
		exporter->abApply.argv[i] = argv[i];

	gen0State->exporter = exporter;
}

void
gen0ProgPopExporterArgs()
{
	AbSyn	exporter = gen0State->exporter;

	assert(abHasTag(exporter, AB_Apply));
	gen0State->exporter = exporter->abApply.op;
	abFreeNode(exporter);
}

Bool
gen0ProgHasReturn(void)
{
	GenFoamState	state = gen0State;
	return state->lines && foamTag(state->lines->first) == FOAM_Return;
}

/*****************************************************************************
 *
 * :: Functions which add to a prog state.
 *
 ****************************************************************************/

void
gen0AddInit(Foam foam)
{
	GenFoamState	state = gen0State;
	Foam *argv;
	int argc, i;

	if (foamTag(foam) == FOAM_Seq) {
		argv = foam->foamSeq.argv;
		argc = foamArgc(foam);
	} else {
		argv = &foam;
		argc = 1;
	}
	
	for (i=0; i<argc; i++)
		state->inits = listCons(Foam)(argv[i], state->inits);
}

void
gen0AddStmt(Foam foam, AbSyn absyn)
{
	GenFoamState	state = gen0State;
	Foam *argv;
	int argc, i;

	if (foamTag(foam) == FOAM_Seq) {
		argv = foam->foamSeq.argv;
		argc = foamArgc(foam);
	} else {
		argv = &foam;
		argc = 1;
	}
	
	for (i=0; i<argc; i++) {
		assert(foamTag(foam) != FOAM_Cast);

		if (absyn) foamPos(argv[i]) = abPos(absyn);
		state->lines = listCons(Foam)(argv[i], state->lines);
	}
}

AInt
gen0AddParam(Foam decl)
{
	return fboxAdd(gen0State->params, decl);
}

AInt
gen0AddLocal(Foam decl)
{
	return vpNewVarDecl(gen0State->localPool, decl);
}

AInt
gen0AddLex(Foam decl)
{
	return vpNewVarDecl(gen0State->lexPool, decl);
}

AInt
gen0AddLexNth(Foam decl, AInt n, AInt offset)
{
	GenFoamState	state = gen0NthState(n);
	VarPool		vp;

	if (offset == 0)
		vp = state->lexPool;
	else
		vp = listElt(VarPool)(state->envLexPools, offset - 1);

	return vpNewVarDecl(vp, decl);
}

/*****************************************************************************
 *
 * :: Local function definitions.
 *
 ****************************************************************************/

local Foam
gen0IssueStmts(void)
{
	GenFoamState	s = gen0State;
	Foam		seq;

	s->lines = listNConcat(Foam)(s->lines, s->inits);
	s->inits = listNil(Foam);

	s->lines = listNReverse(Foam)(s->lines);
	seq = foamNewOfList(FOAM_Seq, s->lines);

	listFree(Foam)(s->lines);
	s->lines = listNil(Foam);

	return seq;
}

#ifdef NEW_FORMATS
local AInt
gen0IssueParams(void)
{
	gen0ParamsList = listCons(Foam)(fboxMake(gen0State->params),
					gen0ParamsList);
	return gen0NumParams++;
}
#else
local Foam
gen0IssueParams(void)
{
	return fboxMake(gen0State->params);
}
#endif

local Foam
gen0IssueLocals(void)
{
	return fboxMake(gen0State->localPool->fbox);
}

local Foam
gen0IssueFluids(void)
{
	return foamNewOfList(FOAM_DFluid, (FoamList) gen0State->fluidsUsed);
}

local Foam
gen0IssueLevels(void)
{
	SlotUsageList	fu = gen0State->formatUsage;
	Length		i, argc = listLength(SlotUsage)(fu);
	Foam		levels;

	levels = foamNewEmpty(FOAM_DEnv, argc);
	for (i = 0; fu; i += 1, fu = cdr(fu))
		levels->foamDEnv.argv[i] = suVal(car(fu));

	if (genfEnvDebug) {
		afprintf(dbOut, "Issue levels - stack %pAIntList\n", gen0State->formatStack);
		afprintf(dbOut, "Issue levels - usage %pSlotUsageList\n", gen0State->formatUsage);
		afprintf(dbOut, "Issue levels - %pFoam\n", levels);
	}
	return levels;
}
