/*****************************************************************************
 *
 * priq_t.c: Test priority queue data structure.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_PRIQ) && !defined(TEST_ALL)

void testPriq(void) { }

#else

#include "axlgen.h"
#include "priq.h"

static PriQKey	testData[] = {
	100, 98, 7, 2, 33, 4, 5, 56, 78, 7, 1, 7, 4, 
	200, 150, 175, 125, 3, 8, 2, 23, 100, 100,
	-1
};

void
testPriq(void)
{
	PriQ	pq;
	int	i;

	for (i = 1; i < 7; i++) {
		printf("priqNew: ");
		pq = priqNew(i);
		priqPrint(osStdout, pq);

		printf("priqFree: ");
		priqFree(pq);
		printf("done\n");
	}

	printf("priqInsert:\n");
	pq = priqNew(int0);
	for (i = 0; testData[i] >= 0; i++) {
		printf("... inserting %.6f\n", testData[i]);
		priqInsert(pq, testData[i], NULL);
	}
	priqPrint(osStdout, pq);

	printf("priqExtractMin:\n");
	while (priqCount(pq) > 0) {
		PriQKey	mk;
		priqExtractMin(pq, &mk);
		printf("... extracted %.6f\n", mk);
	}
	priqFree(pq);
}

#endif
