/****************************************************************************
 *
 * ti_sef.c: Type inference -- sefo pass.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ***************************************************************************/

#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "spesym.h"
#include "stab.h"
#include "ti_sef.h"
#include "tinfer.h"
#include "terror.h"
#include "util.h"
#include "sefo.h"
#include "lib.h"
#include "tposs.h"
#include "tfsat.h"
#include "abpretty.h"

/* see ti_tdn.c for definitions of following */

extern TForm tuniYieldTForm, tuniReturnTForm, tuniExitTForm;

/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

Bool	tipSefDebug		= false;
#define tipSefDEBUG		DEBUG_IF(tipSefDebug)

/*****************************************************************************
 *
 * :: Declarations for sefo pass
 *
 ****************************************************************************/

local void	tisef0Generic	(Stab, Sefo);
local TForm	tisef0Within	(Stab, Sefo, SymeList);
local void	tisef0FarValue	(Stab, Sefo, TForm *);
local void	tisef0ApplySymIfNeeded
				(Stab, Sefo, Length, AbSynGetter, Sefo,
				 TFormPredicate);
local void	tisef0ApplySym	(Stab, Sefo, Length, AbSynGetter, Sefo);
local void	tisef0ApplyFType(Stab, Sefo, Sefo, Length, AbSynGetter);
local void	tisef0ApplyJoin (Stab, Sefo, Sefo, Length, AbSynGetter);
local void	tisef0ApplySpecialSyme
				(Stab, Sefo);

local void	tisefId		(Stab, Sefo);
local void	tisefIdSy	(Stab, Sefo);
local void	tisefBlank	(Stab, Sefo);
local void	tisefLitInteger (Stab, Sefo);
local void	tisefLitFloat	(Stab, Sefo);
local void	tisefLitString	(Stab, Sefo);
local void	tisefAdd	(Stab, Sefo);
local void	tisefAnd	(Stab, Sefo);
local void	tisefApply	(Stab, Sefo);
local void	tisefAssert	(Stab, Sefo);
local void	tisefAssign	(Stab, Sefo);
local void	tisefBreak	(Stab, Sefo);
local void	tisefBuiltin	(Stab, Sefo);
local void	tisefCoerceTo	(Stab, Sefo);
local void	tisefCollect	(Stab, Sefo);
local void	tisefComma	(Stab, Sefo);
local void	tisefDeclare	(Stab, Sefo);
local void	tisefDefault	(Stab, Sefo);
local void	tisefDefine	(Stab, Sefo);
local void	tisefDelay	(Stab, Sefo);
local void	tisefDo		(Stab, Sefo);
local void	tisefExcept	(Stab, Sefo);
local void	tisefRaise	(Stab, Sefo);
local void	tisefExit	(Stab, Sefo);
local void	tisefExport	(Stab, Sefo);
local void	tisefExtend	(Stab, Sefo);
local void	tisefFix	(Stab, Sefo);
local void	tisefFluid	(Stab, Sefo);
local void	tisefFor	(Stab, Sefo);
local void	tisefForeign	(Stab, Sefo);
local void	tisefFree	(Stab, Sefo);
local void	tisefGenerate	(Stab, Sefo);
local void	tisefReference	(Stab, Sefo);
local void	tisefGoto	(Stab, Sefo);
local void	tisefHas	(Stab, Sefo);
local void	tisefHide	(Stab, Sefo);
local void	tisefIf		(Stab, Sefo);
local void	tisefImport	(Stab, Sefo);
local void	tisefInline	(Stab, Sefo);
local void	tisefIterate	(Stab, Sefo);
local void	tisefLabel	(Stab, Sefo);
local void	tisefLambda	(Stab, Sefo);
local void	tisefLocal	(Stab, Sefo);
local void	tisefLet	(Stab, Sefo);
local void	tisefMacro	(Stab, Sefo);
local void	tisefMLambda    (Stab, Sefo);
local void	tisefNever	(Stab, Sefo);
local void	tisefNot	(Stab, Sefo);
local void	tisefNothing	(Stab, Sefo);
local void	tisefOr		(Stab, Sefo);
local void	tisefPretendTo	(Stab, Sefo);
local void	tisefQualify	(Stab, Sefo);
local void	tisefQuote	(Stab, Sefo);
local void	tisefRepeat	(Stab, Sefo);
local void	tisefRestrictTo (Stab, Sefo);
local void	tisefReturn	(Stab, Sefo);
local void	tisefSelect	(Stab, Sefo);
local void	tisefSequence	(Stab, Sefo);
local void	tisefTest	(Stab, Sefo);
local void	tisefTry	(Stab, Sefo);
local void	tisefWhere	(Stab, Sefo);
local void	tisefWhile	(Stab, Sefo);
local void	tisefWith	(Stab, Sefo);
local void	tisefYield	(Stab, Sefo);

/*****************************************************************************
 *
 * :: Sefo pass
 *
 ****************************************************************************/

