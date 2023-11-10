/****************************************************************************
 *
 * tinfer.c: Type inference.
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

#include "opsys.h"
#include "sefo.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "strops.h"
#include "syme.h"
#include "table.h"
#include "tconst.h"
#include "terror.h"
#include "tfcond.h"
#include "tfsat.h"
#include "ti_bup.h"
#include "ti_sef.h"
#include "ti_tdn.h"
#include "ti_top.h"
#include "tinfer.h"
#include "tposs.h"
#include "tqual.h"


/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

Bool	tipAddDebug		= false;
Bool	tipApplyDebug		= false;
Bool	tipAssignDebug		= false;
Bool	tipDeclareDebug		= false;
Bool	tipDefineDebug		= false;
Bool	tipFarDebug		= false;
Bool	tipIdDebug		= false;
Bool	tipLitDebug		= false;
Bool	tipEmbedDebug		= false;
Bool	tipExtendDebug		= false;

Bool	titfDebug		= false;
Bool	titfOneDebug		= false;
Bool	titfStabDebug		= false;
Bool	abExpandDebug		= false;

extern Bool symeRefreshDebug;

#define titfDEBUG		DEBUG_IF(titf)		afprintf
#define titfOneDEBUG		DEBUG_IF(titfOne)	afprintf
#define titfStabDEBUG		DEBUG_IF(titfStab)	afprintf
#define abExpandDEBUG		DEBUG_IF(abExpand)	afprintf
#define symeRefreshDEBUG	DEBUG_IF(symeRefresh)	afprintf

extern Bool tipExtendDebug;
#define tipExtendDEBUG		DEBUG_IF(tipExtend)	afprintf

void
tiReportStore(String s)
{
	static ULong	obytes = 0;
	ULong		nbytes = stoBytesAlloc;

	if (nbytes > obytes) {
		if (s) fprintf(osStdout, "%s", s);
		fprintf(osStdout, "Store  %8ld B  delta  %8ld\n",
			nbytes, nbytes - obytes);
		obytes = nbytes;
	}
}

/*****************************************************************************
 *
 * :: Local Declarations
 *
 ****************************************************************************/

/*
 * DefaultState
 */

enum def_state {
	DEF_State_No,		/* No special action for defaults. */
	DEF_State_NotYet,	/* Skip defaults when walking an absyn. */
	DEF_State_Yes,		/* Walk defaults when walking an absyn. */
	DEF_State_LIMIT
};

typedef Enum(def_state)	DefaultState;

/* typeInferTForms helper functions. */

local SymbolList	tiTfDeclarees		= 0;
local SymbolList	tiTfDefinees		= 0;
local DefaultState	tiTfDoingDefault	= DEF_State_No;

local SymbolList	tiTfGetDeclarees	(TFormUses);
local SymbolList	tiTfPushDeclarees	(SymbolList);
local void		tiTfPopDeclarees	(SymbolList);

local Symbol		tiTfUsesSymbol		(TFormUses);
local void		tiTfPushDefinee0	(Symbol);

local Bool		tiTfIsBoundary		(TFormUses);

/* typeInferTForms cases. */

local void		tiTfOne			(Stab, TFormUses, TForm);
local void		tiTfSort		(Stab, TFormUsesList);
local void		tiTfCycle		(Stab, TFormUsesList);
local void		tiTfDefault		(Stab, TFormUsesList);
local Bool		tiTfDefaultSyntax	(Stab, TForm);
local void		tiTfDefaultSyntaxMap	(Stab, TForm);
local Bool		tiTfDefaultSyntaxDefine	(Stab, TForm);

extern void		tiTfPrint		(FILE *, Stab, String,
						 TFormUsesList);
extern void		tiTfEnter		(FILE *, String,
						 TFormUses, TForm);
extern void		tiTfExit		(FILE *, String,
						 TFormUses, TForm);

#define tiTfPrintDb(v)	if (DEBUG(v)) tiTfPrint
#define tiTfEnterDb(v)	if (DEBUG(v)) tiTfEnter
#define tiTfExitDb(v)	if (DEBUG(v)) tiTfExit

/* typeInferTForms phases. */

local TFormUsesList	tiTfPartition		(Stab, TFormUsesList);
local TFormUsesList	tiTfSyntax		(Stab, TFormUsesList);
local TFormUsesList	tiTfSelect		(Stab, TFormUsesList);

local Bool		tiTfSyntax1		(Stab, TFormUses, TForm,
						 AbSynList);
local Bool		tiTfFloat1		(Stab, TForm);
local void		tiTfMap1		(Stab, TFormUses, TForm,
						 AbSynList);
local Bool		tiTfDefine1		(Stab, TFormUses, TForm,
						 AbSynList);
local void		tiTfThird1		(Stab, TFormUses, TForm,
						 AbSynList);
local void		tiTfCategory1		(Stab, TFormUses, TForm,
						 AbSynList);
local void		tiTfUnknown1		(Stab, TFormUses, TForm,
						 AbSynList);
local void		tiTfPending1		(Stab, TForm);
local void		tiTfBottomUp1		(Stab, TFormUses, TForm);
local void		tiTfAudit1		(Stab, TForm);
local void		tiTfTopDown1		(Stab, TForm);
local void		tiTfMeaning1		(Stab, TForm);
local void		tiTfExtend1		(Stab, TFormUses);
local void		tiTfImport1		(Stab, TFormUses);
local void		tiTfDefault1		(Stab, Sefo);
local void 		tiTfCascades1           (Stab stab, TFormUses tfu);
/* typeInferTForms topological sorting. */

local Table		tiTfGetDeclareeTable	(TFormUsesList);
local void		tiTfFreeDeclareeTable	(Table);
local void		tiTfCollectHasDependees	(TFormUsesList, TFormUses);
local void		tiTfCollectDependees	(Table, TFormUses, TForm);
local void		tiTfCollectSefoDependees(Table, TFormUses, Sefo);
local void		tiTfCollectSymDependees	(Table, TFormUses, Symbol);
local void		tiTfAddDependee		(TFormUses, TFormUses);
local void		tiTfFreeDependees	(TFormUsesList);
local void		tiTfFreeCDependees	(TFormUsesList);

local TFormUsesList	tiTopForward		(TFormUsesList);
local TFormUsesList	tiTopReverse		(TFormUsesList);
local TFormUsesList	tiTopCForward		(TFormUsesList);
local TFormUsesList	tiTopCReverse		(TFormUsesList);
local TFormUsesList	tiTopSort		(TFormUsesList);
local TFormUsesList	tiTopCycle		(TFormUsesList);

/* tiTopClique helper functions. */

local TFormUsesList	tiTopClique		(TFormUsesList);
local TFormUses		tiTopCliqueUnion	(TFormUsesList);
local TFormUses		tiTopCliqueRep		(TFormUses);
local void		tiTopCliqueMark		(TFormUsesList, Bool);
local void		tiTopCliqueAddVertex	(TFormUses);
local void		tiTopCliqueAddCVertex	(TFormUses);
local void		tiTopCliqueDelCVertex	(TFormUses);
local void		tiTopCliqueAddEdge	(TFormUses, TFormUses);
local void		tiTopCliqueDelEdge	(TFormUses, TFormUses);
local Bool		tiTopEqual		(TFormUses, TFormUses);

/*****************************************************************************
 *
 * :: Main entry points for type inference
 *
 ****************************************************************************/

local Bool	tqShouldImport	  (TQual);
local TForm     tiGetTopLevelTForm(AbLogic context, AbSyn type);
local Bool 	tiCheckSymeConditionalImplementation(Stab stab, Syme syme, Syme implSyme);

void
tinferInit()
{
	TiTopLevel topLevel = (TiTopLevel) stoAlloc(OB_Other, sizeof(*topLevel));

	topLevel->terrorTypeConstFailed = terrorTypeConstFailed;
	topLevel->tiBottomUp = tiBottomUp;
	topLevel->tiTopDown = tiTopDown;
	topLevel->tiCanSefo = tiCanSefo;
	topLevel->tiGetTopLevelTForm = tiGetTopLevelTForm;

	topLevel->tiUnaryToRaw = tiUnaryToRaw;
	topLevel->tiRawToUnary = tiRawToUnary;
	topLevel->tiSefo = tiSefo;	
	topLevel->tiTfSefo = tiTfSefo;
	topLevel->typeInferTForms = typeInferTForms;

	topLevel->tqShouldImport = tqShouldImport;
	topLevel->typeInferTForm = typeInferTForm;
	
	tiTopLevelInit(topLevel);

}

/*
 * Type inference consists of two passes:


 * 1) a bottom up pass which constructs a set of possible types for each node
 * 2) a top down pass which restricts each node to a unique type.
 */

TForm
typeInferAs(Stab stab, AbSyn absyn, TForm type)
{
	if (abState(absyn) != AB_State_HasUnique) {
		tiBottomUp    (stab, absyn, type);
		typeInferAudit(stab, absyn);
		tiTopDown     (stab, absyn, type);
	}
	return abTUnique(absyn);
}

TForm
typeInfer(Stab stab, AbSyn absyn)
{
	TForm	tf;

	tcInit();
	typeInferTForms(stab);
	tf = typeInferAs(stab, absyn, tfUnknown);
	tcFini();

	return tf;
}

Bool
typeInferAudit(Stab stab, AbSyn absyn)
{
	return terrorAuditPoss(true, absyn);
}

void
typeInferCheck(Stab stab, AbSyn absyn, TForm type)
{
	Syme	syme = abSyme(absyn);
	TForm	tf;

	if (abIsNothing(absyn))
		return;

	if (type == tfDomain && syme &&
	    (symeIsLibrary(syme) || symeIsArchive(syme)))
		return;

	if (abState(absyn) != AB_State_HasUnique)
		return;

	tf = abTUnique(absyn);
	if (!tfIsUnknown(tf) && !tfSatValues(tf, type)) {
		abState(absyn) = AB_State_Error;
		abTPoss(absyn) = tpossSingleton(tf);
		tiTopDown(stab, absyn, type);
	}
}

void
tiTfSefo(Stab stab, TForm tf)
{
	Length	i;

	if (!tfNeedsSefo(tf))
		return;
	tfClrNeedsSefo(tf);

	if (tfIsAbSyn(tf) && tiCanSefo(tfGetExpr(tf)))
		tiSefo(stab, tfGetExpr(tf));

	if (tfIsNode(tf))
		for (i = 0; i < tfArgc(tf); i += 1)
			tiTfSefo(stab, tfArgv(tf)[i]);

	if (tfTagHasSymes(tfTag(tf)) && tfSymes(tf) == listNil(Syme)) {
		tfGetSymes(stab, tf, tfExpr(tf));
	}
}

/*
 * Make sure that the category exports of context are visible in stab.
 */
