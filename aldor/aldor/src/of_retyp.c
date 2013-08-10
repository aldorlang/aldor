/*****************************************************************************
 *
 * of_retype.c: Foam Retyping And Cast Remotion.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "of_peep.h"
#include "of_retyp.h"
#include "of_util.h"
#include "optfoam.h"
#include "opttools.h"
#include "store.h"
#include "fbox.h"

/******************************************************************************
 *
 * :: Debug
 *
 *****************************************************************************/

Bool	retDebug	= false;

#define retDEBUG	if (DEBUG(ret))

/******************************************************************************
 *
 * :: Macros
 *
 *****************************************************************************/

#define RET_CANDIDATE_BIT		((AInt) 1 << 10)

#define retPromoteCanditateType(decl)

#define retCandidateType(t)    ((t) | RET_CANDIDATE_BIT)
#define retDefinedType(t)    ((t) & ~RET_CANDIDATE_BIT)

#define retSetUnknownDeclType(decl)   ((decl)->foamDecl.type = FOAM_NOp)

#define retSetCandidateDeclTypeTo(d, t)  		            \
            ((d)->foamDecl.type = (retDefinedType(t) == FOAM_Word ? \
	            retDefinedType(t) :retCandidateType(t)) ,	    \
	     retProgInfo.changed = true)

#define retSetDefinedDeclTypeTo(d, t)  \
            (((d)->foamDecl.type = retDefinedType(t)), d->foamDecl.format = emptyFormatSlot)


#define retIsUnknownType(type)	((type) == FOAM_NOp)
#define retIsCandidateType(type)  ((type) & RET_CANDIDATE_BIT)
#define retIsDefinedType(type)   (!retIsUnknownType(type) && \
				  !retIsCandidateType(type))

#define retCanBeRetyped(var)	(foamTag(var) == FOAM_Loc)

#define retTypeNeedsFmt(type) 	(type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR)

#define RET_ParRemappable	(Foam) 0x0
#define RET_ParRemap		(Foam) 0x1
#define RET_ParNoRemap		(Foam) 0x2

/******************************************************************************
 *
 * :: Global Data Structures
 *
 *****************************************************************************/

static struct RetProgInfo {
	Foam	prog;
	Foam	formats;
	Bool	changed;
	FoamTag progRetType;

	Foam    * locv;
	int	locc;

	Foam	* parMap;
	Foam    * parv;
	int	parc;
	int	parIndex;

	int	wtosCount;
	int	stowCount;

	int	wtosMax;
	int	stowMax;

	AIntList newSLocs;
	AIntList newWLocs;

	FoamList newSets;
	Bool	addedNewSets;
	VarPool newLocals;

} retProgInfo;

/******************************************************************************
 *
 * :: Local Prototypes
 *
 *****************************************************************************/

local void	retRetypeProg		(Foam prog);
local Bool	retDDeclInit		(Foam ddecl);
local void	retDDeclFini		(Foam ddecl);
local Foam	retRetypeExpr		(Foam foam, Bool isLhs);
local void	retRetypeDef		(Foam def);

local void	retRetypeBCall		(Foam foam);
local FoamTag	retGetNextType		(FoamTag type0, FoamTag type1);
local void	retPromoteTo		(Foam var, AInt type, AInt format);

local void	retRearrangeProg	(Foam prog);
local Foam	retRearrangeExpr	(Foam foam, Foam father);
local Foam	retWTOSDef		(Foam foam);
local Foam	retSTOWDef		(Foam foam);
local void	retRearrangeDef		(Foam def);

local Foam	retGetDecl		(Foam var);
local Foam	retInsertNewSets	(Foam stmt, FoamList newSets);
local void	retAddNewLocalDecls	(Foam prog);
local Foam	retPeepCasts		(Foam foam);

local void	retProgInit		(Foam prog);
local void	retProgFini		(void);

/******************************************************************************
 *
 * :: Main Entry Point
 *
 *****************************************************************************/

