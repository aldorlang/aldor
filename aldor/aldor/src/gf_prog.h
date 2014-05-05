/*****************************************************************************
 *
 * gf_prog.h: Common declarations and macros for foam prog generation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GF_PROG_H_
#define _GF_PROG_H_

#include "axlobs.h"
#include "gf_util.h"

/*****************************************************************************
 *
 * :: Foam prog construction operations.
 *
 ****************************************************************************/

#define			gen0NLabels()		(gen0State->labelNo)

extern Foam		gen0BuildFunction	(ProgType, String, AbSyn);

extern void		gen0ProgPushState	(Stab, GenFoamTag);
extern void		gen0ProgPopState	(void);
extern GenFoamState	gen0ProgSaveState	(ProgType);
extern void		gen0ProgUseBaseState	(void);
extern void		gen0ProgUseUpperState	(void);
extern void		gen0ProgRestoreState	(GenFoamState);
extern void		gen0ProgPushFormat	(AInt);
extern Foam		gen0ProgInitEmpty	(String, AbSyn);
extern void		gen0ProgFiniEmpty	(Foam, AInt, AInt);
extern Foam		gen0ProgClosEmpty	(void);

extern void		gen0ProgAddParams	(Length, String *);
extern void		gen0ProgAddStateFormat	(AInt);

extern AbSyn		gen0ProgGetExporter	(void);
extern AbSyn		gen0ProgPushExporter	(AbSyn);
extern void		gen0ProgPopExporter	(AbSyn);
extern void		gen0ProgAddExporterArgs	(AbSyn);
extern void		gen0ProgPopExporterArgs	(void);
extern Bool		gen0ProgHasReturn	(void);

extern void		gen0AddInit		(Foam);
extern void		gen0AddStmt		(Foam, AbSyn);
extern void		gen0AddStmtNth		(Foam, AInt);
extern Foam		gen0SeqAdd		(Foam, Foam);
extern AInt		gen0AddParam		(Foam);
extern AInt		gen0AddLocal		(Foam);
extern AInt		gen0AddLex		(Foam);
extern AInt		gen0AddLexNth		(Foam, AInt, AInt);

#endif /* !_GF_PROG_H_ */
