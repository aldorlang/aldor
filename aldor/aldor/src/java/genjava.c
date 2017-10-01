#include "comsg.h"
#include "debug.h"
#include "flog.h"
#include "foamsig.h"
#include "intset.h"
#include "javacode.h"
#include "sexpr.h"
#include "store.h"
#include "syme.h"
#include "table.h"
#include "util.h"

/*
 * Implement as the following...
 * Unit: 
 *   Becomes a class
 *   functions: 
 *      static methods within the class
 *   closures:
 *      library class
 *   formats:
 *      ? inner classes

 * Fluids:
 *    supplied as library
 */

/* static function declarations */
local String gj0ClassName(Foam foam, String name);
local String gj0ClassDocumentation(Foam foam, String name);
local JavaCodeList gj0CollectImports(JavaCode clss);
local JavaCodeList gj0DDef(Foam foam);
local JavaCode gj0Gen(Foam foam);
local JavaCode gj0Gen(Foam foam);
local JavaCodeList gj0GenList(Foam *p, AInt n);
local JavaCode gj0Def(Foam foam);
local JavaCode gj0Default(Foam foam, String s);
local JavaCode gj0Prog(Foam lhs, Foam rhs);
local JavaCode gj0ClosInit(Foam lhs, Foam rhs);
local JavaCode gj0Set(Foam lhs, Foam rhs);
local JavaCode gj0Return(Foam foam);
local JavaCode gj0Seq(Foam seq);
local JavaCode gj0Par(Foam seq);
local JavaCode gj0Loc(Foam seq);
local JavaCode gj0LocSet(Foam ref, Foam rhs);
local JavaCode gj0Glo(Foam ref);
local JavaCode gj0GloSet(Foam ref, Foam rhs);
local JavaCode gj0GloRegister(Foam lhs, Foam rhs);
local JavaCode gj0Nil(Foam foam);

local JavaCode gj0Throw(Foam foam);

local JavaCode gj0ValuesSet(Foam foam, Foam rhs);
local JavaCode gj0ValuesReturn(Foam foam);

local JavaCode gj0ANew(Foam foam);
local JavaCode gj0Arr(Foam foam);
local JavaCode gj0ArrChar(Foam foam);
local JavaCode gj0AElt(Foam foam);
local JavaCode gj0AEltSet(Foam foam, Foam rhs);

local JavaCode gj0RNew(Foam foam);
local JavaCode gj0RElt(Foam foam);
local JavaCode gj0REltSet(Foam foam, Foam rhs);
local JavaCode gj0RecElt(JavaCode ref, Foam ddecl, int idx);

local JavaCode gj0Free(Foam foam);
local JavaCode gj0RecSet(JavaCode lhs, JavaCode rhs, Foam ddecl, int idx);

local JavaCode gj0SInt(Foam foam);
local JavaCode gj0HInt(Foam foam);
local JavaCode gj0BInt(Foam foam);
local JavaCode gj0SFlo(Foam foam);
local JavaCode gj0DFlo(Foam foam);
local JavaCode gj0Byte(Foam foam);

local JavaCode gj0Bool(Foam foam);
local JavaCode gj0Char(Foam foam);

local JavaCode gj0Cast(Foam foam);
local JavaCode gj0CastFmt(Foam foam, AInt cfmt);

local JavaCode gj0Env(Foam foam);
local JavaCode gj0Lex(Foam foam);
local JavaCode gj0PushEnv(Foam foam);
local JavaCode gj0LexSet(Foam foam, Foam rhs);
local JavaCode gj0EElt(Foam foam);
local JavaCode gj0EEltSet(Foam foam, Foam rhs);
local JavaCode gj0EEnv(Foam foam);
local JavaCode gj0EInfo(Foam foam);
local JavaCode gj0EInfoSet(Foam foam, Foam rhs);
local JavaCode gj0EEnsure(Foam foam);
local JavaCode gj0EnvId(int lvl);
local JavaCode gj0LvlId(int lvl);

local JavaCode gj0Clos(Foam foam);
local JavaCode gj0CEnv(Foam foam);
local JavaCode gj0CProg(Foam foam);
local JavaCode gj0CEnvSet(Foam foam, Foam rhs);
local JavaCode gj0CProgSet(Foam foam, Foam rhs);

local JavaCode gj0CCall(Foam call);
local JavaCode gj0OCall(Foam call);
local JavaCode gj0BCall(Foam call);
local JavaCode gj0PCall(Foam call);

local JavaCode gj0MFmt(Foam mfmt);
local JavaCode gj0Const(Foam foam);

local JavaCode gj0PRef(Foam foam);
local JavaCode gj0PRefSet(Foam lhs, Foam rhs);

local void     gjInit(Foam foam, String name);
local void     gj0ProgInit(Foam lhs, Foam rhs);
local void     gj0ProgFini(Foam lhs, Foam rhs);
local JavaCodeList gj0ProgCollectArgs(Foam fm);
local JavaCode     gj0ProgGenerateBody(Foam fm);
local JavaCodeList gj0ProgExceptions();
local int          gj0ProgModifiers();
local String       gj0ProgMethodName(Foam var);
local String       gj0ProgFnName(int idx);
local void         gj0ProgAddStubs(FoamSigList sigList);
local JavaCodeList gj0ProgDeclarations(Foam ddecl, Foam body);

local String       gj0Name(CString prefix, Foam fmt, int idx);
local JavaCode     gj0ProgRetnType(Foam rhs);
local JavaCode     gj0TopConst(Foam lhs, Foam rhs);

local JavaCode     gj0Type(Foam decl);
local JavaCode     gj0TypeFrFmt(AInt id, AInt fmt);
local JavaCode     gj0TypeObjToValue(JavaCode val, FoamTag type, AInt fmt);
local JavaCode     gj0TypeValueToObj(JavaCode val, FoamTag type, AInt fmt);
local JavaCode     gj0TypeValueToArr(JavaCode value, AInt fmt);
local JavaCode     gj0TypeValueToRec(JavaCode value, AInt fmt);

local FoamTag      gj0FoamExprType(Foam foam);
local FoamTag      gj0FoamExprTypeWFmt(Foam foam, AInt *fmt);

local FoamSigList  gj0CCallStubAdd(FoamSigList list, FoamSig sig);

local FoamSig      gj0FoamSigFrCCall(Foam ccall);
local JavaCodeList gj0CCallStubGen(FoamSigList sigs);
local FoamSigList  gj0CCallStubAdd(FoamSigList l, FoamSig sig);
local JavaCode     gj0CCallStubGenFrSig(FoamSig sig);
local String       gj0CCallStubName(FoamSig call);

local JavaCode     gj0SeqNode(JavaCodeList l);
local Bool         gj0IsSeqNode(JavaCode jc);

local JavaCode gj0FmtId(AInt ddeclIdx);
local void     gj0FmtUse(AInt ddeclIdx);
local JavaCodeList gj0FmtInits();

local void gj0NameInit();
local String gj0NameFrString(String fmName);

local JavaCodeList gj0ClassHeader(String className);
local String       gj0InitVar(AInt idx);

local JavaCode     gj0TypeFrJavaObj(Foam format);


enum gjId {
	GJ_INVALID = -1,

	GJ_Foam,
	GJ_FoamWord,
	GJ_FoamClos,
	GJ_FoamRecord,
	GJ_FoamEnv,
	GJ_FoamClass,
	GJ_FoamContext,

	GJ_FoamFn,
	GJ_FoamValue,
	GJ_Multi,
	GJ_FoamGlobals,
	GJ_Format,
	GJ_EnvRecord,

	GJ_Object,
	GJ_String,
	GJ_BigInteger,
	GJ_NullPointerException,
	GJ_ClassCastException,
	
	GJ_ContextVar,
	GJ_Main,

	GJ_LIMIT
};

typedef Enum(gjId) GjId;

local JavaCode     gj0Id(GjId id);
#if 0
		   }
#endif

/* Run arguments */
struct gjArgs {
	String name;
	Bool   createMain;
	int    lineWidth;
};

typedef struct gjArgs GjArgs;

/* State variables */
struct gjContext {
	/* whole program */
	Foam formats;
	Foam constants;
	Foam defs;
	/* Mutable state (caches, etc) */
	int  lvl;
	FoamSigList  ccallStubSigList;
	JavaCodeList gloRegList;
	IntSet       fmtSet;
	/* Per prog */
	Foam prog;
	Foam progParams;
	Foam progLocals;
	Foam progLhs;
	FoamSigList progSigList;
	int multVarIdx;
	/* in codegen */
	AInt mfmt;
	AInt afmt;
};

#define gjContextGlobals (gjContext->formats->foamDFmt.argv[globalsSlot])
#define gjContextGlobal(id) (gjContextGlobals->foamDDecl.argv[id])

static struct gjContext gjCtxt0;
static struct gjContext *gjContext = &gjCtxt0;

static struct gjArgs gjArgs0;
static struct gjArgs *gjArgs = &gjArgs0;

/* DEBUG */

Bool	genJavaDebug	= false;
#define gjDEBUG		if (DEBUG(genJava)) fprintf

/* Functions... */

JavaCode 
genJavaUnit(Foam foam, String name)
{
	JavaCodeList imps, code, mainImpl, interfaces, fmts, body;
	JavaCode clss;
	String className, comment;
	
	gjArgs->name = name;
	gjInit(foam, name);

	className = gj0ClassName(foam, name);
	if (!jcIsLegalClassName(className)) {
		comsgFatal(NULL, ALDOR_F_BadJavaFileName, className);
	}
	code = gj0DDef(foam->foamUnit.defs);
	mainImpl = gj0ClassHeader(name);
	comment = gj0ClassDocumentation(foam, name);

	fmts = gj0FmtInits();
	body = listNConcat(JavaCode)(fmts, mainImpl);
	body = listNConcat(JavaCode)(body, code);
	
	interfaces = listSingleton(JavaCode)(gj0Id(GJ_FoamClass));
	clss = jcClass(JCO_MOD_Public, comment, 
		       jcId(className), NULL, interfaces, body);

	imps = gj0CollectImports(clss);
	return jcFile(NULL, jcId(className), imps, clss);
}

/*
 * :: Options
 */

void
gjGenSetMain(Bool flg)
{
	gjArgs->createMain = flg;
}


local JavaCodeList
gj0CollectImports(JavaCode clss)
{
	JavaCodeList imps = jcCollectImports(clss);
	JavaCodeList ids = listNil(JavaCode);
	JavaCodeList tmp = imps;
	while (tmp) {
		JavaCode id = car(tmp);
		JavaCode stmt = jcStatement(jcImport(id));;
		ids = listCons(JavaCode)(stmt, ids);
		tmp = cdr(tmp);
	}
	ids = listNReverse(JavaCode)(ids);
	listFree(JavaCode)(imps);
	
	return ids;
}


local void
gjInit(Foam foam, String name) 
{
	gj0NameInit();

	gjContext->lvl = 0;
	gjContext->formats = foam->foamUnit.formats;
	gjContext->constants = foamUnitConstants(foam);
	gjContext->defs      = foam->foamUnit.defs;
	gjContext->ccallStubSigList = listNil(FoamSig);
	gjContext->gloRegList = listNil(JavaCode);
	gjContext->fmtSet = intSetNew(foamArgc(gjContext->formats));
	gjContext->mfmt = 0;
}

local String 
gj0ClassName(Foam foam, String name)
{
	return strCopy(name);
}

local String 
gj0ClassDocumentation(Foam foam, String name) 
{
	return strPrintf("Generated by genjava - %s\n", name);
}


local JavaCodeList 
gj0DDef(Foam foam)
{
	JavaCodeList lst = listNil(JavaCode);
	JavaCodeList stubs;
	int i;

	for (i=0; i<foamArgc(foam); i++) {
		Foam fm = foam->foamDDef.argv[i];
		JavaCode code = gj0Gen(fm);
		if (code != 0)
			lst = listCons(JavaCode)(code, lst);
	}
	
	lst  = listNReverse(JavaCode)(lst);

	stubs = gj0CCallStubGen(gjContext->ccallStubSigList);
	lst = listNConcat(JavaCode)(lst, stubs);

	return lst;
}

local JavaCode
gj0Gen(Foam foam)
{
	switch (foamTag(foam)) {
	case  FOAM_NOp:
		return jcComment(strCopy("NOp"));
	case  FOAM_Def:
		return gj0Def(foam);
	case FOAM_Set:
		return gj0Set(foam->foamSet.lhs, foam->foamSet.rhs);
	case FOAM_MFmt:
		return gj0MFmt(foam);
	case FOAM_Par:
		return gj0Par(foam);
	case FOAM_Loc:
		return gj0Loc(foam);
	case FOAM_Glo:
		return gj0Glo(foam);
	case FOAM_Seq:
		return gj0Seq(foam);
	case FOAM_Throw:
		return gj0Throw(foam);
	case FOAM_SInt:
		return gj0SInt(foam);
	case FOAM_HInt:
		return gj0HInt(foam);
	case FOAM_SFlo:
		return gj0SFlo(foam);
	case FOAM_DFlo:
		return gj0DFlo(foam);
	case FOAM_BInt:
		return gj0BInt(foam);
	case FOAM_Byte:
		return gj0Byte(foam);
	case FOAM_Char:
		return gj0Char(foam);
	case FOAM_Bool:
		return gj0Bool(foam);
	case FOAM_Nil:
		return gj0Nil(foam);
	case FOAM_Cast:
		return gj0Cast(foam);
	case FOAM_CCall:
		return gj0CCall(foam);
	case FOAM_OCall:
		return gj0OCall(foam);
	case FOAM_PCall:
		return gj0PCall(foam);
	case FOAM_BCall:
		return gj0BCall(foam);
	case FOAM_Return:
		return gj0Return(foam);
	case FOAM_ANew:
		return gj0ANew(foam);
	case FOAM_AElt:
		return gj0AElt(foam);
	case FOAM_Arr:
		return gj0Arr(foam);
	case FOAM_RNew:
		return gj0RNew(foam);
	case FOAM_RElt:
		return gj0RElt(foam);
	case FOAM_Free:
		return gj0Free(foam);
	case FOAM_Env:
		return gj0Env(foam);
	case FOAM_Lex:
		return gj0Lex(foam);
	case FOAM_EElt:
		return gj0EElt(foam);
	case FOAM_EEnv:
		return gj0EEnv(foam);
	case FOAM_EInfo:
		return gj0EInfo(foam);
	case FOAM_EEnsure:
		return gj0EEnsure(foam);
	case FOAM_PushEnv:
		return gj0PushEnv(foam);
	case FOAM_Clos:
		return gj0Clos(foam);
	case FOAM_CEnv:
		return gj0CEnv(foam);
	case FOAM_CProg:
		return gj0CProg(foam);
	case FOAM_PRef:
		return gj0PRef(foam);
	case FOAM_Const:
		return gj0Const(foam);
	default: 
		return gj0Default(foam, strPrintf("Tag: %s", foamStr(foamTag(foam))));
	}

}

local JavaCodeList
gj0GenList(Foam *argv, AInt n)
{
	JavaCodeList args = listNil(JavaCode);
	int i;
	for (i=0; i<n; i++) {
		Foam elt = argv[i];
		args = listCons(JavaCode)(gj0Gen(elt), args);
	}
	args = listNReverse(JavaCode)(args);
	return args;
}


local JavaCode
gj0Def(Foam foam) {
	Foam lhs, rhs;
	JavaCode jc;
	
	assert(foamTag(foam) == FOAM_Def);

	lhs = foam->foamDef.lhs;
	rhs = foam->foamDef.rhs;
	switch (foamTag(lhs)) {
	case FOAM_Const:
		if (foamTag(rhs) == FOAM_Prog)
			jc = gj0Prog(lhs, rhs);
		else
			jc = gj0TopConst(lhs, rhs);
		break;
	case FOAM_Glo:
		if (gjContext->lvl == 0) 
			jc = gj0GloRegister(lhs, rhs);
		else
			jc = gj0Set(lhs, rhs);
		break;
	default:
		if (gjContext->lvl == 0)
			jc = gj0ClosInit(lhs, rhs);
		else
			jc = gj0Set(lhs, rhs);
		break;
	}
	return jc;
}

local JavaCode 
gj0ClosInit(Foam lhs, Foam rhs)
{
	
	return gj0Default(lhs, strCopy("closInit"));
}

local JavaCode 
gj0TopConst(Foam lhs, Foam rhs)
{
	return 0;
}

/*
 * :: Context globals
 */
local JavaCode
gj0GloRegister(Foam lhs, Foam rhs)
{
	JavaCode ref = gj0Gen(lhs);
	gjContext->gloRegList = listCons(JavaCode)(ref, gjContext->gloRegList);
	
	return 0;
}

/*
 * :: Prog
 */

typedef struct gjProgResult {
	FoamSigList sigs;
	JavaCode fnDef;
	JavaCode body;
	JavaCodeList vars;
	JavaCodeList args;
	JavaCode retnType;
	JavaCode id;
	String comment;
	int modifiers;
} *GjProgResult;

local GjProgResult gj0ProgMain(Foam f);
local JavaCode     gj0ProgResultToJava(GjProgResult r);
local void         gj0ProgResultFree(GjProgResult r);
local void         gj0ProgResultFree(GjProgResult r);
local JavaCode     gj0ProgEnvArg(Foam foam);
local JavaCodeList gj0ProgEnvInitCreate(Foam f);
local JavaCode     gj0ProgEnvDecl(int idx, Foam denv);
local JavaCode     gj0ProgLvlDecl(int idx, Foam denv);
local void         gj0ProgEnvInitCollect(Foam f, BitvClass clss, 
					 Bitv refMask, Bitv envMask);
local JavaCode     gj0ProgFnCreate(Foam lhs, Foam prog);
local JavaCode     gj0ProgFnMethodBody(Foam lhs, Foam prog);
local void         gj0ProgInitVars(IntSet set, Foam body);
local JavaCode     gj0ProgDecl(Foam ddecl, int idx, Bool isSet);
local JavaCode     gj0ProgDeclDefaultValue(Foam decl);
local void         gj0SeqHaltFlush(Foam foam);


local JavaCode
gj0Prog(Foam lhs, Foam rhs)
{
	GjProgResult r;
	JavaCode code;
	assert(foamTag(rhs) == FOAM_Prog);
	
	gj0ProgInit(lhs, rhs);
	r = gj0ProgMain(rhs);
	code = gj0ProgResultToJava(r);
	gj0ProgResultFree(r);

	gj0ProgFini(lhs, rhs);
	
	return code;
}

