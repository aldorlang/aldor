#ifndef INFENV_H
#define INFENV_H
#include "axlobs.h"
#include "ufind.h"

typedef struct unifyError *UnifyError;

DECLARE_LIST(InferEnv);
DECLARE_LIST(UnifyError);

InferEnv infEnvFailed	(void);
InferEnv infEnvEmpty	(void);
InferEnv infEnvNew	(void);

// Need refcounting...
//void	infEnvFree	(InferEnv);

// TODO: Copy on write version of these
InferEnv infEnvCopy	(InferEnv);

InferEnv infEnvSatisfies  (InferEnv, TForm, TForm);
InferEnv infEnvMerge	  (InferEnv, InferEnv);
Bool     infEnvIsEmpty	  (InferEnv);
Bool     infEnvIsFailed	  (InferEnv);
Bool     infEnvIsFailedVar(InferEnv, TForm);
Bool     infEnvIsImmutable(InferEnv);

Bool	 infEnvExtend(InferEnv, TForm, TForm);

TForm infEnvFollow(InferEnv env, TForm tf);

int infEnvOStreamWrite(OStream, InferEnv);

Bool infEnvEqual(InferEnv e1, InferEnv e2);

//InferEnv infEnvAdd	(InferEnv, TForm, TForm);
/**
 * Debug
 */
void infEnvPrintDb(InferEnv);

/**
 * Sets tf1 to tf2; tf1 must be a variable.
 */
void     infEnvSet	(InferEnv, TForm, TForm);
InferEnv infEnvMerge	(InferEnv, InferEnv);


/**
 * Updates each contained tf with its determined value
 */
void	 infEnvCommit       (InferEnv);
void	 infEnvSetImmutable (InferEnv);

/**
 * Iteration
 */

typedef struct inferEnvIterator InferEnvIterator;

struct inferEnvIterator {
	InferEnv env;
	UFTIterator uftIter;
};

#define infEnvITER(x, infEnv) _infEnvITER(&x, (infEnv))
#define infEnvMORE(x) uftMORE(x.uftIter)
#define infEnvSTEP(x) uftSTEP((x).uftIter)
#define infEnvKEY(x)  uftKEY(x.uftIter)
#define infEnvELT(x)  _infEnvELT(&x)
#define infEnvFAIL(x) _infEnvFAIL(&x)
#define infEnvREP(x)  uftREP(x.uftIter)

void _infEnvITER(InferEnvIterator *, InferEnv);
TForm _infEnvELT(InferEnvIterator *);
UnifyErrorList _infEnvFAIL(InferEnvIterator *);

#endif
