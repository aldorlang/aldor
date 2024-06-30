#ifndef _SYMCOINFO_H_
#define _SYMCOINFO_H_

#include "axlobs.h"
#include "foam.h"

/*
 * Symbol information for fast S-Expression IO of compiler types.
 */
typedef struct {
	union {
		Pointer	generic;
		AbSyn	macro;
	}     phaseVal;		/* phase varying info */
	AbSynTag abTagVal;
	FoamTag foamTagVal;
} symCoInfoV, *SymCoInfoVal;

union symCoInfoU {
	symCoInfoV	val;
	MostAlignedType align;	/* Force alignment. */
};

extern  union symCoInfoU * symCoInfoNew  (void);

extern SymCoInfoVal symCoInfo(Symbol sym);

#define symCoInfoInit(sym) (symInfo(sym) = &(symCoInfoNew()->align))

#endif