void
retypeUnit(Foam foam)
{
	int i;
	Foam def;

	assert(foamTag(foam) == FOAM_Unit);

	retProgInfo.formats = foam->foamUnit.formats;

	for (i = 0; i < foamArgc(foam->foamUnit.defs); i++) {
		def = foam->foamUnit.defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.lhs) != FOAM_Const) continue;
		if (foamTag(def->foamDef.rhs) != FOAM_Prog) continue;

		retRetypeProg(def->foamDef.rhs);
	}
}

/******************************************************************************
 *
 * :: Local Function Definitions
 *
 *****************************************************************************/

local void
retRetypeProg(Foam prog)
{
	assert(foamTag(prog) == FOAM_Prog);
	retDEBUG {
		fprintf(dbOut, "Retype..\n");
		foamPrintDb(prog);
	}
	if (!retDDeclInit(prog->foamProg.locals))
		return;

	retProgInit(prog);

	do {
		retProgInfo.changed = false;
		prog->foamProg.body = retRetypeExpr(prog->foamProg.body,false);

	} while (retProgInfo.changed);


	retDDeclFini(prog->foamProg.locals);

	retRearrangeProg(prog);

	prog = peepProg(prog,optIsFloatFoldWanted());


	retProgFini();

	return;
}

/******************************************************************************
 *
 * :: DDecl Init/Fini
 *
 *****************************************************************************/

/* Set all Word types in ddecl to be Unknown.
 * Return true if at least a Word type has been found.
 */
local Bool
retDDeclInit(Foam ddecl)
{
	int 	i;
	Foam 	decl;
	Bool	changed = false;

	assert(foamTag(ddecl) == FOAM_DDecl);

	for (i = 0; i < foamDDeclArgc(ddecl); i++) {
		decl = ddecl->foamDDecl.argv[i];
		if (decl->foamDecl.type == FOAM_Word) {
			changed = true;
			retSetUnknownDeclType(decl);
		}
	}

	return changed;
}

/* Set all vars that have a canditate type to be of that type
 */
local void
retDDeclFini(Foam ddecl)
{
	int i;
	Foam decl;

	assert(foamTag(ddecl) == FOAM_DDecl);

	for (i = 0; i < foamDDeclArgc(ddecl); i++) {
		decl = ddecl->foamDDecl.argv[i];
		if (retIsUnknownType(decl->foamDecl.type)) {
			decl->foamDecl.type   = FOAM_Word;
			decl->foamDecl.format = emptyFormatSlot;
		}
		else if (retIsCandidateType(decl->foamDecl.type))
			decl->foamDecl.type = 
				retDefinedType(decl->foamDecl.type);
	}
}

/******************************************************************************
 *
 * :: Retype Expr
 *
 *****************************************************************************/

local Foam
retRetypeExpr(Foam foam, Bool isLhs)
{
	Foam decl, newFoam = foam;

	if (foamTag(foam) == FOAM_Values && isLhs) return foam;

	if (otIsDef(foam)) {
		foam->foamDef.lhs = retRetypeExpr(foam->foamDef.lhs, true);
		foam->foamDef.rhs = retRetypeExpr(foam->foamDef.rhs, false);
	}
	else foamIter(foam, arg, *arg = retRetypeExpr(*arg, false); );

	if (foamTag(foam) == FOAM_BCall)
		retRetypeBCall(foam);
	
	else if (otIsDef(foam))
		retRetypeDef(foam);

	else if (!isLhs &&
		 retCanBeRetyped(foam)) {
		decl = retGetDecl(foam);

		if (!retIsDefinedType(decl->foamDecl.type))
			newFoam = foamNewCast(FOAM_Word, foam);
	}
		 
	

	return newFoam;
}

/******************************************************************************
 *
 * :: Retype Def
 *
 *****************************************************************************/

