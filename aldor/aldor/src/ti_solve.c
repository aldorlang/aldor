#include "absyn.h"
#include "comsg.h"
#include "comsgdb.h"
#include "debug.h"
#include "orenv.h"
#include "tposs.h"
#include "stab.h"
#include "sefo.h"
#include "sexpr.h"

local OrEnv tisOrEnv(AbSyn ab, OrEnv known);
local void tisApply (AbSyn ab, OrEnv whole);
local void tisCommit(InferEnv);
local void tisApplyOrEnv(AbSyn ab, OrEnv whole);

Bool	tipSolveDebug	= false;
#define tipSolveDEBUG	DEBUG_IF(tipSolve)	afprintf

/*
 * NB: There's no attempt at efficiency here - let's get it working first
 */
void
tiSolve(Stab stab, AbSyn ab)
{
	OrEnv whole;
	static int count = 0;
	int serialNo = count++;
	/* For each absyn, find OrEnv (ie a List(InfEnv))
	 * Produce E = and(orEnv)
	 * For each absyn, adjust TPoss by running (E and X) across each element.
	 */
	tipSolveDEBUG(dbOut, "(Solve(%d): %pAbSyn\n", serialNo, ab);
	whole = tisOrEnv(ab, orEnvOne(infEnvEmpty()));
	tipSolveDEBUG(dbOut, " Solve(%d): %pOrEnv\n", serialNo, whole);

	if (!orEnvIsEmpty(whole)) {
		tisApply(ab, whole);

		if (orEnvSingleton(whole)) {
			stabInfEnvApply(stab, orEnvSingleton(whole));
		}

		if (orEnvIsSingleton(whole)) {
			tipSolveDEBUG(dbOut, "Committing %pOrEnv\n", whole);
			InferEnv infEnv = orEnvSingleton(whole);
			if (infEnvIsFailed(infEnv)) {
				comsgError(ab, ALDOR_E_TinVarNotInferred);
			}
			else {
				tisCommit(infEnv);
			}
		}
	}
	tipSolveDEBUG(dbOut, " Solve(%d): Complete)\n", serialNo);
}

local void
tisCommit(InferEnv infEnv)
{
	InferEnvIterator it;
	infEnvSetImmutable(infEnv);

	for (infEnvITER(it, infEnv); infEnvMORE(it); infEnvSTEP(it)) {
		TForm key = infEnvKEY(it);
		TForm elt = infEnvELT(it);
		TForm rep = infEnvREP(it);

		if (tfIsVar(key) && elt != NULL) {
			tfVarFix(key, infEnv);
		}
	}
}

local OrEnv
tisOrEnv(AbSyn ab, OrEnv known)
{
	if (abStab(ab)) {
		known = orEnvAnd(known, orEnvOne(stabInfEnv(abStab(ab))));
	}
	if (abState(ab) != AB_State_HasPoss) {
		return known;
	}
	else {
		TPoss tp = abTPoss(ab);
		OrEnv orEnv = orEnvFrTPoss(tp);
		known = orEnvAnd(known, orEnv);
	}
	if (!abIsLeaf(ab)) {
		for (int i = 0; i < abArgc(ab); i += 1) {
			AbSyn abi = abArgv(ab)[i];
			known = tisOrEnv(abi, known);
		}
	}
	return known;
}


local void
tisApply(AbSyn ab, OrEnv whole)
{
	if (abState(ab) != AB_State_HasPoss) {
		return;
	}
	if (abStab(ab) != NULL) {
		//stabInfEnvApply(abStab(ab), car(whole->envs));
	}
	tisApplyOrEnv(ab, whole);

	if (!abIsLeaf(ab)) {
		for (int i = 0; i < abArgc(ab); i += 1) {
			AbSyn abi = abArgv(ab)[i];
			tisApply(abi, whole);
		}
	}
}

local void
tisApplyOrEnv(AbSyn ab, OrEnv whole)
{
	TPoss tposs = abTPoss(ab);
	TPoss newTPoss = tpossEmpty();
	TPossIterator it;

	for (tpossITER(it, tposs); tpossMORE(it); tpossSTEP(it)) {
		UTForm utf = tpossUELT(it);
		InferEnv infEnv = tpossINFENV(it);
		OrEnv orEnv = orEnvAnd(whole, orEnvOne(infEnv));
		if (!orEnvIsEmpty(orEnv)) {
			// TODO: This is bad - need to multiply the TPoss
			if (!listIsSingleton(orEnv->envs)) {
				afprintf(dbOut, "*** Lots of envs %pAbSyn %d\n", ab);
			}
			InferEnv infEnv = car(orEnv->envs);
			if (infEnvIsEmpty(infEnv)) {
				tpossAdd1UTForm(newTPoss, utf);
			}
			else {
				tpossAdd1(newTPoss,
					  tformFollowVars(infEnv, utformConstOrFail(utf)));
			}
		}
	}
	tpossFree(tposs);
	tipSolveDEBUG(dbOut, " Apply: %pAbSyn --> %pTPoss\n", ab, newTPoss);
	abTPoss(ab) = newTPoss;
}

/*
 * :: Check
 * Check that the stab looks reasonable, with no uninferred variables
 */

local void tisCheckStab(Stab stab);

void
tisCheck(Stab stab, AbSyn ab)
{
	if (!abIsSefo(ab)) {
		return;
	}
	if (abStab(ab) != NULL) {
		stab = abStab(ab);
		tisCheckStab(stab);
	}

	if (!abIsLeaf(ab)) {
		for (int i = 0; i < abArgc(ab); i += 1) {
			AbSyn abi = abArgv(ab)[i];
			tisCheck(stab, abi);
		}
	}
}

local void
tisCheckStab(Stab stab)
{
	TFormList vars = stabRegisteredVars(stab);

	while (vars != listNil(TForm)) {
		TForm var = car(vars);
		vars = cdr(vars);
		if (!tfIsVarInferred(var)) {
			comsgError(tfExpr(var), ALDOR_E_TinVarNotInferred);
		}
	}
}
