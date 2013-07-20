/*****************************************************************************
 *
 * version.c: Compiler version number.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "opsys.h"
#include "version.h"
#include "comsg.h"
#include "strops.h"

/*
 * The version of this program.
 */

#if EDIT_1_0_n2_07
const char *	axiomxlName          = "Aldor";
#else
char *	axiomxlName          = "Aldor";
#endif

int	axiomxlMajorVersion  =  1;
int	axiomxlMinorVersion  =  2;
int	axiomxlMinorFreeze   =  -1;
int	axiomxlEditNumber    = 0;
 
#if EDIT_1_0_n2_07
const char   *axiomxlPatchLevel    = "";
#else
char   *axiomxlPatchLevel    = "";
#endif

#ifndef axiomxlBuildVersion
#define axiomxlBuildVersion ""
#endif

local String verGetDate(String);

Bool
verBannerWanted(void)
{
	if (strIsPrefix("internal", axiomxlBuildVersion))
		return false;
	if (strIsPrefix("release", axiomxlBuildVersion))
		return false;

	return true;
}

void
verPrint(void)
{
	if (strIsPrefix("prerelease", axiomxlBuildVersion))
		comsgFPrintf(osStdout, ALDOR_I_PreRelease, axiomxlName);

	if (strIsPrefix("demo", axiomxlBuildVersion)) {
		String date;
		date = verGetDate(axiomxlBuildVersion);
		comsgFPrintf(osStdout, ALDOR_I_DemoExpiry, 
			     axiomxlName, date);
		strFree(date);
	}
}

local String
verGetDate(String s)
{
	char *ptr;
	while (*s != ':') s++;
	s++;
	ptr = s = strCopy(s);
	while (*ptr != '\0') {
		if (*ptr == '-') *ptr=' ';
		ptr++;
	}
	return s;
}
