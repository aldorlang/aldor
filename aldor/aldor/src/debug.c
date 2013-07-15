/*****************************************************************************
 *
 * debug.c: Debugging code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axlgen0.h"

Bool    dbFlag  = false;

FILE	*dbIn, *dbOut;

void
dbInit(void)
{
	dbIn  = osStdin;
	dbOut = osStdout;
}
