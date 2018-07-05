#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "gf_util.h"
#include "gf_prog.h"
#include "gf_java.h"
#include "gf_syme.h"
#include "javasig.h"
#include "of_inlin.h"
#include "table.h"
#include "tinfer.h"
#include "tform.h"
#include "tqual.h"
#include "stab.h"
#include "strops.h"
#include "symbol.h"
#include "spesym.h"
#include "util.h"

local AInt gfjFindConst(Syme syme);
local Foam gfjImportApply(Syme syme);
local AInt gfjImportApplyInner(Syme syme, AInt fmtNum);
local Foam gfjImportConstructor(Syme syme);
local Foam gfjImportStaticCall(Syme syme);
local FoamList gfjProgAddParams(TForm tf);

local TForm gfjPCallArgBaseJavaType(TForm tf);
local TForm gfjPCallRetBaseJavaType(TForm tf);
local FoamTag gfjPCallFoamType(TForm tf, AInt *pfmt);
local AInt gfjPCallFoamTypeDDecl(TForm tf);
local Foam gfjPCallFoamToJava(TForm tf, Foam foam);
local Foam gfjPCallJavaToFoam(TForm tf, Foam foam);
local AInt gfjPCallDeclImport(TForm tf, TForm this);
local AInt gfjPCallDeclExport(TForm tf, FoamProtoTag tag);
local Foam gfjPCallDeclArg(TForm tf);
local Foam gfjPCallDeclExn(TForm tf);
local Foam gfjPCallJavaToFoamForExport(TForm tf, Foam foam);
local Foam gfjPCallFoamToJavaForExport(TForm tf, Foam foam);

local AInt gj0ClassDDeclFull(String fullyQualifiedName);
local AInt gj0ClassDDecl(ForeignOrigin origin, String clsName);
local Foam gj0ClassDecl(ForeignOrigin origin, String clsName);

local String gfjDeclMethodNameForType(TForm tf, String methodName);
local String gfjDeclMethodName(String typeName, ForeignOrigin forg, String methodName);
local String gfjDeclClassNameForType(TForm tf);
local String gfjDeclClassName(String typeName, ForeignOrigin forg);

local AInt gfjExportDecoder(TForm tf);
local AInt gfjExportEncoder(TForm tf);

static Table gfjExportEncoderForTForm;
static Table gfjExportDecoderForTForm;

void
gfjInit()
{
	gfjExportEncoderForTForm = tblNew((TblHashFun) tfHash, (TblEqFun) tfEqual);
	gfjExportDecoderForTForm = tblNew((TblHashFun) tfHash, (TblEqFun) tfEqual);
}

void
gfjFini()
{
	tblFree(gfjExportEncoderForTForm);
	tblFree(gfjExportDecoderForTForm);
	gfjExportEncoderForTForm = NULL;
	gfjExportDecoderForTForm = NULL;
}


Foam
gfjGetImport(Syme syme)
{
	if (symeIsJavaApply(syme))
		return gfjImportApply(syme);
	else if (symeIsJavaConstructor(syme))
		return gfjImportConstructor(syme);
	else
		return gfjImportStaticCall(syme);
}

void
gfjVarImport(Syme syme, Stab stab)
{
}

local Foam
gfjImportApply(Syme syme)
{
	GenFoamState saved;
	Foam prog;
	String fnName;
	AInt fmtNum, innerConstNum, constNum;
	fnName  = strCopy(symeJavaApplyName(syme));

	constNum = gen0NumProgs;
	
	prog = gen0ProgInitEmpty(fnName, NULL);
	saved = gen0ProgSaveState(PT_ExFn);

	fmtNum = gen0FormatNum--;

	innerConstNum = gfjImportApplyInner(syme, fmtNum);

	gen0AddParam(foamNewDecl(FOAM_Word, strCopy("this"), emptyFormatSlot));

	gen0AddStmt(foamNewDef(foamNewLex(int0, int0), foamNewPar(int0)), NULL);
	gen0AddStmt(foamNewReturn(foamNewCast(FOAM_Word,
					      foamNewClos(foamNewEnv(int0), foamNewConst(innerConstNum)))), NULL);

	gen0ProgAddFormat(fmtNum, 
			  foamNewDDecl(FOAM_DDecl_LocalEnv, 
				       foamNewDecl(FOAM_Word, strCopy("this"), emptyFormatSlot),
				       NULL));

	gen0ProgFiniEmpty(prog, FOAM_Word, emptyFormatSlot);
        foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);
	gen0ProgRestoreState(saved);
	genSetConstNum(syme, -1, constNum, true);
	return foamNewClos(foamNewEnv(int0), foamNewConst(constNum));
}

