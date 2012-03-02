/*****************************************************************************
 *
 * axlcomp.c: Aldor Compiler Top Level Entry Points.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
# include "axltop.h"
# include "compcfg.h"
# include "emit.h"

String 		compRootDir     = 0;
Bool   		compIsDebug     = false;
static Bool	compDoGc        = true;
static Bool     compDoGcVerbose = false;
static Bool	compDoGcFile    = false;
static EmitInfo *compFinfov	= 0;	/* Tells exit handler about files. */

static JmpBuf	compFintJmpBuf;
static void 	compFintBreakHandler0	(int);

static String compRootFromCmdLine(String cwd, String file);

extern int      compGLoop       (int, char **, FILE *, FILE *);
extern void     compGLoopEval   (FILE *, FILE *, EmitInfo);
extern void     compGLoopInit   (int, char **, FILE *, FileName *, EmitInfo *);
extern int      compGLoopFinish (FileName, EmitInfo);

/*****************************************************************************
 *
 * :: Top-level entry points
 *
 ****************************************************************************/
 
/*
 * Select and call one of the compXxxxLoop entry points below.
 */
int
compCmd(int argc, char **argv)
{
	Bool	echo;

        osInit();
	osObtainLicense();

	echo = cmdSubsumeResponseFiles(1, &argc, &argv);

	/* Display the version string in all its glory */
	if (cmdHasVerboseOption(argc, argv)) {
		fprintf(osStdout, "%s version %d.%d.%d%s",
			axiomxlName,
			axiomxlMajorVersion,
			axiomxlMinorVersion,
			axiomxlMinorFreeze,
			axiomxlPatchLevel);
		if (axiomxlEditNumber)
			fprintf(osStdout, "(%d)", axiomxlEditNumber);
		fprintf(osStdout, " for %s %s\n", CONFIG, DEBUG_CONFIG);
		if (echo) cmdEcho(osStdout, argc, argv);
	}

	/*
	 * Record desire for garbage collection.
	 */
	/* Don't set compDoGcVerbose if -V seen */
	compDoGcFile    = cmdHasGcFileOption(argc, argv) ||
			  cmdHasInteractiveOption(argc, argv); 


	/* Garbage collection is now on by default */
	if (cmdHasNoGcOption(argc, argv))
	{
		/* Can't have -Wgc and -Wno-gc */
		if (cmdHasGcOption(argc, argv))
		{
			(void)fprintf(osStdout,
				"You may not specify both `%s' and `%s': %s.\n",
				"-Wgc", "-no-gc", "ignoring -Wno-gc");
		}
		else
			compDoGc = false;
	}


	/*
	 * Find the root for the system library.
	 */
	if (cmdHasRootOption(argc, argv))
		compRootDir = cmdOptionArg;
	/* $ALDORROOT overrides $AXIOMXLROOT */
	if (!compRootDir)
		compRootDir = strCopyIf(osGetEnv("ALDORROOT"));
	if (!compRootDir)
		compRootDir = strCopyIf(osGetEnv("AXIOMXLROOT"));
	if (!compRootDir)
		compRootDir = compRootFromCmdLine(osCurDirName(), argv[0]);

	if (cmdHasCfgFileOption(argc, argv))
		compCfgSetConfigFile(cmdOptionArg);
	if (cmdHasCfgNameOption(argc, argv))
		compCfgSetSysName(cmdOptionArg);

#ifdef OS_MAC_SYS7
        if (!compRootDir)
		/*!!FIX! Parent of Directory in which binary was found */
                compRootDir = "::";
#endif

	/*
	 * Decide on the entry point.
	 */
	if (cmdHasHelpOption(argc, argv)) {
		compInit();
		cmdArguments(1, argc, argv);
		compFini();
		return 0;
	}

	if (cmdHasDebugOption(argc, argv))
		compIsDebug = true;

	if (cmdHasSEvalOption(argc, argv))
		return compSEvalLoop(osStdin,osStdout);

	if (cmdHasSExprOption(argc, argv))
		return compSExprLoop(osStdin,osStdout);

	if (cmdHasInteractiveOption(argc, argv))
	{
            if (osIsGUI())
                return 0;  /* gloop must be handled in event-driven style */
	    else
                return compGLoop(argc, argv, osStdin, osStdout);
	}

	return compFilesLoop(argc, argv);
}

String
compRootFromCmdLine(String cwd, String file)
{
	FileName fname = fnameParseStaticWithin(file, cwd);
	String binDir = fnameDir(fname);
	FileName rootDir = fnameNew(binDir, "..", "");

	String root = fnameUnparse(rootDir);

	return root;
}

/*
 * Read expressions from the file "in" and write them to the file "out".
 */
int
compSExprLoop(FILE *in, FILE *out)
{
	compInit();

	sxiReadPrintLoop(in, out,
		         compIsDebug ? SXRW_SrcPos : SXRW_NoSrcPos);
	compFini();
	return 0;
}
 
/*
 * Read expressions from the file "in",
 * evaluate them (according to a limited subset of Common Lisp semantics),
 * and write them to the file "out".
 */
int
compSEvalLoop(FILE *in, FILE *out)
{
	compInit();

	if (verBannerWanted()) 
		verPrint();

	sxiReadEvalPrintLoop(in, out,
		             compIsDebug ? SXRW_SrcPos : SXRW_NoSrcPos);
	compFini();
	return 0;
}

