#include "javacode.h"
#include "javaobj.h"

static Bool initialised = false;

enum jc_clss_enum {
	JCO_CLSS_START,
	JCO_CLSS_String = JCO_CLSS_START,
	JCO_CLSS_Integer,    
	JCO_CLSS_Keyword,    
	JCO_CLSS_Id,    
	JCO_CLSS_BinOp,      
	JCO_CLSS_CommaSeq,   
	JCO_CLSS_SpaceSeq,   
	JCO_CLSS_NLSeq,   
	JCO_CLSS_Seq,        
	JCO_CLSS_Parens,    
	JCO_CLSS_Braces,    
	JCO_CLSS_SqBrackets,
	JCO_CLSS_ABrackets, 
	JCO_CLSS_ImportedId, 
	JCO_CLSS_ImportedStatic, 
	JCO_CLSS_Class,      
	JCO_CLSS_JavaDoc,    
	JCO_CLSS_Comment,    
	JCO_CLSS_Method,    
	JCO_CLSS_Declaration,    
	JCO_CLSS_Statement,    
	
	JCO_CLSS_Assign,
	JCO_CLSS_Apply,

	JCO_CLSSS_END
};

typedef Enum(jc_clss_enum) JcClassId;
			   
local JWriteFn jcBinOpPrint; 
local JWriteFn jcClassPrint;
local JWriteFn jcCommentPrint;
local JWriteFn jcDeclarationPrint;
local JWriteFn jcIdPrint;
local JWriteFn jcImportPrint;
local JWriteFn jcIntegerPrint;
local JWriteFn jcJavaDocPrint;
local JWriteFn jcKeywordPrint;
local JWriteFn jcMethodPrint;
local JWriteFn jcParenPrint;
local JWriteFn jcSequencePrint;
local JWriteFn jcStatementPrint;
local JWriteFn jcStringPrint;
local JWriteFn jcApplyPrint;

local JSExprFn jcCommentSExpr;
local JSExprFn jcIdSExpr;
local JSExprFn jcImportedIdSExpr;
local JSExprFn jcImportSExpr;
local JSExprFn jcIntegerSExpr;
local JSExprFn jcKeywordSExpr;
local JSExprFn jcNodeSExpr;
local JSExprFn jcStringSExpr;

local JavaCodeList jc0CreateModifiers(int modifiers);
local Bool jco0NeedsParens(JavaCodeClass c1, JavaCodeClass c2);
local String jc0EscapeString(String s);

static struct jclss jcClss[] = {
	{ JCO_CLSS_String,     jcStringPrint,  jcStringSExpr,  "string",  20 },
	{ JCO_CLSS_Integer,    jcIntegerPrint, jcIntegerSExpr, "integer", 20 },
	{ JCO_CLSS_Keyword,    jcKeywordPrint, jcKeywordSExpr, "keyword", 20 },
	{ JCO_CLSS_Id,         jcIdPrint,      jcIdSExpr,      "id", 20 },
	{ JCO_CLSS_BinOp,      jcBinOpPrint,   jcNodeSExpr,    "binop", 0 },
	{ JCO_CLSS_CommaSeq,   jcSequencePrint,jcNodeSExpr,    "commaseq", 0, ","},
	{ JCO_CLSS_SpaceSeq,   jcSequencePrint,jcNodeSExpr,    "spaceseq", 0, " " },
	{ JCO_CLSS_NLSeq,      jcSequencePrint,jcNodeSExpr,    "nlseq", 0, "\n" },
	{ JCO_CLSS_Seq,        jcSequencePrint,jcNodeSExpr,    "seq", 0, "" },
	{ JCO_CLSS_Parens,     jcParenPrint,  jcNodeSExpr,    "paren", 0, "()" },
	{ JCO_CLSS_Braces,     jcParenPrint,  jcNodeSExpr,    "braces", 0, "{}" },
	{ JCO_CLSS_SqBrackets, jcParenPrint,  jcNodeSExpr,    "sqbracket", 0, "[]" },
	{ JCO_CLSS_ABrackets,  jcParenPrint,  jcNodeSExpr,    "angle", 0, "<>" },
	{ JCO_CLSS_ImportedId, jcImportPrint,  jcImportSExpr,  "importid", 0},
	{ JCO_CLSS_ImportedStatic,
	                       jcImportPrint,  jcImportSExpr,  "static-importid", 1},
	{ JCO_CLSS_Class,      jcClassPrint,   jcNodeSExpr,    "class", 0},
	{ JCO_CLSS_JavaDoc,    jcJavaDocPrint, jcCommentSExpr, "javadoc", 0},
	{ JCO_CLSS_Comment,    jcCommentPrint, jcCommentSExpr, "comment", 0},
	{ JCO_CLSS_Method,     jcMethodPrint,  jcNodeSExpr,  "method", 0},
	{ JCO_CLSS_Declaration,jcDeclarationPrint, jcNodeSExpr, "declaration", 0},
	{ JCO_CLSS_Statement,  jcStatementPrint, jcNodeSExpr, "statement", 0},

