/*****************************************************************************
 *
 * usedef.c: Usage definition chains
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
/***************************************************************************
 *
 * Use-Definition Chains
 *
 * This file provides procedures to build UD chains from a foam tree or a
 * flog. (Refer. to 'Aho, Sethi, Ullman: "Compilers: Principles, Techinques,
 * and Tools", Addison Wesley)
 * UD chains are built only for Locals and Parameters, because the majority
 * of current optimizations work only on this class of variables.
 * 
 *************************************************************************
 *
 * This implementation works in three phases.
 * 
 * 1) The foam tree is traversed and a varDefsVect is built. The i-th
 *    elem. of this vector is the list of all the foam definitions of
 *    (Loc i).
 *
 * 2) DataFlow analysis is performed on the base of the information stored
 *    in the varDefsVect.
 *
 * 3) The foam tree is traversed and, using the information in the IN sets
 *    for each block, the UD chain is built for each usage of Loc or Par.
 *    The list is attached to the corresponding usage. It can be accessed
 *    using the udReachingDefs(foam) macro.
 *
 *************************************************************************
 * NOTE: in date 23 May 1994 usedef has been modified so that for each use
 * 	is possible to know the reaching defs and the blocks to which these
 *	defs belong. This information will be used, in example, in loops optim.
 *************************************************************************
 *
 *    *** EXAMPLE:  (2 blocks)
 *
 *    -------------------------------- Block B3 ----------------
 *    (Set (Loc 0) (Glo 1))    -- s1
 *    (Set (Loc 1) (Loc 0))    -- s2
 *    (Set (Loc 0) (Lex 3))    -- s3
 *    (CCall (Loc 0))          -- s4
 *    (CCall (Loc 8))          -- s5
 *    ----------------------------------------------------------
 *      
 *    -------------------------------- Block B5 ----------------
 *    (Set (Loc 8) (Glo 12))    -- s10
 *    ----------------------------------------------------------
 *
 *    +++++++++++++++++++ After phase (1): ++++++++++++++++++++
 *
 *    VarDefsVect[0] = [s1, s3]
 *    VarDefsVect[1] = [s2]
 *    ...
 *    VarDefsVect[8] = [s10]
 *
 *    +++++++++++++++++++ After phase (2): ++++++++++++++++++++
 *
 *    Let's suppose that 
 *      IN[B3] = { s10 }
 *      IN[B5] = { }
 *
 *    +++++++++++++++++++ After phase (3): ++++++++++++++++++++
 *
 *    -------------------------------- Block B3 ---------------------
 *    (Set (Loc 0) (Glo 1))    -- s1    udReachingDefs(Loc 0) = [s1]
 *
 *    (Set (Loc 1) (Loc 0))    -- s2    udReachingDefs(Loc 1) = [s2]
                                        udReachingDefs(Loc 0) = [s1]
 *
 *    (Set (Loc 0) (Lex 3))    -- s3    udReachingDefs(Loc 0) = [s3]
 * 
 *    (CCall (Loc 0))          -- s4    udReachingDefs(Loc 0) = [s3]
 *    (CCall (Loc 8))          -- s5    udReachingDefs(Loc 0) = [s10]
 *    ----------------------------------------------------------------
 *    -------------------------------- Block B5 ----------------------
 *    (Set (Loc 8) (Glo 12))    -- s10  udReachingDefs(Loc 8) = [s10]
 *    ----------------------------------------------------------------
 *
 ***(end example)************************************************************
 *
 * 30 May 1994: usedef.c generate false definitions for parameters.
 *
 *   For each (Par n) there is a false definition (Set (Par N) (Par N)) as it
 *   was the first instruction.
 *   usedefChainsFree knows about that, and free these false pieces of foam.
 *
 * 1 June 1994: usedef.c fixed to handle (Set (Values ...) (MFmt ....))
 *  	assignments correctly.
 *
 * 2 June 1994: usedef.c has been extended also to lexicals.
 *   The inliner needs to trace lexicals that are assigned with a Def stmt.
 *
 *
 ****************************************************************************
 * ToDo:
 *	- keep information for each function of the lexicals and the globals
 *	modified. This will achive a lot of global optimizations that we
 *	don't perform.
 *
 ****************************************************************************
 * !!!!!FIXME!!!!!
 *  Usedef doesn't handle properly FOAM_Values
 *  usedefFreeChains
 ****************************************************************************/

