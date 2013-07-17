/*****************************************************************************
 *
 * of_cprop.c: Copy Propagation
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
/***************************************************************************
 *
 * Copy propagation optimization
 *
 * (Refer. to 'Aho, Sethi, Ullman: "Compilers: Principles, Techinques,
 * and Tools", Addison Wesley)
 *
 * The implementation of this optimization is really close to the
 * algorithm used for UD chains (see top of usedef.c file)
 * The main differences are:
 *
 * - VarCopiesVect[i] is the list of all the copies where (Loc i) is
 *   lhs or rhs (in usedef is only lhs)
 * - Sets Gen and Kill are build in a slightly different way.
 * - During phase (3), instead of attaching definitions on each usage,
 *   copy propagation is performed. Each usage of (Loc i) such that there
 *   is a unique copy in its reachingDefs list is substitued with the rhs
 *   of the copy.
 *
 * Implementation notes:
 *
 * - Once the dflow analysis has been performed, cpCopyPropagate is called.
 *   For each block, cpCopyPropagate0 is called on the corresponding foam
 *   tree. The algorithm try to substitute an usage skipping all the
 *   intermediate copies.
 *   Consider, in example, the following sequence:
 *   	(L1 := L0), (L2 := L1), (L3 := L2), foo(L3)
 *   In this case, the rightmost occurrence of L3 is initially replaced
 *   by L2; in the second step L2 is replaced by L1, finally with L0.
 *   Therefore foo(L3) will become foo(L0).
 *
 *************************************************************************
 *
 * DEFINITIONS
 *
 * For the current implementation:
 *	- a TMPVAR is a Loc or a Par.
 *	- a IMMEDIATE is SInt, Char, Bool, Byte, etc. (see cpIsImmediate)
 *	- a DEFinition is a Def or a Set stmt. of kind:
 *		(DEF TMPVAR expr)
 *			or 
 *		(DEF (Values ...) expr)
 *
 *	- CopyableCast is of the form (Cast type TMPVAR)
 *
 *	- a COPY is a stmt. of kind:
 *		(DEF TMPVAR TMPVAR) 
 *			or
 *		(DEF TMPVAR (Cast type TMPVAR))
 *			or
 *		(DEF TMPVAR IMMEDIATE)
 *			or
 *		(DEF TMPVAR (Cast type IMMEDIATE))
 *
 *
 *************************************************************************/

#include "debug.h"
#include "of_cprop.h"
#include "of_peep.h"
#include "optfoam.h"
#include "opttools.h"
#include "store.h"
/****************************************************************************
 * :: Macros
 ****************************************************************************/

# define cpIsTmpVar(var) (foamTag(var) == FOAM_Loc || foamTag(var) == FOAM_Par)

# define cpIsTmpVarOrValues(foam) (cpIsTmpVar(foam) || \
				   foamTag(foam)==FOAM_Values)

# define cpIsSpecialCast(foam) ((foam)->foamCast.type == FOAM_SFlo)

# define cpIsCopyableCast(foam) (foamTag(foam)==FOAM_Cast &&               \
				 !cpIsSpecialCast(foam) &&		   \
				 (cpIsTmpVar(foam->foamCast.expr)   ||     \
				 cpIsImmediate(foam->foamCast.expr)))

# define cpIsCopy(foam)		(cpIsDef(foam) && cpDefIsCopy(foam))


#define	  cpDF_CUTOFF  200	/* Max no dataflow iterations to converge. */

# define	cpDefNo(foam)		((foam)->foamGen.hdr.info.defNo)

   /* given a (Loc) or a (Par), assign to varno the index in the varDefsVect */
# define	cpGetVarNo(varno, foamVar)  \
                      if (foamTag(foamVar) == FOAM_Par) \
			  (varno) = (foamVar)->foamPar.index + cpInfo.nLocals;\
		      else \
	 	          (varno) = (foamVar)->foamLoc.index;

/****************************************************************************
 * :: Debug
 ****************************************************************************/

Bool      cpDfDebug 	  = false;
Bool      cpDebug 	  = false;

