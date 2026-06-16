/*****************************************************************************
 *
 * abuse.c: Determine node use by context.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "phase.h"
#include "spesym.h"
#include "comsg.h"
#include "debug.h"
#include "format.h"
#include "util.h"

local AbSyn abPutUseInner(AbSyn ab, AbUse context);
local Bool  abPutPatternUse(AbSyn ab, Bool isPattern); // TODO, does context => pattern sometimes

/*
 * :: abPutUse
 */

static AbSyn	current = 0;

/*
 * Test type of context.
 */
#define isImplicitNoValue(context, parent) \
	((context) == AB_Use_NoValue && (parent) && abTag(parent) != AB_Do)

/*
 * Tests for suspicious forms in no value context.
 */
#define isFunnyEquals(context, absyn, parent) \
   	(isImplicitNoValue(context,parent) && \
	 abIsTheId((absyn)->abApply.op,ssymEquals))

#define isFunnyColon(context, absyn, parent) \
	(isImplicitNoValue(context,parent))

void
abPutUse(AbSyn absyn, AbUse context)
{
	abPutUseInner(absyn, context);
	abPutPatternUse(absyn, false);
}

local AbSyn
abPutUseInner(AbSyn absyn, AbUse context)
{
	int	i, argc;
	AbSyn	parent;
	AbUse	con1;

	if (!absyn) return absyn;

	parent  = current;
	current = absyn;

	switch (abTag(absyn)) {
	  case AB_Id:
	  case AB_LitInteger:
	  case AB_LitFloat:
	  case AB_LitString:
		break;
	  case AB_Blank:
		if (context == AB_Use_Type) {
		        abUse(absyn) = context;
		}
		break;
	  case AB_Apply:
		con1 =  abIsApplyOf(absyn, ssymJoin) || abIsAnyMap(absyn)
			? AB_Use_Type
			: AB_Use_Value;

		for (i = 0; i < abArgc(absyn); i++)
			abPutUseInner(abArgv(absyn)[i], con1);

		if (isFunnyEquals(context, absyn, parent))
			comsgWarning(absyn, ALDOR_W_FunnyEquals);
		break;
	  case AB_Default:
		abPutUseInner(absyn->abDefault.body, AB_Use_Default);
		break;
	  case AB_Define:
		abPutUseInner(absyn->abDefine.lhs,  AB_Use_Define);
		abPutUseInner(absyn->abDefine.rhs,  AB_Use_Value);
		break;
	  case AB_Assign:
		abPutUseInner(absyn->abAssign.lhs,  AB_Use_Assign);
		abPutUseInner(absyn->abAssign.rhs,  AB_Use_Value);
		break;
	  case AB_Declare:
		abPutUseInner(absyn->abDeclare.id,   context);
		abPutUseInner(absyn->abDeclare.type, AB_Use_Type);

		if (isFunnyColon(context, absyn, parent))
			comsgWarning(absyn, ALDOR_W_FunnyColon);
		break;
	  case AB_Label:
		abPutUseInner(absyn->abLabel.label, AB_Use_Label);
		abPutUseInner(absyn->abLabel.expr,  context);
		break;
	  case AB_Lambda:
	  case AB_PLambda:
		abPutUseInner(absyn->abLambda.param, AB_Use_Declaration);
		abPutUseInner(absyn->abLambda.rtype, AB_Use_Type);
		abPutUseInner(absyn->abLambda.body,  AB_Use_RetValue);
		break;
	  case AB_Generate:
		abPutUseInner(absyn->abGenerate.count,AB_Use_Value);
		abPutUseInner(absyn->abGenerate.body, AB_Use_NoValue);
		break;
	  case AB_Reference:
		/* No idea what sort of use this ought to be ... */
		abPutUseInner(absyn->abReference.body, AB_Use_Value);
		break;
	  case AB_Add:
		abPutUseInner(absyn->abAdd.base,     AB_Use_Type);
		abPutUseInner(absyn->abAdd.capsule,  AB_Use_Declaration);
		break;
	  case AB_With:
		abPutUseInner(absyn->abWith.base,    AB_Use_Type);
		abPutUseInner(absyn->abWith.within,  AB_Use_Declaration);
		break;
	  case AB_Where:
		abPutUseInner(absyn->abWhere.context,AB_Use_Declaration);
		abPutUseInner(absyn->abWhere.expr,   context);
		break;
	  case AB_If:
		abPutUseInner(absyn->abIf.test,    AB_Use_Value);
		abPutUseInner(absyn->abIf.thenAlt, context);
		abPutUseInner(absyn->abIf.elseAlt, context);
		break;
	  case AB_Exit:
		abPutUseInner(absyn->abExit.test,  AB_Use_Value);
		abPutUseInner(absyn->abExit.value, context);
		break;
	  case AB_Test:
		abPutUseInner(absyn->abTest.cond,  AB_Use_Value);
		break;
	  case AB_Repeat:
		argc = abRepeatIterc(absyn);
		for (i = 0; i < argc; i++)
			abPutUseInner(absyn->abRepeat.iterv[i], AB_Use_Iterator);
		abPutUseInner(absyn->abRepeat.body, AB_Use_NoValue);
		break;
	  case AB_While:
		abPutUseInner(absyn->abWhile.test,  AB_Use_Value);
		break;
	  case AB_For:
		abPutUseInner(absyn->abFor.lhs,     AB_Use_Assign);
		abPutUseInner(absyn->abFor.whole,   AB_Use_Value);
		abPutUseInner(absyn->abFor.test,    AB_Use_Value);
		break;
	  case AB_Local:
		for (i = 0; i < abArgc(absyn); i++)
			abPutUseInner(absyn->abLocal.argv[i], AB_Use_Declaration);
		break;
	  case AB_Free:
		for (i = 0; i < abArgc(absyn); i++)
			abPutUseInner(absyn->abFree.argv[i], AB_Use_Declaration);
		break;
	  case AB_ForeignImport:
		abPutUseInner(absyn->abForeignImport.what,   AB_Use_Declaration);
		abPutUseInner(absyn->abForeignImport.origin, AB_Use_Type);
		break;
	  case AB_Builtin:
		abPutUseInner(absyn->abBuiltin.what, AB_Use_Declaration);
		break;
	  case AB_Import:
		abPutUseInner(absyn->abImport.what,   AB_Use_Declaration);
		abPutUseInner(absyn->abImport.origin, AB_Use_Type);
		break;
	  case AB_Inline:
		abPutUseInner(absyn->abInline.what,   AB_Use_Declaration);
		abPutUseInner(absyn->abInline.origin, AB_Use_Type);
		break;
	  case AB_Export:
		abPutUseInner(absyn->abExport.what,        AB_Use_Declaration);
		abPutUseInner(absyn->abExport.destination, AB_Use_Type);
		break;
	  case AB_ForeignExport:
		abPutUseInner(absyn->abForeignExport.what, AB_Use_Declaration);
		abPutUseInner(absyn->abForeignExport.dest, AB_Use_Type);
		break;
	  case AB_Extend:
		abPutUseInner(absyn->abExtend.body, AB_Use_Declaration);
		break;
	  case AB_Qualify:
		abPutUseInner(absyn->abQualify.what,   AB_Use_Value);
		abPutUseInner(absyn->abQualify.origin, AB_Use_Type);
		break;
	  case AB_RestrictTo:
		abPutUseInner(absyn->abRestrictTo.expr, AB_Use_Value);
		abPutUseInner(absyn->abRestrictTo.type, AB_Use_Type);
	  	break;
	  case AB_PretendTo:
		abPutUseInner(absyn->abPretendTo.expr, AB_Use_Value);
		abPutUseInner(absyn->abPretendTo.type, AB_Use_Type);
		break;
	  case AB_Comma:
		for (i = 0; i < abArgc(absyn); i++)
			abPutUseInner(abArgv(absyn)[i], context);
		break;
	  case AB_Sequence:
		argc = abArgc(absyn);

		con1 = AB_Use_NoValue;
		if (context == AB_Use_Declaration || context == AB_Use_Default)
			con1 = context;

		for (i = 0; i < argc; i++) {
			AbSyn si   = abArgv(absyn)[i];
			Bool  ex   = abTag(si) == AB_Exit;
			Bool  last = (i == argc - 1);
			AbUse au;

			au = (ex || last) ? context : con1;

			abPutUseInner(si, au);
		}
		break;
	  default:
		if (!abIsLeaf(absyn))
			for (i = 0; i < abArgc(absyn); i++)
				abPutUseInner(abArgv(absyn)[i], AB_Use_Value);
		break;
	}
	abUse(absyn) = context;
	current      = parent;
	return absyn;
}

