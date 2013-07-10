/*****************************************************************************
 *
 * of_deada.h: Dead Assignment Elimination
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OF_DEADA_H_
#define _OF_DEADA_H_

typedef struct {
	int  stmtId;
	Bitv live;
} *LiveInfo;

DECLARE_LIST(LiveInfo);

typedef struct {
	/* Should be a vector so binary search is possible */
	LiveInfoList *vars;
	BitvClass    bvClass;
} *LiveVars;


void	daSetCutOff	(int);
void	deadAssign	(Foam);

#endif /*!_OF_DEADA_H_*/

