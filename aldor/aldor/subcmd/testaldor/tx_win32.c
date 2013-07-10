/******************************************************************************
 *
 * tx_win32.c
 *
 * Win32 variants of the OS-level utilities for testaldor.
 *
 *****************************************************************************/

#include <windows.h>

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
        if (!DuplicateHandle(GetCurrentProcess(), //me
                        hout,                     // handle to copy
                        GetCurrentProcess(),
                        &dout, 0, TRUE, 
                        DUPLICATE_SAME_ACCESS))
                return -1;
        if (!DuplicateHandle(GetCurrentProcess(), //me
                        herr,                     // handle to copy
                        GetCurrentProcess(),
                        &derr, 0, TRUE, 
                        DUPLICATE_SAME_ACCESS))
                return -1;
        if (!DuplicateHandle(GetCurrentProcess(), //me
                        hin,                     // handle to copy
                        GetCurrentProcess(),
                        &din, 0, TRUE, 
                        DUPLICATE_SAME_ACCESS))
                return -1;
        /* Create process. */
        memset(&si, 0, sizeof(si));
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

        return wait ? rc : (int) pi.hProcess;
}

/******************************************************************************
 *
 * :: osRun
 *
 *****************************************************************************/

#define OS_Has_Run

Int
osRun(cmd)
	String	cmd;
{
	return osRunRedirect(cmd, TRUE, NULL, NULL, NULL);
}

/******************************************************************************
 *
 * :: osRunOutput
 *
 *****************************************************************************/

#define OS_Has_RunOutput

Int
osRunOutput(cmd, fout)
	String	cmd;
	String	fout;
{
	HANDLE	hdl;
	BOOL	rc;

	if (!fout) fout = "nul";
	hdl = CreateFile(fout, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			 FILE_ATTRIBUTE_NORMAL, NULL);

	rc = osRunRedirect(cmd, TRUE, NULL, hdl, hdl);
	if (rc == -1) return -1;

	CloseHandle(hdl);
	return 0;
}


/******************************************************************************
 *
 * :: osRunScript
 *
 *****************************************************************************/

#define OS_Has_RunScript

Int
osRunScript(cmd, fnout)
	String	cmd;
	String	fnout;
{
	String	bn = osFileBase(cmd);
	String	fname = strConcat(bn, ".sh");
	String	bname = strConcat(bn, ".s");
	FILE *	fin;
	FILE *	fout;
	Int	c, pc, rc;
	
#if 0
	fin = fopen(fname, "r");
	if (!fin) return -1;

	fout = fopen(bname, "w");
	if (!fout) { fclose(fin); return -1; }

	c = '\n';
	fputs("@echo off\n", fout);
	while (pc = c, (c = fgetc(fin)) != EOF)
		if (pc == '\n' && c == '#')
			fputs("rem #", fout);
		else
			fputc(c, fout);

	fclose(fout);
	fclose(fin);
#endif
	bname = strConcat("sh ", fname);
	rc = osRunOutput(bname, fnout);
#if 0
	osFileRemove(bname);
#endif
	return rc;
}

/******************************************************************************
 *
 * :: osShowDiff
 *
 *****************************************************************************/

#define OSFmtDiff	"fc %s %s"

/******************************************************************************
 *
 * :: osPutEnv
 *
 *****************************************************************************/

#define putenv		_putenv

/******************************************************************************
 *
 * :: osGetCurDir
 *
 *****************************************************************************/

#define getcwd		_getcwd

/******************************************************************************
 *
 * :: osSetCurDir
 *
 *****************************************************************************/

#define chdir		_chdir

/******************************************************************************
 *
 * :: osMakeDir
 *
 *****************************************************************************/

extern int	_mkdir			(const char * path);

Int
osMakeDir(fn)
	String	fn;
{
	return _mkdir(fn);
}

/******************************************************************************
 *
 * :: osRemoveDir
 *
 *****************************************************************************/

Int
osRemoveDir(fn)
	String	fn;
{
	char			cwd[MAX_PATH];
	WIN32_FIND_DATA		wfd;
	HANDLE			hdl;
	String			fn0;

	if (osGetCurDir(cwd, MAX_PATH) != 0)
		return -1;

	if (osSetCurDir(fn) != 0)
		return -1;

	for (hdl = FindFirstFile("*", &wfd); FindNextFile(hdl, &wfd); ) {
		if (hdl == INVALID_HANDLE_VALUE) break;

		fn0 = wfd.cFileName;
		if (strEqual(fn0, ".") || strEqual(fn0, "..")) continue;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			osRemoveDir(fn0);
		else
			osFileRemove(fn0);
	}

	if (hdl != INVALID_HANDLE_VALUE) FindClose(hdl);

	osSetCurDir(cwd);
	return RemoveDirectory(fn) ? 0 : -1;
}

/******************************************************************************
 *
 * :: osFileRemove
 *
 *****************************************************************************/

#define unlink		_unlink

/******************************************************************************
 *
 * OS-level utilities
 *
 *****************************************************************************/

#define	getpid		_getpid

/******************************************************************************
 *
 * :: osFileCombine
 *
 *****************************************************************************/

#define		OS_PATH_SEP		"\\"

/******************************************************************************
 *
 * :: osTempDirName
 *
 *****************************************************************************/

String
osTempDirName()
{
	return strPrintf("\\tmp\\testaldor.%d", osFnameTempSeed());
}
