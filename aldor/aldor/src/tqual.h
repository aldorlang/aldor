/*****************************************************************************
 *
 * tqual.h: Qualified type forms.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TQUAL_H_
#define _TQUAL_H_

#include "axlobs.h"

/******************************************************************************
 *
 * :: Different kinds of qualified base type 
 *
 *****************************************************************************/

enum tqualStatus {
	TQUAL_STATUS_START,
		TQUAL_Normal = TQUAL_STATUS_START,	/* Normal tqual */
		TQUAL_Foreign,				/* Foreign import */
		TQUAL_Builtin,				/* Builtin import */
	TQUAL_STATUS_LIMIT
};

typedef Enum(tqualStatus) TQualStatus;

/******************************************************************************
 *
 * :: Qualified type form structure
 *
 *****************************************************************************/

struct tqual {
	BPack(Bool)	isQual;			/* Is this type unqualified? */
	TQualStatus	status;			/* Normal/Foreign/Builtin */
	TForm		base;			/* Base type form. */
	TFormList	qual;			/* Qualifying category. */
};

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

/*
 * Macros for generic treatment of structures.
 */
#define			tqIsQualified(tq)	((tq)->isQual == true)
#define			tqIsUnqualified(tq)	((tq)->isQual == false)
#define			tqBase(tq)		((tq)->base)
#define			tqQual(tq)		((tq)->qual)
#define			tqStatus(tq)		((tq)->status)
#define			tqIsNormal(tq)		(tqStatus(tq) == TQUAL_Normal)
#define			tqIsForeign(tq)		(tqStatus(tq) == TQUAL_Foreign)
#define			tqIsBuiltin(tq)		(tqStatus(tq) == TQUAL_Builtin)

/*
 * Basic operations.
 */
extern TQual		tqNewFrList		(TForm, TFormList);
extern TQual		tqNewQualified		(TForm, TForm);
extern TQual		tqNewUnqualified	(TForm);
extern TQual		tqSetUnqualified	(TQual);
extern TQual		tqAddQual		(TQual, TForm);
extern void		tqFree			(TQual);
extern int		tqPrint			(FILE *, TQual);
extern int		tqPrintDb		(TQual);
extern TQualStatus	tqSetStatus		(TQual, TQualStatus);

extern SymeList		tqGetQualImports	(TQual);
extern SymeList		tqGetForeignImports	(Stab, TQual);
extern SymeList		tqGetBuiltinImports	(Stab, TQual);
extern Bool		tqHasImport		(TQual, Syme);
extern TQualList	tqListFrArgs		(Stab, TForm *, Length);

extern Bool tqIsWildcard(TQual tq);
extern TForm tqWildcardImporter(TQual tq);

#endif /* !_TQUAL_H_ */
