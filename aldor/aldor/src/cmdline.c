/*****************************************************************************
 *
 * cmdline.c: OS Command line processing.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
#include "bloop.h"
#include "ccomp.h"
#include "cmdline.h"
#include "emit.h"
#include "file.h"
#include "fint.h"
#include "gencpp.h"
#include "genfoam.h"
#include "genlisp.h"
#include "include.h"
#include "of_emerg.h"
#include "of_util.h"
#include "opsys.h"
#include "optfoam.h"
#include "phase.h"
#include "store.h"
#include "syscmd.h"
#include "test.h"
#include "tinfer.h"
#include "util.h"
#include "archive.h"
#include "comsg.h"
#include "strops.h"

String		cmdName		    = "aldor";    /* Could use argv[0]. */
String		cmdInitFile	    = "aldorinit.as";
 
Bool		cmdVerboseFlag	    = false;
Bool		cmdGcFlag           = false;
Bool		cmdGcFileFlag       = false;
Bool		cmdSExprFlag	    = false;
Bool		cmdSEvalFlag	    = false;
Bool		cmdTrapFlag	    = false;
int		cmdFileCount	    = 0;

#define		optIs(a,b)	(toupper(a) == toupper(b))

/*****************************************************************************
 *
 * :: General option processing
 *
 ****************************************************************************/
 
/*
 * These are the option characters used:
 *
 *  A B C D E F G H I   K L M   O P Q R S   U V W X Y Z - 
 *
 * These are still available:
 *
 *                    J                   T              
 */

local void	cmdUseError	  (Msg fmt, String opt);

local Bool	cmdIsOption       (int);
local Bool	cmdIsArgOption	  (int);
local int	cmdGetOption	  (int argc,String*argv,
				   int *i, int *j, String *parg);

local int	cmdDoOptHelp	  (String arg);
local int	cmdDoOptDeveloper (String arg);
local int	cmdDoOptDebug     (String arg);
local int	cmdDoOptGo	  (String arg);

static StringList cmdIncDirs;
static StringList cmdLibDirs;
static StringList cmdLibKeys;
 
int
cmdArguments(int argi0, int argc, String *argv)
{
	int		opt, i, j;
	String		arg;
	StringList	sl;
 
	/*
	 * Initialize default values.
	 */
	cmdFileCount = argc - 1;
	cmdIncDirs   = cmdLibDirs = cmdLibKeys = 0;
 
	/*
	 * Handle the options.
	 */
	for (i = argi0, j = 0; ; ) {
		opt = cmdGetOption(argc, argv, &i, &j, &arg);
		if (!opt) break;
		if (cmdIsArgOption(opt) && !arg)
			cmdUseError(ALDOR_F_CmdBadOption, argv[i-1]);
		cmdHandleOption(opt, arg);
	}
 
	/*
	 * After some sequence of -O and -Q do we still want cc -O ?
	 */
	ccSetOptimize(optIsCcFNonStdWanted(),optIsCcOptimizeWanted());
 
	/*
	 * Push directories etc in correct order.
	 */
	for (sl = cmdIncDirs; sl; sl = cdr(sl))
		fileAddIncludeDirectory(car(sl));
	for (sl = cmdLibDirs; sl; sl = cdr(sl))
		fileAddLibraryDirectory(car(sl));
	for (sl = cmdLibKeys; sl; sl = cdr(sl)) {
		String	id, key, s;
		int	rc;
		s = scmdScanLibraryOption(car(sl), &id, &key);
		rc = s ? scmdHandleLibrary(id, key) : -1;
		if (rc < 0) cmdUseError(ALDOR_F_CmdBadOption, "-l");
	}
	listFree(String)(cmdIncDirs);
	listFree(String)(cmdLibDirs);
	listFree(String)(cmdLibKeys);
 
	/*
	 * Determine number of arguments consumed.
	 */
	if (i + cmdFileCount > argc) cmdFileCount = argc - i;

	return i;
}

String cmdOptionArg = "";

Bool
cmdHasOptionPrefix(int opt0, String arg0, int argc, String *argv)
{
	int opt, i, j;
 
	cmdOptionArg = NULL;

	for (i = 1, j = 0; ; ) {
		opt = cmdGetOption(argc, argv, &i, &j, &cmdOptionArg);
		if (!opt) break;
		
		if (!cmdIsOption(opt)) { i++; j = 0; continue; }
		if (optIs(opt, opt0)) {
                        if (!arg0) return true;
                        if (cmdOptionArg && strAIsPrefix(arg0, cmdOptionArg)) {
				cmdOptionArg+=strlen(arg0);
				return true;
			}
		}
	}
	return false;
}

