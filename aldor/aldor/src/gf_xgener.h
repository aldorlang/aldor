#ifndef _GF_XGENER_H_
#define _GF_XGENER_H_

#include "axlobs.h"

extern Foam     gen0XGenerate   (AbSyn);
extern Foam	gen0XYield	(AbSyn absyn);
extern Foam	gen0XGenCall	(AbSyn forIter);
extern AInt	gen0MakeXGenStepFnFmt(void);

#endif
