/****************************************************************************
 *
 * ti_tdn.c: Type inference -- top down pass.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ***************************************************************************/

#include "ablogic.h"
#include "abpretty.h"
#include "comsg.h"
#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "lib.h"
#include "sefo.h"
#include "simpl.h"
#include "spesym.h"
#include "store.h"
#include "stab.h"
#include "terror.h"
#include "tfcontext.h"
#include "tfsat.h"
#include "tfknown.h"
#include "ti_tdn.h"
#include "tinfer.h"
#include "tposs.h"
#include "util.h"


/*
 * To do:
 * -- titdnApply: Should mark op as erroneous, if necessary.
 */

/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

Bool	condApplyDebug	= false;
Bool	tipTdnDebug	= false;
#define condApplyDEBUG	DEBUG_IF(condApply)	afprintf
#define tipTdnDEBUG	DEBUG_IF(tipTdn)	afprintf

/*****************************************************************************
 *
 * :: Fluids to unify multiple exit points
 *
 ****************************************************************************/

TForm		tuniYieldTForm		= 0;
TForm		tuniReturnTForm		= 0;
TForm		tuniExitTForm		= 0;
static AbSyn	tuniTdnSelectObj	= 0;

/*****************************************************************************
 *
 * :: Fluids to keep trace of multiple exit points
 *
 ****************************************************************************/

AbSynList abExitsList = 0;
AbSynList abReturnsList = 0;
AbSynList abYieldsList = 0;

/*****************************************************************************
 *
 * :: Declarations for top down pass
 *
 ****************************************************************************/

local Bool	titdn0Generic	(Stab, AbSyn, TFContext);
local Bool	titdn0FarValue	(Stab, AbSyn, TFContext, AbSyn,TForm *,
				 AbSynList *);
local Bool	titdn0NoValue	(Stab, AbSyn, TFContext, Msg);

local Bool	titdn0ApplySymIfNeeded
				(Stab, AbSyn, TFContext, Symbol,
				 Length, AbSynGetter, AbSyn, TFormPredicate);
local Bool	titdn0ApplySym	(Stab, AbSyn, TFContext, Symbol,
				 Length, AbSynGetter, AbSyn);
local Bool	titdn0ApplyFType(Stab, AbSyn, TFContext, AbSyn,
				 Length, AbSynGetter);
local Bool	titdn0ApplyJoin (Stab, AbSyn, TFContext, AbSyn,
				 Length, AbSynGetter);

local Bool	titdn0PLambdaArgs	(Stab, AbSyn);
local Bool	titdn0PLambdaRets	(Stab, AbSynList);

local Bool	titdnId		(Stab, AbSyn, TFContext);
local Bool	titdn0IdCondition(AbSyn, Syme);
local Bool	titdnIdSy	(Stab, AbSyn, TFContext);
local Bool	titdnBlank	(Stab, AbSyn, TFContext);
local Bool	titdnLitInteger (Stab, AbSyn, TFContext);
local Bool	titdnLitFloat	(Stab, AbSyn, TFContext);
local Bool	titdnLitString	(Stab, AbSyn, TFContext);
local Bool	titdnAdd	(Stab, AbSyn, TFContext);
local Bool	titdnAnd	(Stab, AbSyn, TFContext);
local Bool	titdnApply	(Stab, AbSyn, TFContext);
local Bool	titdnAssert	(Stab, AbSyn, TFContext);
local Bool	titdnAssign	(Stab, AbSyn, TFContext);
local Bool	titdnBreak	(Stab, AbSyn, TFContext);
local Bool	titdnBuiltin	(Stab, AbSyn, TFContext);
local Bool	titdnCoerceTo	(Stab, AbSyn, TFContext);
local Bool	titdnCollect	(Stab, AbSyn, TFContext);
local Bool	titdnComma	(Stab, AbSyn, TFContext);
local Bool	titdnDeclare	(Stab, AbSyn, TFContext);
local Bool	titdnDefault	(Stab, AbSyn, TFContext);
local Bool	titdnDefine	(Stab, AbSyn, TFContext);
local Bool	titdnDelay	(Stab, AbSyn, TFContext);
local Bool	titdnDo		(Stab, AbSyn, TFContext);
local Bool	titdnExcept	(Stab, AbSyn, TFContext);
local Bool	titdnRaise	(Stab, AbSyn, TFContext);
local Bool	titdnExit	(Stab, AbSyn, TFContext);
local Bool	titdnExport	(Stab, AbSyn, TFContext);
local Bool	titdnExtend	(Stab, AbSyn, TFContext);
local Bool	titdnFix	(Stab, AbSyn, TFContext);
local Bool	titdnFluid	(Stab, AbSyn, TFContext);
local Bool	titdnFor	(Stab, AbSyn, TFContext);
local Bool	titdnForeignImport(Stab, AbSyn, TFContext);
local Bool	titdnForeignExport(Stab, AbSyn, TFContext);
local Bool	titdnFree	(Stab, AbSyn, TFContext);
local Bool	titdnGenerate	(Stab, AbSyn, TFContext);
local Bool	titdnGoto	(Stab, AbSyn, TFContext);
local Bool	titdnHas	(Stab, AbSyn, TFContext);
local Bool	titdnHide	(Stab, AbSyn, TFContext);
local Bool	titdnIf		(Stab, AbSyn, TFContext);
local Bool	titdnImport	(Stab, AbSyn, TFContext);
local Bool	titdnInline	(Stab, AbSyn, TFContext);
local Bool	titdnIterate	(Stab, AbSyn, TFContext);
local Bool	titdnLabel	(Stab, AbSyn, TFContext);
local Bool	titdnLambda	(Stab, AbSyn, TFContext);
local Bool	titdnLet	(Stab, AbSyn, TFContext);
local Bool	titdnLocal	(Stab, AbSyn, TFContext);
local Bool	titdnMacro	(Stab, AbSyn, TFContext);
local Bool	titdnMLambda    (Stab, AbSyn, TFContext);
local Bool	titdnNever	(Stab, AbSyn, TFContext);
local Bool	titdnNot	(Stab, AbSyn, TFContext);
local Bool	titdnNothing	(Stab, AbSyn, TFContext);
local Bool	titdnOr		(Stab, AbSyn, TFContext);
local Bool	titdnPretendTo	(Stab, AbSyn, TFContext);
local Bool	titdnQualify	(Stab, AbSyn, TFContext);
local Bool	titdnQuote	(Stab, AbSyn, TFContext);
local Bool	titdnReference	(Stab, AbSyn, TFContext);
local Bool	titdnRepeat	(Stab, AbSyn, TFContext);
local Bool	titdnRestrictTo (Stab, AbSyn, TFContext);
local Bool	titdnReturn	(Stab, AbSyn, TFContext);
local Bool	titdnSelect	(Stab, AbSyn, TFContext);
local Bool	titdnSequence	(Stab, AbSyn, TFContext);
local Bool	titdnSelect	(Stab, AbSyn, TFContext);
local Bool	titdnTest	(Stab, AbSyn, TFContext);
local Bool	titdnTry	(Stab, AbSyn, TFContext);
local Bool	titdnWhere	(Stab, AbSyn, TFContext);
local Bool	titdnWhile	(Stab, AbSyn, TFContext);
local Bool	titdnWith	(Stab, AbSyn, TFContext);
local Bool	titdnYield	(Stab, AbSyn, TFContext);

local void 	titdnError(Stab stab, AbSyn absyn, TFContext type);
local void 	titdn0Error(Stab stab, AbSyn absyn, TFContext type);

local Bool	titdnSequence0	(Stab, AbSyn, TFContext);

/*****************************************************************************
 *
 * :: Top down pass
 *
 ****************************************************************************/

void
tiTopDown(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tiTopDown");

	TForm	fluid(tuniReturnTForm);
	TForm	fluid(tuniYieldTForm);
	TForm	fluid(tuniExitTForm);
	AbLogic fluid(abCondKnown);

	tuniReturnTForm	= tfNone();
	tuniYieldTForm	= tfNone();
	tuniExitTForm	= tfNone();
	abCondKnown     = abCondKnown ? ablogCopy(abCondKnown) : ablogTrue();

	titdn(stab, absyn, ctxtEmpty(type));

	ablogFree(abCondKnown);

	ReturnNothing; /* (result); */
}

Bool
titdn(Stab stab, AbSyn absyn, TFContext tfc)
{
	TPoss		abtposs;
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;
	Bool		s = false;
	TForm		stype;
	TForm 		type;

	type = ctxtTForm(tfc);
	
	if (abState(absyn) == AB_State_HasUnique)
		return true;

	if (abState(absyn) == AB_State_Error) {
		titdnError(stab, absyn, tfc);
		return false;
	}

	assert(abState(absyn) == AB_State_HasPoss);
	/* MUST use tpossRefer() or abReferTPoss() */
	abtposs	 = tpossRefer(abTPoss(absyn));

	if (tpossIsEmpty(abtposs)) {
		titdnError(stab, absyn, tfc);
		return false;
	}

	stype = tfFollowSubst(type);

	if (tfIsUnknown(stype) || tfIsNone(stype)) {
		if (!tpossIsUnique(abtposs)) {
			terrorNotUniqueType(ALDOR_E_TinExprMeans,
					    absyn,type,abtposs);
			return false;
		}
		if (tpossIsUnique(abtposs))
			tfc = ctxtCopy(tfc, tpossUnique(abtposs));
	}

	if (!abIsLeaf(absyn) && abStab(absyn))
		stab = abStab(absyn);

	serialNo += 1;
	depthNo	 += 1;
	serialThis = serialNo;
	if (DEBUG(tipTdn)) {
		fprintf(dbOut,"->Tdn: %*s%d= ", depthNo, "", serialThis);
		abPrettyPrint(dbOut, absyn);
		fprintf(dbOut," @ ");
		tfPrint(dbOut, type);
		fnewline(dbOut);
	}

	AB_SWITCH(absyn, s = titdn, (stab, absyn, tfc));

	if (s) {
		/* The callee should have set abTUnique(absyn). */
		assert(abTPoss(absyn) != abtposs);

		/* All identifiers should have symes. */
		assert(abTag(absyn) != AB_Id || abSyme(absyn));

		abState(absyn) = AB_State_HasUnique;
		tpossFree(abtposs);
	}
	else {
		/* The callee should not have changed the abState. */
		assert(abState(absyn) == AB_State_HasPoss);

		/* The caller should not have set abTUnique(absyn). */
		assert(abTPoss(absyn) == abtposs);
	}

	if (DEBUG(tipTdn)) {
		fprintf(dbOut, "<-Tdn: %*s%d= ", depthNo, "", serialThis);
		abPrettyPrint(dbOut, absyn);
		fprintf(dbOut, " @ ");
		if (abState(absyn) == AB_State_HasUnique)
			tfPrint(dbOut, abTUnique(absyn));
		else
			tfPrint(dbOut, type);
		fnewline(dbOut);
	}
	depthNo -= 1;
	return s;
}

