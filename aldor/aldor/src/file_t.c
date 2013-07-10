/*****************************************************************************
 *
 * file_t.c: Test file system interaction.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_FILE) && !defined(TEST_ALL)

void testFile(void) { }

#else

# include "axlgen.h"

void
testFile(void)
{
	FileName	 testFile;
	FILE		 *fout;

	printf("fileSetHandler:\n");
	fileSetHandler(fileSetHandler((FileErrorFun) 0));

	printf("pathInit:\n");
	pathInit();

	printf("fileRdFind: yabba -- ");
	testFile = fileRdFind(binSearchPath(), "yabba", "");
	if (testFile) 
		printf("dir \"%s\", name \"%s\", type \"%s\"\n",
			fnameDir(testFile),
			fnameName(testFile),
			fnameType(testFile));
	else
		printf("Not found\n");

	printf("fileRdFind: cat -- ");
	testFile = fileRdFind(binSearchPath(), "cat", osExecFileType);
	if (testFile) {
		printf("dir \"%s\", name \"%s\", type \"%s\"\n",
			fnameDir(testFile),
			fnameName(testFile),
			fnameType(testFile));
		
		printf("fileRdOpen: ");
		fout = fileRdOpen(testFile);
		printf("Opened \"%s\"\n",fnameName(testFile));

		fclose(fout);
	}
	else
		printf("Not found\n");

	printf("DONE.\n");

	fnameFree(testFile);
/*	fileRemove(testFile); */

}

#endif
