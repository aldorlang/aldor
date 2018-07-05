#ifndef _JAVASIG_H_
#define _JAVASIG_H_

#include "foam.h"
#include "foamsig.h"

Foam javaSigNew(Foam retdecl, Foam exndecl, FoamList args);

FoamSig javaSigCreateFoamSig(Foam args);
int  javaSigArgc(Foam sig);
Foam javaSigArgN(Foam sig, int n);
Foam javaSigRet(Foam sig);
Foam javaSigExn(Foam sig);
int  javaSigToDDeclArgIdx(int n);


#endif
