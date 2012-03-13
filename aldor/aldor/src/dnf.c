/*****************************************************************************
 *
 * dnf.c: Disjunctive normal form for boolean expressions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"

Bool	dnfDebug	= false;
#define dnfDEBUG(s)	DEBUG_IF(dnfDebug, s)

/*****************************************************************************
 *
 * :: Forward declarations for local functions.
 *
 ****************************************************************************/

local Bool	dnfAtomLT		(DNF_Atom, DNF_Atom);

local DNF_And	dnfAndNew		(int argc);
local DNF_And	dnfAndCopy		(DNF_And);
local void	dnfAndFree		(DNF_And);
local Bool	dnfAndIsTrue		(DNF_And);
local DNF_And	dnfAndMerge		(DNF_And, DNF_And);
local Bool	dnfAndImplies		(DNF_And, DNF_And);
local DNF	dnfAndNot		(DNF_And);

local DNF	dnfOrNew		(int argc);
local DNF	dnfOrCopy		(DNF);
local void	dnfOrFree		(DNF);
local Bool	dnfOrIsFalse		(DNF);
local void	dnfOrMerge		(DNF);


/*****************************************************************************
 *
 * :: Atoms
 *
 ****************************************************************************/

local Bool
dnfAtomLT(DNF_Atom l1, DNF_Atom l2)
{
	if (l1 < 0) l1 = -l1;
	if (l2 < 0) l2 = -l2;
	return l1 < l2;
}


/*****************************************************************************
 *
 * :: Conjunctions
 *
 ****************************************************************************/

local DNF_And
dnfAndNew(int argc)
{
	int	i;
	DNF_And	xx;

	xx = (DNF_And) stoAlloc(OB_DNF, fullsizeof(*xx, argc, DNF_Atom));

	xx->argc = argc;
	for (i = 0; i < argc; i += 1)
		xx->argv[i] = 0;

	dnfDEBUG(fprintf(dbOut, ">dnfAndNew: %p[%d]\n", xx, argc));

	return xx;
}

local DNF_And
dnfAndCopy(DNF_And xx)
{
	int	i;
	DNF_And yy;

#if EDIT_1_0_n1_07
	dnfDEBUG(fprintf(dbOut, ">dnfAndCopy: %p[%d]\n", xx, (int) xx->argc));
#else
	dnfDEBUG(fprintf(dbOut, ">dnfAndCopy: %p[%d]\n", xx, xx->argc));
#endif

	yy = dnfAndNew(xx->argc);
	for (i = 0; i < xx->argc; i += 1)
		yy->argv[i] = xx->argv[i];

	return yy;
}

local void
dnfAndFree(DNF_And xx)
{
#if EDIT_1_0_n1_07
	dnfDEBUG(fprintf(dbOut, ">dnfAndFree: %p[%d]\n", xx, (int) xx->argc));
#else
	dnfDEBUG(fprintf(dbOut, ">dnfAndFree: %p[%d]\n", xx, xx->argc));
#endif
	stoFree((Pointer) xx);
}

local Bool
dnfAndIsTrue(DNF_And xx)
{
	return xx->argc == 0;
}

/*
 * Combine xx and yy into a new conjunction.
 * Remove redundant literals along the way.
 * Return NULL if a literal and its negation are found.
 */
local DNF_And
dnfAndMerge(DNF_And xx, DNF_And yy)
{
	DNF_And	rr;
	int	xxi, yyi, rri;

	if (dnfAndIsTrue(xx))
		return dnfAndCopy(yy);
	if (dnfAndIsTrue(yy))
		return dnfAndCopy(xx);

	rr = dnfAndNew(xx->argc + yy->argc);
	rri = xxi = yyi = 0;

	/* Merge, preserving order, removing redundant literals. */
	while (xxi < xx->argc && yyi < yy->argc) {
		if (dnfAtomLT(xx->argv[xxi], yy->argv[yyi]))
			rr->argv[rri++] = xx->argv[xxi++];
		else if (dnfAtomLT(yy->argv[yyi], xx->argv[xxi]))
			rr->argv[rri++] = yy->argv[yyi++];
		else if (xx->argv[xxi] == yy->argv[yyi]) {
			xxi += 1;
		}
		else {
			assert(xx->argv[xxi] == - yy->argv[yyi]);
			dnfAndFree(rr);
			return NULL;
		}
	}
	while (xxi < xx->argc)
		rr->argv[rri++] = xx->argv[xxi++];
	while (yyi < yy->argc)
		rr->argv[rri++] = yy->argv[yyi++];
	rr->argc = rri;

	return rr;
}

