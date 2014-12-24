#include "axlobs.h"
#include "format.h"
#include "testlib.h"
#include "strops.h"
#include "formatters.h"

local void testFormat1();
local void testFormat2();
local void testFormat3();
local void testFormat4();
local void testFormat5();
local void testFormat6();
local void testFormatBool();

void
formatTest()
{
	TEST(testFormat1);
	TEST(testFormat2);
	TEST(testFormat3);
	TEST(testFormat4);
	TEST(testFormat5);
	TEST(testFormat6);
	TEST(testFormatBool);
}

int
displayInt(OStream ostream, int n)
{
	return ostreamPrintf(ostream, "[%d]", n);
}

int
displayPtr(OStream ostream, Pointer p)
{
	return ostreamPrintf(ostream, "[%x]", p);
}

local void 
testFormat1()
{
	String s;
	fmtRegister("x", displayPtr);
	s = strPrintf("Hello: %px", 0);
	testStringEqual("test1", "Hello: [0]", s);
}

local void 
testFormat2()
{
	String s;
	fmtRegister("x", displayPtr);
	s = strPrintf("Hello: %pxBlah", 0);
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

local void
testFormat5()
{
	String s;
	fmtRegisterFull("z", displayPtr, false);
	s = strPrintf("Hello: %pz", 0);
	testStringEqual("test2", "Hello: (nil)", s);

	s = strPrintf("Hello: %pz", 1);
	testStringEqual("test2", "Hello: [1]", s);
}

local void
testFormat6()
{
	char *s;
	fmtRegisterI("i", displayInt);

	s = strPrintf("Hello: %oi %d", 0, 999);
	testStringEqual("test2", "Hello: [0] 999", s);

	s = strPrintf("Hello: %oi", 1);
	testStringEqual("test2", "Hello: [1]", s);

	s = strPrintf("Hello: %oi", -1);
	testStringEqual("test2", "Hello: [-1]", s);
}

local void
testFormatBool()
{
	char *s;
	fmttsInit();

	testStringEqual("", "[true]", strPrintf("[%oBool]", true));
	testStringEqual("", "[false]", strPrintf("[%oBool]", false));
	testStringEqual("", "[Bool[123]]", strPrintf("[%oBool]", 123));
}
