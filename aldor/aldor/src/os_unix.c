/*****************************************************************************
 *
 * os_unix.c: Non-portable, operating system specific code for Unix*.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file is meant to be included in opsys.c.
 *
 * If OS_UNIX is defined, then all the parts are included.
 * Individual parts may be activated for platforms with some Unix-like support.
 *
 * Defining:           Includes:
 * -------------       ---------
 * OS_UnixLike_Alloc   sbrk-style osAlloc
 * OS_UnixLike_MemMap  unix-style memory map
 * OS_Procfs_MemMap    /proc memory map
 */

#if defined(OS_UNIX)

#if defined(CC_HPUX) /* wants own source */
#define _INCLUDE_POSIX_SOURCE
#endif

#include "assert.h0"
#include "time.h0"
#include "string.h0"
#include "unistd.h0"

#include <sys/types.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif /* OS_UNIX */

/*****************************************************************************
 *
 * :: osRunConcurrent
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_RunConcurrent

extern int	pipe	(int[2]);
extern int	dup	(int);
/* extern int	fcntl	( int fd, int cmd, ...); */
extern int	close	(int);
/* extern int	execlp  (char *, char *, ... ); */

int
osRunConcurrent(String cmd, FILE **pstdin, FILE **pstdout, FILE **pstderr)
{
	int	rc, pid;
	int	msgioPipe[2],stdinPipe[2],stdoutPipe[2],stderrPipe[2];
	FILE	*msgio;
	char	*s, msgbuf[100];

	if (!cmd) return 0;   /* Just asking if concurrent processes are ok. */

	msgioPipe [0] = msgioPipe [1] = -1;
	stdinPipe [0] = stdinPipe [1] = -1;
	stdoutPipe[0] = stdoutPipe[1] = -1;
	stderrPipe[0] = stderrPipe[1] = -1;

	rc = 0;
	if (rc != -1)		 rc = pipe(msgioPipe);
	if (rc != -1 && pstdin)  rc = pipe(stdinPipe);
	if (rc != -1 && pstdout) rc = pipe(stdoutPipe);
	if (rc != -1 && pstderr) rc = pipe(stderrPipe);
	if (rc != -1)		 rc = fork();

	if (rc == -1) {
		if (msgioPipe [0] != -1) close(msgioPipe [0]);
		if (msgioPipe [1] != -1) close(msgioPipe [1]);
		if (stdinPipe [0] != -1) close(stdinPipe [0]);
		if (stdinPipe [1] != -1) close(stdinPipe [1]);
		if (stdoutPipe[0] != -1) close(stdoutPipe[0]);
		if (stdoutPipe[1] != -1) close(stdoutPipe[1]);
		if (stderrPipe[0] != -1) close(stderrPipe[0]);
		if (stderrPipe[1] != -1) close(stderrPipe[1]);
		return -1;
	}

	pid = rc;

	if (!pid) {
		/* CHILD */

		close(msgioPipe[0]);
		fcntl(msgioPipe[1], F_SETFD, 1); /* Close on exec. */
		msgio = (FILE *) fdopen(msgioPipe[1], "w");

		if (pstdin) {
			close(stdinPipe[1]);
			close(int0);
			rc = dup(stdinPipe[0]);
			close(stdinPipe[0]);
			assert(rc == 0);
		}
		if (pstdout) {
			close(stdoutPipe[0]);
			close(1);
			rc = dup(stdoutPipe[1]);
			close(stdoutPipe[1]);
			assert(rc == 1);
		}
		if (pstderr) {
			close(stderrPipe[0]);
			close(2);
			rc = dup(stderrPipe[1]);
			close(stderrPipe[1]);
			assert(rc == 2);
		}
		
		execlp(cmd, cmd, (char *) 0);

		fprintf(msgio, "Failed to exec %s.\n", cmd);
		fflush (msgio);

		abort();
	}
	else {
		/* PARENT */

		close(msgioPipe[1]);
		msgio = (FILE *) fdopen(msgioPipe[0], "r");

		if (pstdin) {
			close(stdinPipe[0]);
			*pstdin = (FILE *) fdopen(stdinPipe[1], "w");
		}
		if (pstdout) {
			close(stdoutPipe[1]);
			*pstdout = (FILE *) fdopen(stdoutPipe[0], "r");
		}
		if (pstderr) {
			close(stderrPipe[1]);
			*pstdout = (FILE *) fdopen(stderrPipe[0], "r");
		}

		/* If the child execlp succeded, then this should get EOF. */
		s = fgets(msgbuf, sizeof(msgbuf), msgio);
		fclose(msgio);

		if (s) {
			if (pstdin)  fclose(*pstdin);
			if (pstdout) fclose(*pstdout);
			if (pstderr) fclose(*pstderr);
			return -1;
		}
	}
	return 0;
}

