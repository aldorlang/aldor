/*****************************************************************************
 *
 * of_killp.c: Pointer crushing
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 ****************************************************************************
 * WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING
 * WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING
 * WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING
 ****************************************************************************
 * This code is still quite buggy and seems to interfere with TextWriter
 * output. Compiling the test suite with -Qkillp highlights tests that
 * fail with this enabled. It was written as a quick experiment and ought
 * not to be used for serious work. At the moment it can only be enabled
 * by explicitly typing -Qkillp: don't allow it to be included when the
 * user types -Q4 or something similar.
 ****************************************************************************
 * WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING
 * WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING
 * WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING  WARNING
 ****************************************************************************
 *
 * Pointer crushing: ensure that temporaries are reset to a neutral
 * value after their last use. This prevents the GC from using them
 * as roots because it doesn't know any better.
 *
 * Refer to of_deada.c for more details - most of this code is
 * exactly the same as in that file (that's where it came from ;)
 *
 * We apply the following algorithm to each prog in turn: flatten
 * the prog so that the arguments to all eligible calls are either
 * literals or variables. Then we dataflow to identify killable
 * references before generating the kills. Eligible calls are those
 * which are top-level calls and those on the RHS of a Set.
 *
 * To do:
 *  - Only flatten calls which contain killable references. An initial
 *    dataflow pass is required for this.
 *  - Consider LHS of Sets: deal with (Values)
 *  - Consider If, Repeat etc.
 */

#include "axlobs.h"
#include "debug.h"
#include "dflow.h"
#include "flog.h"
#include "format.h"
#include "of_deada.h"
#include "of_killp.h"
#include "of_util.h"
#include "opttools.h"
#include "optfoam.h"
#include "syme.h"
#include "table.h"
#include "strops.h"

Bool	kpDebug		= false;

#define kpDEBUG		DEBUG_IF(kpDebug)

/****************************************************************************
 *
 * Type definitions
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Global Data Structures
 *
 ****************************************************************************/

static int 		kpFlogCutOff = 700;

static BitvClass	kpBitvClass;
static FlowGraph	kpThisFlog;

static Table 	kpKillTbl;
static Table 	kpUseTbl;
static Bitv  	kpLive;
static Bitv  	kpOut;

#ifdef NEW_FORMATS
static Foam *faParamsv;
#endif

/****************************************************************************
 *
 * Local declarations
 *
 ****************************************************************************/

local Foam	kpProg			(Foam);
local Bool	kpFlog0			(int, FlowGraph);
local void	kpFillDefRef		(FlowGraph, BBlock);
local void	kpFillStmt		(Foam, BBlock);
local void	kpFillExpr		(Foam, BBlock);
local void	kpFixBBlock		(BBlock);
local void	kpFixSet		(Bitv, Foam);
local void	kpFixExpr		(Bitv, Foam);

local void	kpKillBBlock		(BBlock);
local void	kpProcessExpr		(int, Foam);
local void	kpProcessSet		(int, Foam);
local void	kpMarkAsDead		(Foam, int);
local AIntList	kpKillEventsAfter	(int, AInt);
local AIntList	kpUseEventsAfter	(int, AInt);
local AIntList	kpEventsAfter		(int, AIntList l);

local void	kpInitTempSets		(BBlock);
local void	kpFillSetsExpr		(int, Foam);
local void	kpFillKillSet		(int, Foam);
local void	kpFiniTempSets		(void);

local void	kpPreprocessProg	(Foam);
local void	kpPostprocessProg	(Foam);

local int kpBitvSize	(Foam prog);
local int kpBitvIndex	(Foam foam);

/****************************************************************************
 *
 * :: External functions
 *
 ****************************************************************************/

void
kpSetCutOff(int n)
{
	kpFlogCutOff = n;
}

