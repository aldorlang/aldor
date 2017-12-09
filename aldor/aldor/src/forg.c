#include "axlobs.h"
#include "forg.h"
#include "spesym.h"
#include "store.h"
#include "comsg.h"
#include "strops.h"
#include "util.h"

/*****************************************************************************
 *
 * :: Foreign origins
 *
 ****************************************************************************/

local ForeignOrigin	forgAlloc		(FoamProtoTag, String);

static ForeignOrigin	stdOrig[FOAM_PROTO_LIMIT - FOAM_PROTO_START];

local ForeignOrigin
forgAlloc(FoamProtoTag ptag, String file)
{
	ForeignOrigin forg = (ForeignOrigin) stoAlloc(OB_Other, sizeof(*forg));

	forg->protocol	= ptag;
	forg->file	= file ? strCopy(file) : NULL;

	return forg;
}

ForeignOrigin
forgNew(FoamProtoTag ptag, String file)
{
	static Bool		stdOrigAreInit = false;
	static ForeignOrigin	oldOrig = NULL;

	if (!stdOrigAreInit) {
		Length	i;
		for (i = 0; i < FOAM_PROTO_LIMIT - FOAM_PROTO_START; i += 1)
			stdOrig[i] = forgAlloc((FoamProtoTag)(i) + FOAM_PROTO_START, NULL);
		stdOrigAreInit = true;
	}

	if (file == NULL)
		return stdOrig[ptag - FOAM_PROTO_START];

	if (oldOrig && ptag==oldOrig->protocol && strEqual(file,oldOrig->file))
		return oldOrig;

	return oldOrig = forgAlloc(ptag, file);
}

ForeignOrigin
forgFrAbSyn(AbSyn origin)
{
	Symbol		psym;
	FoamProtoTag	ptag;
	String		file = NULL;

	if (!origin)
		return forgNew(FOAM_Proto_Other, file);

	if (abHasTag(origin, AB_Apply)) {
		AbSyn	*argv = abApplyArgv(origin);
		if (abApplyArgc(origin)==1 && abHasTag(argv[0],AB_LitString))
			file = argv[0]->abLitString.str;

		origin = origin->abApply.op;
	}

	psym = abHasTag(origin, AB_Id) ? origin->abId.sym : NULL;

	if (!psym || psym == ssymForeign)
		ptag = FOAM_Proto_Other;

	else if (psym == ssymFortran)
		ptag = FOAM_Proto_Fortran;

	else if (psym == ssymC)
		ptag = FOAM_Proto_C;

	else if (psym == ssymLisp)
		ptag = FOAM_Proto_Lisp;

	else if (psym == ssymJava)
		ptag = FOAM_Proto_Java;

	else {
		comsgWarning(origin, ALDOR_W_ScoNotProtocol);
		ptag = FOAM_Proto_Other;
	}

	return forgNew(ptag, file);
}

Bool
forgEqual(ForeignOrigin f1, ForeignOrigin f2)
{
	if (f1->protocol != f2->protocol)
		return false;

	if (f1->file == NULL && f2->file == NULL)
		return true;
	if (f1->file != NULL && f2->file != NULL)
		return strEqual(f1->file, f2->file);
	return false;
}

void
forgFree(ForeignOrigin forg)
{
	/* Not possible to free these as there is a
	 * pre-initialised cache, and some last-value caching
	 */
}

AInt
forgHash(ForeignOrigin forg)
{
	if (forg->file == NULL) {
		return forg->protocol;
	}
	return hashCombine(forg->protocol, strHash(forg->file));
}

ForeignOrigin
forgFrBuffer(Buffer buf)
{
	FoamProtoTag tag = (FoamProtoTag) bufGetHInt(buf);
	String file = NULL;
	Bool flg;
	flg = bufGetByte(buf);
	if (flg) {
		file = bufRdString(buf);
	}

	return forgNew(tag, file);
}

void
forgToBuffer(Buffer buf, ForeignOrigin forg)
{
	bufPutHInt(buf, forg->protocol);
	bufPutByte(buf, forg->file != NULL);
	if (forg->file != NULL) {
		bufWrString(buf, forg->file);
	}
}

void
forgBufferSkip(Buffer buf)
{
	String text;
	Bool flg;
	bufGetHInt(buf);
	flg = bufGetByte(buf);
	if (flg) {
		text = bufRdString(buf);
	}
}
