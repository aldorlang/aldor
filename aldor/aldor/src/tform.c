/*****************************************************************************
 *
 * tform.c: Type forms.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "fint.h"
#include "format.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "tfcond.h"
#include "ti_sef.h"
#include "ti_top.h"
#include "tinfer.h"
#include "util.h"
#include "sefo.h"
#include "archive.h"
#include "lib.h"
#include "tqual.h"
#include "tconst.h"
#include "tposs.h"
#include "tfsat.h"
#include "freevar.h"
#include "absub.h"
#include "ablogic.h"
#include "abpretty.h"
#include "comsg.h"
#include "strops.h"

Bool	tfDebug			= false;
Bool	tfExprDebug		= false;
Bool	tfCrossDebug		= false;
Bool	tfFloatDebug		= false;
Bool	tfHasDebug		= false;
Bool	tfHashDebug		= false;
Bool	tfImportDebug		= false;
Bool	tfMapDebug		= false;
Bool	tfMultiDebug		= false;
Bool	tfCatDebug		= false;
Bool	tfWithDebug		= false;
Bool	tfCascadeDebug		= false;
Bool	tfParentDebug		= false;
Bool	symeRefreshDebug	= false;

#define tfDEBUG			DEBUG_IF(tf)		afprintf
#define tfExprDEBUG		DEBUG_IF(tfExpr)	afprintf
#define tfCrossDEBUG		DEBUG_IF(tfCross)	afprintf
#define tfFloatDEBUG		DEBUG_IF(tfFloat)	afprintf
#define tfHasDEBUG		DEBUG_IF(tfHas)		afprintf
#define tfHashDEBUG		DEBUG_IF(tfHash)	afprintf
#define tfImportDEBUG		DEBUG_IF(tfImport)	afprintf
#define tfMapDEBUG		DEBUG_IF(tfMap)		afprintf
#define tfMultiDEBUG		DEBUG_IF(tfMulti)	afprintf
#define tfCatDEBUG		DEBUG_IF(tfCat)		afprintf
#define tfWithDEBUG		DEBUG_IF(tfWith)	afprintf
#define tfCascadeDEBUG		DEBUG_IF(tfCascade)	afprintf
#define tfParentDEBUG		DEBUG_IF(tfParent)	afprintf
#define symeRefreshDEBUG	DEBUG_IF(symeRefresh)	afprintf

#define	TFormBuiltinSefo
#undef	UseTypeVariables


/*
 * tfMaxBaseSearchDepth defines the number of times that a tform
 * can be expanded by tfDefineeType1() before we give up: it should
 * never be reached. Make sure that it is large enough for all normal
 * tforms to be completely expanded yet small enough to prevent bad
 * tforms from killing performance. Use -Wcheck to see if we ever
 * reach the limit.
 * 
 * !!! To do: this ought to be a command-line switch.
 */
int tfMaxBaseSearchDepth = 20;


SymeList
tfSetSymesFn(TForm tf, SymeList sl)
{
	tf->symes = sl;
	return sl;
}

/******************************************************************************
 *
 * :: Local operations
 *
 *****************************************************************************/

/*
 * Type form constructors.
 */
local TForm		tfNewSymbol		(TFormTag);
local TForm		tfNewSyntax		(AbSyn);
local TForm		tfNewNode		(TFormTag, Length, ...);
local TForm		tfNewBuiltin		(TForm, Symbol);

/*
 * Type form syntax.
 */
local void		tfSetExpr		(TForm, AbSyn);
local AbSyn		tfToAbSyn0		(TForm, Bool);

local Bool		tfOwnsExpr		(TForm);
local AbSyn		tfDisownExpr		(TForm, Bool);
local void		tfPendingFrSyntaxMap	(Stab, AbSyn, TForm);
local void		tfPendingFrSyntaxDefine	(Stab, AbSyn, TForm);
local void		tfp0FoamType		(TForm);

local Bool		tfMeaningEqual		(Sefo, Sefo);

/*
 * Type form floating.
 */
/*local ULong		tfOuterDepth		(Stab, TForm);*/
/*local ULong		abOuterDepth		(Stab, AbSyn);*/

/*
 * Type form representational symes.
 */
local Syme		tfNewRepSyme		(Stab, Symbol, TForm, Hash);
local SymeList		tfSymesFrDeclare	(Sefo);
local SymeList		tfSymesFrCross		(TForm);
local SymeList		tfSymesFrMulti		(TForm);
local SymeList		tfSymesFrMap		(TForm);
local SymeList		tfSymesFrEnum		(Stab, TForm, Sefo);
local void		tfCheckDenseArgs	(TForm, Sefo);
local SymeList		tfSymesFrRawRecord	(Stab, TForm, Sefo);
local SymeList		tfSymesFrRecord		(Stab, TForm, Sefo);
local SymeList		tfSymesFrUnion		(Stab, TForm, Sefo);
local SymeList		tfSymesFrTrailingArray	(Stab, TForm, Sefo);
local void		tfSymesTestCompoundType (Stab, TForm, Bool *);
local SymeList		tfSymesFrCompoundType	(Stab, TForm, Bool *,SymeList);
local SymeList		tfSymesFrAdd		(Sefo);
local SymeList		tfSymesFrDefault	(Sefo);

local AbSyn		tfSymesToAdd		(SymeList);

local void		tfGetExportError	(TForm, String);

local TForm		tfCatExportsPendingFrWith(Sefo);
local Bool		tfHasSymesFrDefault	(Sefo);

local SymeList		tfGetCatSelfFrWith	(Sefo);

local Syme		symeListFindExport	(SymeList, Syme, SymeList);
local void		tfJoinExports		(Syme, Syme);
local Bool		tfJoinExportToList	(SymeList, SymeList, Syme,
						 Sefo);
local SymeList		tfMeetExportLists	(SymeList, SymeList, SymeList,
						 Sefo);

local SymeList		tfAddDomExports		(TForm, SymeList);
local SymeList		tfAddCatExports		(TForm, SymeList);
local SymeList		tfAddThdExports		(TForm, SymeList);
local SymeList		tfAddHasExports		(TForm, TForm);

local SymeList		tfGetCatExportsFrParents(SymeList);
local SymeList		tfGetCatExportsCond	(SymeList, SefoList, Bool);
local SymeList		tfGetCatExportsFilter	(SymeList, SymeList);

local SymeList		tfGetCatExportsFrWith	(TForm);
local SymeList		tfGetCatExportsFrIf	(TForm);
local SymeList		tfGetCatExportsFrJoin	(TForm);
local SymeList		tfGetCatExportsFrMeet	(TForm);

local Syme		tfGetBuiltinSyme	(TForm, Symbol);

local SymeList		tfGetThdConstants	(TForm);
local SymeList		tfGetCatConstantsFrWith	(Sefo);

local TQualList		tfGetCascadesFrStab	(Stab);
local TQualList		tfGetCatCascadesFrWith	(TForm);
local TQualList		tfGetCascadesFrTrailingArray(TForm tf);

local TForm		tfIsIdempotent		(TForm);
local void		tfForwardIdempotent	(TForm, TForm);
local void		tfExtendFinishTwins	(Stab, Syme);

/******************************************************************************
 *
 * :: Compound ops
 *
 *****************************************************************************/

enum tfCompoundOps {
    TFC_START,
	TFC_HasEq = TFC_START,
	TFC_HasNeq,
	TFC_HasPrint,
    TFC_LIMIT
};


/******************************************************************************
 *
 * :: Debugging facilities
 *
 *****************************************************************************/

/*
 * Debugging function designed to fully expand a tform so that
 * there aren't any TF_Subst or TF_Follow nodes and that all
 * symes have been triggered.
 */
TForm
tfFollowFully(TForm tf)
{
	int i;

	tf = tfFollowFn(tf);

	if (tf) {
		for (i = 0;i < tfArgc(tf); i++)
			tfArgv(tf)[i] = tfFollowFully(tfArgv(tf)[i]);
	}

	return tf;
}

void
tformDump(TForm tf)
{
	/* Display the origin of tforms */
	if (tfHasExpr(tf))
	{
		AbSyn	expr = tfGetExpr(tf);

		(void)tfPrintDb(tf);
		spstackPrintDb(expr->abHdr.pos);
		fnewline(dbOut);
	}
}

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

/*
 * Type form constructors.
 */


local int tfFormatter(OStream stream, Pointer p);
local int tfListFormatter(OStream stream, Pointer p);

local int tpossFormatter(OStream stream, Pointer p);
local int fvFormatter(OStream stream, Pointer p);

local int symeFormatter(OStream stream, Pointer p);
local int symeListFormatter(OStream stream, Pointer p);
local int symeListListFormatter(OStream stream, Pointer p);

local int symeConditionFormatter(OStream stream, Pointer p);
local int symeConditionListFormatter(OStream stream, Pointer p);

local int ptrFormatter(OStream stream, Pointer p);
local int ptrListFormatter(OStream stream, Pointer p);

local int aintFormatter(OStream stream, Pointer p);
local int aintListFormatter(OStream stream, Pointer p);

/* For breakpoints */
local void tfBreak(TForm tf);
static TForm tfBreakVal;

TForm
tfNewEmpty(TFormTag tag, Length argc)
{
	TForm	tf;
	Length	i;

	tf = (TForm) stoAlloc((unsigned)OB_TForm, sizeof(*tf)+argc*sizeof(tf));

	tf->tag		= tag;
	tf->ownSyntax	= false;
	tf->hasSelf	= false;
	tf->hasSelfSelf	= false;
	tf->hasCascades	= false;
	tf->raw		= FOAM_LIMIT;
	tf->hash	= 0;
	tf->__absyn	= 0;
	tf->intStepNo	= intStepNo;

	tf->argc	= argc;
	tf->argv	= (argc ? (TForm *) (tf + 1) : NULL);

	tf->stab	= NULL;
	tf->self	= listNil(Syme);
	tf->selfself	= listNil(Syme);
	tf->parents	= listNil(Syme);
	tf->symes	= listNil(Syme);

	tf->domExports	= listNil(Syme);
	tf->catExports	= listNil(Syme);
	tf->thdExports	= listNil(Syme);

	tf->domImports	= listNil(Syme);

	tf->consts	= listNil(TConst);
	tf->queries	= listNil(TForm);
	tf->cascades	= listNil(TQual);

	tf->conditions = NULL;;

	tf->sigma	= NULL;
	tf->fv		= NULL;
	tf->rho		= NULL;

	tf->__mark	= 0;
	tf->parent	= NULL;
	tf->libNum	= TYPE_NUMBER_UNASSIGNED;

	for (i = 0; i < argc; i += 1) tf->argv[i] = 0;

	tfSetTForm(tf);
	tfp0FoamType(tf);

	tfBreak(tf);

	return tf;
}

local void
tfBreak(TForm tf)
{
	if (tf == tfBreakVal) {
		printf("breakpoint");
	}
}

local TForm
tfNewSymbol(TFormTag tag)
{
	TForm	tf = tfNewEmpty(tag, int0);
	tfToAbSyn(tf);
	tfSetMeaning(tf);
	return tf;
}

TForm
tfNewAbSyn(TFormTag tag, AbSyn ab)
{
	TForm	tf = tfNewEmpty(tag, int0);
	tfSetExpr(tf, ab);
	return tf;
}

local TForm
tfNewSyntax(AbSyn ab)
{
	/* we leave one slot for TF_Forward transmogrification */
	TForm	tf = tfNewEmpty(TF_Syntax, 1);
	tfSetPending(tf);
	tfSetExpr(tf, ab);
	tf->argc = 0;
	return tf;
}

local TForm
tfNewNode(TFormTag tag, Length argc, ...)
{
	TForm	tf;
	Length	i;
	va_list argp;

	tf = tfNewEmpty(tag, argc);

	va_start(argp, argc);
	for (i = 0; i < argc; i += 1)
		tfArgv(tf)[i] = va_arg(argp, TForm);
	va_end(argp);

	return tf;
}

local TForm
tfNewBuiltin(TForm tf, Symbol sym)
{
	Syme	syme = tfGetBuiltinSyme(tf, sym);
	return syme ? tfFrSyme(stabGlobal(), syme) : tfUnknown;
}

/*
 * Basic TForm operations.
 */

void
tfInitBasicTypes(TForm tf)
{
	static Bool	isInit = false;

	if (isInit) return;

	tfBoolean	= tfNewBuiltin(tf, ssymBoolean);
	tfTextWriter	= tfNewBuiltin(tf, ssymTextWriter);
	tfSingleInteger	= tfNewBuiltin(tf, ssymSingleInteger);

	isInit = true;
}

void
tfInit(void)
{
	static Bool	isInit = false;
	int		i;

	if (isInit) return;

	abUnknown = abNewBlank(sposNone, symIntern("?"));

	tfUnknown	= tfNewSymbol(TF_Unknown);
	tfExit		= tfNewSymbol(TF_Exit);
	tfTest		= tfNewSymbol(TF_Test);
	tfLiteral	= tfNewSymbol(TF_Literal);
	tfType		= tfNewSymbol(TF_Type);
	tfCategory	= tfNewSymbol(TF_Category);

	tfDomain	= tfWith(tfNone(), tfNone());
	tfTypeTuple	= tfTuple(tfType);

	tfBoolean	= tfUnknown;
	tfTextWriter	= tfUnknown;
	tfSingleInteger = tfUnknown;

	for (i = TF_START; i < TF_LIMIT; i++)
		tformInfo(i).hash = strHash(tformInfo(i).str);

	fmtRegister("TForm", tfFormatter);
	fmtRegister("TFormList", tfListFormatter);

	fmtRegister("FreeVar", fvFormatter);
	fmtRegister("TPoss", tpossFormatter);

	fmtRegister("Syme", symeFormatter);
	fmtRegister("SymeList", symeListFormatter);
	fmtRegister("SymeListList", symeListListFormatter);

	fmtRegister("SymeC", symeConditionFormatter);
	fmtRegister("SymeCList", symeConditionListFormatter);

	fmtRegister("Ptr", ptrFormatter);
	fmtRegister("PtrList", ptrListFormatter);

	fmtRegister("AInt", aintFormatter);
	fmtRegister("AIntList", aintListFormatter);

	/* syme.c checks */

	for (i=SYME_FIELD_START; i<SYME_FIELD_LIMIT; i++)
		assert(symeFieldInfo[i].tag == i);

	isInit = true;
}

/*
 * :: Formatted output
 */
/* we do a couple of different types here 
 * (they should be moved to somewhere better really)
 */

local int
symeFormatter(OStream ostream, Pointer p)
{
	TForm tf = (TForm) p;
	int c;

	c = symeOStreamWrite(ostream, p);

	return c;
}

local int
symeConditionFormatter(OStream ostream, Pointer p)
{
	Syme syme = (Syme) p;
	int c;

	c = symeOStreamWrite(ostream, syme);
	c += listFormat(AbSyn)(ostream, "AbSyn", (AbSynList) symeCondition(syme));

	return c;
}

local int
ptrFormatter(OStream ostream, Pointer p)
{
	char buf[20];
	int c;
	
	sprintf(buf, "%p", p);
	c = ostreamWrite(ostream, buf, -1);

	return c;
}

local int
aintFormatter(OStream ostream, Pointer p)
{
	char buf[20];
	int c;

	sprintf(buf, AINT_FMT, (AInt) p);
	c = ostreamWrite(ostream, buf, -1);

	return c;
}

local int
tfFormatter(OStream ostream, Pointer p)
{
	TForm tf = (TForm) p;
	int c;

	c = tformOStreamWrite(ostream, p);

	return c;
}

local int
fvFormatter(OStream ostream, Pointer p)
{
	FreeVar fv = (FreeVar) p;
	int c;

	c = ostreamPrintf(ostream, "[FV: %pSymeList]", fvSymes(fv));

	return c;
}

local int
tpossFormatter(OStream ostream, Pointer p)
{
	TPoss tp = (TPoss) p;
	int c;

	c = tpossOStreamWrite(ostream, tp);

	return c;
}

local int
tfListFormatter(OStream ostream, Pointer p)
{
	TFormList list = (TFormList) p;
	return listFormat(TForm)(ostream, "TForm", list);
}

local int
symeListFormatter(OStream ostream, Pointer p)
{
	SymeList list = (SymeList) p;
	return listFormat(Syme)(ostream, "Syme", list);
}

local int
symeListListFormatter(OStream ostream, Pointer p)
{
	SymeListList list = (SymeListList) p;
	return listFormat(SymeList)(ostream, "SymeList", list);
}

local int
symeConditionListFormatter(OStream ostream, Pointer p)
{
	SymeList list = (SymeList) p;
	return listFormat(Syme)(ostream, "SymeC", list);
}

local int
ptrListFormatter(OStream ostream, Pointer p)
{
	AbSynList list = (AbSynList) p;
	return listFormat(AbSyn)(ostream, "Ptr", list);
}

local int
aintListFormatter(OStream ostream, Pointer p)
{
	AbSynList list = (AbSynList) p;
	return listFormat(AbSyn)(ostream, "AInt", list);
}


void
tfFree(TForm tf)
{
	if (tfOwnsExpr(tf)) abFree(tfGetExpr(tf));

	listFree(Syme)(tf->symes);
	listFree(Syme)(tf->self);

	/* A type form does not own its domExports. */
	listFree(Syme)(tf->domImports);

	stoFree((Pointer) tf);
}

int
tfPrint(FILE *fout, TForm tf)
{
	return tformPrint(fout, tf);
}

int
tfPrintDb(TForm tf)
{
	int rc = tformPrint(dbOut, tf);
	fnewline(dbOut);
	return rc;
}

Bool
tfEqual(TForm t1, TForm t2)
{
	return tformEqual(t1, t2);
}

#define		tfHashArg(h, hi) {	\
	h ^= (h << 8);			\
	h += (hi) + 200041;		\
	h &= 0x3FFFFFFF;		\
}

Hash
tfHash(TForm tf)
{
	Hash		h = 0;
	Length		i;
	SymeList	symes;
	static int serial = 0;
	int this = serial++;

	tfFollow(tf);
	tfHashDEBUG(dbOut, "(hash %d %pTForm\n", this, tf);
	if (!tfIsDefine(tf))
		tf = tfDefineeType(tf);

	if (tfIsSym(tf))
		h = 0;
	else if (tfIsAbSyn(tf)) {
		h = abHashModDeclares(tfGetExpr(tf));
	}
	else if (tfIsDefine(tf)) {
		tfHashArg(h, tfHash(tfDefineeType(tf)));
		tfHashArg(h, tfHash(tfDefineVal(tf)));
	}
	else if (tfIsNode(tf)) {
		for (h = 0, i = 0; i < tfArgc(tf); i += 1) {
			tfHashArg(h, tfHash(tfArgv(tf)[i]));
		}
	}
	else
		bugBadCase(tfTag(tf));

	if (tfTagHasSymes(tfTag(tf))) {
		for (symes = tfSymes(tf); symes; symes = cdr(symes))
			tfHashArg(h, strHash(symeString(car(symes))));
	}

	h += tformInfo(tfTag(tf)).hash + 200063;
	h &= 0x3FFFFFFF;
	tfHashDEBUG(dbOut, " hash %d = %d)\n", this, h);
	return h;
}

TForm
tfFrSyme(Stab stab, Syme syme)
{
	return tfFullFrAbSyn(stab, abFrSyme(syme));
}

TForm
tfFrSelf(Stab stab, TForm tf)
{
	return tfFrSyme(stab, car(tfDefSelf(stab, tf)));
}

/*****************************************************************************
 *
 * :: Declarations for tfPending
 *
 ****************************************************************************/

local Syme	tfp0SpecialSyme (Stab, Syme syme, Symbol, Bool);
local Syme	tfp0IdSyme	(Stab, Syme, Symbol);
local Syme	tfp0OpSyme	(Stab, Syme, Symbol, Length);
local TForm	tfp0IdTForm	(Symbol);
local TForm	tfp0OpTForm	(Symbol, Length);

local TForm	tfp0General	(Stab, AbSyn);
local TForm	tfp0Float	(Stab, AbSyn);

local TForm	tfpNothing	(Stab, AbSyn);
local TForm	tfpBlank	(Stab, AbSyn);
local TForm	tfpId		(Stab, AbSyn);
local TForm	tfpDeclare	(Stab, AbSyn);
local TForm	tfpDefine	(Stab, AbSyn);
local TForm	tfpAssign	(Stab, AbSyn);
local TForm	tfpComma	(Stab, AbSyn);
local TForm	tfpAdd		(Stab, AbSyn);
local TForm	tfpWith		(Stab, AbSyn);
local TForm	tfpIf		(Stab, AbSyn);
local TForm	tfpExcept	(Stab, AbSyn);
local TForm	tfpApply	(Stab, AbSyn);

/*****************************************************************************
 *
 * :: tfPending
 *
 ****************************************************************************/
local AbSynList tfpConditions;

TForm
tfPending(Stab stab, AbSyn ab)
{
	TForm		tf;
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;

	serialNo += 1;
	depthNo	 += 1;
	serialThis = serialNo;
	tfExprDEBUG(dbOut, "%*s(tfPending %d= %pAbSyn\n",
		    depthNo, "", serialThis, ab);


	/* The stab is used to look up meanings for ids without them. */

	if (abStab(ab))
		stab = abStab(ab);

	switch (abTag(ab)) {
	case AB_Nothing:	tf = tfpNothing (stab, ab); break;
	case AB_Blank:		tf = tfpBlank	(stab, ab); break;
	case AB_Id:		tf = tfpId	(stab, ab); break;
	case AB_Declare:	tf = tfpDeclare (stab, ab); break;
	case AB_Define:		tf = tfpDefine	(stab, ab); break;
	case AB_Assign:		tf = tfpAssign	(stab, ab); break;
	case AB_Comma:		tf = tfpComma	(stab, ab); break;
	case AB_Add:		tf = tfpAdd	(stab, ab); break;
	case AB_With:		tf = tfpWith	(stab, ab); break;
	case AB_If:		tf = tfpIf	(stab, ab); break;
	case AB_Apply:		tf = tfpApply	(stab, ab); break;
	case AB_Except:		tf = tfpExcept  (stab, ab); break;
	default:		tf = tfp0General(stab, ab); break;
	}

	tfExprDEBUG(dbOut, "%*s %d %pTForm)\n", depthNo, "", serialThis, tf);
	depthNo -= 1;

	tfSetExpr(tf, ab);
	tfSetPending(tf);
	return tf;
}

Syme
tfpOpSyme(Stab stab, Symbol sym, Length argc)
{
	return tfp0OpSyme(stab, NULL, sym, argc);
}

/*****************************************************************************
 *
 * :: tfPending helper functions
 *
 ****************************************************************************/

local TForm tf0MapRetFrPending(Stab stab, TForm tf);

local Syme
tfp0SpecialSyme(Stab stab, Syme syme, Symbol sym, Bool op)
{
	TForm tf;
	SymeList	sl;

	if (syme != NULL) {
		tf = symeType(syme);
		if (op) {
			tf = tf0MapRetFrPending(stab, tf);
		}
		if (tf && tfSatType(tf))
			return syme;
	}

	for (sl = stabGetMeanings(stab, NULL, sym); sl; sl = cdr(sl)) {
		Syme	syme = car(sl);

		/*!! Need to check some condition to ensure this syme
		 *!! is really special.	 This one doesn't do it.
		if (!symeTop(syme))
			continue;
		 */

		tf = symeType(syme);
		if (op) {
			tf = tf0MapRetFrPending(stab, tf);
		}

		if (tf && tfSatType(tf))
			return syme;
	}

	return NULL;
}

local TForm
tf0MapRetFrPending(Stab stab, TForm tf)
{
	tf = tfFollowOnly(tf);

	if (tfIsAnyMap(tf))
		tf = tfMapRet(tf);
	else if (tfIsMapSyntax(tf)) {
		AbSyn	ab = tfGetExpr(tf);
		tf = tfSyntaxFrAbSyn(stab, abApplyArg(ab, 1));
		tfFollow(tf);
	}
	else
		return NULL;

	return tf;
}

local Syme
tfp0IdSyme(Stab stab, Syme syme, Symbol sym)
{
	Syme	rsyme = 0;

	if ((sym == ssymExit)		||
	    (sym == ssymType)		||
	    (sym == ssymCategory)	||
	    (sym == ssymTest)		||
	    (sym == ssymLiteral))
		rsyme = tfp0SpecialSyme(stab, syme, sym, false);

	return rsyme;
}

local Syme
tfp0OpSyme(Stab stab, Syme syme, Symbol sym, Length argc)
{
	Syme	rsyme = 0;

	if ((argc == 2 && sym == ssymArrow)		||
	    (argc == 2 && sym == ssymPackedArrow)	||
	    (argc == 1 && sym == ssymTuple)		||
	    (argc == 1 && sym == ssymGenerator)		||
	    (argc == 1 && sym == ssymReference)		||
	    (argc == 1 && sym == ssymRaw)		||

	    (sym == ssymCross)		||
	    (sym == ssymEnum)		||
	    (sym == ssymRawRecord)	||
	    (sym == ssymRecord)		||
	    (sym == ssymTrailingArray)	||
	    (sym == ssymUnion)		||
	    (sym == ssymMeet)		||
	    (sym == ssymJoin))
		rsyme = tfp0SpecialSyme(stab, syme, sym, true);

	return rsyme;
}

local TForm
tfp0IdTForm(Symbol sym)
{
	TForm	tf = NULL;

	     if (sym == ssymExit)	tf = tfExit;
	else if (sym == ssymType)	tf = tfType;
	else if (sym == ssymCategory)	tf = tfThird(listNil(Syme));
	else if (sym == ssymTest)	tf = tfTest;
	else if (sym == ssymLiteral)	tf = tfLiteral;

	return tf;
}

local TForm
tfp0OpTForm(Symbol sym, Length argc)
{
	TFormTag	tag = TF_General;
	TForm		tf;

	     if (sym == ssymArrow)		tag = TF_Map;
	else if (sym == ssymPackedArrow)	tag = TF_PackedMap;
	else if (sym == ssymTuple)		tag = TF_Tuple;
	else if (sym == ssymGenerator)		tag = TF_Generator;
	else if (sym == ssymReference)		tag = TF_Reference;
	else if (sym == ssymRaw)		tag = TF_Raw;

	else if (sym == ssymCross)		tag = TF_Cross;
	else if (sym == ssymEnum)		tag = TF_Enumerate;
	else if (sym == ssymRecord)		tag = TF_Record;
	else if (sym == ssymRawRecord)		tag = TF_RawRecord;
	else if (sym == ssymTrailingArray)	tag = TF_TrailingArray;
	else if (sym == ssymUnion)		tag = TF_Union;
	else if (sym == ssymJoin)		tag = TF_Join;
	else if (sym == ssymMeet)		tag = TF_Meet;

	tf = tfNewEmpty(tag, argc);
	return tf;
}

local TForm
tfp0General(Stab stab, AbSyn ab)
{
	return tfNewAbSyn(TF_General, ab);
}

local TForm
tfp0Float(Stab stab, AbSyn ab)
{
	TForm	tf, ntf;
	Stab	nstab;

	tf    = tfSyntaxFrAbSyn(stab, ab);
	if (tfIsSyntax(tf))
		tfMergeConditions(tf, stab, 
				  tfpConditions == listNil(AbSyn) 
				  ? NULL : tfCondEltNew(stab, tfpConditions));

	nstab = tfFloat(stab, tf);

	if (nstab == NULL)
		return tfPendingFrSyntax(stab, ab, tf);
	
	ntf = tiTopFns()->typeInferTForm(nstab, tf);
	tfTransferSemantics(ntf, tf);


	return tf;
}

local void
tfp0FoamType(TForm tf)
{
	FoamTag		tag = FOAM_LIMIT;

	switch (tfTag(tf)) {
	case TF_With:
		tag = FOAM_Word;
		break;
	case TF_Map:
		tag = FOAM_Clos;
		break;
	case TF_PackedMap:
		tag = FOAM_Clos;
		break;
	case TF_Generator:
		tag = FOAM_Clos;
		break;
	case TF_Multiple:
		tag = FOAM_NOp;
		break;
	case TF_RawRecord:
		tag = FOAM_RRec;
		break;
	case TF_Record:
		tag = FOAM_Rec;
		break;
	case TF_Reference:
		tag = FOAM_Clos;
		break;
	case TF_Cross:
		tag = FOAM_Word;
		break;
	case TF_Tuple:
		tag = FOAM_Word;
		break;
	case TF_TrailingArray:
		tag = FOAM_TR;
		break;
	default:
		break;
	}

	tfFoamType(tf) = tag;
}

/*****************************************************************************
 *
 * :: tfPending cases
 *
 ****************************************************************************/
local Bool tfSymesEqModExtends(Syme syme1, Syme syme2);

local TForm
tfpNothing(Stab stab, AbSyn ab)
{
	return tfNone();
}

local TForm
tfpBlank(Stab stab, AbSyn ab)
{
	TForm	tf;

#ifdef UseTypeVariables
	tf = tfNewNode(TF_Variable, 1, tfUnknown);
#else
	tf = tfUnknown;
#endif

	return tf;
}

