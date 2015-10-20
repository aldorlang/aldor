#include "abquick.h"
#include "testlib.h"
#include "testall.h"
#include "tposs.h"
#include "tform.h"

local void testMapType();

void
tpossTest()
{
	init();
	TEST(testMapType);
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