Bool
cmdHasOption(int opt0, String arg0, int argc, String *argv)
{
	int opt, i, j;
 
	cmdOptionArg = NULL;

	for (i = 1, j = 0; ; ) {
		opt = cmdGetOption(argc, argv, &i, &j, &cmdOptionArg);
		if (!opt) break;
		
		if (!cmdIsOption(opt)) { i++; j = 0; continue; }
		if (optIs(opt, opt0)) {
                        if (!arg0) return true;
                        if (cmdOptionArg && strAEqual(arg0, cmdOptionArg))
                               return true;
		}
	}
	return false;
}

/*
 * Get command option, ignoring spaces after option name.
 * Make i and j point to next one.
 */
local int
cmdGetOption(int argc, String *argv, int *i, int *j, String *parg)
{
	int	opt;
 
	*parg = NULL;
 
	if (argc <= *i) return 0;
 
	/* Require leading hyphen.  Handle free-standing "-" and "--". */
 
	if (*j == 0) {
		if (*argv[*i] != '-')		return 0;
		if (strEqual(argv[*i], "-" ))   return 0;
		if (strEqual(argv[*i], "--"))   { *i += 1; return 0; }
		*j = 1;
	}
 
	/* Grab option. */
	opt = argv[*i][(*j)++];
 
	/* If this is the last character in this argument, start next. */
	if (!argv[*i][*j]) {
		*i += 1;
		*j  = 0;
	}
 
	/* If the option takes an argument, grab it too. */
	if (cmdIsArgOption(opt)) {
		if (*i == argc) {
			*parg = NULL;
		}
		else {
			*parg = &argv[*i][*j];
			*i += 1;
			*j  = 0;
		}
	}
	return opt;
}
 
/*
 * Indicate whether the given character is a valid option.
 */
local Bool
cmdIsOption(int opt)
{
	switch (toupper(opt)) {
	case '-':

	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
	case 'G': case 'H': case 'I':           case 'K': case 'L':
	case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
	case 'S':           case 'U': case 'V': case 'W': case 'X':
	case 'Y': case 'Z':
		return true;

	default:
		return false;
	}
}
 
/*
 * Indicate whether the given option takes an argument.
 */
local Bool
cmdIsArgOption(int opt)
{
	switch (toupper(opt)) {
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'K':
	case 'L': case 'M': case 'N': case 'P': case 'Q': 
        case 'R': case 'S': case 'U': case 'W': case 'Y': 
	case 'Z':
		return true;
	default:
		return false;
	}
}
 
 
void
cmdHandleOption(int opt, String arg)
{
	int	rc  = 0;
	Msg	bad = ALDOR_F_CmdBadOption;
 
	switch (toupper(opt)) {
	  case 'A':
		/* already handled -- should not appear */
		break;
	  case 'B':
		/* already handled -- ignore */
		break;
	  case 'N':
		/* already handled -- ignore */
		break;
	  case 'V':
		cmdVerboseFlag = true;
		ccSetVerbose(true);
		break;
	  case 'H':
		rc = cmdDoOptHelp(arg);
		break;
	  case 'D':
		inclGlobalAssert(arg);
		break;
	  case 'U':
		inclGlobalUnassert(arg);
		break;
	  case 'R':
		rc = emitSetOutputDir(arg);
		bad = ALDOR_F_CmdNoOutputDir;
		break;
	  case 'I':
		cmdIncDirs = listCons(String)(arg, cmdIncDirs);
		break;
	  case 'Y':
		cmdLibDirs = listCons(String)(arg, cmdLibDirs);
		break;
	  case 'L':
		cmdLibKeys = listCons(String)(arg, cmdLibKeys);
		break;
	  case 'O':
		rc = optSetStdOptimization();
		break;
	  case 'Q':
		rc = optSetOptimization(arg);
		break;
	  case 'F':
		rc = emitSelect(arg);
		break;
	  case 'E':
		emitSetEntryFile(arg);
		break;
	  case 'G':
		rc = cmdDoOptGo(arg);
		break;
	  case 'C':
		rc = ccOption(arg);
		break;
	  case 'P':
		rc = cppOption(arg);
		break;
	  case 'S':
		rc = genLispOption(arg);
		break;
	  case 'M':
		rc = comsgSetOption(arg);
		break;
	  case 'W':
		rc = cmdDoOptDeveloper(arg);
		break;
	  case 'Z':
		rc = cmdDoOptDebug(arg);
		break;
	  case 'K':
		cmdFileCount = atoi(arg);
		break;
	  case 'X':
		arUseExpandedReplacement();
		break;
	  default:
		bad = ALDOR_F_CmdNoOption;
		rc  = -1;
		break;
	}
	if (rc < 0) {
		char	s[3];
		sprintf(s, "-%c", opt);
		cmdUseError(bad, s);
	}
}
 
 
/*****************************************************************************
 *
 * :: Echo a command line
 *
 ****************************************************************************/
 
