/*****************************************************************************
 *
 * os_dos.c: Non-portable, operating system specific code for MS DOS.
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
#include <sys/stat.h>

#ifdef CC_BORLAND
extern unsigned _stklen	   = 64000U;
#endif


/*****************************************************************************
 *
 * :: osCpuTime
 *
 ****************************************************************************/

#define OS_Has_CpuTime

/*
 * One user, one process.  Count elapsed time as CPU.
 * Besides "time" is the only function easily available.
 */

Millisec
osCpuTime(void)
{
	long	t;
	t = time(&t);
	return t * 1000;
}


/*****************************************************************************
 *
 * :: osDate
 *
 ****************************************************************************/

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


/*****************************************************************************
 *
 * :: osCurDirName
 * :: osTmpDirName
 *
 * :: osObjectFileType
 * :: osExecFileType
 *
 ****************************************************************************/

#define FCURDIR		 '.'
#define FDEVSEP		 ':'
#define FDIRSEP		 '\\'
#define FDIRSEPALT	 0
#define FTYPESEP	 '.'
#define FTYPEARC	 'A'

#ifdef CC_GNU
  String osObjectFileType = "o";
  String osExecFileType	  = "out";
#else
  String osObjectFileType = "obj";
  String osExecFileType	  = "exe";
#endif

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


/*****************************************************************************
 *
 * :: osDirIsThere
 *
 ****************************************************************************/

#define OS_Has_DirIsThere

Bool
osDirIsThere(String name)
{
	struct stat	buf;
	if (stat(name, &buf) != -1)
		return S_ISDIR(buf.st_mode);
	return false;
}

/*****************************************************************************
 *
 * :: osDirSwap
 *
 ****************************************************************************/

#define OS_Has_DirSwap

extern char	*getcwd(char *buf, int size);
extern int	chdir(const char *path);

int
osDirSwap(String dest, String orgbuf, Length orgsiz)
{
	String		s;
	int		rc;

	if (orgbuf) {
		s = getcwd(orgbuf, orgsiz);
		if (!s) return -1;
	}

	rc = chdir(dest);
	if (rc == -1) return -1;

	return 0;
}


/*****************************************************************************
 *
 * :: Directory path manipulation.
 *
 ****************************************************************************/

#define FPATHSEP	';'


/*****************************************************************************
 *
 * :: osAlloc
 * :: osFree
 * :: osAllocAlignHint
 * :: osAllocShow
 *
 ****************************************************************************/

#ifdef CC_GNU
# define  OS_UnixLike_Alloc
# include "os_unix.c"
# undef   OS_UnixLike_Alloc
#endif /* CC_GNU */

#ifdef CC_BORLAND
#define OS_Has_Alloc

Pointer
osAlloc(ULong *pnbytes)
{
	extern void far *farmalloc(ULong);
	return farmalloc(*pnbytes);
}

void
osFree(Pointer p)
{
	extern void	farfree(void far *);
	farfree(p);
}

void
osAllocAlignHint(unsigned alignment)
{
}

void
osAllocShow(void)
{
}
#endif /* CC_BORLAND */

/*****************************************************************************
 *
 * :: osMemMap
 *
 ****************************************************************************/

#ifdef CC_GNU
# define  OS_UnixLike_MemMap
# include "os_unix.c"
# undef   OS_UnixLike_MemMap
#endif /* CC_GNU */

#ifdef CC_BORLAND
#if defined(__LARGE__)
#define OS_Has_MemMap

# include <dos.h>
# include <alloc.h>

struct osMemMap *
osMemMap(int mask)
{
	static struct osMemMap	mmv[4];
	struct SREGS		segs;
	int			ix;

	segread(&segs);
	ix = 0;

	if (mask & OSMEM_IDATA) {
		mmv[ix].use = OSMEM_IDATA;
		mmv[ix].lo  = ptrCanon(MK_FP(segs.ds, int0));
		mmv[ix].hi  = ptrCanon(MK_FP(segs.ss, int0));
		ix++;
	}
	if (mask & OSMEM_DDATA) {
		mmv[ix].use = OSMEM_DDATA;
		/* Contains overlay info. */
		mmv[ix].lo  = ptrCanon(MK_FP(segs.ss, _stklen));
		mmv[ix].hi  = ptrCanon(sbrk(int0));
		ix++;
	}
	if (mask & OSMEM_STACK) {
		mmv[ix].use = OSMEM_STACK;
		mmv[ix].lo  = ptrCanon(&ix);
		mmv[ix].hi  = ptrCanon(MK_FP(segs.ss, _stklen));
		ix++;
	}
	mmv[ix].use = OSMEM_END;

	return mmv;
}

#endif /* __LARGE__ */
#endif /* CC_BORLAND */
