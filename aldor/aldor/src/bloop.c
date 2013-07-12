/*****************************************************************************
 *
 * bloop.c: Interactive break loop.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axlphase.h"

/*
 * :: External state control
 */

#define MAX_CMD  200

#define MAX_MSG  50000

#define MAX_STR  8000
#define PATH_END (-1)
#define SESCAPE  '\007'

#define stoAllocVec(n, T)  ((T *) stoAlloc(OB_Other, (n) * sizeof(T)))
#define stoAllocObj(oty)   ((oty *) stoAlloc(OB_Other,sizeof(oty)))

/*****************************************************************************
 *
 * :: Initialization/Finalization
 *
 ****************************************************************************/

static FILE	*fin, *fout;
static char     * msg;

struct breakMsg {
	CoMsg	comsg;		/* Message associated with the error. */
	int	depth;		/* Distance from root to node. -1 => no path */
	int	*path;		/* Path to node, or 0. */
};

typedef struct breakMsg	*BreakMsg;


/*
 * :: Internal state control
 */
static Bool		bloopInterface  = false;
static Bool		bloopIsNice     = true;

static int		bloopMsgc       = -1;
static BreakMsg *	bloopMsgv       = 0;

static AbSyn		bloopAbRoot     = 0;
static Stab		bloopStab       = 0;

static int		bloopCurrIx     = -1;
static int *		bloopCurrAbPath = 0;
static int		bloopCurrAbDepth= -1;
static int		bloopTmpSet     = 0;

static AbSyn		bloopAbCurr	= 0;
static AbSyn		bloopAbTmp	= 0;
static AbSyn		bloopAbParent	= 0;

#define BLOOP_CURR	(bloopMsgv[bloopCurrIx])
#define BLOOP_NODE	(BLOOP_CURR->depth != -1)


char *	getTok	(char **);

/*
 * :: Action declarations
 */
local void	bloopInit	(int comsgc, CoMsg *comsgv);
local void	bloopFini	(void);

local void	bloopHandleCmds (void);
local String	bloopGetCommand (void);
local void	bloopSendString	(String, String);
local void	bloopSendMsg	(Msg);

local void	bloopMSelect	(void);
local void	bloopSelect	(int);
local void	bloopSetCurr	(void);

local void	bloopComsg	(void);
local void	bloopNotes	(void);
local void	bloopGetPos	(void);
local void	bloopResetCurr	(void);
local void	bloopPosWhere	(void);
local void	bloopWhere	(void);

local void	bloopShow	(void);
local void	bloopUse	(void);
local void	bloopMeans	(void);
local void	bloopMsg	(void);
local void	bloopSeman	(void);
local void	bloopScope	(void);
local void	bloopUgly	(void);
local void	bloopNice	(void);

local void	bloopHome	(void);
local void	bloopUp		(void);
local void	bloopDown	(void);
local void	bloopNext	(void);
local void	bloopPrev	(void);

local void	bloopStabPretty	(void);

local Bool	breakToQuit = false;

/*****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

AbSyn
breakSetRoot(AbSyn isNew)
{
	AbSyn	old = bloopAbRoot;
	bloopAbRoot = isNew;
	return	old;
}

void
breakInterrupt(void)
{
	if (bloopInterface) {
		fprintf(stderr, "No compiler messages!\n");
		fflush(stderr);
		bloopSendString("done", "\"\"");
		exitSetHandler((ExitFun) 0);
		exitFailure();
	}
}

void
breakLoop(Bool forHuman, int msgc, CoMsg *msgv)
{
	fin            = osStdin;
	fout           = osStdout;
	bloopInterface = !forHuman;

	if (msgc <= 0) return;

	if (!bloopInterface) bloopSendMsg(ALDOR_M_BreakEnter);

	bloopInit(msgc, msgv);
	bloopSelect(int0);
	bloopHandleCmds();
	bloopFini();

	if (!bloopInterface) bloopSendMsg(ALDOR_M_BreakExit);

	if (breakToQuit) exitFailure();
}

/*
 * :: The break loop
 */