/*
 * This tests to see if xx implies yy.
 * If xx => yy then (xx or yy) == yy.
 */


local Bool
dnfAndImplies(DNF_And xx, DNF_And yy)
{
	int	xxi, yyi;

	/* xx implies yy if each atom in yy can be found in xx. */
	if (xx->argc < yy->argc)
		return false;

	xxi = yyi = 0;
	for (xxi = yyi = 0; xxi < xx->argc && yyi < yy->argc; ) {
		DNF_Atom xxa = xx->argv[xxi];
		DNF_Atom yya = yy->argv[yyi];

		if (dnfAtomLT(xxa, yya))
			xxi += 1;	/* Keep looking for yyi. */

		else if (xxa == yya) {
			xxi += 1;	/* Found yyi. */
			yyi += 1;
		}
		else
			return false;	/* Failed to find yyi. */
	}

	/* Return true if we found each of the yyi. */
	return yyi == yy->argc;
}

local DNF
dnfAndNot(DNF_And xx)
{
	DNF	rr = dnfOrNew(xx->argc);
	int	i;

	for (i = 0; i < xx->argc; i += 1) {
		rr->argv[i] = dnfAndNew(1);
		rr->argv[i]->argv[0] = - xx->argv[i];
	}

	return rr;
}


/*****************************************************************************
 *
 * :: Disjunctions
 *
 ****************************************************************************/

local DNF
dnfOrNew(int argc)
{
	int	i;
	DNF	xx;

	xx = (DNF) stoAlloc(OB_DNF, fullsizeof(*xx, argc, DNF_And));

	xx->argc = argc;
	for (i = 0; i < argc; i += 1)
		xx->argv[i] = 0;

	dnfDEBUG(fprintf(dbOut, ">dnfOrNew: %p[%d]\n", xx, argc));

	return xx;
}

local DNF
dnfOrCopy(DNF xx)
{
	int	i;
	DNF	yy;

	dnfDEBUG(fprintf(dbOut, ">dnfOrCopy: %p[%d]\n", xx, xx->argc));

	yy = dnfOrNew(xx->argc);
	for (i = 0; i < xx->argc; i += 1)
		yy->argv[i] = dnfAndCopy(xx->argv[i]);

	return yy;
}

local void
dnfOrFree(DNF xx)
{
	int	i;

	dnfDEBUG(fprintf(dbOut, ">dnfOrFree: %p[%d]\n", xx, xx->argc));

	for (i = 0; i < xx->argc; i += 1)
		dnfAndFree(xx->argv[i]);

	stoFree((Pointer) xx);
}

local Bool
dnfOrIsFalse(DNF xx)
{
	return xx->argc == 0;
}

/*
 * Modify xx to merge redundant disjuncts.
 */
local void
dnfOrMerge(DNF xx)
{
	int	i, j;

	dnfDEBUG(fprintf(dbOut, ">dnfOrMerge: %p[%d]\n", xx, xx->argc));

	/* As we work, terms are merged by replacing them with NULL. */
	for (i = 0; i < xx->argc; i += 1) {
		for (j = 0; j < xx->argc; j += 1) {
			/* If xxi => xxj then (xxi or xxj) == xxj. */
			if (i != j && xx->argv[i] && xx->argv[j] &&
			    dnfAndImplies(xx->argv[i], xx->argv[j])) {
				dnfAndFree(xx->argv[i]);
				xx->argv[i] = NULL;
			}
		}
	}

	/* Now squeeze out NULLs. */
	for (i = 0, j = 0; j < xx->argc; j += 1)
		if (xx->argv[j])
			xx->argv[i++] = xx->argv[j];
	xx->argc = i;

	dnfDEBUG(fprintf(dbOut, "<dnfOrMerge: %p[%d]\n", xx, xx->argc));
}


/*****************************************************************************
 *
 * :: True and False.
 *
 ****************************************************************************/
			
local struct dnf_And 	dnfTrueProd    	= { 0 };
local struct dnf_Or  	dnfTrueStruct  	= { 1, { &dnfTrueProd }};
local DNF	      	dnfTrueValue   	= &dnfTrueStruct;

local struct dnf_Or  	dnfFalseStruct 	= { 0 };
local DNF	      	dnfFalseValue  	= &dnfFalseStruct;


