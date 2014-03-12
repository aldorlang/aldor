#include "abquick.h"
#include "axlobs.h"
#include "format.h"
#include "stab.h"
#include "testlib.h"
#include "strops.h"

local void testTFormFormat(void);
local void testTFormSyntaxConditions(void);
local void testTFormFormatOne(String name, String expect, TForm tf);

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);
void finiFile(void);

void
tformTest(void)
{
	init();
	TEST(testTFormFormat);
	TEST(testTFormSyntaxConditions);
	fini();
}

local void
testTFormFormat(void)
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
testAbParse(void)
{
	AbSyn ab = abqParse("x := y");

	String s = aStrPrintf("%pAbSyn", ab);

	testStringEqual("compare strings:", s, "(Assign x y)");
}

local void
testTFormSyntaxConditions(void)
{
	Stab stab;
	AbSyn ab;
	TForm tf;
	AbSynList cond;

	initFile();
	stab = stabPushLevel(stabFile(), sposNone, STAB_LEVEL_LARGE);
	ab = apply2(id("->"), id("A"), id("B"));
	tf = tfSyntaxFrAbSyn(stabFile(), ab);
	cond = listList(AbSyn)(1, test(has(id("X"), id("Y"))));
	tfSyntaxConditions(stabFile(), tf, tfCondEltNew(stab, cond));
	
	finiFile();
}
