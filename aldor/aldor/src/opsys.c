/*****************************************************************************
 *
 * opsys.c: Non-portable, operating system specific code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/


#define _ALL_SOURCE   /* For RS/6000 - should come before cport.h include. */

#include "cport.h"
#include "editlevels.h"
#include "opsys.h"

/*
 * Select OS-specific file.  Note, olde compilers complain about #elif.
 */
#if defined(OS_UNIX)
#include "os_unix.c"
#endif

#if defined(OS_IBM_OS2)
#include "os_os2.c"
#endif

#if defined(OS_MS_DOS)
#include "os_dos.c"
#endif

#if defined(OS_WIN32)
#include "os_win32.c"
#endif

#if defined(OS_CMS)
#include "os_cms.c"
#endif

#if defined(OS_VMS)
#include "os_vms.c"
#endif

#if defined(OS_MAC_SYS7)
#include "os_macs7.c"
#endif

/*****************************************************************************
 *
 * :: osInit
 *
 ****************************************************************************/

#if !defined(OS_Has_Init)
extern void osSetStreams(FILE * sstdin, FILE * sstdout, FILE * sstderr);

void
osInit()
{
	osSetStreams(stdin, stdout, stderr);
	return;
}
#endif /* OS_Has_Init */
/*****************************************************************************
 *
 * :: osExit
 *
 ****************************************************************************/
#if !defined(OS_Has_Exit)
void
osExit(int n)
{
	exit(n);
}

#endif /* OS_Has_Exit */

/*****************************************************************************
 *
 * :: osDisplayMessage
 *
 ****************************************************************************/
#if !defined(OS_Has_DisplayMessage)
void
osDisplayMessage(char * msg)
{
	printf("%s", msg);
}
#endif /* OS_Has_DisplayMessage */

/*****************************************************************************
 *
 * :: osIsGUI
 *
 ****************************************************************************/

Bool
osIsGUI()
{
#if defined(OS_MAC_SYS7)
	return true;
#else
	return false;
#endif
}

/*****************************************************************************
 *
 * :: osObtainLicense
 *
 ****************************************************************************/

#if defined(OS_Has_License)
Bool
osObtainLicense(void)
{
	struct tm *x;
	time_t t;
	
	t = time(NULL);
	x = gmtime(&t);

	if (x->tm_year != BOMB) {
printf("License manager: Your license to use this software has expired.\n");
printf("Contact infodesk@nag.co.uk for details on purchasing this product.\n");
		/* run away without tidying up! */
		exit(EXIT_SUCCESS);
	}	
}

#else

Bool
osObtainLicense(void)
{
	return true;
}

#endif /* ! OS_Has_License */


/*****************************************************************************
 *
 * :: osRun
 * :: osRunConcurrent
 * :: osRunQuoteArg
 *
 ****************************************************************************/

#if !defined(OS_Has_Run)

int
osRun(String cmd)
{
	return system(cmd);
}

#endif /* ! OS_Has_Run */

#if !defined(OS_Has_RunConcurrent)

int
osRunConcurrent(String cmd, FILE **pstdin, FILE **pstdout, FILE **pstderr)
{
	return -1;
}

#endif /* ! OS_Has_RunConcurrent */

#if !defined(OS_Has_RunQuoteArg)

/*
 *!! Really want os-specific versions.
 *!! Can't handle strings with both ' and ".
 */
int
osRunQuoteArg(String word, int (*putter)(int))
{
	String	s;
	Bool	hasSQ	 = false;
	Bool	hasFunky = false;
	int	cc = 0;
	char	c0	 = word[0];

	if (c0 != '\'' && c0 != '"' && c0 != '$') {
		for (s = word; *s; s++) {
			if (isalnum(*s))
				continue;
			switch (*s) {
			    case '+': case '-': case '=': case '_':
			    case '%': case ':': case '/': case '\\':
			    case '.': case ',':
				continue;
			}
			if (*s == '\'') hasSQ = true;
			hasFunky = true;
		}
	}

	cc = 0;
	if (!hasFunky) {
		for (s = word; *s; s++) { putter(*s); cc++; }
	}
	else if (!hasSQ) {
		putter('\''); cc++;
		for (s = word; *s; s++) { putter(*s); cc++; }
		putter('\''); cc++;
	}
	else {
		putter('\"'); cc++;
		for (s = word; *s; s++) { putter(*s); cc++; }
		putter('\"'); cc++;
	}
	return cc;
}