local void
bloopHandleCmds()
{
	String		cmd;

	if (bloopInterface) {
		sprintf(msg, "%d", bloopMsgc);
		bloopSendString("nummsgs", msg);
	}
	for (;;) {
		cmd = bloopGetCommand();
		/*
		 * Test for commands alphabetically.
		 */
		strcpy(msg, "");
		if (strEqual("", cmd)) {
			bloopSendMsg(ALDOR_M_BreakMsgHelpAvail);
		}
		else if (strIsPrefix("down", cmd)) {
			bloopDown();
		}
		else if (strIsPrefix("getcomsg", cmd)) {
			bloopComsg();
		}
		else if (strIsPrefix("notes", cmd)) {
			bloopNotes();
		}
		else if (strIsPrefix("getpos", cmd)) {
			bloopGetPos();
		}
		else if (strIsPrefix("help", cmd)) {
			bloopSendMsg(ALDOR_M_BreakHelp);
		}
		else if (strIsPrefix("home", cmd)) {
			bloopHome();
		}
		else if (strIsPrefix("means", cmd)) {
			bloopMeans();
		}
		else if (strIsPrefix("mselect", cmd)) {
			bloopMSelect();
		}
		else if (strIsPrefix("mnext", cmd)) {
			bloopSelect(bloopCurrIx+1);
		}
		else if (strIsPrefix("mprev", cmd)) {
			bloopSelect(bloopCurrIx-1);
		}
		else if (strIsPrefix("msg", cmd)) {
			bloopMsg();
		}
		else if (strIsPrefix("next", cmd)) {
			bloopNext();
		}
		else if (strIsPrefix("nice", cmd)) {
			bloopNice();
		}
		else if (strIsPrefix("pos", cmd)) {
			bloopPosWhere();
		}
		else if (strIsPrefix("prev", cmd)) {
			bloopPrev();
		}
		else if (strIsPrefix("quit", cmd)) {
			breakToQuit = true;
			exitSetHandler((ExitFun) 0);
			break;
		}
		else if (strIsPrefix("scope", cmd)) {
			bloopScope();
		}
		else if (strIsPrefix("seman", cmd)) {
			bloopSeman();
		}
		else if (strIsPrefix("show", cmd)) {
			bloopShow();
		}
		else if (strIsPrefix("use", cmd)) {
			bloopUse();
		}
		else if (strIsPrefix("ugly", cmd)) {
			bloopUgly();
		}
		else if (strIsPrefix("up",   cmd)) {
			bloopUp();
		}
		else if (strIsPrefix("where", cmd)) {
			bloopWhere();
		}
		else {
			bloopSendMsg(ALDOR_M_BreakHelp);
			if (!bloopInterface) {
				sprintf(msg, comsgString(ALDOR_M_BreakNoCmd), cmd);
				bloopSendString("msg", msg);
			}
		}
	}
}

local void
bloopInit(int msgc, CoMsg *comsgv)
{
	int	 i, j, ix;

        msg = (char *) stoAlloc(OB_Other, MAX_MSG);

	/*
	 * Filter out the notes from the messages.
	 */
	bloopMsgc = msgc;
	for (i = 0; i < msgc; i++)
		if (comsgv[i]->tag == COMSG_NOTE) bloopMsgc--;
	bloopMsgv = stoAllocVec(bloopMsgc + 1, BreakMsg);
	
	/*
	 * Fill a null-terminated break-msg vector.
	 */
	for (i = 0, ix = 0; i < msgc; i++) {
		if (comsgv[i]->tag != COMSG_NOTE) {
			bloopMsgv[ix] = stoAllocObj(struct breakMsg);
			bloopMsgv[ix]->comsg = comsgv[i];
			bloopMsgv[ix]->depth = -1;
			bloopMsgv[ix]->path  = 0;
			ix++;
		}
	}

	/*
	 * If there is a root, set up path-to-node index vectors.
	 */
	if (bloopAbRoot) {
		int		len;
		AIntList	p, q;
		AbSyn		node;

		for (i = 0; i < bloopMsgc; i++) {
			node = bloopMsgv[i]->comsg->node;
			if (!node) continue;

			p = abPathToNode(bloopAbRoot, node, abEqual, &len,
					 (AIntList) 0);
			if (len >= 0) {
				bloopMsgv[i]->depth = len;
				bloopMsgv[i]->path  = stoAllocVec(len+1, int);

				for (q = p, j = 0; q; q = cdr(q), j++)
					bloopMsgv[i]->path[j] = car(q);
				bloopMsgv[i]->path[j] = PATH_END;
			}
			listFree(AInt)(p);
		}
	}

	/*
	 * Initialize relevant global variables.
	 */
	bloopStab   = stabFile();

	if (bloopAbRoot) {
		int ht = abTreeHeight(bloopAbRoot);
		bloopCurrAbPath = stoAllocVec(ht + 1, int);
	}
}

