/******************************************************************************
 *
 * scobind.c: Deduce the scopes of identifiers.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 *****************************************************************************/

#include "abpretty.h"
#include "abuse.h"
#include "comsg.h"
#include "debug.h"
#include "doc.h"
#include "fint.h"
#include "format.h"
#include "lib.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "strops.h"
#include "symcoinfo.h"
#include "table.h"
#include "tfcond.h"
#include "tposs.h"
#include "tqual.h"
#include "util.h"


Bool	scoDebug	= false;
Bool	scoStabDebug	= false;
Bool	scoFluidDebug	= false;
Bool	scoUndoDebug	= false;

#define scoDEBUG	DEBUG_IF(sco)		afprintf
#define scoStabDEBUG	DEBUG_IF(scoStab)	afprintf
#define scoFluidDEBUG	DEBUG_IF(scoFluid)	afprintf
#define scoUndoDEBUG	DEBUG_IF(scoUndo)	afprintf

/******************************************************************************
 *
 * :: Identifier contexts
 *
 *****************************************************************************/

enum idContext {
    SCO_Id_START,
	SCO_Id_InType = SCO_Id_START,
	SCO_Id_Label,
	SCO_Id_Used,
    SCO_Id_LIMIT
};

typedef Enum(idContext)		IdContext;

static char *IdContextNames[] = {
	"InType",
	"Label",
	"Used"
};

/******************************************************************************
 *
 * :: Declaration contexts
 *
 *****************************************************************************/

enum declContext {
    SCO_Sig_START,
	SCO_Sig_Assign = SCO_Sig_START,
	SCO_Sig_Builtin,
	SCO_Sig_Declare,
	SCO_Sig_Default,
	SCO_Sig_Define,
	SCO_Sig_DDefine,
	SCO_Sig_Export,
	SCO_Sig_Extend,
	SCO_Sig_Fluid,
	SCO_Sig_Foreign,
	SCO_Sig_Free,
	SCO_Sig_FreeRef,
	SCO_Sig_ImplicitLocal,
	SCO_Sig_Import,
	SCO_Sig_Inline,
	SCO_Sig_InType,
	SCO_Sig_Local,
	SCO_Sig_Loop,
	SCO_Sig_Param,
	SCO_Sig_Used,
	SCO_Sig_Reference,
	SCO_Sig_Value,
    SCO_Sig_LIMIT
};

typedef Enum(declContext)	DeclContext;

static char *DeclContextNames[] = {
	"Assign",
	"Builtin",
	"Declare",
	"Default",
	"Define",
	"DDefine",
	"Export",
	"Extend",
	"Fluid",
	"Foreign",
	"Free",
	"FreeReference",
	"ImplicitLocal",
	"Import",
	"Inline",
	"InType",
	"Local",
	"Loop",
	"Param",
	"Used",
	"Referenced",
	"Value"
};

/******************************************************************************
 *
 * :: Declaration information
 *
 *****************************************************************************/

typedef AIntList 	DefnPos;

DECLARE_LIST(DefnPos);
CREATE_LIST(DefnPos);

typedef struct decl_info {
	UShort		intStepNo;
	AbSyn		id;
	AbSyn		type;
	DefnPosList	defpos;
	Doc		doc;
	AbSyn		uses[SCO_Sig_LIMIT];
}      *DeclInfo;

DECLARE_LIST(DeclInfo);
CREATE_LIST(DeclInfo);

/* A condition.  We keep the stab information so its depth can be compared
 * to the tform depth in tfFloat
 */

typedef struct ScoCondition {
	Stab  stab;
	AbSyn absyn;
	Bool  negate;
} *ScoCondition;

DECLARE_LIST(ScoCondition);
CREATE_LIST(ScoCondition);

/******************************************************************************
 *
 * :: Identifier information
 *
 *****************************************************************************/

typedef struct id_info {
	BPack(Bool)	allFree;
	BPack(Bool)	allLocal;
	BPack(Bool)	allFluid;
	ULong		serialNo;
	UShort		intStepNo;
	Symbol		sym;
	AbSyn		uses[SCO_Id_LIMIT];
	DeclInfoList	declInfoList;
	AbSyn		defaultType;
	AbSyn		exampleId;
	AbSynList	usePreDef;
}      *IdInfo;

DECLARE_LIST(IdInfo);
CREATE_LIST(IdInfo);

typedef struct lambda_info {
	AbSyn lhs;
	AbSyn rhs;
	ScoConditionList scoCondList;
} *LambdaInfo;

DECLARE_LIST(LambdaInfo);
CREATE_LIST(LambdaInfo);

local LambdaInfo lambdaInfoAlloc(AbSyn lhs, AbSyn rhs, ScoConditionList condition);
local void       lambdaInfoFree(LambdaInfo info);

/******************************************************************************
 *
 * :: Conditional Definitions
 *
 *****************************************************************************/

local DefnPos 	defposTail	(DefnPos pos);
local Bool    	defposEqual	(DefnPos a, DefnPos b);
local Bool      defposIsRoot	(DefnPos pos);
local void      defposFree      (DefnPos pos);
local AbSynList defposToAbSyn(AIntList defnPos);
/* (ToDo: Rename above functions to defnposXXX) */

/******************************************************************************
 *
 * :: Static variables
 *
 *****************************************************************************/

static Stab		scoStab;
static Bool		scoIsInType;
static Bool		scoIsInAdd;
static Bool		scoIsInExport;
static Bool		scoIsInExtend;
static Bool		scoIsInImport;
static AbSynList	scoDefineList;
static LambdaInfoList	scoLambdaList;
static IdInfoList	scoIdInfoList;
static Bool		scoUndoState;
static SymeList		scoUndoSymes;
static TFormList	scoUndoTForms;
static ScoConditionList	scoCondList;
static int 		scoDefCounter;

/******************************************************************************
 *
 * :: Local operations
 *
 *****************************************************************************/

typedef void		(*ScoBindFun)		(AbSyn);

local void		scobindValue		(AbSyn);
local void		scobindContext		(AbSyn);
local void		scobindType		(AbSyn);
local void		scobindLevel		(AbSyn, ScoBindFun, ULong);

local void		scobindLambdaList	(void);
local void		scobindPushDefine	(AbSyn);
local void		scobindPopDefine	(AbSyn);
local void		scobindSetStab		(AbSyn, ULong);
local void		scobindStabHash		(AbSyn, Stab);

local void		scobindApply		(AbSyn);
local void		scobindAssign		(AbSyn);
local void		scobindBuiltin		(AbSyn);
local void		scobindDeclare		(AbSyn);
local void		scobindDefault		(AbSyn);
local void		scobindDefine		(AbSyn);
local void		scobindDDefine		(AbSyn);
local void		scobindExport		(AbSyn);
local void		scobindExtend		(AbSyn);
local void		scobindFluid		(AbSyn);
local void		scobindFor		(AbSyn);
local void		scobindForeignImport	(AbSyn);
local void		scobindForeignExport	(AbSyn);
local void		scobindFree		(AbSyn);
local void		scobindImport		(AbSyn);
local void		scobindInline		(AbSyn);
local void		scobindLocal		(AbSyn);
local void		scobindReference	(AbSyn);
local void		scobindParam		(AbSyn);
local void		scobindTry		(AbSyn);
local void		scobindIf		(AbSyn);
local void		scobindAnd		(AbSyn);

local void		scobindApplySelf	(AbSyn);
local Bool		scobindApplyNeedsSelf	(AbSyn);
local Bool		scobindApplyNeedsScope	(AbSyn);
local Bool		scobindApplyArgNeedsScope(AbSyn);
local void		scobindApplyScope	(AbSyn);
local void		scobindApplyParam	(AbSyn);
local void		scobindApplyComma	(AbSyn);
local void		scobindApplyArg		(AbSyn);
local void		scobindAssignDeclare	(AbSyn);
local void		scobindAssignId		(AbSyn, AbSyn);
local void		scobindBuiltinDeclare	(AbSyn);
local void		scobindBuiltinId	(AbSyn, AbSyn);
local void		scobindDefaultDeclare	(AbSyn);
local void		scobindDefaultId	(AbSyn, AbSyn);
local void		scobindDefineDeclare	(AbSyn, AbSyn);
local void		scobindDefineId		(AbSyn, AbSyn, AbSyn);
local void		scobindDefineRhs	(AbSyn, AbSyn, DeclContext);
local void		scobindDDefineComma	(AbSyn);
local void		scobindDDefineDeclare	(AbSyn, AbSyn);
local void		scobindDDefineId	(AbSyn, AbSyn, AbSyn);
local void		scobindExportTo		(AbSyn, AbSyn);
local void		scobindExportFrom	(AbSyn, AbSyn);
local void		scobindExportWhat	(AbSyn);
local void		scobindExportDeclare	(AbSyn, AbSyn);
local void		scobindExportId		(AbSyn, AbSyn, AbSyn);
local void		scobindExtendDeclare	(AbSyn, AbSyn);
local void		scobindExtendId		(AbSyn, AbSyn, AbSyn);
local Syme		scobindGetExtend	(AbSyn, AbSyn);
local void		scobindFluidComma	(AbSyn);
local void		scobindFluidDeclare	(AbSyn);
local void		scobindFluidId		(AbSyn, AbSyn);
local void		scobindFor0		(AbSyn, Bool);
local void		scobindForDeclare	(AbSyn, AbSyn);
local void		scobindForId		(AbSyn, AbSyn);
local void		scobindForeignDeclare	(AbSyn, ForeignOrigin);
local void		scobindForeignId	(AbSyn, AbSyn, ForeignOrigin);
local void		scobindParamDefine	(AbSyn, AbSyn);
local void		scobindParamDeclare	(AbSyn, AbSyn);
local void		scobindParamId		(AbSyn, AbSyn, AbSyn);

local TForm scobindTfSyntaxFrAbSyn(Stab stab, AbSyn ab);

/*
 * scobindDeclare
 */
local DeclInfo	scobindDeclareId	(AbSyn, AbSyn, AbSyn, DeclContext);
local TForm	scobindDeclareTForm	(Stab,  AbSyn, AbSyn, AbSyn, DeclContext);
local void	scobindUniqifyDecl	(AbSyn, AbSyn);
local void	scobindIntroduceId	(AbSyn, AbSyn, DeclContext);
local AbSyn	scobindGuessType	(AbSyn);

/*
 * scobindFree
 * scobindLocal
 */
local void		scobindLOF		(AbSyn, DeclContext);
local void		scobindLOFComma		(AbSyn, AbSyn, DeclContext);
local void		scobindLOFDeclare	(AbSyn, AbSyn, DeclContext);
local void		scobindLOFId		(AbSyn, DeclContext);
local void		scobindLOFType		(AbSyn, AbSyn, AbSyn,
						 DeclContext);
local void		scobindLOFDeclInfo	(AbSyn, IdInfo, DeclInfo,
						 DeclContext);

/*
 * Properties of different lexical levels.
 */
#define			scobindApplyFlags	0
#define			scobindLambdaFlags	0
#define			scobindAddFlags		STAB_LEVEL_LARGE
#define			scobindWithFlags	STAB_LEVEL_LARGE
#define			scobindWhereFlags	STAB_LEVEL_WHERE
#define			scobindRepeatFlags	STAB_LEVEL_LOOP
#define			scobindCollectFlags	STAB_LEVEL_LOOP

/*
 * scobindIf (and conditions)
 *
 * FIXME: This really needs some cleanup
 */
local void 	scobindIf	(AbSyn);
local void 	scoCondPush  	(Stab, AbSyn, Bool);
local void 	scoCondPop	(void);
local ScoConditionList  scoConditions	(void);
local DefnPos scoConditionToDefnPos(ScoConditionList);
local TfCondElt scoCondListCondElt();

/*
 * IdInfo
 */
local IdInfo		idInfoNew		(Stab, AbSyn);
local void		idInfoFree		(IdInfo);
local Bool		idInfoIsNew		(IdInfo);
local void		scobindFreeIdInfo	(Stab);
local IdInfoList	scobindSaveIdInfo	(Stab);
local void		scobindRestoreIdInfo	(Stab, IdInfoList, Bool);
local IdInfo		getIdInfoInAnyScope	(Stab, AbSyn);
local IdInfo		getIdInfoInThisScope	(Stab, Symbol);
local void		scobindSetIdUse		(IdInfo, IdContext, AbSyn);
local AbSyn		scobindDefaultType	(Stab, Symbol);

#define			idInfoCell(x)		\
	((IdInfoList) (symCoInfo(x)->phaseVal.generic))

#define			setIdInfoCell(x,y)	\
	(symCoInfo(x)->phaseVal.generic = (Pointer) (y))

/*
 * DeclInfo
 */
local DeclInfo		declInfoNew		(AbSyn, AbSyn, DefnPos);
local void		declInfoFree		(DeclInfo);
local Bool		declInfoIsNew		(DeclInfo);
local Bool		declInfoUseIsNew	(AbSyn);
local Bool		declInfoIsImplicitLocal	(DeclInfo);
local void		scobindRestoreDeclInfo	(IdInfo);
local DeclInfo		idInfoHasType		(IdInfo, AbSyn);
local DeclInfo		idInfoAddType		(IdInfo, AbSyn, AbSyn, AbSyn, ScoConditionList);
local void		scobindSetSigUse	(DeclInfo, DeclContext, AbSyn);
local Bool              scobindCheckCondition   (DeclInfo, ScoConditionList);
local Bool		scobindCheckDefnPos	(DeclInfo, DefnPos);

/*
 * scobindAddMeaning
 */
local void		scobindAddMeaning	(AbSyn, Symbol, Stab,
						 SymeTag, TForm, AInt);
local Bool		scobindNeedsMeaning	(AbSyn, TForm);
local void		scobindSetMeaning	(AbSyn, Syme);
local Syme		scobindDefMeaning	(Stab, SymeTag, Symbol,
						 TForm, AInt);
/*
 * scobindReconcile
 */
local void		scobindReconcile	(Stab,AbSynTag);
local void		scobindReconcileId	(Stab,AbSynTag,Symbol,IdInfo);
local void		scobindReconcileDecls	(Stab,AbSynTag,Symbol,IdInfo);
local void		scobindReconcileDecl	(Stab,AbSynTag,Symbol,IdInfo,
						 DeclInfo);

/*
 * scobindPrint
 */
extern void		scobindPrint		(Stab);
local void		scobindPrintStab	(Stab);
local void		scobindPrintId		(Symbol);
local void		scobindPrintIdInfo	(IdInfo);
local void		scobindPrintDeclInfo	(Length, DeclInfo);

/*
 * scobindUndo
 */
local void		scobindRestore		(void);
local void		scobindSave		(void);
local void		scobindUndo		(void);
local void		scoUndoStab		(Stab);
local void		scoUndoStabLevel	(StabLevel);
local Bool		scoUndoStabEntry	(StabEntry);
local void		scoUndoSyme		(Syme);
local void		scoUndoTForm		(TForm);
local void		scoUndoTFormUses	(TFormUses);
local Bool		isNewSyme		(Syme);
local Bool		isNewTForm		(TForm);
local Bool		isNewTFormUses		(TFormUses);