local AInt
gfjImportApplyInner(Syme syme, AInt fmtNum)
{
	GenFoamState saved;
	TForm exporter, innerTf;
	FoamList params;
	Foam gdecl, prog, pcall, temp, op;
	FoamTag retType;
	String fnName, globName;
	AInt gnum, constnum;
	AInt clsFmt;
	
	constnum = gen0NumProgs;

	exporter = symeExporter(syme);
	innerTf = tfMapRet(symeType(syme));

	clsFmt = gfjPCallFoamTypeDDecl(exporter);

	globName = gfjDeclMethodNameForType(exporter, symeJavaApplyName(syme));

	gdecl = foamNewGDecl(FOAM_Clos, globName,
			     gfjPCallDeclImport(innerTf, tfMapArgN(symeType(syme), 0)),
			     FOAM_GDecl_Import, FOAM_Proto_JavaMethod);
	gnum = gen0AddGlobal(gdecl);
	fnName  = strPrintf("%s-inner", symeJavaApplyName(syme));

	prog = gen0ProgInitEmpty(fnName, NULL);
	saved = gen0ProgSaveState(PT_ExFn);
	
	temp = gen0TempLocal0(FOAM_JavaObj, clsFmt);
	gen0AddStmt(foamNewDef(foamCopy(temp), foamNewCast(FOAM_JavaObj, foamNewLex(1, int0))), NULL);
 
	params = gfjProgAddParams(innerTf);
	params = listCons(Foam)(foamCopy(temp), params);
	op = foamNewGlo(gnum);

	retType = tfMapRetc(innerTf) == 0 ? FOAM_NOp: FOAM_Word;
	if (retType == FOAM_NOp) {
		pcall = foamNewPCallOfList(FOAM_Proto_JavaMethod,
					   FOAM_NOp, op, params);
		gen0AddLexLevels(pcall, 2);
		gen0AddStmt(pcall, NULL);
	}
	else {
		FoamTag type = gfjPCallFoamType(tfMapRet(innerTf), NULL);
		Foam pcall = foamNewPCallOfList(FOAM_Proto_JavaMethod,
					   type, op, params);
		Foam retval = gfjPCallJavaToFoam(tfMapRet(innerTf), pcall);
		gen0AddLexLevels(retval, 2);
		gen0AddStmt(foamNewReturn(retval), NULL);
	}
	foamFree(temp);
	gen0ProgPushFormat(fmtNum);
	gen0ProgPushFormat(emptyFormatSlot);

	gen0ProgFiniEmpty(prog, retType, emptyFormatSlot);
        foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);

	gen0ProgRestoreState(saved);
	return constnum;
}

local Foam
gfjImportConstructor(Syme syme)
{
	GenFoamState saved;
	TForm exporter;
	FoamList params;
	Foam prog, gdecl, pcall;
	String fnName, globName;
	AInt fmtNum, innerConstNum, constNum, gnum;
	
	exporter = symeExporter(syme);
	globName = gfjDeclClassNameForType(exporter);
	fnName = strCopy(globName);

	constNum = gen0NumProgs;
	
	gdecl = foamNewGDecl(FOAM_Clos, globName, gfjPCallDeclImport(symeType(syme), NULL),
			     FOAM_GDecl_Import, FOAM_Proto_JavaConstructor);
	gnum = gen0AddGlobal(gdecl);

	prog = gen0ProgInitEmpty(fnName, NULL);
	saved = gen0ProgSaveState(PT_ExFn);

	params = gfjProgAddParams(symeType(syme));

	pcall = foamNewPCallOfList(FOAM_Proto_JavaConstructor, 
				   gfjPCallFoamType(exporter, NULL),
				   foamNewGlo(gnum), params);

	gen0AddLexLevels(pcall, 1);
	gen0ProgPushFormat(emptyFormatSlot);

	gen0AddStmt(foamNewReturn(foamNewCast(FOAM_Word, pcall)), NULL);

	gen0ProgFiniEmpty(prog, FOAM_Word, emptyFormatSlot);
        foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);
	gen0ProgRestoreState(saved);

	genSetConstNum(syme, -1, constNum, true);
	return foamNewClos(foamNewEnv(int0), foamNewConst(constNum));
}