# define   cpDfDEBUG(s) DEBUG_IF(cpDfDebug,  s)
# define   cpDEBUG(s) 	DEBUG_IF(cpDebug, s)

/****************************************************************************
 * :: Global Data Structures
 ****************************************************************************/

typedef struct {
	FoamList	* varCopiesVect;
	int		nParams;
	int		nLocals;
	int		nCopies;

	int		nPropagated;

	BitvClass	bitvClass;
} CpInfo;

static CpInfo	cpInfo;
#ifdef NEW_FORMATS
static Foam	cpUnit;
#endif

static Bool   	cpFirstTime;

/* maybe a bitmap ? */
typedef enum { CP_Lhs, CP_Rhs, CP_CEnv } CpFlagState;

/****************************************************************************
 *
 * :: Local prototypes
 *
 ****************************************************************************/

local Foam	cpProg			(Foam);

local void	cpFlog0			(FlowGraph);

local void	cpVarCopiesVectBuild	(Foam);
local void	cpVarCopiesVectUpdate	(Foam);
local void	cpVarCopiesVectFree	(void);
extern void	cpVarCopiesVectPrint	(void);

local Bool	cpIsDef			(Foam);
local Bool	cpDefIsCopy		(Foam);
local Bool	cpIsImmediate		(Foam);
local Foam	cpRhsVarFrCopy		(Foam);

local void	cpFillGenKill		(FlowGraph, BBlock);
local void	cpSetKillCopies		(Bitv, Foam, int, Bool);

local void	cpCopyPropagate		(FlowGraph);
local Foam	cpCopyPropagate0	(Foam, Bitv, CpFlagState);

local void	cpProgInit		(Foam);
local void	cpProgFini		(void);

local void	acFoldAssignments	(FlowGraph);


/****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

void
cpropUnit(Foam foam, Bool isFirst)
{
	Foam		defs, def;
	int		i;

	assert(foamTag(foam) == FOAM_Unit);

	cpFirstTime = isFirst;

#ifdef NEW_FORMATS
	cpUnit = foam;
#endif
	defs = foam->foamUnit.defs;

	for (i = 0; i < foamArgc(defs); i++) {

		def = defs->foamDDef.argv[i];

		assert(foamTag(def) == FOAM_Def);
		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		 def->foamDef.rhs = cpProg(def->foamDef.rhs);

		cpDfDEBUG(if (cpInfo.nCopies) {
				fprintf(dbOut, "------ New prog: -----\n");
			     	foamPrintDb(def->foamDef.rhs);
		     });

		/* flogFree(flog); !! no flogFree, write it */
        }
}

/* Given a flow graph, perform copy propagation on it. 
 * NOTE: Assumes that flog comes from a Prog.
 */
Bool
cpFlog(FlowGraph flog)
{
	Bool result = false;

	Foam prog = flog->prog;

	assert(foamTag(prog) == FOAM_Prog);
	
	cpProgInit(prog);

	if (cpInfo.nLocals + cpInfo.nParams == 0) {
		cpProgFini();
		return false;
	}

	flogIter(flog, bb, {
		cpVarCopiesVectBuild(bb->code);
	});

	if (cpInfo.nCopies) {
		flogReuse(flog, FLOG_UniqueExit);
		cpFlog0(flog);
	}

	cpProgFini();

	result = cpInfo.nPropagated > 0;

	return result;
}

/****************************************************************************
 *
 * :: cpProg
 *
 ****************************************************************************/

local Foam
cpProg(Foam foam)
{
	FlowGraph	flog;
	Foam		newProg;

	assert(foamTag(foam) == FOAM_Prog);

	if (!optIsCPropPending(foam)) return foam;
	if (!cpFirstTime)
		optResetCPropPending(foam);
 
	cpProgInit(foam);

	if (cpInfo.nLocals + cpInfo.nParams == 0) {
		cpProgFini();
		return foam;
	}
	
	cpVarCopiesVectBuild(foam->foamProg.body);

	if (cpInfo.nCopies == 0)
		return foam;

	flog = flogFrProg(foam, FLOG_UniqueExit);

	cpFlog0(flog);

	newProg = flogToProg(flog);

	if (cpInfo.nPropagated > 0) {
		optSetPeepPending(newProg);
		optSetJFlowPending(newProg);
		optSetCsePending(newProg);
		/* newProg = peepProg(newProg); */
	}
	
	cpProgFini();

	return newProg;
}

