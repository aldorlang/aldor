/*****************************************************************************
 *
 * gf_fortran.c: Foam code generation for the Aldor/Fortran interface.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file handles almost all the code relating to the Aldor/Fortran
 * interface. The remaining bits are left in genfoam and with some of
 * the support functions in gf_rtime.c.
 *
 * To do:
 *  - factor out the conversions into separate functions
 *  - split gen0ModifyFortranCall into lots of little functions
 *  - shift the Aldor/Fortran conversions out to libraries
 */


/* TODO: delete the #includes that aren't needed anymore */
#include "compcfg.h"
#include "fortran.h"
#include "genfoam.h"
#include "gf_add.h"
#include "gf_excpt.h"
#include "gf_fortran.h"
#include "gf_gener.h"
#include "gf_imps.h"
#include "gf_prog.h"
#include "gf_reference.h"
#include "gf_rtime.h"
#include "gf_util.h"
#include "of_inlin.h"
#include "of_util.h"
#include "optfoam.h"
#include "opttools.h"
#include "scobind.h"
#include "simpl.h"
#include "spesym.h"
#include "stab.h"
#include "tform.h"
#include "util.h"


extern Bool	genfoamDebug;
extern Bool	genfoamHashDebug;
extern Bool	genfoamConstDebug;

/*****************************************************************************
 *
 * :: Local functions for the Aldor-calls-Fortran side.
 *
 ****************************************************************************/

local Foam	gen0FtnFunValue		(Foam, TForm, Syme);
local Foam	gen0FtnArrayValue	(AbSyn, Foam, TForm);
local Foam	gen0FtnUpdateArray	(AbSyn, Foam, Foam, TForm);
local Foam	gen0FtnFSArrayValue	(AbSyn, Foam, TForm);
local Foam	gen0FtnUpdateFSArray	(AbSyn, Foam, Foam, TForm);
local Foam	gen0FtnFSArrayLen	(AbSyn, Foam, TForm);

#if 0
local Foam	gen0MakeAutoApply(AbSyn, Syme, TForm, FoamTag, AInt, Foam *);
local Foam	gen0FtnComplexGet	(AbSyn, Foam, TForm, SymeList);
local Foam	gen0FtnComplexPut	(AbSyn, Foam, TForm, SymeList);
#endif


/*****************************************************************************
 *
 * Foam code generation for Aldor-calls-Fortran
 *
 ****************************************************************************/

/*
 * Rewrite the FOAM for a Fortran function/procedure call so that
 * the actual PCall is executed with arguments and return values
 * in Fortran format. Once we have finished mangling the call there
 * is very little work for genc to do.
 *
 * Future work: we ought not to make any assumptions about the format
 * of FTN_FSComplex values etc. Instead we ought to apply the coerce
 * function exported by domains satisfying FortranComplexReal etc.
 */
