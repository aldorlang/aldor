#include "debug.h"
#include "inlstate.h"
#include "store.h"
#include "tfsat.h"
#include "sefo.h"
#include "stab.h"
#include "util.h"

// FIXME: New debug wanted
extern Bool	crinDebug;
#define crinDEBUG	DEBUG_IF(crin)		afprintf

/******************************
 *
 * :: InlSubstState
 *
 ******************************/

local Syme inlSubstSymeSubstSelf(Syme syme, TForm tfex);

InlSubstState
inlSubstStateNew(Syme syme)
{
	if (syme == NULL || !symeIsImport(syme)) {
		return NULL;
	}
	InlSubstState state = (InlSubstState) stoAlloc(OB_Other, sizeof(*state));
	state->sourceSyme = syme;
	state->sigma = NULL;
	return state;
}

Syme
inlSubstStateSyme(InlSubstState state, Syme syme)
{
	TForm oldExporter, newExporter;
	TForm oldType, newType;
	Syme nsyme;

	if (state == NULL) {
		return NULL;
	}

	if (symeIsExport(syme)) {
		syme = inlSubstSymeSubstSelf(syme, tfFollowFn(symeExporter(state->sourceSyme)));
		if (syme == NULL)
			return NULL;
	}
	if (state->sigma == NULL)
		state->sigma = tfSatSubList(tfGetExpr(tfFollowFn(symeExporter(state->sourceSyme))));

	if (state->sigma == absFail())
		return NULL;
	if (absIsEmpty(state->sigma))
		return syme;

	if (!symeIsImport(syme))
		return NULL;

	oldExporter = symeExporter(syme);
	if (!oldExporter) return NULL;

	newExporter = tformSubst(state->sigma, oldExporter);
	tfFollow(newExporter);

	tfHasSelf(newExporter) = false;

	oldType = symeType(syme);
	newType = tformSubst(state->sigma, oldType);

	nsyme = tfHasDomImport(newExporter, symeId(syme), newType);

	crinDEBUG(dbOut, "Replaced %pSyme %pTForm with %pTForm\n", syme, oldType, newType);

	return nsyme;
}

local Syme
inlSubstSymeSubstSelf(Syme syme, TForm tfex)
{
	SymeList	symes;
	Syme		nsyme;

	symes = symeListSubstSelf(stabFile(), tfex,
				  listCons(Syme)(syme, listNil(Syme)));
	nsyme = symes->first;
	nsyme = tfHasDomImport(tfex, symeId(nsyme), symeType(nsyme));

	return nsyme;
}


/******************************
 *
 * :: InlEnv
 *
 ******************************/

InlEnvState
inlEnvNew(Foam denv, Bool hasEnv0, Foam env1)
{
	InlEnvState state = (InlEnvState) stoAlloc(OB_Other, sizeof(*state));

	state->denv = denv;
	state->hasEnv0 = hasEnv0;
	state->env1 = env1;
	state->env0 = NULL;

	return state;
}

Bool
inlEnvHasEnv0(InlEnvState state)
{
	return state->hasEnv0;
}

void
inlEnvSetEnv0(InlEnvState state, Foam env0)
{
	state->env0 = env0;
}

void
inlEnvFree(InlEnvState state)
{
	// ?? Have we swallowed env0 & env1
	stoFree(state);
}

Foam
inlEnvTransformLex(InlEnvState state, Foam lex)
{
	int level  = lex->foamLex.level;
	int index  = lex->foamLex.index;
	int format = state->denv->foamDEnv.argv[level];

	if (level == 0) {
		if (state->env0 == NULL)
			bug("crinlin: env0 not allocated");
		return foamNewEElt(format, foamCopy(state->env0), 0, index);
	}
	// NB: Could do EElt->Lex mapping here
	if (foamTag(state->env1) == FOAM_Env) {
		return foamNewLex(state->env1->foamEnv.level + level - 1, index);
	}
	return foamNewEElt(format, foamCopy(state->env1), level - 1, index);
}

Foam
inlEnvTransformEnv(InlEnvState state, Foam env)
{
	int level  = env->foamEnv.level;

	if (level == 0) {
		if (state->env0 == NULL)
			bug("crinlin: env0 not allocated");
		return foamCopy(state->env0);
	}

	if (foamTag(state->env1) == FOAM_Env) {
		return foamNewEnv(level - 1);
	}
	else if (level == 0) {
		return foamCopy(state->env1);
	}
	else {
		return foamNewEEnv(level - 1, foamCopy(state->env1));
	}
}