/****************************************************************************
 *
 * :: Generic:	abArgc(ab), abArgv(ab)
 *
 ***************************************************************************/

local Bool
titdn0Generic(Stab stab, AbSyn absyn, TFContext tfc)
{
	Length i;

	if (!abIsLeaf(absyn))
		for (i = 0; i < abArgc(absyn); i += 1)
			titdn(stab, abArgv(absyn)[i], ctxtCopy(tfc, tfUnknown));

	abTUnique(absyn) = tfFollowFn(ctxtTForm(tfc));
	return true;
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

local Bool
titdn0ApplySymIfNeeded(Stab stab, AbSyn absyn, TFContext tfc, Symbol fsym,
		Length argc, AbSynGetter argf,
		AbSyn implicitPart, TFormPredicate pred)
{
	AbSyn	part;
	TPoss	tp;

	assert(argc == 1);

	part = argf(absyn, int0);
	tp   = abReferTPoss(part);

	if (tpossIsHaving(tp, pred) || tpossIsEmpty(tp))
		titdn(stab, part, tfc);
	else
		return titdn0ApplySym(stab, absyn, tfc, fsym, argc, argf,
				      implicitPart);

	tpossFree(tp);
	return true;
}

local Bool
titdn0ApplySym(Stab stab, AbSyn absyn, TFContext tfc,
	       Symbol fsym, Length argc, AbSynGetter argf,
	       AbSyn  implicitPart)
{
	AbSyn	implicitOp;

	if (!implicitPart) implicitPart = absyn;

	/* Get the implicit operation */
	implicitOp = abImplicit(implicitPart);
	assert(implicitOp);

	return titdn0ApplyFType(stab, absyn, tfc, implicitOp, argc, argf);
}

local Bool
titdn0ApplyJoin(Stab stab, AbSyn absyn, TFContext tfc,
		 AbSyn op, Length argc, AbSynGetter argf)
{
	TPoss	abtposs = abTPoss(absyn);
	TForm	opType, retType;

	opType = tpossUnique(abTPoss(op));
	titdn(stab, op, ctxtCopy(tfc, opType));
	assert(abState(op) == AB_State_HasUnique);
	abAddTContext(op, tfMapMultiArgEmbed(opType, argc));

	if (!tpossIsUnique(abtposs))
		return false;

	retType = tpossUnique(abtposs);
	if (!tfSatValues(retType, ctxtTForm(tfc)))
		return false;

	abTUnique(absyn) = retType;
	return true;
}

/*
 * Filter the operations based on arg and ret types.
 * titdn is applied to the virtual arguments.
 */

local Bool
titdn0ApplyFType(Stab stab, AbSyn absyn, TFContext tfc, AbSyn op,
		 Length argc, AbSynGetter argf)
{
	SatMask		mask = tfSatBupMask();
	Length		nopc, popc, parmc;
	TForm		nopt, popt, opType;
	TPoss		opTypes, nopTypes;
	TPossIterator	it;
	Bool		result;

	opTypes = abTPoss(op);
	if (abIsTheId(op, ssymJoin) && tpossIsUnique(opTypes) &&
	    tfSatisfies(tfMapRet(tpossUnique(opTypes)), tfCategory))
		return titdn0ApplyJoin(stab, absyn, tfc, op, argc, argf);

	opTypes  = abReferTPoss(op);	/* Original list of possible types */
	nopTypes = tpossEmpty();	/* Possible (non-pending) types */
	nopc = 0;			/* Number of non-pending matches */
	popc = 0;			/* Number of all possible matches */
	nopt = tfUnknown;		/* Non-pending op type */
	popt = tfUnknown;		/* Any possible op type */
	opType = NULL;

	/* Filter opTypes based on the argument and return types. */
	for (tpossITER(it, opTypes); tpossMORE(it); tpossSTEP(it)) {
		TForm	opType = tpossELT(it);
		SatMask	result;

		opType = tfDefineeType(opType);
		assert(tfIsAnyMap(opType));

		result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, argc, argf);
		if (tfSatSucceed(result)) {
			if (!tfSatPending(result)) {
				nopc += 1;
				nopt = opType;
				nopTypes = tpossAdd1(nopTypes, opType);
			}
			popc += 1;
			popt = opType;
		}
	}

	if (popc == 1) {
		opType = popt;
		result = true;
	}
	else if (nopc == 1) {
		opType = nopt;
		result = true;
	}
	else if (nopc == 0 && popc > 0) {
		terrorApplyNotAnalyzed(absyn, op, popt);
		result = false;
	}
	else {
   		terrorApplyFType(absyn, ctxtTForm(tfc), nopTypes, op, stab, argc, argf);
		result = false;
	}

	tpossFree(opTypes);
	tpossFree(nopTypes);

	if (!result) return false;

	if (abIsTheId(op, ssymTheCase)
	    && argc == 2
	    && tfIsPatternCaseArg(tfMapArgN(opType, 1))) {
		// Leave as is.  Ideally we would have a 'case' builtin on Pattern
		abAddTContext(absyn, AB_Embed_ApplyCase);
		// "Magic" operator from (X, X) -> Bool that matches on the way
		abTUnique(absyn->abApply.op) = opType; 
		abAddTContext(absyn, AB_Embed_ApplyCase);
		
		mask = tfSatTdnMask();
		result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, abApplyArgc(absyn), abApplyArgf);
	}
	else {
		titdn(stab, op, ctxtCopy(tfc, opType));
		parmc = tfMapHasDefaults(opType) ? tfMapArgc(opType) : argc;
		AbEmbed embed = tfMapMultiArgEmbed(opType, parmc);
		abAddTContext(op, embed);

		mask = tfSatTdnMask();
		result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, argc, argf);
	}

	/* We return false rarely (eg titdn0FarValue failure). */
	return tfSatSucceed(result);
}

/****************************************************************************
 *
 * :: Far Values:  return x, yield x, a => x
 *
 ***************************************************************************/

local Bool
titdn0FarValue(Stab stab, AbSyn absyn, TFContext tfc, AbSyn farValue, TForm *pFarType,
	       AbSynList *pFarAbSynList)
{
	AbEmbed embed;
	if (DEBUG(tipFar)) {
		fprintf(dbOut, "Computing far value as a ");
		tfPrint(dbOut, *pFarType);
		fnewline(dbOut);
	}
	titdn(stab, farValue, ctxtCopy(tfc, *pFarType));

	if (abState(farValue) != AB_State_HasUnique)
	  	return false;

	/*
	 * Check for `return value' in contexts where no
	 * value is expected and for `return'  in contexts
	 * where a value _is_ expected.
	 */
	if (abTag(absyn) == AB_Return)
	{
		Bool	abnone = (abTag(farValue) == AB_Nothing);
		Bool	tfnone = tfIsNone(*pFarType);

		if (abnone && !tfnone)
			comsgError(absyn, ALDOR_E_TinReturnNoVal);
		else if (!abnone && tfnone)
			comsgWarning(absyn, ALDOR_W_TinNoValReturn);
	}

	*pFarAbSynList = listCons(AbSyn)(farValue, *pFarAbSynList);
	if (tfIsUnknown(*pFarType)) {
		*pFarType = abTUnique(farValue);
		if (DEBUG(tipFar)) {
			fprintf(dbOut, "Converting far value to a ");
			tfPrint(dbOut, *pFarType);
			fnewline(dbOut);
		}
	}

	if (abTUnique(farValue)) {
		embed = tfSatEmbedType(abTUnique(farValue), *pFarType);
		if (!tfIsNone(*pFarType) && embed != AB_Embed_Identity) 
			abAddTContext(farValue, embed);
	}
	/* Calling program must set abTUnique(absyn). */
	return true;
}

/****************************************************************************
 *
 * :: No Values:  empty sequence, if w/o else, exit.
 *
 ***************************************************************************/

/*
 * This function tells the caller whether or not the void type
 * () satisfies the type provided. As a side-effect, the absyn
 * is given the void type if the type is satisfied otherwise the
 * specified error message is generated.
 */
local Bool
titdn0NoValue(Stab stab, AbSyn absyn, TFContext tfc, Msg msg)
{
	/*
	 * If the context requires a value of type () then
	 * this statement is type correct and has type ().
	 *
	 * We used to use tfSatValues(tfNone(), type) but
	 * this is useless because () satisfies any tuple
	 * type (it represents the empty tuple).
	 */
	if (tfIsEmptyMulti(ctxtTForm(tfc)))
	{
		/* Phew! The types match */
		abTUnique(absyn) = tfNone();
		return true;
	}
	else
	{
		/* Generate the required error message */
		comsgError(absyn, msg);
		return false;
	}
}


/****************************************************************************
 *
 * :: Id:  x, +, 1
 * X
 ***************************************************************************/

local Bool
titdnId(Stab stab, AbSyn absyn, TFContext tfc)
{
	Syme	syme = abSyme(absyn);

	tipIdDEBUG(dbOut,"Entering titdnId\n");


	/* If no meaning yet, find one */
	if (syme == NULL) {
		/*
		 * Use the default type if requested. This may be a
		 * source of bugs because we don't check conditions.
		 */
		if (abUse(absyn) == AB_Use_Default) {
			TForm	tf = tpossUnique(abTPoss(absyn));
			syme = symeNewLexVar(absyn->abId.sym, tf, car(stab));
		}
		else
			syme = tiGetMeaning(stab, absyn, ctxtTForm(tfc));


		/*
		 * Unfortunately we can't tell if tiGetMeaning() failed
		 * because there are no symbols of the correct type or
		 * whether there were some but the conditions were wrong.
		 */
		if (syme == NULL) return false;
	}


	/* Check lazy conditions */
	if (!symeUseIdentifier(absyn, syme))
		comsgError(absyn, ALDOR_E_TinNoMeaningForId, symeString(syme));


	/* Check conditions imposed on local exports/constants */
	if (symeIsExport(syme) || symeIsLexConst(syme)) {
		Bool	important = false;


		/*
		 * Some usages must be ignored: we don't want to
		 * complain that the redefinition of a conditional
		 * export (under a different condition) fails to
		 * satisfy the original condition!
		 */
		switch (abUse(absyn)) {
			/* Ignore the following usages */
			case AB_Use_Assign:		/* Fall through */
			case AB_Use_Declaration:	/* Fall through */
			case AB_Use_Define:		/* Fall through */
		   	case AB_Use_Elided:		/* Fall through */
				break;

			/*
			 * All other usages are important:
			 *    AB_Use_Type:	conditional types are evil
			 *    AB_Use_Value:	normal usage
			 *    AB_Use_RetValue:	normal usage
			 *    AB_Use_NoValue:	don't really care
			 *    AB_Use_Label:	don't really care
			 *    AB_Use_Default:	rarely seen here
			 *    AB_Use_Iterator:	never seen here
			 *    AB_Use_Except:	not sure
			 *    (unspecified):	normal usage
			 */
			default: important = true; break;
		}


		/* If important, ensure that the condition is satisfied */
		if (important) titdn0IdCondition(absyn, syme);
	}


	/* Give this leaf some meaning */
	stabSetSyme(stab, absyn, syme, abCondKnown);
	abTUnique(absyn) = symeType(syme);
	/*
	AbEmbed embed = tfSatEmbedType(symeType(syme), type);
	if (embed != AB_Embed_Identity) {
		abSetTContext(absyn, embed);
	}
	*/
	/* We return success even if an error was raised */
	return true;
}

