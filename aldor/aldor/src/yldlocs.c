
#include "axlobs.h"
#include "bitv.h"
#include "debug.h"
#include "dflow.h"
#include "flog.h"
#include "foam.h"
#include "store.h"
#include "yldlocs.h"

Bool	ylDebug	= false;
#define ylDEBUG	DEBUG_IF(yl)		afprintf

#define ylDF_CUTOFF (100)
/*
 * Local splitting for yields.
 * We want to identify locals whose definition/use does not cross a yield statement
 * Basic idea is dataflow analysis.
 *
 * Loc 1 --> Yld --> Usage
 *
 * 1. Assign a number to each local assignment
 * 2. Associate an implied bit vector to each statement
 *    - The bitvector will mean 'From this point, assignment X is visible' and concurrently, 'assignment X has hit a yield'
 * 3. Iterate over by bblock and statement
 *    - from an empty bitvector,
 *        - assignment X will set bit 'X_s', and clear other sets of that local + yields
 *        - yield => set yld flag for that local
 * 4. Magic of dataflow; hd and tl of each BB will give source of each assignment, plus if a yield was encountered for that source.
 *
 * Simple version:
 * For each BB find candidates (Def/Use in same BB, dropping yields), and fails (fail == use without set, use after yield).
 * If a given local is always a candidate, then mark as local
 */

typedef struct ylState {
	BitvClass bitvClass;
	Foam prog;
	FlowGraph flog;
	Bitv result;
	int nLocals;
} *YlState;

static YlState ylProgState;

local void ylFlowInitBlock0(FlowGraph flog);
local void ylFlowFillGenKillBB(FlowGraph flog, BBlock bb);
local void ylFlowFillGenKillStmt(BBlock bb, Foam stmt, Bitv gen, Bitv kill);
local void ylSet(BBlock bb, Foam lhs, Bitv gen, Bitv kill);
local void ylYield(BBlock bb, Foam lhs, Bitv gen, Bitv kill);
local void ylComputeResult(BBlock bb);
local void ylFindUses(BBlock bb, Bitv bitv, Foam foam);
local void ylFindUsesLhs(BBlock bb, Bitv bitv, Foam foam);
local void ylCheckLoc(BBlock bb, Bitv bitv, AInt idx);
local void ylMarkLoc(YlState state, AInt idx);

local YlState ylStateNew(Foam prog, FlowGraph flog, int nLocals);
local void    ylStateFree(YlState);
local void    ylStateToResult(YlState);
local YldLocResult ylLocResultFrState(YlState state);

YldLocResult
ylProg0(Foam prog)
{
	AInt locCount = foamDDeclArgc(prog->foamProg.locals);
	AIntList reclocs = listNil(AInt);
	YldLocResult result = (YldLocResult) stoAlloc(OB_Other, sizeof(*result));

	for (int i=0; i<locCount; i++) {
		reclocs = listCons(AInt)(i, reclocs);
	}

	result->locs = listNil(AInt);
	result->reclocs = listNReverse(AInt)(reclocs);
	return result;

}

YldLocResult
ylProg(Foam prog)
{
	FlowGraph flog;
	BBlock bb;
	AInt locCount = foamDDeclArgc(prog->foamProg.locals);
	int i, k;

	ylDEBUG(dbOut, "Starting\n    %pFoam\n", prog);

	flog = flogFrProg(prog, FLOG_UniqueExit);
	ylProgState = ylStateNew(prog, flog, locCount);

	flogBitvClass(flog) = bitvClassCreate(locCount * 2);

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		dflowNewBlockInfo(bb, locCount * 2, ylFlowFillGenKillBB);
	}

	// Run dflow...
	i = dflowFwdIterate(flog, DFLOW_Union, ylDF_CUTOFF, &k,
			    (DFlowInitFun) ylFlowInitBlock0);

	// Find the result
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (bb != NULL)
			ylComputeResult(bb);
	}

	YldLocResult result = ylLocResultFrState(ylProgState);
	// Back to Prog mode
	flogToProg(flog);
	ylStateFree(ylProgState);
	ylDEBUG(dbOut, "Locs   %pAIntList\n", result->locs);
	ylDEBUG(dbOut, "Record %pAIntList\n", result->reclocs);
	return result;
}

local void
ylFlowInitBlock0(FlowGraph flog)
{
	// All zeros to start
	return;
}


local void
ylFlowFillGenKillBB(FlowGraph flog, BBlock bb)
{
	Foam seq, stmt;
	int i;
	seq = bb->code;
	ylDEBUG(dbOut, "(BB Starts %pFoam\n", seq);

	bitvClearAll(ylProgState->bitvClass, dfFwdIn(bb));
	bitvClearAll(ylProgState->bitvClass, dfFwdGen(bb));
	bitvClearAll(ylProgState->bitvClass, dfFwdKill(bb, int0));

	for (i=0; i<foamArgc(seq); i++) {
		Foam stmt = seq->foamSeq.argv[i];
		ylFlowFillGenKillStmt(bb, stmt, dfFwdGen(bb), dfFwdKill(bb, int0));

		Bitv chk = bitvNew(ylProgState->bitvClass);
		bitvClearAll(ylProgState->bitvClass, chk);
		bitvAnd(ylProgState->bitvClass, chk, dfFwdGen(bb), dfFwdKill(bb, int0));
		assert(0 == bitvCount(ylProgState->bitvClass, chk));
		bitvFree(chk);
	}
	ylDEBUG(dbOut, " BB Done - Gen  %pAIntList\n", bitvToAIntList(ylProgState->bitvClass, dfFwdGen(bb)));
	ylDEBUG(dbOut, " BB Done - Kill %pAIntList)\n", bitvToAIntList(ylProgState->bitvClass, dfFwdKill(bb, int0)));
}

