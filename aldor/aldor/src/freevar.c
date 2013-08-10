/*****************************************************************************
 *
 * freevar.c: Free variable lists for type forms.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "store.h"
#include "sefo.h"
#include "freevar.h"
#include "absub.h"
#include "table.h"

Bool	fvDebug		= false;
#define fvDEBUG		if (DEBUG(fv))

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

#define	fvAlloc()	((FreeVar) stoAlloc(OB_Other, sizeof(struct fvar)))

local FreeVar
fvFrTheSymes(SymeList symes)
{
	FreeVar		fv = fvAlloc();

	fv->symes	= symes;
	fv->skip	= false;

	return fv;
}

FreeVar
fvFrSymes(SymeList symes)
{
	FreeVar		fv;

	if (symes == listNil(Syme))
		fv = fvEmpty();
	else if (cdr(symes) == listNil(Syme)) {
		fv = fvSingleton(car(symes));
		listFree(Syme)(symes);
	}
	else
		fv = fvFrTheSymes(symes);

	return fv;
}

FreeVar
fvEmpty(void)
{
	static FreeVar	fv = 0;

	if (fv == 0)
		fv = fvFrTheSymes(listNil(Syme));

	assert(fv);
	return fv;
}

FreeVar
fvSingleton(Syme syme)
{
	static Table	tbl = 0;
	FreeVar		fv = 0;

	if (tbl == 0)
		tbl = tblNew((TblHashFun) 0, (TblEqFun) 0);

	assert(tbl);
	fv = (FreeVar) tblElt(tbl, syme, NULL);
	if (fv == 0) {
		fv = fvFrTheSymes(listCons(Syme)(syme, listNil(Syme)));
		tblSetElt(tbl, syme, fv);
	}

	assert(fv);
	return fv;
}

FreeVar
fvUnion(FreeVar fv1, FreeVar fv2)
{
	FreeVar		fv;

	if (fv1 == fvEmpty())
		fv = fv2;
	else if (fv2 == fvEmpty())
		fv = fv1;
	else
		fv = fvFrTheSymes(symeListUnion(fv1->symes,fv2->symes,symeEq));

	return fv;
}

int
fvPrint(FILE * fout, FreeVar fv)
{
	return listPrint(Syme)(fout, fv->symes, symePrint);
}

Bool
fvHasSyme(FreeVar fv, Syme syme)
{
	return listMemq(Syme)(fv->symes, syme);
}

Bool
fvHasAbSub(FreeVar fv, AbSub sigma)
{
	return absHasSymes(sigma, fv->symes);
}
