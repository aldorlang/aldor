#include "fname.h"
#include "srcpos.h"
#include "testall.h"
#include "testlib.h"

local void test_srcpos(void);

void srcposTest()
{
	TEST(test_srcpos);
}

local void
test_srcpos()
{
	FileName file = fnameStdin();
	int flno = osRandom() & ((1<<16)-1);
	int glno = osRandom() & ((1<<14)-1);
	int cno = osRandom() & ((1<<14)-1);
	SrcPos pos = sposNew(file, flno, glno, cno);

	testIntEqual("sposLine", flno, sposLine(pos));
	testIntEqual("sposGlobalLine", glno, sposGlobalLine(pos));
	testIntEqual("sposChar", cno, sposChar(pos));
}

