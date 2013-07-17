/*****************************************************************************
 *
 * main_t.c: Low-level testing entry point.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axltop.h"

int
main(int argc, String *argv)
{
	if (argc == 1) {
#if EDIT_1_0_n1_07
		osInit();
#endif
		sxiInit();
		sxiReadEvalPrintLoop(osStdin, osStdout, SXRW_Default);
	}
	else {
#if EDIT_1_0_n1_07
		osInit();
#endif
		dbInit();
		while (--argc) testSelf(*++argv);
	}

	return EXIT_SUCCESS;
}
