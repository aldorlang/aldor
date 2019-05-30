#include "uvar.h"
#include "store.h"

local void uvarFree(UVar uvar);

static struct uvar uvarConstant_val = { listNil(Syme), listNil(Sefo) };

UVar
uvarNew(SymeList sl, SefoList conditions)
{
	UVar uvar = (UVar) stoAlloc(OB_Other, sizeof(*uvar));

	uvar->symes = sl;
	uvar->conditions = conditions;
	uvar->refc = 1;

	return uvar;
}

UVar
uvarRef(UVar uvar)
{
	uvar->refc++;
	return uvar;
}

void
uvarUnref(UVar uvar)
{
	uvar->refc--;
	if (uvar->refc <= 0) {
		uvarFree(uvar);
	}
}

void
uvarFree(UVar uvar)
{
	listFree(Syme)(uvar->symes);
	listFree(Sefo)(uvar->conditions);
	stoFree(uvar);
}

SymeList
uvarVars(UVar var)
{
	return var->symes;
}

SefoList
uvarConditions(UVar var)
{
	return var->conditions;
}

UVar
uvarConstant()
{
	return &uvarConstant_val;
}
