/*****************************************************************************
 *
 * comsg.c: Compiler message reporting.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "opsys.h"
#include "store.h"
#if EDIT_1_0_n1_04
#include "textcolour.h"
#endif
#include "util.h"
#include "fint.h"
#include "abpretty.h"

/****************************************************************************
 *
 * :: Declarations
 *
 ****************************************************************************/

# define  comsgStream		osStdout
# define  LINE_LENGTH		80
# define  EMAX_DEFAULT		10
# define  EMAX_NONE		30000

# define  SelectDontCare	0
# define  SelectOn		1
# define  SelectOff		2

/* Defaults: */
static String		flineFmt        = "\"%s\", line %d: ";
static String		lcharFmt        = "[L%d C%d] ";
static String		remarkTag       = "[Remark]  ";
static String		warningTag      = "[Warning] ";
static String		errorTag        = "[Error]   ";
static String		fatalTag        = "[Fatal]   ";
static String		noteTag		= "[Note]    ";
static String		preview	        = "[Message Preview] ";
static String		afterMacEx      = "[After Macro Expansion] ";
static String		expandedExpr	= "Expanded expression was: ";

/* Msg control: */
static int		comsgErrorMax	= EMAX_DEFAULT;

/*These are controlled by -M<n> */
static Bool		comsgDoWarnings	= true;
static Bool		comsgDoNumber   = true;
static Bool		comsgDoSource   = true;
static Bool		comsgDoDetails  = true;
static Bool		comsgDoNotes	= true;
static Bool		comsgDoRemarks	= false;
/* These aren't */
static Bool		comsgDoSort     = true;
static Bool		comsgDoMacText  = true;
static Bool		comsgDoAbbrev   = true;
static Bool		comsgDoHuman    = true;
static Bool		comsgDoInspect  = false;
static Bool		comsgDoPreview  = false;
static Bool		comsgDoName     = false;
static Bool		comsgDoRelease  = true;

/* Database: */
static Bool		comsgIsInit     = 0;
static Bool		comsgIsOpen     = 0;
static FileName		comsgFile       = 0;

/* The messages: */
static CoMsgList	messages	= 0;
static int		nmessages       = 0;

static int		nErrors		= 0;
static int		nWarnings	= 0;
static int		nRemarks	= 0;
static int		nNotes		= 0;

static int		comsgPromptSize = 0;

/*
 * Keep track of the last error position for appending messages at the end.
 * comsgInit allocates abMaxPos to avoid allocating in comsgFini.
 */
static AbSyn		abMaxPos	= NULL;

static CoMsg 		lastCoMsgSeeNote = NULL;
static FileName		fnameRef	 = NULL;

void (*defBreakLoop)(Bool forHuman, int msgc, CoMsg* msgv) = 0;

/* Forward declarations: */
local  String		comsgOpt	(String prefix, String whole);
extern int		comsgSetOption	(String opt);
local  void		comsgSetStripDir(String base);

local  void		comsgInitSelect (void);
local  int		comsgSelectState(Msg msg);

extern Bool		comsgOkRemark	(Msg);
extern Bool		comsgOkWarning	(Msg);

extern Bool		comsgOkAbbrev	(void);
extern Bool		comsgOkDetails  (void);

extern void		comsgOpen	(void);
extern void		comsgClose	(void);
extern String		comsgString	(Msg);
extern String		comsgName	(Msg);

extern void		comsgInit	(void);
extern void		comsgFini	(void);
extern int		comsgErrorCount (void);

local  String		comsgText	(Msg, va_list);
local  CoMsg		comsgNew	(CoMsgTag tag, Length serial,
					 AbSyn, Msg, String text);
local  void		comsgFree       (CoMsg msg);
local  int		comsgCmpPtr	(CoMsg *pmsg1, CoMsg *pmsg2);

extern void		comsgRemark	(AbSyn, Msg fmt, ...);
extern void		comsgWarning	(AbSyn, Msg fmt, ...);
extern void		comsgError	(AbSyn, Msg fmt, ...);
extern void		comsgFatal	(AbSyn, Msg fmt, ...);

extern void		comsgNRemark	(AbSyn, Msg fmt, ...);
extern void		comsgNWarning	(AbSyn, Msg fmt, ...);
extern void		comsgNError	(AbSyn, Msg fmt, ...);
extern void		comsgNote	(AbSyn, Msg fmt, ...);

