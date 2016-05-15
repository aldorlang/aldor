/****************************************************************************
 *
 * terror.c: Type errors.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ***************************************************************************/

#include "debug.h"
#include "format.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "terror.h"
#include "util.h"
#include "sefo.h"
#include "lib.h"
#include "tconst.h"
#include "tposs.h"
#include "tfsat.h"
#include "freevar.h"
#include "absub.h"
#include "ablogic.h"
#include "abpretty.h"
#include "comsg.h"
#include "strops.h"
#include "table.h"

extern void		tiBottomUp		(Stab, AbSyn, TForm);
extern void		tiTopDown		(Stab, AbSyn, TForm);

/******************************************************************************
 *
 * :: Type form error rejection analysis structure.
 *
 *****************************************************************************/

struct treject {
	int		why;
	Syme		syme;
	TForm		tf;
	Length		parN;
	Length		argN;
};

typedef struct treject * TReject;

struct trejectInfo {
	TReject * argv;
	Length 	  argc;
	Length	  i;
};

typedef struct trejectInfo * TRejectInfo;

#define			trCurrent(trInfo)	(trInfo->argv[trInfo->i])
#define			trFirst(trInfo)		(trInfo->argv[0])
#define			trInfoGet(trInfo,i)	(trInfo->argv[i])
#define			trInfoArgc(trInfo)	(trInfo->argc)

#define			trWhy(tr)		((tr)->why)
#define			trSyme(tr)		((tr)->syme)
#define			trType(tr)		((tr)->tf)
#define			trParN(tr)		((tr)->parN)
#define			trArgN(tr)		((tr)->argN)

local TReject		trAlloc			(Syme, TForm);
local void		trFree			(TReject);

local void		trInfoFrSymes		(TRejectInfo, SymeList);
local void		trInfoFrTPoss		(TRejectInfo, TPoss);
local void		trInfoFrTUnique		(TRejectInfo, TForm);

local void bputCondition      (Buffer buf, SefoList conds);
local void terrorPrintSymeList(Buffer obuf, String prefix, SymeList msymes);
local void terrorPutConditionallyDefinedExports(Buffer obuf, Stab stab, SymeList mods, AbSyn ab, SymeList symes);

/**************************************************************************
 * TReject / TRejectInfo utility
 **************************************************************************/

local TReject
trAlloc(Syme syme, TForm tf)
{
	TReject		tr;

	tfFollow(tf);
	tr = (TReject) stoAlloc((unsigned) OB_Other, sizeof(*tr));

	tr->syme	= syme;
	tr->tf		= tf;
	tr->parN	= 0;
	tr->argN	= 0;

	return tr;
}

local void
trFree(TReject tr)
{
	stoFree((Pointer) tr);
}

local void
trInfoFrSymes(TRejectInfo trInfo, SymeList symes)
{
	TReject *	trArr;
	Length		nsymes = listLength(Syme)(symes);
	Length 		i = 0;


	trArr =  (TReject *) stoAlloc((unsigned) OB_Other,
				      sizeof(TReject) * nsymes);

	for (; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		TForm	type = symeType(syme);

		trArr[i++] = trAlloc(syme, type);
	}
	assert(i == nsymes);
	trInfo->argv = trArr;
	trInfo->argc = nsymes;
}

local void
trInfoFrTPoss(TRejectInfo trInfo, TPoss tp)
{
	TReject *	trArr;
	Length		ntposs = tpossCount(tp);
	Length 		i = 0;
	TPossIterator	it;

	trArr = (TReject *) stoAlloc((unsigned) OB_Other,
				 sizeof(TReject) * ntposs);

	for (tpossITER(it, tp); tpossMORE(it); tpossSTEP(it)) {
		TForm	type = tpossELT(it);

		trArr[i++] = trAlloc(NULL, type);
	}
	assert(i == ntposs);
	trInfo->argv = trArr;
	trInfo->argc = ntposs;
}

local void
trInfoFrTUnique(TRejectInfo trInfo, TForm tf)
{
	TReject * trArr = (TReject *) stoAlloc((unsigned) OB_Other,
					       sizeof(TReject));

	*trArr = trAlloc(NULL, tf);

	trInfo->argv = trArr;
	trInfo->argc = 1;
}

local void
trInfoFree(TRejectInfo trInfo)
{
	Length i;

	for (i = 0; i < trInfo->argc; i++)
		trFree(trInfo->argv[i]);

	stoFree(trInfo->argv);
}

/***********************************************************************
 * Sorting treject for generic apply
 ***********************************************************************/

local int
trejectCmpPtr(TReject *ptr1, TReject *ptr2)
{
	if (trWhy(*ptr1) < trWhy(*ptr2))
		return -1;
	else if (trWhy(*ptr1) > trWhy(*ptr2))
		return 1;
	else if (trWhy(*ptr1) == TR_BadArgType) {
		if (trArgN(*ptr1) < trArgN(*ptr2))
			return -1;
		else if (trArgN(*ptr1) > trArgN(*ptr2))
			return 1;
		else
			return 0;
	}	
	else
		return 0;
}

/* Sort the treject vector accordind to following rules:
 *  - the order is BadArgType, ParMissing, EmbedFail, BadFnType
 *  - for BadArgType the order is ascendent on the argument number
 */
local void
sortTRejectInfo(TRejectInfo trInfo)
{
	lisort(trInfo->argv, trInfo->argc, sizeof(TReject),
		       (int (*)(ConstPointer, ConstPointer)) trejectCmpPtr);
}

/***********************************************************************
 * Sorting treject for set!
 ***********************************************************************/

/* Gives an ordering for use in setbang.
 * the order is:
 *   - BadFnType,BadArgType,ParMissing,EmbedFail
 *   - for BadArgType the order is discendent on the arg number
 */
local int
trejectSetBangCmpPtr(TReject *ptr1, TReject *ptr2)
{
	if (trWhy(*ptr1) < trWhy(*ptr2))
		return -1;
	else if (trWhy(*ptr1) > trWhy(*ptr2))
		return 1;
	else if (trWhy(*ptr1) == TR_BadArgType) {
		if (trArgN(*ptr1) < trArgN(*ptr2))
			return 1;
		else if (trArgN(*ptr1) > trArgN(*ptr2))
			return -1;
		else
			return 0;
	}	
	else
		return 0;
}

local void
sortSetBangTRejectInfo(TRejectInfo trInfo)
{
	lisort(trInfo->argv, trInfo->argc, sizeof(TReject),
		   (int (*)(ConstPointer,ConstPointer)) trejectSetBangCmpPtr);
}

/******************************************************************************
 *
 * :: Local declarations.
 *
 *****************************************************************************/

#define INDENT	2
#define CLIP	65

local long	terrorClip = CLIP;
local void	operatorErrMsg  (AbSyn, AbSyn, Buffer);
local String	fmtAbSyn	(AbSyn);
local String	fmtTForm	(TForm);

local void	bputMeanings	(Buffer, TPoss);
local void	bputSymes	(Buffer, SymeList, String);
local void   	bputSyme	(Buffer, Syme, TForm, String);
local void	bputType	(Buffer, TForm, String);
local void 	bputTForm	(Buffer, TForm);
local void 	bputTPoss	(Buffer, TPoss);
local void	bputTPoss0(Buffer buf, int indent, TPoss tp);
local void 	bputAbTPoss (Buffer obuf, int indent, AbSyn ab, Msg, Msg);
local void	bputContextType(Buffer obuf, TForm type);
local void 	bputTConst	(Buffer, TConst);
local void	bputTReject	(Buffer, TReject, String);

local void bputAllValidMeanings(Buffer obuf, Stab stab, AbSyn ab, Length argc,
		    AbSynGetter argf, TForm tf, TForm type, Symbol idSym,
		    Bool * firstMean, String fmtOp);
local Bool bputMeaningsOutOfScope(Buffer obuf, Stab stab, AbSyn ab,AbSyn op,
				  Length argc,AbSynGetter argf, TForm type,
				  String fmtOp);
local void bputBadArgType(TRejectInfo trInfo, Buffer obuf, AbSyn ab,
			  Length argc, AbSynGetter argf, String fmtOp);
local void bputParMissing(TRejectInfo trInfo, Buffer obuf, String fmtOp);
local void bputEmbedFail(TRejectInfo trInfo, Buffer obuf, int argc,
			 String fmtOp);
local void bputBadFnType(TRejectInfo trInfo, Buffer obuf, TForm type, String fmtOp);
local void analyseRejectionCause(TReject tr, Stab stab, AbSyn ab, Length argc,
		      AbSynGetter argf, TForm type);
local void fillTRejectInfo(TRejectInfo trInfo, TForm type, AbSyn ab, Stab stab,
		Length argc, AbSynGetter argf);
local void noMeaningsForOperator(Buffer obuf, TForm type, AbSyn ab, AbSyn op,
				 Stab stab, Length argc, AbSynGetter argf,
				 String fmtOp);

local void bputFirstExitTypes(AbSyn ab, String kind);
local void bputOtherExitTypes(AbSyn ab, String strKind);

local void terrorSequence	(Stab, AbSyn, TForm);
local void terrorCoerceTo	(Buffer, AbSyn , TForm);
local void terrorApply 		(Stab stab, AbSyn absyn, TForm type);
local void terrorImplicit 	(Stab stab, AbSyn absyn, TForm type);
local void terrorImplicitSetBang(Stab stab, AbSyn ab, Length argc, AbSynGetter argf, TForm type);
local Bool terrorAssignOrSetBang(Stab stab, AbSyn absyn, TForm type);
local void terrorNoMeaningForLit(AbSyn ab);
local Bool terrorIllegalDepAssign(int, AbSyn *, TForm *);

extern AbSynList abExitsList;
extern AbSynList abYieldsList;
extern AbSynList abReturnsList;


/****************************************************************************
 *
 * :: terror (main external entry point)
 *
 ****************************************************************************/
/* Called on a node with abstate = State_Error
 * Return false if the subtree needs NO more to be examined 
 */