/* Retype a Def or Set lhs */
local void
retRetypeDef(Foam def)
{
	Foam 	lhs = def->foamDef.lhs;
	Foam 	rhs = def->foamDef.rhs;
	Foam	decl;
	AInt 	constrType;
	AInt	fmt;

	assert(otIsDef(def));

	/* Multiple assigm. -> don't change type */
	if (foamTag(lhs) == FOAM_Values) {
		Foam var;
		int  i;

		for (i = 0; i < foamArgc(lhs); i++) {
			var = lhs->foamValues.argv[i];
			if (!retCanBeRetyped(var)) continue;

			decl = retGetDecl(var);
			if (!retIsDefinedType(decl->foamDecl.type)) 
				retSetDefinedDeclTypeTo(decl, FOAM_Word);
		}
	}

	if (!retCanBeRetyped(lhs)) return;

	otDereferenceCast(rhs);
	
	fmt = emptyFormatSlot;
	constrType = foamExprType(rhs, retProgInfo.prog, retProgInfo.formats,
				  NULL, NULL, &fmt);

	if (constrType != FOAM_Word)
		retPromoteTo(lhs, constrType, fmt);
	else if (foamTag(def->foamDef.rhs) != FOAM_Cast) {
		/* Ex.: L0 := T[1], T1 array of Word */
		decl = retGetDecl(lhs);
		retSetDefinedDeclTypeTo(decl, FOAM_Word);
	}
}

/******************************************************************************
 *
 * :: Retype BCall
 *
 *****************************************************************************/

/* Look at the arguments. If the type of an arg must be T1 and there is
 * a cast from a Word to T1: 
 *    if the arg has a type unknown, set it to T1
 *    if has a candidateType != T1, set it to Word again.
 */
local void
retRetypeBCall(Foam foam)
{
	int 	i = 0;
	AInt	op;

	assert(foamTag(foam) == FOAM_BCall);
	op = foam->foamBCall.op;

	foamIter(foam, parg, {
		Foam    arg = *parg;
		AInt constrType;

		otDereferenceCast(arg);
		if (!retCanBeRetyped(arg)) continue;

		constrType = foamBValInfo(op).argTypes[i];

		/* There are no bcall operations on types with
		 * format information
		 */
		retPromoteTo(arg, constrType, emptyFormatSlot);

		i += 1;
	});
}

/******************************************************************************
 *
 * :: Retype Par
 *
 *****************************************************************************/
#if 0
/* Found  (Cast SFlo (Par )) or (Cast Word (Par )) */
local void
retRetypeCastPar(Foam foam, AInt toType)
{
	Foam decl;

	assert(foamTag(foam) == FOAM_Par);

	if (retProgInfo.parMap[foam->foamPar.index] != RET_ParRemappable)
		return;

	decl = retProgInfo.parv[foam->foamPar.index];

	if (decl->foamDecl.type == FOAM_Word &&
	    toType == FOAM_SFlo) {

	}
	else if (decl->foamDecl.type == FOAM_SFlo &&
	    toType == FOAM_Word) {

	}
}
#endif
/******************************************************************************
 *
 * :: Get Next Type
 *
 *****************************************************************************/

local FoamTag
retGetNextType(FoamTag type0, FoamTag type1)
{

	if (type0 == type1) return type0;

	if (type0 == FOAM_Arr || type0 == FOAM_Rec)
		return FOAM_Word;

	if (type0 == FOAM_BInt || type0 == FOAM_SFlo)
		return FOAM_Word;

	switch (type0) {
	case FOAM_Char:
	case FOAM_Byte:
	case FOAM_Bool:
		if (type1 == FOAM_HInt ||
		    type1 == FOAM_SInt ||
		    type1 == FOAM_SFlo)
			return type1;
		else
			return FOAM_Word;

	case FOAM_HInt:
		if (type1 == FOAM_SInt ||
		    type1 == FOAM_SFlo)
			return type1;
		else
			return FOAM_Word;

	case FOAM_SInt:
		if (type1 == FOAM_SFlo)
			return type1;
		else
			return FOAM_Word;

	default:
			return FOAM_Word;
	}
}

/******************************************************************************
 *
 * :: PromoteTo
 *
 *****************************************************************************/

