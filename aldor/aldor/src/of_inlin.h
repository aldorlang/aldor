/*****************************************************************************
 *
 * of_inlin.h: Inlining Foam functions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OF_INLIN_H_
#define _OF_INLIN_H_

# include "axlobs.h"

extern void     	inlineUnit	    	(Foam, Bool, int, Bool);
extern OptInfo		inlInfoNew		(Stab, Foam, Syme, Bool);

extern Bool		inlInlinable		(Stab, Syme);
extern void		inlSetGenerators	(void);

/* To understand the meaning of the following `magic numbers' look how they
 * are used in of_inlin.c or inlutil.c
 */
#define InlLoopMagicNumber		3
#define InlInnerLoopMaxIter		30000
#define InlUnknownCallsMagicNumber	20
#define InlCallMagicNumber		5

#define InlOverGrowthFactor		1.4
#define InlCalledOnceFactor		0.01
#define InlStandardTimeFactor		1
#define InlInlineMeTimeFactor		0.05
#define InlInlineMeSpaceFactor		60
#define InlSingleStmtMagic		2
#define InlBaseFactor			1.1
#define InlParIsGeneratorMagic		0.016

/*
 * The first of these limits is quite critical: make it too
 * large and some programs will require tens of minutes to
 * compile on a PII 300. Make it too low and the user won't
 * get enough inlining and their programs will run slowly.
 *
 * The original limit is 1000 bytes per prog: 5000 bytes per
 * prog seems to be an appropriate ceiling although for some
 * programs a limit of 100000 is acceptable.
 *
 * This limit can now be controlled via -Qinline-size=NNN.
 */
#define InlProgCutOff			1000
#define InlFlogCutOff			2500

#endif /* !_OF_INLIN_H_ */
