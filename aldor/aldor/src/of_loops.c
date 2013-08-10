/*****************************************************************************
 *
 * of_loops.c: Loops Optimization
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
/****************************************************************************
 *
 * Open Problems:
 *
 *  - after a preheader is created, udInfoBlock() must be modified.
 *	Solutions:
 *	   a- traverse the flog and check each ud list
 *	   - 
 *
 *  - after a preheader is created, there should be a new bit in each
 *	loop->blockSet, and also loop->blockList(s) should be updated.
 *	Solutions:
 *	   a- the problem should exist only for nested loops. If loops are
 *	     sorted so that external loops are processed before internal ones,
 *	     then a block > nbits is considered an external block
 *	   b- all the loops that must be still examined are updated. Not so
 *	     hard. A dirty trick could avoid alloc/free of each bitv.
 *
 ****************************************************************************/

#include "debug.h"
#include "flog.h"
#include "loops.h"
#include "of_loops.h"
#include "opttools.h"
#include "store.h"
#include "usedef.h"

/****************************************************************************
 *
 * :: Debug
 *
 ****************************************************************************/

static Bool loopDebug	= false;

#define loopDEBUG	DEBUG_IF(loop)	afprintf

/****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/
#define	loopDefNo(foam)			((foam)->foamGen.hdr.info.defNo)

/****************************************************************************
 *
 * :: Type Definitions
 *
 ****************************************************************************/

struct _InvInfo {
	Foam * 		foamPtr;   /* pointer to the definition */
	BBlock		block;	   /* block to which belong 	*/
};

DECLARE_LIST(InvInfo);
CREATE_LIST(InvInfo);

/****************************************************************************
 *
 * :: Global Data Structures
 *
 ****************************************************************************/

struct  {
	Foam		unit;
	FlowGraph	flog;
	Dominators	doms;

	Bitv		domExits;

	int		numLocs;
	int		numPars;

	InvInfo 	* invInfov;
	int		numInv;
	int		numDefs;

} loopInfo;

/*****************************************************************************
 *
 * :: Local Prototypes
 *
 ****************************************************************************/

local void		loopFindAndMoveInvariantsFrLoop	(LoopList);

local void		loopInvariantsFindFrLoop	(Loop);
local Bool		loopInvariantsFindFrBlock  	(BBlock, Loop);

local Bool		loopIsInvariantExp		(Foam, Loop);


local void		loopInvariantsFilterFrLoop	(Loop);

local BBlock		loopInvariantsMoveFrLoop	(Loop);
local Foam		loopPreHeaderCodeCreate		(Loop);

local int		loopDefinitionsReset		(Loop/* , Bitv*/);

local Bool		loopBlockDominatesAllExits	(BBlock, BBlockList);

local Bitv		loopFindExitsDominators		(Loop);

local InvInfo		loopInvInfoNew			(Foam *, BBlock);

local void		loopInvariantsPrintDb		(int);

local void		loopUpdate		     (Loop, BBlock, LoopList);

/****************************************************************************
 *
 * :: Main Entry Points
 *
 ****************************************************************************/

void
loopUnit(Foam foam)
{
	int 		i;
	Foam 		defs = foam->foamUnit.defs, def, prog;
	FlowGraph	flog;

	loopDebug = 1;

	assert(foamTag(foam) == FOAM_Unit);

	loopInfo.unit = foam;

	/* !! No loop optimization for const 0 */

	for (i = 1; i < foamArgc(defs); i++) {

		def = defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		prog = def->foamDef.rhs;

		loopInfo.numLocs = foamDDeclArgc(prog->foamProg.locals);
		loopInfo.numPars = foamDDeclArgc(prog->foamProg.params);

		flog = flogFrProg(prog, FLOG_UniqueExit);

		loopInvariantsMoveFrFlog(flog);

		def->foamDef.rhs = flogToProg(flog);
	}
}


/* !!! There is something that must be changed. Usedef must decide that
 * he need a FLOG_Union, not the client.
 */