static FILE	*cmdEchoFile;
local  int	cmdEchoChar(int c) { return fputc(c, cmdEchoFile); }
 
void
cmdEcho(FILE *fout, int argc, String *argv)
{
	int	i;
 
	cmdEchoFile = fout;
	fprintf(cmdEchoFile, "Exec: ");
	for (i = 0; i < argc; i++) {
		if (i > 0) fputc(' ', cmdEchoFile);
		osRunQuoteArg(argv[i], cmdEchoChar);
	}
	fprintf(cmdEchoFile, "\n");
	fflush(cmdEchoFile);
}
 


/*****************************************************************************
 *
 * :: Response files
 *
 ****************************************************************************/

/*
 * 'startArg' points to the '-a' option.
 * 'nextArg'  points to the next option.
 * Thus nextArg = startArg+1 if the filename abuts '-a'
 * or	nextArg = startArg+2 if there is whitespace between.
 */
local void cmdOneResponse(int *pargc, String **pargv, String text,
			  int startArg, int nextArg);

/*
 * Subsume options in response file into argument vector.
 * Only the slots 'argi0..*pargc-1 are treated as arguments.
 */
Bool
cmdSubsumeResponseFiles(int argi0, int *pargc, String **pargv)
{
	int 	nresps = 0, i;
	String 	*argv;
	String 	envopts;

	assert(*pargc >= 1);

	/* Copy the original args into a new r/w vector.
	 * The extra slot is potentially used in handling AXIOMXLARGS.
	 */
	argv = (String *) stoAlloc(OB_Other, (*pargc+1) * sizeof(String *));
	for (i = 0; i < *pargc; i++)
		argv[i] = strCopy((*pargv)[i]);
	*pargv  = argv;
 
	/* Check for ALDORARGS/AXIOMXLARGS environment variable. */
	envopts = osGetEnv("ALDORARGS");
	if (!envopts) envopts = osGetEnv("AXIOMXLARGS");
	if (envopts) {
		envopts = strCopy(envopts);
		for (i = *pargc - 1; i >= argi0; i--)
			(*pargv)[i+1] = (*pargv)[i];
		(*pargv)[argi0] = strCopy("-aFake");
		(*pargc)++;
		cmdOneResponse(pargc, pargv, envopts, argi0, argi0+1);
		nresps++;
		strFree(envopts);
	}

	while (cmdHasOption('a', NULL, *pargc, *pargv)) {
		String	 fileName = 0, fileText;
		FileName fn;
		int	 opt = 0, nextArg, startArg, j;
 
		for (nextArg = argi0, j = 0; ; ) {
			startArg = nextArg;
			opt = cmdGetOption(*pargc,*pargv,
					   &nextArg,&j,&fileName);

			/* Have response file option by itself. */
			if (optIs(opt, 'a'))
				break;
 
			/* Next option is response file option. Must be last */
			else if (j > 0 && optIs((*pargv)[startArg][j], 'a')) {
				int oldStartArg = startArg, oldJ = j;
				startArg = nextArg;
				opt = cmdGetOption(*pargc,*pargv,
						   &nextArg,&j,&fileName);
				(*pargv)[oldStartArg][oldJ] = '\0';
				++startArg;
				break;
			}
		}
 
		if (! fileName || ! fileName[0])
			/* Can't use cmdUseError here - no msg db! */
			comsgFatal(NULL,ALDOR_F_CmdBadOption,"-a",cmdName);
 
		fn = fnameParse(fileName);
		if (! fileIsReadable(fn))
			comsgFatal(NULL, ALDOR_F_CantOpen, fileName);
 
		/* We now have an existing response file */
		fileText = fileContentsString(fn);
		cmdOneResponse(pargc, pargv, fileText, startArg, nextArg);
		fileFreeContentsString(fileText);

		nresps++;
	}
 
	return nresps > 0;
}
 
local void
cmdOneResponse(int *pargc, String **pargv, String s, int startArg, int nextArg)
{
	static String *firstArgv = 0;
	String	*newArgv = 0;
	int	newArgc = 0;
 
	if (firstArgv == 0) firstArgv = *pargv;
 
	cmdParseOptions(s, &newArgc, &newArgv);

	if (newArgv) {
		int i, j;
		String *oArgv = newArgv;

		newArgv = (String *) stoAlloc(OB_Other,
			  (*pargc+newArgc-(nextArg-startArg))*sizeof(String));

		for (i = 0; i < newArgc; i++)
			newArgv[startArg+i] = oArgv[i];
		stoFree(oArgv);
 
		for (i = 0; i < startArg; i++)
			newArgv[i] = (*pargv)[i];
 
		j = newArgc - (nextArg - startArg);
 
		for (i = nextArg; i < *pargc; i++)
			newArgv[i+j] = (*pargv)[i];
		*pargc += j;
 
		if (*pargv != firstArgv)
			stoFree(*pargv);
		*pargv = newArgv;
	}
	else {
		/* response file is empty, so skip those args */
 
		int i, k;
		k = nextArg - startArg;
		for (i = nextArg; i < *pargc; i++)
			(*pargv)[i-k] = (*pargv)[i];
		*pargc -= k;
	}
}
 

