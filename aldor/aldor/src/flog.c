/****************************************************************************
 *
 * flog.c: Foam flow graphs.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
#include "axlphase.h"
#include "debug.h"
#include "file.h"
#include "opsys.h"
#include "store.h"
#include "util.h"


Bool	    flogDebug 		= false;
#define	    flogDEBUG(s)	DEBUG_IF(flogDebug, s)

#define	    BLOCK_LABEL_NONE	(-1)

local void  flogFreeFoamNode	(Foam);
local void  flogFreeFoam	(Foam);

/****************************************************************************
 *
 * :: Basic FlowGraph operations
 *
 ****************************************************************************/
 
/*
 * Display a flow graph, including edge info and code.
 * FLAG = true 	-> print all the information
 * FLAG = false -> print only generic graph information
 */
int
flogPrint(FILE *fout, FlowGraph flog, Bool extended)
{
	int		cc = 0, i;
	BBlock	bb;

	/* Gather info to help reader. */
	flogFixEntries(flog);
	flogFixLabels(flog);

	cc += fprintf(fout, "Flow Graph  (upto %d nodes)  entry = %d\n",
			flogBlockC(flog), flog->block0->label);

	for (i = 0; i < flogBlockC(flog); i++) {
		cc += fprintf(fout, "%d. ", i);
		bb =  flogBlock(flog, i);

		cc += bb ? bbPrint(fout, bb, extended): fprintf(fout, "Nil\n");
                cc += fprintf(fout,"<<\n");

	}
	return cc;
}

int
flogPrintDb(FlowGraph flog)
{
	return flogPrint(dbOut, flog, true);
}

void
flogDumpToFile(FlowGraph flog, String name)
{
	FILE *out = fileTryOpen(fnameParse(name), osIoWrMode);

	/* Only dump if we managed to open the file */
	if (out)
	{
		(void)flogPrint(out, flog, true);
		(void)fclose(out);
	}
	else
		(void)fprintf(dbOut, "Sorry: failed to create `%s'\n", name);
}

void
flogClearMarks(FlowGraph flog)
{
	int	   i;
	BBlock bb;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (bb) bb->mark = false;
	}
}

/*
 * Free unused blocks and zero the pointers to them.
 * Returns the number of nodes freed.
 */
int
flogCollect(FlowGraph flog)
{
	int		i, n;
	BBlock	bb;
	
	flogFixRefCounts(flog);

	n = 0;
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb || bb->refc > 0) continue;
		bbFree(bb);
		flogKillBlock(flog, i);
		n++;
	}
	return n;
}

/****************************************************************************
 *
 * :: Foam -> FlowGraph
 *
 ****************************************************************************/
 
/*
 * These local functions operate on partly formed flow graphs.
 */
local FlowGraph	 flogFrSeq0	     (Foam, FlogType, int);
local Length	 flog0EstimateNBB    (Foam, int);
local BlockLabel flog0NextLabel      (FlowGraph, Foam seq, Length *pix0);
local void	 flog0ClipBlocks     (FlowGraph, Foam, int);
local Foam	 flog0Clip1Block     (Foam seq,  Length *pix0);
local void	 flog0MakeExitEdges  (FlowGraph);
local void	 flog0Make1ExitEdge  (FlowGraph, BBlock fr, BlockLabel to);
local Length	 flog0KindExitCount  (BBlock);
local int	 flogCountLabelsInSeq (Foam);

/*
 * Construct a flow graph from a foam program.
 * The foam prog node is kept in the resulting graph.
 * Parameters:
 *    PROG must be a correct foam prog.
 *    MASK contains the information about the number of exits.
 *	   Number of exits:
 *            FLOG_UniqueExit for 1 Out for each block
 *	      FLOG_MultipleExits for multiple Out sets for each block.
 *
 * During formation, the fall-through label for "fs is kept in bb->iextra.
 */

FlowGraph
flogFrProg(Foam foam, FlogType mask)
{
	FlowGraph	flog;
	assert(foamTag(foam) == FOAM_Prog);

	/* Verify we have something sensible. */
        assert(foamTag(foam) == FOAM_Prog);
        assert(foamTag(foam->foamProg.body) == FOAM_Seq);

	flog = flogFrSeq0(foam->foamProg.body, mask, foam->foamProg.nLabels);

        flog->prog   = foam;

        return flog;
}


FlowGraph
flogFrSeq(Foam foam, FlogType mask)
{
	FlowGraph	flog;
	int 		nlabels = flogCountLabelsInSeq(foam);

	assert(foamTag(foam) == FOAM_Seq);

	flog = flogFrSeq0(foam, mask, nlabels);

	flog->prog = (Foam) NULL;

	return flog;

}

