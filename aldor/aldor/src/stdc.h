/*****************************************************************************
 *
 * stdc.h: Fixups to ANSI library for given platform.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Simulate ANSI C library compliance by adding to and modifying
 * the library for this platform.
 */

#ifndef _STDC_H_
#define _STDC_H_

# include "platform.h"
# include "cconfig.h"

#if defined(CC_HPUX) /* wants own source */
#define _INCLUDE_POSIX_SOURCE
#endif

# include "assert.h0"
# include "ctype.h0"
# include "errno.h0"
# include "float.h0"
# include "limits.h0"
# include "locale.h0"
# include "math.h0"
# include "setjmp.h0"
# include "signal.h0"
# include "stdarg.h0"
# include "stddef.h0"
# include "stdio.h0"
# include "stdlib.h0"
# include "string.h0"
# include "time.h0"

#endif /* !_STDC_H */
