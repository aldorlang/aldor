/*
 * 2-level grammar implementation.
 * 
 * zacc [-y yaccfile] [-c cfile] [-v] [-p] infile
 *
 * This command accepts a grammar file and produces a yacc file as output.
 * The grammar file looks like yacc, with the following differences:
 *
 * 1. Rules may be parameterized
 *    E.g.    List(S):    Nothing | List(S) S ;
 *
 * 2. Rules may be typed
 *    E.g.    Node<pt>:   '(' Node ')' | Node '+' Node ;
 *            List(S)<l>: Nothing | List(S) S ;
 *
 * 3. A default type for rules or tokens can be given:
 *    E.g.    %default-token-type <tok>
 *            %default-rule-type  <pt>
 *    
 *    A name which is used but does not have a rule is assumed to be a token.
 *
 *    If a name is only used preceded by a back quote, then no type declaration
 *    is made for it.
 *    E.g.    List(S,f): Nothing { $$ = 0; } | List(S,f) S { $$ = f($2,$1); } ;
 *            conslist:  List(TK_Int, `cons);
 *
 *    A rule typed as <void> is not given a type even in the presence of 
 *    a "%default-rule-type".
 *
 * 4. A C source file may be included to obtain values for tokens:
 *    E.g.    %include-enum "token.h" tokenTag
 *            produces %token statements for the entries in ``enum tokenTag''.
 *
 * The "-y" option gives a name to the generated yacc file and says to keep it.
 * The "-c" option gives a name to the generated C file and says to keep it.
 * The "-v" option causes the program to print debug messages.
 * The "-p" option causes the token symbols to be prefaced with "YY_".
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <unistd.h>
# include "zacc.h"
# include "zaccgram.h"
# include "cenum.h"

#define COMMENTARY

/*
 * Things imported.
 */

extern FILE *yyin;
extern int  yydebug;

extern char *ctime();
/*extern char *malloc();*/
/*extern char *strcpy();*/

/*
 * Forward declarations for utilities.
 */

char *	mustAlloc of((int));
char *  strAlloc  of((char *));
FILE *  mustOpen  of((char *fname, char *mode));
char *  timeNow   of((void));
void	fatalErr  of((void));
char *	fnameDir  of((char *));


/******************************************************************************
 *
 * :: Top-level global variables.
 *
 *****************************************************************************/

#define	DEFAULT_YACC_FILE	"zacc.out"
#define DEFAULT_C_FILE		"y.tab.c"

static int  verbose=0;  	/* -v flag given. */
static char *prefix="";		/* -p flag prefix. */

static char *yfile="zacc.out";	/* -y file name. */
static int  hadyfile=0;

static char *cfile="y.tab.c";	/* -c file name. */
static int  hadcfile=0;

static char *fnin;		/* Name of input file. */
static char *dirin;		/* Directory part of input filename. */
static FILE *fout;		/* Output stream for yacc generation. */

static int  passno;		/* Pass number. */
static int  errcount = 0;	/* Number of syntax errors. */

static char *dfltTokenType = 0;	/* Default type for tokens. */
static char *dfltRuleType  = 0; /* Default type for rules. */
static int  hasPctType     = 0; /* %type used? */

static int  isExtra();		/* Is this a XXX_START or XXX_LIMIT word? */
static int  isTerminal();	/* Is the given word a terminal (token)? */
static void prWord();		/* Print the given word. */

struct phrUse;
struct phrDef;
struct phrInfo;

int yylex();
void handleRule();
void _2_endRule();
void _2_startRule();
void _2_endPhraseName();
void _2_seeArg();
void _2_seeType();
void _2_seeName();
void _2_endCommand();
void _2_startCommand();
void _2_seeIncludeEnum();
void _2_wholeEpilog();
void _2_endProlog();
void _2_startProlog();
void _2_endRule();
void _2_startPhraseName();
void _2_midRule();
void _1_endRule();
void _1_midRule();
void _1_startRule();
void _1_seeType();
void _1_endPhraseName();
void _1_seeArg();
void _1_seeName();
void _1_startPhraseName();
void _1_seeIncludeEnum();
void _1_endCommand();
void _1_startCommand();
void _1_wholeEpilog();
void _1_endProlog();
void _1_startProlog();
struct phrUse *findUse(struct phrInfo	*pinfo, struct phrDef	*context, struct phrUse	*);
void _1_();
void _1_();


