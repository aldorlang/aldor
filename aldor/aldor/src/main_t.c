/*****************************************************************************
 *
 * main_t.c: Low-level testing entry point.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axltop.h"
#include "debug.h"
#include "opsys.h"
#include "test.h"

int
main(int argc, String *argv)
{
	if (argc == 1) {
		osInit();
		sxiInit();
		sxiReadEvalPrintLoop(osStdin, osStdout, SXRW_Default);
	}
	else {
		osInit();
		dbInit();
		while (--argc) testSelf(*++argv);
		dbFini();
	}

	return EXIT_SUCCESS;
}