Bool
terror (Stab stab, AbSyn absyn, TForm type)
{
	Bool result = true;


	/* Deal with the disaster scenario */
	if (abState(absyn) == AB_State_HasUnique) {
		bugWarning("terror: absyn has unique type!");
		abState(absyn) = AB_State_HasPoss;
		abTPoss(absyn) = tpossSingleton(abTUnique(absyn));
	}

	/* gives error msg */
	
	switch (abTag(absyn)) {
	case AB_Id: {
		Symbol	sym  = absyn->abId.sym;
		terrorNoMeaningForId(absyn,symString(sym));
	}
		break;

	case AB_LitInteger:
	case AB_LitFloat:
	case AB_LitString:
		terrorNoMeaningForLit(absyn);
		break;

	case AB_Assign:
		result = terrorAssignOrSetBang(stab, absyn, type);
		break;

	case AB_Add:  /* NB: We may have an error other than
		       * "insufficient exports.  see call to tfSatisfies
		       * in tiBupAdd
		       * Solution is to call tiAddSymes to see which error 
		       * occurred.
		       */
		assert(abState(absyn) != AB_State_HasUnique);
		terrorNotEnoughExports(stab, absyn, abTPoss(absyn), false);
		tpossFree(abTPoss(absyn));
		abTPoss(absyn) = tpossEmpty();
		break;

	case AB_Apply:
		terrorApply(stab, absyn, type);
		break;

	case AB_Sequence:
		terrorSequence(stab, absyn, type);
		break;

	case AB_Generate:
	case AB_Lambda:
	case AB_PLambda:
		break;

	case AB_Return:
		assert(abState(absyn) != AB_State_HasUnique);
		if (tuniIsInappropriate(abTPoss(absyn))) {
			comsgError(absyn, ALDOR_E_TinWildReturn);
			abTPoss(absyn) = NULL;
		}
		else 
			abReturnsList = listCons(AbSyn)(absyn->abExit.value,abReturnsList);
		result = false;
		break;

	case AB_Yield:
		assert(abState(absyn) != AB_State_HasUnique);
		if (tuniIsInappropriate(abTPoss(absyn))) {
			comsgError(absyn, ALDOR_E_TinWildYield);
			abTPoss(absyn) = NULL;
		}
		else 
			abYieldsList = listCons(AbSyn)(absyn->abExit.value,abYieldsList);
		result = false;
		break;

	case AB_Exit:
		assert(abState(absyn) != AB_State_HasUnique);
		if (tuniIsInappropriate(abTPoss(absyn))) {
			comsgError(absyn, ALDOR_E_TinWildExit);
			abTPoss(absyn) = NULL;
		}
		else 
			abExitsList = listCons(AbSyn)(absyn->abExit.value,abExitsList);
		result = false;
		break;

	case AB_Goto:
		abTPoss(absyn) = NULL;
		comsgError(absyn->abGoto.label, ALDOR_E_TinBadGoto);
		result = false;
		break;

	default:
		if (abImplicit(absyn))
			terrorImplicit(stab, absyn, type);
		else
			terrorNotUniqueType(ALDOR_E_TinExprMeans,absyn,type,
					    abTPoss(absyn));
	}

	if ((abState(absyn) != AB_State_HasUnique) && abGoodTPoss(absyn))
		tpossFree(abTPoss(absyn));
	abState(absyn) = AB_State_HasPoss;
	abTPoss(absyn) = tpossEmpty();
	
	return result;
}

/****************************************************************************
 *
 * :: terrorAssignOrSetBang
 *
 ****************************************************************************/


local Bool
terrorAssignOrSetBang (Stab stab, AbSyn absyn, TForm type)
{
	Bool result = false;
	AbSyn lhs = absyn->abAssign.lhs;
	AbSyn rhs = absyn->abAssign.rhs;

	if (abTag(lhs) == AB_Apply) {
		Length argc = abArgc(lhs)+1;
		Length i;

		terrorImplicitSetBang(stab, absyn, argc, abSetArgf, type);

		for (i = 0 ; i < argc ; i++) {
			AbSyn argi = abSetArgf(absyn,i);

			/* Deal with the easy case */
			if (abTag(argi) == AB_Id) {
			 	if (abState(argi) == AB_State_Error) {
					abState(argi) = AB_State_HasPoss;
					abTPoss(argi) = tpossEmpty();
				}
				else /* Pass the buck onto the caller */
					result = true;
			}
		}
	}
	else if (abTag(lhs) == AB_Comma && tpossIsUnique(abGoodTPoss(rhs))) {
		AbSyn	*lhsv  = 0;
		AbSyn   *rhsv  = 0;
		TForm	*trhsv = 0, trhs;
		int	lhsc   = 0, i;

		trhs = tpossUnique(abTPoss(rhs));
		if (tfIsUnknown(trhs)) {
			String	msg = "bad case 1 in terrorAssignOrSetBang";
			comsgFatal(absyn, ALDOR_F_Bug, msg);
			/* bug("Bad case 1 in terrorAssignOrSetBang");*/
		}

		lhsc = abArgc(lhs);
		lhsv = abArgv(lhs);
		
		if (tfIsCross(trhs) && tfCrossArgc(trhs) == lhsc) {
			trhsv = tfArgv(trhs);
			rhsv = abArgv(rhs);
		}
		else if (tfIsMulti(trhs) && tfMultiArgc(trhs) == lhsc) {
			trhsv = tfArgv(trhs);
			rhsv = abArgv(rhs);
		}
		else {
			comsgError(rhs, ALDOR_E_TinCantSplitRHS);
			return false;
		} 
		if (terrorIllegalDepAssign(lhsc, lhsv, trhsv))
			return false;
		
		for (i = 0; i < lhsc; i++) {
			if (!tpossSelectSatisfier(abTPoss(lhsv[i]), 
						  trhsv[i])) {
				AbSyn fake = abNewNothing(abPos(lhsv[i]));
				AbSyn ab = abNewAssign(abPos(lhsv[i]), 
						       lhsv[i], fake);
				abTPoss(fake) = tpossSingleton(trhsv[i]);
				abState(fake) = AB_State_HasPoss;
				terrorAssignOrSetBang(stab, ab, trhsv[i]);
				tpossFree(abTPoss(fake));
				tpossFree(abTPoss(ab));
				abFree(fake);
				abFreeNode(ab);
			}	
			if (abState(lhsv[i]) == AB_State_Error ||
			    abState(rhsv[i]) == AB_State_Error) {
				AbSyn	fake  = abNewNothing(abPos(lhs));
				AbSyn	ab = abNewAssign(abPos(lhs), lhsv[i],
							 fake);
				abTPoss(fake) = tpossSingleton(trhsv[i]);
				abState(fake) = AB_State_HasPoss;
				/* ?? SetBang ?? */
				terrorSetBang(stab, ab, abArgc(lhsv[i])+1,
					      abSetArgf);
						
				tpossFree(abTPoss(fake));
				tpossFree(abTPoss(ab));
				abFree(fake);
				abFreeNode(ab);
			}
		}
	}
	
	else {
		TPoss tp = abTPoss(absyn);
		terrorAssign(absyn, type, tp);

		/* Deal with the easy LHS case */
		if (abTag(lhs) == AB_Id) {
		 	if (abState(lhs) == AB_State_Error) {
				abState(lhs) = AB_State_HasPoss;
				abTPoss(lhs) = tpossEmpty();
			}
			else /* Pass the buck onto the caller */
				result = true;
		}


		/* Deal with the easy RHS case */
		if (abTag(rhs) == AB_Id) {
		 	if (abState(rhs) == AB_State_Error) {
				abState(rhs) = AB_State_HasPoss;
				abTPoss(rhs) = tpossEmpty();
			}
			else /* Pass the buck onto the caller */
				result = true;	
		}
	}

	return result;
}

local Bool
terrorIllegalDepAssign(int argc, AbSyn *lhsv, TForm *trhsv)
{
	SymeList blacklist = listNil(Syme);
	AIntList posns	   = listNil(AInt);
	SymeList symes;
	AIntList pl;

	Bool     ret = false;
	int	 i;
	for (i=0; i<argc; i++) {
		Syme rsyme = tfDefineeSyme(trhsv[i]);
		Syme lsyme = abSyme(lhsv[i]);
		TForm tf   = tfDefineeType(trhsv[i]);

		if (rsyme && symeIsLexVar(lsyme)) {
			blacklist = listCons(Syme)(rsyme, blacklist);
			posns  	  = listCons(AInt)(i, posns);
		}
		if (!tfFVars(tf))
			continue;
		
		symes = blacklist;
		pl    = posns;

		while (symes) {
			if (fvHasSyme(tfFVars(tf), car(symes))) {
			  	String name = symString(abIdSym(lhsv[car(pl)]));
			  	comsgError(lhsv[i], 
					   ALDOR_E_TinAssignCreatesDepType,
					   name);
			  	ret = true;
			}
			symes = cdr(symes);
			pl = cdr(pl);
		}
	}
	listFree(Syme)(blacklist);
	listFree(AInt)(posns);
	return ret;
}

/****************************************************************************
 *
 * :: terrorApply
 *
 ****************************************************************************/

local void
terrorApply (Stab stab, AbSyn absyn, TForm type)
{
	AbSyn op;
	TPoss opTypes;
	Length argc;
	AbSynGetter argf;
	
	if (abImplicit(absyn)) {
		op = abImplicit(absyn);
		argc = abArgc(absyn);
		argf = abArgf;
	}
	else {
		op = abApplyOp(absyn);
		argc = abApplyArgc(absyn);
		argf = abApplyArgf;	
	}
	

	if (abState(op) == AB_State_HasUnique) {
		TForm opType = abTUnique(op);

		if (abIsTheId(op, ssymJoin) &&
		    tfSatisfies(tfMapRet(opType), tfCategory)) {
			terrorNotUniqueType(ALDOR_E_TinOpMeans, absyn, type, abTPoss(op));
		}
		else
			terrorApplyFType(absyn, type, NULL, op, stab, argc, argf);
	}
	else {
		abState(op) = AB_State_HasPoss;
		opTypes = abTPoss(op);

		if (abIsTheId(op, ssymJoin) && tpossIsUnique(opTypes) &&
		    tfSatisfies(tfMapRet(tpossUnique(opTypes)), tfCategory)) {
			terrorNotUniqueType(ALDOR_E_TinOpMeans, absyn, type, abTPoss(op));
		}
		else if (tpossCount(opTypes) == 0)
			terrorMeaningsOutOfScope(stab, absyn, op, type,
						 abApplyArgc(absyn), abApplyArgf);
		else
			terrorApplyFType(absyn, type, NULL, op, stab, argc, argf);
	}
}