/* Old      Constraint  Res
 * ---------------------------
 *  _		u	noth.`
 *  u		cT	cT   `
 *  u		T	cT   `
 *  cx		T	cT or W  `  
 *  cT		T	noth. `
 *  cx          cT      cT or W `
 *  T           *       noth. (or generate cast) `
 *  cT          cT      noth. `
 */
    

local void
retPromoteTo(Foam var, AInt type, AInt format)
{
	Foam decl = retGetDecl(var);
	FoamTag oldType = decl->foamDecl.type;

	assert(retCanBeRetyped(var));

	if (retIsDefinedType(oldType)) return;

	if (retIsUnknownType(type)) return;

	if (retTypeNeedsFmt(type) && format == emptyFormatSlot)
		return;

	if (retIsUnknownType(oldType)) {
		retSetCandidateDeclTypeTo(decl, type);
		decl->foamDecl.format = format;
	}
	else if (retCandidateType(oldType) ==
		 retCandidateType(type))
		return;

	else if (retDefinedType(oldType) == FOAM_SFlo &&
		 type == FOAM_Word)
		return;

	else if (retIsCandidateType(oldType)) {
		FoamTag newType = retGetNextType(oldType, type);
		retSetCandidateDeclTypeTo(decl, newType);
		decl->foamDecl.format = format;
	}

	else retSetDefinedDeclTypeTo(decl, FOAM_Word);

}
/******************************************************************************
 *
 * :: Rearrange
 *
 *****************************************************************************/

local void
retRearrangeProg(Foam prog)
{
	Foam 	stmt;
	Foam	seq = prog->foamProg.body;

	retProgInfo.wtosMax = 0;
	retProgInfo.stowMax = 0;
	retProgInfo.newSLocs = listNil(AInt);
	retProgInfo.newWLocs = listNil(AInt);
	retProgInfo.newLocals = vpNew(fboxNew(prog->foamProg.locals));

	foamIter(seq, arg, {

		stmt = *arg;

		retProgInfo.wtosCount = 0;
		retProgInfo.stowCount = 0;
		retProgInfo.newSets = listNil(Foam);

		stmt = retPeepCasts(stmt);
		retRearrangeExpr(stmt, stmt);

		*arg = retInsertNewSets(stmt, retProgInfo.newSets);
	});

	if (retProgInfo.addedNewSets) {
		retAddNewLocalDecls(prog);
		prog->foamProg.body = utilMakeFlatSeq(prog->foamProg.body);
	}
}


local Foam
retRearrangeExpr(Foam foam, Foam father)
{
	AInt	type;

	foamIter(foam, arg, *arg = retRearrangeExpr(*arg, foam); );

	if (otIsDef(foam)) 
		retRearrangeDef(foam);

	if (foam == father ||
	    otIsDef(father)) return foam;	/* Statement */


	if (foamTag(foam) == FOAM_Cast &&
	    foam->foamCast.type == FOAM_SFlo) {
		type = foamExprType(foam->foamCast.expr, retProgInfo.prog,
				    retProgInfo.formats, NULL, NULL, NULL);
		if (type == FOAM_Word)
			return retWTOSDef(foam);
	}

	if (foamTag(foam) == FOAM_Cast &&
	    foam->foamCast.type == FOAM_Word) {
		type = foamExprType(foam->foamCast.expr, retProgInfo.prog,
				    retProgInfo.formats, NULL, NULL, NULL);
		if (type == FOAM_SFlo)
			return retSTOWDef(foam);
	}

#if 0
	if (foamTag(father) == FOAM_Cast) return foam;

	if (retDefinedType(decl->foamDecl.type) == FOAM_SFlo)
		retProgInfo.stowCount += 1;

	foam = retSTOWDef(foam);

	newCast = foamNewCast(FOAM_Word, foam);

	return newCast;
#endif

	return foam;
}


/* Found exp(Cast SFlo w); transforming into:
 *    (Set (Ls) (Cast SF w)),  exp(Ls)
 * The new Loc is returned.
 */
