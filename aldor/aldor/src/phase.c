/*****************************************************************************
 *
 * phase.c: Phase statististics and reporting.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "include.h"
#include "opsys.h"
#include "phase.h"
#include "store.h"
#include "util.h"
#include "lib.h"
#include "strops.h"
#include "symbol.h"


/*****************************************************************************
 *
 * :: Forward Declarations
 *
 ****************************************************************************/

local void phPrintTime   	(Millisec);
local void phPrintPercent	(Length n, Length tot);
local void phPrintPhPercentages	(Millisec,Length,Length,Length,struct phInfo*);
local void phPrintSourceSummary (Length, Millisec);
local void phPrintLibStats	(LibStats);
local void phPrintStoreSummary  (Length,Length,Length,Length);


/*****************************************************************************
 *
 * :: Phase Info Table
 *
 ****************************************************************************/

# define PH_NAME_MAXLEN	 20	/* Maximum length of a phase name   */

# define PHX_Announce	(1<<1)	/* a: Announce entry to phase	    */
# define PHX_Debug	(1<<2)	/* d: Give any available debug info */
# define PHX_GcAfter	(1<<3)	/* g: Garbage collect after phase   */
# define PHX_Print	(1<<4)	/* p: (pretty) Print result of phase*/
# define PHX_Report	(1<<5)	/* r: Display result of phase	    */
# define PHX_StoAudit	(1<<6)	/* s: Store audit after phase	    */
# define PHX_Terminates (1<<7)	/* t: Quit after this phase	    */

struct phInfo phInfo[] = {
	/* phno,	abbrev name	  flags time alloc free gc */
	{PH_Load,	"ld", "load",	  0,	0,   0,	   0,	0},
	{PH_Include,	"in", "include",  0,	0,   0,	   0,	0},
	{PH_Scan,	"sc", "scan",	  0,	0,   0,	   0,	0},
	{PH_SysCmd,     "sy", "syscmd",   0,    0,   0,    0,   0},
	{PH_Linear,	"li", "linear",	  0,	0,   0,	   0,	0},
	{PH_Parse,	"pa", "parse",	  0,	0,   0,	   0,	0},
	{PH_MacEx,	"ma", "macex",	  0,	0,   0,	   0,	0},
	{PH_AbNorm,	"ab", "abnorm",	  0,	0,   0,	   0,	0},
	{PH_AbCheck,	"ck", "abcheck",  0,	0,   0,	   0,	0},
	{PH_ScoBind,	"sb", "scobind",  0,	0,   0,	   0,	0},
	{PH_TInfer,	"ti", "tinfer",	  0,	0,   0,	   0,	0},
	{PH_GenFoam,	"gf", "genfoam",  0,	0,   0,	   0,	0},
	{PH_OptFoam,	"of", "optfoam",  0,	0,   0,	   0,	0},
	{PH_PutIntermed,"pb", "putao",	  0,	0,   0,	   0,	0},
	{PH_PutLisp,	"pl", "putlisp",  0,	0,   0,	   0,	0},
	{PH_PutC,	"pc", "putc",	  0,	0,   0,	   0,	0},
	{PH_PutObject,	"po", "putobject",0,	0,   0,	   0,	0},
	{PH_Misc,	"mi", "misc",	  0,	0,   0,	   0,	0}
};

struct phInfo *phCurrent = 0;

/*****************************************************************************
 *
 * :: Option Setup
 *
 ****************************************************************************/

#define PH_FLAG_SEP	'+'	/* '+' in -WT adg+all */

int
phTraceOption(String flags)
{
	char	phName[PH_NAME_MAXLEN], doPh[PH_LIMIT];
	String	phases;
	int	i;

	/* Make "phases" point to string tail after separator. */
	for (phases = flags; *phases != 0; phases++)
		if (*phases == PH_FLAG_SEP) {
			phases++;
			break;
		}

	if (*flags == PH_FLAG_SEP || *phases == 0)
		return -1;

	if (strAEqual(phases, "all"))
		for (i = PH_START; i < PH_LIMIT; i++)
			doPh[i] = true;
	else {
		for (i = PH_START; i < PH_LIMIT; i++)
			doPh[i] = false;
		while (*phases) {
			phases = bite(phName, phases, '+');
			if ((i = phNumber(phName)) == PH_LIMIT) return -1;
			doPh[i] = true;
		}
	}

	for (i = PH_START; i < PH_LIMIT; i++) {
		String	f;
		if (!doPh[i])
			continue;
		for (f = flags; *f != 0 && *f != PH_FLAG_SEP; f++)
			switch (toupper(*f)) {
			case 'A':
				phInfo[i].flags |= PHX_Announce;
				break;
			case 'D':
				phInfo[i].flags |= PHX_Debug;
				break;
			case 'G':
				phInfo[i].flags |= PHX_GcAfter;
				break;
			case 'P':
				phInfo[i].flags |= PHX_Print;
				break;
			case 'R':
				phInfo[i].flags |= PHX_Report;
				break;
			case 'S':
				phInfo[i].flags |= PHX_StoAudit;
				break;
			case 'T':
				phInfo[i].flags |= PHX_Terminates;
				break;
			case '0':
				phInfo[i].flags  = 0;
				break;
			default:
				return -1;
			}
	}
	return 0;
}

