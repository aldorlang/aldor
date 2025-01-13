#include "debug.h"
#include "inlstate.h"
#include "flog.h"
#include "fptr.h"
#include "list.h"
#include "of_util.h"
#include "optinfo.h"
#include "store.h"
#include "strops.h"
#include "usedef.h"
#include "util.h"

/**
 * Inlining coroutines
 *
 * A coroutine looks like (Gener (Env 0) (Const 1))
 * ..
 * A call is
 *  (Set v (GenIter g P1 P2))
 *  (GenerStep label v) .. (GenerValue v)
 * ..
 * A coroutine is a prog with yields rather than return values.
 * Inlining:
 * find GenIter, and related steps
 * .. Count yields..
 * 0 => None
 * 1 => Boolean
 * 1 => Int
 *..
 *         (Set
 *           (Loc 2)
 *           (GenIter (Gener 0 (Nil) (Loc 3) (Const 5 xlistIter_coroutine))))
 *         (Label 0)
 *         (GenerStep 1 (Loc 2))
 *         (Set (Loc 1) (Cast SInt (GenerValue (Loc 2))))
 *         (Set (Loc 0 total) (BCall SIntPlus (Loc 0 total) (Loc 1)))
 *         (Goto 0)
 * ++
 *       (Seq
 *         (Yield (Cast Word (SInt 99)))
 *         (Return (Values)))))
 *
 *...
 *  Maybe merge graphs..
 */

typedef struct genCallInfo {
	AInt varId;
	Foam generSource; // GClos for generator
	FoamPtr genIter; // (Set xyz (GenIter () ...))
	AInt progId;
	Foam progCR;
	Foam envCR;
	FoamPtrList steps;
	FoamPtrList values;
	BBlockList  bbs;
} *GenCallInfo;

typedef struct crinInlinee {
	AInt yieldCount;
	AInt labelCount;
	Foam prog;
	InlEnvState env;
	InlSubstState substState;
} *CrinInlinee;

// "Stuff" needed for transformation - This assumes that there is a single 'GenerStep'
typedef struct crinTarget {
	Foam prog;
	Foam env;
	FoamPtr step;
	FoamPtr call;
	FoamPtrList values;
	AInt doneLabel;
	BBlock bb;
} *CrinTarget;

// How to go from target & inlinee to the final result
typedef struct crinResult {
	CrinTarget target;
	// 'input'
	AInt labelBase;
	AInt maxLabel;
	// newBits
	AInt tmpVarBase;
	AInt localBase;
	AInt resultVar;
	AInt stepVar;
	AInt outLabel;
	// How to xform target
	AInt nExtraLabels;
	FoamList newLocals;
	FoamList setupStmts;
	FoamList stmts;
	FoamPtrList doneRefs;
	Foam envRefs;
	// extra info
	Bool newGenerStep;
} *CrinResult;

typedef struct crinUnit {
	Foam unit;
} *CrinUnit;

Bool	crinDebug	= false;
#define crinDEBUG	DEBUG_IF(crin)		afprintf

DECLARE_LIST(GenCallInfo);
CREATE_LIST(GenCallInfo);

local void crinUnitInit(Foam unit);
local void crinUnitFini(Foam unit);
local Foam crinUnitFindConst(AInt constId);
local Foam crinDerefReference(Foam foam);
local Foam crinDerefGener(Foam foam);

local void crinProg(AInt idx, Foam prog);
local void crinProgInit(AInt idx, Foam prog);
local void crinProgFini(Foam prog);
local Bool crinInlineGenIter(Foam prog, GenCallInfo info);
local InlEnvState   crinInlineEnvState(GenCallInfo info);
local InlSubstState crinInlineSubstState(GenCallInfo info);
local Foam crinFindInlinee(Foam genIter);
local Bool crinInlineCoroutine(CrinTarget target, CrinInlinee inlinee);
local Bool crinTransformGraph(CrinTarget target, CrinInlinee inlinee);
local void crinSetCRVars(void);
local void crinSetEnvRefs(void);

local void crinTransformAddLocals(CrinInlinee inlinee);
local void crinTransformAddParams(CrinInlinee inlinee);
local AInt crinTransformLocalId(AInt index);
local void crinTransformSetEnv(CrinInlinee inlinee);
local void crinTransformBody(CrinTarget target, CrinInlinee inlinee);
local Foam crinTransformEElt(Foam foam);
local Foam crinTransformExpr(Foam expr);
local Foam crinTransformEnv(Foam lex);
local Foam crinTransformLex(Foam lex);
local Foam crinGetDecl(AInt format, AInt index);