local void
bloopFini(void)
{
	bloopStab	 = 0;

	bloopCurrIx      = -1;
	bloopMsgc        = -1;
	stoFree((Pointer) bloopMsgv);

	bloopCurrAbDepth = -1;
	if (bloopCurrAbPath) {
		stoFree((Pointer) bloopCurrAbPath);
		bloopCurrAbPath	 = 0;
	}

	bloopAbCurr	 = 0;
	bloopAbParent	 = 0;
        
        stoFree(msg);
}

local void
bloopSelect(int ix)
{
	int i;

	if (ix < 0 || ix >= bloopMsgc) {
		bloopSendMsg(ALDOR_M_BreakMsgCantSelect);
		return;
	}

	if (bloopTmpSet) {
		bloopAbCurr = bloopAbTmp;
		bloopAbTmp  = 0;
		bloopTmpSet = 0;
		bloopResetCurr();
	}

	bloopCurrIx      = ix;
	bloopCurrAbDepth = BLOOP_CURR->depth;

	if (BLOOP_NODE && BLOOP_CURR->comsg->tag != COMSG_NOTE) {
		for (i = 0; i < bloopCurrAbDepth; i++)
			bloopCurrAbPath[i] = BLOOP_CURR->path[i];
		bloopCurrAbPath[i] = PATH_END;
		bloopSetCurr();
	}
	else {
		bloopAbParent = 0;
		bloopAbCurr   = bloopAbRoot;
	}
	/* Make this part of prompt command in bloopGetCommand. */
	if (bloopInterface)
		bloopSendString("ready", "\"\"");
}

local void
bloopMSelect(void)
{
	String		cmd;
	int		index;
	char		*s;

	cmd   = bloopGetCommand();
	s     = getTok(&cmd);
	index = atoi(s);
	bloopSelect(index);
}

local void
bloopSetCurr(void)
{
	int	i;

	if (!BLOOP_NODE) return;

	bloopStab = stabFile();

	bloopAbParent = 0;
	bloopAbCurr   = bloopAbRoot;
	if (abStab(bloopAbCurr)) bloopStab = abStab(bloopAbCurr);

	for (i = 0; bloopCurrAbPath[i] != PATH_END; i++) {
		bloopAbParent = bloopAbCurr;
		bloopAbCurr   = abArgv(bloopAbParent)[bloopCurrAbPath[i]];
		if (abStab(bloopAbCurr)) bloopStab = abStab(bloopAbCurr);
	}
}


local void
bloopSendString(char *cmd, char *str)
{
	if (bloopInterface) {
		if (str)
			fprintf(fout, "(%s \"%s\")\n", cmd, str);
		else
			fprintf(fout, "(%s \"%s\")\n", cmd, "Invalid string!");
		fflush(fout);
	}
	else {
		fprintf(fout, "%s\n", str);
		fflush(fout);
	}
}

local void
bloopSendMsg(Msg msg)
{
	bloopSendString("msg", comsgString(msg));
}

int
bloopMsgFPrintf(FILE *fout, Msg msg, ...)
{
	int	cc;
	va_list argp;

	va_start(argp, msg);
	cc = bloopMsgVFPrintf(fout, msg, argp);
	va_end(argp);

	return cc;
}

int
bloopMsgVFPrintf(FILE *fout, Msg msg, va_list argp)
{
	int	cc = 0;

	cc += fprintf(fout, "(error %c", SESCAPE);
	cc += comsgVFPrintf(fout, msg, argp);
	cc += fprintf(fout, "%c)\n", SESCAPE);
	fflush(fout);

	return cc;
}

/*
 * Read a command and return it in a static string.
 */
local String
bloopGetCommand(void)
{
	char	cmd[MAX_CMD];
	char	*s;

	if (!bloopInterface)
		prompt(fin, fout, comsgString(ALDOR_M_BreakMsgPrompt));
	else
		prompt(fin, fout, "(ready \"\")\n");

	if (fgets(cmd, sizeof(cmd), fin) == NULL)
	  return NULL;

	/* Trim white space from both ends. */
	s = cmd + strlen(cmd) - 1;
	while (s >= cmd && isspace(*s)) s--;
	if (s >= cmd && isspace(*s)) *s = 0;
	s = strCopy(cmd);
	while (*s && isspace(*s)) s++;

	return s;
}

local void
bloopComsg(void)
{
	FileName	file;
	String		fileName;
	Length		lmin, cmin;
	SrcPos		spos = BLOOP_CURR->comsg->pos;
	String		text = BLOOP_CURR->comsg->text;
	CoMsgList	notes = BLOOP_CURR->comsg->notes.noteList;

	file     = sposFile(spos);
	fileName = fnameUnparseStatic(file);
	lmin     = sposLine(spos);
	cmin     = sposChar(spos);
	sprintf(msg, "(getcomsg %d %d %c%s%c %d %d %c%s%c)\n",
#if EDIT_1_0_n1_07
		bloopCurrIx+1, (int)listLength(CoMsg)(notes),
		SESCAPE, fileName, SESCAPE,
		(int) lmin, (int) cmin, SESCAPE, text, SESCAPE);
#else
		bloopCurrIx+1, listLength(CoMsg)(notes),
		SESCAPE, fileName, SESCAPE,
		lmin, cmin, SESCAPE, text, SESCAPE);
