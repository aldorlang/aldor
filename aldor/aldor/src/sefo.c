/*****************************************************************************
 *
 * sefo.c: Semantic form maniuplation
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "genfoam.h"
#include "ostream.h"
#include "spesym.h"
#include "srcpos.h"
#include "stab.h"
#include "store.h"
#include "util.h"
#include "sefo.h"
#include "archive.h"
#include "lib.h"
#include "tqual.h"
#include "tfsat.h"
#include "freevar.h"
#include "absub.h"
#include "abpretty.h"
#include "strops.h"
#include "table.h"

Bool	sstDebug	= false;
Bool	sstMarkDebug	= false;

Bool	sefoPrintDebug	= false;
Bool	sefoEqualDebug	= false;
Bool	sefoFreeDebug	= false;
Bool	sefoSubstDebug	= false;
Bool	sefoUnionDebug	= false;
Bool	sefoInterDebug	= false;
Bool	sefoCloseDebug	= false;

#define sstDEBUG	DEBUG_IF(sst)		afprintf
#define sstMarkDEBUG	DEBUG_IF(sstMark)	afprintf

#define sefoPrintDEBUG	DEBUG_IF(sefoPrint)	afprintf
#define sefoEqualDEBUG	DEBUG_IF(sefoEqual)	afprintf
#define sefoFreeDEBUG	DEBUG_IF(sefoFree)	afprintf
#define sefoSubstDEBUG	DEBUG_IF(sefoSubst)	afprintf
#define sefoUnionDEBUG	DEBUG_IF(sefoUnion)	afprintf
#define sefoInterDEBUG	DEBUG_IF(sefoInter)	afprintf
#define sefoCloseDEBUG	DEBUG_IF(sefoClose)	afprintf

#define SefoSubstTUnique
#undef  SefoSubstShare
#define MarkScheme3

static ULong * SubstDebugTable = NULL;

void
substDebugInit(void)
{
	Length	i;
	SubstDebugTable = (ULong *) stoAlloc((unsigned) OB_Other,
					     (TF_LIMIT + 1) * sizeof(ULong));
	for (i = 0; i <= TF_LIMIT; i += 1)
		SubstDebugTable[i] = 0;
}

void
substDebugReport(void)
{
	Length	i;
	for (i = 0; i <= TF_LIMIT; i += 1)
		fprintf(dbOut, "\t%12s:\t\t%12ld\n",
			tformStr(i), SubstDebugTable[i]);
}

/*****************************************************************************
 *
 * :: Local function declarations.
 *
 ****************************************************************************/

extern void		genGetConstNums		(SymeList);
extern SymeList		tfGetDomConstants	(TForm);

local int		sstPrSefo 		(FILE *, int, Sefo);
local int		sstPrSyme 		(FILE *, int, Syme);
local int		sstPrTForm		(FILE *, int, TForm);
local int		sstPrAbSub		(FILE *, int, AbSub);
local int		sstPrSefoList 		(FILE *, int, SefoList);
local int		sstPrSymeList		(FILE *, int, SymeList);
local int		sstPrTFormList		(FILE *, int, TFormList);
local int		sstPrBool		(FILE *, int, Bool);
local int		sstPrLib		(FILE *, int, Lib);

local int		sefoOStreamPrint0	(OStream, Bool, Sefo);
local int		symeOStreamPrint0	(OStream, Bool, Syme);
local int		tformOStreamPrint0	(OStream, Bool, TForm);
local int		sefoListOStreamPrint0	(OStream, Bool, SefoList);
local int		symeListOStreamPrint0	(OStream, Bool, SymeList);
local int		tformListOStreamPrint0	(OStream, Bool, TFormList);

local Bool		sefoEqual0		(SymeList, Sefo, Sefo);
local Bool		symeEqual0		(SymeList, Syme, Syme);
local Bool		tformEqual0		(SymeList, TForm, TForm);
local Bool		sefoListEqual0		(SymeList, SefoList, SefoList);
local Bool		symeListEqual0		(SymeList, SymeList, SymeList);
local Bool		tformListEqual0		(SymeList,TFormList,TFormList);

local Bool		symeTypeEqual0		(SymeList, Syme, Syme);
local Bool		symeExtendEqual0	(SymeList, Syme, Syme);
local Bool		symeOriginEqual0	(SymeList, Syme, Syme);

local Bool		tformEqualCheckSymes	(TForm);
local Sefo		sefoEqualMods		(Sefo);

local void		sfvInitTable		(void);
local void		sfvFiniTable		(void);
local void		sfvPushTable		(void);
local void		sfvConsTable		(Syme);
local FreeVar		sfvPopTable		(Bool);

local void		sfvAddSyme		(Syme);
local void		sfvAddSymes		(SymeList);
local void		sfvDelSyme		(Syme);
local void		sfvDelSymes		(SymeList);
local void		sfvDelStab		(Stab);

local Bool		sfvIsAncestor		(TForm, TForm);
local TForm		sfvCommonAncestor	(TForm, TForm);

local void		sefoFreeVars0		(TForm *, TForm, Sefo);
local void		symeFreeVars0		(TForm *, TForm, Syme);
local void		tformFreeVars0		(TForm *, TForm, TForm);
local void		tqualFreeVars0		(TForm *, TForm, TQual);
local void		abSubFreeVars0		(TForm *, TForm, AbSub);
local void		sefoListFreeVars0	(TForm *, TForm, SefoList);
local void		symeListFreeVars0	(TForm *, TForm, SymeList);
local void		tformListFreeVars0	(TForm *, TForm, TFormList);
local void		tqualListFreeVars0	(TForm *, TForm, TQualList);

local void		sefoUnboundVars0	(TForm *, TForm, Sefo);
local void		symeUnboundVars0	(TForm *, TForm, Syme);
local void		tformUnboundVars0	(TForm *, TForm, TForm);
local void		tqualUnboundVars0	(TForm *, TForm, TQual);
local void		sefoListUnboundVars0	(TForm *, TForm, SefoList);
local void		symeListUnboundVars0	(TForm *, TForm, SymeList);
local void		tformListUnboundVars0	(TForm *, TForm, TFormList);
local void		tqualListUnboundVars0	(TForm *, TForm, TQualList);

local TQualList		tqualListSubst		(AbSub, TQualList);

local Sefo		sefoSubst0		(AbSub, Sefo);
local Syme		symeSubst0		(AbSub, Syme);
local TForm		tformSubst0		(AbSub, TForm);
local TQual		tqualSubst0		(AbSub, TQual);
local FreeVar		freeVarSubst0		(AbSub, FreeVar);
local SymeList		parentListSubst0	(AbSub, SymeList);
local SefoList		condListSubst0		(AbSub, SefoList);
local SymeList		symeListSubst0		(AbSub, SymeList);
local TFormList		tformListSubst0		(AbSub, TFormList);
local TQualList		tqualListSubst0		(AbSub, TQualList);

local void		symeListMarkTwins	(AbSub, Syme, Syme);
local void		symeListSubstUseConstants(AbSub, SymeList, SymeList,
						 SymeList);

local Bool		symeWillPush		(Syme);
local Bool		symeWillSubst		(AbSub, Syme);
local Bool		tformWillSubst		(AbSub, TForm);

local void		slcAddSyme		(Lib, Syme);
local void		slcAddType		(Lib, TForm);

local void		sefoClosure0		(Lib, Sefo);
local void		symeClosure0		(Lib, Syme);
local void		symeClosure1		(Lib, Syme);
local void		tformClosure0		(Lib, TForm);
local void		tqualClosure0		(Lib, TQual);

local void		sefoListClosure0	(Lib, SefoList);
local void		symeListClosure0	(Lib, SymeList);
local void		tformListClosure0	(Lib, TFormList);
local void		tqualListClosure0	(Lib, TQualList);

local int		freeVarToBuffer		(Lib, Buffer, FreeVar);

local int		tformToBuffer1		(Lib, Buffer, TForm);
local TForm		tformFrBuffer1		(Lib, Buffer);

local void		sefoFrBuffer0		(Buffer);
local void		symeFrBuffer0		(Buffer);
local void		tformFrBuffer0		(Buffer);
local void		tqualFrBuffer0		(Buffer);
local void              sefoListFrBuffer0       (Buffer);
local void		tformListFrBuffer0	(Buffer);
local void		tqualListFrBuffer0	(Buffer);

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal stack
 *
 ****************************************************************************/

union  sstArg {
	Pointer		ptr;
	Bool *		pbool;
	Sefo		sefo;
	SefoList	sefos;
	Syme		syme;
	SymeList	symes;
	TForm		tform;
	TFormList	tforms;
	AbSub		sigma;
	Lib		lib;
};

struct sstFrame {
	String		where;
	union sstArg	arg1;
	union sstArg	arg2;
	struct sstFrame	*prev;
};

static struct sstFrame	*sstStack = 0;

void
sstStackPush(String where, Pointer arg1, Pointer arg2)
{
	struct sstFrame	*frame;

	frame = (struct sstFrame *) stoAlloc(OB_Other, sizeof(*frame));
	frame->where    = where;
	frame->arg1.ptr = arg1;
	frame->arg2.ptr = arg2;
	frame->prev     = sstStack;
	sstStack        = frame;
}

void
sstStackPop(void)
{
	struct sstFrame	*frame;

	if (!sstStack)
		bug("sstStackPop: popping empty stack");
	
	frame = sstStack->prev;
	stoFree((Pointer) sstStack);
	sstStack = frame;
}

int
sstStackPrint(FILE *fout)
{
	int		n, cc = 0;
	struct sstFrame *frame;
	String		where;

	cc += fputcTimes('v', 70, fout);
	cc += fprintf(fout, "\n");

	/* Compute stack depth. */
	for (n = 0, frame = sstStack; frame; frame = frame->prev)
		n++;

	/* Display the stack. */
	for (frame = sstStack; frame; frame = frame->prev, n--) {
		where = frame->where;
		cc += fprintf(fout, "Level %d. %s: \n", n, where);

		/*
		 * Equal
		 */
		if (strEqual(where, "sefoEqual")) {
			cc += sstPrSefo(fout, 1, frame->arg1.sefo);
			cc += sstPrSefo(fout, 2, frame->arg2.sefo);
		}
		else if (strEqual(where, "symeIsTwin")) {
			cc += sstPrSyme(fout, 1, frame->arg1.syme);
			cc += sstPrSyme(fout, 2, frame->arg2.syme);
		}
		else if (strEqual(where, "symeEqual")) {
			cc += sstPrSyme(fout, 1, frame->arg1.syme);
			cc += sstPrSyme(fout, 2, frame->arg2.syme);
		}
		else if (strEqual(where, "tformEqual")) {
			cc += sstPrTForm(fout, 1, frame->arg1.tform);
			cc += sstPrTForm(fout, 2, frame->arg2.tform);
		}
		else if (strEqual(where, "sefoListEqual")) {
			cc += sstPrSefoList(fout, 1, frame->arg1.sefos);
			cc += sstPrSefoList(fout, 2, frame->arg2.sefos);
		}
		else if (strEqual(where, "symeListEqual")) {
			cc += sstPrSymeList(fout, 1, frame->arg1.symes);
			cc += sstPrSymeList(fout, 2, frame->arg2.symes);
		}
		else if (strEqual(where, "tformListEqual")) {
			cc += sstPrTFormList(fout, 1, frame->arg1.tforms);
			cc += sstPrTFormList(fout, 2, frame->arg2.tforms);
		}

		/*
		 * EqualMod
		 */
		else if (strEqual(where, "sefoEqualMod")) {
			cc += sstPrSefo(fout, 1, frame->arg1.sefo);
			cc += sstPrSefo(fout, 2, frame->arg2.sefo);
		}
		else if (strEqual(where, "symeEqualMod")) {
			cc += sstPrSyme(fout, 1, frame->arg1.syme);
			cc += sstPrSyme(fout, 2, frame->arg2.syme);
		}
		else if (strEqual(where, "tformEqualMod")) {
			cc += sstPrTForm(fout, 1, frame->arg1.tform);
			cc += sstPrTForm(fout, 2, frame->arg2.tform);
		}
		else if (strEqual(where, "sefoListEqualMod")) {
			cc += sstPrSefoList(fout, 1, frame->arg1.sefos);
			cc += sstPrSefoList(fout, 2, frame->arg2.sefos);
		}
		else if (strEqual(where, "symeListEqualMod")) {
			cc += sstPrSymeList(fout, 1, frame->arg1.symes);
			cc += sstPrSymeList(fout, 2, frame->arg2.symes);
		}
		else if (strEqual(where, "tformListEqualMod")) {
			cc += sstPrTFormList(fout, 1, frame->arg1.tforms);
			cc += sstPrTFormList(fout, 2, frame->arg2.tforms);
		}

		/*
		 * FreeVars
		 */
		else if (strEqual(where, "abSubFreeVars")) {
			cc += sstPrBool (fout, 1, *(frame->arg1.pbool));
			cc += sstPrAbSub(fout, 2, frame->arg2.sigma);
		}
		else if (strEqual(where, "tformFreeVars")) {
			cc += sstPrBool (fout, 1, *(frame->arg1.pbool));
			cc += sstPrTForm(fout, 2, frame->arg2.tform);
		}

		/*
		 * ListClosure
		 */
		else if (strEqual(where, "symeListClosure")) {
			cc += sstPrLib     (fout, 1, frame->arg1.lib);
			cc += sstPrSymeList(fout, 2, frame->arg2.symes);
		}

		else {
			cc += fprintf(fout, "\tArg1: ???\n");
		}
	}
	cc += fputcTimes('^', 70, fout);
	cc += fprintf(fout, "\n");

	return cc;
}

local int
sstPrSefo(FILE *fout, int n, Sefo sefo)
{
	int	cc = 0;

	cc += fprintf(fout, "\tArg%d: ", n);
	cc += sefoPrint(fout, sefo);
	cc += fprintf(fout, "\n");

	return cc;
}

local int
sstPrSyme(FILE *fout, int n, Syme syme)
{
	int	cc = 0;
	OStream ostream = ostreamNewFrFile(fout);
	cc += fprintf(fout, "\tArg%d: ", n);
	cc += symePrint(fout, syme);
	cc += fprintf(fout, "\n");
	ostreamClose(ostream);
	ostreamFree(ostream);
	return cc;
}

local int
sstPrTForm(FILE *fout, int n, TForm tform)
{
	int	cc = 0;
	cc += fprintf(fout, "\tArg%d: ", n);
	cc += tformPrint(fout, tform);
	cc += fprintf(fout, "\n");
	return cc;
}

local int
sstPrAbSub(FILE *fout, int n, AbSub sigma)
{
	int	cc = 0;
	cc += fprintf (fout, "\tArg%d: ", n);
	cc += absPrint(fout, sigma);
	cc += fprintf (fout, "\n");
	return cc;
}

local int
sstPrSefoList(FILE *fout, int n, SefoList sefos)
{
	int	i, cc = 0;
	cc += fprintf(fout, "\tArg%d:\n", n);
	for (i = 0; sefos; sefos = cdr(sefos), i++) {
		cc += fprintf(fout, "%d. ", i);
		cc += sefoPrint(fout, car(sefos));
		cc += fprintf(fout, "\n");
	}
	return cc;
}

local int
sstPrSymeList(FILE *fout, int n, SymeList symes)
{
	int	i, cc = 0;
	cc += fprintf(fout, "\tArg%d:\n", n);

	for (i = 0; symes; symes = cdr(symes), i++) {
		cc += fprintf(fout, "%d. ", i);
		cc += symePrint(fout, car(symes));
		cc += fprintf(fout, "\n");
	}
	return cc;
}

local int
sstPrTFormList(FILE *fout, int n, TFormList tforms)
{
	int	i, cc = 0;
	cc += fprintf(fout, "\tArg%d:\n", n);

	for (i = 0; tforms; tforms = cdr(tforms), i++) {
		cc += fprintf(fout, "%d. ", i);
		cc += tformPrint(fout, car(tforms));
		cc += fprintf(fout, "\n");
	}
	return cc;
}

local int
sstPrBool(FILE *fout, int n, Bool flag)
{
	return fprintf(fout, "\tArg%d: %s\n", n, flag ? "true" : "false");
}

local int
sstPrLib(FILE *fout, int n, Lib lib)
{
	int	cc = 0;
	cc += fprintf(fout, "\tArg%d: ", n);
	cc += fprintf(fout, "%s", fnameUnparseStatic(lib->name));
	cc += fprintf(fout, "\n");
	return cc;
}

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (General)
 *
 ****************************************************************************/

static int	sstSerialDebug	= 0;	/* debugging counter */

#define	sstNext(wh,a1,a2) {					\
	if (DEBUG(sst)) {					\
		sstNextDB(wh, (Pointer)(a1), (Pointer)(a2));	\
	}							\
	sstNext0();						\
}
#define	sstDone() {		\
	sstDone0();		\
	if (DEBUG(sst)) {	\
		sstDoneDB();	\
	}			\
}

