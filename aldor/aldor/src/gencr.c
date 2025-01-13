/*****************************************************************************
 *
 * gencr.c: Foam coroutine removal.
 *
 * Copyright (c) 2024
 *
 ****************************************************************************/
#include "gencr.h"

#include "axlobs.h"
#include "debug.h"
#include "fluid.h"
#include "foam.h"
#include "int.h"
#include "store.h"
#include "syme.h"
#include "table.h"
#include "yldlocs.h"

Bool	gcrDebug	= false;
Bool	gcrFmtDebug	= false;

#define gcrDEBUG DEBUG_IF(gcr) afprintf
#define gcrFmtDEBUG DEBUG_IF(gcrFmt) afprintf

typedef struct gcrProgState *GcrProgState;
struct gcrProgState {
	// FoamList stmts;
	AInt    *newLocForOld;
	AInt    *recLocForOld;
	Foam     oldLocals;
	FoamList recLocals;
	FoamList newLocals;
	AInt     nNewLocals;
	AInt     nRecLocals;
};

local GcrProgState gcrProgStateNew(Foam prog);
local void gcrProgStateFree(GcrProgState);
local void gcrProgStateLocals(GcrProgState);
local AInt gcrProgStateAllocateLocal(GcrProgState state, AInt oldIdx, Bool isTmp);
local FoamList gcrProgStateNewLocals(GcrProgState state);

local AInt gcrProgStateAllocateRecLocal(GcrProgState state, AInt oldIdx);
local void gcrProgStateSetNewLocal(GcrProgState state, AInt oldIdx, AInt newIdx);
local void gcrProgStateSetNewRecLocal(GcrProgState state, AInt oldIdx, AInt newIdx);
local AInt gcrProgStateRecLocalIdx(GcrProgState state, AInt oldIdx);
local AInt gcrProgStateNewLocalIdx(GcrProgState state, AInt oldIdx);
local Bool gcrProgStateIsNewLocal(GcrProgState state, AInt idx);

/*
 * Replace coroutines with functions.
 *
 * Idea is that
 * - pass a step and record based parameter to the coroutine.
 * - add a 'Select' to the top of the coroutine.
 * - This will use the step to select a continuation point.
 * - The record will be used to contain state where needed.
 * - Yields are replaced by returns.
 * -
 * - Any locals whose usage crosses a yield are moved to a record,
 * - this record is passed in by the caller.
 *  (Prog <void> (Seq (Yield "exit-1") (Yield "exit-2") (Return)))
 * -->
 * (Prog (int state, FiRec rec)
 *    (Seq (Select state Y_0 Y_1 Y_2)
 *        (Label Y_0)
 *	  (set (Ref state) 1)
 *	  (set (Ref value) "exit-1")
 *	  (return)
 *	  (Label Y_1)
 *	  (set (Ref state) 2)
 *	  (set (Ref value) "exit-2")
 *	  (return)
 *	  (Label Y_2)
 *	  (Set (ref state) -1)
 *	  (return)
 */

local void gcrRewriteDefs	(Foam foam);
local void gcrProgParams	(Foam foam);
local void gcrProgLocalRec	(Foam foam);
local void gcrProgLocals	(Foam foam);
local void gcrProgSeq		(Foam foam);
local Foam gcrProgTransformExpr (Foam expr);
local int  gcrCoroutineMaxLabel	(Foam foam);
local Foam gcrStateVar		(void);
local Foam gcrSetReturnValue	(Foam foam);
local int  gcrCoroutineYieldCount(Foam foam);
local void gcrFillYldLocals     (YldLocResult locResult, Foam foam);

local FoamList gcrProgTransformSet(GcrProgState state, Foam stmt);

local void gcrInit(Foam unit);
local void gcrFini(Foam unit);

local void gcrRewriteFormats(Foam formats);
local void gcrResolveFormats();

local void gcrRewriteDefs(Foam defs);

local void gcrProgHeader(Foam foam);
local void gcrProgRewriteProg(Foam lhs, Foam rhs);
local Foam gcrProgRewriteExpr(Foam foam);
local void gcrProgRewriteCoroutine(Foam lhs, Foam foam);
local void gcrProgSetFmt(AInt fmtId);

