/*****************************************************************************
 *
 * gf_add.h: Foam code generation for "add", "with", and default packages.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GF_ADD_H_
#define _GF_ADD_H_

#include "axlobs.h"

extern Foam	genAdd			(AbSyn);
extern Foam	genWith			(AbSyn);
extern Foam	genHas			(AbSyn);

#if EDIT_1_0_n1_06
extern Foam	gen0AddBody0		(AbSyn, Stab, AbSyn);
#else
extern Foam	gen0AddBody0		(AbSyn, AbSyn, Stab, AbSyn);
#endif
extern Foam	gen0MakeDefaultPackage  (AbSyn, Stab, Bool, Syme);
extern Bool	gen0HasDefaults	  	(AbSyn);
extern Foam	gen0GetDomain		(TForm, int);
extern Bool	gen0IsSpecialType	(AbSyn);
extern Foam	gen0ApplySpecialType	(AbSyn);
extern void  	gen0TypeAddExportSlot	(Syme);
extern void	gen0SetUsage		(Foam, AIntList, AIntList);
extern void 	gen0SymeSetInit		(Syme, Foam);
extern Foam	gen0SymeInit		(Syme);
extern void	gen0SymeSetCond		(Syme, Foam);
extern Foam	gen0SymeCond		(Syme);
extern void	gen0AddTypeVar		(Syme);

extern void	gen0InitExport		(Syme);

extern int	gen0StrHash		(String s);
extern Foam 	gen0TypeHash		(TForm, TForm, String);
extern Foam	gen0SefoHashExporter	(Sefo);
extern Foam	gen0SefoHash		(Sefo, Sefo);
extern Foam	gen0RtSetProgHash	(Foam, AInt);

extern Foam	gen0LocalSelf		(void);
extern Syme	gen0LocalSelfSyme	(void);

extern void	genFoamDefSeq		(AbSyn);

extern void	gen0FindUncondSymes	(AbSyn, SymeList);
extern Foam	gen0ExpMapRef		(Syme);

extern Sefo	gen0EqualMods		(Sefo);

extern void     gen0StringsInit 	(void);
extern void	gen0StringsFini		(void);

#endif /* !_GF_ADD_H_ */