#define sstDoneSefo(sefo)	{ sefoClear(sefo); sstDone(); }
#define sstDoneSyme(syme)	{ symeClear(syme); sstDone(); }
#define sstDoneTForm(tform)	{ tformClear(tform); sstDone(); }
#define sstDoneAbSub(sigma)	{ abSubClear(sigma); sstDone(); }
#define sstDoneSefoList(sl)	{ sefoListClear(sl); sstDone(); }
#define sstDoneSymeList(sl)	{ symeListClear(sl); sstDone(); }
#define sstDoneTFormList(tl)	{ tformListClear(tl); sstDone(); }

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (Scheme 1)
 *
 ****************************************************************************/

#if defined(MarkScheme1)
/* To handle simultaneous sst traversals, interpret the mark bits
 * as two fields:  the round number and the traversal number.
 * Increment the round number only after the current traversals complete.
 * Each round can handle up to eight simultaneous traversals.
 * We can have a lot of rounds.  (2^24 for 32 bit longs)
 */

/* A more complete implementation would use a stack of mark bits,
 * but since we do so many traversals, it would be better to have
 * a way that doesn't allocate memory.
 */
/* Where this scheme fails the fastest is when we have a need to
 * do repeated instances of traversal "B" within a traversal "A".
 * Being in traversal "A" fixes the round, and then the repeated
 * instances of traversal "B" use up the bits.
 */

static ULong	sstSerialRound		= 1;	/* sst round counter */
static ULong	sstSerialTraversal	= 0;	/* sst traversal counter */
static ULong	sstMarkMask		= 0;	/* active traversal mask */
static ULong	sstSerialNext		= 0;	/* next available mask bit */

/* sstSerialTraversal always points to the high bit of sstMarkMask. */

#define		SST_TR_NBITS	(8)
#define		SST_RD_NBITS	(bitsizeof(SefoMark) - SST_TR_NBITS)

#define		SST_TR_SHIFT	(0)
#define		SST_TR_MASK	((1 << SST_TR_NBITS) - 1)

#define		SST_RD_SHIFT	(SST_TR_SHIFT + SST_TR_NBITS)
#define		SST_RD_MASK	(((1 << SST_RD_NBITS) - 1) << SST_RD_SHIFT)

#define		sstRound(m)	(((m) & SST_RD_MASK) >> SST_RD_SHIFT)
#define		sstSet(r, t)	(((r) << SST_RD_SHIFT) | (t) << SST_TR_SHIFT)
#define		sstBit()	(1 << sstSerialTraversal)

local void
sstNext0(void)
{
	/* If all the traversals for the current round are complete,
	 * go to the next round, otherwise, go to the next traversal.
	 */
	if (sstMarkMask == 0) {
		sstSerialRound += 1;
		sstSerialNext = 0;
	}
	else
		sstSerialNext += 1;

	if (sstSerialNext == SST_TR_NBITS)
		bug("sstNext:  too many simultaneous traversals");

	sstSerialTraversal = sstSerialNext;
	sstMarkMask |= sstBit();
}

local void
sstDone0(void)
{
	ULong	mask;

	sstMarkMask &= ~sstBit();
	sstSerialTraversal = 0;

	for (mask = sstMarkMask >> 1; mask; mask >>= 1)
		sstSerialTraversal += 1;

	assert(sstMarkMask == 0 || sstMarkMask >> sstSerialTraversal == 1);
}

local ULong
sstSetMark(ULong mark)
{
	if (sstRound(mark) == sstSerialRound)
		return (mark | sstBit());
	else
		return sstSet(sstSerialRound, sstBit());
}

local Bool
sstIsMarked(ULong mark)
{
	return (sstRound(mark) == sstSerialRound) && (mark & sstBit());
}

#define	sstMarkSyme(syme)	symeSetMark(syme, sstSetMark(symeMark(syme)))
#define	sstMarkTForm(tf)	tformSetMark(tf, sstSetMark(tformMark(tf)))

#define	sstSymeIsMarked(syme)	sstIsMarked(symeMark(syme))
#define	sstTFormIsMarked(tf)	sstIsMarked(tformMark(tf))

#endif /* defined(MarkScheme1) */


/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (Scheme 2)
 *
 ****************************************************************************/

#if defined(MarkScheme2)

/*
 * This scheme uses each mark bit individually and relies on being able
 * to clear all the bits which are set.
 */

#define SST_NMASK_BITS	bitsizeof(SefoMark)

static int	sstMarkBitNo	= -1;
static SefoMark	sstMarkMask	= 0;
static int	sstMarkCount    = 0;
static int	sstMarkCountSave[SST_NMASK_BITS];

local void
sstNext0(void)
{
	/* Save state. */
	if (sstMarkBitNo >= 0) {
		sstMarkCountSave[sstMarkBitNo] = sstMarkCount;
	        sstMarkDEBUG(dbOut, "sstNext: saving count %d\n",
			     sstMarkCount);
	}

	/* Increment level. */
	sstMarkBitNo++;
	if (sstMarkBitNo >= SST_NMASK_BITS) {
		sstStackPrint(stderr);
		bug("sstNext:  too many simultaneous traversals");
	}

	/* Initialize new state. */
	sstMarkMask = 1L << sstMarkBitNo;
	sstMarkCount = sstMarkCountSave[sstMarkBitNo] = 0;

	sstMarkDEBUG(dbOut, "sstNext: BitNo = %d\n", sstMarkBitNo);
}

local void
sstDone0(void)
{
	/* Verify current state. */
	sstMarkDEBUG(dbOut, "sstDone: BitNo = %d\n", sstMarkBitNo);
	assert(sstMarkBitNo >= 0);
	if (sstMarkCount != 0) {
		sstStackPrint(stderr);
		bug("sstDone:  lost %d marks.", sstMarkCount);
	}

	/* Decrement level. */
	sstMarkBitNo--;

	/* Restore previous state. */
	if (sstMarkBitNo >= 0) {
		sstMarkMask  = 1L << sstMarkBitNo;
		sstMarkCount = sstMarkCountSave[sstMarkBitNo];
	        sstMarkDEBUG(dbOut, "sstDone: restoring count %d\n",
			     sstMarkCount);
	}
}

#define	sstSymeIsMarked(syme)	(symeMark(syme) & sstMarkMask)
#define	sstTFormIsMarked(tf)	(tformMark(tf)  & sstMarkMask)

#define	sstMarkSyme(syme) \
	{ if (!sstSymeIsMarked(syme)) { \
		sstMarkDEBUG(dbOut, "++ marking %p\n", syme); \
		sstMarkCount++; \
		symeSetMark((syme), symeMark(syme) | sstMarkMask); }}

#define	sstMarkTForm(tf) \
	{ if (!sstTFormIsMarked(tf)) { \
		sstMarkDEBUG(dbOut, "++ marking %p\n", tf); \
		sstMarkCount++; \
		tformSetMark((tf), tformMark(tf) | sstMarkMask); }}

#define	sstClearSyme(syme) \
	{ if (sstSymeIsMarked(syme)) { \
		sstMarkDEBUG(dbOut, "-- clearing %p\n", syme); \
		sstMarkCount--; \
		symeSetMark((syme), symeMark(syme) & ~sstMarkMask); }}

#define	sstClearTForm(tf) \
	{ if (sstTFormIsMarked(tf)) { \
		sstMarkDEBUG(dbOut, "-- clearing %p\n", tf); \
		sstMarkCount--; \
		tformSetMark((tf), tformMark(tf) & ~sstMarkMask); }}


#endif /* defined(MarkScheme2) */


/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (Scheme 3)
 *
 ****************************************************************************/

#if defined(MarkScheme3)

static ULong	 sstMarkMask    =  0;	/* For the generic debug fns. */

/*
 * Not Traversing:        sstMarkDepth=0; sstMarkTable=0; sstMarkStack=0;
 * 1st Level(uses Round): sstMarkDepth=1; sstMarkTable=0; sstMarkStack=[0];
 * 2nd Level(uses Table): sstMarkDepth=2; sstMarkTable=T; sstMarkStack=[0,0];
 * 3rd Level(uses Table): sstMarkDepth=3; sstMarkTable=U; sstMarkStack=[T,0,0];
 * ...
 */

static int	 sstMarkDepth	= 0;	/* current depth of marking. */
static long	 sstMarkRound	= 0;	/* round counter        (level 1) */
static TableList sstMarkStack   = 0;	/* mark table stack  (levels > 1) */
static Table	 sstMarkTable   = 0;	/* current markees   (levels > 1) */

local void
sstNext0(void)
{
	sstMarkDepth++;

	if (sstMarkDepth == 1)
		sstMarkRound++;
	else {
		sstMarkStack = listCons(Table)(sstMarkTable, sstMarkStack);
		sstMarkTable = 0;	/* Will be allocated on first use. */
	}
}


local void
sstDone0(void)
{

	if (sstMarkDepth == 1)
		; /* Nothing */
	else {
		if (sstMarkTable) tblFree(sstMarkTable);
		sstMarkTable = car(sstMarkStack);
		sstMarkStack = listFreeCons(Table)(sstMarkStack);
	}

	sstMarkDepth--;
}

local void
sstMarkSyme(Syme syme)
{
	if (sstMarkDepth == 1)
		symeSetMark(syme, sstMarkRound);
	else {
		if (!sstMarkTable)
			sstMarkTable = tblNew((TblHashFun) 0, (TblEqFun) 0);
		tblSetElt(sstMarkTable, (Pointer) syme, (Pointer) syme);
	}
}

local void
sstMarkTForm(TForm tf)
{
	if (sstMarkDepth == 1)
		tformSetMark(tf, sstMarkRound);
	else {
		if (!sstMarkTable)
			sstMarkTable = tblNew((TblHashFun) 0, (TblEqFun) 0);
		tblSetElt(sstMarkTable, (Pointer) tf, (Pointer) tf);
	}
}

#define	sstSymeIsMarked(syme) \
	(sstMarkDepth == 1 ? symeMark(syme) == sstMarkRound \
	 : sstMarkTable && tblElt(sstMarkTable, (Pointer) (syme), NULL))
		
#define	sstTFormIsMarked(tf) \
	(sstMarkDepth == 1 ? tformMark(tf) == sstMarkRound \
	 : sstMarkTable && tblElt(sstMarkTable, (Pointer) (tf), NULL))

#endif /* defined(MarkScheme3) */

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm unmarking
 *
 ****************************************************************************/

#if defined(MarkScheme1)

#define	sefoClear(sefo)		Nothing
#define	symeClear(syme)		Nothing
#define tformClear(tform)	Nothing
#define abSubClear(sigma)	Nothing
#define sefoListClear(sl)	Nothing
#define symeListClear(sl)	Nothing
#define tformListClear(tl)	Nothing

#endif /* defined(MarkScheme1) */


#if defined(MarkScheme2)

local void	sefoClear	(Sefo);
local void	symeClear	(Syme);
local void	tformClear	(TForm);
local void	tqualClear	(TQual);
local void	abSubClear	(AbSub);
local void	sefoListClear	(SefoList);
local void	symeListClear	(SymeList);
local void	tformListClear	(TFormList);
local void	tqualListClear	(TQualList);

local void
sefoClear(Sefo sefo)
{
	if (abIsLeaf(sefo)) {
		if (abSyme(sefo))
			symeClear(abSyme(sefo));
		if (abImplicit(sefo))
			sefoClear(abImplicit(sefo));
	}
	else {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1)
			sefoClear(abArgv(sefo)[i]);
	}
}

local void
symeClear(Syme syme)
{
	if (!sstSymeIsMarked(syme))
		return;

	sstClearSyme(syme);

	if (!symeIsLazy(syme))
		tformClear(symeType(syme));

	if (symeIsImport(syme))
		tformClear(symeExporter(syme));
	if (symeIsExtend(syme))
		symeListClear(symeExtendee(syme));
	if (symeExtension(syme))
		symeClear(symeExtension(syme));
	if (symeCondition(syme))
		sefoListClear(symeCondition(syme));
}

local void
tformClear(TForm tf)
{
	assert (tf != 0);
	tf = tfFollowOnly(tf);

	if (!sstTFormIsMarked(tf))
		return;

	sstClearTForm(tf);

	if (tfIsSym(tf))
		;
	else if (tfIsAbSyn(tf))
		sefoClear(tfGetExpr(tf));
	else if (tfIsNode(tf)) {
		Length	i;
		for (i = 0; i < tfArgc(tf); i += 1)
			tformClear(tfFollowArg(tf, i));
	}
	else
		bugBadCase(tfTag(tf));

	if (tfSelf(tf))
		symeListClear(tfSelf(tf));
	if (tfSelfSelf(tf))
		symeListClear(tfSelfSelf(tf));
	if (tfParents(tf))
		symeListClear(tfParents(tf));
	if (tfSymes(tf))
		symeListClear(tfSymes(tf));

	/* Extra stuff */
	if (tfDomExports(tf))
		symeListClear(tfDomExports(tf));
	if (tfCatExports(tf))
		symeListClear(tfCatExports(tf));
	if (tfThdExports(tf))
		symeListClear(tfThdExports(tf));

	if (tfDomImports(tf))
		symeListClear(tfDomImports(tf));

	if (tfQueries(tf))
		tformListClear(tfQueries(tf));
	if (tfCascades(tf))
		tqualListClear(tfCascades(tf));
}

local void
tqualClear(TQual tq)
{
	assert(tq != 0);

	tformClear(tqBase(tq));
	if (tqIsQualified(tq))
		tformListClear(tqQual(tq));
}

local void
abSubClear(AbSub sigma)
{
	AbBindList	l0;
	for (l0 = sigma->l; l0; l0 = cdr(l0))
		sefoClear(car(l0)->val);
}

local void
sefoListClear(SefoList sefos)
{
	for ( ; sefos; sefos = cdr(sefos))
		sefoClear(car(sefos));
}

local void
symeListClear(SymeList symes)
{
	for ( ; symes; symes = cdr(symes))
		symeClear(car(symes));
}

local void
tformListClear(TFormList tforms)
{
	for ( ; tforms; tforms = cdr(tforms))
		tformClear(car(tforms));
}

local void
tqualListClear(TQualList tquals)
{
	for ( ; tquals; tquals = cdr(tquals))
		tqualClear(car(tquals));
}

#endif /* defined(MarkScheme2) */


#if defined(MarkScheme3)

#define	sefoClear(sefo)		Nothing
#define	symeClear(syme)		Nothing
#define tformClear(tform)	Nothing
#define abSubClear(sigma)	Nothing
#define sefoListClear(sl)	Nothing
#define symeListClear(sl)	Nothing
#define tformListClear(tl)	Nothing

#endif /* defined(MarkScheme3) */


/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal debugging
 *
 ****************************************************************************/

void
sstNextDB(String where, Pointer arg1, Pointer arg2)
{
	int i;

	sstStackPush(where, arg1, arg2);

	if (sstMarkMask <= 1) return;

	fprintf(dbOut, "-> sstNext (%s) [%d]\n", where, sstSerialDebug);

	fprintf(dbOut, "sstNext +");
	for (i = 0; i < bitsizeof(SefoMark); i++)
		fprintf(dbOut, (sstMarkMask & (1<<i)) ? "1" : "0");
	fprintf(dbOut, "\n");

	sstStackPrint(dbOut);
}

void
sstDoneDB(void)
{
	int i;

	sstStackPop();

	if (sstMarkMask <= 1) return;

	fprintf(dbOut, "<- sstDone\n");

	fprintf(dbOut, "sstDone -");
	for (i = 0; i < bitsizeof(SefoMark); i++)
		fprintf(dbOut, (sstMarkMask & (1<<i)) ? "1" : "0");
	fprintf(dbOut, "\n");
}

/*****************************************************************************
 *
 * :: sstPrint
 *
 ****************************************************************************/

/*
 * Within debugging code, we should avoid using the sst marking code
 * and avoid pushing any lazy symes.
 */

/*
 * sstPrint external entry points.
 */

int
sefoPrint(FILE *fout, Sefo sefo)
{
	struct _OStream ostream;
	int n;

	ostreamInitFrFile(&ostream, fout);
	n = sefoOStreamPrint0(&ostream, true, sefo);
	ostreamClose(&ostream);
	return n;
}

int
symePrint(FILE *fout, Syme syme)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, fout);
	n = symeOStreamPrint0(&ostream, true, syme);
	ostreamClose(&ostream);
	return n;
}

int
symeOStreamWrite(OStream ostream, Syme syme)
{
	int n = symeOStreamPrint0(ostream, false, syme);

	return n;
}

int
tformPrint(FILE *fout, TForm tf)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, fout);
	n = tformOStreamPrint0(&ostream, true, tf);
	ostreamClose(&ostream);
	return n;
}

