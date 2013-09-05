#include "axlobs.h"
#include "abquick.h"
#include "format.h"
#include "testlib.h"
#include "strops.h"
#include "doc.h"

local void testDocco(void);

void
abnormTest()
{
	init();
	TEST(testDocco);
	fini();
}

local void
testDocco()
{
	StringList text;
	AbSyn code;
	initFile();

	text = listList(String)(3, "+++ Words", "a: B;", "  ++ more words");
	code = abqParseLinesAsSeq(text);
	testAIntEqual("an id", AB_Declare, abTag(code));
	testStringEqual("docs", " Words\n more words\n", docString(abComment(abDefineeId(code))));

	text = listList(String)(2, "a: B;", "  ++ some words");
	code = abqParseLinesAsSeq(text);
	testAIntEqual("an id", AB_Declare, abTag(code));
	testStringEqual("docs", " some words\n", docString(abComment(abDefineeId(code))));

	text = listList(String)(2, "+++ ZZZ", "a: B;");
	code = abqParseLinesAsSeq(text);
	testAIntEqual("an id", AB_Declare, abTag(code));
	testStringEqual("docs", " ZZZ\n", docString(abComment(abDefineeId(code))));

	finiFile();
}