/*****************************************************************************
 *
 * :: cmdParseOptions
 * :: cmdFreeOptions
 * 
 * Parse an option string for: -a <file>, AXIOMXLARGS, #int options ... etc.
 *
 ****************************************************************************/


struct optionList {
	char *opt;
	struct optionList *next;
};
 
local struct optionList *
newOpt(String opt, int characters)
{
	struct optionList *ol;
	String s;
 
	ol = (struct optionList *) stoAlloc(OB_Other, sizeof(*ol));
	s = ol->opt = strAlloc(characters);
	ol->next = 0;
	while (*opt && characters > 0) {
		*(s++) = *(opt++);
		characters--;
	}
	*s = '\0';
	return ol;
}
 
local Bool
isOptEnd(char ch)
{
    String optEnders = "\"\'";

    return isspace(ch) || strchr(optEnders, ch);
}
 
void
cmdParseOptions(String opts, int *pargc, String **pargv)
{
	struct optionList *optList = 0;
	String *argv;
	int i, argc;
 
	assert(pargc && pargv);

	/* strip leading white space */
	if (opts) while (*opts && isspace(*opts)) ++opts;
 
	if (! opts || ! *opts) {
		if (pargc) *pargc = 0;
		if (pargv) *pargv = 0;
		return;
	}

	for (argc = 0; *opts; argc++) {
		char		  *opt = opts;
		struct optionList *ol;
 
		if (*opts == '-') {
			++opts;
			if (! *opts || isspace(*opts))
				ol = newOpt(opt, 1);
			else {
				++opts;
				while (*opts && ! isOptEnd(*opts))
					++opts;
				ol = newOpt(opt, (int) (opts - opt));
			}
		}
		else if (*opts == '\'' || *opts == '"') {
			char q = *opts;
			++opts;
			while (*opts && *opts != q)
				++opts;
			if (*opts)
				++opts;
			ol = newOpt(opt, (int) (opts - opt));
		}
		else {
			++opts;
			while (*opts && ! isOptEnd(*opts))
				++opts;
			ol = newOpt(opt, (int) (opts - opt));
		}
 
		ol->next = optList;
		optList  = ol;
 
		while (*opts && isspace(*opts))
			++opts;
	}

	argv = (String *) stoAlloc(OB_Other, argc*sizeof(String));

	for (i = argc-1; i >= 0; i--) {
		struct optionList *tmp = optList->next;
		argv[i] = optList->opt;
		stoFree(optList);
		optList = tmp;
	}

	if (pargc) *pargc = argc;
	if (pargv) *pargv = argv;
}
 
void
cmdFreeOptions(int argc, String *argv)
{
	int	i;
	for (i = 0; i < argc; i++) stoFree((Pointer) argv[i]);
	stoFree((Pointer) argv);
}


/*****************************************************************************
 *
 * :: HELP options (-h)
 *
 ****************************************************************************/
 
local int
helpFPrintf(FILE *fout, Msg msg, ...)
{
	int	cc;
	va_list	argp;

	va_start(argp, msg);

	if (comsgOkBreakLoop())
		cc = bloopMsgVFPrintf(fout, msg, argp);
	else
		cc = comsgVFPrintf   (fout, msg, argp);

	va_end(argp);
	return cc;
}