void
terrorIdCondition(TForm tf, AbSyn id, AbLogic cont, AbLogic cond)
{
	/*
	 * This error routine drastically needs improving.
	 * terrorPutConditionalExports() would be good here
	 * terrorTypeConstFailed() looks interesting too
	 */
	Buffer	obuf = bufNew();
	String	fmtOp = fmtAbSyn(id);

	bufPrintf(obuf, "There are no suitable meanings for `");
	bufPrintf(obuf, fmtOp);
	bufPrintf(obuf, "': it has the condition `");
	bputAblog(obuf, cond);
	bufPrintf(obuf, "' which is not satisfied by the context `");
	bputAblog(obuf, cont);
	bufPrintf(obuf, "'.");

	comsgError(id, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}

void
terrorApplyCondition(AbSyn ab, TForm tf, AbSyn op, AbLogic cont, AbLogic cond)
{
	/*
	 * This error routine drastically needs improving.
	 * terrorPutConditionalExports() would be good here
	 * terrorTypeConstFailed() looks interesting too
	 */
	Buffer	obuf = bufNew();
	String	fmtOp = fmtAbSyn(op);

	bufPrintf(obuf, "There are no suitable meanings for `");
	bufPrintf(obuf, fmtOp);
	bufPrintf(obuf, "': it has the condition `");
	bputAblog(obuf, cond);
	bufPrintf(obuf, "' which is not satisfied by the context `");
	bputAblog(obuf, cont);
	bufPrintf(obuf, "'.");

	comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}

/****************************************************************************
 *
 * :: terrorImplicit
 *
 ****************************************************************************/

local void
terrorImplicit (Stab stab, AbSyn absyn, TForm type)
{
	AbSyn		op;
	TPoss		opTypes;
	Length		argc;
	AbSynGetter	argf;
	
	op = abImplicit(absyn);
	opTypes = abTPoss(op);

	switch (abTag(absyn)) {
	case AB_For:
		argc = 1;
		argf = abForIterArgf;
		break;
	case AB_CoerceTo:
	case AB_Test:
		argc = 1;
		argf = abArgf;
		break;
	default:
		bugBadCase(abTag(absyn));
		NotReached(return);
	}
		
	if (tpossCount(opTypes) == 0)
		terrorMeaningsOutOfScope(stab, absyn, op, type, argc, argf);
	else
		terrorApplyFType(absyn, type, abTPoss(absyn),
				 op, stab, argc, argf);
}

/****************************************************************************
 *
 * :: terrorNotUniqueType
 *
 ****************************************************************************/

void
terrorNoTypes(Msg msg, AbSyn ab, TForm type, TPoss tposs)
{
	Buffer	obuf;
	String	fmt, s;


	/*
	 * Labelled expressions are slightly odd since we never
	 * bother to tinfer the label properly. Instead we just
	 * tinfer the expression and hope that is sufficient.
	 */
	while (abTag(ab) == AB_Label)
		ab = ab->abLabel.expr;


	/* Use terrorNotUnique for other error reports */
	if (abTag(ab) != AB_Sequence) {
		terrorNotUniqueType(msg, ab, type, tposs);
		return;
	}


	/*
	 * We hope that empty sequences don't need to be
	 * handled here because we have made alternative
	 * arrangements (e.g. "a value is needed but an
	 * empty sequence does not produce one").
	 */
	if (!abArgc(ab))
		return;


	terrorClip = comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;
	assert(!tpossCount(tposs));

	obuf = bufNew();
	fmt  = comsgString(msg);
	bufPrintf(obuf, fmt, int0); /* 0 types */

	if (!comsgOkDetails()) {
		/* do nothing */
	}
	else  {
		/* Problem with the final statement */
		AbSyn	abi = abArgv(ab)[abArgc(ab)-1];
		TPoss	tpi;

		if (abState(abi) != AB_State_HasUnique) {
			tpi = abTPoss(abi);
			fmt = comsgString(ALDOR_D_TinSubexprMeans);
			s   = fmtAbSyn(abi);
			bufPrintf(obuf, "\n  ");
			bufPrintf(obuf, fmt, s);
			strFree(s);

			bputMeanings(obuf, tpi);
		}

		bputContextType(obuf, type);
	}

	comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}


void
terrorNotUniqueType(Msg msg, AbSyn ab, TForm type, TPoss tposs)
{
	Buffer	obuf;
	String	fmt, s;
	Length	i, nposs;

	terrorClip = comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	nposs = tpossCount(tposs);

	obuf = bufNew();
	fmt  = comsgString(msg);
	bufPrintf(obuf, fmt, nposs);

	if (!comsgOkDetails()) {
		/* do nothing */
	}
	else if (nposs > 0) {
		bputMeanings(obuf, tposs);
		bputContextType(obuf, type);
	}
	else if (abIsLeaf(ab)) {
		if (abState(ab) == AB_State_HasPoss ||
		    abState(ab) == AB_State_Error) {
			TPoss	tp;

			tp  = abGoodTPoss(ab);
			fmt = comsgString(ALDOR_D_TinSubexprMeans);
			s   = fmtAbSyn(ab);
			bufPrintf(obuf, "\n  ");
			bufPrintf(obuf, fmt, s);
			strFree(s);

			bputMeanings(obuf, tp);
		}
		bputContextType(obuf, type);
	}
	else {
		for (i = 0; i < abArgc(ab); i++) {
			AbSyn	abi = abArgv(ab)[i];
			TPoss	tpi;

			if (abIsNothing(abi)) continue;
			if (abState(abi) != AB_State_HasPoss) continue;

			tpi = abTPoss(abi);
			fmt = comsgString(ALDOR_D_TinSubexprMeans);
			s   = fmtAbSyn(abi);
			bufPrintf(obuf, "\n  ");
			bufPrintf(obuf, fmt, s);
			strFree(s);

			bputMeanings(obuf, tpi);
		}
		bputContextType(obuf, type);
	}

	comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}

/**************************************************************************
 *
 * Sequence: (a;..;b)                                                     
 *
 **************************************************************************/
local
void terrorSequence(Stab stab, AbSyn absyn, TForm type)
{
	AbSynList abl;

	abExitsList = listNReverse(AbSyn)(abExitsList);

	for (abl = abExitsList; abl; abl = cdr(abl)) {
		if (abState(car(abl)) == AB_State_HasPoss &&
		    tpossCount(abTPoss(car(abl))) == 0)
			return;
	}

	if (!abExitsList) {
		comsgError(absyn, ALDOR_E_ExplicitMsg,
			   "Unable to determine return type");
		return;
	}

	bputFirstExitTypes(car(abExitsList), "exit");

	if (!comsgOkDetails()) return;

	for (abl = cdr(abExitsList); abl; abl = cdr(abl)) 
		bputOtherExitTypes(car(abl), "exit");

}

local void
bputFirstExitTypes(AbSyn ab, String strKind)
{
	String fmt, s;
	Buffer obuf = bufNew();


	if (abState(ab) == AB_State_HasUnique) {
		fmt = comsgString(ALDOR_E_TinFirstExitType);
		s = fmtTForm(abTUnique(ab));
		bufPrintf(obuf, fmt, strKind, s);
		strFree(s);
	}
	else if (tpossCount(abTPoss(ab)) == 1) {
		fmt = comsgString(ALDOR_E_TinFirstExitType);
		s = fmtTForm(tpossUnique(abTPoss(ab)));
		bufPrintf(obuf, fmt, strKind, s);
		strFree(s);
	}
	else {
		fmt = comsgString(ALDOR_E_TinFirstExitTypes);
		bufPrintf(obuf, fmt, strKind);
		bputTPoss(obuf, abTPoss(ab));
	}
	if (comsgOkDetails()) {
		bufPrintf(obuf, "\n");
		fmt = comsgString(ALDOR_D_TinFirstExitCant);
		bufPrintf(obuf, fmt, strKind);
	}

	comsgNError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);

}

