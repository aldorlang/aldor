#include "test/testlib.h"
#include <stdlib.h>
#include <stdarg.h>

static int failed = 0;
static int count = 0;

local void testFail(String testname, String fmt, ...);

void showTest(char *name, void (*fn)(void))
{
	printf("(Starting test %s\n", name);
	fn();
	printf(" Test %s complete)\n", name);
}

void
testStringEqual(String testName, String s1, String s2)
{
	count++;
	if (strcmp(s1, s2) == 0) {
		return;
	}
	testFail(testName, "failed; expected %s, got %s", s1, s2);
}

void
testIntEqual(String testName, int i1, int i2)
{
	count++;
	if (i1 == i2) {
		return;
	}
	testFail(testName, "failed; expected %d, got %d", i1, i2);
}

void
testTrue(String testName, Bool flg)
{
	count++;
	if (flg) {
		return;
	}
	testFail(testName, "failed; expected true, got %d", flg);
}

void
testFalse(String testName, Bool flg)
{
	count++;
	if (!flg) {
		return;
	}
	testFail(testName, "failed; expected false, got %d", flg);
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
