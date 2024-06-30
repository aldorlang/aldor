#include "cport.h"
#include "store.h"
#include "ttable.h"

local PointerTSet ptrTSetCreate	(void);
local PointerTSet ptrTSetCreateCustom(TblHashFun, TblEqFun);
local PointerTSet ptrTSetEmpty	(void);
local void 	  ptrTSetFree	(PointerTSet);
local Length 	  ptrTSetSize	(PointerTSet);
local void 	  ptrTSetAdd	(PointerTSet, Pointer);
local void 	  ptrTSetAddAll	(PointerTSet, PointerList);
local void 	  ptrTSetRemove	(PointerTSet, Pointer);
local Bool 	  ptrTSetMember	(PointerTSet, Pointer);
local Bool 	  ptrTSetIsEmpty(PointerTSet);
local PointerTSetIter	ptrTSetIter(PointerTSet);
local PointerTSetIter	ptrTSetIterNext(PointerTSetIter);
local Pointer		ptrTSetIterElt(PointerTSetIter);
local Bool		ptrTSetIterHasNext(PointerTSetIter);
local void		ptrTSetIterDone(PointerTSetIter);

local PointerTSet ptrTSetEmptyVal;

CREATE_TSET(Pointer);

const struct TSetOpsStructName(Pointer) ptrTSetOps = {
	ptrTSetCreate,
	ptrTSetCreateCustom,
	ptrTSetFree,
	ptrTSetSize,
	ptrTSetAdd,
	ptrTSetAddAll,
	ptrTSetRemove,
	ptrTSetMember,
	ptrTSetIsEmpty,
	ptrTSetEmpty,
	ptrTSetIter,
	ptrTSetIterNext,
	ptrTSetIterElt,
	ptrTSetIterHasNext,
	ptrTSetIterDone,
};

local PointerTSet
ptrTSetCreate()
{
	PointerTSet tset = (PointerTSet) stoAlloc(OB_Other, sizeof(*tset));
	tset->table = tblNew(ptrHashFn, ptrEqualFn);
	return tset;
}

local PointerTSet
ptrTSetCreateCustom(TblHashFun hashfn, TblEqFun eqfn)
{
	PointerTSet tset = (PointerTSet) stoAlloc(OB_Other, sizeof(*tset));
	tset->table = tblNew(hashfn, eqfn);
	return tset;
}

local PointerTSet
ptrTSetEmpty()
{
	return ptrTSetCreate();
}

local void
ptrTSetFree(PointerTSet tset)
{
	if (tset == NULL) {
		return;
	}
	tblFree(tset->table);
	stoFree(tset);
}

local Length
ptrTSetSize(PointerTSet tset)
{
	return tblSize(tset->table);
}

local Bool
ptrTSetIsEmpty(PointerTSet tset)
{
	return tblSize(tset->table) == 0;
}

local Bool
ptrTSetMember(PointerTSet tset, Pointer ptr)
{
	return tblElt(tset->table, ptr, NULL) != NULL;
}

local void
ptrTSetAdd(PointerTSet tset, Pointer ptr)
{
	assert(ptr != NULL);
	tblSetElt(tset->table, ptr, ptr);
}

local void
ptrTSetAddAll(PointerTSet tset, PointerList ptrlist)
{
	while (ptrlist != listNil(Pointer)) {
		tblSetElt(tset->table, car(ptrlist), car(ptrlist));
		ptrlist = cdr(ptrlist);
	}
}

local void
ptrTSetRemove(PointerTSet tset, Pointer ptr)
{
	tblDrop(tset->table, ptr);
}

local PointerTSetIter
ptrTSetIter(PointerTSet tset)
{
	PointerTSetIter tsetIter = (PointerTSetIter) stoAlloc(OB_Other, sizeof(*tsetIter));
	tblITER(tsetIter->iter, tset->table);

	return tsetIter;
}

local PointerTSetIter
ptrTSetIterNext(PointerTSetIter tsetIter)
{
	tblSTEP(tsetIter->iter);
	return tsetIter;
}

local Pointer
ptrTSetIterElt(PointerTSetIter tsetIter)
{
	return tblKEY(tsetIter->iter);;
}

local Bool
ptrTSetIterHasNext(PointerTSetIter tsetIter)
{
	Bool res = tblMORE(tsetIter->iter);
	if (!res)
		stoFree(tsetIter);
	return res;
}

local void
ptrTSetIterDone(PointerTSetIter tsetIter)
{
	stoFree(tsetIter);
}
