/*****************************************************************************
 *
 * util_t.c: Test utilities file.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_UTIL) && !defined(TEST_ALL)

void testUtil(void) { }

#else

# include "axlgen.h"

local void
junkobits(int nb, UByte *bb)
{
	int n, b;
	for (n = 0, b = 1; n < nb; n++, b++) bb[n] = (b<<4) + (b/3);
}

local void
printbits(int nb, UByte *bb)
{
	int n;
	for (n = 0; n < nb; n++) printf("%02x.", bb[n]);
}

void
testUtil(void)
{
	int	i, n;
	UByte	bb[10];

	printf("binPrime: \n");
	for (n = 0; n < 32; n++) {
		printf("  %ld\n", binPrime(n));
	}
	printf("cielLg: \n");
	printf("The ceiling of %ld is %d\n",binPrime(10),
	       cielLg(binPrime(10)));
	printf("DONE.\n");

	printf("bfShiftDn (0,0): \n");
	junkobits(sizeof(bb), bb);
	printbits(sizeof(bb), bb);
	printf("\n");
	for (i = 0; i < bitsizeof(bb); i++) {
		junkobits(sizeof(bb), bb);
		printf(" >> %03d = ", i);
		bfShiftDn(sizeof(bb), bb, i, bb, int0, int0);
		printbits(sizeof(bb), bb);
		printf("\n");
	}
	printf("DONE.\n");

	printf("bfShiftDn (0,1): \n");
	junkobits(sizeof(bb), bb);
	printbits(sizeof(bb), bb);
	printf("\n");
	for (i = 0; i < bitsizeof(bb); i++) {
		junkobits(sizeof(bb), bb);
		printf(" >> %03d = ", i);
		bfShiftDn(sizeof(bb), bb, i, bb, int0, 1);
		printbits(sizeof(bb), bb);
		printf("\n");
	}
	printf("DONE.\n");

	printf("bfShiftUp (0): \n");
	junkobits(sizeof(bb), bb);
	printbits(sizeof(bb), bb);
	printf("\n");
	for (i = 0; i < bitsizeof(bb); i++) {
		junkobits(sizeof(bb), bb);
		printf(" << %03d = ", i);
		bfShiftUp(sizeof(bb), bb, i, bb, int0);
		printbits(sizeof(bb), bb);
		printf("\n");
	}
	printf("DONE.\n");

}

#endif
