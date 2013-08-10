/*****************************************************************************
 *
 * loops.c: Loop Optimization Tools
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
/****************************************************************************
 *
 * ToDo:
 *
 *  - The algorithm used search for natural loops. For the optimizations that
 *    create a preheader loops with the some header must be unified.
 *	-> add a new export to this file
 *  - add a function that, given a block B and a LoopList, says how many/which
 *    loops contain B.
 *  - 
 *
 ****************************************************************************/

#include "debug.h"
#include "flog.h"
#include "loops.h"
#include "store.h"
#include "util.h"

/****************************************************************************
 *
 * :: Type Definitions
 *
 ****************************************************************************/

CREATE_LIST(BBlock);

struct _Edge {

	BBlock	tail;
	BBlock	head;

};

typedef struct _Edge  	* Edge;

DECLARE_LIST(Edge);
CREATE_LIST(Edge);


CREATE_LIST(Loop);

/****************************************************************************
 *
 * :: Debug
 *
 ****************************************************************************/

Bool	lpDebug		= false;

#define lpDEBUG		DEBUG_IF(lp)	afprintf

/****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/


/****************************************************************************
 *
 * :: Local Functions
 *
 ****************************************************************************/

local int	lpLoopsCountFrBlock		(BBlock, LoopList);

local Dominators lpDominatorsFrFlog		(FlowGraph);

local EdgeList	lpBackEdgesFrDominators		(Dominators);

local void	lpInsertAndPush0 	    (BBlock, BBlockList*, BBlockList*);

local void	lpUnifyLoops			(Loop, LoopList);

local Loop	lpNaturalLoopFrBackEdge		(Edge, FlowGraph);
local LoopList	lpNaturalLoopsFrBackEdges	(EdgeList, FlowGraph);

local Edge	lpEdgeNew			(BBlock, BBlock);
local void	lpEdgeFree			(Edge);

local Loop 	lpLoopNew	     (BBlock, BBlockList, FlowGraph);

void		lpDominatorsPrintDb	(Dominators);
void		lpEdgeListPrintDb	(EdgeList);
void		lpLoopPrintDb		(Loop);
void		lpLoopListPrintDb	(LoopList);


/****************************************************************************
 *
 *
 * :: Main Entry Points
 *
 *
 ****************************************************************************/

void
testLoop(Foam foam)
{
	Foam 		defs = foam->foamUnit.defs;
	Foam 		def;
	int		i;
	FlowGraph	flog;
	LoopList	loops;

	lpDebug = true;

	for (i = 0; i < foamArgc(defs); i++) {
		def = defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog) continue;

		flog = flogFrProg(def->foamDef.rhs, FLOG_UniqueExit);

		loops = lpNaturalLoopsFrFlog(flog, (Dominators *) NULL);

		loops = lpUnifyCommonHeaders(loops);

		def->foamDef.rhs = flogToProg(flog);

		listFreeDeeply(Loop)(loops, lpLoopFree);
	}
	
}

/*****************************************************************************
 *
 * :: Natural Loops from FlowGraph
 *
 ****************************************************************************/


/* Given a flowgraph FLOG, return the list of all natural loops in the graph.
 * If PDOMS is not NULL, then the dominators found are returned with this
 *	parameter.
 *
 * NOTE: These procedures don't modify the structure of FLOG and don't change
 *	 dataflow info.
 */

LoopList
lpNaturalLoopsFrFlog(FlowGraph flog, Dominators * pDoms)
{
	LoopList	loops;
	Dominators	doms;
	EdgeList	backEdges;

	doms = lpDominatorsFrFlog(flog);

	if (DEBUG(lp)) {
		lpDominatorsPrintDb(doms);
	}

	backEdges = lpBackEdgesFrDominators(doms);

	if (DEBUG(lp)) {
		fprintf(dbOut,"Back Edges: \n");
		lpEdgeListPrintDb(backEdges);
	}

	if (pDoms)
		*pDoms = doms;
	else
		lpDominatorsFree(doms);

	loops = lpNaturalLoopsFrBackEdges(backEdges, flog);

	if (DEBUG(lp)) {
		lpLoopListPrintDb(loops);
	}

	listFreeDeeply(Edge)(backEdges, lpEdgeFree);
	
	return loops;
}

