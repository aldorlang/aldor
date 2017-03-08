#include "axlobs.h"
#include "foam.h"
#include "testlib.h"
#include "int.h"

local void testList();

local Bool eqMod5(AInt, AInt);

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
	AIntList l, m;

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

	l = listList(AInt)(3, 1, 2, 3);
	testTrue("eq0", listEqual(AInt)(listNRemove(AInt)(l, 99, NULL), l, aintEqual));
	testTrue("eq0", listEqual(AInt)(listNRemove(AInt)(l, 2, NULL), listList(AInt)(2, 1, 3), aintEqual));

	l = listList(AInt)(3, 1, 2, 3);
	testTrue("eq0", listEqual(AInt)(listNRemove(AInt)(l, 7, eqMod5),
					listList(AInt)(2, 1, 3), aintEqual));
}

local Bool
eqMod5(AInt a, AInt b)
{
	return a % 5 == b % 5;
}
