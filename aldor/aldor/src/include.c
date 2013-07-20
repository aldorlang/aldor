/*****************************************************************************
 *
 * include.c: The source file includer.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file contains functions for processing the source file directives
 *
 *  #include   fname
 *  #reinclude fname
 *
 *  #assert    property
 *  #unassert  property
 *
 *  #if        property
 *  #elseif    property
 *  #else
 *  #endif
 *
 *  #line      lno [fname]
 *
 * Other #xxx lines are passed on untouched.
 */

#include "file.h"
#include "fluid.h"
#include "include.h"
#include "opsys.h"
#include "syscmd.h"
#include "util.h"
#include "path.h"

/******************************************************************************
 *
 * :: Forward Declarations
 *
 *****************************************************************************/

typedef enum {
	NoIf, 
	ActiveIf, 
	InactiveIf, 
	FormerlyActiveIf
} IfState;

typedef struct {
	String     curDir;       	/* current directory of the file */
	String     curFile;      	/* current file name string */
	FileName   curFname;      	/* current file name */
	FILE       *infile;      	/* file being included */
	HashList   fileCodes;     	/* hash codes of active source files */
	StringList fileNames;     	/* names of active source files */
	int	   lineNumber;   	/* file line number */
} FileState;

/*
 * Recursive file includer.
 */
local FileName    inclFind   		(String fn, String cwd);
local SrcLineList inclFile        	(String, Bool, Bool, long *pnlines);
local SrcLineList inclFileContents	(void);
local Bool	  inclLine        	(SrcLineList *, InclIsContinuedFun);
local SrcLineList inclError      	(Msg, ...);

/*
 * Include directives.
 */
local SrcLineList inclHandleDirective(void);

/*
 * Utilities
 */
# define 	   DIRECTIVE_CHAR      '#'  /* Character starting directives */

# define 	   INCLUDING(state)    ((state)==NoIf || (state)==ActiveIf)

local Bool	   inclIsDirective     (String line);
local String       inclCalcIndentLevel (String line, int *indent);
local String       inclGetLine         (FILE *);
local String	   inclActiveFileChain (StringList, String);

static struct SrcLineListCons EIFC      = {0, 0};
static SrcLineList	      EndifLine = &EIFC;	/* )endif indicator */


/******************************************************************************
 *
 * :: Includer state
 *
 *****************************************************************************/

StringList    	globalAssertList = 0; 	/* Globally asserted properties */
StringList    	localAssertList  = 0; 	/* Per-entry asserted properties */
long    	inclSerialLineNo = 0;	/* The serial line number */
long		inclFileLineNo   = 1;	/* Filled in at end. */

Buffer  	inclBuffer;		/* Input buffer */
String 		curLineString;          /* The current line being processed */
IfState 	ifState;                /* State of current )if */
FileState	fileState;		/* State of current file */
HashList      	includedFileCodes = 0; 	/* Hash codes of all included files */


/******************************************************************************
 *
 * :: Top-level entry points
 *
 *****************************************************************************/

/*
 * Call either include "includeFile" or "includeLine".
 * If fin is stdin include one line.  Otherwise include the whole file.
 */
SrcLineList
include(FileName fn, FILE *fin, int *plineno, InclIsContinuedFun iscont)
{
	if (fnameIsStdin(fn) && fin)
		return includeLine(fn, fin, plineno, iscont);
	else
		return includeFile(fn);
}


/* 
 * Return a processed source line list of the file contents.
 */
SrcLineList
includeFile(FileName fname)
{
	String	      fnameString;
	SrcLineList   r;

	inclSerialLineNo    = 0;
	fnameString         = strCopy(fnameUnparseStatic(fname));
	inclBuffer          = bufNew();
	includedFileCodes   = 0;
	localAssertList     = listCopy(String)(globalAssertList);

	fileState.curDir    = osCurDirName();
	fileState.fileCodes = 0;
	fileState.fileNames = 0;

	r = listNReverse(SrcLine)(
		inclFile(fnameString, false, true, &inclFileLineNo));

	strFree(fnameString);
	bufFree(inclBuffer);
	listFree(String)(localAssertList);
	listFree(Hash)(includedFileCodes);

	return r;
}