local FlowGraph
flogFrSeq0(Foam foam, FlogType mask, int nLabels)
{
        FlowGraph       flog;
	Bool		multipleExits;

	/* Verify the mask */

	if (mask == FLOG_UniqueExit)
		multipleExits = false;
	else if (mask == FLOG_MultipleExits)
		multipleExits = true;
	else {
		bug("flogFrSeq0: bad mask, no exit type.");
		NotReached(multipleExits = false);
	}

	/* Verify we have something sensible. */
        assert(foamTag(foam) == FOAM_Prog ||
	       foamTag(foam) == FOAM_Seq);

	/* Allocate the graph. */
        flog = (FlowGraph) stoAlloc(OB_Other, sizeof(*flog));

	flog->block0 = 0;		/* Set by flog0ClipBlocks */
        flog->blocks = bbufNew(flog0EstimateNBB(foam, nLabels));

	flog->dfIsPerExit = multipleExits;
	flog->fixedEntries = false;

	/* Clip the basic blocks from the sequence. */
	flog0ClipBlocks(flog, foam, nLabels);
        flogFreeFoamNode(foam);
 
	/* Fill the edge buffers connecting the basic blocks. */
	flog0MakeExitEdges(flog);

	flogBitvClass(flog) = (BitvClass) NULL;

        return flog;

}


/* Given a flow graph, clean only the dflow info.
 * Required to use the same flog in several optimizations (if, of course, they
 * don't change it).
 */
void
flogReuse(FlowGraph flog, FlogType mask)
{
	Bool		multipleExits;

	if (mask == FLOG_UniqueExit)
		multipleExits = false;
	else if (mask == FLOG_MultipleExits)
		multipleExits = true;
	else {
		bug("flogReuse: bad mask, no exit type.");
		NotReached(multipleExits = false);
	}

	flog->dfIsPerExit = multipleExits;

	/* Clear dflow info */

	flogIter(flog, bb, {
		dflowFreeBlockInfo(bb);
		bb->dfinfo = (DFlowInfo) 0;
	});

	bitvClassDestroy(flogBitvClass(flog));
	flogBitvClass(flog) = (BitvClass) NULL;
} 

/*
 * Determine the number of basic blocks to expect from a sequence.
 */
local Length
flog0EstimateNBB(Foam seq, int nLabels)
{
	assert(foamTag(seq) == FOAM_Seq);

	return foamCountSubtreesOfKind(seq, FOAM_If)
	     + foamCountSubtreesOfKind(seq, FOAM_Return)
	     + nLabels + 1;
}

/*
 * Clip a sequence into basic blocks, filling the given flow graph.
 */
local void
flog0ClipBlocks(FlowGraph flog, Foam seq, int nLabels)
{
        Length          i, ix0;
        Foam            subseq;
        BlockLabel      prevfall, nextfall;
	BBlock	bb;

	assert(foamTag(seq) == FOAM_Seq);

	/* Reserve the initial slots for blocks with existing labels. */
	for (i = 0; i <= nLabels; i++) 
		flogReserveLabel(flog);

	/* Scan through the sequence, biting of pieces. */
	ix0 = 0;
	prevfall = flog0NextLabel(flog, seq, &ix0);
	
	while (ix0 < foamArgc(seq) || (prevfall >= 0 && prevfall <= nLabels)) {
		subseq     = flog0Clip1Block(seq, &ix0);
		nextfall   = flog0NextLabel(flog, seq, &ix0);
                bb         = bbNew(subseq, prevfall);
		prevfall   = nextfall;
		bb->iextra = (bb->kind==FOAM_If) ? nextfall : BLOCK_LABEL_NONE;

		flogSetBlock(flog, bb->label, bb);
		if (!flog->block0) {
			bb->isblock0 = true;
			flog->block0 = bb;
		}
		flogDEBUG({
			fprintf(dbOut, "Clipped block %d\n", bb->label);
			if (bb->kind == FOAM_If)
				fprintf(dbOut, "... else is %d\n", nextfall);
			foamPrint(dbOut, subseq);
		});
	}
}

/*
 * Determine the block label for the block begining at position *pix0.
 * Free the label statement, if there is one.
 */
local BlockLabel
flog0NextLabel(FlowGraph flog, Foam seq, Length *pix0)
{
	Length	argc  = foamArgc(seq);
	Foam	*argv = seq->foamSeq.argv;
	Length	ix0   = *pix0;
	BlockLabel lab;

	if (ix0 < argc && foamTag(argv[ix0]) == FOAM_Label) {
		lab = argv[ix0]->foamLabel.label;
		flogFreeFoamNode(argv[ix0]);
		ix0++;
	}
	else if (ix0 < argc)
		lab = flogReserveLabel(flog);
	else
		lab = BLOCK_LABEL_NONE;
	*pix0 = ix0;
	return lab;
}

