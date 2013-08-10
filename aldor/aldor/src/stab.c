/*****************************************************************************
 *
 * stab.c: Symbol table definitions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "fint.h"
#include "format.h"
#include "simpl.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "sefo.h"
#include "lib.h"
#include "tqual.h"
#include "tposs.h"
#include "tfsat.h"
#include "ablogic.h"
#include "abpretty.h"
#include "doc.h"
#include "comsg.h"
#include "table.h"
#include "strops.h"

Bool	stabDebug	= false;
Bool	stabImportDebug	= false;
Bool	stabConstDebug	= false;

#define stabDEBUG	DEBUG_IF(stab)		afprintf
#define stabImportDEBUG	DEBUG_IF(stabImport)	afprintf
#define stabConstDEBUG	DEBUG_IF(stabConst)	afprintf

/****************************************************************************
 *
 * :: Local function declarations.
 *
 ****************************************************************************/

local StabEntry		stabEntryNew		(void);
local StabEntry		stabEntryCopy		(StabEntry);

local void		stabEntryClearCache	(StabEntry);
local void		stabEntryAddCache	(StabEntry, AbLogic, SymeList);

local void		stabEntryPutSyme	(StabEntry, Length, Syme);
local void		stabEntryAddSyme	(StabEntry, Syme);

local Bool		stabEntryIsGeneric	(StabEntry);

local SymeList		stabEntryAllSymes	(StabEntry);
local SymeList		stabEntryGetSymes	(StabEntry, AbLogic);
local SymeList		stabEntryCacheSymes	(StabEntry, AbLogic);

local TPoss		stabEntryAllTypes	(StabEntry);
local TPoss		stabEntryGetTypes	(StabEntry, AbLogic);
local TPoss		stabEntryCacheTypes	(StabEntry, Length);

CREATE_LIST(TFormUses);

typedef			TFormUses(*UpdateTfuFun)(TFormUses);

local StabLevel		stabNewLevel		(int, int, SrcPos, Bool);
local StabEntry		stabGetEntry		(Stab, Symbol, Bool);
local int		stabPrEntry		(FILE *, TblKey);

local Bool		stabImportIsCovered	(SymeList, Syme, Stab);
local Bool		stabExtendIsCovered	(SymeList, Syme);

local void		stabPropagateDecl	(Stab, Syme);

local Bool		symeHasExtendee		(Syme, Syme);
local Syme		symeListHasImportee	(SymeList, Syme);
local void		symeRefreshExtendees	(Syme);

local Syme		stabGetSymeOfKind	(Stab, Symbol, SymeTag);
local Syme		stabLevelGetSymeOfKind	(Stab, Symbol, SymeTag);

local void		stabImportRemark	(Stab, TFormList, TForm);

local TFormUses		tfuNew			(TForm);
local TFormUses		tfuSetFlag	        (Stab, TForm, UpdateTfuFun);
local TFormUses		tformDoNothing		(TFormUses);
local TFormUses		tformIsImported		(TFormUses);
local TFormUses		tformExportAll		(TFormUses);
local TFormUses		tformImportAll		(TFormUses);
local TFormUses		tformInlineAll		(TFormUses);
local TFormUses		tformExplicitlyImportAll(TFormUses);
local TFormUses		tformCategoricallyImport(TFormUses);
local TFormUses		tformParameterImport	(TFormUses);

local TForm		addTFormUnused		(Stab stab, TForm tf);
local TForm		findTFormUnused		(Stab stab, AbSyn ab);
local void		removeTFormUnused	(Stab stab, TForm tf);

#define			stabUseList(stab,ab)	!stabLevelIsLarge(stab)
#define			stabUseTable(stab,ab)	 stabLevelIsLarge(stab)

#define			tfuSetUnqualified(f,tf)	\
	((f) = (f) ? tqSetUnqualified(f) : tqNewUnqualified(tf))
#define			tfuSetQualified(f,tf,q)	\
	((f) = (f) ? (tqIsQualified(f) ? tqAddQual(f,q) : (f)) \
	 	   : tqNewQualified(tf,q))

#define			tfuCreateIfNeeded(f,tf)	((f)=(f)?(f):tqNewEmpty(tf))

/****************************************************************************
 *
 * :: -Wdumb-import options
 *
 ****************************************************************************/

static Bool	stabDumbImportFlag = false;

Bool
stabDumbImport(void)
{
	return stabDumbImportFlag;
}


void
stabSetDumbImport(Bool flag)
{
	stabDumbImportFlag = flag;
}

/****************************************************************************
 *
 * :: stabMaxSerialNo/stabMaxDefnNum
 *
 ****************************************************************************/

static ULong stabSerialNoCounter = 0;		/* unique index per stab lvl */
static ULong stabDefinitionCounter = 0;		/* unique index per defn */

ULong
stabMaxSerialNo(void)
{
	return stabSerialNoCounter;
}

UShort
stabMaxDefnNum(void)
{
	return stabDefinitionCounter;
}

/****************************************************************************
 *
 * :: StabGlobal/StabFile
 *
 ****************************************************************************/

static Stab StabGlobal = 0;	/* Persists across all files. */
static Stab StabFile = 0;	/* Persists across one file. */

void
stabInitGlobal(void)
{
	StabGlobal = stabNewGlobal();
	return;
}

void
stabInitFile(void)
{
	StabFile = stabNewFile(StabGlobal);
	stabDefinitionCounter = 0;
	return;
}

Stab
stabNewGlobal(void)
{
	StabLevel slev = stabNewLevel(int0, int0, sposNone, false); /* small */

	Stab stab = listCons(StabLevel) (slev, listNil(StabLevel));
	stabClrSubstable(stab);

	return stab;
}

Stab
stabNewFile(Stab globalStab)
{
	StabLevel slev = stabNewLevel(1, 1, sposNone, true); /* large level */

	Stab fileStab = listCons(StabLevel) (slev, globalStab);
	assert(listLength(StabLevel)(globalStab) == 1);

	stabClrSubstable(fileStab);

	return fileStab;
}

void
stabFiniGlobal(void)
{
	/* stabFree(StabGlobal); */
	StabGlobal = 0;
	return;
}

void
stabFiniFile(void)
{
	/* stabFree(StabFile); */
	StabFile = 0;
	return;
}

Stab
stabGlobal(void)
{
	return StabGlobal;
}

Stab
stabFile(void)
{
	return StabFile ? StabFile : StabGlobal;
}

/****************************************************************************
 *
 * :: Symbol table entries.
 *
 ****************************************************************************/

/*
 * Each stab entry has an array of syme lists, keyed by condition,
 * which represents the cached result of stabGetSymes under various
 * conditions.
 *
 * The cache is set up to make common accesses quickly:
 * 	index 0 - condv == ablogTrue()
 *		The list of unconditional symes are always found here.
 *	index 1 - condv == ablogFalse()
 *		The list of all symes are found here, when conditional
 *		meanings exist for this symbol in this stab.
 *
 * So if no conditional symes are present, we go right to the list.
 * If we ask for the symes present when no conditions are assumed, ditto.
 * If we repeatedly ask for symes under the same condition, ditto.
 * Otherwise we filter through the list of all symes (at index 1).
 */

#define	StabEntryCacheSize 4
local void stabEntryCheckConditions(StabEntry stent);

local StabEntry
stabEntryNew(void)
{
	StabEntry	stent;
	Length		argc, cc, i;

	argc = StabEntryCacheSize;
	cc = sizeof(AbLogic *) + sizeof(SymeList *) + sizeof(TPoss *);
	cc = sizeof(*stent) + argc * cc;

	stent = (StabEntry) stoAlloc((unsigned) OB_Other, cc);

	stent->argc	= 0;
	stent->condv	= (AbLogic *)  (stent + 1);
	stent->symev	= (SymeList *) (stent->condv + argc);
	stent->possv	= (TPoss *)    (stent->symev + argc);
	stent->pending  = listNil(Syme);

	for (i = 0; i < argc; i += 1) {
		stent->condv[i] = NULL;
		stent->symev[i] = listNil(Syme);
		stent->possv[i] = NULL;
	}

	stabEntryAddCache(stent, ablogTrue(), listNil(Syme));

	return stent;
}

local StabEntry
stabEntryCopy(StabEntry stent)
{
	StabEntry	nstent = stabEntryNew();
	Length		i;

	nstent->argc = stent->argc;
	if (nstent->argc > 2) nstent->argc = 2;

	for (i = 0; i < nstent->argc; i += 1) {
		nstent->condv[i] = ablogCopy(stent->condv[i]);
		nstent->symev[i] = listCopy(Syme)(stent->symev[i]);
		nstent->possv[i] = tpossCopy(stent->possv[i]);
	}
	nstent->pending = listCopy(Syme)(stent->pending);

	return nstent;
}