int
compInteractiveLoop(int argc, char **argv, FILE *fin, FILE *fout)
{
	int		iargc, totErrors, lineno;
	FileName	fn;
	EmitInfo	finfo;
	AbSyn		ab;
	Stab		stab;
	Foam		foam;
	Bool		readingInitFile	= true, tmpHistory;
	Bool		endOfInput = false; 
	FILE		* fin0 = fin;

	compInit();

	if (verBannerWanted()) 
		verPrint();

	iargc = cmdArguments(1, argc, argv);
 
	argc	 -= iargc;
	argv	 += iargc;
	emitDoneOptions(argc, argv);


	if (osFileIsThere(cmdInitFile)) {
		FileName	fname;

		fname = fnameParse(cmdInitFile);
		fin = fileMustOpen(fname, osIoRdMode);
		fnameFree(fname);
		intStepNo = 0;
		tmpHistory = fintHistory;
		fintHistory = false;
		fprintf(fout, "Reading %s...\n", cmdInitFile);
	}
	else {
		intStepNo = 1;
		readingInitFile = false;
		tmpHistory = fintHistory;
	}

	fn  = fnameStdin();

	finfo  = emitInfoNew(fn);
 
	lineno	  = 0;
 
	compFileInit(finfo);
	stab = stabFile();
	comsgFini();
 
	fintInit();

	for (; !endOfInput; intStepNo++) {
		comsgInit();
		breakSetRoot(NULL);
		car(stab)->isChecked = false;

		if (SetJmp(compFintJmpBuf)) {
			if (feof(fin)) break;
		}

		comsgPromptPrint(fin, fout, 
		       fintHistory ? "%%%d := " : "%%%d >> ",
		       intStepNo);
 
		osSetBreakHandler(compFintBreakHandler0);

		ab = compFileFront(finfo, stab, fin, &lineno);
		breakSetRoot(ab);

		if (compIsMoreAfterFront(finfo) &&
		    !abIsEmptySequence(ab)) {
			ab = (AbSyn) fintWrap(ab, intStepNo);
 
			foam = compFileMiddle(finfo, stab, ab);
			if (foam) {
 				Bool ok = fint(foam);
				if (ok && fintVerbose)
					fintPrintType(fout, ab);

				foamFree(foam);
				fintDisplayTimings();
			}
		}

		comsgFini();
		/* abFree(ab); !! ab is seeping into types. */

		if (feof(fin)) {
			if (readingInitFile) {
				fclose(fin);

				fin = fin0;
				lineno	  = 0;
				readingInitFile = false;
				fintHistory = tmpHistory;
				intStepNo = 0;
 
				comsgFini();
			}
			else
				endOfInput = true;
		}
	}			
 
	totErrors = comsgErrorCount();

	fintFini();
 
	compFileFini(finfo);
	emitAllDone();
	emitInfoFree(finfo);
	fnameFree(fn);
	compFini();

	return totErrors;
}

int
compGLoop(int argc, char **argv, FILE *fin, FILE *fout)
{
    FileName    fn;
    EmitInfo    finfo;
  
    compGLoopInit(argc, argv, fout, &fn, &finfo);
    compGLoopEval(fin, fout, finfo);
    compGLoopFinish(fn, finfo);
  
    return 0;
}

void
compGLoopEval(FILE * fin, FILE * fout, EmitInfo finfo)
{
        Stab            stab = stabFile();
	AbSyn		ab;
	Foam		foam;
        static int      lineno = 0;
      
        comsgInit();
      
        while (!osFEof(fin)) {
			   Bool ok;
               intStepNo += 1;
               comsgFini();
               comsgInit();
               car(stab)->isChecked = false;

               if (SetJmp(compFintJmpBuf))
                        if (osFEof(fin))
                                break;

               comsgPromptPrint(fin, fout,
                                fintHistory ? "%%%d := " : "%%%d >> ",
                                intStepNo);
               osSetBreakHandler(compFintBreakHandler0);
               ab = compFileFront(finfo, stab, fin, &lineno);
               breakSetRoot(ab);

               if (!compIsMoreAfterFront(finfo) ||
                   abIsEmptySequence(ab))
                      continue;
                
               ab = (AbSyn) fintWrap(ab, intStepNo);
               foam = compFileMiddle(finfo, stab, ab);

               if (!foam)   continue;

               ok = fint(foam);
	       if (!ok)
		       fprintf(fout, "Unhandled Exception!\n");
               foamFree(foam);

               if (fintVerbose && ok)
                      fintPrintType(fout, ab);
               fintDisplayTimings();
        }
      
        comsgFini();                
}

void
compGLoopInit(int argc, char **argv, FILE *fout, FileName *pfn,
              EmitInfo *pfinfo)
{
	int		iargc;
	Bool		tmpHistory;
	FILE		* fin;
        FileName        fname;

	compInit();

	iargc     = cmdArguments(1, argc, argv);
	argc	 -= iargc;
	argv	 += iargc;
	emitDoneOptions(argc, argv);

	*pfn  = fnameStdin();
	*pfinfo  = emitInfoNew(*pfn);

	compFileInit(*pfinfo);
	comsgFini();
	fintInit();

	/* Helpful start-up banner ... */
	fprintf(osStdout,"%s\n",comsgString(ALDOR_M_GloopBanner));
	if (comsgOkRelease()) {
		fprintf(osStdout, "%s: %s(%s) version %d.%d.%d%s",
			"Release",
			axiomxlName, "C", /* C-language version */
			axiomxlMajorVersion,
			axiomxlMinorVersion,
			axiomxlMinorFreeze,
			axiomxlPatchLevel);
		if (axiomxlEditNumber)
			fprintf(osStdout, "(%d)", axiomxlEditNumber);
		fprintf(osStdout, " for %s %s\n", CONFIG, DEBUG_CONFIG);
	}
	(void)fputs("Type \"#int help\" for more details.\n",osStdout);

	if (osFileIsThere(cmdInitFile)) {
		fname = fnameParse(cmdInitFile);
		fin = fileMustOpen(fname, osIoRdMode);
		fnameFree(fname);
		intStepNo = -1;
		tmpHistory = fintHistory;
		fintHistory = false;
		fprintf(fout, "Reading %s...\n", cmdInitFile);
		compGLoopEval(fin, fout, *pfinfo);
                fclose(fin);
	}
	else {
		intStepNo = 0;
		tmpHistory = fintHistory;
	}
	fintHistory = tmpHistory;
}

