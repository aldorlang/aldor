#include "javacode.h"
#include "store.h"
#include "util.h"
#include "sexpr.h"

CREATE_LIST(JavaCode);

local void jco0Indent(JavaCodePContext ctxt);

local JavaCode jcoAlloc(int sz);

local JavaCode jcoAlloc(int sz)
{
	JavaCode jco = (JavaCode) (stoAlloc(OB_JCode, sz));

	return jco;
}


JavaCode 
jcoNewNode(JavaCodeClass clss, int argc) 
{
	JavaCode jco = jcoAlloc(fullsizeof(struct jcoNode, argc, JavaCode));
	assert(clss);

	jcoTag(jco) = JCO_JAVA;
	jcoClass(jco) = clss;
	jcoPos(jco) = sposNone;
	jco->node.argc = argc;

	return jco;
}

JavaCode 
jcoNewToken(JavaCodeClass clss, Symbol sym) 
{
	JavaCode	jco;

	assert(clss && sym);

	jco = jcoAlloc(sizeof(struct jcoToken));
	jcoTag(jco) = JCO_TOKEN;
	jcoClass(jco) = clss;
	jcoPos(jco) = sposNone;
	jco->token.symbol = sym;

	return jco;
}

JavaCode 
jcoNewLiteral(JavaCodeClass clss, String txt) 
{
	JavaCode	jco;
	assert(clss && txt);
	jco = jcoAlloc(sizeof(struct jcoLiteral));

	jcoTag(jco) = JCO_LIT;
	jcoClass(jco) = clss;
	jcoPos(jco) = sposNone;

	jco->literal.txt = txt;

	return jco;
}


JavaCode
jcoNewImport(JavaCodeClass clss, String pkg, String name, Bool isImported) 
{
	JavaCode	jco;
	assert(pkg != NULL);
	assert(name != NULL);

	jco = jcoAlloc(sizeof(struct jcoImport));
	assert(clss && pkg && name);

	jcoTag(jco) = JCO_IMPORT;
	jcoClass(jco)     = clss;
	jcoPos(jco)       = sposNone;
	jco->import.pkg = pkg;
	jco->import.id = name;
	jcoImportSetImported(jco, isImported);

	return jco;
}

JavaCode
jcoNew(JavaCodeClass clss, int argc, ...)
{
	JavaCode jco;
	va_list  argp;

	va_start(argp, argc);
	
	jco = jcoNewP(clss, argc, argp);
	
	va_end(argp);

	return jco;
}

JavaCode 
jcoNewP(JavaCodeClass clss, int argc, va_list argp)
{
	JavaCode jco;
	int i;

	jco = jcoNewNode(clss, argc);
	for (i = 0; i < argc; i++)
		jcoArgv(jco)[i] = (JavaCode) va_arg(argp, JavaCode);

	return jco;
}


JavaCode
jcoNewFrList(JavaCodeClass clss, JavaCodeList lst) 
{
	JavaCode ret = jcoNewNode(clss, listLength(JavaCode)(lst));
	List(JavaCode) tmp = lst;
	int i=0;
	while (tmp != listNil(JavaCode)) {
		jcoArgv(ret)[i] = car(tmp);
		tmp = cdr(tmp);
		i++;
	}
	listFree(JavaCode)(lst);
	return ret;
}

void 
jcoFree(JavaCode code) 
{
	if (code == NULL)
		return;

	if (jcoIsLiteral(code))
		strFree(jcoLiteral(code));
	if (jcoIsNode(code)) {
		int i;
		for (i=0; i<jcoArgc(code); i++) 
			jcoFree(jcoArgv(code)[i]);
	}
	stoFree(code);
}

JavaCode 
jcoCopy(JavaCode code)
{
	if (code == 0)
		return 0;
	if (jcoIsLiteral(code))
		return jcoNewLiteral(jcoClass(code), strCopy(jcoLiteral(code)));
	if (jcoIsToken(code))
		return jcoNewToken(jcoClass(code), jcoToken(code));
	if (jcoIsImport(code))
		return jcoNewImport(jcoClass(code), 
				    jcoImportPkg(code), jcoImportId(code),
				    jcoImportIsImported(code));
	if (jcoIsNode(code)) {
		JavaCodeList l = listNil(JavaCode);
		int i=0;
		for (i=0; i<jcoArgc(code); i++) {
			l = listCons(JavaCode)(jcoCopy(jcoArgv(code)[i]), l);
		}
		l = listNReverse(JavaCode)(l);
		return jcoNewFrList(jcoClass(code), l);
	}
	assert(false);
	return NULL;
}

/*
 * :: Basic access
 */

extern int
jcoArgc(JavaCode jco)
{
	assert(jcoIsNode(jco));
	return jco->node.argc;
}

extern JavaCode *
jcoArgv(JavaCode jco)
{
	assert(jcoIsNode(jco));
	return jco->node.argv;
}

extern Symbol
jcoToken(JavaCode jco)
{
	assert(jcoIsToken(jco));
	return jco->token.symbol;
}

extern String
jcoLiteral(JavaCode jco)
{
	assert(jcoIsLiteral(jco));
	return jco->literal.txt;
}

extern String
jcoImportPkg(JavaCode jco)
{
	assert(jcoIsImport(jco));
	return jco->import.pkg;
}

extern String
jcoImportId(JavaCode jco)
{
	assert(jcoIsImport(jco));
	return jco->import.id;
}

extern Bool
jcoImportIsImported(JavaCode jco)
{
	assert(jcoIsImport(jco));
	return jco->import.isImported;
}

extern void
jcoImportSetImported(JavaCode jco, Bool flg)
{
	assert(jcoIsImport(jco));
	jco->import.isImported = flg;
}