/*
 * Clip one basic block from a Foam sequence.
 */
local Foam
flog0Clip1Block(Foam seq, Length *pix0)
{
        Length          argc  = foamArgc(seq), i, n, ix0 = *pix0, ixL;
        Foam            *argv = seq->foamSeq.argv, subseq, extra;
        FoamTag         ftag;

	/* In case we need to manufacture an extra last statement... */
	extra = 0;

	/* Scan to the end of the block. */
#if EDIT_1_0_n1_07
	flogDEBUG(fprintf(dbOut, "ixL = ix0 at loop start %d\n", (int) ix0));
#else
	flogDEBUG(fprintf(dbOut, "ixL = ix0 at loop start %d\n", ix0));
#endif
	for (ftag = 0, ixL = ix0; !ftag && ixL < argc; )
		switch (foamTag(argv[ixL])) {
		case FOAM_Label:
			ftag = FOAM_Goto;
			/* Ran into label: Implied goto. */
			extra = foamNewGoto(argv[ixL]->foamLabel.label);
			foamPos(extra) = foamPos(argv[ixL]);
			ixL--;
			break;
		case FOAM_Goto:
		case FOAM_Return:
		case FOAM_If:
		case FOAM_Select:
			ftag = foamTag(argv[ixL]);
			break;
#define FlogHalt	1
#if FlogHalt
		case FOAM_BCall:
			if (foamIsBCallOf(argv[ixL], FOAM_BVal_Halt)) {
				ftag = FOAM_Throw;
			} else
				ixL++;
			break;
#endif
		default:
			ixL++;
			break;
		}
#if EDIT_1_0_n1_07
	flogDEBUG(fprintf(dbOut, "ixL at loop end %d\n", (int) ixL));
#else
	flogDEBUG(fprintf(dbOut, "ixL at loop end %d\n", ixL));
#endif
	if (ftag == 0) {
		/* Fell off end: Implied return. */
		ftag = FOAM_Return;
		extra= foamNewReturn(foamNew(FOAM_Values, int0));
		if (argc > 0) foamPos(extra) = foamPos(argv[argc-1]);
		ixL--;
	}
#if EDIT_1_0_n1_07
	flogDEBUG(fprintf(dbOut, "ixL after loop end %d\n", (int) ixL));
#else
	flogDEBUG(fprintf(dbOut, "ixL after loop end %d\n", ixL));
#endif

	/* Extract code sequence.  */
	n = ixL - ix0 + 1;
	subseq = foamNewEmpty(FOAM_Seq, n + (extra != 0));
	for (i = 0; i < n; i++)
		subseq->foamSeq.argv[i] = argv[ix0+i];
	if (extra != 0)
		subseq->foamSeq.argv[n] = extra;
	*pix0 = ixL + 1;
#if FlogHalt
	if (ftag==FOAM_Throw) 
		while (*pix0 < argc 
		       && foamTag(argv[*pix0]) != FOAM_Label) {
			/*foamFree(argv[*pix0]);*/
			(*pix0)++;
		}
#endif	
	return subseq;
}
 
/*
 * Add exit edges to a flow graph.
 */
local void
flog0MakeExitEdges(FlowGraph flog)
{
	BBlock bb;
	Foam 	   flast;
	int	   i, j;

	/* Ensure the entry and exit buffers are big enough. */
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;

		bbufNeed(bb->exits,  flog0KindExitCount(bb));
		bbSetExitC(bb, int0);

		flast = bbLastStat(bb);

		switch (bb->kind) {
		case FOAM_If:
			flog0Make1ExitEdge(flog, bb, bb->iextra);
			flog0Make1ExitEdge(flog, bb, flast->foamIf.label);
			break;
		case FOAM_Select:
			for (j = 0; j < foamArgc(flast)-1; j++)
				flog0Make1ExitEdge(flog, bb, 
					      flast->foamSelect.argv[j]);
			break;
		case FOAM_Goto:
			flog0Make1ExitEdge(flog, bb, flast->foamGoto.label);
			break;
		default:
			break;
		}
	}
}

local void
flog0Make1ExitEdge(FlowGraph flog, BBlock from, BlockLabel tolab)
{
	assert (tolab != BLOCK_LABEL_NONE);
	bbufAdd1(from->exits, flogBlock(flog, tolab));
}

