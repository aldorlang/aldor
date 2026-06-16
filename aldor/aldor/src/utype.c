#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "sefo.h"
#include "store.h"
#include "utype.h"
#include "uvar.h"


UType
utypeNewUVar(UVar uvar, Sefo sefo)
{
	UType utype = (UType) stoAlloc(OB_Other, sizeof(*utype));
	utype->sefo = sefo;
	utype->uvar = uvar;
	return utype;
}

UType
utypeNew(SymeList freevars, Sefo sefo)
{
	return utypeNewUVar(uvarNew(freevars, listNil(Sefo)), sefo);
}

SymeList
utypeVars(UType utype)
{
	return uvarVars(utype->uvar);;
}

UVar
utypeUVar(UType utype)
{
	return utype->uvar;
}

Sefo
utypeSefo(UType utype)
{
	return utype->sefo;
}

Bool
utypeHasVar(UType utype, Syme syme)
{
	return listMember(Syme)(utypeVars(utype), syme, symeEqual);
}

Bool
utypeIsConstant(UType utype)
{
	return utypeVars(utype) == listNil(Syme);
}

UType
utypeNewConstant(Sefo sefo)
{
	return utypeNewUVar(uvarConstant(), sefo);
}

UType
utypeNewVar(Syme syme)
{
	return utypeNew(listSingleton(Syme)(syme), abFrSyme(syme));
}

void
utypePrintDb(UType utype)
{
	utypePrint(dbOut, utype);
	fnewline(dbOut);
}

int
utypePrint(FILE *fout, UType utype)
{
	struct ostream os;
	int cc;

	ostreamInitFrFile(&os, fout);
	cc = utypeOStreamWrite(&os, utype);
	ostreamClose(&os);

	return cc;
}

int
utypeOStreamWrite(OStream ostream, UType utype)
{
	return ostreamPrintf(ostream, "[ForAll %pSymeList %pSefo]",
			     utypeVars(utype), utypeSefo(utype));

}

