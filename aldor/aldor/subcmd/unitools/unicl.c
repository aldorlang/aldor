/*****************************************************************************
 *
 * unicl.c: Universal C Compiler and Linker 
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/


#include "axlgen.h"

#include "cfgfile.h"
#include "file.h"
#include "strops.h"

/*
 * ToDo: 
 *	Support for library path expansion
 *	Check weird platforms
 *	Clean up memory leaks
 */

/*
 * This version is a complete rewrite of the older version,
 * and uses a config file for most of its work.
 * 
 * `#ifdef' count: 0
 *
 * Idea is that it translates the commands produced by the
 * compiler into platform-specific versions.
 * It is _NOT_ a general purpose compiler.
 */


char *helpInfo[100] = {
    "Usage: unicl [options]* file+",
    "This program accepts Unix-style C compile and link options and",
    "parameters and translates them into one or more native commands.",
    "Configuration parameters are retrieved from an ASCII file (see below).",
    "",
    "This program accepts the following options",
    "",
    " -W h        Help: show help information.",
    " -W v=n      Verbose: set verbosity level to n (must be 1-9).",
    "               -Wv=1 shows the command executed (same as -W v)",
    "               -Wv=2 also shows the system name selected",
    "               -Wv=3 also shows the unicl invocation including the arguments",
    "                     inherited from the environment variable UNICL",
    "               -Wv=4 also shows all the (key,value) pairs",
    " -W v        Verbose: show the generated commands on standard output.",
    " -W n        No-execute: do not execute generated commands.",
    "",
/* not supported yet
    " -R <dir>    Put the resulting files in directory <dir>.",
    "",
*/
    " -D <def>    Pass <def> as a #define to the C compiler.",
    " -U <undef>  Pass <undef> as an #undef to the C compiler.",
    " -I <incdir> Pass the include directory to any command that will use it.",
    " -L <libdir> Pass the library directory to any command that will use it.",
    " -l <lib>    Pass the library name to any command that will use it.",
    "",
    " -O          Use the C compiler optimizer.",
    " -c          Compile but do not link the C source file.",
    " -g          Have the C compiler/linker include debug information.",
    " -o <efile>  Use <efile> as the name of the executable file.",
    " -p          Have the C compiler/linker generate code for profiling.",
    "",
    " -Wopts=... ",
    " -Wtwixt=... ",
    " -Wpost=... ",
    "             Pass additional options to the compiler.  If no string is given,",
    "             then the default augument list will be reset.  Multiple arguments",
    "             can be given, separated by commas, with commas escaped with '\',",
    "             or extra `-Wopts' can be specified.",
    "             `Wopts` puts options before filenames, `Wpost` puts options after ",
    "             anything else and `Wtwixt` puts options between filenames and ",
    "             libraries at the link stage",
    " -Wstdc      Compile using ANSI C  compiler (default is K&R)",
    " -Wshared    Generate code suitable for shared libraries",
    " -Wfortran   Add options for linking fortran code",
    " -Wfnonstd   Enable fast and possibly non-IEEE-compliant floating point",
    "",
/* not supported yet
    " -Wkey=foo=bar Define the key `foo' to have value `bar'",
    "",
*/
    "The next options should be given FIRST, before any paths are processesed.",
    "",
    " -W config=xxx Set configuration file name.  Default is aldor.conf in $ALDORROOT/include.", 
    " -W sys=xxx  Set precise subsystem.  This allows you to change things",
    "             such as optimisation options, linker options and suchlike.",
    "",
    "Before the command line options are processed, this program looks for",
    "an environment variable named UNICL.  This variable should contain",
    "valid command line options. These options are processed before those",
    "on the command line.",

    0
};

static char *getOption		(int argc, char **argv, char *name, char *);
static FILE * getCfgFile	(int argc, char **argv);
static String getCfgName	(int argc, char **argv);
static void  loadConfiguration	(void);
static void  printHelp 	(void);
static void  handleOptions	(int argc, char **argv);
static void  handleOption(int i, int *pNewI, char **argv);
static void  handleSysArgument	(int i, int *pNewI, char **argv);
static String getParameterisedArg(int i, int *pNewi, char **argv);
static void  initState		(String);
static Bool  setupState	(void);
static void generateCommands();
static Bool  executeCommands	(void);
static void  subsumeImplicitArgs(int oargc, char **oargv, int *pargc, char ***pargv);


static ConfigItemList uclInitialOptions();
static void  uclAddSysArgs	(StringList *plst, char *opts);
static void uclReconcileDebug(ConfigItemList lst);
static StringList uclFixOptionList(StringList opts0, String key);
static String 	  uclGetKeyName(String);


/*************************************************************************************
 *
 * :: Options -> Command strings
 *
 *************************************************************************************/

static String 	  uclGetCommandName();
static StringList uclGetGeneralCompileOptions();
static StringList uclCompileOnlyArguments();
static StringList uclGetPreLinkOptions();
static StringList uclGetPostLinkOptions();
static StringList uclExpandLibs(StringList path0, StringList libs, String ext);
static int uclCheckCondition(String name);

