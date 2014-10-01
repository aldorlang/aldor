#include "archive.h"
#include "testlib.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

local void testArchive(void);

void
archiveTestSuite()
{
	init();
	TEST(testArchive);
	fini();
}

local void
testArchive()
{
	Archive ar;
	FileName fname = fnameParse("arch-test/foo.al");
	system("mkdir arch-test");
	system("ar r arch-test/foo.al Makefile");

	ar = arFrString("arch-test/foo.al");
	testIsNotNull("t0", ar);

	system("rm -rf arch-test");
}