local AInt gcrDDeclAdd(Foam foam);

local AInt         gcrvFormatCount;
local AInt         gcrvRetFmtId;
local AInt         gcrvRecLocFmtId;
local FoamList     gcrvNewFormats;
local FoamList     gcrvGenerList;
local AInt        *gcrvFmtForConst;
local AInt         gcrvConstId;
local GcrProgState gcrvProgState;

Foam
gcrRewriteUnit(Foam unit)
{
	Foam formats, defs, newUnit;

	if (!foamUnitHasCoroutine(unit)) {
		return unit;
	}

	newUnit = foamCopy(unit);
	gcrInit(newUnit);
	gcrRewriteDefs(newUnit);
	gcrRewriteFormats(newUnit);
	gcrResolveFormats();
	gcrFini(newUnit);

	return newUnit;
}

void
gcrInit(Foam foam)
{
	Foam retFormat;
	AInt count, i;

	gcrvFormatCount = foamArgc(foam->foamUnit.formats);
	gcrvNewFormats = listNil(Foam);
	gcrvGenerList = listNil(Foam);
	gcrvRetFmtId = gcrvFormatCount;
	//gcrvFmtForConst = tblNew((TblHashFun) aintHash, (TblEqFun) aintEqual);
	count = foamArgc(foam->foamUnit.defs);
	gcrvFmtForConst = (AInt *) stoAlloc(OB_Other, count * sizeof(AInt));
	for (i=0; i<count; i++) {
		gcrvFmtForConst[i] = -1;
	}
	retFormat = foamNewDDecl(FOAM_DDecl_Multi,
				 foamNewDecl(FOAM_SInt, strCopy("step"), emptyFormatSlot),
				 foamNewDecl(FOAM_Word, strCopy("retVal"), emptyFormatSlot),
				 NULL);
        gcrDDeclAdd(retFormat);
}

void
gcrFini(Foam foam)
{
	gcrvFormatCount = 0;
	gcrvNewFormats = listNil(Foam);
	stoFree(gcrvFmtForConst);
}

local void
gcrResolveFormats()
{
	listIter(Foam, gcreate, gcrvGenerList, {
			Foam constFn = gcreate->foamGener.prog;
			AInt val = gcrvFmtForConst[constFn->foamConst.index];
			gcrFmtDEBUG(dbOut, "Resolving: const: %d fmt: %d", constFn->foamConst.index, val);
			if (val != -1L) {
				gcreate->foamGener.fmt = val;
			}
		});
	listFree(Foam)(gcrvGenerList);
	gcrvGenerList = listNil(Foam);
}

local void
gcrRewriteFormats(Foam foam)
{
	FoamList fmts;
	Foam newFormats;
	int i = 0;

	newFormats = foamNew(FOAM_DFmt, foamArgc(foam->foamUnit.formats) + listLength(Foam)(gcrvNewFormats));
	for (i=0; i<foamArgc(foam->foamUnit.formats); i++) {
		newFormats->foamDFmt.argv[i] = foam->foamUnit.formats->foamDFmt.argv[i];
	}
	fmts = listNReverse(Foam)(gcrvNewFormats);
	while (fmts != listNil(Foam)) {
		newFormats->foamDFmt.argv[i++] = car(fmts);
		fmts = cdr(fmts);
	}
	foam->foamUnit.formats = newFormats;
}

local void
gcrRewriteDefs(Foam unit)
{
	Foam defs = unit->foamUnit.defs;
	int i;

	for (i=0; i<foamArgc(defs); i++) {
		Foam def = defs->foamDDef.argv[i];
		if (foamTag(def->foamDef.rhs) == FOAM_Prog
		    && foamProgIsCoroutine(def->foamDef.rhs)) {
			gcrProgRewriteCoroutine(def->foamDef.lhs, def->foamDef.rhs);
		}
		if (foamTag(def->foamDef.rhs) == FOAM_Prog
		    && !foamProgIsCoroutine(def->foamDef.rhs)) {
			gcrProgRewriteProg(def->foamDef.lhs, def->foamDef.rhs);
		}
	}
}

local void
gcrProgRewriteProg(Foam lhs, Foam prog)
{
	gcrProgRewriteExpr(prog);
}