void
killPointers(Foam unit)
{
	Foam ddef, def, rhs;
	int  i;
	Length conc;
	Foam *conv;
	DEBUG_DECL(Foam lhs);
	DEBUG_DECL(int j);
	DEBUG_DECL(String progName);

	conc = foamDDeclArgc(foamUnitConstants(unit));
	conv = foamUnitConstants(unit)->foamDDecl.argv;

#ifdef NEW_FORMATS
	faParamsv = foamUnitParams(unit)->foamDDecl.argv;
#endif

	ddef = unit->foamUnit.defs;
	for (i = 0; i<foamArgc(ddef); i++) {
		def = ddef->foamDDef.argv[i];
		assert(foamTag(def) == FOAM_Def);

		kpDEBUG {
			lhs = def->foamDef.lhs;
			j = lhs->foamConst.index;
			if (j < conc)
				progName = conv[j]->foamDecl.id;
			else
				progName = "???";
			fprintf(dbOut, "Function: %s\n", progName);
		}

		rhs = def->foamDef.rhs;
		if (foamTag(rhs) == FOAM_Prog) 
			def->foamDef.rhs = kpProg(rhs);
	}
	assert(foamAudit(unit));
}

void
killProgPointers(Foam prog)
{
	assert(foamTag(prog) == FOAM_Prog);
	kpDEBUG{fprintf(dbOut, "Function: %s\n", "<unknown>");}
	kpProg(prog);
}

/****************************************************************************
 *
 * :: Simple accessors
 *
 ****************************************************************************/

local int
kpBitvSize(Foam prog)
{
	return foamDDeclArgc(prog->foamProg.locals)
		+ foamDDeclArgc(prog->foamProg.params);
}

local int
kpBitvIndex(Foam foam)
{
	switch (foamTag(foam)) {
	  case FOAM_Loc:
		return foam->foamLoc.index;
	  case FOAM_Par:
		return foam->foamPar.index + 
			foamDDeclArgc(kpThisFlog->prog->foamProg.locals);
	  default:
		return -1;
	}
}

/****************************************************************************
 *
 * Compute and kill...
 *
 ****************************************************************************/

local Foam
kpProg(Foam prog)
{
	FlowGraph flog;
	Foam 	  locals, ret;
	int 	  nbits;
	
	kpDEBUG {
		fprintf(dbOut, "--> Before:\n");
		foamWrSExpr(dbOut, prog,int0);
	}

	kpPreprocessProg(prog);

	locals = prog->foamProg.locals;

	nbits  = kpBitvSize(prog);

	if (!nbits) return prog;

	flog = flogFrProg(prog, FLOG_UniqueExit);
	
	kpFlog0(nbits, flog);

	ret = flogToProg(flog);
	kpPostprocessProg(ret);

	kpDEBUG {
		fprintf(dbOut, "<-- After:\n");
		foamWrSExpr(dbOut, ret,int0);
		fnewline(dbOut);
	}
	kpBitvClass = NULL;
	return ret;
}

local Bool
kpFlog0(int nLocals, FlowGraph flog)
{	
	BBlock bb;
	int    i, count;

	kpBitvClass = bitvClassCreate(nLocals);
	flogBitvClass(flog) = kpBitvClass;
	kpThisFlog = flog;

	for (i=0; i<flogBlockC(flog); i++) {
		bb = flogBlock(flog, i);
		dflowNewBlockInfo(bb, nLocals, kpFillDefRef);
	}

	i = dflowRevIterate(flog, DFLOW_Union, kpFlogCutOff, &count, NULL);

	if (i != 0) 
		return false;

	for (i=0; i<flogBlockC(flog); i++) {
		if (!flogBlock(flog,i)) continue;
		kpFixBBlock(flogBlock(flog, i));
	}

	kpThisFlog = NULL;
	return true;
}

local void
kpFillDefRef(FlowGraph flog, BBlock bb)
{
	Foam 	  seq = bb->code;
	Foam 	  stmt;
	BitvClass class = flogBitvClass(flog);
	int 	  i;

	/* clear the vectors */
	bitvClearAll(class, dfRevKill(bb));
	bitvClearAll(class, dfRevGen(bb));

	for (i=0; i<foamArgc(seq); i++) {
		stmt = seq->foamSeq.argv[i];
		kpFillStmt(stmt, bb);
	}
}	

