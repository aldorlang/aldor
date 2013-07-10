/*****************************************************************************
 *
 * genssa.h: Flog-to-SSA translation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GENSSA_H_
#define _GENSSA_H_

# include "axlobs.h"

/*
 * We keep a doubly-linked list of identifier uses. 
 */
struct foamuses_struct
{
	Foam		*foamp;
	FoamUses	prev, next;
};

#define fuEmpty()		((FoamUses)NULL)
#define fuFree(fu)		(stoFree(fu))
#define fuData(fu)		(fu->foamp)
#define fuPrevious(fu)		(fu->prev)
#define fuNext(fu)		(fu->next)
extern FoamUses	fuNew		(Foam *);
extern void	fuDestroy	(FoamUses);
extern FoamUses fuInsertBefore	(FoamUses, Foam *);
extern FoamUses fuInsertAfter	(FoamUses, Foam *);

/*
 * SSA form uses standard flogs organised to satisfy certain criteria (such
 * as the dominence-frontier property). Additionally we keep a table of links
 * to the definition of each local for fast use/def analysis and a table of
 * lists of links to the use of each local. Empty use-lists indicate dead
 * variables which can be removed.
 */
struct ssa_struct
{
	FlowGraph	flog;		/* flog satisfying SSA criteria */
	AInt		root;		/* flog entry point */
	AInt		blockc;		/* Number of bblocks */
	AInt		usec;		/* Number of usage lists */
	FoamUses	*uses;		/* foamLoc usage lists */
	VarPool		locals;		/* For easy addition of new locals */
	AInt		*idom;		/* Immediate dominator table */
	AInt		*dfnum;		/* Depth-first numbers */
	AInt		*semi;		/* Semi-dominator table */
	AInt		*best;		/* Best-skipped path */
	AInt		*ancestor;	/* Ancestor table */
	AInt		*samedom;	/* Same dominator table */
	AInt		*vertex;	/* Vertex table */
	AInt		*parent;	/* Parent table */
	AIntList	*bucketv;	/* For dominator computation */
	AIntList	*dfrontier;	/* Dominance frontiers */
	AIntList	*dtree;		/* Dominator tree nodes */
};

extern SSA		ssaFromFlog		(FlowGraph);
extern FlowGraph	ssaToFlog		(SSA);

extern	void		ssaEdgeSplit		(SSA);
extern	void		ssaDominatorTree	(SSA, AInt);
extern	void		ssaDominanceFrontiers	(SSA, AInt);
extern	void		ssaInsertPhiFunctions	(SSA);
extern	void		ssaRename		(SSA, AInt, AIntList *, AInt *);

#define foamIsPhiFunction(fm) \
	((foamTag(fm) == FOAM_BCall) && (fm->foamBCall.op == FOAM_BVal_ssaPhi))

#endif /* !_GENSSA_H_ */