void
tiWithSymes(Stab stab, TForm context)
{
	SymeList	mods;
	SymeList	symes;
	SymeList	csymes = listNil(Syme);
	SymeList	esymes = listNil(Syme);
	Stab		wstab = stab;
	while (wstab && stabGetSelf(wstab) == NULL) wstab = cdr(wstab);
	tipAddDEBUG(dbOut, ">>tiWithSymes:\n");

	mods = listNConcat(Syme)(tfGetSelfFrStab(wstab),
				 listCopy(Syme)(tfGetCatSelf(context)));
	
	symes = tfGetCatExports(context);

	for (; symes; symes = cdr(symes)) {
		Syme		syme = car(symes), xsyme = NULL;
		Symbol		sym  = symeId(syme);
		TForm		tf   = symeType(syme);

		if (DEBUG(tipAdd)) {
			afprintf(dbOut, "  looking for: %pSyme\n", syme);
		}

		/* Look for syme in the capsule. */
		if ((xsyme = stabGetExportMod(wstab, mods, sym, tf))) {
			if (tiCheckSymeConditionalImplementation(wstab, syme, xsyme))
				tipAddDEBUG(dbOut, "  [export]\n");
			else {
				tipAddDEBUG(dbOut, "  [conditional override]\n");
				esymes = listCons(Syme)(syme, esymes);
			}
		}

		else {
			tipAddDEBUG(dbOut, "  [category]\n");
			csymes = listCons(Syme)(syme, csymes);
		}
	}

	for (; esymes != listNil(Syme); esymes = cdr(esymes)) {
		Syme 	esyme = car(esymes);
		Symbol	sym  = symeId(esyme);
		TForm	tf   = symeType(esyme);
		Syme xsyme = stabGetExportMod(wstab, mods, sym, tf);
		if (symeCondition(esyme) == listNil(Sefo)) {
			symeSetDefinitionConditions(xsyme, listNil(AbSyn));
		}
		else {
			symeSetDefinitionConditions(xsyme,
						    listCons(AbSyn)(abNewAndAll(sposNone,
										(AbSynList) symeCondition(esyme)), 
								    symeDefinitionConditions(xsyme)));
		}
	}


	symes = symeListSubstCat(wstab, mods, context, csymes);

	symes = symeListCheckWithCondition(symes);
	symes = symeListMakeLazyConditions(symes);

	stabPutMeanings(stab, symes);

	/*!! listFree(Syme)(mods); */
	listFree(Syme)(symes);
	listFree(Syme)(csymes);

	tipAddDEBUG(dbOut, "<<tiWithSymes:\n");
}

local SymeList
symeListSetImplicit(Stab stab, SymeList symes)
{
	SymeList	result = listNil(Syme);

	for (;symes;symes = cdr(symes))
	{
		Syme	syme	= car(symes);
		Symbol	id	= symeId(syme);
		TForm	tf	= symeType(syme);


		/* Look for this syme in the stab */
		syme = stabDefExport(stab, id, tf, (Doc)0);


		/* Syme must not have a const lib: it's local */
		if (symeConstLib(syme)) symeSetConstLib(syme, (Lib)0);


		/* Mark as implicit */
		symeSetImplicit(syme);


		/* Add to the list of results */
		result = listCons(Syme)(syme, result);
	}

	return result;
}

local Bool
tiCheckSymeConditionalImplementation(Stab stab, Syme syme, Syme implSyme)
{
	SefoList condition = symeCondition(syme);
	AbSynList implCondition = symeDefinitionConditions(implSyme);
	AbSynList tmp;
	SefoList tmpSefo;
	AbLogic implAbLog, conditionAbLog;
	Bool result;

	if (implCondition == listNil(AbSyn))
		return true;
	/* Need to check that implCondition implies condition */
	/* First, unconditional implies it does */
	for (tmp = implCondition; tmp != listNil(AbSyn); tmp = cdr(tmp)) {
		if (car(tmp) == NULL)
			return true;
	}

	for (tmp = implCondition; tmp != listNil(AbSyn); tmp = cdr(tmp)) {
		tiBottomUp(stab, car(tmp), tfUnknown);
		tiTopDown(stab, car(tmp), tfUnknown);
	}

	implAbLog = ablogFalse();
	for (tmp = implCondition; tmp != listNil(AbSyn); tmp = cdr(tmp)) {
		implAbLog = ablogOr(ablogFrSefo(car(tmp)), implAbLog);
	}
	conditionAbLog = ablogTrue();
	for (tmpSefo = condition; tmpSefo != listNil(Sefo); tmpSefo = cdr(tmpSefo)) {
		conditionAbLog = ablogAnd(ablogFrSefo(car(tmpSefo)), conditionAbLog);
	}

	result = ablogImplies(conditionAbLog,
			      ablogAnd(abCondKnown != NULL
				       ? abCondKnown
				       : ablogTrue(),
				       implAbLog));

	return result;
}

/*
 * Make sure that the category exports of context are visible in capsule.
 * Return the symes which could not be found in the add. Tell the caller
 * about symes we've added to the stab.
 */
SymeList
tiAddSymes(Stab astab, AbSyn capsule, TForm base, TForm context, SymeList *p)
{
	Bool		hasImplicit;
	SymeList	mods;
	SymeList	symes;
	SymeList	isymes = listNil(Syme);
	SymeList	asymes = listNil(Syme);
	SymeList	csymes = listNil(Syme);
	SymeList	dsymes = listNil(Syme);
	SymeList	usymes = listNil(Syme);
	SymeList	aself  = tfGetSelfFrStab(astab);
	Syme		asyme  = (aself ? car(aself) : NULL);

	tipAddDEBUG(dbOut, "(tiAddSymes:\n");


	/*
	 * If the caller wants to know which symes we have added to
	 * the stab then we MUST initialise our workspace now.
	 */
	if (p) *p = listNil(Syme);


	/* We can't do anything with unknown contexts */
	tfFollow(context);
	if (tfIsUnknown(context))
		return dsymes;

	/* Does the context satisfy DenseStorageCategory? */
	hasImplicit = tfCatHasImplicit(context);


	mods = listNConcat(Syme)(listCopy(Syme)(tfGetDomSelf(base)),
				 listCopy(Syme)(tfGetCatSelf(context)));
	mods = listNConcat(Syme)(aself, mods);
	
	symes = tfGetCatExports(context);	
	for ( ; symes; symes = cdr(symes)) {
		Syme		syme = car(symes), xsyme = NULL;
		Symbol		sym  = symeId(syme);
		TForm		tf   = symeType(syme);

		tipAddDEBUG(dbOut, "  looking for: %pSyme %pAbSynList ", syme, symeCondition(syme));
		
		/* Look for syme in the capsule. */
		if ((xsyme = stabGetDomainExportMod(astab, mods, sym, tf)) != NULL
		    && tiCheckSymeConditionalImplementation(astab, syme, xsyme)) {
			tipAddDEBUG(dbOut, "  [export]\n");
			symeImplAddInherit(xsyme, base, syme);
		}

		/* Look for syme in the base. */
		else if ((xsyme = tfHasDomExportMod(base, mods, sym, tf))
			 != NULL) {
			tipAddDEBUG(dbOut, "  [add chain]\n");
			/*!! if (!symeIsSelfSelf(xsyme)) */
				asymes = listCons(Syme)(xsyme, asymes);
		}

		/* Look for syme in the defaults. */
		else if (symeHasDefault(syme)) {
			tipAddDEBUG(dbOut, "  [default]\n");
			/*!! if (!symeIsSelfSelf(syme)) */
				csymes = listCons(Syme)(syme, csymes);
		}

		/* Look for the syme in the implicits */
		else if (hasImplicit &&
			((xsyme = tfImplicitExport(astab,mods,syme))!=NULL)) {
			tipAddDEBUG(dbOut, "  [implicit]\n");
			isymes = listCons(Syme)(xsyme, isymes);
		}

		/* Look for syme in the conditional symes. */
		else if (symeCondition(syme)) {
			tipAddDEBUG(dbOut, "  [conditional]\n");
			/*!! if (!symeIsSelfSelf(syme)) */
				usymes = listCons(Syme)(syme, usymes);
		}

		/* The add doesn't satisfy its context. */
		else {
			tipAddDEBUG(dbOut, "  [missing]\n");
			dsymes = listCons(Syme)(syme, dsymes);
		}
	}

	if (asymes != listNil(Syme)) {
		symes = symeListSubstCat(astab, mods, tfUnknown, asymes);
		stabPutMeanings(astab, symes);
		if (p) *p = listNConcat(Syme)(*p, listCopy(Syme)(symes));
		listFree(Syme)(symes);
	}

	if (isymes != listNil(Syme)) { /* IMPLICIT */
		SymeList symes;
		symes = symeListSubstCat(astab, mods, tfUnknown, isymes);
		symes = symeListSetImplicit(astab, symes);
		/* ******************** CHECK THIS ******************** */
		/* No need to stabPutMeanings as they ought to be there */
		/* ******************** CHECK THIS ******************** */
		if (p) *p = listNConcat(Syme)(*p, listCopy(Syme)(symes));
		listFree(Syme)(symes);
	}

	if (csymes != listNil(Syme)) {
		SymeList symes;
		csymes = symeListSubstCat(astab, mods, context, csymes);
		symes = symeListCheckAddConditions(csymes);
		stabPutMeanings(astab, symes);
		if (p) *p = listNConcat(Syme)(*p, listCopy(Syme)(symes));
		listFree(Syme)(symes);
	}

	if (usymes != listNil(Syme)) {
		SymeList symes;
		symes = symeListSubstCat(astab, mods, context, usymes);
		symes = symeListCheckAddConditions(symes);
		dsymes = listNConcat(Syme)(symes, dsymes);
	}
	if (asyme && (asymes || csymes || dsymes || isymes)
		  && !symeIsExtend(asyme))
	{
		Syme	xself = stabDefExtend(astab, ssymSelf, context);
		symeAddExtendee(xself, asyme);
		symeSetExtension(asyme, xself);
		stabExtendMeanings(astab, xself);
	}

	/*!! listFree(Syme)(mods); */
	listFree(Syme)(asymes);
	/*!! listFree(Syme)(isymes); */

	tipAddDEBUG(dbOut, "tiAddSymes %pSymeList)\n", dsymes);

	dsymes = listNReverse(Syme)(dsymes);
	return dsymes;
}

local TForm 
tiGetTopLevelTForm(AbLogic context, AbSyn type)
{
	TForm tf;

	tf = tiGetTFormContext(stabFile(), context, type);

	return tf;
}

/*
 *  Get a cached or create and cache a new type form for the expression.
 *  It is most likely placed there by scobind.
 */
TForm
tiGetTForm(Stab stab, AbSyn type)
{
	return tiGetTFormContext(stab, ablogTrue(), type);
}

TForm
tiGetTFormContext(Stab stab, AbLogic context, AbSyn type)
{
	TForm	tf, ntf;

	if (abTForm(type)) {
		/* This is a little hacky... we just want to ensure
		 * that if 'type' has an attached tform, then that
		 * tform in turn should have a decent looking sefo
		 * attached */
		abSetTForm(type, tfFollowFn(abTForm(type)));
		if (!tfHasExpr(abTForm(type)))
			tfToAbSyn(abTForm(type));
	}
	tf  = abTForm(type) ? (abTForm(type)) : tfSyntaxFrAbSyn(stab, type);

	/* Transfer semantics from type to tf. */
	if (abIsSefo(type) && tfHasExpr(tf) && !abIsSefo(tfGetExpr(tf))) {
		abTransferSemantics(type, tfGetExpr(tf));
	}

	if (!tfIsMeaning(tf)) {
		tfMergeConditions(tf, stab, tfCondEltNewKnown(stab, context));
	}
	
	ntf = typeInferTForm(stab, tf);
	tfTransferSemantics(ntf, tf);

	/* Transfer semantics from tf to type. */
	if (type != tfGetExpr(tf))
		if (!abIsSefo(type) &&
		    abIsSefo(tfGetExpr(ntf)) && !tfIsUnknown(tfTUnique(ntf)))
			abTransferSemantics(tfGetExpr(ntf), type);

	if (!abIsSefo(type)) {
		abState(type) = AB_State_HasUnique;
		abTUnique(type) = tfUnknown;
	}

	tfFollow(tf);
	return abSetTForm(type, tf);
}