extern CoMsg		comsgVRemark	(AbSyn, Msg fmt, va_list);
extern CoMsg		comsgVWarning	(AbSyn, Msg fmt, va_list);
extern CoMsg		comsgVError	(AbSyn, Msg fmt, va_list);
extern void		comsgVFatal	(AbSyn, Msg fmt, va_list);

local  CoMsg		comsgVDo	(CoMsgTag tag, AbSyn, Msg, va_list);

extern void		comsgReportOne	(FILE *, CoMsg comsg);
local  void		comsgReportFile (FILE *, int comsgc, CoMsg *comsgv);
local  void		comsgReportLine (FILE *, int comsgc, CoMsg *comsgv);

local  void		comsgPrintDots	(FILE *, int comsgc, CoMsg *);
local  void		comsgPrintLead	(FILE *, Length serial, Msg msg,
					 CoMsgTag tag, SrcPos, int noteNumber);
local  void		comsgPrintMacEx (FILE *, SrcPos, AbSyn);
local  void		comsgPrintSeeNotes (FILE *fout, CoMsgList notes);
extern int		comsgFPrintf	(FILE *, Msg msg, ...);
extern int		comsgVFPrintf	(FILE *, Msg msg, va_list);

local void		comsgSetPromptSize	(int);

/*****************************************************************************
 *
 * :: Option Control
 *
 ****************************************************************************/

local String
comsgOpt(String prefix, String whole)
{
	if (strAIsPrefix(prefix, whole))
		return whole + strlen(prefix);
	else
		return 0;
}

int
comsgSetOption(String opt)
{
	Bool	isOn;
	String	arg;

	if (!opt) return -1;

	/*
	 * Handle specific options.
	 */
	if ((arg = comsgOpt("emax=", opt)) != 0) {
		if (!isdigit(arg[0])) return -1;
		comsgErrorMax = atoi(arg);
		return 0;
	}
	if ((arg = comsgOpt("no-emax", opt)) != 0) {
		if (arg[0] != 0) return -1;
		comsgErrorMax = EMAX_NONE;
		return 0;
	}
	if ((arg = comsgOpt("0", opt)) != 0) {
		if (arg[0] != 0) return -1;
		comsgDoWarnings = false;
		comsgDoNumber   = false;
		comsgDoSource   = false;
		comsgDoDetails  = false;
		comsgDoNotes    = false;
		comsgDoRemarks  = false;
		return 0;
	}
	if ((arg = comsgOpt("1", opt)) != 0) {
		if (arg[0] != 0) return -1;
		comsgDoWarnings = true;
		comsgDoNumber   = true;
		comsgDoSource   = false;
		comsgDoDetails  = false;
		comsgDoNotes    = false;
		comsgDoRemarks  = false;
		return 0;
	}
	if ((arg = comsgOpt("2", opt)) != 0) {
		if (arg[0] != 0) return -1;
		comsgDoWarnings = true;
		comsgDoNumber   = true;
		comsgDoSource   = true;
		comsgDoDetails  = true;
		comsgDoNotes	= true;
		comsgDoRemarks  = false;
		return 0;
	}
	if ((arg = comsgOpt("3", opt)) != 0) {
		if (arg[0] != 0) return -1;
		comsgDoWarnings = true;
		comsgDoNumber   = true;
		comsgDoSource   = true;
		comsgDoDetails  = true;
		comsgDoNotes	= true;
		comsgDoRemarks  = true;
		return 0;
	}
	if ((arg = comsgOpt("base=", opt)) != 0) {
		if (arg[0] == 0) return -1;
		comsgSetStripDir((String)arg);
		return 0;
	}
	if ((arg = comsgOpt("no-base", opt)) != 0) {
		if (arg[0] == 0) return -1;
		comsgSetStripDir((String)NULL);
		return 0;
	}
	if ((arg = comsgOpt("db=", opt)) != 0) {
		if (arg[0] == 0) return -1;
		comsgClose();
		comsgFile = fileRdFind(libSearchPath(), arg, FTYPE_MSG);
		if (!comsgFile) return -1;
		comsgOpen();
		return 0;
	}
	if ((arg = comsgOpt("no-db", opt)) != 0) {
		if (arg[0] != 0) return -1;
		comsgClose();
		comsgFile = 0;
		comsgOpen();
		return 0;
	}

	/*
	 * Things which are either on or off.
	 */
	isOn = true;
	while ((arg = comsgOpt("no-", opt)) != 0) {
		isOn = !isOn;
		opt  = arg;
	}
	if (opt[0] == 0) return -1;

	if (strAEqual(opt, "inspect")) {
		comsgDoInspect = isOn;
		return 0;
	}
	if (strAEqual(opt, "sort")) {
		comsgDoSort = isOn;
		return 0;
	}
	if (strAEqual(opt, "source")) {
		comsgDoSource = isOn;
		return 0;
	}
	if (strAEqual(opt, "name")) {
		comsgDoName = isOn;
		return 0;
	}
	if (strAEqual(opt, "abbrev")) {
		comsgDoAbbrev = isOn;
		return 0;
	}
	if (strAEqual(opt, "mactext")) {
		comsgDoMacText = isOn;
		return 0;
	}
	if (strAEqual(opt, "details")) {
		comsgDoDetails = isOn;
		return 0;
	}
	if (strAEqual(opt, "preview")) {
		comsgDoPreview = isOn;
		return 0;
	}
	if (strAEqual(opt, "human")) {
		comsgDoHuman = isOn;
		return 0;
	}
	if (strAEqual(opt, "remarks")) {
		comsgDoRemarks	= isOn;
		return 0;
	}
	if (strAEqual(opt, "warnings")) {
		comsgDoWarnings = isOn;
		return 0;
	}
	if (strAEqual(opt, "number")) {
		comsgDoNumber = isOn;
		return 0;
	}
	if (strAEqual(opt, "release")) {
		comsgDoRelease = isOn;
		return 0;
	}

	if (strAEqual(opt, "notes")) {
		comsgDoNotes = isOn;
		return 0;
	}

	/* Specific named messages.  */

	return comsgSelectByName(opt, isOn);
}

