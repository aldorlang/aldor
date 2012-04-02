/*****************************************************************************
 *
 * sefo.h: Semantic forms
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SEFO_H_
#define _SEFO_H_

# include "axlobs.h"

#define			abIsSefo(ab)	(abState(ab) == AB_State_HasUnique)

/*
 * sstPrint
 */
extern int		sefoPrint		(FILE *, Sefo);
extern int		symePrint		(FILE *, Syme);
extern int		tformPrint		(FILE *, TForm);
extern int		sefoListPrint		(FILE *, SefoList);
extern int		symeListPrint		(FILE *, SymeList);
extern int		tformListPrint		(FILE *, TFormList);

extern int tformOStreamWrite(OStream ostream, TForm tf);
extern int symeOStreamWrite(OStream ostream, Syme syme);
extern int sefoOStreamWrite(OStream ostream, Sefo syme);

/*
 * sstPrintDb
 */
extern int		sefoPrintDb		(Sefo);
extern int		symePrintDb		(Syme);
extern int		symePrintDb2		(Syme);
extern int		tformPrintDb		(TForm);
extern int		sefoListPrintDb		(SefoList);
extern int		symeListPrintDb		(SymeList);
extern int		tformListPrintDb	(TFormList);

/*
 * sstEqual
 */
extern Bool		sefoEqual		(Sefo,  Sefo);
extern Bool		symeEqual		(Syme,  Syme);
extern Bool		tformEqual		(TForm, TForm);
extern Bool		sefoListEqual		(SefoList,  SefoList);
extern Bool		symeListEqual		(SymeList,  SymeList);
extern Bool		tformListEqual		(TFormList, TFormList);

extern Bool		symeIsTwin		(Syme, Syme);

/*
 * sstEqualMod
 */
extern Bool		sefoEqualMod		(SymeList, Sefo,  Sefo);
extern Bool		symeEqualMod		(SymeList, Syme,  Syme);
extern Bool		tformEqualMod		(SymeList, TForm, TForm);
extern Bool		sefoListEqualMod	(SymeList, SefoList, SefoList);
extern Bool		symeListEqualMod	(SymeList, SymeList, SymeList);
extern Bool		tformListEqualMod	(SymeList,TFormList,TFormList);

extern Bool		symeEqualModConditions	(SymeList, Syme,  Syme);

/*
 * sefoCopy
 */
extern Sefo		sefoCopy		(Sefo);

/*
 * sefoAudit
 */
extern Bool		sefoAudit		(Bool, Sefo);

/*
 * sstFreeVars
 */
extern void		sefoFreeVars		(Sefo);
extern void		symeFreeVars		(Syme);
extern void		tformFreeVars		(TForm);
extern void		abSubFreeVars		(AbSub);
extern void		sefoListFreeVars	(SefoList);
extern void		symeListFreeVars	(SymeList);
extern void		tformListFreeVars	(TFormList);

/*
 * symeListSubst
 */
extern AbSub 		absFrSymes(Stab stab, SymeList symes, Sefo sefo);
extern SymeList		symeListSubstSelf	(Stab, TForm, SymeList);
extern SymeList		symeListSubstCat	(Stab, SymeList, TForm,
						 SymeList);
extern SymeList		symeListSubstSigma	(AbSub, SymeList);
extern TForm		tformSubstSigma		(AbSub, TForm);
extern SymeList		symeListSubst		(AbSub, SymeList);

/*
 * sstSubst
 */
extern Sefo		sefoSubst		(AbSub, Sefo);
extern Syme		symeSubst		(AbSub, Syme);
extern TForm		tformSubst		(AbSub, TForm);

/*
 * symeList set operations.
 */
extern Bool		symeListMember		(Syme,SymeList,SymeEqFun);
extern Bool		symeListSubset		(SymeList,SymeList,SymeEqFun);
extern SymeList		symeListUnion		(SymeList,SymeList,SymeEqFun);
extern SymeList		symeListIntersect	(SymeList,SymeList,SymeEqFun);
extern Bool		symeCloseOverDetails	(Syme);
extern void		symeListClosure		(Lib, SymeList);

/*
 * sstToBuffer
 */
extern int		sefoToBuffer		(Lib, Buffer, Sefo);
extern int		symeToBuffer		(Lib, Buffer, Syme);
extern int		tformToBuffer		(Lib, Buffer, TForm);
extern int		tqualToBuffer		(Lib, Buffer, TQual);
extern int		sefoListToBuffer	(Lib, Buffer, SefoList);
extern int		symeListToBuffer	(Lib, Buffer, SymeList);
extern int		tformListToBuffer	(Lib, Buffer, TFormList);
extern int		tqualListToBuffer	(Lib, Buffer, TQualList);

/*
 * sstFrBuffer
 */
extern Sefo		sefoFrBuffer		(Lib, Buffer);
extern Syme		symeFrBuffer		(Lib, Buffer);
extern TForm		tformFrBuffer		(Lib, Buffer);
extern TQual		tqualFrBuffer		(Lib, Buffer);
extern SefoList		sefoListFrBuffer	(Lib, Buffer);
extern SymeList		symeListFrBuffer	(Lib, Buffer);
extern TFormList	tformListFrBuffer	(Lib, Buffer);
extern TQualList	tqualListFrBuffer	(Lib, Buffer);

/*
 * tformType[cp]FrBuffer
 */
extern int		tformTypecFrBuffer	(Buffer);
extern void		tformTypepFrBuffer	(Buffer, int, int *);
extern void		symeListFrBuffer0	(Buffer);

/*
 * Debugging
 */
extern int sstGetMaxDepth(void);

#endif /* !_SEFO_H_ */
