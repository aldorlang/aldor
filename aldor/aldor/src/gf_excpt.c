
/*****************************************************************************
 *
 * gf_excpt.c: Foam code generation for try/except/always/blocks
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "genfoam.h"
# include "gf_util.h"
# include "gf_prog.h"
# include "gf_rtime.h"
# include "gf_excpt.h"

local Foam	gen0TryExprPart	(AbSyn);

typedef struct _tryContext {
	int 	label;
	Foam 	var;
} *TryContext;

static TryContext 	genTryExitContext;

/****************************************************************************
 *
 * :: -Wdumb-import options
 *
 ****************************************************************************/

static Bool	gfLazyCatchFlag = false;

Bool
gfLazyCatch(void)
{
	return gfLazyCatchFlag;
}


void
gfSetLazyCatch(Bool flag)
{
	gfLazyCatchFlag = flag;
}

/****************************************************************************
 *
 * :: Code generation
 *
 ****************************************************************************/


/*
 * Code generation of an exception handler.
 */
local void
gen0CatchHandler(AbSyn handler, int nret, Foam val, Foam flag, TForm tftry)
{
	/* No handler means that we have nothing to do */
	if (abIsNothing(handler)) return;


	/* Deal with void and non-void catch blocks */
	if (nret != 0) {
		Foam tmp = genFoamVal(handler);
		gen0AddStmt(foamNewSet(foamCopy(val), tmp), handler);
	}
	else
		genFoamStmt(handler);


	/* Indicate that the exception has been handled */
	gen0AddStmt(foamNewSet(foamCopy(flag), foamNewBool(true)), handler);
}


/*
 * A typical catch block looks like:
 *   (tmp, val, exn) := Catch( (Clos (Env 0) (Prog xxx)))
 *   if tmp then goto Lxx
 *   U := <handler-code>
 *   if U case bad then goto Lyy
 *   tmp := true
 *   val := U.value
 * Lxx:
 *   <always-code>
 *   if tmp then goto Lzz
 *   raise exn
 *   Halt 		-- temporary
 * Lzz;
 *   return val
 */
Foam
genTry(AbSyn absyn)
{
	Scope("genTry");
	TForm   tf;
	AbSyn	expr 	= absyn->abTry.expr;
	AbSyn	id      = absyn->abTry.id;
	AbSyn	handler = absyn->abTry.except;
	AbSyn	always  = absyn->abTry.always;
	Foam	catch;
	FoamTag exprType;
	Foam    stdExit, ret, val, exn;
	AInt    exprFmt, catchFmt;
	int 	doneLabel, protLabel, nret;
	Foam 	lhs, tmp;
	TryContext     fluid(genTryExitContext);
	TForm		tfTry = tfNone(), tfExn;
	struct _tryContext ctxt;

	tf 	 = gen0AbType(absyn);
	exprType = gen0Type(tf, &exprFmt);
	stdExit  = gen0Temp(FOAM_Bool);


	doneLabel = gen0State->labelNo++;
	protLabel = gen0State->labelNo++;

	if (tfIsMulti(tf))
		nret = tfMultiArgc(tf);
	else
		nret = 1;

	if (nret > 1) {
		val = gen0Temp(FOAM_Word);
		tfTry = gen0AbType(expr);
	}
	else if (nret == 1) {
		val = gen0Temp0(exprType, exprFmt);
		tfTry = gen0AbType(expr);
	}
	else if (nret == 0)
		val = foamNewNOp(); /* Unused */
#if EDIT_1_0_n1_07
	else {
		NotReached(val = NULL);
	}
#endif
 
	if (abIsNothing(id)) {
		bugWarning("try: no identifier!");
		tfExn = tfNone();
	 	exn = gen0Temp(FOAM_Word);
	}
	else {
		tfExn = gen0AbType(id);
	  	exn = genFoamVal(id);
	}

	catch = foamNewCatch(gen0TryExprPart(expr), foamNewSInt(int0));


	if (nret) {
		tmp = foamCopy(stdExit);
		lhs = foamNew(FOAM_Values, 3, tmp, foamCopy(val), exn);
		catchFmt = gen0CatchFormatNumber(tfTry, tfExn);
	}
	else {
		lhs = foamNew(FOAM_Values, 2, foamCopy(stdExit), exn);
		catchFmt = gen0VoidCatchFormatNumber(tfExn);
	}


	/* Wrap the catch in an MFmt */
	catch = foamNewMFmt(catchFmt, catch);
	gen0AddStmt(foamNewSet(lhs, catch), absyn);


	/* Skip the exception handler if no exception thrown */
	gen0AddStmt(foamNewIf(foamCopy(stdExit), protLabel), absyn);


	/* Generate code for the exception handler (catch block) */
	ctxt.label = protLabel;
	ctxt.var   = exn;
	genTryExitContext = &ctxt;
	gen0CatchHandler(handler, nret, val, stdExit, tfTry);
	genTryExitContext = NULL;


	/* Code always executed after try blocks */
	gen0AddStmt(foamNewLabel(protLabel), absyn);
	genFoamStmt(always);


	/* Halt if unhandled exception */
	gen0AddStmt(foamNewIf(foamCopy(stdExit), doneLabel), absyn);
	gen0AddStmt(foamNewThrow(foamNewSInt(int0), foamCopy(exn)), handler);
	gen0AddStmt(foamNew(FOAM_BCall, 2, FOAM_BVal_Halt,
			    foamNewSInt(FOAM_Halt_NeverReached)), absyn);
	gen0AddStmt(foamNewLabel(doneLabel), absyn);


	/* Value of the whole try expression */
	ret = foamCopy(val);


	/* Clean up and return */
	foamFree(stdExit);
	Return(ret);
}

local Foam
gen0TryExprPart(AbSyn expr)
{
  	return gen0BuildFunction(PT_ExFn, strConcat(gen0ProgName, "_expr"), expr);
}

Foam
gen0Raise(AbSyn absyn)
{
	Foam foam;
	AbSyn target = absyn->abRaise.expr;

	/* temporary --- should be a bit more cunning */
	foam =  genFoamVal(target);

	if (genTryExitContext == NULL) {
		gen0AddStmt(foamNewThrow(foamNewSInt(int0), foam), absyn);
		gen0AddStmt(foamNew(FOAM_BCall, 2,
				    FOAM_BVal_Halt,
				    foamNewSInt(FOAM_Halt_NeverReached)),
		    absyn);
	}
	else {
		gen0AddStmt(foamNewSet(foamCopy(genTryExitContext->var),
				       foam), absyn);
		gen0AddStmt(foamNewGoto(genTryExitContext->label), absyn);
	}
	return NULL;
}