#endif /* OS_UNIX */

/*****************************************************************************
 *
 * :: osCpuTime
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_CpuTime

#undef  NULL		/* For platforms with CC_no_redefine, e.g. Sun */
#include <sys/param.h>
#undef  NULL
#define NULL ((Pointer) 0)


#if !defined(HZ) && defined(CLK_TCK)
#  define HZ CLK_TCK
#endif


#  define TICKS_PER_SECOND	 HZ


Millisec
osCpuTime(void)
{
	struct tms	tbuf;
	ULong		mytime, childtime;

	times(&tbuf);
	mytime	  = tbuf.tms_utime  + tbuf.tms_stime;
	childtime = tbuf.tms_cutime + tbuf.tms_cstime;
	return (mytime + childtime) * 1000 / TICKS_PER_SECOND;
}
#endif /* OS_UNIX */

/*****************************************************************************
 *
 * :: osDate
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_Date

String
osDate(void)
{
	time_t	now;
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
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: osPutEnv
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_PutEnv

extern int	putenv(/* String */);

int
osPutEnv(String eqn)
{
	return putenv(eqn);
}

#if defined(OS_NEXT)
extern char	**environ;
extern void	*malloc	(size_t);
extern void	free	(void *);

int
putenv(String eqn)
{
	static char **env0 = 0;

	int	i, n;
	char	*oeqn;

	/* Record original environment -- never free or modify that one. */
	if (!env0) env0 = environ;
	
	/* Determine whether there is already a value in the environment. */ 
	for (n = 0; eqn[n] && eqn[n] != '='; n++) ;
	for (oeqn = 0, i = 0; environ[i]; i++)
		if (!strncmp(environ[i], eqn, n)) { oeqn = environ[i]; break; }
	
	/* Allocate a new environment vector, if needed. */
	for (n = 0; environ[n]; n++) ;
	if (environ == env0 || oeqn == 0) {
		char **env1 = (char **) malloc((n + 2)*sizeof(char *));
		if (!env1) return -1;
		for (i = 0; environ[i]; i++) env1[i] = environ[i];
		env1[i] = 0;
		if (environ != env0) free(environ);
		environ = env1;
	}

	/* Put the equation into the environment. */
	if (oeqn == 0) {
		environ[n]   = eqn;
		environ[n+1] = 0;
	}
	else {
		for (i = 0; environ[i]; i++)
			if (environ[i] == oeqn) { environ[i] = eqn; break; }
	}
	return 0;
}
#endif /* OS_NEXT */
#endif /* OS_UNIX */

/*****************************************************************************
 *
 * :: osCurDirName
 * :: osTmpDirName
 *
 * :: osTmpDirName
 * :: osObjectFileType
 * :: osExecFileType
 *
 ****************************************************************************/

#if defined(OS_UNIX)

#define FCURDIR		'.'
#define FDEVSEP		0	/* No such thing. */
#define FDIRSEP		'/'
#define FDIRSEPALT	0
#define FTYPESEP	'.'
#define FTYPEARC	'a'

String osObjectFileType = "o";
String osExecFileType	= "";


String osCurDirName (void) { return "."; }

String
osTmpDirName (void)
{
	String td;

	/*
	 * Don't need to copy td, since ON THIS PLATFORM the result of
	 * osGetEnv is non-volatile.
	 */
	td = osGetEnv("TMPDIR");
	if (td) return td;

	return "/tmp";
}
#endif /* OS_UNIX */



