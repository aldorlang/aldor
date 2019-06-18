#include "testlib.h"

#include "ablogic.h"
#include "cmdline.h"
#include "comsg.h"
#include "debug.h"
#include "macex.h"
#include "opsys.h"
#include "optfoam.h"
#include "phase.h"
#include "scobind.h"
#include "sexpr.h"
#include "spesym.h"
#include "stab.h"
#include "tform.h"
#include "tinfer.h"
#include "formatters.h"

#include <stdlib.h>
#include <stdarg.h>

static int failed = 0;
static int count = 0;

local void testFail(String testname, String fmt, ...);

extern int fluidLevel;

void 
showTest(char *name, void (*fn)(void))
{
	int wasFailed = failed;
	int localFluidLevel = fluidLevel;
	printf("(Starting test %s\n", name);
	fn();
	testIntEqual("fluidlevel", localFluidLevel, fluidLevel);

	printf(" Test %s %s)\n", name,
	       (wasFailed == failed) ? "complete" : "FAILED");
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
	testFail(testName, "failed; expected true, got %oBool", flg);
}

void
testFalse(String testName, Bool flg)
{
	count++;
	if (!flg) {
		return;
	}
	testFail(testName, "failed; expected false, got %oBool", flg);
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

static Bool inFile = false;

void
initFile()
{
	if (inFile) {
		testFail("<init>", "missing 'finiFile()'");
	}
	macexInitFile();
	comsgInit();
	scobindInitFile();
	stabInitFile();
	inFile = true;
}

void
finiFile()
{
	if (!inFile) {
		testFail("<init>", "missing 'initFile()'");
	}

	saveAndEmptyAllPhaseSymbolData();

	scobindFiniFile();
	stabFiniFile();
	comsgFini();
	macexFiniFile();

	cmdDebugReset();
	inFile = false;
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
	fmttsInit();
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

	if (inFile)
		testFail("<fini>", "Missing fini");

	dbFini();
}
