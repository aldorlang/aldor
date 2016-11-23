/*****************************************************************************
 *
 * cport_t.c: Test C compiler portability code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_CPORT) && !defined(TEST_ALL)

void testCPort(void) { }

#else

#include "axlgen.h"
#include "cport.h"

/*
 * Test the integer byte-ordering macros.
 */

void
testBytes(void)
{
	int	n, b0, b1, b2, b3;

	n = 0x12345678;
	b0 = BYTE0(n);
	b1 = BYTE1(n);
	b2 = BYTE2(n);
	b3 = BYTE3(n);
	printf("The byte mask is: %#x\n", BYTE_MASK);
	printf("The bytes of %#x are: %#x, %#x, %#x, %#x\n", n,b0,b1,b2,b3);
	printf("Reconstructing  %#x gives %#lx\n", n, UNBYTE4(b0,b1,b2,b3));

}

/*
 * Main test function for C portability code.
 */

void
testCPort(void)
{
	testBytes();
}

#endif /* TEST_CPORT || TEST_ALL */