local void
gj0ProgInit(Foam lhs, Foam prog)
{
	gjContext->lvl++;
	gjContext->prog = prog;
	gjContext->progParams = prog->foamProg.params;
	gjContext->progLocals = prog->foamProg.locals;
	gjContext->progLhs    = lhs;
	gjContext->progSigList = listNil(FoamSig);
	gjContext->multVarIdx  = 0;
}

local void
gj0ProgFini(Foam lhs, Foam rhs)
{
	gjContext->lvl--;
	gjContext->progParams = NULL;
	gjContext->progLocals = NULL;

	gj0ProgAddStubs(gjContext->progSigList);
	listFree(FoamSig)(gjContext->progSigList);

	gjContext->progSigList = listNil(FoamSig);
}

local GjProgResult
gj0ProgMain(Foam f)
{
	GjProgResult r;
	JavaCodeList args, declarations, envInit, inits;
	JavaCodeList whole;
	JavaCode body, fnDef;
	String methodName;
	JavaCodeList exns;
	int modifiers;
	

	modifiers = gj0ProgModifiers();
	methodName = gj0ProgMethodName(gjContext->progLhs);

	gjDEBUG(dbOut, "(Entering prog: %s\n", methodName);

	args = gj0ProgCollectArgs(f);

	gj0SeqHaltFlush(f->foamProg.body);

	declarations = gj0ProgDeclarations(gjContext->progLocals,
					   f->foamProg.body);
	exns = gj0ProgExceptions();
	envInit = gj0ProgEnvInitCreate(f);
	
	fnDef = gj0ProgFnCreate(gjContext->progLhs, f);
	r = (GjProgResult) stoAlloc(OB_Other, sizeof(*r));
	body = gj0ProgGenerateBody(f);

	inits = listNConcat(JavaCode)(declarations, envInit);
	whole = listNConcat(JavaCode)(inits, listSingleton(JavaCode)(body));

	r->fnDef = fnDef;
	r->comment = 0;
	r->modifiers = modifiers;
	r->body = jcNLSeq(whole);
	r->args = args;
	r->retnType = gj0ProgRetnType(f);
	r->id = jcId(methodName);

	gjDEBUG(dbOut, " Completed prog: %s)\n", methodName);
	
	return r;
}

local JavaCode     
gj0ProgResultToJava(GjProgResult r)
{
	JavaCode method;

	method = jcMethod(r->modifiers, r->comment, r->retnType,
			  r->id, NULL, r->args, NULL, r->body);

	return jcNLSeqV(2, r->fnDef, method);
}


local void         
gj0ProgResultFree(GjProgResult r)
{
	stoFree(r);
}


local JavaCode
gj0ProgRetnType(Foam prog)
{
	return gj0TypeFrFmt(prog->foamProg.retType, prog->foamProg.format);
}

local String 
gj0ProgMethodName(Foam var)
{
	Foam format;
	int idx = var->foamConst.index;
	format = gjContext->constants;

	return gj0Name("c", format, idx);
}

local String 
gj0ProgFnName(int idx)
{
	Foam format;
	format = gjContext->constants;

	return gj0Name("C", format, idx);
}

local JavaCodeList
gj0ProgCollectArgs(Foam prog)
{
	JavaCodeList lst;
	Foam fmt = prog->foamProg.params;
	int i;

	lst = listNil(JavaCode);
	
	lst = listCons(JavaCode)(gj0ProgEnvArg(prog), lst);
	for (i=0; i<foamDDeclArgc(fmt); i++) {
		Foam decl = fmt->foamDDecl.argv[i];
		String   name = gj0Name("p", fmt, i);
		JavaCode type = gj0Type(decl);
		lst = listCons(JavaCode)(jcParamDecl(0, type, jcId(name)), lst);
	}
	
	return listNReverse(JavaCode)(lst);
}

local JavaCode
gj0ProgEnvArg(Foam foam)
{
	JavaCode type = gj0TypeFrFmt(FOAM_Env, 0);
	return jcParamDecl(0, type, gj0EnvId(1));
}

/** Extract the declarations from the given foam.
 * XXX This needs to be tied into the sequence stuff when
 * we get goto-reduction done.
 */
local JavaCodeList
gj0ProgDeclarations(Foam ddecl, Foam body)
{
	Table tbl = tblNew((TblHashFun) jcoHash, (TblEqFun) jcoEqual);
	TableIterator it;
	JavaCodeList decls;
	IntSet initted;
	int i=0;
	initted = intSetNew(foamDDeclArgc(ddecl));
	

	gj0ProgInitVars(initted, body);
	gjDEBUG(dbOut, "InitVars: %s\n", intSetToString(initted));

	foamIter(ddecl, pdecl, {
			JavaCode type = gj0Type(*pdecl);
			JavaCodeList l = (JavaCodeList) tblElt(tbl, type, 
							       listNil(JavaCode));
			l = listCons(JavaCode)(gj0ProgDecl(ddecl, i, 
							   intSetMember(initted, i)),
					       l);
			tblSetElt(tbl, type, l);
			i++;
		});
	
	decls = listNil(JavaCode);
	for (tblITER(it, tbl); tblMORE(it); tblSTEP(it)) {
		JavaCode type = tblKEY(it);
		JavaCodeList vars = listNReverse(JavaCode)(tblELT(it));
		JavaCode decl = jcParamDecl(0, type, jcCommaSeq(vars));
		decls = listCons(JavaCode)(jcStatement(decl), decls);
	}

	return decls;
}

local JavaCode 
gj0ProgDecl(Foam ddecl, int idx, Bool isSet)
{
	JavaCode var = jcId(gj0Name("t", ddecl, idx));
	if (!isSet) {
		Foam decl = ddecl->foamDDecl.argv[idx];
		var = jcAssign(var, gj0ProgDeclDefaultValue(decl));
	}
	return var;
}

local JavaCode 
gj0ProgDeclDefaultValue(Foam decl)
{
	switch (decl->foamDecl.type) {
	case FOAM_Word:
		return jcKeyword(symInternConst("null"));
	case FOAM_Bool:
		return jcKeyword(symInternConst("false"));
	case FOAM_Char:
		return jcLiteralChar("\\0");
	case FOAM_Byte:
	case FOAM_SInt:
	case FOAM_HInt:
	case FOAM_SFlo:
	case FOAM_DFlo:
		return jcLiteralInteger(0);
	default:
		return jcKeyword(symInternConst("null"));
	}
}


/*
 * Find the variables that are provably initialised.
 */
local void
gj0ProgInitVars(IntSet set, Foam body)
{
	Foam foam;
	int i;
	Bool done = false;
	for (i=0; !done && i<foamArgc(body); i++) {
		foam = body->foamSeq.argv[i];
		switch (foamTag(foam)) {
		case FOAM_Set:
			if (foamTag(foam->foamSet.lhs) == FOAM_Loc) {
				intSetAdd(set,
					  foam->foamSet.lhs->foamLoc.index);
			}
			break;
		case FOAM_If:
			done = true;
			break;
		case FOAM_Select:
			done = true;
			break;
		case FOAM_Label:
			done = true;
			break;
		case FOAM_Goto:
			done = true;
			break;
		}
	}
}

local JavaCode
gj0ProgGenerateBody(Foam fm)
{
	return gj0Gen(fm->foamProg.body);
}

local JavaCodeList
gj0ProgExceptions()
{
	return listNil(JavaCode);
}

local int 
gj0ProgModifiers()
{
	return JCO_MOD_Private;
}

local void
gj0ProgAddStubs(FoamSigList sigList)
{
	while (sigList != listNil(FoamSig)) {
		gjContext->ccallStubSigList = 
			gj0CCallStubAdd(gjContext->ccallStubSigList,
					car(sigList));
		sigList = cdr(sigList);
	}
}

local JavaCodeList
gj0ProgEnvInitCreate(Foam f)
{
	JavaCodeList lines;
	Foam denv;
	Bitv refLvls, refEnvs;
	BitvClass clss;
	AInt lvlCount;
	int i, maxLvl, maxEnv, max;

	denv = f->foamProg.levels;
	lvlCount = foamArgc(denv);
	clss = bitvClassCreate(lvlCount);
	refLvls = bitvNew(clss);
	refEnvs = bitvNew(clss);
	bitvClearAll(clss, refLvls);
	bitvClearAll(clss, refEnvs);

	gj0ProgEnvInitCollect(f->foamProg.body, clss, refLvls, refEnvs);
	maxLvl = bitvMax(clss, refLvls);
	maxEnv = bitvMax(clss, refEnvs);
	max = maxLvl>maxEnv ? maxLvl : maxEnv;

	lines = listNil(JavaCode);

	for (i=2; i<=max; i++) {
		JavaCode line;
		line = jcStatement(gj0ProgEnvDecl(i, denv));
		lines = listCons(JavaCode)(line, lines);
	}
	
	if (bitvTest(clss, refEnvs, 0)) {
		JavaCode line;
		JavaCode envRec;
		AInt fmt = denv->foamDEnv.argv[0];
		
		if (fmt == 0) 
			envRec = jcNull();
		else {
			gj0FmtUse(fmt);
			envRec = jcConstructV(gj0Id(GJ_EnvRecord), 1,
					      gj0FmtId(fmt));
		}
		line = jcInitialisation(JCO_MOD_Final,gj0TypeFrFmt(FOAM_Env, 0),
					gj0EnvId(0), jcConstructV(gj0TypeFrFmt(FOAM_Env, fmt), 2,
								  envRec,
								  gj0EnvId(1)));
		line = jcStatement(line);
		lines = listCons(JavaCode)(line, lines);
			     
	}
	for (i=0; i<lvlCount; i++) {
		if (bitvTest(clss, refLvls, i)) {
			JavaCode line;
			line = jcStatement(gj0ProgLvlDecl(i, denv));
			lines = listCons(JavaCode)(line, lines);
		}
	}

	bitvFree(refLvls);
	bitvFree(refEnvs);
	bitvClassDestroy(clss);

	return listNReverse(JavaCode)(lines);
}

local JavaCode 
gj0ProgEnvDecl(int idx, Foam denv)
{
	return jcInitialisation(JCO_MOD_Final, gj0TypeFrFmt(FOAM_Env, 0),
				gj0EnvId(idx),
				jcApplyMethod(gj0EnvId(idx-1),
					      jcId(strCopy("parent")),
					      listNil(JavaCode))
		);
}


local JavaCode 
gj0ProgLvlDecl(int idx, Foam denv)
{
	return jcInitialisation(JCO_MOD_Final, gj0Id(GJ_EnvRecord),
				gj0LvlId(idx),
				jcApplyMethod(gj0EnvId(idx),
					      jcId(strCopy("level")),
					      listNil(JavaCode))
		);
}


local void
gj0ProgEnvInitCollect(Foam f, BitvClass clss, Bitv refMask, Bitv envMask)
{
	foamIter(f, pelt, {
			Foam elt = *pelt;
			switch (foamTag(elt)) {
			case FOAM_Lex:
				bitvSet(clss, refMask, elt->foamLex.level);
				bitvSet(clss, envMask, elt->foamLex.level);
				break;
			case FOAM_Env:
				bitvSet(clss, envMask, elt->foamEnv.level);
				break;
			default:
				gj0ProgEnvInitCollect(elt, clss, refMask, envMask);
			}
		});
}

/*
    static Fn CFxx = new Fn() {
	    public Value ocall(Env env, Value ... vals) {
	       return cxx(env, vals[0], vals[1]);
	    }
	}
*/
local JavaCode
gj0ProgFnCreate(Foam lhs, Foam prog)
{
	JavaCodeList args;
	JavaCode method;
	JavaCode methodBody;
	JavaCode fnDef;
	JavaCode anonClass;
	String name;

	args = listList(JavaCode)(2,
				  jcParamDecl(0, gj0TypeFrFmt(FOAM_Env, 0),
					      jcId(strCopy("env"))),
				  jcParamDecl(0, 
					      jcNAry(gj0Id(GJ_FoamValue)),
					      jcId(strCopy("vals"))));

	methodBody = gj0ProgFnMethodBody(lhs, prog);

	method = jcMethod(JCO_MOD_Public, 0, gj0Id(GJ_FoamValue), 
			  jcId(strCopy("ocall")),
			  listNil(JavaCode),
			  args,
			  listNil(JavaCode),
			  methodBody);

	name = gj0ProgFnName(lhs->foamConst.index);
	anonClass = jcConstructSubclass(gj0Id(GJ_FoamFn),
					listSingleton(JavaCode)(jcLiteralString(strCopy(name))),
					jcNLSeqV(1, method));
	fnDef = jcInitialisation(JCO_MOD_Private, 
				 gj0Id(GJ_FoamFn),
				 jcId(strCopy(name)),
				 anonClass);
	strFree(name);
	return jcStatement(fnDef);
}

local JavaCode
gj0ProgFnMethodBody(Foam lhs, Foam prog)
{
	Foam pddecl = prog->foamProg.params;
	JavaCodeList l;
	JavaCode call;
	int i;
	JavaCodeList ret;

	l = listNil(JavaCode);
	l = listCons(JavaCode)(jcId(strCopy("env")), l);
	i = 0;
	foamIter(pddecl, pdecl, {
			Foam decl = *pdecl;
			JavaCode arg = jcArrayRef(jcId(strCopy("vals")), 
						  jcLiteralInteger(i));
			JavaCode castArg = gj0TypeValueToObj(arg, 
							     decl->foamDecl.type,
							     decl->foamDecl.format);
			l = listCons(JavaCode)(castArg, l);
			i++;
						  
		});
	l = listNReverse(JavaCode)(l);

	call = jcApply(jcId(gj0ProgMethodName(lhs)), l);


	if (foamTypeIsVoid(gjContext->formats,
			   prog->foamProg.retType, 
			   prog->foamProg.format)) {
		JavaCode l1 = call;
		JavaCode l2 = jcReturn(jcNull());
		ret = listList(JavaCode)(2, 
					 jcStatement(l1), 
					 jcStatement(l2));
	}
	else if (foamTypeIsMulti(gjContext->formats,
				 prog->foamProg.retType, 
				 prog->foamProg.format)) {
		JavaCode l1, l2;
		l1 = jcInitialisation(0, gj0TypeFrFmt(prog->foamProg.retType, 
						      prog->foamProg.format),
				      jcId(strCopy("ret")),
				      call);
		l2 = gj0TypeObjToValue(jcId(strCopy("ret")),
				       prog->foamProg.retType, 
				       prog->foamProg.format);
		l2 = jcReturn(l2);
		ret = listList(JavaCode)(2, jcStatement(l1), jcStatement(l2));
	}
	else {
		JavaCode l1, l2;
		l1 = jcInitialisation(0, gj0TypeFrFmt(prog->foamProg.retType, 
						      prog->foamProg.format),
				      jcId(strCopy("ret")),
				      call);
		l2 = gj0TypeObjToValue(jcId(strCopy("ret")),
				       prog->foamProg.retType, 
				       prog->foamProg.format);
		l2 = jcReturn(l2);
		ret = listList(JavaCode)(2, jcStatement(l1), jcStatement(l2));
	}

	return jcNLSeq(ret);
}


/*
 * :: Java Types
 *
 * The mapping is as follows:
 * Char --> char
 * Bool --> boolean
 * Byte --> byte
 * HInt --> short
 * SInt --> int
 * SFlo --> float
 * DFlo --> double
 * Arr  --> array[x] or Object
 * Word --> foamj.Word
 * NOp  --> void [return types]
 * Clos --> foamj.Clos
 * Env  --> foamj.Env
 */

local JavaCode
gj0Type(Foam decl)
{
	return gj0TypeFrFmt(decl->foamDecl.type, decl->foamDecl.format);
}

local JavaCode
gj0TypeFrFmt(AInt id, AInt fmt)
{
	switch (id) {
	case FOAM_Char:
		return jcKeyword(symInternConst("char"));
	case FOAM_SInt:
		return jcKeyword(symInternConst("int"));
	case FOAM_SFlo:
		return jcKeyword(symInternConst("float"));
	case FOAM_DFlo:
		return jcKeyword(symInternConst("double"));
	case FOAM_HInt:
		return jcKeyword(symInternConst("short"));
	case FOAM_Bool:
		return jcKeyword(symInternConst("boolean"));
	case FOAM_Byte:
		return jcKeyword(symInternConst("byte"));
	case FOAM_BInt:
		return jcImportedId(strCopy("java.math"),
				    strCopy("BigInteger"));
	case FOAM_NOp:
		if (fmt == 0 || fmt == emptyFormatSlot
		    || foamDDeclArgc(gjContext->formats->foamDFmt.argv[fmt]) == 0)
			return jcKeyword(symInternConst("void"));
		else
			return gj0Id(GJ_Multi);
	case FOAM_Clos:
		return gj0Id(GJ_FoamClos);
	case FOAM_Word:
		return gj0Id(GJ_FoamWord);
	case FOAM_Env:
		return gj0Id(GJ_FoamEnv);
	case FOAM_Rec:
		return gj0Id(GJ_FoamRecord);
	case FOAM_Ptr:
		return gj0Id(GJ_Object);
	case FOAM_Nil:
		return gj0Id(GJ_FoamWord);
	case FOAM_Arr:
		if (fmt != 0)
			return jcArrayOf(gj0TypeFrFmt(fmt, 0));
		else
			return gj0Id(GJ_Object);
	case FOAM_Values:
		return gj0Id(GJ_Multi);
	case FOAM_JavaObj:
		if (fmt != 0 && fmt != emptyFormatSlot)
			return gj0TypeFrJavaObj(gjContext->formats->foamDFmt.argv[fmt]);
		else
			return gj0Id(GJ_Object);
	default:
		return jcId(strCopy(foamStr(id)));
	}
}

local JavaCode
gj0TypeFrJavaObj(Foam format)
{
	String txt = format->foamDDecl.argv[0]->foamDecl.id;
	return jcImportedIdFrString(txt);
}


