/******************************************************************************
 *
 * testaldor.c
 *
 * Test driver for the Aldor Compiler.
 *
 *****************************************************************************/

#include "platform.h"
#include "cconfig.h"

#include "signal.h0"
#include "stdarg.h0"
#include "stddef.h0"
#include "stdio.h0"
#include "string.h0"
#include "stdlib.h0"

#include "testaldor.h"
#include "tx_opsys.c"

/******************************************************************************
 *
 * Global variables
 *
 *****************************************************************************/

#define		BUFLEN			1024

String		OldCurDir;		/* Old current directory. */
String		TmpDir;			/* Temporary output directory. */
String		OutDir;			/* Test output directory. */

String		Aldor;			/* Aldor program name. */
String		OAldor;			/* Old Aldor program name. */
String		CC;			/* C Compiler program name. */

String		Kind;			/* Test kinds to run. */
Int		KindAct;		/* All 'Kind' or all but 'Kind'. */
String		Action;			/* What to do with each test. */

Int		failc = 0;		/* Number of failed tests. */
String		failv[BUFLEN];		/* Vector of failed tests. */

int             debug;
/******************************************************************************
 *
 * Aldor test functions
 *
 *****************************************************************************/

void		testAldorHelp		_OF((void));
void		testAldorClean		_OF((String fn));
void		testAldorScript		_OF((String fn));
void		testAldorDispatch	_OF((String args, String fn));
void		testAldorCompile	_OF((String args, String fn));
void		testAldorRun		_OF((String args, String fn));
void		testAldorInterpret	_OF((String args, String fn));
void		testAldorErrors		_OF((String args, String fn));
void		testAldorPhase		_OF((String args, String fn));
void		testAldorGenerate		_OF((String args, String fn));

String testAldorOutDirForTest _OF((String fname));
/******************************************************************************
 *
 * Aldor test top level
 *
 *****************************************************************************/

void		testAldorInit		_OF((void));
void		testAldorCleanup	_OF((void));
void		testAldorFini		_OF((int));
Length		testAldorArgs		_OF((Length, String *));
void		testAldorGlean		_OF((Length, String *));
Int		testAldorFilter		_OF((String));

/******************************************************************************
 *
 * String utilities
 *
 *****************************************************************************/

String
strAlloc(cc)
	Length	cc;
{
	String	s = (String) malloc(cc + 1);

	s[0]  = '\0';
	s[cc] = '\0';

	return s;
}

Length
strLength(s)
	String	s;
{
	return strlen(s);
}

String
strCopy(s)
	String	s;
{
	Length	cc = strLength(s);
	String	t = strAlloc(cc);

	strcpy(t, s);
	return t;
}

String
strConcat(s, t)
	String	s;
	String	t;
{
	Length	cc = strLength(s) + strLength(t);
	String	r = strAlloc(cc);

	strcpy(r, s);
	strcat(r, t);
	return r;
}

#if defined(CC_no_prototype)
String
strPrintf(fmt, va_alist)
	String	fmt;
	int	va_alist;
#else
String
strPrintf(String fmt, ...)
#endif
{
	Length	cc = BUFLEN;
	String	s;
	va_list	argp;

	s = strAlloc(cc);
	va_start(argp, fmt);
	vsprintf(s, fmt, argp);
	va_end(argp);

	return s;
}

Int
strEqual(s, t)
	String	s;
	String	t;
{
	return strcmp(s, t) == 0;
}

Int
strIsPrefix(pre, s)
	String	pre;
	String	s;
{
	Int	c0, cc;

	c0 = strLength(pre);
	cc = strLength(s);
	if (cc < c0) return 0;
	return strncmp(pre, s, c0) == 0;
}

Int
strIsSuffix(suf, s)
	String	suf;
	String	s;
{
	Int	c0, cc;

	c0 = strLength(suf);
	cc = strLength(s);
	if (cc < c0) return 0;
	return strncmp(suf, s + cc - c0, c0) == 0;
}

