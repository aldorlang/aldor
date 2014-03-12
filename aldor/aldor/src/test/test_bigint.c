#include "bigint.h"
#include "strops.h"
#include "testlib.h"

local void testToFromString(void);
local void testBIntNew(void);

void
bintTestSuite()
{
	init();
	TEST(testToFromString);
	TEST(testBIntNew);
	fini();
}

local void testCanonical(char *nText);
local void
testToFromString()
{
	testCanonical("0");
	testCanonical("1");
	testCanonical("-1");
	testCanonical("1000");
	testCanonical("2349587234");
	testCanonical("23434534");
	testCanonical("2343453477777777777777");

	testCanonical("-1000");
	testCanonical("-2349587234");
	testCanonical("-23434534");
	testCanonical("-2343453477777777777777");
}

local void
testCanonical(char *nText)
{
	BInt n = bintFrString(nText);
	char *asText = bintToString(n);
	testStringEqual(strConcat("check ", nText), nText, asText);

}

local void
testBIntNew()
{
	int i;
	for (i = 0; i < bitsizeof(long) - 2; i++)
	{
		BInt b, nb;
		b = bintNew(1L << i);
		testTrue("bit", bintBit(b, i));
		nb = bintNew(-1L << i);
		testTrue("pos", bintIsPos(b));
		testTrue("neg", bintIsNeg(nb));
		testTrue("sum.. ", bintEQ(bint0, bintPlus(b, nb)));
	}
}

