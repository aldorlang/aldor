#include "javacode.h"

CREATE_LIST(JavaCode);

local void jco0Indent(JavaCodePContext ctxt);

JavaCode 
jcoNewNode(JavaCodeClass clss, int argc) 
{
	JavaCode jc = (JavaCode) (stoAlloc( (int) OB_JCode, 
					    fullsizeof(struct jcoNode, argc, JavaCode)));
	jcoTag(jc) = JCO_JAVA;
	jcoClass(jc) = clss;
	jcoPos(jc) = sposNone;
	jcoArgc(jc) = argc;
	return jc;
}

JavaCode 
jcoNewToken(JavaCodeClass clss, Symbol sym) 
{
	JavaCode	jco;
	jco = (JavaCode) stoAlloc((int) OB_JCode, sizeof(struct jcoToken));
	jcoTag(jco) = JCO_TOKEN;
	jcoClass(jco) = clss;
	jcoPos(jco) = sposNone;
	jcoArgc(jco) = 1;
	jcoToken(jco) = sym;

	return jco;
}

JavaCode 
jcoNewLiteral(JavaCodeClass clss, String txt) 
{
	JavaCode	jco;
	jco = (JavaCode) stoAlloc((int) OB_JCode, sizeof(struct jcoToken));
	jcoTag(jco) = JCO_LIT;
	jcoClass(jco) = clss;
	jcoPos(jco) = sposNone;
	jcoArgc(jco) = 1;
	jcoLiteral(jco) = txt;

	return jco;
}


JavaCode 
jcoNewImport(JavaCodeClass clss, String pkg, String name, Bool isImported) 
{
	JavaCode	jco;
	jco = (JavaCode) stoAlloc((int) OB_JCode, sizeof(struct jcoImport));
	jcoTag(jco) = JCO_IMPORT;
	jcoClass(jco)     = clss;
	jcoPos(jco)       = sposNone;
	jcoImportPkg(jco) = pkg;
	jcoImportId(jco)  = name;
	jcoImportIsImported(jco) = isImported;
	return jco;
}

JavaCode
jcoNew(JavaCodeClass clss, int argc, ...)
{
	JavaCode jco;
	va_list  argp;

	va_start(argp, argc);
	
	jco = jcoNewV(clss, argc, argp);
	
	va_end(argp);

	return jco;
}

JavaCode 
jcoNewV(JavaCodeClass clss, int argc, va_list argp)
{
	JavaCode jco;
	int i;
	jco = jcoNewNode(clss, argc);
	for (i = 0; i < argc; i++)
		jcoArgv(jco)[i] = (JavaCode) va_arg(argp, CCode);

	va_end(argp);
	
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


/*
 * :: Java code classes 
 */


JavaCodePContext 
jcoPContextNew(OStream stream, Bool closeStream) 
{
	JavaCodePContext ctxt = (JavaCodePContext) stoAlloc(OB_JCode, sizeof(*ctxt));
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
	// FIXME:
	assert(jcoIsImport(c));
	return strHash(jcoImportId(c)) + strHash(jcoImportPkg(c));
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