local Bool
titdn0IdCondition(AbSyn id, Syme syme)
{
	SImpl	impl;
	AbLogic	cond;


	/* If it has a default implementation let it go */
	if (symeHasDefault(syme)) return true;


	/* Check to see if there are any implementation details. */
	impl = symeImpl(syme);


	/* If there aren't any then there are no conditions. */
	if (!impl) return true;


	/* At the moment we can only cope with simple conditions */
	if (implTag(impl) != SIMPL_Cond) return true;


	/* Check the implementation condition */
	cond = impl->implCond.cond;
	if (ablogImplies(abCondKnown, cond)) return true;


	/* Report the error */
	terrorIdCondition(symeType(syme), id, abCondKnown, cond);
	return false;
}


/****************************************************************************
 *
 * :: LitInteger:   32
 * :: LitFloat:	    4.0
 * :: LitString:    "hello"
 * X
 * !! This stuff could be made to go through ti...0Apply.
 ***************************************************************************/

local Bool	titdn0Literal (Symbol, Stab, AbSyn, TFContext);

/*
 * Top down entry points.
 */

local Bool
titdnLitInteger(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Literal(ssymTheInteger, stab, absyn, tfc);
}

local Bool
titdnLitFloat(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Literal(ssymTheFloat, stab, absyn, tfc);
}

local Bool
titdnLitString(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Literal(ssymTheString, stab, absyn, tfc);
}

/*
 * Functions which actually do the work.
 */

local Bool
titdn0Literal(Symbol sym, Stab stab, AbSyn absyn, TFContext tfc)
{
	SatMask		mask = tfSatBupMask(), result;
	Syme		syme = abSyme(absyn);
	TForm		retType = tfUnknown;

	if (syme) {
		assert(tfIsLitOpType(symeType(syme)));
		retType = tfMapRet(symeType(syme));
	}
	else {
		SymeList ml0, ml, okSymes = listNil(Syme);
		int 	n;

		ml0 = stabGetMeanings(stab, abCondKnown, sym);

		for (n = 0, ml = ml0; ml; ml = cdr(ml)) {
			TForm	tf = symeType(car(ml));

			tfFollow(tf);
			if (!tfIsLitOpType(tf)) continue;

			result = tfSat(mask, tfMapRet(tf), ctxtTForm(tfc));
			if (tfSatPending(result) && cdr(ml0)) {
				terrorApplyNotAnalyzed(absyn, absyn, tf);
				return false;
			}
			if (tfSatSucceed(result)) {
				if (n)
					okSymes= listCons(Syme)(syme, okSymes);

				syme = car(ml);
				retType = tfMapRet(tf);
				if (!symeUseIdentifier(absyn, syme))
					comsgError(absyn, 
						   ALDOR_E_TinNoMeaningForLit,
						   symString(sym), 
						   absyn->abLitString.str);
				n++;
			}
		}
		if (n != 1) {
			if (n) okSymes = listCons(Syme)(syme, okSymes);
			terrorNotUniqueMeaning(ALDOR_E_TinNMeanings,
					       absyn, okSymes, ml0,
					       abLeafStr(absyn), ctxtTForm(tfc));
			listFree(Syme)(okSymes);
			return false;
		}
	}

	mask = tfSatTdnMask();
	tfSat(mask, retType, ctxtTForm(tfc));

	abTUnique(absyn) = retType;
	stabSetSyme(stab, absyn, syme, abCondKnown);

	return true;
}

/***************************************************************************
 *
 * :: Comma: (a, b)
 *
 ***************************************************************************/

local Bool
titdnComma(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn		*argv	= abArgv(absyn);
	Length		i, argc	= abArgc(absyn);
	Length		n = 0;
	TPoss		tp = abTPoss(absyn);
	TPossIterator	it;
	TForm		rtype = NULL;
	AbEmbed 	embed;

	if (abUse(absyn) == AB_Use_Declaration ||
	    abUse(absyn) == AB_Use_Default) {
		for (i = 0; i < argc; i++)
			titdn(stab, argv[i], ctxtCopy(tfc, tfUnknown));
		rtype = ctxtTForm(tfc);
	}
	else {
		for (tpossITER(it,tp); tpossMORE(it); tpossSTEP(it)) {
			TForm	tt = tpossELT(it);
			if (tfSatReturn(tt, ctxtTForm(tfc))) {
				n++;
				rtype = tt;
			}
		}
		if (n != 1) {
			/* Note we haven't done titdn on descendents. */
			terrorNotUniqueType(ALDOR_E_TinExprMeans, absyn, ctxtTForm(tfc), tp);
			return false;
		}
		for (i = 0; i < argc; i++) {
			TForm	rti = tfMultiArgN(rtype,i);
			if (tfIsTypeTuple(ctxtTForm(tfc))) rti = tfType;
			titdn(stab, argv[i], ctxtCopy(tfc, rti));
		}
		embed = tfSatEmbedType(rtype, ctxtTForm(tfc));
		if (!tfIsNone(ctxtTForm(tfc)) && embed != AB_Embed_Identity)
			abAddTContext(absyn, embed);
	}

	abTUnique(absyn) = rtype;
	return true;
}

/****************************************************************************
 *
 * :: Apply:   f(a, b, ...)
 *
 ***************************************************************************/

// TODO: Move this function..
local Bool
titdnApplyCase(Stab stab, AbSyn absyn, TFContext tfc)
{
	TPoss lposs, rposs;
	TPossIterator it;
	AbSyn lhs = absyn->abApply.argv[0];
	AbSyn rhs = absyn->abApply.argv[1];
	TForm ctype;

	if (!tfSatReturn(tfBoolean, ctxtTForm(tfc))) {
		return false;
	}
	
	TPoss itsc = tpossIntersect(abTPoss(lhs), tpossPatternArg(abTPoss(rhs)));

	if (DEBUG(tipPattern)) {
		afprintf(dbOut, "tdnPattern: Itsc: %pTPoss\n", itsc);
	}
	
	if (!tpossIsUnique(itsc)) {
		return false;
	}

	titdn(stab, absyn->abApply.argv[0], ctxtCopy(tfc, tpossUnique(itsc)));
	titdn(stab, absyn->abApply.argv[1], ctxtCopy(tfc, tfPattern(tpossUnique(itsc))));

	abTUnique(absyn) = tfBoolean;
	abAddTContext(absyn, AB_Embed_ApplyCase);
	abTUnique(absyn->abApply.op) = tfUnknown; // "Magic" operator from (X, X) -> Bool that matches on the way
	return true;
}

local Bool
titdnApply(Stab stab, AbSyn absyn, TFContext tfc)
{
	SatMask	mask = tfSatBupMask();
	AbSyn op = abApplyOp(absyn);
	TPoss opTypes, nopTypes;
	TPossIterator it;
	Bool  isImplicit = false;
	Length nopc, popc, parmc;
	TForm  nopt, popt, opType;
	Bool   result;
	Bool   isPPartial = false;
	
	if (abUseIsPattern(abUse(absyn))) {
		mask = tfSatWithPatContext(mask);
	}

	nopc = 0;			/* Number of non-pending matches */
	popc = 0;			/* Number of all possible matches */
	nopt = tfUnknown;		/* Non-pending op type */
	popt = tfUnknown;		/* Any possible op type */
	opType = NULL;

	opTypes = abReferTPoss(op);
	nopTypes = tpossEmpty();
	if (abIsTheId(op, ssymJoin) && tpossIsUnique(opTypes) &&
	    tfSatisfies(tfMapRet(tpossUnique(opTypes)), tfCategory))
		return titdn0ApplyJoin(stab, absyn, tfc, op, abArgc(absyn), abApplyArgf);

	/* At this point, the mapping is either in the implicit part,
	 * or in the operator position.  Let's look at the operator
	 * first.
	 */
	for (tpossITER(it, opTypes); tpossMORE(it); tpossSTEP(it)) {
		TForm	opType = tpossELT(it);
		SatMask	result;

		if (DEBUG(tipApply)) {
			afprintf(dbOut, "--> tdnApply %pTForm\n", opType);
		}
		
		opType = tfDefineeType(opType);
		if (!tfIsAnyMap(opType))
			continue;

		if (tfIsPatMatch(opType) && !abUseIsPat(absyn)) {
			continue;
		}

		if (tfIsFunctionMap(opType) && abUse(absyn) == AB_Use_Pattern) {
			continue;
		}
		
		result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, abApplyArgc(absyn), abApplyArgf);
		if (DEBUG(tipApply)) {
			afprintf(dbOut, "--> tdnApply - SatMap %s\n", tfSatMaskToString(result));
		}
		
		if (tfSatSucceed(result)) {
			if (!tfSatPending(result)) {
				nopc += 1;
				nopt = opType;
				nopTypes = tpossAdd1(nopTypes, opType);
			}
			popc += 1;
			popt = opType;
		}
	}
	if (abUseIsPat(absyn)) {
		for (tpossITER(it, opTypes); tpossMORE(it); tpossSTEP(it)) {
			TForm	opType = tpossELT(it);
			SatMask	result;
			if (!tfIsPPartialMap(opType)) {
				continue;
			}
			TForm patType = tfsEmbedResult(opType, AB_Embed_ApplyPatCall);
			result = tfSatMap(mask, stab, patType, ctxtTForm(tfc), absyn, abApplyArgc(absyn), abApplyArgf);
			if (DEBUG(tipApply)) {
				afprintf(dbOut, "--> tdnApply - SatMap %s\n", tfSatMaskToString(result));
			}
			
			if (tfSatSucceed(result)) {
				if (!tfSatPending(result)) {
					nopc += 1;
					nopt = patType;
					nopTypes = tpossAdd1(nopTypes, patType);
				}
				popc += 1;
				popt = patType;
				isPPartial = true;
			}
		}
	}
	/* And now the implicit part */
	if (abImplicit(absyn) != NULL) {
		AbSyn implicitApply = abImplicit(absyn);
		TPoss implicitOpTypes = abTPoss(implicitApply);
		isImplicit = true;
		for (tpossITER(it, implicitOpTypes); tpossMORE(it); tpossSTEP(it)) {
			TForm	opType = tpossELT(it);
			SatMask	result;

			opType = tfDefineeType(opType);
			assert(tfIsAnyMap(opType));

			result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, abArgc(absyn), abArgf);
			if (tfSatSucceed(result)) {
				if (!tfSatPending(result)) {
					nopc += 1;
					nopt = opType;
					nopTypes = tpossAdd1(nopTypes, opType);
				}
				popc += 1;
				popt = opType;
			}
		}
	}

	if (popc == 1) {
		/* We found one thing.. must be this one */
		opType = popt;
		result = true;
	}
	else if (nopc == 1) {
		/* We found one non-pending one, and possibly some others.  Let's use it */
		opType = nopt;
		result = true;
	}
	else if (nopc == 0 && popc > 0) {
		/* All pending, and more than one of them.  Error - not analyzed */
		terrorApplyNotAnalyzed(absyn, op, popt);
		result = false;
	}
	else {
		/* Anything else - error */
		terrorApplyFType(absyn, ctxtTForm(tfc), nopTypes, op, stab, abApplyArgc(absyn), abApplyArgf);
		result = false;
	}

	if (result && isPPartial) {
		abAddTContext(op, AB_Embed_ApplyPatCall);
	}
	
	tpossFree(opTypes);
	tpossFree(nopTypes);

	if (!result) return false;

	/*
	if (abIsApplyOf(absyn, ssymTheCase)
	    && abApplyArgc(absyn) == 2) {
		afprintf(dbOut, "TF OP: %pTForm\n", opType);
		afprintf(dbOut, "TF arg1: %oBool %pTForm\n",
			 tfIsPattern(tfMapArgN(opType, 1)),
			 tfMapArgN(opType, 1));
	}
	*/
	if (abIsApplyOf(absyn, ssymTheCase)
	    && abApplyArgc(absyn) == 2
	    && tfIsPatternCaseArg(tfMapArgN(opType, 1))) {
		// Leave as is.  Ideally we would have a 'case' builtin on Pattern
		abTUnique(absyn) = tfBoolean;
		abAddTContext(absyn, AB_Embed_ApplyCase);
		// "Magic" operator from (X, X) -> Bool that matches on the way
		abTUnique(absyn->abApply.op) = opType; 

		mask = tfSatTdnMask();
		result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, abApplyArgc(absyn), abApplyArgf);
	}
	else if (isImplicit) {
		AbSyn imp = abImplicit(absyn);
		int parmc;
		titdn(stab, imp, ctxtCopy(tfc, opType));

		parmc = tfMapHasDefaults(opType) ? tfMapArgc(opType) : abArgc(absyn);
		abAddTContext(imp, tfMapMultiArgEmbed(opType, parmc));

		mask = tfSatTdnMask();
		result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, abArgc(absyn), abArgf);
	}
	else {
		int parmc;
		abFree(abImplicit(absyn));
		abSetImplicit(absyn, NULL);
		titdn(stab, op, ctxtCopy(tfc, opType));

		parmc = tfMapHasDefaults(opType) ? tfMapArgc(opType) : abApplyArgc(absyn);
		abAddTContext(op, tfMapMultiArgEmbed(opType, parmc));

		mask = tfSatTdnMask();
		result = tfSatMap(mask, stab, opType, ctxtTForm(tfc), absyn, abApplyArgc(absyn), abApplyArgf);
	}
	/* We return false rarely (eg titdn0FarValue failure). */
	return tfSatSucceed(result);
}