DNF
dnfTrue(void)
{
	return dnfTrueValue;
}

DNF
dnfFalse(void)
{
	return dnfFalseValue;
}

Bool
dnfIsTrue(DNF xx)
{
	return xx->argc == 1 && dnfAndIsTrue(xx->argv[0]);
}

Bool
dnfIsFalse(DNF xx)
{
	return dnfOrIsFalse(xx);
}


/*****************************************************************************
 *
 * :: Atoms
 *
 ****************************************************************************/

DNF
dnfAtom(DNF_Atom atom)
{
	DNF     xx;

	dnfDEBUG(fprintf(dbOut, ">dnfAtom: %d\n", atom));

	xx  = dnfOrNew(1);
	xx->argv[0] = dnfAndNew(1);
	xx->argv[0]->argv[0] = atom;

	return xx;
}

DNF
dnfNotAtom(DNF_Atom atom)
{
	return dnfAtom(-atom);
}


/*****************************************************************************
 *
 * :: Or
 *
 ****************************************************************************/

DNF
dnfOr(DNF xx, DNF yy)
{
	DNF	rr;
	int	i, rri;

	if (dnfIsTrue(xx) || dnfIsTrue(yy))
		return dnfTrue();

	if (dnfIsFalse(xx))
		return dnfCopy(yy);
	if (dnfIsFalse(yy))
		return dnfCopy(xx);

	dnfDEBUG(fprintf(dbOut, ">dnfOr: %p[%d] %p[%d]\n",
			 xx, xx->argc, yy, yy->argc));

	rr  = dnfOrNew(xx->argc + yy->argc);
	rri = 0;

	for (i = 0; i < xx->argc; i += 1)
		rr->argv[rri++] = dnfAndCopy(xx->argv[i]);
	for (i = 0; i < yy->argc; i += 1)
		rr->argv[rri++] = dnfAndCopy(yy->argv[i]);

	dnfOrMerge(rr);

	dnfDEBUG(fprintf(dbOut, "<dnfOr: %p[%d]\n", rr, rr->argc));

	return rr;
}


/*****************************************************************************
 *
 * :: And
 *
 ****************************************************************************/

DNF
dnfAnd(DNF xx, DNF yy)
{
	DNF	rr;
	int	i, j, rri;

	if (dnfIsFalse(xx) || dnfIsFalse(yy))
		return dnfFalse();

	if (dnfIsTrue(xx))
		return dnfCopy(yy);
	if (dnfIsTrue(yy))
		return dnfCopy(xx);

	dnfDEBUG(fprintf(dbOut, ">dnfAnd: %p[%d] %p[%d]\n",
			 xx, xx->argc, yy, yy->argc));

	rr  = dnfOrNew(xx->argc * yy->argc);
	rri = 0;

	for (i = 0; i < xx->argc; i += 1)
		for (j = 0; j < yy->argc; j += 1, rri += 1)
			rr->argv[rri] = dnfAndMerge(xx->argv[i], yy->argv[j]);

	dnfOrMerge(rr);

	dnfDEBUG(fprintf(dbOut, "<dnfAnd: %p[%d]\n", rr, rr->argc));

	return rr;
}


/*****************************************************************************
 *
 * :: Not
 *
 ****************************************************************************/

DNF
dnfNot(DNF xx)
{
	DNF	aa, bb, rr;
	int	i;

	if (dnfIsFalse(xx))
		return dnfTrue();
	if (dnfIsTrue(xx))
		return dnfFalse();

	dnfDEBUG(fprintf(dbOut, ">dnfNot: %p[%d]\n", xx, xx->argc));

	rr = dnfTrue();
	for (i = 0; i < xx->argc; i += 1) {
		aa = rr;
		bb = dnfAndNot(xx->argv[i]);

		rr = dnfAnd(aa, bb);
		dnfFree(aa);
		dnfFree(bb);
	}

	dnfDEBUG(fprintf(dbOut, "<dnfNot: %p[%d]\n", rr, rr->argc));

	return rr;
}


/*****************************************************************************
 *
 * :: General operations.
 *
 ****************************************************************************/

DNF
dnfCopy(DNF xx)
{
	return dnfOrCopy(xx);
}

void
dnfFree(DNF xx)
{
	if (xx->argc == -1)
		stoFree(xx);

	if (ptrNE(xx, dnfTrueValue) && ptrNE(xx, dnfFalseValue))
		dnfOrFree(xx);
}

