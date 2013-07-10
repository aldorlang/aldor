/*****************************************************************************
 *
 * bitvs_t.c: Test Bitv operations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_BITS) && !defined(TEST_ALL)

void testBitv(void) { }

#else

#include "axlgen.h"

static BitvClass  bitvClass;

void
testBitv(void)
{
	Length	i, j, n, sz;

	Bitv	a, b, c, r;

	printf("Sizes:");

	for (j = 0, i = 26; i < 66; j++, i++) {
		if (j % 8 == 0) printf("\n  ");
		bitvClass = bitvClassCreate(i);
		a = bitvNew(bitvClass);
		sz = stoSize(a);
		bitvFree(a);
		bitvClassDestroy(bitvClass);
#if EDIT_1_0_n1_07
		printf("%d=>%d. ", (int) i, (int) sz);
#else
		printf("%d=>%d. ", i, sz);
#endif
	}
	printf("\n");

	n = 49;
	 bitvClass = bitvClassCreate(n);

	a = bitvNew(bitvClass);
	b = bitvNew(bitvClass);
	c = bitvNew(bitvClass);
	r = bitvNew(bitvClass);

	printf("ClearAll: ");
	bitvClearAll(bitvClass, a);
	bitvPrint(osStdout, bitvClass, a);
	printf("\n");

	printf("SetAll:   ");
	bitvSetAll(bitvClass, b);
	bitvPrint(osStdout, bitvClass, b);
	printf("\n");

	bitvSetAll(bitvClass, a);
	printf("\nClear:    ");
	bitvPrint(osStdout, bitvClass, a);
	printf("\n");
	for (i = 0; i < n; i++) {
		bitvClear(bitvClass, a, i);
/*
		if (4*i > n || 4*i < 3*n) continue;
*/
		printf("          ");
		bitvPrint(osStdout, bitvClass, a);
		printf("\n");
	}

	bitvClearAll(bitvClass, a);
	printf("\nSet:      ");
	bitvPrint(osStdout, bitvClass, a);
	printf("\n");
	for (i = 0; i < n; i++) {
		bitvSet(bitvClass, a, i);
/*
		if (4*i > n || 4*i < 3*n) continue;
*/
		printf("          ");
		bitvPrint(osStdout, bitvClass, a);
		printf("\n");
	}
	printf("\n");

	printf("Test:     [");
	for (i = 0; i < n; i++) printf("%c", bitvTest(bitvClass,a,i) ? 'Y' : 'N');
	printf("]\n");
	
	bitvSetAll(bitvClass,a);
	bitvSetAll(bitvClass,b);
	bitvClearAll(bitvClass, c);

	for (i = 0; i < n; i++) bitvSet(bitvClass,c, i);

	bitvClear(bitvClass,a, int0); bitvClear(bitvClass,a, 1);
	bitvSet  (bitvClass,a, 2); bitvSet(bitvClass,a, 3);
	bitvClear(bitvClass,b, int0); bitvSet(bitvClass,b, 1);
	bitvClear(bitvClass,b, 2); bitvSet(bitvClass,b, 3);
	bitvClear(bitvClass,c, int0); bitvSet(bitvClass, c, 1);
	bitvClear(bitvClass,c, 2); bitvSet(bitvClass,c, 3);

	printf("a =       ");
	bitvPrint(osStdout, bitvClass, a);
	printf("\n");

	printf("b =       ");
	bitvPrint(osStdout, bitvClass, b);
	printf(" (extra set)\n");

	printf("c =       ");
	bitvPrint(osStdout, bitvClass, c);
	printf(" (extra clear)\n");

	printf("Test a:   [");
	for (i = 0; i < n; i++) printf("%c", bitvTest(bitvClass,a,i) ? 'Y' : 'N');
	printf("]\n");
	
	printf("Copy a:   ");
	bitvCopy(bitvClass,r, a);
	bitvPrint(osStdout,bitvClass, c);
	printf("\n");

	printf("Not a:    ");
	bitvNot(bitvClass,r, a);
	bitvPrint(osStdout,bitvClass, c);
	printf("\n");

	printf("And a b:  ");
	bitvAnd(bitvClass,r, a, b);
	bitvPrint(osStdout,bitvClass, c);
	printf("\n");

	printf("Or a b:   ");
	bitvOr(bitvClass,r, a, b);
	bitvPrint(osStdout, bitvClass, c);
	printf("\n");

	printf("Minus a b:");
	bitvMinus(bitvClass,r, a, b);
	bitvPrint(osStdout,bitvClass, c);
	printf("\n");

	printf("Equal a a: %s\n", bitvEqual(bitvClass,a,a) ? "true" : "false");
	printf("Equal a b: %s\n", bitvEqual(bitvClass,a,b) ? "true" : "false");
	printf("Equal b c: %s\n", bitvEqual(bitvClass,b,c) ? "true" : "false");

	bitvFree(a);
	bitvFree(b);
	bitvFree(c);
	bitvFree(r);

	bitvClassDestroy(bitvClass);
}

#endif
