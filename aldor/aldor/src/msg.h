/*****************************************************************************
 *
 * msg.h: Message catalog manipulation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _MSG_H_
#define _MSG_H_

# include "axlport.h"
# include "fname.h"

typedef int	Msg;
typedef int	MsgSet;

struct msgInfo {
	int	setno;
	int	msgno;
	char	*name;
	char	*text;
};

# define	MSG_NOT_FOUND	(-1)

extern void	msgDefaults	(struct msgInfo *);

extern void	msgOpen		(FileName);
extern void	msgClose	(void);

extern String   msgGet		(MsgSet, Msg);
extern void	msgClear	(void);		  /* Reclaim messages. */

extern int	msgFPrintf	(FILE *, MsgSet, Msg, ...);
extern int	msgVFPrintf	(FILE *, MsgSet, Msg, va_list);

/*
 * These do not deal with message text so drive themselves from msgDefaults.
 */
extern Msg	msgByName	(MsgSet, String); /* May be MSG_NOT_FOUND. */
extern Msg	msgByAName	(MsgSet, String); /* Case-insensitive. */
extern String	msgName		(MsgSet, Msg);
extern int	msgNumber	(MsgSet, Msg);
extern int	msgMax     	(MsgSet);

#endif /* !_MSG_H_ */
