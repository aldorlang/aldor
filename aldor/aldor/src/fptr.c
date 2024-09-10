#include "fptr.h"
#include "store.h"

CREATE_LIST(FoamPtr);

PtrSet
psetNew(void)
{
	PtrSet pset = (PtrSet) stoAlloc(OB_Other, sizeof(*pset));
	pset->count = 0;
	pset->tbl = tsetCreate(Pointer)();
	return pset;
}

Bool
psetIsEmpty(PtrSet pset)
{
	return pset->count == 0;
}

FoamPtr
fptrNew(PtrSet pset, Foam foam, AInt posn)
{
	FoamPtr ptr;
	assert(posn < foamArgc(foam));
	ptr = (FoamPtr) stoAlloc(OB_Other, sizeof(*ptr));
	ptr->pset = pset;
	ptr->context = foam;
	ptr->posn = posn;
	pset->count = pset->count + 1;

	tsetAdd(Pointer)(pset->tbl, (Pointer) ptr);
	return ptr;
}

void
fptrFree(FoamPtr ptr)
{
	tsetRemove(Pointer)(ptr->pset->tbl, (Pointer) ptr);
	ptr->pset->count--;
	stoFree(ptr);
}

Foam
fptrVal(FoamPtr ptr)
{
	return foamArgv(ptr->context)[ptr->posn].code;
}

void
fptrSet(FoamPtr ptr, Foam value)
{
	foamArgv(ptr->context)[ptr->posn].code = value;
}
