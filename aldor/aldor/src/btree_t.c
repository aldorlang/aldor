/*****************************************************************************
 *
 * btree_t.c: Test B-trees.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_BTREE) && !defined(TEST_ALL)

void testBtree(void) { }

#else

#include "axlgen.h"
#include "btree.h"
#include "opsys.h"

void
testBtree(void)
{
#define BTREE_TEST_T		2
#define BTREE_TEST_ITERS        1000
#define BTREE_TEST_KEY_LIMIT    2000

        int             i, rc;
        BTree           b = btreeNew(BTREE_TEST_T);
        Length          maxk = 0;
        BTreeKey        key[BTREE_TEST_KEY_LIMIT];
        BTree           lastx;
        int             lastxi;

        /*
         * Build tree by inserting random numbers.
         * Keep track of keys used.
         */
	printf("Inserting: ");
        for (i = 0; i < BTREE_TEST_ITERS; i++) {
                key[i] = 2*(osRandom() % (BTREE_TEST_KEY_LIMIT/2));
                if (key[i] > maxk) maxk = key[i];
                btreeInsert(&b, key[i], NULL);
                if ((rc = btreeCheck(b)) != 0) printf(" [Error: %d]", rc);
        }
	printf("\n");

        /*
         * Check memory used.
         */
#if EDIT_1_0_n1_07
        printf("The size of a BTree node is %d\n", (int) sizeof(*b));
#else
        printf("The size of a BTree node is %d\n", sizeof(*b));
#endif

        /*
         * Test searching for best fit.
         */
	lastx  = 0;
	lastxi = -1;
        for (i = maxk-20; i < maxk + 5; i++) {
                BTree   x;
                int     xi;
                x = btreeSearchGE(b, i, &xi);
                if (x) {
			/* If found something new, say so. */
                        if (x != lastx || xi != lastxi)
                                printf("\nFound %ld for:", x->part[xi].key);
			/* Say what we were looking for. */
                        printf(" %d",i);
                }
                else {
                        if (x != lastx || xi != lastxi)
				printf("\nNot Found: ");
                        printf(" %d",i);
                }
                lastx = x;
                lastxi = xi;
        }
        printf("\n");

        /*
         * Test removal.
         */
        printf("Deleting: ");

	if ((rc = btreeCheck(b)) != 0) printf(" [Error: %d]", rc);
        for (i = 0; i < BTREE_TEST_ITERS; i++) {
                btreeDelete(&b, key[i], NULL);
                if ((rc = btreeCheck(b)) != 0) printf(" [Error: %d]", rc);
        }
	printf("\n");
        btreePrint(osStdout, b);
	btreeFree(b);

}

#endif
