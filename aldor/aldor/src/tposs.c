/*****************************************************************************
 *
 * tposs.c: Type possibility sets.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * The bottom-up pass of type inference constructs a set of possible types
 * for each node.  These definitions allow us to change the representation
 * of the type set.
 */

#include "ablogic.h"
#include "axlobs.h"
#include "ablogic.h"
#include "debug.h"
#include "format.h"
#include "infenv.h"
#include "spesym.h"
#include "store.h"
#include "syme.h"
#include "tconst.h"
#include "terror.h"
#include "tfknown.h"
#include "tfsat.h"
#include "tposs.h"
#include "unify.h"
#include "utform.h"
#include "util.h"
#include "utyperes.h"

/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

Bool	tpossLambdaDebug	= false;
Bool	tpossSatTypeDebug	= false;
Bool	tpossSelectSatDebug	= false;
Bool	tpossIntersectDebug	= false;

#define tpossLambdaDEBUG	DEBUG_IF(tpossLambda)	 afprintf
#define tpossSatTypeDEBUG	DEBUG_IF(tpossSatType)	 afprintf
#define tpossSelectSatDEBUG	DEBUG_IF(tpossSelectSat) afprintf
#define tpossIntersectDEBUG	DEBUG_IF(tpossIntersect) afprintf

/*
 *   Each node is given a set of possible meanings.
 *
 * An idea for fast type inference:
 *
 *   Rather than having a *list* of possible types at each node,
 *   it would be more compact to have a vector of all possible types
 *   in a given scope, and to represent the possible types as a bit vector.
 *
 *   For the Fall 1990 implementation, I will use lists.  But I'll
 *   write the code so that this idea could be used.	-- SMW
 */

local UTFContext tpossJoin(USatMask, UTForm, UTForm);


local TPoss
tpossAlloc(void)
{
	TPoss tp = (TPoss) stoAlloc(OB_TPoss, sizeof(struct tposs));

	return tp;
}

TPoss
tpossEmpty(void)
{
	TPoss tp  = tpossAlloc();
	tp->possl = listNil(UTFContext);
	tp->possc = 0;
	tp->refc  = 1;
	return tp;
}

local void
tpossCons(TPoss tp, UTForm t)
{
	assert(tp);
	t = utformFollowOnly(t);
	tp->possl = listCons(UTFContext)(uctxtEmpty(t), tp->possl);
	tp->possc += 1;
}

local void
tpossConsUTFC(TPoss tp, UTFContext utfc)
{
	assert(tp);
	utfc = uctxtFollowOnly(utfc);
	tp->possl = listCons(UTFContext)(utfc, tp->possl);
	tp->possc += 1;
}

TPoss
tpossFrTheList(TFormList l)
{
	TPoss tp  = tpossAlloc();
	tp->possc = listLength(TForm)(l);
	tp->refc  = 1;
	tp->possl = listNil(UTFContext);
	while (l != listNil(TForm)) {
		tp->possl = listCons(UTFContext)(uctxtEmpty(utformNewConstant(car(l))), tp->possl);
		l = cdr(l);
	}
	listNReverse(UTFContext)(tp->possl);
	return tp;
}

TPoss
tpossFrTheUTFormList(UTFormList l)
{
	TPoss tp  = tpossAlloc();
	tp->possc = listLength(UTForm)(l);
	tp->refc  = 1;
	while (l != listNil(UTForm)) {
		tp->possl = listCons(UTFContext)(uctxtEmpty(car(l)), tp->possl);
		l = cdr(l);
	}
	listNReverse(UTFContext)(tp->possl);
	return tp;
}

TPoss
tpossFrTheUTFContextList(UTFContextList l)
{
	TPoss tp  = tpossAlloc();
	tp->possc = listLength(UTFContext)(l);
	tp->refc  = 1;
	tp->possl = l;
	return tp;
}

TPoss
tpossSingletonUTForm(UTForm t)
{
	TPoss tp  = tpossEmpty();
	tpossCons(tp, t);
	return tp;
}

TPoss
tpossSingleton(TForm t)
{
	return tpossSingletonUTForm(utformNewConstant(t));
}

