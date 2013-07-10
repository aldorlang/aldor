/*****************************************************************************
 *
 * priq.h: Priority Queue data structure.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _PRIQ_H
#define _PRIQ_H

# include "axlport.h"

typedef struct priq     *PriQ;

typedef double          PriQKey;
typedef Pointer         PriQElt;

typedef void (* PriQEltFreeFun)		(PriQElt);
typedef void (*PriQMapFn)		(PriQKey, PriQElt);

 
struct priqPart {
	PriQKey         key;            
	PriQElt         entry;
};

struct priq {
	Length		size;		/* Number slots allocated in argv. */
	Length		argc;		/* Number of slots used in argv. */
	struct priqPart	*argv;
};

#define 	priqCount(pq)	((pq)->argc)

#define		priqMinKey	((double) 0)

extern PriQ     priqNew        	(Length argcGuess);
extern void     priqFree       	(PriQ);
extern void	priqFreeDeeply	(PriQ, PriQEltFreeFun);

extern void     priqInsert     	(PriQ, PriQKey, PriQElt);
extern PriQElt  priqExtractMin  (PriQ, PriQKey *);
extern PriQElt	priqPeekMin	(PriQ, PriQKey *);

extern int      priqCheck      	(PriQ);
extern int     	priqPrint      	(FILE *, PriQ);
extern void     priqMap		(PriQMapFn, PriQ);

#endif /* _PRIQ_H_ */




