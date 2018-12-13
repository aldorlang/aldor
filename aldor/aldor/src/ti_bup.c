/****************************************************************************
 *
 * ti_bup.c: Type inference -- bottom up pass.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ***************************************************************************/

#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "ti_bup.h"
#include "ti_tdn.h"
#include "tinfer.h"
#include "terror.h"
#include "util.h"
#include "sefo.h"
#include "lib.h"
#include "tposs.h"
#include "tfsat.h"
#include "absub.h"
#include "ablogic.h"
#include "abpretty.h"
#include "comsg.h"
#include "strops.h"
#include "table.h"


/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

Bool	tipBupDebug	= false;
#define tipBupDEBUG	DEBUG_IF(tipBup)	afprintf

/*****************************************************************************
 *
 * :: Fluids to unify multiple exit points
 *
 ****************************************************************************/

/*
 * One of the following four conditions will hold:
 *
 *   tuniIsInappropriate  => not in an appropriate context.
 *   tuniIsNoValue	  => in no-value context.
 *   tuniIsUnknown	  => in appropriate context, but no exit seen yet.
 *   otherwise		  => seen some possible types -- must intersect.
 */


static TPoss	tuniYieldTPoss		= 0;
static TPoss	tuniReturnTPoss		= 0;
static TPoss	tuniExitTPoss		= 0;
static TPoss	tuniSelectTPoss		= 0;
static TForm	tuniYieldType		= 0;
static TForm	tuniExitType		= 0;
static AbSyn	tuniBupSelectObj	= 0;

/*****************************************************************************
 *
 * :: Other fluids and globals
 *
 ****************************************************************************/

static Bool	  tloopBreakCount    = -1;/* Handle loop exits */
static SymbolList terrorIdComplaints = 0; /* Id complaint list for scope. */

/*****************************************************************************
 *
 * :: Declarations for bottom up pass
 *
 ****************************************************************************/

local Bool	tibup0GiveMsg	(AbSyn);
local void	tibup0Generic	(Stab, AbSyn, TForm);
local TForm	tibup0Within	(Stab, AbSyn, SymeList, Bool);
local SymeList	tibup0DefaultBody(Stab stab, AbSyn absyn, Bool);
local void	tibup0FarValue	(Stab, AbSyn, TForm, AbSyn, TPoss *);
local void	tibup0NoValue	(Stab, AbSyn, TForm, Msg);

local void	tibup0ApplySymIfNeeded
				(Stab, AbSyn, TForm, Symbol,
				 Length, AbSynGetter, AbSyn, TFormPredicate);
local void	tibup0ApplySym	(Stab, AbSyn, TForm, Symbol,
				 Length, AbSynGetter, AbSyn);
local void	tibup0ApplyFType(Stab, AbSyn, TForm, AbSyn,
				 Length, AbSynGetter);
local void	tibup0ApplyJoin (Stab, AbSyn, TForm, AbSyn,
				 Length, AbSynGetter);

local void	tibupSequence0  (Stab, AbSyn, TForm);

local void	tibup0InferLhs		(Stab, AbSyn, AbSyn, AbSyn, TPoss);
local void	tibup0InferLhsId	(Stab, AbSyn, AbSyn, TForm, AbSyn);
local void	tibup0InferLhsApply	(Stab, AbSyn, AbSyn, TForm);

local void	tibupId		(Stab, AbSyn, TForm);
local void	tibupIdSy	(Stab, AbSyn, TForm);
local void	tibupBlank	(Stab, AbSyn, TForm);
local void	tibupLitInteger (Stab, AbSyn, TForm);
local void	tibupLitFloat	(Stab, AbSyn, TForm);
local void	tibupLitString	(Stab, AbSyn, TForm);
local void	tibupAdd	(Stab, AbSyn, TForm);
local void	tibupAnd	(Stab, AbSyn, TForm);
local void	tibupApply	(Stab, AbSyn, TForm);
local void	tibupAssert	(Stab, AbSyn, TForm);
local void	tibupAssign	(Stab, AbSyn, TForm);
local void	tibupBreak	(Stab, AbSyn, TForm);
local void	tibupBuiltin	(Stab, AbSyn, TForm);
local void	tibupCoerceTo	(Stab, AbSyn, TForm);
local void	tibupCollect	(Stab, AbSyn, TForm);
local void	tibupComma	(Stab, AbSyn, TForm);
local void	tibupDeclare	(Stab, AbSyn, TForm);
local void	tibupDefault	(Stab, AbSyn, TForm);
local void	tibupDefine	(Stab, AbSyn, TForm);
local void	tibupDelay	(Stab, AbSyn, TForm);
local void	tibupDo		(Stab, AbSyn, TForm);
local void	tibupExcept	(Stab, AbSyn, TForm);
local void	tibupRaise	(Stab, AbSyn, TForm);
local void	tibupExit	(Stab, AbSyn, TForm);
local void	tibupExport	(Stab, AbSyn, TForm);
local void	tibupExtend	(Stab, AbSyn, TForm);
local void	tibupFix	(Stab, AbSyn, TForm);
local void	tibupFluid	(Stab, AbSyn, TForm);
local void	tibupFor	(Stab, AbSyn, TForm);
local void	tibupForeignImport(Stab, AbSyn, TForm);
local void	tibupForeignExport(Stab, AbSyn, TForm);
local void	tibupFree	(Stab, AbSyn, TForm);
local void	tibupGenerate	(Stab, AbSyn, TForm);
local void	tibupReference	(Stab, AbSyn, TForm);
local void	tibupRefArg	(Stab, AbSyn, TForm);
local void	tibupGoto	(Stab, AbSyn, TForm);
local void	tibupHas	(Stab, AbSyn, TForm);
local void	tibupHide	(Stab, AbSyn, TForm);
local void	tibupIf		(Stab, AbSyn, TForm);
local void	tibupImport	(Stab, AbSyn, TForm);
local void	tibupInline	(Stab, AbSyn, TForm);
local void	tibupIterate	(Stab, AbSyn, TForm);
local void	tibupLabel	(Stab, AbSyn, TForm);
local void	tibupLambda	(Stab, AbSyn, TForm);
local void	tibupLet	(Stab, AbSyn, TForm);
local void	tibupLocal	(Stab, AbSyn, TForm);
local void	tibupMacro	(Stab, AbSyn, TForm);
local void	tibupMLambda    (Stab, AbSyn, TForm);
local void	tibupNever	(Stab, AbSyn, TForm);
local void	tibupNot	(Stab, AbSyn, TForm);
local void	tibupNothing	(Stab, AbSyn, TForm);
local void	tibupOr		(Stab, AbSyn, TForm);
local void	tibupPretendTo	(Stab, AbSyn, TForm);
local void	tibupQualify	(Stab, AbSyn, TForm);
local void	tibupQuote	(Stab, AbSyn, TForm);
local void	tibupRepeat	(Stab, AbSyn, TForm);
local void	tibupRestrictTo (Stab, AbSyn, TForm);
local void	tibupReturn	(Stab, AbSyn, TForm);
local void	tibupSelect	(Stab, AbSyn, TForm);
local void	tibupSequence	(Stab, AbSyn, TForm);
local void	tibupTest	(Stab, AbSyn, TForm);
local void	tibupTry	(Stab, AbSyn, TForm);
local void	tibupWhere	(Stab, AbSyn, TForm);
local void	tibupWhile	(Stab, AbSyn, TForm);
local void	tibupWith	(Stab, AbSyn, TForm);
local void	tibupYield	(Stab, AbSyn, TForm);

local void tibup0RefImps(Stab, AbSyn, TForm);

/*****************************************************************************
 *
 * :: Bottom up pass
 *
 ****************************************************************************/

void
tiBottomUp(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tiBottomUp");

	TPoss	   fluid(tuniReturnTPoss);
	TPoss	   fluid(tuniYieldTPoss);
	TForm	   fluid(tuniYieldType);
	TPoss	   fluid(tuniExitTPoss);
	Bool	   fluid(tloopBreakCount);
	SymbolList fluid(terrorIdComplaints);
	AbLogic    fluid(abCondKnown);

	tuniYieldTPoss	   = tuniInappropriateTPoss;
	tuniYieldType	   = tfUnknown;
	tuniReturnTPoss	   = tuniInappropriateTPoss;
	tuniExitTPoss	   = tuniInappropriateTPoss;
	tuniExitType       = tfUnknown;
	tloopBreakCount	   = -1;
	terrorIdComplaints = 0;
	abCondKnown        = abCondKnown ? ablogCopy(abCondKnown) : ablogTrue();

	tibup(stab, absyn, type);

	listFree(Symbol)(terrorIdComplaints);
	ablogFree(abCondKnown);
	
	ReturnNothing;
}


void
tibup(Stab stab, AbSyn absyn, TForm type)
{
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;

	assert(absyn);

	/* Check before processing the stab, if present. */
	if (abState(absyn) >= AB_State_HasPoss)
		return;

	if (!abIsLeaf(absyn) && abStab(absyn)) {
		stab = abStab(absyn);
		stabSeeOuterImports(stab);
		stabGetSubstable(stab);
		typeInferTForms(stab);
	}

	/* Check that stab processing didn't already process absyn. */
	if (abState(absyn) >= AB_State_HasPoss)
		return;

	serialNo += 1;
	depthNo	 += 1;
	serialThis = serialNo;
	if (DEBUG(tipBup)) {
		fprintf(dbOut,"->Bup: %*s %d= ", depthNo, "", serialThis);
		abPrettyPrint(dbOut, absyn);
		fnewline(dbOut);
	}
	
	AB_SWITCH(absyn, tibup, (stab, absyn, type));

	if (abState(absyn) == AB_State_AbSyn)
		abState(absyn) = AB_State_HasPoss;

#if 0
	/*
	 * !! Not yet, first we need to clean up the type arguments passed
	 * into tibup and change the type satisfier so _ can be a category.
	 * Some (all?) other calls to the type satisfier in this file can
	 * then be removed.
	 */
	{
		TPoss	abtposs;
		abtposs = tpossSatisfiesType(abTPoss(absyn), type);
		tpossFree(abTPoss(absyn));
		abTPoss(absyn) = abtposs;
	}
#endif

	/*
	 * If all the parts of this node had meaning but this node
	 * itself has not meaning, then set the state to error. This
	 * is because it doesn't matter what types we assign to the
	 * leaves, this node will always be unsatified.
	 */
	if (tibup0GiveMsg(absyn)) {
		tpossFree(abTPoss(absyn));
		abState(absyn) = AB_State_Error;
		abTPoss(absyn) = tpossEmpty();
	}

	if (DEBUG(tipBup)) {
		TPoss	abtposs = abReferTPoss(absyn);
		fprintf(dbOut, "<-Bup: %*s %d= ", depthNo, "", serialThis);
		abPrettyPrint(dbOut, absyn);
		fprintf(dbOut, " has %d meanings: ", tpossCount(abtposs));
		tpossPrint(dbOut, abtposs);
		fnewline(dbOut);
		tpossFree(abtposs);
	}
	depthNo -= 1;
}

local Bool
tibup0GiveMsg(AbSyn absyn)
{
	Bool	result;

	/* Does this node have a possible set of types? */
	result = abState(absyn) == AB_State_HasPoss &&
		tpossCount(abTPoss(absyn)) == 0;


	/* If there are possible types then return now. */
	if (!result) return result;


	/* Leaf nodes can be ignored for now */
	if (abIsLeaf(absyn))
		result = false;

	/* So can add-bodies */
	else if (abTag(absyn) == AB_Add)
		result = false;

	else {
		/* Check all parts have meaning */
		Length	i;
		for (i = 0; result && i < abArgc(absyn); i += 1) {
			AbSyn	argi = abArgv(absyn)[i];
			switch (abState(argi)) {
			case AB_State_HasPoss:
				result = tpossCount(abTPoss(argi)) > 0;
				break;
			case AB_State_Error:
				result = false;
				break;
			default:
				break;
			}
		}
	}

	/* Returns true iff all parts had meaning (and we don't) */
	return result;
}

/****************************************************************************
 *
 * :: Generic:	abArgc(ab), abArgv(ab)
 *
 ***************************************************************************/

