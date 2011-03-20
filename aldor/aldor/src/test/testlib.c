#include "test/testlib.h"
#include <stdlib.h>
#include <stdarg.h>

static int failed = 0;
static int count = 0;

local void testFail(String testname, String fmt, ...);

void
testStringEqual(String testName, String s1, String s2)
{
	count++;
	if (strcmp(s1, s2) == 0) {
		return;
	}
	testFail(testName, "failed; expected %s, got %s", s1, s2);
}

local void 
testFail(String testName, String fmt, ...)
{
	va_list argp;

	printf("[%s]:", testName);
	va_start(argp, fmt);
	vprintf(fmt, argp);
	printf("\n");
	va_end(argp);

	failed++;
}

void
testShowSummary()
{
	printf("Tests Failed: %d/%d\n", failed, count);
}

int 
testAllPassed()
{
	return failed == 0;
}