TPoss
tpossUnknown(TPoss tposs)
{
	TPoss tpNew = tpossEmpty();
	TPossIterator	it;
	UTForm unk = utformNewConstant(tfUnknown);

	for (tpossITER(it, tposs); tpossMORE(it); tpossSTEP(it)) {
		UTFContext utfc = tpossUCELT(it);
		tpossAdd1UTFContext(tpNew, uctxtNew(uctxtInfEnv(utfc), unk));
	}
	return tpNew;
}

TPoss
tpossFrSymes(SymeList symes)
{
	TPoss	tp = tpossEmpty();
	for (; symes; symes = cdr(symes))
	{
		Syme syme = car(symes);
		tpossAdd1(tp, symeType(syme));
	}
	return tp;
}

TPoss
tpossDeclare(Syme syme, TPoss tp)
{
	TPoss		ntp = tpossEmpty();
	TPossIterator	it;

	for (tpossITER(it, tp); tpossMORE(it); tpossSTEP(it)) {
		TForm	tf = tpossELT(it);
		tf = tfDeclare(abFrSyme(syme), tf);
		tpossAdd1(ntp, tf);
	}

	return ntp;
}

TPoss
tpossFilterEmpty(TPoss tposs)
{
	TPoss tprhs = tpossEmpty();
	TPossIterator tit;

	for (tpossITER(tit, tposs); tpossMORE(tit); tpossSTEP(tit)) {
		/* Get the next type possibility */
		UTFContext utfc = tpossUCELT(tit);
		TForm t = tpossELT(tit);

		/* Is it an empty multi? */
		if (tfIsMulti(t) && !tfArgc(t)) continue;

		/* No - add it to the set of possible types */
		tprhs = tpossAdd1UTFContext(tprhs, utfc);
	}
	return tprhs;
}

/*
 * tpossMulti(n,v,f) takes the n TPoss sets  f(v,0)...f(v,n-1) and forms
 *   their cross product.
 *!! This will get very expensive if the f(v,i) have many elements so it may
 *!! become necessary to use a symbolic tree form for TPoss sets.
 */

/*
 * tl is one element of the cross product get(v,k)...get(v,n-1)
 */
local void
tposs0Multi(TPoss tp,Length k, UTFormList tl,Length n,Pointer v,TPossGetter get)
{
	if (k == 0)
		tpossCons(tp, utfMultiFrList(tl));
	else {
		TPoss		tpk = get(v, k-1);
		TPossIterator	tpi;

		tl = listCons(UTForm)(NULL, tl);
		for (tpossITER(tpi,tpk); tpossMORE(tpi); tpossSTEP(tpi)) {
			UTForm t = tpossUELT(tpi);
			t = utformFollowOnly(t);
			setcar(tl, t);
			tposs0Multi(tp, k-1, tl, n, v, get);
		}
		listFreeCons(UTForm)(tl);
		tpossFree(tpk);
	}
}

TPoss
tpossMulti(Length argc, Pointer argv, TPossGetter get)
{
	TPoss	tp = tpossEmpty();

	tposs0Multi(tp, argc, listNil(UTForm), argc, argv, get);

	return tp;
}

TPoss
tpossAdd1(TPoss tp, TForm t)
{
	t = tfFollowOnly(t);
	if (! tpossHas(tp, t))
		tpossCons(tp, utformNewConstant(t));
	return tp;
}

Bool
tpossHasUTForm(TPoss tp, UTForm t)
{
        UTFContextList l;
        UTForm s;

        if (tp == NULL)
               return false;
        if (utfIsConstant(t)) {
                return tpossHas(tp, utformTForm(t));
        }

        l = tp->possl;
        while (l != listNil(UTFContext)) {
                s = uctxtUTForm(car(l));
                l = cdr(l);
                // tpossHas({Int}, Vs.S) --> false (actually, replace Int with Vs.S)
                // tpossHas({Vs.S}, Int) --> true
                if (utfIsConstant(s) && utformCanUnify(s, t))
                        continue;
                // Can turn s into t
                if (utformCanUnify(s, utformNewConstant(utformTForm(t)))) {
			if (!infEnvIsEmpty(uctxtInfEnv(car(l)))) {
				afprintf(dbOut, "TP %pTPoss form: %pUTForm\n", tp, t);
				bug("tpossHasUTForm: Possible iffyness");
			}
			return true;
		}
        }

        return false;
}

