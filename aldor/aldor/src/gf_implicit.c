/*****************************************************************************
 *
 * gf_implicit.c: Foam code generation for implicit exports
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file implements Foam code generation from symes for implicit
 * domain exports. At the moment we only deal with array operations.
 */

#include "gf_implicit.h"
#include "util.h"
#include "syme.h"
#include "lib.h"
#include "comsg.h"
#include "strops.h"
#include "symbol.h"

extern Bool	genfoamDebug;
extern Bool	genfoamHashDebug;
extern Bool	genfoamConstDebug;


/*****************************************************************************
 *
 * :: Local helper functions.
 *
 ****************************************************************************/

local Foam	gen1ImplicitExport	(Syme, FoamTag);
local void	gen0ImplicitPANew	(FoamList, FoamTag);
local void	gen0ImplicitPAGet	(FoamList, FoamTag);
local void	gen0ImplicitPASet	(FoamList, FoamTag);
local void	gen0ImplicitPRGet	(FoamList, FoamTag);
local void	gen0ImplicitPRSet	(FoamList, FoamTag);
local void	gen0ImplicitPRSize	(FoamList, FoamTag);
local FoamTag	gen1ImplicitType	(TForm);
local TForm	gen1ImplicitRepValue	(SymeList);
local Syme	gen1ImplicitRep		(SymeList);
local AbSyn	ab0ImplicitExportArgs	(TForm);
local AbSyn	ab1ImplicitExportArg	(Length);
local FoamList	gen0ImplicitExportArgs	(TForm);
local Foam	gen1ImplicitExportArg	(TForm, Length);
local Symbol	gen0ImplicitArgName	(Length);


/*
 * Create an explicit export: see gen0DefineRhs and gen0Lambda
 * for more details on how we do this. The code in gf_fortran may
 * also be helpful.
 */
void
gen0ImplicitExport(Syme syme, SymeList context, AbSyn ab)
{
	TForm	tf;
	FoamTag	repTag;
	Foam	lhs, rhs, def;


	/* What is the Rep of this domain? */
	tf = gen1ImplicitRepValue(context);


	/* Get the FOAM type for this Rep */
	if (tf)
		repTag = gen1ImplicitType(tf);
	else
	{
		/* Raise an error because we can't find Rep */
		comsgWarning(ab, ALDOR_E_GenImpNoRep);


		/* Non-fatal so continue */
		repTag = FOAM_Word;
	}


	/* Get the rhs of the export definition */
	rhs = gen1ImplicitExport(syme, repTag);
	if (!rhs) return;


	/* Create the FOAM for the lhs */
	lhs = gen0ExtendSyme(syme);


	/*
	 * If this is a domain export then record the foam loc/lex
	 * used to hold the value of this syme.
	 */
	if (gen0IsDomLevel(gen0State->tag) && gen0State->tag != GF_File)
		gen0SymeSetInit(syme, lhs);


	/* Create a definition */
	def = foamNewDef(lhs, rhs);


	/* Not sure if this hackery is needed anymore */
	def->foamDef.hdr.defnId = symeDefnNum(syme);


	/* Add the definition to the code stream */
	gen0AddStmt(def, (AbSyn)NULL);
}