/*
 * :: Patterns.
 *
 * Rules are that:
 * - 'case' can contain a pattern 
 * - 'select' (lhs) can contain a pattern (not yet)
 * - Patterns terminate in a nullary call, or a blank
 * - We allow assignment inside a pattern (not yet)
 */
#define PATTERN_UNCHECKED

#ifdef PATTERN_UNCHECKED
// Not quite working 'value/pattern can be mixed' case
	
Bool
abPutPatternUse(AbSyn ab, Bool isPattern)
{
	switch (abTag(ab)) {
		
	case AB_Select:
		abPutPatternUse(ab->abSelect.testPart, isPattern);
		abPutPatternUse(ab->abSelect.alternatives, true);
		return false;

	case AB_Sequence:
		for (int i = 0; i<abArgc(ab); i++) {
			abPutPatternUse(abArgv(ab)[i], isPattern);
		}
		return false;

	case AB_Test: {
		abPutPatternUse(ab->abTest.cond, isPattern);
		return false;
	}
	case AB_Exit: {
		abPutPatternUse(ab->abExit.test, isPattern);
		abPutPatternUse(ab->abExit.value, false);
		return false;
	}
	case AB_Define: {
		abPutPatternUse(ab->abDefine.lhs, false);
		abPutPatternUse(ab->abDefine.rhs, isPattern);
		return false;
	}
	case AB_Apply: {
		if (!isPattern && abIsApplyOf(ab, ssymTheCase)) {
			abPutPatternUse(abApplyArg(ab, 0), false);
			abPutPatternUse(abApplyArg(ab, 1), true);
			return false;
		}
		Bool flg = isPattern && abApplyArgc(ab) == 0;
		for (int i=0; i<abApplyArgc(ab); i++) {
			flg |= abPutPatternUse(abApplyArg(ab, i), isPattern);
		}
		flg |= abPutPatternUse(ab->abApply.op, isPattern);
		if (isPattern) {
			abUse(ab) = AB_Use_PatLocation;
		}
		return flg;
	}
	case AB_Id: {
		if (isPattern) {
			abUse(ab) = AB_Use_PatLocation;
		}
		break;
	}
	case AB_Blank: {
		if (abUse(ab) == AB_Use_Type)
			return true;
		if (!isPattern) {
			comsgError(ab, ALDOR_E_BlankOutsidePattern);
			return false;
		}
		// TODO: Expect value context
		abUse(ab) = AB_Use_Pattern;
		return true;
	}
	case AB_Comma: {
		for (int i=0; i<abCommaArgc(ab); i++) {
			abPutPatternUse(abCommaArg(ab, i), isPattern);
		}
		return isPattern; //??
	}
	default:
		if (!abIsLeaf(ab)) {
			for (int i = 0; i<abArgc(ab); i++) {
				abPutPatternUse(abArgv(ab)[i], false);
			}
		}
	}
	return false;
}
#endif

