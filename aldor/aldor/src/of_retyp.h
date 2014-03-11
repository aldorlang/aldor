/*****************************************************************************
 *
 * of_retype.h: Foam Retyping And Cast Remotion.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OF_RETYPE_H
#define _OF_RETYPE_H

void	retypeUnit	(Foam);

/******************************************************************************
 *
 * :: Context
 *
 *****************************************************************************/

typedef struct retypeContext {
	Foam formats;
	Foam prog;
	int  nLocals;
	Foam *locDecls;
	Foam *parDecls;
	int *parLocs;
	int *nUses;
} *RetContext;

/* Mostly for testing */

extern RetContext rtcInit(Foam unit);
extern void rtcFree(RetContext context);
extern RetContext rtcNewProg(RetContext global, Foam prog, int nLocals);
extern RetContext rtcFreeProg(RetContext global, Foam prog);
extern Foam rtcCurrentDecl(RetContext context, Foam foam);
extern Foam rtcOriginalDecl(RetContext context, Foam foam);
extern Foam rtcNewDecl(RetContext context, Foam foam);
extern Bool rtcHasNewDecl(RetContext context, Foam foam);
extern void rtcAddUse(RetContext global, Foam foam);
extern Bool rtcRearrangeProg(RetContext context);
extern void rtcSetType(RetContext context, Foam foam, FoamTag type, AInt fmt);

#endif /* _OF_RETYPE_H */