Bool
tiMergeSyme(Syme syme, SymeList symes)
{
	SymeList sl = symes;

	while (sl != listNil(Syme)) {
		if (symeEqualModConditions(listNil(Syme), syme, car(sl)))
			return false;
		sl = cdr(sl);
	}
	return true;
}

Syme
tiGetMeaning(Stab stab, AbSyn absyn, TForm type)
{
	SatMask		mask = tfSatBupMask();
	Length		nsymec, psymec;
	Syme		nsyme, psyme, syme;
	SymeList	symes, nsymes, sl;

	symes = stabGetMeanings(stab, abCondKnown, abIdSym(absyn));
	nsymes = listNil(Syme);		/* Possible (non-pending) meanings */
	nsymec = 0;			/* Number of non-pending matches */
	psymec = 0;			/* Number of all possible matches */
	nsyme = NULL;
	psyme = NULL;

	for (sl = symes; sl; sl = cdr(sl)) {
		Syme	syme = car(sl);
		TForm	mtype = symeType(syme);
		SatMask	result;

		result = tfSat1(mask, absyn, mtype, type);
		if (tfSatSucceed(result)) {
			if (!tfSatPending(result)
			    && symeUseIdentifier(absyn, syme)
			    && tiMergeSyme(syme, nsymes)) {
				nsymec += 1;
				nsyme = syme;
				nsymes = listCons(Syme)(syme, nsymes);
			}
			psymec += 1;
			psyme = syme;
		}

	}

	syme = NULL;
	if (psymec == 1 && !tfIsUnknown(symeType(psyme)))
		syme = psyme;

	else if (nsymec == 1 && !tfIsUnknown(symeType(nsyme)))
		syme = nsyme;

	else if (nsymec == 0 && psymec > 0)
		terrorApplyNotAnalyzed(absyn, absyn, type);

	else
		terrorNotUniqueMeaning(ALDOR_E_TinNMeanings, absyn, nsymes,
				       symes, symString(abIdSym(absyn)), type);

	listFree(Syme)(nsymes);

	return syme;
}

Syme
tiGetExtendee(Stab stab, AbSyn absyn, TForm type)
{
	Symbol		sym = absyn->abId.sym;
	int		n;
	SymeList	ml0, ml, okSymes = listNil(Syme);
	Syme		syme = NULL;

	ml0 = stabGetMeanings(stab, abCondKnown, sym);

	for (n = 0, ml = ml0; ml; ml = cdr(ml)) {
		Syme	msyme = car(ml);
		TForm	mtype = symeType(msyme);
		if (tfCanExtend(mtype, type)) {
			if (n > 0) okSymes = listCons(Syme)(syme, okSymes);
			syme = msyme;
			n += 1;
		}
	}

	if (n != 1 || tfIsUnknown(symeType(syme))) {
		if (n > 0) okSymes = listCons(Syme)(syme, okSymes);
		terrorNotUniqueMeaning(ALDOR_E_TinNMeanings, absyn, okSymes, ml0,
				       symString(sym), type);

		syme = NULL;
	}

	listFree(Syme)(okSymes);

	return syme;
}

local Bool tiCheckLambda0(TForm tf);

Bool
tiCheckLambdaType(TForm tf)
{
	if (tfIsDomainMap(tf) || tfIsCategoryMap(tf))
		return false;

	return tiCheckLambda0(tf);
}

local Bool
tiCheckLambda0(TForm tf)
{
	if (tfIsAnyMap(tf))
		tiCheckLambda0(tfMapRet(tf));

	return !(tfSatDom(tf) || tfSatCat(tf));
}


/*****************************************************************************
 *
 * :: Packed map embeddings
 *
 ****************************************************************************/

local AbSyn
abNewRawOf(AbSyn arg)
{
	AbSyn	op = abNewId(abPos(arg), symIntern("raw"));
	AbSyn	ab = abNewApply1(abPos(arg), op, arg);

	return ab;
}

local AbSyn
abNewBoxOf(AbSyn arg)
{
	AbSyn	op = abNewId(abPos(arg), symIntern("box"));
	AbSyn	ab = abNewApply1(abPos(arg), op, arg);

	return ab;
}

Bool
tiUnaryToRaw(Stab stab, AbSyn ab, TForm tf)
{
	AbSyn	imp = abImplicit(ab);

	if (!imp) {
		imp = sefoCopy(ab);
		if (abState(imp) != AB_State_HasUnique) {
			abState(imp) = AB_State_HasUnique;
			abTUnique(imp) = tf;
		}
		imp = abNewRawOf(imp);
		abSetImplicit(ab, imp);
	}

	tiBottomUp(stab, imp, tfUnknown);
	tiTopDown (stab, imp, tfUnknown);
	if (abState(imp) == AB_State_HasUnique) {
		abAddTContext(ab, AB_Embed_UnaryToRaw);
		return true;
	}
	else
		return false;
}

Bool
tiRawToUnary(Stab stab, AbSyn ab, TForm tf)
{
	AbSyn	imp = abImplicit(ab);

	if (!imp) {
		imp = sefoCopy(ab);
		if (abState(imp) != AB_State_HasUnique) {
			abState(imp) = AB_State_HasUnique;
			abTUnique(imp) = tf;
		}
		imp = abNewBoxOf(abNewRawOf(imp));
		abSetImplicit(ab, imp);
	}

	tiBottomUp(stab, imp, tf);
	tiTopDown (stab, imp, tf);
	if (abState(imp) == AB_State_HasUnique) {
		abAddTContext(ab, AB_Embed_RawToUnary);
		return true;
	}
	else
		return false;
}

/*****************************************************************************
 *
 * :: tiDefine
 *
 ****************************************************************************/

TForm
tiDefineFilter(AbSyn absyn, TForm type)
{
	AbSyn		lhs = absyn->abDefine.lhs;
	if (abHasTag(lhs, AB_Declare))
		return abTForm(lhs->abDeclare.type);
	else
		return type;
}

TPoss
tiDefineTPoss(AbSyn absyn)
{
	AbSyn		lhs = absyn->abDefine.lhs;
	if (abHasTag(lhs, AB_Declare))
		return abReferTPoss(lhs);
	else
		return abReferTPoss(absyn);
}

/*****************************************************************************
 *
 * :: typeInferTForms
 *
 ****************************************************************************/

TForm
typeInferTForm(Stab stab, TForm tf)
{
	DefaultState	wasDoingDefaults = tiTfDoingDefault;
	tfFollow(tf);

	tiTfDoingDefault = DEF_State_Yes;
	tiTfOne(stab, NULL, tf);
	tiTfDoingDefault = wasDoingDefaults;

	return tf;
}

void
typeInferTForms(Stab stab)
{
	TFormUsesList	tful0, tful1, tful2, tful;
	static ULong	serialNo = 0, depthNo = 0, serialMax = 0;
	ULong		serialThis;
	
	/* Maybe should check cdr(stab), and use extreme caution */


	if (car(stab)->isChecked) return;
	car(stab)->isChecked = true;

	tful0 = listReverse(TFormUses)(car(stab)->tformsUsed.list);

	serialNo += 1;
	depthNo  += 1;
	serialThis = serialNo;
	if (serialMax == 0) serialMax = stabMaxSerialNo();
	tiTfPrintDb(titf)(dbOut, stab, ">>typeInferTForms:", tful0);
	titfStabDEBUG(dbOut, "->Titf: %*s%ld/%ld\n", (int)depthNo, "",
		      serialThis, serialMax);

	for (tful = tful0; tful; tful = tful1) {
		DefaultState	wasDoingDefaults = tiTfDoingDefault;

		if (false) {
			tiTfDoingDefault = DEF_State_NotYet;
			tiTfOne(stab, car(tful), car(tful)->tf);
			tiTfDoingDefault = DEF_State_Yes;
			tiTfDefault(stab, tful);
		}
		else if (cdr(tful) == NULL || tiTfIsBoundary(car(tful))) {
			tiTfDoingDefault = DEF_State_Yes;
			tful1 = cdr(tful);
			tiTfOne(stab, car(tful), car(tful)->tf);
		}
		else {
			tful1 = tiTfPartition(stab, tful);
			tful2 = listCopyTo(TFormUses)(tful, tful1);

			tiTfDoingDefault = DEF_State_NotYet;
			tiTfSort(stab, tful2);
			tiTfDoingDefault = DEF_State_Yes;
			tiTfDefault(stab, tful2);
			listFree(TFormUses)(tful2);
		}
		tiTfDoingDefault = wasDoingDefaults;
	}

	titfStabDEBUG(dbOut, "<-Titf: %*s%ld/%ld\n", (int)depthNo, "",
		      serialThis, serialMax);
	tiTfPrintDb(titf)(dbOut, stab, "<<typeInferTForms:", tful0);
	depthNo -=1;

	listFree(TFormUses)(tful0);
}