int
sefoListPrint(FILE *fout, SefoList sefos)
{
	struct _OStream ostream;
	int n;

	ostreamInitFrFile(&ostream, fout);
	n = sefoListOStreamPrint0(&ostream, true, sefos);
	ostreamClose(&ostream);
	return n;
}

int
symeListPrint(FILE *fout, SymeList symes)
{
	struct _OStream ostream;
	int n;

	ostreamInitFrFile(&ostream, fout);
	n = symeListOStreamPrint0(&ostream, true, symes);
	ostreamClose(&ostream);
	return n;
}

int
tformListPrint(FILE *fout, TFormList tforms)
{
	struct _OStream ostream;
	int n;

	ostreamInitFrFile(&ostream, fout);
	n = tformListOStreamPrint0(&ostream, true, tforms);
	ostreamClose(&ostream);
	return n;
}

/*
 * sstPrintDb external entry points.
 */

int
sefoPrintDb(Sefo sefo)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, dbOut);
	n = sefoOStreamPrint0(&ostream, false, sefo);

	ostreamClose(&ostream);
	fnewline(dbOut);
	return n;
}

int
symePrintDb(Syme syme)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, dbOut);
	n = symeOStreamPrint0(&ostream, false, syme);
	ostreamClose(&ostream);
	fnewline(dbOut);
	return n;
}

int
symePrintDb2(Syme syme)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, dbOut);
	n = symeOStreamPrint0(&ostream, false, syme);
	ostreamClose(&ostream);
	return n;
}

int 
tformOStreamWrite(OStream ostream, TForm tf)
{
	int n = tformOStreamPrint0(ostream, false, tf);

	return n;
}

int 
sefoOStreamWrite(OStream ostream, Sefo sefo)
{
	int n = sefoOStreamPrint0(ostream, false, sefo);

	return n;
}

int
tformPrintDb(TForm tf)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, dbOut);
	n = tformOStreamPrint0(&ostream, false, tf);
	ostreamClose(&ostream);
	fnewline(dbOut);
	return n;
}

int
sefoListPrintDb(SefoList sefos)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, dbOut);
	n = sefoListOStreamPrint0(&ostream, false, sefos);
	ostreamClose(&ostream);
	fnewline(dbOut);
	return n;
}

int
symeListPrintDb(SymeList symes)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, dbOut);
	n = symeListOStreamPrint0(&ostream, false, symes);
	ostreamClose(&ostream);
	fnewline(dbOut);
	return n;
}

int
tformListPrintDb(TFormList tforms)
{
	struct _OStream ostream;
	int n;
	ostreamInitFrFile(&ostream, dbOut);
	n = tformListOStreamPrint0(&ostream, false, tforms);
	ostreamClose(&ostream);
	fnewline(dbOut);
	return n;
}

/*
 * Local functions.
 */

/* The deep argument is currently unused. */
local int
sefoOStreamPrint0(OStream ostream, Bool deep, Sefo sefo)
{
	int		cc = 0;

	if (!sefo)
		return ostreamPrintf(ostream, "(* NULL *)");

	cc += ostreamPrintf(ostream, "(*");

	if (abIsLeaf(sefo)) {
		cc += ostreamPrintf(ostream, " ");
		cc += abOStreamPrint(ostream, sefo);
		if (DEBUG(sefoPrint)) {
			if (abSyme(sefo)) {
				cc += ostreamPrintf(ostream, " ");
				cc += symeOStreamPrint0(ostream, false, abSyme(sefo));
			}
		}
	}

	else {
		Length i;
		for (i = 0; i < abArgc(sefo); i += 1) {
			cc += ostreamPrintf(ostream, " ");
			cc += sefoOStreamPrint0(ostream, deep, abArgv(sefo)[i]);
		}
	}

	cc += ostreamPrintf(ostream, " *)");

	return cc;
}

local int
symeOStreamPrint0(OStream ostream, Bool deep, Syme syme)
{
	int		cc = 0;

	if (!syme)
		return ostreamPrintf(ostream, "{* NULL *}");

	cc += ostreamPrintf(ostream, "{* (%s) ",
		      symeInfo[symeKind(syme)].str + sizeof("SYME_") - 1);

	cc += ostreamPrintf(ostream, "%s", symeString(syme));

	if (DEBUG(sefoPrint)) {
		cc += ostreamPrintf(ostream, " [cn:%ld, dn:%d %ld],", symeConstNum(syme), symeDefnNum(syme), symeMark(syme));
	}

	if (deep) {
		cc += ostreamPrintf(ostream, " : ");
		if (symeIsLazy(syme))
			cc += ostreamPrintf(ostream, "[lazy] %ld", symeHash(syme));
		else
			cc += tformOStreamPrint0(ostream, deep, symeType(syme));

		if (symeIsImport(syme)) {
			cc += ostreamPrintf(ostream, " from ");
			cc += tformOStreamPrint0(ostream, deep, symeExporter(syme));
		}
	}

	cc += ostreamPrintf(ostream, " *}");

	return cc;
}

local int
tformOStreamPrint0(OStream ostream, Bool deep, TForm tf)
{
	int		cc = 0;

	if (!tf)
		return ostreamPrintf(ostream, "<* NULL *>");

	cc += ostreamPrintf(ostream, "<* %s", tformStr(tfTag(tf)));

	if (tfIsSym(tf))
		;
	else if (tfIsAbSyn(tf)) {
		cc += ostreamPrintf(ostream, " ");
		cc += sefoOStreamPrint0(ostream, deep, tfGetExpr(tf));
	}
	else if (tfIsNode(tf)) {
		Length	i;
		for (i = 0; i < tfArgc(tf); i += 1) {
			cc += ostreamPrintf(ostream, " ");
			cc += tformOStreamPrint0(ostream, deep, tfArgv(tf)[i]);
		}
	}
	else
		bugBadCase(tfTag(tf));

	if (tfSymes(tf))
		cc += symeListOStreamPrint0(ostream, deep, tfSymes(tf));

	cc += ostreamPrintf(ostream, " *>");

	return cc;
}

/* The deep argument is currently unused. */
local int
sefoListOStreamPrint0(OStream ostream, Bool deep, SefoList sefos)
{
	int		cc = 0;

	cc += ostreamPrintf(ostream, " (");
	for (; sefos; sefos = cdr(sefos)) {
		cc += ostreamPrintf(ostream, " ");
		cc += sefoOStreamPrint0(ostream, false, car(sefos));
	}
	cc += ostreamPrintf(ostream, " )");

	return cc;
}

/* The deep argument is currently unused. */
local int
symeListOStreamPrint0(OStream ostream, Bool deep, SymeList symes)
{
	int		cc = 0;

	cc += ostreamPrintf(ostream, " (");
	for (; symes; symes = cdr(symes)) {
		cc += ostreamPrintf(ostream, " ");
		cc += symeOStreamPrint0(ostream, false, car(symes));
	}
	cc += ostreamPrintf(ostream, " )");

	return cc;
}

/* The deep argument is currently unused. */
local int
tformListOStreamPrint0(OStream ostream, Bool deep, TFormList tforms)
{
	int		cc = 0;

	cc += ostreamPrintf(ostream, " (");
	for (; tforms; tforms = cdr(tforms)) {
		cc += ostreamPrintf(ostream, " ");
		cc += tformOStreamPrint0(ostream, false, car(tforms));
	}
	cc += ostreamPrintf(ostream, " )");

	return cc;
}

/*****************************************************************************
 *
 * :: symeIsTwin
 *
 ****************************************************************************/

local void		symeMarkTwins		(Syme);
local Bool		symeFindTwins		(Syme);
local void		symeClearTwins		(Syme);

Bool
symeIsTwin(Syme syme1, Syme syme2)
{
	Bool	eq;

	if (syme1 == syme2) return false;

	symeMarkTwins(syme2);
	eq = symeFindTwins(syme1);
	symeClearTwins(syme2);

	return eq;
}

local void
symeMarkTwins(Syme syme)
{
	SymeList	symes = symeTwins(syme);

	symeSetMarkBit(syme);
	for (; symes; symes = cdr(symes)) {
		Syme	twin = car(symes);
		if (twin != syme) symeMarkTwins(twin);
	}
}

local Bool
symeFindTwins(Syme syme)
{
	SymeList	symes = symeTwins(syme);
	Bool		result = false;

	result = symeMarkBit(syme);
	for (; !result && symes; symes = cdr(symes)) {
		Syme	twin = car(symes);
		if (twin != syme) result = symeFindTwins(twin);
	}

	return result;
}

local void
symeClearTwins(Syme syme)
{
	SymeList	symes = symeTwins(syme);

	symeClrMarkBit(syme);
	for (; symes; symes = cdr(symes)) {
		Syme	twin = car(symes);
		if (twin != syme) symeClearTwins(twin);
	}
}

/*****************************************************************************
 *
 * :: sstEqual
 *
 ****************************************************************************/

/*
 * sstEqual external entry points.
 */

Bool
sefoEqual(Sefo sefo1, Sefo sefo2)
{
	Bool	eq;

	sstNext("sefoEqual", sefo1, sefo2);
	eq = sefoEqual0(NULL, sefo1, sefo2);
	sstDoneSefo(sefo1);

	if (eq)
		assert(abHashSefo(sefo1) == abHashSefo(sefo2));

	return eq;
}

Bool
symeEqual(Syme syme1, Syme syme2)
{
	Bool	eq;

	sstNext("symeEqual", syme1, syme2);
	eq = symeEqual0(NULL, syme1, syme2);
	sstDoneSyme(syme1);

	return eq;
}

Bool
tformEqual(TForm tf1, TForm tf2)
{
	Bool	eq;

	sstNext("tformEqual", tf1, tf2);
	eq = tformEqual0(NULL, tf1, tf2);
	sstDoneTForm(tf1);

	return eq;
}

Bool
sefoListEqual(SefoList sefos1, SefoList sefos2)
{
	Bool	eq;

	sstNext("sefoListEqual", sefos1, sefos2);
	eq = sefoListEqual0(NULL, sefos1, sefos2);
	sstDoneSefoList(sefos1);

	return eq;
}

Bool
symeListEqual(SymeList symes1, SymeList symes2)
{
	Bool	eq;

	sstNext("symeListEqual", symes1, symes2);
	eq = symeListEqual0(NULL, symes1, symes2);
	sstDoneSymeList(symes1);

	return eq;
}

Bool
tformListEqual(TFormList tforms1, TFormList tforms2)
{
	Bool	eq;

	sstNext("tformListEqual", tforms1, tforms2);
	eq = tformListEqual0(NULL, tforms1, tforms2);
	sstDoneTFormList(tforms1);

	return eq;
}

/*
 * sstEqualMod external entry points.
 */

Bool
sefoEqualMod(SymeList mods, Sefo sefo1, Sefo sefo2)
{
	Bool	eq;

	sstNext("sefoEqualMod", sefo1, sefo2);
	eq = sefoEqual0(mods, sefo1, sefo2);
	sstDoneSefo(sefo1);

	return eq;
}

Bool
symeEqualMod(SymeList mods, Syme syme1, Syme syme2)
{
	Bool	eq;

	sstNext("symeEqualMod", syme1, syme2);
	eq = symeEqual0(mods, syme1, syme2);
	sstDoneSyme(syme1);

	return eq;
}

Bool
tformEqualMod(SymeList mods, TForm tf1, TForm tf2)
{
	Bool	eq;

	sstNext("tformEqualMod", tf1, tf2);
	eq = tformEqual0(mods, tf1, tf2);
	sstDoneTForm(tf1);

	return eq;
}

Bool
sefoListEqualMod(SymeList mods, SefoList sefos1, SefoList sefos2)
{
	Bool	eq;

	sstNext("sefoListEqualMod", sefos1, sefos2);
	eq = sefoListEqual0(mods, sefos1, sefos2);
	sstDoneSefoList(sefos1);

	return eq;
}

Bool
symeListEqualMod(SymeList mods, SymeList symes1, SymeList symes2)
{
	Bool	eq;

	sstNext("symeListEqualMod", symes1, symes2);
	eq = symeListEqual0(mods, symes1, symes2);
	sstDoneSymeList(symes1);

	return eq;
}

Bool
tformListEqualMod(SymeList mods, TFormList tforms1, TFormList tforms2)
{
	Bool	eq;

	sstNext("tformListEqualMod", tforms1, tforms2);
	eq = tformListEqual0(mods, tforms1, tforms2);
	sstDoneTFormList(tforms1);

	return eq;
}

Bool
symeEqualModConditions(SymeList mods, Syme syme1, Syme syme2)
{
	Bool	eq;

	symeSetPopConds(syme1);
	symeSetPopConds(syme2);
	eq = symeEqualMod(mods, syme1, syme2);
	symeClrPopConds(syme2);
	symeClrPopConds(syme1);

	return eq;
}

/*
 * Local functions.
 */

#define			symeModsAllowAlpha(mods)		\
	((mods) && cdr(mods) && !symeIsSelf(car(mods)) &&	\
	 symeId(car(mods)) != symeId(car(cdr(mods))))

local Bool
sefoIsDefinedType(Sefo sefo)
{
	TForm	cat = abGetCategory(sefo);
	return tfIsDefineOfType(cat);
}

local Sefo
sefoDefinedVal(Sefo sefo)
{
	TForm	cat = abGetCategory(sefo);
	return tfExpr(tfDefineVal(cat));
}

local Bool
sefoEqual0(SymeList mods, Sefo sefo1, Sefo sefo2)
{
	Bool	result = true;
	Length	serial = 0;

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	sefo1 = sefoEqualMods(sefo1);
	sefo2 = sefoEqualMods(sefo2);

	if (DEBUG(sefoEqual)) {
		fprintf(dbOut, "-> sefoEqual[%d]:", (int) serial);
		fnewline(dbOut);
		sefoPrintDb(sefo1);
		sefoPrintDb(sefo2);
	}

	if (sefo1 == sefo2)
		result = true;

	else if (!sefo1 || !sefo2)
		result = false;

	else if (abTag(sefo1) != abTag(sefo2))
		result = false;

	else if (abArgc(sefo1) != abArgc(sefo2))
		result = false;

	else if (abIsId(sefo1))
		result = symeEqual0(mods, abSyme(sefo1), abSyme(sefo2));

	else if (abIsLeaf(sefo1))
		result = abEqual(sefo1, sefo2) &&
			 symeEqual0(mods, abSyme(sefo1), abSyme(sefo2));

	else {
		Length	i;
		for (i = 0; result && i < abArgc(sefo1); i += 1)
			result = sefoEqual0(mods, abArgv(sefo1)[i],
					    abArgv(sefo2)[i]);
	}

	if (!result) {
		if (sefoIsDefinedType(sefo1))
			result = sefoEqual0(mods, sefoDefinedVal(sefo1), sefo2);

		else if (sefoIsDefinedType(sefo2))
			result = sefoEqual0(mods, sefo1, sefoDefinedVal(sefo2));
	}

	if (DEBUG(sefoEqual)) {
		fprintf(dbOut, "<- sefoEqual[%d] = %s",
			(int) serial, boolToString(result));
		fnewline(dbOut);
	}

	return result;
}

local Bool
symeEqual0(SymeList mods, Syme syme1, Syme syme2)
{
	Bool	result = true;
	Length	serial = 0;

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (syme1 == syme2)
		return true;

	else if (!syme1 || !syme2)
		return false;

	sefoEqualDEBUG(dbOut, "(symeEqual[%d]: %pSyme %pSyme\n",
		       (int) serial, syme1, syme2);

	if (symeIsArchive(syme1) && symeIsLibrary(syme2))
		result = arLibraryIsMember(symeArchive(syme1),
					   symeLibrary(syme2));

	else if (symeIsArchive(syme2) && symeIsLibrary(syme1))
		result = arLibraryIsMember(symeArchive(syme2),
					   symeLibrary(syme1));

	else if (symeId(syme1) != symeId(syme2)) {
		if (symeModsAllowAlpha(mods))
			result = listMember(Syme)(mods, syme1, symeEq) &&
				 listMember(Syme)(mods, syme2, symeEq);
		else
			result = false;
	}

	else if (sstSymeIsMarked(syme1))
		result = true;

	else if (symeExtendEqual0(mods, syme1, syme2))
		result = true;

	else if (listMember(Syme)(mods, syme1, symeEq) &&
		 listMember(Syme)(mods, syme2, symeEq))
		result = true;

	else if (symeIsSelf(syme1))
		result = false;

	else if (symeIsSelfSelf(syme1)) {
		TForm	tf1;
		TForm	tf2;

		if (symeHash(syme1) && symeHash(syme2) &&
		    symeHash(syme1) != symeHash(syme2))
			result = false;
		else {
			tf1 = symeType(syme1);
			tf2 = symeType(syme2);

			tfFollow(tf1);
			tfFollow(tf2);

			assert (tfIsGeneral(tf1) && tfIsGeneral(tf2));
			result = (sefoListEqual0(mods, symeCondition(syme1),
						 symeCondition(syme2)) &&
				  abEqualModDeclares(tfGetExpr(tf1), tfGetExpr(tf2)));
		}
	}

	else if (symeIsTwin(syme1, syme2)) {
		if (symeIsImport(syme1) && symeIsImport(syme2))
			result = symeOriginEqual0(mods, syme1, syme2);
		else
			result = true;
	}

	else {
		sstMarkSyme(syme1);
		result = (symeOriginEqual0(mods, syme1, syme2) &&
			  sefoListEqual0(mods, symeCondition(syme1),
					 symeCondition(syme2)) &&
			  symeTypeEqual0(mods, syme1, syme2));
	}

	sefoEqualDEBUG(dbOut, "  symeEqual[%d] = %s)\n",
		       (int) serial, boolToString(result));

	return result;
}