local Length
flog0KindExitCount(BBlock bb)
{
	switch (bb->kind) {
	case FOAM_If:		return 2;
	case FOAM_Select:	return foamArgc(bbLastStat(bb)) - 1;
	case FOAM_Goto:		return 1;
	case FOAM_Return:	return 0;
#if FlogHalt
	case FOAM_Throw:	return 0;
#endif
	default:		bugBadCase(bb->kind); return 0;
	}
}

local int
flogCountLabelsInSeq(Foam foam)
{
	int counter = 0;
	int i;
	assert(foamTag(foam) == FOAM_Seq);

	for (i = 0; i < foamArgc(foam); i++)
		if (foamTag(foam->foamSeq.argv[i]) == FOAM_Label)
			counter++;

	return counter;
}

/****************************************************************************
 *
 * :: FlowGraph -> Foam
 *
 ****************************************************************************/

local Foam      flogToSeq0			(FlowGraph, int *);
local FoamList  flogConsStats			(FlowGraph, BBlock, FoamList);
local int	flogCountLabelsInFoamList	(FoamList);
local void	flogPeepholeExits		(BBlock bb);

/*
 * Reconstruct a foam prog from a flow graph.
 * If a block has any unconditional predecessors, it will directly follow one.
 */
Foam
flogToProg(FlowGraph flog)
{
        int             lab;
	Foam		prog = flog->prog;
 
        /* Update the prog. */
        prog->foamProg.body     = flogToSeq0(flog, &lab);
        prog->foamProg.nLabels = lab + 1;
 
        return prog;
}

Foam
flogToSeq(FlowGraph flog, int * nLabels)
{
	int	lab;
	Foam	newSeq;
 
        /* Update the prog. */
        newSeq  = flogToSeq0(flog, &lab);
        if (nLabels) *nLabels = lab + 1;

	stoFree(flog);

        return newSeq;
}



local Foam
flogToSeq0(FlowGraph flog, int * nLabels)
{
        FoamList        stats = listNil(Foam);
        int             lab;
	Foam		seq;
 
	flogFixLabels(flog);
	flogFixRefCounts(flog);
	flogClearMarks(flog);
 
        /* Collect statements, starting at the function entry node. */
        stats = flogConsStats(flog, flog->block0, stats);
        stats = listNReverse(Foam)(stats);
 
        /* Determine the maximum label. */
	lab = flogCountLabelsInFoamList(stats);

        if (nLabels) *nLabels = lab;

        seq = foamNewOfList(FOAM_Seq, stats);
 
        /* Clean up and return. */
        listFree(Foam)(stats);

	bitvClassDestroy(flogBitvClass(flog));

	stoFree(flog);

        return seq;
}


/* Given a (Seq ...), returns the number of labels in it */
local int
flogCountLabelsInFoamList(FoamList stats)
{
        int lab = 0;

	listIter(Foam, stat, stats, {
                if (foamTag(stat) != FOAM_Label)
                        continue;
                if (lab < stat->foamLabel.label)
                        lab = stat->foamLabel.label;
        });
 
	return lab;
}

/*
 * Used by flogToProg -- conses statements recursively.
 */
local FoamList
flogConsStats(FlowGraph flog, BBlock bb, FoamList stats)
{
        int             i, fc;
        Foam            *fv, stat;
        BBlock      dd;
 
        if (bb->mark) return stats;
        bb->mark = true;

	/* If the previous statement just comes here, free it. */
	while (stats &&
	       foamTag(car(stats)) == FOAM_Goto &&
	       car(stats)->foamGoto.label == bb->label)
	{
		flogFreeFoamNode(car(stats));
		stats = cdr(stats);
	}
	
	flogPeepholeExits(bb);

        fc = foamArgc(bb->code);
        fv = bb->code->foamSeq.argv;

	/* Label needed if: more than one entry OR any conditional entry.  */
        if (bb->refc > 1 || bb->lrefc > 0) {
		stat          = foamNewLabel(bb->label);
		foamPos(stat) = foamPos(fv[0]);
                stats         = listCons(Foam)(stat, stats);
	}
 
        /* Add the statements. */
        for (i = 0; i < fc; i++)
                stats = listCons(Foam)(fv[i], stats);
	
        if (bb->kind == FOAM_If) {
		/* Else case: Fall through or add goto, as necessary. */
                dd = bbExit(bb, int0);
                if (dd->mark)
                        stats = listCons(Foam)(foamNewGoto(dd->label), stats);
                else
                        stats = flogConsStats(flog, dd, stats);
		/* Then case: Add statements. */
		dd = bbExit(bb, 1);
		stats = flogConsStats(flog, dd, stats);
        }
	else {
		/* Add destinations. */
		for (i = 0; i < bbExitC(bb); i++) {
			dd    = bbExit(bb, i);
			stats = flogConsStats(flog, dd, stats);
		}
	}
 
        return stats;
}