void
loopInvariantsMoveFrFlog(FlowGraph flog)
{
	Dominators	doms;
	LoopList	loops;

	loopInfo.flog = flog;

	/* ----- First step: Build usedef chains ------ */

	if (!usedefChainsFrFlog(flog, UD_OUTPUT_UdList))
		return;

	/* ----- Second step: Find All the Natural Loops ------ */

	loops = lpNaturalLoopsFrFlog(flog, &doms);

	if (!loops) {
		lpDominatorsFree(doms);
		return;
	}

	loopInfo.doms = doms;

	/* ----- Third step: unify loops with some header ------ */

	loops = lpUnifyCommonHeaders(loops);

	/* ----- Fourth step: Optimize each loop ------ */
	
	for (; loops; loops = cdr(loops))
		loopFindAndMoveInvariantsFrLoop(loops);

	usedefChainsFreeFrFlog(flog);

}


/* Given a loopList , find all the invariant computations for the
 * first element, create the preheader and modify the flowgraph, and update
 * the dataflow info for the remaining loops.
 */
local void
loopFindAndMoveInvariantsFrLoop(LoopList loops)
{
	BBlock	preHeader;
	Loop	loop = car(loops);

	otProgInfoInit(OT_ASSOCIATION_LIST, 
		       loopInfo.numLocs, loopInfo.numPars, loopInfo.unit);

	loopInvariantsFindFrLoop(loop);

	assert(loopInfo.numInv);

	if (loopInfo.numInv == 1) {
		otProgInfoFini();
		return;
	}

	loopInvariantsFilterFrLoop(loop);

	otProgInfoFini();

	assert(loopInfo.numInv);

	if (loopInfo.numInv == 1)  return;

	preHeader = loopInvariantsMoveFrLoop(loop);

	if (cdr(loops))
	    loopUpdate(loop, preHeader, cdr(loops));


}

/****************************************************************************
 *
 * :: Find Invariants
 *
 ****************************************************************************/

local void
loopInvariantsFindFrLoop(Loop loop)
{
	int		numDefs;
	Bool		changed;
	BitvClass	class = loop->bitvClass;
	
	loopInfo.domExits = loopFindExitsDominators(loop);

	/* First: reset all invariant info on definitions */

	numDefs = loopDefinitionsReset(loop/*, domExits*/);

	loopInfo.numInv = 1;   /* NOTE: must start from 1 */

	if (numDefs == 0) return;  /* No potential invariants */

	do {
		changed = false;
		listIter(BBlock, bb, loop->blockList, {

			/* Consider only blocks which dominates exits */
			if (!bitvTest(class, loopInfo.domExits, bb->label))
				continue;

			if (loopInvariantsFindFrBlock(bb, loop))
				changed = true;

		});

	} while (changed);

	/* bitvFree(domExits); */

	if (loopDebug) loopInvariantsPrintDb(numDefs);

	return;
}

local void
loopInvariantsPrintDb(int numDefs)
{
	int i;

	if (loopInfo.numInv == 0)
		fprintf(dbOut, "No invariants found among %d definitions.\n",
			numDefs);
	else {
		fprintf(dbOut, "Found %d invariants among %d definitions.\n",
			loopInfo.numInv - 1, numDefs);

		for (i = 1; i < loopInfo.numInv; i++) {
			fprintf(dbOut, "Inv %d in block %d: ", i,
				loopInfo.invInfov[i]->block->label);

			foamPrintDb(*(loopInfo.invInfov[i]->foamPtr));
		}
	}

}

#define LOOP_InvState_Unknown		0
#define LOOP_InvState_Invalid		-1
#define LOOP_InvState_Valid		1

local Bool
loopInvariantsFindFrBlock(BBlock bb, Loop loop)
{
	Foam 		lhs, rhs, stmt, seq = bb->code;
	int		i, state;
	Bool		found = false;
	InvInfo *	invInfov = loopInfo.invInfov;
	FoamList	defs;

	for (i = 0; i < foamArgc(seq); i++) {

		stmt = seq->foamSeq.argv[i];

		if (!otIsDef(stmt)) continue;

		if (loopDefNo(stmt) != LOOP_InvState_Unknown)
			continue;
		
		defs = (FoamList) otGetVarInfoList(stmt->foamDef.lhs);

		assert(defs);

		if (!listIsSingleton(defs)) {
			loopDefNo(stmt) = LOOP_InvState_Invalid;
			continue;
		}

		rhs = stmt->foamDef.rhs;

		state = loopIsInvariantExp(rhs, loop);

		if (state == LOOP_InvState_Invalid)
			loopDefNo(stmt) = LOOP_InvState_Invalid;
	        else if (state != LOOP_InvState_Unknown) {

			found = true;

			lhs = stmt->foamDef.lhs;

			loopDefNo(stmt) = loopInfo.numInv;
			invInfov[loopInfo.numInv] =
				loopInvInfoNew(&(seq->foamSeq.argv[i]), bb);

			/* otAddVarInfo(invInfov[loopInfo.numInv], lhs); */

			loopInfo.numInv += 1;

			/* loopInvalidateNotUniqueDef0(stmt); */
		}
	}

	return found;
}

