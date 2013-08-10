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
# define DEBUG(cond)		false
# define DEBUG_CONFIG		""

#else

# define DEBUG_MODE(flag)	(phaseDebug = (flag))
# define DEBUG(cond)		cond##Debug
# define DEBUG_CONFIG		"(debug version)"

#endif

/* Generic debug flag, enabled per-phase with -WTd+phase. */
extern Bool phaseDebug;

#define phaseDEBUG		if (DEBUG(phase))
#define DEBUG_DECL(s)		s

extern	FILE	*dbOut;

extern	void	dbInit	(void);

#endif /* _DEBUG_H_ */