/*
 * Return a processed source line list for the next line of the file.
 * The source line list may have several entries if the line is an
 * includer directive or requires a continuation.
 */
SrcLineList
includeLine(FileName fn, FILE *fin, int *plineno, InclIsContinuedFun iscont)
{
	SrcLineList   r;

	inclBuffer          = bufNew();
	localAssertList     = globalAssertList;

	fileState.curDir    = osCurDirName();
	fileState.curFile   = strCopy(fnameUnparseStatic(fn));
	fileState.curFname  = fn;
	fileState.infile    = fin;
	fileState.fileCodes = 0;
	fileState.fileNames = 0;
	fileState.lineNumber= *plineno;

	ifState = NoIf;

	r = 0;
	inclLine(&r, iscont);
	r = listNReverse(SrcLine)(r);

	/* Leave the assert list and included files for the next call. */
	globalAssertList = localAssertList;

	bufFree(inclBuffer);
	/* strFree(fileState.curFile);	!!This is held on to by the srclines.*/
	listFree(Hash)  (fileState.fileCodes);
	listFree(String)(fileState.fileNames);

	*plineno       = fileState.lineNumber;
	inclFileLineNo = fileState.lineNumber;

	return r;
}

/*
 * Return line counts from previous include.
 */
long
inclTotalLineCount(void)
{
	return inclSerialLineNo;
}

long
inclFileLineCount(void)
{
	return inclFileLineNo;
}

/*
 * Write included lines in a form suitable for re-inclusion.
 * The number of characters written is returned.
 */
int
inclWrite(FILE *file, SrcLineList sll)
{
	int      cc = 0;
	FileName slfile, curfile  = 0; /* Guarantee first line is #line ... */
	Length   slline, curline = 0;

	for ( ; sll; sll = cdr(sll)) {
		SrcLine sl = car(sll);

		if (sl->isSysCmd && sl->sysCmdHandled) continue;

		slfile = sposFile(sl->spos);
		slline = sposLine(sl->spos);
		if (curline != slline - 1 
		||  (curfile && !fnameEqual(curfile,slfile))) 
		{
			if ((curfile && fnameEqual(curfile, slfile)))
				cc += fprintf(file, "%cline %d\n",
					      DIRECTIVE_CHAR,
#if EDIT_1_0_n1_07
					      (int) slline);
#else
					      slline);
#endif
			else {
				cc += fprintf(file, "%cline %d \"%s\"\n",
					      DIRECTIVE_CHAR,
#if EDIT_1_0_n1_07
					      (int) slline,
#else
					      slline,
#endif
					      fnameUnparseStatic(slfile));
				curfile = slfile;
			}
		}
		curline = slline;
			
		cc += fprintf(file, "%*s%s", sl->indentation, "", sl->text);
	}
	return cc;
}

/* 
 * Free a list of source lines.
 */
void
inclFree(SrcLineList sll)
{
	listFreeDeeply(SrcLine)(sll, slineFree);
}

/*
 * Assertions for conditional includes.
 */
# define INCL_Assert(pl, p)   (*(pl)=listCons(String)((p), *(pl)))
# define INCL_Unassert(pl, p) (*(pl)=listNRemove(String)(*(pl),(p),strEqual))
# define INCL_IsAssert(pl, p) (listMember(String)(*(pl), (p), strEqual))

void
inclGlobalAssert(String property)
{
	INCL_Assert(&globalAssertList, property);
}

void
inclGlobalUnassert(String property)
{
	INCL_Unassert(&globalAssertList, property);
}

/******************************************************************************
 *
 * :: Recursive file inclusion
 *
 *****************************************************************************/

