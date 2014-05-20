#include "axlobs.h"
#include "format.h"
#include "testlib.h"
#include "errorset.h"
#include "list.h"

local void testErrorSet1();

void
errorSetTestSuite()
{
	TEST(testErrorSet1);
}

local void
testErrorSet1()
{
	String someRandomString = "OMG Ponies";
	ErrorSet e;

	e = errorSetNew();
	testFalse("1", errorSetHasErrors(e));

	errorSetAdd(e, someRandomString);
	testTrue("2", errorSetHasErrors(e));

	testStringEqual("3", someRandomString, car(errorSetErrors(e)));
	errorSetFree(e);

	e = errorSetNew();
	errorSetPrintf(e, true, "%s", "not now");
	testFalse("4", errorSetHasErrors(e));

	errorSetPrintf(e, false, "%s", "ouch.  ouch.  ouch.");
	testTrue("5", errorSetHasErrors(e));
	errorSetFree(e);
}
