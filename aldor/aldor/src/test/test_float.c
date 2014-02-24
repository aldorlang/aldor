#include "testlib.h"
#include "debug.h"

local void testFloatSizes(void);

void
floatTestSuite()
{
	osInit();
	dbInit();
	TEST(testFloatSizes);
	dbFini();
}

local void
testFloatSizes()
{
	fprintf(dbOut, "sizeof float: %ld\n", sizeof(SFloat));
	fprintf(dbOut, "sizeof dfloat: %ld\n", sizeof(DFloat));

	testIntEqual("float", 4, sizeof(SFloat));
	testIntEqual("float", 8, sizeof(DFloat));

}


