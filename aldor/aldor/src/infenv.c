#include "axlobs.h"
#include "debug.h"
#include "infenv.h"

#include "store.h"
#include "tform.h"
#include "sefo.h"
#include "sexpr.h"
#include "ufind.h"
#include "util.h"

CREATE_LIST(InferEnv);

Bool	infEnvDebug	= false;
Bool	infEnvUnifyDebug	= false;
#define infEnvDEBUG	DEBUG_IF(infEnv)	afprintf
#define infEnvUnifyDEBUG	DEBUG_IF(infEnvUnify)	afprintf

struct unifyError {
	int errorCode;
	AbSyn src;
	TForm tf1;
	TForm tf2;
};

// NB: Maybe failed should be represented by tbl=NULL
struct inferEnv {
	Bool	  failed;
	Bool	  immutable;
	TFormList vars;
	UFTable   tbl;
};

CREATE_LIST(UnifyError);

static struct inferEnv theEmptyInferEnv  = { false, true, listNil(TForm), NULL };
static struct inferEnv theFailedInferEnv = { true,  true, listNil(TForm), NULL };

static int infEnvSerial = 0;

UnifyError
unifyErrorNew(int errCode, AbSyn src, TForm tf1, TForm tf2)
{
	UnifyError err = (UnifyError) stoAlloc(OB_Other, sizeof(*err));
	err->errorCode = 0;
	err->src = src;
	err->tf1 = tf1;
	err->tf2 = tf2;

	return err;
}


#define infEnvTable(infEnv) ((infEnv)->tbl)

InferEnv
infEnvNew()
{
	InferEnv infEnv = (InferEnv) stoAlloc(OB_InferEnv, sizeof(*infEnv));
	infEnv->failed = false;
	infEnv->immutable = false;
	infEnv->vars = listNil(TForm);
	infEnv->tbl  = uftNew();

	return infEnv;
}

InferEnv
infEnvCopy(InferEnv infEnv)
{
	InferEnvIterator it;
	InferEnv newEnv = infEnvNew();
	newEnv->failed = infEnv->failed;
	newEnv->immutable = false;
	newEnv->vars = listCopy(TForm)(infEnv->vars);
	for (infEnvITER(it, infEnv); infEnvMORE(it); infEnvSTEP(it)) {
		TForm tfk = infEnvKEY(it);
		TForm tfv = infEnvELT(it);
		TForm tfr = infEnvREP(it); // expression
		if (tfr == tfk && tfv == NULL)
			;
		else if (tfr == tfk)
			infEnvExtend(newEnv, tfk, tfv);
		else
			infEnvExtend(newEnv, tfk, tfr);
	}

	return newEnv;
}

Bool
infEnvIsImmutable(InferEnv infEnv)
{
	return infEnv->immutable;
}

InferEnv
infEnvEmpty()
{
	return &theEmptyInferEnv;
}

InferEnv
infEnvFailed()
{
	return &theFailedInferEnv;
}

Bool
infEnvIsFailed(InferEnv infEnv)
{
	assert(infEnv != NULL);
	return infEnv->failed;
}

Bool
infEnvIsEmpty(InferEnv infEnv)
{
	assert(infEnv != NULL);
	return !infEnv->failed && (infEnv->tbl == NULL || uftIsEmpty(infEnv->tbl));
}


void
infEnvRegister(InferEnv env, TForm tf)
{
	env->vars = listCons(TForm)(tf, env->vars);
}

void
infEnvSetFailed(InferEnv env)
{
	env->failed = true;
}

TForm
infEnvFollow(InferEnv env, TForm tf)
{
	tfFollowOnly(tf);
	if (!tfIsVar(tf) && !tfIsInfSubst(tf)) {
		return tf;
	}
	UFElt elt = uftProbe(infEnvTable(env), tf);
	if (elt == NULL) {
		return tf;
	}
	TForm ptf = (TForm) uftEltData(elt);
	return ptf == NULL ? uftEltTForm(elt): ptf;
}

