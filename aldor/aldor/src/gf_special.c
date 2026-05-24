#include "abpretty.h"
#include "comsgdb.h"
#include "comsg.h"
#include "debug.h"
#include "gf_embed.h"
#include "gf_match0.h"
#include "gf_prog.h"
#include "gf_rtime.h"
#include "gf_special.h"
#include "gf_util.h"
#include "sefo.h"
#include "spesym.h"
#include "stab.h"
#include "syme.h"
#include "tform.h"
#include "util.h"

/*****************************************************************************
 *
 * :: Generate code for application of special operations.
 *
 ****************************************************************************/

local TForm	gen0SpecialArgType	(Syme, Length);
local TForm	gen0SpecialRetType	(Syme);
local AInt	gen0RawRecordIndex	(TForm, TForm);
local AInt	gen0RecordIndex		(TForm, TForm);
local AInt	gen0UnionIndex		(TForm, TForm);
local AInt	gen0TrailingIndex	(TForm, TForm);
local AInt	gen0UnionCaseIndex	(TForm, AbSyn);

local Foam	gen0SpecialUnhandled	(Syme);

local Foam	gen0ArrayNew		(Syme,        Length, AbSyn *, Foam *);
local Foam	gen0ArrayElt		(FoamTag,     Length, AbSyn *, Foam *);
local Foam	gen0ArraySet		(FoamTag,     Length, AbSyn *, Foam *);
local Foam	gen0ArrayDispose	(Length, AbSyn *, Foam *);