local void
stabEntryClearCache(StabEntry stent)
{
	Length		i;

	tpossFree(stent->possv[0]);
	stent->possv[0] = NULL;

	if (stabEntryIsGeneric(stent))
		return;

	tpossFree(stent->possv[1]);
	stent->possv[1] = NULL;

	for (i = 2; i < stent->argc; i += 1) {
		ablogFree(stent->condv[i]);
		listFree(Syme)(stent->symev[i]);
		tpossFree(stent->possv[i]);

		stent->condv[i] = NULL;
		stent->symev[i] = listNil(Syme);
		stent->possv[i] = NULL;
	}
	stent->argc = 2;
}

local void
stabEntryAddCache(StabEntry stent, AbLogic abl, SymeList symes)
{
	assert(stent->argc < StabEntryCacheSize);
	stent->condv[stent->argc] = abl;
	stent->symev[stent->argc] = symes;
	stent->possv[stent->argc] = NULL;
	stent->argc += 1;
}

local void
stabEntryPutSyme(StabEntry stent, Length i, Syme syme)
{
	stent->symev[i] = listCons(Syme)(syme, stent->symev[i]);
}

local void
stabEntryAddSyme(StabEntry stent, Syme syme)
{
	stabEntryClearCache(stent);

	if (symeIsCheckCondIncomplete(syme)) {
		stent->pending = listCons(Syme)(syme, stent->pending);
		stabDEBUG(dbOut, "Pending condition: %pSyme %pAbSynList\n",
			  syme, symeCondition(syme));
	}
	if (!symeIsCondChecked(syme) && symeCondition(syme)) {
		stent->pending = listCons(Syme)(syme, stent->pending);
		stabDEBUG(dbOut, "Pending condition [unchecked]: %pSyme %pAbSynList\n",
			  syme, symeCondition(syme));
	}
	if (symeCondIsLazy(syme)) {
		stabEntryPutSyme(stent, int0, syme);
		if (!stabEntryIsGeneric(stent))
			stabEntryPutSyme(stent, 1, syme);
	}
	else if (symeCondition(syme)) {
		if (stabEntryIsGeneric(stent)) {
			SymeList osymes = stent->symev[0];
			stabEntryAddCache(stent, ablogFalse(), osymes);
		}
		stabEntryPutSyme(stent, 1, syme);
	}
	else {
		stabEntryPutSyme(stent, int0, syme);
		if (!stabEntryIsGeneric(stent))
			stabEntryPutSyme(stent, 1, syme);
	}
}

local Bool
stabEntryIsGeneric(StabEntry stent)
{
	/* Return true if all symes in stent are unconditional. */
	return stent->argc == 1;
}

local SymeList
stabEntryAllSymes(StabEntry stent)
{
	return stabEntryGetSymes(stent, ablogFalse());
}

local SymeList
stabEntryGetSymes(StabEntry stent, AbLogic abl)
{
	Length		i;
	SymeList	symes;
	SymeList        psymes, npsymes;

	stabEntryCheckConditions(stent);

	/* Generic entry:  no conditional symes, return them all. */
	if (stent->argc == 1)
		return stent->symev[0];

	/* Generic query:  return the unconditional symes. */
	if (abl == NULL || ablogIsTrue(abl))
		return stent->symev[0];

	/* Check the cache. */
	for (i = 1; i < stent->argc; i += 1)
		if (ablogEqual(abl, stent->condv[i]))
			return stent->symev[i];

	/* Filter the symes. */
	symes = stabEntryCacheSymes(stent, abl);

	/* Cache the result. */
	if (stent->argc < StabEntryCacheSize)
		stabEntryAddCache(stent, ablogCopy(abl), listCopy(Syme)(symes));

	return symes;
}

local void
stabEntryCheckConditions(StabEntry stent)
{
	SymeList psymes, npsymes;

	psymes = stent->pending;
	npsymes = listNil(Syme);
	while (psymes != listNil(Syme)) {
		Syme psyme = car(psymes);
		symeCheckCondition(psyme);

		stabDEBUG(dbOut, "Checked: %pSyme - complete: %d condition: %pAbSynList\n",
			  psyme, symeIsCheckCondIncomplete(psyme),
			  symeCondition(psyme));

		if (symeCondition(psyme) == listNil(Sefo)) {
			stabEntryPutSyme(stent, int0, psyme);
		}
		if (symeIsCheckCondIncomplete(psyme)) {
			npsymes = listCons(Syme)(psyme, npsymes);
		}
		psymes = cdr(psymes);
	}
	listFree(Syme)(stent->pending);
	stent->pending = npsymes;
}


local SymeList
stabEntryCacheSymes(StabEntry stent, AbLogic abl)
{
	SymeList	symes, nsymes;

	/* When conditional symes are present, all symes are at index 1. */
	nsymes = listNil(Syme);
	for (symes = stent->symev[1]; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		if (ablogIsListImplied(abl, symeCondition(syme)))
			nsymes = listCons(Syme)(syme, nsymes);
	}

	nsymes = listNReverse(Syme)(nsymes);
	return nsymes;
}

local TPoss
stabEntryAllTypes(StabEntry stent)
{
	return stabEntryGetTypes(stent, ablogFalse());
}

local TPoss
stabEntryGetTypes(StabEntry stent, AbLogic abl)
{
	Length		i;
	SymeList	symes;
	TPoss		tposs;

	stabEntryCheckConditions(stent);
/*LDR*/
#if EDIT_1_0_n2_06 != 1
	/* Generic entry:  no conditional symes, return all types. */
	if (stent->argc == 1)
		return stabEntryCacheTypes(stent, int0);
#endif

	/* Generic query:  return the unconditional types. */
	if (abl == NULL || ablogIsTrue(abl))
		return stabEntryCacheTypes(stent, int0);

	/* Check the cache. */
	for (i = 1; i < stent->argc; i += 1)
		if (ablogEqual(abl, stent->condv[i]))
			return stabEntryCacheTypes(stent, i);

	/* Filter the symes. */
	symes = stabEntryCacheSymes(stent, abl);
	tposs = tpossFrSymes(symes);

	/* Cache the result. */
	if (stent->argc < StabEntryCacheSize) {
		stabEntryAddCache(stent, ablogCopy(abl), symes);
		stent->possv[i] = tpossRefer(tposs);
	}

	return tposs;
}

local TPoss
stabEntryCacheTypes(StabEntry stent, Length i)
{
	assert(i < stent->argc);
	if (stent->possv[i] == NULL)
		stent->possv[i] = tpossFrSymes(stent->symev[i]);
	return tpossRefer(stent->possv[i]);
}

/****************************************************************************
 *
 * Local structure manipulation.
 *
 ****************************************************************************/

local StabLevel
stabNewLevel(int levno, int lamno, SrcPos spos, Bool isLargeLevel)
{
	StabLevel	slev;

	slev = (StabLevel) stoAlloc((unsigned) OB_Stab, sizeof(*slev));

	slev->lexicalLevel	= levno;
	slev->lambdaLevel	= lamno;
	slev->serialNo		= stabSerialNoCounter++;
	slev->hash		= slev->serialNo;
	slev->isLocked		= false;
	slev->isChecked		= false;
	slev->isSubstable	= true;
	slev->intStepNo		= intStepNo;
	slev->tbl		= tblNew((TblHashFun) 0, (TblEqFun) 0);
	slev->children		= listNil(Stab);
	slev->spos		= spos;
	slev->idsInScope	= 0;
	slev->labelsInScope	= listNil(AbSyn);

	slev->tformsUsed.list	= listNil(TFormUses);
	slev->tformsUnused	= listNil(TForm);

	/*
	 * large levels have a non-0 hash table for faster lookup of tforms.
	 */
	slev->tformsUsed.table	= isLargeLevel ?
		tblNew((TblHashFun) abHash, (TblEqFun) abEqual) : NULL;

	slev->boundSymes	= listNil(Syme);
	slev->extendSymes	= listNil(Syme);

	return slev;
}

/****************************************************************************
 *
 * Exported operations.
 *
 ****************************************************************************/

Stab
stabPushLevel(Stab stab, SrcPos spos, ULong flags)
{
	StabLevel slev;
	Stab	  oldStab = stab;
	int	  levno, lamno;

	Bool	  isLargeLevel	= (flags & STAB_LEVEL_LARGE)  ? 1 : 0;
	Bool	  isLoopLevel	= (flags & STAB_LEVEL_LOOP)   ? 1 : 0;
	Bool	  isWhereLevel	= (flags & STAB_LEVEL_WHERE)  ? 1 : 0;
	Bool	  isProgLevel	= !isLoopLevel && !isWhereLevel;

	assert(stab != 0);

	levno = car(stab)->lexicalLevel + (isLoopLevel ? 0 : 1);
	lamno = car(stab)->lambdaLevel	+ (isProgLevel ? 1 : 0);

	slev  = stabNewLevel(levno, lamno, spos, isLargeLevel);
	stab  = listCons(StabLevel) (slev, stab);
	car(oldStab)->children =
		listCons(Stab) (stab, car(oldStab)->children);

	return stab;
}

Stab
stabPopLevel(Stab stab)
{
	return cdr(stab);
}

void
stabFree(Stab stab)
{
	/*!!*/
}

/******************************************************************************
 *
 * :: Symbol lookup
 *
 *****************************************************************************/