local void
kpFillStmt(Foam stmt, BBlock bb)
{
	Foam *argv;
	int   idx, argc, i;

	if (!otIsDef(stmt)) {
		kpFillExpr(stmt, bb);
		return;
	}
		
	kpFillExpr(stmt->foamSet.rhs, bb);

	switch(foamTag(stmt->foamSet.lhs)) {
	  case FOAM_Values:
		argv = stmt->foamSet.lhs->foamValues.argv;
		argc = foamArgc(stmt->foamSet.lhs);
		break;
	  default:
		argv = &stmt->foamSet.lhs;
		argc = 1;
		break;
	}
		
	for (i=0; i<argc; i++) {
		idx = kpBitvIndex(argv[i]);
		if (idx != -1) {
			if (!bitvTest(kpBitvClass, dfRevGen(bb), idx)) 
				bitvSet(kpBitvClass, dfRevKill(bb), idx);
		} else
			kpFillExpr(argv[i], bb);
	}
}

local void
kpFillExpr(Foam foam, BBlock bb)
{
	int idx;

	idx = kpBitvIndex(foam);
	if (idx != -1) {
		if (!bitvTest(kpBitvClass, dfRevKill(bb), idx))
			bitvSet(kpBitvClass, dfRevGen(bb), idx);
	}
	else
		foamIter(foam, arg, kpFillExpr(*arg, bb));
}

local void
kpFixBBlock(BBlock bb)
{
	Foam seq = bb->code;
	Foam stmt;
	Bitv bitv;
	int i;

	/* Do dataflow by hand backwards through the BB */
	/* Start with the 'out' set and work upwards    */
	/* when complete, out will be a subset of "in" */
	kpDEBUG {
		fprintf(dbOut, "(Fixing: Out\n");
		bitvPrint(dbOut, kpBitvClass, dfRevOut(bb));
		fprintf(dbOut, "\nIn\n");
		bitvPrint(dbOut, kpBitvClass, dfRevIn(bb));
		fprintf(dbOut, "\nFoam\n");
		foamWrSExpr(dbOut, seq, int0);
	}
	bitv = bitvNew(kpBitvClass);
	bitvCopy(kpBitvClass, bitv, dfRevOut(bb));
	
	for (i=foamArgc(seq) - 1; i >= 0 ; i--) {
		stmt = seq->foamSeq.argv[i];

		if (otIsDef(stmt)) 
			kpFixSet(bitv, stmt);
		else
			kpFixExpr(bitv, stmt);
	}

	/*
	 * This is the part that we have been waiting for. All
	 * the stuff before this call is to compute whether or
	 * not locals are live at the BB entry and exit.
	 */
	kpKillBBlock(bb);

	kpDEBUG {
		bitvPrint(dbOut, kpBitvClass, bitv);
		fprintf(dbOut, "\nDone fix)\n");
	}

}

local void
kpFixSet(Bitv bitv, Foam set)
{
	Foam lhs = set->foamSet.lhs;
	Foam rhs = set->foamSet.rhs;
	Foam *argv;
	int   argc;
	Bool  isLive;
	int   i, idx;

	switch(foamTag(lhs)) {
	  case FOAM_Values:
		argv = lhs->foamValues.argv;
		argc = foamArgc(lhs);
		break;
	  default:
		argv = &lhs;
		argc = 1;
		break;
	}

	isLive = false;
	for (i=0; i<argc; i++) {
		idx = kpBitvIndex(argv[i]);
		if (idx == -1) {
			isLive = true;
			kpFixExpr(bitv, argv[i]);
		}
		else {
			isLive |= bitvTest(kpBitvClass, bitv, idx);
			bitvClear(kpBitvClass, bitv, idx);
		}
	}

	if (isLive || (foamTag(rhs) == FOAM_MFmt))
		kpFixExpr(bitv, rhs); 
	else
	{
		if (foamHasSideEffect(rhs))
			kpFixExpr(bitv, rhs);
	}
}

