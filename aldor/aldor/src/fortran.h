/*****************************************************************************
 *
 * fortran.h: Common declarations and macros for Fortran support.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FORTRAN_H_
#define _FORTRAN_H_

enum fortranType
{
	FTN_Machine = 1,
	FTN_FSComplex,
	FTN_FDComplex,
	FTN_XLString,
	FTN_String,
	FTN_StringArray,
	FTN_Character,
	FTN_FnParam,
	FTN_SingleInteger,
	FTN_Boolean,
	FTN_FSingle,
	FTN_FDouble,
	FTN_Array,
	FTN_Word
};

typedef Enum(fortranType) FortranType;

#define MODIFIABLEARG "[Modifiable]"

#include "axlobs.h"
#include "tinfer.h"
#include "tform.h"
#include "gf_util.h"
#include "genfoam.h"

extern TForm tfFtnBool; /* Boolean/LOGICAL */
extern TForm tfFtnChar; /* Character/CHARACTER or CHARACTER*1 */
extern TForm tfFtnXStr; /* String/CHARACTER(*) */
extern TForm tfFtnFStr; /* String/CHARACTER*N for some N */
extern TForm tfFtnFSA;  /* Array FixedString N for some N */
extern TForm tfFtnArry; /* FSingleArray/REAL(*) etc. */
extern TForm tfFtnSInt; /* SingleInteger/INTEGER */
extern TForm tfFtnSFlo; /* SingleFloat/REAL */
extern TForm tfFtnDFlo; /* DoubleFloat/DOUBLE PRECISION */
extern TForm tfFtnSCpx; /* FSComplex/COMPLEX */
extern TForm tfFtnDCpx; /* FDComplex/COMPLEX(KIND=KIND(0.D0)) */

extern FortranType	ftnTypeFrCatTForm(AbSyn, TForm);
extern FortranType	ftnTypeFrDomTForm(TForm);
extern FortranType	ftnTypeFrString(String);
extern String		ftnNameFrType(FortranType);
extern FoamTag		gen0FtnMachineType(FortranType);

extern SymeList		ftnComplexRealExports(void);
extern SymeList		ftnComplexDoubleExports(void);
extern SymeList		ftnArrayExports(void);
extern SymeList		ftnFSArrayExports(void);
#endif /* !_FORTRAN_H_ */