local Foam
gfjImportStaticCall(Syme syme)
{
	GenFoamState saved;
	ForeignOrigin forg;
	TForm exporter;
	Syme esyme;
	FoamList params;
	Foam prog, gdecl, pcall;
	FoamTag retType;
	String fnName, globName;
	AInt fmtNum, innerConstNum, constNum, gnum;
	
	exporter = symeExporter(syme);
	fnName  = strCopy(symeString(syme));

	globName = gfjDeclMethodNameForType(exporter, symeString(syme));
	
	constNum = gen0NumProgs;
	
	gdecl = foamNewGDecl(FOAM_Clos, globName, gfjPCallDeclImport(symeType(syme), NULL),
			     FOAM_GDecl_Import, FOAM_Proto_Java);
	gnum = gen0AddGlobal(gdecl);

	prog = gen0ProgInitEmpty(fnName, NULL);
	saved = gen0ProgSaveState(PT_ExFn);

	params = gfjProgAddParams(symeType(syme));
	retType = gfjPCallFoamType(tfMapRet(symeType(syme)), NULL);
	pcall = foamNewPCallOfList(FOAM_Proto_Java, 
				   retType,
				   foamNewGlo(gnum), params);

	gen0AddLexLevels(pcall, 1);
	gen0ProgPushFormat(emptyFormatSlot);
	gen0AddStmt(foamNewReturn(foamNewCast(FOAM_Word, pcall)), NULL);

	gen0ProgFiniEmpty(prog, FOAM_Word, emptyFormatSlot);
        foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);
	gen0ProgRestoreState(saved);

	genSetConstNum(syme, -1, constNum, true);
	return foamNewClos(foamNewEnv(int0), foamNewConst(constNum));
}

local FoamList
gfjProgAddParams(TForm tf)
{
	FoamList params = listNil(Foam);
	int i;

	for (i=0; i<tfMapArgc(tf); i++) {
		AInt idx = gen0AddParam(foamNewDecl(FOAM_Word, strPrintf("%d", i),
						    emptyFormatSlot));
		Foam converted = gfjPCallFoamToJava(tfMapArgN(tf, i), foamNewPar(idx));
		params = listCons(Foam)(converted, params);
	}
	return listNReverse(Foam)(params);
}

/*
 * :: Java representation
 *
 * Aldor types used in Java imports are assumed to have toJava and fromJava methods
 * which convert to and from java respectively.
 */
local FoamTag
gfjPCallFoamType(TForm tf, AInt *pfmt)
{
	tf = tfIgnoreExceptions(tf);
	if (tfIsJavaImport(tf)) {
		if (pfmt != NULL) {
			*pfmt = gfjPCallFoamTypeDDecl(tf);
		}
		return FOAM_JavaObj;
	}
	else if (stabIsForeignExport(gen0State->stab, tf)) {
		if (pfmt != NULL) {
			ForeignOrigin forg = stabForeignExportLocation(gen0State->stab, tf);
			*pfmt = gj0ClassDDecl(forg, symeString(tfIdSyme(tf)));
		}
		return FOAM_JavaObj;
	}
	else {
		Syme javaToSelf = tfGetDomImport(tf, symString(ssymTheFromJava),
						 tfIsJavaDecoder);
		if (javaToSelf != NULL) {
			TForm convTf = symeType(javaToSelf);
			tfFollow(convTf);
			return gfjPCallFoamType(tfMapArg(convTf), pfmt);
		}
		else {
			return gen0Type(tf, pfmt);
		}
	}
}


local Foam
gfjPCallFoamToJava(TForm tf, Foam foam)
{
	tf = tfIgnoreExceptions(tf);

	if (tfIsJavaImport(tf)) {
		return foamNewCast(FOAM_JavaObj, foam);
	}
	else if (stabIsForeignExport(gen0State->stab, tf)) {
		return gfjPCallFoamToJavaForExport(tf, foam);
	}
	else {
		if (tfIsId(tf) && symeExtension(tfIdSyme(tf))) {
			Syme syme = tfIdSyme(tf);
			syme = symeExtensionFull(syme);
			tf = tfFrSyme(stabFile(), syme);
		}

		Syme selfToJava = tfGetDomImport(tf,
						 symString(ssymTheToJava),
						 tfIsJavaEncoder);
		if (selfToJava != NULL) {
			TForm convTf = symeType(selfToJava);
			tfFollow(convTf);
			TForm retTf = tfMapRetN(convTf, 0);
			FoamTag type = gen0Type(retTf, NULL);
			Foam call;
			call = gen0ExtendSyme(selfToJava);
			call = foamNewCCall(type, call, foam, NULL);
			return gfjPCallFoamToJava(retTf, call);
		}
		else {
			return foam;
		}
	}
}