local Foam
gen1ImplicitExport(Syme syme, FoamTag repTag)
{
	TForm		tf, tfret;
	AInt		retfmt, index;
	Foam		foam, clos;
	FoamTag		retType;
	AbSyn		params, oldex, id;
	FoamList	pars;
	Length		i, gfTag;
	GenFoamState	saved;
	Hash		hash;


	/* Paranoia */
	assert(syme);


	/* Get the type of this syme */
	tf = symeType(syme);
	assert (tfIsMap(tf));


	/* Name of the export */
	gen0ProgName = strCopy(symeString(syme));


	/* Type hash code */
	hash = tfHash(tf);


	/* Is this something we know about? */
	for (i = gfTag = 0;(i < GFI_LIMIT) && !gfTag; i++)
	{
		struct gf_impl_info *e = &gfImplicitInfoTable[i];

		if (e->type != hash) continue;
		if (!strEqual(e->name, gen0ProgName)) continue;
		gfTag = i + 1;
	}


	/* Did we recognise it? */
	if (!gfTag)
	{
		bug("[%s] %s#%ld not recognised\n",
			"gen1ImplicitExport", gen0ProgName, hash);
		return (Foam)NULL;
	}
	else
		gfTag--;


	/* Note the function signature */
	tfret   = tfMapRet(tf);
	retType = gen0Type(tfret, &retfmt);


	/* Fake up a bit of absyn */
	id = abNewId(sposNone, symIntern(gen0ProgName));
	abSetDefineIdx(id, symeDefnNum(syme));


	/* Not sure if we need this ... */
	oldex = gen0ProgPushExporter(id);


	/* Deal with const number */
	/* gen0AddConst(symeConstNum(syme), gen0NumProgs); */
	genSetConstNum(syme, abDefineIdx(id), (UShort) gen0NumProgs, true);


	/* Create a closure for the function */
	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(gen0ProgName, id);


	/* What format number are we using? */
	index = gen0FormatNum;


	/* Save the current state */
	saved = gen0ProgSaveState(PT_ExFn);


	/*
	 * Deal with special return types. None of these
	 * ought to appear at the moment but there is no
	 * point in creating extra work for the future.
	 */
	if (tfIsMulti(tfret))
		retfmt = gen0MultiFormatNumber(tfret);

	if (tfIsGenerator(tfret))
		foamProgSetGenerator(foam);


	/* Create the parameters for this function */
	params = ab0ImplicitExportArgs(tfMapArg(tf));


	/* Initialise the program state */
	gen0State->type    = tf;
	gen0State->param   = params;
	gen0State->program = foam;


	/* Not sure if we really need this ... see below */
#ifdef PUSH_FORAMTS
	gen0PushFormat(index);
#endif


	/* Create the parameter list */
	pars = gen0ImplicitExportArgs(tfMapArg(tf));


	/* Generate code for the body of this export */
	switch (gfTag)
	{
	   case GFI_PackedArrayNew:
		gen0ImplicitPANew(pars, repTag);
		break;
	   case GFI_PackedArrayGet:
		gen0ImplicitPAGet(pars, repTag);
		break;
	   case GFI_PackedArraySet:
		gen0ImplicitPASet(pars, repTag);
		break;
	   case GFI_PackedRecordSet:
		gen0ImplicitPRSet(pars, repTag);
		break;
	   case GFI_PackedRecordGet:
		gen0ImplicitPRGet(pars, repTag);
		break;
	   case GFI_PackedRepSize:
		gen0ImplicitPRSize(pars, repTag);
		break;
	   default:
		bug("[%s] GFI tag #%d not recognised\n",
			"gen1ImplicitExport", gfTag);
	}


#ifdef PUSH_FORAMTS
	gen0ProgAddFormat(index);
	gen0ProgFiniEmpty(foam, retType, retfmt);
#else
	/*
	 * Finish off the FOAM creation. Note that we want to
	 * use a basic machine type for the return type of this
	 * function so that Fortran can understand the result.
	 * This means we use `rtype' in gen0ProgFiniEmpty()
	 * rather than `retType' which we would do normally.
	 */
 	gen0UseStackedFormat(int0); /* These two lines provide a format */
 	gen0ProgPushFormat(int0);   /* for the lexical argument `op'   */
	gen0ProgFiniEmpty(foam, retType, retfmt);
#endif


	/* Optimisation bits */
	/* foam->foamProg.infoBits = IB_INLINEME; */
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);


	/* Compute side-effects of this foam */
	/* gen0ComputeSideEffects(foam); */


	/* Restore the saved state before returning */
	gen0ProgRestoreState(saved);
	return clos;
}


/*
 * Construct the body of PackedArrayNew: SInt -> Arr
 */
