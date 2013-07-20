/*****************************************************************************
 *
 * genc.h: Foam-to-C translation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GENC_H_
#define _GENC_H_

#include "axlobs.h"
#include "ccode.h"

extern CCodeList	genC			(Foam, String);
extern CCode		genAXLmainC		(String);

extern void		genCSetSMax		(int);
extern void		genCSetIdLen		(int);
extern void		genCSetIdHash		(int);


/* Tracking Fortran functional parameter passing */
typedef struct
{
	String	base;	/* Name of function of which this is a parameter */
	AInt	num;	/* Functional parameter postion */
	CCode	fun;	/* Name of the parameter (wrapper function) */
	CCode	clos;	/* Name of the closure for Aldor called via wrapper */
	CCode	class;	/* Storage class of parameter and closure */
} *FtnFunParam;


/* Accessers for FtnFunParam */
#define gc0FtnFunBase(p)	((p)->base)
#define gc0FtnFunNumber(p)	((p)->num)
#define gc0FtnFunName(p)	((p)->fun)
#define gc0FtnFunClosure(p)	((p)->clos)
#define gc0FtnFunClass(p)	((p)->class)

#endif /* !_GENC_H_ */
