/*****************************************************************************
 *
 * ablogic.c: Structures for inference about conditional exports.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "dnf.h"
#include "format.h"
#include "store.h"
#include "ti_top.h"
#include "sefo.h"
#include "tfsat.h"
#include "tform.h"
#include "ablogic.h"
#include "abpretty.h"
#include "table.h"

Bool	ablogDebug	= false;
#define ablogDEBUG	DEBUG_IF(ablog)	afprintf


/*****************************************************************************
 *
 * :: Conversion between internal and external types.
 *
 ****************************************************************************/

/*
 * The representation of this type is opaque --
 * all external access is through the fake type AbLogic
 * and all internal use is through the real type DNF.
 *
 * The ablogIn and ablogOut conversions are by local functions to
 * ensure typechecked, inexpensive coercions.
 *
 * This fire wall ensures that we do not use DNF values as AbLogic in error.
 * This is important, since we only allow aliased dnfs to be used in
 * free operations.
 */

local DNF	ablogIn (AbLogic xx) {
	return dnfFollow((DNF)xx);
}

local AbLogic	ablogOut(DNF     xx) {
	return (AbLogic) xx;
}

#if 0
local void
ablogAlias(AbLogic old, AbLogic new)
{
	dnfAlias((DNF) old, ablogIn(new));
}
#endif
/*****************************************************************************
 *
 * :: Correspondence between Sefos and DNF_Atoms.
 *
 ****************************************************************************/

local Table	ablogToTable;
local Table	ablogFrTable;
local DNF_Atom	ablogNextIx;

local void
ablogInitTables(void)
{
	ablogToTable = tblNew((TblHashFun) abHash, (TblEqFun) sefoEqual);
	ablogFrTable = tblNew((TblHashFun) 0,      (TblEqFun) 0);
	ablogNextIx  = 1;
}

local void
ablogFiniTables(void)
{
	tblFreeDeeply(ablogToTable, (TblFreeKeyFun) abFree, (TblFreeEltFun) 0);
	tblFree      (ablogFrTable);
}

local DNF_Atom
ablogToAtom(Sefo sefo)
{
	AInt	x = (AInt) tblElt(ablogToTable, (TblKey) sefo, (TblElt) 0);
	if (!x) {
		sefo = sefoCopy(sefo);
		tblSetElt(ablogToTable, (TblKey) sefo, (TblElt) (UAInt) ablogNextIx);
		tblSetElt(ablogFrTable, (TblKey) (UAInt) ablogNextIx, (TblElt) sefo);
		x    = ablogNextIx++;
	}
	return x;
}

local Sefo
ablogFrAtom(DNF_Atom lit)
{
	Sefo	sefo;

	if (lit < 0) lit = -lit;

	sefo = (Sefo) tblElt(ablogFrTable, (TblKey) (UAInt) lit, (TblElt) NULL);

	return sefo;
}


/*****************************************************************************
 *
 * :: Initialization and finalization.
 *
 ****************************************************************************/

local Bool	ablogIsInit = false;

local int ablogFormatter(OStream stream, Pointer p);

void
ablogInit(void)
{
	if (ablogIsInit) return;

	ablogInitTables();
	fmtRegister("AbLogic", ablogFormatter);
	fmtRegister("DNF", dnfFormatter);

	ablogIsInit = true;
}

void
ablogFini(void)
{
	if (!ablogIsInit) return;

	ablogFiniTables();

	ablogIsInit = false;
}


/*****************************************************************************
 *
 * :: General operations.
 *
 ****************************************************************************/