local void
tibup0Generic(Stab stab, AbSyn absyn, TForm type)
{
	Length	i;
	if (!abIsLeaf(absyn))
		for (i = 0; i < abArgc(absyn); i += 1)
			tibup(stab, abArgv(absyn)[i], tfUnknown);

	if (tfIsUnknown(type)) type = tfNone();
	abTPoss(absyn) = tpossSingleton(type);
}

/****************************************************************************
 *
 * :: Within:	with bodies, 'with' conditional branches
 *
 ***************************************************************************/

local TForm
tibup0Within(Stab stab, AbSyn absyn, SymeList bsymes, Bool doDefault)
{
	SymeList	xsymes, isymes, dsymes, ssymes, symes;
	Length		i, argc;
	AbSyn		*argv;
	TForm		tf;
	Bool		pending = false;

	AB_SEQ_ITER(absyn, argc, argv);

	xsymes = isymes = dsymes = ssymes = listNil(Syme);
	for (i = 0; i < argc; i += 1) {
		AbSyn	id = abDefineeIdOrElse(argv[i], NULL);

		/* Empty body. */
		if (abTag(argv[i]) == AB_Nothing)
			typeInferAs(stab, argv[i], tfUnknown);
		/* Default body. */
		else if (abTag(argv[i]) == AB_Default) {
			typeInferAs(stab, argv[i], tfUnknown);
			symes = abGetCatExports(argv[i]);
			dsymes = listConcat(Syme)(symes, dsymes);
		}
		/* Explicit declaration/definition. */
		else if (id && abTag(argv[i]) != AB_Id) {
			if (abTag(argv[i]) == AB_Define && !doDefault) {
				AbSyn	lhs = argv[i]->abDefine.lhs;
				tiTfPushDefinee(lhs);
				typeInferAs(stab, lhs,
					    tiDefineFilter(argv[i],tfUnknown));
				tiTfPopDefinee(lhs);
			}
			else /*if (doDefault)*/
				typeInferAs(stab, argv[i], tfUnknown);
			if (abSyme(id) && symeIsExport(abSyme(id)))
				xsymes = listCons(Syme)(abSyme(id), xsymes);
		}
		else if (abTag(argv[i]) == AB_Import
			|| abTag(argv[i]) == AB_Inline) {
			typeInferAs(stab, argv[i], tfNone());
		}
		/* Category expression. */
		else {
			typeInferAs(stab, argv[i], tfCategory);
			/*!! typeInferCheck(stab, argv[i], tfCategory); */
			symes = abGetCatExports(argv[i]);
			isymes = symeListUnion(isymes, symes, symeEqual);
			symes = abGetCatSelf(argv[i]);
			ssymes = symeListUnion(ssymes, symes, symeEqual);

			if (abState(argv[i]) != AB_State_HasUnique ||
			    !tfIsMeaning(abTUnique(argv[i])))
				pending = true;
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
				symeSetSrcPos(xsyme, symeSrcPos(dsyme));
			}

		/* If the default is inherited, use the default syme. */
		if (xsyme == NULL) {
			xsymes = listCons(Syme)(dsyme, xsymes);
			symeSetDefault(dsyme);
		}
	}

	xsymes = listNReverse(Syme)(xsymes);
	/* Order of args to ListUnion is important
	 * This way, we prefer local symes to imports */
	xsymes = symeListUnion(isymes, xsymes, symeEqual);

	if (abTag(absyn) == AB_Nothing || abTag(absyn) == AB_Sequence) {
		abState(absyn) = AB_State_HasUnique;
		abTUnique(absyn) = tfThird(xsymes);
	}

	tf = tfThird(xsymes);
	tfAddSelf(tf, ssymes);
	tfHasSelf(tf) = !pending;

	return tf;
}

local SymeList
tibup0DefaultBody(Stab stab, AbSyn absyn, Bool doDef)
{
	SymeList xsymes;
	AbSyn *argv;
	int    argc, i;
	
	switch (abTag(absyn)) {
	  case AB_Sequence:
		argc = abArgc(absyn);
		argv = absyn->abSequence.argv;
		break;
	  default:
		argc = 1;
		argv = &absyn;
		break;
	}

	xsymes = listNil(Syme);
	for (i=0; i<argc; i++) {
		if (abTag(argv[i]) == AB_If) {
			AbLogic	saveCond;
			SymeList sl1;
			/*
			 * An unfixed compiler bug means that parts of Salli
			 * programs (and thus libAldor) are tinfered with
			 * (tfBoolean == tfUnknown). The correct fix is to
			 * ensure that tfBoolean has been imported into every
			 * scope that needs it before we get this far.
			 */
			if (tfBoolean == tfUnknown)
				comsgFatal(absyn, ALDOR_F_BugNoBoolean);

			/* !! need abExpandDefs() here */
			typeInferAs(stab, argv[i]->abIf.test, tfBoolean);
			ablogAndPush(&abCondKnown, &saveCond, argv[i]->abIf.test, 
				     true);
			sl1 = tibup0DefaultBody(stab, argv[i]->abIf.thenAlt, doDef);
			ablogAndPop (&abCondKnown, &saveCond);
			/* Should add the condition, but that leads to problems
			 * when merging
			 */
			/* !!else part!!*/
			xsymes = listNConcat(Syme)(sl1, xsymes);
		}
		else if (abTag(argv[i]) == AB_Define) {
			AbSyn	id = abDefineeIdOrElse(argv[i], NULL);
			AbSyn	lhs = argv[i]->abDefine.lhs;

			tiTfPushDefinee(lhs);
			typeInferAs(stab, lhs,
				    tiDefineFilter(argv[i],tfUnknown));
			tiTfPopDefinee(lhs);

			if (abSyme(id) && symeIsExport(abSyme(id)))
				xsymes = listCons(Syme)(abSyme(id), xsymes);
		}
		else if (abTag(argv[i]) == AB_Sequence) {
			SymeList sl;
			sl = tibup0DefaultBody(stab, argv[i], doDef);
			/* Why is `sl' not appended to `xsymes' ??? */
		}

		if (doDef)
			typeInferAs(stab, argv[i], tfUnknown);
		/* !! 'where' clauses */
	}
	return xsymes;
}


/****************************************************************************
 *
 * :: Implied call:  apply, set!, test, generator, ...
 *
 ***************************************************************************/

local Bool tibup0ApplyGiveMessage(AbSyn absyn, Length argc, AbSynGetter argf);
local void tibup0ApplyFilter(Stab stab, AbSyn absyn, TForm type, TPoss opTypes,
			     AbSyn op, Length argc, AbSynGetter argf,
			     TPoss *nopTypes, TPoss *retTypes);
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
tibup0ApplySymIfNeeded(Stab stab, AbSyn absyn, TForm type, Symbol fsym,
		Length argc, AbSynGetter argf,
		AbSyn implicitPart, TFormPredicate pred)
{
	AbSyn	part;
	TPoss	tp;

	assert(argc == 1);

	part = argf(absyn, int0);
	tibup(stab, part, tfUnknown);
	tp   = abReferTPoss(part);

	if (tpossCount(tp) == 0)
		abTPoss(absyn) = tpossRefer(tp);

	else if (tpossIsHaving(tp, pred)) {
		TPossIterator	it;
		TForm		tf;
		TPoss		tpOk;

		tpOk = tpossEmpty();
		for (tpossITER(it, tp); tpossMORE(it); tpossSTEP(it)) {
			tf = tpossELT(it);
			if (pred(tf) && tfSatisfies(tf, type))
				tpossAdd1(tpOk, tf);
		}
		abResetTPoss(part, tpOk);
		abTPoss(absyn) = tpossRefer(tpOk);
	}
	else
		tibup0ApplySym(stab,absyn,type, fsym,argc,argf,implicitPart);

	tpossFree(tp);
}

local void
tibup0ApplySym(Stab stab, AbSyn absyn, TForm type,
	       Symbol fsym, Length argc, AbSynGetter argf,
	       AbSyn implicitPart)
{
	AbSyn	imp = abNewId(abPos(absyn), fsym);

	if (!implicitPart) implicitPart = absyn;

	abSetImplicit(implicitPart, imp);
	tibup(stab, imp, tfUnknown);

	tibup0ApplyFType(stab, absyn, type, imp, argc, argf);
}

local void
tibup0ApplyJoin(Stab stab, AbSyn absyn, TForm type,
		 AbSyn op, Length argc, AbSynGetter argf)
{
	SymeList	symes = listNil(Syme), mods = listNil(Syme);
	TForm		selftf;
	Length		i;

	/* Type check each argument as a category. */
	for (i = 0; i < argc; i += 1) {
		AbSyn		argi = argf(absyn, i);

		typeInferAs(stab, argi, tfCategory);
		typeInferCheck(stab, argi, tfCategory);
	}

	/* Collect the category exports for the result. */
	for (i = 0; i < argc; i += 1) {
		AbSyn		argi = argf(absyn, i);
		SymeList	symesi;

		if (abState(argi) == AB_State_HasUnique) {
			symesi = tfGetThdExports(abTUnique(argf(absyn, i)));
			symes = tfJoinExportLists(mods, symes, symesi, NULL);
		}
	}
	selftf = tfThird(symes);

	/* Collect the symbol meaning(s) for self for the result. */
	tfCopySelf(selftf, abTForm(absyn));
	for (i = 0; i < argc; i += 1) {
		AbSyn		argi = argf(absyn, i);
		if (abState(argi) == AB_State_HasUnique)
			tfAddSelf(selftf, tfGetThdSelf(abTUnique(argi)));
	}

	if (!tfSatisfies(selftf, type)) 
		abState(absyn) = AB_State_Error;

	abTPoss(absyn) = tpossSingleton(selftf);
	return;
}

/*
 * For each operator type, see whether some combination of args work.
 * tibup is applied to the virtual arguments.
 */

local void
tibup0ApplyFType(Stab stab, AbSyn absyn, TForm type,
		 AbSyn op, Length argc, AbSynGetter argf)
{
	Length		i;
	TPossIterator	it;
	TPoss		opTypes  = abReferTPoss(op);
	TPoss		nopTypes;
	TPoss		retTypes;

	if (abIsTheId(op, ssymJoin) && tpossIsUnique(opTypes) &&
	    tfSatisfies(tfMapRet(tpossUnique(opTypes)), tfCategory)) {
		tibup0ApplyJoin(stab, absyn, type, op, argc, argf);
		tpossFree(opTypes);
		return;
	}

	for (i = 0; i < argc; i += 1)
		tibup(stab, argf(absyn, i), tfUnknown);

	tibup0ApplyFilter(stab, absyn, type, opTypes, op, argc, argf, &nopTypes, &retTypes);

	/* If the op and the parts had meaning, then give an error. */
	if (tpossCount(nopTypes) == 0) {
		Bool giveMsg = tpossCount(opTypes) > 0
			|| tibup0ApplyGiveMessage(absyn, argc, argf);

		if (giveMsg) {
			abState(absyn) = AB_State_Error;
			abState(op) = AB_State_Error;
		}
		else {
			if (tpossCount( opTypes ) == 0)
				abState(absyn) = AB_State_Error;

			abResetTPoss(op, nopTypes);
		}
	}
	else
		abResetTPoss(op, nopTypes);

	abResetTPoss(absyn, retTypes);
	tpossFree(opTypes);
}

local Bool
tibup0ApplyGiveMessage(AbSyn absyn, Length argc, AbSynGetter argf)
{
	Bool	giveMsg = true;
	int i;

	for (i = 0; giveMsg && i < argc; i += 1) {
		AbSyn	argi = argf(absyn, i);
		if (abState(argi) == AB_State_Error ||
		    (abState(argi) == AB_State_HasPoss &&
		     tpossCount(abTPoss(argi)) == 0))
			giveMsg = false;
	}

	return giveMsg;
}