local Foam
retWTOSDef(Foam foam)
{
	AInt 	newIndex = -1, i, newIndex0;
	Foam	newSet, newLoc;

	assert(listLength(AInt)(retProgInfo.newSLocs) == retProgInfo.wtosMax);

	if (retProgInfo.wtosCount == retProgInfo.wtosMax) {
		newIndex = retProgInfo.wtosMax + retProgInfo.stowMax +
			   retProgInfo.locc;
		listPush(AInt, newIndex, retProgInfo.newSLocs);
		retProgInfo.wtosMax += 1;

		newIndex0 = vpNewVar(retProgInfo.newLocals, FOAM_SFlo);

		assert(newIndex0 == newIndex);
	}
	else {
		i = retProgInfo.wtosMax - retProgInfo.wtosCount - 1;
		listIter(AInt, el, retProgInfo.newSLocs, {
			if (i == 0) {
				newIndex = el;
				break;
			}
			i -= 1;
		});
		assert(i == 0);
	}

	assert(newIndex >= retProgInfo.locc);

	retProgInfo.wtosCount += 1;

	newLoc = foamNewLoc(newIndex);
	
	/* foamCopy not needed for foam */
	newSet = foamNewSet(newLoc, foam);

	retDEBUG {
		fprintf(dbOut, "Generating:");
		foamPrintDb(newSet);
	}

	listPush(Foam, newSet, retProgInfo.newSets);
	       
	return foamCopy(newLoc);
}

/* Found exp(Cast Word sf); transforming into:
 *    (Set (Lw) (Cast W sf)),  exp(Lw)
 * The new Loc is returned.
 */
local Foam
retSTOWDef(Foam foam)
{
	AInt 	newIndex = -1, i, newIndex0;
	Foam	newSet, newLoc;

	assert(listLength(AInt)(retProgInfo.newWLocs) == retProgInfo.stowMax);

	if (retProgInfo.stowCount == retProgInfo.stowMax) {
		newIndex = retProgInfo.stowMax + retProgInfo.wtosMax +
			   retProgInfo.locc;
		listPush(AInt, newIndex, retProgInfo.newWLocs);
		retProgInfo.stowMax += 1;

		newIndex0 = vpNewVar(retProgInfo.newLocals, FOAM_Word);
		assert(newIndex0 == newIndex);
	}
	else {
		i = retProgInfo.stowMax - retProgInfo.stowCount - 1;
		listIter(AInt, el, retProgInfo.newWLocs, {
			if (i == 0) {
				newIndex = el;
				break;
			}
			i -= 1;
		});
		assert(i == 0);
	}

	assert(newIndex >= retProgInfo.locc);

	retProgInfo.stowCount += 1;

	newLoc = foamNewLoc(newIndex);
	
	/* foamCopy not needed for foam */
	newSet = foamNewSet(newLoc, foam);

	retDEBUG {
		fprintf(dbOut, "Generating:");
		foamPrintDb(newSet);
	}

	listPush(Foam, newSet, retProgInfo.newSets);

	return foamCopy(newLoc);
}

local void
retRearrangeDef(Foam def)
{
	Foam 	lhs = def->foamDef.lhs;
	Foam 	rhs = def->foamDef.rhs;
	Foam	decl;
	AInt 	lhsType, rhsType;

	assert(otIsDef(def));

	if (!retCanBeRetyped(lhs)) return;

	decl = retGetDecl(lhs);
	lhsType = decl->foamDecl.type;

	/* Note: Assumes at most a unique cast. retPeepCasts removed others */
	if (foamTag(rhs) == FOAM_Cast &&
	    lhsType != rhs->foamCast.type) {
		def->foamDef.rhs = rhs->foamCast.expr;
		foamFreeNode(rhs);
		rhs = def->foamDef.rhs;
	}

	rhsType = foamExprType(rhs, retProgInfo.prog, retProgInfo.formats,
				  retProgInfo.newLocals->fbox, NULL, NULL);


	if (lhsType == rhsType) return;

	otDereferenceCast(rhs);

	def->foamDef.rhs = foamNewCast(lhsType, rhs);
}

