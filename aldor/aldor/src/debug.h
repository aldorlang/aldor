/*****************************************************************************
 *
 * debug.h: Debugging code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "cport.h"

#if defined(NDEBUG)

# define DEBUG_MODE(flag)	Nothing
# define DEBUG			if (false)
# define DEBUG_IF(v)		if (false)
# define DEBUG_CONFIG		""
# define DEBUG_DECL(s)		s

#else
extern Bool dbFlag;

# define DEBUG_MODE(flag)	(dbFlag = (flag))
# define DEBUG			if (dbFlag)
# define DEBUG_IF(v)		if (v)
# define DEBUG_CONFIG		"(debug version)"
# define DEBUG_DECL(s)		s

#endif

extern	FILE	*dbOut;

extern	void	dbInit	(void);

#endif /* _DEBUG_H_ */