#endif
	if (bloopInterface) {
		fprintf(fout, "%s", msg);
		fflush(fout);
	}
}

local void
bloopNotes(void)
{
	CoMsgList	notes = BLOOP_CURR->comsg->notes.noteList;

	if (BLOOP_CURR->comsg->tag == COMSG_NOTE) {
		bloopSendString("ready", "This is a note.");
		return;
	}
	fprintf(fout, "(notes %d ", bloopCurrIx+1);
	for (notes = listNReverse(CoMsg)(notes); notes; notes = cdr(notes)) {
		CoMsg	co = car(notes);
		if (co && co->notes.noteNumber)
			fprintf(fout, " %d ", co->notes.noteNumber);
	}
	fprintf(fout, ")\n");

	if (bloopInterface) {
		fprintf(fout, "%s", msg);
		fflush(fout);
	}
}

/*
 * Grab the next word from buf.
 */
char *
getTok(char **prest)
{
	char	*s = *prest, *result;

	while (*s && isspace(*s)) s++;
	result = s;

	if (*s == '\"') {
		s++;
		result++;
		while (*s != '\"') {s++;};
	}
	else
		while (*s && !isspace(*s)) {s++;};

	if (*s) {
		*s++ = 0;
		while (*s && isspace(*s)) {s++;};
	}

	*prest = (*s && !isspace(*s)) ? s : 0;

	return result;
}

local void
bloopGetPos(void)
{
	FileName	fName;
	SrcPos		pmin, pmax;
	Length		glmin, glmax, lmin = 0, cmin = 0, lmax = 0, cmax = 0;
	String		cmd;
	SrcPos		bPos;
	AbSyn		node1, node2, tmpCurr;

	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}
	else if (bloopInterface)
		bloopSendString("ready", "\"\"");
	else if (!bloopInterface) {
		bloopSendString("msg", "Cannot execute this command!");
		return;
	}

	cmd   = bloopGetCommand();
	if (cmd) lmin  = atoi(getTok(&cmd));
	if (cmd) cmin  = atoi(getTok(&cmd));
	if (cmd) lmax  = atoi(getTok(&cmd));
	if (cmd) cmax  = atoi(getTok(&cmd));
	if (*cmd) getTok(&cmd);
	bPos  = abPos(bloopAbCurr);
	fName = sposFile(bPos);
  	glmin = sposGLine(fName, lmin);
  	glmax = sposGLine(fName, lmax);
	pmin  = sposGet(glmin, cmin);
	node1 = abContainer(bloopAbRoot, pmin);
	pmax  = sposGet(glmax, cmax);
	node2 = abContainer(bloopAbRoot, pmax);

	tmpCurr = bloopAbCurr;
	bloopAbCurr = abSupremum(bloopAbRoot, node1, node2, abEqual);
	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		bloopAbCurr = tmpCurr;
		return;
	}
	else {
		bloopTmpSet = 1;
		if (!bloopAbTmp) bloopAbTmp  = tmpCurr;
		bloopResetCurr();
		bloopSendString("ready", "\"\"");
	}
}

local void
bloopResetCurr(void)
{
	int		j, len;
	AIntList	p, q;
	AbSyn		node;

	if (!bloopAbRoot) return;
	BLOOP_CURR->comsg->node = bloopAbCurr;
	BLOOP_CURR->comsg->pos  = abPos(bloopAbCurr);
	node = BLOOP_CURR->comsg->node;
	if (!node) return;

	p = abPathToNode(bloopAbRoot, node, abEqual, &len, (AIntList) 0);
	if (len >= 0) {
		BLOOP_CURR->depth = len;
		BLOOP_CURR->path = stoAllocVec(len+1, int);

		for (q = p, j = 0; q; q = cdr(q), j++)
			BLOOP_CURR->path[j] = car(q);
		BLOOP_CURR->path[j] = PATH_END;
	}
	listFree(AInt)(p);

	bloopCurrAbDepth = BLOOP_CURR->depth;

	if (BLOOP_NODE && BLOOP_CURR->comsg->tag != COMSG_NOTE) {
		for (j = 0; j < bloopCurrAbDepth; j++)
			bloopCurrAbPath[j] = BLOOP_CURR->path[j];
		bloopCurrAbPath[j] = PATH_END;
		bloopSetCurr();
	}
}