/*****************************************************************************
 *
 * :: Loop Depth Info for FlowGraph
 *
 ****************************************************************************/


void
lpLoopDepthInfoSetInFlog(FlowGraph flog)
{
	LoopList loops = lpNaturalLoopsFrFlog(flog, (Dominators *) int0);

	flogIter(flog, bb, {
		bbSetIExtra(bb, lpLoopsCountFrBlock(bb, loops));
	});
	
	listFreeDeeply(Loop)(loops, lpLoopFree);
}

/* Return the number of loops in LOOPS containing BLOCK */
local int
lpLoopsCountFrBlock(BBlock block, LoopList loops)
{
	int		counter = 0;
	
	listIter(Loop, loop, loops, {
		if (bitvTest(loop->bitvClass, loop->blockSet, block->label))
			counter += 1;
	});

	return counter;
}


/*****************************************************************************
 *
 * :: Exit Blocks from Loop
 *
 ****************************************************************************/

/* Given a loop, return the list of exit blocks */
BBlockList
lpExitBlocksFrLoop(Loop loop)
{
	BBlockList	exitBlocks = listNil(BBlock);
	int		i;

	/* Criteria: EXIT BLOCKS are blocks with a successor not in the loop */

	listIter(BBlock, bb, loop->blockList, {
		for (i = 0; i < bbExitC(bb); i++) {
			if (!lpIsBlockInLoop(bbExit(bb, i), loop))
				break;
		}

		if (i != bbExitC(bb))
			listPush(BBlock, bb, exitBlocks);
	});

	return exitBlocks;
}

/*****************************************************************************
 *
 * :: Unify Common Headers
 *
 ****************************************************************************/

/* Given a looplist, search for all the loops with the some header and
 * unify them.
 * Upon exit LOOPS is destroyed and a new list is returned.
 *
 * Algorithm:
 *   1) LOOPS is traversed and all the bits in bitvMark corresponding to the
 *	  headers are marked.
 *	When a loop is found with marked header (=> is not unique), then it's
 *	added to the unifiables list. Not that if there are, in example, 3
 *	loops with the same header, the first is not pushed in this list.
 *	BitvUnmark keep trace of loop headers find more then once.
 *
 *   2) LOOPS is traversed and all the loops with bit not marked are pushed
 *	in the notUnifiables list.
 *
 *   3) LOOPS is traversed again. When a loop is found with the mark bit on
 *	and the unmark bit off (=> unifiable, must still be unified),
 *	then it is the one that hasn't been pushed in the unifiables list.
 *	So we call lpUnifyLoops that unify all the loops with the same header
 *	in unifiables, we add this loop to the unified list, and clear the
 *	bit. This last action ensure that all the loops with same header are
 *	unified exactly once.
 */