local TForm
tfpId(Stab stab, AbSyn ab)
{
	Symbol	sym = ab->abId.sym;
	Syme	syme, absyme;
	TForm	tf;

	tf = abTForm(ab);
	if (abIsTheId(ab, ssymCategory) && tf && tfIsDefine(tf)) {
		TForm	tfc = tfDeclareType(tfDefineDecl(tf));
		TForm	tfw = tfDefineVal(tf);
		if (tfIsSyntax(tfw))
			tfw = tfPendingFrSyntax(stab, tfGetExpr(tfw), tfw);
		if (tfIsSyntax(tfc))
			tfForwardFrSyntax(tfc, tfThirdFrTForm(tfw));
		tfFollow(tfc);
		tfSetPending(tfc);
		return tfc;
	}

	absyme = abSyme(ab);
	syme = tfp0IdSyme(stab, absyme, sym);

	if (! syme)
		/* This sym is not one of the known special type syms. */
		tf = tfp0General(stab, ab);

	else if (absyme && symeOriginal(absyme) == symeOriginal(syme))
		/* This syme is one of the known special type symes. */
		tf = tfp0IdTForm(sym);

	else if (!absyme && listMemq(Syme)(stabGetMeanings(stab, NULL, sym), syme))
		/* This ab could be one of the known special type forms. */
		/* Don't try to do any type inference here. */
		tf = tfp0IdTForm(sym);

	else
		/* This ab is not one of the known special type forms. */
		tf = tfp0General(stab, ab);

	if (sym == ssymBoolean && tfBoolean == tfUnknown)
		tfBoolean = tf;
	if (sym == ssymTextWriter && tfTextWriter == tfUnknown)
		tfTextWriter = tf;

	return tf;
}

local TForm
tfpDeclare(Stab stab, AbSyn ab)
{
	TForm	tf;

	tf = tfNewNode(TF_Declare, 1, tfp0Float(stab, ab->abDeclare.type));

	return tf;
}

local TForm
tfpDefine(Stab stab, AbSyn ab)
{
	AbSyn	lhs, rhs;
	TForm	tfl, tfr;

	lhs = ab->abDefine.lhs;
	rhs = ab->abDefine.rhs;
	if (abTag(lhs) != AB_Declare)
		lhs = abNewDeclare(abPos(ab), lhs, abNewNothing(abPos(ab)));

	tfl = tfp0Float(stab, lhs);
	tfr = tfSyntaxFrAbSyn(stab, rhs);

	return tfNewNode(TF_Define, 2, tfl, tfr);
}

local TForm
tfpAssign(Stab stab, AbSyn ab)
{
	AbSyn	lhs, rhs;
	TForm	tfl, tfr;

	lhs = ab->abAssign.lhs;
	rhs = ab->abAssign.rhs;
	if (abTag(lhs) != AB_Declare)
		lhs = abNewDeclare(abPos(ab), lhs, abNewNothing(abPos(ab)));

	tfl = tfp0Float(stab, lhs);
	tfr = tfSyntaxFrAbSyn(stab, rhs);

	return tfNewNode(TF_Assign, 2, tfl, tfr);
}

local TForm
tfpComma(Stab stab, AbSyn ab)
{
	TForm	tf;
	Length	i, argc;

	argc = abArgc(ab);
	if (argc == 1)
		tf = tfp0Float(stab, abArgv(ab)[0]);
	else {
		tf = tfNewEmpty(TF_Multiple, argc);
		for (i = 0; i < argc; i += 1)
			tfArgv(tf)[i] = tfp0Float(stab, abArgv(ab)[i]);
	}

	return tf;
}

local TForm
tfpAdd(Stab stab, AbSyn ab)
{
	TForm	tf;

	tf = tfNewNode(TF_Add, 1, tfp0Float(stab, ab->abAdd.base));

	return tf;
}

local TForm
tfpWith(Stab stab, AbSyn ab)
{
	TForm	tf;

	tf = tfNewNode(TF_With, 2,
		       tfp0Float(stab, ab->abWith.base),
		       tfp0Float(stab, ab->abWith.within));

	return tf;
}

local TForm
tfpExcept(Stab stab, AbSyn ab)
{
	TForm tf;

	tf = tfNewNode(TF_Except, 2,
		       tfp0Float(stab, ab->abExcept.type),
		       tfp0Float(stab, ab->abExcept.except));

	return tf;
}

local TForm
tfpIf(Stab stab, AbSyn ab)
{
	TForm	test, thenAlt, elseAlt;
	TForm	tf;

	test = tfp0Float(stab, ab->abIf.test);

	tfpConditions = listCons(AbSyn)(ab->abIf.test, tfpConditions);
	thenAlt = tfp0Float(stab, ab->abIf.thenAlt);
	tfpConditions = cdr(tfpConditions);

	elseAlt = tfp0Float(stab, ab->abIf.elseAlt);

	tf = tfNewNode(TF_If, 3, test, thenAlt, elseAlt);

	return tf;
}

local TForm
tfpApply(Stab stab, AbSyn ab)
{
	AbSyn	op;
	Symbol	sym;
	Length	i, argc;
	Syme	syme;
	Syme    opSyme;
	TForm	tf;

	op = abApplyOp(ab);
	if (abTag(op) != AB_Id)
		return tfp0General(stab, ab);

	sym  = op->abId.sym;
	argc = abApplyArgc(ab);
	opSyme = abSyme(op);

	syme = tfp0OpSyme(stab, opSyme, sym, argc);

	if (syme == NULL)
		/* This op is not one of the known special tform ops. */
		tf = tfp0General(stab, ab);

	else if (abSyme(op) 
		 && tfSymesEqModExtends(symeOriginal(abSyme(op)), 
					symeOriginal(syme))) {
		/* This syme is one of the known special tform ops. */
		tf = tfp0OpTForm(sym, argc);
		for (i = 0; i < argc; i += 1)
			tfArgv(tf)[i] = tfp0Float(stab, abApplyArg(ab, i));
	}

	else if (abSyme(op) == NULL &&
		 listMemq(Syme)(stabGetMeanings(stab, NULL, sym), syme)) {
		/* This op could be one of the known special tform ops. */
		/* Don't try to do any type inference here. */
		tf = tfp0OpTForm(sym, argc);
		for (i = 0; i < argc; i += 1)
			tfArgv(tf)[i] = tfp0Float(stab, abApplyArg(ab, i));
	}

	else
		/* This op is not one of the known special tforms ops. */
		tf = tfp0General(stab, ab);

	return tf;
}

/* 
 * tfPending may have to avoid symeEqual, as we might not
 * yet know enough about types.  NB: There might be a better
 * test for this case (where we get an extended meaning in 
 * the above fn).
 */
local Bool
tfSymesEqModExtends(Syme syme1, Syme syme2)
{
	SymeList exts;

	if (syme1 == syme2)
		return true;
	if (symeIsExtend(syme2)) {
		for (exts = symeExtendee(syme2); exts ; exts = cdr(exts)) {
			if (tfSymesEqModExtends(car(exts), syme1))
				return true;
		}	
	}

	if (!symeIsExtend(syme1))
		return false;

	for (exts = symeExtendee(syme1); exts ; exts = cdr(exts)) {
		if (tfSymesEqModExtends(car(exts), syme2))
			return true;
	}
	return false;
}

/*****************************************************************************
 *
 * :: Declarations for tfMeaning
 *
 ****************************************************************************/

local TForm	tfm0General	(Stab, AbSyn, TForm);
local TForm	tfm0Args	(Stab, TForm);
local void	tfm0FoamType	(TForm);

local TForm	tfmNothing	(Stab, AbSyn, TForm);
local TForm	tfmBlank	(Stab, AbSyn, TForm);
local TForm	tfmId		(Stab, AbSyn, TForm);
local TForm	tfmDeclare	(Stab, AbSyn, TForm);
local TForm	tfmDefine	(Stab, AbSyn, TForm);
local TForm	tfmAssign	(Stab, AbSyn, TForm);
local TForm	tfmComma	(Stab, AbSyn, TForm);
local TForm	tfmAdd		(Stab, AbSyn, TForm);
local TForm	tfmWith		(Stab, AbSyn, TForm);
local TForm	tfmIf		(Stab, AbSyn, TForm);
local TForm	tfmApply	(Stab, AbSyn, TForm);
local TForm 	tfmExcept	(Stab, AbSyn, TForm);

/*****************************************************************************
 *
 * :: tfMeaning
 *
 ****************************************************************************/

TForm
tfMeaning(Stab stab, AbSyn ab, TForm tf)
{
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;

	tfFollow(tf);

	assert(!tfIsSyntax(tf));
	if (tfIsMeaning(tf))
		return tf;
	tfSetMeaning(tf);

	if (!abIsSefo(ab)) {
		tfm0Args(stab, tf);
		tfGetSymes(stab, tf, ab);
		tfCheckConsts(tf);
		return tf;
	}
	if (!abIsSefo(tfGetExpr(tf)))
		abTransferSemantics(ab, tfGetExpr(tf));

	serialNo += 1;
	depthNo	 += 1;
	serialThis = serialNo;
	if (DEBUG(tfExpr)) {
		fprintf(dbOut, "->tfm: %*s%d= ", depthNo, "", serialThis);
		abPrettyPrint(dbOut, ab);
		fnewline(dbOut);
	}

	/* The stab is used to look up meanings for ids without them. */
	if (abStab(ab))
		stab = abStab(ab);

	switch (abTag(ab)) {
	case AB_Nothing:	tf = tfmNothing (stab, ab, tf); break;
	case AB_Blank:		tf = tfmBlank	(stab, ab, tf); break;
	case AB_Id:		tf = tfmId	(stab, ab, tf); break;
	case AB_Declare:	tf = tfmDeclare (stab, ab, tf); break;
	case AB_Define:		tf = tfmDefine	(stab, ab, tf); break;
	case AB_Assign:		tf = tfmAssign	(stab, ab, tf); break;
	case AB_Comma:		tf = tfmComma	(stab, ab, tf); break;
	case AB_Add:		tf = tfmAdd	(stab, ab, tf); break;
	case AB_With:		tf = tfmWith	(stab, ab, tf); break;
	case AB_If:		tf = tfmIf	(stab, ab, tf); break;
	case AB_Apply:		tf = tfmApply	(stab, ab, tf); break;
	case AB_Except:		tf = tfmExcept	(stab, ab, tf); break;
	default:		tf = tfm0General(stab, ab, tf); break;
	}

	tfCheckConsts(tf);
	tfm0FoamType(tf);

	if (DEBUG(tfExpr)) {
		fprintf(dbOut, "<-tfm: %*s%d= ", depthNo, "", serialThis);
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}
	depthNo -= 1;

	return tf;
}

void
tfSetMeaningArgs(TForm tf)
{
	Length	i;

	tfSetMeaning(tf);
	for (i = 0; i < tfArgc(tf); i += 1)
		if (!tfIsMeaning(tfFollowArg(tf, i)))
			tfSetTForm(tf);
}

/*****************************************************************************
 *
 * :: tfMeaning helper functions
 *
 ****************************************************************************/

local TForm
tfm0General(Stab stab, AbSyn ab, TForm tf)
{
	return tf;
}

local TForm
tfm0Args(Stab stab, TForm tf)
{
	Length	i, argc = tfArgc(tf);

	for (i = 0; i < argc; i += 1) {
		TForm	tfarg = tfArgv(tf)[i];
		tfMeaning(stab, tfGetExpr(tfarg), tfarg);
	}

	return tf;
}

local void
tfm0FoamType(TForm tf)
{
	AbSyn	ab;
	Syme	syme;
	TForm	ntf;
	Bool	chk = false;
	Symbol	sym;
	FoamTag	tag = FOAM_LIMIT;

	ab = tfGetExpr(tf);
	if (ab == NULL) return;

	syme = abSyme(ab);
	if (syme == NULL) return;

	switch (symeKind(syme)) {
	case SYME_Export:
		ntf = tfDefineeType(symeType(syme));
		chk = tfIsType(ntf);
		break;
	case SYME_Import:
		ntf = tfDefineeType(symeExporter(syme));
		chk = tfIsTheId(ntf, ssymMachine) || tfIsTheId(ntf, ssymBasic);
		break;
	default:
		break;
	}

	if (chk == false) return;
	sym = symeId(syme);

	     if (sym == ssymBool)	tag = FOAM_Bool;
	else if (sym == ssymByte)	tag = FOAM_Byte;
	else if (sym == ssymHInt)	tag = FOAM_HInt;
	else if (sym == ssymSInt)	tag = FOAM_SInt;
	else if (sym == ssymBInt)	tag = FOAM_BInt;
	else if (sym == ssymChar)	tag = FOAM_Char;
	else if (sym == ssymSFlo)	tag = FOAM_SFlo;
	else if (sym == ssymDFlo)	tag = FOAM_DFlo;
	else if (sym == ssymNil)	tag = FOAM_Nil;
	else if (sym == ssymPtr)	tag = FOAM_Ptr;
	else if (sym == ssymArr) 	tag = FOAM_Arr;
	tfFoamType(tf) = tag;
}

/*****************************************************************************
 *
 * :: tfMeaning cases
 *
 ****************************************************************************/

local TForm
tfmNothing(Stab stab, AbSyn ab, TForm tf)
{
	assert(tfIsNone(tf));
	return tfm0General(stab, ab, tf);
}

local TForm
tfmBlank(Stab stab, AbSyn ab, TForm tf)
{
#ifdef UseTypeVariables
	assert(tfIsVariable(tf));
#else
	assert(tfIsUnknown(tf));
#endif

	return tfm0General(stab, ab, tf);
}

local TForm
tfmId(Stab stab, AbSyn ab, TForm tf)
{
	Syme	syme = abSyme(ab);

	/*!! assert(syme); */
	if (!syme) return tf;

	if (symeIsLibrary(syme) || symeIsArchive(syme))
		tfSetSymes(tf, listCons(Syme)(syme, listNil(Syme)));

	return tf;
}

local TForm
tfmDeclare(Stab stab, AbSyn ab, TForm tf)
{
	assert(tfIsDeclare(tf));
	tfMeaning(stab, ab->abDeclare.type, tfDeclareType(tf));
	tfSetSymes(tf, tfSymesFrDeclare(ab));
	return tf;
}

local TForm
tfmDefine(Stab stab, AbSyn ab, TForm tf)
{
	AbSyn	lhs, rhs;
	TForm	tfl, tfr;

	lhs = ab->abDefine.lhs;
	rhs = ab->abDefine.rhs;

	tfl = tfArgv(tf)[0];
	tfr = tfArgv(tf)[1];

	assert(tfIsDefine(tf));
	if (abTag(lhs) != AB_Declare)
		lhs = tfGetExpr(tfl);

	tfMeaning(stab, lhs, tfl);
	if (tfIsSyntax(tfr)) {
		if (tfSatType(tfl))
			tfForwardFrSyntax(tfr, tfp0Float(stab, rhs));
		else
			tfForwardFrSyntax(tfr, tfNewAbSyn(TF_General, rhs));
	}
	tfMeaning(stab, rhs, tfr);

	return tf;
}

local TForm
tfmAssign(Stab stab, AbSyn ab, TForm tf)
{
	AbSyn	lhs, rhs;
	TForm	tfl, tfr;

	lhs = ab->abDefine.lhs;
	rhs = ab->abDefine.rhs;

	tfl = tfArgv(tf)[0];
	tfr = tfArgv(tf)[1];

	assert(tfIsAssign(tf));
	if (abTag(lhs) != AB_Declare)
		lhs = tfGetExpr(tfl);

	tfMeaning(stab, lhs, tfl);
	if (tfIsSyntax(tfr)) {
		if (tfSatType(tfl))
			tfForwardFrSyntax(tfr, tfp0Float(stab, rhs));
		else
			tfForwardFrSyntax(tfr, tfNewAbSyn(TF_General, rhs));
	}
	tfMeaning(stab, rhs, tfr);

	return tf;
}

local TForm
tfmComma(Stab stab, AbSyn ab, TForm tf)
{
	Length	i, argc;

	argc = abArgc(ab);
	if (argc == 1) {
		tfSetTForm(tf);
		tfMeaning(stab, abArgv(ab)[0], tf);
	}
	else {
		assert(tfIsMulti(tf) && argc == tfMultiArgc(tf));
		for (i = 0; i < argc; i += 1)
			tfMeaning(stab, abArgv(ab)[i], tfArgv(tf)[i]);
		tfSetSymes(tf, tfSymesFrMulti(tf));
	}

	return tf;
}

local TForm
tfmAdd(Stab stab, AbSyn ab, TForm tf)
{
	assert(tfIsAdd(tf));

	tfMeaning(stab, ab->abAdd.base, tfAddBase(tf));
	tfSetSymes(tf, tfSymesFrAdd(ab));
	tfGetSelf(stab, tf);

	return tf;
}

local TForm
tfmWith(Stab stab, AbSyn ab, TForm tf)
{
	assert(tfIsWith(tf));

	tfMeaning(stab, ab->abWith.base,   tfWithBase(tf));
	tfMeaning(stab, ab->abWith.within, tfWithWithin(tf));
	tfGetSelf(stab, tf);

	return tf;
}

local TForm
tfmExcept(Stab stab, AbSyn ab, TForm tf)
{
	assert(tfIsExcept(tf));

	tfMeaning(stab, ab->abExcept.type,   tfExceptType(tf));
	tfMeaning(stab, ab->abExcept.except, tfExceptExcept(tf));
	tfGetSelf(stab, tf);

	return tf;
}

local TForm
tfmIf(Stab stab, AbSyn ab, TForm tf)
{
	assert(tfIsIf(tf));

	tfMeaning(stab, ab->abIf.test,    tfIfTest(tf));
	tfMeaning(stab, ab->abIf.thenAlt, tfIfThen(tf));
	tfMeaning(stab, ab->abIf.elseAlt, tfIfElse(tf));
	tfGetSelf(stab, tf);

	return tf;
}

local TForm
tfmApply(Stab stab, AbSyn ab, TForm tf)
{
	if (!tfIsGeneral(tf)) {
		Length		i, argc = abApplyArgc(ab);
		assert(argc == tfArgc(tf));
		for (i = 0; i < argc; i += 1)
			tfMeaning(stab, abApplyArg(ab, i), tfArgv(tf)[i]);
	}
	
	tfGetSymes(stab, tf, ab);

	return tf;
}

void
tfGetSymes(Stab stab, TForm tf, AbSyn ab)
{
	SymeList	symes = listNil(Syme);
	assert(ab);

	switch (tfTag(tf)) {
	case TF_Map:
	case TF_PackedMap:
		symes = tfSymesFrMap(tf);
		break;
	case TF_Cross:
		symes = tfSymesFrCross(tf);
		break;
	case TF_Enumerate:
		symes = tfSymesFrEnum(stab, tf, ab);
		break;
	case TF_RawRecord:
		symes = tfSymesFrRawRecord(stab, tf, ab);
		break;
	case TF_Record:
		symes = tfSymesFrRecord(stab, tf, ab);
		break;
	case TF_TrailingArray:
		symes = tfSymesFrTrailingArray(stab, tf, ab);
		break;
	case TF_Union:
		symes = tfSymesFrUnion(stab, tf, ab);
		break;
	default:
		break;
	}

	tfSetSymes(tf, symes);
}

/******************************************************************************
 *
 * :: tfToAbSyn
 *
 *****************************************************************************/

AbSyn
tfToAbSyn(TForm tf)
{
	return tfToAbSyn0(tf, false);
}

AbSyn
tfToAbSynPretty(TForm tf)
{
	return tfToAbSyn0(tf, true);
}

