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

// NB: Maybe failed should be represented by tbl=NULL
struct inferEnv {
	Bool	  failed;
	Bool	  immutable;
	TFormList vars;
	UFTable   tbl;
};

// We store VarInfo in the UFTable
struct uinfo {
	SExpr failInfo;
	TForm tform;
};
typedef struct UInfo *UVarInfo;

static struct inferEnv theEmptyInferEnv  = { false, true, listNil(TForm), NULL };
static struct inferEnv theFailedInferEnv = { true,  true, listNil(TForm), NULL };

UInfo
varInfoNew(TForm tf)
{
	UInfo info = (UInfo) stoAlloc(OB_Other, sizeof(*info));
	info->tform = tf;
	info->failInfo = sxNil;
	return info;
}

TForm
varInfoTForm(UInfo info)
{
	return info->tform;
}

void
varInfoSetFailure(UInfo info, SExpr error)
{
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
	InferEnv newEnv = infEnvNew();
	newEnv->failed = infEnv->failed;
	newEnv->immutable = false;
	newEnv->vars = listCopy(TForm)(infEnv->vars);
	newEnv->tbl = infEnv->tbl == NULL ? uftNew() : uftCopy(infEnv->tbl);

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
	return &theEmptyInferEnv;
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
infEnvRegister(InferEnv infEnv, TForm tf)
{
	infEnv->vars = listCons(TForm)(tf, infEnv->vars);
}

void
infEnvSet(InferEnv infEnv, TForm tf1, TForm tf2)
{
	assert(tfIsVar(tf1));
	assert(!infEnvIsImmutable(infEnv));
	assert(!infEnvIsFailed(infEnv));

	if (tfIsVar(tf2)) {
		uftUnion(infEnvTable(infEnv), tf1, tf2);
	}
	else {
		UInfo info = varInfoNew(tf2);
		uftSetData(infEnvTable(infEnv), tf1, info);
	}
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
	UInfo info = (UInfo) uftEltData(elt);

	return info == NULL ? tf : varInfoTForm(info);
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
		TForm tfi = infEnvREP(it);
		UFElt ue = uftProbe(infEnvTable(env2), tfi);
		if (ue == NULL) {
			ok = false;
		}
		else {
			ok = tfEqual(tfi, uftEltTForm(ue));
		}
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
	UInfo info = (UInfo) uftELT(it->uftIter);
	if (info == NULL) {
		return NULL;
	}
	return varInfoTForm(info);
}

/*
 * :: Output
 */

int
infEnvOStreamWrite(OStream os, InferEnv infEnv)
{
	InferEnvIterator it;
	int n = 0;
	n += ostreamPrintf(os, "(InfEnv", infEnvIsFailed(infEnv) ? "failed" : "");

	for (infEnvITER(it, infEnv); infEnvMORE(it); infEnvSTEP(it)) {
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
 * :: ::Higher level functions
 */
Bool infEnvUnifyTForm(InferEnv env, TForm tf1, TForm tf2);
Bool infEnvUnifySefo(InferEnv env, Sefo sefo1, Sefo sefo2);

/*
 * :: Constraints
 */

Bool
infEnvExtend(InferEnv env, TForm S, TForm T)
{
	Bool result = false;
	assert(tfIsVariable(S));
	assert(!infEnvIsImmutable(env));

	TForm val = infEnvFollow(env, S);
	if (val == NULL) {
		infEnvSet(env, S, T);
		result = true;
	}
	else {
		result = infEnvUnifyTForm(env, val, T);
	}
	return result;
}
/*
 * :: Unification
 */

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
		infEnvSetFailed(env);
		result = false;
	}
	else if (tfIsGeneral(tf1)) {
		result = infEnvUnifySefo(env, tfExpr(tf1), tfExpr(tf2));
	}
	else if (tf1->argc != tf2->argc) {
		infEnvSetFailed(env);
		result = false;
	}
	else {
		result = tformUnifyMod(env, listNil(Syme), tf1, tf2);
		if (!result) {
			infEnvSetFailed(env);
		}
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
	infEnvDEBUG(dbOut, " infEnvUnifySefo(%d:%d): %pInferEnv)\n", serialNo, depth, result);
	depth--;
	return result;
}

/*
 * Always return an unaliased mutable object, or failed
 */
InferEnv
infEnvMerge(InferEnv env1, InferEnv env2)
{
	InferEnvIterator iter;

	if (infEnvIsFailed(env1) || infEnvIsFailed(env2)) {
		return infEnvFailed();
	}
	if (infEnvIsEmpty(env1)) {
		return infEnvCopy(env2);
	}
	if (infEnvIsEmpty(env2)) {
		return infEnvCopy(env1);
	}
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

		infEnvDEBUG(dbOut, "Merge %d %pTForm %pTForm\n", infEnvMORE(iter), tfk, tfr);

		infEnvExtend(newEnv, tfk, tfr);
		if (tfv != NULL) {
			infEnvExtend(newEnv, tfr, tfv);
		}
		if (infEnvIsFailed(newEnv))
			break;
	}
	infEnvDEBUG(dbOut, "Merge Complete %pInferEnv\n", newEnv);

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

		if (tfIsVar(tfk)) {
			tfVarFix(tfk, infEnv);
		}
	}
	infEnv->immutable = true;
}

void
infEnvPrintDb(InferEnv infEnv)
{
	afprintf(dbOut, "%pInferEnv\n", infEnv);
}

