
#include "foamsig.h"

local Bool aintEqual(AInt i1, AInt i2);

FoamSig
foamSigNew(AIntList inArgs, FoamTag retType, int nRets, FoamTag *rets)
{
	FoamSig new;

	new = (FoamSig) stoAlloc(OB_Other, sizeof(*new));
	
	new->inArgs   = listCopy(AInt)(inArgs);
	new->retType  = retType;
	new->nRets    = nRets;
	new->rets     = rets;
	new->constNum = -1;
	new->ref      = NULL;

	return new;
}

void
foamSigFree(FoamSig sig)
{
	if (sig->rets)
		stoFree(sig->rets);

	stoFree(sig);
}

Bool
foamSigEqual(FoamSig sig1, FoamSig sig2)
{
	int i;
	
	if (sig1->retType != sig2->retType)
		return false;

	if (sig1->nRets != sig2->nRets)
		return false;
	
	if (!listEqual(AInt)(sig1->inArgs, sig2->inArgs, aintEqual))
		return false;

	if (sig1->rets != sig2->rets) {
		if (sig1->rets == NULL || sig2->rets == NULL)
			return false;
		for (i=0; i<sig1->nRets; i++)
			if (sig1->rets[i] != sig2->rets[i])
				return false;
	}

	return true;
}

Hash
foamSigHash(FoamSig s1)
{
	AIntList ltmp;
	Hash h = s1->retType;;
	ltmp = s1->inArgs;
	while (ltmp) {
		h = hashCombine(h, car(ltmp));
		ltmp = cdr(ltmp);
	}
	return h;
}


local Bool
aintEqual(AInt i1, AInt i2)
{
	return i1 == i2;
}
