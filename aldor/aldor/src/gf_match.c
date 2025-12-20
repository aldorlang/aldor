#include "absyn.h"
#include "fluid.h"
#include "foam.h"
#include "gf_match.h"
#include "gf_match0.h"
#include "gf_prog.h"
#include "gf_special.h"
#include "gf_util.h"
#include "tform.h"
#include "util.h"


/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

Bool	genfMatchDebug	= false;
#define gendfMatchDEBUG	DEBUG_IF(genfMatch)	afprintf


/*****************************************************************************
 *
 * :: Matching things
 *
 ****************************************************************************/
local void gfm0MatchApply	(AbSyn ab, Foam lhs);
local void gfm0MatchDefine	(AbSyn ab, Foam lhs);
local void gfm0MatchBlank	(AbSyn ab, Foam lhs);
local void gfm0MatchComma	(AbSyn ab, Foam lhs);
local Foam gfm0PPartialHasFailed(Foam foam);

static int gfmOutLabel;

void
gfmSelectMatch(AbSyn ab, Foam lhs, AInt trueLabel, AInt falseLabel)
{
	Scope("genSelectMatch");
	int fluid(gfmOutLabel);
	int endLabel;
	
	endLabel = gen0State->labelNo++;
	gfmOutLabel = falseLabel;
	gfm0MatchExpr(ab, lhs);
	gen0AddStmt(foamNewGoto(trueLabel), ab);
	Return(Nothing);
}

Foam
gfm0MatchExit()
{
	return foamNewGoto(gfmOutLabel);
}

AInt
gfm0MatchExitLabel()
{
	return gfmOutLabel;
}

Foam
gfmCaseMatch(AbSyn ab, Foam lhs)
{
	Scope("genMatch");
	int endLabel;
	int fluid(gfmOutLabel);
	Foam tmp = gen0TempLocal(FOAM_Bool);
	endLabel = gen0State->labelNo++;
	gfmOutLabel = gen0State->labelNo++;
	gfm0MatchExpr(ab, lhs);
	gen0AddStmt(foamNewSet(foamCopy(tmp), foamNewBool(true)), ab);
	gen0AddStmt(foamNewGoto(endLabel), ab);
	gen0AddStmt(foamNewLabel(gfmOutLabel), ab);
	gen0AddStmt(foamNewSet(foamCopy(tmp), foamNewBool(false)), ab);
	gen0AddStmt(foamNewLabel(endLabel), ab);

	Return(foamNewCast(FOAM_Word, tmp));
}

// Search the pattern.. Find vars to assign
// PatMatch(X, T) == (fn: T -> Partial X, final: X -> ())

void
gfm0MatchExpr(AbSyn ab, Foam lhs)
{
	Foam ret;
	switch (abTag(ab)) {
	case AB_Blank:
		gfm0MatchBlank(ab, lhs);
		break;
	case AB_Apply:
		gfm0MatchApply(ab, lhs);
		break;
	case AB_Comma:
		gfm0MatchComma(ab, lhs);
		break;
	case AB_Define:
		gfm0MatchDefine(ab, lhs);
		break;
	default:
		bug("not reached");
	}
}

local void
gfm0MatchDefine(AbSyn ab, Foam lhs)
{
	gfm0MatchExpr(ab->abDefine.rhs, lhs);
}

local void
gfm0MatchComma(AbSyn ab, Foam lhs)
{
	// ToDo: Games with default parameters, etc
	AInt i;
	assert(foamTag(lhs) == FOAM_Values);
	for (i=0; i<abCommaArgc(ab); i++) {
		gfm0MatchExpr(ab->abComma.argv[i], lhs->foamValues.argv[i]);
	}
}