local Bool tformStructSimilar(TForm tf1, TForm tf2);

local Bool
tformEqual0(SymeList mods, TForm tf1, TForm tf2)
{
	Bool	result = true;
	Length	serial = 0;
#if CheckSimilar
	Bool    similar = true;
#endif
	tf1 = tfFollowOnly(tf1);
	tf2 = tfFollowOnly(tf2);


	if (tf1 == tf2) return true;

	/* Only use tfFollow if exactly one of the types is lazy. */
	if (tfIsSubst(tf1) != tfIsSubst(tf2)) {
		if (!tformStructSimilar(tf1, tf2)) {
#if CheckSimilar
			fprintf(dbOut, "(Similar test failed:\n");
			tfPrintDb(tf1);
			tfPrintDb(tf2);
			fprintf(dbOut, ")\n");
			similar = false;
#else
			return false;
#endif
		}
		tfFollow(tf1);
		tfFollow(tf2);
	}

	/* If we can determine equality w/o using tfFollow, do so. */
	if (tfIsSubst(tf1) || tfIsSubst(tf2)) {
		assert(tfIsSubst(tf1) && tfIsSubst(tf2));
		if (tfSubstSigma(tf1) == tfSubstSigma(tf2) &&
		    tformEqual0(mods, tfSubstArg(tf1), tfSubstArg(tf2)))
			return true;
		tfFollow(tf1);
		tfFollow(tf2);
	}

	/* Only use tfDefineeType if both types are fully analyzed. */
	if (!tfIsSyntax(tf1) && !tfIsSyntax(tf2)) {
		tf1 = tfDefineeType(tf1);
		tf2 = tfDefineeType(tf2);
	}

	/* The tfDefineeType of a type might also be a syntax type. */
	if (tfIsSyntax(tf1) || tfIsSyntax(tf2))
		return abEqualModDeclares(tfExpr(tf1), tfExpr(tf2));

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (DEBUG(sefoEqual)) {
		fprintf(dbOut, "-> tformEqual[%d]:", (int) serial);
		fnewline(dbOut);
		tformPrintDb(tf1);
		tformPrintDb(tf2);
	}

	if (tf1 == tf2)
		result = true;

	else if ((tfIsLibrary(tf1) || tfIsArchive(tf1)) &&
		 (tfIsLibrary(tf2) || tfIsArchive(tf2)))
		result = true;

	else if ((tfTag(tf1) != tfTag(tf2)) || (tfArgc(tf1) != tfArgc(tf2)))
		result = false;

	else if (tfIsSym(tf1))
		result = true;

	else if (tfTag(tf1) == TF_General)
		result = sefoEqual0(mods, tfGetExpr(tf1), tfGetExpr(tf2));

	else if (tfIsRecord(tf1) || tfIsRawRecord(tf1) ||
		 tfIsUnion(tf1) || tfIsEnum(tf1)) {
		Length	i;
		for (i = 0; result && i < tfArgc(tf1); i += 1) {
			Symbol	sym1 = tfCompoundId(tf1, i);
			Symbol	sym2 = tfCompoundId(tf2, i);
			if (!sym1 || !sym2 || sym1 == sym2)
				result = tformEqual0(mods, tfArgv(tf1)[i],
						     tfArgv(tf2)[i]);
			else
				result = false;
		}
	}
	/* ??? Do we need to special-case TrailingArray */
	else if (tfIsNode(tf1)) {
		Length	i;
		for (i = 0; result && i < tfArgc(tf1); i += 1)
			result = tformEqual0(mods, tfArgv(tf1)[i],
					     tfArgv(tf2)[i]);
	}
	else
		bugBadCase(tfTag(tf1));

	if (result && tfIsWith(tf1) &&
	    (tfUseCatExports(tf1) || tfUseCatExports(tf2)))
		result = symeListEqual0(mods, tfParents(tf1),
					tfParents(tf2));

	if (result && tfIsThird(tf1) &&
	    (tfUseThdExports(tf1) || tfUseThdExports(tf2)))
		result = symeListEqual0(mods, tfGetThdExports(tf1),
					tfGetThdExports(tf2));

	if (result && tformEqualCheckSymes(tf1) && tformEqualCheckSymes(tf2))
		result = symeListEqual0(mods, tfSymes(tf1), tfSymes(tf2));

	if (!result) {
		if (tfIsDefinedType(tf1))
			result = tformEqual0(mods, tfDefinedVal(tf1), tf2);

		else if (tfIsDefinedType(tf2))
			result = tformEqual0(mods, tf1, tfDefinedVal(tf2));
	}

	if (DEBUG(sefoEqual)) {
		fprintf(dbOut, "<- tformEqual[%d] = %s",
			(int) serial, boolToString(result));
		fnewline(dbOut);
	}
#if CheckSimilar
	if (result && !similar)
		bug("tformEqual, dissimilar, but the same!");
#endif
	return result;
}

local Bool
tformStructSimilar(TForm tf1, TForm tf2)
{
	int i;
	/* We use the fact that only absyn can be replaced
	 * by substitution
	 */
	tf1 = tfFollowSubst(tf1);
	tf2 = tfFollowSubst(tf2);

	if (tfIsDeclare(tf1))
		tf1 = tfDeclareType(tf1);
	if (tfIsDeclare(tf2))
		tf2 = tfDeclareType(tf2);

	/* These may change into something real */
	if (tfIsGeneral(tf1) || tfIsGeneral(tf2))
		return true;

	if (tfIsSyntax(tf1) || tfIsSyntax(tf2))
		return true;
	/* TF_Define is downright bizzare, and can be whatever it 
	 * feels like.
	 */
	if (tfIsDefine(tf1) || tfIsDefine(tf2))
		return true;

	if (tfTag(tf1) != tfTag(tf2))
		return false;
	if (tfArgc(tf1) != tfArgc(tf2))
		return false;
	
	for (i=0; i<tfArgc(tf1); i++)
		if (!tformStructSimilar(tfArgv(tf1)[i], tfArgv(tf2)[i]))
		    return false;
	
	return true;
}

local Bool
sefoListEqual0(SymeList mods, SefoList sefos1, SefoList sefos2)
{
	/* Quick check for empty and identical lists. */
	if (sefos1 == sefos2)
		return true;

	if (listLength(Sefo)(sefos1) != listLength(Sefo)(sefos2))
		return false;

	for (; sefos1 && sefos2; sefos1 = cdr(sefos1), sefos2 = cdr(sefos2))
		if (!sefoEqual0(mods, car(sefos1), car(sefos2)))
			return false;

	return true;
}

local Bool
symeListEqual0(SymeList mods, SymeList symes1, SymeList symes2)
{
	/* Quick check for empty and identical lists. */
	if (symes1 == symes2)
		return true;

	if (listLength(Syme)(symes1) != listLength(Syme)(symes2))
		return false;

	for (; symes1 && symes2; symes1 = cdr(symes1), symes2 = cdr(symes2))
		if (!symeEqual0(mods, car(symes1), car(symes2)))
			return false;

	return true;
}

local Bool
tformListEqual0(SymeList mods, TFormList tl1, TFormList tl2)
{
	/* Quick check for empty and identical lists. */
	if (tl1 == tl2)
		return true;

	if (listLength(TForm)(tl1) != listLength(TForm)(tl2))
		return false;

	for (; tl1 && tl2; tl1 = cdr(tl1), tl2 = cdr(tl2))
		if (!tformEqual0(mods, car(tl1), car(tl2)))
			return false;

	return true;
}

local Bool
symeTypeEqual0(SymeList mods, Syme syme1, Syme syme2)
{
	if ((symeIsLazy(syme1) && symeIsImport(syme1)) ||
	    (symeIsLazy(syme2) && symeIsImport(syme2)))
		return symeTypeCode(syme1) == symeTypeCode(syme2);
	else if (tfIsTrigger(syme1->type) || tfIsTrigger(syme2->type))
		return symeTypeCode(syme1) == symeTypeCode(syme2);
	else
		return tformEqual0(mods, symeType(syme1), symeType(syme2));
}

local Bool
symeExtendEqual0(SymeList mods, Syme syme1, Syme syme2)
{
	Syme		syme, ext, e1, e2;
	SymeList	symes;

	/* Check the extension, if any. */

	e1 = symeExtension(syme1);
	e2 = symeExtension(syme2);

	if (e1 || e2) {
		Bool	result;
		Syme	s1, s2;

		symeSetExtension(syme1, NULL);
		symeSetExtension(syme2, NULL);

		s1 = e1 ? e1 : syme1;
		s2 = e2 ? e2 : syme2;
		result = symeEqual0(mods, s1, s2);

		symeSetExtension(syme1, e1);
		symeSetExtension(syme2, e2);

		return result;
	}

	/* Check the extendees, if any. */

	if (symeIsImport(syme1) && symeIsImport(syme2))
		return false;

	if (symeIsImportOfExtend(syme1)) syme1 = symeOriginal(syme1);
	if (symeIsImportOfExtend(syme2)) syme2 = symeOriginal(syme2);

	if (symeIsExtend(syme1)) {
		ext = syme1;
		syme = syme2;
	}
	else if (symeIsExtend(syme2)) {
		ext = syme2;
		syme = syme1;
	}
	else
		return false;

	for (symes = symeExtendee(ext); symes; symes = cdr(symes))
		if (symeEqual0(mods, car(symes), syme))
			return true;

	return false;
}

local Bool
symeOriginEqual0(SymeList mods, Syme syme1, Syme syme2)
{
	Bool	result = true;
	Length	serial = 0;

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (DEBUG(sefoEqual)) {
		fprintf(dbOut, "-> symeOriginEqual[%d]:", (int) serial);
		fnewline(dbOut);
		symePrintDb(syme1);
		symePrintDb(syme2);
	}

	if (symeKind(syme1) != symeKind(syme2))
		result = false;

	else if (symeOrigin(syme1) == symeOrigin(syme2))
		result = true;

	else switch (symeKind(syme1)) {
	case SYME_Builtin:
		result = (symeBuiltin(syme1) == symeBuiltin(syme2));
		break;
	case SYME_Foreign:
		result = forgEqual(symeForeign(syme1), symeForeign(syme2));
		break;
	case SYME_Import:
		result = tformEqual0(mods, symeExporter(syme1),
				     symeExporter(syme2));
		break;
	case SYME_Library:
		result = libEqual(symeLibrary(syme1), symeLibrary(syme2));
		break;
	case SYME_Archive:
		result = arEqual(symeArchive(syme1), symeArchive(syme2));
		break;
	case SYME_Export:
	case SYME_Extend:
		result = true;
		break;
	default:
		bugBadCase(symeKind(syme1));
	}

	if (DEBUG(sefoEqual)) {
		fprintf(dbOut, "<- symeOriginEqual[%d] = %s",
			(int) serial, boolToString(result));
		fnewline(dbOut);
	}

	return result;
}

local Bool
tformEqualCheckSymes(TForm tf)
{
	switch (tfTag(tf)) {
	case TF_Declare:
	case TF_Cross:
	case TF_Map:
	case TF_PackedMap:
	case TF_Multiple:
	case TF_Add:
	case TF_Third:
		return true;
	default:
		if (tfIsLibrary(tf) || tfIsArchive(tf))
			return true;
		return false;
	}
}

/*
 * Equality preserving functions for sstEqual.
 */

local Sefo
sefoEqualMods(Sefo sefo)
{
	Bool	changed = (sefo != NULL);

	while (changed)
		switch (abTag(sefo)) {
		case AB_PretendTo:
			sefo = sefo->abPretendTo.expr;
			break;
		case AB_RestrictTo:
			sefo = sefo->abRestrictTo.expr;
			break;
		case AB_Qualify:
			sefo = sefo->abQualify.what;
			break;
		case AB_Declare:
			sefo = sefo->abDeclare.type;
			break;
		case AB_Test: {
			if (tfEqual(abTUnique(sefo), tfBoolean))
				sefo = sefo->abTest.cond;
			break;
		}

		default:
			changed = false;
			break;
		}

	return sefo;
}

/*****************************************************************************
 *
 * :: sefoCopy
 *
 ****************************************************************************/

Sefo
sefoCopy(Sefo sefo)
{
	Sefo	result = abCopy(sefo);
	abTransferSemantics(sefo, result);
	return result;
}

/*****************************************************************************
 *
 * :: sefoAudit
 *
 ****************************************************************************/

Bool
sefoAudit(Bool verbose, Sefo sefo)
{
	int	i;
	Bool	ok = true;

	if (abIsLeaf(sefo) && !abSyme(sefo)) {
		if (verbose) {
			String expr = abPretty(sefo);
			fprintf(dbOut, "No symbol meaning for leaf: %s", expr);
			fnewline(dbOut);
			strFree(expr);
		}
		ok = false;
	}
	if (!abIsLeaf(sefo))
		for (i = 0; i < abArgc(sefo); i++)
			ok &= sefoAudit(verbose, abArgv(sefo)[i]);
	return ok;
}

/******************************************************************************
 *
 * :: sstFreeVars
 *
 *****************************************************************************/

/*
 * sstFreeVars entry points.
 */

void
sefoFreeVars(Sefo sefo)
{
	if (abIsLeaf(sefo)) {
		if (abSyme(sefo))
			symeFreeVars(abSyme(sefo));
	}
	else {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1)
			sefoFreeVars(abArgv(sefo)[i]);

		if (abState(sefo) == AB_State_HasUnique)
			tformFreeVars(abTUnique(sefo));
	}
}

void
symeFreeVars(Syme syme)
{
	if (symeIsImport(syme))
		tformFreeVars(symeExporter(syme));
	else if (!symeIsSelf(syme))
		tformFreeVars(symeType(syme));

	if (symeCondition(syme))
		sefoListFreeVars(symeCondition(syme));
}

local int sfvPrint(FILE *out);

void
tformFreeVars(TForm tf)
{
	TForm		ancestor;
	FreeVar		fv;

	tf = tfFollowOnly(tf);
	if (tfIsPending(tf) && tfIsSyntax(tf))
		return;

	if (tfFVars(tf))
		return;

	sfvInitTable();
	sfvPushTable();
	ancestor = tf;

	sstNext("tformFreeVars", &ancestor, tf);
	tformFreeVars0(&ancestor, NULL, tf);
	sstDoneTForm(tf);

	fv = sfvPopTable(true);
	sfvFiniTable();

	tfSetFVars(tf, fv);
	sefoFreeDEBUG(dbOut, "FV(final) %pTForm = %pFreeVar\n", tf, tf->fv);
	if (DEBUG(sefoFree)) {
		sfvPrint(dbOut);
	}
}

void
tqualFreeVars(TQual tqual)
{
	tformFreeVars(tqBase(tqual));
	tformListFreeVars(tqQual(tqual));
}

void
abSubFreeVars(AbSub sigma)
{
	TForm		ancestor = tfNone();
	FreeVar		fv;

	if (absFVars(sigma))
		return;

	sfvInitTable();
	sfvPushTable();

	sstNext("abSubFreeVars", &ancestor, sigma);
	abSubFreeVars0(&ancestor, ancestor, sigma);
	sstDoneAbSub(sigma);

	fv = sfvPopTable(true);
	sfvFiniTable();

	absSetFVars(sigma, fv);
}

void
sefoListFreeVars(SefoList sefos)
{
	for (; sefos; sefos = cdr(sefos))
		sefoFreeVars(car(sefos));
}

void
symeListFreeVars(SymeList symes)
{
	for (; symes; symes = cdr(symes))
		symeFreeVars(car(symes));
}

void
tformListFreeVars(TFormList tforms)
{
	for (; tforms; tforms = cdr(tforms))
		tformFreeVars(car(tforms));
}

void
tqualListFreeVars(TQualList tquals)
{
	for (; tquals; tquals = cdr(tquals))
		tqualFreeVars(car(tquals));
}

/*
 * sstFreeVars table stack functions.
 */

static SymeListList	sfvTableList		= listNil(SymeList);
static AIntList		sfvBase			= listNil(AInt);
static AInt		sfvDepth		= 0;

