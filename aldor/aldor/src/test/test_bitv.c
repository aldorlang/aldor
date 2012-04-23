#include "axlobs.h"
#include "foam.h"
#include "testlib.h"

local void testBitvToInt();
local void testBitvCount();

void
bitvTestSuite()
{
	init();
	TEST(testBitvToInt);
	TEST(testBitvCount);
	fini();
}

local void
testBitvToInt()
{
	BitvClass clss = bitvClassCreate(10);
	int i;

	Bitv bitv = bitvFromInt(clss, 0);
	bitvPrintDb(clss, bitv);
	testIntEqual("", 0, bitvCount(clss, bitv));

	for (i=0; i<1024; i++) {
		Bitv bits = bitvFromInt(clss, i);
		int back = bitvToInt(clss, bits);

		testIntEqual("Should be equal", i, back);
		bitvFree(bits);
	}

	for (i=0; i<10; i++) {
		Bitv bitv = bitvFromInt(clss, 1<<i);
		testIntEqual("ZZ", 1, bitvCount(clss, bitv));
		bitvFree(bitv);
	}
	for (i=0; i<10; i++) {
		Bitv bitv = bitvFromInt(clss, (1<<i) - 1);
		testIntEqual("AA", i, bitvCount(clss, bitv));
		bitvFree(bitv);
	}
	bitvClassDestroy(clss);
	
}

local void
testBitvCount()
{
	BitvClass clss = bitvClassCreate(10);
	Bitv bitv = bitvNew(clss);
	int i;
	bitvClearAll(clss, bitv);
	testIntEqual("A", 0, bitvCount(clss, bitv));
	bitvSetAll(clss, bitv);
	testIntEqual("B", 10, bitvCount(clss, bitv));
	for (i=0; i<10; i++) {
		testIntEqual("C", i, bitvCountTo(clss, bitv, i));
	}
	bitvClassDestroy(clss);
}
