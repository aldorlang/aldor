/*****************************************************************************
 *
 * gf_seq.c: Generating code for add/default definition levels 
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "depdag.h"
#include "format.h"
#include "gf_add.h"
#include "gf_implicit.h"
#include "gf_prog.h"
#include "gf_seq.h"
#include "gf_util.h"
#include "stab.h"
#include "store.h"
#include "sefo.h"
#include "lib.h"
#include "tfsat.h"
#include "ablogic.h"
#include "abpretty.h"
#include "comsg.h"
#include "table.h"
#include "strops.h"
#include "symbol.h"

extern	Bool genfExportDebug;	/* gf_add.c */
Bool	genfImplicitDebug	= false;

#define genfExportDEBUG		if (DEBUG(genfExport))
#define genfImplicitDEBUG	if (DEBUG(genfImplicit))

typedef enum {
	DG_Const,
	DG_Lambda,
	DG_Cond,
	DG_Fix,
	DG_Type,
	DG_NonDefn
} DefGroupTag;

typedef struct defGroup *DefGroup;
typedef struct defSet	*DefSet;

DECLARE_LIST(DefGroup);

struct defGroup { 
	DefGroupTag 	tag;
	AbSyn 		ab;
	int 		ordinal;
	SymeList 	defines;
	SymeList	usedSymes;
	DefGroupList	usedDefs;
};

struct defSet {
	int 		argc;
	DefGroupList 	defs;
	SymeList	defines;
	SymeList 	exports;
};

#define dgTag(dg)		((dg)->tag)
#define dgUses(dg)		((dg)->usedSymes)
#define dgDefines(dg)		((dg)->defines)
#define dgStmt(dg)		((dg)->ab)

#define dgSetDefs(ds)		((ds)->defs)

local void	gen0EnsureUsedSymes	(DefSet, DefGroup);
local void	gen0Fix			(AbSyn, SymeList);
local void	gen0DefTypeCond		(AbSyn, SymeList);

local void 		dgSortDefs		(DefSet);
local int 		dgSortClassify		(DefGroup);
local DefSet		dgSeqToDefSet		(AbSyn, SymeList);
local DefGroup		dgMakeFixGroup		(DefGroupList);
local DefGroup		dgStmtToDef		(AbSyn, int);
local void		dgAbGetUsedSymes	(AbSyn, DefGroup);
local DefGroupTag	dgGetTag		(AbSyn);
local Bool		dgSymeIsLocal		(Syme);
local DefGroup		dgNewGroup		(DefGroupTag, AbSyn);
local void		dgFreeGroup		(DefGroup);
local DefGroupList	dgProcessDependencies	(DefGroupList, Bool);
local Syme		dgSymeImportToExport	(Syme);
local DefGroupList	dgSortDependencies	(DepDag, DepDagList, Bool);
local void		dgCycleError		(DepDag, DepDagList);
local void		dgOrderError		(DepDag, DepDagList);
local String		dgPretty		(DefGroup);
local Syme		dgGetAbMeaning		(AbSyn);
local SymeList		dgFindDependencies	(AbSyn);


/******************************************************************************
 *
 * :: Generating sequences
 *
 *****************************************************************************/

void
gen0DefTypeSequence(AbSyn ab, SymeList exports)
{
	DefSet 		set;
	DefGroupList	lst;
	SymeList 	symes, isymes;

	genfImplicitDEBUG {
		(void)fprintf(dbOut, "gen0DefTypeSequence():");
		fnewline(dbOut);
		for (symes = exports;symes;symes = cdr(symes)) {
			Syme sy  = car(symes);
			Bool im  = symeIsImplicit(sy);
			String s = symePretty(sy);

			(void)fprintf(dbOut,"   [%c] %s",(im ? '*' : ' '),s);
			fnewline(dbOut);
		}
		fnewline(dbOut);
	}


	if (abTag(ab) == AB_Nothing)
		return;

	set = dgSeqToDefSet(ab, exports);

	dgSortDefs(set);

	lst = dgSetDefs(set);

	while (lst) {
		DefGroup dg = car(lst);
		genfExportDEBUG {
			fprintf(dbOut, "looking at:\n");
			abWrSExpr(dbOut, dgStmt(dg),int0);
			fprintf(dbOut, "defines:\n");
			symeListPrintDb(dg->defines);
			fprintf(dbOut, "uses:\n");
			symeListPrintDb(dgUses(dg));
		}
		gen0EnsureUsedSymes(set, dg);

		switch (dg->tag) {
		  case DG_Fix:
			gen0Fix(dgStmt(dg), dg->defines);
			break;
		  case DG_Cond:
			gen0DefTypeCond(dgStmt(dg), exports);
			break;
		  default:
			genFoamStmt(dgStmt(dg));
			break;
		}
		for (symes = dg->defines; symes; symes = cdr(symes))
			if (symeIsExport(car(symes)))
				gen0TypeAddExportSlot(car(symes));
		dgFreeGroup(dg);
		lst = cdr(lst);
	}


	/* Collect implicit exports */
	isymes = listNil(Syme);
	for (symes = exports;symes;symes = cdr(symes))
	{
		Syme syme = car(symes);

		if (symeIsImplicit(syme) && symeIsExport(syme)) {
			/* Assume unconditional */
			symeSetUnconditional(syme);


			/* Add to the list */
			isymes = listCons(Syme)(syme, isymes);
		}
	}


	/* Do we have any implicit symes? */
	if (isymes == listNil(Syme)) return;


	/* Create definitions for these exports */
	for (symes = isymes;symes;symes = cdr(symes))
	{
		Syme syme = car(symes);

		gen0ImplicitExport(syme, exports, ab);
		gen0TypeAddExportSlot(syme);
	}
}