/*****************************************************************************
 *
 * :: osFnameTempSeed
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_FnameTempSeed

int
osFnameTempSeed (void)
{
	return getpid();
}
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: Directory path manipulation.
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define FPATHSEP	':'

#define OS_Has_DefaultPaths

String	 osDefaultExecutePath = "/bin:/usr/bin:/usr/local/bin";
String	 osDefaultLibraryPath = "/lib:/usr/lib:/usr/local/lib";
String	 osDefaultIncludePath = "/usr/include";
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: osFileRemove
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_FileRemove

extern int unlink(const char *);

int
osFileRemove(String name)
{
	return unlink(name);
}
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: osFileIsThere
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_FileIsThere

Bool
osFileIsThere(String name)
{
	struct stat	buf;
	int		rc;
	rc = stat(name, &buf);
	return rc == 0 && !S_ISDIR(buf.st_mode);
}
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: osFileSize
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_FileSize

Length
osFileSize(String name)
{
	struct stat	buf;
	if (stat(name, &buf) != -1)
		return buf.st_size;
	return 0;
}
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: osFileHash
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_FileHash

Hash
osFileHash(String name)
{
	struct stat	buf;
	if (stat(name, &buf) != -1)
		return (Hash) buf.st_ino;
	return -1;
}
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: osDirIsThere
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_DirIsThere

Bool
osDirIsThere(String name)
{
	struct stat	buf;
	if (stat(name, &buf) != -1)
		return S_ISDIR(buf.st_mode);
	return false;
}
#endif /* OS_UNIX */


/*****************************************************************************
 *
 * :: osDirSwap
 *
 ****************************************************************************/

#if defined(OS_UNIX)
#define OS_Has_DirSwap

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

#ifdef OS_NEXT
#include <sys/param.h>
extern char	*getwd(char *path);

char *
getcwd(char *buf, int size)
{
	char	locbuf[MAXPATHLEN + 1];
	char	*s;

	s = getwd(locbuf);
	if (s) {
		strncpy(buf, locbuf, size);
		return buf;
	}
	else 
		return 0;
}
#endif /* OS_NEXT */

#endif /* OS_UNIX */
	

/*****************************************************************************
 *
 * :: osAlloc
 * :: osFree
 * :: osAllocAlignHint
 * :: osAllocShow
 *
 ****************************************************************************/

#if defined(OS_NEXT)
#define OS_Has_Alloc
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
#endif /* OS_NEXT */


#if (defined(OS_UNIX) && !defined(OS_NEXT)) || defined(OS_UnixLike_Alloc)
#define OS_Has_Alloc

/*
 * It is preferable for calls of osAlloc to provide contiguous memory.
 * For this reason, when possible, we avoid "malloc" and do not store any
 * information in the pieces we pass on from the operating system.
 */

/*
 * The "osStore" list holds on to all pieces.  This keeps the program memory
 *   clean and contiguous.
 * The busy pieces are kept so we know how big they are if freed.
 * The free pieces which are not at the "sbrk" frontier are kept
 *   to be freed later.
 * The admin pieces are used for our own linked lists.
 *
 * The list is consulted first when a new allocation request is made.
 * If any piece suffices, the lowest address one is taken.
 * This way the higher address pieces can eventually be disowned.
 *
 * The static structure "osStoreHd" allows loops to keep a "previous" pointer.
 * "osFreeFrontier" is what "sbrk" must return to be able to disown store.
 */

#define OS_STORE_ADMIN	0	/* Used for our lists */
#define OS_STORE_BUSY	1	/* In use by program */
#define OS_STORE_FREE	2	/* Available for use or to be freed by sbrk */
#define OS_STORE_NALLOC (4096/sizeof(struct osStore))
				/* Number to alloc at once */

struct osStore {
	int		use;	/* One of OS_STORE_{ADMIN,BUSY,FREE}. */
	Pointer		piece;	/* User piece pointer in canonical form. */
	ULong		nbytes; /* Number of bytes in it. */
	Pointer		piece0; /* The allocn pointer in canonical form. */
	ULong		nbytes0;/* Number of bytes in it. */
	struct osStore *next;	/* Canonical ptr to higher address piece. */
};

#if !defined(OS_MAC_OSX)
/* The OS_MAC_OSX version does not use sbrk0
 * Making this code conditional avoids compiler warning messages.
 */
