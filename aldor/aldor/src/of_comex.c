/*****************************************************************************
 *
 * of_comex.c: Common Subexpressions Elimination
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Common Subexpression Elimination
 *
 * (Refer. [1] 'Aho, Sethi, Ullman: "Compilers: Principles, Techinques,
 * and Tools", Addison Wesley)
 *
 * The current implementation is a variant of the algorithm proposed in [1].
 * The main difference is that, once a subexpression is found, the flow graph
 * is NOT traversed backward from the block of the subexpression.
 * This is achieved assigning a different index to all the subexpr that are
 * equal but not eq, and grouping the equals.
 *
 ****************************************************************************
 *
 * THE ALGORITHM:
 *
 * The algorithm works in 3 main steps:
 *
 *   1. The foam tree is traversed and an unique index is assigned to each
 *      subexpr (mainly BCalls). During this step a table is built that,
 *	given a subexpr, return the set of all equal subexpr.
 *      NOTE: With `equal' I mean `with the same structure', not the same
 *	      address.
 *   2. Dataflow analysis for available expressions is performed, as in [1].
 *   3. The foam tree is traversed and, for each subexpr E, the table and
 *      bbIn are used to get the set {E1..En} of common subexprs available.
 *	The following transformation is performed for i in 1..n:
 *
 *		(....Ei....) 	->	 (Set tmp Ei) (....tmp....)
 *	and, finally:
 *		(....E.....)	->	(....tmp....)
 *
 * NOTE: to contain the number of new generated locals, the new local is
 *	generated only if needed.
 *
 * NOTE: The algorithm described works also locally, in the sense that:
 *   		(Set L0 (+ (* t1 t1) (* t1 t1)))
 *       is transformed in:
 *		(Set L1 (* t1 t1))
 *   		(Set L0 (+ L1 L1))
 ****************************************************************************
 * IMPLEMENTATION DETAILS:
 *
 * ++ THE PHANTOM BIT ++
 * This algorithm makes use of a technique that we call `phantom bit'.
 * When we perform dflow analysis for available expressions we use a special
 * bit for each set of equal expressions to detect the existence of a path
 * in the flowgraph, starting from the first block, such that the expression
 * cannot be found on that path. Example: suppose that in a point P of the
 * program there is an expression E that we are trying to replace with a
 * local. We look at the dflow bits and we discover that E is reached by
 * E' and by E". But before we eliminate E we look at the phantom bit for that
 * expression: if it's on then there is a path that does not touch E' or E",
 * so E _cannot_ be eliminated.
 *
 *
 * ++ Dynamic Formats (RRFmt) ++
 * Dynamic formats represented by RRFmt are strange beasts: they represent
 * constants whose value must be computed without side-effects. It is not
 * possible to ensure that the expressions appearing in an RRFmt are free
 * from side-effects so the user will simply have to accept that their code
 * may behave differently according to the platform and the optimisations
 * which have been selected.
 *
 * The problem is that we have two conflicting interests: RRFmts whose
 * value can be determined statically (i.e. it only contains literals and
 * BCalls with BCall or literal arguments) and those whose value cannot.
 *
 * Static RRFmts need to be cprop'd into each RRNew and RRElt that uses
 * them so that other optimisations can determine the meaning of each
 * raw record use. It also allows genc to replace each static RRFmt with
 * a reference to a global that gets initialised once per module. Thus
 * we must not CSE these RRFmts.
 *
 * On the other hand, non-static RRFmts are extremely expensive to compute
 * so we do not want to cprop them and instead want to CSE them. We are
 * unlikely to be able to do much with these RRFmts except to try and reduce
 * the number of times that they are computed.
 * 
 ****************************************************************************
 * DEFINITIONS:
 *	- LITERAL datas are the ones that aren't represented with pointers
 *	   Ex: SInt, Char, Bool, Byte, are Literals,
 *	       Rec, Env, Prog aren't Literals
 *	- a subexpression is PURE if all the leafs are LITERAL, Locs or Pars.
 *     	- a subexpression E is VALID if has no side effects and:
 *		- is PURE
 *			or
 *		- all the other subexpression in the some statement have no
 *		  side effects.
 *
 *
 * NOTE: CSE is performed only on valid expressions.
 *
 *      The meaning of the definition for 'VALID' is the following.
 * 	Let's consider the statement:
 *		(+ (+ (Lex 2 1) 1) (CCall foo ...))
 *
 *	The subexpr (+ (Lex 2 1)) is not valid. If it's transformed in:
 *		(Set L0 (+ (Lex 2 1) 1))
 *		(+ L0 (CCall foo ...))
 *	the semantics of the execution might change, because foo can modify
 *	the lexical environment.
 *
 ****************************************************************************
 * TO DO:
 *	- use the commutative property of some operators in the equality test
 *	- move sideing effect function calls outside of expr
 *		may be we can do that only if an optimiz. is possible
 *	- avoid redundant foamHash calls (caching them)
 *	   (or add tbl fun that take hash codes instead of keys)
 *	- use side effect info on runtime calls
 *	- avoid building the flog if num expr < 2
 *
 ****************************************************************************/

#include "of_comex.h"
#include "of_util.h"
#include "optfoam.h"
#include "of_rrfmt.h"

/****************************************************************************
 *
 * Macros
 *
 ****************************************************************************/

# define       cseExpInfo(foam)	((ExpInfo) (foam)->foamGen.hdr.info.expInfo)
# define       cseSetExpInfo(foam, einfo)  \
                    ((foam)->foamGen.hdr.info.expInfo = (ExpInfo) (einfo))