local AInt crinAllocateLabels(AInt count);
local void crinAddStatement(Foam stmt);
local AInt crinLabelIndex(AInt labelBase, AInt index);
local AInt crinAllocateLocal(Foam foam);
local void crinAddSetupStatement(Foam foam);
local void crinEnvSetUsage(AInt level, AInt format);

local void crinApplyResult(CrinTarget target, CrinResult result);
local void crinApplyResultEnvRefs(CrinTarget target, CrinResult result);

local GenCallInfoList crinBuildCandidates(GenCallInfoList list);
local GenCallInfoList crinProgFindCandidates(Foam prog);

local CrinResult crinNewResult(CrinTarget target);
local void       crinResultAddStatement(CrinResult result, Foam stmt);
local void       crinResultAddSetupStatement(CrinResult result, Foam stmt);
local AInt       crinResultAllocateLocal(CrinResult result, Foam stmt);
local void       crinResultEnvSetUsage(CrinResult result, AInt level, AInt format);

local GenCallInfo crgcInfoNew (AInt varId);
local void        crgcInfoFree(GenCallInfo info);
local void crgcAddStep(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates);
local void crgcAddValue(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates);
local void crgcAddIter(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates);

local GenCallInfo crgcFindRef(GenCallInfoList lst, AInt id);

local GenCallInfo crinProgFillVar(GenCallInfo info);
local Foam 	  crinProgFind(GenCallInfo info);

local CrinUnit crinvUnit;
local CrinResult crinvResult;
local CrinInlinee crinvInlinee;
local PtrSet crinvFindContext;
local AInt   crinvProgId;

void
crinUnit(Foam unit)
{
	Foam defs, def;
	int i;

	crinDEBUG(dbOut, "(Starting unit\n");
	crinUnitInit(unit);

	defs = unit->foamUnit.defs;

	for (i=0; i<foamArgc(defs); i++) {
		def = defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		crinDEBUG(dbOut, "(Starting const %d...\n", i);
		crinDEBUG(dbOut, " Prog %d\n%pFoamTypes\n", i, def->foamDef.rhs);
		crinProg(i, def->foamDef.rhs);
		crinDEBUG(dbOut, "Finished const %d)\n", i);
	}
	crinDEBUG(dbOut, "After---->\n");
	crinDEBUG(dbOut, "%pFoamTypes\n", unit);
	crinDEBUG(dbOut, "Unit complete)\n");

	crinUnitFini(unit);
	assert(foamAudit(unit));
}

local void
crinUnitInit(Foam unit)
{
	crinvUnit = (CrinUnit) stoAlloc(OB_Other, sizeof(*crinvUnit));
	crinvUnit->unit = unit;
}

local void
crinUnitFini(Foam unit)
{
	stoFree(crinvUnit);
	crinvUnit = NULL;
}

local Foam
crinUnitFindConst(AInt constId)
{
	AInt i;
	Foam defs = crinvUnit->unit->foamUnit.defs;
	for (i=0; i<foamArgc(defs); i++) {
		Foam lhs = defs->foamDDef.argv[i]->foamDef.lhs;
		Foam rhs = defs->foamDDef.argv[i]->foamDef.rhs;
		if (foamTag(lhs) == FOAM_Const && lhs->foamConst.index == constId) {
			return rhs;
		}
	}
	return NULL;
}


local void
crinProg(AInt constId, Foam prog)
{
	GenCallInfoList candidates;
	FlowGraph flog;
	Bool more, chk;
	int count = 0, inl = 0;

	crinProgInit(constId, prog);


	more = true;
	while (more) {
		flog = flogFrProg(prog, FLOG_UniqueExit);
		chk = usedefChainsFrFlog(flog, UD_OUTPUT_SinglePointer);
		flogToProg(flog);

		more = false;
		candidates = crinProgFindCandidates(prog); // Maybe add priority queue here

		crinDEBUG(dbOut, "Found %d candidates (round %d) - Usedef: %d\n", listLength(GenCallInfo)(candidates), count, chk);

		candidates = crinBuildCandidates(candidates);
		crinDEBUG(dbOut, ".. filtered to %d candidates\n", listLength(GenCallInfo)(candidates));
		while (candidates != listNil(GenCallInfo)) {
			more = more || crinInlineGenIter(prog, car(candidates));
			crgcInfoFree(car(candidates));
			candidates = cdr(candidates);
			inl++;
		}
		prog->foamProg.body = utilMakeFlatSeq(prog->foamProg.body);
		count++;

		// Hard cut-off on prog size
		if (foamArgc(prog->foamProg.body) > 25)
			more = false;
	}
	if (inl > 0) {
		foamOptInfo(prog)->inlState = INL_NotInlined;
	}

	crinDEBUG(dbOut, "Finished on %d rounds\n", count);
	crinProgFini(prog);
}

