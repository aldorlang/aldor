/*****************************************************************************
 *
 * ccomp.h: C compiler interface
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _CCOMP_H_
#define _CCOMP_H_

#include "axlobs.h"

/*
 * Option handling
 */
extern int	ccOption		(String);

extern void	ccSetVerbose		(Bool);
extern void	ccSetDebug		(Bool);
extern void	ccSetProfile		(Bool);
extern void	ccSetOptimize		(Bool,Bool);
extern void	ccSetOutputFile		(String);
extern void	ccSetLineNos		(Bool);
extern Bool	ccLineNos		(void);
extern Bool	ccDoStandardC		(void);

/*
 * Actually do stuff
 */
extern void	ccGetReady		(void);
extern void	ccCompileFile		(String newwd, FileName);
extern void	ccLinkProgram		(String newwd, FileName *, int);
extern void	ccGoProgram		(FileName, int argc1, String *argv1);

#endif /* _CCOMP_H_ */
