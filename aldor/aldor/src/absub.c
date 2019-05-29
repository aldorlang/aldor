/*****************************************************************************
 *
 * absub.c: Semantic substitutions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "stab.h"
#include "store.h"
#include "sefo.h"
#include "lib.h"
#include "absub.h"
#include "abpretty.h"
#include "symbol.h"
#include "bigint.h"
#include "table.h"

Bool	absDebug	= false;
#define absDEBUG	DEBUG_IF(abs)	afprintf

/******************************************************************************
 *
 * :: Substitution bindings.
 *
 *****************************************************************************/

typedef void	(*AbsFreeBindingFun)	(AbBind);

local AbBind	absNewBinding		(Syme, Sefo);
local void	absFreeBinding		(AbBind);
local void	absFreeBindingDeeply	(AbBind);
local int	absPrintBinding		(FILE *, AbBind);

local AbBind
absNewBinding(Syme key, Sefo val)
{
	AbBind	b;

	b	= (AbBind) stoAlloc((unsigned) OB_AbBind, sizeof(*b));
	b->key	= key;
	b->val	= val;

	return b;
}

local void
absFreeBinding(AbBind b)
{
	stoFree((Pointer) b);
	return;
}

local void
absFreeBindingDeeply(AbBind b)
{
	abFree(b->val);
	absFreeBinding(b);
}

local int
absPrintBinding(FILE *fout, AbBind b)
{
	int	cc = 0;

	cc += fprintf(fout, "{ ");
	/* Also useful:  cc += symePrint(fout, b->key); */
	cc += fprintf(fout, "%s", symeString(b->key));
	cc += fprintf(fout, " +-> ");
	cc += abPrettyPrint(fout, b->val);
	cc += fprintf(fout, " }\n");

	return cc;
}

/******************************************************************************
 *
 * :: Substitution lists.
 *
 *****************************************************************************/

static int	absSerialNo	= 0;
static Syme	absSymeMark	= (Syme) (0xFFFFFFFF);

AbSub
absNew(Stab stab)
{
	AbSub	sigma;

	sigma	= (AbSub) stoAlloc((unsigned) OB_Other, sizeof(*sigma));

	sigma->stab	= stab;
	sigma->self	= false;
	sigma->lazy	= true;
	sigma->serialNo	= absSerialNo++;
	sigma->refc	= 1;
	sigma->l	= listNil(AbBind);
	sigma->results	= tblNew((TblHashFun) 0, (TblEqFun) 0);
	sigma->fv	= NULL;

	return sigma;
}

AbSub
absFail(void)
{
	return (AbSub) 0;
}

int
absPrint(FILE *fout, AbSub sigma)
{
	if (sigma == absFail())
		return fprintf(fout, "[ FAIL ]");
	else
		return listPrint(AbBind)(fout, sigma->l, absPrintBinding);
}

int
absOStreamWrite(OStream os, AbSub sigma)
{
	int c = 0;
	if (sigma == absFail()) {
		return ostreamPrintf(os, "{ FAIL }");
	}

	c += ostreamPrintf(os, "{(%d) ", sigma->serialNo);
	c += listFormat(AbBind)(os, "AbBind", sigma->l);
	c += ostreamPrintf(os, "}");
	return c;
}

int
abbOStreamWrite(OStream os, AbBind bind)
{
	int c = 0;
	c += ostreamPrintf(os, "%pSyme -> %pAbSyn", bind->key, bind->val);
	return c;
}

int
absPrintDb(AbSub sigma)
{
	int	rc = absPrint(dbOut, sigma);
	fnewline(dbOut);
	return rc;
}

AbSub
absRefer(AbSub sigma)
{
	++sigma->refc;
	return sigma;
}

local void
absFreeDeeply0(AbSub sigma, AbsFreeBindingFun f)
{
	if (sigma == absFail()) return;
	if (--sigma->refc > 0) return;

	listFreeDeeply(AbBind)(sigma->l, f);
	tblFree(sigma->results);
	/*!! Could free sigma->fv. */

	stoFree((Pointer) sigma);
	return;
}

void
absFree(AbSub sigma)
{
	absFreeDeeply0(sigma, absFreeBinding);
	return;
}

void
absFreeDeeply(AbSub sigma)
{
	absFreeDeeply0(sigma, absFreeBindingDeeply);
	return;
}

Bool
absIsEmpty(AbSub sigma)
{
	return sigma->l == listNil(AbBind);
}

Bool
absHasSymes(AbSub sigma, SymeList symes)
{
	if (absIsEmpty(sigma))
		return false;

	for (; symes; symes = cdr(symes)) {
		Syme	syme = car(symes), final;
		if (absLookup(syme, NULL, sigma))
			return true;
		final = absGetSyme(sigma, syme);
		if (final && final != syme)
			return true;
	}

	return false;
}

AbSub
absExtend(Syme syme, Sefo ab, AbSub sigma)
{
	assert(syme);
	if (sigma == absFail()) return sigma;

	sigma->l = listCons(AbBind)(absNewBinding(syme, ab), sigma->l);
	if (symeIsParam(syme)) {
		SymeList	symes;
		for (symes = symeTwins(syme); symes; symes = cdr(symes)) {
			Syme	twin = car(symes);
			if (twin != syme) absExtend(twin, sefoCopy(ab), sigma);
		}
	}
	return sigma;
}

Sefo
absLookup(Syme syme, Sefo fail, AbSub sigma)
{
	AbBindList	l0;

	for (l0 = sigma->l; l0; l0 = cdr(l0))
		/** if (syme == car(l0)->key) **/ /** commented by C.O. for ALMA usage**/
		    if (symeEqual(syme, car(l0)->key)) /** code introduced by C.O. for ALMA usage**/
			return car(l0)->val;

	return fail;
}

local Pointer
absSet(AbSub sigma, Pointer p, Pointer q)
{
	return (Pointer) tblSetElt(sigma->results, (TblKey) p, (TblElt) q);
}

local Pointer
absGet(AbSub sigma, Pointer p)
{
	return (Pointer) tblElt(sigma->results, (TblKey) p, (TblElt) NULL);
}

void
absSetStab(AbSub sigma, Syme syme)
{
	if (symeLib(syme) == NULL && stabLevelIsSubstable(absStab(sigma))) {
		symeSetDefLevel(syme, car(absStab(sigma)));
		symeSetHash(syme, (Hash) 0);
	}
}

Syme
absSetSyme(AbSub sigma, Syme p, Syme q)
{
	return (Syme) absSet(sigma, (Pointer) p, (Pointer) q);
}

Syme
absGetSyme(AbSub sigma, Syme p)
{
	return (Syme) absGet(sigma, (Pointer) p);
}

Syme
absMarkSyme(AbSub sigma, Syme p)
{
	absSetSyme(sigma, p, absSymeMark);
	return p;
}

Bool
absSymeIsMarked(AbSub sigma, Syme p)
{
	return absGetSyme(sigma, p) == absSymeMark;
}

TForm
absSetTForm(AbSub sigma, TForm p, TForm q)
{
	return (TForm) absSet(sigma, (Pointer) p, (Pointer) q);
}

TForm
absGetTForm(AbSub sigma, TForm p)
{
	return (TForm) absGet(sigma, (Pointer) p);
}
