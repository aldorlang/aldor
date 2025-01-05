/*****************************************************************************
 *
 * of_cfold.c: Foam constant folding.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "format.h"
#include "genfoam.h"
#include "optfoam.h"
#include "of_cfold.h"
#include "of_util.h"
#include "optinfo.h"
#include "store.h"
#include "syme.h"
#include "bigint.h"
#include "strops.h"
#include "symbol.h"
#include "fbox.h"
#include "util.h"

Bool	cfoldDebug	= false;
#define cfoldDEBUG	DEBUG_IF(cfold)	afprintf

/*****************************************************************************
 *
 * :: Local function declarations
 *
 ****************************************************************************/

local void	cfoldDefs		(Foam);
local void	cfoldProg		(Foam);
local Foam	cfoldExpr		(Foam);
local Foam	cfoldBCall		(Foam);
local Bool	cfoldIsConst		(Foam);
local Bool	cfoldBCallIsConst	(Foam);
local String	cfoldArrToString	(Foam);
local Foam	cfoldCast		(Foam);
local void	cfoldMoveConstExports	(Foam unit);
local void	cfoldMoveProgConsts	(Foam prog);
local void	cfoldConstDef		(Foam def);


Bool	cfoldFoldFloat;	/* constant fold floats */
Bool	cfoldFoldAll;	/* constant fold everything but floats. */

Foam	cfoldFormats = 0;
Foam	cfoldProgram = 0;

FoamBox	cfoldConstDecls;
FoamBox	cfoldConstDefs;
Bool	cfoldMovedConsts;
Bool	cfoldChanged;

Bool
cfoldUnit(Foam unit, Bool foldAll, Bool foldFloat)
{
	assert (foamTag(unit) == FOAM_Unit);

	if (DEBUG(cfold)) {
		fprintf(dbOut, ">>cfoldUnit:\n");
		foamPrint(dbOut, unit);
		fnewline(dbOut);
	}

	cfoldFoldFloat = foldFloat;
	cfoldFoldAll   = foldAll;
	cfoldFormats   = unit->foamUnit.formats;
	cfoldMovedConsts = false;
	cfoldDefs(unit->foamUnit.defs);
	cfoldMoveConstExports(unit);
	assert(foamAudit(unit));

	if (DEBUG(cfold)) {
		fprintf(dbOut, "<<cfoldUnit:\n");
		foamPrint(dbOut, unit);
		fnewline(dbOut);
	}

	return cfoldMovedConsts;
}

local void
cfoldDefs(Foam defs)
{
	int	i;
	Foam	def;
	assert(foamTag(defs) == FOAM_DDef);

	for(i=0; i < foamArgc(defs); i++)  {
		def = defs->foamDDef.argv[i];
		assert(foamTag(def) == FOAM_Def);
		if (foamTag(def->foamDef.rhs) == FOAM_Prog) 
			cfoldProg(def->foamDef.rhs);
	}
}

local void
cfoldProg(Foam prog)
{	
	int	i;
	Foam	body = prog->foamProg.body;
	assert(foamTag(prog) == FOAM_Prog);
	assert(foamTag(body) == FOAM_Seq);
	cfoldProgram = prog;
	cfoldChanged = false;
	for(i=0; i<foamArgc(body); i++)
		body->foamSeq.argv[i] = cfoldExpr(body->foamSeq.argv[i]);
	if (cfoldChanged) {
		optSetPeepPending(prog);
		optSetCPropPending(prog);
	}
}

local Foam
cfoldExpr(Foam expr)
{
	FoamTag	tag = foamTag(expr);
	Foam		newArg, newExpr = expr;

	foamIter(expr, arg, {
		newArg = cfoldExpr(*arg);
		*arg = newArg;
	});
	switch (tag) {
	  case FOAM_BCall:
		newExpr = cfoldBCall(expr);
		break;
	  case FOAM_Cast:
		newExpr = cfoldCast(expr);
		break;
	  default:
		break;
	}

	if (newExpr != expr) {
		foamFreeNode(expr);
		cfoldChanged = true;
	}
	return newExpr;
}

