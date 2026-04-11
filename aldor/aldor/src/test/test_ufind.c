#include "testlib.h"
#include "ufind.h"

local void testUFind_0(void);
local void testUFind_1(void);

void ufindTest()
{
	init();
	TEST(testUFind_0);
	TEST(testUFind_1);
	fini();
}

local void
testUFind_0()
{
	UFTable tbl = uftNew();
	UFElt e = uftGet(tbl, 1);
	testIntEqual("", 1, uftEltCount(e));
	uftUnion(tbl, 1, 1);
	testPointerEqual("", e, uftGet(tbl, 1));
	testIntEqual("", 1, uftEltCount(e));
}

local void
testUFind_1()
{
	UFTable tbl = uftNew();
	UFElt e1, e2;

	uftUnion(tbl, 1, 2);

	e1 = uftGet(tbl, 1);
	e2 = uftGet(tbl, 2);

	testPointerEqual("", e1, e2);
	testIntEqual("", 2, uftEltCount(uftGet(tbl, 2)));
}