# define       cseExpNo(foam)		(cseExpInfo(foam)->expNo)
# define       csePhantomNo(foam)	(cseExpInfo(foam)->phantomNo + cseProgInfo.numExps)

# define		CSE_UndefinedLocal	-1
# define		cseDF_CUTOFF		200

# define	cseSetInvalidExp(foam)  (cseSetExpInfo((foam), NULL))

# define	cseIsTransformed(expInfo)    \
                            ((expInfo)->newLoc != CSE_UndefinedLocal)

# define	cseIsDef(foam)  (foamTag(foam) == FOAM_Set || \
                                 foamTag(foam) == FOAM_Def)

# define	cseIsBCall(foam)	(foamTag(foam) == FOAM_BCall)
# define	cseIsRRFmt(foam)	(foamTag(foam) == FOAM_RRFmt)
#if EDIT_1_0_n1_AA
# define	cseIsCEnv(foam)		(foamTag(foam) == FOAM_CEnv)
# define	cseIsOkay(foam)		(cseIsBCall(foam) || \
	       				 cseIsRRFmt(foam) || \
					 cseIsCEnv(foam))
#else
# define	cseIsOkay(foam)		(cseIsBCall(foam) || cseIsRRFmt(foam))
#endif

# define	cseIsMarkedValidExp(foam) (cseIsOkay(foam) && \
					   cseExpIsMarkedValid(foam))
# define	cseExpIsMarkedValid(foam) (cseExpInfo(foam) != NULL)

/****************************************************************************
 * :: Debug
 ****************************************************************************/

Bool      cseDebug 	  = false;
Bool      cseDfDebug 	  = false;
Bool      cseDfiDebug 	  = false;

# define   cseDEBUG(s)    DEBUG_IF(cseDebug,  s)
# define   cseDfDEBUG(s)  DEBUG_IF(cseDfDebug,  s)
# define   cseDfiDEBUG(s) DEBUG_IF(cseDfiDebug, s)

/****************************************************************************
 *
 * Type definitions
 *
 ****************************************************************************/

typedef int		ExpNo;	

DECLARE_LIST(ExpInfo);
CREATE_LIST(ExpInfo);

typedef struct {
	Foam		var;
	ExpInfoList	expInfoList;

} * ExternVarInfo;


DECLARE_LIST(ExternVarInfo);
CREATE_LIST(ExternVarInfo);


/****************************************************************************
 *
 * Global Data Structures
 *
 ****************************************************************************/

local Table	expTable;

/* Information corresponding to the current Prog */
struct {
	Foam		prog;		/* current prog */
	Foam		currentStmt;
	int		numExps;
	int		numEqualExps;

	int		numLocs;
	int		numPars;
	int		numLexs;
	int		numGlos;

	int		newLocs;  	/* starting from numLocs */

        ExpInfoList	*locv;
        ExpInfoList	*parv;

	ExternVarInfoList lexl;
	ExternVarInfoList glol;

	Bool		expRemoved;
	Bool		blockChanged;
	FoamBox		fbox;
	VarPool		newLocals;

	BitvClass	bitvClass;

} cseProgInfo;

typedef struct {
	ExpInfoList	expInfoList;

} * EqualExps;


/****************************************************************************
 *
 * Local Functions
 *
 ****************************************************************************/

local Foam		cseProg			(Foam);
local Bool		cseFlog0		(FlowGraph);

local void		cseAnalyseSeq		(Foam);
local Foam		cseAnalyseExpr		(Foam);

local Bool		cseIsValidExp		(Foam);
local Bool		cseIsValidBCall		(Foam);
local Bool		cseIsValidRRFmt		(Foam);
local Bool		cseIsValidSubTree	(Foam);
local Bool		cseIsSidingEffect	(Foam);

local void		cseFillGenKill		(FlowGraph, BBlock);
local void		cseGenExpDeeply		(Foam, BBlock);
local void		cseGenExp		(Foam, Bitv, Bitv);
local void		cseKillExpFrDef		(Foam, Bitv, Bitv, Bool);
local void		cseKillImpureExp	(Bitv, Bitv, Bool);
local void		cseKillExpFrExpInfoList	(ExpInfoList, Bitv, Bitv,Bool);

local void		cseCommonExpEliminate	(FlowGraph);
local void		cseCommonExpElimFrSeq	(Foam, BBlock);
local void		cseCommonExpElim	(Foam, BBlock);

local AInt		cseNewLocFrReachingExps	(ExpInfoList);

local void		cseRebuildCode		(FlowGraph);
local Foam		cseRebuildSeq		(Foam);
local Foam		cseRebuildExp		(Foam);

local void		cseAddToExpTable	(Foam);

local void		cseSetExpAndPhantomNo	(ExpInfo, EqualExps);
local Foam		cseSetVarsInExp		(ExpInfo, Foam);
local ExpInfoList	cseGetExpInfoListFrVar	(Foam);
local void		cseAddExpInfoToVarList	(ExpInfo, Foam);
local ExpInfoList	cseGetExpInfoListFrExternVar (Foam);
local void		cseAddExpInfoToExternVarList	(ExpInfo, Foam);
local Bool		cseNoEqualExps		(EqualExps);
local void		cseEqualExpsFree	(EqualExps);

local void		cseAddCopyOfExp		(AInt, Foam);
local void		cseAddCopyOfLoc		(AInt, ExpInfo);

local void		cseInitBlock0		(FlowGraph);

local void		cseProgInit		(Foam);
local void		cseProgFini		(void);
#if EDIT_1_0_n1_AA
local Bool		cseIsValidCEnv		(Foam);
#endif

