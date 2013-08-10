/*****************************************************************************
 *
 * of_jflow.c: Jump removal
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "loops.h"
#include "of_jflow.h"
#include "opttools.h"

Bool	jflowCatDebug	= false;
Bool	jflowDfDebug	= false;
Bool	jflowGoDebug	= false;
Bool	jflowDfiDebug	= false;
Bool	jflowDmDebug	= false;

#define jflowCatDEBUG	DEBUG_IF(jflowCat)	afprintf
#define jflowDfDEBUG	DEBUG_IF(jflowDf)	afprintf
#define jflowDfiDEBUG	DEBUG_IF(jflowDfi)	afprintf
#define jflowGoDEBUG	DEBUG_IF(jflowGo)	afprintf
#define jflowDmDEBUG	DEBUG_IF(jflowDm)	afprintf


/*****************************************************************************
 *
 * :: JFlow parameters
 *
 ****************************************************************************/

#define	  jflowDF_CUTOFF  200	/* Max no dataflow iterations to converge. */

static int   jflowMaxClone;	/* Max no of stats to clone. */


/**************************************************************************
 *
 * :: JFInfo structure
 *
 *************************************************************************/

/*
 * This structure is used to keep track of data-flow information
 * about "control" variables. 
 *
 * These variables are introduced by the code generation for generators
 * or by user-level code.  Specifically, a control variable is a Bool or SInt
 * typed local that
 * + is assigned only constants
 * + via simple assignments and
 * + appears in the condition of an If (possibly "Not"-ed) or Select.
 */
struct jfInfo {
	FlowGraph	flog;	  	/* The graph.                    */
	int		locCount;	/* Number of local variables.    */
	Foam		locDDecl;	/* DDecl from prog.              */
	struct jvInfo * locInfo;	/* Information about each local. */
	int		bitCount;	/* Number of dataflow bits.	 */

	Foam		locVals;	/* Work struct for bbLastValues. */
	Foam		same, dunno;	/* Markers for bbLastValues.	 */

	BitvClass	bitvClass;
};

/*
 * Information about a variable.
 */
struct jvInfo {
	BPack(Bool)	isControl;	/* Is it a control variable?   */
	short		origin;		/* Index into dataflow bits.   */
	short		maxval;		/* May take values 0..maxval.  */
};


typedef struct jvInfo	*JVInfo;
typedef struct jfInfo	*JFInfo;


/* Exits from if-bblocs */
#define FALSE_EXIT	0
#define TRUE_EXIT	1

/*****************************************************************************
 *
 * :: Forward declarations
 *
 ****************************************************************************/

/*
 * Info maintainters
 */
local JFInfo	jflowInfoNew		(FlowGraph);
local void	jflowInfoRefresh	(JFInfo);
local void	jflowInfoFree		(JFInfo);

/*
 * Code improvers
 */
local void	jflowFlipNegates	(FlowGraph);
local int	jflowUltimateGotos      (FlowGraph);
local int	jflowConcat		(FlowGraph);
local int	jflowSpecializeByAsst	(JFInfo, FlowGraph);
local int	jflowSpecializeByExit	(JFInfo, FlowGraph);
local int	jflowDummyTestsRemove	(JFInfo, FlowGraph);

/*
 * Helpers
 */
local Foam	jflowFindBBControlVar		(BBlock);
local Bool	jflowIsVarAssignedInBlock	(int, BBlock);
local Bool	jflowIsNegatedLocal		(Foam);
local void	jflowAssignDFIndices		(JFInfo);
local void	jflowFillDFGenKill		(FlowGraph, BBlock);
local void	jflowFindControlVars		(JFInfo);
local void	jflowFillControlVar		(JFInfo,    BBlock);
local int	jflowHowIsBitvDominating	(JFInfo, Bitv, BBlock);
local int	jflowFindEndOfLine		(FlowGraph, int, int *);


/*
 * Macros
 */
#define    	jvIsControl(jf,n)  	((jf)->locInfo[n].isControl)
#define		jvMaxVal(jf,n)		((jf)->locInfo[n].maxval)
#define		jvOrgIx(jf,n)		((jf)->locInfo[n].origin)
#define		jvLimIx(jf,n)		((jf)->locInfo[n].origin + \
					 (jf)->locInfo[n].maxval + 1)

#define		jflowLocIndex(loc)	((loc)->foamLoc.index)
#define		bbControlData(bb)	((AInt)((bb)->pextra))
#define		bbControlVar(bb)	(bbControlData(bb))

/*****************************************************************************
 *
 * :: Main entry point
 *
 ****************************************************************************/

/* "inlineLimit" is used to decide the max num. of statements that can be
 * clone. We could have an extra option, but we want to avoid a complicated
 * user interface.
 */
void
jflowUnit(Foam unit, int jflowLimit)
{
	Foam	ddef, def, rhs;
	int	i;

	assert(foamTag(unit) == FOAM_Unit);

	jflowMaxClone = jflowLimit;

	ddef = unit->foamUnit.defs;
	assert(foamTag(ddef) == FOAM_DDef);

	for (i = 0; i < foamArgc(ddef); i++) {
		def = ddef->foamDDef.argv[i];
		assert(foamTag(def) == FOAM_Def);

		rhs = def->foamDef.rhs;
		if (foamTag(rhs) == FOAM_Prog) jflowProg(rhs);
	}
	assert(foamAudit(unit));
}