#define SysCmdLine(isHandled) \
	slineNewSysCmd(sposNew(fileState.curFname, \
			       fileState.lineNumber, \
			       inclSerialLineNo, \
			       1),\
			 0, curLineString, isHandled)

#define addSysCmd(sll,sl) listNConcat(SrcLine)\
	((sll),listCons(SrcLine)(sl, listNil(SrcLine)))

#define botchSysCmd(kind) addSysCmd(inclError(ALDOR_E_SysCmdBad, kind), \
				       SysCmdLine(true))

#define isThisEndifLine(sll) (sll && \
	(sll == EndifLine || car(listLastCons(SrcLine)(sll))->isEndifLine))


local SrcLineList
inclFile(String fname, Bool reincluding, Bool top, long *pnlines)
{
	Scope("inclFile");

	SrcLineList     sll;
	Hash            fhash;
	FileName        fn;
	FileState	o_fileState; 
	IfState		fluid(ifState);
	String		curdir;

	o_fileState 	     = fileState;     /* no fluid(struct) */
	ifState              = NoIf;
	fileState.lineNumber = 0;

	fn = inclFind(fname, fileState.curDir);

	if (fn != 0) {
		fileState.curDir   = strCopy(fnameDir(fn));
		fileState.curFile  = strCopy(fnameUnparseStatic(fn));
		fileState.curFname = fn;
	}
	curdir = fileState.curDir;

	if (fn == 0) {
		fileState = o_fileState;
		if (top) {
			comsgFatal(NULL, ALDOR_F_CantOpen, fname);
			NotReached(sll = 0);
		}
		else
			sll = inclError(ALDOR_F_CantOpen, fname);
	} 
	else {
		fhash = fileHash(fn);
		fname = strCopy (fnameUnparseStatic(fn));

		if (!reincluding && listMemq(Hash)(includedFileCodes, fhash)) {
			sll = listNil(SrcLine);
		}
		else if (listMemq(Hash)(fileState.fileCodes, fhash)) {
			String s = inclActiveFileChain
				(fileState.fileNames, "->");
			fileState = o_fileState;
			sll = inclError(ALDOR_E_InclInfinite, s);
			strFree(s);
		}
		else {
			includedFileCodes   =
			   listCons(Hash)  (fhash,includedFileCodes);
			fileState.fileCodes =
			   listCons(Hash)  (fhash,fileState.fileCodes);
			fileState.fileNames =
			   listCons(String)(fname,fileState.fileNames);
			fileState.infile    = fileRdOpen(fn);

			sll = inclFileContents();

			listFreeCons(Hash)  (fileState.fileCodes);
			listFreeCons(String)(fileState.fileNames);
			fclose(fileState.infile);
		}
		fnameFree(fn);
		strFree(curdir);
				 /*!! curFile is used in src lines */
		strFree(fname);
	}
	if (pnlines) *pnlines = fileState.lineNumber;
	fileState = o_fileState;
	Return(sll);
}

/* 
 * Open a file, looking first in the current directory, then
 * in the list of include directories.
 */
local FileName
inclFind(String fname, String curdir)
{
	StringList    dl;
	FileName      fn;

	dl = listCons(String)(curdir, incSearchPath());
	fn = fileRdFind(dl, fname, FTYPE_SRC);
	listFreeCons(String)(dl);
	return fn;
}

/*
 * Conses (reversed) lines for the file.
 */
local SrcLineList
inclFileContents(void)
{
	SrcLineList	sll = listNil(SrcLine);

	while (inclLine(&sll, (InclIsContinuedFun) NULL))
		;

	return sll;
}

/*
 * Decide whether the line is an include or system command directive.
 */
local Bool
inclIsDirective(String line)
{
	return *line == DIRECTIVE_CHAR;
}

/*
 * Conses (reversed) lines which arise from "including" this one line.
 * Returns true if there may be more.
 */
