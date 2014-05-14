/*****************************************************************************
 *
 * spesym.c: Special symbols, used throughout compiler.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "spesym.h"
#include "symbol.h"

/*
 * Symbols for Foam types.
 */
Symbol	ssymArr,
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
Symbol	ssymBoolean,
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
	ssymRawRecord,
	ssymRecord,
	ssymReference,
	ssymSelf,
	ssymSelfSelf,
	ssymSingleInteger,
	ssymTest,
	ssymTextWriter,
	ssymTrailingArray,
	ssymThird,
	ssymTuple,
	ssymType,
	ssymUnion,
	ssymVariable;

/*
 * Symbols for operation names we care about.
 */
Symbol	ssymArrow,
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
	ssymTheInteger,
	ssymTheGenerator,
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
Symbol	ssymBasic,
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

Symbol	ssymFtnSInt,
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

Symbol ssymImplPAOps;

/*
 * Symbols for Foam arrays and records.
 */
Symbol	 ssymArrElt,
	 ssymArrNew,
	 ssymArrSet,
	 ssymArrDispose,
	 ssymRawRecSet,
	 ssymRawRecElt,
	 ssymRawRecNew,
	 ssymRawRecDispose,
	 ssymRecElt,
	 ssymRecNew,
	 ssymRecSet,
	 ssymRecDispose,
	 ssymTRNew,
	 ssymTRElt,
	 ssymIRElt,
	 ssymBIntDispose;


void
ssymInit(void)
{
	static Bool isInit = false;

	if (isInit) return;

	/*
	 * Symbols for Foam types.
	 */
	ssymArr 	 = symIntern("Arr");
	ssymBInt	 = symIntern("BInt");
	ssymBool	 = symIntern("Bool");
	ssymByte	 = symIntern("XByte");
	ssymChar	 = symIntern("Char");
	ssymDFlo	 = symIntern("DFlo");
	ssymHInt	 = symIntern("HInt");
	ssymNil		 = symIntern("Nil");
	ssymPtr		 = symIntern("Ptr");
	ssymSFlo	 = symIntern("SFlo");
	ssymSInt	 = symIntern("SInt");

	/*
	 * Symbols for Aldor types we care about.
	 */
	ssymBoolean      = symIntern("Boolean");
	ssymCategory     = symIntern("Category");
	ssymCross        = symIntern("Cross");
	ssymDelayed      = symIntern("Delayed");
	ssymEnum         = symIntern("Enumeration");
	ssymExit         = symIntern("Exit");
	ssymGenerator    = symIntern("Generator");
	ssymJoin         = symIntern("Join");
	ssymLiteral      = symIntern("Literal");
	ssymMap          = symIntern("Map");
	ssymMeet         = symIntern("Meet");
	ssymPackedMap    = symIntern("PackedMap");
	ssymPointer      = symIntern("Pointer");
	ssymRaw          = symIntern("Raw");
	ssymRawRecord    = symIntern("RawRecord");
	ssymRecord       = symIntern("Record");
	ssymReference    = symIntern("Ref");
	ssymSelf         = symIntern("%");
	ssymSelfSelf     = symIntern("%%");
	ssymSingleInteger= symIntern("SingleInteger");
	ssymTest         = symIntern("Test");
	ssymTextWriter   = symIntern("TextWriter");
	ssymTrailingArray= symIntern("TrailingArray");
	ssymThird        = symIntern("Third");
	ssymTuple        = symIntern("Tuple");
	ssymType         = symIntern("Type");
	ssymUnion        = symIntern("Union");
	ssymVariable     = symIntern("?");

	/*
	 * Symbols for operation names we care about.
	 */
	ssymArrow        = symIntern("->");
	ssymApply	 = symIntern("apply");
	ssymBrace	 = symIntern("brace");
	ssymBracket	 = symIntern("bracket");
	ssymCoerce	 = symIntern("coerce");
	ssymEquals	 = symIntern("=");
	ssymNotEquals	 = symIntern("~=");
	ssymPackedArrow  = symIntern("->*");
	ssymPrint	 = symIntern("<<");
	ssymSetBang	 = symIntern("set!");
	ssymTheCase	 = symIntern("case");
	ssymTheDispose   = symIntern("dispose!");
	ssymTheExplode   = symIntern("explode");
	ssymTheFloat	 = symIntern("float");
	ssymTheInteger	 = symIntern("integer");
	ssymTheGenerator = symIntern("generator");
	ssymThePrint	 = symIntern("print");
	ssymTheNew	 = symIntern("new");
	ssymTheRawRecord = symIntern("rawrecord");
	ssymTheRecord    = symIntern("record");
	ssymTheString	 = symIntern("string");
	ssymTheTest	 = symIntern("test");
	ssymTheTrailingArray = symIntern("trailing");
	ssymTheUnion     = symIntern("union");

	/*
	 * Symbols naming function interfaces.
	 */
	ssymBasic        = symIntern("Basic");
	ssymBuiltin      = symIntern("Builtin");
	ssymForeign      = symIntern("Foreign");
	ssymC	         = symIntern("C");
	ssymFortran      = symIntern("Fortran");
	ssymJava         = symIntern("Java");
	ssymLisp         = symIntern("Lisp");
	ssymMachine      = symIntern("Machine");

	/*
	 * Symbols of attributes we like to know about. This is
	 * mainly used for Fortran-specific types.
	 */
	ssymFtnSInt      = symIntern("FortranInteger");
	ssymFtnSFlo      = symIntern("FortranReal");
	ssymFtnDFlo      = symIntern("FortranDouble");
	ssymFtnSCpx      = symIntern("FortranComplexReal");
	ssymFtnDCpx      = symIntern("FortranComplexDouble");
	ssymFtnBool      = symIntern("FortranLogical");
	ssymFtnXStr      = symIntern("FortranString");
	ssymFtnFSA       = symIntern("FortranFStringArray");
	ssymFtnFStr      = symIntern("FortranFString");
	ssymFtnChar      = symIntern("FortranCharacter");
	ssymFtnArry      = symIntern("FortranArray");

	/*
	 * Symbols for implicit category stuff
	 */
	ssymImplPAOps	= symIntern("DenseStorageCategory");

	/*
	 * Symbols for Foam arrays and records.
	 */
	ssymArrNew       = symIntern("ArrNew");
	ssymArrElt       = symIntern("ArrElt");
	ssymArrSet       = symIntern("ArrSet");
	ssymArrDispose   = symIntern("ArrDispose");
	ssymRawRecNew    = symIntern("RawRecNew");
	ssymRawRecSet    = symIntern("RawRecSet");
	ssymRawRecElt    = symIntern("RawRecElt");
	ssymRecNew       = symIntern("RecNew");
	ssymRecElt       = symIntern("RecElt");
	ssymRecSet       = symIntern("RecSet");
	ssymTRNew        = symIntern("TRNew");
	ssymTRElt        = symIntern("TRElt");
	ssymIRElt        = symIntern("IRElt");
	ssymRawRecDispose= symIntern("RawRecDispose");
	ssymRecDispose   = symIntern("RecDispose");
	ssymBIntDispose  = symIntern("BIntDispose");

	isInit	         = true;
}
