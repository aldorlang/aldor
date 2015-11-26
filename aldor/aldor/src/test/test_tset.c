#include "testall.h"
#include "testlib.h"
#include "ttable.h"
#include "list.h"

DECLARE_LIST(String);
DECLARE_TSET(String);

CREATE_TSET(String);

local void testTSet(void);

void
tsetTestSuite()
{
	init();
	TEST(testTSet);
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

	tsetAdd(String)(set, y);
	testTrue("", tsetMember(String)(set, x));
	testTrue("", tsetMember(String)(set, y));

	tsetRemove(String)(set, x);
	testFalse("", tsetMember(String)(set, x));
	testTrue("", tsetMember(String)(set, y));

	tsetRemove(String)(set, y);
	testFalse("", tsetMember(String)(set, x));
	testFalse("", tsetMember(String)(set, y));

	tsetFree(String)(set);
}