/****************************************************************************
 *
 * :: Define:	a == e
 * X
 ***************************************************************************/

local Bool
titdnDefine(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn		lhs = absyn->abDefine.lhs;
	AbSyn		rhs = absyn->abDefine.rhs;
	TForm		rtype, ltype, idtype;
	TPoss		idtposs;

	if (tfIsNone(ctxtTForm(tfc))) tfc = ctxtCopy(tfc, tfUnknown);

	idtype = tiDefineFilter(absyn, ctxtTForm(tfc));
	idtposs = tiDefineTPoss(absyn);

	rtype = tpossSelectSatisfier(idtposs, idtype);
	if (!rtype) {
		terrorNotUniqueType(ALDOR_E_TinDefnMeans, absyn, idtype, idtposs);
		return false;
	}

	/* If the r.h. type satisfies the constraint, relax on the l.h. */
	if (abState(lhs) == AB_State_HasUnique &&
	    tfSatValues(abTUnique(lhs), rtype))
		ltype = abTUnique(lhs);
	else {
		assert(abState(lhs) == AB_State_HasPoss);
		if (tpossIsUnique(abTPoss(lhs)) &&
		    tfSatValues(tpossUnique(abTPoss(lhs)), rtype))
			ltype = tpossUnique(abTPoss(lhs));
		else
			ltype = rtype;
	}

	if (DEBUG(tipDefine)) {
		fprintf(dbOut, "************** Defining: ");
		abPrettyPrint(dbOut, lhs);
		fnewline(dbOut);
	}

	titdn(stab, lhs, ctxtCopy(tfc, ltype));
	titdn(stab, rhs, ctxtCopy(tfc, rtype));

	if (abTag(lhs) == AB_Declare) {
		rtype = tpossSelectSatisfier(abTPoss(absyn), ctxtTForm(tfc));
		if (!rtype) {
			terrorNotUniqueType(ALDOR_E_TinDefnMeans, absyn,
					    ctxtTForm(tfc), abTPoss(absyn));
			return false;
		}
	}

	
	abTUnique(absyn) = rtype;
	
	if (DEBUG(tipDefine)) {
		fprintf(dbOut,"Tdn: Define of ");
		abPrint(dbOut, lhs);
		fprintf(dbOut," has type ");
		tfPrint(dbOut, rtype);
		fnewline(dbOut);
	}
	return true;
}

/****************************************************************************
 *
 * :: Assign:	a := e
 * X
 ***************************************************************************/

/*!! To do: (v.i,v.j) := (v.j, v.i)	 */

local Bool
titdnAssign(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn		rhs = absyn->abAssign.rhs;
	AbSyn		lhs = absyn->abAssign.lhs;
	TPoss		abtposs = abTPoss(absyn);
	TForm		rtype;
	AbEmbed 	embed;

	if (tfIsNone(ctxtTForm(tfc))) tfc = ctxtCopy(tfc, tfUnknown);
	rtype = tpossSelectSatisfier(abtposs, ctxtTForm(tfc));
	if (!rtype) {
		terrorNotUniqueType(ALDOR_E_TinAssMeans, absyn, ctxtTForm(tfc), abtposs);
		return false;
	}

	if (abTag(lhs) == AB_Apply)
		return titdn0ApplySym(stab, absyn, ctxtCopy(tfc, rtype), ssymSetBang,
				      abArgc(lhs) + 1, abSetArgf, lhs);

	titdn(stab, rhs, ctxtCopy(tfc, rtype));
	titdn(stab, lhs, ctxtCopy(tfc, rtype));
	abTUnique(absyn) = rtype;
	
	embed = tfSatEmbedType(abTUnique(rhs), rtype);
	if (!tfIsNone(rtype) && embed != AB_Embed_Identity) 
		abAddTContext(rhs, embed);


	if (DEBUG(tipAssign)) {
		fprintf(dbOut,"Tdn: Assignment to ");
		abPrint(dbOut, lhs);
		fprintf(dbOut," has type ");
		tfPrint(dbOut, rtype);
		fnewline(dbOut);
	}
	return true;
}

/****************************************************************************
 *
 * :: Declare:	 a: A
 *
 ***************************************************************************/

local Bool
titdnDeclare(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn id     = absyn->abDeclare.id;
	AbSyn idtype = absyn->abDeclare.type;
	TForm tf, rtype;
	Syme  syme;

	if (DEBUG(tipDeclare)) {
		fprintf(dbOut, "In the declaration ");
		abPrettyPrint(dbOut, absyn);
		fprintf(dbOut, ", the semantics field is ");
		if (abTForm(absyn))
			tfPrint(dbOut, abTForm(absyn));
		else
			fprintf(dbOut, "_");
		fnewline(dbOut);
	}

	if (abUse(absyn) == AB_Use_Define || abUse(absyn) == AB_Use_Assign)
		tf = (tfIsUnknown(ctxtTForm(tfc)) ? tiGetTForm(stab, idtype) : ctxtTForm(tfc));
	else
		tf = tiGetTForm(stab, idtype);

	/* Prevent stabUseMeaning during titdn(stab, id, tf) */
	if (abUse(absyn) != AB_Use_Assign) stabUseMeaningShadow(id);

 	syme = abSyme(id);
	if (syme && tfIsMulti(ctxtTForm(tfc))) {
		comsgError(absyn, ALDOR_E_TinBadDeclare);
		return false;
	}

	titdn(stab, id, ctxtCopy(tfc, tf));
	stabUseMeaningUnshadow();

	/* idtype has been handled already. */

	rtype = tpossSelectSatisfier(abTPoss(absyn), ctxtTForm(tfc));
	if (!rtype) rtype = tfUnknown;
	abTUnique(absyn) = rtype;

	if (DEBUG(tipDeclare)) {
		fprintf(dbOut,"Tdn: Declare of ");
		abPrint(dbOut, id);
		fprintf(dbOut," has type ");
		tfPrint(dbOut, rtype);
		fnewline(dbOut);
	}
	return true;
}

/****************************************************************************
 *
 * :: Label:   @@ x @@ [e]
 *
 ***************************************************************************/

local Bool
titdnLabel(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn	expr = absyn->abLabel.expr;

	if (titdn(stab, expr, tfc))
		abTUnique(absyn) = abTUnique(expr);
	else
		abTUnique(absyn) = tfUnknown;

	return true;
}

/****************************************************************************
 *
 * :: Goto:   goto id
 *
 ***************************************************************************/

local Bool
titdnGoto(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn label = absyn->abGoto.label;

	if (stabLabelExistsInThisStab(stab, abIdSym(label))) {
		abTUnique(absyn) = tfExit;
		return true;
	} 
	else {
		AbSynList abl = stabGetLabels(stab, abIdSym(label));
		while (abl) {
			AbSyn label = car(abl);
			Syme syme = label->abHdr.seman->syme;
			ULong labelLamLev = symeDefLambdaLevelNo(syme);
			ULong labelLexLev = symeDefLevelNo(syme);
			ULong gotoLamLev = stabLambdaLevelNo(stab);
			ULong gotoLexLev = stabLevelNo(stab);
			
			if (labelLamLev == gotoLamLev && labelLexLev <= gotoLexLev) {
				abTUnique(absyn) = tfExit;
				return true;
			}
			abl = cdr(abl);
		}

		comsgError(absyn->abGoto.label, ALDOR_E_TinFarGoto);
		return false;
	}
}

/****************************************************************************
 *
 * :: Lambda:	(a: A): B +-> b
 * :: PLambda:	(a: A): B +->* b
 *
 ***************************************************************************/