/* 
 * Apply copy propagation to flog
 */
local void
cpFlog0(FlowGraph flog)
{
	BBlock	bb;
	int		i, k;

	if (false)
		acFoldAssignments(flog);

	flogBitvClass(flog) = bitvClassCreate(cpInfo.nCopies);
	cpInfo.bitvClass = flogBitvClass(flog);
	
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb || bb->dfinfo) continue;
		dflowNewBlockInfo(bb, cpInfo.nCopies, cpFillGenKill);
	}

	i = dflowFwdIterate(flog, DFLOW_Intersection, cpDF_CUTOFF, &k, NULL);

	cpDfDEBUG({
		fprintf(dbOut, i == 0 ? "Converged" : "Did not converge");
		fprintf(dbOut, " after %d iterations\n", k);
		flogPrint(dbOut, flog, true);
	});

	if (i != 0) return;

	cpCopyPropagate(flog);

	dflowFreeGraphInfo(flog);
}

/****************************************************************************
 *
 * :: Local function
 *
 ****************************************************************************/


/* Assign a progressive number to each copy, and add it to varCopiesVect 
 * in the entries corresponding to the lhs and the rhs.
 * This info will be used in the dflow analysis.
 */
local void
cpVarCopiesVectBuild(Foam seq)
{
	Foam	stmt;
	int	i;

	assert(foamTag(seq) == FOAM_Seq);

	for (i = 0; i < foamArgc(seq); i++) {
		stmt = foamArgv(seq)[i].code;

		if (cpIsCopy(stmt)) {
		    cpDefNo(stmt) = cpInfo.nCopies++;
		    cpVarCopiesVectUpdate(stmt);
	        }
	}

	cpDfDEBUG( cpVarCopiesVectPrint(); );

	return;
}

local void
cpVarCopiesVectUpdate(Foam def)
{
	Foam		lhs = def->foamDef.lhs;
	Foam	        rhs;
	int		varNo;
	
	assert(foamTag(lhs) == FOAM_Loc || foamTag(lhs) == FOAM_Par);

	cpGetVarNo(varNo, lhs);

	assert(varNo < cpInfo.nLocals + cpInfo.nParams);

	cpInfo.varCopiesVect[varNo] = listCons(Foam)
		(foamCopy(def), cpInfo.varCopiesVect[varNo]);

	cpInfo.nCopies += 1;

	rhs = cpRhsVarFrCopy(def);

	if (!rhs)
		return;

	cpGetVarNo(varNo, rhs);

	assert(varNo < cpInfo.nLocals + cpInfo.nParams);

	cpInfo.varCopiesVect[varNo] = listCons(Foam)
		(foamCopy(def), cpInfo.varCopiesVect[varNo]);
}

local void
cpVarCopiesVectFree()
{
	int		i, totVars = cpInfo.nLocals + cpInfo.nParams;

	if (!cpInfo.varCopiesVect) 	return;

	for (i = 0; i < totVars; i++)
		listFreeDeeply(Foam)(cpInfo.varCopiesVect[i], foamFree);

	stoFree(cpInfo.varCopiesVect);
}

void
cpVarCopiesVectPrint()
{
	FoamList	fl;
	int		nLoc = cpInfo.nLocals;
	int		nPar = cpInfo.nParams;
	int		i;

	fprintf(dbOut, "**** varCopiesVect ****\n");

	for (i = 0; i < nLoc; i++) {
		if (!cpInfo.varCopiesVect[i]) continue;
		fprintf(dbOut, "Loc %d:", i);
		for (fl = cpInfo.varCopiesVect[i]; fl ; fl = cdr(fl))
			foamPrintDb(car(fl));
	}
	for (i = 0; i < nPar; i++) {
		if (!cpInfo.varCopiesVect[i + nLoc]) continue;
		fprintf(dbOut, "Par %d:", i);
		for (fl = cpInfo.varCopiesVect[i + nLoc]; fl ; fl = cdr(fl))
			foamPrintDb(car(fl));
	}
}