local void
crinProgInit(AInt progId, Foam prog)
{
	crinvFindContext = psetNew();
	crinvProgId = progId;
}

local void
crinProgFini(Foam prog)
{
	// Check for leaks..
	assert(psetIsEmpty(crinvFindContext));
	crinvFindContext = NULL;
}

local GenCallInfoList
crinBuildCandidates(GenCallInfoList list)
{
	GenCallInfoList newList = listNil(GenCallInfo);
	// Filtered priority queue
	while (list != listNil(GenCallInfo)) {
		GenCallInfo candidate = car(list);
		list = listFreeCons(GenCallInfo)(list);
		if (candidate == NULL) {
			continue;
		}
		else if (candidate->genIter == NULL) {
			crgcInfoFree(candidate);
			continue;
		}
		else if (candidate->progId == crinvProgId) {
			crgcInfoFree(candidate);
			continue;
		}
		else if (listLength(FoamPtr)(candidate->steps) > 1) {
			// TODO: Deal with #steps > 1
			crgcInfoFree(candidate);
			continue;
		}
		newList = listCons(GenCallInfo)(candidate, newList);
	}
	return listNReverse(GenCallInfo)(newList);
}

local CrinTarget
crinNewTarget(Foam prog)
{
	CrinTarget target = (CrinTarget) stoAlloc(OB_Other, sizeof(*target));
	target->prog = prog;
	target->env = NULL;
	target->step = NULL;
	target->call = NULL;
	target->values = listNil(FoamPtr);
	target->bb = NULL;
	return target;
}

local CrinInlinee
crinNewInlinee(GenCallInfo info, InlEnvState env, InlSubstState substState)
{
	CrinInlinee inlinee = (CrinInlinee) stoAlloc(OB_Other, sizeof(*inlinee));
	inlinee->yieldCount = foamCountSubtreesOfKind(info->progCR->foamProg.body, FOAM_Yield);
	inlinee->labelCount = info->progCR->foamProg.nLabels;
	inlinee->prog = info->progCR;
	inlinee->env = env;
	inlinee->substState = substState;
	return inlinee;
}

local Bool
crinInlineGenIter(Foam prog, GenCallInfo info)
{
	Foam genClos, env;

	genClos = info->generSource;

	env  = genClos->foamGener.env;

	CrinTarget target  = crinNewTarget(prog);
	target->step = car(info->steps);
	target->call = info->genIter;
	target->doneLabel = fptrVal(target->step)->foamGenerStep.label;
	target->values = info->values;
	target->bb = car(info->bbs);
	InlEnvState envState = crinInlineEnvState(info);
	InlSubstState substState = crinInlineSubstState(info);
	CrinInlinee inlinee = crinNewInlinee(info, envState, substState);
	return crinInlineCoroutine(target, inlinee);
}

local InlEnvState
crinInlineEnvState(GenCallInfo info)
{
	Foam prog = info->progCR;
	Foam levels = prog->foamProg.levels;
	Foam env1;
	Bool hasEnv0;

	hasEnv0 = true;
	env1 = info->envCR;
	if (levels->foamDEnv.argv[0] == emptyFormatSlot
	    || levels->foamDEnv.argv[0] == envUsedSlot) {
		hasEnv0 = false;
	}

	return inlEnvNew(levels, hasEnv0, env1);
}

local InlSubstState
crinInlineSubstState(GenCallInfo info)
{
	Foam gener = info->generSource;

	return inlSubstStateNew(foamSyme(gener->foamGener.env));
}

local Bool
crinInlineCoroutine(CrinTarget target, CrinInlinee inlinee)
{
	// Set up parameters
	// Move prog into "this" environment
	return crinTransformGraph(target, inlinee);
	// clean up loose ends
}

