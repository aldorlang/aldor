#include "ti_top.h"

static TiTopLevel tiTopLevel;

void
tiTopLevelInit(TiTopLevel fns)
{
	tiTopLevel = fns;
}

void
tiTopLevelFini()
{
	tiTopLevel = NULL;
}

TiTopLevel
tiTopFns()
{
	return tiTopLevel;
}

