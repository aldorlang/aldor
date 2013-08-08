/*****************************************************************************
 *
 * of_peep.c: Foam peep-hole optimizations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "of_peep.h"
#include "of_util.h"
#include "optfoam.h"
#include "opttools.h"
#include "util.h"
#include "sexpr.h"

Bool	peepDebug	= false;
#define peepDEBUG	DEBUG_IF(peepDebug)


/*****************************************************************************
 *
 * :: Local function declarations
 *
 ****************************************************************************/

local Foam peepExpr		(Foam, Bool *);
local void peepAux		(Foam *);
local Foam peepBCall		(Foam);
local Foam peepCast		(Foam);
local Foam peepIf		(Foam);
local Foam peepSelect		(Foam);
local Foam peepCCall		(Foam);
local Foam peepEEnsure		(Foam);

#ifdef PeepEnv
local Foam peepEElt		(Foam);
local Foam peepEEnv		(Foam);
#endif

local Foam peepIf		(Foam);

local FoamTag	peepFoamExprType	(Foam);
local Bool	peepFoamIsValue		(FoamTag, int, Foam);
local Bool	peepFoamIsPowerOf2	(Foam);
local Foam	peepFoamValue		(FoamTag, int);

local Foam 	peepUnaryBCall	(Foam);
local Foam 	peepBinaryBCall	(Foam);
local Foam	peepNegate	(Foam);

static Foam	peepProgram = 0;

#define peepNoSideFx(foam)	(!foamHasSideEffect(foam))

Foam
peepUnit(Foam unit,Bool foldfloats)
{
	int	i;
	Foam	defs, def, rhs;

	assert (foamTag(unit) == FOAM_Unit);

	peepDEBUG {
		fprintf(dbOut, ">>peepUnit:\n");
		if (!_dont_assert) foamPrint(dbOut, unit);
		fnewline(dbOut);
	}

	defs = unit->foamUnit.defs;

	for (i = 0; i < foamArgc(defs); i++) {
		def = defs->foamDDef.argv[i];
		rhs = def->foamDef.rhs;

		if (foamTag(rhs) == FOAM_Prog)
			def->foamDef.rhs = peepProg(rhs,foldfloats);
	}

	assert(foamAudit(unit));

	peepDEBUG {
		fprintf(dbOut, "<<peepUnit:\n");
		if (!_dont_assert) foamPrint(dbOut, unit);
		fnewline(dbOut);
	}

	return unit;
}


local Foam
peepExpr(Foam expr, Bool *changed)
{
	FoamTag	tag = foamTag(expr);
	Foam	newExpr = expr;

	foamIter(expr, arg, peepAux(arg));

	switch (tag) {
	case FOAM_BCall:
		newExpr = peepBCall(expr);
		break;
	case FOAM_Cast:
		newExpr = peepCast(expr);
		break;
	case FOAM_If:
		newExpr = peepIf(expr);
		break;
	case FOAM_Select:
		newExpr = peepSelect(expr);
		break;
#ifdef PeepEnv
	case FOAM_EElt:
		newExpr = peepEElt(expr);
		break;
	case FOAM_EEnv:
		newExpr = peepEEnv(expr);
		break;
#endif
	case FOAM_CCall:
		newExpr = peepCCall(expr);
		break;
	case FOAM_EEnsure:
		newExpr = peepEEnsure(expr);
		break;
	default:
		break;
	}

	if (expr != newExpr) { 
		foamFreeNode(expr);
		*changed = true;
	}
	return newExpr;
}

local void
peepAux(Foam *arg)
{
	Bool	subChanged;
	Foam	newArg;
#ifndef NDEBUG
	SExpr	s = sxNil;
#endif

	do {    
		subChanged = false;
		peepDEBUG{s = foamToSExpr(*arg);}
		newArg = peepExpr(*arg, &subChanged);
		peepDEBUG {
			if (subChanged) {
				sxiWrite(dbOut, s, SXRW_MixedCase);
				foamWrSExpr(dbOut, newArg, SXRW_MixedCase);
				fprintf(dbOut, "=====\n");
				sxiFree(s);			
			}
		}
		*arg = newArg;
	} while (subChanged);
}

#define peepHasTag(type, expr) (foamTag(expr) == type)

#define peepIsBCallOf(fn, expr) (foamIsBCallOf(expr, fn))
#define peepIsTheBool(val, expr) (peepHasTag(FOAM_Bool, expr) && \
				    expr->foamBool.BoolData == val)
#define peepIsTheSInt(val, expr) (peepHasTag(FOAM_SInt, expr) && \
				  expr->foamSInt.SIntData == val)
#define peepIsTheBInt(val, expr) (peepHasTag(FOAM_BInt, expr) && \
				  expr->foamBInt.BIntData == val)
#define peepIsTheSFlo(val, expr) (peepHasTag(FOAM_SFlo, expr) && \
				  expr->foamSFlo.SFloData == val)
