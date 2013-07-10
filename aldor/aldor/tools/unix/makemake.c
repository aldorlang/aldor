/*
 * a.out [-loose] [-expanded] [-verbose]
 *                -Vx="a.h .." 
 *                -Iv="/u/i"
 *                -As.a="w.c .."
 *                -Ly.a="u.c .."
 *                -Xp="m.c y.a -lm"
 *
 */

/******************************************************************************
 *
 * :: Declarations
 *
 *****************************************************************************/

# define NDEBUG

# include <stdio.h>
# include <assert.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>	/* for file size */

/*
 * Top Level.
 */
char		*Prog = "makemake";
int		verboseFlag    = 0;
int		looseFlag      = 0;
int		expandedFlag   = 0;
int		explicitLibDir = 0;
int		isMsDos	       = 0;
int		isMinGW	       = 0;
void		parseArgDef();

char *arFlags = "rv";

/*
 * Basic structure manipulations.
 */
struct link	*newLink();
void		freeLinks();
void		printLinks();
struct link	*nreverseLinks();
struct link	*copyLinks();
int		countLinks();
struct link	*sortLinks();
struct link	*nmergesortLinks();
struct link	*nmergeLinks();

int		nodeCount = 0;
struct link 	*allNodes = 0;

struct node	*getNode();
struct node	*needNodeIn();
void		printNodes();

struct link	*unionLinks();
struct link	*unionNode();

void		probeNode();

#define NO_ADJACENCY	0
#define ADJACENCY	1

#define NO_CLOSURE	0
#define CLOSURE		1

#define NO_RETRACTION	0
#define RETRACTING      1

#define NO_PROBE	0
#define PROBE		1

/*
 * Include graph construction.
 */
void		getIncludes();

void		buildGraph();
void		closeGraph();
struct link	*closeNode();
void 		retractGraph();
void		retractNode();

/*
 * File stuff.
 */
long		fsize();
char		*freadIntoString();

int		fntype();
char		*fnWithType();

#define 	FTYPE_LIB	10	/* libaxl.a -- can be .o collection */
#define		FTYPE_AR	11	/* libaxl.a -- must be an archive   */
#define		FTYPE_SRC	12	/* tinfer.c */
#define 	FTYPE_HDR	13	/* tinfer.h */
#define		FTYPE_OBJ	14	/* tinfer.o */
#define 	FTYPE_VAR	15	/* $(ALDORPORTH) */
#define		FTYPE_EXE	16	/* aldor */
#define		FTYPE_DIR	17	/* /usr/include */
#define		FTYPE_NONE	18	/* Makefile */
#define		FTYPE_FLAG	19	/* -lm */

/*
 * Makefile generation.
 */
# define	MK_NAME_MAX_SIZE 1024

char		mkNameBuf[MK_NAME_MAX_SIZE];

char		* mkNameBin();		/* $(BIN)/foo          */
char		* mkNameLibOpt();	/* -lfoo               */
char		* mkNameLib();		/* $(LIB)/libfoo.a     */
char		* mkNameSrc();		/* $(SRC)/foo.c        */
char		* mkNameObj();		/* $(LIB)/foo.o        */
char		* mkNameLibObj();	/* $(LIB)/foo.a(bar.o) */
char		* mkNameVar();		/* $(MyName)           */
char		* mkNameWith();		/* fooMyName           */
char		* mkNameVarWith();	/* $(fooMyName)        */

void		mkPutBoilerPlate();
void		mkPutSetDefs();
void		mkPutTargetDefs();

void		mkPutTargets();
void		mkPutLibraries();
void		mkPutObjRules();

/*
 * Makefile output.
 */
void		mkPutStr();	/* This will break as necessary with "\". */
void		mkBlank();	/* Put a blank. */
void		mkNewLn();	/* End line. */
void		mkBegStats();	/* Begin indent commands. */
void		mkEndStats();	/* End indented commands. */

/*
 * Utilities.
 */
char		*mustAlloc();
char		*strCopy();
void		fatal();


/******************************************************************************
 *
 * :: Main program
 *
 *****************************************************************************/

