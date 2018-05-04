#include "axlobs.h"

#include "ablogic.h"
#include "absyn.h"
#include "archive.h"
#include "doc.h"
#include "debug.h"
#include "int.h"
#include "sefo.h"
#include "sexpr.h"
#include "stab.h"
#include "store.h"
#include "syme.h"
#include "spesym.h"
#include "table.h"
#include "ti_bup.h"
#include "ti_tdn.h"
#include "tform.h"

/*****************************************************************************
 *
 * :: Annotated SExpression
 *
 ****************************************************************************/
static Bool abcElideInnerExpressions;

SExpr abToAnnotatedSExpr(AbSyn whole);

typedef struct _AbAnnotationBucket {
	Table indexForSefo;
	Table sxForIndex;
	AInt nextIndex;
	Table indexForSyme;
	Table symeSxForIndex;
	AInt nextSymeIndex;
} *AbAnnotationBucket;

local AbAnnotationBucket abcNew(void);
local void abcFree(AbAnnotationBucket bucket);
local AInt abcGetSefo(AbAnnotationBucket bucket, Sefo sefo);
local AInt abcAddSefo(AbAnnotationBucket bucket, Sefo sefo);
local void abcSetSefoSExpr(AbAnnotationBucket bucket, AInt idx, SExpr sx);

local AInt abcGetSyme(AbAnnotationBucket bucket, Syme syme);
local AInt abcAddSyme(AbAnnotationBucket bucket, Syme syme);
local void abcSetSymeSExpr(AbAnnotationBucket bucket, AInt idx, SExpr sx);

local SExpr abAnnotateSymeRef(Syme syme, AbAnnotationBucket bucket);
local SExpr abAnnotateSyme(Syme syme, AbAnnotationBucket bucket);

local SExpr abAnnotatedSExpr(AbSyn ab, AbAnnotationBucket bucket);
local SExpr abAnnotateId(AbSyn id, AbAnnotationBucket bucket);
local SExpr abAnnotateUnique(AbSyn id, AbAnnotationBucket bucket);
local SExpr abAnnotateTPoss(AbSyn id, AbAnnotationBucket bucket);
local SExpr abAnnotateError(AbSyn id, AbAnnotationBucket bucket);
local SExpr abToAnnotatedTForm(TForm tf, AbAnnotationBucket bucket);
local SExpr abAnnotateSefo(Sefo sefo, AbAnnotationBucket bucket);
local SExpr abAnnotateExportArchive(Archive ar, Syme syme);

local AbAnnotationBucket
abcNew(void)
{
	AbAnnotationBucket bucket = (AbAnnotationBucket) stoAlloc((int) OB_Other, sizeof(*bucket));
	bucket->indexForSefo = tblNew((TblHashFun) abHashSefo, (TblEqFun) sefoEqual);
	bucket->sxForIndex = tblNew((TblHashFun) aintHash, (TblEqFun) aintEqual);
	bucket->indexForSyme = tblNew((TblHashFun) symeHashFn, (TblEqFun) symeEqualWithAnnotation);
	bucket->symeSxForIndex = tblNew((TblHashFun) aintHash, (TblEqFun) aintEqual);
	bucket->nextIndex = 0;
	bucket->nextSymeIndex = 0;
	return bucket;
}

local void
abcFree(AbAnnotationBucket bucket)
{
	tblFree(bucket->indexForSefo);
	tblFree(bucket->sxForIndex);
	tblFree(bucket->indexForSyme);
	tblFree(bucket->symeSxForIndex);
	stoFree(bucket);
}


local AInt
abcGetSefo(AbAnnotationBucket bucket, Sefo sefo)
{
	return (AInt) tblElt(bucket->indexForSefo, sefo, (TblElt) (AInt) -1);
}

local AInt
abcAddSefo(AbAnnotationBucket bucket, Sefo sefo)
{
	AInt idx = bucket->nextIndex++;
	tblSetElt(bucket->indexForSefo, sefo, (TblElt) (AInt) idx);
	return idx;
}

