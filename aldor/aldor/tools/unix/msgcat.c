/*
 * Usage: msgcat [-cat] [-h] [-c] [-detab] fn  
 *
 * This program is used to create a message data base in the X/Open format.
 * It takes the file "fn.msg" and produces files "fn.cat", "fn.h" and "fn.c"
 * when the flags -cat, -h or -c are given.
 * -detab flag causes all '\t' chars to be changed in ' ' chars
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ID_MAX		100	/* Maximum length of message or set id. */
#define SET_MAX		100	/* Maximum number of message sets. */
#define LINE_BUF	127

#define MSG_BREAK_LINE  250

/******************************************************************************
 *
 * X/Open message catalog file format
 * 
 *****************************************************************************/

/*
 *  msg0hdr
 *    msg0set	++ 1
 *      msg0msg -- 1.1
 *      msg0msg -- 1.2
 *    msg0set   ++ 2
 *      msg0msg -- 2.1
 *      msg0msg -- 2.2
 *      msg0msg -- 2.3
 *   ...
 *  Message bodies
 */

#define MSG0_MAGIC 505

struct msg0hdr {
	unsigned long 		magic;
	unsigned short 		setcount;
	unsigned short		setmax;
	char 			filler[20];
};

struct msg0set {
	unsigned short		setno;
	unsigned short		msgcount;
};

struct msg0msg {
	unsigned short 		msgno;
	unsigned short		length;
	unsigned long		position;
};

/******************************************************************************
 *
 * Utility functions
 *
 *****************************************************************************/

char *
mustAlloc(n)
	int	n;
{
	char *t = (char *) malloc(n);
	if (!t) {fprintf(stderr, "Out of memory\n"); exit(EXIT_FAILURE); }
	return t;
}

char *
mustRealloc(s, n)
	char	*s;
	int	n;
{
	char *t = (char *) realloc(s, n);
	if (!t) {fprintf(stderr, "Out of memory\n"); exit(EXIT_FAILURE); }
	return t;
}

char *
strAlloc(s)
	char	*s;
{
	int  n  = strlen(s);
	char *m = mustAlloc(n+1);
	strcpy(m, s);
	return m;
}

char *
fileName(name, type)
	char	*name;
	char 	*type;
{
	int	n;
	char	*fn;

	n  = strlen(name) + strlen(type) + 1;
	fn = (char *) mustAlloc(n + 1);
	sprintf(fn, "%s.%s", name, type);
	return fn;
}

FILE *
mustOpen(fname, mode)
	char	*fname, *mode;
{
	FILE	*f;
	f = fopen(fname, mode);
	if (!f) { fprintf(stderr, "Can't open %s.\n", fname); exit(EXIT_FAILURE); }
	return f;
}

/******************************************************************************
 *
 * Scanner for message file.
 *
 *****************************************************************************/

char	setName[ID_MAX];	/* Name of the current set; "" => none. */
int	setNo;
char	msgName[ID_MAX];	/* Name of the current msg; "" => none. */
int     msgNo;
int	quoteChar;	   	/* -1 => none. */
int	lineNo;
char	*file;
char	*copyright = 0;
int doCat = 0, doH = 0, doC = 0, doDetab = 0;

/*
 * Reset the state.
 */
void
scanReset(infile)
	char	*infile;
{
	strcpy(setName, ""); setNo = 0;
	strcpy(msgName, ""); msgNo = 0;
	quoteChar = -1;
	lineNo = 1;
	file   = infile;
}

void
scanError(s)
	char	*s;
{
	fprintf(stderr,"\"%s\" line %d: Syntax error -- %s\n",file,lineNo,s);
	exit(EXIT_FAILURE);
}

/*
 * Collect an identifying name or number, skipping surrounding white space.
 * Name and *pnumber are updated.
 */