local void
kpFixExpr(Bitv bitv, Foam expr)
{
	int idx;

	idx = kpBitvIndex(expr);
	if (idx != -1)
		bitvSet(kpBitvClass, bitv, idx);
	else
		foamIter(expr, arg, kpFixExpr(bitv, *arg));
}


/****************************************************************************
 *
 * The core of the optimisation: determine which locals are dead.
 *
 ****************************************************************************/

local void
kpKillBBlock(BBlock bb)
{
	Foam seq, stmt;
	int i;

	/* Compute uses and kills of all locals */
	kpInitTempSets(bb);


	seq = bb->code;
	for (i=0; i<foamArgc(seq); i++) {
		stmt = seq->foamSeq.argv[i];

		switch (foamTag(stmt)) {
		  case FOAM_Def:
		  case FOAM_Set:
			kpProcessSet(i, stmt);
			break;
		  default:
			kpProcessExpr(i, stmt);
		}
	}

	kpFiniTempSets();
}

local void
kpProcessExpr(int stmtId, Foam expr)
{
	AIntList kills, uses;
	Bool     liveAfter;
	int 	 id;

	foamIter(expr, arg, kpProcessExpr(stmtId, *arg));

	if (foamTag(expr) != FOAM_Loc) return;

	id = kpBitvIndex(expr);

	if (bitvTest(kpBitvClass, kpLive, id))
	{
		/* Determine if this is the last use of this local */
		uses  = kpUseEventsAfter(stmtId + 1, id);
		kills = kpKillEventsAfter(stmtId, id);
		
		liveAfter = bitvTest(kpBitvClass, kpOut, id);
		
		kpDEBUG {
			fprintf(dbOut,
				"Checking %d var: %d next use: %d next kill: %d  used later: %s\n", 
				stmtId, id,
				uses ? (int)car(uses): 999999, 
				kills ? (int)car(kills): 999999, 
				liveAfter ? "Yes" : "No");
		}

		if (!uses && !liveAfter)
			kpMarkAsDead(expr, id);
		else if (uses && kills && (car(uses) > car(kills)))
			kpMarkAsDead(expr, id);
	}
}

local void
kpProcessSet(int stmtId, Foam set)
{
	AInt	id;
	Foam	lhs = set->foamSet.lhs;
	Foam	rhs = set->foamSet.rhs;

	kpProcessExpr(stmtId, rhs);

	if (foamTag(lhs) == FOAM_Loc)
	{
		/* Mark as live (could tighten this up) */
		id = lhs->foamLoc.index;
		bitvSet(kpBitvClass, kpLive, id);
	}
	else
		kpProcessExpr(stmtId, lhs);
}

local void
kpMarkAsDead(Foam foam, int id)
{
	foamDvMark(foam) = 1;
	bitvClear(kpBitvClass, kpLive, id);
	kpDEBUG {
		fprintf(dbOut, "   Killable:  ");
		foamPrintDb(foam);
	}
}

local AIntList
kpKillEventsAfter(int stmt, AInt id)
{
	AIntList kills;
	kills = (AIntList) tblElt(kpKillTbl, (TblKey) id,
				  (TblElt) listNil(AInt));
	return kpEventsAfter(stmt, kills);
}

local AIntList
kpUseEventsAfter(int stmt, AInt id)
{
	AIntList uses;
	uses = (AIntList) tblElt(kpUseTbl, (TblKey) id,
				  (TblElt) listNil(AInt));

	uses = kpEventsAfter(stmt, uses);
	return uses;
}


local AIntList
kpEventsAfter(int id, AIntList l)
{
	while (l && car(l) < (AInt) id)
		l = cdr(l);
	return l;
}

/****************************************************************************
 *
 * Pre- and post-processing of FOAM progs
 *
 ****************************************************************************/

local FoamList	kpFlattenSetOrDef(Foam);
/* local FoamList	kpFlattenBCall(Foam); */
local FoamList	kpFlattenCCall(Foam);
local FoamList	kpFlattenOCall(Foam);
local FoamList	kpFlattenPCall(Foam);
local FoamList	kpFlattenCall(int, Foam *);
local FoamList	kpFlattenReturn(Foam);
local FoamList	kpKillsForSetOrDef(Foam);
/* local FoamList	kpKillsForBCall(Foam); */
local FoamList	kpKillsForCCall(Foam);
local FoamList	kpKillsForOCall(Foam);
local FoamList	kpKillsForPCall(Foam);
local FoamList	kpKillsForCall(int, Foam *);
local Foam	kpNewTemporary(AInt);

