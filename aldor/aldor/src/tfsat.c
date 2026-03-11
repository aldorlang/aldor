/*****************************************************************************
 *
 * tfsat.c: Type form satisfaction.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "ablogic.h"
#include "absub.h"
#include "axlobs.h"
#include "comsg.h"
#include "debug.h"
#include "format.h"
#include "ostream.h"
#include "sefo.h"
#include "tconst.h"
#include "tfsat.h"
#include "tposs.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "terror.h"
#include "tfknown.h"
#include "ti_top.h"
#include "util.h"
#include "unify.h"
#include "utform.h"
#include "utyperes.h"

Bool	tfsDebug	= false;
Bool	tfsMultiDebug	= false;
Bool	tfsExportDebug	= false;
Bool	tfsParentDebug	= false;

#define tfsDEBUG	DEBUG_IF(tfs)		afprintf
#define tfsMultiDEBUG	DEBUG_IF(tfsMulti)	afprintf
#define tfsExportDEBUG	DEBUG_IF(tfsExport)	afprintf
#define tfsParentDEBUG	DEBUG_IF(tfsParent)	afprintf

static int		tfsDepthNo;
static int		tfsSerialNo;

extern void		tiBottomUp		(Stab, AbSyn, TForm);
extern void		tiTopDown		(Stab, AbSyn, TForm);
extern Bool		tiUnaryToRaw		(Stab, AbSyn, TForm);
extern Bool		tiRawToUnary		(Stab, AbSyn, TForm);
extern Stab		stabFindLevel		(Stab, Syme);

/*
 * Naming convention:
 *	S - source type
 *	T - target type
 */

/******************************************************************************
 *
 * :: Type form satisfaction flags.
 *
 *****************************************************************************/

/*
 *	(operation modes)
 *	TFS_Probe			Allow side-fx w/o changing state.
 *	TFS_Commit			Perform side-fx operations.
 *	TFS_Missing			Collect missing exports.
 *	TFS_Sigma			Just collect the substitution.
 *      TFS_Conditions                  Allow use of abCondKnown
 *
 *	(type form embeddings)
 *	TFS_Pending			Pending T -> T
 *	TFS_AnyToNone			S -> ()
 *	TFS_Sefo			S -> T
 *
 *	TFS_CrossToTuple		Cross(A, ..., A) -> Tuple(A)
 *	TFS_CrossToMulti		Cross(A, ..., B) -> (A, ..., B)
 *	TFS_CrossToUnary		Cross(S) -> S
 *	TFS_MultiToTuple		(A, ..., A) -> Tuple(A)
 *	TFS_MultiToCross		(A, ..., B) -> Cross(A, ..., B)
 *	TFS_MultiToUnary		(S) -> S
 *	TFS_UnaryToTuple		S -> Tuple(S)
 *	TFS_UnaryToCross		S -> Cross(S)
 *	TFS_PPartialToPat		[S -> PPartial T] -> PatMatch(T, S)

 *	TFS_Unify			ForAll(x, S) -> S'
 *      TFS_Any                         S -> ForAll(x, x)
 *      TFS_AnyToPattern                S -> Pattern(S)
 *
 *	(error return modes)
 *	TFS_Fail
 *	TFS_ExportsMissing
 *	TFS_EmbedFail
 *	TFS_ArgMissing
 *	TFS_BadArgType
 *	TFS_DifferentArity
 */

/*!! Remember to update tfSatAbEmbed when these change. */
struct maskInfo {
	String name;
};
struct maskInfo tfSatMaskInfo[] = {
	{"Probe"},
	{"Commit"},
	{"Missing"},
	{"Sigma"},
	{"Info"},
	{"Conditions"},
	{"Pending"},
	{"AnyToNone"},
	{"Sefo"},
	{"CrossToTuple"},
	{"CrossToMulti"},
	{"CrossToUnary"},
	{"MultiToTuple"},
	{"MultiToCross"},
	{"MultiToUnary"},
	{"UnaryToTuple"},
	{"UnaryToCross"},
	{"UnaryToMulti"},
	{"AnyToPattern"},
	{"Unify"},
	{"AnyEmbed"},

	{"Fail"},
	{"ExportsMissing"},
	{"EmbedFail"},
	{"ArgMissing"},
	{"BadArgType"},
	{"DifferentArity"},
	{"UnifyFail"},
	{NULL}
};

#define	TFS_Succeed		((SatMask) 0)

#define	TFS_Probe		(((SatMask) 1) << 0)
#define	TFS_Commit		(((SatMask) 1) << 1)
#define	TFS_Missing		(((SatMask) 1) << 2)
#define	TFS_Sigma		(((SatMask) 1) << 3)
#define TFS_Info		(((SatMask) 1) << 4)
#define TFS_Conditions		(((SatMask) 1) << 5)

#define	TFS_Pending		(((SatMask) 1) << 6)
#define	TFS_AnyToNone		(((SatMask) 1) << 7)
#define	TFS_Sefo		(((SatMask) 1) << 8)

#define	TFS_EmbedShift		9
#define	TFS_CrossToTuple	(((SatMask) 1) << 9)
#define	TFS_CrossToMulti	(((SatMask) 1) << 10)
#define	TFS_CrossToUnary	(((SatMask) 1) << 11)
#define	TFS_MultiToTuple	(((SatMask) 1) << 12)
#define	TFS_MultiToCross	(((SatMask) 1) << 13)
#define	TFS_MultiToUnary	(((SatMask) 1) << 14)
#define	TFS_UnaryToTuple	(((SatMask) 1) << 15)
#define	TFS_UnaryToCross	(((SatMask) 1) << 16)
#define	TFS_UnaryToMulti	(((SatMask) 1) << 17)
#define TFS_AnyToPattern	(((SatMask) 1) << 18)
#define TFS_PPartialToPat	(((SatMask) 1) << 19)
#define	TFS_Unify		(((SatMask) 1) << 20)
#define	TFS_AnyEmbed		(((SatMask) 1) << 21)

#define	TFS_Fail		(((SatMask) 1) << 22)
#define	TFS_ExportsMissing	(((SatMask) 1) << 23)
#define	TFS_EmbedFail		(((SatMask) 1) << 24)
#define	TFS_ArgMissing		(((SatMask) 1) << 25)
#define	TFS_BadArgType		(((SatMask) 1) << 26)
#define	TFS_DifferentArity	(((SatMask) 1) << 27)
#define	TFS_UnifyFail		(((SatMask) 1) << 28)

#define	TFS_BitsWidth		29
#define	TFS_BitsMask		((((SatMask) 1) << TFS_BitsWidth) - 1)

#define	TFS_ModeMask		(\
				 TFS_Probe		| \
				 TFS_Commit		| \
				 TFS_Missing		| \
				 TFS_Sigma		| \
				 TFS_Info		)

#define TFS_EmbedMask		(\
				TFS_Pending		| \
				TFS_AnyToNone		| \
				TFS_Sefo		| \
				TFS_CrossToTuple	| \
				TFS_CrossToMulti	| \
				TFS_CrossToUnary	| \
				TFS_MultiToTuple	| \
				TFS_MultiToCross	| \
				TFS_MultiToUnary	| \
				TFS_UnaryToTuple	| \
				TFS_UnaryToCross	| \
				TFS_UnaryToMulti	)

#define	TFS_ErrorMask		(\
				TFS_Fail		| \
				TFS_ExportsMissing	| \
				TFS_EmbedFail		| \
				TFS_ArgMissing		| \
				TFS_BadArgType		| \
				TFS_DifferentArity	| \
				TFS_UnifyFail     	)

#define	TFS_ParnMask		(~TFS_BitsMask)

#define	tfsParNBits(n)		(((SatMask) (n)) << TFS_BitsWidth)

#define TFS_UsualMask		(TFS_EmbedMask & ~TFS_Sefo)
#define TFS_NPendingMask	(TFS_UsualMask & ~TFS_Pending)
#define TFS_NAnyToNoneMask	(TFS_UsualMask & ~TFS_AnyToNone)

#define			tfSatMode(m)		((m) & TFS_ModeMask)
#define			tfSatEmbed(m)		((m) & TFS_EmbedMask)
#define			tfSatError(m)		((m) & TFS_ErrorMask)
#define			tfSatParn(m)		((m) & TFS_ParnMask)

#define			tfSatProbe(m)		((m) & TFS_Probe)
#define			tfSatCommit(m)		((m) & TFS_Commit)
#define			tfSatMissing(m)		((m) & TFS_Missing)
#define			tfSatSigma(m)		((m) & TFS_Sigma)
#define			tfSatInfo(m)		((m) & TFS_Info)
#define			tfSatUseConditions(m)	((m) & TFS_Conditions)
#define			tfSatUnify(m)		((m) & TFS_Unify)
#define			tfSatAnyArg(m)		((m) & TFS_Any)

#define			tfSatAllow(m,c)		((m) & (c))
#define			tfSatWithout(m,c)	((m) & ~(c))

#define			tfSatResult(m,c)	(tfSatMode(m) | (c))
#define			tfSatParNFail(m, r, n)	(tfSatMode(m) | (r) | tfsParNBits(n))


#define			tfSatTrue(m)		tfSatResult(m, TFS_Succeed)
#define			tfSatFalse(m)		tfSatResult(m, TFS_Fail)

#define			tfSatInner(m)		\
	(tfSatMode(m) | tfSatAllow(m, TFS_Pending))


/******************************************************************************
 *
 * :: Local declarations.
 *
 *****************************************************************************/

local SatMask		tfSatArgPoss		(SatMask, AbSyn S, TForm T);
local SatMask		tfSatArg		(SatMask, AbSyn, TForm);


local SatMask		tfSatDOM		(SatMask, TForm S);
local SatMask		tfSatCAT		(SatMask, TForm S);
local SatMask		tfSatTYPE		(SatMask, TForm S);

local SatMask		tfSatUsePending		(SatMask, TForm S, TForm T);
local SatMask 		tfSatUsePending1	(SatMask, AbSyn, TForm, TForm);
local SatMask		tfSatEvery		(SatMask, TForm S, TForm T);
local SatMask		tfSatEach		(SatMask, TForm S, TForm T);
local SatMask		tfSatMap0		(SatMask, TForm S, TForm T);
local SatMask		tfSatTuple		(SatMask, TForm S, TForm T);
local SatMask		tfSatCross		(SatMask, TForm S, TForm T);
local SatMask		tfSatMulti		(SatMask, TForm S, TForm T);
local SatMask		tfSatExcept		(SatMask, TForm S, TForm T);

local SatMask		tfSatCatExports		(SatMask, AbSyn Sab, TForm S, TForm T);
local SatMask		tfSatThdExports		(SatMask, TForm S, TForm T);

local SatMask		tfSatExports	(SatMask,SymeList,SymeList,SymeList);
local SatMask		tfSatExport	(SatMask,SymeList,AbSyn Stf, SymeList S,Syme t, AbSub *lazy);
local AbSub		tfSatExportLazySelfSubst(SymeList mods, Sefo Sab, AbSub *lazySelfSubst);
local SatMask		tfSatParents	(SatMask,SymeList, AbSyn, SymeList,SymeList);

local SatMask		tfSatConditions		(SatMask, SymeList, Syme, Syme);
local Bool		sefoListMemberMod	(SymeList, Sefo, SefoList);
local void		tfSatPushMapConds	(TForm);
local void		tfSatPopMapConds	(TForm);
local Sefo		tfSatCond		(TForm);
local SefoList		tfSatConds		(void);

local SymeList	tfSatExportsMissing	(SatMask,SymeList,AbSyn,SymeList,SymeList);
local SymeList	tfSatParentsFilterTable	(SymeTSet, SymeList);

local void	tfSatSetPendingFail	(TForm);

/******************************************************************************
 *
 * :: Type form satisfaction bit mask accessors.
 *
 *****************************************************************************/

SatMask
tfSatHasMask(void)
{
	return TFS_Probe | TFS_NPendingMask;
}

SatMask
tfSatBupMask(void)
{
	return TFS_Probe | TFS_UsualMask | TFS_Conditions;
}

SatMask
tfSatTdnMask(void)
{
	return TFS_Commit | TFS_UsualMask | TFS_Conditions;
}

SatMask
tfSatReturnMask(void)
{
	return TFS_Commit | TFS_UsualMask;
}

SatMask
tfSatTdnInfoMask(void)
{
	return TFS_Commit | TFS_UsualMask | TFS_Info;
}

SatMask
tfSatSefMask(void)
{
	return TFS_Commit | TFS_UsualMask | TFS_Sefo;
}

SatMask
tfSatTErrorMask(void)
{
	return TFS_Probe | TFS_UsualMask;
}

Bool
tfSatSucceed(SatMask mask)
{
	return !tfSatError(mask);
}

Bool
tfSatPending(SatMask mask)
{
	return mask & TFS_Pending;
}

Bool
tfSatFailedExportsMissing(SatMask mask)
{
	return mask & TFS_ExportsMissing;
}

SatMask
tfSatWithPatContext(SatMask mask)
{
	return mask | TFS_AnyToPattern;
}


Bool
tfSatFailedEmbedFail(SatMask mask)
{
	return mask & TFS_EmbedFail;
}

Bool
tfSatFailedArgMissing(SatMask mask)
{
	return mask & TFS_ArgMissing;
}

Bool
tfSatFailedBadArgType(SatMask mask)
{
	return mask & TFS_BadArgType;
}

Bool
tfSatFailedDifferentArity(SatMask mask)
{
	return mask & TFS_DifferentArity;
}

Length
tfSatParN(SatMask mask)
{
	return tfSatParn(mask) >> TFS_BitsWidth;
}

Length
tfSatArgN(AbSyn ab, Length argc, AbSynGetter argf, Length parN, TForm S)
{
	Length	parmc, ai;
	TForm	tfi;
	Bool	def;

	parmc = tfMapHasDefaults(S) ? tfMapArgc(S) : argc;
	tfi = tfAsMultiArgN(tfMapArg(S), parmc, parN);
	tfAsMultiSelectArg(ab, argc, parN, argf, tfi, &def, &ai);

	return ai;
}

AbEmbed
tfSatAbEmbed(SatMask mask)
{
	SatMask		result = tfSatEmbed(mask);
	/* -1 to allow for AB_Embed_Identity */
	AbEmbed		embed = ((AbEmbed) result) >> (TFS_EmbedShift-1);

	return embed;
}

