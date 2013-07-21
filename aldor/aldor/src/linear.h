/*****************************************************************************
 *
 * linear.h: Bracketing of piles.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Convert a two dimensional list of lines of tokens to a flat list.
 * The layout information is encodes as SETTAB, BACKSET and BACKTAB tokens.
 *
 * The lists of tokens in the original lines get modified.
 */

#ifndef _LINEAR_H_
#define _LINEAR_H_

#include "axlobs.h"

extern TokenList        linearize       (TokenList tl);

extern Token		linDoPileStart	(SrcPos);
extern Token		linDoPileEnd	(SrcPos);
extern TokenList	linDoPileWrap	(TokenList);


extern 	Bool		linIsPileOn	(void);
extern 	void		linPilePush	(Bool);
extern 	void		linPilePop	(void);
extern 	void		linResetPileStack(void);


#endif /* !_LINEAR_H_ */
