#include "testlib.h"
#include <stdlib.h>
#include <stdarg.h>

static int failed = 0;
static int count = 0;

local void testFail(String testname, String fmt, ...);

extern int fluidLevel;

void 
showTest(char *name, void (*fn)(void))
{
	int localFluidLevel = fluidLevel;
	printf("(Starting test %s\n", name);
	fn();
	testIntEqual("fluidlevel", localFluidLevel, fluidLevel);

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
testAIntEqual(String testName, AInt i1, AInt i2)
{
	count++;
	if (i1 == i2) {
		return;
	}
	testFail(testName, "failed; expected %pAInt, got %pAInt", i1, i2);
}

void
testIntIsNotZero(String testName, int i1)
{
	count++;
	if (i1 != 0) {
		return;
	}
	testFail(testName, "failed; expected non-zero value");
}

void
testPointerEqual(String testName, void *i1, void *i2)
{
	count++;
	if (i1 == i2) {
		return;
	}
	testFail(testName, "failed; expected %p, got %p", i1, i2);
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

void
testIsNull(String testName, void *p)
{
	count++;
	if (p == NULL) {
		return;
	}
	testFail(testName, "failed; expected null, got %d", p);
}


void
testIsNotNull(String testName, void *p)
{
	count++;
	if (p != NULL) {
		return;
	}
	testFail(testName, "failed; expected non-null, got %d", p);
}


local void 
testFail(String testName, String fmt, ...)
{
	va_list argp;

	printf("[%s]:", testName);
	va_start(argp, fmt);
	avprintf(fmt, argp);
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

void
initFile()
{
	macexInitFile();
	comsgInit();
	scobindInitFile();
	stabInitFile();
}

void
finiFile()
{
	scobindFiniFile();
	stabFiniFile();
	comsgFini();
	macexFiniFile();

	cmdDebugReset();
}

void
init()
{
	osInit();
	sxiInit();
	keyInit();
	ssymInit();
	dbInit();
	stabInitGlobal();
	tfInit();
	foamInit();
	optInit();
	tinferInit();

	sposInit();
	ablogInit();
	comsgInit();
}

void
fini()
{
	saveAndEmptyAllPhaseSymbolData();

	dbFini();
}
