/*****************************************************************************
 *
 * tqual.c: Qualified type forms.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "format.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "sefo.h"
#include "lib.h"
#include "tqual.h"
#include "abpretty.h"
#include "comsg.h"
#include "strops.h"

Bool	tqDebug		= false;

#define tqDEBUG		DEBUG_IF(tq)	afprintf

/******************************************************************************
 *
 * :: Local function declarations
 *
 *****************************************************************************/

#define			tqIsQual(tq)		((tq)->isQual)

local TQual		tqNewEmpty		(TForm, Bool);
local void		tqFreeQual		(TQual);
local TForm		tqGetQual		(TQual);

/******************************************************************************
 *
 * :: Local functions
 *
 *****************************************************************************/

local TQual
tqNewEmpty(TForm base, Bool isQual)
{
	TQual		tq;

	tq = (TQual) stoAlloc((unsigned) OB_TQual, sizeof(*tq));

	tq->isQual	= isQual;
	tq->status	= TQUAL_Normal;
	tq->base	= base;
	tq->qual	= listNil(TForm);

	return tq;
}

local void
tqFreeQual(TQual tq)
{
	listFree(TForm)(tqQual(tq));
	tqQual(tq) = listNil(TForm);
}

local TForm
tqGetQual(TQual tq)
{
	TFormList	ql;
	TForm		cat;

	assert(tqIsQualified(tq));

	if (tqQual(tq) == listNil(TForm))
		return tfNone();

	if (cdr(tqQual(tq)) == listNil(TForm))
		return car(tqQual(tq));

	ql = listNReverse(TForm)(tqQual(tq));
	cat = tfJoinFrList(ql);
	tqQual(tq) = listCons(TForm)(cat, listNil(TForm));
	listFree(TForm)(ql);

	return cat;
}

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

TQual
tqNewFrList(TForm base, TFormList qual)
{
	Bool	isQual = (qual != listNil(TForm));
	TQual	tq = tqNewEmpty(base, isQual);
	tqQual(tq) = qual;
	return tq;
}

TQual
tqNewQualified(TForm base, TForm qual)
{
	return tqAddQual(tqNewEmpty(base, true), qual);
}

TQual
tqNewUnqualified(TForm base)
{
	return tqNewEmpty(base, false);
}

TQual
tqSetUnqualified(TQual tq)
{
	if (tqIsQualified(tq)) {
		tqIsQual(tq) = false;
		tqFreeQual(tq);
	}

	return tq;
}

TQual
tqAddQual(TQual tq, TForm qual)
{
	if (tqIsQualified(tq) && !listMemq(TForm)(tqQual(tq), qual))
		tqQual(tq) = listCons(TForm)(qual, tqQual(tq));

	return tq;
}

void
tqFree(TQual tq)
{
	if (tqIsQualified(tq))
		tqFreeQual(tq);
	stoFree(tq);
}

int
tqPrint(FILE *fout, TQual tq)
{
	int		cc = 0;
	TFormList	ql;

	cc += tformPrint(fout, tqBase(tq));
	if (tqIsQualified(tq)) {
		cc += fnewline(fout);
		cc += fprintf(fout, "Qualified:");
		findent += 2;
		cc += fnewline(fout);
		for (ql = tqQual(tq); ql; ql = cdr(ql)) {
			cc += tformPrint(fout, car(ql));
			if (cdr(ql)) cc += fnewline(fout);
		}
		findent -= 2;
	}
	else {
		cc += fnewline(fout);
		cc += fprintf(fout, "Unqualified.");
	}

	return cc;
}

SymeList
tqGetQualImports(TQual tq)
{
	TForm		dom = tqBase(tq), cat = tqGetQual(tq);
	SymeList	xsymes = tfGetDomExports(dom);
	SymeList	isymes = tfGetDomImports(dom);
	SymeList	csymes = tfGetCatExports(cat);
	SymeList	mods, xl, il, cl, symes = listNil(Syme);

	assert(tqIsQualified(tq));
	assert(tqIsNormal(tq));


	/* Comparisons modulo % and %% */
	mods = listConcat(Syme)(tfGetDomSelf(dom), tfGetCatSelf(cat));


	/* Traversing both lists in parallel avoids substituting
	 * into csymes and maintains eqity against isymes.
	 */
	for (xl = xsymes, il = isymes; xl && il; xl = cdr(xl)) {
		Syme	xsyme = car(xl);
		Syme	isyme = car(il);
		Bool	found = false;

		/* Exports may appear in xsymes and not appear in isymes. */
		if (symeOriginal(xsyme) == symeOriginal(isyme)) {
			for (cl = csymes; !found && cl; cl = cdr(cl))
				found = symeEqualMod(mods, car(cl), xsyme);
			if (found) symes = listCons(Syme)(isyme, symes);
			il = cdr(il);
		}
	}
	assert(il == listNil(Syme));

	symes = listNReverse(Syme)(symes);
	return symes;
}


