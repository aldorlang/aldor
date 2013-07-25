/*****************************************************************************
 *
 * macex.h: Macro expansion of parse trees.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _MACEX_H_
#define _MACEX_H_

#include "axlobs.h"

extern void 	macexInitFile	(void);
extern void 	macexFiniFile	(void);

extern  AbSyn   macroExpand     (AbSyn);

extern  void	macexUseGlobalMacros	(Bool);
extern  AbSyn	macexGetMacros();
extern  void    macexAddMacro(AbSyn, Bool);
#endif /* !_MACEX_H_ */