local JavaCode
gj0TypeValueToObj(JavaCode val, FoamTag type, AInt fmt)
{
	switch (type) {
	case FOAM_Char:
		return jcApplyMethodV(val, jcId(strCopy("toChar")), 0);
	case FOAM_SInt:
		return jcApplyMethodV(val, jcId(strCopy("toSInt")), 0);
	case FOAM_Byte:
		return jcApplyMethodV(val, jcId(strCopy("toByte")), 0);
	case FOAM_HInt:
		return jcApplyMethodV(val, jcId(strCopy("toHInt")), 0);
	case FOAM_BInt:
		return jcApplyMethodV(val, jcId(strCopy("toBInt")), 0);
	case FOAM_SFlo:
		return jcApplyMethodV(val, jcId(strCopy("toSFlo")), 0);
	case FOAM_DFlo:
		return jcApplyMethodV(val, jcId(strCopy("toDFlo")), 0);
	case FOAM_NOp:
		assert(false);
		return 0;
	case FOAM_Clos:
		return jcApplyMethod(gj0Id(GJ_FoamClos),
				     jcId(strCopy("fromValue")),
				     listSingleton(JavaCode)(val));
	case FOAM_Word:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromValue")),
				     listSingleton(JavaCode)(val));
	case FOAM_Bool:
		return jcApplyMethodV(val, jcId(strCopy("toBool")), 0);
	case FOAM_Rec:
		return gj0TypeValueToRec(val, fmt);
	case FOAM_Arr:
		return gj0TypeValueToArr(val, fmt);
	case FOAM_Ptr:
		return jcApplyMethodV(val, jcId(strCopy("toPtr")), 0);
	case FOAM_Env:
		return jcApplyMethodV(val, jcId(strCopy("toEnv")), 0);
	default:
		return jcCast(jcSpaceSeqV(2, gj0Id(GJ_FoamValue),
					  jcComment(strCopy(foamStr(type)))),
			      val);

	}
}

local JavaCode
gj0TypeValueToArr(JavaCode value, AInt fmt)
{
	JavaCode arrobj = jcApplyMethodV(value, jcId(strCopy("toArray")), 0);
	if (fmt == 0)
		return arrobj;
	else
		return jcCast(jcArrayOf(gj0TypeFrFmt(fmt, 0)), arrobj);
}

local JavaCode
gj0TypeValueToRec(JavaCode val, AInt fmt)
{
	JavaCode record = jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
						  jcId(strCopy("U"))),
					 jcId(strCopy("toRecord")), 1,
					 val);
	return record;
}

local JavaCode
gj0TypeObjToValue(JavaCode val, FoamTag type, AInt fmt)
{
	switch (type) {
	case FOAM_Word:
		return jcCast(gj0Id(GJ_FoamValue), val);
	case FOAM_SInt:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromSInt")), 1,
				      val);
	case FOAM_BInt:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromBInt")), 1,
				      val);
	case FOAM_Bool:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromBool")), 1, 
				      val);
	case FOAM_Arr:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromArray")), 1,
				      val);
	case FOAM_Ptr:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromPtr")), 1,
				      val);
	case FOAM_Char:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromChar")), 1,
				      val);
	case FOAM_HInt:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromHInt")), 1,
				      val);
	case FOAM_DFlo:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromDFlo")), 1,
				      val);
	case FOAM_SFlo:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromSFlo")), 1,
				      val);
	case FOAM_Byte:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamValue),
					       jcId(strCopy("U"))),
				      jcId(strCopy("fromByte")), 1,
				      val);
	case FOAM_JavaObj:
		return jcSpaceSeqV(2,
				   jcComment(strPrintf("asWord %d %d", type, fmt)),
				   val);
	case FOAM_Env:
	case FOAM_Clos:
	case FOAM_Rec:
	case FOAM_NOp:
		return val;
	default:
		assert(false);
		return NULL;
	}

}

local JavaCode 
gj0Set(Foam lhs, Foam rhs)
{
	switch (foamTag(lhs)) {
	case FOAM_Fluid:
		return gj0Default(lhs, strCopy("fluidSet"));
	case FOAM_Glo:
		return gj0GloSet(lhs, rhs);
	case FOAM_AElt:
		return gj0AEltSet(lhs, rhs);
	case FOAM_RElt:
		return gj0REltSet(lhs, rhs);
	case FOAM_Lex:
		return gj0LexSet(lhs, rhs);
	case FOAM_EElt:
		return gj0EEltSet(lhs, rhs);
	case FOAM_CEnv:
		return gj0CEnvSet(lhs, rhs);
	case FOAM_CProg:
		return gj0CProgSet(lhs, rhs);
	case FOAM_Values:
		return gj0ValuesSet(lhs, rhs);
	case FOAM_PRef:
		return gj0PRefSet(lhs, rhs);
	case FOAM_EInfo:
		return gj0EInfoSet(lhs, rhs);
	case FOAM_Loc:
		return gj0LocSet(lhs, rhs);
	default: {
		JavaCode lhsJ = gj0Gen(lhs);
		JavaCode rhsJ = gj0Gen(rhs);
		return jcAssign(lhsJ, rhsJ);
	}
		
	}
}

local JavaCode
gj0SetGenRhs(Foam foam, Foam decl)
{
	if (foamTag(foam) == FOAM_Cast) {
		assert(decl->foamDecl.type == foam->foamCast.type);
		return gj0CastFmt(foam, decl->foamDecl.format);
	}
	else 
		return gj0Gen(foam);
}



local JavaCode
gj0Par(Foam ref)
{
	Foam fmt = gjContext->progParams;
	return jcId(gj0Name("p", fmt, ref->foamPar.index));
}

local JavaCode
gj0Loc(Foam ref)

{
	Foam fmt = gjContext->progLocals;
	return jcId(gj0Name("t", fmt, ref->foamLoc.index));
}

local JavaCode
gj0LocSet(Foam ref, Foam rhs)
{
	Foam fmt  = gjContext->progLocals;
	Foam decl = fmt->foamDDecl.argv[ref->foamLoc.index];
	
	JavaCode lhsJ = jcId(gj0Name("t", fmt, ref->foamLoc.index));
	JavaCode rhsJ = gj0SetGenRhs(rhs, decl);

	return jcAssign(lhsJ, rhsJ);
}

local JavaCode
gj0Glo(Foam ref)
{
	Foam decl;
	JavaCode jc;
	String id;
	
	decl = gjContextGlobal(ref->foamGlo.index);
	if (decl->foamGDecl.protocol == FOAM_Proto_Init) {
		return jcId(gj0InitVar(ref->foamGlo.index));
	}
	id = decl->foamDecl.id;
	jc = jcApplyMethod(gj0Id(GJ_FoamGlobals),
			     jcId(strCopy("getGlobal")),
			     listSingleton(JavaCode)(jcLiteralString(strCopy(id))));

	return gj0TypeValueToObj(jc, decl->foamDecl.type, decl->foamDecl.format);
}

local JavaCode
gj0GloSet(Foam lhs, Foam rhs)
{
	JavaCode jc;
	Foam decl;
	String id;

	decl = gjContextGlobal(lhs->foamGlo.index);
	id = decl->foamDecl.id;
	
	jc = gj0TypeObjToValue(gj0SetGenRhs(rhs, decl),
			       decl->foamDecl.type, decl->foamDecl.format);

	jc = jcApplyMethodV(gj0Id(GJ_FoamGlobals),
			    jcId(strCopy("setGlobal")), 
			    2, jcLiteralString(strCopy(id)), jc);
	return jc;
}


/*
 * :: Control flow
 *
 * ...
 * int target = 0;
 * while (true) {
 *      case 0:
 *          [intro]
 *      case 1:
 *          // (Goto 5) 
 *          target = 5;
 *          continue;
 *      case n:
 *          ...
 *          break;
 * }
 *
 */
struct gjSeqBucket {
	AInt label; /* -1 ==> prefix */
	Bool unreachable;
	JavaCodeList list;
};

typedef struct gjSeqBucket *GjSeqBucket;
DECLARE_LIST(GjSeqBucket);
CREATE_LIST(GjSeqBucket);

struct gjSeqStore {
	GjSeqBucketList buckets;
};

typedef struct gjSeqStore *GjSeqStore;

#define GJ_SEQ_Prefix (-2)
#define GJ_SEQ_Init (-1)
#define GJ_SEQ_Halt (-3)

local void gj0SeqGen(GjSeqStore seqs, Foam stmt);
local void gj0SeqGoto(GjSeqStore store, Foam foam);
local void gj0SeqLabel(GjSeqStore store, Foam foam);
local void gj0SeqSelect(GjSeqStore store, Foam foam);
local void gj0SeqSelect2(GjSeqStore store, Foam foam);
local void gj0SeqGenDefault(GjSeqStore store, Foam foam);
local void gj0SeqSelectMulti(GjSeqStore store, Foam foam);
local void gj0SeqIf(GjSeqStore store, Foam foam);
local void gj0SeqBCall(GjSeqStore store, Foam foam);
local void gj0SeqValues(GjSeqStore store, Foam foam);

local JavaCode gj0SeqSwitchId();

local GjSeqStore   gj0SeqStoreNew(void);
local void         gj0SeqStoreFree(GjSeqStore store);
local JavaCode     gj0SeqStoreToJava(GjSeqStore store);
local void         gj0SeqStoreAddStmt(GjSeqStore store, JavaCode stmt);
local void         gj0SeqStoreAddLabel(GjSeqStore store, AInt label);
local void         gj0SeqStoreEnsureBody(GjSeqStore store);
local void         gj0SeqStoreAddHalt(GjSeqStore store, JavaCode stmt);

local GjSeqBucket  gj0SeqBucketNew(AInt label);
local void         gj0SeqBucketFree(GjSeqBucket store);
local JavaCodeList gj0SeqBucketToJava(GjSeqBucket bucket);
local GjSeqBucket  gj0SeqBucketGetPrefix(GjSeqBucketList buckets);
local Bool         gj0SeqBucketIsPrefix(GjSeqBucket bucket);
local Bool         gj0SeqBucketIsHalt(GjSeqBucket bucket);

local JavaCode     gj0ReturnArray(Foam foam);

local JavaCode 
gj0Seq(Foam seq)
{
	JavaCode code;
	GjSeqStore seqs = gj0SeqStoreNew();
	int i;
	
	for (i=0; i != -1; i = foamSeqNextReachable(seq, i)) {
		Foam stmt = seq->foamSeq.argv[i];

		gj0SeqGen(seqs, seq->foamSeq.argv[i]);
		if (foamInfo(foamTag(stmt)).properties & FOAMP_SeqExit
		    && gj0SeqBucketIsPrefix(car(seqs->buckets))) {
			break;
		}
	}

	code = gj0SeqStoreToJava(seqs);
	gj0SeqStoreFree(seqs);
	return code;
}

local JavaCode
gj0Return(Foam r)
{
	if (foamTag(r->foamReturn.value) == FOAM_Values)
		return gj0ValuesReturn(r->foamReturn.value);

	if (gjContext->prog->foamProg.retType == FOAM_Arr)
		return gj0ReturnArray(r->foamReturn.value);

	return jcReturn(gj0Gen(r->foamReturn.value));
}

local JavaCode
gj0ReturnArray(Foam foam)
{
	JavaCode v;
	FoamTag tag;
	AInt fmt;

	tag = gj0FoamExprTypeWFmt(foam, &fmt);
	if (tag == FOAM_Arr || fmt == gjContext->prog->foamProg.format)
		return jcReturn(gj0Gen(foam));

	v = jcCast(gj0TypeFrFmt(tag, fmt), gj0Gen(foam));
	return jcReturn(v);
}


local void 
gj0SeqGen(GjSeqStore seqs, Foam foam)
{
	switch (foamTag(foam)) {
	case FOAM_Goto:
		gj0SeqGoto(seqs, foam);
		break;
	case FOAM_Select:
		gj0SeqSelect(seqs, foam);
		break;
	case FOAM_If:
		gj0SeqIf(seqs, foam);
		break;
	case FOAM_Label:
		gj0SeqLabel(seqs, foam);
		break;
	case FOAM_BCall:
		gj0SeqBCall(seqs, foam);
		break;
	case FOAM_Cast:
		gj0SeqGen(seqs, foam->foamCast.expr);
		break;
	case FOAM_Values:
		gj0SeqValues(seqs, foam);
		break;
	default:
		gj0SeqGenDefault(seqs, foam);
		break;
	}
}

local void
gj0SeqGoto(GjSeqStore store, Foam foam)
{
	AInt tgt = foam->foamGoto.label;
	JavaCode s1 = jcAssign(gj0SeqSwitchId(), jcLiteralInteger(tgt));
	JavaCode s2 = jcContinue(0);

	gj0SeqStoreEnsureBody(store);

	gj0SeqStoreAddStmt(store, jcStatement(s1));
	gj0SeqStoreAddStmt(store, jcStatement(s2));
}

local void
gj0SeqLabel(GjSeqStore store, Foam foam)
{
	gj0SeqStoreEnsureBody(store);
	gj0SeqStoreAddLabel(store, foam->foamLabel.label);
}

local void
gj0SeqSelect(GjSeqStore bucketList, Foam foam)
{
	if (foamSelectArgc(foam) == 2)
		gj0SeqSelect2(bucketList, foam);
	else 
		gj0SeqSelectMulti(bucketList, foam);
}

local void
gj0SeqSelect2(GjSeqStore store, Foam foam) 
{
	JavaCode lhs, rhs, s1, s2;

	lhs = gj0SeqSwitchId();
	rhs = jcConditional(jcBinOp(JCO_OP_Equals,
				    gj0Gen(foam->foamSelect.op), 
				    jcLiteralInteger(0)),
			    jcLiteralInteger(foam->foamSelect.argv[0]),
			    jcLiteralInteger(foam->foamSelect.argv[1]));

	s1 = jcAssign(lhs, rhs);
	s2 = jcContinue(0);

	gj0SeqStoreEnsureBody(store);
	gj0SeqStoreAddStmt(store, jcStatement(s1));
	gj0SeqStoreAddStmt(store, jcStatement(s2));

}

local void
gj0SeqIf(GjSeqStore store, Foam foam)
{
	/* if (...) { label = .; break;} */
	JavaCode seq, block;
	JavaCode s1, s2;

	s1 = jcAssign(gj0SeqSwitchId(), jcLiteralInteger(foam->foamIf.label));
	s2 = jcContinue(0);
	seq = jcSpaceSeqV(2, jcStatement(s1), jcStatement(s2));
	block = jcBlockNoNL(seq);
	
	gj0SeqStoreEnsureBody(store);
	gj0SeqStoreAddStmt(store, jcIf(gj0Gen(foam->foamIf.test), block));
}

 
local void 
gj0SeqSelectMulti(GjSeqStore store, Foam foam)
{
	/* switch (xxx) {
	 *   case 0: target = 1; break;
	 *   case 1: target = 4;
	 *   case 2: target = 9;
	 * }
	 * continue
         */
	JavaCodeList body;
	JavaCode tst;
	int i;
	
	body = listNil(JavaCode);
	tst = gj0Gen(foam->foamSelect.op);
	for (i=0; i<foamSelectArgc(foam); i++) {
		JavaCode l1, s2, s3;
		l1 = jcCaseLabel(jcLiteralInteger(i));
		s2 = jcAssign(gj0SeqSwitchId(), jcLiteralInteger(foam->foamSelect.argv[i]));
		s3 = jcBreak(0);

		body = listCons(JavaCode)(l1, body);
		body = listCons(JavaCode)(jcStatement(s2), body);
		body = listCons(JavaCode)(jcStatement(s3), body);
	}
	body = listNReverse(JavaCode)(body);

	gj0SeqStoreEnsureBody(store);
	gj0SeqStoreAddStmt(store, jcSwitch(tst, body));
	gj0SeqStoreAddStmt(store,  jcStatement(jcContinue(0)));

}


local void
gj0SeqBCall(GjSeqStore seqs, Foam foam)
{
	JavaCode jc;
	if (foam->foamBCall.op != FOAM_BVal_Halt) {
		gj0SeqGenDefault(seqs, foam);
		return;
	}
	jc = jcStatement(gj0Gen(foam));
	
	gj0SeqStoreAddHalt(seqs, jc);
}

local void
gj0SeqValues(GjSeqStore store, Foam foam)
{
	int i;
	for (i=0; i < foamArgc(foam); i++) {
		FoamTag tag = foamTag(foam->foamValues.argv[i]);
		if (tag != FOAM_Loc && tag != FOAM_Lex)
			bug("Odd foam found");
	}
}

local void
gj0SeqGenDefault(GjSeqStore store, Foam foam)
{
	JavaCode stmt = gj0Gen(foam);
	int i;

	if (!gj0IsSeqNode(stmt)) {
		gj0SeqStoreAddStmt(store, jcStatement(stmt));
		return;
	}

	for (i=0; i<jcoArgc(stmt); i++) {
		gj0SeqStoreAddStmt(store, jcoArgv(stmt)[i]);
	}
}

local JavaCode
gj0SeqSwitchId()
{
	return jcId(strCopy("target"));
}


local GjSeqStore 
gj0SeqStoreNew(void)
{
	GjSeqStore store = (GjSeqStore) stoAlloc(OB_Other, sizeof(*store));
	store->buckets = listNil(GjSeqBucket);
	return store;
}

local void     
gj0SeqStoreFree(GjSeqStore store)
{
	listFreeDeeply(GjSeqBucket)(store->buckets, gj0SeqBucketFree);
	stoFree(store);
}

local void
gj0SeqStoreEnsureBody(GjSeqStore store)
{
	if (store->buckets == listNil(GjSeqBucket)) {
		store->buckets = listSingleton(GjSeqBucket)(gj0SeqBucketNew(GJ_SEQ_Init));
		return;
	}
	if (gj0SeqBucketIsPrefix(car(store->buckets))) {
		store->buckets = listCons(GjSeqBucket)(gj0SeqBucketNew(GJ_SEQ_Init),
						       store->buckets);
		return;
	}
}

local void
gj0SeqStoreAddStmt(GjSeqStore store, JavaCode stmt)
{
	GjSeqBucket bucket;

	if (store->buckets == listNil(GjSeqBucket)) {
		store->buckets = listSingleton(GjSeqBucket)(gj0SeqBucketNew(GJ_SEQ_Prefix));
	}

	bucket = car(store->buckets);
	bucket->list = listCons(JavaCode)(stmt, bucket->list);
}

local void
gj0SeqStoreAddLabel(GjSeqStore store, AInt label)
{
	GjSeqBucket bucket;
	
	bucket = gj0SeqBucketNew(label);
	store->buckets = listCons(GjSeqBucket)(bucket,
					       store->buckets);
}

local void 
gj0SeqStoreAddHalt(GjSeqStore store, JavaCode stmt)
{
	GjSeqBucket bucket;

	bucket = gj0SeqBucketNew(GJ_SEQ_Halt);
	bucket->list = listSingleton(JavaCode)(stmt);
	store->buckets = listCons(GjSeqBucket)(bucket,
					       store->buckets);
}