static AInt	kpNextTemp;
static AIntList kpTemporaries;

local void
kpPreprocessProg(Foam prog)
{
	/*
	 * Flatten calls and strip out any existing Kills - they were
	 * probably put there by us during a previous pass over the
	 * FOAM and don't want to have an explosion of them. 
	 * Notes: *Calls might be buried inside casts ...
	 */
	AInt		i, ntemps;
	Foam		foam = prog->foamProg.body;
	Length		locc, parc;
	Foam		*locv, *parv;
	FoamList	seqbody, extra;

	assert(foamTag(foam) == FOAM_Seq);

	locc = foamDDeclArgc(prog->foamProg.locals);
	locv = prog->foamProg.locals->foamDDecl.argv;

#ifdef NEW_FORMATS
	parc = foamDDeclArgc(faParamsv[prog->foamProg.params-1]);
	parv = (faParamsv[prog->foamProg.params-1])->foamDDecl.argv;
#else
	parc = foamDDeclArgc(prog->foamProg.params);
	parv = prog->foamProg.params->foamDDecl.argv;
#endif

	/* Prepare the way of the addition of new locals */
	/* !!! Will use fbox when we figure out what it does */
	kpNextTemp    = locc;
	kpTemporaries = listNil(AInt);


	/* Start with an empty list of statements */
	seqbody = listNil(Foam);


	kpDEBUG {
		fprintf(dbOut, "------------------------ (before flattening) --------------------\n");
		foamPrintDb(foam);
		fprintf(dbOut, "-----------------------------------------------------------------\n\n");
	}

	/* Construct a new sequence based on the original */
	for (i = 0;i < foamArgc(foam); i++)
	{
		Foam stmt = foam->foamSeq.argv[i];

		switch (foamTag(stmt))
		{
		   case FOAM_Def:
			/* Fall through */
		   case FOAM_Set:
			extra = kpFlattenSetOrDef(stmt);
			break;
/***************************************
		   case FOAM_BCall:
			extra = kpFlattenBCall(stmt);
			break;
 ***************************************/
		   case FOAM_CCall:
			extra = kpFlattenCCall(stmt);
			break;
		   case FOAM_OCall:
			extra = kpFlattenOCall(stmt);
			break;
		   case FOAM_PCall:
			extra = kpFlattenPCall(stmt);
			break;
		   case FOAM_Return:
			extra = kpFlattenReturn(stmt);
			break;
		   case FOAM_Loose:
			stmt = foamNewNOp();
			/* Fall through */
		   default:
			seqbody = listCons(Foam)(stmt, seqbody);
			continue;
		}


		/* Add in the extra statements (if any) */
		seqbody = listNConcat(Foam)(extra, seqbody);


		/* Add in the original statement */
		seqbody = listCons(Foam)(stmt, seqbody);
	}


	/* Put the statements back in the correct order */
	seqbody = listNReverse(Foam)(seqbody);


	/* Create a new sequence */
	foam = foamNewOfList(FOAM_Seq, seqbody);


	kpDEBUG {
		fprintf(dbOut, "----------------------- (after flattening) ----------------------\n");
		foamPrintDb(foam);
		fprintf(dbOut, "-----------------------------------------------------------------\n\n");
	}


	/* Release our temporary storage */
	listFree(Foam)(seqbody);


	/* Update the prog with its new body */
	prog->foamProg.body = foam;


	/* Now deal with the addition of new locals (if any) */
	ntemps = listLength(AInt)(kpTemporaries);
	if (ntemps)
	{
		Foam ddecl = foamNewEmpty(FOAM_DDecl, 1 + locc + ntemps);


		/* Define the usage of these declarations */
		ddecl->foamDDecl.usage = FOAM_DDecl_Local;


		/* Copy over the existing locals */
		for (i = 0;i < locc; i++)
			ddecl->foamDDecl.argv[i] = locv[i];


		/* Put the new locals in the correct order */
		kpTemporaries = listNReverse(AInt)(kpTemporaries);


		/* Add in the new locals */
		i = locc;
		listIter(AInt, type, kpTemporaries, {
			String	id = strCopy("");
			Foam	decl = foamNewDecl(type, id, emptyFormatSlot);

			ddecl->foamDDecl.argv[i++] = decl;
		});


		/* Replace the existing locals declaration */
		prog->foamProg.locals = ddecl;
	}
}