/*!! This needs to be cleaned up. */
local void	  markOuterInstanceOfFree     (AbSyn, AbSyn, DeclContext);

local void	  checkOuterUseOfImplicitLocal   (Stab, AbSyn, AbSyn);
local void	  checkOuterUseOfLexicalConstant (Stab, AbSyn);
local Bool	  scobindCheckOuterUseOfFluid	 (AbSyn, AbSyn);

local void	  scobindUsedType             (Stab, AbSyn);
local void	  scobindImportType           (Stab, AbSyn);

local void	  scobindMatchWiths           (AbSyn, AbSyn, DeclContext);
local Bool	  scobindMatchParams	      (AbSyn, AbSyn);
local Bool	  scobindMatchParam	      (AbSyn, AbSyn);

local void	  scobindAdd		      (AbSyn);
local void	  scobindCollect	      (AbSyn);
local void	  scobindHas		      (AbSyn);
local void	  scobindId		      (AbSyn, IdContext);
local void	  scobindLabel		      (AbSyn);
local void	  scobindLambda		      (AbSyn);
local void	  scobindRepeat		      (AbSyn);
local void	  scobindWhere		      (AbSyn);
local void	  scobindWith		      (AbSyn);

local Bool	  scobindTFormMustBeUnique	(AbSyn ab);

#define abSetTFormCond(a,t) if (! abTForm(a)) abSetTForm((a), (t))

#if 1
#define scobindRetNeedsDefn(ab) \
	(abIsTheId(ab, ssymCategory) || abHasTag(ab, AB_With))

#define	scobindMapNeedsDefn(ab) \
	(abIsAnyMap(ab) && scobindRetNeedsDefn(abMapRet(ab)))
#else
local Bool
scobindRetNeedsDefn(AbSyn ab)
{
	if (abIsTheId(ab, ssymCategory) || abHasTag(ab, AB_With))
		return true;

	return false;
}

local Bool
scobindMapNeedsDefn(AbSyn ab)
{
	AbSyn ret;
	if (!abIsAnyMap(ab))
		return false;
	ret = abMapRet(ab);

	if (abIsAnyMap(ret))
		return scobindMapNeedsDefn(ret);
	
	return scobindRetNeedsDefn(ret);
	
}

#endif
local Bool
scobindTFormMustBeUnique(AbSyn ab)
{
	if (abIsAnyMap(ab))
		return scobindTFormMustBeUnique(abMapRet(ab));

	return abIsTheId(ab,ssymCategory);
}


/******************************************************************************
 *
 * :: scopeBind		Top-level external entry points.
 *
 *****************************************************************************/

void
scobindInitFile(void)
{
	scoIdInfoList = listNil(IdInfo);
	scoUndoState  = false;
	scoDefCounter = 1;
}

void
scobindFiniFile(void)
{
	listFreeDeeply(IdInfo)(scoIdInfoList, idInfoFree);
	scoIdInfoList = listNil(IdInfo);
	scoUndoState = false;
}

int
scobindMaxDef()
{
	return scoDefCounter;
}

void
scoSetUndoState(void)
{
	scoUndoState = true;
}

void
scopeBind(Stab stab, AbSyn absyn)
{
	scoStab		= stab;
	scoIsInType	= false;
	scoIsInAdd	= false;
	scoIsInExtend	= false;
	scoIsInExport	= false;
	scoIsInImport	= false;
	scoDefineList	= listNil(AbSyn);
	scoLambdaList	= listNil(LambdaInfo);

	if (DEBUG(sco)) {
		fprintf(dbOut, "Top-level Scope Begin");
		findent += 2;
		fnewline(dbOut);
	}

	scobindRestore();

	scobindValue(absyn);
	scobindLambdaList();
	scobindReconcile(scoStab, AB_Add);

	scobindSave();

	if (DEBUG(sco)) {
		findent -= 2;
		fnewline(dbOut);
		scobindPrint(scoStab);
		if (DEBUG(scoStab)) {
			stabPrint(dbOut, scoStab);
		}
		fnewline(dbOut);
		fprintf(dbOut, "Top-level Scope End\n\n");
		fnewline(dbOut);
	}
}

local void
scobindRestore()
{
	if (scoIdInfoList) {
		scobindRestoreIdInfo(scoStab, scoIdInfoList, scoUndoState);
		scoIdInfoList = listNil(IdInfo);

		if (scoUndoState) scobindUndo();
	}	
}

local void
scobindSave()
{
	if (fintMode == FINT_LOOP)
		scoIdInfoList = scobindSaveIdInfo(scoStab);
	else
		scobindFreeIdInfo(scoStab);
}

/******************************************************************************
 *
 * :: scobindValue	Top-level recursive entry point.
 *
 *****************************************************************************/

local void
scobindValue(AbSyn absyn)
{
	switch (abTag(absyn)) {
	case AB_Nothing:
		break;

	case AB_Assign:
	case AB_Builtin:
	case AB_Declare:
	case AB_Default:
	case AB_Define:
	case AB_DDefine:
	case AB_Export:
	case AB_Extend:
	case AB_Fluid: 
	case AB_For:
	case AB_ForeignImport:
	case AB_ForeignExport:
	case AB_Free:
	case AB_Import:
	case AB_Inline:
	case AB_Local:
		scobindContext(absyn);
		break;

	case AB_Add:
		scobindLevel(absyn, scobindAdd, scobindAddFlags);
		break;

	case AB_Apply:
		scobindApply(absyn);
		break;

	case AB_CoerceTo:
		scobindValue(absyn->abCoerceTo.expr);
		scobindUsedType(scoStab, absyn->abCoerceTo.type);
		break;

	case AB_Collect:
		scobindLevel(absyn, scobindCollect, scobindCollectFlags);
		break;

	case AB_Fix:
		bugUnimpl("scoBind");

	case AB_Has:
		scobindHas(absyn);
		break;

	case AB_Hide:
		scobindUsedType(scoStab, absyn->abHide.type);
		abSetTFormCond(absyn, abTForm(absyn->abHide.type));
		break;

	case AB_Id:
		scobindId(absyn, SCO_Id_Used);
		break;

	case AB_Label:
		scobindLabel(absyn->abLabel.label);
		scobindValue(absyn->abLabel.expr);
		break;

	case AB_Lambda:
	case AB_PLambda:
		scobindLevel(absyn, scobindLambda, scobindLambdaFlags);
		break;

	case AB_PretendTo:
		scobindValue(absyn->abPretendTo.expr);
		scobindUsedType(scoStab, absyn->abPretendTo.type);
		break;

	case AB_Qualify:
		scobindValue(absyn->abQualify.what);
		scobindUsedType(scoStab, absyn->abQualify.origin);
		break;

	case AB_Reference:
		scobindReference(absyn->abReference.body);
		break;

	case AB_Repeat:
		scobindLevel(absyn, scobindRepeat, scobindRepeatFlags);
		break;

	case AB_RestrictTo:
		scobindValue(absyn->abRestrictTo.expr);
		scobindUsedType(scoStab, absyn->abRestrictTo.type);
		break;
	case AB_Where:
		scobindLevel(absyn, scobindWhere, scobindWhereFlags);
		break;
	case AB_Try:
		scobindTry(absyn);
		break;

	case AB_With:
		if (!abStab(absyn))	 /* check if already done */
			scobindLevel(absyn, scobindWith, scobindWithFlags);
		break;

	case AB_If:
		scobindIf(absyn);
		break;

	case AB_And:
		scobindAnd(absyn);
		break;

	default:
		if (!abIsLeaf(absyn)) {
			Length		i, argc = abArgc(absyn);
			AbSyn		*argv = abArgv(absyn);

			for (i = 0; i < argc; i += 1)
				scobindValue(argv[i]);
		}
		break;
	}
}

/******************************************************************************
 *
 * :: scobindContext	Dispatch function for signature introducing contexts.
 *
 *****************************************************************************/

local void
scobindContext(AbSyn absyn)
{
	switch (abTag(absyn)) {
	case AB_Assign:
		scobindAssign(absyn);
		break;

	case AB_Builtin:
		scobindBuiltin(absyn);
		break;

	case AB_Declare:
		scobindDeclare(absyn);
		break;

	case AB_Default:
		scobindDefault(absyn);
		break;

	case AB_Define:
		scobindDefine(absyn);
		break;

	case AB_DDefine:
		scobindDDefine(absyn);
		break;

	case AB_Export:
		scobindExport(absyn);
		break;

	case AB_Extend:
		scobindExtend(absyn);
		break;

	case AB_Fluid:
		scobindFluid(absyn);
		break;

	case AB_For:
		scobindFor(absyn);
		break;

	case AB_ForeignImport:
		scobindForeignImport(absyn);
		break;

	case AB_ForeignExport:
		scobindForeignExport(absyn);
		break;

	case AB_Free:
		scobindFree(absyn);
		break;

	case AB_Import:
		scobindImport(absyn);
		break;

	case AB_Inline:
		scobindInline(absyn);
		break;

	case AB_Local:
		scobindLocal(absyn);
		break;

	default:
		bugBadCase(abTag(absyn));
		break;
	}
}

/******************************************************************************
 *
 * :: scobindType	Top-level entry point for types.
 *
 *****************************************************************************/

local void
scobindType(AbSyn type)
{
	Bool	save = scoIsInType;

	scoIsInType = true;
	scobindValue(type);
	scoIsInType = save;
}

/******************************************************************************
 *
 * :: scobindLevel	Apply 'fun' within a new scope level.
 *
 *****************************************************************************/

local void
scobindLevel(AbSyn absyn, ScoBindFun fun, ULong flags)
{
	LambdaInfoList	savedLambdaList;
	Bool		saveInType = scoIsInType;


	assert(absyn);

	if (abIsNothing(absyn))
		return;

	savedLambdaList = scoLambdaList;

	scoLambdaList = listNil(LambdaInfo);

	/*
	 * If we start a new scope level then we are no longer in a
	 * type. This fixes bugs such as bug 988 where we used to give
	 * spurious errors about variables being used in constants.
	 * However, this hides some instances where a variable is used
	 * in a type. The simplest example of this is as follows: given
	 * a domain constructor Foo(X:SingleInteger),
	 *
	 *	local myVar:SingleInteger := 42;
	 *	import from Foo(X == myVar);
	 *
	 * We compute the type Foo(X) before giving X the value of myVar.
	 * This is probably one area where variables can be used in a
	 * type because the definition nails the value into a constant.
	 */
	scoIsInType   = false;

	scobindSetStab(absyn, flags);
	scoStab = abStab(absyn);
	
	if (DEBUG(sco)) {
		fprintf(dbOut, "%s Scope Begin (# %lu)",
			abInfo(abTag(absyn)).str, car(scoStab)->serialNo);
		findent += 2;
		fnewline(dbOut);
	}

	fun(absyn);
	scobindLambdaList();
	scobindReconcile(scoStab, abTag(absyn));

	if (DEBUG(sco)) {
		scobindPrint(scoStab);
		if (DEBUG(scoStab)) {
			stabPrintTo(dbOut, scoStab, -1);
		}
		findent -= 2;
		fnewline(dbOut);
		fprintf(dbOut, "%s Scope End (# %lu)",
			abInfo(abTag(absyn)).str, car(scoStab)->serialNo);
		fnewline(dbOut);
	}

	scobindFreeIdInfo(scoStab);

	scoStab       = stabPopLevel(scoStab);
	scoLambdaList = savedLambdaList;
	scoIsInType   = saveInType;
}

local void
scobindLambdaList(void)
{
	ScoConditionList savedScoCondList = scoCondList;

	scoLambdaList = listNReverse(LambdaInfo)(scoLambdaList);
	while (scoLambdaList) {
		LambdaInfo info = car(scoLambdaList);
		AbSyn	lhs = info->lhs;
		AbSyn	rhs = info->rhs;

		scoCondList = info->scoCondList;
		scobindPushDefine(lhs);
		scobindLevel(rhs, scobindLambda, scobindLambdaFlags);
		scobindPopDefine(lhs);
		
		lambdaInfoFree(info);
		scoLambdaList = listFreeCons(LambdaInfo)(scoLambdaList);
	}

	scoCondList = savedScoCondList;
}

local void
scobindPushDefine(AbSyn lhs)
{
	if (!abHasTag(lhs, AB_Comma))
		listPush(AbSyn, abDefineeId(lhs), scoDefineList);
}

local void
scobindPopDefine(AbSyn lhs)
{
	if (!abHasTag(lhs, AB_Comma))
		scoDefineList = listFreeCons(AbSyn)(scoDefineList);
}

local void
scobindSetStab(AbSyn absyn, ULong flags)
{
	Stab	stab = stabPushLevel(scoStab, abPos(absyn), flags);
	scobindStabHash(absyn, stab);
	abSetStab(absyn, stab);
}

local Bool
scobindStabHashIsUsed(Stab stab, Hash h)
{
	StabList	l;

	if (car(stab)->hash == h)
		return true;

	for (l = car(stab)->children; l; l = cdr(l))
		if (scobindStabHashIsUsed(car(l), h))
			return true;

	return false;
}

local void
scobindStabHash(AbSyn absyn, Stab stab0)
{
	Hash	h = abHashList(listCons(AbSyn)(absyn, scoDefineList));

	if (h && !scobindStabHashIsUsed(stabFile(), h))
		car(stab0)->hash = h;
}

/*
 * scopeBind functions called through scobindLevel
 */

local void
scobindAdd(AbSyn absyn)
{
	AbSyn	base	= absyn->abAdd.base;
	AbSyn	capsule = absyn->abAdd.capsule;
	Bool	save = scoIsInAdd;

	scoIsInAdd = true;
	scobindValue(base);
	scobindValue(capsule);
	scoIsInAdd = save;

	stabDefLexVar(scoStab, ssymSelf, tfType);
}

local void
scobindLambda(AbSyn absyn)
{
	TForm	tf;
	AbSyn	ret = absyn->abLambda.rtype;
	Bool	save = scoIsInAdd;

	scobindParam(absyn->abLambda.param);
	if (ret && abIsNotNothing(ret)) {
		AbSyn	*retv	= abArgvAs(AB_Comma, ret);
		Length	i, retc	= abArgcAs(AB_Comma, ret);

		scobindType(ret);

		/* Save the rhs on the return type if needed. */
		if (scobindRetNeedsDefn(ret))
			tf = tfSyntaxDefine(scoStab,ret,absyn->abLambda.body);

		/* Import from each of the return types. */
		for (i = 0; i < retc; i += 1) {
			AbSyn	reti = retv[i];

			tf = abTForm(ret);
			if (tf == NULL) {
				tf = scobindTfSyntaxFrAbSyn(scoStab, reti);
				abSetTForm(reti, tf);
			}
			if (!abHasTag(reti, AB_Hide))
				stabImportTForm(scoStab, tf);
		}

		/* Make sure product types have a type form. */
		if (abTForm(ret) == NULL)
			abSetTForm(ret, scobindTfSyntaxFrAbSyn(scoStab, ret));
	}
	else
                comsgError(absyn, ALDOR_E_ChkMissingRetType);

	scoIsInAdd = false;
	scobindValue(absyn->abLambda.body);
	scoIsInAdd = save;
}