local AbSyn
tfToAbSyn0(TForm tf, Bool pretty)
{
	AbSyn	ab;
	Syme	syme;
	Length	i;

	tfFollow(tf);

	if (tfHasExpr(tf) && !pretty)
		return tfGetExpr(tf);

	if (DEBUG(tfExpr)) {
		fprintf(dbOut, "tfToAbSyn -> ");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	switch (tfTag(tf)) {
	case TF_Unknown:
		ab = abUnknown;
		break;
	case TF_Exit:
		ab = abNewId(sposNone, ssymExit);
		break;
	case TF_Literal:
		ab = abNewId(sposNone, ssymLiteral);
		break;
	case TF_Test:
		ab = abNewId(sposNone, ssymTest);
		break;
	case TF_Type:
		ab = abNewId(sposNone, ssymType);
		break;
	case TF_Category:
		ab = abNewId(sposNone, ssymCategory);
		break;

	case TF_Syntax:
		ab = tfGetExpr(tf);
		break;
	case TF_General:
		ab = tfGetExpr(tf);
		break;

	case TF_Add:
		ab = abNewAdd(sposNone,
			      tfDisownExpr(tfAddBase(tf), pretty),
			      tfSymesToAdd(tfSymes(tf)));
		break;
	case TF_Assign:
		ab = abNewAssign(sposNone,
				 tfDisownExpr(tfAssignDecl(tf), pretty),
				 tfDisownExpr(tfAssignVal(tf), pretty));
		break;
	case TF_Cross:
		ab = abNewEmpty(AB_Apply, tfArgc(tf) + 1);
		abApplyOp(ab) = abNewId(sposNone, ssymCross);
		for (i = 0; i < tfArgc(tf); i += 1 )
			abSetApplyArg(ab, i,
				      tfDisownExpr(tfArgv(tf)[i], pretty));
		break;
	case TF_Declare:
		syme = tfDeclareSyme(tf);
		if (syme)
			ab = abFrSyme(syme);
		else
			ab = abNewNothing(sposNone);
		ab = abNewDeclare(sposNone, ab,
				  tfDisownExpr(tfDeclareType(tf), pretty));
		break;
	case TF_Define:
		ab = abNewDefine(sposNone, 
				 tfDisownExpr(tfDefineDecl(tf), pretty),
				 tfDisownExpr(tfDefineVal(tf), pretty));
		break;
	case TF_Enumerate:
		ab = abNewEmpty(AB_Apply, tfArgc(tf) + 1);
		abApplyOp(ab) = abNewId(sposNone, ssymEnum);
		for (i = 0; i < tfArgc(tf); i += 1 )
			abSetApplyArg(ab, i,
				      tfDisownExpr(tfArgv(tf)[i], pretty));
		break;
	case TF_Generator:
		ab = abNewApply1(sposNone, abNewId(sposNone, ssymGenerator),
				 tfDisownExpr(tfGeneratorArg(tf), pretty));
		break;
	case TF_If:
		ab = abNewIf(sposNone,
			     tfDisownExpr(tfIfTest(tf), pretty),
			     tfDisownExpr(tfIfThen(tf), pretty),
			     tfDisownExpr(tfIfElse(tf), pretty));
		break;
	case TF_Join:
		ab = abNewEmpty(AB_Apply, tfArgc(tf) + 1);
		abApplyOp(ab) = abNewId(sposNone, ssymJoin);
		for (i = 0; i < tfArgc(tf); i += 1 )
			abSetApplyArg(ab, i,
				      tfDisownExpr(tfArgv(tf)[i], pretty));
		break;
	case TF_Map:
		ab = abNewApply2(sposNone, abNewId(sposNone, ssymArrow),
				 tfDisownExpr(tfMapArg(tf), pretty),
				 tfDisownExpr(tfMapRet(tf), pretty));
		break;
	case TF_Meet:
		ab = abNewEmpty(AB_Apply, tfArgc(tf) + 1);
		abApplyOp(ab) = abNewId(sposNone, ssymMeet);
		for (i = 0; i < tfArgc(tf); i += 1 )
			abSetApplyArg(ab, i,
				      tfDisownExpr(tfArgv(tf)[i], pretty));
		break;
	case TF_Multiple:
		ab = abNewEmpty(AB_Comma, tfArgc(tf));
		for (i = 0; i < tfArgc(tf); i += 1 )
			abArgv(ab)[i] = tfDisownExpr(tfArgv(tf)[i], pretty);
		break;
	case TF_PackedMap:
		ab = abNewApply2(sposNone, abNewId(sposNone, ssymPackedArrow),
				 tfDisownExpr(tfMapArg(tf), pretty),
				 tfDisownExpr(tfMapRet(tf), pretty));
		break;
	case TF_Raw:
		ab = abNewApply1(sposNone, abNewId(sposNone, ssymRaw),
				 tfDisownExpr(tfRawArg(tf), pretty));
		break;
	case TF_RawRecord:
		ab = abNewEmpty(AB_Apply, tfArgc(tf) + 1);
		abApplyOp(ab) = abNewId(sposNone, ssymRawRecord);
		for (i = 0; i < tfArgc(tf); i += 1 )
			abSetApplyArg(ab, i,
				      tfDisownExpr(tfArgv(tf)[i], pretty));
		break;
	case TF_Record:
		ab = abNewEmpty(AB_Apply, tfArgc(tf) + 1);
		abApplyOp(ab) = abNewId(sposNone, ssymRecord);
		for (i = 0; i < tfArgc(tf); i += 1 )
			abSetApplyArg(ab, i,
				      tfDisownExpr(tfArgv(tf)[i], pretty));
		break;
	case TF_Reference:
		ab = abNewApply1(sposNone, abNewId(sposNone, ssymReference),
				 tfDisownExpr(tfReferenceArg(tf), pretty));
		break;
	case TF_TrailingArray: 
		ab = abNewApply2(sposNone, abNewId(sposNone, ssymTrailingArray),
				 tfDisownExpr(tfArgv(tf)[0], pretty),
				 tfDisownExpr(tfArgv(tf)[1], pretty));
		break;
	case TF_Third:
		ab = tfDisownExpr(tfThirdRestrictions(tf), pretty);
		if (pretty)
			ab = abNewDefine(sposNone,
					 abNewId(sposNone, ssymCategory), ab);
		else
			ab = abNewApply1(sposNone,
					 abNewId(sposNone, ssymThird), ab);
		break;
	case TF_Tuple:
		ab = abNewApply1(sposNone, abNewId(sposNone, ssymTuple),
				 tfDisownExpr(tfTupleArg(tf), pretty));
		break;
	case TF_Union:
		ab = abNewEmpty(AB_Apply, tfArgc(tf) + 1);
		abApplyOp(ab) = abNewId(sposNone, ssymUnion);
		for (i = 0; i < tfArgc(tf); i += 1 )
			abSetApplyArg(ab, i,
				      tfDisownExpr(tfArgv(tf)[i], pretty));
		break;
	case TF_Variable:
		ab = abNewBlank(sposNone, ssymVariable);
		break;
	case TF_With: {
		AbSyn	abbase, abwith;

		if (tfIsNone(tfWithBase(tf)))
			abbase = abNewNothing(sposNone);
		else
			abbase = tfDisownExpr(tfWithBase(tf), pretty);

		if (pretty && tfUseCatExports(tf))
			abwith = tfSymesToWith(tfGetCatExports(tf));
		else if (tfIsNone(tfWithWithin(tf)))
			abwith = abNewNothing(sposNone);
		else
			abwith = tfDisownExpr(tfWithWithin(tf), pretty);

		ab = abNewWith(sposNone, abbase, abwith);
		break;
	}
	case TF_Except:
		ab = abNewExcept(sposNone, 
				 tfDisownExpr(tfExceptType(tf), pretty),
				 tfDisownExpr(tfExceptExcept(tf), pretty));
		break;

	default:
		bugBadCase(tfTag(tf));
		NotReached(ab = abNewNothing(sposNone));
	}

	if (DEBUG(tfExpr)) {
		fprintf(dbOut, "tfToAbSyn <- ");
		abPrettyPrintClippedIn(dbOut, ab, 65, 1);
		fnewline(dbOut);
	}

	if (!pretty) {
		tfSetExpr(tf, ab);
		tfOwnExpr(tf);
	}
	return ab;
}

/******************************************************************************
 *
 * :: Type form syntax.
 *
 *****************************************************************************/

local void
tfSetExpr(TForm tf, AbSyn ab)
{
	if (!tfHasExpr(tf))
		tf->__absyn = ab;

	if (!abTForm(ab))
		abSetTForm(ab, tf);

	return;
}

local Bool
tfOwnsExpr(TForm tf)
{
	return tf->ownSyntax;
}

/*
 * tfOwnExpr assumes that tfExpr(tf) was just constructed
 * from newly created abstract syntax nodes.
 */
void
tfOwnExpr(TForm tf)
{
	tf->ownSyntax = true;
	return;
}

/* Release any ownership interest that tf has in ab. */
void
tfReleaseExpr(TForm tf, AbSyn ab)
{
	if (tf->__absyn == ab) {
		tf->__absyn = 0;
		tf->ownSyntax = false;
	}
}

/*
 * Return an abstract syntax tree which the caller
 * is allowed/required to free.
 */
local AbSyn
tfDisownExpr(TForm tf, Bool pretty)
{
	AbSyn	ab = tfToAbSyn0(tf, pretty);

	if (DEBUG(tfExpr)) {
		fprintf(dbOut, "tfDisownExpr -> ");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
		abPrettyPrintClippedIn(dbOut, ab, 65, 1);
		fnewline(dbOut);
	}

	if (!pretty) {
		if (!tfOwnsExpr(tf)) ab = sefoCopy(ab);
		tf->ownSyntax = false;
	}
	return ab;
}

/* Transfer the type form semantics from ntf to tf. */
void
tfTransferSemantics(TForm ntf, TForm tf)
{
	if (!abIsSefo(tfGetExpr(tf)) &&
	    abIsSefo(tfGetExpr(ntf)) && !tfIsUnknown(tfTUnique(ntf))) {
		abTransferSemantics(tfGetExpr(ntf), tfGetExpr(tf));
		tfCopyState(tf, ntf);
	}
}

/******************************************************************************
 *
 * :: Type form construction path.
 *
 *****************************************************************************/

/*
 * Create a syntactic tform.
 */
TForm
tfSyntaxFrAbSyn(Stab stab, AbSyn ab)
{
	Stab	nstab = (abStab(ab) ? abStab(ab) : stab);
	TForm	tf, tfret;

	if ((tf = abTForm(ab)) != NULL)
		return tf;

	if (abIsTheId(ab, ssymCategory))
		return tfCategory;
	
	if (abIsId(ab) && abSyme(ab) && symeIsParam(abSyme(ab))) {
		Stab rstab = stabFindLevel(stab, abSyme(ab));
		stab = rstab;
		    
	}
	if ((tf = stabGetTForm(stab, ab, NULL)) != NULL)
		return tf;

	if (abIsAnyMap(ab)) {
		tfret = tfSyntaxFrAbSyn(nstab, abMapRet(ab));
		tf = tfSyntaxMap(nstab, ab, tfret);
	}
	else
		tf = tfNewSyntax(ab);

	stabDefTForm(stab, tf);

	return tf;
}

/*
 * Create a map type form with syntactic parts.
 */
TForm
tfSyntaxMap(Stab stab, AbSyn map, TForm tfret)
{
	AbSyn	ab, abarg = abMapArg(map);
	Length	i, argc = abArgc(abarg);
	TForm	tf, tfarg;

	if (abTag(abarg) == AB_Comma && argc != 1) {
		tfarg = tfNewEmpty(TF_Multiple, argc);
		for (i = 0; i < argc; i += 1)
			tfArgv(tfarg)[i] =
				tfSyntaxFrAbSyn(stab, abArgv(abarg)[i]);
		abSetPos(tfExpr(tfarg), abPos(abarg));
	}
	else
		tfarg = tfSyntaxFrAbSyn(stab, abarg);

	tf = tfAnyMap(tfarg, tfret, abIsPackedMap(map));
	ab = tfExpr(tf);

	tfSetPending(tf);
	abSetTForm(map, tf);
	abTransferSemantics(map, ab);

	abSetPos(ab, abPos(map));
	abSetPos(abApplyOp(ab), abPos(abApplyOp(map)));

	return tf;
}

void
tfSyntaxConditions(Stab stab, TForm tf, TfCondElt conditions)
{
	int i;
	if (DEBUG(tf)) {
		if (conditions != NULL)
			afprintf(dbOut, "Adding Condition: %pTForm - %pAbSynList %d\n", 
				 tf, conditions->list,
				 tfIsSyntax(tf) || tfIsMap(tf) || tfIsEmptyMulti(tf) || tfIsWith(tf));
	}
	assert(tfIsSyntax(tf) || tfIsMap(tf) || tfIsMulti(tf) || tfIsWith(tf) || tfIsCategory(tf));
	if (!tfIsMeaning(tf))
		tfMergeConditions(tf, stab, conditions);

	for (i=0; i<tfArgc(tf); i++) {
		tfSyntaxConditions(stab, tfArgv(tf)[i], conditions);
	}
}


/*
 * Create a define type form with syntactic parts.
 */
TForm
tfSyntaxDefine(Stab stab, AbSyn lhs, AbSyn rhs)
{
	TForm	tf, tflhs, tfrhs;
	AbSyn	ab;

	if (abHasTag(rhs, AB_Label))
		rhs = rhs->abLabel.expr;

	tflhs = tfNewNode(TF_Declare, 1, tfNewSyntax(lhs));
	tfrhs = tfNewSyntax(rhs);
	tf = tfNewNode(TF_Define, 2, tflhs, tfrhs);

	ab = tfExpr(tf);

	abTransferSemantics(lhs, ab->abDefine.lhs);
	abTransferSemantics(rhs, ab->abDefine.rhs);

	tfSetPending(tf);
	abSetTForm(lhs, tf);

	stabDefTForm(stab, tf);

	return tf;
}

TForm
tfSyntaxDefineMap(Stab stab, AbSyn ab, AbSyn rhs)
{
	Stab	nstab = (abStab(ab) ? abStab(ab) : stab);
	TForm	tf, tfret;

	assert(abIsAnyLambda(rhs));

	tfret = tfSyntaxDefine(nstab, abMapRet(ab), rhs->abLambda.body);
	tf = tfSyntaxMap(nstab, ab, tfret);

	stabDefTForm(stab, tf);

	return tf;
}

/*
 * Create a template type form for an extension.
 */
TForm
tfSyntaxExtend(Stab stab, AbSyn id, AbSyn type)
{
	Stab	nstab = (abStab(type) ? abStab(type) : stab);
	TForm	tf;

	if (abIsAnyMap(type)) {
		tf = tfSyntaxExtendMap(nstab, type, tfUnknown);
		tfSetStab(tf, nstab);
	}
	else
		tf = tfUnknown;

	return tfDeclare(id, tf);
}

TForm
tfSyntaxExtendMap(Stab stab, AbSyn map, TForm tfret)
{
	AbSyn	abarg = abMapArg(map);
	Length	i, argc = abArgc(abarg);
	TForm	tfarg;

	if (abTag(abarg) == AB_Comma && argc != 1) {
		tfarg = tfNewEmpty(TF_Multiple, argc);
		for (i = 0; i < argc; i += 1) {
			AbSyn	id = abDefineeId(abarg->abComma.argv[i]);
			tfArgv(tfarg)[i] = tfDeclare(id, tfUnknown);
		}
	}
	else {
		AbSyn id = abDefineeId(abarg);
		tfarg = tfDeclare(id, tfUnknown);
	}

	return tfAnyMap(tfarg, tfret, abIsPackedMap(map));
}

/*
 * Create non-forwarding tform from type analysed abstract syntax.
 */
TForm
tfFullFrAbSyn(Stab stab, Sefo sefo)
{
	TForm tf;

	if ((tf = abTForm(sefo)) != NULL)
		return tfMeaningFrSyntax(stab, sefo, tf);

	else if ((tf = stabGetTForm(stab, sefo, NULL)) != NULL)
		return tfMeaningFrSyntax(stab, sefo, tf);

	if (abStab(sefo))
		stab = abStab(sefo);

	tf = tfMeaning(stab, sefo, tfPending(stab, sefo));
	stabDefTForm(stab, tf);

	return tf;
}

/*
 * Create a forwarding tform from a syntax tform.
 * The new tform may be a full tform or a pending tform.
 */
TForm
tfForwardFrSyntax(TForm otf, TForm ntf)
{
	if (!tfIsSyntax(otf)) return otf;
	assert(otf != ntf);
	assert(!tfIsForward(ntf));

	otf->tag     = TF_Forward;
	otf->argc    = 1;
	otf->argv[0] = ntf;

	tfCopyQueries	(ntf, otf);	/* queries (ntf) := queries (otf) */
	tfCopyState	(otf, ntf);	/* state   (otf) := state   (ntf) */
	tfCopySelf	(ntf, otf);	/* self    (ntf) := self    (otf) */
	tcMove		(ntf, otf);	/* const   (ntf) := const   (otf) */

	return otf;
}

/*
 * Create a pending tform from a syntax tform.
 */
TForm
tfPendingFrSyntax(Stab stab, AbSyn ab, TForm tf)
{
	tfFollow(tf);

	if (tfIsSyntax(tf)) {
		TForm tfp = tfPending(stab, ab);
		/* This test is probably too weak */
		if (!tfIsId(tfp))
			tfSetConditions(tfp, tfConditions(tf));
		tfForwardFrSyntax(tf, tfFollowOnly(tfp));
	}
	else if (tfIsAnyMap(tf)) {
		assert(abIsAnyMap(ab));
		tfPendingFrSyntaxMap(stab, ab, tf);
	}

	else if (tfIsDefine(tf) && abHasTag(ab, AB_Define))
		tfPendingFrSyntaxDefine(stab, ab, tf);

	return tf;
}

local void
tfPendingFrSyntaxMap(Stab stab, AbSyn ab, TForm tf)
{
	AbSyn	abarg = abMapArg(ab);
	AbSyn	abret = abMapRet(ab);
	TForm	tfarg = tfMapArg(tf);
	TForm	tfret = tfMapRet(tf);
	Length	i;

	if (tfIsMulti(tfarg))
		for (i = 0; i < tfArgc(tfarg); i += 1) {
			TForm	tfi = tfFollowArg(tfarg, i);
			AbSyn	abi = abArgv(abarg)[i];
			tfPendingFrSyntax(stab, abi, tfi);
		}

	if (abTag(abarg) == AB_Comma && abArgc(abarg) == 1)
		abarg = abArgv(abarg)[0];

	tfPendingFrSyntax(stab, abarg, tfarg);
	tfPendingFrSyntax(stab, abret, tfret);
}

local void
tfPendingFrSyntaxDefine(Stab stab, AbSyn ab, TForm tf)
{
	AbSyn	abdec = abDefineDecl(ab);
	AbSyn	abval = abDefineVal(ab);
	TForm	tfdec = tfDefineDecl(tf);
	TForm	tfval = tfDefineVal(tf);

	tfPendingFrSyntax(stab, abdec, tfdec);
	tfPendingFrSyntax(stab, abval, tfval);
}

/*
 * Create a meaningful tform from a syntax tform.
 */
TForm
tfMeaningFrSyntax(Stab stab, Sefo sefo, TForm tf)
{
	TForm	ntf, stf;

	tfFollow(tf);

	if (!abIsSefo(sefo))
		return tf;

	stf = abTForm(sefo);
	tfFollow(stf);

	/* Convert syntax tforms to pending tforms. */
	if (tfIsSyntax(tf))
		ntf = tfPending(stab, sefo);

	/* Avoid using pending tforms which don't have semantics. */
	else if (tfIsPending(tf) && !abIsSefo(tfGetExpr(tf)))
		ntf = tfPending(stab, sefo);

	/* Avoid using tforms with different semantics. */
	else if (tfIsMeaning(tf) && tf != stf &&
		!tfMeaningEqual(sefo, tfGetExpr(tf)))
		ntf = tfPending(stab, sefo);

	/* Convert pending tforms to meaningful tforms. */
	else
		ntf = tf;

	ntf = tfMeaning(stab, sefo, ntf);

	if (tfIsSyntax(tf))
		tfForwardFrSyntax(tf, ntf);

	return ntf;
}

/* Quickie version of sefoEqual since we can't use sefoEqual
 * inside tfMeaningFrSyntax.
 */
local Bool
tfMeaningEqual(Sefo sefo1, Sefo sefo2)
{
	Bool	result;

	if (sefo1 == sefo2)
		result = true;

	else if (!sefo1 || !sefo2)
		result = false;

	else if (abTag(sefo1) != abTag(sefo2))
		result = false;
	else if (abIsLeaf(sefo1))
		result = abSyme(sefo1) == abSyme(sefo2);

	else
		result = false;

	return result;
}

/******************************************************************************
 *
 * :: Type form floating.
 *
 *****************************************************************************/

/*
 * Float a type form to its natural level. Return 0 if floating
 * not done.
 */
Stab
tfFloat(Stab stab, TForm tf)
{
	ULong	odepth, ndepth;
	TForm	outerTf = NULL;
	Stab    ostab = stab;

	if (! tfIsSyntax(tf))
		return NULL;

	odepth = stabLevelNo(stab);
	ndepth = tfOuterDepth(stab, tf);
	if (ndepth >= odepth) {
		/* No floating needed - clean up conditions */
		tfFloatConditions(stab, tf);
		return NULL;
	}
	while (stabLevelNo(stab) > ndepth) {
		stab = cdr(stab);
		outerTf = stabGetTForm(stab, tfGetExpr(tf), NULL);
		if (outerTf && tfQueries(outerTf)) break;
	}

	if (! outerTf) {
		outerTf = tfNewSyntax(tfGetExpr(tf));
		tfSetConditions(outerTf, tfFloatConditions(stab, tf)); 
		stabDefTForm(stab, outerTf);
	}

	if (tf == outerTf)
		return NULL;

	if (DEBUG(tfFloat)) {
		fprintf(dbOut, "Floating from stab level %lu to level %lu.",
			odepth, ndepth);
		findent += 2;
		fnewline(dbOut);
		fprintf(dbOut,"Inner tform = ");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
		fprintf(dbOut,"Outer tform = ");
		tfPrint(dbOut, outerTf);
		findent -= 2;
		fnewline(dbOut);
	}
	outerTf = tfFollowOnly(outerTf);
	tf = tfForwardFrSyntax(tf, outerTf);
	tf->conditions = NULL;

	return stab;
}

/*
 * Compute the outermost symbol table depth for which the interpretation
 * of tf is known to be the same as it is in stab.
 */
ULong
tfOuterDepth(Stab stab, TForm tf)
{
	ULong		depth;
	TFormList	hl;

	depth = abOuterDepth(stab, tfGetExpr(tf));
	for (hl = tfQueries(tf); hl; hl = cdr(hl)) {
		ULong	d = tfOuterDepth(stab, car(hl));
		if (d > depth) depth = d;
	}

	return depth;
}

/*
 * Compute the outermost symbol table depth for which the interpretation
 * of ab is known to be the same as it is in stab.
 */
local ULong abOuterDepth0	(Stab stab, Stab istab, AbSyn ab);

ULong
abOuterDepth(Stab stab, AbSyn ab)
{
	return abOuterDepth0(stab, stab, ab);
}

local ULong
abOuterDepth0(Stab stab, Stab istab, AbSyn ab)
{
	SymeList	sl;
	ULong	depth;
	Symbol  sym;
	Length	i;

	if (abStab(ab))
		istab = abStab(ab);

	switch (abTag(ab)) {
	case AB_LitInteger: 
		sym = ssymTheInteger;
		goto handle_id;
		break;
	case AB_LitString: 
		sym = ssymTheString;
		goto handle_id;
		break;
	case AB_LitFloat: 
		sym = ssymTheFloat;
		goto handle_id;
		break;
	case AB_Id: {
		sym = ab->abId.sym;

		/* Do not float the symbol Category. */
handle_id:		
		if (sym == ssymCategory)
			return stabLevelNo(stab);

		/* Do not float symbols with no meanings. */
		sl = stabGetMeanings(istab, NULL, sym);
		if (sl == listNil(Syme))
			return stabLevelNo(stab);

		depth = 0;
		for (; sl; sl = cdr(sl)) {
			Syme	syme = car(sl);
			ULong	d = symeDefLevelNo(syme);
			
			/* Ignore locally defined symbol meanings */
			if (d > stabLevelNo(stab)) {
				d = 0;
			}
			/* Do not float to stabGlobal. */
			/* Do not float to a level where sym is assigned. */
			/* Do not float to a level where sym is extended. */
			/* Do not float if a type is not yet analyzed. */
			/* Except if is `%', then just wing it... */
			else if (sym == ssymSelf)
				;
			else if (d == 0 ||
			    symeIsLexVar(syme) ||
			    symeIsExtend(syme) ||
			    (!symeIsImport(syme) &&
			     (tfIsUnknown(symeType(syme)) ||
			      tfIsSyntax(symeType(syme)))))
				d += 1;
			if (d > depth) depth = d;
		}
		
		/* Do not float to a level where sym is extended. */
		for (; stab; stab = cdr(stab)) {
			ULong	d = car(stab)->lexicalLevel + 1;
			sl = car(stab)->extendSymes;
			for (; d > depth && sl; sl = cdr(sl)) {
				Syme	syme = car(sl);
				if (symeIsExtend(syme) && symeId(syme) == sym)
					depth = d;
			}
		}
		if (DEBUG(tfFloat)) {
			Buffer buf = bufNew();
			bufPrintf(buf, "AbSyn: %s [%pAbSyn] %d\n", symString(sym), ab, depth);
			fprintf(dbOut, "%s", bufLiberate(buf));
		}
		break;
	}
	case AB_PretendTo:
	case AB_Declare:
	case AB_Apply: 
	case AB_Comma:
	case AB_With:
	case AB_Has:
	case AB_RestrictTo:
	case AB_And:
	case AB_Or:
	case AB_If:
	case AB_Test:
		depth = 1;
		for (i = 0; i < abArgc(ab); i += 1) {
			ULong	d = abOuterDepth0(stab, istab, abArgv(ab)[i]);
			if (d > depth) depth = d;
		}
		break;
	default:
		depth = stabLevelNo(stab);
		break;
	}

	assert(depth > 0 || stab != istab);
	return depth;
}


/******************************************************************************
 *
 * :: Type form semantics.
 *
 *****************************************************************************/

/******************************************************************************
 *
 * :: Type form has questions.
 *
 *****************************************************************************/

TFormList
tfCopyQueries(TForm to, TForm from)
{
	if (tfQueries(from) == listNil(TForm))
		return tfQueries(to);

	/*
	 * No sense in duplicating queries. In fact we ought to do better
	 * than this and compute the set union of both sets of queries.
	 * That way we don't duplicate them.
	 */
	if (tfQueries(from) == tfQueries(to))
		return tfQueries(to);

	tfSetQueries(to, listConcat(TForm)(tfQueries(from), tfQueries(to)));

	if (tfHasSelf(to)) /* Ooops: see stabIsImportedTForm */
		tfHasSelf(to) = false;
	if (tfDomExports(to))
		tfSetDomExports(to, listNil(Syme));
	if (tfDomImports(to)) {
		listFree(Syme)(tfDomImports(to));
		tfSetDomImports(to, listNil(Syme));
	}

	return tfQueries(to);
}

TForm
tfAddQuery(TForm tf, TForm cat)
{
	tfDEBUG(dbOut, "Adding query %pTForm %pTForm\n", tf, cat);
	tfSetQueries(tf, listCons(TForm)(cat, tfQueries(tf)));
	return cat;
}

/******************************************************************************
 *
 * :: Type form sefo accessors.
 *
 *****************************************************************************/

TForm
abGetCategory(AbSyn ab)
{
	TForm	cat = tfUnknown;

	if (abState(ab) == AB_State_HasUnique)
		cat = abTUnique(ab);

	else if (abState(ab) == AB_State_HasPoss && tpossIsUnique(abTPoss(ab)))
		cat = tpossUnique(abTPoss(ab));

	else if (abTag(ab) == AB_Id && abSyme(ab))
		cat = symeType(abSyme(ab));

	else if (abTForm(ab))
		cat = tfGetCategory(abTForm(ab));

	else if (tiTopFns()->tiCanSefo(ab)) {
		tiTopFns()->tiSefo(stabFile(), ab);
		if (abState(ab) == AB_State_HasUnique)
			cat = abTUnique(ab);
	}

	return cat;
}

SymeList
abGetCatExports(Sefo cat)
{
	if (abIsNotNothing(cat) && abState(cat) == AB_State_HasUnique)
		return tfGetThdExports(abTUnique(cat));
	else
		return listNil(Syme);
}

SymeList
abGetCatSelf(Sefo cat)
{
	if (abIsNotNothing(cat) && abState(cat) == AB_State_HasUnique)
		return tfGetThdSelf(abTUnique(cat));
	else
		return listNil(Syme);
}

/******************************************************************************
 *
 * :: Type form symbol table.
 *
 *****************************************************************************/

Stab
tfDefStab(Stab stab, TForm tf)
{
	Stab	nstab = NULL;

	if (tfStab(tf))
		return tfStab(tf);

	if (tfHasExpr(tf)) {
		nstab = abStab(tfGetExpr(tf));
		if (nstab == NULL) {
			nstab = stabPushLevel(stab, sposNone, long0);
			abSetStab(tfGetExpr(tf), nstab);
		}
	}
	else
		nstab = stabPushLevel(stab, sposNone, long0);

	tfSetStab(tf, nstab);
	return nstab;
}

Stab
tfGetStab(TForm tf)
{
	Stab	stab = NULL;

	if (tfStab(tf))
		return tfStab(tf);

	if (tfHasExpr(tf))
		stab = abStab(tfGetExpr(tf));

	tfSetStab(tf, stab);
	return stab;
}

/******************************************************************************
 *
 * :: Type form category.
 *
 *****************************************************************************/

/*
 * Anything that might have a later extend.... 
 */

#define		tfp0IsSpecialTag(tag)		\
	((tag) == TF_Tuple 	 	|| (tag) == TF_Record \
	 || (tag) == TF_RawRecord	|| (tag) == TF_Reference \
	 || (tag) == TF_Generator	|| (tag) == TF_Union \
	 || (tag) == TF_TrailingArray   || (tag) == TF_Enumerate)

TForm
tfGetCategory(TForm tf)
{
	TForm	cat = tfUnknown;

	if (tfHasUnique(tf) && !tfIsUnknown(tfTUnique(tf)))
		cat = tfTUnique(tf);

	else if (tfHasPoss(tf) && tpossIsUnique(tfTPoss(tf)))
		cat = tpossUnique(tfTPoss(tf));

	else if (tfIsId(tf) && tfIdSyme(tf))
		cat = symeType(tfIdSyme(tf));

	else if (tfNeedsSefo(tf)) {
		tiTopFns()->tiTfSefo(stabFile(), tf);
		cat = tfGetCategory(tf);
	}

#ifdef TFormBuiltinSefo
	else if (tfIsSym(tf) && abSyme(tfExpr(tf)))
		cat = symeType(tfIdSyme(tf));

#endif
	else if (tfp0IsSpecialTag(tfTag(tf)) && tiTopFns()->tiCanSefo(tfExpr(tf))) {
		tiTopFns()->tiSefo(stabFile(), tfExpr(tf));
		if (tfHasUnique(tf))
			cat = tfTUnique(tf);
	}

	if (tfIsDefine(cat) && tfDefineVal(cat) == tf)
		cat = tfDefineDecl(cat);

	return cat;
}

/******************************************************************************
 *
 * :: Type form symbol meanings for self.
 *
 *****************************************************************************/

SymeList
tfCopySelf(TForm to, TForm from)
{
	if (!tfHasSelf(to) && tfSelf(to) == listNil(Syme)) {
		tfSetSelf(to, listCopy(Syme)(tfSelf(from)));
		tfHasSelf(to) = tfHasSelf(from);
	}

	return tfSelf(to);
}

SymeList
tfAddSelf(TForm tf, SymeList self2)
{
	SymeList	self1  = tfSelf(tf);
	SymeList	result = listNil(Syme);

	for (; self2; self2 = cdr(self2))
		if (!symeListMember(car(self2), self1, symeEq))
			result = listCons(Syme)(car(self2), result);

	result = listNConcat(Syme)(self1, listNReverse(Syme)(result));
	tfSetSelf(tf, result);

	return result;
}

SymeList
tfUnionSelf(TForm tf, SymeList self2)
{
	return tfAddSelf(tf, self2);
}

SymeList
tfGetSelfFrStab(Stab stab)
{
	SymeList	symes = listNil(Syme);
	Syme		syme = stabGetSelf(stab);

	if (syme) symes = listCons(Syme)(syme, symes);

	return symes;
}

SymeList
tfGetSelf(Stab stab, TForm tf)
{
	return tfSetSelf(tf, tfGetSelfFrStab(stab));
}

SymeList
tfDefSelf(Stab stab, TForm tf)
{
	SymeList	symes;
	Syme		syme;
	TForm		ntf;

	if (tfSelf(tf) || tfGetSelf(stab, tf))
		return tfSelf(tf);

	ntf = tfDefineOfType(tf);
	stab = tfDefStab(stab, tf);
	syme = stabDefLexVar(stab, ssymSelf, ntf);
	stabSetSubstable(stab);

	symes = listCons(Syme)(syme, listNil(Syme));
	tfSetSelf(tf, symes);
	return symes;
}

/*
 * Called on a domain to get the symbol meaning(s) for %.
 */
SymeList
tfGetDomSelf(TForm tf)
{
	TForm		cat;
	TFormList	hl;

	tf = tfDefineeType(tf);

	if (tfHasSelf(tf))
		return tfSelf(tf);

	cat = tfGetCategory(tf);
	if (tfIsUnknown(cat))
		return tfSelf(tf);
	if (!tfIsAdd(tf) && !tfHasCatExports(cat))
		return tfSelf(tf);
	
	if (DEBUG(tfImport)) {
		fprintf(dbOut, "(tfGetDomSelf:  from ");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}
	
	if (tfIsArchive(tf) || tfIsLibrary(tf))
		tfAddSelf(tf, listNil(Syme));
	else if (tfIsAdd(tf) && tfGetStab(tf))
		tfAddSelf(tf, tfGetSelfFrStab(tfGetStab(tf)));
	else 
		tfAddSelf(tf, tfGetCatSelf(cat));

	for (hl = tfQueries(tf); hl; hl = cdr(hl))
		tfAddSelf(tf, tfGetCatSelf(car(hl)));

	tfImportDEBUG(dbOut, ")\n");

	tfHasSelf(tf) = true;
	return tfSelf(tf);
}

/*
 * Called on a category to get the symbol meaning(s) for %.
 */
SymeList
tfGetCatSelf(TForm cat)
{
	tfFollow(cat);

	if (tfIsDefine(cat)) {
		if (tfHasSelf(cat) || !tfHasCatExports(cat))
			return tfSelf(cat);

		tfAddSelf(cat, tfGetDomSelf(tfDefineVal(cat)));
		tfAddSelf(cat, tfGetCatSelf(tfDefineeType(cat)));

		tfHasSelf(cat) = true;
		return tfSelf(cat);
	}

	cat = tfDefineeType(cat);

	if (tfHasSelf(cat))
		return tfSelf(cat);

	if (!tfHasCatExports(cat))
		return tfSelf(cat);

	if (tfIsUnknown(cat) || tfIsNone(cat))
		return tfSelf(cat);

	if (DEBUG(tfCat)) {
		fprintf(dbOut, "(tfGetCatSelf:  from ");
		tfPrint(dbOut, cat);
		fnewline(dbOut);
	}

	if (tfIsWith(cat)) {
		TForm		tfb = tfWithBase(cat);
		TForm		tfw = tfWithWithin(cat);
		SymeList	wself;

		tfAddSelf(cat, tfGetCatSelf(tfb));

		tfFollow(tfw);
		if (tfHasSelf(tfw))
			wself = tfSelf(tfw);

		else if (tfIsDeclare(tfw))
			wself = listNil(Syme);

		else if (tfHasExpr(tfw))
			wself = tfGetCatSelfFrWith(tfGetExpr(tfw));
		else
			wself = tfGetCatSelf(tfw);

		tfAddSelf(tfw, wself);
		tfHasSelf(tfw) = true;

		tfAddSelf(cat, wself);
	}

	else if (tfIsIf(cat)) {
		tfAddSelf(cat, tfGetCatSelf(tfIfThen(cat)));
		tfAddSelf(cat, tfGetCatSelf(tfIfElse(cat)));
	}

	else if (tfIsJoin(cat) || tfIsMeet(cat)) {
		Length	i, argc = tfArgc(cat);
		for (i = 0; i < argc; i += 1)
			tfAddSelf(cat, tfGetCatSelf(tfFollowArg(cat, i)));
	}

	else
		tfAddSelf(cat, tfGetThdSelf(tfGetCategory(cat)));

	if (DEBUG(tfCat)) {
		fprintf(dbOut, ")\n");
		fnewline(dbOut);
	}

	tfHasSelf(cat) = true;
	return tfSelf(cat);
}

/*
 * Called on a third-order type to get the symbol meaning(s) for %.
 */
SymeList
tfGetThdSelf(TForm thd)
{
	thd = tfDefineeType(thd);

	if (tfHasSelf(thd))
		return tfSelf(thd);

	if (!tfHasThdExports(thd))
		return tfSelf(thd);

	if (tfIsThird(thd))
		tfAddSelf(thd, tfGetCatSelf(tfThirdRestrictions(thd)));

	tfHasSelf(thd) = true;
	return tfSelf(thd);
}

/*
 * tfGet...Self helper functions.
 */

local SymeList
tfGetCatSelfFrWith(Sefo sefo)
{
	SymeList	csymes, symes;
	Length		i, argc;
	Sefo		*argv;
	TForm		cat;

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	default:
		argc = 1;
		argv = &sefo;
		break;
	}

	symes = listNil(Syme);
	for (i = 0; i < argc; i++) {
		Sefo	id = abDefineeIdOrElse(argv[i], NULL);

		if (abTag(argv[i]) == AB_Nothing)
			continue;

		/* Defaults package. */
		else if (abTag(argv[i]) == AB_Default)
			continue;

		/* Explicit declaration. */
		else if (id && abTag(argv[i]) != AB_Id)
			continue;

		/* Category expression. */
		else if (!tfIsUnknown(cat = abGetCategory(argv[i])) &&
			 tfSatCat(cat)) {
			csymes = tfGetThdSelf(cat);
			symes = symeListUnion(symes, csymes, symeEq);
		}
	}

	return symes;
}