local void
tibup0ApplyFilter(Stab stab, AbSyn absyn, TForm type, TPoss opTypes,
		  AbSyn op, Length argc, AbSynGetter argf, TPoss *pnopTypes, TPoss *pretTypes)
{
	SatMask		mask = tfSatBupMask(), result;
	TPossIterator	it;
	TPoss nopTypes = tpossEmpty();
	TPoss retTypes = tpossEmpty();

	/* Filter opTypes based on the argument and return types. */
	for (tpossITER(it, opTypes); tpossMORE(it); tpossSTEP(it)) {
		TForm	opType = tpossELT(it), retType;
		AbSub	sigma;

		opType = tfDefineeType(opType);
		if (!tfIsAnyMap(opType)) continue;

		retType = tfMapRet(opType);
		sigma	= absNew(stab);

		result = tfSatMapArgs(mask, sigma, opType, absyn, argc, argf);

		if (tfSatSucceed(result)) {
			retType = tformSubst(sigma, retType);
			result = tfSat(mask, retType, type);
			if (tfSatSucceed(result)) {
				nopTypes = tpossAdd1(nopTypes, opType);
				retTypes = tpossAdd1(retTypes, retType);
			}
		}

		absFreeDeeply(sigma);
	}
	*pnopTypes = nopTypes;
	*pretTypes = retTypes;
}

/****************************************************************************
 *
 * :: Far Values:  return x, yield x, a => x
 *
 ***************************************************************************/

local void
tibup0FarValue(Stab stab, AbSyn absyn, TForm type,
	       AbSyn farValue, TPoss *pFarTPoss)
{
	tibup(stab, farValue, type);

	if (tuniIsInappropriate(*pFarTPoss)) {
		abState(absyn) = AB_State_Error;
		abTPoss(absyn) = tuniInappropriateTPoss;
	}
	else if (tuniIsNoValue(*pFarTPoss)) {
		if (abTag(farValue) != AB_Nothing && abTag(absyn) == AB_Return)
			comsgWarning(absyn, ALDOR_W_TinNoValReturn);
	}
	else {
		TPoss	tp0, tp1;

		tp1 = abReferTPoss(farValue);

		if (tuniIsUnknown(*pFarTPoss)) {
			if (DEBUG(tipFar)) {
				fprintf(dbOut, "Setting ");
				tpossPrint(dbOut, tp1);
				fnewline(dbOut);
			}
			*pFarTPoss = tpossRefer(tp1);
		}
		else if (tpossCount(tp1) != 0) {
			if (DEBUG(tipFar)) {
				fprintf(dbOut, " with ");
				tpossPrint(dbOut, *pFarTPoss);
			}
			tp0 = tpossIntersect(tp1, *pFarTPoss);

			tpossFree(*pFarTPoss);
			*pFarTPoss = tp0;
			
			if (DEBUG(tipFar)) {
				fprintf(dbOut, " to get ");
				tpossPrint(dbOut, *pFarTPoss);
				fnewline(dbOut);
			}
		}
		tpossFree(tp1);
	}
	/* Calling program must set abTPoss(absyn). */
}


/****************************************************************************
 *
 * :: No Values:  empty sequence, if w/o else, exit.
 *
 ***************************************************************************/

local void
tibup0NoValue(Stab stab, AbSyn absyn, TForm type, Msg msg)
{
	if (tfIsNoValueContext(type, absyn))
	{
		/* No values are permitted ... */
		abTPoss(absyn) = tpossSingleton(tfNone());
	}
	else
	{
		/* Oops - value required */
		comsgError(absyn, msg);
		abTPoss(absyn) = tpossEmpty();
	}
}


/****************************************************************************
 *
 * :: Id:  x, +, 1
 * X
 ***************************************************************************/

local void		tibup0IdComplain	(AbSyn);

local void
tibupId(Stab stab, AbSyn absyn, TForm type)
{
	TPoss	tp;

	if (abUse(absyn) == AB_Use_Default)
		tp = tpossSingleton(type);
	else if (abSyme(absyn))
		tp = tpossSingleton(symeType(abSyme(absyn)));
	else {
		/*
		 * Get all possible types for this symbol which satisfy
		 * the current context. Unfortunately this includes types
		 * for export symes that may not be applicable.
		 */
		tp = stabGetTypes(stab, abCondKnown, absyn->abId.sym);
	}

	/*
	 * Future work: if abUse(absyn) == AB_Use_Type and
	 * tpossCount(tp) == 1 then we really ought to enrich
	 * the type in tp with any information about it that
	 * we have in abCondKnown. This will remove the need
	 * for the pretend in the following code:
	 *
	 * MyDomain(S:Type):SomeCategory ==
	 * {
	 *    if (S has AnotherCategory) then
	 *       AnotherDomain(S pretend AnotherCategory);
	 *    else
	 *       ...
	 * }
	 * AnotherDomain(S:AnotherCategory):XXX ...
	 *
	 * At the moment we only get tp = [<* Type *>] and
	 * so we need the pretend for this to go through. The
	 * only problem is that AbLogic is intended for testing
	 * with other AbLogic values rather than for extracting
	 * information from.
	 */

	if (tpossCount(tp) == 1 && tfIsUnknown(tpossUnique(tp)))
		tp = tpossEmpty();
	if (tpossCount(tp) == 0)
		tibup0IdComplain(absyn);

	abTPoss(absyn) = tp;
}

local void
tibup0IdComplain(AbSyn absyn)
{
	Symbol	sym = absyn->abId.sym;

	if (listMemq(Symbol)(terrorIdComplaints, sym)) return;
	terrorIdComplaints = listCons(Symbol)(sym, terrorIdComplaints);

	tpossFree(abTPoss(absyn));
	abState(absyn) = AB_State_Error;
	abTPoss(absyn) = tpossEmpty();
}

/****************************************************************************
 *
 * :: LitInteger:   32
 * :: LitFloat:	    4.0
 * :: LitString:    "hello"
 * X
 * !! This stuff could be made to go through ti...0Apply.
 ***************************************************************************/

local void		tibup0Literal		(Symbol, Stab, AbSyn, TForm);

/*
 * Bottom up entry points.
 */

local void
tibupLitInteger(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Literal(ssymTheInteger, stab, absyn, type);
}

local void
tibupLitFloat(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Literal(ssymTheFloat, stab, absyn, type);
}

local void
tibupLitString(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Literal(ssymTheString, stab, absyn, type);
}

/*
 * Functions which actually do the work.
 */

local void
tibup0Literal(Symbol sym, Stab stab, AbSyn absyn, TForm type)
{
	SatMask		mask = tfSatBupMask();
	Syme		syme = abSyme(absyn);

	/*
	 * Consider the node to be the application of the given symbol
	 * to a string argument:  String -> X
	 * If there is already a symbol meaning on the node, use it.
	 * Otherwise, consult the symbol table and filter by type.
	 */
	if (syme) {
		TForm	opType = symeType(syme);
		TForm	retType;

		if (!tfIsLitOpType(opType)) {
			String	msg = "inappropriate meaning on literal";
			comsgFatal(absyn, ALDOR_F_Bug, msg);
			/*bug("Inappropriate meaning on literal."); */
		}

		retType = tfMapRet(opType);
		if (!tfSatSucceed(tfSat(mask, retType, type))) {
			String	msg = "inappropriate meaning on literal";
			comsgFatal(absyn, ALDOR_F_Bug, msg);
			/*bug("Inappropriate meaning on literal."); */
		}

		abTPoss(absyn) = tpossSingleton(retType);
	}
	else {
		TPoss		opTypes, litTypes;
		TPossIterator	tit;

		litTypes = tpossEmpty();
		opTypes	 = stabGetTypes(stab, abCondKnown, sym);

		if (DEBUG(tipLit)) {
			fprintf(dbOut, "tibup0Literal:\n");
			tpossPrint(dbOut, opTypes);
			fnewline(dbOut);
		}

		for (tpossITER(tit,opTypes) ; tpossMORE(tit); tpossSTEP(tit)) {
			TForm opType = tpossELT(tit);
			TForm retType;

			tfFollow(opType);
			if (!tfIsLitOpType(opType)) continue;

			retType = tfMapRet(opType);
			if (tfSatSucceed(tfSat(mask, retType, type)))
				litTypes = tpossAdd1(litTypes, retType);
		}
		tpossFree(opTypes);

		abTPoss(absyn) = litTypes;

		if (tpossCount(litTypes) == 0)
			abState(absyn) = AB_State_Error;
	}
}

/***************************************************************************
 *
 * :: Comma: (a, b)
 *
 ***************************************************************************/

local TPoss
abGetArgTPoss(AbSyn *abv, Length i)
{
	return abReferTPoss(abv[i]);
}

local Bool
abSymeInducesDependency(AbSyn id, AbSyn absyn)
{
	if (id == absyn)
		return false;

	else if (abHasTag(absyn, AB_Id) && id->abId.sym == absyn->abId.sym)
		return true;

	else if (!abIsLeaf(absyn)) {
		Length	i;
		for (i = 0; i < abArgc(absyn); i += 1)
			if (abSymeInducesDependency(id, abArgv(absyn)[i]))
				return true;
	}

	return false;
}

local TPoss
tpossDefine(Syme syme, TPoss tp, AbSyn ab)
{
	TPoss		ntp = tpossEmpty();
	TPossIterator	it;

	for (tpossITER(it, tp); tpossMORE(it); tpossSTEP(it)) {
		TForm	tf = tpossELT(it);
		tf = tfDefine(tfDeclare(abFrSyme(syme), tf), ab);
		tpossAdd1(ntp, tf);
	}

	return ntp;
}

local void
tibupComma(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn *	argv	= abArgv(absyn);
	Length	i, argc	= abArgc(absyn);
	Stab    istab  = NULL;
	AbSub   sigma  = NULL;
	TForm *	trhsv	= NULL;
	Bool	decl;
	TForm	tf;
	TPoss	tp;

	decl =	abUse(absyn) == AB_Use_Declaration ||
		abUse(absyn) == AB_Use_Default;

	tf = decl ? type : tfUnknown;

	for (i = 0; i < argc; i++)
		tibup(stab, argv[i], tf);

	if (abUse(absyn) == AB_Use_Value ||
	    abUse(absyn) == AB_Use_RetValue) {
		if (tfIsCross(type) && tfCrossArgc(type) == argc)
			trhsv = tfCrossArgv(type);
		if (tfIsMulti(type) && tfMultiArgc(type) == argc)
			trhsv = tfMultiArgv(type);
	}

	for (i = 0; i < argc; i++) {
		AbSyn	abi = argv[i];
		TPoss	ntp = NULL;

		if (abState(abi) != AB_State_HasPoss) continue;
		tp = abTPoss(abi);

		/* Add the syme to the type in declaration contexts. */
		if (abTag(abi) == AB_Declare) {
			AbSyn	id = abi->abDeclare.id;
			if (abSymeInducesDependency(id, absyn))
				ntp = tpossDeclare(abSyme(id), tp);
		}
		else if (abTag(abi) == AB_Id)
			if (abUse(absyn) == AB_Use_Define)
				ntp = tpossDeclare(abSyme(abi), tp);

		/* Add the value to the type in value contexts. */
		if (!ntp && trhsv) {
			/* Technique for shifting a type form to an 
			 * unrelated position in stab.
			 */
			Syme	syme = (Syme)NULL;
			TForm	tfi = trhsv[i];

			if (istab == NULL && tfIsDeclare(tfi)) {
				istab = stabPushLevel(stab, abPos(absyn),(ULong) 0);
				sigma = absNew(tfStab(type));
			}
			tfFollow(tfi);

#if AXL_EDIT_1_1_12p6_07
			if (tfIsDeclare(tfi))
				syme = tfDeclareSyme(tfi);

			/*
			 * If this value is a dependent type and we
			 * failed to obtain a unique meaning during
			 * typeInferTForms then we may not have a
			 * syme to associate with this definition.
			 * (see bug 1238 for example).
			 */
			if (syme) {
				if (!stabHasMeaning(stab, syme)) {
					if (!istab) {
						istab = stabPushLevel(stab, abPos(absyn),(ULong) 0);
						sigma = absNew(tfStab(type));
					}
					syme = stabDefParam(istab, symeId(syme), tformSubst(sigma, tfi));
				}
				ntp = tpossDefine(syme, tp, abi);
			}
#else
			if (tfIsDeclare(tfi)) {
				Syme syme = tfDeclareSyme(tfi);

				if (!stabHasMeaning(stab, syme)) {
					if (!istab) {
						istab = stabPushLevel(stab, abPos(absyn),(ULong) 0);
						sigma = absNew(tfStab(type));
					}
					syme = stabDefParam(istab, symeId(syme), tformSubst(sigma, tfi));
				}
				ntp = tpossDefine(syme, tp, abi);
			}
#endif
		}

		if (ntp) abResetTPoss(abi, ntp);
	}

	if (decl)
		tp = tpossSingleton(tfNone());
	else
		tp = tpossMulti(argc, argv, (TPossGetter) abGetArgTPoss);

	abTPoss(absyn) = tp;
}

