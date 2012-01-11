#include "axlobs.h"
#include "testlib.h"
#include "abquick.h"

local void init(void);
local void fini(void);
local void testTFormFormat();
local void testAbSynFormatList();
local void testAbParse();
local void testTFormFormatOne(String name, String expect, TForm tf);

void
tformTest()
{
	init();
	TEST(testTFormFormat);
	fini();
}

local void
init()
{
	tfInit();
}

local void
fini()
{
}

local void
testTFormFormat()
{
	TForm tf = tfSyntaxFrAbSyn(stabFile(), id("x"));
	testTFormFormatOne("id", "<* Syntax (* x *) *>", tf);
	testTFormFormatOne("id", "<* NULL *>", NULL);
}

local void
testTFormFormatOne(String name, String expect, TForm tf)
{
	Buffer b = bufNew();
	int cc = bufPrintf(b, "[%pTForm:%pTForm]", tf, tf);
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