local Bool
titdnLambda(Stab stab, AbSyn absyn, TFContext tfc)
{
	Scope("titdnLambda");
	AbSyn	param = absyn->abLambda.param;
	AbSyn	body  = absyn->abLambda.body;
	AbSyn	ret   = absyn->abLambda.rtype;
	TPoss	abtposs;
	TForm	rtype;
	Bool	result = true;

	TForm		fluid(tuniReturnTForm);
	TForm		fluid(tuniYieldTForm);
	TForm		fluid(tuniExitTForm);
	AbSynList	fluid(abReturnsList);

	tuniReturnTForm	= tfNone();
	tuniYieldTForm	= tfNone();
	tuniExitTForm	= tfNone();
	abReturnsList = listNil(AbSyn);

	tuniReturnTForm = tfFullFrAbSyn(stab, ret);

	titdn(stab, param, ctxtCopy(tfc, tfUnknown));
	titdn(stab, body, ctxtCopy(tfc, tuniReturnTForm));

	abtposs = abTPoss(absyn);
	if (tiCheckLambdaType(ctxtTForm(tfc)) || !tfIsAnyMap(ctxtTForm(tfc)))
		rtype = tpossSelectSatisfier(abtposs, ctxtTForm(tfc));
	else
		rtype = ctxtTForm(tfc);

	if (!rtype) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans, absyn, ctxtTForm(tfc), abtposs);
		result = false;
	}

	else if (abHasTag(absyn, AB_PLambda)) {
		Bool tres;

		while (abTag(body) == AB_Label)
			body = body->abLabel.expr;
		abReturnsList = listCons(AbSyn)(body, abReturnsList);
		tres = titdn0PLambdaArgs(stab, param) &&
			 titdn0PLambdaRets(stab, abReturnsList);

		/* Return false if either result or tres are false */
		result = result ? tres : result;
	}

	/* Only allowed to set abTUnique if successful */
	if (result) abTUnique(absyn) = rtype;

	listFree(AbSyn)(abReturnsList);
	Return(result);
}

local Bool
titdn0PLambdaArgs(Stab stab, AbSyn param)
{
	AbSyn	*argv;
	Length	i, argc;
	Bool	result = true;

	switch (abTag(param)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Comma:
		argc = abArgc(param);
		argv = abArgv(param);
		break;
	default:
		argc = 1;
		argv = &param;
		break;
	}

	for (i = 0; result && i < argc; i += 1) {
		AbSyn	argi = abDefineeId(argv[i]);
		assert(abState(argi) == AB_State_HasUnique);
		result = tiRawToUnary(stab, argi, abTUnique(argi));
	}

	return result;
}

local Bool
titdn0PLambdaRets(Stab stab, AbSynList vals)
{
	Bool	result = true;

	for (; result && vals; vals = cdr(vals)) {
		AbSyn	val = car(vals);
		assert(abState(val) == AB_State_HasUnique);
		result = tiUnaryToRaw(stab, val, tfUnknown);
	}

	return result;
}

/****************************************************************************
 *
 * :: Sequence:	  (a; b; c)
 *
 ***************************************************************************/

local Bool
titdnSequence(Stab stab, AbSyn absyn, TFContext tfc)
{
	Scope("titdnSequence");
	TForm		fluid(tuniExitTForm);
	AbSynList	fluid(abExitsList);
	AbSyn		fluid(tuniTdnSelectObj);
	Bool		result;

	tuniExitTForm    = ctxtTForm(tfc);
	abExitsList      = listNil(AbSyn);
	tuniTdnSelectObj = NULL;

	result = titdnSequence0(stab, absyn, tfc);

	Return(result);
}

local Bool
titdnSequence0(Stab stab, AbSyn absyn, TFContext tfc)
{
	int		i, n = abArgc(absyn);
	Bool		result;

	if (abState(absyn) == AB_State_Error)
		abState(absyn) = AB_State_HasPoss;

	if (n == 0)
		result = titdn0NoValue(stab, absyn, tfc, ALDOR_E_TinContextSeq);
	else
	{
		TForm none = tfNone();

		for (i = 0; i < n-1; i++)
			titdn(stab, abArgv(absyn)[i], ctxtCopy(tfc, none));

		titdn0FarValue(stab,absyn, tfc, abArgv(absyn)[n-1],
			       &tuniExitTForm, &abExitsList);

		if (abState(absyn) == AB_State_Error) {
			terror(stab, absyn, ctxtTForm(tfc));
			result = false;
		}
		else {
			abTUnique(absyn) = tuniExitTForm;
			result = true;
		}
	}
	listFree(AbSyn)(abExitsList);

	return result;
}

/****************************************************************************
 *
 * :: Exit:  (...; b => x ; ...)
 *
 ***************************************************************************/

local Bool
titdnExit(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn	test  = absyn->abExit.test;
	AbSyn	value = absyn->abExit.value;
	AbLogic	saveCond;
	Bool    pushCond;
	titdn(stab, test, ctxtCopy(tfc, tfUnknown));

	pushCond = !tuniTdnSelectObj && abIsSefo(test);
	if (pushCond) {
		/* See tibupExit for comments */
		AbSyn nTest = abExpandDefs(stab, test);
		ablogAndPush(&abCondKnown, &saveCond, nTest, true);
	}

	titdn0FarValue(stab, absyn, tfc, value, &tuniExitTForm, &abExitsList);

	if (pushCond)
		ablogAndPop (&abCondKnown, &saveCond);
	
	return titdn0NoValue(stab, absyn, tfc, ALDOR_E_TinContextExit);
}

/***************************************************************************
 *
 * :: return: return x,	 return;
 *
 ***************************************************************************/

local Bool
titdnReturn(Stab stab, AbSyn absyn, TFContext tfc)
{
	titdn0FarValue(stab, absyn, tfc, absyn->abReturn.value,
		       &tuniReturnTForm, &abReturnsList);
	abTUnique(absyn) = tfExit;
	return true;
}

/****************************************************************************
 *
 * :: Generate:	 generate [N] of (... yield ...)
 *
 ***************************************************************************/

local Bool
titdnGenerate(Stab stab, AbSyn absyn, TFContext tfc)
{
	Scope("titdnGenerate");

	TForm		fluid(tuniReturnTForm);
	TForm		fluid(tuniYieldTForm);
	TForm		fluid(tuniExitTForm);
	AbSynList	fluid(abYieldsList);
	AbSyn		body = absyn->abGenerate.body;
	AbSyn		count = absyn->abGenerate.count;
	Bool		result;
	TfGenType	tfGenType;

	tuniReturnTForm	= tfNone();
	tuniYieldTForm	= tfNone();
	tuniExitTForm	= tfNone();
	abYieldsList = listNil(AbSyn);

	tfGenType = abFlag_IsNewIter(absyn) ? TFG_XGenerator : TFG_Generator;

	if (tfIsAnyGenerator(ctxtTForm(tfc)))
		tuniYieldTForm = tfAnyGeneratorArg(ctxtTForm(tfc));
	else
		tuniYieldTForm = tfUnknown;

	if (abTag(count) != KW_From) {
		titdn(stab, count, ctxtCopy(tfc, tfUnknown));
	}
	titdn(stab, body,  ctxtCopy(tfc, tfNone()));

	if (abState(absyn) == AB_State_Error) {
		/*
		 * The problem must be in the body. We hope that
		 * errors in the count expression will be handled
		 * by someone else. The trouble is that this kind
		 * of thinking caused bugs like #1144 which we are
		 * trying to fix here ...
		 *
		 * Anyway, since we have fully analysed the body of
		 * the generator, it probably has a unique type now.
		 * If so we turn it into a tposs for terror().
		 */
		if (abState(body) == AB_State_HasUnique) {
			abState(body) = AB_State_HasPoss;
			abTPoss(body) = tpossSingleton(abTUnique(body));
		}


		/*
		 * If errors were detected then they will have been
		 * reported by titdn and the unique type associated
		 * with the body will satisfy the context of the
		 * generator. This means that we won't repeat these
		 * error messages when we call terror().
		 *
		 * If we didn't detect any errors when we dealt with
		 * the body then this node must be bad because its
		 * inferred type does not satisfy the context type.
		 * Hopefully terror() will report the problem for us.
		 */
		terror(stab, body, ctxtTForm(tfc));
	}

	/* Were we successful? */
	result = (abState(absyn) != AB_State_Error) ? true : false;

	/* Only allowed to set abTUnique if returning true */
	if (result) abTUnique(absyn) = tfAnyGenerator(tfGenType, tuniYieldTForm);

	listFree(AbSyn)(abYieldsList);
	Return(result);
}

/***************************************************************************
 *
 * :: Yield:  yield x
 *
 ***************************************************************************/

local Bool
titdnYield(Stab stab, AbSyn absyn, TFContext tfc)
{
	titdn0FarValue(stab, absyn, tfc, 
                       absyn->abYield.value,
		       &tuniYieldTForm, 
                       &abYieldsList);
	abTUnique(absyn) = tfExit;
	return true;
}

/****************************************************************************
 *
 * :: Add:  [D] add (a: A == ...)
 *
 ***************************************************************************/

local Bool
titdnAdd(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn		base	= absyn->abAdd.base;
	AbSyn		capsule = absyn->abAdd.capsule;

	titdn(stab, base,    ctxtCopy(tfc, tfUnknown));
	titdn(stab, capsule, ctxtCopy(tfc, tfUnknown));

	/* !! Should infer type of body. */
	abTUnique(absyn) = ctxtTForm(tfc);
	return true;
}

/****************************************************************************
 *
 * :: With:  [C] with (a: A; ...)
 *
 ***************************************************************************/

local Bool
titdnWith(Stab stab, AbSyn absyn, TFContext tfc)
{
	TPoss	abtposs = abTPoss(absyn);
	TForm	rtype;

	rtype = tpossSelectSatisfier(abtposs, ctxtTForm(tfc));
	if (!rtype) return false;

	abTUnique(absyn) = rtype;
	return true;
}

/****************************************************************************
 *
 * :: Where:  e where d
 *
 ***************************************************************************/

local Bool
titdnWhere(Stab stab, AbSyn absyn, TFContext tfc)
{
	TPoss   abtposs = abTPoss(absyn);
	AbSyn	context = absyn->abWhere.context;
	AbSyn	expr	= absyn->abWhere.expr;
	TForm   type;

	titdn(stab, context, ctxtCopy(tfc, tfNone()));
	titdn(stab, expr,    tfc);
	
	type = tpossSelectSatisfier(abtposs, ctxtTForm(tfc));
	if (!type) return false;

	abTUnique(absyn) = type;
	return true;
}

/****************************************************************************
 *
 * :: If:  if b then t [else e]
 *
 ***************************************************************************/

