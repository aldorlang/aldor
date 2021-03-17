/*****************************************************************************
 *
 * bitv.c: Bit vector operations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/****************************************************************************
 * This file provides all the basic operations for bit vectors manipulation.
 *
 * For space saving reasons, the size of a bitv is not saved in the bitv
 * representation. This avoid, in example in the dflow analysis algorithms,
 * to allocate many copies of the same length-information, since generally
 * all the bit vectors used have the same size.
 *
 * Therefore creating a bitvClass is needed before working with bitv of size
 * NBITS, and the bitvClass is passed to all the bitv operations.
 *
 * Example:
 *
 * 	BitvClass	bitvClass = bitvClassCreate(NBITS);
 *
 *	Bitv		bitv 	  = bitvNew(bitvClass);
 *
 *	bitvClearAll	(bitvClass, bitv);
 *	bitvSet		(bitvClass, bitv);
 *	bitvPrint	(fout, bitvClass, bitv);
 *
 *	bitvFree	(bitv);		// class not needed
 *	bitvClassDestroy(bitvClass);
 *
 ***************************************************************************/

#include "axlgen.h"
#include "buffer.h"
#include "bitv.h"
#include "debug.h"
#include "store.h"
#include "util.h"

#define 	BpW	bitsizeof(BitvWord)

BitvClass
bitvClassCreate(int nbits)
{
	BitvClass	bitvClass;

	bitvClass = (BitvClass) stoAlloc(OB_Other, sizeof(struct _BitvClass));
	
	bitvClass->nbits  = nbits;
	bitvClass->nwords = QUO_ROUND_UP(nbits, BpW);

	return bitvClass;
}

void
bitvClassDestroy(BitvClass class)
{
	if (class) stoFree(class);
}

Bitv
bitvNew(BitvClass class)
{
	Bitv bv = (Bitv) stoAlloc(OB_Other, class->nwords * sizeof(BitvWord));
	return bv;
}

void
bitvFree(Bitv a)
{
	stoFree((Pointer) a);
}

/* Given a bitv and its class, convert is to a new class, possible w/out
 * reallocate memory.
 * If the lenght is bigger, the old bits are copied, and the new ones are
 * undefined.
 * If the length is less than the original, it's truncated.
 */
Bitv
bitvResize(BitvClass newc, BitvClass oldc, Bitv b)
{
	Bitv	new;
	int	i;

	if (oldc->nwords >= newc->nwords) return b;

	new = bitvNew(newc);
	for (i = 0; i < oldc->nwords; i++)
		new[i] = *b++;
	
	bitvFree(b);

	return new;
}

Bitv *
bitvManyNew(BitvClass class, Length n)
{
# 	define bitvOffset(n,i) ((n)*sizeof(Bitv) + (i)*class->nwords*sizeof(BitvWord))

	Bitv 	*bvv = (Bitv *) stoAlloc(OB_Other, bitvOffset(n,n));
	int	i;

	for (i = 0; i < n; i++) {
		bvv[i] = (Bitv)((char *) bvv + bitvOffset(n, i));
		bitvClearAll(class, bvv[i]);
	}
	return bvv;
}

void
bitvManyFree(Bitv * bvv)
{
	stoFree((Pointer) bvv);
}

int
bitvPrint(FILE *fout, BitvClass class, Bitv a)
{
	int	i, cc = 0;

	if (!class) {
		cc += fprintf(fout,
			      "[** Unprintable bitv: bitvClass required **]");
		return cc;
	}

	cc += fprintf(fout, "[");
	for (i = 0; i < class->nbits; i++) {
		cc += fprintf(fout, "%d",
			      bitvTest(class, a,i));
		if (i % 5 == 4) cc += fprintf(fout, " ");
	}
	cc += fprintf(fout, "]");
	return cc;
}

int
bitvPrintDb(BitvClass class, Bitv a)
{
	int i = bitvPrint(dbOut, class, a);
	fprintf(dbOut, "\n");
	return i;
}

String
bitvToString(BitvClass class, Bitv a)
{
	Buffer buf;
	int    i;

	buf = bufNew();
	if (!class) {
		bufPrintf(buf,
			      "[** Unprintable bitv: bitvClass required **]");
		return bufLiberate(buf);
	}

	bufPrintf(buf, "[");
	for (i = 0; i < class->nbits; i++) {
		bufPrintf(buf, "%d",
				bitvTest(class, a,i));
		if (i % 5 == 4) bufPrintf(buf, " ");
	}
	bufPrintf(buf, "]");

	return bufLiberate(buf);
}


