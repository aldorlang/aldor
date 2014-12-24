#ifndef UTYPE0_H
#define UTYPE0_H

struct utype {
	Sefo sefo;
	UVar uvar;
};

// UTYPES
UType utypeNew(SymeList freevars, Sefo sefo);
UType utypeNewUVar(UVar uvar, Sefo sefo);
UType utypeNewConstant(Sefo sefo);
UType utypeNewVar(Syme syme);
Bool  utypeIsConstant(UType type);

UVar     utypeUVar(UType utype);
SymeList utypeVars(UType utype);
Sefo     utypeSefo(UType utype);


Bool utypeHasVar(UType, Syme);

int  utypeOStreamWrite(OStream ostream, UType utype);
int  utypePrint(FILE *fout, UType utype);
void utypePrintDb(UType utype);

#endif