local Foam	gen0RawRecordNew	(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RawRecordExplode	(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RawRecordElt	(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0RawRecordSet	(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen1RawRecordSet	(TForm, Foam, Foam, Foam, AInt);
local Foam	gen0RawRecordDispose	(Length, AbSyn *, Foam *);
local Foam	gen1RawRecordFormat	(TForm);

local Foam	gen0RecordNew		(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RecordExplode	(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RecordElt		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0RecordSet		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0RecordDispose	(Length, AbSyn *, Foam *);

local Foam	gen0BIntDispose		(Length, AbSyn *, Foam *);

local Foam	gen0UnionNew		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0UnionCase		(TForm,       Length, AbSyn *, Foam *);
local Foam      gen0UnionCaseBool       (TForm, Length, AbSyn *, Foam *);
local Foam	gen0UnionElt		(FoamTag,TForm,Length,AbSyn *, Foam *);
local Foam	gen0UnionSet		(FoamTag,TForm,Length,AbSyn *, Foam *);
local Foam	gen0UnionDispose	(Length, AbSyn *, Foam *);

local Foam	gen0EnumEqual		(AbSyn *, Foam *);
local Foam	gen0EnumNotEqual	(AbSyn *, Foam *);

local Foam	gen0PPartialSuccess	(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0PPartialFailed	(Syme, TForm, Length, AbSyn *, Foam *);

local Foam	gen0TrailingNew		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0TrailingDispose	(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0TrailingElt		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0TrailingSet	(Syme, TForm, Length, AbSyn *, Foam *);

local Foam	   genFoamArg		  (AbSyn *, Foam *, int);

Foam
gen0SpecialOp(FoamTag type, Syme syme, Length argc, AbSyn *argv, Foam *vals)
{
	Symbol		sym = symeId(syme);
	Foam		foam = NULL;

	/* Arrays. */

	if (sym == ssymArrNew)
		foam = gen0ArrayNew(syme, argc, argv, vals);

	else if (sym == ssymArrElt)
		foam = gen0ArrayElt(type, argc, argv, vals);

	else if (sym == ssymArrSet)
		foam = gen0ArraySet(type, argc, argv, vals);

	else if (sym == ssymArrDispose)
		foam = gen0ArrayDispose(argc, argv, vals);

	/* Big Integers. */

	else if (sym == ssymBIntDispose)
		foam = gen0BIntDispose(argc, argv, vals);

	/* Raw Records. */
	else if (sym == ssymRawRecNew || sym == ssymTheRawRecord) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordNew(key, argc, argv, vals);
	}

	else if (sym == ssymRawRecElt) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordElt(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRawRecSet) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordSet(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRawRecDispose) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordDispose(argc, argv, vals);
	}

	/* Records. */
	else if (sym == ssymRecNew || sym == ssymTheRecord) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsRecord(key))
			foam = gen0RecordNew(key, argc, argv, vals);
	}

	else if (sym == ssymRecElt) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordElt(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRecSet) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordSet(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRecDispose) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordDispose(argc, argv, vals);
	}

	/* Unions. */

	else if (sym == ssymTheUnion) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsUnion(key))
			foam = gen0UnionNew(syme, key, argc, argv, vals);
	}

	else if (sym == ssymTheCase) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsUnion(key))
			foam = gen0UnionCase(key, argc, argv, vals);
	}

	/* TrailingArrays. */
	else if (sym == ssymTheTrailingArray) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsTrailingArray(key))
			foam = gen0TrailingNew(syme, key, argc, argv, vals);
	}
	/* Enumerations. */

	else if (sym == ssymEquals) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsEnum(key))
			foam = gen0EnumEqual(argv, vals);
	}

	else if (sym == ssymNotEquals) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsEnum(key))
			foam = gen0EnumNotEqual(argv, vals);
	}

	/* Generators. */

	else if (sym == ssymTheGenerator)
		foam = genFoamArg(argv, vals, int0);

	/* bracket, explode, apply, set!, dispose!. */

	else if (sym == ssymBracket) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsRecord(key))
			foam = gen0RecordNew(key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordNew(key, argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionNew(syme, key, argc, argv, vals);
		if (tfIsTrailingArray(key))
			foam = gen0TrailingNew(syme, key, argc, argv, vals);
	}

	else if (sym == ssymTheExplode) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordExplode(key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordExplode(key, argc, argv, vals);
	}

	else if (sym == ssymApply) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordElt(syme, key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordElt(syme, key, argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionElt(type, key, argc, argv, vals);
		else if (tfIsTrailingArray(key))
			foam = gen0TrailingElt(syme, key, argc, argv, vals);
	}

	else if (sym == ssymSetBang) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordSet(syme, key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordSet(syme, key, argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionSet(type, key, argc, argv, vals);
		else if (tfIsTrailingArray(key))
			foam = gen0TrailingSet(syme, key, argc, argv, vals);
	}

	else if (sym == ssymTheDispose) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordDispose(argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordDispose(argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionDispose(argc, argv, vals);
		else if (tfIsTrailingArray(key))
			foam = gen0TrailingDispose(syme, key, argc, argv, vals);
	}
	/* PPartials. */
	else if (sym == ssymTheSuccess) {
		TForm	key = gen0SpecialRetType(syme);
		foam = gen0PPartialSuccess(syme, key, argc, argv, vals);
	}
	else if (sym == ssymTheFailed) {
		TForm	key = gen0SpecialRetType(syme);
		foam = gen0PPartialFailed(syme, key, argc, argv, vals);
	}

	if (foam == NULL)
		foam = gen0SpecialUnhandled(syme);

	return gen0SetValue(foam, argv[0]);
}


/*****************************************************************************
 *
 * :: Specific generators for special operations.
 *
 ****************************************************************************/

/* Helper functions. */

local Foam
genFoamArg(AbSyn *argv, Foam *vals, int index)
{
	if (vals && vals[index])
		return vals[index];

	return genFoamVal(argv[index]);
}

TForm
gen0SpecialKeyType(TForm tf)
{
	SymeList	xsymes;
	TFormTag	tag = TF_Unknown;
	TFormList	args = listNil(TForm);
	TForm		ntf;
	Length		i, argc;

	tfFollow(tf);
	if (tfIsRecord(tf) || tfIsRawRecord(tf) || tfIsUnion(tf)
	    || tfIsEnum(tf) || tfIsTrailingArray(tf)
	    || tfIsPPartial(tf)
	    )
		return tf;

	for (xsymes = tfGetDomExports(tf); xsymes; xsymes = cdr(xsymes)) {
		Syme		xsyme = car(xsymes);
		Symbol		sym = symeId(xsyme);
		TFormTag	ntag;
		TForm		arg = NULL;

		if (!symeIsSpecial(xsyme)) continue;

		if (sym == ssymTheRecord)
			ntag = TF_Record;
		else if (sym == ssymTheRawRecord)
			ntag = TF_RawRecord;
		else if (sym == ssymTheUnion)
			ntag = TF_Union;
		else if (sym == ssymTheTrailingArray)
			ntag = TF_TrailingArray;
		else if (tfIsSelf(symeType(xsyme)))
			ntag = TF_Enumerate;
		else
			ntag = TF_Unknown;
			
		if (ntag == TF_Unknown)
			;
		else if (tag == TF_Unknown)
			tag = ntag;
		else if (tag != ntag)
			/* Seen two of record/union/enum!? */
			return tfUnknown;

		/* We assume the special symes stay in order. */
		if (tag == TF_Record && sym == ssymApply)
			arg = tfMapRet(symeType(xsyme));
		if (tag == TF_RawRecord && sym == ssymApply)
			arg = tfMapRet(symeType(xsyme));
		if (tag == TF_Union && sym == ssymTheUnion)
			arg = tfMapArgN(symeType(xsyme), int0);
		if (tag == TF_Enumerate && tfIsSelf(symeType(xsyme)))
			arg = tfDeclare(abFrSyme(xsyme), tfType);

		if (arg) args = listCons(TForm)(arg, args);
	}

	args = listNReverse(TForm)(args);
	argc = listLength(TForm)(args);

	ntf = tfNewEmpty(tag, argc);
	for (i = 0; i < argc; i += 1, args = cdr(args))
		tfArgv(ntf)[i] = car(args);

	return ntf;
}

local TForm
gen0SpecialArgType(Syme syme, Length argc)
{
	TForm	tf = tfDefineeType(symeType(syme));
	TForm	key = tfMapMultiArgN(tf, argc, int0);
	return gen0SpecialKeyType(key);
}

local TForm
gen0SpecialRetType(Syme syme)
{
	TForm	tf = tfDefineeType(symeType(syme));
	TForm	key = tfMapRet(tf);
	return gen0SpecialKeyType(key);
}

local AInt
gen0RawRecordIndex(TForm whole, TForm part)
{
	Symbol	sym = tfEnumId(part, int0);
	AInt	i;

	for (i = 0; i < tfRawRecordArgc(whole); i += 1)
		if (sym == tfDefineeSymbol(tfRawRecordArgN(whole, i)))
			return i;

	bug("gen0RawRecordIndex: accessor not in record");
	return -1;
}

local AInt
gen0RecordIndex(TForm whole, TForm part)
{
	Symbol	sym = tfEnumId(part, int0);
	AInt	i;

	for (i = 0; i < tfRecordArgc(whole); i += 1)
		if (sym == tfDefineeSymbol(tfRecordArgN(whole, i)))
			return i;

	bug("gen0RecordIndex: accessor not in record");
	return -1;
}

local AInt
gen0UnionIndex(TForm whole, TForm part)
{
	AInt	i;
	Symbol  sym = tfDefineeSymbol(part);
	for (i = 0; i < tfUnionArgc(whole); i += 1)
		if ((!sym || tfCompoundId(whole,i) == sym)
		    && tfEqual(part, tfUnionArgN(whole, i)))
			return i;

	bug("gen0UnionIndex: branch not in union");
	return -1;
}

local AInt
gen0UnionCaseIndex(TForm whole, AbSyn part)
{
	Symbol	sym = part->abId.sym;
	AInt	i;

	for (i = 0; i < tfUnionArgc(whole); i += 1)
		if (sym == tfDefineeSymbol(tfUnionArgN(whole, i)))
			return i;

	bug("gen0UnionCaseIndex: branch not in union");
	return -1;
}

AInt
gen0EnumIndex(TForm whole, Symbol sym)
{
	AInt	i;

	for (i = 0; i < tfEnumArgc(whole); i += 1)
		if (sym == tfEnumId(whole, i))
			return i;

	bug("gen0EnumIndex: case not in enumeration");
	return -1;
}

local AInt
gen0TrailingIndex(TForm whole, TForm tf)
{
	AInt	i;
	int 	argc = tfAsMultiArgc(whole);
	Symbol  sym = tfEnumId(tf, int0);
	for (i = 0; i < argc; i += 1)
		if (sym == tfDefineeSymbol(tfAsMultiArgN(whole, argc, i)))
			return i;

	bug("gen0TrailingIndex: label not in record");
	return -1;
}

Bool
gen0IsEnumLit(Syme syme)
{
	/* SpecialKeyType may be costly, so avoid if poss */
	return	symeIsImport(syme) && 
		symeIsSpecial(syme) &&
		!tfIsMap(symeType(syme)) &&
		tfIsEnum(gen0SpecialKeyType(symeType(syme)));
}

local Foam
gen0SpecialUnhandled(Syme syme)
{
	fprintf(stderr, 
	       "Implementation restriction: unhandled special symbol meaning");
	fnewline(stderr);
	fprintf(stderr, "  %s : ", symeString(syme));
	tfPrettyPrint(stderr, symeType(syme));
	fnewline(stderr);

	bugUnimpl(symeString(syme));
	return NULL;
}

/* Arrays. */

local Foam 
gen0ArrayNew(Syme syme, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf = tfDefineeType(symeType(syme));
	TForm	tfi = tfMapMultiArgN(tf, argc, int0);
	FoamTag	type = gen0Type(tfi, NULL);
	Foam	value = genFoamArg(argv, vals, 1);

	return foamNewANew(type, value);
}

local Foam
gen0ArrayElt(FoamTag type, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	whole = genFoamArg(argv, vals, int0);
	Foam	index = genFoamArg(argv, vals, 1);

	return foamNewAElt(type, index, whole);
}

local Foam
gen0ArraySet(FoamTag type, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	whole = genFoamArg(argv, vals, int0);
	Foam	index = genFoamArg(argv, vals, 1);
	Foam	value = genFoamArg(argv, vals, 2);

	return foamNewSet(foamNewAElt(type, index, whole), value);
}

local Foam
gen0ArrayDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}


/* Big Integers. */

local Foam
gen0BIntDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}


/* Raw Records. */

/*
 * PackedRepSize: () -> SInt;
 */
local Bool
tfIsPackedRepSize(TForm tf)
{
	/* Follow substitutions */
	tfFollow(tf);


	/*
	 * Type checking - must be nullary map returning a
	 * SInt$Machine. We probably ought to be using tfSat():
	 * since we don't we don't check the return type.
	 */
	if (!tfIsAnyMap(tf)) return false;
	if (tfMapArgc(tf) != 0) return false;


	/* Looks like the correct type */
	return true;
}


/*
 * PackedRecordGet: Ptr -> %
 */
local Bool
tfIsPackedRecordGet(TForm tf)
{
	/* Follow substitutions */
	tfFollow(tf);


	/*
	 * Type checking - must be unary map to self. We probably
	 * ought to be using tfSat() but since we aren't we ignore
	 * the argument type.
	 */
	if (!tfIsAnyMap(tf)) return false;
	if (tfMapArgc(tf) != 1) return false;
	if (!tfIsSelf(tfMapRet(tf))) return false;


	/* Looks like the correct type */
	return true;
}


/*
 * PackedRecordSet: (Ptr, %) -> %
 */
local Bool
tfIsPackedRecordSet(TForm tf)
{
	/* Follow substitutions */
	tfFollow(tf);


	/*
	 * Type checking - must be a binary map from a Ptr
	 * and self to self. We probably ought to be using
	 * tfSat() but since we aren't we ignore the type
	 * of the first argument.
	 */
	if (!tfIsAnyMap(tf)) return false;
	if (tfMapArgc(tf) != 2) return false;
	if (!tfIsSelf(tfMapArgN(tf, 1))) return false;
	if (!tfIsSelf(tfMapRet(tf))) return false;


	/* Looks like the correct type */
	return true;
}


local Foam
gen0RawRecordNew(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	i;
	Foam	format, whole, fmt, foam;


	/* Create the raw record format/index */
	format = gen1RawRecordFormat(key);


	/* Create a temporary for the index */
	fmt = gen0Temp(FOAM_Word);


	/* Store the index in a temporary */
	gen0AddStmt(foamNewSet(foamCopy(fmt), format), (AbSyn)NULL);


	/* Create a local for the record pointer */
	whole = gen0Temp(FOAM_RRec);


	/* Create the uninitialised raw record */
	foam = foamNewRRNew(foamCopy(fmt), argc);
	foam = foamNewSet(foamCopy(whole), foam);
	gen0AddStmt(foam, (AbSyn)NULL);


	/* Fill in each of the fields */
	for (i = 0; i < argc; i++)
	{
		Foam	rrec  = foamCopy(whole);
		Foam	rfmt  = foamCopy(fmt);
		Foam	value = genFoamArg(argv, vals, i);
		Foam	set   = gen1RawRecordSet(key, rrec, rfmt, value, i);
		gen0AddStmt(set, NULL);
	}

	return foamCopy(whole);
}


local Foam
gen1RawRecordFormat(TForm key)
{
	AInt		i, fmtc;
	FoamList	fmts;


	/* Create the dynamic format */
	fmtc = tfArgc(key);
	fmts = listNil(Foam);


	/* Build the format list in reverse order */
	for (i = (fmtc - 1); i >= 0 ; i--)
	{
		Syme	op;
		Foam	call, *ignored;
		TForm	tf = tfArgv(key)[i];


		/* Get the actual type */
		tf = tfDefineeType(tf);


		/* Look for PackedRepSize: () -> SInt */
		op = tfGetDomImport(tf, "PackedRepSize", tfIsPackedRepSize);


		/* Did we find it? */
		if (!op)
		{
			AbSyn	ab = abNewNothing(sposNone);
			String	msg = strPrintf("%s: %s is missing export %s",
					"gen1RawRecordFormat",
					abPretty(tfToAbSyn(tf)),
					"PackedRepSize");
			comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
			String	dom = abPretty(tfToAbSyn(tf));
			bug("gen1RawRecordFormat: %s is missing export %s",
				dom, "PackedRepSize");
			strFree(dom);
#endif
		}


		/* Apply this export to get the type */
		call = gen0ExtendSyme(op);
		call = gen0CCallFrFoam(FOAM_SInt, call, (Length)0, &ignored);


		/* Prepend onto the list of formats */
		listPush(Foam, call, fmts);
	}


	/* The dynamic format must be a multi */
	return foamNewRRFmt(foamNewOfList(FOAM_Values, fmts));
}


local Foam
gen0RawRecordElt(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt		field;
	TForm		tf, elt;
	Foam		rrec, call, format, *fp;
	Syme		op;
	SymeList	symes;


	/* Create the raw record format */
	format = gen1RawRecordFormat(key);


	/* Get the type of the map application */
	tf = tfDefineeType(symeType(syme));


	/* Extract the type of the argument */
	elt = tfMapMultiArgN(tf, argc, 1);


	/* Find the field for this type */
	field = gen0RawRecordIndex(key, elt);


	/* Get the raw record value being dereferenced */
	rrec  = genFoamArg(argv, vals, int0);


	/* Get the field type from the key */
	tf = tfDefineeType(tfArgv(key)[field]);


	/* Look for PackedRecordGet: Ptr -> % */
	op = tfGetDomImport(tf, "PackedRecordGet", tfIsPackedRecordGet);


	/* Did we find the export? */
	if (!op)
	{
		AbSyn	ab = abNewNothing(sposNone);
		String	msg = strPrintf("%s: %s is missing export %s",
				"gen0RawRecordElt",
				abPretty(tfToAbSyn(tf)),
				"PackedRecordGet");
		comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
		String	dom = abPretty(tfToAbSyn(tf));

		bug("gen0RawRecordElt: %s is missing export %s",
			dom, "PackedRecordGet");
		strFree(dom);
#endif
	}


	/* Convert the export into an import */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Apply this export to get the domain value */
	call = gen0ExtendSyme(op);
	call = gen0CCallFrFoam(FOAM_Word, call, 1, &fp);
	*fp  = foamNewRRElt(rrec, field, format);


	/* Return the call */
	return call;
}


local Foam
gen0RawRecordSet(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt		field;
	TForm		tf, elt;
	Foam		rrec, value, format;


	/* Create the raw record format */
	format = gen1RawRecordFormat(key);


	/* Get the type of the map application */
	tf = tfDefineeType(symeType(syme));


	/* Extract the type of the argument */
	elt = tfMapMultiArgN(tf, argc, 1);


	/* Find the field for this type */
	field = gen0RawRecordIndex(key, elt);


	/* Get the raw record value being dereferenced */
	rrec  = genFoamArg(argv, vals, int0);


	/* Get the value being stored */
	value = genFoamArg(argv, vals, 2);


	/* Generate the update */
	return gen1RawRecordSet(key, rrec, format, value, field);
}


local Foam
gen1RawRecordSet(TForm key, Foam rrec, Foam fmt, Foam value, AInt field)
{
	Foam		call, *fp;
	TForm		tf;
	Syme		op;
	SymeList	symes;


	/* Get the field type from the key */
	tf = tfDefineeType(tfArgv(key)[field]);


	/* Look for PackedRecordSet: (Ptr, %) -> % */
	op = tfGetDomImport(tf, "PackedRecordSet", tfIsPackedRecordSet);


	/* Did we find the export? */
	if (!op)
	{
		AbSyn	ab = abNewNothing(sposNone);
		String	msg = strPrintf("%s: %s is missing export %s",
				"gen1RawRecordSet",
				abPretty(tfToAbSyn(tf)),
				"PackedRecordSet");
		comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
		String	dom = abPretty(tfToAbSyn(tf));

		bug("gen1RawRecordSet: %s is missing export %s",
			dom, "PackedRecordSet");
		strFree(dom);
#endif
	}


	/* Convert the export into an import */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Apply this export to get the domain value */
	call = gen0ExtendSyme(op);
	call = gen0CCallFrFoam(FOAM_Word, call, 2, &fp);


	/* Fill in the argument slots */
	fp[0] = foamNewRRElt(rrec, field, fmt);
	fp[1] = value;


	/* Return the call to do the update */
	return call;
}


local Foam
gen0RawRecordExplode(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	tvals  = gen0TempFrDDecl(int0, true);

	/*
	 * Not implemented yet!
	 */
	(void)fprintf(dbOut, "*** gen0RawRecordExplode unimplemented\n");
	return tvals;
}


local Foam
gen0RawRecordDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}


