/****************************************************************************
 *
 * terror.h: Type errors.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ***************************************************************************/

#ifndef _TERROR_H_
#define _TERROR_H_

#include "axlobs.h"
#include "comsgdb.h"

extern Bool terror (Stab stab, AbSyn absyn, TForm type);
extern void terrorNoTypes	    (Msg, AbSyn, TForm, TPoss);
extern void terrorNotUniqueType     (Msg, AbSyn, TForm, TPoss);
extern void terrorNotUniqueMeaning  (Msg, AbSyn, SymeList, SymeList, String,
				     TForm);

extern void terrorNotEnoughExports  (Stab, AbSyn, TPoss, Bool);
extern void terrorAssign	    (AbSyn, TForm, TPoss);
extern void terrorSetBang	    (Stab, AbSyn, Length, AbSynGetter);
extern void terrorTypeConstFailed   (TConst);
extern void terrorApplyFType	    (AbSyn, TForm, TPoss, AbSyn op, Stab,
				     Length argc, AbSynGetter argf);
extern void terrorIdCondition       (TForm, AbSyn, AbLogic, AbLogic);
extern void terrorApplyCondition    (AbSyn, TForm, AbSyn, AbLogic, AbLogic);
extern void terrorApplyNotAnalyzed  (AbSyn, AbSyn, TForm);
extern void terrorMeaningsOutOfScope(Stab, AbSyn, AbSyn, TForm,
				     Length argc, AbSynGetter argf);
extern void terrorNoMeaningForId(AbSyn,String);


extern Bool terrorAuditPoss	    (Bool verbose, AbSyn absyn);
extern Bool terrorAuditBottomUp	    (Bool verbose, AbSyn absyn);
extern Bool terrorAuditTopDown	    (Bool verbose, AbSyn absyn);

#define tuniInappropriateTPossVal	12L
#define tuniNoValueTPossVal		4L
#define tuniUnknownTPossVal		8L
#define tuniErrorTPossVal		14L

#define tuniInappropriateTPoss		((TPoss) tuniInappropriateTPossVal)
#define tuniNoValueTPoss		((TPoss) tuniNoValueTPossVal)
#define tuniUnknownTPoss		((TPoss) tuniUnknownTPossVal)
#define tuniErrorTPoss			((TPoss) tuniErrorTPossVal)

#define tuniIsInappropriate(tposs)	((tposs) == tuniInappropriateTPoss)
#define tuniIsNoValue(tposs)		((tposs) == tuniNoValueTPoss)
#define tuniIsUnknown(tposs)		((tposs) == tuniUnknownTPoss)
#define tuniIsError(tposs)		((tposs) == tuniErrorTPoss)

/* NOTE: don't change the number; sorting procedures in terror.c use it */
#define TR_BadFnType			1
#define TR_BadArgType			2
#define TR_ArgMissing			3
#define TR_EmbedFail			4
#endif /* !_TERROR_H_ */



