/******************************************************************************
 *
 * tx_opsys.c
 *
 * Generic versions of the OS-level utilities for testaldor.
 *
 *****************************************************************************/

/*
 * Select OS-specific file.
 */
#if defined(OS_UNIX)
#include "tx_unix.c"
#endif

#if defined(OS_WIN32)
#include "tx_win32.c"
#endif

/******************************************************************************
 *
 * :: osRun
 *
 *****************************************************************************/

#if !defined(OS_Has_Run)

Int
osRun(cmd)
	String	cmd;
{
	fflush(stdout);
	fflush(stderr);
	return system(cmd);
}

#endif	/* ! OS_Has_Run */

/******************************************************************************
 *
 * :: osRunOutput
 *
 *****************************************************************************/

#if !defined(OS_Has_RunOutput)

Int
osRunOutput(cmd, fout)
	String	cmd;
	String	fout;
{
	if (!fout) fout = "/dev/null";
	return osRun(strPrintf(OSFmtOutput, cmd, fout));
}

#endif	/* ! OS_Has_RunScript */

/******************************************************************************
 *
 * :: osRunScript
 *
 *****************************************************************************/

#if !defined(OS_Has_RunScript)

Int
osRunScript(cmd, fout)
	String	cmd;
	String	fout;
{
	return osRunOutput(strPrintf(OSFmtScript, cmd), fout);
}

#endif	/* ! OS_Has_RunScript */

/******************************************************************************
 *
 * :: osShowDiff
 *
 *****************************************************************************/

#if !defined(OS_Has_ShowDiff)

Int
osShowDiff(src, dest)
	String	src;
	String	dest;
{
	return osRun(strPrintf(OSFmtDiff, src, dest));
}

#endif	/* ! OS_Has_ShowDiff */

/******************************************************************************
 *
 * :: osPutEnv
 *
 *****************************************************************************/

#if !defined(OS_Has_PutEnv)

extern int	putenv			(/* char * eqn */);

Int
osPutEnv(eqn)
	String	eqn;
{
	return putenv(eqn);
}

#endif	/* ! OS_Has_PutEnv */

/******************************************************************************
 *
 * :: osGetCurDir
 *
 *****************************************************************************/

#if !defined(OS_Has_GetCurDir)

extern char *	getcwd			(/* char * buf, int size */);

Int
osGetCurDir(fn, cc)
	String	fn;
	Length	cc;
{
	return getcwd(fn, cc) ? 0 : -1;
}

#endif	/* ! OS_Has_GetCurDir */

/******************************************************************************
 *
 * :: osSetCurDir
 *
 *****************************************************************************/

#if !defined(OS_Has_SetCurDir)

extern int	chdir			(/* char * path */);

Int
osSetCurDir(fn)
	String	fn;
{
	return chdir(fn);
}

#endif	/* ! OS_Has_SetCurDir */

/******************************************************************************
 *
 * :: osMakeDir
 *
 *****************************************************************************/

/* No generic version of osMakeDir. */

/******************************************************************************
 *
 * :: osRemoveDir
 *
 *****************************************************************************/

/* No generic version of osRemoveDir. */

/******************************************************************************
 *
 * :: osFileIsThere
 *
 *****************************************************************************/

#if !defined(OS_Has_FileIsThere)

Int
osFileIsThere(fn)
	String	fn;
{
	FILE *	fin = fopen(fn, "r");

	if (fin) fclose(fin);

	return fin != 0;
}

#endif	/* ! OS_Has_FileIsThere */

/******************************************************************************
 *
 * :: osFileCat
 *
 *****************************************************************************/

#if !defined(OS_Has_FileCat)

Int
osFileCat(fn)
	String	fn;
{
	FILE *	fin = fopen(fn, "r");
	Int	c;

	if (!fin) return -1;
	while ((c = fgetc(fin)) != EOF)
		fputc(c, stdout);

	fclose(fin);
	return 0;
}

#endif	/* ! OS_Has_FileCat */

/******************************************************************************
 *
 * :: osFileCopy
 *
 *****************************************************************************/

#if !defined(OS_Has_FileCopy)

Int
osFileCopy(src, dest)
	String	src;
	String	dest;
{
	FILE *	fin;
	FILE *	fout;
	Int	c;

	fin = fopen(src, "r");
	if (!fin) return -1;

	fout = fopen(dest, "w");
	if (!fout) { fclose(fin); return -1; }

	while ((c = fgetc(fin)) != EOF)
		fputc(c, fout);

	fclose(fout);
	fclose(fin);
	return 0;
}

#endif	/* ! OS_Has_FileCopy */

/******************************************************************************
 *
 * :: osFileEqual
 *
 *****************************************************************************/

#if !defined(OS_Has_FileEqual)

Int
osFileEqual(src, dest)
	String	src;
	String	dest;
{
	FILE *	f1;
	FILE *	f2;
	Int	c1, c2;

	f1 = fopen(src, "r");
	if (!f1) return 0;

	f2 = fopen(dest, "r");
	if (!f2) { fclose(f1); return 0; }

	c1 = c2 = '\0';
	while (c1 == c2 && c1 != EOF && c2 != EOF) {
		c1 = fgetc(f1);
		c2 = fgetc(f2);
		if (c1 == '\r') c1 = fgetc(f1);
		if (c2 == '\r') c2 = fgetc(f2);
		if (c1 == OS_PATH_SEP[0]) c1 = '/';
		if (c2 == OS_PATH_SEP[0]) c2 = '/';
	}

	fclose(f1);
	fclose(f2);

	return c1 == c2;
}

#endif	/* ! OS_Has_FileEqual */

/******************************************************************************
 *
 * :: osFileRemove
 *
 *****************************************************************************/

#if !defined(OS_Has_FileRemove)

extern int	unlink			(/* char * path */);

Int
osFileRemove(fn)
	String	fn;
{
	return unlink(fn);
}

#endif	/* ! OS_Has_FileRemove */

/******************************************************************************
 *
 * :: osFileBase
 *
 *****************************************************************************/

#if !defined(OS_Has_FileBase)

String
osFileBase(fn)
	String	fn;
{
	String	res, s, t;
	Length	cc;
	t = strrchr(fn, '/');
	if (t)
		fn = t+1;
	s = strrchr(fn, '.');
	if (!s) return fn;

	cc = s - fn;
	res = strAlloc(cc);
	strncpy(res, fn, cc);
	return res;
}

#endif	/* ! OS_Has_FileBase */

#if !defined(OS_Has_FileDirName)

String
osFileDirName(fn)
	String fn;
{
	String res;
	int c;
	char *t = strrchr(fn, '/');
	if (t == NULL) {
		return NULL;
	}
	c = t - fn;
	res = strAlloc(c);
	strncpy(res, fn, c);
	return res;
}

#endif

/******************************************************************************
 *
 * :: osFileCombine
 *
 *****************************************************************************/

#if !defined(OS_Has_FileCombine)

String
osFileCombine(dir, fn)
	String	dir;
	String	fn;
{
	return strPrintf("%s%s%s", dir, OS_PATH_SEP, fn);
}

#endif	/* ! OS_Has_FileBase */

/******************************************************************************
 *
 * :: osFnameTempSeed
 *
 *****************************************************************************/

#if !defined(OS_Has_FnameTempSeed)

extern int	getpid			(/* void */);

Int
osFnameTempSeed()
{
	return getpid();
}

#endif	/* ! OS_Has_FnameTempSeed */

/******************************************************************************
 *
 * :: osTempDirName
 *
 *****************************************************************************/

/* No generic version of osTempDirName. */