	{ JCO_CLSS_Assign,     jcBinOpPrint, jcNodeSExpr, "assign", 5, " = "},
	{ JCO_CLSS_Apply,      jcApplyPrint, jcNodeSExpr, "apply", 10},
};

local JavaCodeClass jc0ClassObj(JcClassId);

JavaCode 
jcClass(int modifiers, String comment, 
	 JavaCode id, JavaCode superclass,
	 JavaCodeList extendList, JavaCodeList body)
{
	JavaCodeList jcmods = jc0CreateModifiers(modifiers);
	
	JavaCode clss = jcoNew(jc0ClassObj(JCO_CLSS_Class),
			       5,
			       jcSpaceSeq(jcmods),
			       id, superclass, 
			       jcCommaSeq(extendList),
			       jcNLSeq(body));
	if (comment == NULL) 
		return clss;

	return jcDocumented(comment, clss);
}

void 
jcClassPrint(JavaCodePContext ctxt, JavaCode clss)
{
	JavaCode modifiers = jcoArgv(clss)[0];
	JavaCode id         = jcoArgv(clss)[1];
	JavaCode superclass = jcoArgv(clss)[2];
	JavaCode implList   = jcoArgv(clss)[3];
	JavaCode body       = jcoArgv(clss)[4];

	if (modifiers != NULL && jcoArgc(modifiers) > 0) {
		jcoWrite(ctxt, modifiers);
		jcoPContextWrite(ctxt, " ");
	}
	jcoPContextWrite(ctxt, "class ");
	jcoWrite(ctxt, id);
	if (superclass != NULL) {
		jcoPContextWrite(ctxt, " extends ");
		jcoWrite(ctxt, implList);
	}
	jcoPContextWrite(ctxt, " {");
	jcoPContextNewlineIndent(ctxt);
	jcoWrite(ctxt, body);
	jcoPContextNewlineUnindent(ctxt);
	jcoPContextWrite(ctxt, "}");
}

/*
 * :: Methods (actually, could be 'member' instead)
 */
JavaCode 
jcMethod(int modifiers, String comment, 
			 JavaCode retnType,
			 JavaCode id, JavaCodeList genArgs,
			 JavaCodeList args,
			 JavaCodeList exns, JavaCode body)
{
	JavaCodeList jcmods = jc0CreateModifiers(modifiers);
	
	JavaCode meth = jcoNew(jc0ClassObj(JCO_CLSS_Method),
			       2,
			       jcDeclaration(modifiers, retnType, 
					     id, listNil(JavaCode), 
					     jcParens(jcCommaSeq(args)), exns),
			       body);
}

local void 
jcMethodPrint(JavaCodePContext ctxt, JavaCode code)
{
	JavaCode decl = jcoArgv(code)[0];
	JavaCode body = jcoArgv(code)[1];
	
	jcoWrite(ctxt, decl);
	jcoPContextWrite(ctxt, " {");
	jcoPContextNewlineIndent(ctxt);
	jcoWrite(ctxt, body);
	jcoPContextNewlineUnindent(ctxt);
	jcoPContextWrite(ctxt, "}");
	jcoPContextNewline(ctxt);
}


/*
 * :: Declarations
 */