int
ablogWrite(OStream ostream, AbLogic xx0)
{
	DNF	xx = ablogIn(xx0);
	DNF_And xxi;
	int	i, j, cc = 0;
	Sefo	xxij;

	if (dnfIsFalse(xx))
		cc += ostreamWrite(ostream, ".FALSE.", -1);

	for (i = 0; i < xx->argc; i += 1) {
		xxi = xx->argv[i];

		if (i > 0) cc += ostreamWrite(ostream," .OR. ", -1);

		if (xxi->argc == 0)
			cc += ostreamWrite(ostream, ".TRUE.", -1);

		for (j = 0; j < xxi->argc; j += 1) {
			if (j > 0) cc += ostreamWrite(ostream, ".AND.", -1);
			if (xxi->argv[j] < 0)
				cc += ostreamWrite(ostream, ".NOT.", -1);
			xxij = ablogFrAtom(xxi->argv[j]);
			cc  += sefoOStreamWrite(ostream, xxij);
		}
	}

	return cc;
}

local int
ablogFormatter(OStream ostream, Pointer p)
{
	TForm tf = (TForm) p;
	int c;

	c = ablogWrite(ostream, p);

	return c;
}


/*****************************************************************************
 *
 * :: General operations.
 *
 ****************************************************************************/

AbLogic
ablogCopy(AbLogic xx)
{
	if (!xx || ablogIsTrue(xx) || ablogIsFalse(xx)) return xx;
	return ablogOut(dnfCopy(ablogIn(xx)));
}

void
ablogFree(AbLogic xx)
{
	if (!xx || ablogIsTrue(xx) || ablogIsFalse(xx)) return;
	dnfFree(ablogIn(xx));
}

int
ablogPrintDb(AbLogic xx0)
{
	int cc = ablogPrint(dbOut, xx0);
	fnewline(dbOut);
	return cc+1;
}

int
ablogPrint(FILE *fout, AbLogic xx0)
{
	DNF	xx = ablogIn(xx0);
	DNF_And xxi;
	int	i, j, cc = 0;
	Sefo	xxij;

	if (dnfIsFalse(xx))
		cc += fprintf(fout, ".FALSE.");

	for (i = 0; i < xx->argc; i += 1) {
		xxi = xx->argv[i];

		if (i > 0) cc += fprintf(fout," .OR. ");

		if (xxi->argc == 0)
			cc += fprintf(fout, ".TRUE.");

		for (j = 0; j < xxi->argc; j += 1) {
			if (j > 0) cc += fprintf(fout, ".AND.");
			if (xxi->argv[j] < 0)
				cc += fprintf(fout, ".NOT.");
			xxij = ablogFrAtom(xxi->argv[j]);
			cc  += sefoPrint(fout, xxij);
		}
	}

	return cc;
}

/******************************************************************************
 *
 * :: Maintaining an And-stack.
 *
 *****************************************************************************/

AbLogic	  abCondKnown = NULL;	  /* Conditions with known value (tinfer) */
AbLogic	  gfCondKnown = NULL;	  /* Ditto (genfoam) */

void
ablogAndPush(AbLogic *glo, AbLogic *save, Sefo cond, Bool sense)
{
	AbLogic thisCond;

	/* Save the old value of *glo and compute the new one. */
	*save = *glo;

	thisCond = ablogFrSefo(cond);
	if (!sense) {
		AbLogic tt = thisCond;
		thisCond   = ablogNot(tt);
		ablogFree(tt);
	}

	*glo = ablogAnd(*save, thisCond);
	ablogFree(thisCond);

	if (DEBUG(ablog)) {
		fprintf(dbOut, ">> Changed condition to ");
		ablogPrint(dbOut, *glo);
		fnewline(dbOut);
	}
}

void
ablogAndPop(AbLogic *glo, AbLogic *save)
{
	ablogFree(*glo);
	*glo = *save;

	if (DEBUG(ablog)) {
		fprintf(dbOut, "<< Changed back\n");
	}
}


/******************************************************************************
 *
 * :: Conversion Sefo -> AbLogic
 *
 *****************************************************************************/