local void
flogPeepholeExits(BBlock bb)
{
	int i;
	int out;

	if (bbExitC(bb) > 1) {
		out = bbExit(bb, int0)->label;
		for (i=1; i < bbExitC(bb); i++) {
			if (bbExit(bb, i)->label != out) {
				out = -1;
				break;
			}
		}
		if (out != -1)
			bbSpecializeExit(bb, int0);
	}
}


/******************************************************************************
 *
 * :: Block Insertion/Removal in FlowGraph
 *
 *****************************************************************************/

/* Insert "newb" on the edge between "frb" and "tob" and add it to the graph.
 * NOTE: assumes that there is such edge in the flowgraph.
 * NOTE: doesn't care about entries fixing.
 */
void
flogBlockInsertBtwn(FlowGraph flog, BBlock newb, BBlock frb, BBlock tob)
{
	int i;

	/* Add "newb" to the graph... */
	flogSetBlock(flog, newb->label, newb);

	/* .. and set up entry and exit edges */

	bbufNeed(newb->exits, 1);
	bbSetExit(newb, int0, tob);	/* newb -> tob */
	bbSetExitC(newb, 1);

	for (i = 0; i < bbExitC(frb); i++)
		if (bbExit(frb, i) == tob) break;

	assert(i < bbExitC(frb));

	bbSetExit(frb, i, newb);
}

/* Insert "newb" in "flog" after block "bb" */
void
flogBlockInsertAfter(FlowGraph flog, BBlock newb, BBlock bb)
{
	/* ToDo */
}

/* Insert "newb" in "flog" before block "bb" */
void
flogBlockInsertBefore(FlowGraph flog, BBlock newb, BBlock bb)
{
	/* ToDo */
}


/****************************************************************************
 *
 * :: Internal FlowGraph consistency operations
 *
 ****************************************************************************/

/*
 * Fix the labels in a flow graph:
 * Set the correct
 * -- block labels,
 * -- fall through labels,
 * -- destination labels in the code parts,
 * -- max label in the prog.
 */
void
flogFixLabels(FlowGraph flog)
{
	int	   i, j, maxi;
	Foam	   flast;
	BBlock bb;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;

		flast = bbLastStat(bb);
		
		switch (bb->kind) {
		case FOAM_If:
			flast->foamIf.label = bbExit(bb, 1)->label;
			break;
		case FOAM_Select:
			for (j = 0; j < foamArgc(flast)-1; j++)
				flast->foamSelect.argv[j]=bbExit(bb,j)->label;
			break;
		case FOAM_Goto:
			flast->foamGoto.label = bbExit(bb, int0)->label;
			break;
		default:
			break;
		}
	}

	for (maxi = 0, i = 0; i < flogBlockC(flog); i++)
		if (flogBlock(flog,i)) maxi = i;

	flog->prog->foamProg.nLabels = maxi;
}

/*
 * Fix the reference counts in a flow graph:
 */
local void flogFixRefCountsTo    (FlowGraph, BBlock, Bool isLabel);
local void flogFixRefCountsFrom	 (FlowGraph, BBlock);

void
flogFixRefCounts(FlowGraph flog)
{
	int	   i;
	BBlock bb;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;
		bb->mark  = false;
		bb->refc  = 0;		
		bb->lrefc = 0;		
	}
	if (flog->block0) flogFixRefCountsTo(flog, flog->block0, false);
}

local void
flogFixRefCountsTo(FlowGraph flog, BBlock dd, Bool isLabel)
{
	if (isLabel) dd->lrefc++;
	dd->refc++;
	flogFixRefCountsFrom(flog, dd);
}

local void
flogFixRefCountsFrom(FlowGraph flog, BBlock bb)
{
	int	   i;

	if (!bb || bb->mark) return;
	bb->mark = true;

	switch (bb->kind) {
	case FOAM_If:
		flogFixRefCountsTo(flog, bbExit(bb, int0), false);
		flogFixRefCountsTo(flog, bbExit(bb,    1), true);
		break;
	case FOAM_Select:
		for (i = 0; i < bbExitC(bb); i++)
			flogFixRefCountsTo(flog, bbExit(bb, i), true);
		break;
	case FOAM_Goto:
		flogFixRefCountsTo(flog, bbExit(bb, int0), false);
		break;
	default:
		break;
	}
}

/*
 * Set the entry buffers to contain back pointers.
 */