extern Bool
jcoIsEmpty(JavaCode jco)
{
	return jcoIsNode(jco) && jcoArgc(jco) == 0;
}

/*
 * :: Java code classes 
 */


JavaCodePContext 
jcoPContextNew(OStream stream, Bool closeStream) 
{
	JavaCodePContext ctxt = (JavaCodePContext) stoAlloc(OB_JCode, sizeof(*ctxt));
	ctxt->cpos = 0;
	ctxt->line = 0;
	ctxt->indent = 0;
	ctxt->stream = stream;
	ctxt->closeStream = closeStream;
	
	return ctxt;
}

void 
jcoPContextFree(JavaCodePContext ctxt)
{
	if (ctxt->line > 0 && ctxt->cpos ==0)
		ostreamWrite(ctxt->stream, "\n", 1);

	if (ctxt->closeStream) {
		ostreamClose(ctxt->stream);
		ostreamFree(ctxt->stream);
	}

	stoFree(ctxt);
}

void 
jcoPContextWrite(JavaCodePContext ctxt, String s)
{
	String p;
	while (*s != '\0') {
		if (ctxt->cpos == 0)
			jco0Indent(ctxt);
		p = strchr(s, '\n');
		if (p == 0) {
			ostreamWrite(ctxt->stream, s, -1);
			break;
		}
		if (p != s)
			ostreamWrite(ctxt->stream, s, p-s);
		jcoPContextNewline(ctxt);
		s = p+1;
	}
}


void 
jcoPContextNewline(JavaCodePContext ctxt) 
{
	ostreamWrite(ctxt->stream, "\n", 1);
	ctxt->line++;
	ctxt->cpos = 0;
}


void 
jco0Indent(JavaCodePContext ctxt) 
{
	int i;
	for (i=0; i<ctxt->indent; i++)
		ostreamWrite(ctxt->stream, " ", 1);
	ctxt->cpos = ctxt->indent;
}


void 
jcoPContextNewlineIndent(JavaCodePContext ctxt)
{
	ctxt->indent += 4;
	jcoPContextNewline(ctxt);
}

void 
jcoPContextNewlineUnindent(JavaCodePContext ctxt)
{
	ctxt->indent -= 4;
	jcoPContextNewline(ctxt);
}


void 
jcoWrite(JavaCodePContext ctxt, JavaCode code) 
{
	jcoClass(code)->writer(ctxt, code);
}

SExpr 
jcoSExpr(JavaCode code)
{
  return jcoClass(code)->sexpr(code);
}

Hash
jcoHash(JavaCode c)
{
	/* FIXME: */
	if (c == 0)
		return 20041;
	if (jcoIsImport(c))
		return strHash(jcoImportId(c)) + strHash(jcoImportPkg(c));
	if (jcoIsToken(c))
		return symHash(jcoToken(c));
	if (jcoIsImport(c))
		return hashCombine(strHash(jcoImportPkg(c)), strHash(jcoImportId(c)));
	if (jcoIsLiteral(c))
		return strHash(jcoLiteral(c));
	if (jcoIsNode(c)) {
		Hash h = jcoClass(c)->id;
		int i;
		for (i=0; i < jcoArgc(c); i++)
			h = hashCombine(h, jcoHash(jcoArgv(c)[i]));
		return h;
	}
	assert(false);
	return 0;
}

Bool 
jcoEqual(JavaCode c1, JavaCode c2)
{
	if ( (c1 == 0) != (c2 == 0))
		return false;
	if (c1 == 0)
		return true;
	if (jcoTag(c1) != jcoTag(c2))
		return false;
	if (jcoClass(c1) != jcoClass(c2))
		return false;

	if (jcoIsImport(c1))
		return strcmp(jcoImportPkg(c1), jcoImportPkg(c2)) == 0
			&& strcmp(jcoImportId(c1), jcoImportId(c2)) == 0;
	if (jcoIsToken(c1))
		return strcmp(symString(jcoToken(c1)), 
			      symString(jcoToken(c2))) == 0;
	if (jcoIsNode(c1)) {
		int i;
		if (jcoArgc(c1) != jcoArgc(c2))
			return false;
		for (i=0; i < jcoArgc(c1); i++) {
			if (!jcoEqual(jcoArgv(c1)[i], jcoArgv(c2)[i]))
				return false;
		}
		return true;
	}
	return false;
}

SExpr 
jcoNodeSExpr(JavaCodePContext ctxt, JavaCode code) 
{
	SExpr head = sxiFrString(jcoClass(code)->txt);
	SExpr whole = sxiList(1, head);
	int i;
	for (i=0; i<jcoArgc(code); i++) {
		whole = sxCons(jcoSExpr(jcoArgv(code)[i]), whole);
	}
	return sxNReverse(whole);
}

void 
jcoSimplePrint(JavaCodePContext ctxt, JavaCode code) 
{
	String name = jcoClass(code)->txt;
	jcoPContextWrite(ctxt, name);
}

local void 
jcoListPrint(JavaCodePContext ctxt, JavaCode code) 
{
	char *theSep = (char *) jcoClass(code)->txt;
	char *sep = "";
	int i;
	for (i=0; i<jcoArgc(code); i++) {
		jcoPContextWrite(ctxt, sep);
		jcoWrite(ctxt, jcoArgv(code)[i]);
		sep = theSep;
	}
}



/*
 * :: Debug
 */

void
jcoPrint(FILE *f, JavaCode c)
{
	SExpr sx = jcoSExpr(c);
	sxiWrite(f, sx, SXRW_MixedCase);
}

void jcoPrintDb(JavaCode c)
{
	jcoPrint(stdout, c);
}