local void
scobindCollect(AbSyn absyn)
{
	int	i, itc = abCollectIterc(absyn);

	for (i = 0; i < itc; i++)
		scobindValue(absyn->abCollect.iterv[i]);
	stabLockLevel(scoStab);
	scobindValue(absyn->abCollect.body);
	stabUnlockLevel(scoStab);
}

local void
scobindRepeat(AbSyn absyn)
{
	int	i, itc = abRepeatIterc(absyn);

	stabLockLevel(scoStab);
	for (i = 0; i < itc; i++)
		scobindValue(absyn->abRepeat.iterv[i]);
	scobindValue(absyn->abRepeat.body);
	stabUnlockLevel(scoStab);
}

local void
scobindWhere(AbSyn absyn)
{
	scobindValue(absyn->abWhere.context);
	stabLockLevel(scoStab); /* 'for' does unlock locally */
	scobindValue(absyn->abWhere.expr);
	stabUnlockLevel(scoStab);
}

local void
scobindWith(AbSyn absyn)
{
	AbSyn	base   = absyn->abWith.base;
	AbSyn	within = absyn->abWith.within;
	TForm	tfbase, tf;
	Syme	syme;

	if (abIsNotNothing(base)) {
		tfbase = scobindTfSyntaxFrAbSyn(scoStab, base);
		stabCategoricallyImportTForm(scoStab, tfbase);
	}
	tf = stabMakeUsedTForm(cdr(scoStab), absyn, scoCondListCondElt());
	syme = stabDefLexVar(scoStab, ssymSelf, tf);

	scobindValue(base);
	scobindExportWhat(within);
}

local void
scobindHas(AbSyn absyn)
{
	AbSyn	cat = absyn->abHas.property;
	AbSyn	dom = absyn->abHas.expr;
	TForm	tfdom, tfcat;

	scobindValue(cat);
	scobindValue(dom);

	tfcat = stabMakeUsedTForm(scoStab, cat, scoCondListCondElt());
	tfdom = stabMakeUsedTForm(scoStab, dom, scoCondListCondElt());

	stabAddTFormQuery(scoStab, tfdom, tfcat);
}

local void
scobindId(AbSyn id, IdContext context)
{
	if (id) {
		IdInfo	idInfo;
		Bool	okayContext;

		assert(abHasTag(id, AB_Id));

		idInfo = getIdInfoInAnyScope(scoStab, id);
		scobindSetIdUse(idInfo, context, id);

		if (scoIsInType)
			scobindSetIdUse(idInfo, SCO_Id_InType, id);

		/*
		 * Note use-before-definitions. We ought to be doing
		 * this using data-flow _after_ type inference (so
		 * that we can ignore lazy values such as domains).
		 * Until then we have to exclude certain contexts.
		 */
		okayContext = scoIsInAdd || scoIsInExport || scoIsInImport ||
				scoIsInExtend;
		if (!idInfo->declInfoList && !okayContext) {
			AbSynList	uses = idInfo->usePreDef;
			idInfo->usePreDef = listCons(AbSyn)(id, uses);
		}
	}
}

local void
scobindLabel(AbSyn lab)
{
	assert(lab);

	if (abIsNotNothing(lab)) {
		IdInfo	idInfo;
		assert(abHasTag(lab, AB_Id));

		idInfo = getIdInfoInAnyScope(scoStab, lab);
		scobindSetIdUse(idInfo, SCO_Id_Label, lab);
		stabAddLabel(scoStab, lab);
	}
}

/******************************************************************************
 *
 * :: scobind utility functions.
 *
 *****************************************************************************/

/*
 * Try to match up symbol table of 'with' forms in abType with those in
 * abLamb.  abType has already been analyzed.
 *
 * Future work: we need to be much better at detecting domains and
 * categories. We rely on finding a "with" for the type. This is
 * usually okay because abnorm tries hard to ensure that all domain
 * definitions get a "with" for their type. It fails when we have
 * non-obvious domain definitions such as:
 *
 *    MyInteger:Ring == Integer;
 *
 * The problem is that we don't recognise Integer as being a domain
 * or Ring as being a category until after tinfer. By then it might
 * be too late.
 */
local void
scobindMatchWiths(AbSyn abType, AbSyn abLamb, DeclContext context)
{
	AbSyn	lparam = abLamb->abLambda.param,
		lrtype = abLamb->abLambda.rtype;

	if (abIsAnyMap(abType) &&
	    (context == SCO_Sig_Extend ||
	     context == SCO_Sig_DDefine ||
	     scobindRetNeedsDefn(lrtype))) {
		AbSyn	aparam = abArgv(abType)[1],
			artype = abArgv(abType)[2];

		if (!abEqual(lrtype, artype))
			return;
		if (abHasTag(aparam, AB_Comma) && 1 == abArgc(aparam))
			aparam = abArgv(aparam)[0];
		if (abHasTag(lparam, AB_Comma) && 1 == abArgc(lparam))
			lparam = abArgv(lparam)[0];

		if (scobindMatchParams(aparam, lparam)) {
			abTransferSemantics(aparam, lparam);
			abTransferSemantics(artype, lrtype);
		}
	}
}

/*
 * Do the map parameters from a type declaration match the
 * map parameters from the lambda definition?
 */
local Bool
scobindMatchParams(AbSyn aparam, AbSyn lparam)
{
	AbSyn	*parv	= abArgvAs(AB_Comma, lparam);
	Length	parc	= abArgcAs(AB_Comma, lparam);
	AbSyn	*argv	= abArgvAs(AB_Comma, aparam);
	Length	i, argc = abArgcAs(AB_Comma, aparam);

	Bool	result  = (parc == argc);

	for (i = 0; result && i < argc; i += 1) {
		AbSyn	par = parv[i];
		AbSyn	arg = argv[i];

		result &= scobindMatchParam(arg, par);
	}

	return result;
}

/*
 * Does a parameter from a map type declaration match the corresponding
 * parameter from the lambda definition? Accepts declaration, definition
 * and identifier nodes as arguments.
 */
local Bool
scobindMatchParam(AbSyn arg, AbSyn par)
{
	AbSynTag aTag = abTag(arg);
	AbSynTag pTag = abTag(par);
	const char *err = "parameter is not an identifier or a declaration";

	/* Safety check */
	assert(aTag == AB_Define || aTag == AB_Declare || aTag == AB_Id);
	assert(pTag == AB_Define || pTag == AB_Declare || pTag == AB_Id);

	/* Extract declarations from default value definitions */
	if (aTag == AB_Define)
		return scobindMatchParam(arg->abDefine.lhs, par);
	if (pTag == AB_Define)
		return scobindMatchParam(arg, par->abDefine.lhs);

	/*
	 * Argument and parameter are declarations or identifiers.
	 * If only one is a declaration then ignore that declaration.
	 * We could simply return abEqual(arg->abDeclare.id, par) or
	 * abEqual(arg, par->abDeclare.id) but we play safe.
	 */
	if (aTag != pTag) {
		/* Which is the declaration node? */
		if (aTag == AB_Declare) /* && (pTag == AB_Id) */
			return scobindMatchParam(arg->abDeclare.id, par);
		if (pTag == AB_Declare) /* && (aTag == AB_Id) */
			return scobindMatchParam(arg, par->abDeclare.id);

		/* Impossible */
		comsgFatal(arg, ALDOR_F_Bug, err);
		NotReached(return false);
	}

	/* Either two declarations or two identifiers */
	if (aTag == AB_Id) /* && (pTag == AB_Id) */
		return abEqual(arg, par);

	/* Must be two declarations */
	if (aTag != AB_Declare) {
		comsgFatal(arg, ALDOR_F_Bug, err);
		NotReached(return false);
	}

	/* Compare identifiers */
	if (!abEqual(arg->abDeclare.id, par->abDeclare.id)) return false;

	/* Identifiers match: check the types */
	arg = arg->abDeclare.type;
	par = par->abDeclare.type;

	/* Identifiers match: compare types if possible */
	if (abIsNothing(arg) || abIsNothing(par)) return true;

	/* Types are present: must be equal */
	return abEqual(arg, par);
}

local void
markOuterInstanceOfFree(AbSyn id, AbSyn type, DeclContext context)
{
	Stab	stab;
	Symbol	sym = id->abId.sym;
	Bool	diffType = false;

	assert(type);

	for (stab = cdr(scoStab); stab; stab = cdr(stab)) {
		IdInfo		idInfo = getIdInfoInThisScope(stab, sym);
		DeclInfo	di;

		if (!idInfo) continue;

		if (idInfo->allFree || !idInfo->declInfoList)
			continue;

		if (abIsUnknown(type))
			di = car(idInfo->declInfoList);
		else if ((di = idInfoHasType(idInfo, type)) != NULL)
			;
		else if ((di = idInfoHasType(idInfo, abUnknown)) != NULL)
			di->type = type;
		else
			diffType = true;

		if (!di || di->uses[SCO_Sig_Free])
			continue;

		scobindSetSigUse(di, context, id);
		return;
	}

	if (diffType)
		comsgError(id, ALDOR_E_ScoBadTypeFree, symString(sym));
	else
		comsgError(id, ALDOR_E_ScoUnknownFree, symString(sym));
}

local void
scobindImportType(Stab stab, AbSyn type)
{
	if (abHasTag(type, AB_Hide)) {
		scobindUsedType(stab, type->abHide.type);
		abSetTFormCond(type, abTForm(type->abHide.type));
	}
	else {
		scobindType(type);
		abSetTFormCond(type,
			stabImportTForm(stab, scobindTfSyntaxFrAbSyn(stab, type)));
	}
}

local void
scobindUsedType(Stab stab, AbSyn type)
{
	scobindType(type);
	stabMakeUsedTForm(stab, type, scoCondListCondElt());
}

local void
checkOuterUseOfImplicitLocal(Stab stab, AbSyn id, AbSyn type)
{
	/*
	 * id is an implicit local. It is an error if it is local, free or a
	 * parameter in an outer scope. If type != 0, the types must match
	 * in an outer scope to generate the message.
	 */

	Symbol sym = id->abId.sym;

	for (stab = cdr(stab); stab; stab = cdr(stab)) {
		IdInfo idInfo = getIdInfoInThisScope(stab, sym);
		DeclInfoList dil;

		if (! idInfo)
			continue;

		for (dil = idInfo->declInfoList; dil; dil = cdr(dil)) {
			DeclInfo outerDeclInfo = car(dil);

			if ((abIsUnknown(type) ||
			     abIsUnknown(outerDeclInfo->type) ||
			     abEqualModDeclares(type, outerDeclInfo->type)) &&
			    (outerDeclInfo->uses[SCO_Sig_ImplicitLocal] ||
			     outerDeclInfo->uses[SCO_Sig_Local] ||
			     outerDeclInfo->uses[SCO_Sig_Free] ||
			     outerDeclInfo->uses[SCO_Sig_Param]))
			{
				comsgWarning(id, ALDOR_W_ScoBadLocal,
					symString(sym));
				return;
			}
		}
	}
}

local void
checkOuterUseOfLexicalConstant(Stab stab, AbSyn id)
{
	/*
	 * id is an implicit or explicit lexical constant. It is illegal
	 * to have a parameter or lexical variable in an outer scope with
	 * the same name.
	 */

	Symbol sym = id->abId.sym;

	for (stab = cdr(stab); stab; stab = cdr(stab)) {
		IdInfo idInfo = getIdInfoInThisScope(stab, sym);
		DeclInfoList dil;

		if (! idInfo)
			continue;

		for (dil = idInfo->declInfoList; dil; dil = cdr(dil)) {
			DeclInfo odi = car(dil); /* outer decl info */

			if (odi->uses[SCO_Sig_Assign] ||
			    odi->uses[SCO_Sig_Param])
			{
				comsgNError(id,ALDOR_E_ScoBadLexConst);
				if (odi->uses[SCO_Sig_Assign])
					comsgNote(odi->uses[SCO_Sig_Assign],
						  ALDOR_N_Here);
				else
					comsgNote(odi->uses[SCO_Sig_Param],
						  ALDOR_N_Here);
				return;
			}
		}
	}
}

local Bool
scobindCheckOuterUseOfFluid(AbSyn id, AbSyn type)
{
	Stab stab = scoStab;
	Symbol sym = id->abId.sym;
	Bool foundOuter = false;
	for (stab = cdr(stab); stab; stab = cdr(stab)) {
		IdInfo idInfo = getIdInfoInThisScope(stab, sym);
		if (idInfo) {
			DeclInfo declInfo;
			
			if (type)
				declInfo = idInfoHasType(idInfo, type);
			else
				declInfo = car(idInfo->declInfoList);
			if (declInfo && !declInfo->uses[SCO_Sig_Fluid])
				comsgError(id, ALDOR_E_ScoFluidShadow);
			foundOuter = true;
		}
	}
	return foundOuter;
}

/******************************************************************************
 *
 * :: scobindApply
 *
 *****************************************************************************/

local void
scobindApply(AbSyn ab)
{
	int		i, argc = abArgc(ab);
	AbSyn		*argv = abArgv(ab);

	if (scobindApplyNeedsSelf(ab))
		scobindApplySelf(ab);

	if (scobindApplyNeedsScope(ab))
		scobindLevel(ab, scobindApplyScope, scobindApplyFlags);

	else
		for (i = 0; i < argc; i += 1)
			scobindValue(argv[i]);
}

local void
scobindApplySelf(AbSyn ab)
{
	TForm		tf, tf0;
	Syme		syme;
	SymeList	self;

	tf = stabMakeUsedTForm(scoStab, ab, scoCondListCondElt());

	/* We don't want this self visible. */
	tf0 = abIsApplyOf(ab, ssymJoin) ? tf : tfType;
	syme = symeNewLexVar(ssymSelf, tf0, car(scoStab));
	self = listCons(Syme)(syme, listNil(Syme));
	tfSetSelf(tf, self);
}

local Bool
scobindApplyNeedsSelf(AbSyn ab)
{
	AbSyn	op;
	Symbol	sym;

	assert(abIsApply(ab));
	op = abApplyOp(ab);
	sym = abIsId(op) ? abIdSym(op) : NULL;

	return	sym == ssymJoin			||
		sym == ssymRawRecord		||
		sym == ssymRecord		||
		sym == ssymUnion		||
		sym == ssymTrailingArray;
}

local Bool
scobindApplyNeedsScope(AbSyn ab)
{
	int	i, argc = abArgc(ab);
	AbSyn  *argv = abArgv(ab);

	for (i = 0; i < argc; i++)
		if (scobindApplyArgNeedsScope(argv[i]))
			return true;
	return false;
}