/* Records. */

local Foam
gen0RecordNew(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0RecordFormatNumber(key);
	AInt	index;
	Foam	whole = gen0Temp0(FOAM_Rec, format);

	gen0AddStmt(gen0RNew(whole, format), NULL);
	for (index = 0; index < argc; index += 1) {
		TForm tf = tfRecordArgN(key, index);
		Foam value = genFoamArg(argv, vals, index);
		FoamTag type = gen0Type(tf, NULL);
		if (type != FOAM_Word)
			value = foamNewCast(type, value);
		gen0AddStmt(gen0RSet(whole, format, index, value), NULL);
	}

	return foamCopy(whole);
}

local Foam
gen0RecordElt(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf  = tfDefineeType(symeType(syme));
	TForm	elt = tfMapMultiArgN(tf, argc, 1);

	AInt	format = gen0RecordFormatNumber(key);
	AInt	index  = gen0RecordIndex(key, elt);
	Foam	whole  = foamNewCast(FOAM_Rec, genFoamArg(argv, vals, int0));

	return foamNewRElt(format, whole, index);
}

local Foam
gen0RecordSet(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf  = tfDefineeType(symeType(syme));
	TForm	elt = tfMapMultiArgN(tf, argc, 1);

	AInt	format = gen0RecordFormatNumber(key);
	AInt	index  = gen0RecordIndex(key, elt);
	Foam	whole  = genFoamArg(argv, vals, int0);
	Foam	value  = genFoamArg(argv, vals, 2);

	return foamNewSet(foamNewRElt(format, whole, index), value);
}

