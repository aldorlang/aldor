#include "testlib.h"
#include "debug.h"
#include "int.h"

local void testLongIs32(void);
local void verifyAIntEqual(void);

void
intTestSuite()
{
	osInit();
	dbInit();
	TEST(testLongIs32);
	TEST(verifyAIntEqual);
	dbFini();
}

local void
testLongIs32()
{
	IF_LongOver32Bits(
		int i;
		testTrue("t1", longIsInt32(-1L));
		testFalse("t2", longIsInt32(0x100000000L));
		testFalse("t3", longIsInt32(-0x100000000L));
		for (i=0; i<30; i++)
			testTrue("tn", longIsInt32(1L<<i));
		for (i=0; i<31; i++)
			testTrue("tn", longIsInt32(- (1L<<i)));
		);
	testTrue("t1", longIsInt32(0));
	testTrue("t1", longIsInt32(0x7FFFFFFF));
	testTrue("t1", longIsInt32(-1*0x7FFFFFFF));
	testTrue("t1", longIsInt32(-1*0x7FFFFFFF-1));
}

local void
verifyAIntEqual()
{
	testFalse("t1", aintEqual(0, 1));
	testTrue("t1", aintEqual(0, 0));
}