local Bool
crinTransformGraph(CrinTarget target, CrinInlinee inlinee)
{
	Bool moreToDo;
	Foam stmt;
	int i;

	// 1. Replace GenIter with vars
	//    - Step number
	//    - Done flag
	//    - Return value

	// Replace inlinee code,
	//   -- adding initial select/step thing
	//   -- Yield => Assignment to return var
	//   -- Return => branch to out of here

	crinDEBUG(dbOut, "Inline prog\n  %pFoamTypes\n", inlinee->prog);
	CrinResult result = crinNewResult(target);
	crinvResult = result;
	crinvInlinee = inlinee;

	crinSetCRVars();
	crinSetEnvRefs();
	crinTransformAddParams(inlinee);
	crinTransformAddLocals(inlinee);
	crinTransformSetEnv(inlinee);
	crinTransformBody(target, inlinee);

	crinDEBUG(dbOut, "Transformed\n");
	crinDEBUG(dbOut, "  (Seq\n");
	listIter(Foam, stmt, listReverse(Foam)(result->setupStmts), {
			crinDEBUG(dbOut, "  %pFoamTypes\n", stmt);
		});
	crinDEBUG(dbOut, ")\n");
	crinDEBUG(dbOut, "Code\n (Seq\n");
	listIter(Foam, stmt, listReverse(Foam)(result->stmts), {
			crinDEBUG(dbOut, "  %pFoamTypes\n", stmt);
		});
	crinDEBUG(dbOut, ")\n");

	crinApplyResult(target, result);

	moreToDo = result->newGenerStep;

	crinvResult = NULL;
	crinvInlinee = NULL;

	return moreToDo;
}

local void
crinApplyResult(CrinTarget target, CrinResult result)
{
	Foam prog = target->prog;
	// Add Locals
	Foam oldLocals = prog->foamProg.locals;
	AInt sz = foamDDeclArgc(oldLocals) + listLength(Foam)(result->newLocals);
	Foam decls = foamNewDDeclEmpty(sz, oldLocals->foamDDecl.usage);
	int idx = 0;

	for (;idx < foamDDeclArgc(oldLocals); idx++) {
		decls->foamDDecl.argv[idx] = oldLocals->foamDDecl.argv[idx];
	}
	listIter(Foam, var, listNReverse(Foam)(result->newLocals), {
			decls->foamDDecl.argv[idx++] = var;
		});
	prog->foamProg.locals = decls;

	// Sort environment
	crinApplyResultEnvRefs(target, result);

	// Replace call with setup (nb: We will flatten the whole later)
	Foam seq = foamNewOfList(FOAM_Seq, listNReverse(Foam)(result->setupStmts));
	fptrSet(target->call, seq);

	// Stomp in variable references
	listIter(FoamPtr, valueRef, target->values, {
			fptrSet(valueRef, foamNewLoc(result->resultVar));
		});
	// Replace GenerStep with goto
	if (target->bb != NULL) {
		BBlock bb = target->bb;
		bb->kind = FOAM_Goto;
		bbSetExitC(bb, 1);
		Foam outLabel = foamNewLabel(result->outLabel);
		Foam nextGoto = foamNewGoto(bbExit(bb, 0));
		result->stmts = listCons(Foam)(outLabel, result->stmts);
		result->stmts = listCons(Foam)(nextGoto, result->stmts);
	}
	else {
		Foam outLabel = foamNewLabel(result->outLabel);
		result->stmts = listCons(Foam)(outLabel, result->stmts);
	}

	FoamList whole = listNReverse(Foam)(result->stmts);
	fptrSet(target->step, foamNewSeqOfList(whole));
	// Clean up nested seq
	//prog->foamProg.body = utilMakeFlatSeq(prog->foamProg.body);
	prog->foamProg.nLabels = result->maxLabel + 1;


	if (target->bb != NULL) {
		flogPrintDb(target->bb->graph);
	}
	else {
		crinDEBUG(dbOut, "Applied:\n%pFoamTypes\n", prog);
	}
}

local void
crinSetCRVars()
{
	// Return var - FIXME: Type
	Foam retDecl = foamNewDecl(FOAM_Word, strCopy("crRet"), emptyFormatSlot);
	crinvResult->tmpVarBase = foamDDeclArgc(crinvResult->target->prog->foamProg.locals);
	crinvResult->resultVar  = crinAllocateLocal(retDecl);
	// Step Var
	Foam stepDecl  = foamNewDecl(FOAM_SInt, strCopy("step"), emptyFormatSlot);
	crinvResult->stepVar = crinAllocateLocal(stepDecl);
	// outLabel
	crinvResult->outLabel  = crinAllocateLabels(1);

	crinAddSetupStatement(foamNewSet(foamNewLoc(crinvResult->stepVar), foamNewSInt(0)));
}

