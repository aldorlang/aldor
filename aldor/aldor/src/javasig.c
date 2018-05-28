#include "foam.h"
#include "syme.h"
#include "javasig.h"
#include "strops.h"

Foam
javaSigNew(Foam retdecl, Foam exndecl, FoamList args)
{
	Foam nodecl = foamNewDecl(FOAM_NOp, strCopy("extra"), int0);

	return foamNewDDeclOfList(FOAM_DDecl_JavaSig,
				  listNConcat(Foam)(listList(Foam)(3, nodecl, retdecl, exndecl),
						    args));

}

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

int
javaSigArgc(Foam sig)
{
	assert(sig->foamDDecl.usage == FOAM_DDecl_JavaSig);
	return foamDDeclArgc(sig) - 3;
}

Foam
javaSigArgN(Foam sig, int n)
{
	assert(sig->foamDDecl.usage == FOAM_DDecl_JavaSig);
	return sig->foamDDecl.argv[n+3];
}

Foam
javaSigRet(Foam sig)
{
	assert(sig->foamDDecl.usage == FOAM_DDecl_JavaSig);
	return sig->foamDDecl.argv[1];
}

Foam
javaSigExn(Foam sig)
{
	assert(sig->foamDDecl.usage == FOAM_DDecl_JavaSig);
	return sig->foamDDecl.argv[2];
}

int
javaSigToDDeclArgIdx(int n)
{
	return n+3;
}