#define DEBUG_NODE_

local void
bloopPosWhere(void)
{
	SrcPos		pmin, pmax;
	Length		lmin, cmin, lmax, cmax;
#ifdef DEBUG_NODE
	FileName	fileName;
	String		fName;
	SrcPos		bloopPos;

	bloopPos = abPos(bloopAbCurr);
	fileName = sposFile(bloopPos);
	fName    = fnameUnparseStatic(fileName);
	strFree(fName);
#endif
	if (!bloopAbCurr) {
		lmin = cmin = lmax = cmax = -1;
		sprintf(msg, "(pos \"%d\" \"%d\" \"%d\" \"%d\")\n",
			-1, -1, -1, -1);
	}
	else {
		abPosSpan(bloopAbCurr, &pmin, &pmax);
/*
		pmin = abPos(bloopAbCurr);
		pmax = abEnd(bloopAbCurr);
 */
		lmin = sposLine(pmin);
		cmin = sposChar(pmin);
		lmax = sposLine(pmax);
		cmax = sposChar(pmax);
		if (lmax < lmin) lmax = lmin;
		if (cmax < cmin) cmax = cmin;
		sprintf(msg, "(pos \"%d\" \"%d\" \"%d\" \"%d\")\n",
#if EDIT_1_0_n1_07
			(int) lmin, (int) cmin, (int) lmax, (int) cmax);
#else
			lmin, cmin, lmax, cmax);
#endif
	}
	if (bloopInterface) {
		fprintf(fout, "%s", msg);
		fflush(fout);
	}
}

local void
bloopWhere(void)
{
	Length		lmin, cmin, lmax, cmax;
	SrcPos		pmin, pmax;
	FileName	file;
	String		fileName;
	SrcPos		bloopPos;

	if (!bloopAbCurr) {
		bloopPos = sposTop();
		lmin = cmin = lmax = cmax = -1;
	}
	else {
		bloopPos = abPos(bloopAbCurr);
#ifdef DEBUG_WHERE
		fprintf(stderr, "\n\t!!!Where %d\n", bloopPos);
		abPrint(stderr, bloopAbCurr);
		fprintf(stderr, "\n");
#endif
		abPosSpan(bloopAbCurr, &pmin, &pmax);
		lmin = sposLine(pmin);
		cmin = sposChar(pmin);
		lmax = sposLine(pmax);
		cmax = sposChar(pmax);
		if (lmax < lmin) lmax = lmin;
		if (cmax < cmin) cmax = cmin;
	}

	file     = sposFile(bloopPos);
	if (file)
		fileName = fnameUnparseStatic(file);
	else
		fileName = 0;

	if (fileName)
		sprintf(msg, "(where \"%s\" %d %d %d %d)\n",
#if EDIT_1_0_n1_07
			fileName, (int) lmin, (int) cmin, (int) lmax, (int) cmax);
#else
			fileName, lmin, cmin, lmax, cmax);
#endif
	else
		sprintf(msg, "(where \"%s\" %d %d %d %d)\n",
			fileName, -1, -1, -1, -1);
	if (bloopInterface) {
		fprintf(fout, "%s", msg);
		fflush(fout);
	}
#ifdef DEBUG_WHERE
	fprintf(stderr, msg);
#endif
}

local void
bloopNice(void)
{
	bloopIsNice = true;
	bloopSendString("msg", "nice");
}

local void
bloopUgly(void)
{
	bloopIsNice = false;
	bloopSendString("msg", "ugly");
}

local void
bloopShow(void)
{
	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}

	if (bloopInterface) {
		if (bloopIsNice) {
			strcpy(msg, abPretty(bloopAbCurr));
			bloopSendString("msg", msg);
		}
		else {
			fprintf(fout, "(%s %c", "msg", SESCAPE);
			abWrSExpr(fout, bloopAbCurr, SXRW_Default);
			fprintf(fout, "%c)\n", SESCAPE);
			fflush(fout);
		}
	}
	else {
		int	(*p)(FILE *, AbSyn);
		p = bloopIsNice ? abPrettyPrint : abPrint;
		p(fout, bloopAbCurr);
		fprintf(fout, "\n");
	}
}

local void
bloopScope(void)
{
	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}
	if (!bloopStab)
		bloopSendMsg(ALDOR_M_BreakMsgNoStab);
	else {
		if (bloopInterface) {
			fprintf(fout, "(msg \"");
			bloopStabPretty();
			fprintf(fout, "\")\n");
			fflush(fout);
		}
		else
			stabPrint(fout, bloopStab);
	}
}


