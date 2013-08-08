/*****************************************************************************
 *
 * inlutil.c: Utilities for the inliner.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "flog.h"
#include "inlutil.h"
#include "opttools.h"
#include "of_inlin.h"
#include "of_util.h"
#include "loops.h"
#include "optfoam.h"
#include "strops.h"

Bool	inuProgDebug	= false;

#define inuProgDEBUG	DEBUG_IF(inuProgDebug)

/*****************************************************************************
 *
 * :: inuDereference
 *
 ****************************************************************************/

#define	inuIsVar(foam)		(foamTag(foam) == FOAM_Loc || \
				 foamTag(foam) == FOAM_Lex || \
				 foamTag(foam) == FOAM_Glo)

#define inuReachingDef(foam)	((Foam) udReachingDefs(foam))

/* Make one step through the use/def chain. */
local Foam
inuDereference(Foam foam)
{
	Foam	def, val = NULL;

	if (foam && (def = inuReachingDef(foam)) != NULL) {
		assert(otIsDef(def));
		val = def->foamDef.rhs;
	}

	return val;
}

/* Walk through the use/def chain, following uses of foam reached
 * by a unique definition.  Stop at the first object which is not a var.
 * If a var with more than a one definition is found, return NULL.
 */
local Foam
inuDereferenceDefChain(Foam foam)
{
	while (foam && inuIsVar(foam))
		foam = inuDereference(foam);

	return foam;
}

/* Return the first syme found on the use/def chain. */
local Syme
inuDereferenceSyme(Foam foam)
{
	while (foam && !foamSyme(foam) && inuIsVar(foam))
		foam = inuDereference(foam);

	return foam ? foamSyme(foam) : NULL;
}

/* Return the first closure found on the use/def chain. */
local Foam
inuDereferenceClos(Foam foam)
{
	while (foam && foamTag(foam) != FOAM_Clos && inuIsVar(foam))
		foam = inuDereference(foam);

	return (foam && foamTag(foam) == FOAM_Clos) ? foam : NULL;
}

/*****************************************************************************
 *
 * :: inuGetClos
 *
 ****************************************************************************/

Foam
inuGetClosFrVar(Foam foam)
{
	return inuDereferenceClos(foam);
}

/*****************************************************************************
 *
 * :: inuGetSyme
 *
 ****************************************************************************/

Syme
inuGetSymeFrClosVar(Foam clos)
{
	clos = inuDereferenceDefChain(clos);
	if (clos && foamTag(clos) == FOAM_Clos)
		return inuGetSymeFrEnv(clos->foamClos.env);

	return NULL;
}

Syme
inuGetSymeFrEnv(Foam env)
{
	Syme	syme = inuDereferenceSyme(env);

	if (otIsConstSyme(syme))
		return syme;

	env = inuDereferenceDefChain(env);
	if (env && foamTag(env) == FOAM_CEnv)
		return inuGetSymeFrClosVar(env->foamCEnv.env);

	return NULL;
}

/*****************************************************************************
 *
 * :: inuGetConstEnv
 *
 ****************************************************************************/

Foam
inuGetConstEnvFrClosVar(Foam clos)
{
	clos = inuDereferenceDefChain(clos);
	if (clos && foamTag(clos) == FOAM_Clos)
		return inuGetConstEnvFrEnv(clos->foamClos.env);

	return NULL;
}

Foam
inuGetConstEnvFrEnv(Foam env)
{
	Syme	syme = inuDereferenceSyme(env);

	if (otIsConstSyme(syme)) {
		foamSyme(env) = syme;
		return env;
	}

	env = inuDereferenceDefChain(env);
	if (env && foamTag(env) == FOAM_CEnv)
		return inuGetConstEnvFrClosVar(env->foamCEnv.env);

	return NULL;
}

/*****************************************************************************
 *
 * :: inuGetPushEnv
 *
 ****************************************************************************/

Foam
inuGetPushEnvFrClosVar(Foam clos)
{
	clos = inuDereferenceDefChain(clos);
	if (clos && foamTag(clos) == FOAM_Clos)
		return inuGetPushEnvFrVar(clos->foamClos.env);

	return NULL;
}