local int
loopIsInvariantExp(Foam foam, Loop loop)
{
	foamIter(foam, arg, {
		int state = loopIsInvariantExp(*arg, loop);

		if (state == LOOP_InvState_Unknown ||
		    state == LOOP_InvState_Invalid)
			return state;
	});

	if (otIsLocalVar(foam)) {
		UdInfoList udInfol = udReachingDefs(foam);

		/* Has this var a unique reaching def. and is this def marked
		 * invariant ?
		 */

		if (listIsSingleton(udInfol)) {
			UdInfo ud = car(udInfol);

			if (!lpIsBlockInLoop(udInfoBlock(ud), loop))
				return LOOP_InvState_Valid;
			
			if (loopDefNo(udInfoDef(ud)) == LOOP_InvState_Invalid)
    				return LOOP_InvState_Invalid;

			if (loopDefNo(udInfoDef(ud)) == LOOP_InvState_Unknown)
    				return LOOP_InvState_Unknown;

			return LOOP_InvState_Valid;
		}

		listIter(UdInfo, ud, udInfol, {
			if (lpIsBlockInLoop(udInfoBlock(ud), loop))
				return LOOP_InvState_Invalid;
		});

		return LOOP_InvState_Valid;
	}

	if (otIsNonLocalVar(foam)) {
		if (otIsConstSyme(foamSyme(foam))) 
			return LOOP_InvState_Valid;
		else
			return LOOP_InvState_Invalid;
	}

	if (foamTag(foam) == FOAM_Cast) return LOOP_InvState_Valid;
	if (otIsMovableData(foam))	return LOOP_InvState_Valid;
	if (foamTag(foam) == FOAM_RRFmt) return LOOP_InvState_Valid;

	if (foamTag(foam) == FOAM_BCall &&
	    !foamBValInfo(foam->foamBCall.op).hasSideFx)
		return LOOP_InvState_Valid;

	if (foamTag(foam) == FOAM_OCall)
		/*!! Could check for non-side-effecting expressions here. */
		return LOOP_InvState_Invalid;

	if (foamTag(foam) == FOAM_CCall) {
		if (otIsForcer(foam->foamCCall.op))
			return LOOP_InvState_Valid;

		/*!! Could check for non-side-effecting expressions here. */
		return LOOP_InvState_Invalid;
	}
	
	if (foamTag(foam) == FOAM_PCall)
		return LOOP_InvState_Invalid;

	return LOOP_InvState_Invalid;
}

/* Assign 0 to each def in the blocks in LOOP and build
 * association lists: (Var in LOOP) -> (All definition of Var in LOOP)
 * Return the number of defs found (== potential invariants)
 */
local int
loopDefinitionsReset(Loop loop /*, Bitv domExits $$*/ ) 
{
	int 		numDefs = 0, i;

	listIter(BBlock, bb, loop->blockList, {
		int 	i;
		Foam	 seq = bb->code;

		/* if (!bitvTest(class, domExits, bb->label)) continue; */

		for (i = 0; i < foamArgc(seq); i++) {

			Foam stmt = seq->foamSeq.argv[i];

			if (otIsDef(stmt)) {
				loopDefNo(stmt) = 0;
				numDefs++;

				otAddVarInfo(stmt, stmt->foamDef.lhs);
			}
		}
	});
	
	if (numDefs) {
		loopInfo.invInfov = (InvInfo *) 
			stoAlloc(OB_Other,
				 sizeof(InvInfo) * (numDefs + 1));

		for (i = 0; i < numDefs + 1; i++)
			loopInfo.invInfov[i] = (InvInfo) 0;
	}
	loopInfo.numDefs = numDefs;

	return numDefs;
}
/****************************************************************************
 *
 * :: Filter Invariants
 *
 ****************************************************************************/
