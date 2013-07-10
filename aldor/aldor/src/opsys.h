/*****************************************************************************
 *
 * opsys.h: Portable interface to operating system dependent code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OPSYS_H_
#define _OPSYS_H_

#include "axlport.h"
#include "editlevels.h"

/*****************************************************************************
 *
 * :: Miscellaneous.
 *
 ****************************************************************************/

extern void     osInit              (void);
extern void     osExit              (int);
extern Bool     osIsGUI             (void);
extern void     osDisplayMessage    (char *);

/*****************************************************************************
 *
 * :: License management.
 *
 ****************************************************************************/

extern Bool	osObtainLicense	(void);

/*****************************************************************************
 *
 * :: Running programs.
 *
 ****************************************************************************/

extern int	osRun		(String);
	/*
	 * Run the command given as a string argument.
	 * A return value of 0 indicates success.
	 */

extern int	osRunConcurrent (String, FILE **, FILE **, FILE **);
	/*
	 * If possible, spawn an asynchronous process and communicate with it.
	 * The three FILE** parameters correspond to stdin, stdout and stderr.
	 *
	 * E.g. Suppose foo is run by  osRunConcurrent("foo", &fin, &fout, 0);
	 * Then anything written to fin becomes available to foo on its stdin
	 * and anything which foo writes to stdout can be read from fout.
	 * The last argument, 0, causes foo's stderr to remain uncaptured.
	 *
	 * A return code of 0 indicates success spawning.  Failure gives -1.
	 * To determine whether the platform supports concurrent processes,
	 * call osRunConcurrent with a NULL command string.
	 */

extern int	osRunQuoteArg	(String word, int (*putter)(int));
	/*
	 * Quote "word" to be a suitable argument in an osRun command.
	 * The characters of the result are delivered one by one to "putter".
	 * The return value is number of characters delivered.
	 * E.g: On Unix the string >$1 is converted to '>$1' and 5 is returned.
	 */

/*****************************************************************************
 *
 * :: Time and Date.
 *
 ****************************************************************************/

extern Millisec osCpuTime	(void);
	/*
	 * The time accounted to this process, in milliseconds.
	 * On systems where it counts, this is user + system time.
	 */

extern String	osDate		(void);
	/*
	 * Static area containing a human-readable date.
	 * The exact format is system dependent.
	 */


/*****************************************************************************
 *
 * :: Environment variables.
 *
 ****************************************************************************/

extern String	osGetEnv	(String name);
extern int	osPutEnv	(String eqn);
extern Bool	osPutEnvIsKept	(void);
	/*
	 * osGetEnv("FOO")
	 *	Gets the value for the name "FOO" from the environment.  
	 *	  The resulting string is owned by the environment
	 *        and LATER CALLS MAY MODIFY IT and YOU MUST NOT.
	 *	
	 * osPutEnv("FOO=xxx")
	 *	Sets the string associated with the name "FOO".
	 *	  The string might be modified by the osPutEnv call.
	 *	  A result of -1 indicates failure,  0 success.
	 *
	 * osPutEnvIsKept()
	 *	Returns true iff a call to osPutEnv causes its argument
	 *	  to become owned by the environment. 
	 *	  If so, a program should not then update or free it.
	 */

/*****************************************************************************
 *
 * :: Fix command line for standalone ports.
 *
 ****************************************************************************/

extern        int osFixCmdLine(int *pargc, char ***pargv);

      /*
       *      Obtain or accept command line arguments
       */


/*****************************************************************************
 *
 * :: File system I/O Modes.
 *
 ****************************************************************************/

extern IOMode	osIoRdMode;
extern IOMode	osIoWrMode;
extern IOMode	osIoApMode;
extern IOMode	osIoRbMode;
extern IOMode	osIoWbMode;
extern IOMode	osIoAbMode;
extern IOMode	osIoRubMode;
extern IOMode	osIoWubMode;
extern IOMode	osIoAubMode;

/*****************************************************************************
 *
 * :: File I/O.
 *
 ****************************************************************************/

extern int  osGetc      (FILE *);
extern int  osPutc      (FILE *, int);
extern int  osFPrintf   (FILE *, char *, ...);
extern int  osFEof      (FILE *);

/*****************************************************************************
 *
 * :: File name manipulation.
 *
 ****************************************************************************/