int
main(argc, argv)
	int	argc;
	char	**argv;
{
	/*
	 * Handle command line.
	 */
	argc--, argv++;

	for ( ; argc > 0 && argv[0][0] == '-'; argc--, argv++) {
		if (!strcmp(argv[0], "-loose"))
			looseFlag = 1;
		else if (!strcmp(argv[0], "-verbose"))
			verboseFlag = 1;
		else if (!strcmp(argv[0], "-expanded"))
			expandedFlag = 1;
		else if (!strcmp(argv[0], "-msdos"))
			isMsDos = 1;
		else if (!strcmp(argv[0], "-mingw"))
			isMinGW = 1;
		else
			switch (argv[0][1]) {
			case 'V':
				parseArgDef(argv[0] + 2, FTYPE_VAR);
				break;
			case 'I':
				parseArgDef(argv[0] + 2, FTYPE_VAR);
				break;
			case 'A':
				parseArgDef(argv[0] + 2, FTYPE_AR);
				break;
			case 'L':
				parseArgDef(argv[0] + 2, FTYPE_LIB);
				break;
			case 'X':
				parseArgDef(argv[0] + 2, FTYPE_EXE);
				break;
			case 'f':
				arFlags = argv[0] + 2;
				break;
			default:
				fatal("Don't recognize <%s>.", argv[0]);
			}
	}
	allNodes = nreverseLinks(allNodes);

	/*
	 * Process files.
	 */
	buildGraph();
	closeGraph();
	if (!expandedFlag) retractGraph();

	/*
	 * Write make file.
	 */
	mkPutBoilerPlate();

	if (!expandedFlag) mkPutSetDefs();
	mkPutTargetDefs (looseFlag);

	mkPutTargets    (looseFlag);
	mkPutLibraries  (looseFlag);
	mkPutObjRules   (looseFlag);

	return 0;
}

/******************************************************************************
 *
 * :: File information structures
 *
 *****************************************************************************/

/*
 * A node structure represents either
 * (1) a vertex, in which case its out are the direct includees, or
 * (2) a set,    in which case inclusion of a member is implies
 *               inlcusion of the set.
 */

struct node {
	char		type;

	char		adjacency;
	int		closure;
	char		retraction;
	char		probe;

	int		id;
	char		*name;
	char		*full;	/* full name only exists if probe == PROBE */
	struct link	*in;	/* only has FTYPE_LIB and FTYPE_SET nodes */
	struct link	*out;
};

struct link {
	struct node	*node;
	struct link	*next;
};

#define nodeName(n)	(((n)->probe == PROBE) ? (n)->full : (n)->name)

#define LINK_ALLOC_CHUNK	1024
struct link	*theFreeLinks = 0;

struct link *
newLink(n, lo)
	struct node	*n;
	struct link	*lo;
{
	struct link	*ln;

	if (!theFreeLinks) {
		int	i;
		ln = (struct link *) mustAlloc(LINK_ALLOC_CHUNK * sizeof(*ln));
		for (i = 0; i < LINK_ALLOC_CHUNK; i++) {
			ln[i].next = theFreeLinks;
			theFreeLinks = &ln[i];
		}
	}
	ln = theFreeLinks;
	theFreeLinks = theFreeLinks->next;

	ln->node = n;
	ln->next = lo;
	return ln;
}

struct link *
nreverseLinks(l)
	struct link	*l;
{
	struct link	*r, *t;

	r = 0;
	while (l) {
		t = l->next;
		l->next = r;
		r = l;
		l = t;
	}
	return r;
}

void
freeLinks(l)
	struct link	*l;
{
	struct link	*t;

	while (l) {
		t = l->next;
		l->next = theFreeLinks;
		theFreeLinks = l;
		l = t;
	}
}

struct link *
copyLinks(l)
	struct link	*l;
{
	struct link	*r, *p;

	if (l) {
		r = p = newLink(l->node, 0);
		
		for (l = l->next; l; l = l->next)
			p = p->next = newLink(l->node, 0);
	}
	else
		r = 0;
	return r;
}

void
printLinks(l)
	struct link	*l;
{
	printf("[");
	for ( ; l ; l = l->next)
		printf(" %s", l->node->name);
	printf("]");
}

int
countLinks(l)
	struct link	*l;
{
	int		n;
	for (n = 0; l; l = l->next) n++;
	return n;
}

struct link *
sortLinks(l)
	struct link	*l;
{
	return nmergesortLinks(copyLinks(l), countLinks(l));
}

struct link *
nmergesortLinks(l, n)
	struct link	*l;
	int		n;
{
	struct link	*l1, *l2, *t;
	int		i, n1, n2;

	assert(n == countLinks(l));

	if (n < 2) return l;

	n1 = n / 2;
	n2 = n - n1;
	for (i = n1, l1 = 0, l2 = l; i > 0 && l2; i--) {
		t = l2->next;
		l2->next = l1;
		l1 = l2;
		l2 = t;
	}
	l1 = nmergesortLinks(l1, n1);
	l2 = nmergesortLinks(l2, n2);
	l  = nmergeLinks(l1, l2);

	return l;
}
		