/* Return the embedding needed to convert tf1 into tf2 */
AbEmbed
tfSatEmbedType(TForm tf1, TForm tf2)
{
	TFormTag	t1, t2;

	tfFollow(tf1);
	tfFollow(tf2);

	/* Deal with delta-equality */
	tf1 = tfDefineeMaybeType(tf1);
	tf2 = tfDefineeMaybeType(tf2);
	/* Ignore exceptions for the purposes of embedding as well */
	tf2 = tfIgnoreExceptions(tf2);

	// FIXME: This is for examples like Union(x: Cross(A, B))
	// Need to figure out what the best thing here is..
	tf1 = tfDefineeType(tf1);

	t1 = tfTag(tf1);
	t2 = tfTag(tf2);

	if (t1 == t2)				return AB_Embed_Identity;
	else if (t1 == TF_Exit)			return AB_Embed_Identity;
	else if (t1 == TF_Cross) {
		if	(t2 == TF_Tuple)	return AB_Embed_CrossToTuple;
		else if (t2 == TF_Cross)	return AB_Embed_Identity;
		else if (t2 == TF_Multiple)	return AB_Embed_CrossToMulti;
		else				return AB_Embed_CrossToUnary;
	}
	else if (t1 == TF_Multiple) {
		if	(t2 == TF_Tuple)	return AB_Embed_MultiToTuple;
		else if (t2 == TF_Cross)	return AB_Embed_MultiToCross;
		else if (t2 == TF_Multiple)	return AB_Embed_Identity;
		else				return AB_Embed_MultiToUnary;
	}
	else if (t1 == TF_Map) {
		if (t2 == TF_PatMatch)		return AB_Embed_ApplyPatCall;
		else				return AB_Embed_Identity;
	}
	else {
		if	(t2 == TF_Tuple)	return AB_Embed_UnaryToTuple;
		else if (t2 == TF_Cross)	return AB_Embed_UnaryToCross;
		else if (t2 == TF_Multiple)	return AB_Embed_UnaryToMulti;
		else				return AB_Embed_Identity;
	}
}

TForm
tfsEmbedResult(TForm tf, AbEmbed embed)
{
	TFormList tfl;
	int i;

	if (!tf) return tf;

	tf  = tfDefineeType(tf);
	if (tfIsDefinedType(tf)) {
		tf = tfDefinedVal(tf);
	}

	if (embed & AB_Embed_ApplyPatCall) {
		return tfPatMatch(tfPPartialAsMapArg(tfMapRet(tf)), tfMapArg(tf));
	}

	/* See comment in tiTfTopDown 
	if ((embed & AB_Embed_Identity) && embed != AB_Embed_Identity) {
		afprintf(dbOut, "Embed %oAbEmbed %pTForm\n", embed, tf);
		bug("odd embed");
	}
	*/
	
	if (embed & AB_Embed_Identity || embed == 0)
		return tf;

	if (embed & AB_Embed_CrossToTuple) {
		assert(tfIsCross(tf));
		assert(tfCrossArgc(tf));
		return tfTuple(tfCrossArgN(tf, int0));
	}
	if (embed & AB_Embed_CrossToMulti) {
		assert(tfIsCross(tf));
		tfl = listNil(TForm);
		for (i = tfArgc(tf) - 1; i >= 0; i--)
			tfl = listCons(TForm)(tfCrossArgv(tf)[i], tfl);
		tf = tfMultiFrList(tfl);
		listFree(TForm)(tfl);
		return tf;
	}
	if (embed & AB_Embed_CrossToUnary) {
		assert(tfIsCross(tf));
		assert(tfCrossArgc(tf));
		return tfCrossArgN(tf, int0);
	}
	if (embed & AB_Embed_MultiToTuple) {
		assert(tfIsMulti(tf));
		assert(!tfIsEmptyMulti(tf));
		return tfTuple(tfMultiArgN(tf, int0));
	}
	if (embed & AB_Embed_MultiToCross) {
		assert(tfIsMulti(tf));
		return tfCrossFrMulti(tf);
	}
	if (embed & AB_Embed_MultiToUnary) {
		assert(tfIsMulti(tf));
		assert(!tfIsEmptyMulti(tf));
		return tfMultiArgN(tf, int0);
	}
	if (embed & AB_Embed_UnaryToTuple) {
		return tfTuple(tf);
	}
	if (embed & AB_Embed_UnaryToCross) {
		return tfCross(1, tf);
	}
	if (embed & AB_Embed_UnaryToMulti) {
		return tfMulti(1, tf);
	}

	if (embed & AB_Embed_ApplyPatCall) {
		return tfPatMatch(tfPPartialAsMapArg(tfMapRet(tf)), tfMapArg(tf));
	}

	return tf;
}

/******************************************************************************
 *
 * :: External entry points.
 *
 *****************************************************************************/

Bool
tfSatisfies(TForm S, TForm T)
{
	SatMask		mask = TFS_Commit | TFS_UsualMask;
	return tfSatBit(mask, S, T);
}

Bool
tfSatisfies1(AbSyn Sab, TForm S, TForm T)
{
	SatMask		mask = TFS_Commit | TFS_UsualMask | TFS_Conditions;
	return tfSatSucceed(tfSat1(mask, Sab, S, T));
}

Bool
tfSatValues(TForm S, TForm T)
{
	SatMask		mask = TFS_Commit | TFS_NAnyToNoneMask;
	return tfSatBit(mask, S, T);
}

Bool
tfSatReturn(TForm S, TForm T)
{
	SatMask		mask = TFS_Commit | TFS_UsualMask;
	return tfSatBit(mask, S, T);
}

Bool
tfSatDom(TForm S)
{
	SatMask		mask = TFS_Probe;
	return tfSatSucceed(tfSatDOM(mask, S));
}

Bool
tfSatCat(TForm S)
{
	SatMask		mask = TFS_Probe;
	return tfSatSucceed(tfSatCAT(mask, S));
}

Bool
tfSatType(TForm S)
{
	SatMask		mask = TFS_Probe;
	return tfSatSucceed(tfSatTYPE(mask, S));
}

AbSub
tfSatSubList(AbSyn ab)
{
	AbSub	sigma;

	switch (abTag(ab)) {
	case AB_Id:
		sigma = absNew(stabFile());
		break;
	case AB_Apply: {
		SatMask	mask = TFS_Probe | TFS_Sigma | TFS_UsualMask;
		AbSyn	op   = abApplyOp(ab);
		Length	argc = abApplyArgc(ab);
		TForm	S;

		assert(abState(op) == AB_State_HasUnique);

		S     = abTUnique(op);
		sigma = tfSatSubList(op);
		mask  = tfSatMapArgs(mask, sigma, S, ab, argc, abApplyArgf);

		if (!tfSatSucceed(mask)) {
			/*!! bug("tfSatSubList:  tfSatMapArgs failed."); */
			absFree(sigma);
			sigma = absFail();
		}
		break;
	}
	default:
		sigma = absFail();
		break;
	}

	return sigma;
}


/******************************************************************************
 *
 * :: tfSatCase
 * (maybe not needed)
 *****************************************************************************/

Bool
tfSatCase(TForm S, TForm T)
{
	if (tfIsMulti(T)) {
		Bool ok = true;
		Length argc = tfAsMultiArgc(S);
		if (argc != tfMultiArgc(T)) {
			return false;
		}
		
		for (int i=0; i<tfArgc(T) && ok; i++) {
			ok = tfSatisfies(tfPattern(tfAsMultiArgN(S, argc, i)), tfMultiArgN(T, i));
		}
		return ok;
	}
	else if (!tfIsPattern(T)) {
		return false;
	}
	else if (tfIsExit(tfPatternArg(T))) {
		return true;
	}
	return tfSatisfies(tfPattern(S), T);
}


/******************************************************************************
 *
 * :: tfSatMap
 *
 *****************************************************************************/

SatMask
tfSatMap(SatMask mask, Stab stab, TForm S, TForm T,
	 AbSyn ab, Length argc, AbSynGetter argf)
{
	SatMask		result;
	TForm		Sret;
	AbSub		sigma;

	assert(tfIsAnyMap(S));
	Sret = tfMapRet(S);

	sigma = absNew(stab);

	result = tfSatMapArgs(mask, sigma, S, ab, argc, argf);
	if (tfSatSucceed(result)) {
		Sret = tformSubst(sigma, Sret);
		if (tfIsPatMatch(S)) {
			Sret = tfPattern(Sret);
		}
		result = tfSatEmbed(result) | tfSat1(mask, ab, Sret, T);

		if (tfSatSucceed(result) && tfSatCommit(mask)) {
			abTUnique(ab) = Sret;
		}
	}

	absFreeDeeply(sigma);

	return result;
}

SatMask
tfSatMapArgs(SatMask mask, AbSub sigma, TForm S,
	     AbSyn ab, Length argc, AbSynGetter argf)
{
	assert(tfIsAnyMap(S));
	/*
	if (tfIsPPartialMap(S) && tfSatEmbedPartialToPat(mask)) {
		
	}
	*/
	return tfSatAsMulti(mask, sigma, tfMapArg(S), S, ab, argc, argf);
}

SatMask
tfSatAsMulti(SatMask mask, AbSub sigma, TForm S, TForm TScope,
	 AbSyn ab, Length argc, AbSynGetter argf)
{
	SatMask		result = tfSatTrue(mask);
	Length		i, parmc, usedc;
	int		serialThis;
	Bool		packed = tfIsPackedMap(TScope);
	Bool 		pattern = tfIsPatMatch(TScope);
	AbSyn		abc = NULL;

	if (DEBUG(tfsMulti)) {
		afprintf(dbOut, "SatAsMulti: S: %pTForm Scope: %pTForm\n", S, TScope);
	}

	if (tfAsMultiEmbed(S, argc) == AB_Embed_Fail)
		return tfSatResult(mask, TFS_EmbedFail);

	tfsSerialNo += 1;
	tfsDepthNo  += 1;
	serialThis   = tfsSerialNo;

	if (DEBUG(tfsMulti)) {
		fprintf(dbOut, "->Tfc: %*s%d= ", tfsDepthNo, "", serialThis);
		tfPrint(dbOut, S);
		fprintf(dbOut, " satisfies ");
		abPrint(dbOut, ab);
		fnewline(dbOut);
	}

	parmc = tfMultiHasDefaults(S) ? tfAsMultiArgc(S) : argc;

	if (parmc != 1 && tfIsTuple(tfDefineeType(S))) {
		abc = abNewEmpty(AB_Comma, parmc);
	}

	for (i = 0, usedc = 0; i < parmc; i += 1) {
		AbSyn	abi;
		TForm	tfi;
		Syme	syme;
		Length	pi, ai;
		Bool	def;
		SatMask	maski = tfSatTrue(mask);

		pi   = (S->rho ? S->rho[i] : i);
		tfi  = tfAsMultiArgN(S, parmc, pi);
		abi  = tfAsMultiSelectArg(ab, argc, pi, argf, tfi, &def, &ai);

		if (!abi) {
			result = tfSatParNFail(mask, TFS_ArgMissing, pi);
			break;
		}
		if (!def) usedc += 1;

		syme = tfDefineeSyme(tfi);
		tfi  = tfDefineeType(tfi);
		tfi  = tformSubst(sigma, tfi);
		if (pattern) {
			tfi = tfPattern(tfi);
		}
		/* Check to see if abi satisfies tfi. */
		if (!def && !tfSatSigma(mask)) {
			maski = tfSatArg(mask, abi, tfi);
			if (!tfSatSucceed(maski)) {
				result = tfSatParNFail(mask, TFS_BadArgType, pi);
				break;
			}
			if (tfSatPending(maski))
				result = tfSatResult(mask, TFS_Pending);
		}
		/* Type infer abi of type tfi, if needed. */
		if (tfSatCommit(mask)) {
			/*
			 * Ensure that we have the correct set of types
			 * for this expression. In almost every case we
			 * will return immediately but it doesn't hurt
			 * to make sure.
			 */
			tiTopFns()->tiBottomUp(absStab(sigma), abi, tfUnknown);


			/*
			 * Occasionally tiTopDown returns false and in
			 * the past this meant that we had to TFS_Fail
			 * or something similar. Seems fine now.
			 */

			tiTopFns()->tiTopDown(absStab(sigma), abi, tfi);

			if (abUse(abi) != AB_Use_Type) {
				/* Double check the type on abi (now unique) against tfi
				 * - it may have changed due to embeddings being applied
				 * within abi itself. (Note: embeddings are applied by
				 * callers, with the type on the absyn being the 'original'
				 * type).
				 * cf. axllib/tests/bug13138 and aldor/tests/hang
				 */
				SatMask checkMask = tfSatArg(mask, abi, tfi);
				abAddTContext(abi, tfSatAbEmbed(checkMask));
			}
		}
		/* Install the packed embedding on abi, if needed. */
		if (tfSatCommit(mask) && packed)
			if (!tiTopFns()->tiUnaryToRaw(absStab(sigma), abi, tfi)) {
				result = tfSatParNFail(mask, TFS_BadArgType, pi);
				break;
			}
		/*
		 * Extend the sublist for dependent or recursive maps.
		 *
		 * BUG: if tfi is a tfSyntax then we will almost certainly
		 * fail to spot any dependencies. See bug 1303 for example.
		 */
		if (syme && (tfSymeInducesDependency(syme, TScope)
			     || listMemq(Syme)(tfSymes(TScope), syme)
			     || listMember(Syme)(tfSymes(TScope), syme, symeEqual))) {
			abi = sefoCopy(abi);
			tiTopFns()->tiBottomUp(absStab(sigma), abi, tfUnknown);
			tiTopFns()->tiTopDown (absStab(sigma), abi, tfi);

			if (abState(abi) == AB_State_HasUnique) {
				if (absFVars(sigma)) absSetFVars(sigma, NULL);
				sigma = absExtend(syme, abi, sigma);
			}
			else {
				result = tfSatParNFail(mask, TFS_BadArgType, pi);
				break;
			}
		}
		if (abc) {
			abArgv(abc)[i] = sefoCopy(abi);
		}
	}

	/* Extend the sublist for dependent and recursive maps. */
	if (tfSatSucceed(result) && abc) {
		Syme	syme = tfDefineeSyme(S);
		TForm	tfi = tfDefineeType(S);
		if (syme && (tfSymeInducesDependency(syme, TScope) ||
			     listMemq(Syme)(tfSymes(TScope), syme))) {
			tiTopFns()->tiBottomUp(absStab(sigma), abc, tfUnknown);
			tiTopFns()->tiTopDown (absStab(sigma), abc, tfi);
			if (abState(abc) == AB_State_HasUnique) {
				if (absFVars(sigma)) absSetFVars(sigma, NULL);
				sigma = absExtend(syme, abc, sigma);
			}
			else {
				result = tfSatParNFail(mask, TFS_BadArgType, 1);
			}
		}
	}

	/* Install the packed embedding on the return value, if needed. */
	if (tfSatSucceed(result) && tfSatCommit(mask) && packed)
		if (!tiTopFns()->tiRawToUnary(absStab(sigma), ab, tfMapRet(TScope)))
			result = tfSatResult(mask, TFS_EmbedFail);

	if (tfSatSucceed(result) && usedc < argc)
		result = tfSatResult(mask, TFS_DifferentArity);

	if (DEBUG(tfsMulti)) {
		fprintf(dbOut, "<-Tfc: %*s%d= %s!", tfsDepthNo, "",
			serialThis, boolToString(tfSatSucceed(result)));
		fnewline(dbOut);
	}
	tfsDepthNo -= 1;

	return result;
}