local void
bloopMsg(void)
{
	if (bloopInterface) return;
	comsgReportOne(fout, BLOOP_CURR->comsg);
}

local void
bloopSeman(void)
{
	char	str[MAX_STR];

	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}
	if (abComment(bloopAbCurr)) {
		sprintf(str, "Documentation: ++%s\n",
			docString(abComment(bloopAbCurr)));
		if (bloopInterface)
			strncat(msg, str, strlen(str));
		else
			bloopSendString("msg", str);
	}
	if (abSyme(bloopAbCurr)) {
		sprintf(str, "Symbol meaning: %s\n",
			symePretty(abSyme(bloopAbCurr)));
		if (bloopInterface)
			strncat(msg, str, strlen(str));
		else
			bloopSendString("msg", str);
	}
	if (abTForm(bloopAbCurr)) {
		sprintf(str, "Type form: %s\n",
			tfPretty(abTForm(bloopAbCurr)));
		if (bloopInterface)
			strncat(msg, str, strlen(str));
		else
			bloopSendString("msg", str);
	}
	if (abImplicit(bloopAbCurr)) {
		sprintf(str, "Implicit operator: %s\n",
			abPretty(abImplicit(bloopAbCurr)));
		if (bloopInterface)
			strncat(msg, str, strlen(str));
		else
			bloopSendString("msg", str);
	}
	if (abTContext(bloopAbCurr)) {
		sprintf(msg, "Embedding map: %ld\n",
			abTContext(bloopAbCurr));
		if (bloopInterface)
			strncat(msg, str, strlen(str));
		else
			bloopSendString("msg", str);
	}
	if (bloopInterface)
		bloopSendString("msg", msg);
}

local void
bloopUse(void)
{
	String s;

	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}

	switch (abUse(bloopAbCurr)) {
	case AB_Use_Declaration:s = "Declaration"; break;
	case AB_Use_Type:	s = "Type"; break;
	case AB_Use_Label:	s = "Label"; break;
	case AB_Use_Assign:	s = "Assign"; break;
	case AB_Use_Define:	s = "Define"; break;
	case AB_Use_Value:	s = "Value"; break;
	case AB_Use_RetValue:	s = "RetValue"; break;
	case AB_Use_NoValue:	s = "NoValue"; break;
	case AB_Use_Iterator:	s = "Iterator"; break;
	case AB_Use_LIMIT:	s = "No use info"; break;
	default:
	      	bloopSendMsg(ALDOR_M_BreakMsgBadNode);
		s = 0;
	      	break;
	}
	if (s) {
		sprintf(msg, comsgString(ALDOR_M_BreakMsgUsedContext), s);
		bloopSendString("msg", msg);
	}
}

