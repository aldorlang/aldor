/*****************************************************************************
 *
 * optfoam.h: Foam-to-foam optimizaion.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OPTFOAM_H_
#define _OPTFOAM_H_

#include "axlobs.h"


extern void	optInit		        (void);
extern void	optSetInit	        (void);
extern int	optSetOptimization	(String);
extern int	optSetStdOptimization	(void);
extern void	optSetLoopOption	(void);

extern Bool	optIsCcOptimizeWanted	(void);
extern Bool     optIsCcFNonStdWanted    (void);
extern Bool	optIsFloatFoldWanted	(void);
extern Bool	optIsDeadVarWanted	(void);
extern Bool	optIsMaxLevel		(void);
extern Bool 	optIsIgnoreAssertsWanted(void);
extern Bool	optIsKillPointersWanted	(void);

extern Foam     optimizeFoam    	(Foam);

#define	OPT_CPROP		(1 << 0)
#define OPT_JFLOW		(1 << 1)
#define OPT_CSE			(1 << 2)
#define OPT_DEADV		(1 << 3)
#define OPT_PEEP		(1 << 4)
#define OPT_CAST		(1 << 5)

#define optIsCPropPending(prog)	(foamOptInfo(prog)->optMask & OPT_CPROP)
#define optIsJFlowPending(prog)	(foamOptInfo(prog)->optMask & OPT_JFLOW)
#define optIsCsePending(prog)	(foamOptInfo(prog)->optMask & OPT_CSE)
#define optIsDeadvPending(prog)	(foamOptInfo(prog)->optMask & OPT_DEADV)
#define optIsPeepPending(prog)	(foamOptInfo(prog)->optMask & OPT_PEEP)
#define optIsCastPending(prog)	(foamOptInfo(prog)->optMask & OPT_CAST)

#define optSetCPropPending(prog) (foamOptInfo(prog)->optMask |= OPT_CPROP)
#define optSetJFlowPending(prog) (foamOptInfo(prog)->optMask |= OPT_JFLOW)
#define optSetCsePending(prog)	 (foamOptInfo(prog)->optMask |= OPT_CSE)
#define optSetDeadvPending(prog) (foamOptInfo(prog)->optMask |= OPT_DEADV)
#define optSetPeepPending(prog)	 (foamOptInfo(prog)->optMask |= OPT_PEEP)
#define optSetCastPending(prog)	 (foamOptInfo(prog)->optMask |= OPT_CAST)

#define optResetCPropPending(prog) (foamOptInfo(prog)->optMask &= ~OPT_CPROP)
#define optResetJFlowPending(prog) (foamOptInfo(prog)->optMask &= ~OPT_JFLOW)
#define optResetCsePending(prog)   (foamOptInfo(prog)->optMask &= ~OPT_CSE)
#define optResetDeadvPending(prog) (foamOptInfo(prog)->optMask &= ~OPT_DEADV)
#define optResetPeepPending(prog)  (foamOptInfo(prog)->optMask &= ~OPT_PEEP)
#define optResetCastPending(prog)  (foamOptInfo(prog)->optMask &= ~OPT_CAST)

#endif /* !_OPTFOAM_H_ */