Foam
gen0ModifyFortranCall(Syme syme, Foam call, AbSyn ftnFnResult, Bool valueMode)
{
	TForm	tf = symeType(syme);
	Length	argc = tfMapArgc(tf), i;
	Foam arg, res;
	Foam foam;
	FoamList fixups;
	int numresults;

	/* --------------------------------------------- */
	/* This function is FAR too long and needs to be */
	/* cut into tiny fragments to make it readable.  */
	/* --------------------------------------------- */

	TForm		tfret;
	Foam		*argloc;
	Foam		tmpfoam, tmpvar, tmpget, tmpset;
	Foam		rhs;
	Foam		cpx, creal, cimag, rpart, ipart;
	FoamList	befCall, aftCall;
	FortranType	ftnType, ftnRetType;
	FoamTag		fmType, refType;
	int		extraArg;
	AInt		cfmt, afmt, dfmt;

	/* Compute return types */
	tfret       = tfMapRet(tf);
	ftnRetType  = ftnTypeFrDomTForm(tfret);


	/* Treat Char and Character in the same way */
	if (!ftnRetType && (gen0Type(tfret, NULL) == FOAM_Char))
		ftnRetType = FTN_Character;


	/* Do we have an extra first argument? */
	switch (ftnRetType)
	{
	   case FTN_Character:
	   case FTN_String:
	   case FTN_XLString:
		extraArg = 1;
		break;
	   default:
		extraArg = 0;
		break;
	}

	if (tfMapRetc(tf) > 1)
		bug("Too many return values for fortran function");


	numresults = tfMapArgc(tf);

	if (!tfIsNone(tfMapRet(tf)) || ftnFnResult)
		numresults += 1;


	foam    = call; /* Why do we alias call in this way? */
	fixups  = listNil(Foam);
	befCall = listNil(Foam);
	aftCall = listNil(Foam);


	for (i = 0; i < argc; i++) {
		TForm	tfi, tfiget, tfiset;
		FoamList ltmp;
		Bool	isReference;
		AbSyn	tmpab;
		FoamTag	fmret;
		Foam	retfoam, recfoam;
		Foam	tmparr = (Foam)NULL;
		SrcPos	fp;
		AInt	cfmt, afmt;

		tfi    = tfMapArgN(tf, i);
		arg    = foam->foamPCall.argv[i + extraArg];
		fp     = foamPos(arg);
		tmpab  = abNewNothing(fp);


		/* Skip any declaration */
		if (tfIsDeclare(tfi))
			tfi = tfDeclareType(tfi);


		/* Is it a reference? */
		if ((isReference = tfIsReferenceFn(tfi)))
			tfi = tfReferenceArg(tfi);


		/* What is the type of this argument? */
		ftnType = ftnTypeFrDomTForm(tfi);
		fmType  = gen0Type(tfi, NULL);


		/*
		 * Check to see if this is a known Fortran type. We
		 * really ought to be doing this during when we are
		 * processing the import/export declaration.
		 */
		if (!ftnType)
		{
			switch (fmType)
			{
				case FOAM_Bool: /* Fall through */
				case FOAM_Char: /* Fall through */
				case FOAM_SInt: /* Fall through */
				case FOAM_SFlo: /* Fall through */
				case FOAM_DFlo: /* Fall through */
				case FOAM_Clos: /* Fall through */
					break;
				default:
					comsgWarnPos(fp, ALDOR_W_FtnNotFtnArg);
			}
		}


		/* Remove the casts (if any) */
		while (foamTag(arg) == FOAM_Cast) 
			arg = arg->foamCast.expr;


		if (isReference)
		{
			/*
			 * When presented with the Aldor:
			 *
			 *    local a:T;
			 *    import {foo: (Ref T) -> ()} from Foreign Fortran;
			 *    foo(ref(a));
			 *
			 * the compiler converts ref(a) into a nullary
			 * function which returns a multi of two functions,
			 * the getter and the setter.
			 *
			 *    local a:T;
			 *    import {foo: (Ref T) -> ()} from Foreign Fortran;
			 *    foo
			 *    (
			 *       ():Cross(()->T, T->T) +->
			 *       (
			 *          deref   == ():T    +-> {free a:T; a},
			 *          update! == (v:T):T +-> {free a:T; a := v; v}
			 *       )
			 *    )
			 *
			 * What we need to do here is extract the getter and
			 * setter functions and then apply the getter function
			 * to initialise the temporary variable. This temporary
			 * is passed to Fortran and its value after the call
			 * is used in an application of the setter function.
			 */


			/* Compute the return types of the functions */
			tfiget  = tfMap(tfNone(), tfi);
			tfiset  = tfMap(tfi, tfi);
			tfret   = tfMulti(2, tfiget, tfiset);
			fmret   = gen0Type(tfret, NULL);


			/* Extract the getter/setter pair. */
			retfoam = foamCopy(arg);
			retfoam = gen0CCallFrFoam(fmret, retfoam,(Length) 0, &argloc);
			retfoam = gen0ApplyReturn(tmpab, (Syme)NULL, tfret,
								retfoam);


			/* Extract the getter */
			tmpfoam = foamCopy(retfoam);
			assert(foamTag(tmpfoam) == FOAM_Values);
			tmpget  = tmpfoam->foamValues.argv[0];


			/* Extract the setter */
			tmpfoam = foamCopy(retfoam);
			assert(foamTag(tmpfoam) == FOAM_Values);
			tmpset  = tmpfoam->foamValues.argv[1];


			/* Extract the value to pass to the function */
			tmpfoam = gen0CCallFrFoam(fmType, tmpget,(Length) 0, &argloc);
			tmpfoam = gen0ApplyReturn(tmpab, (Syme)NULL, tfi,
								tmpfoam);
		}
		else {
			tmpfoam = arg;
			tmpset   = 0;
		}


		/*
		 * Since Fortran uses call-by-reference we need to
		 * pass a pointer to each argument rather than the
		 * argument itself. We need to do this now rather than
		 * during C code generation otherwise the optimiser
		 * will perform invalid transformations on the FOAM.
		 *
		 * This part looks messy but it works and it makes
		 * C generation much simpler. The basic technique
		 * is to convert SingleInteger/SingleFloat etc into
		 * the corresponding machine types SInt/SFlo etc
		 * and stuff these in a record. We assume that any
		 * unrecognised type is represented as a pointer to
		 * something and will always be passed by reference.
		 * The user will have been warned about this already.
		 * 
		 * Array-like objects are passed through conversion
		 * conversion functions before and after the call.
		 * This allows, for example, sparse arrays to be
		 * passed to a Fortran function expecting a dense
		 * array and allows multi-dimensional arrays to be
		 * changed into Fortran format.
		 *
		 * Complex numbers are assumed to be represented as
		 * as Record(real:R, imag:R) where R is SingleFloat
		 * or DoubleFloat (which is Record(x:DFlo). These
		 * values are converted into the correct Fortran
		 * format values for the call.
		 */
		switch (ftnType)
		{
		   case FTN_Character :
			refType = FOAM_Char;
			tmpfoam = foamNewCast(refType, tmpfoam);
			break;
		   case FTN_Boolean :
			/* Fall through */
		   case FTN_SingleInteger :
			/* Store in a record */
			refType = FOAM_SInt;
			tmpfoam = foamNewCast(refType, tmpfoam);
			break;
		   case FTN_FSingle :
			refType = FOAM_SFlo;
			tmpfoam = foamNewCast(refType, tmpfoam);
			break;
		   case FTN_FDouble :
			refType = FOAM_DFlo;
			tmpfoam = gen0DoubleValue(foamCopy(tmpfoam));
			break;
		   case FTN_FSComplex:
			/* Convert Complex SF into COMPLEX REAL */
			refType = FOAM_Word;

			/* Create a Fortran-format local */
			cfmt    = gen0SingleCpxFormat(); /* COMPLEX REAL */
			afmt    = gen0AldorCpxFormat();  /* Complex SF */
			cpx     = gen0TempLocal0(FOAM_Rec, cfmt);

			/* Allocate storage for the COMPLEX REAL */
			recfoam = gen0RNew(cpx, cfmt);
			befCall = listCons(Foam)(recfoam, befCall);

			/* Copy the real part of the Complex SF */
			creal = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)0);
			creal = foamNewCast(FOAM_SFlo, creal);
			creal = gen0RSet(foamCopy(cpx), cfmt, (AInt)0, creal);
			befCall = listCons(Foam)(creal, befCall);

			/* Copy the imaginary part */
			cimag = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)1);
			cimag = foamNewCast(FOAM_SFlo, cimag);
			cimag = gen0RSet(foamCopy(cpx), cfmt, (AInt)1, cimag);
			befCall = listCons(Foam)(cimag, befCall);

			/* Now use the Fortran-format local */
			tmpfoam = foamCopy(cpx);
			break;
		   case FTN_FDComplex:
			/* Convert Complex DF into COMPLEX DOUBLE */
			refType = FOAM_Word;

			/* Create a Fortran-format local */
			cfmt    = gen0DoubleCpxFormat();  /* COMPLEX DOUBLE */
			afmt    = gen0AldorCpxFormat();   /* Complex DF */
			dfmt    = gen0MakeDoubleFormat(); /* DF */
			cpx     = gen0TempLocal0(FOAM_Rec, cfmt);

			/* Allocate storage for the COMPLEX DOUBLE */
			recfoam = gen0RNew(cpx, cfmt);
			befCall = listCons(Foam)(recfoam, befCall);

			/* Copy the real DFlo part of the Complex DF */
			creal = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)0);
			creal = foamNewRElt(dfmt, creal, (AInt)0);
			creal = gen0RSet(foamCopy(cpx), cfmt, (AInt)0, creal);
			befCall = listCons(Foam)(creal, befCall);

			/* Copy the imaginary part */
			cimag = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)1);
			cimag = foamNewRElt(dfmt, cimag, (AInt)0);
			cimag = gen0RSet(foamCopy(cpx), cfmt, (AInt)1, cimag);
			befCall = listCons(Foam)(cimag, befCall);

			/* Now use the Fortran-format local */
			tmpfoam = foamCopy(cpx);
			break;
		   case FTN_StringArray:
			/* Replace with (BArr, SInt) pair */
			refType = fmType;

			/* Store the original array value in a local */
			tmparr  = gen0TempLocal(refType);
			tmpfoam = foamNewSet(tmparr, tmpfoam);
			befCall = listCons(Foam)(tmpfoam, befCall);

			/* Apply the array converter */
			tmpfoam = gen0FtnFSArrayValue(tmpab, tmparr, tfi);
			break;
		   case FTN_Array:
			refType = fmType;

			/* Store the original array value in a local */
			tmparr  = gen0TempLocal(refType);
			tmpfoam = foamNewSet(tmparr, tmpfoam);
			befCall = listCons(Foam)(tmpfoam, befCall);

			/* Apply the array converter */
			tmpfoam = gen0FtnArrayValue(tmpab, tmparr, tfi);
			break;
		   default:
			switch (fmType)
			{
			   case FOAM_Bool:
				refType = FOAM_SInt;
				tmpfoam = foamNewCast(refType, foamCopy(tmpfoam));
				break;
		   	   case FOAM_Clos :
				refType = FOAM_Clos;
				tmpfoam = gen0FtnFunValue(tmpfoam, tfi, syme);
				break;
			   default:
			   	refType = fmType;
				break;
			}
			break;
		}


		/* Before the call we do all our packing */
		if (ftnType == FTN_StringArray)
		{
			/* Safety checks */
			assert(foamTag(tmpfoam) == FOAM_Values);
			assert(foamArgc(tmpfoam) == 2);


			/*
			 * Replace the BArr in the FOAM_Values pair
			 * with a temporary variable.
			 */
			tmpvar  = tmpfoam;
			tmpfoam = tmpvar->foamValues.argv[0];
			tmpfoam = gen0MakePointerTo(refType, tmpfoam, &ltmp);
			tmpvar->foamValues.argv[0] = tmpfoam;
		}
		else
			tmpvar  = gen0MakePointerTo(refType, tmpfoam, &ltmp);


		/* Code to be executed before the call */
		befCall = listNConcat(Foam)(ltmp, befCall);


		/*
		 * The argument to the call is now the tmpvar
		 * unless it is an StringArray, in which case
		 * it will be a FOAM_Values.
		 */
		call->foamPCall.argv[i + extraArg] = foamCopy(tmpvar);


		/*
		 * In Aldor, array elements are always passed by
		 * reference. Since Fortran can only change the
		 * elements of an array (it cannot change where the
		 * array will find the elements) there is no need
		 * to apply the ref-setter for arrays passed using
		 * ref(). However, we do need to apply the convert
		 * export from FortranArray to allow the elements
		 * of the array to be updated.
		 *
		 * Complex numbers also need special care if they
		 * are passed by reference: the updated Fortran
		 * value must be converted into a Complex R value
		 * which can be used by the ref-setter.
		 *
		 * For all other types that we recognise, we need
		 * to generate code to be executed immediately after
		 * the call which applies the ref-setter to update
		 * the Aldor reference.
		 */
		if (ftnType == FTN_Array)
		{
			/* tmparr holds the original array value */
			tmpfoam = gen0FtnUpdateArray(tmpab, tmparr, tmpvar, tfi);
			aftCall = listCons(Foam)(tmpfoam, aftCall);
		}
		else if (ftnType == FTN_StringArray)
		{
			/*
			 * tmparr holds the original array value while
			 * tmpvar holds a FOAM_Values with two elements,
			 * the flattened array and the length of the
			 * fixed string elements.
			 */
			assert(foamTag(tmpvar) == FOAM_Values);
			assert(foamArgc(tmpvar) == 2);
			tmpfoam = gen0FtnUpdateFSArray(tmpab, tmparr, tmpvar, tfi);
			aftCall = listCons(Foam)(tmpfoam, aftCall);
		}
		else if (isReference)
		{
			/*
			 * Now add the after-call code. References
			 * need to be updated with their new value,
			 * other values are just ignored.
			 *
			 * To do: use gen0FortranConvArg()
			 */
			tmpfoam = gen0ReadPointerTo(refType, tmpvar);
			switch (ftnType)
			{
			   case FTN_Character :
				/* Fall through */
			   case FTN_Boolean :
				/* Fall through */
			   case FTN_SingleInteger :
				tmpfoam = foamNewCast(FOAM_Word, tmpfoam);
				break;
			   case FTN_FSingle :
				tmpfoam = foamNewCast(FOAM_Word, tmpfoam);
				break;
			   case FTN_FDouble :
				tmpfoam = gen0MakeDoubleCode(tmpfoam, &ltmp);
				aftCall = listNConcat(Foam)(ltmp, aftCall);
				break;
			   case FTN_FSComplex:
				/*
				 * Convert COMPLEX REAL into Complex SF. To
				 * do this we store the updated Fortran value
				 * in a local. Then we copy and convert the
				 * two components into a new local which is
				 * passed to the ref-setter.
				 */
				cfmt = gen0SingleCpxFormat();
				afmt = gen0AldorCpxFormat();

				/* Create a local and store the updated value */
				tmpvar  = gen0TempLocal0(FOAM_Rec, cfmt);
				tmpfoam = foamNewSet(foamCopy(tmpvar), tmpfoam);
				aftCall = listCons(Foam)(tmpfoam, aftCall);

				/* Allocate storage for the Complex SF result */
				res     = gen0TempLocal0(FOAM_Rec, afmt);
				tmpfoam = gen0RNew(foamCopy(res), afmt);
				aftCall = listCons(Foam)(tmpfoam, aftCall);

				/* Copy the real part into the local */
				creal = foamNewRElt(cfmt, foamCopy(tmpvar),
						(AInt)0);
				creal = foamNewCast(FOAM_Word, creal);
				creal = gen0RSet(foamCopy(res), afmt, (AInt)0,
						creal);
				aftCall = listCons(Foam)(creal, aftCall);

				/* Copy the imaginary part into the local */
				cimag = foamNewRElt(cfmt, foamCopy(tmpvar),
						(AInt)1);
				cimag = foamNewCast(FOAM_Word, cimag);
				cimag = gen0RSet(foamCopy(res), afmt, (AInt)1,
						cimag);
				aftCall = listCons(Foam)(cimag, aftCall);

				/* Use the Complex DF as the result */
				tmpfoam = res;
				break;
			   case FTN_FDComplex:
				/*
				 * Convert COMPLEX DOUBLE into Complex DF.
				 * To do this we store the updated Fortran
				 * value in a local. Then we copy and convert
				 * the two components into a new local which
				 * is passed to the ref-setter.
				 */
				cfmt = gen0DoubleCpxFormat();
				afmt = gen0AldorCpxFormat();
				dfmt = gen0MakeDoubleFormat();

				/* Create a local and store the updated value */
				tmpvar  = gen0TempLocal0(FOAM_Rec, cfmt);
				tmpfoam = foamNewSet(foamCopy(tmpvar), tmpfoam);
				aftCall = listCons(Foam)(tmpfoam, aftCall);


				/* Create a DF for the real part */
				rpart = gen0TempLocal0(FOAM_Rec, dfmt);
				tmpfoam = gen0RNew(foamCopy(rpart), dfmt);
				aftCall = listCons(Foam)(tmpfoam, aftCall);


				/* Copy the real DFlo part into the DF */
				creal = foamNewRElt(cfmt, foamCopy(tmpvar),
						(AInt)0);
				creal = gen0RSet(foamCopy(rpart), dfmt,
						(AInt)0, creal);
				aftCall = listCons(Foam)(creal, aftCall);


				/* Create a DF for the imaginary part */
				ipart = gen0TempLocal0(FOAM_Rec, dfmt);
				tmpfoam = gen0RNew(foamCopy(ipart), dfmt);
				aftCall = listCons(Foam)(tmpfoam, aftCall);


				/* Copy the imaginary part into the DF */
				cimag = foamNewRElt(cfmt, foamCopy(tmpvar),
						(AInt)1);
				cimag = gen0RSet(foamCopy(ipart), dfmt,
						(AInt)0, cimag);
				aftCall = listCons(Foam)(cimag, aftCall);


				/* Create the Complex DF */
				res = gen0TempLocal0(FOAM_Rec, afmt);
				tmpfoam = gen0RNew(foamCopy(res), afmt);
				aftCall = listCons(Foam)(tmpfoam, aftCall);


				/* Fill in the slots of the Complex DF */
				creal = gen0RSet(foamCopy(res), afmt, (AInt)0,
						foamCopy(rpart));
				cimag = gen0RSet(foamCopy(res), afmt, (AInt)1,
						foamCopy(ipart));
				aftCall = listCons(Foam)(creal, aftCall);
				aftCall = listCons(Foam)(cimag, aftCall);


				/* Use the Complex DF as the result */
				tmpfoam = res;
				break;
			   default:
				switch (fmType)
				{
				   case FOAM_Bool:
					tmpfoam = foamNewCast(fmType, tmpfoam);
					break;
				   default :
					break;
				}
				break;
			}


			/* Invoke the setter to update the reference */
			rhs       = gen0CCallFrFoam(fmType, tmpset, 1, &argloc);
			argloc[0] = foamCopy(tmpfoam);
			tmpfoam   = gen0ApplyReturn(tmpab, (Syme)NULL, tfi, rhs);
			aftCall   = listCons(Foam)(tmpfoam, aftCall);
		}
	}


	/* Emit the code to be executed before the function call */
	befCall = listNReverse(Foam)(befCall);
	while (befCall)
	{
		gen0AddStmt(car(befCall), NULL);
		befCall = listFreeCons(Foam)(befCall);
	}


	/* Generate the Fortran call */
	if (numresults && valueMode)
	{
		/*
		 * Bool, Boolean, SingleFloat, DoubleFloat,
		 * Complex(SingleFloat) and Complex(DoubleFloat)
		 * are special cases.
		 */
		tfret   = tfMapRet(tf);
		fmType	= gen0Type(tfret, NULL);
		ftnType = ftnTypeFrDomTForm(tfret);

		switch (ftnType)
		{
		   case FTN_FSingle:
			/*
			 * Store the SFlo result in a newly created
			 * SingleFloat. Add the cast and return.
			 */
			call->foamPCall.type = FOAM_SFlo;
			tmpvar  = gen0TempLocal(FOAM_Word);
			tmpfoam = foamNewCast(FOAM_Word, foamCopy(foam));
			tmpfoam = foamNewSet(foamCopy(tmpvar), tmpfoam);
			gen0AddStmt(tmpfoam, NULL);
			res = foamCopy(tmpvar);
			break;
		   case FTN_FDouble:
			/*
			 * Store DFlo result in a newly created
			 * DoubleFloat. Add appropriate casts and
			 * return the result.
			 */
			call->foamPCall.type = FOAM_DFlo;
			tmpvar  = gen0TempLocal(FOAM_Word);
			tmpfoam = gen0MakeDouble(foamCopy(foam));
			tmpfoam = foamNewSet(foamCopy(tmpvar), tmpfoam);
			gen0AddStmt(tmpfoam, NULL);
			res = foamCopy(tmpvar);
			break;
		   case FTN_Boolean:
			/*
			 * Store the Word result in a newly created
			 * Boolean. Add the cast and return.
			 */
			call->foamPCall.type = FOAM_Word;
			tmpvar  = gen0TempLocal(FOAM_Word);
			tmpfoam = foamNewSet(foamCopy(tmpvar), foamCopy(foam));
			gen0AddStmt(tmpfoam, NULL);
			res = foamCopy(tmpvar);
			break;
		   case FTN_FSComplex:
			/*
			 * Convert COMPLEX REAL into Complex SF. To do
			 * this we store the result of the Fortran call
			 * in a local. Then we copy and convert the two
			 * components into a new local which is used as
			 * the return value for the whole call.
			 */
			cfmt    = gen0SingleCpxFormat(); /* COMPLEX REAL */
			afmt    = gen0AldorCpxFormat();  /* Complex SF */

			/* Create a local and store the return value */
			tmpvar  = gen0TempLocal0(FOAM_Rec, cfmt);
			tmpfoam = foamNewSet(foamCopy(tmpvar), foamCopy(foam));
			gen0AddStmt(tmpfoam, NULL);

			/* Allocate storage for the Complex SF result */
			res     = gen0TempLocal0(FOAM_Rec, afmt);
			tmpfoam = gen0RNew(res, afmt);
			gen0AddStmt(tmpfoam, NULL);

			/* Copy the real part into the local (with cast) */
			creal = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)0);
			creal = foamNewCast(FOAM_Word, creal);
			creal = gen0RSet(foamCopy(res), afmt, (AInt)0, creal);
			gen0AddStmt(creal, NULL);

			/* Copy the imaginary part into the local */
			cimag = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)1);
			cimag = foamNewCast(FOAM_Word, cimag);
			cimag = gen0RSet(foamCopy(res), afmt, (AInt)1, cimag);
			gen0AddStmt(cimag, NULL);
			break;
		   case FTN_FDComplex:
			/*
			 * Convert COMPLEX DOUBLE into Complex DF. To do
			 * this we store the result of the Fortran call
			 * in a local. Then we copy and convert the two
			 * components into a new local which is used as
			 * the return value for the whole call.
			 */
			cfmt    = gen0DoubleCpxFormat();  /* COMPLEX DOUBLE */
			afmt    = gen0AldorCpxFormat();   /* Complex DF */
			dfmt    = gen0MakeDoubleFormat(); /* DF */

			/* Create a local and store the return value */
			tmpvar  = gen0TempLocal0(FOAM_Rec, cfmt);
			tmpfoam = foamNewSet(foamCopy(tmpvar), foamCopy(foam));
			gen0AddStmt(tmpfoam, NULL);


			/* Create a DF for the real part */
			rpart = gen0TempLocal0(FOAM_Rec, dfmt);
			gen0AddStmt(gen0RNew(foamCopy(rpart), dfmt), NULL);


			/* Copy the real DFlo part into the DF */
			creal = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)0);
			creal = gen0RSet(foamCopy(rpart), dfmt, (AInt)0, creal);
			gen0AddStmt(creal, NULL);


			/* Create a DF for the imaginary part */
			ipart   = gen0TempLocal0(FOAM_Rec, dfmt);
			gen0AddStmt(gen0RNew(foamCopy(ipart), dfmt), NULL);


			/* Copy the imaginary part into the DF */
			cimag = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)1);
			cimag = gen0RSet(foamCopy(ipart), dfmt, (AInt)0, cimag);
			gen0AddStmt(cimag, NULL);


			/* Create the Complex DF */
			res = gen0TempLocal0(FOAM_Rec, afmt);
			gen0AddStmt(gen0RNew(res, afmt), NULL);


			/* Fill in the slots of the Complex DF */
			creal = foamCopy(rpart);
			cimag = foamCopy(ipart);
			creal = gen0RSet(foamCopy(res), afmt, (AInt)0, creal);
			cimag = gen0RSet(foamCopy(res), afmt, (AInt)1, cimag);
			gen0AddStmt(creal, NULL);
			gen0AddStmt(cimag, NULL);
			break;
		   default:
			if (fmType == FOAM_Bool)
			{
				/* f95 uses words not bytes */
				call->foamPCall.type = FOAM_Word;
				tmpvar  = gen0TempLocal(FOAM_Word);
			}
			else
				tmpvar  = gen0TempLocal(gen0Type(tfret, NULL));

			tmpfoam = foamNewSet(foamCopy(tmpvar), foamCopy(foam));
			gen0AddStmt(tmpfoam, NULL);
			res = foamCopy(tmpvar);
			break;
		}
	}
	else
	{
		gen0AddStmt(foam, NULL);
		res = NULL;
	}


	/* Emit the code to be executed after the function call */
	aftCall = listNReverse(Foam)(aftCall);
	while (aftCall)
	{
		gen0AddStmt(car(aftCall), NULL);
		aftCall = listFreeCons(Foam)(aftCall);
	}


	return res;
}


