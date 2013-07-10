/****************************************************************************
 *
 * os_cms.c: Non-portable, operating system specific code for CMS.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file is meant to be included in opsys.c.
 */

#include <time.h>
#include <ctype.h>

/*****************************************************************************
 *
 * :: osRunQuoteArg
 *
 ****************************************************************************/

#define	OS_Has_RunQuoteArg

/*
 *!! Can't handle strings with ".
 */
int
osRunQuoteArg(String word, int (*putter)(int))
{
	String	s;
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
			hasFunky = true;
		}
	}

	cc = 0;
	if (!hasFunky) {
		for (s = word; *s; s++) { putter(*s); cc++; }
	}
	else {
		putter('\"'); cc++;
		for (s = word; *s; s++) { putter(*s); cc++; }
		putter('\"'); cc++;
	}
	return cc;
}

/*****************************************************************************
 *
 * :: osCpuTime
 *
 ****************************************************************************/

#define OS_Has_CpuTime

/* This is actually an ANSI definition. */

Millisec
osCpuTime(void)
{
	clock_t t = clock();
	return (t * 1000)/CLK_TCK;
}


/*****************************************************************************
 *
 * :: osPutEnv
 * :: osPutEnvIsKept
 *
 ****************************************************************************/

#define OS_Has_PutEnv

int
osPutEnv(String eqn)
{
	char	*lhs, *rhs;
	int	rc, n;
	char 	cmd[2048];
	char	*cmdfmt = "GLOBALV SELECT CENV SETLP %s %s CENV";

	lhs = eqn;
	for (rhs = eqn; *rhs; rhs++)
		if (*rhs = '=') { *rhs = 0; rhs++; break; }

	n = strlen(lhs) + strlen(rhs) + strlen(cmdfmt);
	if (!*rhs || n + 1 >= sizeof(cmd)) return -1;
	
	sprintf(cmd, cmdfmt, lhs, rhs);
	rc = system(cmd);

	return (rc == 0) ? 0 : -1;
}

#define OS_Has_PutEnvIsKept

Bool
osPutEnvIsKept(void)
{
	return false;
}

/*****************************************************************************
 *
 * :: osCurDirName
 * :: osTmpDirName

 * :: osObjectFileType
 * :: osExecFileType
 *
 ****************************************************************************/

#define FCURDIR		 'A'
#define FDEVSEP		 ' '
#define FDIRSEP		 ' '
#define FDIRSEPALT	 0
#define FTYPESEP	 ' '
#define FTYPEARC	 'A'

String osObjectFileType = "TEXT";
String osExecFileType	= "MODULE";

String osCurDirName (void) { return "A"; }
String osTmpDirName (void) { return "A"; }

/*****************************************************************************
 *
 * :: osFnameNParts
 * :: osFnameParse
 * :: osFnameUnparse
 * :: osFnameParseSize
 * :: osFnameUnparseSize
 * :: osFnameDirEqual
 *
 ****************************************************************************/

#define OS_Has_FnameParse

#define FNAME_SIZE	8

/*
 *  We accept file names of the forms:	"fn.ft"	 "fn ft fm"  "fn ft"  "fn"
 *
 *  The first form allows the source to contain portable "#include" lines
 *  The other forms give the standard CMS representation.
 */

local Bool
fnameIsCmsStyle(String fname)
{
	for ( ; *fname; fname++) if (isspace(*fname)) return true;
	return false;
}

Length osFnameNParts = 3;

void
osFnameParse(String *partv, String sbuf, String fname, String relativeTo)
{
	String	s;
	int	i;

	if (!fnameIsCmsStyle(fname)) {
		/* fn  or  fn.ft */

		strcpy(sbuf, fname);
		partv[0]  = relativeTo ? relativeTo : "";
		partv[1]  = sbuf;
		partv[2]  = "";
		for (s = sbuf + strlen(sbuf)-1; s >= sbuf; s--)
			if (*s == '.') { *s = 0; partv[2] = s+1; break; }
		if (strlen(partv[1]) > FNAME_SIZE) partv[1][FNAME_SIZE] = 0;
		if (strlen(partv[2]) > FNAME_SIZE) partv[2][FNAME_SIZE] = 0;
	}
	else {
		/* fn ft  or  fn ft fm */
		s = strcpy(sbuf, fname);

		partv[1] = s;
		for (i = 0; *s && !isspace(*s) && i < FNAME_SIZE; i++) s++;
		if (!*s) { partv[0] = partv[2] = ""; return; }
		while (*s && !isspace(*s)) s++;
		while (isspace(*s)) *s++ = 0;

		partv[2] = s;
		for (i = 0; *s && !isspace(*s) && i < FNAME_SIZE; i++) s++;
		if (!*s) { partv[0] = ""; return; }
		while (*s && !isspace(*s)) s++;
		while (isspace(*s)) *s++ = 0;

		partv[0] = s;
		while (*s && !isspace(*s)) s++;
	}
	for (i = 0; i < 3; i++) for (s = partv[i]; *s; *s++) *s = tolower(*s);
}