void
jflowProg(Foam prog)
{
	FlowGraph	flog;
	JFInfo		jfinfo;
	Bool		changed = false;
	int		n;

	assert(foamTag(prog) == FOAM_Prog);

	if (prog->foamProg.nLabels == 0) return;

	if (!optIsJFlowPending(prog)) return;
	optResetJFlowPending(prog);

	flog = flogFrProg(prog, FLOG_MultipleExits);
	if (DEBUG(jflowDf)) {
		fprintf  (dbOut, "--> Enter jflowProg: The flow graph is:\n");
		flogPrint(dbOut, flog, true);
		fprintf  (dbOut, "\n");
	}

	jfinfo = jflowInfoNew(flog);

	for (n = 1; n > 0; ) {
		jflowFlipNegates(flog);
		jflowUltimateGotos(flog);
		jflowConcat(flog);

		jflowInfoRefresh(jfinfo);
		n = jflowSpecializeByAsst(jfinfo, flog);

		if (n > 0) {
			changed = true;
			continue;
		}

		n = jflowSpecializeByExit(jfinfo, flog);

		if (n > 0) {
			changed = true;
			continue;
		}

		n = jflowDummyTestsRemove(jfinfo, flog);

		if (n > 0) changed = true;
	}

	jflowInfoFree(jfinfo);

	flogCompress(flog);
	flogToProg(flog);	/* This updates prog (pointed to by flog). */

	if (changed) {
		optSetPeepPending(prog);
		optSetCPropPending(prog);
		optSetDeadvPending(prog);
	}

	if (DEBUG(jflowDf)) {
		fprintf  (dbOut, "<-- Exit jflowProg: The foam is:\n");
		foamPrint(dbOut, prog);
		fprintf  (dbOut, "\n");
	}
}

/**************************************************************************
 *
 * :: jflowFlipNegates -- Change (If (BCall BoolNot (Loc N))) into
 *                        (If (Loc N)) by flipping exits.
 *
 *************************************************************************/

local void
jflowFlipNegates(FlowGraph flog)
{
	BBlock	btrue, bfalse;
	Foam	code, bbcode;


	/* Return immediately if the user doesn't want this */
	if (!jflowCanNegate()) return;

	flogIter(flog, bb, {
		/* Get the FOAM for the bblock */
		bbcode = bb->code;


		/* Only want to process if statements */
		if ((bb->kind) != FOAM_If) continue;


		/* Extract the controlling statement */
		bbcode = bbcode->foamSeq.argv[foamArgc(bbcode)-1];
		code = bbcode->foamIf.test;


		/* Is the test a negated local? */
		if (!jflowIsNegatedLocal(code)) continue;


		/* Remove the negation from the test */
		code = code->foamBCall.argv[0];


		/* Update the bblock */
		bbcode->foamIf.test = code;


		/* Read the two exits */
		btrue  = bbExit(bb, TRUE_EXIT);
		bfalse = bbExit(bb, FALSE_EXIT);


		/* Rewire the exits */
		bbSetExit(bb, TRUE_EXIT, bfalse);
		bbSetExit(bb, FALSE_EXIT, btrue);
	});


	/* Clean up the flog */
	flogCollect(flog);
}

/**************************************************************************
 *
 * :: jflowUltimateGotos -- Retarget branches with ultimate destinations.
 *
 *************************************************************************/

/*
 * Follow goto-chains.
 */
local int
jflowUltimateGotos(FlowGraph flog)
{
	int		i, j, n, L, nsaved, *whereTo;
	BBlock	bb;

	jflowGoDEBUG(dbOut, ">> Enter jflowUltimateGotos\n");

	/* Use an array to hold the ultimate labels for blocks. */
	n = flogBlockC(flog);
	whereTo = (int *) stoAlloc(OB_Other, n * sizeof(int));
	for (i = 0; i < n; i++) whereTo[i] = -1;
	for (i = 0; i < n; i++) {
		jflowGoDEBUG(dbOut, ".. Following block %d\n", i);
		jflowFindEndOfLine(flog, i, whereTo);
	}

	/* Update all exits to go straight where they are going. */
	nsaved = 0;
	for (i = 0; i < n; i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;
		jflowGoDEBUG(dbOut, "== Resetting block %d\n", i);
		for (j = 0; j < bbExitC(bb); j++) {
			L  = bbExit(bb, j)->label;
			if (whereTo[L] != L) {
				L = whereTo[L];
				assert(L != -1);
				bbSetExit(bb, j, flogBlock(flog, L));
				nsaved++;
			}
		}
	}

	/* Return store and clean up graph. */
	stoFree((Pointer) whereTo);
	flogCollect(flog);

	jflowGoDEBUG(dbOut, "<< Exit  jflowUltimateGotos: saved %d\n",
		     nsaved);

	return nsaved;
}

local int
jflowFindEndOfLine(FlowGraph flog, int lab, int *whereTo)
{
	if (DEBUG(jflowGo)) {
		/*static int	odo = 0;
		if (odo++ > 10000)*/ {
			BBlock bb = flogBlock(flog, lab);
			fprintf(dbOut, "*** Entering EOL: lab=%d, => %d\n",
				lab, whereTo[lab]);
			fprintf(dbOut, "*** == 0 ? %d\n", bb == 0);
			if (bb) bbPrint(dbOut, bb, true);
		}
	}
		
	if (whereTo[lab] == -1) {
		BBlock bb = flogBlock(flog, lab);
		whereTo[lab] = lab;
		if (bb && bb->kind == FOAM_Goto && foamArgc(bb->code) == 1) {
			BlockLabel golab = bbExit(bb, int0)->label;
			whereTo[lab] = jflowFindEndOfLine(flog,golab,whereTo);
		}
	}

	jflowGoDEBUG(dbOut, "*** Returning %d\n", whereTo[lab]);

	return whereTo[lab];
}


