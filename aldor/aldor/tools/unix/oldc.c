/*
 * oldc [-va "id"] < infile > outfile
 *
 * This program converts ANSI-style function headers to K+R old C style.
 * If the option -va is given, then a "..." indicator in functions
 * with a variable number of artuments is  replaced with the identifier "id",
 * declared to be int.
 *
 *	f(int a)     becomes   f(a)int a;
 *	{		       {
 *	}		       }
 *
 *	g(int a,...) becomes   g(a)int a;  or  g(a,id) int a; int id;
 *	{                      {               {
 *      }                      }               }
 *
 *
 * SMW Dec/91
 * SMW Aug/92 -- added -va
 * SMW Sep/01 -- added includes to get sizes right on Alpha.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ccode.h"

#define NEW_STYLE 

char	word[100000];
char	*dddotText = 0; /* Normally eliminate "..." in function param lists. */

/******************************************************************************
 *
 * Structure manipulation
 *
 *****************************************************************************/

char *
#ifdef NEW_STYLE
ctreeGetMem(int len)
#else
ctreeGetMem(len)
	int	len;
#endif
{
	char *p = (char *) malloc(len);
	if (!p) { fprintf(stderr, "Out of memory!\n"); exit(3); }
	return p;
}

char *
strCopy(s)
	char * s;
{
	char * t = ctreeGetMem(strlen(s) + 1);
	strcpy(t, s);
	return t;
}

CCode
ccNewToken(kind, neutral, text)
	CTokTag kind;
	char * neutral;
	char * text;
{
	CCode	cc;
	cc = (CCode) ctreeGetMem(sizeof(struct ccodeToken));
	cc->token.hdr.nodeKind = CC_Token;
	cc->token.hdr.opKind   = kind;
	cc->token.neutral      = strCopy(neutral);

	/* Remove suffixes that old compilers can't handle. */
	if (kind == CTOK_IntegerConstant || kind == CTOK_FloatingConstant) {
		int	i = strlen(text) - 1;
		char	c = text[i];
		while (((c=='f'||c=='F') && (kind == CTOK_FloatingConstant))
		       ||c=='l'||c=='L'||c=='u'||c=='U')
			c = text[--i];
		c = text[++i];
		text[i] = '\0';
		cc->token.text = strCopy(text);
		text[i] = c;
	}
	else
		cc->token.text = strCopy(text);

	return cc;	
}

/*
 * Fake N-ary arguments since <stdarg.h> is not universally available and
 * do not want to depend on ../src.
 */
CCode
ccNewNode(n, nodeKind, opKind, a0, a1, a2, a3, a4, a5, a6, a7, a8)
	int      n;
	CCodeTag nodeKind;
	CTokTag  opKind;
	CCode	 a0, a1, a2, a3, a4, a5, a6, a7, a8;
{
	CCode	cc;

	cc = (CCode) ctreeGetMem(sizeof(*cc)+(n-NARY)*sizeof(CCode));
	cc->node.hdr.nodeKind = nodeKind;
	cc->node.hdr.opKind   = opKind;
	cc->node.argc         = n;

	if (n > 9) {
		fprintf(stderr, "Too many arguments to ccNewNode\n");
		exit(1);
	}

	switch (n) {
	case  9: cc->node.argv[8] = a8;
	case  8: cc->node.argv[7] = a7;
	case  7: cc->node.argv[6] = a6;
	case  6: cc->node.argv[5] = a5;
	case  5: cc->node.argv[4] = a4;
	case  4: cc->node.argv[3] = a3;
	case  3: cc->node.argv[2] = a2;
	case  2: cc->node.argv[1] = a1;
	case  1: cc->node.argv[0] = a0;
	}

	return cc;	
}

int
ccPrint(fout, cc)
	FILE *fout;
	CCode cc;
{
	unsigned int	i, nc;
	char	*w;

	if (!cc) return 0;
	if (cc->hdr.nodeKind == CC_Token) {
		w = cc->token.neutral;
		nc = fprintf(fout, "%s%s", w, cc->token.text);
	}
	else
		for (nc = 0, i = 0; i < cc->node.argc; i++)
			nc += ccPrint(fout, cc->node.argv[i]);
	return nc;
		
}


#define ccIs(t,k)	((t) && (t)->hdr.nodeKind == (k))
#define ccIsOp(t,k,o)	(ccIs(t,k) && (t)->hdr.opKind == (o))

int
ccIsTypedef(cc)
	CCode cc;
{
	if (!ccIs(cc, CC_Decl)) return 0;;

	for (cc = cc->node.argv[0]; ccIs(cc,CC_Seq); cc = cc->node.argv[0]) 
		if (ccIsOp(cc->node.argv[1],CC_StoClass,CTOK_Typedef)) return 1;
	return ccIsOp(cc,CC_StoClass,CTOK_Typedef);
}

void
ccAddTypedefName(s)
	char *s;
{
	struct cspecial	*p;
	extern int yydebug;
	p = (struct cspecial *) ctreeGetMem(sizeof(struct cspecial));
	p->kind   = CTOK_TypedefName;
	p->string = s;
	p->next   = 0;
	cscanSpecial(p);
}