/*************************************************************************************
 *
 * :: Executing commands
 *
 *************************************************************************************/

typedef String CCommand;

static void ccInitCommand();

static CCommand ccCompleteCommand	(void);
static void ccSetCommandName		(String name);
static void ccPushArguments		(StringList lst);
static void ccPushArgument		(String s);
static void ccPrintCommand		(CCommand cmd);
static Bool ccExecute			(CCommand cmd);
static void ccSetNoExecute		(void);


/*************************************************************************************
 *
 * :: "Global" constants
 *
 *************************************************************************************/

/* aldor.conf has preference over axiomxl.conf */
#define CONFFILE "aldor.conf"
#define OLDCONFFILE "axiomxl.conf"

static String uclSysName;
static FILE  *uclOptFile;
static ConfigItemList uclOptions;

static int dbgLevel;

/* Needed to keep linker happy */
CREATE_LIST(String);

/*************************************************************************************
 *
 * :: State
 *
 *************************************************************************************/

Bool  	   uclIsCompileOnly;
Bool 	   uclIsLink;
Bool 	   uclStdc;
Bool 	   uclShared;
Bool	   uclFortran;
Bool 	   uclFloatNonStd;
int	   uclOptimize;
int	   uclDebug;
int	   uclProfile;
StringList uclFileList;
StringList uclIncludePath;
StringList uclLibPath;
StringList uclDefines;
StringList uclUnDefines;
StringList uclLibraries;
String	   uclOutputDir;
String	   uclOutputFile;
StringList uclStartOptions;
StringList uclPostOptions;
StringList uclTwixtOptions;
StringList uclExtraKeys;
String uclCommand;

int
main(int argc, char *argv[])
{
	int largc;
	char **largv;
	osInit();
	subsumeImplicitArgs(argc, argv, &largc, &largv);
	initState(argv[0]);
	uclSysName = getCfgName(largc, largv);
	uclOptFile = getCfgFile(largc, largv);

	if (!uclOptFile) exit(2);

	loadConfiguration();
	handleOptions(largc, largv);

	if (!setupState())
		exit(2);

	if (dbgLevel > 2) {
		int i;
		printf("Command:");
		for (i=0; i<largc; i++) {
			printf(" %s", largv[i]);
		}
		printf("\n");
	}

	generateCommands();
	
	if (!executeCommands())
                exit(1);
	exit(0);
	/* gack */
	return 0;
}
static void
printHelp()
{
	int i;
	for (i=0;helpInfo[i];i++) { fprintf(stderr, "%s\n", helpInfo[i]);}
}


static char *
getOption(int argc, char **argv, char *name, char *def)
{
	int i, len;
	len = strlen(name);

	for (i = argc-1; i>=0; i--) {
		if (strncmp(argv[i], name, len) == 0) 
			return strCopy(argv[i]+len);
	}	
	return def;
}

static FILE *
getCfgFile(int argc, char **argv)
{
	FileName cfgFileName;
	FILE *file;
	char *name;
	
	name = getOption(argc, argv, "-Wconfig=", NULL);
	
	/* Should strip off any ".conf" */

	if (!name) {
		cfgFileName = cfgFindFile(CONFFILE, "");
		if (!cfgFileName) cfgFileName = cfgFindFile(OLDCONFFILE, "");
	}
	else
		cfgFileName = fnameParse(name);
	
	if (!cfgFileName) {
		fprintf(stderr, "Can't find config file\n");
		return NULL;
	}
	if ((file = fileTryOpen(cfgFileName, osIoRdMode)) == NULL)
		fprintf(stderr, "Can't open: %s\n", fnameUnparse(cfgFileName));

	return file;
}

static String
getCfgName(int argc, char **argv)
{
	char *name;
	name = getOption(argc, argv, "-Wsys=", NULL);
	if (!name) 	name = getOption(argc, argv, "-WSYS=", NULL);
	if (!name) name = strCopy(CONFIGSYS);

	return name;
}

static void
loadConfiguration()
{
	String		defSection;
	ConfigItemList	lst, all;
	StringList	sections, tmp;

	all = listNil(ConfigItem);
	sections = listSingleton(String)(uclSysName);

	while (sections) {
		/* Read the name of the next section */
		defSection = car(sections);


		/* Remove it from the to-do list */
		sections = cdr(sections);


		/* Get all the options in the section */
		lst = cfgRead(uclOptFile, defSection);


		/* We ignore errors: could display if verbose */
		cfgReadClearErrors();


		/* Add them all to the result list */
		all = listNConcat(ConfigItem)(all, lst);


		/* Get the list of inherited sections */
		tmp = cfgLookupKeyNameList("inherit", lst);


		/* Place them at the front of our to-do list */
		sections = listNConcat(String)(tmp, sections);


		/* Back to the start of the file */
		rewind(uclOptFile);
	}


	/* Now add the default options */
	all = listNConcat(ConfigItem)(all, cfgRead(uclOptFile, "default"));


	/* Finally the initial options */
	all = listNConcat(ConfigItem)(all, uclInitialOptions());

	uclOptions = all;
}