/**************************************************************************
 *
 * :: jflowConcat -- Merge blocks with single unconditional entry.
 *
 *************************************************************************/

local int
jflowConcat(FlowGraph flog)
{
	BBlock	bb,  dd;
	BlockLabel	lbb, ldd;
	int		i, j, nm, nsaved, nrounds;

	/* Mark blocks which cannot be merged with their predecessors. */
	flogFixRefCounts(flog);
	flogClearMarks(flog);

	flog->block0->mark = true;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;

		if (bb->refc > 1)
			bb->mark = true;

		if (bb->kind != FOAM_Goto) 
			for (j = 0; j < bbExitC(bb); j++)
				bbExit(bb, j)->mark = true;
	}

	/* Count how many we could merge. */
	nsaved = 0;
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (bb && !bb->mark) nsaved++;
	}

	/* Merge the blocks. */
	nm = 1;
	for (nrounds = 0; nm > 0; nrounds++) {
		nm = 0;

		jflowCatDEBUG(dbOut,"+++ jflowConcat [%d]\n",nrounds);

		for (i = 0; i < flogBlockC(flog); i++) {
			jflowCatDEBUG(dbOut, "++ %d.\n", i);

			bb = flogBlock(flog, i);
			if (!bb || bb->kind != FOAM_Goto) continue;

			dd = bbExit(bb, int0);
			if (dd->mark) continue;	 /* Marked non-mergeable. */

			if (DEBUG(jflowCat)) {
				fprintf(dbOut, "++ Concatenating the nodes\n");
				bbPrint(dbOut, bb, true);
				fprintf(dbOut, "++ and\n");
				bbPrint(dbOut, dd, true);
			}

			lbb = bb->label;
			ldd = dd->label;

			bb = bbNConcat(bb, dd);
			flogSetBlock (flog, lbb, bb);
			flogKillBlock(flog, ldd);

			if (DEBUG(jflowCat)) {
				fprintf(dbOut, "++ to get\n");
				bbPrint(dbOut, bb, true);
			}

			nm++;
		}
	}
			
	jflowCatDEBUG(dbOut, "+++ Save %d catenating [%d]\n",
		      nsaved, nrounds);

	return nsaved;
}


/**************************************************************************
 *
 * :: JFInfo structure
 *
 *************************************************************************/

local JFInfo
jfinfoAlloc(FlowGraph flog)
{
	JFInfo	jfinfo;
	Foam	locs  = flog->prog->foamProg.locals;
	int	nlocs = foamDDeclArgc(locs);

	jfinfo = (JFInfo) stoAlloc(OB_Other, sizeof(*jfinfo));

	jfinfo->flog     = flog;
	jfinfo->locCount = nlocs;
	jfinfo->locDDecl = locs;

	jfinfo->locInfo  = (struct jvInfo *)
			   stoAlloc(OB_Other, nlocs * sizeof(struct jvInfo));

	jfinfo->locVals  = foamNewEmpty(FOAM_Seq, nlocs);
	jfinfo->same     = foamNewLabel(1);
	jfinfo->dunno    = foamNewLabel(2);

	return jfinfo;
}

local void
jfinfoFree(JFInfo jfinfo)
{
	foamFreeNode(jfinfo->locVals);
	foamFreeNode(jfinfo->same);
	foamFreeNode(jfinfo->dunno);

	stoFree(jfinfo->locInfo);
	stoFree(jfinfo);
}

/*****************************************************************************
 *
 * :: Setup functions for dataflow analysis of control variables
 *
 ****************************************************************************/

local JFInfo
jflowInfoNew(FlowGraph flog)
{
	JFInfo	jfinfo;

	jfinfo = jfinfoAlloc(flog);

	flog->pextra      = (Pointer) jfinfo;

	jflowFindControlVars(jfinfo);
	jflowAssignDFIndices(jfinfo);

	jflowInfoRefresh(jfinfo);

	if (DEBUG(jflowDf)) {
		fprintf(dbOut, "The flow graph with dataflow structures:\n");
		flogPrint(dbOut, flog, true);
	}

	return jfinfo;
}

local void
jflowInfoRefresh(JFInfo jfinfo)
{
	FlowGraph	flog = jfinfo->flog;
	BBlock	bb;
	int		i;

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb || bb->dfinfo) continue;
		jflowFillControlVar(jfinfo, bb);
	}

	flogBitvClass(flog) = bitvClassCreate(jfinfo->bitCount);
	jfinfo->bitvClass   = flogBitvClass(flog);

	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb || bb->dfinfo) continue;
		dflowNewBlockInfo(bb, jfinfo->bitCount, jflowFillDFGenKill);
	}

}

local void 
jflowInfoFree(JFInfo jfinfo)
{
	dflowFreeGraphInfo(jfinfo->flog);
	jfinfoFree(jfinfo);
}


/*
 * Find the control variables and their ranges.
 */