LoopList
lpUnifyCommonHeaders(LoopList loops)
{
	BitvClass	class;
	Bitv		bitvMark, bitvUnmark;
	LoopList	notUnifiables = listNil(Loop);
	LoopList	unifiables    = listNil(Loop);
	LoopList	unified	      = listNil(Loop);

	if (!loops) return loops;

	lpDEBUG(dbOut, "lpUnifyCommonHeaders << %d loops ...",
		(int) listLength(Loop)(loops));

	class = car(loops)->bitvClass;

	bitvMark = bitvNew(class);
	bitvUnmark = bitvNew(class);
	bitvClearAll(class, bitvMark);
	bitvClearAll(class, bitvUnmark);

	/* Step 1 */

	listIter(Loop, loop, loops, {

		/* Already seen ? */
		if (bitvTest(class, bitvMark, loop->header->label)) {
			listPush(Loop, loop, unifiables);
			bitvSet(class, bitvUnmark, loop->header->label);
		}
		else
			bitvSet(class, bitvMark, loop->header->label);
	});

	/* Final Assertions 
	 *  - each loop has bitvUnmark == 0 iif is not unifiable
	 *  - all loops have bitvMark == 1
	 */

	/* Step 2 */

	listIter(Loop, loop, loops, {
		if (!bitvTest(class, bitvUnmark, loop->header->label))
			listPush(Loop, loop, notUnifiables);
	});
	
	/* Step 3 */
		 
	listIter(Loop, loop, loops, {

		/* The test means: unifiable /\ must be still unified */

		if (bitvTest(class, bitvUnmark, loop->header->label) &&
		    bitvTest(class, bitvMark, loop->header->label)) {
			lpUnifyLoops(loop, unifiables);
			listPush(Loop, loop, unified);
			bitvClear(class, bitvMark, loop->header->label);

			/* bitvMark off ==> loop class already unified */
		}
	});

	bitvFree(bitvMark);
	bitvFree(bitvUnmark);

	listFreeDeeply(Loop)(unifiables, lpLoopFree);

	lpDEBUG(dbOut, ">> %d loops.\n",
		(int) listLength(Loop)(notUnifiables) +
		(int) listLength(Loop)(unified));


	return listNConcat(Loop)(notUnifiables, unified);
}


/* Look for loops with the same header of LOOP and unify them with it.
 */
local void
lpUnifyLoops(Loop mainLoop, LoopList unifiables)
{
	BitvClass  class = mainLoop->bitvClass;

	listIter(Loop, loop, unifiables, {

		if (loop->header == mainLoop->header) {

			assert(loop->graph == mainLoop->graph);
			assert(loop->bitvClass->nbits == 
			       mainLoop->bitvClass->nbits);

			/* Add to mainLoop all the blocks that it hasn't */

			listIter(BBlock, bb, loop->blockList, {

				if (!lpIsBlockInLoop(bb, mainLoop))
					listPush(BBlock, bb,
						 mainLoop->blockList);
			});

			/* Update mainLoop->blockSet */
			bitvOr(class, mainLoop->blockSet,
			       mainLoop->blockSet, loop->blockSet);
		}
	});
}

/****************************************************************************
 *
 *
 * :: Dominators
 *
 *
 ****************************************************************************/

local Dominators	lpDominatorsNew		(FlowGraph);
local void		lpDomsInit	(Dominators, FlowGraph);
local Bool		lpFindDoms0	(Dominators, FlowGraph);

/*
 * Given a flowgraph FLOG, find all the dominators for each node.
 * if doms is the returned set, then lpDoms(b1, b2, doms) can be used to know
 * if b1 dominates b2. 
 *
 */
local Dominators
lpDominatorsFrFlog(FlowGraph flog)
{
	Dominators	doms;
	Bool		changed = true;

	flogFixEntries(flog);

	doms = lpDominatorsNew(flog);

	lpDomsInit(doms, flog);

	while (changed)
		changed = lpFindDoms0(doms, flog);

	return doms;
}


local void
lpDomsInit(Dominators doms, FlowGraph flog)
{
	int		nBlocks = doms->nBlocks;
	int		i;

	/* ----- Initialization ----- */

	for (i = 0; i < nBlocks; i++) {
		doms->doms[i] = bitvNew(doms->bitvClass);

		bitvSetAll(doms->bitvClass, doms->doms[i]);
	}

	/* Initialize block 0 */

	i = flog->block0->label;
	bitvClearAll(doms->bitvClass, doms->doms[i]);
	bitvSet(doms->bitvClass, doms->doms[i], i);
}