/****************************************************************************
 *
 * :: Apply:   f(a, b, ...)
 *
 ***************************************************************************/

local void
tibupApply(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn op = abApplyOp(absyn);
	AbSyn imp = NULL;
	TPoss opTypes;
	TPoss nopTypes;
	TPoss retTypes;
	TPoss impOpTypes;
	TPoss impRetTypes;
	int i;

	tibup(stab, abApplyOp(absyn), tfUnknown);

	opTypes = abReferTPoss(op);

	if (abIsTheId(op, ssymJoin) && tpossIsUnique(opTypes) &&
	    tfSatisfies(tfMapRet(tpossUnique(opTypes)), tfCategory)) {
		tibup0ApplyJoin(stab, absyn, type, op, abApplyArgc(absyn), abApplyArgf);
		tpossFree(opTypes);
		return;
	}

	for (i = 0; i < abApplyArgc(absyn); i += 1)
		tibup(stab, abApplyArg(absyn, i), tfUnknown);

	tibup0ApplyFilter(stab, absyn, type, opTypes,
			  op, abApplyArgc(absyn), abApplyArgf, &nopTypes, &retTypes);

	if (tpossHasNonMapType(opTypes)) {
		imp = abNewId(abPos(absyn), ssymApply);

		abSetImplicit(absyn, imp);
		tibup(stab, imp, tfUnknown);

		tibup0ApplyFilter(stab, absyn, type, abTPoss(imp),
				  imp, abArgc(absyn), abArgf, &impOpTypes, &impRetTypes);

		if (tpossCount(impOpTypes) > 0) {
			TPoss tmp2 = retTypes;
			retTypes = tpossUnion(retTypes, impRetTypes);
			tpossFree(tmp2);
		}
		else {
			abFree(imp);
			imp = NULL;
			abSetImplicit(absyn, NULL);
		}
	}

	/* If the op and the parts had meaning, then give an error. */
	if (tpossCount(retTypes) == 0) {
		Bool giveMsg = tpossCount(opTypes) > 0
			|| tibup0ApplyGiveMessage(absyn, abApplyArgc(absyn), abApplyArgf);

		if (giveMsg) {
			abState(absyn) = AB_State_Error;
			abState(op) = AB_State_Error;
		}
		else {
			if (tpossCount( opTypes ) == 0)
				abState(absyn) = AB_State_Error;

			if (!imp)
				abResetTPoss(op, nopTypes);
			if (imp)
				abResetTPoss(imp, impOpTypes);
		}
	}
	else {
		if (!imp)
			abResetTPoss(op, nopTypes);
		if (imp)
			abResetTPoss(imp, impOpTypes);
	}

	abResetTPoss(absyn, retTypes);
	tpossFree(opTypes);
}

/****************************************************************************
 *
 * :: Define:	a == e
 * X
 ***************************************************************************/

/* Select the types from T which absyn satisfies. */

local TPoss
tpossFilterSatisfiers(AbSyn absyn, TPoss T)
{
	TPoss	tp, result;

	tp = abReferTPoss(absyn);
	result = tpossSatisfies(tp, T);
	tpossFree(tp);

	return result;
}

local void
tibupDefine(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	lhs = absyn->abDefine.lhs;
	AbSyn	rhs = absyn->abDefine.rhs;
	Bool	key = abTag(lhs) != AB_Declare && abUse(absyn) == AB_Use_Value;

	tiTfPushDefinee(lhs);

	if (key) {
		/* !! We don't check the type of keyword arguments. */
		tibup(stab, rhs, type);
		abState(lhs) = AB_State_HasPoss;
		abTPoss(lhs) = abReferTPoss(rhs);
	}
	else if (abTag(lhs) == AB_Declare) {
		tibup(stab, lhs, type);
		type = abTForm(lhs->abDeclare.type);
		tibup(stab, rhs, type);
	}
	else {
		TPoss	tprhs, tpnew;
		TForm	t;
		TPossIterator	tit;


		/* Compute possible types for the RHS */
		tibup(stab, rhs, type);
		tpnew = abReferTPoss(rhs);


		/* Filter out void types */
		tprhs = tpossEmpty();
		for (tpossITER(tit,tpnew); tpossMORE(tit); tpossSTEP(tit))
		{
			/* Get the next type possibility */
			t = tpossELT(tit);


			/* Is it an empty multi? */
			if (tfIsMulti(t) && !tfArgc(t)) continue;


			/* No - add it to the set of possible types */
			tprhs = tpossAdd1(tprhs, t);
		}


		/* Update the tposs for the RHS */
		/* abTPoss(rhs) = tpossRefer(tprhs); */
		tpossFree(tpnew);


		/* Compute types for LHS based on RHS set */
		tibup0InferLhs(stab, absyn, lhs, rhs, tprhs);
		tibup(stab, lhs, type);


		/* Check that we have at least one type for RHS */
		if (!tpossCount(tprhs))
			abState(absyn) = AB_State_Error;
	}

	tiTfPopDefinee(lhs);

	if (abTag(lhs) == AB_Declare)
	{
		AbSyn idtype   = lhs->abDeclare.type;
		abTPoss(absyn) = abReferTPoss(idtype);
	}
	else
		abTPoss(absyn) = abReferTPoss(rhs);


	if (DEBUG(tipDefine)) {
		TPoss	abtposs = abReferTPoss(absyn);
		fprintf(dbOut,"Bup: Define of ");
		abPrint(dbOut, lhs);
		fprintf(dbOut," has %d types ", tpossCount(abtposs));
		tpossPrint(dbOut, abtposs);
		fnewline(dbOut);
		tpossFree(abtposs);
	}
}

/****************************************************************************
 *
 * :: Assign:	a := e
 * X
 ***************************************************************************/

local void
tibup0InferLhs(Stab stab, AbSyn absyn, AbSyn lhs, AbSyn rhs, TPoss tprhs)
{
	AbSyn *	lhsv	= abArgvAs(AB_Comma, lhs);
	Length	i, lhsc	= abArgcAs(AB_Comma, lhs);
	TForm *	trhsv	= NULL;
	TForm	trhs;
	AbSub	sigma;

	if (!tpossIsUnique(tprhs)) return;
	trhs = tpossUnique(tprhs);
	tfFollow(trhs);
	if (tfIsUnknown(trhs)) return;

	if (abTag(lhs) == AB_Id) {
		trhsv= &trhs;
	}
	else if (abTag(lhs) == AB_Comma) {
		trhs = tfDefineeBaseType(trhs);
		rhs = NULL;
		if (tfIsCross(trhs) && tfCrossArgc(trhs) == lhsc)
			trhsv = tfCrossArgv(trhs);
		if (tfIsMulti(trhs) && tfMultiArgc(trhs) == lhsc)
			trhsv = tfMultiArgv(trhs);
	}
	if (!trhsv) return;

	sigma = absNew(stab);
	for (i = 0; i < lhsc; i++) {
		AbSyn	abi = lhsv[i];
		TForm	tfi = trhsv[i];
		Syme	syme = NULL;

		tfi = tformSubst(sigma, tfi);
		tfFollow(tfi);
		if (tfIsDeclare(tfi)) {
			syme = tfDeclareSyme(tfi);
			tfi = tfDeclareType(tfi);
		}

		if (abTag(abi) == AB_Id)
			tibup0InferLhsId(stab, lhs, abi, tfi, rhs);

		else if (abTag(abi) == AB_Apply) {
			tibup0InferLhsApply(stab, lhs, abi, tfi);
			if (abState(abi) == AB_State_Error)
				abState(absyn) = AB_State_Error;
		}

		/* Extend the sublist for dependent symes. */
		if (abTag(abi) == AB_Declare)
			abi = abi->abDeclare.id;

		if (abTag(abi) == AB_Id && syme != NULL) {
			AbSyn	ab = abCopy(abi);
			tiBottomUp(stab, ab, tfUnknown);
			tiTopDown (stab, ab, tfi);
			if (abState(ab) == AB_State_HasUnique) {
				if (absFVars(sigma))
					absSetFVars(sigma, NULL);
				sigma = absExtend(syme, ab, sigma);
			}
		}
	}

	absFree(sigma);
}

local void
tibup0InferLhsId(Stab stab, AbSyn lhs, AbSyn ab, TForm tf, AbSyn rhs)
{
	Symbol	sym = abIdSym(ab);
	
	if (stabIsUndeclaredId(stab, sym) && !tfIsUnknown(tf)) {
		if (tfIsMulti(tf) && rhs) {
			tf = tfCrossFrMulti(tf);
			abSetTContext(rhs, AB_Embed_MultiToCross);
		}
		if (comsgOkRemark(ALDOR_R_TinInferring)) {
			String	typestr = tfPretty(tf);
			comsgRemark(lhs, ALDOR_R_TinInferring,
				    symString(sym), typestr);
			strFree(typestr);
		}
		stabDeclareId(stab, sym, tf);
	}
}

local void
tibup0InferLhsApply(Stab stab, AbSyn lhs, AbSyn ab, TForm tf)
{
	AbSyn	rhs = abNewNothing(abPos(lhs));
	AbSyn	aba = abNewAssign(abPos(lhs), ab, rhs);

	abTPoss(rhs) = tpossSingleton(tf);
	abState(rhs) = AB_State_HasPoss;
	tibup0ApplySym(stab, aba, tfUnknown, ssymSetBang,
		       abArgc(ab) + 1, abSetArgf, ab);

	if (abState(aba) == AB_State_Error) {
		abState(ab) = AB_State_Error;
		tibup0IdComplain(ab);
	}
	/* This is because the tibup following tibup0LhsInfer
	 * ignores the information we've just found, and puts 
	 * in implicit apply on this node.
	 */
	comsgError(ab, ALDOR_E_TinEmbeddedSet);

	tpossFree(abTPoss(rhs));
	tpossFree(abTPoss(aba));
	abFree(rhs);
	abFreeNode(aba);
}

local void
tibupAssign(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	lhs = absyn->abAssign.lhs;
	AbSyn	rhs = absyn->abAssign.rhs;
	TPoss	tplhs, tprhs, tpnew;
	TForm	t;
	TPossIterator	tit;

	if (abTag(lhs) == AB_Apply) {
		tibup0ApplySym(stab, absyn, type,
			       ssymSetBang, abArgc(lhs) + 1, abSetArgf, lhs);
		if (abState(absyn) != AB_State_HasUnique &&
		    tpossCount(abGoodTPoss(absyn))== 0)  {
 			abState(lhs) = AB_State_HasPoss;
 			abTPoss(lhs) = tpossEmpty();
		}
		return;
	}


	/* Compute the set of possible types for the RHS */
	tibup(stab, rhs, type);
	tpnew = abReferTPoss(rhs);


	/* Filter out void types */
	tprhs = tpossEmpty();
	for (tpossITER(tit,tpnew); tpossMORE(tit); tpossSTEP(tit))
	{
		/* Get the next type possibility */
		t = tpossELT(tit);


		/* Is it an empty multi? */
		if (tfIsEmptyMulti(t)) continue;


		/* No - add it to the set of possible types */
		tprhs = tpossAdd1(tprhs, t);
	}
	tpossFree(tpnew);


	/*
	 * Compute the possible types of the lhs restricted to
	 * those in the set of possible types of the rhs.
	 */
	tibup0InferLhs(stab, absyn, lhs, rhs, tprhs);
	tibup(stab, lhs, type);

	if (abState(absyn) == AB_State_Error) {
		tpossFree(tprhs);
		return;
	}
	if (abState(rhs) == AB_State_Error && abTag(lhs) == AB_Id) {
		abState(lhs) = AB_State_HasPoss;
		abTPoss(lhs) = tpossEmpty();
		abState(absyn) = AB_State_HasPoss;
		abTPoss(absyn) = tpossEmpty();
		return;
	}

	tplhs = abReferTPoss(lhs);
	abTPoss(absyn) = tpossFilterSatisfiers(rhs, tplhs);

	if (tpossCount(tprhs) > 0 && tpossCount(abTPoss(absyn)) != 1)
		abState(absyn) = AB_State_Error;

	tpossFree(tplhs);
	tpossFree(tprhs);

	if (DEBUG(tipAssign)) {
		TPoss tposs = abTPoss(absyn);
		fprintf(dbOut,"Bup: Assignment to ");
		abPrint(dbOut, lhs);
		fprintf(dbOut,"has %d types ", tpossCount(tposs));
		tpossPrint(dbOut, tposs);
		fnewline(dbOut);
	}
}