local void
abcSetSefoSExpr(AbAnnotationBucket bucket, AInt idx, SExpr sx)
{
	tblSetElt(bucket->sxForIndex, (TblElt) idx, sx);
}


local AInt
abcGetSyme(AbAnnotationBucket bucket, Syme syme)
{
	return (AInt) tblElt(bucket->indexForSyme, syme, (TblElt) (AInt) -1);
}

local AInt
abcAddSyme(AbAnnotationBucket bucket, Syme syme)
{
	AInt idx = bucket->nextSymeIndex++;
	tblSetElt(bucket->indexForSyme, syme, (TblElt) (AInt) idx);
	return idx;
}

local void
abcSetSymeSExpr(AbAnnotationBucket bucket, AInt idx, SExpr sx)
{
	tblSetElt(bucket->symeSxForIndex, (TblElt) idx, sx);
}

local SExpr
abAnnotatedSExprElided(AbSyn ab, AbAnnotationBucket bucket)
{
	SExpr sx;
	Bool current = abcElideInnerExpressions;
	abcElideInnerExpressions = true;
	sx = abAnnotatedSExpr(ab, bucket);
	abcElideInnerExpressions = current;
	return sx;
}



local SExpr
abcSExpr(AbAnnotationBucket bucket)
{
	SExpr wholeSefo = sxNil;
	SExpr wholeSyme = sxNil;
	int i;
	for (i=0; i<bucket->nextIndex; i++) {
		SExpr sx = tblElt(bucket->sxForIndex, (TblElt) (AInt) i, NULL);
		wholeSefo = sxCons(sx, wholeSefo);
	}
	for (i=0; i<bucket->nextSymeIndex; i++) {
		SExpr sx = tblElt(bucket->symeSxForIndex, (TblElt) (AInt) i, NULL);
		wholeSyme = sxCons(sx, wholeSyme);
	}

	return sxiList(2, sxNReverse(wholeSyme), sxNReverse(wholeSefo));
}

SExpr
abToAnnotatedSExpr(AbSyn whole)
{
	AbAnnotationBucket bucket = abcNew();
	SExpr sexpr;

	sexpr = abAnnotatedSExpr(whole, bucket);
	sexpr = sxCons(sexpr, abcSExpr(bucket));
	abcFree(bucket);
	
	return sexpr;
}

local SExpr
abAnnotatedSExpr(AbSyn ab, AbAnnotationBucket bucket)
{
	Length	ai;
	SExpr	sx;

	if (!ab) return sxNil;

	switch (abTag(ab)) {
	  case AB_Nothing:
		sx = sxNil;
		break;
	  case AB_Blank:
	  case AB_IdSy:
		sx  = sxiList(2,
			abInfo(abTag(ab)).sxsym,
			sxiFrSymbol(ab->abId.sym));
		break;
	  case AB_Id:
		  sx  = abAnnotateId(ab, bucket);
		break;
	  case AB_DocText:
		sx  = sxiList(2,
			abInfo(abTag(ab)).sxsym,
			sxiFrString(docString(ab->abDocText.doc))
		);
		break;
	  case AB_LitInteger:
	  case AB_LitString:
	  case AB_LitFloat:
		sx  = sxiList(2,
			abInfo(abTag(ab)).sxsym,
			sxiFrString(ab->abLitString.str)
		);
		break;
	  case AB_Declare: {
		Syme	syme = abSyme(ab->abDeclare.id);

		sx  = sxCons(abInfo(abTag(ab)).sxsym, sxNil);
		for (ai = 0; ai < abArgc(ab); ai++)
			sx = sxCons(abAnnotatedSExpr(abArgv(ab)[ai], bucket), sx);

		sx = sxNReverse(sx);
		break;
		}
	  case AB_Add:
	  case AB_With: {
		if (abcElideInnerExpressions) {
			sx = sxCons(abInfo(abTag(ab)).sxsym, sxNil);
		}
		else {
			sx  = sxCons(abInfo(abTag(ab)).sxsym, sxNil);
			for (ai = 0; ai < abArgc(ab); ai++)
				sx = sxCons(abAnnotatedSExpr(abArgv(ab)[ai], bucket), sx);
			sx = sxNReverse(sx);
		}
		break;
	  }
	  default:
		sx  = sxCons(abInfo(abTag(ab)).sxsym, sxNil);
		for (ai = 0; ai < abArgc(ab); ai++)
			sx = sxCons(abAnnotatedSExpr(abArgv(ab)[ai], bucket), sx);
		sx = sxNReverse(sx);
	}

	sx = sxiRepos(abPos(ab), sx);
	return sx;
}