/****************************************************************************
 *
 * :: Utility
 *
 ****************************************************************************/

local Bool
cpIsDef(Foam foam)
{
	Foam	lhs;

	if (foamTag(foam) != FOAM_Def &&
    	    foamTag(foam) != FOAM_Set)
		return false;

	lhs = foam->foamDef.lhs;

	if (!cpIsTmpVarOrValues(lhs))
		return false;

	return true;
}

local Bool
cpDefIsCopy(Foam foam)
{
	Foam   rhs;

	assert(foamTag(foam) == FOAM_Def || foamTag(foam) == FOAM_Set);

	if (foamTag(foam->foamDef.lhs) == FOAM_Values)
		return false;

	rhs = foam->foamDef.rhs;

	if (foamTag(rhs) != FOAM_Loc &&
    	    foamTag(rhs) != FOAM_Par &&
#if 1
	    foamTag(rhs) != FOAM_Clos &&
#endif
	    !cpIsImmediate(rhs) &&
	    !cpIsCopyableCast(rhs))
		return false;

	return true;
}

/* FOAM is generally the rhs of a definition.
 * Returns true if FOAM is an immediate data or a cast of it
 *
 * Note: DFlo, BInt, Rec, Arr, Prog and Clos are not immediate data.
 *       Roughly speaking, an immediate data is a data such that a foam expr
 *	 like:
 * 		(Seq (Set Loc0 DATA) (use Loc0) (use Loc0)) 
 *	 is semantically equivalent to:
 *		(Seq (use DATA) (use DATA)) 
 * Note: RRFmt is considered immediate, as are Type* and SizeOf* bcalls.
 */