/******************************************************************************
 *
 * Aldor test functions
 *
 *****************************************************************************/

String	HelpMsg[] = {
	"testAldor [option...] [dir|file..]",
	"",
	"    The possible options are:",
	"        -help       Display this information",
	"        -show       Run the tests and display the output",
	"        -install    Run the tests and install the output as correct",
	"        -oldaldor xx  Run the tests, using xx to display old .ao files",
	"        -aldor xx  Run the tests, using xx as aldor executable",
	"        -cc xx     Run the tests, using xx as unicl executable",
	"        -only kk    Run only tests of kind kk, where kk may be",
	"		         one of test{script,comp,run,errs,phase,gen,int}.",
	"        -but kk     Run all the tests, except those of kind kk.",
	"",
	"    If files are given as args, they are the tests to be used.",
	"    If a directory is given as an arg, then all the test files",
	"        in it are used. (.as .sh files)",
	"    Otherwise, all the test files in the current directory",
	"        are used. (.as .sh files)",
	"",
	"    .as files contain one or more lines like:",
	"        --> testcomp          (compile and compare byte-code output)",
	"        --> testrun           (compile, run and compare output)",
	"        --> testint           (interpret and compare output)",
	"        --> testerrs          (-D TestErrorsToo and compare msgs)",
	"        --> testphase phname  (trace phase and compare output)",
	"        --> testgen   ftype   (for generated files of a given type)",
	"",
	"    .sh files do not contain any special lines."
};

/*
 * Display help message.
 */
void
testAldorHelp()
{
	Length	i, msgc = sizeof(HelpMsg)/sizeof(HelpMsg[0]);

	for (i = 0; i < msgc; i += 1) {
		fputs(HelpMsg[i], stdout);
		fnewline(stdout);
	}
}

/*
 * Remove all generated files.
 */

String	Ext[] = { "ax", "ai", "ao", "fm", "asy", "l", "c", "o" };

void
testAldorClean(fn)
	String	fn;
{
	String	bn = osFileBase(fn);
	String	on;
	Length	i, extc = sizeof(Ext)/sizeof(Ext[0]);

	osSetCurDir(TmpDir);

	for (i = 0; i < extc; i += 1) {
		on = strPrintf("%s.%s", bn, Ext[i]);
		osFileRemove(on);
	}
	osFileRemove(bn);

	osSetCurDir(OldCurDir);
}

/*
 * Test .sh files.
 */
void
testAldorScript(fn)
	String	fn;
{
	String	bn = osFileBase(fn);
	String	fname = strConcat(bn, ".sh");
	String	Rname = strConcat(bn, ".shR");
	String	TRname = osFileCombine(TmpDir, Rname);
	String	ORname = osFileCombine(OutDir, Rname);

	fprintf(stdout, ">> %s: (script) ", fname);
	fflush(stdout);

	osPutEnv(strPrintf("TMPDIR=%s", TmpDir));
	if (osRunScript(fname, TRname) != 0) {
		fprintf(stdout, "ERROR\n");
		failv[failc++] = fname;
	}
	else if (strEqual(Action, "show")) {
		fnewline(stdout);
		osFileCat(TRname);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", ORname);
		osFileCopy(TRname, ORname);
	}
	else if (osFileEqual(ORname, TRname)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "DIFFERENT\n");
		osShowDiff(ORname, TRname);
		failv[failc++] = fname;
	}

	osFileRemove(TRname);
	osSetCurDir(OldCurDir);
}

/*
 * Dispatch function for .as tests.
 */
typedef void	(*TaxFun)	_OF((String, String));

struct tax_info {
	String		tag;
	TaxFun		fn;
};

struct tax_info taxInfoTable[] = {
	{ "testcomp",	testAldorCompile },
	{ "testrun",	testAldorRun },
	{ "testint",	testAldorInterpret },
	{ "testerrs",	testAldorErrors },
	{ "testphase",	testAldorPhase },
	{ "testgen",	testAldorGenerate }
};