local void
gen0ImplicitPANew(FoamList pars, FoamTag repTag)
{
	Foam		par, foam;


	/* Extract the only parameter (array size) */
	assert(pars);
	par = foamCopy(car(pars)); pars = cdr(pars);
	assert(!pars);


	/* Construct a new array */
	foam = foamNewANew(repTag, par);


	/* Finally return the new array */
	foam = foamNewReturn(foam);
	gen0AddStmt(foam, (AbSyn)NULL);
}


/*
 * Construct the body of PackedArrayGet: (Arr, SInt) -> %
 */
local void
gen0ImplicitPAGet(FoamList pars, FoamTag repTag)
{
	Foam		parArr, parElt, foam;


	/* Get the array */
	assert(pars);
	parArr = foamCopy(car(pars)); pars = cdr(pars);


	/* Get the index */
	assert(pars);
	parElt = foamCopy(car(pars)); pars = cdr(pars);
	assert(!pars);


	/* Construct an array access */
	foam = foamNewAElt(repTag, parElt, parArr);


	/* Cast to uniform type */
	foam = foamNewCast(FOAM_Word, foam);


	/* Return the array element selected */
	foam = foamNewReturn(foam);
	gen0AddStmt(foam, (AbSyn)NULL);
}


/*
 * Construct the body of PackedArraySet: (Arr, SInt, %) -> %
 */
local void
gen0ImplicitPASet(FoamList pars, FoamTag repTag)
{
	Foam		parArr, parElt, parVal, foam, cast;


	/* Get the array */
	assert(pars);
	parArr = foamCopy(car(pars)); pars = cdr(pars);


	/* Get the index */
	assert(pars);
	parElt = foamCopy(car(pars)); pars = cdr(pars);


	/* Get the value */
	assert(pars);
	parVal = foamCopy(car(pars)); pars = cdr(pars);
	assert(!pars);


	/* Cast to raw type */
	cast = foamNewCast(repTag, foamCopy(parVal));


	/* Construct an array access */
	foam = foamNewAElt(repTag, parElt, parArr);


	/* Create the update */
	foam = foamNewSet(foam, cast);
	gen0AddStmt(foam, (AbSyn)NULL);


	/* Return the value inserted */
	foam = foamNewReturn(parVal);
	gen0AddStmt(foam, (AbSyn)NULL);
}


/*
 * Construct the body of PackedRecordGet: Ptr -> %
 */
local void
gen0ImplicitPRGet(FoamList pars, FoamTag repTag)
{
	Foam		par, foam;


	/* Get the pointer */
	assert(pars);
	par = foamCopy(car(pars)); pars = cdr(pars);
	assert(!pars);


	/* Construct an array access */
	foam = foamNewAElt(repTag, foamNewSInt(int0), par);


	/* Cast to uniform type */
	foam = foamNewCast(FOAM_Word, foam);


	/* Return the value extracted */
	foam = foamNewReturn(foam);
	gen0AddStmt(foam, (AbSyn)NULL);
}


/*
 * Construct the body of PackedRecordSet: (Ptr, %) -> %
 */
local void
gen0ImplicitPRSet(FoamList pars, FoamTag repTag)
{
	Foam		parPtr, parVal, foam, cast;


	/* Get the pointer and cast to Arr */
	assert(pars);
	parPtr = foamCopy(car(pars)); pars = cdr(pars);
	parPtr = foamNewCast(FOAM_Arr, parPtr);


	/* Get the value */
	assert(pars);
	parVal = foamCopy(car(pars)); pars = cdr(pars);
	assert(!pars);


	/* Cast to raw type */
	cast = foamNewCast(repTag, foamCopy(parVal));


	/* Construct an array access */
	foam = foamNewAElt(repTag, foamNewSInt(int0), parPtr);


	/* Create the update */
	foam = foamNewSet(foam, cast);
	gen0AddStmt(foam, (AbSyn)NULL);


	/* Return the value inserted */
	foam = foamNewReturn(parVal);
	gen0AddStmt(foam, (AbSyn)NULL);
}