/*
 * Get entry for "id" in "stab". If "id" has never been searched at this
 * level, then the meanings from the outer level are added at this level.
 */

local StabEntry
stabGetEntry(Stab stab0, Symbol id, Bool recurse)
{
	Stab	stab;
	StabEntry stent;
	Bool	first = true;

	stabDEBUG(dbOut, "Searching for symbol %s", symString(id));

	stab = stab0;
	stent = 0;
	while (stab && !stent) {
		stabDEBUG(dbOut,
			  (first ? " looking in level %lu" : ", %lu"),
			  stabLevelNo(stab));
		first = false;
		stent = (StabEntry) tblElt(car(stab)->tbl, id, NULL);
		if (! recurse)
			break;
		if (! stent)
			stab = cdr(stab);
	}

	if (!stent) {
		stent = stabEntryNew();
		tblSetElt(car(stab0)->tbl, id, stent);
		stabDEBUG(dbOut, " ... manufacturing");
	}
	else if (stab != stab0) {
		stent = stabEntryCopy(stent);
		tblSetElt(car(stab0)->tbl, id, stent);
		stabDEBUG(dbOut, " ... copying");
	}

/*LDR*/
#if 1 && EDIT_1_0_n2_06
	if (DEBUG(stab)) {
		fnewline(dbOut);
	}
	stabEntryGetTypes(stent, ablogFalse());
#endif
	if (DEBUG(stab)) {
		SymeList sl = stabEntryAllSymes(stent);
		TPoss tp = stabEntryAllTypes(stent);
		int i;

		if (sl) {
			findent += 2;
			fnewline(dbOut);
			fprintf(dbOut, "Meanings found: ");
			findent++;
			fnewline(dbOut);
			for (i = 1; sl; i++, sl = cdr(sl)) {
				fprintf(dbOut,"%2d. ",i);
				symePrint(dbOut, car(sl));
				if (cdr(sl))
					fnewline(dbOut);
			}
			findent -= 3;
		}

		if (tp && tpossCount(tp)) {
			TPossIterator	tit;

			findent += 2;
			fnewline(dbOut);
			fprintf(dbOut, "Types found:");
			findent++;
			fnewline(dbOut);

			for (i = 1, tpossITER(tit, tp);
			     tpossMORE(tit);
			     i++, tpossSTEP(tit))
			{
				fprintf(dbOut,"%2d. ",i);
				tfPrint(dbOut, tpossELT(tit));
				if (tpossMORE(tit))
					fnewline(dbOut);
			}
			findent -= 3;
		}
		fnewline(dbOut);
	}

	return stent;
}

/*
 * For each constant symbol here, add meanings from outer levels.
 */

void
stabSeeOuterImports(Stab stab0)
{
	TableIterator	it;

	for (tblITER(it, car(stab0)->tbl); tblMORE(it); tblSTEP(it)) {
		Symbol		symb = (Symbol) tblKEY(it);
		StabEntry	stent0 = (StabEntry) tblELT(it);
		SymeList	symes0 = stabEntryAllSymes(stent0);
		Stab		stab;
		StabEntry	stent = NULL;
		SymeList	symes;

		/* Consider only symbols with constant meanings. */
		if (symes0 == NULL)
			continue;

		if (symeIsParam(car(symes0)) ||
		    symeIsLexVar(car(symes0)) ||
		    symeIsExtend(car(symes0)))
			continue;

		/* Establish there is something to add. */
		for (stab = cdr(stab0); !stent && stab; stab = cdr(stab))
			stent = (StabEntry) tblElt(car(stab)->tbl, symb, NULL);
		if (stent == NULL)
			continue;

		/* Get outer meanings that are not covered. */
		/* Outer imports are covered by local exports. */
		for (symes = stabEntryAllSymes(stent); symes; symes = cdr(symes)) {
			Syme	syme = car(symes);
			Bool	covered;

			if (listMemq(Syme)(symes0, syme))
				covered = true;
			else if (symeIsExtend(syme))
				covered = stabExtendIsCovered(symes0, syme);
			else if (symeIsImport(syme))
				covered = stabImportIsCovered(symes0, syme,
							      stab0);
			else
				covered = false;

			if (!covered)
				stabEntryAddSyme(stent0, syme);
		}
	}
}

local Bool
stabImportIsCovered(SymeList symes, Syme syme, Stab stab)
{
	Syme		syme0;
	TForm		tf0;
	SymeList	aself;
	Syme		asyme;
	TForm		tfd;

	if (symeListHasImportee(symes, syme))
		return true;

	for (; symes; symes = cdr(symes)) {
		syme0 = car(symes);
		if (!symeIsExport(syme0)) continue;

		tf0 = symeType(syme0);
		if (tfEqual(symeType(syme), tf0))
			return true;

		if (!tfEqual(symeType(symeOriginal(syme)), tf0))
			continue;

		aself = tfGetSelfFrStab(stab);
		asyme = (aself ? car(aself) : NULL);
		tfd   = (asyme ? symeType(asyme) : NULL);
		if (tfd && tfIsDefineOfType(tfd) &&
		    tfEqual(tfDefineVal(tfd), symeExporter(syme)))
			return true;
	}
	return false;
}

local Bool
stabExtendIsCovered(SymeList symes, Syme syme)
{
	for (; symes; symes = cdr(symes))
		if (symeExtension(car(symes)) == syme)
			return true;
	return false;
}

void
stabGetSubstable(Stab stab)
{
	SymeList	symes;

	if (stab == stabGlobal()) return;

	stabLevelIsSubstable(stab) = stabLevelIsSubstable(cdr(stab));
	symes = stabGetBoundSymes(stab);
	for (; !stabLevelIsSubstable(stab) && symes; symes = cdr(symes))
		if (symeIsSubstable(car(symes)))
			stabSetSubstable(stab);
}

void
stabExtendMeanings(Stab stab, Syme syme)
{
	StabEntry	stent = stabGetEntry(stab, symeId(syme), true);
	Length		i;

	if (stent) {
		stabEntryClearCache(stent);
		for (i = 0; i < stent->argc; i += 1) {
			stent->symev[i] = symeListExtend(stent->symev[i], syme);
			
		}
	}

	car(stab)->boundSymes = symeListExtend(car(stab)->boundSymes, syme);
}

SymeList
symeListExtend(SymeList symes, Syme syme)
{
	SymeList	result;
	Syme		xsyme;

	if (symeIsExtend(syme))
		xsyme = syme;
	else if (symeIsImportOfExtend(syme))
		xsyme = symeOriginal(syme);
	else
		return symes;

	result = listNil(Syme);

#if 0	
{
	SymeList tmp = symes;

	printf("(Adding: %p %p %s\nCurrent: %s\nOrigin: %s\n", 
	       syme, xsyme, 
	       syme->id->str,
	       syme->lib && syme->lib->name ? syme->lib->name->partv[1]: "Local",
	       xsyme->lib->name->partv[1]);
	

	while (tmp) {
		printf("%p %s %s\n", car(tmp), 
		       car(tmp)->lib && car(tmp)->lib->name 
		       ? car(tmp)->lib->name->partv[1] : "Local",
		       car(tmp)->id->str);
		tmp = cdr(tmp);
	}
	printf("\n");
}
#endif
	for (; symes; symes = listFreeCons(Syme)(symes)) {
		/*if (symeOriginal(car(symes)) == xsyme)
			stabExtendPair(symeOriginal(car(symes)), xsyme);
		else*/ if (car(symes) != syme && symeExtension(car(symes)) != syme) 
			result = listCons(Syme)(car(symes), result);
		else {
#if 0
			printf("Deleted: %p %s %s\n",
			       car(symes), 
			       car(symes)->lib && car(symes)->lib->name 
			       ? car(symes)->lib->name->partv[1] : "Local",
			       car(symes)->id->str);
#endif
		}	
	}
	result = listCons(Syme)(syme, listNReverse(Syme)(result));

	return result;
}

/* Is syme1 an extendee of syme2? */
local Bool
symeHasExtendee(Syme syme1, Syme syme2)
{
	SymeList	symes;
	Bool		result = false;

	assert(symeIsExtend(syme2));

	symes = symeExtendee(syme2);
	for (; !result && symes; symes = cdr(symes)) {
		Syme	ext0 = car(symes);
		Syme	ext1 = symeExtension(ext0);
		symeSetExtension(ext0, NULL);
		result = symeEqual(symeOriginal(syme1), ext0);
		symeSetExtension(ext0, ext1);
	}

	return result;
}

/* Is there an import for syme2 in symes? */
local Syme
symeListHasImportee(SymeList symes, Syme syme2)
{
	Syme	result = NULL;

	for (; !result && symes; symes = cdr(symes)) {
		Syme	syme1 = car(symes);
		if (symeIsImport(syme1) && symeIsImport(syme2) &&
		    symeOriginal(syme1) == symeOriginal(syme2) &&
		    tformEqual(symeExporter(syme1), symeExporter(syme2)) &&
		    tformEqual(symeType(syme1), symeType(syme2)) &&
		    sefoListEqualMod(NULL, symeCondition(syme1),
				     symeCondition(syme2)))
			result = syme1;
	}
	return result;
}

