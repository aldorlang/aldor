#include "abcheck.h"
#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "comsg.h"
#include "format.h"
#include "strops.h"
#include "scobind.h"
#include "stab.h"
#include "testlib.h"

local void testWithDeclarations(void);
local void testReturnTypes(void);

void abcheckTest()
{
	init();
	TEST(testWithDeclarations);
	TEST(testReturnTypes);
	fini();
}

local void
testWithDeclarations()
{
	AbSyn ab1;
	CoMsg message;

	initFile();
	ab1 = abqParse("C: Category == with { (a,b,c): %}");
	abCheck(ab1);
	testIntEqual("Error count", 1, comsgErrorCount());

	message = car(comsgMessagesForMsg(ALDOR_E_ChkBadForm));
	testIntEqual("tag", abTag(message->node), AB_Declare);
	finiFile();
}

local void
testReturnTypes()
{
	AbSyn ab1;
	CoMsg message;

	initFile();
	ab1 = abqParse("foo(x: Int) == 12");
	abCheck(ab1);
	abPutUse(ab1, AB_Use_NoValue);
	scopeBind(stabFile(), ab1);
	testIntEqual("Error count", 1, comsgErrorCount());
	message = car(comsgMessagesForMsg(ALDOR_E_ChkMissingRetType));
	testIntEqual("tag", abTag(message->node), AB_Lambda);
	finiFile();
}
