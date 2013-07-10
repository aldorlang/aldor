/*****************************************************************************
 *
 * os_macs7.c: Non-portable, operating system specific code for the Macintosh.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file is meant to be included in opsys.c.
 */

#include "assert.h0"
#include "time.h0"
#include "string.h0"


/*
 *	:			current directory
 *	:here			from current directory
 *	here			from current directory
 *	top:mid:here		full path
 *	here:sub		bad! interpretted as full path
 *	::			up one directory
 *	::nearby		up and over
 */

#define FCURDIR		 ':'
#define FDEVSEP		 0
#define FDIRSEP		 ':'
#define FDIRSEPALT	 0
#define FTYPESEP	 '.'
#define FTYPEARC	 'A'
#define FPATHSEP	 ','

int osDirSwap(String newwd, String oldwd, Length oldwdlen) { return 0; } /*!!FIX!*/

String osTmpDirName(void) { return ""; }
String osCurDirName(void) { return ""; }


#define OS_Has_PutEnv
int	osPutEnv(String s) { return 0; } /*!!FIX!*/

#define OS_Has_IsInteractive
Bool osIsInteractive (FILE *fp) { return fp == stdin; }

String osExecFileType = "";
String osObjectFileType = "";

#define OS_Has_DirIsThere

#undef true
#undef false
#include <Files.h>

Bool
osDirIsThere(String name)
{
	CInfoPBRec pb;
	char pname[256];
	StringPtr c2pstr();
	int i, len;
	
	len = strlen(name);
	assert(len < sizeof(pname));
	pname[0] = len;
	for (i=0; i<len; ++i) pname[i+1] = name[i];
	pb.hFileInfo.ioNamePtr = (StringPtr) pname;
	pb.hFileInfo.ioFVersNum = 0;
	pb.hFileInfo.ioVRefNum = 0;
	pb.hFileInfo.ioFDirIndex = 0;
	pb.hFileInfo.ioDirID = 0;
	if (PBGetCatInfo((CInfoPBPtr)&pb, false) || pb.hFileInfo.ioResult)
		return 0;
	else
		return pb.hFileInfo.ioFlAttrib & (1L << 4);
}

#define OS_Has_FixCmdLine

char *myArgv[512] = { "aldor" };
char myAxiomxlArgs[1024] = {0}; /* !!FIX! */

int
osFixCmdLine(int *pargc, char ***pargv)
{
	char *res, *p;
	int i;

	/*
	 * if under MPW Shell, "Boot" is always in environment
	 * if alone, getenv always returns NULL
	 */	
	if (! getenv("Boot")) {
		printf("aldor ");
		fflush(stdout);
		res = gets(myAxiomxlArgs);
		
		for (i=1, p=res; ; ++i) {
			while (*p == ' ') {
				*p = '\0';
				++p;
			}
			if (!*p) break;
			myArgv[i] = p;
			while (*p && *p != ' ') ++p;
		}
		*pargc = i;
		*pargv = myArgv;
	}
	return 0;
}