/*!! Should be expanded to allow imports and inheritance 
 *   from conditional symes
 */
local void
gen0DefTypeCond(AbSyn ab, SymeList exports)
{
	FoamList topLines;
	int	l1 = gen0State->labelNo++, l2 = gen0State->labelNo++;
	Bool flag;

 	/* COND-DEF */
	AbLogic	saveCond;
	AbSyn	nTest;
	Stab	stab = abStab(ab) ? abStab(ab) : stabFile();

	flag = gen0AddImportPlace(&topLines);

	nTest = abExpandDefs(stab, (ab->abIf.test)); /* COND-DEF */

	gen0AddStmt(foamNewIf(genFoamBit(ab->abIf.test), l1), ab);
	ablogAndPush(&gfCondKnown, &saveCond, nTest, false); /* COND-DEF */
	gen0DefTypeSequence(ab->abIf.elseAlt, exports);
	ablogAndPop (&gfCondKnown, &saveCond); /* COND-DEF */
	gen0AddStmt(foamNewGoto(l2), ab);

	gen0AddStmt(foamNewLabel(l1), ab);
	ablogAndPush(&gfCondKnown, &saveCond, nTest, true); /* COND-DEF */
	gen0DefTypeSequence(ab->abIf.thenAlt, exports);
	ablogAndPop (&gfCondKnown, &saveCond); /* COND-DEF */
	gen0AddStmt(foamNewLabel(l2), ab);

	if (flag) gen0ResetImportPlace(topLines);
}


local void
gen0EnsureUsedSymes(DefSet set, DefGroup dg)
{
	SymeList symes = dgUses(dg);
	
	while (symes) {
		Syme syme = car(symes);
		if (symeIsExport(syme) && gen0SymeInit(syme) == NULL) {
			gen0InitExport(syme);
			gen0AddInit(foamNewSet(gen0ExpMapRef(syme),
					       foamNewBool(false)));
			gen0AddStmt(foamNewSet(gen0ExpMapRef(syme),
					       foamNewBool(true)), NULL);
			set->defines = listCons(Syme)(syme, set->defines);
			gen0SymeSetInit(syme, gen0Syme(syme));
		}
		symes = cdr(symes);
	}
}

/******************************************************************************
 *
 * :: Fix-Pointing expressions...
 *
 *****************************************************************************/

typedef struct {
	FoamList inits;
	FoamList finis;
} FixInfoStruct, *FixInfo;

local void 	gen0FixDummy		(Syme, AbSyn, FixInfo);
local Foam	gen0FixMakeDummyCat	(void);
local Foam	gen0FixMakeDummyDom	(void);
local Foam	gen0FixFillDom		(Foam, Foam);
local Foam	gen0FixFillCat		(Foam, Foam);
local void	gen0FixGenStmts		(FoamList, AbSyn);

local void
gen0Fix(AbSyn ab, SymeList dsymes )
{
	SymeList symes;
	FixInfoStruct info_s;
	FixInfo       info = &info_s;
	int i;

	info->inits = listNil(Foam);
	info->finis = listNil(Foam);
	for (symes = dsymes; symes ; symes = cdr(symes)) 
		gen0FixDummy(car(symes), ab, info);

	info->inits = listNReverse(Foam)(info->inits);
	info->finis = listNReverse(Foam)(info->finis);

	gen0FixGenStmts(info->inits, ab);

	for (i = 0; i< abArgc(ab) ; i++) 
		genFoamStmt(abArgv(ab)[i]);
	
	gen0FixGenStmts(info->finis, ab);
	
	listFree(Foam)(info->inits);
	listFree(Foam)(info->finis);
	return ;
}

