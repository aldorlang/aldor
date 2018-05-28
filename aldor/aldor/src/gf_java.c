#include "axlobs.h"
#include "gf_util.h"
#include "gf_prog.h"
#include "gf_java.h"
#include "gf_syme.h"
#include "javasig.h"
#include "of_inlin.h"
#include "tform.h"
#include "sefo.h"
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
local Foam gfjPCallFoamToJava(TForm tf, Foam foam);
local Foam gfjPCallJavaToFoam(TForm tf, Foam foam);
local AInt gfjPCallDecl(TForm tf, TForm this);
local Foam gfjPCallDeclArg(TForm tf);

local AInt gj0ClassDDecl(ForeignOrigin origin, String clsName);

void
gfjInit()
{
}

void
gfjFini()
{
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
	Syme esyme;
	ForeignOrigin forg;
	FoamList params;
	Foam gdecl, prog, pcall, temp, op;
	FoamTag retType;
	String fnName, globName;
	AInt gnum, constnum;
	AInt clsFmt;
	
	constnum = gen0NumProgs;

	exporter = symeExporter(syme);
	esyme = tfIdSyme(exporter);
	forg = symeForeign(esyme);
	innerTf = tfMapRet(symeType(syme));

	clsFmt = gj0ClassDDecl(forg, symeString(esyme));

	globName = (forg->file ? strPrintf("%s.%s.%s", 
					   forg->file, symeString(esyme), symeJavaApplyName(syme))
		    : strPrintf("%s.%s", symeString(esyme), symeJavaApplyName(syme)));
	gdecl = foamNewGDecl(FOAM_Word, globName,
			     gfjPCallDecl(innerTf, tfMapArgN(symeType(syme), 0)),
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
		Foam retval;
		pcall = foamNewPCallOfList(FOAM_Proto_JavaMethod, 
					   gfjPCallFoamType(tfMapRet(innerTf), NULL),
					   op, params);
		retval = gfjPCallJavaToFoam(tfMapRet(innerTf), pcall);
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
	ForeignOrigin forg;
	TForm exporter;
	Syme esyme;
	FoamList params;
	Foam prog, gdecl, pcall;
	String fnName, globName;
	AInt fmtNum, innerConstNum, constNum, gnum;
	
	exporter = symeExporter(syme);
	esyme = tfIdSyme(exporter);
	forg = symeForeign(esyme);

	fnName  = strCopy(symString(tfIdSym(exporter)));
	globName = strPrintf("%s.%s", forg->file,
			     symString(tfIdSym(exporter)));
	constNum = gen0NumProgs;
	
	gdecl = foamNewGDecl(FOAM_Word, globName, gfjPCallDecl(symeType(syme), NULL),
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
	esyme = tfIdSyme(exporter);
	forg = symeForeign(esyme);

	fnName  = strCopy(symeString(syme));
	globName = forg->file == NULL
		? strPrintf("%s.%s",
			     symString(tfIdSym(exporter)),
			    symeString(syme))
		: strPrintf("%s.%s.%s", forg->file,
			     symString(tfIdSym(exporter)),
			     symeString(syme));
	
	constNum = gen0NumProgs;
	
	gdecl = foamNewGDecl(FOAM_Word, globName, gfjPCallDecl(symeType(syme), NULL),
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
	if (tfIsJavaImport(tf)) {
		if (pfmt != NULL) {
			Syme syme = tfIdSyme(tf);
			TForm exporter = symeExporter(syme);
			Syme esyme = tfIdSyme(exporter);
			ForeignOrigin forg = symeForeign(esyme);
			*pfmt = gj0ClassDDecl(forg, symeString(esyme));
		}
		return FOAM_JavaObj;
	}
	else {
		Syme javaToSelf = tfGetDomImport(tf, symString(ssymTheFromJava),
						 tfIsJavaDecoder);
		TForm convTf = symeType(javaToSelf);
		tfFollow(convTf);
		return gen0Type(tfMapArg(convTf), pfmt);
	}
}


local Foam
gfjPCallFoamToJava(TForm tf, Foam foam)
{
	if (tfIsJavaImport(tf)) {
		return foamNewCast(FOAM_JavaObj, foam);
	}
	else {
		Syme selfToJava = tfGetDomImport(tf, 
						 symString(ssymTheToJava),
						 tfIsJavaEncoder);
		FoamTag type = gen0Type(tfMapRetN(symeType(selfToJava), 0), NULL);
		Foam call;
		call = gen0ExtendSyme(selfToJava);
		call = foamNewCCall(type, call, foam, NULL);
		
		return call;
	}
}

local Foam
gfjPCallJavaToFoam(TForm tf, Foam foam)
{
	if (tfIsJavaImport(tf)) {
		return foamNewCast(FOAM_Word, foam);
	}
	else {
		Syme javaToSelf = tfGetDomImport(tf, 
						 symString(ssymTheFromJava),
						 tfIsJavaDecoder);
		Foam call;
		call = gen0ExtendSyme(javaToSelf);
		call = foamNewCCall(FOAM_Word, call, foam, NULL);
		
		return call;
	}
}

local AInt
gfjPCallDecl(TForm tf, TForm this)
{
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

	retdecl = gfjPCallDeclArg(tfMapRet(tf));
	exndecl = foamNewDecl(FOAM_NOp, strCopy(""), emptyFormatSlot);

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
	Foam decl;

	if (tfIsDeclare(tf)) tf = tfDeclareType(tf);

	if (tfIsJavaImport(tf)) {
		Syme syme = tfIdSyme(tf);
		ForeignOrigin forg = symeForeign(syme);
		String name = forg->file == NULL ? strCopy(symeString(syme))
			: strPrintf("%s.%s", forg->file, symeString(syme));
		AInt fmt = gj0ClassDDecl(forg, symeString(syme));
		decl = foamNewDecl(FOAM_JavaObj, name, fmt);
	}
	else {
		FoamTag type;
		AInt fmt;
		type = gen0Type(tf, &fmt);
		decl = foamNewDecl(type, strCopy(""), fmt);
	}

	return decl;
}

AInt
gj0ClassDDecl(ForeignOrigin origin, String clsName)
{
	String name = origin->file == NULL ? strCopy(clsName): strPrintf("%s.%s", origin->file, clsName);
	Foam decl = foamNewDecl(FOAM_Word, name, int0);

	return gen0AddRealFormat(foamNewDDecl(FOAM_DDecl_JavaClass, decl, NULL));
}
