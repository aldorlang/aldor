#include "symcoinfo.h"
#include "store.h"

/******************************************************************************
 *
 * :: Phases
 *
 ****************************************************************************/

union symCoInfoU *
symCoInfoNew(void)
{
	union symCoInfoU * info;

	info = (union symCoInfoU *) stoAlloc(OB_Other, sizeof(*info));

	info->val.phaseVal.generic = 0;
	info->val.abTagVal	   = AB_LIMIT;
	info->val.foamTagVal	   = (FoamTag)FOAM_BVAL_LIMIT;

	return info;
}
