/*****************************************************************************
 *
 * symbol.c: Pooled symbols.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axlgen.h"

static Table	symbolPool = 0;
static long	genSymCount = 0;

void
symClear(void)
{
	tblFreeDeeply(symbolPool, (TblFreeKeyFun) 0, (TblFreeEltFun) stoFree);
	symbolPool = 0;
}

Symbol
symGen(void)
{
    static char gs[20];
    sprintf(gs, "GenSym #%ld",++genSymCount);
    return symIntern(gs);
}

Symbol
symProbe(String str, int options)
{
	Symbol	sym;

	if (!symbolPool)
		symbolPool = tblNew((TblHashFun) strHash, (TblEqFun) strEqual);

	sym = (Symbol) tblElt(symbolPool, str, NULL);
	if (sym || !(options & SYM_ALLOC)) return sym;

	sym = (Symbol) stoAlloc((unsigned) OB_Symbol, sizeof(*sym));
	sym->info = 0;
	sym->str  = (options & SYM_STRCOPY) ? strCopy(str) : str;

	tblSetElt(symbolPool, sym->str, sym);
	return sym;
}

int
symPrint(FILE *fout, Symbol sym)
{
	return fprintf(fout, "%s", sym != NULL ? sym->str : "<NULL>" );
}

void
symMap(void (*symfun)(Symbol))
{
	TableIterator it;

	for (tblITER(it,symbolPool); tblMORE(it); tblSTEP(it))
		symfun(tblELT(it));
}