local JavaCode
gj0SeqStoreToJava(GjSeqStore store)
{
	JavaCodeList jlst, l1, l2;
	JavaCode stmt, decl;
	GjSeqBucketList buckets, lst;
	GjSeqBucket prefix;
	int labelCount;
	assert(listLength(GjSeqBucket)(store->buckets) > 0);

	if (listLength(GjSeqBucket)(store->buckets) == 1) {
		JavaCodeList code = gj0SeqBucketToJava(car(store->buckets));
		store->buckets = listNil(GjSeqBucket);
		return jcNLSeq(code);
	}
	
	buckets = listNReverse(GjSeqBucket)(store->buckets);
	prefix = gj0SeqBucketGetPrefix(buckets);

	jlst = listNil(JavaCode);
	lst = buckets;

	labelCount = 0;
	while (lst) {
		JavaCodeList block;
		GjSeqBucket bucket;
		bucket = car(lst);
		if (gj0SeqBucketIsPrefix(bucket)) {
			lst = cdr(lst);
			continue;
		}
		if (!gj0SeqBucketIsHalt(bucket)) {
			labelCount++;
			jlst = listCons(JavaCode)(jcCaseLabel(jcLiteralInteger(bucket->label)),
						  jlst);
		}
		block = gj0SeqBucketToJava(bucket);
		jlst  = listNConcat(JavaCode)(listNReverse(JavaCode)(block),
					      jlst);
		lst = cdr(lst);
	}

	listFree(GjSeqBucket)(buckets);
	store->buckets = listNil(GjSeqBucket);
	jlst = listNReverse(JavaCode)(jlst);

	l1 = prefix == 0 ? listNil(JavaCode) : gj0SeqBucketToJava(prefix);
	if (labelCount == 0) {
		return jcNLSeq(listNConcat(JavaCode)(l1, jlst));
	}
	else {
		decl = jcStatement(jcInitialisation(0, 
					    jcKeyword(symInternConst("int")), 
					    gj0SeqSwitchId(),
					    jcLiteralInteger(GJ_SEQ_Init)));

		stmt = jcWhile(jcKeyword(symInternConst("true")),
			       jcBlock(jcSwitch(gj0SeqSwitchId(), jlst)));
		l2 = listList(JavaCode)(2, decl, stmt);
		return jcNLSeq(listNConcat(JavaCode)(l1, l2));
	}
}

local GjSeqBucket 
gj0SeqBucketNew(AInt label)
{
	GjSeqBucket bucket = (GjSeqBucket) stoAlloc(OB_Other, sizeof(*bucket));
	
	bucket->label = label;
	bucket->list = listNil(JavaCode);

	return bucket;
}

local void
gj0SeqBucketFree(GjSeqBucket bucket)
{
	listFreeDeeply(JavaCode)(bucket->list, jcoFree);
	stoFree(bucket);
}

local JavaCodeList
gj0SeqBucketToJava(GjSeqBucket bucket)
{
	JavaCodeList l = listNReverse(JavaCode)(bucket->list);
	bucket->list = listNil(JavaCode);

	if (bucket->label == GJ_SEQ_Halt) {
		if (listLength(JavaCode)(l) > 1) 
			car(l) = jcIf(jcFalse(), car(l));
	}

	return l;
}

local GjSeqBucket
gj0SeqBucketGetPrefix(GjSeqBucketList buckets)
{

	while (buckets) {
		GjSeqBucket bucket;
		bucket = car(buckets);
		if (gj0SeqBucketIsPrefix(bucket))
			return bucket;
		buckets = cdr(buckets);
	}
	return 0;
}

local Bool
gj0SeqBucketIsPrefix(GjSeqBucket bucket)
{
	return bucket->label == GJ_SEQ_Prefix;
}

local Bool
gj0SeqBucketIsHalt(GjSeqBucket bucket)
{
	return bucket->label == GJ_SEQ_Halt;
}

local JavaCode
gj0Throw(Foam foam)
{
	SExpr sx = foamToSExpr(foam);
	String s = sxiFormat(sx);
	
	sxiFree(sx);

	return jcComment(s);
}


/*
 * :: Custom java classes
 */

static struct jclss gjClss[] = {
	{ -1,     jcNodePrint,  jcNodeSExpr,  "stmtseq", "\""}
};

local JavaCode
gj0SeqNode(JavaCodeList l)
{
	return jcoNewFrList(&gjClss[0], l);
}

local Bool
gj0IsSeqNode(JavaCode jc)
{
	return jcoClass(jc) == &gjClss[0];
}

/*
 * Destructively replaces any statements containing a 'halt'
 * with the halt itself (safe as all foam statements evaluate
 * all their arguments.
 */
local Bool gj0SeqIsBCallHalt(Foam f);

local void
gj0SeqHaltFlush(Foam foam) 
{
	int i;
	
	assert(foamTag(foam) == FOAM_Seq);

	for (i=0; i<foamArgc(foam); i++) {
		Foam stmt = foam->foamSeq.argv[i];
		Foam f = foamFindFirst(gj0SeqIsBCallHalt, stmt);
		if (f == 0)
			continue;
		foam->foamSeq.argv[i] = foamCopy(f);
		foamFree(stmt);
	}
	
}

local Bool
gj0SeqIsBCallHalt(Foam f)
{
	if (foamTag(f) != FOAM_BCall)
		return false;
	if (f->foamBCall.op != FOAM_BVal_Halt)
		return false;
	return true;
}

/*
 * :: Method calls
 */


local JavaCode
gj0CCall(Foam call)
{
	JavaCode code;

	FoamSig sig = gj0FoamSigFrCCall(call);
	String id = gj0CCallStubName(sig);
	gjContext->progSigList = gj0CCallStubAdd(gjContext->progSigList, sig);
	code = jcApply(jcId(id), gj0GenList(&call->foamCCall.op, foamArgc(call)-1));

	return code;
}

String 
gj0TypeAbbrev(FoamTag tag)
{
	switch (tag) {
	case FOAM_Rec:
		return "R";
	case FOAM_Arr:
		return "A";
	case FOAM_Word:
		return "W";
	case FOAM_HInt:
		return "H";
	case FOAM_SInt:
		return "I";
	case FOAM_NOp:
		return "X";
	case FOAM_Clos:
		return "C";
	case FOAM_Bool:
		return "B";
	case FOAM_Byte:
		return "Y";
	case FOAM_BInt:
		return "N";
	case FOAM_Char:
		return "L";
	case FOAM_Ptr:
		return "P";
	case FOAM_SFlo:
		return "S";
	case FOAM_DFlo:
		return "D";
	default:
		return "?";
	}
}

/*
 * :: OCall
 */

local JavaCode
gj0OCall(Foam foam)
{
	JavaCodeList args;
	JavaCode env;

	env = gj0Gen(foam->foamOCall.env);
	args = gj0GenList(foam->foamOCall.argv, foamArgc(foam)-3);
	
	return jcApply(jcId(gj0ProgMethodName(foam->foamOCall.op)), 
		       listCons(JavaCode)(env, args));
		     
}


/*
 * :: Multiple values
 */

local Foam     gj0ValuesDDeclCopy(Foam foam);
local JavaCode gj0ValuesTmpVar();

local JavaCode
gj0ValuesSet(Foam lhs, Foam rhs)
{
	JavaCodeList l;
	JavaCode assign, rhsJ, var, rhsType, init;
	Foam rhsDDecl;
	int i;

	if (foamTag(rhs) == FOAM_MFmt
	    && foamTag(rhs->foamMFmt.value) == FOAM_Values)
		rhs = rhs->foamMFmt.value;
	
	if (foamTag(rhs) == FOAM_Values) {
		l = listNil(JavaCode);
		for (i=0; i<foamArgc(lhs); i++) {
			assign = gj0Set(lhs->foamValues.argv[i],
					rhs->foamValues.argv[i]);
			l = listCons(JavaCode)(assign, l);
		}
		return jcNLSeq(listNReverse(JavaCode)(l));
	}

	l = listNil(JavaCode);
	rhsJ = gj0Gen(rhs);
	rhsDDecl = gj0ValuesDDeclCopy(rhs);
	var = gj0ValuesTmpVar();
	rhsType = gj0TypeFrFmt(FOAM_Values, 0);

	init = jcInitialisation(0, rhsType, jcoCopy(var), rhsJ); 
	l = listCons(JavaCode)(jcStatement(init), l);
	for (i=0; i<foamArgc(lhs); i++) {
		assign = jcAssign(gj0Gen(lhs->foamValues.argv[i]),
				  gj0RecElt(jcoCopy(var), rhsDDecl, i));
		l = listCons(JavaCode)(jcStatement(assign), l);
	}
	
	jcoFree(var);
	foamFree(rhsDDecl);

	return gj0SeqNode(listNReverse(JavaCode)(l));
}

local JavaCode
gj0ValuesTmpVar()
{
	int idx = gjContext->multVarIdx++;
	
	return jcId(strPrintf("var%d", idx));
}

local Foam
gj0ValuesDDeclCopy(Foam foam)
{
	if (foamTag(foam) == FOAM_MFmt) {
		Foam ddecls = gjContext->formats;
		int idx = foam->foamMFmt.format;
		return foamCopy(ddecls->foamDFmt.argv[idx]);
	}
	
	if (foamTag(foam) == FOAM_BCall) {
		switch (foam->foamBCall.op) {
		case FOAM_BVal_SIntDivide:
			return foamNew(FOAM_DDecl, 2, 
				       foamNewDecl(FOAM_SInt, "q", 0),
				       foamNewDecl(FOAM_SInt, "r", 0));
		case FOAM_BVal_BIntDivide:
			return foamNew(FOAM_DDecl, 2, 
				       foamNewDecl(FOAM_BInt, "q", 0),
				       foamNewDecl(FOAM_BInt, "r", 0));
		default:
			assert(false);
		}
		
	}
	assert(false);
	return NULL;
}

local JavaCode
gj0ValuesReturn(Foam foam)
{
	Foam ddecl, ddecls;
	JavaCodeList l;
	JavaCode assign, lhs, set, ret;
	int i, idx;

	if (foamArgc(foam)  == 0)
		return jcReturnVoid();

	lhs = gj0ValuesTmpVar();
	l = listNil(JavaCode);
	ddecls = gjContext->formats;
	idx    = gjContext->prog->foamProg.format;
	ddecl  = ddecls->foamDFmt.argv[idx];

	gj0FmtUse(idx);
	assign = jcInitialisation(0,
				  gj0TypeFrFmt(FOAM_Values, 0),
				  jcoCopy(lhs),
				  jcConstructV(gj0TypeFrFmt(FOAM_Values, 0), 1,
					       gj0FmtId(idx)));
	l = listCons(JavaCode)(jcStatement(assign), l);
	for (i=0; i<foamArgc(foam); i++) {
		set = gj0RecSet(jcoCopy(lhs),
				gj0Gen(foam->foamValues.argv[i]),
				ddecl, i);
		l = listCons(JavaCode)(jcStatement(set), l);
	}

	ret = jcReturn(jcoCopy(lhs));
	l = listCons(JavaCode)(jcStatement(ret), l);
	jcoFree(lhs);

	return gj0SeqNode(listNReverse(JavaCode)(l));
}

local JavaCode
gj0MFmt(Foam f)
{
	JavaCode jc;
	gjContext->mfmt = f->foamMFmt.format;
	jc = gj0Gen(f->foamMFmt.value);
	gjContext->mfmt = 0;
	
	return jc;
}


/*
 * :: Integer Literals
 */


local JavaCode
gj0SInt(Foam foam)
{
	return jcLiteralInteger(foam->foamSInt.SIntData);
}

local JavaCode
gj0BInt(Foam foam)
{
	BInt val = foam->foamBInt.BIntData;
	if (bintIsZero(val))
		return jcMemRef(gj0Id(GJ_BigInteger),
				jcId(strCopy("ZERO")));
	if (bintLength(val) < 30 && bintIsSmall(val)) {
		long smallval = bintSmall(val);
		if (smallval == 1) {
			return jcMemRef(gj0Id(GJ_BigInteger),
					jcId(strCopy("ONE")));
		}
		else {
			return jcApplyMethodV(gj0Id(GJ_BigInteger),
					      jcId(strCopy("valueOf")),
					      1, jcLiteralInteger(smallval));
		}
	}
	else {
		return jcConstructV(gj0Id(GJ_BigInteger),
				    1, jcLiteralString(bintToString(val)));
	}
}

local JavaCode
gj0Byte(Foam foam)
{
	return jcCast(gj0TypeFrFmt(FOAM_Byte, 0),
		      jcLiteralInteger(foam->foamByte.ByteData));
}

local JavaCode
gj0HInt(Foam foam)
{
	return jcCast(gj0TypeFrFmt(FOAM_HInt, 0),
		      jcLiteralInteger(foam->foamHInt.HIntData));
}


/*
 * :: Float Literals
 */

local JavaCode
gj0SFlo(Foam foam)
{
	char buf[MAX_FLOAT_SIZE];
	DFloatSprint(buf, foam->foamSFlo.SFloData);
	return jcCast(gj0TypeFrFmt(FOAM_SFlo, 0),
		      jcLiteralFloatFrString(strCopy(buf)));
}
local JavaCode
gj0DFlo(Foam foam)
{
	char buf[MAX_FLOAT_SIZE];
	DFloatSprint(buf, foam->foamSFlo.SFloData);
	return jcLiteralFloatFrString(strCopy(buf));
}

/*
 * :: Boolean Literals
 */


local JavaCode
gj0Bool(Foam foam)
{
	return foam->foamBool.BoolData 
		? jcKeyword(symInternConst("true"))
		: jcKeyword(symInternConst("false"));
}

/*
 * :: Char Literals
 */


local JavaCode
gj0Char(Foam foam)
{
	char buf[2] = "";
	buf[0] = foam->foamChar.CharData;
	return jcLiteralChar(buf);
}

local JavaCode
gj0Nil(Foam foam)
{
	return jcKeyword(symInternConst("null"));
}


/*
 * :: Default code
 */

local JavaCode 
gj0Default(Foam foam, String prefix) 
{
	bug("%s", aStrPrintf("Java not implemented: %s %pFoam\n", prefix, foam));
	return NULL; /* Not reached */
}

local FoamTag
gj0FoamExprType(Foam foam)
{
	FoamTag tag;
	AInt    extra;

	tag = foamExprType0(foam, gjContext->prog, 
			    gjContext->formats,
			    0, 0, &extra);
	
	return tag;
}

local FoamTag
gj0FoamExprTypeWFmt(Foam foam, AInt *fmt)
{
	FoamTag tag;

	tag = foamExprType0(foam, gjContext->prog, 
			    gjContext->formats,
			    0, 0, fmt);
	
	return tag;
}


/*
 * :: Arrays
 */

local JavaCode 
gj0ANew(Foam foam) {
	AInt type = foam->foamANew.eltType;
	JavaCode sz = gj0Gen(foam->foamANew.size);

	return jcArrayNew(gj0TypeFrFmt(type, 0), sz);
}

local JavaCode
gj0Arr(Foam foam)
{
	switch (foam->foamArr.baseType) {
	case FOAM_Char:
		return gj0ArrChar(foam);
	default:
		return gj0Default(foam, 
				  strPrintf("Arr(%s)", 
					    foamStr(foam->foamArr.baseType)));
	}
}

local JavaCode
gj0ArrChar(Foam foam)
{
	int	i, arrSize;
	String	str;

	arrSize = foamArgc(foam);
	str = strAlloc(arrSize);
	for (i = 0; i < arrSize - 1; i++)
		str[i] = foam->foamArr.eltv[i];
	str[i] = '\0';
	
	return jcApplyMethodV(jcLiteralStringWithTerminalChar(str), jcId(strCopy("toCharArray")), 0);
}


local JavaCode
gj0AElt(Foam foam)
{
	JavaCode exprCode;
	FoamTag tag;
	AInt type;
	Bool needsCast;
	
	tag = gj0FoamExprTypeWFmt(foam->foamAElt.expr, &type);
	needsCast = (type == 0 || type == emptyFormatSlot);
	
	exprCode = gj0Gen(foam->foamAElt.expr);
	if (needsCast)
		exprCode = jcCast(gj0TypeFrFmt(FOAM_Arr, 
					       foam->foamAElt.baseType),
				  exprCode);

	return jcArrayRef(exprCode,
			  gj0Gen(foam->foamAElt.index));
}

local JavaCode
gj0AEltSet(Foam lhs, Foam rhs)
{
	return jcAssign(gj0Gen(lhs), gj0Gen(rhs));
}

/*
 * :: Record references
 */

local JavaCode
gj0RNew(Foam foam)
{
	gj0FmtUse(foam->foamRNew.format);
	return jcConstructV(gj0TypeFrFmt(FOAM_Rec, foam->foamRNew.format), 1,
			    gj0FmtId(foam->foamRNew.format));
}

local JavaCode
gj0Free(Foam foam)
{
	return gj0SeqNode(listNil(JavaCode));
}


local JavaCode
gj0RElt(Foam foam)
{
	AInt fmt = foam->foamRElt.format;
	AInt idx = foam->foamRElt.field;
	Foam ref = foam->foamRElt.expr;
	Foam innerRef = ref;
	Foam ddecl;
	foamDereferenceCast(innerRef);
	if (foamTag(innerRef) == FOAM_Nil) {
		JavaCode exception, value;
		Foam decl;
		ddecl = gjContext->formats->foamDFmt.argv[fmt];
		decl = ddecl->foamDDecl.argv[idx];
		/* call throwException(new NullPointerException())
		   and cast to ... */
		exception = jcConstructV(gj0Id(GJ_NullPointerException), 1,
					 jcLiteralString(strCopy("RElt")));
		value = jcCast(gj0TypeFrFmt(decl->foamDecl.type, decl->foamDecl.format),
			       jcApplyV(jcMemRef(gj0Id(GJ_Foam), jcId(strCopy("throwException"))),
					1, exception));

		return value;
	}
	ddecl = gjContext->formats->foamDFmt.argv[fmt];
	
	return gj0RecElt(gj0Gen(ref), ddecl, idx);
}

local JavaCode
gj0RecElt(JavaCode ref, Foam ddecl, int idx)
{
	JavaCode c;
	Foam decl = ddecl->foamDDecl.argv[idx];
	String id;

	id = decl->foamDecl.id;
	c = jcApplyMethodV(ref, jcId(strCopy("getField")), 
			   2, jcLiteralInteger(idx), 
			   jcLiteralString(strCopy(id)));
	c = gj0TypeValueToObj(c, decl->foamDecl.type, decl->foamDecl.format);
	return c;
}