local Bool
cpIsImmediate(Foam foam)
{
	Bool	result = false;

	while (foamTag(foam) == FOAM_Cast)
		foam = foam->foamCast.expr;

	if (foamTag(foam) == FOAM_BInt) {
		result = bintIsZero(foam->foamBInt.BIntData) ||
			bintEQ(foam->foamBInt.BIntData, bint1);
	}
	else if (foamTag(foam) == FOAM_BCall) {
		/*
		 * We want to fold RawRepSize and Type* calls
		 * into RRFmt instructions (their primary use).
		 * This allows RRFmt values to be examined in
		 * detail: for example, RRFmts whose arguments
		 * do not refer to locs, lex or globs ought to
		 * be lifted to the start of progs and units.
		 * (see of_rrfmt for more details).
		 *
		 * Note that SizeOf* and Type* calls correspond to
		 * integer values which can be computed cheaply.
		 * Also, while RawRepSize values must be computed
		 * at runtime, they are constant and relatively
		 * cheap to compute.
		 */
		FoamBValTag	tag = foam->foamBCall.op;

		switch (tag) {
		   case FOAM_BVal_RawRepSize:	result = true;break;
#if 0
		   case FOAM_BVal_SizeOfInt8:	/* Fall through */
		   case FOAM_BVal_SizeOfInt16:	/* Fall through */
		   case FOAM_BVal_SizeOfInt32:	/* Fall through */
		   case FOAM_BVal_SizeOfInt64:	/* Fall through */
		   case FOAM_BVal_SizeOfInt128:	/* Fall through */
#endif
		   case FOAM_BVal_SizeOfNil:	/* Fall through */
		   case FOAM_BVal_SizeOfChar:	/* Fall through */
		   case FOAM_BVal_SizeOfBool:	/* Fall through */
		   case FOAM_BVal_SizeOfByte:	/* Fall through */
		   case FOAM_BVal_SizeOfHInt:	/* Fall through */
		   case FOAM_BVal_SizeOfSInt:	/* Fall through */
		   case FOAM_BVal_SizeOfBInt:	/* Fall through */
		   case FOAM_BVal_SizeOfSFlo:	/* Fall through */
		   case FOAM_BVal_SizeOfDFlo:	/* Fall through */
		   case FOAM_BVal_SizeOfWord:	/* Fall through */
		   case FOAM_BVal_SizeOfClos:	/* Fall through */
		   case FOAM_BVal_SizeOfPtr:	/* Fall through */
		   case FOAM_BVal_SizeOfRec:	/* Fall through */
		   case FOAM_BVal_SizeOfArr:	/* Fall through */
		   case FOAM_BVal_SizeOfTR:	result = true; break;

#if 0
		   case FOAM_BVal_TypeInt8:	/* Fall through */
		   case FOAM_BVal_TypeInt16:	/* Fall through */
		   case FOAM_BVal_TypeInt32:	/* Fall through */
		   case FOAM_BVal_TypeInt64:	/* Fall through */
		   case FOAM_BVal_TypeInt128:	/* Fall through */
#endif
		   case FOAM_BVal_TypeNil:	/* Fall through */
		   case FOAM_BVal_TypeChar:	/* Fall through */
		   case FOAM_BVal_TypeBool:	/* Fall through */
		   case FOAM_BVal_TypeByte:	/* Fall through */
		   case FOAM_BVal_TypeHInt:	/* Fall through */
		   case FOAM_BVal_TypeSInt:	/* Fall through */
		   case FOAM_BVal_TypeBInt:	/* Fall through */
		   case FOAM_BVal_TypeSFlo:	/* Fall through */
		   case FOAM_BVal_TypeDFlo:	/* Fall through */
		   case FOAM_BVal_TypeWord:	/* Fall through */
		   case FOAM_BVal_TypeClos:	/* Fall through */
		   case FOAM_BVal_TypePtr:	/* Fall through */
		   case FOAM_BVal_TypeRec:	/* Fall through */
		   case FOAM_BVal_TypeArr:	/* Fall through */
		   case FOAM_BVal_TypeTR:	result = true; break;
		   default:			result = false; break;
		}
	}
	else {
		result = (foamTag(foam) == FOAM_SInt ||
			  foamTag(foam) == FOAM_Char ||
			  foamTag(foam) == FOAM_Bool ||
			  foamTag(foam) == FOAM_Byte ||
			  foamTag(foam) == FOAM_HInt ||
			  foamTag(foam) == FOAM_SFlo ||
			  foamTag(foam) == FOAM_Nil ||
			  foamTag(foam) == FOAM_RRFmt ||
			  foamTag(foam) == FOAM_Env);
	}

	return result;
}

/* Given a copy, returns NULL if the rhs is not a (casted) tmpvar,
 * the tmpvar elsewhere.
 */
local Foam
cpRhsVarFrCopy(Foam copy)
{
	Foam rhs;

	if (cpIsTmpVar(copy))
		return copy->foamDef.rhs;

	rhs = copy->foamDef.rhs;

	if (foamTag(rhs) == FOAM_Cast)
		rhs = rhs->foamCast.expr;

	if (cpIsTmpVar(rhs))
		return rhs;

	return NULL;
}
/****************************************************************************
 *
 * :: Build Gen and Kill sets
 *
 ****************************************************************************/

local void
cpFillGenKill(FlowGraph flog, BBlock bb)
{
	Foam		seq = bb->code, stmt;
	int		defNo, i;
	BitvClass	class = cpInfo.bitvClass;

	assert(class == bbBitvClass(bb));

	/*
	 * Clear the vectors.
	 */
	bitvClearAll(class, dfFwdGen(bb));
	bitvClearAll(class, dfFwdKill(bb, int0));

	for (i = 0; i < foamArgc(seq); i++) {
		stmt = foamArgv(seq)[i].code;

                if (!cpIsDef(stmt))
                        continue;
  
                defNo = cpDefNo(stmt);
  
                cpSetKillCopies(dfFwdKill(bb, int0), stmt, defNo, true);
  
		if (cpDefIsCopy(stmt)) {
			/* Set gen bit for the corresponding def */
			bitvSet(class, dfFwdGen(bb), defNo);
			/* Unkill, needed because we use (IN \/ Gen) - Kill */
			bitvClear(class, dfFwdKill(bb, int0), defNo);
		}
		
	}
}