/****************************************************************************
 *
 * Main External Entry Point
 *
 ****************************************************************************/

void
cseUnit(Foam foam)
{
	Foam		defs, def;
	int		i;

	assert(foamTag(foam) == FOAM_Unit);
	defs = foam->foamUnit.defs;

	for (i = 0; i < foamArgc(defs); i++) {

		def = defs->foamDDef.argv[i];

		assert(foamTag(def) == FOAM_Def);

		if (foamTag(def->foamDef.rhs) != FOAM_Prog)
			continue;

		def->foamDef.rhs = cseProg(def->foamDef.rhs);
        }
}

/* Given a flow graph, perform cse on it. 
 * NOTE: Prog is required only for the decls.
 */
void
cseFlog(FlowGraph flog, Foam prog)
{
	BBlock	bb;
	int		i;

	assert(foamTag(prog) == FOAM_Prog);

	cseProgInit(prog);
	
	/*------- First step: data structures building ---------*/

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;
		cseAnalyseSeq(bb->code);
	}

	if (cseProgInfo.numExps >= 2) {
		flogReuse(flog, FLOG_UniqueExit);
		cseFlog0(flog);
	}

	cseProgFini();
}


local Foam
cseProg(Foam foam)
{
	FlowGraph	flog;
	Bool		changed;

	assert(foamTag(foam) == FOAM_Prog);

	if (!optIsCsePending(foam)) return foam;
	optResetCsePending(foam);

	cseProgInit(foam);

	/*------- First step: data structures building ---------*/

	cseAnalyseSeq(foam->foamProg.body);

	if (cseProgInfo.numExps < 2) {
		cseProgFini();
		return foam;
	}

	flog = flogFrProg(foam, FLOG_UniqueExit);

	changed = cseFlog0(flog);

	cseProgFini();

	if (changed) {
		optSetCPropPending(foam);
		optSetDeadvPending(foam);
		optSetJFlowPending(foam);
	}

	return flogToProg(flog);
}

/****************************************************************************
 *
 * Main Procedure: CSE from a flog
 *
 ****************************************************************************/

/* Return true if some expr. have been removed */
local Bool
cseFlog0(FlowGraph flog)
{
	BBlock	bb;
	int		i, k;
	
	/* Remove redundant elements from the table */
	tblRemoveIf(expTable, (TblFreeEltFun) cseEqualExpsFree,
		    	      (TblTestEltFun) cseNoEqualExps);

	/*------ Second step: dflow analysis for available expressions -----*/

	flogBitvClass(flog) = bitvClassCreate(cseProgInfo.numExps +
					      cseProgInfo.numEqualExps);

	cseProgInfo.bitvClass = flogBitvClass(flog);

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb || bb->dfinfo) continue;
		dflowNewBlockInfo(bb,
			cseProgInfo.numExps + cseProgInfo.numEqualExps,
			cseFillGenKill);
	}

	i = dflowFwdIterate(flog, DFLOW_Union, cseDF_CUTOFF, &k,
			    (DFlowInitFun) cseInitBlock0);

	cseDfDEBUG({
		fprintf(dbOut, i == 0 ? "Converged" : "Did not converge");
		fprintf(dbOut, " after %d iterations\n", k);
		flogPrint(dbOut, flog, true);
	});

	if (i != 0) return false;

	/*------- Third step: common subexpression elimination ------- */

	cseProgInfo.newLocals =
		vpNew(fboxNew(cseProgInfo.prog->foamProg.locals));

	cseCommonExpEliminate(flog);

	/*------- Fourth step: Rebuild code with introduced stmts ---- */

	if (cseProgInfo.expRemoved == 0) return false;

	cseRebuildCode(flog);

	return true;

}

/****************************************************************************
 *
 * First Step: main data structures building
 *
 ****************************************************************************/

local void
cseAnalyseSeq(Foam seq)
{
	int	i;

	assert(foamTag(seq) == FOAM_Seq);

	for (i = 0; i < foamArgc(seq); i++) {
		cseProgInfo.currentStmt = seq->foamSeq.argv[i];
		cseAnalyseExpr(cseProgInfo.currentStmt);
	}
}

local Foam
cseAnalyseExpr(Foam foam)
{
	foamIter(foam, arg, cseAnalyseExpr(*arg));

	if (cseIsValidExp(foam))
		cseAddToExpTable(foam);

	return foam;
}

/* A foam is a Valid Expression if it:
 *   - is a valid BCall or
 *   - is a valid RRFmt
 * If foam is not valid, it's marked Bad.
 */
local Bool
cseIsValidExp(Foam foam)
{
	if (cseIsBCall(foam))
		return cseIsValidBCall(foam);
	else if (cseIsRRFmt(foam))
		return cseIsValidRRFmt(foam);
#if EDIT_1_0_n1_AA
	else if (cseIsCEnv(foam))
		return cseIsValidCEnv(foam);
#endif
	else
		return false;
}

#if EDIT_1_0_n1_AA
/*
 * A CEnv is a Valid Expression if:
 *   - its argument is a local
 */
local Bool
cseIsValidCEnv(Foam foam)
{
	return foamTag(foam->foamCEnv.env)==FOAM_Loc;
}
#endif

/*
 * A RRFmt is a always a valid expression if it doesn't contain
 * any SInt literals or BCalls with BCall or SInt arguments. This
 * strange requirement ensures that RRFmts which can be lifted into
 * C globals will do so. It also allows RRNew and RRElt instructions
 * with liftable RRFmts and non-escaping data to be replaced with
 * RNew and RElt calls.
 *
 * If an RRFmt contains CCalls or PCalls then we insist that these
 * calls are pure functions and we desparately want to comex them
 * to alleviate their cost.
 */