local Bool
scobindApplyArgNeedsScope(AbSyn ab)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, ab);
	Length	i, argc = abArgcAs(AB_Comma, ab);

	for (i = 0; i < argc; i += 1)
		switch (abTag(argv[i])) {
		case AB_Declare:
		case AB_Define:
		case AB_With:
			return true;
		default:
			break;
		}

	return false;
}

/******************************************************************************
 *
 * :: scobindApplyScope
 *
 *****************************************************************************/

local void
scobindApplyScope(AbSyn absyn)
{
	AbSyn	*argv	= abArgv(absyn);
	Length	i, argc = abArgc(absyn);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];
		if (abIsAnyMap(absyn) && i == 1)
			scobindApplyParam(arg);
		else if (abHasTag(arg, AB_Comma))
			scobindApplyComma(arg);
		else
			scobindApplyArg(arg);
	}
}

/*
 * Scobind the parameter list of a function declaration. This examines
 * the parameter list from left-to-right adding new declarations as they
 * are encountered. Unfortunately this means that dependent maps must be
 * defined in a specific order with earlier parameters being available
 * as arguments to the types of later parameters.
 *
 * Ideally parameter declarations ought to be considered simultaneously 
 * so that a wider class of dependent maps are possible. This requires
 * extra work during code generation to ensure that all definitions take
 * place in the correct order. This might happen automatically.
 */
local void
scobindApplyParam(AbSyn params)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, params);
	Length	i, argc	= abArgcAs(AB_Comma, params);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];
		if (abHasTag(arg, AB_Declare) || abHasTag(arg, AB_Define))
			scobindParam(arg);
		else
			scobindApplyArg(arg);
	}
}

local void
scobindApplyComma(AbSyn ab)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, ab);
	Length	i, argc	= abArgcAs(AB_Comma, ab);

	for (i = 0; i < argc; i += 1)
		scobindApplyArg(argv[i]);
}

local void
scobindApplyArg(AbSyn arg)
{
	if (abHasTag(arg, AB_Assign)) {
		Stab	ostab = scoStab;
		scoStab = cdr(scoStab);
		scobindValue(arg);
		scoStab = ostab;
	}
	else if (abHasTag(arg, AB_With)) {
		scobindValue(arg);
		abSetTFormCond(arg, scobindTfSyntaxFrAbSyn(scoStab, arg));
	}
	else
		scobindValue(arg);
}

/******************************************************************************
 *
 * :: scobindAssign
 *
 *****************************************************************************/

local void
scobindAssign(AbSyn ab)
{
	AbSyn	lhs	= ab->abAssign.lhs;
	AbSyn	rhs	= ab->abAssign.rhs;
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc = abArgcAs(AB_Comma, lhs);
	Bool	isComma	= abHasTag(lhs, AB_Comma);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			scobindIntroduceId(arg, (isComma ? NULL : rhs),
					   SCO_Sig_Assign);
			break;

		case AB_Declare:
			scobindAssignDeclare(arg);
			break;

		case AB_Apply:
			scobindValue(arg);
			break;

		default:
			bugBadCase(abTag(arg));
			break;
		}
	}

	scobindValue(rhs);
}

local void
scobindAssignDeclare(AbSyn decl)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindAssignId(argv[i], type);
}

local void
scobindAssignId(AbSyn id, AbSyn type)
{
	DeclInfo	declInfo;

	declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Assign);
	scobindSetSigUse(declInfo, SCO_Sig_Assign, id);

	if (declInfo->uses[SCO_Sig_Free])
		markOuterInstanceOfFree(id, type, SCO_Sig_Assign);

	if (declInfoIsImplicitLocal(declInfo))
		scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal, id);
}

/******************************************************************************
 *
 * :: scobindBuiltin
 *
 *****************************************************************************/

local void
scobindBuiltin(AbSyn ab)
{
	AbSyn	what	= ab->abBuiltin.what;
	AbSyn	*argv	= abArgvAs(AB_Sequence, what);
	Length	i, argc = abArgcAs(AB_Sequence, what);
	Bool	save	= scoIsInImport;

	scoIsInImport = true;
	for (i = 0; i < argc; i += 1)
		scobindBuiltinDeclare(argv[i]);
	scoIsInImport = save;
}

local void
scobindBuiltinDeclare(AbSyn decl)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindBuiltinId(argv[i], type);
}

local void
scobindBuiltinId(AbSyn id, AbSyn type)
{
	Symbol		sym = id->abId.sym;
	AInt		bv;
	DeclInfo	declInfo;

	if (!symInfo(sym) || !symCoInfo(sym))
		symCoInfoInit(sym);

	bv = foamBValIdTag(sym);

	if (bv == FOAM_BVAL_LIMIT
	    && sym != ssymArrNew
	    && sym != ssymArrElt
	    && sym != ssymArrSet
            && sym != ssymArrDispose
	    && sym != ssymRecNew
	    && sym != ssymRecElt
	    && sym != ssymRecSet
            && sym != ssymRecDispose
            && sym != ssymBIntDispose)
	{
		comsgError(id, ALDOR_E_ScoNotBuiltin);
		return;
	}

	declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Builtin);
	scobindSetSigUse(declInfo, SCO_Sig_Builtin, id);
	scobindAddMeaning(id, sym, scoStab, SYME_Builtin, abTForm(type), bv);
}

/******************************************************************************
 *
 * :: scobindDeclare
 *
 *****************************************************************************/

local void
scobindDeclare(AbSyn ab)
{
	AbSyn	id	= ab->abDeclare.id;
	AbSyn	type	= ab->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, id);
	Length	i, argc = abArgcAs(AB_Comma, id);

	for (i = 0; i < argc; i += 1)
		scobindDeclareId(argv[i], type, NULL, SCO_Sig_Declare);
}

local DeclInfo
scobindDeclareId(AbSyn id, AbSyn type, AbSyn val, DeclContext context)
{
	Symbol		sym   = id->abId.sym;
	AbSyn		dtype = scobindDefaultType(scoStab, sym);
	IdInfo		info  = getIdInfoInAnyScope(scoStab, id);
	DeclInfo	di    = idInfoAddType(info, id, type, val, scoConditions());
	AbSyn		oval  = di->uses[SCO_Sig_Value];
	TForm		tform;

	assert(abIsNotNothing(type));

	/* Check the type against any default type which was given. */
	if (dtype && !abEqualModDeclares(type, dtype))
		comsgWarning(id, ALDOR_W_ScoVarDefault, symString(sym));

	/* Check the value against any previous value which was given (extends get a free pass). */
	if (val && oval && !abEqual(val, oval)
		&& context != SCO_Sig_Extend) {
		/*!! comsgError(id, ALDOR_E_ScoVal, symString(sym)); */
		return di;
	}

	/* Mark the identifier as declared. */
	scobindSetSigUse(di, SCO_Sig_Declare, id);
	if (val) scobindSetSigUse(di, SCO_Sig_Value, val);
	if (scoIsInType) scobindSetSigUse(di, SCO_Sig_InType, id);

	/* Construct the syntax type for the declaration. */
	scobindType(type);
	tform = scobindDeclareTForm(scoStab, id, type, val, context);
	tform = stabAddTFormDeclaree(scoStab, tform, id);
	if (!abHasTag(type, AB_Hide)) {
		stabImportTForm(scoStab, tform);
		if (context == SCO_Sig_Param)
			stabParameterImportTForm(scoStab, tform);
	}

	abSetTForm(type, tform);
	return di;
}

local TForm
scobindDeclareTForm(Stab stab, AbSyn id, AbSyn type, AbSyn val, DeclContext context)
{
	TForm tf;

	if (val == NULL) {
		if (abTForm(type))
			return abTForm(type);
		else
			return scobindTfSyntaxFrAbSyn(stab, type);
	}
	else if (abIsAnyLambda(val) &&
		 (context == SCO_Sig_Extend ||
		  context == SCO_Sig_DDefine ||
		  scobindMapNeedsDefn(type)))
		tf = tfSyntaxDefineMap(scoStab, type, val);

	else if (context == SCO_Sig_Extend ||
		 context == SCO_Sig_DDefine ||
		 (context == SCO_Sig_Define && abIsUnknown(type)) ||
		 scobindRetNeedsDefn(type))
		tf = tfSyntaxDefine(scoStab, type, val);
	else 
		tf = scobindTfSyntaxFrAbSyn(stab, type);

	if (scobindTFormMustBeUnique(type)) {
		scobindUniqifyDecl(tfExpr(tf), id);
	}
	return tf;
}

local void
scobindUniqifyDecl(AbSyn decl, AbSyn id)
{

	while (abIsAnyMap(decl))
		decl = abMapRet(decl);
	    
	assert(abTag(id) == AB_Id);
	assert(abTag(decl) == AB_Define);
	assert(abTag(decl->abDefine.lhs) == AB_Declare);

	decl->abDefine.lhs->abDeclare.id = abNewLabel(sposNone,
						      id, 
						      abNewNothing(sposNone));
}

local void
scobindIntroduceId(AbSyn id, AbSyn val, DeclContext context)
{
	Symbol		sym = id->abId.sym;
	AbSyn		type;
	TForm		tform;
	IdInfo		idInfo;
	DeclInfo	declInfo;

	/* Compute a type expression for the identifier. */

	idInfo = getIdInfoInAnyScope(scoStab, id);

	/* Try the default type. */
	type = scobindDefaultType(scoStab, sym);

	if (!type && val)
		/* Try to take the type from the syntax of the val. */
		type = scobindGuessType(val);

	if (!type && idInfo->declInfoList)
		/* Take the type from a previous declaration. */
		type = car(idInfo->declInfoList)->type;

	if (!type) {
		/* Give up. (Although later we may infer the type from val.) */
		type = abCopy(abUnknown);
		abSetTForm(type, tfUnknown);
	}

	if (context == SCO_Sig_Assign) val = NULL;
	declInfo = idInfoAddType(idInfo, id, type, val, scoConditions());

	/* Construct the syntax type for the identifier. */
	scobindType(type);
	tform = scobindDeclareTForm(scoStab, id, type, val, context);
	tform = stabAddTFormDeclaree(scoStab, tform, id);
	if (abIsUnknown(type)) abSetTForm(type, tform);
	if (!abHasTag(type, AB_Hide)) stabImportTForm(scoStab, tform);

	/* Determine if the identifier is free. */

	if (idInfo->allFree || (declInfo && declInfo->uses[SCO_Sig_Free])) {
		if (context == SCO_Sig_Define) {
			comsgNError(id, ALDOR_E_ScoFreeConst, symString(sym));
			if (declInfo && declInfo->uses[SCO_Sig_Free])
				comsgNote(declInfo->uses[SCO_Sig_Free],
					  ALDOR_N_Here);
		}
		else if (context == SCO_Sig_Assign)
			markOuterInstanceOfFree(id, type, context);
	}

	scobindSetSigUse(declInfo, context, id);
	if (val) scobindSetSigUse(declInfo, SCO_Sig_Value, val);

	if (context == SCO_Sig_Default)
		scobindSetSigUse(declInfo, SCO_Sig_Define, id);

	if (scoIsInType)
		scobindSetSigUse(declInfo, SCO_Sig_InType, id);

	if (context == SCO_Sig_Loop)
		scobindSetSigUse(declInfo, SCO_Sig_Assign, id);

	if (context != SCO_Sig_Define && declInfoIsImplicitLocal(declInfo))
		scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal, id);
}

local AbSyn
scobindGuessType(AbSyn val)
{
	AbSyn	type, id, arg, ret;

	switch (abTag(val)) {
	case AB_PretendTo:
		type = val->abPretendTo.type;
		break;
	case AB_CoerceTo:
		type = val->abCoerceTo.type;
		break;
	case AB_RestrictTo:
		type = val->abRestrictTo.type;
		break;
	case AB_Lambda:
		id = abNewId(abPos(val), ssymArrow);
		arg = val->abLambda.param;
		ret = val->abLambda.rtype;
		type = abNewApply2(abPos(val), id, arg, ret);
		break;
	case AB_PLambda:
		id = abNewId(abPos(val), ssymPackedArrow);
		arg = val->abPLambda.param;
		ret = val->abPLambda.rtype;
		type = abNewApply2(abPos(val), id, arg, ret);
		break;
	default:
		type = NULL;
		break;
	}

	return type;
}

/******************************************************************************
 *
 * :: scobindDefault
 *
 *****************************************************************************/

local void
scobindDefault(AbSyn ab)
{
	AbSyn	body	= ab->abDefault.body;
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		if (abHasTag(argv[i], AB_Declare)) {
			scobindDefaultDeclare(argv[i]);
			/* argv[i] = abNewNothing(abPos(argv[i])); */
		}
		else
			scobindValue(argv[i]);
	}
	/* ab->abDefault.body = body; */
}

local void
scobindDefaultDeclare(AbSyn decl)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindDefaultId(argv[i], type);
}

local void
scobindDefaultId(AbSyn id, AbSyn type)
{
	IdInfo	idInfo = getIdInfoInAnyScope(scoStab, id); 
	scobindImportType(scoStab, type);
	idInfo->defaultType = abCopy(type);
}

/******************************************************************************
 *
 * :: scobindDefine
 *
 *****************************************************************************/

local void
scobindDefine(AbSyn ab)
{
	AbSyn	lhs	= ab->abDefine.lhs;
	AbSyn	rhs	= ab->abDefine.rhs;
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc = abArgcAs(AB_Comma, lhs);
	Bool	isComma	= abHasTag(lhs, AB_Comma);
	Bool	isKey	= abUse(ab) == AB_Use_Value;
	AbSyn	val	= isComma ? NULL : rhs;
	DeclContext	context = isKey ? SCO_Sig_Default : SCO_Sig_Define;

	scobindPushDefine(lhs);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			scobindIntroduceId(arg, val, context);
			break;

		case AB_Declare:
			scobindDefineDeclare(arg, val);
			break;

		default:
			bugBadCase(abTag(arg));
			break;
		}
	}

	scobindDefineRhs(lhs, rhs, context);
	scobindPopDefine(lhs);
}

local void
scobindDefineDeclare(AbSyn decl, AbSyn val)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindDefineId(argv[i], type, val);
}

local void
scobindDefineId(AbSyn id, AbSyn type, AbSyn val)
{
	DeclInfo	declInfo;

	declInfo = scobindDeclareId(id, type, val, SCO_Sig_Define);
	scobindSetSigUse(declInfo, SCO_Sig_Define, id);

	abSetDefineIdx(id, scoDefCounter++);
	
	if (declInfo->uses[SCO_Sig_Free])
		comsgError(id, ALDOR_E_ScoFreeConst, symString(id->abId.sym));
}

local void
scobindDefineRhs(AbSyn lhs, AbSyn rhs, DeclContext context)
{
	if (abIsAnyLambda(rhs)) {
		LambdaInfo info = lambdaInfoAlloc(lhs, rhs, 
						  scoCondList);
		scoLambdaList = listCons(LambdaInfo)(info, scoLambdaList);

		if (abHasTag(lhs, AB_Declare))
			scobindMatchWiths(lhs->abDeclare.type, rhs, context);
	}
	else
		scobindValue(rhs);
}

