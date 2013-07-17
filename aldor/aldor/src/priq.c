/*****************************************************************************
 *
 * priq.c: Priority Queue data structure.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "priq.h"
#include "store.h"


/*****************************************************************************
 *
 * :: Heap operations
 *
 *****************************************************************************/

/*
 * A heap is a complete binary tree with the property that each
 * non-leaf N satisfies N.key >= N.right.key and N.key >= N.left.key.  
 * The nodes are stored in an array, with node A[i] having children
 * A[2*i+1] (left) and A[2*i+2] (right).
 */

typedef struct priqPart *Heap;
typedef PriQKey		HeapKey;
typedef PriQElt		HeapElt;

typedef void (*HeapMapFn)(HeapKey, HeapElt);

#define	heapParent(i)	(((i)-1)/2)
#define	heapLeft(i)	(2*(i)+1)
#define	heapRight(i)	(2*(i)+2)

#define heapExchange(h,i,j) {     \
	HeapKey	_tk = h[i].key;   \
	HeapElt _te = h[i].entry; \
	h[i].key    = h[j].key;   \
	h[i].entry  = h[j].entry; \
	h[j].key    = _tk;	  \
	h[j].entry  = _te;	  \
}

/*
 * Assuming that only node i violates the heap property, convert
 * the subtree rooted at i to be a heap.   Note the key at i may initially
 * be out of order, but the subtrees heapLeft(i) and heapRight(i) are heaps.
 */
local void
heapSiftOutward(Heap h, int n, int i)
{
	int	ic, imax = i;
	
	for (; ; i = imax) {
		ic = heapLeft(i);
		if (ic < n && h[ic].key <= h[imax].key) imax = ic;
		ic = heapRight(i);
		if (ic < n && h[ic].key <= h[imax].key) imax = ic;
		if (imax == i) break;

		heapExchange(h, i, imax);
	}
}

/*
 * Assuming that only leaf node i violates the heap property, convert
 * the subtree rooted at n to be a heap.
 */
local void
heapSiftInward(Heap h, int r, int i)
{
	int	ip;
	
	for (; i > r; i = ip) {
		ip = heapParent(i);
		if (h[ip].key < h[i].key) break;

		heapExchange(h, i, ip);
	}
}

local void
heapInsert(Heap h, int n, HeapKey key, HeapElt entry)
{
	h[n].key   = key;
	h[n].entry = entry;
	heapSiftInward(h, int0, n);
}

local HeapElt
heapExtractMin(Heap h, int n, HeapKey *pkey)
{
	heapExchange(h, int0, n-1); 	/* Keep return value in n-1 slot. */
	heapSiftOutward(h, n-1, int0);  /* Sift out new smaller root. */
	if (pkey) *pkey = h[n-1].key;
	return h[n-1].entry;
}

local HeapElt
heapPeekMin(Heap h, HeapKey *pkey)
{
	if (pkey) *pkey = h[0].key;
	return h[0].entry;
}

local Bool
heapCheck(Heap h, int n)
{
	int	i, ip;

	for (i = 1; i < n; i++) {
		ip = heapParent(i);
		if (h[ip].key >= h[i].key) bug("Heap out of order.");
	}
	return true;
}

local int
heapPrint0(FILE *fout, Heap h, int n, int ix, String pre, int d)
{
	int	cc = 0;

	if (ix < n) {
		cc += fprintf(fout, "%*s", 2*d + 4, pre);
		cc += fprintf(fout, "%.6f\n", h[ix].key);
		cc += heapPrint0(fout, h, n, heapLeft(ix),  "L ", d+1);
		cc += heapPrint0(fout, h, n, heapRight(ix), "R ", d+1);
	}

	return cc;
}

local int
heapPrint(FILE *fout, Heap h, int n)
{
	return heapPrint0(fout, h, n, int0, "* ", int0);
}

local void
heapMap0(HeapMapFn fn, Heap h, int n, int ix)
{
	if (ix < n) {
		(*fn)(h[ix].key, h[ix].entry);
		heapMap0(fn, h, n, heapLeft(ix));
		heapMap0(fn, h, n, heapRight(ix));
	}
		
}

local void
heapMap(HeapMapFn fn, Heap h, int n)
{
	heapMap0(fn, h, n, int0);
}

/*****************************************************************************
 *
 * :: Priority queue operations
 *
 *****************************************************************************/

PriQ   
priqNew(Length argcGuess)
{
	PriQ pq  = (PriQ) stoAlloc(OB_Other, sizeof(*pq));
	pq->size = 1 << cielLg(argcGuess);
	pq->argc = 0;
	pq->argv = (Heap) stoAlloc(OB_Other, (pq->size)*sizeof(pq->argv[0]));
	return pq;
}

void   
priqFree(PriQ pq)
{
	stoFree((Pointer) pq->argv);
	stoFree((Pointer) pq);
}

void
priqFreeDeeply(PriQ pq, PriQEltFreeFun eltFree)
{
	int	i;

	for (i = 0; i < pq->argc; i++)
		eltFree(pq->argv[i].entry);

	pq->argc = 0;

	priqFree(pq);
}

void   
priqInsert(PriQ pq, PriQKey key, PriQElt entry)
{
	if (pq->size == pq->argc) {
		pq->size = 2*pq->size;
		pq->argv = (Heap)stoResize(pq->argv,
					   (pq->size)*sizeof(pq->argv[0]));
	}
	heapInsert(pq->argv, pq->argc++, key, entry);
}

PriQElt
priqPeekMin(PriQ pq, PriQKey *pkey)
{
	if (pq->size == 0) bug("Cannot take min of empty priority queue.");
	return heapPeekMin(pq->argv, pkey);
}

PriQElt
priqExtractMin(PriQ pq, PriQKey *pkey)
{
	if (pq->size == 0) bug("Cannot take min of empty priority queue.");
	return heapExtractMin(pq->argv, pq->argc--, pkey);
}

Bool    
priqCheck(PriQ pq)
{
	return heapCheck(pq->argv, pq->argc);
}

int    
priqPrint(FILE *fout, PriQ pq)
{
	int	cc = 0;

	cc += fprintf(fout, "priority-queue");
#if EDIT_1_0_n1_07
	cc += fprintf(fout, "(argc = %d, size = %d):\n",
                      (int) pq->argc, (int) pq->size);
#else
	cc += fprintf(fout, "(argc = %d, size = %d):\n", pq->argc, pq->size);
#endif
	cc += heapPrint(fout, pq->argv, pq->argc);

	return cc;
}

void
priqMap(PriQMapFn fn, PriQ q)
{
	heapMap((HeapMapFn) fn, q->argv, q->argc);
}
