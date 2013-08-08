/*****************************************************************************
 *
 * debug.c: Debugging code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "opsys.h"

Bool	dbFlag	= false;

FILE	*dbOut;

void
dbInit(void)
{
	dbOut = osStdout;
}
