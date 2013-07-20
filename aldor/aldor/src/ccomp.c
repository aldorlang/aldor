/*****************************************************************************
 *
 * ccomp.c:  C compiler interface.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "ccomp.h"
#include "compcfg.h"
#include "emit.h"
#include "genc.h"
#include "opsys.h"
#include "store.h"

/*
 * Default compiler and linker.  
 * They can be overridden by the env. variable CC or the option * -Ccc=....
 * They can be overridden individually with  -Ccomp=... and -Clink-....
 */
#define CC_DEFAULT	"unicl"
#define LD_DEFAULT	"unicl"
#define RUNTIME_DEFAULT "foam"

/*****************************************************************************
 *
 * :: C compiler + Linker Interface
 *
 ****************************************************************************/

enum ccoptTag {
    CCOPT_START,
	CCOPT_IncPath = CCOPT_START,
	CCOPT_Define,
	CCOPT_Undefine,
	CCOPT_CompileOnly,
	CCOPT_Optimize,
        CCOPT_FNonStd,
	CCOPT_DebugInfo,
	CCOPT_ProfileInfo,
	CCOPT_LibPath,
	CCOPT_Library,
	CCOPT_OutputFile,
        CCOPT_System,
        CCOPT_Fortran,
        CCOPT_Runtime,
    CCOPT_LIMIT
};

struct ccOption {
	enum ccoptTag	tag;
	String		arg;
	struct ccOption *next;
};

struct ccOptionInfo {
	enum ccoptTag	tag;
	String		text;

	BPack(Bool)	isComp;
	BPack(Bool)	isLink;
	BPack(Bool)	isDir;
	BPack(Bool)	isFile;
};

/*
 * These options are valid on Unix C compilers *and* the "unicl" wrapper.
 * The extra -WV option for "unicl" is passed via the environment.
 * [As we make a vague attempt to call a standard C compiler, we can't 
 *  pass it through the command line 
 */
struct ccOptionInfo ccOptionTable[] = {
	/*  Tag            Text        C L D F */
	{CCOPT_IncPath,    "-I",       1,0,1,0},
	{CCOPT_Define,	   "-D",       1,0,0,0},
	{CCOPT_Undefine,   "-U",       1,0,0,0},
	{CCOPT_CompileOnly,"-c",       1,0,0,0},
	{CCOPT_Optimize,   "-O",       1,1,0,0},
	/* -Wfnonstd is not a valid option to gcc or to ld anymore */
	{CCOPT_FNonStd,    "-Wfnonstd",0,0,0,0},
	{CCOPT_DebugInfo,  "-g",       1,1,0,0},
	{CCOPT_ProfileInfo,"-p",       1,1,0,0},
	{CCOPT_LibPath,    "-L",       0,1,1,0},
	{CCOPT_Library,    "-l",       0,1,0,0},
	{CCOPT_OutputFile, "-o ",      0,1,0,1},   /* Note trailing blank. */
	{CCOPT_System, 	   "-Wsys=",   1,1,0,0},
};

#define ccIsCompilerOption(tag) (ccOptionTable[tag].isComp)
#define ccIsLinkerOption(tag)   (ccOptionTable[tag].isLink)
#define ccIsDirOption(tag)	(ccOptionTable[tag].isDir)
#define ccIsFileOption(tag)	(ccOptionTable[tag].isFile)


/****************************************************************************
 *
 * :: Option Handling
 *
 ***************************************************************************/

local void ccPushLibrary	(String);

local void 	ccPushOptionList(enum ccoptTag, String);
local void	ccPushOption	(enum ccoptTag, String);

/*
 * Remember to change comsgdb.msg if these change.
 */
