#include "orenv.h"
#include "store.h"
#include "debug.h"
#include "format.h"
#include "infenv.h"
#include "tposs.h"

extern Bool	tipSolveDebug;
#define tipSolveDEBUG	DEBUG_IF(tipSolve)	afprintf

/*
 * :: OrEnv
 *
 * Or(InferEnv_1, InferEnv_2, ...)
 */
static int orEnvSerial = 0;

OrEnv
orEnvNewNone(void)
{
	OrEnv e = (OrEnv) stoAlloc(OB_Other, sizeof(*e));
	e->serialNo = orEnvSerial++;
	e->envs = listNil(InferEnv);
	return e;
}

OrEnv
orEnvOne(InferEnv infEnv)
{
	OrEnv e = orEnvNewNone();
	e->envs = listSingleton(InferEnv)(infEnv);
	return e;
}

OrEnv
orEnvFrTPoss(TPoss tp)
{
	TPossIterator it;
	OrEnv orEnv = orEnvNewNone();

	List(InferEnv) envs = listNil(InferEnv);
	for (tpossITER(it, tp); tpossMORE(it); tpossSTEP(it)) {
		InferEnv infEnv = tpossINFENV(it);
		orEnvAdd(orEnv, infEnv);
	}
	tipSolveDEBUG(dbOut, " OrEnvTP:      TP: %pTPoss\n", tp);
	tipSolveDEBUG(dbOut, " ..OrEnvTP: OrEnv: %pOrEnv\n", orEnv);
	return orEnv;
}

TPoss
orEnvToTPoss(TForm tf, OrEnv orEnv)
{
	OrEnvIterator it;
	TPoss tposs = tpossEmpty();

	for (orEnvITER(it, orEnv); orEnvMORE(it); orEnvSTEP(it)) {
		InferEnv env = orEnvELT(it);
		tpossAdd1UTFContext(tposs, uctxtNewConst(env, tf));
	}
	return tposs;
}

Bool
orEnvIsSingleton(OrEnv orEnv)
{
	return listIsSingleton(orEnv->envs);
}

InferEnv
orEnvSingleton(OrEnv orEnv)
{
	assert(orEnvIsSingleton(orEnv));
	return car(orEnv->envs);
}

InferEnvList
orEnvContent(OrEnv orEnv)
{
	return orEnv->envs;
}

Bool
orEnvIsEmpty(OrEnv orEnv)
{
	return orEnv->envs == listNil(InferEnv);
}

OrEnv
orEnvAnd(OrEnv orEnv1, OrEnv orEnv2)
{
	static int count;
	int serialNo = count++;
	OrEnvIterator it1;
	OrEnvIterator it2;
	OrEnv envs = orEnvNewNone();

	tipSolveDEBUG(dbOut, "(OrEnvAnd[%d]: 1: %pOrEnv\n", serialNo, orEnv1);
	tipSolveDEBUG(dbOut, " OrEnvAnd[%d]: 2: %pOrEnv\n", serialNo, orEnv2);

	for (orEnvITER(it1, orEnv1); orEnvMORE(it1); orEnvSTEP(it1)) {
		for (orEnvITER(it2, orEnv2); orEnvMORE(it2); orEnvSTEP(it2)) {
			InferEnv e1 = orEnvELT(it1);
			InferEnv e2 = orEnvELT(it2);
			InferEnv m = infEnvMerge(e1, e2);
			orEnvAdd(envs, m);
		}
	}
	tipSolveDEBUG(dbOut, "OrEnvAnd[%d]: --> %pOrEnv)\n", serialNo, envs);
	return envs;
}

void
orEnvAdd(OrEnv orEnv, InferEnv env)
{
	OrEnvIterator it;
	Bool found = false;

	if (infEnvIsFailed(env)) {
		return;
	}
	for (orEnvITER(it, orEnv); !found && orEnvMORE(it); orEnvSTEP(it)) {
		InferEnv iterEnv = orEnvELT(it);
		if (infEnvEqual(env, iterEnv)) {
			found = true;
		}
	}
	if (!found) {
		orEnv->envs = listCons(InferEnv)(env, orEnv->envs);
	}
}

/*
 * :: Iteration
 */

void
orEnvIter(OrEnvIterator *it, OrEnv orEnv)
{
	it->li = orEnv->envs;
}

Bool
orEnvMore(OrEnvIterator *it)
{
	return it->li != listNil(InferEnv);
}

void
orEnvStep(OrEnvIterator *it)
{
	it->li = cdr(it->li);
}

InferEnv
orEnvElt(OrEnvIterator *it)
{
	return car(it->li);
}

int
orEnvOStreamWrite(OStream os, OrEnv orEnv)
{
	InferEnvList content = orEnvContent(orEnv);

	return ostreamPrintf(os, "(Or[%d](%d) %pInferEnvList)",
			     orEnv->serialNo,
			     listLength(InferEnv)(content), content);
}

/*
 * :: Debug
 */

void
orEnvPrintDb(OrEnv orEnv)
{
	afprintf(dbOut, "%pOrEnv\n", orEnv);
}