/* Is there an extension of syme1 in symes? */
Syme
symeListHasExtension(SymeList symes, Syme syme1)
{
	Syme	result = NULL;

	for (; !result && symes; symes = cdr(symes)) {
		Syme	syme2 = car(symes);
		if (symeIsImportOfExtend(syme2) &&
		    symeHasExtendee(syme1, symeOriginal(syme2)))
			result = syme2;
	}
	return result;
}

/* Is there an extendee of syme2 in symes? */
Syme
symeListHasExtendee(SymeList symes, Syme syme2)
{
	Syme	result = NULL;

	for (; !result && symes; symes = cdr(symes)) {
		Syme	syme1 = car(symes);
		if (symeIsImportOfExtend(syme2) &&
		    symeHasExtendee(syme1, symeOriginal(syme2)))
			result = syme1;
	}

	return result;
}

local void
symeRefreshExtendees(Syme syme)
{
	SymeList	symes;

	assert(symeIsImportOfExtend(syme));
	syme = symeOriginal(syme);

	symes = symeExtendee(syme);
	for (; symes; symes = cdr(symes)) {
		Syme	ext = car(symes);
		if (symeExtension(ext) == NULL)
			symeSetExtension(ext, syme);
	}
}

SymeList
stabGetMeanings(Stab stab, AbLogic abl, Symbol sym)
{
	StabEntry stent = stabGetEntry(stab, sym, true);
	return stent ? stabEntryGetSymes(stent, abl) : listNil(Syme);
}

TPoss
stabGetTypes(Stab stab, AbLogic abl, Symbol sym)
{
	StabEntry stent = stabGetEntry(stab, sym, true);
	return stent ? stabEntryGetTypes(stent, abl) : tpossEmpty();
}

/*
 * Get the symbol meaning for %, or return NULL for failure.
 */
Syme
stabGetSelf(Stab stab)
{
	StabEntry	stent;
	SymeList	symes;

	if (stab == NULL) return NULL;

	stent = stabGetEntry(stab, ssymSelf, false);
	if (stent == NULL) return NULL;

	for (symes = stabEntryAllSymes(stent); symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		if ((symeIsLexVar(syme) || symeIsExtend(syme)) &&
		    symeDefLevel(syme) == car(stab))
			return syme;
	}

	return NULL;
}

/*
 * Get an export symbol meaning for id whose type is equal to tf modulo mods.
 */
Syme
stabGetExportMod(Stab stab, SymeList mods, Symbol id, TForm tf)
{
	StabEntry	stent;
	SymeList	symes;
	
	if (stab == NULL) return NULL;

	stent = stabGetEntry(stab, id, true);
	if (stent == NULL) return NULL;

	for (symes = stabEntryAllSymes(stent); symes; symes = cdr(symes)) {
		Syme	syme = car(symes);

		if (!symeIsExport(syme)) continue;

		/*!! This should eventually be replaced by something which
		 *!! knows how to compare the public parts of the types.
		 */
		if (tfIsCategory(tf) && tfSatCat(symeType(syme)))
			return syme;

		if (tformEqualMod(mods, tf, symeType(syme)))
			return syme;
	}

	return NULL;
}

/*
 *
 */
Bool
stabGetLex(Stab stab, Symbol sym)
{
	return	stabGetSymeOfKind(stab, sym, SYME_LexVar)   != NULL ||
		stabGetSymeOfKind(stab, sym, SYME_LexConst) != NULL;
}

/*
 * Get the library symbol meaning for the given identifier.
 */
Syme
stabGetLibrary(Symbol sym)
{
	return stabGetSymeOfKind(stabFile(), sym, SYME_Library);
}

/*
 * Get the archive symbol meaning for the given identifier.
 */
Syme
stabGetArchive(Symbol sym)
{
	return stabGetSymeOfKind(stabFile(), sym, SYME_Archive);
}

/*
 * Get the first syme we find for the sym that has one of the given tags.
 */
local Syme
stabGetSymeOfKind(Stab stab, Symbol sym, SymeTag tag)
{
	Syme	syme = NULL;

	for (; !syme && stab; stab = cdr(stab))
		syme = stabLevelGetSymeOfKind(stab, sym, tag);

	return syme;
}

local Syme
stabLevelGetSymeOfKind(Stab stab, Symbol sym, SymeTag tag)
{
	StabEntry	stent = (StabEntry) tblElt(car(stab)->tbl, sym, NULL);
	SymeList	symes = stent ? stabEntryAllSymes(stent) : listNil(Syme);

	for (; symes; symes = cdr(symes))
		if (symeKind(car(symes)) == tag)
			return car(symes);
	return NULL;
}

/******************************************************************************
 *
 * :: Symbol meaning creation
 *
 *****************************************************************************/

void
stabUseMeaning(Stab stab, Syme syme)
{
	UShort	d = stabLevelNo(stab) - symeDefLevelNo(syme);

	if (DEBUG(stab)) {
		fprintf(dbOut, "Using %s (.%ld) of depth %d at %d",
			symeString(syme), symeConstNum(syme), 
			symeUsedDepth(syme), d);
		fnewline(dbOut);
	}

	if (symeUnused(syme) || symeUsedDepth(syme) < d)
		symeSetUsedDepth(syme, d);
}

Bool
stabHasMeaning(Stab stab, Syme syme)
{
	StabEntry	stent;
	SymeList	symes;
	Bool		result = false;

	for (; !result && stab; stab = cdr(stab)) {
		stent = (StabEntry) tblElt(car(stab)->tbl, symeId(syme), NULL);
		symes = stent ? stabEntryAllSymes(stent) : listNil(Syme);
		result = listMemq(Syme)(symes, syme);
	}

	return result;
}

Syme
stabAddMeaning(Stab stab, Syme syme)
{
	Symbol		id;
	StabEntry	stent;

	assert(stab != 0);

	if (stabLevelIsLocked(stab))
		return stabAddMeaning(cdr(stab), syme);

	if (stabHasMeaning(stab, syme))
		return syme;

	id = symeId(syme);

	switch (symeKind(syme)) {
	case SYME_LexVar:
	case SYME_Param:
	case SYME_Extend:
		stent = stabGetEntry(stab, id, false);
		break;
	default:
		stent = stabGetEntry(stab, id, true);
		break;
	}

	if (symeIsImportOfExtend(syme))
		symeRefreshExtendees(syme);

	if (stent) {
		SymeList osymes = stabEntryAllSymes(stent);
		Syme	osyme;

		osyme = symeListHasImportee(osymes, syme);
		if (osyme)
			return osyme;

		osyme = symeListHasExtendee(osymes, syme);
		if (osyme) {
			symeSetExtension(osyme, syme);
			stabExtendMeanings(stab, syme);
			return syme;
		}

		osyme = symeListHasExtension(osymes, syme);
		if (osyme)
			return osyme;
	}

	car(stab)->boundSymes = listCons(Syme)(syme, car(stab)->boundSymes);
	stabDEBUG(dbOut, "Adding stab entry %d %pSyme %pAbSynList\n", car(stab)->lexicalLevel, 
		  syme, symeCondition(syme));
	stabEntryAddSyme(stent, syme);

	return syme;
}

void
stabPutMeanings(Stab stab, SymeList symes)
{
	SymeList	l;

	for (l = symes; l; l = cdr(l)) {
		Syme	syme = car(l);

		stabAddMeaning(stab, syme);
		symeSetDefLevel(syme, car(stab));
	}
}

Syme
stabDefParam(Stab stab, Symbol id, TForm tform)
{
	Syme	syme = symeNewParam(id, tform, car(stab));

	syme = stabAddMeaning(stab, syme);
	return syme;
}

Syme
stabDefLexConst(Stab stab, Symbol id, TForm tform)
{
	Syme	syme = symeNewLexConst(id, tform, car(stab));
	symeSetDefnNum(syme, (int) ++stabDefinitionCounter);

	if (DEBUG(stabConst)) {
		fprintf(dbOut, "defnNum[%d]:  ", symeDefnNum(syme));
		symePrint(dbOut, syme);
		fnewline(dbOut);
	}

	syme = stabAddMeaning(stab, syme);
	return syme;
}

Syme
stabDefLexVar(Stab stab, Symbol id, TForm tform)
{
	Syme	syme = symeNewLexVar(id, tform, car(stab));

	syme = stabAddMeaning(stab, syme);
	symeSetDefnNum(syme, (int) ++stabDefinitionCounter);

	if (DEBUG(stabConst)) {
		fprintf(dbOut, "defnNum[%d]:  ", symeDefnNum(syme));
		symePrint(dbOut, syme);
		fnewline(dbOut);
	}

	return syme;
}

Syme
stabDefFluid(Stab stab, Symbol id, TForm tform)
{
	Syme	syme = symeNewFluid(id, tform, car(stab));

	syme = stabAddMeaning(stab, syme);

	return syme;
}