local void
crinSetEnvRefs()
{
	crinvResult->envRefs = foamNewDEnvUnused(foamDEnvArgc(crinvResult->target->prog->foamProg.levels));
}

local void
crinTransformAddParams(CrinInlinee inlinee)
{
}

local void
crinTransformAddLocals(CrinInlinee inlinee)
{
	// Fixme: crinvAddLocalsDDecl
	Foam locals = inlinee->prog->foamProg.locals;
	crinvResult->localBase = crinvResult->tmpVarBase;
	for (int i=0; i<foamDDeclArgc(locals); i++) {
		crinAllocateLocal(foamCopy(locals->foamDDecl.argv[i]));
	}
}

local void
crinTransformSetEnv(CrinInlinee inlinee)
{
	AInt fmt, var;

	if (!inlEnvHasEnv0(inlinee->env)) {
		return;
	}

	fmt = inlinee->prog->foamProg.levels->foamDEnv.argv[0];
	var = crinAllocateLocal(foamNewDecl(FOAM_Env, strCopy("e"), emptyFormatSlot));
	inlEnvSetEnv0(inlinee->env, foamNewLoc(var));
	crinAddSetupStatement(foamNewSet(foamNewLoc(var), foamNewPushEnv(fmt, foamCopy(inlinee->env->env1))));
}

local void
crinApplyResultEnvRefs(CrinTarget target, CrinResult result)
{
	Foam prog   = target->prog;
	Foam levels = prog->foamProg.levels;
	AInt i;
	for (i=0; i<foamArgc(levels); i++) {
		AInt newUsage = result->envRefs->foamDEnv.argv[i];
		AInt oldUsage = levels->foamDEnv.argv[i];
		if (newUsage != emptyFormatSlot && oldUsage != newUsage) {
			if (oldUsage == emptyFormatSlot || oldUsage == envUsedSlot) {
				levels->foamDEnv.argv[i] = newUsage;
			}
			else {
				bug("usage update error");
			}
		}
	}
}


local void
crinTransformBody(CrinTarget target, CrinInlinee inlinee)
{
	Foam seq, control;
	AInt labelBase;
	AInt yieldBase;
	AInt stepIdx;

	yieldBase = crinAllocateLabels(1+inlinee->yieldCount);
	labelBase = crinAllocateLabels(inlinee->labelCount);
	stepIdx = 0;
	crinAddStatement(foamNewSelectRange(foamNewLoc(crinvResult->stepVar),
					    yieldBase,
					    1 + inlinee->yieldCount));
	crinAddStatement(foamNewLabel(yieldBase + stepIdx));
	stepIdx++;

	seq = inlinee->prog->foamProg.body;
	assert(foamTag(seq) == FOAM_Seq);

	for (int i=0; i<foamArgc(seq); i++) {
		Foam foam = seq->foamSeq.argv[i];
		switch (foamTag(foam)) {
		case FOAM_Label:
			crinAddStatement(foamNewLabel(crinLabelIndex(labelBase, foam->foamLabel.label)));
			break;
		case FOAM_Goto:
			crinAddStatement(foamNewGoto(crinLabelIndex(labelBase, foam->foamLabel.label)));
			break;
		case FOAM_GenerStep:
			crinAddStatement(foamNewGenerStep(crinLabelIndex(labelBase, foam->foamGenerStep.label),
							  crinTransformExpr(foamCopy(foam->foamGenerStep.gener))));
			break;
		case FOAM_If:
			crinAddStatement(foamNewIf(crinTransformExpr(foamCopy(foam->foamIf.test)),
						   crinLabelIndex(labelBase, foam->foamIf.label)));
			break;
		case FOAM_Select: {
			Foam select = foamNewSelect(crinTransformExpr(foamCopy(foam->foamSelect.op)), foamSelectArgc(foam));
			int i;
			for (i=0; i<foamSelectArgc(select); i++)
				select->foamSelect.argv[i] = crinLabelIndex(labelBase, foam->foamSelect.argv[i]);
			crinAddStatement(select);
			break;
		}
		case FOAM_Yield: {
			crinAddStatement(foamNewSet(foamNewLoc(crinvResult->resultVar),
						    crinTransformExpr(foamCopy(foam->foamYield.value))));
			crinAddStatement(foamNewSet(foamNewLoc(crinvResult->stepVar),
						    foamNewSInt(stepIdx)));
			crinAddStatement(foamNewGoto(crinvResult->outLabel));
			crinAddStatement(foamNewLabel(yieldBase + stepIdx));
			stepIdx++;
			break;
			}
		case FOAM_Return:
			crinAddStatement(foamNewGoto(target->doneLabel));
			break;
		default:
			crinAddStatement(crinTransformExpr(foamCopy(foam)));
		}
	}
	crinvResult = NULL;
}