#define COPT_STANDARDC	"standard"
#define COPT_OLDC	"old"
#define COPT_IDHASH	"idhash"
#define COPT_NOIDHASH	"no-idhash"
#define COPT_LINENOS	"lines"
#define COPT_NOLINENOS	"no-lines"
#define COPT_COMP	"comp"
#define COPT_LIB	"lib"
#define COPT_LINK	"link"
#define COPT_CC		"cc"
#define COPT_ARGS	"args"
#define COPT_GO		"go"
#define COPT_SMAX	"smax"
#define COPT_IDLEN	"idlen"
#define COPT_FNAME	"fname"
#define COPT_SYS	"sys"
#define COPT_FORTRAN	"fortran"
#define COPT_RUNTIME	"runtime"

/*
 * Control C Code generation and compilation.
 */
String		ccCompiler   = 0;
String		ccLinker     = 0;
String		ccGoer	     = 0;
String		ccOptions    = 0;
String 		ccSystem     = 0;
String 		ccRuntime    = 0;
Bool 		ccFortran    = 0;
Bool		ccVerboseFlag= false;
Bool		ccLineNosFlag= false;
int		ccDoStandardCFlag = -1;
StringList      ccLibraries;
String          ccRoot;

local void ccSetStandardC(Bool flg);

void ccSetRoot(String root)
{
	ccRoot = root;
}

int
ccOption(String opt)
{
	String	s;

	if (!*opt) return -1;		       /* Must have letters */

	if      (strAEqual(opt, COPT_STANDARDC))	ccSetStandardC(true);
	else if (strAEqual(opt, COPT_LINENOS))   	ccSetLineNos(true);
	else if (strAEqual(opt, COPT_NOLINENOS))   	ccSetLineNos(false);
	else if (strAEqual(opt, COPT_OLDC))		ccSetStandardC(false);
	else if (strAEqual(opt, COPT_IDHASH))		genCSetIdHash(true);
	else if (strAEqual(opt, COPT_NOIDHASH))		genCSetIdHash(false);
	else if (strAEqual(opt, COPT_FORTRAN))		ccFortran = true;
	else if (s = strAIsPrefix(COPT_CC, opt), s)	ccCompiler= ccLinker= ++s;
	else if (s = strAIsPrefix(COPT_ARGS, opt), s)	ccOptions = ++s;
	else if (s = strAIsPrefix(COPT_COMP, opt), s)	ccCompiler= ++s;
	else if (s = strAIsPrefix(COPT_LINK, opt), s)	ccLinker  = ++s;
	else if (s = strAIsPrefix(COPT_GO, opt), s)	ccGoer    = ++s;
	else if (s = strAIsPrefix(COPT_SMAX, opt), s)	genCSetSMax(atoi(++s));
	else if (s = strAIsPrefix(COPT_IDLEN, opt), s)	genCSetIdLen(atoi(++s));
	else if (s = strAIsPrefix(COPT_FNAME, opt), s)	emitSetCName(++s);
	else if (s = strAIsPrefix(COPT_SYS, opt), s)	ccSystem  = ++s;
	else if (s = strAIsPrefix(COPT_RUNTIME, opt), s)ccRuntime = ++s;
	else if (s = strAIsPrefix(COPT_LIB, opt), s)    ccPushLibrary(++s);
	else return -1;

	return 0;
}

/*
 * Option list to pass to the C compiler.
 * If the option has no arguments, arg should be NULL.
 */
struct ccOption *ccOptionList = 0;

local void
ccPushOptionList(enum ccoptTag opt, String args)
{
	String buf = strCopy(args);
	char *ptr   = buf;
	char *start = buf;
	while (*ptr != '\0') {
		if (*ptr == ',') {
			*ptr = '\0';
			ccPushOption(opt, start);
			start = ptr + 1;
		}
		ptr++;
	}
	
	if (start != ptr)
		ccPushOption(opt, start);
	
}

local void
ccPushOption(enum ccoptTag opt, String arg)
{
	struct ccOption *newOpt;

	newOpt = (struct ccOption *) stoAlloc(OB_Other, sizeof(*newOpt));
	newOpt->tag  = opt;
	newOpt->arg  = arg;
	newOpt->next = ccOptionList;

	ccOptionList = newOpt;
}