local void
cpSetKillCopies(Bitv statev, Foam def, int defNo, Bool bit)
{
	int		varNo, n, var, nVars;
	FoamList	defs;
	Foam		lhs = def->foamDef.lhs;
	Foam		* lhsVarVect;
	BitvClass	class = cpInfo.bitvClass;

	assert(cpIsTmpVarOrValues(lhs));

	if (foamTag(lhs) == FOAM_Values) {
		nVars = foamArgc(lhs);
		lhsVarVect = lhs->foamValues.argv;
	}
	else {
		nVars = 1;
		lhsVarVect = &lhs;
	}

	for (var = 0; var < nVars; var++) {

		lhs = lhsVarVect[var];

		if (!cpIsTmpVar(lhs))
			continue;

		cpGetVarNo(varNo, lhs);

		defs = cpInfo.varCopiesVect[varNo];

		for ( ; defs ; defs = cdr(defs)) {
			n = cpDefNo(car(defs));
			if (bit) 
				bitvSet(class, statev, n);
			else
				bitvClear(class, statev, n);
		}
	}

	return; 
}

/****************************************************************************
 *
 * :: Foam traversal and copy propagation
 *
 ****************************************************************************/

local void
cpCopyPropagate(FlowGraph flog)
{
	BBlock	bb;
	int	i;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;
		if (bbEntryC(bb) == 0 && !bb->isblock0) continue;

		bb->code = cpCopyPropagate0(bb->code, dfFwdIn(bb), CP_Rhs);
		
		cpDfDEBUG(foamPrintDb(bb->code));
	}
}