PhTag
phNumber(String name)
{
	int	i;
	for (i = 0; i < PH_LIMIT; i++) {
		if (strAEqual(phInfo[i].name, name))
			return (PhTag) i;
		if (strAEqual(phInfo[i].abbrev, name))
			return (PhTag) i;
	}
	return PH_LIMIT;
}

/*****************************************************************************
 *
 * :: Recording and Summarizing
 *
 ****************************************************************************/

/*
 * {phStartAll {phStart .. phEnd}* phEndAll}* phGrandTotals
 */

/*
 * thisPhaseXxxx	Refers to the phase in progress.
 * allPhasesXxxx	Refers to the current phStartAll/phEndAll group.
 * grandPhasesXxxx	Refers to everything ever seen.
 */

static Bool		allPhasesVerbose;

static Millisec 	thisPhaseStartCPU,   allPhasesStartCPU;
static ULong		thisPhaseStartAlloc, allPhasesStartAlloc;
static ULong		thisPhaseStartFree,  allPhasesStartFree;
static ULong		thisPhaseStartGc,    allPhasesStartGc;

static struct libStats	thisLibStatsBuf,     grandLibStatsBuf;
static struct libStats	*thisLibStatsSeen,   *grandLibStatsSeen = 0;

static Length	grandPhasesLines;

void
phStartAll(Bool verboseFlag)
{
	int	i;

	for (i = 0; i < PH_LIMIT; i++) {
		phInfo[i].time = 0;
		phInfo[i].alloc = 0;
		phInfo[i].free = 0;
		phInfo[i].gc = 0;
	}
	allPhasesVerbose    = verboseFlag;
	allPhasesStartCPU   = osCpuTime();
	allPhasesStartAlloc = stoBytesAlloc;
	allPhasesStartFree  = stoBytesFree;
	allPhasesStartGc    = stoBytesGc;

	thisLibStatsSeen    = 0;
}

void
phStart(PhTag phno)
{
	thisPhaseStartCPU   = osCpuTime();
	thisPhaseStartAlloc = stoBytesAlloc;
	thisPhaseStartFree  = stoBytesFree;
	thisPhaseStartGc    = stoBytesGc;

	phCurrent = &(phInfo[phno]);

	DEBUG_MODE(phCurrent->flags & PHX_Debug);

	if (phCurrent->flags & (PHX_Announce|PHX_Print|PHX_Report|PHX_Debug))
		fprintf(osStdout, "*** Starting \"%s\" phase...\n", phCurrent->name);
}

void
phEnd(PhPrFun prf, PhPrFun pprf, Pointer ob)
{
	if (phCurrent->flags & PHX_GcAfter) {
		int	cd;

		if (phCurrent->flags & PHX_StoAudit) stoAudit();
		stoGc();
		if (phCurrent->flags & PHX_StoAudit) stoAudit();

		for (cd = 0; cd < OB_LIMIT; cd++) {
			if (stoPiecesGc[cd])
				fprintf(osStdout, "%ld * %s\n",
					stoPiecesGc[cd], obInfo[cd].str);
		}
		fprintf(osStdout, "\n");
	}
	else if (phCurrent->flags & PHX_StoAudit)
		stoAudit();

	phCurrent->time	 += osCpuTime()	  - thisPhaseStartCPU;
	phCurrent->alloc += stoBytesAlloc - thisPhaseStartAlloc;
	phCurrent->free	 += stoBytesFree  - thisPhaseStartFree;
	phCurrent->gc	 += stoBytesGc	  - thisPhaseStartGc;

	if ((phCurrent->flags & PHX_Announce) && allPhasesVerbose) {
		fprintf(osStdout, "Time  %5ld.%.3ld s\n",
			phCurrent->time / 1000, phCurrent->time % 1000);
		fprintf(osStdout, "Store  %8ld B ", stoBytesOwn);
#ifndef NDEBUG
		fprintf(osStdout, ": %ld B alloc - %ld B free - %ld gc = %ld\n",
			phCurrent->alloc,
			phCurrent->free,
			phCurrent->gc,
			stoBytesAlloc - stoBytesFree - stoBytesGc);
#else
		fprintf(osStdout, "\n");
#endif
	}

	if (phCurrent->flags & (PHX_Print | PHX_Report))
		fprintf(osStdout, "*** Result of %s:\n", phCurrent->name);

	if (phCurrent->flags & PHX_Print) {
		if (pprf) pprf(osStdout, ob);
		fprintf(osStdout, "\n\n");
	}
	if (phCurrent->flags & PHX_Report) {
		if (prf) prf(osStdout, ob);
		fprintf(osStdout, "\n\n");
	}
	if (phCurrent->flags & PHX_Terminates)
		exitSuccess();
}