#define	TaxInfoTag(i)	(taxInfoTable[i].tag)
#define TaxInfoFun(i)	(taxInfoTable[i].fn)

void
testAldorDispatch(args, fn)
	String	args;
	String	fn;
{
	Length	i, taxc = sizeof(taxInfoTable) / sizeof(taxInfoTable[0]);

	for (i = 0; i < taxc; i += 1)
		if (strIsPrefix(TaxInfoTag(i), args)) {
			args += strlen(TaxInfoTag(i));
			TaxInfoFun(i)(args, fn);
			testAldorClean(fn);
			return;
		}
}

/*
 * For .as files containing a line '--> testcomp'
 */
void
testAldorCompile(args, fn)
	String	args;
	String	fn;
{
	String	bn = osFileBase(fn);
	String	fname = strConcat(bn, ".as");
	String	Bname = strConcat(bn, ".ao");
	String	Sname = strConcat(bn, ".fm");
	String	TBname = osFileCombine(TmpDir, Bname);
	String	TSname = osFileCombine(TmpDir, Sname);
	String	OBname = osFileCombine(OutDir, Bname);
	String	TNSname = osFileCombine(TmpDir, strPrintf("new-%s", Sname));
	String	TSSname = osFileCombine(TmpDir, strPrintf("std-%s", Sname));
	String	cmd;

	fprintf(stdout, ">> %s: (compilation) ", fname);
	fflush(stdout);

	cmd = strPrintf("%s -R %s %s %s",
			Aldor, TmpDir, args, fname);
	if (osRunOutput(cmd, NULL) != 0) {
		fprintf(stdout, "ERROR\n");
		failv[failc++] = fname;
	}
	else if (strEqual(Action, "show")) {
		fnewline(stdout);
		cmd = strPrintf("%s -R %s -F fm %s", Aldor, TmpDir, TBname);
		osRunOutput(cmd, NULL);
		osFileCat(TSname);
		osFileRemove(TSname);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", OBname);
		osFileCopy(TBname, OBname);
	}
	else if (osFileEqual(OBname, TBname)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "(not identical) ");
		fflush(stdout);

		cmd = strPrintf("%s -R %s -F fm %s",
				Aldor, TmpDir, TBname);
		osRunOutput(cmd, NULL);
		osFileCopy(TSname, TNSname);
		osFileRemove(TSname);

		cmd = strPrintf("%s -R %s -F fm %s",
				OAldor, TmpDir, OBname);
		osRunOutput(cmd, NULL);
		osFileCopy(TSname, TSSname);
		osFileRemove(TSname);

		if (osFileEqual(TSSname, TNSname)) {
			fprintf(stdout, "OK\n");
		}
		else {
			fprintf(stdout, "DIFFERENT\n");
			osShowDiff(TSSname, TNSname);
			failv[failc++] = fname;
		}
		osFileRemove(TNSname);
		osFileRemove(TSSname);
	}

	osFileRemove(TBname);
	osSetCurDir(OldCurDir);
}

/*
 * For .as files containing a line '--> testrun'
 */
void
testAldorRun(args, fn)
	String	args;
	String	fn;
{
	String	bn = osFileBase(fn);
	String	fname = strConcat(bn, ".as");
	String	Tname = strConcat(bn, ".out");
	String	OTname = osFileCombine(OutDir, Tname);
	String	Pfname = osFileCombine(OldCurDir, fname);
	String	POname = osFileCombine(OldCurDir, OTname);
	String	cmd;

	fprintf(stdout, ">> %s: (execution) ", fname);
	fflush(stdout);

	osSetCurDir(TmpDir);

	/* Requires 1.1.13(12) */
	cmd = strPrintf("%s -M base=%s%s -Ccc=\"%s\" -grun %s %s",
			Aldor, OldCurDir, OS_PATH_SEP, CC, args, Pfname);
	/*fprintf(stdout, "cmd: (%s) %s\n", getcwd(0), cmd);*/
	if (osRunOutput(cmd, Tname) != 0) {
		fprintf(stdout, "ERROR\n");
		failv[failc++] = fname;
		osFileCat(Tname);
	}
	else if (strEqual(Action, "show")) {
		fnewline(stdout);
		osFileCat(Tname);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", OTname);
		osFileCopy(Tname, POname);
	}
	else if (osFileEqual(POname, Tname)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "DIFFERENT\n");
		fflush(stdout);
		osShowDiff(POname, Tname);
		failv[failc++] = fname;
	}

	osFileRemove(Tname);
	osSetCurDir(OldCurDir);
}

