#include "abquick.h"
#include "axlobs.h"
#include "scobind.h"
#include "stab.h"
#include "testlib.h"
#include "ablogic.h"
#include "symbol.h"

local void testScobind(void);
local void testScobindCondition(void);
local void testScobindConditionMulti(void);

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);
void finiFile(void);

void
scobindTest(void)
{
	init();
	TEST(testScobind);
	TEST(testScobindCondition);
	TEST(testScobindConditionMulti);
	fini();
}


local void scobindTestCheckUnique(Stab stab, Symbol sym);

local void
testScobind(void)
{
	Stab stabGlobal, stabFile, stab;
	AbSyn ab;

	initFile();
	stabGlobal = stabNewGlobal();
	stabFile = stabNewFile(stabGlobal);
	stab = stabPushLevel(stabFile, sposNone, STAB_LEVEL_LARGE);
	
	ab = abqParse("X: with == add");
	
	scopeBind(stab, ab);
	scobindTestCheckUnique(stab, symInternConst("X"));
	finiFile();
}


local void
testScobindCondition(void)
{
	Stab stabGlobal, stabFile, stab;
	AbSyn ab;
	Symbol sym_f;

	initFile();
	stabGlobal = stabNewGlobal();
	stabFile = stabNewFile(stabGlobal);
	stab = stabPushLevel(stabFile, sposNone, STAB_LEVEL_LARGE);
	
	ab = abqParse("if A then f: () -> ()");
	
	scopeBind(stab, ab);
	sym_f = symInternConst("f");
	scobindTestCheckUnique(stab, sym_f);
	
	finiFile();
}

local void
testScobindConditionMulti(void)
{
	Stab stabGlobal, stabFile, stab;
	AbSyn ab;

	initFile();
	stabGlobal = stabNewGlobal();
	stabFile = stabNewFile(stabGlobal);
	stab = stabPushLevel(stabFile, sposNone, STAB_LEVEL_LARGE);
	
	finiFile();
	return;
	/*
	  ideally, I'd like to test this here, but
	  comsgNote (used for multiple defs) doesn't play nice.
	*/
#if 0
	AbSyn ab = abqParse("if A then { f: X == 1; f: X == 1}");
	
	scopeBind(stab, ab);
	Symbol sym_f = symInternConst("f");
	scobindTestCheckUnique(stab, sym_f);
#endif
}

local void
scobindTestCheckUnique(Stab stab, Symbol sym)
{
	SymeList sl = stabGetMeanings(stab, ablogFalse(), sym);
	testIntEqual("unique", 1, listLength(Syme)(sl));
	testPointerEqual("name", sym, symeId(car(sl)));
}