void
comsgSetInteractiveOption()
{
	comsgDoDetails  = true;
	comsgDoWarnings = true;
	comsgDoNotes 	= false;
	comsgErrorMax 	= EMAX_NONE;
}

Bool
comsgOkAbbrev(void)
{
	return comsgDoAbbrev;
}

Bool
comsgOkDetails(void)
{
	return comsgDoDetails;
}

Bool
comsgOkMacText(void)
{
	return comsgDoMacText;
}

Bool
comsgOkBreakLoop(void)
{
	return !comsgDoHuman && comsgDoInspect;
}

/* Set by -M strip=<dir> */
static String comsgStripDirName = (String)NULL;

local void
comsgSetStripDir(String str)
{
	/* Release any storage associated with a previous value */
	if (comsgStripDirName != (String)NULL) strFree(comsgStripDirName);

	/* Duplicate the argument if not NULL */
	comsgStripDirName = (str != (String)NULL) ? strCopy(str) : str;
}

local String
comsgStripDir(void)
{
	return comsgStripDirName;
}

Bool
comsgOkRelease(void)
{
	return comsgDoRelease;
}

/*****************************************************************************
 *
 * :: Message Filter
 *
 ****************************************************************************/

static UByte	*comsgFilterV =	 0;
static int	comsgFilterC  = -1;

local void
comsgInitSelect(void)
{
	int	i;

	if (comsgFilterV) return;

	msgDefaults(comsgdb_msgs);

	comsgFilterC = 1 + msgMax(ALDOR_GROUP);
	comsgFilterV = (UByte *) stoAlloc(OB_Other,comsgFilterC*sizeof(UByte));

	for (i = 0; i < comsgFilterC; i++)
		comsgFilterV[i] = SelectDontCare;
}

int
comsgSelectByName(String name, Bool isOn)
{
	Msg	msg;
	int	mno;
#if EDIT_1_0_n2_03
	String  s = (String)NULL;
 
	/* Allow AXL_ as well as ALDOR_ */
	if (strAIsPrefix("AXL_", name)) {
		s = strConcat("ALDOR_", name + 4);
		comsgWarning(NULL, ALDOR_W_OldMsgPrefix);
		name = s;
	}
#endif

	comsgInitSelect();

	msg = msgByAName(ALDOR_GROUP, name);
#if EDIT_1_0_n2_03
	if (s) strFree(s);
#endif
	if (msg == MSG_NOT_FOUND) return -1;

	mno = msgNumber(ALDOR_GROUP, msg);
	comsgFilterV[mno] = isOn ? SelectOn : SelectOff;

	return 0;
}

local int
comsgSelectState(Msg msg)
{
	int	n;

	comsgInitSelect();

	n = msgNumber(ALDOR_GROUP, msg);
	if (n == MSG_NOT_FOUND || n >= comsgFilterC) return SelectDontCare;

	return comsgFilterV[n];
}