void
flogFixEntries(FlowGraph flog)
{
	int	   i, j;
	BBlock bb, dd;

 	if (flog->fixedEntries)	
		return;
	
	flogFixRefCounts(flog);
	
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;

		bbufNeed(bb->entries, bb->refc);
		bbSetEntryC(bb, int0);
	}
	
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb || bb->refc == 0) continue;

		for (j = 0; j < bbExitC(bb); j++) {
			dd = bbExit(bb, j);
			bbufAdd1(dd->entries, bb);
		}
	}
        
	flog->fixedEntries = true;
}

/****************************************************************************
 *
 * :: BBlock operations
 *
 ****************************************************************************/
 
/*
 * Allocate a basic block but do not fill in the edges.
 */
BBlock
bbNew(Foam foam, BlockLabel in)
{
	BBlock	bb;
	FoamTag tag;

	assert(foam && foamTag(foam) == FOAM_Seq && foamArgc(foam) > 0);

	bb = (BBlock) stoAlloc(OB_Other, sizeof(*bb));
	bb->code    = foam;
	tag =  foamTag(bbLastStat(bb));
	if (tag == FOAM_BCall)
		tag = FOAM_Throw;
	bb->kind    = tag;
	bb->mark    = false;
	bb->label   = in;
	bb->isblock0= false;
	bb->refc    = 0;
	bb->lrefc   = 0;
	bb->entries = bbufNew(int0);
	bb->graph   = 0;
	bb->exits   = bbufNew(int0);
	bb->dfinfo  = 0;
	bb->pextra  = 0;
	bb->iextra  = 0;

	return bb;
}

void
bbFree(BBlock bb)
{
	if (bb->graph) bb->graph->blocks->argv[bb->label] = 0;
	foamFree(bb->code);
	bbufFree(bb->entries);
	bbufFree(bb->exits);
	if (bb->dfinfo) dflowFreeBlockInfo(bb);
	stoFree((Pointer) bb);
}

int
bbPrint(FILE *fout, BBlock bb, Bool extended)
{
	int	cc = 0, i;
	String	s;

	switch (bb->kind) {
	case FOAM_If:      s = "If    "; break;
	case FOAM_Select:  s = "Select"; break;
	case FOAM_Return:  s = "Return"; break;
	case FOAM_Goto:    s = "Goto  "; break;
#if FlogHalt
	case FOAM_Throw:   s = "Exit  "; break;
#endif
	default:           s = "??????"; break;
	}

	cc += fprintf(fout, "Block %d: %s", bb->label, s);
	cc += fprintf(fout, " [refc=%d, lrefc=%d, isblock0=%d, mark=%d, iextra=%d]\n",
			    bb->refc, bb->lrefc, bb->isblock0, bb->mark, bb->iextra);
	cc += fprintf(fout, "  Exits:");
	for (i = 0; i < bbExitC(bb); i++)
		cc += fprintf(fout, " %d", bbExit(bb,i)->label);
	cc += fprintf(fout, "  Entries:");
	for (i = 0; i < bbEntryC(bb); i++)
		cc += fprintf(fout, " %d", bbEntry(bb,i)->label);

	cc += fprintf(fout, "\n");
	if (extended) cc += foamPrint(fout, bb->code);
	if (extended) cc += dflowPrintBlockInfo(fout, bb);

	return cc;
}

Foam
bbLastStat(BBlock bb)
{
	assert(bb->code && foamArgc(bb->code) != 0);

	return bb->code->foamSeq.argv[foamArgc(bb->code)-1];
}

/*
 * Find the last values of local variables in a basic block.
 * The returned values are shared and are only guaranteed to exist as long
 * as the basic block.  This function does not take into account the
 * constraint imposed by an If or Select.
 */
local void
bbLastValuesUpdate(Foam parv, Foam locv, Foam lhs, Foam rhs)
{
	if (foamTag(lhs) == FOAM_Loc && locv)
		locv->foamSeq.argv[lhs->foamLoc.index] = rhs;
	if (foamTag(lhs) == FOAM_Par && parv)
		parv->foamSeq.argv[lhs->foamPar.index] = rhs;
}

void
bbLastValues(BBlock bb, Foam parv, Foam locv, Foam same, Foam dunno)
{
	int	i, j;

	Foam 	code, stat, lhs, rhs;

	if (parv) {
		assert(foamTag(parv) == FOAM_Seq);
		for (i=0; i < foamArgc(parv); i++)
			parv->foamSeq.argv[i] = same;
	}
	if (locv) {
		assert(foamTag(locv) == FOAM_Seq);
		for (i=0; i < foamArgc(locv); i++)
			locv->foamSeq.argv[i] = same;
	}

	if (!bb) return;

	code = bb->code;
	for (i = 0; i < foamArgc(code); i++) {
		stat = code->foamSeq.argv[i];

		switch (foamTag(stat)) {
		case FOAM_Def:
			lhs = stat->foamDef.lhs;
			rhs = stat->foamDef.rhs;
			bbLastValuesUpdate(parv, locv, lhs, rhs);
			break;
		case FOAM_Set:
			lhs = stat->foamSet.lhs;
			rhs = stat->foamSet.rhs;

			if (foamTag(lhs) == FOAM_Values) {
				for (j = 0; j < foamArgc(lhs); j++) {
					Foam v = lhs->foamValues.argv[j];
					bbLastValuesUpdate(parv,locv,v,dunno);
				}
				break;
			}
			else
				bbLastValuesUpdate(parv, locv, lhs, rhs);
			break;
		default:
			break;
		}
	}
}