local FoamList
kpFlattenSetOrDef(Foam foam)
{
	Foam rhs = foam->foamSet.rhs; /* Assumes Def/Set are similar */

	/* Skip past any casts */
	while (foamTag(rhs) == FOAM_Cast)
		rhs = rhs->foamCast.expr;

	switch (foamTag(rhs))
	{
/***************************************
	   case FOAM_BCall:
		return kpFlattenBCall(rhs);
 ***************************************/
	   case FOAM_CCall:
		return kpFlattenCCall(rhs);
	   case FOAM_OCall:
		return kpFlattenOCall(rhs);
	   case FOAM_PCall:
		return kpFlattenPCall(rhs);
	   default:
		break;
	}

	return listNil(Foam);
}


#if 0
local FoamList
kpFlattenBCall(Foam call)
{
	int	argc  = foamArgc(call) - 1;
	Foam	*argv = call->foamBCall.argv;

	return kpFlattenCall(argc, argv);
}
#endif

local FoamList
kpFlattenCCall(Foam call)
{
	int	argc  = foamArgc(call) - 2;
	Foam	*argv = call->foamCCall.argv;

	return kpFlattenCall(argc, argv);
}

local FoamList
kpFlattenOCall(Foam call)
{
	int	argc  = foamArgc(call) - 3;
	Foam	*argv = call->foamOCall.argv;

	return kpFlattenCall(argc, argv);
}

local FoamList
kpFlattenPCall(Foam call)
{
	int	argc  = foamArgc(call) - 3;
	Foam	*argv = call->foamPCall.argv;

	return kpFlattenCall(argc, argv);
}

local FoamList
kpFlattenCall(int argc, Foam argv[])
{
	int		i;
	AInt		type;
	Foam		tmp, set;
	FoamList	extra, result = listNil(Foam);

	for (i = 0; i < argc; i++)
	{
		Foam *slot	= &argv[i];
		Foam arg	= *slot;


		/* Skip past any casts */
		while (foamTag(arg) == FOAM_Cast)
		{
			slot = &(arg->foamCast.expr);
			arg  = *slot;
		}

		switch (foamTag(arg))
		{
/***************************************
		   case FOAM_BCall:
			extra = kpFlattenBCall(arg);
			type  = (AInt)(foamBValRetType(arg->foamBCall.op));
			break;
 ***************************************/
		   case FOAM_CCall:
			extra = kpFlattenCCall(arg);
			type  = arg->foamCCall.type;
			break;
		   case FOAM_OCall:
			extra = kpFlattenOCall(arg);
			type  = arg->foamCCall.type;
			break;
		   case FOAM_PCall:
			extra = kpFlattenPCall(arg);
			type  = arg->foamCCall.type;
			break;
		   default:
			/* Ignored - try next argument */
			continue;
		}


		/* Extra statements to add to our list? */
		if (extra)
			result = listNConcat(Foam)(extra, result);


		/* Create a temporary for the result of this call */
		tmp = kpNewTemporary(type);


		/* Create the set for the call */
		set = foamNewSet(tmp, arg);


		/* Add to the list of statements */
		result = listCons(Foam)(set, result);


		/* Replace the call site with the temporary */
		/* argv[i] = foamCopy(tmp); */
		*slot = foamCopy(tmp);
	}

	return result;
}

