/*****************************************************************************
 *
 * main.c: Compiler entry point.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axl.h"

int
main(int argc, String *argv)
{
	osFixCmdLine(&argc, &argv);
	return compCmd(argc, argv);
}