void
phLibStats(FileName libfn)
{
	if (!grandLibStatsSeen) {
		grandLibStatsSeen = &grandLibStatsBuf;
		libStatsClear(grandLibStatsSeen);
	}
	if (!thisLibStatsSeen)
		thisLibStatsSeen = &thisLibStatsBuf;

	libStats(libfn, thisLibStatsSeen);
	libStatsIncrement(grandLibStatsSeen, thisLibStatsSeen);
}

void
phEndAll(void)
{
	Millisec	allCPU;
	Length		allAlloc, allFree, allGc;

	grandPhasesLines += inclTotalLineCount();

	if (!allPhasesVerbose) return;

	allCPU   = osCpuTime()   - allPhasesStartCPU;
	allAlloc = stoBytesAlloc - allPhasesStartAlloc;
	allFree  = stoBytesFree  - allPhasesStartFree;
	allGc    = stoBytesGc    - allPhasesStartGc;

	phPrintPhPercentages(allCPU, allAlloc, allFree, allGc, phInfo);
	fprintf(osStdout, "\n");
	phPrintSourceSummary(inclTotalLineCount(), allCPU);
	phPrintLibStats(thisLibStatsSeen);
	phPrintStoreSummary(stoBytesOwn, allAlloc, allFree, allGc);
}

void
phGrandTotals(Bool verboseFlag)
{
	Millisec ttot;

	if (!verboseFlag) return;

	ttot = osCpuTime();

	fprintf(osStdout, "\nTotals:\n");
	phPrintTime(ttot);
	phPrintSourceSummary(grandPhasesLines, osCpuTime());
	phPrintLibStats(grandLibStatsSeen);
	phPrintStoreSummary(stoBytesOwn,stoBytesAlloc,stoBytesFree,stoBytesGc);
}


/******************************************************************************
 *
 * :: Printing parts of file summary
 *
 *****************************************************************************/

/*
 * Convert Bytes to KiloBytes.
 */
#define BtoK(n) ((int) (((n) + 512)/1024))

/*
 * Conversion to whole and tenth seconds.  t/100 =  wholeSec(t) + tenthSec(t)
 */
#define wholeSec(ms)	((int) ( (ms) / 1000))
#define tenthSec(ms)	((int) (((ms) % 1000) / 100))

local void
phPrintTime(Millisec time)
{
	fprintf(osStdout, " Time%5d.%.1d s\n",
		wholeSec(time), tenthSec(time));
}

#define PerCent(n,d)	((int) (((n)* 100 + (d)/2)/(d)))
#define PerThou(n,d)	((int) (((n)*1000 + (d)/2)/(d)))

local void
phPrintPercent(Length xi, Length xtot)
{
	if (xi == 0 || xtot == 0)
		fprintf(osStdout, "  0");
	else if (PerCent(xi,xtot) >= 1)
		fprintf(osStdout, "%3d",    PerCent(xi,xtot));
	else
		fprintf(osStdout, " .%.1d", PerThou(xi,xtot));
}

local void
phPrintPhPercentages(
	Millisec allCPU,
        Length allAlloc, Length allFree, Length allGc,
	struct phInfo *phInfo)
{
	int	i;

	/* Compute ``miscellaneous'' column. */
	phInfo[PH_Misc].time  = allCPU;
	phInfo[PH_Misc].alloc = allAlloc;
	phInfo[PH_Misc].free  = allFree;
	phInfo[PH_Misc].gc    = allGc;