local void
gen0FixDummy(Syme syme, AbSyn pos, FixInfo info)
{
	TForm 	tf = symeType(syme);
	Foam 	self = gen0Syme(syme);
	Foam 	new;

	/* Record the initialisation, if necessary */
	if (symeIsExport(syme) && !gen0SymeInit(syme))
		gen0SymeSetInit(syme, foamCopy(self));

	if (tfSatDom(tf)) {
		new = gen0Temp(FOAM_Word);
		info->inits = listCons(Foam)
			(foamNewSet(foamCopy(self), gen0FixMakeDummyDom()),
			 info->inits);
		info->finis = listCons(Foam)
			(gen0FixFillDom(foamCopy(new), foamCopy(self)),
			 info->finis);

	}
	else if (tfSatCat(tf)) {
		new = gen0Temp(FOAM_Word);
		info->inits = listCons(Foam)
			(foamNewSet(foamCopy(self), gen0FixMakeDummyCat()),
			 info->inits);
		info->finis = listCons(Foam)
			(gen0FixFillCat(foamCopy(new), foamCopy(self)),
			 info->finis);

	} 	
	else {
		foamFree(self);
		return;
	}
	info->inits = listCons(Foam)(foamNewDef(foamCopy(new), 
						foamCopy(self)), info->inits);
	info->finis = listCons(Foam)(foamNewSet(foamCopy(self),
						foamCopy(new)), info->finis);

	foamFree(new);
	foamFree(self);
}

local Foam
gen0FixMakeDummyCat()
{
	return gen0BuiltinCCall(FOAM_Word, "categoryMakeDummy", "runtime", int0);
}

local Foam
gen0FixFillCat(Foam new, Foam self)
{
	return gen0BuiltinCCall(FOAM_NOp, "categoryFill!", "runtime", 2, new, self);
}

local Foam
gen0FixMakeDummyDom()
{
	return gen0BuiltinCCall(FOAM_Word, "domainMakeDummy", "runtime", int0);
}

local Foam
gen0FixFillDom(Foam new, Foam self)
{
	return gen0BuiltinCCall(FOAM_NOp, "domainFill!", "runtime", 2, new, self);
}


local void
gen0FixGenStmts(FoamList lst, AbSyn pos)
{
	while (lst) {
		gen0AddStmt(car(lst), pos);
		lst = cdr(lst);
	}
}

/******************************************************************************
 *
 * :: DefGroup/Set manipulation
 *
 *****************************************************************************/

/*
 * Two stages in defining a top level export
 * 1. Defining 
 * 2. Exporting
 *
 * Defining is safe providing all the symes used by the definition
 * have either been defined or imported, or the defn is a with, add or lambda
 * 
 * Exporting is safe providing all the symes used in the type of the 
 * definition have been defined.
 *
 * 'Used' here is a closure over the textual definition ignoring lambda-inducing
 * constructs. [NB this is not sufficient for absolute safety]
 *
 * We also must try to spot mutual recursion in definitions of types.
 * In this situation an AB_Fix node is created, which can then be dealt with by
 * gf_add.c
 *
 * Sort process:
 *   1) Find dependencies
 *   2) Compute dependencies, NB subsumption
 *   3) Sort
 *   4) transform to linear format
 *
 * We sort into the following groups:
 *   Tough maps		(list[0]) (domain/category DG_Const -> DG_Fix)
 *   Simple maps	(list[1]) (DG_Lambda)
 *   Types		(list[2]) (DG_Type)
 *   Other junk		(list[3]) (DG_Const, DG_Cond)
 *   Non-definitions	(list[4]) (DG_NonDefn)
 *
 * Locals used to be lumped together with "other junk" as DG_Const nodes.
 * Then local lambda's were split into a group of their own after DG_Type.
 * Simple locals (just one local being defined) are now placed in their
 * proper group (local lambdas go in DG_Lambda, local consts in "other junk"
 * etc); complicated locals stay in "other junk" unless they are DG_NonDefn.
 *
 * It would be nice if we could use dgProcessDependencies() to re-order
 * the non-lazy constants so that their values are correctly computed.
 * Unfortunately some people write code in add bodies in which non-lazy
 * constants depend on the values of local variables. Thus re-ordering
 * is not viable.
 *
 * Instead, we now place all members of "other junk" in with "non-defns"
 * for the overall sorting operation. We also take a separate copy of
 * "other junk" and perform dependency analysis to check for user bugs.
 */
CREATE_LIST(DefGroup);

#define DG_GROUP_TOUGH_MAP	(0)
#define DG_GROUP_SIMPLE_MAP	(1)
#define DG_GROUP_TYPE		(2)
#define DG_GROUP_OTHER_JUNK	(3)
#define DG_GROUP_NON_DEFN	(4)