/******************************************************************************
 *
 * :: Type form parent symes.
 *
 *****************************************************************************/

local SymeList		tfGetCatParentsFrWith	(TForm);
local SymeList		tfGetCatParentsFrIf	(TForm);
local SymeList		tfGetCatParentsFrJoin	(TForm);
local SymeList		tfGetCatParentsFrInner	(TForm);

local SymeList		abGetCatParents		(Sefo);

/*
 * Called on a semantic category form to get the symbol meaning for %%
 * to serve as a representation of all of the exports of the category.
 */
SymeList
abGetCatSelfSelf(Sefo sefo)
{
	TForm		thd;
	AbSub		sigma;
	SymeList	symes;

	if (!tfIsUnknown(thd = abGetCategory(sefo)))
		return tfGetThdSelfSelf(thd);

	else if (abHasTag(sefo, AB_Id))
		thd = symeType(abSyme(sefo));

	/*!! We really should handle curried categories. */
	else if (abHasTag(sefo, AB_Apply)) {
		TForm	tf = symeType(abSyme(abApplyOp(sefo)));
		tfFollow(tf);
		assert(tfIsAnyMap(tf));
		thd = tfMapRet(tf);
	}

	else
		return listNil(Syme);

	sigma = tfSatSubList(sefo);
	/*!! assert(sigma); */
	if (sigma == NULL) return listNil(Syme);

	symes = symeListSubstSigma(sigma, tfGetThdSelfSelf(thd));
	absFree(sigma);
	return symes;
}

/*
 * Called on a category to get the symbol meaning for %%
 * to serve as a representation of all of the exports of the category.
 */
SymeList
tfGetCatSelfSelf(TForm cat)
{
	SymeList	symes = listNil(Syme);

	cat = tfDefineeType(cat);

	if (tfHasSelfSelf(cat) ||
	    tfIsUnknown(cat) || tfIsNone(cat))
		return tfSelfSelf(cat);

	if (tfIsId(cat) || tfIsApply(cat)) {
		if (tfHasUnique(cat) && !tfIsUnknown(tfTUnique(cat)))
			symes = abGetCatSelfSelf(tfGetExpr(cat));
		else if (tfIsGeneral(cat) && tiTopFns()->tiCanSefo(tfGetExpr(cat))) {
			tiTopFns()->tiSefo(stabFile(), tfGetExpr(cat));
			symes = abGetCatSelfSelf(tfGetExpr(cat));
		}
		else
			/* Don't cache if semantics may come later. */
			return symes;
	}

	tfHasSelfSelf(cat) = true;
	tfSelfSelf(cat) = symes;
	tfAuditExportList(symes);
	return symes;
}

/*
 * Called on a third-order type to get the symbol meaning for %%
 * to serve as a representation of all of the exports of the third-order type.
 */
SymeList
tfGetThdSelfSelf(TForm thd)
{
	SymeList	symes;

	thd = tfDefineeType(thd);

	if (tfHasSelfSelf(thd) || tfNeedsSefo(thd) ||
	    tfIsUnknown(thd) || tfIsNone(thd))
		return tfSelfSelf(thd);

	if (tfIsThird(thd))
		symes = listCopy(Syme)(tfSymes(thd));
	else
		symes = listNil(Syme);

	tfHasSelfSelf(thd) = true;
	tfSelfSelf(thd) = symes;
	tfAuditExportList(symes);
	return symes;
}

/*
 * Called on a category to get the symbol meanings which serve as
 * 'parents' of all of the exports of the category.
 */
SymeList
tfGetCatParents(TForm cat, Bool top)
{
	SymeList	symes;

	tfFollow(cat);
	if (tfIsDefineOfType(cat))
		return tfGetCatParents(tfGetCategory(tfDefineVal(cat)), top);

	cat = tfDefineeType(cat);

	if (tfCatExports(cat) || tfIsUnknown(cat) || tfIsNone(cat))
		symes = listCopy(Syme)(tfCatExports(cat));

	else if (tfIsWith(cat) && tfUseCatExports(cat))
		symes = listCopy(Syme)(tfParents(cat));

	else if (!tfHasCatExports(cat))
		symes = tfCatExports(cat);

	else if (tfIsWith(cat))
		symes = tfGetCatParentsFrWith(cat);

	else if (tfIsIf(cat))
		symes = tfGetCatParentsFrIf(cat);

	else if (tfIsJoin(cat))
		symes = tfGetCatParentsFrJoin(cat);

	else if (tfIsMeet(cat))
		symes = tfGetCatExportsFrMeet(cat);

	else if (top)
		symes = tfGetThdParents(tfGetCategory(cat));

	else if (tfIsDeclare(cat))
		symes = listCopy(Syme)(tfSymes(cat));

	else
		symes = listCopy(Syme)(tfGetCatSelfSelf(cat));

	tfAuditExportList(symes);
	return symes;
}

/*
 * Called on a third-order type to get the symbol meanings which serve as
 * 'parents' of all of the exports of the third-order type.
 */
SymeList
tfGetThdParents(TForm thd)
{
	SymeList	symes;

	thd = tfDefineeType(thd);

	if (tfThdExports(thd) || tfIsUnknown(thd) || tfIsNone(thd))
		symes = listCopy(Syme)(tfThdExports(thd));

	else if (tfIsThird(thd) && tfUseThdExports(thd))
		symes = listCopy(Syme)(tfParents(thd));

	else if (tfIsThird(thd))
		symes = tfGetCatParents(tfThirdRestrictions(thd), false);

	else
		symes = listNil(Syme);

	tfAuditExportList(symes);
	return symes;
}

/*
 * tfGet...Parents helper functions.
 */

local SymeList
tfGetCatParentsFrWith(TForm cat)
{
	TForm		tfb = tfWithBase(cat);
	TForm		tfw = tfWithWithin(cat);
	SymeList	bsymes, wsymes;

	bsymes = tfGetCatParents(tfb, false);

	tfFollow(tfw);
	wsymes = tfGetCatParentsFrInner(tfw);

	return listNConcat(Syme)(bsymes, wsymes);
}

local SymeList
abGetCatParents(Sefo sefo)
{
	SymeList	xsymes, isymes, dsymes, csymes, symes;
	Length		i, argc;
	Sefo		*argv;
	Sefo		id;
	TForm		cat;

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	default:
		argc = 1;
		argv = &sefo;
		break;
	}

	xsymes = isymes = dsymes = listNil(Syme);
	for (i = 0; i < argc; i++) {
		id = abDefineeIdOrElse(argv[i], NULL);

		if (abTag(argv[i]) == AB_Nothing)
			continue;

		/* Defaults package. */
		else if (abTag(argv[i]) == AB_Default) {
			AbSyn	def = argv[i]->abDefault.body;
			symes = tfSymesFrDefault(def);
			dsymes = listNConcat(Syme)(symes, dsymes);
		}

		/* Explicit declaration. */
		else if (id && abTag(argv[i]) != AB_Id) {
			assert(abSyme(id));
			xsymes = listCons(Syme)(abSyme(id), xsymes);
		}
		/* Category expression. */
		else if (!tfIsUnknown(cat = abGetCategory(argv[i]))) {
			csymes = listCopy(Syme)(tfGetThdSelfSelf(cat));
			if (csymes == listNil(Syme))
				csymes = tfGetThdParents(cat);
			if (csymes == listNil(Syme))
				csymes = tfGetThdExports(cat);
			isymes = listNConcat(Syme)(isymes, csymes);
		}
	}

	/* Mark the symes which have a default implementation. */
	dsymes = listNReverse(Syme)(dsymes);
	for (; dsymes; dsymes = listFreeCons(Syme)(dsymes)) {
		Syme	dsyme = car(dsymes);
		Syme	xsyme = NULL;

		/* If the default is one of our exports, just mark it. */
		for (symes = xsymes; !xsyme && symes; symes = cdr(symes))
			if (symeEqual(car(symes), dsyme)) {
				xsyme = car(symes);
				symeSetDefault(xsyme);
			}

		/* If the default is inherited, use the default syme. */
		if (xsyme == NULL) {
			xsymes = listCons(Syme)(dsyme, xsymes);
			symeSetDefault(dsyme);
		}
	}

	xsymes = listNReverse(Syme)(xsymes);
	return listNConcat(Syme)(isymes, xsymes);
}

local SymeList
tfGetCatParentsFrIf(TForm cat)
{
	SymeList	tsymes, esymes, symes = listNil(Syme);
	Sefo	 	cond;

	if (tfParents(cat))
		return listCopy(Syme)(tfParents(cat));
	cond = tfIfCond(cat);

	if (abState(cond) != AB_State_HasUnique) {
		/* This is seriously not nice, as calling
		 * tiSefo will be expensive here
		 */
		tiTopFns()->tiSefo(stabFile(), cond);
	}

	tsymes = tfGetCatParentsFrInner(tfIfThen(cat));
	tsymes = listCopy(Syme)(tsymes);
	for (symes = tsymes; symes; symes=cdr(symes)) {
		car(symes) = symeCopy(car(symes));
		symeAddCondition(car(symes), cond, true);
	}

	esymes = tfGetCatParentsFrInner(tfIfElse(cat));
	esymes = listCopy(Syme)(esymes);
	symes = listNConcat(Syme)(tsymes, esymes);
	tfSetParents(cat, listCopy(Syme)(symes));
	return symes;
}

local SymeList
tfGetCatParentsFrJoin(TForm cat)
{
	SymeList	nsymes, symes = listNil(Syme);
	Length		i;

	for (i = 0; i < tfJoinArgc(cat); i += 1) {
		nsymes = tfGetCatParents(tfJoinArgN(cat, i), false);
		symes = listNConcat(Syme)(symes, nsymes);
	}

	return symes;
}

/*
 * This handles the case where the TForm could be
 * a declaration (or sequence of)
 */
local SymeList
tfGetCatParentsFrInner(TForm cat)
{
	SymeList symes;

	if (tfIsDeclare(cat))
		symes = listCopy(Syme)(tfSymes(cat));
	else if (tfHasExpr(cat))
		symes = abGetCatParents(tfGetExpr(cat));
	else
		symes = tfGetCatParents(cat, false);

	return symes;
}
/******************************************************************************
 *
 * :: Type form predicates to determine if the exports can be computed.
 *
 *****************************************************************************/

void
tfAuditExportList(SymeList symes)
{
	for (; symes; symes = cdr(symes))
		if (!symeIsExport(car(symes)) && !symeIsExtend(car(symes)))
			bug("tfAuditExportList");
}

local void
tfGetExportError(TForm tf, String order)
{
	if (DEBUG(tf)) {
		fprintf(dbOut, "No semantics for %s:  ", order);
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}
	/* "Implementation restriction:
	   import from %s before its exports could be determined." */
	/* comsgWarning(tfExpr(tf), ALDOR_W_TinEarlyImport, order); */
}

Bool
tfHasCatExports(TForm cat)
{
	return tfCatExportsPending(cat) == NULL;
}

Bool
tfHasThdExports(TForm thd)
{
	return tfThdExportsPending(thd) == NULL;
}

Bool
tfHasCatExportsFrWith(Sefo sefo)
{
	return tfCatExportsPendingFrWith(sefo) == NULL;
}

TForm
tfCatExportsPending(TForm cat)
{
	TForm	pending;

	cat = tfDefineeTypeSubst(cat);

	if (tfCatExports(cat))
		return NULL;

	if (tfIsUnknown(cat) || tfIsNone(cat))
		return NULL;

	if (!tfIsMeaning(cat)) {
		assert(tfIsPending(cat));
		return cat;
	}

	if (tfIsWith(cat)) {
		TForm		tfb = tfWithBase(cat);
		TForm		tfw = tfWithWithin(cat);

		pending = tfCatExportsPending(tfb);
		if (pending) return pending;

		tfFollow(tfw);
		if (tfCatExports(tfw))
			pending = NULL;
		else if (tfIsDeclare(tfw))
			pending = (tfSymes(tfw) ? NULL : tfUnknown);
		else if (tfHasExpr(tfw))
			pending = tfCatExportsPendingFrWith(tfGetExpr(tfw));
		else
			pending = tfCatExportsPending(tfw);
			
		if (pending && tfIsUnknown(pending)) {
			assert(tfIsPending(tfw));
			return tfw;
		}
		if (pending) return pending;

		return NULL;
	}

	else if (tfIsIf(cat)) {
		pending = tfCatExportsPending(tfIfThen(cat));
		if (pending) return pending;

		pending = tfCatExportsPending(tfIfElse(cat));
		if (pending) return pending;

		return NULL;
	}

	else if (tfIsJoin(cat) || tfIsMeet(cat)) {
		Length	i, argc = tfArgc(cat);

		for (i = 0; i < argc; i += 1) {
			pending = tfCatExportsPending(tfFollowArg(cat, i));
			if (pending) return pending;
		}

		return NULL;
	}

	else
		return tfThdExportsPending(tfGetCategory(cat));
}

local TForm
tfCatExportsPendingFrWith(Sefo sefo)
{
	Length		i, argc;
	Sefo		*argv;
	TForm		pending;

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	default:
		argc = 1;
		argv = &sefo;
		break;
	}

	for (i = 0; i < argc; i += 1) {
		AbSyn	id = abDefineeIdOrElse(argv[i], NULL);

		if (abTag(argv[i]) == AB_Nothing)
			continue;

		/* Defaults package. */
		else if (abTag(argv[i]) == AB_Default) {
			AbSyn	def = argv[i]->abDefault.body;
			if (!tfHasSymesFrDefault(def))
				return tfUnknown;
		}

		/* Explicit declaration. */
		else if (id && abTag(argv[i]) != AB_Id) {
			if (!abSyme(id))
				return tfUnknown;
		}

		/* Category expression. */
		else if (abState(argv[i]) == AB_State_HasUnique &&
			 tfSatCat(abTUnique(argv[i]))) {
			pending = tfThdExportsPending(abTUnique(argv[i]));
			if (pending) return pending;
		}
	}

	return NULL;
}

local Bool
tfHasSymesFrDefault(Sefo sefo)
{
	Length		i, argc;
	Sefo		*argv;
	Sefo		id;

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_If:
		argc = 2;
		argv = &sefo->abIf.thenAlt;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	default:
		id = abDefineeIdOrElse(sefo, NULL);
		if (id && !abSyme(id)) return false;
		return true;
	}

	for (i = 0; i < argc; i += 1) {
		if (!tfHasSymesFrDefault(argv[i]))
			return false;
	}

	return true;
}


#define tfTUniqueOrZero(tf) (tfHasExpr(tf) ? abTUnique(tfGetExpr(tf)) : NULL)

TForm
tfThdExportsPending(TForm thd)
{
        TForm inthd = thd;
  /* in bad case we come in here with a Define or a Declare (in the case of == Bug) */
        thd = tfDefineeTypeSubst(thd);
	if (tfThdExports(thd))
		return NULL;

	if (tfIsUnknown(thd) || tfIsNone(thd))
		return NULL;

	if (!tfIsMeaning(thd)) {
		assert(tfIsPending(thd));
		return thd;
	}

	if (tfIsThird(thd)) {
	  /* in bad case thd is now a Third whose argv[0] is the rhs of the category */
	  if tfIsJoin(tfThirdRestrictions(thd)) {
	  /* if it is a Join, thd->argv[0]->argv[0..] are the arguments argn of the Join
	     and argn->__absyn->abHdr.type.unique would be their Defines
	     We will loop indefinitely if one of these Defines is the original.
	  */
	    Length i, all;
	    TForm candidate;
	    all = tfJoinArgc(tfThirdRestrictions(thd));
	    for (i=0 ; i < all ; i+=1) {
	      candidate = tfTUniqueOrZero(tfJoinArgv(tfThirdRestrictions(thd))[i]);
	      if (candidate == inthd) { 
		fprintf(stderr,"Oops - circular category definition\n");
		fflush(stderr);
		abPrintDb(inthd->__absyn);
		exit(1);
	      }
	    } /* for */
	    return tfCatExportsPending(tfThirdRestrictions(thd));

	  } /* if Join */
	  else if tfIsWith(tfThirdRestrictions(thd)){
	  /* if it is a With, thd->argv[0]->argv[1..] are the arguments argn of the With
	     and argn->__absyn->abHdr.type.unique would be their Defines
	     We will loop indefinitely if one of these Defines is the original.
	  */
	    Length i;
	    TForm candidate;
	    for (i=0 ; i < tfArgc(tfThirdRestrictions(thd)) ; i+=1) {
	      candidate = tfTUniqueOrZero(tfArgv(tfThirdRestrictions(thd))[i]);
	      if (candidate == inthd) { 
		fprintf(stderr,"Oops - circular category definition\n");
		fflush(stderr);
		abPrintDb(inthd->__absyn);
		exit(1);
	      }
	    }
	  }
	  else if tfIsGeneral(tfThirdRestrictions(thd)){
	    /* if it is a General we need to check circularity in case inthd is a Declare
	     */
	    if ( tfIsDeclare(inthd) && 
		 (tfTUniqueOrZero(tfThirdRestrictions(thd))->argv[0] == inthd) && 
		 (tfTUniqueOrZero(tfThirdRestrictions(thd))->argv[1] == thd->argv[0])) {
		fprintf(stderr,"Oops - circular category definition\n");
		fflush(stderr);
		abPrintDb(inthd->__absyn);
		exit(1);

	       }
	  }
	  return tfCatExportsPending(tfThirdRestrictions(thd));
	}

	else
		return NULL;
}

/******************************************************************************
 *
 * :: Type form exported symes.
 *
 *****************************************************************************/

/* Find the syme in symes corresponding to syme.  Return NULL on failure. */ 
local Syme
symeListFindExport(SymeList mods, Syme syme1, SymeList symes)
{
	Syme		syme = NULL;

	for (; !syme && symes; symes = cdr(symes)) {
		Syme	syme2 = car(symes);

		if (symeEqualModConditions(mods, syme1, syme2))
			syme = syme2;
	}

	return syme;
}

/* Incorporate default/constant number/condition info from syme2 into syme1. */
local void
tfJoinExports(Syme syme1, Syme syme2)
{
	if (!symeHasDefault(syme1) && symeHasDefault(syme2)) {
		symeSetDefault(syme1);
		symeTransferImplInfo(syme1, syme2);
	}

	if (symeCondition(syme1) &&
	    !symeCondition(syme2))
		symeSetCondition(syme1, listNil(Sefo));
}

local Bool
tfJoinExportToList(SymeList mods, SymeList symes, Syme syme2, Sefo cond)
{
	Bool	merge = false;

	for (; !merge && symes; symes = cdr(symes)) {
		Syme	syme1 = car(symes);

		/* Decide if the two meanings can be merged into one. */
		if (!symeEqualModConditions(mods, syme1, syme2))
			merge = false;

		else if (!symeCondition(syme1))
			merge = true;

		else if (symeCondition(syme2))
			merge = sefoListEqualMod(mods, symeCondition(syme1),
						 symeCondition(syme2));
		else
			merge = (cond == NULL);

		if (merge) {
			tfJoinExports(syme1, syme2);
			symeAddTwin(syme1, syme2);
		}
	}

	return merge;
}

SymeList
tfJoinExportLists(SymeList mods, SymeList symes1, SymeList symes2, Sefo cond)
{
	SymeList	result = symes1, next;

	for (; symes2; symes2 = cdr(symes2)) {
		Syme	syme2 = car(symes2);

		if (!tfJoinExportToList(mods, result, syme2, cond)) {
			Syme	syme1 = symeCopy(syme2);
			if (cond) symeAddCondition(syme1, cond, true);
			next = listCons(Syme)(syme1, listNil(Syme));
			result = listNConcat(Syme)(result, next);
			symeAddTwin(syme1, syme2);
		}
	}

	return result;
}

local SymeList
tfMeetExportLists(SymeList mods, SymeList symes1, SymeList symes2, Sefo cond)
{
	SymeList	result = listNil(Syme);

	for (; symes1; symes1 = cdr(symes1)) {
		Syme	syme1 = car(symes1);
		Syme	syme2 = symeListFindExport(mods, syme1, symes2);

		if (syme2) {
			/*!! Combine default/const num./condition info. */;
			if (cond) symeAddCondition(syme1, cond, true);
			result = listCons(Syme)(syme1, result);
		}
	}

	result = listNReverse(Syme)(result);
	return result;
}

local SymeList
tfAddDomExports(TForm tf, SymeList symes)
{
	SymeList	nsymes;
	SymeList	mods = tfGetDomSelf(tf);

	nsymes = tfJoinExportLists(mods, tfDomExports(tf), symes, NULL);

	return tfSetDomExports(tf, nsymes);
}

local SymeList
tfAddCatExports(TForm tf, SymeList symes)
{
	SymeList	nsymes;
	SymeList	mods = tfGetCatSelf(tf);

	nsymes = tfJoinExportLists(mods, tfCatExports(tf), symes, NULL);

	return tfSetCatExports(tf, nsymes);
}

local SymeList
tfAddThdExports(TForm tf, SymeList symes)
{
	SymeList	nsymes;
	SymeList	mods = tfGetThdSelf(tf);

	nsymes = tfJoinExportLists(mods, tfThdExports(tf), symes, NULL);

	return tfSetThdExports(tf, nsymes);
}

local SymeList
tfAddHasExports(TForm tf, TForm cat)
{
	Sefo		cond = NULL;
	SymeList	nsymes;
	SymeList	mods = tfGetDomSelf(tf);

	cat = tfDefineeType(cat);
	if (tfHasExpr(tf) && tfHasExpr(cat))
		cond = abNewHas(sposNone, tfGetExpr(tf), tfGetExpr(cat));

	nsymes = tfGetCatExports(cat);
	nsymes = tfJoinExportLists(mods, tfDomExports(tf), nsymes, cond);

	return tfSetDomExports(tf, nsymes);
}

/*
 * tfMangleSymes(tf, cat, exports, symes) takes the type "tf",
 * its category "cat", a list of category exports "exports"
 * from the category of "cat" and a list of domain exports
 * "symes" from the add body of "cat". The implementation
 * details from "symes" are transferred into "exports".
 *
 * Note that the symes in "exports" are modified in-place.
 */
SymeList
tfMangleSymes(TForm tf, TForm cat, SymeList esymes, SymeList symes)
{
	SymeList	sl, exports = esymes;
	SymeList	mods = tfGetDomSelf(tf);


	/* Stop if we have nothing to do */
	if (!symes || !exports) return esymes;


	/* Add in self from the category (??? needed ???) */
	mods = listNConcat(Syme)(mods, listCopy(Syme)(tfGetCatSelf(cat)));


	/* Shift defnNum and constNum from symes into tfDomExports(tf) */
	for (;exports; exports = cdr(exports))
	{
		Syme	syme = car(exports);
		TForm	stf  = symeType(syme);
		int	cnum = symeConstNum(syme);


		/* Skip symes that aren't exports */
		if (!symeIsExport(syme))
			continue;


		/* Skip symes that are okay */
		if (cnum != SYME_NUMBER_UNASSIGNED)
			continue;


		/* Search for this syme in symes */
		for (sl = symes;sl;sl = cdr(sl))
		{
			Syme	csyme = car(sl);
			TForm	cstf  = symeType(csyme);
			int	ccnum = symeConstNum(csyme);
			int	cdnum = symeDefnNum(csyme);


			/* Skip symes that aren't exports */
			if (!symeIsExport(syme))
				continue;


			/* Skip symes with no defn or const number */
			if (!cdnum && (ccnum == SYME_NUMBER_UNASSIGNED))
				continue;


			/* Do the names match? */
			if (symeId(syme) != symeId(csyme))
				continue;


			/* Do the types match? */
			if (!tformEqualMod(mods, stf, cstf))
				continue;


			/* DEBUGGING */
			symeRefreshDEBUG(dbOut, "\t* %d --> %d, %d --> %d [%s]\n",
					 symeDefnNum(syme),
					 symeDefnNum(csyme),
					 symeConstNum(syme),
					 symeConstNum(csyme),
					 symePretty(syme));


			/* Transfer implementation details */
			symeTransferImplInfo(syme, csyme);
		}
	}


	/* Return a copy of the modified symes */
	return esymes;
}


/*
 * Called on a domain to get the symbol meanings which are
 * exported from the domain.
 */