/******************************************************************************
 *
 * :: scobindDDefine
 *
 *****************************************************************************/

local void
scobindDDefine(AbSyn absyn)
{
	AbSyn	body	= absyn->abDDefine.body;
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1)
		scobindDDefineComma(argv[i]);
}

local void
scobindDDefineComma(AbSyn defn)
{
	AbSyn	lhs	= defn->abDefine.lhs;
	AbSyn	rhs	= defn->abDefine.rhs;
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc = abArgcAs(AB_Comma, lhs);
	Bool	isComma	= abHasTag(lhs, AB_Comma);

	scobindPushDefine(lhs);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			scobindIntroduceId(arg, (isComma ? NULL : rhs),
					   SCO_Sig_DDefine);
			break;

		case AB_Declare:
			scobindDDefineDeclare(arg, rhs);
			break;

		default:
			bugBadCase(abTag(arg));
			break;
		}
	}

	scobindDefineRhs(lhs, rhs, SCO_Sig_DDefine);
	scobindPopDefine(lhs);
}

local void
scobindDDefineDeclare(AbSyn decl, AbSyn val)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindDDefineId(argv[i], type, val);
}

local void
scobindDDefineId(AbSyn id, AbSyn type, AbSyn val)
{
	DeclInfo	declInfo;

	declInfo = scobindDeclareId(id, type, val, SCO_Sig_DDefine);
	scobindSetSigUse(declInfo, SCO_Sig_DDefine, id);

	if (declInfo->uses[SCO_Sig_Free])
		comsgError(id, ALDOR_E_ScoFreeConst, symString(id->abId.sym));
}

/******************************************************************************
 *
 * :: scobindExtend
 *
 *****************************************************************************/

local void
scobindExtend(AbSyn absyn)
{
	AbSyn	body	= absyn->abExtend.body;
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		if (abHasTag(argv[i], AB_Define)) {
			AbSyn	lhs = argv[i]->abDefine.lhs;
			AbSyn	rhs = argv[i]->abDefine.rhs;

			scobindExtendDeclare(lhs, rhs);
			scobindDefineRhs(lhs, rhs, SCO_Sig_Extend);
		}
		else
			scobindExtendDeclare(argv[i], NULL);
	}
}

local void
scobindExtendDeclare(AbSyn lhs, AbSyn rhs)
{
	Bool	save = scoIsInExtend;

	scoIsInExtend = true;
	switch (abTag(lhs)) {
	case AB_Declare:
		scobindExtendId(lhs->abDeclare.id, lhs->abDeclare.type, rhs);
		break;

	case AB_Id:
		scobindExtendId(lhs, NULL, rhs);
		break;

	default:
		bugBadCase(abTag(lhs));
		break;
	}
	scoIsInExtend = save;
}

local void
scobindExtendId(AbSyn id, AbSyn type, AbSyn val)
{
	Symbol		sym = id->abId.sym;
	Doc		doc = abComment(id);
	DeclInfo	declInfo;
	Syme		syme, ext;

	if (doc == NULL) doc = docNone;

	/* Create the syme for the extendee. */
	declInfo = scobindDeclareId(id, type, val, SCO_Sig_Extend);
	scobindSetSigUse(declInfo, SCO_Sig_Extend, id);
	syme = stabDefExtendee(scoStab, sym, abTForm(type), doc);
	abSetSyme(id, syme);

	/* Create the syme for the extension. */
	ext = scobindGetExtend(id, type);
	if (ext == NULL) {
		TForm	template = tfSyntaxExtend(scoStab, id, type);
		ext = symeNewExtend(sym, template, car(scoStab));
		car(scoStab)->extendSymes =
			listCons(Syme)(ext, car(scoStab)->extendSymes);
	}

	/* Associate the extendee with the extension. */
	symeSetExtension(syme, ext);
	symeAddExtendee(ext, syme);
	stabAddTFormDeclaree(scoStab, symeType(ext), id);
	stabAddTFormExtendees(scoStab, symeType(ext), id);
	stabAddTFormExtension(scoStab, abTForm(type), id);
}

local Syme
scobindGetExtend(AbSyn id, AbSyn type)
{
	Stab	stab;
	Symbol	sym = id->abId.sym;

	for (stab = scoStab; stab; stab = cdr(stab)) {
		IdInfo		idInfo = getIdInfoInThisScope(stab, sym);
		DeclInfoList	dil;

		dil = idInfo ? idInfo->declInfoList : listNil(DeclInfo);

		for (; dil; dil = cdr(dil)) {
			DeclInfo	di = car(dil);
			AbSyn		abuse = di->uses[SCO_Sig_Extend];
			Syme		ext;

			ext = abuse ? symeExtension(abSyme(abuse)) : NULL;
			if (ext && tfCanExtend(abTForm(type), symeType(ext)))
				return ext;
		}
	}

	return NULL;
}

/******************************************************************************
 *
 * :: scobindExport
 *
 *****************************************************************************/

local void
scobindExport(AbSyn ab)
{
	AbSyn	what	= ab->abExport.what;
	AbSyn	from	= ab->abExport.origin;
	AbSyn	dest	= ab->abExport.destination;
	Bool	save	= scoIsInExport;

	assert(abIsNothing(from) || abIsNothing(dest));

	scoIsInExport = true;

	if (abIsNotNothing(dest))
		scobindExportTo(what, dest);

	else if (abIsNotNothing(from))
		scobindExportFrom(what, from);

	else
		scobindExportWhat(what);

	scoIsInExport = save;
}

local void
scobindExportTo(AbSyn what, AbSyn dest)
{
	scobindType(dest);
	abSetTFormCond(dest, scobindTfSyntaxFrAbSyn(scoStab, dest));
	scobindLOF(what, SCO_Sig_Local);
}

local void
scobindExportFrom(AbSyn what, AbSyn origin)
{
	TForm	tf;

	scobindType(origin);
	scobindValue(what);

	tf = scobindTfSyntaxFrAbSyn(scoStab, origin);
	if (abIsNothing(what))
		tf = stabExportTForm(scoStab, tf);
	else
		tf = stabQualifiedExportTForm(scoStab, what, tf);

	abSetTFormCond(origin, tf);
}

local void
scobindExportWhat(AbSyn what)
{
	Length	i;

	switch (abTag(what)) {
	case AB_Nothing:
		break;

	case AB_Comma:
	case AB_Sequence:
		for (i = 0; i < abArgc(what); i += 1)
			scobindExportWhat(abArgv(what)[i]);
		break;

	case AB_Define:	{
		AbSyn	lhs = what->abDefine.lhs;
		AbSyn	rhs = what->abDefine.rhs;

		scobindExportDeclare(lhs, rhs);
		scobindDefineRhs(lhs, rhs, SCO_Sig_Export);
		break;
	}

	case AB_Declare:
		scobindExportDeclare(what, NULL);
		break;

	case AB_Default:
		/* 'default' statement within 'with' body */

		scobindValue(what);
		break;

	case AB_If:
		/* 'if' statement within 'with' body */

		scobindValue(what->abIf.test);
		scoCondPush(scoStab, what->abIf.test, false);
		scobindExportWhat(what->abIf.thenAlt);
		scoCondPop();
		scoCondPush(scoStab, what->abIf.test, true);
		scobindExportWhat(what->abIf.elseAlt);
		scoCondPop();
		break;

	case AB_Import:
		/* 'import' within 'with' body */

		scobindValue(what);
		break;

	default:
		scobindValue(what);
		break;
	}
}

local void
scobindExportDeclare(AbSyn decl, AbSyn val)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindExportId(argv[i], type, val);
}

local void
scobindExportId(AbSyn id, AbSyn type, AbSyn val)
{
	Symbol		sym = id->abId.sym;
	Doc		doc = abComment(id);
	DeclInfo	declInfo;

	if (doc == NULL) doc = docNone;

	declInfo = scobindDeclareId(id, type, val, SCO_Sig_Export);
	scobindSetSigUse(declInfo, SCO_Sig_Export, id);
	scobindAddMeaning(id, sym, scoStab, SYME_Export, abTForm(type),
			  (AInt) doc);
}

/******************************************************************************
 *
 * :: scobindForeignExport
 *
 *****************************************************************************/

local void
scobindForeignExport(AbSyn ab)
{
	AbSyn	dest	= ab->abForeignExport.dest;
	AbSyn	what	= ab->abForeignExport.what;
	ForeignOrigin forg = forgFrAbSyn(dest->abApply.argv[0]);

	scoIsInExport = true;

	if (forg->protocol == FOAM_Proto_Java) {
		scobindValue(what);
	}
	else {
		scobindLOF(what, SCO_Sig_Local);
	}
	

	scoIsInExport = false;
}

/******************************************************************************
 *
 * :: scobindFluid
 *
 *****************************************************************************/

local void
scobindFluid(AbSyn ab)
{
	AbSyn	body	= ab->abFluid.argv[0];
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		if (abHasTag(argv[i], AB_Assign)) {
			scobindAssign(argv[i]);
			scobindFluidComma(argv[i]->abAssign.lhs);
		}
		else
			scobindFluidComma(argv[i]);
	}
}

local void
scobindFluidComma(AbSyn lhs)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc = abArgcAs(AB_Comma, lhs);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			scobindFluidId(arg, NULL);
			break;

		case AB_Declare:
			scobindFluidDeclare(arg);
			break;

		default:
			bugBadCase(abTag(arg));
			break;
		}
	}
}

local void
scobindFluidDeclare(AbSyn decl)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindFluidId(argv[i], type);
}

local void
scobindFluidId(AbSyn id, AbSyn type)
{
	IdInfo		idInfo = getIdInfoInAnyScope(scoStab, id);

	if (type) {
		DeclInfo	declInfo;
		declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Fluid);
		scobindSetSigUse(declInfo, SCO_Sig_Fluid, id);
	}
	else {
		DeclInfoList	dil;
		idInfo->allFluid = true;
		for (dil = idInfo->declInfoList; dil; dil = cdr(dil))
			scobindSetSigUse(car(dil), SCO_Sig_Fluid, id);
	}
}

/******************************************************************************
 *
 * :: scobindFor
 *
 *****************************************************************************/

local void
scobindFor(AbSyn ab)
{
	AbSyn	lhs	= ab->abFor.lhs;
	AbSyn	whole	= ab->abFor.whole;
	AbSyn	test	= ab->abFor.test;
	Stab	ostab	= scoStab;
	Bool	stabWasLocked = stabLevelIsLocked(scoStab);

	/* whole should be analyzed in the enclosing symbol table. */
	scoStab = stabPopLevel(scoStab);
	scobindValue(whole);
	scoStab = ostab;

	if (stabWasLocked)
		stabUnlockLevel(scoStab);

	scobindFor0(lhs, true);
	scobindValue(test);

	if (stabWasLocked)
		stabLockLevel(scoStab);
}

local void
scobindFor0(AbSyn var, Bool check)
{
	Length	i;

	switch (abTag(var)) {
	case AB_Comma:
	case AB_Sequence:
		for (i = 0; i < abArgc(var); i += 1)
			scobindFor0(abArgv(var)[i], check);
		break;

	case AB_Free:
		scobindFree(var);
		scobindFor0(abArgv(var)[0], false);
		break;

	case AB_Local:
		scobindLocal(var);
		scobindFor0(abArgv(var)[0], false);
		break;

	case AB_Id:
		if (check) {
			scobindLOF(var, SCO_Sig_Local);
			checkOuterUseOfImplicitLocal(scoStab, var, abUnknown);
		}
		scobindIntroduceId(var, NULL, SCO_Sig_Loop);
		break;

	case AB_Declare:
		if (check) {
			scobindLOF(var, SCO_Sig_Local);
			checkOuterUseOfImplicitLocal(scoStab, abDefineeId(var),
						     var->abDeclare.type);
		}
		scobindForDeclare(var->abDeclare.id, var->abDeclare.type);
		break;

	default:
		bugBadCase(abTag(var));
		break;
	}
}

local void
scobindForDeclare(AbSyn name, AbSyn type)
{
	Length	i, argc = abArgcAs(AB_Comma, name);
	AbSyn	*argv = abArgvAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindForId(argv[i], type);
}

local void
scobindForId(AbSyn id, AbSyn type)
{
	DeclInfo	declInfo;

	declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Loop);
	scobindSetSigUse(declInfo, SCO_Sig_Loop, id);
	scobindSetSigUse(declInfo, SCO_Sig_Assign, id);

	if (declInfo->uses[SCO_Sig_Free])
		markOuterInstanceOfFree(id, type, SCO_Sig_Assign);

	if (declInfoIsImplicitLocal(declInfo))
		scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal, id);
}

/******************************************************************************
 *
 * :: scobindForeignImport
 *
 *****************************************************************************/

local void
scobindForeignImport(AbSyn ab)
{
	AbSyn	origin	= ab->abForeignImport.origin;
	AbSyn	what	= ab->abForeignImport.what;
	AbSyn	*argv	= abArgvAs(AB_Sequence, what);
	Length	i, argc = abArgcAs(AB_Sequence, what);

	if (abHasTag(what, AB_Id) || abHasTag(what, AB_With))
	{
		TForm		tf;
		AbSyn		abTf;
		TFormUses	tfu;

		/* Treat as a traditional import */
		scobindType(origin);
		scobindValue(what);
		tf = scobindTfSyntaxFrAbSyn(scoStab, origin);
		tf = stabQualifiedImportTForm(scoStab, what, tf);
		abSetTFormCond(origin, tf);


		/* Find the use of this tform */
		abTf = tfExpr(tf);
		tfu = stabFindTFormUses(scoStab, abTf);


		/* Mark as qualified foreign or builtin import */
		if (abIsTheId(origin, ssymBuiltin))
			tqSetStatus(tfu->imports, TQUAL_Builtin);
		else
			tqSetStatus(tfu->imports, TQUAL_Foreign);
	}
	else
	{
		Bool	save	= scoIsInImport;
		scoIsInImport = true;
		for (i = 0; i < argc; i += 1)
		{
			AbSyn	arg = argv[i];

			if (abHasTag(argv[i], AB_Declare))
			{
				ForeignOrigin	forg;

				forg = forgFrAbSyn(origin);
				scobindForeignDeclare(arg, forg);
			}
		}
		scoIsInImport = save;
	}
}

local void
scobindForeignDeclare(AbSyn decl, ForeignOrigin forg)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1)
		scobindForeignId(argv[i], type, forg);
}

local void
scobindForeignId(AbSyn id, AbSyn type, ForeignOrigin forg)
{
	Symbol		sym = id->abId.sym;
	DeclInfo	declInfo;

	declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Foreign);
	scobindSetSigUse(declInfo, SCO_Sig_Foreign, id);
	scobindAddMeaning(id, sym, scoStab,SYME_Foreign,abTForm(type),
			  (AInt) forg);
}

/******************************************************************************
 *
 * :: scobindImport
 *
 *****************************************************************************/

