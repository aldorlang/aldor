#include "axlobs.h"
#include "abquick.h"
#include "testlib.h"

local void init(void);
local void fini(void);
local void testAbSynFormat();
local void testAbSynFormatList();
local void testAbParse();
local void testAbSynFormatOne(String name, String expect, AbSyn absyn);

void
absynTest()
{
	init();
	TEST(testAbSynFormat);
	TEST(testAbSynFormatList);
	TEST(testAbParse);
	fini();
}

local void
init()
{

}

local void
fini()
{

}

local void
testAbSynFormat()
{
	testAbSynFormatOne("id", "x", id("x"));
	testAbSynFormatOne("id", "(Declare x T)", declare(id("x"), id("T")));
}


local void
testAbSynFormatList()
{
	{
		AbSynList l = listNil(AbSyn);
		Buffer b = bufNew();
		bufPrintf(b, "%pAbSynList", l);
		String result = bufLiberate(b);
		testStringEqual("emptyList", "[]", result);
	}
	{
		AbSynList l = listList(AbSyn)(1, id("x"));
		Buffer b = bufNew();
		bufPrintf(b, "+%pAbSynList+", l);
		String result = bufLiberate(b);
		testStringEqual("twoitem", "+[x]+", result);
	}
	{
		AbSynList l = listList(AbSyn)(2, id("x"), id("y"));
		Buffer b = bufNew();
		bufPrintf(b, "+%pAbSynList+", l);
		String result = bufLiberate(b);
		testStringEqual("twoitem", "+[x, y]+", result);
	}
}

local void
testAbSynFormatOne(String name, String expect, AbSyn absyn)
{
	Buffer b = bufNew();
	int cc = bufPrintf(b, "[%pAbSyn:%pAbSyn]", absyn, absyn);
	String s = bufLiberate(b);
	String fullExpect = strlConcat("[", expect, ":", expect, "]", NULL);
	testStringEqual(name, fullExpect, s);

	testIntEqual("retlen", strlen(fullExpect), cc);
	strFree(fullExpect);
	strFree(s);
}

local void
testAbParse()
{
	AbSyn ab = abqParse("x := y");

	String s = asprintf("%pAbSyn", ab);

	testStringEqual("compare strings:", s, "(Assign x y)");
}