#define		sfvHasDepth(l,n)	((l) != listNil(AInt) && car(l) == (n))
#define		sfvGetDepths(sy)	symeDepths(sy)
#define		sfvSetDepths(sy,l)	symeSetDepths(sy,l)
#define		sfvIsBase(n)		((n) == car(sfvBase))

local void
sfvInitTable(void)
{
	sfvBase = listCons(AInt)(sfvDepth, sfvBase);
}

local void
sfvFiniTable(void)
{
	sfvBase = listFreeCons(AInt)(sfvBase);
}

local int
sfvPrint(FILE *out)
{
	return afprintf(out,
		       "(SFV: %d  %pAIntList, %pSymeListList)\n",
		       sfvDepth, sfvBase, sfvTableList);
}

local void
sfvPushTable(void)
{
	SymeList	tbl = listNil(Syme);
	sfvTableList = listCons(SymeList)(tbl, sfvTableList);
	sfvDepth += 1;
}

local void
sfvConsTable(Syme syme)
{
	SymeList	tbl = car(sfvTableList);
	listPush(Syme, syme, tbl);
	setcar(sfvTableList, tbl);
}

local FreeVar
sfvPopTable(Bool save)
{
	SymeList	tbl = car(sfvTableList);
	SymeList	symes, fsymes;
	int		odepth = sfvDepth;

	sfvTableList = listFreeCons(SymeList)(sfvTableList);
	sfvDepth -= 1;

	fsymes = listNil(Syme);
	for (symes = tbl; symes; symes = cdr(symes)) {
		Syme		syme = car(symes);
		List(AInt)	elt = sfvGetDepths(syme);

		sefoFreeDEBUG(dbOut, " popTable: %pSyme %d %d %pAIntList\n",
			      syme, sfvHasDepth(elt, odepth), odepth, elt);

		if (!sfvHasDepth(elt, odepth)) continue;

		if (sfvIsBase(sfvDepth)) {
			elt = listFreeCons(AInt)(elt);
			sfvSetDepths(syme, elt);
		}
		else if (sfvHasDepth(cdr(elt), sfvDepth)) {
			elt = listFreeCons(AInt)(elt);
			sfvSetDepths(syme, elt);
		}
		else {
			setcar(elt, sfvDepth);
			sfvConsTable(syme);
		}

		if (save) fsymes = listCons(Syme)(syme, fsymes);
	}

	listFree(Syme)(tbl);

	return save ? fvFrSymes(fsymes) : fvEmpty();
}

local void
sfvAddSyme(Syme syme)
{
	List(AInt)	elt = sfvGetDepths(syme);
	if (!sfvHasDepth(elt, sfvDepth)) {
		elt = listCons(AInt)(sfvDepth, elt);
		sfvSetDepths(syme, elt);
		sfvConsTable(syme);
	}
	sefoFreeDEBUG(dbOut, " Adding syme: %pSyme %pAIntList\n",
		      syme, sfvGetDepths(syme));
}

local void
sfvAddSymes(SymeList symes)
{
	for (; symes; symes = cdr(symes))
		if (symeIsSubstable(car(symes)))
			sfvAddSyme(car(symes));
}

local void
sfvDelSyme(Syme syme)
{
	List(AInt)	elt = sfvGetDepths(syme);
	if (sfvHasDepth(elt, sfvDepth)) {
		elt = listFreeCons(AInt)(elt);
		sfvSetDepths(syme, elt);
	}
	sefoFreeDEBUG(dbOut, " Del syme: %pSyme %pAIntList\n",
		      syme, sfvGetDepths(syme));
}

local void
sfvDelSymes(SymeList symes)
{
	for (; symes; symes = cdr(symes))
		if (symeIsSubstable(car(symes)))
			sfvDelSyme(car(symes));
}

local void
sfvDelStab(Stab stab)
{
	sfvDelSymes(stabGetBoundSymes(stab));
}

local Bool
sfvIsAncestor(TForm ancestor, TForm tf)
{
	while (tf) {
		if (tf == ancestor) return true;
		tf = tf->parent;
	}
	return false;
}

local TForm
sfvCommonAncestor(TForm tf1, TForm tf2)
{
	while (tf1) {
		if (sfvIsAncestor(tf1, tf2)) return tf1;
		tf1 = tf1->parent;
	}
	assert(false);
	return NULL;
}

/*
 * sstFreeVars local functions.
 */

local void
sefoFreeVars0(TForm *pa, TForm parent, Sefo sefo)
{
	Length	serial = 0;

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (DEBUG(sefoFree)) {
		afprintf(dbOut, "(sefoFree[%d]: %pAbSyn\n", (int) serial, sefo);
		sfvPrint(dbOut);
	}

	if (abIsLeaf(sefo)) {
		Syme	syme = abSyme(sefo);

		/*!! assert(syme); */
		if (!syme) return;

		if (symeIsSubstable(syme))
			sfvAddSyme(syme);
		symeUnboundVars0(pa, parent, syme);
	}
	else {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1)
			sefoUnboundVars0(pa, parent, abArgv(sefo)[i]);

		if (abState(sefo) == AB_State_HasUnique)
			tformUnboundVars0(pa, parent, abTUnique(sefo));
	}

	if (DEBUG(sefoFree)) {
		sfvPrint(dbOut);
		fprintf(dbOut, " sefoFree[%d]: %p)\n", (int) serial, *pa);
	}
}

local void
symeFreeVars0(TForm *pa, TForm parent, Syme syme)
{
	Length	serial = 0;

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (DEBUG(sefoFree)) {
		afprintf(dbOut, "(symeFree[%d]: %pSyme\n", (int) serial, syme);
		sfvPrint(dbOut);
	}

	sefoFreeDEBUG(dbOut, " symeFree[%d]: Self: %d Substable: %d\n",
		      (int) serial, symeIsSelf(syme),
		      (int) symeDefLevelIsSubstable(syme));
	if (symeIsImport(syme))
		tformUnboundVars0(pa, parent, symeExporter(syme));
	else if (!symeIsSelf(syme) &&
		 symeDefLevelIsSubstable(syme))
		tformUnboundVars0(pa, parent, symeType(syme));

	if (symeCondition(syme))
		sefoListUnboundVars0(pa, parent, symeCondition(syme));

	if (DEBUG(sefoFree)) {
		sfvPrint(dbOut);
		afprintf(dbOut, " symeFree[%d]: %pTForm)\n", (int) serial, *pa);
	}
}

local void
tformFreeVars0(TForm *pa, TForm parent, TForm tf)
{
	TForm		ancestor;
	Length		serial = 0;
	Bool		cascade = false;
	SymeList	symes;

	assert(tf);
	tf = tfFollowOnly(tf);
	if (tfIsDefine(tf) && tfIsWith(tfDefineVal(tf)))
		tf = tfDefineeType(tf);

	if (tfIsSubst(tf) && tfFVars(tf) == NULL) {
		TForm	arg = tfSubstArg(tf);
		if (sstTFormIsMarked(arg)) {
			*pa = sfvCommonAncestor(*pa, arg);
			return;
		}
		tformFreeVars(arg);
		tfSetFVars(tf, freeVarSubst0(tfSubstSigma(tf), tfFVars(arg)));
		sefoFreeDEBUG(dbOut, "FV(subst) %pTForm = %pFreeVar\n",
			      tf, tf->fv);
	}

	if (tfFVars(tf)) {
		sfvAddSymes(fvSymes(tfFVars(tf)));
		return;
	}

	if (sstTFormIsMarked(tf)) {
		*pa = sfvCommonAncestor(*pa, tf);
		return;
	}
	sstMarkTForm(tf);

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	sefoFreeDEBUG(dbOut, "(tformFree[%d]: %pTForm\n", (int) serial, tf);

	sfvPushTable();

	tf->parent = parent;
	ancestor = parent = tf;

	if (tfIsSyntax(tf))
		;
	else if (tfIsSym(tf))
		;
	else if (tfIsLibrary(tf) || tfIsArchive(tf))
		;
	else if (tfIsAbSyn(tf))
		sefoUnboundVars0(&ancestor, parent, tfGetExpr(tf));
	else if (tfIsWith(tf) && (symes = tfGetCatParents(tf, true)) != NULL) {
		tfGetCatSelf(tf);
		symeListUnboundVars0(&ancestor, parent, symes);
		tqualListFreeVars0(&ancestor, parent, tfGetCatCascades(tf));
		cascade = true;
	}
	else if (tfIsThird(tf) && (symes = tfGetThdParents(tf)) != NULL) {
		tfGetThdSelf(tf);
		symeListUnboundVars0(&ancestor, parent, symes);
		tqualListFreeVars0(&ancestor, parent, tfGetThdCascades(tf));
		cascade = true;
	}
	else if (tfIsNode(tf)) {
		Length	i;
		for (i = 0; i < tfArgc(tf); i += 1)
			tformUnboundVars0(&ancestor, parent, tfArgv(tf)[i]);
	}
	else
		bugBadCase(tfTag(tf));

	if (tfSymes(tf)) {
		if (!(tfIsLibrary(tf) || tfIsArchive(tf)))
			symeListUnboundVars0(&ancestor, parent, tfSymes(tf));
		if (tfIsDeclare(tf))
			sfvAddSymes(tfSymes(tf));
		else
			sfvDelSymes(tfSymes(tf));
	}
	if (tfQueries(tf))
		tformListUnboundVars0(&ancestor, parent, tfQueries(tf));
	if (!cascade && tfCascades(tf))
		tqualListUnboundVars0(&ancestor, parent, tfCascades(tf));

	if (tfStab(tf))
		sfvDelStab(tfStab(tf));
	if (tfSelf(tf))
		sfvDelSymes(tfSelf(tf));
	if (tfSelfSelf(tf))
		sfvDelSymes(tfSelfSelf(tf));

	if (ancestor == tf && tfIsMeaning(tf)) {
		tfSetFVars(tf, sfvPopTable(true));
		sefoFreeDEBUG(dbOut, "FV %pTForm = %pFreeVar\n", tf, tf->fv);
	}
	else {
		sfvPopTable(false);
		*pa = ancestor;
	}

	sefoFreeDEBUG(dbOut, " tformFree[%d]: %d %pTForm)\n",
		      (int) serial, tfIsMeaning(tf), *pa);
}

local void
tqualFreeVars0(TForm *pa, TForm parent, TQual tq)
{
	tformUnboundVars0(pa, parent, tqBase(tq));
	if (tqIsQualified(tq))
		tformListUnboundVars0(pa, parent, tqQual(tq));
}

local void
abSubFreeVars0(TForm *pa, TForm parent, AbSub sigma)
{
	AbBindList	l0;
	for (l0 = sigma->l; l0; l0 = cdr(l0))
		sefoUnboundVars0(pa, parent, car(l0)->val);
}

local void
sefoListFreeVars0(TForm *pa, TForm parent, SefoList sefos)
{
	for (; sefos; sefos = cdr(sefos))
		sefoUnboundVars0(pa, parent, car(sefos));
}

local void
symeListFreeVars0(TForm *pa, TForm parent, SymeList symes)
{
	for (; symes; symes = cdr(symes))
		symeUnboundVars0(pa, parent, car(symes));
}

local void
tformListFreeVars0(TForm *pa, TForm parent, TFormList tforms)
{
	for (; tforms; tforms = cdr(tforms))
		tformUnboundVars0(pa, parent, car(tforms));
}

local void
tqualListFreeVars0(TForm *pa, TForm parent, TQualList tquals)
{
	for (; tquals; tquals = cdr(tquals))
		tqualUnboundVars0(pa, parent, car(tquals));
}

local void
sefoUnboundVars0(TForm *pa, TForm parent, Sefo sefo)
{
	sefoFreeVars0(pa, parent, sefo);
	if (abStab(sefo))
		sfvDelStab(abStab(sefo));
}

local void
symeUnboundVars0(TForm *pa, TForm parent, Syme syme)
{
	symeFreeVars0(pa, parent, syme);
}

local void
tformUnboundVars0(TForm *pa, TForm parent, TForm tf)
{
	tfGetStab(tf);
	tformFreeVars0(pa, parent, tf);
}

local void
tqualUnboundVars0(TForm *pa, TForm parent, TQual tq)
{
	tqualFreeVars0(pa, parent, tq);
}

local void
sefoListUnboundVars0(TForm *pa, TForm parent, SefoList sefos)
{
	sefoListFreeVars0(pa, parent, sefos);
}

local void
symeListUnboundVars0(TForm *pa, TForm parent, SymeList symes)
{
	symeListFreeVars0(pa, parent, symes);
}

local void
tformListUnboundVars0(TForm *pa, TForm parent, TFormList tforms)
{
	tformListFreeVars0(pa, parent, tforms);
}

local void
tqualListUnboundVars0(TForm *pa, TForm parent, TQualList tquals)
{
	tqualListFreeVars0(pa, parent, tquals);
}

/*****************************************************************************
 *
 * :: sstSubst
 *
 ****************************************************************************/

/*
 * symeListSubst external entry points.
 */

AbSub
absFrSymes(Stab stab, SymeList symes, Sefo sefo)
{
	AbSub		sigma = absNew(stab);

	if (symes && symeIsSelf(car(symes))) absSetSelf(sigma);
	for (; symes; symes = cdr(symes))
		sigma = absExtend(car(symes), sefo, sigma);

	return sigma;
}

SymeList
symeListSubstSelf(Stab stab, TForm tf, SymeList osymes)
{
	AbSub		sigma = absFrSymes(stab, tfGetDomSelf(tf), tfExpr(tf));
	SymeList	nsymes, symes;

	nsymes = symeListSubstSigma(sigma, osymes);

	for (symes = osymes; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		Syme	nsyme = absGetSyme(sigma, syme);
		symeAddTwin(nsyme, syme);

		/* Now mutate the syme kind. */
		symeSetKind(nsyme, SYME_Import);
		symeSetExporter(nsyme, tf);

		if (tfIsLazyExporter(tf)) {
			symeSetLib(nsyme, symeLib(syme));
			symeSetHash(nsyme, symeHash(syme));
		}
		else if (!symeIsLazy(nsyme))
			symeSetHash(nsyme, (Hash) 0);
	}

	/* Modifies nsymes indirectly via sigma ... */
	symeListSubstUseConstants(sigma, tfGetDomSelf(tf), osymes,
				  tfGetDomConstants(tf));
	absFree(sigma);

	if (tfCascades(tf)) {
		/* Use a fresh sigma to avoid getting stomped-on symes. */
		sigma = absFrSymes(stab, tfGetDomSelf(tf), tfExpr(tf));
		tfCascades(tf) = tqualListSubst(sigma, tfCascades(tf));
		absFree(sigma);
	}

	return nsymes;
}

SymeList
symeListSubstCat(Stab stab, SymeList mods, TForm cat, SymeList osymes)
{
	AbSub		sigma;
	SymeList	nsymes, symes;

	if (!mods) return listCopy(Syme)(osymes);

	sigma = absFrSymes(stab, cdr(mods), abFrSyme(car(mods)));

	nsymes = symeListSubstSigma(sigma, osymes);

	for (symes = osymes; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		Syme	nsyme = absGetSyme(sigma, syme);
		symeAddTwin(nsyme, syme);
	}

	symeListSubstUseConstants(sigma, mods, osymes, tfGetCatConstants(cat));

	absFree(sigma);
	return nsymes;
}

SymeList
symeListSubstSigma(AbSub sigma, SymeList osymes)
{
	SymeList	symes;

	if (absIsEmpty(sigma)) {
		for (symes = listNil(Syme); osymes; osymes = cdr(osymes)) {
			Syme	syme = car(osymes);
			Syme	nsyme = symeCopy(syme);
			symes = listCons(Syme)(nsyme, symes);
			absSetSyme(sigma, syme, nsyme);
			absSetStab(sigma, nsyme);
		}
		return listNReverse(Syme)(symes);
	}

	symeListFreeVars(osymes);
	abSubFreeVars(sigma);

	/* Mark the symes so tformWillSubst can find them. */
	/* Mark the twins so that after the subst they become ==. */
	for (symes = osymes; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		absMarkSyme(sigma, syme);
		symeListMarkTwins(sigma, syme, syme);
	}

	/* Copy the symes that symeSubst doesn't need to. */
	for (symes = osymes; symes; symes = cdr(symes)) {
		Syme	syme = car(symes), nsyme;

		if (!symeWillSubst(sigma, syme)) {
			nsyme = symeCopy(syme);
			absSetSyme(sigma, syme, nsyme);
			absSetStab(sigma, nsyme);
		}
	}

	/* Now symeSubst0 will use the symes we copied, and copy the rest. */
	return symeListSubst0(sigma, osymes);
}