JavaCode 
jcDeclaration(int modifiers, 
	      JavaCode retnType,
	      JavaCode id, JavaCodeList genArgs,
	      JavaCode args,
	      JavaCodeList exns)
{
	JavaCodeList l      = listNil(JavaCode);
	JavaCodeList jcmods = jc0CreateModifiers(modifiers);

	l = listCons(JavaCode)(jcSpaceSeq(jcmods), l);
	l = listCons(JavaCode)(retnType, l);
	l = listCons(JavaCode)(id, l);
	if (args != NULL) {
		l = listCons(JavaCode)(args, l);
		l = listCons(JavaCode)(jcCommaSeq(exns), l);
	}
	l = listNReverse(JavaCode)(l);
	return jcoNewFrList(jc0ClassObj(JCO_CLSS_Declaration), l);
}

JavaCode 
jcParamDecl(int modifiers, 
	    JavaCode type,
	      JavaCode id)
{
	return jcDeclaration(modifiers, type, id, 0, 0, 0);
}

local void 
jcDeclarationPrint(JavaCodePContext ctxt, JavaCode code)
{
	JavaCode mods = jcoArgv(code)[0];
	JavaCode retn = jcoArgv(code)[1];
	JavaCode name = jcoArgv(code)[2];
	
	if (jcoArgc(mods) > 0) {
		jcoWrite(ctxt, mods);
		jcoPContextWrite(ctxt, " ");
	}
	if (jcoArgc(retn) > 0) {
		jcoWrite(ctxt, retn);
		jcoPContextWrite(ctxt, " ");
	}
	jcoWrite(ctxt, name);
	if (jcoArgc(code) > 3) {
		JavaCode args = jcoArgv(code)[3];
		JavaCode exns = jcoArgv(code)[4];
		jcoWrite(ctxt, args);
		if (jcoArgc(exns) > 0) {
			jcoPContextWrite(ctxt, " throws ");
			jcoWrite(ctxt, exns);
		}

	}

}

/*
 * :: Function application
 */

extern JavaCode 
jcApply(JavaCode c, JavaCodeList args)
{
	return jcoNew(jc0ClassObj(JCO_CLSS_Apply), 2, c, jcParens(jcCommaSeq(args)));
}

local void
jcApplyPrint(JavaCodePContext ctxt, JavaCode code)
{
	jcoWrite(ctxt, jcoArgv(code)[0]);
	jcoWrite(ctxt, jcoArgv(code)[1]);
}


/*
 * :: Parens
 */
JavaCode
jcParens(JavaCode args) 
{
	JavaCode jco = jcoNew(jc0ClassObj(JCO_CLSS_Parens), 1, args);
	return jco;
}

JavaCode
jcBraces(JavaCode args) 
{
	JavaCode jco = jcoNew(jc0ClassObj(JCO_CLSS_Parens), 1, args);
	return jco;
}

JavaCode
jcSqBrackets(JavaCode args) 
{
	JavaCode jco = jcoNew(jc0ClassObj(JCO_CLSS_SqBrackets), 1, args);
	return jco;
}

JavaCode
jcABrackets(JavaCode args) 
{
	JavaCode jco = jcoNew(jc0ClassObj(JCO_CLSS_ABrackets), 1, args);
	return jco;
}

local void 
jcParenPrint(JavaCodePContext ctxt, JavaCode code)
{
	char s[2] = " ";
	String txt = jcoClass(code)->txt;
	s[0]=txt[0];
	jcoPContextWrite(ctxt, s);
	jcoWrite(ctxt, jcoArgv(code)[0]);
	s[0]=txt[1];
	jcoPContextWrite(ctxt, s);
}


/*
 * :: Comments
 */
JavaCode
jcDocumented(String comment, JavaCode code) 
{
	JavaCode doc = jcoNewLiteral(jc0ClassObj(JCO_CLSS_JavaDoc), comment);
	return jcNLSeqV(2, doc, code);
}

JavaCode
jcComment(String comment) 
{
	JavaCode jc = jcoNewLiteral(jc0ClassObj(JCO_CLSS_Comment), comment);
	return jc;
}

local void
jcJavaDocPrint(JavaCodePContext ctxt, JavaCode code)
{
	String s = strReplace(jcoLiteral(code), "\n", "\n *");
	jcoPContextWrite(ctxt, "/** ");
	jcoPContextWrite(ctxt, s);
	jcoPContextWrite(ctxt, "*/");
	strFree(s);
}