local JavaCode
gj0REltSet(Foam lhs, Foam rhs)
{
	AInt fmt = lhs->foamRElt.format;
	AInt idx = lhs->foamRElt.field;
	Foam ref = lhs->foamRElt.expr;
	Foam ddecl = gjContext->formats->foamDFmt.argv[fmt];
	Foam decl = ddecl->foamDDecl.argv[idx];
	
	JavaCode rhsJ = gj0SetGenRhs(rhs, decl);
	return gj0RecSet(gj0Gen(ref), rhsJ, ddecl, idx);
}

local JavaCode
gj0RecSet(JavaCode lhsJ, JavaCode rhsJ, Foam ddecl, int idx)
{
	JavaCode c, valJ;
	Foam decl = ddecl->foamDDecl.argv[idx];
	String id;

	id = decl->foamDecl.id;

	valJ = gj0TypeObjToValue(rhsJ, decl->foamDecl.type, decl->foamDecl.format);
	c = jcApplyMethodV(lhsJ, jcId(strCopy("setField")), 3, 
			   jcLiteralInteger(idx), 
			   jcLiteralString(strCopy(id)),
			   valJ);

	return c;
}

/*
 * :: Environments & Lex
 */

local JavaCode
gj0Env(Foam foam)
{
	return gj0EnvId(foam->foamEnv.level);
}

local JavaCode
gj0Lex(Foam foam)
{
	AInt lvl = foam->foamLex.level;
	AInt idx = foam->foamLex.index;
	AInt fmt   = gjContext->prog->foamProg.levels->foamDEnv.argv[lvl];
	Foam ddecl = gjContext->formats->foamDFmt.argv[fmt];
	JavaCode lvlId = gj0LvlId(lvl);

	return gj0RecElt(lvlId, ddecl, idx);
}



local JavaCode
gj0LexSet(Foam foam, Foam rhs)
{
	AInt lvl = foam->foamLex.level;
	AInt idx = foam->foamLex.index;
	AInt fmt   = gjContext->prog->foamProg.levels->foamDEnv.argv[lvl];
	Foam ddecl = gjContext->formats->foamDFmt.argv[fmt];
	JavaCode lvlId = gj0LvlId(lvl);
	JavaCode rhsJ = gj0SetGenRhs(rhs, ddecl->foamDDecl.argv[idx]);

	return gj0RecSet(lvlId, rhsJ, ddecl, idx);
}

local JavaCode
gj0EElt(Foam foam)
{
	JavaCode env, ref, lvlJ;
	AInt fmt = foam->foamEElt.env;
	Foam ddecl = gjContext->formats->foamDFmt.argv[fmt];
	AInt idx = foam->foamEElt.lex;
	AInt lvl = foam->foamEElt.level;
	
	if (lvl == 0)
		env = gj0Gen(foam->foamEElt.ref);
	else 
		env = jcApplyMethodV(gj0Gen(foam->foamEElt.ref),
				      jcId(strCopy("nthParent")),
				      1, jcLiteralInteger(lvl));

	lvlJ = jcApplyMethodV(env, jcId(strCopy("level")), 0);
	ref = gj0RecElt(lvlJ, ddecl, idx);

	return ref;
}

local JavaCode
gj0EEnv(Foam foam)
{
	JavaCode env;
	AInt lvl = foam->foamEEnv.level;
	
	if (lvl == 0)
		env = gj0Gen(foam->foamEEnv.env);
	else 
		env = jcApplyMethodV(gj0Gen(foam->foamEEnv.env),
				      jcId(strCopy("nthParent")),
				      1, jcLiteralInteger(lvl));
	return env;
}

local JavaCode
gj0EEnsure(Foam foam)
{
	JavaCode stmt;
	Foam env = foam->foamEEnsure.env;

	stmt = jcApplyMethodV(gj0Gen(env),
			      jcId(strCopy("ensure")),
			      0);
	return stmt;
}


local JavaCode
gj0EEltSet(Foam foam, Foam rhs)
{
	JavaCode env, ref, rhsJ, lvlJ;
	AInt fmt = foam->foamEElt.env;
	Foam ddecl = gjContext->formats->foamDFmt.argv[fmt];
	AInt idx = foam->foamEElt.lex;
	AInt lvl = foam->foamEElt.level;

	if (lvl == 0)
		env = gj0Gen(foam->foamEElt.ref);
	else
		env = jcApplyMethodV(gj0Gen(foam->foamEElt.ref),
				     jcId(strCopy("nthParent")),
				     1, jcLiteralInteger(lvl));
	lvlJ = jcApplyMethodV(env, jcId(strCopy("level")), 0);
	rhsJ = gj0SetGenRhs(rhs, ddecl->foamDDecl.argv[idx]);
	ref = gj0RecSet(lvlJ, rhsJ, ddecl, idx);

	return ref;
}

local JavaCode
gj0EInfo(Foam foam)
{
	JavaCode stmt;
	Foam env = foam->foamEInfo.env;

	stmt = jcApplyMethodV(gj0Gen(env),
			      jcId(strCopy("getInfo")),
			      0);
	return stmt;
}

local JavaCode
gj0EInfoSet(Foam foam, Foam rhs)
{
	JavaCode stmt;
	Foam env = foam->foamEInfo.env;

	stmt = jcApplyMethodV(gj0Gen(env),
			      jcId(strCopy("setInfo")),
			      1, gj0Gen(rhs));
	return stmt;
}


local JavaCode
gj0PushEnv(Foam foam)
{
	gj0FmtUse(foam->foamPushEnv.format);
	return jcConstructV(gj0TypeFrFmt(FOAM_Env, foam->foamPushEnv.format),
			    2,
			    jcConstructV(gj0Id(GJ_EnvRecord), 1,
					 gj0FmtId(foam->foamPushEnv.format)),
			    gj0Gen(foam->foamPushEnv.parent));
}


local JavaCode
gj0EnvId(int lvl)
{
	return jcId(strPrintf("env%d", lvl));
}

local JavaCode
gj0LvlId(int lvl)
{
	return jcId(strPrintf("lvl%d", lvl));
}

/*
 * :: Closures
 */


local JavaCode
gj0CEnv(Foam foam)
{
	return jcApplyMethodV(gj0Gen(foam->foamCEnv.env), 
			      jcId(strCopy("getEnv")), 0);
}

local JavaCode
gj0CProg(Foam foam)
{
	return jcApplyMethodV(gj0Gen(foam->foamCProg.prog), 
			      jcId(strCopy("getProg")), 0);
}

local JavaCode
gj0CEnvSet(Foam foam, Foam rhs)
{
	return jcApplyMethodV(gj0Gen(foam->foamCEnv.env), 
			      jcId(strCopy("setEnv")), 1,
			      gj0Gen(rhs)
		);
}

local JavaCode
gj0CProgSet(Foam foam, Foam rhs)
{
	return jcApplyMethodV(gj0Gen(foam->foamCProg.prog), 
			      jcId(strCopy("setProg")), 1,
			      gj0Gen(rhs)
		);
}

local JavaCode
gj0Clos(Foam foam)
{
	Foam env  = foam->foamClos.env;
	Foam prog = foam->foamClos.prog;

	assert(foamTag(prog) == FOAM_Const);
	return jcConstructV(gj0TypeFrFmt(FOAM_Clos, 0),
			    2, gj0Gen(env), 
			    jcId(gj0ProgFnName(prog->foamConst.index)));
}

/*
 * :: ProgRefs
 */

local JavaCode
gj0PRef(Foam foam)
{
	return jcApplyMethodV(gj0Gen(foam->foamPRef.prog), jcId(strCopy("getInfo")),
			      1, jcLiteralInteger(foam->foamPRef.idx));
}

local JavaCode
gj0PRefSet(Foam lhs, Foam rhs)
{
	return jcApplyMethodV(gj0Gen(lhs->foamPRef.prog), jcId(strCopy("setInfo")),
			      2, 
			      jcLiteralInteger(lhs->foamPRef.idx),
			      gj0Gen(rhs));
}

/*
 * :: Consts
 */

local JavaCode
gj0Const(Foam foam)
{
	return gj0Nil(foam);
}


/*
 * :: Record formats
 * includes envs and mutis
 */
local JavaCode gj0FmtInit(int idx);

typedef struct fmtSet
{
	BitvClass clss;
	Bitv      bitv;
} *GjFmtSet;

local void
gj0FmtUse(AInt ddeclIdx)
{
	intSetAdd(gjContext->fmtSet, ddeclIdx);
}

local JavaCode
gj0FmtId(AInt ddeclIdx)
{
	return jcId(strPrintf("FORMAT_%d", ddeclIdx));
}

local JavaCodeList
gj0FmtInits() 
{
	JavaCodeList inits;
	int i;

	inits = listNil(JavaCode);
	for (i=0; i<foamArgc(gjContext->formats); i++) {
		if (intSetMember(gjContext->fmtSet, i))
			inits = listCons(JavaCode)(gj0FmtInit(i), inits);
	}
	
	return listNReverse(JavaCode)(inits);
}

local JavaCode
gj0FmtInit(int idx)
{
	Foam ddecl;
	JavaCode rhs, init;

	ddecl = gjContext->formats->foamDFmt.argv[idx];
	rhs = jcConstructV(gj0Id(GJ_Format), 1,
			   jcLiteralInteger(foamDDeclArgc(ddecl)));
	init = jcInitialisation(JCO_MOD_Static|JCO_MOD_Private,
				gj0Id(GJ_Format),
				gj0FmtId(idx),
				rhs);
	return jcStatement(init);
}

/*
 * :: CCall stub functions
 */

local String      gj0CCallStubParam(int idx);
local FoamTag *gj0FoamSigRets(Foam ddecl, int *nVals);

local FoamSig
gj0FoamSigFrCCall(Foam ccall)
{
	FoamTag *retVals;
	AIntList inArgList;
	FoamSig sig;
	int nRets;

	inArgList = listNil(AInt);
	foamIter(ccall, elt, {
			FoamTag type = gj0FoamExprType(*elt);
			inArgList = listCons(AInt)(type, inArgList);
		});
	/* FIXME: Not sure how to get return types. */

	inArgList = listNReverse(AInt)(inArgList);
	if (ccall->foamCCall.type == FOAM_NOp && gjContext->mfmt != 0) {
		Foam ddecl = gjContext->formats->foamDFmt.argv[gjContext->mfmt];
		retVals   = gj0FoamSigRets(ddecl, &nRets);
	}
	else {
		retVals = 0;
		nRets = 0;
	}

	sig = foamSigNew(cdr(inArgList), ccall->foamCCall.type, nRets, retVals);
	listFreeCons(AInt)(inArgList);
	return sig;
}

local FoamTag *
gj0FoamSigRets(Foam ddecl, int *nVals)
{
	FoamTag *arr = (FoamTag*) stoAlloc(OB_Other, sizeof(FoamTag)*foamDDeclArgc(ddecl));
	int i;

	for (i=0; i<foamDDeclArgc(ddecl); i++) {
		arr[i] = ddecl->foamDDecl.argv[i]->foamDecl.type;
	}

	*nVals = foamDDeclArgc(ddecl);
	return arr;
}


local JavaCodeList
gj0CCallStubGen(FoamSigList sigs)
{
	JavaCodeList fns = listNil(JavaCode);

	while (sigs != listNil(FoamSig)) {
		JavaCode c = gj0CCallStubGenFrSig(car(sigs));
		fns = listCons(JavaCode)(c, fns);
		sigs = cdr(sigs);
	}
	return listNReverse(JavaCode)(fns);
}



/* unconditionally eats its argument 
 * FIXME: Quadratic in number of distinct
 * signatures (which is probably very small).
 */
local FoamSigList
gj0CCallStubAdd(FoamSigList list, FoamSig sig)
{
	FoamSigList tmp = list;
	while (tmp) {
		FoamSig osig = car(tmp);
		if (foamSigEqual(sig, osig))
			break;
		tmp = cdr(tmp);
	}
	if (tmp == listNil(FoamSig)) {
		list = listCons(FoamSig)(sig, list);
	}
	else {
		foamSigFree(sig);
	}
	return list;
}


/*
	static Word callWxWWW(Closure a, Word b, Word c) {
L2		Value r = a.call(b.toValue(), c.toValue());
L3		return r.asWord();
	}
*/

local JavaCode
gj0CCallStubGenFrSig(FoamSig sig)
{
	JavaCodeList valList, paramList;
	JavaCode l1, l2, call;
	JavaCode resultVar, retnType, body;
	AIntList argList = sig->inArgs;
	int idx;

	valList = listNil(JavaCode);
	paramList = listNil(JavaCode);
	idx = 0;
	while (argList != listNil(AInt)) {
		AInt type = car(argList);
		String   id   = gj0CCallStubParam(idx+1);
		JavaCode decl = jcParamDecl(0, gj0TypeFrFmt(type, 0), 
					    jcId(strCopy(id)));
		JavaCode asValue = gj0TypeObjToValue(jcId(strCopy(id)), type, 0);

		valList   = listCons(JavaCode)(asValue, valList);
		paramList = listCons(JavaCode)(decl, paramList);
		strFree(id);

		argList = cdr(argList);
		idx++;
	}
	
	valList = listNReverse(JavaCode)(valList);
	paramList = listNReverse(JavaCode)(paramList);

	paramList = listCons(JavaCode)(jcParamDecl(0, gj0TypeFrFmt(FOAM_Clos, 0),
						   jcId(gj0CCallStubParam(0))),
				       paramList);

	call = jcApplyMethod(jcId(gj0CCallStubParam(0)), 
				   jcId(strCopy("call")), valList);
	if (sig->retType == FOAM_NOp && sig->nRets == 0) {
		l1 = call;
		l2 = jcReturnVoid();
		body = jcNLSeqV(2, jcStatement(l1), jcStatement(l2));
	}
	else if (sig->nRets > 1) {
		JavaCode tmp;
		resultVar = jcId(strCopy("result"));

		l1 = jcInitialisation(0, gj0Id(GJ_FoamValue), jcoCopy(resultVar),
				      call);
		tmp = jcApplyMethodV(jcoCopy(resultVar), jcId(strCopy("toMulti")), 0);
		l2 = jcReturn(tmp);
		body = jcNLSeqV(2, 
				jcStatement(l1), 
				jcStatement(l2));
	}
	else {
		resultVar = jcId(strCopy("result"));
		l1 = jcInitialisation(0, gj0Id(GJ_FoamValue), jcoCopy(resultVar),
				      call);
		l2 = jcReturn(gj0TypeValueToObj(jcoCopy(resultVar), sig->retType, 0));
		jcoFree(resultVar);
		body = jcNLSeqV(2, jcStatement(l1), jcStatement(l2));
	}

	retnType = sig->nRets == 0 
		? gj0TypeFrFmt(sig->retType, 0)
		: gj0Id(GJ_Multi);
	
	return jcMethod(JCO_MOD_Private | JCO_MOD_Static, NULL,
			retnType, jcId(gj0CCallStubName(sig)),
			listNil(JavaCode), paramList,
			listNil(JavaCode),
			body);
}


local String
gj0CCallStubParam(int idx)
{
	return strPrintf("_%d", idx);
}


local String
gj0CCallStubName(FoamSig call) 
{
	AIntList tmp;
	String name = strCopy("ccall_");
	String res;
	String suffix;
	int i;

	if (call->nRets == 0)
		suffix = strCopy(gj0TypeAbbrev(call->retType));
	else {
		suffix = strCopy("");
		for (i=0; i<call->nRets; i++) {
			suffix = strNConcat(suffix, gj0TypeAbbrev(call->rets[i]));
		}
	}
	suffix = strNConcat(suffix, "x");
	tmp = call->inArgs;
	while (tmp != listNil(AInt)) {
		suffix=strNConcat(suffix, gj0TypeAbbrev(car(tmp)));
		tmp = cdr(tmp);
	}

	res = strNConcat(name, suffix);
	strFree(suffix);
	return res;
}

/*
 * :: Cast
 */

local JavaCode gj0CastWordToObj(JavaCode jc, FoamTag type, AInt fmt);
local JavaCode gj0CastObjToWord(JavaCode jc, FoamTag type, AInt fmt);
local JavaCode gj0CastObjToPtr(JavaCode jc, FoamTag type, AInt fmt);

local JavaCode
gj0Cast(Foam foam)
{
	return gj0CastFmt(foam, emptyFormatSlot);
}

local JavaCode
gj0CastFmt(Foam foam, AInt cfmt)
{
	Foam ref     = foam->foamCast.expr;
	FoamTag type = foam->foamCast.type;
	AInt    fmt;
	FoamTag iType = gj0FoamExprTypeWFmt(ref, &fmt);
	JavaCode jc = gj0Gen(ref);
	
	if (cfmt != -1)
		fmt = cfmt;
	if (type == FOAM_Word) {
		return gj0CastObjToWord(jc, iType, fmt);
	}
	else if (type == FOAM_Ptr) {
		return gj0CastObjToPtr(jc, iType, fmt);
	}
	else if (iType == FOAM_Word) {
		return gj0CastWordToObj(jc, type, cfmt);
	}
	else if (iType == type)
		return jc;
	else if (iType == FOAM_Nil)
		return jcNull();
	else if ((iType == FOAM_Clos && type == FOAM_Rec)
		 ||(iType == FOAM_Arr && type == FOAM_SInt)) {
		JavaCode exception = jcConstructV(gj0Id(GJ_ClassCastException), 1,
						  jcLiteralString(aStrPrintf("%s to %s", foamStr(iType), foamStr(type))));
		JavaCode throw = jcApplyV(jcMemRef(gj0Id(GJ_Foam), jcId(strCopy("throwException"))),
					  1, exception);
		return jcCast(gj0TypeFrFmt(type, 0), throw);

	}
	else  {
		return gj0Default(foam,
			   strPrintf("No cast: %s, %s", foamStr(type), foamStr(iType)));
	}
}

/*
 * :: Casts
 */


