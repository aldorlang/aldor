/*****************************************************************************
 *
 * store3_t.c: Test discontiguous heap and large pieces.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_STORE3) && !defined(TEST_ALL)

void testStore3(void) { }

#else

#include "axlgen.h"

void
testStore3(void)
{
#define TEST_SIZE 1000
#define TEST_ITER 41
	Pointer test[TEST_SIZE], mallocs[TEST_ITER+2];
	long    i, j, sz, mi;

	for (j = 0, mi = 0; j < TEST_ITER; j++) {
		if (j % 2 == 0)
			for (i = 0; i < TEST_SIZE; i++) {
				if (i % 250 == 99)
					sz = 123321;
				else
					sz = i % 201;
				test[i] = (Pointer) stoAlloc(3, sz);
				if (i == 1)
					mallocs[mi++] = malloc(1000);
				if (i == TEST_SIZE/2) 
					mallocs[mi++] = malloc(10000);
			}
		else
			for (i = 0; i < TEST_SIZE; i++) {
				stoFree(test[i]);
			}
	}
	stoAudit();
	stoShow();
	if (j % 2 != 0) {
		for (i = 0; i < TEST_SIZE; i++) {
			stoFree(test[i]);
		}
	}
	for (i = 0; i < mi; i++) {
		free(mallocs[i]);
	}

	stoGc();
}

#endif