static Pointer		sbrk0 = 0;
#endif /* defined(OS_MAC_OSX) */

static unsigned		alignHint = 0;

static struct osStore	osStoreHd	= {0, 0, 0, 0, 0, 0};
static struct osStore * osFreeList	= 0;

#if !defined(OS_MAC_OSX)
/* osFreeFrontier is not used by the OS_MAC_OSX version */
static Pointer		osFreeFrontier	= 0;
#endif /* !defined(OS_MAC_OSX) */

local  int		osAllocFreeList (void);
local  void		osAllocLinkIn	(struct osStore *);


void
osAllocAlignHint(unsigned hint)
{
	alignHint = hint;
}

#if !defined(OS_MAC_OSX)
Pointer
osAlloc(ULong *pnbytes)
{
	ULong		nbytes = *pnbytes, nextra;
	Pointer		p, p0;
	struct osStore	*fx;

	/*
	 * Avoid creating zero-sized pieces.
	 */
	if (!sbrk0) sbrk0 = (char *) sbrk(int0);
	if (nbytes == 0) nbytes = 1;

	/*
	 * Consult free list and return lowest address suitable piece.
	 */
	for (fx = osStoreHd.next; fx; fx = fx->next)
		if (fx->use == OS_STORE_FREE && fx->nbytes >= nbytes) {
			fx->use	 = OS_STORE_BUSY;
			*pnbytes = fx->nbytes;
			return fx->piece;
		}
	/*
	 * Ensure there is a link for a new piece.
	 */
	if (!osFreeList	 && osAllocFreeList() == -1) {
		*pnbytes = 0;
		return 0;
	}

	/*
	 * Will allocate piece.	 First align sbrk.
	 */
	p0 = (Pointer) sbrk(int0);

	if (alignHint == 0 || ptrToLong(p0) % alignHint == 0)
		nextra = 0;
	else  {
		nextra = alignHint - ptrToLong(p0) % alignHint;

		if (ptrToLong((Pointer) sbrk(nextra)) == -1) {
			*pnbytes = 0;
			return 0;
		}
	}

	/*
	 * Allocate piece and record it.
	 */
	p = (Pointer) sbrk(nbytes);
	if (ptrToLong(p) == -1) {
		*pnbytes = 0;
		return 0;
	}

	fx	   = osFreeList;
	osFreeList = osFreeList->next;
	fx->use	   = OS_STORE_BUSY;
	fx->piece  = ptrCanon(p);
	fx->piece0 = ptrCanon(p0);
	fx->nbytes = ptrDiff((char *) sbrk(int0), (char *) p);
	fx->nbytes0= fx->nbytes + nextra;

	osAllocLinkIn(fx);
	*pnbytes = fx->nbytes;
	return fx->piece;
}

#define pcEnd(pc)	ptrCanon((char *)((pc)->piece) + (pc)->nbytes)
#define pcAbuts(p,q)	(pcEnd(p) == (q)->piece)

void
osFree(Pointer p)
{
	struct osStore	*fp, *ff, *fn;
	Pointer		ex;

	/*
	 * Find the link and predecessor corresponding to this piece.
	 */
	p  = ptrCanon(p);

	for (fp = &osStoreHd, ff = fp->next; ff; fp = ff, ff = fp->next)
		if (ff->piece == p) break;

	if (!ff) return;	/* Bad free request; ignore. */

	/*
	 * Maintain reccord of the end of the highest free piece.
	 */
	ex = ptrCanon(pcEnd(ff));
	if (ex > osFreeFrontier) osFreeFrontier = ex;

	/*
	 * Mark free, and merge if possible.
	 */
	ff->use = OS_STORE_FREE;
	fn	= ff->next;

	if (fn && fn->use == OS_STORE_FREE && pcAbuts(ff, fn)) {
		ff->nbytes += fn->nbytes;
		ff->next   =  fn->next;
		fn->next   =  osFreeList;
		osFreeList = fn;
	}
	if (fp && fp->use == OS_STORE_FREE && pcAbuts(fp, ff)) {
		fp->nbytes += ff->nbytes;
		fp->next   =  ff->next;
		ff->next   =  osFreeList;
		osFreeList = ff;
	}
	/*
	 * Return pending high memory to OS, if possible.
	 */
	if (ptrCanon(sbrk(int0)) == osFreeFrontier) {
		ex = 0; /* New frontier */
		for (fp= &osStoreHd, ff=fp->next; ff; fp=ff, ff=fp->next) {
			if (ff->use != OS_STORE_FREE) continue;
			if (pcEnd(ff) == osFreeFrontier) break;
			ex = pcEnd(ff);
		}
		assert(ff && ff->next == 0);
		sbrk(-ff->nbytes0);
		fp->next   = 0;
		ff->next   = osFreeList;
		osFreeList = ff;
		osFreeFrontier = ex;
	}
}