local JavaCode
gj0CastWordToObj(JavaCode jc, FoamTag type, AInt fmt)
{
	/* Rely on Word == Value */
	switch (type) {
	case FOAM_Bool:
		return jcApplyMethodV(jc, jcId(strCopy("toBool")), 0);
	case FOAM_Byte:
		return jcApplyMethodV(jc, jcId(strCopy("toByte")), 0);
	case FOAM_SInt:
		return jcApplyMethodV(jc, jcId(strCopy("toSInt")), 0);
	case FOAM_HInt:
		return jcApplyMethodV(jc, jcId(strCopy("toHInt")), 0);
	case FOAM_BInt:
		return jcApplyMethodV(jc, jcId(strCopy("toBInt")), 0);
	case FOAM_Char:
		return jcApplyMethodV(jc, jcId(strCopy("toChar")), 0);
	case FOAM_SFlo:
		return jcApplyMethodV(jc, jcId(strCopy("toSFlo")), 0);
	case FOAM_DFlo:
		return jcApplyMethodV(jc, jcId(strCopy("toDFlo")), 0);
	case FOAM_Ptr:
		return jc;
	case FOAM_Arr:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamWord),
					       jcId(strCopy("U"))),
				      jcId(strCopy("toArray")), 1, jc);
	case FOAM_JavaObj:
		return jcApplyMethodV(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				      jcId(strCopy("toJavaObj")), 1, jc);
	default:
		return jcCast(gj0TypeFrFmt(type, fmt), jc);
	}
}

local JavaCode
gj0CastObjToWord(JavaCode val, FoamTag type, AInt fmt)
{
	switch (type) {
	case FOAM_Clos:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromClos")),
				     listSingleton(JavaCode)(val));
	case FOAM_SInt:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromSInt")),
				     listSingleton(JavaCode)(val));
							   
	case FOAM_Bool:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromBool")),
				     listSingleton(JavaCode)(val));
	case FOAM_Arr:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromArray")),
				     listSingleton(JavaCode)(val));
	case FOAM_Ptr:
		return jcCast(gj0Id(GJ_FoamWord), val);
	case FOAM_Rec:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromRec")),
				     listSingleton(JavaCode)(val));
	case FOAM_Char:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromChar")),
				     listSingleton(JavaCode)(val));
	case FOAM_Byte:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromByte")),
				     listSingleton(JavaCode)(val));
	case FOAM_HInt:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromHInt")),
				     listSingleton(JavaCode)(val));
	case FOAM_BInt:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromBInt")),
				     listSingleton(JavaCode)(val));
	case FOAM_SFlo:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromSFlo")),
				     listSingleton(JavaCode)(val));
	case FOAM_DFlo:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromDFlo")),
				     listSingleton(JavaCode)(val));
	case FOAM_JavaObj:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamWord),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromJavaObj")),
				     listSingleton(JavaCode)(val));
	case FOAM_Nil:
		return val;
	default:
		return jcCast(jcSpaceSeqV(2, gj0Id(GJ_FoamWord),
					  jcComment(strCopy(foamStr(type)))),
			      val);
	}
}

local JavaCode
gj0CastObjToPtr(JavaCode val, FoamTag type, AInt fmt)
{
	return val;
}

/*
 * :: Class level boilerplate
 */

local JavaCodeList gj0ClassFields();
local JavaCode     gj0ClassMainMethod(String className);
local JavaCode     gj0ClassConstructor(String className);
local JavaCode     gj0ClassRunMethod(String className);
local int          gj0ClassFindInitDeclIdx();
local Foam         gj0ClassFindInitDef();
local JavaCode     gj0UtilCCall(JavaCode c, JavaCodeList args);

local JavaCodeList
gj0ClassHeader(String className)
{
	JavaCodeList list, fields;
	JavaCode constructor;
	JavaCode runMethod;

	list = listNil(JavaCode);
	fields      = gj0ClassFields();
	constructor = gj0ClassConstructor(className);
	runMethod   = gj0ClassRunMethod(className);

	list = listNConcat(JavaCode)(fields, list);
	list = listCons(JavaCode)(constructor, list);
	list = listCons(JavaCode)(runMethod, list);

	if (gjArgs->createMain)
		list = listCons(JavaCode)(gj0ClassMainMethod(className), list);
	
	return listNReverse(JavaCode)(list);
}

local JavaCodeList
gj0ClassFields()
{
	JavaCodeList vars;
	JavaCode ctxtDecl, declJ;
	int i;

	/* private FoamContext ctxt;*/
	ctxtDecl = jcParamDecl(JCO_MOD_Private,
			       gj0Id(GJ_FoamContext), gj0Id(GJ_ContextVar));
	
	vars = listNil(JavaCode);
	for (i=0; i< foamDDeclArgc(gjContextGlobals); i++) {
		Foam decl = gjContextGlobal(i);
		
		if (decl->foamGDecl.protocol != FOAM_Proto_Init)
			continue;
		declJ = jcParamDecl(JCO_MOD_Private,
				    gj0TypeFrFmt(FOAM_Clos, 0),
				    jcId(gj0InitVar(i)));
		vars = listCons(JavaCode)(jcStatement(declJ), vars);
	}


	return listCons(JavaCode)(jcStatement(ctxtDecl), vars);
				  
}

local JavaCode
gj0ClassConstructor(String className)
{
	/* public foo(FoamContext ctxt) {
	 *     this.ctxt = ctxt;
	 *     <init_foo> = new Clos(null, CFX);
	 *     <init_*>   = ctxt.createLoadFn(ctxt)
	 * }
         */
	JavaCodeList args;
	JavaCodeList body, inits;
	JavaCode s1, declJ, rhs;
	Foam decl;
	int i;

	args = listSingleton(JavaCode)(jcParamDecl(0, gj0Id(GJ_FoamContext), gj0Id(GJ_ContextVar)));
	s1 = jcAssign(jcMemRef(jcThis(), gj0Id(GJ_ContextVar)), gj0Id(GJ_ContextVar));
	
	s1 = jcStatement(s1);
	
	inits = listNil(JavaCode);
	for (i = 0; i < foamDDeclArgc(gjContextGlobals); i++) {
		decl = gjContextGlobal(i);
		
		if (decl->foamGDecl.protocol != FOAM_Proto_Init)
			continue;
		if (decl->foamGDecl.dir == FOAM_GDecl_Import) 
			rhs = jcApplyMethodV(gj0Id(GJ_ContextVar), 
					     jcId(strCopy("createLoadFn")),
					     1, jcLiteralString(strCopy(decl->foamGDecl.id)));
		else 
			rhs = jcConstructV(gj0TypeFrFmt(FOAM_Clos, 0),
					   2, jcNull(), 
					   jcId(gj0ProgFnName(0)));
		declJ = jcAssign(jcId(gj0InitVar(i)), rhs);
		inits = listCons(JavaCode)(jcStatement(declJ), inits);
	}

	body = listCons(JavaCode)(s1, listNReverse(JavaCode)(inits));
	return jcConstructor(JCO_MOD_Public, NULL,
			     jcId(strCopy(className)),
			     listNil(JavaCode), args,
			     listNil(JavaCode), 
			     jcNLSeq(body));
}

local JavaCode
gj0ClassRunMethod(String className)
{
	/* public void run() {
	 *    c0_langx(null)
	 *    Clos c = ctxt.getGlobal(basic)
	 *    c.getProg().ocall(c.getEnv());
	 * }
         */
	
	JavaCode s1;
	int idx;

	idx = gj0ClassFindInitDeclIdx();
	s1 = gj0UtilCCall(jcId(gj0InitVar(idx)), listNil(JavaCode));

	return jcMethod(JCO_MOD_Public, NULL, 
			jcKeyword(symInternConst("void")),
			jcId(strCopy("run")), 
			listNil(JavaCode), listNil(JavaCode),listNil(JavaCode),
			jcStatement(s1));
}


local int
gj0ClassFindInitDeclIdx()
{
	int i;
	for (i=0; i<foamArgc(gjContextGlobals); i++) {
		Foam decl = gjContextGlobal(i);
		if (decl->foamGDecl.dir == FOAM_GDecl_Export
		    && decl->foamGDecl.protocol == FOAM_Proto_Init) {
			return i;
		}
	}
	return -1;
}

local Foam
gj0ClassFindInitDef()
{
	int i;
	for (i=0; i<foamArgc(gjContext->defs); i++) {
		Foam def = gjContext->defs->foamDDef.argv[i];
		if (foamTag(def) != FOAM_Def)
			continue;
		if (foamTag(def->foamDef.lhs) != FOAM_Glo)
			continue;
		return def;
	}
	return NULL;
}


local JavaCode 
gj0UtilCCall(JavaCode c, JavaCodeList args)
{
	JavaCode prog, env, call;
	prog = jcApplyMethodV(jcoCopy(c), jcId(strCopy("getProg")), 0);
	env  = jcApplyMethodV(jcoCopy(c), jcId(strCopy("getEnv")), 0);
	args = listCons(JavaCode)(env, args);
	call = jcApplyMethod(prog, jcId(strCopy("ocall")), args);

	jcoFree(c);
	return call;
}



local JavaCode
gj0ClassMainMethod(String className)
{
	JavaCodeList args;
	JavaCode body, s1, s2, s3;
	JavaCode instance, ctxt, method;
	int modifiers;

	instance = jcId(strCopy("instance"));
	ctxt = gj0Id(GJ_ContextVar);
	s1 = jcInitialisation(0, gj0Id(GJ_FoamContext), jcoCopy(ctxt),
			      jcConstructV(gj0Id(GJ_FoamContext), 0));
	s2 = jcInitialisation(0, gj0Id(GJ_FoamClass), jcoCopy(instance), 
			      jcConstructV(jcId(strCopy(className)), 
					   1, jcoCopy(ctxt)));

	s3 = jcApplyMethodV(jcoCopy(ctxt), jcId(strCopy("startFoam")), 2,
			    jcoCopy(instance), jcId(strCopy("args")));

	body = jcNLSeqV(3, jcStatement(s1), jcStatement(s2), jcStatement(s3));
	modifiers = JCO_MOD_Public | JCO_MOD_Static;
	args = listList(JavaCode)(1, 
				  jcParamDecl(0, jcArrayOf(gj0Id(GJ_String)), 
					      jcId(strCopy("args"))));

	jcoFree(ctxt);
	jcoFree(instance);
	method =  jcMethod(modifiers, strCopy("main method"),
			   jcKeyword(symInternConst("void")),
			   gj0Id(GJ_Main), listNil(JavaCode), args, listNil(JavaCode),
			   body);
	return method;
}


/*
 * :: Initialisation vars
 */

local String
gj0InitVar(AInt idx)
{
	Foam gdecl = gjContextGlobal(idx);
	return strPrintf("%s_init", gdecl->foamGDecl.id);
}


/*
 * :: Ids.
 * Collected together for readability
 */

struct gjIdInfo {
	GjId id;
	String pkg;
	String name;
};

struct gjIdInfo gjIdInfo[] = {
	{GJ_Foam,       "foamj", "Foam"},
	{GJ_FoamWord,   "foamj", "Word"},
	{GJ_FoamClos,   "foamj", "Clos"},
	{GJ_FoamRecord, "foamj", "Record"},
	{GJ_FoamEnv,    "foamj", "Env"},
	{GJ_FoamClass,  "foamj", "FoamClass"},
	{GJ_FoamContext,"foamj", "FoamContext"},

	{GJ_FoamFn,     "foamj", "Fn"},

	{GJ_FoamValue,  "foamj", "Value"},
	{GJ_Multi,      "foamj", "MultiRecord"},
	{GJ_FoamGlobals,"foamj", "Globals"},
	{GJ_Format,     "foamj", "Format"},
	{GJ_EnvRecord,  "foamj", "EnvRecord"},

	{GJ_Object,     0, "Object"},
	{GJ_String,     0, "String"},
	{GJ_BigInteger, "java.math", "BigInteger"},
	{GJ_NullPointerException, 0, "NullPointerException"},
	{GJ_ClassCastException, 0, "ClassCastException"},

	{GJ_ContextVar, 0, "ctxt"},
	{GJ_Main,       0, "main"},
	{GJ_INVALID,    0, 0 },
};


local JavaCode
gj0Id(GjId id) 
{
	struct gjIdInfo *info;
	JavaCode javaId;
	assert(gjIdInfo[GJ_LIMIT].id == GJ_INVALID);

	info = &gjIdInfo[id];
	assert(id == info->id);

	if (info->pkg != 0)
		javaId = jcImportedId(strCopy(info->pkg),
				    strCopy(info->name));
	else
		javaId = jcId(strCopy(info->name));

	assert(!jcoIsEmpty(javaId));
	return javaId;
}



/*
 * :: PCall
 */
local JavaCode gj0PCallOther(Foam foam);
local JavaCode gj0PCallJavaMethod(Foam foam);
local JavaCode gj0PCallJavaConstructor(Foam foam);
local JavaCode gj0PCallJavaStatic(Foam foam);
local JavaCodeList gj0PCallCastArgs(Foam op, JavaCodeList args);

local JavaCode
gj0PCall(Foam foam)
{
	switch (foam->foamPCall.protocol) {
	case FOAM_Proto_Other:
	case FOAM_Proto_C:
		return gj0PCallOther(foam);
	case FOAM_Proto_JavaMethod:
		return gj0PCallJavaMethod(foam);
	case FOAM_Proto_JavaConstructor:
		return gj0PCallJavaConstructor(foam);
	case FOAM_Proto_Java:
		return gj0PCallJavaStatic(foam);
	default:
		return jcSpaceSeqV(2, jcNull(), jcComment(strPrintf("%pFoam", foam)));
	}
}

local JavaCode
gj0PCallOther(Foam foam)
{
	JavaCodeList args;
	Foam decl, op;

	op = foam->foamPCall.op;

	assert(foamTag(op) == FOAM_Glo);

	decl = gjContextGlobal(op->foamGlo.index);
	args = gj0GenList(foam->foamPCall.argv, foamPCallArgc(foam));
	return jcApplyMethod(gj0Id(GJ_Foam), jcId(strCopy(decl->foamGDecl.id)),
			     args);
}

local JavaCode
gj0PCallJavaMethod(Foam foam)
{
	JavaCodeList args;
	JavaCode target;
	Foam decl, op;
	String type, opName, pkg;

	op = foam->foamPCall.op;

	assert(foamTag(op) == FOAM_Glo);

	assert(foamPCallArgc(foam) > 0);

	decl = gjContextGlobal(op->foamGlo.index);
	args = gj0GenList(foam->foamPCall.argv+1, foamPCallArgc(foam)-1);
	target = gj0Gen(foam->foamPCall.argv[0]);

	strSplitLast(strCopy(decl->foamGDecl.id), '.', &type, &opName);
	assert(type != 0);
	strSplitLast(type, '.', &pkg, &type);

	return jcApplyMethod(jcCast(jcImportedId(pkg, type), target),
			     jcId(opName),
			     gj0PCallCastArgs(op, args));
}

local JavaCode
gj0PCallJavaConstructor(Foam foam)
{
	JavaCodeList args;
	Foam decl, op;
	String type, pkg;

	op = foam->foamPCall.op;

	assert(foamTag(op) == FOAM_Glo);

	assert(foamPCallArgc(foam) > 1);

	decl = gjContextGlobal(op->foamGlo.index);
	args = gj0GenList(foam->foamPCall.argv, foamPCallArgc(foam));

	strSplitLast(strCopy(decl->foamGDecl.id), '.', &pkg, &type);

	return jcConstruct(jcImportedId(pkg, type), gj0PCallCastArgs(op, args));
}


local JavaCode
gj0PCallJavaStatic(Foam foam)
{
	JavaCodeList args;
	Foam decl, op;
	String id, type;

	op = foam->foamPCall.op;

	assert(foamTag(op) == FOAM_Glo);

	decl = gjContextGlobal(op->foamGlo.index);
	args = gj0GenList(foam->foamPCall.argv, foamPCallArgc(foam));

	strSplitLast(strCopy(decl->foamGDecl.id), '.', &type, &id);
	JavaCode typeId = jcImportedIdFrString(type);

	return jcApply(jcMemRef(typeId, jcId(id)),
		       gj0PCallCastArgs(op, args));
}

local JavaCodeList
gj0PCallCastArgs(Foam op, JavaCodeList argsIn)
{
	JavaCodeList args = argsIn;
	Foam glo = gjContextGlobals->foamDDecl.argv[op->foamGlo.index];
	Foam ddecl = gjContext->formats->foamDFmt.argv[glo->foamGDecl.format];
	int i = 1;

	assert(ddecl->foamDDecl.usage == FOAM_DDecl_JavaSig);
	assert(foamDDeclArgc(ddecl) == listLength(JavaCode)(argsIn));

	/* Cast java-valued arguments - all other types are not converted */
	while (args != listNil(JavaCode)) {
		Foam decl = ddecl->foamDDecl.argv[i];
		String pkg, type;
		if (decl->foamDecl.type == FOAM_Ptr) {
			strSplitLast(strCopy(decl->foamGDecl.id), '.', &pkg, &type);
			car(args) = jcCast(jcImportedId(pkg, type), car(args));
		}
		args = cdr(args);
		i++;
	}

	return argsIn;
}


/*
 * :: BCall
 */

enum gj_BCallMethod {
	GJ_Keyword, 
	GJ_Apply,
	GJ_Meth,
	GJ_Op,
	GJ_OpMod,
	GJ_LitChar,
	GJ_LitString,
	GJ_LitFloat,
	GJ_LitInt,
	GJ_Const,
	GJ_NegConst,
	GJ_Cast,
	GJ_Exception,
	GJ_NotImpl
};

typedef Enum(gj_BCallMethod) GJ_BCallMethod;

struct gjBVal_info {
	FoamBValTag tag;
	GJ_BCallMethod method;
	AInt gjTag;
	String c1;
	String c2;
};

typedef struct gjBVal_info *GJBValInfo;

local GJBValInfo gj0BCallBValInfo(FoamBValTag tag);

local JavaCode gj0BCallKeyword (Foam foam);
local JavaCode gj0BCallApply   (Foam foam);
local JavaCode gj0BCallMethod  (Foam foam);
local JavaCode gj0BCallOp      (Foam foam);
local JavaCode gj0BCallOpMod   (Foam foam);
local JavaCode gj0BCallLitChar (Foam foam);
local JavaCode gj0BCallLitString(Foam foam);
local JavaCode gj0BCallLitFloat(Foam foam);
local JavaCode gj0BCallLitInt  (Foam foam);
local JavaCode gj0BCallConst   (Foam foam);
local JavaCode gj0BCallNegConst(Foam foam);
local JavaCode gj0BCallCast    (Foam foam);
local JavaCode gj0BCallException(Foam foam);
local JavaCode gj0BCallNotImpl (Foam foam);

struct gjBVal_info gjBValInfoTable[];
struct gjBVal_info gjBValNotImpl;