static int	inLhs, inAction;	/* context information */


/******************************************************************************
 *
 * :: Top-level entry point and parser tie-ins.
 *
 *****************************************************************************/


void
zaccPass(char	*fn, int n)
{
	yyin   = mustOpen(fn, "r");
	fnin   = fn;
	passno = n;
	yyparse();
	fclose(yyin);
}


void
zacc(char	*fn, char	*yfn)
{
	dirin = fnameDir(fn);
	
	fout = mustOpen(yfn, "w");

#ifdef COMMENTARY
	fprintf(fout, "/*\n * \"zacc\" output from \"%s\" on %s\n */\n\n", 
		      fn, timeNow());
#endif
	zaccPass(fn, 1);
	if (errcount > 0) exit(3);
	zaccPass(fn, 2);

	fclose(fout);
}

int
main(int argc, char **argv)
{
	int	options = 1;

	while (options) {
		if (argc > 1 && !strcmp(argv[1], "-v")) {
			verbose = 1;
			argc--, argv++;
			continue;
		}
		if (argc > 1 && !strcmp(argv[1], "-p")) {
			prefix = "_YY_";
			argc--, argv++;
			continue;
		}
		if (argc > 2 && !strcmp(argv[1], "-y")) {
			yfile = argv[2];
			hadyfile = 1;
			argc -= 2, argv += 2;
			continue;
		}
		if (argc > 2 && !strcmp(argv[1], "-c")) {
			cfile = argv[2];
			hadcfile = 1;
			argc -= 2, argv += 2;
			continue;
		}
		options = 0;
	}

	if (argc != 2) fatalErr();

	if (hadyfile || hadcfile) {
		zacc(argv[1], yfile);
	}
	if (hadcfile) {
		char cmd[500];
		int  rc;

		sprintf(cmd, "yacc %s", yfile);
		if (verbose) printf("Exec: %s\n", cmd);
		rc = system(cmd);
		if (rc != 0) {
			fprintf(stderr, "Yacc failed on file \"%s\"\n", yfile);
			return 3;
		}
		if (!hadyfile) unlink(yfile);

		rc = rename("y.tab.c", cfile);
		if (rc != 0) {
			fprintf(stderr,
				"Rename failed from \"y.tab.c\" to \"%s\"\n",
				cfile);
			return 3;
		}

	}
	
        return 0;
}

int lineNo;

int
yyerror(char	*s)
{
	fprintf(stderr, "\"%s\", line %d: %s\n", fnin, lineNo, s);
	return errcount++;
}

void
incLineCount()
{
  	lineNo++;
}

int tokEcho;		/* In "token", echo the string. */
int tokEnlist;		/* In "token", add a link to the token list. */ 

struct tokList {
	int		tag;
	char		*str;
	struct tokList	*next;
	int		show;	/* for rule emission */
} *tokListHead, *tokListTail;

void
token(int	tag, char	*str)
{
	/* 
	 * To finesse parser lookahead. 
	 */
	static char *prevTok = "";
	static int   prevEcho= 1;

	yylval.str = strAlloc(str);

	if (prevEcho && tokEcho) 
		prWord(fout, prevTok);

	prevEcho = tokEcho;
	prevTok  = yylval.str;

	if (tag == TK_PctId && !strcmp(str, "%type"))
		hasPctType = 1;
	
	if (tag == TK_NewLine) incLineCount();

	if (tokEnlist) {
		struct tokList	*t;
		t = (struct tokList *) mustAlloc(sizeof(struct tokList));
		t->tag = inAction ? TK_CTok : tag;
		t->str = yylval.str;
		t->show= 0;
		t->next= 0;

		if (!tokListHead) 
			tokListHead = tokListTail = t;
		else {
			tokListTail->next = t;
			tokListTail = t;
		}
	}
}


/******************************************************************************
 *
 * :: Grammar actions
 *
 *****************************************************************************/

void
startProlog()
{
	inLhs = 0;
	inAction = 0;

	switch (passno) {
	  case 1: _1_startProlog(); break;
	  case 2: _2_startProlog(); break;
	}
}

