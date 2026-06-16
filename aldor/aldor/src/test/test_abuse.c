#include "axlobs.h"
#include "abquick.h"
#include "abuse.h"
#include "format.h"
#include "testlib.h"
#include "strops.h"
#include "doc.h"

local void testSelect(void);

void
abuseTest()
{
	init();
	TEST(testSelect);
	fini();
}

local void
testSelect()
{
	StringList text;
	AbSyn code;
	AbSyn exitNode;
	AbSyn blankNode;
	AbSyn applyNode;

	initFile();

	text = listList(String)(1, "select x in {foo(?v) => 1; never}");
	code = abqParseLinesAsSeq(text);

	abPutUse(code, AB_Use_NoValue);

	exitNode = abFindNode(code, AB_Exit);
	applyNode = abFindNode(code, AB_Apply);
	blankNode = abFindNode(code, AB_Blank);
	
	testIntEqual("use1", AB_Use_Value, abUse(exitNode));
	testIntEqual("use2", AB_Use_Value, abUse(exitNode->abExit.test));
	testIntEqual("use3", AB_Use_PatLocation, abUse(applyNode));
	testIntEqual("use4", AB_Use_Pattern, abUse(blankNode));
	
	text = listList(String)(1, "select x in {foo(5) => 1; never}");
	code = abqParseLinesAsSeq(text);

	abPutUse(code, AB_Use_NoValue);

	exitNode = abFindNode(code, AB_Exit);
	testIntEqual("use5", AB_Use_Value, abUse(exitNode));
	testIntEqual("use6", AB_Use_Value, abUse(exitNode->abExit.test));
	
	finiFile();
}

