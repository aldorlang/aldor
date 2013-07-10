/*****************************************************************************
 *
 * ablogic.h: Structures for inference about conditional exports.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _ABLOGIC_H_
#define _ABLOGIC_H_

#include "axlobs.h"

/*
 * The abLogic structure is used to keep track of conditions known or required
 * in type inference for conditional exports.
 */

struct abLogic {
	double	fake;
};

extern AbLogic	abCondKnown;	  /* Conditions with known value (tinfer) */
extern AbLogic	gfCondKnown;	  /* Ditto (genfoam) */

extern void	ablogInit	  (void);
extern void	ablogFini	  (void);

extern AbLogic	ablogFrSefo	  (Sefo);

extern AbLogic	ablogCopy	  (AbLogic);
extern void	ablogFree	  (AbLogic);
extern int	ablogPrint	  (FILE *, AbLogic);
extern int	ablogPrintDb	  (AbLogic);

extern AbLogic	ablogTrue	  (void);
extern AbLogic	ablogFalse	  (void);
extern AbLogic	ablogNot	  (AbLogic);
extern AbLogic	ablogAnd	  (AbLogic, AbLogic);
extern AbLogic	ablogOr		  (AbLogic, AbLogic);

extern Bool	ablogIsTrue	  (AbLogic);
extern Bool	ablogIsFalse	  (AbLogic);

extern Bool	ablogEqual	  (AbLogic, AbLogic);
extern Bool	ablogIsImplied	  (AbLogic, Sefo cond);
extern Bool	ablogImplies	  (AbLogic, AbLogic);
extern Bool	ablogIsListImplied(AbLogic, SefoList);
extern Bool	ablogIsListKnown  (SefoList sefolist);
 
extern void	ablogAndPush	  (AbLogic* glo,AbLogic* save,Sefo,Bool sense);
extern void	ablogAndPop	  (AbLogic* glo,AbLogic* save);

extern int	bputAblog         (Buffer, AbLogic);
#endif /* !_ABLOGIC_H_ */