local Foam
gfjPCallJavaToFoam(TForm tf, Foam foam)
{
	tf = tfIgnoreExceptions(tf);

	if (tfIsJavaImport(tf)) {
		return foamNewCast(FOAM_Word, foam);
	}
	else if (stabIsForeignExport(gen0State->stab, tf)) {
		return gfjPCallJavaToFoamForExport(tf, foam);
	}
	else {
		if (tfIsId(tf) && symeExtension(tfIdSyme(tf))) {
			Syme syme = tfIdSyme(tf);
			syme = symeExtensionFull(syme);
			tf = tfFrSyme(stabFile(), syme);
		}

		Syme javaToSelf = tfGetDomImport(tf, 
						 symString(ssymTheFromJava),
						 tfIsJavaDecoder);
		Foam call;
		call = gen0ExtendSyme(javaToSelf);
		call = foamNewCCall(FOAM_Word, call, foam, NULL);
		
		return call;
	}
}

local Foam
gfjPCallJavaToFoamForExport(TForm tf, Foam foam)
{
	AInt gnum = gfjExportDecoder(tf);
	return foamNewPCallOfList(FOAM_Proto_JavaMethod, FOAM_Word,
				  foamNewGlo(gnum),
				  listSingleton(Foam)(foam));
}

local Foam
gfjPCallFoamToJavaForExport(TForm tf, Foam foam)
{
	AInt gnum = gfjExportEncoder(tf);
	return foamNewPCallOfList(FOAM_Proto_JavaConstructor, FOAM_JavaObj,
				  foamNewGlo(gnum),
				  listSingleton(Foam)(foam));
}


local AInt
gfjPCallDeclImport(TForm tf, TForm this)
{
	TForm retTf;
	FoamList decls;
	Foam ddecl, retdecl, exndecl;
	int i;

	decls = listNil(Foam);

	if (this != NULL) {
		TForm tfThis = gfjPCallArgBaseJavaType(this);
		Foam decl = gfjPCallDeclArg(tfThis);
		decls = listCons(Foam)(decl, decls);
	}

	for (i=0; i<tfMapArgc(tf); i++) {
		TForm tfi = gfjPCallArgBaseJavaType(tfMapArgN(tf, i));
		Foam decl = gfjPCallDeclArg(tfi);
		decls = listCons(Foam)(decl, decls);
	}

	retTf = tfMapRet(tf);
	retdecl = gfjPCallDeclArg(gfjPCallRetBaseJavaType(retTf));
	exndecl = gfjPCallDeclExn(tfIsExcept(retTf) ? tfExceptExcept(retTf): NULL);

	ddecl = javaSigNew(retdecl, exndecl, listNReverse(Foam)(decls));

	return gen0AddRealFormat(ddecl);

}

local AInt
gfjPCallDeclExport(TForm tf, FoamProtoTag protocol)
{
	FoamList decls;
	Foam ddecl, retdecl, exndecl;
	int i;

	decls = listNil(Foam);

	for (i=0; i<tfMapArgc(tf); i++) {
		TForm tfi = gfjPCallArgBaseJavaType(tfMapArgN(tf, i));
		Foam decl = gfjPCallDeclArg(tfi);
		decls = listCons(Foam)(decl, decls);
	}

	retdecl = protocol == FOAM_Proto_JavaConstructor
		? foamNewDecl(FOAM_Word, strCopy(""), emptyFormatSlot)
		: gfjPCallDeclArg(gfjPCallRetBaseJavaType(tfMapRet(tf)));

	exndecl = gfjPCallDeclExn(tfIsExcept(tfMapRet(tf)) ? tfExceptExcept(tfMapRet(tf)): NULL);

	ddecl = javaSigNew(retdecl, exndecl, listNReverse(Foam)(decls));

	return gen0AddRealFormat(ddecl);
}

local TForm
gfjPCallArgBaseJavaType(TForm tf)
{
	int i = 0;
	while (i<5) {
		Syme enc = tfGetDomExport(tf, symString(ssymTheFromJava), tfIsJavaDecoder);
		if (enc == NULL) {
			return tf;
		}
		TForm retType = tfMapArg(symeType(enc));
		tf = retType;
		i++;
	}

	bug("Recursive fromJava?");
	return NULL;
}