local void
tiTfSort(Stab stab, TFormUsesList tful0)
{
	Table		tbl;
	TFormUsesList	tful, before, cycle, after;
	Bool		allBefore = false;
	titfStabDEBUG(dbOut, "[sorting %d tfus]\n",
		      (int) listLength(TFormUses)(tful0));

	/* Collect the type forms used to declare each symbol. */
	tbl = tiTfGetDeclareeTable(tful0);
				      
	/* Collect the type forms used to declare symbols in each type form. */
	for (tful = tful0; tful; tful = cdr(tful))
		tiTfCollectDependees(tbl, car(tful), car(tful)->tf);

	/* Collect the dependencies which arise from has questions. */
	for (tful = tful0; tful; tful = cdr(tful))
		tiTfCollectHasDependees(tful0, car(tful));

	/* Free the table of type forms used to declare each symbol. */
	tiTfFreeDeclareeTable(tbl);

	/* Perform a topological sort on the type forms. */
	before = tiTopForward	(tful0);
	after  = tiTopReverse	(tful0);
	cycle  = tiTopCycle	(tful0);

	if (DEBUG(titf)) {
		tiTfPrint(dbOut, stab, ">>tiTfSort[before]:", before);
		tiTfPrint(dbOut, stab, ">>tiTfSort[after ]:", after);
		tiTfPrint(dbOut, stab, ">>tiTfSort[cycle ]:", cycle);
	}
	if (DEBUG(titfStab)) {
		allBefore = (!cycle && !after);
		if (allBefore) fprintf(dbOut, "[all before]\n");
	}

	/* Process the type forms in before. */
	if (DEBUG(titfStab)) {
		if (!allBefore)
			fprintf(dbOut, "[%d before]",
				(int) listLength(TFormUses)(before));
	}
	for (tful = before; tful; tful = cdr(tful)) {
		if (DEBUG(titfStab)) {
			if (!allBefore)
				fprintf(dbOut, ".");
		}
		tiTfOne(stab, car(tful), car(tful)->tf);
	}
	if (DEBUG(titfStab)) {if (!allBefore) fnewline(dbOut);}

	/* Process the type forms in cycle. */
	if (cycle) {
		TFormUsesList	clique = tiTopClique(cycle);
		titfStabDEBUG(dbOut, "[%d cliques]\n",
			      (int) listLength(TFormUses)(clique));
		for (tful = clique; tful; tful = cdr(tful)) {
			TFormUsesList	clq = car(tful)->cdependents;
			titfStabDEBUG(dbOut, "[clique]\n");
			tiTfCycle(stab, clq);
		}
		tiTfFreeCDependees(clique);
		listFree(TFormUses)(clique);
	}

	/* Process the type forms in after. */
	if (DEBUG(titfStab)) {
		if (!allBefore)
			fprintf(dbOut, "[%d after]",
				(int) listLength(TFormUses)(after));
	}
	for (tful = after; tful; tful = cdr(tful)) {
		titfStabDEBUG(dbOut, ".");
		tiTfOne(stab, car(tful), car(tful)->tf);
	}
	if (DEBUG(titfStab)) {if (!allBefore) fnewline(dbOut);}

	/* Free the lists of dependencies between type forms. */
	tiTfFreeDependees(tful0);

	listFree(TFormUses)(before);
	listFree(TFormUses)(after);
	listFree(TFormUses)(cycle);
}

local void
tiTfOne(Stab stab, TFormUses tfu, TForm tf)
{
	tiTfEnterDb(titf)(dbOut, "tiTfOne", tfu, tf);

	/* tfu == NULL just means that typeInferTForm doesn't have a tfu. */

	if (tiTfSyntax1(stab, tfu, tf, listNil(AbSyn))) {
		if (tfu == NULL) return;
	}

	if (tfIsSyntax(tf) || tfIsPending(tf) || tfu == NULL) {
		tiTfPending1	(stab, tf);
		tiTfBottomUp1	(stab, tfu, tf);
		tiTfAudit1	(stab, tf);
		tiTfTopDown1	(stab, tf);
		tiTfMeaning1	(stab, tf);
	}

	if (tfu) {
		tiTfExtend1(stab, tfu);
		tiTfImport1(stab, tfu);
		tiTfCascades1(stab, tfu);
	}

	tiTfExitDb(titf)(dbOut, "tiTfOne", tfu, tf);
}

local void
tiTfCycle(Stab stab, TFormUsesList tful)
{
	TFormUsesList	tful0, tfl;

	tiTfPrintDb(titf)(dbOut, stab, ">>tiTfCycle:", tful);

	titfStabDEBUG(dbOut, "[syntax]\n");
	tful  = tiTfSyntax	(stab, tful);

	titfStabDEBUG(dbOut, "[select]\n");
	tful0 = tiTfSelect	(stab, tful);

	titfStabDEBUG(dbOut, "[pending]\n");
	for (tfl=tful0; tfl; tfl=cdr(tfl))
		tiTfPending1(stab, car(tfl)->tf);

	titfStabDEBUG(dbOut, "[bottom up]\n");
	for (tfl=tful0; tfl; tfl=cdr(tfl))
		tiTfBottomUp1(stab, car(tfl), car(tfl)->tf);

	titfStabDEBUG(dbOut, "[audit]\n");
	for (tfl=tful0; tfl; tfl=cdr(tfl))
		tiTfAudit1(stab, car(tfl)->tf);

	titfStabDEBUG(dbOut, "[top down]\n");
	for (tfl=tful0; tfl; tfl=cdr(tfl))
		tiTfTopDown1(stab, car(tfl)->tf);

	titfStabDEBUG(dbOut, "[meaning]\n");
	for (tfl=tful0; tfl; tfl=cdr(tfl))
		tiTfMeaning1(stab, car(tfl)->tf);

	titfStabDEBUG(dbOut, "[extend]\n");
	for (tfl=tful;  tfl; tfl=cdr(tfl))
		tiTfExtend1(stab, car(tfl));

	titfStabDEBUG(dbOut, "[import]\n");
	for (tfl=tful;  tfl; tfl=cdr(tfl))
		tiTfImport1(stab, car(tfl));

	for (tfl=tful;  tfl; tfl=cdr(tfl))
		tiTfCascades1(stab, car(tfl));

	tiTfPrintDb(titf)(dbOut, stab, "<<titfCycle:", tful);

	listFree(TFormUses)(tful0);
}

local void
tiTfDefault(Stab stab, TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TForm		tf = car(tful)->tf;

		if (!tiTfDefaultSyntax(stab, tf))
			tiTfDefault1(stab, tfGetExpr(tf));
	}

	for (tful = tful0; tful; tful = cdr(tful))
		tiTfImport1(stab, car(tful));
	for (tful = tful0; tful; tful = cdr(tful))
		tiTfCascades1(stab, car(tful));
}

local Bool
tiTfDefaultSyntax(Stab stab, TForm tf)
{
	Bool	result = true;

	if (tfIsAnyMap(tf)) 
		tiTfDefaultSyntaxMap(stab, tf);

	else if (tfIsDefine(tf))
		result = tiTfDefaultSyntaxDefine(stab, tf);

	else
		result = false;

	return result;
}

local void
tiTfDefaultSyntaxMap(Stab stab, TForm tf)
{
	AbSyn	ab = tfGetExpr(tf);
	Stab	nstab = (abStab(ab) ? abStab(ab) : stab);
	TForm	tfarg = tfMapArg(tf);
	TForm	tfret = tfMapRet(tf);

	tiTfDefault1(nstab, tfGetExpr(tfarg));
	if (!tiTfDefaultSyntax(nstab, tfret))
		tiTfDefault1(nstab, tfGetExpr(tfret));
}

local Bool
tiTfDefaultSyntaxDefine(Stab stab, TForm tf)
{
	TForm	tfd = tfDefineDecl(tf);
	AbSyn	abd = tfGetExpr(tfd);
	Bool	result = true;

	if (abd && abHasTag(abd, AB_Declare))
		abd = abd->abDeclare.type;
	else
		return false;

	if (!abd)
		result = false;

	else if (abIsTheId(abd, ssymCategory)) {
		tiTfDefault1(stab, abd);
		tiTfDefault1(stab, tfGetExpr(tfDefineVal(tf)));
	}

	else if (abHasTag(abd, AB_With)) {
		tiTfDefault1(stab, abd);
		/* default inside an add means something else */
	}

	else
		result = false;

	return result;
}

void
tiTfPrint(FILE *fout, Stab stab, String str, TFormUsesList tful0)
{
	TFormUsesList	tful;
	Length		i;

	fprintf(dbOut, "%s\n", str);
	if (stab) {
		fprintf(dbOut, "Symbol Table Level %ld.",
			car(stab)->lexicalLevel);
		findent += 2;
		fnewline(dbOut);
		fprintf(dbOut, "Declaree stack is: ");
		listPrint(Symbol)(dbOut, tiTfDeclarees, symPrint);
		fnewline(dbOut);
		fprintf(dbOut, "Definee stack is: ");
		listPrint(Symbol)(dbOut, tiTfDefinees, symPrint);
		fnewline(dbOut);
		fprintf(dbOut, "Pending type forms:");
		if (tful0) fnewline(dbOut);
	}

	for (i = 0, tful = tful0; tful; i += 1, tful = cdr(tful)) {
		fnewline(dbOut);
		fprintf(dbOut, "%d. ", (int) i);
		tfuPrint(dbOut, car(tful));
	}

	findent -= 2;
	fnewline(dbOut);
}

void
tiTfEnter(FILE *fout, String str, TFormUses tfu, TForm tf)
{
	fprintf(fout, ">>%s:\n", str);
	findent += 2;
	fnewline(fout);
	if (tfu) tfuPrint(fout, tfu); else tfPrint(fout, tf);
	findent -= 2;
	fnewline(fout);
}

void
tiTfExit(FILE *fout, String str, TFormUses tfu, TForm tf)
{
	fprintf(fout, "<<%s:\n", str);
	findent += 2;
	fnewline(fout);
	if (tfu) tfuPrint(fout, tfu); else tfPrint(fout, tf);
	findent -= 2;
	fnewline(fout);
}

/*****************************************************************************
 *
 * :: typeInferTForms helper functions
 *
 ****************************************************************************/

local SymbolList
tiTfGetDeclarees(TFormUses tfu)
{
	TForm	tf;

	if (tfu == 0) return listNil(Symbol);

	tf = tfu->tf;
	tfFollow(tf);

	if (tfIsWith(tf) || (tfIsAnyMap(tf) && tfIsWith(tfMapRet(tf))))
		return tfu->declarees;
	else
		return listNil(Symbol);
}

local SymbolList
tiTfPushDeclarees(SymbolList sl)
{
	SymbolList	ol = tiTfDeclarees;
	tiTfDeclarees = listNConcat(Symbol)(listCopy(Symbol)(sl),
					    tiTfDeclarees);
	return ol;
}

local void
tiTfPopDeclarees(SymbolList ol)
{
	tiTfDeclarees = listFreeTo(Symbol)(tiTfDeclarees, ol);
}

local Symbol
tiTfUsesSymbol(TFormUses tfu)
{
	Symbol	sym = NULL;

	if (tfu && tfu->declarees && cdr(tfu->declarees) == listNil(Symbol))
		sym = car(tfu->declarees);
	return sym;
}

local void
tiTfPushDefinee0(Symbol sym)
{
	tiTfDefinees = listCons(Symbol)(sym, tiTfDefinees);
}

void
tiTfPushDefinee(AbSyn lhs)
{
	Symbol	sym;

	if (abTag(lhs) == AB_Comma) {
		int i;
		for (i=0; i<abArgc(lhs); i++) {
			tiTfPushDefinee(lhs->abComma.argv[i]);
		}
	}
	else {
		lhs = abDefineeIdOrElse(lhs, NULL);
		sym = lhs ? lhs->abId.sym : NULL;
		tiTfPushDefinee0(sym);
	}
}

void
tiTfPopDefinee(AbSyn lhs)
{
	if (abTag(lhs) == AB_Comma) {
		int i;
		for (i=0; i<abArgc(lhs); i++) {
			tiTfPopDefinee(lhs->abComma.argv[i]);
		}
	}
	else {
		tiTfDefinees = listFreeCons(Symbol)(tiTfDefinees);
	}

}

void
tiTfPopDefinee0(Symbol sym)
{
	tiTfDefinees = listFreeCons(Symbol)(tiTfDefinees);
}

Bool
tiTfDoDefault(Sefo sefo)
{
	/* Skip AB_Default trees if we will traverse them later. */
	if (abTag(sefo) != AB_Default)
		return true;
	if (tiTfDoingDefault == DEF_State_NotYet)
		return false;
	return true;
}