struct link *
nmergeLinks(l1, l2)
	struct link	*l1, *l2;
{
	struct link	*t, *r;

	r = 0;
	while (l1 || l2) {
		if (l1 && (!l2 || l1->node->id <= l2->node->id)) {
			t  = l1->next;
			l1->next = r;
			r  = l1;
			l1 = t;
		}
		if (l2 && (!l1 || l1->node->id >  l2->node->id)) {
			t  = l2->next;
			l2->next = r;
			r  = l2;
			l2 = t;
		}
	}
	return nreverseLinks(r);
}

struct node *
getNode(name)
	char *name;
{
	struct link	*l;
	struct node	*n;

	for (l = allNodes; l; l = l->next) 
		if (!strcmp(name, l->node->name)) return l->node;

	n = (struct node *) mustAlloc(sizeof(*n));
	n->type       = fntype(name);
	n->adjacency  = NO_ADJACENCY;
	n->closure    = NO_CLOSURE;
	n->retraction = NO_RETRACTION;
	n->probe      = NO_PROBE;
	n->name       = strCopy(name);
	n->full       = 0;
	n->in         = 0;
	n->out        = 0;
	n->id         = nodeCount++;

	allNodes      = newLink(n, allNodes);

	return n;
}

struct node *
needNodeIn(name, plink)
	char		*name;
	struct link	**plink;
{
	struct node	*n;
	struct link	*l;

	for (l = *plink ; l; l = l->next)
		if (!strcmp(name, l->node->name)) return l->node;
	n = getNode(name);
	*plink = newLink(n, *plink);
	return n;
}

void
printNodes()
{
	struct link	*l, *a;
	struct node	*n;

	for (l = allNodes; l; l = l->next) {
		n = l->node;
		printf("Node '%s'", n->name);
		switch (n->type) {
		case FTYPE_LIB:   printf(" (lib) "); break;
		case FTYPE_OBJ:   printf(" (obj) "); break;
		case FTYPE_SRC:   printf(" (src) "); break;
		case FTYPE_HDR:   printf(" (hdr) "); break;
		case FTYPE_VAR:   printf(" (var) "); break;
		case FTYPE_EXE:   printf(" (exe) "); break;
		default:   	  printf(" (other) "); break;
		}
		printf("<-");
		for (a = n->in; a; a = a->next)
			printf("'%s' ", a->node->name);
		printf("->");
		for (a = n->out; a; a = a->next)
			printf("'%s' ", a->node->name);
		printf("\n");
	}
}


struct link *
unionLinks(l1, l2)
	struct link	*l1, *l2;
{
	struct link	*r = l2;

	for ( ; l1 ; l1 = l1->next)
		r = unionNode(l1->node, r);
	return r;
}

struct link *
unionNode(n, l0)
	struct node	*n;
	struct link	*l0;
{
	struct link	*l;

	for (l = l0; l; l = l->next) 
		if (n->id == l->node->id) return l0;
	return newLink(n, l0);
}

void
probeNode(n)
	struct node	*n;
{
	int		rc;
	struct stat	buf;
	struct link	*l;

	if (n->probe == PROBE)
		return;

	rc = stat(n->name, &buf);
	if (rc != -1) {
		n->probe = PROBE;
		n->full = n->name;
		return;
	}

	for (l = allNodes; l; l = l->next)
		if (l->node->type == FTYPE_DIR) {
			sprintf(mkNameBuf, "%s/%s", l->node->name, n->name);
			rc = stat(mkNameBuf, &buf);
			if (rc != -1) {
				n->probe = PROBE;
				n->full = strCopy(mkNameBuf);
				return;
			}
		}
	fatal("Could not read file '%s'.", n->name);
}

/******************************************************************************
 *
 * :: Argument parser
 *
 *****************************************************************************/

/*
 * Create a new structure from a string "xxx = yyy yyy yyy"
 */
void
parseArgDef(arg0, ftype)
	char		*arg0;
	int		ftype;
{
	char		*arg, *t;
	struct node	*f, *a;

	t = arg0 = strCopy(arg0);

	/* xxx */
	while (*t == ' ' || *t == '\t') t++;
	arg = t;
	while (*t && *t != ' ' && *t != '\t' && *t != '=') t++;
	*t++ = 0;

	f = getNode(arg);
	f->type = ftype;

	/* = */
	while (*t == ' ' || *t == '\t') t++;
	if (*t == '=') t++;

	while (*t) {
		/* yyy */
		while (*t == ' ' || *t == '\t') t++;
		arg = t;
		while (*t && *t != ' ' && *t != '\t') t++;
		if (*t) *t++ = 0;
		
		if (*arg) {
			a = getNode(arg);
			f->out = unionNode(a, f->out);
			if (ftype != FTYPE_EXE)
				a->in  = unionNode(f, a->in);
		}
	}
	f->out       = nreverseLinks(f->out);
	f->adjacency = ADJACENCY;

	free(arg0);
}
			