local Bool
cseIsValidRRFmt(Foam foam)
{
	/* Safety check */
	assert(foamTag(foam) == FOAM_RRFmt);
	assert(foamTag(foam->foamRRFmt.fmt) == FOAM_Values);

	return !rrRRFmtIsIndependent(foam);
}


/*
 * A BCall is a Valid Expression if:
 *   - there aren't leafs that are BCall marked Bad.
 *   - there aren't leafs with side effects
 *   - there is no access to not literals structures (record, env, etc.)
 * If foam is not valid, it's marked Bad.
 */
local Bool
cseIsValidBCall(Foam foam)
{
	int	i;
	Foam	argi;

	assert(cseIsBCall(foam));


	/* We don't want to comex Type* and SizeOf* bcalls */
	switch (foam->foamBCall.op)
	{
#if 0
		case FOAM_BVal_TypeInt8:	/* Fall through */
		case FOAM_BVal_TypeInt16:	/* Fall through */
		case FOAM_BVal_TypeInt32:	/* Fall through */
		case FOAM_BVal_TypeInt64:	/* Fall through */
		case FOAM_BVal_TypeInt128:	/* Fall through */
#endif
		case FOAM_BVal_TypeNil:		/* Fall through */
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
		case FOAM_BVal_TypePtr:		/* Fall through */
		case FOAM_BVal_TypeRec:		/* Fall through */
		case FOAM_BVal_TypeArr:		/* Fall through */
		case FOAM_BVal_TypeTR:		return false;

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
		case FOAM_BVal_SizeOfTR:	return false;
	}

	if (foamBValInfo(foam->foamBCall.op).retType == FOAM_NOp ||
	    foamBValInfo(foam->foamBCall.op).hasSideFx) {
		cseSetInvalidExp(foam);
		return false;
	}

	for (i = 0; i < foamArgc(foam) - 1; i++) {
		argi = foam->foamBCall.argv[i];

		if (!cseIsValidSubTree(argi))
			break;
	}

	/* Found bad leaf ? */
	if (i < foamArgc(foam) - 1)  {
		cseSetInvalidExp(foam);
		return false;
	}
	else
		return true;
}

local Bool
cseIsValidSubTree(Foam foam)
{
	if (cseIsBCall(foam))
		return cseExpIsMarkedValid(foam);

	if (foamTag(foam) == FOAM_Cast)
		return cseIsValidSubTree(foam->foamCast.expr);

	switch (foamTag(foam)) {
	case FOAM_Loc:
	case FOAM_Par:
	case FOAM_Lex:
	case FOAM_Glo:
	case FOAM_Nil:
	case FOAM_Char:
	case FOAM_Bool:
	case FOAM_Byte:
	case FOAM_HInt:
	case FOAM_SInt:
	case FOAM_BInt:
	case FOAM_SFlo:
	case FOAM_DFlo:
		return true;
	case FOAM_CCall:
	case FOAM_PCall:
	case FOAM_OCall: {
		/* int 	i; */

		if (cseIsSidingEffect(foam)) return false;

/* !! The current implementation could be less conservative. When, for
 * example, a CCall is found among one BCall's arguments, then this BCall
 * is rejected from the set of candidates for cse.
 * At the moment I prefer safety to more aggressive optimizations.
 */
#if 0	
/* !! ToDo: introduce foamNArgc */
	
		for (i = 0; i < foamNArgc(foam) - 1; i++) {
			Foam argi = foam->foamBCall.argv[i];
			if (!cseIsValidSubTree(argi))
				return false;
		}
#endif
		return true;

	}
	default:
		return false;
	}
}

/* !! Improve */
local Bool
cseIsSidingEffect(Foam foam)
{
	if (foamTag(foam) == FOAM_CCall ||
	    foamTag(foam) == FOAM_OCall ||
	    foamTag(foam) == FOAM_PCall)
		return true;

	return false;
}

/****************************************************************************
 *
 * Second Step: dflow analysis for available expressions
 *
 ****************************************************************************/

local void
cseFillGenKill(FlowGraph flog, BBlock bb)
{
	Foam		seq = bb->code, stmt;
	int		i;
	BitvClass	class = flogBitvClass(flog);

	assert(class && class == bbBitvClass(bb));

	cseDfiDEBUG(fprintf(dbOut, "Filling Gen/Kill for %d\n", bb->label));

	/*
	 * Clear the vectors.
	 */
	bitvClearAll(class, dfFwdGen(bb));
	bitvClearAll(class, dfFwdKill(bb, int0));

	for (i = 0; i < foamArgc(seq); i++) {
		stmt = seq->foamSeq.argv[i];

		cseGenExpDeeply(stmt, bb);

		if (cseIsDef(stmt))
			cseKillExpFrDef(stmt,
					dfFwdKill(bb, int0), dfFwdGen(bb),
					true);
	}
}

/* Write 1 in bitv if the corresponding exp is generated */
local void
cseGenExpDeeply(Foam foam, BBlock bb)
{
	foamIter(foam, arg, cseGenExpDeeply(*arg, bb));

	cseGenExp(foam, dfFwdGen(bb), dfFwdKill(bb, int0));

	/* Kill phantom bit */
	if (cseIsMarkedValidExp(foam))
		bitvSet(bbBitvClass(bb),
			dfFwdKill(bb, int0), csePhantomNo(foam));

	if (cseIsSidingEffect(foam))
		cseKillImpureExp(dfFwdKill(bb, int0), dfFwdGen(bb), true);
}