Bool
tiCanSefo(Sefo sefo)
{
	Bool	result = true;
	Length	i;

	if (abIsApply(sefo))
		tisef0ApplySpecialSyme(stabFile(), sefo);

	if (abIsId(sefo))
		result = abSyme(sefo) != NULL;

	else if (!abIsLeaf(sefo))
		for (i = 0; result && i < abArgc(sefo); i += 1)
			result = tiCanSefo(abArgv(sefo)[i]);

	return result;
}


/* Adjust the stab as needed to localize parameter symes in sefo. */
local Stab
tiSefoStab(Stab stab, Sefo sefo)
{
	if (abStab(sefo))
		return stab;

	if (abIsId(sefo)) {
		assert(abSyme(sefo));
		if (symeIsParam(abSyme(sefo)))
			stab = stabFindLevel(stab, abSyme(sefo));
	}

	else if (!abIsLeaf(sefo)) {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1)
			stab = tiSefoStab(stab, abArgv(sefo)[i]);
	}

	return stab;
}

void
tiSefo(Stab stab, Sefo sefo)
{
	Scope("tiSefo");

	TForm	fluid(tuniReturnTForm);
	TForm	fluid(tuniYieldTForm);
	TForm	fluid(tuniExitTForm);

	tuniReturnTForm = tfNone();
	tuniYieldTForm	= tfNone();
	tuniExitTForm	= tfNone();

	stab = tiSefoStab(stab, sefo);
	tisef(stab, sefo);

	ReturnNothing;
}

void
tisef(Stab stab, Sefo sefo)
{
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;

	assert(sefo);

	if (abState(sefo) == AB_State_HasUnique)
		return;

	if (abState(sefo) == AB_State_HasPoss) {
		TPoss	tposs = abTPoss(sefo);

		if (tpossIsUnique(tposs))
			abTUnique(sefo) = tpossUnique(tposs);
		else {
			terrorNotUniqueType(ALDOR_E_TinExprMeans,
					    sefo, tfNone(), tposs);
			abTUnique(sefo) = tfUnknown;
		}
		abState(sefo) = AB_State_HasUnique;
		tpossFree(tposs);
		return;
	}

	if (!abIsLeaf(sefo) && abStab(sefo))
		stab = abStab(sefo);

	serialNo += 1;
	depthNo	 += 1;
	serialThis = serialNo;
	tipSefDEBUG {
		fprintf(dbOut,"->Sef: %*s%d= ", depthNo, "", serialThis);
		abPrettyPrint(dbOut, sefo);
		fnewline(dbOut);
	}

	abState(sefo) = AB_State_HasUnique;
	abTUnique(sefo) = tfUnknown;

	switch (abTag(sefo)) {
	case AB_Id:		tisefId		(stab, sefo); break;
	case AB_IdSy:		tisefIdSy	(stab, sefo); break;
	case AB_Blank:		tisefBlank	(stab, sefo); break;
	case AB_LitInteger:	tisefLitInteger (stab, sefo); break;
	case AB_LitFloat:	tisefLitFloat	(stab, sefo); break;
	case AB_LitString:	tisefLitString	(stab, sefo); break;
	case AB_Add:		tisefAdd	(stab, sefo); break;
	case AB_And:		tisefAnd	(stab, sefo); break;
	case AB_Apply:		tisefApply	(stab, sefo); break;
	case AB_Assert:		tisefAssert	(stab, sefo); break;
	case AB_Assign:		tisefAssign	(stab, sefo); break;
	case AB_Break:		tisefBreak	(stab, sefo); break;
	case AB_Builtin:	tisefBuiltin	(stab, sefo); break;
	case AB_CoerceTo:	tisefCoerceTo	(stab, sefo); break;
	case AB_Collect:	tisefCollect	(stab, sefo); break;
	case AB_Comma:		tisefComma	(stab, sefo); break;
	case AB_Declare:	tisefDeclare	(stab, sefo); break;
	case AB_Default:	tisefDefault	(stab, sefo); break;
	case AB_Define:		tisefDefine	(stab, sefo); break;
	case AB_Delay:		tisefDelay	(stab, sefo); break;
	case AB_Do:		tisefDo		(stab, sefo); break;
	case AB_Except:		tisefExcept	(stab, sefo); break;
	case AB_Raise:		tisefRaise	(stab, sefo); break;
	case AB_Exit:		tisefExit	(stab, sefo); break;
	case AB_Export:		tisefExport	(stab, sefo); break;
	case AB_Extend:		tisefExtend	(stab, sefo); break;
	case AB_Fix:		tisefFix	(stab, sefo); break;
	case AB_Fluid:		tisefFluid	(stab, sefo); break;
	case AB_For:		tisefFor	(stab, sefo); break;
	case AB_Foreign:	tisefForeign	(stab, sefo); break;
	case AB_Free:		tisefFree	(stab, sefo); break;
	case AB_Generate:	tisefGenerate	(stab, sefo); break;
	case AB_Reference:	tisefReference	(stab, sefo); break;
	case AB_Goto:		tisefGoto	(stab, sefo); break;
	case AB_Has:		tisefHas	(stab, sefo); break;
	case AB_Hide:		tisefHide	(stab, sefo); break;
	case AB_If:		tisefIf		(stab, sefo); break;
	case AB_Import:		tisefImport	(stab, sefo); break;
	case AB_Inline:		tisefInline	(stab, sefo); break;
	case AB_Iterate:	tisefIterate	(stab, sefo); break;
	case AB_Label:		tisefLabel	(stab, sefo); break;
	case AB_Lambda:		tisefLambda	(stab, sefo); break;
	case AB_Let:		tisefLet	(stab, sefo); break;
	case AB_Local:		tisefLocal	(stab, sefo); break;
	case AB_Macro:		tisefMacro	(stab, sefo); break;
	case AB_MLambda:	tisefMLambda    (stab, sefo); break;
	case AB_Never:		tisefNever	(stab, sefo); break;
	case AB_Not:		tisefNot	(stab, sefo); break;
	case AB_Nothing:	tisefNothing	(stab, sefo); break;
	case AB_Or:		tisefOr		(stab, sefo); break;
	case AB_PLambda:	tisefLambda	(stab, sefo); break;
	case AB_PretendTo:	tisefPretendTo	(stab, sefo); break;
	case AB_Qualify:	tisefQualify	(stab, sefo); break;
	case AB_Quote:		tisefQuote	(stab, sefo); break;
	case AB_Repeat:		tisefRepeat	(stab, sefo); break;
	case AB_RestrictTo:	tisefRestrictTo (stab, sefo); break;
	case AB_Return:		tisefReturn	(stab, sefo); break;
	case AB_Select:		tisefSelect	(stab, sefo); break;
	case AB_Sequence:	tisefSequence	(stab, sefo); break;
	case AB_Test:		tisefTest	(stab, sefo); break;
	case AB_Try:		tisefTry	(stab, sefo); break;
	case AB_Where:		tisefWhere	(stab, sefo); break;
	case AB_While:		tisefWhile	(stab, sefo); break;
	case AB_With:		tisefWith	(stab, sefo); break;
	case AB_Yield:		tisefYield	(stab, sefo); break;
	default:		bugBadCase	(abTag(sefo));
	}

	tipSefDEBUG {
		fprintf(dbOut, "<-Sef: %*s%d= ", depthNo, "", serialThis);
		abPrettyPrint(dbOut, sefo);
		fprintf(dbOut, " @ ");
		tfPrint(dbOut, abTUnique(sefo));
		fnewline(dbOut);
	}
	depthNo -= 1;
}