void
fscanId(infile, name, pnumber)
	FILE	*infile;
	char	*name;
	int	*pnumber;
{
	int	c;
	
	while ((c = fgetc(infile)) == ' ' || c == '\t' || c == '\n')
		if (c == '\n') lineNo++;
	if (isdigit(c)) {
		int n = c - '0';
		while (isdigit(c = fgetc(infile))) n = 10*n + c - '0';
		ungetc(c, infile);
		*pnumber = n;
	}
	else if (isalpha(c) || c == '_') {
		*name++ = c;
		while (isalnum(c = fgetc(infile)) || c == '_') *name++ = c;
		ungetc(c, infile);
		*pnumber = *pnumber + 1;
	}
	else
		scanError("Expecting name or numeric identifier");

	*name = 0;
	while ((c = fgetc(infile)) == ' ' || c == '\t') ;
	ungetc(c, infile);
}

/*
 * Prepare to handle the next message: 
 * -- Handle $set, $quote or other (commentary) $ lines (including $copyright)
 * -- Process message name/no and position file at beginning of text.
 * Returns 0 on success and -1 on EOF.
 */
int
prepMsg(infile)
	FILE	*infile;
{
	int	c, n;
	long	fpos;
	char	*s, line[LINE_BUF];

	for (c = fgetc(infile); c == '$'; c = fgetc(infile)) {
		fpos = ftell(infile);
		s = fgets(line, LINE_BUF, infile);
		if (s == 0) return -1;

		if (!strncmp(line, "set",   3)) {
			char *readLine;
			/* $set {<name>|<no>} [comment] */
			fseek(infile, fpos+3, 0);	/* backup */
			fscanId(infile, setName, &setNo);
			readLine = fgets(line, LINE_BUF, infile);	/* reswallow line */
			if (readLine == NULL)
				return -1;
			msgNo = 0;
		}
		else if (!strncmp(line, "quote", 5)) {
			/* $quote [char [comment]] */
			char *s = line + 5;
			while (*s == ' ' || *s == '\t') s++;
			quoteChar = (*s == 0 || *s == '\n') ? -1 : *s;
		}
		else if (!strncmp(line, "copyright", 9)) {
			/* $copyright [text] */
			char *s = line + 9;
			while (*s == ' ' || *s == '\t') s++;
			copyright = mustAlloc((int) strlen(s) + 1);
			strcpy(copyright, s);
		}

		n = strlen(line);
		while (line[n-1] != '\n') {
			s = fgets(line, LINE_BUF, infile);	
			if (s == 0) return -1;
			n = strlen(line);
		}
		lineNo++;	/* for \n swallowed by fgets. */
	}
	if (c == EOF) return -1;

	ungetc(c, infile);	/* Put back first character of line. */
	fscanId(infile, msgName, &msgNo);
	return 0;
}

/*
 * Read message from infile.
 */
char *scanbuf = 0;
int  scanlen  = 0;
int  scanix   = 0;	/* Next place to put a char. */

void
scanInit()
{
	if (!scanbuf) {
		scanlen = 100;
		scanbuf = (char *) mustAlloc(scanlen);
	}
	scanix = 0;
}

void
scanIn(c)
	int	c;
{
	if (scanix == scanlen) {
		scanlen += 100;
		scanbuf = (char *) mustRealloc(scanbuf, scanlen);
	}
	scanbuf[scanix++] = c;
}

char *
scanFini()
{
	scanIn(0);
	return scanbuf;
}

char *
scanMsg(infile)
	FILE	*infile;
{
	int	c;

	scanInit();
	c = fgetc(infile);
	if (c == quoteChar) c = fgetc(infile);

	while (c != EOF && c != '\n' && c != quoteChar) {
		if (c == '\\') {
			c = fgetc(infile);
			/* An escaped newline isn't there. */
			if (c == '\n') {
				lineNo++;
				c = fgetc(infile);
				continue;
			}
			switch (c) {
			case 'n':  c = '\n'; break;
			case 't':  c = '\t'; break;
			case 'v':  c = '\v'; break;
			case 'b':  c = '\b'; break;
			case 'r':  c = '\r'; break;
			case 'f':  c = '\f'; break;
			case EOF:  scanError("End of file in escape sequence.");
			default:   if (c != quoteChar && c != '\\')
					scanError("Bad escape sequence.");
				   break;
			}
		}
		if (!isprint(c) && !isspace(c))
			scanError("Unpritable character.");

		scanIn(c);
		c = fgetc(infile);
	}

	if (c == quoteChar) while (c != EOF && c != '\n') c = fgetc(infile);
	if (c == '\n') lineNo++;

	return scanFini();
}