local void
scobindImport(AbSyn ab)
{
	AbSyn	what	= ab->abImport.what;
	AbSyn	from	= ab->abImport.origin;
	TForm	tf;
	Bool	save	= scoIsInImport;

	scobindType(from);

	scoIsInImport = true;
	scobindValue(what);
	scoIsInImport = save;

	tf = scobindTfSyntaxFrAbSyn(scoStab, from);
	if (abIsNothing(what))
		tf = stabExplicitlyImportTForm(scoStab, tf);
	else
		tf = stabQualifiedImportTForm(scoStab, what, tf);

	abSetTFormCond(from, tf);
}

/******************************************************************************
 *
 * :: scobindInline
 *
 *****************************************************************************/

local void
scobindInline(AbSyn ab)
{
	AbSyn	what	= ab->abInline.what;
	AbSyn	from	= ab->abInline.origin;
	TForm	tf;

	scobindType(from);
	scobindValue(what);

	tf = scobindTfSyntaxFrAbSyn(scoStab, from);
	if (abIsNothing(what))
		tf = stabInlineTForm(scoStab, tf);
	else
		tf = stabQualifiedInlineTForm(scoStab, what, tf);

	abSetTFormCond(from, tf);
}

/******************************************************************************
 *
 * :: scobindParam
 *
 *****************************************************************************/

local void
scobindParam(AbSyn ab)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, ab);
	Length	i, argc	= abArgcAs(AB_Comma, ab);

	for (i = 0; i < argc; i += 1) {
		if (abHasTag(argv[i], AB_Define)) {
			AbSyn	lhs = argv[i]->abDefine.lhs;
			AbSyn	rhs = argv[i]->abDefine.rhs;

			scobindParamDefine(lhs, rhs);
			scobindDefineRhs(lhs, rhs, SCO_Sig_Param);
		}
		else
			scobindParamDefine(argv[i], NULL);
	}
}

local void
scobindParamDefine(AbSyn lhs, AbSyn rhs)
{
	switch (abTag(lhs)) {
	case AB_Declare:
		scobindParamDeclare(lhs, rhs);
		break;

	default:
		/*!! abcheck */
		comsgError(lhs, ALDOR_E_ScoBadParameter);
		break;
	}
}

local void
scobindParamDeclare(AbSyn decl, AbSyn val)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	for (i = 0; i < argc; i += 1) {
		AbSyn	id = argv[i];

		/*!! This is almost certainly wrong. */
		Symbol	sym = id->abId.sym;
		AbSyn	idType = type;
		if (abIsNothing(idType)) {
			idType = scobindDefaultType(scoStab, id->abId.sym);
			if (idType) decl->abDeclare.type = abCopy(idType);
		}
		if (!idType) {
			comsgError(id, ALDOR_E_ScoParmType, symString(sym));
			continue;
		}

		scobindParamId(id, idType, val);
	}
}

local void
scobindParamId(AbSyn id, AbSyn type, AbSyn val)
{
	Symbol		sym = id->abId.sym;
	DeclInfo	declInfo;

	declInfo = scobindDeclareId(id, type, val, SCO_Sig_Param);
	scobindSetSigUse(declInfo, SCO_Sig_Param, id);

	if (abSyme(id))
		stabAddMeaning(scoStab, abSyme(id));
	else
		scobindAddMeaning(id, sym, scoStab, SYME_Param, abTForm(type),
				  (AInt) NULL);
}

/******************************************************************************
 *
 * :: scobindReference
 *
 *****************************************************************************/

local void
scobindReference(AbSyn ab)
{
	/* We ought to continue down the tree */
	scobindValue(ab);


	/* Ignore complicated things */
	if (!abHasTag(ab, AB_Id))
		return;

	/* Mark the thing being referenced */
	scobindIntroduceId(ab, NULL, SCO_Sig_Reference);
}

/******************************************************************************
 *
 * :: scobindFree
 * :: scobindLocal
 *
 *****************************************************************************/

local void
scobindFree(AbSyn ab)
{
	scobindLOF(ab->abFree.argv[0], SCO_Sig_Free);
}

local void
scobindLocal(AbSyn ab)
{
	scobindLOF(ab->abLocal.argv[0], SCO_Sig_Local);
}

local void
scobindLOF(AbSyn body, DeclContext context)
{
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
		case AB_Declare:
		case AB_Comma:
			scobindLOFComma(arg, NULL, context);
			break;

		case AB_Assign:
			scobindLOFComma(arg->abAssign.lhs, NULL, context);
			scobindAssign(arg);
			break;

		case AB_Define:
			scobindLOFComma(arg->abDefine.lhs,
					arg->abDefine.rhs, context);
			scobindDefine(arg);
			break;

		default:
			bugBadCase(abTag(arg));
			break;
		}
	}
}

local void
scobindLOFComma(AbSyn lhs, AbSyn rhs, DeclContext context)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc = abArgcAs(AB_Comma, lhs);
	Bool	isComma	= abHasTag(lhs, AB_Comma);
	AbSyn	val	= isComma ? NULL : rhs;

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			scobindLOFId(arg, context);
			break;

		case AB_Declare:
			scobindLOFDeclare(arg, val, context);
			break;

		default:
			bugBadCase(abTag(arg));
			break;
		}
	}
}

local void
scobindLOFDeclare(AbSyn decl, AbSyn rhs, DeclContext context)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	type	= decl->abDeclare.type;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);
	Bool	isComma	= abHasTag(name, AB_Comma);
	AbSyn	val	= isComma ? NULL : rhs;

	for (i = 0; i < argc; i += 1)
		scobindLOFType(argv[i], type, val, context);
}

local void
scobindLOFId(AbSyn id, DeclContext context)
{
	IdInfo		idInfo = getIdInfoInAnyScope(scoStab, id);
	DeclInfoList	dil = idInfo->declInfoList;

	if (context == SCO_Sig_Local)
		idInfo->allLocal = true;
	else
		idInfo->allFree = true;

	if (!dil && context == SCO_Sig_Free)
		markOuterInstanceOfFree(id, abUnknown, SCO_Sig_FreeRef);

	for (; dil; dil = cdr(dil))
		scobindLOFDeclInfo(id, idInfo, car(dil), context);
}

local void
scobindLOFType(AbSyn id, AbSyn type, AbSyn val, DeclContext context)
{
	IdInfo		idInfo = getIdInfoInAnyScope(scoStab, id);
	DeclInfo	declInfo;

	scobindDeclareId(id, type, val, context);
	declInfo = idInfoHasType(idInfo, type);

	if (declInfo)
		scobindLOFDeclInfo(id, idInfo, declInfo, context);
	else if (context == SCO_Sig_Free)
		markOuterInstanceOfFree(id, type, SCO_Sig_FreeRef);
}

local void
scobindLOFDeclInfo(AbSyn id, IdInfo idInfo, DeclInfo di, DeclContext context)
{
	AbSyn	abNote = NULL;

	if (idInfo->uses[SCO_Id_Used])
		abNote = idInfo->uses[SCO_Id_Used];
	else if (di->uses[SCO_Sig_Used])
		abNote = di->uses[SCO_Sig_Used];
	else if (di->uses[SCO_Sig_Assign])
		abNote =  di->uses[SCO_Sig_Assign];
	else if (di->uses[SCO_Sig_Define])
		abNote = di->uses[SCO_Sig_Define];

	if (abNote) {
		comsgNError(id, ALDOR_E_ScoLateFreeLocal);
		comsgNote(abNote, ALDOR_N_Here);
	}

	scobindSetSigUse(di, context, id);

	if (di->uses[SCO_Sig_Param])
		comsgError(id, ALDOR_E_ScoParmLocFree);

	if (di->uses[SCO_Sig_Free] && di->uses[SCO_Sig_Local])
		comsgError(id, ALDOR_E_ScoFreeAndLoc, symString(id->abId.sym));

	if (context == SCO_Sig_Free)
		markOuterInstanceOfFree(id, di->type, SCO_Sig_FreeRef);
}

/******************************************************************************
 *
 * :: scobindTry
 *
 *****************************************************************************/

local void
scobindTry(AbSyn ab)
{
	AbSyn    id = ab->abTry.id;

	if (!abIsNothing(id)) {
		AbSyn    with, base, within;
		base  = abNewNothing(abPos(id));
		abPutUse(base, AB_Use_Type);
		within  = abNewNothing(abPos(id));
		abPutUse(within, AB_Use_Declaration);
		with = abNewWith(abPos(id), base, within);

		id = abDefineeId(ab->abTry.id);
		scobindDeclareId(id, with,
				 NULL, SCO_Sig_Define);
	}
	scobindValue(ab->abTry.expr);
	scobindValue(ab->abTry.except);
	scobindValue(ab->abTry.always);
}

/******************************************************************************
 *
 * :: scobindIf
 *
 *****************************************************************************/

local void
scobindIf(AbSyn ab)
{
	AbSyn test = ab->abIf.test;
	scobindValue(test);
	scoCondPush(scoStab, test, false);
	scobindValue(ab->abIf.thenAlt);
	scoCondPop();
	scoCondPush(scoStab, test, true);
	scobindValue(ab->abIf.elseAlt);
	scoCondPop();
}

local void
scobindAnd(AbSyn ab)
{
	int i;
	for (i=0; i<abArgc(ab); i++) {
		AbSyn cond = ab->abAnd.argv[i];
		scobindValue(cond);
		scoCondPush(scoStab, cond, false);
	}
	for (i=0; i<abArgc(ab); i++) {
		scoCondPop();
	}
}


local ScoCondition
scoConditionNew(Stab stab, AbSyn absyn, Bool isNegated)
{
	ScoCondition cond = (ScoCondition) stoAlloc(OB_Other, sizeof(*cond));
	cond->stab   = stab;
	cond->absyn  = absyn;
	cond->negate = isNegated;

	return cond;
}

local void
scoCondPush(Stab stab, AbSyn ab, Bool isNegated)
{
	scoCondList = listCons(ScoCondition)(scoConditionNew(stab, ab, isNegated), scoCondList);
}

local void
scoCondPop()
{
	/* FIXME: Blatant memleak */
	scoCondList = cdr(scoCondList);
}

local ScoConditionList
scoConditions()
{
	return scoCondList;
}

local AInt
defPosEltFromAbSyn(AbSyn ab, Bool isNegated)
{
	return isNegated ? (((AInt) ab) | 1) : (AInt) ab;
}

local DefnPos 
defposTail(DefnPos pos)
{
	return cdr(pos);
}

local Bool
defposEqInner(AInt a, AInt b)
{
	return a == b;
}

local AbSynList
defposToAbSyn(AIntList defnPos)
{
	AbSynList list = listNil(AbSyn);
	while (defnPos != listNil(AInt)) {
		AInt c = car(defnPos);
		AbSyn elt = c & 1 ? abNewNot(sposNone, (AbSyn)(c & ~1)): (AbSyn) c;
		list = listCons(AbSyn)(elt, list);
		defnPos = cdr(defnPos);
	}

	return listNReverse(AbSyn)(list);
}

local Bool
defposEqual(DefnPos a, DefnPos b)
{
	Bool flg = listEqual(AInt)(a, b, defposEqInner);
#if 0
	afprintf(dbOut, "DefPosEqual: %pAbSynList %pAbSynList = %d\n", defposToAbSyn(a), defposToAbSyn(b), flg);
#endif
	return flg;
}

local Bool
defposIsRoot(DefnPos pos)
{
	return pos == listNil(AInt);
}

local void
defposFree(DefnPos pos)
{
	listFree(AInt)(pos);
}

/******************************************************************************
 *
 * :: IdInfo
 *
 *****************************************************************************/

local IdInfo
idInfoNew(Stab stab, AbSyn id)
{
	Symbol		sym = id->abId.sym;
	IdInfo		idInfo;
	IdInfoList	iil;
	Length		c;

	if (!symInfo(sym) || !symCoInfo(sym))
		symCoInfoInit(sym);

	car(stab)->idsInScope = listCons(Symbol)(sym, car(stab)->idsInScope);

	idInfo = (IdInfo) stoAlloc(OB_Other, sizeof(*idInfo));

	idInfo->allFree		= false;
	idInfo->allLocal	= false;
	idInfo->allFluid	= false;
	idInfo->serialNo	= car(stab)->serialNo;
	idInfo->intStepNo	= intStepNo;
	idInfo->sym		= sym;
	idInfo->declInfoList	= listNil(DeclInfo); /* NULL; */
	idInfo->defaultType	= NULL;
	idInfo->exampleId	= id;
	idInfo->usePreDef	= listNil(AbSyn);

	for (c = 0; c < SCO_Id_LIMIT; c += 1)
		idInfo->uses[c] = NULL;

	/*
	 * Insert the new id info so that the id info structures are sorted
	 * from innermost to outermost.  (Inner stabs have higher serialNo's.)
	 */

	iil = listCons(IdInfo)(idInfo, idInfoCell(sym));
	setIdInfoCell(sym, iil);

	while (cdr(iil) && idInfo->serialNo < car(cdr(iil))->serialNo) {
		car(iil) = car(cdr(iil));
		iil = cdr(iil);
	}
	setcar(iil, idInfo);

	return idInfo;
}

local void
idInfoFree(IdInfo idInfo)
{
	listFreeDeeply(DeclInfo)(idInfo->declInfoList, declInfoFree);
	listFree(AbSyn)(idInfo->usePreDef);
	stoFree(idInfo);
}

local Bool
idInfoIsNew(IdInfo idInfo)
{
	return idInfo->intStepNo == intStepNo - 1;
}

local void
scobindFreeIdInfo(Stab stab)
{
	SymbolList	ids = car(stab)->idsInScope;

	for (; ids; ids = listFreeCons(Symbol)(ids)) {
		Symbol		id = car(ids);
		IdInfoList	il = idInfoCell(id);
		IdInfo		info = car(il);

		idInfoFree(info);
		setIdInfoCell(id, listFreeCons(IdInfo)(il));
	}
	car(stab)->idsInScope = ids;
}

local IdInfoList
scobindSaveIdInfo(Stab stab)
{
	SymbolList	ids = car(stab)->idsInScope;
	IdInfoList	iil = listNil(IdInfo);

	for (; ids; ids = listFreeCons(Symbol)(ids)) {
		Symbol		id = car(ids);
		IdInfoList	il = idInfoCell(id);
		IdInfo		info = car(il);

		iil = listCons(IdInfo)(info, iil);
		setIdInfoCell(id, listFreeCons(IdInfo)(il));
	}
	car(stab)->idsInScope = ids;

	return iil;
}

local void
scobindRestoreIdInfo(Stab stab, IdInfoList iil, Bool undo)
{
	SymbolList	ids = listNil(Symbol);

	for (; iil; iil = listFreeCons(IdInfo)(iil)) {
		IdInfo		info = car(iil);
		Symbol		id = info->sym;
		IdInfoList	il = idInfoCell(id);

		if (undo && idInfoIsNew(info))
			idInfoFree(info);
		else {
			ids = listCons(Symbol)(id, ids);
			setIdInfoCell(id, listCons(IdInfo)(info, il));
			if (undo) scobindRestoreDeclInfo(info);
		}
	}
	car(stab)->idsInScope = ids;
}