/****************************************************************************
 *
 * :: Generic:	abArgc(ab), abArgv(ab)
 *
 ***************************************************************************/

local void
tisef0Generic(Stab stab, Sefo sefo)
{
	Length	i;
	if (!abIsLeaf(sefo))
		for (i = 0; i < abArgc(sefo); i += 1)
			tisef(stab, abArgv(sefo)[i]);
	abTUnique(sefo) = tfNone();
}

/****************************************************************************
 *
 * :: Within:	with bodies, default bodies, conditional branches
 *
 ***************************************************************************/

local TForm
tisef0Within(Stab stab, Sefo sefo, SymeList bsymes)
{
	SymeList	xsymes, isymes, dsymes, ssymes, symes;
	Length		i, argc;
	AbSyn		*argv;
	TForm		tf;

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	default:
		argc = 1;
		argv = &sefo;
		break;
	}

	xsymes = isymes = dsymes = ssymes = listNil(Syme);
	for (i = 0; i < argc; i += 1) {
		AbSyn	id = abDefineeIdOrElse(argv[i], NULL);

		/* Empty body. */
		if (abTag(argv[i]) == AB_Nothing)
			;

		/* Default body. */
		else if (abTag(argv[i]) == AB_Default) {
			tisef(stab, argv[i]);
			symes = abGetCatExports(argv[i]);
			dsymes = listConcat(Syme)(symes, dsymes);
		}

		/* Explicit declaration/definition. */
		else if (id && abTag(argv[i]) != AB_Id) {
			assert(abSyme(id));
			if (symeIsExport(abSyme(id)))
				xsymes = listCons(Syme)(abSyme(id), xsymes);
		}

		/* Category expression. */
		else {
			tisef(stab, argv[i]);
			symes = abGetCatExports(argv[i]);
			isymes = symeListUnion(isymes, symes, symeEqual);
			symes = abGetCatSelf(argv[i]);
			ssymes = symeListUnion(ssymes, symes, symeEqual);
		}
	}

	/* Mark the symes which have a default implementation. */
	dsymes = listNReverse(Syme)(dsymes);
	for (; dsymes; dsymes = listFreeCons(Syme)(dsymes)) {
		Syme	dsyme = car(dsymes);
		Syme	xsyme = NULL;

		/* If the default is one of our exports, just mark it. */
		for (symes = xsymes; !xsyme && symes; symes = cdr(symes))
			if (symeEqual(car(symes), dsyme)) {
				xsyme = car(symes);
				symeSetDefault(xsyme);
			}

		/* If the default is inherited, use the default syme. */
		if (xsyme == NULL) {
			xsymes = listCons(Syme)(dsyme, xsymes);
			symeSetDefault(dsyme);
		}
	}

	xsymes = listNReverse(Syme)(xsymes);
	xsymes = symeListUnion(isymes, xsymes, symeEqual);

	if (abTag(sefo) == AB_Sequence)
		abTUnique(sefo) = tfThird(xsymes);

	tf = tfThird(xsymes);
	tfAddSelf(tf, ssymes);
	tfHasSelf(tf) = true;

	return tf;
}