local Bool
titdnIf(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn	test	= absyn->abIf.test;
	AbSyn	thenAlt = absyn->abIf.thenAlt;
	AbSyn	elseAlt = absyn->abIf.elseAlt;
	AbSyn	nTest	= test;
	AbLogic	saveCond;

	TPoss	abtposs = abTPoss(absyn);
	abtposs = tpossSatisfiesType(abtposs, ctxtTForm(tfc));

	if (tpossCount(abtposs) > 1) {
		terrorNotUniqueType(ALDOR_E_TinIfMeans, absyn, ctxtTForm(tfc), abtposs);
		return false;
	}

	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	assert(tfBoolean != tfUnknown);


	/* This ought to do nothing since we tinfered test during tibup */
	titdn(stab, test, ctxtCopy(tfc, tfBoolean));


	if (tfIsCategoryContext(ctxtTForm(tfc), absyn))
		tfc = ctxtCopy(tfc, tpossUnique(abtposs));
	else	/* Normalise the test for other contexts */
		nTest = abExpandDefs(stab, test);

	if (abIsSefo(nTest)) {
		ablogAndPush(&abCondKnown, &saveCond, nTest, true); /* test, true); */
		titdn(stab, thenAlt, tfc);
		ablogAndPop (&abCondKnown, &saveCond);

		ablogAndPush(&abCondKnown, &saveCond, nTest, false); /* test, false); */
		titdn(stab, elseAlt, tfc);
		ablogAndPop (&abCondKnown, &saveCond);
	}
	else {
		titdn(stab, thenAlt, tfc);
		titdn(stab, elseAlt, tfc);
	}

	/*
	 * We can't use tpossUnique(abtposs) here because otherwise we
	 * will end up performing embeddings on ourself in addition to
	 * the same embeddings that we performed on the branches.
	 */
	abTUnique(absyn) = ctxtTForm(tfc);
	return true;
}

/****************************************************************************
 *
 * :: Test:  implied test
 *
 ***************************************************************************/

AbSyn
titdnSelectArgf(AbSyn ab, Length i)
{
	if (i == 0)
		return tuniTdnSelectObj;
	else
		return abArgv(ab)[i - 1];
}

local Bool
titdnTest(Stab stab, AbSyn absyn, TFContext tfc)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	assert(tfBoolean != tfUnknown);

	if (tuniTdnSelectObj != NULL) {
		titdn0ApplySym(stab, absyn, 
			       ctxtCopy(tfc, tfBoolean),
			       ssymTheCase, 2,
			       titdnSelectArgf, NULL);
	}
	else {
		titdn0ApplySymIfNeeded(stab, absyn, ctxtCopy(tfc, tfBoolean), 
				       ssymTheTest, 1, abArgf, NULL,
				       tfIsBooleanFn);
	}
	abTUnique(absyn) = tfBoolean;
	return true;
}

/***************************************************************************
 *
 * :: Collect:	e <iter>*
 *
 ***************************************************************************/

local Bool
titdnCollect(Stab stab, AbSyn absyn, TFContext tfc)
{
	Scope("titdnCollect");
	AbSyn	body	  = absyn->abCollect.body;
	AbSyn	*iterv	  = absyn->abCollect.iterv;
	Length	i, iterc  = abCollectIterc(absyn);
	TForm	type, rtype;


	TForm	fluid(tuniReturnTForm);
	TForm	fluid(tuniYieldTForm);
	TForm	fluid(tuniExitTForm);

	tuniReturnTForm	= tfNone();
	tuniYieldTForm	= tfNone();
	tuniExitTForm	= tfNone();

	for (i = 0; i < iterc; i++)
		titdn(stab, iterv[i], ctxtCopy(tfc, tfUnknown));

	type = ctxtTForm(tfc);
	if (tfIsAnyGenerator(type))
		rtype = tfAnyGeneratorArg(type);
	else
		rtype = tfUnknown;

	titdn(stab, body, ctxtCopy(tfc, rtype));
	abTUnique(absyn) = ctxtTForm(tfc);

	{
		AbEmbed	embed = tfSatEmbedType(abTUnique(body), rtype);
		if (!tfIsNone(rtype) && embed != AB_Embed_Identity) 
			abAddTContext(body, embed);
	}

	Return(true);
}

/****************************************************************************
 *
 * :: Repeat:  <iter>* repeat e
 *
 ***************************************************************************/

local Bool
titdnRepeat(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn	body	  = absyn->abRepeat.body;
	AbSyn	*iterv	  = absyn->abRepeat.iterv;
	Length	i, iterc  = abRepeatIterc(absyn);
	Bool	result;

	for (i = 0; i < iterc; i++)
		titdn(stab, iterv[i], ctxtCopy(tfc, tfUnknown));

	titdn(stab, body, ctxtCopy(tfc, tfNone()));
	if (tfIsNone(tpossUnique(abTPoss(absyn)))) {
		result = titdn0NoValue(stab,absyn,tfc,ALDOR_E_TinContextRepeat);
	}
	else {
		abTUnique(absyn) = ctxtTForm(tfc);
		result = true;
	}

	return result;
}

/***************************************************************************
 *
 * :: Never
 * X
 ***************************************************************************/

local Bool
titdnNever(Stab stab, AbSyn absyn, TFContext tfc)
{
	abTUnique(absyn) = tfExit;
	return true;
}

/***************************************************************************
 *
 * :: Iterate
 * X
 ***************************************************************************/

local Bool
titdnIterate(Stab stab, AbSyn absyn, TFContext tfc)
{
	abTUnique(absyn) = tfExit;
	return true;
}

/***************************************************************************
 *
 * :: Break
 * X
 ***************************************************************************/

local Bool
titdnBreak(Stab stab, AbSyn absyn, TFContext tfc)
{
	abTUnique(absyn) = tfExit;
	return true;
}

/****************************************************************************
 *
 * :: While:  while c
 *
 ***************************************************************************/

local Bool
titdnWhile(Stab stab, AbSyn absyn, TFContext tfc)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	assert(tfBoolean != tfUnknown);

	return titdn0Generic(stab, absyn, ctxtCopy(tfc, tfBoolean));
}

/***************************************************************************
 *
 * :: For:  for x in l | c
 *
 ***************************************************************************/

local Bool
titdnFor(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn	lhs   = absyn->abFor.lhs;
	AbSyn	test  = absyn->abFor.test;
	TForm	twhole;
	Bool unique;

	unique = tfIsUnknown(ctxtTForm(tfc)) && tpossIsUnique(abTPoss(lhs));
	if (unique && abFlag_IsNewIter(absyn))
		twhole = tfXGenerator(tpossUnique(abTPoss(lhs)));
	else if (unique)
		twhole = tfGenerator(tpossUnique(abTPoss(lhs)));
	else
		twhole = ctxtTForm(tfc);

	/*
	 * Subtle note: the generator in a for-iterator lies
	 * outside the scope level of the repeat. This means
	 * that we have to use cdr(stab) whenever we tinfer
	 * absyn->abFor.whole or via abForIterArgf().
	 */

	if (!abFlag_IsNewIter(absyn)) {
		titdn0ApplySymIfNeeded(cdr(stab), absyn, ctxtCopy(tfc, twhole),
				       ssymTheGenerator, 1, abForIterArgf,
				       NULL, tfIsGeneratorFn);
	}
	else if (abFlag_IsNewIter(absyn)) {
		titdn0ApplySymIfNeeded(cdr(stab), absyn, ctxtCopy(tfc, twhole),
				       ssymTheXGenerator, 1, abForIterArgf,
				       NULL, tfIsXGeneratorFn);
	}

	/*
	 * The for-variable and test lie within the scope
	 * of the repeat clause.
	 */
	titdn(stab, lhs,  ctxtCopy(tfc, tfUnknown));
	titdn(stab, test, ctxtCopy(tfc, tfUnknown));
        
	abTUnique(absyn) = twhole;
	return true;
}

/****************************************************************************
 *
 * :: Foreign:	import ... from Foreign(...)
 *
 ***************************************************************************/

local Bool titdnForeignJava(Stab stab, AbSyn absyn);
local Bool titdnForeignJavaDeclare(Stab stab, AbSyn decl);

local Bool
titdnForeignImport(Stab stab, AbSyn absyn, TFContext tfc)
{
	ForeignOrigin forg = forgFrAbSyn(absyn->abForeignImport.origin);
	Bool ok;
	titdn(stab, absyn->abForeignImport.what, ctxtCopy(tfc, tfUnknown));

	switch (forg->protocol) {
	case FOAM_Proto_Java:
		ok = titdnForeignJava(stab, absyn->abForeignImport.what);
		break;
	default:
		ok = true;
		break;
	}
	if (!ok) {
		return false;
	}

	abTUnique(absyn) = ctxtTForm(tfc);
	return true;
}

local Bool
titdnForeignJava(Stab stab, AbSyn what)
{
	AbSyn inner;
	int i;
	Bool ok;

	switch (abTag(what)) {
	case AB_Sequence:
		ok = true;
		for (i=0; i<abArgc(what); i++) {
			ok = ok && titdnForeignJava(stab, what->abSequence.argv[i]);
		}
		return ok;
	case AB_Declare:
		return titdnForeignJavaDeclare(stab, what);
	case AB_Id:
		return false;
	default:
		return false;
	}
	return true;
}

local Bool
titdnForeignJavaDeclare(Stab stab, AbSyn decl)
{
	TForm tf;
	Syme syme;
	SymeList l;
	assert(abTag(decl) == AB_Declare);

	syme = abSyme(decl->abDeclare.id);
	if (syme == NULL)
		return true; /* Error is elsewhere - not here */
	tf = symeType(syme);

	l = tfGetCatExports(tf);

	while (l != listNil(Syme)) {
		ErrorSet errors;
		Syme syme = car(l);
		Bool bad;
		l = cdr(l);
		errors = symeIsJavaExport(syme);

		if (errorSetHasErrors(errors)) {
			/* This isn't the most efficient, but if the list has more than
			 * a few elements something is badly wrong anyway */
			StringList tmp = errorSetErrors(errors);
			String s = strPrintf("%s: %s cannot be used as a java function:\n",
					     symeString(syme),
					     abPretty(tfExpr(symeType(syme))));
			while (tmp != listNil(String)) {
				s = strNConcat(s, "\t");
				s = strNConcat(s, car(tmp));
				s = strNConcat(s, "\n");
				tmp = cdr(tmp);
			}
			errorSetFree(errors);
			comsgError(decl, ALDOR_E_ExplicitMsg, s);

			abState(decl) = AB_State_Error;
			return false;
		}
		errorSetFree(errors);

	}
	return true;
}

/****************************************************************************
 *
 * :: export:  export ... to D
 *
 ***************************************************************************/

local Bool
titdnForeignExport(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn	what	= absyn->abForeignExport.what;
	AbSyn	dest	= absyn->abForeignExport.dest;
	ForeignOrigin forg = forgFrAbSyn(dest->abApply.argv[0]);

	Bool success = titdn(stab, absyn->abForeignExport.what, ctxtCopy(tfc, tfUnknown));
	if (success && forg->protocol == FOAM_Proto_Java) {
		stabAddForeignExport(stab, tiGetTForm(stab, what), forg);
	}
	abTUnique(absyn) = ctxtTForm(tfc);
	return true;
}

/****************************************************************************
 *
 * :: Import:  import ... from D
 *
 ***************************************************************************/


