/*****************************************************************************
 *
 * freevar.h: Free variable lists for type forms.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FREEVAR_H_
#define _FREEVAR_H_

#include "axlobs.h"

/******************************************************************************
 *
 * :: Free variable lists
 *
 *****************************************************************************/

struct fvar {
	SymeList	symes;
	Bool		skip;
};

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

/*
 * Structure accessor macros.
 */

#define			fvSymes(fv)		((fv)->symes)
#define			fvSkipParam(fv)		((fv)->skip)
#define			fvSetSkipParam(fv)	(fvSkipParam(fv) = true)
#define			fvCount(fv)		listLength(Syme)(fvSymes(fv))

/*
 * Basic FreeVar operations.
 */

extern FreeVar		fvFrSymes		(SymeList);
extern FreeVar		fvEmpty			(void);
extern FreeVar		fvSingleton		(Syme);
extern FreeVar		fvUnion			(FreeVar, FreeVar);
extern int		fvPrint			(FILE *, FreeVar);

extern Bool		fvHasSyme		(FreeVar, Syme);
extern Bool		fvHasAbSub		(FreeVar, AbSub);

#endif /* !_FREEVAR_H_ */
