/*****************************************************************************
 *
 * abuse.c: Determine node use by context.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "phase.h"

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

AbSyn
abPutUse(AbSyn absyn, AbUse context)
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
	  case AB_Apply:
		con1 =  abIsApplyOf(absyn, ssymJoin) || abIsAnyMap(absyn)
			? AB_Use_Type
			: AB_Use_Value;

		for (i = 0; i < abArgc(absyn); i++)
			abPutUse(abArgv(absyn)[i], con1);

		if (isFunnyEquals(context, absyn, parent))
			comsgWarning(absyn, ALDOR_W_FunnyEquals);
		break;
	  case AB_Default:
		abPutUse(absyn->abDefault.body, AB_Use_Default);
		break;
	  case AB_Define:
		abPutUse(absyn->abDefine.lhs,  AB_Use_Define);
		abPutUse(absyn->abDefine.rhs,  AB_Use_Value);
		break;
	  case AB_Assign:
		abPutUse(absyn->abAssign.lhs,  AB_Use_Assign);
		abPutUse(absyn->abAssign.rhs,  AB_Use_Value);
		break;
	  case AB_Declare:
		abPutUse(absyn->abDeclare.id,   context);
		abPutUse(absyn->abDeclare.type, AB_Use_Type);

		if (isFunnyColon(context, absyn, parent))
			comsgWarning(absyn, ALDOR_W_FunnyColon);
		break;
	  case AB_Label:
		abPutUse(absyn->abLabel.label, AB_Use_Label);
		abPutUse(absyn->abLabel.expr,  context);
		break;
	  case AB_Lambda:
	  case AB_PLambda:
		abPutUse(absyn->abLambda.param, AB_Use_Declaration);
		abPutUse(absyn->abLambda.rtype, AB_Use_Type);
		abPutUse(absyn->abLambda.body,  AB_Use_RetValue);
		break;
	  case AB_Generate:
		abPutUse(absyn->abGenerate.count,AB_Use_Value);
		abPutUse(absyn->abGenerate.body, AB_Use_NoValue);
		break;
	  case AB_Reference:
		/* No idea what sort of use this ought to be ... */
		abPutUse(absyn->abReference.body, AB_Use_Value);
		break;
	  case AB_Add:
		abPutUse(absyn->abAdd.base,     AB_Use_Type);
		abPutUse(absyn->abAdd.capsule,  AB_Use_Declaration);
		break;
	  case AB_With:
		abPutUse(absyn->abWith.base,    AB_Use_Type);
		abPutUse(absyn->abWith.within,  AB_Use_Declaration);
		break;
	  case AB_Where:
		abPutUse(absyn->abWhere.context,AB_Use_Declaration);
		abPutUse(absyn->abWhere.expr,   context);
		break;
	  case AB_If:
		abPutUse(absyn->abIf.test,    AB_Use_Value);
		abPutUse(absyn->abIf.thenAlt, context);
		abPutUse(absyn->abIf.elseAlt, context);
		break;
	  case AB_Exit:
		abPutUse(absyn->abExit.test,  AB_Use_Value);
		abPutUse(absyn->abExit.value, context);
		break;
	  case AB_Test:
		abPutUse(absyn->abTest.cond,  AB_Use_Value);
		break;
	  case AB_Repeat:
		argc = abRepeatIterc(absyn);
		for (i = 0; i < argc; i++)
			abPutUse(absyn->abRepeat.iterv[i], AB_Use_Iterator);
		abPutUse(absyn->abRepeat.body, AB_Use_NoValue);
		break;
	  case AB_While:
		abPutUse(absyn->abWhile.test,  AB_Use_Value);
		break;
	  case AB_For:
		abPutUse(absyn->abFor.lhs,     AB_Use_Assign);
		abPutUse(absyn->abFor.whole,   AB_Use_Value);
		abPutUse(absyn->abFor.test,    AB_Use_Value);
		break;
	  case AB_Local:
		for (i = 0; i < abArgc(absyn); i++)
			abPutUse(absyn->abLocal.argv[i], AB_Use_Declaration);
		break;
	  case AB_Free:
		for (i = 0; i < abArgc(absyn); i++)
			abPutUse(absyn->abFree.argv[i], AB_Use_Declaration);
		break;
	  case AB_Foreign:
		abPutUse(absyn->abForeign.what,   AB_Use_Declaration);
		abPutUse(absyn->abForeign.origin, AB_Use_Type);
		break;
	  case AB_Builtin:
		abPutUse(absyn->abBuiltin.what, AB_Use_Declaration);
		break;
	  case AB_Import:
		abPutUse(absyn->abImport.what,   AB_Use_Declaration);
		abPutUse(absyn->abImport.origin, AB_Use_Type);
		break;
	  case AB_Inline:
		abPutUse(absyn->abInline.what,   AB_Use_Declaration);
		abPutUse(absyn->abInline.origin, AB_Use_Type);
		break;
	  case AB_Export:
		abPutUse(absyn->abExport.what,        AB_Use_Declaration);
		abPutUse(absyn->abExport.destination, AB_Use_Type);
		break;
	  case AB_Extend:
		abPutUse(absyn->abExtend.body, AB_Use_Declaration);
		break;
	  case AB_Qualify:
		abPutUse(absyn->abQualify.what,   AB_Use_Value);
		abPutUse(absyn->abQualify.origin, AB_Use_Type);
		break;
	  case AB_RestrictTo:
		abPutUse(absyn->abRestrictTo.expr, AB_Use_Value);
		abPutUse(absyn->abRestrictTo.type, AB_Use_Type);
	  	break;
	  case AB_PretendTo:
		abPutUse(absyn->abPretendTo.expr, AB_Use_Value);
		abPutUse(absyn->abPretendTo.type, AB_Use_Type);
		break;
	  case AB_Comma:
		for (i = 0; i < abArgc(absyn); i++)
			abPutUse(abArgv(absyn)[i], context);
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

			abPutUse(si, au);
		}
		break;
	  default:
		if (!abIsLeaf(absyn))
			for (i = 0; i < abArgc(absyn); i++)
				abPutUse(abArgv(absyn)[i],AB_Use_Value);
		break;
	}
	abUse(absyn) = context;
	current      = parent;
	return absyn;
}