local Bool
lpFindDoms0(Dominators doms, FlowGraph flog)
{
	Bool 		changed = false;
	int		i, j, label;
	BBlock	bb, pred;
	Bitv		oldMask = bitvNew(doms->bitvClass);
	Bitv		newMask;
	
	assert(doms->nBlocks == flogBlockC(flog) && doms->doms);

	for (i = 0; i < doms->nBlocks; i++) {
		bb = flogBlock(flog, i);

		if (!bb || bb->isblock0) continue;

		label = bb->label;
		newMask = doms->doms[label];
		bitvCopy(doms->bitvClass, oldMask, newMask);

		/* doms(n) = {n} \/ (/\ doms(p), for each p predecessor of n */

		if (bbEntryC(bb))
		    bitvSetAll(doms->bitvClass, newMask);
		else
		    bitvClearAll(doms->bitvClass, newMask);

		for (j = 0; j < bbEntryC(bb); j++) {
			pred = bbEntry(bb, j);
			bitvAnd(doms->bitvClass, newMask,
				newMask, doms->doms[pred->label]);
   	        }

		bitvSet(doms->bitvClass, newMask, bb->label);

		if (!bitvEqual(doms->bitvClass, oldMask, newMask))
			changed = true;
	    }

	bitvFree(oldMask);

	return changed;
}


/****************************************************************************
 *
 *
 * :: Back Edges
 *
 *
 ****************************************************************************/

local EdgeList
lpBackEdgesFrDominators(Dominators doms)
{
	EdgeList	backEdges = listNil(Edge);
	BBlock	tail, head;
	int		i, j;
	FlowGraph 	flog = doms->graph;

	for (i = 0; i < flogBlockC(flog); i++) {

		tail = flogBlock(flog, i);

		if (!tail) continue;

		for (j = 0; j < bbExitC(tail); j++) {
			head = bbExit(tail, j);

			if (lpIsDom(doms, head, tail))
				listPush(Edge, lpEdgeNew(tail, head),
					 backEdges);
		}
	}
	
	return backEdges;
}

/****************************************************************************
 *
 *
 * :: Natural Loops
 *
 *
 ****************************************************************************/


local LoopList
lpNaturalLoopsFrBackEdges(EdgeList backEdges, FlowGraph flog)
{
	LoopList	loopList = listNil(Loop);
	Loop		loop;

	listIter(Edge, backEdge, backEdges,
		 { 
		   loop = lpNaturalLoopFrBackEdge(backEdge, flog);
		   listPush(Loop, loop, loopList);
	         });

	return loopList;
}

local Loop
lpNaturalLoopFrBackEdge(Edge backEdge, FlowGraph flog)
{	
	BBlockList	bbStackList = listNil(BBlock);
	BBlockList	bbLoopList = listNil(BBlock);
	Loop		loop;
	int		i;

	flogFixEntries(flog);

	listPush(BBlock, backEdge->head, bbLoopList);

	lpInsertAndPush0(backEdge->tail, &bbLoopList, &bbStackList);

	while (bbStackList) {

		/* Pop an element from the stack */

		BBlock bb = car(bbStackList);
		bbStackList = cdr(bbStackList);

		/* Insert each predecessor of bb */

		for (i = 0; i < bbEntryC(bb); i++) {
			lpInsertAndPush0(bbEntry(bb, i),
					 &bbLoopList, &bbStackList);
		 }
	}

	loop = lpLoopNew(backEdge->head, bbLoopList, flog);

       	return loop;
}


/* Auxiliary function for finding a loop from a back edge */

local void
lpInsertAndPush0(BBlock bb, BBlockList * ploop, BBlockList *pstack)
{
	/* Return if it's already in the loop */
	if (listMemq(BBlock)(*ploop, bb)) return;

	listPush(BBlock, bb, *ploop);

	listPush(BBlock, bb, *pstack);
}

/****************************************************************************
 *
 *
 * :: Contructors and Destructors
 *
 *
 ****************************************************************************/


