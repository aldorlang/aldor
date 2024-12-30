#include "csig.h"
#include "strops.h"
#include "util.h"

Foam
csigNew(FoamList args, FoamList rets)
{
	Foam ddecl;
	Length i, argc, retc;

	argc = listLength(Foam)(args);
	retc = listLength(Foam)(rets);

	ddecl = foamNewDDecl(FOAM_DDecl, 1 + argc + 1 + retc);
	ddecl->foamDDecl.usage = FOAM_DDecl_CSig;

	i = 0;
	listIter(Foam, argDecl, args, {
		ddecl->foamDDecl.argv[i++] = argDecl;
		});
	// * Add a marker to separate args and return
	ddecl->foamDDecl.argv[i++] = foamNewDDecl(FOAM_Nil, strCopy(""), emptyFormatSlot);

	listIter(Foam, retDecl, rets, {
			ddecl->foamDDecl.argv[i++] = retDecl;
		});
	return ddecl;
}

AInt
csigArgc(Foam foam)
{
	int i;
	for (i=0; i<foamDDeclArgc(foam); i++) {
		if (foam->foamDDecl.argv[i]->foamDecl.type == FOAM_Nil) {
			return i;
		}
	}
	bug("no gap in csig");	
}

Foam
csigArgN(Foam foam, Length n)
{
	return foam->foamDDecl.argv[n];
}

AInt
csigRetc(Foam foam)
{
	int argc = csigArgc(foam);
	return foamDDeclArgc(foam) - argc - 1;
}

Foam
csigRetN(Foam foam, Length n)
{
	int pos = csigArgc(foam);
	return foam->foamDDecl.argv[pos + 1 + n];
}
