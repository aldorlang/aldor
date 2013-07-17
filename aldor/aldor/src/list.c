/*****************************************************************************
 *
 * list.c: List operations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "format.h"
#include "list.h"
#include "store.h"
#include "strops.h"

typedef void    (*PointerListEltFreeFun)(Pointer);
typedef Pointer (*PointerListEltFun)    (Pointer);
typedef Bool    (*PointerListEltEqFun)  (Pointer, Pointer);
typedef int     (*PointerListEltPrFun)  (FILE *, Pointer);
typedef Bool	(*PointerListEltSatFun)	(Pointer);

/*
 * Create a new list with unique element x.
 */
local PointerList
ptrlistSingleton(Pointer x)
{
	PointerList     t;
	t = (PointerList) stoAlloc((unsigned) OB_List, sizeof(*t));
	t->first = x;
	t->rest  = (PointerList) 0;
	return t;
}

local PointerList
ptrlistList(int n, ...)
{
	va_list  argp;
	int i;
	PointerList head = listNil(Pointer);
	PointerList *nextptr = &head;
	PointerList tail;

	va_start(argp, n);
	for (i=0; i<n; i++) {
		PointerList cell = listCons(Pointer)((Pointer) va_arg(argp, Pointer), 
						     listNil(Pointer));
		*nextptr = cell;
		nextptr = &cell->rest;
	}
	va_end(argp);

	return head;
}

local PointerList
ptrlistListNull(Pointer arg0, ...)
{
	PointerList l = listNil(Pointer);
	va_list argp;

	va_start(argp, arg0);
	if (arg0 != NULL) {
		l = listListv(Pointer)(argp);
		l = listCons(Pointer)(arg0, l);
	}
	va_end(argp);

	return l;
}

local PointerList
ptrlistListv(va_list argp)
{
	int i;
	PointerList head = listNil(Pointer);
	PointerList *nextptr = &head;

	while (true) {
		Pointer nextElt = (Pointer) va_arg(argp, Pointer);
		PointerList cell;
		if (nextElt == NULL)
			break;
		cell = listCons(Pointer)(nextElt,
						     listNil(Pointer));
		*nextptr = cell;
		nextptr = &cell->rest;
	}

	return head;
}

/*
 * Create a new list with first element x, and tail l.  The tail is shared.
 */
local PointerList
ptrlistCons(Pointer x, PointerList l)
{
	PointerList     t;
	t = (PointerList) stoAlloc((unsigned) OB_List, sizeof(*t));
	t->first = x;
	t->rest  = l;
	return t;
}

/*
 * Compare the two lists for equality using `eq' as the equality test
 * for the respective members of the list.
 */
local Bool
ptrlistEqual(PointerList l1, PointerList l2, PointerListEltEqFun eq)
{
	for ( ; l1 && l2; l1 = l1->rest, l2 = l2->rest)
		if (!(*eq)(l1->first, l2->first))
			return false;
	return l1 == l2;
}

/*
 * Determine whether x is in l using `eq' as the equality test. 
 * If not, set pos = -1. In this case, the value returned is 0, but
 * it is pos that should be tested for a match.
 */
local Pointer
ptrlistFind(PointerList l, Pointer x, PointerListEltEqFun eq, int *pos)
{
	int     i;
	for (i = 0; l; l = l->rest, i++)
		if ((*eq)(l->first, x)) {
			*pos = i;
			return l->first;
		}
	*pos = -1;
	return 0;
}

/*
 * Free just the first cons cell, return the former tail.
 */
local PointerList
ptrlistFreeCons(PointerList l)
{
	PointerList     t;
	if (l) {
		t = cdr(l);
		stoFree((Pointer) l);
	}
	else
		t = (PointerList) 0;
	return t;
}

/*
 * Free the cons cells in the list until a given tail, or the end is reached.
 * The fate of the individual entries is determined by `eltfree'. 
 * The unfreed tail is returned.
 */
local PointerList
ptrlistFreeDeeplyTo(PointerList l, PointerList tail,
	            PointerListEltFreeFun eltfree)
{
	PointerList     t;

	while (l && l != tail) {
		t = l->rest;
		if (eltfree) (*eltfree)(l->first);
		stoFree((Pointer) l);
		l = t;
	}
	return l;
}

local void
ptrlistFreeDeeply(PointerList l, PointerListEltFreeFun eltfree)
{
	ptrlistFreeDeeplyTo(l, (PointerList) 0, eltfree);
}

local PointerList
ptrlistFreeTo(PointerList l, PointerList tail)
{
	return ptrlistFreeDeeplyTo(l, tail, (void (*) (Pointer)) 0);
}

local void
ptrlistFree(PointerList l)
{
	ptrlistFreeDeeplyTo(l, (PointerList) 0, (void (*) (Pointer)) 0);
}

/* Discard all elements in l that satisfy cond. eltFree can be null, eltSat no.
 * Returns the new list.
 */