/*
 * Construct the body of PackedRepSize: () -> SInt
 */
local void
gen0ImplicitPRSize(FoamList pars, FoamTag repTag)
{
	Foam		foam;
	FoamBValTag	bvTag;


	/* Check that there are no parameters */
	assert(!pars);

#if UseTypeTag
	/* Work out which TypeTag* call we ought to use */
	switch (repTag)
	{
#if 0
		case FOAM_Int8   : bvTag = FOAM_BVal_TypeInt8;   break;
		case FOAM_Int16  : bvTag = FOAM_BVal_TypeInt16;  break;
		case FOAM_Int32  : bvTag = FOAM_BVal_TypeInt32;  break;
		case FOAM_Int64  : bvTag = FOAM_BVal_TypeInt64;  break;
		case FOAM_Int128 : bvTag = FOAM_BVal_TypeInt128; break;
#endif
		case FOAM_Nil    : bvTag = FOAM_BVal_TypeNil;    break;
		case FOAM_Char   : bvTag = FOAM_BVal_TypeChar;   break;
		case FOAM_Bool   : bvTag = FOAM_BVal_TypeBool;   break;
		case FOAM_Byte   : bvTag = FOAM_BVal_TypeByte;   break;
		case FOAM_HInt   : bvTag = FOAM_BVal_TypeHInt;   break;
		case FOAM_SInt   : bvTag = FOAM_BVal_TypeSInt;   break;
		case FOAM_BInt   : bvTag = FOAM_BVal_TypeBInt;   break;
		case FOAM_SFlo   : bvTag = FOAM_BVal_TypeSFlo;   break;
		case FOAM_DFlo   : bvTag = FOAM_BVal_TypeDFlo;   break;
		case FOAM_Word   : bvTag = FOAM_BVal_TypeWord;   break;
		case FOAM_Clos   : bvTag = FOAM_BVal_TypeClos;   break;
		case FOAM_Ptr    : bvTag = FOAM_BVal_TypePtr;    break;
		case FOAM_Rec    : bvTag = FOAM_BVal_TypeRec;    break;
		case FOAM_Arr    : bvTag = FOAM_BVal_TypeArr;    break;
		case FOAM_TR     : bvTag = FOAM_BVal_TypeTR;     break;
		default          : bvTag = FOAM_BVal_TypeWord;   break;
	}


	/* Create a call to get the type tag */
	foam = foamNew(FOAM_BCall, 1, bvTag);


	/* Pass this to the builtin size computer */
	foam = foamNew(FOAM_BCall, 2, FOAM_BVal_RawRepSize, foam);
#else
	/* Work out which SizeOf* call we ought to use */
	switch (repTag)
	{
#if 0
		case FOAM_Int8   : bvTag = FOAM_BVal_SizeOfInt8;   break;
		case FOAM_Int16  : bvTag = FOAM_BVal_SizeOfInt16;  break;
		case FOAM_Int32  : bvTag = FOAM_BVal_SizeOfInt32;  break;
		case FOAM_Int64  : bvTag = FOAM_BVal_SizeOfInt64;  break;
		case FOAM_Int128 : bvTag = FOAM_BVal_SizeOfInt128; break;
#endif
		case FOAM_Nil    : bvTag = FOAM_BVal_SizeOfNil;    break;
		case FOAM_Char   : bvTag = FOAM_BVal_SizeOfChar;   break;
		case FOAM_Bool   : bvTag = FOAM_BVal_SizeOfBool;   break;
		case FOAM_Byte   : bvTag = FOAM_BVal_SizeOfByte;   break;
		case FOAM_HInt   : bvTag = FOAM_BVal_SizeOfHInt;   break;
		case FOAM_SInt   : bvTag = FOAM_BVal_SizeOfSInt;   break;
		case FOAM_BInt   : bvTag = FOAM_BVal_SizeOfBInt;   break;
		case FOAM_SFlo   : bvTag = FOAM_BVal_SizeOfSFlo;   break;
		case FOAM_DFlo   : bvTag = FOAM_BVal_SizeOfDFlo;   break;
		case FOAM_Word   : bvTag = FOAM_BVal_SizeOfWord;   break;
		case FOAM_Clos   : bvTag = FOAM_BVal_SizeOfClos;   break;
		case FOAM_Ptr    : bvTag = FOAM_BVal_SizeOfPtr;    break;
		case FOAM_Rec    : bvTag = FOAM_BVal_SizeOfRec;    break;
		case FOAM_Arr    : bvTag = FOAM_BVal_SizeOfArr;    break;
		case FOAM_TR     : bvTag = FOAM_BVal_SizeOfTR;     break;
		default          : bvTag = FOAM_BVal_SizeOfWord;   break;
	}


	/* Create a call to get the type tag */
	foam = foamNew(FOAM_BCall, 1, bvTag);
#endif


	/* Return the result of the function call */
	foam = foamNewReturn(foam);
	gen0AddStmt(foam, (AbSyn)NULL);
}


