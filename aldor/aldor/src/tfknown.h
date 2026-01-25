#ifndef TFKNOWN_H
#define TFKNOWN_H
#include "axlobs.h"

extern AbLogic	abCondKnown;	  /* Conditions with known value (tinfer) */

extern void	ablogAndPush	  (AbLogic* glo,AbLogic* save,Sefo,Bool sense);
extern void	ablogAndPop	  (AbLogic* glo,AbLogic* save);

extern Bool ablogIsListKnown(SefoList sefolist);

#endif