/******************************************************************************
 *
 * :: Utility
 *
 *****************************************************************************/

#if 0
/* Return true if the "foam" object has an address. Intuitively, every object
 * to which (in the corresponding C translation) '&' can be applied.
 */
local Bool
retHasAddress(Foam foam)
{
	FoamTag tag = foamTag(foam);

	if (tag == FOAM_Loc || tag == FOAM_Par ||
	    tag == FOAM_Lex || tag == FOAM_Glo) return true;

	if (tag == FOAM_AElt || tag == FOAM_RElt) return true;

	return false;
}
#endif

local Foam
retGetDecl(Foam var)
{
	assert(foamTag(var) == FOAM_Loc);

	return retProgInfo.locv[var->foamLoc.index];

}

local Foam
retInsertNewSets(Foam stmt, FoamList newSets)
{
	int	numStmts = listLength(Foam)(newSets) + 1;
	Foam newSeq = foamNewEmpty(FOAM_Seq, numStmts);
	int	i = 0;

	if (!newSets) return stmt;

	newSets = listNReverse(Foam)(newSets);

	listIter(Foam, stat, newSets, {
		newSeq->foamSeq.argv[i++] = stat;
	});	

	listFree(Foam)(newSets);

	newSeq->foamSeq.argv[i] = stmt;

	retProgInfo.addedNewSets = true;

	retDEBUG {
		fprintf(dbOut, "InsertNewSeq: ");
		foamPrintDb(newSeq);
	}

	return newSeq;
}

local void
retAddNewLocalDecls(Foam prog)
{
	
	assert(foamTag(prog) == FOAM_Prog);

	prog->foamProg.locals = fboxMake(retProgInfo.newLocals->fbox);
}

local Foam
retPeepCasts(Foam foam)
{
	Foam value;
	AInt type;

	foamIter(foam, arg, *arg = retPeepCasts(*arg););

	if (foamTag(foam) != FOAM_Cast) return foam;

	value = foam->foamCast.expr;

	/* Dereference Casts releasing unusefull nodes */
	while (foamTag(value) == FOAM_Cast) {
		Foam expr = value->foamCast.expr;
		foamFreeNode(value);
		value = expr;
	}

	if (foamTag(value) == FOAM_BCall)
		type =  foamBValInfo(value->foamBCall.op).retType;

	else if (foamTag(value) == FOAM_CCall)
	    	type = value->foamCCall.type;

	else
		type = FOAM_NOp;

	if (foam->foamCast.type == type) {
		foamFreeNode(foam);
		return value;
	}
	else {
		foam->foamCast.expr = value;
		return foam;
	}
}

/******************************************************************************
 *
 * :: Prog Init / Fini
 *
 *****************************************************************************/

local void
retProgInit(Foam prog)
{
	int i;

	retProgInfo.prog = prog;
	retProgInfo.progRetType = prog->foamProg.retType;
	retProgInfo.locv = prog->foamProg.locals->foamDDecl.argv;
	retProgInfo.locc = foamDDeclArgc(prog->foamProg.locals);
	retProgInfo.addedNewSets = false;

	retProgInfo.parv = prog->foamProg.params->foamDDecl.argv;
	retProgInfo.parc = foamDDeclArgc(prog->foamProg.params);
	retProgInfo.parIndex = 0;

	if (retProgInfo.parc)
		retProgInfo.parMap = (Foam *) 
			stoAlloc(OB_Other, sizeof(Foam) * retProgInfo.parc);
	else
		retProgInfo.parMap = NULL;

	for (i = 0; i < retProgInfo.parc; i++) {
		if (retProgInfo.parv[i]->foamDecl.type == FOAM_SFlo)
			retProgInfo.parMap[i] = RET_ParNoRemap;
		else
			retProgInfo.parMap[i] = RET_ParRemappable;
	}
}

local void
retProgFini()
{
	if (retProgInfo.parMap)
		stoFree(retProgInfo.parMap);

	retProgInfo.parMap = NULL;
}
