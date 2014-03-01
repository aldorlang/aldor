#ifndef _SYMCOINFO_H_
#define _SYMCOINFO_H_

#include "axlobs.h"
#include "foam.h"

/*
 * Symbol information for fast S-Expression IO of compiler types.
 */
union symCoInfoU {
	struct {
		union {
			Pointer	generic;
			AbSyn	macro;
		}     phaseVal;		/* phase varying info */

		AbSynTag abTagVal;
		FoamTag foamTagVal;
	} val;
	MostAlignedType 	align;	/* Force alignment. */
};

extern  union symCoInfoU * symCoInfoNew  (void);

#define symCoInfo(sym)     (&(((union symCoInfoU *) symInfo(sym))->val))
#define symCoInfoInit(sym) (symInfo(sym) = &(symCoInfoNew()->align))

#endif