#include "debug.h"
#include "dflow.h"
#include "flog.h"
#include "opttools.h"
#include "store.h"
#include "usedef.h"
#include "util.h"

/****************************************************************************
 * :: Debug
 ****************************************************************************/

Bool	udDfDebug	= false;
Bool	udDfiDebug	= false;

#define udDfDEBUG	DEBUG_IF(udDf)	afprintf
#define udDfiDEBUG	DEBUG_IF(udDfi)	afprintf

/*****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/

# define	udDefNo(foam)		((foam)->foamGen.hdr.info.defNo)

  /* Note: false defs have udInfo with block == NULL, for safety */
# define	udIsFalseDef(UDI)        				     \
    (foamTag(udInfoDef(UDI)->foamSet.lhs) == FOAM_Par &&		     \
     foamTag(udInfoDef(UDI)->foamSet.rhs) == FOAM_Par &&		     \
     udInfoDef(UDI)->foamSet.lhs->foamPar.index == udInfoDef(UDI)->foamSet.rhs->foamPar.index && \
     !udInfoBlock(UDI))

/****************************************************************************
 * :: Global Data Structures
 ****************************************************************************/


typedef struct {
	FoamList     * varDefsVect;
	int	     nPars;
	int	     nLocs;
	int	     nDefs;
	BitvClass    bitvClass;
	UdOutputKind outputType;
} UdProgInfo;

static UdProgInfo	udProgInfo;
#ifdef NEW_FORMATS
static Foam	udUnit;
#endif

CREATE_LIST(UdInfo);

/****************************************************************************
 *
 * :: Local prototypes
 *
 ****************************************************************************/

local void	udDestroyUses		(Foam);

local int	udVarDefsVectBuild	(FlowGraph);
local void	udVarDefsVectUpdate	(Foam);
local void	udVarDefsVectFree	(void);

extern void	udVarDefsVectPrint	(void);
extern void	udPrintDbFrFlog		(FlowGraph);

local void	udFillGenKill		(FlowGraph, BBlock);
local void	udKillAllButThis	(Bitv, Foam, int, Bool);
local void	udCreateUDLists		(FlowGraph);
local Foam	udFindUses		(Foam, Bitv, BBlock, Bool);

local UdInfo	udInfoNew		(Foam, BBlock);

static int	udFlogCutOff =  200; /* Max no of dataflow iterations */


/****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

#if 0
void
useDefChainsFrFoamProg(Foam foam)
{
	FlowGraph 	flog;
	Foam		prog, defs, def;
	int		i;

	assert(foamTag(foam) == FOAM_Prog);

#ifdef NEW_FORMATS
	udUnit = foam;
#endif

	if (foamArgc(foam->foamProg.locals) + 
#ifdef NEW_FORMATS
	    foamArgc(foamUnitParams(udUnit)->foamDDecl.argv[prog->foamProg.params]) == 0)
#else
	    foamArgc(foam->foamProg.params) == 0)
#endif
		return;

	flog = flogFrProg(foam, FLOG_UniqueExit);

	useDefChainsFrFlog(flog, UD_OUTPUT_SinglePointer);
		
	flogToProg(flog);
}

void
usedefChainsFreeFrProg(Foam foam)
{
	assert(foamTag(foam) == FOAM_Prog);

	/* used by udIsFalseDef */
	udProgInfo.nPars = foamDDeclArgc(foam->foamProg.params);

	udDestroyUses(foam);
}
#endif

/* Compute, for each use of local/parameter, the set of definitions reaching
 * it. This information is associated to the use, using the 
 * foamhdr.info.defList field.
 *
 * outputType must be either UD_OUTPUT_UdList or UD_OUTPUT_SinglePointer.
 */
