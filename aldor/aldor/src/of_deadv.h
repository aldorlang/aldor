/*****************************************************************************
 *
 * of_deadv.h: Dead variable elimination.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OF_DEADV_H_
#define _OF_DEADV_H_

# include "axlobs.h"

extern void		dvInit			(void);
extern void     	dvElim		    	(Foam);

#endif /* !_OF_DEADV_H_ */
