/*****************************************************************************
 *
 * fname_t.c: Test file names.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_FNAME) && !defined(TEST_ALL)

void testFname(void) { }

#else

# include "axlgen.h"

#define NFNTEMP	40	/* number of temp files to allocate, */
			/* > 36 to test base 36 conversion */

static String		ftv[] = {"a", "x", "i", "o", "m", 0};

void
testFname(void)
{
	FileName	fn;
	FileName	buf[NFNTEMP];
	FileName *	fnv;
	int		i;

	printf("fnameParse: ");
	fn = fnameParse("newfile");
	printf("\"%s\"\n",fnameName(fn));

	printf("fnameUnparseStatic: ");
	printf("\"%s\"\n",fnameUnparseStatic(fn));

	printf("fnameFree: ");
	fnameFree(fn);
	printf("DONE.\n");

	printf("\n");
	for (i = 0; i < NFNTEMP; i++) {
		buf[i] = fnameTemp("", "", "");
		printf("fnameTemp: \"%s\"\n", fnameName(buf[i]));
		fclose(fileTryOpen(buf[i], osIoWrMode));
	}

	printf("\n");
	fnv = fnameTempVector("", "", ftv);
	for (i = 0; fnv[i]; i++) {
		printf("fnameTempVector: \"%s\"\n",fnameUnparseStatic(fnv[i]));
		fnameFree(fnv[i]);
	}

	for (i = 0; i < NFNTEMP; i++) {
		fileRemove(buf[i]);
		fnameFree(buf[i]);
	}
	stoFree((Pointer) fnv);

	printf("\nThis requires 160 bytes of memory in static buffers.\n");
}

#endif