Foam
inuGetPushEnvFrVar(Foam foam)
{
	foam = inuDereferenceDefChain(foam);
	if (!foam) return NULL;

	if (foamTag(foam) == FOAM_PushEnv)
		return foam;

	if (foamTag(foam) == FOAM_CEnv)
		return inuGetPushEnvFrClosVar(foam->foamCEnv.env);

	return NULL;
}

/* Return true if FOAM is a var referring to a PushEnv */
Bool
inuIsLocalEnv(Foam foam)
{
	return inuGetPushEnvFrVar(foam) != NULL;
}

/*****************************************************************************
 *
 * :: Peep Expr
 *
 ****************************************************************************/

/* Probably should be moved in of_peep.c */
/* Should call foamFreeNode on casts */
Foam
inuPeepExpr(Foam foam)
{
	Foam core;

	foamIter(foam, arg, *arg = inuPeepExpr(*arg); );

	if (foamTag(foam) != FOAM_Cast) return foam;

	core = foam;

	otDereferenceCast(core);

	if (foamTag(core) == FOAM_BCall &&
	    foamBValInfo(core->foamBCall.op).retType == foam->foamCast.type)
		return core;

	if (foamTag(core) == FOAM_CCall &&
	    core->foamCCall.type == foam->foamCast.type)
		return core;

	return foam;
}

/*****************************************************************************
 *
 * :: Prog Init/Fini
 *
 ****************************************************************************/

struct {
	int 	numCCalls;
	int	numOCalls;
	int 	numConsts;
	Bool	dontInline;
	int    	size;

} inuProgInfo;

struct {
	Foam gdecl;
	Foam * ddefv;
} inuUnitInfo;

void
inuProgInit(Foam prog)
{	
	OptInfo inlProg = foamOptInfo(prog);
	FlowGraph flog = flogFrProg(prog, FLOG_UniqueExit);

	assert(foamTag(prog) == FOAM_Prog);

	/* Loop Info */
	lpLoopDepthInfoSetInFlog(flog);

	/* UD Chains */
	inlProg->converged = usedefChainsFrFlog(flog, UD_OUTPUT_SinglePointer);

	inlProg->flog = flog;

	inuProgInfo.numCCalls = 0;
	inuProgInfo.numOCalls = 0;
	inuProgInfo.numConsts = 0;
	inuProgInfo.dontInline = false;
	inuProgInfo.size      = 0;

	assert(!genIsRuntime() || inlProg->converged);
}

#define INU_FLOG_CUTOFF		5000

/* !! This might be a BOTTLENECK */
void
inuProgUpdate(Foam prog)
{
	OptInfo inlProg = foamOptInfo(prog);
	FlowGraph flog;

	assert(foamTag(prog) == FOAM_Prog);

	if (inlProg->flog)
		prog = flogToProg(inlProg->flog);

	flog = flogFrProg(prog, FLOG_UniqueExit);

	/* Loop Info */
	lpLoopDepthInfoSetInFlog(flog);

		/* UD Chains */
	if (flogBlockC(flog) < INU_FLOG_CUTOFF ||
	    genIsRuntime() || optIsMaxLevel())
		inlProg->converged = usedefChainsFrFlog(flog,
						UD_OUTPUT_SinglePointer);
	else
		inlProg->converged = false;

	inlProg->flog = flog;

	assert(!genIsRuntime() || inlProg->converged);
}

void
inuProgFini(Foam prog)
{
	OptInfo	    inlProg = foamOptInfo(prog);
	
	assert(foamTag(prog) == FOAM_Prog);

	if (inlProg->flog)
		flogToProg(inlProg->flog);

	inlProg->flog = NULL;
}

/*****************************************************************************
 *
 * :: Unit Init/Fini 
 *
 ****************************************************************************/
local void 	inuAnalyseProg(Foam foam, int constNum);
local void	inuAnalyseExpr(Foam foam, Foam father, int constNum);

