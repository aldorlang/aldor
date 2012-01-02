#include "axlobs.h"
#include "testlib.h"


void testStabIsChild();

void stabTest()
{
	TEST(testStabIsChild);
}


void testStabIsChild()
{
	Stab global = stabNewGlobal();
	Stab root = stabNewFile(global);

	Stab c1 = stabPushLevel(root, sposNone, 0);
	Stab c2 = stabPushLevel(root, sposNone, 0);
	
	Stab c11 = stabPushLevel(c1, sposNone, 0);

	testTrue("selfparent-1", stabIsChild(root, root));
	testTrue("selfparent-2", stabIsChild(c1, c1));
	testTrue("selfparent-3", stabIsChild(c2, c2));
	testTrue("selfparent-3", stabIsChild(c11, c11));

	testTrue("t1", stabIsChild(root, c1));
	testTrue("t1", stabIsChild(root, c11));
	testTrue("t1", stabIsChild(root, c2));

	testFalse("t2", stabIsChild(c1, root));
	testFalse("t2", stabIsChild(c2, root));
	testFalse("t2", stabIsChild(c11, root));

	testTrue("c11", stabIsChild(c1, c11));
	testFalse("c11", stabIsChild(c11, c1));
	testFalse("c11", stabIsChild(c2, c1));
	testFalse("c11", stabIsChild(c2, c11));
}