local void
ccReverseOptions(void)
{
	struct ccOption *t, *r = 0, *l = ccOptionList;

	while(l) {
		t = l->next;
		l->next = r;
		r = l;
		l = t;
	}
	ccOptionList = r;
}

void
ccSetVerbose(Bool wantVerbose)
{
	ccVerboseFlag = wantVerbose;
}

void
ccSetDebug(Bool isWanted)
{
	if (isWanted) ccPushOption(CCOPT_DebugInfo, NULL);
}

void
ccSetProfile(Bool isWanted)
{
	if (isWanted) ccPushOption(CCOPT_ProfileInfo, NULL);
}

void
ccSetOptimize(Bool isNonStd,Bool isWanted)
{
	if (isWanted) ccPushOption(CCOPT_Optimize, NULL);
	if (isNonStd) ccPushOption(CCOPT_FNonStd,NULL);
}

void
ccSetOutputFile(String fn)
{
	ccPushOption(CCOPT_OutputFile, fn);
}


void
ccSetLineNos(Bool flg)
{
	ccLineNosFlag = flg;
}
		
Bool
ccLineNos()
{
	return ccLineNosFlag;
}

local void
ccSetStandardC(Bool flg)
{
	ccDoStandardCFlag = flg;
}

local void
ccPushLibrary(String string)
{
	ccLibraries = listCons(String)(strCopy(string), ccLibraries);
}

Bool
ccDoStandardC()
{
	if (ccDoStandardCFlag == -1)
		ccDoStandardCFlag = compCfgLookupBoolean("generate-stdc");

	return ccDoStandardCFlag;
}

/*****************************************************************************
 *
 * :: Command formulation
 *
 ****************************************************************************/

static Buffer ccBuf;
static int    ccPutc    (int c)    { return bufPutc(ccBuf, c); }
static void   ccPutq    (String s) { osRunQuoteArg(s, ccPutc); }
static void   ccPuts    (String s) { bufPuts(ccBuf, s); }
static void   ccPutOpt            (struct ccOption *, String newwd, String oldwd);
static void   ccPutFname	  (FileName,          String newwd, String oldwd);
local  void   ccAppendToHiddenArgs(String str);

/*
 * Construct a compile command suitable to run in the directory "newwd".
 * The paths and file names (including "newwd") are given relative to "oldwd".
 */
String
ccCompileCommand(String compiler, String options, int numFiles, FileName *fns,
		 struct ccOption *opts,
		 String newwd, String oldwd, FileName outfile)
{
	struct ccOption *o;
	int		 i;

	ccBuf = bufNew();

	if (!compiler && ccRoot) compiler = strPrintf("%s/bin/%s", ccRoot, CC_DEFAULT);
	if (!compiler) compiler = CC_DEFAULT;

	ccPutq(compiler);
	if (options) {ccPutc(' '); ccPuts(options);} /* Don't quote options */

	for (o = opts; o; o = o->next)
		if (ccIsCompilerOption(o->tag)) ccPutOpt(o, newwd, oldwd);

	if (outfile) {
		ccPutc(' ');
		ccPuts(ccOptionTable[CCOPT_OutputFile].text);
		ccPutFname(outfile, newwd, oldwd);
	}

	ccPutc(' '); ccPutq(ccOptionTable[CCOPT_CompileOnly].text);
	for(i = 0; i < numFiles; i++) ccPutFname(fns[i], newwd, oldwd);

	return bufLiberate(ccBuf);
}

String
ccLinkCommand(String linker, String options, int numFiles, FileName *fns,
	      struct ccOption *opts,
	      String newwd, String oldwd)
{
	struct ccOption *o;
	int		 i;

	ccBuf = bufNew();

	if (!linker && ccRoot) linker = strPrintf("%s/bin/%s",ccRoot, LD_DEFAULT);
	if (!linker) linker = LD_DEFAULT;

	ccPutq(linker);
	if (options) {ccPutc(' '); ccPuts(options);} /* Don't quote options */

	for(i = 0; i < numFiles; i++)
		ccPutFname(fns[i], newwd, oldwd);

	for(o = opts; o; o = o->next)
		if (o->tag == CCOPT_LibPath)
			ccPutOpt(o, newwd, oldwd);
	for(o = opts; o; o = o->next)
		if (ccIsLinkerOption(o->tag) && o->tag != CCOPT_LibPath)
			ccPutOpt(o, newwd, oldwd);

	return bufLiberate(ccBuf);
}