SymeList
tfGetDomExports(TForm tf)
{
	TFormList	hl;

	tf = tfDefineeType(tf);

	tf = tfIgnoreExceptions(tf);

	if (tfDomExports(tf) || tfIsUnknown(tf) || tfIsNone(tf))
		return tfDomExports(tf);

	if (!tfIsMeaning(tf)) {
		tfGetExportError(tf, "domain");
		return tfDomExports(tf);
	}

	if (DEBUG(tfImport)) {
		fprintf(dbOut, "(tfGetDomExports:  from ");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	/*
	 * Collect the domain exports from the type form.
	 */
	if (tfIsNotDomain(tf))
		tfSetDomExports(tf, listNil(Syme));

	else if (tfIsAdd(tf))
		tfSetDomExports(tf, tfSymes(tf));

	else if (tfIsSym(tf) || tfIsSyntax(tf))
		tfSetDomExports(tf, listNil(Syme));

	else if (tfIsLibrary(tf))
		tfSetDomExports(tf, libGetSymes(tfLibraryLib(tf)));

	else if (tfIsArchive(tf))
		tfSetDomExports(tf, arGetSymes(tfArchiveAr(tf)));

	else {
		SymeList exps, vexps;
		TForm cat, val = (TForm)NULL;
		if (tfIsEnum(tf) || tfIsRecord(tf) || tfIsRawRecord(tf) ||
		    tfIsTrailingArray(tf) || tfIsUnion(tf))
			tfSetDomExports(tf, listCopy(Syme)(tfSymes(tf)));
		tfGetDomSelf(tf);
		tfGetDomCascades(tf);
		cat = tfGetCategory(tf);
		tfFollow(cat);

		assert(tfIsDefine(cat) ? (tfDefineVal(cat) != tf) : true);

		exps = tfGetCatExports(cat);
		if (tfIsDefine(cat))
			val = tfDefineVal(cat);

		if (DEBUG(symeRefresh)) {
			if (val && !tfIsAdd(val)) {
				(void)fprintf(dbOut, "\n-------- (not add)\n");
				(void)tfPrintDb(val);
			}
		}

		if (val && tfIsAdd(val)) {
			vexps = tfGetDomExports(val);
			exps = tfMangleSymes(tf, cat, exps, vexps);
		}
		tfAddDomExports(tf, exps);
	}

	/*
	 * Collect the conditional exports from the has questions.
	 */
	for (hl = tfQueries(tf); hl; hl = cdr(hl))
		tfAddHasExports(tf, car(hl));

	tfImportDEBUG(dbOut, ")\n");

	tfAuditExportList(tfDomExports(tf));
	return tfDomExports(tf);
}

/*
 * Called on a category to get the symbol meanings which are
 * exported by domains of this category.
 */
SymeList
tfGetCatExports(TForm cat)
{
	tfFollow(cat);
	if (tfIsDefineOfType(cat))
		return tfGetDomExports(tfDefineVal(cat));

	cat = tfDefineeType(cat);

	if (tfCatExports(cat) || tfIsUnknown(cat) || tfIsNone(cat) ||
	    !tfHasCatExports(cat))
		return tfCatExports(cat);

	if (!tfIsMeaning(cat)) {
		tfGetExportError(cat, "category");
		return tfCatExports(cat);
	}

	if (DEBUG(tfCat)) {
		fprintf(dbOut, "(tfGetCatExports:  from ");
		tfPrint(dbOut, cat);
	}

	tfGetCatSelf(cat);
	tfGetCatCascades(cat);

	if (tfIsWith(cat) && tfUseCatExports(cat))
		tfAddCatExports(cat,tfGetCatExportsFrParents(tfParents(cat)));

	else if (tfIsWith(cat))
		tfGetCatExportsFrWith(cat);

	else if (tfIsIf(cat))
		tfGetCatExportsFrIf(cat);

	else if (tfIsJoin(cat))
		tfGetCatExportsFrJoin(cat);

	else if (tfIsMeet(cat))
		tfGetCatExportsFrMeet(cat);

	else
		tfAddCatExports(cat, tfGetThdExports(tfGetCategory(cat)));

	tfCatDEBUG(dbOut, ")\n");

	tfAuditExportList(tfCatExports(cat));
	return tfCatExports(cat);
}

/*
 * Called on the type of a category to get the symbol meanings which are
 * exported by domains of this category.
 */
SymeList
tfGetThdExports(TForm thd)
{
	thd = tfDefineeType(thd);

	if (tfThdExports(thd) || tfIsUnknown(thd) || tfIsNone(thd) ||
	    !tfHasThdExports(thd))
		return tfThdExports(thd);

	if (!tfIsMeaning(thd)) {
		tfGetExportError(thd, "third-order type");
		return tfThdExports(thd);
	}

	if (DEBUG(tfCat)) {
		fprintf(dbOut, "(tfGetThdExports:  from ");
		tfPrint(dbOut, thd);
		fnewline(dbOut);
	}

	tfGetThdSelf(thd);
	tfGetThdCascades(thd);

	if (tfIsThird(thd) && tfUseThdExports(thd)) {
		tfAddThdExports(thd,tfGetCatExportsFrParents(tfParents(thd)));
		tfAddThdExports(thd,tfSymes(thd));
	}

	else if (tfIsThird(thd)) {
		tfAddThdExports(thd,tfGetCatExports(tfThirdRestrictions(thd)));
		tfAddThdExports(thd,tfSymes(thd));
	}

	tfCatDEBUG(dbOut, ")\n");

	tfAuditExportList(tfThdExports(thd));
	return tfThdExports(thd);
}

/*
 * tfGet...Exports helper functions.
 */

local SymeList
tfGetCatExportsFrParents(SymeList symes)
{
	SymeList	nsymes, osymes = listCopy(Syme)(symes);
	SymeList	queue = listCopy(Syme)(symes);
	SymeList	xsymes = listNil(Syme);
	SefoList	cond;

	while (queue) {
		Syme		syme = car(queue);
		SymeList	cell = queue;
		queue = cdr(queue);

		/* Move syme to xsymes. */
		setcdr(cell, xsymes);
		xsymes = cell;

		if (!symeIsSelfSelf(syme)) continue;

		if (DEBUG(tfParent)) {
			afprintf(dbOut, "(tfCatExports: expanding %pTForm %pAbSynList\n",
				 symeType(syme), symeCondition(syme));
		}

		nsymes = tfGetCatParents(symeType(syme), true);
		cond = symeCondition(syme);
		if (cond) nsymes = tfGetCatExportsCond(nsymes, cond, true);

		if (DEBUG(tfParent)) {
			afprintf(dbOut, "tfCatExports: into %pSymeList)\n", nsymes);
		}

		nsymes = tfGetCatExportsFilter(osymes, nsymes);
		osymes = listNConcat(Syme)(osymes, nsymes);
		queue = listNConcat(Syme)(listCopy(Syme)(nsymes), queue);
	}
	listFree(Syme)(osymes);

	return listNReverse(Syme)(xsymes);
}

local SymeList
tfGetCatExportsCond(SymeList symes0, SefoList conds0, Bool pos)
{
	SymeList	symes, nsymes = listNil(Syme);
	SefoList	conds;
	SefoList reversedConds0 = listReverse(Sefo)(conds0);

	/* Reverse conditions so that any dependency in evaluation is preserved.
	 * For example S has Ring and X has Algebra S
	 */
	for (symes = symes0; symes; symes = cdr(symes)) {
		Syme nsyme = symeCopy(car(symes));
		for (conds = reversedConds0; conds; conds = cdr(conds)) {
			symeAddCondition(nsyme, car(conds), pos);
		}
		nsymes = listCons(Syme)(nsyme, nsymes);
	}
	listFree(Sefo)(reversedConds0);

	return listNReverse(Syme)(nsymes);
}

local SymeList
tfGetCatExportsFilter(SymeList osymes, SymeList nsymes)
{
	SymeList	symes, rsymes = listNil(Syme);

	/* Remove symes for %% which have been seen before. */
	for (symes = nsymes; symes; symes = cdr(symes))
		if (!(symeIsSelfSelf(car(symes)) &&
		      symeListMember(car(symes), osymes, symeEqual)))
			rsymes = listCons(Syme)(car(symes), rsymes);

	listFree(Syme)(nsymes);
	return listNReverse(Syme)(rsymes);
}

local SymeList
tfGetCatExportsFrWith(TForm cat)
{
	TForm		tfb = tfWithBase(cat);
	TForm		tfw = tfWithWithin(cat);
	SymeList	bsymes, wsymes;

	bsymes = tfGetCatExports(tfb);

	tfFollow(tfw);
	if (tfCatExports(tfw))
		wsymes = tfCatExports(tfw);

	else if (tfIsDeclare(tfw))
		wsymes = tfSymes(tfw);

	else if (tfHasExpr(tfw))
		wsymes = tfGetCatImportsFrWith(tfGetExpr(tfw), bsymes);

	else
		wsymes = tfGetCatExports(tfw);

	tfSetCatExports(tfw, wsymes);

	tfAddCatExports(cat, bsymes);
	tfAddCatExports(cat, wsymes);

	return tfCatExports(cat);
}

local SymeList
tfGetCatExportsFrIf(TForm cat)
{
	SymeList	tsymes, esymes, symes = listNil(Syme);
	SymeList	mods = tfGetCatSelf(cat);
	Sefo		cond;

	tfFollow(cat->argv[0]);
	
	if (tfNeedsSefo(cat->argv[0])) 
		tiTopFns()->tiTfSefo(stabFile(), cat->argv[0]);

	cond = tfIfCond(cat);
	tsymes = tfGetCatExports(tfIfThen(cat));
	symes = tfJoinExportLists(mods, symes, tsymes, cond);

	cond = abNewNot(sposNone, cond);
	esymes = tfGetCatExports(tfIfElse(cat));
	symes = tfJoinExportLists(mods, symes, esymes, cond);

	return tfSetCatExports(cat, symes);
}

local SymeList
tfGetCatExportsFrJoin(TForm cat)
{
	SymeList	nsymes, symes = tfCatExports(cat);
	SymeList	mods = tfGetCatSelf(cat);
	Length		i;

	for (i = 0; i < tfJoinArgc(cat); i += 1) {
		nsymes = tfGetCatExports(tfJoinArgN(cat, i));
		symes = tfJoinExportLists(mods, symes, nsymes, NULL);
	}

	return tfSetCatExports(cat, symes);
}

local SymeList
tfGetCatExportsFrMeet(TForm cat)
{
	SymeList	nsymes, symes = tfCatExports(cat);
	SymeList	mods = tfGetCatSelf(cat);
	Length		i;

	nsymes = tfGetCatExports(tfMeetArgN(cat, int0));
	symes = tfJoinExportLists(mods, symes, nsymes, NULL);
	for (i = 1; i < tfMeetArgc(cat); i += 1) {
		nsymes = tfGetCatExports(tfMeetArgN(cat, i));
		symes = tfMeetExportLists(mods, symes, nsymes, NULL);
	}

	return tfSetCatExports(cat, symes);
}

/******************************************************************************
 *
 * :: Type form imported symes.
 *
 *****************************************************************************/

Syme
tfHasDomExportMod(TForm tf, SymeList mods, Symbol sym, TForm type)
{
	SymeList	sl;

	for (sl = tfGetDomExports(tf); sl; sl = cdr(sl)) {
		Syme syme = car(sl);
		if (symeId(syme) != sym) continue;
		if (!tformEqualMod(mods, symeType(syme), type)) continue;
		return syme;
	}
	return NULL;
}

Syme
tfHasDomImport(TForm tf, Symbol sym, TForm type)
{
	SymeList	sl;

	for (sl = tfGetDomImports(tf); sl; sl = cdr(sl)) {
		Syme syme = car(sl);
		if (symeId(syme) == sym && tformEqual(symeType(syme), type))
			return syme;
	}
	return NULL;
}

local Syme
tfGetBuiltinSyme(TForm tf, Symbol sym)
{
	SymeList	symes;
	Syme		syme0 = NULL;

	assert(tfDomImports(tf));
	for (symes = tfDomImports(tf); symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		if (symeId(syme) == sym)
			syme0 = syme;
	}
	return syme0;
}

/*
 * Called on a domain to get the symbol meanings to include
 * in the current scope. Use this in preference to the older
 * tfGetDomImports() to reduce the chance of polluting the
 * top-level stab for the current file.
 */
SymeList
tfStabGetDomImports(Stab stab, TForm tf)
{
	SymeList	xsymes, symes;

	tf = tfDefineeType(tf);
	
	tf = tfIgnoreExceptions(tf);

	if (tfDomImports(tf))
		return tfDomImports(tf);

	if (DEBUG(tfImport)) {
		fprintf(dbOut, "(tfStabGetDomImports:  from ");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	xsymes = tfGetDomExports(tf);

	symes = symeListSubstSelf(stab, tf, xsymes);

	if (tfConditions(tf) != NULL) {
		Syme syme;
		SymeList sl = symes;
		while (sl != listNil(Syme)) {
			Syme syme = car(sl);
			TForm symeTf = symeType(syme);
			tfDEBUG(dbOut, "Setting imported condition %s %pTForm\n", 
				symeString(syme), symeTf);
			tfSetConditions(symeTf, tfConditions(tf));
			symeSetConditionContext(syme, tfConditionalAbSyn(tf));
			sl = cdr(sl);
		}
	
	}

	symes = symeListCheckCondition(symes);

	tfSetDomImports(tf, symes);

	if (tfIsBasicLib(tf))
		tfInitBasicTypes(tf);

	if (DEBUG(tfImport)) {
		symeListPrintDb(symes);
		fprintf(dbOut, ")\n");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	return symes;
}

/*
 * Called on a domain to get the symbol meanings to include
 * in the current scope. This may cause tforms to be imported
 * into stabFile() which can cause grief (e.g. bug1192) so
 * use tfStabGetDomImports() if you have a localised stab.
 */
SymeList
tfGetDomImports(TForm tf)
{
	/*
	 * This use of stabFile() is extremely unfortunate because it
	 * associates the tform with the top-level of the file being
	 * compiled. This allows inner symbols to escape their stab
	 * levels and jump directly to the top.
	 */
	return tfStabGetDomImports(stabFile(), tf);
}


SymeList
tfGetCatImportsFrWith(Sefo sefo, SymeList bsymes)
{
	SymeList	xsymes, isymes, dsymes, csymes, symes;
	Length		i, argc;
	Sefo		*argv;
	Sefo		id;
	TForm		cat;

	/*!! assert(tfHasCatExportsFrWith(sefo)); */

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	default:
		argc = 1;
		argv = &sefo;
		break;
	}

	xsymes = isymes = dsymes = listNil(Syme);
	for (i = 0; i < argc; i++) {
		id = abDefineeIdOrElse(argv[i], NULL);

		if (abTag(argv[i]) == AB_Nothing)
			continue;

		/* Defaults package. */
		else if (abTag(argv[i]) == AB_Default) {
			AbSyn	def = argv[i]->abDefault.body;
			symes = tfSymesFrDefault(def);
			dsymes = listNConcat(Syme)(symes, dsymes);
		}

		/* Explicit declaration. */
		else if (id && abTag(argv[i]) != AB_Id) {
			assert(abSyme(id));
			if (symeIsExport(abSyme(id)))
				xsymes = listCons(Syme)(abSyme(id), xsymes);
		}

		/* Category expression. */
		else if (!tfIsUnknown(cat = abGetCategory(argv[i])) &&
			 tfSatCat(cat)) {
			csymes = tfGetThdExports(cat);
			isymes = symeListUnion(isymes, csymes, symeEqual);
		}
	}

	/* Mark the symes which have a default implementation. */
	dsymes = listNReverse(Syme)(dsymes);
	for (; dsymes; dsymes = listFreeCons(Syme)(dsymes)) {
		Syme	dsyme = car(dsymes);
		Syme	xsyme = NULL;

		/* If the default is one of our exports, just mark it. */
		for (symes = xsymes; !xsyme && symes; symes = cdr(symes))
			if (symeEqual(car(symes), dsyme)) {
				xsyme = car(symes);
				symeSetDefault(xsyme);
			}

		/* If the default is inherited, use the default syme. */
		if (xsyme == NULL) {
			xsymes = listCons(Syme)(dsyme, xsymes);
			symeSetDefault(dsyme);
		}
	}

	xsymes = listNReverse(Syme)(xsymes);
	return symeListUnion(isymes, xsymes, symeEqual);
}

/******************************************************************************
 *
 * :: Type form constant definitions.
 *
 *****************************************************************************/

SymeList
tfGetDomConstants(TForm tf)
{
	tf = tfDefineeType(tf);

	return tfGetCatConstants(tfGetCategory(tf));
}

SymeList
tfGetCatConstants(TForm cat)
{
	SymeList	symes;

	tfFollow(cat);
	if (tfIsDefine(cat)) {
		if (tfIsDefineOfType(cat))
			return tfGetDomConstants(tfDefineVal(cat));
		cat = tfDefineVal(cat);
		tfFollow(cat);
		return tfSymes(cat);
	}

	cat = tfDefineeType(cat);

	if (tfIsUnknown(cat) || tfIsNone(cat))
		symes = listNil(Syme);

	else if (tfIsWith(cat)) {
		TForm		tfb = tfWithBase(cat);
		TForm		tfw = tfWithWithin(cat);
		SymeList	bsymes, wsymes;

		bsymes = tfGetCatConstants(tfb);

		/*!! tfCatExports will find the constant numbers from any
		 *!! defaults in the with, but not from any categories.
		 *!! We could save the constant symes in the tform.
		 */
		tfFollow(tfw);
		if (tfCatExports(tfw))
			wsymes = tfCatExports(tfw);

		else if (tfIsDeclare(tfw))
			wsymes = tfSymes(tfw);

		else if (tfHasExpr(tfw))
			wsymes = tfGetCatConstantsFrWith(tfGetExpr(tfw));

		else
			wsymes = tfGetCatConstants(tfw);

		symes = symeListUnion(bsymes, wsymes, symeEq);
	}

	else if (tfIsIf(cat))
		symes = listNConcat(Syme)(tfGetCatConstants(tfIfThen(cat)),
					  tfGetCatConstants(tfIfElse(cat)));

	else if (tfIsJoin(cat) || tfIsMeet(cat)) {
		Length		i, argc = tfArgc(cat);
		SymeList	nsymes;

		symes = listNil(Syme);
		for (i = 0; i < argc; i += 1) {
			nsymes = tfGetCatConstants(tfFollowArg(cat, i));
			symes = symeListUnion(symes, nsymes, symeEq);
		}
	}

	else
		symes = tfGetThdConstants(tfGetCategory(cat));

	return symes;
}

local SymeList
tfGetThdConstants(TForm thd)
{
	SymeList	symes = listNil(Syme);

	thd = tfDefineeType(thd);

	if (tfIsThird(thd))
		symes = tfGetCatConstants(tfThirdRestrictions(thd));

	return symes;
}

/*
 * tfGetCatConstants helper functions.
 */

local SymeList
tfGetCatConstantsFrWith(Sefo sefo)
{
	SymeList	csymes, symes = listNil(Syme);
	Length		i, argc;
	Sefo		*argv;

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	default:
		argc = 1;
		argv = &sefo;
		break;
	}

	for (i = 0; i < argc; i++) {
		Sefo	id = abDefineeIdOrElse(argv[i], NULL);

		if (abTag(argv[i]) == AB_Nothing)
			continue;

		/* Defaults package. */
		else if (abTag(argv[i]) == AB_Default)
			continue;

		/* Explicit declaration. */
		else if (id && abTag(argv[i]) != AB_Id)
			continue;

		/* Category expression. */
		else if (abState(argv[i]) == AB_State_HasUnique &&
			 tfSatCat(abTUnique(argv[i]))) {
			csymes = tfGetThdConstants(abTUnique(argv[i]));
			symes = listNConcat(Syme)(csymes, symes);
		}
	}

	return symes;
}

/******************************************************************************
 *
 * :: Type form cascaded imports.
 *
 *****************************************************************************/

/*
 * Called on a domain to get the cascaded imports which are supplied
 * by the domain.
 */
TQualList
tfGetDomCascades(TForm tf)
{
	TForm		cat;
	TQualList	ql;

	tf = tfDefineeType(tf);

	if (tfHasCascades(tf))
		return tfCascades(tf);

	if (tfIsUnknown(tf) || tfIsNone(tf))
		return listNil(TQual);

	if (!tfIsMeaning(tf)) {
		tfGetExportError(tf, "domain");
		return listNil(TQual);
	}

	cat = tfGetCategory(tf);
	if (tfIsUnknown(cat))
		return listNil(TQual);
	if (!tfHasCatExports(cat))
		return listNil(TQual);

	if (DEBUG(tfCascade)) {
		fprintf(dbOut, "tfGetDomCascades:  from ");
		tfPrint(dbOut, tf);
		findent += 2;
		fnewline(dbOut);
	}

	/*
	 * Collect the domain cascades from the type form.
	 */
	if (tfIsNotDomain(tf))
		ql = listNil(TQual);

	else if (tfIsRecord(tf) || tfIsRawRecord(tf) || tfIsUnion(tf))
		ql = tqListFrArgs(tfGetStab(tf), tfArgv(tf), tfArgc(tf));
	else if (tfIsTrailingArray(tf))
		ql = tfGetCascadesFrTrailingArray(tf);
	else if (tfIsSym(tf) || tfIsSyntax(tf) || tfIsAdd(tf))
		ql = listNil(TQual);

	/*!! Perhaps this can be expanded later. */
	else if (tfIsLibrary(tf) || tfIsArchive(tf))
		ql = listNil(TQual);
	else
		ql = listCopy(TQual)(tfGetCatCascades(tfGetCategory(tf)));

	if (DEBUG(tfCascade)) {
		fprintf(dbOut, "domain cascades:");
		fnewline(dbOut);
		listPrint(TQual)(dbOut, ql, tqPrint);
		findent -= 2;
		fnewline(dbOut);
	}

	tfHasCascades(tf) = true;
	tfSetCascades(tf, ql);
	return ql;
}

/*
 * Called on a category to get the cascaded imports which are supplied
 * by the category.
 */
TQualList
tfGetCatCascades(TForm cat)
{
	TQualList	ql;

	tfFollow(cat);

	if (tfIsDefine(cat)) {
		TQualList	ql2;
		
		if (tfHasCascades(cat) || !tfIsMeaning(cat))
			return tfCascades(cat);
		
		ql = listCopy(TQual)
			(tfGetDomCascades(tfDefineVal(cat)));

		ql2 = listCopy(TQual)
			(tfGetCatCascades(tfDefineeType(cat)));
		ql = listNConcat(TQual)(ql, ql2);

		tfHasCascades(cat) = true;
		tfSetCascades(cat, ql);
		return ql;
	}

	cat = tfDefineeType(cat);

	if (tfHasCascades(cat))
		return tfCascades(cat);

	if (tfIsUnknown(cat) || tfIsNone(cat))
		return listNil(TQual);

	if (!tfIsMeaning(cat)) {
		tfGetExportError(cat, "category");
		return listNil(TQual);
	}

	if (DEBUG(tfCat)) {
		fprintf(dbOut, "(tfGetCatCascades:  from ");
		tfPrint(dbOut, cat);
		fnewline(dbOut);
	}

	if (tfIsWith(cat))
		ql = tfGetCatCascadesFrWith(cat);

	else if (tfIsIf(cat)) {
		TQualList	ql1, ql2;

		ql1 = listCopy(TQual)(tfGetCatCascades(tfIfThen(cat)));
		ql2 = listCopy(TQual)(tfGetCatCascades(tfIfElse(cat)));
		ql = listNConcat(TQual)(ql1, ql2);
	}

	else if (tfIsJoin(cat) || tfIsMeet(cat)) {
		Length		i, argc = tfArgc(cat);
		TQualList	nql;

		ql = listNil(TQual);
		for (i = 0; i < argc; i += 1) {
			nql = tfGetCatCascades(tfFollowArg(cat, i));
			ql = listNConcat(TQual)(ql, listCopy(TQual)(nql));
		}
	}

	else
		ql = listCopy(TQual)(tfGetThdCascades(tfGetCategory(cat)));

	tfCatDEBUG(dbOut, ")\n");

	tfHasCascades(cat) = true;
	tfSetCascades(cat, ql);
	return ql;
}

/*
 * Called on a third-order type to get the cascaded imports which are supplied
 * by the third-order type.
 */
TQualList
tfGetThdCascades(TForm thd)
{
	TQualList	ql;

	thd = tfDefineeType(thd);

	if (tfHasCascades(thd))
		return tfCascades(thd);

	if (tfIsUnknown(thd) || tfIsNone(thd))
		return listNil(TQual);

	if (DEBUG(tfCat)) {
		fprintf(dbOut, "(tfGetThdCascades:  from ");
		tfPrint(dbOut, thd);
		fnewline(dbOut);
	}

	if (tfIsThird(thd)) {
		ql = tfGetCatCascades(tfThirdRestrictions(thd));
		ql = listCopy(TQual)(ql);
	}

	else
		ql = listNil(TQual);

	tfCatDEBUG(dbOut, ")\n");

	tfHasCascades(thd) = true;
	tfSetCascades(thd, ql);
	return ql;
}

/*
 * tfGet...Cascades helper functions.
 */

local TQualList
tfGetCascadesFrStab(Stab stab)
{
	TFormUsesList	tful;
	TQualList	ql = listNil(TQual);

	for (tful = car(stab)->tformsUsed.list; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);
		if (tfu->exports) ql = listCons(TQual)(tfu->exports, ql);
	}

	return ql;
}

local TQualList
tfGetCatCascadesFrWith(TForm cat)
{
	TForm		tfb = tfWithBase(cat);
	TForm		tfw = tfWithWithin(cat);
	TQualList	bql, wql, ql;

	bql = tfGetCatCascades(tfb);

	if (tfCascades(tfw))
		wql = tfCascades(tfw);

	else if (tfGetStab(cat))
		wql = tfGetCascadesFrStab(tfGetStab(cat));

	else
		wql = tfGetCatCascades(tfw);

	ql = listNConcat(TQual)(listCopy(TQual)(bql), listCopy(TQual)(wql));

	return ql;
}


local TQualList
tfGetCascadesFrTrailingArray(TForm tf)
{
	TQualList ql = listNil(TQual);
	TForm atf, itf, *tfv;
	Stab stab;
	Length tfc;

	if (tfArgc(tf) != 2) return listNil(TQual);
	stab = tfGetStab(tf);

	itf = tfTrailingIArg(tf);
	atf = tfTrailingAArg(tf);
	
	tfv = tfIsMulti(itf) ? tfMultiArgv(itf): &itf;
	tfc = tfIsMulti(itf) ? tfMultiArgc(itf): 1;
	ql = listNConcat(TQual)(ql, tqListFrArgs(stab, tfv, tfc));

	tfv = tfIsMulti(atf) ? tfMultiArgv(atf): &atf;
	tfc = tfIsMulti(atf) ? tfMultiArgc(atf): 1;
	ql = listNConcat(TQual)(ql, tqListFrArgs(stab, tfv, tfc));

	return ql;
}


/******************************************************************************
 *
 * :: Type form representational symes.
 *
 *****************************************************************************/

local SymeList tfSymesFrDepGroup(Stab, TForm, TForm, SymeList);

Bool
tfTagHasSymes(TFormTag tag)
{
	switch (tag) {
	case TF_Declare:
	case TF_Cross:
	case TF_Map:
	case TF_PackedMap:
	case TF_Multiple:
	case TF_Enumerate:
	case TF_RawRecord:
	case TF_Record:
	case TF_TrailingArray:
	case TF_Union:
	case TF_Add:
	case TF_Third:
		return true;
	default:
		return false;
	}
}

Bool
tfSymeInducesDependency(Syme syme, TForm tf)
{
	Stab	stab = tfGetStab(tf);
	Bool	result = false;

	if (stab && symeDefLevel(syme) == car(stab))
		result = !symeUnused(syme) || tfIsCategoryMap(tf);

	tfMapDEBUG(dbOut, "tformSymeInducesDependency: %s %pSyme %pTForm\n", boolToString(result),
		   syme, tf);
	tfMapDEBUG(dbOut, "tformSymeInducesDependency: %pSyme Lvl %s unused: %s catMap %s\n", 
		   syme,
		   boolToString(stab && symeDefLevel(syme) == car(stab)),
		   boolToString(symeUnused(syme)),
		   boolToString(tfIsCategoryMap(tf)));

	return result;
}

/*
 * Functions to extract representational symes
 * from fully-analysed abstract syntax.
 */

local Syme
tfNewRepSyme(Stab stab, Symbol sym, TForm tf, Hash code)
{
	Syme	syme = symeNewExport(sym, tf, car(stab));
	symeSetSpecial(syme);
	symeAddHash(syme, code);
	return syme;
}

local SymeList
tfSymesFrDeclare(Sefo sefo)
{
	SymeList	symes = listNil(Syme);
	Sefo		id    = abDefineeIdOrElse(sefo, NULL);
	Syme		syme  = id ? abSyme(id) : NULL;

	if (syme) symes = listCons(Syme)(syme, symes);
	return symes;
}

local SymeList
tfSymesFrCross(TForm tf)
{
	SymeList	symes = listNil(Syme);
	Length		i, argc = tfArgc(tf);

	if (DEBUG(tfCross)) {
		fprintf(dbOut, "tfSymesFrCross:");
		fnewline(dbOut);
		fprintf(dbOut, "  tf:");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfCrossArgN(tf, i);
		Syme	syme = tfDefineeSyme(tfi);

		if (syme != 0 && tfSymeInducesDependency(syme, tf))
			symes = listCons(Syme)(syme, symes);
	}
	symes = listNReverse(Syme)(symes);

	if (DEBUG(tfCross)) {
		fprintf(dbOut, "  symes:");
		fnewline(dbOut);
		listPrint(Syme)(dbOut, symes, symePrint);
		fnewline(dbOut);
	}

	return symes;
}

local SymeList
tfSymesFrMulti(TForm tf)
{
	SymeList	symes = listNil(Syme);
	Length		i, argc = tfArgc(tf);

	if (DEBUG(tfMulti)) {
		fprintf(dbOut, "tfSymesFrMulti:");
		fnewline(dbOut);
		fprintf(dbOut, "  tf:");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfMultiArgN(tf, i);
		Syme	syme = tfDefineeSyme(tfi);

		if (syme != 0 && tfSymeInducesDependency(syme, tf))
			symes = listCons(Syme)(syme, symes);
	}
	symes = listNReverse(Syme)(symes);

	if (DEBUG(tfMulti)) {
		fprintf(dbOut, "  symes:");
		fnewline(dbOut);
		listPrint(Syme)(dbOut, symes, symePrint);
		fnewline(dbOut);
	}

	return symes;
}

local SymeList
tfSymesFrMap(TForm tf)
{
	SymeList	symes = listNil(Syme);
	Length		i, argc = tfMapArgc(tf);

	if (DEBUG(tfMap)) {
		fprintf(dbOut, "tfSymesFrMap:");
		fnewline(dbOut);
		fprintf(dbOut, "  tf:");
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfMapArgN(tf, i);
		Syme	syme = tfDefineeSyme(tfi);

		if (syme != 0 && tfSymeInducesDependency(syme, tf))
			symes = listCons(Syme)(syme, symes);
	}
	symes = listNReverse(Syme)(symes);

	if (DEBUG(tfMap)) {
		fprintf(dbOut, "  symes:");
		fnewline(dbOut);
		listPrint(Syme)(dbOut, symes, symePrint);
		fnewline(dbOut);
	}

	return symes;
}

local SymeList
tfSymesFrEnum(Stab stab, TForm tf, Sefo sefo)
{
	Syme		syme;
	SymeList	symes = listNil(Syme);
	Length		i, argc = abApplyArgc(sefo);
	TForm		me = tfFrSelf(stab, tf);
	TForm		tfm;
	Hash		code = abHash(sefo);

	/*
	 *  = : (me, me) -> Boolean
	 * ~= : (me, me) -> Boolean
	 */

	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * (and thus libAldor) are tinfered with (tfBoolean == tfUnknown).
	 * The correct fix is to ensure that tfBoolean has been imported
	 * into every scope that needs it before we get this far.
	 */
	tfm = tfMulti(2, me, me);
	if (tfBoolean == tfUnknown) comsgFatal(sefo, ALDOR_F_BugNoBoolean);
	tfm = tfMap(tfm, tfBoolean);

	syme = tfNewRepSyme(stab, ssymEquals, tfm, code);
	symes = listCons(Syme)(syme, symes);

	syme = tfNewRepSyme(stab, ssymNotEquals, tfm, code);
	symes = listCons(Syme)(syme, symes);

	for (i = 0; i < argc; i += 1) {
		Sefo	argi = abDefineeIdOrElse(abApplyArg(sefo, i), NULL);

		if (! argi) continue;
		assert(abTag(argi) == AB_Id);

		/*
		 * ni : E
		 */
		syme = tfNewRepSyme(stab, argi->abId.sym, me, code);
		symes = listCons(Syme)(syme, symes);
	}
	symes = listNReverse(Syme)(symes);

	stabSetSubstable(stab);
	return symes;
}

local void
tfCheckDenseArgs(TForm tf, Sefo sefo)
{
	Length	i;
	Length	argc = abApplyArgc(sefo);

	for (i = 0;i < argc; i++)
	{
		TForm	tfarg = tfArgv(tf)[i];
		Sefo	argi = abDefineeIdOrElse(abApplyArg(sefo, i), NULL);

		if (!tfDomHasImplicit(tfarg))
		{
			String cat = symString(ssymImplPAOps);
			comsgError(argi, ALDOR_E_TinPackedNotSat, cat);
		}
	}
}