AbLogic
ablogFrSefo(Sefo sefo)
{
	AbLogic	 rr, xx, yy;
	int	 i;

	switch (abTag(sefo)) {
	case AB_Not:
		xx = ablogFrSefo(sefo->abNot.expr);
		rr = ablogNot(xx);
		ablogFree(xx);
		break;
	case AB_And:
		if (abArgc(sefo) == 0)
			rr = ablogTrue();
		else {
			rr = ablogFrSefo(sefo->abAnd.argv[0]);
			for (i = 1; i < abArgc(sefo); i++) {
				xx = rr;
				yy = ablogFrSefo(sefo->abAnd.argv[i]);
				rr = ablogAnd(xx, yy);
				ablogFree(xx);
				ablogFree(yy);
			}
		}
		break;
	case AB_Or:
		if (abArgc(sefo) == 0)
			return ablogFalse();
		else {
			rr = ablogFrSefo(sefo->abOr.argv[0]);
			for (i = 1; i < abArgc(sefo); i++) {
				xx = rr;
				yy = ablogFrSefo(sefo->abOr.argv[i]);
				rr = ablogOr(xx, yy);
				ablogFree(xx);
				ablogFree(yy);
			}
		}
		break;
	case AB_Test:
		rr = ablogFrSefo(sefo->abTest.cond);
		break;
	default:
		rr = ablogOut(dnfAtom(ablogToAtom(sefo)));
		break;
	}
	return rr;
}


/*****************************************************************************
 *
 * :: Boolean arithmetic operations.
 *
 ****************************************************************************/

AbLogic
ablogTrue(void)
{
	return ablogOut(dnfTrue());
}

AbLogic
ablogFalse(void)
{
	return ablogOut(dnfFalse());
}

AbLogic
ablogOr(AbLogic xx, AbLogic yy)
{
	return ablogOut(dnfOr(ablogIn(xx), ablogIn(yy)));
}

AbLogic
ablogAnd(AbLogic xx, AbLogic yy)
{
	return ablogOut(dnfAnd(ablogIn(xx), ablogIn(yy)));
}

AbLogic
ablogNot(AbLogic xx)
{
	return ablogOut(dnfNot(ablogIn(xx)));
}

Bool
ablogIsTrue(AbLogic xx)
{
	return dnfIsTrue(ablogIn(xx));
}

Bool
ablogIsFalse(AbLogic xx)
{
	return dnfIsFalse(ablogIn(xx));
}

Bool
ablogEqual(AbLogic xx, AbLogic yy)
{
	return dnfEqual(ablogIn(xx), ablogIn(yy));
}

/******************************************************************************
 *
 * :: Test whether given forms are implied.
 *
 *****************************************************************************/

/*
 * ToDo: ablogIsListImplied should use ablogImplies.
 * ablogImplies should do some form of result caching.
 */
extern TForm	tiGetTForm		(Stab, AbSyn);

local Bool ablogIsListImpliedInner(AbLogic, SefoList, AbLogic *);
local Bool ablogExpandKnown	  (AbLogic known, Sefo sefo, AbLogic *final);
local Bool ablogAtomize		  (void *, DNF_Atom);
local Bool ablogTestAtoms	  (void *, DNF_Atom);
local Bool ablogTestProperties	  (Sefo test, Sefo know);
local Bool ablogIsListImplied0	  (AbLogic xx, SefoList sefolist);
local Bool ablogTestImplies	  (void *clos, DNF_Atom a, DNF_Atom b);

typedef struct ablogExpandClos {
	AbLogic   new;
	int	  atomc;
	int	  lim;
	DNF_Atom *atomv;
	DNF_Atom  atom;
} _AbLogExpandClos, *AbLogExpandClos;

typedef struct ablogImpliesClos {
	int cache;	/* Unused.  Need to think about
			 * good caching schemes.
			 */
} _AblogImpliesClos, *AblogImpliesClos;