/****************************************************************************
 *
 * :: Implied call:  apply, set!, test, generator, ...
 *
 ***************************************************************************/

/*
 * ab ==> m(i,...)	 ->  tibup0Apply(stab, ab, 'apply,    n+1, [m,i,...])
 * ab ==> m(i,...) := x	 ->  tibup0Apply(stab, ab, 'set!,     n+2, [m,i,...,x])
 * ab ==> if bb then ... ->  tibup0Apply(stab, bb, 'test,     1,   [bb.cond]);
 * ab ==> for i in l	 ->  tibup0Apply(stab, ab, 'iterator, 1,   [l])
 * ab ==> x::T		 ->  tibup0Apply(stab, ab, 'coerce,   1,   [x])
 *
 * Could also do...
 *
 * ab ==> 3		 ->  tibup0Apply(stab, ab, 'integer,  0,   [])
 * ab ==> "3.14"	 ->  tibup0Apply(stab, ab, 'float,    0,   [])
 * ab ==> "hello"	 ->  tibup0Apply(stab, ab, 'string,   0,   [])
 */

local void
tisef0ApplySymIfNeeded(Stab stab, Sefo sefo, Length argc, AbSynGetter argf,
		       Sefo implicitPart, TFormPredicate pred)
{
	Sefo	part;
	TForm	tf;

	assert(argc == 1);

	part = argf(sefo, int0);
	tf   = abTUnique(part);

	if (pred(tf))
		abTUnique(sefo) = abTUnique(part);
	else
		tisef0ApplySym(stab, sefo, argc, argf, implicitPart);
}

local void
tisef0ApplySym(Stab stab, Sefo sefo, Length argc, AbSynGetter argf,
	       Sefo implicitPart)
{
	Sefo	imp;
	if (!implicitPart) implicitPart = sefo;

	imp = abImplicit(implicitPart);

	if (imp) {
		tisef(stab, imp);
		tisef0ApplyFType(stab, sefo, imp, argc, argf);
	}
	else
		abTUnique(sefo) = tfUnknown;
}

local void
tisef0ApplyJoin(Stab stab, Sefo sefo, Sefo op, Length argc, AbSynGetter argf)
{
	SymeList	symes = listNil(Syme), mods = listNil(Syme);
	TForm		selftf;
	Length		i;

	/* Collect the category exports for the result. */
	for (i = 0; i < argc; i += 1) {
		TForm		tfi    = abTUnique(argf(sefo, i));
		SymeList	symesi = tfGetThdExports(tfi);
		symes = tfJoinExportLists(mods, symes, symesi, NULL);
	}
	selftf = tfThird(symes);

	/* Collect the symbol meaning(s) for self for the result. */
	for (i = 0; i < argc; i += 1)
		tfAddSelf(selftf, tfGetThdSelf(abTUnique(argf(sefo, i))));

	abTUnique(sefo) = selftf;
}

local void
tisef0ApplyFType(Stab stab, Sefo sefo, Sefo op, Length argc, AbSynGetter argf)
{
	SatMask		mask = tfSatSefMask();
	TForm		opType = abTUnique(op), retType;

	assert(tfIsAnyMap(opType));
	retType = tfMapRet(opType);

	if (abIsTheId(op, ssymJoin) && tfSatisfies(retType, tfCategory)) {
		tisef0ApplyJoin(stab, sefo, op, argc, argf);
		return;
	}

	tiTfSefo(stab, opType);
	tfSatMap(mask, stab, opType, tfUnknown, sefo, argc, argf);
}

/****************************************************************************
 *
 * :: Far Values:  return x, yield x, a => x
 *
 ***************************************************************************/

local void
tisef0FarValue(Stab stab, Sefo farValue, TForm *pFarType)
{
	if (tfIsUnknown(*pFarType))
		*pFarType = abTUnique(farValue);
}


/****************************************************************************
 *
 * :: Id:  x, +, 1
 * X
 ***************************************************************************/

local void
tisefId(Stab stab, Sefo sefo)
{
	Syme	syme = abSyme(sefo);

	if (syme)
		abTUnique(sefo) = symeType(syme);
	else
		abTUnique(sefo) = tfUnknown;
}

/****************************************************************************
 *
 * :: LitInteger:   32
 * :: LitFloat:	    4.0
 * :: LitString:    "hello"
 * X
 * !! This stuff could be made to go through ti...0Apply.
 ***************************************************************************/