local Edge
lpEdgeNew(BBlock tail, BBlock head)
{
	Edge	edge = (Edge) stoAlloc(OB_Other, sizeof(struct _Edge));

	edge->tail = tail;
	edge->head = head;

	return edge;
}

local void
lpEdgeFree(Edge edge)
{
	stoFree(edge);
}

local Loop
lpLoopNew(BBlock headerBlock, BBlockList blockList, FlowGraph flog)
{
	BitvClass	bitvClass = bitvClassCreate(flogBlockC(flog));
	Loop	loop = (Loop) stoAlloc(OB_Other, sizeof(struct _Loop));

	loop->header	  = headerBlock;
	loop->graph	  = flog;
	loop->blockList	  = blockList;
	loop->bitvClass   = bitvClass;

	loop->blockSet    = bitvNew(bitvClass);

	bitvClearAll(bitvClass, loop->blockSet);

	listIter(BBlock, bb, blockList, {

		assert(bb->label < flogBlockC(flog));

		bitvSet(bitvClass, loop->blockSet, bb->label);

	});

	return loop;
}

void
lpLoopFree(Loop loop)
{
	listFree(BBlock)(loop->blockList);
	bitvFree(loop->blockSet);

	stoFree(loop);
}

local Dominators
lpDominatorsNew(FlowGraph flog)
{
	int		nBlocks = flogBlockC(flog);
	Dominators 	doms;

	doms 		= (Dominators) stoAlloc(OB_Other, sizeof(*doms));
	doms->doms 	= (Bitv *) stoAlloc(OB_Other, nBlocks * sizeof(Bitv));
	doms->nBlocks 	= nBlocks;
	doms->bitvClass 	= bitvClassCreate(nBlocks);

	/* NOTE: Don't store bitvClass info on the flog ! */

	doms->graph	= flog;

	return doms;
}

void
lpDominatorsFree(Dominators doms)
{
	int i;

	for (i = 0; i < doms->nBlocks; i++)
		bitvFree(doms->doms[i]);

	bitvClassDestroy(doms->bitvClass);

	stoFree(doms);
}

/****************************************************************************
 *
 *
 * :: Debug
 *
 *
 ****************************************************************************/

void
lpDominatorsPrintDb(Dominators doms)
{
	assert(doms->nBlocks == bitvClassSize(doms->bitvClass));

	fprintf(dbOut, "Dominators:\n\n");
      
	flogIter(doms->graph, bbA, {

		fprintf(dbOut, "[%d dom: ", bbA->label);

		flogIter(doms->graph, bbB, {

			if (lpIsDom(doms, bbA, bbB))
				fprintf(dbOut, "%d ", bbB->label);

			else if (bbA == bbB)
				bug("lpDominatorsPrint: node %d is NOT dominating itself...",
				    bbA->label);
		});
		fprintf(dbOut, "]\n");
	});

	fprintf(dbOut, "\n");

}

void
lpEdgeListPrintDb(EdgeList edges)
{
	listIter(Edge, edge, edges, {
		fprintf(dbOut, "( %d -> %d ) ",
			edge->tail->label, edge->head->label);
	});

	fprintf(dbOut, "\n\n");
}

void
lpLoopPrintDb(Loop loop)
{
	fprintf(dbOut, "Loop (%p):  headerBlock = %d\n",
		loop, loop->header->label);

	assert(loop->bitvClass);
	assert(bitvClassSize(loop->bitvClass) >= listLength(BBlock)(loop->blockList));

	fprintf(dbOut, "Bitv: ");
	bitvPrintDb(loop->bitvClass, loop->blockSet);

	fprintf(dbOut, "Blocks in the loop are: ");

	listIter(BBlock, bb, loop->blockList, {
		fprintf(dbOut, "%d ", bb->label);
	});

	fprintf(dbOut, "\n\n");

}

void
lpLoopListPrintDb(LoopList loops)
{
	listIter(Loop, loop, loops, { 
		lpLoopPrintDb(loop);
	});
}

