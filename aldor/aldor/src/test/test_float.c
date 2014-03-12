#include "debug.h"
#include "opsys.h"
#include "testlib.h"

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
	fprintf(dbOut, "sizeof float: " LENGTH_FMT "\n", sizeof(SFloat));
	fprintf(dbOut, "sizeof dfloat: " LENGTH_FMT "\n", sizeof(DFloat));

	testIntEqual("float", 4, sizeof(SFloat));
	testIntEqual("float", 8, sizeof(DFloat));

}