local void	tisef0Literal (Stab, Sefo);

/*
 * Sefo entry points.
 */

local void
tisefLitInteger(Stab stab, Sefo sefo)
{
	tisef0Literal(stab, sefo);
}

local void
tisefLitFloat(Stab stab, Sefo sefo)
{
	tisef0Literal(stab, sefo);
}

local void
tisefLitString(Stab stab, Sefo sefo)
{
	tisef0Literal(stab, sefo);
}

/*
 * Functions which actually do the work.
 */

local void
tisef0Literal(Stab stab, Sefo sefo)
{
	Syme	syme = abSyme(sefo);

	if (syme)
		abTUnique(sefo) = tfMapRet(symeType(syme));
	else
		abTUnique(sefo) = tfUnknown;
}


/***************************************************************************
 *
 * :: Comma: (a, b)
 *
 ***************************************************************************/

local void
tisefComma(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	abTUnique(sefo) = tfMultiFrTUnique(sefo);
}

/****************************************************************************
 *
 * :: Apply:   f(a, b, ...)
 *
 ***************************************************************************/

local void
tisef0ApplySpecialSyme(Stab stab, Sefo sefo)
{
	Sefo		op = abApplyOp(sefo);
	Syme		syme;

	if (abTag(op) == AB_Id && abSyme(op) == NULL &&
	    (syme = tfpOpSyme(stab, op->abId.sym, abApplyArgc(sefo))) != 0)
		abSetSyme(op, syme);

	return;
}

local void
tisefApply(Stab stab, Sefo sefo)
{
	Sefo		op;
	TForm		tf;

	tipApplyDEBUG{fprintf(dbOut, "Entering tisefApply\n");}

	tisef0ApplySpecialSyme(stab, sefo);

	tisef0Generic(stab, sefo);
	op = abApplyOp(sefo);

	tfFollow(abTUnique(op));
	tf = abTUnique(op);

	if (tfIsAnyMap(tf))
		tisef0ApplyFType(stab,sefo,op,abApplyArgc(sefo),abApplyArgf);
	else
		tisef0ApplySym(stab, sefo, abArgc(sefo), abArgf, NULL);
}

/****************************************************************************
 *
 * :: Define:	a == e
 * X
 ***************************************************************************/

local void
tisefDefine(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	if (abTag(sefo->abDefine.lhs) == AB_Declare)
		abTUnique(sefo) =
			abTUnique(sefo->abDefine.lhs->abDeclare.type);
	else
		abTUnique(sefo) = abTUnique(sefo->abDefine.rhs);
}


/****************************************************************************
 *
 * :: Assign:	a := e
 * X
 ***************************************************************************/

/*!! To do: (v.i,v.j) := (v.j, v.i)	 */

local void
tisefAssign(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	abTUnique(sefo) = abTUnique(sefo->abAssign.rhs);
}


/****************************************************************************
 *
 * :: Declare:	 a: A
 *
 ***************************************************************************/

local void
tisefDeclare(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);

	if (abUse(sefo) == AB_Use_Define || abUse(sefo) == AB_Use_Assign)
		abTUnique(sefo) = abTUnique(sefo->abDeclare.id);
	else
		abTUnique(sefo) = abTUnique(sefo->abDeclare.type);
}

/****************************************************************************
 *
 * :: Label:   @@ x @@ [e]
 *
 ***************************************************************************/

local void
tisefLabel(Stab stab, Sefo sefo)
{
	Sefo expr = sefo->abLabel.expr;
	tisef0Generic(stab, expr);
	abTUnique(sefo) = abTUnique(sefo->abLabel.expr);
}

/****************************************************************************
 *
 * :: Goto:   goto id
 *
 ***************************************************************************/

local void
tisefGoto(Stab stab, Sefo sefo)
{
	abTUnique(sefo) = tfExit;
}

/****************************************************************************
 *
 * :: Lambda:	(a: A): B +-> b
 * :: PLambda:	(a: A): B +->* b
 *
 ***************************************************************************/

local void
tisefLambda(Stab stab, Sefo sefo)
{
	Scope("tisefLambda");
	TForm	tfarg, tfret;
	Bool	pack = abHasTag(sefo, AB_PLambda);

	TForm	fluid(tuniReturnTForm);
	TForm	fluid(tuniYieldTForm);
	TForm	fluid(tuniExitTForm);

	tuniReturnTForm = tfNone();
	tuniYieldTForm	= tfNone();
	tuniExitTForm	= tfNone();

	tisef0Generic(stab, sefo);
	tfarg = tiGetTForm(stab, sefo->abLambda.param);
	tfret = tiGetTForm(stab, sefo->abLambda.rtype);

	if (tfarg && tfret)
		abTUnique(sefo) = tfAnyMap(tfarg, tfret, pack);
	else
		abTUnique(sefo) = tfUnknown;

	ReturnNothing;
}

/****************************************************************************
 *
 * :: Sequence:	  (a; b; c)
 *
 ***************************************************************************/

