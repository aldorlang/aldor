#include "javacode.h"
#include "foamsig.h"
#include "intset.h"

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
local JavaCodeList gj0GenList(Foam foam);
local JavaCode gj0Def(Foam foam);
local JavaCode gj0Default(Foam foam);
local JavaCode gj0Prog(Foam lhs, Foam rhs);
local JavaCode gj0ClosInit(Foam lhs, Foam rhs);
local JavaCode gj0Set(Foam lhs, Foam rhs);
local JavaCode gj0Return(Foam foam);
local JavaCode gj0Seq(Foam seq);
local JavaCode gj0Par(Foam seq);
local JavaCode gj0Loc(Foam seq);
local JavaCode gj0Glo(Foam ref);
local JavaCode gj0GloSet(Foam ref, Foam rhs);
local JavaCode gj0GloRegister(Foam lhs, Foam rhs);
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
local JavaCode gj0Bool(Foam foam);
local JavaCode gj0Char(Foam foam);

local JavaCode gj0Cast(Foam foam);

local JavaCode gj0Env(Foam foam);
local JavaCode gj0Lex(Foam foam);
local JavaCode gj0PushEnv(Foam foam);
local JavaCode gj0LexSet(Foam foam, Foam rhs);
local JavaCode gj0EElt(Foam foam);
local JavaCode gj0EEltSet(Foam foam, Foam rhs);
local JavaCode gj0EEnv(Foam foam);
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

local JavaCode gj0MFmt(Foam mfmt);

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
local JavaCodeList gj0ProgDeclarations(Foam ddecl);

local String       gj0Name(String prefix, Foam fmt, int idx);
local JavaCode     gj0ProgRetnType(Foam rhs);
local JavaCode     gj0TopConst(Foam lhs, Foam rhs);

local JavaCode     gj0Type(Foam decl);
local JavaCode     gj0TypeFrFmt(AInt id, AInt fmt);
local JavaCode     gj0TypeObjToValue(JavaCode val, FoamTag type, AInt fmt);
local JavaCode     gj0TypeValueToObj(JavaCode val, FoamTag type, AInt fmt);

local FoamTag      gj0FoamExprType(Foam foam);
local FoamTag      gj0FoamExprTypeWFmt(Foam foam, AInt *fmt);

local JavaCodeList gj0CCallCollectArgs(Foam call);
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

enum gjId {
	GJ_FoamWord,
	GJ_FoamClos,
	GJ_FoamRecord,
	GJ_FoamEnv,

	GJ_FoamFn,
	GJ_FoamValue,
	GJ_Multi,
	GJ_FoamGlobals,
	GJ_Format,
	GJ_EnvRecord,
};

typedef Enum(gjId) GjId;

local JavaCode     gj0Id(GjId id);


/* State variables */
struct gjContext {
	Foam formats;
	Foam constants;
	// Mutable state (caches, etc)
	int  lvl;
	FoamSigList  ccallStubSigList;
	JavaCodeList gloRegList;
	IntSet       fmtSet;
	// Per prog
	Foam prog;
	Foam progParams;
	Foam progLocals;
	Foam progLhs;
	FoamSigList progSigList;
	int multVarIdx;
	// in codegen
	AInt mfmt;
};

#define gjContextGlobals (gjContext->formats->foamDFmt.argv[globalsSlot])
#define gjContextGlobal(id) (gjContextGlobals->foamDDecl.argv[id])

static struct gjContext gjCtxt0;
static struct gjContext *gjContext = &gjCtxt0;

/* Functions... */

JavaCode 
gjGenJavaUnit(Foam foam, String name)
{
	JavaCodeList imps, code;
	JavaCode clss;
	String className, comment;

	gjInit(foam, name);

	className = gj0ClassName(foam, name);
	code = gj0DDef(foam->foamUnit.defs);
	comment = gj0ClassDocumentation(foam, name);

	clss = jcClass(JCO_MOD_Public, comment, 
				jcId(className), NULL, NULL, code);

	imps = gj0CollectImports(clss);
	return jcFile(NULL, jcId(className), imps, clss);
}