local void
cseGenExp(Foam foam, Bitv gen, Bitv kill)
{
	int 	expNo;

	if (!cseIsMarkedValidExp(foam))
		return;

	expNo = cseExpNo(foam);

	bitvSet(cseProgInfo.bitvClass, gen, expNo);
	bitvClear(cseProgInfo.bitvClass, kill, expNo);

}

/* Take the rhs, access to the expInfoList, i.e. all the expressions
 * containing that var, and assign the corresponding bits.
 *
 * BIT is the state in which bits are turned.
 */
local void
cseKillExpFrDef(Foam def, Bitv bitv, Bitv bitvPhantom, Bool bit)
{
	Foam		lhs = def->foamDef.lhs;
	ExpInfoList	expInfoList;

	assert(cseIsDef(def));

	expInfoList = cseGetExpInfoListFrVar(lhs);

	cseKillExpFrExpInfoList(expInfoList, bitv, bitvPhantom, bit);
}

/* Assign BIT to all the expr containing lex or glob
 *
 * BIT is the state in which bits are turned.
 */
local void
cseKillImpureExp(Bitv bitv, Bitv bitvPhantom, Bool bit)
{
	ExternVarInfoList	evInfol;

	for (evInfol = cseProgInfo.lexl; evInfol; evInfol = cdr(evInfol))
		cseKillExpFrExpInfoList(car(evInfol)->expInfoList, bitv,
					bitvPhantom, bit);

	for (evInfol = cseProgInfo.glol; evInfol; evInfol = cdr(evInfol))
		cseKillExpFrExpInfoList(car(evInfol)->expInfoList, bitv,
					bitvPhantom, bit);
}

/* Phantom bit: 1 -> expr NOT available
 * When an expr is killed	-> its phantom bit must be generated
 * When an expr is generated	-> its phantom bit must be killed
 *
 * Therefore:
 *
 * - building Kill and Gen: (bit = true)
 *
 * 	- bitv is Kill 		(we set expr bits)
 *	- bitvPhantom is Gen  	(we set phantom bit)
 *
 * - building IN: (bit = false)
 *
 * 	- bitv is In 	 	(we clear expr bits)
 *	- bitvPhantom is In	(we set phantom bit)
 */

local void
cseKillExpFrExpInfoList(ExpInfoList expInfoList, Bitv bitv,
			Bitv bitvPhantom, Bool bit)
{
	ExpInfo		expInfo;
	BitvClass	class = cseProgInfo.bitvClass;

	if (!expInfoList) return;


	for ( ; expInfoList; expInfoList = cdr(expInfoList)) {

		expInfo = car(expInfoList);

		/* Don't care about invalidated expressions */
		if (!cseExpIsMarkedValid(expInfo->exp))  continue;


		/* Valid RRFmt values can't be modified */
		if (cseIsRRFmt(expInfo->exp)) continue;

#if EDIT_1_0_n1_AA
		/* Valid CEnvs can't be modified (their contents can tho') */
		if (cseIsCEnv(expInfo->exp)) continue;
#endif

		if (bit)
			bitvSet(class, bitv, expInfo->expNo);
		else
			bitvClear(class, bitv, expInfo->expNo);

		bitvSet(class, bitvPhantom, csePhantomNo(expInfo->exp));

	}


	return;

}

/****************************************************************************
 *
 * Third Step: common exp elimination
 *
 ****************************************************************************/

local void
cseCommonExpEliminate(FlowGraph flog)
{
	BBlock	bb;
	int		i;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;

		cseCommonExpElimFrSeq(bb->code, bb);
	}
}

local void
cseCommonExpElimFrSeq(Foam foam, BBlock bb)
{
	Bitv 	dfin = dfFwdIn(bb);

	foamIter(foam, arg, cseCommonExpElimFrSeq(*arg, bb));

	cseCommonExpElim(foam, bb);

	cseGenExp(foam, dfin, dfFwdKill(bb, int0));

	/* Kill phantom bit */
	if (cseIsMarkedValidExp(foam))
		bitvClear(bbBitvClass(bb), dfin, csePhantomNo(foam));


	if (cseIsSidingEffect(foam))
		cseKillImpureExp(dfin, dfin, false);

	if (cseIsDef(foam))
		cseKillExpFrDef(foam, dfin, dfin, false);
}

local void
cseCommonExpElim(Foam foam, BBlock bb)
{
	ExpInfoList	equalReaching = listNil(ExpInfo);
	ExpInfoList	eqExpl;
	EqualExps	equalExps;
	ExpInfo		thisExp;
	Foam		expr;
	Bitv		dfin = dfFwdIn(bb);
	Bool		isTransformed;
	BitvClass	class = cseProgInfo.bitvClass;

	assert(class == bbBitvClass(bb));

	if (!cseIsMarkedValidExp(foam))
		return;

	equalExps = (EqualExps) tblElt(expTable, (TblKey) foam, (TblElt) 0);

	if (!equalExps) return;

	/* Is phantom bit alive ? */
	if (bitvTest(class, dfin, csePhantomNo(foam)))
		return;

	isTransformed = cseIsTransformed(cseExpInfo(foam));

	for (eqExpl = equalExps->expInfoList; eqExpl; eqExpl = cdr(eqExpl)) {
		expr = car(eqExpl)->exp;

		/* skip this expr */
		if (expr == foam)  continue;

		/* is expr available ? */
		if (bitvTest(class, dfin, cseExpNo(expr))) {

			/* Avoid loops: only virgins can replace not virgins */
			if (isTransformed &&
			    cseIsTransformed(cseExpInfo(foam)))
				continue;

			listPush(ExpInfo, car(eqExpl), equalReaching);
		}
	}

	if (!equalReaching) return;   /* No expr available */

	/* ----- equalReaching is the list of expr that can be substitued */

	thisExp = cseExpInfo(foam);

	assert(thisExp->evaluated);

	thisExp->evaluated = false;
	thisExp->newLoc = cseNewLocFrReachingExps(equalReaching);

	cseProgInfo.expRemoved = true;

	listFree(ExpInfo)(equalReaching);

	return;
}

