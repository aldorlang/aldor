/*****************************************************************************
 *
 * dword.h: Double-Word arithmetic
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _DWORD_H_
#define _DWORD_H_

#include "cport.h"
 
extern void	xxTestGtDouble(int *, ULong, ULong, ULong, ULong);
extern void	xxPlusStep    (ULong *, ULong *, ULong, ULong, ULong);
extern void	xxTimesStep   (ULong *, ULong *, ULong, ULong, ULong, ULong);

#ifndef OPT_NoDoubleOps
extern void	xxTimesDouble (ULong *, ULong *, ULong, ULong);
extern ULong	xxModDouble   (ULong, ULong, ULong);
extern void	xxDivideDouble(ULong *, ULong *, ULong *,
			       ULong, ULong, ULong);
#endif

#endif /*!_DWORD_H_*/
