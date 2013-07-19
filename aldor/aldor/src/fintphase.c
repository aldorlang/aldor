/*****************************************************************************
 *
 * fint.c: foam interpreter.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/


#include "axlobs.h"
#include "cmdline.h"
#include "fint.h"
#include "fintphase.h"
#include "foam_c.h"
#include "opsys.h"
#include "output.h"
#include "phase.h"
#include "store.h"
#include "syscmd.h"
#include "tinfer.h"

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

	/*
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
	*/
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

/*
 * :: Wrapping
 */
local AbSyn	fintWrapHistory		(AbSyn, int);
local AbSyn	fintWrapVerbose		(AbSyn);
local Bool tfHasPrintFlag;



/* This function is called from Aldor in interactive mode. It looks if the absyn
 * generated from a line of Aldor source has a return type that can be a rhs.
 * In this case it generates an assignment "%<line> := <absyn>", so that the
 * user can use %<line> as an identified.
 * Return value: the original ab if his type isn't a rhs, else the new absyn
 * with the assignment.
 *
 * NB: This is all very gross and unpleasant.  A better idea would be a 
 * way of calling some precompiled foam so we can set up exception
 * handling at the Aldor level (not down here where no bugger knows what is
 * happening).
 */

AbSyn
wrapAssignIfNeeded(AbSyn ab, int stepNo)
{
	return fintWrap(ab, stepNo);
}

AbSyn
fintWrap(AbSyn ab, int stepNo)
{
	TForm	type = abTUnique(ab);

	if (tfIsUnknown(type) || tfIsNone(type))
		return ab;

	if (fintHistory && tfIsGeneral(type))
		ab = fintWrapHistory(ab, stepNo);

	if (fintVerbose)
		ab = fintWrapVerbose(ab);

	return ab;
}

local AbSyn
fintWrapHistory(AbSyn ab, int stepNo)
{
	Stab	stab = stabFile();
	TForm	type = abTUnique(ab);

	char	idStr[10];
	Symbol	newId;
	AbSyn	abId, abAssign;
	Syme	syme;

	(void)sprintf(idStr, "%%%d", stepNo);

	newId = symIntern(idStr);

	abId = abNewId(sposNone, newId);
	abUse(abId) = AB_Use_Assign;

	syme = stabDefLexVar(stab, newId, type);
	stabSetSyme(stab, abId, syme, ablogTrue());

	abState(abId) = AB_State_HasUnique;
	abTUnique(abId) = type;

	abAssign = abNewAssign(sposNone, abId, ab);
	abState(abAssign) = AB_State_HasUnique;
	abTUnique(abAssign) = type;
	abUse(abAssign) = AB_Use_Assign;

	return abAssign;
}

local AbSyn
fintWrapVerbose(AbSyn ab)
{
	TForm	type = tfDefineeType(abTUnique(ab));
	Stab	stab = stabFile();
	AbSyn	op;

	if (tfSatDom(type)) {
		/* printf("(domain ???)\n"); */
/* 		ab = abNew(AB_Apply, sposNone, 2,
 *				 abNewId(sposNone, ssymDomainName),
 *				 ab);
 */
	}

	else if (tfSatCat(type))
		/*printf("(category ???)\n") */;

	else if (abTag(ab) == AB_Define)
		/* Definitions are printed specially. */;

	else if ((tfHasPrintFlag = tfHasPrint(stab, type)) == true) {
		/* (print << ab)$type */
		op = abNewQualify(sposNone, abNewId(sposNone, ssymPrint),
				  tfExpr(type));
		ab = abNewApply2(sposNone, op,
				 abNewId(sposNone, ssymThePrint), ab);

		(void)typeInferAs(stab, ab, tfUnknown);
		abState(ab) = AB_State_HasUnique;
		abTUnique(ab) = type;
	}

	return ab;
}


void
fintPrintType(FILE * fout, AbSyn ab)
{
	TForm tf = abTUnique(ab);
	String s;

	if (!fintVerbose || !tfIsValue(tf)) return;

	/* if (tfIsId(tf) && abIsTheId(tfIdSym(tf), ssymTextWriter)) return; */

	if (abTag(ab) == AB_Define) {
                AbSyn   abId;
                AbSyn   abComma;
                Length  ai;
 
                if (abTag(ab->abDefine.lhs) == AB_Declare)
                {
                        abId = ab->abDefine.lhs->abDeclare.id;
                        (void)fprintf(fout, "Defined %s", symString(abLeafSym(abId)));
                        tf = abTUnique(ab->abDefine.lhs);
                }
                else if (abTag(ab->abDefine.lhs) == AB_Comma)
                {
                        abComma = ab->abDefine.lhs;
                        (void)fprintf(fout, "Defined (");
                        for (ai = 0; ai < abArgc(abComma); ai++)
                        {
                                abId = abArgv(abComma)[ai];
                                (void)fprintf(fout, "%s%s",
                                        (ai == 0) ? "" : ", ",
                                        symString(abLeafSym(abId))
                                        );
                        }
                        (void)fprintf(fout, ")");
                }
                else
                {
                        abId = ab->abDefine.lhs;
                        (void)fprintf(fout, "Defined %s", symString(abLeafSym(abId)));
                        tf = abTUnique(ab->abDefine.lhs);
                }
	}
	else if (!tfHasPrintFlag) 
		(void)fprintf(fout, "  ()");

	s =tfPrettyClippedIn(tf, fintMsgLimit, int0);
	(void)fprintf(fout, " @ %s\n", s);
	strFree(s);
}