TForm
infEnvTForm(InferEnv env, TForm tf)
{
	assert(tfIsVar(tf));
	UFElt elt = (UFElt) uftProbe(infEnvTable(env), tf);
	if (elt == NULL) {
		return NULL;
	}
	return (TForm) uftEltData(elt);
}

Bool
infEnvEqual(InferEnv env1, InferEnv env2)
{
	InferEnvIterator it;
	Bool ok = true;
	if (env1 == env2)
		return true;
	if (infEnvIsFailed(env1) != infEnvIsFailed(env2)) {
		return false;
	}
	if (infEnvIsFailed(env1)) {
		return true;
	}
	if (infEnvIsEmpty(env1) && infEnvIsEmpty(env2)) {
		return true;
	}
	if (infEnvIsEmpty(env1) || infEnvIsEmpty(env2)) {
		return false;
	}
	if (uftSize(infEnvTable(env1)) != uftSize(infEnvTable(env2))) {
		return false;
	}
	for (infEnvITER(it, env1); infEnvMORE(it) && ok; infEnvSTEP(it)) {
		TForm tfi = infEnvFollow(env1, infEnvREP(it));
		TForm tfi2 = infEnvFollow(env2, infEnvREP(it));
		ok = tfEqual(tfi, tfi2);
	}
	return ok;
}


/*
 * :: Iteration
 */
void
_infEnvITER(InferEnvIterator *it, InferEnv e)
{
	it->env = e;
	uftITER(it->uftIter, e->tbl ? e->tbl: uftTheEmpty());
}

TForm
_infEnvELT(InferEnvIterator *it)
{
	return (TForm) uftELT(it->uftIter);
}

/*
 * :: Output
 */

int
infEnvOStreamWrite(OStream os, InferEnv env)
{
	InferEnvIterator it;
	int n = 0;
	n += ostreamPrintf(os, "(InfEnv", infEnvIsFailed(env) ? "failed" : "");

	for (infEnvITER(it, env); infEnvMORE(it); infEnvSTEP(it)) {
		TForm tfk = infEnvKEY(it);
		TForm tfv = infEnvELT(it);
		TForm tfr = infEnvREP(it);

		n += ostreamPrintf(os, " ");
		if (tfr == tfk && tfv == NULL)
			n += ostreamPrintf(os, "(%pTForm)", tfk);
		else if (tfr == tfk)
			n += ostreamPrintf(os, "(%pTForm -> %pTForm)", tfk, tfv);
		else
			n += ostreamPrintf(os, "(%pTForm == %pTForm)", tfk, tfr);
	}
	n += ostreamPrintf(os, ")");
	return n;

}


/*
 * :: Constraints
 */

Bool
infEnvExtend(InferEnv env, TForm S, TForm T)
{
	Bool result = false;
	assert(tfIsVariable(S));
	assert(!infEnvIsImmutable(env));

	infEnvDEBUG(dbOut, "(InfEnvExtend: %pInferEnv %pTForm %pTForm\n", env, S, T);
	if (S == T) {
		result = true;
	}
	else if (tfIsVariable(T)
		 && uftProbe(infEnvTable(env), S) != NULL
		 && uftProbe(infEnvTable(env), S) == uftProbe(infEnvTable(env), T)) {
		infEnvDEBUG(dbOut, " InfEnvExtend: Same\n");
		result = true;
	}
	else if (tfIsVariable(T)) {
		TForm tfS = infEnvTForm(env, S);
		TForm tfT = infEnvTForm(env, T);
		if (tfS != NULL && tfT != NULL) {
			result = tformUnifyMod(env, listNil(Syme), tfS, tfT);
			if (result) {
				uftSetData(infEnvTable(env), S, NULL);
				uftSetData(infEnvTable(env), T, NULL);
				uftUnion(infEnvTable(env), S, T);
				uftSetData(infEnvTable(env), S, tfS);
				infEnvDEBUG(dbOut, " InfEnvExtend.. Merge both: %pInferEnv\n", env);
			}
		}
		else {
			uftSetData(infEnvTable(env), S, tfS == NULL ? tfT : tfS);
			uftSetData(infEnvTable(env), T, NULL);
			uftUnion(infEnvTable(env), S, T);
			result = true;
		}
	}
	else {
		TForm tfS = infEnvTForm(env, S);
		if (tfS) {
			result = tformUnifyMod(env, listNil(Syme), tfS, T);
		}
		else {
			uftSetData(infEnvTable(env), S, T);
			result = true;
		}
	}
	if (!result) {
		infEnvSetFailed(env);
	}
	infEnvDEBUG(dbOut, " InfEnvExtend: %oBool %pInferEnv)\n", result, env);
	return result;
}