local FoamTag
gen1ImplicitType(TForm tf)
{
	FoamTag result;


	/* Get the FOAM type for this tform */
	result = gen0Type(tf, (AInt *)NULL);


	/* Filter out things we can't handle */
	switch (result)
	{
		/* Start with the cases we can handle */
#if 0
		case FOAM_Int8   : /* Fall through */
		case FOAM_Int16  : /* Fall through */
		case FOAM_Int32  : /* Fall through */
		case FOAM_Int64  : /* Fall through */
		case FOAM_Int128 : /* Fall through */
#endif
		case FOAM_Nil    : /* Fall through */
		case FOAM_Char   : /* Fall through */
		case FOAM_Bool   : /* Fall through */
		case FOAM_Byte   : /* Fall through */
		case FOAM_HInt   : /* Fall through */
		case FOAM_SInt   : /* Fall through */
		case FOAM_BInt   : /* Fall through */
		case FOAM_SFlo   : /* Fall through */
		case FOAM_DFlo   : /* Fall through */
		case FOAM_Word   : /* Fall through */
		case FOAM_Clos   : /* Fall through */
		case FOAM_Ptr    : break;


		/* These are the ones we cannot handle */
		case FOAM_Rec    : /* Fall through */
		case FOAM_RRec   : /* Fall through */
		case FOAM_Arr    : /* Fall through */
		case FOAM_TR     : /* Fall through */
		default          : result = FOAM_Word; break;
	}


	/* Return the tag */
	return result;
}


/*
 * Extract the value of the Rep syme. Assumes that
 * there is only one Rep and that it is a type-valued
 * constant.
 */
local TForm
gen1ImplicitRepValue(SymeList symes)
{
	Syme	syme;
	TForm	result;


	/* Get the Rep syme */
	syme = gen1ImplicitRep(symes);


	/* Drop out if we failed to find Rep */
	if (!syme) return (TForm)NULL;


	/* Get the type of the syme */
	result = symeType(syme);


	/* Probably a define: get its value */
	switch (tfTag(result))
	{
	   case TF_Assign:
		result = tfAssignVal(result);
		break;
	   case TF_Define:
		result = tfDefineVal(result);
		break;
	   default:
		break;
	}


	/* Return the type */
	return result;
}


/*
 * Extract the Rep syme
 */
local Syme
gen1ImplicitRep(SymeList symes)
{
	/* Assume that there is only one Rep */
	for (;symes;symes = cdr(symes))
	{
		Syme syme = car(symes);

		if (strEqual(symeString(syme), "Rep"))
			return syme;
	}
	return (Syme)NULL;
}


/*
 * Invent some absyn for the parameter list of a function.
 */
