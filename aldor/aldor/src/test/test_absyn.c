#include "axlobs.h"
#include "abquick.h"
#include "testlib.h"

void init(void);
void fini(void);
void testAbSynFormat();
void testAbSynFormatOne(String name, String expect, AbSyn absyn);

void
absynTest()
{
	init();
	TEST(testAbSynFormat);
	fini();
}

void
testAbSynFormat()
{
	testAbSynFormatOne("id", "x", id("x"));
	testAbSynFormatOne("id", "(Declare x T)", declare(id("x"), id("T")));
}

void
testAbSynFormatOne(String name, String expect, AbSyn absyn)
{
	Buffer b = bufNew();
	bufPrintf(b, "[%pAbSyn:%pAbSyn]", absyn, absyn);
	String s = bufLiberate(b);
	String fullExpect = strlConcat("[", expect, ":", expect, "]", NULL);
	testStringEqual(name, fullExpect, s);
	strFree(fullExpect);
	strFree(s);
}