Length
osFnameParseSize(String fname, String relativeTo)
{
	Length sz = 0;
	sz += strlen(fname);
	sz += relativeTo ? strlenRelativeTo : 0;
	sz += osFnameNParts;
	return sz;
}

String
osFnameUnparse(String buf, String* partv, Bool full)
{
	char	*s, *b;
	String	dir = partv[0], name = partv[1], type = partv[2];

	b = buf;

	for (s = name; *s; *s++) *b++ = toupper(*s);
	*b++ = ' ';
	for (s = type; *s; *s++) *b++ = toupper(*s);
	if (dir && *dir) {
		*b++ = ' ';
		for (s = dir; *s; *s++) *b++ = toupper(*s);
	}
	*b++ = 0;

	return buf;
}

Length
osFnameUnparseSize(String *partv, Bool full)
{
	String	dir = partv[0], name = partv[1], type = partv[2];

	return strlen(dir) + strlen(name) + strlen(type) + 2;
}

Length
osSubdirLength(String relativeTo, String subdir)
{
	return strlen(relativeTo);
}

void
osSubdir(String buf, String relativeTo, String subdir)
{
	strcpy(buf, relativeTo);
}

Bool
osFnameDirEqual(String dir1, String dir2)
{
	/* Strip of explicit leading current directory. */
	if (*dir1 == FCURDIR) dir1++;
	if (*dir2 == FCURDIR) dir2++;

	return !strcmp(dir1, dir2);
}

/*****************************************************************************
 *
 * :: osDirSwap
 *
 ****************************************************************************/

#define OS_Has_DirSwap

int
osDirSwap(String dest, String orgbuf, Length orgsiz)
{
	return -1;
}

/*****************************************************************************
 *
 * :: Directory path manipulation.
 *
 ****************************************************************************/

#define OS_Has_DefaultPaths

#define FPATHSEP	 ' '

#define ALL_DISK_LETTERS "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"

String	 osDefaultExecutePath = ALL_DISK_LETTERS;
String	 osDefaultLibraryPath = ALL_DISK_LETTERS;
String	 osDefaultIncludePath = ALL_DISK_LETTERS;


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

#define OS_Has_IoModes

IOMode	osIoRdMode  = "r";
IOMode	osIoWrMode  = "w";
IOMode	osIoApMode  = "a";
IOMode	osIoRbMode  = "rb,recfm=V,lrecl=16384";
IOMode	osIoWbMode  = "wb,recfm=V,lrecl=16384";
IOMode	osIoAbMode  = "ab,recfm=V,lrecl=16384";
IOMode	osIoRubMode = "r+b,recfm=V,lrecl=16384";
IOMode	osIoWubMode = "w+b,recfm=V,lrecl=16384";
IOMode	osIoAubMode = "a+b,recfm=V,lrecl=16384";

/*****************************************************************************
 *
 * :: osIsInteractive
 *
 ****************************************************************************/

#if defined(CC_C370)

#define OS_Has_IsInteractive

#define CMS_FNAME_MAX	100

Bool
osIsInteractive(FILE *f)
{
	char	 fname[CMS_FNAME_MAX];
	fldata_t fdata;
	int	 rc;

	rc = fldata(f, fname, &fdata);

	return !rc && fdata.__device == __TERMINAL;
}

#endif /* CC_C370 */


/*****************************************************************************
 *
 * :: osMemMap
 *
 ****************************************************************************/

#if defined(CC_C370)

#define OS_Has_MemMap

/*
 * On CMS, the stack and data segments may be discontiguous.
 * Initial data is placed in a malloc-ed area if the program is re-entrant.
 * Otherwise it is in scattered sections and we cannot find it.
 *
 * This code applies to library version 2 release 1 or earlier and may need to
 * be changed for later versions.
 *
 * Layout information is taken from
 *
 * "IBM C/370 Diagnostics Guide and Reference Version 2 Release 1.0"
 * LY09-1804-00.
 *
 * However, that document incorrectly describes the "Task Information Appendage"
 * (TIA) and the "Heap Storage Segment" structures.
 * Thanks go to Bob Duncan of the IBM Toronto Lab for help determining the
 * correct layout.
 */

/* Version number for which these structures are valid. */
# define  KNOWN_C370_LIB_VERSION 0x02010000

/* Register 13 points to most recent DSA (call stack frame.) */
struct DynStoreArea {
	int			rsrv1[1];
	struct DynStoreArea *	prev;
	int			rsrv2[17];
	Pointer			frontier;
};

