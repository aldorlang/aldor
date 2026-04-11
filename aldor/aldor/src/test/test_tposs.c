#include "abquick.h"
#include "testlib.h"
#include "testall.h"
#include "tposs.h"
#include "tform.h"

local void testMapType();
local void testFilterEmpty();
local void testHasUTForm();

void
tpossTest()
{
	init();
	TEST(testMapType);
	TEST(testFilterEmpty);
	TEST(testHasUTForm);
	fini();
}

local void
testMapType()
{
	TPoss tp = tpossEmpty();
	
	testFalse("", tpossHasMapType(tp));
	testFalse("", tpossHasNonMapType(tp));

	tpossAdd1(tp, tfMap(tfNone(), tfNone()));
	testTrue("", tpossHasMapType(tp));
	testFalse("", tpossHasNonMapType(tp));

	tpossAdd1(tp, tfNone());
	testTrue("", tpossHasMapType(tp));
	testTrue("", tpossHasNonMapType(tp));

}

local void
testFilterEmpty()
{
	TPoss tp = tpossEmpty();
	TPoss tpr;

	tpr = tpossFilterEmpty(tp);
	testIntEqual("", 0, tpossCount(tpr));

	tpossAdd1(tp, tfMulti(0));
	tpr = tpossFilterEmpty(tp);
	testIntEqual("", 0, tpossCount(tpr));

	tpossAdd1(tp, tfMulti(1, tfMap(tfNone(), tfNone())));
	tpr = tpossFilterEmpty(tp);
	testIntEqual("", 1, tpossCount(tpr));
}

local void
testHasUTForm()
{
	TPoss tp = tpossEmpty();

	TForm tf1 = tfCross(0);
	TForm tf2 = tfCross(1, tfCross(0));

	tpossAdd1(tp, tf1);
	testFalse("", tpossHasUTForm(tp, utformNewConstant(tf2)));
	testTrue("", tpossHasUTForm(tp, utformNewConstant(tf1)));
}
