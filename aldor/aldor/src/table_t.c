/*****************************************************************************
 *
 * table_t.c: Test table type.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_TABLE) && !defined(TEST_ALL)

void testTable(void) { }

#else

#include "axlgen.h"
#include "opsys.h"
#include "table.h"

local void	tblHistogram	(Table);
local TblElt	tblTestMap	(TblElt);
local int	intprint	(FILE *, Pointer);

local int
intprint(FILE *fout, Pointer p)
{
	return fprintf(fout, "%ld", (long) p);
}

local TblElt
tblTestMap(TblElt e)
{
	return (TblElt) (10 * (long) e);
}

local void
tblHistogram(Table t)
{
	int     count[50];
	int     infty = sizeof(count)/sizeof(int);

	int     n, i, j, max, maxc;
	struct TblSlot *b;

	printf("Table with %d buckets (%d entries):\n",
		(int) t->buckc, (int) t->count);

	for (i = 0; i < infty; i++) count[i] = 0;
	max = 0;

	for (i = 0; i < t->buckc; i++) {
		for (b = t->buckv[i], n = 0; b; b = b->next, n++)
			;
		if (n > infty) n = infty;
		if (n > max)   max = n;
		count[n]++;
	}
	printf("  L:  No. of buckets with L entries (%d = infinity)\n", infty);
	
	for (maxc=0, i=0; i<=max; i++) if (count[i] > maxc) maxc = count[i];
	for (i = 0; i <= max; i++) {
		printf("%3d: %4d ", i, count[i]);
		n = (int) (50 * ((float) count[i])/((float) maxc));
		for (j = 0; j < n; j++) putchar('+');
		printf("\n");
	}
}

void
testTable(void)
{
	Table           t, tc;
	TableIterator   tit;
	int             i;

	printf("tblNew: ");
	t = tblNew((TblHashFun) 0, (TblEqFun) 0);
	tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
	printf("\n");

	printf("tblSetElt i->2*i: ");
	for (i = 0; i < 37; i++)
		tblSetElt(t, (TblKey) (long) i, (TblElt) (long) (2*i));
	printf("DONE.\n");

	printf("tblElt: ");
	for (i = 0; i < 50; i += 11) {
		printf("%d -> %ld; ",i,
			(long)tblElt(t,(TblKey) (long) i,(TblElt) (long) (-1)));
	}
	printf("\n for ");
	tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
	printf("\n");

	printf("tblSetElt i->i+1: ");
	for (i = 0; i < 37; i++)
		tblSetElt(t, (TblKey) (long) i, (TblElt) (long) (i+1));
	printf("DONE.\n");

	printf("tblElt: ");
	for (i = 0; i < 50; i += 11) {
		printf("%d -> %ld; ",
			i,(long)tblElt(t,(TblKey) (long) i,(TblElt)(long)(-1)));
	}
	printf("\n");

	printf("tblHistogram: ");
	tblHistogram(t);

	printf("tblDrop: ");
	for (i = 0; i < 37; i += 2) tblDrop(t, (TblKey) (long) i);
	tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
	printf("\n");

	printf("tblDrop: ");
	for (i = 1; i < 37; i += 2) tblDrop(t, (TblKey) (long) i);
	tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
	printf("\n");

	printf("tblFree: ");
	tblFree(t);
	printf("DONE.\n");

	printf("tblITER: ");
	t = tblNew((TblHashFun) 0, (TblEqFun) 0);
	for (i = 1; i < 60; i += 17)
		tblSetElt(t, (TblKey) (long) i, (TblElt) (long)(2*i));
	tblSetElt(t, (Pointer) 0, (Pointer) 1);
	for (tblITER(tit, t); tblMORE(tit); tblSTEP(tit)) {
		printf("%ld->%ld, ", (long)tblKEY(tit), (long)tblELT(tit));
	}
	printf("\n for ");
	tblPrint(osStdout,t,(TblPrKeyFun) intprint, (TblPrEltFun) intprint);
	printf("\n");

	printf("tblCopy: ");
	tc = tblCopy(t);
	tblPrint(osStdout, tc, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
	printf("\n");

	printf("tblNMap: ");
	tblNMap(tblTestMap, tc);
	tblPrint(osStdout, tc, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
	printf("\n");

	tblFree(t);
	tblFree(tc);

}

#endif
