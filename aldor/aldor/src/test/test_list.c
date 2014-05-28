#include "axlobs.h"
#include "foam.h"
#include "testlib.h"

local void testList();

void
listTestSuite()
{
	init();
	TEST(testList);
	fini();
}

local void
testList()
{
	AIntList l;

	l = listListNull(AInt)(0);
	testIntEqual("", 0, listLength(AInt)(l));

	l = listListNull(AInt)(1, 0);
	testIntEqual("", 1, listLength(AInt)(l));
	testIntEqual("", 1, car(l));

	l = listListNull(AInt)(1, 2, 0);
	testIntEqual("", 2, listLength(AInt)(l));
	testIntEqual("", 1, car(l));
	testIntEqual("", 2, car(cdr(l)));

	l = listListNull(AInt)(1, 2, 3,0);
	testIntEqual("L", 3, listLength(AInt)(l));
	testIntEqual("0", 1, listElt(AInt)(l, 0));
	testIntEqual("1", 2, listElt(AInt)(l, 1));
	testIntEqual("2", 3, listElt(AInt)(l, 2));

	l = listList(AInt)(0);
	testIntEqual("", 0, listLength(AInt)(l));

	l = listList(AInt)(1, 1);
	testIntEqual("", 1, listLength(AInt)(l));
	testIntEqual("", 1, car(l));

	l = listList(AInt)(2, 1, 2);
	testIntEqual("", 2, listLength(AInt)(l));
	testIntEqual("", 1, car(l));
	testIntEqual("", 2, car(cdr(l)));

	l = listList(AInt)(3, 1, 2, 3);
	testIntEqual("L", 3, listLength(AInt)(l));
	testIntEqual("0", 1, listElt(AInt)(l, 0));
	testIntEqual("1", 2, listElt(AInt)(l, 1));
	testIntEqual("2", 3, listElt(AInt)(l, 2));
}