SatMask
tfSatArg(SatMask mask, AbSyn ab, TForm T)
{
	SatMask		result;

	mask &= ~TFS_AnyToNone;

	/* We'd rather not use pending embeddings to decide argc questions. */
	if (tfIsMulti(T) && tfIsPending(T))
		mask &= ~TFS_Pending;

	switch (abState(ab)) {
	case AB_State_HasPoss:
		result = tfSatArgPoss(mask, ab, T);
		break;

	case AB_State_HasUnique:
		result = tfSat1(mask, ab, abTUnique(ab), T);
		break;

	default:
		result = tfSatFalse(mask);
		break;
	}

	return result;
}

local SatMask
tfSatArgPoss(SatMask mask, AbSyn Sab, TForm T)
{
	UTFormList	l;
	SatMask		result;
	TPoss S = abTPoss(Sab);

	if (tfSatAllow(mask, TFS_Pending) && tpossIsUnique(S)) {
		tcSatPush(tpossUnique(S), T);
		result = tfSatUsePending1(mask, Sab, tpossUnique(S), T);
		tcSatPop();
		if (tfSatSucceed(result))
			return result;
	}

	TPossIterator ip;
	for (tpossITER(ip, S); tpossMORE(ip); tpossSTEP(ip)) {
		UTForm s = tpossUELT(ip);
		result = tfSat1(mask, Sab, utformConstOrFail(s), T);
		if (tfSatSucceed(result))
			return result;
	}

	return tfSatFalse(mask);
}

/******************************************************************************
 *
 * :: tfSat
 *
 *****************************************************************************/

/*
 * Succeed if S satisfies T.
 */
Bool
tfSatBit(SatMask mask, TForm S, TForm T)
{
	return tfSatSucceed(tfSat(mask, S, T));
}

SatMask
tfSat(SatMask mask, TForm S, TForm T)
{
	return tfSat1(mask, 0, S, T);
}

SatMask
tfSat1(SatMask mask, AbSyn Sab, TForm S, TForm T)
{
	SatMask		result = tfSatFalse(mask);
	int		serialThis;

	S = tfFollowOnly(S);
	T = tfFollowOnly(T);

	tfsSerialNo += 1;
	serialThis   = tfsSerialNo;

	/* If we can determine satisfaction w/o using tfFollow, do so. */
	if (tfIsSubst(S)) {
		tfsDEBUG(dbOut, "(%d - skip subst\n", serialThis);
		result = tfSat(mask & ~TFS_Pending, tfSubstArg(S), T);
		tfsDEBUG(dbOut, " %d - skip subst - %oBool)\n", serialThis, tfSatSucceed(result));
		if (tfSatSucceed(result))
			return result;
	}
	S = tfDefineeType(S);
	T = tfDefineeType(T);

	if (tfSatAllow(mask, TFS_Sefo))
		return tfSatResult(mask, TFS_Sefo);

	tfsDepthNo  += 1;

	if (DEBUG(tfs)) {
		fprintf(dbOut, "->Tfs: %*s%d= ", tfsDepthNo, "", serialThis);
		tfPrint(dbOut, S);
		fprintf(dbOut, " satisfies ");
		tfPrint(dbOut, T);
		fnewline(dbOut);
	}

	tcSatPush(S, T);

	if (S == T)
		result = tfSatTrue(mask);

	/*
	 * tfType
	 */
	else if (tfIsType(T) || tfIsTypeSyntax(T))
		result = tfSatTYPE(mask, S);

	/*
	 * tfCategory
	 */
	else if (tfIsCategory(T) || tfIsCategorySyntax(T))
		result = tfSatCAT(mask, S);

	/*
	 * tfExit
	 */
	else if (tfIsExit(S))
		result = tfSatTrue(mask);
	/*
	 * tfUnknown
	 */
	else if (tfIsUnknown(T))
		result = tfSatTrue(mask);

	else if (tfIsUnknown(S))
		result = tfSatFalse(mask);

	/*
	 * tfSyntax
	 */
	else if (tfIsSyntax(S) || tfIsSyntax(T)) {
		if (tfSatAllow(mask, TFS_Pending))
			result = tfSatUsePending(mask, S, T);
	}

	/*
	 * tfRaw
	 */
	else if (tfIsRaw(S))
		result = tfSat(mask, tfRawType(tfRawArg(S)), T);

	else if (tfIsRaw(T))
		result = tfSat(mask, S, tfRawType(tfRawArg(T)));

	/*
	 * tfPattern
	 */
	else if (tfIsPattern(T)) {
		if (tfIsPatternExit(T)) {
			result = tfSatTrue(mask);
		}
		else if (tfIsPattern(S)) {
			result = tfSat(mask, tfPatternArg(S), tfPatternArg(T));
		}
		else if (tfSatAllow(mask, TFS_AnyToPattern)) {
			result = tfSat(tfSatWithout(mask, TFS_AnyToPattern), S, tfPatternArg(T));
			result = tfSatResult(result, TFS_AnyToPattern);
		}
	}
	/*
	 * tfExcept
	 */
	else if (tfIsExcept(S) && !tfIsExcept(T))
		result = tfSat(mask, tfExceptType(S), T);
	else if (tfIsExcept(T))
		result = tfSatExcept(mask, S, T);
	/*
	 * tfMap
	 */
	else if (tfIsAnyMap(T))
		result = tfSatMap0(mask, S, T);

	/*
	 * tfTuple
	 */
	else if (tfIsTuple(T))
		result = tfSatTuple(mask, S, T);

	/*
	 * tfCross
	 */
	else if (tfIsCross(T)) {
		result = tfSatCross(mask, S, T);
		if (!(tfSatSucceed(result)))
		{
			/* Delta-equality of S with cross */
			TForm Snorm = tfDefineeBaseType(S);
			result = tfSatCross(mask, Snorm, T);
		}
	}

	/*
	 * tfMultiple
	 */
	else if (tfIsMulti(T)) {
		result = tfSatMulti(mask, S, T);
		if (!(tfSatSucceed(result)))
		{
			/* Delta-equality of S with multi */
			TForm Snorm = tfDefineeBaseType(S);
			result = tfSatMulti(mask, Snorm, T);
		}
	}

	/*
	 * Other product rules.
	 */
	else if (tfIsCross(S)) {
		TForm	Sarg = tfCrossArgN(S, int0);
		/* Embed Cross(S) in S. */
		if (tfSatAllow(mask, TFS_CrossToUnary) &&
		    tfCrossArgc(S) == 1 &&
		    tfSatBit(tfSatInner(mask), Sarg, T))
		{
			result = tfSatResult(mask, TFS_CrossToUnary);
		}
		else /* The logic of this needs cleaning up */
		{
			/* Delta-equality of T with cross/multi */
			TForm Tnorm = tfDefineeBaseType(T);
			if (tfIsCross(Tnorm))
				result = tfSatCross(mask, S, Tnorm);
			else if (tfIsMulti(Tnorm))
				result = tfSatMulti(mask, S, Tnorm);
		}
	}
	else if (tfIsMulti(S)) {
		TForm	Sarg = tfMultiArgN(S, int0);
		/* Embed Multi(S) in S. */
		if (tfSatAllow(mask, TFS_MultiToUnary) &&
		    tfMultiArgc(S) == 1 &&
		    tfSatBit(tfSatInner(mask), Sarg, T))
		{
			result = tfSatResult(mask, TFS_MultiToUnary);
		}
		else /* The logic of this needs cleaning up */
		{
			/* Delta-equality of T with cross/multi */
			TForm Tnorm = tfDefineeBaseType(T);
			if (tfIsCross(Tnorm))
				result = tfSatCross(mask, S, Tnorm);
			else if (tfIsMulti(Tnorm))
				result = tfSatMulti(mask, S, Tnorm);
		}
	}

	/*
	 * Category forms
	 */
	else if (tfSatSucceed(tfSatDOM(mask, T)))  {
		if (tfSatSucceed(tfSatDOM(mask, S))) {
 			if (tfSatUseConditions(mask) && abCondKnown != NULL
			    && Sab != NULL) {
				if (tfIsPending(S)) {
					if (tfSatAllow(mask, TFS_Pending)) {
						result = tfSatUsePending1(mask,
									  Sab, S, T);
						return result;
					}
				}
				TForm tf = ablogImpliedType(abCondKnown, Sab, S);
				if (tf != NULL) {
					tfsDEBUG(dbOut, "Swapping type: %pTForm to %pTForm\n", S, tf);
					S = tf;
				}
			}
			result = tfSatCatExports(mask, Sab, S, T);
		}
	}

	/*
	 * Third forms
	 */
	else if (tfSatSucceed(tfSatCAT(mask, T))) {
		if (tfSatSucceed(tfSatCAT(mask, S)))
			result = tfSatThdExports(mask, S, T);
	}

	/*
	 * Default case
	 */
	else if (tfEqual(S, T))
		result = tfSatTrue(mask);

	tcSatPop();

	if (DEBUG(tfs)) {
		fprintf(dbOut, "<-Tfs: %*s%d= %s!", tfsDepthNo, "",
			serialThis, boolToString(tfSatSucceed(result)));
		if (tfSatEmbed(result))
			fprintf(dbOut, " (after embedding)");
		if (tfSatPending(result))
			afprintf(dbOut, " (pending) - %pTForm", tfSatGetPendingFail());
		fnewline(dbOut);
	}
	tfsDepthNo -= 1;

	return result;

}

/******************************************************************************
 *
 * :: tfSat cases
 *
 *****************************************************************************/

/*
 * Succeed if S is a category.
 */
#if 0
local SatMask tfSatDOM0(SatMask mask, TForm S);
local SatMask tfSatCAT0(SatMask mask, TForm S);

local SatMask
tfSatDOM(SatMask mask, TForm S)
{
	fprintf(dbOut, "(SatDom: ");
	tfPrintDb(S);
	mask = tfSatDOM0(mask, S);
	fprintf(dbOut, ")");
	return mask;
}

local SatMask
tfSatCAT(SatMask mask, TForm S)
{
	fprintf(dbOut, "(SatCat: ");
	tfPrintDb(S);
	mask = tfSatCAT0(mask, S);
	fprintf(dbOut, ")");
	return mask;
}
#endif

local SatMask
tfSatDOM(SatMask mask, TForm S)
{
	TForm	nS = tfDefineeTypeSubst(S);

	if (tfIsAnyMap(nS))
		return tfSatFalse(mask);
	else if (tfIsRawRecord(nS) || tfIsRecord(nS) ||
		 tfIsUnion(nS) || tfIsEnum(nS) ||
		 tfIsTrailingArray(nS))
		return tfSatFalse(mask);
	/*!! This clause is really not quite right. */
	else if (tfIsType(nS) || tfIsTypeSyntax(nS) || tfIsSyntax(nS))
		return tfSatTrue(mask);

	else if (tfIsWith(nS) || tfIsWithSyntax(nS) || tfIsIf(nS) ||
	    tfIsJoin(nS) || tfIsMeet(nS))
		return tfSatTrue(mask);

	else if (tfSatSucceed(tfSatCAT(mask, S)))
		return tfSatFalse(mask);
	else
		return tfSatCAT(mask, tfGetCategory(tfDefineeType(S)));
}


/*
 * Succeed if S is a third-order type.
 */
local SatMask
tfSatCAT(SatMask mask, TForm S)
{
	TForm	nS = tfDefineeTypeSubst(S);

	if (tfIsAnyMap(nS))
		return tfSatFalse(mask);
	else if (tfIsRawRecord(nS) || tfIsRecord(nS) ||
		 tfIsUnion(nS) || tfIsEnum(nS) ||
		 tfIsTrailingArray(nS))
		return tfSatFalse(mask);
	else if (tfIsCategory(nS) || tfIsCategorySyntax(nS) || tfIsThird(nS))
		return tfSatTrue(mask);
	else
		return tfSatFalse(mask);
}

/*
 * Succeed if S is a higher-order type.
 */
local SatMask
tfSatTYPE(SatMask mask, TForm S)
{
	if (tfIsAnyMap(S))
		return tfSatFalse(mask);

	else if (tfSatSucceed(tfSatDOM(mask, S)))
		return tfSatTrue(mask);

	else if (tfSatSucceed(tfSatCAT(mask, S)))
		return tfSatTrue(mask);

	else if (tfSatAllow(mask, TFS_Pending))
		return tfSatUsePending(mask, tfDefineeType(S), tfType);

	else
		return tfSatFalse(mask);
}

local SatMask
tfSatUsePending(SatMask mask, TForm S, TForm T)
{
	return tfSatUsePending1(mask, NULL, S, T);
}

local SatMask
tfSatUsePending1(SatMask mask, AbSyn Sab, TForm S, TForm T)
{
	SatMask		result;

	if (tfIsPending(S)) {
		tfSatSetPendingFail(S);
		result = tfSatResult(mask, TFS_Pending);
		if (tfSatCommit(mask))
			tcNewSat1(S, abCondKnown, Sab, S, T, NULL);
		return result;
	}
	if (tfIsPending(T)) {
		tfSatSetPendingFail(T);
		result = tfSatResult(mask, TFS_Pending);
		if (tfSatCommit(mask))
			tcNewSat1(T, abCondKnown, Sab, S, T, NULL);
		return result;
	}

	return tfSatFalse(mask);
}

/*
 * Succeed if every argument of S satisfies T.
 */
local SatMask
tfSatEvery(SatMask mask, TForm S, TForm T)
{
	Length	i;

	for (i = 0; i < tfArgc(S); i += 1)
		if (!tfSatBit(tfSatInner(mask), tfArgv(S)[i], T))
			return tfSatFalse(mask);

	return tfSatTrue(mask);
}

/*
 * Succeed if each argument of S satisfies the corresponding argument of T.
 */
local SatMask
tfSatEach(SatMask mask, TForm S, TForm T)
{
	SatMask		result = tfSatTrue(mask);
	Stab		stab = tfGetStab(T);
	AbSub		sigma;
	Length		i, argc = tfArgc(S);

	assert(tfArgc(S) == tfArgc(T));

	for (i = 0; !stab && i < argc; i += 1) {
		TForm	Targ = tfArgv(T)[i];
		Syme	Tsyme = tfDefineeSyme(Targ);

		if (Tsyme) stab = stabFindLevel(stabFile(), Tsyme);
	}

	sigma = absNew(stab);
	for (i = 0; tfSatSucceed(result) && i < argc; i += 1) {
		TForm	Sarg = tfArgv(S)[i];
		TForm	Targ = tformSubst(sigma, tfArgv(T)[i]);
		Syme	Ssyme = tfDefineeSyme(Sarg);
		Syme	Tsyme = tfDefineeSyme(Targ);
		AbSyn	ab = NULL;

		if (!tfSatBit(tfSatInner(mask), Sarg, Targ))
			result = tfSatFalse(mask);

		if (Ssyme && Ssyme != Tsyme)
			ab = abFrSyme(Ssyme);
		else if (tfIsDefine(Sarg))
			ab = tfGetExpr(tfDefineVal(Sarg));

		/* Extend the sublist for dependent symes. */
		if (stab && ab && Tsyme) {
			tiTopFns()->tiBottomUp(stab, ab, tfUnknown);
			tiTopFns()->tiTopDown (stab, ab, Targ);
			if (abState(ab) == AB_State_HasUnique) {
				if (absFVars(sigma))
					absSetFVars(sigma, NULL);
				sigma = absExtend(Tsyme, ab, sigma);
			}
			else
				result = tfSatFalse(mask);
		}
	}

	absFree(sigma);
	return result;
}

