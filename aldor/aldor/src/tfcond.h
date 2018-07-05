#ifndef _TFCOND_H_
#define _TFCOND_H_

#include "axlobs.h"
#include "ablogic.h"

typedef struct tfCondElt {
	Stab stab;
	AbSynList list;
	AbLogic known;
} *TfCondElt;

DECLARE_LIST(TfCondElt);

typedef struct tfCond {
	Bool          containsEmpty;
	TfCondEltList conditions;
	AbLogic       known;
} *TfCond;

TfCond tfCondNew(void);
TfCondElt tfCondEltNew(Stab stab, AbSynList absynList);
TfCondElt tfCondEltNewKnown(Stab stab, AbLogic known);

void   tfCondSetCondition(TfCond cond, AbSynList list);
TfCond tfCondFloat(Stab stab, TfCond tfcond);
TfCond tfCondMerge(TfCond c1, Stab stab, TfCondElt condition);

ULong		tfOuterDepth		(Stab, TForm);
#endif