local AInt
crinAllocateLocal(Foam foam)
{
	return crinResultAllocateLocal(crinvResult, foam);
}

local void
crinAddSetupStatement(Foam foam)
{
	crinResultAddSetupStatement(crinvResult, foam);
}

local void
crinAddStatement(Foam foam)
{
	crinResultAddStatement(crinvResult, foam);
}

local AInt
crinLabelIndex(AInt labelBase, AInt index)
{
	// FIXME: Are labels contiguous?
	return labelBase + index;
}

local AInt
crinAllocateLabels(AInt count)
{
	// FIXME: Need 'result' specific version
	AInt label0 = crinvResult->maxLabel;
	crinvResult->maxLabel += count;
	return label0;
}


local Foam
crinTransformExpr(Foam expr)
{
	foamIter(expr, arg, *arg = crinTransformExpr(*arg));

	switch (foamTag(expr)) {
	case FOAM_Loc:
		return foamNewLoc(crinTransformLocalId(expr->foamLoc.index));
	case FOAM_Lex:
		return crinTransformLex(expr);
	case FOAM_Env:
		return crinTransformEnv(expr);
	case FOAM_EElt:
		return crinTransformEElt(expr);
	default:
		return expr;
	}
}

local Foam
crinTransformLex(Foam lex)
{
	Foam foam = inlEnvTransformLex(crinvInlinee->env, lex);
	if (foamSyme(lex) != NULL) {
		Syme syme = inlSubstStateSyme(crinvInlinee->substState, foamSyme(lex));
		if (syme != NULL)
			foamSyme(foam) = syme;
	}
	else {
		AInt format = foamProgFormatForLevel(crinvInlinee->prog, lex->foamLex.level);
		AInt index = lex->foamLex.index;
		Foam decl = crinGetDecl(format, index);
		Syme syme = foamSyme(decl);
		if (syme != NULL)
			syme = inlSubstStateSyme(crinvInlinee->substState, syme);
		foamSyme(foam) = syme;
	}
	if (foamTag(foam) == FOAM_Lex) {
		crinEnvSetUsage(foam->foamLex.level, foamProgFormatForLevel(crinvInlinee->prog, lex->foamLex.level));
	}
	return foam;
}

local Foam
crinTransformEElt(Foam foam)
{
	if (foamSyme(foam) != NULL) {
		Syme syme = inlSubstStateSyme(crinvInlinee->substState, foamSyme(foam));
		if (syme != NULL)
			foamSyme(foam) = syme;
	}
	else {
		AInt index = foam->foamEElt.lex;
		Foam decl = crinGetDecl(foam->foamEElt.env, index);
		Syme syme = foamSyme(decl);
		foamSyme(foam) = syme;
	}
	return foam;
}

local Foam
crinGetDecl(AInt format, AInt index)
{
	Foam ddecl = crinvUnit->unit->foamUnit.formats->foamDFmt.argv[format];
	assert(index < foamDDeclArgc(ddecl));
	return ddecl->foamDDecl.argv[index];
}


local void
crinEnvSetUsage(AInt level, AInt format)
{
	crinResultEnvSetUsage(crinvResult, level, format);
}

local Foam
crinTransformEnv(Foam env)
{
	env = inlEnvTransformEnv(crinvInlinee->env, env);
	return env;
}


local AInt
crinTransformLocalId(AInt id)
{
	return crinvResult->localBase + id;
}

local CrinResult
crinNewResult(CrinTarget target)
{
	CrinResult res = (CrinResult) stoAlloc(OB_Other, sizeof(*res));
	res->target     = target;
	res->tmpVarBase = -1;
	res->localBase  = -1;
	res->labelBase  = -1;
	res->maxLabel   = target->prog->foamProg.nLabels;
	res->stepVar    = 0;
	res->resultVar  = 0;
	res->nExtraLabels = 0;
	res->newLocals  = listNil(Foam);
	res->setupStmts = listNil(Foam);
	res->stmts      = listNil(Foam);
	res->newGenerStep = false;
	res->envRefs      = NULL;
	return res;
}