static void
handleOptions(int argc, char **argv)
{
	int i = 1;
        if (argc == 1 ) {printHelp();	exit(0);}
	while (i < argc) {
		if (*argv[i] != '-') {
			uclFileList = listCons(String)(strCopy(argv[i]), uclFileList);
			i++;
		}
		else {	
			int newi;
			handleOption(i, &newi, argv);
			i = newi;
		}
	}
}

#define listNConc1(t, var, v) (listNConcat(t)(var, listSingleton(t)(v)))
static void
handleOption(int i, int *pNewI, char **argv)
{
	switch (argv[i][1]) {
	case 'W':
		handleSysArgument(i, pNewI, argv);
		break;
	case 'R':
		uclOutputDir = argv[i+1];
		*pNewI = i + 2;
		break;
	case 'D':
		uclDefines = listNConc1(String, uclDefines, 
					getParameterisedArg(i, &i, argv));
		break;
	case 'U':
		uclUnDefines = listNConc1(String, uclUnDefines, 
					getParameterisedArg(i, &i, argv));
		break;
	case 'I':
		uclIncludePath = listNConc1(String, uclIncludePath,
					getParameterisedArg(i, &i, argv));
		break;
	case 'L':
		uclLibPath = listNConc1(String, uclLibPath, 
					getParameterisedArg(i, &i, argv));
		break;
	case 'l':
		uclLibraries = listNConc1(String, uclLibraries, 
					getParameterisedArg(i, &i, argv));
		break;
	case 'O':
		uclOptimize = i;
		break;
	case 'c':
		uclIsCompileOnly = true;
		break;
	case 'g':
		uclDebug = i;
		break;
	case 'o':
		uclOutputFile = getParameterisedArg(i, &i, argv);
		break;
	case 'p':
		uclProfile = i;
		break;
	default:
		fprintf(stderr, "unicl: option '%s' not supported.\n", argv[i]);
	}
	*pNewI = i + 1;
}

#define checkArgument(a, x) (strncmp((a)+1, x, strlen(x)) == 0)

static void
handleSysArgument(int i, int *pNewI, char **argv)
{	
	/* Check for things we know about */
	if (checkArgument(argv[i], "Wconfig="))
		/* Dunnit */;
	else if (checkArgument(argv[i], "Wsys="))
		/* Dunnit */;
	else if (checkArgument(argv[i], "WV="))
		/* Dunnit */;
	else if (checkArgument(argv[i], "Wopts="))
		uclAddSysArgs(&uclStartOptions, argv[i] + strlen("-Wopts="));
	else if (checkArgument(argv[i], "Wpost="))
		uclAddSysArgs(&uclPostOptions, argv[i] + strlen("-Wpost="));
	else if (checkArgument(argv[i], "Wtwixt="))
		uclAddSysArgs(&uclTwixtOptions, argv[i] + strlen("-Wtwixt="));
	else if (checkArgument(argv[i], "Wkey="))
		uclAddSysArgs(&uclExtraKeys, argv[i] + strlen("-Wkey="));
	else if (checkArgument(argv[i], "Wv=") || checkArgument(argv[i], "WV="))
		dbgLevel = argv[i][4] > '\0' ? argv[i][4] - '0' : 1;
	else if (checkArgument(argv[i], "Wstdc"))
		uclStdc = true;
	else if (checkArgument(argv[i], "Wfnonstd"))
		uclFloatNonStd = true;
	else if (checkArgument(argv[i], "Wfortran"))
		uclFortran = true;
	else if (checkArgument(argv[i], "Wn") || checkArgument(argv[i], "WN"))
		ccSetNoExecute();
	else if (checkArgument(argv[i], "Wv") || checkArgument(argv[i], "WV"))
		dbgLevel = 1;
	else if (checkArgument(argv[i], "Wshared"))
		uclShared = true;
	else if (checkArgument(argv[i], "Wh") || checkArgument(argv[i], "WH"))
		printHelp();
	else
		fprintf(stderr, "Option: `%s' unknown.\n", argv[i]);
}	

static String
getParameterisedArg(int i, int *pNewI, char **argv)
{
	if (argv[i][2] != '\0') {
		*pNewI = i;
		return strCopy(argv[i]+2);
	}
	

	*pNewI = i+1;
	return strCopy(argv[i+1]);
}


static void
uclAddSysArgs(StringList *plst, char *opts)
{
	char **argv;
	int i, argc;

	if (opts[0] == '\0') {
		*plst = listSingleton(String)("\0");
		return;
	}

	cstrParseCommaified(opts, &argc, &argv);
	for (i=0; i<argc; i++)
		*plst = listNConcat(String)(*plst, listCons(String)(argv[i], listNil(String)));
	stoFree(argv);
}

/*************************************************************************************
 *
 * :: Generating commands
 *
 *************************************************************************************/

static String
uclState(Bool flg)
{
	return flg ? "true" : "false";
}

