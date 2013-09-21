/*****************************************************************************
 *
 * os_win32.c: Non-portable, operating system specific code for Win32(s).
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(FOAM_RTS) && !defined(CC_MICROSOFT) && !defined(CC_GNU)
# define OS_Has_SpecialStreams
# define OS_Has_FixCmdLine
#endif

/*
 * This file is meant to be included in opsys.c.
 */

#include "assert.h0"
#include "time.h0"
#include "string.h0"
#include "cport.h"
#include <sys/stat.h>
#include <windows.h>

#if defined(OS_Has_SpecialStreams)

#include "strops.h"
#include "buffer.h"

FILE * osStdin;
FILE * osStdout;
FILE * osStderr;

static Buffer   osStdinBuf;
static Buffer   osStdoutBuf;
static Buffer   osStderrBuf;

#endif /* OS_Has_SpecialStreams */

#define MAX_HEAP_SECTIONS               1024

static Bool osWin32s = false;   /* True if we're running under win32s.  */

void osSetStreams(FILE * fin, FILE * fout, FILE * ferr);

static int osInitAlloc(void);

static Millisec		osInitTime = 0;

/*****************************************************************************
 *
 * :: osInit
 *
 ****************************************************************************/

#define OS_Has_Init

void
osInit()
{
	/* Initialise our memory management system */
	osInitAlloc();

#if defined(OS_Has_SpecialStreams)
	osStdinBuf = NULL;
	osStdoutBuf = NULL;
	osStderrBuf = NULL;
#endif /* OS_Has_SpecialStreams */

	osSetStreams(stdin, stdout,stderr);
	osCpuTime();	/* start the clock */
}

local Bool
osIsWin32s(void)
{
        return osWin32s;
}

#if defined(OS_Has_SpecialStreams)

#define OS_Has_DisplayMessage

void
osDisplayMessage(char * msg)
{
	MessageBox(NULL, msg, "Aldor", MB_ICONHAND | MB_OK);
}
#endif /* OS_Has_SpecialStreams */

/*****************************************************************************
 *
 * :: osStreams
 *
 ****************************************************************************/

#if defined(OS_Has_SpecialStreams)

/* This nastiness is because the -gloop mode needs a stream 
   of input, and we sometimes use a buffer for this. */

#define OS_REDIRECTED_STDIN	(FILE*)0x1
#define OS_REDIRECTED_STDOUT	(FILE*)0x2
#define OS_REDIRECTED_STDERR	(FILE*)0x3

/* Redirect stdio streams to the streams given as parameters.
 * A NULL stream in any parameter is ignored.  Example:
 *    osSetStreams(NULL, fout, NULL)
 * redirects stdout to fout and leaves other streams alone.
 */
void
osSetStreams(FILE * fin, FILE * fout, FILE * ferr)
{
	if (fin)  osStdin  = fin;
	if (fout) osStdout = fout;
	if (ferr) osStderr = ferr;
}

/* Redirect stdin to the buffer "buf". */
void
osSetStdinBuffer(Buffer buf)
{
	osStdinBuf = buf;
	osStdin = OS_REDIRECTED_STDIN;
}

/* Redirect stdout to the buffer "buf". */
void
osSetStdoutBuffer(Buffer buf)
{
	osStdoutBuf = buf;
	osStdout = OS_REDIRECTED_STDOUT;
}

/* Redirect stderr to the buffer "buf". */
void
osSetStderrBuffer(Buffer buf)
{
	osStderrBuf = buf;
	osStderr = OS_REDIRECTED_STDERR;
}

int
osGetc(FILE * f)
{
    signed char c;
    int x;
    if (f == osStdin && osStdinBuf) {
          c = (signed char) bufGet1(osStdinBuf);
		if (c == EOF) bufBack1(osStdinBuf);
          return (int) c;
	}
    else {
          x = getc(f);
          printf("[%x %c]\n", x, isprint(x) ?  x : "?");
          return x;
    }
}

int
osPutc(FILE * f, int c)
{
    if (f == osStdout && osStdoutBuf)
        return bufPutc(osStdoutBuf, c);
    else if (f == osStderr && osStderrBuf)
        return bufPutc(osStderrBuf, c);
    else
        return putc(c, f); 
}

int
osFPrintf(FILE * f, char * format, ...)
{
	int 	res;
	va_list	argp;

	va_start(argp, format);

	if (f == osStdout && osStdoutBuf)
		res = bufVPrintf(osStdoutBuf, format, argp);
	else if (f == osStderr && osStderrBuf)
		res = bufVPrintf(osStderrBuf, format, argp);
	else
		res = vfprintf(f, format, argp);

	va_end(argp);

	return res;
}

int
osFEof(FILE * f)
{
    if (f == osStdin && osStdinBuf)
        return bufNext1(osStdinBuf) == (char) EOF;
    else
        return feof(f);
}

#endif /* OS_Has_SpecialStreams */

/*****************************************************************************
 *
 * :: osRun
 *
 ****************************************************************************/

#define OS_Has_Run