local void
jcCommentPrint(JavaCodePContext ctxt, JavaCode code)
{
	String s = strReplace(jcoLiteral(code), "\n", "\n *");
	jcoPContextWrite(ctxt, "/* ");
	jcoPContextWrite(ctxt, s);
	jcoPContextWrite(ctxt, "*/");
	strFree(s);
}

local SExpr 
jcCommentSExpr(JavaCode code)
{
	SExpr  h = sxiFrSymbol(symIntern(jcoClass(code)->name));
	String s = jc0EscapeString(jcoLiteral(code));
	SExpr sx = sxiFrString(s);
	strFree(s);
	return sxiList(2, h, sx);
}

/*
 * :: Imports
 */

JavaCode
jcImportedId(String pkg, String name)
{
	return jcoNewImport(jc0ClassObj(JCO_CLSS_ImportedId), pkg, name, false);
}

JavaCode
jcImportedStaticId(String pkg, String name)
{
	return jcoNewImport(jc0ClassObj(JCO_CLSS_ImportedStatic), pkg, name, false);
}

local void 
jcImportPrint(JavaCodePContext ctxt, JavaCode code)
{
	if (jcoImportIsImported(code))
		jcoPContextWrite(ctxt, jcoImportId(code));
	else {
		jcoPContextWrite(ctxt, jcoImportPkg(code));
		jcoPContextWrite(ctxt, ".");
		jcoPContextWrite(ctxt, jcoImportId(code));
	}
}

local SExpr 
jcImportSExpr(JavaCode code)
{
	SExpr sym = sxiFrSymbol(symIntern(jcoClass(code)->name));
	return sxiList(3, sym, 
		       sxiFrString(jcoImportPkg(code)),
		       sxiFrString(jcoImportId(code)));
}

/*
 * :: String literals
 */


JavaCode 
jcLiteralString(String s)
{
	return jcoNewLiteral(JCO_CLSS_String, s);
}

local void
jcStringPrint(JavaCodePContext ctxt, JavaCode code)
{
	jcoPContextWrite(ctxt, "\"");
	jcoPContextWrite(ctxt, jcoLiteral(code));
	jcoPContextWrite(ctxt, "\"");
}

local SExpr 
jcStringSExpr(JavaCode code)
{
	String s  = jcoLiteral(code);
	SExpr  sx;
	s = jc0EscapeString(s);
	sx = sxiFrString(s);
	strFree(s);
	return sx;
}

/*
 * :: Integer literals
 */


JavaCode 
jcLiteralInteger(AInt i)
{
	String s = strPrintf("%d", i);
	return jcoNewLiteral(jc0ClassObj(JCO_CLSS_Integer), s);
}

void
jcIntegerPrint(JavaCodePContext ctxt, JavaCode code)
{
	jcoPContextWrite(ctxt, jcoLiteral(code));
}

local SExpr 
jcIntegerSExpr(JavaCode code)
{
	int i = atoi(jcoLiteral(code));
	return sxiFrInteger(i);
}

/*
 * :: Keywords
 */


JavaCode 
jcKeyword(Symbol sym) 
{
	return jcoNewToken(jc0ClassObj(JCO_CLSS_Keyword), sym);
}


JavaCode 
jcNull(String name) 
{
	return jcKeyword(symIntern("null"));
}

local SExpr 
jcKeywordSExpr(JavaCode code)
{
	return sxiFrSymbol(jcoToken(code));
}

local void
jcKeywordPrint(JavaCodePContext ctxt, JavaCode code)
{
	jcoPContextWrite(ctxt, symString(jcoToken(code)));
}

/*
 * :: Ids
 */

JavaCode 
jcId(String name) 
{
	return jcoNewLiteral(jc0ClassObj(JCO_CLSS_Id), name);
}

local SExpr 
jcIdSExpr(JavaCode code)
{
	return sxiFrString(jcoLiteral(code));
}

void 
jcIdPrint(JavaCodePContext ctxt, JavaCode code) 
{
	String name = jcoLiteral(code);
	jcoPContextWrite(ctxt, name);
}

