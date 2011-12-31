#include "axlobs.h"
#include "ostream.h"
#include "testlib.h"

local void testFormat1();
local void testFormat2();
local void testFormat3();
local void testFormat4();

void formatTest()
{
	TEST(testFormat1);
	TEST(testFormat2);
	TEST(testFormat3);
	TEST(testFormat4);
}

int displayPtr(OStream ostream, Pointer p)
{
	ostreamPrintf(ostream, "[%x]", p);
}

local void 
testFormat1()
{
	fmtRegister("x", displayPtr);
	String s = strPrintf("Hello: %px", 0);
	testStringEqual("test1", "Hello: [0]", s);
}

local void 
testFormat2()
{
	fmtRegister("x", displayPtr);
	String s = strPrintf("Hello: %pxBlah", 0);
	testStringEqual("test2", "Hello: [0]Blah", s);
}

local void 
testFormat3()
{
	String s = strPrintf("Hello: %pBlah", 0);
	testStringEqual("test3", "Hello: (nil)Blah", s);
}

local void 
testFormat4()
{
	OStream ostream = ostreamNewFrDevNull();
	char *fmt = "Hello: %pBlah";
	String s = strPrintf(fmt, 0);
	int c = ostreamPrintf(ostream, fmt, 0);

	testStringEqual("test4a", "Hello: (nil)Blah", s);
	testIntEqual("test4b", strlen(s), c);
}