#endif /* ! OS_Has_RunQuoteArg */

/*****************************************************************************
 *
 * :: osCpuTime
 * :: osDate
 *
 ****************************************************************************/

#if !defined(OS_Has_CpuTime)

Millisec
osCpuTime(void)
{
	return 0;
}

#endif /* ! OS_CpuTime */


#if !defined(OS_Has_Date)

String
osDate(void)
{
	return 0;
}

#endif	/* OS_Has_Date */


/*****************************************************************************
 *
 * :: osGetEnv
 * :: osPutEnv
 * :: osPutEnvIsKept
 *
 ****************************************************************************/

/*
 * OS_specific file should define dummy versions if not available.
 * Note that ANSI C requires getenv but not putenv.
 */

#if !defined(OS_Has_GetEnv)

String
osGetEnv(String envar)
{
	return (String) getenv(envar);
}

#endif	/* OS_Has_GetEnv */

#if !defined(OS_Has_PutEnv)

extern int	putenv(/* String */);

int
osPutEnv(String eqn)
{
	return putenv(eqn);
}

#endif /* OS_Has_PutEnv */

#if !defined(OS_Has_PutEnvIsKept)

Bool
osPutEnvIsKept(void)
{
	return true;
}

#endif /* OS_Has_PutEnvIsKept */

/*****************************************************************************
 *
 * :: Fix command line for standalone ports.
 *
 ****************************************************************************/

#if !defined(OS_Has_FixCmdLine)

int
osFixCmdLine(int *argc, char ***argv)
{
	return 0;
}

#endif /* OS_Has_FixCmdLine */


/*****************************************************************************
 *
 * :: osCurDirName
 * :: osTmpDirName
 *
 * :: osObjectFileType
 * :: osExecFileType
 *
 ****************************************************************************/

/*
 * No defaults.
 */


/****************************************************************************
 *
 * :: osFnameNParts
 * :: osFnameParse
 * :: osFnameParseSize
 * :: osFnameUnparse
 * :: osFnameUnparseSize
 * :: osFnameDirEqual
 *
 ****************************************************************************/

/*
 * Default file name syntax is [<dir>] <name> [FTYPESEP <type>]
 *
 * where <dir> is	       [<dev> FDEVSEP] [<subdir> FDIRSEP|FDIRSEPALT]*
 *
 * and FDEVSEP, FDIRSEP, FDIRSEPALT, FTYPESEP are defined in the os_*.c files.
 *
 * FDIRSEPALT was introduced to allow names in #includes of aldor source to 
 * allow the same format: namely '/' on all platforms that have different 
 * directory separator.
 */

#if !defined(OS_Has_FnameIsAbsolute)
local Bool
osFnameIsAbsolute(String fname)
{
	return (FDIRSEP && fname[0] == FDIRSEP)
#if EDIT_1_0_n2_08
	||     (FDIRSEPALT && fname[0] == FDIRSEPALT)
#endif
	||     (FDEVSEP && fname[1] == FDEVSEP);
}

#endif /* !OS_Has_FnameIsAbsolute */


#if !defined(OS_Has_FnameParse)

Length	osFnameNParts = 3;