extern String	osObjectFileType;
extern String	osExecFileType;

extern String	osCurDirName	(void);
extern String   osTmpDirName	(void);
	/*
	 * These don't change as the program executes and moves around.
	 */

extern Bool	osFnameDirEqual (String d1, String d2);
extern void	osSubdir	(String buf,String relativeTo, String subdir);
extern Length	osSubdirLength	(String relativeTo, String subdir);
	/*
	 * osFnameDirEqual(d1, d2)
	 *	Determine whether d1 and d2 are equal as directories.
	 *	For example on Unix "foo/." is considered equal to "foo".
	 *
	 * osSubdir(buf, relativeTo, subdir)
	 *	Place the name of a subdirectory of "relativeTo".
	 *	This may or may not be disctinct from the original.
	 *	E.g. on DOS   osSubdir("c:\foo", "bar") would be "c:\foo\bar"
	 *	while on CMS  osSubDir("A", "lib") would be "A".
	 *
	 * osSubdirLength(relativeTo, subdir)
	 *	Determine the number of characters needed for the
	 *	subdirectory name.
	 */

extern Length	osFnameNParts;

extern void	osFnameParse	(String* partv, String buffer,
				 String fname, String relativeTo);
extern Length	osFnameParseSize  (String fname, String relativeTo);

extern String	osFnameUnparse	  (String buffer,
			           String *partv, Bool full);
extern Length	osFnameUnparseSize(String *partv, Bool full);
	/*
	 * osFnameNParts;
	 *	is the number of parts file names have on this platform.
	 *	This number is >= 3 and the meaning of parts 0..2 are fixed:
	 *	partv[0] = dir, partv[1] = name, partv[2] = type.
	 *	On some platforms additional parts, such as version numbers or
	 *	host names, may be part of the name.
	 *
	 * osFnameParse(partv, buffer, fname, relativeTo)
	 *	The argument partv must have osFnameNParts() slots.
	 *      The size required for buffer can be found by osFnameParseSize.
	 *	The argument relativeTo may be 0, which means either that the
	 *	name is relative to the current directory or that the name
	 *	is absolute.
	 *
	 * osFnameParseSize(fname, relativeto) determines how many characters
	 *	are needed for the buffer to parse a file name (including
	 *	trailing 0s etc).
	 *
	 * osFnameUnparse(b,pv,f) fills the buffer b with a file name
	 *	having the operating system's syntax.  If f is true, then
	 *	the full name is used, instead of using current directory
	 *	conventions.
	 *
	 * osFnameUnparseSize(pv, f) determines how many characters are needed
	 *	to unparse a file name (including trailing 0 etc).
	 */

extern int	osFnameTempSeed	(void);
extern String	osFnameTempDir	(String relativeTo);
	/*
	 * osFnameTempSeed() returns a process seed for contructing
	 *	temporary file names.
	 *
	 * osFnameTempDir(r) returns a directory name for temporary files
	 *	relative to r.
	 */

/*****************************************************************************
 *
 * :: File system interaction.
 *
 ****************************************************************************/

extern Bool	osIsInteractive (FILE *);

extern int	osFileRemove    (String fn);
extern int	osFileRename    (String from, String to);
extern Bool	osFileIsThere   (String fn);
extern Hash	osFileHash      (String fn);
extern Length	osFileSize      (String fn);

extern Bool	osDirIsThere    (String dn);
extern int	osDirSwap	(String newwd, String oldwd, Length oldwdlen);
	/*
	 * osIsInteractive determines whether an input stream is interactive,
	 *   e.g. a keyboard.
	 *
	 * osFileRemove and osFileRename return 0 on success, -1 on error.
	 * osFileIsThere tests whether the file exists.
	 *
	 * osDirIsThere tests whether the directory exists.
	 *
	 * osDirSwap changes the current directory.  
	 *   If the pointer oldwd is non-null, then string is filled with
	 *   the name of the original directory.  This name can then be used to
	 *   swap back or to form file names relative to the original directory.
	 *   Returns 0 on success and -1 on failure.
	 */

/*****************************************************************************
 *
 * :: Directory path manipulation.
 *
 ****************************************************************************/

