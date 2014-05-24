/*****************************************************************************
 *
 * gf_imps.h: Routines dealing with retrieving lazy imports
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GF_IMPS_H_
#define _GF_IMPS_H_

#include "gf_util.h"

extern Bool	gen0IsLazyConst		(TForm tf);
extern void 	gen0IssueLazyFunctions	(void);
extern void 	gen0InitImport		(Syme);
extern Foam 	gen0GetDomainLex	(TForm);
extern void 	gen0GetLazyImport	(void);
extern Foam 	gen0GetDomImport	(Syme, Foam);
extern Foam 	gen0LazyValue		(Foam, Syme);
extern Foam	gen0GetLazyBuiltin	(String, AInt, Length, Length);

extern void 	gen0InitGVectTable	(void);
extern void 	gen0IssueGVectFns	(void);
extern void 	gen0FiniGVectTable	(void);

typedef struct _GenSaveState {
	GenFoamState state;
	FoamList     savedLines;
	FoamList     *savedPlace;
	FoamList     *wherePlace;
	Bool 	     deep;
	int 	     idx;
} GenSaveState;

extern int	gen0MoveToImportPlace		(GenSaveState *, AInt);
extern void	gen0RestoreFromImportPlace	(GenSaveState *);

void	gen0StdLazyGetsCreate			(void);

#endif