#define DG_MAX			(DG_GROUP_NON_DEFN+1)

static SymeList dgExports;

local void
dgSortDefs(DefSet set)
{
	int		i;
	DefGroupList	otherJunk = listNil(DefGroup);
	DefGroupList	lists[DG_MAX];
	DefGroupList	lst = dgSetDefs(set);
	DefGroup	types;

	for (i=0; i<DG_MAX; i++) 
		lists[i]=listNil(DefGroup);


#if SORT_NON_LAZY_CONSTANTS
	/*
	 * This code separates non-lazy constants from other
	 * non-definitions in an add-body before sorting them
	 * so that their dependencies will be satisfied when
	 * code generation takes place. Note that this causes
	 * severe problems when people write code in which
	 * non-lazy constants depend on the value of variables.
	 */
	while (lst) {
		DefGroup dg = car(lst);


		/* Classify into one of the sets */
		i = dgSortClassify(dg);


		/* Add to the chosen set */
		lists[i] = listCons(DefGroup)(dg, lists[i]);


		/* Get the next group */
		lst = cdr(lst);
	}


	/* The "other junk" group needs special care */
	tmp = lists[DG_GROUP_OTHER_JUNK];
	tmp = dgProcessDependencies(tmp, true);
	lists[DG_GROUP_OTHER_JUNK] = tmp;
#else
	/*
	 * This code keeps non-lazy constants in the same
	 * set as other non-definitions in an add-body and
	 * does not sort them. However, these constants
	 * are recorded separately and analysed for bad
	 * dependencies.
	 */
	while (lst) {
		DefGroup dg = car(lst);


		/* Classify into one of the sets */
		i = dgSortClassify(dg);


		/* DG_GROUP_OTHER_JUNK is treated differently */
		if (i == DG_GROUP_OTHER_JUNK) {
			/* Record DG_GROUP_OTHER_JUNK separately */
			otherJunk = listCons(DefGroup)(dg, otherJunk);


			/* But also place them with DG_GROUP_NON_DEFN */
			i = DG_GROUP_NON_DEFN;
		}


		/* Add to the relevent set */
		lists[i] = listCons(DefGroup)(dg, lists[i]);


		/* Get the next group */
		lst = cdr(lst);
	}


	/* Check for bad dependencies */
	assert(!lists[DG_GROUP_OTHER_JUNK]);
	otherJunk = dgProcessDependencies(otherJunk, false);
	listFree(DefGroup)(otherJunk);
#endif


	/* Concatenate the simple groups (assume !DG_GROUP_TOUGH_MAP) */
	assert(!lst);
	for (i = DG_MAX - 1; i; i--) {
		DefGroupList	tmp = listNReverse(DefGroup)(lists[i]);
		lst = listNConcat(DefGroup)(tmp, lst);
	}


	/* Tough maps require special care */
	if (lists[DG_GROUP_TOUGH_MAP]) {
		types = dgMakeFixGroup(lists[DG_GROUP_TOUGH_MAP]);
		lst = listCons(DefGroup)(types, lst);
		listFreeDeeply(DefGroup)(lists[DG_GROUP_TOUGH_MAP],dgFreeGroup);
	}

	dgSetDefs(set) = lst;
}


local int 
dgSortClassify(DefGroup dg)
{
	SymeList lst;
	TForm 	 tf;
	int class;

	if (dgTag(dg) == DG_Lambda)
		return DG_GROUP_SIMPLE_MAP;
	else if (dgTag(dg) == DG_Type)
		return DG_GROUP_TYPE;
	else if (dgTag(dg) == DG_NonDefn)
		return DG_GROUP_NON_DEFN;

	
	lst = dg->defines;
	class = DG_GROUP_OTHER_JUNK;

	while (lst) {
		tf = tfDefineeType(symeType(car(lst)));
		if (tfSatDom(tf) || tfSatCat(tf))
			class = DG_GROUP_TOUGH_MAP;
		lst = cdr(lst);
	}
	return class;
}

local DefGroupList
dgSeqToDefs(AbSyn ab)
{
	DefGroupList lst = listNil(DefGroup);
	int i;

	switch(abTag(ab)) {
	  case AB_Sequence:
	  case AB_Default:
		for (i = 0; i < abArgc(ab); i++) {
			AbSyn sub = ab->abSequence.argv[i];
			lst = listNConcat(DefGroup)(dgSeqToDefs(sub), lst);
		}
		break;
	  default:
		lst = listCons(DefGroup)(dgStmtToDef(ab, int0), lst);
	}


	/*
	 * This function may be applied to nested sequences. To
	 * ensure that all the definitions stay in the same order
	 * as in the source code, it is essential that the list
	 * is not reversed during recursive calls. Instead the
	 * result of the top-level call must be reversed.
	 */
	return lst;
}