/****************************************************************************
 *
 * :: Declare:	 a: A
 *
 ***************************************************************************/

local void
tibupDeclare(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	id     = absyn->abDeclare.id;
	AbSyn	idtype = absyn->abDeclare.type;
	TPoss	tp, ntp;
	Bool	tupe = (abUse(absyn) == AB_Use_Type);
	Bool	defn = (abUse(absyn) == AB_Use_Define) ||
		       (abUse(absyn) == AB_Use_Assign);

	tiGetTForm(stab, idtype);

	tibup(stab, id, abTForm(idtype));

	if (abIsNothing(id))
		tp = tpossSingleton(abTForm(idtype));
	else if (defn)
		tp = abReferTPoss(id);
	else
		tp = abReferTPoss(idtype);

	if (!defn && !tupe) {
		ntp = tpossSatisfiesType(tp, type);
		tpossFree(tp);
		tp = ntp;
	}

	abTPoss(absyn) = tp;

	if (DEBUG(tipDeclare)) {
		TPoss tposs = abGoodTPoss(absyn);
		fprintf(dbOut,"Bup: Declare of ");
		abPrint(dbOut, id);
		fprintf(dbOut," has %d types ", tpossCount(tposs));
		tpossPrint(dbOut, tposs);
		fnewline(dbOut);
	}
}

/****************************************************************************
 *
 * :: Label:   @@ x @@ [e]
 *
 ***************************************************************************/

local void
tibupLabel(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn expr = absyn->abLabel.expr;
	tibup(stab, expr, type);
#if 0
	if (abState(expr) == AB_State_Error)
		abState(absyn) = AB_State_Error;
#endif
	abTPoss(absyn) = abReferTPoss(expr);
}

/****************************************************************************
 *
 * :: Goto:   goto id
 *
 ***************************************************************************/

local void
tibupGoto(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn		label, l0;
	AbSynList	labelList;
	Syme		syme;

	label	  = absyn->abGoto.label;
	labelList = stabGetLabels(stab, label->abId.sym);

	if (!labelList) {
		abState(absyn) = AB_State_Error;
	}
	else {
		l0 = car(labelList);
		listFree(AbSyn)(labelList);

		syme = abSyme(l0);
		assert(syme);
		symeSetDVMark(syme, symeDVMark(syme) + 1);
		abSetSyme(label, syme);
	}

	abTPoss(absyn) = tpossSingleton(tfExit);
}

/****************************************************************************
 *
 * :: Lambda:	(a: A): B +-> b
 * :: PLambda:	(a: A): B +->* b
 *
 ***************************************************************************/

local void
tibupLambda(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupLambda");
	AbSyn		param = absyn->abLambda.param;
	AbSyn		ret   = absyn->abLambda.rtype;
	AbSyn		body  = absyn->abLambda.body;
	TForm		tf    = tiGetTForm(stab, ret);
	Bool		pack  = abHasTag(absyn, AB_PLambda);

	TPoss		fluid(tuniReturnTPoss);
	TPoss		fluid(tuniYieldTPoss);
	TForm		fluid(tuniYieldType);
	TPoss		fluid(tuniExitTPoss);
	SymbolList	fluid(terrorIdComplaints);

	tuniReturnTPoss	   = tuniInappropriateTPoss;
	tuniYieldTPoss	   = tuniInappropriateTPoss;
	tuniYieldType	   = tfUnknown;
	tuniExitTPoss	   = tuniInappropriateTPoss;
	terrorIdComplaints = 0;

	tuniReturnTPoss = tpossSingleton(tf);
	typeInferAs(stab, param, tfUnknown);
	tibup(stab, body,  tf);

	/* There can be only one type for this lambda ... */
	tf = tfAnyMap(tfFullFrAbSyn(stab, param), tf, pack);
	abTPoss(absyn) = tpossSingleton(tf);

	listFree(Symbol)(terrorIdComplaints);
	ReturnNothing;
}

/****************************************************************************
 *
 * :: Sequence:	  (a; b; c)
 *
 ***************************************************************************/

local void
tibupSequence(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupSequence");
	TPoss	fluid(tuniExitTPoss);
	TForm	fluid(tuniExitType);
	TPoss   fluid(tuniSelectTPoss);
	AbSyn   fluid(tuniBupSelectObj);

	tuniSelectTPoss  = NULL;
	tuniBupSelectObj = NULL;

 	if (abUse(absyn) == AB_Use_NoValue || tfIsNone(type))
		tuniExitTPoss = tuniNoValueTPoss;
	else
		tuniExitTPoss = tuniUnknownTPoss;

	tuniExitType = type;

	tibupSequence0(stab, absyn, type);

	ReturnNothing;
}

local void
tibupSequence0(Stab stab, AbSyn absyn, TForm type)
{
	Length i, n = abArgc(absyn);

	if (n == 0)
		tibup0NoValue(stab, absyn, type, ALDOR_E_TinContextSeq);
	else {
		AbSyn	arg;
		TPoss tp;

		for (i = 0; i < n-1; i++)
			tibup(stab, abArgv(absyn)[i], tfUnknown);

		arg = abArgv(absyn)[n-1];
		tibup0FarValue(stab, absyn, type, arg, &tuniExitTPoss);

		if (tuniIsNoValue(tuniExitTPoss))
			tuniExitTPoss = tpossSingleton(tfNone());
		else if (tuniIsUnknown(tuniExitTPoss))
			tuniExitTPoss = abReferTPoss(arg);

		tp = tpossSatisfiesType(tuniExitTPoss, type);

		/*
		 * If this context is completely unconstrained
		 * and we cannot find any meanings for this
		 * sequence then there is an error. Otherwise
		 * the caller must deal with checking that the
		 * set of possible types is sensible.
		 */
		if (tpossCount(tp) == 0 && tfIsUnknown(type)) {
			abState(absyn) = AB_State_Error;
			abTPoss(absyn) = tpossEmpty();
			tpossFree(tp);
		}
		else {
			abTPoss(absyn) = tp;
			tpossFree(tuniExitTPoss);
		}
	}

	return;
}

/****************************************************************************
 *
 * :: Exit:  (...; b => x ; ...)
 *
 ***************************************************************************/

local void
tibupExit(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	test  = absyn->abExit.test;
	AbSyn	value = absyn->abExit.value;
	AbLogic	saveCond;

	tibup(stab, test, tfUnknown);

	if (!tuniBupSelectObj) {
		/*
		 * Cases in a select don't affect abCondKnown at the
		 * moment. We could do better though because we know
		 * that `t case v' holds for the exit branch.
		 */
		AbSyn	nTest = abExpandDefs(stab, test);
		ablogAndPush(&abCondKnown, &saveCond, nTest, true);
	}

	tibup0FarValue(stab, absyn, tuniExitType, value, &tuniExitTPoss);

	if (!tuniBupSelectObj)
		ablogAndPop (&abCondKnown, &saveCond);

	if (abState(absyn) != AB_State_Error)
		tibup0NoValue(stab, absyn, type, ALDOR_E_TinContextExit);
}

/***************************************************************************
 *
 * :: return: return x,	 return;
 *
 ***************************************************************************/

local void
tibupReturn(Stab stab, AbSyn absyn, TForm type)
{
	tibup0FarValue(stab, absyn, tfUnknown, absyn->abReturn.value,
		       &tuniReturnTPoss);
	if (abState(absyn) != AB_State_Error)
		abTPoss(absyn) = tpossSingleton(tfExit);
}

/****************************************************************************
 *
 * :: Generate:	 generate [N] of (... yield ...)
 *
 ***************************************************************************/

local void
tibupGenerate(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupGenerate");
	TPoss		tpossIt;
	TPossIterator	tit;
	TForm t, inner;

	TPoss	fluid(tuniReturnTPoss);
	TPoss	fluid(tuniYieldTPoss);
	TPoss	fluid(tuniExitTPoss);
	TForm   fluid(tuniYieldType);

	tuniReturnTPoss = tuniInappropriateTPoss;
	tuniYieldTPoss	= tuniInappropriateTPoss;
	tuniExitTPoss	= tuniInappropriateTPoss;

	tuniYieldTPoss	= tuniUnknownTPoss;

	if (tfIsGenerator(type)) 
		inner = tfGeneratorArg(type);
	else 
		inner = tfUnknown;

	tuniYieldType = inner;

	tibup(stab, absyn->abGenerate.count, tfUnknown);
	tibup(stab, absyn->abGenerate.body,  tfUnknown);

	if (tuniIsUnknown(tuniYieldTPoss)) {
		if (tfIsUnknown(inner))
			tuniYieldTPoss = tpossSingleton(tfNone());
		else
			tuniYieldTPoss = tpossSingleton(inner);
	}

	tpossIt = tpossEmpty();

	for (tpossITER(tit,tuniYieldTPoss); tpossMORE(tit); tpossSTEP(tit)) {
		t = tpossELT(tit);
		if (tfSatisfies(t, inner)) {
			if (tfIsMulti(t)) t = tfCrossFrMulti(t);
			tpossIt = tpossAdd1(tpossIt, tfGenerator(t));
		}
	}

	if (!tfIsUnknown(type) && !tfIsGenerator(type)) {
		tpossFree(tpossIt);
		abState(absyn) = AB_State_Error;
		abTPoss(absyn) = tpossEmpty();
	} else if (tpossCount(tpossIt) == 0 &&
		   tpossCount(tuniYieldTPoss) != 0) {
		tpossFree(tpossIt);
		abState(absyn) = AB_State_Error;
		tpossIt = tpossEmpty();
		for (tpossITER(tit,tuniYieldTPoss); 
		     tpossMORE(tit); 
		     tpossSTEP(tit)) {
			t = tpossELT(tit);
			if (tfIsMulti(t)) t = tfCrossFrMulti(t);
			tpossIt = tpossAdd1(tpossIt, tfGenerator(t));
		}
		abTPoss(absyn) = tpossIt;
		tpossFree(tuniYieldTPoss);
	}
	else {
		abTPoss(absyn) = tpossIt;
		tpossFree(tuniYieldTPoss);
	}

	ReturnNothing;
}

/***************************************************************************
 *
 * :: Yield:  yield x
 *
 ***************************************************************************/

local void
tibupYield(Stab stab, AbSyn absyn, TForm type)
{
	tibup0FarValue(stab, absyn, tuniYieldType, absyn->abYield.value,
		       &tuniYieldTPoss);
	if (abState(absyn) != AB_State_Error)
		abTPoss(absyn) = tpossSingleton(tfExit);
}

/***************************************************************************
 *
 * :: Reference: ref id
 *
 ***************************************************************************/