/*
 * :: Unification
 */
#if 0
/*
 * :: ::Higher level functions
 */
Bool infEnvUnifyTForm(InferEnv env, TForm tf1, TForm tf2);
Bool infEnvUnifySefo(InferEnv env, Sefo sefo1, Sefo sefo2);

Bool
infEnvUnifyTForm(InferEnv env, TForm tf1, TForm tf2)
{
	static int instance = 0, depth = 0;
	int serialNo = instance++;
	Bool result = false;
	depth++;

	tfFollow(tf1);
	tfFollow(tf2);
	tf1 = infEnvFollow(env, tf1);
	tf2 = infEnvFollow(env, tf2);

	infEnvUnifyDEBUG(dbOut, "(infenvUnifyTForm(%d:%d): %pInferEnv\n", serialNo, depth, env);
	infEnvUnifyDEBUG(dbOut, " infenvUnifyTForm(%d:%d): %pTForm\n", serialNo, depth, tf1);
	infEnvUnifyDEBUG(dbOut, " infenvUnifyTForm(%d:%d): %pTForm\n", serialNo, depth, tf2);
	if (tf1 == tf2) {
		result = true;
	}
	else if (tfIsVar(tf1) && tfIsVar(tf2)) {
		infEnvSet(env, tf1, tf2);
		result = true;
	}
	else if (tfIsVar(tf1)) {
		infEnvSet(env, tf1, tf2);
		result = true;
	}
	else if (tfIsVar(tf2)) {
		infEnvSet(env, tf2, tf1);
		result = true;
	}
	else if (tfTag(tf1) != tfTag(tf2)) {
		result = false;
	}
	else if (tfIsGeneral(tf1)) {
		result = infEnvUnifySefo(env, tfExpr(tf1), tfExpr(tf2));
	}
	else if (tf1->argc != tf2->argc) {
		result = false;
	}
	else {
		result = tformUnifyMod(env, listNil(Syme), tf1, tf2);
	}
	infEnvUnifyDEBUG(dbOut, " infenv(%d:%d): result: %pInferEnv)\n", serialNo, depth, env);
	infEnvUnifyDEBUG(dbOut, " infenv(%d:%d): result: %oBool)\n", serialNo, depth, result);
	depth--;

	return result;
}

Bool
infEnvUnifySefo(InferEnv env, Sefo sefo1, Sefo sefo2)
{
	static int instance = 0, depth = 0;
	int serialNo = instance++;
	depth++;
	Bool result;

	infEnvDEBUG(dbOut, "(infEnvUnifySefo(%d:%d): %pInferEnv\n", serialNo, depth, env);
	infEnvDEBUG(dbOut, " infEnvUnifySefo(%d:%d): %pSefo\n", serialNo, depth, sefo1);
	infEnvDEBUG(dbOut, " infEnvUnifySefo(%d:%d): %pSefo\n", serialNo, depth, sefo2);

	if (abIsVar(sefo1) && abIsVar(sefo2)) {
		result = infEnvUnifyTForm(env, abTForm(sefo1), abTForm(sefo2));
	}
	else if (abIsVar(sefo1)) {
		infEnvSet(env, abTForm(sefo1), abTForm(sefo2));
		result = true;
	}
	else if (abIsVar(sefo2)) {
		infEnvSet(env, abTForm(sefo1), abTForm(sefo2));
		result = true;
	}
	else if (abTag(sefo1) != abTag(sefo2)) {
		result = false;
	}
	else if (abIsLeaf(sefo1)) {
		result = sefoEqual(sefo1, sefo2);
	}
	else if (abArgc(sefo1) != abArgc(sefo2)) {
		result = false;
	}
	else {
		for (int i=0; i<abArgc(sefo1) && !infEnvIsFailed(env); i++) {
			infEnvUnifySefo(env, abArgv(sefo1)[i], abArgv(sefo2)[i]);
		}
		result = !infEnvIsFailed(env);
	}
	infEnvDEBUG(dbOut, " infEnvUnifySefo(%d:%d): %oBool %pInferEnv)\n", serialNo, depth, result, env);
	depth--;
	return result;
}