Bool
usedefChainsFrFlog(FlowGraph flog, UdOutputKind outputType)
{
	int		i, k, nDefs;

	/* !!$$ assume flog coming from a Prog, because we need to know the
 	 * num of locals/params
	 * probably this info can be stored when the flog is built.
	 */
	assert(flog->prog);

	udProgInfo.outputType = outputType;

	nDefs = udVarDefsVectBuild(flog);

	if (nDefs == 0) return true;

	flogBitvClass(flog) = bitvClassCreate(nDefs);
	udProgInfo.bitvClass = flogBitvClass(flog);

	flogIter(flog, bb, {
		dflowNewBlockInfo(bb, nDefs, udFillGenKill);
	});

	i = dflowFwdIterate(flog, DFLOW_Union, udFlogCutOff, &k, NULL);

	if (DEBUG(udDf)) {
		fprintf(dbOut, i == 0 ? "Converged" : "Did not converge");
		fprintf(dbOut, " after %d iterations\n", k);
		flogPrint(dbOut, flog, true);
	}

	if (i != 0) return false;

	udCreateUDLists(flog);

	dflowFreeGraphInfo(flog);
	udVarDefsVectFree();

	return true;
}

void
usedefChainsFreeFrFlog(FlowGraph flog)
{
	/* used by udIsFalseDef */
	udProgInfo.nPars = foamDDeclArgc(flog->prog->foamProg.params);

	flogIter(flog, bb, {
		udDestroyUses(bb->code);
	});
}

void
udSetFlogCutOff(int n)
{
	udFlogCutOff = n;
}

/****************************************************************************
 *
 * :: Local functions
 *
 ****************************************************************************/

local void
udDestroyUses(Foam foam)
{
	foamIter(foam, arg, udDestroyUses(*arg));

	if (otIsLocalVar(foam) /*!! || otIsNonLocalVar(foam) */ &&
	    udReachingDefs(foam))
		udReachingDefs(foam) = listNil(UdInfo);
		/* listFreeDeeply(UdInfo)(udReachingDefs(foam), udInfoFree); !!*/

	return;
}

/* Assign a progressive number to each definition (Def or Set), and add
 * it in the entry corresponding to the lhs, so that for each var is possible
 * access to the list of stmts where is defined.
 */
local int
udVarDefsVectBuild(FlowGraph flog)
{
	Foam	seq = flog->prog->foamProg.body;
	Foam	stmt, lhs;
	int	i, nDefs;
	int	nLocs = foamDDeclArgc(flog->prog->foamProg.locals);
#ifdef NEW_FORMATS
	int	nPars = foamArgc(foamUnitParams(udUnit)->foamDDecl.argv[flog->prog->foamProg.params-1]);
#else
	int	nPars = foamDDeclArgc(flog->prog->foamProg.params);
#endif

	udProgInfo.nPars = nPars;
	udProgInfo.nLocs = nLocs;

	if (nLocs + nPars == 0) return 0;

	otProgInfoInit(OT_ASSOCIATION_LIST, nLocs, nPars, NULL);

	nDefs = nPars;   /* false definition for parameters */

	/* Generate false definitions for parameters */
	for (i = 0; i < nPars; i++) {
		stmt = foamNewSet(foamNewPar(i), foamNewPar(i));
		udDefNo(stmt) = i;
		udVarDefsVectUpdate(stmt);
	}

	flogIter(flog, bb, {

		seq = bb->code;

		for (i = 0; i < foamArgc(seq); i++) {
		   	stmt = foamArgv(seq)[i].code;

			if (!otIsDef(stmt)) continue;

			lhs = stmt->foamDef.lhs;

			if (otIsLocalVar(lhs) ||
			    foamTag(lhs) == FOAM_Values ||
			    (otIsNonLocalVar(lhs) &&
			     foamTag(stmt) == FOAM_Def)) {

			    udDefNo(stmt) = nDefs++;
			    udVarDefsVectUpdate(stmt);
		    }
		}
	});

	if (DEBUG(udDf)) {
		udVarDefsVectPrint();
	}

	return nDefs;
}