void
unscanMsg(outfile, msg)
	FILE	*outfile;
	char	*msg;
{
	fputc('"', outfile);
	
	for (; *msg; msg++) {
		switch (*msg) {
		case '\n': fprintf(outfile, "\\n\\\n"); break;
		case '\t': 
			if (doDetab)
				fputc(' ', outfile);
			else
				fprintf(outfile, "\\t");
			break;
		case '\v': fprintf(outfile, "\\v");     break;
		case '\b': fprintf(outfile, "\\b");     break;
		case '\r': fprintf(outfile, "\\r");     break;
		case '\f': fprintf(outfile, "\\f");     break;
		case '\\': fprintf(outfile, "\\\\");    break;
		case '\"': fprintf(outfile, "\\\"");    break;
		default:   fputc(*msg, outfile);        break;
		}
	}
	fputc('"', outfile);
}

void
unscanGrind(outfile, msg)
	FILE	*outfile;
	char	*msg;
{
	int	i;
	fprintf(outfile, "{");

	for (i = 0; msg[i]; i++) {
		if (i % 15 == 0) fprintf(outfile, "\n  ");
		switch (msg[i]) {
		case '\n': fprintf(outfile, "'\\n',"); break;
		case '\t': 
			if (doDetab)
				fprintf(outfile, "' ', ");
			else
				fprintf(outfile, "'\\t',");
			break;
		case '\v': fprintf(outfile, "'\\v',");     break;
		case '\b': fprintf(outfile, "'\\b',");     break;
		case '\r': fprintf(outfile, "'\\r',");     break;
		case '\f': fprintf(outfile, "'\\f',");     break;
		case '\\': fprintf(outfile, "'\\\\',");    break;
		case '\"': fprintf(outfile, "'\\\"',");    break;
		case '\'': fprintf(outfile, "'\\\'',");    break;
		default:   fprintf(outfile, "'%c', ", msg[i]); break;
		}
	}
	fprintf(outfile, "0\n}");
}

int
canBeCLiteral(msg)
	char	*msg;
{
	return strlen(msg) <= MSG_BREAK_LINE;
}


/*****************************************************************************
 *
 * First pass: Read data. Count sets and messages.
 *
 *****************************************************************************/

struct msg0set	set[SET_MAX];
struct msg0hdr	hdr      = {MSG0_MAGIC, 0, 0};
int		msgcount = 0;

struct msg1msg {
	unsigned short		setno;
	char			*setname;
	unsigned short 		msgno;
	char			*msgname;
	char			*msgtext;
	struct msg1msg		*next;
};

struct msg1msg	msg1all = {0, 0, 0, 0, 0, 0};
	

void
passToRead(basename)
	char		*basename;
{
	char		*msgname, *msgText;
	FILE		*msgfile;
	int		rc, nmsgs, prevset;
	struct msg1msg	*m1 = &msg1all;

	msgname = fileName(basename, "msg");
	msgfile = mustOpen(msgname, "rb");
	scanReset(msgname);

	for (nmsgs = 0, prevset = 0; ; prevset = setNo) {
		rc = prepMsg(msgfile);
		if ((rc == -1  || setNo != prevset) && prevset != 0) {
			set[hdr.setcount].setno    = prevset;
			set[hdr.setcount].msgcount = nmsgs;
			hdr.setcount++;
			if ((int) hdr.setmax < setNo) hdr.setmax = setNo;
			nmsgs = 0;
		}
		if (rc == -1) break;
		msgText = scanMsg(msgfile);
		msgcount++;
		nmsgs++;

		m1->next     = (struct msg1msg *) mustAlloc(sizeof(*m1));
		m1           = m1->next;
		m1->setno    = setNo;
		m1->setname  = strAlloc(setName);
		m1->msgno    = msgNo;
		m1->msgname  = strAlloc(msgName);
		m1->msgtext  = strAlloc(msgText);
		m1->next     = 0;
	}
	free(msgname);
	fclose(msgfile);
}

