#include "axlobs.h"
#include "abquick.h"
#include "format.h"
#include "testlib.h"
#include "strops.h"
#include "abcheck.h"
#include "comsg.h"

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