local void
tibupReference(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn		body = absyn -> abReference.body;
	TPoss		tp, tpRef;
	TPossIterator	iter;
	TForm		tf, inner;


	/* Check the body */
	tibupRefArg(stab, body, tfUnknown);


	/* Get the type of the Ref argument */
	if (tfIsReference(type))
		inner = tfReferenceArg(type);
	else
		inner = tfUnknown;


	/*
	 * Convert: e -> Ref(e) for all e in tp. In the process
	 * we filter our any maps (functions) since we can't take
	 * references of function applications.
	 */
	tp	= abReferTPoss(body);
	tpRef	= tpossEmpty();
	for (tpossITER(iter, tp); tpossMORE(iter); tpossSTEP(iter))
	{
		tf = tpossELT(iter);
		if (tfSatisfies(tf, inner))
		{
			/* Multi's are converted into Cross's */
			if (tfIsMulti(tf))
				tf = tfCrossFrMulti(tf);


			/* Convert e -> Ref(e) */
			tpRef = tpossAdd1(tpRef, tfReference(tf));
		}
	}


	/* Check the type context and the possible types */
	if (!tfIsUnknown(type) && !tfIsReference(type))
	{
		/* This ought not to happen? */
		tpossFree(tpRef);
		abState(absyn) = AB_State_Error;
		abTPoss(absyn) = tpossEmpty();
	}
	else if ((tpossCount(tpRef) == 0) && (tpossCount(tp) == 0))
	{
		abState(absyn) = AB_State_Error;
		abTPoss(absyn) = tpRef;
	}
	else if ((tpossCount(tpRef) == 0) && (tpossCount(tp) != 0))
	{
		/*
		 * There were some possible types for the expression that
		 * the user wants to make a reference of. However, none
		 * were suitable for our needs, either because they were
		 * mappings/functions or because they don't satisfy the
		 * type context.
		 *
		 * Set the error flag and recompute the set of possible
		 * types that we ignored since they weren't any good. The
		 * error messaging system can use these types to tell the
		 * user which ones were considered and discarded.
		 *
		 * Our error message is going to be somewhat confusing.
		 * At the moment they will get something akin to the
		 * classic S-algol error "int and int are not compatible".
		 */
		tpossFree(tpRef);
		abState(absyn) = AB_State_Error;

		tpRef = tpossEmpty();
		for (tpossITER(iter,tp); tpossMORE(iter); tpossSTEP(iter))
		{
			tf = tpossELT(iter);

			if (tfIsMulti(tf))
				tf = tfCrossFrMulti(tf);

			tpRef = tpossAdd1(tpRef, tfReference(tf));
		}

		abTPoss(absyn) = tpRef;
	}
	else
		abTPoss(absyn) = tpRef;
}

/*
 * Ensure that ref() arguments aren't
 * function applications or other
 * unsuitable items
 */
local void
tibupRefArg(Stab stab, AbSyn absyn, TForm type)
{


	/* Type infer the whole expression first */
	tibup(stab, absyn, type);


	/* If it isn't an application then we've finished */
	if (!abIsApply(absyn))
		return;


	/*
	 * At the moment we only permit references of identifiers. In
	 * future we would like to allow references of array/record
	 * elements. This can be generalised to any domain which has
	 * suitable apply/set! operators hinting that it is some form
	 * of updatable aggregate. If the domain has these operators
	 * and is a functional aggregate then the user ought not to
	 * be taking references anyway ...
	 */
	abState(absyn) = AB_State_Error;
	return;


	/* Get all possible meanings */
	tibup0RefImps(stab, absyn, type);
}


local void
tibup0RefImps(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn		op;
	TPossIterator	iter;
	Length		i;
	TPoss		opTypes;
	TPoss		nopTypes = tpossEmpty();
	TPoss		retTypes = tpossEmpty();
	TPoss		bangTypes = tpossEmpty();
	SatMask		mask = tfSatBupMask();
	SatMask		result;


	/* First identify all set! exports */
	op = abNewId(abPos(absyn), ssymSetBang);
	tibup(stab, op, tfUnknown);
	opTypes = abReferTPoss(op);


	/* Clean up the tposs */
	for (tpossITER(iter,opTypes); tpossMORE(iter); tpossSTEP(iter))
	{
		TForm	opType = tpossELT(iter);


		/* Extract the true type of this operator */
		opType = tfDefineeType(opType);
		if (!tfIsAnyMap(opType)) continue;


		/* Multi's are converted into Cross's */
		if (tfIsMulti(opType))
			opType = tfCrossFrMulti(opType);


		/* Add it to our set of possible types */
		bangTypes = tpossAdd1(bangTypes, opType);
	}


	/* Free up opTypes */
	tpossFree(opTypes);


	/* Only interested in implicit apply operations */
	op = abNewId(abPos(absyn), ssymApply);
	tibup(stab, op, tfUnknown);
	opTypes = abReferTPoss(op);


	/*
	 * Filter out any application where the symbol being applied
	 * comes from a domain which doesn't export both
	 *    apply : (%, SingleInteger) -> T
	 *    set!  : (%, SingleInteger, T) -> T
	 * This is too weak in general but will suffice for now
	 * since detecting function applications is tricky.
	 *
	 * Note that we know there must be a suitable apply() operator
	 * since this will have been picked up with the tibup() above.
	 * We can use this to obtain the type T and check if there
	 * is an equivalent set!() export.
	 */
	for (tpossITER(iter,opTypes); tpossMORE(iter); tpossSTEP(iter))
	{
		TForm	opType = tpossELT(iter), retType;
		AbSub	sigma;


		/* Extract the true type of this operator */
		opType = tfDefineeType(opType);
		if (!tfIsAnyMap(opType)) continue;


		/* Multi's are converted into Cross's */
		if (tfIsMulti(opType))
			opType = tfCrossFrMulti(opType);


		/* Determine the return type */
		retType = tfMapRet(opType);


		/* Create a substitution map and check satisfaction */
		sigma	= absNew(stab);
		result = tfSatMapArgs(mask, sigma, opType, absyn,
				abArgc(absyn), abArgf);

		if (tfSatSucceed(result)) {
			retType = tformSubst(sigma, retType);
			result = tfSat(mask, retType, type);
			if (tfSatSucceed(result)) {
				nopTypes = tpossAdd1(nopTypes, opType);
				retTypes = tpossAdd1(retTypes, retType);
			}
		}

		absFreeDeeply(sigma);
	}


	/*
	 * rettypes - these are the types for the whole expression
	 * noptypes - these are the types of specific apply operators
	 */
	/* If the op and the parts had no meaning, then give an error. */
	if (tpossCount(nopTypes) == 0) {
		Bool giveMsg = tpossCount(opTypes) > 0
			|| tibup0ApplyGiveMessage(absyn, abArgc(absyn), abApplyArgf);

		if (giveMsg) {
			abState(absyn) = AB_State_Error;
			abState(op) = AB_State_Error;
		}
		else {
			if (tpossCount( opTypes ) == 0)
				abState(absyn) = AB_State_Error;

			abResetTPoss(op, nopTypes);
		}
	}
	else
		abResetTPoss(op, nopTypes);

	abResetTPoss(absyn, retTypes);
	tpossFree(opTypes);
}


/****************************************************************************
 *
 * :: Add:  [D] add (a: A == ...)
 *
 ***************************************************************************/

local void
tibupAdd(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupAdd");
	SymbolList	fluid(terrorIdComplaints);
	AbSyn		base	= absyn->abAdd.base;
	AbSyn		capsule = absyn->abAdd.capsule;
	SymeList	symes;
	TForm		tfw;

	terrorIdComplaints = listNil(Symbol);

	tiGetTForm(stab, base);
	typeInferCheck(stab, base, tfDomain);
	symes = tiAddSymes(stab, capsule, abTForm(base), type, (SymeList*)NULL);

	typeInferAs(stab, capsule, tfUnknown);

	if (symes) {
		if (tiIsSoftMissing()) {
			terrorNotEnoughExports(stab, absyn,
				tpossSingleton(tfWithFrSymes(symes)), true);
			tfw = tfWithFrAbSyn(absyn);
		}
		else {
			abState(absyn) = AB_State_Error;
			tfw = tfWithFrSymes(symes);
			tfSetSelf(tfw, tfGetCatSelf(type));
		}
	}
	else
		tfw = tfWithFrAbSyn(absyn);

	abTPoss(absyn) = tpossSingleton(tfw);
	if (!tfSatisfies(tfw, type)) 
		abState(absyn) = AB_State_Error;

	listFree(Symbol)(terrorIdComplaints);
	ReturnNothing;
}

/****************************************************************************
 *
 * :: With:  [C] with (a: A; ...)
 *
 ***************************************************************************/

local void
tibupWith(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupWith");
	AbSyn		base   = absyn->abWith.base;
	AbSyn		within = absyn->abWith.within;
	SymeList	bsymes, wsymes, symes = listNil(Syme);
	SymeList	mods = listNil(Syme);
	TForm		wtf, tf;

	SymbolList	fluid(terrorIdComplaints);
	terrorIdComplaints = 0;

	typeInferAs(stab, base, tfCategory);
	typeInferCheck(stab, base, tfCategory);
	bsymes = abGetCatExports(base);

	wtf = tibup0Within(stab, within, bsymes, true);
	wsymes = tfGetThdExports(wtf);

	symes = tfJoinExportLists(mods, symes, bsymes, NULL);
	symes = tfJoinExportLists(mods, symes, wsymes, NULL);

	tf = tfThird(symes);
	tfAddSelf(tf, abGetCatSelf(base));
	tfAddSelf(tf, tfGetThdSelf(wtf));

	abTPoss(absyn) = tpossSingleton(tf);

	listFree(Symbol)(terrorIdComplaints);
	ReturnNothing;
}

/****************************************************************************
 *
 * :: Where:  e where d
 *
 ***************************************************************************/

local void
tibupWhere(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn  context = absyn->abWhere.context;
	AbSyn  expr    = absyn->abWhere.expr;

	tibup(stab, context, tfUnknown);
	tibup(stab, expr,    type);

	abTPoss(absyn) = abReferTPoss(expr);
}

/****************************************************************************
 *
 * :: If:  if b then t [else e]
 *
 ***************************************************************************/

local void
tibupIf(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	test	= absyn->abIf.test;
	AbSyn	thenAlt = absyn->abIf.thenAlt;
	AbSyn	elseAlt = absyn->abIf.elseAlt;
	AbSyn	nTest;
	AbLogic	saveCond;

	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);

	/* Completely analyze the test on the spot. */
	typeInferAs(stab, test, tfBoolean);

	if (tfIsCategoryContext(type, absyn)) {
		SymeList	tsymes, esymes, symes = listNil(Syme);
		SymeList	mods = listNil(Syme);
		TForm		ttf, etf, tf;
		Bool		pending = false;

		ablogAndPush(&abCondKnown, &saveCond, test, true);
		ttf = tibup0Within(stab, thenAlt, listNil(Syme), true);
		ablogAndPop (&abCondKnown, &saveCond);

		if (!tfHasSelf(ttf)) pending = true;
		tsymes = tfGetThdExports(ttf);

		ablogAndPush(&abCondKnown, &saveCond, test, false);
		etf = tibup0Within(stab, elseAlt, listNil(Syme), true);
		ablogAndPop (&abCondKnown, &saveCond);

		if (!tfHasSelf(etf)) pending = true;
		esymes = tfGetThdExports(etf);

		symes = tfJoinExportLists(mods, symes, tsymes, test);
		test = abNewNot(sposNone, test);
		symes = tfJoinExportLists(mods, symes, esymes, test);

		tf = tfThird(symes);
		tfAddSelf(tf, tfGetThdSelf(ttf));
		tfAddSelf(tf, tfGetThdSelf(etf));

		if (pending) {
			tformFreeVars(tf);
			tfArgv(tf)[0] = tfFullFrAbSyn(stab, absyn);
		}

		abResetTPoss(absyn, tpossSingleton(tf));

		return;
	}


	/*
	 * Normalise the test condition for abCondKnown: this probably
	 * ought to be done for categories as well - see titdnIf().
	 *
	 * Note that we only use this for abCondKnown - we leave the
	 * optimiser to do constant folding to optimise the test
	 * during code generation. Thus even if abExpandDefs() is
	 * broken we still generate the right code.
	 */
	nTest = abExpandDefs(stab, test);

	if (abIsSefo(nTest)) {
		ablogAndPush(&abCondKnown, &saveCond, nTest, true); /* test, true); */
		tibup(stab, thenAlt, type);
		ablogAndPop (&abCondKnown, &saveCond);

		ablogAndPush(&abCondKnown, &saveCond, nTest, false); /* test, false); */
		tibup(stab, elseAlt, abIsNothing(elseAlt) ? tfUnknown : type);
		ablogAndPop (&abCondKnown, &saveCond);
	}
	else {
		tibup(stab, thenAlt, type);
		tibup(stab, elseAlt, abIsNothing(elseAlt) ? tfUnknown : type);
	}
	/* Analyze the branches in the presence of the condition. */

	/* No value required. */
	if (abUse(absyn) == AB_Use_NoValue)
		abResetTPoss(absyn, tpossSingleton(tfNone()));

	/* Both branches present. */
	else if (abIsNotNothing(elseAlt)) {
		TPoss	thenPoss = abReferTPoss(thenAlt);
		TPoss	elsePoss = abReferTPoss(elseAlt);

		abResetTPoss(absyn, tpossIntersect(thenPoss, elsePoss));

		tpossFree(thenPoss);
		tpossFree(elsePoss);
	}

	/* One branch present. */
	else {
		TPoss	thenPoss = abReferTPoss(thenAlt);

		if (tpossHasSatisfier(thenPoss, tfCategory))
			abResetTPoss(absyn, tpossRefer(thenPoss));
		else
			tibup0NoValue(stab, absyn, type, ALDOR_E_TinContextIf);

		tpossFree(thenPoss);
	}
}