/*****************************************************************************
 *
 * Last pass: Free data.
 *
 *****************************************************************************/

void
passToFree()
{
	struct msg1msg *m0, *m1;

	m0 = msg1all.next;
	msg1all.next = 0;

	while (m0) {
		m1 = m0->next;
		free(m0->setname);
		free(m0->msgname);
		free(m0->msgtext);
		free((char *) m0);
		m0 = m1;
	}
}

/*****************************************************************************
 *
 * Middle passes: Generate files.
 *
 *****************************************************************************/

void
stars(outf)
	FILE	*outf;
{
	int	i;
	for (i = 0; i < 76; i++) fputc('*', outf);
}

void
passToDoH(basename)
	char		*basename;
{
	FILE		*hfile = 0;
	char		*hname = 0;
	int		prevset;
	struct msg1msg	*m;

	hname   = fileName(basename, "h");
	hfile   = mustOpen(hname,    "w");

	fprintf(hfile, "/*"); stars(hfile); fprintf(hfile, "\n");
	fprintf(hfile, " *\n");
	fprintf(hfile, " * %s: Generated from %s.msg\n", hname, basename);
	if (copyright) {
	 fprintf(hfile," *\n");
	 fprintf(hfile," * %s", copyright);
	}
	fprintf(hfile, " *\n");
	fprintf(hfile, " *"); stars(hfile); fprintf(hfile, "/\n");
	fprintf(hfile, "\n");
	fprintf(hfile, "#ifndef _%s_MSG_H_\n", basename);
	fprintf(hfile, "#define _%s_MSG_H_\n", basename);
	fprintf(hfile, "\n");
	fprintf(hfile, "#include \"msg.h\"\n"); fprintf(hfile, "\n");
	fprintf(hfile, "/* Last member has setno == -1. */\n");
	fprintf(hfile, "\n");
	fprintf(hfile, "extern struct msgInfo %s_msgs[];\n",
			basename);
	fprintf(hfile, "\n");

	for (prevset = 0, m = msg1all.next; m; prevset = m->setno, m = m->next)
	{
		if (m->setno != prevset) {
			fprintf(hfile, "\n");
			fprintf(hfile,
				"/* Messages for set %s = %d */\n",
				m->setname,m->setno);
			if (*m->setname)
				fprintf(hfile,"# define %s %d\n",
					m->setname,m->setno);
			fprintf(hfile, "\n");
		}
		if (*m->msgname)
			fprintf(hfile, "# define %s %d\n",
				m->msgname, m->msgno);
	}

	fprintf(hfile, "\n");
	fprintf(hfile, "#endif\n");
	free(hname);   
	fclose(hfile); 
}

