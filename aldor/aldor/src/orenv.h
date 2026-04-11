#ifndef ORENV_H
#define ORENV_H
#include "axlobs.h"
#include "infenv.h"

typedef struct orEnv {
	List(InferEnv) envs;
} *OrEnv;

typedef struct orEnvIterator {
	List(InferEnv) li;
} OrEnvIterator;

#define orEnvITER(x, orEnv) orEnvIter(&x, orEnv)
#define orEnvSTEP(x) orEnvStep(&x)
#define orEnvMORE(x) orEnvMore(&x)
#define orEnvELT(x)  orEnvElt(&x)

void orEnvIter(OrEnvIterator *, OrEnv env);
void orEnvStep(OrEnvIterator *);
Bool orEnvMore(OrEnvIterator *);
InferEnv orEnvElt(OrEnvIterator *);

OrEnv	orEnvNewNone	(void);
OrEnv	orEnvOne	(InferEnv);
OrEnv 	orEnvFrTPoss	(TPoss);
Bool  	orEnvIsEmpty	(OrEnv);
Bool  	orEnvIsSingleton(OrEnv);
InferEnv orEnvSingleton	(OrEnv);

InferEnvList orEnvContent(OrEnv);

OrEnv orEnvAnd(OrEnv, OrEnv);

TPoss orEnvToTPoss(TForm, OrEnv);

void orEnvPrintDb(OrEnv);

#endif