local AbSyn
ab0ImplicitExportArgs(TForm tf)
{
	/* How many parameters does this function have? */
	Length numargs = tfIsMulti(tf) ? tfMultiArgc(tf) : 1;


	/* Deal with single and multiple arguments separately */
	if (numargs > 1)
	{
		/* Multiple arguments: (Comma ...) */
		Length		i;
		AbSynList	lst = listNil(AbSyn);


		/* Create each argument */
		for (i = 0; i < numargs; i++)
		{
			AbSyn arg = ab1ImplicitExportArg(i);
			lst = listCons(AbSyn)(arg, lst);
		}


		/* Make sure that the list is in the right order */
		lst = listNReverse(AbSyn)(lst);


		/* Return the absyn for the parameter list */
		return abNewCommaL(sposNone, lst);
	}
	else
		return ab1ImplicitExportArg((Length)0);
}


local AbSyn
ab1ImplicitExportArg(Length i)
{
	/* Create absyn for single argument */
	Symbol	sym;
	AbSyn	type, param;


	/* Construct a name for this parameter. */
	sym = gen0ImplicitArgName(i);


	/* Create the absyn for the name and type */
	param = abNewId(sposNone, sym);
	type  = abNewId(sposNone, symIntern("Word"));


	/* Return the parameter declaration */
	return abNewDeclare(sposNone, param, type);
}


/*
 * Construct the parameter list for the function
 */
local FoamList
gen0ImplicitExportArgs(TForm tf)
{
	Foam		par;
	FoamList	lst = listNil(Foam);
	Length		i, numargs;


	/* How many parameters does this function have? */
	numargs = tfIsMulti(tf) ? tfMultiArgc(tf) : 1;


	/* Deal with single and multiple arguments separately */
	if (numargs > 1)
	{
		/* Process each argument */
		for (i = 0;i < numargs;i++)
		{
			/* Get the next argument */
			TForm	t = tfMultiArgN(tf, i);

			par = gen1ImplicitExportArg(t, (Length)i);
			lst = listCons(Foam)(par, lst);
		}
	}
	else if (numargs == 1)
	{
		/* A single argument */
		par = gen1ImplicitExportArg(tf, (Length)0);
		lst = listCons(Foam)(par, lst);
	}


	/* Reverse the list and return it */
	lst = listNReverse(Foam)(lst);
	return lst;
}


local Foam
gen1ImplicitExportArg(TForm tf, Length i)
{
	FoamTag		fmtype;
	Foam		decl;
	Symbol		sym;
	String		symstr;


	/* What was the name of this parameter? */
	sym    = gen0ImplicitArgName(i);
	symstr = strCopy(symString(sym));


	/* What is the type of this argument? */
	fmtype  = gen0Type(tf, NULL);


	/* Create a declaration for this parameter */
	decl = foamNewDecl(fmtype, symstr, emptyFormatSlot);


	/* Add the new parameter to the FOAM prog */
	gen0AddParam(decl);


	/* Return the FOAM for this parameter */
	return foamNewPar(i);
}


/*
 * We need to invent names for parameters for the wrapper
 * functions. We do this based on the argument number.
 */
local Symbol
gen0ImplicitArgName(Length i)
{
	char	num[40];

	(void)sprintf(num, "%s%d", "x", (int) i);
	return symIntern(num);
}


/*
 * Although not perfect, this table allows us to check if a given
 * syme is an implicit export that we know about. It also gives us
 * a tag to be used when deciding which FOAM body to constuct.
 */
struct gf_impl_info gfImplicitInfoTable[] =
{
	{ GFI_PackedArrayNew,  "PackedArrayNew",  (Hash)( 230929250L) },
	{ GFI_PackedArrayGet,  "PackedArrayGet",  (Hash)( 504190320L) },
	{ GFI_PackedArraySet,  "PackedArraySet",  (Hash)(   7666178L) },
	{ GFI_PackedRecordSet, "PackedRecordSet", (Hash)( 278278893L) },
	{ GFI_PackedRecordGet, "PackedRecordGet", (Hash)( 632591839L) },
	{ GFI_PackedRepSize,   "PackedRepSize",   (Hash)( 932039034L) },
};