local SatMask tfSatMap1(SatMask mask, TForm S, TForm T);

local SatMask
tfSatMap0(SatMask mask, TForm S, TForm T)
{
	assert(tfIsAnyMap(T));

	if (!tfIsAnyMap(S)) {
		return tfSatFalse(mask);
	}
	if (tfIsPatMatch(S) && tfIsPatMatch(T)) {
		return tfSatMap1(mask, S, T);
	}
	else if (tfIsFunctionMap(S) && tfIsFunctionMap(T)) {
		return tfSatMap1(mask, S, T);
	}
	else if (tfSatAllow(mask, TFS_AnyToPattern)
		 && tfIsPPartialMap(S)) {
		TForm patS = tfsEmbedResult(S, AB_Embed_ApplyPatCall);
		SatMask r = tfSatMap1(mask, patS, T);
		tfsDEBUG(dbOut, "SatPPartial %oBool - %oSatMask %pTForm %pTForm\n", tfSatSucceed(r), r, patS, T);
		return tfSatSucceed(r) ? tfSatResult(mask, TFS_PPartialToPat) : tfSatFalse(mask);
	}
	return tfSatFalse(mask);
}

local SatMask
tfSatMap1(SatMask mask, TForm S, TForm T)
{
	SatMask		result = tfSatFalse(mask);
	SatMask		mask0 = tfSatInner(mask);

	if (!(tfTag(T) == tfTag(S) && tfMapArgc(T) == tfMapArgc(S)))
		/* result = tfSatFalse(mask) */;

	else if (tfIsDependentMap(S) && tfIsPending(S) &&
		 tfSatAllow(mask, TFS_Pending))
		result = tfSatUsePending(mask, S, T);

	else if (tfIsDependentMap(S)) {
		Stab	stab = tfGetStab(S);
		AbSub	sigma = absNew(stab);
		Length	i, argc = tfMapArgc(S);

		tfSatPushMapConds(T);
		result = tfSatTrue(mask0);
		for (i = 0; tfSatSucceed(result) && i < argc; i += 1) {
			TForm	Sarg = tformSubst(sigma, tfMapArgN(S,i));
			TForm	Targ = tfMapArgN(T,i);
			Syme	Ssyme = tfDefineeSyme(Sarg);
			Syme	Tsyme = tfDefineeSyme(Targ);

			result = tfSatEmbed(result) |
				tfSat(mask0, Targ, Sarg);

			/* Extend the sublist for dependent symes. */
			if (Ssyme && Tsyme && Ssyme != Tsyme &&
			    (tfSymeInducesDependency(Ssyme, S) ||
                             listMember(Syme)(tfSymes(S), Ssyme, symeEqual))) {
			     /* listMemq(Syme)(tfSymes(S), Ssyme))) { */ /* Commented by C.O. for ALMA usage*/
				AbSyn	ab = abFrSyme(Tsyme);

				/** CODE INTRODUCED BY C.O. for ALMA usage**/
				int p; listFind(Syme)(tfSymes(S), Ssyme, symeEqual, &p);
                                if(p>-1) {
					Ssyme = listElt(Syme)(tfSymes(S), p);
                                }
				/*****************************/

				tiTopFns()->tiBottomUp(absStab(sigma), ab, tfUnknown);
				tiTopFns()->tiTopDown (absStab(sigma), ab, Sarg);
				if (abState(ab) == AB_State_HasUnique) {
					if (absFVars(sigma))
						absSetFVars(sigma, NULL);
					sigma = absExtend(Ssyme, ab, sigma);
				}
				else {
					result = tfSatResult(mask,
							TFS_BadArgType);
				}
			}
		}


		if (tfSatSucceed(result)) {
			TForm	Sret = tformSubst(sigma, tfMapRet(S));
			TForm	Tret = tfMapRet(T);
			result = tfSatEmbed(result) |
				tfSat(mask0, Sret, Tret);
		}
		tfSatPopMapConds(T);
		absFree(sigma);
	}

	else {
		tfSatPushMapConds(T);
		result = tfSat(mask0, tfMapArg(T), tfMapArg(S));
		if (tfSatSucceed(result))
			result = tfSatEmbed(result) |
				tfSat(mask0, tfMapRet(S), tfMapRet(T));
		tfSatPopMapConds(T);
	}

	return result;
}

local SatMask
tfSatTuple(SatMask mask, TForm S, TForm T)
{
	TForm		Targ = tfTupleArg(T);
	SatMask		result = tfSatFalse(mask);

	if (tfIsTuple(S))
		result = tfSat(mask, tfTupleArg(S), Targ);

	else if (!tfSatEmbed(mask))
		/* result = tfSatFalse(mask) */;

	else if (tfIsCross(tfDefineeTypeSubst(S))) {
		/* Embed Cross(A, ..., A) in Tuple(A). */
		if (tfSatAllow(mask, TFS_CrossToTuple) &&
		    tfSatSucceed(tfSatEvery(mask, S, Targ)))
			result = tfSatResult(mask, TFS_CrossToTuple);

		/* Embed S in Tuple(S). */
		else if (tfSatAllow(mask, TFS_UnaryToTuple) &&
			 tfSatBit(tfSatInner(mask), S, Targ))
			result = tfSatResult(mask, TFS_UnaryToTuple);
	}

	else if (tfIsMulti(S)) {
		/* Embed Multi(A, ..., A) in Tuple(A). */
		if (tfSatAllow(mask, TFS_MultiToTuple) &&
		    tfSatSucceed(tfSatEvery(mask, S, Targ)))
			result = tfSatResult(mask, TFS_MultiToTuple);
	}

	else {
		/* Embed S in Tuple(S). */
		if (tfSatAllow(mask, TFS_UnaryToTuple) &&
		    tfSatBit(tfSatInner(mask), S, Targ))
			result = tfSatResult(mask, TFS_UnaryToTuple);
	}

	return result;
}

local SatMask
tfSatCross(SatMask mask, TForm S, TForm T)
{
	Length		argc = tfCrossArgc(T);
	TForm		Targ = tfCrossArgN(T, int0);
	SatMask		result = tfSatFalse(mask);

	if (tfIsCross(S)) {
		if (tfCrossArgc(S) == argc &&
		    tfSatSucceed(tfSatEach(mask, S, T)))
			result = tfSatTrue(mask);

		/* Embed S in Cross(S). */
		else if (tfSatAllow(mask, TFS_UnaryToCross) &&
			 argc == 1 &&
			 tfSatBit(tfSatInner(mask), S, Targ))
			result = tfSatResult(mask, TFS_UnaryToCross);
	}

	else if (!tfSatEmbed(mask))
		/* result = tfSatFalse(mask) */;

	else if (tfIsMulti(S)) {
		/* Embed Multi(A, ..., B) in Cross(A, ..., B). */
		if (tfSatAllow(mask, TFS_MultiToCross) &&
		    tfMultiArgc(S) == argc &&
		    tfSatSucceed(tfSatEach(mask, S, T)))
			result = tfSatResult(mask, TFS_MultiToCross);
	}

	else {
		/* Embed S in Cross(S). */
		if (tfSatAllow(mask, TFS_UnaryToCross) &&
		    argc == 1 &&
		    tfSatBit(tfSatInner(mask), S, Targ))
			result = tfSatResult(mask, TFS_UnaryToCross);
	}

	return result;
}

local SatMask
tfSatMulti(SatMask mask, TForm S, TForm T)
{
	Length		argc = tfMultiArgc(T);
	TForm		Targ = tfMultiArgN(T, int0);
	SatMask		result = tfSatFalse(mask);

	if (tfIsMulti(S)) {
		if (tfMultiArgc(S) == argc &&
		    tfSatSucceed(tfSatEach(mask, S, T)))
			result = tfSatTrue(mask);

		/* Embed S in Multi(). */
		else if (tfSatAllow(mask, TFS_AnyToNone) && argc == 0)
			result = tfSatResult(mask, TFS_AnyToNone);
	}

	else if (!tfSatEmbed(mask))
		/* result = tfSatFalse(mask) */;

	else if (tfIsCross(S)) {
		/* Embed Cross(A, ..., B) in Multi(A, ..., B). */
		if (tfSatAllow(mask, TFS_CrossToMulti) &&
		    tfCrossArgc(S) == argc &&
		    tfSatSucceed(tfSatEach(mask, S, T)))
			result = tfSatResult(mask, TFS_CrossToMulti);

		/* Embed S in Multi(S). */
		else if (tfSatAllow(mask, TFS_UnaryToMulti) &&
			 argc == 1 &&
			 tfSatBit(tfSatInner(mask), S, Targ))
			result = tfSatResult(mask, TFS_UnaryToMulti);

		/* Embed S in Multi(). */
		else if (tfSatAllow(mask, TFS_AnyToNone) && argc == 0)
			result = tfSatResult(mask, TFS_AnyToNone);
	}

	else {
		/* Embed S in Multi(S). */
		if (tfSatAllow(mask, TFS_UnaryToMulti) &&
		    argc == 1 &&
		    tfSatBit(tfSatInner(mask), S, Targ))
			result = tfSatResult(mask, TFS_UnaryToMulti);

		/* Embed S in Multi(). */
		else if (tfSatAllow(mask, TFS_AnyToNone) && argc == 0)
			result = tfSatResult(mask, TFS_AnyToNone);
	}

	return result;
}


/******************************************************************************
 *
 * :: Exceptions
 *
 *****************************************************************************/

local SatMask
tfSatExcept(SatMask mask, TForm S, TForm T)
{
	TForm si, ti;
	TForm se, te;
	SatMask res = tfSatFalse(mask);

	assert(tfIsExcept(T));
	ti = tfExceptType(T);
	te = tfExceptExcept(T);

	if (tfIsExcept(S)) {
		si = tfExceptType(S);
		se = tfExceptExcept(S);
	}
	else {
		si = S;
		se = NULL;
	}
	/* !! this is a bit naugty, as then one can write:
	 * foo(n: Integer): Integer == {
	 *  	throw BBB
	 * }
	 *
	 * bar(n: Integer): Integer except ZZZ == {
	 * 	foo(n)
	 * }
	 *
	 * Consequently, perhaps we should infer
	 * identifiers as
	 * 	id except ()
	 * This would be horribly inefficient, so we don't do
	 * it yet.  Plus we'd need bigtime changes to the libraries
	 * to force 'except ()' where necessary.
	 */

	res = tfSat(mask, si, ti);
	if (!se)
		return res;
	if (tfSatSucceed(res)) {
		TForm sei, tej;
		int i, j, sc, tc;
		SatMask eres;

		sc  = tfAsMultiArgc(se);
		tc  = tfAsMultiArgc(te);
		eres = tfSatTrue(mask);
		for (i=0; i < sc && tfSatSucceed(eres); i++) {
			sei = tfAsMultiArgN(se, sc, i);
			for (j=0; j < tc; j++) {
				eres = tfSatFalse(mask);
				tej = tfAsMultiArgN(te, tc, j);
				if (tfSatSucceed(tfSat(mask, sei, tej))) {
					eres = tfSatTrue(mask);
					break;
				}
			}
		}
		res = tfSatSucceed(eres) ? res : tfSatFalse(mask);
	}
	return res;
}


/******************************************************************************
 *
 * :: Type form exports
 *
 *****************************************************************************/

/*
 * Succeed if the category exports of S satisfy the category exports of T.
 */
local SatMask
tfSatCatExports(SatMask mask, AbSyn Sab, TForm S, TForm T)
{
	TForm		Sp, Tp, p;
	SatMask		result = tfSatFalse(mask);

	Sp = tfCatExportsPending(S);
	Tp = tfCatExportsPending(T);
	p  = Sp ? Sp : Tp;

	if (DEBUG(tfsExport)) {
		if (p) {
			fprintf(dbOut, "Pending: \n");
			tfPrintDb(S);
			tfPrintDb(T);
			tfPrintDb(p);
		}
	}

	if (p == NULL) {
		SymeList	mods, Ssymes, Tsymes;

		assert(tfHasCatExports(S) && tfHasCatExports(T));

		Tsymes = listCopy(Syme)(tfGetCatSelfSelf(T));
		if (Tsymes == listNil(Syme))
			Tsymes = tfGetCatParents(T, true);
		if (Tsymes == listNil(Syme))
			Tsymes = tfGetCatExports(T);
		/* 
		 * PAB: Assume that if T has no exports, then
		 * S must be OK.
		 */
		if (Tsymes == listNil(Syme)) {
			if (DEBUG(tfsExport)) {
				fprintf(dbOut, "tfSatCatExports: 'T' has no exports\n");
				tfPrintDb(T);
			}
			return tfSatTrue(mask);
		}

		mods = listConcat(Syme)(tfGetCatSelf(S), tfGetCatSelf(T));

		Ssymes = listCopy(Syme)(tfGetCatSelfSelf(S));
		if (Ssymes == listNil(Syme))
			Ssymes = tfGetCatParents(S, true);
		if (Ssymes == listNil(Syme))
			Ssymes = tfGetCatExports(S);

		result = tfSatParents(mask, mods, Sab, Ssymes, Tsymes);
	}
	else if (tfSatAllow(mask, TFS_Pending)) {
		extern AbSyn symeLazyCheckData;
		assert(tfIsPending(p));
		result = tfSatResult(mask, TFS_Pending);
		tfSatSetPendingFail(p);
		if (tfSatCommit(mask))
			tcNewSat(p, abCondKnown, S, T, tfSatInfo(mask) ? symeLazyCheckData : NULL);
	}

	return result;
}

/*
 * Succeed if the 3d-order exports of S satisfy the 3d-order exports of T.
 */
local SatMask
tfSatThdExports(SatMask mask, TForm S, TForm T)
{
	TForm		Sp, Tp, p;
	SatMask		result = tfSatFalse(mask);

	Sp = tfThdExportsPending(S);
	Tp = tfThdExportsPending(T);
	p  = Sp ? Sp : Tp;

	if (p == NULL) {
		SymeList	mods;

		assert(tfHasThdExports(S) && tfHasThdExports(T));

		mods = listConcat(Syme)(tfGetThdSelf(S), tfGetThdSelf(T));
		result = tfSatExports(mask, mods,
				      tfGetThdExports(S),
				      tfGetThdExports(T));
	}
	else if (tfSatAllow(mask, TFS_Pending)) {
		extern AbSyn symeLazyCheckData;
		assert(tfIsPending(p));
		result = tfSatResult(mask, TFS_Pending);
		tfSatSetPendingFail(p);
		if (tfSatCommit(mask))
			tcNewSat(p, abCondKnown, S, T, tfSatInfo(mask) ? symeLazyCheckData : NULL);
	}

	return result;
}

/*
 * Succeed if each of the symes in T can be found in S.
 */
local SatMask
tfSatExports(SatMask mask, SymeList mods, SymeList S, SymeList T)
{
	SymeList	missing = tfSatExportsMissing(mask, mods, NULL, S, T);

	if (missing) {
		listFree(Syme)(missing);
		return tfSatResult(mask, TFS_ExportsMissing);
	}
	else
		return tfSatTrue(mask);
}

