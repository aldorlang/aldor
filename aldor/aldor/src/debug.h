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

#   define DEBUG_MODE(flag)             Nothing
#   define DEBUG(e)                     Nothing
#   define DEBUG_IF(v,e)                Nothing
#   define DEBUG_CONFIG			""
#   define DEBUG_DECL(s)		Nothing

#else
    extern Bool dbFlag;

#   define DEBUG_MODE(flag)             (dbFlag = (flag))
#   define DEBUG(e)                     Statement(if (dbFlag) {e;})
#   define DEBUG_IF(v,e)                Statement(if (v) {e;})
#   define DEBUG_CONFIG			"(debug version)"
#   define DEBUG_DECL(s)		s

#endif

extern	FILE	*dbOut;

extern	void	dbInit	(void);

#endif /* _DEBUG_H_ */