#ifdef PATTERNS_MUST_CONTAIN_BLANKS
local void abPutPatternUseSelect(AbSyn ab);
local void abPutPatternUseSelectTerm(AbSyn ab);
local void abPutPatternUseGeneric(AbSyn ab);
local void abPutPatternUseCase(AbSyn ab);
local Bool abContainsBlank(AbSyn ab);
local void abPutPatternUseFillMatch(AbSyn ab);

local void
abPutPatternUse(AbSyn ab)
{
	if (abIsLeaf(ab)) {
		return;
	}
	
	switch (abTag(ab)) {
	case AB_Select:
		abPutPatternUseSelect(ab);
		break;
	case AB_Apply:
		if (abIsApplyOf(ab, ssymTheCase)) {
			abPutPatternUseCase(ab);
		}
		abPutPatternUseGeneric(ab);
		break;
	default:
		abPutPatternUseGeneric(ab);
	}
}

local void
abPutPatternUseSelect(AbSyn ab)
{
	abPutPatternUse(ab->abSelect.testPart);
	abPutPatternUseSelectTerm(ab->abSelect.alternatives);
}

local void
abPutPatternUseSelectTerm(AbSyn ab)
{
	int i;
	
	if (abIsLeaf(ab)) {
		return;
	}
	switch (abTag(ab)) {
	case AB_Test:
		abPutPatternUseFillMatch(ab->abTest.cond);
		break;
	case AB_Exit:
		abPutPatternUseSelectTerm(ab->abExit.test);
		abPutPatternUse(ab->abExit.value);
		break;
	case AB_Sequence:
		for (int i=0; i<abArgc(ab); i++) {
			abPutPatternUseSelectTerm(abArgv(ab)[i]);
		}
		break;
	default:
		abPutPatternUseGeneric(ab);
	}
}

local void
abPutPatternUseGeneric(AbSyn ab)
{
	int i;
	
	if (abIsLeaf(ab)) {
		bug("unexpected leaf - %s", aStrPrintf("%pAbSyn", ab));
	}
	for (int i=0; i<abArgc(ab); i++) {
		abPutPatternUse(abArgv(ab)[i]);
	}
}

local void
abPutPatternUseCase(AbSyn ab)
{
	abPutPatternUse(ab->abApply.argv[0]);

	if (abContainsBlank(ab->abApply.argv[1])) {
		abPutPatternUseFillMatch(ab->abApply.argv[1]);
	}
	else {
	  abPutPatternUse(ab->abApply.argv[1]);
	}
}

// NB: Check use of return value
local Bool
abContainsBlank(AbSyn ab)
{
	int i;
	
	switch (abTag(ab)) {
	case AB_Id:
		return false;
	case AB_Blank:
		return true;
	default: {
		for (i=0; i<abArgc(ab); i++) {
			if (abContainsBlank(abArgv(ab)[i])) {
				return true;
			}
		}
		return false;
	}
	}
}

local void
abPutPatternUseFillMatch(AbSyn ab)
{
	int i;
	
	switch (abTag(ab)) {
	case AB_Id:
		comsgError(ab, ALDOR_E_BadPatternContent);
		return;
	case AB_Blank:
		abUse(ab) = AB_Use_Pattern;
		break;
	case AB_Apply:
		abUse(ab) = AB_Use_Pattern;
		for (i=0; i<abApplyArgc(ab); i++) {
			abPutPatternUseFillMatch(ab->abApply.argv[i]);
		}
	}
}

#endif