/******************************************************************************
 *
 * :: Build graph
 *
 *****************************************************************************/

/*
 * Find immediate neighbors from "# include" statements.
 */
void
buildGraph()
{
	int		c0;
	struct link	*l;
	struct node	*n;

	do {
		c0 = nodeCount;
		for (l = allNodes; l; l = l->next) {
			n = l->node;
			if ((n->type == FTYPE_HDR || n->type == FTYPE_SRC) &&
			     n->adjacency == NO_ADJACENCY)
			{
				getIncludes(n);
				n->adjacency = ADJACENCY;
			}
		}
	} while (c0 != nodeCount);
}

/*
 * Find all direct and indirect includes.
 */
int CLOSING = CLOSURE;

void
closeGraph()
{
	struct link	*l;
	struct node	*n;

	for (l = allNodes; l; l = l->next)
		l->node->closure = NO_CLOSURE;

	for (l = allNodes; l; l = l->next) {
		n = l->node;
		if (n->type == FTYPE_HDR || n->type == FTYPE_SRC) {
			CLOSING   += 1;
			n->out     = closeNode(n);
			n->closure = CLOSURE;
		}
	}
}

struct link *
closeNode(n)
	struct node	*n;
{
	struct link	*l, *all;

	if (n->closure == CLOSURE)
		all = n->out;
	else if (n->closure == CLOSING)
		all = 0;
	else {
		n->closure = CLOSING;
		all = n->out;
		for (l = n->out; l; l = l->next)
			all = unionLinks(closeNode(l->node), all);
	}

	return all;
}

/*
 * Make each source file's include list use variables, wherever possible.
 */
void
retractGraph()
{
	struct link	*l;
	struct node	*n;

	for (l = allNodes; l; l = l->next) {
		n = l->node;
		if (n->type == FTYPE_SRC) retractNode(n);
	}
}

void
retractNode(n)
	struct node	*n;
{
	struct link	*l, *p, *new;

	/* 1. Clear all retraction bits. */
	for (l = allNodes; l; l = l->next)
		l->node->retraction = NO_RETRACTION;

	/* 2. Mark all nodes in the out list and their parents. */
	for (l = n->out; l; l = l->next) {
		l->node->retraction = RETRACTING;
		for (p = l->node->in; p; p = p->next)
			p->node->retraction = RETRACTING;
	}

	/* 3. Turn off all parents with unmarked children. */
	for (l = allNodes; l; l = l->next) {
		if (l->node->type != FTYPE_VAR) continue;
		if (l->node->retraction == NO_RETRACTION) continue;

		for (p = l->node->out; p; p = p->next)
			if (p->node->retraction == NO_RETRACTION) {
				l->node->retraction = NO_RETRACTION;
				break;
			}
	}

	/* 5. Collect marked parents of children and loose children. */
	new = 0;
	for (l = n->out; l; l = l->next) {
		int	looseChild = 1;
		for (p = l->node->in; p; p = p->next)
			if (p->node->retraction == RETRACTING) {
				looseChild = 0;
				new = unionNode(p->node, new);
			}
		if (looseChild)
			new = unionNode(l->node, new);
	}
	freeLinks(n->out);
	n->out = new;
}

/*
 * Record the #include directives in a file.
 */
void
getIncludes(n)
	struct node	*n;
{
	register char	*s, *e;
	int		olde;

	probeNode(n);
	s = freadIntoString(nodeName(n), verboseFlag);

	while (*s) {
		/* Skip lines which are not preprocessor directives. */
		if (*s != '#') goto skip_line;

		/* Skip lines which are not "#[ \t]*include[ \t]*". */
		s++;
		while (*s == ' ' || *s == '\t') s++;
		if (strncmp(s, "include", 7)) goto skip_line;
		s += 7;
		while (*s == ' ' || *s == '\t') s++;

		/* Only look at includes in quotes, not in angles. */
		if (*s != '"') goto skip_line;

		/* Look ahead to end of file name. */
		s++;
		e = s;
		while (*e && *e != '"') e++;

		/* Temporarily stomp it. */
		olde = *e;
		*e = 0;

		/* Add to include list. */
		needNodeIn(s, &n->out);

		/* Restore and resume. */
		*e = olde;
		s = e;

	skip_line:
		while (*s && *s != '\n') s++;
		if (*s == '\n') s++;
	}
}

