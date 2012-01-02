#include "axlobs.h"
#include "abquick.h"
#include "testlib.h"

void init(void);
void fini(void);
void testAbSynFormat();
void testAbSynFormatList();
void testAbSynFormatOne(String name, String expect, AbSyn absyn);

void
absynTest()
{
	init();
	TEST(testAbSynFormat);
	TEST(testAbSynFormatList);
	fini();
}

void
testAbSynFormat()
{
	testAbSynFormatOne("id", "x", id("x"));
	testAbSynFormatOne("id", "(Declare x T)", declare(id("x"), id("T")));
}

void
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

void
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
