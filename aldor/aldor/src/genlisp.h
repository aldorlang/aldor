/*****************************************************************************
 *
 * genlisp.h: Foam-to-Lisp translation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GENLISP_H_
#define _GENLISP_H_

# include "axlobs.h"

extern int	genLispOption(String);

extern SExpr    genLisp(Foam);

extern ULong	glWriteMode;	/* used to write .l files */

#endif /* !_GENLISP_H_ */