/******************************************************************************
 *
 * :: Makefile generation
 *
 *****************************************************************************/

void
mkPutBoilerPlate()
{
	mkPutStr("#"); mkNewLn();
	mkPutStr("# This makefile was machine-generated."); mkNewLn();
	mkPutStr("#"); mkNewLn();
	mkPutStr("# The variables can be overridden by, e.g.,"); mkNewLn();
	mkPutStr("#"); mkNewLn();
	mkPutStr("#       make SRC=../src CC=xlc -f thisfile"); mkNewLn();
	mkPutStr("#"); mkNewLn();
	mkNewLn();

	mkPutStr("SHELL=/bin/sh");	mkNewLn();
	if (isMsDos)
		mkPutStr("RANLIB=ranlib");
	else
		mkPutStr("RANLIB=doranlib");
	mkNewLn();
        mkPutStr("AR=ar");   		         mkNewLn();
        mkPutStr("ARFLAGS="); mkPutStr(arFlags); mkNewLn();
	mkPutStr("LD=$(CC)");			 mkNewLn();
	mkPutStr("LDFLAGS=$(CFLAGS)");		 mkNewLn();

	if (isMsDos) {
		mkPutStr("OBJ=obj");	mkNewLn();

                mkPutStr("CFLAGS= -DSTO_FREE_JUNK -DSTO_NEW_JUNK -O -DNDEBUG -I$(SRC)");

		mkNewLn();

		mkPutStr("DOSEXT=go32.exe");	mkNewLn();
		mkPutStr("#DOSEXT=stub.exe");	mkNewLn();

	}
	mkNewLn();

	if (isMsDos) {
		mkPutStr("BIN=..\\bin");	mkNewLn();
		mkPutStr("SRC=E:");		mkNewLn();
	}
	else {
		mkPutStr("BIN=../bin");		mkNewLn();
		mkPutStr("SRC=.");		mkNewLn();
	}
	mkNewLn();
}

/*
 * Put defintions of variables, e.g.
 *
 *	ALDORPORT = axlport.h opsys.h
 */
void
mkPutSetDefs()
{
	struct link	*l, *out;
	struct node	*s, *n;

	for (l = allNodes; l; l = l->next) {
		s = l->node;
		if (s->type == FTYPE_VAR) {
			mkPutStr(nodeName(s));
			mkPutStr(" = ");
			for (out = s->out; out; out = out->next) {
				n = out->node;
				mkPutStr(mkNameSrc(nodeName(n)));
				mkBlank();
			}
			mkNewLn();
			mkNewLn();
		}
	}
}

/*
 * Put definitions for all targets:
 *
 *	fooObjects = a.o b.o ..
 */
void
mkPutTargetDefs(looseFlag)
	int	looseFlag;
{
	struct link	*t, *l, *p;
	struct node	*n, *s, *o;

	for (t = allNodes; t; t = t->next) {
		n = t->node;

		if (n->type != FTYPE_EXE) continue;

		mkPutStr(mkNameWith(nodeName(n), "Objects"));
		mkPutStr(" =");
		for (l = n->out; l; l = l->next) {
			s = l->node;
			if (s->type == FTYPE_SRC) {
				mkBlank();
				mkPutStr(mkNameObj(nodeName(s)));
			}
			if (s->type == FTYPE_LIB && looseFlag) {
				for (p = s->out; p; p = p->next) {
					o = p->node;
					mkBlank();
					mkPutStr(mkNameObj(nodeName(o)));
				}
			}
		}
		mkNewLn();
		mkNewLn();
	}
	mkNewLn();
	mkNewLn();
}

/*
 * Put rules for all targets:
 *
 *	all: this that
 *	this: $(BIN)/this
 *	that: $(BIN)/that
 *
 *	$(BIN)/this: ....
 */