local FoamList
kpFlattenReturn(Foam ret)
{
	Foam foam = ret->foamReturn.value;

	switch (foamTag(foam))
	{
/***************************************
	   case FOAM_BCall:
		return kpFlattenBCall(foam);
 ***************************************/
	   case FOAM_CCall:
		return kpFlattenCCall(foam);
	   case FOAM_OCall:
		return kpFlattenOCall(foam);
	   case FOAM_PCall:
		return kpFlattenPCall(foam);
	   default:
		break;
	}

	return listNil(Foam);
}

local void
kpPostprocessProg(Foam prog)
{
	/*
	 * Add Kill instructions where required.
	 *
	 * Things to be aware of:
	 *  - locals used in Return are marked as killable. This
	 *    means that statements of the form (Return (Loc 0))
	 *    must not generate a kill for (Loc 0). However, if
	 *    the return value is a call then the arguments to
	 *    that call could be killed.
	 *  - parameters are not marked as killable yet
	 *  - CCalls might be buried inside casts ...
	 */
	AInt		i;
	Foam		foam = prog->foamProg.body;
	FoamList	seqbody, extra;

	assert(foamTag(foam) == FOAM_Seq);


	/*
	 * Process the flattened sequence for killable locals. Note
	 * that we only examine Set and *Call instructions - any other
	 * type of statement that contained killable references will
	 * have been converted into a Set.
	 */
	seqbody = listNil(Foam);
	for (i = 0;i < foamArgc(foam); i++)
	{
		Foam stmt = foam->foamSeq.argv[i];

		switch (foamTag(stmt))
		{
		   case FOAM_Def:
			/* Fall through */
		   case FOAM_Set:
			extra = kpKillsForSetOrDef(stmt);
			break;
/***************************************
		   case FOAM_BCall:
			extra = kpKillsForBCall(stmt);
			break;
 ***************************************/
		   case FOAM_CCall:
			extra = kpKillsForCCall(stmt);
			break;
		   case FOAM_OCall:
			extra = kpKillsForOCall(stmt);
			break;
		   case FOAM_PCall:
			extra = kpKillsForPCall(stmt);
			break;
		   default:
			extra = listNil(Foam);
			break;
		}


		/* Add the statement */
		seqbody = listCons(Foam)(stmt, seqbody);


		/* Add any kills */
		listIter(Foam, killstmt, extra, {
			seqbody = listCons(Foam)(killstmt, seqbody);});
	}


	/* Put the statements back in the correct order */
	seqbody = listNReverse(Foam)(seqbody);


	/* Create a new sequence */
	foam = foamNewOfList(FOAM_Seq, seqbody);


	kpDEBUG {
		fprintf(dbOut, "-------------------------- (after kills) ------------------------\n");
		foamPrintDb(foam);
		fprintf(dbOut, "-----------------------------------------------------------------\n\n");
	}


	/* Release our temporary storage */
	listFree(Foam)(seqbody);


	/* Update the prog with its new body */
	prog->foamProg.body = foam;
}

local FoamList
kpKillsForSetOrDef(Foam foam)
{
	Foam rhs = foam->foamSet.rhs; /* Assumes Def/Set are similar */

	/* Skip past any casts */
	while (foamTag(rhs) == FOAM_Cast)
		rhs = rhs->foamCast.expr;

	switch (foamTag(rhs))
	{
/***************************************
	   case FOAM_BCall:
		return kpKillsForBCall(rhs);
 ***************************************/
	   case FOAM_CCall:
		return kpKillsForCCall(rhs);
	   case FOAM_OCall:
		return kpKillsForOCall(rhs);
	   case FOAM_PCall:
		return kpKillsForPCall(rhs);
	   case FOAM_Loc:
		if (foamDvMark(rhs))
		{
			Foam tmp = foamNewLoose(foamCopy(rhs));
			return listCons(Foam)(tmp, listNil(Foam));
		}
		break;
	   default:
		break;
	}

	return listNil(Foam);
}

#if 0
local FoamList
kpKillsForBCall(Foam call)
{
	int	argc  = foamArgc(call) - 1;
	Foam	*argv = call->foamBCall.argv;

	return kpKillsForCall(argc, argv);
}
#endif