Bool
comsgOkRemark(Msg msg)
{
	int msgstate = comsgSelectState(msg);
	if (comsgDoRemarks  && msgstate != SelectOff) return true;
	if (!comsgDoRemarks && msgstate == SelectOn)  return true;
	return false;
}

Bool
comsgOkWarning(Msg msg)
{
	int msgstate = comsgSelectState(msg);
	if (comsgDoWarnings  && msgstate != SelectOff) return true;
	if (!comsgDoWarnings && msgstate == SelectOn)  return true;
	return false;
}

/*****************************************************************************
 *
 * :: Initialize-Finalize
 *
 ****************************************************************************/

/*
 * comsgOpen();
 * comsgInit(); comsgError...; comsgError...; comsgFini();
 * ...
 * comsgClose();
 */

void
comsgOpen(void)
{
	if (!comsgIsOpen) {
		comsgIsOpen = 1;
		msgDefaults(comsgdb_msgs);
		if (comsgFile) msgOpen(comsgFile);
	}

	flineFmt	= comsgString(ALDOR_P_MsgSposFileLine);
	lcharFmt	= comsgString(ALDOR_P_MsgSposLineChar);
	remarkTag	= comsgString(ALDOR_P_MsgTagRemark);
	warningTag	= comsgString(ALDOR_P_MsgTagWarning);
	errorTag	= comsgString(ALDOR_P_MsgTagError);
	fatalTag	= comsgString(ALDOR_P_MsgTagFatal);
	noteTag		= comsgString(ALDOR_P_MsgTagNote);
	preview		= comsgString(ALDOR_P_MsgPreview);
	afterMacEx	= comsgString(ALDOR_P_MsgAfterMacEx);
	expandedExpr	= comsgString(ALDOR_P_MsgExpandedExpr);

}

void
comsgClose(void)
{
	if (comsgIsOpen) {
		msgClose();
		comsgIsOpen = 0;
		comsgFile   = 0;
	}
}

/* Returns a pointer to the shared msg. */
String
comsgString(Msg msg)
{
	return msgGet(ALDOR_GROUP, msg);
}


String
comsgName(Msg msg)
{
	return msgName(ALDOR_GROUP, msg);
}

void
comsgInit(void)
{
	if (!comsgIsInit) {
		comsgOpen();

		messages	= 0;
		nmessages       = 0;
		nErrors		= 0;
		nWarnings	= 0;
		nRemarks	= 0;
		comsgIsInit	= 1;

		abMaxPos	= abNewNothing(sposNone);
	}
}

void
comsgFini(void)
{
	static Bool	inFini = false;

	if (comsgIsInit && inFini == false) {
		CoMsgList msgl;
		int	  msgc;
		CoMsg	  *msgv;

		inFini = true;

		abSetPos(abMaxPos, sposEnd());

		/* Add message summary, if advised. */
		if (nErrors + nWarnings + nRemarks > 0) {
			Msg whatsUp = (nErrors==0)? ALDOR_R_MsgCongratulations
				                  : ALDOR_R_MsgCondolences;
			comsgRemark(abMaxPos, ALDOR_R_MsgCountMessages,
					    nErrors, nWarnings, nRemarks + 2);
			comsgRemark(abMaxPos, whatsUp);
		}

		/* Stash the list. */
		msgl	    = messages;
		messages    = 0;
		comsgIsInit = 0;

		/* Convert message list to vector. */
		msgc = listLength(CoMsg)(msgl);
		msgv = (CoMsg *) stoAlloc(OB_Other, msgc*sizeof(CoMsg));
		listFillVector(CoMsg)(msgv, msgl);

		/* Do the job and clean up. */
		comsgReportFile(comsgStream, msgc, msgv);
		if (comsgDoInspect) (*defBreakLoop)(comsgDoHuman, msgc, msgv);
		stoFree((Pointer) msgv);
		listFreeDeeply(CoMsg)(msgl, comsgFree);

		inFini = false;
	}
}


int
comsgErrorCount(void)
{
	return nErrors;
}


/*****************************************************************************
 *
 * :: CoMsg Objects
 *
 ****************************************************************************/

local String
comsgText(Msg msg, va_list argp)
{
	String fmt = comsgString(msg);
	String s   = strVPrintf(fmt, argp);
	return s;
}