local void
gfm0MatchApply(AbSyn ab, Foam lhs)
{
	AbSyn op = ab->abApply.op;
	TForm patTf = gen0AbContextType(op);
	Foam *argv, content;
	int i;
	// Maybe op is a pattern
	if (false /*abIsPattern(op) */) {
		bug("not yet");
	}
	assert(tfIsPatMatch(patTf));

	if (abTag(op) == AB_Id && symeIsSpecial(abSyme(op))) {
		gfm0SpecialMatch(abSyme(op), ab, lhs);
	}
	else {
		AInt retFmt = gen0RecordFormatNumber(tfMapArg(patTf));
		// NB: Need to consider default arguments, etc
		assert(abApplyArgc(ab) == tfMapArgc(patTf));
		Foam patFn = genFoamVal(op);
		Foam tmp = gen0Temp(FOAM_Rec);
		assert(tfMapRetc(patTf) == 1);
		
		gen0AddStmt(foamNewSet(foamCopy(tmp), foamNewCast(FOAM_Rec, foamNewCCall(FOAM_Word, patFn, lhs, NULL))), NULL);
		gen0AddStmt(foamNewIf(gfm0PPartialHasFailed(foamCopy(tmp)), gfm0MatchExitLabel()), NULL);
		
		if (abApplyArgc(ab) == 0) {
			argv = NULL;
		}

		Foam temps = foamNewEmpty(FOAM_Values, abApplyArgc(ab));
		for(i = 0; i < abApplyArgc(ab); i++) {
			FoamTag tag;
			AInt    fmt;
			tag = gen0Type(tfMapArgN(patTf, i), &fmt);
			temps->foamValues.argv[i] = gen0TempLocal0(tag, fmt);
		}
		for(i = 0; i < abApplyArgc(ab); i++) {
			gen0AddStmt(foamNewSet(temps->foamValues.argv[i], foamNewRElt(retFmt, foamCopy(tmp), i)), ab);
			argv = temps->foamValues.argv;
			assert(abApplyArgc(ab) == tfMapArgc(patTf));
		}
		
		for (int i=0; i<abApplyArgc(ab); i++) {
			AbSyn abi = abApplyArgv(ab)[i];
			gfm0MatchExpr(abi, argv[i]);
		}
	}
}

local void
gfm0ApplyResultToArgs(Syme syme, AbSyn absyn, Length argc, Foam *argv, TForm patTf)
{
	for (int i=0; i<tfMapArgc(patTf); i++) {
		AInt    fmt, newFmt;
		FoamTag exprType = gen0Type(tfMapArgN(patTf, i), &fmt);
		/*
		FoamTag paramType = gen0TfMapType(syme, patTf, exprType, &newFmt);
		AbSyn abi = tfMapSelectArg(patTf, absyn, i);
		assert(abi);
		argv[i] = foamCastIfNeeded(paramType, exprType, genFoamVal(abi));
		*/
	}
}

local Foam
gfm0PPartialHasFailed(Foam foam)
{
	// TODO: Create foamNewBCall..
	return foamNew(FOAM_BCall, 2, FOAM_BVal_PtrIsNil, foam);
}

/*
local void
gfm0MatchUnion(TForm key, AbSyn ab, Foam lhs)
{
	AInt format = gen0MakeUnionFormat();
	AInt index = gen0UnionCaseIndex(key, ?);
	Foam foam = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntEQ;
	foam->foamBCall.argv[0] = foamNewRElt(format, 
					      foamNewCast(FOAM_Rec, whole), (AInt) 0);
	foam->foamBCall.argv[1] = foamNewSInt(index);

	return foam;
	
}

*/

local void
gfm0MatchBlank(AbSyn ab, Foam lhs)
{
	if (abSyme(ab) != NULL) {
		bug("Should generate an assignment here...");
	}
#if 0	
	assert(abTag(ab) == AB_Blank);
	if (abSyme(lhs)) {
		FoamTag tag;
		AInt fmt;
		Foam tmp;
		tag = gen0Type(ab, &fmt);
		tmp = gen0TempLocal(tag, fmt);
		foamNewSet(tmp, lhs);
	}
	return gen0MatchPartialSuccess(lhs);
#endif	
}


#if 0


#endif