/*
 * Parameters of functions imported from Fortran which
 * are themselves functions must be wrapped up so that
 * the Fortran arguments can be converted into a function
 * which Fortran is capable of invoking. The actual
 * exported function will be created at during the C
 * generation phase but we can deal with reading and
 * writing argument values during foam generation. We
 * reuse the export-to-Fortran code.
 */
local Foam
gen0FtnFunValue(Foam foam, TForm tf, Syme syme)
{
	Foam		wrapper;
	FoamTag		rtype;
	TForm		tfret = tfMapRet(tf);
	FortranType	ftntype = ftnTypeFrDomTForm(tfret);
	String		opname = symString(symeId(syme));

	if (ftntype)
		rtype  = gen0FtnMachineType(ftntype);
	else
		rtype  = gen0Type(tfret, NULL);

	wrapper = gen0FortranExportFn(tf, rtype, foam, opname, NULL);

   	return foamCopy(wrapper);
}


#if 0
/* Apply an export found via a category */
local Foam
gen0MakeAutoApply(AbSyn ab, Syme op, TForm tf, FoamTag rtype,
	AInt argc, Foam *arg)
{
	AInt		i;
	SymeList	symes;
	Foam		call, *argv;
	TForm		optf, tfret;


	/* Convert this export into an import from tf */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Get its full type and follow any substitutions (again) */
	optf = symeType(op);
	tfFollow(optf);


	/* What is the return type for this call? */
	tfret = tfMapRet(optf);


	/* Apply this export to the array value */
	call  = gen0ExtendSyme(op);
	call  = gen0CCallFrFoam(rtype, call, argc, &argv);


	/* Fill in the arguments */
	for (i = 0;i < argc;i++)
		argv[i] = foamCopy(arg[i]);


	/* Generate the call */
	call  = gen0ApplyReturn(ab, op, tfret, call);
   	return foamCopy(call);
}
#endif


