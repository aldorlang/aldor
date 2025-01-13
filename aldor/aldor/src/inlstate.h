#ifndef _INLSTATE_H
#define _INLSTATE_H

#include "absub.h"
#include "foam.h"
#include "syme.h"
#include "tform.h"

typedef struct inlEnvState *InlEnvState;
typedef struct inlSubstState *InlSubstState;

struct inlEnvState {
	Foam denv;
	Foam env0;
	Foam env1;
	Bool hasEnv0;
};

struct inlSubstState {
	Syme sourceSyme;
	AbSub sigma; // Lazy
	Foam formats;
};

InlSubstState	inlSubstStateNew(Syme syme);
Syme		inlSubstStateSyme(InlSubstState state, Syme syme);

InlEnvState inlEnvNew(Foam denv, Bool hasEnv0, Foam env1);
void 	    inlEnvFree(InlEnvState);
Foam        inlEnvTransformLex(InlEnvState state, Foam lex);
Foam        inlEnvTransformEnv(InlEnvState state, Foam env);
void        inlEnvSetEnv0(InlEnvState state, Foam env0);
Bool        inlEnvHasEnv0(InlEnvState state);

#endif