/****************************************************************************
 *
 * :: Test:  implied test
 *
 ***************************************************************************/

AbSyn tibupSelectArgf(AbSyn ab, Length i)
{
	if (i == 0)
		return tuniBupSelectObj;
	else
		return abArgv(ab)[i - 1];
}

local void
tibupTest(Stab stab, AbSyn absyn, TForm type)
{
	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);

	if (tuniBupSelectObj != NULL) {
		tibup0ApplySym(stab, absyn, 
			       tfBoolean, 
			       ssymTheCase,
			       2,
			       tibupSelectArgf,
			       NULL);
	}
	else 
		tibup0ApplySymIfNeeded(stab, absyn, tfBoolean, ssymTheTest,
				       1, abArgf, NULL, tfIsBooleanFn);
}

/***************************************************************************
 *
 * :: Collect:	e <iter>*
 *
 ***************************************************************************/

local void
tibupCollect(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupCollect");
	AbSyn	body	  = absyn->abCollect.body;
	AbSyn	*iterv	  = absyn->abCollect.iterv;
	Length	i, iterc  = abCollectIterc(absyn);
	TPoss	cposs, bposs;
	TPossIterator tit;

	TPoss	fluid(tuniReturnTPoss);
	TPoss	fluid(tuniYieldTPoss);
	TPoss	fluid(tuniExitTPoss);
	Bool	fluid(tloopBreakCount);

	tuniReturnTPoss = tuniInappropriateTPoss;
	tuniYieldTPoss	= tuniInappropriateTPoss;
	tuniExitTPoss	= tuniInappropriateTPoss;
	tloopBreakCount = 0;

	for (i = 0; i < iterc; i++)
		tibup(stab, iterv[i], tfUnknown);

	tibup(stab, body, tfUnknown);

	bposs = abGoodTPoss(body);
	cposs = tpossEmpty();

	for (tpossITER(tit,bposs); tpossMORE(tit); tpossSTEP(tit)){
		TForm	t = tpossELT(tit);
		TForm   retType;
		SatMask result;
		tfFollow(t);
		if (tfIsMulti(t)) t = tfCrossFrMulti(t);
		retType = tfGenerator(t);
		result = tfSat(tfSatBupMask(), retType, type);
		if (tfSatSucceed(result)) {
			cposs = tpossAdd1(cposs, retType);
		}
	}
	abTPoss(absyn) = cposs;

	ReturnNothing;
}

/****************************************************************************
 *
 * :: Repeat:  <iter>* repeat e
 *
 ***************************************************************************/

local void
tibupRepeat(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupRepeat");

	Bool	fluid(tloopBreakCount);

	AbSyn	body	  = absyn->abRepeat.body;
	AbSyn	*iterv	  = absyn->abRepeat.iterv;
	Length	i, iterc  = abRepeatIterc(absyn);

	tloopBreakCount = 0;

	for (i = 0; i < iterc; i++) 
		tibup(stab, iterv[i], tfUnknown);

	tibup(stab, body, tfUnknown);

	if (iterc == 0 && tloopBreakCount == 0)
		abTPoss(absyn) = tpossSingleton(tfExit);
	else
		tibup0NoValue(stab, absyn, type, ALDOR_E_TinContextRepeat);

	ReturnNothing;
}

/***************************************************************************
 *
 * :: Never
 * X
 ***************************************************************************/

local void
tibupNever(Stab stab, AbSyn absyn, TForm type)
{
	abTPoss(absyn) = tpossSingleton(tfExit);
}

/***************************************************************************
 *
 * :: Iterate
 * X
 ***************************************************************************/

local void
tibupIterate(Stab stab, AbSyn absyn, TForm type)
{
	abTPoss(absyn) = tpossSingleton(tfExit);
}

/***************************************************************************
 *
 * :: Break
 * X
 ***************************************************************************/

local void
tibupBreak(Stab stab, AbSyn absyn, TForm type)
{
	if (tloopBreakCount == -1) {
		abState(absyn) = AB_State_Error;
		abTPoss(absyn) = tpossEmpty();
	}
	else {
		tloopBreakCount++;
		abTPoss(absyn) = tpossSingleton(tfExit);
	}
}

/****************************************************************************
 *
 * :: While:  while c
 *
 ***************************************************************************/

local void
tibupWhile(Stab stab, AbSyn absyn, TForm type)
{
	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);
	tibup0Generic(stab, absyn, tfBoolean);
}

/***************************************************************************
 *
 * :: For:  for x in l | c
 *
 ***************************************************************************/

local void
tibupFor(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	lhs   = absyn->abFor.lhs;
	AbSyn	whole = absyn->abFor.whole;
	AbSyn	test  = absyn->abFor.test;
	TPoss	tparg, tplhs, tp;
	Stab	ostab = stab;

	/*
	 * Subtle note: the generator in a for-iterator lies
	 * outside the scope level of the repeat. This means
	 * that we have to use cdr(stab) whenever we tinfer
	 * absyn->abFor.whole or via abForIterArgf().
	 */
	stab = cdr(stab);
	tibup(stab, whole, tfUnknown);
	tibup0ApplySymIfNeeded(stab, absyn, tfUnknown, ssymTheGenerator,
			       1, abForIterArgf, NULL, tfIsGeneratorFn);
	stab = ostab;


	/*
	 * The for-variable and test lie within the scope
	 * of the repeat clause.
	 */
	tparg = tpossGeneratorArg(abGoodTPoss(absyn));
	tibup0InferLhs(stab, absyn, lhs, test, tparg); /* !! test */
	tibup(stab, lhs,  tfUnknown);

	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (!abIsNothing(test) && tfBoolean == tfUnknown)
		comsgFatal(absyn, ALDOR_F_BugNoBoolean);
	tibup(stab, test, abIsNothing(test) ? tfUnknown : tfBoolean);

	lhs = abTag(absyn->abFor.lhs) == AB_Comma ? lhs : abDefineeId(lhs);

	tplhs = abReferTPoss(lhs);
	tp = tpossSatisfies(tparg, tplhs);
	abResetTPoss(lhs, tp);
	tpossFree(tplhs);

	if (tpossCount(tparg) > 0 && tpossCount(tp) != 1)
		abState(absyn) = AB_State_Error;

	if (abState(absyn) == AB_State_Error) {
		tpossFree(tparg);
		return;
	}

	if (tpossIsUnique(tp) && !tpossIsUnique(abTPoss(absyn))) {
		TForm tf = tpossUnique(tp);
		tpossFree(abTPoss(absyn));
		abTPoss(absyn) = tpossSingleton(tfGenerator(tf));
	}

	/* Avoid two error messages */
	if (abState(whole) == AB_State_Error &&
	    abState(lhs) == AB_State_Error) {
		abState(lhs) = AB_State_HasPoss;
		abTPoss(lhs) = tpossEmpty();
	}

	tpossFree(tparg);
}

/****************************************************************************
 *
 * :: Foreign:	import ... from Foreign(...)
 *
 ***************************************************************************/

local void
tibupForeignImport(Stab stab, AbSyn absyn, TForm type)
{
	tibup(stab, absyn->abForeignImport.what, tfUnknown);
	abTPoss(absyn) = tpossSingleton(tfNone());
}

/****************************************************************************
 *
 * :: Foreign:	import ... from Foreign(...)
 *
 ***************************************************************************/

local void
tibupForeignExport(Stab stab, AbSyn absyn, TForm type)
{
	tibup(stab, absyn->abForeignExport.what, tfUnknown);
	abTPoss(absyn) = tpossSingleton(tfNone());
}

/****************************************************************************
 *
 * :: Import:  import ... from D
 *
 ***************************************************************************/

local void
tibupImport(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	what = absyn->abImport.what;
	AbSyn	from = absyn->abImport.origin;

	tiGetTForm(stab, from);
	typeInferCheck(stab, from, tfDomain);

	tibup0Within(stab, what, listNil(Syme), true);

	abTPoss(absyn) = tpossSingleton(tfNone());
}

/****************************************************************************
 *
 * :: Inline:  inline .. from D
 *
 ***************************************************************************/

local void
tibupInline(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	what = absyn->abInline.what;
	AbSyn	from = absyn->abInline.origin;

	tiGetTForm(stab, from);
	typeInferCheck(stab, from, tfDomain);

	tibup0Within(stab, what, listNil(Syme), true);

	abTPoss(absyn) = tpossSingleton(tfNone());
}

/****************************************************************************
 *
 * :: Qualify:	 A $ B
 *
 * ToDo: 3$Integer
 *
 ***************************************************************************/

local void
tibupQualify(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn		origin = absyn->abQualify.origin;
	AbSyn		what   = absyn->abQualify.what;
	Symbol		sym    = what->abId.sym;

	Syme		syme = 0;
	SymeList	symes, msymes, fsymes;
	TPoss		tposs = tpossEmpty();
	TForm		tforg;

	assert(abTag(what) == AB_Id);
	tforg = tiGetTForm(stab, origin);
	typeInferCheck(stab, origin, tfDomain);

	if (tfIsForeign(tforg)) {
		msymes = stabGetMeanings(stab, abCondKnown, what->abId.sym);
		symes  = listNil(Syme);
		for ( ; msymes; msymes = cdr(msymes))
			if (symeId(car(msymes)) == sym 
			    && symeIsForeign(car(msymes)))
				symes = listCons(Syme)(car(msymes), symes);
		fsymes = symes;
	}
	else if (tfIsSelf(tforg)) {
		/*symes  = tfGetDomImports(tforg);*/
		symes = listNil(Syme);
		fsymes = listNil(Syme);

		if (symes == listNil(Syme)) {
			msymes = stabGetMeanings(stab, abCondKnown, what->abId.sym);
			for ( ; msymes; msymes = cdr(msymes))
				if (symeId(car(msymes)) == sym && symeIsExport(car(msymes)))
					symes = listCons(Syme)(car(msymes),
							       symes);
			fsymes = symes;
		}
	}
	else {
		symes  = listNil(Syme);
		msymes = tfGetDomImportsByName(tforg, sym);
		for ( ; msymes; msymes = cdr(msymes)) {
			Syme syme = car(msymes);
			if (sym != symeId(syme))
				continue;
			if (ablogIsListKnown(symeCondition(syme)))
				symes = listCons(Syme)(syme, symes);
		}
		fsymes = symes;
	}

	for (; symes; symes = cdr(symes)) {
		syme = car(symes);
		if (tfSatReturn(symeType(syme), type))
			tposs = tpossAdd1(tposs, symeType(syme));
	}
	abTPoss(absyn) = tposs;
	if (tpossCount(tposs) != 0) {
		abState(what) = AB_State_HasPoss;
		abTPoss(what) = tpossRefer(tposs);
	}
	listFree(Syme)(fsymes);
}

/***************************************************************************
 *
 * :: CoerceTo:	 x :: T
 *
 ***************************************************************************/

local void
tibupCoerceTo(Stab stab, AbSyn absyn, TForm type)
{
	TForm tf = tiGetTForm(stab, absyn->abCoerceTo.type);
	tibup0ApplySym(stab, absyn, tf, ssymCoerce, 1, abArgf, NULL);
}

/****************************************************************************
 *
 * :: RestrictTo:   A @ B
 *
 ***************************************************************************/