local CoMsg
comsgNew(CoMsgTag tag, Length serial, AbSyn ab, Msg msg, String text)
{
	CoMsg comsg;

	comsg = (CoMsg) stoAlloc((int) OB_CoMsg, sizeof(*comsg));
	comsg->tag    = tag;
	comsg->serial = serial;
	comsg->pos    = ab ? abPos(ab) : sposNone;
	comsg->node   = abCopy(ab);
	comsg->msg    = msg;
	comsg->text   = text;
	comsg->notes.noteList = NULL;
	return comsg;
}

local void
comsgFree(CoMsg msg)
{
	abFree(msg->node);
	if (msg->notes.noteList && msg->tag != COMSG_NOTE)
		listFree(CoMsg)(msg->notes.noteList);
	stoFree((Pointer) msg->text);
	stoFree((Pointer) msg);
}

local int
comsgCmpPtr(CoMsg *pmsg1, CoMsg *pmsg2)
{
	return sposCmp((*pmsg1)->pos, (*pmsg2)->pos);
}


/*****************************************************************************
 *
 * :: Functions to collect messages.  Remark/Warning/Error/Fatal.
 *
 *****************************************************************************/

void
comsgRemark(AbSyn ab, Msg msg, ...)
{
	va_list argp;
	va_start(argp, msg);
	comsgVRemark(ab, msg, argp);
	va_end(argp);
}

void
comsgWarning(AbSyn ab, Msg msg, ...)
{
	va_list argp;
	va_start(argp, msg);
	comsgVWarning(ab, msg, argp);
	va_end(argp);
}

void
comsgWarnPos(SrcPos pos, Msg msg, ...)
{
	va_list argp;
	va_start(argp, msg);
	comsgVWarnPos(pos, msg, argp);
	va_end(argp);
}

void
comsgError(AbSyn ab, Msg msg, ...)
{
	va_list argp;

	va_start(argp, msg);
	comsgVError(ab, msg, argp);
	va_end(argp);
}

void
comsgFatal(AbSyn ab, Msg msg, ...)
{
	va_list argp;
	va_start(argp, msg);
	comsgVFatal(ab, msg, argp);
	va_end(argp);
}

/*
 * The first arg must be comsgVError, comsgVWarning, ... 
 * Example:
 *	comsgNotePoint(comsgVError, ab, msg, str);
 *	comsgNote(ab, msg, str);
 *	comsgNote(ab, msg, str);
 */
local void
comsgNotePoint(CoMsg (* comsgf)(AbSyn, Msg, va_list), AbSyn ab, 
               Msg msg, va_list argp)
{
        lastCoMsgSeeNote = comsgf(ab, msg, argp);
        fnameRef = sposFile(lastCoMsgSeeNote->pos);
        lastCoMsgSeeNote->notes.noteList = listNil(CoMsg);
}

void
comsgNRemark(AbSyn ab, Msg msg, ...)
{
	va_list argp;
	va_start(argp, msg);
	comsgNotePoint(comsgVRemark, ab, msg, argp);
	va_end(argp);
}
 
void
comsgNWarning(AbSyn ab, Msg msg, ...)
{
	va_list argp;
	va_start(argp, msg);

	comsgNotePoint(comsgVWarning, ab, msg, argp);

	va_end(argp);
}
 
void
comsgNError(AbSyn ab, Msg msg, ...)
{
	va_list argp;
	va_start(argp, msg);

	comsgNotePoint(comsgVError, ab, msg, argp);

	va_end(argp);
}
 
void
comsgNote(AbSyn ab, Msg msg, ...)
{
	CoMsg comsg;
	va_list argp;
	int lnRef, cnRef;
	SrcPos spos;
	String s, fmt;
	Buffer obuf;


	if (!comsgDoNotes)
		return;

	assert(lastCoMsgSeeNote);

	va_start(argp, msg);
	obuf = bufNew();
	comsg = comsgVDo(COMSG_NOTE, ab, msg, argp);
	assert(comsg);
	comsg->notes.noteNumber = ++nNotes;
	lastCoMsgSeeNote->notes.noteList =
		listCons(CoMsg)(comsg,lastCoMsgSeeNote->notes.noteList);
	spos  = comsg->pos;

	if (sposIsSpecial(spos))
		bug("Bad case in comsgNote");

	lnRef = sposLine(lastCoMsgSeeNote->pos);
	cnRef = sposChar(lastCoMsgSeeNote->pos);

	s = comsg->text;
	bufPrintf(obuf, "%s", s);

	if (sposFile(comsg->pos) == fnameRef) {
		fmt = comsgString(ALDOR_P_MsgCfNote);
		bufPrintf(obuf, " ");
		bufPrintf(obuf, fmt, lnRef, cnRef);
	}
	else {
		fmt = comsgString(ALDOR_P_MsgCfFarNote);
		bufPrintf(obuf, "\n");
		bufPrintf(obuf, fmt, fnameUnparseStatic(fnameRef), lnRef, cnRef);
	}

	comsg->text = bufLiberate(obuf);
	strFree(s);
	va_end(argp);
}

