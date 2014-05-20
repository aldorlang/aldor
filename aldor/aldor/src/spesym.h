/*****************************************************************************
 *
 * spesym.h: Special symbols, used throughout compiler.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SPESYM_H_
#define _SPESYM_H_

#include "symbol.h"

extern void	ssymInit(void);

/*
 * Symbols for Foam types.
 */

extern Symbol	ssymArr,
		ssymBInt,
		ssymBool,
		ssymByte,
		ssymChar,
		ssymDFlo,
		ssymHInt,
		ssymNil,
		ssymPtr,
		ssymSFlo,
		ssymSInt;

/*
 * Symbols for Aldor type constructors.
 */

extern Symbol	ssymBoolean,
		ssymCategory,
		ssymCross,
		ssymDelayed,
		ssymEnum,
		ssymExit,
		ssymGenerator,
		ssymJoin,
		ssymLiteral,
		ssymMap,
		ssymMeet,
		ssymPackedMap,
		ssymPointer,
		ssymRaw,
		ssymReference,
		ssymRawRecord,
		ssymRecord,
		ssymSelf,
		ssymSelfSelf,
		ssymSingleInteger,
		ssymTest,
		ssymTextWriter,
		ssymThird,
		ssymTrailingArray,
		ssymTuple,
		ssymType,
		ssymUnion,
		ssymVariable;

/*
 * Symbols for operation names we care about.
 */

extern Symbol	ssymArrow,
		ssymApply,
		ssymBrace,
		ssymBracket,
		ssymCoerce,
		ssymEquals,
		ssymNotEquals,
		ssymPackedArrow,
		ssymPrint,
		ssymSetBang,
		ssymTheCase,
		ssymTheDispose,
		ssymTheExplode,
		ssymTheFloat,
		ssymTheGenerator,
		ssymTheInteger,
		ssymTheJava,
		ssymTheJavaDecoder,
		ssymThePrint,
	        ssymTheNew,
		ssymTheRawRecord,
		ssymTheRecord,
		ssymTheString,
		ssymTheTest,
		ssymTheTrailingArray,
		ssymTheUnion;


/*
 * Symbols naming function interfaces.
 */

extern Symbol	ssymBasic,
		ssymBuiltin,
		ssymForeign,
		ssymC,
		ssymFortran,
		ssymJava,
		ssymLisp,
		ssymMachine;


/*
 * Symbols of attributes we like to know about. This is
 * mainly used for Fortran-specific types.
 */

extern Symbol	ssymFtnSInt,
		ssymFtnSFlo,
		ssymFtnDFlo,
		ssymFtnSCpx,
		ssymFtnDCpx,
		ssymFtnBool,
		ssymFtnXStr,
		ssymFtnFStr,
		ssymFtnFSA,
		ssymFtnChar,
		ssymFtnArry;

/*
 * Symbols for implicit category stuff
 */

extern Symbol ssymImplPAOps;

/*
 * Symbols for Foam arrays and records.
 */

extern Symbol	ssymArrElt,
		ssymArrNew,
		ssymArrSet,
		ssymArrDispose,
		ssymRawRecElt,
		ssymRawRecSet,
		ssymRawRecNew,
		ssymRawRecDispose,
		ssymRecElt,
		ssymRecNew,
		ssymRecSet,
		ssymRecDispose,
		ssymBIntDispose;


#endif /* _SPESYM_H_ */