local PointerList
ptrlistFreeIfSat(PointerList l, PointerListEltFreeFun eltFree,
		 PointerListEltSatFun eltSat)
{
	PointerList	l0 = l;
	PointerList	* pred = &l;	

	while (l0) {
		if ((*eltSat)(l0->first)) {
			if (eltFree) (*eltFree)(l0->first);
			l0 = ptrlistFreeCons(l0);
			*pred = l0;
		}
		else {
			pred = &(l0->rest);
			l0 = l0->rest;
		}
	}

	return l;
}

/*
 * The n-th tail of l, obtained by dropping n elements from the front.
 * ptrlistDrop(l,1) == cdr(l).
 */
local PointerList
ptrlistDrop(PointerList l, Length n)
{
	for ( ; l && n > 0; l = l->rest, n--)
		;
	assert(n == 0);
	return l;
}

/*
 * The n-th element of l.
 * ptrlistElt(l, 0) == car(l).
 */
local Pointer
ptrlistElt(PointerList l, Length n)
{
	l = ptrlistDrop(l,n);
	assert(l != 0);
	return l->first;
}

/*
 * A pointer to the last cons of the list.
 * If the list is empty then this is 0.
 * E.g. x := cons(c,_nil); l := cons(a,cons(b,x))
 *      lastCons(l) == x.
 */
local PointerList
ptrlistLastCons(PointerList l)
{
	if (!l) return 0;

	while (l->rest) l = l->rest;
	return l;
}

/*
 * Length of l as the number of cons cells.
 */
local Length
ptrlistLength(PointerList l)
{
	Length     i;

	for (i = 0; l; l = l->rest, i++)
		;
	return i;
}

/*
 * True iff length l == n
 */
local Bool
ptrlistIsLength(PointerList l, Length n)
{
	for ( ; l && n > 0; l = l->rest, n--)
		;
	return (!l && n == 0);
}

/*
 * True iff length l > n
 */
local Bool
ptrlistIsLonger(PointerList l, Length n)
{
	for ( ; l && n > 0; l = l->rest, n--)
		;
	return (l && n == 0);
}

/*
 * True iff length l < n
 */
local Bool
ptrlistIsShorter(PointerList l, Length n)
{
	for ( ; l && n > 0; l = l->rest, n--)
		;
	return (!l && n > 0);
}

local PointerList	ptrlistNReverse		(PointerList l);

/*
 * Copy the cons cells in the list until a given tail, or the end is reached.
 * The elements of the list are copied using eltcopy.
 */
local PointerList
ptrlistCopyDeeplyTo(PointerList l, PointerList tail,
		    PointerListEltFun eltcopy)
{
	PointerList	r = 0;

	for ( ; l && l != tail; l = l->rest)
		if (eltcopy)
			r = ptrlistCons((*eltcopy)(l->first), r);
		else
			r = ptrlistCons(l->first, r);

	return ptrlistNReverse(r);
}

/*
 * Produce a copy of the list l.  The elements are copied using eltcopy.
 */
local PointerList
ptrlistCopyDeeply(PointerList l, PointerListEltFun eltcopy)
{
	return ptrlistCopyDeeplyTo(l, (PointerList) 0, eltcopy);
}

/*
 * Copy the cons cells in the list until a given tail, or the end is reached.
 * The elements of the list are not copied.
 */
local PointerList
ptrlistCopyTo(PointerList l, PointerList tail)
{
	return ptrlistCopyDeeplyTo(l, tail, (Pointer (*) (Pointer)) 0);
}

/*
 * Produce a copy of the list l.  The elements are not copied.
 */
local PointerList
ptrlistCopy(PointerList l)
{
	return ptrlistCopyDeeplyTo(l, (PointerList) 0,
				   (Pointer (*) (Pointer)) 0);
}

/*
 * Modify l: l.i = f(l.i) for i in 0..#l1-1
 */
local PointerList
ptrlistNMap(PointerListEltFun f, PointerList l)
{
	PointerList     t;

	for (t = l; t; t = t->rest)
		t->first = (*f)(t->first);
	return l;
}

/*
 * Compute new list l = [f(e) for e in l]
 */
local PointerList
ptrlistMap(PointerListEltFun f, PointerList l)
{
	return ptrlistNMap(f, ptrlistCopy(l));
}

/*
 * Modify the input list by reversing the `rest' links and returning a 
 * pointer to the former `lastCons'.
 * The net effect is to have the elements in reverse order.             
 */
local PointerList
ptrlistNReverse(PointerList l)
{
	PointerList     r, t;

	r = 0;
	while(l) {
		t = l->rest;
		l->rest = r;
		r = l;
		l = t;
	}
	return r;
}

/*
 * A new list containing the same elements in the reverse order.        
 */
local PointerList
ptrlistReverse(PointerList l)
{
	return ptrlistNReverse(ptrlistCopy(l));
}

/*
 * Modify the lastCons of l1 to point to l2 so that the result is a list 
 * containing all the elements of l1, followed by the elements of l2.           
 */
local PointerList
ptrlistNConcat(PointerList l1, PointerList l2)
{
	if (!l1) return l2;
	ptrlistLastCons(l1)->rest = l2;
	return l1;
}