local DefSet
dgSeqToDefSet(AbSyn ab, SymeList exports)
{
	DefSet new = (DefSet) stoAlloc(OB_Other, sizeof(*new));
	
	dgExports 	= exports;
	new->argc 	= 0;
	new->defs 	= listNReverse(DefGroup)(dgSeqToDefs(ab));
	new->exports 	= exports;
	dgExports	= NULL;

	return new;
}


local DefGroup
dgMakeFixGroup(DefGroupList dlst)
{
	AbSynList    stmts   = listNil(AbSyn);
	SymeList     defines = listNil(Syme);
	SymeList     uses    = listNil(Syme);
	SymeList     lst;
	DefGroup     new;
	int 	     ordinal = 0;

	while (dlst) {
		DefGroup dg = car(dlst);
		stmts       = listCons(AbSyn)(dg->ab, stmts);
		defines = listConcat(Syme)(dg->defines, defines);
		uses    = listConcat(Syme)(dg->usedSymes, uses);
		ordinal = ordinal < dg->ordinal ? dg->ordinal : ordinal;

		dlst = cdr(dlst);
	}
	new = dgNewGroup(DG_Fix, abNewOfList(AB_Fix, sposNone, 
					listNReverse(AbSyn)(stmts)));
	
	lst = uses;
	uses = listNil(Syme);
	while (lst) {
		if (!listMemq(Syme)(defines, car(lst)))
			uses = listCons(Syme)(car(lst), uses);
		lst = cdr(lst);
	}
	new->defines = defines;
	new->usedSymes = uses;
	new->ordinal = ordinal;

	return new;
}

local DefGroup
dgStmtToDef(AbSyn absyn, int i)
{
	DefGroup dg = dgNewGroup(dgGetTag(absyn), absyn);
	
	dgAbGetUsedSymes(absyn, dg);
	if (dg->tag == DG_Cond) {
		listFree(Syme)(dg->defines);
		dg->defines = listNil(Syme);
	}
	dg->ordinal = i;
	return dg;
}


local void
dgAbGetUsedSymes(AbSyn ab, DefGroup dg)
{
	Syme	syme;
	int	i, argc;
	AbSyn	*argv;
	
	switch(abTag(ab)) {
	  case AB_Add:
	  case AB_With:
	  case AB_Generate:
	  case AB_Lambda:
	  case AB_PLambda:
		return;

	  case AB_Define:
		/* Treat all definitions as a multi */
		argc = abArgcAs(AB_Comma, ab->abDefine.lhs);
		argv = abArgvAs(AB_Comma, ab->abDefine.lhs);


		/* Find out all the things being defined */
		for (i = 0; i < argc; i++) {
			syme = abSyme(abDefineeId(argv[i]));
			if (!syme) continue;
			dg->defines = listCons(Syme)(syme, dg->defines);
		}


		/* Now find all the symes used from the rhs */
		dgAbGetUsedSymes(ab->abDefine.rhs, dg);
		break;

	  case AB_LitInteger:
	  case AB_LitString:
	  case AB_LitFloat:
	  case AB_Id:
		if ((syme = abSyme(ab)) != NULL && dgSymeIsLocal(syme)) {
			dg->usedSymes = listCons(Syme)(syme, dgUses(dg));
		}
		break;
	  case AB_CoerceTo:
	  case AB_Test:
	  case AB_For:
		if ( (syme = abImplicitSyme(ab)) != NULL && dgSymeIsLocal(syme))
			dg->usedSymes = listCons(Syme)(syme, dgUses(dg));
		for (i=0; i<abArgc(ab); i++)
			dgAbGetUsedSymes(abArgv(ab)[i], dg);
		break;
	  default:
		if ((syme = abSyme(ab)) != NULL && dgSymeIsLocal(syme)) {
			dg->usedSymes = listCons(Syme)(syme, dgUses(dg));
		}
		for (i=0; i<abArgc(ab); i++)
			dgAbGetUsedSymes(abArgv(ab)[i], dg);
		break;
	}
		
}

local DefGroupTag
dgGetTag(AbSyn absyn)
{
	switch(abTag(absyn)) {
	  case AB_Fix:
		return DG_Fix;
	  case AB_Define:
		switch (abTag(absyn->abDefine.rhs)) {
		  case AB_Lambda:
		  case AB_PLambda:
			return DG_Lambda;
		  case AB_Add:
		  case AB_With:
			return DG_Type;
		  default:
			return DG_Const;
		}
	  case AB_If:
		return DG_Cond;
	  case AB_Local: {
		/* Can only handle simple locals */
		if (abArgc(absyn) != 1)
			return DG_Const; /* can do better than this ... */

		/* Return the tag of the local object */
		return dgGetTag(absyn->abLocal.argv[0]);
	  }
	  default:
		return DG_NonDefn;
	}
}