TForm
tformSubstSigma(AbSub sigma, TForm tf)
{
	AbBindList	l0;

	if (absIsEmpty(sigma)) return tf;

	tformFreeVars(tf);
	abSubFreeVars(sigma);

	/* Mark the symes in sigma which have symes as substituands. */
	for (l0 = sigma->l; l0; l0 = cdr(l0)) {
		Syme	syme = car(l0)->key;
		AbSyn	sefo = car(l0)->val;
		if (abSyme(sefo))
			absSetSyme(sigma, syme, abSyme(sefo));
	}

	/* Now tformSubst0 will use the symes we marked. */
	return tformSubst0(sigma, tf);
}

local void
symeListMarkTwins(AbSub sigma, Syme final, Syme twin)
{
	SymeList	symes;

	for (symes = symeTwins(twin); symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		absSetSyme(sigma, syme, final);
		symeListMarkTwins(sigma, final, syme);
	}
}

local void
symeListSubstUseConstants(AbSub sigma, SymeList mods, SymeList osymes,
			  SymeList rsymes)
{
	/*for (; osymes; osymes = cdr(osymes)) { */
	for (; rsymes && osymes; osymes = cdr(osymes)) {
		Syme		osyme = car(osymes);
		Syme		nsyme = absGetSyme(sigma, osyme);
		SymeList	symes;

		for (symes = rsymes; symes; symes = cdr(symes)) {
			Syme	rsyme = car(symes);
			if (symeId(osyme) == symeId(rsyme) &&
			    tformEqualMod(mods, symeType(osyme),
					  symeType(rsyme))) {
				symeTransferImplInfo(nsyme, rsyme);
				if (symeIsImport(nsyme))
					symeClrDefault(nsyme);
			}
		}
	}
}

/*
 * sstSubst external entry points.
 */

Sefo
sefoSubst(AbSub sigma, Sefo sefo)
{
	if (absIsEmpty(sigma)) return sefo;
	sefoFreeVars(sefo);
	abSubFreeVars(sigma);

	return sefoSubst0(sigma, sefo);
}

Syme
symeSubst(AbSub sigma, Syme syme)
{
	if (absIsEmpty(sigma)) return syme;
	symeFreeVars(syme);
	abSubFreeVars(sigma);

	return symeSubst0(sigma, syme);
}

TForm
tformSubst(AbSub sigma, TForm tf)
{
	if (absIsEmpty(sigma)) return tf;
	if (!tfIsPending(tf)) tformFreeVars(tf);
	abSubFreeVars(sigma);

	return tformSubst0(sigma, tf);
}

SymeList
symeListSubst(AbSub sigma, SymeList symes)
{
	if (absIsEmpty(sigma)) return symes;
	symeListFreeVars(symes);
	abSubFreeVars(sigma);

	return symeListSubst0(sigma, symes);
}

local TQualList
tqualListSubst(AbSub sigma, TQualList tquals)
{
	if (absIsEmpty(sigma)) return tquals;
	tqualListFreeVars(tquals);
	abSubFreeVars(sigma);

	return tqualListSubst0(sigma, tquals);
}

/*
 * The result from sefoSubst0 does not share Sefo nodes with sefo.
 */
local Sefo
sefoSubst0(AbSub sigma, Sefo sefo)
{
	Sefo	final;
	extern	Bool stabDumbImport(void);

	assert(sefo);

	if (DEBUG(sefoSubst)) {
		fprintf(dbOut, "-> sefoSubst[%ld]:\n", absSerial(sigma));
		sefoPrintDb(sefo);
	}

	if (abIsLeaf(sefo)) {
		Syme	syme = abSyme(sefo);

		/*!! assert(syme); */
		if (!syme)
			return sefoCopy(sefo);

		final = absLookup(syme, sefo, sigma);
		/* If absLookup fails, walk the syme. */
		if (final == sefo) {
			final = sefoCopy(final);
			abSetSyme(final, symeSubst0(sigma, syme));
		}
		else
			final = sefoCopy(final);
	}
	else {
		Length	i;
		final = abNewEmpty(abTag(sefo), abArgc(sefo));
		for (i = 0; i < abArgc(sefo); i++)
			abArgv(final)[i] = sefoSubst0(sigma, abArgv(sefo)[i]);

#ifdef SefoSubstTUnique
		/*!! This may have weaker semantics than we need.
		 *!! Instead of creating the semantics on the resulting form,
		 *!! which may yield more information, we are just
		 *!! using the semantics on the unsubstituted form.
		 */
		if (abState(sefo) == AB_State_HasUnique) {
			abState(final) = abState(sefo);
			abTUnique(final) = tformSubst0(sigma,abTUnique(sefo));
		}


		/* Copy semantics if they have some and we don't */
		if (abTForm(sefo) && !abTForm(final) && !stabDumbImport()) {
			/*
			 * Really not sure about this: without it we fail
			 * to process conditional imports correctly after
			 * calling tfDefineeBaseType() in stabImportFrom().
			 * One might expect that tformSubst0() ought to be
			 * used but this induces bad mutual recursion with
			 * this function. At least this way seems to work.
			 */
			abSetTForm(final, abTForm(sefo));
		}
#else
		if (abState(sefo) == AB_State_HasUnique)
			abState(final) = AB_State_AbSyn;
#endif
	}

	if (DEBUG(sefoSubst)) {
		fprintf(dbOut, "<- sefoSubst[%ld]:\n", absSerial(sigma));
		sefoPrintDb(final);
	}

	return final;
}

local Bool
tformSubstSkipCategory(TForm tf)
{
	if (tfIsAnyMap(tf)) tf = tfMapRet(tf);
	tf = tfDefineeTypeSubst(tf);

	/*!! The first if clause is really not quite right. */
	if (tfIsType(tf) || tfIsTypeSyntax(tf) || tfIsSyntax(tf))
		return true;

	if (tfIsCategory(tf) || tfIsCategorySyntax(tf) || tfIsThird(tf))
		return true;

	return false;
}

local Bool
tformSubstSkipDomain(TForm tf)
{
	if (tfIsAnyMap(tf)) tf = tfMapRet(tf);
	tf = tfDefineeTypeSubst(tf);

	/*!! The first if clause is really not quite right. */
	if (tfIsType(tf) || tfIsTypeSyntax(tf) || tfIsSyntax(tf))
		return true;

	if (tfIsWith(tf) || tfIsWithSyntax(tf) || tfIsIf(tf) ||
	    tfIsJoin(tf) || tfIsMeet(tf))
		return true;

	if (tfIsCategory(tf) || tfIsCategorySyntax(tf) || tfIsThird(tf))
		return true;

	if (tfIsId(tf) && tfIdSyme(tf) &&
	    tformSubstSkipCategory(symeType(tfIdSyme(tf))))
		return true;

	return false;
}

local Syme
symeSubst0(AbSub sigma, Syme syme)
{
	Syme	final;
	Bool	marked;

	assert(syme);

	final = absGetSyme(sigma, syme);
	marked = absSymeIsMarked(sigma, syme);
	if (final && !marked) {
		if (symeIsTwin(final, syme))
			return absSetSyme(sigma, syme,
					  symeSubst0(sigma, final));
		return final;
	}

	if (!symeWillSubst(sigma, syme))
		return absSetSyme(sigma, syme, syme);

	if (symeWillPush(syme)) symeType(syme);
	final = absSetSyme(sigma, syme, symeCopy(syme));
	absSetStab(sigma, final);

	if (DEBUG(sefoSubst)) {
		fprintf(dbOut, "-> symeSubst[%ld]:\n", absSerial(sigma));
		symePrintDb(syme);
	}

	if (symeIsImport(syme)) {
		symeSetExporter(final, tformSubst0(sigma, symeExporter(syme)));
#define SefoSubstImportType
#ifdef SefoSubstImportType
		if (tformWillSubst(sigma, symeType(syme)))
			symeSetType(final, tformSubst0(sigma, symeType(syme)));
#endif
	}

	else if (absSelf(sigma) && !symeIsSelf(syme) && !marked &&
		 tformSubstSkipDomain(symeType(syme)))
		;

	else if (!symeIsSelf(syme) && symeDefLevelIsSubstable(syme))
		symeSetType(final, tformSubst0(sigma, symeType(syme)));

	if (symeCondition(syme))
		symeSetCondition(final,
				 condListSubst0(sigma, symeCondition(syme)));

	if (DEBUG(sefoSubst)) {
		fprintf(dbOut, "<- symeSubst[%ld]: ", absSerial(sigma));
		symePrintDb(final);
	}

	return final;
}

local TForm
tformSubst0(AbSub sigma, TForm tf)
{
	TForm		final;
	Bool		lazy = absLazy(sigma);
	Bool		fresh = true;
	Bool		cascade = false;
	AbSyn		abnew;
	Length		i;
	SymeList	symes;

	assert(tf);
	absSetLazy(sigma);

	tf = tfFollowOnly(tf);
	if (tfIsDefine(tf) && tfIsWith(tfDefineVal(tf)))
		tf = tfDefineeType(tf);

	final = absGetTForm(sigma, tf);
	if (final && (!tfIsSubstOf(tf, final) || lazy)) return final;

	sefoSubstDEBUG(dbOut, "(-> tformSubst[%ld]: %pTForm\n",
		       absSerial(sigma), tf);

	if (!lazy) tfFollow(tf);

	if (tfFVars(tf) && !fvHasAbSub(tfFVars(tf), sigma)) {
		final = absSetTForm(sigma, tf, tf);
		fresh = false;
	}
	else if (tfIsSym(tf)) {
		final = absSetTForm(sigma, tf, tf);
		fresh = false;
	}
	else if (tfIsPending(tf)) {
		/*!! May need to print an error message here. */
		final = absSetTForm(sigma, tf, tf);
		fresh = false;
	}
	else if (tfIsLibrary(tf) || tfIsArchive(tf)) {
		final = absSetTForm(sigma, tf, tf);
		fresh = false;
	}
	else if (lazy) {
		final = tfSubst(sigma, tf);
		fresh = false;
	}
	else if (tfIsAbSyn(tf)) {
		TForm	tfnew;

		final = absSetTForm(sigma, tf, tfNewEmpty(TF_Forward, 1));
		final->argv[0] = tf;

		abnew = sefoSubst0(sigma, tfGetExpr(tf));
#if SefoSubstShare
		tfnew = tfFullFrAbSyn(absStab(sigma), abnew);
#else
		tfnew = tfMeaning(absStab(sigma), abnew,
				  tfPending(absStab(sigma), abnew));
#endif
		fresh = (tfGetExpr(tfnew) == abnew);

		if (fresh) tfOwnExpr(tfnew);

		final->argv[0] = tfnew;
		final = tfFollowOnly(final);
	}
	else if (tfIsWith(tf)) {
		final = tfNewEmpty(tfTag(tf), tfArgc(tf));
		final = absSetTForm(sigma, tf, final);
		tfArgv(final)[0] = tfNone();
		tfArgv(final)[1] = tfNone();
		symes = tfGetCatParents(tf, true);
		if (symes) {
			tfGetCatSelf(tf);
			tfParents(final) = parentListSubst0(sigma, symes);
			if (tfCatExports(tf))
				tfCatExports(final) =
					listCopy(Syme)(tfParents(final));
			tfCascades(final) =
				tqualListSubst0(sigma, tfGetCatCascades(tf));
			tfHasCascades(final) = true;
			cascade = true;
			tfAuditExportList(tfParents(final));
		}
		else {
			for (i = 0; i < tfArgc(tf); i += 1)
				tfArgv(final)[i] =
					tformSubst0(sigma, tfArgv(tf)[i]);
		}
		tfSetMeaning(final);
	}
	else if (tfIsThird(tf)) {
		final = tfNewEmpty(tfTag(tf), tfArgc(tf));
		final = absSetTForm(sigma, tf, final);
		tfArgv(final)[0] = tfNone();
		symes = tfGetThdParents(tf);
		if (symes) {
			tfGetThdSelf(tf);
			tfParents(final) = parentListSubst0(sigma, symes);
			tfCascades(final) =
				tqualListSubst0(sigma, tfGetCatCascades(tf));
			tfHasCascades(final) = true;
			cascade = true;
			tfAuditExportList(tfParents(final));
		}
		else {
			for (i = 0; i < tfArgc(tf); i += 1)
				tfArgv(final)[i] =
					tformSubst0(sigma, tfArgv(tf)[i]);
		}
		tfSetMeaning(final);
	}
	else if (tfIsRecord(tf) && tfRecordArgc(tf) == 1) {
		TForm	tf0, tfnew;
		Length	argc;

		final = absSetTForm(sigma, tf, tfNewEmpty(TF_Forward, 1));
		final->argv[0] = tf;

		tf0 = tformSubst0(sigma, tfArgv(tf)[0]);
		argc = tfAsMultiArgc(tf0);
		tfnew = tfNewEmpty(tfTag(tf), argc);
		for (i = 0; i < argc; i += 1)
			tfArgv(tfnew)[i] = tfAsMultiArgN(tf0, argc, i);
		tfGetSymes(absStab(sigma), tfnew, tfExpr(tfnew));
		tfSetMeaning(tfnew);

		final->argv[0] = tfnew;
		final = tfFollowOnly(final);
	}
	else if (tfIsRawRecord(tf) && tfRawRecordArgc(tf) == 1) {
		/*
		 * Don't know if we need this or not.
		 * Don't know if having it will break things.
		 */
		TForm	tf0, tfnew;
		Length	argc;

		final = absSetTForm(sigma, tf, tfNewEmpty(TF_Forward, 1));
		final->argv[0] = tf;

		tf0 = tformSubst0(sigma, tfArgv(tf)[0]);
		argc = tfAsMultiArgc(tf0);
		tfnew = tfNewEmpty(tfTag(tf), argc);
		for (i = 0; i < argc; i += 1)
			tfArgv(tfnew)[i] = tfAsMultiArgN(tf0, argc, i);
		tfGetSymes(absStab(sigma), tfnew, tfExpr(tfnew));
		tfSetMeaning(tfnew);

		final->argv[0] = tfnew;
		final = tfFollowOnly(final);
	}
	else if (tfIsNode(tf)) {
		final = tfNewEmpty(tfTag(tf), tfArgc(tf));
		final = absSetTForm(sigma, tf, final);
		for (i = 0; i < tfArgc(tf); i += 1)
			tfArgv(final)[i] =
				tformSubst0(sigma, tfArgv(tf)[i]);
		tfSetMeaning(final);
	}
	else {
		bugBadCase(tfTag(tf));
	}

	if (!fresh) {
		sefoSubstDEBUG(dbOut, " <- tformSubst[%ld]: %pTForm)\n",
			       absSerial(sigma), final);
		return final;
	}

	if (tfSymes(tf) && !tfSymes(final))
		tfSetSymes(final, symeListSubst0(sigma, tfSymes(tf)));

	if (tfQueries(tf))
		tfSetQueries(final,
			     tformListSubst0(sigma, tfQueries(tf)));
	if (!cascade && tfCascades(tf)) {
		tfSetCascades(final,
			      tqualListSubst0(sigma, tfCascades(tf)));
		tfHasCascades(final) = true;
	}

	if (tfGetStab(tf))
		tfSetStab(final, tfStab(tf));

	if (tfSelf(tf)) {
		SymeList ssymes;
		ssymes = listCopy(Syme)(tfSelf(tf));
		tfSetSelf(final, ssymes);
		tfHasSelf(final) = tfHasSelf(tf);
	}
	if (tfFVars(tf)) {
		tfSetFVars(final, freeVarSubst0(sigma, tfFVars(tf)));
		sefoFreeDEBUG(dbOut, "FV(subst) %pTForm = %pFreeVar\n",
			      final, final->fv);
	}
	if (tfHasExpr(final)) tfSetNeedsSefo(final);

	sefoSubstDEBUG(dbOut, "<- tformSubst[%ld]: %pTForm)\n",
		       absSerial(sigma), final);

	return final;
}

local TQual
tqualSubst0(AbSub sigma, TQual tq)
{
	return tqNewFrList(tformSubst0(sigma, tqBase(tq)),
			   tformListSubst0(sigma, tqQual(tq)));
}

local FreeVar
freeVarSubst0(AbSub sigma, FreeVar fv)
{
	FreeVar		fv0 = absFVars(sigma);
	SymeList	symes, final;

	if (fvSymes(fv) == listNil(Syme))
		return fv0;

	final = listReverse(Syme)(fvSymes(fv0));
	for (symes = fvSymes(fv); symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		if (absLookup(syme, NULL, sigma))
			continue;
		syme = symeSubst0(sigma, syme);
		if (!fvHasSyme(fv0, syme))
			final = listCons(Syme)(syme, final);
	}
	final = listNReverse(Syme)(final);
	return fvFrSymes(final);
}

/* Make sure that the symes are not going to be stomped on. */
local SymeList
parentListSubst0(AbSub sigma, SymeList symes)
{
	if (absSelf(sigma))
		return listCopy(Syme)(symes);
	else
		return symeListSubst0(sigma, symes);
}

