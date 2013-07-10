/*****************************************************************************
 *
 * os_os2.c: Non-portable, operating system specific code for IBM OS/2.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
/* This file is meant to be included in opsys.c.			    */
 
#include "assert.h0"
#include "time.h0"
#include "string.h0"
 
#define INCL_BASE
#include <os2.h>
#include <sys\stat.h>
 
 
/******************************************************************************
 *
 * :: osCpuTime
 *
 *****************************************************************************/
 
#define OS_Has_CpuTime
 
/*
 * One user, one process.  Count elapsed time as CPU.
 * Besides, "time" is the only function easily available.
 */
 
#include <time.h>
#include <sys\timeb.h>
 
Millisec
osCpuTime(void)
{
	struct timeb t;
	ftime(&t);
	return t.time * 1000L + (long) t.millitm;
}
 
 
/******************************************************************************
 *
 * :: osDate
 *
 *****************************************************************************/
 
#define OS_Has_Date
 
String
osDate(void)
{
	long	now;
	int	n;
	String	s;
	static char str[40];
 
	now = time(NULL);
	s   = ctime(&now);
	n   = strlen(s);
 
	assert(n < sizeof(str));
	strcpy(str, s);
	str[n-1] = '\0';  /* Stomp \n */
 
	return str;
}
 
 
/******************************************************************************
 *
 * :: osCurDirName
 * :: osTmpDirName
 *
 * :: osObjectFileType
 * :: osExecFileType
 *
 *****************************************************************************/
 
#define FCURDIR		'.'
#define FDEVSEP		':'
#define FDIRSEP		'\\'
#define FDIRSEPALT	0
#define FTYPESEP	'.'
#define FTYPEARC	'A'
 
String osObjectFileType = "OBJ";
String osExecFileType	= "EXE";
 
String osCurDirName (void) { return "."; }
 
String
osTmpDirName (void)
{
	String td;
 
	/*
 	 * Don't need to copy td, since ON THIS PLATFORM the result of
	 * osGetEnv is non-volatile.
	 */
	td = osGetEnv("TEMP");
	if (td) return td;
 
	td = osGetEnv("TMP");
	if (td) return td;
 
	td = osGetEnv("TMPDIR");
	if (td) return td;
 
	return osCurDirName();
}
 
/******************************************************************************
 *
 * :: Directory path manipulation.
 *
 *****************************************************************************/
 
#define FPATHSEP	';'
 
 
/*****************************************************************************
 *
 * :: osFileIsThere
 *
 ****************************************************************************/
 
#define OS_Has_FileIsThere
 
Bool
osFileIsThere(String name)
{
	struct stat	buf;
	int		rc;
	rc = stat(name, &buf);
	return rc == 0;
}
 
 
/*****************************************************************************
 *
 * :: osFileSize
 *
 ****************************************************************************/
 
#define OS_Has_FileSize
 
Length
osFileSize(String name)
{
	struct stat	buf;
	if (stat(name, &buf) != -1)
		return buf.st_size;
	return 0;
}
 
 
/******************************************************************************
 *
 * :: osIsInteractive
 *
 *****************************************************************************/
 
#if defined(CC_ICC)
 
#define OS_Has_IsInteractive
 
/* can probably get this with extended or migration libraries */
 
Bool
osIsInteractive(FILE *f)
{
	return false;
}
 
#endif	/* CC_ICC */
 
 
/******************************************************************************
 *
 * :: osAlloc
 * :: osFree
 *
 *****************************************************************************/
 
#define OS_Has_Alloc
 
Pointer
osAlloc(ULong *pnbytes)
{
	/* OS/2 2.0 allocates in 4K chunks, so we round up. */
 
	ULong quo = *pnbytes / 4096, rem = *pnbytes % 4096;
	PVOID pobject;
	APIRET rc;
 
	if (rem)
		*pnbytes = (quo + 1) * 4096;
 
	rc = DosAllocMem(&pobject, *pnbytes,
		PAG_READ | PAG_WRITE | PAG_COMMIT);
 
	if (rc != 0) {
		*pnbytes = 0;
		pobject	 = 0;
	}
 
	return (Pointer) pobject;
}
 
void
osFree(Pointer p)
{
	DosFreeMem((PVOID) p);
}
 
void
osAllocAlignHint(unsigned alignment)
{
}
 
void
osAllocShow(void)
{
}
 
/*****************************************************************************
 *
 * :: osSetEnv
 *
 ****************************************************************************/
 
#define OS_Has_PutEnv
 
int
osPutEnv(String eqn)
{
	return putenv(eqn);
}
 
/*****************************************************************************
 *
 * :: osDirSwap
 *
 ****************************************************************************/
 
#define OS_Has_DirSwap
 
#ifdef CC_BORLAND
#include <dir.h>
#else
#include <direct.h>
#endif
 
int
osDirSwap(String dest, String orgbuf, Length orgsiz)
{
	int rc;
 
	if (orgbuf) {
		String s;
		s = getcwd(orgbuf, orgsiz);
		if (!s) return -1;
	}
 
	rc = chdir(dest);
	if (rc == -1) return -1;
 
	return 0;
}
 
 
/*****************************************************************************
 *
 * :: osIsInteractive
 *
 ****************************************************************************/
 
#include <io.h>
 
#define OS_Has_IsInteractive
 
Bool
osIsInteractive(FILE *f)
{
	return isatty(fileno(f));
}