local Foam
gen0RecordExplode(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0RecordFormatNumber(key);
	Foam	whole  = gen0TempLocal0(FOAM_Rec, format);
	Foam	tvals  = gen0TempFrDDecl(format, true);
	int	i;

	gen0AddStmt(foamNewSet(whole, foamNewCast(FOAM_Rec, genFoamArg(argv, vals, int0))), NULL);

	whole = gen0MakeMultiEvaluable(FOAM_Rec, format, whole);

	for (i = 0; i < foamArgc(tvals); i++) {
		Foam lhs = foamCopy(tvals->foamValues.argv[i]);
		Foam rhs = foamNewRElt(format, foamCopy(whole), (AInt) i); 
		gen0AddStmt(foamNewSet(lhs, rhs), NULL);
	}

	return tvals;
}


local Foam
gen0RecordDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}

/* Unions. */

local Foam
gen0UnionNew(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf  = tfDefineeType(symeType(syme));
	TForm	elt = tfMapMultiArgN(tf, argc, int0);

	AInt	format = gen0MakeUnionFormat();
	Foam	index  = foamNewSInt(gen0UnionIndex(key, elt));
	Foam	value  = genFoamArg(argv, vals, int0);
	Foam	whole  = gen0Temp0(FOAM_Rec, format);

	if (gen0Type(elt, NULL) != FOAM_Word)
		value = foamNewCast(FOAM_Word, value);

	gen0AddStmt(gen0RNew(whole, format), NULL);
	gen0AddStmt(gen0RSet(whole, format, (AInt) 0, index), NULL);
	gen0AddStmt(gen0RSet(whole, format, (AInt) 1, value), NULL);

	return foamNewCast(FOAM_Word, foamCopy(whole));
}