/* Register 12 points to TCA. */
struct TaskCommArea {
	int			rsrv1[2];
	struct SecStackSeg *	stackHOC;	/* Newest.  Head-of-chain. */
	Pointer			stackEnd;	/* Past end of first or 0. */
	int			rsrv2[6];
	struct TaskInfoArea *	TIA;
	int			rsrv3[68];
	int			isaLength;
	int			rsrv4[45];
	Pointer			statics;	/* Writable data (RENT). */
};

struct TaskInfoArea {
	Pointer			isaEnd;		/* TISA	 */
	int			rsrv1[23];
	Pointer			realEOS;	/* TXRES */
	int			isaIncrSz;
	int			heapInitSz;
	int			heapIncrSz;
	struct HeapSeg *	heapInit;
	struct HeapSeg *	heapHOC;	/* Head-of-chain */
};

struct SecStackSeg {
	struct SecStackSeg *	prev;
	Pointer			end;
};

struct HeapSeg {
	struct HeapSeg		*prev;
	int			length;
};

/*
 * Helper functions and macros.
 */

# define StackPtr(x)	   ((Pointer) (((unsigned long)(x)) & 0x00FFFFFF))
# define StackSeg(x)	   ((struct SecStackSeg	 *) StackPtr(x))

# define getDynStoreArea() ((struct DynStoreArea *) getRegister(13)) /* bound */
# define getTaskCommArea() ((struct TaskCommArea *) getRegister(12))

# include <setjmp.h>

ULong
getRegister(int n)
{
	jmp_buf	    jb;
	setjmp(jb);
	return (unsigned int) jb[n+1];
}

/*
 * Memory map vector -- grown or reused on each call.
 */
static struct osMemMap	*mmv = 0;
static int		mmc  = 0;	/* Number of slots allocated in mmv. */

/*
 * There may be any number of stack and heap segments so this function
 * first counts them and then allocates a memory map vector to contain the info.
 */
struct osMemMap *
osMemMap(int mask)
{
	struct TaskCommArea	* TCA;
	struct TaskInfoArea	* TIA;
	struct HeapSeg		* heap;
	struct SecStackSeg	* stack;
	int			n, nh, ns;

	/*
	 * Verify that this is a version we can handle.
	 * Can only find initial data if re-entrant.
	 */
	if (__librel() > KNOWN_C370_LIB_VERSION) return 0;

	TCA = getTaskCommArea();
	TIA = TCA->TIA;

	if ((mask & OSMEM_IDATA) && !TCA->statics) return 0;

	/*
	 * Count segments of interest.
	 */
	nh = ns = 0;
	if (mask & (OSMEM_IDATA | OSMEM_DDATA)) {
		heap = TIA->heapHOC;
		for (; heap; heap = heap->prev) nh++;
	}
	if (mask & OSMEM_STACK) {
		stack= StackSeg(TCA->stackHOC);
		for (; stack; stack= StackSeg(stack->prev)) ns++;
	}

	/*
	 * Allocate array to hold memory map.
	 */
	n = nh + ns + 2;	/* 1 for OSMEM_END, and 1 possibly for mmv. */
	if (n > mmc) {
		ULong nbytes = n * sizeof(struct osMemMap);
		if (mmc) free(mmv);
		mmc = n;
		mmv = (struct osMemMap *) osAlloc(&nbytes);
	}
	if (!mmv) return 0;	/* Can't alloc -- give up. */

	/*
	 * Fill array with info about interesting segments.
	 */
	n = 0;
	if (mask & (OSMEM_IDATA | OSMEM_DDATA)) {
		heap = TIA->heapHOC;

		for ( ; heap; heap = heap->prev) {
			mmv[n].use = OSMEM_DDATA;
			mmv[n].lo  = (Pointer) heap;
			mmv[n].hi  = (Pointer) ((char *) heap + heap->length);
			n++;
		}
	}
	if (mask & OSMEM_STACK) {
		Pointer sp = StackPtr(getDynStoreArea()->prev->prev->frontier);

		/* Newest stack segment. */
		mmv[n].use = OSMEM_STACK;
		mmv[n].lo  = StackPtr(TCA->stackHOC);
		mmv[n].hi  = StackPtr(TIA->realEOS);
		if (mmv[n].lo <= sp && sp < mmv[n].hi) mmv[n].hi = sp;
		n++;

		/* Older stack segments. */
		stack = StackSeg(TCA->stackHOC);
		for ( ; StackSeg(stack->prev); stack = StackSeg(stack->prev)) {
			mmv[n].use = OSMEM_STACK;
			mmv[n].lo  = StackPtr(stack->prev);
			mmv[n].hi  = StackPtr(stack->end);
			if (mmv[n].lo <= sp && sp < mmv[n].hi) mmv[n].hi = sp;
			n++;
		}
	}
	mmv[n].use = OSMEM_END;

	return mmv;
}

#endif /* CC_C370 */