void
endProlog()
{
	switch (passno) {
	  case 1: _1_endProlog(); break;
	  case 2: _2_endProlog(); break;
	}
}


void
wholeEpilog()
{
	switch (passno) {
	  case 1: _1_wholeEpilog(); break;
	  case 2: _2_wholeEpilog(); break;
	}
}


void
startCommand()
{
	switch (passno) {
	  case 1: _1_startCommand(); break;
    	  case 2: _2_startCommand(); break;
	}
}

void
endCommand()
{
	switch (passno) {
	  case 1: _1_endCommand(); break;
	  case 2: _2_endCommand(); break;
	}
}


void
seeTokenType(char	*str)
{
	dfltTokenType = str;
}

void
seeIncludeEnum(char	*fname, char	*ename)
{
	char	fbuf[500];
	int	i;

	/* Convert "\"foo.h\"" -> "dirin/foo.h" */
	if (*fname == '"') fname++;
	for (i = 0; dirin[i]; i++) fbuf[i] = dirin[i];
	fbuf[i++] = '/';
	while (*fname && *fname != '"') fbuf[i++] = *fname++;
	fbuf[i] = 0;

	switch (passno) {
	 case 1: _1_seeIncludeEnum(fbuf, ename); break;
	 case 2: _2_seeIncludeEnum(fbuf, ename); break;
	}
}

void
seeRuleType(char	*str)
{
	dfltRuleType = str;
}

void
startPhraseName()
{
	switch (passno) {
	  case 1: _1_startPhraseName(); break;
	  case 2: _2_startPhraseName(); break;
	}
}

void
seeName(char *str)
{
	switch(passno) {
	  case 1: _1_seeName(str); break;
	  case 2: _2_seeName(str); break;
	}
}

void
seeArg(char *str)
{
	switch(passno) {
	  case 1: _1_seeArg(str); break;
	  case 2: _2_seeArg(str); break;
	}
}

void
endPhraseName()
{
	switch(passno) {
	  case 1: _1_endPhraseName(); break;
	  case 2: _2_endPhraseName(); break;
	}
}

void
seeType(char *str)
{
	switch(passno) {
	  case 1: _1_seeType(str); break;
	  case 2: _2_seeType(str); break;
	}
}


void
startRule()
{
	inLhs = 1;

	switch(passno) {
	  case 1: _1_startRule(); break;
	  case 2: _2_startRule(); break;
	}
}

void
midRule()
{
	inLhs = 0;

	switch(passno) {
	  case 1: _1_midRule(); break;
	  case 2: _2_midRule(); break;
	}
}

void
endRule()
{
	switch(passno) {
	  case 1: _1_endRule(); break;
	  case 2: _2_endRule(); break;
	}
}

void
startAction()
{
	inAction = 1;
}

void
endAction()
{
	inAction = 0;
}


/******************************************************************************
 *
 * :: Structures for Pass 1.
 *
 *****************************************************************************/


/*
 * These structures maintain information on rules defined and used.
 *
 * E.g.  z(a,b)<tok> :	OPren x(b) y(q,r) a CPren ;
 */

static char *	argName[] = { "&1","&2","&3","&4","&5","&6","&7","&8","&9" };
# define	MAXARGC	    (9)

struct phrInfo {
	char	*name;
	int	argc;
	char	*argv[MAXARGC];
};

struct phrDef {
	char		*name;		/* z   */
	int		argc;		/* 2   */
	char		*type;		/* tok */
	struct phrDef	*next;
};
	
struct phrUse {
	struct phrInfo	info;		/* x(&2)       ;   y(q)      */
	struct phrDef	*context;	/* ^{z,2,..}   ;   0         */
	struct phrUse	*next;
};

struct phrDef	*phrDefList = 0;
struct phrUse	*phrUseList = 0;      

struct phrUse	*tokenSet, *genSet;

#define UNQ(s)  ( ((s)[0] == '`') ? (s) + 1 : (s) )
void
fprintInfo(FILE		*fout, struct phrInfo	*pinfo)
{
	int	i;

	prWord(fout, UNQ(pinfo->name));

	if (pinfo->argc == 0) return;
	
	for (i = 0; i < pinfo->argc; i++) {
		fprintf(fout, "_");
		prWord(fout, UNQ(pinfo->argv[i]));
	}
	fprintf(fout, "_");
}