Syme
stabDefImport(Stab stab, Symbol id, TForm tform, TForm exporter)
{
	Syme	syme = symeNewImport(id, tform, car(stab), exporter);

	return stabAddMeaning(stab, syme);
}

Syme
stabDefExport(Stab stab, Symbol id, TForm tform, Doc doc)
{
	Syme	syme = symeNewExport(id, tform, car(stab));

	if (DEBUG(stab)) {
		fprintf(dbOut, "Defining export %s with comment ",
			symString(id));
		docPrint(dbOut, doc);
		fnewline(dbOut);
	}

	symeSetComment(syme, doc);
	syme = stabAddMeaning(stab, syme);
	symeSetDefnNum(syme, (int) ++stabDefinitionCounter);

	if (DEBUG(stabConst)) {
		fprintf(dbOut, "defnNum[%d]:  ", symeDefnNum(syme));
		symePrint(dbOut, syme);
		fnewline(dbOut);
	}

	return syme;
}

Syme
stabDefExtendee(Stab stab, Symbol id, TForm tform, Doc doc)
{
	Syme	syme = symeNewExport(id, tform, car(stab));

	symeSetComment(syme, doc);
	symeSetDefnNum(syme, (int) ++stabDefinitionCounter);

	return syme;
}

Syme
stabDefExtend(Stab stab, Symbol id, TForm tform)
{
	Syme	syme = symeNewExtend(id, tform, car(stab));

	return stabAddMeaning(stab, syme);
}

Syme
stabDefLibrary(Stab stab, Symbol id, TForm tform, Lib lib)
{
	SymeList	symes;
	Syme		syme;

	/* Check for symbol meanings that already exist. */
	symes = stabGetMeanings(stab, NULL, id);
	if (symes) {
		comsgWarning(NULL, ALDOR_W_LibRedefined, symString(id));
		return car(symes);
	}

	syme = symeNewLibrary(id, tform, car(stab), lib);
	tfFrSyme(stab, syme);

	return stabAddMeaning(stab, syme);
}

Syme
stabDefArchive(Stab stab, Symbol id, TForm tform, Archive ar)
{
	SymeList	symes;
	Syme		syme;

	/* Check for symbol meanings that already exist. */
	symes = stabGetMeanings(stab, NULL, id);
	if (symes) {
		comsgWarning(NULL, ALDOR_W_LibRedefined, symString(id));
		return car(symes);
	}

	syme = symeNewArchive(id, tform, car(stab), ar);
	tfFrSyme(stab, syme);

	return stabAddMeaning(stab, syme);
}

Syme
stabDefForeign(Stab stab, Symbol id, TForm tform, ForeignOrigin forg)
{
	Syme	syme = symeNewForeign(id, tform, car(stab), forg);

	return stabAddMeaning(stab, syme);
}

Syme
stabDefBuiltin(Stab stab, Symbol id, TForm tform, FoamBValTag builtin)
{
	Syme	syme = symeNewBuiltin(id, tform, car(stab), builtin);

	return stabAddMeaning(stab, syme);
}

/******************************************************************************
 *
 * :: Shadow Stuff
 *
 *****************************************************************************/

static AbSyn	StabUseMeaningShadow = 0;

void
stabUseMeaningShadow(AbSyn ab)
{
	StabUseMeaningShadow = ab;
}

void
stabUseMeaningUnshadow(void)
{
	StabUseMeaningShadow = 0;
}

void
stabSetSyme(Stab stab, AbSyn ab, Syme syme, AbLogic cond)
{
	SImpl impl;
	/*!! if (abUse(ab) != AB_Use_Declaration &&
		 abUse(ab) != AB_Use_Define) */
	if (ab != StabUseMeaningShadow)
		stabUseMeaning(stab, syme);
	abSetSyme(ab, syme);
	
	if (abDefineIdx(ab) != -1)
		symeImplAddConst(syme, cond, abDefineIdx(ab));

	/* NB: We assume that the impl information is correct.
	 * It ought to be, at least for add bodies, 'cos it is 
	 * set in tiAddSymes.
	 */
	impl = implEvaluate(symeImpl(syme), abCondKnown);
	abSetImpl(ab, impl);
}

Bool
stabIsUndeclaredId(Stab stab, Symbol sym)
{
	Length		symec = 0;
	SymeList	symes;

	for (symes = stabGetMeanings(stab, NULL, sym); symes; symes = cdr(symes))
		if (tfIsUnknown(symeType(car(symes))))
			symec += 1;

	return symec == 1;
}

void
stabDeclareId(Stab stab, Symbol sym, TForm tform)
{
	Syme		syme = NULL;
	SymeList	symes;

	for (symes = stabGetMeanings(stab, NULL, sym); symes; symes = cdr(symes))
		if (tfIsUnknown(symeType(car(symes))))
			syme = car(symes);

	assert(syme);
	symeSetType(syme, tform);
	stabPropagateDecl(stab, syme);
}

/* Once the type of syme has been inferred,
 * clear the stab entry caches for each level in which syme is visible.
 * Ex: for x in 1..10
 *	     j := 1     -- declared here
 *     j := 2		-- update this stent
 */
local void
stabPropagateDecl(Stab stab, Syme syme)
{
	StabEntry	stent;
	Symbol 		sym = symeId(syme);

	for (; stab; stab = cdr(stab)) {
		stent = (StabEntry) tblElt(car(stab)->tbl, sym, NULL);

		if (stent && listMemq(Syme)(stabEntryAllSymes(stent), syme))
			stabEntryClearCache(stent);

		/* Stop when the definition level has been reached. */
		if (car(stab) == symeDefLevel(syme)) break;
	}
}
/******************************************************************************
 *
 * :: Symbol table printing.
 *
 *****************************************************************************/

int
stabPrint(FILE * fout, Stab stab)
{
	return stabPrintTo(fout, stab, (int) 0);
}

/*
 * Print "stab" level by level until "minlev" is reached. If "minlev" is
 * negative, then the count is made from the top of the stack instead of the
 * bottom.
 */

int
stabPrintTo(FILE * fout, Stab stab, int minlev)
{
	int	cc, ts;
	StabLevel slev;

	if (stab && minlev < 0)
		minlev = car(stab)->lexicalLevel + minlev + 1;

	for (cc = 0; stab; stab = cdr(stab)) {
		slev = car(stab);
		if (slev->lexicalLevel < minlev)
			break;
		cc += fnewline(fout);
		cc += fprintf(fout, "SymbolTable Level %ld, %s, (serial %ld),",
			slev->lexicalLevel,
			stabLevelIsLarge(stab) ? "LARGE" : "SMALL",
			slev->serialNo);
		cc += fprintf(fout, "  %d symbols.", ts = tblSize(slev->tbl));
		findent += 2;
		cc += fnewline(fout);
		if (ts)
			cc += tblColumnPrint(fout, slev->tbl,
					     (TblPrKeyFun) 0, stabPrEntry);
		findent -= 2;
		cc += fnewline(fout);

		if (slev->tformsUnused) {
			TFormList tfl;

			cc += fprintf(fout,
				"Type forms registered but not used: ");
			findent += 2;
			cc += fnewline(fout);
			for (tfl = slev->tformsUnused; tfl; tfl = cdr(tfl)) {
				cc += tfPrint(fout, car(tfl));
				cc += fnewline(fout);
			}
			findent -= 2;
			cc += fnewline(fout);
		}

		cc += fprintf(fout, "All type forms used: ");
		findent += 2;
		cc += fnewline(fout);

		if (!slev->tformsUsed.list)
			cc += fprintf(fout, "NONE");
		else
			cc += tfulPrint(fout, slev->tformsUsed.list);

		findent -= 2;
		cc += fnewline(fout);

		if (slev->labelsInScope) {
			AbSynList asl = slev->labelsInScope;
			cc += fprintf(fout, "Labels in this scope: ");
			while (asl) {
			    cc += abPrettyPrint(fout, car(asl));
			    asl = cdr(asl);
			    cc += asl ? fprintf(fout, ", ") : fnewline(fout);
			}
			cc += fnewline(fout);
		}

		if (slev->boundSymes) {
			SymeList sl =  slev->boundSymes;
			cc += fprintf(fout, "Bound symbol meanings: ");
			findent += 2;
			cc += fnewline(fout);
			while (sl) {
			    cc += symePrint(fout, car(sl));
			    sl = cdr(sl);
			    if (sl)
				cc += fnewline(fout);
			}
			findent -= 2;
		}

		cc += fnewline(fout);
	}
	return cc;
}

local int
stabPrEntry(FILE * fout, TblElt e)
{
	StabEntry stent = (StabEntry) e;
	SymeList symes = stabEntryAllSymes(stent);
	int	cc = 0;

	for (; symes; symes = cdr(symes)) {
		cc += symePrint(fout, car(symes));
		if (cdr(symes))
			cc += fnewline(fout);
	}
	return cc;
}

int
tfulPrint(FILE *fout, TFormUsesList tful)
{
	int	cc = 0;
	while (tful) {
		TFormUses tfu = car(tful);
		tful = cdr(tful);
		cc += tfuPrint(fout, tfu);
	}
	return cc;
}