Bool
tpossHasUTFContext(TPoss tp, UTFContext t)
{
	if (tpossHasUTForm(tp, t->utform) && infEnvIsEmpty(t->infEnv))
		return true;
	return false;
}

TPoss
tpossAdd1UTForm(TPoss tp, UTForm t)
{
	if (!tpossHasUTForm(tp, t))
		tpossCons(tp, t);
	return tp;
}

TPoss
tpossAdd1UTFContext(TPoss tp, UTFContext utfc)
{
	if (!tpossHasUTFContext(tp, utfc))
		tpossConsUTFC(tp, utfc);
	return tp;
}

TPoss
tpossAdd1InferEnv(TPoss tp, TForm tf, InferEnv infEnv)
{
	return tpossAdd1UTFContext(tp, uctxtNewConst(infEnv, tf));
}


Bool
tpossHasIntersection(TPoss tp1, TPoss tp2)
{
	TPoss tp = tpossIntersect(tp1, tp2);
	Bool flg = tpossCount(tp) == 0;
	tpossFree(tp);
	return flg;
}

TPoss
tpossRefer(TPoss tp)
{
	if (tp != NULL)
		++tp->refc;
	return tp;
}

TPoss
tpossCopy(TPoss tp)
{
	TPoss np;

	if (tp == NULL)
	       	return NULL;

	np = tpossAlloc();
	np->possl = listCopy(UTFContext)(tp->possl);
	np->possc = tp->possc;
	np->refc  = 1;
	return np;
}

int
tpossCount(TPoss tp)
{
	if (tp)
		return tp->possc;
	else
		return 0;
}

int debugflag = 0; /* Don't want "clever" C compiler's dead coding this */
void
tpossFree(TPoss tp)
{
	if (tp == NULL)
		return;
	if (--tp->refc > 0) return;
	assert(tp->refc == 0);
	if (!debugflag) {
		listFree(UTFContext)(tp->possl);
		stoFree((Pointer) tp);
	}
	debugflag = 0;
}

int
tpossPrint(FILE *fout, TPoss tp)
{
	switch ((UAInt) tp)
	{
		case 0:
			return fprintf(fout, "[]");
		case tuniInappropriateTPossVal:
			return fprintf(fout, "(inappropriate)");
		case tuniNoValueTPossVal:
			return fprintf(fout, "(no value)");
		case tuniUnknownTPossVal:
			return fprintf(fout, "(unknown)");
		case tuniErrorTPossVal:
			return fprintf(fout, "(error)");
		default:
			return afprintf(fout, "%pUTFContextList", tp->possl);
	}
	return 0;
}

int
tpossOStreamWrite(OStream ostream, TPoss tp)
{
	switch ((UAInt) tp)
	{
		case 0:
			return ostreamPrintf(ostream, "[]");
		case tuniInappropriateTPossVal:
			return ostreamPrintf(ostream, "(inappropriate)");
		case tuniNoValueTPossVal:
			return ostreamPrintf(ostream, "(no value)");
		case tuniUnknownTPossVal:
			return ostreamPrintf(ostream, "(unknown)");
		case tuniErrorTPossVal:
			return ostreamPrintf(ostream, "(error)");
		default:
			return ostreamPrintf(ostream, "[TP: %d %pUTFContextList]", tp->possc, tp->possl);
	}
	return 0;
}


int
tpossPrintDb(TPoss tp)
{
	int rc = tpossPrint(dbOut, tp);
	fnewline(dbOut);
	return rc;
}

Bool
tpossIsEmpty(TPoss tp)
{
	if (tp)
		return tp->possc == 0;
	else
		return true;
}
Bool
tpossIsUnique(TPoss tp)
{
	if (tp)
		return tp->possc == 1;
	else
		return false;
}

#if 0
TPoss
tpossIntersect(TPoss tp1, TPoss tp2)
{
	TFormList l = 0;
	TFormList l1;

	if (tp1 == NULL || tp2 == NULL)
		return NULL;

	l1 = tp1->possl;

	for (; l1; l1 = cdr(l1)) {
		car(l1) = tfFollowOnly(car(l1));
		if (tpossHas(tp2, car(l1)))
			l = listCons(UTForm)(car(l1), l);
	}

	l = listNReverse(UTForm)(l);
	return tpossFrTheList(l);
}
#endif