local TForm
gfjPCallRetBaseJavaType(TForm tf)
{
	int i = 0;
	tf = tfIgnoreExceptions(tf);

	while (i<5) {
		Syme enc = tfGetDomExport(tf, symString(ssymTheToJava), tfIsJavaEncoder);
		if (enc == NULL) {
			return tf;
		}
		TForm retType = tfMapRet(symeType(enc));
		tf = retType;
		i++;
	}

	bug("Recursive toJava?");
	return NULL;
}

local Foam
gfjPCallDeclArg(TForm tf)
{
	String id = "";
	Foam decl;

	if (tfIsDeclare(tf)) {
		id = symString(tfDeclareId(tf));
		tf = tfDeclareType(tf);
	}

	if (tfIsJavaImport(tf)) {
		AInt fmt = gfjPCallFoamTypeDDecl(tf);
		decl = foamNewDecl(FOAM_JavaObj, strCopy(id), fmt);
	}
	else if (stabIsForeignExport(gen0State->stab, tf)) {
		assert(tfIsId(tf));
		ForeignOrigin forg = stabForeignExportLocation(gen0State->stab, tf);
		Syme syme = abSyme(tfExpr(tf));
		AInt fmt = gj0ClassDDecl(forg, symeString(syme));
		decl = foamNewDecl(FOAM_JavaObj, strCopy(id), fmt);
	}
	else {
		FoamTag type;
		AInt fmt;
		type = gen0Type(tf, &fmt);
		decl = foamNewDecl(type, strCopy(id), fmt);
	}

	return decl;
}

local Foam
gfjPCallDeclExn(TForm tf)
{
	Foam decl;

	if (tf == NULL) {
		decl = foamNewDecl(FOAM_NOp, strCopy(""), emptyFormatSlot);
	}
	else {
		// For the moment, everything maps to exception.
		decl = foamNewDecl(FOAM_JavaObj, strCopy("exn"),
				   gj0ClassDDeclFull(strCopy("java.lang.Exception")));
	}
	return decl;
}

local AInt gfjPCallFoamTypeDDeclAbSyn(AbSyn);
local AInt gfjPCallFoamTypeDDeclId   (AbSyn);

local AInt
gfjPCallFoamTypeDDecl(TForm tf)
{
	return gfjPCallFoamTypeDDeclAbSyn(tfExpr(tf));
}

local AInt
gfjPCallFoamTypeDDeclAbSyn(AbSyn ab)
{
	AInt fmt;
	if (abIsId(ab)) {
		return gfjPCallFoamTypeDDeclId(ab);
	}
	else if (abIsApply(ab)) {
		Syme opSyme = abSyme(ab->abApply.op);
		ForeignOrigin forg = symeForeign(opSyme);
		String name = gfjDeclClassName(symeString(opSyme), forg);
		Foam opDecl = foamNewDecl(FOAM_JavaObj, name, int0);
		FoamList args = listNil(Foam);
		int i;
		for (i=0; i<abApplyArgc(ab); i++) {
			TForm argTf = abTForm(abApplyArg(ab, i))
				? abTForm(abApplyArg(ab, i))
				: tiGetTForm(gen0State->stab, abApplyArg(ab, i));
			AInt fmt;
			FoamTag tag = gfjPCallFoamType(argTf, &fmt);
			Foam decl = foamNewDecl(tag, aStrPrintf("%d", i), fmt);
			args = listCons(Foam)(decl, args);
		}
		args = listNReverse(Foam)(args);
		return gen0AddRealFormat(foamNewDDeclOfList(FOAM_DDecl_JavaClass,
							    listCons(Foam)(opDecl, args)));
	}
	else {
		afprintf(dbOut, "%pAbSyn\n", ab);
		bug("Unknown conversion");
	}
}

local AInt
gfjPCallFoamTypeDDeclId(AbSyn id)
{
	TForm tf = abTForm(id) ? abTForm(id): tiGetTForm(gen0State->stab, id);
	Syme syme = tfIdSyme(tf);
	ForeignOrigin forg = symeForeign(syme);
	String name = gfjDeclClassName(symeString(syme), forg);
	AInt fmt = gj0ClassDDecl(forg, symeString(syme));

	return fmt;
}


local AInt
gj0ClassDDecl(ForeignOrigin origin, String clsName)
{
	String name = gfjDeclClassName(clsName, origin);
	return gj0ClassDDeclFull(name);
}

local AInt
gj0ClassDDeclFull(String fullyQualifiedName)
{
	Foam decl = foamNewDecl(FOAM_JavaObj, fullyQualifiedName, int0);

	return gen0AddRealFormat(foamNewDDecl(FOAM_DDecl_JavaClass, decl, NULL));
}

