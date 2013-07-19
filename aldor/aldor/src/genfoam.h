/*****************************************************************************
 *
 * genfoam.h: Foam code generation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GENFOAM_H_
#define _GENFOAM_H_

#include "axlobs.h"

extern Foam	generateFoam	(Stab, AbSyn, String);

extern Bool	genIsConst	(Syme);
extern Bool	genIsLocalConst	(Syme);
extern Bool	genIsVar	(Foam);

extern void	genSetConstNum		(Syme, int, UShort, Bool);
extern Bool	genHasConstNum		(Syme);
extern UShort	genGetConstNum		(Syme);
extern void	genGetConstNums		(SymeList);
extern SymeList	genGetSymeInlined	(Syme);
extern void	genKillOldSymeConstNums (int);

extern void	genSetAxiomAx		(Bool);
extern void	genSetDebugWanted	(Bool);
extern void	genSetDebuggerWanted	(Bool);
extern void	genSetSmallHashCodes	(Bool);

extern Foam	gen0ApplyReturn		(AbSyn, Syme, TForm, Foam);
extern Foam	gen1ApplyReturn		(AbSyn, Syme, TForm, Foam, Foam *);
extern Foam	gen0CCallFrFoam		(FoamTag, Foam, Length, Foam **);

extern Bool	gen0IsFortranCall	(AbSyn);

extern AInt	gen0CSigFormatNumber(TForm);
extern AInt	gen0FortranSigExportNumber(TForm);

extern AInt	gen0CatchFormatNumber	(TForm, TForm);
extern AInt	gen0VoidCatchFormatNumber(TForm);


/* COND-DEF */
struct GF_COND
{
	Syme	syme;
	AbLogic	condition;
};

typedef struct GF_COND _GfCond;
typedef _GfCond *GfCond;

#endif /* !_GENFOAM_H_ */