local Bool
tiTfIsBoundary(TFormUses tfu)
{
	return tfu->isExplicitImport || tfu->isParamImport;
}

local Bool
tqShouldImport(TQual tq)
{
	TForm	tf;
	Symbol	sym = NULL;

	if (tq == NULL)
		return false;

	tf = tqBase(tq);

	tfFollow(tf);
	if (tfIsNotDomain(tf) || tfSatDom(tf) || tfSatCat(tf))
		return false;

	if (tfIsVariable(tf) || tfIsAnyMap(tf) || tfIsMulti(tf))
		return false;

	if (tfIsId(tf))
		sym = tfIdSym(tf);
	else if (tfIsApply(tf) && abTag(tfApplyOp(tf)) == AB_Id)
		sym = tfApplyOp(tf)->abId.sym;
	else
		return true;
	
	if (sym == ssymSelf)
		return false;
	
	if (listMemq(Symbol)(tiTfDeclarees, sym) ||
	    listMemq(Symbol)(tiTfDefinees,  sym))
		return false;
	
	return true;
}

/*****************************************************************************
 *
 * :: typeInferTForms phases
 *
 ****************************************************************************/

/* Return the first group of tforms from tful which s/b inferred as a group. */
local TFormUsesList
tiTfPartition(Stab stab, TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = cdr(tful0); tful; tful = cdr(tful))
		if (tiTfIsBoundary(car(tful)))
			break;

	return tful;
}

/* Fill in syntax type forms which appear as type form components. */
local TFormUsesList
tiTfSyntax(Stab stab, TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful))
		tiTfSyntax1(stab, car(tful), car(tful)->tf, listNil(AbSyn));

	return tful0;
}

/* Select the syntax type forms. */
local TFormUsesList
tiTfSelect(Stab stab, TFormUsesList tful)
{
	TFormUsesList	tful0 = listNil(TFormUses);

	tiTfPrintDb(titf)(dbOut, stab, ">>tiTfSelect:", tful);

	for (; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);

		if (tfIsSyntax(tfu->tf))
			tful0 = listCons(TFormUses)(tfu, tful0);
	}

	tful0 = listNReverse(TFormUses)(tful0);

	tiTfPrintDb(titf)(dbOut, stab, "<<tiTfSelect:", tful0);

	return tful0;
}

local Bool
tiTfSyntax1(Stab stab, TFormUses tfu, TForm tf, AbSynList params)
{
	Bool	result = true;

	if (!tfIsPending(tf))
		result = false;

	else if (tfIsSyntax(tf))
		result = tiTfFloat1(stab, tf);

	else if (tfIsAnyMap(tf))
		tiTfMap1(stab, tfu, tf, params);

	else if (tfIsDefine(tf))
		result = tiTfDefine1(stab, tfu, tf, params);

	else
		result = false;

	return result;
}

local Bool
tiTfFloat1(Stab stab, TForm tf)
{
	TForm	ntf;
	Stab	nstab;

	nstab = tfFloat(stab, tf);
	if (nstab == NULL)
		return false;

	tiTfEnterDb(titfOne)(dbOut, "tiTfFloat1", NULL, tf);

	ntf = typeInferTForm(nstab, tf);
	tfTransferSemantics(ntf, tf);

	tiTfExitDb(titfOne)(dbOut, "tiTfFloat1", NULL, tf);

	return true;
}

local void
tiTfMap1(Stab stab, TFormUses tfu, TForm tf, AbSynList params)
{
	AbSyn	ab = tfGetExpr(tf);
	Stab	nstab = (abStab(ab) ? abStab(ab) : stab);
	TForm	tfarg = tfMapArg(tf);
	TForm	tfret = tfMapRet(tf);
	Length	i;

	tiTfEnterDb(titfOne)(dbOut, "tiTfMap1", NULL, tf);

	if (nstab != stab) {
		stabSeeOuterImports(nstab);
		stabGetSubstable(nstab);
		typeInferTForms(nstab);
	}

	if (tfIsMulti(tfarg))
		for (i = 0; i < tfArgc(tfarg); i += 1)
			typeInferTForm(nstab, tfFollowArg(tfarg, i));
	typeInferTForm(nstab, tfarg);

	params = listCons(AbSyn)(abMapArg(ab), params);
	if (!tiTfSyntax1(nstab, tfu, tfret, params))
		typeInferTForm(nstab, tfret);
	listFreeCons(AbSyn)(params);

	tiTfBottomUp1	(stab, tfu, tf);
	tiTfAudit1	(stab, tf);
	tiTfTopDown1	(stab, tf);
	tiTfMeaning1	(stab, tf);

	tiTfExitDb(titfOne)(dbOut, "tiTfMap1", NULL, tf);
}

local Bool
tiTfDefine1(Stab stab, TFormUses tfu, TForm tf, AbSynList params)
{
	TForm	tfd = tfDefineDecl(tf);
	AbSyn	abd = tfGetExpr(tfd);
	Bool	result = true;

	if (abd && abHasTag(abd, AB_Declare))
		abd = abd->abDeclare.type;
	else
		return false;

	if (!abd)
		result = false;

	else if (abIsUnknown(abd))
		tiTfUnknown1(stab, tfu, tf, params);

	else if (abIsTheId(abd, ssymCategory))
		tiTfThird1(stab, tfu, tf, params);

	else /* if (abHasTag(abd, AB_With)) */
		tiTfCategory1(stab, tfu, tf, params);

	return result;
}

local void
tiTfThird1(Stab stab, TFormUses tfu, TForm tf, AbSynList params)
{
	TForm	tfc = tfDeclareType(tfDefineDecl(tf));
	TForm	tfw = tfDefineVal(tf);
	Symbol	sym = tiTfUsesSymbol(tfu);
	AbSyn	ab, abc, abw;

	tiTfEnterDb(titfOne)(dbOut, "tiTfThird1", tfu, tf);

	tiTfPushDefinee0(sym);

	abw = tfExpr(tfw);

	typeInferAs(stab, abw, tfCategory);

	tiTfOne(stab, NULL, tfw);
	/* typeInferTForm(stab, tfw); */
	if (tfIsSyntax(tfc))
		tfForwardFrSyntax(tfc, tfThirdFrTForm(tfw));
	tiTfPopDefinee0(sym);

	ab  = tfExpr(tf);
	abc = abDefineDecl(ab)->abDeclare.type;
	abw = abDefineVal(ab);

	abTransferSemantics(tfGetExpr(tfc), abc);
	abTransferSemantics(tfGetExpr(tfw), abw);

	abSetPos(abc, abPos(tfGetExpr(tfc)));
	abSetPos(abw, abPos(tfGetExpr(tfw)));

	if (abState(ab) != AB_State_HasUnique) {
		abTUnique(ab) = tfType;
		abState(ab) = AB_State_HasUnique;
	}

	tfFollow(tfc);
	tfSetPending(tfc);
	tfSetPending(tf);

	if (sym && sym != ssymJoin) {
		AbSyn		abd;
		TForm		tfd;
		Syme		pp;
		AbSynList	pl;
		Stab		nstab = (abStab(abw) ? abStab(abw) : stab);

		abd = abNewDefineLhs(sym, params);
		tfd = tiGetTForm(stab, abd);
		pp = symeNewExport(ssymSelfSelf, tfd, car(nstab));
		symeSetDefault(pp);
		tfSetSymes(tfc, listCons(Syme)(pp, listNil(Syme)));
	}

	tfSetMeaning(tfc);
	tfCheckConsts(tfc);

	tfSetMeaning(tf);
	tfCheckConsts(tf);
	tiTfMeaning1(stab, tf);

	tiTfExitDb(titfOne)(dbOut, "tiTfThird1", tfu, tf);
}

/*
 * This function adds as many symes from the specified list to
 * the symes of tf as possible. We leave out %% symes because
 * they cause too many problems and probably ought not to be
 * seen in tfSymes() for add bodies. Hopefully this function
 * won't be invoked too many times (just once for each add in
 * a program) so its O(N^2) cost won't hurt too much.
 */
local void
tiAppendSymes(TForm tf, SymeList symes)
{
	SymeList originals = tfSymes(tf);
	SymeList additions = listNil(Syme);


	/* Check each syme in symes */
	for (;symes;symes = cdr(symes))
	{
		Syme syme = car(symes);


		/* Skip %% */
		if (symeIsSelfSelf(syme)) continue;


		/* Skip symes already in tfSymes(tf) */
		if (symeListMember(syme, originals, symeEqual)) continue;


		/* Remember this additional syme */
		additions = listCons(Syme)(syme, additions);
	}


	/* Do nothing if nothing to add */
	if (!additions) return;


	/* Reverse the new list */
	additions = listNReverse(Syme)(additions);


	/* Debugging */
	if (DEBUG(symeRefresh)) {
		(void)fprintf(dbOut, "-----------> (extra)\n");
		symeListPrintDb(additions);
		(void)fprintf(dbOut, "\n\n");
	}


	/* Append onto tfSymes(tf) */
	originals = listNConcat(Syme)(originals, additions);


	/* Unnecessary, but makes things obvious */
	tfSetSymes(tf, originals);
}