local Foam
gen0UnionElt(FoamTag type, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	l = gen0State->labelNo++;
	AInt	format = gen0MakeUnionFormat();
	Foam	whole = genFoamArg(argv, vals, int0);
	Foam	stmt, foam;
	Foam 	myVals[2];
	
	whole = gen0MakeMultiEvaluable(FOAM_Rec, format, foamNewCast(FOAM_Rec, whole));
	myVals[0] = foamCopy(whole);
	myVals[1] = NULL;

	stmt = foamNewIf(gen0UnionCaseBool(key, argc, argv, myVals), l);
	gen0AddStmt(stmt, NULL);
	stmt = foamNew(FOAM_BCall, 2, FOAM_BVal_Halt,
		       foamNewSInt(FOAM_Halt_BadUnionCase));
	gen0AddStmt(stmt, NULL);
	gen0AddStmt(foamNewLabel(l), NULL);

	foam = foamNewRElt(format, foamCopy(whole), (AInt) 1);

	if (type != FOAM_Word)
		foam = foamNewCast(type, foam);

	return foam;
}

local Foam
gen0UnionCase(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	Foam foam = gen0UnionCaseBool(key, argc, argv, vals);
	return foamNewCast(FOAM_Word, foam);
}

local Foam
gen0UnionCaseBool(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0MakeUnionFormat();
	AInt	index  = gen0UnionCaseIndex(key, argv[1]);
	Foam	whole  = genFoamArg(argv, vals, int0);
	Foam	foam   = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntEQ;
	foam->foamBCall.argv[0] = foamNewRElt(format, 
					      foamNewCast(FOAM_Rec, whole), (AInt) 0);
	foam->foamBCall.argv[1] = foamNewSInt(index);

	return foam;
}

