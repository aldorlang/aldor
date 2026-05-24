#ifndef _UVAR_H
#define _UVAR_H

#include "axlobs.h"

struct uvar {
	SymeList symes;
	SefoList conditions; // implicitly AND
	int refc;
};

/* NB: Swallows arguments */
UVar uvarNew(SymeList sl, SefoList conditions);
UVar uvarRef(UVar uvar);
void uvarUnref(UVar uvar);

SymeList uvarVars(UVar var);
SefoList uvarConditions(UVar var);

UVar uvarConstant(void);

#endif