local Foam
gj0ClassDecl(ForeignOrigin origin, String clsName)
{
	return foamNewDecl(FOAM_JavaObj, strCopy(clsName), gj0ClassDDecl(origin, clsName));
}

/*
 * :: Export to java
 */

local void gfjExportToJavaClass(AbSyn syme, ForeignOrigin forg);
local void gfjExportToJavaSyme(TForm tf, Syme syme, Foam clos);
local String gfjExportToJavaSymeMethodName(Syme syme);
local AInt gfjExportToJavaSymeProtocol(TForm exporter, Syme syme);
local Foam gfjExportToJavaFn(ForeignOrigin forg, String clssName, TForm tf, Syme syme);

/*
 * AbSyn: export Foo to Foreign Java("aldor.types")
 *
 * FOAM_Proto_JavaType:
 *    (DDecl Globals
 *       (GDecl Word "aldor.types.Foo" JavaMethod)
 * ...
 * (Def (Glo n) (Clos 0 (const blah))
 */

void
gfjExportToJava(AbSyn absyn, AbSyn dest)
{
	AbSyn	name = abDefineeId(absyn);
	Syme	syme = abSyme(name);
	TForm   tf;
	Foam    decl;
	FoamTag	rtype;
	AInt	index;
	ForeignOrigin forg = forgFrAbSyn(abApplyArg(dest, 0));

	if (abIsId(absyn) && abSyme(absyn) != NULL) {
		gfjExportToJavaClass(absyn, forg);
	}
	else {
		bug("Unknown java export");
	}
}

local void
gfjExportToJavaClass(AbSyn absyn, ForeignOrigin forg)
{
	TForm tf;
	SymeList imports;
	String clssName;

	if (!abIsId(absyn)) {
		return;
	}

	clssName = symString(abIdSym(absyn));

	tf = tiGetTForm(stabFile(), absyn);
	// Normal imports
	imports = tfGetDomImports(tf);
	while (imports != listNil(Syme)) {
		Syme import = car(imports);
		imports = cdr(imports);
		if (tfJavaCanExport(gen0State->stab, tf, symeType(import))) {
			Foam clos = gfjExportToJavaFn(forg, clssName, tf, import);
			gfjExportToJavaSyme(tf, import, clos);
		}
	}

	TQualList cascades = tfGetDomCascades(tf);
	// Qualified imports
	while (cascades != listNil(TQual)) {
		TQual tq = car(cascades);

		// Currently, ignore imports from qualified sources
		// - more code, and we only want toString at the moment.
		if (tqIsQualified(tq)) continue;
		SymeList sl = tfGetDomImports(tqBase(tq));

		while (sl != listNil(Syme)) {
			Syme import = car(sl);
			if (strEqual(symeString(import), "toString")
			    && tfJavaCanExport(gen0State->stab, tf, symeType(import))) {
				Foam clos = gfjExportToJavaFn(forg, clssName, tf, import);
				gfjExportToJavaSyme(tf, import, clos);
			}
			sl = cdr(sl);
		}
		cascades = cdr(cascades);
	}
}

local void
gfjExportToJavaSyme(TForm exporter, Syme syme, Foam clos)
{
	TForm tf = symeType(syme);
	Foam decl;
	AInt index;

	assert(syme);

	if (!tfIsMap(tf))
		return;

	/* Generate a java style function matching this symbol's type */
	ForeignOrigin forg = stabForeignExportLocation(gen0State->stab, exporter);
	AInt protocol = gfjExportToJavaSymeProtocol(exporter, syme);
	AInt declFmt = gfjPCallDeclExport(tf, protocol);
	AInt rtype = gen0Type(tfMapRet(tf), NULL);
	String methodName = gfjExportToJavaSymeMethodName(syme);
	String foamName = gfjDeclMethodName(symeString(tfIdSyme(exporter)), forg, methodName);

	strFree(methodName);
	decl = foamNewGDecl(FOAM_Clos, foamName,
			    declFmt, FOAM_GDecl_Export, protocol);

	foamGDeclSetRType(decl, rtype);

	index = gen0AddGlobal(decl);
	gen0BuiltinExports = listCons(AInt)(index, gen0BuiltinExports);
	gen0BuiltinExports = listCons(AInt)(int0, gen0BuiltinExports);

	gen0AddStmt(foamNewSet(foamNewGlo(index), clos), NULL);
}