int
compGLoopFinish(FileName fn, EmitInfo finfo)
{
   int totErrors = comsgErrorCount();

        /* Prevent calling gc upon exit */
	stoCtl(StoCtl_GcLevel, StoCtl_GcLevel_Never);
	fintFini();
 	compFileFini(finfo);
	emitAllDone();
	emitInfoFree(finfo);
	fnameFree(fn);
	compFini();

	return totErrors;
}
 
 
/*
 * Compile files controlled by the argument vector and
 * return the total error count.
 */
int
compFilesLoop(int argc, char **argv)
{
	int		i, iargc, totErrors, nErrors;
	FileName	fn;
	Bool		isSolo;
 
	compInit();

	if (verBannerWanted()) 
		verPrint();

	iargc = cmdArguments(1, argc, argv);
 
	argc -= iargc;
	argv += iargc;
	if (argc == 0) {
		if (comsgOkBreakLoop())
			bloopMsgFPrintf(osStdout, ALDOR_W_NoFiles, cmdName);
		comsgWarning(NULL, ALDOR_W_NoFiles, cmdName);
	}
	emitDoneOptions(argc, argv);
	ccGetReady();
 
	isSolo    = (cmdFileCount == 1);
 
	compFinfov = (EmitInfo *) stoAlloc((unsigned) OB_Other,
					   (cmdFileCount+1) * sizeof(EmitInfo));
	for (i = 0; i <= cmdFileCount; i += 1) compFinfov[i] = 0;
 
	totErrors = 0;
	for (i = 0; i < cmdFileCount; i++) {
		fn = fnameParse(argv[i]);
		compFinfov[i] = emitInfoNew(fn);
		nErrors = 0;
 
		if (!fileIsReadable(fn)) {
			if (comsgOkBreakLoop())
				bloopMsgFPrintf(osStdout, ALDOR_F_CantOpen, argv[i]);
			comsgFatal(NULL, ALDOR_F_CantOpen, argv[i]);
		}
 
		switch (ftypeNo(fnameType(fn))) {
#if 0
		case FTYPENO_C:
			nErrors = compCFile(compFinfov[i]);
			break;
#endif
		case FTYPENO_OBJECT:
		case FTYPENO_AR_OBJ:
		case FTYPENO_AR_INT:
			break;
		case FTYPENO_FOAMEXPR:
		case FTYPENO_INTERMED:
			if (!isSolo) fprintf(osStdout, "\n%s:\n", argv[i]);
			nErrors = compSavedFile(compFinfov[i]);
			break;
		default:
			if (!ftypeEqual(fnameType(fn), "")) {
				if (comsgOkBreakLoop())
					bloopMsgFPrintf(osStdout,
							ALDOR_F_BadFType,
							argv[i],
							fnameType(fn),
							FTYPE_SRC);
				comsgFatal(NULL, ALDOR_F_BadFType, argv[i],
					   fnameType(fn), FTYPE_SRC);
			}
			/* Fall through. */
		case FTYPENO_NONE:
		case FTYPENO_SRC:
		case FTYPENO_INCLUDED:
		case FTYPENO_ABSYN:
		case FTYPENO_OLDABSYN:
			if (!isSolo) fprintf(osStdout, "\n%s:\n", argv[i]);
			nErrors = compSourceFile(compFinfov[i]);
			break;
		}
		totErrors += nErrors;
		fnameFree(fn);
	}
 
	if (cmdFileCount > 0 && totErrors == 0) {
		compFinfov[cmdFileCount] = emitInfoNewAXLmain();
		compAXLmainFile(compFinfov[cmdFileCount]);
		emitLink(cmdFileCount + 1, compFinfov);
		argc -= cmdFileCount;
		argv += cmdFileCount;
		emitInterp(argc, argv);
		emitRun   (argc, argv);
	}
	if (totErrors > 0) emitAllDone();
 
	for (i = 0; i < cmdFileCount + 1; i++) emitInfoFree(compFinfov[i]);
	stoFree((Pointer) compFinfov);
	compFinfov = 0;

	if (!isSolo) phGrandTotals(cmdVerboseFlag);
	compFini();

	return totErrors;
}
 
 
/*****************************************************************************
 *
 * :: Constituents of the "compXxxxLoop" programs.
 *
 ****************************************************************************/
 
/*
 * Directories to search for {library,include} files come from:
 *
 *      0. relative to included src file:   N/A             .
 *	1. source-file system commands:	    #libraryDir     #includeDir
 *	2. command-line arguments:	    -Y <dir> 	    -I <dir>
 *	3. current directory:		    osCurDirName()  N/A
 *	4. compRootDir default directories: AXIOMXLROOT/lib  AXIOMXLROOT/include
 *	5. PATH environment variables:	    LIBPATH,	    INCPATH
 *	6. system-specific defaults:        osLibraryPath() osIncludePath()
 *
 * The resulting directories are searched in the order listed above.
 * They are generated and added to corresponding variables in reverse order.
 * ALDORROOT overrides AXIOMXLROOT.
 */

static String	compLibraryFiles[] = {  0 };
static String	compLibraryKeys[]  = {  0 };
 