static void 
generateCommands()
{
	ConfigItemList lst = uclOptions;

	if (dbgLevel > 1) 
		printf("SysName: %s\n", uclSysName);

	if (dbgLevel > 3) {
		printf("[Config]\n");
		while (lst != listNil(ConfigItem)) {
			printf("\t`%s' = `%s'\n", 
			       cfgName(car(lst)), cfgVal(car(lst)));
			lst = cdr(lst);
		}
		printf("[C'est Tout]\n");

		printf("Compile Only: %s\n", uclState(uclIsCompileOnly));
		printf("Link Only:    %s\n", uclState(uclIsLink));
		printf("Optimize:     %s\n", uclState(uclOptimize));
		printf("Debug:        %s\n", uclState(uclDebug));
		printf("Profile:      %s\n", uclState(uclIsCompileOnly));
		printf("OutputDir:    %s\n", uclOutputDir ? uclOutputDir : "(null)");
		printf("OutputFile:   %s\n", uclOutputFile ? uclOutputFile : "(null)");
	}

	ccInitCommand();
	ccSetCommandName(uclGetCommandName());
	ccPushArguments(uclStartOptions);

	if (uclIsCompileOnly) {
		/* -D, -I, -U, -g, -O, -p, -o, -Wfnonstd */
		ccPushArguments(uclGetGeneralCompileOptions());
		ccPushArguments(uclCompileOnlyArguments());
		ccPushArgument(car(uclFileList));
		ccPushArguments(uclPostOptions);
	}
	else {
		ccPushArguments(uclGetPreLinkOptions());
		ccPushArguments(uclFileList);
		ccPushArguments(uclTwixtOptions);
		ccPushArguments(uclGetPostLinkOptions());
		ccPushArguments(uclPostOptions);
	}
	uclCommand = ccCompleteCommand();
}

static Bool
executeCommands()
{
	if (dbgLevel > 0) {
		printf("Exec: ");
		ccPrintCommand(uclCommand);
		printf("\n");
		fflush(stdout);
	}
	return ccExecute(uclCommand);
}


/*************************************************************************************
 *
 * :: Options -> Command strings
 *
 *************************************************************************************/
static StringList uclConstructOptList(String name, StringList given);

static String
uclGetCommandName()
{
	String str = uclGetKeyName("name");
	str = cfgLookupString(str, uclOptions);
	return str;
}

static StringList
uclGetGeneralCompileOptions()
{
	StringList res;
	res = listNil(String);

	/* -I */
	res = listNConcat(String)(res, uclConstructOptList("include", uclIncludePath));
	/* -D */
	res = listNConcat(String)(res, uclConstructOptList("define", uclDefines));
	/* -U */
	res = listNConcat(String)(res, uclConstructOptList("undefine", uclUnDefines));

	if (uclDebug)
		res = listNConcat(String)(res, cfgLookupStringList(uclGetKeyName("debug"), uclOptions));
	if (uclProfile)
		res = listNConcat(String)(res, cfgLookupStringList(uclGetKeyName("profile"), uclOptions));

	if (uclOptimize) {
		if (!uclFloatNonStd) 
			res = listNConcat(String)(res, cfgLookupStringList(uclGetKeyName("optimize"), 
									   uclOptions));
		else
			res = listNConcat(String)(res, cfgLookupStringList(uclGetKeyName("non-std-float"),
									   uclOptions));
	}

	if (uclOutputFile) 
		res = listNConcat(String)(res, 
					  uclConstructOptList("output-name",
							      listSingleton(String)(uclOutputFile)));

	return res;
}


static StringList
uclGetPreLinkOptions()
{
	StringList res;
	res = listNil(String);

	/* -I */
	res = listNConcat(String)(res, uclConstructOptList("include", uclIncludePath));
	/* -D */
	res = listNConcat(String)(res, uclConstructOptList("define", uclDefines));
	/* -U */
	res = listNConcat(String)(res, uclConstructOptList("undefine", uclUnDefines));

	if (uclDebug)
		res = listNConcat(String)(res, 
					  cfgLookupStringList(uclGetKeyName("debug"),
							      uclOptions));
	if (uclProfile)
		res = listNConcat(String)(res,
					  cfgLookupStringList(uclGetKeyName("profile"),
							      uclOptions));
	if (uclOptimize) {
		if (!uclFloatNonStd)
			res = listNConcat(String)(res,
						  cfgLookupStringList(uclGetKeyName("optimize"),
								      uclOptions));
		else
			res = listNConcat(String)(res,
						  cfgLookupStringList(uclGetKeyName("non-std-float"),
								      uclOptions));
	}

	if (uclOutputFile) 
		res = listNConcat(String)(res, 
				  uclConstructOptList("output-name",
						      listSingleton(String)(uclOutputFile)));
	return res;
}

static StringList
uclGetPostLinkOptions()
{
	StringList res;
	Bool flg;
	res = listNil(String);
	
	flg = cfgLookupBoolean("expand-libs", uclOptions);

	if (flg) {
		String ext = cfgLookupString("lib-ext", uclOptions);
		res = uclExpandLibs(uclLibPath, uclLibraries, ext);
	}
	else {
		/* -L */
		res = listNConcat(String)(res, uclConstructOptList("libpath", uclLibPath));
		/* -l */
		res = listNConcat(String)(res, uclConstructOptList("library", uclLibraries));
	}
	
	if (uclFortran) {
		StringList lst = cfgLookupStringList("fortran-libraries", uclOptions);
		res = listNConcat(String)(res, lst);
	}

	return res;
}