local SymeList
tfSymesFrRawRecord(Stab stab, TForm tf, Sefo sefo)
{
	SymeList	symes = listNil(Syme);
	Syme		syme;
	Length		i, argc = abApplyArgc(sefo);
	TForm		tfc, tfm, me = tfFrSelf(stab, tf);
	Hash		code = abHash(sefo);
	Bool		ops[TFC_LIMIT];


	/*
	 * First ensure that all types have DenseStorageCategory.
	 * We ought to do this in libraries with the definition
	 * of RawRecord() but we also do it here for safety. The
	 * sting in the tail is that we never want to check the
	 * initial definition `RawRecord(T:Tuple Type): ...'.
	 */
	if ((argc != 1) || !tfIsTypeTuple(tfCatFrDom(tfArgv(tf)[0])))
		tfCheckDenseArgs(tf, sefo);


	/* These ought to be all true ... */
	ops[TFC_HasEq] 	  = false;
	ops[TFC_HasNeq]   = false;
	ops[TFC_HasPrint] = false;

	/*
	 *   [ ... ]:   (T1, T2, ... TN) -> %
	 *   rawrecord: (T1, T2, ... TN) -> %
	 *   explode:   % -> (T1, T2, ... TN)
	 *   dispose!:  % -> ()
	 *   apply:     (%, Enumerate(ti:Type)) -> Ti
	 *   set!:      (%, Enumerate(ti:Type), Ti) -> Ti
	 */


	/* Create the argument type for RawRecord: (t1:T1, ..., tN:TN) */
	tfc = tfNewEmpty(TF_Multiple, argc);
	for (i = 0; i < argc; i += 1) tfc->argv[i] = tf->argv[i];
	tfSetStab(tfc, abStab(sefo));
	tfSetSymes(tfc, tfSymesFrMulti(tfc));
	tfSetStab(tfc, NULL);


	/* Now create the type: (t1:T1, ..., tN:TN) -> % */
	tfm  = tfMap(tfc, me);
	tfSetStab(tfm, abStab(sefo));
	tfSetSymes(tfm, tfSymesFrMap(tfm));
	tfSetStab(tfm, NULL);

	tfSetSymes(tf, tfSymes(tfm));


	/* [ ... ] */
	syme = tfNewRepSyme(stab, ssymBracket, tfm, code);
	symes = listCons(Syme)(syme, symes);


	/* rawrecord( ... ) */
	syme = tfNewRepSyme(stab, ssymTheRawRecord, tfm, code);
	symes = listCons(Syme)(syme, symes);


        /* Reverse map: % -> (T1, T2, ..., TN) */
	tfm  = tfMap(me, tfc);


	/* explode: % -> (T1, T2, ..., TN) */
	syme = tfNewRepSyme(stab, ssymTheExplode, tfm, code);
	symes = listCons(Syme)(syme, symes);


	/* Void map: % -> () */
	tfc = tfMulti(int0);
	tfm = tfMap(me, tfc);


	/* dispose!: % -> () */
	syme = tfNewRepSyme(stab, ssymTheDispose, tfm, code);
	symes = listCons(Syme)(syme, symes);


	/* Create the `apply' and `set!' exports */
	for (i = 0; i < argc; i += 1) {
		TForm	tfi  = tf->argv[i];
		TForm	tfit = tfDefineeType(tfi);
		Syme	si   = tfDefineeSyme(tfi);
		Sefo	argi = abDefineeIdOrElse(abApplyArg(sefo, i), NULL);
		TForm	tfe;

		if (!si || !argi) continue;
		assert(abTag(argi) == AB_Id);

		if (!tfEqual(me, tfit))
			tfSymesTestCompoundType(stab, tfit, ops);

		/* apply: (%, Enumerate(ti: Type)) -> Ti */
		tfe   = tfEnum(stab, argi);
		tfm   = tfMap(tfMulti(2, me, tfe), tfi);
		syme  = tfNewRepSyme(stab, ssymApply, tfm, code);
		symes = listCons(Syme)(syme, symes);

		/* set!: (%, Enumerate(ti: Type), Ti) -> Ti */
		if (!listMemq(Syme)(tfSymes(tf), si)) {
			tfm  = tfMap(tfMulti(3, me, tfe, tfi), tfi);
			syme = tfNewRepSyme(stab, ssymSetBang, tfm, code);
			symes = listCons(Syme)(syme, symes);
		}
	}

	symes = tfSymesFrDepGroup(stab, me, tfc, symes);
	symes = tfSymesFrCompoundType(stab, me, ops, symes);
	symes = listNReverse(Syme)(symes);

	stabSetSubstable(stab);
	return symes;
}

local SymeList
tfSymesFrRecord(Stab stab, TForm tf, Sefo sefo)
{
	SymeList	symes = listNil(Syme);
	Syme		syme;
	Length		i, argc = abApplyArgc(sefo);
	TForm		tfc, tfm, me = tfFrSelf(stab, tf);
	Hash		code = abHash(sefo);

	Bool		  ops[TFC_LIMIT];
	ops[TFC_HasEq] 	  = true;
	ops[TFC_HasNeq]   = true;
	ops[TFC_HasPrint] = true;

	/*
	 * [ ]:	     (A1, ..., AN) -> me
	 * record:   (A1, ..., AN) -> me
	 * explode:  me -> (A1, ..., AN)
	 * dispose!: me -> ()
	 */

	tfc = tfNewEmpty(TF_Multiple, argc);
	for (i = 0; i < argc; i += 1) tfc->argv[i] = tf->argv[i];
	tfSetStab(tfc, abStab(sefo));
	tfSetSymes(tfc, tfSymesFrMulti(tfc));
	tfSetStab(tfc, NULL);

	tfm  = tfMap(tfc, me);
	tfSetStab(tfm, abStab(sefo));
	tfSetSymes(tfm, tfSymesFrMap(tfm));
	tfSetStab(tfm, NULL);

	tfSetSymes(tf, tfSymes(tfm));

	syme = tfNewRepSyme(stab, ssymBracket, tfm, code);
	symes = listCons(Syme)(syme, symes);

	syme = tfNewRepSyme(stab, ssymTheRecord, tfm, code);
	symes = listCons(Syme)(syme, symes);

	tfm  = tfMap(me, tfc);

	syme = tfNewRepSyme(stab, ssymTheExplode, tfm, code);
	symes = listCons(Syme)(syme, symes);

	tfc = tfMulti(int0);
	tfm = tfMap(me, tfc);

	syme = tfNewRepSyme(stab, ssymTheDispose, tfm, code);
	symes = listCons(Syme)(syme, symes);

	for (i = 0; i < argc; i += 1) {
		TForm	tfi  = tf->argv[i];
		TForm	tfit = tfDefineeType(tfi);
		Syme	si   = tfDefineeSyme(tfi);
		Sefo	argi = abDefineeIdOrElse(abApplyArg(sefo, i), NULL);
		TForm	tfe;

		if (!si || !argi) continue;
		assert(abTag(argi) == AB_Id);

		if (!tfEqual(me, tfit))
			tfSymesTestCompoundType(stab, tfit, ops);

		/*
		 * apply: (me, Enumerate(ni: Type)) -> Ai
		 * set!:  (me, Enumerate(ni: Type), Ai) -> Ai
		 */

		tfe = tfEnum(stab, argi);

		tfm  = tfMap(tfMulti(2, me, tfe), tfi);
		syme = tfNewRepSyme(stab, ssymApply, tfm, code);
		symes = listCons(Syme)(syme, symes);

		if (!listMemq(Syme)(tfSymes(tf), si)) {
			tfm  = tfMap(tfMulti(3, me, tfe, tfi), tfi);
			syme = tfNewRepSyme(stab, ssymSetBang, tfm, code);
			symes = listCons(Syme)(syme, symes);
		}
	}

	symes = tfSymesFrDepGroup(stab, me, tfc, symes);
	symes = tfSymesFrCompoundType(stab, me, ops, symes);
	symes = listNReverse(Syme)(symes);

	stabSetSubstable(stab);
	return symes;
}

local SymeList
tfSymesFrDepGroup(Stab stab, TForm me, TForm tfc, SymeList symes)
{
	return symes;
}

local SymeList
tfSymesFrTrailingArray(Stab stab, TForm tf, Sefo sefo)
{
	SymeList	symes = listNil(Syme);
	Syme		syme;
	Length		i, iargc, aargc;
	TForm 		me = tfFrSelf(stab, tf);
	TForm		atf, itf, tfc, tfm;
	Sefo		isefo, asefo;
	Hash		code = abHash(sefo);
	
	Bool		  ops[TFC_LIMIT];

	/* We might have an invalid TForm here... */
	if (tfArgc(tf) != 2) return listNil(Syme);

	ops[TFC_HasEq] 	  = false;
	ops[TFC_HasNeq]   = false;
	ops[TFC_HasPrint] = false;

	/*
	 * TrailingArray((i1: I1, i2: I2..., IN), (A1,A2,...AN))
	 * [ ]:	     	    (I1, ..., IN) -> Tuple Cross A -> me
	 * trailingArray:   (A1, ..., AN) -> Tuple Cross A -> me
	 * dispose!: me -> ()
	 */
	
	itf = tfTrailingIArg(tf);
	atf = tfTrailingAArg(tf);
	aargc = tfAsMultiArgc(atf);
	iargc = tfAsMultiArgc(itf);

	isefo = abApplyArg(sefo, int0);
	asefo = abApplyArg(sefo, 1);

	if (!tfIsMulti(itf)) itf = tfMulti(1, itf);
	if (!tfIsMulti(atf)) atf = tfMulti(1, atf);
	tfc = tfNewEmpty(TF_Multiple, 3);
	tfc->argv[0] = tfSingleInteger;
	tfc->argv[1] = tfCrossFrMulti(itf);
	tfc->argv[2] = tfCrossFrMulti(atf);
	tfSetStab(tfc, abStab(sefo));
	tfSetSymes(tfc, tfSymesFrMulti(tfc));
	tfSetStab(tfc, NULL);

	tfm  = tfMap(tfc, me);
	tfSetStab(tfm, abStab(sefo));
	tfSetSymes(tfm, tfSymesFrMap(tfm));
	tfSetStab(tfm, NULL);

	syme = tfNewRepSyme(stab, ssymTheTrailingArray, tfm, code);
	symes = listCons(Syme)(syme, symes);

	syme = tfNewRepSyme(stab, ssymBracket, tfm, code);
	symes = listCons(Syme)(syme, symes);

	tfc = tfMulti(int0);
	tfm = tfMap(me, tfc);

	syme = tfNewRepSyme(stab, ssymTheDispose, tfm, code);
	symes = listCons(Syme)(syme, symes);

	for (i = 0; i < iargc; i += 1) {
		TForm	tfi  = tfAsMultiArgN(itf, iargc, i);
		Syme	si   = tfDefineeSyme(tfi);
		Sefo	argi = abDefineeIdOrElse(abArgvAs(AB_Comma, isefo)[i], NULL);
		TForm	tfe;

		if (!si || !argi) continue;
		assert(abTag(argi) == AB_Id);
		
		/*
		 * apply: (me, 'ni') -> Ii
		 * set!:  (me, 'ni', Ii) -> Ii
		 */

		tfe = tfEnum(stab, argi);

		tfm  = tfMap(tfMulti(2, me, tfe), tfi);
		syme = tfNewRepSyme(stab, ssymApply, tfm, code);
		symes = listCons(Syme)(syme, symes);

		if (!listMemq(Syme)(tfSymes(tf), si)) {
			tfm  = tfMap(tfMulti(3, me, tfe, tfi), tfi);
			syme = tfNewRepSyme(stab, ssymSetBang, tfm, code);
			symes = listCons(Syme)(syme, symes);
		}

	}
	
	for (i = 0; i < aargc; i++) {
		TForm	tfi  = tfAsMultiArgN(atf, aargc, i);
		Syme	si   = tfDefineeSyme(tfi);
		Sefo	argi = abDefineeIdOrElse(abArgvAs(AB_Comma, asefo)[i], NULL);
		TForm	tfe;

		if (!si || !argi) continue;
		assert(abTag(argi) == AB_Id);
		/*
		 * apply: (me, SingleInteger, 'ni') -> Ai
		 * set!:  (me, SingleInteger, 'ni', Ii) -> Ai
		 */
		tfe = tfEnum(stab, argi);
		
		tfm  = tfMap(tfMulti(3, me, tfSingleInteger, tfe), tfi);
		syme = tfNewRepSyme(stab, ssymApply, tfm, code);
		symes = listCons(Syme)(syme, symes);
		
		if (!listMemq(Syme)(tfSymes(tf), si)) {
			tfm  = tfMap(tfMulti(4, me, tfSingleInteger, tfe, tfi), tfi);
			syme = tfNewRepSyme(stab, ssymSetBang, tfm, code);
			symes = listCons(Syme)(syme, symes);
		}
	}

	symes = tfSymesFrCompoundType(stab, me, ops, symes);
	symes = listNReverse(Syme)(symes);

	stabSetSubstable(stab);
#if 0
	tfPrintDb(tf);
	abPrintDb(sefo);
	symeListPrintDb(symes);
#endif	
	return symes;
}

local SymeList
tfSymesFrUnion(Stab stab, TForm tf, Sefo sefo)
{
	Syme		syme;
	SymeList	symes = listNil(Syme);
	Length		i, argc = abApplyArgc(sefo);
	TForm		me = tfFrSelf(stab, tf), tfm, tfc;
	Hash		code = abHash(sefo);

	Bool		  ops[TFC_LIMIT];
	ops[TFC_HasEq] 	  = true;
	ops[TFC_HasNeq]   = true;
	ops[TFC_HasPrint] = true;

	/*
	 * dispose!: me -> ()
	 */
	tfc = tfMulti(int0);
	tfm = tfMap(me, tfc);

	syme = tfNewRepSyme(stab, ssymTheDispose, tfm, code);
	symes = listCons(Syme)(syme, symes);

	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tf->argv[i];
		TForm	tfit = tfDefineeType(tfi);
		Sefo	argi = abDefineeIdOrElse(abApplyArg(sefo, i), NULL);
		TForm	tfe;
		TForm   tfc;
		Syme    prmi, vali;
		String  pname;
		if (! argi) continue;
		assert(abTag(argi) == AB_Id);

		if (! tfEqual(me, tfit))
			tfSymesTestCompoundType(stab, tfit, ops);

		/*
		 * [ ]:	    Ai -> me
		 * union:   Ai -> me
		 */
		tfe  = tfEnum(stab, argi);
		pname = strPrintf("!%s", argi->abId.sym->str);
		prmi = symeNewParam(symIntern(pname), tfe, car(stab));
		strFree(pname);
		vali = symeNewImport(argi->abId.sym, tfe, car(stab),
				       tfe);
		symeSetSpecial(vali);
		tfc  = tfDeclare(abFrSyme(prmi), tfe);
		tfc  = tfDefine(tfc, abFrSyme(vali));
		tfc  = tfMulti(2, tfi, tfc);
		tfm  = tfMap(tfc, me);
		syme = tfNewRepSyme(stab, ssymBracket, tfm, code);
		symes = listCons(Syme)(syme, symes);

		syme = tfNewRepSyme(stab, ssymTheUnion, tfm, code);
		symes = listCons(Syme)(syme, symes);

		/*
		 * case:  (me, Enumerate(ni: Type)) -> Boolean
		 * apply: (me, Enumerate(ni: Type)) -> Ai
		 * set!:  (me, Enumerate(ni: Type), Ai) -> Ai
		 */
		tfe = tfEnum(stab, argi);
		tfc = tfMulti(2, me, tfe);

		/*
		 * An unfixed compiler bug means that parts of Salli
		 * programs (and thus libAldor) are tinfered with
		 * (tfBoolean == tfUnknown). The correct fix is to
		 * ensure that tfBoolean has been imported into every
		 * scope that needs it before we get this far.
		 */
		if (tfBoolean == tfUnknown)
			comsgFatal(sefo, ALDOR_F_BugNoBoolean);
		tfm = tfMap(tfc, tfBoolean);
		syme = tfNewRepSyme(stab, ssymTheCase, tfm, code);
		symes = listCons(Syme)(syme, symes);

		tfm = tfMap(tfc, tfi);
		syme = tfNewRepSyme(stab, ssymApply, tfm, code);
		symes = listCons(Syme)(syme, symes);
 
		tfm  = tfMap(tfMulti(3, me, tfe, tfi), tfi);
		syme = tfNewRepSyme(stab, ssymSetBang, tfm, code);
		symes = listCons(Syme)(syme, symes);
	}

	symes = tfSymesFrCompoundType(stab, me, ops, symes);
	symes = listNReverse(Syme)(symes);

	stabSetSubstable(stab);
	return symes;
}

/*
 * Test if the following symes should be generated by seeing if the
 * the constituent types have them:
 *	 = : (me, me) -> Boolean
 *	~= : (me, me) -> Boolean
 *	<< : (TextWriter, me) -> TextWriter
 */
local void
tfSymesTestCompoundType(Stab stab, TForm tfi, Bool *ops)
{
	TForm		tfm = NULL, me = tfFrSelf(stab, tfi);
	SymeList	mods = tfGetDomSelf(tfi);

	/*
	 * An unfixed compiler bug means that parts of Salli programs
	 * tinfered with (tfBoolean == tfUnknown). We want to catch
	 * this problem as soon as possible.
	 */

	if (ops[TFC_HasEq]) {
		/*
		 * An unfixed compiler bug means that parts of Salli
		 * programs (and thus libAldor) are tinfered with
		 * (tfBoolean == tfUnknown). The correct fix is to
		 * ensure that tfBoolean has been imported into every
		 * scope that needs it before we get this far.
		 */
		tfm = tfMulti(2, me, me);
		if (tfBoolean == tfUnknown) {
			AbSyn ab = tfGetExpr(me);
			comsgFatal(ab, ALDOR_F_BugNoBoolean);
		}
		tfm = tfMap(tfm, tfBoolean);
		if (!tfHasDomExportMod(tfi, mods, ssymEquals, tfm))
			ops[TFC_HasEq] = false;
	}

	if (ops[TFC_HasNeq]) {
		if (!tfm) {
			tfm = tfMulti(2, me, me);
			if (tfBoolean == tfUnknown) {
				AbSyn ab = tfGetExpr(me);
				comsgFatal(ab, ALDOR_F_BugNoBoolean);
			}
			tfm = tfMap(tfm, tfBoolean);
		}
		if (!tfHasDomExportMod(tfi, mods, ssymNotEquals, tfm))
			ops[TFC_HasNeq] = false;
	}

	if (ops[TFC_HasPrint]) {
		tfm = tfMap(tfMulti(2, tfTextWriter, me), tfTextWriter);
		if (!tfHasDomExportMod(tfi, mods, ssymPrint, tfm))
			ops[TFC_HasPrint] = false;
	}
}

local SymeList
tfSymesFrCompoundType(Stab stab, TForm me, Bool *ops, SymeList symes)
{
	TForm		tfm = NULL;
	Symbol		sym;
	Syme		syme;

	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	if (ops[TFC_HasEq]) {
		tfm = tfMulti(2, me, me);
		if (tfBoolean == tfUnknown) {
			AbSyn ab = tfGetExpr(me);
			comsgFatal(ab, ALDOR_F_BugNoBoolean);
		}
		tfm = tfMap(tfm, tfBoolean);

		sym = ssymEquals;
		syme = symeNewExport(sym, tfm, car(stab));
		symes = listCons(Syme)(syme, symes);
	}

	if (ops[TFC_HasNeq]) {
		if (!tfm) {
			tfm = tfMulti(2, me, me);
			if (tfBoolean == tfUnknown) {
				AbSyn ab = tfGetExpr(me);
				comsgFatal(ab, ALDOR_F_BugNoBoolean);
			}
			tfm = tfMap(tfm, tfBoolean);
		}

		sym = ssymNotEquals;
		syme = symeNewExport(sym, tfm, car(stab));
		symes = listCons(Syme)(syme, symes);
	}

	if (ops[TFC_HasPrint]) {
		tfm = tfMap(tfMulti(2, tfTextWriter, me), tfTextWriter);

		sym = ssymPrint;
		syme = symeNewExport(sym, tfm, car(stab));
		symes = listCons(Syme)(syme, symes);
	}

	return symes;
}

Bool
tfHasPrint(Stab stab, TForm dom)
{
	Bool		  ops[TFC_LIMIT];
	ops[TFC_HasEq] 	  = false;
	ops[TFC_HasNeq]   = false;
	ops[TFC_HasPrint] = true;

	/* Why do we need this test? */
	if (tfIsRecord(dom) || tfIsRawRecord(dom))
		return false;

	tfHasSelf(dom) = false;
	tfSymesTestCompoundType(stab, dom, ops);

	return ops[TFC_HasPrint];
}

local SymeList
tfSymesFrAdd(Sefo sefo)
{
	assert(abStab(sefo));
	return stabGetExportedSymes(abStab(sefo));
}

/* tfSymesFrDefault handles conditionals spectacularly badly.
 * It isn't clear how to modify tfGetCat{Exports,Parents}FrWith
 * to do the right thing, even if we manage to remember what the
 * conditions are.  Modifying the meanings in place is probably 
 * doomed.
 */

local SymeList
tfSymesFrDefault(Sefo sefo)
{
	SymeList	symes = listNil(Syme);
	Length		i, argc;
	Sefo		*argv;
	Sefo		id;

	switch (abTag(sefo)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(sefo);
		argv = abArgv(sefo);
		break;
	case AB_If:
		argc = 2;
		argv = &sefo->abIf.thenAlt;
		break;
	default:
		id = abDefineeIdOrElse(sefo, NULL);
		assert(id == NULL || abSyme(id));
		if (id && abSyme(id) && symeIsExport(abSyme(id)))
			symes = listCons(Syme)(abSyme(id), symes);
		return symes;
	}

	symes = listNil(Syme);
	for (i = 0; i < argc; i += 1) {
		symes = listNConcat(Syme)(tfSymesFrDefault(argv[i]), symes);
	}

	return symes;
}


/*
 * Functions to construct abstract syntax from representational symes.
 */

local AbSyn
tfSymesToAdd(SymeList symes)
{
	/*!!*/
	return abNewNothing(sposNone);
}

AbSyn
tfSymesToWith(SymeList symes)
{
	AbSyn		ab;
	AbSynList	decls;
	Length		argc;

	decls = listNil(AbSyn);
	for ( ; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		AbSyn	id, type, decl;

		id    = abFrSyme(syme);
		type  = sefoCopy(tfExpr(symeType(syme)));

		decl  = abNewDeclare(sposNone, id, type);
		decls = listCons(AbSyn)(decl, decls);
	}

	argc = listLength(AbSyn)(decls);
	switch (argc) {
	case 0:
		ab = abNewNothing(sposNone);
		break;
	case 1:
		ab = car(decls);
		break;
	default:
		ab = abNewSequenceL(sposNone, listNReverse(AbSyn)(decls));
		break;
	}

	listFree(AbSyn)(decls);
	return ab;
}

AbSyn
tfWithFrAdd(AbSyn ab)
{
	return tfSymesToWith(listNReverse(Syme)(tfSymesFrAdd(ab)));
}


/******************************************************************************
 *
 * :: Specific import/export selection
 *
 *****************************************************************************/

/*
 * Select an import from an exporter. Wonder if tformEqualMod works?
 */
Syme
tfGetDomImport(TForm tf, String name, Bool (*tfCheck)(TForm))
{
	Syme	result = tfGetDomExport(tf, name, tfCheck);

	/* Did we find the export? */
	if (result)
	{
		SymeList	symes;

		/* Convert the export into an import */
		symes  = listSingleton(Syme)(result);
		symes  = symeListSubstSelf(stabFile(), tf, symes);
		result = car(symes);
	}


	/* Return the export (or nothing) */
	return result;
}

/*
 * Select an export from an exporter. Wonder if tformEqualMod works?
 */
Syme
tfGetDomExport(TForm tf, String name, Bool (*tfCheck)(TForm))
{
	Syme		op = (Syme)NULL;
	SymeList	symes = tfGetDomExports(tf);


	/* Search for the specified export */
	for (;!op && symes; symes = cdr(symes))
	{
		Syme	syme = car(symes);


		/* Does this export have the correct name? */
		if (!strEqual(name, symeString(syme))) continue;


		/* Check the type */
		if (!tfCheck(symeType(syme))) continue;


		/* Found it (assume type is correct) */
		op = syme;
	}


	/* Return the export (or nothing) */
	return op;
}


/******************************************************************************
 *
 * :: Special type forms
 *
 *****************************************************************************/

/*
 * Type form utility functions
 */

/* Find the symbol in a Declare/Define/Assign type form. */
Symbol
tfDefineeSymbol(TForm tf)
{
	tfFollow(tf);

	while (tfTag(tf) != TF_Declare) {
		switch (tfTag(tf)) {
		case TF_Assign:
			tf = tfAssignDecl(tf);
			break;
		case TF_Define:
			tf = tfDefineDecl(tf);
			break;
		case TF_Multiple:
			if (tfMultiArgc(tf) == 1) {
				tf = tfMultiArgN(tf, int0);
				break;
			}
			/* else fall through to default case. */
		default:
			return NULL;
		}
		tf = tfFollowSubst(tf);
	}
	return tfDeclareId(tf);
}

/* Find the syme in a Declare/Define/Assign type form. */
Syme
tfDefineeSyme(TForm tf)
{
	tf = tfFollowOnly(tf);

	while (tfTag(tf) != TF_Declare)
		switch (tfTag(tf)) {
		case TF_Syntax: {
			AbSyn	id = abDefineeIdOrElse(tfExpr(tf), NULL);
			if (id && abSyme(id))
				return abSyme(id);
			else
				return NULL;
			break;
		}
		case TF_Subst:
			if (tfDefineeSyme(tfSubstArg(tf)))
				tfFollow(tf);
			else
				return NULL;
			break;
		case TF_Assign:
			tf = tfAssignDecl(tf);
			break;
		case TF_Define:
			tf = tfDefineDecl(tf);
			break;
		case TF_Multiple:
			if (tfMultiArgc(tf) == 1)
				tf = tfMultiArgN(tf, int0);
			else
				return NULL;
			break;
		default:
			return NULL;
		}

	return tfDeclareSyme(tf);
}

/* Find the type in a Declare/Define/Assign type form. */
local TForm
tfDefineeType0(TForm tf, Bool subst)
{
	Bool	done = false;

	while (!done) {
		if (subst)
			tfFollow(tf);
		else
			tf = tfFollowSubst(tf);

		switch (tfTag(tf)) {
		case TF_Declare:
			tf = tfDeclareType(tf);
			break;
		case TF_Assign:
			tf = tfAssignDecl(tf);
			break;
		case TF_Define:
			if (tfIsDefineOfType(tf)) {
				TForm	cat = tfGetCategory(tfDefineVal(tf));
				if (tfIsUnknown(cat))
					tf = tfDefineDecl(tf);
				else
					tf = cat;
			}
			else
				tf = tfDefineDecl(tf);
			break;
		default:
			done = true;
			break;
		}
	}

	return tf;
}

/*
 * Walk down the definitions/declares/assigns to get the base type.
 * This is essentially a function to compute the normal-form of tf
 * when this is possible. Unfortunately some type definitions cause
 * us to expand a type to the same tform and we end up looping. To
 * prevent this we keep a list of all expansions and check that we
 * haven't come across this one before. Since it is most likely that
 * the previous tform will be the same as the current one we trace
 * backwards towards the original tform.
 *
 * One further twist - we don't want to expand % so we stop as soon
 * as we come across it. We might also want to stop at %% but they
 * are quite a rare beast.
 *
 * Yet another twist - we may not want to replace definitions with
 * the original `add' expression: if `notAdd' is true then we stop
 * before replacing a tform with an add expression. Might want to
 * extend this later for `with'.
 *
 * Problem - we don't want to replace definitions whose type is not
 * known with their value as we lose information.
 */
local TForm
tfDefineeType1(TForm tf, Bool subst, Bool notAdd)
{
	Length		depth = 0;
	Bool		done = false;
	TFormList	seen = listNil(TForm);
	TFormList	tfs;


	/* Continually expand tf */
	while (!done)
	{
		/* Stop this time unless there is a reason not to */
		done = true;


		/* Be brutal ... */
		tf = tfDefineeType0(tf, subst);


		/* We don't expand % */
		if (tfIsSelf(tf)) break;


		/* Check previous tforms in case of cycles */
		for (tfs = seen;tfs;tfs = cdr(tfs))
		{
			/* Have we seen this tform before? */
			if (tfEqual(car(tfs), tf))
				goto byeBye; /* Yes */
		}


		/* No - are we in too deep? */
		if (depth > tfMaxBaseSearchDepth)
		{
			bugWarning("tfDefineeType1: too deep");
			break; /* Yes */
		}


		/* No - push tform onto the list of tforms seen */
		seen = listCons(TForm)(tf, seen);
		depth++;


		/* See if we can expand further */
		if (tfIsAbSyn(tf))
		{
			TForm	tfn	= (TForm) NULL;
			Sefo	sefo	= tfGetExpr(tf);
			TForm	cat	= abGetCategory(sefo);


			/* Did we get anything useful? */
			switch (tfTag(cat))
			{
			   case TF_Assign:
				tfn  = tfAssignVal(cat);
				done = false; /* Not finished yet */
				break;
			   case TF_Define:
				tfn  = tfDefineVal(cat);
				done = false; /* Not finished yet */
				break;
			   default:
				/* Finished */
				break;
			}


			/* We must tread carefully again */
			if (!done)
			{
				/* Must have something to replace */
				if (!tfn)
				{
					done = true;
					continue;
				}


				/* Can't replace with add unless OK'd */
				if (notAdd && (tfTag(tfn) == TF_Add))
				{
					done = true;
					continue;
				}


				/* Special tforms are fine */
				if (!tfIsAbSyn(tfn))
				{
					/* Do the replacement */
					tf = tfn;
					continue;
				}


				/* Get the absyn */
				sefo = tfGetExpr(tfn);


				/* Don't replace if it is a sequence */
				if (!abHasTag(sefo, AB_Sequence))
					tf = tfn;
				else
					done = true;
			}
		}
	}


	/* Common exit-point */
byeBye:
	listFree(TForm)(seen);
	return tf;
}