local void
ccPutOpt(struct ccOption *o, String newwd, String oldwd)
{
	ccPutc(' ');
	ccPuts(ccOptionTable[o->tag].text);

	if (!o->arg) return;

	if (strEqual(newwd, osCurDirName()))
		ccPutq(o->arg);
	else if (ccIsDirOption(o->tag)) {
		String s = fileSubdir(oldwd, o->arg);
		ccPutq (s);
		strFree(s);
	}
 	else if (ccIsFileOption(o->tag))
		ccPutFname(fnameParseStatic(o->arg), newwd, oldwd);
	else
		ccPutq(o->arg);
}

local void
ccPutFname(FileName fn, String newwd, String oldwd)
{
	String	s;

	ccPutc(' ');

	if (strEqual(fnameDir(fn), newwd)) {
		s = fnameUnparseStaticWithout(fn);
		ccPutq(s);
	}
	else {
		String odir = fnameDir(fn);
		fnameTSetDir(fn, fileSubdir(oldwd, odir));
		s = fnameUnparseStatic(fn);
		ccPutq(s);
		fnameTSetDir(fn, odir);
	}
}

/******************************************************************************
 *
 * :: Command issue
 *
 *****************************************************************************/

local String ccOutputDir(String, struct ccOption *);
local void   ccSwapDir  (String, String, Length);
local void   ccEchoIf	(String fmt, String arg);

/*
 * Reconcile the options and initialize the commands.
 */

void
ccGetReady(void)
{
	PathList l;
	StringList tmp;
	String	 s;
	static Bool	isReady = false;

	if (isReady) return;
	
	if (!ccRuntime)
		ccRuntime = RUNTIME_DEFAULT;
	
	if (!ccSystem) ccSystem = compCfgGetSysName();

	if (!strEqual(ccSystem, CONFIGSYS)) {

		/* Treat paths as stacks -- newer covers older. */
		for(l = libSearchPathDelta(); l; l = cdr(l)) { 
			/* !!! This is UNIX (+ DOS) Specific */
			String s0, s1;
			s0 = strConcat(car(l), "/");
			s1 = strConcat(s0, ccSystem);
			strFree(s0);
			ccPushOption(CCOPT_LibPath, s1);
		}
	}
	for(l = libSearchPathDelta(); l; l = cdr(l)) 
		ccPushOption(CCOPT_LibPath, car(l));
	for(l = incSearchPathDelta(); l; l = cdr(l))
		ccPushOption(CCOPT_IncPath, car(l));

	for(l = arLibraryKeys(); l; l = cdr(l))
		ccPushOption(CCOPT_Library, car(l));

	tmp = listReverse(String)(ccLibraries);
	for(l = tmp; l; l = cdr(l))
		ccPushOption(CCOPT_Library, car(l));
	listFree(String)(tmp);

	ccPushOptionList(CCOPT_Library, ccRuntime);

	ccReverseOptions();

	s = osGetEnv("CC");
	if (s) {
    		if (!ccCompiler) ccCompiler = strCopy(s);
		if (!ccLinker)	 ccLinker   = strCopy(s);
	}
	s = osGetEnv("CGO");
	if (s) {
		if (!ccGoer)     ccGoer	    = strCopy(s);
	}

	if (ccVerboseFlag) 
		ccAppendToHiddenArgs("-WV");

	if (ccFortran)
		ccAppendToHiddenArgs("-Wfortran");

	s = "-Wsys=";
	s = strConcat(s, ccSystem);
	ccAppendToHiddenArgs(s);
	strFree(s);

	if (ccDoStandardC())
		ccAppendToHiddenArgs("-Wstdc");

	isReady = true;
}