/*
 * Get the id info from the innermost level which can have it.
 * A lexical level can have info for an id if the id is already present,
 * or if the level is unlocked.
 */
local IdInfo
getIdInfoInAnyScope(Stab stab, AbSyn id)
{
	Symbol		sym = id->abId.sym;
	IdInfo		info = NULL;

	for (; stab && !info; stab = cdr(stab)) {
		info = getIdInfoInThisScope(stab, sym);
		if (!info && !stabLevelIsLocked(stab))
			info = idInfoNew(stab, id);
	}

	assert(info);
	return info;
}

/*
 * Get the id info for sym if it exists in the given scope.
 * Return NULL if no id info exists for sym in the given scope.
 */
local IdInfo
getIdInfoInThisScope(Stab stab, Symbol sym)
{
	ULong		serialNo = car(stab)->serialNo;
	IdInfoList	info;

	if (!symInfo(sym) || !symCoInfo(sym))
		symCoInfoInit(sym);

	for (info = idInfoCell(sym); info; info = cdr(info)) {
		if (car(info)->serialNo == serialNo)
			return car(info);
		else if (car(info)->serialNo < serialNo)
			return NULL;
	}
	return NULL;
}

local void
scobindSetIdUse(IdInfo idInfo, IdContext context, AbSyn use)
{
	idInfo->uses[context] = use;
}

local AbSyn
scobindDefaultType(Stab stab, Symbol sym)
{
	AbSyn	dtype = NULL;

	for (; stab && !dtype; stab = cdr(stab)) {
		IdInfo	info = getIdInfoInThisScope(stab, sym);
		dtype = info ? info->defaultType : NULL;
	}

	return dtype;
}

/******************************************************************************
 *
 * :: DeclInfo
 *
 *****************************************************************************/

local DeclInfo
declInfoNew(AbSyn id, AbSyn type, DefnPos cond)
{
	DeclInfo	di;
	Length		c;

	di = (DeclInfo) stoAlloc(OB_Other, sizeof(*di));

	di->intStepNo	= intStepNo;
	di->id		= id;
	di->type	= type;
	di->defpos	= listSingleton(DefnPos)(cond);
	di->doc		= id ? abComment(id) : docNone;

	for (c = 0; c < SCO_Sig_LIMIT; c += 1)
		di->uses[c] = NULL;

	return di;
}

local void
declInfoFree(DeclInfo di)
{
	stoFree(di);
}

local Bool
declInfoIsNew(DeclInfo di)
{
	return di->intStepNo == intStepNo - 1;
}

local Bool
declInfoUseIsNew(AbSyn ab)
{
	return ab && (!abSyme(ab) || isNewSyme(abSyme(ab)));
}

local Bool
declInfoIsImplicitLocal(DeclInfo di)
{
	return !(di->uses[SCO_Sig_Local] ||
		 di->uses[SCO_Sig_Free]  ||
		 di->uses[SCO_Sig_Param] ||
		 di->uses[SCO_Sig_Export]);
}

local void
scobindRestoreDeclInfo(IdInfo ido)
{
	DeclInfoList	dil = ido->declInfoList;
	dil = listFreeIfSat(DeclInfo)(dil, declInfoFree, declInfoIsNew);
	ido->declInfoList = dil;

	for (; dil; dil = cdr(dil)) {
		DeclInfo	di = car(dil);
		Length		i;
		for (i = SCO_Sig_START; i < SCO_Sig_LIMIT; i += 1)
			if (declInfoUseIsNew(di->uses[i]))
				di->uses[i] = NULL;
	}
}

local DeclInfo
idInfoHasType(IdInfo ido, AbSyn type)
{
	DeclInfoList	dil;

	for (dil = ido->declInfoList; dil; dil = cdr(dil))
		if (abEqualModDeclares(car(dil)->type, type))
			return car(dil);
	return NULL;
}

/*
 * Augment list of types and symbol documentation.
 */
local DeclInfo
idInfoAddType(IdInfo ido, AbSyn id, AbSyn type, AbSyn val, ScoConditionList cond)
{
	DeclInfo	di;

	/* If this id has been declared with this type,
	 * use the old decl info.
	 */
	if ((di = idInfoHasType(ido, type)) != NULL)
		;

	/* If this id has been declared with type Unknown,
	 * substitute the new type in the old decl info.
	 */
	else if ((di = idInfoHasType(ido, abUnknown)) != NULL &&
		 (val == NULL || di->uses[SCO_Sig_Value] == NULL))
		di->type = type;

	/* Otherwise create a new decl info structure. */
	else {
		di = declInfoNew(id, type, scoConditionToDefnPos(cond));

		if (ido->allFree)
			scobindSetSigUse(di, SCO_Sig_Free, id);
		if (ido->allLocal)
			scobindSetSigUse(di, SCO_Sig_Local, id);

		ido->declInfoList = listCons(DeclInfo)(di, ido->declInfoList);
	}

	return di;
}

local void
scobindSetSigUse(DeclInfo declInfo, DeclContext context, AbSyn use)
{
	/* check for double definitions */
	
	/*!! This shouldn't be here. */
	if (context == SCO_Sig_Define && declInfo->uses[context]) {
		if (fintMode == FINT_LOOP &&
		    fintYesOrNo("Redefine? (y/n): ")) {
			comsgFPrintf(stdout, ALDOR_M_FintRedefined,
				     use->abId.sym->str);
		}
		else if (!scobindCheckCondition(declInfo, scoCondList)) {
			comsgNError(use, ALDOR_E_ScoDupDefine,
				     symString(use->abId.sym));
			comsgNote(declInfo->uses[context], ALDOR_N_Here);
		}
		declInfo->defpos = listCons(DefnPos)(scoConditionToDefnPos(scoCondList),
						     declInfo->defpos);
	}

	declInfo->uses[context] = use;
}

local TfCondElt scoConditionListToCondElt(ScoConditionList);
local AbSynList scoConditionToAbSyn(ScoConditionList);

local Bool
scobindCheckCondition(DeclInfo declInfo, ScoConditionList conditionList)
{
	DefnPos defnPos = scoConditionToDefnPos(conditionList);
	Bool check = scobindCheckDefnPos(declInfo, defnPos);
	scoDEBUG(dbOut, "scobindCheckCondition: %pAbSynList %d\n", 
		 scoConditionToAbSyn(conditionList), check);
	defposFree(defnPos);

	return check;
}

local DefnPos
scoConditionToDefnPos(ScoConditionList condition)
{
	AIntList list = listNil(AInt);
	while (condition != listNil(ScoCondition)) {
		ScoCondition conditionElt = car(condition);
		AInt elt = defPosEltFromAbSyn(conditionElt->absyn, conditionElt->negate);
		list = listCons(AInt)(elt, list);
		condition = cdr(condition);
	}

	return listNReverse(AInt)(list);
}

local AbSynList
scoConditionToAbSyn(ScoConditionList condition)
{
	AbSynList list = listNil(AbSyn);
	while (condition != listNil(ScoCondition)) {
		ScoCondition conditionElt = car(condition);
		AbSyn elt = conditionElt->negate 
			? abNewNot(sposNone, conditionElt->absyn) 
			: conditionElt->absyn;
		list = listCons(AbSyn)(elt, list);
		condition = cdr(condition);
	}

	return listNReverse(AbSyn)(list);
}

local TfCondElt 
scoCondListCondElt()
{
	return scoConditionListToCondElt(scoCondList);
}

local TfCondElt
scoConditionListToCondElt(ScoConditionList conditionList)
{
	AbSynList list = listNil(AbSyn);
	ScoConditionList condition = conditionList;
	if (condition == listNil(ScoCondition))
		return NULL;

	while (condition != listNil(ScoCondition)) {
		ScoCondition conditionElt = car(condition);
		AbSyn elt = conditionElt->negate 
			? abNewNot(sposNone, conditionElt->absyn) 
			: conditionElt->absyn;
		list = listCons(AbSyn)(elt, list);
		condition = cdr(condition);
	}

	list = listNReverse(AbSyn)(list);
	return tfCondEltNew(conditionList->first->stab, list);
}


/*
 * Returns true iff a definition is found with the same 
 * conditionalisation as the current symbol.
 */
local Bool
scobindCheckDefnPos(DeclInfo declInfo, DefnPos posn)
{
	DefnPosList lpos = declInfo->defpos;

	while (lpos) {
		if (defposEqual(car(lpos), posn))
			return false;
		lpos = cdr(lpos);
	}

	if (defposIsRoot(posn))	return true;

	return scobindCheckDefnPos(declInfo, defposTail(posn));
}

AbSynList
scobindDefnPosToList(DefnPosList defnPosList)
{
	AbSynList conditionList = listNil(AbSyn);
	while (defnPosList != listNil(DefnPos)) {
		AbSynList absynList = defposToAbSyn(car(defnPosList));
		defnPosList = cdr(defnPosList);

		if (absynList == listNil(AbSyn))
			conditionList = listCons(AbSyn)(NULL, conditionList);
		else {
			AbSyn absyn = (cdr(absynList) == listNil(AbSyn))
				? car(absynList) : abNewAndAll(sposNone, absynList);
			conditionList = listCons(AbSyn)(absyn, conditionList);
		}
	}
	return listNReverse(AbSyn)(conditionList);
}

/******************************************************************************
 *
 * :: scobindAddMeaning
 *
 *****************************************************************************/

local void
scobindAddMeaning(AbSyn ab, Symbol sym, Stab stab, SymeTag kind,
		  TForm tf, AInt data)
{
	if (scobindNeedsMeaning(ab, tf)) {
		Syme	syme = scobindDefMeaning(stab,kind,sym,tf,data);
		scobindSetMeaning(ab, syme);
		symeSetSrcPos(syme, abPos(ab));
	}
}

local Bool
scobindNeedsMeaning(AbSyn ab, TForm tf)
{
	return	ab == NULL || abSyme(ab) == NULL ||
		symeIsLazy(abSyme(ab)) || symeType(abSyme(ab)) != tf;
}

local void
scobindSetMeaning(AbSyn ab, Syme syme)
{
	if (ab) {
		if (abSyme(ab) == NULL) {
			String	s = abPrettyClippedIn(tfExpr(symeType(syme)),
						      60, ABPP_NOINDENT);

			comsgRemark(ab, ALDOR_R_ScoMeaning,
				    comsgString(symeTagToDescrMsgId(symeKind(syme))),
				    symeString(syme), s);
			strFree(s);
		}
		abSetSyme(ab, syme);
	}
}

local Syme
scobindDefMeaning(Stab stab, SymeTag kind, Symbol sym, TForm tf, AInt data)
{
	Syme	syme = NULL;

	switch (kind) {
	case SYME_Param:
		syme = stabDefParam(stab, sym, tf);
		break;
	case SYME_LexVar:
		syme = stabDefLexVar(stab, sym, tf);
		break;
	case SYME_LexConst:
		syme = stabDefLexConst(stab, sym, tf);
		break;
	case SYME_Fluid:
		syme = stabDefFluid(stab, sym, tf);
		break;
	case SYME_Export:
		syme = stabDefExport(stab, sym, tf, (Doc) data);
		break;
	case SYME_Builtin:
		syme = stabDefBuiltin(stab, sym, tf, (FoamBValTag) data);
		break;
	case SYME_Foreign:
		syme = stabDefForeign(stab, sym, tf, (ForeignOrigin) data);
		break;
	default:
		bugBadCase(kind);
		break;
	}

	return syme;
}

/******************************************************************************
 *
 * :: scobindReconcile
 *
 *****************************************************************************/

local void
scobindReconcile(Stab stab, AbSynTag context)
{
	SymbolList	ids = car(stab)->idsInScope;

	for (; ids; ids = cdr(ids)) {
		Symbol		id = car(ids);
		IdInfoList	il = idInfoCell(id);
		IdInfo		info = car(il);


		/* The standard checks */
		scobindReconcileId(stab, context, id, info);
	}
}

local void
scobindReconcileId(Stab stab, AbSynTag context, Symbol sym, IdInfo idInfo)
{
	AbSyn	abuse;

	if (idInfo->declInfoList)
		scobindReconcileDecls(stab, context, sym, idInfo);

	/* Check for bad uses or non-uses of locals. */
	else if (idInfo->allLocal) {
		AbSyn	ab = idInfo->exampleId;
		String	str = symString(sym);

		if (idInfo->allFree)
			comsgError(ab, ALDOR_E_ScoFreeAndLoc, str);
		else if ((abuse = idInfo->uses[SCO_Id_Used]) != NULL)
			comsgWarning(abuse, ALDOR_W_ScoBadUse, str);
		else
			comsgWarning(ab, ALDOR_W_ScoLocalNoUse, str);

		if (idInfo->allFluid)
			bug("local, free, fluid, wassa matta?");
	}
	else {
		/*
		 * The identifier is not defined in this scope
		 * level and so it must either be defined in an
		 * outer level or it has been imported. Create
		 * new usage information in the level outside
		 * this one to allow the symbol meaning to be
		 * bound in that level, if appropriate.
		 */
		abuse = idInfo->uses[SCO_Id_InType];
		if (abuse && stab != stabFile()) {
			idInfo = getIdInfoInAnyScope(cdr(stab), abuse);
			idInfo->uses[SCO_Id_InType] = abuse;
		}
		abuse = idInfo->uses[SCO_Id_Used];
		if (abuse && stab != stabFile()) {
			idInfo = getIdInfoInAnyScope(cdr(stab), abuse);
			idInfo->uses[SCO_Id_Used] = abuse;
		}
	}
}