local Bool
dgSymeIsLocal(Syme syme)
{
	if (symeIsParam(syme) || symeIsImport(syme))
		return false;
	if (symeIsExport(syme) && !listMemq(Syme)(dgExports, syme))
		return false;

	return true;

}

local DefGroup
dgNewGroup(DefGroupTag tag, AbSyn ab)
{
	DefGroup new;

	new = (DefGroup) stoAlloc(OB_Other, sizeof(*new));
	
	new->tag 	= tag;
	new->ab		= ab;
	new->usedSymes	= NULL;
	new->defines	= NULL;
	new->ordinal	= -1;

	return new;
}

local void
dgFreeGroup(DefGroup dg)
{
	listFree(Syme)(dg->defines);
	listFree(Syme)(dg->usedSymes);

	stoFree(dg);
}


/******************************************************************************
 *
 * :: Dependency analysis
 *
 *****************************************************************************/


/*
 * Analyse the dependencies between a set of DG_Const and DG_Cond
 * groups and return them correctly sorted. If `reOrder' is false
 * then it is an error if a defgroup depends on one that has a
 * higher ordinal (was defined later in the code).
 */
local DefGroupList
dgProcessDependencies(DefGroupList defs, Bool reOrder)
{
	int		ord = -10;
	DepDag		root;
	Table		symeToDegDag;
	DefGroupList	result = listNil(DefGroup);


	/* Create a new mapping table */
	symeToDegDag = tblNew((TblHashFun)symeHashFn, (TblEqFun)symeEqual);


	/* Root dag node with no dependencies */
	root = depdagNewLeaf((void *)NULL);


	/*
	 * Phase 1: compute the mapping from symes to defgroups
	 * and add the dependencies of the root node. Each group
	 * is numbered such that the first defgroup in the source
	 * has the lowest number.
	 */
	listIter(DefGroup, dg, defs, {
		AbSyn	absyn = dgStmt(dg);
		Syme	syme = dgGetAbMeaning(absyn);
		DepDag	node = depdagNewLeaf((void *)dg);


		/* Number the defgroup */
		dg->ordinal = --ord;


		/* Add root dependency */
		root = depdagAddDependency(root, node);


		/*
		 * Only index meaningful dgs. The side-effect of this
		 * is that we only find dependencies between dgs that
		 * have meaning.
		 */
		if (syme) tblSetElt(symeToDegDag, (TblKey)syme, (TblElt)node);
	});


	/* Phase 2: compute dependencies */
	listIter(DepDag, dd, depdagDependsOn(root), {
		DefGroup	dg = (DefGroup)depdagLabel(dd);
		AbSyn		absyn = dgStmt(dg);
		SymeList	deps = dgFindDependencies(absyn);


		/* Skip if no dependencies found */
		if (!deps) continue;


		/* Add each dependency */
		listIter(Syme, syme, deps, {
			DepDag	dag;


			/* Search for the defgroup */
			dag = tblElt(symeToDegDag, (TblKey)syme, (TblElt)NULL);


			/* Skip if not found */
			if (!dag) continue;


			/* Add the dependency */
			dd = depdagAddDependency(dd, dag);
		});


		/* Release storage */
		listFree(Syme)(deps);
	});


	/* Phase 3: sort */
	result = dgSortDependencies(root, listNil(DepDag), reOrder);


	/*
	 * Phase 4: clean up. Note that the peculiar nature of
	 * this dependency graph is that all nodes are children
	 * of the root node. We don't have to walk the graph
	 * to find nodes to free.
	 */
	listFree(DepDag)(depdagDependsOn(root));
	depdagFree(root);
	tblFree(symeToDegDag);


	/* Return the sorted list */
	return listNReverse(DefGroup)(result);
}


/*
 * dgSortDependencies(dag, deps, reOrder) produces a sorted list of
 * def-groups from the dependency DAG "dag". If the caller doesn't
 * want dependencies re-ordered then "reOrder" must be false and this
 * function generates an error if a valid sort cannot be computed
 * without re-ordering definitions. The "deps" parameter is a list
 * of dependencies which are pending and is used simply for telling
 * the user which nodes occur in a cycle in the graph if one is found.
 */
