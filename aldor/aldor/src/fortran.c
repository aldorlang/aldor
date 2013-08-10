/*****************************************************************************
 *
 * fortran.c: Fortran interactions and utilities.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file contains routines for helping to bridge the gap between
 * Aldor and Fortran.
 */

#include "debug.h"
#include "format.h"
#include "fortran.h"
#include "spesym.h"
#include "tfsat.h"
#include "abpretty.h"
#include "strops.h"

Bool	fortranTypesDebug	= false;
#define fortranTypesDEBUG	DEBUG_IF(fortranTypes)	afprintf


/* This is a local macro just to make the if tests more readable */
#define XtfIsValid(tf)	(tf && !tfIsUnknown(tf))


/* Some interesting TForms which we want to keep a lookout for */
TForm tfFtnBool = (TForm)NULL; /* Boolean/LOGICAL */
TForm tfFtnChar = (TForm)NULL; /* Character/CHARACTER or CHARACTER*1 */
TForm tfFtnXStr = (TForm)NULL; /* String/CHARACTER(*) */
TForm tfFtnFStr = (TForm)NULL; /* String/CHARACTER*N for some N */
TForm tfFtnFSA  = (TForm)NULL; /* Array FixedString N for some N */
TForm tfFtnArry = (TForm)NULL; /* FSingleArray/REAL(*) etc. */
TForm tfFtnSInt = (TForm)NULL; /* SingleInteger/INTEGER */
TForm tfFtnSFlo = (TForm)NULL; /* SingleFloat/REAL */
TForm tfFtnDFlo = (TForm)NULL; /* DoubleFloat/DOUBLE PRECISION */
TForm tfFtnSCpx = (TForm)NULL; /* FSComplex/COMPLEX */
TForm tfFtnDCpx = (TForm)NULL; /* FDComplex/COMPLEX(KIND=KIND(0.D0)) */


/*
 * We lazily initialise the TForms for Fortran category
 * attributes. We ought to put them in tform.c to allow
 * them to be picked up by tfFrSymbol() properly by other
 * clients. That would make ftnTypeFrString() seem a bit
 * cleaner but there isn't much point.
 */
local void
ftnTypeInitTForms()
{
	if (!tfFtnSInt) tfFtnSInt = tfFrSymbol(ssymFtnSInt);
	if (!tfFtnSFlo) tfFtnSFlo = tfFrSymbol(ssymFtnSFlo);
	if (!tfFtnDFlo) tfFtnDFlo = tfFrSymbol(ssymFtnDFlo);
	if (!tfFtnSCpx) tfFtnSCpx = tfFrSymbol(ssymFtnSCpx);
	if (!tfFtnDCpx) tfFtnDCpx = tfFrSymbol(ssymFtnDCpx);
	if (!tfFtnBool) tfFtnBool = tfFrSymbol(ssymFtnBool);
	if (!tfFtnChar) tfFtnChar = tfFrSymbol(ssymFtnChar);
	if (!tfFtnXStr) tfFtnXStr = tfFrSymbol(ssymFtnXStr);
	if (!tfFtnFStr) tfFtnFStr = tfFrSymbol(ssymFtnFStr);
	if (!tfFtnFSA)  tfFtnFSA  = tfFrSymbol(ssymFtnFSA);
	if (!tfFtnArry) tfFtnArry = tfFrSymbol(ssymFtnArry);
}


/*
 * Given a domain, check to see if it has any of the Fortran
 * attributes such as FortranLogical. If it does then we return
 * the corresponding Fortran type, if not we return 0.
 * Fortran type indicates both input type and output type.
 *
 * Essentially this routine evaluates `dom has FortranLogical'
 * etc for each of the Fortran* categories.
 */
FortranType
ftnTypeFrDomTForm(TForm tf)
{
	Sefo	sefo;
	TForm	tfcat;


	/* We hope that non-word-sized values don't need converting */
	if (gen0Type(tf, NULL) != FOAM_Word) 
		return 0;


	/* Walk past the declaration to get the type */
	if (tfIsDeclare(tf))
		tf = tfDeclareType(tf);


	/* We use used to only consider leaf nodes ... */
	sefo  = tfGetExpr(tf);
	tfcat = abGetCategory((AbSyn)sefo);

	if (tfcat)
		return ftnTypeFrCatTForm(sefo, tfcat);
	else
		return 0;
}