local void
jflowFindControlVars(JFInfo jfinfo)
{
	FlowGraph  flog = jfinfo->flog;
	Foam	   cv;
	int	   i, n, ox, nx;

	/*
	 * Find all variables controlling a multi-way exit and set
	 * the max value to be the max spread - 1.
	 */
	for (i = 0; i < jfinfo->locCount; i++) {
		jvIsControl(jfinfo, i) = false;
		jvMaxVal(jfinfo, i)    = -1;
	}

	flogIter(flog, bb, {
		cv = jflowFindBBControlVar(bb);
		if (!cv) continue;
		n  = jflowLocIndex(cv);
		ox = jvMaxVal(jfinfo, n);
		nx = bbExitC(bb) - 1;
		if (ox < nx) jvMaxVal(jfinfo,n) = nx;
		jvIsControl(jfinfo, n) = true;
	});

#if 0
	/*
	 * Cancel any control variables which are assigned non-constants.
	 */

	flogIter(flog, bb, {

		bbLastValues(bb, NULL, jfinfo->locVals,
			     jfinfo->same,jfinfo->dunno);

		for (n = 0; n < jfinfo->locCount; n++) {
			Foam vi = jfinfo->locVals->foamSeq.argv[n];
			int  vn;

			if (!jvIsControl(jfinfo, n) || vi == jfinfo->same)
				continue;

			otDereferenceCast(vi);

			switch (foamTag(vi)) {
			case FOAM_SInt:
				vn = vi->foamSInt.SIntData;
				if (vn < 0 || vn > jvMaxVal(jfinfo,n))
					jvIsControl(jfinfo,n) = false;
				break;
			case FOAM_Bool:
				vn = vi->foamBool.BoolData;
				if (vn > jvMaxVal(jfinfo,n))
					jvIsControl(jfinfo,n) = false;
				break;
			default:
#if 1 /* $$ */
				if (!jvIsInIf, n))
#endif
					jvIsControl(jfinfo,n) = false;
				break;
			}
		}
	});
#endif
}

/*
 * Return the local for a basic block's control variable.
 * If the block is not controlled by a local, then return 0.
 */
local Foam
jflowFindBBControlVar(BBlock bb)
{
	Foam code;

	if (!bb) return 0;

	code = bb->code;
	code = code->foamSeq.argv[foamArgc(code)-1];

	switch (foamTag(code)) {
	case FOAM_If:
		code = code->foamIf.test;
		if (foamTag(code) == FOAM_Loc) return code;
		break;
	case FOAM_Select:
		code = code->foamSelect.op;
		if (foamTag(code) == FOAM_Loc) return code;
		break;
	default:
		break;
	}
	return 0;
}


/*
 * Put control variable information onto a block.
 */
local void
jflowFillControlVar(JFInfo jfinfo, BBlock bb)
{
	Foam	cv;
	int	ix;

	bbControlData(bb) = -1;

	cv = jflowFindBBControlVar(bb);

	if (!cv) return;
	ix = jflowLocIndex(cv);

	if (jvIsControl(jfinfo, ix)) {
		bbControlData(bb) = jflowLocIndex(cv);
		if (DEBUG(jflowDfi)) {
			fprintf(dbOut, "Filled c.v. for block %d ", bb->label);
			foamPrint(dbOut, cv);
		}
	}
}


/*
 * Determine the indices within the dataflow bit set for the properties
 * of each variable.  The function returns the number of indicies.
 */
local void
jflowAssignDFIndices(JFInfo jfinfo)
{
	int	i, front;

	for (i = 0, front = 0; i < jfinfo->locCount; i++) {
		if (jvIsControl(jfinfo, i)) {
			jvOrgIx(jfinfo, i) = front;
			front += jvMaxVal(jfinfo, i) + 1;
		}
		else
			jvOrgIx(jfinfo, i) = 0;
	}

	jfinfo->bitCount =  front;

	if (DEBUG(jflowDf)) {
		int	i;
		fprintf(dbOut, "Have %d dataflow bits.\n", jfinfo->bitCount);
		fprintf(dbOut, "The control variables are:\n");
		for (i = 0; i < jfinfo->locCount; i++) {
			if (!jvIsControl(jfinfo, i)) continue;
			fprintf(dbOut, "Indices %d..%d for (Loc %d) = ",
				jvOrgIx(jfinfo,i),
				jvLimIx (jfinfo,i) - 1,
				i);
			foamPrint(dbOut, jfinfo->locDDecl->foamDDecl.argv[i]);
		}
	}
}

/*
 * This function decides what information is generated and killed
 * for dataflow analysis.
 */ 
local void jflowSetKillAllBut(JFInfo, Bitv, int, int);