void
compInit(void)
{
	int	gclevel;

	_dont_assert = true;		 /* Turn off assertions (cf -Wcheck).*/

	gclevel = compDoGc     ? StoCtl_GcLevel_Automatic :
		  compDoGcFile ? StoCtl_GcLevel_Demand    :
		                 StoCtl_GcLevel_Never;
	stoCtl(StoCtl_GcLevel, gclevel); /* If and when to GG.           */
	if (compDoGcVerbose) stoCtl(StoCtl_GcFile,  osStdout);
	else stoCtl(StoCtl_GcFile, (int) 0);  /* Put GC messagess on osStdout.  */
	stoCtl(StoCtl_Wash,    false);	 /* Do not initialize pieces.    */

	obInit();
	dbInit();
	comsgOpen();
	compInfoAudit();
 
	osSetBreakHandler(compSignalHandler);
	osSetFaultHandler(compSignalHandler);
	osSetLimitHandler(compSignalHandler);
	osSetDangerHandler(compSignalHandler);

	exitSetHandler	 (compExitHandler);
	fileSetHandler	 (compFileError);
	stoSetHandler	 (compStoreError);
	sxiSetHandler	 (compSExprError);
 
	pathInit();
	if (compRootDir) {
		fileAddLibraryDirectory(fileSubdir(compRootDir, "lib"));
		fileAddLibraryDirectory(fileSubdir(fileSubdir(compRootDir, "share"),
						   "lib"));
		fileAddIncludeDirectory(fileSubdir(compRootDir, "include"));
	}
	fileAddLibraryDirectory(osCurDirName());

	compCfgInit(compRootDir);
	arInit(compLibraryFiles, compLibraryKeys);
 
	sxiInit();
	keyInit();
	ssymInit();
	stabInitGlobal();
	tfInit();
	foamInit();
	optSetInit();
	tinferInit();
}
 
 
/*
 * Compile .as file, returning error count.
 */
int
compSourceFile(EmitInfo finfo)
{
	AbSyn		ab;
	Stab		stab;
	Foam		foam;
	int		msgCount;
 
	compFileInit(finfo);
	breakSetRoot(NULL);
 
	stab = stabFile();
 
	ab = compFileFront(finfo, stab, NULL, NULL);
	breakSetRoot(ab);
 
	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_CPP)) {
		/* Need to inspect finfo to get the filename */
		FileName fn = emitFileName(finfo, FTYPENO_INTERMED);
		genCpp(ab,fnameDir(fn),fnameName(fn));
		emitTheCpp();
	}

	if (compIsMoreAfterFront(finfo)) {
		foam = compFileMiddle(finfo, stab, ab);
 
		compFileSave(finfo, stab, foam);
		compFileBack(finfo, foam);
 
		foamFree(foam);
	}
 
	msgCount = comsgErrorCount();
	compFileFini(finfo);
	/* abFree(ab); !! ab is seeping into types. */
	if (msgCount < 1) breakInterrupt();
 
	return msgCount;
}
 
/*
 * Compile .fm or .ao file, returning error count.
 */
int
compSavedFile(EmitInfo finfo)
{
	Foam	foam;
	Stab	stab;
 
	compFileInit(finfo);
 
	stab  = stabFile();
	foam  = compFileLoadFoam(finfo);
 
	compFileSave(finfo, stab, foam);
	compFileBack(finfo, foam);
	foamFree(foam);
 
	compFileFini(finfo);
	return comsgErrorCount();
}

/*
 * Generate and compile a temporary main C file.
 */
void
compAXLmainFile(EmitInfo finfo)
{
	if (!finfo) return;

	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_AXLMAINC)) {
		compFileInit(finfo);
		compPhasePutAXLmainC(finfo);
		compPhasePutObject(finfo);
		compFileFini(finfo);
	}
}

int
compCFile(EmitInfo finfo)
{
	compFileInit(finfo);
	compPhasePutObject(finfo);
	compFileFini(finfo);

	return comsgErrorCount();
}

void
compFini(void)
{
	comsgClose();
	stabFiniGlobal();
	compCfgFini();
}


/*****************************************************************************
 *
 * :: Constituents of the "compXxxxFile" programs.
 *
 ****************************************************************************/

void
compFileInit(EmitInfo finfo)
{
	phStartAll(cmdVerboseFlag);

	macexInitFile();
	scobindInitFile();
	stabInitFile();
	ablogInit();
	sposInit();
	comsgInit();
	arFileInit(emitFileName(finfo, FTYPENO_INTERMED), 
		   emitGetFileIdName(finfo));
}

void
compFileFini(EmitInfo finfo)
{
	comsgFini();
	sposFini();
	if (compDoGcFile) stoGc();
	phEndAll();
	ablogFini();
	scobindFiniFile();
	stabFiniFile();
	macexFiniFile();
}

Foam
compFileLoadFoam(EmitInfo finfo)
{
	return compPhaseLoadFoam(finfo);
}

void
compFileSave(EmitInfo finfo, Stab stab, Foam foam)
{
	compPhaseSave(finfo, foam, stab);
}