/*
 * We allow users to pass array-like objects to Fortran.
 * This is achieved by applying the (convert: % -> BArr)
 * operation (from the FortranArray category), to their
 * array-like value. After the call we apply the other
 * FortranArray export: (convert: (%, BArr) -> BSInt)
 * (see gen0FtnUpdateArray() below).
 */
local Foam
gen0FtnArrayValue(AbSyn ab, Foam foam, TForm tf)
{
	Syme		op = (Syme)NULL;
	SymeList	symes;
	TForm		optf, tfret;
	Foam		call, *argv;


	/* Get the exports of the FortranArray category */
	symes = ftnArrayExports();


	/* Locate the export { convert: % -> BArr } */
	for (;!op && symes;symes = cdr(symes))
	{
		Syme	syme = car(symes);
		String	name = symeString(syme);


		/* Does this export have the correct name? */
		if (!strEqual(name, "convert")) continue;


		/* Get its full type and follow any substitutions */
		optf = symeType(syme);
		tfFollow(optf);


		/*
		 * Type checking: must be a map with one argument
		 * of type % and a return type Arr$Machine. This
		 * isn't the correct way to do this (we ought to
		 * be using tfSat) so we don't check return types.
		 */
		if (!tfIsAnyMap(optf)) continue;
		if (tfMapArgc(optf) != 1) continue;
		if (!tfIsSelf(tfMapArgN(optf,(Length) 0))) continue;


		/* Found it (assume type is correct) */
		op = syme;
	}


	/* Did we find the required category export? */
	if (!op) return foamCopy(foam);

/* return gen0MakeAutoApply(ab, op, tf, FOAM_Arr, 1, &foam); */


	/* Convert this export into an import from tf */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Get its full type and follow any substitutions (again) */
	optf = symeType(op);
	tfFollow(optf);


	/* What is the return type for this call? */
	tfret = tfMapRet(optf);


	/* Apply this export to the array value */
	call  = gen0ExtendSyme(op);
	call  = gen0CCallFrFoam(FOAM_Arr, call, 1, &argv);
	*argv = foamCopy(foam);
	call  = gen0ApplyReturn(ab, op, tfret, call);
   	return foamCopy(call);
}


local Foam
gen0FtnUpdateArray(AbSyn ab, Foam orig, Foam flat, TForm tf)
{
	Syme		op = (Syme)NULL;
	SymeList	symes;
	TForm		optf, tfret;
	Foam		call, *argv;


	/* Get the exports of the FortranArray category */
	symes = ftnArrayExports();


	/* Locate the export { convert: (%, BArr) -> BSInt } */
	for (;!op && symes;symes = cdr(symes))
	{
		Syme	syme = car(symes);
		String	name = symeString(syme);


		/* Does this export have the correct name? */
		if (!strEqual(name, "convert")) continue;


		/* Get its full type and follow any substitutions */
		optf = symeType(syme);
		tfFollow(optf);


		/*
		 * Type checking: must be a map with two arguments of
		 * type % and Arr$Machine. As before we ought to be using
		 * tfSat but since we are not we skip the check that the
		 * second argument is Arr$Machine and the result is of
		 * type SInt$Machine.
		 */
		if (!tfIsAnyMap(optf)) continue;
		if (tfMapArgc(optf) != 2) continue;
		if (!tfIsSelf(tfMapArgN(optf, (Length) 0))) continue;
		/* Check return type is SInt ... */


		/* Found it (assume type is correct) */
		op = syme;
	}


	/* Did we find the required category export? */
	if (!op) return foamNewNOp();

/*
 * args[0] = orig;
 * args[1] = flat;
 * return gen0MakeAutoApply(op, tf, FOAM_SInt, 2, &args);
 */


	/* Convert this export into an import from tf */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Get its full type and follow any substitutions (again) */
	optf = symeType(op);
	tfFollow(optf);


	/* What is the return type for this call? */
	tfret = tfMapRet(optf);


	/* Apply this export to the array value */
	call    = gen0ExtendSyme(op);
	call    = gen0CCallFrFoam(FOAM_SInt, call, 2, &argv);
	argv[0] = foamCopy(orig);
	argv[1] = foamCopy(flat);
	call    = gen0ApplyReturn(ab, op, tfret, call);
   	return foamCopy(call);
}


/*
 * Fixed-string arrays are strange beasts. We want to
 * auto-convert them just like any other array but we
 * also have to note how long the string elements are.
 * We return a pair containing the raw array and the
 * length of the string elements (see gen0FtnArrayValue()
 * above for details of the automatic array conversion).
 *
 * Note: FortranStringArray includes FortranArray.
 */
local Foam
gen0FtnFSArrayValue(AbSyn ab, Foam foam, TForm tf)
{
	Foam		arr, len;


	/* Convert the array */
	arr = gen0FtnArrayValue(ab, foam, tf);


	/* Now find and apply the #: () -> SInt export. */
	len = gen0FtnFSArrayLen(ab, foam, tf);


	/* Return the pair */
   	return foamNew(FOAM_Values, 2, foamCopy(arr), foamCopy(len));
}


