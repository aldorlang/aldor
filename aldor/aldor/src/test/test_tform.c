#include "abquick.h"
#include "axlobs.h"
#include "format.h"
#include "stab.h"
#include "testlib.h"

local void testTFormFormat();
local void testTFormSyntaxConditions();
local void testTFormFormatOne(String name, String expect, TForm tf);

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);
void finiFile(void);

void
tformTest()
{
	init();
	TEST(testTFormFormat);
	TEST(testTFormSyntaxConditions);
	fini();
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

	String s = aStrPrintf("%pAbSyn", ab);

	testStringEqual("compare strings:", s, "(Assign x y)");
}

local void
testTFormSyntaxConditions()
{
	initFile();
	Stab stab = stabPushLevel(stabFile(), sposNone, STAB_LEVEL_LARGE);
	AbSyn ab = apply2(id("->"), id("A"), id("B"));
	TForm tf = tfSyntaxFrAbSyn(stabFile(), ab);
	AbSynList cond = listList(AbSyn)(1, test(has(id("X"), id("Y"))));
	tfSyntaxConditions(stabFile(), tf, tfCondEltNew(stab, cond));
	
	finiFile();
}