TPoss
tpossIntersect_Orig(TPoss S, TPoss T)
{
	UTFContextList LS, LT, l = 0;
	UTForm carLS, carLT;

	if (S == NULL || T == NULL)
		return NULL;

	/* If T is free of duplicates, then the result will also be. */
	for (LT = T->possl; LT; LT = cdr(LT)) {
		car(LT) = uctxtFollowOnly(car(LT));
		carLT = uctxtUTForm(car(LT));

	for (LS = S->possl; LS; LS = cdr(LS)) {
			car(LS) = uctxtFollowOnly(car(LS));
			carLS = uctxtUTForm(car(LS));
			if (utfSatisfies(carLS, carLT)) {
				l = listCons(UTFContext)(car(LT), l);
				break;
			}
			if (utfSatisfies(carLT, carLS)) {
				if (!listMember(UTFContext)(l, car(LS), uctxtEqual))
					l = listCons(UTFContext)(car(LS), l);
			}
			// We need to preserve context
			assert(uctxtIsEmpty(car(LS)));
			assert(uctxtIsEmpty(car(LT)));
		}
	}

	l = listNReverse(UTFContext)(l);
	return tpossFrTheUTFContextList(l);
}

TPoss
tpossConst(TPoss tp, TForm tf)
{
	TPossIterator tpit;
	if (tpossIsEmpty(tp))
		return tpossEmpty();

	TPoss tpNew = tpossEmpty();
	for (tpossITER(tpit, tp); tpossMORE(tpit); tpossSTEP(tpit)) {
		InferEnv env = tpossINFENV(tpit);
		tpossAdd1UTFContext(tpNew, uctxtNewConst(env, tf));
	}
	return tpNew;
}

TPoss
tpossIntersect(TPoss S, TPoss T)
{
	static int serialCount = 0;
	int serialId = serialCount++;
	UTFContextList LS, LT, l = 0;
	UTForm carLS, carLT;

	if (S == NULL || T == NULL)
		return NULL;

	tpossIntersectDEBUG(dbOut, "(TP Intersect(%d)\n", serialId);
	tpossIntersectDEBUG(dbOut, " TP Intersect(%d) - S: %pTPoss\n", serialId, S);
	tpossIntersectDEBUG(dbOut, " TP Intersect(%d) - T: %pTPoss\n", serialId, T);

	/* If T is free of duplicates, then the result will also be. */
	for (LT = T->possl; LT; LT = cdr(LT)) {
		car(LT) = uctxtFollowOnly(car(LT));

		carLT = uctxtUTForm(car(LT));
		Bool found = false;
		for (LS = S->possl; LS && !found; LS = cdr(LS)) {
			InferEnv newEnv = infEnvMerge(uctxtInfEnv(car(LT)), uctxtInfEnv(car(LS)));
			if (infEnvIsFailed(newEnv)) {
				continue;
			}

			tfkSetEnv(newEnv);
			car(LS) = uctxtFollowOnly(car(LS));
			carLS = uctxtUTForm(car(LS));
			if (tfSatisfies(utformConstOrFail(carLS), utformConstOrFail(carLT))) {
				l = listCons(UTFContext)(uctxtNew(newEnv, carLT), l);
				found = true;
				tfkClearEnv();
				break;
			}
			if (tfSatisfies(utformConstOrFail(carLT), utformConstOrFail(carLS))) {
				if (!listMember(UTFContext)(l, car(LS), uctxtEqual))
					l = listCons(UTFContext)(uctxtNew(newEnv, carLS), l);
			}

			tfkClearEnv();
		}
	}

	l = listNReverse(UTFContext)(l);
	TPoss final = tpossFrTheUTFContextList(l);
	tpossIntersectDEBUG(dbOut, " TP Intersect(%d) --> %pTPoss\n", serialId, final);
	tpossIntersectDEBUG(dbOut, " TP Intersect(%d0))\n", serialId);
	return final;
}