local Foam
gen0FtnUpdateFSArray(AbSyn ab, Foam orig, Foam flat, TForm tf)
{
	/* The flat array is in a FOAM_Values */
	assert(foamTag(flat) == FOAM_Values);

	return gen0FtnUpdateArray(ab, orig, flat->foamValues.argv[0], tf);
}


/*
 * We need to be able to determine the length of fixed
 * string values stored in arrays. This is achieved by
 * applying the (#: () -> BSInt) operation (from the
 * FortranFSArray category), to the * array-like value.
 */
local Foam
gen0FtnFSArrayLen(AbSyn ab, Foam foam, TForm tf)
{
	Syme		op = (Syme)NULL;
	SymeList	symes;
	TForm		optf, tfret;
	Foam		call, *argv;


	/* Get the exports of the FortranFSArray category */
	symes = ftnFSArrayExports();


	/* Locate the export { #: () -> BSInt } */
	for (;!op && symes;symes = cdr(symes))
	{
		Syme	syme = car(symes);
		String	name = symeString(syme);


		/* Does this export have the correct name? */
		if (!strEqual(name, "#")) continue;


		/* Get its full type and follow any substitutions */
		optf = symeType(syme);
		tfFollow(optf);


		/*
		 * Type checking: must be a map with no arguments
		 * and a return type SInt$Machine. This isn't the
		 * correct way to do this (we ought to be using
		 * tfSat) so we don't check return types.
		 */
		if (!tfIsAnyMap(optf)) continue;
		if (tfMapArgc(optf)) continue;


		/* Found it (assume type is correct) */
		op = syme;
	}


	/* Did we find the required category export? */
	/* !!! Ought to generate a compiler error !!!!! */
	if (!op) return foamCopy(foam);

/* return gen0MakeAutoApply(ab, op, tf, FOAM_SInt, 0, (Foam*)NULL); */


	/* Convert this export into an import from tf */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Get its full type and follow any substitutions (again) */
	optf = symeType(op);
	tfFollow(optf);


	/* What is the return type for this call? */
	tfret = tfMapRet(optf);


	/* Apply this export to the array value */
	call  = gen0ExtendSyme(op);
	call  = gen0CCallFrFoam(FOAM_SInt, call, (Length)0, &argv);
	call  = gen0ApplyReturn(ab, op, tfret, call);
   	return foamCopy(call);
}


/*
 * Given a machine-type, return the FOAM for a value of
 * this type stored in a record. We update the final
 * argument with the FOAM statements required to pack
 * the value in the record.
 */
Foam
gen0MakePointerTo(FoamTag tag, Foam foam, FoamList *ltmp)
{
	FoamList	lst;
	Foam		tmpvar;
	Foam		tmpfoam = foamCopy(foam);

	switch (tag)
	{
	   case FOAM_Char:
		return gen0MakeCharRecValue(tmpfoam, ltmp);
	   case FOAM_SInt:
		return gen0MakeIntRecValue(tmpfoam, ltmp);
	   case FOAM_SFlo:
		return gen0MakeFloatRecValue(tmpfoam, ltmp);
	   case FOAM_DFlo:
		return gen0MakeDoubleCode(tmpfoam, ltmp);
	   default:
		/*
		 * Assume it is already a pointer and
		 * store it in a temporary variable.
		 */
		tmpvar  = gen0TempLocal(tag);
		tmpfoam = foamNewSet(tmpvar, tmpfoam);
		lst     = listNil(Foam);
		lst     = listCons(Foam)(tmpfoam, lst);
		*ltmp   = lst;
		return foamCopy(tmpvar);
	}
}

/*
 * This function simply reverses the job done
 * by gen0MakePointerTo(). Given a the type
 * of the value being pointed to by foam we
 * pull it out and return it to the caller.
 */
Foam
gen0ReadPointerTo(FoamTag tag, Foam foam)
{
	Foam tmpfoam = foamCopy(foam);

	switch (tag)
	{
	   case FOAM_Char:
		return gen0ReadCharRecValue(tmpfoam);
	   case FOAM_SInt:
		return gen0ReadIntRecValue(tmpfoam);
	   case FOAM_SFlo:
		return gen0ReadFloatRecValue(tmpfoam);
	   case FOAM_DFlo:
		return gen0DoubleValue(tmpfoam);;
	   default:
		return tmpfoam;
	}
}

/*
 * This function is the opposite to gen0ReadPointerTo().
 * Given a the type of the value being pointed to by
 * dst we stuff the value stored in foam into it.
 */
Foam
gen0WritePointerTo(FoamTag tag, Foam dst, Foam foam)
{
	Foam tmpfoam = foamCopy(foam);

	switch (tag)
	{
	   case FOAM_Char:
		return gen0WriteCharRecValue(dst, tmpfoam);
	   case FOAM_SInt:
		return gen0WriteIntRecValue(dst, tmpfoam);
	   case FOAM_SFlo:
		return gen0WriteFloatRecValue(dst, tmpfoam);
	   case FOAM_DFlo:
		return gen0WriteDoubleValue(dst, tmpfoam);;
	   default:
		return dst;
	}
}


#if 0
/* Invoked from gen0FtnSCpxGet() and gen0FtnDCpxGet(). */
local Foam
gen0FtnComplexGet(AbSyn ab, Foam foam, TForm tf, SymeList symes)
{
	Syme op = (Syme)NULL;


	/*
	 * Locate export { coerce: % -> Record(real:R, imag:R) }
	 * where R is SFlo or DFlo.
	 */
	for (;!op && symes;symes = cdr(symes))
	{
		TForm	optf;
		Syme	syme = car(symes);
		String	name = symeString(syme);


		/* Does this export have the correct name? */
		if (!strEqual(name, "coerce")) continue;


		/* Get its full type and follow any substitutions */
		optf = symeType(syme);
		tfFollow(optf);


		/*
		 * Type checking: must be a map with one argument
		 * of type % and a return type Record(). This isn't
		 * the correct way to do this (we ought to be using
		 * tfSat) so we don't check return types.
		 */
		if (!tfIsAnyMap(optf)) continue;
		if (tfMapArgc(optf) != 1) continue;
		if (!tfIsSelf(tfMapArgN(optf, (Length) 0))) continue;


		/* Found it (assume type is correct) */
		op = syme;
	}


	/* Did we find the required category export? */
	if (!op) return foamCopy(foam);


	/* Apply this operation */
	return gen0MakeAutoApply(ab, op, tf, FOAM_Rec, 1, &foam);
}


/* Invoked from gen0FtnSCpxPut() and gen0FtnDCpxPut(). */
local Foam
gen0FtnComplexPut(AbSyn ab, Foam foam, TForm tf, SymeList symes)
{
	Syme op = (Syme)NULL;


	/*
	 * Locate export { coerce: Record(real:R, imag:R) -> % }
	 * where R is SFlo or DFlo.
	 */
	for (;!op && symes;symes = cdr(symes))
	{
		TForm	optf;
		Syme	syme = car(symes);
		String	name = symeString(syme);


		/* Does this export have the correct name? */
		if (!strEqual(name, "coerce")) continue;


		/* Get its full type and follow any substitutions */
		optf = symeType(syme);
		tfFollow(optf);


		/*
		 * Type checking: must be a map with one argument
		 * of type Record() and a return type %. This isn't
		 * the correct way to do this (we ought to be using
		 * tfSat) so we don't check argument types.
		 */
		if (!tfIsAnyMap(optf)) continue;
		if (tfMapArgc(optf) != 1) continue;
		if (!tfIsSelf(tfMapRet(optf))) continue;


		/* Found it (assume type is correct) */
		op = syme;
	}


	/* Did we find the required category export? */
	if (!op) return foamCopy(foam);


	/* Apply this operation */
	return gen0MakeAutoApply(ab, op, tf, FOAM_Word, 1, &foam);
}
#endif


/*****************************************************************************
 *
 * :: Local functions for the Fortran-calls-Aldor side.
 *
 ****************************************************************************/

local AbSyn        ab0FortranExportArgs   (TForm);
local AbSyn        ab1FortranExportArg    (Length);
local FoamList     gen0FortranExportArgs  (TForm, FoamList *, FoamList *);
local Foam         gen1FortranExportArg   (TForm, Length, 
						FoamList *, FoamList *);
local Foam         gen0FortranConvArg     (Foam, FoamTag, FortranType,
						FoamList *);
local Foam         gen0FortranPackArg     (Foam, FoamTag, FortranType,
						FoamTag *);
local Symbol       gen0FortranArgName     (Length);


/*****************************************************************************
 *
 * Foam code generation for Fortran-calls-Aldor.
 *
 ****************************************************************************/

/*
 * Create a global variable binding for exporting to Fortran.
 */
