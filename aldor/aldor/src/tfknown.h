#ifndef TFKNOWN_H
#define TFKNOWN_H
#include "axlobs.h"

extern AbLogic	abCondKnown;	  /* Conditions with known value (tinfer) */
extern InferEnv tfkInfEnvKnown;

extern void	ablogAndPush	(AbLogic* glo,AbLogic* save,Sefo,Bool sense);
extern void	ablogAndPop	(AbLogic* glo,AbLogic* save);

extern Bool ablogIsListKnown(SefoList sefolist);

extern void tfkSetEnv		(InferEnv env);
extern void tfkClearEnv		(void);
extern Bool tfkAddSatConstraint	(TForm v, TForm S, TForm T);

extern Bool tfkHasInferEnv	(void);
#endif
