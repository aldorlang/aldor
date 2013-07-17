/*****************************************************************************
 *
 * store1_t.c: Storage tests.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * 	Store1a -- Working of alloc/free.
 * 	Store1b -- Timing  of alloc/free.
 * 	Store1c -- Working of GC.
 * 	Store1d -- Timing  of GC.
 */

#if !defined(TEST_STORE1) && !defined(TEST_ALL)

void testStore1a(void) { }
void testStore1b(void) { }
void testStore1c(void) { }
void testStore1d(void) { }

#else

#include "axlgen.h"
#include "store.h"

/*****************************************************************************
 *
 * :: Entry points
 *
 *****************************************************************************/

extern void stoTest(int niterations, int flags);

#define N_ITER		20
#define STOTEST_AUDIT	1
#define STOTEST_GC	2


void
testStore1a(void)
{
	printf("Test working of alloc/free.\n");
	stoTest(N_ITER, STOTEST_AUDIT);
}


void
testStore1b(void)
{
	printf("Test timing  of alloc/free.\n");
	stoTest(5*N_ITER, int0);
}

void
testStore1c(void)
{
	printf("Test working of GC.\n");
	stoTest(N_ITER, STOTEST_GC | STOTEST_AUDIT);
}

void
testStore1d(void)
{
	printf("Test timing  of GC.\n");
	stoTest(5*N_ITER, STOTEST_GC);
}


/*****************************************************************************
 *
 * :: Test for small v. large heap.
 *
 *****************************************************************************/

static StoErrorFun h0;

local MostAlignedType *
okStoHandler(int errnum)
{
	if (errnum == StoErr_OutOfMemory) return 0;
	return h0(errnum);
}

local Bool
testIsSmallStore(void)
{
	Pointer	p;
	Bool	b;

	h0 = stoSetHandler(okStoHandler);
	p  = (Pointer) stoAlloc(int0, 1000000);
   	/* If we can get 1M, it's not small. */
	if (p) stoFree(p);
	stoSetHandler(h0);

	b = p ? false : true;

	printf("Available storage is %s.\n", b ? "small" : "large");

	return b;
}

/*****************************************************************************
 *
 * :: Parameters controlling the test.
 *
 *****************************************************************************/

static int N_ALLOC;     /* Number of pieces at high water mark. */
static int N_FREE;      /* Number of allocs to free. */
static int MIN_RG;      /* Smallest pieces in 0..regime[MIN_RG]-1. */
static int MAX_RG;      /* Largest  pieces in 0..regime[MAX_RG]-1. */


/*****************************************************************************
 *
 * :: Flag-controlled test actions.
 *
 *****************************************************************************/

void
stoTestSay(char *kind, long requested, long owned, int flags)
{
	Bool	dogc = flags & STOTEST_GC, doaudit = flags & STOTEST_AUDIT;

	printf("%s mark: %ld%% ", kind, requested/(owned/100));
	if (doaudit)		{ printf(" Auditing "); stoAudit(); }
	if (dogc)		{ stoGc(); }
	if (doaudit && dogc)	{ printf(" Auditing "); stoAudit(); }
	printf("\n");
}

#define stoWash(flag,a,n) \
	{ if (flag) memlset ((a)[n].piece, (n) % 100, (a)[n].size); }
#define stoCheckWash(flag,a,n) \
	{ if (flag) memltest((a)[n].piece, (n) % 100, (a)[n].size); }



/*****************************************************************************
 *
 * :: Test driver.
 *
 *****************************************************************************/

/*
 *	For each test do:
 * 	1. Allocate an array of pieces of random graded size.
 * 	2. Free or crush a random subset of them.
 * 	3. Repeat.
 */ 

void
stoTest(int niter, int flags)
{
        Pointer   p;
	Millisec  t = osCpuTime();
	long      requested=0, nallocs=0, nfrees=0, owned0=stoBytesAlloc ;
        long      i, j, k, n, n0, sz;
	Bool	  dogc = flags & STOTEST_GC, doaudit = flags & STOTEST_AUDIT;

	long	  *regime;
	long      *freeSlot;
	struct pc {
		long	size;
		Pointer piece;
	} *alloc;

	if (testIsSmallStore()) {
		N_ALLOC =     100;
		N_FREE  =      50;
		MIN_RG  =       1;
		MAX_RG  =       3;
	}
	else {
		N_ALLOC =    1000;
		N_FREE  =     500;
		MIN_RG  =       1;
		MAX_RG  =       4;
	}

	alloc    = (struct pc *)stoAlloc(int0,N_ALLOC    * sizeof(*alloc));
	freeSlot = (long *)     stoAlloc(int0,N_FREE     * sizeof(*freeSlot));
	regime   = (long *)     stoAlloc(int0,(MAX_RG+1) * sizeof(*regime));

        regime[0] = 1; for (i = 1; i <= MAX_RG; i++) regime[i] = 10*regime[i-1];

        /*
         * Setup initial low-water mark with first part of array full,
         * and second part free.
         */
        for (i = 0; i < N_ALLOC - N_FREE; i++) {
		sz = rand();
                sz = sz % regime[sz % (MAX_RG - MIN_RG + 1) + MIN_RG];
                p  = (Pointer) stoAlloc(int0, sz);

                alloc[i].size  = sz;
                alloc[i].piece = p;
		stoWash(doaudit, alloc, i);
                requested      += sz;
                nallocs++;
	}
        for (i = 0; i < N_FREE; i++) {
                k = N_ALLOC - N_FREE + i;
                freeSlot[i] = k;
                alloc[k].piece = 0;
        }

        for (i = 0; i < niter; i++) {
                /*
                 * First alloc available slots, then free randomly.
                 */
                for (j = 0; j < N_FREE; j++) {
                        sz = rand();
                        sz = sz % regime[sz % (MAX_RG - MIN_RG + 1) + MIN_RG];
                        p  = (Pointer) stoAlloc(int0, sz);

                        n  = freeSlot[j];
                        alloc[n].size  = sz;
                        alloc[n].piece = p;
			stoWash(doaudit, alloc, n);
                        requested      += sz;
                        nallocs++;
                }
		stoTestSay("Hi", requested, stoBytesOwn-owned0, flags);

                for (j = 0; j < N_FREE; j++) {
			n = n0 = rand() % N_ALLOC;

                        /* Look for next full slot if already free. */
                        if (!alloc[n].piece) {
                                for (n++; n < N_ALLOC; n++)
                                        if (alloc[n].piece) goto gotcha;
                                for (n=0; n < n0; n++)
                                        if (alloc[n].piece) goto gotcha;
                                printf("Can't find any pieces??\n");
				exitFailure();
                        }
                gotcha:
			stoCheckWash(doaudit, alloc, n);
			if (!dogc) stoFree(alloc[n].piece);
                        alloc[n].piece = 0;
                        requested      -= alloc[n].size;
                        freeSlot[j]    = n;
                        nfrees++;
                }
		stoTestSay("Lo", requested, stoBytesOwn-owned0, flags);
        }
	for (i = 0; i < N_ALLOC; i++)
		if (alloc[i].piece) {
			stoCheckWash(doaudit, alloc, i);
			stoFree(alloc[i].piece);
                        requested -= alloc[i].size;
			nfrees++;
		}
	stoTestSay("Lo", requested, stoBytesOwn-owned0, flags);

	printf("Time = %ld msec\n", osCpuTime() - t);

	stoFree((Pointer) alloc);
	stoFree((Pointer) freeSlot);
	stoFree((Pointer) regime);

	stoShow();
}

#endif