void
mkPutTargets(looseFlag)
	int	looseFlag;
{
	struct link	*l, *t, *targs;
	struct node	*n, *s;

	targs = 0;
	for (l = allNodes; l; l = l->next)
		if (l->node->type == FTYPE_EXE)
			targs = newLink(l->node, targs);

	mkPutStr("all:");

	if (isMsDos) {
		mkBlank();
		mkPutStr("subst ");
	}

	for (t = targs; t; t = t->next) {
		mkBlank();
		mkPutStr(nodeName(t->node));
	}

	if (isMsDos) {
		mkBlank();
		mkPutStr("undosubst ");
	}

	mkNewLn();
	mkNewLn();

	if (isMsDos) {
		mkPutStr("subst:; -@subst $(SRC) $(ALDORVER)\\src >nul");
		mkNewLn();
		mkPutStr("undosubst:; -@subst /d $(SRC)");
		mkNewLn();
	}

	for (t = targs; t; t = t->next) {
		mkPutStr(nodeName(t->node));
		mkPutStr(": ");
		mkPutStr(mkNameBin(nodeName(t->node)));
		mkNewLn();
	}
	mkNewLn();

	for (t = targs; t; t = t->next) {
		n = t->node;

		mkPutStr(mkNameBin(nodeName(n)));
		mkPutStr(": ");
		mkPutStr(mkNameVarWith(nodeName(n), "Objects"));

		for (l = n->out; l; l = l->next) {
			s = l->node;
			if (s->type == FTYPE_LIB && !looseFlag) {
				mkBlank();
				mkPutStr(mkNameLib(nodeName(s)));
			}
			if (s->type == FTYPE_AR) {
				mkBlank();
				mkPutStr(mkNameLib(nodeName(s)));
			}
		}
		mkNewLn();

		mkBegStats(); 

		mkPutStr("$(LD) $(LDFLAGS) ");
		mkPutStr(mkNameVarWith(nodeName(n), "Objects"));

		mkPutStr(" -L.");

		for (l = n->out; l; l = l->next) {
			s = l->node;
			if (s->type == FTYPE_LIB && !looseFlag) {
				mkBlank();
				mkPutStr(mkNameLibOpt(nodeName(s)));
			}
			if (s->type == FTYPE_AR) {
				mkBlank();
				mkPutStr(mkNameLibOpt(nodeName(s)));
			}
			if (s->type == FTYPE_FLAG) {
				mkBlank();
				mkPutStr(nodeName(s));
			}
		}
		mkNewLn();

		if (isMsDos) 
			mkPutStr("copy /b $(GNU)\\bin\\$(DOSEXT)+a.out ");
		else if (isMinGW)
			mkPutStr("mv a.exe ");
		else
			mkPutStr("mv a.out ");
		mkPutStr(mkNameBin(nodeName(n)));
		mkNewLn();

		mkEndStats();
	}

	freeLinks(targs);
}

/*
 * Put rules for all libraries, including .PRECIOUS rules.
 */
void
mkPutLibraries(looseFlag)
	int	looseFlag;
{
	struct link	*l, *out, *libs;
	struct node	*n, *o;

	libs = 0;
	for (l = allNodes; l; l = l->next) {
		n = l->node;
		if (n->type == FTYPE_LIB && !looseFlag)
			libs = newLink(n, libs);
		if (n->type == FTYPE_AR)
			libs = newLink(n, libs);
	}

	for (l = libs; l; l = l->next) {
		n = l->node;
		mkPutStr(".PRECIOUS: ");
		mkPutStr(mkNameLib(nodeName(n)));
		mkNewLn();
	}
	mkNewLn();
		
	for (l = libs; l; l = l->next) {
		n = l->node;
		mkPutStr(mkNameLib(nodeName(n)));
		mkPutStr(":");
		for (out = n->out; out; out = out->next) {
			o = out->node;
			mkBlank();
			mkPutStr(mkNameLibObj(nodeName(o), nodeName(n)));
		}
		mkNewLn();
	
		mkBegStats();

		mkPutStr("$(RANLIB) ");
		mkPutStr(mkNameLib(nodeName(n)));
		mkNewLn();

		mkEndStats();
	}
}

/*
 * Put object rules, e.g.
 *
 *	x.o libt.a(x.o): $(SRC)/x.c $(SRC)/foo.h $(ALDORPORT)
 */
