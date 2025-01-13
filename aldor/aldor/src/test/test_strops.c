#include "debug.h"
#include "strops.h"
#include "testlib.h"

local void testStrIsEmpty();

void
stropsTestSuite()
{
	testStrIsEmpty();
}

local void
testStrIsEmpty()
{
	testTrue("t1", strIsEmpty(""));
	testFalse("t2", strIsEmpty(" "));
}

