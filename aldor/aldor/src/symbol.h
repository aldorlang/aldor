/*****************************************************************************
 *
 * symbol.h: Pooled symbols.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Symbols can be compared using "==".
 *
 * Asking for the string of a symbol does not allocate new storage
 * so DON'T modify the string returned.
 *
 * "Intern"ing a symbol does not keep a pointer to the original string
 * so the original can be freed if desired.
 */

#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "axlobs.h"
#include "ttable.h"

struct symbol {
	MostAlignedType *info;	  /* Used as desired, initialized to 0 */
	String	str;
};

# define	SYM_LOOK	0
# define	SYM_ALLOC	1	/* Alloc if not there. Else 0. */
# define	SYM_STRCOPY	2	/* Copy string. Else use original. */

# define	symIntern(str)		symProbe(str, SYM_ALLOC | SYM_STRCOPY)
# define	symInternConst(str)	symProbe(str, SYM_ALLOC)
# define	symHash(sym)		((Hash) ptrCanon(sym))
# define	symString(sym)		((sym)->str)
# define	symInfo(sym)		((sym)->info)

extern Symbol	symGen			(void);
extern Symbol	symProbe		(String, int options);
extern void	symClear		(void);
extern int	symPrint		(FILE *, Symbol);
extern void	symMap			(void (*symfun)(Symbol));

DECLARE_TSET(Symbol);

#endif /* !_SYMBOL_H_ */