local void
udVarDefsVectUpdate(Foam def)
{
	Foam		lhs = def->foamDef.lhs;
	int		var, nVars;
	Foam		* lhsVarVect;

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
	        assert(nVars == 1 || otIsVar(lhs)); 

		otAddVarInfo(def, lhs);
	}
	
	udProgInfo.nDefs += 1;
}

local void
udVarDefsVectFree()
{
	otProgInfoFini();
}


void
udVarDefsVectPrint()
{
	otPrintVarAssociations((VarInfoPrintFn) foamPrintDb);
}

/****************************************************************************
 *
 * :: Build Gen and Kill sets
 *
 ****************************************************************************/

local void
udFillGenKill(FlowGraph flog, BBlock bb)
{
	Foam		seq = bb->code, lhs, stmt;
	int		defNo, i;
	BitvClass	class = udProgInfo.bitvClass;

	udDfiDEBUG(dbOut, "Filling Gen/Kill for %d\n", bb->label);

	/*
	 * Clear the vectors.
	 */
	bitvClearAll(class, dfFwdGen(bb));
	bitvClearAll(class, dfFwdKill(bb, int0));

	/* Generate false definitions for parameters */
	if (bb == flog->block0)
		for (i = 0; i < udProgInfo.nPars; i++)
			bitvSet(class, dfFwdGen(bb), i);

	for (i = 0; i < foamArgc(seq); i++) {
		stmt = seq->foamSeq.argv[i];

		if (!otIsDef(stmt)) continue;

		lhs = stmt->foamDef.lhs;

		if (!otIsLocalVar(lhs) &&
		    foamTag(lhs) != FOAM_Values &&
		    (!otIsNonLocalVar(lhs) ||
		     foamTag(stmt) != FOAM_Def)) {
			udKillAllButThis(dfFwdKill(bb, int0), lhs, -1, true);
			continue;
		}

		defNo = udDefNo(stmt);

		/* Set gen bit for the corresponding def */
		bitvSet(class, dfFwdGen(bb), defNo);

		/* Unkill, needed because we use (IN \/ Gen) - Kill */
		bitvClear(class, dfFwdKill(bb, int0), defNo); 


		udKillAllButThis(dfFwdKill(bb, int0), lhs, defNo, true);
	}
}

/* NOTE: "defNo" may be -1; this means that we want to kill all the definition
 * of the lhs.
 * This is generally used for multiple value assignments: we kill all and
 * nothing is generated
 */
local void
udKillAllButThis(Bitv killv, Foam lhs, int defNo, Bool bit)
{
	int		n;
	int		var, nVars;
	Foam		* lhsVarVect;
	FoamList	defs;
	Bool		found = false;
	BitvClass	class = udProgInfo.bitvClass;

	if (!otIsVar(lhs) && foamTag(lhs) != FOAM_Values) 
		return;

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
		assert(otIsVar(lhs));
		defs = (FoamList) otGetVarInfoList(lhs);

        	listIter(Foam, def, defs, {
			n = udDefNo(def);
			if (n != defNo) {
				if (bit)
					bitvSet(class, killv, n);
				else
					bitvClear(class, killv, n);
			}
			else {
				/* !! Used only for the safety test,
			 	 * could be removed
			 	 */
				found = true;
			}
		});

		assert(defNo == -1 || found);
	}
}

/****************************************************************************
 *
 * :: Foam traversal and creations of ud chains.
 *
 ****************************************************************************/

local void
udSetReachingList(Foam foam, FoamList defs, Bitv dfin, BBlock block)
{
	int 	defNo;
	UdInfo	udInfo;
	UdInfoList udList = listNil(UdInfo);

	listIter(Foam, def, defs, {
		defNo = udDefNo(def);

		/* Still def alive ? */
		if (bitvTest(udProgInfo.bitvClass, dfin, defNo)) {

			if (udDefNo(def) < udProgInfo.nPars)
				udInfo = udInfoNew(foamCopy(def),
						   (BBlock) int0);
			else
				udInfo = udInfoNew(def, block);

			listPush(UdInfo, udInfo, udList);
		}
	});

	/* Attach the udList to the used var */

	udReachingDefs(foam) = udList;
}

