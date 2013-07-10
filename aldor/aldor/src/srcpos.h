/*****************************************************************************
 *
 * srcpos.h: Source position operations
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SRCPOS_H_
#define _SRCPOS_H_

# include "axlport.h"
# include "fname.h"
# include "buffer.h"

typedef ULong			SrcPos;
typedef struct sposCell	*	SrcPosCell;
typedef union sposStack		SrcPosStack;

/*
 * :: SrcPos
 */
extern SrcPos	sposNone;
extern SrcPos	sposTop		(void);
extern SrcPos	sposEnd		(void);

extern void	sposTableToBuffer(Buffer);
extern void	sposTableFrBuffer(Buffer);

extern void	sposShow	(void);
extern void	sposInit	(void);
extern void	sposFini	(void);
extern SrcPos	sposNew(FileName fn, Length flno, Length glno, Length cno);
extern SrcPos	sposGet		(Length glno, Length cno);
extern Length	sposGlobalLine	(SrcPos);
extern FileName sposFile	(SrcPos);
extern Length	sposGLine	(FileName, Length);
extern Length	sposLine	(SrcPos);
extern Length	sposChar	(SrcPos);

extern SrcPos	sposOffset	(SrcPos, int);
extern Bool	sposEqual	(SrcPos, SrcPos);
extern SrcPos	sposMin		(SrcPos, SrcPos);
extern SrcPos	sposMax		(SrcPos, SrcPos);

extern int	sposCmp		(SrcPos, SrcPos);
extern Bool	sposIsSpecial	(SrcPos);
#define 	sposIsNone(pos)	sposEqual(pos, sposNone)

extern int	sposLineText	(Buffer, SrcPos);
extern int	sposPrint	(FILE *, SrcPos);

extern Bool	sposIsSynthetic     (SrcPos);
extern SrcPos	sposSynthetic       (SrcPos);

extern Bool	sposIsMacroExpanded (SrcPos);
extern SrcPos	sposMacroExpanded   (SrcPos);

extern void     sposGrowGloLineTbl  (FileName fname, Length flno, Length glno);

/*
 * :: SrcPosStack
 */

union sposStack {
	SrcPos		spos;
	SrcPosCell	stack;
};

struct sposCell {
	SrcPos		spos;
	SrcPosStack	rest;
};

extern SrcPosStack	spstackEmpty;

extern SrcPosStack	spstackPush		(SrcPos, SrcPosStack);
extern SrcPosStack	spstackCopy		(SrcPosStack);
extern void		spstackFree		(SrcPosStack);

extern SrcPos		spstackFirst		(SrcPosStack);
extern SrcPosStack	spstackRest		(SrcPosStack);

extern SrcPosStack	spstackSetFirst		(SrcPosStack, SrcPos);
extern SrcPosStack	spstackSetSecond	(SrcPosStack, SrcPos);

extern void		spstackPrintDb		(SrcPosStack);

#endif /* !_SRCPOS_H_ */