TForm
tfDefineeType(TForm tf)
{
	return tfDefineeType0(tf, true);
}

TForm
tfDefineeTypeSubst(TForm tf)
{
	return tfDefineeType0(tf, false);
}

TForm
tfDefineeBaseType(TForm tf)
{
	/* Normalise tf if possible */
	extern Bool stabDumbImport(void);
	return tfDefineeType1(tf, true, !stabDumbImport()); /* true); */
}

/*
 * Same as tfDefineeType but if it can be expanded
 * further then we do so.
 */
TForm
tfDefineeMaybeType(TForm tf)
{
	/* Deal with delta-equality */
	if (tfIsAbSynTag(tfTag(tf)))
		tf = tfDefineeBaseType(tf);
	return tf;
}

/*
 * Type form symbols
 */

/* These are initialized in tfInit(). */
AbSyn	abUnknown;
TForm	tfUnknown;
TForm	tfExit;
TForm	tfLiteral;
TForm	tfTest;
TForm	tfType;
TForm	tfTypeTuple;
TForm	tfCategory;
TForm	tfDomain;
TForm	tfBoolean;
TForm	tfTextWriter;
TForm	tfSingleInteger;

/* Is tf the type of a domain? */
Bool
tfIsDomainType(TForm tf)
{
	if (tfIsSyntax(tf))
		return abHasTag(tfGetExpr(tf), AB_With);

	if (tfIsDefine(tf) && tfIsDeclare(tfDefineDecl(tf)))
		return tfIsWith(tfDeclareType(tfDefineDecl(tf)));

	if (tfIsJoin(tf))
		return true;

	if (tfIsCategoryType(tfGetCategory(tf)))
		return true;
	return false;
}

/* Is tf the type of a domain constructor? */
Bool
tfIsDomainMap(TForm tf)
{
	if (tfIsAnyMap(tf))
		return tfIsDomainMap(tfMapRet(tf));
	else
		return tfIsDomainType(tf);
}

/* Is tf the type of a category? */
Bool
tfIsCategoryType(TForm tf)
{
	AbSyn	ab = NULL;

	tf = tfFollowSubst(tf);

	if (tfIsDefine(tf) && tfIsDeclare(tfDefineDecl(tf)))
		return tfIsCategoryType(tfDeclareType(tfDefineDecl(tf)));

	if (tfIsCategory(tf) || tfIsThird(tf))
		return true;

	if (tfIsSyntax(tf))
		ab = tfGetExpr(tf);

	return ab && abIsTheId(ab, ssymCategory);
}

/* Is tf the type of a category constructor? */
Bool
tfIsCategoryMap(TForm tf)
{
	if (tfIsAnyMap(tf))
		return tfIsCategoryMap(tfMapRet(tf));
	else
		return tfIsCategoryType(tf);
}

/*
 * tfBoolean
 */

Bool
tfIsBooleanFn(TForm tf)
{
	/*
	 * An unfixed compiler bug means that parts of Salli
	 * programs (and thus libAldor) are tinfered with
	 * (tfBoolean == tfUnknown). The correct fix is to
	 * ensure that tfBoolean has been imported into every
	 * scope that needs it before we get this far.
	 */
	AbSyn ab = tfGetExpr(tf);
	if (tfBoolean == tfUnknown) comsgFatal(ab, ALDOR_F_BugNoBoolean);
	return tfSatisfies(tf, tfBoolean);
}

/*
 * tfLibrary
 */

TForm
tfLibrary(Syme syme)
{
	TForm	tf;
	assert(symeIsLibrary(syme));
	tf = tfFrSyme(stabFile(), syme);
	if (tfSymes(tf) == listNil(Syme))
		tfSetSymes(tf, listCons(Syme)(syme, listNil(Syme)));
	return tf;
}

TForm
tfGetLibrary(Syme syme)
{
	Syme osyme = stabGetLibrary(symeId(syme));
	if (osyme && libEqual(symeLibrary(osyme), symeLibrary(syme)))
		syme = osyme;
	return tfLibrary(syme);
}

Bool
tfIsLibrary(TForm tf)
{
	tfFollowSubst(tf);
	return	(tfSymes(tf) != listNil(Syme)) &&
		(symeIsLibrary(car(tfSymes(tf))));
}

Bool
tfIsBasicLib(TForm tf)
{
	if (tfIsLibrary(tf))
		return libIsBasicLib(tfLibraryLib(tf));
	else if (tfIsArchive(tf))
		return arHasBasicLib(tfArchiveAr(tf));
	else
		return false;
}

Syme
tfLibrarySyme(TForm tf)
{
	tfFollow(tf);
	return car(tfSymes(tf));
}

Lib
tfLibraryLib(TForm tf)
{
	tfFollow(tf);
	return symeLibrary(tfLibrarySyme(tf));
}

String
tfLibraryName(TForm tf)
{
	tfFollow(tf);
	return libGetFileId(tfLibraryLib(tf));
}

/*
 * tfArchive
 */

TForm
tfArchive(Syme syme)
{
	TForm	tf;
	assert(symeIsArchive(syme));
	tf = tfFrSyme(stabFile(), syme);
	if (tfSymes(tf) == listNil(Syme))
		tfSetSymes(tf, listCons(Syme)(syme, listNil(Syme)));
	return tf;
}

TForm
tfGetArchive(Syme syme)
{
	Syme osyme = stabGetArchive(symeId(syme));
	if (osyme && arEqual(symeArchive(osyme), symeArchive(syme)))
		syme = osyme;
	return tfArchive(syme);
}

Bool
tfIsArchive(TForm tf)
{
	tfFollowSubst(tf);
	return	(tfSymes(tf) != listNil(Syme)) &&
		(symeIsArchive(car(tfSymes(tf))));
}

Syme
tfArchiveSyme(TForm tf)
{
	tfFollow(tf);
	return car(tfSymes(tf));
}

Archive
tfArchiveAr(TForm tf)
{
	tfFollow(tf);
	return symeArchive(tfArchiveSyme(tf));
}

TForm
tfArchiveLib(TForm tf, Syme syme)
{
	tfFollow(tf);
	return tfLibrary(arLibrarySyme(tfArchiveAr(tf), syme));
}

/*
 * tfDeclare
 */

TForm
tfDeclare(AbSyn id, TForm tf)
{
	TForm	new;

	tfFollow(tf);

	new = tfNewNode(TF_Declare, 1, tf);
	tfSetSymes(new, tfSymesFrDeclare(id));
	tfSetMeaningArgs(new);

	return new;
}

Symbol
tfDeclareId(TForm tf)
{
	Syme	syme;
	tfFollow(tf);
	syme = tfDeclareSyme(tf);
	return syme ? symeId(syme) : NULL;
}

Syme
tfDeclareSyme(TForm tf)
{
	tfFollow(tf);
	return tfSymes(tf) ? car(tfSymes(tf)) : NULL;
}

/*
 * tfDefine
 */

TForm
tfDefine(TForm tf, AbSyn val)
{
	tfFollow(tf);
	return tfNewNode(TF_Define, 2, tf, tfNewAbSyn(TF_General, val));
}

TForm
tfDefineOfType(TForm tf)
{
	TForm	ntf;

	ntf = tfNewNode(TF_Declare, 1, tfType);
	tfSetMeaningArgs(ntf);

	ntf = tfNewNode(TF_Define, 2, ntf, tf);
	tfSetMeaningArgs(ntf);

	return ntf;
}

Bool
tfIsDefineOfType(TForm tf)
{
	Bool	result = false;

	if (tfIsDefine(tf) && tfIsDeclare(tfDefineDecl(tf))) {
		tf = tfDeclareType(tfDefineDecl(tf));
		result = tfIsType(tf) || tfIsUnknown(tf);
	}

	return result;
}

Bool
tfIsDefinedType(TForm tf)
{
	TForm	cat;
	TForm   tf2 = tfDefineeTypeSubst(tf);
	/* !!Maybe a bit strong.
	 * What it should do is return true
	 * for uses of garbage types like Foo == zzz(x)(y);
	 * This function is performance critical because
	 * of the call from tformEqual0.  Consequently,
	 * it shouldn't call tfGetCategory or symeType
	 * if at all possible 
	 */
	if (!tfIsGeneral(tf2))
		return false;

	if (tfIsSelf(tf) && tfIdSyme(tf))
		cat = symeType(tfIdSyme(tf));
	else
		cat = tfGetCategory(tf);

	return tfIsDefineOfType(cat);
}

TForm
tfDefinedVal(TForm tf)
{
	TForm	cat;

	if (tfIsSelf(tf) && tfIdSyme(tf))
		cat = symeType(tfIdSyme(tf));
	else
		cat = tfGetCategory(tf);

	return tfDefineVal(cat);
}

/*
 * tfAssign
 */

TForm
tfAssign(TForm tf, AbSyn val)
{
	tfFollow(tf);
	return tfNewNode(TF_Assign, 2, tf, tfNewAbSyn(TF_General, val));
}


/*
 * tfMap
 */

TForm
tfAnyMap(TForm arg, TForm ret, Bool packed)
{
	if (packed)
		return tfPackedMap(arg, ret);
	else
		return tfMap(arg, ret);
}

TForm
tfMap(TForm arg, TForm ret)
{
	TForm	tf;
	tfFollow(arg);
	tfFollow(ret);
	tf = tfNewNode(TF_Map, 2, arg, ret);
	tfSetSymes(tf, tfSymesFrMap(tf));
	return tf;
}

TForm
tfPackedMap(TForm arg, TForm ret)
{
	TForm	tf;
	tfFollow(arg);
	tfFollow(ret);
	tf = tfNewNode(TF_PackedMap, 2, arg, ret);
	tfSetSymes(tf, tfSymesFrMap(tf));
	return tf;
}