local AInt
crinResultAllocateLocal(CrinResult result, Foam decl)
{
	AInt idx = result->tmpVarBase;
	result->newLocals = listCons(Foam)(decl, result->newLocals);
	result->tmpVarBase++;
	return idx;
}

local void
crinResultAddSetupStatement(CrinResult result, Foam stmt)
{
	result->setupStmts = listCons(Foam)(stmt, result->setupStmts);
}

local void
crinResultAddStatement(CrinResult result, Foam stmt)
{
	if (foamTag(stmt) == FOAM_GenerStep) {
		result->newGenerStep = true;
	}
	result->stmts = listCons(Foam)(stmt, result->stmts);
}

local void
crinResultEnvSetUsage(CrinResult result, AInt level, AInt format)
{
	AInt oldUsage = result->envRefs->foamDEnv.argv[level];
	if (oldUsage == format)
		return;
	assert(oldUsage == emptyFormatSlot || oldUsage == envUsedSlot);
	result->envRefs->foamDEnv.argv[level] = format;
}

/*
 * :: Identifying GenIter calls
 *
 * Iterates over the current flow graph and finds calls to
 * GenIter (or similar).
 * Also adds Step calls..
 */

local void crinProgFindCandidatesExpr(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates);

GenCallInfoList
crinProgFindCandidates(Foam foam)
{
	GenCallInfoList ll = listNil(GenCallInfo);
	FlowGraph flog;
	flog = foamOptInfo(foam) == NULL ? NULL : foamOptInfo(foam)->flog;

	if (flog != NULL) {
		flogIter(flog, bb, {
				foamIter(bb->code, expr,
					 crinProgFindCandidatesExpr(fptrNew(crinvFindContext,
									    bb->code, _i),
								    bb,
								    &ll));
			});
	}
	else {
		foamIter(foam->foamProg.body, expr,
			 crinProgFindCandidatesExpr(fptrNew(crinvFindContext, foam->foamProg.body, _i), NULL, &ll));
	}
	listNMap(GenCallInfo)(crinProgFillVar, ll);
	return ll;
}


local GenCallInfo
crinProgFillVar(GenCallInfo info)
{
	Foam var = crinProgFind(info);
	if (var == NULL) {
		crgcInfoFree(info);
		return NULL;
	}

	foamDereferenceCast(var);

	if (foamTag(var) != FOAM_Gener) {
		crgcInfoFree(info);
		return NULL;
	}
	if (foamTag(var->foamGener.prog) != FOAM_Const) {
		crgcInfoFree(info);
		return NULL;
	}
	info->generSource = var;
	info->progId = var->foamGener.prog->foamConst.index;
	info->progCR = crinUnitFindConst(var->foamGener.prog->foamConst.index);
	if (info->progCR->foamProg.body == NULL) {
		info->progCR = flogToProg(foamOptInfo(info->progCR)->flog);
	}
	info->envCR = var->foamGener.env;
	if (info->progCR == NULL) {
		crgcInfoFree(info);
		return NULL;
	}
	return info;
}

local Foam
crinProgFind(GenCallInfo info)
{
	FoamPtr genIter = info->genIter;
	Foam rhs, g;
	if (genIter == NULL) {
		return NULL;
	}
	rhs = fptrVal(genIter)->foamSet.rhs;
	assert(foamTag(rhs) == FOAM_GenIter);

	g = rhs->foamGenIter.gener;
	foamDereferenceCast(g);

	if (foamTag(g) == FOAM_Loc || foamTag(g) == FOAM_Lex) {
	}
	if (foamTag(g) == FOAM_Gener) {
		return g;
	}

	// Next: Look at flow graph...
	Foam ref = crinDerefGener(g);
	if (ref == NULL) {
		crinDEBUG(dbOut, "Failed to find ref for %pFoam\n", fptrVal(genIter));
		return NULL;
	}

	crinDEBUG(dbOut, "Switched %pFoam --> %pFoam\n", g, ref);
	foamDereferenceCast(ref); // NB: Cancel casts nicely
	return ref;
}

local Foam
crinDerefGener(Foam foam)
{
	switch (foamTag(foam)) {
	case FOAM_Gener:
		return foam;
	case FOAM_Cast: {
		Foam inner = crinDerefGener(foam->foamCast.expr);
		if (inner == NULL)
			return NULL;
		else
			return foamNewCast(foam->foamCast.type, inner);
	}
	case FOAM_Loc:
	case FOAM_Lex:
		return crinDerefReference(foam);
	default:
		return NULL;
	}
}

