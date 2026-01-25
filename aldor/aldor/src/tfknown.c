#include "tfknown.h"
#include "ablogic.h"
#include "debug.h"
#include "util.h"

Bool	tfKnownDebug	= false;
#define tfKNownDEBUG	DEBUG_IF(tfKnown)	afprintf

AbLogic	abCondKnown = NULL;	  /* Conditions with known value (tinfer) */


/******************************************************************************
 *
 * :: Maintaining an And-stack.
 *
 *****************************************************************************/

void
ablogAndPush(AbLogic *glo, AbLogic *save, Sefo cond, Bool sense)
{
	AbLogic thisCond;

	/* Save the old value of *glo and compute the new one. */
	*save = *glo;

	thisCond = ablogFrSefo(cond);
	if (!sense) {
		AbLogic tt = thisCond;
		thisCond   = ablogNot(tt);
		ablogFree(tt);
	}

	*glo = ablogAnd(*save, thisCond);
	ablogFree(thisCond);

	if (DEBUG(tfKnown)) {
		fprintf(dbOut, ">> Changed condition to ");
		ablogPrint(dbOut, *glo);
		fnewline(dbOut);
	}
}

void
ablogAndPop(AbLogic *glo, AbLogic *save)
{
	ablogFree(*glo);
	*glo = *save;

	if (DEBUG(tfKnown)) {
		fprintf(dbOut, "<< Changed back\n");
	}
}

/*
Bool
ablogIsListKnown(SefoList sefolist)
{
	AbLogic new  = ablogCopy(abCondKnown);
	Bool res;
	res = ablogIsListImpliedInner(abCondKnown, sefolist, &new);
	return res;
}

*/

Bool
ablogIsListKnown(SefoList sefolist)
{
	return ablogIsListImplied(abCondKnown, sefolist);
}
