/*****************************************************************************
 *
 * depdag.h: Dependency DAGs
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _DEPDAG_H_
#define _DEPDAG_H_

# include "axlobs.h"


/*
 * Currently depDag labels are DefGroup objects; leaves are nodes
 * with empty dependsOn lists. The pending flag is set during the
 * top-down pass: if a pending node is encountered during this pass
 * then a cycle has been detected.
 */
struct depDag
{
	void *		label;		/* Node label (NULL for root) */
	DepDagList	dependsOn;	/* Non-lazy dependencies */
	Bool		pending;	/* Generating code for dependencies */
	Bool		finished;	/* Code generated */
};


/* Accessors */
#define depdagLabel(d)		((d)->label)
#define	depdagDependsOn(d)	((d)->dependsOn)
#define depdagIsPending(d)	((d)->pending)
#define depdagIsFinished(d)	((d)->finished)


/* Mutators */
#define depdagSetLabel(d, s)		(depdagLabel(d) = s)
#define	depdagSetDependsOn(d, s)	(depdagDependsOn(d) = s)
#define depdagSetPending(d, s)		(depdagIsPending(d) = s)
#define depdagSetFinished(d, s)		(depdagIsFinished(d) = s)


/* Other queries */
#define depdagIsNode(d)			(depdagDependsOn(d))
#define depdagIsLeaf(d)			(!depdagIsNode(d))


/* Exported functions */
extern DepDag	depdagNewLeaf		(void *);
extern DepDag	depdagAddDependency	(DepDag, DepDag);
extern void	depdagFree		(DepDag);
extern void	depdagFreeDeeply	(DepDag);

#endif /* !_DEPDAG_H_ */