	for (i = 0; i < PH_LIMIT; i++) 
		if (i != PH_Misc) {
			phInfo[PH_Misc].time  -= phInfo[i].time;
			phInfo[PH_Misc].alloc -= phInfo[i].alloc;
			phInfo[PH_Misc].free  -= phInfo[i].free;
			phInfo[PH_Misc].gc    -= phInfo[i].gc;
		}

	/*** Titles ***/
	fprintf(osStdout, "              ");
	for (i = 0; i < PH_LIMIT; i++)
		fprintf(osStdout, " %s", phInfo[i].abbrev);
	fprintf(osStdout, "\n");

	/*** Time stats ***/
	fprintf(osStdout, " Time%5d.%.1d s",
		wholeSec(allCPU), tenthSec(allCPU));
	for (i = 0; i < PH_LIMIT; i++)
		phPrintPercent((Length)phInfo[i].time, (Length)allCPU);
	fprintf(osStdout, " %%\n");

	/*** Storage stats ***/
	if (allAlloc != 0) {
		fprintf(osStdout, " Alloc%6d K", BtoK(allAlloc));
		for (i = 0; i < PH_LIMIT; i++)
			phPrintPercent(phInfo[i].alloc, allAlloc);
		fprintf(osStdout, " %%\n");
	}
	if (allFree != 0) {
		fprintf(osStdout, " Free %6d K", BtoK(allFree));
		for (i = 0; i < PH_LIMIT; i++)
			phPrintPercent(phInfo[i].free, allFree);
		fprintf(osStdout, " %%\n");
	}
	if (allGc != 0) {
		fprintf(osStdout, " GC   %6d K", BtoK(allGc));
		for (i = 0; i < PH_LIMIT; i++)
			phPrintPercent(phInfo[i].gc, allGc);
		fprintf(osStdout, " %%\n");
	}
}

local void
phPrintLibStats(LibStats stats)
{
	int	i;

	if (stats) {
		fprintf(osStdout, " Lib%8d bytes, ", (int) stats->libSize);
		for (i = LIB_NAME_START; i < LIB_NAME_LIMIT; i++)
			if (stats->sectSize[i] != 0) {
				fprintf(osStdout, " %d%s",
					(int) stats->sectSize[i],
					libSectInfo(i).abbrev);
			}
		fprintf(osStdout, "\n");
	}
}

local void
phPrintSourceSummary(Length lines, Millisec time)
{
	if (lines != 0 && time != 0) {
		Length lpm = (lines * 60 * 1000) / time;
		fprintf(osStdout, " Source%5d lines,  %d lines per minute\n",
			(int) lines, (int) lpm);
	}
}

local void
phPrintStoreSummary(Length pool, Length alloc, Length free, Length gc)
{
	fprintf(osStdout, " Store%6d K pool", BtoK(pool));
	if (alloc != 0) {
		fprintf(osStdout,
			", %dK alloc - %dK free - %dK gc = %dK final",
			BtoK(alloc), BtoK(free), BtoK(gc),
			BtoK((signed long) (alloc - free - gc)));
	}
	fprintf(osStdout, "\n");
}


/******************************************************************************
 *
 * :: Saving and restoring symbol info
 *
 * !! This shouldn't be here.
 *
 *****************************************************************************/

CREATE_LIST(PhaseSymbolData);

static PhaseSymbolDataList psdl = 0;

local void
saveAndEmptyPhaseSymbolData(Symbol sym)
{
	if (symInfo(sym) && symCoInfo(sym) && symCoInfo(sym)->phaseVal.generic){
		PhaseSymbolData psd =
			(PhaseSymbolData) stoAlloc(OB_Other, sizeof(*psd));
		psd->sym  = sym;
		psd->data = symCoInfo(sym)->phaseVal.generic;
		psdl = listCons(PhaseSymbolData)(psd, psdl);
		symCoInfo(sym)->phaseVal.generic = 0;
	}
}

PhaseSymbolDataList
saveAndEmptyAllPhaseSymbolData(void)
{
	psdl = 0;
	symMap(saveAndEmptyPhaseSymbolData);
	return psdl;
}

void
restoreAllPhaseSymbolData(PhaseSymbolDataList psdl)
{
	while (psdl) {
		Symbol sym		= car(psdl)->sym;
		symCoInfo(sym)->phaseVal.generic = car(psdl)->data;
		psdl = listFreeCons(PhaseSymbolData)(psdl);
	}
}