local DefGroupList
dgSortDependencies(DepDag dag, DepDagList depends, Bool reOrder)
{
	DefGroupList	tmp;
	DefGroupList	result = listNil(DefGroup);
	DefGroup	us = (DefGroup)depdagLabel(dag);
	DepDagList	deps = depends;
	DepDagList	orders = listNil(DepDag);


	/* Have we been here before and finished? */
	if (depdagIsFinished(dag)) return result;


	/* Have we been here before and not finished? */
	if (depdagIsPending(dag)) {
		/* Cycle in dependency graph: report the error */
		dgCycleError(dag, depends);


		/* Ignore this dependency */
		return result;
	}


	/* Add ourselves to the dependency chain (if not the root) */
	if (us) deps = listCons(DepDag)(dag, deps);


	/* Indicate that we are processing this node */
	depdagSetPending(dag, true);


	/* Process our dependencies first */
	listIter(DepDag, d, depdagDependsOn(dag), {
		/* Convert into a defgroup */
		DefGroup dg = (DefGroup)depdagLabel(d);


		/* Check for dependency ordering errors */
		if (!reOrder && us && (dg->ordinal > us->ordinal))
			orders = listCons(DepDag)(d, orders);


		/* Sort the dependencies */
		tmp = dgSortDependencies(d, deps, reOrder);


		/* Add to the result list if found */
		if (tmp) result = listNConcat(DefGroup)(result, tmp);
	});


	/* Emit ordering errors in one go */
	if (orders) {
		dgOrderError(dag, orders);
		listFree(DepDag)(orders);
	}


	/* Remove ourselves from the dependency chain (if not the root) */
	if (us) deps = listFreeCons(DepDag)(deps);


	/* Add ourselves to the list (if not the root) */
	us = (DefGroup)depdagLabel(dag);
	if (us) {
		tmp = listSingleton(DefGroup)(us);
		result = listNConcat(DefGroup)(result, tmp);
	}


	/*
	 * Indicate that we have finished processing this node. We
	 * clear the pending flag for tidiness but don't have to.
	 */
	depdagSetFinished(dag, true);
	depdagSetPending(dag, false);


	/* Return the sorted list */
	return result;
}


local void
dgCycleError(DepDag dag, DepDagList deps)
{
	Buffer		buf = bufNew();
	String		pretty;
	DefGroup	dg = (DefGroup)depdagLabel(dag);
	DepDagList	ptr, cycle = listSingleton(DepDag)(dag);


	/* Root cannot be pending */
	assert(deps);
	assert(dg);


	/* Compute the dependency cycle */
	for (;deps && (car(deps) != dag);deps = cdr(deps)) {
		DepDag	d = car(deps);
		cycle = listCons(DepDag)(d, cycle);
	}


	/* Start with the bad dependency source/target */
	pretty = dgPretty(dg);
	bufPrintf(buf, "<%s, ", pretty);
	strFree(pretty);


	/* Display the dependency cycle */
	for (ptr = cycle; ptr; ptr = cdr(ptr)) {
		DepDag		d = car(ptr);
		DefGroup	dg = (DefGroup)depdagLabel(d);


		/* Format this node */
		pretty = dgPretty(dg);
		bufPrintf(buf, "%s", pretty);
		strFree(pretty);


		/* Any more nodes in this cycle? */
		if (cdr(ptr)) bufPrintf(buf, ", ");
	}
	bufPrintf(buf, ">");


	/* Release storage used */
	listFree(DepDag)(cycle);


	/* Report the error */
	comsgWarning(dgStmt(dg), ALDOR_W_GenBadDefCycle, bufChars(buf));


	/* Free the buffer */
	bufLiberate(buf);
}


local void
dgOrderError(DepDag dag, DepDagList deps)
{
	Buffer		buf = bufNew();
	String		pretty, s;
	DefGroup	dg = (DefGroup)depdagLabel(dag);
	DepDagList	ptr, order = listNil(DepDag);


	/* Root cannot be pending */
	assert(deps);
	assert(dg);


	/* Re-order the dependency list */
	for (;deps;deps = cdr(deps)) {
		DepDag	d = car(deps);
		order = listCons(DepDag)(d, order);
	}


	/* Display the dependencies */
	for (ptr = order; ptr; ptr = cdr(ptr)) {
		DepDag		d = car(ptr);
		DefGroup	dg = (DefGroup)depdagLabel(d);


		/* Format this node */
		pretty = dgPretty(dg);
		bufPrintf(buf, "`%s'", pretty);
		strFree(pretty);


		/* Any more nodes in this cycle? */
		if (cdr(ptr)) {
			if (cdr(cdr(ptr)))
				bufPrintf(buf, ", ");
			else
				bufPrintf(buf, " and ");
		}
	}


	/* Release storage used */
	listFree(DepDag)(order);


	/* Name of the problem definition */
	s = dgPretty(dg);


	/* Report the error */
	comsgWarning(dgStmt(dg), ALDOR_W_GenBadDefOrder, s, bufChars(buf), s);


	/* Free the buffer and other storage */
	bufLiberate(buf);
	strFree(s);
}