local void
tisefSequence(Stab stab, Sefo sefo)
{
	Scope("tisefSequence");
	Length	n = abArgc(sefo);

	TForm	fluid(tuniExitTForm);
	tuniExitTForm = tfUnknown;

	tisef0Generic(stab, sefo);
	if (n > 0) {
		tisef0FarValue(stab, abArgv(sefo)[n-1], &tuniExitTForm);
		abTUnique(sefo) = tuniExitTForm;
	}

	ReturnNothing;
}

/****************************************************************************
 *
 * :: Exit:  (...; b => x ; ...)
 *
 ***************************************************************************/

local void
tisefExit(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	tisef0FarValue(stab, sefo->abExit.value, &tuniExitTForm);
}

/***************************************************************************
 *
 * :: return: return x,	 return;
 *
 ***************************************************************************/

local void
tisefReturn(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	/* tuniReturnTForm is not used by tisefLambda. */
	abTUnique(sefo) = tfExit;
}

/****************************************************************************
 *
 * :: Generate:	 generate (... yield ...)
 *
 ***************************************************************************/

local void
tisefGenerate(Stab stab, Sefo sefo)
{
	Scope("tisefGenerate");

	TForm	fluid(tuniReturnTForm);
	TForm	fluid(tuniYieldTForm);
	TForm	fluid(tuniExitTForm);

	tuniReturnTForm = tfNone();
	tuniYieldTForm	= tfUnknown;
	tuniExitTForm	= tfNone();

	tisef0Generic(stab, sefo);
	abTUnique(sefo) = tfGenerator(tuniYieldTForm);

	ReturnNothing;
}

/****************************************************************************
 *
 * :: Reference: ref (id or id of array/record/union)
 *
 ***************************************************************************/

local void
tisefReference(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	abTUnique(sefo) = tfReference(abTUnique(sefo->abReference.body));
}

/***************************************************************************
 *
 * :: Yield:  yield x
 *
 ***************************************************************************/

local void
tisefYield(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	tisef0FarValue(stab, sefo->abYield.value, &tuniYieldTForm);
	abTUnique(sefo) = tfExit;
}

/****************************************************************************
 *
 * :: Add:  [D] add (a: A == ...)
 *
 ***************************************************************************/

local void
tisefAdd(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);

	/* !! Should infer type of body. */
	abTUnique(sefo) = tfType;
}

/****************************************************************************
 *
 * :: With:  [C] with (a: A; ...)
 *
 ***************************************************************************/

local void
tisefWith(Stab stab, Sefo sefo)
{
	SymeList	bsymes, wsymes, symes = listNil(Syme);
	SymeList	mods = listNil(Syme);
	TForm		wtf, tf;

	tisef(stab, sefo->abWith.base);
	bsymes = abGetCatExports(sefo->abWith.base);

	tisef(stab, sefo->abWith.within);
	wtf = tisef0Within(stab, sefo->abWith.within, bsymes);
	wsymes = tfGetThdExports(wtf);

	symes = tfJoinExportLists(mods, symes, bsymes, NULL);
	symes = tfJoinExportLists(mods, symes, wsymes, NULL);

	tf = tfThird(symes);
	tfAddSelf(tf, abGetCatSelf(sefo->abWith.base));
	tfAddSelf(tf, tfGetThdSelf(wtf));

	abTUnique(sefo) = tf;
}

/****************************************************************************
 *
 * :: Where:  e where d
 *
 ***************************************************************************/

local void
tisefWhere(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	abTUnique(sefo) = abTUnique(sefo->abWhere.expr);
}

/****************************************************************************
 *
 * :: If:  if b then t [else e]
 *
 ***************************************************************************/

local void
tisefIf(Stab stab, Sefo sefo)
{
	Sefo	test	= sefo->abIf.test;
	Sefo	thenAlt = sefo->abIf.thenAlt;
	Sefo	elseAlt = sefo->abIf.elseAlt;

	tisef0Generic(stab, sefo);

	if (true) {
		SymeList	tsymes, esymes, symes = listNil(Syme);
		SymeList	mods = listNil(Syme);
		TForm		ttf, etf, tf;

		ttf = tisef0Within(stab, thenAlt, listNil(Syme));

		tsymes = tfGetThdExports(ttf);

		etf = tisef0Within(stab, elseAlt, listNil(Syme));

		esymes = tfGetThdExports(etf);

		symes = tfJoinExportLists(mods, symes, tsymes, test);
		test = abNewNot(sposNone, test);
		symes = tfJoinExportLists(mods, symes, esymes, test);

		tf = tfThird(symes);
		tfAddSelf(tf, tfGetThdSelf(ttf));
		tfAddSelf(tf, tfGetThdSelf(etf));

		abTUnique(sefo) = tf;

		return;
	}

	/*!! This looks entirely suspect. */
	if (abIsNotNothing(elseAlt) &&
	    tfSatisfies(abTUnique(thenAlt), abTUnique(elseAlt)))
		abTUnique(sefo) = abTUnique(elseAlt);
	else
		abTUnique(sefo) = abTUnique(thenAlt);
}

