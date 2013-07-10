-----------------------------------------------------------------------------
----
---- oslow.as: Lower-level portable interface to operating system function.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

CChar      ==> SingleInteger;		-- In C: int
CInt       ==> SingleInteger;		-- In C: int
CUnsigned  ==> SingleInteger;		-- In C: unsigned
CULong     ==> SingleInteger;		-- In C: unsigned long
CULongRef  ==> Record(val: CULong);	-- In C: unsinged long *
CFile      ==> Pointer;			-- In C: FILE *
CFileRef   ==> Record(val: CFile);	-- In C: FILE **
CFun(A,R)  ==> Pointer;			-- In C: function pointer
StringArr  ==> PrimitiveArray String; 	-- In C: char **

Millisec   ==> CInt;
Length     ==> CInt;
Hash       ==> CInt;
IOMode     ==> String;
SigHandler ==> CFun(CInt, ());
MemMap     ==> PrimitiveArray Pointer;


+++ `OperatingSystemInterface' provides a portable interface to 
+++ functions provided by the operating system.  
+++ It is low-level in that none of the operations return dynamically
+++ allocated objects, with the exception of `alloc'.

OperatingSystemInterface: with {

	---------------------------------------------------------------------
	--
	-- Running Programs
	--
	---------------------------------------------------------------------
	run: String -> CInt;
		++ Run the command given as a string argument.
		++ A return value of 0 indicates success.

	run: (String, CFileRef, CFileRef, CFileRef) -> CInt;
		++ If possible, spawn an asynchronous process and communicate 
		++ with it.  The three CFileRef parameters correspond to stdin,
		++ stdout and stderr.  E.g. Suppose foo is run by: 
		++
		++   run("foo", fin, fout, nil pretend CFileRef);
		++
		++ Then anything written to fin becomes available to foo
		++ on its stdin and anything which foo writes to stdout can
		++ be read from fout. The last argument, 0, causes foo's stderr
		++ to remain uncaptured.
		++
		++ A return code of 0 indicates success spawning.  Failure
		++ gives -1.
		++ To determine whether the platform supports concurrent 
		++ processes, call osRunConcurrent with a NULL command string.

        canRunConcurrent?: () -> Boolean;
		++ canRunConcurrent?() returns true if the operating system supports 
		++ concurrent processes (used by operation run).

	runQuoteWord: (String, CFun(CInt, CInt)) -> CInt;
		++ Quote a string to be a suitable word in a string to `run'. 
		++ The characters of the result are delivered one by one to
		++ the functional argument.
		++ The return value is number of characters delivered.
		++ E.g: On Unix the string >$1 is converted to '>$1' and 5 
		++ is returned.

	---------------------------------------------------------------------
	--
	-- Time and Date
	--
	---------------------------------------------------------------------
	cpuTime: () -> Millisec;
		++ The time accounted to this process, in milliseconds.
		++ On systems where it counts, this is user + system time.

	date: () -> String;
		++ Static area containing a human-readable date.
		++ The exact format is system dependent.

	---------------------------------------------------------------------
	--
	-- Environment Variables
	--
	---------------------------------------------------------------------
	getenv: String -> String;
		++ `getenv("FOO")' gets the value for the name "FOO"
		++ from the environment.  The resulting string is owned
		++ by the environment and LATER CALLS MAY MODIFY IT
		++ and YOU MUST NOT.

	putenv: String -> CInt;
		++ `putenv("FOO=xxx")' sets the string associated with the name
		++ "FOO". The string might be modified by the `putenv' call.
		++ A result of -1 indicates failure,  0 success.

	putenvIsKept?: () -> Boolean;
		++ putenvIsKept?()
		++ Returns true iff a call to putenv causes its argument
		++ to become owned by the environment. 
		++ If so, a program should not then update or free it.

	---------------------------------------------------------------------
	--
	-- File System I/O Modes
	--
	---------------------------------------------------------------------
	ioRdMode:  IOMode;
	ioWrMode:  IOMode;
	ioApMode:  IOMode;
	ioRbMode:  IOMode;
	ioWbMode:  IOMode;
	ioAbMode:  IOMode;
	ioRubMode: IOMode;
	ioWubMode: IOMode;
	ioAubMode: IOMode;

	---------------------------------------------------------------------
	--
	-- File Name Manipulation
	--
	---------------------------------------------------------------------
	objectFileType: String;
	execFileType:   String;

	curDirName: () -> String;
		++ Doesn't change as the program executes and moves around.

	tmpDirName: () -> String;
		++ Doesn't change as the program executes and moves around.

	fnameDirEqual: (String, String) -> Boolean;
		++`fnameDirEqual(d1, d2)'
		++ Determines whether d1 and d2 are equal as directories.
		++ For example on Unix "foo/." is considered equal to "foo".

	subdir: (String, String, String) -> ();
		++`subdir(buf, relativeTo, subdir)'
		++ Place the name of a subdirectory of "relativeTo".
		++ This may or may not be disctinct from the original.
		++ E.g. on DOS  subdir("c:\foo","bar") would be "c:\foo\bar"
		++ while on CMS subDir("A", "lib") would be "A".

	subdirLength: (String, String) -> Length;
		++`subdirLength(relativeTo, subdir)'
		++ Determine the number of characters needed for the
		++ subdirectory name.

	fnameNParts: Length;
		++ fnameNParts;
		++ is the number of parts file names have on this platform.
		++ This number is >= 3 and the meaning of parts 1..3 are fixed:
		++ partv.1 = dir, partv.2 = name, partv.3 = type.
		++ On some platforms additional parts, such as version numbers
		++ or host names, may be part of the name.
	
	fnameParse: (StringArr, String, String, String) -> ();
		++ `fnameParse(partv, buffer, fname, relativeTo)'
		++ The argument partv must have fnameNParts() slots.
		++ The size required for buffer can be found by fnameParseSize.
		++ The argument relativeTo may be 0, which means either that
		++ the name is relative to the current directory or that the
		++ name is absolute.

	fnameParseSize: (String, String) -> Length;
		++ `fnameParseSize(fname, relativeto)' determines how many
		++ characters are needed for the buffer to parse a file name
		++ (including trailing 0s etc).

	fnameUnparse: (String, StringArr, Boolean) -> String;
		++ `fnameUnparse(b,pv,f)' fills the buffer b with a file name
		++ having the operating system's syntax.  If f is true, then
		++ the full name is used, instead of using current directory
		++ conventions.

	fnameUnparseSize: (StringArr, Boolean) -> Length;
		++ `fnameUnparseSize(pv, f)' determines how many characters are
		++ needed to unparse a file name (including trailing 0 etc).
  		++ If f is true, then the full name is used, instead of using
		++ current directory conventions.

	fnameTempSeed: () -> CInt;
		++ `fnameTempSeed()' returns a process seed for contructing
		++ temporary file names.

	fnameTempDir: String -> String;
		++ `fnameTempDir(r)' returns a directory name for temporary
		++ files relative to r.

	---------------------------------------------------------------------
	--
	-- File System Interaction
	--
	---------------------------------------------------------------------
	interactive?: CFile -> Boolean;
		++ `interactive?' determines whether an input stream is
		++ interactive, e.g. a keyboard.

	fileRemove: String -> CInt;
		++ `fileRemove' and fileRename return 0 on success, -1 on
		++ error fileExists? tests whether the file exists.

	fileRename: (String, String) -> CInt;
	fileExists?: String -> Boolean;
	fileHash:    String -> Hash;
	fileSize:    String -> Length;

	dirExists?: String -> Boolean;
		++ `dirExists?' tests whether the directory exists.

	dirSwap: (String, String, Length) -> CInt;
		++ `dirSwap' changes the current directory.  
		++ If the pointer oldwd is non-null, then string is filled with
		++ the name of the original directory.  This name can then be
		++ used to swap back or to form file names relative to the
		++ original directory.
		++ Returns 0 on success and -1 on failure.

	---------------------------------------------------------------------
	--
	-- Directory Path Manipulation
	--
	---------------------------------------------------------------------
	includePath: () -> String;
		++ `includePath()' returns a list of directories searched for
		++ include files, THE RESULT MAY BE VOLATILE.
	libraryPath: () -> String;
		++ `libraryPath()' returns a list of directories searched for
		++ object files, THE RESULT MAY BE VOLATILE.
	executePath: () -> String;
		++ `executePath()' returns a list of directories searched for
		++ executable programs, THE RESULT MAY BE VOLATILE.

	pathLength: String -> Length;
		++ `pathLength(path)' returns the number of directory names in
		++ path.

	pathParse: (StringArr, String, String) -> ();
		++ `pathParse(partv, buffer, path)' splits path into its
		++ constituent directories, using space from buffer, and
		++ placing the String pointers in partv.
	
	---------------------------------------------------------------------
	--
	-- Signals
	--
	---------------------------------------------------------------------
	setBreakHandler: SigHandler -> SigHandler;
		++ `setBreakHandler' sets the handler for interactive
		++ interrupts, e.g. user break generated from terminal.

	setFaultHandler: SigHandler -> SigHandler;
		++ `setFaultHandler' sets the handler for faults which
		++ normally cause the program to terminate, e.g. illegal 
		++ instruction or segmentation fault.

	setLimitHandler: SigHandler -> SigHandler;
		++ `setLimitHandler' sets the handler for interrupts which
		++ arise when resource limits are exceeded, e.g. cpu or file
		++ size limits.  In each case, the default handler can be
		++ re-instated by passing a null function pointer..
		++ The previous handler (or null for default) is returned.

	setDangerHandler: SigHandler -> SigHandler;

	---------------------------------------------------------------------
	--
	-- Memory Management
	--
	---------------------------------------------------------------------
	alloc: CULongRef -> Pointer;
		++ `alloc' gets more dynamic storage from the operating system.
		++ This function should called only occasionally to get large
		++ pieces of memory. The return value is an appropriately
		++ aligned pointer to the low address of the new store.
		++ The argument is updated to contain the number of bytes
		++ actually obtained.  On failure, a null pointer and byte
		++ count of zero are returned.

	release: Pointer -> ();
		++ `release' gives back store obtained from `alloc'.

	allocAlignHint: CUnsigned -> ();
		++ `allocAlignHint' provides the low-level allocation code a
		++ hint about the desired alignment.  Subsequent allocations
		++ will be aligned to multiples of this quantity, if
		++ convenient.

	allocShow: () -> ();
		++ `allocShow', in some implementations, provides detailed
		++ information about the low-level low level storage use.  
		++ For implementations where this information is not 
		++ available, the call does nothing.

	memMap: CInt -> MemMap;
		++ `memMap' returns a pointer to a static array containing a
		++  map of memory at the time it was called.
		++ The array is terminated by an element with use == OSMEM_END.
		++ Each element of the array, e, is a structure indicating
		++ that the address range [e.lo, e.hi) of used for e.use.
		++ On some platforms it is not possible to distinguish the
		++ classes IDATA/DDATA.  In these casee it is all counted as
		++ DDATA.
		++ The argument to osMemMap is a mask indicating the uses to
		++ report. If the information is not available, the pointer 0
		++ is returned.
		++
		++ OSMEM_IDATA	1 -- initial static data.
		++ OSMEM_DDATA	2 -- dynamic data, e.g. from sbrk malloc.
		++ OSMEM_STACK	4 -- call-stack.
		++ OSMEM_END	0 -- end.
} 
== add {
	import {
		osRun: String -> CInt;
		osRunConcurrent: (String, CFileRef, CFileRef, CFileRef)-> CInt;
		osRunQuoteArg: (String, CFun(CInt, CInt)) -> CInt;

		osCpuTime: () -> Millisec;
		osDate: () -> String;

		osGetEnv: String -> String;
		osPutEnv: String -> CInt;
		osPutEnvIsKept: () -> Boolean;

		osIoRdMode: IOMode;
		osIoWrMode: IOMode;
		osIoApMode: IOMode;
		osIoRbMode: IOMode;
		osIoWbMode: IOMode;
		osIoAbMode: IOMode;
		osIoRubMode: IOMode;
		osIoWubMode: IOMode;
		osIoAubMode: IOMode;

		osObjectFileType: String;
		osExecFileType: String;

		osCurDirName: () -> String;
		osTmpDirName: () -> String;

		osFnameDirEqual: (String, String) -> Boolean;
		osSubdir: (String, String, String) -> ();
		osSubdirLength: (String, String) -> Length;

		osFnameNParts: Length;
	
		osFnameParse: (StringArr, String, String, String) -> ();
		osFnameParseSize: (String, String) -> Length;
		osFnameUnparse: (String, StringArr, Boolean) -> String;
		osFnameUnparseSize: (StringArr, Boolean) -> Length;

		osFnameTempSeed: () -> CInt;
		osFnameTempDir: String -> String;

		osIsInteractive: CFile -> Boolean;

		osFileRemove: String -> CInt;
		osFileRename: (String, String) -> CInt;
		osFileIsThere: String -> Boolean;
		osFileHash: String -> Hash;
		osFileSize: String -> Length;

		osDirIsThere: String -> Boolean;
		osDirSwap: (String, String, Length) -> CInt;

		osIncludePath: () -> String;
		osLibraryPath: () -> String;
		osExecutePath: () -> String;

		osPathLength: String -> Length;
		osPathParse: (StringArr, String, String) -> ();

		osSetBreakHandler: SigHandler -> SigHandler;
		osSetFaultHandler: SigHandler -> SigHandler;
		osSetLimitHandler: SigHandler -> SigHandler;
		osSetDangerHandler: SigHandler -> SigHandler;
	
		osAlloc: CULongRef -> Pointer;
		osFree:  Pointer -> ();
		osAllocAlignHint: CUnsigned -> ();
		osAllocShow: () -> ();

		osMemMap: CInt -> MemMap;

	} from Foreign C;


	run(s: String): CInt ==
		osRun(s);
	run(s:String, f1:CFileRef, f2:CFileRef, f3:CFileRef):CInt == {
		#s = 0 => -1;
		osRunConcurrent(s, f1, f2, f3);
	}
	runQuoteWord(s: String, f: CFun(CInt, CInt)):CInt == 
		osRunQuoteArg(s, f);
	canRunConcurrent?():Boolean == {
		import from CInt, Pointer;
		osRunConcurrent(nil pretend String, 
				nil pretend CFileRef,
				nil pretend CFileRef,
				nil pretend CFileRef) = 0;
	}
	cpuTime(): Millisec ==
		osCpuTime();
	date(): String ==
		osDate();

	getenv(s: String): String ==
		osGetEnv(s);
	putenv(s:String): CInt ==
		osPutEnv(s);
	putenvIsKept?(): Boolean ==
		osPutEnvIsKept();

	ioRdMode:  IOMode == osIoRdMode;
	ioWrMode:  IOMode == osIoWrMode;
	ioApMode:  IOMode == osIoApMode;
	ioRbMode:  IOMode == osIoRbMode;
	ioWbMode:  IOMode == osIoWbMode;
	ioAbMode:  IOMode == osIoAbMode;
	ioRubMode: IOMode == osIoRubMode;
	ioWubMode: IOMode == osIoWubMode;
	ioAubMode: IOMode == osIoAubMode;

	objectFileType: String ==
		osObjectFileType;
	execFileType: String ==
		osExecFileType;
	curDirName(): String ==
		osCurDirName();
	tmpDirName(): String ==
		osTmpDirName();

	fnameDirEqual(s1: String, s2: String): Boolean == 
		osFnameDirEqual(s1, s2);
	subdir(s1: String, s2: String, s3: String):() ==
		osSubdir(s1, s2, s3);
	subdirLength(s1: String, s2: String): Length ==
		osSubdirLength(s1, s2);
	fnameNParts: Length ==
		osFnameNParts;
	fnameParse(sr: StringArr, s1: String, s2: String, s3: String):() ==
		osFnameParse(sr, s1, s2, s3);
	fnameParseSize(s1: String, s2: String): Length ==
		osFnameParseSize(s1, s2);
	fnameUnparse(s1: String, sr: StringArr, b: Boolean): String ==
		osFnameUnparse(s1, sr, b);
	fnameUnparseSize(sr: StringArr, b: Boolean): Length ==
		osFnameUnparseSize(sr, b);
	fnameTempSeed(): CInt ==
		osFnameTempSeed();
	fnameTempDir(s: String): String ==
		osFnameTempDir(s);
	interactive?(cf: CFile): Boolean ==
		osIsInteractive(cf);
	fileRemove(s: String): CInt ==
		osFileRemove(s);
	fileRename(s1: String, s2: String): CInt ==
		osFileRename(s1,s2);
	fileExists?(s: String): Boolean ==
		osFileIsThere(s);
	fileHash(s: String): Hash ==
		osFileHash(s);
	fileSize(s: String): Length ==
		osFileSize(s);
	dirExists?(s: String): Boolean ==
		osDirIsThere(s);
	dirSwap(s1: String, s2: String, l: Length): CInt ==
		osDirSwap(s1, s2, l);

	includePath(): String ==
		osIncludePath();
	libraryPath(): String ==
		osLibraryPath();
	executePath(): String ==
		osExecutePath();
	pathLength(s: String): Length ==
		osPathLength(s);
	pathParse(sr: StringArr, s1: String, s2: String): () ==
		osPathParse(sr, s1, s2);

	setBreakHandler(sh: SigHandler): SigHandler == 
		osSetBreakHandler(sh);
	setFaultHandler(sh: SigHandler): SigHandler ==
		osSetFaultHandler(sh);
	setLimitHandler(sh: SigHandler): SigHandler ==
		osSetLimitHandler(sh);
	setDangerHandler(sh: SigHandler): SigHandler ==
		osSetDangerHandler(sh);

	alloc(u: CULongRef): Pointer ==
		osAlloc(u);
	release(p: Pointer): () ==
		osFree(p);
	allocAlignHint(u: CUnsigned): () ==
		osAllocAlignHint(u);
	allocShow(): () ==
		osAllocShow();
	memMap(i: CInt): MemMap ==
		osMemMap(i);
}