/* Given an expInfoList of reaching exprs, updated their state.
 * Intuitively:
 *     a) (Set l1 (a + b)), (Set l1 (a + b)) -> return l1
 *     b) (Set l1 (a + b)), (Set l2 (a + b))
 *  -> 	generate (Set l3 l1), (Set l3 l2) and return l3
 *     c) (...(a + b)..), (Set l1 (a + b))
 *  -> 	generate (Set l2 (a + b)), (Set l2 l1) and return l2
 *
 * NOTE: One feature of this algorithm is that try to avoid the introduction
 *	of new locals whenever it is possible.
 *      It might be more paranoid looking in the copies list,
 *	but is not convenient spending all this time tring to avoid new locs.
 *	Probably now it's already too smart.
 */
local AInt
cseNewLocFrReachingExps(ExpInfoList expInfoList)
{
	AInt		newLoc = cseProgInfo.newLocs;
	AInt		oldLoc = CSE_UndefinedLocal;
	Bool		useNewLoc = false;
	AInt		newLocType = 0;
	Foam		fm;
	ExpInfo		expInfo;
	ExpInfoList	expInfoList0 = expInfoList;
	
	assert(expInfoList);

	for (; expInfoList; expInfoList = cdr(expInfoList)) {
		expInfo = car(expInfoList);

		if (expInfo->newLoc == CSE_UndefinedLocal) {
			expInfo->newLoc = newLoc;
			useNewLoc = true;

			fm = expInfo->exp;

			if (cseIsBCall(fm)) {
				AInt op = fm->foamBCall.op;
				newLocType = foamBValInfo(op).retType;
			}
			else if (cseIsRRFmt(fm))
				newLocType = FOAM_Word;
#if EDIT_1_0_n1_AA
			else if (cseIsCEnv(fm))
				newLocType = FOAM_Env;
#endif
			else {
				bug("cseNewLocFrReachingExps: bad foam");
				newLocType = FOAM_Word;
			}

			cseAddCopyOfExp(newLoc, expInfo->exp);
		}
		else if (useNewLoc)
			cseAddCopyOfLoc(newLoc, expInfo);
		else if (oldLoc == CSE_UndefinedLocal)
			oldLoc = expInfo->newLoc;
		else if (oldLoc != expInfo->newLoc) {
			useNewLoc = true;

			fm = expInfo->exp;

			if (cseIsBCall(fm)) {
				AInt op = fm->foamBCall.op;
				newLocType = foamBValInfo(op).retType;
			}
			else if (cseIsRRFmt(fm))
				newLocType = FOAM_Word;
#if EDIT_1_0_n1_AA
			else if (cseIsCEnv(fm))
				newLocType = FOAM_Env;
#endif
			else {
				bug("cseNewLocFrReachingExps: bad foam");
				newLocType = FOAM_Word;
			}

			cseAddCopyOfLoc(newLoc, expInfo);
		}
	}

	if (useNewLoc) {
		AInt newLoc0; 

		assert(newLoc == cseProgInfo.newLocs);
		cseProgInfo.newLocs += 1;

		expInfoList = expInfoList0;
	
		for (; expInfoList; expInfoList = cdr(expInfoList)) {
			expInfo = car(expInfoList);

			if (expInfo->newLoc != newLoc)
				cseAddCopyOfLoc(newLoc, expInfo);
			else
				break;  /* the remaing are already updated */
		}

		newLoc0 = vpNewVar(cseProgInfo.newLocals, newLocType);

		assert(newLoc0 == newLoc);
	}
	else
		newLoc = oldLoc;

	return newLoc;
}

/****************************************************************************
 *
 * Fourth step: Rebuild code
 *
 ****************************************************************************/

local void
cseRebuildCode(FlowGraph flog)
{
	BBlock	bb;
	int		i;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;

		bb->code = cseRebuildSeq(bb->code);
		/* cseDEBUG(if (cseProgInfo.blockChanged)
		 *	    	foamPrintDb(bb->code););
		 */
	}

	/* Create a new locals section */
	cseProgInfo.prog->foamProg.locals = 
		fboxMake(cseProgInfo.newLocals->fbox);
}

local Foam
cseRebuildSeq(Foam foam)
{
	int 		i;
	Foam		stmt;

	assert(foamTag(foam) == FOAM_Seq);

        cseProgInfo.blockChanged = false;
	cseProgInfo.fbox = fboxNewEmpty(FOAM_Seq);

	for (i = 0; i < foamArgc(foam); i++) {
		stmt = foam->foamSeq.argv[i];
		stmt = cseRebuildExp(stmt);
		fboxAdd(cseProgInfo.fbox, stmt);
	}

	if (cseProgInfo.blockChanged)
		foam = fboxMake(cseProgInfo.fbox);
	else
		fboxFree(cseProgInfo.fbox);

	return foam;
}