local Bool
inclLine(SrcLineList *psll, InclIsContinuedFun isCont)
{
	int             indent;
	String          s;
	SrcLine         sl;
	SrcPos		spos;
	SrcLineList	d_sll;

	do {
		curLineString = inclGetLine(fileState.infile);
		if (!curLineString) {
			if (ifState != NoIf) 
			*psll = listNConcat(SrcLine)
			(inclError(ALDOR_E_InclIfEof), *psll);
			return false;
		}
		fileState.lineNumber++;
		inclSerialLineNo++;
		if (inclIsDirective(curLineString)) {
			/*!! This may be too costly with deep nesting. */
			d_sll = inclHandleDirective();
			if (isThisEndifLine(d_sll)) {
				if (d_sll == EndifLine) return false;
				*psll = listNConcat(SrcLine)(d_sll, *psll);
				return false;
			}
			*psll = listNConcat(SrcLine)(d_sll, *psll);
		}
		else if (INCLUDING(ifState)) {
			s = inclCalcIndentLevel(curLineString, &indent);
			spos = sposNew(fileState.curFname, fileState.lineNumber,
				       inclSerialLineNo, 1);
			sl = slineNew(spos, indent, s);
			*psll = listCons(SrcLine)(sl, *psll);
		}

	} while (isCont && (*isCont)(curLineString));

	 return true;
}	 

local SrcLineList
inclError(Msg msg, ...)
{
	SrcPos	spos;
	va_list argp;

	spos = sposNew(fileState.curFname,
		       fileState.lineNumber,
                       inclSerialLineNo,
                       1);

	va_start(argp, msg);
	comsgVError(abNewNothing(spos), msg, argp);
	va_end(argp);

	return listNil(SrcLine);
}

/******************************************************************************
 *
 * :: Handle includer directives such as ")include file", etc.
 *
 *****************************************************************************/

local SrcLineList inclHandleReinclude	(String fname);
local SrcLineList inclHandleInclude	(String fname);
local SrcLineList inclHandleIncludeDir	(String dname);
local SrcLineList inclHandleAssert	(String property);
local SrcLineList inclHandleUnassert	(String property);
local SrcLineList inclHandleIf		(String property);
local SrcLineList inclHandleElseif	(String property);
local SrcLineList inclHandleElse	(void);
local SrcLineList inclHandleEndif	(void);
local SrcLineList inclHandleLine	(int lno, String fname);
local SrcLineList inclHandleUnknown	(void);

local SrcLineList
inclHandleDirective(void)
{
	String	s, s0, fname, property;
	int	lno;

	s = s0 = curLineString;

	if ((s = scmdIsDirective(s0,"include")) != 0) {
		if (!scmdScanFName(s, &fname)) return botchSysCmd("include");
		return inclHandleInclude(fname);
	}
	if ((s = scmdIsDirective(s0,"reinclude")) != 0) {
		if (!scmdScanFName(s, &fname)) return botchSysCmd("reinclude");
		return inclHandleReinclude(fname);
	}
	if ((s = scmdIsDirective(s0,"includeDir")) != 0) {
		if (!scmdScanFName(s, &fname)) return botchSysCmd("includeDir");
		return inclHandleIncludeDir(fname);
	}
	if ((s = scmdIsDirective(s0,"assert")) != 0) {
		if (!scmdScanId(s, &property)) return botchSysCmd("assert");
		return inclHandleAssert(property);
	}
	if ((s = scmdIsDirective(s0,"unassert")) != 0) {
		if (!scmdScanId(s, &property)) return botchSysCmd("unassert");
		return inclHandleUnassert(property);
	}
	if ((s = scmdIsDirective(s0,"if")) != 0) {
		if (!scmdScanId(s, &property)) return botchSysCmd("if");
		return inclHandleIf(property);
	}
	if ((s = scmdIsDirective(s0,"elseif")) != 0) {
		if (!scmdScanId(s, &property)) return botchSysCmd("elseif");
		return inclHandleElseif(property);
	}
	if ((s = scmdIsDirective(s0,"endif")) != 0) {
		return inclHandleEndif();
	}
	if ((s = scmdIsDirective(s0,"else")) != 0) {
		return inclHandleElse();
	}
	if ((s = scmdIsDirective(s0,"line")) != 0) {
		if ((s = scmdScanInteger(s, &lno)) == 0)
			return botchSysCmd("line");
		if (!scmdScanFName(s, &fname))  fname = 0;
		return inclHandleLine(lno, fname);
	}

	return inclHandleUnknown();
}

