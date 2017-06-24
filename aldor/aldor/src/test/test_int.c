#include "debug.h"
#include "int.h"
#include "opsys.h"
#include "testlib.h"

local void testLongIs32(void);
local void verifyAIntEqual(void);
local void verifyAIntAbsorbingSum(void);

void
intTestSuite()
{
	osInit();
	dbInit();
	TEST(testLongIs32);
	TEST(verifyAIntEqual);
	TEST(verifyAIntAbsorbingSum);
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


local void
verifyAIntAbsorbingSum()
{
	testTrue("t1", aintAbsorbingSum(100, 99, 1) == 100);
	testTrue("t2", aintAbsorbingSum(100, 99, 0) == 99);
	testTrue("t3", aintAbsorbingSum(100, 99, 10) == 100);

	testTrue("t4", aintAbsorbingSum(1<<30, (1<<30) - 10, 20) == 1<<30);
}