local SymeList
tfSatExportsMissing(SatMask mask, SymeList mods, AbSyn Sab, SymeList S, SymeList T)
{
	SymeList	symes, missing;
	AbSub		lazySelfSubst;

	if (DEBUG(tfsExport)) {
		fprintf(dbOut, "(->tfSatExportMissing: %*s= source list: ",
			tfsDepthNo, "");
		listPrint(Syme)(dbOut, S, symePrint);
		fnewline(dbOut);
	}

	missing	= listNil(Syme);
	lazySelfSubst = NULL;
	for (symes = T; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);

		tfsExportDEBUG(dbOut, "->tfSatExportMissing: %*s= looking for: %pSyme %pTForm\n",
			       tfsDepthNo, "", syme, symeType(syme));

		if (tfSatSucceed(tfSatExport(mask, mods, Sab, S, syme, &lazySelfSubst)))
			continue;

		missing = listCons(Syme)(syme, missing);
		if (tfSatMissing(mask))
			continue;

		tfsExportDEBUG(dbOut, "No: %s %pSyme)\n", syme->id->str, syme);

		return missing;
	}

	tfsExportDEBUG(dbOut, "%s)\n", missing ? "OK" : "Bad news");
	return missing;
}

typedef struct satModAbSyn {
	SymeList mods;
	AbSyn ab;
} *SatModAbSyn;

local SatModAbSyn
satModAbSynNew(SymeList mods, AbSyn ab)
{
	SatModAbSyn satModAbSyn;

	satModAbSyn = (SatModAbSyn) stoAlloc(OB_Other, sizeof(*satModAbSyn));
	satModAbSyn->mods = mods;
	satModAbSyn->ab = ab;
	return satModAbSyn;
}

local void
satModAbSynFree(SatModAbSyn satModAbSyn)
{
	stoFree(satModAbSyn);
}

local AbEqualValue
tfSatAbCompareModAbSyn(void *ctxt, AbSyn ab1, AbSyn ab2)
{
	SatModAbSyn satModAbSyn = (SatModAbSyn) ctxt;
	// For ids, make sure % in ab1, if present at all
	if (!abIsTheId(ab1, ssymSelf) && abIsTheId(ab2, ssymSelf)) {
		return tfSatAbCompareModAbSyn(ctxt, ab2, ab1);
	}
	if (abTag(ab1) != AB_Id && abTag(ab2) == AB_Id) {
		return tfSatAbCompareModAbSyn(ctxt, ab2, ab1);
	}

	if (abTag(ab1) != AB_Id) {
		return AbEqual_Struct;
	}
	else if (abIsTheId(ab1, ssymSelf)) {
		Bool eqAbSyn = abEqualModDeclares(satModAbSyn->ab, ab2);
		if (eqAbSyn)
			return AbEqual_True;
		else {
			// NB: This is a bit too lax, but we can wait for a counterexample
			if (abIsTheId(ab2, ssymSelf)) {
				return AbEqual_True;
			}
			Bool eq = sefoEqualMod(satModAbSyn->mods, ab1, ab2);
			return eq ? AbEqual_True : AbEqual_False;
		}
	}
	else {
		Bool eq = sefoEqualMod(satModAbSyn->mods, ab1, ab2);
		return eq ? AbEqual_True : AbEqual_False;
	}
}

/*
 * Succeed if t can be found in S.
 */
local SatMask
tfSatExport(SatMask mask, SymeList mods, AbSyn Sab, SymeList S, Syme t, AbSub *lazySelfSubst)
{
	SatMask		result = tfSatFalse(mask);
	TForm           substT;
	SymeList	symes;
	Bool tryHarder = true;
	static int serialNo = 0;
	int serialThis = serialNo++;

	/* Check for % explicitly
	* More exactly, as long as Sab is %, find % from t; if it corresponds to Sab or mods,
	* then we have the thing we want.
	* This fixes up cases like Rng: C == with Module(%); Module(X: Rng) == ...
	*/
	if (Sab && tfHasSelf(symeType(t))
	    && abIsTheId(Sab, ssymSelf)) {
		for (symes = tfSelf(symeType(t)); !tfSatSucceed(result) && symes; symes = cdr(symes)) {
			if (listMemq(Syme)(mods, car(symes))) {
				result = tfSatTrue(mask);
			}
		}
		if (tfSatSucceed(result)) {
			return result;
		}
	}

	tfsExportDEBUG(dbOut, "tfSatExport[%d]:: Start S: %pAbSyn\n", serialThis, Sab);
	tfsExportDEBUG(dbOut, "tfSatExport[%d]:: Target %pSyme %pTForm\n", serialThis, t, symeType(t));

	if (symeHasDefault(t) && !symeIsSelfSelf(t))
		return tfSatTrue(mask);

	/* First round.. try "normally" */
	int iterCount = 0;
	for (symes = S; !tfSatSucceed(result) && symes; symes = cdr(symes)) {
		SatMask satConditions;
		Syme	s = car(symes);
		int     iterThis = iterCount++;

		tfsExportDEBUG(dbOut, "tfSatExport[%d.%d]:: Test %pSyme %pTForm %pAbSynList\n",
			       serialThis, iterThis, s, symeType(s), symeCondition(s));
		if (!symeEqualModConditions(mods, s, t))
			continue;
		satConditions = tfSatConditions(mask, mods, s, t);
		if (tfSatSucceed(satConditions)) {
			result = tfSatTrue(mask);
			tryHarder = false;
		}
		else if (tfSatPending(satConditions)) {
			result = tfSatPending(mask);
			tryHarder = false;
		}
	}

	tfsExportDEBUG(dbOut, "tfSatExport[%d]:: Incoming S: %pAbSyn retry: %d\n", serialThis, Sab, tryHarder);

	if (!tryHarder)
		return result;

	if (Sab == NULL)
		return result;

	/* Second time, with feeling. */
	/* More precisely, we substitute anything in 'mods' with the original
	 * 'S' Sefo, if we have it.  The assumption is the mods should contain
	 * various local values for '%', and swapping them with the value used locally
	 * should let us match 'Foo %' with 'Foo X'.
	 */

	tfsExportDEBUG(dbOut, "(tfSatExportExtra[%d]:: Incoming S: %pAbSyn %pTForm\n",
		       serialThis, Sab, symeType(t));

	SatModAbSyn satModAbSyn = satModAbSynNew(mods, Sab);
	for (symes = S; !tfSatSucceed(result) && symes; symes = cdr(symes)) {
		Syme	s = car(symes);
		Bool    weakEq;

		if (symeId(s) != symeId(t)) {
			continue;
		}

		if (!abHasSymbol(tfExpr(symeType(s)), ssymSelf))
			continue;

		//substS = tfSubst(sigma, symeType(s));
		//weakEq = abEqualModDeclares(tfExpr(substS), tfExpr(substT));
		weakEq = abCompareModDeclares(tfSatAbCompareModAbSyn, satModAbSyn, tfExpr(symeType(s)), tfExpr(symeType(t)));

		if (weakEq) {
			if (symeCondition(s) != listNil(Sefo)) {
				result = tfSatConditions(mask, mods, s, t);
			}
			else {
				result = tfSatTrue(mask);
			}
		}
	}
	satModAbSynFree(satModAbSyn);

	tfsExportDEBUG(dbOut, " tfSatExportExtra[%d]:: --> %d)\n",
		       serialThis, tfSatSucceed(result));

	return result;
}

AbSub
tfSatExportLazySelfSubst(SymeList mods, Sefo Sab, AbSub *lazySelfSubst)
{
	AbSub sigma = *lazySelfSubst;
	if (sigma == NULL) {
		sigma = absFrSymes(stabFile(), mods, Sab);
		*lazySelfSubst = sigma;
	}
	return sigma;
}


extern TForm		tiGetTForm		(Stab, AbSyn);

static SatMask tfSatConditionOnSelf(SatMask mask, SymeList mods, Syme s, Sefo property);

local SatMask
tfSatConditions(SatMask mask, SymeList mods, Syme s, Syme t)
{
	SefoList	Sconds = symeCondition(s);
	SefoList	Tconds = symeCondition(t);
	SatMask		result = tfSatTrue(mask);
	static int count = 0;
	int serial = count++;

	for (; Sconds; Sconds = cdr(Sconds)) {
		Sefo	cond = car(Sconds);
		if (sefoListMemberMod(mods, cond, Tconds))
			continue;
		if (sefoListMemberMod(mods, cond, tfSatConds()))
			continue;

		/*
		 * This is to remove any trivially satisfied conditions
		 * remaining on `s'.
		 * Should consider squelching the condition out of the
		 * export list.
		 */
		if (abTag(cond) ==  AB_Has) {
			TForm tfdom, tfcat;
			AbSyn dom, cat;

			if (abIsTheId(cond->abHas.expr, ssymSelf)) {
				if (tfSatSucceed(tfSatConditionOnSelf(mask, mods, s, cond->abHas.property)))
					continue;
				else
					return tfSatFalse(mask);
			}
			tfsExportDEBUG(dbOut, "(%d Check condition %pSyme %pTForm %pAbSyn\n", serial, s, symeType(s), cond);
			dom   = cond->abHas.expr;
			tfdom = abGetCategory(dom);
			if (tfTestSeen(tfdom, cond->abHas.property)) {
				return tfSatFalse(mask);
			}
			if (tfSatUseConditions(mask) && abCondKnown != NULL) {
				TForm tfdomNew = ablogImpliedType(abCondKnown, dom, tfdom);
				if (tfdomNew != NULL) {
					tfsExportDEBUG(dbOut, "Domain switch: %pTForm --> %pTForm\n", tfdom, tfdomNew);
					tfdom = tfdomNew;
				}
			}
			cat   = cond->abHas.property;
			tfcat = abTForm(cat) ? abTForm(cat) : tiTopFns()->tiGetTopLevelTForm(ablogTrue(), cat);
			tfTestPush(tfdom, cond->abHas.property);
			result = tfSat1(mask, dom, tfdom, tfcat);
			tfTestPop(tfdom, cond->abHas.property);

			tfsExportDEBUG(dbOut, " %d Check condition %pSyme %oBool)\n", serial, s, tfSatSucceed(result));
			if (tfSatSucceed(result))
				continue;
			else if (tfSatPending(result)) {
				result = tfSatResult(mask, TFS_Pending);
				continue;
			}
		}
		return tfSatFalse(mask);
	}
	return result;
}

SatMask
tfSatConditionOnSelf(SatMask mask, SymeList mods, Syme s, Sefo property)
{
	tfsExportDEBUG(dbOut, "tfsExport: Check self condition %pSyme %pTForm %pAbSyn\n", s, symeType(s), property);
	// Might as well say true as this is an export list.. need to retain
	// in case it becomes true on import
	return tfSatTrue(mask);
}


local Bool
sefoListMemberMod(SymeList mods, Sefo sefo, SefoList sefos)
{
	for (; sefos; sefos = cdr(sefos))
		if (sefoEqualMod(mods, sefo, car(sefos)))
			return true;
	return false;
}

static TFormList	TfSatCondTypes = listNil(TForm);
static SefoList		TfSatCondExprs = listNil(Sefo);

local void
tfSatPushMapConds(TForm tfm)
{
	Length	i, argc = tfMapArgc(tfm);

	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfMapArgN(tfm, i);
		if (!tfIsDeclare(tfi)) continue;

		listPush(TForm, tfi, TfSatCondTypes);
		if (TfSatCondExprs)
			listPush(Sefo, tfSatCond(tfi), TfSatCondExprs);
	}
}

local void
tfSatPopMapConds(TForm tfm)
{
	Length	i, argc = tfMapArgc(tfm);

	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfMapArgN(tfm, i);
		if (!tfIsDeclare(tfi)) continue;

		TfSatCondTypes = listFreeCons(TForm)(TfSatCondTypes);
		if (TfSatCondExprs)
			TfSatCondExprs = listFreeCons(Sefo)(TfSatCondExprs);
	}
}

local SefoList
tfSatConds(void)
{
	TFormList	types;
	SefoList	exprs = listNil(Sefo);

	if (!TfSatCondExprs) {
		for (types = TfSatCondTypes; types; types = cdr(types))
			exprs = listCons(Sefo)(tfSatCond(car(types)), exprs);
		TfSatCondExprs = listNReverse(Sefo)(exprs);
	}

	return TfSatCondExprs;
}

local Sefo
tfSatCond(TForm tf)
{
	assert(tfIsDeclare(tf));
	return abHas(tfDefineeSyme(tf), tfDefineeType(tf));
}

/*
 * Succeed if each of the symes in T can be found in the parent tree
 * for the symes in S.
 */
local SatMask
tfSatParents(SatMask mask, SymeList mods, AbSyn Sab, SymeList S, SymeList T)
{
	SymeList	newS = S, oldS = listNil(Syme);
	SymeList	queue = listNil(Syme);
	SymeTSet        oldTbl = tsetCreateCustom(Syme)(symeHashFn, symeEqual);
	int		serialThis;
	int		iterThis = 0;

	tfsSerialNo += 1;
	serialThis = tfsSerialNo;

	/* Collect all of the missing exports. */
	mask |= TFS_Missing;

	tfsParentDEBUG(dbOut, "(->tfpSyme: %*s%d = source list: %pSymeList\n",
		       tfsDepthNo, "", serialThis, S);

	while (newS || queue) {
		iterThis++;
		SymeList currentS = newS;
		T = tfSatExportsMissing(mask, mods, Sab, currentS, T);
		if (T == listNil(Syme)) {
			tfsParentDEBUG(dbOut, " ->tfpSyme: %*s%d = No parents)\n", tfsDepthNo, "", serialThis);
			return tfSatTrue(mask);
		}
		newS = tfSatParentsFilterTable(oldTbl, currentS);
		queue = listNConcat(Syme)(queue, listCopy(Syme)(newS));
		tsetAddAll(Syme)(oldTbl, newS);

		if (queue) {
			Syme	oldSyme = car(queue);

			tfsParentDEBUG(dbOut, " ->tfpSyme: %*s%d.%d= expanding: %pSyme %pTForm %pAbSynList\n",
				       tfsDepthNo, "", serialThis, iterThis, oldSyme,
				       symeType(oldSyme), symeCondition(oldSyme));

			newS = tfGetCatParents(symeType(oldSyme), true);
			/*
			if (symeCondition(oldSyme) != listNil(Sefo)) {
				newS = symeListAddCondition(newS, abNewOfList(AB_And, sposNone,
									      (AbSynList) symeCondition(oldSyme)), true);
			}
			*/
			queue = cdr(queue);

			tfsParentDEBUG(dbOut, " ->tfpSyme: %*s%d.%d= into: %pSymeList\n",
				       tfsDepthNo, "", serialThis, iterThis, newS);
		}
		else
			newS = listNil(Syme);
	}
	tfsParentDEBUG(dbOut, " ->tfpSyme: %*s%d= Left: %pSymeList)\n",
		       tfsDepthNo, "", serialThis, T);
	if (T == listNil(Syme))
		return tfSatTrue(mask);
	tsetFree(Syme)(oldTbl);
	while (T && tfsParentDebug) {
		tfsParentDEBUG(dbOut, "%d Missing %pAbSyn %pSyme: %pTForm %pAbSynList\n", serialThis, Sab, car(T),
			       symeType(car(T)),
			       symeCondition(car(T)));
		T = cdr(T);
	}

	return tfSatResult(mask, TFS_ExportsMissing);
}