void
mkPutObjRules(looseFlag)
	int	looseFlag;
{
	struct link	*l, *in, *out0, *out;
	struct node	*s, *n;
	int		nsaid, nin;

	for (l = allNodes; l; l = l->next) {
		s = l->node;
		if (s->type != FTYPE_SRC) continue;

		if (s->in) {
			for (nin = 0, in = s->in; in; in = in->next)
				if (in->node->type != FTYPE_VAR) nin++;
			if (nin == 0) continue;
		}

		/*
		 * Give targets:
		 *	archive member for each parent if !looseFlag
		 *	separate .o if looseFlag or no parent
		 */
		if (!s->in) {
			mkPutStr(mkNameObj(nodeName(s)));
		}
		else {
			nsaid = 0;
			if (looseFlag) {
				mkPutStr(mkNameObj(nodeName(s)));
				nsaid++;
			}
			for (in = s->in; in; in = in->next) {
				n = in->node;
				if ((!looseFlag && n->type != FTYPE_VAR)
				    || n->type == FTYPE_AR)
				{
					if (nsaid > 0) mkBlank();
					mkPutStr(mkNameLibObj(nodeName(s),
							      nodeName(n)));
					nsaid++;
				}
			}
		}
				
		mkPutStr(": ");
		mkPutStr(mkNameSrc(nodeName(s)));
		mkBlank();

		out0 = sortLinks(s->out);
		for (out = out0; out; out = out->next) {
			n = out->node;
			if (n->type == FTYPE_VAR)
				mkPutStr(mkNameVar(nodeName(n)));
			else
				mkPutStr(mkNameSrc(nodeName(n)));
			mkBlank();
		}
		freeLinks(out0);
		mkNewLn();

		mkBegStats();
		mkPutStr("$(CC) $(CFLAGS) -c ");
		mkPutStr(mkNameSrc(nodeName(s)));
		mkNewLn();

		if (isMsDos) {
			mkPutStr("copy ");
			mkPutStr(mkNameObj(nodeName(s)));
			mkPutStr(" $(OBJ)\\.");
			mkNewLn();
		}

		if (s->in) {
			for (in = s->in; in; in = in->next) {
				if (!looseFlag || in->node->type == FTYPE_AR) {
					mkPutStr("$(AR) $(ARFLAGS) ");
					mkPutStr(nodeName(in->node));
					mkBlank();
					mkPutStr(mkNameObj(nodeName(s)));
					mkNewLn();
				}
			}
			if (!looseFlag) {
				mkPutStr("rm -f ");
				mkPutStr(mkNameObj(nodeName(s)));
				mkNewLn();
			}
		}
		mkEndStats();
	}
}

/*
 * Make different flavours of file names.
 */

/* $(SRC)/foo.c */
char *
mkNameSrc(fn)
	char	*fn;
{
	if (*fn == '/')
		sprintf(mkNameBuf, "%s", fn);
	else
		sprintf(mkNameBuf, "$(SRC)/%s", fn);
	return mkNameBuf;
}

/* $(LIB)/foo.o */
char *
mkNameObj(fn)
	char 	*fn;
{
	char	*s = mkNameBuf;
	if (explicitLibDir) {
		strcpy(s, "$(LIB)/");
		s += strlen(s);
	}
	fnWithType(s, fn, FTYPE_OBJ);
	return mkNameBuf;
}

/* $(LIB)/foo.a */
char *
mkNameLib(fn)
	char	*fn;
{
	char	*s = mkNameBuf;
	if (explicitLibDir) {
		strcpy(s, "$(LIB)/");
		s += strlen(s);
	}
	strcpy(s, fn);
	return mkNameBuf;
}

/* $(BIN)/foo */
char *
mkNameBin(fn)
	char	*fn;
{
	if (isMsDos)
		sprintf(mkNameBuf, "$(BIN)\\%s.exe", fn);
	else
		sprintf(mkNameBuf, "$(BIN)/%s", fn);
	return mkNameBuf;
}

/* $(LIB)/foo.a(bar.o) */
char *
mkNameLibObj(fn, ln)
	char	*fn, *ln;
{
	char	*s = mkNameBuf;

	if (isMsDos) {
		strcpy(s, "$(OBJ)/");
		s += strlen(s);
		fnWithType(s, fn, FTYPE_OBJ);
		s += strlen(s);
		*s = 0;
		return mkNameBuf;
	}

	if (explicitLibDir) {
		strcpy(s, "$(LIB)/");
		s += strlen(s);
	}
	strcpy(s, ln);
	s += strlen(s);
	*s++ = '(';
	fnWithType(s, fn, FTYPE_OBJ);
	s += strlen(s);
	*s++ = ')';
	*s   = 0;
	return mkNameBuf;
}

/* libfoo.a -> -lfoo */
char *
mkNameLibOpt(fn)
	char	*fn;
{
	char	*s = mkNameBuf;
	strcpy(s, "-l");
	s += strlen(s);
	fnWithType(s, fn + strlen("lib"), FTYPE_NONE);
	return mkNameBuf;
}

/* $(foo) */
char *
mkNameVar(fn)
	char 	*fn;
{
	sprintf(mkNameBuf, "$(%s)", fn);
	return mkNameBuf;
}

/* fooWith */
char *
mkNameWith(fn, more)
	char 	*fn;
	char	*more;
{
	sprintf(mkNameBuf, "%s%s", fn, more);
	return mkNameBuf;
}

/* $(fooWith) */
char *
mkNameVarWith(fn, more)
	char 	*fn;
	char 	*more;
{
	sprintf(mkNameBuf, "$(%s%s)", fn, more);
	return mkNameBuf;
}

