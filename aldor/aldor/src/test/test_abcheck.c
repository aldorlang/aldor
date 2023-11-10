#include "abcheck.h"
#include "abquick.h"
#include "axlobs.h"
#include "comsg.h"
#include "format.h"
#include "strops.h"
#include "testlib.h"

local void testWithDeclarations(void);

void abcheckTest()
{
	init();
	TEST(testWithDeclarations);
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