void
gen0ExportToFortran(AbSyn absyn)
{
	TForm		tf, tfret;
	String		str;
	FoamTag		rtype;
	FortranType	ftntype;
	Foam		decl, wrapper, glo;
	AInt		fmtslot, index;
	AbSyn		name = abDefineeId(absyn);
	Syme		syme = abSyme(name);

	assert(syme);
	tf = symeType(syme);
	assert (tfIsMap(tf));

	/*!! Assumes export to Fortran is exporting a function! */
	/*
	 * Now create the wrapper function. This is stored in a
	 * global so that it is visible to Fortran. The wrapper
	 * prepares the arguments for the real exported function,
	 * notes the return value and updates the arguments passed
	 * to us from Fortran (if appropriate).
	 */
	tfret	= tfMapRet(tf);
	ftntype = ftnTypeFrDomTForm(tfret);
	if (ftntype)
		rtype  = gen0FtnMachineType(ftntype);
	else
		rtype  = gen0Type(tfret, NULL);

	fmtslot = gen0FortranSigExportNumber(tf);
	str     = strCopy(symeString(syme));
	wrapper = gen0FortranExportFn(tf, rtype, gen0Syme(syme), str, absyn);
	decl    = foamNewGDecl(FOAM_Clos, str, fmtslot,
			    FOAM_GDecl_Export, FOAM_Proto_Fortran);
	foamGDeclSetRType(decl, rtype);
        index   = gen0AddGlobal(decl);
	glo     = foamNewGlo(index);
	gen0AddStmt(foamNewSet(glo, wrapper), absyn);


	/* Note the global index of the exported wrapper */
	gen0BuiltinExports = listCons(AInt)(index, gen0BuiltinExports);
	gen0BuiltinExports = listCons(AInt)(int0, gen0BuiltinExports);
}


/*
 * Create a wrapper function in FOAM so that Fortran can invoke
 * our exported Aldor function and allow its arguments to be
 * updated by Aldor (where appropriate). We really want to do as
 * much of this as possible in FOAM rather than leaving it to the
 * C generation phase where life is much more messy. It also
 * gives the optimiser a chance to do some work.
 *
 * Ideally our wrapper function foo'() for the exported function
 * foo() would have a signature using pointers to machine types.
 * The wrapper would unpack the Fortran arguments and create ref()
 * values for any parameter of foo() which has type Ref(T). The
 * Fortran arguments would be updated after the foo() call.
 *
 * Thus if we were exporting:
 *
 * 	foo: (SingleInteger, Ref(DoubleFloat)) -> Boolean
 *
 * then our wrapper would be:
 *
 * 	foo'(t1:Record(sint:SInt), t2:Record(dflo:DFlo)):Bool ==
 * 	{
 * 	   local l1:SingleInteger    := (t1.sint)::SingleInteger;
 * 	   local l2:DoubleFloat      := (t2.dflo)::DoubleFloat;
 * 	   local l3:Ref(DoubleFloat) := ref(l2);
 * 	   local result:Boolean      := foo(l1, l3);
 *
 *	   t2.dflo := deref(l3)::DFlo; -- Confuse the optimiser
 * 	   result;
 * 	}
 *
 * Note that the obvious optimisation here is to change the
 * deref() line to read:
 *
 *	   t2.dflo := l2::DFlo;
 *
 * The trouble is that the optimiser might try to be too
 * clever and assume that the call to foo() will not affect
 * the value of l2. This ought not to happen but I don't
 * think it is worth the risk just yet. At least this code
 * ought to be relatively stable. Anyway, we usually eliminate
 * the reference getter/setter functions during inlining.
 */
Foam
gen0FortranExportFn(TForm tf, FoamTag rtype, Foam op, String name, AbSyn absyn)
{
	TForm		tfret;
	Foam		*argloc;
	Foam		foam, clos, retvar, tmpfoam;
	Foam		cpx, recfoam, creal, cimag;
	FoamTag		retType;
	AInt		retfmt, cfmt, afmt, dfmt;
	AbSyn		params;
	Length		i, nargs;
	FoamList	bef, aft, pars;
	FortranType	ftnType;
	GenFoamState	saved;


	/* Note the function signature */
	tfret   = tfMapRet(tf);
	retType = gen0Type(tfret, &retfmt);
	ftnType = ftnTypeFrDomTForm(tfret);


	/* Create a closure for the function */
	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(name, absyn);


	/* Save the current state */
	saved = gen0ProgSaveState(PT_ExFn);


	/*
	 * Deal with special return types. Since we
	 * are exporting to Fortran these sort of
	 * things ought never to appear. However, it
	 * doesn't hurt to be careful.
	 */
	if (!tfIsNone(tfret) && tfIsMulti(tfret))
		retfmt = gen0MultiFormatNumber(tfret);

	if (tfIsGenerator(tfret))
		foamProgSetGenerator(foam);


	/* Create the parameters for this function */
	params = ab0FortranExportArgs(tfMapArg(tf));


	/* Initialise the program state */
	gen0State->type    = tf;
	gen0State->param   = params;
	gen0State->program = foam;


	/* Create the before and after wrapper code */
	bef    = listNil(Foam);
	aft    = listNil(Foam);
	pars   = gen0FortranExportArgs(tfMapArg(tf), &bef, &aft);


	/*
	 * Add the code to be executed before the exported
	 * function is invoked.
	 */
	bef = listNReverse(Foam)(bef);
	while (bef)
	{
		gen0AddStmt(car(bef), absyn);
		bef = listFreeCons(Foam)(bef);
	}


	/* Generate code to apply the exported function */
	nargs   = listLength(Foam)(pars);
	tmpfoam = gen0CCallFrFoam(retType, op, nargs, &argloc);


	/* Create the FOAM for the procedure arguments */
	for (i = 0;i < nargs;pars = listFreeCons(Foam)(pars), i++)
		argloc[i] = foamCopy(car(pars));
	tmpfoam = gen0ApplyReturn(absyn, (Syme)NULL, tfret, tmpfoam);


	/* Store the function result (if any) in a temp */
	if (!tfIsNone(tfret))
	{
		retvar = gen0TempLocal0(retType, retfmt);
		tmpfoam = foamNewSet(retvar, foamCopy(tmpfoam));
	}
#if EDIT_1_0_n1_07
	else
		retvar = 0;
#endif

	/* Add the function call statement */
	gen0AddStmt(tmpfoam, absyn);


	/*
	 * Add the code to be executed after the exported
	 * function is invoked.
	 */
	aft = listNReverse(Foam)(aft);
	while (aft)
	{
		gen0AddStmt(car(aft), absyn);
		aft = listFreeCons(Foam)(aft);
	}


	/*
	 * Convert the return value into the right type and
	 * return it (if there is one).
	 */
	if (!tfIsNone(tfret))
	{
		FoamTag *junk = (FoamTag *)NULL;
		tmpfoam = gen0FortranPackArg(retvar, rtype, ftnType, junk);


		/* Complex numbers need special treatment */
		switch (ftnType)
		{
		   case FTN_FSComplex:
			/* Convert Complex SF into COMPLEX REAL */
			cfmt    = gen0SingleCpxFormat(); /* COMPLEX REAL */
			afmt    = gen0AldorCpxFormat();  /* Complex SF */
			cpx     = gen0TempLocal0(FOAM_Rec, cfmt);

			/* Allocate storage for the COMPLEX REAL */
			recfoam = gen0RNew(cpx, cfmt);
			gen0AddStmt(recfoam, absyn);

			/* Copy the real part of the Complex SF */
			creal = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)0);
			creal = foamNewCast(FOAM_SFlo, creal);
			creal = gen0RSet(foamCopy(cpx), cfmt, (AInt)0, creal);
			gen0AddStmt(creal, absyn);

			/* Copy the imaginary part */
			cimag = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)1);
			cimag = foamNewCast(FOAM_SFlo, cimag);
			cimag = gen0RSet(foamCopy(cpx), cfmt, (AInt)1, cimag);
			gen0AddStmt(cimag, absyn);

			/* Now use the Fortran-format local */
			tmpfoam = foamCopy(cpx);
			break;
		   case FTN_FDComplex:
			/* Convert Complex DF into COMPLEX DOUBLE */
			cfmt    = gen0DoubleCpxFormat();  /* COMPLEX DOUBLE */
			afmt    = gen0AldorCpxFormat();   /* Complex DF */
			dfmt    = gen0MakeDoubleFormat(); /* DF */
			cpx     = gen0TempLocal0(FOAM_Rec, cfmt);

			/* Allocate storage for the COMPLEX DOUBLE */
			recfoam = gen0RNew(cpx, cfmt);
			gen0AddStmt(recfoam, absyn);

			/* Copy the real DFlo part of the Complex DF */
			creal = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)0);
			creal = foamNewRElt(dfmt, creal, (AInt)0);
			creal = gen0RSet(foamCopy(cpx), cfmt, (AInt)0, creal);
			gen0AddStmt(creal, absyn);

			/* Copy the imaginary part */
			cimag = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)1);
			cimag = foamNewRElt(dfmt, cimag, (AInt)0);
			cimag = gen0RSet(foamCopy(cpx), cfmt, (AInt)1, cimag);
			gen0AddStmt(cimag, absyn);

			/* Now use the Fortran-format local */
			tmpfoam = foamCopy(cpx);
			break;
		   default:
			break;
		}


		/* Return the value */
		tmpfoam = foamNewReturn(foamCopy(tmpfoam));
		gen0AddStmt(tmpfoam, absyn);
	}
	else
	{
		/* Return nothing */
		tmpfoam = foamNewReturn(foamNew(FOAM_Values, (AInt)0));
		gen0AddStmt(tmpfoam, absyn);
	}


	/*
	 * Finish off the FOAM creation. Note that we want to
	 * use a basic machine type for the return type of this
	 * function so that Fortran can understand the result.
	 * This means we use `rtype' in gen0ProgFiniEmpty()
	 * rather than `retType' which we would do normally.
	 */
 	gen0UseStackedFormat((AInt)0); /* These two lines provide a format */
 	gen0ProgPushFormat((AInt)0);   /* for the lexical argument `op'   */
	gen0ProgFiniEmpty(foam, rtype, emptyFormatSlot);


	/* We are down one lexical level */
	gen0AddLexLevels(foam, 1);


	/* Optimisation bits */
	/* foam->foamProg.infoBits = IB_INLINEME; */
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);
	foam->foamProg.format = retfmt;


	/* Restore the saved state before returning */
	gen0ProgRestoreState(saved);
	return clos;
}