AbSyn
compFileFront(EmitInfo finfo, Stab stab, FILE *fin, int *plno)
{
	FileName	fn = emitSrcFile(finfo);
	AbSyn		ab;
	FTypeNo		ft = ftypeNo(fnameType(fn));

	if (ft == FTYPENO_OLDABSYN) {
		comsgWarning(NULL, ALDOR_W_OldTypeAbsyn, cmdName);
	}
	if ((ft == FTYPENO_ABSYN) || (ft == FTYPENO_OLDABSYN)) {
		ab = compPhaseLoadAbSyn(finfo);
	}
	else {
		SrcLineList	sll;
		TokenList	tl;

		sll  = compPhaseInclude(finfo, fin, plno);

		fintGetInitCompTime();

		if (!compIsMoreAfterInclude(finfo)) { inclFree(sll); return 0; }

		tl   = compPhaseScan   (finfo, sll);
		tl   = compPhaseSysCmd (finfo, tl);
		tl   = compPhaseLinear (finfo, tl);
		ab   = compPhaseParse  (finfo, tl);

		inclFree(sll);
		listFreeDeeply(Token)(tl,tokFree);
		if (comsgErrorCount())		    return ab;
	}

	ab = compPhaseAbNorm (finfo, ab, false);
	ab = compPhaseMacEx  (finfo, ab);
	if (comsgErrorCount())		    return ab;

	ab = compPhaseAbNorm (finfo, ab, true);
	ab = compPhaseAbCheck(finfo, ab); /* creates the .ax file */
	if (!compIsMoreAfterSyntax(finfo))  return ab;

	compPhaseScoBind(finfo, stab, ab);
	if (comsgErrorCount())	{
		if (fintMode == FINT_LOOP) scoSetUndoState();
		return ab;
	}

	compPhaseTInfer (finfo, stab, ab);
	if (comsgErrorCount())	{
		if (fintMode == FINT_LOOP) scoSetUndoState();
		return ab;
	}

	return ab;
}

Foam
compFileMiddle(EmitInfo finfo, Stab stab, AbSyn ab)
{
	Foam foam;
	foam = compPhaseGenFoam(finfo, stab, ab);
	foam = compPhaseOptFoam(finfo, foam);
	return foam;
}

void
compFileBack(EmitInfo finfo, Foam foam)
{
	compPhasePutLisp  (finfo, foam);
	compPhasePutC	  (finfo, foam);
	compPhasePutObject(finfo);
}

Bool
compIsMoreAfterInclude(EmitInfo finfo)
{
	if (comsgErrorCount() != 0) return false;
	if (fintMode == FINT_LOOP) return true;

	return	emitIsOutputNeeded(finfo, FTYPENO_ABSYN) ||
		emitIsOutputNeeded(finfo, FTYPENO_OLDABSYN) ||
		compIsMoreAfterSyntax(finfo);
}

Bool
compIsMoreAfterSyntax(EmitInfo finfo)
{
	if (comsgErrorCount() != 0) return false;
	if (fintMode == FINT_LOOP) return true;

	return	emitIsOutputNeeded(finfo, FTYPENO_INTERMED) ||
		emitIsOutputNeeded(finfo, FTYPENO_FOAMEXPR) ||
		emitIsOutputNeeded(finfo, FTYPENO_SYMEEXPR) ||
		emitIsOutputNeeded(finfo, FTYPENO_C)	  ||
		emitIsOutputNeeded(finfo, FTYPENO_CPP)	  ||
		emitIsOutputNeeded(finfo, FTYPENO_LISP)	    ||
		emitIsOutputNeeded(finfo, FTYPENO_EXEC);
}

Bool
compIsMoreAfterFront(EmitInfo finfo)
{
	return compIsMoreAfterSyntax(finfo);
}


/*****************************************************************************
 *
 * :: Compiler phases -- constituents of the "compFileXxxx" programs.
 *
 ****************************************************************************/


AbSyn
compPhaseLoadAbSyn(EmitInfo finfo)
{
	FileName	fn = emitSrcFile(finfo);
	AbSyn		ab;
	FILE		*fin;

	phStart(PH_Load);

	fin  = fileRdOpen(fn);
	ab   = abRdSExpr(fin, &fn, NULL);
	fclose(fin);

	phEnd((PhPrFun) abPrint, (PhPrFun) abPrettyPrint, (Pointer) ab);
	return ab;
}


Foam
compPhaseLoadFoam(EmitInfo finfo)
{
	SymeList	symes = 0;
	Foam		foam;
	FileName	fn    = emitSrcFile(finfo);
	int		ftype = ftypeNo(fnameType(fn));
 
	phStart(PH_Load);
 
	foam = 0;
	if (ftype == FTYPENO_INTERMED) {
		Lib lib = libRead(fn);
		emitSetFileIdName(libGetFileId(lib));
		if (emitIsOutputNeeded(finfo, FTYPENO_SYMEEXPR)) {
			symes = libGetSymes(lib);
			stabPutMeanings(stabFile(), symes);
		}
		foam = libGetFoam(lib);
		/*!! libClose(lib); */
	}
	if (ftype == FTYPENO_FOAMEXPR) {
		FILE   *fin;
		fin  = fileRdOpen(fn);
		foam = foamRdSExpr(fin, &fn, NULL);
		fclose(fin);
	}
 
	phEnd((PhPrFun) foamPrint, (PhPrFun) 0, (Pointer) foam);
	return foam;
}
 
SrcLineList
compPhaseInclude(EmitInfo finfo, FILE *fin, int *plno)
{
	SrcLineList	sll;
	FileName	fn = emitSrcFile(finfo);

	phStart(PH_Include);

	sll = include(fn, fin, plno, scanIsContinued);

	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_INCLUDED))
		emitTheIncluded(finfo, sll);

	phEnd((PhPrFun) sllPrint, (PhPrFun) 0, (Pointer) sll);
	return sll;
}

TokenList
compPhaseScan(EmitInfo finfo, SrcLineList sll)
{
	TokenList tl;

	phStart(PH_Scan);

	tl = scan(sll);

	phEnd((PhPrFun) toklistPrint, (PhPrFun) 0, (Pointer) tl);
	return tl;
}

