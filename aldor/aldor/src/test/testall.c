#include <stdlib.h>
#include <stdio.h>
#include "testall.h"

static int theArgc;
static char **theArgv;

extern int fluidLevel;

int 
main(int argc, char *argv[])
{
	theArgc = argc;
	theArgv = argv;

	if (testShouldRun("printf")) printfTest();
	if (testShouldRun("ostream")) ostreamTest();
	if (testShouldRun("fname")) fnameTest();
	if (testShouldRun("foam")) foamTest();
	if (testShouldRun("format")) formatTest();
	if (testShouldRun("tinfer")) tinferTest();
	if (testShouldRun("stab")) stabTest();
	if (testShouldRun("srcpos")) srcposTest();
	if (testShouldRun("absyn")) absynTest();

	testIntEqual("fluidlevel", 0, fluidLevel);

	testShowSummary();
	
	return testAllPassed() ? 0 : 1;
}

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