local SymeList
tfSatParentsFilterTable(SymeTSet tbl, SymeList nsymes)
{
	SymeList symes, rsymes = listNil(Syme);
	/* Collect symes for %% which have not been seen before. */
	for (symes = nsymes; symes; symes = cdr(symes))
		if (symeIsSelfSelf(car(symes)) &&
		    !tsetMember(Syme)(tbl, car(symes)))
			rsymes = listCons(Syme)(car(symes), rsymes);

	listFree(Syme)(nsymes);
	return listNReverse(Syme)(rsymes);

}

String
tfSatMaskToString(SatMask mask)
{
	String sep="";
	if (mask == TFS_Succeed) {
		return "Success";
	}
	else {
		Buffer b = bufNew();
		OStream os = ostreamNewFrBuffer(b);
		int i = 0;

		while (tfSatMaskInfo[i].name != 0) {
			if (mask & (1<<i)) {
				ostreamWrite(os, sep, -1);
				ostreamWrite(os, tfSatMaskInfo[i].name, -1);
				sep = "|";
			}
			i++;
		}
		ostreamFree(os);
		return bufLiberate(b);
	}
}

/******************************************************************************
 *
 * :: Universally quantified types
 *
 *****************************************************************************/

CREATE_LIST(USatMask);

static int utfSatMaskInstances;

local USatMask utfSatArgPoss(SatMask mask, AbSyn Sab, UTForm T);
local USatMaskList utfSatArgAsList(SatMask mask, AbSyn ab, UTForm T);
local USatMaskList utfSatArgPossAsList(SatMask, AbSyn ab, UTForm T);
local USatMask utfSatTYPE(SatMask mask, UTForm S);
local USatMask utfSatCAT(SatMask mask, UTForm S);
local USatMask utfSatTuple(SatMask mask, UTForm S, UTForm T);
local USatMask utfSatMulti(SatMask mask, UTForm S, UTForm T);
local USatMask utfSatCross(SatMask mask, UTForm S, UTForm T);
local USatMask utfSatMultiLHS(SatMask mask, UTForm S, UTForm T);
local USatMask utfSatCrossLHS(SatMask mask, UTForm S, UTForm T);

local USatMask utfSatEvery(SatMask mask, UTForm S, UTForm T);
local USatMask utfSatEach(SatMask mask, UTForm S, UTForm T);

local USatMask utfSatResultEmbed(USatMask mask, SatMask moreMask);
local USatMask utfSatResult(SatMask mask, SatMask moreMask, UTypeResult result);
local USatMask utfSatTrue(SatMask mask);
local USatMask utfSatFalse(SatMask mask);
local USatMask utfSatFail(SatMask mask, SatMask fail);
local USatMask utfSatSimple(SatMask mask, SatMask result);
local USatMask  utfSatEmbed(USatMask result);
local UTypeResult utfSatMaskLiberate(USatMask umask);
local USatMask utfSatUsePending(SatMask mask, UTForm S, UTForm T);
local USatMask utfSatMaskCombine(SatMask mask, USatMask mask1, USatMask mask2);

local Bool utfSatMapCheckConstant(UTForm S, UTForm T,
				  AbSyn ab, Length argc, AbSynGetter argf);

local Bool utfSatMapExtendSigma(UTForm TScope, UTForm tfi, AbSyn abi, int pi, AbSub sigma);

/*
 * Succeed if S satisfies T.
 */
Bool
utfSatBit(SatMask mask, UTForm S, UTForm T)
{
	USatMask result = utfSat(mask, S, T);
	SatMask resultMask = result->mask;
	utfSatMaskFree(result);
	return tfSatSucceed(resultMask);
}

Bool
utfSatisfies(UTForm utfS, UTForm utfT)
{
	SatMask	mask = TFS_Commit | TFS_UsualMask;
	return utfSatBit(mask, utfS, utfT);
}

local USatMask
utfSatResult(SatMask mask, SatMask moreMask, UTypeResult result)
{
	USatMask umask = (USatMask) stoAlloc(OB_Other, sizeof(*umask));
	umask->mask = tfSatResult(mask, moreMask);
	umask->result = result;

	utfSatMaskInstances++;
	if (tfSatSucceed(umask->mask) && result == NULL)
		bug("oops");
	return umask;
}

local USatMask
utfSatResultEmbed(USatMask mask, SatMask moreMask)
{
	if (tfSatSucceed(utfSatMaskMask(mask))) {
		mask->mask = tfSatResult(mask->mask, moreMask);
	}
	return mask;
}

local void
utfSatResultFree(USatMask mask)
{
	utfSatMaskInstances--;
	utypeResultFree(mask->result);
	stoFree(mask);
}

SatMask
utfSatMaskMask(USatMask mask)
{
	return mask->mask;
}

UTypeResult
utfSatMaskResult(USatMask mask)
{
	return mask->result;
}


local USatMask
utfSatTrue(SatMask mask)
{
	return utfSatSimple(mask, TFS_Succeed);
}

local USatMask
utfSatFalse(SatMask mask)
{
	return utfSatSimple(mask, TFS_Fail);
}

local USatMask
utfSatFail(SatMask mask, SatMask fail)
{
	return utfSatSimple(mask, fail);
}

local USatMask
utfSatSimple(SatMask mask, SatMask result)
{
	return utfSatResult(mask, result,
			    tfSatSucceed(result) ? utypeResultEmpty(): utypeResultFailed());
}

local USatMask
utfSatEmbed(USatMask result)
{
	SatMask mask = tfSatEmbed(utfSatMaskMask(result));
	return utfSatResult(mask, 0,
			    utfSatMaskLiberate(result));
}


Bool
utfSatSucceed(USatMask umask)
{
	return tfSatSucceed(umask->mask);
}

Bool
utfSatPending(USatMask umask)
{
	return tfSatPending(umask->mask);
}

local USatMask
utfSatUsePending(SatMask mask, UTForm S, UTForm T)
{
	return utfSatSimple(mask, tfSatUsePending(mask, utformTForm(S), utformTForm(T)));
}

local Bool
utfSatCommit(USatMask mask)
{
	return tfSatCommit(utfSatMaskMask(mask));
}

local USatMask
utfSatMaskCopy(USatMask mask)
{
	return utfSatResult(TFS_Succeed, utfSatMaskMask(mask),
			    utypeResultCopy(utfSatMaskResult(mask)));
}

local UTypeResult
utfSatMaskLiberate(USatMask umask)
{
	UTypeResult result = umask->result;
	umask->result = utypeResultFailed();
	utfSatMaskFree(umask);
	return result;
}


void
utfSatMaskFree(USatMask umask)
{
	utypeResultFree(umask->result);
	stoFree(umask);
	utfSatMaskInstances--;
	if (utfSatMaskInstances < 0)
		bug("Double free");
}

local USatMask
utfSatMaskCombine(SatMask mask, USatMask mask1, USatMask mask2)
{
	if (utfSatSucceed(mask1) && utfSatSucceed(mask2)) {
		SatMask resultMask = mask1->mask | mask2->mask;
		UTypeResult result1 = utfSatMaskLiberate(mask1);
		UTypeResult result2 = utfSatMaskLiberate(mask2);
		UTypeResult newResult = utypeResultMerge(result1, result2);

		if (newResult == NULL) {
			return utfSatSimple(mask, TFS_UnifyFail);
		}
		else {
			return utfSatResult(mask, resultMask, newResult);
		}
	}
	else if (utfSatSucceed(mask1)) {
		utfSatMaskFree(mask1);
		return mask2;
	}
	else {
		utfSatMaskFree(mask2);
		return mask1;
	}
}

local USatMaskList
utfSatMaskCross(USatMaskList masks1, USatMaskList masks2)
{
	USatMaskList newResults = listNil(USatMask);;

	while (masks1 != listNil(USatMask)) {
		USatMaskList masksi = masks2;
		USatMask mask = car(masks1);
		masks1 = cdr(masks1);
		if (!utfSatSucceed(mask)) {
			continue;
		}
		while (masksi != listNil(USatMask)) {
			USatMask maski = car(masksi);
			masksi = cdr(masksi);
			if (!utfSatSucceed(maski)) {
				continue;
			}
			USatMask result = utfSatMaskCombine(TFS_Succeed, utfSatMaskCopy(mask), utfSatMaskCopy(maski));
			if (utfSatSucceed(result)) {
				newResults = listCons(USatMask)(result,
								newResults);
			}
		}
	}
	return newResults;
}

local USatMask
utfSatParNFail(SatMask mask, SatMask fail, int n)
{
	return utfSatResult(mask, fail | tfsParNBits(n), utypeResultFailed());
}

USatMask
utfSatMap0(SatMask mask, UTForm S, UTForm T)
{
	USatMask result = utfSatFalse(mask);
	SatMask  mask0 = tfSatInner(mask);
	TForm    Stf = utformTForm(S);
	TForm    Ttf = utformTForm(T);

	if (!(tfTag(Ttf) == tfTag(Stf) && tfMapArgc(Ttf) == tfMapArgc(Stf)))
		/* result = tfSatFalse(mask) */;

	else if (tfIsDependentMap(Stf) && tfIsPending(Stf)
		 && tfSatAllow(mask, TFS_Pending)) {
		result = utfSatUsePending(mask, S, T);
	}
	else if (tfIsDependentMap(Stf)) {
		bug("Not implemented");
	}
	else {
		tfSatPushMapConds(Ttf);
		result = utfSat(mask0, utfMapArg(T), utfMapArg(S));
		if (utfSatSucceed(result)) {
			result = utfSatMaskCombine(mask, utfSatEmbed(result),
						   utfSat(mask0, utfMapRet(S), utfMapRet(T)));
		}
		tfSatPopMapConds(Ttf);
	}

	return result;
}

USatMask
utfSatMapArgs(SatMask mask, AbSub sigma, UTForm S,
	      AbSyn ab, Length argc, AbSynGetter argf)
{
	USatMask result;
	static int count = 0;
	int serialThis = count++;

	if (DEBUG(tfs)) {
		afprintf(dbOut, "(utfSatMapArgs: %d %pSefo %pUTForm\n", serialThis, ab, S);
	}
	result = utfSatAsMulti(mask, sigma, utfMapArg(S), S, ab, argc, argf);
	if (DEBUG(tfs)) {
		afprintf(dbOut, " utfSatMapArgs: %d %s)\n", serialThis,
			 tfSatMaskToString(utfSatMaskMask(result)));
	}
	return result;
}

USatMaskList
utfSatMapArgsList(SatMask mask, AbSub sigma, UTForm S,
	      AbSyn ab, Length argc, AbSynGetter argf)
{
	USatMaskList result;
	utformFollow(S);
	result = utfSatAsMultiList(mask, sigma, utfMapArg(S), S, ab, argc, argf);

	return result;
}

local Bool
utfSatMapCommit(SatMask mask, UTForm TScope, Stab absStabSigma, UTForm tfi, AbSyn abi);

USatMask
utfSatAsMulti(SatMask mask, AbSub sigma, UTForm S, UTForm TScope,
	      AbSyn ab, Length argc, AbSynGetter argf)
{
	static int count;
	USatMask result = utfSatTrue(mask);
	AbSyn    abc = NULL;
	AbEmbed  embed;
	Length   usedc = 0, parmc;
	Bool	 packed = utfIsPackedMap(TScope);
	int      i;
	int      serialThis = count++;

	utformFollow(S);

	embed = utfAsMultiEmbed(S, argc);
	if (embed == AB_Embed_Fail)
		return utfSatSimple(mask, TFS_EmbedFail);

	tfsDepthNo++;

	if (DEBUG(tfsMulti) || DEBUG(tfs)) {
		afprintf(dbOut, "(utfSatAsMulti: %*s%d S: %pUTForm\n", tfsDepthNo, "",
			 serialThis, S);
		afprintf(dbOut, " utfSatAsMulti: %*s%d Tab: %pSefo\n", tfsDepthNo, "",
			 serialThis, ab);
		afprintf(dbOut, " utfSatAsMulti: %*s%d TScope: %pUTForm\n", tfsDepthNo, "",
			 serialThis, TScope);
	}

	parmc = utfMultiHasDefaults(S) ? utfAsMultiArgc(S) : argc;
	if (parmc != 1 && utfIsTuple(utfDefineeType(S))) {
		abc = abNewEmpty(AB_Comma, parmc);
	}

	for (i=0; i<parmc; i++) {
		AbSyn	abi;
		UTForm	tfi;
		USatMask maski = utfSatTrue(mask);
		Length ai, pi;
		Bool def;

		pi   = (utformTForm(S)->rho ? utformTForm(S)->rho[i] : i);
		tfi = utfAsMultiArgN(S, parmc, pi);
		abi = utfAsMultiSelectArg(ab, argc, i, argf, tfi, &def, &ai);

		if (DEBUG(tfsMulti) || DEBUG(tfs)) {
			afprintf(dbOut, " utfSatAsMulti: %*s%d %d %d: %oBool %pSefo\n", tfsDepthNo, "",
				 serialThis, i, pi, def, abi);
			afprintf(dbOut, " utfSatAsMulti: %*s%d %d %d: %pUTForm\n", tfsDepthNo, "",
				 serialThis, i, pi, tfi);
			afprintf(dbOut, " utfSatAsMulti: %*s%d %d %d: %pUTypeResult\n", tfsDepthNo, "",
				 serialThis, i, pi, result->result);
		}
		if (!abi) {
			utfSatMaskFree(result);
			result = utfSatParNFail(mask, TFS_ArgMissing, i);
			break;
		}

		if (!def) usedc++;

		tfi = utfDefineeType(tfi);
		tfi = utformSubst(sigma, tfi);

		if (!def && !tfSatSigma(mask)) {
			maski = utfSatArg(mask, abi, tfi);
			if (!utfSatSucceed(maski)) {
				utfSatMaskFree(result);
				result = utfSatParNFail(mask, TFS_BadArgType, i);
				break;
			}
			if (utfSatPending(maski)) {
				utfSatMaskFree(result);
				result = utfSatSimple(mask, TFS_Pending);
			}
			else {
				result = utfSatMaskCombine(mask, result, maski);
			}
		}

		if (tfSatCommit(mask) && packed) {
			if (!tiTopFns()->tiUnaryToRaw(absStab(sigma), abi, utformConstOrFail(tfi))) {
				result = utfSatParNFail(mask, TFS_BadArgType, pi);
				break;
			}
		}
		if (tfSatCommit(mask)) {
			if (!utfSatMapCommit(mask, TScope, absStab(sigma), tfi, abi)) {
				result = utfSatParNFail(mask, TFS_BadArgType, pi);
				break;
			}
		}
		if (!utfSatMapExtendSigma(TScope, tfi, abi, pi, sigma)) {
			result = utfSatParNFail(mask, TFS_BadArgType, pi);
		}

		if (abc) {
			abArgv(abc)[i] = sefoCopy(abi);
		}
	}

	if (utfSatSucceed(result) && tfSatCommit(mask) && packed)
		if (!tiTopFns()->tiRawToUnary(absStab(sigma), ab, tfMapRet(utformConstOrFail(TScope))))
			result = utfSatSimple(mask, TFS_EmbedFail);

	if (utfSatSucceed(result) && usedc < argc)
		result = utfSatSimple(mask, TFS_DifferentArity);

	if (DEBUG(tfsMulti) || DEBUG(tfs)) {
		afprintf(dbOut, " utfSatAsMulti: %*s%d= %pUTypeResult\n", tfsDepthNo, "",
			 serialThis, utfSatMaskResult(result));
		afprintf(dbOut, " utfSatAsMulti: %*s%d= %s!)\n", tfsDepthNo, "",
			 serialThis, tfSatMaskToString(utfSatMaskMask(result)));
	}
	tfsDepthNo -= 1;

	return result;
}

