#include "axlobs.h"
#include "testlib.h"
#include "abquick.h"

local void testScobind();
local void testScobindCondition();
local void testScobindConditionMulti();

void
scobindTest()
{
	init();
	TEST(testScobind);
	TEST(testScobindCondition);
	TEST(testScobindConditionMulti);
	fini();
}


local void scobindTestCheckUnique(Stab stab, Symbol sym);

local void
testScobind()
{
	initFile();
	Stab stabGlobal = stabNewGlobal();
	Stab stabFile = stabNewFile(stabGlobal);
	Stab stab = stabPushLevel(stabFile, sposNone, STAB_LEVEL_LARGE);
	
	AbSyn ab = abqParse("X: with == add");
	
	scopeBind(stab, ab);
	scobindTestCheckUnique(stab, symInternConst("X"));
	finiFile();
}


local void
testScobindCondition()
{
	initFile();
	Stab stabGlobal = stabNewGlobal();
	Stab stabFile = stabNewFile(stabGlobal);
	Stab stab = stabPushLevel(stabFile, sposNone, STAB_LEVEL_LARGE);
	
	AbSyn ab = abqParse("if A then f: () -> ()");
	
	scopeBind(stab, ab);
	Symbol sym_f = symInternConst("f");
	scobindTestCheckUnique(stab, sym_f);
	
	finiFile();
}

local void
testScobindConditionMulti()
{
	initFile();
	Stab stabGlobal = stabNewGlobal();
	Stab stabFile = stabNewFile(stabGlobal);
	Stab stab = stabPushLevel(stabFile, sposNone, STAB_LEVEL_LARGE);
	
	finiFile();
	return;
	/*
	  ideally, I'd like to test this here, but
	  comsgNote (used for multiple defs) doesn't play nice.

	AbSyn ab = abqParse("if A then { f: X == 1; f: X == 1}");
	
	scopeBind(stab, ab);
	Symbol sym_f = symInternConst("f");
	scobindTestCheckUnique(stab, sym_f);
	*/
}

local void
scobindTestCheckUnique(Stab stab, Symbol sym)
{
	SymeList sl = stabGetMeanings(stab, ablogFalse(), sym);
	testIntEqual("unique", 1, listLength(Syme)(sl));
	testPointerEqual("name", sym, symeId(car(sl)));
}
	

