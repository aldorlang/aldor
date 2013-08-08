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

	ret = genFoamVal(expr);
	if (ret) gen0AddStmt(foamNewReturn(ret), expr);

	gen0ProgPushFormat(emptyFormatSlot);
	gen0IssueDCache();
#ifdef ORIGINAL_WORKING_VERSION
	/* Delete this version */
	tag = gen0Type(gen0AbType(expr), &fmt);
#else
	/* This fixes a bug with embeddings */
	tag = gen0Type(gen0AbContextType(expr), &fmt);
#endif
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
	AIntList	fu, fs, ef;
	Length		i, argc;

	assert(state && state->parent);
	fu = state->parent->formatUsage;
	fs = state->parent->formatStack;
	ef = state->parent->envFormatStack;

	argc = listLength(AInt)(ef);
	for (i = 0; i < argc - 1; i += 1)
		fu = listCons(AInt)(emptyFormatSlot, fu);

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
	AIntList fu, ofu;
	int i;
	gen0State = gen0State->parent;
	
	ofu = state->parent->formatUsage;
	fu  = cdr(state->formatUsage);
	/* Copy usage information downwards */
	for (i=0; i<state->parent->whereNest; i++)
		fu = cdr(fu);
	while (fu != listNil(AInt)) {
		if (car(ofu) == emptyFormatSlot)
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
	AIntList	fu, ef;

	fu = state->formatUsage;
	ef = state->envFormatStack;

	assert(fu);
	if (ef && cdr(ef)) fu = listConcat(AInt)(ef, cdr(fu));
	fu = listCons(AInt)(index, fu);

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

	genfConstDEBUG{genfNumProg(gen0NumProgs, name);}

#ifdef NEW_FORMATS
	foam = foamNewProg(int0,int0,int0,int0,int0,emptyFormatSlot,NULL,NULL,NULL,NULL);
#else
	foam = foamNewProg(int0,int0,int0,int0,int0,NULL,NULL,NULL,NULL,NULL);
#endif
	gen0ProgList = listCons(Foam)(foam, gen0ProgList);
	gen0NumProgs += 1;

#ifdef MODS
	name = gen0InitialiserName(name);
#else
	name = strCopy(name);
#endif
	decl = foamNewDecl(FOAM_Prog, name, emptyFormatSlot);
	gen0DeclList = listCons(Foam)(decl, gen0DeclList);

	if (absyn) foamPos(foam) = abPos(absyn);
	return foam;
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
}

Foam
gen0ProgClosEmpty(void)
{
	GenFoamState	state = gen0State;
	Foam		env;

	env = state ? foamCopy(car(state->envVarStack)) : foamNewEnv(int0);
	if (state && car(state->formatUsage) == emptyFormatSlot)
		car(state->formatUsage) = envUsedSlot;
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
gen0ProgAddFormat(AInt index)
{
	Foam	format = fboxMake(gen0State->lexPool->fbox);
	AInt	nindex;

	if (foamDDeclArgc(format) == 0)
		nindex = emptyFormatSlot;
	else {
		if (gen0State->hasTemps) setcar(gen0State->formatUsage, index);
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
	AIntList	fu = gen0State->formatUsage;
	Length		i, argc = listLength(AInt)(fu);
	Foam		levels;

	levels = foamNewEmpty(FOAM_DEnv, argc);
	for (i = 0; fu; i += 1, fu = cdr(fu))
		levels->foamDEnv.argv[i] = car(fu);

	return levels;
}