local void
jflowFillDFGenKill(FlowGraph flog, BBlock bb)
{
	int		n, i, vval; 
	JFInfo  	jfinfo;
	BitvClass	class = bbBitvClass(bb);

	if (!bb) return;

	jfinfo = (JFInfo)(flog->pextra);

	assert(jfinfo->bitvClass == class);

	jflowDfiDEBUG(dbOut, "Filling Gen/Kill for %d\n", bb->label);

	/*
	 * Clear the vectors.
	 */
	bitvClearAll(class, dfFwdGen(bb));
	for (i = 0; i < bbExitC(bb); i++)
		bitvClearAll(class, dfFwdKill(bb,i));

	/*
	 * Set the information about what values control variables  receive.
	 * By construction, control variables are only ever assigned constants.
	 * So we do not need kill information from "dunno" assignments.
	 */
	bbLastValues(bb, NULL, jfinfo->locVals, jfinfo->same, jfinfo->dunno);

	for (n = 0; n < jfinfo->locCount; n++) {
		Foam vi = jfinfo->locVals->foamSeq.argv[n];

		if (!jvIsControl(jfinfo, n) || vi == jfinfo->same)
			continue;

		/* Determine the value. */
		switch (foamTag(vi)) {
		case FOAM_SInt:
			vval = vi->foamSInt.SIntData;
			break;
		case FOAM_Bool:
			vval = vi->foamBool.BoolData;
			break;
		default:
			vval = -1;
			break;
		}
		
		/* If we found the value, generate it and kill all else. */
		if (vval != -1) {
			bitvSet(class, dfFwdGen(bb), jvOrgIx(jfinfo,n) + vval);
			/* Set kill bits for other values. */
			for (i = 0; i < bbExitC(bb); i++)
				jflowSetKillAllBut(jfinfo,
						   dfFwdKill(bb,i), n, vval);
		}
		/* Otherwise all are possible. */
		else {
			int	j, n0, nL;
			n0 = jvOrgIx(jfinfo, n);
			nL = jvLimIx(jfinfo, n);
			for (j = n0; j < nL; j++) { 
				bitvSet(class, dfFwdGen(bb), j);
				for (i = 0; i < bbExitC(bb); i++)
					bitvClear(class, dfFwdKill(bb,i), j);
			}
		}
	}

	/* Ignore bblocks that aren't If or Select bblocks */
	if ((bb->kind != FOAM_If) && (bb->kind != FOAM_Select)) return;


	/* Get the control-var for this bblock */
	n = bbControlData(bb);


	/* Return if bblock has no controlling variable */
	if (n < 0) return;


	/*
	 * Set any kill information based on exit choice.
	 */
	for (i = 0; i < bbExitC(bb); i++)
		jflowSetKillAllBut(jfinfo, dfFwdKill(bb,i), n, i);

	if (DEBUG(jflowDfi)) {
		fprintf(dbOut, "Filled Gen/Kill for block:\n");
		bbPrint(dbOut, bb, true);
	}
}

local void
jflowSetKillAllBut(JFInfo jfinfo, Bitv killv, int varno, int but)
{
	int	i, iOrg, iMax;
	
	iOrg = jvOrgIx (jfinfo, varno);
	iMax = jvMaxVal(jfinfo, varno);

	for (i = 0; i <= iMax; i++)
		if (i != but) bitvSet(jfinfo->bitvClass, killv, iOrg + i);
}


/*
 * Determine whether the outcome of the test at the end of dd is determined
 * by the dataflow bits bv.
 * If the outcome is determined, the number of the branch taken is returned.
 * Otherwise, -1 is returned.
 */
local int
jflowHowIsBitvDominating(JFInfo jfinfo, Bitv bv, BBlock dd)
{
	int	cvn;
	int	n, ix0, ixN;

	cvn = bbControlVar(dd);
	if (cvn == -1) return -1;

	
	ix0 = jvOrgIx(jfinfo, cvn);
	ixN = jvLimIx(jfinfo, cvn);
	
	n   = bitvUnique1IndexInRange(jfinfo->bitvClass, bv, ix0, ixN);

	if (n == -1) return -1;
	return n - ix0;
}


/**************************************************************************
 *
 * :: jflowSpecializeByAsst -- Use local info to eliminate control nodes.
 *
 *************************************************************************/

local int
jflowSpecializeByAsst(JFInfo jfinfo, FlowGraph flog)
{
	int		i, nspec, eix;
	BBlock	bb;

	nspec = 0;
	for (i = 0; i < flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		if (!bb) continue;

		eix = jflowHowIsBitvDominating(jfinfo,dfFwdGen(bb),bb);
		if (eix == -1) continue;

		if (DEBUG(jflowDf)) {
			fprintf(dbOut, ":= := := Specializing the block\n");
			bbPrint(dbOut, bb, true);
		}

		bbSpecializeExit(bb, eix);

		if (DEBUG(jflowDf)) {
			fprintf(dbOut, ":= := := Got\n");
			bbPrint(dbOut, bb, true);
		}

		nspec++;
	}
	flogCollect(flog);

	return nspec;
}


/**************************************************************************
 *
 * :: jflowSpecializeByExit -- Use dataflow to eliminate control nodes.
 *
 *************************************************************************/

local int	jflowSpecializeByExitBB (JFInfo, FlowGraph, BBlock);


local int
jflowSpecializeByExit(JFInfo jfinfo, FlowGraph flog)
{
	int	n, k;

	/* 1. Compute the possible values of the control vars on each exit. */
	n = dflowFwdIterate(flog, DFLOW_Union, jflowDF_CUTOFF, &k, NULL);
	if (DEBUG(jflowDf)) {
		fprintf(dbOut, n == 0 ? "Converged" : "Did not converge");
		fprintf(dbOut, " after %d iterations\n", k);
		flogPrint(dbOut, flog, true);
	}
	flogClearMarks(flog);
	if (n != 0) return 0;

	/* 2. Recursive cloning traversal. */

	n = jflowSpecializeByExitBB(jfinfo, flog, flog->block0);

	/* 3. Collect nodes no longer referenced. */
	flogCollect(flog);

	return n;
}


/* This procedure is totally inefficient; must be rewrotten.
 * Futhermore, all dummy blocks must be searched before proceeding.
 */
