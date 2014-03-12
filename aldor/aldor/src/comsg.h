/*****************************************************************************
 *
 * comsg.h: Compiler message reporting.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _COMSG_H_
#define _COMSG_H_

#include "axlobs.h"
#include "comsgdb.h"
#include "msg.h"

enum comsgTag {
	COMSG_REMARK,
	COMSG_WARNING,
	COMSG_ERROR,
	COMSG_FATAL,
	COMSG_NOTE
};

typedef union {
	CoMsgList noteList;
	int	  noteNumber;
} NoteInfo;

typedef Enum(comsgTag)	CoMsgTag;

struct comsg {
	CoMsgTag	tag;		/* Kind of message */
	Length		serial;		/* Serial number with a compilation. */
	SrcPos		pos;		/* Position where it originated */
	AbSyn		node;		/* .. or node where it originated */
	Msg		msg;		/* The message template. */
	String		text;		/* The text of the message */
	NoteInfo	notes;  	/* list of eventual notes */
};


/*
 * :: Functions to control behaviour.
 */
extern int	comsgSetOption	  (String);	       /* 0 => OK, -1 => err */
extern void	comsgSetInteractiveOption (void);
extern int	comsgSelectByName (String, Bool isOn); /* 0 => OK, -1 => err */

extern Bool	comsgOkAbbrev     (void); /* Is it OK to abbrev types?   */
extern Bool	comsgOkMacText    (void); /* Pt to macro text (not use)? */
extern Bool	comsgOkDetails    (void); /* Is it OK to give details?   */
extern Bool	comsgOkBreakLoop  (void); /* Is it OK to talk to break loop? */

extern Bool	comsgOkRemark	  (Msg);  /* Is this remark OK?          */
extern Bool	comsgOkWarning	  (Msg);  /* Is this warning OK?         */
extern Bool	comsgOkRelease    (void); /* Show "Release: Aldor(... " */

extern CoMsgList comsgMessages(void);     /* Read-only list */
extern CoMsgList comsgMessagesForMsg(Msg); /* Newly created list */

/*
 * :: Functions to access message database.
 */
extern void	comsgOpen	  (void);
extern void	comsgClose	  (void);
extern String	comsgString	  (Msg);		/* Shared. */

/*
 * :: Functions to collect a list of messages, then print them.
 */
extern void	comsgInit	  (void);
extern void	comsgFini	  (void);
extern int	comsgErrorCount	  (void);
			/*
			 * comsgInit initializes the list.
			 * comsgFini finalizes message structures
			 *     and prints the messages on stdout.
			 */

/*
 * Add messages.
 */
extern void	comsgFatal	  (AbSyn,  Msg fmt, ...);
extern void	comsgError	  (AbSyn,  Msg fmt, ...);
extern void	comsgWarning	  (AbSyn,  Msg fmt, ...);
extern void	comsgRemark	  (AbSyn,  Msg fmt, ...);
extern void 	comsgWarnPos	  (SrcPos, Msg fmt, ...);
/*
 * Varargs versions
 */
extern void	comsgVFatal	  (AbSyn,  Msg fmt, va_list);
extern CoMsg	comsgVError	  (AbSyn,  Msg fmt, va_list);
extern CoMsg	comsgVWarning	  (AbSyn,  Msg fmt, va_list);
extern CoMsg	comsgVRemark	  (AbSyn,  Msg fmt, va_list);
extern CoMsg	comsgVWarnPos	  (SrcPos, Msg fmt, va_list);

/*
 * Add messages with notes attached.
 */
extern void	comsgNError	  (AbSyn, Msg fmt, ...);
extern void	comsgNWarning	  (AbSyn, Msg fmt, ...);
extern void	comsgNRemark	  (AbSyn, Msg fmt, ...);

/* Add note to previous comsgNXxxx */
extern void	comsgNote	  (AbSyn, Msg fmt, ...);

/*
 * :: Functions for program-controlled display in "comsg" format.
 */
extern void	comsgReportOne	  (FILE *, CoMsg);

extern int	comsgFPrintf	  (FILE *, Msg fmt, ...);
extern int	comsgVFPrintf	  (FILE *, Msg fmt, va_list);

extern void	comsgPromptPrint  (FILE *, FILE *, String, ...) chk_fmt (3, 4);

/*
 * :: Strictly local but quite useful.
 */
extern  int	comsgPrintLine	(FILE *, SrcPos);

#endif /* !_COMSG_H_ */
