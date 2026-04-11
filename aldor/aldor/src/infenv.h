#ifndef INFENV_H
#define INFENV_H
#include "axlobs.h"
#include "ufind.h"

DECLARE_LIST(InferEnv);

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
Bool     infEnvIsImmutable(InferEnv);

Bool	 infEnvExtend(InferEnv, TForm, TForm);

TForm infEnvFollow(InferEnv env, TForm tf);

int infEnvOStreamWrite(OStream, InferEnv);

Bool infEnvEqual(InferEnv e1, InferEnv e2);

typedef struct uinfo *UInfo;
TForm varInfoTForm(UInfo info);

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
#define infEnvREP(x)  uftREP(x.uftIter)

void _infEnvITER(InferEnvIterator *, InferEnv);
TForm _infEnvELT(InferEnvIterator *);

#endif