int
tfuPrint(FILE *fout, TFormUses tfu)
{
	int	cc = 0;
	
	cc += tfPrint(fout, tfu->tf);
	findent += 2;
	cc += fnewline(fout);
	
	if (tfu->isExplicitImport) {
		cc += fprintf(fout, "Explicitly imported.");
		cc += fnewline(fout);
	}
	if (tfu->isCategoryImport) {
		cc += fprintf(fout, "Categorically imported.");
		cc += fnewline(fout);
	}
	if (tfu->isParamImport) {
		cc += fprintf(fout, "Parameter import.");
		cc += fnewline(fout);
	}

	if (tfu->exports) {
		cc += fprintf(fout, "exports:  ");
		cc += tqPrint(fout, tfu->exports);
		cc += fnewline(fout);
	}
	if (tfu->imports) {
		cc += fprintf(fout, "imports:  ");
		cc += tqPrint(fout, tfu->imports);
		cc += fnewline(fout);
	}
	if (tfu->inlines) {
		cc += fprintf(fout, "inlines:  ");
		cc += tqPrint(fout, tfu->inlines);
		cc += fnewline(fout);
	}

	if (tfQueries(tfu->tf)) {
		TFormList cl;
		int	  i;

		cc += fnewline(fout);
		cc += fprintf(fout, "Conditional categories:");
		cl  = tfQueries(tfu->tf);
		findent += 2;
		cc += fnewline(fout);
		for (i = 1; cl; cl = cdr(cl), i++) {
			cc += fprintf(fout, "%d: ", i);
			cc += tformPrint(fout, car(cl));
			cc += fnewline(fout);
		}
		findent -= 2;
	}
	
	if (tfu->declarees) {
		cc += fnewline(fout);
		cc += fprintf(fout, "Declarees: ");
		listPrint(Symbol)(fout,tfu->declarees,symPrint);
	}
	
	findent -= 2;
	cc += fnewline(fout);
	
	return cc;
}

/******************************************************************************
 *
 * :: Imports
 *
 *****************************************************************************/

SymeList
stabGetExportedSymes(Stab stab)
{
	SymeList symes, exports = listNil(Syme);

	for (symes = stabGetBoundSymes(stab); symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		if (symeIsExport(syme) || symeIsExtend(syme))
			exports = listCons(Syme)(car(symes), exports);
	}

	return exports;
}

TQualList
stabImportFrom(Stab stab, TQual tq)
{
	TQualList	ql;
	SymeList	dsymes;
	TForm		origin = tqBase(tq);


	/* Carefully follow this tform */
	tfFollow(origin);


	/*
	 * We can significantly improve inlining if we replace
	 * the domain with its normal form. Users can turn this
	 * off by compiling with -Wdumb-import.
	 */
	if (!stabDumbImport())
	{
		TForm base = tfDefineeBaseType(origin);

		if (tfQueries(origin) != listNil(TForm))
			(void)tfCopyQueries(base, origin);
		origin = base;
	}


	/* Don't import a tform if has already been imported */
	if (stabIsImportedTForm(stab, origin))
		return listNil(TQual);

	if (DEBUG(stabImport)) {
		fprintf(dbOut, "Importing %s from ",
			tqIsForeign(tq) ? " foreign exports" :
			tqIsBuiltin(tq) ? " builtin exports" :
			tqIsQualified(tq) ? " explicit exports" : "");
		tfPrint(dbOut, origin);
		fnewline(dbOut);
	}

	stabImportRemark(stab, tqQual(tq), origin);

	if (tqIsForeign(tq))
		dsymes = tqGetForeignImports(stab, tq);
	else if (tqIsBuiltin(tq))
		dsymes = tqGetBuiltinImports(stab, tq);
	else if (tqIsQualified(tq))
		dsymes = tqGetQualImports(tq);
	else {
		Stab nstab = stab;

		/*
		 * We want to avoid using stabFile() otherwise some
		 * tforms will seep into the top level and cause us
		 * grief later on (e.g. bug1192). However, there are
		 * times when we really do want to use stabFile(). If
		 * we have sefo that isn't associated with any source
		 * code then stabFile() ought to be okay.
		 *
		 * An alternative to using the local stab passed down
		 * to us, we be to use tfStab() if it was present. That
		 * may be more reliable if it corresponds to a higher
		 * lexical level.
		 */
		if (!tfGetExpr(origin) || !abPos(tfGetExpr(origin)))
			nstab = stabFile();


		/* Get the domain imports of origin */
		dsymes = tfStabGetDomImports(nstab, origin);


		/*
		 * It doesn't matter which stab we chose to get domain
		 * imports from, we are importing into the local one.
		 */
		if (dsymes) stabMakeImportedTForm(stab, origin);
	}

	stabPutMeanings(stab, dsymes);

	stabImportDEBUG(dbOut, "... imported: %pSymeCList\n", dsymes);

	if (!tqIsQualified(tq))
		return tfGetDomCascades(origin);
	else
		return listNil(TQual);
}

local void
stabImportRemark(Stab stab, TFormList what, TForm origin)
{
	if (comsgOkRemark(ALDOR_R_StabImporting)) {
		String s = tfPretty(origin);
		comsgRemark(abNewNothing(car(stab)->spos), ALDOR_R_StabImporting, s);
		strFree(s);
	}

	if (what && comsgOkRemark(ALDOR_R_StabImportingQual)) {
		Buffer		buf = bufNew();
		String		explicits;
		TFormList	tmpList;

		for (tmpList = what; tmpList; tmpList = cdr(tmpList)) {
			String pp = abPretty(tfExpr(car(tmpList)));
			BUF_ADD1(buf, '\n');
			BUF_ADD1(buf, '\t');
			bufPuts(buf, pp);
			strFree(pp);
		}

		explicits = bufLiberate(buf);
		comsgRemark(abNewNothing(car(stab)->spos),
			    ALDOR_R_StabImportingQual, explicits);

		strFree(explicits);
	}
}

/****************************************************************************
 *
 * TForm and TFormUses code.
 *
 ****************************************************************************/

/*
 * General scheme is this: TFormUses holds TForms and their usage
 * information.	 If a TForm is "registered" via stabDefTForm but not
 * used explicitly yet for importing or inlining, say, then it is stored on
 * the list tformsUnused.  A TForm is taken off that list when it is
 * added to tformsUsed.
 */

local TFormUses
tformDoNothing(TFormUses tfu)
{
	/*
	 * Used to return an old or newly created TFormUses object.
	 */
	return tfu;
}

local TFormUses
tformIsImported(TFormUses tfu)
{
	tfu->isImported = true;
	return tfu;
}

local TFormUses
tformExportAll(TFormUses tfu)
{
	tfuSetUnqualified(tfu->exports, tfu->tf);
	return tfu;
}

local TFormUses
tformImportAll(TFormUses tfu)
{
	tfuSetUnqualified(tfu->imports, tfu->tf);
	return tfu;
}

local TFormUses
tformInlineAll(TFormUses tfu)
{
	tfuSetUnqualified(tfu->inlines, tfu->tf);
	return tfu;
}

local TFormUses
tformExplicitlyImportAll(TFormUses tfu)
{
	tfu->isExplicitImport = true;
	return tformImportAll(tfu);
}

local TFormUses
tformCategoricallyImport(TFormUses tfu)
{
	tfu->isCategoryImport = true;
	return tfu;
}

local TFormUses
tformCatConditionImport(TFormUses tfu)
{
	tfu->isCatConditionImport = true;
	return tfu;
}

local TFormUses
tformParameterImport(TFormUses tfu)
{
	tfu->isParamImport = true;
	return tfu;
}

local TFormUses
tfuNew(TForm tf)
{
	TFormUses tu = (TFormUses) stoAlloc(OB_Other, sizeof(*tu));

	tu->isImported		= false;
	tu->isExplicitImport	= false;
	tu->isCategoryImport	= false;
	tu->isCatConditionImport= false;
	tu->isParamImport	= false;
	tu->tf			= tf;
	tu->exports		= NULL;
	tu->imports		= NULL;
	tu->inlines		= NULL;
	tu->cascades		= listNil(TQual);
	tu->extension		= listNil(AbSyn);
	tu->extendees		= listNil(AbSyn);
	tu->declarees		= listNil(Symbol);
	tu->dependents		= listNil(TFormUses);
	tu->dependees		= listNil(TFormUses);
	tu->nbefore		= 0;
	tu->nafter		= 0;
	tu->cdependents		= listNil(TFormUses);
	tu->cdependees		= listNil(TFormUses);
	tu->ncbefore		= 0;
	tu->ncafter		= 0;
	tu->sortMark		= false;
	tu->outEdges		= listNil(TFormUses);
	tu->inDegree		= 0;
	tu->cmarked		= false;
	tu->crep		= NULL;

	return tu;
}