TokenList
compPhaseSysCmd(EmitInfo finfo, TokenList tl)
{
	phStart(PH_SysCmd);

	tl = scmdProcessList(tl);

	phEnd((PhPrFun) toklistPrint, (PhPrFun) 0, (Pointer) tl);
	return tl;
}

TokenList
compPhaseLinear(EmitInfo finfo, TokenList tl)
{
	phStart(PH_Linear);

	tl = linearize(tl);

	phEnd((PhPrFun) toklistPrint, (PhPrFun) 0, (Pointer) tl);
	return tl;
}

AbSyn
compPhaseParse(EmitInfo finfo, TokenList tl)
{
	AbSyn	  ab;

	phStart(PH_Parse);

	ab = parse(&tl);

	phEnd((PhPrFun) abPrint, (PhPrFun) abPrettyPrint, (Pointer) ab);
	return ab;
}

AbSyn
compPhaseMacEx(EmitInfo finfo, AbSyn ab)
{
	phStart(PH_MacEx);

	ab = macroExpand(ab);		/* modifies the tree in-place */

	phEnd((PhPrFun) abPrint, (PhPrFun) abPrettyPrint, (Pointer) ab);
	return ab;
}

AbSyn
compPhaseAbNorm(EmitInfo finfo, AbSyn ab, Bool afterMacex)
{
	phStart(PH_AbNorm);

	ab = abNormal(ab, afterMacex);	/* modifies the tree in-place */

	phEnd((PhPrFun) abPrint, (PhPrFun) abPrettyPrint, (Pointer) ab);
	return ab;
}
AbSyn
compPhaseAbCheck(EmitInfo finfo, AbSyn ab)
{
	phStart(PH_AbCheck);

	abCheck(ab);			/* does not modify tree */

	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_OLDABSYN))
		emitTheOldAbSyn(finfo, ab);
	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_ABSYN))
		emitTheAbSyn(finfo, ab);

	phEnd((PhPrFun) abPrint, (PhPrFun) abPrettyPrint, (Pointer) ab);
	return ab;
}

AbSyn
compPhaseScoBind(EmitInfo finfo, Stab stab, AbSyn ab)
{
	AbUse context = AB_Use_NoValue;

	phStart(PH_ScoBind);

	if (fintMode == FINT_LOOP && (fintVerbose || fintHistory)) {
		if (abTag(ab) != AB_Define)
			context = AB_Use_Value;
	}

	abPutUse(ab, context);		/* annotates the tree in-place */
	scopeBind(stab, ab);		/* annotates the tree in-place */

	phEnd((PhPrFun) abPrint, (PhPrFun) abPrettyPrint, (Pointer) ab);
	return ab;
}

AbSyn
compPhaseTInfer(EmitInfo finfo, Stab stab, AbSyn ab)
{
	phStart(PH_TInfer);

	typeInfer(stab, ab);		/* annotates the tree in-place */

	phEnd((PhPrFun) abPrint, (PhPrFun) abPrettyPrint, (Pointer) ab);
	return ab;
}

Foam
compPhaseGenFoam(EmitInfo finfo, Stab stab, AbSyn ab)
{
	String		fn = emitGetFileIdName(finfo);
	Foam		foam;

	phStart(PH_GenFoam);

	foam = generateFoam(stab, ab, fn);

	phEnd((PhPrFun) foamPrint, (PhPrFun) 0, (Pointer) foam);
	return foam;
}

Foam
compPhaseOptFoam(EmitInfo finfo, Foam foam)
{
	phStart(PH_OptFoam);

	foam = optimizeFoam(foam);	/* modifies the tree in-place */

	phEnd((PhPrFun) foamPrint, (PhPrFun) 0, (Pointer) foam);
	return foam;
}

void
compPhaseSave(EmitInfo finfo, Foam foam, Stab stab)
{
	SymeList symes = 0;
	AbSyn	 macs;
	phStart(PH_PutIntermed);

	macs = macexGetMacros();
	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_SYMEEXPR)) {
		symes = emitCollectIntermedSymes(stab, foam);
		emitTheSymbolExpr(finfo, symes, macs);
		listFree(Syme)(symes);
	}
	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_INTERMED)) {
		symes = emitCollectIntermedSymes(stab, foam);
		emitTheIntermed(finfo, symes, foam, macs);
		listFree(Syme)(symes);
		phLibStats(emitFileName(finfo, FTYPENO_INTERMED));
		if (emitDependsWanted())
			emitTheDependencies(finfo);
	}
	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_FOAMEXPR)) {
		emitTheFoamExpr(finfo, foam);
	}

	phEnd((PhPrFun) 0, (PhPrFun) 0, (Pointer) NULL);
}

void
compPhasePutC(EmitInfo finfo, Foam foam)
{
	String   id    = emitGetFileIdName(finfo);
	phStart(PH_PutC);

	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_C)) {
		/* Should do what emitGetFileId does */
#if 0
		FileName fname = emitSrcFile(finfo);
		CCodeList ccodel = genC(foam,
					strCopy(fnameName(fname)));
#else
		CCodeList ccodel = genC(foam, id);
#endif

		emitTheC(finfo, ccodel);

		listFreeDeeply(CCode)(ccodel, ccoFree);
	}

	phEnd((PhPrFun) 0, (PhPrFun) 0, (Pointer) NULL);
}

