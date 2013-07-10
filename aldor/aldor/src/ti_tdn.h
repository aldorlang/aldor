/*****************************************************************************
 *
 * ti_tdn.h: Type inference -- top down pass.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TI_TDN_H_
#define _TI_TDN_H_

#include "axlobs.h"

extern void		tiTopDown		(Stab, AbSyn, TForm);
extern Bool		titdn			(Stab, AbSyn, TForm);

#endif /* !_TI_TDN_H_ */
