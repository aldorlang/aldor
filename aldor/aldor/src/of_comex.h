/*****************************************************************************
 *
 * of_comex.h: Common Subexpressions Elimination
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef OF_COMEX_H
#define OF_COMEX_H

#include "axlobs.h"


struct _ExpInfo {
		/* NOTE: EXP and STMT fields use structure sharing */
	Foam		exp; 		/* expression foam code */
	int		expNo;		/* expression index */
	int		phantomNo;	/* number phantom bit */

	AInt		newLoc;		/* local used for the transformation */
	FoamList	copies;		/*     "    "      "        "        */
	Bool		evaluated;	/* false -> use newLoc instead of exp*/

};


extern void cseUnit	(Foam);
extern void cseFlog	(FlowGraph, Foam);


#endif