/*
 * Place a link in the pieces list, preserving address order of the pieces.
 * The "piece" field is a pointer in canonical form.
 */
local void
osAllocLinkIn(struct osStore *fx)
{
	struct osStore	*fp, *ff;

	for (fp = &osStoreHd, ff = fp->next; ; fp = ff, ff = fp->next)
		if (!ff || ff->piece > fx->piece) {
			fp->next = fx;
			fx->next = ff;
			break;
		}
}

/*
 * Allocate more links into the free list.
 * Success => 0.  Failure => -1.
 */
local int
osAllocFreeList(void)
{
	struct osStore	*fx;
	Pointer		p;
	ULong		i;

	p  = (Pointer) sbrk(OS_STORE_NALLOC*sizeof(struct osStore));
	if ((long)(p) == -1) return -1;

	fx = (struct osStore *) p;
	for (i = 0; i < OS_STORE_NALLOC; i++) {
		fx[i].next = osFreeList;
		osFreeList = &fx[i];
	}

	fx	   = osFreeList;
	osFreeList = osFreeList->next;
	fx->use	   = OS_STORE_ADMIN;
	fx->nbytes = ptrDiff((char *) sbrk(int0), (char *) p);
	fx->piece  = ptrCanon(p);

	osAllocLinkIn(fx);
	return 0;
}

void
osAllocShow(void)
{
	struct osStore *ff;

	for (ff = osStoreHd.next; ff; ff = ff->next) {
		char	*s;
		switch (ff->use) {
		case OS_STORE_ADMIN: s = "Ad"; break;
		case OS_STORE_BUSY:  s = "Us"; break;
		case OS_STORE_FREE:  s = "Fr"; break;
		default:	     s = "??"; break;
		}
		printf("%s [%#lx @ %#lx]\n",
			s, ff->nbytes, ptrDiff((char *) ff->piece, sbrk0));
	}
	printf("Delta sbrk  = %#lx\n", ptrDiff(sbrk(int0), sbrk0));
}
#else /* defined(OS_MAC_OSX) */
#include "os_macosx_vm.c"
#endif

#endif /* OS_UNIX || OS_UnixLike_Alloc */


/****************************************************************************
 *
 * :: osMemMap
 *
 ****************************************************************************/

#if !defined(OS_MAC_OSX) /* mac version is in "os_macosx_vm.c" */
#if defined(OS_UNIX) || defined(OS_UnixLike_MemMap)
#define OS_Has_MemMap

extern char	**environ;

#define		ROUND_UP(n,d)	((n) % (d) ? (n) + (d) - (n) % (d) : (n))
#define		REMAINDER(n, d) ((n) % (d))
#define 	ROUND_DOWN(n, d) ((n) - (n) % (d))

#if defined(OS_AIX_RS)
# define OsBegInitData	((Pointer) (&_data))
# define OsEndInitData	((Pointer) (&_end))
  extern int	_data, _end;
#endif

#if defined(OS_AIX_RT)
#include <sys/seg.h>
# define OsBegInitData	((Pointer) (BASE(DATASEG)))
# define OsEndInitData	((Pointer) (&end))
  extern int	end;
#endif

#if defined(OS_AIX_PS2)
# define OsBegInitData	((Pointer) (0x00400000L))
# define OsEndInitData	((Pointer) (&end))
  extern int	end;
#endif

#if defined(OS_AIX_370)
# define OsBegInitData	((Pointer) (0x00200000L))
# define OsEndInitData	((Pointer) (&end))
  extern int	end;
#endif