static StringList
uclExpandLibs(StringList path0, StringList libs, String ext)
{
	StringList res = listNil(String);
	
	while (libs != listNil(String)) {
		String basename = car(libs);
		StringList path = path0;
                basename = strConcat("lib", basename);
		while (path != listNil(String)) {
			FileName name = fnameNew(car(path), basename, ext);
			if (fileIsOpenable(name, osIoRdMode)) {
				res = listCons(String)(fnameUnparse(name), res);
				break;
			}
			path = cdr(path);
		}
		if (path == listNil(String))
			printf("Warning: %s not found\n", basename);
                strFree(basename);
		libs = cdr(libs);
	}
	return listNReverse(String)(res);
}

static StringList
uclCompileOnlyArguments()
{
	StringList lst = cfgLookupStringList("compile-only", uclOptions);
	/* "-c" or whatnot */
	return lst;
}

static StringList
uclConstructOptList(String name, StringList given)
{
	StringList res;
	String flag;
	String tmp;
	Bool sep;
	
	flag = cfgLookupString(name, uclOptions);

	tmp = strConcat(name, "-sep");
	sep = cfgLookupBoolean(tmp, uclOptions);
	strFree(tmp);
	
	res = listNil(String);
	while (given) {
		if (sep) {
			res = listCons(String)(flag, res);
			res = listCons(String)(car(given), res);  
		}
		else {
			tmp = strConcat(flag, car(given));
			res = listCons(String)(tmp, res);
		}
		given = cdr(given);
	}
	return listNReverse(String)(res);
}

/*************************************************************************************
 *
 * :: State
 *
 *************************************************************************************/

static void initPath(String argv0);
String rootFromCmdLine(String cwd, String file);

static void
initState(String argv0)
{
	uclIsCompileOnly = false;
	uclIsLink   = false;
	uclOptimize = false;
	uclDebug    = false;
	uclProfile  = false;
	uclFortran  = false;
	uclStdc     = false;
	uclFileList = listNil(String);
	uclIncludePath = listNil(String);
	uclDefines = listNil(String);
	uclUnDefines = listNil(String);
	uclOptFile = NULL;
	
	cfgSetCondFunc(uclCheckCondition);
	initPath(argv0);
}

static char *uclDefaultPath = ".%c%s/include%c%s/share/include";

static void 
initPath(String argv0)
{
	char *root, *path;
	/* $ALDORROOT overrides $AXIOMXLROOT */
	root = getenv("ALDORROOT");
	if (!root) root = getenv("AXIOMXLROOT");
	if (!root) root = rootFromCmdLine(osCurDirName(), argv0);
	if (!root) path = strCopy(".");
	else {
		path = (String) stoAlloc(OB_Other, strlen(uclDefaultPath) + 2 * strlen(root) + 1);
		sprintf(path, uclDefaultPath, osPathSeparator(), root, osPathSeparator(), root);
	}

	cfgSetConfPath(path);
	strFree(path);
}

String
rootFromCmdLine(String cwd, String file)
{
	FileName fname = fnameParseStaticWithin(file, cwd);
	String binDir = fnameDir(fname);
	FileName rootDir = fnameNew(binDir, "..", "");

	String root = fnameUnparse(rootDir);

	return root;
}

/* 
 * This function cleans up the state so that we never 
 * generate a bad sequence of options
 */
static Bool
setupState()
{
	String key;
	if (uclExtraKeys) {
		printf("Extra keys not supported yet\n");
		return false;
	}

	uclReconcileDebug(uclOptions);
	
	if (uclIsCompileOnly && cdr(uclFileList)) {
		printf("More than one file specified for compilation only\n");
		return false;
	}

	/* Should deal with '-L' and '-l' expansion here if necessary */
	
	/* If we're not compiling, then we are linking */
	if (!uclIsCompileOnly) uclIsLink = true;

	if (uclOutputDir) {
		printf("-R: Not supported\n");
		return false;
	}

	/* Start Options */
	key = uclGetKeyName("opts");
	uclStartOptions = uclFixOptionList(uclStartOptions, key);

	/* Post Options */
	key = uclGetKeyName("post");
	uclPostOptions = uclFixOptionList(uclPostOptions, key);


	if (uclIsLink) {
	        /* Extra libraries */
		StringList libs = cfgLookupStringList("lib-extra", uclOptions);
		uclLibraries = listNConcat(String)(uclLibraries, libs);
		uclLibPath   = uclFixOptionList(uclLibPath, "lib-default-path");
        	/* Twixt Options */
	        key = uclGetKeyName("twixt");
	        uclTwixtOptions = uclFixOptionList(uclTwixtOptions, key);
	}
	if (uclIsCompileOnly) {
	        uclIncludePath = uclFixOptionList(uclIncludePath,"include-default-path");
	}

	return true;
}