CoMsg
comsgVRemark(AbSyn ab, Msg msg, va_list argp)
{
	CoMsg comsg = NULL;

	if (comsgOkRemark(msg)) {
		nRemarks++;
		comsg = comsgVDo(COMSG_REMARK, ab, msg, argp);
	}
	return comsg;
}

CoMsg
comsgVWarning(AbSyn ab, Msg msg, va_list argp)
{
	CoMsg comsg = NULL;

	if (comsgOkWarning(msg)) {
		nWarnings++;
		comsg = comsgVDo(COMSG_WARNING, ab, msg, argp);
	}
	return comsg;
}

CoMsg
comsgVWarnPos(SrcPos pos, Msg msg, va_list argp)
{
	CoMsg comsg = NULL;
	AbSyn ab;
	ab = abNewNothing(pos);
	if (comsgOkWarning(msg)) {
		nWarnings++;
		comsg = comsgVDo(COMSG_WARNING, ab, msg, argp);
	}
	return comsg;
}

CoMsg
comsgVError(AbSyn ab, Msg msg, va_list argp)
{
	CoMsg comsg;

	nErrors++;
	comsg = comsgVDo(COMSG_ERROR, ab, msg, argp);
	
	if (nErrors == comsgErrorMax)
		comsgFatal(abMaxPos, ALDOR_F_MsgTooManyErrors);

	return comsg;
}

void
comsgVFatal(AbSyn ab, Msg msg, va_list argp)
{
	nErrors++;
	comsgVDo(COMSG_FATAL, ab, msg, argp);
	comsgFini();
	exitFailure();
}

local CoMsg
comsgVDo(CoMsgTag tag, AbSyn ab, Msg msg, va_list argp)
{
	CoMsg comsg = NULL;

	if (!comsgIsInit || !ab) {
		SrcPos spos = ab ? abPos(ab) : sposNone;
		comsgInit();
#if EDIT_1_0_n1_04
		fprintf(comsgStream, "%s", tcolPrefix(tag));
		comsgPrintLead(comsgStream, ++nmessages, msg, tag, spos, int0);
		comsgVFPrintf (comsgStream, msg, argp);
		fprintf(comsgStream, "%s", tcolPostfix(tag));
#else
		comsgPrintLead(comsgStream, ++nmessages, msg, tag, spos, int0);
		comsgVFPrintf (comsgStream, msg, argp);
#endif
	}
	else {
		String text  = comsgText(msg, argp);
		Bool   debug = false;
		DEBUG(debug = true);

		if (ab) abSetPos(abMaxPos,
			         sposMax(abPos(abMaxPos), abPos(ab)));

		comsg = comsgNew(tag, ++nmessages, ab, msg, text);
		messages = listCons(CoMsg)(comsg, messages);

		if (comsgDoPreview || debug) {
			fprintf(comsgStream, "%s\n", preview);
			comsgReportOne(comsgStream, comsg);
			if (comsgDoInspect)
				(*defBreakLoop)(comsgDoHuman, 1, &comsg);
		}
	}

	return comsg;
}

/*****************************************************************************
 *
 * :: Sorting and display of lines and their messages.
 *
 ****************************************************************************/

/*
 * Print each line and its messages as:
 *
 * "foo.sp", line 10: f(x) == y +-+ 1
 *		      ........^..^
 * [L10 C 9] (Warning) Variable y is undeclared.
 * [L10 C12] Improper syntax.
 */

void
comsgReportOne(FILE *fout, CoMsg comsg)
{
	comsgReportFile(fout, 1, &comsg);
}