void
compPhasePutAXLmainC(EmitInfo finfo)
{
	phStart(PH_PutC);

	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_AXLMAINC)) {
		/* ?? Should do what emitGetFileIdName does */
		CCode	  mainc  = genAXLmainC(strCopy(emitGetEntryFile()));
		CCodeList ccodel = listCons(CCode)(mainc, listNil(CCode));

		emitTheC(finfo, ccodel);

		listFreeDeeply(CCode)(ccodel, ccoFree);
	}

	phEnd((PhPrFun) 0, (PhPrFun) 0, (Pointer) NULL);
}

void
compPhasePutLisp(EmitInfo finfo, Foam foam)
{
	SExpr	lisp;

	phStart(PH_PutLisp);

	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_LISP)) {
		lisp = genLisp(foam);
		emitTheLisp(finfo, lisp);
		sxiFree(lisp);
	}

	phEnd((PhPrFun) 0, (PhPrFun) 0, (Pointer) NULL);
}

void
compPhasePutObject(EmitInfo finfo)
{
	phStart(PH_PutObject);

	if (emitIsOutputNeededOrWarn(finfo, FTYPENO_OBJECT))
		emitTheObject(finfo);

	phEnd((PhPrFun) 0, (PhPrFun) 0, NULL);
}

/*****************************************************************************
 *
 * :: Debugging tools
 *
 *****************************************************************************/

#if !defined(NDEBUG)
void
compInfoByteLimit(int n, String name)
{
	if (n > UCHAR_MAX) bug("%s [%d] cannot be packed in a byte.", name, n);
}
#endif

void
compInfoAudit(void)
{
#if !defined(NDEBUG)
	int	i;

	for (i = 0; i < OB_LIMIT; i++)
		if (obInfo[i].code != i)
			bug("obInfo is badly initialized at %d.", i);
	compInfoByteLimit(OB_LIMIT, "OB_LIMIT");

	for (i = 0; i < PH_LIMIT; i++)
		if (phInfo[i].phno != i)
			bug("phInfo is badly initialized at %d.", i);
	compInfoByteLimit(PH_LIMIT, "PH_LIMIT");

	for (i = TK_START; i < TK_LIMIT; i++)
		if (tokInfo(i).tag != i)
			bug("tokInfo is badly initialized at %d.", i);
	compInfoByteLimit(TK_LIMIT, "TK_LIMIT");

	for (i = AB_START; i < AB_LIMIT; i++)
		if (abInfo(i).tag != i)
			bug("abInfo is badly initialized at %d.", i);
	compInfoByteLimit(AB_LIMIT, "AB_LIMIT");

	for (i = TF_START; i < TF_LIMIT; i++)
		if (tformInfo(i).tag != i)
			bug("tformInfo is badly initialized at %d.", i);
	compInfoByteLimit(TF_LIMIT, "TF_LIMIT");

	for (i = 0; i < SYME_LIMIT; i++)
		if (symeInfo[i].kind != i)
			bug("symeInfo is badly initialized at %d.", i);
	compInfoByteLimit(SYME_LIMIT, "SYME_LIMIT");

	for (i = FOAM_START; i < FOAM_LIMIT; i++)
		if (foamInfo(i).tag != i)
			bug("foamInfo is badly initialized at %s = %d.",
			    foamStr(i), i);
	compInfoByteLimit(foamTagLimit(), "foamTagLimit()");

	for (i = LIB_NAME_START; i < LIB_NAME_LIMIT; i++)
		if (libSectInfo(i).tag != i)
			bug("libSectInfo is badly initialized at %s = %d.",
			    libSectInfo(i).str, i);
	compInfoByteLimit(LIB_NAME_LIMIT, "LIB_NAME_LIMIT");

	for (i = FOAM_BVAL_START; i < FOAM_BVAL_LIMIT; i++)
		if (foamBValInfo(i).tag != i)
			bug("foamBValInfo is badly initialized at %s = %d.",
			    foamBValStr(i), i);
#if SMALL_BVAL_TAGS
	compInfoByteLimit(FOAM_BVAL_LIMIT, "FOAM_BVAL_LIMIT");
#endif

	for (i = FOAM_PROTO_START; i < FOAM_PROTO_LIMIT; i++)
		if (foamProtoInfo(i).tag != i)
			bug("foamProtoInfo is badly initialized at %s = %d.",
			    foamProtoStr(i), i);
	compInfoByteLimit(FOAM_PROTO_LIMIT, "FOAM_PROTO_LIMIT");

	for (i = CCO_START; i < CCO_LIMIT; i++)
		if (ccoInfo(i).tag != i)
			bug("ccoInfo is badly initialized at %d.", i);
	compInfoByteLimit(CCO_LIMIT, "CCO_LIMIT");
#endif
}


/*****************************************************************************
 *
 * :: Handlers passed to lower-level libraries
 *
 *****************************************************************************/

void
compFintBreakHandler(int signo)
{
	comsgFPrintf(osStdout, ALDOR_M_FintBreakHandler);

	LongJmp(compFintJmpBuf, 1);
}

void
compFintFaultHandler(int signo)
{
        osDisplayMessage(comsgString(ALDOR_E_SigSegv));
	comsgFPrintf(osStdout, ALDOR_E_SigSegv);

	LongJmp(compFintJmpBuf, 1);
}

static void
compFintBreakHandler0(int signo)
{
	comsgFPrintf(osStdout, ALDOR_M_FintBreakHandler0);
}

void
compExitHandler(int status)
{
	exitSetHandler((ExitFun) 0);
	if (status == EXIT_FAILURE)
		emitCleanup(cmdFileCount + 1, compFinfov);
	comsgFini();
	if (cmdTrapFlag) abort();
}