local Foam
gcrProgRewriteExpr(Foam expr)
{
	foamIter(expr, arg, *arg = gcrProgRewriteExpr(*arg));

	switch (foamTag(expr)) {
	case FOAM_Gener:
		gcrvGenerList = listCons(Foam)(expr, gcrvGenerList);
		return expr;
	default:
		return expr;
	}

}

local void
gcrProgRewriteCoroutine(Foam lhs, Foam foam)
{
	Scope("gcrProgRewrite");
	GcrProgState fluid(gcrvProgState);
	AInt fluid(gcrvRecLocFmtId);
	AInt fluid(gcrvConstId);

	YldLocResult ylLocResult = ylProg(foam);

	gcrvProgState = gcrProgStateNew(foam);

	gcrFillYldLocals(ylLocResult, foam);
	ylLocResultFree(ylLocResult);

	gcrvConstId = lhs->foamConst.index;
	gcrProgHeader(foam);
	gcrProgLocalRec(foam);
	gcrProgSeq(foam);
	gcrProgLocals(foam);
	gcrProgParams(foam);

	gcrProgStateFree(gcrvProgState);

	Return(Nothing);
}

local void
gcrProgHeader(Foam foam)
{
	foam->foamProg.retType = FOAM_NOp;
	foam->foamProg.format = gcrvRetFmtId;
}

local void
gcrFillYldLocals(YldLocResult locResult, Foam foam)
{
	AIntList loclst = locResult->locs;
	AIntList reclst = locResult->reclocs;
	AInt newIdx;

	while (loclst != listNil(AInt)) {
		AInt locIdx = car(loclst);
		loclst = cdr(loclst);
		newIdx = gcrProgStateAllocateLocal(gcrvProgState, locIdx, false);
		gcrProgStateSetNewLocal(gcrvProgState, locIdx, newIdx);
	}

	while (reclst != listNil(AInt)) {
		AInt locIdx = car(reclst);
		reclst = cdr(reclst);
		newIdx = gcrProgStateAllocateRecLocal(gcrvProgState, locIdx);
		gcrProgStateSetNewRecLocal(gcrvProgState, locIdx, newIdx);
	}
}

local void
gcrProgParams(Foam foam)
{
	Foam ddecl;
	ddecl = foamNewDDecl(FOAM_DDecl_Param,
			     foamNewDecl(FOAM_SInt, "step", 0),
			     foamNewDecl(FOAM_Rec, "state", gcrvRecLocFmtId),
			     NULL);

	assert(foamDDeclArgc(foam->foamProg.params) == 0);
	foamFree(foam->foamProg.params);
	foam->foamProg.params = ddecl;
}

local void
gcrProgLocalRec(Foam prog)
{
	Foam newDDecl;
	FoamList lst;
	AInt len;
	int i;

	lst = listNReverse(Foam)(gcrvProgState->recLocals);
	len = listLength(Foam)(lst);
	newDDecl = foamNewDDeclEmpty(len, FOAM_DDecl_Record);

	for (i=0; i<len; i++) {
		newDDecl->foamDDecl.argv[i] = foamCopy(car(lst));
		lst = cdr(lst);
	}

	gcrvRecLocFmtId = gcrDDeclAdd(newDDecl);
}

local void
gcrProgLocals(Foam prog)
{
	gcrProgSetFmt(gcrvRecLocFmtId);
	foamFree(prog->foamProg.locals);
	prog->foamProg.locals = foamNewDDeclOfList(FOAM_DDecl_Local, gcrProgStateNewLocals(gcrvProgState));
}

local void
gcrProgSetFmt(AInt fmtId)
{
	gcrFmtDEBUG(dbOut, "Const-Fmt: %d %d\n", gcrvConstId, fmtId);
	gcrvFmtForConst[gcrvConstId] = fmtId;
}