local void
bloopMeans(void)
{
	SymeList	ml;
	int		i;

	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}

	if (abTag(bloopAbCurr) == AB_Id) {
		if (!bloopStab) {
			bloopSendMsg(ALDOR_M_BreakMsgNoStab);
			return;
		}

		ml = stabGetMeanings(bloopStab, NULL, bloopAbCurr->abId.sym);
		for (i = 1; ml; ml = cdr(ml), i++) {
			char	str[MAX_MSG];
			if (i == 1)
				sprintf(msg, "%d: %s\n", i,
					symePretty(car(ml)));
			else {
				sprintf(str, "%d: %s\n", i,
					symePretty(car(ml)));
				strncat(msg, str, strlen(str));
			}
		}
		bloopSendString("msg", msg);
	}
	else {
		int	(*p)(FILE *, TForm);
		p = bloopIsNice ? tfPrettyPrint : tfPrint;

		if (!bloopAbCurr) {
			bloopSendMsg(ALDOR_M_BreakMsgNoNode);
			return;
		}

		switch (abState(bloopAbCurr)) {
		case AB_State_AbSyn:
			bloopSendMsg(ALDOR_M_BreakMsgNoTypeInfo);
			break;
		case AB_State_Error: {
			char		str[MAX_MSG];
			if (bloopInterface) {
				sprintf(msg, comsgString(ALDOR_M_BreakMsgNTypes), NULL);
				sprintf(str, "\n");
				strcat(msg, str);
				bloopSendString("msg", msg);
			}
			else
				fprintf(fout, comsgString(ALDOR_M_BreakMsgNTypes), NULL);

			break;
		}
		case AB_State_HasPoss: {
			TPoss		tp;
			TPossIterator	ti;
			int		i, n;
			char		str[MAX_MSG];

			tp = abTPoss(bloopAbCurr);
			n  = tpossCount(tp);

			if (bloopInterface) {
				sprintf(msg, comsgString(ALDOR_M_BreakMsgNTypes), n);
				sprintf(str, "\n");
				strcat(msg, str);
			}
			else
				fprintf(fout, comsgString(ALDOR_M_BreakMsgNTypes), n);
			for (tpossITER(ti,tp),i=1; tpossMORE(ti); tpossSTEP(ti),i++) {
				if (bloopInterface) {
					sprintf(str, "%d: ", i);
					strcat(msg, str);
					if (bloopIsNice)
						sprintf(str, "%s\n",
							tfPretty(tpossELT(ti)));
					else
						sprintf(str, "%s\n",
							tfPretty(tpossELT(ti)));
					strcat(msg, str);
				}
				else {
					fprintf(fout, comsgString(ALDOR_M_BreakMsgNTypes), n);
					fprintf(fout, "%d: ", i);
					p(fout, tpossELT(ti));
					fprintf(fout, "\n");
				}
			}
			if (bloopInterface)
				bloopSendString("msg", msg);
			break;
		  }
		case AB_State_HasUnique:
			if (bloopInterface) {
				strcpy(msg, comsgString(ALDOR_M_BreakMsg1Type));
				if (bloopIsNice)
					strcat(msg, tfPretty(abTUnique(bloopAbCurr)));
				else
					strcat(msg, tfPretty(abTUnique(bloopAbCurr)));
				strcat(msg, "\n");
				bloopSendString("msg", msg);
			}
			else {
				fprintf(fout, "%s", comsgString(ALDOR_M_BreakMsg1Type));
				p(fout, abTUnique(bloopAbCurr));
				fprintf(fout, "\n");
			}
			break;
		default:
			bloopSendMsg(ALDOR_M_BreakMsgBadNode);
			break;
		}
	}
}

local void
bloopHome(void)
{
	int	i;

	if (!BLOOP_NODE || !bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}

	if (bloopTmpSet) {
		bloopAbCurr = bloopAbTmp;
		bloopAbTmp  = 0;
		bloopTmpSet = 0;
		bloopResetCurr();
	}

	for (i = 0; BLOOP_CURR->path[i] != PATH_END; i++)
		bloopCurrAbPath[i] = BLOOP_CURR->path[i];
	bloopCurrAbPath[i] = PATH_END;
	bloopCurrAbDepth   = BLOOP_CURR->depth;

	bloopSetCurr();
	if (!bloopInterface)
		bloopShow();
	else
		bloopSendString("ready", "\"\"");
}

local void
bloopUp(void)
{
	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}
	if (!BLOOP_NODE || bloopAbParent == 0) {
		bloopSendMsg(ALDOR_M_BreakMsgAtTop);
		return;
	}

	bloopCurrAbDepth--;
	bloopCurrAbPath[bloopCurrAbDepth] = PATH_END;

	bloopSetCurr();
	if (abTag(bloopAbCurr) == AB_Nothing) bloopUp();
	if (!bloopInterface)
		bloopShow();
	else
		bloopSendString("ready", "\"\"");
}

local void
bloopDown(void)
{
	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}
	if (!BLOOP_NODE || abIsLeaf(bloopAbCurr)) {
		bloopSendMsg(ALDOR_M_BreakMsgAtLeaf);
		return;
	}

	bloopCurrAbDepth++;
	bloopCurrAbPath[bloopCurrAbDepth-1] = 0;
	bloopCurrAbPath[bloopCurrAbDepth]   = PATH_END;

	bloopSetCurr();
	if (abTag(bloopAbCurr) == AB_Nothing) bloopHome();
	if (!bloopInterface)
		bloopShow();
	else
		bloopSendString("ready", "\"\"");
}

local void
bloopPrev(void)
{
	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}
	if (!BLOOP_NODE) {
		bloopSendMsg(ALDOR_M_BreakMsgNoPrev);
		return;
	}
	if (bloopAbParent == 0) {
		bloopSendMsg(ALDOR_M_BreakMsgAtTop);
		return;
	}

	if (bloopCurrAbPath[bloopCurrAbDepth-1] == 0) {
		bloopSendMsg(ALDOR_M_BreakMsgNoPrev);
		return;
	}

	bloopCurrAbPath[bloopCurrAbDepth-1]--;
	bloopSetCurr();
	if (abTag(bloopAbCurr) == AB_Nothing) bloopPrev(); /* should be a while loop*/
	if (!bloopInterface)
		bloopShow();
	else
		bloopSendString("ready", "\"\"");
}