Bool
ablogImplies(AbLogic known, AbLogic query)
{
	_AblogImpliesClos clos;
	Bool result;

	if (dnfImplies(ablogIn(known), ablogIn(query)))
		return true;

	result = dnfExpandImplies(ablogTestImplies, &clos,
				  ablogIn(known), ablogIn(query));

	return result;
}

/* Test for a => b */
local Bool
ablogTestImplies(void *clos, DNF_Atom a, DNF_Atom b)
{
	Sefo know, test;
	Bool result = false;

	if (a < 0) {
		int t = a;
		a = -b;
		b = -t;
	}
	/* Can't do negatives yet */
	if (a < 0 || b < 0) return false;

	know = ablogFrAtom(a);
	test = ablogFrAtom(b);

	if (DEBUG(ablog)) {
		fprintf(dbOut, "Implies test\n");
		abPrintDb(know);
		abPrintDb(test);
	}
	if (abTag(test) != AB_Has || abTag(know) != AB_Has)
		return false;

	if (sefoEqual(test->abHas.expr, know->abHas.expr))
		result = ablogTestProperties(test, know);

	/* if result, we should stash the result someplace. */
	return result;
}

Bool
ablogIsListKnown(SefoList sefolist)
{
	AbLogic new  = ablogCopy(abCondKnown);
	Bool res;
	res = ablogIsListImpliedInner(abCondKnown, sefolist, &new);
	return res;
}

Bool
ablogIsListImplied(AbLogic xx, SefoList sl)
{
	AbLogic final = ablogCopy(xx);
	Bool    result;
	result = ablogIsListImpliedInner(xx, sl, &final);
	return result;
}

local Bool
ablogIsListImpliedInner(AbLogic xx, SefoList sefolist, AbLogic *final)
{
	SefoList sl;

	if (ablogIsListImplied0(xx, sefolist))
		return true;

	sl = sefolist;
	while (sl != listNil(Sefo)) {
		if (ablogExpandKnown(xx, car(sl), final)
		    && ablogIsListImplied0(*final, sefolist))
			return true;
		sl = cdr(sl);
	}
	return false;
}



local Bool
ablogIsListImplied0(AbLogic xx, SefoList sefolist)
{
	for ( ; sefolist; sefolist = cdr(sefolist)) {
		Sefo sefo = car(sefolist);
		if (!ablogIsImplied(xx, sefo)) return false;
	}

	return true;
}

Bool
ablogIsImplied(AbLogic xx, Sefo sefo)
{
	AbLogic rhs = ablogFrSefo(sefo);

	if (dnfImplies(ablogIn(xx), ablogIn(rhs)))
		return true;

	return false;
}

local Bool
ablogExpandKnown(AbLogic known, Sefo sefo, AbLogic *final)
{
	AbLogic rhs;
	_AbLogExpandClos clos;
	Bool res;
	int  i;

	/* Find all the atoms in rhs */
	rhs = ablogFrSefo(sefo);

	clos.new = ablogTrue();
	clos.atomc = 0;
	clos.atomv = NULL;

	dnfMap(ablogAtomize, (void*) &clos, ablogIn(rhs));

	for (i=0; i<clos.atomc; i++) {
		clos.atom = clos.atomv[i];
		dnfMap(ablogTestAtoms, (void*) &clos, ablogIn(known));
	}

	*final = ablogAnd(known, clos.new);
	res = !ablogEqual(clos.new, ablogTrue());
	ablogFree(clos.new);
	if (clos.atomv) stoFree(clos.atomv);

	return res;
}

local Bool
ablogAtomize(void *ptr, DNF_Atom atom)
{
	AbLogExpandClos clos = (AbLogExpandClos) ptr;

	if (clos->atomv == NULL) {
		clos->atomv = (DNF_Atom *) stoAlloc(OB_Other, 5*sizeof(DNF_Atom));
		clos->lim = 5;
	}

	/* Grow slowly */
	if (clos->atomc == clos->lim) {
		clos->atomv = (DNF_Atom*)
			stoResize(clos->atomv,
				  (clos->lim + 5)*sizeof(DNF_Atom));
		clos->lim += 5;
	}
	clos->atomv[clos->atomc++] = atom;
	return false; /* false => dnfMap continues */
}