local AInt
gfjExportToJavaSymeProtocol(TForm exporter, Syme syme)
{
	TForm tf = symeType(syme);
	assert(tfIsMap(tf));
	assert(symeIsImport(syme));

	if (tfMapRetc(tf) == 1
	    && symeId(syme) == ssymTheNew
	    && tfEqual(exporter, tfMapRetN(tf, 0))) {
		return FOAM_Proto_JavaConstructor;
	}
	else if (tfMapArgc(tf) > 0 && tfEqual(exporter, tfMapArgN(tf, 0))) {
		return FOAM_Proto_JavaMethod;
	}
	else {
		return FOAM_Proto_Java;
	}
}


local String
gfjExportToJavaSymeMethodName(Syme syme)
{
	TForm fullType = symeType(syme);
	TForm type = tfMapArgc(fullType) == 0 ? tfNone() : tfMapRetN(fullType, 0);
	String name = symeString(syme);

	if (name[strLength(name) - 1] == '?'
	    && tfEqual(type, tfBoolean)) {
		name = strPrintf("is%c%s", toupper(name[0]), name+1);
		name[strLength(name)-1] = '\0';
	}
	if (strEqual(name, "=")) {
		return strCopy("equals");
	}

	if (strEqual(name, "~=")) {
		return strCopy("notEquals");
	}

	return strCopy(name);
}

local Foam
gfjExportToJavaFn(ForeignOrigin forg, String clssName, TForm this, Syme syme)
{
	GenFoamState saved;
	FoamList     args;
	TForm        tf;
	Foam         prog, clos;
	String       fnName;
	AInt         index;
	int          i;

	AInt protocol = gfjExportToJavaSymeProtocol(this, syme);

	tf = symeType(syme);
	fnName = aStrPrintf("%s-wrapper", symeString(syme));
	clos = gen0ProgClosEmpty();
	prog = gen0ProgInitEmpty(fnName, NULL);
	index = car(gen0State->envFormatStack);
	saved = gen0ProgSaveState(PT_ExFn);

	/*
	 * Generate a method which calls the syme, with args matching outer.
	 * Idea is that this can (probably) be inlined away.
	 */

	for (i=0; i<tfMapArgc(tf); i++) {
		TForm tfi = gfjPCallArgBaseJavaType(tfMapArgN(tf, i));
		Foam decl = tfEqual(tfi, this)
			? gj0ClassDecl(forg, clssName)
			: gfjPCallDeclArg(tfi);
		gen0AddParam(decl);
	}

	args = listNil(Foam);
	for (i=0; i<tfMapArgc(tf); i++) {
		TForm tfi = tfMapArgN(tf, i);
		Foam foam;
		if (tfEqual(tfi, this)) {
			foam = foamNewPCall(FOAM_Proto_JavaMethod,
					    FOAM_Word,
					    gen0CharArray("rep"), foamNewPar(i), NULL);
		}
		else if (stabIsForeignExport(gen0State->stab, tfi)) {
			foam = foamNewPCall(FOAM_Proto_JavaMethod,
					    FOAM_Word,
					    gen0CharArray("rep"), foamNewPar(i), NULL);
		}
		else {
			foam = gfjPCallJavaToFoam(tfi, foamNewPar(i));
		}
		args = listCons(Foam)(foam, args);
	}
	args = listNReverse(Foam)(args);

	TForm retTf = tfMapRet(tf);
	FoamTag retType = gen0Type(retTf, NULL);
	Foam ccall = foamNewCCallOfList(retType, gen0Syme(syme), args);

	Foam retVal;
	AInt fmt;
	FoamTag type;

	retTf = tfIgnoreExceptions(retTf);
	if (protocol == FOAM_Proto_JavaConstructor) {
		type = FOAM_Word;
		fmt = emptyFormatSlot;
		gen0AddStmt(foamNewReturn(ccall), NULL);
	}
	else if (tfIsNone(retTf)) {
		gen0AddStmt(ccall, NULL);
		type = FOAM_NOp;
		fmt = emptyFormatSlot;
	}
	else if (tfEqual(retTf, this)) {
		retVal = foamNewPCall(FOAM_Proto_JavaConstructor, FOAM_JavaObj,
				      gen0CharArray(gfjDeclClassName(clssName, forg)),
				      ccall, NULL);
		Foam retDecl = gj0ClassDecl(forg, clssName);
		type = retDecl->foamDecl.type;
		fmt = retDecl->foamDecl.format;
		gen0AddStmt(foamNewReturn(retVal), NULL);
	}
	else if (stabIsForeignExport(gen0State->stab, retTf)) {
		ForeignOrigin forgRet = stabForeignExportLocation(gen0State->stab, retTf);
		type = gfjPCallFoamType(retTf, &fmt);

		retVal = foamNewPCall(FOAM_Proto_JavaConstructor, FOAM_JavaObj,
				      gen0CharArray(gfjDeclClassName(symeString(tfIdSyme(retTf)), forgRet)),
				      ccall, NULL);
		gen0AddStmt(foamNewReturn(retVal), NULL);
	}
	else {
		Foam retVal = gfjPCallFoamToJava(retTf, ccall);
		type = gfjPCallFoamType(retTf, &fmt);
		gen0AddStmt(foamNewReturn(retVal), NULL);
	}

	gen0ProgPushFormat(emptyFormatSlot);

	gen0ProgFiniEmpty(prog, type, fmt);
	gen0AddLexLevels(prog, 1);

        foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);
	gen0ProgRestoreState(saved);

	return clos;
}