static String
uclGetKeyName(String s)
{
	String r = strCopy(s);
	String mode, std;
	String tmp;
	mode   = uclIsCompileOnly ? "cc-" 	: "link-";
	std    = uclStdc 	  ? "std-" 	: "";
	tmp = r;

	r = strConcat(mode, r);
	strFree(tmp); tmp = r;
	r = strConcat(std, r);
	strFree(tmp); 

	return r;
}

static StringList
uclFixOptionList(StringList opts0, String key)
{
	StringList opts;

	if (opts0 && car(opts0)[0] == '\0') 
		opts = cdr(opts0);
	else {
		opts = cfgLookupStringList(key, uclOptions);
		opts = listNConcat(String)(opts, opts0);
	}
	
	return opts;
}

/* 
 * Assumption is that profile/Optimize is always OK
 * debug/profile may be bad
 * debug/optimize may be bad
 */
static void
uclReconcileDebug(ConfigItemList lst)
{
	Bool wantDebug, wantProf, wantOpt;

	wantDebug = uclDebug != 0;
	wantProf  = uclProfile != 0;
	wantOpt   = uclOptimize != 0;

	if (wantDebug && wantOpt && !cfgLookupBoolean("debug-optimize-ok", lst)) {
		if (uclOptimize > uclDebug) {
			printf("Warning: Both -O and -g specified, ignoring -g\n");
			wantDebug = 0;
		} 
		else {
			printf("Warning: Both -O and -g specified, ignoring -O\n");
			wantOpt = 0;
		}
	}
	if (wantDebug && wantProf && !cfgLookupBoolean("debug-profile-ok", lst)) {
		if (uclProfile > uclDebug) {
			printf("Warning: Both -p and -g specified, ignoring -g\n");
			wantDebug = 0;
		} 
		else {
			printf("Warning: Both -p and -g specified, ignoring -p\n");
			wantProf = 0;
		}
	}
	
	uclOptimize = wantOpt;
	uclDebug    = wantDebug;
	uclProfile  = wantProf;
}


/*************************************************************************************
 *
 * :: Reading @ and environment options
 * 
 * Actually, '@' isn't supported just yet
 *
 *************************************************************************************/

static void
subsumeImplicitArgs(int oargc, char **oargv, int *pargc, char ***pargv)
{
	char **largv, **argv;
	char *env_args;
	char **targv;
	int largc, argc;
	int i;
	largc = 0;
	largv = NULL;

	env_args = osGetEnv("UNICL");
	
	if (env_args) {
		cstrParseUnquoted(env_args, &largc, &largv);
	}
	
	targv = largv;
	argc = largc + oargc;
	argv = (char **) stoAlloc(OB_Other, argc * sizeof(char *));
	argv[0] = oargv[0];
	for (i=0; i<largc; i++) argv[i+1] = targv[i];
	for (i=1; i<oargc; i++) argv[i + largc] = oargv[i];

	*pargc = argc;	
	*pargv = argv;
}

static int
uclCheckCondition(String name)
{
	if (strEqual(name, "stdc"))
		return uclStdc;
	if (strEqual(name, "link"))
		return uclIsLink;
	if (strEqual(name, "compile"))
		return uclIsCompileOnly;
	if (strEqual(name, "shared"))
		return uclShared;
	if (strEqual(name, "optimize"))
		return uclOptimize;
	if (strEqual(name, "nonstdfloat"))
		return uclFloatNonStd;
	if (strEqual(name, "stdfloat"))
		return !uclFloatNonStd;
	if (strEqual(name, "profile"))
		return uclProfile;
	printf("Warning: `%s' not a known keyword\n", name);
	return -1;
}

/*************************************************************************************
 *
 * :: Command retrieval
 *
 *************************************************************************************/

/* 
 * Strategy is that args should be comma separated, and if commas have to be 
 * passed through, then escape them with "\"
 */  

struct _option {
	char *name;
	char *value;
};

struct _option defaultOptions[] = 
{
	/* -g */
	{ "debug",  "-g" },
	{ "link-debug", "$debug"},

	/* -p */
	{ "profile", "-pg" },
	{ "profile", "$profile" },

	/* -O */
	{ "optimize", "-pg" },
	{ "link-optimize", "$optimize" },

	/* -l */
	{ "library", "-l" },
	{ "library-sep", "false" },

	/* -L */
	{ "libpath", "-L" },
	{ "libpath-sep", "true" },
	{ "expand-libs", "false"},
	{ "lib-default-path", ""},

	/* -R */
	{ "output-dir-strategy", "prepend"},

	/* -D */
	{ "define", "-D"},
	{ "define-sep", "false"}, 	/* Some compilers like '-D foo' for definitions */

	/* -U */
	{ "undefine", "-U"},
	{ "undefine-sep", "false" },

	/* -I */
	{ "include", "-I" },
	{ "include-sep", "true" },
	{ "include-default-path", ""},

	/* -c */
	{ "compile-only", "-c" },


