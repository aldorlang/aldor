#ifndef _FOAMSIG_H_
#define _FOAMSIG_H_
#include "axlobs.h"
/* 
 * For re-using boiler-plate code
 * nRets == NULL indicates that all return values
 * are FOAM_Words.
 */
typedef struct foamSig {
	AIntList inArgs;
	int	 nRets;
	AInt     retType;
	FoamTag *rets;
	/* These are filled in as needed */
	int 	 constNum;
	Foam	 ref;
} *FoamSig;

DECLARE_LIST(FoamSig);

void	foamSigFree	(FoamSig);
Bool	foamSigEqual	(FoamSig, FoamSig);
Bool	foamSigEqualModFmt(FoamSig, FoamSig);
FoamSig foamSigNew      (AIntList inArgs, FoamTag retType, int nRets, FoamTag *rets);
Hash    foamSigHash     (FoamSig s);

#endif