local Bool	loopFilterExp(Foam foam);
/* local void	loopInvalidateNotUniqueDef(void);*/

local Bool	loopFilterDependencies(Foam foam);
local Bool	loopIsStillInvariant(Foam foam);

/* For each invariant x := (exp), check the following conditions:
 *
 *   a)  this must be the unique def. for x in LOOP.
 *   b)  all uses of x in LOOP are reached only by this definition.
 *
 *   How: build a vector: var -> invariant.
 *   (b) It's easy using usedef chains: traverse the loop, for each use look
 *	at the usedef
 */
local void
loopInvariantsFilterFrLoop(Loop loop)
{
	Bool	removed = false;
	BitvClass class = loop->bitvClass;

	/* 1) Check, for each inv x := ..., that all uses of x in LOOP
	 *    can be reached only by the invariant.
 	 */
	
	listIter(BBlock, bb, loop->blockList, {

		if (!loopFilterExp(bb->code))
			removed = true;

	});

	if (!removed) return;

	/* 2) Check if other invariants must be removed 
	 *    due to removed invariants.
	 */


	while (removed) {

		removed = false;

		listIter(BBlock, bb, loop->blockList, {

			/* Consider only blocks which dominates exits */
			if (!bitvTest(class, loopInfo.domExits, bb->label))
				continue;

			if (!loopFilterDependencies(bb->code))
				removed = true;

		});
}

}


/* For each var use, look at the reaching defs:
 *   is there is an invariant among them, the list must be a singleton
 *   otherwise invalid invariant.
 */   
local Bool
loopFilterExp(Foam foam)
{
	UdInfoList	udInfol;
	Bool		removed = false;
	FoamList 	defs;
	Foam		invDef;

	foamIter(foam, arg, {
		
		if (otIsDef(*arg)) {
			if (!loopFilterExp((*arg)->foamDef.rhs))
				removed = true;
		}
		else
			if (!loopFilterExp(*arg))
				removed = true;
	});

	if (!otIsVar(foam)) 
		return removed;

	/* Take all definition for this var inside the loop */
	defs = (FoamList) otGetVarInfoList(foam);

	if (!defs)  return removed;

	if (!listIsSingleton(defs)) return removed;
	    
	if (loopDefNo(car(defs)) == LOOP_InvState_Invalid) return removed;

	if (loopDefNo(car(defs)) == LOOP_InvState_Unknown) {
		loopDefNo(car(defs)) = LOOP_InvState_Unknown;
		return removed;
	}

	/* There is an invariant with this lhs. */

	/* Is this the only reaching def for foam ? */

	udInfol = udReachingDefs(foam);
	invDef  = car(defs);

	if (!listIsSingleton(udInfol) &&
	    udInfoDef(car(udInfol))->foamDef.lhs != foam) {

		/* The invariant is marked invalid */

		loopInfo.invInfov[loopDefNo(invDef)] = (InvInfo) 0;
		loopDefNo(invDef) = LOOP_InvState_Invalid;

		loopInfo.numInv--;
		
		loopDEBUG(dbOut,"* Invariant rejected by loopFilterExp *\n");

		removed = true;

	}

	return removed;

}

local Bool
loopFilterDependencies(Foam foam)
{
	Foam stmt;
	int	i;
	Bool	changed = false;

	assert(foamTag(foam) == FOAM_Seq);


	for (i = 0; i < foamArgc(foam); i++) {

		stmt = foam->foamSeq.argv[i];

		if (!otIsDef(stmt)) continue;

		if (loopDefNo(stmt) == LOOP_InvState_Invalid) continue;

		assert(loopDefNo(stmt));

		/* Found an invariant. Is it still valid ? */

		if (!loopIsStillInvariant(stmt->foamDef.rhs)) {

			/* The invariant is marked invalid */

			loopInfo.invInfov[loopDefNo(stmt)] = (InvInfo) 0;
			loopDefNo(stmt) = LOOP_InvState_Invalid;

			loopInfo.numInv--;
		
			loopDEBUG(dbOut,"* Invariant rejected by loopFilterDependencies *\n");

			changed = true;
		}

	}

	return changed;
}