/*
 * For .as files containing a line '--> testint'
 */
void
testAldorInterpret(args, fn)
	String	args;
	String	fn;
{
	String	bn = osFileBase(fn);
	String	fname = strConcat(bn, ".as");
	String	Tname = strConcat(bn, ".out");
	String	OTname = osFileCombine(OutDir, Tname);
	String	Pfname = osFileCombine(OldCurDir, fname);
	String	POname = osFileCombine(OldCurDir, OTname);
	String	cmd;

	fprintf(stdout, ">> %s: (interpreting) ", fname);
	fflush(stdout);

	osSetCurDir(TmpDir);

	/* Requires 1.1.13(12) */
	cmd = strPrintf("%s -M base=%s%s -ginterp %s %s",
			Aldor, OldCurDir, OS_PATH_SEP, args, Pfname);

	if (osRunOutput(cmd, Tname) != 0) {
		fprintf(stdout, "ERROR\n");
		failv[failc++] = fname;
		osFileCat(Tname);
	}
	else if (strEqual(Action, "show")) {
		fnewline(stdout);
		osFileCat(Tname);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", OTname);
		osFileCopy(Tname, POname);
	}
	else if (osFileEqual(POname, Tname)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "DIFFERENT\n");
		osShowDiff(POname, Tname);
		failv[failc++] = fname;
	}

	osFileRemove(Tname);
	osSetCurDir(OldCurDir);
}

/*
 * For .as files containing a line '--> testerrs'
 */
void
testAldorErrors(args, fn)
	String	args;
	String	fn;
{
	String	bn = osFileBase(fn);
	String	fname = strConcat(bn, ".as");
	String	Ename = strConcat(bn, ".E");
	String	Bname = strConcat(bn, ".ao");
	String	TEname = osFileCombine(TmpDir, Ename);
	String	OEname = osFileCombine(OutDir, Ename);
	String	TBname = osFileCombine(TmpDir, Bname);
	String	cmd;

	fprintf(stdout, ">> %s: (error report) ", fname);
	fflush(stdout);

	cmd = strPrintf("%s -M2 -D TestErrorsToo -R %s %s %s",
			Aldor, TmpDir, args, fn);
	osRunOutput(cmd, TEname);

	if (strEqual(Action, "show")) {
		fnewline(stdout);
		osFileCat(TEname);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", OEname);
		osFileCopy(TEname, OEname);
	}
	else if (osFileEqual(OEname, TEname)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "DIFFERENT\n");
		osShowDiff(OEname, TEname);
		failv[failc++] = fname;
	}

	osFileRemove(TEname);
	osFileRemove(TBname);
	osSetCurDir(OldCurDir);
}

/*
 * For .as files containing a line '--> testphase phase-name'
 */
