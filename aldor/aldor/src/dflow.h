/*****************************************************************************
 *
 * dflow.h: Data-flow analysis.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _DFLOW_H_
#define _DFLOW_H_

# include "axlobs.h"

struct dflowInfo {
	Bitv		in;
	Bitv		gen;
	Bitv		temp;
	struct {
		Bitv	kill;
		Bitv	out;
	}	exit[NARY];
};
	
typedef void	(*DFlowFillGKFun)  (FlowGraph, BBlock);

/* Viewed Forwards */
#define		  dfFwdIn(bb)		((bb)->dfinfo->in)
#define		  dfFwdGen(bb)		((bb)->dfinfo->gen)
#define		  dfFwdTemp(bb)		((bb)->dfinfo->temp)
#define		  dfFwdKill(bb,j)	((bb)->dfinfo->exit[j].kill)
#define		  dfFwdOut(bb,j)	((bb)->dfinfo->exit[j].out)

/* Viewed Backwards */
#define		  dfRevIn(bb)		((bb)->dfinfo->in)
#define		  dfRevGen(bb)		((bb)->dfinfo->gen)
#define		  dfRevTemp(bb)		((bb)->dfinfo->temp)
#define		  dfRevKill(bb)		((bb)->dfinfo->exit[0].kill)
#define		  dfRevOut(bb)		((bb)->dfinfo->exit[0].out)


typedef enum {DFLOW_Intersection, DFLOW_Union } DFlowType;

/* DFLOW_Intersection 	=> IN = /\ OUTi, for each OUTi predecessor.
 * DFLOW_Union		=> IN = \/ OUTi, for each OUTi predecessor.
 */

typedef	void (* DFlowInitFun)(FlowGraph);

/*
 * Add/remove dataflow info in graph nodes.
 */
extern void	dflowNewGraphInfo  (FlowGraph,
				    Length nbits, DFlowFillGKFun);
extern void	dflowNewBlockInfo  (BBlock,
				    Length nbits, DFlowFillGKFun);
					

extern void	dflowFreeGraphInfo (FlowGraph);
extern void	dflowFreeBlockInfo (BBlock);

extern int	dflowPrintBlockInfo(FILE *, BBlock);

/*
 * Dataflow iterations.
 */

/* $$ */
extern int	dflowFwdStep   	   (FlowGraph, DFlowType);
extern int	dflowRevStep   	   (FlowGraph, DFlowType);
			/*
			 * Returns the number of "in" sets which changed.
			 */

extern int	dflowFwdIterate	   (FlowGraph, DFlowType, int cutoff,
				    int *pcount, DFlowInitFun);
extern int	dflowRevIterate	   (FlowGraph, DFlowType, int cutoff,
				    int *pcount, DFlowInitFun);
			/*
			 * Iterate until "in", "out", respv sets do not 
			 * change, or until "cutoff" steps have been 
			 * performed.
 			 * "equations" is the type of equations to use.
			 * If "cutoff" is negative, then no limit is imposed.
			 *
			 * Returns the number of "in" sets which changed on
			 * the last step.
			 *
			 * If "pcount" is non-null, then "*pcount" is set to
			 * the number of iterations.
			 */

#endif /* !_DFLOW_H_ */