void
ccMapDeclees(f, cc)
	void (*f)();
	CCode cc;
{
	/* optlist */
	if (!cc) return;

	if (ccIs(cc, CC_Decl)) {
		ccMapDeclees(f, cc->node.argv[1]);
		return;
	}
	if (ccIsOp(cc, CC_Infix, CTOK_Comma)) {
		ccMapDeclees(f, cc->node.argv[0]);
		ccMapDeclees(f, cc->node.argv[2]);
		return;
	}
	/* InitDeclarator */
	if (ccIsOp(cc, CC_Infix, CTOK_Asst)) {
		ccMapDeclees(f, cc->node.argv[0]);
		return;
	}
	/* Declarator */
	if (ccIs(cc, CC_PtrDecl)) {
		ccMapDeclees(f, cc->node.argv[1]);
		return;
	}
	/* DirectDeclarator */
	if (ccIs(cc, CC_Paren)) {
		ccMapDeclees(f, cc->node.argv[1]);
		return;
	}
	if (ccIs(cc, CC_FCall)) {
		ccMapDeclees(f, cc->node.argv[0]);
		return;
	}
	if (ccIs(cc, CC_ARef)) {
		ccMapDeclees(f, cc->node.argv[0]);
		return;
	}
	/* Identifier */
	if (ccIsOp(cc, CC_Token, CTOK_Identifier)) {
		f(cc->token.text);
		return;
	}
	/* Other */
	return;
}

CCode
ccDoTypedefs(cc)
	CCode	cc;
{
	if (ccIsTypedef(cc)) ccMapDeclees(ccAddTypedefName, cc->node.argv[1]);
	return cc;
}

static CCode	ilist;

void
ccMakeIdList(name)
	char	*name;
{
	CCode	id = ccNewToken(CTOK_Identifier, " ", name);
	if (!ilist) {
		ilist = id;
	}
	else {
		CCode comma = ccNewToken(CTOK_Comma, "", ",");
		ilist = ccNewInfix(CTOK_Comma, ilist, comma, id);
	}
}

CCode
ccOld(cc)
	CCode	cc;
{
	int i;

	if (!cc)
		return 0;
	/*
	 * 1. Remove "const" and "volatile" keywords.
	 */
	if (ccIs(cc, CC_Token)) {
		if (cc->hdr.opKind==CTOK_Const || cc->hdr.opKind==CTOK_Volatile)
			cc->token.text = ""; /*!!free */
		return cc;
	}

	/*
	 * 2. Convert function headers.
	 *    int f(int a) { }  -->  int f(a) int a; { }
	 */
	if (ccIs(cc, CC_FDef)) {
		CCode	hdr = cc->node.argv[1];

		while (hdr && hdr->hdr.nodeKind != CC_FCall) {
			switch (hdr->hdr.nodeKind) {
			  case CC_PtrDecl:	hdr = hdr->node.argv[1]; break;
			  case CC_Paren:	hdr = hdr->node.argv[1]; break;
			  case CC_ARef:	 	hdr = hdr->node.argv[0]; break;
			  default: 	 	goto ForgetIt;
			}
		}
		if (!hdr) goto ForgetIt;

		if (ccIs(hdr->node.argv[2], CC_Param)) {
			CCode	t;
			CCode	plist = hdr->node.argv[2]->node.argv[0];
			CCode	dddot = hdr->node.argv[2]->node.argv[2];
			
			/* If appropriate, add va_alist for "..." param. */
			if (dddotText && dddot) {
				CCode comma= ccNewToken(CTOK_Comma,"",",");
				CCode type = ccNewToken(CTOK_Int, " ", "int");
				CCode id   = ccNewToken(CTOK_Identifier,
						        " ", dddotText);
				CCode decl = ccNewDecl(0, type, id, 0);

				plist = ccNewInfix(CTOK_Comma, plist, comma, decl);
			}
			
			/* Construct comma separated list of parameters. */
			ilist = 0;
			ccMapDeclees(ccMakeIdList, plist);
			hdr->node.argv[2] = ilist;

			/* Modify param sequence in-place to be a sequence. */
			t = plist;
			while (t) {
				CCode t1;
				if (ccIsOp(t, CC_Infix, CTOK_Comma)) {
					t1 = t->node.argv[2];

					t->hdr.nodeKind = CC_Seq;
					t->hdr.opKind   = 0;
					t->node.argc    = 2;
					t->node.argv[2] = 0;
					t->node.argv[1] = t1; /*!!free*/

					t = t->node.argv[0];
				}
				else {
					t1 = t;
					t = 0;
				}

				if (ccIs(t1, CC_Decl)) {
					if (!t1->node.argv[1])
						/* Handle f(void) {} */
						t1->node.argv[0] = 0; /*!!free*/
					else
						t1->node.argv[2] =
						   ccNewToken(CTOK_Semi, "", ";");
				}
			}

			/* Now put the sequence as the old-style declaration. */
			cc ->node.argv[2] = plist;
			/*!!free*/
		}
	}
ForgetIt:
	for (i = 0; i < (int)cc->node.argc; i++) {
		CCode t = ccOld(cc->node.argv[i]);
		if (t != cc->node.argv[i])
			cc->node.argv[i] = t;	/*!!free*/
	}
	/*
	 * 3. Remove argument types from function declarations. 
	 *    E.g.  extern int f(int);  --> extern int f();
	 */
	if (ccIs(cc, CC_FCall)) {
		if (ccIs(cc->node.argv[2], CC_Param))
			cc->node.argv[2] = 0;	/*!!free*/
	}
	return cc;
}

void
ccParseFile()
{
	extern CCode	yytree;

	do {
		if (yyparse() != 0) break;
		yytree = ccOld(yytree);
		ccPrint(stdout, yytree);

	} while (!yytree ||
		yytree->hdr.nodeKind != CC_Token ||
		yytree->hdr.opKind   != CTOK_EOF );
}

main(argc, argv)
	int		argc;
	char		**argv;
{
	if (argc > 2 && !strcmp(argv[1], "-va")) dddotText = argv[2];

	cscanInit(word, sizeof(word), "stdin", stdin, 0);
	ccParseFile();
	exit(0);
}