local Bool
loopIsStillInvariant(Foam foam)
{
	UdInfoList udInfol;
	UdInfo ud;

	foamIter(foam, arg, {

		if (!loopIsStillInvariant(*arg))
			return false;
	});

	if (!otIsVar(foam))
		return true;

	udInfol = udReachingDefs(foam);

	/* Has this var still a unique reaching def. and is this def marked
	 * invariant ?
	 */

	assert(listIsSingleton(udInfol));

	ud = car(udInfol);

	assert(loopDefNo(udInfoDef(ud)));

	if (loopDefNo(udInfoDef(ud)) == LOOP_InvState_Invalid)
		return false;

	return true;
}

/****************************************************************************
 *
 * :: Move Invariants in the Preheader
 *
 ****************************************************************************/


local BBlock
loopInvariantsMoveFrLoop(Loop loop)
{
	int		i, j;
	BBlock		bbPre, bbHeader = loop->header;
	Length		newLab;
	FlowGraph	flog = loopInfo.flog;

	Foam		newSeq = loopPreHeaderCodeCreate(loop);

	flogFixEntries(flog);    /* $$ !! */

	/* Create a new block with PREHEADER as code .. */

	newLab = flogReserveLabel(flog);

	bbPre = bbNew(newSeq, newLab);

	/* Attach it to the graph... */
	flogSetBlock(flog, newLab, bbPre);

	/* .. and set up entry and exit edges */

	bbufNeed(bbPre->exits, 1);
	bbSetExit(bbPre, int0, bbHeader);	/* preheader -> header */
	bbSetExitC(bbPre, 1);

	bbufNeed(bbPre->entries, bbEntryC(bbHeader));

	/* Modify each entry of bbHeader not from LOOP
 	 * so that now are entries in bbPre
	 */

	for (i = 0; i < bbEntryC(bbHeader); i++) {

		BBlock entryBlock = bbEntry(bbHeader, i);

		if (lpIsBlockInLoop(entryBlock, loop)) continue;
		
		for (j = 0; j < bbExitC(entryBlock); j++)
			if (bbExit(entryBlock, j) == bbHeader)
				bbSetExit(entryBlock, j, bbPre);
	}

	/* If bbHeader was the first, now bbPre will be the first */

	if (bbHeader == flog->block0)  flog->block0 = bbPre;

	/* NOTE: entries have not been modified, but this isn't important,
	 * because they have been invalidated.
	 */

	return bbPre;

}

local Foam
loopPreHeaderCodeCreate(Loop loop)
{
	Foam	preHeader = foamNewEmpty(FOAM_Seq, loopInfo.numInv);
	Foam	foam;
	InvInfo	* invInfov = loopInfo.invInfov;
	int	j = 0, i;

	for (i = 1; i < loopInfo.numDefs + 1; i++) {

		if (!invInfov[i]) continue;

		foam = *(invInfov[i]->foamPtr);

		assert(loopDefNo(foam) > 0);

		*(invInfov[i]->foamPtr) = foamNewNOp();

		preHeader->foamSeq.argv[j++] = foam;
	}

	assert(j == loopInfo.numInv - 1);

	/* Add: (Goto the header block) */
	preHeader->foamSeq.argv[j] = foamNewGoto(loop->header->label);

	if (DEBUG(loop)) {
		fprintf(dbOut, "New preheader: \n");
		foamPrintDb(preHeader);
	}

	return preHeader;

}

/****************************************************************************
 *
 * :: Updated dflow info and loops
 *
 ****************************************************************************/

local void	loopUpdateBlocks	(Loop, BBlock, LoopList);
local void	loopUpdateDominators	(Loop, BBlock);
local void	loopUpdateUseDefChains	(void);

/******************************************************************************
 *
 * :: Update dflow info and loops
 *
 *****************************************************************************/

local void
loopUpdate(Loop loop, BBlock preHeader, LoopList loops)
{

	loopUpdateBlocks(loop, preHeader, loops);
	loopUpdateDominators(loop, preHeader);
	loopUpdateUseDefChains();
}