local SExpr
abAnnotateId(AbSyn id, AbAnnotationBucket bucket)
{
	SExpr sx;
	switch (abState(id)) {
	case AB_State_AbSyn:
	case AB_State_HasPoss:
		sx = abAnnotateTPoss(id, bucket);
		break;
	case AB_State_HasUnique:
		sx = abAnnotateUnique(id, bucket);
		break;
	case AB_State_Error:
		sx = abAnnotateError(id, bucket);
		break;
	default:
		sx = NULL;
		assert(false);
	}

	return sxCons(abInfo(abTag(id)).sxsym, sx);
}

local SExpr
abAnnotateAbSyn(AbSyn id, AbAnnotationBucket bucket)
{
	SExpr whole = sxNil;

	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("name")), sxiFrSymbol(abIdSym(id))), whole);

	return whole;
}

local SExpr
abAnnotateUnique(AbSyn id, AbAnnotationBucket bucket)
{
	Syme syme = abSyme(id);
	SrcPos spos = abPos(id);
	SExpr sposSx =  (sposIsNone(spos)) ? sxNil : sposToSExpr(spos);

	if (abIsTheId(id, ssymCategory)) {
		syme = car(stabGetMeanings(stabFile(), ablogFalse(), ssymCategory));
	}
	else if (syme == NULL) {
		return abAnnotateAbSyn(id, bucket);
	}
	SExpr symeSx = abAnnotateSyme(syme, bucket);
	SExpr whole = sxNil;
	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("syme")), symeSx), whole);
	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("srcpos")), sposSx), whole);

	return whole;
}

local SExpr
abAnnotateSyme(Syme syme, AbAnnotationBucket bucket)
{
	AInt idx = abcGetSyme(bucket, syme);
	if (idx == -1) {
		AInt newIdx = abcAddSyme(bucket, syme);
		SExpr sx = abAnnotateSymeRef(syme, bucket);
		abcSetSymeSExpr(bucket, newIdx, sx);
		return sxCons(sxiFrSymbol(symInternConst("ref")), sxiFrInteger(newIdx));
	}
	else {
		return sxCons(sxiFrSymbol(symInternConst("ref")), sxiFrInteger(idx));
	}
	
}

