/*****************************************************************************
 *
 * debug.h: Debugging code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "format.h"

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

/* For explicitly marking declarations as "required for debug" code. */
#define DEBUG_DECL(s)		s

/* Make sure that xyzDEBUG behaves like a function call. */
#define DEBUG_IF(cond)		if (!DEBUG(cond)) { } else

#define phaseDEBUG		DEBUG_IF(phase) afprintf

extern	FILE	*dbOut;

extern	void	dbInit	(void);

#endif /* _DEBUG_H_ */