void
passToDoC(basename)
	char		*basename;
{
	FILE		*cfile = 0;
	char		*cname = 0;
	int		prevset;
	struct msg1msg	*m;

	cname   = fileName(basename, "c");
	cfile   = mustOpen(cname,    "w");

	fprintf(cfile, "/*"); stars(cfile); fprintf(cfile, "\n");
	fprintf(cfile, " *\n");
	fprintf(cfile, " * %s: Generated from %s.msg.\n", cname, basename);
	if (copyright) {
	 fprintf(cfile," *\n");
	 fprintf(cfile," * %s", copyright);
	}
	fprintf(cfile, " *\n");
	fprintf(cfile, " *"); stars(cfile); fprintf(cfile, "/\n");
	fprintf(cfile, "\n");
	fprintf(cfile, "#include \"%s.h\"\n", basename);
	fprintf(cfile, "\n");
	fprintf(cfile, "/* Messages to long for string literals: */\n");
	fprintf(cfile, "\n");
	for (m = msg1all.next; m; m = m->next) {
		if (!canBeCLiteral(m->msgtext)) {
			fprintf(cfile, "static char msg_%s_%d_%d[] = ",
				basename, m->setno, m->msgno);
			unscanGrind(cfile, m->msgtext);
			fprintf(cfile, ";\n\n");
		}
	}

	fprintf(cfile, "/* Last member has setno == -1. */\n");
	fprintf(cfile, "\n");
	fprintf(cfile, "struct msgInfo %s_msgs[] = {\n", basename);
	fprintf(cfile, "#ifndef NO_MSG_TEXT\n");

	for (prevset = 0, m = msg1all.next; m; prevset = m->setno, m = m->next)
	{
		fprintf  (cfile, "  {%2d, %3d, ", m->setno, m->msgno);
		unscanMsg(cfile, m->msgname);
		fprintf  (cfile, ",\n\t\t");

		if (canBeCLiteral(m->msgtext))
			unscanMsg(cfile, m->msgtext);
		else
			fprintf(cfile, "msg_%s_%d_%d",
				basename, m->setno, m->msgno);
		fprintf  (cfile, "},\n");
	}

	fprintf(cfile, "#endif /* !NO_MSG_TEXT */\n");
	fprintf(cfile, "  {-1, 0, 0, 0}\n");
	fprintf(cfile, "};\n");

	free(cname);
	fclose(cfile);
}

void
passToDoCat(basename)
	char		*basename;
{
	char		*catname = 0;
	FILE		*catfile = 0;
	int		iset, prevset;
	long		textpos;	/* Place to put text. */
	struct msg1msg	*m;

	catname = fileName(basename, "cat");
	catfile = mustOpen(catname,  "wb");
	fwrite(&hdr, sizeof(hdr), 1, catfile);

	textpos = sizeof(struct msg0hdr)
		+ hdr.setcount * sizeof(struct msg0set)
		+ msgcount     * sizeof(struct msg0msg);

	for (iset = 0, prevset = 0, m = msg1all.next;
	     m;
	     prevset = m->setno, m = m->next)
	{
		struct msg0msg	msg;
		long		hdrpos;

		if (m->setno != prevset) {
			fwrite(set+iset, sizeof(set[iset]), 1, catfile);
			iset++;
		}
		msg.msgno    = m->msgno;
		msg.length   = strlen(m->msgtext);
		msg.position = textpos;
		fwrite(&msg, sizeof(msg), 1, catfile);

		hdrpos = ftell(catfile);
		fseek(catfile, textpos, 0);
		fwrite(m->msgtext, sizeof(char), msg.length + 1, catfile);
		textpos = ftell(catfile);
		fseek(catfile, hdrpos, 0);
	}
	free(catname);   
	fclose(catfile); 
}
			
/*****************************************************************************
 *
 * Main program
 *
 *****************************************************************************/

void
usageError()
{
	fprintf(stderr,
		"Usage: msgcat [-cat] [-h] [-c] [-detab] fn \n");
	fprintf(stderr,
		"takes 'fn.msg' and makes 'fn.cat' 'fn.h' and 'fn.c'\n");
	fprintf(stderr,
	        "depending on which options -cat, -h or -c are given.\n");
	fprintf(stderr,
	        "Option -detab causes conversion of tabs to spaces.\n");
	exit(EXIT_FAILURE);
}

int
main(argc, argv)
	int	argc;
	char	**argv;
{
	while (argc > 1 && argv[1][0] == '-') {
		if      (!strcmp("cat",   argv[1]+1)) doCat   = 1;
		else if (!strcmp("h",     argv[1]+1)) doH     = 1;
		else if (!strcmp("c",     argv[1]+1)) doC     = 1;
		else if (!strcmp("detab", argv[1]+1)) doDetab = 1;
		else usageError();

		argc--, argv++;
	}
	if (argc != 2) usageError();

	passToRead(argv[1]);
	if (doCat) passToDoCat(argv[1]);
	if (doC)   passToDoC  (argv[1]);
	if (doH)   passToDoH  (argv[1]);
	passToFree();

    return EXIT_SUCCESS;
}