/****************************************************************************
 *
 * :: Test:  implied test
 *
 ***************************************************************************/

local void
tisefTest(Stab stab, Sefo sefo)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	tisef0Generic(stab, sefo);
	assert(tfBoolean != tfUnknown);
	abTUnique(sefo) = tfBoolean;
}

/***************************************************************************
 *
 * :: Collect:	e <iter>*
 *
 ***************************************************************************/

local void
tisefCollect(Stab stab, Sefo sefo)
{
	Scope("tisefCollect");

	TForm	fluid(tuniReturnTForm);
	TForm	fluid(tuniYieldTForm);
	TForm	fluid(tuniExitTForm);

	tuniReturnTForm = tfNone();
	tuniYieldTForm	= tfNone();
	tuniExitTForm	= tfNone();

	tisef0Generic(stab, sefo);
	abTUnique(sefo) = tfGenerator(abTUnique(sefo->abCollect.body));

	ReturnNothing;
}

/****************************************************************************
 *
 * :: Repeat:  <iter>* repeat e
 *
 ***************************************************************************/

local void
tisefRepeat(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Never
 * X
 ***************************************************************************/

local void
tisefNever(Stab stab, Sefo sefo)
{
	abTUnique(sefo) = tfExit;
}

/***************************************************************************
 *
 * :: Iterate
 * X
 ***************************************************************************/

local void
tisefIterate(Stab stab, Sefo sefo)
{
	abTUnique(sefo) = tfExit;
}

/***************************************************************************
 *
 * :: Break
 * X
 ***************************************************************************/

local void
tisefBreak(Stab stab, Sefo sefo)
{
	abTUnique(sefo) = tfExit;
}

/****************************************************************************
 *
 * :: While:  while c
 *
 ***************************************************************************/

local void
tisefWhile(Stab stab, Sefo sefo)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	tisef0Generic(stab, sefo);
	assert(tfBoolean != tfUnknown);
	abTUnique(sefo) = tfBoolean;
}


/***************************************************************************
 *
 * :: For:  for x in l | c
 *
 ***************************************************************************/

local void
tisefFor(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
	tisef0ApplySymIfNeeded(stab,sefo,1,abForIterArgf,NULL,tfIsGeneratorFn);
}

/****************************************************************************
 *
 * :: Foreign:	import ... from Foreign(...)
 *
 ***************************************************************************/

local void
tisefForeign(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/****************************************************************************
 *
 * :: Import:  import ... from D
 *
 ***************************************************************************/

local void
tisefImport(Stab stab, Sefo sefo)
{
	AbSyn	what = sefo->abImport.what;
	AbSyn	from = sefo->abImport.origin;

	tisef(stab, from);
	tisef(stab, what);
	tisef0Within(stab, what, listNil(Syme));

	abTUnique(sefo) = tfNone();
}

/****************************************************************************
 *
 * :: Inline:  inline .. from D
 *
 ***************************************************************************/

local void
tisefInline(Stab stab, Sefo sefo)
{
	AbSyn	what = sefo->abInline.what;
	AbSyn	from = sefo->abInline.origin;

	tisef(stab, from);
	tisef(stab, what);
	tisef0Within(stab, what, listNil(Syme));

	abTUnique(sefo) = tfNone();
}

/****************************************************************************
 *
 * :: Qualify:	 A $ B
 *
 * ToDo: 3$Integer
 *
 ***************************************************************************/

local void
tisefQualify(Stab stab, Sefo sefo)
{
	AbSyn	what = sefo->abQualify.what;
	Syme	syme = abSyme(what);

	tisef0Generic(stab, sefo);

	if (syme)
		abTUnique(sefo) = symeType(syme);
	else
		abTUnique(sefo) = tfUnknown;
}

/***************************************************************************
 *
 * :: CoerceTo:	 x :: T
 *
 ***************************************************************************/

local void
tisefCoerceTo(Stab stab, Sefo sefo)
{
	TForm	tf;

	tisef0Generic(stab, sefo);
	tf = tiGetTForm(stab, sefo->abCoerceTo.type);

	abTUnique(sefo) = (tf ? tf : tfUnknown);
}

/****************************************************************************
 *
 * :: RestrictTo:   A @ B
 *
 ***************************************************************************/

local void
tisefRestrictTo(Stab stab, Sefo sefo)
{
	TForm	tf;

	tisef0Generic(stab, sefo);
	tf = tiGetTForm(stab, sefo->abRestrictTo.type);

	abTUnique(sefo) = (tf ? tf : tfUnknown);
}


/****************************************************************************
 *
 * :: PretendTo:   A pretend B
 *
 ***************************************************************************/

local void
tisefPretendTo(Stab stab, Sefo sefo)
{
	TForm	tf;

	tisef0Generic(stab, sefo);
	tf = tiGetTForm(stab, sefo->abPretendTo.type);

	abTUnique(sefo) = (tf ? tf : tfUnknown);
}

/***************************************************************************
 *
 * :: Not: not a
 *
 ***************************************************************************/

local void
tisefNot(Stab stab, Sefo sefo)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	tisef0Generic(stab, sefo);
	assert(tfBoolean != tfUnknown);
	abTUnique(sefo) = tfBoolean;
}