#endif
/*
 * Always return an unaliased mutable object, or failed
 */
InferEnv
infEnvMerge(InferEnv env1, InferEnv env2)
{
	InferEnvIterator iter;
	static int instance = 0;
	int serialNo = instance++;
	
	if (infEnvIsFailed(env1) || infEnvIsFailed(env2)) {
		infEnvDEBUG(dbOut, "(Merge(%d) %pInferEnv %pInferEnv\n", serialNo, env1, env2);
		infEnvDEBUG(dbOut, " Merge(%d) FAIL)\n", serialNo);
		return infEnvFailed();
	}
	if (infEnvIsEmpty(env1)) {
		infEnvDEBUG(dbOut, "(Merge(%d) <<Empty>> %pInferEnv)\n", serialNo, env2);
		return infEnvCopy(env2);
	}
	if (infEnvIsEmpty(env2)) {
		infEnvDEBUG(dbOut, "(Merge(%d) %pInferEnv <<Empty>>)\n", serialNo, env1);
		return infEnvCopy(env1);
	}
	infEnvDEBUG(dbOut, "(Merge(%d):\n", serialNo);
	infEnvDEBUG(dbOut, " Merge(%d) %pInferEnv\n", serialNo, env1);
	infEnvDEBUG(dbOut, " Merge(%d) %pInferEnv\n", serialNo, env2);

	InferEnv newEnv = infEnvCopy(env1);
	for (infEnvITER(iter, env2); infEnvMORE(iter); infEnvSTEP(iter)) {
		TForm tfk = infEnvKEY(iter); // var
		TForm tfr = infEnvREP(iter); // canonical
		TForm tfv = infEnvELT(iter); // expression
		infEnvDEBUG(dbOut, "Env2 %pTForm %pTForm %pTForm\n", tfk, tfr, tfv);
	}

	for (infEnvITER(iter, env2); infEnvMORE(iter); infEnvSTEP(iter)) {
		TForm tfk = infEnvKEY(iter); // var
		TForm tfr = infEnvREP(iter); // canonical
		TForm tfv = infEnvELT(iter); // expression

		infEnvDEBUG(dbOut, "Merge %pTForm %pTForm\n", tfk, tfr);

		infEnvExtend(newEnv, tfk, tfr);
		if (tfv != NULL) {
			infEnvExtend(newEnv, tfr, tfv);
		}
		if (infEnvIsFailed(newEnv))
			break;
	}

	infEnvDEBUG(dbOut, " Merge(%d) Complete %pInferEnv)\n", serialNo, newEnv);

	return newEnv;
}

void
infEnvCommit(InferEnv infEnv)
{
	InferEnvIterator it;

	assert(!infEnvIsFailed(infEnv));
	if (infEnvIsEmpty(infEnv)) {
		return;
	}

	for (infEnvITER(it, infEnv); infEnvMORE(it); infEnvSTEP(it)) {
		TForm tfk = infEnvKEY(it);
		TForm tfr = infEnvREP(it); // expression

		if (tfIsVar(tfk)) {
			tfVarFix(tfk, infEnv);
		}
	}
	infEnv->immutable = true;
}

void
infEnvSetImmutable(InferEnv infEnv)
{
	infEnv->immutable = true;
}

void
infEnvPrintDb(InferEnv infEnv)
{
	afprintf(dbOut, "%pInferEnv\n", infEnv);
}

