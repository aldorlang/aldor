#include "testall.h"

int 
main(int argc, char *argv[])
{
	fnameTest();
	testShowSummary();
	
	return testAllPassed() ? 0 : 1;
}
