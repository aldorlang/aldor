#include "ufind.h"
#include "table.h"
#include "tform.h"
#include "cport.h"
#include "int.h"
#include "store.h"

struct ufElt {
	AInt    id; // Not strictly needed, but might be helpful
	UFElt   parent;
	AInt    count;
	Pointer data;
	TForm   tf;
};

local UFElt uftCreateElt	(TForm);
local long  utfHashVarFun	(TForm);
local Bool  utfEqVarFun		(TForm, TForm);

local UFTable uftEmpty = NULL;

UFTable
uftNew()
{
	UFTable tbl = (UFTable) stoAlloc(OB_Other, sizeof(*tbl));
	tbl->elts = tblNew((TblHashFun) utfHashVarFun, (TblEqFun) utfEqVarFun);

	return tbl;
}

UFTable
uftCopy(UFTable tbl)
{
	UFTable newTbl = (UFTable) stoAlloc(OB_Other, sizeof(*tbl));
	newTbl->elts = tblCopy(tbl->elts);

	return newTbl;
}

UFTable
uftTheEmpty()
{
	if (uftEmpty == NULL) {
		uftEmpty = uftNew();
	}
	return uftEmpty;
}

Bool
uftIsEmpty(UFTable tbl)
{
	return tblIsEmpty(tbl->elts);
}


local long
utfHashVarFun(TForm tf)
{
	assert(tfIsVar(tf) || tfIsInfSubst(tf));
	return aintHash(tf->varId);
}

local Bool
utfEqVarFun(TForm tf1, TForm tf2)
{
	assert(tfIsVar(tf1) || tfIsInfSubst(tf1));
	assert(tfIsVar(tf2) || tfIsInfSubst(tf2));

	return tf1->varId == tf2->varId;
}

void
uftUnion(UFTable tbl, TForm id1, TForm id2)
{
	UFElt uf1 = uftGet(tbl, id1);
	UFElt uf2 = uftGet(tbl, id2);
	assert(uf1->parent == NULL);
	assert(uf2->parent == NULL);

	if (uf1 == uf2) {
		return;
	}

	if (uftEltCount(uf1) < uftEltCount(uf2)) {
		uf1->parent = uf2;
		uf2->count += uf1->count;
	}
	else {
		uf2->parent = uf1;
		uf1->count += uf2->count;
	}
}

Length
uftSize(UFTable tbl)
{
	return tblSize(tbl->elts);
}

AInt
uftEltCount(UFElt elt)
{
	return elt->count;
}

UFElt
uftProbe(UFTable table, TForm tf)
{
	UFElt elt = (UFElt) tblElt(table->elts, (TblKey) tf, NULL);
	return uftEltFollow(elt);
}

UFElt
uftEltFollow(UFElt elt)
{
	UFElt elt0 = elt;
	if (elt == NULL) {
		return elt;
	}
	while (elt->parent != NULL) {
		elt = elt->parent;
	}
	while (elt0->parent != NULL) {
		elt0->parent = elt;
		elt0 = elt0->parent;
	}

	return elt;
}

UFElt
uftGet(UFTable table, TForm id)
{
	UFElt elt = uftProbe(table, id);
	if (elt == NULL) {
		elt = uftCreateElt(id);
		tblSetElt(table->elts, (TblKey) id, elt);
	}
	return elt;
}

TForm
uftEltTForm(UFElt elt)
{
	return elt->tf;
}

void
uftSetData(UFTable tbl, TForm tf, Pointer data)
{
	UFElt uf = uftGet(tbl, tf);

	uf->data = data;
}

Pointer
uftEltData(UFElt elt)
{
	if (elt == NULL) {
		return NULL;
	}
	return elt->data;
}


local UFElt
uftCreateElt(TForm tf)
{
	UFElt elt = (UFElt) stoAlloc(OB_Other, sizeof(*elt));
	//elt->id = id;
	elt->parent = NULL;
	elt->count = 1; // Just me
	elt->data = NULL;
	elt->tf = tf;

	return elt;
}

void
_uftITER(UFTIterator *iter, UFTable uft)
{
	tblITER(iter->tblIter, uft->elts);
}