local Foam
gen0UnionSet(FoamTag type, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0MakeUnionFormat();
	Foam	whole  = genFoamArg(argv, vals, int0);
	Foam	index  = foamNewSInt(gen0UnionCaseIndex(key, argv[1]));
	Foam	value  = genFoamArg(argv, vals, 2);

	whole = gen0MakeMultiEvaluable(FOAM_Rec, format, foamNewCast(FOAM_Rec, whole));

	gen0AddStmt(gen0RSet(whole, format, (AInt) 0, index), NULL);
	gen0AddStmt(gen0RSet(whole, format, (AInt) 1, value), NULL);

	return foamNewRElt(format, foamCopy(whole), (AInt) 1);
}

local Foam
gen0UnionDispose(Length argc,AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}

/* Enumerations. */

local Foam
gen0EnumEqual(AbSyn *argv, Foam *vals)
{
	Foam	arg0 = genFoamArg(argv, vals, int0);
	Foam	arg1 = genFoamArg(argv, vals, 1);
	Foam	foam = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntEQ;
	foam->foamBCall.argv[0] = arg0;
	foam->foamBCall.argv[1] = arg1;

	return foamNewCast(FOAM_Word, foam);
}

local Foam
gen0EnumNotEqual(AbSyn *argv, Foam *vals)
{
	Foam	arg0 = genFoamArg(argv, vals, int0);
	Foam	arg1 = genFoamArg(argv, vals, 1);
	Foam	foam = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntNE;
	foam->foamBCall.argv[0] = arg0;
	foam->foamBCall.argv[1] = arg1;

	return foamNewCast(FOAM_Word, foam);
}