int
dnfPrint(FILE *fout, DNF xx)
{
	DNF_And	xxi;
	int	i, j, cc = 0;

	cc = fprintf(fout, "DNF{");
	for (i = 0; i < xx->argc; i += 1) {
		xxi = xx->argv[i];
		if (i > 0) cc += fprintf(fout, " ");
		cc += fprintf(fout, "[");
		for (j = 0; j < xxi->argc; j += 1) {
			if (j > 0) cc += fprintf(fout, " ");
			cc += fprintf(fout, "%d", xxi->argv[j]);
		}
		cc += fprintf(fout, "]");
	}
	cc += fprintf(fout, "}");
	return cc;
}


/******************************************************************************
 *
 * :: Equal
 *
 *****************************************************************************/

Bool
dnfEqual(DNF xx, DNF yy)
{
	if (xx == yy) return true;
	return dnfImplies(xx, yy) && dnfImplies(yy, xx);
}

/*
 * xx => yy if each disjunct in xx implies a disjunct in yy.
 */
Bool
dnfImplies(DNF xx, DNF yy)
{
	Bool	result = true;
	int	i, j;

	for (i = 0; result && i < xx->argc; i += 1) {
		result = false;
		for (j = 0; !result && j < yy->argc; j += 1)
			result = dnfAndImplies(xx->argv[i], yy->argv[j]);
	}
	
	return result;
}

/******************************************************************************
 *
 * :: Mapping
 *
 *****************************************************************************/
local Bool
dnfExpandAndImplies(Bool (*testFn)(void *, DNF_Atom, DNF_Atom),
		    void *clos, DNF_And xx, DNF_And yy);

void
dnfMap(Bool (*mapFn)(void*, DNF_Atom), void * clos, DNF xx)
{
	DNF_And xxi;
	int i, j;

	for (i=0; i<xx->argc; i++) {
		xxi = xx->argv[i];
		for (j=0; j < xxi->argc; j++) 
			if (mapFn(clos, xxi->argv[j]))
				return;
	}
}

Bool
dnfExpandImplies(Bool (*testFn)(void *, DNF_Atom, DNF_Atom), 
		 void *clos,
		 DNF xx, DNF yy)
{
	Bool	result = true;
	int	i, j;

	for (i = 0; result && i < xx->argc; i += 1) {
		result = false;
		for (j = 0; !result && j < yy->argc; j += 1)
			result = dnfExpandAndImplies(testFn, clos,
						     xx->argv[i], yy->argv[j]);
	}
	
	return result;
}

local Bool
dnfExpandAndImplies(Bool (*testFn)(void *, DNF_Atom, DNF_Atom),
		    void *clos, DNF_And xx, DNF_And yy)
{
	int	xxi, yyi;

	/* xx implies yy if each atom in yy can be found in xx. */
	if (xx->argc < yy->argc)
		return false;

	xxi = yyi = 0;
	for (xxi = yyi = 0; xxi < xx->argc && yyi < yy->argc; ) {
		DNF_Atom xxa = xx->argv[xxi];
		DNF_Atom yya = yy->argv[yyi];

		if (xxa == yya) {
			xxi += 1;	/* Found yyi. */
			yyi += 1;
		}
		else {	
			/* Be a bit more enthusiastic */
			Bool res = false;
			int  ti;
			for (ti=0; (!res) && ti < xx->argc; ti++) {
				xxa = xx->argv[ti];
				res = (*testFn)(clos, xxa, yya);
			}
			if (!res)
				return false;	/* Failed to find yyi. */
			yyi++;
		}
	}

	/* Return true if we found each of the yyi. */
	return yyi == yy->argc;
}

		 
/******************************************************************************
 *
 * :: Aliasing
 * !!! This is currently broken, so don't try to use it.
 *****************************************************************************/

DNF 
dnfFollow(DNF dnf)
{
#if 0
	if (dnf->argc != -1) 
		return dnf;
	else {
		dnf->argv[0] = dnfFollow( (DNF) dnf->argv[0]);
		return (DNF) dnf->argv[0];
	}
#endif
	return dnf;
}

void
dnfAlias(DNF old, DNF new)
{
	int i;
	/* If we start wanting to alias false (unlikely),
	 * then fix min. alloc to be 1 slot.
	 */
	if (old->argc != -1) {
		assert(old->argc != 0);
		for (i=0; i < old->argc; i++) 
			dnfAndFree(old->argv[i]);
		old->argc = -1;
	}
	old->argv[0] = (DNF_And) new;
}