local int
jflowSpecializeByExitBB(JFInfo jfinfo, FlowGraph flog, BBlock bb)
{
	int		i, eix, ddN, nclone = 0;
	BBlock		cc, dd;

	if (!bb || bb->mark || !bb->dfinfo) return 0;
	bb->mark = true;

	/* Clone exits if desired. */
	for (i = 0; i < bbExitC(bb); i++) {
		dd = bbExit(bb, i);

		eix = jflowHowIsBitvDominating(jfinfo,dfFwdOut(bb,i),dd);
		if (eix == -1) continue;


		/* Get the control-var for the dominated bblocks */
		ddN = bbControlData(dd);


		/* Check if the var is assigned in the block */
		if (jflowIsVarAssignedInBlock(ddN, dd))
			continue;

		jflowDfDEBUG(dbOut, "------------ Ahem!\n");

		if (foamArgc(dd->code) > jflowMaxClone) {
			jflowDfDEBUG(dbOut, "----------- TOO BIG!\n");
			continue;
		}



		/* Clone the bblock so that we can modify it */
		cc = bbCloneExitFrom(bb, i);


		/* Replace the if with a goto */
		bbSpecializeExit(cc, eix);

		if (DEBUG(jflowDf)) {
			fprintf(dbOut, "------------ Changed the old block\n");
			bbPrint(dbOut, bb, true);
			fprintf(dbOut, "------------ .. point to the block\n");
			bbPrint(dbOut, cc, true);
			fprintf(dbOut, "------------ .. instead of block\n");
			bbPrint(dbOut, dd, true);
		}

		nclone++;
	}

	/* Recurse into children. */
	for (i = 0; i < bbExitC(bb); i++)
		nclone += jflowSpecializeByExitBB(jfinfo, flog, bbExit(bb,i));

	return nclone;
}
/******************************************************************************
 *
 * :: Dummy Tests Removal.
 *
 *****************************************************************************/

#define jflowMAX_BRANCHES	20

/* Return NULL if none */
local BBlockList
jflowDummyBlocksFind(JFInfo jfinfo, FlowGraph flog)
{
	int 		cvn, maxVal, ix0, ixN, i;
	BitvClass	class = flog->bitvClass;
	BBlockList	bbDummyl = listNil(BBlock);
	

	flogFixEntries(flog);

	flogIter(flog, bb, {

		if (bbEntryC(bb) == 0) continue;

		cvn = bbControlVar(bb);
		if (cvn == -1) continue;

		maxVal = jvMaxVal(jfinfo, cvn);

		if (maxVal > jflowMAX_BRANCHES) continue;

		ix0 = jvOrgIx(jfinfo, cvn);
		ixN = jvLimIx(jfinfo, cvn);

		for (i = ix0; i < ixN; i++)
			if (!bitvTest(class, dfFwdIn(bb), i)) break;

		if (i < ixN) continue;

		listPush(BBlock, bb, bbDummyl);
	});

	return bbDummyl;
}

/* This info should be stored somewhere */
local Bool
jflowIsVarAssignedInBlock(int varno, BBlock bb)
{
	int i, j, nlhs;
	Foam 	lhs;
	Foam	* lhsv;

	for (i = 0; i < foamArgc(bb->code); i++) {
		Foam stmt = bb->code->foamSeq.argv[i];

		if (!otIsDef(stmt)) continue;

		lhs = stmt->foamDef.lhs;

		if (foamTag(lhs) == FOAM_Values) {
			nlhs = foamArgc(lhs);
			lhsv = lhs->foamValues.argv;
		}
		else {
			nlhs = 1;
			lhsv = &lhs;
		}

		for (j = 0; j < nlhs; j++) {
			if (foamTag(lhsv[j]) == FOAM_Loc &&
		    	    jflowLocIndex(lhsv[j]) == varno)
				return true;
		}
	}

	return false;
}

local void
jflowSpecializeBlock(int val, BBlock bb, int exit, BBlock dummy,
		      Bitv cloneSet, BitvClass class)
{
	BBlock	exitb = bbExit(bb, exit);
	BBlock	newb;
	int	i;

	newb = bbCloneExitFrom(bb, exit);

	if (exitb == dummy) {
		bbSpecializeExit(newb, val);
	}

	for (i = 0; i < bbExitC(newb); i++) {
		BBlock dd = bbExit(newb, i);

		if (bitvTest(class, cloneSet, dd->label))
			jflowSpecializeBlock(val, newb, i, dummy, cloneSet,
					     class);
	}
}

/* "header" is reached only by constant values of control var.
 *  NOTE: header my be reached by the some value from 2 different
 *    blocks, in example, and I don't want to copy it twice.
 */