/* TrailingArrays. */

local Foam
gen0TrailingNew(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm tf = symeType(syme);
	AInt format = gen0TrailingFormatNumber(key);
	Foam arg0   = genFoamArg(argv, vals, int0); /* size  */
	Foam arg1   = genFoamArg(argv, vals, 1);    /* hdr   */
	Foam arg2   = genFoamArg(argv, vals, 2);    /* proto */
	Foam whole, hdr, proto, sz;
	int  i, iargc, aargc;
	iargc = tfAsMultiArgc(tfTrailingIArg(key));
	aargc = tfAsMultiArgc(tfTrailingAArg(key));

	whole = gen0Temp0(FOAM_TR, format);
	sz    = foamNewCast(FOAM_SInt, arg0);
	hdr   = gen0CrossToMulti(arg1, tfDefineeMaybeType(tfMapArgN(tf, 1)));
	proto = gen0CrossToMulti(arg2, tfDefineeMaybeType(tfMapArgN(tf, 2)));

	/* Idea is to generate:
	 * tr   := TRNew(fmt, sz)
	 * tr.x := argx
	 * -- ... and maybe initialize the trailing part as well...
	 */
	gen0AddStmt(foamNewSet(foamCopy(whole), foamNewTRNew(format, sz)), NULL);
	for (i=0; i < iargc; i++) {
		gen0AddStmt(foamNewSet(foamNewIRElt(format, foamCopy(whole), i),
				       hdr->foamValues.argv[i]), NULL);
	}
	return foamNewCast(FOAM_Word, whole);
}

local Foam
gen0TrailingDispose(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}

local Foam
gen0TrailingElt(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm tf  = tfDefineeType(symeType(syme));
	AInt format = gen0TrailingFormatNumber(key);
	Foam whole  = genFoamArg(argv, vals, int0);
	Foam foam = NULL;
	AInt idx;

	whole = foamNewCast(FOAM_TR, whole);
	if (argc == 2) {
		TForm elt = tfMapMultiArgN(tf, argc, 1);
		TForm xtf  = tfTrailingIArg(key);
		idx  = gen0TrailingIndex(xtf, elt);
		foam = foamNewIRElt(format, whole, idx);
	}
	else if (argc == 3) {
		TForm elt = tfMapMultiArgN(tf, argc, 2);
		TForm xtf  = tfTrailingAArg(key);
		Foam  arg1 = genFoamArg(argv, vals, 1);
		idx  = gen0TrailingIndex(xtf, elt);
		arg1 = foamNewCast(FOAM_SInt, arg1);
		arg1 = foamNew(FOAM_BCall, 2, FOAM_BVal_SIntPrev, arg1);
		foam = foamNewTRElt(format, whole, arg1, idx);
	}
	else {
		tf = NULL;
		bug("badly formed special operation");
	}

	return foam;
}

