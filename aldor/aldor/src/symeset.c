#include "symeset.h"
#include "sefo.h"
#include "store.h"
#include "ttable.h"
#include "format.h"

SymeSet
symeSetFrSymes(SymeList symes)
{
	SymeSet set = (SymeSet) stoAlloc(OB_SymeSet, sizeof(*set));
	Syme syme;

	set->names = tsetCreate(Symbol)();
	set->symes = listNil(Syme);
	set->symes = symes;

	while (symes != listNil(Syme)) {
		tsetAdd(Symbol)(set->names, symeId(symes->first));
		symes = cdr(symes);
	}

	return set;
}

void
symeSetFree(SymeSet symeSet)
{
	tsetFree(Symbol)(symeSet->names);
	listFree(Syme)(symeSet->symes);
	stoFree(symeSet);
}

Bool
symeSetIsEmpty(SymeSet symeSet)
{
	if (tsetIsEmpty(Symbol)(symeSet->names))
		return true;
	return listNil(Syme) == symeSet->symes;
}

Bool
symeSetMember(SymeSet symeSet, Syme syme)
{
	if (!tsetMember(Symbol)(symeSet->names, symeId(syme)))
		return false;
	return listMember(Syme)(symeSet->symes, syme, symeEqual);
}

Bool
symeSetMayHave(SymeSet symeSet, Symbol sym)
{
	return tsetMember(Symbol)(symeSet->names, sym);
}

SymeList
symeSetList(SymeSet symeSet)
{
	return symeSet->symes;
}

SymeList
symeSetSymesForSymbol(SymeSet symeSet, Symbol symbol)
{
	SymeList sl = listNil(Syme);
	SymeList tmp;
	if (!tsetMember(Symbol)(symeSet->names, symbol))
		return listNil(Syme);

	tmp = symeSet->symes;
	while (tmp != listNil(Syme)) {
		if (symbol == symeId(car(tmp)))
			sl = listCons(Syme)(car(tmp), sl);
		tmp = cdr(tmp);
	}

	return listNReverse(Syme)(sl);
}

int
symeSetFormat(OStream ostream, SymeSet symeSet)
{
	SymeList sl = symeSet->symes;
	return ostreamPrintf(ostream, "{S: %pSymeList}", sl);
}
