
#include "foamsig.h"
#include "store.h"
#include "util.h"
#include "int.h"

local Bool foamSigEqual0(FoamSig sig1, FoamSig sig2, Bool ignoreFmt);
local Bool foamSigEqualArgs(AIntList args1, AIntList args2, Bool ignoreFmt);

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
	return foamSigEqual0(sig1, sig2, false);
}

Bool
foamSigEqualModFmt(FoamSig sig1, FoamSig sig2)
{
	return foamSigEqual0(sig1, sig2, true);
}

local Bool
foamSigEqual0(FoamSig sig1, FoamSig sig2, Bool ignoreFmt)
{
	int i;
	if (sig1->retType != sig2->retType)
		return false;

	if (sig1->nRets != sig2->nRets)
		return false;
	
	if (!foamSigEqualArgs(sig1->inArgs, sig2->inArgs, ignoreFmt))
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

local Bool
foamSigEqualArgs(AIntList args1, AIntList args2, Bool ignoreFmt)
{
	if (!ignoreFmt)
		return listEqual(AInt)(args1, args2, aintEqual);

	while (args1 != listNil(AInt) && args2 != listNil(AInt)) {
		if ((car(args1) & 0xFF) != (car(args2) & 0xFF))
			return false;
		args1 = cdr(args1);
		args2 = cdr(args2);
	}
	return args1 == args2;
}

Hash
foamSigHash(FoamSig s1)
{
	AIntList ltmp;
	Hash h = s1->retType;;
	ltmp = s1->inArgs;
	while (ltmp) {
		h = hashCombinePair(h, car(ltmp));
		ltmp = cdr(ltmp);
	}
	return h;
}