USatMaskList
utfSatAsMultiList(SatMask mask, AbSub sigma, UTForm S, UTForm TScope,
	      AbSyn ab, Length argc, AbSynGetter argf)
{
	USatMaskList results;
	Length parmc, i, ai;
	AbEmbed embed;
	Bool def;
	int usedc;

	S = utformFollow(S);

	embed = utfAsMultiEmbed(S, argc);
	if (embed == AB_Embed_Fail)
		return listSingleton(USatMask)(utfSatFail(mask, TFS_EmbedFail));

	tfsDepthNo++;

	assert(!tfSatSigma(mask));

	results = listSingleton(USatMask)(utfSatTrue(mask));

	parmc = utfMultiHasDefaults(S) ? utfAsMultiArgc(S) : argc;

	usedc = 0;
	for (i=0; i<parmc; i++) {
		int    pi  = (utformTForm(S)->rho ? utformTForm(S)->rho[i] : i);
		UTForm tfi = utfAsMultiArgN(S, parmc, pi);
		AbSyn  abi = utfAsMultiSelectArg(ab, argc, i, argf, tfi, &def, &ai);

		if (!abi) {
			listFreeDeeply(USatMask)(results, utfSatMaskFree);
			results = listSingleton(USatMask)(utfSatParNFail(mask, TFS_ArgMissing, i));
			break;
		}
		if (!def) usedc++;

		tfi = utfDefineeType(tfi);
		tfi = utformSubst(sigma, tfi);

		if (!def) {
			USatMaskList masksi;
			masksi = utfSatArgAsList(mask, abi, tfi);
			if (masksi == listNil(USatMask)) {
				listFreeDeeply(USatMask)(results, utfSatMaskFree);
				results = listSingleton(USatMask)(utfSatParNFail(mask, TFS_BadArgType, i));
				break;
			}
			else if (utfSatPending(car(masksi))) {
				listFreeDeeply(USatMask)(results, utfSatMaskFree);
				results = listSingleton(USatMask)(utfSatSimple(mask, TFS_Pending));
			}
			else {
				results = utfSatMaskCross(results, masksi);
			}
		}
		if (!utfSatMapExtendSigma(TScope, tfi, abi, pi, sigma)) {
			results = listSingleton(USatMask)(utfSatParNFail(mask, TFS_BadArgType, pi));
			break;
		}
	}

	tfsDepthNo--;

	return results;
}

local Bool
utfSatMapCommit(SatMask mask, UTForm TScope, Stab absStabSigma, UTForm tfi, AbSyn abi)
{
	Bool success = true;
	Bool packed = utfIsPackedMap(TScope);

	// Infer argument types - we should be ok, but just in case
	tiTopFns()->tiBottomUp(absStabSigma, abi, tfUnknown);
	tiTopFns()->tiTopDown(absStabSigma, abi, utformConstOrFail(tfi));
	if (abUse(abi) != AB_Use_Type) {
		// Make sure embedding is ok
		USatMask checkMask = utfSatArg(mask, abi, tfi);
		abAddTContext(abi, tfSatAbEmbed(tfSatEmbed(checkMask->mask)));
	}

	if (packed) {
		if (!tiTopFns()->tiUnaryToRaw(absStabSigma, abi, utformConstOrFail(tfi))) {
			success = false;
		}
	}

	return success;
}


local Bool
utfSatMapExtendSigma(UTForm TScope, UTForm tfi, AbSyn abi, int pi, AbSub sigma)
{
	Syme syme = utfDefineeSyme(tfi);
	Bool success = true;

	if (syme && (tfSymeInducesDependency(syme, utformTForm(TScope))
		     || listMemq(Syme)(tfSymes(utformTForm(TScope)), syme)
		     || listMember(Syme)(tfSymes(utformTForm(TScope)), syme, symeEqual))) {
		abi = sefoCopy(abi);
		tiTopFns()->tiBottomUp(absStab(sigma), abi, tfUnknown);
		tiTopFns()->tiTopDown (absStab(sigma), abi, utformConstOrFail(tfi));

		if (abState(abi) == AB_State_HasUnique) {
			if (absFVars(sigma)) absSetFVars(sigma, NULL);
			sigma = absExtend(syme, abi, sigma);
		}
		else {
			success = false;
		}
	}
	return success;
}


USatMask
utfSatArg(SatMask mask, AbSyn ab, UTForm T)
{
	USatMask result;

	mask &= ~TFS_AnyToNone;

	/* We'd rather not use pending embeddings to decide argc questions. */
	if (utfIsMulti(T) && utfIsPending(T))
		mask &= ~TFS_Pending;

	switch (abState(ab)) {
	case AB_State_HasPoss:
		result = utfSatArgPoss(mask, ab, T);
		break;

	case AB_State_HasUnique:
		result = utfSat1(mask, ab, utformNewConstant(abTUnique(ab)), T);
		break;

	default:
		result = utfSatFalse(mask);
		break;
	}

	return result;
}


local USatMask
utfSatTYPE(SatMask mask, UTForm utf)
{
	assert(!utfIsAny(utf));
	return utfSatSimple(mask, tfSatTYPE(mask, utformTForm(utf)));;
}

local USatMask
utfSatCAT(SatMask mask, UTForm utf)
{
	assert(!utfIsAny(utf));
	return utfSatSimple(mask, tfSatCAT(mask, utformTForm(utf)));
}

local USatMask
utfSatMultiLHS(SatMask mask, UTForm S, UTForm T)
{
	USatMask result = utfSatFalse(mask);

	/* Embed Multi(S) in S. */
	if (tfSatAllow(mask, TFS_MultiToUnary) &&
	    utfMultiArgc(S) == 1) {
		UTForm Sarg = utfMultiArgN(S, int0);
		result = utfSat(tfSatInner(mask), Sarg, T);
		result = utfSatResultEmbed(result, TFS_MultiToUnary);
	}
	else if (utfIsConstant(T)) {
		/* Delta-equality of T with cross/multi */
		TForm Tnorm = tfDefineeBaseType(utformTForm(T));
		if (tfIsCross(Tnorm))
			result = utfSatCross(mask, S, utformNewConstant(Tnorm));
		else if (tfIsMulti(Tnorm))
			result = utfSatMulti(mask, S, utformNewConstant(Tnorm));
	}
	return result;
}

local USatMask
utfSatCrossLHS(SatMask mask, UTForm S, UTForm T)
{
	USatMask result = utfSatFalse(mask);
	/* Embed Cross(S) in S. */
	if (tfSatAllow(mask, TFS_CrossToUnary) &&
	    utfCrossArgc(S) == 1) {
		UTForm Sarg = utfCrossArgN(S, int0);
		result = utfSat(tfSatInner(mask), Sarg, T);
		result = utfSatResultEmbed(result, TFS_CrossToUnary);
	}
	else if (utfIsAny(T)) {
		UTypeResult unify = utformUnify(S, T);
		result = utfSatResult(mask, TFS_Unify, unify);
	}
	else if (utfIsConstant(T)) {
		/* Delta-equality of T with cross/multi */
		TForm Tnorm = tfDefineeBaseType(utformTForm(T));
		UTForm UTnorm = utformNewConstant(Tnorm);
		if (tfIsCross(Tnorm))
			result = utfSatCross(mask, S, utformNewConstant(Tnorm));
		else if (tfIsMulti(Tnorm))
			result = utfSatMulti(mask, S, utformNewConstant(Tnorm));
	}

	return result;
}

local USatMaskList
utfSatArgAsList(SatMask mask, AbSyn ab, UTForm T)
{
	USatMaskList results;
	mask &= ~TFS_AnyToNone;

	if (utfIsMulti(T) && utfIsPending(T))
		mask &= ~TFS_Pending;

	switch (abState(ab)) {
	case AB_State_HasUnique:
		results = listSingleton(USatMask)(utfSat1(mask, ab, utformNewConstant(abTUnique(ab)), T));
		break;
	case AB_State_HasPoss:
		results = utfSatArgPossAsList(mask, ab, T);
		break;
	default:
		results = listNil(USatMask);
	}
	return results;
}

local USatMask
utfSatArgPoss(SatMask mask, AbSyn Sab, UTForm T)
{
	TPossIterator it;
	USatMask result;
	TPoss S = abTPoss(Sab);
	int i=0, count;

	if (DEBUG(tfs)) {
		count = tpossCount(S);
		afprintf(dbOut, "utfSatArgPoss: 0/%d %pSefo %pUTForm\n", count, Sab, T);
	}
	for (tpossITER(it, S); tpossMORE(it); tpossSTEP(it)) {
		UTForm	utf = tpossUELT(it);
		result = utfSat1(mask, Sab, utf, T);
		if (utfSatSucceed(result)) {
			if (DEBUG(tfs)) {
				afprintf(dbOut, "utfSatArgPoss: %d/%d %pAbSyn %pUTForm --> %d Succeed\n",
					 i, count, Sab, T, utfSatSucceed(result));
			}
			return result;
		}
		i++;
	}
	if (DEBUG(tfs)) {
		afprintf(dbOut, "utfSatArgPoss: %d/%d FAILED\n", i, count);
	}
	return utfSatFalse(mask);
}


USatMaskList
utfSatArgPossAsList(SatMask mask, AbSyn Sab, UTForm T)
{
	USatMaskList results = listNil(USatMask);
	TPossIterator it;
	TPoss S = abTPoss(Sab);
	Bool simpleFound = false;;
	int i=0, count;

	for (tpossITER(it, S); tpossMORE(it); tpossSTEP(it)) {
		UTForm	utf = tpossUELT(it);
		USatMask result = utfSat1(mask, Sab, utf, T);
		if (utfSatPending(result)) {
			listFreeDeeply(USatMask)(results, utfSatResultFree);
			results = listSingleton(USatMask)(utfSatSimple(mask, TFS_Pending));
			break;
		}
		else if (utfSatSucceed(result) && utypeResultIsEmpty(result->result)) {
			if (!simpleFound) {
				results = listCons(USatMask)(result, results);
				simpleFound = true;
			}
		}
		else if (utfSatSucceed(result)) {
			results = listCons(USatMask)(result, results);
		}
		i++;
	}
	if (results == listNil(USatMask)) {
		results = listSingleton(USatMask)(utfSatFail(mask, TFS_BadArgType));
	}
	return results;
}


USatMask
utfSat1(SatMask mask, AbSyn Sab, UTForm S, UTForm T)
{
	UTypeResult unify;
	UTForm uS;
	UTForm uT;
	SatMask result;
	USatMask final;
	int serialThis;

	S = utfDefineeType(S);
	T = utfDefineeType(T);

	tfsSerialNo += 1;
	tfsDepthNo  += 1;
	serialThis   = tfsSerialNo;

	if (DEBUG(tfs)) {
		afprintf(dbOut, "%*s(UtfSat: %d %d: %pUTForm sat %pUTForm\n", tfsDepthNo, "", serialThis, tfsDepthNo, S, T);
	}
	if (S == T)
		final = utfSatTrue(mask);
	else if (utfIsConstant(S) && utfIsConstant(T)) {
		result = tfSat1(mask, Sab, utformTForm(S), utformTForm(T));
		final = utfSatSimple(mask, result);
	}
	else if (utfIsConstant(S) && tfIsUnknown(utformTForm(S))) {
		final = utfSatFalse(mask);
	}
	else if (utfIsConstant(T) && tfIsUnknown(utformTForm(T))) {
		final = utfSatTrue(mask);
	}
	else if (tfIsType(utformTForm(T))) {
		final = utfSatTYPE(mask, S);
	}
	else if (tfIsCategory(utformTForm(T)) || tfIsCategorySyntax(utformTForm(T)))
		final = utfSatCAT(mask, S);
	else if (utfIsExit(S))
		final = utfSatTrue(mask);
	else if (tfIsSyntax(utformTForm(S)) || tfIsSyntax(utformTForm(T))) {
		if (tfSatAllow(mask, TFS_Pending))
			final = utfSatUsePending(mask, S, T);
	}
	else if (utfIsRaw(S))
		final = utfSat(mask, utfRawType(utfRawArg(S)), T);
	else if (utfIsRaw(T))
		final = utfSat(mask, S, utfRawType(utfRawArg(T)));

	else if (tfIsTuple(utformTForm(T))) {
		final = utfSatTuple(mask, S, T);
	}
	else if (tfIsMulti(utformTForm(T))) {
		final = utfSatMulti(mask, S, T);
	}
	else if (tfIsCross(utformTForm(T))) {
		final = utfSatCross(mask, S, T);
	}
	else if (utfIsAnyMap(T)) {
		final = utfSatMap0(mask, S, T);
	}
	else if (utfIsCross(S)) {
		final = utfSatCrossLHS(mask, S, T);
	}
	else if (utfIsMulti(S)) {
		final = utfSatMultiLHS(mask, S, T);
	}
	else {
		unify = utformUnify(S, T);
		// At this point, validate the result condition
		if (utypeResultIsFail(unify)) {
			final = utfSatSimple(mask, TFS_UnifyFail);
		}
		else {
			uS = utypeResultApplyTForm(unify, S);
			uT = utypeResultApplyTForm(unify, T);
			result = tfSat1(mask, Sab, utformTForm(uS), utformTForm(uT));
			final = utfSatResult(result, TFS_Unify, unify);
		}
	}

	if (DEBUG(tfs)) {
		afprintf(dbOut, "%*s UtfSat %d --> %s\n", tfsDepthNo, "", serialThis, tfSatMaskToString(utfSatMaskMask(final)));
		afprintf(dbOut, "%*s UtfSat %d --> %pUTypeResult\n", tfsDepthNo, "", serialThis, utfSatMaskResult(final));
		if (utfSatMaskResult(final) && utfSatMaskResult(final)->conditions != listNil(Sefo))
			afprintf(dbOut, "%*s UtfSat %d --> %pAbSynList\n", tfsDepthNo, "", serialThis,
				 utfSatMaskResult(final)->conditions);
		afprintf(dbOut, "%*s UtfSat %d)\n", tfsDepthNo, "", serialThis);
	}
	tfsDepthNo--;
	return final;
}