local Foam
cfoldBCall(Foam bcall)
{
	FoamBValTag	tag = bcall->foamBCall.op;
	int		i, nargs = foamArgc(bcall) - 1; /* -1 for op */
	long		n;
	Foam		*argv = bcall->foamBCall.argv;
	Foam		foam = bcall;
	String		s;

	/* make sure all arguments are constant. */
	for(i=0; i < nargs; i++) 
		if (!cfoldIsConst(argv[i]))
			return bcall;
	switch (tag) {
		
	  case FOAM_BVal_BoolFalse:
		if (!cfoldFoldAll) break;
		foam = foamNewBool(false);
		break;
	  case FOAM_BVal_BoolTrue:
		if (!cfoldFoldAll) break;
		foam = foamNewBool(true);
		break;
	  case FOAM_BVal_BoolNot:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Bool);
		foam = foamNewBool(!argv[0]->foamBool.BoolData);
		break;
	  case FOAM_BVal_BoolAnd:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Bool);
		assert(foamTag(argv[1]) == FOAM_Bool);
		foam = foamNewBool(argv[0]->foamBool.BoolData &&
				  argv[0]->foamBool.BoolData);
		break;
	  case FOAM_BVal_BoolOr:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Bool);
		assert(foamTag(argv[1]) == FOAM_Bool);
		foam = foamNewBool(argv[0]->foamBool.BoolData ||
				  argv[0]->foamBool.BoolData);
		break;
	  case FOAM_BVal_BoolEQ:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Bool);
		assert(foamTag(argv[1]) == FOAM_Bool);
		foam = foamNewBool(argv[0]->foamBool.BoolData ==
				  argv[0]->foamBool.BoolData);
		break;
	  case FOAM_BVal_BoolNE:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Bool);
		assert(foamTag(argv[1]) == FOAM_Bool);
		foam = foamNewBool(argv[0]->foamBool.BoolData !=
				  argv[0]->foamBool.BoolData);
		break;
		
	  case FOAM_BVal_CharSpace:
		if (!cfoldFoldAll) break;
		foam = foamNewChar(' ');
		break;
	  case FOAM_BVal_CharNewline:
		if (!cfoldFoldAll) break;
		foam = foamNewChar('\n');
		break;
	  case FOAM_BVal_CharTab:
		if (!cfoldFoldAll) break;
		foam = foamNewChar('\t');
		break;
	  case FOAM_BVal_CharMin:
		if (!cfoldFoldAll) break;
		break; /* Platform-dependent */
	  case FOAM_BVal_CharMax:
		if (!cfoldFoldAll) break;
		break; /* Platform-dependent */
	  case FOAM_BVal_CharIsDigit:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		foam = foamNewBool(isdigit(argv[0]->foamChar.CharData));
		break;
	  case FOAM_BVal_CharIsLetter:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		foam = foamNewBool(isalpha(argv[0]->foamChar.CharData));
		break;
	  case FOAM_BVal_CharEQ:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		assert(foamTag(argv[1]) == FOAM_Char);
		foam = foamNewBool(argv[0]->foamChar.CharData ==
				  argv[0]->foamChar.CharData);
		break;
	  case FOAM_BVal_CharNE:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		assert(foamTag(argv[1]) == FOAM_Char);
		foam = foamNewBool(argv[0]->foamChar.CharData !=
				  argv[0]->foamChar.CharData);
		break;
	  case FOAM_BVal_CharLT:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		assert(foamTag(argv[1]) == FOAM_Char);
		foam = foamNewBool(argv[0]->foamChar.CharData <
				  argv[0]->foamChar.CharData);
		break;
	  case FOAM_BVal_CharLE:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		assert(foamTag(argv[1]) == FOAM_Char);
		foam = foamNewBool(argv[0]->foamChar.CharData <=
				  argv[0]->foamChar.CharData);
		break;
	  case FOAM_BVal_CharLower:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		foam = foamNewChar(tolower(argv[0]->foamChar.CharData));
		break;
	  case FOAM_BVal_CharUpper:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		foam = foamNewChar(toupper(argv[0]->foamChar.CharData));
		break;
	  case FOAM_BVal_CharOrd:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Char);
		foam = foamNewSInt(argv[0]->foamChar.CharData);
		break;
	  case FOAM_BVal_CharNum:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewChar(argv[0]->foamSInt.SIntData);
		break;

	  case FOAM_BVal_SFlo0:
		if (!cfoldFoldFloat) break;
		foam = foamNewSFlo(0.0);
		break;
	  case FOAM_BVal_SFlo1:
		if (!cfoldFoldFloat) break;
		foam = foamNewSFlo(1.0);
		break;
	  case FOAM_BVal_SFloMin:
		if (!cfoldFoldFloat) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_SFloMax:
		if (!cfoldFoldFloat) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_SFloEpsilon:
		if (!cfoldFoldFloat) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_SFloIsZero:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		foam = foamNewBool(argv[0]->foamSFlo.SFloData == 0.0);
		break;
	  case FOAM_BVal_SFloIsNeg:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		foam = foamNewBool(argv[0]->foamSFlo.SFloData < 0.0);
		break;
	  case FOAM_BVal_SFloIsPos:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		foam = foamNewBool(argv[0]->foamSFlo.SFloData > 0.0);
		break;
	  case FOAM_BVal_SFloEQ:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewBool(argv[0]->foamSFlo.SFloData ==
				  argv[1]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloNE:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewBool(argv[0]->foamSFlo.SFloData !=
				  argv[1]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloLT:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewBool(argv[0]->foamSFlo.SFloData <
				  argv[1]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloLE:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewBool(argv[0]->foamSFlo.SFloData <=
				  argv[1]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloNegate:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		foam = foamNewSFlo(-argv[0]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloPlus:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewSFlo(argv[0]->foamSFlo.SFloData +
				   argv[1]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloMinus:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewSFlo(argv[0]->foamSFlo.SFloData -
				   argv[1]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloTimes:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewSFlo(argv[0]->foamSFlo.SFloData *
				   argv[1]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloTimesPlus:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		assert(foamTag(argv[2]) == FOAM_SFlo);
		foam = foamNewSFlo(argv[0]->foamSFlo.SFloData *
				   argv[1]->foamSFlo.SFloData +
				   argv[2]->foamSFlo.SFloData);
		break;
	  case FOAM_BVal_SFloDivide:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_SFlo);
		assert(foamTag(argv[1]) == FOAM_SFlo);
		foam = foamNewSFlo(argv[0]->foamSFlo.SFloData /
				   argv[1]->foamSFlo.SFloData);
		break;
		
	  case FOAM_BVal_DFlo0:
		/*if (!cfoldFoldFloat) break;*/
		foam = foamNewDFlo(0.0);
		break;
	  case FOAM_BVal_DFlo1:
		/*if (!cfoldFoldFloat) break;*/
		foam = foamNewDFlo(1.0);
		break;
	  case FOAM_BVal_DFloMin:
		if (!cfoldFoldFloat) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_DFloMax:
		if (!cfoldFoldFloat) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_DFloEpsilon:
		if (!cfoldFoldFloat) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_DFloIsZero:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		foam = foamNewBool(argv[0]->foamDFlo.DFloData == 0.0);
		break;
	  case FOAM_BVal_DFloIsNeg:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		foam = foamNewBool(argv[0]->foamDFlo.DFloData < 0.0);
		break;
	  case FOAM_BVal_DFloIsPos:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		foam = foamNewBool(argv[0]->foamDFlo.DFloData > 0.0);
		break;
	  case FOAM_BVal_DFloEQ:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewBool(argv[0]->foamDFlo.DFloData ==
				  argv[1]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloNE:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewBool(argv[0]->foamDFlo.DFloData !=
				  argv[1]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloLT:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewBool(argv[0]->foamDFlo.DFloData <
				  argv[1]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloLE:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewBool(argv[0]->foamDFlo.DFloData <=
				  argv[1]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloNegate:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		foam = foamNewDFlo(-argv[0]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloPlus:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewDFlo(argv[0]->foamDFlo.DFloData +
				   argv[1]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloMinus:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewDFlo(argv[0]->foamDFlo.DFloData -
				   argv[1]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloTimes:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewDFlo(argv[0]->foamDFlo.DFloData *
				   argv[1]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloTimesPlus:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		assert(foamTag(argv[2]) == FOAM_DFlo);
		foam = foamNewDFlo(argv[0]->foamDFlo.DFloData *
				   argv[1]->foamDFlo.DFloData +
				   argv[2]->foamDFlo.DFloData);
		break;
	  case FOAM_BVal_DFloDivide:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_DFlo);
		assert(foamTag(argv[1]) == FOAM_DFlo);
		foam = foamNewDFlo(argv[0]->foamDFlo.DFloData /
				   argv[1]->foamDFlo.DFloData);
		break;
		
	  case FOAM_BVal_Byte0:
		if (!cfoldFoldAll) break;
		foam = foamNewByte(int0);
		break;
	  case FOAM_BVal_Byte1:
		if (!cfoldFoldAll) break;
		foam = foamNewByte(1);
		break;
	  case FOAM_BVal_ByteMin:
		if (!cfoldFoldAll) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_ByteMax:
		if (!cfoldFoldAll) break;
		break;	/* Platform-dependent */
		
	  case FOAM_BVal_HInt0:
		if (!cfoldFoldAll) break;
		foam = foamNewHInt(int0);
		break;
	  case FOAM_BVal_HInt1:
		if (!cfoldFoldAll) break;
		foam = foamNewHInt(1);
		break;
	  case FOAM_BVal_HIntMin:
		if (!cfoldFoldAll) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_HIntMax:
		if (!cfoldFoldAll) break;
		break;	/* Platform-dependent */
		
	  case FOAM_BVal_SInt0:
		if (!cfoldFoldAll) break;
		foam = foamNewSInt(int0);
		break;
	  case FOAM_BVal_SInt1:
		if (!cfoldFoldAll) break;
		foam = foamNewSInt(1);
		break;
	  case FOAM_BVal_SIntMin:
		if (!cfoldFoldAll) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_SIntMax:
		if (!cfoldFoldAll) break;
		break;	/* Platform-dependent */
	  case FOAM_BVal_SIntIsZero:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewBool(argv[0]->foamSInt.SIntData == 0);
		break;
	  case FOAM_BVal_SIntIsNeg:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewBool(argv[0]->foamSInt.SIntData < 0);
		break;
	  case FOAM_BVal_SIntIsPos:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewBool(argv[0]->foamSInt.SIntData > 0);
		break;
	  case FOAM_BVal_SIntIsEven:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewBool((argv[0]->foamSInt.SIntData % 2) == 0);
		break;
	  case FOAM_BVal_SIntIsOdd:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewBool((argv[0]->foamSInt.SIntData % 2) == 1);
		break;
	  case FOAM_BVal_SIntEQ:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewBool(argv[0]->foamSInt.SIntData ==
				  argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntNE:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewBool(argv[0]->foamSInt.SIntData !=
				  argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntLT:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewBool(argv[0]->foamSInt.SIntData <
				  argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntLE:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewBool(argv[0]->foamSInt.SIntData <=
				  argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntNegate:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewSInt(-argv[0]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntPrev:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData-1);
		break;
	  case FOAM_BVal_SIntNext:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData+1);
		break;
	  case FOAM_BVal_SIntPlus:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData +
				   argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntMinus:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData -
				   argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntTimes:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData *
				   argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntTimesPlus:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		assert(foamTag(argv[2]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData *
				   argv[1]->foamSInt.SIntData +
				   argv[2]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntMod:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData %
				   argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntQuo:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData /
				   argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntRem:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData %
				   argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntDivide:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntGcd:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */

	  case FOAM_BVal_SIntPlusMod:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		assert(foamTag(argv[2]) == FOAM_SInt);
		n = argv[0]->foamSInt.SIntData + argv[1]->foamSInt.SIntData;
		foam = foamNewSInt(n % argv[2]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntMinusMod:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		assert(foamTag(argv[2]) == FOAM_SInt);
		n = argv[0]->foamSInt.SIntData - argv[1]->foamSInt.SIntData;
		foam = foamNewSInt(n % argv[2]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntTimesMod:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		assert(foamTag(argv[2]) == FOAM_SInt);
		n = argv[0]->foamSInt.SIntData * argv[1]->foamSInt.SIntData;
		foam = foamNewSInt(n % argv[2]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntLength:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */

         case FOAM_BVal_SIntShiftUp:
                if (!cfoldFoldAll) break;
                assert(foamTag(argv[0]) == FOAM_SInt);
                assert(foamTag(argv[1]) == FOAM_SInt);
                n = argv[1]->foamSInt.SIntData;
                foam = foamNewSInt((long) argv[0]->foamSInt.SIntData << n);
                break;
          case FOAM_BVal_SIntShiftDn:
                if (!cfoldFoldAll) break;
                assert(foamTag(argv[0]) == FOAM_SInt);
                assert(foamTag(argv[1]) == FOAM_SInt);
                n = argv[1]->foamSInt.SIntData;
                foam = foamNewSInt(argv[0]->foamSInt.SIntData >> n);
                break;

	  case FOAM_BVal_SIntBit:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntNot:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntAnd:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData &
				   argv[1]->foamSInt.SIntData);
		break;
	  case FOAM_BVal_SIntOr:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData |
				   argv[1]->foamSInt.SIntData);
		break;
		
	  case FOAM_BVal_SIntXOr:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(argv[0]->foamSInt.SIntData ^
				   argv[1]->foamSInt.SIntData);
		break;
		
	  case FOAM_BVal_SIntHashCombine:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_SInt);
		assert(foamTag(argv[1]) == FOAM_SInt);
		foam = foamNewSInt(hashCombinePair(argv[0]->foamSInt.SIntData,
					       argv[1]->foamSInt.SIntData));
		break;

	  case FOAM_BVal_BInt0:
		if (!cfoldFoldAll) break;
		foam = foamNewBInt(bint0);
		break;
	  case FOAM_BVal_BInt1:
		if (!cfoldFoldAll) break;
		foam = foamNewBInt(bint1);
		break;
	  case FOAM_BVal_BIntIsZero:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		foam = foamNewBool(bintIsZero(argv[0]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntIsNeg:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		foam = foamNewBool(bintIsNeg(argv[0]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntIsPos:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		foam = foamNewBool(bintIsPos(argv[0]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntIsEven:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		/*!! fill in later 
		foam = foamNewBool((argv[0]->foamBInt.BIntData % 2) == 0);
		*/
		break;
	  case FOAM_BVal_BIntIsOdd:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		/*!! fill in later 
		foam = foamNewBool((argv[0]->foamBInt.BIntData % 2) == 1);
		*/
		break;
	  case FOAM_BVal_BIntEQ:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		foam = foamNewBool(bintEQ(argv[0]->foamBInt.BIntData,
					 argv[1]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntNE:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		foam = foamNewBool(bintNE(argv[0]->foamBInt.BIntData,
					 argv[1]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntLT:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		foam = foamNewBool(bintLT(argv[0]->foamBInt.BIntData,
					 argv[1]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntLE:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		foam = foamNewBool(bintLE(argv[0]->foamBInt.BIntData,
					 argv[1]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntNegate:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		foam = foamNewBInt(bintNegate(argv[0]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntPrev:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		foam = foamNewBInt(bintMinus(argv[0]->foamBInt.BIntData,
				             bint1));
		break;
	  case FOAM_BVal_BIntNext:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		foam = foamNewBInt(bintPlus(argv[0]->foamBInt.BIntData,
				            bint1));
		break;
	  case FOAM_BVal_BIntPlus:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		foam = foamNewBInt(bintPlus(argv[0]->foamBInt.BIntData,
					    argv[1]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntMinus:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		foam = foamNewBInt(bintMinus(argv[0]->foamBInt.BIntData,
					     argv[1]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntTimes:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		foam = foamNewBInt(bintTimes(argv[0]->foamBInt.BIntData,
					     argv[1]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntTimesPlus:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_BInt);
		assert(foamTag(argv[1]) == FOAM_BInt);
		assert(foamTag(argv[2]) == FOAM_BInt);
		foam = foamNewBInt(bintPlus(
					bintTimes(argv[0]->foamBInt.BIntData,
					          argv[1]->foamBInt.BIntData),
					argv[2]->foamBInt.BIntData));
		break;
	  case FOAM_BVal_BIntMod:
		if (!cfoldFoldAll) break;
		/*!! fill in later */
		break;
	  case FOAM_BVal_BIntQuo:
		if (!cfoldFoldAll) break;
		/*!! fill in later */
		break;
	  case FOAM_BVal_BIntRem:
		if (!cfoldFoldAll) break;
		/*!! fill in later */
		break;
	  case FOAM_BVal_BIntDivide:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_BIntGcd:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_BIntSIPower:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_BIntBIPower:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	case FOAM_BVal_BIntPowerMod:
	        if (!cfoldFoldAll) break;
	        break; /*!! fill in later */
	  case FOAM_BVal_BIntLength:
		if (!cfoldFoldAll) break;
		foam = foamNewSInt(bintLength(argv[0]->foamBInt.BIntData));
		break;

          case FOAM_BVal_BIntShiftUp:
                if (!cfoldFoldAll) break;
                assert(foamTag(argv[0]) == FOAM_BInt);
                assert(foamTag(argv[1]) == FOAM_SInt);
                foam = foamNewBInt(bintShift(argv[0]->foamBInt.BIntData,
                                             argv[1]->foamSInt.SIntData));
                break;
          case FOAM_BVal_BIntShiftDn:
                if (!cfoldFoldAll) break;
                assert(foamTag(argv[0]) == FOAM_BInt);
                assert(foamTag(argv[1]) == FOAM_SInt);
                foam = foamNewBInt(bintShift(argv[0]->foamBInt.BIntData,
                                            -argv[1]->foamSInt.SIntData));
                break;

	  case FOAM_BVal_BIntBit:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
		
	  case FOAM_BVal_PtrNil:
		if (!cfoldFoldAll) break;
		foam = foamNewNil();
		break;
	  case FOAM_BVal_PtrIsNil:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_Ptr);
		foam = foamNewBool(argv[0]->foamPtr.val == 0);
		break;
	  case FOAM_BVal_PtrEQ:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_Ptr);
		assert(foamTag(argv[1]) == FOAM_Ptr);
		foam = foamNewBool(argv[0]->foamPtr.val ==
				   argv[1]->foamPtr.val);
		break;
	  case FOAM_BVal_PtrNE:
		if (!cfoldFoldFloat) break;
		assert(foamTag(argv[0]) == FOAM_Ptr);
		assert(foamTag(argv[1]) == FOAM_Ptr);
		foam = foamNewBool(argv[0]->foamPtr.val !=
				   argv[1]->foamPtr.val);
		break;
	  case FOAM_BVal_FormatSFlo:
		if (!cfoldFoldFloat) break;
		break; /*!! fill in later */
	  case FOAM_BVal_FormatDFlo:
		if (!cfoldFoldFloat) break;
		break; /*!! fill in later */
	  case FOAM_BVal_FormatSInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_FormatBInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
		
	  case FOAM_BVal_ScanSFlo:
		if (!cfoldFoldFloat) break;
		break; /*!! fill in later */
	  case FOAM_BVal_ScanDFlo:
		if (!cfoldFoldFloat) break;
		break; /*!! fill in later */
	  case FOAM_BVal_ScanSInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_ScanBInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
		
	  case FOAM_BVal_SFloToDFlo:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_DFloToSFlo:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_ByteToSInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntToByte:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_HIntToSInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntToHInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntToBInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_BIntToSInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntToSFlo:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntToDFlo:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_BIntToSFlo:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_BIntToDFlo:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_PtrToSInt:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */
	  case FOAM_BVal_SIntToPtr:
		if (!cfoldFoldAll) break;
		break; /*!! fill in later */

	  case FOAM_BVal_ArrToSFlo:
	      /*if (!cfoldFoldFloat) break;*/
		assert(foamTag(argv[0]) == FOAM_Arr);
		assert(argv[0]->foamArr.baseType == FOAM_Char);
		s = cfoldArrToString(argv[0]);
		foam = foamNewSFlo((SFloat) atof(s));
		strFree(s);
		break;
	  case FOAM_BVal_ArrToDFlo:
	      /*if (!cfoldFoldFloat) break;*/
		assert(foamTag(argv[0]) == FOAM_Arr);
		assert(argv[0]->foamArr.baseType == FOAM_Char);
		s = cfoldArrToString(argv[0]);
		foam = foamNewDFlo(atof(s));
		strFree(s);
		break;
	  case FOAM_BVal_ArrToSInt:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Arr);
		assert(argv[0]->foamArr.baseType == FOAM_Char);
		s = cfoldArrToString(argv[0]);
		foam = foamNewSInt(fiArrToSInt(s));
		strFree(s);
		break;
	  case FOAM_BVal_ArrToBInt:
		if (!cfoldFoldAll) break;
		assert(foamTag(argv[0]) == FOAM_Arr);
		assert(argv[0]->foamArr.baseType == FOAM_Char);
		s = cfoldArrToString(argv[0]);
		foam = foamNewBInt(bintFrString(s));
		strFree(s);
		break;


	  case FOAM_BVal_RawRepSize:
	  {
		Bool		fold = true;
		FoamBValTag	bvtag = FOAM_BVAL_LIMIT;


		/* Only do this when folding everything */
		if (!cfoldFoldAll) break;


		/* Argument must be a BCall */
		if (foamTag(argv[0]) != FOAM_BCall) break;


		/*
		 * Check if arg is a BVal_Type* call and replace
		 * with BVal_*Size call.
		 */
		switch (argv[0]->foamBCall.op)
		{
#if 0
		  case FOAM_BVal_TypeInt8:
			bvtag = FOAM_BVal_SizeOfInt8;
			break;
		  case FOAM_BVal_TypeInt16:
			bvtag = FOAM_BVal_SizeOfInt16;
			break;
		  case FOAM_BVal_TypeInt32:
			bvtag = FOAM_BVal_SizeOfInt32;
			break;
		  case FOAM_BVal_TypeInt64:
			bvtag = FOAM_BVal_SizeOfInt64;
			break;
		  case FOAM_BVal_TypeInt128:
			bvtag = FOAM_BVal_SizeOfInt128;
			break;
#endif
		  case FOAM_BVal_TypeNil:  bvtag = FOAM_BVal_SizeOfNil;  break;
		  case FOAM_BVal_TypeChar: bvtag = FOAM_BVal_SizeOfChar; break;
		  case FOAM_BVal_TypeBool: bvtag = FOAM_BVal_SizeOfBool; break;
		  case FOAM_BVal_TypeByte: bvtag = FOAM_BVal_SizeOfByte; break;
		  case FOAM_BVal_TypeHInt: bvtag = FOAM_BVal_SizeOfHInt; break;
		  case FOAM_BVal_TypeSInt: bvtag = FOAM_BVal_SizeOfSInt; break;
		  case FOAM_BVal_TypeBInt: bvtag = FOAM_BVal_SizeOfBInt; break;
		  case FOAM_BVal_TypeSFlo: bvtag = FOAM_BVal_SizeOfSFlo; break;
		  case FOAM_BVal_TypeDFlo: bvtag = FOAM_BVal_SizeOfDFlo; break;
		  case FOAM_BVal_TypeWord: bvtag = FOAM_BVal_SizeOfWord; break;
		  case FOAM_BVal_TypeClos: bvtag = FOAM_BVal_SizeOfClos; break;
		  case FOAM_BVal_TypePtr:  bvtag = FOAM_BVal_SizeOfPtr;  break;
		  case FOAM_BVal_TypeRec:  bvtag = FOAM_BVal_SizeOfRec;  break;
		  case FOAM_BVal_TypeArr:  bvtag = FOAM_BVal_SizeOfArr;  break;
		  case FOAM_BVal_TypeTR:   bvtag = FOAM_BVal_SizeOfTR;   break;
		  default:                 fold  = false; break;
		}

		if (fold) foam = foamNew(FOAM_BCall, 1, bvtag);
		break;
	  }
	  default:
		break;
	}
	if (foam != bcall)
		for(i=0; i < nargs; i++) stoFree(argv[i]);
	return foam;
}

local Bool
cfoldIsConst(Foam foam)
{
	int	tag = foamTag(foam);
	if (tag >= FOAM_DATA_START && tag < FOAM_DATA_LIMIT) return true;
	switch (tag) {
	  case FOAM_Arr: /* Fall through */
	  case FOAM_Rec:
		return true;
	  case FOAM_BInt:
		return true;
	  case FOAM_BCall:
		return cfoldBCallIsConst(foam);
	  default:
		return false;
	}
}

local Bool
cfoldBCallIsConst(Foam bcall)
{
	/* Safety check */
	assert(foamTag(bcall) == FOAM_BCall);


	/* Which BCalls are constants? */
	switch (bcall->foamBCall.op)
	{
#if 0
		case FOAM_BVal_TypeInt8:/* Fall through */
		case FOAM_BVal_TypeInt16:/* Fall through */
		case FOAM_BVal_TypeInt32:/* Fall through */
		case FOAM_BVal_TypeInt64:/* Fall through */
		case FOAM_BVal_TypeInt128:/* Fall through */
#endif
		case FOAM_BVal_TypeNil:/* Fall through */
		case FOAM_BVal_TypeChar:/* Fall through */
		case FOAM_BVal_TypeBool:/* Fall through */
		case FOAM_BVal_TypeByte:/* Fall through */
		case FOAM_BVal_TypeHInt:/* Fall through */
		case FOAM_BVal_TypeSInt:/* Fall through */
		case FOAM_BVal_TypeBInt:/* Fall through */
		case FOAM_BVal_TypeSFlo:/* Fall through */
		case FOAM_BVal_TypeDFlo:/* Fall through */
		case FOAM_BVal_TypeWord:/* Fall through */
		case FOAM_BVal_TypeClos:/* Fall through */
		case FOAM_BVal_TypePtr:/* Fall through */
		case FOAM_BVal_TypeRec:/* Fall through */
		case FOAM_BVal_TypeArr:/* Fall through */
		case FOAM_BVal_TypeTR:return true;
	}


	/* Probably not constant */
	return false;
}

local String
cfoldArrToString(Foam arr)
{
	String	s;
	int	i;

	s = strAlloc(foamArgc(arr));
	for(i=0; i < foamArgc(arr)-1; i++) s[i] = arr->foamArr.eltv[i];
	s[i] = '\0';
	return s;
}

#define isIntType(type) ((type) == FOAM_SInt || (type) == FOAM_HInt ||  \
			 (type) == FOAM_Word || (type) == FOAM_BInt)

local Foam
cfoldCast(Foam expr)
{
	Foam arg;
	int  type;

	
	type = expr->foamCast.type;
	arg = expr->foamCast.expr;

	if (foamTag(arg) == FOAM_Cast && arg->foamCast.type == FOAM_Word
	    && foamExprType(arg->foamCast.expr,
			    cfoldProgram,cfoldFormats,NULL,NULL,NULL) == type)
		return arg->foamCast.expr;

	if (!isIntType(type)) 		     return expr;

	if (foamTag(arg) == FOAM_Cast && foamTag(arg->foamCast.expr) == type)
		return arg->foamCast.expr;
	if (!isIntType(type)) 		     return expr;
	if (foamExprType(arg,cfoldProgram,cfoldFormats,NULL,NULL,NULL) == type)
		return arg;
	if (foamTag(arg) != FOAM_Cast) 	     return expr;
	if (!isIntType(arg->foamCast.type))  return expr;
	arg = arg->foamCast.expr;
	if (foamExprType(arg,cfoldProgram,cfoldFormats,NULL,NULL,NULL) == type)
		return arg;

	if (foamTag(arg) != FOAM_Cast) 	     return expr;
	if (arg->foamCast.type != type)	     return expr;
	return arg->foamCast.expr;
}
	
local void
cfoldMoveConstExports(Foam unit)
{
	int	i;
	Foam	def;
	Foam	defs = unit->foamUnit.defs;

	assert(foamTag(defs) == FOAM_DDef);

	cfoldConstDecls = fboxNew(foamUnitConstants(unit));
	cfoldConstDefs  = fboxNew(unit->foamUnit.defs);
	for(i=0; i < foamArgc(defs); i++)  {
		def = defs->foamDDef.argv[i];
		assert(foamTag(def) == FOAM_Def);
		if (foamTag(def->foamDef.rhs) == FOAM_Prog) 
			cfoldMoveProgConsts(def->foamDef.rhs);
	}
	foamUnitConstants(unit) = fboxMake(cfoldConstDecls);
	unit->foamUnit.defs     = fboxMake(cfoldConstDefs);
}

local void
cfoldMoveProgConsts(Foam prog)
{
	Foam	*body = prog->foamProg.body->foamSeq.argv;
	int	i;

	assert(foamTag(prog->foamProg.body) == FOAM_Seq);
	cfoldProgram = prog;
	for(i=0; i<foamArgc(prog->foamProg.body); i++)
		if (foamTag(body[i]) == FOAM_Def)
			cfoldConstDef(body[i]);
}

local void
cfoldConstDef(Foam def)
{
	Foam	lhs = def->foamDef.lhs, rhs = def->foamDef.rhs;
	Syme	syme = foamSyme(lhs);
	Foam	newDef, decl;
	UShort	index;

	if (!foamIsData(rhs)) return;
	if (!genIsLocalConst(syme)) return;

	if (genHasConstNum(syme)) return;

	decl=foamNewDecl(foamExprType(rhs,cfoldProgram,
				      cfoldFormats,NULL,NULL,NULL),
		   	 strCopy(symeString(syme)), emptyFormatSlot);

	index = fboxAdd(cfoldConstDecls, decl);
	newDef = foamNewDef(foamNewConst(index), foamCopy(rhs));
	foamSyme(newDef->foamDef.lhs) = syme;
	/* FIXME */
	genSetConstNum(syme, def->foamDef.hdr.defnId, index, false);
	fboxAdd(cfoldConstDefs, newDef);
	cfoldMovedConsts = true;
}