TPoss
tpossUnion(TPoss tp1, TPoss tp2)
{
	UTFContextList l1;
	UTFContextList l;

	if (tp1 == NULL)
		return tp2;
	else if (tp2 == NULL)
		return tp1;

	l1 = tp1->possl;
	l = listReverse(UTFContext)(tp2->possl);	/* Reversed copy */

	for (; l1; l1 = cdr(l1)) {
		car(l1) = uctxtFollowOnly(car(l1));
		/* const or fail here could be: if only one utf, then keep both
		 * otherwise utfEquality */
		if (!tpossHasUTForm(tp2, uctxtUTForm(car(l1))))
			l = listCons(UTFContext)(car(l1), l);
	}

	l = listNReverse(UTFContext)(l);
	return tpossFrTheUTFContextList(l);
}

TForm
tpossUnique(TPoss tp)
{
	car(tp->possl) = uctxtFollowOnly(car(tp->possl));
	return utformConstOrFail(uctxtUTForm(car(tp->possl)));
}

UTForm
tpossUniqueUTForm(TPoss tp)
{
	car(tp->possl) = uctxtFollowOnly(car(tp->possl));
	return uctxtUTForm(car(tp->possl));
}

UTFContext
tpossUniqueUTFContext(TPoss tp)
{
	car(tp->possl) = uctxtFollowOnly(car(tp->possl));
	return car(tp->possl);
}

Bool
tpossHas(TPoss tp, TForm t)
{
	UTFContextList l;
	Bool needsConstT = false;
	UTForm utfT = NULL;

	if (tp == NULL)
		return false;
	l = tp->possl;
	while (l != listNil(UTFContext)) {
		if (uctxtUTFIsConstant(car(l)) && t == uctxtUTFConstOrFail(car(l)))
			return true;
		else if (!uctxtUTFIsConstant(car(l))) {
			needsConstT = true;
		}
		l = cdr(l);
	}
	if (needsConstT) {
		utfT = utformNewConstant(t);
	}
	l = tp->possl;
	while (l != listNil(UTFContext)) {
		if (uctxtUTFIsConstant(car(l)) && tfEqual(t, uctxtUTFConstOrFail(car(l))))
			return true;
		else if (!uctxtUTFIsConstant(car(l))) {
			bug("unification not implemented");
			/*
			if (utformCanUnify(utfT, car(l))) {
				return true;
			}
			*/
		}
		l = cdr(l);
	}

	return false;
}

local Bool
tpossIsPending(TPoss tp, TForm t)
{
	Bool	result = false;

	if (tpossIsUnique(tp)) {
		UTForm Sutf = tpossUniqueUTForm(tp);

		if (utfIsConstant(Sutf)) {
			result = false;
		}
		else {
			TForm S = utformTForm(Sutf);
			tcSatPush(S, t);

			if (tfIsPending(S)) {
				tcNewSat(S, ablogFalse(), S, t, NULL);
				result = true;
			}
			if (tfIsPending(t)) {
				tcNewSat(t, ablogFalse(), S, t, NULL);
				result = true;
			}
			tcSatPop();
		}

	}

	return result;
}

Bool
tpossHasSatisfier(TPoss tp, TForm t)
{
	UTFContextList l;

	if (tp == NULL)
		return false;

	if (tpossIsPending(tp, t))
		return true;

	for (l = tp->possl; l; l = cdr(l))
		if (tfSatValues(uctxtUTFConstOrFail(car(l)), t))
			return true;

	return false;
}

TForm
tpossSelectSatisfier(TPoss tp, TForm t)
{
	UTFContextList l;
	TForm	  r = NULL;

	if (tp == NULL)
		return 0;

	for (l = tp->possl; l; l = cdr(l)) {
/*		car(l) = tfFollowOnly(car(l));	    */
		if (tfSatValues(uctxtUTFConstOrFail(car(l)), t)) {
			if (r) return 0;
			r = uctxtUTFConstOrFail(car(l));
		}
	}
	return r;
}