Bool
tfIsDependentMap(TForm tf)
{
	Bool	result;

	tfFollow(tf);

	if (tfIsAnyMap(tf) && !tfSymes(tf))
		tfSetSymes(tf, tfSymesFrMap(tf));

	result = tfIsAnyMap(tf) && tfSymes(tf) != listNil(Syme);

	if (DEBUG(tfMap)) {
		fprintf(dbOut, "tfIsDependentMap:  %s\n  tf:",
			boolToString(result));
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	return result;
}

/* Return true if the map has default values for arguments.  */
Bool
tfMapHasDefaults(TForm tf)
{
	Bool	result = false;
	Length	i;

	tfFollow(tf);
	assert(tfIsAnyMap(tf));

	for (i = 0; i < tfMapArgc(tf); i++)
		if (tfIsDefine(tfMapArgN(tf, i)))
			result = true;

	return result;
}

Length
tfMapArgc(TForm tf)
{
	tfFollow(tf);
	return tfAsMultiArgc(tfMapArg(tf));
}

/* Return the type of the nth argument value of a map of type tf. */
TForm
tfMapArgN(TForm tf, Length n)
{
	tfFollow(tf);
	return tfAsMultiArgN(tfMapArg(tf), tfMapArgc(tf), n);
}

/* Return the type of the nth argument value of a map of type tf
 * in an application with argc arguments.
 */
TForm
tfMapMultiArgN(TForm tf, Length argc, Length n)
{
	tfFollow(tf);
	return tfAsMultiArgN(tfMapArg(tf), argc, n);
}

/* Return the embedding necessary to pass argc arguments
 * to a map of type tf.
 */
AbEmbed
tfMapMultiArgEmbed(TForm tf, Length argc)
{
	tfFollow(tf);
	return tfAsMultiEmbed(tfMapArg(tf), argc);
}

Length
tfMapRetc(TForm tf)
{
	tfFollow(tf);
	return tfAsMultiArgc(tfIgnoreExceptions(tfMapRet(tf)));
}

/* Return the type of the nth return value of a map of type tf. */
TForm
tfMapRetN(TForm tf, Length n)
{
	tfFollow(tf);
	return tfAsMultiArgN(tfMapRet(tf), tfMapRetc(tf), n);
}

/* Return the type of the nth return value of a map of type tf
 * in an application with argc arguments.
 */
TForm
tfMapMultiRetN(TForm tf, Length argc, Length n)
{
	tfFollow(tf);
	return tfAsMultiArgN(tfMapRet(tf), argc, n);
}

/* Return the embedding necessary to return argc arguments
 * from a map of type tf.
 */
AbEmbed
tfMapMultiRetEmbed(TForm tf, Length argc)
{
	tfFollow(tf);
	return tfAsMultiEmbed(tfMapRet(tf), argc);
}

/*
 * tfCross
 */

TForm
tfCross(Length argc, ...)
{
	TForm	tf;
	Length	i;
	va_list argp;

	tf = tfNewEmpty(TF_Cross, argc);

	va_start(argp, argc);
	for (i = 0; i < argc; i += 1)
		tfArgv(tf)[i] = va_arg(argp, TForm);
	va_end(argp);

	tfSetSymes(tf, tfSymesFrCross(tf));

	return tf;
}

TForm
tfCrossFrMulti(TForm tfm)
{
	TForm	tf;
	Length	i, argc = tfMultiArgc(tfm);

	tf = tfNewEmpty(TF_Cross, argc);
	
	for (i = 0; i < argc; i++)
		tfArgv(tf)[i] = tfArgv(tfm)[i];

	tfSetSymes(tf, tfSymesFrCross(tf));

	return tf;
}

Bool
tfIsDependentCross(TForm tf)
{
	Bool	result;

	tfFollow(tf);

	if (tfIsCross(tf) && !tfSymes(tf))
		tfSetSymes(tf, tfSymesFrCross(tf));

	result = tfIsCross(tf) && tfSymes(tf) != listNil(Syme);

	if (DEBUG(tfCross)) {
		fprintf(dbOut, "tfIsDependentCross:  %s\n  tf:",
			boolToString(result));
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	return result;
}

Length
tfCrossArgc(TForm tf)
{
	assert(tfIsCross(tf));
	return tfArgc(tf);
}

/*
 * tfMultiple
 */

local void
tfMultiAfter(TForm tf)
{
	tfSetMeaningArgs(tf);
	tfSetSymes(tf, tfSymesFrMulti(tf));
	if (tfArgc(tf) == 0) tfToAbSyn(tf);
}

TForm
tfMulti(Length argc, ...)
{
	TForm	tf;
	Length	i;
	va_list argp;

	tf = tfNewEmpty(TF_Multiple, argc);

	va_start(argp, argc);
	for (i = 0; i < argc; i += 1)
		tfArgv(tf)[i] = va_arg(argp, TForm);
	va_end(argp);

	tfMultiAfter(tf);
	return tf;
}

Bool
tfIsDependentMulti(TForm tf)
{
	Bool	result;

	tfFollow(tf);

	if (tfIsMulti(tf) && !tfSymes(tf))
		tfSetSymes(tf, tfSymesFrMulti(tf));

	result = tfIsMulti(tf) && tfSymes(tf) != listNil(Syme);

	if (DEBUG(tfMulti)) {
		fprintf(dbOut, "tfIsDependentMulti:  %s\n  tf:",
			boolToString(result));
		tfPrint(dbOut, tf);
		fnewline(dbOut);
	}

	return result;
}

TForm
tfMultiFrList(TFormList tfl)
{
	TForm	tf;
	Length	i, argc = listLength(TForm)(tfl);

	tf = tfNewEmpty(TF_Multiple, argc);

	for (i = 0; tfl; i += 1, tfl = cdr(tfl))
		tfArgv(tf)[i] = car(tfl);

	tfMultiAfter(tf);
	return tf;
}

TForm
tfMultiFrTUnique(Sefo sefo)
{
	TForm	tf;
	Length	i, argc = abArgc(sefo);

	tf = tfNewEmpty(TF_Multiple, argc);

	for (i = 0; i < argc; i += 1)
		tfArgv(tf)[i] = abTUnique(abArgv(sefo)[i]);

	tfMultiAfter(tf);
	return tf;
}

Length
tfMultiArgc(TForm tf)
{
	assert(tfIsMulti(tf));
	return tfArgc(tf);
}

Bool
tfMultiHasDefaults(TForm tf)
{
	Bool	result = false;
	Length	i;

	tfFollow(tf);

	if (!tfIsMulti(tf))
		return tfIsDefine(tf);

	for (i = 0; i < tfMultiArgc(tf); i++) {
		TForm tfi = tfFollowSubst(tfMultiArgN(tf,i));
		if (tfIsDefine(tfi))
			result = true;
	}
	return result;
}

AbEmbed
tfAsMultiEmbed(TForm tf, Length argc)
{
	AbEmbed		result = AB_Embed_Fail;
	TForm		ntf;

	ntf = tfDefineeType(tf);
	tf  = tfFollowOnly(tf);

	if (tfIsMulti(ntf) &&
	    (tfMultiArgc(ntf) == argc ||
	     (tfMultiArgc(ntf) > argc && tfMultiHasDefaults(ntf))))
		result = AB_Embed_Identity;

	else if (argc == 1)
		result = AB_Embed_Identity;

	else if (tfIsTuple(ntf))
		result = AB_Embed_ApplyMultiToTuple;

	else if (tfIsCross(ntf) && tfCrossArgc(ntf) == argc)
		result = AB_Embed_ApplyMultiToCross;
	else if (argc == 0 && tfIsDefine(tf))
		return AB_Embed_Identity;

	return result;
}

Length
tfAsMultiArgc(TForm tf)
{
	tfFollow(tf);
	return (tfIsMulti(tf) ? tfMultiArgc(tf) : 1);
}

TForm
tfAsMultiArgN(TForm tf, Length argc, Length n)
{
	TForm	result = 0;
	TForm	ntf;

	assert(n < argc);
	ntf = tfDefineeType(tf);

	if (tfIsMulti(ntf) && tfMultiArgc(ntf) == argc)
		result = tfMultiArgN(ntf, n);

	else if (argc == 1)
		result = tf;

	else if (tfIsTuple(ntf))
		result = tfTupleArg(ntf);

	else if (tfIsCross(ntf) && tfCrossArgc(ntf) == argc)
		result = tfCrossArgN(ntf, n);

	assert(result);
	return result;
}

#define		abIsDefineKeyword(ab)	\
	(abHasTag(ab, AB_Define) && abHasTag(abDefineDecl(ab), AB_Id))

AbSyn
tfAsMultiSelectArg(AbSyn ab, Length argc, Length n, AbSynGetter argf,
		   TForm tfi, Bool * def, Length * pos)
{
	AbSyn	abpos, abkey, abi;
	Symbol	name;
	Length	i;

	/* ?? tfFollowSubst */
	tfFollow(tfi); 
	/* Find the keyword name. */
	name = tfDefineeSymbol(tfi);

	/* Find the positional argument. */
	abpos = (n < argc) ? argf(ab, n) : NULL;
	*pos = n;

	/* If abpos is a define, assume its a keyword, not positional.
	 * But if tfi doesn't have a keyword (if name == NULL),
	 * treat a define as positional so we can type check
	 * 'Record(x: Integer == 0)' correctly.
	 */
	if (name && abpos && abIsDefineKeyword(abpos))
		abpos = NULL;

	/* Find the keyword argument. */
	abkey = NULL;
	for (i = 0; name && !abkey && i < argc; i += 1) {
		AbSyn	ak = argf(ab, i);
		if (abIsDefineKeyword(ak) &&
		    abIsTheId(abDefineeId(ak), name)) {
			abkey = ak;
			*pos = i;
		}
	}

	/* Select the argument to use. */
	*def = false;
	if (abpos && abkey)
		abi = NULL;

	else if (abpos)
		abi = abpos;

	else if (abkey)
		abi = abkey->abDefine.rhs;

	else if (tfIsDefine(tfi)) {
		/*abi = tfGetExpr(tfDefineVal(tfi));*/
		abi = tfExpr(tfDefineVal(tfi));
		*def = true;
	}
	else
		abi = NULL;

	return abi;
}

/*
 * Grab the ith argument from a flat absyn,
 */
AbSyn
tfMapSelectArg(TForm opTf, AbSyn ab, Length i)
{
	Length  ai;
	Bool 	def;
	TForm tfi = tfMapArgN(opTf, i);
	AbSyn abi = tfAsMultiSelectArg(ab, 
				       abApplyArgc(ab), i, 
				       abApplyArgf, tfi, &def, &ai);

	if (abTag(abi) == AB_Define)
		abi = abi->abDefine.rhs;

	return abi;
}

/*
 * tfTuple
 */

TForm
tfTuple(TForm arg)
{
	TForm	tf = tfNewNode(TF_Tuple, 1, arg);
	tfSetMeaningArgs(tf);
	return tf;
}

/*
 * tfEnumerate
 */

TForm
tfEnum(Stab stab, AbSyn id)
{
	TForm   tf = tfNewNode(TF_Enumerate, 1, tfDeclare(id, tfType));
	AbSyn	ab = tfExpr(tf);
	TForm	ntf;

	if ((ntf = stabGetTForm(stab, ab, NULL)) != NULL)
		return ntf;

	tfSetSymes(tf, tfSymesFrEnum(stab, tf, ab));
	tfSetMeaningArgs(tf);

	stabDefTForm(stab, tf);
	return tf;
}

Length
tfEnumArgc(TForm tf)
{
	assert(tfIsEnum(tf));
	return tfArgc(tf);
}

Symbol
tfEnumId(TForm tf, Length n)
{
	TForm	tfi;

	assert(tfIsEnum(tf));
	tfi = tfEnumArgN(tf, n);

	if (tfIsDeclare(tfi))
		return tfDeclareId(tfi);
	else if (tfIsId(tfi))
		return tfIdSym(tfi);
	else
		return NULL;
}

Symbol
tfCompoundId(TForm tf, Length n)
{
	TForm	tfi = tfFollow(tfArgv(tf)[n]);

	if (tfIsDeclare(tfi))
		return tfDeclareId(tfi);
	else if (tfIsId(tfi))
		return tfIdSym(tfi);
	else
		return NULL;
}

/*
 * tfRawRecord
 */
Length
tfRawRecordArgc(TForm tf)
{
	assert(tfIsRawRecord(tf));
	return tfArgc(tf);
}

/*
 * tfRecord
 */

Length
tfRecordArgc(TForm tf)
{
	assert(tfIsRecord(tf));
	return tfArgc(tf);
}

/*
 * tfTrailingArray
 */

Length
tfTrailingArrayIArgc(TForm tf)
{
	assert(tfIsTrailingArray(tf));
	return tfArgc(tf);
}

Length
tfTrailingArrayAArgc(TForm tf)
{
	assert(tfIsTrailingArray(tf));
	return tfArgc(tf);
}

TForm
tfTrailingArrayIArgN(TForm tf, Length n)
{
	assert(tfIsTrailingArray(tf));
	bug("tfTrailingArrayIArgN: aargh");
	return tfFollowArg(tf, n);
}

TForm
tfTrailingArrayAArgN(TForm tf, Length n)
{
	assert(tfIsTrailingArray(tf));
	bug("tfTrailingArrayAArgN: aargh");
	return tfFollowArg(tf, n);
}

/*
 * tfUnion
 */

Length
tfUnionArgc(TForm tf)
{
	assert(tfIsUnion(tf));
	return tfArgc(tf);
}

/*
 * tfWith
 */

TForm
tfWith(TForm base, TForm body)
{
	TForm	tf = tfNewNode(TF_With, 2, base, body);
	tfSetMeaningArgs(tf);
	return tf;
}

TForm
tfWithFrSymes(SymeList symes)
{
	TForm	tf;
	tf = tfNewNode(TF_With, 2, tfNone(), tfNone());
	tfCatExports(tf) = symes;
	tfSetMeaning(tf);
	return tf;
}

TForm
tfWithFrAbSyn(AbSyn absyn)
{
	TForm	tf;
	tf = tfWithFrSymes(listNReverse(Syme)(tfSymesFrAdd(absyn)));
	tfAddSelf(tf, tfGetSelfFrStab(abStab(absyn)));
	return tf;
}

/*
 * tfThird
 */

TForm
tfThirdFrTForm(TForm tfw)
{
	TForm	tf;
	tf = tfNewNode(TF_Third, 1, tfw);
	return tf;
}

TForm
tfThird(SymeList symes)
{
	TForm	tf;
	tf = tfNewNode(TF_Third, 1, tfNone());
	tfThdExports(tf) = symes;
	tfSetMeaning(tf);
	return tf;
}

/*
 * tfGenerator
 */

TForm
tfGenerator(TForm arg)
{
	TForm	tf = tfNewNode(TF_Generator, 1, arg);
	tfSetMeaningArgs(tf);
	return tf;
}

Bool
tfIsGeneratorFn(TForm tf)
{
	tf = tfFollowSubst(tf);
	return tfIsGenerator(tf);
}

/*
 * tfReference
 */

TForm
tfReference(TForm arg)
{
	TForm	tf = tfNewNode(TF_Reference, 1, arg);
	tfSetMeaningArgs(tf);
	return tf;
}

Bool
tfIsReferenceFn(TForm tf)
{
	tf = tfFollowSubst(tf);
	return tfIsReference(tf);
}

/*
 * tfSubst
 */

static int	TFSubstCount		= 0;

TForm
tfSubst(AbSub sigma, TForm arg)
{
	TForm	tf = tfNewNode(TF_Subst, 1, arg);
	tfSubstSigma(tf) = absRefer(sigma);
	tfSetMeaningArgs(tf);
	TFSubstCount += 1;
	return tf;
}

TForm
tfSubstPush(TForm tf)
{
	AbSub	sigma;

	assert(tfIsSubst(tf));
	assert(tf->sigma);

	sigma = absRefer(tf->sigma);
	absClrLazy(sigma);

	tf->argv[0] = tformSubst(sigma, tf->argv[0]);

	absSetLazy(sigma);
	absFree(sigma);

	if (tf->sigma) {
		tf->sigma	= NULL;
		tf->tag		= TF_Forward;
		absFree(sigma);
	}
	if (tfConditions(tf) != NULL) {
		tfDEBUG(dbOut, "SubstPushCond: %pTForm %pAbSynList\n",
			tf->argv[0], tfConditionalAbSyn(tf));
		tfSetConditions(tf->argv[0], tfConditions(tf));
	}

	assert(tf->argv[0] != tf);
	return tf->argv[0];
}

/*
 * tfTrigger
 */

TForm
tfTrigger(Syme syme)
{
	TForm	tf = tfNewEmpty(TF_Trigger, 1);
	tfSetSymes(tf, listCons(Syme)(syme, listNil(Syme)));
	return tf;
}

Lib
tfTriggerLib(TForm tf)
{
	assert(tfIsTrigger(tf));
	assert(tfSymes(tf));
	return symeLib(car(tfSymes(tf)));
}

TForm
tfForwardFrTrigger(TForm otf, TForm ntf)
{
	assert(tfIsTrigger(otf));
	assert(otf != ntf);

	otf->tag	= TF_Forward;
	otf->argc	= 1;
	otf->argv[0]	= ntf;

	return otf;
}

/*
 * tfFollow
 */

#if 0
/* Debugging code for tfFollowFn. */

static int	TFFollowFrom		= TFF_Other;
static int	TFFollowCount[TFF_Limit];

void
tfFollowInit(void)
{
	int	i;
	for (i = 0; i < TFF_Limit; i += 1)
		TFFollowCount[i] = 0;
}

void
tfFollowFini(void)
{
	int	i;

	fprintf(dbOut, "calls to tfSubst:  %d\n", TFSubstCount);
	fprintf(dbOut, "calls to tfSubstPush from:");
	fnewline(dbOut);

	for (i = 0; i < TFF_Limit; i += 1)
		fprintf(dbOut, "%d\t%d\n", i, TFFollowCount[i]);
}

void
tfFollowFrom(int loc)
{
	TFFollowFrom = loc;
}
#endif

TForm
tfFollowOnly(TForm tf)
{
	Bool	done = (tf == NULL);

	while (!done) {
		if (tfIsForward(tf))
			tf = tf->argv[0];
		else if (tfIsTrigger(tf))
			libGetAllSymes(tfTriggerLib(tf));
		else
			done = true;
	}
	return tf;
}

TForm
tfFollowSubst(TForm tf)
{
	Bool	done = (tf == NULL);

	while (!done) {
		if (tfIsForward(tf))
			tf = tf->argv[0];
		else if (tfIsSubst(tf))
			tf = tf->argv[0];
		else if (tfIsTrigger(tf))
			libGetAllSymes(tfTriggerLib(tf));
		else
			done = true;
	}
	return tf;
}

TForm
tfFollowFn(TForm tf)
{
	Bool	done = (tf == NULL);

	while (!done) {
		if (tfIsForward(tf))
			tf = tf->argv[0];
		else if (tfIsSubst(tf))
			tf = tfSubstPush(tf);
		else if (tfIsTrigger(tf))
			libGetAllSymes(tfTriggerLib(tf));
		else
			done = true;
	}
	return tf;
}

TForm
tfFollowArg(TForm tf, Length i)
{
	assert(!tfIsForward(tf));
	if (i < tfArgc(tf)) {
		tfArgv(tf)[i] = tfFollowOnly(tfArgv(tf)[i]);
		return tfArgv(tf)[i];
	}
	else
		return NULL;
}

/*
 * tfRaw
 */

local Bool
tfIsRawMap(TForm tf)
{
	return	tfIsMap(tf) && tfMapArgc(tf) == 1 &&
		tfIsSelf(tfDefineeType(tfMapArgN(tf, int0)));
}

TForm
tfRawType(TForm tf)
{
	SymeList	sl;

	for (sl = tfGetDomExports(tf); sl; sl = cdr(sl)) {
		Syme	syme = car(sl);
		if (symeId(syme) == symIntern("RawType") &&
		    tfIsRawMap(symeType(syme)))
			return tfMapRet(symeType(syme));
	}
	return tf;
}

/*
 * tfVariable
 */

TForm
tfSetVariable(TForm var, TForm val)
{
	assert(tfIsVariable(var));
	var->tag	= TF_Forward;
	var->argv[0]	= val;
	return var;
}

/*
 * tfIf
 */

TForm
tfIf(TForm test, TForm thenAlt, TForm elseAlt)
{
	TForm	tf;
	tfFollow(test);
	tfFollow(thenAlt);
	tfFollow(elseAlt);
	tf = tfNewNode(TF_If, 3, test, thenAlt, elseAlt);
	return tf;
}

/*
 * tfJoin
 */

TForm
tfJoin(Length argc, ...)
{
	TForm	tf;
	Length	i;
	va_list argp;

	tf = tfNewEmpty(TF_Join, argc);

	va_start(argp, argc);
	for (i = 0; i < argc; i += 1)
		tfArgv(tf)[i] = va_arg(argp, TForm);
	va_end(argp);

	return tf;
}

TForm
tfJoinFrList(TFormList tl0)
{
	Length		i, j, argc;
	TFormList	tl;
	TForm		tf;

	for (tl = tl0, argc = 0; tl; tl = cdr(tl))
		argc += tfIsJoin(car(tl)) ? tfJoinArgc(car(tl)) : 1;

	tf = tfNewEmpty(TF_Join, argc);
	for (tl = tl0, i = 0; tl; tl = cdr(tl))
		if (tfIsJoin(car(tl)))
			for (j = 0; j < tfJoinArgc(car(tl)); j += 1)
				tfArgv(tf)[i++] = tfArgv(car(tl))[j];
		else
			tfArgv(tf)[i++] = car(tl);

	tfSetMeaningArgs(tf);
	assert(tfIsMeaning(tf));

	return tf;
}

/*
 * tfMeet
 */

TForm
tfMeet(Length argc, ...)
{
	TForm	tf;
	Length	i;
	va_list argp;

	tf = tfNewEmpty(TF_Meet, argc);

	va_start(argp, argc);
	for (i = 0; i < argc; i += 1)
		tfArgv(tf)[i] = va_arg(argp, TForm);
	va_end(argp);

	return tf;
}

/*
 * tfHas
 */

AbSyn
abHas(Syme syme, TForm tf)
{
	return abNewHas(sposNone, abFrSyme(syme), tfExpr(tfDefineeType(tf)));
}

TForm
tfHas(Syme syme, TForm tf)
{
	return tfNewAbSyn(TF_General, abHas(syme, tf));
}

/*
 * tfExcept
 */

TForm
tfExcept(TForm t, TForm e)
{
	TForm tf;
	tf = tfNewNode(TF_Except, 2, t, e);
	tfSetMeaningArgs(tf);
	return tf;
}

TForm
tfIgnoreExceptions(TForm tf)
{
	tf = tfFollowOnly(tf);
	if (tfIsExcept(tf))
		return tfExceptType(tf);
	
	return tf;
}

/*
 * tfExtend
 */

local TForm	tfJoinFlatten		       (TForm tf);

Bool
tfIsExtendTemplate(TForm tf)
{
	if (tfIsDeclare(tf))
		tf = tfDeclareType(tf);

	return tfIsUnknown(tf) || tfIsUnknownMap(tf);
}

Bool
tfCanExtend(TForm tf, TForm tmpl)
{
	Bool	result;

	if (tfIsDeclare(tmpl))
		tmpl = tfDeclareType(tmpl);

	if (tfIsAnyMap(tmpl))
		result = tfIsAnyMap(tf) && tfMapArgc(tf) == tfMapArgc(tmpl);

	else
		result = !tfIsAnyMap(tf);

	return result;
}

TForm
tfExtendEmpty(TForm tmpl, Length argc)
{
	TForm	tf;

	if (tfIsDeclare(tmpl))
		tmpl = tfDeclareType(tmpl);

	if (tfIsAnyMap(tmpl)) {
		Length		i, targc = tfMapArgc(tmpl);
		TForm		tfarg, tfret;

		if (targc == 1)
			tfarg = tfNewEmpty(TF_Meet, argc);
		else {
			tfarg = tfNewEmpty(TF_Multiple, targc);
			for (i = 0; i < targc; i += 1)
				tfArgv(tfarg)[i] = tfNewEmpty(TF_Meet, argc);
		}
		tfret = tfNewEmpty(TF_Join, argc);
		tf = tfAnyMap(tfarg, tfret, tfIsPackedMap(tmpl));
		tfSetStab(tf, tfStab(tmpl));
	}
	else
		tf = tfNewEmpty(TF_Join, argc);

	return tf;
}

Length
tfExtendNext(TForm tf)
{
	Length	i;

	if (tfIsAnyMap(tf)) tf = tfMapRet(tf);

	for (i = 0; i < tfArgc(tf) && tfArgv(tf)[i]; i += 1) ;

	return i;
}

#define			tfExtendCheck(tag,tf,i)	\
	(tfTag(tf) == (tag) && (i) < tfArgc(tf) && tfArgv(tf)[i] == NULL)

void
tfExtendFill(TForm tf, Length i, TForm ext)
{
	if (tfIsAnyMap(tf)) {
		Length		j;

		assert(tfIsAnyMap(ext));
		assert(tfMapArgc(tf) == tfMapArgc(ext));

		/* Fill the arg types w/ the arg types from the extendees. */
		for (j = 0; j < tfMapArgc(tf); j += 1) {
			TForm	tfj = tfMapArgN(tf, j);
			assert(tfExtendCheck(TF_Meet, tfj, i));
			tfArgv(tfj)[i] = tfMapArgN(ext, j);
		}

		/* Fill the ret types w/ the ret types from the extendees. */
		assert(tfExtendCheck(TF_Join, tfMapRet(tf), i));
		tfArgv(tfMapRet(tf))[i] = tfMapRet(ext);
	}
	else {
		assert(tfExtendCheck(TF_Join, tf, i));
		tfArgv(tf)[i] = ext;
	}
}

TForm
tfExtendeeSubst(Stab stab, TForm tf, TForm tmpl)
{
	Length		i;
	AbSub		sigma;

	if (tfIsDeclare(tmpl))
		tmpl = tfDeclareType(tmpl);
	assert(tfIsAnyMap(tf) && tfIsAnyMap(tmpl) &&
	       tfMapArgc(tf) == tfMapArgc(tmpl));

	/* Replace old parameter symes with new symes for the extendee. */
	sigma = absNew(stab);
	for (i = 0; i < tfMapArgc(tf); i += 1) {
		Syme	osyme = tfDefineeSyme(tfMapArgN(tf, i));
		Syme	nsyme = tfDefineeSyme(tfMapArgN(tmpl, i));

		assert(osyme && nsyme);
		sigma = absExtend(osyme, abFrSyme(nsyme), sigma);
	}

	tf = tformSubstSigma(sigma, tfMapRet(tf));
	tmpl = tfMapRet(tmpl);
	absFree(sigma);

	if (tfIsAnyMap(tf))
		tf = tfExtendeeSubst(stab, tf, tmpl);

	return tf;
}

void
tfExtendSubst(Stab stab, TForm tf)
{
	Length		i, j;
	TForm		tfarg, tfret, ntf, ntfret;
	AbSub		sigma;
	SymeList	symes;

	if (!tfIsAnyMap(tf))
		return;

	tfarg = tfMapArg(tf);
	tfret = tfMapRet(tf);

	assert(tfIsJoin(tfret) && tfJoinArgc(tfret) > 0);

	/* Replace old parameter symes with new symes for the extension. */
	assert(tfStab(tf));
	sigma = absNew(tfStab(tf));
	symes = listNil(Syme);
	for (j = 0; j < tfMapArgc(tf); j += 1) {
		TForm	tfj = tfMapArgN(tf, j);
		Syme	osyme = NULL, nsyme;
		AbSyn	ab;

		assert(tfIsMeet(tfj) && tfMeetArgc(tfj) > 1);

		/* Create the new param syme from an extension param syme. */
		for (i = 1; !osyme && i < tfMeetArgc(tfj); i += 1)
			osyme = tfDefineeSyme(tfMeetArgv(tfj)[i]);
		assert(osyme);
		symeType(osyme);
		nsyme = symeCopy(osyme);
		symeSetLib(nsyme, NULL);
		symeSetHash(nsyme, (Hash) 0);
		ab = abFrSyme(nsyme);

		for (i = 0; i < tfMeetArgc(tfj); i += 1) {
			TForm	tfi = tfMeetArgv(tfj)[i];
			Syme	syme = tfDefineeSyme(tfi);

			if (syme) {
				symeAddTwin(nsyme, syme);
				sigma = absExtend(syme, ab, sigma);
			}
		}

		symes = listCons(Syme)(nsyme, symes);
	}
	symes = listNReverse(Syme)(symes);
	tfSetSymes(tf, symes);

	ntf = tformSubstSigma(sigma, tf);
	absFree(sigma);

	/* Replace old types with new types for the extension. */
	tfFollow(ntf);
	assert(tfIsAnyMap(ntf));
	for (j = 0; j < tfMapArgc(tf); j += 1) {
		TForm	otfj = tfMapArgN(tf, j);
		TForm	ntfj = tfMapArgN(ntf, j);

		tfFollow(ntfj);
		assert(tfIsMeet(ntfj) && tfMeetArgc(ntfj) == tfMeetArgc(otfj));

		for (i = 0; i < tfMeetArgc(otfj); i += 1)
			tfMeetArgv(otfj)[i] = tfMeetArgv(ntfj)[i];
		tfSetFVars(otfj, NULL);
	}

	ntfret = tfMapRet(ntf);
	tfFollow(ntfret);
	assert(tfIsJoin(ntfret) && tfJoinArgc(ntfret) == tfJoinArgc(tfret));

	for (i = 0; i < tfJoinArgc(tfret); i += 1)
		tfJoinArgv(tfret)[i] = tfJoinArgv(ntfret)[i];
	tfSetFVars(tfret, NULL);
	tfSetFVars(tf, NULL);
}

local TForm
tfIsIdempotent(TForm tf)
{
	TForm	tf0 = NULL, tf1 = NULL;
	Length	i, argc;

	assert(tfIsJoin(tf) || tfIsMeet(tf));

	argc = tfArgc(tf);

	for (i = 0; !tf1 && i < argc; i += 1) {
		TForm	tfi = tfArgv(tf)[i];
		if (tf0 == NULL)
			tf0 = tfi;
		else if (!tformEqual(tf0, tfi))
			tf1 = tfi;
	}

	if (tf0 == NULL)
		return tfArgv(tf)[0];
	else if (tf1 == NULL)
		return tf0;
	else
		return NULL;
}

local void
tfForwardIdempotent(TForm tf, TForm ntf)
{
	assert(tfIsJoin(tf) || tfIsMeet(tf));

	tf->tag		= TF_Forward;
	tf->argc	= 1;
	tf->argv[0]	= ntf;
}

void
tfExtendFinish(TForm tf)
{
	Length		i, j;
	TForm		tfarg, tfret, ntfj;
	SymeList	symes;

	if (!tfIsAnyMap(tf)) {
		Stab	stab = NULL;

		assert(tfIsJoin(tf) && tfJoinArgc(tf) > 0);
		for (j = tfJoinArgc(tf); !stab && j > 0; ) {
			TForm	tfj = tfJoinArgN(tf, --j);
			stab = tfGetStab(tfDefineeType(tfj));
			if (!stab && tfIsDefine(tfj))
				stab = tfGetStab(tfDefineVal(tfj));
		}
		if (!stab) stab = stabFile();
		
		tf = tfJoinFlatten(tf);
		tfSetMeaning(tf);
		for (symes = tfGetCatExports(tf); symes; symes = cdr(symes))
			tfExtendFinishTwins(stab, car(symes));
		return;
	}

	tfarg = tfMapArg(tf);
	tfret = tfMapRet(tf);

	assert(tfIsJoin(tfret) && tfJoinArgc(tfret) > 0);

	for (j = tfMapArgc(tf); j > 0; ) {
		TForm	tfj = tfMapArgN(tf, --j);
		Syme	syme = NULL;
		Length	argc;

		assert(tfIsMeet(tfj) && tfMeetArgc(tfj) > 0);
		argc = tfMeetArgc(tfj);

		/* If all args are the same, forward to the first. */
		if ((ntfj = tfIsIdempotent(tfj)) != NULL) {
			tfForwardIdempotent(tfj, ntfj);
			tfSetMeaning(tfj);
			continue;
		}

		/* Otherwise conditionalize the ret types. */
		for (i = 0; !syme && i < argc; i += 1)
			syme = tfDefineeSyme(tfMeetArgv(tfj)[i]);
		assert(syme);
		for (i = 0; i < tfJoinArgc(tfret); i += 1) {
			TForm	tfi = tfJoinArgv(tfret)[i];
			TForm	ifi = tfMeetArgv(tfj)[i];
			ifi = tfDefineeType(ifi);
			if (tfIsType(ifi))
				ifi = tfi;
			else
				ifi = tfIf(tfHas(syme, ifi), tfi, tfNone());
			tfJoinArgv(tfret)[i] = ifi;
			tfSetMeaning(ifi);
		}

		/* ... and commute Meet v. Declare. */
		ntfj = tfNewEmpty(TF_Meet, argc);
		for (i = 0; i < argc; i += 1)
			tfArgv(ntfj)[i] = tfDefineeType(tfMeetArgv(tfj)[i]);
		tfSetMeaning(ntfj);
		symeSetUsedDeeply(syme);
		ntfj = tfDeclare(abFrSyme(syme), ntfj);
		tfForwardIdempotent(tfj, ntfj);
		tfSetMeaning(tfj);
	}

	tfret = tfJoinFlatten(tfret);

	if ((ntfj = tfIsIdempotent(tfret)) != NULL)
		tfForwardIdempotent(tfret, ntfj);

	tfSetMeaning(tfarg);
	tfSetMeaning(tfret);
	tfSetMeaning(tf);

	assert(tfStab(tf));
	for (symes = tfGetCatExports(tfret); symes; symes = cdr(symes))
		tfExtendFinishTwins(tfStab(tf), car(symes));
}

local void
tfExtendFinishTwins(Stab stab, Syme syme)
{
	SymeList	symes = symeTwins(syme);

	if (syme == symeOriginal(syme)) return;
	if (symeIsLazy(syme)) symeType(syme);

	if (!symeIsSelfSelf(syme)) {
		symeSetLib(syme, NULL);
		symeSetHash(syme, (Hash) 0);
		symeSetDefLevel(syme, car(stab));
	}
	for (; symes; symes = cdr(symes)) {
		Syme	twin = car(symes);
		if (twin != syme) tfExtendFinishTwins(stab, twin);
	}
}

local TForm
tfJoinFlatten(TForm tf)
{
	TForm tf2;
	int  argc, i, ii, j;

	for (i=0, argc=0; i<tfJoinArgc(tf); i++) 
		argc += tfIsJoin(tfJoinArgv(tf)[i]) ? tfJoinArgc(tfJoinArgv(tf)[i]) : 1;
	
	if (argc == i) 
		return tf;

	tf2 = tfNewEmpty(TF_Join, argc);
	for (i=0, j=0; i<tfJoinArgc(tf); i++) {
		TForm tfi = tfJoinArgv(tf)[i];
		if (!tfIsJoin(tfi))
			tfJoinArgv(tf2)[j++] = tfi;
		else 
			for (ii=0; ii<tfJoinArgc(tfi); ii++) 
				tfJoinArgv(tf2)[j++] = tfJoinArgv(tfi)[ii];
	}
	/* Forward tf to tf2 */
	tfTag(tf) = TF_Forward;
	tf->argc  = 1;
	tf->argv[0] = tf2;
	return tf2;
}


/*
 * Given a symbol, return its unique tform or NULL if not possible.
 * This is really only applicable for types where we can be certain
 * of name uniqueness.
 */
TForm
tfFrSymbol(Symbol sym)
{
	/* Get the meanings for this symbol */
	SymeList symes  = stabGetMeanings(stabFile(), ablogFalse(), sym);


	/* There can only be one ... */
	if (symes && !cdr(symes))
		return tiTopFns()->tiGetTopLevelTForm(NULL, abFrSyme(car(symes)));
	else
		return (TForm)NULL;
}


/*
 * Return the tform for a functor symbol applied to an argument
 * This is really only applicable for types where we can be certain
 * of name uniqueness.
 */
TForm
tfFrSymbolPair(Symbol functor, Symbol argument)
{
	/* Get the meanings for these symbols */
	SymeList fsymes	= stabGetMeanings(stabFile(), ablogFalse(), functor);
	SymeList asymes	= stabGetMeanings(stabFile(), ablogFalse(), argument);
	AbSyn op	= abNewOfToken(AB_Id, tokId(sposNone, sposNone, functor));
	AbSyn arg	= abNewOfToken(AB_Id, tokId(sposNone, sposNone, argument));
	AbSyn ab	= abNewApply1(sposNone, op, arg);


	/*
	 * Construct a tform for the expression functor(arg).
	 * If we can't find meanings for either the functor or the
	 * argument symbols then they can't have been defined. We
	 * won't return tforms for this pair otherwise the type
	 * inference phase of tiGetTForm() below will scream. This
	 * is to allow libraries to be defined which don't necessarily
	 * have the requested value defined anywhere.
	 */
	if (!fsymes || !asymes)
		return tfUnknown; /* Please don't return (TForm)NULL */
	else
		return tiTopFns()->tiGetTopLevelTForm(NULL, ab);
}

/*****************************************************************************
 *
 * :: Types of operations used in syntactic sugar
 *
 ****************************************************************************/

/*
 * LitOpType
 */

Bool
tfIsLitOpType(TForm tf)
{
	tfFollow(tf);
	return tfIsAnyMap(tf) &&
	       tfMapArgc(tf) == 1 &&
	       tfEqual(tfMapArgN(tf, int0), tfLiteral);
}

TForm
tfLitOpTypeRet(TForm tf)
{
	tfFollow(tf);
	if (!tfIsLitOpType(tf))
		bug("tfLitOpTypeRet:  !tfIsLitOpType(tf)");

	return tfMapRet(tf);
}

/*
 * TestOpType
 */

Bool
tfIsTestOpType(TForm tf)
{
	tfFollow(tf);
	return tfIsAnyMap(tf) &&
	       tfMapArgc(tf) == 1 &&
	       tfEqual(tfMapRet(tf), tfTest);
}

TForm
tfTestOpTypeArg(TForm tf)
{
	tfFollow(tf);
	if (!tfIsTestOpType(tf))
		bug("tfTestOpTypeArg:  !tfIsTestOpType(tf)");

	return tfMapArg(tf);
}

/*
 * GeneratorOpType
 */

Bool
tfIsGeneratorOpType(TForm tf)
{
	tfFollow(tf);
	return tfIsAnyMap(tf) &&
		tfIsGenerator(tfMapRet(tf));
}

TForm
tfGeneratorOpTypeArg(TForm tf)
{
	tfFollow(tf);
	if (!tfIsGeneratorOpType(tf))
		bug("tfGeneratorOpTypeArg:  !tfIsGeneratorOpType(tf)");

	return tfMapArg(tf);
}

TForm
tfGeneratorOpTypeRet(TForm tf)
{
	tfFollow(tf);
	if (!tfIsGeneratorOpType(tf))
		bug("tfGeneratorOpTypeRet:  !tfIsGeneratorOpType(tf)");

	return tfGeneratorArg(tfMapRet(tf));
}

Bool
abUseIsDContext(AbSyn ab)
{
	return	abUse(ab) == AB_Use_NoValue ||
		abUse(ab) == AB_Use_Type ||
		abUse(ab) == AB_Use_Declaration;
}

Bool
tfIsTypeTuple(TForm tf)
{
	return tfIsTuple(tf) && tfIsType(tfTupleArg(tf));
}

Bool
tfIsCategoryContext(TForm tf, AbSyn ab)
{
	if (tfIsUnknown(tf) || tfIsNone(tf))
		return false;

	if (tfIsCategory(tf) || tfIsTypeTuple(tf))
		return true;

	if (abUseIsDContext(ab))
		return tfSatCat(tf) || tfSatisfies(tf, tfTypeTuple);

	return false;
}

Bool
tfIsNoValueContext(TForm tf, AbSyn ab)
{
	return	abUseIsDContext(ab) ||
		tfIsNone(tf) ||
		tfIsUnknown(tf);
}

/******************************************************************************
 *
 * :: Implict category stuff
 *
 *****************************************************************************/

TForm tfImplPAOps = (TForm)NULL; /* DenseStorageCategory */

void
tfInitImplicit(void)
{
	static	Bool	isInit = false;

	if (isInit) return;


	/* Start with the easy one */
	tfImplPAOps = tfFrSymbol(ssymImplPAOps);
}


TForm
tfCatFrDom(TForm tf)
{
	/* Walk past the declaration to get the type */
	if (tfIsDeclare(tf))
		 tf = tfDeclareType(tf);


	/* Do we have the absyn for this domain? */
	if (!tfHasExpr(tf))
		return (TForm)NULL;


	/* Return the category of this domain (if possible) */
	return abGetCategory((AbSyn)tfGetExpr(tf));
}


Bool
tfDomHasImplicit(TForm tf)
{
	TForm	tfcat = tfCatFrDom(tf);


	/* Did we manage to get the category? */
	if (!tfcat) return false;


	/* Ensure that we have the necessary tforms available */
	tfInitImplicit();


	/* Paranoia - do we have the required tform? */
	if (!tfImplPAOps || tfIsUnknown(tfImplPAOps)) return false;


	/* Does this domain satisfy the category? */
	return tfSatBit(tfSatHasMask(), tfcat, tfImplPAOps);
}


Bool
tfCatHasImplicit(TForm tf)
{
	/* Walk past the declaration to get the type */
	if (tfIsDeclare(tf))
		tf = tfDeclareType(tf);


	/* Ensure that we have the necessary tforms available */
	tfInitImplicit();


	/* Paranoia - do we have the required tform? */
	if (!tfImplPAOps || tfIsUnknown(tfImplPAOps)) return false;


	/* Does this domain satisfy the category? */
	return tfSatBit(tfSatHasMask(), tf, tfImplPAOps);
}


/*
 * If this syme is one of the correct exports then return it
 * otherwise return nothing.
 */
Syme
tfImplicitExport(Stab stab, SymeList mods, Syme syme)
{
	Syme		nsyme = (Syme)NULL;
	SymeList	isymes;
	Symbol		sym = symeId(syme);
	TForm		tf = symeType(syme);


	/* Ensure that the necessary tforms are available */
	tfInitImplicit();


	/* Paranoia - do we have the required tform? */
	if (!tfImplPAOps || tfIsUnknown(tfImplPAOps)) return nsyme;


	/* Extract the symes that we are looking for */
	isymes = tfGetCatExports(tfImplPAOps);


	/* Check to see if it is in the list */
	for (; isymes; isymes = cdr(isymes))
	{
		Syme xsyme = car(isymes);

		if (symeId(xsyme) != sym) continue;
		if (!tformEqualMod(mods, symeType(xsyme), tf)) continue;

#if STAB_DEF_IMPLICIT
		syme = stabDefExport(stab, sym, tf, (Doc)NULL);
#endif
		return syme; /* not xsyme ... */
	}


	/* Not one of the implicit packed array operations */
	return nsyme;
}

/*********************************
 * 
 * :: Conditions
 *
 *********************************/
extern void   
tfMergeConditions(TForm tf, Stab stab, TfCondElt conditions)
{
	if (DEBUG(tf)) {
		if (conditions != NULL)
			afprintf(dbOut, "Merge condition %pAbSynList to %pTForm\n", 
				 conditions->list, tf);
	}
	tfSetConditions(tf, tfCondMerge(tfConditions(tf), stab, conditions));
}

extern TfCond tfFloatConditions(Stab stab, TForm tf)
{
	if (tfConditions(tf) != NULL) {
		TfCond cond = tfCondFloat(stab, tfConditions(tf));
		tfSetConditions(tf, cond);
	}

	return tfConditions(tf);
}

extern 
TfCond tfConditions(TForm tf)
{
	tfFollow(tf);
	return tf->conditions;
}


AbSynList
tfConditionalAbSyn(TForm tf)
{
	if (tfConditions(tf) == NULL)
		return listNil(AbSyn);
	if (tfConditions(tf)->containsEmpty) 
		return listNil(AbSyn);

	if (DEBUG(tf)) {
		TfCondEltList list;
		list = tfConditions(tf)->conditions;
		while (list != listNil(TfCondElt)) {
			afprintf(dbOut, "Condition: %pTForm %pAbSynList\n", tf, car(list)->list);
			list = cdr(list);
		};
	}
	return tfConditions(tf)->conditions->first->list;
}

Stab
tfConditionalStab(TForm tf)
{
	if (tfConditions(tf) == NULL)
		return NULL;
	if (tfConditions(tf)->containsEmpty) 
		return NULL;

	return tfConditions(tf)->conditions->first->stab;
}

/******************************************************************************
 *
 * :: Table of information about type form tags
 *
 *****************************************************************************/

#define	 TF_NARY	(-1)	/* Identifies tags with N-ary data argument. */

struct tform_info tformInfoTable[] = {

	/* Special type form symbols */
	{TF_Unknown,	"Unknown",      "Unknown",      0, 0},
	{TF_Exit,	"Exit",         "Exit",         0, 0},
	{TF_Literal,	"Literal",      "Literal",      0, 0},
	{TF_Test,	"Test",         "Test",         0, 0},
	{TF_Type,	"Type",         "Type",         0, 0},
	{TF_Category,	"Category",     "Category",     0, 0},

	/* Type forms described by abstract syntax */
	{TF_Syntax,	"Syntax",       "Syntax",       0, 0},
	{TF_General,	"General",      "General",      0, 0},

	/* Type forms described by subordinate type forms */
	{TF_Add,	"Add",          "Add",          0, 0},
	{TF_Assign,	"Assign",       ":=",           0, 1},
	{TF_Cross,	"Cross",        "Cross",        0, TF_NARY},
	{TF_Declare,	"Declare",      ":",            0, 1},
	{TF_Default,	"Default",      "==",           0, 2},
	{TF_Define,	"Define",       "==",           0, 2},
	{TF_Enumerate,	"Enumeration",  "Enumeration",  0, 1},
	{TF_Forward,	"Forward",      "Forward",      0, 1},
	{TF_Generator,	"Generator",    "Generator",    0, 1},
	{TF_If,		"If",           "If",           0, 3},
	{TF_Instance,	"Instance",     "Instance",     0, 2},
	{TF_Join,	"Join",         "Join",         0, TF_NARY},
	{TF_Map,	"Map",          "->",           0, 2},
	{TF_Meet,	"Meet",         "Meet",         0, TF_NARY},
	{TF_Multiple,	"Multiple",	"Multiple",     0, TF_NARY},
	{TF_PackedMap,	"PackedMap",	"->*",          0, 2},
	{TF_Raw,	"Raw",		"Raw",          0, 1},
	{TF_RawRecord,	"RawRecord",    "RawRecord",    0, 0},
	{TF_Record,	"Record",       "Record",       0, 0},
	{TF_Reference,	"Reference",    "Ref",		0, 1},
	{TF_Subst,	"Subst",        "Subst",        0, 1},
	{TF_Third,	"Third",        "Third",        0, 2},
	{TF_Trigger,	"Trigger",      "Trigger",      0, 1},
	{TF_TrailingArray,"TrailingArray",    "TrailingArray",0, 1},
	{TF_Tuple,	"Tuple",        "Tuple",        0, 1},
	{TF_Union,	"Union",        "Union",        0, 0},
	{TF_Variable,	"Variable",     "Variable",     0, 1},
	{TF_With,	"With",         "With",         0, 2},
	{TF_Except,	"Except",       "Except",       0, 2},

	{TF_LIMIT,	"LIMIT",        "LIMIT",        0, 0}
};
