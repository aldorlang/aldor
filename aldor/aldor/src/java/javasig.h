#ifndef _JAVASIG_H_
#define _JAVASIG_H_

#include "foam.h"
#include "foamsig.h"
/*
 * Special macros for FOAM_DDECL_JavaSig
 */
#define javaSigArgc(ddecl) (foamDDeclArgc(ddecl) - 1)
#define javaSigArgN(ddecl, n) ((ddecl)->foamDDecl.argv[n+1])
#define javaSigRet(ddecl) ((ddecl)->foamDDecl.argv[0])

FoamSig javaSigCreateFoamSig(Foam args);


#endif