local Foam
cseRebuildExp(Foam foam)
{
	FoamList  	copies;

	foamIter(foam, arg, *arg = cseRebuildExp(*arg));

	if (!cseIsMarkedValidExp(foam))
		return foam;

	assert (!cseExpInfo(foam)->copies ||
		cseExpInfo(foam)->newLoc != CSE_UndefinedLocal );

	copies = cseExpInfo(foam)->copies;
	copies = listNReverse(Foam)(copies);

	for (; copies; copies = cdr(copies))
		fboxAdd(cseProgInfo.fbox, car(copies));

	if (cseExpInfo(foam)->newLoc != CSE_UndefinedLocal) {
 		cseProgInfo.blockChanged = true;

		cseDEBUG(if (!cseExpInfo(foam)->evaluated) {
			 fprintf(dbOut, "CSE>> ------- Removed redundant evaluation of: ------- \n");
			 foamPrintDb(foam);
		 });

		return foamNewLoc(cseExpInfo(foam)->newLoc);
	}
	else 
		return foam;
}
/****************************************************************************
 *
 * ExpTable managment
 *
 ****************************************************************************/

local void
cseAddToExpTable(Foam foam)
{
	EqualExps	equalExps;
	ExpInfo		expInfo;

	assert(cseIsOkay(foam));
	
		/* Create a new expInfo for the current expr */
	expInfo = (ExpInfo) stoAlloc(OB_Other, sizeof(*expInfo));

	expInfo->exp 		= foam;
	expInfo->newLoc		= CSE_UndefinedLocal;
	expInfo->copies 	= listNil(Foam);
	expInfo->evaluated 	= true;

	equalExps = (EqualExps) tblElt(expTable, (TblKey) foam, (TblElt) 0);

	if (!equalExps) {

		/* Create a new entry ... */
		equalExps = (EqualExps) stoAlloc(OB_Other, sizeof(*equalExps));

		/* ... initialize it */
		equalExps->expInfoList = listNil(ExpInfo);
		
		/* ... add it to the table */
		tblSetElt(expTable, (TblKey) foam, (TblElt) equalExps);

	}

	cseSetExpAndPhantomNo(expInfo, equalExps);

	listPush(ExpInfo, expInfo, equalExps->expInfoList);

	cseSetVarsInExp(expInfo, foam);

	cseSetExpInfo(foam, expInfo);
}

/****************************************************************************
 *
 * Utility
 *
 ****************************************************************************/

/*
 * This block of procedures lack of comments. Look at their usage to
 * understand what they do.
 *
 */


local void
cseSetExpAndPhantomNo(ExpInfo expInfo, EqualExps equalExps)
{
	ExpInfoList	expInfoList = equalExps->expInfoList;

	/* Is expInfo the first of its class ? */
	if (!expInfoList) return;

	/* Already >1 equal ? */
	if (cdr(expInfoList)) {

		/* Only this needs to be updated */

		expInfo->phantomNo = car(expInfoList)->phantomNo;
		expInfo->expNo = cseProgInfo.numExps++;
	}
	else {
		/* This is the second one: update first and second */

		expInfo->phantomNo = cseProgInfo.numEqualExps;
		expInfo->expNo = cseProgInfo.numExps++;

		car(expInfoList)->expNo = cseProgInfo.numExps++; 
		car(expInfoList)->phantomNo = cseProgInfo.numEqualExps; 

		/* Why increment numExps again? */
		cseProgInfo.numExps++;
		cseProgInfo.numEqualExps += 1;
	}
}

local Foam
cseSetVarsInExp(ExpInfo expInfo, Foam foam)
{
	foamIter(foam, arg, cseSetVarsInExp(expInfo, *arg));

	if (foamTag(foam) == FOAM_Loc ||
	    foamTag(foam) == FOAM_Par ||
	    foamTag(foam) == FOAM_Lex ||
	    foamTag(foam) == FOAM_Glo)
		cseAddExpInfoToVarList(expInfo, foam);	

	return foam;
}

/* Return NULL if there is no expInfoList for VAR */
local ExpInfoList
cseGetExpInfoListFrVar(Foam var)
{
	if (foamTag(var) == FOAM_Loc) {
		assert(var->foamLoc.index < cseProgInfo.numLocs);
		return cseProgInfo.locv[var->foamLoc.index];
	}
	else if (foamTag(var) == FOAM_Par) {
		assert(var->foamPar.index < cseProgInfo.numPars);
		return cseProgInfo.parv[var->foamPar.index];
	}
	else if (foamTag(var) == FOAM_Glo || foamTag(var) == FOAM_Lex)
		return cseGetExpInfoListFrExternVar(var);
	else
		return listNil(ExpInfo);
}

local void
cseAddExpInfoToVarList(ExpInfo expinfo, Foam var)
{
	ExpInfoList	* pExpinfolist;

	if (foamTag(var) == FOAM_Loc) {
		assert(var->foamLoc.index < cseProgInfo.numLocs);
		pExpinfolist = &cseProgInfo.locv[var->foamLoc.index];
	}
	else if (foamTag(var) == FOAM_Par) {
		assert(var->foamPar.index < cseProgInfo.numPars);
		pExpinfolist = &cseProgInfo.parv[var->foamPar.index];
	}
	else if (foamTag(var) == FOAM_Glo || foamTag(var) == FOAM_Lex) {
		cseAddExpInfoToExternVarList(expinfo, var);
		return;
	}
	else
		return;

	if (!listMemq(ExpInfo)(*pExpinfolist, expinfo))
		listPush(ExpInfo, expinfo, *pExpinfolist);
}