local Bool
ablogTestAtoms(void *ptr, DNF_Atom known)
{
	AbLogExpandClos clos = (AbLogExpandClos) ptr;
	Sefo  test = ablogFrAtom(clos->atom);
	Sefo  know = ablogFrAtom(known);
	Bool  result = false;

	if (abTag(test) != AB_Has || abTag(know) != AB_Has)
		return false;

	/*
	 * This test is a bit strong,
	 * eg. if A then Foo(A)
	 */
	if (sefoEqual(test->abHas.expr, know->abHas.expr))
		result = ablogTestProperties(test, know);
	if (result) {
		clos->new = ablogAnd(clos->new, ablogOut(dnfAtom(clos->atom)));
		return true;
	}
	return false;
}

local Bool
ablogTestProperties(Sefo test, Sefo know)
{
	TForm tftest, tfknown;
	Bool  result;
	test = test->abHas.property,
	know = know->abHas.property;

	tftest  = abTForm(test) ?
		abTForm(test) : (tiTopFns()->tiGetTopLevelTForm)(NULL, test);
	tfknown = abTForm(know) ?
		abTForm(know) : (tiTopFns()->tiGetTopLevelTForm)(NULL, know);

	if (DEBUG(ablog)){
		fprintf(dbOut, "Checking: \n");
		tfPrintDb(tftest);
		tfPrintDb(tfknown);
	}

	result = tfSatBit(tfSatHasMask(), tfknown, tftest);
	return result;
}

int
bputAblog(Buffer buf, AbLogic abl)
{
	/* Hacked from ablogPrint() */
	DNF	xx = ablogIn(abl);
	DNF_And xxi;
	int	i, j, cc = 0;
	Sefo	xxij;

	if (dnfIsFalse(xx))
		cc += bufPrintf(buf, "false");

	for (i = 0; i < xx->argc; i += 1) {
		xxi = xx->argv[i];

		if (i > 0) cc += bufPrintf(buf," \\/ ");

		if (xxi->argc == 0)
			cc += bufPrintf(buf, "true");

		for (j = 0; j < xxi->argc; j += 1) {
			if (j > 0) cc += bufPrintf(buf, " /\\ ");
			if (xxi->argv[j] < 0)
				cc += bufPrintf(buf, "not ");
			xxij = ablogFrAtom(xxi->argv[j]);
			cc  += bufPrintf(buf, "%s", abPretty(xxij));
		}
	}

	return cc;
}

/*
 * :: Categorical implications
 */

typedef struct ablogImpliedClos {
	Sefo lhs;
	TFormList list;
} *AblogImpliedClos;

local Bool ablogAtomImpliedType(void *ptr, DNF_Atom atom);

TForm
ablogImpliedType(AbLogic known, AbSyn lhs, TForm type)
{
	TForm tf;
	struct ablogImpliedClos clos;
	clos.lhs = lhs;
	clos.list = listNil(TForm);

	dnfMap(ablogAtomImpliedType, &clos, ablogIn(known));

	if (clos.list == listNil(TForm))
		return NULL;

	tf = tfJoinFrList(listCons(TForm)(type, clos.list));

	return tf;
}


local Bool
ablogAtomImpliedType(void *ptr, DNF_Atom atom)
{
	AblogImpliedClos clos = (AblogImpliedClos) ptr;
	Sefo known = ablogFrAtom(atom);
	AbSyn lhs, rhs;

	if (abTag(known) != AB_Has)
		return false;

	lhs = known->abHas.expr;
	rhs = known->abHas.property;

	if (abEqual(lhs, clos->lhs)) {
		clos->list = listCons(TForm)(abTForm(rhs), clos->list);
	}

	return false;
}