int
osRunRedirect(String cmd, BOOL wait, HANDLE hin, HANDLE hout, HANDLE herr)
{
	HANDLE				sin, sout, serr;
	HANDLE				din, dout, derr;
	static STARTUPINFO		si;
	static PROCESS_INFORMATION	pi;
	BOOL				rc;
	DWORD                           ec;

        sin  = GetStdHandle(STD_INPUT_HANDLE);
        sout = GetStdHandle(STD_OUTPUT_HANDLE);
        serr = GetStdHandle(STD_ERROR_HANDLE);
        hin  = hin  ? hin : sin;
        hout = hout ? hout : sout;
        herr = herr ? herr : serr;

        FlushFileBuffers(hout);
        FlushFileBuffers(herr);
        if (!DuplicateHandle(GetCurrentProcess(), /* me */
                        hout,                     /* handle to copy */
                        GetCurrentProcess(),
                        &dout, int0, true, 
                        DUPLICATE_SAME_ACCESS))
                return -1;
        if (!DuplicateHandle(GetCurrentProcess(), /* me */
                        herr,                     /* handle to copy */
                        GetCurrentProcess(),
                        &derr, int0, true, 
                        DUPLICATE_SAME_ACCESS))
                return -1;
        if (!DuplicateHandle(GetCurrentProcess(), /* me */
                        hin,                     /* handle to copy */
                        GetCurrentProcess(),
                        &din, int0, true, 
                        DUPLICATE_SAME_ACCESS))
                return -1;
        /* Create process. */
        memset(&si, int0, sizeof(si));
	si.cb		= sizeof(si);
	si.lpReserved	= NULL;
	si.lpDesktop	= NULL;
	si.lpTitle	= "!";
	si.dwFlags	= 0;
	si.cbReserved2	= 0;
	si.lpReserved2	= NULL;

        si.dwFlags    =  STARTF_USESTDHANDLES;
        si.hStdInput  = hin ? din : sin;
        si.hStdOutput = hout ? dout : sout;
        si.hStdError  = herr ? derr : serr;

        rc = CreateProcess(NULL, cmd, NULL, NULL, TRUE,
			   NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
        CloseHandle(dout);
        CloseHandle(derr);
        CloseHandle(din);
	if (rc == FALSE) return -1;

	if (wait == TRUE) {
		rc = WaitForSingleObject(pi.hProcess, INFINITE);
		if (rc == WAIT_FAILED) return -1;
	}
        if (wait && !GetExitCodeProcess(pi.hProcess, &ec)) {
                printf("Unknown status\n");
                return -1;
        }

        return wait ? rc : (int)pi.hProcess;
}

#if __CYGWIN__
int
osRun(String cmd)
{
	return system(cmd);
}
#else
int
osRun(String cmd)
{
	return osRunRedirect(cmd, TRUE, NULL, NULL, NULL);
}
#endif


/*****************************************************************************
 *
 * :: osCpuTime
 *
 ****************************************************************************/

#define OS_Has_CpuTime

/*
 * Count elapsed time as CPU.
 */

Millisec
osCpuTime(void)
{
	Millisec t = (Millisec) GetTickCount();
	if (osInitTime == 0) osInitTime = t;
	return t - osInitTime;
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
        static char buf[50];
        SYSTEMTIME date;

        GetSystemTime(&date);

	sprintf(buf, "%02d/%02d/%02d",
		date.wMonth, date.wDay, date.wYear);
        return buf;
}


/*****************************************************************************
 *
 * :: osGetEnv
 * :: osPutEnv
 *
 ****************************************************************************/

#define OS_Has_GetEnv

#define BUF_SIZE	4096

#ifdef __CYGWIN__
String
osGetEnv(String envar)
{
	return getenv(envar);
}
#else
String
osGetEnv(String envar)
{
	static char buf[BUF_SIZE];
	LPSTR       str;
	ULong requiredBytes;
	MostAlignedType *stoAlloc(unsigned long, unsigned long);

	requiredBytes = (ULong) GetEnvironmentVariable(envar, buf, (DWORD) BUF_SIZE);

	if (requiredBytes) {
		str = (LPSTR) stoAlloc((unsigned) 0, requiredBytes + 2);
		strcpy(str, buf);
		str[requiredBytes] = '\0';
		return str;
	}
	else return 0;
}
#endif

#define OS_Has_PutEnv

extern int	putenv(/* String */);
extern void stoFree (Pointer p);

int
osPutEnv(String eqn)
{
	MostAlignedType *stoAlloc(unsigned long, unsigned long);
	String var = (String)stoAlloc((unsigned)0, strlen(eqn) + 2);
	String value;
	int    ret = 0;

	strcpy(var, eqn);
	for (value = var; *value && *value != '='; value++)
	     ;
	assert(*value == '=');
	*value = '\0';

	value += 1;

	if (SetEnvironmentVariable(var, value))
		ret = 0;
	else
		ret = -1;

	stoFree(var);
	return ret;
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
#ifdef __CYGWIN__
#define FDIRSEP		 '/'
#else
#define FDIRSEP		 '\\'
#endif
#define FDIRSEPALT	 '/'
#define FTYPESEP	 '.'
#define FTYPEARC	 'A'

#if defined (CC_GNU)
String osObjectFileType = "o";
#else	
String osObjectFileType = "obj";
#endif
String osExecFileType	= "exe";


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
 * :: osIsInteractive, osFileRemove, osFileRename
 *
 ****************************************************************************/

#define OS_Has_IsInteractive

Bool
osIsInteractive(FILE *f)
{
#if 0
        return true; 
#else
	return _isatty(_fileno(f));
#endif
}


#define OS_Has_FileRemove

int
osFileRemove(String name)
{
	int res;
	Sleep(10);
	res = DeleteFile((LPCSTR) name);
#if 0
	if (!res) {
		DWORD dw = GetLastError();
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, (DWORD) 0, NULL);
		printf("Error: %s while deleting file %s\n", lpMsgBuf, name);
		LocalFree(lpMsgBuf);
	}
#endif
	return res;
}


#define OS_Has_FileRename

int
osFileRename(String from, String to)
{
	return MoveFile((LPCSTR) from, (LPCSTR) to);
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
	DWORD res = GetFileAttributes((LPCSTR) name);

	return res != 0xffffffff &&
	       (res & FILE_ATTRIBUTE_DIRECTORY);
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
	DWORD		res;

	if (orgbuf) {
                res = GetCurrentDirectory((DWORD) orgsiz, (LPTSTR) orgbuf);
                if (res == 0) return -1;
	}

        if (SetCurrentDirectory(dest))
            return 0;
        else
            return 1;
}

/*****************************************************************************
 *
 * :: Directory path manipulation.
 *
 ****************************************************************************/

#define FPATHSEP	';'

/*****************************************************************************
 *
 * :: osFixCmdLine
 *
 ****************************************************************************/

/* Some C compilers for Win32 pass arguments to main in a single string.
 * osFixCmdLine() breaks out the arguments into separate strings.
 */

#if defined(OS_Has_FixCmdLine)

local char *
osGetNextArg(char** pstart)
{
    char * p;
    char * ret;
    int  len = 0;
    
    while (**pstart && **pstart == ' ')
        *pstart += 1;
        
    if (**pstart == 0) return NULL;
    
    p = *pstart;
    while (*p && *p != ' ') {
        len += 1;
        p += 1;
    }
    
    ret = (char *) stoAlloc((unsigned) 0,((len + 1) * sizeof(char))+1);
    strncpy(ret, *pstart, len);
    ret[len] = 0;
    *pstart = p;
    return ret;
}

local int
osCountArgs(char * cmdLine)
{
    int     argc = 0;
    Bool    findSpace = false;    

    while (*cmdLine) {
		if (findSpace && *cmdLine == ' ') {
            findSpace = false;
        }
		else if (!findSpace && *cmdLine != ' ') {
            findSpace = true;
			argc += 1;
        }
        cmdLine += 1;
    }
    
    return argc;
}

int
osFixCmdLine(int* pargc, char*** pargv)
{
    char *  cmdLine = (*pargv)[0];
    char *  arg;
	int	argc = 0;

	*pargc = osCountArgs(cmdLine);
    *pargv = (char **) stoAlloc((unsigned) 0, (*pargc * sizeof(char *))+1);

    while ((arg = osGetNextArg(&cmdLine)) != NULL)
        (*pargv)[argc++] = arg;
        
    assert(argc == *pargc);
    return 0;
}

#endif /* OS_Has_FixCmdLine */

/*****************************************************************************
 *
 * :: osAlloc, osFree, etc.
 *
 ****************************************************************************/

#define OS_Has_Alloc

/*===========================================================================*/
/*
 *                       Win32 OS-level memory manager
 *                       =============================
 *
 * The functions and data structures below provide an interface to the OS-level
 * memory-manager. Calls to osAlloc are expected to be made to satisfy requests
 * in units of the page-size for the platform (usually 4K). Smaller requests
 * can be handled of course, but may degrade what little performance we have.
 * Calls to osFree may result in pages being removed from the physical memory
 * map (which is good) but may lead to (Aldor) heap fragmentation. 
 *
 * The osMemMap function returns an array of nodes describing a region of the
 * process (virtual) address space. Static/initialised data sections (IDATA)
 * could possibly be identified but are not. Instead, all read-write memory is
 * classed as dynamic data (DDATA) unless it is the stack.
 *
 * We assume that read-only pages do not contain pointers into the Aldor heap.
 * This could be an invalid assumption if the user has code which plays games
 * with page protection (allocates a read-write page, fills it with pointers
 * and then makes it read-only). 
 *
 *
 * Notes on Win32 Memory (ignoring Win32s)
 * =======================================
 *
 * [This description derives from experiments under Win98 made in conjunction
 *  with Microsoft documentation of Win32 memory management under NT ...]
 *
 * Each Win32 process can address a total of 4Gb of virtual memory. However,
 * the OS splits this in half reserving the bottom 2Gb for the application and
 * the rest for itself. As a result, we will consider the total address space
 * of the application to be just 2Gb and ignore the bits we cannot access.
 *
 * The virtual address space of a typical Win95/98 application might look like:
 *
 *    +---------------------------------------+ 2Gb
 *    | unmapped, unreserved virtual memory   |
 *    +---------------------------------------+
 *    | alternating regions of committed and  |
 *    | reserved pages. Contains parts of the |
 *    | default heap, malloc'd memory and any |
 *    | pages explicitly reserved/committed   |
 *    | by the application via VirtualAlloc   |
 *    +---------------------------------------+
 *    | environment variable space (heap)     |
 *    +---------------------------------------+
 *    | committed/reserved pages for malloc   |
 *    +---------------------------------------+
 *    | stack (grows downwards)               |
 *    +---------------------------------------+
 *    | committed/reserved pages including    |
 *    | the first segment of the default heap |
 *    +---------------------------------------+
 *    | read-only pages (string constants?)   |
 *    +---------------------------------------+
 *    | writable pages (static variables?)    |
 *    +---------------------------------------+
 *    | read-only pages (executable image)    |
 *    +---------------------------------------+ 4Mb
 *    | reserved (for OS use, no access)      |
 *    +---------------------------------------+ 0Mb
 *
 * The 4Mb mark is the lowest valid address for the application while the
 * 2Gb mark is the highest valid address. Pages are usually 4K in size but
 * virtual memory operations act on 64K regions. Under WinNT and Win2000
 * the memory map is similar except that only the bottom 16K and top 16K
 * are reserved by the OS. Pages are also reserved and committed at the
 * upper end of the 2Gb application space for various purposes.
 *
 * There are several ways of allocating memory on a Win32 platform? For C
 * programs ported from other platforms malloc/free will be used. Under NT
 * (and probably Win9x also), this memory is taken from the default heap.
 * Alternative ways of obtaining heap memory are to use the LocalAlloc,
 * GlobalAlloc and HeapAlloc functions. These functions essentially work
 * in the same way at the level of virtual memory but differ in semantics.
 * Finally, virtual memory can be manipulated directly with VirtualAlloc.
 *
 * Each Win32 process has a heap (the default heap) in which individual
 * blocks of dynamic memory are found. Some blocks may be marked as being
 * movable: the caller is given a handle which can be used to later nail
 * the memory in one location so that it can be accessed. If the initial
 * heap region is filled then the heap management functions will obtain
 * more virtual memory and continue the heap there. This means that the
 * default heap may actually consist of several discontiguous regions of
 * virtual memory. Obtaining the allocation base of a piece of heap memory
 * does not provide access to any other region of the heap.
 *
 * In addition to the default heap, NT processes (but not Win9x processes)
 * can create extra heaps which behave just like the default heap.
 *
 * So where does virtual memory fit in? This is the lowest-level of memory
 * management under Win32 and is surprisingly easy to deal with once you
 * understand how it works.
 *
 * The 2Gb virtual address space is split into three main types of region:
 * free, reserved and committed. Free virtual memory pages are those which
 * have not been reserved or committed into physical memory. Any free pages
 * which lie above the lowest valid address for the process can be reserved
 * using VirtualAlloc with the MEM_RESERVE flag. This action simply marks
 * a contiguous region of the virtual address space as reserved for use by
 * the caller (although in principle any other thread in the process could
 * commit these pages without the caller knowing). Reserved pages are still
 * not associated with any physical memory: this requires a commit.
 *
 * Note that VirtualAlloc rounds up reserved the address range so that it is
 * aligned on allocation boundaries (usually 64K). Thus a request to reserve
 * 1K of virtual memory actually reserves 64K.
 *
 * Committing reserved virtual memory is performed by VirtualAlloc using the
 * MEM_COMMIT flag. This causes the virtual memory pages to be associated
 * with physical memory (possibly in swap space on disk). Only whole pages
 * can be committed (usually 4K) and requests are rounded up. Thus committing
 * 1 byte of virtual memory actually commits 4K.
 *
 * If an application does not care where in the virtual address space its
 * memory comes from it can MEM_COMMIT pages giving the base address of 0.
 * Applications which manage their own virtual address space explicitly will
 * usually give a base address of 0 for MEM_RESERVE requests: although this
 * is probably not essential it keeps the virtual address space contiguous.
 *
 * What about heap memory? Well the heap-management functions will reserve
 * a region of virtual memory using VirtualAlloc and commit pages as needed.
 * This means that our memory management code for the garbage collector can
 * scan the entire virtual address space and not worry about the difference
 * between malloc heaps, default heaps or even executable images.
 *
 *
 *                    Design of a OS-Level Memory Manager
 *                    ===================================
 *
 * Aims
 * ====
 *
 * The Aldor memory manager will request 4K blocks of memory via osAlloc.
 * It may release storage via osFree although this is unlikely at present.
 * On entry to the garbage collector, a map of the entire address space will
 * be requested via osMemMap. This map will isolate the stack (if requested)
 * and all accessible (read-write) pages, including those already returned
 * by calls to osAlloc.
 *
 * Since we would like the Aldor heap to be as contiguous as possible we
 * reserve 1Gb of virtual memory for our own use. If this fails we ask for
 * a smaller region until we succeed.
 *
 * Calls to osAlloc will be taken from this region using a simple barrier
 * methodology reminiscent of sbreak. Calls to osFree will turn committed
 * pages into reserved pages but these will not be re-used unless entire
 * 1Gb of reserved space is exhausted. If no reserved pages can be used to
 * satisfy a request then we give up (at present). 1Gb of virtual memory
 * ought to be enough for any Aldor application for the time being!
 */

/*===========================================================================*/

/*
 * Our heap covers virtual memory from [osHeapBase, osHeapBase+osHeapSize)
 * with frontier requests beginning at osHeapFront. The frontier is always
 * within the heap and there are osHeapAvail bytes between the frontier and
 * the end of the heap.
 */
static void *osHeapBase  = (void *)NULL; /* Start of our heap */
static void *osHeapFront = (void *)NULL; /* Frontier for requests */
static ULong osHeapSize  = 0L;           /* Total extent of the heap */
static ULong osHeapAvail = 0L;           /* Amount of unused frontier space */


/* We assume that the OS page size is an integral power of two. */
static ULong osPageSize  = 0L;
static ULong osPageMask  = 0L;


/* We assume that the base of the stack is constant. */
static void *osStackBase = 0L;


/* Assume that the minimum and maximum address is constant. */
static void *osLowTide   = 0L;
static void *osHighTide  = 0L;


/* Local functions. */
static void *osAllocFrontier(ULong);
static void *osFindStackBase(void *);
static void osAllocShowAddressSpace(FILE *);

/*===========================================================================*/

/*
 * osAlloc(&nbytes) attempts to return a pointer to nbytes of memory. On
 * failure NULL is returned; on success the pointer to a block of memory
 * is returned and nbytes is updated to hold the actual size of the block.
 */
Pointer
osAlloc(ULong *pbytes)
{
  static int initialised = 0;
  void *result = (void *)NULL;

  /* Must initialise the allocator first. */
  if (!initialised)
  {
    osInitAlloc();
    initialised = 1;
  }


  /* Don't want to allocate empty blocks of memory. */
  if (!*pbytes) return (Pointer)result;


  /*
   * Try allocating from the frontier. This either succeeds and returns
   * the address of a memory block exactly *pbytes in length which will
   * be committed in the physical memory map (or in swap).
   */
  result = osAllocFrontier(*pbytes);
  if (result) return (Pointer)result;


  /* Our heap is full (very unusual this). */
#if 0
  void *ptr, *end, *next;
  MEMORY_BASIC_INFORMATION vmem_info;

  /*
   * Future work: try to find a reserved block big enough using the first-fit
   * algorithm for speed. If there are no reserved blocks in our heap then the
   * loop body will execute once: our heap is one allocation block containing
   * committed or reserved pages all exec-read-write. Thus VirtualQuery will
   * return the details of each alternating reserved/committed section in the
   * heap: if there are no reserved blocks then it will return details of the
   * whole heap marked as committed.
   *
   * IMPORTANT: this code has been commented out because the current version
   * of osFree does NOT decommit pages. This is because the user might return
   * a pointer to something which is not a whole number of pages. When this
   * occurs we are unable to release the pages because VirtualAlloc works on
   * whole pages (and could release pages containing live data).
   */
  end = (void *)((ULong)osHeapBase + osHeapSize); 
  for (ptr = osHeapBase;!result && ((ULong)ptr < (ULong)end); ptr = next)
  {
    /* Determine the memory state at the current address. */
    if (VirtualQuery(ptr, &vmem_info, sizeof(vmem_info)) != sizeof(vmem_info))
      break; /* Failed */


    /* Compute the address of the next section. */
    next = (void *)((ULong)ptr + vmem_info.RegionSize);


    /* Only interested in reserved pages. */
    if (vmem_info.State != MEM_RESERVE) continue;


    /* Must be big enough to satisfy our request? */
    if (vmem_info.RegionSize < *pbytes) continue;


    /* Success! */
    result = ptr;
    break;
  }


  /* If we succeeded then ... */
  if (result)
  {
    /* Commit the pages. */
#if 0
    result = VirtualAlloc(result, *pbytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
	result = VirtualAlloc(result, *pbytes, MEM_COMMIT, PAGE_READWRITE);
#endif


    /* This might have failed ... */
    if (result)
    {
      /* Compute the size of the allocated region ... */
      if (VirtualQuery(result, &vmem_info, sizeof(vmem_info)) == sizeof(vmem_info))
      {
        /* Success - update the region size. */
        *pbytes = vmem_info.RegionSize;


        /* Return the address of the allocation. */
        return (Pointer)vmem_info.AllocationBase;
      }
    }
  }
#endif

  /*
   * If we reached this point then there were no reserved pages in our heap
   * or we were unable to commit/query the reserved pages. Just ask the OS
   * for any block in memory using a standard heap allocation function. We
   * don't use VirtualAlloc because this would commit whole pages even when
   * the allocation request is for a tiny amount of memory. Assume that the
   * request is satisfied precisely.
   */
  return (Pointer)malloc(*pbytes);
}


/*
 * osFree(ptr) returns memory associated with the pointer "ptr" back to the
 * allocation pool. The pages will be decommitted (removed from the physical
 * memory map).
 *
 * Future work: actually do something!
 *
 * Future work: if "ptr" corresponds to a block at the frontier then we can
 * roll-back the frontier; otherwise we leave the whole in the heap for later.
 */
void
osFree(Pointer p)
{
  /*
   * IMPORTANT: this code is commented out because VirtualFree will release
   * whole pages. If ptr is not an allocation base then the call will fail.
   * If it is an allocation base then all the pages in the region will be
   * released, including any that might still be live. This is the drawback
   * of reserving heap space in one chunk and managing it ourselves.
   */
#if 0
  /* Decommit the pages but do not release them. */
  VirtualFree((void *)p, 0L, MEM_DECOMMIT);
#endif
}


void
osAllocAlignHint(unsigned alignment)
{
}


void
osAllocShow(void)
{
  /* Display the whole address space. */
  osAllocShowAddressSpace(stderr);
}

/*****************************************************************************
 *
 * :: osMemMap
 *
 ****************************************************************************/
#define OS_Has_MemMap

/*
 * WARNING: the address ranges returned by this call are ONLY valid for
 * this moment. After this call returns, the page state and protection
 * of some pages may have been changed! Anyone for Russian Roulette?
 */
struct osMemMap **
osMemMap(int mask)
{
	static struct osMemMap  mmv[MAX_HEAP_SECTIONS];
	static struct osMemMap *mmvp[MAX_HEAP_SECTIONS];

	int i;
	struct osMemMap	*mm = mmv;
	ULong protection;
	void *ptr, *next;
	MEMORY_BASIC_INFORMATION vmem_info;
	void *shi, *slo = &vmem_info;

	/* Ensure that we have initialised our heap system. */
	osInitAlloc();

	/* Ensure that slo <= shi. */
	shi = osStackBase;
	if ((ULong)slo > (ULong)shi)
	{
		void *tmp = slo;
		slo = shi;
		shi = tmp;
	}

	/* Add the stack segment if requested. */
	if (mask & OSMEM_STACK)
	{
		mm->use = OSMEM_STACK;
		mm->lo  = slo;
		mm->hi  = shi;                
		mm++;
	}

	/* Shock-horror - a goto statement! */
	if (!(mask & OSMEM_DDATA))
		goto finishUp;

	/* Scan memory from low-tide upwards looking for writable sections. */
	for (ptr = osLowTide; ptr < osHighTide; ptr = next)
	{

		/*
		 * Ask for details of the page at the specified address. If the page
		 * is not free then we find out about all pages in the same allocation
		 * block with the same attributes.
		 */
		if (VirtualQuery(ptr, &vmem_info, sizeof(vmem_info)) != sizeof(vmem_info))
			break; /* Failed - don't scan any further. */

		/* Remember the address of the next region. */
#if 0
		/*LD*/
		if (1)
			next = (void *)((ULong)vmem_info.BaseAddress + vmem_info.RegionSize);
		else
#endif
			next = (void *)((ULong)ptr + vmem_info.RegionSize);

		/* Skip pages not associated with physical memory. */
		switch (vmem_info.State)
		{
			case MEM_COMMIT:
				/* These pages are okay. */
				break;
			case MEM_FREE: /* FALL THROUGH */
			case MEM_RESERVE: /* FALL THROUGH */
				/* These pages are definitely not okay: try the next region. */
				continue;
		}

#if 0
		switch (vmem_info.Type) {
			case MEM_IMAGE: break;
			case MEM_MAPPED:
					/* These pages might be okay. */
					(void)fprintf(stderr, "      *** found mapped region ");
					(void)fprintf(stderr, "[%p, %p) ", ptr, next);
					break;
			case MEM_PRIVATE:
					/* These pages might be okay. */
					(void)fprintf(stderr, "      *** found private region ");
					(void)fprintf(stderr, "[%p, %p) ", ptr, next);
					break;
#if 0
			default:
					/* Unknown page type: might be okay. */
					(void)fprintf(stderr, "      *** found unknown region ");
					(void)fprintf(stderr, "[%p, %p) ", ptr, next);
					(void)fprintf(stderr, " (state = %08x)\n", vmem_info.State);
					break;
#endif
		}
#endif

		/*
		 * Check the state of the page ignoring guard and cache status. We
		 * want to skip any page that we cannot read from or write to. If we
		 * were paranoid then we only skip pages we cannot read.
		 */
		protection = vmem_info.Protect & ~(PAGE_GUARD | PAGE_NOCACHE);
		switch (protection)
		{
			case PAGE_NOACCESS:
				/* Even if these pages contained pointers, we cannot scan them. */
				continue;
			case PAGE_READONLY:
			case PAGE_EXECUTE:
			case PAGE_EXECUTE_READ:
				/* Probably don't contain pointers: try the next region. */
				continue;
			case PAGE_READWRITE:
			case PAGE_WRITECOPY:
			case PAGE_EXECUTE_READWRITE:
			case PAGE_EXECUTE_WRITECOPY:
			case PAGE_WRITECOMBINE:
				/* Might contain pointers so add region to the root set. */
				break;
			default:
				(void)fprintf(stderr, "   (NASTY) [%p, %p) ", ptr, next);
				(void)fprintf(stderr, "(protect = %08x\n", (unsigned int)protection);
				/* Ignore this region in case it is something nasty. */
				continue;
		}

		if (vmem_info.Protect & PAGE_GUARD) {
			continue;
			/* 
			 * remove the pages protected by PAGE_GUARD.  
			 * Something is wrong in XP SP2.  Address 0x32000
			 */
#if 0
			ULong old_protect;
			if (!VirtualProtect(ptr, vmem_info.RegionSize, protection, &old_protect))
				fprintf(stderr, "Error in memory protection: 0x%lx\n", GetLastError());
#endif
		}

#if 0
		if (ptr == 0x32000) {
			ULong old_protect;
			VirtualProtect(vmem_info.BaseAddress, vmem_info.RegionSize, protection, &old_protect);
		}
#endif

		/*
		 * If this section does not include the stack then we can output it directly
		 * and continue onto the next section. There are two possible scenarios:
		 *
		 * (a) Stack lies before the region:
		 *        +------------+
		 *        |            |
		 *       slo          shi         ptr         next
		 *                                 |           |
		 *                                 +-----------+
		 *
		 * (b) Stack lies after the region:
		 *                                 +-----------+
		 *                                 |           |
		 *       ptr          next        slo         shi
		 *        |            |
		 *        +------------+
		 */
		if (((ULong)shi <= (ULong)ptr) || ((ULong)slo >= (ULong)next))
		{
			mm->use = OSMEM_DDATA;
			mm->lo  = ptr;
			mm->hi  = next;                
			mm++;
			continue;
		}

		/*
		 * This section includes some or all of the stack. We split the region
		 * into at most two additional regions. There are four possible scenarios:
		 *
		 * (a) Stack covers bottom of region:
		 *        +------------------------+
		 *        |                        |
		 *       slo          ptr         shi @@@@@@@@ next
		 *                     |                        |
		 *                     +------------------------+
		 *
		 * (b) Stack lies within region:
		 *                     +-----------+
		 *                     |           |
		 *       ptr ######## slo         shi @@@@@@@@ next
		 *        |                                     |
		 *        +-------------------------------------+
		 *
		 * (c) Stack covers top of region:
		 *                     +------------------------+
		 *                     |                        |
		 *       ptr ######### slo         next         shi
		 *        |                        |
		 *        +------------------------+
		 *
		 * (d) Stack covers whole of region:
		 *        +-------------------------------------+
		 *        |                                     |
		 *       slo          ptr         next         shi
		 *                     |           |
		 *                     +-----------+
		 */
		if ((ULong)ptr < (ULong)slo) /* ### area of cases (b) and (c) */
		{
			mm->use = OSMEM_DDATA;
			mm->lo  = ptr;
			mm->hi  = slo;                
			mm++;
		}
		if ((ULong)shi < (ULong)next) /* @@@ area of cases (a) and (b) */
		{
			mm->use = OSMEM_DDATA;
			mm->lo  = shi;
			mm->hi  = next;                
			mm++;
		}
	}

	/* Target of the jump preceding the above for loop. */
finishUp: { }

	  /* Mark the end of the map. */
	  mm->use = OSMEM_END;

	  /* Create an array of pointers to the map entries. */
	  for (mm = mmv, i= 0; mm->use != OSMEM_END; mm++, i++)
		  mmvp[i] = mm;

	  /* Add the OSMEM_END marker entry. */
	  mmvp[i] = mm;

	  /* Return a pointer the array (which lives in the static area). */
	  return mmvp;
}

/*****************************************************************************
 *
 * :: Win32(s) specific code
 *
 ****************************************************************************/


/*
 * Invoked by osInit and used to prepare the memory manager. We attempt to
 * reserve 1Gb anywhere in virtual memory. If that fails we try to reserve
 * the largest amount of virtual memory possible down to a minimum request
 * of 1Mb. We return zero if successful, -1 if not. On success the statics,
 * osHeapBase, osHeapFront, osHeapSize, osHeapAvail, osPageSize, osPageMask,
 * osStackBase, osLowTide and osHighTide will hold valid values.
 */
static int osInitAlloc(void)
{
  static int initialised = 0;
  void *heap = (void *)NULL;
  ULong want;
  ULong meg = 1L << 20; /* 1Mb */
  ULong gig = 1L << 30; /* 1Gb */
  SYSTEM_INFO sys_info;
  MEMORY_BASIC_INFORMATION vmem_info;

  /* Do nothing if we have already been invoked once. */
  if (initialised) return -1;
  initialised = 1;

  /* Remember the page size and minimum address of this application. */
  GetSystemInfo(&sys_info);
  osPageSize = (ULong)(sys_info.dwPageSize);
  osPageMask = osPageSize - 1;
  osLowTide  = sys_info.lpMinimumApplicationAddress;
  osHighTide = sys_info.lpMaximumApplicationAddress;

  /* Find the base of the stack. */
  osStackBase = osFindStackBase((void *)&want);

  /*
   * Try to find the largest block anywhere in virtual memory. We ask for
   * half of the previous request on failure.
   */
  for (want = gig; !heap && (want > meg); want >>= 1)
#if 0
	heap = VirtualAlloc((void *)NULL, want, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
	heap = VirtualAlloc((void *)NULL, want, MEM_RESERVE, PAGE_READWRITE);
#endif

  /* If we have still failed then give up. */
  if (!heap) return -1;

  /* Compute the size of our heap. */
  if (VirtualQuery(heap, &vmem_info, sizeof(vmem_info)) != sizeof(vmem_info))
    return -1; /* Failed to query heap size. */

  /* Record the details locally. */
  osHeapSize  = osHeapAvail = vmem_info.RegionSize;
  osHeapFront = osHeapBase  = vmem_info.AllocationBase;

  /* Return success. */
  return 0;
}


/*
 * Returns the address of the base of the stack. If the stack grows downwards
 * then this address will be the address of a machine word immediately above
 * the stack top when it is empty. If the stack grows upwards then this will be
 * the address of the machine word of first item on the stack. Returns NULL on
 * failure or the stack base on success. The input argument must be the address
 * of an object already on the stack.
 */
static void *osFindStackBase(void *sp)
{
  MEMORY_BASIC_INFORMATION vmem_info;
  void *sptr = (void *)&vmem_info;

  /*
   * Get information about the region of virtual memory which includes the
   * stack location represented by vmem_info (assumes that all automatic
   * variables such as vmem_info are allocated on the stack).
   */
  if (VirtualQuery(sp, &vmem_info, sizeof(vmem_info)) != sizeof(vmem_info))
    return (void *)NULL; /* Bad news. */

  /* This object must lie in a committed page. */
  if (vmem_info.State != MEM_COMMIT) return (void *)NULL;

  /* What happens next depends on the direction of stack growth. */
  if ((ULong)sp > (ULong)sptr)
  {
    /*
     * Stack grows downwards: find the address of the end of the allocation
     * region. We assume that this marks the end of the stack.
     */
    return (void *)((ULong)vmem_info.BaseAddress + vmem_info.RegionSize);
  }
  else
  {
    /*
     * Stack grows upwards (unusual for Win32). Assume that the allocation
     * base marks the beginning of the stack.
     */
    return (void *)(vmem_info.AllocationBase);
  }
}


/*
 * Invoked by osAlloc to satisfy a request for nbytes of memory starting
 * at the current heap frontier. Returns NULL on failure but returns a
 * virtual memory address on success (never NULL and always above the
 * minimum legal address for the process). If there is not enough space
 * above the frontier we fail - the caller must find space elsewhere. We
 * make no assumptions about frontier alignment or allocation size.
 */
static void *osAllocFrontier(ULong nbytes)
{
  /* Note the current frontier (return value) */
  void *result = osHeapFront;

  /* Is there enough space left in the frontier region? */
  if (nbytes > osHeapAvail) return (void *)NULL;

  /*
   * Commit the requested amount of memory: the frontier address will be
   * rounded down to the next lowest page boundary and will include enough
   * whole pages to encompass the region [osFrontier, osFrontier+nbytes).
   */
#if 0
  if (!VirtualAlloc(osHeapFront, nbytes, MEM_COMMIT, PAGE_EXECUTE_READWRITE))
#else
  if (!VirtualAlloc(osHeapFront, nbytes, MEM_COMMIT, PAGE_READWRITE))
#endif
    return (void *)NULL;

  /* Move the frontier forward. */
  osHeapFront = (void *)((ULong)osHeapFront + nbytes);

  /* Return the address of the block we committed. */
  return result;
}


static void osAllocShowAddressSpace(FILE *outfile)
{
  void *ptr;
  void *maxptr = (void *)(1L << 31);
  ULong start, finish;
  ULong length = 0L;
  char rd, wr, ex, state;
  MEMORY_BASIC_INFORMATION buffer;

  /* Introduction to the address-space-scan section. */
  (void)fprintf(outfile, "Scan of whole 2Gb address space\n");
  (void)fprintf(outfile, "===============================\n\n");

  /* Output a table header. */
  (void)fprintf(outfile, "   %8s:  %8s ", "Address", "Length");
  (void)fprintf(outfile, "%5s %s\n", "State", "Purpose");

  /* Run through every sequence of virtual pages. */
  for (ptr = (void *)NULL; (ULong)ptr < (ULong)maxptr;)
  {
    /* Query virtual memory at the current address. */
    VirtualQuery(ptr, &buffer, sizeof(buffer));

    /* Find out the access permissions of address. */
    switch (buffer.State)
    {
    case MEM_FREE:
      /* AllocationBase, AllocationProtection, Protect and Type are undefined. */
      rd = wr = ex = '*';
      break;
    case MEM_MAPPED:
    case MEM_PRIVATE:
    case MEM_RESERVE:
    case MEM_COMMIT:
      switch (buffer.Protect & ~(PAGE_GUARD | PAGE_NOCACHE))
      {
      case PAGE_NOACCESS:
        rd = '-', wr = '-', ex = '-';
        break;
      case PAGE_READONLY:
        rd = 'r', wr = '-', ex = '-';
        break;
      case PAGE_READWRITE:
        rd = 'r', wr = 'w', ex = '-';
        break;
      case PAGE_WRITECOPY:
        rd = '-', wr = 'w', ex = '-';
        break;
      case PAGE_WRITECOMBINE:
        rd = '-', wr = 'W', ex = '-';
        break;
      case PAGE_EXECUTE:
        rd = '-', wr = '-', ex = 'x';
        break;
      case PAGE_EXECUTE_READ:
        rd = 'r', wr = '-', ex = 'x';
        break;
      case PAGE_EXECUTE_READWRITE:
        rd = 'r', wr = 'w', ex = 'x';
        break;
      case PAGE_EXECUTE_WRITECOPY:
        rd = '-', wr = 'w', ex = 'x';
        break;
      default:
        rd = wr = ex = '!';
        break;
      }
      break;
    default:
      rd = wr = ex = ':';
      state = '?';
    }

    /* Memory state flag. */
    switch (buffer.State)
    {
    case MEM_FREE:    state = '-'; break;
    case MEM_RESERVE: state = 'R'; break;
    case MEM_COMMIT:  state = 'C'; break;
    case MEM_MAPPED:  state = 'M'; break;
    case MEM_PRIVATE: state = 'P'; break;
    default:          state = '?'; break;
    }

    /* Compute details of this region. */
    start  = (ULong)ptr;
    finish = start + buffer.RegionSize;
    length = finish - start;

    /* Display the region. */
    (void)fprintf(outfile, "   %08x: %8luK ", (unsigned int)start, length/1024);
    (void)fprintf(outfile, "%c %c%c%c\n", state, rd, wr, ex);


    /* Move onto the next region. */
    ptr = (void *)finish;
  }
  (void)fprintf(outfile, "\n");
}