local SExpr
abAnnotateSymeRef(Syme syme, AbAnnotationBucket bucket)
{
	TForm tf = symeType(syme);
	SExpr exporterSx = sxNil;
	
	SrcPos spos = symeSrcPos(syme);
	SExpr sposSx =  (sposIsNone(spos)) ? NULL : sposToSExpr(spos);
	Bool wantType = true;
	SExpr whole = sxNil;

	if (symeIsLibrary(syme)) {
		wantType = false;
	}
	if (symeIsExport(syme)) {
		Syme original = symeOriginal(syme);
		AInt constNum = symeConstNum(original);
		AInt defnNum = symeDefnNum(original);
		Lib  lib     = symeLib(syme);

		whole = sxCons(sxCons(sxiFrSymbol(symInternConst("typeCode")),
				      sxiFrInteger(symeTypeCode(original))), whole);
		if (lib != NULL) {
			whole = sxCons(sxCons(sxiFrSymbol(symInternConst("lib")),
					      sxiFrString(fnameUnparse(lib->name))), whole);
		}
	}
	if (symeIsImport(syme)) {
		TForm exporter = symeExporter(syme);

		if (tfIsArchive(exporter)) {
			whole = sxCons(sxCons(sxiFrSymbol(symInternConst("typeCode")),
					      sxiFrInteger(symeTypeCode(syme))), whole);
			exporterSx = abAnnotateExportArchive(tfArchiveAr(exporter), syme);
			wantType = false;
		}
		else {
			Syme original = symeOriginal(syme);
			SExpr originalSx = abAnnotateSyme(original, bucket);
			whole = sxCons(sxCons(sxiFrSymbol(symInternConst("original")),
					      originalSx), whole);
			whole = sxCons(sxCons(sxiFrSymbol(symInternConst("typeCode")),
					      sxiFrInteger(symeTypeCode(original))), whole);
			exporterSx = abToAnnotatedTForm(exporter, bucket);
		}
	}
	
	if (!sxiNull(exporterSx)) {
		whole = sxCons(sxCons(sxiFrSymbol(symInternConst("exporter")), exporterSx), whole);
	}
	if (wantType) {
		SExpr typeSx = sxNil;
		if (tfIsDomainType(tf)) {
			typeSx = sxiFrString("-- Domain --");
		}
		else if (tfIsCategoryType(tf) || tfIsThird(tf)) {
			typeSx = sxiFrString("-- Category --");
		}
		else {
			typeSx = abToAnnotatedTForm(tf, bucket);
		}
		whole = sxCons(sxCons(sxiFrSymbol(symInternConst("type")), typeSx), whole);
	}
	
	if (sposSx != NULL) {
		whole = sxCons(sxCons(sxiFrSymbol(symInternConst("srcpos")), sposSx), whole);
	}
	
	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("name")), sxiFrSymbol(symeId(syme))), whole);

	return whole;
}

local SExpr
abAnnotateExportArchive(Archive ar, Syme syme)
{
	Syme original = symeOriginal(syme);
	Syme symeLib = arLibrarySyme(ar, original);
	Lib  lib = symeLibrary(symeLib);
	SExpr whole = sxNil;

	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("lib")),
			      sxiFrString(fnameUnparse(lib->name))), whole);
	
	return whole;
}



local SExpr
abAnnotateTPoss(AbSyn id, AbAnnotationBucket bucket)
{
	SExpr whole = sxNil;
	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("name")), sxiFrSymbol(abIdSym(id))),
		       whole);
	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("state")), sxiFrString("tposs")),
		       whole);
	return whole;
}

local SExpr
abAnnotateError(AbSyn id, AbAnnotationBucket bucket)
{
	SExpr whole = sxNil;
	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("name")), sxiFrSymbol(abIdSym(id))),
		       whole);
	whole = sxCons(sxCons(sxiFrSymbol(symInternConst("state")), sxiFrString("error")),
		       whole);
	return whole;
}

local SExpr
abToAnnotatedTForm(TForm tf, AbAnnotationBucket bucket)
{
	return abAnnotateSefo(tfExpr(tf), bucket);
}

local SExpr
abAnnotateSefo(Sefo sefo, AbAnnotationBucket bucket)
{
	AInt idx = abcGetSefo(bucket, sefo);
	if (idx == -1) {
		AInt newIdx = abcAddSefo(bucket, sefo);
		SExpr sx = abAnnotatedSExprElided(sefo, bucket);
		abcSetSefoSExpr(bucket, newIdx, sx);
		return sxCons(sxiFrSymbol(symInternConst("ref")), sxiFrInteger(newIdx));
	}
	else {
		return sxCons(sxiFrSymbol(symInternConst("ref")), sxiFrInteger(idx));
	}
}