/* !! This must be changed ASAP. It's really bad the way in which
 * progs are initialized and finished. inlProg should call inuProgInit/Fini
 */
void
inuUnitInit(Foam foam)
{
	Foam	def, cnst, prog;
	Foam    * ddefv = foam->foamUnit.defs->foamDDef.argv;
	int	i, ddefc = foamArgc(foam->foamUnit.defs);
	assert(foamTag(foam) == FOAM_Unit);

	inuUnitInfo.gdecl = foamUnitGlobals(foam);
	inuUnitInfo.ddefv = ddefv;

	for (i = 0; i < ddefc; i++) {
		def = ddefv[i];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		foamOptInfo(def->foamDef.rhs)->numRefs = 0;
	}

	for (i = 0; i < ddefc; i++) {
		def = ddefv[i];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		cnst = def->foamDef.lhs;
		assert(foamTag(cnst) == FOAM_Const);

		inuAnalyseProg(def->foamDef.rhs, cnst->foamConst.index);
	}

	for (i = 0; i < ddefc; i++) {
		def = ddefv[i];
		
		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		prog = def->foamDef.rhs;

		if (foamOptInfo(prog)->numRefs == 1)
			foamProgSetIsCalledOnce(prog);
		else
			foamProgUnsetIsCalledOnce(prog);
	}
}

void
inuUnitFini(Foam foam)
{
	Foam	def, defs = foam->foamUnit.defs;
	int	i;
	assert(foamTag(foam) == FOAM_Unit);

	for (i = 0; i < foamArgc(defs); i++) {
		def = defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.lhs) != FOAM_Const ||
		    foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		inuProgFini(def->foamDef.rhs);
	}

	return;
}

/******************************************************************************
 *
 * :: Analyse Progs
 *
 *****************************************************************************/

local void
inuAnalyseProg(Foam foam, int constNum)
{
	AInt	timeCost;
	OptInfo	optInfo = foamOptInfo(foam);

	assert(foamTag(foam) == FOAM_Prog);

	if (foamArgc(foam->foamProg.body) == 1)
		foamProgSetHasSingleStmt(foam);

	inuProgInit(foam);

	/* -- perform dflow analysis on prog */
	/* -- find loops and update info on blocks */
	/* -- set num-loops info in prog */

	flogIter(optInfo->flog, bb, {
		inuAnalyseExpr(bb->code, bb->code, constNum);
	});

	if (inuProgInfo.numOCalls == 0)
		foamProgSetHasNoOCalls(foam);

	if (inuProgInfo.numConsts > 0)
		foamProgSetHasConsts(foam);

	if (inuProgInfo.dontInline)
		foamProgSetDontInlineMe(foam);

	foam->foamProg.size = inuProgInfo.size;

	timeCost = (inuProgInfo.numCCalls + inuProgInfo.numOCalls) *
			InlUnknownCallsMagicNumber;

	flogIter(optInfo->flog, bb, {
		timeCost += (1 << (bb->iextra * InlLoopMagicNumber));
	});

	foam->foamProg.time = timeCost;

	foamProgSetHasInlineInfo(foam);
}

local void
inuAnalyseExpr(Foam foam, Foam father, int constNum)
{
	foamIter(foam, arg, inuAnalyseExpr(*arg, foam, constNum); );

	switch(foamTag(foam)) {
      	case FOAM_CCall:
		inuProgInfo.numCCalls++; break;

	case FOAM_OCall: 
		inuProgInfo.numOCalls++; break;
		/* -- inc num of OCall in prog */
		/* Check, using dflow, if it's a self call */

	case FOAM_Const: {
		Foam def = inuUnitInfo.ddefv[foam->foamConst.index];

		assert(foamTag(def) == FOAM_Def);
		
		if (foam->foamConst.index == constNum)
			break;

		inuProgInfo.numConsts++;

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			break;

		/* Note: Only with OCalls we can assume 1 call.
 		 * In all other cases we increment more than 1 to be sure that
 		 * the Const will not result called exactly once (that is the
 		 * case in which we set the bit UniqueCall.
		 */
		if (foamTag(father) == FOAM_OCall)
			foamOptInfo(def->foamDef.rhs)->numRefs += 1;
		else
			foamOptInfo(def->foamDef.rhs)->numRefs += 5;

		break;
	}

		/* !! TEST FOR THE EVIL GLOBAL */
	case FOAM_Return: {
			Foam 	val = foam->foamReturn.value;
			AInt	index;
			String	str;

			if (foamTag(val) != FOAM_Glo) break;
			index = val->foamGlo.index;
			str = inuUnitInfo.gdecl->foamDDecl.argv[index]->foamGDecl.id;
			if (strEqual(str, "dummyDefault"))
				inuProgInfo.dontInline = true;
		break;
		}
	default:
		break;
	}

	inuProgInfo.size++;
}

