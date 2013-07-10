/******************************************************************************
 *
 * testaldor.h
 *
 * Test driver for the Aldor Compiler.
 *
 *****************************************************************************/

#ifndef _TESTALDOR_H_
#define _TESTALDOR_H_

/******************************************************************************
 *
 * Declarations
 *
 *****************************************************************************/

typedef char * 		String;
typedef unsigned long	Length;
typedef long		Int;

#define	fnewline(s)	fputc('\n', s)
#define	exitFailure()	exit(1)
#define exitSuccess()	exit(0)

#if defined(CC_no_prototype)
#define	_OF(X)	()
#else
#define	_OF(X)	X
#endif

/******************************************************************************
 *
 * String utilities
 *
 *****************************************************************************/

String		strAlloc		_OF((Length));
Length		strLength		_OF((String));
String		strCopy			_OF((String));
String		strConcat		_OF((String, String));
String		strPrintf		_OF((String, ...));
Int		strEqual		_OF((String, String));
Int		strIsPrefix		_OF((String, String));
Int		strIsSuffix		_OF((String, String));

/******************************************************************************
 *
 * OS-level utilities
 *
 *****************************************************************************/

Int		osRun			_OF((String cmd));
	/*
	 * osRun(cmd)
	 *	Execute the command from the operating system.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osRunOutput		_OF((String cmd, String fout));
	/*
	 * osRunOutput(cmd)
	 *	Execute the command from the operating system.
	 *	Direct the output from the command to the file fout.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osRunScript		_OF((String cmd, String fout));
	/*
	 * osRunScript(cmd, fout)
	 *	Execute the command script from the operating system.
	 *	Direct the output from the command to the file fout.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osShowDiff		_OF((String src, String dest));
	/*
	 * osShowDiff(src, dest)
	 *	Show the differences between the named files.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osPutEnv		_OF((String eqn));
	/*
	 * osPutEnv(eqn)
	 *	Add the assignment given by eqn to the current environment.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osGetCurDir		_OF((String fn, Length cc));
	/*
	 * osGetCurDir(fn, cc)
	 *	Place the name of the current working directory in fn.
	 *	cc is the length of the destination buffer fn.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osSetCurDir		_OF((String fn));
	/*
	 * osSetCurDir(fn)
	 *	Change the current working directory to fn.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osMakeDir		_OF((String fn));
	/*
	 * osMakeDir(fn)
	 *	Created the named directory.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osRemoveDir		_OF((String fn));
	/*
	 * osRemoveDir(fn)
	 *	Remove the named directory.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osFileIsThere		_OF((String fn));
	/*
	 * osFileIsThere(fn)
	 *	Check for the presence of the named file.
	 *	Return 1 if the file is found, 0 otherwise.
	 */
Int		osFileCat		_OF((String fn));
	/*
	 * osFileCat(fn)
	 *	Print the contents of the named file on stdout.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osFileCopy		_OF((String src, String dest));
	/*
	 * osFileCat(src, dest)
	 *	Copy the named file src to dest.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osFileEqual		_OF((String src, String dest));
	/*
	 * osFileEqual(src, dest)
	 *	Return 1 if the contents of src and dest are the same.
	 *	Return 0 otherwise.
	 */
Int		osFileRemove		_OF((String fn));
	/*
	 * osFileRemove(fn)
	 *	Remove the named file.
	 *	Return 0 on success and -1 on failure.
	 */
String		osFileBase		_OF((String fn));
	/*
	 * osFileBase(fn)
	 *	Return the base name of the named file.
	 */
String		osFileCombine		_OF((String dir, String fn));
	/*
	 * osFileCombine(dir, fn)
	 *	Return the file name of a file named fn in the directory dir.
	 */
Int		osFnameTempSeed		_OF((void));
	/*
	 * osFnameTempSeed()
	 * 	Return a unique id for use in constructing temporary names.
	 */
String		osTempDirName		_OF((void));
	/*
	 * osTempDirName()
	 *	Return the name of a unique temporary directory.
	 */

#endif	/* !_TESTALDOR_H_ */
