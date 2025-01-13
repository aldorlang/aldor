/*****************************************************************************
 *
 * inlutil.h: Utilities for the inliner.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _INLUTIL_H_
#define  _INLUTIL_H_

#include "axlobs.h"

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

extern Foam	inuGetClosFrVar		(Foam);

extern Syme	inuGetSymeFrClosVar	(Foam);
extern Syme	inuGetSymeFrEnv		(Foam);

extern Foam	inuGetConstEnvFrClosVar	(Foam);
extern Foam	inuGetConstEnvFrEnv	(Foam);

extern Foam	inuGetPushEnvFrClosVar	(Foam);
extern Foam	inuGetPushEnvFrVar	(Foam);
extern Bool	inuIsLocalEnv		(Foam);

extern void	inuProgInit		(Foam);
extern void	inuProgUpdate		(Foam);
extern void	inuProgFini		(Foam);

extern void	inuUnitInit		(Foam);
extern void	inuUnitFini		(Foam);

extern Foam	inuPeepExpr		(Foam);

extern void	inuUnitInfoRefresh	(Foam);

#endif