SymeList
tqGetBuiltinImports(Stab stab, TQual tq)
{
	SymeList	tsymes;
	Msg		msg = ALDOR_W_TqNotBuiltin;
	TForm		dom = tqBase(tq);
	TForm		cat = tqGetQual(tq);
	SymeList	symes = tfGetCatExports(cat);
	AbSyn		ab = tfExpr(dom);


	/* Safety check */
	assert(tqIsBuiltin(tq));
	assert(stab);


	/* Convert domain exports into domain imports */
	symes = symeListSubstSelf(stab, dom, symes);


	/* Delete unsatisfied conditional symes */
	symes = symeListCheckCondition(symes);


	/* Turn these symes into builtins */
	for (tsymes = listNil(Syme); symes; symes = cdr(symes))
	{
		FoamBValTag	bval;
		Syme		syme = car(symes);
		Symbol		sym = symeId(syme);


		/* Make sure that we have fast symbol info */
		if (!symInfo(sym) || !symCoInfo(sym))
			symCoInfoInit(sym);


		/* Get the tag for this built-in (from fast info) */
		bval = foamBValIdTag(sym);


		/* Safety check */
		if ((bval == FOAM_BVAL_LIMIT) &&
			(sym != ssymArrNew) &&
			(sym != ssymArrElt) &&
			(sym != ssymArrSet) &&
			(sym != ssymRecNew) &&
			(sym != ssymRecElt) &&
			(sym != ssymRecSet) &&
			(sym != ssymArrDispose) &&
			(sym != ssymRecDispose) &&
			(sym != ssymBIntDispose))
		{
			/* Not recognised: error and skip syme */
			String	id = symString(sym);
			TForm	tf = symeType(syme);
			String	tip = abPretty(tfExpr(tf));

			comsgWarning(ab, msg, id, tip);
			strFree(tip);
			continue;
		}
		else /* Add syme to result list */
			listPush(Syme, syme, tsymes);


		/* Turn it into a builtin */
		symeSetKind(syme, SYME_Builtin);


		/* Shove in the buitin tag */
		symeSetBuiltin(syme, bval);


		/* Deal with special builtins */
		if (bval == FOAM_BVAL_LIMIT) symeSetSpecial(syme);
	}


	/* Reverse the list of builtin symes */
	tsymes = listNReverse(Syme)(tsymes);
	return tsymes;
}


SymeList
tqGetForeignImports(Stab stab, TQual tq)
{
	TForm		dom = tqBase(tq);
	TForm		cat = tqGetQual(tq);
	SymeList	symes = tfGetCatExports(cat);
	SymeList	tsymes;
	ForeignOrigin	forg = forgFrAbSyn(tfToAbSyn(dom));


	/* Safety check */
	assert(tqIsForeign(tq));
	assert(stab);


	/* Convert domain exports into domain imports */
	symes = symeListSubstSelf(stab, dom, symes);


	/* Delete unsatisfied conditional symes */
	symes = symeListCheckCondition(symes);


	/* Turn these symes into foreigns */
	for (tsymes = symes; tsymes; tsymes = cdr(tsymes))
	{
		Syme	syme = car(tsymes);


		/* Turn it into a foreign */
		symeSetKind(syme, SYME_Foreign);


		/* Shove in the foreign origin */
		symeSetForeign(syme, forg);
	}


	return symes;
}


Bool
tqHasImport(TQual tq, Syme syme)
{
	assert(symeIsImport(syme));
	return	tq && tfEqual(tqBase(tq), symeExporter(syme)) &&
		(tqIsUnqualified(tq) ||
		 symeListMember(syme, tqGetQualImports(tq), symeEqual));
}

TQualList
tqListFrArgs(Stab stab, TForm *tfv, Length len)
{
	TQualList	ql = listNil(TQual);
	Length		i;

	if (!stab) stab = stabFile();
	for (i = 0; i < len; i += 1) {
		TForm	tfi  = tfv[i];
		TForm	tft  = tfDefineeType(tfi);
		Syme	syme = tfDefineeSyme(tfi);

		if (syme) {
			TForm	tfe = tfEnum(stab, abFrSyme(syme));
			ql = listCons(TQual)(tqNewUnqualified(tft), ql);
			ql = listCons(TQual)(tqNewUnqualified(tfe), ql);
		}
	}

	ql = listNReverse(TQual)(ql);
	return ql;
}

/*
 * Change the status of a tqual - the previous status is returned
 * for convenience. This ought to be called just once after creation
 * to make a new TQual foreign or builtin.
 */
TQualStatus
tqSetStatus(TQual tq, TQualStatus status)
{
	TQualStatus	old = tqStatus(tq);

	tqStatus(tq) = status;
	return old;
}