/*
 * Handle the include directive if we are in an active section
 */
local SrcLineList
inclHandleReinclude(String fname)
{
	if (INCLUDING(ifState)) {
		SrcLine	sl = SysCmdLine(true);
		return addSysCmd(inclFile(fname, true, false, NULL), sl);
	}
	return listNil(SrcLine);
}

/*
 * Handle the include directive if we are in an active section
 */
local SrcLineList
inclHandleInclude(String fname)
{
	if (INCLUDING(ifState)) {
		SrcLine	sl = SysCmdLine(true);
		return addSysCmd(inclFile(fname, false, false, NULL), sl);
	}
	return listNil(SrcLine);
}

local SrcLineList
inclHandleIncludeDir(String dname)
{
	if (INCLUDING(ifState)) {
		SrcLine sl = SysCmdLine(true);
		if (scmdHandleIncludeDir(dname) == -1)
			return botchSysCmd("includeDir");
		return addSysCmd(listNil(SrcLine), sl);
	}
	return listNil(SrcLine);
}

local SrcLineList
inclHandleAssert(String property)
{
	if (INCLUDING(ifState)) {
		SrcLine sl = SysCmdLine(true);
		INCL_Assert(&localAssertList, property);
		return addSysCmd(listNil(SrcLine), sl);
	}
	return listNil(SrcLine);
}

local SrcLineList
inclHandleUnassert(String property)
{
	if (INCLUDING(ifState)) {
		SrcLine sl = SysCmdLine(true);
		INCL_Unassert(&localAssertList, property);
		return addSysCmd(listNil(SrcLine), sl);
	}
	return listNil(SrcLine);
}

local SrcLineList                     
inclHandleIf(String property)
{
	Scope("inclHandleIf");

	IfState		fluid(ifState);
	SrcLineList   	result = listNil(SrcLine);

	if (INCLUDING(ifState)) {
		SrcLine sl = SysCmdLine(true);

		if (INCL_IsAssert(&localAssertList, property)) {
			ifState = ActiveIf;
			result = addSysCmd(inclFileContents(),sl);
		} 
		else {
			ifState = InactiveIf;
			result = addSysCmd(inclFileContents(),sl);
		}
	} 
	else {
		ifState = FormerlyActiveIf;
		result = inclFileContents();
	}
	Return(result);
}

local SrcLineList
inclHandleElseif(String property)
{
	SrcLine sl = SysCmdLine(true);
	if (ifState == NoIf)
		return addSysCmd(inclError(ALDOR_E_InclUnbalElseif), sl);
	if (ifState == InactiveIf) {
		if (INCL_IsAssert(&localAssertList, property))
			ifState = ActiveIf;
		return addSysCmd(listNil(SrcLine), sl);
	}
	else {
		if (ifState == ActiveIf) {
			ifState = FormerlyActiveIf;
			return addSysCmd(listNil(SrcLine), sl);
		}
		ifState = FormerlyActiveIf;
	}
	return listNil(SrcLine);
}

local SrcLineList
inclHandleElse(void)
{
	SrcLine sl = SysCmdLine(true);
	if (ifState == NoIf)
		return addSysCmd(inclError(ALDOR_E_InclUnbalElse), sl);
	else if (ifState == ActiveIf) {
		ifState = InactiveIf;
		return addSysCmd(listNil(SrcLine), sl);
	}
	else if (ifState == InactiveIf) {
		ifState = ActiveIf;
		return addSysCmd(listNil(SrcLine), sl);
	}
	return listNil(SrcLine);
}