extern String	osIncludePath	(void);
extern String	osLibraryPath	(void);
extern String	osExecutePath	(void);
	/*
	 * osIncludePath() returns a list of directories searched for
	 *   include files, THE RESULT MAY BE VOLATILE.
	 *
	 * osLibraryPath() returns a list of directories searched for
	 *   object files, THE RESULT MAY BE VOLATILE.
	 *
	 * osExecutePath() returns a list of directories searched for
	 *   executable programs, THE RESULT MAY BE VOLATILE.
	 */

extern char     osPathSeparator (void);
extern Length	osPathLength	(String path);
extern void	osPathParse	(String* partv, String buffer, String path);
	/*
	 * osPathSeparator(path) returns the character that separates the 
	 *	directories in the path.
	 * osPathLength(path) returns the number of directory names in path.
	 *
	 * osPathParse(partv, buffer, path) splits path into its constituent
	 *	directories, using space from buffer, and placing the String
	 *	pointers in partv.
	 */

/*****************************************************************************
 *
 * :: Signals.
 *
 ****************************************************************************/

typedef void SignalModifier (*OsSignalHandler) (int signal);

extern OsSignalHandler	osSetBreakHandler(OsSignalHandler);
extern OsSignalHandler	osSetFaultHandler(OsSignalHandler);
extern OsSignalHandler	osSetLimitHandler(OsSignalHandler);
extern OsSignalHandler	osSetDangerHandler(OsSignalHandler);

	/*
	 * osSetBreakHandler sets the handler for interactive interrupts,
	 *   e.g. user break generated from terminal.
	 *
	 * osSetFaultHandler sets the handler for faults which normally cause
	 *   the program to terminate, e.g. illegal instruction or segmentation
	 *   fault.
	 *
	 * osSetLimitHandler sets the handler for interrupts which arise when
	 *   resource limits are exceeded, e.g. cpu or file size limits.
	 *
	 * In each case, the default handler can be re-instated by passing 0.
	 * The previous handler (or 0 for default) is returned.
	 */

/*****************************************************************************
 *
 * :: Memory management.
 *
 ****************************************************************************/

extern Pointer	osAlloc		(ULong *pnbytes);
extern void	osFree		(Pointer p);
extern void	osAllocAlignHint(unsigned);
extern void	osAllocShow	(void);
	/*
	 * osAlloc gets more dynamic storage from the operating system.
	 *    A pointer to the low address of the new store is returned.
	 *    This pointer is aligned to have byte alignment = 1<<alignshift.
	 *    The value *pnbytes is updated to be the number actually gotten.
	 *    On failure, a null pointer and byte count of zero are returned.
	 *
	 * osFree returns store obtained from osAlloc.
	 *
	 * osAllocAlignHint provides the low-level allocation code a hint
	 *    about the desired alignment.  Subsequent allocations will be
	 *    aligned to multiples of this quantity, if convenient.
	 *
	 * osAllocShow, in some implementations, provides detailed information
	 *    about the low-level low level storage use.  For implementations
	 *    where this information is not available, the call does nothing.
	 */

struct osMemMap {
        UAInt	use;
	Pointer lo, hi;
};

extern struct osMemMap ** osMemMap(int mask);
	/*
	 * osMemMap returns an array of pointers to osMemMap structs
	 * containing a map of memory at the time it was called.
	 * The array is terminated by an element with use == OSMEM_END.
	 * Each element of the array, e, is a structure indicating that the
	 *   address range [e.lo, e.hi) of used for e.use.
	 * On some platforms it is not possible to distinguish the classes
	 *   IDATA/DDATA.  In these casee it is all counted as DDATA.
	 * The argument to osMemMap is a mask indicating the uses to report.
	 * If the information is not available, the pointer 0 is returned.
	 */
#	define OSMEM_IDATA	0x01 /* initial static data  */
#	define OSMEM_DDATA	0x02 /* dynamic data, e.g. from sbrk malloc */
#	define OSMEM_STACK	0x04 /* call-stack   */
#	define OSMEM_END	0x00 /* end */


/*****************************************************************************
 *
 * :: osStreams
 *
 ****************************************************************************/

extern FILE * osStdout;
extern FILE * osStdin;
extern FILE * osStderr;


/*****************************************************************************
 *
 * :: osRandom
 *
 ****************************************************************************/

extern Length osRandom(void);

#endif	/* !_OPSYS_H_ */