/*
 * Given a category, check to see if it satisfies any of the
 * Fortran attribute categories such as FortranLogical and
 * return 0 if not. The sefo is only used for debugging.
 */
FortranType
ftnTypeFrCatTForm(AbSyn sefo, TForm tf)
{
	/*
	 * Make sure that we have the category symbols that we
	 * are interested in. Initialise them if we don't.
	 */
	ftnTypeInitTForms();


	/* Some debugging information. */
	if (DEBUG(fortranTypes)) {
		(void)fprintf(dbOut, "- ftnTypeFrCatTForm: %s\n", abPretty(sefo));

		if (XtfIsValid(tfFtnBool) && tfSatisfies(tf,tfFtnBool))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnBool));

		if (XtfIsValid(tfFtnChar) && tfSatisfies(tf,tfFtnChar))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnChar));

		if (XtfIsValid(tfFtnXStr) && tfSatisfies(tf,tfFtnXStr))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnXStr));

		if (XtfIsValid(tfFtnFSA) && tfSatisfies(tf,tfFtnFSA))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnFSA));

		if (XtfIsValid(tfFtnFStr) && tfSatisfies(tf,tfFtnFStr))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnFStr));

		if (XtfIsValid(tfFtnSInt) && tfSatisfies(tf,tfFtnSInt))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnSInt));

		if (XtfIsValid(tfFtnSFlo) && tfSatisfies(tf,tfFtnSFlo))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnSFlo));

		if (XtfIsValid(tfFtnDFlo) && tfSatisfies(tf,tfFtnDFlo))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnDFlo));

		if (XtfIsValid(tfFtnSCpx) && tfSatisfies(tf,tfFtnSCpx))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnSCpx));

		if (XtfIsValid(tfFtnDCpx) && tfSatisfies(tf,tfFtnDCpx))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnDCpx));

		if (XtfIsValid(tfFtnArry) && tfSatisfies(tf,tfFtnArry))
			(void)fprintf(dbOut, "     has %s\n", symString(ssymFtnArry));

		fnewline(dbOut);
	}


	/*
	 * Use type satisfaction to check category attributes. We
	 * need to be slightly cautious as some of these categories
	 * might not been defined (eg. the library doesn't use them).
	 *
	 * IMPORTANT: the test for FortranFStringArray MUST occur before
	 * the test for FortranArray. This is because anything that
	 * has FortranFStringArray will also have FortranArray.
	 *
	 * IMPORTANT: consider using tfSatBit(tfSatHasMask(), tfcat, ...)
	 * as this does not commit the has question.
	 */
	if (XtfIsValid(tfFtnBool) && tfSatisfies(tf, tfFtnBool))
		return FTN_Boolean;
	if (XtfIsValid(tfFtnChar) && tfSatisfies(tf, tfFtnChar))
		return FTN_Character;
	if (XtfIsValid(tfFtnSInt) && tfSatisfies(tf, tfFtnSInt))
		return FTN_SingleInteger;
	if (XtfIsValid(tfFtnSFlo) && tfSatisfies(tf, tfFtnSFlo))
		return FTN_FSingle;
	if (XtfIsValid(tfFtnDFlo) && tfSatisfies(tf, tfFtnDFlo))
		return FTN_FDouble;
	if (XtfIsValid(tfFtnSCpx) && tfSatisfies(tf, tfFtnSCpx))
		return FTN_FSComplex;
	if (XtfIsValid(tfFtnDCpx) && tfSatisfies(tf, tfFtnDCpx))
		return FTN_FDComplex;
	if (XtfIsValid(tfFtnXStr) && tfSatisfies(tf, tfFtnXStr))
		return FTN_XLString;
	if (XtfIsValid(tfFtnFStr) && tfSatisfies(tf, tfFtnFStr))
		return FTN_String;
	if (XtfIsValid(tfFtnFSA) && tfSatisfies(tf, tfFtnFSA))
		return FTN_StringArray;
	if (XtfIsValid(tfFtnArry) && tfSatisfies(tf, tfFtnArry))
		return FTN_Array;


	/* Failed to match */
	return 0;
}


/*
 * Convert from FortranType into text strings. Used to insert
 * FortranType values into FOAM declarations etc.
 */