	/* -o */
	{ "output-name", "-o" },
	{ "output-name-sep", "false" },

	/* -Wfnonstd */
	{ "non-std-float-args", ""},

	/* General things */
	{ "cc-name", "cc" },
	{ "link-name", "$cc-name" },
	{ "std-cc-name" , "cc" },
	{ "std-link-name" , "$std-cc-name" },
	{ "debug-profile-ok", "false"},
	{ "debug-optimize-ok", "false"},
	{NULL, NULL}
};

static ConfigItemList
uclInitialOptions()
{
	ConfigItemList lst = listNil(ConfigItem);
	int i = 0;
	while (defaultOptions[i].name != NULL) {
		lst = listCons(ConfigItem)(cfgNew(defaultOptions[i].name, defaultOptions[i].value),
					   lst);
		i++;
	}
	return lst;
}



/*************************************************************************************
 *
 * :: Executing commands
 *
 *************************************************************************************/

static Buffer ccBuf;
static Bool   ccNoExecute;

static int    ccPutc    (int c)    { return bufPutc(ccBuf, c); }
static void   ccPutq    (String s) { osRunQuoteArg(s, ccPutc); }

/** No longer needed **
static void   ccPuts    (String s) { bufPuts(ccBuf, s); }
**/

static void 
ccInitCommand()
{
	ccBuf = bufNew();
}

static CCommand
ccCompleteCommand()
{
	String s = bufLiberate(ccBuf);
	ccBuf = NULL;
	return s;
}

static void
ccSetCommandName(String name)
{
	ccPutq(name);
	ccPutc(' ');
}

static void
ccPushArguments(StringList lst)
{
	while (lst != listNil(String)) {
		ccPushArgument(car(lst));
		lst = cdr(lst);
	}
}

static void
ccPushArgument(String s)
{
	ccPutq(s);
	ccPutc(' ');
}

static void
ccPrintCommand(CCommand cmd)
{
	printf("%s", cmd);
}

#if defined (CC_MICROSOFT)
#include <windows.h>
#endif

static Bool 
ccExecute(CCommand cmd)
{
#if defined (CC_MICROSOFT)
	HANDLE x;
	int    r;
	if (ccNoExecute) return true;
	x = CreateFile("xxx", GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, 
	               FILE_ATTRIBUTE_NORMAL, NULL);
	r = osRunRedirect(cmd, TRUE, NULL, x, NULL);
	CloseHandle(x);
	DeleteFile(TEXT("xxx"));
	return (r == 0);
#else
	if (ccNoExecute) return true;
	return (osRun(cmd) == 0);
#endif
}

static void
ccSetNoExecute()
{
	ccNoExecute = true;
}