void
testAldorPhase(args, fn)
	String	args;
	String	fn;
{
	String	bn = osFileBase(fn);
	String	fname = strConcat(bn, ".as");
	String	Rname = strConcat(bn, ".R");
	String	Bname = strConcat(bn, ".ao");
	String	TRname = osFileCombine(TmpDir, Rname);
	String	ORname = osFileCombine(OutDir, Rname);
	String	TBname = osFileCombine(TmpDir, Bname);
	String	cmd;

	while (args[0] == ' ') args++;

	fprintf(stdout, ">> %s: (phase %s) ", fname, args);
	fflush(stdout);

	cmd = strPrintf("%s -WTrt+%s -R %s %s",
			Aldor, args, TmpDir, fname);
	osRunOutput(cmd, TRname);

	if (strEqual(Action, "show")) {
		fnewline(stdout);
		osFileCat(TRname);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", ORname);
		osFileCopy(TRname, ORname);
	}
	else if (osFileEqual(ORname, TRname)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "DIFFERENT\n");
		osShowDiff(ORname, TRname);
		failv[failc++] = fname;
	}

	osFileRemove(TRname);
	osFileRemove(TBname);
	osSetCurDir(OldCurDir);
}

/*
 * For .as files containing a line  '--> testgen ftype'
 * It gets called as: testgen flet fname.as, where flet is one of i a o y f l c
 */
void
testAldorGenerate(args, fn)
	String	args;
	String	fn;
{
	String	bn = osFileBase(fn);
	String	fname = strConcat(bn, ".as");
	String	Bname = strConcat(bn, ".ao");
	String	TBname = osFileCombine(TmpDir, Bname);
	String	Gname, TGname, OGname;
	String	fext, cmd;

	while (args[0] == ' ') args++;

	switch (args[0]) {
	case 'i': fext = "ai";  break;
	case 'a': fext = "ax";  break;
	case 'o': fext = "ao";  break;
	case 'y': fext = "asy"; break;
	case 'f': fext = "fm";  break;
	case 'c': fext = "c";   break;
	case 'l': fext = "lsp"; break;
	default:  fext = "unk"; break;
	}
	args++;
	while (args[0] == ' ') args++;

	fprintf(stdout, ">> %s: (generation .%s) ", fname, fext);
	fflush(stdout);

	Gname = strPrintf("%s.%s", bn, fext);
	TGname = osFileCombine(TmpDir, Gname);
	OGname = osFileCombine(OutDir, Gname);

	cmd = strPrintf("%s -F %s -R %s %s %s",
			Aldor, fext, TmpDir, args, fn);

	if (osRunOutput(cmd, NULL) != 0) {
		fprintf(stdout, "ERROR\n");
		failv[failc++] = fname;
	}
	else if (strEqual(Action, "show")) {
		fnewline(stdout);
		osFileCat(TGname);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", OGname);
		osFileCopy(TGname, OGname);
	}
	else if (osFileEqual(OGname, TGname)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "DIFFERENT\n");
		osShowDiff(OGname, TGname);
		failv[failc++] = fname;
	}

	osFileRemove(TGname);
	osFileRemove(TBname);
	osSetCurDir(OldCurDir);
}

/******************************************************************************
 *
 * Aldor test top level
 *
 *****************************************************************************/

void
testAldorInit()
{
	Length	cc = BUFLEN, rc;
	String	buf = strAlloc(cc);

	rc = osGetCurDir(buf, cc);
	if (rc != 0) exitFailure();

	OldCurDir = strCopy(buf);
	TmpDir    = osTempDirName();
	OutDir    = osFileCombine("..", "testout");

	Aldor   = "aldor ";
	OAldor  = "aldor ";
	CC        = "unicl";

	Kind      = "all";
	KindAct   = 1;
	Action    = "compare";

	if (signal(SIGINT,  testAldorFini) == SIG_ERR) exitFailure();
	if (signal(SIGABRT, testAldorFini) == SIG_ERR) exitFailure();
	if (signal(SIGTERM, testAldorFini) == SIG_ERR) exitFailure();
#if SIGQUIT != SIGFAKE
	if (signal(SIGQUIT, testAldorFini) == SIG_ERR) exitFailure();
#endif

	osMakeDir(TmpDir);
}

void
testAldorCleanup()
{
	osRemoveDir(TmpDir);
}