/*
 * :: Utils
 */

local String
gfjDeclMethodNameForType(TForm tf, String methodName)
{
	assert(tfIsApply(tf) || tfIsId(tf));

	Syme esyme = tfIsApply(tf)
		? abSyme(tfExpr(tf)->abApply.op)
		: tfIdSyme(tf);
	ForeignOrigin forg = symeForeign(esyme);

	return gfjDeclMethodName(symeString(esyme), forg, methodName);
}

local String
gfjDeclMethodName(String typeName, ForeignOrigin forg, String methodName)
{
	String name = forg->file
		? strPrintf("%s.%s.%s", forg->file, typeName, methodName)
		: strPrintf("%s.%s", typeName, methodName);
	return name;
}

local String
gfjDeclClassNameForType(TForm tf)
{
	assert(tfIsApply(tf) || tfIsId(tf));

	Syme esyme = tfIsApply(tf)
		? abSyme(tfExpr(tf)->abApply.op)
		: tfIdSyme(tf);
	ForeignOrigin forg = symeForeign(esyme);

	return gfjDeclClassName(symeString(esyme), forg);
}

local String
gfjDeclClassName(String typeName, ForeignOrigin forg)
{
	String name = forg->file
		? strPrintf("%s.%s", forg->file, typeName)
		: strCopy(typeName);
	return name;
}

local AInt
gfjExportDecoder(TForm tf)
{
	Foam gdecl;
	AInt cached;

	cached = (AInt) tblElt(gfjExportDecoderForTForm, (TblKey) tf, (TblElt) -1);
	if (cached != -1) {
		return cached;
	}
	// Need a global that converts a java object to tf.
	// This will be <object>.rep()
	String globName = gfjDeclMethodName(symeString(tfIdSyme(tf)),
					    stabForeignExportLocation(gen0State->stab, tf),
					    "rep");;

	Foam ddecl = javaSigNew(foamNewDecl(FOAM_Word, strCopy(""), emptyFormatSlot),
				foamNewDecl(FOAM_NOp, strCopy(""), emptyFormatSlot),
				listSingleton(Foam)(gfjPCallDeclArg(tf)));
	AInt sigIdx = gen0AddRealFormat(ddecl);
	gdecl = foamNewGDecl(FOAM_Clos, globName, sigIdx,
			     FOAM_GDecl_Import, FOAM_Proto_JavaMethod);

	AInt idx = gen0AddGlobal(gdecl);

	tblSetElt(gfjExportDecoderForTForm, (TblKey) tf, (TblElt) idx);

	return idx;
}

local AInt
gfjExportEncoder(TForm tf)
{
	Foam gdecl;
	AInt cached;

	cached = (AInt) tblElt(gfjExportEncoderForTForm, (TblKey) tf, (TblElt) -1);
	if (cached != -1) {
		return cached;
	}
	// Need a global that converts a tf to a java object
	// This will be new <X>();
	String globName = gfjDeclClassName(symeString(tfIdSyme(tf)), stabForeignExportLocation(gen0State->stab, tf));

	Foam ddecl = javaSigNew(gfjPCallDeclArg(tf),
			   foamNewDecl(FOAM_NOp, strCopy(""), emptyFormatSlot),
			   listSingleton(Foam)(foamNewDecl(FOAM_Word, strCopy(""), emptyFormatSlot)));
	AInt sigIdx = gen0AddRealFormat(ddecl);

	gdecl = foamNewGDecl(FOAM_Clos, globName, sigIdx,
			     FOAM_GDecl_Import, FOAM_Proto_JavaMethod);

	AInt idx = gen0AddGlobal(gdecl);

	tblSetElt(gfjExportEncoderForTForm, (TblKey) tf, (TblElt) idx);

	return idx;
}
