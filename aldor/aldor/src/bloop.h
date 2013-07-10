/*****************************************************************************
 *
 * bloop.h: Interactive break loop.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _BLOOP_H_
#define _BLOOP_H_

# include "axlobs.h"

extern AbSyn	breakSetRoot	(AbSyn);
extern void     breakLoop	(Bool forHuman, int msgc, CoMsg* msgv);
extern void	breakInterrupt	(void);

extern int	bloopMsgFPrintf	(FILE *fout, Msg msg, ...);
extern int	bloopMsgVFPrintf(FILE *fout, Msg msg, va_list);

#endif /* !_BLOOP_H_ */
