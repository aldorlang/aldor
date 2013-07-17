/*****************************************************************************
 *
 * format_t.c: Test program for xprintf.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_FORMAT) && !defined(TEST_ALL)

void testFormat(void) { }

#else

#include "axlgen.h"
#include "strops.h"

#define ZF	((XPutFun) 0)

void
testFormat(void)
{
	char		buf0[1024];
	char *		buf1;
	int             n;
#if TestShort
	short           s = -1;
#endif
	long            l = 22;
	double          d = 3.2;
#if TestLongDouble
	LongDouble      L = 3.2;
#endif

	n = sprintf(buf0, "The rain in Spain\n");
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in Spain\n");
#if EDIT_1_0_n1_07
	printf("*** CC = %d\n", (int) strLength(buf1));
#else
	printf("*** CC = %d\n", strLength(buf1));
#endif
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);

	n = sprintf(buf0, "The rain in %spain\n", "my ");
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in %spain\n", "my ");
#if EDIT_1_0_n1_07
	printf("*** CC = %d\n", (int) strLength(buf1));
#else
	printf("*** CC = %d\n", strLength(buf1));
#endif
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);

#if EDIT_1_0_n1_07
	n = sprintf(buf0, "The rain in %.*spain\n", -4, "my ");
#else
	n = sprintf(buf0, "The rain in %#.*spain\n", -4, "my ");
#endif
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in %#.*spain\n", -4, "my ");
#if EDIT_1_0_n1_07
	printf("*** CC = %d\n", (int) strLength(buf1));
#else
	printf("*** CC = %d\n", strLength(buf1));
#endif
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);

#if EDIT_1_0_n1_07
	n = sprintf(buf0, "The rain in %.*spain falls %+-6d times\n",-4,"my ",5);
#else
	n = sprintf(buf0, "The rain in %#.*spain falls %+-6d times\n",-4,"my ",5);
#endif
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in %#.*spain falls %+-6d times\n",-4,"my ",5);
	printf("*** CC = " AINT_FMT "\n", strLength(buf1));
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);

	n = sprintf(buf0, "The rain in %#.*E pain\n", 200, d);
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in %#.*E pain\n", 200, d);
#if EDIT_1_0_n1_07
	printf("*** CC = %d\n", (int) strLength(buf1));
#else
	printf("*** CC = %d\n", strLength(buf1));
#endif
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);

#if TestLongDouble
	n = sprintf(buf0, "The rain in %#.*Lg pain\n", 200, L);
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in %#.*Lg pain\n", 200, L);
	printf("*** CC = %d\n", strLength(buf1));
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);
#endif

#if TestShort
	n = sprintf(buf0, "The rain in %+hu pain, %+hd, %u, %d\n", s, s, s, s);
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in %+hu pain, %+hd, %u, %d\n", s, s, s, s);
	printf("*** CC = %d\n", strLength(buf1));
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);
#endif

	n = sprintf(buf0, "The rain in %lu pain\n", l);
	printf("*** CC = %d\n", n);
	buf1 = strPrintf("The rain in %lu pain\n", l);
#if EDIT_1_0_n1_07
	printf("*** CC = %d\n", (int) strLength(buf1));
#else
	printf("*** CC = %d\n", strLength(buf1));
#endif
	if (!strEqual(buf0, buf1))
		printf("*** [DIFFERENT]:\n%s\n%s\n", buf0, buf1);
}

#endif