local Foam
udSetReachingDef(Foam foam, FoamList defs, Bitv dfin, Bool refContext)
{
	Foam 	uniqueDef = NULL;
	int	defNo;

	listIter(Foam, def, defs, {

		defNo = udDefNo(def);

		/* Still def alive ? */
		if (bitvTest(udProgInfo.bitvClass, dfin, defNo)) {

			if (udDefNo(def) < udProgInfo.nPars) {
				uniqueDef = NULL;
				break;
			}
			else if (!uniqueDef)
				uniqueDef = def;
			else {
				uniqueDef = NULL;
				break;
			}
		}
	});

	udReachingDefs(foam) = (UdInfoList) uniqueDef;

	/* Last condition ensure that (Set (Loc 0) (SInt 1)) is not
 	 * transformed
	 */
	if (uniqueDef && !refContext &&
	    otIsMovableData(uniqueDef->foamDef.rhs))
		return foamCopy(uniqueDef->foamDef.rhs);
	else
		return foam;
}


local void
udCreateUDLists(FlowGraph flog)
{
	flogIter(flog, bb, {

		bb->code = udFindUses(bb->code, dfFwdIn(bb), bb, false);
	});
}

local Foam
udFindUses(Foam foam, Bitv dfin, BBlock block, Bool refContext)
{
	Foam newFoam = foam;

	switch (foamTag(foam)) {
	case FOAM_Loc:
	case FOAM_Par:
	case FOAM_Lex:
	case FOAM_Glo:	{
		FoamList	defs;

		/* Look which definitions are alive at this point */

		defs = (FoamList) otGetVarInfoList(foam);
		
		switch (udProgInfo.outputType) {
		  case UD_OUTPUT_UdList:
			udSetReachingList(foam, defs, dfin, block);
			break;

		  case UD_OUTPUT_SinglePointer:
			newFoam = udSetReachingDef(foam, defs,
						   dfin,refContext);
			break;

		  default:
			bug("udFindUses: bad output type...");
		}
		break;
	}

	case FOAM_Set:
	case FOAM_Def: {
		int		defNo;
		Foam		lhs = foam->foamDef.lhs;
		BitvClass	class = udProgInfo.bitvClass;

		foam->foamDef.rhs = udFindUses(foam->foamDef.rhs,
					       dfin, block, false);

		/* E.g.: (Set Lex ...) or (Set (Values ... )(..)) */
		
		defNo = udDefNo(foam);
		
		if (otIsNonLocalVar(lhs) && foamTag(foam) == FOAM_Set) {
		    	udKillAllButThis(dfin, lhs, -1, false);
			return foam;
		}

		/* E.g.: (Set (EElt ..(Loc 4) ...) */
		if (!otIsVar(lhs) && foamTag(lhs) != FOAM_Values) {
			foam->foamDef.lhs = udFindUses(foam->foamDef.lhs,
					       dfin, block, false);
			return foam;
		}

		/* Cases: (Set/Def (Loc ..)), (Def (Lex/Glo)...) */

		/* Kill the other definitions of lhs */
		udKillAllButThis(dfin, lhs, defNo, false);

		/* Generate this */

		bitvSet(class, dfin, defNo);

		/* Analyse lhs. */
		foam->foamDef.lhs = udFindUses(foam->foamDef.lhs,
				       		dfin, block, true);

		return foam;
	}

	default:
		break;
	}

	foamIter(foam, arg, {
		*arg = udFindUses(*arg, dfin, block, refContext);
	});

	return newFoam;
}


/****************************************************************************
 *
 * :: Constructors and Destructors
 *
 ****************************************************************************/

local UdInfo
udInfoNew(Foam def, BBlock block)
{
	UdInfo udinfo = (UdInfo) stoAlloc(OB_Other, sizeof(*udinfo));

	udinfo->foam 	= def;
	udinfo->block 	= block;

	return udinfo;
}


#if 0  /* DON'T REMOVE ! */

local void
udInfoFree(UdInfo ud)
{
	if (!ud) return;

	/* if (udIsFalseDef(ud)) foamFree(udInfoDef(ud)); !!$$*/
	stoFree(ud);
}

#endif