local void
jflowBlocksClone(JFInfo jfinfo, BBlockList clones, BBlock header, BBlock dummy)
{
	BBlock	bbv [jflowMAX_BRANCHES];
	Bool	mark[jflowMAX_BRANCHES];
	int     i, j, eix;
	int	cvn = bbControlVar(dummy);
	int 	maxVal = jvMaxVal(jfinfo, cvn);
	Bitv	cloneSet;
	BitvClass class = bitvClassCreate(flogBlockC(jfinfo->flog));
	
	cloneSet = bitvNew(class);
	bitvClearAll(class, cloneSet);

	/* With cloneSet we can quickly know if a block is in clones */
	listIter(BBlock, bb, clones, {
		bitvSet(class, cloneSet, bb->label);
	});

	for (i = 0; i < maxVal; i++) {
		mark[i] = false;
		bbv[i] = NULL;
	}

	assert(bbEntryC(header)); 

	for (i = 0; i < bbEntryC(header); i++) {
		BBlock	dd = bbEntry(header, i);

		for (j = 0;  j < bbExitC(dd); j++)
			if (bbExit(dd, j) == header) break;

		assert(j < bbExitC(dd));

		eix = jflowHowIsBitvDominating(jfinfo,dfFwdOut(dd, j),dummy);

		jflowSpecializeBlock(eix, dd, j, dummy, cloneSet, class);
	}

#if 0 /* Paranoid */
	/* Count different values reaching the header */
	for (i = 0; i < bbEntryC(header); i++) {
		BBlock	dd = bbEntry(header, i);
		eix = jflowHowIsBitvDominating(jfinfo,dfFwdOut(dd),bb);
		assert(eix != -1);
		if (!mark[i]) {
			mark[i] = true;
			counter += 1;
		}
	}

	for (i = 0; i < bbEntryC(header); i++) {
		BBlock	dd = bbEntry(header, i);
		eix = jflowHowIsBitvDominating(jfinfo,dfFwdOut(dd),bb);

		/* ... */
	}
#endif
	bitvFree(cloneSet);
}

local Bool
jflowBlocksCloneIfCan(JFInfo jfinfo, Loop loop, BBlock bb, Dominators doms)
{
	BBlockList	clones = listNil(BBlock);
	int		cvn = bbControlVar(bb);
	int		size = 0;

	/* CloneSet = { b | b in loop and dom(b, bb) } */

	listIter(BBlock, b, loop->blockList, {

		if (!lpIsDom(doms, b, bb)) continue;

		if (jflowIsVarAssignedInBlock(cvn, b)) {
			listFree(BBlock)(clones);
			return false;
		}

		listPush(BBlock, b, clones);

		size += foamArgc(b->code);
	});

	/* !! NOTE: To be paranoid, size should be multiplied for the number of
	 *          copies we will do. Since usually are 2 or 3, we omit this.
	 */

	/* Too big ? */
	if (size > jflowMaxClone) {
		listFree(BBlock)(clones);
		jflowDmDEBUG(dbOut, "---dummy test: TOO BIG!\n");
		return false;
	}

	/* "clones" is the list of block that can be cloned */

	jflowBlocksClone(jfinfo, clones, loop->header, bb);

	return true;
}

/* NOTE: "loop" contains the dummy block "bb".
 * NOTE: Assume entries fixed. */
local Bool
jflowDummyRemovalTryWithLoop(JFInfo jfinfo, Loop loop, BBlock bb,
			      Dominators doms)
{
	int	i, j, eix;
	BBlock	header = loop->header;

	/* Check if all entries in header have constant value */

	/* A loop header must have at least 1 entry */
	assert(bbEntryC(header)); 

	for (i = 0; i < bbEntryC(header); i++) {
		BBlock	dd = bbEntry(header, i);

		for (j = 0;  j < bbExitC(dd); j++)
			if (bbExit(dd, j) == header) break;

		assert(j < bbExitC(dd));

		eix = jflowHowIsBitvDominating(jfinfo,dfFwdOut(dd,j),bb);
		if (eix == -1) return false;
	}

	return jflowBlocksCloneIfCan(jfinfo, loop, bb, doms);
}

local Bool
jflowLoopEq(Loop l1, Loop l2)
{
	return l1 == l2;
}

/*
 * "loops" is the list of all the loops in the flog. "bb" is the d
 * Return true if removed.
 */
local Bool
jflowDummyRemovalTryWithLoops(JFInfo jfinfo, LoopList loops, BBlock bb,
			       Dominators doms)
{
	Loop		selLoop = NULL;
	LoopList	selLoops = listNil(Loop);
 	LoopList	sl;
	int		min, n;

	listIter(Loop, loop, loops, {
		if (lpIsBlockInLoop(bb, loop))
			listPush(Loop, loop, selLoops);
	});

	while (selLoops) {
		min = 32000;
		selLoop = NULL;
		
		for (sl = selLoops; sl; sl = cdr(sl)) {
			Loop	loop = car(sl);
			n = lpNumBlocksInLoop(loop);
			if (n < min) {
				min = n;
				selLoop = loop;
			}
		}

		assert(selLoop);

		if (jflowDummyRemovalTryWithLoop(jfinfo, selLoop, bb, doms))
			return true;

		selLoops = listNRemove(Loop)(selLoops, selLoop, jflowLoopEq);
	}

	return false;
}

/* Return false if no dummy test has been removed. */
local Bool
jflowDummyTestRemove(JFInfo jfinfo, FlowGraph flog, BBlockList bbl)
{
	LoopList	loops;
	Dominators	doms;

	loops = lpNaturalLoopsFrFlog(flog, &doms);

	if (!loops) return false;

	listIter(BBlock, bb, bbl,  {
		if (jflowDummyRemovalTryWithLoops(jfinfo, loops, bb, doms))
			return true;
	});

	return false;
}


/* NOTE: This version is not really efficient. DFlow info is recomputed for
 * 	each dummy test removed.
 */
