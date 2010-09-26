#include "axlgen.h"
#include "bitv.h"
#include "intset.h"

/*
 * Sets of integers.
 * A very thin wrapper around bitv.
 */

struct intSet {
	BitvClass clss;
	Bitv bitv;
};

IntSet 
intSetNew(int sz)
{
	IntSet s = (IntSet) stoAlloc(OB_Other, sizeof(*s));
	s->clss = bitvClassCreate(sz);
	s->bitv = bitvNew(s->clss);
	bitvClearAll(s->clss, s->bitv);
	return s;
}

void 
intSetFree(IntSet s)
{
	bitvFree(s->bitv);
	bitvClassDestroy(s->clss);
	stoFree(s);
}

void 
intSetAdd(IntSet s, int i)
{
	if (i >= bitvClassSize(s->clss)) {
		assert(false);
	}
	bitvSet(s->clss, s->bitv, i);
}

void 
intSetRemove(IntSet s, int i)
{
	bitvClear(s->clss, s->bitv, i);
}

Bool 
intSetMember(IntSet s, int i)
{
	return bitvTest(s->clss, s->bitv, i);
}

void   
intSetPrint(FILE *file, IntSet s)
{
	bitvPrint(file, s->clss, s->bitv);
}

void   
intSetPrintDb(IntSet s)
{
	intSetPrint(dbOut, s);
}

String
intSetToString(IntSet s)
{
	return bitvToString(s->clss, s->bitv);
}