local Foam
cpCopyPropagate0(Foam foam, Bitv dfin, CpFlagState cpState)
{
	Foam 	newFoam = foam;
	Bool    isDef = false;
	
	if (otIsDef(foam))   {
	        Foam lhs, rhs;
		isDef = cpDefIsCopy(foam);
		
		foam->foamDef.rhs = cpCopyPropagate0(foam->foamDef.rhs, dfin,
						     CP_Rhs);
		foam->foamDef.lhs = cpCopyPropagate0(foam->foamDef.lhs, dfin,
						     CP_Lhs);

		lhs = foam->foamDef.lhs;
		rhs = foam->foamDef.rhs;

		/* Remove (Set (Loc n) (Loc n)) that might appear 
		 * after a propagation. */

		if (foamTag(lhs) == foamTag(rhs) &&
		    cpIsTmpVar(lhs) &&
		    lhs->foamLoc.index == rhs->foamLoc.index) {

		        foamFree(foam);
			return foamNewNOp();
	        }
	}
	else if (foamTag(foam) == FOAM_CEnv) {
		Foam clos = cpCopyPropagate0(foam->foamCEnv.env, dfin, CP_CEnv);
		/* Should check that clos->env is a constant */
		if (foamTag(clos) == FOAM_Clos)
			return clos->foamClos.env;
		return foam;
	}
	else if (foamTag(foam) == FOAM_Values && cpState == CP_Lhs)
		return foam;
	else foamIter(foam, arg, *arg = cpCopyPropagate0(*arg, dfin, CP_Rhs); );


	switch (foamTag(foam)) {
	case FOAM_Loc:
	case FOAM_Par: {
		int 		defNo, varNo, index;
		FoamList	defs;
		Foam		def, copy, lhs, rhsVar;
		Bool		unique, foundLoop = false;
		AInt		castType = FOAM_Nil;

		if (cpState == CP_Lhs) break;

		while (!foundLoop && foam) {
			copy = NULL;

			cpGetVarNo(varNo, foam);
			index = foam->foamLoc.index;

			/* Look which copies are alive at this point */
			defs = cpInfo.varCopiesVect[varNo];

			for (unique = true; defs && unique; defs = cdr(defs)) {
				def = car(defs);
				defNo = cpDefNo(def);
				lhs = def->foamDef.lhs;
				
				/* Still alive ? */
				if (!bitvTest(cpInfo.bitvClass, dfin, defNo))
					continue;
				
				/* Is the same lhs ? */
				if (index == lhs->foamLoc.index &&
				    foamTag(lhs) == foamTag(foam)) {
					if (copy) unique = false;
					copy = def;
				}
			}
			
			/* Found a unique copy ? */
			if (!copy || !unique) break;

			rhsVar = cpRhsVarFrCopy(copy);
			if (cpState != CP_CEnv && rhsVar && foamTag(rhsVar) == FOAM_Clos)
				break;

			if (cpState != CP_CEnv && foamTag(copy->foamDef.rhs) == FOAM_Clos)
				break;

			if (foamTag(copy->foamDef.rhs) == FOAM_Cast &&
			    castType == FOAM_Nil)
			        castType = copy->foamDef.rhs->foamCast.type;

			/* It's a loop?  (Set (Loc 0) (Loc 0)) is a loop. */
			if (rhsVar && index == rhsVar->foamLoc.index &&
			    foamTag(foam) == foamTag(rhsVar))
				foundLoop = true;

			/* Is the unique copy/definition reaching this point.
			 * Substitution can be performed.
			 */

			/* foamFree(foam); */

			newFoam = foamCopy(copy->foamDef.rhs);
			cpInfo.nPropagated += 1;

			cpDEBUG(fprintf(dbOut, "CPROP>> Copy propagated ---\n"););

			foam = rhsVar; /* Will be NULL if rhs is IMMEDIATE */
		}


		/* "castType" is used to handle situations like:
		 *    l6 -> (Cast Word l5)
		 *    l5 -> (Cast SInt l3)
		 * In this case l6 should be substitued by (Cast Word l3).
		 * "castType" keeps trace of the first cast type.
		 */

		if (castType != FOAM_Nil)
	                newFoam = foamNewCast(castType, newFoam);
		

		break;
	}

	case FOAM_Set:
	case FOAM_Def: {
		Foam		lhs = foam->foamDef.lhs;
		int 		defNo;

		/* Update the IN vector */

		if (!cpIsTmpVarOrValues(lhs))
			break;

		defNo = cpDefNo(foam);
		cpSetKillCopies(dfin, foam, defNo, false);

		/* 
		 * !!NB: cpDefIsCopy can return garbage if propogation
		 * has happened
		 */
		if (isDef)
			/* Set bit for the corresponding def */
			bitvSet(cpInfo.bitvClass, dfin, defNo);
		
		break;
	}

	default:
		break;
	}

	return newFoam;
}


/****************************************************************************
 *
 * :: cpProgInit and cpProgFini
 *
 ****************************************************************************/

local void
cpProgInit(Foam prog)
{
	int	nLoc = foamDDeclArgc(prog->foamProg.locals);
#ifdef NEW_FORMATS
	int	nPar = foamDDeclArgc(foamUnitParams(cpUnit)->foamDDecl.argv[flog->prog->foamProg.params-1]);
#else
	int	nPar = foamDDeclArgc(prog->foamProg.params);
#endif
	int 	i;

	assert(foamTag(prog) == FOAM_Prog);

	cpInfo.nLocals 	= nLoc;
	cpInfo.nParams 	= nPar;
	cpInfo.nCopies 	= 0;
	cpInfo.nPropagated = 0;
	cpInfo.varCopiesVect = (FoamList *) 0;

	if (nLoc + nPar == 0)
		return;

	cpInfo.varCopiesVect =
	  (FoamList *) stoAlloc(OB_Other, (nLoc + nPar) * sizeof(FoamList));

	for (i = 0; i < nLoc + nPar; i++)
		cpInfo.varCopiesVect[i] = listNil(Foam);


}

local void
cpProgFini()
{
	cpVarCopiesVectFree();
	return;
}

/****************************************************************************
 *
 * :: acFlog --- Collect assignments into BB
 *
 ****************************************************************************/