local SefoList
condListSubst0(AbSub sigma, SefoList conds)
{
	Bool		self  = absSelf(sigma);
	SefoList	final = listNil(Sefo);

	for (; conds; conds = cdr(conds)) {
		Sefo	cond = car(conds);
		Sefo	expr, prop;

		if (abHasTag(cond, AB_Has)) {
			expr = car(conds)->abHas.expr;
			prop = car(conds)->abHas.property;

			expr = sefoSubst0(sigma, expr);
			prop = self ? sefoCopy(prop) : sefoSubst(sigma, prop);

			cond = abNewHas(sposNone, expr, prop);
		}
		else
			cond = sefoSubst0(sigma, cond);

		final = listCons(Sefo)(cond, final);
	}

	return listNReverse(Sefo)(final);
}

local SymeList
symeListSubst0(AbSub sigma, SymeList symes)
{
	SymeList	final = listNil(Syme);

	for (; symes; symes = cdr(symes))
		final = listCons(Syme)(symeSubst0(sigma, car(symes)),
					final);

	return listNReverse(Syme)(final);
}

local TFormList
tformListSubst0(AbSub sigma, TFormList tforms)
{
	TFormList	final = listNil(TForm);

	for (; tforms; tforms = cdr(tforms))
		final = listCons(TForm)(tformSubst0(sigma, car(tforms)),
					 final);

	return listNReverse(TForm)(final);
}

local TQualList
tqualListSubst0(AbSub sigma, TQualList tquals)
{
	TQualList	final = listNil(TQual);

	for (; tquals; tquals = cdr(tquals))
		final = listCons(TQual)(tqualSubst0(sigma, car(tquals)),
					 final);

	return listNReverse(TQual)(final);
}

/* Return true if symeSubst0 must explicitly push a lazy syme. */
local Bool
symeWillPush(Syme syme)
{
	Bool	result = false;

	if (symeIsLazy(syme)) {
		switch (symeKind(syme)) {
		case SYME_Export:
		case SYME_Param:
			if (symeDefLevelIsSubstable(syme))
				result = true;
			break;
		case SYME_Import: {
			TForm	exporter = symeExporter(syme);
			if (!tfIsLazyExporter(exporter))
				result = true;
			break;
		}
		default:
			break;
		}
	}

	return result;
}

/* Return true if symeSubst0 will create a new syme. */
local Bool
symeWillSubst(AbSub sigma, Syme syme)
{
	assert(syme);

	if (symeCondition(syme))
		return true;
	else if (symeIsImport(syme) &&
		 tformWillSubst(sigma, symeExporter(syme)))
		return true;
	else if (!symeIsImport(syme) &&
		 absSelf(sigma) && !symeIsSelf(syme) &&
		 !absSymeIsMarked(sigma, syme) &&
		 tformSubstSkipDomain(symeType(syme)))
		return false;
	else if (!symeIsImport(syme) &&
		 !symeIsSelf(syme) &&
		 symeDefLevelIsSubstable(syme) &&
		 tformWillSubst(sigma, symeType(syme)))
		return true;
	else
		return false;
}

/* Return true if tformSubst0 will create a new tform. */
local Bool
tformWillSubst(AbSub sigma, TForm tf)
{
	TForm	final;

	assert(tf);

	tf = tfFollowOnly(tf);

	final = absGetTForm(sigma, tf);
	if (final && (!tfIsSubstOf(tf, final) || absLazy(sigma)))
		return tf != final;

	if (tfFVars(tf) && !fvHasAbSub(tfFVars(tf), sigma))
		return false;

	else if (tfIsSym(tf))
		return false;

	else if (tfIsLibrary(tf) || tfIsArchive(tf))
		return false;

	else if (tfIsPending(tf))
		return false;

	else
		return true;
}

/*****************************************************************************
 *
 * :: symeListClosure
 *
 ****************************************************************************/

local void	sefoClosureReleaseType	(Sefo);
/*
 * External entry points.
 */

Bool
symeCloseOverDetails(Syme syme)
{
	if (symeIsLibrary(syme) || symeIsArchive(syme))
		return false;

	if (symeLib(syme) == NULL)
		return true;

	if (symeConstLib(syme) == NULL)
		return true;

	if (symeIsExport(syme) 
	    && symeConstLib(symeOriginal(syme)) != symeConstLib(syme)
	    && symeConstNum(syme) != SYME_NUMBER_UNASSIGNED)
		return true;

	if (symeIsExtend(syme) && syme->lib != NULL)
		return false;

	if (symeIsExport(syme) || symeIsParam(syme) || symeIsSelf(syme))
		return false;

	if (symeIsImport(syme)) {
		TForm	exporter = symeExporter(syme);
		if (tfIsLazyExporter(exporter))
			return false;
	}

	return true;
}

void
symeListClosure(Lib lib, SymeList symes)
{
	SymeList	sl0, sl;

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, "symeListClosure: %d symes", (int) listLength(Syme)(symes));
		fnewline(dbOut);
		symeListPrintDb(symes);
	}

	lib->topc  = 0;
	lib->symec = 0;
	lib->symes = 0;
	lib->symev = 0;

	lib->typec = 0;
	lib->types = 0;
	lib->typev = 0;

	/* Collect the symes and types, putting the original list first. */
	sstNext("symeListClosure", lib, symes);
	for (sl = symes; sl; sl = cdr(sl))
		slcAddSyme(lib, car(sl));
	sl0 = lib->symes;
	for (sl = sl0; sl; sl = cdr(sl))
		symeClosure1(lib, car(sl));
	sstDoneSymeList(sl0);

	/* Cache the symes/types in the lib. */
	lib->symes = listNReverse(Syme)(lib->symes);
	lib->types = listNReverse(TForm)(lib->types);

	sefoCloseDEBUG(dbOut, " -> %d symes\n",
		       (int) listLength(Syme)(lib->symes));
}

/*
 * Local functions.
 */

local void
slcAddSyme(Lib lib, Syme syme)
{
	if (sstSymeIsMarked(syme))
		return;

	sstMarkSyme(syme);
	lib->symes = listCons(Syme)(syme, lib->symes);
	lib->symec += 1;
	if (libSymeIsTop(lib, syme)) lib->topc += 1;

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, "+syme:");
		fnewline(dbOut);
		symePrintDb(syme);
	}
}

local void
slcAddType(Lib lib, TForm tf)
{
	lib->types = listCons(TForm)(tf, lib->types);
	lib->typec += 1;

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, "+type:");
		fnewline(dbOut);
		tformPrintDb(tf);
	}
}

local void
sefoClosure0(Lib lib, Sefo sefo)
{
	Length	serial = 0;

	assert(sefo);

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, "(S ");
		sefoPrintDb(sefo);
	}

	if (abIsLeaf(sefo)) {
		if (abSyme(sefo))
			symeClosure0(lib, abSyme(sefo));
		if (abImplicit(sefo))
			sefoClosure0(lib, abImplicit(sefo));
	}
	else {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1)
			sefoClosure0(lib, abArgv(sefo)[i]);
		if (abState(sefo) == AB_State_HasUnique) {
#ifdef Bug1075
			abState(sefo) = AB_State_AbSyn;
#endif
			sefoClosureReleaseType(sefo);
		}
	}

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, " S)");
		sefoPrintDb(sefo);
	}
}

local void
sefoClosureReleaseType(Sefo sefo)
{
	abTUnique(sefo) = tfUnknown;
}

/*
 * Close using the given syme as a root, adding it to the lib.
 */
local void
symeClosure0(Lib lib, Syme syme)
{
	if (sstSymeIsMarked(syme))
		return;

	slcAddSyme(lib, syme);
	symeClosure1(lib, syme);
}

/*
 * Close using the given syme as a root, even though it is marked already.
 */
local void
symeClosure1(Lib lib, Syme syme)
{
	Length		serial = 0;
	Syme		extension = NULL;
	SymeList	extendee = NULL, symes;
	Bool		details = symeCloseOverDetails(syme);

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, "([%d]:\t", (int) serial);
		symePrintDb(syme);
		if (sstSerialDebug % 25 == 0)
			fprintf(dbOut, "\n");
	}

	assert(syme);
	if (symeIsExtend(syme)) {
		if (symeExtension(syme))
			symeClosure0(lib, symeExtension(syme));
		extension = syme;
		extendee = symeExtendee(extension);
	}
	else if (symeExtension(syme)) {
		extension = symeExtension(syme);
		if (symeIsExtend(extension))
			extendee = symeExtendee(extension);
	}
	for (symes = extendee; details && symes; symes = cdr(symes))
		slcAddSyme(lib, car(symes));
	symeListSetExtension(extendee, NULL);
	symeSetExtension(syme, NULL);

	if (symeLib(syme))
		symeClosure0(lib, libLibrarySyme(symeLib(syme)));
	if (symeConstLib(syme))
		symeClosure0(lib, libLibrarySyme(symeConstLib(syme)));

	if (symeIsImport(syme))
		tformClosure0(lib, symeExporter(syme));
	else if (details == false)
		;
	else if (symeIsSelf(syme))
		tformClosure0(lib, tfType);
	else {
		TForm	tf = symeType(syme);
		tformClosure0(lib, tf);
		if (symeTop(syme) && !tfIsAnyMap(tf) &&
		    fvCount(tfFVars(tf)) > 1)
			fvSetSkipParam(tfFVars(tf));
	}

	if (details) {
		sefoListClosure0(lib, symeCondition(syme));
		symeListClosure0(lib, symeTwins(syme));

		if (!symeInlined(syme)) 
			symeSetInlined(syme, genGetSymeInlined(syme));

		if (symeInlined(syme)) 
			symeListClosure0(lib, symeInlined(syme));

		if (extension)
			symeClosure0(lib, extension);
		for (symes = extendee; symes; symes = cdr(symes))
			symeClosure1(lib, car(symes));
	}

	symeListSetExtension(extendee, extension);

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, " [%d])", (int) serial);
		symePrintDb(syme);
	}

	return;
}

local void
tformClosure0(Lib lib, TForm tf)
{
	Length	serial = 0;
	Bool	cascade = false;

	assert(tf);

	tfFollow(tf);

	if (sstTFormIsMarked(tf))
		return;
	sstMarkTForm(tf);

	sstSerialDebug += 1;
	serial = sstSerialDebug;

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, "(T ");
		tformPrintDb(tf);
	}

	if (tfFVars(tf) == NULL)
		tformFreeVars(tf);

	if (tfSatDom(tf)) {
		tfGetCatSelf(tf);
		tfGetCatSelfSelf(tf);
	}

	if (tfIsWith(tf) && tfUseCatExports(tf)) {
		SymeList catExports = tfGetCatExports(tf);
		symeListClosure0(lib, catExports);
		tqualListClosure0(lib, tfCascades(tf));
		while (catExports != listNil(Syme)) {
			sefoListClosure0(lib, symeCondition(car(catExports)));
			catExports = cdr(catExports);
		}
		cascade = true;
	}
	if (tfIsThird(tf) && tfUseThdExports(tf)) {
		symeListClosure0(lib, tfGetThdExports(tf));
		tqualListClosure0(lib, tfCascades(tf));
		cascade = true;
	}

	if (tfIsSym(tf))
		;
	else if (tfIsAbSyn(tf)) {
		sefoClosure0(lib, tfGetExpr(tf));
		tfSetNeedsSefo(tf);
	}
	else if (tfIsNode(tf)) {
		Length	i;
		for (i = 0; i < tfArgc(tf); i += 1) {
			tfFollow(tfArgv(tf)[i]);
			tformClosure0(lib, tfArgv(tf)[i]);
		}
	}
	else
		bugBadCase(tfTag(tf));

	slcAddType(lib, tf);

	if (tfSymes(tf))
		symeListClosure0(lib, tfSymes(tf));
	if (tfSelf(tf))
		symeListClosure0(lib, tfSelf(tf));
	if (tfSelfSelf(tf))
		symeListClosure0(lib, tfSelfSelf(tf));
	if (tfQueries(tf))
		tformListClosure0(lib, tfQueries(tf));
	if (!cascade && tfCascades(tf))
		tqualListClosure0(lib, tfCascades(tf));

	if (DEBUG(sefoClose)) {
		fprintf(dbOut, " T)");
		tformPrintDb(tf);
	}
}

local void
tqualClosure0(Lib lib, TQual tqual)
{
	tfFollow(tqBase(tqual));
	tformClosure0(lib, tqBase(tqual));
	if (tqIsQualified(tqual))
		tformListClosure0(lib, tqQual(tqual));
}

local void
sefoListClosure0(Lib lib, SefoList sefos)
{
	for ( ; sefos; sefos = cdr(sefos))
		sefoClosure0(lib, car(sefos));
}

local void
symeListClosure0(Lib lib, SymeList symes)
{
	for ( ; symes; symes = cdr(symes))
		symeClosure0(lib, car(symes));
}

local void
tformListClosure0(Lib lib, TFormList tforms)
{
	for ( ; tforms; tforms = cdr(tforms)) {
		tfFollow(car(tforms));
		tformClosure0(lib, car(tforms));
	}
}

local void
tqualListClosure0(Lib lib, TQualList tquals)
{
	for ( ; tquals; tquals = cdr(tquals))
		tqualClosure0(lib, car(tquals));
}

/*****************************************************************************
 *
 * :: symeList set operations
 *
 ****************************************************************************/

Bool
symeListMember(Syme syme, SymeList symes, SymeEqFun eq)
{
	for (; symes; symes = cdr(symes)) {
		/* The extra '==' check saves a lot of funcalls on symeEq. */
		if (syme == car(symes))
			return true;
		if (eq != symeEq && eq(syme, car(symes)))
			return true;
	}
	return false;
}

/* Return true if each syme in symes1 is a member of symes2. */
Bool
symeListSubset(SymeList symes1, SymeList symes2, SymeEqFun eq)
{
	for (; symes1; symes1 = cdr(symes1))
		if (!symeListMember(car(symes1), symes2, eq))
			return false;
	return true;
}

SymeList
symeListUnion(SymeList symes1, SymeList symes2, SymeEqFun eq)
{
	SymeList	result = listNil(Syme);

	if (DEBUG(sefoUnion)) {
		fprintf(dbOut, "symeListUnion: \n");
		fprintf(dbOut, "    symes1: ");
		symeListPrintDb(symes1);
		fprintf(dbOut, "    symes2: ");
		symeListPrintDb(symes2);
	}

	for (; symes1; symes1 = cdr(symes1))
		if (!symeListMember(car(symes1), symes2, eq))
			result = listCons(Syme)(car(symes1), result);

	result = listNConcat(Syme)(listNReverse(Syme)(result),
				   listCopy(Syme)(symes2));

	if (DEBUG(sefoUnion)) {
		fprintf(dbOut, "symeListUnion result: ");
		symeListPrintDb(result);
	}

	return result;
}

SymeList
symeListIntersect(SymeList symes1, SymeList symes2, SymeEqFun eq)
{
	SymeList	result = listNil(Syme);

	if (DEBUG(sefoInter)) {
		fprintf(dbOut, "symeListIntersect: \n");
		fprintf(dbOut, "    symes1: ");
		symeListPrintDb(symes1);
		fprintf(dbOut, "    symes2: ");
		symeListPrintDb(symes2);
	}

	for (; symes1; symes1 = cdr(symes1))
		if (symeListMember(car(symes1), symes2, eq))
			result = listCons(Syme)(car(symes1), result);

	result = listNReverse(Syme)(result);

	if (DEBUG(sefoInter)) {
		fprintf(dbOut, "symeListIntersect result: ");
		symeListPrintDb(result);
	}

	return result;
}

/*****************************************************************************
 *
 * :: sstToBuffer
 *
 ****************************************************************************/

int
sefoToBuffer(Lib lib, Buffer buf, Sefo sefo)
{
	Length		start = bufPosition(buf);
	AbSynTag	tag = abTag(sefo);
	UShort		i, argc;

	bufPutByte(buf, tag);

	switch (tag) {
	case AB_Nothing:
	case AB_Blank:
		break;

	case AB_IdSy:
		bugUnimpl("sefoToBuffer:  abIdSy");
		break;

	case AB_Id:
		assert(abSyme(sefo));
		symeToBuffer(lib, buf, abSyme(sefo));
		break;

	case AB_LitInteger:
	case AB_LitString:
	case AB_LitFloat:
		assert(abSyme(sefo));
		symeToBuffer(lib, buf, abSyme(sefo));
		bufWrString(buf, abLeafStr(sefo));
		break;

	case AB_Lambda:  /* Lies, damn lies, and leaving out the body */
		sefoToBuffer(lib, buf, sefo->abLambda.param);
		sefoToBuffer(lib, buf, sefo->abLambda.rtype);
		break;
	default:
		argc = abArgc(sefo);
		bufPutHInt(buf, argc);
		for (i = 0; i < argc; i += 1)
			sefoToBuffer(lib, buf, abArgv(sefo)[i]);
	}

	return bufPosition(buf) - start;
}