void
testAldorFini(sig)
	int	sig;
{
	fprintf(stdout, "testAldor:  Removing temporary directory %s\n", TmpDir);
	osSetCurDir(OldCurDir);
	testAldorCleanup();
	exitFailure();
}

Length
testAldorArgs(argc, argv)
	Length	argc;
	String	*argv;
{
	Length	i;

	for (i = 1; i < argc; i += 1) {
		if (strEqual(argv[i], "-help")) {
			testAldorHelp();
			exitSuccess();
		}
		else if (strEqual(argv[i], "-compare"))
			Action = "compare";
		else if (strEqual(argv[i], "-debug"))
			debug = 1;
		else if (strEqual(argv[i], "-install"))
			Action = "install";
		else if (strEqual(argv[i], "-show"))
			Action = "show";
		else if (strEqual(argv[i], "-oldaldor"))
			OAldor = strCopy(argv[++i]);
		else if (strEqual(argv[i], "-aldor"))
			Aldor = strCopy(argv[++i]);
		else if (strEqual(argv[i], "-cc"))
			CC = strCopy(argv[++i]);
		else if (strEqual(argv[i], "-only")) {
			Kind = strCopy(argv[++i]);
			KindAct = 1;
		}
		else if (strEqual(argv[i], "-but")) {
			Kind = strCopy(argv[++i]);
			KindAct = 0;
		}
		else
			break;
	}

	return i;
}

void
testAldorGlean(argc, argv)
	Length	argc;
	String	*argv;
{
	Length	i, cc = BUFLEN;
	String	buf = strAlloc(cc);

	for (i = 0; i < argc; i += 1) {
		if (!osFileIsThere(argv[i])) continue;
		OutDir = testAldorOutDirForTest(argv[i]);
		fprintf(stderr, "Outdir: %s\n", OutDir);
		if (strIsSuffix(".as", argv[i])) {
			FILE *	argi;
			String	key = "\n--> ";
			Int	state = 1, j = 0;
			Int	c, k;

			argi = fopen(argv[i], "r");
			if (argi == NULL) exitFailure();

			while ((c = fgetc(argi)) != EOF) {
				if ((k = key[state]) != '\0') {
					if (c == k)
						state += 1;
					else if (c == key[0])
						state = 1;
					else
						state = 0;
				}
				else if (c == '\n') {
					buf[j] = '\0';
					if (testAldorFilter(buf)) {
						testAldorDispatch(buf, argv[i]);
					}
					state = 1;
					j = 0;
				}
				else
					buf[j++] = c;
			}
			fclose(argi);
		}
		else if (strIsSuffix(".sh", argv[i])) {
			if (testAldorFilter("testscript"))
				testAldorScript(argv[i]);
		}
		free(OutDir);
		OutDir = NULL;
	}
}

String
testAldorOutDirForTest(fname)
	String fname;
{
	String dir = osFileDirName(fname);
	String base = osFileBase(fname);
	if (dir == NULL) {
		return osFileCombine("..", "testout");
	}
	else {
		String backOne = osFileCombine(dir, "..");
		String outDir = osFileCombine(backOne, "testout");
		free(backOne);
		return outDir;
	}
}


Int
testAldorFilter(buf)
	String	buf;
{
	if (strEqual(Kind, "all"))
		return 1;
	else
		return KindAct == (strncmp(buf, Kind, strlen(Kind)) == 0);
}

int
main(argc, argv)
	int	argc;
	String	*argv;
{
	Length	i;

	testAldorInit();

	i = testAldorArgs(argc, argv);
	argc -= i;
	argv += i;
	testAldorGlean(argc, argv);

	if (failc == 0) {
		fprintf(stdout, ">> No tests failed\n");
		testAldorCleanup();
		exitSuccess();
	}
	else {
		fprintf(stdout, ">> Failed tests:");
		for (i = 0; i < failc; i += 1)
			fprintf(stdout, " %s", failv[i]);
		fnewline(stdout);
		testAldorCleanup();
		exitFailure();
	}
}