local void
gcrProgSeq(Foam foam)
{
	FoamList stmts;
	Foam seq, newSeq, selectStmt;
	int yieldc, maxLabel, i, label, yieldno, mylabel;

	seq = foam->foamProg.body;
	yieldc = gcrCoroutineYieldCount(foam);
	maxLabel = gcrCoroutineMaxLabel(foam);

	selectStmt = foamNewEmpty(FOAM_Select, 1 + yieldc + 1);
	selectStmt->foamSelect.op = gcrStateVar();
	label = 10 * (maxLabel/10) + 10;

	stmts = listSingleton(Foam)(selectStmt);
	yieldno = 0;

	mylabel = label++;
	stmts = listCons(Foam)(foamNewLabel(mylabel), stmts);
	selectStmt->foamSelect.argv[yieldno++] = mylabel;

	for (i=0; i<foamArgc(seq); i++) {
		FoamTag tag;
		Foam stmt = seq->foamSeq.argv[i];
		Foam val;
		switch (foamTag(stmt)) {
		case FOAM_Yield:
			mylabel = label++;
			val = gcrProgTransformExpr(stmt->foamYield.value);
			//stmts = listCons(Foam)(gcrSetReturnValue(stmt->foamYield.value), stmts);
			stmts = listCons(Foam)(foamNewReturn(foamNew(FOAM_Values, 2,
								     foamNewSInt(yieldno),
								     val)),
					       stmts);
			stmts = listCons(Foam)(foamNewLabel(mylabel), stmts);
			selectStmt->foamSelect.argv[yieldno++] = mylabel;
			break;
		case FOAM_Return:
			stmts = listCons(Foam)(foamNewReturn(foamNew(FOAM_Values, 2, foamNewSInt(-1), foamNewCast(FOAM_Word, foamNewNil()))), stmts);
			break;
		case FOAM_Def:
		case FOAM_Set:
			stmts = listNConcat(Foam)(gcrProgTransformSet(gcrvProgState, stmt), stmts);
			break;
		default:
			stmts = listCons(Foam)(gcrProgTransformExpr(stmt), stmts);
			break;
		}
	}
	newSeq = foamNewOfList(FOAM_Seq, listNReverse(Foam)(stmts));
	assert(yieldno == yieldc + 1);
	foam->foamProg.body = newSeq;
}

/*
 * (Set (Values l1, l2, l3) (...))
 * -->
 * (Set (values (l1', l2', l3') (...)))
 * (Set tx(l1) l1')
 * (Set tx(l2) l2')
 * (Set tx(l3) l3')
 */
local FoamList
gcrProgTransformSet(GcrProgState state, Foam stmt)
{
	FoamList stmts, locals;
	FoamTag tag;
	Foam    setStmt;
	tag = foamTag(stmt);

	if (foamTag(stmt->foamSet.lhs) != FOAM_Values)
		return listSingleton(Foam)(gcrProgTransformExpr(stmt));

	locals = listNil(Foam);
	stmts  = listNil(Foam);
	for (int i=0; i<foamArgc(stmt->foamSet.lhs); i++) {
		Foam loc       = stmt->foamSet.lhs->foamValues.argv[i];
		assert(foamTag(loc) == FOAM_Loc);
		if (gcrProgStateIsNewLocal(gcrvProgState, loc->foamLoc.index)) {
			locals = listCons(Foam)(foamNewLoc(gcrProgStateNewLocalIdx(gcrvProgState, loc->foamLoc.index)), locals);
		}
		else {
			AInt newLocIdx = gcrProgStateAllocateLocal(state, loc->foamLoc.index, true);
			locals = listCons(Foam)(foamNewLoc(newLocIdx), locals);
			stmts = listCons(Foam)(foamNew(tag, 2, gcrProgTransformExpr(loc), foamNewLoc(newLocIdx)), stmts);
		}
	}
	locals = listNReverse(Foam)(locals);
	setStmt = foamNew(tag, 2, foamNewValuesOfList(locals), gcrProgTransformExpr(stmt->foamSet.rhs));

	return listNConcat(Foam)(stmts, listSingleton(Foam)(setStmt));
}

local Foam
gcrProgTransformExpr(Foam expr)
{
	foamIter(expr, arg, *arg = gcrProgTransformExpr(*arg));

	switch (foamTag(expr)) {
	case FOAM_Loc:
		return gcrProgStateIsNewLocal(gcrvProgState, expr->foamLoc.index)
			? foamNewLoc(gcrProgStateNewLocalIdx(gcrvProgState, expr->foamLoc.index))
			: foamNewRElt(gcrvRecLocFmtId, foamNewPar(1), gcrProgStateRecLocalIdx(gcrvProgState, expr->foamLoc.index));
	case FOAM_Gener:
		gcrvGenerList = listCons(Foam)(expr, gcrvGenerList);
		return expr;
	default:
		return expr;
	}
}