local void
tibupRestrictTo(Stab stab, AbSyn absyn, TForm type)
{
	TForm tf = tiGetTForm(stab, absyn->abRestrictTo.type);

	tibup(stab, absyn->abRestrictTo.expr, tf);

	abTPoss(absyn) = tpossSingleton(tf);
}

/****************************************************************************
 *
 * :: PretendTo:   A pretend B
 *
 ***************************************************************************/

local void
tibupPretendTo(Stab stab, AbSyn absyn, TForm type)
{
	TForm tf = tiGetTForm(stab, absyn->abPretendTo.type);

	tibup(stab, absyn->abPretendTo.expr, tfUnknown);
	abTPoss(absyn) = tpossSingleton(tf);
}

/***************************************************************************
 *
 * :: Not: not a
 *
 ***************************************************************************/

local void
tibupNot(Stab stab, AbSyn absyn, TForm type)
{
	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);
	tibup(stab, absyn->abNot.expr, tfBoolean);
	abTPoss(absyn) = tpossSingleton(tfBoolean);
}

/***************************************************************************
 *
 * :: And: a and b and c ...
 *
 ***************************************************************************/

local void
tibupAnd(Stab stab, AbSyn absyn, TForm type)
{
	int	i;
	int	argc = abArgc(absyn);
	AbLogic *saveCond = (AbLogic*) stoAlloc(OB_Other, sizeof(AbLogic) * argc);
	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);

	for (i = 0; i < argc; i++) {
		tibup(stab, abArgv(absyn)[i], tfBoolean);
		ablogAndPush(&abCondKnown, &saveCond[i], abArgv(absyn)[i], true);
	}
	for (i = 0; i < argc; i++) {
		ablogAndPop(&abCondKnown, &saveCond[argc-i-1]);
	}

	abTPoss(absyn) = tpossSingleton(tfBoolean);

	stoFree(saveCond);
}

/***************************************************************************
 *
 * :: Or: a or b or c ...
 *
 ***************************************************************************/

local void
tibupOr(Stab stab, AbSyn absyn, TForm type)
{
	int	i;
	int	argc = abArgc(absyn);

	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);
	for (i = 0; i < argc; i++)
		tibup(stab, abArgv(absyn)[i], tfBoolean);

	abTPoss(absyn) = tpossSingleton(tfBoolean);
}

/***************************************************************************
 *
 * :: Assert:
 *
 ***************************************************************************/

local void
tibupAssert(Stab stab, AbSyn absyn, TForm type)
{
	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);
	tibup(stab, absyn->abAssert.test, tfBoolean);
	abTPoss(absyn) = tpossSingleton(tfNone());
}

/***************************************************************************
 *
 * :: Blank:
 *
 ***************************************************************************/

local void
tibupBlank(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Builtin:
 *
 ***************************************************************************/

local void
tibupBuiltin(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Default:
 *
 ***************************************************************************/

local void
tibupDefault(Stab stab, AbSyn absyn, TForm type)
{
	SymeList	sl;
	AbSyn		body;
	TForm		tf;

	body = absyn->abDefault.body;
	sl = tibup0DefaultBody(stab, body, tiTfDoDefault(absyn));
	tf = tfThird(sl);
	tfHasSelf(tf) = true;
	abTPoss(absyn) = tpossSingleton(tf);
}

/***************************************************************************
 *
 * :: Delay:
 *
 ***************************************************************************/

local void
tibupDelay(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Do:
 *
 ***************************************************************************/

local void
tibupDo(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, tfUnknown);
	abTPoss(absyn) = tpossSingleton(tfNone());
}

/***************************************************************************
 *
 * :: Except:
 *
 ***************************************************************************/

local void
tibupExcept(Stab stab, AbSyn absyn, TForm type)
{
	tibup(stab, absyn->abExcept.except, tfTuple(tfCategory));
	tibup(stab, absyn->abExcept.type, type);
	abTPoss(absyn) = abReferTPoss(absyn->abExcept.type);
}

/***************************************************************************
 *
 * :: Raise:
 *
 ***************************************************************************/

local void
tibupRaise(Stab stab, AbSyn absyn, TForm type)
{
	/* We can't check this, because we need a type form
	 * on the RHS
	 */
	tibup(stab, absyn->abRaise.expr, tfDomain);
	abTPoss(absyn) = tpossSingleton(tfExit);
}

/***************************************************************************
 *
 * :: Export:
 *
 ***************************************************************************/

local void
tibupExport(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	what = absyn->abExport.what;
	AbSyn	from = absyn->abExport.origin;
	AbSyn	dest = absyn->abExport.destination;

	if (!abIsNothing(dest))
		tiGetTForm(stab, dest);

	if (!abIsNothing(from)) {
		tiGetTForm(stab, from);
		typeInferCheck(stab, from, tfDomain);
	}

	if (abIsNothing(from) && abIsNothing(dest))
		tibup(stab, what, tfUnknown);
	else
		tibup0Within(stab, what, listNil(Syme), true);

	abTPoss(absyn) = tpossSingleton(tfNone());
}

/***************************************************************************
 *
 * :: Extend:
 *
 ***************************************************************************/

local void
tibupExtend(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Fix:
 *
 ***************************************************************************/

local void
tibupFix(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Fluid:
 *
 ***************************************************************************/

local void
tibupFluid(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Free:
 *
 ***************************************************************************/

local void
tibupFree(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Has:
 *
 ***************************************************************************/

local void
tibupHas(Stab stab, AbSyn absyn, TForm type)
{
	AbSyn	expr = absyn->abHas.expr;
	AbSyn	prop = absyn->abHas.property;

	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	tiGetTFormContext(stab, abCondKnown, expr);
	tiGetTFormContext(stab, abCondKnown, prop);
	if (tfBoolean == tfUnknown) comsgFatal(absyn, ALDOR_F_BugNoBoolean);
	tibup0Generic(stab, absyn, tfBoolean);

	typeInferCheck(stab, expr, tfDomain);
	typeInferCheck(stab, prop, tfCategory);
}

/***************************************************************************
 *
 * :: Hide:
 *
 ***************************************************************************/

local void
tibupHide(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: IdSy:
 *
 ***************************************************************************/

local void
tibupIdSy(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Let:
 *
 ***************************************************************************/

local void
tibupLet(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Local:
 *
 ***************************************************************************/

local void
tibupLocal(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Macro:
 *
 ***************************************************************************/

local void
tibupMacro(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: MLambda:
 *
 ***************************************************************************/

local void
tibupMLambda(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Nothing:
 *
 ***************************************************************************/

local void
tibupNothing(Stab stab, AbSyn absyn, TForm type)
{
	tibup0NoValue(stab, absyn, type, ALDOR_E_TinContextSeq);
}

/***************************************************************************
 *
 * :: Quote:
 *
 ***************************************************************************/

local void
tibupQuote(Stab stab, AbSyn absyn, TForm type)
{
	tibup0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Select:
 *
 ***************************************************************************/

local void
tibupSelect(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tibupSelect");
	TPoss   fluid(tuniSelectTPoss);
	AbSyn   fluid(tuniBupSelectObj);
	TPoss	fluid(tuniExitTPoss);
	TForm	fluid(tuniExitType);
	TPoss tp;
	AbSyn seq;
	tuniSelectTPoss  = NULL;
	tuniBupSelectObj = NULL;

	tibup(stab, absyn->abSelect.testPart, tfUnknown);
	
	tuniSelectTPoss  = abTPoss(absyn->abSelect.testPart);
	tuniBupSelectObj = absyn->abSelect.testPart;

	if (abUse(absyn) == AB_Use_NoValue || tfIsNone(type))
		tuniExitTPoss = tuniNoValueTPoss;
	else
		tuniExitTPoss = tuniUnknownTPoss;

	tuniExitType = type;

	seq = absyn->abSelect.alternatives;
	tibupSequence0(stab, seq, type);
	
	
	if (abState(seq) == AB_State_Error) 
	    	tp = tpossEmpty();
	else {
		abState(seq) = AB_State_HasPoss;
		tp = abReferTPoss(seq);
	}
	    
	/*
	 * tp is under constrained, as the alternatives
	 * may narrow the options. wtf.
	 */
	abTPoss(absyn) = tp;
	ReturnNothing;
}

/***************************************************************************
 *
 * :: Try:
 *
 ***************************************************************************/

local void
tibupTry(Stab stab, AbSyn absyn, TForm type)
{
	tibup(stab, absyn->abTry.id, tfUnknown);
	/* 
	 * Strictly, given
	 * expr: Bar except (A, B, C)
	 * and handler covers (X, Y, Z)
	 * then we should:
	 * check expr with:
	 *     Bar except (A, B, C, X, Y, Z)
	 * check handler with:
	 *     Bar except (A, B, C)
	 * The hard part is grabbing (X, Y, Z)
	 */
	tibup(stab, absyn->abTry.expr, tfIgnoreExceptions(type));
	tibup(stab, absyn->abTry.always, tfNone());

	if (!abIsNothing(absyn->abTry.except))
		tibup(stab, absyn->abTry.except, type);

	if (abUse(absyn) == AB_Use_NoValue || tfIsNone(type))
		abResetTPoss(absyn, tpossSingleton(tfNone()));
	else 
		abTPoss(absyn) = abReferTPoss(absyn->abTry.expr);
	
}

/*****************************************************************************
 *
 * :: Cache for computed results (currently disabled)
 *
 ****************************************************************************/

typedef struct {
	Stab    stab;
	AbSyn   ab;
	TForm   tf;
	AbLogic known;
} *ArgSet, _ArgSet;

local Hash tibupCacheHash	(ArgSet);
local Bool tibupCacheEq 	(ArgSet, ArgSet);
#if 0 /* Seemingly unused */
	local void tibupCacheFreeElt(ArgSet set);
#endif
local Table tuniCache;
void
tibupCacheAdd(Stab stab, AbSyn ab, TForm tf, AbLogic known, TPoss poss)
{
	ArgSet set = (ArgSet) stoAlloc(OB_Other, sizeof(*set));

	set->stab  = stab;
	set->ab    = ab;
	set->tf    = tf;
	set->known = ablogCopy(known);

	tpossRefer(poss);
	if (tuniCache == NULL) {
		tuniCache = tblNew((TblHashFun) tibupCacheHash, 
				   (TblEqFun) tibupCacheEq);
	}

	tblSetElt(tuniCache, (TblKey) set, (TblElt) poss);
}

TPoss
tibupCacheLookup(Stab stab, AbSyn ab, TForm tf, AbLogic known)
{
	_ArgSet myset;
	TPoss   res;

	if (tuniCache == NULL) return NULL;

	myset.stab = stab;
	myset.ab   = ab;
	myset.tf   = tf;
	myset.known = known;
	res = tblElt(tuniCache, (TblKey) &myset, NULL);

	if (res != NULL) {
		static int c;
		printf("Got %d\n", c++);
		tpossRefer(res);
	}
	return res;
}

/*
 * !! This function does not appear to be used anymore !!
 */
#if 0
local void
tibupCacheFlush(Stab stab)
{
	/* Iterate over the cache destroying all instances of `stab' in the cache. */
	/* 
	 * Not sure this is the best way, maybe using a `depth' index or something
	 * would be best
	 */

	TableIterator it;
	if (tuniCache == NULL) return;

	for (tblITER(it, tuniCache); tblMORE(it); tblSTEP(it)) {
		ArgSet k = (ArgSet) tblKEY(it);
		TPoss  r = (TPoss)  tblELT(it);
		tpossFree(r);
		tibupCacheFreeElt(k);
	}
}
#endif


local Bool
tibupCacheEq(ArgSet a, ArgSet b)
{
	if (a->stab != b->stab) return false;
	if (a->tf   != b->tf)   return false;
	if (abTContext(a->ab) != abTContext(b->ab)) return false;
	if (!abEqual(a->ab, b->ab))   return false;
	if (!ablogEqual(a->known, b->known)) return false;
	return true;
}


local Hash
tibupCacheHash(ArgSet set)
{
	return abHash(set->ab) ^ tfHash(set->tf);
}

/*
 * !! This function does not appear to be used anymore !!
 */
#if 0
local void
tibupCacheFreeElt(ArgSet set)
{
	ablogFree(set->known);
	stoFree(set);
}
#endif