UTFContext
tpossSelectSatisfierContext(TPoss tp, TFContext tfc)
{
	UTFContextList l;
	UTFContext  r = 0;
	TForm t = ctxtTForm(tfc);
	Bool  failed = false;
	if (tp == NULL)
		return 0;

	for (l = tp->possl; l && !failed; l = cdr(l)) {
/*		car(l) = tfFollowOnly(car(l));	    */
		InferEnv newEnv = infEnvMerge(uctxtInfEnv(car(l)), ctxtInfEnv(tfc));
		tfkSetEnv(newEnv);
		if (tfSatValues(uctxtUTFConstOrFail(car(l)), t)) {
			if (!r) {
				tpossSelectSatDEBUG(dbOut, "PossSet: Success: %pInferEnv %pTForm %pTForm\n",
						    newEnv, uctxtUTFConstOrFail(car(l)), t);
				r = uctxtNew(newEnv, uctxtUTForm(car(l)));
			}
			else {
				failed = true;
				r = NULL;
				tpossSelectSatDEBUG(dbOut, "PossSat: Failed: %pInferEnv %p\n", newEnv,
						    uctxtUTFConstOrFail(car(l)), t);
			}
		}
		tfkClearEnv();
	}
	return r;
}

local UTFContext
tpossJoin(USatMask mask, UTForm S, UTForm T)
{
       S = utypeResultApplyTForm(mask->result, S);
       T = utypeResultApplyTForm(mask->result, T);
       return uctxtEmpty(tfIsThird(utformConstOrFail(S)) ? S : T);
}

TPoss
tpossSatisfies(TPoss S, TPoss T)
{
	UTFContextList LS, LT, l = listNil(UTFContext);;

	if (S == NULL || T == NULL) 
		return NULL;

	/* If T is free of duplicates, then the result will also be. */
	for (LT = T->possl; LT; LT = cdr(LT)) {
		car(LT) = uctxtFollowOnly(car(LT));
		for (LS = S->possl; LS; LS = cdr(LS)) {
			UTForm	stf = uctxtUTForm(car(LS));
			UTForm  ttf = uctxtUTForm(car(LT));

			USatMask result = utfSat(tfSatBupMask(), stf, ttf);
			if (utfSatSucceed(result)) {
				l = listCons(UTFContext)(tpossJoin(result, stf, ttf), l);
			}
		}
	}

	l = listNReverse(UTFContext)(l);
	return tpossFrTheUTFContextList(l);
}

TPoss
tpossSatisfiesType(TPoss S, TForm T)
{
	UTFContextList LS, l = listNil(UTFContext);
	UTForm uT;
	TForm  Tp;

	if (S == NULL)
		return NULL;
	tpossSatTypeDEBUG(dbOut, "(TP SatType: %pTPoss\n", S);
	tpossSatTypeDEBUG(dbOut, " TP SatType: %pTForm\n", T);
	T = tfFollowOnly(T);

	if (tfIsUnknown(T) || tpossIsPending(S, T)) {
		tpossSatTypeDEBUG(dbOut, " TP SatType: Pending)\n");
		return tpossRefer(S);
	}

	for (LS = S->possl; LS; LS = cdr(LS)) {
		InferEnv newEnv = infEnvCopy(uctxtInfEnv(car(LS)));
		tfkSetEnv(newEnv);
		if (tfSatisfies(uctxtUTFConstOrFail(car(LS)), T)) {
			l = listCons(UTFContext)(uctxtNew(newEnv, uctxtUTForm(car(LS))), l);
		}
		tfkClearEnv();
	}
	l = listNReverse(UTFContext)(l);
	TPoss res = tpossFrTheUTFContextList(l);
	tpossSatTypeDEBUG(dbOut, " TP SatType: %pTPoss)\n", res);
	return res;
}

Bool
tpossHasMapType(TPoss tp)
{
	TPossIterator	tit;

	if (tp == NULL)
		return false;

	for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit)) {
		UTForm utf = tpossUELT(tit);
		TForm tf = utformTForm(utf);
		/* Should also accept 'Ax, x' here */
		tf = tfDefineeType(tf);
		if (tfIsAnyMap(tf) || tfIsMapSyntax(tf))
			return true;
	}
	return false;
}

Bool
tpossHasNonMapType(TPoss tp)
{
	TPossIterator	tit;

	if (tp == NULL)
		return false;

	for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit)) {
		TForm tf = utformTForm(tpossUELT(tit));
		tf = tfDefineeType(tf);
		if (!tfIsAnyMap(tf) && !tfIsMapSyntax(tf))
			return true;
	}
	return false;
}

Bool
tpossIsConstant(TPoss tp)
{
	return !tpossHasAnyUTForm(tp);
}

