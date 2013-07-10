/*****************************************************************************
 *
 * ti_sef.h: Type inference -- sefo pass.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TI_SEF_H_
#define _TI_SEF_H_

#include "axlobs.h"

extern void		tiSefo			(Stab, Sefo);
extern void		tisef			(Stab, Sefo);
extern Bool		tiCanSefo		(Sefo);
extern Stab		stabFindLevel		(Stab, Syme);

#endif /* !_TI_SEF_H_ */