String
ftnNameFrType(FortranType ftype)
{
	String	s;

	switch (ftype)
	{
		case FTN_Boolean	: s = symString(ssymFtnBool);break;
		case FTN_Character	: s = symString(ssymFtnChar);break;
		case FTN_SingleInteger	: s = symString(ssymFtnSInt);break;
		case FTN_FSingle	: s = symString(ssymFtnSFlo);break;
		case FTN_FDouble	: s = symString(ssymFtnDFlo);break;
		case FTN_FSComplex	: s = symString(ssymFtnSCpx);break;
		case FTN_FDComplex	: s = symString(ssymFtnDCpx);break;
		case FTN_XLString	: s = symString(ssymFtnXStr);break;
		case FTN_String		: s = symString(ssymFtnFStr);break;
		case FTN_StringArray	: s = symString(ssymFtnFSA);break;
		case FTN_Array		: s = symString(ssymFtnArry);break;
		default			: s = "";break;
	}

	return strCopy(s);
}


/*
 * Convert from text strings into FortranType. Used to extract
 * FortranType values from FOAM declarations etc.
 */
FortranType
ftnTypeFrString(String s)
{
	if (!strcmp(symString(ssymFtnChar), s))
		return FTN_Character;
	if (!strcmp(symString(ssymFtnBool), s))
		return FTN_Boolean;
	if (!strcmp(symString(ssymFtnSInt), s))
		return FTN_SingleInteger;
	if (!strcmp(symString(ssymFtnSFlo), s))
		return FTN_FSingle;
	if (!strcmp(symString(ssymFtnDFlo), s))
		return FTN_FDouble;
	if (!strcmp(symString(ssymFtnSCpx), s))
		return FTN_FSComplex;
	if (!strcmp(symString(ssymFtnDCpx), s))
		return FTN_FDComplex;
	if (!strcmp(symString(ssymFtnXStr), s))
		return FTN_XLString;
	if (!strcmp(symString(ssymFtnFSA), s))
		return FTN_StringArray;
	if (!strcmp(symString(ssymFtnFStr), s))
		return FTN_String;
	if (!strcmp(symString(ssymFtnArry), s))
		return FTN_Array;
	return 0;
}


/*
 * Convert an Aldor type into the machine type
 * which will be passed to/from Fortran. For
 * example, SingleInteger is passed as SInt.
 */
FoamTag
gen0FtnMachineType(FortranType ftntype)
{
	switch (ftntype)
	{
		/* FTN_Boolean -> FOAM_SInt really ... */
		case FTN_Character	: return FOAM_Char;
		case FTN_Boolean	: return FOAM_Bool;
		case FTN_SingleInteger	: return FOAM_SInt;
		case FTN_FSingle	: return FOAM_SFlo;
		case FTN_FDouble	: return FOAM_DFlo;
		case FTN_FSComplex	: return FOAM_Word;
		case FTN_FDComplex	: return FOAM_Word;
		case FTN_XLString	: return FOAM_Word;
		case FTN_String		: return FOAM_Word;
		case FTN_StringArray	: return FOAM_Word;
		case FTN_Array		: return FOAM_Word;
		default			: return FOAM_Word;
	}
}


/*
 * Return a list of all the exports of FortranComplexReal.
 */
SymeList
ftnComplexRealExports(void)
{
	/*
	 * Make sure that we have the category symbols that we
	 * are interested in. Initialise them if we don't.
	 */
	ftnTypeInitTForms();


	/* Return the exports of this category */
	return tfGetCatExports(tfFtnSCpx);
}


/*
 * Return a list of all the exports of FortranComplexDouble.
 */
SymeList
ftnComplexDoubleExports(void)
{
	/*
	 * Make sure that we have the category symbols that we
	 * are interested in. Initialise them if we don't.
	 */
	ftnTypeInitTForms();


	/* Return the exports of this category */
	return tfGetCatExports(tfFtnDCpx);
}


/*
 * Return a list of all the exports of FortranArray.
 */
SymeList
ftnArrayExports(void)
{
	/*
	 * Make sure that we have the category symbols that we
	 * are interested in. Initialise them if we don't.
	 */
	ftnTypeInitTForms();


	/* Return the exports of this category */
	return tfGetCatExports(tfFtnArry);
}


/*
 * Return a list of all the exports of FortranFStringArray.
 */
SymeList
ftnFSArrayExports(void)
{
	/*
	 * Make sure that we have the category symbols that we
	 * are interested in. Initialise them if we don't.
	 */
	ftnTypeInitTForms();


	/* Return the exports of this category */
	return tfGetCatExports(tfFtnFSA);
}