local void
scobindReconcileDecls(Stab stab, AbSynTag context, Symbol sym, IdInfo idInfo)
{
	Bool		lazy = false;
	AbSynList	earlyUse;
	DeclInfoList	declInfoList;
	AbSyn		isAssigned = NULL, isDefined = NULL, isReffed = NULL;

	/* Reverse list so we see signatures in correct order. */
	idInfo->declInfoList = listNReverse(DeclInfo)(idInfo->declInfoList);
	declInfoList = idInfo->declInfoList;

	for ( ; declInfoList; declInfoList = cdr(declInfoList)) {
		DeclInfo	declInfo = car(declInfoList);
		AbSyn		abuse;

		abuse = declInfo->uses[SCO_Sig_Assign];
		if (abuse) {
			if (isAssigned) {
				comsgNError(isAssigned, ALDOR_E_ScoVarOverload);
				comsgNote(abuse, ALDOR_N_Here);
				abState(declInfo->id) = AB_State_Error;
			}
			isAssigned = abuse;
			if (fintMode == FINT_LOOP && isDefined)
				abState(declInfo->id) = AB_State_Error;
		}

		abuse = declInfo->uses[SCO_Sig_Define];
		if (abuse && !isDefined) {
			isDefined = abuse;
			if (fintMode == FINT_LOOP && isAssigned)
				abState(declInfo->id) = AB_State_Error;
		}

		abuse = declInfo->uses[SCO_Sig_Reference];
		if (abuse)
			isReffed = abuse;


		/* Does it look like a lazy value? */
		if (!lazy && abIsAnyMap(declInfo->type))
			lazy = true;

		scobindReconcileDecl(stab, context, sym, idInfo, declInfo);
	}


	/* Ensure id is not assigned and defined. */
	if (isAssigned && isDefined) {
		comsgNError(isAssigned, ALDOR_E_ScoAssAndDef, symString(sym));
		comsgNote(isDefined, ALDOR_N_Here);
	}


	/* Ensure id is not referenced and defined */
	if (isReffed && isDefined) {
		comsgNError(isReffed, ALDOR_E_ScoAssAndRef, symString(sym));
		comsgNote(isDefined, ALDOR_N_Here);
	}


	/* Ensure id is not library or archive identifier and assigned	*/
	/* or defined.							*/
	if (isAssigned || isDefined) {
		AbSyn	ab = isAssigned ? isAssigned : isDefined;
		if (stabGetLibrary(sym) || stabGetArchive(sym))
			comsgError(ab, ALDOR_E_ScoLibrary, symString(sym));
	}


	/*
	 * Ensure that non-lazy constants aren't used before their
	 * definition outside an `add'. We don't seem to need to
	 * watch for domains and categories as they never seem to
	 * generate use-before-definition errors. If we do, try
	 * extending the range of cases in which `lazy' is true.
	 */
	earlyUse = idInfo->usePreDef;
	if (!lazy && isDefined && earlyUse) {
		/*
		 * Display an error for each use-before-definition of
		 * non-lazy constants used outside and `add' body.
		 * We don't need to reverse `earlyUse' because the
		 * comsg system sorts messages by line number.
		 */
		for (;earlyUse; earlyUse = cdr(earlyUse)) {
			AbSyn	id = car(earlyUse);
			comsgNError(id, ALDOR_E_ScoEarlyUse, symString(sym));
			comsgNote(isDefined, ALDOR_N_Here);
		}
	}
}

/*
 * This function processes the data for a given signature.
 */
local void
scobindReconcileDecl(Stab stab, AbSynTag context, Symbol sym, IdInfo idInfo,
		     DeclInfo declInfo)
{
	AbSyn	assigned = declInfo->uses[SCO_Sig_Assign];

	/* Error if name used in type is a variable. */

	if ((idInfo->uses[SCO_Id_InType] || declInfo->uses[SCO_Sig_InType])
	    && assigned)
	{
		comsgError(assigned, ALDOR_E_ScoAssTypeId, symString(sym));
	}

	/*
	 * Error if a loop variable is assigned to. The initial creation
	 * of the loop variable will be an assignment, but if the use is
	 * not the same then there has been a later assignment.
	 */

	if (declInfo->uses[SCO_Sig_Loop] &&
	    declInfo->uses[SCO_Sig_Loop] != assigned)
	{
		comsgError(assigned, ALDOR_E_ScoBadLoopAss);
	}

	/* Leave early if we don't require symbol meaning generation */

	if (declInfo->uses[SCO_Sig_Builtin] && declInfo->uses[SCO_Sig_Foreign])
		comsgError(declInfo->uses[SCO_Sig_Builtin],
			ALDOR_E_ScoSameSig);

	if (declInfo->uses[SCO_Sig_Builtin] || declInfo->uses[SCO_Sig_Foreign])
	{
		if (declInfo->uses[SCO_Sig_Free])
			comsgError(declInfo->uses[SCO_Sig_Free],ALDOR_E_ScoNoFree);
		if (declInfo->uses[SCO_Sig_Local])
			comsgError(declInfo->uses[SCO_Sig_Local],ALDOR_E_ScoNoFree);
		if (declInfo->uses[SCO_Sig_Param])
			comsgError(declInfo->uses[SCO_Sig_Free],ALDOR_E_ScoNoParm);
		if (assigned)
			comsgError(assigned, ALDOR_E_ScoNoSet);
		if (declInfo->uses[SCO_Sig_Define])
			comsgError(declInfo->uses[SCO_Sig_Define],ALDOR_E_ScoNoSet);
		return;
	}

	if (declInfo->uses[SCO_Sig_Free] || declInfo->uses[SCO_Sig_Param])
		return;

	/* Process explicit locals */

	if (declInfo->uses[SCO_Sig_Local]) {
		TForm	tf = scobindTfSyntaxFrAbSyn(stab, declInfo->type);

		if (declInfo->uses[SCO_Sig_Define]) {
			checkOuterUseOfLexicalConstant(stab, declInfo->id);
			scobindAddMeaning(declInfo->id,sym,stab,SYME_LexConst,
				tf, (AInt) NULL);
			return;
		}

		if (assigned) {
			scobindAddMeaning(declInfo->id,sym,stab,SYME_LexVar,
			    tf, (AInt) NULL);
			return;
		}

		/* we seem to need to make the syme to keep tinfer happy */

		scobindAddMeaning(declInfo->id,sym,stab,SYME_LexConst,
				  tf, (AInt) NULL);

		/* check for use without assignment or definition */

		if (idInfo->uses[SCO_Id_Used])
			 comsgWarning(idInfo->uses[SCO_Id_Used],
				      ALDOR_W_ScoBadUse, symString(sym));
		else if (declInfo->uses[SCO_Sig_Used])
			 comsgWarning(declInfo->uses[SCO_Sig_Used],
				      ALDOR_W_ScoBadUse, symString(sym));
		else if (! declInfo->uses[SCO_Sig_FreeRef])
			 comsgWarning(declInfo->uses[SCO_Sig_Local],
				      ALDOR_W_ScoLocalNoUse, symString(sym));
		return;
	}
	if (declInfo->uses[SCO_Sig_Fluid]) {
		TForm tf;
		if (abIsNothing(declInfo->type))
			tf = tfUnknown;
		else
			tf = scobindTfSyntaxFrAbSyn(stab,declInfo->type);

		scoFluidDEBUG(dbOut, "Adding fluid: %s", symString(sym));
		if (DEBUG(scoFluid)) {
			tfPrintDb(tf);
		}

		if (!scobindCheckOuterUseOfFluid(declInfo->id, declInfo->type)) {
			scoFluidDEBUG(dbOut, " New\n");
			scobindAddMeaning(declInfo->id, sym, stab, SYME_Fluid, 
					  tf, (AInt) NULL);
		}
		else
			scoFluidDEBUG(dbOut, " See'd it before.\n");
		return;
	}

	/* Signature without 'local' declaration. */

	if (assigned) {
		TForm tf;

		if (context != AB_Apply)
			checkOuterUseOfImplicitLocal(stab,
				assigned,
				declInfo->type);

		tf = scobindTfSyntaxFrAbSyn(stab, declInfo->type);

		if (fintMode == FINT_LOOP) {
			if (abSyme(declInfo->id))
				return;
			if (tfIsUnknown(tf) && stabGetLex(stab, sym))
				return;
		}

		scobindAddMeaning(declInfo->id, sym, stab, SYME_LexVar,
				  tf, (AInt) NULL);
		return;
	}

	if (declInfo->uses[SCO_Sig_Define]) {
		TForm	tf = scobindTfSyntaxFrAbSyn(stab, declInfo->type);
		if (context == AB_Add || context == AB_With) {
			Syme syme;
			AbSynList defConditions;
			if (!abSyme(declInfo->id))
				scobindAddMeaning(declInfo->id,
					  sym, stab, SYME_Export,
					  tf, (AInt) declInfo->doc);
			assert(abSyme(declInfo->id));
			syme = abSyme(declInfo->id);
			defConditions = scobindDefnPosToList(declInfo->defpos);
			symeSetDefinitionConditions(syme, defConditions);
		}
		else {
			checkOuterUseOfLexicalConstant(stab, declInfo->id);
			scobindAddMeaning(declInfo->id, sym, stab,
					  SYME_LexConst, tf, (AInt) NULL);

			if (context != AB_Apply)
				checkOuterUseOfImplicitLocal(stab,
					declInfo->uses[SCO_Sig_Define],
					declInfo->type);

			scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal,
				declInfo->uses[SCO_Sig_Define]);
		}
		return;
	}

	if (abSyme(declInfo->id))
		return;

	/* make anything else into a lexical constant */

	scobindAddMeaning(declInfo->id, sym, stab, SYME_LexConst,
			  scobindTfSyntaxFrAbSyn(stab, declInfo->type), (AInt) NULL);
}

/******************************************************************************
 *
 * :: scobindPrint
 *
 *****************************************************************************/

void
scobindPrint(Stab stab)
{
	scobindPrintStab(stab);
}

local void
scobindPrintStab(Stab stab)
{
	SymbolList	ids;

	for (ids = car(stab)->idsInScope; ids; ids = cdr(ids)) {
		scobindPrintId(car(ids));
		if (cdr(ids)) fnewline(dbOut);
	}
}

local void
scobindPrintId(Symbol id)
{
	IdInfoList	il = idInfoCell(id);
	IdInfo		info = (il ? car(il) : NULL);
	DeclInfoList	dil;
	Length		i;

	if (!info) return;

	fprintf(dbOut, "        %-14s: ", symString(id));
	scobindPrintIdInfo(info);
	for (i = 1, dil = info->declInfoList; dil; i += 1, dil = cdr(dil))
		scobindPrintDeclInfo(i, car(dil));
}

local void
scobindPrintIdInfo(IdInfo info)
{
	Length		c;

	if (info->allFree)
		fprintf(dbOut, "AllFree ");
	if (info->allLocal)
		fprintf(dbOut, "AllLocal ");
	if (info->allFluid)
		fprintf(dbOut, "AllFluid ");

	for (c = SCO_Id_START; c < SCO_Id_LIMIT; c += 1)
		if (info->uses[c])
			fprintf(dbOut, "%s ", IdContextNames[c]);

	if (info->defaultType) {
		fprintf(dbOut, "Default (");
		abPrettyPrintClippedIn(dbOut, info->defaultType,
				       ABPP_UNCLIPPED, 1);
		fprintf(dbOut, ")");
	}
}

local void
scobindPrintDeclInfo(Length i, DeclInfo declInfo)
{
	Length		c;

	fnewline(dbOut);
	fprintf(dbOut, "        %-14s: [%d] ", " ", (int) i);

	if (declInfo->type) {
		fprintf(dbOut, "<");
		abPrettyPrintClippedIn(dbOut, declInfo->type,
				       ABPP_UNCLIPPED, 1);
		fprintf(dbOut, "> ");
	}

	for (c = SCO_Sig_START; c < SCO_Sig_LIMIT; c += 1)
		if (declInfo->uses[c])
			fprintf(dbOut, "%s ", DeclContextNames[c]);
}

/******************************************************************************
 *
 * :: scobindUndo
 *
 *****************************************************************************/

local void
scobindUndo()
{
	scoUndoSymes = listNil(Syme);
	scoUndoTForms = listNil(TForm);

	scoUndoStab(scoStab);

	listFreeDeeply(Syme)(scoUndoSymes, symeFree);
	listFreeDeeply(TForm)(scoUndoTForms, tfFree);

	scoUndoState = false;
}

local void
scoUndoStab(Stab stab)
{
	StabLevel	stabLev;
	StabList	stabl;

	for (stabLev = car(stab); stab; stab = cdr(stab)) {
		scoUndoStabLevel(stabLev);
		for (stabl = stabLev->children; stabl; stabl = cdr(stabl))
			scoUndoStab(car(stabl));
	}
}

local void
scoUndoStabLevel(StabLevel stabLev)
{
	tblRemoveIf(stabLev->tbl, (TblFreeEltFun) stoFree,
		    (TblTestEltFun) scoUndoStabEntry);

	stabLev->boundSymes= listFreeIfSat(Syme)
		(stabLev->boundSymes, scoUndoSyme, isNewSyme);

	stabLev->tformsUsed.list = listFreeIfSat(TFormUses)
		(stabLev->tformsUsed.list, scoUndoTFormUses, isNewTFormUses);

	if (stabLev->tformsUsed.table)
		tblRemoveIf(stabLev->tformsUsed.table,
			    (TblFreeEltFun) scoUndoTFormUses, 
			    (TblTestEltFun) isNewTFormUses);

	stabLev->tformsUnused = listFreeIfSat(TForm)
		(stabLev->tformsUnused, scoUndoTForm, isNewTForm);
}

local Bool
scoUndoStabEntry(StabEntry stent)
{
	SymeList	osymes, nsymes;
	Length		oldLength;

	if (!stent) return false;

	osymes = stent->symev[0];
	oldLength = listLength(Syme)(osymes);

	nsymes = listFreeIfSat(Syme)(osymes, scoUndoSyme, isNewSyme);
	stent->symev[0] = nsymes;

	if (listLength(Syme)(nsymes) != oldLength) {
		tpossFree(stent->possv[0]);
		stent->possv[0] = NULL;
	}

	return (nsymes == listNil(Syme));
}

local void
scoUndoSyme(Syme syme)
{
	if (!listMemq(Syme)(scoUndoSymes, syme))
		scoUndoSymes = listCons(Syme)(syme, scoUndoSymes);
}

local void
scoUndoTForm(TForm tf)
{
	if (!listMemq(TForm)(scoUndoTForms, tf))
		scoUndoTForms = listCons(TForm)(tf, scoUndoTForms);
}

local void
scoUndoTFormUses(TFormUses tfu)
{
	scoUndoTForm(tfu->tf);
	/* stoFree(tfu); $$ USE tfUsesPool or intStepNo or refCounter */
}

local Bool
isNewSyme(Syme syme)
{
	return symeIntStepNo(syme) == intStepNo - 1;
}

local Bool
isNewTForm(TForm tf)
{
	return tf->intStepNo == intStepNo - 1;
}

local Bool
isNewTFormUses(TFormUses tfu)
{
	return tfu->tf->intStepNo == intStepNo - 1;
}


/******************************************************************************
 *
 * :: Setting conditions
 *
 *****************************************************************************/

local void scobindTfConditions(Stab stab, TForm tf, TfCondElt conditions);

local TForm
scobindTfSyntaxFrAbSyn(Stab stab, AbSyn ab)
{
	TForm tf = tfSyntaxFrAbSyn(stab, ab);
	scobindTfConditions(stab, tf, scoCondListCondElt());
	return tf;
}

local void
scobindTfConditions(Stab stab, TForm tf, TfCondElt conditions)
{
	if (!tfIsPending(tf))
		return;

	if (conditions == NULL)
		return;

	tfSyntaxConditions(stab, tf, conditions);
}

/******************************************************************************
 *
 * :: LambdaInfo
 *
 *****************************************************************************/

local LambdaInfo
lambdaInfoAlloc(AbSyn lhs, AbSyn rhs, ScoConditionList condition)
{
	LambdaInfo info = (LambdaInfo) stoAlloc(OB_Other, sizeof(*info));
	info->lhs = lhs;
	info->rhs = rhs;
	info->scoCondList = condition;

	return info;
}

local void
lambdaInfoFree(LambdaInfo info)
{
	stoFree(info);
}