/* Does the arg use &1, etc? */
int
usesBV(struct phrInfo	*pinfo)
{
	int	i, j;
	for (i = 0; i < pinfo->argc; i++)
		for (j = 0; j < MAXARGC; j++)
			if (pinfo->argv[i] == argName[j]) return 1;
	return 0;
}

/* Rename bound variables to &1, etc. */
char *
renameBV(char		*str, struct phrInfo	*pinfo)
{
	int	i;

	for (i = 0; i < pinfo->argc; i++)
		if (!strcmp(str, pinfo->argv[i]))
			return argName[i];

	return str;
}

int
unnameBV(char	*str)
{
	int	i;
	for (i = 0; i < MAXARGC; i++)
		if (str == argName[i]) return i;
	return -1;
}

struct phrUse	*
instance(struct phrUse	*withFV, struct phrUse	*withBV)
{
	struct phrUse	*inst;
	int		i;

	inst = (struct phrUse *) mustAlloc(sizeof(struct phrUse));
	inst->context = 0;
	inst->next    = 0;

	inst->info    = withFV->info;

	for (i = 0; i < withFV->info.argc; i++) {
		int	a = unnameBV(withFV->info.argv[i]);

		if (a >= 0) inst->info.argv[i] = withBV->info.argv[a];
	}

	return inst;
}

struct phrDef *
findDef(char *str, struct phrDef 	*l)
{
	for ( ; l; l = l->next) {
		if (!strcmp(str, l->name)) return l;
	}
	return 0;
}

struct phrUse *
findUse(struct phrInfo	*pinfo, struct phrDef	*context, struct phrUse	*l)
{
	for ( ; l; l = l->next) {
		int	different = strcmp(pinfo->name, l->info.name);
		int	i;
		for (i = 0; !different && i < pinfo->argc; i++) 
			different = strcmp(pinfo->argv[i], l->info.argv[i]);
		if (!different)
			different = l->context && (l->context != context);
		if (!different)
			return l;
	}
	return 0;
}


struct phrDef *
addDef(char	*name, int argc, char *type)
{
	struct phrDef	*def;

	def = findDef(name, phrDefList);

	if (!def) {
		def = (struct phrDef *) mustAlloc(sizeof(struct phrDef));
		def->name = name;
		def->argc = argc;
		def->type = type;
		def->next = phrDefList;
		phrDefList = def;
	}
	else {
		if (def->argc != argc) 
			fatalErr();
		if (def->type && type && strcmp(def->type, type)) 
			fatalErr();
		if (type && !(def->type)) 
			def->type = type;
	}
	return def;
}

struct phrDef *
defListNReverse(struct phrDef	*l)
{
	struct phrDef	*r, *t;

	r = 0;
	while(l) {
		t = l->next;
		l->next = r;
		r = l;
		l = t;
	}
	return r;
}

int
dontAddUse(char	*str)
{
	int	i;
	
	if (*str == '`') return 1;
	if (!strcmp(str, "error")) return 1;

	for (i = 0; i < MAXARGC; i++)
		if (str == argName[i]) return 1;
	return 0;
}

struct phrUse *
addUse(struct phrInfo	*pinfo, struct phrDef	*context)
{
	struct phrUse	*use;

	if (dontAddUse(pinfo->name)) 
		return phrUseList;
	
	use = findUse(pinfo, context, phrUseList);

	if (!use) {
		use = (struct phrUse *) mustAlloc(sizeof(struct phrUse));
		use->info    = *pinfo;
		use->context = usesBV(pinfo) ? context : 0;
		use->next    = phrUseList;
		phrUseList   = use;
	}
	return phrUseList;
}


void
showDefs(struct phrDef	*l)
{
	for ( ; l; l = l->next) {
		fprintf(stdout, "\t%s", l->name);
		if (l->argc > 0) 
			fprintf(stdout, "[%d]", l->argc);
		if (l->type)
			fprintf(stdout,"<%s>", l->type);
		fprintf(stdout,"\n");
	}
}