local Foam
peepBCall(Foam bcall)
{
	FoamBValTag	tag = bcall->foamBCall.op;
	Foam		*argv = bcall->foamBCall.argv;
	Foam		foam = bcall;
	int 		argc = foamBCallArgc(bcall);

	switch (tag) {
	/* Logical expressions */
	  case FOAM_BVal_BoolFalse:
		foam = foamNewBool(false);
		break;
	  case FOAM_BVal_BoolTrue:
		foam = foamNewBool(true);
		break;
	  case FOAM_BVal_BoolNot:
		foam = peepNegate(bcall);
		break;
	  case FOAM_BVal_BoolAnd:
		if (peepIsTheBool(false,argv[0])) {
			if (peepNoSideFx(argv[1]))
				foam = foamNewBool(false);
		}
		else if (peepIsTheBool(true,argv[0])) 
			foam = argv[1];
		else if (peepIsTheBool(false,argv[1])) {
			if (peepNoSideFx(argv[0]))
				foam = foamNewBool(false);
		}
		else if (peepIsTheBool(true,argv[1]))
			foam = argv[0];
		break;
	  case FOAM_BVal_BoolOr:
		if (peepIsTheBool(true,argv[0])) {
			if (peepNoSideFx(argv[1]))
				foam = foamNewBool(true);
		}
		else if (peepIsTheBool(false,argv[0])) 
			foam = argv[1];
		else if (peepIsTheBool(true,argv[1])) {
			if (peepNoSideFx(argv[0]))
				foam = foamNewBool(true);
		}
		else if (peepIsTheBool(false,argv[1])) 
			foam = argv[0];
		break;
	  case FOAM_BVal_BIntToSInt:
		/* NB: sizeof(SInt) may vary too */
		if (foamTag(argv[0]) == FOAM_BInt 
		    && bintSmall(argv[0]->foamBInt.BIntData)
		    && bintLT(bintAbs(argv[0]->foamBInt.BIntData), 
			      bintNew(1L<<24)))
			foam = foamNewSInt(bintSmall(argv[0]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_SIntToBInt:
		if (foamTag(argv[0]) == FOAM_SInt)
			foam = foamNewBInt(bintNew(argv[0]->foamSInt.SIntData));
		break;
	  default:
		break;
	}		
	if (foam != bcall) return foam;

	/* arithmetic, and similar */
	if (argc == 1) 
		foam = peepUnaryBCall(bcall);
	else if (argc == 2)
		foam = peepBinaryBCall(bcall);

	return foam;
}

local Foam
peepCast(Foam cast)
{
	FoamTag castTag = cast->foamCast.type;
	Foam expr = cast->foamCast.expr;
	Foam foam = cast;

	if (peepHasTag(FOAM_Cast, expr)) {
		while (peepHasTag(FOAM_Cast, expr)) {
			expr = expr->foamCast.expr;
		}
       		foam = foamNewCast(castTag, expr);
	}

	if (peepFoamExprType(expr) == castTag)
		return expr;
	return foam;
}

local Foam 
peepIf(Foam foam)
{
	if (peepIsTheBool(true, foam->foamIf.test))
		return foamNewGoto(foam->foamIf.label);
	if (peepIsTheBool(false, foam->foamIf.test))
		return foamNewNOp();
	return foam;
}

local Foam 
peepSelect(Foam foam)
{
	if (foamTag(foam->foamSelect.op) == FOAM_SInt) {
		int idx = foam->foamSelect.op->foamSInt.SIntData;
		return foamNewGoto(foam->foamSelect.argv[idx]);
	}
	return foam;
}

#ifdef PeepEnv
local Foam 
peepEElt(Foam eelt) 
{ 
	Foam foam = eelt;
	Foam ienv = eelt->foamEElt.ref;
	int level = eelt->foamEElt.level;

	if (peepHasTag(FOAM_EEnv, ienv)) 
		foam = foamNewEElt(eelt->foamEElt.env,
				   ienv->foamEEnv.env,
				   level + ienv->foamEnv.level,
				   eelt->foamEElt.lex);
	else if (peepHasTag(FOAM_Env, ienv))
		foam = foamNewLex(level + ienv->foamEnv.level,
			          eelt->foamEElt.lex );
	return foam;
}

local Foam 
peepEEnv(Foam eenv)
{
	Foam foam = eenv;
	Foam ienv = eenv->foamEEnv.env;
	int count = eenv->foamEEnv.level;

	if (peepHasTag(FOAM_EEnv, ienv)) {
		int level = count + ienv->foamEEnv.level;
		assert(level >= 0); /* bug 1168 */
		foam = foamNewEEnv(level, ienv->foamEEnv.env);
	} 
	else if (peepHasTag(FOAM_Env, ienv)) 
		foam = foamNewEnv(count + ienv->foamEnv.level);

	return foam;
}
#endif

local Foam
peepCCall(Foam foam)
{
	Foam op = foam->foamCCall.op;
	Foam new = foam;
	int  i;

	if (foamTag(foam) == FOAM_Clos) {
		new = foamNewEmpty(FOAM_OCall, foamArgc(foam) + 1);
		new->foamOCall.type = foam->foamCCall.type;
		new->foamOCall.op   = op->foamClos.prog;
		new->foamOCall.env  = op->foamClos.env;
		for (i=0; i<foamArgc(foam)-2; i++) 
			new->foamOCall.argv[i] = foam->foamCCall.argv[i];
	}
	return new;
}

local Foam
peepEEnsure(Foam foam)
{
	if (foamTag(foam->foamEEnsure.env) == FOAM_Env)
		return foamNewNOp();
	else
		return foam;
}

local FoamTag 
peepFoamExprType(Foam foam)
{
	FoamTag tag = foamTag(foam);

	if (tag<FOAM_DATA_LIMIT) 
		return tag;
	
	switch (foamTag(foam)) {
	  case FOAM_AElt:
		return foam->foamAElt.baseType;
	  case FOAM_PCall:
		return foam->foamPCall.type;
	  case FOAM_BCall:
	      return foamBValInfo(foam->foamBCall.op).retType;
	  case FOAM_CCall:
	      return foam->foamCCall.type;
	  case FOAM_OCall:
	      return foam->foamOCall.type;
	  case FOAM_Loc:
		return peepProgram->foamProg.locals
			->foamDDecl.argv[foam->foamLoc.index]->foamDecl.type;
	  case FOAM_Par:
		return peepProgram->foamProg.params
			->foamDDecl.argv[foam->foamPar.index]->foamDecl.type;
	  default:
		return 0;
      }
}

local Bool
peepFoamIsValue(FoamTag type, int value, Foam foam)
{
	if (foamTag(foam) != type) 
		return false;

	switch(type) {
	  case FOAM_Bool:
		return foam->foamBool.BoolData == (AInt) value;
	  case FOAM_SInt:
		return foam->foamSInt.SIntData == (AInt) value;
	  case FOAM_HInt:
		return foam->foamHInt.HIntData == (AInt) value;
	  case FOAM_BInt:
		return bintEQ(foam->foamBInt.BIntData, bintNew(value));
	  case FOAM_DFlo:
		return foam->foamDFlo.DFloData == (DFloat) value;
	  case FOAM_SFlo:
		return foam->foamSFlo.SFloData == (SFloat) value;
	  case FOAM_Char:
		return foam->foamChar.CharData == (AInt) value;
	  default:
		bug("odd");
		NotReached(return false);
	}
}

local int
peepFoamIsPowerOf2(Foam foam)
{
	if (peepFoamIsValue(foamTag(foam), int0, foam))
		return false;
	if (peepFoamIsValue(foamTag(foam), 1, foam))
		return false;

	switch (foamTag(foam)) {
	  case FOAM_SInt: 
	{
		  AInt val = (AInt) foam->foamSInt.SIntData;
		  return (val & (val - 1)) == 0;
	  }
		break;
	  case FOAM_BInt: 
	{
		BInt val = foam->foamBInt.BIntData;
		return bintLength(val) > bintLength(bintMinus(val, bint1));
	}
		break;
	  default:
		return false;
	}
	return false;
}

local Foam
peepFoamValue(FoamTag type, int value)
{
	switch(type) {
	  case FOAM_Bool:
		return foamNewBool(value);
	  case FOAM_SInt:
		return foamNewSInt(value);
	  case FOAM_HInt:
		return foamNewHInt(value);
	  case FOAM_BInt:
		return foamNewBInt(bintNew(value));
	  case FOAM_DFlo:
		return foamNewDFlo((DFloat) value);
	  case FOAM_SFlo:
		return foamNewSFlo((SFloat) value);
	  case FOAM_Char:
		return foamNewChar(value);
	  default:
		bug("odd2");
		NotReached(return NULL);
	}
}

/*****************************************************************************
 *
 * :: BCalls of arithmetic functions
 *
 ****************************************************************************/

/* Plan is to describe the arithmetic ops, and then use peepBinary/Unary
 * to implement them
 *
 * To do:
 *	associativity
 *      more types 
 *	use OpZero, One instead of OpFalse, OpTrue
 */
enum bvalOp { 
	OpNone,
	/* Binary */
	OpPlus, 
	OpMinus, 
	OpTimes, 
	OpDivide, 
	OpDivRem, 
	OpGCD,
	OpEQ, 
	OpNE, 
	OpLT, 
	OpLE,
	/* floating point careful */
	OpFPlus,
	OpFMinus,
	OpFTimes,
	OpFDivide,
	OpFEQ,
	OpFNE,
	OpFLT,
	OpFLE,
	OpFNeg,
	OpFIsZero,
	OpFIsNeg,
	OpFIsPos,
	/* Unary */
	OpNeg, 
	OpNext,
	OpPrev,
	OpIsZero,
	OpIsNeg,
	OpIsPos,
	/* nullary */
	OpZero,
	OpOne,
	/*-1*/	OpMOne,
	OpTrue,
	OpFalse,
	/* Faked Operations */
	OpNonZero,
	OpNonNeg,
	OpNonPos,
	OpId
};

typedef enum bvalOp BValOp;

typedef struct _bvalOpInfo {
  BValOp op;
  int    arity;
  /* "Inverse" */
  BValOp dual;
  /* lhs = rhs */
  BValOp leqr;
  /* Operations to use if arg is literal one, zero */
  BValOp leftOne;
  BValOp rightOne;
  BValOp leftZero;
  BValOp rightZero;
} BValOps, *BValOpInfo;

typedef struct {
  FoamBValTag	foamOp;
  FoamTag		type;
  BValOp		peepOp;
} FoamBVals, *FoamBValInfo;

static FoamBValInfo peepBValTbl;

static FoamBVals foamBValOpInfoTableFast[] = {

{ FOAM_BVal_CharEQ,	FOAM_Char,	OpEQ },
{ FOAM_BVal_CharNE,	FOAM_Char,	OpNE },
{ FOAM_BVal_CharLT,	FOAM_Char,	OpLT },
{ FOAM_BVal_CharLE,	FOAM_Char,	OpLE },

{ FOAM_BVal_BoolEQ,	FOAM_Bool,	OpEQ },
{ FOAM_BVal_BoolNE,	FOAM_Bool,	OpNE },

{ FOAM_BVal_SIntPlus, 	FOAM_SInt, 	OpPlus },
{ FOAM_BVal_SIntMinus, 	FOAM_SInt, 	OpMinus },
{ FOAM_BVal_SIntTimes,	FOAM_SInt, 	OpTimes },
{ FOAM_BVal_SIntGcd,	FOAM_SInt,	OpGCD },
{ FOAM_BVal_SIntDivide,	FOAM_SInt,	OpDivRem },
{ FOAM_BVal_SIntEQ,	FOAM_SInt,	OpEQ },
{ FOAM_BVal_SIntNE,	FOAM_SInt,	OpNE },
{ FOAM_BVal_SIntLT,	FOAM_SInt,	OpLT },
{ FOAM_BVal_SIntLE,	FOAM_SInt,	OpLE },
{ FOAM_BVal_SIntNegate,	FOAM_SInt,	OpNeg },
{ FOAM_BVal_SIntNext,	FOAM_SInt,	OpNext },
{ FOAM_BVal_SIntPrev,	FOAM_SInt,	OpPrev },
{ FOAM_BVal_SIntIsZero,	FOAM_SInt,	OpIsZero },
{ FOAM_BVal_SIntIsPos,	FOAM_SInt,	OpIsPos },

{ FOAM_BVal_SFloPlus, 	FOAM_SFlo, 	OpPlus },
{ FOAM_BVal_SFloMinus, 	FOAM_SFlo, 	OpMinus },
{ FOAM_BVal_SFloTimes, 	FOAM_SFlo, 	OpTimes },
{ FOAM_BVal_SFloDivide,	FOAM_SFlo, 	OpDivide },
{ FOAM_BVal_SFloEQ,	FOAM_SFlo,	OpEQ },
{ FOAM_BVal_SFloNE,	FOAM_SFlo,	OpNE },
{ FOAM_BVal_SFloLT,	FOAM_SFlo,	OpLT },
{ FOAM_BVal_SFloLE,	FOAM_SFlo,	OpLE },
{ FOAM_BVal_SFloNegate,	FOAM_SFlo,	OpNeg },
{ FOAM_BVal_SFloIsZero,	FOAM_SFlo,	OpIsZero },
{ FOAM_BVal_SFloIsPos,	FOAM_SFlo,	OpIsPos },
{ FOAM_BVal_SFloIsNeg,	FOAM_SFlo,	OpIsNeg },

{ FOAM_BVal_DFloPlus, 	FOAM_DFlo, 	OpPlus },
{ FOAM_BVal_DFloMinus, 	FOAM_DFlo, 	OpMinus },
{ FOAM_BVal_DFloTimes, 	FOAM_DFlo, 	OpTimes },
{ FOAM_BVal_DFloDivide,	FOAM_DFlo, 	OpDivide },
{ FOAM_BVal_DFloEQ,	FOAM_DFlo,	OpEQ },
{ FOAM_BVal_DFloNE,	FOAM_DFlo,	OpNE },
{ FOAM_BVal_DFloLT,	FOAM_DFlo,	OpLT },
{ FOAM_BVal_DFloLE,	FOAM_DFlo,	OpLE },
{ FOAM_BVal_DFloNegate,	FOAM_DFlo,	OpNeg },
{ FOAM_BVal_DFloIsZero,	FOAM_DFlo,	OpIsZero },
{ FOAM_BVal_DFloIsPos,	FOAM_DFlo,	OpIsPos },
{ FOAM_BVal_DFloIsNeg,	FOAM_DFlo,	OpIsNeg },

{ FOAM_BVal_BIntPlus, 	FOAM_BInt, 	OpPlus },
{ FOAM_BVal_BIntMinus, 	FOAM_BInt, 	OpMinus },
{ FOAM_BVal_BIntTimes, 	FOAM_BInt, 	OpTimes },
{ FOAM_BVal_BIntDivide,	FOAM_BInt, 	OpDivRem },
{ FOAM_BVal_BIntGcd,	FOAM_BInt,	OpGCD },
{ FOAM_BVal_BIntEQ,	FOAM_BInt,	OpEQ },
{ FOAM_BVal_BIntNE,	FOAM_BInt,	OpNE },
{ FOAM_BVal_BIntLT,	FOAM_BInt,	OpLT },
{ FOAM_BVal_BIntLE,	FOAM_BInt,	OpLE },
{ FOAM_BVal_BIntNegate,	FOAM_BInt,	OpNeg },
{ FOAM_BVal_BIntNext,	FOAM_BInt,	OpNext },
{ FOAM_BVal_BIntPrev,	FOAM_BInt,	OpPrev },
{ FOAM_BVal_BIntIsZero,	FOAM_BInt,	OpIsZero },
{ FOAM_BVal_BIntIsPos,	FOAM_BInt,	OpIsPos },
{ FOAM_BVal_BIntIsNeg,	FOAM_BInt,	OpIsNeg },

{ FOAM_BVAL_LIMIT,	FOAM_Nil,	OpNone }
};


static FoamBVals foamBValOpInfoTableSlow[] = {

{ FOAM_BVal_CharEQ,	FOAM_Char,	OpEQ },
{ FOAM_BVal_CharNE,	FOAM_Char,	OpNE },
{ FOAM_BVal_CharLT,	FOAM_Char,	OpLT },
{ FOAM_BVal_CharLE,	FOAM_Char,	OpLE },

{ FOAM_BVal_BoolEQ,	FOAM_Bool,	OpEQ },
{ FOAM_BVal_BoolNE,	FOAM_Bool,	OpNE },

{ FOAM_BVal_SIntPlus, 	FOAM_SInt, 	OpPlus },
{ FOAM_BVal_SIntMinus, 	FOAM_SInt, 	OpMinus },
{ FOAM_BVal_SIntTimes,	FOAM_SInt, 	OpTimes },
{ FOAM_BVal_SIntGcd,	FOAM_SInt,	OpGCD },
{ FOAM_BVal_SIntDivide,	FOAM_SInt,	OpDivRem },
{ FOAM_BVal_SIntEQ,	FOAM_SInt,	OpEQ },
{ FOAM_BVal_SIntNE,	FOAM_SInt,	OpNE },
{ FOAM_BVal_SIntLT,	FOAM_SInt,	OpLT },
{ FOAM_BVal_SIntLE,	FOAM_SInt,	OpLE },
{ FOAM_BVal_SIntNegate,	FOAM_SInt,	OpNeg },
{ FOAM_BVal_SIntNext,	FOAM_SInt,	OpNext },
{ FOAM_BVal_SIntPrev,	FOAM_SInt,	OpPrev },
{ FOAM_BVal_SIntIsZero,	FOAM_SInt,	OpIsZero },
{ FOAM_BVal_SIntIsPos,	FOAM_SInt,	OpIsPos },
{ FOAM_BVal_SIntIsNeg,	FOAM_SInt,	OpIsNeg },

{ FOAM_BVal_SFloPlus, 	FOAM_SFlo, 	OpFPlus  },
{ FOAM_BVal_SFloMinus, 	FOAM_SFlo, 	OpFMinus },
{ FOAM_BVal_SFloTimes, 	FOAM_SFlo, 	OpFTimes },
{ FOAM_BVal_SFloDivide,	FOAM_SFlo, 	OpFDivide },
{ FOAM_BVal_SFloEQ,	FOAM_SFlo,	OpFEQ },
{ FOAM_BVal_SFloNE,	FOAM_SFlo,	OpFNE },
{ FOAM_BVal_SFloLT,	FOAM_SFlo,	OpFLT },
{ FOAM_BVal_SFloLE,	FOAM_SFlo,	OpFLE },
{ FOAM_BVal_SFloNegate,	FOAM_SFlo,	OpFNeg },
{ FOAM_BVal_SFloIsZero,	FOAM_SFlo,	OpFIsZero },
{ FOAM_BVal_SFloIsPos,	FOAM_SFlo,	OpFIsPos },
{ FOAM_BVal_SFloIsNeg,	FOAM_SFlo,	OpFIsNeg },

{ FOAM_BVal_DFloPlus, 	FOAM_DFlo, 	OpFPlus  },
{ FOAM_BVal_DFloMinus, 	FOAM_DFlo, 	OpFMinus },
{ FOAM_BVal_DFloTimes, 	FOAM_DFlo, 	OpFTimes },
{ FOAM_BVal_DFloDivide,	FOAM_DFlo, 	OpFDivide  },
{ FOAM_BVal_DFloEQ,	FOAM_DFlo,	OpFEQ },
{ FOAM_BVal_DFloNE,	FOAM_DFlo,	OpFNE },
{ FOAM_BVal_DFloLT,	FOAM_DFlo,	OpFLT },
{ FOAM_BVal_DFloLE,	FOAM_DFlo,	OpFLE },
{ FOAM_BVal_DFloNegate,	FOAM_DFlo,	OpFNeg },
{ FOAM_BVal_DFloIsZero,	FOAM_DFlo,	OpFIsZero },
{ FOAM_BVal_DFloIsPos,	FOAM_DFlo,	OpFIsPos },
{ FOAM_BVal_DFloIsNeg,	FOAM_DFlo,	OpFIsNeg },

{ FOAM_BVal_BIntPlus, 	FOAM_BInt, 	OpPlus },
{ FOAM_BVal_BIntMinus, 	FOAM_BInt, 	OpMinus },
{ FOAM_BVal_BIntTimes, 	FOAM_BInt, 	OpTimes },
{ FOAM_BVal_BIntDivide,	FOAM_BInt, 	OpDivRem },
{ FOAM_BVal_BIntGcd,	FOAM_BInt,	OpGCD },
{ FOAM_BVal_BIntEQ,	FOAM_BInt,	OpEQ },
{ FOAM_BVal_BIntNE,	FOAM_BInt,	OpNE },
{ FOAM_BVal_BIntLT,	FOAM_BInt,	OpLT },
{ FOAM_BVal_BIntLE,	FOAM_BInt,	OpLE },
{ FOAM_BVal_BIntNegate,	FOAM_BInt,	OpNeg },
{ FOAM_BVal_BIntNext,	FOAM_BInt,	OpNext },
{ FOAM_BVal_BIntPrev,	FOAM_BInt,	OpPrev },
{ FOAM_BVal_BIntIsZero,	FOAM_BInt,	OpIsZero },
{ FOAM_BVal_BIntIsPos,	FOAM_BInt,	OpIsPos },
{ FOAM_BVal_BIntIsNeg,	FOAM_BInt,	OpIsNeg },

{ FOAM_BVAL_LIMIT,	FOAM_Nil,	OpNone }
};






local Foam	peepMakeUnaryOp		(BValOp, FoamTag, Foam);
local Foam	peepMakeBinaryOp	(BValOp, FoamTag, Foam, Foam);
local FoamBValInfo	peepFindOpInfo	(FoamBValTag);
local FoamBValTag	peepFindFoamOp	(BValOp, FoamTag);
local Foam	peepAdditiveOp		(FoamTag, BValOp, Foam, Foam);
local Foam	peepTimesOp		(FoamTag, BValOp, Foam, Foam);
local Foam	peepPositive		(Foam);

/* Could warn about potential zero-divides here */
/* NB: gcd(x,x) = x iff x>=0 */
BValOps peepBValOpInfo[] = {
/*   op	    arity  dual      l=r      l=1      r=1    l=0         r=0  */
{ OpNone, 	0, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone },
{ OpPlus, 	2, OpNone,  OpNone,  OpNext,  OpNext, OpId,      OpId   },
{ OpMinus,	2, OpNone,  OpZero,  OpNone,  OpPrev, OpNeg,     OpId   },
{ OpTimes,	2, OpNone,  OpNone,  OpId,    OpId,   OpZero,    OpZero },
{ OpDivide,	2, OpNone,  OpOne,   OpNone,  OpId,   OpZero,    OpNone },
{ OpDivRem,	2, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone },
{ OpGCD, 	2, OpNone,  OpNone,  OpOne,   OpOne,  OpNone,    OpNone },

/* Boolean ops (don't map ~(a < b) into (b <= a) as jflow can't cope) */
{ OpEQ,		2, OpNE,    OpTrue,  OpNone,  OpNone, OpIsZero,  OpIsZero  },
{ OpNE,		2, OpEQ,    OpFalse, OpNone,  OpNone, OpNonZero, OpNonZero },
{ OpLT,		2, OpNone,  OpFalse, OpNone,  OpNone, OpIsPos,   OpIsNeg   },
{ OpLE,		2, OpLT,    OpTrue,  OpNone,  OpNone, OpNonNeg,  OpNonPos  },

/* floating point careful (IEEE)*/
{ OpFPlus, 	2, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone   },
{ OpFMinus,	2, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone   },
{ OpFTimes,	2, OpNone,  OpNone,  OpId,    OpId,   OpNone,    OpNone },
{ OpFDivide,	2, OpNone,  OpNone,  OpNone,  OpId,   OpNone,    OpNone },
{ OpFEQ,       	2, OpNE,    OpNone,  OpNone,  OpNone, OpIsZero,  OpIsZero  },
{ OpFNE,	2, OpEQ,    OpNone,  OpNone,  OpNone, OpNonZero, OpNonZero },
{ OpFLT,	2, OpNone,  OpNone,  OpNone,  OpNone, OpIsPos,   OpIsNeg   },
{ OpFLE,	2, OpNone,  OpNone,  OpNone,  OpNone, OpNonNeg,  OpNonPos  },
{ OpFNeg,       1, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone },
{ OpFIsZero,	1, OpNone,  OpNone,  OpFalse, OpNone, OpNone,    OpNone},
{ OpFIsNeg,	1, OpNone,  OpNone,  OpFalse, OpNone, OpNone,    OpNone},
{ OpFIsPos,	1, OpNone,  OpNone,  OpTrue,  OpNone, OpNone,    OpNone},
/* Unary operations (on lhs) */
{ OpNeg,	1, OpNeg,   OpNone,  OpMOne,  OpNone, OpZero,    OpNone},
{ OpNext,	1, OpPrev,  OpNone,  OpNone,  OpNone, OpOne,     OpNone},
{ OpPrev,	1, OpNext,  OpNone,  OpZero,  OpNone, OpMOne,    OpNone},
{ OpIsZero,	1, OpNone,  OpNone,  OpFalse, OpNone, OpTrue,    OpNone},
{ OpIsNeg,	1, OpNone,  OpNone,  OpFalse, OpNone, OpFalse,   OpNone},
{ OpIsPos,	1, OpNone,  OpNone,  OpTrue,  OpNone, OpFalse,   OpNone},
/* faked */
{ OpZero,	0, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone},
{ OpOne,	0, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone},
{ OpMOne,	0, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone},
{ OpTrue,	0, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone},
{ OpFalse,	0, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone},
/* Finis. */
{ -1,		1, OpNone,  OpNone,  OpNone,  OpNone, OpNone,    OpNone}
};


local Foam
peepBinaryBCall(Foam bcall)
{
	Foam		l = bcall->foamBCall.argv[0];
	Foam		r = bcall->foamBCall.argv[1];
	Foam		arg = NULL, new;
	FoamBValTag 	op = bcall->foamBCall.op;
	FoamTag		type;
	FoamBValInfo 	bopInfo = peepFindOpInfo(op);
	BValOp		bop, newOp = OpNone;
	
	if (bopInfo == NULL) 
		return bcall;
	
	type = bopInfo->type;
	bop  = bopInfo->peepOp;
	assert(peepBValOpInfo[bop].op == bop);

	if (bop == OpPlus || bop == OpMinus) {
		new = peepAdditiveOp(type, bop, l, r);
		if (new) return new;
	}
	if (bop == OpTimes) {
		new = peepTimesOp(type, bop, l, r);
		if (new) return new;
	}
	if ( (!foamHasSideEffect(l)) && foamEqual(l, r)) {
		newOp = peepBValOpInfo[bop].leqr;
		arg   = l;
	}
	if (peepBValOpInfo[bop].leftZero != OpNone
	    && peepFoamIsValue(type, int0, l)) {
		newOp = peepBValOpInfo[bop].leftZero;
		arg = r;
	}
	else if (peepBValOpInfo[bop].leftOne != OpNone
		 && peepFoamIsValue(type, 1, l)) {
		newOp = peepBValOpInfo[bop].leftOne;
		arg = r;
	}
	else if (peepBValOpInfo[bop].rightZero != OpNone
		 && peepFoamIsValue(type, int0, r)) {
		newOp = peepBValOpInfo[bop].rightZero;
		arg = l;
	}
	else if (peepBValOpInfo[bop].rightOne != OpNone
		 && peepFoamIsValue(type, 1, r)) {
		newOp = peepBValOpInfo[bop].rightOne;
		arg = l;
	}

	if (newOp == OpNone)
		return bcall;

	new = peepMakeUnaryOp(newOp, type, arg);

	return new != NULL ? new : bcall;
			
}

local Foam
peepUnaryBCall(Foam bcall)
{
	Foam 		arg = bcall->foamBCall.argv[0];
	FoamBValTag 	op  = bcall->foamBCall.op;
	FoamBValInfo 	bopInfo, inInfo;

	if (foamTag(arg) != FOAM_BCall) 
		return bcall;
	
	bopInfo = peepFindOpInfo(op);
	inInfo = peepFindOpInfo(arg->foamBCall.op);

	if (bopInfo == NULL || inInfo == NULL)
		return bcall;
	
	/* inverse ? */
	if (peepBValOpInfo[bopInfo->peepOp].dual == inInfo->peepOp) 
		return arg->foamBCall.argv[0];
	
	return bcall;
}


local Foam
peepNegate(Foam foam)
{
	Foam 	    arg = foam->foamBCall.argv[0];
	Foam	    new = foam;
	FoamBValTag op;
	int 	    argc;
	FoamBValInfo 	bopInfo;

	if (foamTag(arg) != FOAM_BCall)
		return foam;

	op   = arg->foamBCall.op;
	argc =  foamBCallArgc(arg);
	bopInfo = peepFindOpInfo(op);

	if (argc == 1 && op == FOAM_BVal_BoolNot)
		new = arg->foamBCall.argv[0];
	else if (bopInfo == NULL) 
		return foam;
	else if (argc == 2 
		 && peepBValOpInfo[bopInfo->peepOp].dual != OpNone
		 && (peepNoSideFx(arg->foamBCall.argv[0])
		     || peepNoSideFx(arg->foamBCall.argv[1]))) {
		/* Deal with an inverse/dual relationship */
		new = peepMakeBinaryOp(peepBValOpInfo[bopInfo->peepOp].dual,
				       bopInfo->type, 
				       arg->foamBCall.argv[1],
				       arg->foamBCall.argv[0]);
		if (!new) 
			new = foam;
	}
	return new;
}


local Foam 
peepMakeUnaryOp(BValOp op, FoamTag type, Foam arg0)
{
	Foam 	new = NULL;
	FoamBValTag fop;

	if (peepBValOpInfo[op].arity == 0 && 
	    !peepNoSideFx(arg0))
		return NULL;

	switch(op) {
	  case OpZero:
		new = peepFoamValue(type, int0);
		break;
	  case OpOne:		
		new = peepFoamValue(type, 1);
		break;
	  case OpMOne:
		new = peepFoamValue(type, -1);
		break;
	  case OpTrue:
		new = foamNewBool(true);
		break;
	  case OpFalse:
		new = foamNewBool(false);
		break;
	  case OpId:
		new = arg0;
		break;
	  case OpNonZero:
		if (peepFindFoamOp(OpIsZero, type))
			new = foamNew(FOAM_BCall, 2, FOAM_BVal_BoolNot,
				      peepMakeUnaryOp(OpIsZero, type, arg0));
		break;
	  case OpNonPos:
		if (peepFindFoamOp(OpIsPos, type))
			new = foamNew(FOAM_BCall, 2, FOAM_BVal_BoolNot,
				      peepMakeUnaryOp(OpIsPos, type, arg0));
		break;
	  case OpNonNeg:
		if (peepFindFoamOp(OpIsNeg, type))
			new = foamNew(FOAM_BCall, 2, FOAM_BVal_BoolNot,
				      peepMakeUnaryOp(OpIsNeg, type, arg0));
		break;
	  default:
		fop = peepFindFoamOp(op, type);
		if (fop)
			new = foamNew(FOAM_BCall, 2, fop, arg0);
	}
	return new;
}

local Foam
peepMakeBinaryOp(BValOp op, FoamTag type, Foam arg0, Foam arg1)
{
	FoamBValTag fop = peepFindFoamOp(op, type);
	Foam 	new;
	
	if (!fop)
		return NULL;
	new = foamNew(FOAM_BCall, 3,
		      fop, arg0, arg1);

	return new;
}

local FoamBValInfo
peepFindOpInfo(FoamBValTag op)
{
	FoamBValInfo info = peepBValTbl;

	while (info->foamOp != FOAM_BVAL_LIMIT) {
		if (info->foamOp == op)
			return info;
		info++;
	}
	return NULL;
}

local FoamBValTag
peepFindFoamOp(BValOp op, FoamTag type)
{
	FoamBValInfo info = peepBValTbl;

	while (info->foamOp != FOAM_BVAL_LIMIT) {
		if (info->peepOp == op && info->type == type)
			return info->foamOp;
		info++;
	}
	return 0;
}

local Foam
peepAdditiveOp(FoamTag type, BValOp bop, Foam lhs, Foam rhs)
{
	Foam    tmp, pos = NULL, new;

	if (bop == OpPlus) {
		/* (-a) + b ==> b - a */
		pos = peepPositive(lhs);
		if (pos) {
			tmp = lhs;
			lhs = rhs;
			rhs = tmp;
		}
	}
	/* a +/- (-b) ==> a -/+ b */
	if (!pos) 
		pos = peepPositive(rhs);

	if (!pos) 
		return NULL;
	
	new = peepMakeBinaryOp( bop == OpPlus ? OpMinus : OpPlus, type,
			       lhs, pos);

	return new;
}

local Foam
peepTimesOp(FoamTag tag, BValOp op, Foam l, Foam r)
{
	int shift;

	if (tag != FOAM_BInt && tag != FOAM_SInt)
		return NULL;

	if (otIsFoamConst(l)) {
		Foam tmp = l;
		l = r;
		r = tmp;
	}

	if (!otIsFoamConst(r))
		return NULL;
	
	if (!peepFoamIsPowerOf2(r)) 
		return NULL;

	switch (foamTag(r)) {
	  case FOAM_SInt:
		shift = intLength(r->foamSInt.SIntData) - 1;
		/* Shifting it not defined for bigger numbers */ 
		if (shift > 30) return NULL;
		return foamNew(FOAM_BCall, 3, FOAM_BVal_SIntShiftUp, l,
			       foamNewSInt(shift));
		break;
	  case FOAM_BInt:
		shift = bintLength(r->foamBInt.BIntData) - 1;
		return foamNew(FOAM_BCall, 3, FOAM_BVal_BIntShiftUp, l, 
			       foamNewSInt(shift));
		break;
	  default:
		break;
	}
	return NULL;
}

local Foam
peepPositive(Foam expr)
{
	Foam 	     new = NULL;
	FoamBValInfo info;

	switch (foamTag(expr)) {
	case FOAM_BCall:
		info = peepFindOpInfo(expr->foamBCall.op);
		if (info && info->peepOp == OpNeg) {
			new =  expr->foamBCall.argv[0];
		}
		break;
	case FOAM_BInt:
		if (bintIsNeg(expr->foamBInt.BIntData))
			new = foamNewBInt(bintNegate(expr->foamBInt.BIntData));
		break;
	case FOAM_SInt:
		if (expr->foamSInt.SIntData < 0)
			new = foamNewSInt(-expr->foamSInt.SIntData);
		break;
	default:
		break;
	}

	return new;
}

Foam
peepProg(Foam prog,Bool foldfloats)
{	
	int	i;
	Bool	changed = false;
	Foam	body = prog->foamProg.body;

	if (foldfloats) peepBValTbl = &foamBValOpInfoTableFast[0];
	else peepBValTbl = &foamBValOpInfoTableSlow[0];

	if (!optIsPeepPending(prog)) return prog;
	optResetPeepPending(prog);
	
	peepProgram = prog;

	assert(foamTag(prog) == FOAM_Prog);
	assert(foamTag(body) == FOAM_Seq);

	for(i=0; i<foamArgc(body); i++)
		body->foamSeq.argv[i] = peepExpr(body->foamSeq.argv[i], 
						 &changed);

	if (changed)
		optSetJFlowPending(prog);

	return prog;
}