void SignalModifier
compSignalHandler(int signo)
{
	int	sigerr;

	/* Can't use switch, since since several might be mapped to SIGFAKE. */

	if	(signo == SIGFAKE)	sigerr = ALDOR_E_SigUnknown;
	else if (signo == SIGHUP)	sigerr = ALDOR_E_SigHup;
	else if (signo == SIGINT)	sigerr = ALDOR_E_SigInt;
	else if (signo == SIGQUIT)	sigerr = ALDOR_E_SigQuit;
	else if (signo == SIGILL)	sigerr = ALDOR_E_SigIll;
	else if (signo == SIGTRAP)	sigerr = ALDOR_E_SigTrap;
	else if (signo == SIGABRT)	sigerr = ALDOR_E_SigAbrt;
	else if (signo == SIGEMT)	sigerr = ALDOR_E_SigEmt;
	else if (signo == SIGFPE)	sigerr = ALDOR_E_SigFpe;
	else if (signo == SIGBUS)	sigerr = ALDOR_E_SigBus;
	else if (signo == SIGSEGV)	sigerr = ALDOR_E_SigSegv;
	else if (signo == SIGSYS)	sigerr = ALDOR_E_SigSys;
	else if (signo == SIGPIPE)	sigerr = ALDOR_E_SigPipe;
	else if (signo == SIGTERM)	sigerr = ALDOR_E_SigTerm;
	else if (signo == SIGXCPU)	sigerr = ALDOR_E_SigXcpu;
	else if (signo == SIGXFSZ)	sigerr = ALDOR_E_SigXfsz;
 	else if (signo == SIGDANGER)	sigerr = ALDOR_E_SigDanger;
	else				sigerr = ALDOR_E_SigUnknown;

        osDisplayMessage(comsgString(sigerr));
        comsgError(NULL, sigerr, signo);
	fflush(dbOut);
	exitFailure();
}

MostAlignedType *
compStoreError(int errnum)
{
	int	stoerr;

	switch (errnum) {
	case StoErr_OutOfMemory:
		stoerr = ALDOR_F_StoOutOfMemory;
		break;
	case StoErr_UsedNonalloc:
		stoerr = ALDOR_F_StoUsedNonalloc;
		break;
	case StoErr_CantBuild:
		stoerr = ALDOR_F_StoCantBuild;
		break;
	case StoErr_FreeBad:
		stoerr = ALDOR_F_StoFreeBad;
		break;
	default:
		bugBadCase(errnum);
		NotReached(stoerr = 0);
	}
	if (comsgOkBreakLoop())
		bloopMsgFPrintf(osStdout, stoerr);
	comsgFatal(NULL, stoerr);
	NotReached(return 0);
}

MostAlignedType *
compFreeError(int errnum)
{
	fprintf(osStdout, "Freeing non-allocated space, by the way.\n");
	return 0;
}

FILE   *
compFileError(FileName fn, IOMode mode)
{
	String	name = fnameUnparseStatic(fn);
	if (comsgOkBreakLoop())
		bloopMsgFPrintf(osStdout, ALDOR_F_CantOpenMode, name, mode);
	comsgFatal(NULL, ALDOR_F_CantOpenMode, name, mode);
	NotReached(return 0);
}

SExpr
compSExprError(SrcPos spos, int errnum, va_list argp)
{
	Msg	msg;

	switch (errnum) {
	case SX_ErrPackageExists:
		msg = ALDOR_F_SxPackageExists;
		break;
	case SX_ErrInternNeeds:
		msg = ALDOR_F_SxInternNeeds;
		break;
	case SX_ErrNumDenNeeds:
		msg = ALDOR_F_SxNumDenNeeds;
		break;
	case SX_ErrNReverseNeeds:
		msg = ALDOR_F_SxNReverseNeeds;
		break;
	case SX_ErrBadArgumentTo:
		msg = ALDOR_F_SxBadArgumentTo;
		break;
	case SX_ErrReadEOF:
		msg = ALDOR_F_SxReadEOF;
		break;
	case SX_ErrBadPunct:
		msg = ALDOR_F_SxBadPunct;
		break;
	case SX_ErrBadChar:
		msg = ALDOR_F_SxBadChar;
		break;
	case SX_ErrMacroIlleg:
		msg = ALDOR_F_SxMacroIlleg;
		break;
	case SX_ErrMacroUndef:
		msg = ALDOR_F_SxMacroUndef;
		break;
	case SX_ErrMacroUnimp:
		msg = ALDOR_F_SxMacroUnimp;
		break;
	case SX_ErrCantMacroArg:
		msg = ALDOR_F_SxCantMacroArg;
		break;
	case SX_ErrMustMacroArg:
		msg = ALDOR_F_SxMustMacroArg;
		break;
	case SX_ErrBadFeatureForm:
		msg = ALDOR_F_SxBadFeatureForm;
		break;
	case SX_ErrTooManyElts:
		msg = ALDOR_F_SxTooManyElts;
		break;
	case SX_ErrCantShare:
		msg = ALDOR_F_SxCantShare;
		break;
	case SX_ErrAlreadyShare:
		msg = ALDOR_F_SxAlreadyShare;
		break;
	case SX_ErrBadCharName:
		msg = ALDOR_F_SxBadCharName;
		break;
	case SX_ErrBadComplexNum:
		msg = ALDOR_F_SxBadComplexNum;
		break;
	case SX_ErrBadToken:
		msg = ALDOR_F_SxBadToken;
		break;
	case SX_ErrBadPotNum:
		msg = ALDOR_F_SxBadPotNum;
		break;
	case SX_ErrBadUninterned:
		msg = ALDOR_F_SxBadUninterned;
		break;

	default:
		bugBadCase(errnum);
		NotReached(msg = 0);
	}
	comsgVFatal(abNewNothing(spos), msg, argp);
	NotReached(return 0);
}