local int
jflowDummyTestsRemove(JFInfo jfinfo, FlowGraph flog)
{
	Bool		changed = false;
	BBlockList	bbDummyl;
	int		n, k;

	n = dflowFwdIterate(flog, DFLOW_Union, jflowDF_CUTOFF, &k, NULL);

	if (DEBUG(jflowDf)) {
		fprintf(dbOut, n == 0 ? "Converged" : "Did not converge");
		fprintf(dbOut, " after %d iterations\n", k);
		flogPrint(dbOut, flog, true);
	}

	flogClearMarks(flog);

	if (n != 0) return 0;

	bbDummyl = jflowDummyBlocksFind(jfinfo, flog);

	if (!bbDummyl) return 0;

	changed = jflowDummyTestRemove(jfinfo, flog, bbDummyl);	

	return changed;
}

/*===========================================================================*/

#if 0  /* DON'T REMOVE: for future use */

/******************************************************************************
 *
 * :: Propagate Induction Values
 *
 *****************************************************************************/

#define JFLOW_PHANTOM_BLOCK		1
#define JFLOW_REAL_BLOCK		2

local BBlock
jflowMakePhantomBlock(FlowGraph flog, Foam var, Foam value)
{
	Foam def = foamNewSet(var, value);
	Foam seq = foamNewEmpty(FOAM_Seq, 1);
	BBlock	newBlock;
	Length	newLab;

	newLab = flogReserveLabel(flog);

	seq->foamSeq.argv[0] = def;

	newBlock = bbNew(seq, newLab);

	newBlock->iextra = JFLOW_PHANTOM_BLOCK;

	return newBlock;
}


local Bool
jflowAddPhantomBlockFrIf(FlowGraph flog, BBlock bb)
{
	Foam var = jflowFindBBControlVar(bb);
	BBlock	falseB, trueB;

	assert(var);

	if (!otIsLocalVar(var)) return false;

	falseB= jflowMakePhantomBlock(flog, foamCopy(var), foamNewBool(false));
	trueB = jflowMakePhantomBlock(flog, foamCopy(var), foamNewBool(true));

	flogBlockInsertBtwn(flog, falseB, bb, bbExit(bb, FALSE_EXIT));
	flogBlockInsertBtwn(flog, trueB, bb,  bbExit(bb, TRUE_EXIT));

	return true;
}

local Bool
jflowAddPhantomBlockFrSelect(FlowGraph flog, BBlock bb)
{
	BBlock	newb;
	int	i;
	Foam 	code = bb->code;
	Foam    select = code->foamSeq.argv[foamArgc(code)-1];
	Foam	var = select->foamSelect.op;

	assert(var && foamTag(select) == FOAM_Select);

	if (!otIsLocalVar(var)) return false;

	for (i = 0; i < foamArgc(select); i++) {
		newb = jflowMakePhantomBlock(flog, foamCopy(var),
					     foamNewSInt(i));
		
		flogBlockInsertBtwn(flog, newb, bb, bbExit(bb, i));
	}

	return true;
}



local Bool
jflowAddPhantomBlocks(FlowGraph flog)
{
	Bool	added = false;

	flogIter(flog, bb, {

		bb->iextra = JFLOW_REAL_BLOCK;

		if (bb->kind == FOAM_If) {

			if (jflowAddPhantomBlockFrIf(flog, bb))
				added = true;
		}
		else if (bb->kind == FOAM_Select) {			
			if (jflowAddPhantomBlockFrSelect(flog, bb))
				added = true;
		}
	});

	return added;
}

/* NOTE: assumes entries fixed. 
 * NOTE: This is temporary. In the future flog will export this operation.
 */
local void
jflowRemovePhantomBlock(FlowGraph flog, BBlock bb)
{
	BBlock  fromB = bbEntry(bb, int0);
	BBlock  toB  = bbExit(bb, int0);
	int	i;

	assert(bbEntryC(bb)  == 1 &&
	       bbExitC(bb)   == 1);
	assert(fromB->iextra == JFLOW_REAL_BLOCK);
	assert(toB->iextra   == JFLOW_REAL_BLOCK);

	for (i = 0; i < bbExitC(fromB); i++)
		if (bbExit(fromB, i) == bb) break;

	assert(i < bbExitC(fromB));

	bbSetExit(fromB, i, toB);

	bbFree(bb);
}

local void
jflowRemovePhantomBlocks(FlowGraph flog)
{
	flogIter(flog, bb, {

		assert(bb->iextra == JFLOW_PHANTOM_BLOCK ||
		       bb->iextra == JFLOW_REAL_BLOCK);

		if (bb->iextra == JFLOW_PHANTOM_BLOCK)
			jflowRemovePhantomBlock(flog, bb);
	});

}

local Bool
jflowPropagateInductionVars(FlowGraph flog)
{
	Bool result;

	if (!jflowAddPhantomBlocks(flog))
		return false;

	result = cpFlog(flog);

	flogFixEntries(flog);

	jflowRemovePhantomBlocks(flog);

	return result;
}

#endif

local Bool
jflowIsNegatedLocal(Foam val)
{
	FoamBValTag	tag;

	if (foamTag(val) != FOAM_BCall) return false;
	tag = (FoamBValTag)(val->foamBCall.op);
	return (tag == FOAM_BVal_BoolNot);
}


/* Allow users to turn off experimental optimisation */
static Bool jflowNegateFlag = true;


Bool
jflowCanNegate(void)
{
	return jflowNegateFlag;
}


void
jflowSetNegate(Bool canNegate)
{
	jflowNegateFlag = canNegate;
}