/*
 * Invent some absyn for the parameter list of a function.
 */
local AbSyn
ab0FortranExportArgs(TForm tf)
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
			AbSyn arg = ab1FortranExportArg(i);
			lst = listCons(AbSyn)(arg, lst);
		}


		/* Make sure that the list is in the right order */
		lst = listNReverse(AbSyn)(lst);


		/* Return the absyn for the parameter list */
		return abNewCommaL(sposNone, lst);
	}
	else
		return ab1FortranExportArg((Length)0);
}

local AbSyn
ab1FortranExportArg(Length i)
{
	/* Create absyn for single argument */
	Symbol	sym;
	AbSyn	type, param;


	/* Construct a name for this parameter. */
	sym = gen0FortranArgName(i);


	/* Create the absyn for the name and type */
	param = abNewId(sposNone, sym);
	type  = abNewId(sposNone, symIntern("Word"));


	/* Return the parameter declaration */
	return abNewDeclare(sposNone, param, type);
}


/*
 * Construct before and after code for each parameter.
 */
local FoamList
gen0FortranExportArgs(TForm tf, FoamList *bef, FoamList *aft)
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

			par = gen1FortranExportArg(t, i, bef, aft);
			lst = listCons(Foam)(par, lst);
		}
	}
	else
	{
		/* A single argument */
		par = gen1FortranExportArg(tf, (Length) 0, bef, aft);
		lst = listCons(Foam)(par, lst);
	}


	/* Reverse the list and return it */
	lst = listNReverse(Foam)(lst);
	return lst;
}

local Foam
gen1FortranExportArg(TForm tforig, Length i,
                     FoamList *befLst, FoamList *aftLst)
{
	FoamTag		fmtype, argtype, lextype, realType;
	FortranType	ftnType;
	Foam		decl, tmpvar, fmparam, tmpfoam, refvar;
	Foam		cpx, creal, cimag;
	FoamList	bef, aft;
	Symbol		sym;
	String		symstr;
	Bool		isRef = false;
	AInt		fmt;
	TForm		tf;


	/* Note the "before" and "after" code */
	bef = *befLst;
	aft = *aftLst;


	/* What was the name of this parameter? */
	sym    = gen0FortranArgName(i);
	symstr = strCopy(symString(sym));


	/* Note if passed by reference and map Ref(T) to T */
	if (tfIsReference(tforig))
	{
		isRef = true;
		tf    = tfReferenceArg(tforig);
	}
	else
		tf = tforig;


	/* What is the type of this argument? */
	ftnType = ftnTypeFrDomTForm(tf);
	if (ftnType)
		fmtype  = gen0FtnMachineType(ftnType);
	else
		fmtype  = gen0Type(tf, NULL);


	/* We only care about a few argument types */
	argtype = (fmtype == FOAM_Clos) ? FOAM_Clos : FOAM_Word;


	/* Create a declaration for this parameter */
	decl = foamNewDecl(argtype, symstr, emptyFormatSlot);


	/* Add the new parameter to the FOAM prog */
	gen0AddParam(decl);


	/*
	 * Add to the "before" code: this is simply unpacking
	 * the value from the Fortran pointer and storing it
	 * locally as an Aldor value. If we are passing values
	 * by reference then we use a lexical otherwise a local.
	 */
	fmparam = foamNewPar(i);
	lextype = gen0Type(tf, NULL);
	tmpvar  = isRef ? gen0TempLex(lextype) : gen0TempLocal(lextype);
	tmpfoam = gen0FortranConvArg(fmparam, fmtype, ftnType, &bef);
	tmpfoam = foamNewSet(tmpvar, tmpfoam);
	bef     = listCons(Foam)(foamCopy(tmpfoam), bef);


	/*
	 * If we are passing this parameter by reference
	 * then wrap it up in a Reference(T) value and
	 * pass that instead. Then after the call we need
	 * to unpack the reference and update the Fortran
	 * argument.
	 */
	if (isRef)
	{
		FoamTag	fmret;
		AbSyn	tmpab;
		Foam	*argloc;
		AInt	cfmt, afmt, dfmt;
		Foam	tmpget, retfoam, stmt;
		TForm	tfiget, tfiset, tfret;
		Syme	nsyme = (Syme)NULL;


		/* Construct the reference to the local */
		refvar  = gen0TempLocal(gen0Type(tforig, &fmt));
		tmpab   = abNewId(sposNone, sym);
		tmpfoam = genReferenceFrFoam(tmpvar, tf, tmpab);
		tmpfoam = foamNewSet(refvar, tmpfoam);
		bef     = listCons(Foam)(foamCopy(tmpfoam), bef);


		/*
		 * Dereference refvar after the call - see the
		 * comments in gen0ModifyFortranCall() for more
		 * details on how this bit works. Basically a
		 * reference is stored as a pair of functions,
		 * the getter and the setter:
		 *
		 *    ref: () -> Cross(() -> T, T -> T);
		 *
		 * Start by computing function return types.
		 */
		tfiget = tfMap(tfNone(), tf);
		tfiset = tfMap(tf, tf);
		tfret  = tfMulti(2, tfiget, tfiset);
		fmret  = gen0Type(tfret, NULL);


		/*
		 * Extract the getter/setter pair. This is slightly
		 * complicated by the fact that we have a multiple
		 * return value. The gen0ApplyReturn() function will
		 * invoke gen0AddStmt() in this situation so we can't
		 * use it here. Instead we use a similar function
		 * which returns some FOAM to be added to our list
		 * of things to-do.
		 */
		retfoam = foamCopy(refvar);
		retfoam = gen0CCallFrFoam(fmret, retfoam,(Length) 0, &argloc);
		retfoam = gen1ApplyReturn(tmpab, nsyme, tfret, retfoam, &stmt);
		aft     = listCons(Foam)(foamCopy(stmt), aft);


		/* Extract the getter */
		tmpfoam = foamCopy(retfoam);
		assert(foamTag(tmpfoam) == FOAM_Values);
		tmpget  = tmpfoam->foamValues.argv[0];


		/* Apply the getter to obtain the updated Aldor value */
		tmpfoam = gen0CCallFrFoam(lextype, tmpget, (Length) 0, &argloc);
		tmpfoam = gen0ApplyReturn(tmpab, (Syme)NULL, tf, tmpfoam);


		/* Convert the Aldor value into a Fortran value */
		tmpfoam = gen0FortranPackArg(tmpfoam,fmtype,ftnType,&realType);


		/* !!! BUG !!!
		 * Note that the lexical that we used for the reference
		 * to the function parameter may now point to somewhere
		 * else in memory particularly if it is a FixedString or
		 * String). Somehow we need to strncpy from the lexical
		 * to the function parameter. Quite how we can do this
		 * is beyond me at the moment.
		 */
		retfoam = foamCopy(fmparam);


		/* 
		 * gen0FortranPackArg and gen0WritePointerTo do not
		 * deal with complex numbers. This means that tmpfoam
		 * is still a Complex R value (where R = SF or DF).
		 */
		if (ftnType == FTN_FSComplex)
		{
			/* Compute the record formats involved */
			cfmt = gen0SingleCpxFormat(); /* COMPLEX REAL */
			afmt = gen0AldorCpxFormat();  /* Complex SF */
			cpx  = retfoam;

			/* Copy the real part of the Complex SF */
			creal = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)0);
			creal = foamNewCast(FOAM_SFlo, creal);
			creal = gen0RSet(foamCopy(cpx), cfmt, (AInt)0, creal);
			aft   = listCons(Foam)(creal, aft);

			/* Copy the imaginary part */
			cimag = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)1);
			cimag = foamNewCast(FOAM_SFlo, cimag);
			cimag = gen0RSet(foamCopy(cpx), cfmt, (AInt)1, cimag);
			aft   = listCons(Foam)(cimag, aft);

			/* Now use the Fortran-format local */
			tmpfoam = foamCopy(cpx);
		}
		else if (ftnType == FTN_FDComplex)
		{
			/* Convert Complex DF into COMPLEX DOUBLE */
			cfmt    = gen0DoubleCpxFormat();  /* COMPLEX DOUBLE */
			afmt    = gen0AldorCpxFormat();   /* Complex DF */
			dfmt    = gen0MakeDoubleFormat(); /* DF */
			cpx     = retfoam;

			/* Copy the real DFlo part of the Complex DF */
			creal = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)0);
			creal = foamNewRElt(dfmt, creal, (AInt)0);
			creal = gen0RSet(foamCopy(cpx), cfmt, (AInt)0, creal);
			aft   = listCons(Foam)(creal, aft);

			/* Copy the imaginary part */
			cimag = foamNewRElt(afmt, foamCopy(tmpfoam), (AInt)1);
			cimag = foamNewRElt(dfmt, cimag, (AInt)0);
			cimag = gen0RSet(foamCopy(cpx), cfmt, (AInt)1, cimag);
			aft   = listCons(Foam)(cimag, aft);

			/* Now use the Fortran-format local */
			tmpfoam = foamCopy(cpx);
		}
		else
		{
			/* Stuff the value in the pointer if possible */
			tmpfoam = gen0WritePointerTo(realType,retfoam,tmpfoam);
			if (tmpfoam != (Foam)NULL)
				aft = listCons(Foam)(foamCopy(tmpfoam), aft);
		}


		/* Use the reference from now-on */
		tmpvar  = refvar;
	}


	/* Update the before and after code */
	*befLst = bef;
	*aftLst = aft;


	/* Return the FOAM for this parameter */
	return foamCopy(tmpvar);
}

