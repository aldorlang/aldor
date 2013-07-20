#include "axlobs.h"
#include "abquick.h"
#include "format.h"
#include "testlib.h"
#include "strops.h"

local void init(void);
local void fini(void);
local void testAbSynFormat();
local void testAbSynFormatList();
local void testAbParse();
local void testExquo();
local void testAbContains();
local void testAbSynFormatOne(String name, String expect, AbSyn absyn);

void
absynTest()
{
	init();
	TEST(testAbSynFormat);
	TEST(testAbSynFormatList);
	TEST(testAbParse);
	TEST(testExquo);
	TEST(testAbContains);
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
testExquo()
{
	AbSyn ab1 = abqParse("5 quo 7");
	testIntEqual("isApply", AB_Apply, abTag(ab1));
	AbSyn ab2 = abqParse("5 exquo 7");
	testIntEqual("isApply", AB_Apply, abTag(ab2));
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

	String s = aStrPrintf("%pAbSyn", ab);

	testStringEqual("compare strings:", s, "(Assign x y)");
}

local void
testAbContains()
{
	AbSyn ab = abqParse("x := y");
	testTrue("1", abContains(ab, id("x")));
	testTrue("2", abContains(ab, id("y")));
	testTrue("3", abContains(ab, ab));
	testFalse("4", abContains(ab, id("z")));

	AbSyn ab2 = abqParse("A has B");
	testTrue("1", abContains(ab2, id("A")));

}