local void
loopUpdateBlocks(Loop loop, BBlock preHeader, LoopList loops)
{
	BitvClass	oldClass, newClass;
	Length		newbit = bitvClassSize(loop->bitvClass);

	listIter(Loop, loop0, loops, {
		oldClass = loop0->bitvClass;
		newClass = bitvClassCreate(bitvClassSize(oldClass) + 1);

		assert(bitvClassSize(newClass) == preHeader->label + 1);

		loop0->blockSet = 
			bitvResize(newClass, oldClass, loop0->blockSet);

		loop0->bitvClass = newClass;

		if (lpIsBlockInLoop(loop->header, loop0)) {
			bitvSet(newClass, loop0->blockSet, newbit);
			listPush(BBlock, preHeader, loop0->blockList);
		}

		bitvClassDestroy(oldClass);
	});
}

local void
loopUpdateDominators(Loop loop, BBlock preHeader)
{
	Dominators doms = loopInfo.doms;
	int	   	i;
	BitvClass	newClass, oldClass;

	oldClass = doms->bitvClass;
	newClass = bitvClassCreate(bitvClassSize(oldClass) + 1);

	for (i = 0; i < doms->nBlocks; i++)
		doms->doms[i] = bitvResize(newClass, oldClass, doms->doms[i]);

	doms->nBlocks += 1;
	bitvClassDestroy(doms->bitvClass);
	doms->bitvClass = newClass;

	assert(bitvClassSize(newClass) == doms->nBlocks);
	assert(bitvClassSize(newClass) == bitvClassSize(loop->bitvClass) + 1);

	/* 1) Each dominator of loop->header become a dominator of preheader.*/

	flogIter(loopInfo.flog, bb, {

		if (bb == preHeader) continue;

		if (lpIsDom(doms, bb, loop->header))
			lpSetDom(doms, bb, preHeader);
	});

	/* 2) Preheader become dominator of header. */

	lpSetDom(doms, preHeader, loop->header);
	
}

/* Now this operation is expensive, because all the ud-chains are recomputed
 * again.
 * ToDo:
 *   The UdInfo can be a pair (BB, index), where index is such that
 *    bb->code->foamSeq.argv[index] is the reaching def.
 *   In such way each (NOp) generated when a def is moved will have a UdInfo
 *   on it, (new-block, new-index), and the definition can be found
 *   dereferencing this chains.
 */
local void
loopUpdateUseDefChains()
{
	usedefChainsFreeFrFlog(loopInfo.flog);

	flogReuse(loopInfo.flog, FLOG_UniqueExit);

	usedefChainsFrFlog(loopInfo.flog, UD_OUTPUT_UdList);
}

/****************************************************************************
 *
 * :: Utility
 *
 ****************************************************************************/

/* Return a bitv where are on only bits corresponding to blocks in LOOP which
 * dominates all the loop exits
 */
local Bitv
loopFindExitsDominators(Loop loop)
{
	BitvClass	bclass = loop->bitvClass;
	Bitv	     	bres = bitvNew(bclass);
	BBlockList	exitBlocks = lpExitBlocksFrLoop(loop);

	bitvCopy(bclass, bres, loop->blockSet);

	listIter(BBlock, bb, loop->blockList, {

		if (!loopBlockDominatesAllExits(bb, exitBlocks))
			bitvClear(bclass, bres, bb->label);

	});

	return bres;
}


/* Return true iif BB dominates all the blocks in EXITS */

local Bool
loopBlockDominatesAllExits(BBlock bb, BBlockList exits)
{
	Dominators	doms = loopInfo.doms;

	listIter(BBlock, bbLoopExit, exits, {
		if (!lpIsDom(doms, bb, bbLoopExit))
		    	return false;
	});

	return true;
}

/*****************************************************************************
 *
 * :: Constructors and Destructors
 *
 ****************************************************************************/

local InvInfo
loopInvInfoNew(Foam * pstmt, BBlock bb)
{
	InvInfo invInfo = (InvInfo) stoAlloc(OB_Other, sizeof(*invInfo));

	invInfo->foamPtr = pstmt;
	invInfo->block   = bb;

	return invInfo;
}