/*
 * Read a pointer to a machine type such as DFlo into
 * an Aldor domain such as DoubleFloat. This is used for
 * passing values to-and-from Fortran. If this is a string
 * argument then parameter holds its length.
 */
local Foam
gen0FortranConvArg(Foam foam, FoamTag fmType, FortranType ftnType,
			FoamList *l)
{
	FoamTag		realType;
	FoamList	ltmp, lst;
	AInt		cfmt, afmt, dfmt;
	Foam		tmpfoam, tmpvar;
	Foam		res, creal, cimag, rpart, ipart;


	/* Local copy of the list */
	lst = *l;


	/* Fortran passes LOGICAL values as INTEGERs */
	realType = (fmType == FOAM_Bool) ? FOAM_SInt : fmType;


	/* Unpack the Fortran pointer */
	tmpfoam = gen0ReadPointerTo(realType, foam);


	/* Apply any extra fixes required */
	switch (ftnType)
	{
	   case FTN_Boolean :
		/* Fall through */
	   case FTN_SingleInteger :
		/* Fall through */
	   case FTN_FSingle :
		tmpfoam = foamNewCast(FOAM_Word, tmpfoam);
		break;
	   case FTN_FDouble :
		tmpfoam = gen0MakeDoubleCode(tmpfoam, &ltmp);
		lst = listNConcat(Foam)(ltmp, lst);
		break;
	   case FTN_Character :
		/* Fall through */
	   case FTN_String :
		/* Fall through */
	   case FTN_XLString :
		/*
		 * Refer to the code in genc.c to see why
		 * we don't have to do anything special here.
		 */
		break;
	   case FTN_FSComplex:
		/*
		 * Convert COMPLEX REAL into Complex SF. To
		 * do this we store the Fortran argument in
		 * a local. Then we copy and convert the two
		 * components into a new local for returning.
		 */
		cfmt = gen0SingleCpxFormat(); /* COMPLEX REAL */
		afmt = gen0AldorCpxFormat();  /* Complex SF */

		/* Create a local and store the updated value */
		tmpvar  = gen0TempLocal0(FOAM_Rec, cfmt);
		tmpfoam = foamNewSet(foamCopy(tmpvar), tmpfoam);
		lst     = listCons(Foam)(tmpfoam, lst);

		/* Allocate storage for the Complex SF result */
		res   = gen0TempLocal0(FOAM_Rec, afmt);
		lst   = listCons(Foam)(gen0RNew(foamCopy(res), afmt), lst);

		/* Copy the real part into the local */
		creal = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)0);
		creal = foamNewCast(FOAM_Word, creal);
		creal = gen0RSet(foamCopy(res), afmt, (AInt)0, creal);
		lst   = listCons(Foam)(creal, lst);

		/* Copy the imaginary part into the local */
		cimag = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)1);
		cimag = foamNewCast(FOAM_Word, cimag);
		cimag = gen0RSet(foamCopy(res), afmt, (AInt)1, cimag);
		lst   = listCons(Foam)(cimag, lst);

		/* Use the Complex DF as the result */
		tmpfoam = res;
		break;
	   case FTN_FDComplex:
		/*
		 * Convert COMPLEX DOUBLE into Complex DF. To
		 * do this we store the Fortran argument in
		 * a local. Then we copy and convert the two
		 * components into a new local for returning.
		 */
		cfmt = gen0DoubleCpxFormat();
		afmt = gen0AldorCpxFormat();
		dfmt = gen0MakeDoubleFormat();

		/* Create a local and store the updated value */
		tmpvar  = gen0TempLocal0(FOAM_Rec, cfmt);
		tmpfoam = foamNewSet(foamCopy(tmpvar), tmpfoam);
		lst = listCons(Foam)(tmpfoam, lst);


		/* Create a DF for the real part */
		rpart = gen0TempLocal0(FOAM_Rec, dfmt);
		tmpfoam = gen0RNew(foamCopy(rpart), dfmt);
		lst = listCons(Foam)(tmpfoam, lst);


		/* Copy the real DFlo part into the DF */
		creal = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)0);
		creal = gen0RSet(foamCopy(rpart), dfmt, (AInt)0, creal);
		lst = listCons(Foam)(creal, lst);


		/* Create a DF for the imaginary part */
		ipart = gen0TempLocal0(FOAM_Rec, dfmt);
		tmpfoam = gen0RNew(foamCopy(ipart), dfmt);
		lst = listCons(Foam)(tmpfoam, lst);


		/* Copy the imaginary part into the DF */
		cimag = foamNewRElt(cfmt, foamCopy(tmpvar), (AInt)1);
		cimag = gen0RSet(foamCopy(ipart), dfmt, (AInt)0, cimag);
		lst = listCons(Foam)(cimag, lst);


		/* Create the Complex DF */
		res = gen0TempLocal0(FOAM_Rec, afmt);
		tmpfoam = gen0RNew(foamCopy(res), afmt);
		lst = listCons(Foam)(tmpfoam, lst);


		/* Fill in the slots of the Complex DF */
		creal = gen0RSet(foamCopy(res), afmt, (AInt)0, foamCopy(rpart));
		cimag = gen0RSet(foamCopy(res), afmt, (AInt)1, foamCopy(ipart));
		lst = listCons(Foam)(creal, lst);
		lst = listCons(Foam)(cimag, lst);


		/* Use the Complex DF as the result */
		tmpfoam = res;
		break;
	   default:
		if (fmType == FOAM_Bool)
			tmpfoam = foamNewCast(FOAM_Bool, tmpfoam);
		break;
	}


	*l = lst;
	return tmpfoam;
}

/*
 * Convert a value from an Aldor domain such as DoubleFloat
 * into a machine domain such as DFlo. This is used for passing values
 * to-and-from Fortran. If there is no action to be taken to
 * achieve the conversion then (Foam)NULL is returned.
 */
local Foam
gen0FortranPackArg(Foam value, FoamTag fmType, FortranType ftnType, FoamTag *res)
{
	FoamTag		realType;
	Foam		tmpfoam = foamCopy(value);


	/* Fortran passes LOGICAL values as INTEGERs */
	realType = (fmType == FOAM_Bool) ? FOAM_SInt : fmType;


	/* Apply any extra fixes required */
	switch (ftnType)
	{
	   case FTN_Boolean :       /* Fall through */
	   case FTN_SingleInteger : /* Fall through */
	   case FTN_FSingle :
		tmpfoam = foamNewCast(realType, tmpfoam);
		break;
	   case FTN_FDouble :
		/* Pull out the DFlo */
		tmpfoam = gen0DoubleValue(tmpfoam);
		break;
	   default:
		/* What is the format of the argument pointer? */
		if (fmType == FOAM_Bool)
			tmpfoam = foamNewCast(realType, tmpfoam);
		break;
	}


	/* Tell the caller the real type of this foam */
	if (res) *res = realType;
	return tmpfoam;
}

/*
 * We need to invent names for parameters for the wrapper
 * functions. We do this based on the argument number.
 */
local Symbol
gen0FortranArgName(Length i)
{
	char	num[40];

#if EDIT_1_0_n1_07
	(void)sprintf(num, "%s%d", "x", (int) i);
#else
	(void)sprintf(num, "%s%d", "x", i);
#endif
	return symIntern(num);
}

