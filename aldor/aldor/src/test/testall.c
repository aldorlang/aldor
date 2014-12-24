#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "testall.h"
#include "testlib.h"

static int theArgc;
static char **theArgv;

extern int fluidLevel;

int
testShouldRun(char *name)
{
	int i;

	if (theArgc == 1) {
		return 1;
	}
	for (i=1; i<theArgc; i++) {
		if (strcmp(theArgv[i], name) == 0) {
			return 1;
		}
	}
	return 0;
}

int 
main(int argc, char *argv[])
{
	theArgc = argc;
	theArgv = argv;

	if (testShouldRun("printf")) printfTest();
	if (testShouldRun("ostream")) ostreamTest();
	if (testShouldRun("float")) floatTestSuite();
	if (testShouldRun("int")) intTestSuite();
	if (testShouldRun("bigint")) bintTestSuite();
	if (testShouldRun("errorset")) errorSetTestSuite();
	if (testShouldRun("bitv")) bitvTestSuite();
	if (testShouldRun("list")) listTestSuite();
	if (testShouldRun("fname")) fnameTest();
	if (testShouldRun("archive")) archiveTestSuite();
	if (testShouldRun("foam")) foamTest();
	if (testShouldRun("format")) formatTest();
	if (testShouldRun("flog")) flogTest();
	if (testShouldRun("java")) javaTestSuite();
	if (testShouldRun("jflow")) jflowTest();
	if (testShouldRun("tinfer")) tinferTest();
	if (testShouldRun("stab")) stabTest();
	if (testShouldRun("srcpos")) srcposTest();
	if (testShouldRun("absyn")) absynTest();
	if (testShouldRun("abnorm")) abnormTest();
	if (testShouldRun("abcheck")) abcheckTest();
	if (testShouldRun("ablog")) ablogTest();
	if (testShouldRun("tform")) tformTest();
	if (testShouldRun("scobind")) scobindTest();
	if (testShouldRun("syme")) symeTest();
	if (testShouldRun("tibup")) tibupTest();
	if (testShouldRun("tfsat")) tfsatTest();
	if (testShouldRun("retype")) retypeTest();
	if (testShouldRun("genfoam")) genfoamTestSuite();
	if (testShouldRun("utype")) utypeTestSuite();

	testIntEqual("fluidlevel", 0, fluidLevel);

	testShowSummary();
	
	return testAllPassed() ? 0 : 1;
}