local void
ylFlowFillGenKillStmt(BBlock bb, Foam stmt, Bitv gen, Bitv kill)
{
	switch (foamTag(stmt)) {
	case FOAM_Set:
	case FOAM_Def:
		ylSet(bb, stmt->foamSet.lhs, gen, kill);
		break;
	case FOAM_Yield:
		ylYield(bb, stmt, gen, kill);
		break;
	default:
		break;
	}
}

local void
ylSet(BBlock bb, Foam lhs, Bitv gen, Bitv kill)
{
	if (foamTag(lhs) == FOAM_Values) {
		foamIter(lhs, v, {
				ylSet(bb, *v, gen, kill);
			});
	}
	if (foamTag(lhs) == FOAM_Loc) {
		bitvClear(bbBitvClass(bb), kill, lhs->foamLoc.index);
		bitvSet(bbBitvClass(bb), gen, lhs->foamLoc.index);
		bitvClear(bbBitvClass(bb), gen, ylProgState->nLocals + lhs->foamLoc.index);
		bitvSet(bbBitvClass(bb), kill, ylProgState->nLocals + lhs->foamLoc.index);
	}
}

local void
ylYield(BBlock bb, Foam stmt, Bitv gen, Bitv kill)
{
	AInt count = ylProgState->nLocals;
	for (int i = count; i < 2*count; i++) {
		bitvClear(bbBitvClass(bb), kill, i);
		bitvSet(bbBitvClass(bb), gen, i);
	}
}

local void
ylComputeResult(BBlock bb)
{
	Foam code = bb->code;
	Bitv bitv = dfFwdIn(bb);

	for (int i=0; i<foamArgc(code); i++) {
		Foam stmt = code->foamSeq.argv[i];
		ylFindUses(bb, bitv, stmt);
	}
}

local void
ylFindUses(BBlock bb, Bitv bitv, Foam foam)
{
	switch (foamTag(foam)) {
	case FOAM_Set:
	case FOAM_Def:
		// NB: Evaluation order
		ylFindUses(bb, bitv, foam->foamSet.rhs);
		ylFindUsesLhs(bb, bitv, foam->foamSet.lhs);
		break;
	case FOAM_Loc:
		ylCheckLoc(bb, bitv, foam->foamLoc.index);
		break;
	case FOAM_Yield:
		ylFindUses(bb, bitv, foam->foamYield.value);
		for (int i=0; i<ylProgState->nLocals; i++) {
			bitvSet(bbBitvClass(bb), bitv, ylProgState->nLocals + i);
		}
		break;
	default:
		foamIter(foam, expr, ylFindUses(bb, bitv, *expr));
	}
}

local void
ylFindUsesLhs(BBlock bb, Bitv bitv, Foam foam)
{
	switch (foamTag(foam)) {
	case FOAM_Values:
		foamIter(foam, e, ylFindUsesLhs(bb, bitv, *e));
		break;
	case FOAM_Loc:
		bitvSet(ylProgState->bitvClass, bitv, foam->foamLoc.index);
		bitvClear(ylProgState->bitvClass, bitv, ylProgState->nLocals + foam->foamLoc.index);
		break;
	default:
		foamIter(foam, expr, ylFindUses(bb, bitv, *expr));
	}
}


local void
ylCheckLoc(BBlock bb, Bitv bitv, AInt idx)
{
	Bool live = bitvTest(ylProgState->bitvClass, bitv, idx);
	Bool yld = bitvTest(ylProgState->bitvClass, bitv, idx + ylProgState->nLocals);

	if (live && yld) {
		ylMarkLoc(ylProgState, idx);
	}
}

local void
ylMarkLoc(YlState state, AInt idx)
{
	ylDEBUG(dbOut, "Marking loc %d\n", idx);
	bitvSet(state->bitvClass, state->result, idx);
}

local YlState
ylStateNew(Foam prog, FlowGraph flog, int nLocals)
{
	YlState state = (YlState) stoAlloc(OB_Other, sizeof(*state));
	state->nLocals = nLocals;
	state->prog = prog;
	state->flog = flog;
	state->bitvClass = bitvClassCreate(2*nLocals);
	state->result = bitvNew(state->bitvClass);
	bitvClearAll(state->bitvClass, state->result);
	return state;
}

local void
ylStateFree(YlState state)
{
	bitvClassDestroy(state->bitvClass);
	stoFree(state);
}

local YldLocResult
ylLocResultFrState(YlState state)
{
	YldLocResult result = (YldLocResult) stoAlloc(OB_Other, sizeof(*result));
	AIntList reclocs = listNil(AInt);
	AIntList locs = listNil(AInt);

	for (int i=0; i<state->nLocals; i++) {
		if (bitvTest(state->bitvClass, state->result, i))
			reclocs = listCons(AInt)(i, reclocs);
		else
			locs = listCons(AInt)(i, locs);
	}
	result->reclocs = listNReverse(AInt)(reclocs);
	result->locs = listNReverse(AInt)(locs);

	return result;
}

void
ylLocResultFree(YldLocResult result)
{
	listFree(AInt)(result->locs);
	listFree(AInt)(result->reclocs);
	stoFree(result);
}