#if defined(OS_AIX_ESA) 
#include <sys/addrconf.h>
local Pointer
osMachBegInitData()
{
	struct addressconf addrConf[AC_N_AREAS];
	getaddressconf(addrConf, sizeof(addrConf));
	return (Pointer) addrConf[AC_DATA].ac_base;
}
# define OsBegInitData	osMachBegInitData()
# define OsEndInitData	((Pointer) (&_end))
  extern int	_end;
#endif

#if defined(OS_SUN) && !defined(OS_SUNOS5)
#include <sys/param.h>
# define OsBegInitData	((Pointer) (ROUND_UP((long) &etext, DATA_ALIGN)))
# define OsEndInitData	((Pointer) (&end))
  extern int	etext, end;
#endif

#if defined(OS_SUNOS5) 
#define OS_Procfs_MemMap
#endif

#if defined(OS_LINUX)
#if 1 
#include <sys/param.h>
# define OsBegInitData	((Pointer) (ROUND_UP((long) &etext, EXEC_PAGESIZE)))
# define OsEndInitData	((Pointer) (&end))
  extern int	etext, end;
#endif
#define OS_Linux_Procfs_Memmap 
#endif

#if defined(OS_OSF1)
#define OS_Procfs_MemMap
#endif

#if defined(OS_NEXT)
#include <sys/param.h>
# define OsBegInitData	((Pointer) (ROUND_UP((long) get_etext(), NBPG)))
# define OsEndInitData	((Pointer) get_end())
  extern void	*get_etext(void), *get_end(void);
#endif

#if defined(OS_MS_DOS) && defined(CC_GNU)
# define OsBegInitData	((Pointer) ((Pointer) (ROUND_UP((long) &etext, 0x1000)))
# define OsEndInitData	((Pointer) (&end))
  extern int	end;
#endif

#if defined(OS_IRIX)
# define OsBegInitData	((Pointer) (&_fdata))
# define OsEndInitData	((Pointer) (&end))
  extern int	_fdata,end;
#endif

#if defined(OS_HPUX)
# define OsBegInitData	((Pointer) (&__data_start))
# define OsEndInitData	((Pointer) (&end))
  extern int	__data_start, end;
#endif


/*
 * Maximum map is:
 *   IData, DData, Stack, End
 */

#if !defined(OS_Procfs_MemMap) && !defined(OS_Linux_Procfs_Memmap)

struct osMemMap **
osMemMap(int mask)
{
	static struct osMemMap	mmv[4];
	static struct osMemMap *mmvp[4];
	static Pointer	        mmenv = 0;
	int i;
	struct osMemMap		*mm;
	Pointer			dhi, slo, shi;

	dhi = (Pointer) sbrk(int0);

	/*
	 * If environ is in heap, then we can't find the base of the stack.
	 * If we do find it, save it for next time (in case of putenvs).
	 */
	if (!mmenv) mmenv = (Pointer) environ;
	if (OsEndInitData <= mmenv && mmenv <= dhi && (mask & OSMEM_STACK))
		return 0;

	slo = (Pointer) &mm;
	shi = mmenv;
	if (slo > shi) { Pointer t = slo; slo = shi; shi = t; }

	mm = mmv;
	if (mask & OSMEM_IDATA) {
		mm->use = OSMEM_IDATA;
		mm->lo  = OsBegInitData;
		mm->hi  = OsEndInitData;
		mm++;
	}
	if (mask & OSMEM_DDATA) {
		mm->use = OSMEM_DDATA;
		mm->lo  = OsEndInitData;
		mm->hi  = dhi;
		mm++;
	}
	if (mask & OSMEM_STACK) {
		mm->use = OSMEM_STACK;
		mm->lo  = slo;
		mm->hi  = shi;
		mm++;
	}
	mm->use = OSMEM_END;
	/* print what we found */
	mm = mmv;
	i = 0;
	while (mm->use != OSMEM_END) {
	  char *desc;
	  if (mm->use == OSMEM_DDATA) desc = "Data";
	  if (mm->use == OSMEM_STACK) desc = "Stack";
	  mmvp[i] = mm;
#if 0
#if EDIT_1_0_n1_07
          printf("%s: %p --> %p\n", desc, mm->lo, mm->hi);
#else
          printf("%s: %lx --> %lx\n", desc, mm->lo, mm->hi);
#endif
#endif
	  mm++; i++;
	}
	mmvp[i] = mm;
	return mmvp;
}
#elif !defined(OS_Linux_Procfs_Memmap)  /* OS_Procfs_MemMap */
/* 
 * Shared library code.  This works on the alpha, 
 * and similar code will work under solaris.  Linux
 * is pretty similar too.
 */
  