local void
comsgReportFile(FILE *fout, int comsgc, CoMsg *comsgv)
{
	int	i0, n, glno;

	if (comsgc <= 0) return;

	comsgInit();

	/* Reverse the messages and sort if requested. */
	for (i0 = 0, n = comsgc - 1; i0 < comsgc/2; i0++, n--) {
		CoMsg m    = comsgv[n];
		comsgv[n]  = comsgv[i0];
		comsgv[i0] = m;
	}

	/* Messages at the same position are reported in the order
	 * in which they were generated.
	 */
	if (comsgDoSort)
		lisort(comsgv, comsgc, sizeof(CoMsg),
		       (int (*)(ConstPointer, ConstPointer)) comsgCmpPtr);

	for (i0 = 0; i0 < comsgc; i0 += n) {
		glno = sposGlobalLine(comsgv[i0]->pos);
		for (n = 1; i0 + n < comsgc; n++)
			if (sposGlobalLine(comsgv[i0+n]->pos) != glno) break;
		comsgReportLine(fout, n, comsgv+i0);
	}


	/* Flush the output stream just incase */
	(void)fflush(fout);
}

local void
comsgReportLine(FILE *fout, int comsgc, CoMsg *comsgv)
{
	int	indent;
	String  lastText = "";

	if (comsgc <= 0) return;

	if (comsgDoSource) {
		indent = comsgPrintLine(fout, comsgv[0]->pos);
		fputcTimes(' ', indent, fout);
		comsgPrintDots(fout, comsgc, comsgv);
	}

	for ( ; comsgc-- > 0; comsgv++) {
		CoMsg	co = *comsgv;
		if (strcmp(lastText, co->text)) {
#if EDIT_1_0_n1_04
			fprintf(fout, "%s", tcolPrefix(co->tag));
			comsgPrintLead(fout, co->serial, co->msg, co->tag,
				       co->pos, co->notes.noteNumber);
			fprintf(fout, "%s", co->text);
			if (co->notes.noteList && co->tag != COMSG_NOTE)
				comsgPrintSeeNotes(fout, co->notes.noteList);
			fprintf(fout, "%s\n", tcolPostfix(co->tag));
			comsgPrintMacEx(fout, co->pos, co->node);
#else
			comsgPrintLead(fout, co->serial, co->msg, co->tag,
				       co->pos, co->notes.noteNumber);
			fprintf(fout, "%s", co->text);
			if (co->notes.noteList && co->tag != COMSG_NOTE)
				comsgPrintSeeNotes(fout, co->notes.noteList);
			fprintf(fout, "\n");
			comsgPrintMacEx(fout, co->pos, co->node);
#endif
		}
		lastText = co->text;
	}
	if (comsgDoDetails || comsgDoSource)
		fprintf(fout, "\n");
}


/*****************************************************************************
 *
 * :: Output style.
 *
 ****************************************************************************/

/*
 * Print the source position and source line text.
 * Return the indendation for the first (possibly blank) char of the line text.
 * The value -1 indicates an error.
 */
int
comsgPrintLine(FILE *fout, SrcPos spos)
{
	Buffer	buf;
	String	s;
	int	rc, cc;
	Length	n, nu;
	Bool	splitLine = false;

	/*
	 * Get the source line.
	 */
	buf = bufNew();
	rc = sposLineText(buf, spos);
	if (rc == -1) {
		bufFree(buf);
		return (fintMode == FINT_LOOP ? comsgPromptSize : -1); 
	}

	s  = bufLiberate(buf);
	n  = strLength(s);
	nu = strUntabLength(s, TABSTOP);

	/*
	 * Print the position and source line,	splitting them if necessary.
	 */
	if (sposIsSpecial(spos))
		cc = sposPrint(fout, spos);
	else {
		String		name;
		FileName	fn = sposFile(spos);

		String		rest = (String)NULL;

		/* If -M strip=<dir>, strip <dir> from front of filename */
		name = fnameUnparseStatic(fn);
		if (comsgStripDir()) rest = strIsPrefix(comsgStripDir(), name);
		if (rest) {
			/* Change the name of the file */
			name = rest;
			fn = fnameParseStatic(name);
		}

		/* Display the line details */
		cc = fprintf(fout, flineFmt, name, sposLine(spos));

		/* Do we need to split the line? */
		if (fnameHasDir(fn) && !strEqual(fnameDir(fn), osCurDirName()))
			splitLine = true;
	}

	if (cc + nu >= LINE_LENGTH) splitLine = true;
	if (splitLine) fputc('\n', fout);

	fputsUntab(s, TABSTOP, fout);
	if (n == 0 || s[n-1] != '\n') fputc('\n', fout);

	return splitLine ? 0 : cc;
}

/*
 * Print a "...^..^" indicator line.
 */