/******************************************************************************
 *
 * :: Refresh Prog Info
 *
 *****************************************************************************/
local void	inuProgInfoRefresh		(Foam, int);
local void	inuProgSizeComputeWithConsts	(Foam, Foam, int);

void
inuUnitInfoRefresh(Foam foam)
{
	Foam	def, defs = foam->foamUnit.defs;
	int	i;
	assert(foamTag(foam) == FOAM_Unit);

	inuUnitInfo.ddefv = foam->foamUnit.defs->foamDDef.argv;

	for (i = 0; i < foamArgc(defs); i++) {
		def = defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		foamOptInfo(def->foamDef.rhs)->dvState = DV_NotChecked;
	}

	for (i = 0; i < foamArgc(defs); i++) {
		def = defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		inuProgInfoRefresh(def->foamDef.rhs, i);
	}
}


local void
inuProgInfoRefresh(Foam prog, int constNum)
{

	assert(foamTag(prog) == FOAM_Prog);

	if (foamOptInfo(prog)->dvState == DV_Checked) return;

	foamOptInfo(prog)->dvState = DV_Checked;

	inuProgInfo.numCCalls = 0;
	inuProgInfo.numOCalls = 0;
	inuProgInfo.numConsts = 0;
	inuProgInfo.dontInline = false;
	prog->foamProg.size = 0;

	foamProgSetHasNoOCalls(prog);
	foamProgUnsetHasConsts(prog);

	inuProgSizeComputeWithConsts(prog->foamProg.body, prog, constNum);

	if (foamArgc(prog->foamProg.body) == 1)
		foamProgSetHasSingleStmt(prog);
	else
		foamProgUnsetHasSingleStmt(prog);

	foamProgSetHasInlineInfo(prog);
}

local void
inuProgSizeComputeWithConsts(Foam foam, Foam prog, int constNum)
{
	foamIter(foam, arg, inuProgSizeComputeWithConsts(*arg, prog, constNum); );

	switch(foamTag(foam)) {

	case FOAM_OCall: 
		foamProgUnsetHasNoOCalls(prog);
		break;

	case FOAM_Const: {
		int index = foam->foamConst.index;
		Foam newProg, def;


		if (index == constNum) break;
		def = inuUnitInfo.ddefv[index];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog) break;

		newProg = def->foamDef.rhs;
		inuProgInfoRefresh(newProg, index);

		prog->foamProg.size += newProg->foamProg.size;
		foamProgSetHasConsts(prog);

		break;
	}
		
	default:
		break;
	}

	prog->foamProg.size++;

	

}

/******************************************************************************
 *
 * :: FlowGraph Merging
 *
 *****************************************************************************/

#ifdef FlogsMerge

/* "stmt" is the point in "bb" where flogChild must be inserted.
 * NOTE: assumes that flogChild has a unique exit. An exit is a block that
 * has no successors. This assumtion should be true by construction.
 */
local void
inuFlogsMerge(FlowGraph flogFather, FlowGraph flogChild, BBlock bb,
	      Foam stmt)
{
	BBlock bbLast;

	/* Find unique exit block of flogChild */

	flogIter(flogChild, b, {
		if (bbExitC(b) == 0) {
			bbLast = b;
			break;
		}
	});

	/* !! to be completed !! $$*/

}
#endif