TFormUses
stabFindTFormUses(Stab stab, AbSyn ab)
{
	TFormUses	tfu = NULL;
	TFormUsesList	tful;

	assert(stab != 0);
	assert(ab != 0);

	if (stabUseList(stab, ab)) {
		tful = car(stab)->tformsUsed.list;
		for (; tful && !tfu; tful = cdr(tful))
			if (abEqual(ab, tfGetExpr(car(tful)->tf)))
				tfu = car(tful);
	}
	else if (stabUseTable(stab, ab))
		tfu = (TFormUses) tblElt(car(stab)->tformsUsed.table,
			(TblKey) ab, (TblElt) NULL);

	return tfu;
}

/*
 * Find or create a TFormUses and then call fun on it
 */
local TFormUses
tfuSetFlag(Stab stab, TForm tf, UpdateTfuFun fun)
{
	AbSyn		ab = tfExpr(tf);
	TFormUses	tfu = stabFindTFormUses(stab, ab);

	if (!tfu) {
		/* first remove tf from tformsUnused, if present */
		removeTFormUnused(stab, tf);

		/* now make a new one */
		tfu = tfuNew(tf);

		if (stabUseTable(stab, ab))
			tblSetElt(car(stab)->tformsUsed.table,
				  (TblKey) ab, (TblElt) tfu);
		listPush(TFormUses, tfu, car(stab)->tformsUsed.list);
	}

	return fun(tfu);
}

local TForm
findTFormUnused(Stab stab, AbSyn ab)
{
	TFormList tfl;

	for (tfl = car(stab)->tformsUnused; tfl; tfl = cdr(tfl))
		if (abEqual(ab, tfGetExpr(car(tfl))))
			return car(tfl);

	return 0;
}

local TForm
addTFormUnused(Stab stab, TForm tf)
{
	car(stab)->tformsUnused =
		listCons(TForm) (tf, car(stab)->tformsUnused);
	return tf;
}

local void
removeTFormUnused(Stab stab, TForm tf)
{
	car(stab)->tformsUnused =
		listNRemove(TForm) (car(stab)->tformsUnused, tf, tfEqual);
}

TForm
stabMakeUsedTForm(Stab stab, AbSyn ab, TfCondElt conditions)
{
	TFormUses	tfu;
	TForm		tf;

	tfu = stabFindTFormUses(stab, ab);
	if (tfu == NULL) {
		tf = tfSyntaxFrAbSyn(stab, ab);
		tfu = tfuSetFlag(stab, tf, tformDoNothing);
	}
	else {
		tf = tfu->tf;
	}

	tfSyntaxConditions(stab, tf, conditions);
	abSetTForm(ab, tf);
	return tf;
}

TForm
stabMakeImportedTForm(Stab stab, TForm tf)
{
	return tfuSetFlag(stab, tf, tformIsImported)->tf;
}

TForm
stabExportTForm(Stab stab, TForm tf)
{
	return tfuSetFlag(stab, tf, tformExportAll)->tf;
}

TForm
stabImportTForm(Stab stab, TForm tf)
{
	return tfuSetFlag(stab, tf, tformImportAll)->tf;
}

TForm
stabInlineTForm(Stab stab, TForm tf)
{
	return tfuSetFlag(stab, tf, tformInlineAll)->tf;
}

TForm
stabExplicitlyImportTForm(Stab stab, TForm tf)
{
	return tfuSetFlag(stab, tf, tformExplicitlyImportAll)->tf;
}

TForm
stabCategoricallyImportTForm(Stab stab, TForm tf)
{
	return tfuSetFlag(stab, tf, tformCategoricallyImport)->tf;
}

TForm
stabParameterImportTForm(Stab stab, TForm tf)
{
	return tfuSetFlag(stab, tf, tformParameterImport)->tf;
}

TForm
stabQualifiedExportTForm(Stab stab, AbSyn ab, TForm tf)
{
	TFormUses tfu = tfuSetFlag(stab, tf, tformDoNothing);

	tfuSetQualified(tfu->exports, tfu->tf, tfSyntaxFrAbSyn(stab, ab));

	return tf;
}

TForm
stabQualifiedImportTForm(Stab stab, AbSyn ab, TForm tf)
{
	TFormUses tfu = tfuSetFlag(stab, tf, tformDoNothing);

	tfuSetQualified(tfu->imports, tfu->tf, tfSyntaxFrAbSyn(stab, ab));
	tfu->isExplicitImport = true;

	return tf;
}

TForm
stabQualifiedInlineTForm(Stab stab, AbSyn ab, TForm tf)
{
	TFormUses tfu = tfuSetFlag(stab, tf, tformDoNothing);

	tfuSetQualified(tfu->inlines, tfu->tf, tfSyntaxFrAbSyn(stab, ab));

	return tf;
}

/*
 * Add an id declared to be an extension with the type tf.
 */
TForm
stabAddTFormExtension(Stab stab, TForm tf, AbSyn extension)
{
	TFormUses	tfu;
	int		i, declc = 0;
	AbSyn		*declv;

	tfu = tfuSetFlag(stab, tf, tformDoNothing);

	switch (abTag(extension)) {
	case AB_Nothing:
		declc = 0;
		declv = 0;
		break;
	case AB_Sequence:
	case AB_Comma:
		declc = abArgc(extension);
		declv = abArgv(extension);
		break;
	default:
		declc = 1;
		declv = &extension;
		break;
	}

	for (i = 0; i < declc; i += 1)
		tfu->extension = listCons(AbSyn)(declv[i], tfu->extension);

	return tf;
}

/*
 * Add an id declared to be an extendee with the type tf.
 */
TForm
stabAddTFormExtendees(Stab stab, TForm tf, AbSyn extendees)
{
	TFormUses	tfu;
	int		i, declc = 0;
	AbSyn		*declv;

	tfu = tfuSetFlag(stab, tf, tformDoNothing);

	switch (abTag(extendees)) {
	case AB_Nothing:
		declc = 0;
		declv = 0;
		break;
	case AB_Sequence:
	case AB_Comma:
		declc = abArgc(extendees);
		declv = abArgv(extendees);
		break;
	default:
		declc = 1;
		declv = &extendees;
		break;
	}

	for (i = 0; i < declc; i += 1)
		tfu->extendees = listCons(AbSyn)(declv[i], tfu->extendees);

	return tf;
}

/*
 * Record the fact that the question "dom has cat" has been asked.
 */
TForm
stabAddTFormQuery(Stab stab, TForm dom, TForm cat)
{
	tfFollow(dom);
	tfAddQuery(dom, cat);
	if (tfIsSyntax(dom) && abIsTheId(tfGetExpr(dom), ssymSelf)) {
		TFormUses tfu = stabFindTFormUses(stab, tfExpr(cat));
		tformCatConditionImport(tfu);
	}
	return dom;
}

/*
 * Add an id declared to have type tf if not already in the list.
 * Handles sequences or commas of ids.
 */
TForm
stabAddTFormDeclaree(Stab stab, TForm tf, AbSyn declarees)
{
	TFormUses	tfu;
	int		i, declc = 0;
	AbSyn		*declv;
	Bool		wasEmpty;

	tfu = tfuSetFlag(stab, tf, tformDoNothing);

	switch (abTag(declarees)) {
	case AB_Nothing:
		declc = 0;
		declv = 0;
		break;
	case AB_Sequence:
	case AB_Comma:
		declc = abArgc(declarees);
		declv = abArgv(declarees);
		break;
	default:
		declc = 1;
		declv = &declarees;
		break;
	}

	wasEmpty = tfu->declarees ? false : true;

	for (i = 0; i < declc; i += 1) {
		Symbol s = declv[i]->abId.sym;
		if (wasEmpty || ! listMemq(Symbol)(tfu->declarees, s))
			tfu->declarees = listCons(Symbol)(s, tfu->declarees);
	}

	return tfu->tf;
}


TForm
stabGetTForm(Stab stab, AbSyn ab, TForm failed)
{
	TFormUses tfu;
	TForm	tf;

	assert(stab != 0);

	if (stabLevelIsLocked(stab))
		return stabGetTForm(cdr(stab), ab, failed);

	tf = findTFormUnused(stab, ab);
	if (tf)
		return tf;

	tfu = stabFindTFormUses(stab, ab);
	if (tfu)
		return tfu->tf;

	return failed;
}

TForm
stabDefTForm(Stab stab, TForm tf)
{
	/* !! presume NOT already present */
	/* !! (this is presumptious) */
	if (stabLevelIsLocked(stab))
		stabDefTForm(cdr(stab), tf);

#if 0
	/* Someone turn this into a proper -Wd debug hook ... */
	if (tfGetExpr(tf) && abIsTheId(tfGetExpr(tf), ssymSelf))
	{
		Stab	fil = stabFile();
		SrcPosStack sposStk = tfGetExpr(tf)->abHdr.pos;

		(void)fprintf(dbOut, "defTForm: <%3d: %d/%d> (%3d: %d/%d)\n",
			stabSerialNo(stab),
			stabLevelNo(stab),
			stabLambdaLevelNo(stab),
			stabSerialNo(fil),
			stabLevelNo(fil),
			stabLambdaLevelNo(fil));
		if (sposStk.stack != NULL)
		{
			SrcPos pos = spstackFirst(sposStk);
			spstackPrintLine(dbOut, pos);
		}
		fnewline(dbOut);
	}
#endif

	return addTFormUnused(stab, tf);
}