void
osFnameParse(String *partv, String sbuf, String fname, String relativeTo)
{
	String	s, b, name = 0, type = 0;

	/* Find parts of fname. */
	for (s = fname + strlen(fname)-1; s >= fname; s--) {
#if EDIT_1_0_n2_08 
		if (!name && (*s == FDEVSEP || *s == FDIRSEP || *s == FDIRSEPALT)) {
#else
		if (!name && (*s == FDEVSEP || *s == FDIRSEP)) {
#endif
			name = s+1;
			break;
		}
		if (!type && *s == FTYPESEP) {
			type = s+1;
		}
	}
	if (!name)
		name = fname;
	b = sbuf;

	/* Directory part. */
	*partv++ = b;
	if ( relativeTo && *relativeTo && !osFnameIsAbsolute(fname)) {
		for (s = relativeTo; *s; )
			*b++ = *s++;
#if EDIT_1_0_n2_08
		if (b[-1] != FDEVSEP && (b[-1] != FDIRSEP || b[-1] != FDIRSEPALT))
#else
		if (b[-1] != FDEVSEP && b[-1] != FDIRSEP)
#endif
			*b++ = FDIRSEP;
	}
#if EDIT_1_0_n2_08
	for (s = fname; s != name; b++, s++) {
		*b = *s;
		if (*b == FDIRSEPALT) *b = FDIRSEP;
	}
#else
	for (s = fname; s != name; )
		*b++ = *s++;
#endif
	if (b > sbuf+1)	    /* Zap last dir separator. (but not if it's the first!) */
		b--;
	*b++ = 0;

	/* Name part. */
	*partv++ = b;
	while (*s && s != type)
		*b++ = *s++;
	if (type)	    /* Zap type separator. */
		b--;
	*b++ = 0;

	/* Type part. */
	*partv++ = b;
	if (type)
		while (*s)
			*b++ = *s++;
	*b++ = 0;
}

Length
osFnameParseSize(String fname, String relativeTo)
{
	Length	sz = 0;

	sz += fname      ? strlen(fname)        : 0;
	sz += relativeTo ? strlen(relativeTo)+1 : 0;	/* +1 for FDIRSEP    */
	sz += osFnameNParts;				/* one '\0' per part */

	return sz;
}

String
osFnameUnparse(String buf, String *partv, Bool full)
{
	String	s, d;
	String	dir = partv[0], name = partv[1], type = partv[2];

	d = buf;

	/* Only give the directory if it is not the current one. */
	if ((dir && *dir) && (full || strcmp(dir, osCurDirName()))) {
		for(s = dir; *s; ) *d++ = *s++;
#if EDIT_1_0_n2_08
		if (d[-1] != FDEVSEP && (d[-1] != FDIRSEP || d[-1] != FDIRSEPALT)) *d++ = FDIRSEP;
#else
		if (d[-1] != FDEVSEP && d[-1] != FDIRSEP) *d++ = FDIRSEP;
#endif
	}

	for(s = name; *s; )	   *d++ = *s++;

	if (type && *type) {
		*d++ = FTYPESEP;
		for(s = type; *s;) *d++ = *s++;
	}
	*d++ = 0;

	return buf;
}

Length
osFnameUnparseSize(String *partv, Bool full)
{
	String	dir = partv[0], name = partv[1], type = partv[2];
	Length dirsz, namesz, typesz;

	dirsz  = ((dir && *dir) && (full||strcmp(dir, osCurDirName())))?strlen(dir)+1:0;
	namesz = strlen(name);
	typesz = (type && *type) ? strlen(type) + 1 : 0;

	return (dirsz + namesz + typesz) + 1;
}

Length
osSubdirLength(String relativeTo, String subdir)
{
	return strlen(relativeTo) + strlen(subdir) + 2;
}

void
osSubdir(String buffer, String relativeTo, String subdir)
{
	String	s, d;

	/* If subdir is absolute, just use it. */
	if (osFnameIsAbsolute(subdir)) { strcpy(buffer, subdir); return; }
		
	d = buffer;

	/* Only give the directory if it is not the current one. */
	if (*relativeTo && strcmp(relativeTo, osCurDirName())) {
		for(s = relativeTo; *s; ) *d++ = *s++;
#if EDIT_1_0_n2_08
		if (d[-1] != FDEVSEP && (d[-1] != FDIRSEP || d[-1] != FDIRSEPALT)) *d++ = FDIRSEP;
#else
		if (d[-1] != FDEVSEP && d[-1] != FDIRSEP) *d++ = FDIRSEP;
#endif
	}

	/* Only use subdir if it is not current. */
	if (strcmp(subdir, osCurDirName())) for (s = subdir; *s; ) *d++ = *s++;

	/* Use current if we didn't put in anything. */
	if (d == buffer) for (s = osCurDirName(); *s; ) *d++ = *s++;

	/* Back up on trailing "/" (but not if buffer contains only "/"). */
#if EDIT_1_0_n2_08
	if (d != buffer && d != buffer + 1 && (d[-1] == FDIRSEP || d[-1] == FDIRSEPALT)) d--;
#else
	if (d != buffer && d != buffer + 1 && d[-1] == FDIRSEP) d--;
#endif
	d[0] = 0;
}

/*
 *!! Perhaps should handle x/../y == y
 */
Bool
osFnameDirEqual(String dir1, String dir2)
{
	/* Strip off explicit leading current directories. */

	while (*dir1 == FCURDIR || *dir2 == FCURDIR) {
		if (*dir1 == FCURDIR) {
			dir1++;
#if EDIT_1_0_n2_08
			if (*dir1 == FDIRSEP || (FDIRSEPALT && *dir1 == FDIRSEPALT)) dir1++;
#else
			if (*dir1 == FDIRSEP) dir1++;
#endif
		}
		if (*dir2 == FCURDIR) {
			dir2++;
#if EDIT_1_0_n2_08
			if (*dir2 == FDIRSEP || (FDIRSEPALT && *dir2 == FDIRSEPALT)) dir2++;
#else
			if (*dir2 == FDIRSEP) dir2++;
#endif
		}
	}
	return !strcmp(dir1, dir2);
}

#endif /* !OS_Has_FnameParse */

/*****************************************************************************
 *
 * :: osFnameTempSeed
 * :: osFnameTempDir
 *
 ****************************************************************************/

#if !defined(OS_Has_FnameTempSeed)

int
osFnameTempSeed(void)
{
	static int seed = -1;
	if (seed == -1) seed = (int) osCpuTime();
	return seed;
}

#endif /* !OS_Has_FnameSeed */


#if !defined(OS_Has_FnameTempDir)

String
osFnameTempDir(String relativeTo)
{
	return relativeTo;
}

#endif /* !OS_Has_FnameTempDir */


/*****************************************************************************
 *
 * :: osIncludePath
 * :: osLibraryPath
 * :: osExecutePath
 * :: osPathLength
 * :: osPathParse
 *
 ****************************************************************************/

/*
 * No default for osPathSep
 */
#if !defined(OS_Has_DefaultPaths)

String	osDefaultExecutePath = "";
String	osDefaultLibraryPath = "";
String	osDefaultIncludePath = "";

#endif /* !OS_Has_DefaultPaths */


String
osIncludePath(void)
{
	String path = osGetEnv("INCPATH");
	return (path == 0 ? osDefaultIncludePath : path);
}

String
osLibraryPath(void)
{
	String path = osGetEnv("LIBPATH");
	return (path == 0 ? osDefaultLibraryPath : path);
}

String
osExecutePath(void)
{
	String path = osGetEnv("PATH");
	return (path == 0 ? osDefaultExecutePath : path);
}

char
osPathSeparator(void)
{
	return FPATHSEP;
}

Length
osPathLength(String path)
{
	Length	n;

	n = path[0] ? 1 : 0;

	/*
	 * Add count of separators which have something after them.
	 */
	for ( ; path[0] && path[1]; path++) if (*path == FPATHSEP) n++;

	return n;
}

void
osPathParse(String* partv, String sbuf, String path)
{
	Length		n = 0;

	strcpy(sbuf, path);
	while (*sbuf) {
		partv[n++] = sbuf;
		while (*sbuf && *sbuf != FPATHSEP)
			sbuf += 1;
		if (*sbuf) *(sbuf++) = 0;
	}
}

/*****************************************************************************
 *
 * :: osIoRdMode	read   text
 * :: osIoWrMode	write  text
 * :: osIoApMode	append text
 * :: osIoRbMode	read   binary
 * :: osIoWbMode	write  binary
 * :: osIoAbMode	append binary
 * :: osIoRubMode	read   binary for update
 * :: osIoWubMode	write  binary for update
 * :: osIoAubMode	append binary for update
 *
 ****************************************************************************/

#if !defined(OS_Has_IoModes)

IOMode	osIoRdMode  = "r";
IOMode	osIoWrMode  = "w";
IOMode	osIoApMode  = "a";
IOMode	osIoRbMode  = "rb";
IOMode	osIoWbMode  = "wb";
IOMode	osIoAbMode  = "ab";
IOMode	osIoRubMode = "r+b";
IOMode	osIoWubMode = "w+b";
IOMode	osIoAubMode = "a+b";

#endif /* ! OS_Has_IoModes */


/*****************************************************************************
 *
 * :: osStreams
 *
 ****************************************************************************/

#if (!defined(OS_Has_SpecialStreams))

FILE * osStdout;
FILE * osStdin;
FILE * osStderr; 

void
osSetStreams(FILE * sstdin, FILE * sstdout, FILE * sstderr)
{
	osStdout = stdout;
	osStdin  = stdin;
	osStderr = stderr;
}

int  
osGetc(FILE * f)
{
	return getc(f);
}

int
osPutc(FILE *f, int c)
{
	return putc(c, f);
}

int
osFPrintf(FILE *f, char *format, ...)
{
	int ret;
	va_list argp;
	va_start(argp, format);
	ret = vfprintf(f, format, argp);
	va_end(argp);

	return ret;
}

int
osFEof(FILE * f)
{
	return feof(f);
}

#endif /* !OS_Has_SpecialStreams */

/*****************************************************************************
 *
 * :: osIsInteractive
 * :: osFileRemove
 * :: osFileIsThere
 * :: osFileSize
 * :: osFileHash
 * :: osDirIsThere
 * :: osDirSwap
 *
 ****************************************************************************/

/*
 * No default for osDirSwap
 */

#if !defined(OS_Has_IsInteractive)

extern	int	isatty	(int);

Bool
osIsInteractive(FILE *f)
{
	return isatty(fileno(f));
}

#endif /* ! OS_Has_IsInteractive */


#if !defined(OS_Has_FileRemove)

/* Assume ANSI compliant. */

int
osFileRemove(String name)
{
	return remove(name);
}

#endif	/* ! OS_Has_FileRemove */


#if !defined(OS_Has_FileRename)

/* Assume ANSI compliant. */

int
osFileRename(String from, String to)
{
	return rename(from, to);
}

#endif	/* ! OS_Has_FileRename */


#if !defined(OS_Has_FileIsThere)

Bool
osFileIsThere(String name)
{
	FILE	*file;

	file = fopen(name, osIoRdMode);

	if (file) fclose(file);

	return file != 0;
}

#endif /* ! OS_Has_FileIsThere */


#if !defined(OS_Has_FileSize)

#ifndef SEEK_END
#  define SEEK_END	2
#endif

Length
osFileSize(String name)
{
	FILE	*file;
	int	rc;
	long	pos;

	file = fopen(name, osIoRbMode);
	if (!file) return 0;

	rc = fseek(file, long0, SEEK_END);
	if (rc) return 0;

	pos = ftell(file);
	fclose(file);

	return (Length) pos;
}

#endif /* ! OS_Has_FileSize */


#if !defined(OS_Has_FileHash)

/*!! Should avoid ..\src\foo.c != foo.c */

Hash
osFileHash(String name)
{
	int	n, i;
	long	ltor, rtol;

	n = strlen(name);

	ltor = rtol = 0;
	for (i = 0; i < n; i++) {
		ltor *= 17; ltor += name[i];
		rtol *= 17; rtol += name[n-i-1];
	}
	return (Hash) (ltor + rtol);
}

#endif /* ! OS_Has_FileHash */


#if !defined(OS_Has_DirIsThere)

Bool
osDirIsThere(String name)
{
	return osFileIsThere(name);
}

#endif /* ! OS_Has_DirIsThere */


/*****************************************************************************
 *
 * :: osSetBreakHandler
 * :: osSetFaultHandler
 * :: osSetLimitHandler
 *
 ****************************************************************************/

static OsSignalHandler	breakHandler = 0, faultHandler = 0, limitHandler = 0,
                        dangerHandler = 0;

int osBreakSignals[] = {
	SIGHUP, SIGBREAK, SIGINT, SIGQUIT, SIGTERM, -1
};

int osFaultSignals[] = {
	SIGILL, SIGTRAP, SIGABRT, SIGEMT, SIGFPE,
	SIGBUS, SIGSEGV, SIGSYS,  SIGPIPE, -1
};

int osLimitSignals[] = {
	SIGXCPU, SIGXFSZ, -1
};

int osDangerSignals[] = {
	SIGDANGER, -1
};

#if defined(OS_WIN32)
OsSignalHandler
osSetSignalHandlers(OsSignalHandler *posigfn,int *sigv,OsSignalHandler nsigfn)
{
	OsSignalHandler osigfn;

	osigfn	 = *posigfn;
	*posigfn = nsigfn;
	if (!nsigfn) nsigfn = (OsSignalHandler) SIG_DFL;

	for (; *sigv != -1; sigv++)
		if (*sigv != SIGFAKE) signal(*sigv, nsigfn);

	return osigfn;
}
#else
/* sigaction preferred */
OsSignalHandler
osSetSignalHandlers(OsSignalHandler *posigfn,int *sigv,OsSignalHandler nsigfn)
{
	struct sigaction oldaction = { };
	struct sigaction newaction = { };

	newaction.sa_handler = nsigfn ? nsigfn : SIG_DFL;

	for (; *sigv != -1; sigv++)
		if (*sigv != SIGFAKE) sigaction(*sigv,&newaction,&oldaction);
	
	return oldaction.sa_handler;
}
#endif
	


OsSignalHandler
osSetBreakHandler(OsSignalHandler sigfn)
{
	return osSetSignalHandlers(&breakHandler, osBreakSignals, sigfn);
}

OsSignalHandler
osSetFaultHandler(OsSignalHandler sigfn)
{
	return osSetSignalHandlers(&faultHandler, osFaultSignals, sigfn);
}

OsSignalHandler
osSetLimitHandler(OsSignalHandler sigfn)
{
	return osSetSignalHandlers(&limitHandler, osLimitSignals, sigfn);
}

OsSignalHandler
osSetDangerHandler(OsSignalHandler sigfn)
{
	return osSetSignalHandlers(&dangerHandler, osDangerSignals, sigfn);
}


/*****************************************************************************
 *
 * :: osAlloc
 * :: osFree
 * :: osAllocAlignHint
 * :: osAllocShow
 *
 ****************************************************************************/

#if !defined(OS_Has_Alloc)

Pointer
osAlloc(ULong *pnbytes)
{
	return malloc(*pnbytes);
}

void
osFree(Pointer p)
{
	free(p);
}

void
osAllocAlignHint(unsigned alignment)
{
}

void
osAllocShow(void)
{
}

#endif	/* ! OS_Has_Alloc */


/*****************************************************************************
 *
 * :: osMemMap
 *
 ****************************************************************************/

#if !defined(OS_Has_MemMap)

struct osMemMap **
osMemMap(int mask)
{
	return 0;
}

#endif /* ! OS_Has_MemMap */


/*****************************************************************************
 *
 * :: osRandom
 *
 ****************************************************************************/

#if (!defined(OS_Has_Random))

Length
osRandom(void)
{
      return rand();
}

#endif /* !OS_Has_Random */