BBlock
bbCopy(BBlock odd)
{
	FlowGraph  flog;
	Foam	   code;
	BlockLabel nlab;
	BBlock ndd;
	int i;

	flog = odd->graph;
	nlab = flogReserveLabel(flog);
	code = foamCopy(odd->code);
	ndd  = bbNew(code, nlab);

	/* Attatch it to the graph. */
	flogSetBlock(flog, nlab, ndd);

	/* Copy exits of odd into ndd. */
	bbufNeed(ndd->exits, bbExitC(odd));
	bbSetExitC(ndd, bbExitC(odd));

	for (i = 0; i < bbExitC(odd); i++)
		bbSetExit(ndd, i, bbExit(odd, i));

	return ndd;
}

/*
 * Make the specified exit of bb point to a new node, which is
 * a copy of the old one.
 */
BBlock
bbCloneExitFrom(BBlock bb, int exitno)
{
	BBlock odd, ndd;

	/* Make the new node. */
	odd  = bbExit(bb, exitno);
	ndd = bbCopy(odd);

	/* Reset exit of bb to be ndd. */
	bbSetExit(bb, exitno, ndd);

	return ndd;
}

/*
 * Convert an If or Select node to a goto (to the given exit).
 */
void
bbSpecializeExit(BBlock bb, int exitno)
{
	Foam	   code, olast, nlast;
	int        i;
	assert(bb->kind == FOAM_If || bb->kind == FOAM_Select);

	/* Replacing the last statement of the code. */
	code  = bb->code;
	olast = code->foamSeq.argv[foamArgc(code)-1];
	if (foamTag(olast) == FOAM_If && foamHasSideEffect(olast->foamIf.test)) {
		Foam newCode = foamNew(FOAM_Seq, foamArgc(code) + 1);
		for (i=0; i<foamArgc(code); i++) {
			newCode->foamSeq.argv[i] = code->foamSeq.argv[i];
		}
		newCode->foamSeq.argv[i-1] = foamCopy(code->foamSeq.argv[i-1]->foamIf.test);
		newCode->foamSeq.argv[i] = code->foamSeq.argv[i-1];
		foamFreeNode(code);
		bb->code = code = newCode;
	}
	if (foamTag(olast) == FOAM_Select && foamHasSideEffect(olast->foamSelect.op)) {
		Foam newCode = foamNew(FOAM_Seq, foamArgc(code) + 1);
		for (i=0; i<foamArgc(code); i++) {
			newCode->foamSeq.argv[i] = code->foamSeq.argv[i];
		}
		newCode->foamSeq.argv[i-1] = foamCopy(code->foamSeq.argv[i-1]->foamSelect.op);
		newCode->foamSeq.argv[i] = code->foamSeq.argv[i-1];
		foamFreeNode(code);
		bb->code = code = newCode;
	}
	olast = code->foamSeq.argv[foamArgc(code)-1];
	nlast = foamNewGoto(bbExit(bb,exitno)->label);
	foamPos(nlast) = foamPos(olast);
	code->foamSeq.argv[foamArgc(code)-1] = nlast;
	foamFree(olast);

	/* Keep only the one exit. */
	bbSetExit (bb, int0, bbExit(bb, exitno));
	bbSetExitC(bb, 1);

	/* Dataflow information will have to be recalculated. */
	if (bb->dfinfo) { dflowFreeBlockInfo(bb); bb->dfinfo = 0; }

	/* Change the node type. */
	bb->kind = FOAM_Goto;
}

/*
 * Produce a new node, which is the concatenation of two old ones.
 * The old nodes are modified/freed.
 */
