/*****************************************************************************
 *
 * tfsat.h: Type form satisfaction.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TFSAT_H_
#define _TFSAT_H_

#include "axlobs.h"
#include "utype.h"

/******************************************************************************
 *
 * :: Type form satisfaction flags.
 *
 *****************************************************************************/

typedef	ULong		SatMask;

extern SatMask		tfSatHasMask		(void);
extern SatMask		tfSatBupMask		(void);
extern SatMask		tfSatTdnMask		(void);
extern SatMask		tfSatTdnInfoMask	(void);
extern SatMask		tfSatSefMask		(void);
extern SatMask		tfSatTErrorMask		(void);

extern Bool		tfSatSucceed		(SatMask);
extern Bool		tfSatPending		(SatMask);
extern TForm		tfSatGetPendingFail	(void);

extern Bool		tfSatFailedExportsMissing(SatMask);
extern Bool		tfSatFailedEmbedFail	 (SatMask);
extern Bool		tfSatFailedArgMissing	 (SatMask);
extern Bool		tfSatFailedBadArgType	 (SatMask);
extern Bool		tfSatFailedDifferentArity(SatMask);

extern Length		tfSatParN		(SatMask);
extern Length		tfSatArgN		(AbSyn, Length, AbSynGetter,
						 Length, TForm);

extern AbEmbed		tfSatAbEmbed		(SatMask);
extern AbEmbed		tfSatEmbedType		(TForm, TForm);
extern TForm 		tfsEmbedResult		(TForm, AbEmbed);
/******************************************************************************
 *
 * :: tfSatisfies
 *
 *****************************************************************************/

/*
 * Return true if any object of type S is valid in any context
 * which requires an object of type T.
 */
extern Bool		tfSatisfies	(TForm S, TForm T);

/*
 * Return true if any object of type S is valid in a value context which
 * requires an object of type T.  The embedding from S -> () is not used.
 */
extern Bool		tfSatValues	(TForm S, TForm T);

/*
 * Return true if any object of type S is valid in a return context which
 * requires an object of type T.  The embedding from S -> () is used if needed.
 */
extern Bool		tfSatReturn	(TForm S, TForm T);
 
/******************************************************************************
 *
 * :: Type orders
 *
 *****************************************************************************/

/*
 * Return true if S is a subtype of Domain.
 * Symbols whose type is S represent Domains.
 */
extern Bool		tfSatDom	(TForm S);

/*
 * Return true if S is a subtype of Category.
 * Symbols whose type is S represent Categories.
 */
extern Bool		tfSatCat	(TForm S);

/*
 * Return true if S is a subtype of Type.
 * Symbols whose type is S represent Types.
 */
extern Bool		tfSatType	(TForm S);

/******************************************************************************
 *
 * :: tfSatMap
 *
 *****************************************************************************/

extern SatMask		tfSatMap	(SatMask, Stab, TForm, TForm,
					 AbSyn, Length, AbSynGetter);
extern Bool		tfSatBit	(SatMask, TForm S, TForm T);
extern SatMask		tfSat		(SatMask, TForm S, TForm T);
extern SatMask          tfSat1          (SatMask mask, AbSyn Sab, TForm S, TForm T);


extern AbSub		tfSatSubList	(AbSyn);

/******************************************************************************
 *
 * :: tfSatMulti
 *
 *****************************************************************************/

extern SatMask		tfSatMapArgs	(SatMask, AbSub, TForm,
					 AbSyn, Length, AbSynGetter);
extern SatMask 		tfSatAsMulti 	(SatMask, AbSub, TForm, TForm,
					 AbSyn, Length, AbSynGetter);


/******************************************************************************
 *
 * :: utfSat
 *
 *****************************************************************************/
typedef struct usatmask {
	SatMask mask;
	UTypeResult result;
} *USatMask;

USatMask utfSatArg(SatMask mask, AbSyn ab, UTForm T);

USatMask utfSatMapArgs(SatMask mask, AbSub sigma, UTForm S,
		      AbSyn ab, Length arg, AbSynGetter argf);

USatMask utfSatAsMulti(SatMask mask, AbSub sigma, UTForm S, UTForm TScope,
		      AbSyn ab, Length argc, AbSynGetter argf);
USatMask utfSat1(SatMask mask, AbSyn Sab, UTForm S, UTForm T);
USatMask utfSat(SatMask mask, UTForm S, UTForm T);
Bool	 utfSatSucceed(USatMask);
Bool	 utfSatPending(USatMask);
void	 utfSatMaskFree(USatMask);


#endif /* !_TFSAT_H_ */
