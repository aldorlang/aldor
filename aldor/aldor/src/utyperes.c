#include "absub.h"
#include "axlobs.h"
#include "debug.h"
#include "sefo.h"
#include "stab.h"
#include "store.h"
#include "utype.h"
#include "utyperes.h"

CREATE_LIST(UTypeSubst);

static struct utypeResult empty = { listNil(UTypeSubst) };

UTypeResult
utypeResultFailed()
{
	return NULL;
}

UTypeResult
utypeResultEmpty()
{
	return &empty;
}

Bool
utypeResultIsFail(UTypeResult res)
{
	return res == NULL;
}

Bool
utypeResultIsEmpty(UTypeResult res)
{
	if (utypeResultIsFail(res))
		return false;

	return res->substs == listNil(UTypeSubst);
}

UTypeResult
utypeResultCopy(UTypeResult res)
{
	UTypeResult res2 = utypeResultNew();
	res2->substs = listCopyDeeply(UTypeSubst)(res->substs, utypeSubstCopy);
	res2->news = listCopy(Syme)(res->news);
	res2->conditions = listCopy(Sefo)(res->conditions);

	return res2;
}

UTypeSubst
utypeSubstNew(Syme syme, Sefo sefo) {
	UTypeSubst subst = (UTypeSubst) stoAlloc(OB_Other, sizeof(*subst));
	subst->syme = syme;
	subst->sefo = sefo;
	return subst;
}

UTypeSubst
utypeSubstCopy(UTypeSubst subst)
{
	return utypeSubstNew(subst->syme, subst->sefo);
}

void
utypeSubstFree(UTypeSubst subst)
{
	stoFree(subst);
}

Sefo
utypeSubstApply(UTypeSubst subst, Sefo sefo)
{
	AbSub sigma = absNew(stabFile());
	sigma = absExtend(subst->syme, subst->sefo, sigma);
	return sefoSubst(sigma, sefo);
}


UTypeSubst
utypeResultSubst(UTypeResult result, Syme syme)
{
	UTypeSubstList substs = result->substs;
	while (substs != listNil(UTypeSubst)) {
		if (car(substs)->syme == syme)
			return car(substs);
		substs = cdr(substs);
	}
	return NULL;
}

SymeList
utypeResultSymes(UTypeResult result)
{
	UTypeSubstList substs = result->substs;
	SymeList symes = listNil(Syme);

	while (substs != listNil(UTypeSubst)) {
		symes = listCons(Syme)(car(substs)->syme, symes);
		substs = cdr(substs);
	}
	return symes;

}

UTypeResult
utypeResultNew()
{
	UTypeResult res = (UTypeResult) stoAlloc(OB_Other, sizeof(*res));
	res->substs = listNil(UTypeSubst);
	res->news = listNil(Syme);
	res->conditions = listNil(Sefo);
	return res;
}

void
utypeResultAdd(UTypeResult res, Syme syme, Sefo sefo)
{
	res->substs = listCons(UTypeSubst)(utypeSubstNew(syme, sefo), res->substs);
}

UTypeResult
utypeResultOne(Syme syme, Sefo sefo)
{
	UTypeResult res = utypeResultNew();

	utypeResultAdd(res, syme, sefo);

	return res;
}

UTypeResult
utypeResultTwo(Syme syme1, Sefo sefo1, Syme syme2, Sefo sefo2, Syme new)
{
	UTypeResult res = utypeResultNew();

	utypeResultAdd(res, syme1, sefo1);
	utypeResultAdd(res, syme2, sefo2);
	res->news = listSingleton(Syme)(new);

	return res;
}

void
utypeResultFree(UTypeResult result)
{
	if (utypeResultIsFail(result)) {
		return;
	}
	if (result == &empty) {
		return;
	}
	listFreeDeeply(UTypeSubst)(result->substs, utypeSubstFree);
	listFree(Syme)(result->news);
	stoFree(result);
}

void
utypeResultPrintDb(UTypeResult utypeResult)
{
	utypeResultPrint(dbOut, utypeResult);
	fnewline(dbOut);
}

int
utypeResultPrint(FILE *fout, UTypeResult utypeResult)
{
	struct ostream os;
	int cc;

	ostreamInitFrFile(&os, fout);
	cc = utypeResultOStreamWrite(&os, utypeResult);
	ostreamClose(&os);

	return cc;
}

int
utypeResultOStreamWrite(OStream ostream, UTypeResult utypeResult)
{
	int cc = 0;
	if (utypeResultIsFail(utypeResult)) {
		cc += ostreamPrintf(ostream, "[UTR: Fail]");
	}
	else {
		UTypeSubstList substs = utypeResult->substs;
		String sep = "";
		cc += ostreamPrintf(ostream, "[UTR: ");

		while (substs != listNil(UTypeSubst)) {
			cc += ostreamPrintf(ostream, "%s %pSyme %pAbSyn",
					    sep, car(substs)->syme, car(substs)->sefo);
			substs = cdr(substs);
			sep = ",";
		}
		cc += ostreamPrintf(ostream, "]");
	}
	return cc;
}

