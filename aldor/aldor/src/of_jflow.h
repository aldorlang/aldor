/*****************************************************************************
 *
 * of_jflow.h: Test and jump optimization.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OF_JFLOW_H_
#define _OF_JFLOW_H_

#include "axlobs.h"

extern void     	jflowUnit    	(Foam, int);
extern void     	jflowProg    	(Foam);
extern void		jflowSetNegate	(Bool);
extern Bool		jflowCanNegate	(void);

#endif /* !_OF_JFLOW_H_ */