local void
comsgPrintDots(FILE *fout, int comsgc, CoMsg *comsgv)
{
	int cno = 1;
	while (comsgc-- > 0) {
		SrcPos spos = (*comsgv++)->pos;
		if (!sposIsSpecial(spos)) {
			int	Dcno = sposChar(spos) - cno;
			if (Dcno >= 0) {
				fputcTimes('.', Dcno, fout);
				fprintf(fout, "^");
			}
			cno = sposChar(spos) + 1;
		}
	}
	fprintf(fout, "\n");
}

/*
 * Print the position and any prefixes for a message.
 * E.g. "{ALDOR_E_Stupid} [L32 C47] #3 (Error) (After macro expansion)"
 */
local void
comsgPrintLead(FILE *fout, Length serial, Msg msg, CoMsgTag tag,
               SrcPos spos, int noteNumber)
{
	String	tagstring;

	if (comsgDoName)
		fprintf(fout, "{%s} ", comsgName(msg));

	if (!sposIsSpecial(spos)) {
		FileName fn = sposFile(spos);
		int	 ln = sposLine(spos), cn = sposChar(spos);

		if (comsgDoSource)
			fprintf(fout, lcharFmt, ln, cn);
		else
			fprintf(fout, flineFmt, fnameUnparseStatic(fn), ln);
	}

	if (comsgDoNumber)
#if EDIT_1_0_n1_07
		fprintf(fout, "#%d ", (int) serial);
#else
		fprintf(fout, "#%d ", serial);
#endif

	switch (tag) {
	case COMSG_REMARK:  tagstring = remarkTag;  break;
	case COMSG_WARNING: tagstring = warningTag; break;
	case COMSG_ERROR:   tagstring = errorTag;   break;
	case COMSG_FATAL:   tagstring = fatalTag;   break;
	case COMSG_NOTE:    tagstring = noteTag;    break;
	default:	    tagstring = "";         bugBadCase(tag);
	}

	if (tag != COMSG_NOTE)
		fprintf(fout, "%s", tagstring);
	else
		fprintf(fout, tagstring, noteNumber);

	if (sposIsMacroExpanded(spos)) fprintf(fout, "%s", afterMacEx);
}

local void
comsgPrintMacEx(FILE *fout, SrcPos spos, AbSyn ab)
{
	if (! sposIsMacroExpanded(spos) || !ab) return;

	fprintf(fout, "%s", expandedExpr);
	abPrettyPrint(fout, ab);
	fprintf(fout, "\n");
}

local void
comsgPrintSeeNotes(FILE *fout, CoMsgList notes)
{
	CoMsg co;
	String fmtNote = comsgString(ALDOR_P_MsgNote);
	String fmtAnd  = comsgString(ALDOR_P_MsgConjunction);
	String fmt     = comsgString(ALDOR_P_MsgSeeNote);
	Buffer obuf    = bufNew();
	Bool first     = true;


	for (notes = listNReverse(CoMsg)(notes); notes; notes = cdr(notes)) {
		co = car(notes);
		if (first)
			first = false;
		else
			bufPrintf(obuf, cdr(notes) ? ", " : fmtAnd);
		bufPrintf(obuf, fmtNote, (int) co->notes.noteNumber);
	}
	fprintf(fout, "\n");
	fprintf(fout, fmt, bufChars(obuf));
	bufFree(obuf);
}

int
comsgFPrintf(FILE *fout, Msg msg, ...)
{
	int	cc;
	va_list argp;

	va_start(argp, msg);
	cc = comsgVFPrintf(fout, msg, argp);
	va_end(argp);

	return cc;
}

int
comsgVFPrintf(FILE *fout, Msg msg, va_list argp)
{
	int	cc;
	String	fmt;

	fmt = comsgString(msg);

	cc = vfprintf(fout, fmt, argp);
	fputc('\n', fout);
	cc++;

	return cc;
}

local void
comsgSetPromptSize(int size)
{
	comsgPromptSize = size;
}

/*
 * If fin is interactive print the prompt on fout using
 * printf-style formatting.
 */
void
comsgPromptPrint(FILE *fin, FILE *fout, String fmt, ...)
{
	int promptSize;

	va_list	argp;

	if (!osIsInteractive(fin)) return;

	va_start(argp, fmt);
	promptSize = vfprintf(fout, fmt, argp);
	va_end(argp);

	comsgSetPromptSize(promptSize);

	fflush(fout);
}

