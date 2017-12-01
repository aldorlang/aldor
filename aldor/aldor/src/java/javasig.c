#include "javasig.h"

FoamSig
javaSigCreateFoamSig(Foam sig)
{
	AIntList inArgs;
	FoamTag retType;

	inArgs = listNil(AInt);
	for (int i=0; i<javaSigArgc(sig); i++) {
		Foam decl = javaSigArgN(sig, i);
		inArgs = listCons(AInt)(decl->foamDecl.type, inArgs);
	}
	inArgs = listNReverse(AInt)(inArgs);
	return foamSigNew(inArgs, javaSigRet(sig)->foamDecl.type, 0, NULL);
}