local JavaCode
gj0BCall(Foam foam) 
{
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	switch (inf->method) {
	case GJ_Keyword:
		return gj0BCallKeyword(foam);
	case GJ_Apply:
		return gj0BCallApply(foam);
	case GJ_Meth:
		return gj0BCallMethod(foam);
	case GJ_Op:
		return gj0BCallOp(foam);
	case GJ_OpMod:
		return gj0BCallOpMod(foam);
	case GJ_LitChar:
		return gj0BCallLitChar(foam);
	case GJ_LitString:
		return gj0BCallLitString(foam);
	case GJ_LitFloat:
		return gj0BCallLitFloat(foam);
	case GJ_LitInt:
		return gj0BCallLitInt(foam);
	case GJ_Const:
		return gj0BCallConst(foam);
	case GJ_NegConst:
		return gj0BCallNegConst(foam);
	case GJ_Cast:
		return gj0BCallCast(foam);
	case GJ_NotImpl:
		return gj0BCallNotImpl(foam);
	case GJ_Exception:
		return gj0BCallException(foam);
	default:
		return gj0Default(foam, strCopy("BCallType"));
	}
}

local JavaCode
gj0BCallKeyword(Foam foam)
{
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	return jcKeyword(symInternConst(inf->c1));
}

local JavaCode
gj0BCallApply(Foam foam)
{
	JavaCodeList args;
	JavaCode tgtClss;
	GJBValInfo inf;
	CString p;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	args = gj0GenList(foam->foamBCall.argv, foamArgc(foam)-1);

	tgtClss = jcImportedIdFrString(inf->c1);

	return jcApplyMethod(tgtClss, jcId(strCopy(inf->c2)), args);
}

local JavaCode
gj0BCallMethod(Foam foam)
{
	JavaCodeList args;
	JavaCode obj;
	GJBValInfo inf;
	
	inf = gj0BCallBValInfo(foam->foamBCall.op);
	args = gj0GenList(foam->foamBCall.argv, foamArgc(foam)-1);
	obj = car(args);
	args = listFreeCons(JavaCode)(args);

	return jcApplyMethod(obj, jcId(strCopy(inf->c1)), args);
}

local JavaCode
gj0BCallOp(Foam foam) 
{
	JavaCodeList args;
	JavaCode r;
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	args = gj0GenList(foam->foamBCall.argv, foamArgc(foam)-1);
	
	if (inf->c1 != 0) {
		JavaCode arg2 = jcLiteralIntegerFrString(strCopy(inf->c1));
		args = listNConcat(JavaCode)(args, listSingleton(JavaCode)(arg2));
	}
	r = jcOp(inf->gjTag, args);
	
	return r;
}

local JavaCode
gj0BCallOpMod(Foam foam)
{
	JavaCodeList args;
	JavaCode r, extraArg;
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	args = gj0GenList(foam->foamBCall.argv, foamArgc(foam)-1);
	assert(listLength(JavaCode)(args) == 3);

	extraArg = listElt(JavaCode)(args, 2);
	args = listList(JavaCode)(2, car(args), car(cdr(args)));
	r = jcOp(inf->gjTag, args);

	return jcBinOp(JCO_OP_Modulo, r, extraArg);
}

local JavaCode
gj0BCallLitChar(Foam foam)
{
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	
	return jcLiteralChar(strCopy(inf->c1));
}

local JavaCode
gj0BCallLitString(Foam foam) 
{
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	
	return jcLiteralString(strCopy(inf->c1));
}

local JavaCode
gj0BCallLitFloat(Foam foam)
{
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	
	return jcLiteralFloatFrString(strCopy(inf->c1));
}

local JavaCode
gj0BCallLitInt(Foam foam)
{
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	
	return jcLiteralIntegerFrString(strCopy(inf->c1));
}

local JavaCode
gj0BCallConst(Foam foam)
{
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	return jcMemRef(jcId(strCopy(inf->c1)), 
			jcId(strCopy(inf->c2)));
}

local JavaCode
gj0BCallNegConst(Foam foam)
{
	GJBValInfo inf;
	JavaCode c;
	inf = gj0BCallBValInfo(foam->foamBCall.op);
	c = jcMemRef(jcId(strCopy(inf->c1)), 
		     jcId(strCopy(inf->c2)));
	return jcOp(JCO_OP_Negate, listSingleton(JavaCode)(c));
}

local JavaCode
gj0BCallCast(Foam foam) 
{
	GJBValInfo inf;
	inf = gj0BCallBValInfo(foam->foamBCall.op);
	return jcCast(jcId(strCopy(inf->c1)), gj0Gen(foam->foamBCall.argv[0]));
}

local JavaCode
gj0BCallException(Foam foam) 
{
	JavaCodeList args;
	GJBValInfo inf;
	JavaCode jc;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	args = gj0GenList(foam->foamBCall.argv, foamArgc(foam)-1);
	jc = jcThrow(jcConstruct(jcImportedId(strCopy(inf->c1),
						       strCopy(inf->c2)), 
					  args));
	return jc;
}

local JavaCode
gj0BCallNotImpl(Foam foam)
{
	return gj0Default(foam, strCopy(""));
}


struct gjBVal_info gjBValNotImpl = { 0, GJ_NotImpl };
struct gjBVal_info gjBValInfoTable[] = {
	{FOAM_BVal_BoolFalse, GJ_Keyword,  0, "true"},
	{FOAM_BVal_BoolTrue,  GJ_Keyword,  0, "false"},

	{FOAM_BVal_BoolNot, GJ_Op, JCO_OP_Not },
	{FOAM_BVal_BoolAnd, GJ_Op, JCO_OP_And },
	{FOAM_BVal_BoolOr,  GJ_Op, JCO_OP_Or },
	{FOAM_BVal_BoolEQ,  GJ_Op, JCO_OP_Equals },
	{FOAM_BVal_BoolNE,  GJ_Op, JCO_OP_NEquals },

	{FOAM_BVal_CharSpace,   GJ_LitChar, 0, " "},
	{FOAM_BVal_CharNewline, GJ_LitChar, 0, "\n"},
	{FOAM_BVal_CharTab,     GJ_LitChar, 0, "\t"},

	{FOAM_BVal_CharMin, GJ_Const, 0, "Character", "MIN_VALUE" },
	{FOAM_BVal_CharMax, GJ_Const, 0, "Character", "MAX_VALUE" },

	{FOAM_BVal_CharIsDigit,  GJ_Apply, 0, "Character", "isDigit"},
	{FOAM_BVal_CharIsLetter, GJ_Apply, 0, "Character", "isLetter"},

	{FOAM_BVal_CharEQ, GJ_Op, JCO_OP_Equals},
	{FOAM_BVal_CharNE, GJ_Op, JCO_OP_NEquals},
	{FOAM_BVal_CharLT, GJ_Op, JCO_OP_LT},
	{FOAM_BVal_CharLE, GJ_Op, JCO_OP_LE},

	{FOAM_BVal_CharLower, GJ_Apply, 0, "Character", "toLowerCase"},
	{FOAM_BVal_CharUpper, GJ_Apply, 0, "Character", "toUpperCase"},
	{FOAM_BVal_CharOrd, GJ_Cast, 0, "int"},
	{FOAM_BVal_CharNum, GJ_Cast, 0, "char"},

	{FOAM_BVal_SFlo0,   GJ_LitFloat, 0,"0.0f"},
	{FOAM_BVal_SFlo1,   GJ_LitFloat, 0,"0.0f"},
	{FOAM_BVal_SFloMin, GJ_NegConst, 0, "Float", "MAX_VALUE"},
	{FOAM_BVal_SFloMax, GJ_Const,     0, "Float", "MAX_VALUE"},
	{FOAM_BVal_SFloEpsilon, GJ_Const, 0,"Float",  "MIN_VALUE"},
	{FOAM_BVal_SFloIsZero,   GJ_Op, JCO_OP_Equals, "0.0"},
	{FOAM_BVal_SFloIsNeg,    GJ_Op, JCO_OP_LT,     "0.0"},
	{FOAM_BVal_SFloIsPos,    GJ_Op, JCO_OP_GT,     "0.0"},
	{FOAM_BVal_SFloEQ,       GJ_Op, JCO_OP_Equals},
	{FOAM_BVal_SFloNE,       GJ_Op, JCO_OP_NEquals},
	{FOAM_BVal_SFloLT,       GJ_Op, JCO_OP_LT},
	{FOAM_BVal_SFloLE,       GJ_Op, JCO_OP_LE},
	{FOAM_BVal_SFloNegate,   GJ_Op, JCO_OP_Negate},
	{FOAM_BVal_SFloPrev,      GJ_Apply, 0, "foamj.Math", "sfloPrev"},
	{FOAM_BVal_SFloNext,      GJ_Apply, 0, "foamj.Math", "sfloNext"},
	{FOAM_BVal_SFloPlus,      GJ_Op, JCO_OP_Plus},
	{FOAM_BVal_SFloMinus,     GJ_Op, JCO_OP_Minus},
	{FOAM_BVal_SFloTimes,     GJ_Op, JCO_OP_Times},
	{FOAM_BVal_SFloTimesPlus, GJ_Op, JCO_OP_TimesPlus},
	{FOAM_BVal_SFloDivide,    GJ_Op, JCO_OP_Divide},
	{FOAM_BVal_SFloRPlus,     GJ_Apply, 0, "foamj.Math", "sfloRPlus"},
	{FOAM_BVal_SFloRMinus,    GJ_Apply, 0, "foamj.Math", "sfloRMinus"},
	{FOAM_BVal_SFloRTimes,    GJ_Apply, 0, "foamj.Math", "sfloRTimes"},
	{FOAM_BVal_SFloRTimesPlus,GJ_Apply, 0, "foamj.Math", "sfloRTimesPlus"},
	{FOAM_BVal_SFloRDivide,   GJ_Apply, 0, "foamj.Math", "sfloRDivide"},   
	{FOAM_BVal_SFloDissemble, GJ_Apply, 0, "foamj.Math", "sfloDissemble"},
	{FOAM_BVal_SFloAssemble,  GJ_Apply, 0, "foamj.Math", "sfloAssemble"},

	{FOAM_BVal_DFlo0,   GJ_LitFloat, 0,"0.0f"},
	{FOAM_BVal_DFlo1,   GJ_LitFloat, 0,"0.0f"},
	{FOAM_BVal_DFloMin, GJ_NegConst, 0, "Double", "MAX_VALUE"},
	{FOAM_BVal_DFloMax, GJ_Const,     0, "Double", "MAX_VALUE"},
	{FOAM_BVal_DFloEpsilon, GJ_Const, 0, "Double",  "MIN_VALUE"},
	{FOAM_BVal_DFloIsZero,   GJ_Op, JCO_OP_Equals, "0.0"},
	{FOAM_BVal_DFloIsNeg,    GJ_Op, JCO_OP_LT,     "0.0"},
	{FOAM_BVal_DFloIsPos,    GJ_Op, JCO_OP_GT,     "0.0"},
	{FOAM_BVal_DFloEQ, GJ_Op, JCO_OP_Equals},
	{FOAM_BVal_DFloNE, GJ_Op, JCO_OP_NEquals},
	{FOAM_BVal_DFloLT, GJ_Op, JCO_OP_LT},
	{FOAM_BVal_DFloLE, GJ_Op, JCO_OP_LE},
	{FOAM_BVal_DFloNegate, GJ_Op, JCO_OP_Negate},
	{FOAM_BVal_DFloPrev,     GJ_Apply, 0, "foamj.Math", "dfloPrev"},
	{FOAM_BVal_DFloNext,     GJ_Apply, 0, "foamj.Math", "dfloNext"},
	{FOAM_BVal_DFloPlus,      GJ_Op, JCO_OP_Plus},
	{FOAM_BVal_DFloMinus,     GJ_Op, JCO_OP_Minus},
	{FOAM_BVal_DFloTimes,     GJ_Op, JCO_OP_Times},
	{FOAM_BVal_DFloTimesPlus, GJ_Op, JCO_OP_TimesPlus},
	{FOAM_BVal_DFloDivide,    GJ_Op, JCO_OP_Divide},
	{FOAM_BVal_DFloRPlus,     GJ_Apply, 0, "foamj.Math", "dfloRPlus"},     
	{FOAM_BVal_DFloRMinus,    GJ_Apply, 0, "foamj.Math", "dfloRMinus"},    
	{FOAM_BVal_DFloRTimes,    GJ_Apply, 0, "foamj.Math", "dfloRTimes"},    
	{FOAM_BVal_DFloRTimesPlus,GJ_Apply, 0, "foamj.Math", "dfloRTimesPlus"},
	{FOAM_BVal_DFloRDivide,   GJ_Apply, 0, "foamj.Math", "dfloRDivide"},   
	{FOAM_BVal_DFloDissemble, GJ_Apply, 0, "foamj.Math", "dfloDissemble"},
	{FOAM_BVal_DFloAssemble,  GJ_Apply, 0, "foamj.Math", "dfloAssemble"},

	{FOAM_BVal_Byte0,        GJ_LitInt,  0, "(byte) 0" },
	{FOAM_BVal_Byte1,        GJ_LitInt,  0, "(byte) 1" },
	{FOAM_BVal_ByteMin,      GJ_LitInt,  0, "(byte) 0"},
	{FOAM_BVal_ByteMax,      GJ_Const,   0, "Byte", "MAX_VALUE"},

	{FOAM_BVal_HInt0,        GJ_LitInt,  0,"(short) 0" },		      
	{FOAM_BVal_HInt1,        GJ_LitInt,  0,"(short) 1" },		      
	{FOAM_BVal_HIntMin,      GJ_Const ,  0,"Short", "MIN_VALUE"},
	{FOAM_BVal_HIntMax,      GJ_Const,   0,"Short", "MAX_VALUE"},

	{FOAM_BVal_SInt0,        GJ_LitInt,  0,"0" },		      
	{FOAM_BVal_SInt1,        GJ_LitInt,  0,"1" },		      
	{FOAM_BVal_SIntMin,      GJ_LitInt,  0,"0"},		      
	{FOAM_BVal_SIntMax,      GJ_Const,       0,"Integer", "MAX_VALUE"},

	{FOAM_BVal_SIntIsZero,   GJ_Op, JCO_OP_Equals, "0"},
	{FOAM_BVal_SIntIsNeg,    GJ_Op, JCO_OP_LT,     "0"},
	{FOAM_BVal_SIntIsPos,    GJ_Op, JCO_OP_GT,     "0"},

	{FOAM_BVal_SIntIsEven,   GJ_Apply, 0, "foamj.Math", "isEven"},
	{FOAM_BVal_SIntIsOdd,    GJ_Apply, 0, "foamj.Math", "isOdd"},

	{FOAM_BVal_SIntEQ,       GJ_Op, JCO_OP_Equals},
	{FOAM_BVal_SIntNE,       GJ_Op, JCO_OP_NEquals},
	{FOAM_BVal_SIntLT,       GJ_Op, JCO_OP_LT},    
	{FOAM_BVal_SIntLE,       GJ_Op, JCO_OP_LE},    
	{FOAM_BVal_SIntNegate,   GJ_Op, JCO_OP_Negate},
	{FOAM_BVal_SIntPrev,     GJ_Op, JCO_OP_Minus,   "1"},
	{FOAM_BVal_SIntNext,     GJ_Op, JCO_OP_Plus,    "1"},
	{FOAM_BVal_SIntPlus,     GJ_Op, JCO_OP_Plus},
	{FOAM_BVal_SIntMinus,    GJ_Op, JCO_OP_Minus},
	{FOAM_BVal_SIntTimes,    GJ_Op, JCO_OP_Times},
	{FOAM_BVal_SIntTimesPlus,GJ_Op, JCO_OP_TimesPlus},
	{FOAM_BVal_SIntMod,      GJ_Op, JCO_OP_Modulo},
	{FOAM_BVal_SIntQuo,      GJ_Op, JCO_OP_Divide},
	{FOAM_BVal_SIntRem,      GJ_Op, JCO_OP_Modulo},
	{FOAM_BVal_SIntDivide,   GJ_Apply, 0, "foamj.Math", "divide" },

	{FOAM_BVal_SIntGcd,        GJ_Apply,  0,"foamj.Math",      "gcd"},
	{FOAM_BVal_SIntPlusMod,    GJ_OpMod,  JCO_OP_Plus},
	{FOAM_BVal_SIntMinusMod,   GJ_OpMod,  JCO_OP_Minus},
	{FOAM_BVal_SIntTimesMod,   GJ_OpMod,  JCO_OP_Times},
	{FOAM_BVal_SIntTimesModInv,GJ_Apply,  0,"foamj.Math","timesModInv"},
	{FOAM_BVal_SIntLength,   GJ_Apply,    0,"foamj.Math",   "length"},
	{FOAM_BVal_SIntShiftUp,  GJ_Op,       JCO_OP_ShiftUp},
	{FOAM_BVal_SIntShiftDn,  GJ_Op,       JCO_OP_ShiftDn},
	{FOAM_BVal_SIntBit,      GJ_Apply,   0,"foamj.Math",      "bit"},

	{FOAM_BVal_SIntNot,      GJ_Op,     JCO_OP_XOr, "0"},
	{FOAM_BVal_SIntAnd,      GJ_Op,     JCO_OP_And},
	{FOAM_BVal_SIntOr,       GJ_Op,     JCO_OP_Or},
	{FOAM_BVal_SIntXOr,      GJ_Op,     JCO_OP_XOr},
	{FOAM_BVal_SIntHashCombine,GJ_Apply,0, "foamj.Math", "hashCombine"},

	{FOAM_BVal_WordTimesDouble, GJ_Apply,   0,"foamj.Math",      "wordTimesDouble"},
	{FOAM_BVal_WordDivideDouble,GJ_Apply,   0,"foamj.Math",      "wordDivideDouble"},
	{FOAM_BVal_WordPlusStep,    GJ_Apply,   0,"foamj.Math",      "wordPlusStep"},
	{FOAM_BVal_WordTimesStep,   GJ_Apply,   0,"foamj.Math",      "wordTimesStep"},