local void
tiTfCategory1(Stab stab, TFormUses tfu, TForm tf, AbSynList params)
{
	TForm	tfw = tfDeclareType(tfDefineDecl(tf));
	TForm	tfa = tfDefineVal(tf);
	Symbol	sym = tiTfUsesSymbol(tfu);
	AbSyn	ab, abw, aba, abt = NULL;
	Stab	nstab;

	tiTfEnterDb(titfOne)(dbOut, "tiTfCategory1", NULL, tf);

	tiTfPushDefinee0(sym);

	typeInferTForm(stab, tfw);

	/* Process the add tform (tfa) to get the add symes, but
	 * without analyzing the add body, to avoid unnecessary
	 * dependencies between type forms.
	 * The add body will be analyzed in its normal sequence.
	 */
	aba = tfGetExpr(tfa);
	if (tfIsSyntax(tfa))
		tfForwardFrSyntax(tfa, tfPending(stab, aba));
	tfFollow(tfa);

	/* Identify % in the add body (aba) with the definee. */
	if (abTag(aba) == AB_Sequence && abArgc(aba) > 0)
		aba = abArgv(aba)[abArgc(aba) - 1];
	nstab = abStab(aba);
	if (nstab == NULL) nstab = stab;

	if (sym) {
		SymeList	aself  = tfGetSelfFrStab(nstab);
		Syme		asyme  = (aself ? car(aself) : NULL);
		Syme		xsyme;
		AbSyn		abd;
		AbSynList	pl;
		TForm		tfd, val;

		if (asyme) {
			Stab	istab = (params ? stab : nstab);
			abd = abNewDefineLhs(sym, params);
			tfd = tfSyntaxFrAbSyn(istab, abd);
			tfd = tfDefineOfType(tfd);
			xsyme = stabDefExtend(nstab, ssymSelf, tfd);
			symeAddExtendee(xsyme, asyme);
			symeSetExtension(asyme, xsyme);
			stabExtendMeanings(nstab, xsyme);

			if (nstab != stab) {
				stabSeeOuterImports(nstab);
				stabGetSubstable(nstab);
			}

			typeInferAs(istab, abd, tfType);
			val = tfDefineVal(tfd);
			if (tfIsSyntax(val)) {
				tfMeaningFrSyntax(istab, abd, val);
				tfSetMeaningArgs(tfd);
			}
			abt = abd;
		}
	}

	if (nstab != stab) {
		stabSeeOuterImports(nstab);
		stabGetSubstable(nstab);
	}

	if (nstab != stab) {
		/*!! Try to fill the types for parameterized add symes. */
		if (params) typeInferTForms(nstab);
		tfSetSymes(tfa, stabGetExportedSymes(nstab));
		tfGetSelf(nstab, tfa);
	}
	tfSetMeaning(tfa);
	tfCheckConsts(tfa);

	tfSetMeaning(tf);
	tfCheckConsts(tf);

	if (abTag(aba) == AB_Add) {
		AbSyn		base    = aba->abAdd.base;
		AbSyn		capsule = aba->abAdd.capsule;
		AbSynList	extl = tfu ? tfu->extension : NULL;
		SymeList	extras;

		tiGetTForm(nstab, base);
		for (; extl; extl = cdr(extl)) {
			AbSyn	ab0 = car(extl);
			Syme	syme = abSyme(ab0);
			Syme	extension = symeExtension(syme);
			TForm	tf = symeType(extension);
			Syme	osyme;

			assert(ab0 && abHasTag(ab0, AB_Id));
			assert(syme && symeIsExport(syme));
			assert(extension && symeIsExtend(extension));
			assert(listMemq(Syme)(symeExtendee(extension), syme));

			osyme = tiGetExtendee(stab, ab0, tf);
			if (osyme) {
				TForm	otf = symeType(osyme);
				if (tfIsAnyMap(otf))
					otf = tfExtendeeSubst(nstab, otf, tf);
				tiWithSymes(nstab, otf);
			}
		}
		tiAddSymes(nstab, capsule, abTForm(base), tfw, &extras);

		/* When is tfu allowed to be NULL? */
		if (tfu) {
			listFree(AbSyn)(tfu->extension);
			tfu->extension = NULL;
		}

		/* Add the new symes to tfSymes for the add */
		tiAppendSymes(tfa, extras);
	}
	if (abt && tfu)
		tfu->extension = listCons(AbSyn)(abt, listNil(AbSyn));

	tiTfPopDefinee0(sym);

	ab  = tfExpr(tf);
	abw = abDefineDecl(ab)->abDeclare.type;
	aba = abDefineVal(ab);

	abTransferSemantics(tfGetExpr(tfw), abw);
	abTransferSemantics(tfGetExpr(tfa), aba);

	abSetPos(abw, abPos(tfGetExpr(tfw)));
	abSetPos(aba, abPos(tfGetExpr(tfa)));

	if (abState(ab) != AB_State_HasUnique) {
		abTUnique(ab) = tfType;
		abState(ab) = AB_State_HasUnique;
	}
	tiTfMeaning1(stab, tfDefineDecl(tf));
	tiTfMeaning1(stab, tf);
	
	tiTfExitDb(titfOne)(dbOut, "tiTfCategory1", NULL, tf);
}

local void
tiTfUnknown1(Stab stab, TFormUses tfu, TForm tf, AbSynList params)
{
	TForm	tfl = tfDeclareType(tfDefineDecl(tf));
	TForm	tfr = tfDefineVal(tf);
	Symbol	sym = tiTfUsesSymbol(tfu);
	AbSyn	ab, abl, abr;

	tiTfEnterDb(titfOne)(dbOut, "tiTfUnknown1", NULL, tf);

	tiTfPushDefinee0(sym);

	typeInferAs(stab, tfExpr(tfr), tfUnknown);
	if (tfIsSyntax(tfr))
		tfMeaningFrSyntax(stab, tfGetExpr(tfr), tfr);
	tfFollow(tfr);
	
	typeInferTForm(stab, tfl);

	tfSetMeaning(tf);
	tfCheckConsts(tf);

	tiTfPopDefinee0(sym);

	ab  = tfExpr(tf);
	abl = abDefineDecl(ab)->abDeclare.type;
	abr = abDefineVal(ab);

	abTransferSemantics(tfGetExpr(tfl), abl);
	abTransferSemantics(tfGetExpr(tfr), abr);

	abSetPos(abl, abPos(tfGetExpr(tfl)));
	abSetPos(abr, abPos(tfGetExpr(tfr)));

	if (abState(ab) != AB_State_HasUnique) {
		abTUnique(ab) = tfType;
		abState(ab) = AB_State_HasUnique;
	}
	tiTfMeaning1(stab, tf);

	tiTfExitDb(titfOne)(dbOut, "tiTfUnknown1", NULL, tf);
}

/* Convert syntax tforms to pending tforms. */
local void
tiTfPending1(Stab stab, TForm tf)
{
	tiTfEnterDb(titfOne)(dbOut, "tiTfPending1", NULL, tf);
	tfPendingFrSyntax(stab, tfGetExpr(tf), tf);
	tiTfExitDb(titfOne)(dbOut, "tiTfPending1", NULL, tf);
}

local AbLogic tiTfCondition(Stab stab, TForm tf);
/* Perform bottom-up analysis to generate tposs sets. */
local void
tiTfBottomUp1(Stab stab, TFormUses tfu, TForm tf)
{
	Scope("tiTfBottomUp1");
	SymbolList	ol = listNil(Symbol);
	AbSyn		absyn = tfGetExpr(tf);
	TForm		type = tfTypeTuple;
	AbLogic    fluid(abCondKnown);

	abCondKnown = tiTfCondition(stab, tf);

	if (abUse(absyn) == AB_Use_Define || abUse(absyn) == AB_Use_Assign)
		type = tfUnknown;
	else if (abUse(absyn) == AB_Use_Except)
		type = tfTuple(tfCategory);

	tiTfEnterDb(titfOne)(dbOut, "tiTfBottomUp1", tfu, tf);
	ol = tiTfPushDeclarees(tiTfGetDeclarees(tfu));
	tiBottomUp(stab, absyn, type);
	tiTfPopDeclarees(ol);
	tiTfExitDb(titfOne)(dbOut, "tiTfBottomUp1", tfu, tf);

	Return(Nothing);
}

local AbLogic
tiTfCondition(Stab stab, TForm tf)
{
	Scope("tiTfCondition");
	AbSynList condition = tfConditionalAbSyn(tf);
	AbSyn	absyn = tfGetExpr(tf);
	AbLogic rule = ablogTrue();
	AbLogic fluid(abCondKnown);

	while (condition != listNil(AbSyn)) {
		Stab cstab = stab;/*tfConditionalStab(tf);*/
		AbSyn ab = car(condition);

		assert(cstab != NULL);
		if (DEBUG(titf)) {
			afprintf(dbOut, "Condition: ");
			abPrintDb(car(condition));
		}
		if (abContains(ab, absyn)) {
			condition = cdr(condition);
			continue;
		}
		abCondKnown = rule;
		tiBottomUp(cstab, ab, tfUnknown);
		tiTopDown (cstab, ab, tfUnknown);
		rule = ablogAnd(rule, ablogFrSefo(ab));
		condition = cdr(condition);
	}
	TfCond conds = tfConditions(tf);

	Return(rule);
}

/* Audit the bottom-up type analysis phase. */
local void
tiTfAudit1(Stab stab, TForm tf)
{
	tiTfEnterDb(titfOne)(dbOut, "tiTfAudit1", NULL, tf);
	typeInferAudit(stab, tfGetExpr(tf));
	tiTfExitDb(titfOne)(dbOut, "tiTfAudit1", NULL, tf);
}

/* Perform top-down analysis to generate semantics for each AbSyn. */
local void
tiTfTopDown1(Stab stab, TForm tf)
{
	Scope("tiTfTopDown1");
	AbSyn		absyn = tfGetExpr(tf);
	TForm		type = tfTypeTuple;
	AbLogic         fluid(abCondKnown);

	abCondKnown = tiTfCondition(stab, tf);

	if (abUse(absyn) == AB_Use_Define || abUse(absyn) == AB_Use_Assign)
		type = tfUnknown;
	else if (abUse(absyn) == AB_Use_Except)
		type = tfTuple(tfCategory);

	tiTfEnterDb(titfOne)(dbOut, "tiTfTopDown1", NULL, tf);
	tiTopDown(stab, absyn, type);
	tiTfExitDb(titfOne)(dbOut, "tiTfTopDown1", NULL, tf);

	Return(Nothing);
}

/* Convert the pending type forms to full type forms. */
local void
tiTfMeaning1(Stab stab, TForm tf)
{
	tiTfEnterDb(titfOne)(dbOut, "tiTfMeaning1", NULL, tf);
	tfMeaning(stab, tfGetExpr(tf), tf);
	tiTfExitDb(titfOne)(dbOut, "tiTfMeaning1", NULL, tf);
}

/* Fill the extension type using the type form as the type of an extendee. */
local void
tiTfExtend1(Stab stab, TFormUses tfu)
{
	AbSynList	alist;

	tiTfEnterDb(titfOne)(dbOut, "tiTfExtend1", tfu, tfu->tf);

	for (alist = tfu->extendees; alist; alist = cdr(alist)) {
		AbSyn	ab = car(alist), abt;
		Syme	syme = abSyme(ab), osyme = NULL;
		Syme	extension = symeExtension(syme);
		TForm	tf = symeType(extension);
		TFormUses tu0;
		
		titfOneDEBUG(dbOut, "Syme: %pSyme TForm: %pTForm\n", syme, tf);

		assert(ab && abHasTag(ab, AB_Id));
		assert(syme && symeIsExport(syme));
		assert(extension && symeIsExtend(extension));
		assert(listMemq(Syme)(symeExtendee(extension), syme));

		if (tfIsExtendTemplate(tf)) {
			SymeList	extendee = symeExtendee(extension);
			Length		argc = listLength(Syme)(extendee);

			osyme = tiGetExtendee(stab, ab, tf);
			if (osyme == NULL) continue;

			tf = tfExtendEmpty(tf, argc + 1);
			symeSetType(extension, tf);

			tfExtendFill(tf, tfExtendNext(tf), symeType(osyme));
			symeAddExtendee(extension, osyme);
			symeSetExtension(osyme, extension);
		}

		symeSetExtension(syme, NULL);

		tfExtendFill(tf, tfExtendNext(tf), symeType(syme));

		abt = tfGetExpr(symeType(syme));
		tu0 = stabFindTFormUses(stab, abt);
		abt = tu0 && tu0->extension ? car(tu0->extension) : NULL;
		if (abt && osyme && abIsId(abt) && abSyme(abt) == osyme) {
			abState(abt) = AB_State_AbSyn;
			typeInferAs(stab, abt, tfType);
		}

		symeSetExtension(syme, extension);

		if (tfExtendDone(tf)) {
			if (DEBUG(tipExtend)) {
				fprintf(dbOut, "titfExtend1:\n");
				symePrint(dbOut, extension);
				fnewline(dbOut);
			}
			tfExtendSubst(stab, tf);
			tfExtendFinish(tf);
			stabExtendMeanings(stab, extension);
		}
	}
		
	tiTfExitDb(titfOne)(dbOut, "tiTfExtend1", tfu, tfu->tf);
}