Bool
bitvEqual(BitvClass class, Bitv a, Bitv b)
{
	int	 i;
	BitvWord mask;

	if (class->nwords == 0) return true;

	for (i = 0; i < class->nwords-1; i++) if (*a++ != *b++) return false;
	if (class->nbits % BpW == 0) return *a == *b;

	/*
	 * Test last word differently -- mask out extra bits so they
	 * do not influence the equality test.
	 */
	mask = ~((~0UL) << (class->nbits % BpW));
	return (*a & mask) == (*b & mask);
}

void
bitvSetAll(BitvClass class, Bitv r)
{
	int	i;

	for (i = 0; i < class->nwords; i++) *r++ = (BitvWord)(~0L);
}

void
bitvClearAll(BitvClass class, Bitv r)
{
	int	i;
	for (i = 0; i < class->nwords; i++) *r++ = 0;
}

int
bitvTest(BitvClass class, Bitv r, int ix)
{
	assert(ix < class->nbits);
	return !!(r[ix/BpW] & (1L << (ix % BpW)));
}

void
bitvSet(BitvClass class, Bitv r, int ix)
{
	assert(ix < class->nbits);
	r[ix/BpW] |=  (1L << (ix % BpW));
}

void
bitvClear(BitvClass class, Bitv r, int ix)
{
	assert(ix < class->nbits);
	r[ix/BpW] &= ~(1L << (ix % BpW));
}

void
bitvCopy(BitvClass class, Bitv r, Bitv a)
{
	int	i;
	for (i = 0; i < class->nwords; i++) *r++ = *a++;
}

void
bitvNot(BitvClass class, Bitv r, Bitv a)
{
	int	i;
	for (i = 0; i < class->nwords; i++) *r++ = ~*a++;
}

void
bitvAnd(BitvClass class, Bitv r, Bitv a, Bitv b)
{
	int	i;
	for (i = 0; i < class->nwords; i++) *r++ = *a++ & *b++;
}

void
bitvOr(BitvClass class, Bitv r, Bitv a, Bitv b)
{
	int 	i;
	for (i = 0; i < class->nwords; i++) *r++ = *a++ | *b++;
}

void
bitvMinus(BitvClass class, Bitv r, Bitv a, Bitv b)
{
	int	i;
	for (i = 0; i < class->nwords; i++) *r++ = *a++ & ~*b++;
}

/**
 * Returns last set bit.
 * Returns -1 if none set.
 */
int 
bitvMax(BitvClass class, Bitv bv)
{
	int i;
	/* FIXME: Could be _much_ more efficient */
	for (i=class->nbits-1; i>= 0; i--) {
		if (bitvTest(class, bv, i))
			return i;
	}
	return i;
}

int
bitvCount(BitvClass class, Bitv bv)
{
	int total = 0;
	int i;
	/* FIXME: Could be _much_ more efficient */
	for (i=0; i<class->nbits; i++) {
		if (bitvTest(class, bv, i))
			total++;
	}
	return total;
}

int
bitvCountTo(BitvClass class, Bitv bv, int n)
{
	int total = 0;
	int i = 0;

	/* FIXME: Could be _much_ more efficient */
	for (i=0; i<n; i++) {
		if (bitvTest(class, bv, i))
			total++;
	}
	return total;
}

int
bitvUnique1IndexInRange(BitvClass class, Bitv bv, int org, int lim)
{
	int	n1s, i, last1=-1;

	n1s = 0;

	for (i = org; i < lim; i++)
		if (bitvTest(class, bv, i)) {
			n1s++;
			last1 = i;
			if (n1s > 1) return -1;
		}

	if (n1s != 1) return -1;
	return last1;
}

Bitv
bitvFromInt(BitvClass class, int n)
{
	Bitv bitv = bitvNew(class);
	int i;

	assert(class->nbits < 8 * sizeof(int));

	for (i=0; i<class->nbits; i++) {
		if (n & (1<<i))
			bitvSet(class, bitv, i);
		else
			bitvClear(class, bitv, i);
	}
	return bitv;
}

int
bitvToInt(BitvClass class, Bitv bitv)
{
	int result = 0;
	int i;
	assert(class->nbits < 8 * sizeof(int));
	for (i=0; i<class->nbits; i++) {
		if (bitvTest(class, bitv, i))
			result = result | (1<<i);
	}
	return result;
}
