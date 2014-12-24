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

#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "spesym.h"
#include "store.h"
#include "terror.h"
#include "syme.h"
#include "tconst.h"
#include "tposs.h"
#include "tfsat.h"
#include "utype.h"

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
	tp->possl = listNil(UTForm);
	tp->possc = 0;
	tp->refc  = 1;
	return tp;
}

local void
tpossCons(TPoss tp, UTForm t)
{
	assert(tp);
	t = utformFollowOnly(t);
	tp->possl = listCons(UTForm)(t, tp->possl);
	tp->possc += 1;
}

TPoss
tpossFrTheList(TFormList l)
{
	TPoss tp  = tpossAlloc();
	tp->possc = listLength(TForm)(l);
	tp->refc  = 1;
	tp->possl = listNil(UTForm);
	while (l != listNil(TForm)) {
		tp->possl = listCons(UTForm)(utformNewConstant(car(l)), tp->possl);
		l = cdr(l);
	}
	listNReverse(UTForm)(tp->possl);
	return tp;
}

TPoss
tpossFrTheUTFormList(UTFormList l)
{
	TPoss tp  = tpossAlloc();
	tp->possl = l;
	tp->possc = listLength(UTForm)(l);
	tp->refc  = 1;
	return tp;
}

TPoss
tpossSingleton(TForm t)
{
	TPoss tp  = tpossEmpty();
	tpossCons(tp, utformNewConstant(t));
	return tp;
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
tposs0Multi(TPoss tp,Length k,TFormList tl,Length n,Pointer v,TPossGetter get)
{
	if (k == 0)
		tpossCons(tp, utformNewConstant(tfMultiFrList(tl)));

	else {
		TPoss		tpk = get(v, k-1);
		TPossIterator	tpi;

		tl = listCons(TForm)(NULL, tl);
		for (tpossITER(tpi,tpk); tpossMORE(tpi); tpossSTEP(tpi)) {
			TForm t = tpossELT(tpi);
			t = tfFollowOnly(t);
			setcar(tl, t);
			tposs0Multi(tp, k-1, tl, n, v, get);
		}
		listFreeCons(TForm)(tl);
		tpossFree(tpk);
	}
}

TPoss
tpossMulti(Length argc, Pointer argv, TPossGetter get)
{
	TPoss	tp = tpossEmpty();

	tposs0Multi(tp, argc, listNil(TForm), argc, argv, get);

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
	np->possl = listCopy(UTForm)(tp->possl);
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
if (!debugflag)
{
	listFree(UTForm)(tp->possl);
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
			return listPrint(UTForm)(fout, tp->possl, utformPrint);
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
			return ostreamPrintf(ostream, "[TP: %d %pTFormList]", tp->possc, tp->possl);
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
tpossIntersect(TPoss S, TPoss T)
{
	UTFormList LS, LT, l = 0;

	if (S == NULL || T == NULL)
		return NULL;

	/* If T is free of duplicates, then the result will also be. */
	for (LT = T->possl; LT; LT = cdr(LT)) {
		car(LT) = utformFollowOnly(car(LT));
		for (LS = S->possl; LS; LS = cdr(LS)) {
			car(LS) = utformFollowOnly(car(LS));
			if (utfSatisfies(car(LS), car(LT))) {
				l = listCons(UTForm)(car(LT), l);
				break;
			}
			if (utfSatisfies(car(LT), car(LS))) {
				if (!listMember(UTForm)(l, car(LS), utformEqual))
					l = listCons(UTForm)(car(LS), l);
			}
		}
	}

	l = listNReverse(UTForm)(l);
	return tpossFrTheUTFormList(l);
}

TPoss
tpossUnion(TPoss tp1, TPoss tp2)
{
	UTFormList l1;
	UTFormList l;

	if (tp1 == NULL)
		return tp2;
	else if (tp2 == NULL)
		return tp1;

	l1 = tp1->possl;
	l = listReverse(UTForm)(tp2->possl);	/* Reversed copy */

	for (; l1; l1 = cdr(l1)) {
		car(l1) = utformFollowOnly(car(l1));
		/* const or fail here could be: if only one utf, then keep both
		 * otherwise utfEquality */
		if (!tpossHas(tp2, utformConstOrFail(car(l1))))
			l = listCons(UTForm)(car(l1), l);
	}

	l = listNReverse(UTForm)(l);
	return tpossFrTheUTFormList(l);
}

TForm
tpossUnique(TPoss tp)
{
	car(tp->possl) = utformFollowOnly(car(tp->possl));
	return utformConstOrFail(car(tp->possl));
}

UTForm
tpossUniqueUTForm(TPoss tp)
{
	car(tp->possl) = utformFollowOnly(car(tp->possl));
	return car(tp->possl);
}

Bool
tpossHas(TPoss tp, TForm t)
{
	UTFormList l;

	if (tp == NULL)
		return false;
	l = tp->possl;
	while (l != listNil(UTForm)) {
		if (t == utformConstOrFail(car(l)))
			return true;
		l = cdr(l);
	}

	l = tp->possl;
	while (l != listNil(UTForm)) {
		if (tfEqual(t, utformConstOrFail(car(l))))
			return true;
		l = cdr(l);
	}

	return false;
}

local Bool
tpossIsPending(TPoss tp, TForm t)
{
	Bool	result = false;

	if (tpossIsUnique(tp)) {
		TForm	S = tpossUnique(tp);

		tcSatPush(S, t);

		if (tfIsPending(S)) {
			tcNewSat(S, S, t, NULL);
			result = true;
		}
		if (tfIsPending(t)) {
			tcNewSat(t, S, t, NULL);
			result = true;
		}

		tcSatPop();
	}

	return result;
}

Bool
tpossHasSatisfier(TPoss tp, TForm t)
{
	UTFormList l;

	if (tp == NULL)
		return false;

	if (tpossIsPending(tp, t))
		return true;

	for (l = tp->possl; l; l = cdr(l))
		if (tfSatValues(utformConstOrFail(car(l)), t))
			return true;

	return false;
}

TForm
tpossSelectSatisfier(TPoss tp, TForm t)
{
	UTFormList l;
	TForm	  r = 0;

	if (tp == NULL)
		return 0;

	for (l = tp->possl; l; l = cdr(l)) {
/*		car(l) = tfFollowOnly(car(l));	    */
		if (tfSatValues(utformConstOrFail(car(l)), t)) {
			if (r) return 0;
			r = utformConstOrFail(car(l));
		}
	}
	return r;
}

local UTForm
tpossJoin(UTForm S, UTForm T)
{
	return tfIsThird(utformConstOrFail(S)) ? S : T;
}

TPoss
tpossSatisfies(TPoss S, TPoss T)
{
	UTFormList LS, LT, l = 0;

	if (S == NULL || T == NULL) 
		return NULL;

	/* If T is free of duplicates, then the result will also be. */
	for (LT = T->possl; LT; LT = cdr(LT)) {
		car(LT) = utformFollowOnly(car(LT));
		for (LS = S->possl; LS; LS = cdr(LS)) {
			TForm	stf = utformConstOrFail(car(LS));
			TForm   ttf = utformConstOrFail(car(LT));
			if (tfSatBit(tfSatBupMask(), stf, ttf))
				l = listCons(UTForm)(tpossJoin(car(LS), car(LT)), l);
		}
	}

	l = listNReverse(UTForm)(l);
	return tpossFrTheUTFormList(l);
}

TPoss
tpossSatisfiesType(TPoss S, TForm T)
{
	UTFormList LS, l = 0;

	if (S == NULL)
		return NULL;

	T = tfFollowOnly(T);

	if (tfIsUnknown(T) || tpossIsPending(S, T))
		return tpossRefer(S);

	for (LS = S->possl; LS; LS = cdr(LS))
		if (tfSatisfies(utformConstOrFail(car(LS)), T))
			l = listCons(UTForm)(car(LS), l);

	l = listNReverse(UTForm)(l);
	return tpossFrTheUTFormList(l);
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

Bool
tpossIsHaving(TPoss tp, TFormPredicate pred)
{
	TPossIterator tit;

	for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit))
		if (pred(tpossELT(tit))) return true;

	return false;
}

TForm
tpossELT_(TPossIterator *ip)
{
	return utformConstOrFail(car(ip->possl));
}