#define OS_Has_MemMap

#define MAX_MMAPS 30


#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/fault.h>
#include <sys/syscall.h>
#include <sys/procfs.h>
#include <unistd.h>


void procSysMap();
static prmap_t hmap[512];

struct osMemMap ** osMemMap(int mask)
{
  static struct osMemMap	mmv[MAX_MMAPS];
  static struct osMemMap *      mmvp[MAX_MMAPS];
  struct osMemMap		*mm;
  Pointer			slo;
  prmap_t *maps;
  int i;
  
  slo = &mm;
  mm  = mmv;
  i   = 0;
  /* IData is indistinguishable from DData (nearly, see `man end'). */
  procSysMap();
  maps = hmap;
  while (maps[i].pr_vaddr != NULL) {
    caddr_t lo = maps[i].pr_vaddr;
    caddr_t hi = maps[i].pr_vaddr + maps[i].pr_size;
    
    /* Ignore read-only segments (because they will never contain 
       any pointers to Aldor objects ) */
    if ( (maps[i].pr_mflags & MA_WRITE) == 0) ;
    /* check if it is a stack map */
    else if ( lo <= (caddr_t) slo && (caddr_t) slo < hi) {
      /* slo is in the stack - it's a local variable */
      if (mask & OSMEM_STACK) {
	mm->use = OSMEM_STACK;
	mm->lo  = slo; /* estimate of "top" of stack */
	mm->hi  = hi;
	mm++;
      }
    }
    /* if we are not looking for data maps we are done */
    else if ( (mask & OSMEM_DDATA) == 0) ;
    /* we ARE looking for data maps */
    /* check if the previous one was a data map and 
       if contiguous collapse them */
    else if (mm[-1].use == OSMEM_DDATA && mm[-1].hi == lo)
      mm[-1].hi = hi;
    /* "new" data map - record it */
    else {
      if (mask & OSMEM_DDATA) {
	mm->lo = lo;
	mm->hi = hi;
	mm->use = OSMEM_DDATA;
	mm++;
      }
    }
    i++;
  }
  mm->use = OSMEM_END;
  /* print what we found */
  mm = mmv;
  i = 0;
  while (mm->use != OSMEM_END) {
    char *desc;
    if (mm->use == OSMEM_DDATA) desc = "Data";
    if (mm->use == OSMEM_STACK) desc = "Stack";
    mmvp[i] = mm;
#if 0
#if EDIT_1_0_n1_07
    printf("%s: %p --> %p\n", desc, mm->lo, mm->hi);
#else
    printf("%s: %lx --> %lx\n", desc, mm->lo, mm->hi);
#endif
#endif
    mm++; i++;
  }
  mmvp[i] = mm;
  return mmvp;
}
  

void
procSysMap()
{
  char pid[50];
  int fd;
  int nm, ret;

#if EDIT_1_0_n2_01
  sprintf(pid, "/proc/%d", (int) getpid());
#else
  sprintf(pid, "/proc/%d", getpid());
#endif
  fd = open(pid, O_RDONLY);
  if (!fd) perror("Open");

  ret = ioctl(fd, PIOCNMAP, &nm);
  if (ret == -1) perror("ioctl");

  /* NB: The PIOCMAP call needs one extra space for a zero segment */
  if (nm+1 > 512)perror("two many maps");
  
  ret = ioctl(fd, PIOCMAP, hmap);
  if (ret == -1) perror("ioctl2");

  close(fd);
}

#elif defined(OS_Linux_Procfs_Memmap)  /* OS_Procfs_MemMap */

#define MAX_MMAPS 30
/* 
   code for Linux - does not support ioctl 
   CAREFUL: if we close the FILE* too early there will be 
   a munmap before we start marking. This means that our 
   memory map will be wrong and we will segfault.
   
 */

extern int etext,end;

