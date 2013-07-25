/*****************************************************************************
 *
 * scobind.h: Deduce the scopes of identifiers.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SCOBIND_H_
#define _SCOBIND_H_

#include "axlobs.h"

extern void	scopeBind 	(Stab, AbSyn);

extern void	scobindInitFile		(void);
extern void	scobindFiniFile		(void);
extern void	scoSetUndoState 	(void);
extern int	scobindMaxDef		(void);
#endif /* !_SCOBIND_H_ */