/*
 * :: Binary operations
 */

JavaCode
jcAssign(JavaCode lhs, JavaCode rhs)
{
	return jcBinaryOp(jc0ClassObj(JCO_CLSS_Assign), lhs, rhs);
}

JavaCode 
jcBinaryOp(JavaCodeClass c, JavaCode lhs, JavaCode rhs)
{
	JavaCode r = jcoNew(c, 2, lhs, rhs);
	return r;
}

local void
jcBinOpPrint(JavaCodePContext ctxt, JavaCode code) 
{
	JavaCodeClass thisClss = jcoClass(code);
	JavaCode lhs = jcoArgv(code)[0];
	JavaCode rhs = jcoArgv(code)[1];
	
	if (jco0NeedsParens(thisClss, jcoClass(lhs))) {
		jcoPContextWrite(ctxt, "(");
		jcoWrite(ctxt, lhs);
		jcoPContextWrite(ctxt, ")");
	}
	else {
		jcoWrite(ctxt, lhs);
	}

	jcoPContextWrite(ctxt, thisClss->txt);
	if (jco0NeedsParens(thisClss, jcoClass(rhs))) {
		jcoPContextWrite(ctxt, "(");
		jcoWrite(ctxt, rhs);
		jcoPContextWrite(ctxt, ")");
	}
	else {
		jcoWrite(ctxt, rhs);
	}

}

local Bool
jco0NeedsParens(JavaCodeClass c1, JavaCodeClass c2) 
{
	return c1->val > c2->val;
}

/*
 * :: Statements
 */
JavaCode
jcStatement(JavaCode stmt) 
{
	return jcoNew(jc0ClassObj(JCO_CLSS_Statement), 1, stmt);
}

local void
jcStatementPrint(JavaCodePContext ctxt, JavaCode code)
{
	jcoWrite(ctxt, jcoArgv(code)[0]);
	jcoPContextWrite(ctxt, ";");
}

/*
 * :: Sequences
 */

JavaCode
jcCommaSeq(JavaCodeList lst) 
{
	return jcoNewFrList(jc0ClassObj(JCO_CLSS_CommaSeq), lst);
}

JavaCode
jcNLSeq(JavaCodeList lst) 
{
	return jcoNewFrList(jc0ClassObj(JCO_CLSS_NLSeq), lst);
}

JavaCode
jcSpaceSeq(JavaCodeList lst) 
{
	return jcoNewFrList(jc0ClassObj(JCO_CLSS_SpaceSeq), lst);
}

JavaCode
jcSpaceSeqV(int n, ...) 
{
	va_list argp;

	va_start(argp, n);
	JavaCode jc = jcoNewV(jc0ClassObj(JCO_CLSS_SpaceSeq), n, argp);
	va_end(argp);
	return jc;
}

JavaCode
jcNLSeqV(int n, ...) 
{
	va_list argp;

	va_start(argp, n);
	JavaCode jc = jcoNewV(jc0ClassObj(JCO_CLSS_NLSeq), n, argp);
	va_end(argp);
	return jc;
}

local void 
jcSequencePrint(JavaCodePContext ctxt, JavaCode code)
{
	char *theSep = jcoClass(code)->txt;
	char *sep = 0;
	int i=0;
	for (i=0; i<jcoArgc(code); i++) {
		if (sep != 0)
			jcoPContextWrite(ctxt, sep);
		jcoWrite(ctxt, jcoArgv(code)[i]);
		sep = theSep;
	}
	if (jcoClass(code)->val & 1)
		jcoPContextWrite(ctxt, sep);
}

/*
 * :: File
 */

JavaCode
jcFile(JavaCode pkg, JavaCode name, JavaCodeList imports, JavaCode body)
{
	// FIXME: Very temporary
	listNConcat(JavaCode)(imports, listSingleton(JavaCode)(body));
	return jcNLSeq(imports);
}


/*
 * :: Generic operations
 */
