#ifndef ORENV_H
#define ORENV_H
#include "axlobs.h"
#include "infenv.h"

typedef struct orEnv {
	int serialNo;
	List(InferEnv) envs;
} *OrEnv;

typedef struct orEnvIterator {
	List(InferEnv) li;
} OrEnvIterator;

#define orEnvITER(x, orEnv) orEnvIter(&x, orEnv)
#define orEnvSTEP(x) orEnvStep(&x)
#define orEnvMORE(x) orEnvMore(&x)
#define orEnvELT(x)  orEnvElt(&x)

void orEnvIter(OrEnvIterator *, OrEnv);
void orEnvStep(OrEnvIterator *);
Bool orEnvMore(OrEnvIterator *);
InferEnv orEnvElt(OrEnvIterator *);

OrEnv	orEnvNewNone	(void);
OrEnv	orEnvOne	(InferEnv);
OrEnv 	orEnvFrTPoss	(TPoss);
Bool  	orEnvIsEmpty	(OrEnv);
Bool  	orEnvIsSingleton(OrEnv);
InferEnv orEnvSingleton	(OrEnv);
void 	orEnvAdd	(OrEnv, InferEnv);

InferEnvList orEnvContent(OrEnv);

OrEnv orEnvAnd(OrEnv, OrEnv);

TPoss orEnvToTPoss(TForm, OrEnv);

int orEnvOStreamWrite(OStream, OrEnv);
void orEnvPrintDb(OrEnv);

#endif