Bool
stabIsImportedTForm(Stab stab, TForm tf)
{
	/*
	 * This function walks up the stab stack to see if tf has already been
	 * imported.
	 */

	/* Sanity check/hack */
	if (!tfHasSelf(tf))
	{
		/* 
	 	 * Something bad has happened to the tf but we re-import
		 * it anyway. See tfCopyQueries for one example of "Bad".
	 	 * Currently, we don't reset it anywhere else.
	 	 */
		/* (void)fprintf(dbOut, "(((ooops)))\n"); */
		return false;
	}

	for (; stab; stab = cdr(stab)) {
		TFormUses tfu = stabFindTFormUses(stab, tfExpr(tf));

		if (tfu && tfu->isImported) {
			if (tfIsMeaning(tfu->tf) && tfIsMeaning(tf) &&
			    !tformEqual(tfu->tf, tf))
				continue;
			return true;
		}
	}
	return false;
}

TForm
stabFindOuterTForm(Stab stab, AbSyn ab)
{
	assert(stab);

	for (; stab; stab = cdr(stab)) {
		TForm	    tf = findTFormUnused(stab, ab);

		if (! tf) {
			TFormUses tfu = stabFindTFormUses(stab, ab);
			if (tfu)
				tf = tfu->tf;
		}
		if (tf) {
			tfFollow(tf);
			return tf;
		}
	}
	return NULL;
}


/*
 *  getAllTypesUsed recurses from sl down and gets all types imported,
 *  inlined, or otherwise used.	 The first two list may overlap but the
 *  third will not overlap either of the other two.
 */

local void getAllTypesUsed0(StabLevel sl, struct typesUsed *tu);

struct typesUsed *
getAllTypesUsed(StabLevel sl)
{
	struct typesUsed *tu = (struct typesUsed *)
		stoAlloc((unsigned) OB_Other, sizeof(struct typesUsed));

	tu->typesImported = tu->typesInlined = tu->typesOther = 0;

	getAllTypesUsed0(sl, tu);

	/*
	 * do a final check to make sure 'other' types are not imported
	 * or inlined.
	 */

	if (tu->typesOther) {
		AbSynList al = tu->typesOther, reallyUnused = 0;

		while (al) {
			int pos;
			AbSyn ab = car(al);

			al = cdr(al);
			listFind(AbSyn)(tu->typesImported,ab,abEqual,&pos);
			if (pos != -1)
				continue;
			listFind(AbSyn)(tu->typesInlined,ab,abEqual,&pos);
			if (pos == -1)
				reallyUnused = listCons(AbSyn)(ab,reallyUnused);
		}

		if (reallyUnused) {
			listFree(AbSyn)(tu->typesOther);
			tu->typesOther = reallyUnused;
		}
	}


	return tu;
}

local void
getAllTypesUsed0(StabLevel sl, struct typesUsed *tu)
{
	StabList	children    = sl->children;
	TFormUsesList	tfuses	    = sl->tformsUsed.list;
	TFormList	tfunused    = sl->tformsUnused;

	/* first do children and then me */

	while (children) {
		getAllTypesUsed0(car(car(children)), tu);
		children = cdr(children);
	}

	/* iterate across the used tforms */

	while (tfuses) {
		Bool used = false;
		TFormUses tfu = car(tfuses);
		AbSyn ab;
		int pos;

		tfuses = cdr(tfuses);
		if (tfIsWith(tfu->tf))
			continue;
		ab = tfGetExpr(tfu->tf);
		if (abHasTag(ab, AB_Apply))
			ab = ab->abApply.op;
		if (abHasTag(ab, AB_With))
			continue;

		if (tfu->imports) {
			used = true;
			listFind(AbSyn)(tu->typesImported,ab,abEqual,&pos);
			if (pos == -1)
				tu->typesImported =
					listCons(AbSyn)(ab,tu->typesImported);
		}

		if (tfu->inlines) {
			used = true;
			listFind(AbSyn)(tu->typesInlined,ab,abEqual,&pos);
			if (pos == -1)
				tu->typesInlined =
					listCons(AbSyn)(ab,tu->typesInlined);
		}

		if (used == false) {
			listFind(AbSyn)(tu->typesOther,ab,abEqual,&pos);
			if (pos == -1)
				tu->typesOther =
					listCons(AbSyn)(ab,tu->typesOther);
		}
	}

	/* iterate across the unused tforms */

	while (tfunused) {
		TForm tf = car(tfunused);
		AbSyn ab;
		int pos;

		tfunused = cdr(tfunused);

		if (tfIsWith(tf))
			continue;
		ab = tfGetExpr(tf);
		if (abHasTag(ab, AB_Apply))
			ab = ab->abApply.op;
		if (abHasTag(ab, AB_With))
			continue;

		listFind(AbSyn)(tu->typesImported,ab,abEqual,&pos);
		if (pos != -1)
			continue;

		listFind(AbSyn)(tu->typesInlined,ab,abEqual,&pos);
		if (pos != -1)
			continue;

		listFind(AbSyn)(tu->typesOther,ab,abEqual,&pos);
		if (pos == -1)
			tu->typesOther = listCons(AbSyn)(ab,tu->typesOther);
	}
}


/*
 * stabAddLabel(stab, label): Add label to the list of labels in this scope.
 * It is an error if the label is already present.
 */
void
stabAddLabel(Stab stab, AbSyn label)
{
	Symbol	labsym = label->abId.sym;
	if (stabLabelExistsInThisStab(stab, labsym)) {
		AbSyn abLab = stabGetLabelInThisStab(stab, labsym);
		comsgNError(label, ALDOR_E_StabDupLabels, symString(labsym));
		comsgNote(abLab, ALDOR_N_Here);
	}
	else {
		Syme syme = symeNewLabel(labsym, tfNone(), car(stab));
		abSetSyme(label, syme);
		car(stab)->labelsInScope =
			listCons(AbSyn)(label, car(stab)->labelsInScope);
	}
}

/*
 * stabGetAllLabels(stab): return a list of all labels in this or outer scopes.
 */
AbSynList
stabGetAllLabels(Stab stab)
{
	Stab stab0;
	AbSynList allLabels = listNil(AbSyn);
	for (stab0 = stab; stab0; stab0 = cdr(stab0))
		allLabels = listConcat(AbSyn)(car(stab)->labelsInScope,
			allLabels);
	return allLabels;
}

/*
 * stabLabelExists(stab, label): returns true if the label is present in
 *	this or any outer scope, false otherwise.
 */
Bool
stabLabelExists(Stab stab, Symbol label)
{
	Stab stab0;
	for (stab0 = stab; stab0; stab0 = cdr(stab0))
		if (stabLabelExistsInThisStab(stab0, label))
			return true;
	return false;
}

/*
 * stabLabelExistsInThisStab(stab, label): returns true if the label is
 *	present in this symbol table level, false otherwise.
 */
Bool
stabLabelExistsInThisStab(Stab stab, Symbol label)
{
	AbSynList asl = car(stab)->labelsInScope;
	while (asl) {
		if (label == car(asl)->abId.sym)
			return true;
		asl = cdr(asl);
	}
	return false;
}

/*
 * stabGetLabels(stab, label): returns all labels (absyns) with the given
 *	name in this or any outer symbol table level.
 */
AbSynList
stabGetLabels(Stab stab, Symbol label)
{
	AbSynList labels = listNil(AbSyn);
	Stab stab0;

	for (stab0 = stab; stab0; stab0 = cdr(stab0)) {
		AbSyn foundLabel = stabGetLabelInThisStab(stab0, label);
		if (foundLabel)
			labels = listCons(AbSyn)(foundLabel, labels);
	}
	return listNReverse(AbSyn)(labels);
}

/*
 * stabGetLabelInThisStab(stab, label): returns the unique label in this
 *	symbol table level with the given name, if it exists. If it is
 *	note present, 0 is returned.
 */
AbSyn
stabGetLabelInThisStab(Stab stab, Symbol label)
{
	AbSynList asl = car(stab)->labelsInScope;
	while (asl) {
		if (label == car(asl)->abId.sym)
			return car(asl);
		asl = cdr(asl);
	}
	return 0;
}

/* If an inner stab level can be found which binds syme, return it. */
Stab
stabFindLevel(Stab stab, Syme syme)
{
	StabList	sl;

	if (stabLevelNo(stab) >= symeDefLevelNo(syme))
		return stab;

	for (sl = car(stab)->children; sl; sl = cdr(sl)) {
		Stab	istab = car(sl), nstab;

		if (car(istab) == symeDefLevel(syme))
			return istab;

		nstab = stabFindLevel(istab, syme);
		if (nstab != istab)
			return nstab;
	}

	return stab;
}


Bool stabIsChild(Stab parent, Stab child)
{
	Bool isChild = false;

	while (child != listNil(StabLevel)) {
		if (child == parent)
			return true;
		child = cdr(child);
	}

	return isChild;
}