local FoamList
kpKillsForCCall(Foam call)
{
	int	argc  = foamArgc(call) - 2;
	Foam	*argv = call->foamCCall.argv;

	return kpKillsForCall(argc, argv);
}

local FoamList
kpKillsForOCall(Foam call)
{
	int	argc  = foamArgc(call) - 3;
	Foam	*argv = call->foamOCall.argv;

	return kpKillsForCall(argc, argv);
}

local FoamList
kpKillsForPCall(Foam call)
{
	int	argc  = foamArgc(call) - 3;
	Foam	*argv = call->foamPCall.argv;

	return kpKillsForCall(argc, argv);
}

local FoamList
kpKillsForCall(int argc, Foam argv[])
{
	int	i;
	FoamList result = listNil(Foam);

	for (i = 0;i < argc;i++)
	{
		Foam tmp, arg = argv[i];

		/* Skip past any casts */
		while (foamTag(arg) == FOAM_Cast)
			arg = arg->foamCast.expr;


		/* Only interested in top-level locals */
		if (foamTag(arg) != FOAM_Loc) continue;


		/* Only interested in killable locals */
		if (!foamDvMark(arg)) continue;


		/* Create a new kill and add to the list */
		tmp    = foamNewLoose(foamCopy(arg));
		result = listCons(Foam)(tmp, result);
	}

	/* Return the list of kills at this level */
	return result;
}

local Foam
kpNewTemporary(AInt type)
{
	/* Add the details of this temporary to our database */
	kpTemporaries = listCons(AInt)(type, kpTemporaries);


	/* Return the new temporary */
	return foamNewLoc(kpNextTemp++);
}


/****************************************************************************
 *
 * Initialisation for killing in bblocks
 *
 ****************************************************************************/

local void
kpInitTempSets(BBlock bb)
{
	Foam seq, stmt;
	int i;

	kpKillTbl   = tblNew(NULL, NULL);
	kpUseTbl    = tblNew(NULL, NULL);

	seq = bb->code;
	/* First find all uses and kills */
	for (i=foamArgc(seq)-1; i>=0; i--) {
		stmt = seq->foamSeq.argv[i];
		kpFillSetsExpr(i, stmt);
	}
	
	/* Current live variables */
	kpLive = dfRevIn(bb);
	kpOut  = dfRevOut(bb);
}

local void
kpFillSetsExpr(int stmtId, Foam foam)
{
	AIntList ids;
	AInt     loc;

	switch (foamTag(foam)) {
	  case FOAM_Set:
	  case FOAM_Def:
		kpFillKillSet(stmtId, foam->foamDef.lhs);
		foam = foam->foamDef.rhs;
	  default:
		break;
	}

	foamIter(foam, arg, kpFillSetsExpr(stmtId, *arg));

	
	if (foamTag(foam) != FOAM_Loc)
		return;

	loc = foam->foamLoc.index;
	ids = (AIntList) tblElt(kpUseTbl, (TblKey) loc, listNil(AInt));

	tblSetElt(kpUseTbl, (TblKey) loc, 
		  (TblElt) listCons(AInt)(stmtId, ids));
}

local void
kpFillKillSet(int stmtId, Foam foam)
{
	Foam *argv;
	AIntList ids;
	AInt 	 loc;
	int   i, argc;

	switch (foamTag(foam)) {
	  case FOAM_Values:
		argv = foam->foamValues.argv;
		argc = foamArgc(foam);
		break;
	  default:
		argv = &foam;
		argc = 1;
	}

	for (i = 0; i<argc; i++) {
		if (foamTag(argv[i]) == FOAM_Loc) 
		{
			loc = argv[i]->foamLoc.index;
			ids = (AIntList) tblElt(kpKillTbl, (TblElt)loc,
						listNil(AInt));
			tblSetElt(kpKillTbl, (TblKey) loc, 
				  (TblElt) listCons(AInt)(stmtId, ids));
		}
		else
			kpFillSetsExpr(stmtId, argv[i]);
	}
}

local void
kpFiniTempSets()
{

}