local void
bloopNext(void)
{
	if (!bloopAbCurr) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNode);
		return;
	}
	if (!BLOOP_NODE) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNext);
		return;
	}
	if (bloopAbParent == 0) {
		bloopSendMsg(ALDOR_M_BreakMsgAtTop);
		return;
	}

	if (bloopCurrAbPath[bloopCurrAbDepth-1] == abArgc(bloopAbParent)-1) {
		bloopSendMsg(ALDOR_M_BreakMsgNoNext);
		return;
	}

	bloopCurrAbPath[bloopCurrAbDepth-1]++;
	bloopSetCurr();
	if (abTag(bloopAbCurr) == AB_Nothing) bloopNext();  /* should be a while loop*/
	if (!bloopInterface)
		bloopShow();
	else
		bloopSendString("ready", "\"\"");
}

local void
bloopStabPretty(void)
{
	char		str[MAX_STR];
	int		ts, minlev = 0;
	int		c, count = 0;
	StabLevel	slev;

	for (; bloopStab && count < MAX_MSG-10; bloopStab = cdr(bloopStab)) {
		slev = car(bloopStab);
		if (slev->lexicalLevel < minlev)
			break;
		fprintf(fout, "\n");
		count += 1;
		sprintf(str, "Symbol Table Level %ld , %s, (serial %ld), ",
			slev->lexicalLevel,
			slev->tformsUsed.table ? "LARGE" :
			"SMALL", slev->serialNo);
		fprintf(fout, "%s", str);
		count += strlen(str);
		fprintf(fout, "  %d symbols.\n", ts = tblSize(slev->tbl));
		if (ts) {
			Table		t = slev->tbl;
			struct TblSlot	*b;
			int		i;

			fprintf(fout, "\n");
			count += 1;
			for (i = 0; i < t->buckc; i++) {
				for (b = t->buckv[i]; b;
				     b = b->next) {
					StabEntry stent = (StabEntry) b->elt;
					SymeList symes = stent->symev[0];

					for (c = 0; symes && c < MAX_STR; symes = cdr(symes)) {
						sprintf(str, "%s", symePretty(car(symes)));
						c += strlen(str);
						fprintf(fout, "%s", str);
						if (cdr(symes))
							fprintf(fout, "\n");
					}
					fprintf(fout, "\n");
					count += c + 1;
				}
			}
		}
		fprintf(fout, "\n");
		count += 1;
				
		if (slev->tformsUnused) {
			TFormList tfl;

			fprintf(fout, "Type forms registered but not used: \n");
			for (c = 0, tfl = slev->tformsUnused; tfl && c < MAX_STR; tfl = cdr(tfl)) {
				sprintf(str, "%s\n",
					tfPretty(car(tfl)));
				c += strlen(str);
				fprintf(fout, "%s", str);
			}
			fprintf(fout, "\n");
			count += c + 1;
		}

		sprintf(str, "All type forms used: \n");
		fprintf(fout, "%s", str);
		count += strlen(str);

		if (!slev->tformsUsed.list) {
			sprintf(str, "NONE");
			fprintf(fout, "%s", str);
			count += strlen(str);
		}
		else {
			TFormUsesList tful = slev->tformsUsed.list;
			c = 0;
			while (tful && c < MAX_STR) {
				TFormUses tfu = car(tful);
				tful = cdr(tful);
				c += tfuPrint(fout, tfu);
			}
			count += c;
		}
		fprintf(fout, "\n");
		count += 1;

		if (slev->labelsInScope) {
			AbSynList asl = slev->labelsInScope;
			fprintf(fout, "Labels in this scope: ");
			c = 0;
			while (asl && c < MAX_STR) {
				sprintf(str, "%s", abPretty(car(asl)));
				c += strlen(str);
				fprintf(fout, "%s", str);
				asl = cdr(asl);
				asl ? fprintf(fout,", ") : fprintf(fout,"\n");
			}
			fprintf(fout, "\n");
			count += c + 1;
		}

		if (slev->boundSymes) {
			SymeList sl =  slev->boundSymes;
			sprintf(str, "Bound symbol meanings: \n");
			fprintf(fout, "%s", str);
			count += strlen(str);
			c = 0;
			while (sl && c < MAX_STR) {
				sprintf(str, "%s", symePretty(car(sl)));
				c += strlen(str);
				fprintf(fout, "%s", str);
				sl = cdr(sl);
				if (sl) fprintf(fout, "\n");
			}
			count += c;
		}
		fprintf(fout, "\n");
		count += 1;
	}
}