local void
bputOtherExitTypes(AbSyn ab, String strKind)
{
	String fmt, s;
	Buffer obuf = bufNew();

	if (abState(ab) == AB_State_HasUnique) {
		fmt = comsgString(ALDOR_N_TinOtherExitType);
		s = fmtTForm(abTUnique(ab));
		bufPrintf(obuf, fmt, strKind, s);
		strFree(s);
	}
	else if (tpossCount(abTPoss(ab)) == 1) {
		fmt = comsgString(ALDOR_N_TinOtherExitType);
		s = fmtTForm(tpossUnique(abTPoss(ab)));
		bufPrintf(obuf, fmt, strKind, s);
		strFree(s);
	}
	else {
		fmt = comsgString(ALDOR_N_TinOtherExitTypes);
		bufPrintf(obuf, fmt, strKind);
		bputTPoss(obuf, abTPoss(ab));
		bufPrintf(obuf, "\n");
	}

	comsgNote(ab, ALDOR_N_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}

/**************************************************************************
 *
 * terrorCoerceTo
 *
 **************************************************************************/

local void
terrorCoerceTo(Buffer obuf, AbSyn ab, TForm type)
{
 	String fmt;

	assert(abState(ab->abCoerceTo.expr) == AB_State_HasPoss); 
	fmt = comsgString(ALDOR_E_TinNoGoodOp);
	bufPrintf(obuf, fmt, "coerce");

	if (!comsgOkDetails()) goto done;
	assert(abState(ab->abCoerceTo.expr) != AB_State_AbSyn);
	bufPrintf(obuf, "\n   ");
	fmt = comsgString(ALDOR_D_TinPossTypesLhs);
	bufPrintf(obuf, fmt);
	if (abState(ab->abCoerceTo.expr) == AB_State_HasPoss ||
	    abState(ab->abCoerceTo.expr) == AB_State_Error) {
		TPoss tpFrom = abGoodTPoss(ab->abCoerceTo.expr); 
		bputTPoss(obuf, tpFrom);	
	}
	else {
		TForm tfFrom = abTUnique(ab->abCoerceTo.expr); 
		bputTForm(obuf, tfFrom);
	}

	/* To do: "the available coerce to `type' are:..." */
done:	
	comsgError(ab->abCoerceTo.type, ALDOR_E_ExplicitMsg, bufChars(obuf));
}

/**************************************************************************
 *
 * terrorSetBang
 *
 **************************************************************************/

void
terrorSetBang(Stab stab, AbSyn ab, Length argc, AbSynGetter argf)
{
	Buffer		obuf;
	String		fmt, s, s0;
	AbSyn		rhs = ab->abAssign.rhs;
	AbSyn		argi;
	Length		i;
	AbSyn		abp = ab;

	terrorClip =  comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	obuf = bufNew();

	fmt = comsgString(ALDOR_E_TinNoGoodOp);
	bufPrintf(obuf, fmt, "set!");
	if (!comsgOkDetails()) goto done;

	bufPrintf(obuf, "\n");

/* (1) */

	for (i = 0 ; i < argc ; i++) {
		argi = argf(ab, i);
		if (tpossCount(abGoodTPoss((argi))) == 0) {
			s = fmtAbSyn(argi);
			if (abTag(argi) == AB_Id) {
				fmt = comsgString(ALDOR_D_TinNoMeaningForId);
				bufPrintf(obuf, fmt, s);
			}
			else {
				fmt=comsgString(ALDOR_D_TinNoGoodInterp);
				bufPrintf(obuf, fmt, s);
			}
			
			
			abp = argi;
			goto done;
		}
	}

/* (2) */

	for (i = 0 ; i < argc - 1 ; i++) {
		argi = argf(ab,i);
		s = fmtAbSyn(argi);
		if (abTag(argi) == AB_Id) {		     /* vvv !!FIXME */
			SymeList symes= stabGetMeanings(stab,ablogFalse(),
							argi->abId.sym);

			if (symes) {
				fmt = comsgString(ALDOR_D_TinPossInterps);
				bufPrintf(obuf, fmt, s);
				bputSymes(obuf, symes, s);
				bufPrintf(obuf, "\n");
			}
		}
		else if (abState(argi) == AB_State_HasPoss &&
	    		tpossCount(abTPoss(argi))) {
			fmt = comsgString(ALDOR_D_TinPossInterps);
			bufPrintf(obuf, fmt, s);
			bputTPoss(obuf, abTPoss(argi));
			bufPrintf(obuf, "\n");
		}
		else if (abState(argi) == AB_State_HasUnique) {
			fmt = comsgString(ALDOR_D_TinPossInterps);
			bufPrintf(obuf, fmt, s);
			bputTForm(obuf, abTUnique(argi));
			bufPrintf(obuf, "\n");
		}
		strFree(s);
	}

	if (abState(rhs) == AB_State_HasPoss &&
	    tpossCount(abTPoss(rhs))) {

		s0 = abIsNothing(rhs) ? strCopy("?")  : fmtAbSyn(rhs);
		fmt = comsgString(ALDOR_D_TinPossTypesRhs);
		bufPrintf(obuf, fmt, s0);
		bputTPoss(obuf, abTPoss(rhs));
		strFree(s0);
	}

done:	comsgError(abp, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);

}

local void
bputOthersBadArgNumber(TRejectInfo trInfo, Buffer obuf)
{
	Length i;
	String fmt;

	for ( i = 1; i < trInfo->argc ; i++)
		if (trWhy(trInfo->argv[i]) == TR_EmbedFail ||
		    trWhy(trInfo->argv[i]) == TR_ArgMissing) {
			bufPrintf(obuf, "  ");
			fmt = comsgString(ALDOR_D_TinOtherDiffArgNum);
			bufPrintf(obuf, fmt);
			bufPutc(obuf, '\n');
			break;
		}
}

local void
bputSetBangBadFnType(TRejectInfo trInfo, Buffer obuf, AbSyn ab, TForm type)
{
	TPoss retTypes = tpossEmpty();
	String fmt = comsgString(ALDOR_D_TinPossRetTypeSetBang);
	int i;

	bufPrintf(obuf, "  ");
	bufPrintf(obuf, fmt);
	for (i = 0; i < trInfo->argc && trWhy(trInfo->argv[i]) == TR_BadFnType;
	     i++) {
		TForm tfRet = tfMapRet(trType(trInfo->argv[i]));
		tfRet = tfDefineeType(tfRet);
		tpossAdd1(retTypes, tfRet);
	}

	bputTPoss(obuf, retTypes);
	bputContextType(obuf, type);
	tpossFree(retTypes);
}

local void
bputSetBangBadArgType(TReject tr, Buffer obuf, AbSyn ab, Length argc,
		      AbSynGetter argf)
{
	String fmt;
	Length argN = trArgN(tr);
	AbSyn  argErr = argf(ab, argN);
	String fmtArgErr = fmtAbSyn(argErr);

	if (argN == argc - 1)  {
		TForm type;
		bputAbTPoss(obuf, 2, argErr,
				ALDOR_D_TinRejectedTypeForRhs,
				ALDOR_D_TinRejectedTypesForRhs);
		type = tfMapArgN(trType(tr), argN);
		type = tfDefineeType(type);
		bputContextType(obuf, type);
		}
	else if (argN > 0) {
		bufPrintf(obuf, "  ");
		fmt = comsgString(ALDOR_D_TinPossSelectorTypes);
		bufPrintf(obuf, fmt, fmtArgErr);
		bputTPoss0(obuf, 4, abTPoss(argErr));
	}
	else {
		bufPrintf(obuf, "  ");
		fmt = comsgString(ALDOR_D_TinPossTypesForSetBang);
		bufPrintf(obuf, fmt, fmtArgErr);
		bputTPoss0(obuf, 4, abTPoss(argErr));
	}
	strFree(fmtArgErr);
}

local void
bputSetBangArgNumber(Buffer obuf)
{
	String fmt = comsgString(ALDOR_D_TinSetBangBadArgNum);
	bufPrintf(obuf, "  ");
	bufPrintf(obuf, fmt);
}



local void
terrorImplicitSetBang(Stab stab, AbSyn ab, Length argc, AbSynGetter argf,
		 TForm type)
{
	Buffer			obuf;
	String			fmt;
	AbSyn			abp = ab;
	struct trejectInfo	trInfoStruct;
	TReject			tr = (TReject)NULL;
	

	terrorClip =  comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	obuf = bufNew();
	trInfoStruct.i = 0;

	fmt = comsgString(ALDOR_E_TinNoGoodOp);
	bufPrintf(obuf, fmt, "set!");
	if (!comsgOkDetails()) goto done;

	bufPrintf(obuf, "\n");

/* set! take state error only if all the children have abGoodTPoss not empty.
 * Here we assume this. 
 * !!! FIXME (ablogFalse)
 */
	trInfoFrSymes(&trInfoStruct, stabGetMeanings(stab, ablogFalse(), ssymSetBang));

	fillTRejectInfo(&trInfoStruct, type, ab, stab, argc, argf);
	sortSetBangTRejectInfo(&trInfoStruct);

	if (trInfoStruct.argv) tr = trInfoStruct.argv[0];

	if (tr && trWhy(tr) == TR_BadFnType) {
		bputSetBangBadFnType(&trInfoStruct, obuf, ab, type);
		bputOthersBadArgNumber(&trInfoStruct, obuf);
	}
	else if (tr && trWhy(tr) == TR_BadArgType) {
		abp = argf(ab, trArgN(tr));
		bputSetBangBadArgType(tr, obuf, ab, argc, argf);
		bputOthersBadArgNumber(&trInfoStruct, obuf);
	}
	else    /* different parameter number */
		bputSetBangArgNumber(obuf); 

	trInfoFree(&trInfoStruct);

done:	comsgError(abp, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);

}

/**************************************************************************/
/* type const failure                                                     */
/**************************************************************************/

void
terrorTypeConstFailed(TConst tc)
{
	AbSyn	ab = tcPos(tc);
	Buffer	obuf = bufNew();

	terrorClip = comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	bufPrintf(obuf, comsgString(ALDOR_E_TinTypeConstIntro));
	bputTForm(obuf, tcOwner(tc));
	bufPrintf(obuf, "\n");
	bufPrintf(obuf, comsgString(ALDOR_X_TinTypeConstFailed));
	bputTConst(obuf, tc);

	comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}

/**************************************************************************/
/* Output for all the meanings which could be suitable if imported        */
/**************************************************************************/

local void
bputAllValidMeanings(Buffer obuf, Stab stab, AbSyn ab, Length argc,
		    AbSynGetter argf, TForm tf, TForm type, Symbol idSym,
		    Bool * firstMean, String fmtOp)
{
	String		fmt = comsgString(ALDOR_D_TinAlternativeMeanings);
	SatMask		mask = tfSatTErrorMask(), result;
	SymeList	symes;

	for (symes = tfGetDomImportsByName(tf, idSym); symes; symes = cdr(symes)) {
		Syme syme = car(symes);
		TForm opType;

		if (symeId(syme) != idSym)
			continue;

		opType = tfDefineeType(symeType(syme));
		if (!tfIsAnyMap(opType))
			continue;

		result = tfSatMap(mask, stab, opType, type, ab, argc, argf);
		if (!tfSatSucceed(result))
			continue;

		if (*firstMean) {
			bufPrintf(obuf,"\n");
			bufPrintf(obuf, fmt);
			*firstMean = false;
		}
		bputSyme(obuf, syme, opType, fmtOp);
	}
}

/*
 * Return false iif 0 meanings have been found out of current scope.
 */
local Bool
bputMeaningsOutOfScope(Buffer obuf, Stab stab, AbSyn ab,AbSyn op, Length argc,
		       AbSynGetter argf, TForm type, String fmtOp)
{
	TPoss tpArgn;
	TPoss tp = tpossEmpty();
	TPossIterator ti;
	TForm tf;
	Length i;
	Symbol idSym;
	Bool firstMean = true;

	if (abTag(op) != AB_Id) return false;  
	
	idSym = op->abId.sym;

	for (i = 0; i < argc ; i++) {   
		AbSyn abArgn = argf(ab, i);

		if (abState(abArgn) == AB_State_HasUnique)  
			tpArgn = tpossSingleton(abTUnique(abArgn));
		else
			tpArgn = abGoodTPoss(argf(ab, i));
		
		for (tpossITER(ti,tpArgn); tpossMORE(ti); tpossSTEP(ti)) {
		  tf = tpossELT(ti);

		      tf = tfDefineeType(tf);  

		      if (!tfSatType(tf) && !tpossHas(tp ,tf)) {   
				/*
				 * NOTE that tpossAdd1 calls tpossHas again;
				 * this redundat test could be skipped if
				 * tpossCons wasn't a local function.
				 */ 			      
			     	tpossAdd1(tp, tf);  
			      	bputAllValidMeanings(obuf,stab,ab,argc,
						     argf,tf,type,idSym,
						     &firstMean,fmtOp);
						    
	          } 
	     } 
		if (abState(abArgn) == AB_State_HasUnique)  
			tpossFree(tpArgn);
	}
   	tpossFree(tp); 
	return (!firstMean);
}

void
terrorMeaningsOutOfScope(Stab stab,AbSyn ab,AbSyn op,TForm type,Length argc,
			 AbSynGetter argf)
{
	Buffer 	obuf;
	String 	fmtOp;
	String	fmt = comsgString(ALDOR_E_TinNoGoodOp);

	terrorClip =  comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	fmtOp  = fmtAbSyn(abApplyOp(ab));
	obuf = bufNew();

	bufPrintf(obuf, fmt, fmtOp);

	if (comsgOkDetails())  
		bputMeaningsOutOfScope(obuf,stab,ab,op,argc,argf,type,fmtOp);

	operatorErrMsg(ab, op, obuf);
	strFree(fmtOp);				   
	bufFree(obuf);
}

/**************************************************************************/
/* Procedures handling 0 meanings for an application                      */
/**************************************************************************/

/* Prints all meanings rejected because some arg type doesn't match */
local void
bputBadArgType(TRejectInfo trInfo, Buffer obuf, AbSyn ab, Length argc,
	       AbSynGetter argf, String fmtOp) 
{
	String fmt = comsgString(ALDOR_X_TinNoArgumentMatch);
	String fmtParType;
	TForm  tf, parType, opType;
	TReject	tr;
	AbSyn   abArgi;

	for ( ; trInfo->i < trInfo->argc &&
	     trWhy(trCurrent(trInfo)) == TR_BadArgType;
	     trInfo->i++) {
		Length iargc;
		tr = trCurrent(trInfo);
		abArgi = argf(ab, trArgN(tr));

		opType = trType(tr);
		tf = tfMapArg(opType);

		bputTReject(obuf, tr, fmtOp);

		iargc = tfMapHasDefaults(opType) ? tfMapArgc(opType) : argc;
		parType = tfAsMultiArgN(tf, iargc, trParN(tr));
		fmtParType = fmtTForm(parType);
		
		/* "rejected because arg .. did not match ... */
		bufPrintf(obuf, "\n      ");
		bufPrintf(obuf, fmt, trArgN(tr)+1, fmtParType);

		strFree(fmtParType);

		if ((!tfSatType(tfDefineeType(parType)))
		    && (abTag(abArgi) == AB_Define)
	    	    && (abTag(abArgi->abDefine.lhs) == AB_Declare)) {
			String fmtId = 
  			   fmtAbSyn((abArgi->abDefine.lhs)->abDeclare.id);
			String fmt1 = comsgString(ALDOR_D_TinShouldUseDoubleEq);
			String fmtRhs = fmtAbSyn((abArgi->abDefine.rhs));
			String fmtArg = fmtAbSyn(abArgi);
			bufPrintf(obuf, "\n      ");
			bufPrintf(obuf, fmt1, fmtId, fmtRhs, fmtArg);

			strFree(fmtId);
			strFree(fmtRhs);
			strFree(fmtArg);
		}

			/* output for: "the possible types for arg n are:" */
		if (trInfo->i + 1 == trInfo->argc ||
		    trWhy(trInfo->argv[trInfo->i + 1]) != TR_BadArgType ||
		    trArgN(tr) != trArgN(trInfo->argv[trInfo->i + 1])) {
			
			String fmt1 = comsgString(ALDOR_D_TinAvailableTypesForArg);
			
			bufPrintf(obuf, "\n          ");
			bufPrintf(obuf, fmt1, trArgN(tr)+1);
			if (abState(abArgi) == AB_State_HasPoss ||
			    abState(abArgi) == AB_State_Error)   
				bputTPoss(obuf, abGoodTPoss(abArgi));  
			else
				bputTForm(obuf, abTUnique(abArgi));
		}
	}
}

/* Prints all meanings rejected due to some parameter missing */
local void
bputParMissing(TRejectInfo trInfo, Buffer obuf, String fmtOp)
{
	String		fmtParType, fmt;
	TForm		tf, parType;
	Length		argc;

	for ( ; trInfo->i < trInfo->argc &&
	     trWhy(trCurrent(trInfo)) == TR_ArgMissing;
	     trInfo->i++) {
		
		TReject	tr = trCurrent(trInfo);
		TForm	opType = trType(tr);

		bputTReject(obuf, tr, fmtOp);

		tf = tfMapArg(opType);
        	argc = tfMapArgc(opType);
		parType = tfAsMultiArgN(tf, argc, trParN(tr));
		fmtParType = fmtTForm(parType);
		
		/* "rejected because parameter ... is missing */
		bufPrintf(obuf, "\n     ");
		fmt = comsgString(ALDOR_X_TinParameterMissing);
    		bufPrintf(obuf, fmt, trParN(tr)+1, fmtTForm(parType));

		strFree(fmtParType);
	}
}

/* Prints all meanings rejected due to different arity */
local void
bputEmbedFail(TRejectInfo trInfo, Buffer obuf, int argc, String fmtOp) 
{
	String	fmt = comsgString(ALDOR_X_TinBadArgumentNumber);
	Bool	found = false;

	for ( ; trInfo->i < trInfo->argc &&
	     trWhy(trCurrent(trInfo)) == TR_EmbedFail;
	     trInfo->i++) {
		TReject	tr = trCurrent(trInfo);
		bputTReject(obuf, tr, fmtOp);
		found = true;
	}
	if (found) {
		bufPrintf(obuf, "\n      ");
		bufPrintf(obuf, fmt, argc);
	}
}

/* Prints all meanings rejected due to the unmatching return type */
local void
bputBadFnType(TRejectInfo trInfo, Buffer obuf, TForm type, String fmtOp)
{
	String	fmt, fmtType;
	Bool	found = false;

	for ( ; trInfo->i < trInfo->argc &&
	     trWhy(trCurrent(trInfo)) == TR_BadFnType;
	     trInfo->i++) {
		TReject	tr = trCurrent(trInfo);

		bputTReject(obuf, tr, fmtOp);
		found = true;
	}

	if (found) {
		fmt     = comsgString(ALDOR_X_TinBadFnType);
		fmtType = fmtTForm(type);
		bufPrintf(obuf, "\n      ");
		bufPrintf(obuf, fmt, fmtType);

		strFree(fmtType);
	}

}

/**************************************************************************
 * Error output for the most likely meaning for an operator
 **************************************************************************/
local void
bputBadFnType0(TRejectInfo trInfo, Buffer obuf, TForm type, String fmtOp)
{
	String	fmt;
	TPoss retTypes = tpossEmpty();
	int i;

	for ( i = 0; i < trInfo->argc &&
	     trWhy(trInfo->argv[i]) == TR_BadFnType; i++) {
		TForm tfRet = tfMapRet(tfDefineeType(trType(trInfo->argv[i])));
		tfRet = tfDefineeType(tfRet);
		tpossAdd1(retTypes, tfRet);
	}

	bufPrintf(obuf, "\n  ");
	fmt = comsgString(ALDOR_D_TinRejectedRetTypes);
	bufPrintf(obuf, fmt);

	bputTPoss(obuf, retTypes);

	bputContextType(obuf, type);
        tpossFree(retTypes);
}

local Bool
bputBadArgType0(TRejectInfo trInfo, Stab stab, Buffer obuf, AbSyn ab, AbSyn op, 
		Length argc, AbSynGetter argf, String fmtOp) 
{
	String	fmt = comsgString(ALDOR_X_TinNoArgumentMatch);
	String	fmtParType;
	TForm	opType, argType, parType, defType;
	AbSyn   abArgi;
	int 	i, j;
	TPoss	parTypes = tpossEmpty();
	TReject	tr;
	Bool	result = false;
	Length	argN, argc0;

	assert(trWhy(trFirst(trInfo)) == TR_BadArgType);

	for (i = 0 ; i < trInfoArgc(trInfo) ; i += 1) {
		AbSub	sigma;

		tr = trInfoGet(trInfo,i);

		if (trWhy(tr) != TR_BadArgType ||
		    trArgN(tr) != trArgN(trFirst(trInfo)))
			break;

		opType  = tfDefineeType(trType(tr));
		if (tfIsDeclare(opType))
			opType = tfDeclareType(opType);
		argc0   = tfMapHasDefaults(opType) ? tfMapArgc(opType) : argc;
		argType = tfMapArg(opType);

		/* We might be given a tfUnknown (bug 1210) */
		defType = tfDefineeType(argType);
		if (defType == tfUnknown) continue;
		sigma = absNew(stab);
		for (j = 0; j < trArgN(tr); j += 1) {
			Syme	syme;
			TForm	tfj;
			AbSyn	abj;
			Bool	def;
			Length	ai;

			tfj  = tfAsMultiArgN(argType, argc0, j);
			syme = tfDefineeSyme(tfj);
			tfj  = tfDefineeType(tfj);
			if (!syme) continue;

			abj = tfAsMultiSelectArg(ab, argc, j, argf, tfj, &def, &ai);
			abj = sefoCopy(abj);
			tiBottomUp(stab, abj, tfUnknown);
			tiTopDown (stab, abj, tfj);
			if (abState(abj) == AB_State_HasUnique) {
				if (absFVars(sigma)) absSetFVars(sigma, NULL);
				sigma = absExtend(syme, abj, sigma);
			}
		}

		parType = tfAsMultiArgN(argType, argc0, trParN(tr));
		parType = tfDefineeType(parType);
		parType = tformSubst(sigma, parType);
		tpossAdd1(parTypes, parType);
		absFree(sigma);
	}

	i -= 1;

	if (trArgN(trInfoGet(trInfo,i)) == trArgN(trFirst(trInfo)) &&
	    ((i == trInfoArgc(trInfo)-1) ||
	     trWhy(trInfoGet(trInfo, i+1)) != TR_ArgMissing)) {
		result = true;
		argN = trArgN(trFirst(trInfo));
		fmt = comsgString(ALDOR_D_TinArgNoMatchParTypes);
		if (abIsTheId(op, ssymApply)) {
			if (argN > 0) {
				bufPrintf(obuf, fmt, argN+1, fmtOp);
			}
			else {
				fmt = comsgString(ALDOR_D_TinOperatorNoMatch);
				bufPrintf(obuf, fmt, argN+1);
			}
		}
		else
			bufPrintf(obuf, fmt, argN+1, fmtOp);
		bufPrintf(obuf, "\n");

		if (comsgOkDetails()) {
			abArgi = argf(ab, trArgN(trFirst(trInfo)));
			bputAbTPoss(obuf, 4, abArgi, 
				    ALDOR_D_TinRejectedType,
				    ALDOR_D_TinRejectedTypes);
			if (tpossCount(parTypes) == 1) {
				fmt = comsgString(ALDOR_D_TinExpectedType);
				fmtParType = fmtTForm(tpossUnique(parTypes));
				bufPrintf(obuf, "\n    ");
				bufPrintf(obuf, fmt, fmtParType);
				strFree(fmtParType);
			}
			else {
				bufPrintf(obuf, "\n    ");
				fmt = comsgString(ALDOR_D_TinExpectedTypes);
				bufPrintf(obuf, fmt);
				bputTPoss0(obuf, 6, parTypes);
			}
		}
	}
	else
		result = false;

	tpossFree(parTypes);
	return result;
}

/***************************************************************************
 * Try to determine which meaning(s) the user mean.
 *If is not possible, return false.
 ***************************************************************************/
local Bool
guessOpMeanings(TRejectInfo trInfo, Stab stab, Buffer obuf, AbSyn ab, AbSyn op,
		Length argc, AbSynGetter argf, TForm type, String fmtOp)
{
	Bool result = false;
	String fmt;
	AbSyn abErr = op;
	/* TO Do: if no -M guess option return */

	if (trWhy(trFirst(trInfo)) == TR_BadFnType) {
		result = true;
 		fmt = comsgString(ALDOR_D_TinRetTypesCantContext);
		bufPrintf(obuf, fmt);
		if (comsgOkDetails())
			bputBadFnType0(trInfo, obuf, type, fmtOp);
	}
	else if (trWhy(trFirst(trInfo)) == TR_BadArgType) {
		result = bputBadArgType0(trInfo,stab,obuf,ab,op,argc,argf,fmtOp);
		abErr = argf(ab, trArgN(trFirst(trInfo)));
	}

	if (result) {
		bputMeaningsOutOfScope(obuf,stab, ab,op,argc,argf,type,fmtOp);
		operatorErrMsg(ab, abErr, obuf);
	}
	return result;
}
/***********************************************************
 * Analyse rejection cause and put the information in tr
 ***********************************************************/
local void
analyseRejectionCause(TReject tr, Stab stab, AbSyn ab, Length argc,
		      AbSynGetter argf, TForm type)
{
	SatMask		mask = tfSatTErrorMask(), result;
	TForm		opType = trType(tr);

	opType = tfDefineeType(opType);

	if (!tfIsAnyMap(opType)) return;

	result = tfSatMap(mask, stab, opType, type, ab, argc, argf);

	if (tfSatFailedBadArgType(result)) {
		trParN(tr) = tfSatParN(result);
		trArgN(tr) = tfSatArgN(ab, argc, argf, trParN(tr), opType);
		trWhy(tr) = TR_BadArgType;
	}
	else if (tfSatFailedArgMissing(result)) {
		trParN(tr) = tfSatParN(result);
		trWhy(tr) = TR_ArgMissing;
	}
	else if (tfSatFailedEmbedFail(result) |
		 tfSatFailedDifferentArity(result))
		trWhy(tr) = TR_EmbedFail;
	else
		trWhy(tr) = TR_BadFnType;
}

/**************************************************************************
 * Fills the trInfo structure calling analyseRejectionCause on each tr
 **************************************************************************/
local void
fillTRejectInfo(TRejectInfo trInfo, TForm type, AbSyn ab, Stab stab,
		Length argc, AbSynGetter argf)
{
	Length i;
	
	for (i = 0; i < trInfo->argc; i++)
		analyseRejectionCause(trInfo->argv[i], stab, ab, argc, 
				      argf, type);
}

/**************************************************************************
 * If there are no suitable meanings for an operator, says why all the
 * meanings have been rejected.
 **************************************************************************/
local void
noMeaningsForOperator(Buffer obuf, TForm type, AbSyn ab, AbSyn op, Stab stab,
		      Length argc, AbSynGetter argf, String fmtOp)
{
	String	 		fmt;
	struct trejectInfo	trInfoStruct;
	Bool			imp = (abImplicit(ab) != NULL);

	if (abTag(ab) == AB_CoerceTo) {
		terrorCoerceTo(obuf, ab, type);
		return;
	}

	if (abTag(op) == AB_Id)
		trInfoFrSymes(&trInfoStruct,	    /* vvv FIXME */
			      stabGetMeanings(stab, ablogFalse(), op->abId.sym));
	else if (abState(op) == AB_State_HasPoss ||
		 abState(op) == AB_State_Error)
		trInfoFrTPoss(&trInfoStruct, abGoodTPoss(op));
	else
		trInfoFrTUnique(&trInfoStruct, abTUnique(op));

	fillTRejectInfo(&trInfoStruct, type, ab, stab, argc, argf);

	if (imp)
		sortSetBangTRejectInfo(&trInfoStruct);
	else
		sortTRejectInfo(&trInfoStruct);

	trInfoStruct.i = 0;

	if ((!imp && abIsTheId(op, ssymApply)) ||
	    !guessOpMeanings(&trInfoStruct,stab,obuf,ab,op,argc, argf,
    			     type,fmtOp)) {
		fmt = comsgString(ALDOR_E_TinNoGoodOp);
		bufPrintf(obuf, fmt, fmtOp);
		if (comsgOkDetails()) {
			bputBadFnType(&trInfoStruct, obuf, type, fmtOp);
			bputBadArgType(&trInfoStruct, obuf, ab, argc, argf,
				       fmtOp);
			bputParMissing(&trInfoStruct, obuf, fmtOp);
			bputEmbedFail(&trInfoStruct, obuf, argc, fmtOp);
			bputMeaningsOutOfScope(obuf, stab, ab, op, argc, argf,
					       type, fmtOp);
		}

		operatorErrMsg(ab, op, obuf);
	}


	trInfoFree(&trInfoStruct);
}

/**************************************************************************/
/* 0 or >1 meanings for an application                                    */
/**************************************************************************/

void
terrorApplyFType(AbSyn ab, TForm type, TPoss tposs, AbSyn op, Stab stab,
		 Length argc, AbSynGetter argf)
{
	String		fmt, fmtOp;
	Buffer		obuf;

	if (abTag(ab) == AB_Assign)
		return;

	
	terrorClip =  comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	obuf = bufNew();
	fmtOp = fmtAbSyn(op);
	
	if (tpossCount(tposs) == 0)   /* => 0 possible types */
		noMeaningsForOperator(obuf,type,ab,op,stab,argc,argf,
				      fmtOp);

	else {    /* => >1 possible types */
		fmt = comsgString(ALDOR_D_TinMoreMeanings);
   		bufPrintf(obuf, fmt, tpossCount(tposs) , fmtOp);
		bputMeanings(obuf, tposs);
		operatorErrMsg(ab, op, obuf);
	}
	
	strFree(fmtOp);
	bufFree(obuf);
	
}


/*************************************************************************
 * Called from tibup0ApplyFType when a symbol has a meaning in which the
 * type cannot be completely analyzed.
 *************************************************************************/
void
terrorApplyNotAnalyzed(AbSyn ab, AbSyn op, TForm tf)
{
	String fmt = comsgString(ALDOR_E_TinCantBeAnalyzed);
	Buffer obuf;
	obuf = bufNew();

	bufPrintf(obuf, fmt);
	bufPrintf(obuf,"\n");
	operatorErrMsg(ab, op, obuf);
	bufFree(obuf);
}

/**************************************************************************/
/* Assignment error                                                       */
/**************************************************************************/
void
terrorAssign(AbSyn ab, TForm type, TPoss tposs)
{
	String fmt, s;
	Buffer obuf;
	AbSyn rhs   = ab->abAssign.rhs;
	AbSyn lhs   = ab->abAssign.lhs;
	AbSyn abp;
	
        terrorClip =  comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	obuf = bufNew();

        if (abState(lhs) == AB_State_Error || 
	    (abState(lhs) == AB_State_HasPoss &&
	    tpossCount(abTPoss(lhs)) == 0)) {
		fmt = comsgString(ALDOR_E_TinCantInferLhs);
		bufPrintf(obuf, fmt);
		abp = abNewNothing(abPos(lhs));
	}
	else {
                fmt = comsgString(ALDOR_E_TinNoGoodInterp);  
		s = fmtAbSyn(rhs);
		bufPrintf(obuf, fmt, s);
		strFree(s);
		abp = abNewNothing(abPos(rhs));

		/* Prevent error msg at the upper level */
		if (abState(lhs) == AB_State_HasPoss) {
			TForm type = tpossUnique(abTPoss(lhs));
			assert(tpossCount(abTPoss(lhs)) == 1);
			
			bputContextType(obuf, type);
			
			tpossFree(abGoodTPoss(lhs));
			abTPoss(lhs) = tpossEmpty();
		}
		else {
		    abState(lhs) = AB_State_HasPoss;
		    abTPoss(lhs) = tpossEmpty();
	    	}
	}

	if (!comsgOkDetails()) goto done;

        bufPrintf(obuf, "\n");

	fmt = comsgString(ALDOR_D_TinPossTypesRhs);
	s = fmtAbSyn(rhs);
        bufPrintf(obuf,"     ");
        bufPrintf(obuf, fmt, s);
	strFree(s);
	bputTPoss(obuf, abGoodTPoss(rhs));

        if (abState(lhs) != AB_State_HasPoss &&
	    tpossCount(abTPoss(lhs)) > 0) {   /* 1 := >1 */
		assert(tpossIsUnique(abTPoss(lhs)));
		bputContextType(obuf, type);
	}

 done:
	comsgError(abp, ALDOR_E_ExplicitMsg, bufChars(obuf));
	abFree(abp);
	bufFree(obuf);

}

/**************************************************************************/
/* The following exports were not provided:...                           */
/**************************************************************************/

local void 
terrorPutConditionalExports(Stab stab, Buffer buf, SymeList csymes)
{
	SefoList conds;
	SymeList gsymes, symes, nsymes, hsyme;
	String fmt;
	/* Print sorted by condition */
	/* !!Should also try to not print meanings where the
	 * meaning is also missing for a weaker condition
	 */
	fmt = comsgString(ALDOR_D_TinMissingExport);
	while (csymes != listNil(Syme)) {
		String scond;
		conds = symeCondition(car(csymes));
		nsymes = listNil(Syme);
		gsymes = listNil(Syme);
		symes = csymes;
		while (symes != listNil(Syme)) {
			Syme syme = car(symes);
			hsyme = symes;
			symes = cdr(symes);
			if (sefoListEqual(symeCondition(syme), conds)) {
				cdr(hsyme) = gsymes;
				gsymes = hsyme;
			}
			else {
				cdr(hsyme) = nsymes;
				nsymes = hsyme;
			}
		}
		csymes = listNReverse(Syme)(nsymes);
		gsymes = listNReverse(Syme)(gsymes);
		scond = fmtAbSyn(car(conds));
		bufPrintf(buf, "\n\tif %s", scond);
		conds = cdr(conds);
		while (conds != listNil(Sefo)) {
			scond = fmtAbSyn(car(conds));
			bufPrintf(buf, " and %s", scond);
			conds = cdr(conds);
		}
		bufPrintf(buf, " then", scond);
		while (gsymes != listNil(Syme)) {
			String s;
			s = fmtTForm(symeType(car(gsymes)));
			bufPrintf(buf , "\n\t\t");

			bufPrintf(buf, fmt,
				  symeString(car(gsymes)), s);
			strFree(s);
			gsymes = listFreeCons(Syme)(gsymes);
		}
	}
}

void
terrorNotEnoughExports(Stab stab, AbSyn ab, TPoss tposs, Bool onlyWarning)
{
	TForm 		base;
	SymeList	symes;
	SymeList	csymes;
	SymeList	isymes;
	SymeList        msymes;
	SymeList        mods;
	SymeList 	aself;
	Buffer		obuf;

	terrorClip =  comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	obuf = bufNew();

	if (!comsgOkDetails()) goto done;
	
	base = abTForm(ab->abAdd.base);
	assert(tpossIsUnique(tposs));
	symes = tfGetCatExports(tpossUnique(tposs));

	aself = tfGetSelfFrStab(stab);	
	mods = listCopy(Syme)(tfGetCatSelf(tpossUnique(tposs)));
	mods = listNConcat(Syme)(listCopy(Syme)(tfGetDomSelf(base)), mods);
	mods = listNConcat(Syme)(aself, mods);

	csymes = listNil(Syme);
	isymes = listNil(Syme);
	msymes = listNil(Syme);
	for (; symes; symes = cdr(symes)) {
		Syme syme = car(symes);
		Syme isyme = stabGetDomainExportMod(stab, mods, symeId(syme), symeType(syme));
		if (isyme != NULL) {
			isymes = listCons(Syme)(syme, isymes);
		}
		else if (symeCondition(car(symes)))
			csymes = listCons(Syme)(syme, csymes);
		else {
			msymes = listCons(Syme)(syme, msymes);
		}
	}
	
	if (msymes != listNil(Syme)) {
		String fmt = comsgString(ALDOR_D_TinMissingExports);
		bufPrintf(obuf, "%s", fmt);
		terrorPrintSymeList(obuf, "", msymes);
	}

	if (csymes) {
		terrorPutConditionalExports(stab, obuf, csymes);
	}
	if (isymes) {
		terrorPutConditionallyDefinedExports(obuf, stab, mods, ab, isymes);
	}
done:
	if (onlyWarning)
		comsgWarning(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	else
		comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);

}

local void
terrorPrintSymeList(Buffer obuf, String prefix, SymeList msymes)
{
	String fmt = comsgString(ALDOR_D_TinMissingExport);

	for (; msymes != listNil(Syme); msymes = listFreeCons(Syme)(msymes)) {
		Syme syme = car(msymes);
		String s = fmtTForm(symeType(syme));
		bufPrintf(obuf , "\n\t%s", prefix);
		bufPrintf(obuf, fmt, symeString(syme), s);
		strFree(s);
	}
}

local void
terrorPutConditionallyDefinedExports(Buffer obuf, Stab stab, SymeList mods, AbSyn ab, SymeList symes)
{
	SymeList iter;

	iter = listCopy(Syme)(symes);
	while (iter != listNil(Syme)) {
		Syme syme = car(iter);
		SymeList msymes, nsymes;
		SefoList condition = symeCondition(syme);
		Syme implSyme = stabGetDomainExportMod(stab, mods, symeId(syme), symeType(syme));
		AbSynList defCondition = symeDefinitionConditions(implSyme);

		nsymes = listNil(Syme);
		msymes = listCons(Syme)(syme, listNil(Syme));
		iter = listFreeCons(Syme)(iter);
		while (iter != listNil(Syme)) {
			Syme iterSyme = car(iter);
			Syme implSyme = stabGetDomainExportMod(stab, mods, symeId(iterSyme), symeType(iterSyme));
			if (sefoListEqual(condition, symeCondition(iterSyme))
			    && sefoListEqual((SefoList) defCondition, 
					     (SefoList) symeDefinitionConditions(implSyme)))
				msymes = listCons(Syme)(iterSyme, msymes);
			else
				nsymes = listCons(Syme)(iterSyme, nsymes);
			iter = listFreeCons(Syme)(iter);
		}
		iter = nsymes;
		bufPrintf(obuf, "\n");
		if (condition == listNil(Sefo)) {
			AbSyn expr = abNewNot(sposNone, abNewOrAll(sposNone, defCondition));
			bufPrintf(obuf, "\tMissing where %s", abPretty(expr));
			terrorPrintSymeList(obuf, "\t", msymes);
		}
		else {
			AbSyn expr = abNewNot(sposNone, abNewOrAll(sposNone, defCondition));
			bufPrintf(obuf, "\tMissing where %s\n", abPretty(abNewAndAll(sposNone, 
											   (AbSynList) condition)));
			bufPrintf(obuf, "\t\t  and %s", abPretty(expr));
			terrorPrintSymeList(obuf, "\t", msymes);
		}
	}
}

/***************************************************************************/
/* Specific error msg for Identifier without meaning.			   */
/***************************************************************************/
void
terrorNoMeaningForId(AbSyn ab, String s)
{
	String fmt = comsgString(ALDOR_E_TinNoMeaningForId);
	Buffer obuf;
	obuf = bufNew();

	bufPrintf(obuf, fmt, s);
	comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}

/***************************************************************************/
/* Specific error msg for literal without meaning.			   */
/***************************************************************************/
local void
terrorNoMeaningForLit(AbSyn ab)
{
	String	fmt = comsgString(ALDOR_E_TinNoMeaningForLit);
	String	s = fmtAbSyn(ab);
	String	s0;
	Buffer	obuf = bufNew();

	switch (abTag(ab)) {
	case AB_LitInteger:
		s0 = "integer";
		break;
	case AB_LitString:
		s0 = "string";
		break;
	case AB_LitFloat:
		s0 = "float";
		break;
	default:
		bugBadCase(abTag(ab));
		NotReached(return);
	}
	
	bufPrintf(obuf, fmt, s0, s);
	strFree(s);
	comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}

/***************************************************************************/

/*
 * <symes> is the list of symes satisfing the context type <type>.
 * <allSymes> is the list of all possible symes for ab.
 * If symes is an empty list, the produced message is: "No meanings for..."
 * otherwhise (more than 1 syme), allSymes are displayed.
 * Otherwise the message is "n meanings for ..." and symes are displayed.
 * NOTE: if the call is correct, the list should never contain 1 element.
 */
void
terrorNotUniqueMeaning(Msg msg, AbSyn ab, SymeList symes, SymeList allSymes,
		       String s, TForm type)
{
	Buffer	obuf;
	String	fmt;
	Length	nsymes;

	terrorClip =  comsgOkAbbrev() ? CLIP : ABPP_UNCLIPPED;

	nsymes = listLength(Syme)(symes);

	assert(nsymes != 1);

	fmt  = comsgString(msg);
	obuf = bufNew();
	bufPrintf(obuf, fmt, nsymes, s);

	if (!comsgOkDetails()) goto done;

	if (!symes)
		symes = allSymes;

	if (symes) {
		fmt = comsgString(ALDOR_D_TinPossTypes);
		bufPrintf(obuf, "\n");
		bufPrintf(obuf, fmt);
	}

	for (; symes; symes = cdr(symes)) {
		String	opstr, stype, sfrom;

		bufPrintf(obuf, "\n\t  ");

		opstr = symString(symeId(car(symes)));
		stype = fmtTForm(symeType(car(symes)));
		switch (symeKind(car(symes))) {
		case SYME_Import:
			sfrom = fmtTForm(symeExporter(car(symes)));
			fmt   = comsgString(ALDOR_D_TinOneImpMeaning);
			bufPrintf(obuf, fmt, opstr, stype, sfrom);
			strFree(sfrom);
			break;
		case SYME_Library:
			fmt = comsgString(ALDOR_D_TinOneLibMeaning);
			bufPrintf(obuf, fmt, opstr, stype);
			break;
		default:
			fmt   = comsgString(ALDOR_D_TinOneLexMeaning);
			bufPrintf(obuf, fmt, opstr, stype);
			break;
		}
		strFree(stype);
	}

	bputContextType(obuf, type);
done:
	comsgError(ab, ALDOR_E_ExplicitMsg, bufChars(obuf));
	bufFree(obuf);
}



/*****************************************************************************
 *
 * :: Formatting utilities
 *
 *****************************************************************************/

local String
fmtAbSyn(AbSyn ab)
{
	return abPrettyClippedIn(ab, terrorClip, INDENT);
}

local String
fmtTForm(TForm tf)
{
	return tfPrettyClippedIn(tf, terrorClip, INDENT);
}

local void
bputMeanings(Buffer buf, TPoss tp)
{
	TPossIterator	ti;
	int		n;
	String		fmt = comsgString(ALDOR_D_TinOneMeaning);

	for (n = 1, tpossITER(ti,tp); tpossMORE(ti); n++, tpossSTEP(ti)) {
		String s = fmtTForm(tpossELT(ti));
		bufPrintf(buf, "\n\t");
		bufPrintf(buf, fmt, n, s);
		strFree(s);
	}
}

local void
bputSymes(Buffer buf, SymeList symes, String fmtOp)
{
	for (; symes; symes = cdr(symes))
		bputSyme(buf, car(symes), symeType(car(symes)), fmtOp);
}

local void
bputSyme(Buffer obuf, Syme syme, TForm type, String fmtOp)
{
	String	stype, sfrom, fmt;
	SefoList cond;
	stype = fmtTForm(type);
	cond = symeCondition(syme);
	bufPrintf(obuf, "\n  ");
	
	switch (symeKind(syme)) {
	case SYME_Import:
		sfrom = fmtTForm(symeExporter(syme));
		fmt = comsgString(ALDOR_D_TinOneImpMeaning);
		bufPrintf(obuf, fmt, fmtOp, stype, sfrom);
		bputCondition(obuf, cond);
		strFree(sfrom);
		break;
	case SYME_Library:
		fmt = comsgString(ALDOR_D_TinOneLibMeaning);
		bufPrintf(obuf, fmt, fmtOp, stype);
		break;
	default:
		fmt = comsgString(ALDOR_D_TinOneLexMeaning);
		bufPrintf(obuf, fmt, fmtOp, stype);
		bputCondition(obuf, cond);
		break;
	}
	strFree(stype);
	
}

local void
bputCondition(Buffer buf, SefoList conds)
{
	Sefo cond ;
	String scond, fmt;
	if (!conds) return;
	
	cond  = car(conds);
	conds = cdr(conds);
	scond = fmtAbSyn(cond);
	fmt = ", if %s"; /* comsgString(ALDOR_D_TinCondSeparator) */
	bufPrintf(buf, fmt, scond);
	
	while (conds) {
		scond = fmtAbSyn(car(conds));
		conds = cdr(conds);
		bufPrintf(buf, ", %s", scond);
	}
}

/* Prints the application type without the exporter */
local void
bputType(Buffer obuf, TForm tf, String fmtOp)
{
	String stype = fmtTForm(tf);
	String fmt   = comsgString(ALDOR_D_TinOneMeaning0);
	bufPrintf(obuf, "\n  ");
	bufPrintf(obuf, fmt, stype);
	strFree(stype);
}

local void
bputTForm0(Buffer buf, int indent, TForm tf)    
{
	String s = fmtTForm(tf);

	bufPutc(buf, '\n');
	bufPutcTimes(buf, ' ', indent);
	bufPrintf(buf, "-- %s", s);
	strFree(s);
}

local void
bputTPoss0(Buffer buf, int indent, TPoss tp)
{
	TPossIterator	ti;

	for (tpossITER(ti,tp); tpossMORE(ti); tpossSTEP(ti)) 
		bputTForm0(buf, indent, tpossELT(ti));	
}

local void
bputTForm(Buffer buf, TForm tf)    
{
	String s = fmtTForm(tf);
	bufPrintf(buf, "\n          -- %s", s);
	strFree(s);
}

local void
bputTPoss(Buffer buf, TPoss tp)
{
	TPossIterator	ti;

	for (tpossITER(ti,tp); tpossMORE(ti); tpossSTEP(ti)) 
		bputTForm(buf, tpossELT(ti));	
}


/* Dipending on the state of ab, puts in obuf:
 * <plurmsg> (types)
 *    OR
 * <singmsg> (type).
 *    OR
 * nothing
 *
 * Indent is the current indentation level which must be used.
 * <singmsg> should have a %s for the type and <plurmsg> should not.
 */
local void
bputAbTPoss(Buffer obuf, int indent, AbSyn ab, Msg singmsg, Msg plurmsg)
{
	String fmt;

	switch (abState(ab)) {
	case AB_State_HasPoss: {
		if (tpossCount(abTPoss(ab)) == 0 ||
		    (tpossCount(abTPoss(ab)) == 1 &&
		     tfIsUnknown(tpossUnique(abTPoss(ab)))))
			return;
		
		if (tpossCount(abTPoss(ab)) != 1) {
			fmt = comsgString(plurmsg);
			bufPutcTimes(obuf, ' ', indent);
			bufPrintf(obuf, fmt);	
			bputTPoss0(obuf, indent+2, abTPoss(ab));  
			
		}
		else {
			String s0 = fmtTForm(tpossUnique(abTPoss(ab)));
			fmt = comsgString(singmsg);
			bufPutcTimes(obuf, ' ', indent);
			bufPrintf(obuf, fmt, s0);
			strFree(s0);
		}
	}
		break;
	case AB_State_HasUnique: {
		String s0 = fmtTForm(abTUnique(ab));
		fmt = comsgString(singmsg);
		bufPutcTimes(obuf, ' ', indent);
		bufPrintf(obuf, fmt, s0);
		strFree(s0);
		break;
	}
	default: 
		return;
	}
}

local void
bputContextType(Buffer obuf, TForm type)
{
	String s, fmt;

	if (!tfIsUnknown(type) && !tfIsNone(type)) {
		s   = fmtTForm(type);
		fmt = comsgString(ALDOR_D_TinContextType);
		bufPrintf(obuf, "\n  ");
		bufPrintf(obuf, fmt, s);
		strFree(s);
	}
}

local void
bputTConst(Buffer buf, TConst tc)
{
	String	s0, s1;

	switch(tcTag(tc)) {
	case TC_Satisfies:
		s0 = fmtTForm(tcArgv(tc)[0]);
		s1 = fmtTForm(tcArgv(tc)[1]);
		break;
	default:
		bugBadCase(tcTag(tc));
		NotReached(s0 = s1 = NULL);
	}

	bufPrintf(buf, "\n      -- %s satisfies %s", s0, s1);
	strFree(s0);
	strFree(s1);
}

local void
bputTReject(Buffer obuf, TReject tr, String fmtOp)
{
	if (trSyme(tr))
		bputSyme(obuf, trSyme(tr), trType(tr), fmtOp);
	else
		bputType(obuf, trType(tr), fmtOp);
}

/* cares that ^ catches the operator if infixed o 'coerce' */
local void
operatorErrMsg(AbSyn ab, AbSyn op, Buffer obuf)
{
	AbSyn	abp;

	if (abTag(ab) == AB_CoerceTo)
		abp = ab->abCoerceTo.type;
	else
		abp = op;

	comsgError(abp, ALDOR_E_ExplicitMsg, bufChars(obuf));
}


/*****************************************************************************
 *
 * :: Data structure audits
 *
 ****************************************************************************/

static Bool	reallyAudit = false;

local Bool	terrorAuditPoss0	(Bool, AbSyn, Table);
local Bool	terrorAuditPoss1	(Bool, AbSyn, Table);


Bool
terrorAuditPoss(Bool verbose, AbSyn absyn)
{
	Table	counts;
	Bool	ok;

	if (!reallyAudit) return true;

	counts = tblNew((TblHashFun) 0, (TblEqFun) 0); /* "==" for ref counts */
	ok     = true;

	ok &= terrorAuditPoss0(verbose, absyn, counts);
	ok &= terrorAuditPoss1(verbose, absyn, counts);

	tblFree(counts);
	return ok;
}

local Bool
terrorAuditPoss0(Bool verbose, AbSyn absyn, Table counts)
{
	int	i;
	Bool	ok = true;

	if (absyn == 0) {
		fprintf(dbOut,"Failure Mode 0");
		fnewline(dbOut);
		return false;
	}
	if (abState(absyn) == AB_State_HasPoss) {
		TPoss	tposs = abTPoss(absyn);
		long	n;
		n = ptrToLong(tblElt(counts, tposs, ptrFrLong(long0)));
		tblSetElt (counts, tposs, ptrFrLong(n+1));
	}
	else {
		if (verbose) {
			String expr = abPretty(absyn);
			if (abTag(absyn) == AB_Nothing) {
				fprintf(dbOut,"'Twas nothing");
				fnewline(dbOut);
			}
			fprintf(dbOut,"State != AB_State_HasPoss in: %s", expr);
			fnewline(dbOut);
			strFree(expr);
		}
		ok = false;
	}
	if (!abIsLeaf(absyn))
		for (i = 0; i < abArgc(absyn); i++)
			ok &= terrorAuditPoss0(verbose,abArgv(absyn)[i],counts);

	return ok;
}

local Bool
terrorAuditPoss1(Bool verbose, AbSyn absyn, Table counts)
{
	int	i;
	Bool	ok = true;

	if (abState(absyn) == AB_State_HasPoss) {
		TPoss	tposs = abTPoss(absyn);
		long	n;
		n = ptrToLong(tblElt(counts, tposs, ptrFrLong(long0)));
		if (n != tposs->refc) {
			if (verbose) {
				String expr = abPretty(absyn);
				fprintf(dbOut,
					"Type possibilty set on expression %s ",
					expr);
				fprintf(dbOut,
					"is used %ld times but believes %d.",
					n, tposs->refc);
				fnewline(dbOut);
			}
			ok = false;

			/*
			 * Stifle further messages.
			 */
			tblSetElt(counts, tposs, ptrFrLong((long) tposs->refc));
		}
	}

	if (!abIsLeaf(absyn))
		for (i = 0; i < abArgc(absyn); i++)
			ok &= terrorAuditPoss1(verbose,abArgv(absyn)[i],counts);

	return ok;
}

Bool
terrorAuditBottomUp(Bool verbose, AbSyn absyn)
{
	int	i;
	Bool	ok = true;

	assert(absyn);
	if ((abState(absyn) != AB_State_HasPoss) && !abIsSefo(absyn)) {
		if (verbose) {
			String expr = abPretty(absyn);
			fprintf(dbOut, "No type possibilty set on node: %s", expr);
			fnewline(dbOut);
			strFree(expr);
		}
		ok = false;
	}
	if (!abIsLeaf(absyn))
		for (i = 0; i < abArgc(absyn); i++)
			ok &= terrorAuditBottomUp(verbose, abArgv(absyn)[i]);
	return ok;
}

Bool
terrorAuditTopDown(Bool verbose, AbSyn absyn)
{
	int	i;
	Bool	ok = true;

	assert(absyn);
	if (!abIsSefo(absyn)) {
		if (verbose) {
			String expr = abPretty(absyn);
			fprintf(dbOut, "No unique type on node: %s", expr);
			fnewline(dbOut);
			strFree(expr);
		}
		ok = false;
	} else if (abIsLeaf(absyn) && !abSyme(absyn)) {
		if (verbose) {
			String expr = abPretty(absyn);
			fprintf(dbOut, "No symbol meaning for leaf: %s", expr);
			fnewline(dbOut);
			strFree(expr);
		}
		ok = false;
	}
	if (!abIsLeaf(absyn))
		for (i = 0; i < abArgc(absyn); i++)
			ok &= terrorAuditTopDown(verbose, abArgv(absyn)[i]);
	return ok;
}