	{FOAM_BVal_BInt0,        GJ_Const, 0,   "BigInteger",   "ZERO"},
	{FOAM_BVal_BInt1,        GJ_Const, 0,   "BigInteger",   "ONE"},
	{FOAM_BVal_BIntIsZero,   GJ_Apply, 0,   "foamj.Math",   "isZero"},
	{FOAM_BVal_BIntIsNeg,    GJ_Apply, 0,   "foamj.Math",   "isNeg"},
	{FOAM_BVal_BIntIsPos,    GJ_Apply, 0,   "foamj.Math",   "isPos"},
	{FOAM_BVal_BIntIsEven,   GJ_Apply, 0,   "foamj.Math",   "isEven"},
	{FOAM_BVal_BIntIsOdd,    GJ_Apply, 0,   "foamj.Math",   "isOdd"},
	{FOAM_BVal_BIntIsSingle, GJ_Apply, 0,   "foamj.Math",   "isSingle"},
	{FOAM_BVal_BIntEQ,       GJ_Apply, 0,   "foamj.Math",   "eq"},
	{FOAM_BVal_BIntNE,       GJ_Apply, 0,   "foamj.Math",   "ne"},
	{FOAM_BVal_BIntLT,       GJ_Apply, 0,   "foamj.Math",   "lt"},
	{FOAM_BVal_BIntLE,       GJ_Apply, 0,   "foamj.Math",   "le"},
	{FOAM_BVal_BIntNegate,   GJ_Meth, 0,   "negate"},
	{FOAM_BVal_BIntPrev,     GJ_Apply, 0,   "foamj.Math",   "prev"},
	{FOAM_BVal_BIntNext,     GJ_Apply, 0,   "foamj.Math",   "next"},
	{FOAM_BVal_BIntPlus,     GJ_Apply, 0,   "foamj.Math",   "plus"},
	{FOAM_BVal_BIntMinus,    GJ_Apply, 0,   "foamj.Math",   "minus"},
	{FOAM_BVal_BIntTimes,    GJ_Apply, 0,   "foamj.Math",   "times"},
	{FOAM_BVal_BIntTimesPlus,GJ_Apply, 0,   "foamj.Math",   "timesPlus"},
	{FOAM_BVal_BIntMod,      GJ_Apply, 0,   "foamj.Math",   "mod"},
	{FOAM_BVal_BIntQuo,      GJ_Apply, 0,   "foamj.Math",   "quo"},
	{FOAM_BVal_BIntRem,      GJ_Apply, 0,   "foamj.Math",   "rem"},
	{FOAM_BVal_BIntDivide,   GJ_Apply, 0,   "foamj.Math",   "divide"},
	{FOAM_BVal_BIntGcd,      GJ_Meth,  0,   "gcd"},
	{FOAM_BVal_BIntSIPower,  GJ_Apply, 0,   "foamj.Math",   "sIPower"},
	{FOAM_BVal_BIntBIPower,  GJ_Apply, 0,   "foamj.Math",   "bIPower"},
	{FOAM_BVal_BIntPowerMod, GJ_Apply, 0,   "foamj.Math",   "powerMod"},
	{FOAM_BVal_BIntLength,   GJ_Meth, 0,   "bitCount"},
	{FOAM_BVal_BIntShiftUp,  GJ_Apply, 0,   "foamj.Math",   "shiftUp"},
	{FOAM_BVal_BIntShiftDn,  GJ_Apply, 0,   "foamj.Math",   "shiftDn"},
	{FOAM_BVal_BIntShiftRem, GJ_Apply, 0,   "foamj.Math",   "shiftRem"},
	{FOAM_BVal_BIntBit,      GJ_Meth,  0,   "testBit"},

	{FOAM_BVal_PtrNil,       GJ_Keyword, 0, "null"},
	{FOAM_BVal_PtrIsNil,     GJ_Op,     JCO_OP_Equals, "null"},
	{FOAM_BVal_PtrMagicEQ,   GJ_Op,     JCO_OP_Equals},
	{FOAM_BVal_PtrEQ,        GJ_Op,     JCO_OP_Equals},
	{FOAM_BVal_PtrNE,        GJ_Op,     JCO_OP_NEquals},


	{FOAM_BVal_FormatSFlo,   GJ_Apply, 0, "foamj.Math", "formatSFlo"},
	{FOAM_BVal_FormatDFlo,   GJ_Apply, 0, "foamj.Math", "formatDFlo"},
	{FOAM_BVal_FormatSInt,   GJ_Apply, 0, "foamj.Math", "formatSInt"},
	{FOAM_BVal_FormatBInt,   GJ_Apply, 0, "foamj.Math", "formatBInt"},

	{FOAM_BVal_ScanSFlo,     GJ_Apply, 0, "foamj.Math", "scanSFlo"},
	{FOAM_BVal_ScanDFlo,     GJ_Apply, 0, "foamj.Math", "scanDFlo"},
	{FOAM_BVal_ScanSInt,     GJ_Apply, 0, "foamj.Math", "scanSInt"},
	{FOAM_BVal_ScanBInt,     GJ_Apply, 0, "foamj.Math", "scanBInt"},

	{FOAM_BVal_SFloToDFlo,   GJ_Cast, 0, "double"},
	{FOAM_BVal_DFloToSFlo,   GJ_Cast, 0, "float"},
	{FOAM_BVal_ByteToSInt,   GJ_Cast, 0, "int"},
	{FOAM_BVal_SIntToByte,   GJ_Cast, 0, "byte"},
	{FOAM_BVal_HIntToSInt,   GJ_Cast, 0, "int"},
	{FOAM_BVal_SIntToHInt,   GJ_Cast, 0, "short"},
	{FOAM_BVal_SIntToBInt,   GJ_Apply,  0, "java.math.BigInteger", "valueOf"},
	{FOAM_BVal_BIntToSInt,   GJ_Meth, 0,"intValue"},
	{FOAM_BVal_SIntToSFlo,   GJ_Cast, 0, "float"},
	{FOAM_BVal_SIntToDFlo,   GJ_Cast, 0, "double"},
	{FOAM_BVal_BIntToSFlo,   GJ_Meth,  0,"floatValue"},
	{FOAM_BVal_BIntToDFlo,   GJ_Meth,  0, "doubleValue"},
	{FOAM_BVal_PtrToSInt,    GJ_Apply, 0, "foamj.Foam", "ptrToSInt"},
	{FOAM_BVal_SIntToPtr,    GJ_Apply, 0, "foamj.Foam", "sintToPtr"},
	
	{FOAM_BVal_ArrToSFlo,    GJ_Apply, 0, "foamj.Foam", "arrToSFlo"},
	{FOAM_BVal_ArrToDFlo,    GJ_Apply, 0, "foamj.Foam", "arrToDFlo"},
	{FOAM_BVal_ArrToSInt,    GJ_Apply, 0, "foamj.Foam", "arrToSInt"},
	{FOAM_BVal_ArrToBInt,    GJ_Apply, 0, "foamj.Foam", "arrToBInt"},
	
	{FOAM_BVal_PlatformRTE,  GJ_Const, 0, "foamj.Foam", "RTE"},
	{FOAM_BVal_PlatformOS,   GJ_Const, 0, "foamj.Foam", "PlatformOS"},

	{FOAM_BVal_Halt,         GJ_Exception, 0, "foamj", "FoamException"},

 {FOAM_BVal_RoundZero,    GJ_Const, 0,"foamj.Math", "ROUND_ZERO"},
 {FOAM_BVal_RoundNearest, GJ_Const, 0,"foamj.Math", "ROUND_NEAREST"},
 {FOAM_BVal_RoundUp,      GJ_Const, 0,"foamj.Math", "ROUND_UP"},
 {FOAM_BVal_RoundDown,    GJ_Const, 0,"foamj.Math", "ROUND_DOWN"},
 {FOAM_BVal_RoundDontCare,GJ_Const, 0,"foamj.Math", "ROUND_DONTCARE"},

 {FOAM_BVal_SFloTruncate,  GJ_Apply,  0, "foamj.Math", "sfloTruncate"},
 {FOAM_BVal_SFloFraction,  GJ_Apply,  0,"foamj.Math", "sfloFraction"},
 {FOAM_BVal_SFloRound,     GJ_Apply,  0,"foamj.Math", "sfloRound"},

 {FOAM_BVal_DFloTruncate,  GJ_Apply,  0,"foamj.Math", "dfloTruncate"},
 {FOAM_BVal_DFloFraction,  GJ_Apply,  0,"foamj.Math", "dfloFraction"},
 {FOAM_BVal_DFloRound,     GJ_Apply,  0,"foamj.Math", "dfloRound"},

};
#if 0
 {FOAM_BVal_StoForceGC,    GJB_FCall,  0,"fiStoForceGC",    0},	
 {FOAM_BVal_StoInHeap,     GJB_FCall,  0,"fiStoInHeap",     0},
 {FOAM_BVal_StoIsWritable, GJB_FCall,  0,"fiStoIsWritable", 0},
 {FOAM_BVal_StoMarkObject, GJB_FCall,  0,"fiStoMarkObject", 0},
 {FOAM_BVal_StoRecode,     GJB_FCall,  0,"fiStoRecode",     0},
 {FOAM_BVal_StoNewObject,  GJB_FCall,  0,"fiStoNewObject",  0},
 {FOAM_BVal_StoATracer,    GJB_FCall,  0,"fiStoATracer",    0},
 {FOAM_BVal_StoCTracer,    GJB_FCall,  0,"fiStoCTracer",    0},
 {FOAM_BVal_StoShow,       GJB_FCall,  0,"fiStoShow",       0},
 {FOAM_BVal_StoShowArgs,   GJB_FCall,  0,"fiStoShowArgs",   0},

 {FOAM_BVal_TypeInt8,      GJB_FCall,  0,"fiTypeInt8",      0},
 {FOAM_BVal_TypeInt16,     GJB_FCall,  0,"fiTypeInt16",     0},
 {FOAM_BVal_TypeInt32,     GJB_FCall,  0,"fiTypeInt32",     0},
 {FOAM_BVal_TypeInt64,     GJB_FCall,  0,"fiTypeInt64",     0},
 {FOAM_BVal_TypeInt128,    GJB_FCall,  0,"fiTypeInt128",    0},

 {FOAM_BVal_TypeNil,       GJB_FCall,  0,"fiTypeNil",       0},
 {FOAM_BVal_TypeChar,      GJB_FCall,  0,"fiTypeChar",      0},
 {FOAM_BVal_TypeBool,      GJB_FCall,  0,"fiTypeBool",      0},
 {FOAM_BVal_TypeByte,      GJB_FCall,  0,"fiTypeByte",      0},
 {FOAM_BVal_TypeHInt,      GJB_FCall,  0,"fiTypeHInt",      0},
 {FOAM_BVal_TypeSInt,      GJB_FCall,  0,"fiTypeSInt",      0},
 {FOAM_BVal_TypeBInt,      GJB_FCall,  0,"fiTypeBInt",      0},
 {FOAM_BVal_TypeSFlo,      GJB_FCall,  0,"fiTypeSFlo",      0},
 {FOAM_BVal_TypeDFlo,      GJB_FCall,  0,"fiTypeDFlo",      0},
 {FOAM_BVal_TypeWord,      GJB_FCall,  0,"fiTypeWord",      0},
 {FOAM_BVal_TypeClos,      GJB_FCall,  0,"fiTypeClos",      0},
 {FOAM_BVal_TypePtr,       GJB_FCall,  0,"fiTypePtr",       0},
 {FOAM_BVal_TypeRec,       GJB_FCall,  0,"fiTypeRec",       0},
 {FOAM_BVal_TypeArr,       GJB_FCall,  0,"fiTypeArr",       0},
 {FOAM_BVal_TypeTR,        GJB_FCall,  0,"fiTypeTR",        0},
 {FOAM_BVal_RawRepSize,    GJB_FCall,  0,"fiRawRepSize",    0},
 {FOAM_BVal_SizeOfInt8,    GJB_FCall,  0,"fiSizeOfInt8",    0},
 {FOAM_BVal_SizeOfInt16,   GJB_FCall,  0,"fiSizeOfInt16",   0},
 {FOAM_BVal_SizeOfInt32,   GJB_FCall,  0,"fiSizeOfInt32",   0},
 {FOAM_BVal_SizeOfInt64,   GJB_FCall,  0,"fiSizeOfInt64",   0},
 {FOAM_BVal_SizeOfInt128,  GJB_FCall,  0,"fiSizeOfInt128",  0},

 {FOAM_BVal_SizeOfNil,     GJB_FCall,  0,"fiSizeOfNil",     0},
 {FOAM_BVal_SizeOfChar,    GJB_FCall,  0,"fiSizeOfChar",    0},
 {FOAM_BVal_SizeOfBool,    GJB_FCall,  0,"fiSizeOfBool",    0},
 {FOAM_BVal_SizeOfByte,    GJB_FCall,  0,"fiSizeOfByte",    0},
 {FOAM_BVal_SizeOfHInt,    GJB_FCall,  0,"fiSizeOfHInt",    0},
 {FOAM_BVal_SizeOfSInt,    GJB_FCall,  0,"fiSizeOfSInt",    0},
 {FOAM_BVal_SizeOfBInt,    GJB_FCall,  0,"fiSizeOfBInt",    0},
 {FOAM_BVal_SizeOfSFlo,    GJB_FCall,  0,"fiSizeOfSFlo",    0},
 {FOAM_BVal_SizeOfDFlo,    GJB_FCall,  0,"fiSizeOfDFlo",    0},
 {FOAM_BVal_SizeOfWord,    GJB_FCall,  0,"fiSizeOfWord",    0},
 {FOAM_BVal_SizeOfClos,    GJB_FCall,  0,"fiSizeOfClos",    0},
 {FOAM_BVal_SizeOfPtr,     GJB_FCall,  0,"fiSizeOfPtr",     0},
 {FOAM_BVal_SizeOfRec,     GJB_FCall,  0,"fiSizeOfRec",     0},
 {FOAM_BVal_SizeOfArr,     GJB_FCall,  0,"fiSizeOfArr",     0},
 {FOAM_BVal_SizeOfTR,      GJB_FCall,  0,"fiSizeOfTR",      0},

 {FOAM_BVal_ListNil,       GJB_FCall,  0,"fiListNil",       "fiLIST_NIL"},
 {FOAM_BVal_ListEmptyP,    GJB_FCall,  0,"fiListEmptyP",    "fiLIST_EMPTYP"},
 {FOAM_BVal_ListHead,      GJB_FCall,  0,"fiListHead",      "fiLIST_HEAD"},
 {FOAM_BVal_ListTail,      GJB_FCall,  0,"fiListTail",      "fiLIST_TAIL"},
 {FOAM_BVal_ListCons,      GJB_FCall,  0,"fiListCons",      0},
 {FOAM_BVal_NewExportTable, GJB_FCall, 0, "fiNewExportTable", 0},
 {FOAM_BVal_AddToExportTable, GJB_FCall, 0, "fiAddToExportTable", 0},
 {FOAM_BVal_FreeExportTable, GJB_FCall, 0, "fiFreeExportTable", 0},
#if EDIT_1_0_n1_AB
 /* This BVal must NEVER be seen by genc (or genjava) ... */
 {FOAM_BVal_ssaPhi, GJB_FCall, 0, "fiNonExistentFunction", 0},
#endif
};
#endif


local GJBValInfo 
gj0BCallBValInfo(FoamBValTag tag)
{
	GJBValInfo inf = &gjBValInfoTable[tag];
	if ((char*) inf >= ((char*) gjBValInfoTable) + sizeof(gjBValInfoTable))
		return &gjBValNotImpl;
	assert(inf->tag == tag || (fprintf(stderr, "%s %s", foamBValStr(tag), foamBValStr(inf->tag)), 0));
	return inf;
}


/*
 * :: Names
 */
struct gjSpecCharId_info {
	unsigned char c;
	CString s;
};
struct gjSpecCharId_info gjSpecCharIdTable[];

CString gjCharIds[CHAR_MAX];
#define STDCHAR_MAX 127

local void
gj0NameInit() 
{
	struct gjSpecCharId_info *p = &gjSpecCharIdTable[0];

	while (p->c != '\0') {
		/* Identifiers must never contain UTF-8 encoded unicode
		 * characters or extended ASCII. */
		assert(p->c <= STDCHAR_MAX);
		gjCharIds[p->c] = p->s;
		p++;
	}
}

local String
gj0Name(CString prefix, Foam fmt, int idx)
{
	Foam decl;
	Buffer b = bufNew();

	assert(idx < foamArgc(fmt));
	decl = fmt->foamDDecl.argv[idx];
	bufPrintf(b, "%s%d", prefix, idx);

	if (strlen(decl->foamDecl.id) > 0) {
		bufPutc(b, '_');
		bufPuts(b, gj0NameFrString(decl->foamDecl.id));
	}

	return bufLiberate(b);
}


local String
gj0NameFrString(String fmName)
{
	Buffer buf = bufNew();
	Bool flg;
	CString p;

	flg = false;
	p = fmName;
	while (*p != 0) {
		CString repl;
		assert(*p >= 0 && *p < CHAR_MAX);
		repl = gjCharIds[(unsigned char)*p];
		if (repl != 0)
			flg = true;
		p++;
	}
	if (!flg)
		return fmName;

	p = fmName;
	bufNeed(buf, strlen(fmName));
	while (*p != 0) {
		CString repl;
		assert(*p >= 0 && *p < CHAR_MAX);
		repl = gjCharIds[(unsigned char)*p];
		if (!repl) {
			bufAdd1(buf, *p);
		}
		else {
			bufPuts(buf, repl);
		}
		p++;
	}
	bufAdd1(buf, '\0');
	return bufLiberate(buf);
}


struct gjSpecCharId_info gjSpecCharIdTable[] = {
 {'!',	"_BANG_"},
 {'\"',	"_QUOTE_"},
 {'#',	"_SHARP_"},
 {'$',	"_DOLLR_"},
 {'%',	"_PCENT_"},
 {'&',	"_AMPER_"},
 {'\'',	"_APOS_"},
 {'(',	"_OPAREN_"},
 {')',	"_CPAREN_"},
 {'*',	"_STAR_"},
 {'+',	"_PLUS_"},
 {',',	"_COMMA_"},
 {'-',	"_MINUS_"},
 {'.',	"_DOT_"},
 {'/',	"_SLASH_"},
 {':',	"_COLON_"},
 {';',	"_SEMI_"},
 {'<',	"_LT_"},
 {'=',	"_EQ_"},
 {'>',	"_GT_"},
 {'?',	"_QMARK_"},
 {'@',	"_AT_"},
 {'[',	"_OBRACK_"},
 {'\\',	"_BSLSH_"},
 {']',	"_CBRACK_"},
 {'^',	"_HAT_"},
 {'_',	"__"},
 {'`',	"_GRAVE_"},
 {'{',	"_OBRACE_"},
 {'|',	"_BAR_"},
 {'}',	"_CBRACE_"},
 {'~',	"_TILDE_"},
 {0,	0}
};