local Foam
gcrStateVar()
{
	return foamNewPar(int0);
}

local Foam
gcrSetReturnValue(Foam foam)
{
	return foam;
}

local int
gcrCoroutineYieldCount(Foam foam)
{
	return foamCountSubtreesOfKind(foam->foamProg.body, FOAM_Yield);
}

local int
gcrCoroutineMaxLabel(Foam foam)
{
	int i, maxLabel;
	maxLabel = 0;
	for (i=0; i<foamArgc(foam->foamProg.body); i++) {
		Foam stmt = foam->foamProg.body->foamSeq.argv[i];
		if (foamTag(stmt) == FOAM_Label) {
			AInt lbl = stmt->foamLabel.label;
			if (lbl > maxLabel) {
				maxLabel = lbl;
			}
		}
	}

	return maxLabel;
}

local AInt
gcrDDeclAdd(Foam foam)
{
	gcrvNewFormats = listCons(Foam)(foam, gcrvNewFormats);
	return gcrvFormatCount++;
}

local GcrProgState
gcrProgStateNew(Foam prog)
{
	AInt nLocals = foamDDeclArgc(prog->foamProg.locals);
	GcrProgState state = (GcrProgState) stoAlloc(OB_Other, sizeof(*state));
	state->newLocForOld = (AInt*) stoAlloc(OB_Other, nLocals * sizeof(AInt));
	state->recLocForOld = (AInt*) stoAlloc(OB_Other, nLocals * sizeof(AInt));
	state->oldLocals = prog->foamProg.locals;
	state->newLocals = listNil(Foam);
	state->recLocals = listNil(Foam);
	state->nNewLocals = 0;
	state->nRecLocals = 0;

	for (int i=0; i<nLocals; i++) {
		state->newLocForOld[i] = -1;
		state->recLocForOld[i] = -1;
	}
	return state;
}

local Bool
gcrProgStateIsNewLocal(GcrProgState state, AInt idx)
{
	return state->newLocForOld[idx] != -1;
}

local FoamList
gcrProgStateNewLocals(GcrProgState state)
{
	FoamList ll = listNReverse(Foam)(state->newLocals);
	state->newLocals = listNil(Foam);
	return ll;
}

local void
gcrProgStateFree(GcrProgState state)
{
	listFree(Foam)(state->newLocals);
	stoFree(state);
}

local AInt
gcrProgStateAllocateLocal(GcrProgState state, AInt oldIdx, Bool isTmp)
{
	Foam decl = foamCopy(state->oldLocals->foamDDecl.argv[oldIdx]);
	AInt id = state->nNewLocals;
	if (isTmp && !strIsEmpty(decl->foamDecl.id)) {
		decl->foamDecl.id = strPrintf("%s_tmp", decl->foamDecl.id);
	}
	state->newLocals = listCons(Foam)(decl, state->newLocals);
	state->nNewLocals++;
	return id;
}

local AInt
gcrProgStateAllocateRecLocal(GcrProgState state, AInt oldIdx)
{
	Foam decl = foamCopy(state->oldLocals->foamDDecl.argv[oldIdx]);
	AInt id = state->nRecLocals;
	state->recLocals = listCons(Foam)(decl, state->recLocals);
	state->nRecLocals++;
	return id;
}

local void
gcrProgStateSetNewLocal(GcrProgState state, AInt oldIdx, AInt newIdx)
{
	state->newLocForOld[oldIdx] = newIdx;
}

local void
gcrProgStateSetNewRecLocal(GcrProgState state, AInt oldIdx, AInt newIdx)
{
	state->recLocForOld[oldIdx] = newIdx;
}

local AInt
gcrProgStateNewLocalIdx(GcrProgState state, AInt oldIdx)
{
	return state->newLocForOld[oldIdx];
}

local AInt
gcrProgStateRecLocalIdx(GcrProgState state, AInt oldIdx)
{
	return state->recLocForOld[oldIdx];
}