#if 0
/**/ /* Old unicl's configuation function. */
/**/local void
/**/setConfiguration()
/**/{
/**/  switch (defaultComp) {
/**/  case UNI_COMP_gnu:
/**/    switch (defaultOS) {
/**/    case UNI_OS_AIXRS:
/**/      LINK = "gcc";
/**/      CC = "gcc -Wall -Wno-unused -Wno-uninitialized -Wno-parentheses";
/**/      break;
/**/    case UNI_OS_LINUX: /* May want to specialise for a.out */
/**/      LINK = "gcc";
/**/      CC = "gcc";
/**/      break;
/**/    default:
/**/      LINK = "gcc";
/**/      CC = "gcc";
/**/      break;
/**/    }
/**/    conf_optimize = "-O3";
/**/    break;
/**/
/**/  case UNI_COMP_suncc:
/**/    CC = "cc -fsingle";
/**/    LINK = "cc";
/**/    break;
/**/  case UNI_COMP_decosf:
/**/    CC = "cc -ieee_with_inexact -float -Olimit 1000";
/**/    LINK = "cc";
/**/    break;
/**/
/**/  case UNI_COMP_mips:
/**/#if HW_MIPS == 1  
/**/    CC = "cc -mips1 ";
/**/    LINK = "cc -mips1";
/**/#elif HW_MIPS == 2
/**/    CC = "cc -mips2 ";
/**/    LINK = "cc -mips2";
/**/#elif HW_MIPS == 3
/**/    CC = "cc -mips3 -n32 -OPT:Olimit=0  -OPT:fold_arith_limit=8000 -woff 1177";
/**/    LINK = "cc -mips3 -n32 \"-Wl,-woff 84\"";
/**/#elif HW_MIPS == 4
/**/    CC = "cc -mips4 -n32 -OPT:Olimit=0 -OPT:fold_arith_limit=8000";
/**/    LINK = "cc -mips4 -n32 -woff 84";
/**/#endif
/**/    break;
/**/
/**/  case UNI_COMP_borland:
/**/    CC = "bcc";
/**/    LINK = "bcc";
/**/    break;
/**/
/**/  case UNI_COMP_microsoft:
/**/    CC = "cl /nologo";
/**/    LINK = "cl /nologo";
/**/    osLibExt = "lib";
/**/    break;
/**/
/**/  case UNI_COMP_c370:
/**/    CC = "EXEC CC";
/**/    LINK = "EXEC CMOD";
/**/    break;
/**/
/**/  case UNI_COMP_ibm:
/**/    switch (defaultOS) {
/**/    case UNI_OS_AIXRS:
/**/#ifdef OS_AIX41_RS
/**/      CC = "cc";
/**/      LINK = "cc";
/**/#else
/**/      CC = "xlc";
/**/      LINK = "xlc";
/**/#endif
/**/      break;
/**/    case UNI_OS_AIXRT:
/**/      CC = "cc -a -Nn5000 -Nd5000 -Np6000 -Nt5000 -lm";
/**/      LINK = "cc";
/**/      break;
/**/    case UNI_OS_OS2:
/**/      CC = "icc";
/**/      LINK = "icc";
/**/      break;
/**/    default:
/**/      CC = "cc";
/**/      LINK = "cc";
/**/    };
/**/    break;
/**/
/**/  default:
/**/    CC = "cc";
/**/    LINK = "cc";
/**/  };
/**/
/**/  switch (defaultOS) {
/**/  case UNI_OS_DOS:
/**/    osDirSep	   = '\\';
/**/    osSwitchChar   = '/';
/**/    osPathSep	   = ';';
/**/    osOptSep	   = "";
/**/
/**/    if (defaultComp == UNI_COMP_gnu) {
/**/      osDefaultLibraryPath = "C:\\DJGPP\\LIB";
/**/      osDefaultIncludePath = "C:\\DJGPP\\INCLUDE";
/**/    }
/**/    else {
/**/      osDefaultLibraryPath = "";
/**/      osDefaultIncludePath = "";
/**/    }
/**/
/**/    break;
/**/
/**/  case UNI_OS_WIN32:
/**/    osDirSep	   = '\\';
/**/    osSwitchChar   = '/';
/**/    osPathSep	   = ';';
/**/    osOptSep	   = "";
/**/
/**/    if (defaultComp == UNI_COMP_microsoft) {
/**/      osDefaultLibraryPath = "C:\\MSDEV\\LIB";
/**/      osDefaultIncludePath = "C:\\MSDEV\\INCLUDE";
/**/    }
/**/    else {
/**/      osDefaultLibraryPath = "";
/**/      osDefaultIncludePath = "";
/**/    }
/**/
/**/    break;
/**/
/**/  case UNI_OS_OS2:
/**/    osDirSep	   = '\\';
/**/    osSwitchChar   = '/';
/**/    osPathSep	   = ';';
/**/    osOptSep	   = "";
/**/
/**/    if (defaultComp == UNI_COMP_ibm) {
/**/      osDefaultLibraryPath = "C:\\AXIOMXL\\BASE\\OS2_ICC\\LIB";
/**/      osDefaultIncludePath = "C:\\AXIOMXL\\BASE\\OS2_ICC\\INCLUDE";
/**/    }
/**/    else if (defaultComp == UNI_COMP_borland) {
/**/      osDefaultLibraryPath = "C:\\AXIOMXL\\BASE\\OS2_BCC\\LIB;C:\\BCOS2\\LIB";
/**/      osDefaultIncludePath = "C:\\AXIOMXL\\BASE\\OS2_BCC\\INCLUDE;C:\\BCOS2\\INCLUDE";
/**/    }
/**/    else {
/**/      osDefaultLibraryPath = "";
/**/      osDefaultIncludePath = "";
/**/    }
/**/
/**/    break;
/**/
/**/  case UNI_OS_CMS:
/**/    osDirSep	   = ' ';
/**/    osSwitchChar   = ' ';
/**/    osPathSep	   = ' ';
/**/    osOptSep	   = " ";
/**/
/**/    osDefaultLibraryPath = ALL_DISK_LETTERS;
/**/    osDefaultIncludePath = ALL_DISK_LETTERS;
/**/    break;
/**/
/**/  case UNI_OS_IRIX:
/**/
/**/    /* because on IRIX libraries are in /usr/lib /usr/lib32 /usr/lib64 */
/**/    osDirSep	   = '/';
/**/    osSwitchChar   = '-';
/**/    osPathSep	   = ':';
/**/    osOptSep	   = "";
/**/
/**/    osDefaultLibraryPath = "";
/**/    osDefaultIncludePath = "";
/**/    break;
/**/
/**/  default:
/**/    /* default is to treat like Unix* */
/**/
/**/    osDirSep	   = '/';
/**/    osSwitchChar   = '-';
/**/    osPathSep	   = ':';
/**/    osOptSep	   = "";
/**/
/**/    osDefaultLibraryPath = "/lib:/usr/lib:/usr/local/lib";
/**/    osDefaultIncludePath = "";
/**/  };
/**/
/**/  /* make sure following are not in read-only storage */
/**/
/**/  osOptSep = strDuplicate(osOptSep);
/**/  osLibExt = strDuplicate(osLibExt);
/**/  osDefaultLibraryPath = strDuplicate(osDefaultLibraryPath);
/**/  osDefaultIncludePath = strDuplicate(osDefaultIncludePath);
/**/}
/**/
#endif