int
symeToBuffer(Lib lib, Buffer buf, Syme syme)
{
	UShort	n = symeLibNum(syme);

	assert(libCheckSymeNumber(lib, syme, n));
	bufPutHInt(buf, n);

	return HINT_BYTES;
}

local int
tformToBuffer1(Lib lib, Buffer buf, TForm tf)
{
	ULong	n = tf->libNum;

	assert(libCheckTypeNumber(lib, tf, n));
	bufPutSInt(buf, n);

	return SINT_BYTES;
}

int
tformToBuffer(Lib lib, Buffer buf, TForm tf)
{
	Length		start = bufPosition(buf);
	TFormTag	tag = tfTag(tf);
	UShort		i, argc;

	if (tfHasSelf(tf))	tag |= 0x80;
	if (tfHasCascades(tf))	tag |= 0x40;
	bufPutByte(buf, tag);
	tag &= 0x3f;

	if (tfIsSymTag(tag))
		;

	else if (tfIsAbSynTag(tag))
		sefoToBuffer(lib, buf, tfGetExpr(tf));

	else if (tfIsNodeTag(tag)) {
		argc = tfArgc(tf);
		bufPutHInt(buf, argc);
		for (i = 0; i < argc; i += 1)
			tformToBuffer1(lib, buf, tfArgv(tf)[i]);
	}

	else
		bugBadCase(tag);

	symeListToBuffer (lib, buf, tfSelf(tf));
	symeListToBuffer (lib, buf, tfSelfSelf(tf));
	tformListToBuffer(lib, buf, tfQueries(tf));
	tqualListToBuffer(lib, buf, tfCascades(tf));

	if (tfTagHasSymes(tfTag(tf)))
		symeListToBuffer(lib, buf, tfSymes(tf));

	if (tfIsWith(tf)) {
		if (tfUseCatExports(tf)) {
			SymeList l;
			symeListToBuffer(lib, buf, tfGetCatExports(tf));
			l = tfGetCatExports(tf);
			bufPutHInt(buf, listLength(Syme)(l));
			while (l != listNil(Syme)) {
				if (symeIsSelfSelf(car(l)))
					sefoListToBuffer(lib, buf, listNil(Sefo));
				else
					sefoListToBuffer(lib, buf, symeCondition(car(l)));
				l = cdr(l);
			}
			
		}
		else {
			bufPutHInt(buf, 0);
			symeListToBuffer(lib, buf, listNil(Syme));
		}
	}

	if (tfIsThird(tf)) {
		if (tfUseThdExports(tf))
			symeListToBuffer(lib, buf, tfGetThdExports(tf));
		else
			symeListToBuffer(lib, buf, listNil(Syme));
	}

	if (tfFVars(tf) == NULL)
		tformFreeVars(tf);

	assert(tfFVars(tf) != NULL);
	/*afprintf(dbOut, "TF: %pPtr %pTForm FreeVars: %pSymeList\n", tf, tf, tf->fv->symes);*/

	freeVarToBuffer(lib, buf, tfFVars(tf));

	return bufPosition(buf) - start;
}

int
tqualToBuffer(Lib lib, Buffer buf, TQual tq)
{
	Length		start = bufPosition(buf);

	tformToBuffer1(lib, buf, tqBase(tq));
	tformListToBuffer(lib, buf, tqQual(tq));

	return bufPosition(buf) - start;
}

int
sefoListToBuffer(Lib lib, Buffer buf, SefoList sefos)
{
	Length		start = bufPosition(buf);
	UShort		sefoc = listLength(Sefo)(sefos);

	bufPutHInt(buf, sefoc);

	for (; sefos; sefos = cdr(sefos))
		sefoToBuffer(lib, buf, car(sefos));

	return bufPosition(buf) - start;
}

int
symeListToBuffer(Lib lib, Buffer buf, SymeList symes)
{
	Length		start = bufPosition(buf);
	UShort		symec = listLength(Syme)(symes);

	bufPutHInt(buf, symec);

	for (; symes; symes = cdr(symes))
		symeToBuffer(lib, buf, car(symes));

	return bufPosition(buf) - start;
}

int
tformListToBuffer(Lib lib, Buffer buf, TFormList tforms)
{
	Length		start = bufPosition(buf);
	UShort		tformc = listLength(TForm)(tforms);

	bufPutHInt(buf, tformc);

	for (; tforms; tforms = cdr(tforms))
		tformToBuffer1(lib, buf, car(tforms));

	return bufPosition(buf) - start;
}

int
tqualListToBuffer(Lib lib, Buffer buf, TQualList tquals)
{
	Length		start = bufPosition(buf);
	UShort		tqualc = listLength(TQual)(tquals);

	bufPutHInt(buf, tqualc);
	for (; tquals; tquals = cdr(tquals))
		tqualToBuffer(lib, buf, car(tquals));

	return bufPosition(buf) - start;
}

local int
freeVarToBuffer(Lib lib, Buffer buf, FreeVar fv)
{
	Length		start = bufPosition(buf);
	UShort		symec;
	SymeList	symes;
	Bool		skipParams = fvSkipParam(fv);

	/* Count the number of symes to put. */
	for (symec = 0, symes = fvSymes(fv); symes; symes = cdr(symes)) {
		if (skipParams && symeIsParam(car(symes))) continue;
		if (libHasSyme(lib, car(symes)))
			symec += 1;
	}

	bufPutHInt(buf, symec);

	for (symes = fvSymes(fv); symes; symes = cdr(symes)) {
		if (skipParams && symeIsParam(car(symes))) continue;
		if (libHasSyme(lib, car(symes)))
			symeToBuffer(lib, buf, car(symes));
	}

	return bufPosition(buf) - start;
}

/*****************************************************************************
 *
 * :: sstFrBuffer
 *
 ****************************************************************************/

Sefo
sefoFrBuffer(Lib lib, Buffer buf)
{
	Sefo		sefo, sefo1;
	Syme		syme;
	AbSynTag	tag;
	UShort		i, argc;

	tag = bufGetByte(buf);

	switch (tag) {
	case AB_Nothing:
		sefo = abNewNothing(sposNone);
		break;
	case AB_Blank:
		sefo = abNewBlank(sposNone, ssymVariable);
		break;
	case AB_IdSy:
		bugUnimpl("sefoFrBuffer:  abIdSy");
		NotReached(sefo = NULL);
		break;
	case AB_Id:
		syme = symeFrBuffer(lib, buf);
		sefo = abNewId(sposNone, symeId(syme));
		abSetSyme(sefo, syme);
		break;
	case AB_LitInteger:
		syme = symeFrBuffer(lib, buf);
		sefo = abNewLitInteger(sposNone, bufRdString(buf));
		abSetSyme(sefo, syme);
		break;
	case AB_LitString:
		syme = symeFrBuffer(lib, buf);
		sefo = abNewLitString(sposNone, bufRdString(buf));
		abSetSyme(sefo, syme);
		break;
	case AB_LitFloat:
		syme = symeFrBuffer(lib, buf);
		sefo = abNewLitFloat(sposNone, bufRdString(buf));
		abSetSyme(sefo, syme);
		break;
	case AB_Lambda: {
		Sefo body;
		sefo1 = sefoFrBuffer(lib, buf);
		sefo  = sefoFrBuffer(lib, buf);
		body = abNewNever(sposNone);
		abUse(body) = AB_Use_Elided;
		sefo  = abNewLambda(sposNone, sefo1, sefo, body);
		}
		break;
	default:
		argc = bufGetHInt(buf);
		sefo = abNewEmpty(tag, argc);
		for (i = 0; i < argc; i += 1)
			abArgv(sefo)[i] = sefoFrBuffer(lib, buf);
	}

	return sefo;
}

Syme
symeFrBuffer(Lib lib, Buffer buf)
{
	UShort	n;

	n = bufGetHInt(buf);
	assert(libCheckSymeNumber(lib, NULL, n));

	return lib->symev[n];
}

local TForm
tformFrBuffer1(Lib lib, Buffer buf)
{
	ULong	n;

	n = bufGetSInt(buf);
	assert(libCheckTypeNumber(lib, NULL, n));

	/* The type forms are filled in later. */
	return (TForm) n;
}

TForm
tformFrBuffer(Lib lib, Buffer buf)
{
	TForm		tf;
	TFormTag	tag;
	UShort		i, argc;
	Bool		hasSelf = false, hasCascades = false;

	tag = bufGetByte(buf);
	if (tag & 0x80) hasSelf = true;
	if (tag & 0x40) hasCascades = true;
	tag &= 0x3f;

	if (tfIsSymTag(tag))
		switch (tag) {
		case TF_Unknown:
			tf = tfUnknown;
			break;
		case TF_Exit:
			tf = tfExit;
			break;
		case TF_Test:
			tf = tfTest;
			break;
		case TF_Literal:
			tf = tfLiteral;
			break;
		case TF_Type:
			tf = tfType;
			/* TTT */
			return tf;
			break;
		case TF_Category:
			tf = tfCategory;
			break;
		default:
			bugBadCase(tag);
			NotReached(tf = NULL);
		}

	else if (tfIsAbSynTag(tag)) {
		Sefo	sefo = sefoFrBuffer(lib, buf);
		Syme	syme = abSyme(sefo);

		if (syme && syme->kind == SYME_Library)
			tf = tfGetLibrary(syme);
		else if (syme && syme->kind == SYME_Archive)
			tf = tfGetArchive(syme);
		else {
			tf = tfNewAbSyn(tag, sefo);
			tf->intStepNo = 0;
			tfOwnExpr(tf);
		}
	}

	else if (tfIsNodeTag(tag)) {
		argc = bufGetHInt(buf);
		tf = tfNewEmpty(tag, argc);
		tf->intStepNo = 0;
		for (i = 0; i < argc; i += 1)
			tfArgv(tf)[i] = tformFrBuffer1(lib, buf);
	}

	else {
		bugBadCase(tag);
		NotReached(tf = NULL);
	}

	tfSetSelf(tf, symeListFrBuffer(lib, buf));
	tfSetSelfSelf(tf, symeListFrBuffer(lib, buf));
	tfSetQueries(tf, tformListFrBuffer(lib, buf));
	tfSetCascades(tf, tqualListFrBuffer(lib, buf));

	tfHasSelf(tf) = hasSelf;
	tfHasCascades(tf) = hasCascades;

	if (tfTagHasSymes(tfTag(tf)))
		tfSetSymes(tf, symeListFrBuffer(lib, buf));

	if (tfIsWith(tf)) {
		SymeList symes;
		int n;
		tfCatExports(tf) = symeListFrBuffer(lib, buf);
		n = bufGetHInt(buf);
		symes = tfCatExports(tf);
		if (n != listLength(Syme)(symes)) {
			bug("incorrect number of exports");
		}
		while (symes != listNil(Syme)) {
			SefoList condition = sefoListFrBuffer(lib, buf);
			symeSetCondition(car(symes), condition);
			symes = cdr(symes);
		}
	}
	if (tfIsThird(tf))
		tfThdExports(tf) = symeListFrBuffer(lib, buf);

	tfSetFVars(tf, fvFrSymes(symeListFrBuffer(lib, buf)));

	return tf;
}

TQual
tqualFrBuffer(Lib lib, Buffer buf)
{
	TForm		base = tformFrBuffer1(lib, buf);
	TFormList	qual = tformListFrBuffer(lib, buf);

	return tqNewFrList(base, qual);
}

SefoList
sefoListFrBuffer(Lib lib, Buffer buf)
{
	SefoList	sefos = listNil(Sefo);
	UShort		i, sefoc;

	sefoc = bufGetHInt(buf);

	for (i = 0; i < sefoc; i += 1) {
		Sefo sefo = sefoFrBuffer(lib, buf);
		sefos = listCons(Sefo)(sefo, sefos);
	}
	return listNReverse(Sefo)(sefos);
}

SymeList
symeListFrBuffer(Lib lib, Buffer buf)
{
	SymeList	symes = listNil(Syme);
	UShort		i, symec;

	symec = bufGetHInt(buf);

	for (i = 0; i < symec; i += 1)
		symes = listCons(Syme)(symeFrBuffer(lib, buf), symes);

	return listNReverse(Syme)(symes);
}

TFormList
tformListFrBuffer(Lib lib, Buffer buf)
{
	TFormList	tforms = listNil(TForm);
	UShort		i, tformc;

	tformc = bufGetHInt(buf);

	for (i = 0; i < tformc; i += 1)
		tforms = listCons(TForm)(tformFrBuffer1(lib, buf), tforms);

	return listNReverse(TForm)(tforms);

}

TQualList
tqualListFrBuffer(Lib lib, Buffer buf)
{
	TQualList	tquals = listNil(TQual);
	UShort		i, tqualc;

	tqualc = bufGetHInt(buf);

	for (i = 0; i < tqualc; i += 1)
		tquals = listCons(TQual)(tqualFrBuffer(lib, buf), tquals);

	return listNReverse(TQual)(tquals);

}

/*****************************************************************************
 *
 * :: sstFrBuffer0
 *
 ****************************************************************************/

/*
 * Read the number of types from the type form section in a buffer.
 */
int
tformTypecFrBuffer(Buffer buf)
{
	ULong	argc;

	bufStart(buf);
	argc = bufGetSInt(buf);

	return argc;
}

/*
 * Compute the positions of the types from the type form section in a buffer.
 */
void
tformTypepFrBuffer(Buffer buf, int posc, int *posv)
{
	int	i;

	for (i = 0; i < posc; i += 1)
		posv[i] = 0;

	/* Skip over the number of type forms. */
	tformTypecFrBuffer(buf);

	for (i = 0; i < posc; i += 1) {
		posv[i] = bufPosition(buf);
		tformFrBuffer0(buf);
	}
}

local void
sefoFrBuffer0(Buffer buf)
{
	AbSynTag	tag;
	UShort		i, argc;
	ULong		cc;

	tag = bufGetByte(buf);

	switch (tag) {
	case AB_Nothing:
	case AB_Blank:
	case AB_IdSy:
		break;

	case AB_Id:
		symeFrBuffer0(buf);
		break;

	case AB_LitInteger:
	case AB_LitString:
	case AB_LitFloat:
		symeFrBuffer0(buf);
		cc = bufGetSInt(buf);
		bufSkip(buf, cc);
		break;

	case AB_Lambda:
		sefoFrBuffer0(buf);
		sefoFrBuffer0(buf);
		break;

	default:
		argc = bufGetHInt(buf);
		for (i = 0; i < argc; i += 1)
			sefoFrBuffer0(buf);
		break;
	}
}

local void
symeFrBuffer0(Buffer buf)
{
	bufSkip(buf, HINT_BYTES);
}

local void
tformFrBuffer0(Buffer buf)
{
	TFormTag	tag;

	tag = bufGetByte(buf);
	tag &= 0x3f;

	if (tfIsSymTag(tag))
		;

	else if (tfIsAbSynTag(tag))
		sefoFrBuffer0(buf);

	else if (tfIsNodeTag(tag))
		tformListFrBuffer0(buf);

	symeListFrBuffer0 (buf);	/* tfSelf(tf) */
	symeListFrBuffer0 (buf);	/* tfSelfSelf(tf) */
	tformListFrBuffer0(buf);	/* tfQueries(tf) */
	tqualListFrBuffer0(buf);	/* tfCascades(tf) */

	if (tfTagHasSymes(tag))
		symeListFrBuffer0(buf); /* tfSymes(tf) */

	if (tag == TF_With) {
		UShort n, i;
		symeListFrBuffer0(buf);	/* tfCatExports(tf) */
		n = bufGetHInt(buf);
		for (i=0; i<n; i++) {
			sefoListFrBuffer0(buf);
		}
	}
	if (tag == TF_Third)
		symeListFrBuffer0(buf); /* tfThdExports(tf) */

	symeListFrBuffer0(buf);		/* fvSymes(tfFVars(tf)) */
}

local void
tqualFrBuffer0(Buffer buf)
{
	bufSkip(buf, SINT_BYTES);
	tformListFrBuffer0(buf);
}

local void
sefoListFrBuffer0(Buffer buf)
{
	SefoList	sefos = listNil(Sefo);
	UShort		i, sefoc;

	sefoc = bufGetHInt(buf);

	for (i = 0; i < sefoc; i += 1) {
		sefoFrBuffer0(buf);
	}
}


void
symeListFrBuffer0(Buffer buf)
{
	UShort		symec;

	symec = bufGetHInt(buf);
	bufSkip(buf, symec * HINT_BYTES);
}

local void
tformListFrBuffer0(Buffer buf)
{
	UShort		tformc;

	tformc = bufGetHInt(buf);
	bufSkip(buf, tformc * SINT_BYTES);
}

local void
tqualListFrBuffer0(Buffer buf)
{
	UShort		i, tqualc;

	tqualc = bufGetHInt(buf);

	for (i = 0; i < tqualc; i += 1)
		tqualFrBuffer0(buf);
}
