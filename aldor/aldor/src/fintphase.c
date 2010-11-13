/*****************************************************************************
 *
 * fint.c: foam interpreter.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/


#include "axlobs.h"
#include "syscmd.h"
#include "foam_c.h"
#include "fint.h"
#include "output.h"
#include "phase.h"
#include "cmdline.h"

/* Declare a new option for the interactive mode. 
 * Example:
 *      FINT_DECLARE_OPTION(verbose, "verbose") 
 * creates the following declarations:
 *      String verboseOpt = "verbose";  
 *      Length verboseOptLen = strLength(verboseOpt);
 */

#define FINT_DECLARE_OPTION(var, str)  \
         String Abut(var,Opt) = str;   \
         Length Abut(var,OptLen) = strLength(Abut(var,Opt))

void
fintParseOptions(String str)
{
	FINT_DECLARE_OPTION(verbose, "verbose");
	FINT_DECLARE_OPTION(history, "history");
	FINT_DECLARE_OPTION(confirm, "confirm");
	FINT_DECLARE_OPTION(timings, "timing");
	FINT_DECLARE_OPTION(msgLimit, "msg-limit");
	FINT_DECLARE_OPTION(runGbc, "gc");
	FINT_DECLARE_OPTION(help, "help");
	FINT_DECLARE_OPTION(options, "options");
	FINT_DECLARE_OPTION(shell, "shell");
	FINT_DECLARE_OPTION(cd, "cd");
	FINT_DECLARE_OPTION(exntrace, "exntrace");
	
	while (*str && *str == ' ') str++;

	if (!strncmp(str, verboseOpt, verboseOptLen)) {
		str += verboseOptLen;
		if (!strncmp(str, " on", 3)) fintVerbose = true;
		else if (!strncmp(str, " off", 4)) fintVerbose = false;
		(void)comsgFPrintf(osStdout, ALDOR_M_FintOptionState, verboseOpt,
			     fintVerbose ? "on" : "off");
		return;
	}

	if (!strncmp(str, historyOpt, historyOptLen)) {
		str += historyOptLen;
		if (!strncmp(str, " on", 3)) fintHistory = true;
		else if (!strncmp(str, " off", 4)) fintHistory = false;
		return;
	}

	if (!strncmp(str, confirmOpt, confirmOptLen)) {
		str += confirmOptLen;
		if (!strncmp(str, " on", 3)) fintConfirm = true;
		else if (!strncmp(str, " off", 4)) fintConfirm = false;
		(void)comsgFPrintf(stdout, ALDOR_M_FintOptionState, confirmOpt,
			     fintConfirm ? "on" : "off");
		return;
	}

	if (!strncmp(str, timingsOpt, timingsOptLen)) {
		str += timingsOptLen;
		if (!strncmp(str, " on", 3)) fintTimings = true;
		else if (!strncmp(str, " off", 4)) fintTimings = false;
		(void)comsgFPrintf(stdout, ALDOR_M_FintOptionState, timingsOpt,
			     fintTimings ? "on" : "off");
		return;
	}

	if (!strncmp(str, msgLimitOpt, msgLimitOptLen)) {
		int limit;
		str += msgLimitOptLen;

		if (scmdScanInteger(str, &limit)) 
			fintMsgLimit = (limit ? limit : ABPP_UNCLIPPED);

		if (fintMsgLimit == ABPP_UNCLIPPED)
			(void)comsgFPrintf(osStdout, ALDOR_M_FintOptionState,
				     msgLimitOpt, "none");
		else
			(void)comsgFPrintf(osStdout, ALDOR_M_FintOptionValue,
				     msgLimitOpt, fintMsgLimit);
		return;
	}
	if (!strncmp(str, exntraceOpt, exntraceOptLen)) {
		int mode;
		str += exntraceOptLen;

		if (scmdScanInteger(str, &mode)) 
			fintExntraceMode = (mode ? mode : 0);
		if (fintExntraceMode < 0 ) fintExntraceMode = 0;
		if (fintExntraceMode > 2 ) fintExntraceMode = 2;

		(void)comsgFPrintf(osStdout, ALDOR_M_FintOptionValue,
				     exntraceOpt, fintExntraceMode);
		return;
	}

	if (!strncmp(str, runGbcOpt, runGbcOptLen)) {
		(void)comsgFPrintf(osStdout, ALDOR_M_FintGbcStart);
		fintFreeJunk();
		stoGc();
		(void)comsgFPrintf(osStdout, ALDOR_M_FintGbcEnd);
		if (fintVerbose) phGrandTotals(true);
			
		return;
	}

	if (!strncmp(str, optionsOpt, optionsOptLen)) {
	        int     argc, iargc;
	        String  *argv;
	
	        str += optionsOptLen;
	        cmdParseOptions     (str, &argc, &argv);
	        iargc = cmdArguments(int0, argc, argv);
	        cmdFreeOptions      (argc, argv);
	
	        if (argc > iargc)
			(void)comsgFPrintf(osStdout, ALDOR_M_FintIntOptionsNoFile,
				     argv[iargc]);
		return;
	}

	if (!strncmp(str, helpOpt, helpOptLen)) {
		(void)comsgFPrintf(osStdout, ALDOR_M_FintOptions,
		       verboseOpt, historyOpt, confirmOpt, timingsOpt,
		       msgLimitOpt, optionsOpt, runGbcOpt, shellOpt, cdOpt,
		       exntraceOpt, helpOpt);
		return;
	}

	if (!strncmp(str, shellOpt, shellOptLen)) {
		String	shellCmd;

		str += shellOptLen;

		if (!scmdScanFName(str, &shellCmd)) {
			(void)comsgFPrintf(osStdout, ALDOR_M_ShellSyntax, shellOpt);
			return;
		}

		(void)osRun(shellCmd);

		return;
	}

	if (!strncmp(str, cdOpt, cdOptLen)) {
		String	newDir;

		str += cdOptLen;

		if (!scmdScanId(str, &newDir)) {
			(void)comsgFPrintf(osStdout, ALDOR_M_CdSyntax, cdOpt);
			return;
		}

		if (osDirSwap(newDir, NULL, int0) == -1)
			(void)comsgFPrintf(osStdout, ALDOR_M_InvalidDir);

		return;
	}


	(void)comsgFPrintf(osStdout, ALDOR_M_FintUnknownOpt, helpOpt);
	return;
}