local ExpInfoList
cseGetExpInfoListFrExternVar(Foam var)
{
	ExternVarInfoList	evInfol;

	evInfol = (foamTag(var) == FOAM_Lex ?
			 cseProgInfo.lexl : cseProgInfo.glol);

	for (; evInfol; evInfol = cdr(evInfol))
		if (foamEqual(var, car(evInfol)->var))
			return car(evInfol)->expInfoList;

	return listNil(ExpInfo);
}

local void
cseAddExpInfoToExternVarList(ExpInfo expInfo, Foam var)
{
	ExternVarInfoList	evInfol;

	evInfol = (foamTag(var) == FOAM_Lex ?
			 cseProgInfo.lexl : cseProgInfo.glol);
			
	for (; evInfol; evInfol = cdr(evInfol))
		if (foamEqual(var, car(evInfol)->var))
			break;
	
	if (!evInfol) {
		ExternVarInfo evInfo =
			(ExternVarInfo) stoAlloc(OB_Other, sizeof(*evInfo));

		ExternVarInfoList * pl =(foamTag(var) == FOAM_Lex ?
			  		&(cseProgInfo.lexl) :
					&(cseProgInfo.glol));

		evInfo->var = var;
		evInfo->expInfoList = listNil(ExpInfo);

		listPush(ExternVarInfo, evInfo, *pl);
		listPush(ExpInfo, expInfo, evInfo->expInfoList);
	}
	else if (!listMemq(ExpInfo)(car(evInfol)->expInfoList, expInfo)) {

		assert(car(evInfol)->expInfoList);

		listPush(ExpInfo, expInfo, car(evInfol)->expInfoList);
	}
		
}

local Bool
cseNoEqualExps(EqualExps equalExps)
{
	assert(equalExps && equalExps->expInfoList);

	/* 1 element list ? */
	return (cdr(equalExps->expInfoList) == listNil(ExpInfo));
}

local void
cseEqualExpsFree(EqualExps eqExp)
{
	if (!eqExp) return;

	listIter(ExpInfo, expInfo, eqExp->expInfoList,
		 { cseSetInvalidExp(expInfo->exp); })
	listFree(ExpInfo)(eqExp->expInfoList);
	stoFree(eqExp);
}

local void
cseExternVarInfoFree(ExternVarInfo evi)
{
	listFree(ExpInfo)(evi->expInfoList);
}

local void
cseAddCopyOfExp(AInt newLoc, Foam exp)
{
	Foam	newSet;

	newSet = foamNewSet(foamNewLoc(newLoc), exp);

	assert(!cseExpInfo(exp)->copies);

	listPush(Foam, newSet, cseExpInfo(exp)->copies);
}

local void
cseAddCopyOfLoc(AInt newLoc, ExpInfo expInfo)
{
	Foam	newSet;

	newSet = foamNewSet(foamNewLoc(newLoc), foamNewLoc(expInfo->newLoc));

	listPush(Foam, newSet, expInfo->copies);
}

/* Set phantom bits */
local void
cseInitBlock0(FlowGraph flog)
{
	int i;

	assert(flog->block0);

	for (i = cseProgInfo.numExps;
	     i < cseProgInfo.numExps + cseProgInfo.numEqualExps;i++)
		bitvSet(cseProgInfo.bitvClass, dfFwdIn(flog->block0), i);
}

/****************************************************************************
 *
 * Init / Fini procedures
 *
 ****************************************************************************/

local void
cseProgInit(Foam foam)
{
	/* Foam		levels = foam->foamProg.levels; */
	int		i;

	assert(foamTag(foam) == FOAM_Prog);

	cseProgInfo.numLocs = foamDDeclArgc(foam->foamProg.locals);
	cseProgInfo.numPars = foamDDeclArgc(foam->foamProg.params);

	cseProgInfo.numExps = 0;
	cseProgInfo.numEqualExps = 0;
	cseProgInfo.prog = foam;

	cseProgInfo.locv = (ExpInfoList *) stoAlloc(OB_Other,
				sizeof(ExpInfoList) * cseProgInfo.numLocs);

	for (i = 0; i < cseProgInfo.numLocs; i++)
		cseProgInfo.locv[i] = listNil(ExpInfo);

	cseProgInfo.parv = (ExpInfoList *) stoAlloc(OB_Other,
				sizeof(ExpInfoList) * cseProgInfo.numPars);

	for (i = 0; i < cseProgInfo.numPars; i++)
		cseProgInfo.parv[i] = listNil(ExpInfo);

	cseProgInfo.lexl = listNil(ExternVarInfo);
	cseProgInfo.glol = listNil(ExternVarInfo);

	cseProgInfo.newLocs = cseProgInfo.numLocs;
	cseProgInfo.expRemoved = false;
	cseProgInfo.newLocals = (VarPool) NULL;

	/* $$!! use foamQuickEqual to avoid string comparison */
	/* $$!! use foamSmartHash for commutative property */
	expTable = tblNew((TblHashFun) foamHash, (TblEqFun) foamEqual);

	return;
}

local void
cseProgFini()
{
	tblFreeDeeply(expTable, (TblFreeKeyFun) NULL,
		      (TblFreeEltFun) cseEqualExpsFree);

	stoFree(cseProgInfo.locv);
	stoFree(cseProgInfo.parv);

	listFreeDeeply(ExternVarInfo)(cseProgInfo.lexl,
				      cseExternVarInfoFree);

	listFreeDeeply(ExternVarInfo)(cseProgInfo.glol,
				      cseExternVarInfoFree);

	if (cseProgInfo.newLocals)
		vpFree(cseProgInfo.newLocals);
}

/****************************************************************************
 *
 * End of File
 *
 ****************************************************************************/