USatMask
utfSat(SatMask mask, UTForm S, UTForm T)
{
	return utfSat1(mask, NULL, S, T);
}

USatMask
utfSatMap(SatMask mask, Stab stab, UTForm S, UTForm T,
	  AbSyn ab, Length argc, AbSynGetter argf)
{
	static int utfSatCount = 0;
	int count = utfSatCount++;
	USatMask	result;
	UTForm		Sret;
	AbSub		sigma;

	assert(utformIsAnyMap(S));

	if (utfSatMapCheckConstant(S, T, ab, argc, argf)) {
		return utfSatSimple(mask, tfSatMap(mask, stab, utformConstOrFail(S), utformConstOrFail(T), ab, argc, argf));
	}

	Sret = utfMapRet(S);

	sigma = absNew(stab);
	tfsDEBUG(dbOut, "(utfSatMap(%d): %pUTForm ++ %pUTForm\n", count, S, T);
	result = utfSatMapArgs(mask, sigma, S, ab, argc, argf);
	tfsDEBUG(dbOut, " utfSatMap(%d): Args: %d %pUTypeResult)\n", count, utfSatSucceed(result), result->result);
	if (utfSatSucceed(result)) {
		UTypeResult unifyResult;
		USatMask    retResult;
		SatMask     retMask;

		Sret = utformSubst(sigma, Sret);

		retResult = utfSat1(mask, ab, Sret, T);
		unifyResult = utypeResultMerge(result->result, retResult->result);
		retMask = tfSatEmbed(utfSatMaskMask(result)) | utfSatMaskMask(retResult);
		if (utypeResultIsFail(unifyResult))
			retMask = retMask | TFS_UnifyFail;
		result = utfSatResult(mask, retMask, unifyResult);

		if (utfSatSucceed(result) && tfSatCommit(mask) && utfIsConstant(Sret))
			abTUnique(ab) = utformConstOrFail(Sret);
	}

	tfsDEBUG(dbOut, " utfSatMap(%d): ---> %d %pUTypeResult)\n", count, utfSatSucceed(result), result->result);
	absFreeDeeply(sigma);

	return result;

}

local Bool
utfSatMapCheckConstant(UTForm S, UTForm T,
		       AbSyn ab, Length argc, AbSynGetter argf)
{
	TForm Stf, Ttf;
	int i;

	if (!utfIsConstant(S))
		return false;
	if (!utfIsConstant(T))
		return false;
	Stf = utformTForm(S);
	Ttf = utformTForm(T);

	if (tfAsMultiEmbed(tfMapArg(Stf), argc) == AB_Embed_Fail)
		return true;

	for (i=0; i<argc; i++) {
		Length parmc = tfMapHasDefaults(Stf) ? tfMapArgc(Stf) : argc;
		Length ai;
		Bool   def;
		int    pi  = Stf->rho ? Stf->rho[i] : i;
		TForm  tfi = tfAsMultiArgN(tfMapArg(Stf), parmc, pi);
		AbSyn  abi = tfAsMultiSelectArg(ab, argc, i, argf, tfi, &def, &ai);

		if (abState(abi) == AB_State_HasUnique)
			continue;
		else if (abState(abi) == AB_State_HasPoss
			 && !tpossIsConstant(abTPoss(abi)))
			return false;
		else if (abState(abi) == AB_State_Error)
			return false;
	}
	return true;;
}


Length
utfSatArgN(AbSyn ab, Length argc, AbSynGetter argf, Length parN, UTForm S)
{
	return tfSatArgN(ab, argc, argf, parN, utformTForm(S));
}

USatMask
utfSatTuple(SatMask mask, UTForm S, UTForm T)
{
	TForm Ttuple = utformTForm(T);
	TForm Stf = utformTForm(S);
	USatMask result = utfSatFalse(mask);

	if (tfIsTuple(Stf)) {
		result = utfSat1(mask, NULL, utfTupleArg(S), utfTupleArg(T));
	}
	else if (utfIsAny(S)) {
		UTypeResult unify = utformUnify(S, T);
		if (utypeResultIsFail(unify)) {
			result = utfSatSimple(mask, TFS_UnifyFail);
		}
		else {
			result = utfSatResult(mask, TFS_Unify, unify);
		}
	}
	else if (!tfSatEmbed(mask)) {
		result = utfSatFalse(mask);
	}
	else if (tfIsCross(Stf) && tfSatAllow(mask, TFS_CrossToTuple)) {
		result = utfSatEvery(mask, S, utfTupleArg(T));
		result = utfSatResultEmbed(result, TFS_CrossToTuple);
	}
	else if (tfIsMulti(Stf) && tfSatAllow(mask, TFS_MultiToTuple)) {
		result = utfSatEvery(mask, S, utfTupleArg(T));
		result = utfSatResultEmbed(result, TFS_MultiToTuple);
	}
	else if (!tfIsMulti(Stf)) {
		if (tfSatAllow(mask, TFS_UnaryToTuple)) {
			UTForm Targ = utfTupleArg(T);
			UTypeResult unify = utformUnify(S, Targ);
			if (!utypeResultIsFail(unify)) {
				result = utfSat(mask, S, Targ);
				result = utfSatResultEmbed(result, TFS_UnaryToTuple);
			}
		}
	}
	return result;
}


local USatMask
utfSatMulti(SatMask mask, UTForm S, UTForm T)
{
	Length		argc = utfMultiArgc(T);
	UTForm		Targ = utfMultiArgN(T, int0);
	USatMask	result = utfSatFalse(mask);
	USatMask	tmp;

	if (utfIsMulti(S)) {
		if (utfMultiArgc(S) == argc) {
			result = utfSatEach(mask, S, T);
		}
		/* Embed S in Multi(). */
		else if (tfSatAllow(mask, TFS_AnyToNone) && argc == 0) {
			result = utfSatSimple(mask, TFS_AnyToNone);
		}
	}
	else if (!tfSatEmbed(mask))
		/* result = utfSatFalse(mask) */;
	else if (utfIsCross(S)) {
		/* Embed Cross(A, ..., B) in Multi(A, ..., B). */
		if (tfSatAllow(mask, TFS_CrossToMulti) &&
		    utfCrossArgc(S) == argc) {
			tmp = utfSatEach(mask, S, T);
			if (utfSatSucceed(tmp))
				result = utfSatResult(mask, TFS_CrossToMulti, utfSatMaskResult(tmp));
		}
		/* Embed S in Multi(S). */
		else if (tfSatAllow(mask, TFS_UnaryToMulti) &&
			 argc == 1 &&
			 utfSatSucceed(tmp = utfSat(tfSatInner(mask), S, Targ))) {
			result = utfSatResult(mask, TFS_UnaryToMulti, utfSatMaskResult(tmp));
		}
		/* Embed S in Multi(). */
		else if (tfSatAllow(mask, TFS_AnyToNone) && argc == 0)
			result = utfSatSimple(mask, TFS_AnyToNone);
	}
	else {
		/* Embed S in Multi(S). */
		if (tfSatAllow(mask, TFS_UnaryToMulti) &&
		    argc == 1 &&
		    utfSatSucceed(tmp = utfSat(tfSatInner(mask), S, Targ))) {
			result = utfSatResult(mask, TFS_UnaryToMulti, utfSatMaskResult(tmp));
		}
		/* Embed S in Multi(). */
		else if (tfSatAllow(mask, TFS_AnyToNone) && argc == 0)
			result = utfSatSimple(mask, TFS_AnyToNone);
	}

	return result;
}

local USatMask
utfSatCross(SatMask mask, UTForm S, UTForm T)
{
	USatMask result = utfSatFalse(mask);
	USatMask tmp;
	Length   argc = utfCrossArgc(T);

	if (utfIsCross(S)) {
		if (utfCrossArgc(S) == argc) {
			result = utfSatEach(mask, S, T);
		}
		/* Embed S in Cross(S). */
		else if (tfSatAllow(mask, TFS_UnaryToCross) &&
			 argc == 1) {
			UTForm	Targ = utfCrossArgN(T, int0);
			tmp = utfSat(tfSatInner(mask), S, Targ);
			result = utfSatResultEmbed(tmp, TFS_UnaryToCross);
		}
	}

	else if (!tfSatEmbed(mask))
		/* result = tfSatFalse(mask) */;

	else if (utfIsMulti(S)) {
		/* Embed Multi(A, ..., B) in Cross(A, ..., B). */
		if (tfSatAllow(mask, TFS_MultiToCross) &&
		    utfMultiArgc(S) == argc) {
			tmp = utfSatEach(mask, S, T);
			if (utfSatSucceed(tmp)) {
				result = utfSatResultEmbed(tmp, TFS_MultiToCross);
			}
		}
	}

	else {
		/* Embed S in Cross(S). */
		if (tfSatAllow(mask, TFS_UnaryToCross) &&
		    argc == 1) {
			UTForm	Targ = utfCrossArgN(T, int0);
			tmp = utfSat(tfSatInner(mask), S, Targ);
			result = utfSatResultEmbed(tmp, TFS_UnaryToCross);
		}
	}

	return result;
}

local USatMask
utfSatEvery(SatMask mask, UTForm S, UTForm T)
{
	UTypeResult unify = utypeResultEmpty();
	USatMask result = utfSatTrue(mask);

	for (Length i=0; i<utfArgc(S); i++) {
		USatMask eltResult;
		eltResult = utfSat(mask, utfArgN(S, i), T);
		result = utfSatMaskCombine(mask, result, eltResult);
	}
	return result;
}

local USatMask
utfSatEach(SatMask mask, UTForm S, UTForm T)
{
	USatMask	result = utfSatTrue(mask);
	USatMask	tmp;
	Stab		stab = utfGetStab(T);
	AbSub		sigma;
	Length		i, argc = utfArgc(S);

	assert(utfArgc(S) == utfArgc(T));

	for (i = 0; !stab && i < argc; i += 1) {
		TForm	Ttf = tfArgv(utformTForm(T))[i];
		Syme	Tsyme = tfDefineeSyme(Ttf);

		if (Tsyme) stab = stabFindLevel(stabFile(), Tsyme);
	}

	sigma = absNew(stab);
	for (i = 0; utfSatSucceed(result) && i < argc; i += 1) {
		UTForm	Sarg = utfArgN(S, i);
		UTForm	Targ = utformSubst(sigma, utfArgN(T, i));
		Syme	Ssyme = utfDefineeSyme(Sarg);
		Syme	Tsyme = utfDefineeSyme(Targ);
		AbSyn	ab = NULL;

		if (!utfSatSucceed(tmp = utfSat(tfSatInner(mask), Sarg, Targ))) {
			result = utfSatFalse(mask);
		}
		else {
			result = utfSatMaskCombine(mask, result, tmp);
		}

		if (Ssyme && Ssyme != Tsyme)
			ab = abFrSyme(Ssyme);
		else if (utfIsDefine(Sarg))
			ab = tfGetExpr(utformTForm(utfDefineVal(Sarg)));

		/* Extend the sublist for dependent symes. */
		if (stab && ab && Tsyme) {
			tiTopFns()->tiBottomUp(stab, ab, tfUnknown);
			tiTopFns()->tiTopDown (stab, ab, utformTForm(Targ));
			if (abState(ab) == AB_State_HasUnique) {
				if (absFVars(sigma))
					absSetFVars(sigma, NULL);
				sigma = absExtend(Tsyme, ab, sigma);
			}
			else
				result = utfSatFalse(mask);
		}
	}

	absFree(sigma);
	return result;
}

UTForm
utfSatMapArgAnyTuple(Stab stab, UTForm utf)
{
	UTForm arg;
	utf = utformFollow(utf);
	assert(utfIsAnyMap(utf));

	arg = utfMapArg(utf);
	if (!utfIsAny(arg))
		return NULL;

	AbSub sigma = absNew(stabFile());
	Syme syme = symeNewLexConst(symGen(), tfType, car(stabFile()));
	utf = utformNew(listCons(Syme)(syme, utformVars(utf)), utformTForm(utf));
	TForm tupleAny = tfTuple(tfFrSyme(stabFile(), syme));
	AbSyn expr = tfExpr(tupleAny);
	tiTopFns()->tiBottomUp(stab, expr, tfUnknown);
	tiTopFns()->tiTopDown(stab, expr, tfUnknown);
	absExtend(tfIdSyme(utformTForm(arg)), expr, sigma);
	UTForm utfTuple = utformSubst(sigma, utf);
	absFree(sigma);
	if (tfsDebug) {
		afprintf(dbOut, "AnyTuple %pUTForm %pAbSub %pUTForm\n", utf, sigma, utfTuple);
	}
	return utfTuple;
}

UTForm
utfSatMapArgAnyCross(Stab stab, Length argc, UTForm utf)
{
	UTForm arg;
	utf = utformFollow(utf);
	assert(utfIsAnyMap(utf));

	arg = utfMapArg(utf);
	if (!utfIsAny(arg))
		return NULL;

	if (argc < 2)
		return NULL;

	SymeList vars = listNil(Syme);
	TFormList tfl = listNil(TForm);
	for (int i = 0; i < argc; i++) {
		Syme syme = symeNewLexConst(symGen(), tfType, car(stabFile()));
		TForm tfi = tfFrSyme(stabFile(), syme);
		vars = listCons(Syme)(syme, vars);
		tfl = listCons(TForm)(tfi, tfl);
	}
	AbSub sigma = absNew(stabFile());
	TForm tfc = tfCrossFrList(listNReverse(TForm)(tfl));

	AbSyn expr = tfExpr(tfc);
	tiTopFns()->tiBottomUp(stab, expr, tfUnknown);
	tiTopFns()->tiTopDown(stab, expr, tfUnknown);
	absExtend(tfIdSyme(utformTForm(arg)), expr, sigma);
	SymeList ovars = listNRemove(Syme)(listCopy(Syme)(utformVars(utf)), tfIdSyme(utformTForm(arg)), (Bool (*)(Syme, Syme)) ptrEqualFn);
	UTForm utfCrossFn = utformNew(listConcat(Syme)(listNReverse(Syme)(vars), ovars),
				      tformSubst(sigma, utformTForm(utf)));
	absFree(sigma);
	if (tfsDebug) {
		afprintf(dbOut, "AnyTuple %pUTForm %pAbSub %pUTForm\n", utf, sigma, utfCrossFn);
	}

	return utfCrossFn;
}

int
utfSatMaskFormatter(OStream ostream, Pointer p)
{
	USatMask mask = (USatMask) p;
	int c = 0;

	c += ostreamPrintf(ostream, "{%s / %pUTypeResult}",
			   tfSatMaskToString(utfSatMaskMask(mask)),
			   utfSatMaskResult(mask));

	return c;
}

/******************************************************************************
 *
 * :: Type form satisfaction flags.
 *
 *****************************************************************************/

static TForm tfSatPendingFailValue;

local void
tfSatSetPendingFail(TForm S)
{
	tfSatPendingFailValue = S;
}

TForm
tfSatGetPendingFail()
{
	return tfSatPendingFailValue;
}