static BitvClass acBitClass;
static Bitv	 acMarkBits;
local Bool	acIsCandidate		(BBlock);
local BBlock	acMergeWithSuccessor	(FlowGraph, int);
local void	acFoldBlock(FlowGraph flog, int label);

local void
acFoldAssignments(FlowGraph flog)
{
	Bitv   candidates;
	BBlock bb;
	int    i, sz;
	sz = flogBlockC(flog);
	acBitClass = bitvClassCreate(sz);
	candidates = bitvNew(acBitClass);
	acMarkBits = bitvNew(acBitClass);
	bitvClearAll(acBitClass, candidates);
	bitvClearAll(acBitClass, acMarkBits);

	for (i=0; i<sz; i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;
		if (acIsCandidate(bb))
			bitvSet(acBitClass, candidates, i);
	}

	acFoldBlock(flog, flog->block0->label);
	bitvClassDestroy(acBitClass);
}

local void
acFoldBlock(FlowGraph flog, int label)
{
	BBlock bb = flogBlock(flog, label);
	int    i;
	
	/* Depth first, post-order ensures that the
	 * target blocks should already be merged
	 * before the parent looks at them
	 */
	if (!bb) return;
	if (label >= bitvClassSize(acBitClass))
		return;
	if (bitvTest(acBitClass, acMarkBits, label))
		return;
	    
	bitvSet(acBitClass, acMarkBits, label);

	for (i=0; i<bbExitC(bb); i++) 
		acFoldBlock(flog, bbExit(bb, i)->label);
	
	if (bbExitC(bb) != 1)		    
		return;

	if (acIsCandidate(bbExit(bb,int0)))
		bb = acMergeWithSuccessor(flog, label);
}

local Bool
acIsCandidate(BBlock bb)
{
	Foam seq = bb->code;
	int i;
	
	if (bbExitC(bb) != 1)
		return false;
	for (i=0; i<foamArgc(seq) - 1; i++) {
		Foam foam = seq->foamSeq.argv[i];
		
		if (!otIsDef(foam)) return false;
		if (!otIsLocalVar(foam->foamSet.lhs))
			return false;
		foam = foam->foamSet.rhs;
		otDereferenceCast(foam);
		if (!otIsLocalVar(foam))
			return false;
	} 

	return true;
}

local BBlock
acMergeWithSuccessor(FlowGraph flog, int label)
{
	BBlock thisBB, nextBB, newBB;
	Foam new;
	int  i, j;
	
	thisBB = flogBlock(flog, label);
	nextBB = bbExit(thisBB, int0);
	if (thisBB == nextBB)
		return thisBB;
	
	new = foamNew(FOAM_Seq, foamArgc(thisBB->code) + foamArgc(nextBB->code) - 1);
	for (i=0; i<foamArgc(thisBB->code) - 1 ; i++)
		new->foamSeq.argv[i] = foamCopy(thisBB->code->foamSeq.argv[i]);
	for (j = 0; j<foamArgc(nextBB->code); i++, j++) 
		new->foamSeq.argv[i] = foamCopy(nextBB->code->foamSeq.argv[j]);
	
	i = flogReserveLabel(flog);
	newBB = bbNew(new, i);
	flogSetBlock(flog, i, newBB);
	bbufNeed(newBB->exits, 1);
	bbSetExitC(newBB, 1);
	bbSetExit(newBB, int0, bbExit(nextBB, int0));
#if 0	
	fprintf(dbOut, "merging: %d with %d -> %d\n", label, nextBB->label, newBB->label);
	foamPrintDb(thisBB->code);
	foamPrintDb(nextBB->code);
#endif
	if (flog->block0 == thisBB) {
		flog->block0 = newBB;
		newBB->isblock0 = true;
		thisBB->isblock0 = false;
	}
	for (i=0; i<flogBlockC(flog); i++) {
		BBlock bb = flogBlock(flog, i);
		if (!bb) continue;
		for (j=0; j<bbExitC(bb); j++) {
			if (bbExit(bb, j) == thisBB)
				bbSetExit(bb, j, newBB);
		}
	}
	flogKillBlock(flog, label);

	return newBB;
}