local void
ccAppendToHiddenArgs(String str)
{
	String s;
	s = osGetEnv("UNICL");
	s = s ? s : "";
	if (s[0] != '\0') 
		str = strConcat(str, " ");
	s = strConcat(str, s);
	s = strConcat("UNICL=", s);
	osPutEnv(s);
	if (!osPutEnvIsKept()) strFree(s);
}

/*
 * Compile a single C file.
 * Have to assume output will go in the current directory of the compile.
 */
void
ccCompileFile(String newwd, FileName outfile, FileName fn)
{
	String	command;
	char	oldwd[1024];
	int	rc;

	newwd = ccOutputDir(newwd, NULL /* ccOptionList */);
	ccSwapDir(newwd, oldwd, sizeof(oldwd));

	command = ccCompileCommand(ccCompiler, ccOptions, 1, &fn,
				   ccOptionList, newwd, oldwd, outfile);

	ccEchoIf("Exec: %s\n", command);
	rc = osRun(command);
	if (rc != 0) comsgFatal(NULL, ALDOR_F_CcFailed, command);

	ccSwapDir(oldwd, NULL, int0);
	strFree(command);
	strFree(newwd);
}

/*
 * Link a program.
 * Have to assume output will go in the current directory of the link.
 */
void
ccLinkProgram(String newwd, FileName *files, int numFiles)
{
	String	command;
	char	oldwd[1024];
	int	rc;

	newwd = ccOutputDir(newwd, ccOptionList);
	ccSwapDir(newwd, oldwd, sizeof(oldwd));

	command = ccLinkCommand(ccLinker, ccOptions, numFiles, files,
				ccOptionList, newwd, oldwd);

	ccEchoIf("Exec: %s\n", command);
	rc = osRun(command);
	if (rc != 0) comsgFatal(NULL, ALDOR_F_LinkFailed, command);

	ccSwapDir(oldwd, NULL, int0);
	strFree(command);
	strFree(newwd);
}

/*
 * Run a program
 */
void
ccGoProgram(FileName fn, int argc1, String *argv1)
{
	int	i;
	Buffer  cmdbuf = bufNew();

	if (ccGoer)
		bufPrintf(cmdbuf, "%s ", ccGoer);

	bufPrintf(cmdbuf, "%s", fnameUnparseStaticWith(fn));
	
	for (i = 0; i < argc1; i++)
		bufPrintf(cmdbuf, "  '%s'", argv1[i]);

	ccEchoIf("Exec: %s\n", bufChars(cmdbuf));

	osRun(bufChars(cmdbuf));

	bufFree(cmdbuf);
}

/*
 * Find the directory in which the linked program will be produced.
 */
local String
ccOutputDir(String newwd, struct ccOption *opts)
{
	String	ofiledir = 0;
	FileName fn;

	while (opts) {
		if (opts->tag == CCOPT_OutputFile) {
			if (ofiledir) strFree(ofiledir);
			fn = fnameParseStatic(opts->arg);
			ofiledir = strCopy(fnameDir(fn));
		}
		opts = opts->next;
	}
	if (!ofiledir)
		ofiledir = strCopy(newwd);
	return ofiledir;
}

/*
 * Change to the desired directory.
 */
local void
ccSwapDir(String newwd, String oldwdbuf, Length oldwdsize)
{
	int rc;

	if (strEqual(newwd, osCurDirName()) || strEqual(newwd, "")) {
		if (oldwdbuf) strcpy(oldwdbuf, osCurDirName());
	}
	else {
		ccEchoIf("Cd:   %s\n", newwd);
		rc = osDirSwap(newwd, oldwdbuf, oldwdsize);
		if (rc != 0) comsgFatal(NULL, ALDOR_F_CdFailed, newwd);
	}
}

local void
ccEchoIf(String fmt, String arg)
{
	if (ccVerboseFlag) { fprintf(osStdout, fmt, arg); fflush(osStdout); }
}