struct osMemMap **osMemMap(int mask)
{
  static struct osMemMap	mmv[MAX_MMAPS];
  static struct osMemMap*	mmvp[MAX_MMAPS];
  
  struct osMemMap		*mm;
  Pointer			slo;
  static FILE *fp =0 ;
  char  line[1024];
  char  maps[100];
#if EDIT_1_0_n1_07
  unsigned int i,read_only;
  unsigned long lo, hi;
#else
  unsigned int i,lo,hi,read_only;
#endif
  char perm[4];


  if (fp) fclose(fp);
  slo = &mm;
  mm  = mmv;
  sprintf(maps,"/proc/%d/maps",getpid());
  fp= fopen(maps,"r");

  while (fgets(line,1022,fp)) {
#if 0
    fputs(line,stdout);
#endif
#if EDIT_1_0_n1_07
    sscanf(line,"%lx-%lx %4c",&lo,&hi,perm);
#else
    sscanf(line,"%x-%x %4c",&lo,&hi,perm);
#endif
    read_only = perm[1] == 'w' ? 0 :1;
    /* Ignore read-only segments (because they will never contain 
       any pointers to Aldor objects ) */
    if (read_only) ; 
    /* check if it is a stack map */
    else if ( (Pointer)lo <= slo && slo < (Pointer) hi) {
      /* slo is in the stack - it's a local variable */
      if (mask & OSMEM_STACK) {
	mm->use = OSMEM_STACK;
	mm->lo  = slo; /* estimate of "top" of stack */
	mm->hi  = (Pointer)hi;
	mm++;
      }
    }
    /* if we are not looking for data maps we are done */
    else if ( (mask & OSMEM_DDATA) == 0) ;
    /* we ARE looking for data maps */
    /* check if the previous one was a data map and 
       if contiguous collapse them */
#if EDIT_1_0_n1_07
    else if (mm[-1].use == OSMEM_DDATA && mm[-1].hi == (Pointer) lo)
      mm[-1].hi = (Pointer) hi;
#else
    else if (mm[-1].use == OSMEM_DDATA && mm[-1].hi == (Pointer)lo)
      mm[-1].hi = (Pointer)hi;
#endif
    /* "new" data map - record it */
    else {
      if (mask & OSMEM_DDATA) {
#if EDIT_1_0_n1_07
	mm->lo = (Pointer)lo;
	mm->hi = (Pointer)hi;
#else
	mm->lo = (Pointer) (UAInt) lo;
	mm->hi = (Pointer) (UAInt) hi;
#endif
	mm->use = OSMEM_DDATA;
	mm++;
      }
    }
  }
  mm->use = OSMEM_END;
  mm = mmv; i =0;
  /* print what we found */
  while (mm->use != OSMEM_END) {
#if 0
#if EDIT_1_0_n2_07
    const char *desc = "";
#else
    char *desc = "";
#endif
    if (mm->use == OSMEM_DDATA) desc = "Data";
    if (mm->use == OSMEM_STACK) desc = "Stack";
#if EDIT_1_0_n1_07
    printf("%s: %lx --> %lx\n", desc, mm->lo, mm->hi);
#else
    printf("%s: %p --> %p\n", desc, mm->lo, mm->hi);
#endif
#endif
    mmvp[i]=mm;
    mm++;
    i++;
  }
  mmvp[i]=mm;
  return mmvp;
}


#endif /* OS_Procfs_MemMap */
#endif /* OS_UNIX || OS_UnixLike_MemMap */
#endif /* !defined(OS_MAC_OSX) */
/*****************************************************************************
 *
 * :: osRandom
 *
 ****************************************************************************/

#if defined(OS_SUN)
#define OS_Has_Random

Length
osRandom(void)
{
	return rand() >> 16;
}

#endif /* OS_SUN */

/*****************************************************************************
 *
 * :: osGetNamedSymbol
 *
 ****************************************************************************/

#if 0 && defined(OS_SUN)

#define OS_Has_GetSymbol
Pointer
osGetNamedSymbol(String sym)
{
	void *self, *res;

	self = dlopen(NULL, RTLD_LAZY);
	
	val = dlsym(self, sym);
	dlclose(self);

	return val;
}

#endif