local Foam
gen0TrailingSet(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm tf  = tfDefineeType(symeType(syme));
	AInt format = gen0TrailingFormatNumber(key);
	Foam whole  = genFoamArg(argv, vals, int0);
	Foam ref, val;
	AInt idx;
	
	whole = foamNewCast(FOAM_TR, whole);
	if (argc == 3) {
		TForm elt  = tfMapMultiArgN(tf, argc, 1);
		TForm xtf  = tfTrailingIArg(key);
		Foam  arg1 = genFoamArg(argv, vals, 2);
		idx  = gen0TrailingIndex(xtf, elt);
		val = gen0MakeMultiEvaluable(FOAM_TR, format, arg1);
		ref = foamNewIRElt(format, whole, idx);
	}
	else if (argc == 4) {
		TForm elt  = tfMapMultiArgN(tf, argc, 2);
		TForm xtf  = tfTrailingAArg(key);
		Foam  arg1 = genFoamArg(argv, vals, 1);
		Foam  arg2 = genFoamArg(argv, vals, 3);
		idx  = gen0TrailingIndex(xtf, elt);
		arg1 = foamNewCast(FOAM_SInt, arg1);
		arg1 = foamNew(FOAM_BCall, 2, FOAM_BVal_SIntPrev, arg1);
		val  = gen0MakeMultiEvaluable(FOAM_TR, format, arg2);
		ref  = foamNewTRElt(format, whole, arg1, idx);
	}
	else {
		tf = NULL;
		val = ref = NULL; 
		bug("badly formed special operation");
	}
	
	gen0AddStmt(foamNewSet(ref, val), NULL);
	return foamCopy(val);
}

/*****************************************************************************
 *
 * :: End of specific generators for special operations.
 *
 ****************************************************************************/


local Foam
gen0PPartialSuccess(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0PPartialFormatNumber(key);
	AInt	index;
	Foam	whole = gen0Temp0(FOAM_Rec, format);

	gen0AddStmt(gen0RNew(whole, format), NULL);
	for (index = 0; index < argc; index += 1) {
		TForm tf = tfRecordArgN(key, index);
		Foam value = genFoamArg(argv, vals, index);
		FoamTag type = gen0Type(tf, NULL);
		if (type != FOAM_Word)
			value = foamNewCast(type, value);
		gen0AddStmt(gen0RSet(whole, format, index, value), NULL);
	}

	return foamCopy(whole);
}

local Foam
gen0PPartialFailed(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	return foamNewCast(FOAM_Word, foamNew(FOAM_BCall, 1, FOAM_BVal_PtrNil));
}


/*****************************************************************************
 *
 * :: Special matchers
 *
 ****************************************************************************/

local void gfm0MatchUnion(Syme syme, TForm key, AbSyn ab, Foam in);
local void gfm0MatchRecord(TForm key, AbSyn ab, Foam in);
local void gfm0MatchRawRecord(TForm key, AbSyn ab, Foam in);

void
gfm0SpecialMatch(Syme syme, AbSyn ab, Foam in)
{
	Symbol sym = symeId(syme);

	if (sym == ssymBracket) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsRecord(key))
			gfm0MatchRecord(key, ab, in);
		else if (tfIsRawRecord(key))
			gfm0MatchRawRecord(key, ab, in);
		else if (tfIsUnion(key))
			gfm0MatchUnion(syme, key, ab, in);
	}
	else {
		bug("not implemented");
	}
}

local void
gfm0MatchUnion(Syme syme, TForm key, AbSyn ab, Foam in)
{
	AInt format = gen0MakeUnionFormat();
	TForm tf0   = tfMapMultiArgN(symeType(syme), 2, int0);
	AInt index  = gen0UnionIndex(key, tf0);
	Foam foam   = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntEQ;
	foam->foamBCall.argv[0] = foamNewRElt(format,
					      foamNewCast(FOAM_Rec, in), (AInt) 0);
	foam->foamBCall.argv[1] = foamNewSInt(index);
	gen0AddStmt(foamNewIf(foamNotThis(foam), gfm0MatchExitLabel()), ab);

	gfm0MatchExpr(ab->abApply.argv[0], foamNewRElt(format, foamNewCast(FOAM_Rec, in), 1));
}

local void
gfm0MatchRecord(TForm key, AbSyn ab, Foam lhs)
{
	bug("not implemented");
}

local void
gfm0MatchRawRecord(TForm key, AbSyn ab, Foam lhs)
{
	bug("not implemented");
}

local void
gfm0MatchTrailingArray(TForm key, AbSyn ab, Foam lhs)
{
	bug("not implemented");
}