/*
 * Pretty-print the symbol associated with a def-group for use in
 * error messages about cyclic dependencies. We assume that this
 * node has meaning: if it didn't we would not have been able to
 * work out that there is a cycle involved.
 */
local String
dgPretty(DefGroup dg)
{
	/* name: type */
	AbSyn	absyn;
	Syme	syme;
	String	name, type, result;


	/* Extract the syme for this definition */
	absyn = dgStmt(dg);
	syme = dgGetAbMeaning(absyn);


	/* Paranoia */
	assert(syme);


	/* Create the text "name:Type" */
	name = symString(symeId(syme));
	type = tfPretty(symeType(syme));
	result = strlConcat(name, ":", type, (String)NULL);


	/* Free the text associated with the type */
	strFree(type);


	/* Return the pretty text */
	return result;

}


/*
 * Try to find the meaning of a piece of absyn. The absyn
 * will probably be associated with a def-group and will
 * most likely be a constant definition. We want the syme
 * for the symbol being defined or NULL if not found.
 */
local Syme
dgGetAbMeaning(AbSyn absyn)
{
	AbSyn	id;

	/* Search for the identifier in this absyn */
	id = abDefineeIdOrElse(absyn, (AbSyn)NULL);


	/* Return the meaning of the absyn, if known */
	return id ? abSyme(id) : (Syme)NULL;
}


/*
 * Convert an import syme into the corresponding export
 * syme used as indices into the dependency graph.
 */
local Syme
dgSymeImportToExport(Syme syme)
{
	SymeList	symes;
	TForm		exporter, tf = symeType(syme);


	/* Get the exporter of this syme */
	exporter = symeExporter(syme);


	/* If can't find exporter, return untouched */
	if (!exporter) return syme;


	/* Get the constants defined in the exporter */
	symes = tfGetDomConstants(exporter);


	/* If no constants, return untouched */
	if (!symes) return syme;


	/* Search constants for this import */
	for (;symes;symes = cdr(symes)) {
		Syme	esyme = car(symes);


		/* Do the symbols match? */
		if (symeId(esyme) != symeId(syme)) continue;


		/* Check the types match */
		if (!tfEqual(symeType(esyme), tf)) continue;


		/* Return the matching syme */
		return esyme;
	}


	/* Not found: return untouched */
	return syme;
}


local SymeList
dgFindDependencies(AbSyn absyn)
{
	SymeList	result = listNil(Syme);

	/*
	 * Check the absyn: we want to find all dependencies
	 * but don't want to find false ones. For example, we
	 * need to examine the type of a declaration but we
	 * don't want to examine the thing being declared: it
	 * might be the node whose dependency is being checked!
	 */
	if (abIsId(absyn)) {
		Syme	syme = dgGetAbMeaning(absyn);

		if (syme) {
			/* Imports need to be converted to exports */
			if (symeIsImport(syme))
				syme = dgSymeImportToExport(syme);


			/*
			 * Add to the dependency list: at the moment
			 * we add all symes to the list. Ideally we
			 * ought to filter out those which aren't
			 * nodes of the dependency graph. They get
			 * filtered out later but if we remove them
			 * down here then there are fewer store
			 * operations via listCons/listNConcat.
			 */
			result = listCons(Syme)(syme, result);
		}
		return result;
	}
	else if (abTag(absyn) < AB_NODE_START)
		return result;


	/* Most cases are not very special */
	switch (abTag(absyn)) {
		/* Some nodes must not be fully checked */
		case AB_Add:
			result = dgFindDependencies(absyn->abAdd.base);
			break;

		case AB_With:
			result = dgFindDependencies(absyn->abWith.base);
			break;

		case AB_Declare:
			result = dgFindDependencies(absyn->abDeclare.type);
			break;

		case AB_Define:
			result = dgFindDependencies(absyn->abDefine.rhs);
			break;

		case AB_PLambda: /* Fall through */
		case AB_Lambda: {
			SymeList	plst, rlst;

			/* Dependencies in types */
			plst = dgFindDependencies(absyn->abLambda.param);
			rlst = dgFindDependencies(absyn->abLambda.rtype);

			result = listNConcat(Syme)(plst, rlst);
			break;
		}

		/* Check everything else completely */
		default: {
			int	i;

			for (i = 0; i < abArgc(absyn); i++) {
				SymeList	tmp;

				tmp = dgFindDependencies(abArgv(absyn)[i]);
				if (tmp)
					result = listNConcat(Syme)(result, tmp);
			}

			break;
		}
	}


	/* Return the list of dependencies */
	return result;
}