local Bool
titdnImport(Stab stab, AbSyn absyn, TFContext tfc)
{
	if (!tfSatReturn(tfNone(), ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
				    absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}
	abTUnique(absyn) = tfNone();
	return true;
}

/****************************************************************************
 *
 * :: Inline:  inline .. from D
 *
 ***************************************************************************/

local Bool
titdnInline(Stab stab, AbSyn absyn, TFContext tfc)
{
	if (!tfSatReturn(tfNone(), ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
				    absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}
	abTUnique(absyn) = tfNone();
	return true;
}

/****************************************************************************
 *
 * :: Qualify:	 A $ B
 *
 * ToDo: 3$Integer
 *
 ***************************************************************************/

local Bool
titdnQualify(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn		origin = absyn->abQualify.origin;
	AbSyn		what   = absyn->abQualify.what;
	Symbol		sym    = what->abId.sym;

	TForm		tforg;
	SymeList	symes, msymes, fsymes, allSymes, okSymes;
	Syme		syme, osyme;

	tforg = tiGetTForm(stab, origin);

	if (tfIsForeign(tforg)) {
		msymes = stabGetMeanings(stab, abCondKnown, what->abId.sym);
		symes = listNil(Syme);

		for ( ; msymes; msymes = cdr(msymes))
			if (symeId(car(msymes)) == sym && symeIsForeign(car(msymes)))
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
			assert(symeId(car(msymes)) == sym);
			if (ablogIsListKnown(symeCondition(car(msymes))))
				symes = listCons(Syme)(car(msymes), symes);
		}
		fsymes = symes;
	}

	allSymes = symes;
	okSymes  = listNil(Syme);
	syme	 = NULL;

	for (symes = allSymes; symes; symes = cdr(symes)) {
		syme = car(symes);

		if (symeId(syme) == sym 
		    && tfSatReturn(symeType(syme), ctxtTForm(tfc))) {
			osyme = symeListHasExtendee(okSymes, syme);
			if (osyme) {
				symeSetExtension(osyme, syme);
				okSymes = symeListExtend(okSymes, syme);
				continue;
			}

			osyme = symeListHasExtension(okSymes, syme);
			if (osyme) continue;
			if (tiMergeSyme(syme, okSymes))
			    okSymes = listCons(Syme)(syme, okSymes);
		}
	}

	if (okSymes == NULL || cdr(okSymes) != NULL) {
		terrorNotUniqueMeaning(ALDOR_E_TinNMeanings, absyn, okSymes,
				       allSymes, symString(sym), ctxtTForm(tfc));
		listFree(Syme)(okSymes);
		listFree(Syme)(fsymes);
		return false;
	}

	syme = car(okSymes);
	listFree(Syme)(okSymes);
	listFree(Syme)(fsymes);

	if (abState(what) == AB_State_HasPoss)
		tpossFree(abTPoss(what));

	stabSetSyme(stab, what,	 syme, abCondKnown);
	stabSetSyme(stab, absyn, syme, abCondKnown);
	abTUnique(what)	 = symeType(syme);
	abTUnique(absyn) = symeType(syme);
	abState(what) = AB_State_HasUnique;
	return true;
}

/***************************************************************************
 *
 * :: CoerceTo:	 x :: T
 *
 ***************************************************************************/

local Bool
titdnCoerceTo(Stab stab, AbSyn absyn, TFContext tfc)
{
	TForm	tf = tiGetTForm(stab, absyn->abCoerceTo.type);

	if (!tfSatReturn(tf, ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
				    absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}
 	titdn0ApplySym(stab, absyn, ctxtCopy(tfc, tf), ssymCoerce, 1, abArgf, NULL);
	abTUnique(absyn) = tf;
	return true;
}

/****************************************************************************
 *
 * :: RestrictTo:   A @ B
 *
 ***************************************************************************/


local Bool
titdnRestrictTo(Stab stab, AbSyn absyn, TFContext tfc)
{
	TForm tf = tiGetTForm(stab, absyn->abRestrictTo.type);

	if (!tfSatReturn(tf, ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
			            absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}
	titdn(stab, absyn->abRestrictTo.expr, ctxtCopy(tfc, tf));
	abTUnique(absyn) = tf;
	return true;
}


/****************************************************************************
 *
 * :: PretendTo:   A pretend B
 *
 ***************************************************************************/

local Bool
titdnPretendTo(Stab stab, AbSyn absyn, TFContext tfc)
{
	TForm tf = tiGetTForm(stab, absyn->abPretendTo.type);

	if (!tfSatReturn(tf, ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
				    absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}
	titdn(stab, absyn->abPretendTo.expr, ctxtCopy(tfc, tfUnknown));
	if (!tfIsMulti(ctxtTForm(tfc)) && tfIsMulti(abTUnique(absyn->abPretendTo.expr))) {
		abSetTContext(absyn->abPretendTo.expr, AB_Embed_MultiToCross);
	}

	abTUnique(absyn) = tf;
	return true;
}

/***************************************************************************
 *
 * :: Not: not a
 *
 ***************************************************************************/

local Bool
titdnNot(Stab stab, AbSyn absyn, TFContext tfc)
{
	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	assert(tfBoolean != tfUnknown);

	if (!tfSatReturn(tfBoolean, ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
				    absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}
	titdn(stab, absyn->abNot.expr, ctxtCopy(tfc, tfBoolean));
	abTUnique(absyn) = tfBoolean;
	return true;
}

/***************************************************************************
 *
 * :: And: a and b and c ...
 *
 ***************************************************************************/

local Bool
titdnAnd(Stab stab, AbSyn absyn, TFContext tfc)
{
	int	i;
	int	argc = abArgc(absyn);
	AbLogic *saveCond = (AbLogic*) stoAlloc(OB_Other, sizeof(AbLogic) * argc);

	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	assert(tfBoolean != tfUnknown);

	if (!tfSatReturn(tfBoolean, ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
				    absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}

	for (i = 0; i < argc; i++) {
		titdn(stab, abArgv(absyn)[i], ctxtCopy(tfc, tfBoolean));
		ablogAndPush(&abCondKnown, &saveCond[i], abArgv(absyn)[i], true);
	}
	for (i = 0; i < argc; i++) {
		ablogAndPop(&abCondKnown, &saveCond[argc-i-1]);
	}
	abTUnique(absyn) = tfBoolean;
	stoFree(saveCond);
	return true;
}

/***************************************************************************
 *
 * :: Or: a or b or c ...
 *
 ***************************************************************************/


local Bool
titdnOr(Stab stab, AbSyn absyn, TFContext tfc)
{
	int	i;

	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	assert(tfBoolean != tfUnknown);

	if (!tfSatReturn(tfBoolean, ctxtTForm(tfc))) {
		terrorNotUniqueType(ALDOR_E_TinExprMeans,
				    absyn, ctxtTForm(tfc), abTPoss(absyn));
		return false;
	}
	for (i = 0; i < abArgc(absyn); i++)
		titdn(stab, abArgv(absyn)[i], ctxtCopy(tfc, tfBoolean));
	abTUnique(absyn) = tfBoolean;
	return true;
}

/***************************************************************************
 *
 * :: Assert:
 *
 ***************************************************************************/

local Bool
titdnAssert(Stab stab, AbSyn absyn, TFContext tfc)
{
	Bool ok;

	/* Ensure that the context requires no value */
	ok = titdn0NoValue(stab, absyn, tfc, ALDOR_E_TinContextAssert);

	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */
	assert(!ok || (tfBoolean != tfUnknown));

	if (ok) titdn(stab, absyn->abAssert.test, ctxtCopy(tfc, tfBoolean));
	return ok;
}

/***************************************************************************
 *
 * :: Blank:
 *
 ***************************************************************************/

local Bool
titdnBlank(Stab stab, AbSyn absyn, TFContext tfc)
{
	abTUnique(absyn) = ctxtTForm(tfc);
	return true;
}

/***************************************************************************
 *
 * :: Builtin:
 *
 ***************************************************************************/

local Bool
titdnBuiltin(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Default:
 *
 ***************************************************************************/

local Bool
titdnDefault(Stab stab, AbSyn absyn, TFContext tfc)
{
	TPoss	abtposs = abTPoss(absyn);
	TForm	rtype;

	rtype = tpossSelectSatisfier(abtposs, ctxtTForm(tfc));
	if (!rtype) return false;

	abTUnique(absyn) = rtype;
	return true;
}

/***************************************************************************
 *
 * :: Delay:
 *
 ***************************************************************************/

local Bool
titdnDelay(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Do:
 *
 ***************************************************************************/

local Bool
titdnDo(Stab stab, AbSyn absyn, TFContext tfc)
{
	Bool ok;

	/* Ensure that the context requires no value */
	ok = titdn0NoValue(stab, absyn, tfc, ALDOR_E_TinContextDo);
	return titdn0Generic(stab, absyn, ctxtCopy(tfc, tfNone()));
}

/***************************************************************************
 *
 * :: Except:
 *
 ***************************************************************************/

local Bool
titdnExcept(Stab stab, AbSyn absyn, TFContext tfc)
{
	titdn(stab, absyn->abExcept.except, ctxtCopy(tfc, tfTuple(tfCategory)));
	if (!titdn(stab, absyn->abExcept.type, tfc))
		return false;
	if (abState(absyn->abExcept.type) != AB_State_HasUnique)
		return false;
	abTUnique(absyn) = abTUnique(absyn->abExcept.type);
	return true;
}

/***************************************************************************
 *
 * :: Raise:
 *
 ***************************************************************************/

local Bool
titdnRaise(Stab stab, AbSyn absyn, TFContext tfc)
{
	TForm tf;
	Sefo sef;
	titdn(stab, absyn->abRaise.expr, ctxtCopy(tfc, tfDomain));
	tf = tiGetTForm(stab, absyn->abRaise.expr);
	sef = tfGetExpr(tf);
	assert(sef);
	tf = tfExcept(tfExit, abGetCategory(sef));
	if (!tfSatReturn(tf, ctxtTForm(tfc))) {
		/* !!This is the _wrong_ routine to call */
		terrorNotUniqueType(ALDOR_E_TinExprMeans, absyn, ctxtTForm(tfc), 
				    tpossSingleton(tf));
		return false;
	}
	abTUnique(absyn) = tf;
	return true;
}

/***************************************************************************
 *
 * :: Export:
 *
 ***************************************************************************/

local Bool
titdnExport(Stab stab, AbSyn absyn, TFContext tfc)
{
	abTUnique(absyn) = tfNone();
	return true;
}

/***************************************************************************
 *
 * :: Extend:
 *
 ***************************************************************************/

local Bool
titdnExtend(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Fix:
 *
 ***************************************************************************/

local Bool
titdnFix(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Fluid:
 *
 ***************************************************************************/

local Bool
titdnFluid(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Free:
 *
 ***************************************************************************/

local Bool
titdnFree(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Has:
 *
 ***************************************************************************/

local Bool
titdnHas(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Hide:
 *
 ***************************************************************************/

local Bool
titdnHide(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: IdSy:
 *
 ***************************************************************************/

local Bool
titdnIdSy(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Local:
 *
 ***************************************************************************/

local Bool
titdnLocal(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Macro:
 *
 ***************************************************************************/

local Bool
titdnMacro(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: MLambda:
 *
 ***************************************************************************/

local Bool
titdnMLambda(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Nothing:
 *
 ***************************************************************************/

local Bool
titdnNothing(Stab stab, AbSyn absyn, TFContext tfc)
{
	abTUnique(absyn) = tfNone();
	return true;
}

/***************************************************************************
 *
 * :: Quote:
 *
 ***************************************************************************/

local Bool
titdnQuote(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}

/***************************************************************************
 *
 * :: Reference: ref id
 *
 ***************************************************************************/

local Bool
titdnReference(Stab stab, AbSyn absyn, TFContext tfc)
{
	AbSyn	body = absyn -> abReference.body;
	TForm	inner;


	/* What is the argument to the reference? */
	if (tfIsReference(ctxtTForm(tfc)))
		inner = tfReferenceArg(ctxtTForm(tfc));
	else
		inner = tfUnknown;


	/* Continue type inference to the leaves */
	titdn(stab, body, ctxtCopy(tfc, tfUnknown));


	/* Return now if an error has occurred */
	if (abState(absyn) == AB_State_Error) return false;


	/* Fix our type */
	abTUnique(absyn) = tfReference(inner);
	return true;
}

/***************************************************************************
 *
 * :: Select:
 *
 ***************************************************************************/

local Bool
titdnSelect(Stab stab, AbSyn absyn, TFContext tfc)
{
	Scope("titdnSelect");
	TForm		fluid(tuniExitTForm);
	AbSynList	fluid(abExitsList);
	AbSyn		fluid(tuniTdnSelectObj);
	AbSyn		seq;
	Bool		result;

	tuniExitTForm    = ctxtTForm(tfc);
	abExitsList      = listNil(AbSyn);
	tuniTdnSelectObj = absyn->abSelect.testPart;
	
	titdn(stab, absyn->abSelect.testPart, ctxtCopy(tfc, tfUnknown));

	seq = absyn->abSelect.alternatives;
	
	result = titdnSequence0(stab, absyn->abSelect.alternatives, tfc);

	if (result) 
		abTUnique(absyn) = abTUnique(absyn->abSelect.alternatives);
	Return(result);
}

/***************************************************************************
 *
 * :: Try:
 *
 ***************************************************************************/

local Bool
titdnTry(Stab stab, AbSyn absyn, TFContext tfc)
{
	AInt	i, argc;
	TForm	inner;
	AbEmbed	embed;
	AbSyn	expr, seq, nuttin, *argv;
	TForm   type = ctxtTForm(tfc);
	extern void tibup(Stab, AbSyn, TForm);

	
	/* Try-blocks with no value are tricky */
	if (tfIsNone(type)) {
		/* (void)fprintf(dbOut, "*** Ick: empty multi.\n"); */
		expr = absyn->abTry.expr;


		/* We want to force a void return value */
		if(abHasTag(expr, AB_Sequence)) {
			argc = abArgc(expr);
			argv = abArgv(expr);
		}
		else {
			argc = 1;
			argv = &expr;
		}


		/* Create a new sequence */
		seq = abNewEmpty(AB_Sequence, argc + 1);


		/* Copy across the old members */
		for (i = 0;i < argc; i++)
			abArgv(seq)[i] = argv[i];


		/* Create a void value and type infer */
		nuttin = abNewNothing(abPos(expr));
		tibup(stab, nuttin, tfNone());


		/* Append the void to the sequence */
		abArgv(seq)[argc] = nuttin;


		/* We require the type to be () */
		abUse(seq)   = abUse(expr);
		abState(seq) = AB_State_HasPoss;
		abTPoss(seq) = tpossSingleton(tfNone());


		/* Update the try-expression */
		absyn->abTry.expr = seq;
	}
	else if (tfIsMulti(type)) {
		/* (void)fprintf(dbOut, "*** Yum: hot-cross multi!\n"); */
		type = tfCrossFrMulti(type);
	}


	/* Finish the type inference on this node */
	titdn(stab, absyn->abTry.id, ctxtCopy(tfc, tfUnknown));
	titdn(stab, absyn->abTry.expr, ctxtCopy(tfc, tfIgnoreExceptions(type)));
	titdn(stab, absyn->abTry.always, ctxtCopy(tfc, tfNone()));
	if (!abIsNothing(absyn->abTry.except))
		titdn(stab, absyn->abTry.except, ctxtCopy(tfc, type));


	/* Embed multi-valued try-blocks in a Cross */
	if (abState(absyn->abTry.expr) == AB_State_HasUnique) {
		inner = abTUnique(absyn->abTry.expr);

		if (tfAsMultiArgc(inner) > 1) {
			embed = tfSatEmbedType(inner, type);
			if (!tfIsNone(type) && (embed != AB_Embed_Identity))
				abAddTContext(absyn->abTry.expr, embed);
		}
	}

	abTUnique(absyn) = type;
	return true;
}

/***************************************************************************
 *
 * :: Let:
 *
 ***************************************************************************/

local Bool
titdnLet(Stab stab, AbSyn absyn, TFContext tfc)
{
	return titdn0Generic(stab, absyn, tfc);
}


/**************************************************************************
 * titdnError: permforms a recursive discendent visit of parse tree
 * looking for nodes with state == AB_State_Error and giving error
 * message.
 * Note: every error find in bup process should be handled here.
 **************************************************************************/

local void	titdn0ErrorSequence(Stab stab, AbSyn ab, TFContext tfc);

/* Call titdnError on each subtree using .type. as constraint type */
local void
titdn0Error(Stab stab, AbSyn absyn, TFContext tfc)
{
	int	i;
	int argc = abArgc(absyn);

	for (i = 0; i < argc; i++) {
		AbSyn	argi = abArgv(absyn)[i];
		titdnError(stab, argi, tfc);
	}
}

local void
titdnError(Stab stab, AbSyn absyn, TFContext tfc)
{
	tfc = ctxtCopy(tfc, tfFollowFn(ctxtTForm(tfc)));

	if (!abIsLeaf(absyn) && abStab(absyn))
		stab = abStab(absyn);

	if (abState(absyn) == AB_State_Error) {
		Bool	exit = true;
		Bool	result = false;

		if (abTag(absyn) == AB_Sequence)
			result = titdnSequence(stab, absyn, tfc);
		else if (abIsAnyLambda(absyn))
			result = titdnLambda(stab, absyn, tfc);
		else if (abTag(absyn) == AB_Generate)
			result = titdnGenerate(stab, absyn, tfc);
		else
			exit = !terror(stab, absyn, ctxtTForm(tfc));

		/*assert(!result);*/
		if (result) abState(absyn) = AB_State_HasUnique;

		if (exit) return;
	}

	if (!abIsLeaf(absyn)) {

			/* Determine the constraint types */
		switch(abTag(absyn)) {
		case AB_Declare:
			{
			TForm tf = abTForm(absyn->abDeclare.type);
			titdnError(stab, absyn->abDeclare.id, ctxtCopy(tfc, tf));
		}
			break;
		case AB_Lambda:
		case AB_PLambda: {
			TForm	tf    = tiGetTForm(stab,absyn->abLambda.rtype);
			titdnError(stab, absyn->abLambda.param, ctxtCopy(tfc, tfUnknown));
			titdnError(stab, absyn->abLambda.body,  ctxtCopy(tfc, tf));
		}
			break;
		case AB_With:
			titdnError(stab,absyn->abWith.base, ctxtCopy(tfc, tfCategory));
			titdnError(stab,absyn->abWith.within, ctxtCopy(tfc, tfUnknown));
			break;
		case AB_Where:
			titdnError(stab,absyn->abWhere.context, ctxtCopy(tfc, tfUnknown));
			titdnError(stab,absyn->abWhere.expr, tfc);
			break;
		case AB_If:
			assert(tfBoolean != tfUnknown);
			titdnError(stab, absyn->abIf.test,     ctxtCopy(tfc, tfBoolean));
			titdnError(stab, absyn->abIf.thenAlt,  tfc);
			titdnError(stab, absyn->abIf.elseAlt,  tfc);
			break;
		case AB_RestrictTo: {
			TForm tf = tiGetTForm(stab, absyn->abRestrictTo.type);
			titdnError(stab, absyn->abRestrictTo.expr, ctxtCopy(tfc, tf));
		}
			break;
		case AB_For:
			titdnError(stab, absyn->abFor.whole, ctxtCopy(tfc, tfUnknown));
			assert(tfBoolean != tfUnknown);
			titdnError(stab, absyn->abFor.test, ctxtCopy(tfc, tfBoolean));
			titdnError(stab, absyn->abFor.lhs, ctxtCopy(tfc, tfUnknown));
			break;
		case AB_ForeignImport:
			titdnError(stab, absyn->abForeignImport.what, ctxtCopy(tfc, tfUnknown));
			break;
		case AB_ForeignExport:
			titdnError(stab, absyn->abForeignExport.what, ctxtCopy(tfc, tfUnknown));
			break;
		case AB_Import:
			titdnError(stab, absyn->abImport.what, ctxtCopy(tfc, tfUnknown));
			break;
		case AB_Inline:
			titdnError(stab, absyn->abInline.what, ctxtCopy(tfc, tfUnknown));
			break;
		case AB_Sequence:
			titdn0ErrorSequence(stab, absyn, tfc);
			break;
		case AB_PretendTo:
		case AB_Do:
		case AB_Repeat:
		case AB_Collect:
		case AB_Generate:
		case AB_Return:
		case AB_Exit:
		case AB_Comma:
		case AB_Apply:
			titdn0Error(stab, absyn, ctxtCopy(tfc, tfUnknown));
			break;
		case AB_Not:
		case AB_And:
		case AB_Or:
		case AB_Has:
		case AB_While:
			assert(tfBoolean != tfUnknown);
			titdn0Error(stab, absyn, ctxtCopy(tfc, tfBoolean));
			break;
		default:
			titdn0Error(stab, absyn, tfc);
			break;
		}
	}
	return;
}


/* NB: We should also handle 'exit' cases, and similar with
 * generate and return (probably)
 */
local void
titdn0ErrorSequence(Stab stab, AbSyn absyn, TFContext tfc)
{
	TForm  none = tfNone();
	int	i;
	int argc = abArgc(absyn);

	if (argc == 0)
		return;

	for (i = 0; i < argc-1; i++) {
		AbSyn	argi = absyn->abSequence.argv[i];
		titdnError(stab, argi, ctxtCopy(tfc, none));
	}
	titdnError(stab, absyn->abSequence.argv[i], tfc);
}