void
showUses(struct phrUse	*l)
{
	for ( ; l; l = l->next) {
		int	i;
		fprintf(stdout, "\t%s", l->info.name);
		if (l->info.argc > 0) {
			fprintf(stdout, "(");
			for (i = 0; i < l->info.argc; i++)
				fprintf(stdout, " %s", l->info.argv[i]);
			fprintf(stdout, ")");
			
			if (l->context) 
				fprintf(stdout, " in %s", l->context->name);
		}
		fprintf(stdout, "\n");
	}
}

void
phraseClosure()
{
	struct phrUse	*withFV = 0, *undone = 0;
	int		n;

	phrDefList = defListNReverse(phrDefList);

	tokenSet = 0;
	genSet   = 0;

	while (phrUseList) {
		struct phrUse	*t;

		t = phrUseList; phrUseList = phrUseList->next;

		if (t->context) {
			t->next = withFV; withFV = t;
		}
		else if (!findDef(t->info.name, phrDefList)) {
			if (t->info.argc != 0) fatalErr();
			t->next = tokenSet; tokenSet = t;
		}
		else if(t->info.argc > 0) {
			t->next = undone; undone = t;  
		}
	}

	if (verbose) {
		printf("Starting to compute closure.\n");
		printf("-- Defined rules and metarules:\n");
		showDefs(phrDefList);
		printf("-- Metarule uses:\n");
		showUses(withFV);
	}

	for (n = 0; undone; n++) {
		struct phrUse 	*u0, *u1, *l;

		if (verbose) {
			printf("Iteration %d.\n", n);
/*
			printf("-- Done forms:\n");
			showUses(undone);
			printf("-- So far generated rules:\n");
			showUses(genSet);
*/
		}

		u0 = undone;     
		undone = undone->next; 

		u0->next = genSet; 
		genSet = u0;

		for (l = withFV; l; l = l->next) {
			if (!strcmp(l->context->name, u0->info.name)) {
				u1 = instance(l, u0);
				if (!findUse(&(u1->info), 0, undone) &&
				    !findUse(&(u1->info), 0, genSet) )
				{
					u1->next = undone; undone = u1;
				}
			}
		}
	}
		
	if (verbose) {
		printf("-- Implicit tokens:\n");
		showUses(tokenSet);
		printf("-- Generated rules:\n");
		showUses(genSet);
	}
}


/*
 * Code to gather information about parameterized phrases as they are parsed.
 */

static struct phrInfo phrInfo;
static struct phrInfo lhsInfo;
static char          *lhsType;
static struct phrDef *lhsDef;

void
_1_startProlog()
{
	tokEcho     = 0;
	tokEnlist   = 0;
}

void
_1_endProlog()
{
	if (hasPctType && (dfltTokenType || dfltRuleType))
		fatalErr();
}

void
_1_wholeEpilog()
{
	while (yylex() != 0) ;

	phraseClosure();
}

void
_1_startCommand()
{
}

void
_1_endCommand()
{
}

static EnumItem inclEnums = 0;

void
_1_seeIncludeEnum(char *fname, char *ename)
{
	EnumItem tt;

	tt        = skimEnums(fname, 1, &ename);
	if (!tt) {
		fprintf(stderr, "Could not find enumeration %s in file %s.\n",
			ename, fname);
		exit(3);
	}
	inclEnums = skimNConcat(inclEnums, tt);
}

void
_1_startPhraseName()
{
	phrInfo.name = 0;
	phrInfo.argc = 0;
}

void
_1_seeName(char *str)
{
	phrInfo.name = (!inLhs) ? renameBV(str, &lhsInfo) : str;
}

void
_1_seeArg(char *str)
{
	if (phrInfo.argc == MAXARGC) 
		fatalErr();

	str = (!inLhs) ? renameBV(str, &lhsInfo) : str;
	phrInfo.argv[phrInfo.argc++] = str;

	if (!inLhs) {
		struct phrInfo	argInfo;
		argInfo.name = str;
		argInfo.argc = 0;
		addUse(&argInfo, lhsDef);
	}
}

void
_1_endPhraseName()
{
	if (inLhs) 
		lhsInfo = phrInfo;
	else 
		addUse(&phrInfo, lhsDef);
}

void
_1_seeType(char *str)
{
	if (passno != 1) return;

	lhsType = str;
}


void
_1_startRule()
{
	lhsType = 0;
	lhsDef  = 0;
}

