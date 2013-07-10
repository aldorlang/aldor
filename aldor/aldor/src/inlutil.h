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
