/*****************************************************************************
 *
 * dnf_t.c: Test Boolean DNF operations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_DNF) && !defined(TEST_ALL)

void testDnf(void) { }

#else

#include "axlgen.h"

void
testDnf(void)
{
	DNF	tt, ff, x1, x2, nx2, x3, x4, nx4, xx, yy, zz;
	DNF	t0, t1, t2;

	printf("True:   ");
	tt = dnfTrue();
	dnfPrint(osStdout, tt);
	printf("\n");

	printf("False:  ");
	ff = dnfFalse();
	dnfPrint(osStdout, ff);
	printf("\n");


	printf("Atom:   ");
	x1 = dnfAtom(1);
	dnfPrint(osStdout, x1);
	printf(" ");
	x2 = dnfAtom(2);
	dnfPrint(osStdout, x2);
	printf(" ");
	nx2 = dnfNotAtom(2);
	dnfPrint(osStdout, nx2);
	printf(" ");
	x3 = dnfAtom(3);
	dnfPrint(osStdout, x3);
	printf(" ");
	x4 = dnfAtom(4);
	dnfPrint(osStdout, x4);
	printf(" ");
	nx4 = dnfNotAtom(4);
	dnfPrint(osStdout, nx4);
	printf("\n");

	printf("Or:     ");
	t0 = dnfOr(x1,nx2);
	t1 = dnfOr(x3, x4);
	xx = dnfOr(t0, t1);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(t1);
	dnfFree(xx);
	printf("\n");

	printf("And:    ");
	t0 = dnfAnd(x1,nx2);
	t1 = dnfAnd(x3, x4);
	xx = dnfAnd(t0, t1);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(t1);
	printf("\n");

	printf("Not:    ");
	yy = dnfNot(xx);
	dnfPrint(osStdout, yy);
	dnfFree(xx);
	dnfFree(yy);
	printf("\n");

	printf("And *0: ");
	t0 = dnfAnd(x4,x3);
	t1 = dnfAnd(x2,nx4);
	xx = dnfAnd(t0, t1);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(t1);
	dnfFree(xx);
	printf("\n");

	printf("And *0: ");
	t0 = dnfAnd(x2,nx4);
	xx = dnfAnd(ff, t0);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(xx);
	printf("\n");

	printf("And *0: ");
	t0 = dnfAnd(x2,nx4);
	xx = dnfAnd(t0, ff);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(xx);
	printf("\n");

	printf("And *1: ");
	t0 = dnfAnd(x2,nx4);
	xx = dnfAnd(tt, t0);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(xx);
	printf("\n");

	printf("And *1: ");
	t0 = dnfAnd(x2,nx4);
	xx = dnfAnd(t0, tt);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(xx);
	printf("\n");

	printf("And *2: ");
	t0 = dnfAnd(x4,x3);
	t1 = dnfAnd(x2,x4);
	xx = dnfAnd(t0, t1);
	dnfPrint(osStdout, xx);
	dnfFree(t0);
	dnfFree(t1);
	dnfFree(xx);
	printf("\n");

	printf("And 2*3:");
	t0 = dnfAnd(x1,nx4);
	t1 = dnfAnd(x1,x2);
	t2 = dnfAnd(t1,x3);
	xx = dnfOr(t0, t2);
	dnfFree(t0);
	dnfFree(t1);
	dnfFree(t2);
	t0 = dnfAnd(x2,x4);
	t1 = dnfAnd(x2,x3);
	t2 = dnfAnd(t1,x4);
	dnfFree(t1);
	t1 = dnfOr(t2, x1);
	dnfFree(t2);
	yy = dnfOr(t0,  t1);
	dnfFree(t0);
	dnfFree(t1);
	zz = dnfAnd(xx, yy);
	dnfPrint(osStdout, zz);
	dnfFree(xx);
	dnfFree(yy);
	printf("\n");

	printf("Not 2*3:");
	xx = dnfNot(zz);
	dnfPrint(osStdout, xx);
	dnfFree(xx);
	dnfFree(zz);
	printf("\n");
	
	printf("Free:   ");
	dnfPrint(osStdout, tt);
	printf(" ");
	dnfPrint(osStdout, ff);
	printf(" ");
	dnfPrint(osStdout, x1);
	printf(" ");
	dnfPrint(osStdout, x2);
	printf(" ");
	dnfPrint(osStdout, nx2);
	printf(" ");
	dnfPrint(osStdout, x3);
	printf(" ");
	dnfPrint(osStdout, x4);
	printf(" ");
	dnfPrint(osStdout, nx4);

	dnfFree(tt);
	dnfFree(ff);
	dnfFree(x1);
	dnfFree(x2);
	dnfFree(nx2);
	dnfFree(x3);
	dnfFree(x4);
	dnfFree(nx4);
	printf("\n");
}

#endif