local Foam
crinDerefReference(Foam foam)
{
	Foam ref;
	if (foamOptInfo(foam) == NULL)
		return NULL;
	ref = (Foam) foam->foamGen.hdr.info.defList;
	if (ref == NULL)
		return NULL;
	switch (foamTag(ref)) {
	case FOAM_Def:
	case FOAM_Set:
		return crinDerefGener(ref->foamDef.rhs);
	default:
		return NULL;
	}
}

local void
crinProgFindCandidatesExpr(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates)
{
	Foam expr = fptrVal(ptr);

	foamIter(expr, subexpr, crinProgFindCandidatesExpr(fptrNew(crinvFindContext, expr, _i), bb, candidates));

	switch (foamTag(expr)) {
	case FOAM_Def:
	case FOAM_Set:
		if (foamTag(expr->foamSet.rhs) == FOAM_GenIter) {
			crgcAddIter(ptr, bb, candidates);
		}
		else {
			fptrFree(ptr);
		}
		break;
	case FOAM_GenerValue:
		crgcAddValue(ptr, bb, candidates);
		break;
	case FOAM_GenerStep:
		crgcAddStep(ptr, bb, candidates);
		break;
	default:
		fptrFree(ptr);
	}
}

local void
crgcAddIter(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates)
{
	GenCallInfo info;
	GenCallInfoList ll;
	Foam expr;
	Foam var;
	AInt varId;

	expr = fptrVal(ptr);
	assert(foamTag(expr) == FOAM_Set || foamTag(expr) == FOAM_Def);
	if (foamTag(expr->foamSet.lhs) != FOAM_Loc) {
		fptrFree(ptr);
		return;
	}
	var = expr->foamSet.lhs;
	varId = var->foamLoc.index;
	info = crgcFindRef(*candidates, varId);

	if (info == NULL) {
		info = crgcInfoNew(varId);
		*candidates = listCons(GenCallInfo)(info, *candidates);
	}

	assert(info->genIter == NULL);
	info->genIter = ptr;
}

local void
crgcAddValue(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates)
{
	Foam expr;
	AInt varId;

	expr = fptrVal(ptr);
	assert(foamTag(expr) == FOAM_GenerValue);
	varId = expr->foamGenerValue.gener->foamLoc.index;

	GenCallInfo info = crgcFindRef(*candidates, varId);
	if (info == NULL) {
		info = crgcInfoNew(varId);
		*candidates = listCons(GenCallInfo)(info, *candidates);
	}
	info->values = listCons(FoamPtr)(ptr, info->values);
}

local void
crgcAddStep(FoamPtr ptr, BBlock bb, GenCallInfoList *candidates)
{
	Foam expr;
	AInt varId;
	expr = fptrVal(ptr);

	assert(foamTag(expr) == FOAM_GenerStep);
	varId = expr->foamGenerStep.gener->foamLoc.index;

	GenCallInfo info = crgcFindRef(*candidates, varId);
	if (info == NULL) {
		info = crgcInfoNew(varId);
		*candidates = listCons(GenCallInfo)(info, *candidates);
	}
	info->bbs   = listCons(BBlock)(bb, info->bbs);
	info->steps = listCons(FoamPtr)(ptr, info->steps);
}

local GenCallInfo
crgcInfoNew(AInt varId)
{
	GenCallInfo info = (GenCallInfo) stoAlloc(OB_Other, sizeof(*info));
	info->varId = varId;
	info->generSource = NULL;
	info->genIter     = NULL;
	info->progCR      = NULL;
	info->envCR       = NULL;
	info->bbs     = listNil(BBlock);
	info->steps   = listNil(FoamPtr);
	info->values  = listNil(FoamPtr);

	return info;
}

local void
crgcInfoFree(GenCallInfo info)
{
	listIter(FoamPtr, step, info->steps, fptrFree(step));
	listIter(FoamPtr, val, info->values, fptrFree(val));
	if (info->genIter != NULL)
		fptrFree(info->genIter);
	stoFree(info);
}


local GenCallInfo
crgcFindRef(GenCallInfoList lst, AInt id)
{
	while (lst != listNil(GenCallInfo)) {
		GenCallInfo inf = car(lst);
		lst = cdr(lst);
		if (inf->varId == id) {
			return inf;
		}
	}
	return NULL;
}