/******************************************************************************
 *
 * :: Makefile output
 *
 *****************************************************************************/

int	mkIsInStats = 0;
int	mkCol = 0;
int	mkMax = 78;
#define	TABSTOP 8

void
mkPutStr(s)
	char	*s;
{
	int	n = strlen(s);
	int	i;
	if (n + mkCol >= mkMax) {
		printf("\\\n");
		mkCol = 0;
		for (i = 0; i < 12; i++) { putchar(' '); mkCol++; }
	}
	printf("%s", s);
	mkCol += n;
}

void
mkBlank()
{
	printf(" ");
	mkCol++;
}

void
mkBegStats()
{
	mkIsInStats = 1;

	if (mkCol == 0) {
		printf("\t");
		mkCol += TABSTOP;
	}
	else
		mkNewLn();
}

void
mkEndStats()
{
	mkIsInStats = 0;
	mkNewLn();
}

void
mkNewLn()
{
	printf("\n");
	mkCol = 0;
	if (mkIsInStats) {
		printf("\t");
		mkCol += TABSTOP;
	}
}

/******************************************************************************
 *
 * :: File stuff
 *
 *****************************************************************************/

/*
 * Return file type.
 */
int
fntype(fn)
	char	*fn;
{
	char	*e;

	if (*fn == '-') return FTYPE_FLAG;
	if (*fn == '/') return FTYPE_DIR;

	e = fn + strlen(fn) - 1;
	while (e > fn && *e != '.') e--;

	if (e == fn)         	return FTYPE_VAR;
	if (!strcmp(e, ".a"))	return FTYPE_LIB;
	if (!strcmp(e, ".c"))	return FTYPE_SRC;
	if (!strcmp(e, ".h"))	return FTYPE_HDR;
	if (!strcmp(e, ".h0"))	return FTYPE_HDR;

	return FTYPE_VAR;
}

/*
 * Replace type of file name into a buffer.
 */
char *
fnWithType(buf, fn, type)
	char	*buf;
	char	*fn;
	int	type;
{
	int	n;
	char	*e, *b = buf;

	/* Compute length of base part. */
	e = fn + strlen(fn) - 1;
	while (e > fn && *e != '.') e--;
	n = (e == fn) ? strlen(fn) : e - fn;

	/* Copy in base, then type. */
	while (n-- > 0) *b++ = *fn++;
	switch (type) {
	case FTYPE_LIB: *b++ = '.'; *b++ = 'a'; *b++ = 0; break;
	case FTYPE_SRC: *b++ = '.'; *b++ = 'c'; *b++ = 0; break;
	case FTYPE_HDR: *b++ = '.'; *b++ = 'h'; *b++ = 0; break;
	case FTYPE_OBJ: *b++ = '.'; *b++ = 'o'; *b++ = 0; break;
	default:        *b++ = 0;   break;
	}
	return buf;
}

/*
 * Determine file size.
 */
long
fsize(fn)
	char	*fn;
{
	int	rc;
	struct stat buf;

	rc = stat(fn, &buf);
	if (rc == -1) fatal("Cannot find file '%s'.", fn);
	return buf.st_size;
}

/*
 * Read a file into a local buffer.
 * The buffer grows to accomodate the largest file seen.
 */
char *
freadIntoString(fn, vb)
	char	*fn;
	int	vb;
{
	long		sz;
	int		n;
	FILE		*file;
	static char	*buf = 0;
	static long	bufsize = 0;

	if (vb) fprintf(stderr, "Reading %s\n", fn);

	sz = fsize(fn);

	if (sz + 1 > bufsize) {
		bufsize = sz + 1;		/* +1 since we add a \0. */
		if (buf) free(buf);
		buf = mustAlloc((unsigned) bufsize);
	}

	buf[sz] = 0;

	file = fopen(fn, "r");
	if (!file) fatal("Could not read file '%s'.", fn);
	n = fread(buf, sz, 1, file);
	if (n != 1) fatal("Could not read contents of file '%s'.", fn);
	fclose(file);

	return buf;
}

/******************************************************************************
 *
 * :: Utilities
 *
 *****************************************************************************/

char *
mustAlloc(n)
	unsigned n;
{
	char *s = (char *) malloc(n);

	if (!s) fatal("Out of memory.", 0);
	return s;
}

char *
strCopy(s)
	char	*s;
{
	return (char *) strcpy(mustAlloc(strlen(s)+1), s);
}

void
fatal(fmt, s)
	char	*fmt;
	char	*s;
{
	fprintf(stderr, "%s: ", Prog);
	fprintf(stderr, fmt, s);
	fprintf(stderr, "\n");
	exit(3);
}