local void
typeInferTFormList(Stab stab, TFormList tfl)
{
	for (; tfl; tfl = cdr(tfl)) {
		TForm	tf = car(tfl);
		if (tfIsJoin(tf)) {
			Length	i;
			for (i = 0; i < tfJoinArgc(tf); i += 1)
				typeInferTForm(stab, tfJoinArgN(tf, i));
			tfSetMeaningArgs(tf);
			assert(tfIsMeaning(tf));
		}
		else
			typeInferTForm(stab, tf);
	}
}

/* Import the exports from each type form. */
local void
tiTfImport1(Stab stab, TFormUses tfu)
{
	Scope("tiTfImport1");
	AbLogic    fluid(abCondKnown);

	TForm	tf = tfFollow(tfu->tf);
	abCondKnown = tiTfCondition(stab, tf);

	tiTfEnterDb(titfOne)(dbOut, "tiTfImport1", tfu, tf);

	if (tiTfDoingDefault == DEF_State_NotYet )
		Return(Nothing);

	if (tfu->exports)
		typeInferTFormList(stab, tqQual(tfu->exports));

	if (tqShouldImport(tfu->imports)) {
		TQualList cascades;
		typeInferTFormList(stab, tqQual(tfu->imports));

		if (tfQueries(tf))
			typeInferTFormList(stab, tfQueries(tf));

		cascades = stabImportFrom(stab, tfu->imports);

		if (tfu != NULL)
			tfu->cascades = cascades;
		else
			tiTfImportCascades(stab, cascades);
	}
	else if (tfu->isCategoryImport)
		tiWithSymes(stab, tf);
	else if (tfu->isCatConditionImport) {
		Syme self;
		Stab sstab = stab;
		TForm cond;
		assert(sstab); /* -- MND -- */
		self = NULL;   /* Avoid uninitialized msg */
		while ( sstab && (self = stabGetSelf(sstab)) == NULL)
			sstab = cdr(sstab);
		cond = tfIf(tfHas(self, tf), tf, tfNone());
		tfSetMeaning(cond);
		tiWithSymes(stab, cond);
	}
	else if (tfu->isExplicitImport) {
		String s = tfPretty(tf);
		comsgWarning(abNewNothing(car(stab)->spos), ALDOR_W_StabNotImporting, s);
		strFree(s);
	}

	tiTfExitDb(titfOne)(dbOut, "tiTfImport1", tfu, tf);
	Return(Nothing);
}

void
tiTfCascades1(Stab stab, TFormUses tfu)
{
	tiTfImportCascades(stab, tfu->cascades);
}

void
tiTfImportCascades(Stab stab, TQualList list)
{
	TQualList ql;

	for (ql = list; ql; ql = cdr(ql)) {
		TQual innerTq = car(ql);
		if (!tqShouldImport(innerTq))
			continue;
		if (tqIsQualified(innerTq)) {
			stabImportFrom(stab, innerTq);
		}
		else {
			TQualList moreImports;
			TForm innerTf = tqBase(innerTq);
			TForm tf = stabFindOuterTForm(stab, tfExpr(innerTf));
			if (tf != NULL) {
				tf = typeInferTForm(stab, tf);
				if (tfQueries(tf))
					typeInferTFormList(stab, tfQueries(tf));

				innerTf = tf;
			}
			innerTq = tqNewUnqualified(innerTf);
			moreImports = stabImportFrom(stab, innerTq);
			ql = listNConcat(TQual)(ql, listCopy(TQual)(moreImports));
			tqFree(innerTq);
		}
	}
}


/* Type infer the default clauses for each type form. */
local void
tiTfDefault1(Stab stab, Sefo sefo)
{
	if (abStab(sefo))
		stab = abStab(sefo);

	if (abTag(sefo) == AB_Default) {
		abState(sefo) = AB_State_AbSyn;
		typeInferAs(stab, sefo, tfUnknown);
	}
	else if (!abIsLeaf(sefo)) {
		Length i;
		for (i = 0; i < abArgc(sefo); i += 1)
			tiTfDefault1(stab, abArgv(sefo)[i]);
	}
}

/*****************************************************************************
 *
 * :: typeInferTForms topological sorting
 *
 ****************************************************************************/

local Table
tiTfGetDeclareeTable(TFormUsesList tful0)
{
	Table		tbl = tblNew((TblHashFun) 0, (TblEqFun) 0);
	TFormUsesList	tful, tfl;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		SymbolList	dl = tfu->declarees;

		for (; dl; dl = cdr(dl)) {
			tfl = (TFormUsesList) tblElt(tbl, car(dl),
						     listNil(TFormUses));
			tblSetElt(tbl, car(dl), listCons(TFormUses)(tfu, tfl));
		}
	}

	if (DEBUG(titf)) {
		fprintf(dbOut, ">>tiTfGetDeclareeTable:\n");
		tblPrint(dbOut, tbl, (TblPrKeyFun) symPrint,
			 (TblPrEltFun) tfulPrint);
		fnewline(dbOut);
	}

	return tbl;
}

local void
tiTfFreeDeclareeTable(Table tbl)
{
	tblFreeDeeply(tbl,
	              (TblFreeKeyFun) 0, (TblFreeEltFun) listFree(TFormUses));
}

local void
tiTfCollectHasDependees(TFormUsesList tful, TFormUses S)
{
	TFormList	hl;

	for (hl = tfQueries(S->tf); hl; hl = cdr(hl)) {
		TForm		cat = car(hl);
		TFormUsesList	tfl;
		for (tfl = tful; tfl; tfl = cdr(tfl))
			if (cat == car(tfl)->tf)
				tiTfAddDependee(S, car(tfl));
	}
}

local void
tiTfCollectDependees(Table tbl, TFormUses S, TForm tf)
{
	Symbol		sym = tiTfUsesSymbol(S);
	AbSynList	al, l;

	if (sym && tfIsCategoryMap(tf))
		tiTfCollectSymDependees(tbl, S, sym);
	for (al = S->extendees; al; al = cdr(al))
		tiTfCollectSefoDependees(tbl, S, car(al));
	tiTfCollectSefoDependees(tbl, S, tfGetExpr(tf));
	l = tfConditionalAbSyn(tf);
	while (l != listNil(AbSyn)) {
		tiTfCollectSefoDependees(tbl, S, car(l));
		l = cdr(l);
	}
}

local void
tiTfCollectSefoDependees(Table tbl, TFormUses S, Sefo sefo)
{
	if (abIsStrTag(abTag(sefo))) {
		Symbol		sym = NULL;

		if (abTag(sefo) == AB_LitInteger)
			sym = ssymTheInteger;
		else if (abTag(sefo) == AB_LitString)
			sym = ssymTheString;
		else if (abTag(sefo) == AB_LitFloat)
			sym = ssymTheFloat;

		assert(sym);
		tiTfCollectSymDependees(tbl, S, sym);
	}

	else if (abIsSymTag(abTag(sefo)))
		tiTfCollectSymDependees(tbl, S, abLeafSym(sefo));

	else if (tiTfDoDefault(sefo)) {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1)
			tiTfCollectSefoDependees(tbl, S, abArgv(sefo)[i]);
	}
}

local void
tiTfCollectSymDependees(Table tbl, TFormUses S, Symbol sym)
{
	TFormUsesList	tfl;

	tfl = (TFormUsesList) tblElt(tbl, sym, listNil(TFormUses));
	for (; tfl; tfl = cdr(tfl))
		tiTfAddDependee(S, car(tfl));
}

local void
tiTfAddDependee(TFormUses S, TFormUses T)
{
	/* T must be type analyzed before S. */
	if (S != T && !listMemq(TFormUses)(S->dependees, T)) {
		listPush(TFormUses, T, S->dependees);
		listPush(TFormUses, S, T->dependents);
		T->nafter  += 1;
		S->nbefore += 1;
	}
}

local void
tiTfFreeDependees(TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);

		listFree(TFormUses)(tfu->dependents);
		listFree(TFormUses)(tfu->dependees);

		tfu->nbefore	= 0;
		tfu->nafter	= 0;
		tfu->dependents	= listNil(TFormUses);
		tfu->dependees	= listNil(TFormUses);
	}
}

local void
tiTfFreeCDependees(TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);

		listFree(TFormUses)(tfu->cdependents);
		listFree(TFormUses)(tfu->cdependees);

		tfu->ncbefore	= 0;
		tfu->ncafter	= 0;
		tfu->cdependents= listNil(TFormUses);
		tfu->cdependees	= listNil(TFormUses);
	}
}

local TFormUsesList
tiTopForward(TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		tfu->inDegree = tfu->nbefore;
		tfu->outEdges = tfu->dependents;
		assert(tfu->nbefore == listLength(TFormUses)(tfu->dependees));
		assert(tfu->nafter == listLength(TFormUses)(tfu->dependents));
	}

	return tiTopSort(tful0);
}

local TFormUsesList
tiTopReverse(TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		tfu->inDegree = tfu->nafter;
		tfu->outEdges = tfu->dependees;
		assert(tfu->nbefore == listLength(TFormUses)(tfu->dependees));
		assert(tfu->nafter == listLength(TFormUses)(tfu->dependents));
	}

	return listNReverse(TFormUses)(tiTopSort(tful0));
}

local TFormUsesList
tiTopCForward(TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		tfu->inDegree = tfu->ncbefore;
		tfu->outEdges = tfu->cdependents;
		assert(tfu->ncbefore==listLength(TFormUses)(tfu->cdependees));
		assert(tfu->ncafter==listLength(TFormUses)(tfu->cdependents));
	}

	return tiTopSort(tful0);
}

local TFormUsesList
tiTopCReverse(TFormUsesList tful0)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		tfu->inDegree = tfu->ncafter;
		tfu->outEdges = tfu->cdependees;
		assert(tfu->ncbefore==listLength(TFormUses)(tfu->cdependees));
		assert(tfu->ncafter==listLength(TFormUses)(tfu->cdependents));
	}

	return tiTopSort(tful0);
}

/* Use tfu->inDegree and tfu->outEdges to perform a topological sort. */
local TFormUsesList
tiTopSort(TFormUsesList tful0)
{
	TFormUsesList	tful, stack, result;

	/* tful collects the type forms which can now be first. */
	stack = listNil(TFormUses);
	for (tful = tful0; tful; tful = cdr(tful))
		if (car(tful)->inDegree == 0)
			stack = listCons(TFormUses)(car(tful), stack);

	result = listNil(TFormUses);
	while (stack) {
		/* Pop the next type form to be processed. */
		TFormUses	next = car(stack);
		stack = listFreeCons(TFormUses)(stack);

		assert(next->inDegree == 0);
		if (next->sortMark == false) {
			result = listCons(TFormUses)(next, result);
			next->sortMark = true;
		}

		/* Unmark the type forms which depend on next. */
		for (tful = next->outEdges; tful; tful = cdr(tful)) {
			TFormUses	tfu = car(tful);

			/* Assert that tfu is not yet on the list. */
			assert (tfu->inDegree > 0);

			/* If all of the type forms on which tfu depends
			 * have been processed, then tfu can be next.
			 */
			if ((tfu->inDegree -= 1) == 0)
				stack = listCons(TFormUses)(tfu, stack);
		}
	}

	result = listNReverse(TFormUses)(result);
	return result;
}