Bool
tpossHasAnyUTForm(TPoss tp)
{
	TPossIterator	tit;

	if (tp == NULL)
		return false;

	for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit)) {
		UTForm utf = tpossUELT(tit);
		if (!utfIsConstant(utf))
			return true;
	}
	return false;
}

TPoss
tpossGeneratorArg(TPoss tpit)
{
	TPoss		tparg;
	TPossIterator	it;

	tparg = tpossEmpty();
	for (tpossITER(it,tpit); tpossMORE(it); tpossSTEP(it)) {
		TForm	t = tpossELT(it);
		tfFollow(t);
		if (tfIsGenerator(t))
			tparg = tpossAdd1(tparg, tfGeneratorArg(t));
	}
	return tparg;
}

TPoss
tpossAnyGeneratorArg(TPoss tpit)
{
	TPoss		tparg;
	TPossIterator	it;

	tparg = tpossEmpty();
	for (tpossITER(it,tpit); tpossMORE(it); tpossSTEP(it)) {
		TForm	t = tpossELT(it);
		tfFollow(t);
		if (tfIsAnyGenerator(t))
			tparg = tpossAdd1(tparg, tfAnyGeneratorArg(t));
	}
	return tparg;
}

Bool
tpossIsHaving(TPoss tp, TFormPredicate pred)
{
	TPossIterator tit;

	for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit))
		if (pred(tpossELT(tit))) return true;

	return false;
}

TPoss
tpossPattern(TPoss tp)
{
	TPoss tpNew;
	TPossIterator tit;

	tpNew = tpossEmpty();
	for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit)) {
		tpossCons(tpNew, utformNewConstant(tfPattern(tpossELT(tit))));
	}
	return tpNew;
}

TPoss
tpossPatternArg(TPoss tpit)
{
	TPoss		tparg;
	TPossIterator	it;

	tparg = tpossEmpty();
	for (tpossITER(it,tpit); tpossMORE(it); tpossSTEP(it)) {
		TForm	t = tpossELT(it);
		tfFollow(t);
		if (tfIsPattern(t))
			tparg = tpossAdd1(tparg, tfPatternArg(t));
	}
	return tparg;
}

TPoss
tpossPatternCase(TPoss tpit)
{
	TPoss		tparg;
	TPossIterator	it;

	tparg = tpossEmpty();
	for (tpossITER(it,tpit); tpossMORE(it); tpossSTEP(it)) {
		TForm	t = tpossELT(it);
		tfFollow(t);
		tparg = tpossAdd1(tparg, tfPatternCase(t));
	}
	return tparg;
}

TPoss
tpossLambda(TPoss argPoss, TPoss retPoss, AbMapType mapType)
{
	TPoss		tpL;
	TPossIterator	argIt, retIt;
	tpL = tpossEmpty();
	for (tpossITER(argIt, argPoss); tpossMORE(argIt); tpossSTEP(argIt)) {
		UTFContext arg = tpossUCELT(argIt);
		for (tpossITER(retIt, retPoss); tpossMORE(retIt); tpossSTEP(retIt)) {
			UTFContext ret = tpossUCELT(retIt);
			InferEnv ee = infEnvMerge(uctxtInfEnv(arg), uctxtInfEnv(ret));
			if (!infEnvIsFailed(ee)) {
				TForm tfL = tfAnyMap(utformConstOrFail(utformFollow(uctxtUTForm(arg))),
						     utformConstOrFail(utformFollow(uctxtUTForm(ret))), mapType);
				tpossAdd1UTFContext(tpL, uctxtNew(ee, utformNewConstant(tfL)));
			}
		}
	}
	tpossLambdaDEBUG(dbOut, "(TPossLambda\n  %pTPoss\n  %pTPoss\n", argPoss, retPoss);
	tpossLambdaDEBUG(dbOut, " TPossLambda:\n  %pTPoss)\n", tpL);

	return tpL;
}

TForm
tpossELT_(TPossIterator *ip)
{
	return utformConstOrFail(uctxtUTForm(car(ip->possl)));
}

UTForm
tpossUELT_(TPossIterator *ip)
{
	return uctxtUTForm(car(ip->possl));
}

UTFContext
tpossUCELT_(TPossIterator *ip)
{
	return car(ip->possl);
}