local JavaCodeList
gj0CollectImports(JavaCode clss)
{
	JavaCodeList imps = jcCollectImports(clss);
	JavaCodeList ids = listNil(JavaCode);
	JavaCodeList tmp = imps;
	while (tmp) {
		JavaCode id = car(tmp);
		JavaCode stmt = jcStatement(jcSpaceSeqV(2, jcId(strCopy("import")), id));
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
	JavaCodeList fmts;
	int i;

	for (i=0; i<foamArgc(foam); i++) {
		Foam fm = foam->foamDDef.argv[i];
		JavaCode code = gj0Gen(fm);
		if (code != 0)
			lst = listCons(JavaCode)(code, lst);
	}
	
	lst  = listNReverse(JavaCode)(lst);

	fmts = gj0FmtInits();
	stubs = gj0CCallStubGen(gjContext->ccallStubSigList);
	lst = listNConcat(JavaCode)(lst, stubs);

	return listNConcat(JavaCode)(fmts, lst);
}

local JavaCode
gj0Gen(Foam foam)
{
	switch (foamTag(foam)) {
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
	case FOAM_SInt:
		return gj0SInt(foam);
	case FOAM_Char:
		return gj0Char(foam);
	case FOAM_Bool:
		return gj0Bool(foam);
	case FOAM_Cast:
		return gj0Cast(foam);
	case FOAM_CCall:
		return gj0CCall(foam);
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

	default: 
		return gj0Default(foam);
	}

}

local JavaCodeList
gj0GenList(Foam foam)
{
	JavaCodeList args = listNil(JavaCode);
	foamIter(foam, pelt, {
			Foam elt = *pelt;
			args = listCons(JavaCode)(gj0Gen(elt), args);
		});
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
	
	return gj0Default(lhs);
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

local JavaCode
gj0Prog(Foam lhs, Foam rhs)
{
	GjProgResult r;
	assert(foamTag(rhs) == FOAM_Prog);
	
	gj0ProgInit(lhs, rhs);
	r = gj0ProgMain(rhs);
	JavaCode code = gj0ProgResultToJava(r);
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
	String methodName = gj0ProgMethodName(gjContext->progLhs);
	JavaCodeList args = gj0ProgCollectArgs(f);
	JavaCodeList whole;
	JavaCode body, fnDef;
	JavaCodeList exns = gj0ProgExceptions();
	int modifiers = gj0ProgModifiers();

	JavaCodeList declarations = gj0ProgDeclarations(gjContext->progLocals);
	JavaCodeList envInit = gj0ProgEnvInitCreate(f);
	JavaCodeList inits;

	fnDef = gj0ProgFnCreate(gjContext->progLhs, f);
	GjProgResult r = (GjProgResult) stoAlloc(OB_Other, sizeof(*r));
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
	return jcParamDecl(0, type, gj0EnvId(0));
}


local JavaCodeList
gj0ProgDeclarations(Foam ddecl)
{
	Table tbl = tblNew((TblHashFun) jcoHash, (TblEqFun) jcoEqual);
	TableIterator it;
	JavaCodeList decls;
	int i=0;
	foamIter(ddecl, pdecl, {
			JavaCode type = gj0Type(*pdecl);
			JavaCodeList l = (JavaCodeList) tblElt(tbl, type, 
							       listNil(JavaCode));
			l = listCons(JavaCode)(jcId(gj0Name("t", ddecl, i)), l);
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
	return JCO_MOD_Private | JCO_MOD_Static;
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
	int i, maxLvl;

	denv = f->foamProg.levels;
	lvlCount = foamArgc(denv);
	clss = bitvClassCreate(lvlCount);
	refLvls = bitvNew(clss);
	refEnvs = bitvNew(clss);
	bitvClearAll(clss, refLvls);
	bitvClearAll(clss, refEnvs);

	gj0ProgEnvInitCollect(f->foamProg.body, clss, refLvls, refEnvs);
	maxLvl = bitvMax(clss, refLvls);

	lines = listNil(JavaCode);
	for (i=1; i<=maxLvl; i++) {
		JavaCode line;
		line = jcStatement(gj0ProgEnvDecl(i, denv));
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
	JavaCode method;
	JavaCode methodBody;
	JavaCode fnDef;
	JavaCode anonClass;
	JavaCodeList args;
	
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

	anonClass = jcConstructSubclass(gj0Id(GJ_FoamFn),
					listNil(JavaCode),
					jcNLSeqV(1, method));
	fnDef = jcInitialisation(JCO_MOD_Static, 
				 gj0Id(GJ_FoamFn),
				 jcId(gj0ProgFnName(lhs->foamConst.index)),
				 anonClass);

	return jcStatement(fnDef);
}

local JavaCode
gj0ProgFnMethodBody(Foam lhs, Foam prog)
{
	Foam pddecl = prog->foamProg.params;
	JavaCodeList l, tmp;
	JavaCode call;
	int i;

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
	JavaCodeList ret;

	if (prog->foamProg.retType == FOAM_NOp) {
		JavaCode l1 = call;
		JavaCode l2 = jcReturn(jcNull());
		ret = listList(JavaCode)(2, 
					 jcStatement(l1), 
					 jcStatement(l2));
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
	case FOAM_HInt:
		return jcKeyword(symInternConst("short"));
	case FOAM_Bool:
		return jcKeyword(symInternConst("boolean"));
	case FOAM_Byte:
		return jcKeyword(symInternConst("byte"));
	case FOAM_NOp:
		if (fmt == 0 || fmt == emptyFormatSlot)
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
	case FOAM_Arr:
		if (fmt != 0)
			return jcArrayOf(gj0TypeFrFmt(fmt, 0));
		else
			return jcId(strCopy("Object"));
	case FOAM_Values:
		return gj0Id(GJ_Multi);

	default:
		return jcId(strCopy(foamStr(id)));
	}
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
	case FOAM_NOp:
		assert(false);
		return 0;
	case FOAM_Clos:
		return jcApplyMethodV(val, jcId(strCopy("toClos")), 0);
	case FOAM_Word:
		return jcApplyMethodV(val, jcId(strCopy("asWord")), 0);
	case FOAM_Rec:
		return jcApplyMethodV(val, jcId(strCopy("toRecord")), 0);
	case FOAM_Arr:
		return jcApplyMethodV(val, jcId(strCopy("toArray")), 0);
	default:
		return jcCast(jcSpaceSeqV(2, gj0Id(GJ_FoamValue),
					  jcComment(strCopy(foamStr(type)))),
			      val);

	}
}

local JavaCode
gj0TypeObjToValue(JavaCode val, FoamTag type, AInt fmt)
{
	switch (type) {
	case FOAM_Word:
		return jcApplyMethod(val, 
			     jcId(strCopy("toValue")), 
			     listNil(JavaCode));
	case FOAM_SInt:
		return jcApplyMethod(jcMemRef(gj0Id(GJ_FoamValue),
					      jcId(strCopy("U"))),
				     jcId(strCopy("fromSInt")),
				     listSingleton(JavaCode)(val));
							   
	default:
		return val;
	}

}

local JavaCode 
gj0Set(Foam lhs, Foam rhs)
{
	switch (foamTag(lhs)) {
	case FOAM_Fluid:
		return gj0Default(lhs);
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
	default: {
		JavaCode lhsJ = gj0Gen(lhs);
		JavaCode rhsJ = gj0Gen(rhs);
		return jcAssign(lhsJ, rhsJ);
	}
		
	}
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
gj0Glo(Foam ref)
{
	Foam decl;
	JavaCode jc;
	String id;
	
	decl = gjContextGlobal(ref->foamGlo.index);
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
	
	jc = gj0TypeObjToValue(gj0Gen(rhs), decl->foamDecl.type, decl->foamDecl.format);

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

local void gj0SeqGen(GjSeqStore seqs, Foam stmt);
local void gj0SeqGoto(GjSeqStore store, Foam foam);
local void gj0SeqLabel(GjSeqStore bucketList, Foam foam);
local void gj0SeqSelect(GjSeqStore bucketList, Foam foam);
local void gj0SeqSelect2(GjSeqStore store, Foam foam);
local void gj0SeqIf(GjSeqStore store, Foam foam);
local void gj0SeqSelectMulti(GjSeqStore store, Foam foam);
local void gj0SeqGenDefault(GjSeqStore bucketList, Foam foam);
local JavaCode gj0SeqSwitchId();

local GjSeqStore   gj0SeqStoreNew(void);
local void         gj0SeqStoreFree(GjSeqStore store);
local JavaCode     gj0SeqStoreToJava(GjSeqStore store);
local void         gj0SeqStoreAddStmt(GjSeqStore store, JavaCode stmt);
local void         gj0SeqStoreAddLabel(GjSeqStore store, AInt label);
local void         gj0SeqStoreEnsureBody(GjSeqStore store);

local GjSeqBucket  gj0SeqBucketNew(AInt label);
local void         gj0SeqBucketFree(GjSeqBucket store);
local JavaCodeList gj0SeqBucketToJava(GjSeqBucket bucket);
local GjSeqBucket  gj0SeqBucketGetPrefix(GjSeqBucketList buckets);
local Bool         gj0SeqBucketIsPrefix(GjSeqBucket bucket);

local JavaCode 
gj0Seq(Foam seq)
{
	JavaCode code;
	GjSeqStore seqs = gj0SeqStoreNew();
	int i;
	
	for (i=0; i<foamArgc(seq); i++) {
		gj0SeqGen(seqs, seq->foamSeq.argv[i]);
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
	return jcReturn(gj0Gen(r->foamReturn.value));
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
	
	gj0SeqStoreAddStmt(store, jcStatement(s1));
	gj0SeqStoreAddStmt(store, jcStatement(s2));
}

local void
gj0SeqLabel(GjSeqStore store, Foam foam)
{
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
	rhs = jcConditional(gj0Gen(foam->foamSelect.op), 
			    jcLiteralInteger(foam->foamSelect.argv[0]),
			    jcLiteralInteger(foam->foamSelect.argv[1]));

	s1 = jcAssign(lhs, rhs);
	s2 = jcContinue(0);

	gj0SeqStoreAddStmt(store, jcStatement(s1));
	gj0SeqStoreAddStmt(store, jcStatement(s2));

}

local void
gj0SeqIf(GjSeqStore store, Foam foam)
{
	// if (...) { label = .; break;}
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
	// switch (xxx) {
	//   case 0: target = 1; break;
	//   case 1: target = 4;
	//   case 2: target = 9;
	// }
	// continue
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
	if (car(store->buckets)->label == GJ_SEQ_Prefix) {
		store->buckets = listCons(GjSeqBucket)(gj0SeqBucketNew(GJ_SEQ_Init),
						       store->buckets);
		return;
	}
}

local void
gj0SeqStoreAddStmt(GjSeqStore store, JavaCode stmt)
{
	GjSeqBucket bucket;
	int i;

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

local JavaCode
gj0SeqStoreToJava(GjSeqStore store)
{
	JavaCodeList jlst, l1, l2;
	JavaCode stmt, decl;
	GjSeqBucketList buckets, lst;
	GjSeqBucket prefix;
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
	while (lst) {
		JavaCodeList block;
		GjSeqBucket bucket;
		bucket = car(lst);
		if (gj0SeqBucketIsPrefix(bucket)) {
			lst = cdr(lst);
			continue;
		}
		jlst = listCons(JavaCode)(jcCaseLabel(jcLiteralInteger(bucket->label)),
					  jlst);
		block = gj0SeqBucketToJava(bucket);
		jlst  = listNConcat(JavaCode)(listNReverse(JavaCode)(block),
					      jlst);
		lst = cdr(lst);
	}
	listFree(GjSeqBucket)(buckets);
	store->buckets = listNil(GjSeqBucket);
	jlst = listNReverse(JavaCode)(jlst);

	l1 = prefix == 0 ? listNil(JavaCode) : gj0SeqBucketToJava(prefix);
	decl = jcStatement(jcInitialisation(0, 
					    jcKeyword(symInternConst("int")), 
					    gj0SeqSwitchId(),
					    jcLiteralInteger(GJ_SEQ_Init)));
	stmt = jcWhile(jcKeyword(symInternConst("true")),
		       jcBlock(jcSwitch(gj0SeqSwitchId(), jlst)));
	l2 = listList(JavaCode)(2, decl, stmt);

	return jcNLSeq(listNConcat(JavaCode)(l1, l2));
}

local GjSeqBucket 
gj0SeqBucketNew(AInt label)
{
	GjSeqBucket bucket = (GjSeqBucket) stoAlloc(OB_Other, sizeof(*bucket));
	
	bucket->label = label;
	bucket->list = listNil(JavaCode);
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
 * :: Method calls
 */


local JavaCode
gj0CCall(Foam call)
{
	JavaCode code;

	FoamSig sig = gj0FoamSigFrCCall(call);
	String id = gj0CCallStubName(sig);
	gjContext->progSigList = gj0CCallStubAdd(gjContext->progSigList, sig);
	code = jcApply(jcId(id), gj0GenList(call));

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
	case FOAM_SInt:
		return "I";
	case FOAM_HInt:
		return "H";
	case FOAM_NOp:
		return "X";
	case FOAM_Clos:
		return "C";
	default:
		return "?";
	}
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


/*
 * :: Default code
 */

local JavaCode 
gj0Default(Foam foam) 
{
	SExpr sx = foamToSExpr(foam);
	String s = sxiFormat(sx);

	return jcSpaceSeqV(2, jcNull(), jcComment(s));
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
	AInt    extra;

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
		return gj0Default(foam);
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
	
	return jcApplyMethodV(jcLiteralString(str), jcId(strCopy("toCharArray")), 0);
}


local JavaCode
gj0AElt(Foam foam)
{
	JavaCode exprCode;
	FoamTag tag;
	AInt type;
	Bool needsCast;
	
	tag = gj0FoamExprTypeWFmt(foam->foamAElt.expr, &type);
	needsCast = (type == 0);
	
	exprCode = gj0Gen(foam->foamAElt.expr);
	if (needsCast)
		exprCode = jcCast(jcArrayOf(gj0TypeFrFmt(FOAM_Arr, 
							 foam->foamAElt.baseType)),
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
	Foam ddecl = gjContext->formats->foamDFmt.argv[fmt];
	
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
	
	return gj0RecSet(gj0Gen(ref),
			 gj0Gen(rhs), ddecl, idx);
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
	JavaCode rhsJ = gj0Gen(rhs);

	return gj0RecSet(lvlId, rhsJ, ddecl, idx);
}

local JavaCode
gj0EElt(Foam foam)
{
	JavaCode env, ref;
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
	ref = gj0RecElt(env, ddecl, idx);

	return ref;
}

local JavaCode
gj0EEnv(Foam foam)
{
	JavaCode env, ref;
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
	JavaCode env, ref;
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
	ref = gj0RecSet(env, gj0Gen(rhs), ddecl, idx);

	return ref;
}

local JavaCode
gj0PushEnv(Foam foam)
{
	gj0FmtUse(foam->foamPushEnv.format);
	return jcConstructV(gj0TypeFrFmt(FOAM_Env, foam->foamPushEnv.format),
			    2,
			    gj0Gen(foam->foamPushEnv.parent),
			    gj0FmtId(foam->foamPushEnv.format));
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
	int nRets;

	inArgList = listNil(AInt);
	foamIter(ccall, elt, {
			FoamTag type = gj0FoamExprType(*elt);
			inArgList = listCons(AInt)(type, inArgList);
		});
	/* FIXME: Not sure how to get return types. */

	inArgList = listNReverse(AInt)(inArgList);
	if (gjContext->mfmt != 0) {
		Foam ddecl = gjContext->formats->foamDFmt.argv[gjContext->mfmt];
		retVals   = gj0FoamSigRets(ddecl, &nRets);
	}
	else {
		retVals = 0;
		nRets = 0;
	}

	FoamSig sig = foamSigNew(cdr(inArgList), ccall->foamCCall.type, nRets, retVals);
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
	JavaCode retType = gj0TypeFrFmt(sig->retType, 0);
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
	}
	else if (sig->retType == FOAM_NOp) {
		JavaCode tmp;
		resultVar = jcId(strCopy("result"));

		l1 = jcInitialisation(0, gj0Id(GJ_FoamValue), jcoCopy(resultVar),
				      call);
		tmp = jcApplyMethodV(jcoCopy(resultVar), jcId(strCopy("toMulti")), 0);
		l2 = jcReturn(tmp);
	}
	else {
		resultVar = jcId(strCopy("result"));
		l1 = jcInitialisation(0, gj0Id(GJ_FoamValue), jcoCopy(resultVar),
				      call);
		l2 = jcReturn(gj0TypeValueToObj(jcoCopy(resultVar), sig->retType, 0));
		jcoFree(resultVar);
	}

	body = jcNLSeqV(2, jcStatement(l1), jcStatement(l2));
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

local JavaCode
gj0Cast(Foam foam)
{
	Foam ref     = foam->foamCast.expr;
	FoamTag type = foam->foamCast.type;
	AInt    fmt;
	FoamTag iType = gj0FoamExprTypeWFmt(ref, &fmt);
	JavaCode jc = gj0Gen(ref);
	
	if (type == FOAM_Word) {
		return gj0CastObjToWord(jc, iType, fmt);
	}
	if (iType == FOAM_Word) {
		return gj0CastWordToObj(jc, type, fmt);
	}
	
}

/*
 * :: Casts
 */


local JavaCode
gj0CastWordToObj(JavaCode jc, FoamTag type, AInt fmt)
{
	/* Rely on Word == Value */
	return gj0TypeValueToObj(jc, type, fmt);
}

local JavaCode
gj0CastObjToWord(JavaCode val, FoamTag type, AInt fmt)
{
	switch (type) {
	case FOAM_Clos:
		return jcApplyMethod(val, 
			     jcId(strCopy("asWord")), 
			     listNil(JavaCode));
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
							   
	default:
		return jcCast(jcSpaceSeqV(2, gj0Id(GJ_FoamWord),
					  jcComment(strCopy(foamStr(type)))),
			      val);
	}
}


/*
 * :: Ids.
 * Collected together for readability
 */

struct gjIdInfo {
	GjId id;
	char *pkg;
	char *name;
};

struct gjIdInfo gjIdInfo[] = {
	{GJ_FoamWord,   "foamj", "Word"},
	{GJ_FoamClos,   "foamj", "Clos"},
	{GJ_FoamRecord, "foamj", "Record"},
	{GJ_FoamEnv,    "foamj", "Env"},

	{GJ_FoamFn,     "foamj", "Fn"},

	{GJ_FoamValue,  "foamj", "Value"},
	{GJ_Multi,      "foamj", "MultiRecord"},
	{GJ_FoamGlobals,"foamj", "Globals"},
	{GJ_Format,     "foamj", "Format"},
	{GJ_EnvRecord,  "foamj", "EnvRecord"},
};


local JavaCode
gj0Id(GjId id) 
{
	struct gjIdInfo *info = &gjIdInfo[id];
	assert(id == info->id);
	if (info->pkg != 0)
		return jcImportedId(strCopy(info->pkg),
				    strCopy(info->name));
	else
		return jcId(strCopy(info->name));
}



/*
 * :: BCall
 */

enum gj_BCallMethod {
	GJ_Keyword, 
	GJ_Apply,
	GJ_Op,
	GJ_OpMod,
	GJ_LitChar,
	GJ_LitString,
	GJ_LitFloat,
	GJ_LitInt,
	GJ_Const,
	GJ_NegConst,
	GJ_Cast,
	GJ_NotImpl, 
};

typedef Enum(gj_BCallMethod) GJ_BCallMethod;

struct gjBVal_info {
	FoamBValTag tag;
	GJ_BCallMethod method;
	AInt gjTag;
	char *c1;
	char *c2;
};

typedef struct gjBVal_info *GJBValInfo;

local GJBValInfo gj0BCallBValInfo(FoamBValTag tag);

local JavaCode gj0BCallKeyword (Foam foam);
local JavaCode gj0BCallApply   (Foam foam);
local JavaCode gj0BCallOp      (Foam foam);
local JavaCode gj0BCallOpMod   (Foam foam);
local JavaCode gj0BCallLitChar (Foam foam);
local JavaCode gj0BCallLitString(Foam foam);
local JavaCode gj0BCallLitFloat(Foam foam);
local JavaCode gj0BCallLitInt  (Foam foam);
local JavaCode gj0BCallConst   (Foam foam);
local JavaCode gj0BCallNegConst(Foam foam);
local JavaCode gj0BCallCast    (Foam foam);
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
	default:
		return gj0Default(foam);
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
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	args = gj0GenList(foam);
	return jcApplyMethod(jcId(strCopy(inf->c1)), jcId(strCopy(inf->c2)), args);
}

local JavaCode
gj0BCallOp(Foam foam) 
{
	JavaCodeList args;
	JavaCode r, extraArg;
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	args = gj0GenList(foam);
	
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
	args = gj0GenList(foam);
	assert(listLength(JavaCode)(args) == 3);

	extraArg = listElt(JavaCode)(args, 2);
	args = listList(JavaCode)(2, car(args), car(cdr(args)));
	r = jcOp(inf->gjTag, args);
	return jcBinOp(JCO_OP_Modulo, r, extraArg);

}

local JavaCode
gj0BCallLitChar(Foam foam)
{
	JavaCodeList args;
	GJBValInfo inf;

	inf = gj0BCallBValInfo(foam->foamBCall.op);
	
	return jcLiteralChar(strCopy(inf->c1));
}

local JavaCode
gj0BCallLitString(Foam foam) 
{
	JavaCodeList args;
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
gj0BCallCast(Foam foam) {
	return gj0BCallNotImpl(foam);
}

local JavaCode
gj0BCallNotImpl(Foam foam)
{
	return gj0Default(foam);
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
	{FOAM_BVal_CharNewline, GJ_LitChar, 0, "\\n"},
	{FOAM_BVal_CharTab,     GJ_LitChar, 0, "\\t"},

	{FOAM_BVal_CharMin, GJ_Const, 0, "Character", "MIN_VALUE" },
	{FOAM_BVal_CharMax, GJ_Const, 0, "Character", "MAX_VALUE" },

	{FOAM_BVal_CharIsDigit,  GJ_Apply, 0, "Character", "isDigit"},
	{FOAM_BVal_CharIsLetter, GJ_Apply, 0, "Character", "isLetter"},

	{FOAM_BVal_CharEQ, GJ_Op, JCO_OP_Equals},
	{FOAM_BVal_CharNE, GJ_Op, JCO_OP_NEquals},
	{FOAM_BVal_CharLT, GJ_Op, JCO_OP_LT},
	{FOAM_BVal_CharLE, GJ_Op, JCO_OP_LE},

	{FOAM_BVal_CharLower, GJ_Apply, 0, "Character", "toLower"},
	{FOAM_BVal_CharUpper, GJ_Apply, 0, "Character", "toUpper"},
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
	{FOAM_BVal_SFloPrev,     GJ_NotImpl},
	{FOAM_BVal_SFloNext,     GJ_NotImpl},
	{FOAM_BVal_SFloPlus,      GJ_Op, JCO_OP_Plus},
	{FOAM_BVal_SFloMinus,     GJ_Op, JCO_OP_Minus},
	{FOAM_BVal_SFloTimes,     GJ_Op, JCO_OP_Times},
	{FOAM_BVal_SFloTimesPlus, GJ_Op, JCO_OP_TimesPlus},
	{FOAM_BVal_SFloDivide,    GJ_Op, JCO_OP_Divide},
	{FOAM_BVal_SFloRPlus,     GJ_NotImpl},
	{FOAM_BVal_SFloRMinus,    GJ_NotImpl},
	{FOAM_BVal_SFloRTimes,    GJ_NotImpl},
	{FOAM_BVal_SFloRTimesPlus,GJ_NotImpl},
	{FOAM_BVal_SFloRDivide,   GJ_NotImpl},
	{FOAM_BVal_SFloDissemble, GJ_NotImpl},
	{FOAM_BVal_SFloAssemble,  GJ_NotImpl},

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
	{FOAM_BVal_DFloPrev,     GJ_NotImpl},
	{FOAM_BVal_DFloNext,     GJ_NotImpl},
	{FOAM_BVal_DFloPlus,      GJ_Op, JCO_OP_Plus},
	{FOAM_BVal_DFloMinus,     GJ_Op, JCO_OP_Minus},
	{FOAM_BVal_DFloTimes,     GJ_Op, JCO_OP_Times},
	{FOAM_BVal_DFloTimesPlus, GJ_Op, JCO_OP_TimesPlus},
	{FOAM_BVal_DFloDivide,    GJ_Op, JCO_OP_Divide},
	{FOAM_BVal_DFloRPlus,     GJ_NotImpl},
	{FOAM_BVal_DFloRMinus,    GJ_NotImpl},
	{FOAM_BVal_DFloRTimes,    GJ_NotImpl},
	{FOAM_BVal_DFloRTimesPlus,GJ_NotImpl},
	{FOAM_BVal_DFloRDivide,   GJ_NotImpl},
	{FOAM_BVal_DFloDissemble, GJ_NotImpl},
	{FOAM_BVal_DFloAssemble,  GJ_NotImpl},

	{FOAM_BVal_Byte0,        GJ_LitInt,  0, "0" },
	{FOAM_BVal_Byte1,        GJ_LitInt,  0, "1" },
	{FOAM_BVal_ByteMin,      GJ_LitInt,  0, "0"},
	{FOAM_BVal_ByteMax,      GJ_Const,       0, "Byte", "MAX_VALUE"},

	{FOAM_BVal_HInt0,        GJ_LitInt,  0,"0" },		      
	{FOAM_BVal_HInt1,        GJ_LitInt,  0,"1" },		      
	{FOAM_BVal_HIntMin,      GJ_LitInt,  0,"0"},		      
	{FOAM_BVal_HIntMax,      GJ_Const,       0,"Byte", "MAX_VALUE"},

	{FOAM_BVal_SInt0,        GJ_LitInt,  0,"0" },		      
	{FOAM_BVal_SInt1,        GJ_LitInt,  0,"1" },		      
	{FOAM_BVal_SIntMin,      GJ_LitInt,  0,"0"},		      
	{FOAM_BVal_SIntMax,      GJ_Const,       0,"Byte", "MAX_VALUE"},

	{FOAM_BVal_SIntIsZero,   GJ_Op, JCO_OP_Equals, "0"},
	{FOAM_BVal_SIntIsNeg,    GJ_Op, JCO_OP_LT,     "0"},
	{FOAM_BVal_SIntIsPos,    GJ_Op, JCO_OP_GT,     "0"},

	{FOAM_BVal_SIntIsEven,   GJ_NotImpl},
	{FOAM_BVal_SIntIsOdd,    GJ_NotImpl},

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
};
#if 0

 {FOAM_BVal_WordTimesDouble, GJB_FCall,0,"fiWordTimesDouble",  0},
 {FOAM_BVal_WordDivideDouble,GJB_FCall,0,"fiWordDivideDouble", 0},
 {FOAM_BVal_WordPlusStep,    GJB_FCall,0,"fiWordPlusStep",     0},
 {FOAM_BVal_WordTimesStep,   GJB_FCall,0,"fiWordTimesStep",    0},

 {FOAM_BVal_BInt0,        GJB_FCall,   0,"fiBInt0",        "fiBINT_0"},
 {FOAM_BVal_BInt1,        GJB_FCall,   0,"fiBInt1",        "fiBINT_1"},
 {FOAM_BVal_BIntIsZero,   GJB_FCall,   0,"fiBIntIsZero",   "fiBINT_IS_ZERO"},
 {FOAM_BVal_BIntIsNeg,    GJB_FCall,   0,"fiBIntIsNeg",    "fiBINT_IS_NEG"},
 {FOAM_BVal_BIntIsPos,    GJB_FCall,   0,"fiBIntIsPos",    "fiBINT_IS_POS"},
 {FOAM_BVal_BIntIsEven,   GJB_FCall,   1,"fiBIntEQ",       "fiBINT_IS_EVEN"},
 {FOAM_BVal_BIntIsOdd,    GJB_FCall,   1,"fiBIntNE",       "fiBINT_IS_ODD"},
 {FOAM_BVal_BIntIsSingle, GJB_FCall,   0,"fiBIntIsSingle", "fiBINT_IS_SINGLE"},
 {FOAM_BVal_BIntEQ,       GJB_FCall,   0,"fiBIntEQ",       "fiBINT_EQ"},
 {FOAM_BVal_BIntNE,       GJB_FCall,   0,"fiBIntNE",       "fiBINT_NE"},
 {FOAM_BVal_BIntLT,       GJB_FCall,   0,"fiBIntLT",       "fiBINT_LT"},
 {FOAM_BVal_BIntLE,       GJB_FCall,   0,"fiBIntLE",       "fiBINT_LE"},
 {FOAM_BVal_BIntNegate,   GJB_FCall,   0,"fiBIntNegate",   "fiBINT_NEGATE"},
 {FOAM_BVal_BIntPrev,     GJB_FCall,   1,"fiBIntMinus",    "fiBINT_MINUS1"},
 {FOAM_BVal_BIntNext,     GJB_FCall,   1,"fiBIntPlus",     "fiBINT_PLUS1"},
 {FOAM_BVal_BIntPlus,     GJB_FCall,   0,"fiBIntPlus",     "fiBINT_PLUS"},
 {FOAM_BVal_BIntMinus,    GJB_FCall,   0,"fiBIntMinus",    "fiBINT_MINUS"},
 {FOAM_BVal_BIntTimes,    GJB_FCall,   0,"fiBIntTimes",    "fiBINT_TIMES"},
 {FOAM_BVal_BIntTimesPlus,GJB_FCall,   0,"fiBIntTimesPlus","fiBINT_TIMES_PLUS"},
 {FOAM_BVal_BIntMod,      GJB_FCall,   0,"fiBIntMod",      "fiBINT_MOD"},
 {FOAM_BVal_BIntQuo,      GJB_FCall,   0,"fiBIntQuo",      "fiBINT_QUO"},
 {FOAM_BVal_BIntRem,      GJB_FCall,   0,"fiBIntRem",      "fiBINT_REM"},
 {FOAM_BVal_BIntDivide,   GJB_FCall,   0,"fiBIntDivide",   0},
 {FOAM_BVal_BIntGcd,      GJB_FCall,   0,"fiBIntGcd",      "fiBINT_GCD"},
 {FOAM_BVal_BIntSIPower,  GJB_FCall,   0,"fiBIntSIPower",  "fiBINT_SI_POWER"},
 {FOAM_BVal_BIntBIPower,  GJB_FCall,   0,"fiBIntBIPower",  "fiBINT_BI_POWER"},
 {FOAM_BVal_BIntPowerMod, GJB_FCall,   0,"fiBIntPowerMod", "fiBINT_POWER_MOD"},
 {FOAM_BVal_BIntLength,   GJB_FCall,   0,"fiBIntLength",   "fiBINT_LENGTH"},
 {FOAM_BVal_BIntShiftUp,  GJB_FCall,   0,"fiBIntShiftUp",  "fiBINT_SHIFT_UP"},
 {FOAM_BVal_BIntShiftDn,  GJB_FCall,   0,"fiBIntShiftDn",  "fiBINT_SHIFT_DN"},
 {FOAM_BVal_BIntShiftRem, GJB_FCall,   0,"fiBIntShiftRem", 0},
 {FOAM_BVal_BIntBit,      GJB_FCall,   0,"fiBIntBit",      "fiBINT_BIT"},

 {FOAM_BVal_PtrNil,       GJB_Id,      1,"fiPtrNil",        0},
 {FOAM_BVal_PtrIsNil,     GJB_FCall,   0,"fiPtrIsNil",      "fiPTR_IS_NIL"},
 {FOAM_BVal_PtrMagicEQ,   GJB_FCall,   0,"fiPtrMagicEQ",    "fiPTR_MAGIC_EQ"},
 {FOAM_BVal_PtrEQ,        GJB_EQ,      0,0,                 0},
 {FOAM_BVal_PtrNE,        GJB_NE,      0,0,                 0},

 {FOAM_BVal_FormatSFlo,   GJB_FCall,   0,"fiFormatSFlo",    "fiFORMAT_SFLO"},
 {FOAM_BVal_FormatDFlo,   GJB_FCall,   0,"fiFormatDFlo",    "fiFORMAT_DFLO"},
 {FOAM_BVal_FormatSInt,   GJB_FCall,   0,"fiFormatSInt",    "fiFORMAT_SINT"},
 {FOAM_BVal_FormatBInt,   GJB_FCall,   0,"fiFormatBInt",    "fiFORMAT_BINT"},

 {FOAM_BVal_ScanSFlo,     GJB_FCall,   0,"fiScanSFlo",      0},
 {FOAM_BVal_ScanDFlo,     GJB_FCall,   0,"fiScanDFlo",      0},
 {FOAM_BVal_ScanSInt,     GJB_FCall,   0,"fiScanSInt",      0},
 {FOAM_BVal_ScanBInt,     GJB_FCall,   0,"fiScanBInt",      0},

 {FOAM_BVal_SFloToDFlo,   GJB_FCall,   0,"fiSFloToDFlo",    "fiSFLO_TO_DFLO"},
 {FOAM_BVal_DFloToSFlo,   GJB_FCall,   0,"fiDFloToSFlo",    "fiDFLO_TO_SFLO"},
 {FOAM_BVal_ByteToSInt,   GJB_FCall,   0,"fiByteToSInt",    "fiBYTE_TO_SINT"},
 {FOAM_BVal_SIntToByte,   GJB_FCall,   0,"fiSIntToByte",    "fiSINT_TO_BYTE"},
 {FOAM_BVal_HIntToSInt,   GJB_FCall,   0,"fiHIntToSInt",    "fiHINT_TO_SINT"},
 {FOAM_BVal_SIntToHInt,   GJB_FCall,   0,"fiSIntToHInt",    "fiSINT_TO_HINT"},
 {FOAM_BVal_SIntToBInt,   GJB_FCall,   0,"fiSIntToBInt",    "fiSINT_TO_BINT"},
 {FOAM_BVal_BIntToSInt,   GJB_FCall,   0,"fiBIntToSInt",    "fiBINT_TO_SINT"},
 {FOAM_BVal_SIntToSFlo,   GJB_Cast,    0,gcFiSFlo,          "fiSINT_TO_SFLO"},
 {FOAM_BVal_SIntToDFlo,   GJB_Cast,    0,gcFiDFlo,          "fiSINT_TO_DFLO"},
 {FOAM_BVal_BIntToSFlo,   GJB_FCall,   0,"fiBIntToSFlo",    "fiBINT_TO_SFLO"},
 {FOAM_BVal_BIntToDFlo,   GJB_FCall,   0,"fiBIntToDFlo",    "fiBINT_TO_DFLO"},
 {FOAM_BVal_PtrToSInt,    GJB_FCall,   0,"fiPtrToSInt",     "fiPTR_TO_SINT"},
 {FOAM_BVal_SIntToPtr,    GJB_FCall,   0,"fiSIntToPtr",     "fiSINT_TO_PTR"},

 {FOAM_BVal_ArrToSFlo,    GJB_FCall,   0,"fiArrToSFlo",     "fiARR_TO_SFLO"},
 {FOAM_BVal_ArrToDFlo,    GJB_FCall,   0,"fiArrToDFlo",     "fiARR_TO_DFLO"},
 {FOAM_BVal_ArrToSInt,    GJB_FCall,   0,"fiArrToSInt",     "fiARR_TO_SINT"},
 {FOAM_BVal_ArrToBInt,    GJB_FCall,   0,"fiArrToBInt",     "fiARR_TO_BINT"},

 {FOAM_BVal_PlatformRTE,  GJB_FCall,   0,"fiPlatformRTE",   0},
 {FOAM_BVal_PlatformOS,   GJB_FCall,   0,"fiPlatformOS",    0},

 {FOAM_BVal_Halt,         GJB_FCall,   0,"fiHalt",          0},

 {FOAM_BVal_RoundZero,    GJB_FCall,   0,"fiRoundZero",    "FI_ROUND_ZERO"},
 {FOAM_BVal_RoundNearest, GJB_FCall,   0,"fiRoundNearest", "FI_ROUND_NEAREST"},
 {FOAM_BVal_RoundUp,      GJB_FCall,   0,"fiRoundUp",      "FI_ROUND_UP"},
 {FOAM_BVal_RoundDown,    GJB_FCall,   0,"fiRoundDown",    "FI_ROUND_DOWN"},
 {FOAM_BVal_RoundDontCare,GJB_FCall,   0,"fiRoundDontCare","FI_ROUND_DONT_CARE"},

 {FOAM_BVal_SFloTruncate,  GJB_FCall,  0,"fiSFloTruncate",  0},
 {FOAM_BVal_SFloFraction,  GJB_FCall,  0,"fiSFloFraction",  0},
 {FOAM_BVal_SFloRound,     GJB_FCall,  0,"fiSFloRound",     0},

 {FOAM_BVal_DFloTruncate,  GJB_FCall,  0,"fiDFloTruncate",  0},
 {FOAM_BVal_DFloFraction,  GJB_FCall,  0,"fiDFloFraction",  0},
 {FOAM_BVal_DFloRound,     GJB_FCall,  0,"fiDFloRound",     0},

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
#if EDIT_1_0_n1_06
 {FOAM_BVal_NewExportTable, GJB_FCall, 0, "fiNewExportTable", 0},
 {FOAM_BVal_AddToExportTable, GJB_FCall, 0, "fiAddToExportTable", 0},
 {FOAM_BVal_FreeExportTable, GJB_FCall, 0, "fiFreeExportTable", 0},
#endif
#if EDIT_1_0_n1_AB
 /* This BVal must NEVER be seen by genc ... */
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
	assert(inf->tag == tag);
	return inf;
}


/*
 * :: Names
 */
struct gjSpecCharId_info {
	char c;
	char *s;
};
struct gjSpecCharId_info gjSpecCharIdTable[];

char *gjCharIds[CHAR_MAX];

local void
gj0NameInit() 
{
	struct gjSpecCharId_info *p = &gjSpecCharIdTable[0];

	while (p->c != '\0') {
		gjCharIds[p->c] = p->s;
		p++;
	}
}

local String
gj0Name(String prefix, Foam fmt, int idx)
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
	char *p;

	flg = false;
	p = fmName;
	while (*p != 0) {
		char *repl = gjCharIds[*p];
		if (repl != 0)
			flg = true;
		p++;
	}
	if (!flg)
		return fmName;

	p = fmName;
	buf = bufNeed(buf, strlen(fmName));
	while (*p != 0) {
		char *repl = gjCharIds[*p];
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

