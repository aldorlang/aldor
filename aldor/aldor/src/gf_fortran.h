/*****************************************************************************
 *
 * gf_fortran.h: Foam code generation for the Aldor/Fortran interface.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GF_FORTRAN_H_
#define _GF_FORTRAN_H_

#include "axlobs.h"

/* Aldor-calls-Fortran */
extern Foam	gen0ModifyFortranCall	(Syme, Foam, AbSyn, Bool);
extern Foam	gen0MakePointerTo	(FoamTag, Foam, FoamList *);
extern Foam	gen0ReadPointerTo	(FoamTag, Foam);
extern Foam	gen0WritePointerTo	(FoamTag, Foam, Foam);


/* Fortran-calls-Aldor */
extern void	gen0ExportToFortran	(AbSyn fun);
extern Foam	gen0FortranExportFn	(TForm, FoamTag, Foam, String, AbSyn);

#endif /* !_GF_FORTRAN_H_ */
