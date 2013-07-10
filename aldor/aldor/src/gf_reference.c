/*****************************************************************************
 *
 * gf_reference.c: Foam code generation for reference expressions
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "genfoam.h"
# include "gf_util.h"
# include "gf_prog.h"
# include "gf_rtime.h"
# include "gf_reference.h"
# include "of_inlin.h"
# include "of_util.h"
# include "tinfer.h"
# include "abuse.h"


local Foam gen0RefGetter(Foam, AbSyn, TForm);
local Foam gen0RefSetter(Foam, AbSyn, TForm);
local Foam gen0RefId(AbSyn);

local AInt gen0RefFormat = 0;

#define		gen0RefFormatSize 2
static String	gen0RefFormatNames[] = { "deref", "update!" };
static FoamTag	gen0RefFormatTypes[] = { FOAM_Clos, FOAM_Clos };
static AInt	gen0RefFormatFmts[]  = { emptyFormatSlot, emptyFormatSlot };

#define getterFName()	("refGetter")
#define setterFName()	("refSetter")

#define getterPlace	((AInt)0)
#define setterPlace	((AInt)1)


/*
 * This function has been split into two parts
 * so that we can call it from places such as
 * gen0FortranExportFn().
 */
Foam
genReference(AbSyn ab)
{
	TForm	tf   = tfReferenceArg(gen0AbType(ab));
	AbSyn	rexp = ab->abReference.body;
	Foam	rid  = gen0RefId(rexp);

	return genReferenceFrFoam(rid, tf, rexp);
}


Foam
genReferenceFrFoam(Foam rid, TForm tf, AbSyn ab)
{
	AInt		fmt;
	Foam		getterFn, setterFn;
	Foam		tmpvar;
	GenFoamState	saved;
	Foam		foam, clos;


	/* Create a closure for the function */
	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty("refBaseFn", ab);


	/* Save the current state (WHY?) */
	saved = gen0ProgSaveState(PT_ExFn);


	/* Create the getter/setter functions */
	getterFn = gen0RefGetter(rid, ab, tf);
	setterFn = gen0RefSetter(rid, ab, tf);


	/*
	 * Wrap the getter/setter functions in a multi
	 * which will be returned by a nullary function:
	 *    refBaseFn():Cross(()->T, T->T) == (getter, setter);
	 */
	fmt    = gen0MakeRefFormat();
	tmpvar = foamNew(FOAM_Values, 2, getterFn, setterFn);
	tmpvar = foamNewReturn(tmpvar);


	/* Add this to the current FOAM block */
	gen0AddStmt(tmpvar, ab);


	/* Standard gubbins ... */
	gen0UseStackedFormat(int0);
	gen0ProgPushFormat(int0);
	gen0ProgFiniEmpty(foam, FOAM_NOp, int0);
	foam->foamProg.format = fmt;
	gen0AddLexLevels(foam, 1);


	/* Optimisation bits */
	foam->foamProg.infoBits = IB_SIDE | IB_INLINEME;
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);


	/* Restore the saved state before returning. */
	gen0ProgRestoreState(saved);
	return clos;
}


local Foam
gen0RefGetter(Foam rid, AbSyn id, TForm tf)
{
	/* Create the reference getter function */
	GenFoamState	saved;
	Foam		foam, clos;
	AInt		fmt;
	FoamTag		type;


	/* Create a closure for the function */
	type = gen0Type(tf, &fmt);
	clos = foamNewClos(foamNewEnv(-1), foamNewConst(gen0NumProgs));
	foam = gen0ProgInitEmpty(getterFName(), id);


	/* Save the current state (WHY?) */
	saved = gen0ProgSaveState(PT_ExFn);


	/* Generate the code for { free x:T; return x } */
	gen0AddStmt(foamNewReturn(foamCopy(rid)), id);
	/* gen0AddStmt(foamNewReturn(gen0RefId(id)), id); */


	/* Standard gubbins ... */
	gen0UseStackedFormat(int0);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgPushFormat(emptyFormatSlot); /* Two lexical levels */
	gen0ProgFiniEmpty(foam, type, fmt);
	gen0AddLexLevels(foam, 2);


	/* Optimisation bits */
	foam->foamProg.infoBits = IB_INLINEME;
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);


	/* Restore the saved state before returning. */
	gen0ProgRestoreState(saved);
	return clos;
}


local Foam
gen0RefSetter(Foam rid, AbSyn id, TForm tf)
{
	/* Create the reference setter function */
	GenFoamState	saved;
	Foam		foam, clos;
	AInt		fmt, paridx;
	FoamTag		type;
	Foam		param;


	/* Create a closure for the function */
	type = gen0Type(tf, &fmt);
	clos = foamNewClos(foamNewEnv(-1), foamNewConst(gen0NumProgs));
	foam = gen0ProgInitEmpty(setterFName(), id);


	/* Save the current state and create a new one for us */
	saved = gen0ProgSaveState(PT_ExFn);


	/* Update the state of this lambda */
	gen0State->type    = tfMap(tf, tf); /* T -> T */
	gen0State->program = foam;


	/* Add a parameter */
	param = foamNewDecl(type, strCopy("v"), emptyFormatSlot);
	paridx = gen0AddParam(param);


	/* Generate the code for { free x:T; x := v; return v } */
	/* gen0AddStmt(foamNewSet(gen0RefId(id), foamNewPar(paridx)), id); */
	gen0AddStmt(foamNewSet(foamCopy(rid), foamNewPar(paridx)), id);
	gen0AddStmt(foamNewReturn(foamNewPar(paridx)), id);


	/* Standard gubbins ... */
	gen0UseStackedFormat(int0);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgPushFormat(emptyFormatSlot); /* Two lexical levels */
	gen0ProgFiniEmpty(foam, type, fmt);
	gen0AddLexLevels(foam, 2);


	/* Optimisation bits */
	foam->foamProg.infoBits = IB_SIDE | IB_INLINEME;
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);


	/* Restore the saved state before returning. */
	gen0ProgRestoreState(saved);
	return clos;
}


local Foam
gen0RefId(AbSyn absyn)
{
	return gen0ExtendSyme(abSyme(absyn));
}


AInt
gen0MakeRefFormat()
{
	if (gen0RefFormat)
		return gen0RefFormat;

	gen0RefFormat = gen0StdDeclFormat(
				gen0RefFormatSize,
				gen0RefFormatNames,
				gen0RefFormatTypes,
				gen0RefFormatFmts);
	gen0SetDDeclUsage(gen0RefFormat, FOAM_DDecl_Multi);
	return gen0RefFormat;
}