void
_1_midRule()
{
	lhsDef = addDef(lhsInfo.name, lhsInfo.argc, lhsType);
}

void
_1_endRule()
{
}


/******************************************************************************
 *
 * :: Functions for pass 2.
 *
 *****************************************************************************/

static int
isTerminal(char *w)
{
	struct phrUse	*l = tokenSet;

	for (l = tokenSet; l; l = l->next)
		if (!strcmp(w, l->info.name)) return 1;

	return 0;
}

char *extraSuffix[] = { "_START", "_LIMIT", 0 };

static int
isExtra(char *w)
{
	char	*x;
	int	i, nx, nw;

	nw = strlen(w);

	for (i = 0; extraSuffix[i]; i++) {
		x = extraSuffix[i];
		nx = strlen(x);

		if (nx > nw) continue;
		if (!strcmp(w + (nw-nx), x)) return 1;

	}
	return 0;
}

static void
prWord(FILE *fout, char *w)
{
	char *p;
	p = isTerminal(w) ? prefix : "";
	fprintf(fout, "%s%s", p, w);

}

void
_2_startProlog()
{	tokEcho  = 1;
	tokEnlist= 1;

	tokListHead = 0;
	tokListTail = 0;
}

void
_2_endProlog()
{
	struct phrUse	*ul;
	struct phrDef	*dl;
	char		*type;
	int		n = 0;

	/*
	 * Emit type declarations.
	 */

	if (dfltTokenType) {
#ifdef COMMENTARY
		fprintf(fout, "\n/*\n * Generated type declarations:\n */\n");
#endif
		for (ul = tokenSet; ul; ul = ul->next, n++) {
			fprintf(fout, "%%type <%s> ", dfltTokenType);
			prWord(fout, ul->info.name);
			fprintf(fout, "\n");
		}
		fprintf(fout, "\n");
	}

	for (dl = phrDefList; dl; dl = dl->next) {
		if (dl->name[0] == '`') 
			continue;
		if (dl->argc == 0) {
				type = 0;
				if (dl->type)
					type = dl->type;
				else if (dfltRuleType)
					type = dfltRuleType;
				if (type && strcmp(type, "void")) {
					n++;
					fprintf(fout, "%%type <%s> %s\n",
						type, dl->name);
				}
		}
		else for (ul = genSet; ul; ul = ul->next) {
			if (!strcmp(dl->name, ul->info.name)) {
				type = 0;
				if (dl->type)
					type = dl->type;
				else if (dfltRuleType)
					type = dfltRuleType;
				if (type && strcmp(type, "void")) {
					n++;
					fprintf(fout, "%%type <%s> ", type);
					fprintInfo(fout, &(ul->info));
					fprintf(fout, "\n");
				}
			}
		}
	}
	if (n > 0)
		fprintf(fout,"\n");
}

void
_2_wholeEpilog()
{
	while (yylex() != 0) ;

  	token(TK_Other, "");	/* Force echo of last token read */
}

void
_2_seeIncludeEnum(char *fname, char *ename)
{
	EnumItem el, tl;
	el = skimEnums(fname, 1, &ename);

#ifdef COMMENTARY
	fprintf(fout, "\n/*\n * Included token declarations:\n */\n");
#endif
	for (tl = el; tl; tl = tl->next) {
		if (isExtra(tl->id)) continue;
		fprintf(fout, "%%token %s%s %d\n", prefix, tl->id, tl->value);
	}
	fprintf(fout, "\n");

	skimFree(el);
}

void
_2_startCommand()
{
	tokEcho = 0;
}

void
_2_endCommand()
{
	tokEcho = 1;
}

void
_2_startPhraseName()
{
}

void
_2_seeName(char *str)
{
}

void
_2_seeArg(char *str)
{
}

void
_2_endPhraseName()
{
}

void
_2_seeType(char *str)
{
}

void
_2_startRule()
{
	tokEcho = 0;
	tokListHead = tokListTail;
}

void
_2_midRule()
{
}

void
_2_endRule()
{
	tokEcho = 1;
	handleRule(tokListHead);
}


struct tokList *
nextMeaty(struct tokList	*tl)
{
	while (tl && (tl->tag == TK_Space || tl->tag == TK_Comment))
		tl = tl->next;
	return tl;
}
	