local SExpr 
jcNodeSExpr(JavaCode code) 
{
	Symbol sym = symIntern(jcoClass(code)->name);
	SExpr whole = sxiList(1, sxiFrSymbol(sym));
	int i=0;

	for (i=0; i<jcoArgc(code); i++) {
		JavaCode jc = jcoArgv(code)[i];
		SExpr sexpr = jc == NULL ?  sxNil : jcoSExpr(jc);
		whole = sxCons(sexpr, whole);
	}
	return sxNReverse(whole);
}


void 
jcListPrint(JavaCodePContext ctxt, JavaCode code) 
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
 * :: Utils
 */
local Symbol jc0ModifierSymbol(int idx);

local JavaCodeClass 
jc0ClassObj(JcClassId id)
{
	JavaCodeClass clss = &jcClss[id];
	assert(clss->id == id);
	return clss;
}

struct jcModifierInfo {
	int mask;
	String txt;
	Symbol sym;
};


static struct jcModifierInfo jcModifierList[] = {
	{ JCO_MOD_Public,    "public"},
	{ JCO_MOD_Private,   "private"},
	{ JCO_MOD_Protected, "protected"},
	{ JCO_MOD_Static,    "static"},
	{ JCO_MOD_Final,     "final"},
	{ JCO_MOD_Transient, "transient"},
	{ JCO_MOD_Volatile,  "volatile"},
};

local JavaCodeList 
jc0CreateModifiers(int modifiers)
{
	JavaCodeList l = listNil(JavaCode);
	int i, m;
	for (m=1; m< JCO_MOD_MAX; m=m<<1) {
		if (modifiers & i) 
			l = listCons(JavaCode)(jcoNewToken(jc0ClassObj(JCO_CLSS_Keyword),
							   jc0ModifierSymbol(i)), l);
	}
	return listNReverse(JavaCode)(l);
}

local Symbol
jc0ModifierSymbol(int idx) 
{
	struct jcModifierInfo *inf = &jcModifierList[idx];
	if (inf->sym == NULL)
		inf->sym = symInternConst(inf->txt);
	return inf->sym;
}


local Bool jc0ImportEq(JavaCode c1, JavaCode c2);
local void jc0CollectImports(Table tbl, JavaCode code);

JavaCodeList 
jcCollectImports(JavaCode code)
{
	Table tbl = tblNew((TblHashFun) jcoHash, (TblEqFun) jc0ImportEq);
	TableIterator it;
	JavaCodeList resList = listNil(JavaCode);

	jc0CollectImports(tbl, code);

	for (tblITER(it, tbl); tblMORE(it); tblSTEP(it)) {
		JavaCode id = (JavaCode) tblKEY(it);
		JavaCodeList codes = (JavaCodeList) tblELT(it);
		JavaCodeList tmp;
		JavaCode cp = jcImportedId(jcoImportPkg(id), jcoImportId(id));
		resList = listCons(JavaCode)(cp, resList);
		tmp = codes;
		while (tmp != 0) {
			JavaCode imp = car(tmp);
			jcoImportIsImported(imp) = 1;
			tmp = cdr(tmp);
		}
		listFree(JavaCode)(codes);
	}
	return resList;
}

local Bool
jc0ImportEq(JavaCode c1, JavaCode c2)
{
	assert(jcoIsImport(c1) && jcoIsImport(c2));
	
	if (strcmp(jcoImportPkg(c1), jcoImportPkg(c2)) != 0)
		return false;
	if (strcmp(jcoImportId(c1), jcoImportId(c2)) != 0)
		return false;
	
	return true;
}


local void 
jc0CollectImports(Table tbl, JavaCode code) 
{
	if (code == 0)
		return;
	if (jcoIsImport(code)) {
		JavaCodeList l = (JavaCodeList) tblElt(tbl, code, listNil(JavaCode));
		JavaCode key = code;
		l = listCons(JavaCode)(code, l);
		tblSetElt(tbl, key, l);
	}
	if (jcoIsNode(code)) {
		int i=0;
		for (i=0; i<jcoArgc(code); i++) {
			jc0CollectImports(tbl, jcoArgv(code)[i]);
		}
	}
}

/*
 * Returns a newly allocated string with some substitutions to make it clearer.
 */
local String 
jc0EscapeString(String s)
{
	s = strReplace(s, "\n", "\\n");
	s = strNReplace(s, "\"", "\\\"");
	return s;
}