/*
 * A new list containing first the elements of l1, followed by those of l2.
 * l2 is shared by the result.  
 */
local PointerList
ptrlistConcat(PointerList l1, PointerList l2)
{
	return ptrlistNConcat(ptrlistCopy(l1), l2);
}

/*
 * Return the position of e in l using `==' as the equality test.
 * If e is not there, -1 is returned.   
 */
local int
ptrlistPosq(PointerList l, Pointer x)
{
	Length     i;
	for (i = 0; l; l = l->rest, i++)
		if (l->first == x) return i;
	return -1;
}

/*
 * Return the position of e in l using `eq' as the equality test.
 * If e is not there, -1 is returned.   
 */
local int
ptrlistPosition(PointerList l, Pointer x, PointerListEltEqFun eq)
{
	Length     i;
	for (i = 0; l; l = l->rest, i++)
		if ((*eq)(l->first, x)) return i;
	return -1;
}

/*
 * Determine whether e is in l using `==' as the equality test. 
 */
local Bool
ptrlistMemq(PointerList l, Pointer x)
{
	return ptrlistPosq(l,x) != -1;
}

/*
 * Determine whether e is in l using `eq' as the equality test. 
 */
local Bool
ptrlistMember(PointerList l, Pointer x, PointerListEltEqFun eq)
{
	return ptrlistPosition(l,x,eq) != -1;
}

/*
 * Remove the first occurance of x from the list l, freeing the link and
 * modifying l.
 */
local PointerList
ptrlistNRemove(PointerList l, Pointer x, PointerListEltEqFun eq)
{
	PointerList	p, t;

	if (!l) return l;
	if ((*eq)(l->first, x)) return ptrlistFreeCons(l);

	for (p = l, t = l->rest; t; p = t, t = t->rest) {
		if ((*eq)(t->first, x))  {
			p->rest = ptrlistFreeCons(t);		
			break;
		}
	}
	return l;
}

/*
 * Fill the array pointed to by v with the elements of the list l.      
 */
local void
ptrlistFillVector(Pointer *v, PointerList l)
{
	for (; l; l = l->rest, v++) *v = l->first;
}

/*
 * Print the list as `open'p0`sep'...p3`close' where p0 etc are printed by 
 * applying prf to the elements.  The number of chars printed is returned.
 */
local int
ptrlistGPrint(FILE *fout, PointerList l, PointerListEltPrFun prf,
	      String open, String sep, String close)
{
	int             cc;

	if (!l) {
		cc = fprintf(fout, "%s%s", open, close);
		return cc;
	}
	cc =  fprintf(fout, "%s", open);
	cc += (*prf)(fout, l->first);

	for (l = l->rest; l; l = l->rest) {
		cc += fprintf(fout, "%s", sep);
		cc += (*prf)(fout, l->first);
	}
	cc += fprintf(fout, "%s", close);
	return cc;
}

/*
 * Print the ptrlist as [p0, ... p3] where p0 etc are printed by 
 * applying prf to the elements. The number of chars printed is returned.       
 */
local int
ptrlistPrint(FILE *fout, PointerList l, PointerListEltPrFun prf)
{
	return ptrlistGPrint(fout, l, prf, "[", ", ", "]");
}

local int
ptrListFormat(OStream stream, CString formatName, PointerList l)
{
	int cc = 0;
	String format = strConcat("%p", (String) formatName);
	String sep = "";
	cc += ostreamWrite(stream, "[", -1);
	while (l) {
		Pointer p = l->first;
		cc += ostreamWrite(stream, sep, -1);
		cc += ostreamPrintf(stream, format, p);
		sep = ", ";
		l = l->rest;
	}
	cc += ostreamWrite(stream, "]", -1);
	strFree(format);

	return cc;
}

/*
 * Create the shared table of operations.
 */
const struct ListOpsStructName(Pointer) ptrlistOps = {
	ptrlistCons,
	ptrlistSingleton,
	ptrlistList,
	ptrlistListv,
	ptrlistListNull,
	ptrlistEqual,
	ptrlistFind,
	ptrlistFreeCons,                                
	ptrlistFree,
	ptrlistFreeTo,
	ptrlistFreeDeeply,
	ptrlistFreeDeeplyTo,
	ptrlistFreeIfSat,
	ptrlistElt,                                     
	ptrlistDrop,
	ptrlistLastCons,
	ptrlistLength,
	ptrlistIsLength,
	ptrlistIsLonger,
	ptrlistIsShorter,
	ptrlistCopy,
	ptrlistCopyTo,
	ptrlistCopyDeeply,
	ptrlistCopyDeeplyTo,
	ptrlistMap,
	ptrlistNMap,
	ptrlistReverse,                         
	ptrlistNReverse,                                
	ptrlistConcat,
	ptrlistNConcat,
	ptrlistMemq,
	ptrlistMember,
	ptrlistPosq,
	ptrlistPosition,                                
	ptrlistNRemove,
	ptrlistFillVector,                              
	ptrlistPrint,
	ptrlistGPrint,
	ptrListFormat,
};

CREATE_LIST(Pointer);