void
emitRule(struct tokList	*tl0, struct phrUse	*use)
{
	struct tokList	*tl;
	char	*str;

	for (tl = tl0; tl; tl = tl->next) {
		if (!tl->show) continue;

		str = tl->str;
		
		switch (tl->tag) {
		  case TK_CTok:
		  case TK_Id: {
			if (use) {
				int a = unnameBV(str);
				if (a >= 0) str = use->info.argv[a];
			}
			if (*str == '`') str++;
			prWord(fout, str);
			break;
		  }
		  case TK_OPren:
		  case TK_Comma:
		  case TK_CPren:
			fprintf(fout, "_");
			break;
		  default:
			fprintf(fout, "%s", str);
		}
	}
}


void
handleRule(struct tokList	*tl0)
{
	struct tokList	*tl;
	struct phrInfo	info;
	struct phrUse	*ul;
	int		inAppl, inType; /* For removing words */
	int		n;

	/*
	 * Preprocess: 
	 * 1. Remove type decl as well as spaces and comments within 
	 *    metarule applications.
	 * 2. Rename bound variables to standard names.
	 */

	inAppl = inType = 0;
	for (tl = tl0; tl; tl = tl->next) {
		if (tl->tag == TK_Id) {
			struct tokList	*tn = nextMeaty(tl->next);
			inAppl = inAppl || (tn && tn->tag == TK_OPren);
		}
		else if (tl->tag == TK_OAngle) {
			inType = 1;
		}

		if (inAppl && (tl->tag == TK_Comment || tl->tag == TK_Space))
			tl->show = 0;
		else if (inType)
			tl->show = 0;
		else
			tl->show = 1;

		if (tl->tag == TK_CPren) {
			inAppl = 0;
		}
		else if (tl->tag == TK_CAngle) {
			inType = 0;
		}
	}

	info.name = 0;
	info.argc = 0;

	for (tl = tl0; tl; tl = tl->next) {
		if (!tl->show)
			continue;
		if (tl->tag == TK_Id) {
			if (!info.name) 
				info.name = tl->str;
			else 
				info.argv[info.argc++] = tl->str;
		}
		else if (tl->tag == TK_Colon)
			break;
	}

	for (tl = tl0; tl; tl = tl->next) {
		if (tl->tag == TK_Id || tl->tag == TK_CTok) 
			tl->str = renameBV(tl->str, &info);
	}

	/*
	 * Emit rule instances.
	 */

	if (info.argc == 0) {
		emitRule(tl0, 0);
	}
	else for (n = 0, ul = genSet; ul; n++, ul = ul->next) {
		if (!strcmp(info.name, ul->info.name)) {
			if (n != 0) fprintf(fout, "\n");
			emitRule(tl0, ul);
		}
	}
}

/*****************************************************************************
 *
 * :: Utilities
 *
 *****************************************************************************/

char *
mustAlloc(int	n)
{
	/*extern char *malloc();*/
	char	*s;
	s = malloc(n);
	if (!s) fatalErr();
	return s;
}

char *
strAlloc(char	*str)
{
	return strcpy(mustAlloc(strlen(str) + 1), str);
}

char *
fnameDir(char	*fn)
{
	int	i, n, slashPos;
	char	*d;
	
	n        = strlen(fn);
	slashPos = -1;
	for (i = n-1; i >= 0 && slashPos == -1; i--)
		if (fn[i] == '/') slashPos = i;

	if (slashPos == -1)
		d = strAlloc(".");
	else {
		d = mustAlloc(slashPos+1);
		strncpy(d, fn, slashPos);
		d[slashPos] = 0;
	}
	return d;
}

FILE *
mustOpen(char	*fname, char	*mode)
{
	FILE *f;
	f = fopen(fname, mode);
	if (!f) fatalErr();
	return f;
}

char *
timeNow()
{
	char	*s;
	long	t;
	
	t = time(0);
	s = ctime(&t);

	s[strlen(s)-1] = 0;
	return s;
}

void
fatalErr()
{
#if 0
	va_list argp;
#endif

	fprintf(stderr, "zacc (fatal error): ");
#if 0
	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);
#endif
	fprintf(stderr, "\n");

	exit(3);
}