local int
cmdDoOptHelp(String arg)
{
	int	rc = 0;
	if (!arg || strAEqual(arg, "elp")) {
		helpFPrintf(osStdout, ALDOR_H_HelpCmd);
	}
	else if (strAEqual(arg, "all")) {
		helpFPrintf(osStdout, ALDOR_H_HelpCmd);

		helpFPrintf(osStdout, ALDOR_H_HelpFileTypes, FTYPE_OBJECT);
		helpFPrintf(osStdout, ALDOR_H_HelpOptionSummary);
		helpFPrintf(osStdout, ALDOR_H_HelpHelpOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpArgOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpDirOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpFileOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpGoOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpOptimOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpDebugOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpConfigOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpCOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpCppOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpLispOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMsgOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpDevOpt);
	}
	else if (strAEqual(arg, "files")) {
		helpFPrintf(osStdout, ALDOR_H_HelpFileTypes, FTYPE_OBJECT);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "options")) {
		helpFPrintf(osStdout, ALDOR_H_HelpOptionSummary);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "info")) {
		helpFPrintf(osStdout, ALDOR_H_HelpProductInfo);
	}
	else if (strAEqual(arg, "h") || strAEqual(arg, "help")) {
		helpFPrintf(osStdout, ALDOR_H_HelpHelpOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "a") || strAEqual(arg, "args")) {
		helpFPrintf(osStdout, ALDOR_H_HelpArgOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "dir")) {
		helpFPrintf(osStdout, ALDOR_H_HelpDirOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "F") || strAEqual(arg, "fout")) {
		helpFPrintf(osStdout, ALDOR_H_HelpFileOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "g") || strAEqual(arg, "go")) {
		helpFPrintf(osStdout, ALDOR_H_HelpGoOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "Q") || strAEqual(arg, "O") ||
		 strAEqual(arg, "optimize"))
	{
		helpFPrintf(osStdout, ALDOR_H_HelpOptimOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "Z") || strAEqual(arg, "debug")) {
		helpFPrintf(osStdout, ALDOR_H_HelpDebugOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "N") || strAEqual(arg, "config")) {
		helpFPrintf(osStdout, ALDOR_H_HelpConfigOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "C")) {
		helpFPrintf(osStdout, ALDOR_H_HelpCOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "P")) {
		helpFPrintf(osStdout, ALDOR_H_HelpCppOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "S") || strAEqual(arg, "lisp")) {
		helpFPrintf(osStdout, ALDOR_H_HelpLispOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "M") || strAEqual(arg, "message")) {
		helpFPrintf(osStdout, ALDOR_H_HelpMsgOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else if (strAEqual(arg, "W") || strAEqual(arg, "dev")) {
		helpFPrintf(osStdout, ALDOR_H_HelpDevOpt);
		helpFPrintf(osStdout, ALDOR_H_HelpMenuPointer);
	}
	else
		rc = -1;

	return rc;
}
 
local  void
cmdUseError(Msg msg, String s)
{
	helpFPrintf(osStdout, msg, s, cmdName);
	if (fintMode != FINT_LOOP)
		exitFailure();
}
 
 
/*****************************************************************************
 *
 * :: GO options (-g)
 *
 ****************************************************************************/

local int
cmdDoOptGo(String opt)
{
	int	rc = 0;

	if (strAEqual(opt, "run")) {
		fintMode = FINT_DONT;
		emitSetRun(true);
	}
	else if (strAEqual(opt, "interp")) {
		fintMode = FINT_RUN;
		emitSetInterp(true);
	}
	else if (strAEqual(opt, "loop")) {
		fintMode = FINT_LOOP;
		emitSetInterp(false);
		emitSetRun(false);
		comsgSetInteractiveOption();
	}
	else
		rc = -1;

	return rc;
}


/*****************************************************************************
 *
 * :: DEBUG options (-Z)
 *
 ****************************************************************************/
 
local int
cmdDoOptDebug(String arg)
{
	int	rc = 0;
 
	if (!arg[0])
		rc = -1;

	else if (strAEqual("db", arg))
		emitSetDebug(true);

	else if (strAEqual("prof", arg))
		emitSetProfile(true);
	else
		rc = -1;

	return rc;
}


/*****************************************************************************
 *
 * :: DEVELOPER options (-W)
 *
 ****************************************************************************/
 
local String	cmdDGetOptArg		(String, String, char *psep);
local int	cmdDDebug		(String, Bool sense);
local void	cmdDoKeywordStatus	(String, Bool);

extern	void gencSetTraceFuns(Bool);
extern	void stabSetDumbImport(Bool);
extern	void gfSetLazyCatch(Bool);
extern	void jflowSetNegate(Bool);
extern	Bool NoWhereHack;
 
local int
cmdDoOptDeveloper(String arg)
{
	int	rc = 0;
 
	if (!arg[0])
		rc = -1;
 
	if (strAEqual("trap", arg)) {
		cmdTrapFlag = true;
	}
	else if (strAEqual("check", arg)) {
		_dont_assert = false;	   /* Test assertions. */
		stoCtl(StoCtl_Wash, true); /* Initialize of store. */
	}
	else if (strAEqual("no-fatal", arg))
		_fatal_assert = false;	   /* Make assertions non-fatal. */
	else if (strAEqual("runtime", arg))
		genSetRuntime();
	else if (strEqual("debug", arg))
		genSetDebugWanted(true);
	else if (strEqual("debugger", arg))
		genSetDebuggerWanted(true);
	else if (strEqual("depend", arg))
		emitSetDependsWanted(true);
	else if (strEqual("small-hcodes", arg))
		genSetSmallHashCodes(true);
	else if (strEqual("lazy-catch", arg))
		gfSetLazyCatch(true);
	else if (strEqual("dumb-import", arg))
		stabSetDumbImport(true);
	else if (strEqual("trace-cfuns", arg))
		gencSetTraceFuns(true);
	else if (strEqual("no-where", arg))
		NoWhereHack = true;
	else if (strAEqual("runtime-hashcheck", arg))
		genSetHashcheck();
	else if (strAIsPrefix("keyword", arg)) {
		String darg = cmdDGetOptArg(arg, "keyword", NULL);
		cmdDoKeywordStatus(darg, true);
	}
	else if (strAIsPrefix("no-keyword", arg)) {
		String darg = cmdDGetOptArg(arg, "no-keyword", NULL);
		cmdDoKeywordStatus(darg, false);
	}
	else if (strEqual("no-negated-tests", arg))
		jflowSetNegate(false);
	else if (strAEqual("audit", arg))
		foamAuditSetAll();
	else if (strAEqual("nhash", arg))
		genSetAxiomAx(true);
	else if (strAEqual("missing-ok", arg))
		tiSetSoftMissing(true);
	else if (strAEqual("sexpr", arg)) {
		cmdSExprFlag = true;
	}
	else if (strAEqual("seval", arg)) {
		cmdSEvalFlag = true;
	}
	else if (strAEqual("no-gc", arg)) {
		/* Handled already - no action need be taken here */
	}
	else if (strAEqual("gc", arg)) {
		/* Handled already - no action need be taken here */
		cmdGcFlag = true;
	}
	else if (strAEqual("gcfile", arg)) {
		/* Handled already - no action need be taken here */
		cmdGcFileFlag = true;
	}
	else if (strAEqual("loops", arg)) {
		optSetLoopOption();
	}
	else if (strAEqual("emerge-noalias", arg)) {
		emSetNoAlias();
	}
	else if (strAIsPrefix("test", arg)) {
		String darg = cmdDGetOptArg(arg, "test", NULL);
		testSelf(darg);
		exitSuccess();
	}
	else if (strAIsPrefix("rtcache", arg)) {
		String darg = cmdDGetOptArg(arg, "rtcache", NULL);
		long rtsize;
		extern void gen0SetRtCacheSize(AInt);

		sscanf(darg, "%ld", &rtsize);
		gen0SetRtCacheSize((AInt)rtsize);
	}
	else if (strAIsPrefix("D", arg)) {
		char	sep;
		String  darg = cmdDGetOptArg(arg, "D", &sep);
		rc   = cmdDDebug(darg, sep == '+');
	}
	else if (strAIsPrefix("T", arg)) {
		rc = phTraceOption(arg+strlen("T"));
	}
	else if (strAIsPrefix("name", arg)) {
		String darg = cmdDGetOptArg(arg, "name", NULL);
		emitSetFileIdName(darg);
	}
	else if (strAIsPrefix("prefix", arg)) {
		String darg = cmdDGetOptArg(arg, "prefix", NULL);
		emitSetFileIdPrefix(darg);
	}
	else if (strAEqual("floatrep", arg)) {
		cmdFloatRepFlag = true;
	}
	else
		rc = -1;

	return rc;
}
 
 
local String
cmdDGetOptArg(String arg, String prefix, char *psep)
{
	if (!strAIsPrefix(prefix, arg))
		cmdUseError(ALDOR_F_CmdBadOption, "-W");
	arg += strlen(prefix);
	if (*arg != '+' && *arg != '-' && *arg != '=' && *arg != ':')
		cmdUseError(ALDOR_F_CmdBadOption, "-W");

	if (psep) *psep = *arg;
	arg++;

	if (!*arg)
		cmdUseError(ALDOR_F_CmdBadOption, "-W");
	return arg;
}


local void
cmdDoKeywordStatus(String keyword, Bool enable)
{
	/* Turn keywords into normal identifiers */
	TokenTag	tag = keyTag(keyword);


	/* Is this actually a keyword? */
	if (tag != TK_LIMIT)
		keySetDisabled(tag, !enable);
	else
		cmdUseError(ALDOR_W_DisableNotKeyword, keyword);
}
 
 
/*****************************************************************************
 *
 * :: Setting of debug variables.
 *
 ****************************************************************************/
 
extern Bool
	abDebug, abExpandDebug, ablogDebug,
	abnDefineDebug, abnWhereDebug, abnWithMergeDebug, abnormDebug,
	absDebug, arDebug,
	cfoldDebug, condApplyDebug, cpDebug, cpDfDebug,
        cseDebug, cseDfDebug, cseDfiDebug, daDebug,
        dnfDebug, dvDebug, emergeDebug, fileDebug,
        fintDebug, fintLinkDebug, fintStoDebug,
	flogDebug, foamConstDebug, foamDebug, foamSposDebug, 
	fortranTypesDebug, fvDebug,
	genfImplicitDebug,
	genfDebug, genfHashDebug, genfConstDebug, genfExportDebug,
	gfaddDebug,
	implDebug, inlCallDebug,
	inlCallInfoDebug, inlExportDebug, inlExprDebug, inlExtendDebug,
	inlProgDebug, inlTransDebug, inlUnitDebug, inlineDebug, 
	jflowCatDebug, jflowDfDebug, jflowDfiDebug, jflowGoDebug, jflowDmDebug,
	kpDebug,
	libConstDebug, libDebug, libLazyDebug, libRepDebug, libVerboseDebug,
	linDebug,
	macDebug, oeDebug,
	optfDebug, peepDebug,
	retDebug, rrfmtDebug,
	scoDebug, scoFluidDebug, scoStabDebug, scoUndoDebug,
	sefoCloseDebug, sefoEqualDebug, sefoFreeDebug,
	sefoPrintDebug, sefoSubstDebug, sefoUnionDebug, sefoInterDebug,
	sexprDebug, sstDebug, sstMarkDebug,
	stabConstDebug, stabDebug, stabImportDebug,
	symeDebug, symeFillDebug, symeHasDebug,
	symeRefreshDebug,
	tcDebug,
	tfDebug, tfCascadeDebug, tfCatDebug, tfCrossDebug,
	tfExprDebug, tfFloatDebug, tfHasDebug, tfImportDebug, tfMapDebug,
	tfMultiDebug, tfParentDebug, tfWithDebug,
	tfsDebug, tfsExportDebug, tfsMultiDebug, tfsParentDebug,
	tipAddDebug, tipApplyDebug, tipAssignDebug, tipBupDebug,
	tipDeclDebug,
	tipDeclareDebug, tipDefineDebug, tipExtendDebug,
	tipFarDebug, tipIdDebug,
	tipLitDebug, tipEmbedDebug, tipSefDebug, tipTdnDebug, 
	titfDebug, titfOneDebug, titfStabDebug,
	udDfDebug, udDfiDebug;
 
struct dbVarInfo {
	Bool	*pvar;
	String	name;
};
 
struct dbVarInfo dbVarInfo[] = {
	{ & abDebug,		"ab" },
	{ & abExpandDebug,	"abExpand" },
	{ & ablogDebug,		"ablog" },
	{ & abnDefineDebug,	"abnDefine" },
	{ & abnWhereDebug,	"abnWhere" },
	{ & abnWithMergeDebug,	"abnWithMerge" },
	{ & abnormDebug,	"abnorm" },
	{ & absDebug,		"abs" },
	{ & arDebug,		"ar" },
	{ & cfoldDebug,		"cfold" },
	{ & condApplyDebug,	"condApply" },
 	{ & cpDebug,		"cp" },
 	{ & cpDfDebug,		"cpDf" },
 	{ & cseDebug,		"cse" },
 	{ & cseDfDebug,		"cseDf" },
 	{ & cseDfiDebug,	"cseDfi" },
	{ & daDebug,		"da" },
	{ & dnfDebug,		"dnf" },
	{ & dvDebug,		"dv" },
	{ & emergeDebug,	"emerge" },
	{ & fileDebug,		"file" },
	{ & fintDebug,		"fint" },
	{ & fintLinkDebug,	"fintLink" },
	{ & fintStoDebug,	"fintSto" },
	{ & flogDebug,          "flog" },
	{ & foamConstDebug,	"foamConst" },
	{ & foamDebug,		"foam" },
	{ & foamSposDebug,	"foamSpos" },
	{ & fortranTypesDebug,	"fortranTypes" },
	{ & fvDebug,		"fv" },
	{ & genfExportDebug,	"genfExport" },
	{ & genfImplicitDebug,	"genfImplicit" },
	{ & genfDebug,		"genf"},
	{ & genfHashDebug,	"genfHash"},
	{ & genfConstDebug,	"genfConst"},
	{ & gfaddDebug,		"gfadd"},
	{ & implDebug,		"impl" },
	{ & inlCallDebug,	"inlCall" },
	{ & inlCallInfoDebug,	"inlCallInfo" },
	{ & inlExportDebug,	"inlExport" },
	{ & inlExprDebug,	"inlExpr" },
	{ & inlExtendDebug,	"inlExtend" },
	{ & inlProgDebug,	"inlProg" },
	{ & inlTransDebug,	"inlTrans" },
	{ & inlUnitDebug,	"inlUnit" },
	{ & inlineDebug,	"inline" },
	{ & jflowCatDebug,	"jflowCat" },
	{ & jflowDfDebug,	"jflowDf" },
	{ & jflowDfiDebug,	"jflowDfi" },
	{ & jflowGoDebug,	"jflowGo" },
	{ & jflowDmDebug,	"jflowDm" },
	{ & kpDebug,		"killp" },
	{ & libConstDebug,	"libConst" },
	{ & libDebug,		"lib" },
	{ & libLazyDebug,	"libLazy" },
	{ & libRepDebug,	"libRep" },
	{ & libVerboseDebug,	"libVerbose" },
	{ & linDebug,           "lin" },
	{ & macDebug,           "macex" },
	{ & oeDebug,          "oe" },
	{ & optfDebug,          "optf" },
	{ & peepDebug,          "peep" },
	{ & retDebug,		"ret" },
	{ & rrfmtDebug,		"rrfmt" },
	{ & scoDebug,		"sco" },
	{ & scoFluidDebug,	"scoFluid" },
	{ & scoStabDebug,	"scoStab" },
	{ & scoUndoDebug,	"scoUndo" },
	{ & sefoCloseDebug,	"sefoClose" },
	{ & sefoEqualDebug,	"sefoEqual" },
	{ & sefoFreeDebug,	"sefoFree" },
	{ & sefoInterDebug,	"sefoInter" },
	{ & sefoPrintDebug,	"sefoPrint" },
	{ & sefoSubstDebug,	"sefoSubst" },
	{ & sefoUnionDebug,	"sefoUnion" },
	{ & sexprDebug,    	"sexpr" },
	{ & sstDebug,		"sst" },
	{ & sstMarkDebug,	"sstMark" },
	{ & stabConstDebug,	"stabConst" },
	{ & stabDebug,		"stab" },
	{ & stabImportDebug,	"stabImport" },
	{ & symeDebug,		"syme" },
	{ & symeFillDebug,	"symeFill" },
	{ & symeHasDebug,	"symeHas" },
	{ & symeRefreshDebug,	"symeRefresh" },
	{ & tcDebug,		"tc" },
	{ & tfDebug,		"tf" },
	{ & tfCascadeDebug,	"tfCascade" },
	{ & tfCatDebug,		"tfCat" },
	{ & tfCrossDebug,	"tfCross" },
	{ & tfExprDebug,	"tfExpr" },
	{ & tfFloatDebug,	"tfFloat" },
	{ & tfHasDebug,		"tfHas" },
	{ & tfImportDebug,	"tfImport" },
	{ & tfMapDebug,		"tfMap" },
	{ & tfMultiDebug,	"tfMulti" },
	{ & tfParentDebug,	"tfParent" },
	{ & tfWithDebug,	"tfWith" },
	{ & tfsDebug,		"tfs" },
	{ & tfsExportDebug,	"tfsExport" },
	{ & tfsMultiDebug,	"tfsMulti" },
	{ & tfsParentDebug,	"tfsParent" },
	{ & tipAddDebug,	"tipAdd" },
	{ & tipApplyDebug,	"tipApply" },
	{ & tipAssignDebug,	"tipAssign" },
	{ & tipBupDebug,	"tipBup" },
	{ & tipDeclDebug,	"tipDecl" },
	{ & tipDeclareDebug,	"tipDeclare" },
	{ & tipDefineDebug,	"tipDefine" },
	{ & tipExtendDebug,	"tipExtend" },
	{ & tipFarDebug,	"tipFar" },
	{ & tipIdDebug,		"tipId" },
	{ & tipLitDebug,	"tipLit" },
	{ & tipEmbedDebug,	"tipEmbed" },
	{ & tipSefDebug,	"tipSef" },
	{ & tipTdnDebug,	"tipTdn" },
	{ & titfDebug,		"titf" },
	{ & titfOneDebug,	"titfOne" },
	{ & titfStabDebug,	"titfStab" },
	{ & udDfDebug,		"udDf" },
	{ & udDfiDebug,		"udDfi" },
	{ 0,			0}
};
 
 
local int
cmdDDebug(String dbName, Bool sense)
{
	int	i;

	if (strAEqual(dbName, "show")) {
		printf("\nThe debug hooks are:");
		for (i = 0; dbVarInfo[i].name; i++)
			printf(" \"%s\"", dbVarInfo[i].name);
		printf("\n");
		return 0;
	}

	if (strAEqual(dbName, "all")) {
		for (i = 0; dbVarInfo[i].name; i++)
			*dbVarInfo[i].pvar = sense;
		return 0;
	}

	for (i = 0; dbVarInfo[i].name; i++) {
		if (strAEqual(dbName, dbVarInfo[i].name)) {
			*(dbVarInfo[i].pvar) = sense;
			return 0;
		}
	}

	return -1;
}

void 
cmdDebugReset()
{
	int i;
	for (i = 0; dbVarInfo[i].name; i++) {
		*dbVarInfo[i].pvar = 0;
	}
}
