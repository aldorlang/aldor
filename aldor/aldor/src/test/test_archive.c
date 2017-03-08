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
	int status;
	status = system("mkdir arch-test");
	testIntEqual("", 0, status);
	status = system("ar r arch-test/foo.al Makefile");
	testIntEqual("", 0, status);

	ar = arFrString("arch-test/foo.al");
	testIsNotNull("t0", ar);

	status = system("rm -rf arch-test");
	testIntEqual("", 0, status);
}