local SrcLineList
inclHandleEndif(void)
{
	SrcLine sl = SysCmdLine(true);
	sl->isEndifLine = true;
	if (ifState == NoIf)
		return addSysCmd(inclError(ALDOR_E_InclUnbalEndif), sl);
	if (ifState == ActiveIf || ifState == InactiveIf)
		return addSysCmd(listNil(SrcLine), sl);
	return EndifLine;
}

local SrcLineList
inclHandleLine(int lno, String fname)
{
	if (INCLUDING(ifState)) {

		fileState.lineNumber = lno - 1; /* The next line is 'lno' */

#if EDIT_1_0_n2_09
		if (fname) { 
		  fileState.curFile = fname;
		  /* rhx: We trust the programmer of the #line statement
		     in the .as file that the filename is correct.
		     If in an error case the file cannot be found the
		     compiler aborts with a (Fatal Error) message.

		     #1 (Fatal Error) Could not open file `dir/file.ext' with mode `r'.

		     It would actually be very helpful for a
		     programmer if we could abort the compilation
		     already here with an error message saying that
		     the filename appearing in the #line directive
		     cannot be found.
		  */
		  fileState.curFname = fnameParse(fname);
		}
		  sposGrowGloLineTbl(fileState.curFname, fileState.lineNumber,
				     inclSerialLineNo);
#else
		if (fname) { 
		  fileState.curFile = fname;
		  fileState.curFname = inclFind(fname, fileState.curDir);
		}
		else
		  sposGrowGloLineTbl(fileState.curFname, fileState.lineNumber,
				     inclSerialLineNo);
#endif


		return listNil(SrcLine);
	}
	return listNil(SrcLine);
}

local SrcLineList
inclHandleUnknown(void)
{
	if (INCLUDING(ifState)) {
		SrcLine sl  = SysCmdLine(false);
		return addSysCmd(listNil(SrcLine), sl);
	}
	else {
		SrcPos spos = sposNew(fileState.curFname,
				      fileState.lineNumber,
				      inclSerialLineNo,
				      1);
		scmdCheck(spos, curLineString);
	}
	return listNil(SrcLine);
}

/******************************************************************************
 *
 * :: General Utilities
 *
 ****************************************************************************/

local String
inclCalcIndentLevel(String ln, int *indent)
{
	String s;
	int i;
	for (s = ln, i = 0; ; s++) {
		if (*s == ' ')
			i++;
		else if (*s == '\t') {
			if (i % TABSTOP == 0) i += TABSTOP;
			else i = ROUND_UP(i, TABSTOP);
		}
		else
			break;
	}
	*indent = i;
	return s;
}


local String
inclGetLine(FILE *file)
{
	int     c;
	String	s;

	BUF_START(inclBuffer);
	while ((c = osGetc(file)) != EOF) {
		BUF_ADD1(inclBuffer, c);
		if (c == '\n') break;
	}
	BUF_ADD1(inclBuffer, char0);

	s = bufChars(inclBuffer);
	if (c == EOF && *s == 0) return 0;
	return s;
}

/*
 * Given the list fnames [z,...,b,a] and punct "->",
 * allocate the string "'a'->'b'->...->'z'".
 */
local String	 
inclActiveFileChain(StringList fnames, String punct)
{
	StringList sl0, sl;
	Buffer	   buf;
	String	   s;
	int	   i;

	buf = bufNew();
	sl0 = listReverse(String)(fnames);

	for (sl = sl0, i = 0; sl; sl = cdr(sl), i++) {
		if (i > 0) bufPuts(buf, punct);
		bufPrintf(buf, "'%s'", car(sl));
	}
	s = bufLiberate(buf);
	listFree(String)(sl0);

	return s;
}
