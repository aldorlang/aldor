/*****************************************************************************
 *
 * tconst.h: Type form constraints.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TCONST_H_
#define _TCONST_H_

#include "axlobs.h"

/******************************************************************************
 *
 * :: Type Form Constraint kinds
 *
 *****************************************************************************/

enum tconstTag {
    TC_START,
	TC_Satisfies,
    TC_LIMIT
};

typedef Enum(tconstTag)		TConstTag;

/******************************************************************************
 *
 * :: Type Form Constraint structure
 *
 *****************************************************************************/

struct tconst {
	BPack(TConstTag)	tag;		/* What kind of constraint. */
	AbSyn			pos;		/* Where to report errors. */
	AbSyn			id;		/* const checked on behalf of id */
	AbLogic                 known;          /* Conditional context at point */
	TConst			parent;		/* Traceback parent. */
	Length			serial;		/* Serial number. */
	TForm			owner;		/* TForm which checks it. */
	AbSyn			ab0;		/* Random piece of absyn. */
	Length			argc;		/* Number of arguments. */
	TForm			*argv;		/* Additional arguments. */
};

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

/*
 * Macros for generic treatment of structures.
 */
#define			tcTag(tc)		((tc)->tag)
#define			tcPos(tc)		((tc)->pos)
#define			tcParent(tc)		((tc)->parent)
#define			tcSerial(tc)		((tc)->serial)
#define			tcOwner(tc)		((tc)->owner)
#define			tcArgc(tc)		((tc)->argc)
#define			tcArgv(tc)		((tc)->argv)
#define			tcKnown(tc)		((tc)->known)

#define			tcSetParent(tc, p)	(tcParent(tc) = (p))

/*
 * Basic operations.
 */
extern void		tcInit			(void);
extern void		tcFini			(void);
extern void		tcSatPush		(TForm, TForm);
extern void		tcSatPop		(void);

extern TConst		tcAlloc			(TConstTag, TForm, AbLogic, AbSyn, Length,
						 va_list);
extern void		tcFree			(TConst);
extern void		tcPush			(TConst);
extern void		tcPop			(TConst);
extern Bool		tcEq			(TConst, TConst);
extern Bool		tcEqual			(TConst, TConst);
extern int		tcPrint			(FILE *, TConst);

extern void		tcNew			(TConstTag, TForm, AbLogic, AbSyn, AbSyn,
						 Length, ...);
extern void		tcNewSat		(TForm, AbLogic, TForm, TForm, AbSyn);
extern void		tcNewSat1		(TForm, AbLogic, AbSyn, TForm, TForm, AbSyn);
extern void		tcMove			(TForm, TForm);

extern void		tfCheckConsts		(TForm);
extern void		tcCheck			(TConst);

#endif /* !_TCONST_H_ */