/***************************************************************************
 *
 * :: And: a and b and c ...
 *
 ***************************************************************************/

local void
tisefAnd(Stab stab, Sefo sefo)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	tisef0Generic(stab, sefo);
	assert(tfBoolean != tfUnknown);
	abTUnique(sefo) = tfBoolean;
}

/***************************************************************************
 *
 * :: Or: a or b or c ...
 *
 ***************************************************************************/

local void
tisefOr(Stab stab, Sefo sefo)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	tisef0Generic(stab, sefo);
	assert(tfBoolean != tfUnknown);
	abTUnique(sefo) = tfBoolean;
}

/***************************************************************************
 *
 * :: Assert:
 *
 ***************************************************************************/

local void
tisefAssert(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);	
	abTUnique(sefo) = tfNone();
}

/***************************************************************************
 *
 * :: Blank:
 *
 ***************************************************************************/

local void
tisefBlank(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Builtin:
 *
 ***************************************************************************/

local void
tisefBuiltin(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Default:
 *
 ***************************************************************************/

local void
tisefDefault(Stab stab, Sefo sefo)
{
	AbSyn		body;
	TForm		tf;

	body = sefo->abDefault.body;
	tisef(stab, body);
	tf = tisef0Within(stab, body, listNil(Syme));

	abTUnique(sefo) = tf;
}

/***************************************************************************
 *
 * :: Delay:
 *
 ***************************************************************************/

local void
tisefDelay(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Do:
 *
 ***************************************************************************/

local void
tisefDo(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Except:
 *
 ***************************************************************************/

local void
tisefExcept(Stab stab, Sefo sefo)
{
	tisef(stab, sefo->abExcept.except);
	tisef(stab, sefo->abExcept.type);

	abTUnique(sefo) = abTUnique(sefo->abExcept.type);

}

/***************************************************************************
 *
 * :: Raise:
 *
 ***************************************************************************/

local void
tisefRaise(Stab stab, Sefo sefo)
{
	tisef(stab, sefo->abRaise.expr);

	abTUnique(sefo) = tfExcept(tfExit, tiGetTForm(stab, sefo->abRaise.expr));
}

/***************************************************************************
 *
 * :: Export:
 *
 ***************************************************************************/

local void
tisefExport(Stab stab, Sefo sefo)
{
	AbSyn	what = sefo->abExport.what;
	AbSyn	from = sefo->abExport.origin;
	AbSyn	dest = sefo->abExport.destination;

	tisef(stab, from);
	tisef(stab, dest);

	tisef(stab, what);
	if (!abIsNothing(from) || !abIsNothing(dest))
		tisef0Within(stab, what, listNil(Syme));

	abTUnique(sefo) = tfNone();
}

/***************************************************************************
 *
 * :: Extend:
 *
 ***************************************************************************/

local void
tisefExtend(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Fix:
 *
 ***************************************************************************/

local void
tisefFix(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Fluid:
 *
 ***************************************************************************/

local void
tisefFluid(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Free:
 *
 ***************************************************************************/

local void
tisefFree(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Has:
 *
 ***************************************************************************/

local void
tisefHas(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Hide:
 *
 ***************************************************************************/

local void
tisefHide(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: IdSy:
 *
 ***************************************************************************/

local void
tisefIdSy(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Let:
 *
 ***************************************************************************/

local void
tisefLet(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Local:
 *
 ***************************************************************************/

local void
tisefLocal(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}


/***************************************************************************
 *
 * :: Macro:
 *
 ***************************************************************************/

local void
tisefMacro(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}


/***************************************************************************
 *
 * :: MLambda:
 *
 ***************************************************************************/

local void
tisefMLambda(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}

/***************************************************************************
 *
 * :: Nothing:
 *
 ***************************************************************************/

local void
tisefNothing(Stab stab, Sefo sefo)
{
	abTUnique(sefo) = tfNone();
}

/***************************************************************************
 *
 * :: Quote:
 *
 ***************************************************************************/

local void
tisefQuote(Stab stab, Sefo sefo)
{
	tisef0Generic(stab, sefo);
}


/***************************************************************************
 *
 * :: Select:
 *
 ***************************************************************************/

local void
tisefSelect(Stab stab, Sefo sefo)
{
	/* Not 100% sure about this...*/
	tisef0Generic(stab, sefo);

	return;				    
}


/***************************************************************************
 *
 * :: Try:
 *
 ***************************************************************************/

local void
tisefTry(Stab stab, Sefo sefo)
{
	tisef(stab, sefo->abTry.id);
	tisef(stab, sefo->abTry.expr);
	tisef(stab, sefo->abTry.always);
	tisef(stab, sefo->abTry.except);

	abTUnique(sefo) = abTUnique(sefo->abTry.expr);
}