BBlock
bbNConcat(BBlock bb, BBlock dd)
{
	int	   i, nbb, ndd;
	Foam	   code;
	BlockBuf   bbuf;

	assert(bb->kind == FOAM_Goto);

	bb->graph->fixedEntries = false;

	/* The second block determines the kind. */
	bb->kind = dd->kind;

	/* Form the new Foam sequence, omitting goto from bb. */
	nbb  = foamArgc(bb->code);
	ndd  = foamArgc(dd->code);
	code = foamNewEmpty(FOAM_Seq, nbb-1+ndd);

	for (i = 0; i < nbb-1; i++) 
		code->foamSeq.argv[i]       = bb->code->foamSeq.argv[i];
	for (i = 0; i < ndd; i++)
		code->foamSeq.argv[nbb-1+i] = dd->code->foamSeq.argv[i];

	flogFreeFoam    (bb->code->foamSeq.argv[nbb-1]);
	flogFreeFoamNode(bb->code);
	flogFreeFoamNode(dd->code);
	bb->code = code;
	dd->code = 0;		/* for bbFree */

	/* The new block will use the old one's exits. */
	bbuf      = dd->exits;
	dd->exits = bb->exits;
	bb->exits = bbuf;

	/* Dataflow information will have to be recalculated. */
	if (bb->dfinfo) { dflowFreeBlockInfo(bb); bb->dfinfo = 0; }

	bbFree(dd);
	return bb;
}

/* Return all the entries of bb.
 * NOTE: block entries aren't fixed. This procedure is convenient only if we
 * are only interested to the entries of one block and we don't want to call
 * flogFixEntries.
 */
BBlockList
bbGetEntries(BBlock bb)
{
	BBlockList	res = listNil(BBlock);
	int		i;

	if (bb->graph->fixedEntries) {
		for (i = 0; i < bbEntryC(bb); i++)
			listPush(BBlock, bbEntry(bb, i), res);

	}
	else {
		flogIter(bb->graph, block, {
			for (i = 0; i < bbExitC(block); i++)
				if (bbExit(block, i) == bb)
					listPush(BBlock, block, res);
		});
	}
	return res;
}

/****************************************************************************
 *
 * :: BlockBuf operations
 *
 ****************************************************************************/
 
BlockBuf
bbufNew(Length initCount)
{
        BlockBuf        bf;
 
        bf       = (BlockBuf) stoAlloc(OB_Other, sizeof(*bf));
        bf->pos  = 0;
        bf->argc = 0;
	bf->argv = 0;
	bbufNeed(bf, initCount);

        return bf;
}
 
void
bbufFree(BlockBuf bf)
{
        if (bf->argv) stoFree((Pointer) bf->argv);
        stoFree((Pointer) bf);
}
 
void
bbufNeed(BlockBuf bf, Length need)
{
	if (bf->argc >= need) return;

	if (!bf->argv)
		bf->argv = (BBlock *)
			   stoAlloc(OB_Other,  need * sizeof(BBlock));
	else
		bf->argv = (BBlock *)
			   stoResize(bf->argv, need * sizeof(BBlock));

	bf->argc = stoSize(bf->argv)/sizeof(BBlock); /* Any extra space! */
}

void
bbufAdd1(BlockBuf bf, BBlock bb)
{
        if (bf->pos == bf->argc) bbufNeed(bf, bf->argc + bf->argc/2 + 1);
	bf->argv[bf->pos++] = bb;
}

Length
bbufReserve1(BlockBuf bf)
{
	bbufAdd1(bf, NULL);
	return bf->pos - 1;
}

/*
 * Squish the blocks to the beginning of the buffer.
 */
void
bbufCompress(BlockBuf bf)
{
	int 		i, ic;
	BBlock	bb;

	for (ic = i = 0; i < bf->pos; i++) {
		bb = bf->argv[i];
		if (bb) {
			bb->label      = ic;
			bf->argv[ic++] = bb;
		}
	}
	bf->pos = ic;
}

/*
 * Find the block with the given label.
 */
BBlock
bbufBlockFn(BlockBuf bf, BlockLabel lab)
{
	assert(lab < bf->pos);
	return bf->argv[lab];
}

/*
 * Set the block with the given label.
 */
BBlock
bbufSetBlockFn(BlockBuf bf, BlockLabel lab, BBlock bb)
{
	assert(bb->label < bf->pos);
	bf->argv[bb->label] = bb;
	return bb;
}

/****************************************************************************
 *
 * :: Freeing statements.
 *
 ***************************************************************************/

local void
flogFreeFoamNode(Foam foam)
{
	flogDEBUG(fprintf(dbOut, "Freeing foam node %s[%d]\n",
			  foamInfo(foamTag(foam)).str, (int) foamArgc(foam)));
	foamFreeNode(foam);
}

local void
flogFreeFoam(Foam foam)
{
	flogDEBUG(fprintf(dbOut, "Freeing foam tree %s[%d]\n",
			  foamInfo(foamTag(foam)).str, (int) foamArgc(foam)));
	foamFree(foam);
}
