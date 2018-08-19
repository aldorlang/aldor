#include "testall.h"
#include "testlib.h"
#include "ttable.h"
#include "list.h"

DECLARE_LIST(String);
DECLARE_TSET(String);

CREATE_TSET(String);

local void testTSet(void);
local void testTSetIter(void);

void
tsetTestSuite()
{
	init();
	TEST(testTSet);
	TEST(testTSetIter);
	fini();
}


local void
testTSet()
{
	StringTSet set;
	String x = "x";
	String y = "y";

	set = tsetCreate(String)();
	testFalse("", tsetMember(String)(set, x));
	testFalse("", tsetMember(String)(set, y));

	tsetAdd(String)(set, x);
	testTrue("", tsetMember(String)(set, x));
	testFalse("", tsetMember(String)(set, y));
	testIntEqual("", 1, tsetSize(String)(set));

	tsetAdd(String)(set, y);
	testTrue("", tsetMember(String)(set, x));
	testTrue("", tsetMember(String)(set, y));
	testIntEqual("", 2, tsetSize(String)(set));

	tsetRemove(String)(set, x);
	testFalse("", tsetMember(String)(set, x));
	testTrue("", tsetMember(String)(set, y));
	testIntEqual("", 1, tsetSize(String)(set));

	tsetRemove(String)(set, y);
	testFalse("", tsetMember(String)(set, x));
	testFalse("", tsetMember(String)(set, y));
	testIntEqual("", 0, tsetSize(String)(set));

	tsetFree(String)(set);
}

local void
testTSetIter()
{
	StringTSet set;
	StringTSetIter iter;
	String someString;
	
	set = tsetCreate(String)();
	tsetAdd(String)(set, "x");

	iter = tsetIter(String)(set);
	testTrue("", tsetIterHasNext(String)(iter));

	someString = tsetIterElt(String)(iter);
	testPointerEqual("", someString, "x");
	iter = tsetIterNext(String)(iter);
	
	testFalse("", tsetIterHasNext(String)(iter));
}