local TFormUsesList
tiTopCycle(TFormUsesList tful0)
{
	TFormUsesList	tful, cycle;

	/* Collect any type forms which participate in a cycle. */
	cycle = listNil(TFormUses);
	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		if (tfu->sortMark == false)
			cycle = listCons(TFormUses)(tfu, cycle);

		tfu->sortMark = false;
		tfu->inDegree = 0;
		tfu->outEdges = listNil(TFormUses);
	}

	cycle = listNReverse(TFormUses)(cycle);
	return cycle;
}

/*****************************************************************************
 *
 * :: tiTopClique
 *
 ****************************************************************************/

/* Topologically sort the cliques in the graph. */
local TFormUsesList
tiTopClique(TFormUsesList tful0)
{
	TFormUsesList	tful, tfl, graph, result;

	tiTfPrintDb(titf)(dbOut, NULL, ">>tiTopClique:", tful0);

	/* Mark the nodes in the clique graph without adding their edges. */
	tiTopCliqueMark(tful0, true);

	/* For each vertex, add the edges and sort to find the cliques. */
	graph = listCopy(TFormUses)(tful0);
	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful), crep;
		TFormUsesList	before, after, cycle;

		/* Add the edges of V to the clique graph. */
		tiTopCliqueAddVertex(tfu);

		/* Perform a topological sort on the graph. */
		before = tiTopCForward	(graph);
		after  = tiTopCReverse	(graph);
		cycle  = tiTopCycle	(graph);

		if (cycle) {
			/* Remove the cycle from the graph. */
			for (tfl = cycle; tfl; tfl = cdr(tfl))
				listPop(TFormUses,car(tfl),graph,tiTopEqual);

			/* Mark the cycle as part of the same clique. */
			crep = tiTopCliqueUnion(cycle);
			listPush(TFormUses, crep, graph);
		}

		listFree(TFormUses)(before);
		listFree(TFormUses)(after);
		listFree(TFormUses)(cycle);
	}

	/* Topologically sort the final clique graph. */
	result = tiTopCForward(graph);

	/* Free the clique graph. */
	tiTfFreeCDependees(tful0);

	/* Collect the cliques on their canonical representative. */
	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		TFormUses	crep = tiTopCliqueRep(tfu);
		listPush(TFormUses, tfu, crep->cdependents);
	}
	for (tful = result; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		tfu->cdependents = listNReverse(TFormUses)(tfu->cdependents);
	}
	tiTopCliqueMark(tful0, false);

	tiTfPrintDb(titf)(dbOut, NULL, "<<tiTopClique:", result);

	return result;
}

local TFormUses
tiTopCliqueUnion(TFormUsesList cycle)
{
	TFormUsesList	tful;
	TFormUses	nrep = car(cycle);

	/* Mark the nodes in the cycle with a new canonical representative. */
	for (tful = cdr(cycle); tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		assert(tfu->crep == NULL);
		tfu->crep = nrep;
	}

	/* Move edges to the new canonical representative. */
	for (tful = cdr(cycle); tful; tful = cdr(tful))
		tiTopCliqueAddCVertex(car(tful));

	/* Delete edges to non-canonical nodes. */
	for (tful = cycle; tful; tful = cdr(tful))
		tiTopCliqueDelCVertex(car(tful));

	return nrep;
}

local TFormUses
tiTopCliqueRep(TFormUses S)
{
	if (S->crep) {
		S->crep = tiTopCliqueRep(S->crep);
		return S->crep;
	}
	else
		return S;
}

local void
tiTopCliqueMark(TFormUsesList tful0, Bool mark)
{
	TFormUsesList	tful;

	for (tful = tful0; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		tfu->sortMark	= false;
		tfu->cmarked	= true;
		tfu->crep	= NULL;
	}
}

local void
tiTopCliqueAddVertex(TFormUses S)
{
	TFormUsesList	tful;

	for (tful = S->dependees; tful; tful = cdr(tful))
		tiTopCliqueAddEdge(car(tful), S);

	for (tful = S->dependents; tful; tful = cdr(tful))
		tiTopCliqueAddEdge(S, car(tful));
}

local void
tiTopCliqueAddCVertex(TFormUses S)
{
	TFormUsesList	tful;

	for (tful = S->cdependees; tful; tful = cdr(tful))
		tiTopCliqueAddEdge(car(tful), S);

	for (tful = S->cdependents; tful; tful = cdr(tful))
		tiTopCliqueAddEdge(S, car(tful));
}

local void
tiTopCliqueDelCVertex(TFormUses S)
{
	TFormUsesList	tful;

	for (tful = S->cdependees; tful; ) {
		TFormUses	tfu = car(tful);
		tful = cdr(tful);		/* cdr before delete */
		tiTopCliqueDelEdge(tfu, S);
	}

	for (tful = S->cdependents; tful; ) {
		TFormUses	tfu = car(tful);
		tful = cdr(tful);		/* cdr before delete */
		tiTopCliqueDelEdge(S, tfu);
	}
}

/* Add an edge from S to T to the clique graph. */
local void
tiTopCliqueAddEdge(TFormUses S, TFormUses T)
{
	S = tiTopCliqueRep(S);
	T = tiTopCliqueRep(T);

	if (S == T) return;
	if (S->cmarked == false || T->cmarked == false) return;
	if (listMemq(TFormUses)(S->cdependents, T)) return;
	assert(!listMemq(TFormUses)(T->cdependees, S));

	listPush(TFormUses, T, S->cdependents);
	listPush(TFormUses, S, T->cdependees);
	T->ncbefore += 1;
	S->ncafter  += 1;

	assert(T->ncbefore == listLength(TFormUses)(T->cdependees));
	assert(S->ncafter  == listLength(TFormUses)(S->cdependents));
}

local void
tiTopCliqueDelEdge(TFormUses S, TFormUses T)
{
	if (S == tiTopCliqueRep(S) && T == tiTopCliqueRep(T)) return;
	assert(listMemq(TFormUses)(S->cdependents, T));
	assert(listMemq(TFormUses)(T->cdependees,  S));

	listPop(TFormUses, T, S->cdependents, tiTopEqual);
	listPop(TFormUses, S, T->cdependees,  tiTopEqual);
	T->ncbefore -= 1;
	S->ncafter  -= 1;

	assert(!listMemq(TFormUses)(S->cdependents, T));
	assert(!listMemq(TFormUses)(T->cdependees,  S));

	assert(T->ncbefore == listLength(TFormUses)(T->cdependees));
	assert(S->ncafter  == listLength(TFormUses)(S->cdependents));
}

local Bool
tiTopEqual(TFormUses tfu0, TFormUses tfu1)
{
	return tfu0 == tfu1;
}

/******************************************************************************
 *
 * :: SetSoftMissing, GetSoftMissing
 *
 *****************************************************************************/

/* When tiSoftMissing is `on', then the compiler will give a warning msg
 * (instead of an error msg) when a domain implementation is missing some
 * exports.
 */
static Bool tiSoftMissing = false;

void
tiSetSoftMissing(Bool state)
{
	tiSoftMissing = state;
}

Bool
tiIsSoftMissing() { return tiSoftMissing; }

/******************************************************************************
 *
 * :: abExpandDefs (normalise a piece of absyn)
 *
 *****************************************************************************/

AbSyn
abExpandDefs(Stab stab, AbSyn ab)
{
	TForm	tf = (TForm)NULL;
	int	i;
	AbSyn	newAb, tmpAb, def;
	TPoss tp;

	/* Safety check */
	assert(ab != 0);


	/* Deal with the simple cases. */
	if (abIsNothing(ab)) return ab;


	/* Bad absyn gets thrown back immediately */
	if (abState(ab) == AB_State_Error) return ab;


	/* Local copy of the abstract syntax node. */
	newAb = (AbSyn)sefoCopy((Sefo)ab);


	/*
	 * We MUST have a unique type for this to work. Type
	 * inferring at this level is dangerous and may blow
	 * up in our face (eg if still in tibupSelect).
	 */
	if (abState(newAb) == AB_State_HasPoss)
	{
		TPoss tp = abTPoss(newAb);

		/* Ensure that type inference is complete */
		if (!tpossIsUnique(tp))
			typeInferAs(stab, newAb, tfUnknown);
	}
	else if (abState(newAb) == AB_State_AbSyn)
		typeInferAs(stab, newAb, tfUnknown);


	/* Hopefully we have a unique type or singleton tposs */
	switch (abState(newAb))
	{
	   case AB_State_HasUnique:
		tf = abTUnique(newAb);
		break;
	   case AB_State_HasPoss:
		tp = abTPoss(newAb);
		if (tpossIsUnique(tp))
			tf = tpossUnique(tp);
		break;
	   default:
		break;
	}


	/* If we still don't have a unique type then give up */
	if (!tf) return ab;


	/* Recursively expand the definition */
	switch (abTag(newAb)) {
	   case AB_Not:
		tmpAb = newAb->abNot.expr;
		tmpAb = abExpandDefs(stab, tmpAb);
		newAb->abNot.expr = tmpAb;
		break;
	   case AB_And:
		if (abArgc(newAb) != 0) {
			for (i = 0; i < abArgc(newAb); i++) {
				tmpAb = newAb->abAnd.argv[i];
				tmpAb = abExpandDefs(stab, tmpAb);
				newAb->abAnd.argv[i] = tmpAb;
			}
		}
		break;
	   case AB_Or:
		if (abArgc(newAb) != 0) {
			for (i = 0; i < abArgc(newAb); i++) {
				tmpAb = newAb->abOr.argv[i];
				tmpAb = abExpandDefs(stab, tmpAb);
				newAb->abOr.argv[i] = tmpAb;
			}
		}
		break;
	   case AB_Id:
		/*
		 * If this symbol is a constant definition then we want
		 * to replace it with the definition body. This assumes
		 * that the correct RHS of a definition can always be obtained
		 * and therefore requires that the definition has lexical
		 * scoping. Dynamically scoped constants (fluids) are
		 * hopefully impossible.
		 *
		 * Only perform substitutions on definitions
		 */
		if (!((tfTag(tf) == TF_Define) && tfHasExpr(tf)))
			break;


		/* Get the absyn for the definition */
		def = tfGetExpr(tf);


		/* Safety check: I'm paranoid */
		if (abTag(def) != AB_Define)
			break;


		/* Use the expanded RHS of the definition */
		def = def->abDefine.rhs;


		/* A little bit of debugging info */
		if (DEBUG(abExpand)) {
			(void)fprintf(dbOut,
				      "abExpand: %s --> %s",
				      abPretty(newAb),
				      abPretty(def));
			fnewline(dbOut);
		}


		/* I really hope this can't loop */
		newAb = abExpandDefs(stab, def);
		break;
	   case AB_Test:
		tmpAb = newAb->abTest.cond;
		tmpAb = abExpandDefs(stab, tmpAb);
		newAb->abTest.cond = tmpAb;
		break;
	   default:
		ab = newAb;
		break;
	}

	return newAb;
}


