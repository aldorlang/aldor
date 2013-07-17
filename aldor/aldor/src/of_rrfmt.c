
/*****************************************************************************
 *
 * of_rrfmt.c: Dynamic format (RRFmt) optimizations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*****************************************************************************
 * Dynamic formats are constants whose value can only be computed when the
 * target program is executed, not at compile time. Although they are cheap
 * to compute, dynamic formats need heap storage and tend to populate loop
 * bodies rather than dominent blocks. As a result they are rarely available
 * for optimisations such as common sub-expression elimination and their
 * runtime computation prevents them being cfolded away. Repeatedly computing
 * their value seriously degrades performance and churns heap memory.
 *
 * The semantics of raw records are as follows:
 *   - an RRNew returns a pointer to an area of memory that will be used
 *     to hold N elements of non-uniform data.
 *   - an RRElt returns a pointer into a block of memory representing the
 *     specified field of a raw record. Usually this pointer lies within
 *     a block of memory created by RRNew.
 *
 ****************************************************************************
 * FIXME:
 * - free bitv and bivClass objects
 * - free any other objects we leave lying around
 ****************************************************************************/

#include "debug.h"
#include "of_rrfmt.h"

/*****************************************************************************
 *
 * :: Debugging
 *
 ****************************************************************************/

Bool    rrfmtDebug       = false;
#define rrfmtDEBUG(s)    DEBUG_IF(rrfmtDebug, s)

/*****************************************************************************
 *
 * :: Local macros
 *
 ****************************************************************************/

#define	rrDF_CUTOFF	(200)

/*****************************************************************************
 *
 * :: Local state
 *
 ****************************************************************************/

static	rrEmergeInfo	rrInfo;

/*****************************************************************************
 *
 * :: Local function declarations
 *
 ****************************************************************************/

local	Foam		rrProgEmerge		(Foam /* prog */);
local	Bool		rrFlogEmerge		(FlowGraph, Foam /* prog */);
local	AInt		rrCountDefs		(BBlock);
local	void		rrFindEscapes		(BBlock);
local	void		rrFindDefEscapes	(Foam);
local	void		rr0FindDefEscapes	(Foam, Foam, Foam);
local	void		rrDefEscapes		(Foam);
local	void		rrDefNotEscapes		(Foam);
local	void		rrFindDeepEscapes	(Foam);
local	void		rrReconcileLocal	(Foam);
local	void		rrReturnEscapes		(Foam);
local	void		rrBCallEscapes		(Foam);
local	void		rrCCallEscapes		(Foam);
local	void		rrOCallEscapes		(Foam);
local	void		rrPCallEscapes		(Foam);
local	void		rrCallArgsEscape	(Foam *, AInt);
local	void		rrLocalEscapes		(Foam);
local	void		rr0LocalEscapes		(Foam, Bitv, Bitv, Bitv);
local	Bool		rrNonEscapingLocal	(Foam);
local	Bool		rr0NonEscapingLocal	(Foam, Bitv, Bitv, Bitv);
local	void		rrReplace		(BBlock);
local	Foam		rrReplaceDef		(Foam, FoamList *);
local	Foam		rrReplaceDeeply		(Foam);
local	FoamTag		rrSizeToType		(Foam);
local	FoamTag		rrTypeToType		(Foam);
local	AInt		rrNewRecordFmt		(FoamTag, AInt);
local	void		rrInitFormats		(Foam /* unit */);
local	AInt		rrAddFormat		(Foam /* ddecl */);
local	void		rrMakeNewFormats	(Foam /* unit */);
local	AInt		rrLookupRRFmt		(Foam /* rrfmt */);
local	void		rrCacheRRFmt		(Foam /* rrfmt */, AInt);
local	AInt		rrTranslateRRFmt	(Foam /* rrfmt */);
local	Bool		rrFoamHasRRFmt		(Foam);
local	Bool		rrIndepExpr		(Foam);

/*****************************************************************************
 *
 * :: Top-level entry points.
 *
 ****************************************************************************/


/*
 * This optimisation attempts to replace raw records with plain records
 * in such a way that raw record semantics are preserved. The solution
 * adopted here is to replace non-escaping raw records with plain records
 * whose fields are pointers to individual records of the correct size.
 * Then all RRElt instructions operating over non-escaping raw records
 * can be replaced with RElt instructions.
 *
 * To do: allow raw records to be placed in globals, lexicals and parameters
 * if we can show that these definitions do not escape. Raw records that are
 * written to records and arrays can be ignored: if they don't escape then
 * the emerger ought to replace them with locals that we can examine.
 *
 * To do: replace ALL non-escaping raw records with plain records. At the
 * moment we only consider raw records whose format can be computed at
 * compile time. However, with BCall StoAlloc we can allocate store based
 * on the RawRepSize of the RRFmt type field. Hmm, what happens in LISP
 * though - perhaps we only allow this optimisation for really special
 * occasions?
 */
void
rrUnitEmerge(Foam unit)
{
	AInt	i;
	Foam	defs;

	/* Sanity check */
	assert(foamTag(unit) == FOAM_Unit);


	/* Extract the top-level definitions */
	defs = unit->foamUnit.defs;
	assert(foamTag(defs) == FOAM_DDef);


	/* Formats may change during optimisation */
	rrInitFormats(unit);


	/* Search for suitable progs */
	for (i = 0;i < foamArgc(defs); i++)
	{
		Foam	prog;
		Foam	def = defs->foamDDef.argv[i];


		/* Sanity check */
		assert(foamTag(def) == FOAM_Def);


		/* We only care about progs */
		prog = def->foamDef.rhs;
		if (foamTag(prog) != FOAM_Prog) continue;


		/* Replace the prog with an optimised version */
		def->foamDef.rhs = rrProgEmerge(prog);
	}


	/* Rebuild the format declarations */
	rrMakeNewFormats(unit);
}


/*****************************************************************************
 *
 * :: Prog-level optimisers
 *
 ****************************************************************************/

/*
 * Replace raw records in a prog with arrays or plain records
 */
local Foam
rrProgEmerge(Foam prog)
{
	FlowGraph	flog;


	/* Are there any RRFmt instructions in this prog? */
	if (!rrFoamHasRRFmt(prog)) return prog;


	/* We may need to create more locals */
	rrInfo.locals = vpNew(fboxNew(prog->foamProg.locals));


	/* Create a flow-graph for optimisation */
	flog = flogFrProg(prog, FLOG_UniqueExit);


	/* cprop the prog to make RRFmts available */
	cpFlog(flog);


	/* Compute all the reaching definitions and merge */
	if (usedefChainsFrFlog(flog, UD_OUTPUT_UdList))
	{
		if (rrFlogEmerge(flog, prog))
			optSetDeadvPending(prog);
	}


	/* Rebuild the locals vector */
	prog->foamProg.locals = fboxMake(rrInfo.locals->fbox);


	/* Rebuild the prog and return */
	prog = flogToProg(flog);
	return prog;
}


local Bool
rrFlogEmerge(FlowGraph flog, Foam prog)
{
	/*
	 * Determine which raw record locals don't escape. Those
	 * which don't are replaced with records: we hope that
	 * emerge will continue and replace these with locals.
	 */
	AInt	i, nlocs, npars, ndefs;


	/* Safety check */
	assert(foamTag(prog) == FOAM_Prog);


	/* How many locals and parameters are we tracking? */
	nlocs = foamDDeclArgc(prog->foamProg.locals);
	npars = foamDDeclArgc(prog->foamProg.params);


	/*
	 * How many definitions are there in this prog? See
	 * udVarDefsVectBuild() to see why we begin with the
	 * number of parameters to this prog.
	 */
	ndefs = npars;
	flogIter(flog, bb, { ndefs += rrCountDefs(bb); });


	/* Data structures for caching reaching results */
	rrInfo.changed = false;
	rrInfo.ndefs   = ndefs;
	rrInfo.npars   = npars;
	rrInfo.class   = bitvClassCreate(rrInfo.ndefs);
	rrInfo.seen    = bitvNew(rrInfo.class);
	rrInfo.cached  = bitvNew(rrInfo.class);
	rrInfo.escapes = bitvNew(rrInfo.class);


	/* Initialise cached results */
	bitvClearAll(rrInfo.class, rrInfo.cached);
	bitvClearAll(rrInfo.class, rrInfo.escapes);


	/*
	 * The parameters passed to this function all escape. From the
	 * source in usedef.c we see that values passed to a function
	 * are given false definitions with a definition number equal
	 * to the parameter number.
	 */
	for (i = 0;i < npars; i++) rrMarkEscapingDef(i);


	/* Identify escaping and non-escaping raw records */
	flogIter(flog, bb, { rrFindEscapes(bb); });


	/* Display the numbers of non-escaping raw records */
	rrfmtDEBUG({
		(void)fprintf(dbOut, "Non-escapes: [ ");
		for (i = 0; i < ndefs; i++)
		{
			/* Skip past unknown definitions */
			if (!rrDefIsKnown(i))
				continue;


			/* Skip definitions that escape */
			if (rrDefHasEscaped(i))
				continue;


			/* Print this one */
			(void)fprintf(dbOut, "<%d> ", (int)i);
		}
		(void)fprintf(dbOut, "]\n");
	});


	/* Walk the prog replacing raw records */
	flogIter(flog, bb, { rrReplace(bb); });


	/* Did we change anything? */
	return rrInfo.changed;
}


local AInt
rrCountDefs(BBlock bb)
{
	AInt	ndefs = 0;
	Foam	code = bb->code;


	/* Count the number of definitions in this bblock */
	foamIter(code, stmtp, { if (rrIsDef(*stmtp)) ndefs++; });


	/* Return the number of definitions found */
	return ndefs;
}


local void
rrFindEscapes(BBlock bb)
{
	/* Walk the bb to find escapes */
	foamIter(bb->code, stmtp,
	{
		Foam	stmt = *stmtp;

		/* Definitions need special care */
		if (rrIsDef(stmt))
			rrFindDefEscapes(stmt);
		else
			rrFindDeepEscapes(stmt);
	});
}


local void
rrFindDefEscapes(Foam foam)
{
	/* Examine a Set/Def for escapes */
	Bool	isDef = (foamTag(foam) == FOAM_Def);
	Foam	lhs = (isDef ? foam->foamDef.lhs : foam->foamSet.lhs);
	Foam	rhs = (isDef ? foam->foamDef.rhs : foam->foamSet.rhs);


	/* Safety check (a tad late) */
	assert((foamTag(foam) == FOAM_Set) || (foamTag(foam) == FOAM_Def));


	/*
	 * If the lhs is a multi then we consider it to be an escaping
	 * definition. Future work: check each element of the multi to
	 * see which ones escape and which do no.
	 */
	if (foamTag(lhs) == FOAM_Values)
	{
		/* This definition escapes */
		rrDefEscapes(foam);


		/* Check for other escapes */
		rrFindDeepEscapes(lhs);
		rrFindDeepEscapes(rhs);
	}


	/* If the lhs is a local then check to see if the rhs escapes. */
	else if (rrIsLocal(lhs))
		rr0FindDefEscapes(foam, lhs, rhs);


	/* If the lhs is non-local but the rhs is, then the rhs escapes */
	else if (rrIsLocal(rhs))
	{
		rrFindDeepEscapes(lhs);
		rrLocalEscapes(rhs);
	}


	/* Otherwise both sides may contain escapes */
	else
	{
		rrFindDeepEscapes(rhs);
		rrFindDeepEscapes(lhs);
	}
}


local void
rr0FindDefEscapes(Foam def, Foam loc, Foam val)
{
	/*
	 * Given the foam (Set (Loc a) (Loc b)), or the equivalent Def,
	 * mark the definition as non-escaping if all definitions of
	 * (Loc b) reaching this point can be traced to RRNew expressions.
	 * Definitions which trace back to other expressions are deemed
	 * to escape and this definition escapes as a result. However, if
	 * (Loc a) is already known to escape then (Loc b) is marked as
	 * escaping.
	 *
	 * Given the foam (Set (Loc a) (RRNew ...)), or the equivalent
	 * Def, then this definition does not escape.
	 *
	 * Given the foam (Set (Loc a) <expr>), or the equivalent Def,
	 * consider this definition to escape and check <expr> to see if
	 * any other locals escape.
	 */
	Foam		uncast;


	/* Safety check */
	assert(rrIsLocal(loc));


	/* What does the rhs look like without a cast? */
	uncast = val;
	while (foamTag(uncast) == FOAM_Cast)
		uncast = uncast->foamCast.expr;


	/* Deal with each case in turn */
	if (foamTag(uncast) == FOAM_RRNew)
	{
		Foam	fmt = uncast->foamRRNew.fmt;

		/*
		 * If we can determine the raw record format statically
		 * then it is non-escaping.
		 */
		if (!rrRRFmtIsIndependent(fmt))
		{
			/* Check for escapes in format */
			rrDefEscapes(def);
			rrFindDeepEscapes(fmt);
		}
		else
			rrDefNotEscapes(def);
	}
	else if (rrIsLocal(uncast))
	{
		/* If either escapes then mark them */
		if (!rrNonEscapingLocal(uncast) || !rrNonEscapingLocal(loc))
		{
			rrDefEscapes(def);
			rrLocalEscapes(uncast);
		}
		else
			rrDefNotEscapes(def);
	}
	else
	{
		/* Assume def escapes and check rhs for escapes */
		rrDefEscapes(def);
		rrFindDeepEscapes(val);
	}
}


local void
rrDefEscapes(Foam def)
{
	/* This definition escapes */
	AInt defno = rrDefNo(def);


	/* We always record escaping definitions */
	rrMarkEscapingDef(defno);
}


local void
rrDefNotEscapes(Foam def)
{
	/* This definition does not escape */
	AInt defno = rrDefNo(def);


	/* Only mark non-escaping if no result known */
	if (!rrDefIsKnown(defno))
		rrMarkNonEscapingDef(defno);
}


/*
 * Identify escaping raw records: those which are passed to
 * functions and those whose set of reaching definitions
 * includes an escaping definition. Note that Defs/Sets are
 * examined elsewhere and may call us recursively.
 */
local void
rrFindDeepEscapes(Foam foam)
{
	/* Safety checks */
	assert(foam);
	assert(foamTag(foam) != FOAM_Set);
	assert(foamTag(foam) != FOAM_Def);


	/* Special check on locals */
	if (rrIsLocal(foam))
	{
		rrReconcileLocal(foam);
		return;
	}


	/* Ignore other variables */
	if (rrIsVar(foam)) return;


	/* Raw records can escape if they are arguments to a call. */
	switch (foamTag(foam))
	{
		case FOAM_Return: rrReturnEscapes(foam); break;
		case FOAM_BCall:  rrBCallEscapes(foam); break;
		case FOAM_CCall:  rrCCallEscapes(foam); break;
		case FOAM_OCall:  rrOCallEscapes(foam); break;
		case FOAM_PCall:  rrPCallEscapes(foam); break;
		default: foamIter(foam, arg, rrFindDeepEscapes(*arg));
	}
}


/*
 * The set of all definitions that reach the use of this local must be
 * all escaping or all non-escaping. If any of the reaching definitions
 * escape then all the others are marked as escaping.
 */
local void
rrReconcileLocal(Foam loc)
{
	/* If no definitions escape then we can stop now */
	if (rrNonEscapingLocal(loc)) return;


	/* Mark all the definitions as escaping */
	rrLocalEscapes(loc);
}


local void
rrReturnEscapes(Foam ret)
{
	Foam	value;

	/* Safety check */
	assert(foamTag(ret) == FOAM_Return);


	/* Get the value being returned */
	value = ret->foamReturn.value;


	/* Single or multi? */
	if (foamTag(value) == FOAM_Values)
		rrCallArgsEscape(value->foamValues.argv, foamArgc(value));


	/* Mark all definitions of this local as escaping */
	else if (rrIsLocal(value))
		rrLocalEscapes(value);


	/* Check for other escapes */
	else
		rrFindDeepEscapes(value);
}


local void
rrBCallEscapes(Foam call)
{
	assert(foamTag(call) == FOAM_BCall);
	rrCallArgsEscape(call->foamBCall.argv, foamArgc(call) - 1);
}


local void
rrCCallEscapes(Foam call)
{
	assert(foamTag(call) == FOAM_CCall);
	rrCallArgsEscape(call->foamCCall.argv, foamArgc(call) - 2);
}


local void
rrOCallEscapes(Foam call)
{
	assert(foamTag(call) == FOAM_OCall);
	rrCallArgsEscape(call->foamOCall.argv, foamArgc(call) - 3);
}


local void
rrPCallEscapes(Foam call)
{
	assert(foamTag(call) == FOAM_PCall);
	rrCallArgsEscape(call->foamPCall.argv, foamArgc(call) - 3);
}


local void
rrCallArgsEscape(Foam *argv, AInt argc)
{
	AInt	i;

	for (i = 0;i < argc; i++)
	{
		Foam	arg = argv[i];

		/* Mark all definitions of this local as escaping */
		if (rrIsLocal(arg))
			rrLocalEscapes(arg);
		else
			rrFindDeepEscapes(arg);
	}
}


/*
 * Find all definitions that reach this use of a local and
 * mark them as escaping.
 */
local void
rrLocalEscapes(Foam loc)
{
	Bitv	seen = rrInfo.seen;
	Bitv	cached = rrInfo.cached;
	Bitv	escapes = rrInfo.escapes;


	/* Clear the loop detection marks */
	bitvClearAll(rrInfo.class, seen);


	/* Inner marker */
	rr0LocalEscapes(loc, seen, cached, escapes);
}


local void
rr0LocalEscapes(Foam loc, Bitv seen, Bitv cached, Bitv escapes)
{
	/*
	 * Walk the chain of definitions that reach this local
	 * marking them all as escaping.
	 *
	 * Since the reaching definitions may contain loops
	 * we have to be careful where we tread.
	 *
	 * !!! See if we can optimise this to prevent us from
	 * !!! having to walk all definitions repeatedly for
	 * !!! the same local.
	 */
	AInt		defno;
	Bool		isDef;
	UdInfoList	udList;
	Foam		def, val;


	/* Safety check */
	assert(rrIsLocal(loc));


	/* Get the chain of definitions that reach this usage */
	udList = (UdInfoList)rrReachingDefs(loc);


	/* Mark each definition and its ancestors as escaping. */
	listIter(UdInfo, udInfo, udList,
	{
		/* Extract the definition reaching here */
		def = udInfo -> foam;
		isDef = (foamTag(def) == FOAM_Def);
		defno = rrDefNo(def);


		/* Have we checked this node already? */
		if (rrDefAlreadySeen(defno)) continue;


		/* Note that we have checked this node */
		rrMarkDefSeen(defno);


		/* Mark this definition as escaping */
		rrDefEscapes(def);


		/* Get the value that reaches this point */
		val = (isDef ? def->foamDef.rhs : def->foamSet.rhs);


		/* Remove any casts from the value */
		while (foamTag(val) == FOAM_Cast)
			val = val->foamCast.expr;


		/* If it isn't a local then it is a leaf node */
		if (!rrIsLocal(val)) continue;


		/* Mark the reaching definitions of this local */
		rr0LocalEscapes(val, seen, cached, escapes);
	});
}


local Bool
rrNonEscapingLocal(Foam loc)
{
	Bitv	seen = rrInfo.seen;
	Bitv	cached = rrInfo.cached;
	Bitv	escapes = rrInfo.escapes;


	/* Clear the loop detection marks */
	bitvClearAll(rrInfo.class, seen);


	/* Inner checker */
	return rr0NonEscapingLocal(loc, seen, cached, escapes);
}


local Bool
rr0NonEscapingLocal(Foam loc, Bitv seen, Bitv cached, Bitv escapes)
{
	/*
	 * Walk the chain of definitions that reach this local.
	 * If any of the leaves is not an RRNew then return
	 * false indicating that this local escapes somehow.
	 *
	 * Since the reaching definitions may contain loops
	 * we have to be careful where we tread.
	 */
	AInt		defno;
	Bool		isDef;
	UdInfoList	udList;
	Foam		def, val;


	/* Safety check */
	assert(rrIsLocal(loc));


	/* Get the chain of definitions that reach this usage */
	udList = (UdInfoList)rrReachingDefs(loc);


	/*
	 * If we can't find any definitions it must have been
	 * used before definition (or we have a bug). Consider
	 * it to escape for safety.
	 */
	if (!udList) return false;


	/*
	 * Check each definition - if any of the definitions
	 * that reach this use escape then this use escapes.
	 * If none escape then this use doesn't escape.
	 */
	listIter(UdInfo, udInfo, udList,
	{
		/* Extract the definition reaching here */
		def = udInfo -> foam;
		isDef = (foamTag(def) == FOAM_Def);
		defno = rrDefNo(def);


		/* Have we checked this node already? */
		if (rrDefAlreadySeen(defno)) continue;


		/* Note that we have checked this node */
		rrMarkDefSeen(defno);


		/* Do we have a cached result? */
		if (rrDefIsKnown(defno))
		{
			/* Stop if this escapes */
			if (rrDefHasEscaped(defno))
				return false;


			/* Try next definition reaching here */
			continue;
		}


		/* Get the value that reaches this point */
		val = (isDef ? def->foamDef.rhs : def->foamSet.rhs);


		/* Remove any casts from the value */
		while (foamTag(val) == FOAM_Cast)
			val = val->foamCast.expr;


		/* Try to mark RRNew definitions as non-escaping */
		if (foamTag(val) == FOAM_RRNew)
		{
			/* Continue with the next reaching definition */
			rrDefNotEscapes(def);
			continue;
		}


		/* If it isn't a local then it must escape. */
		if (!rrIsLocal(val))
		{
			/* Mark as an escaping definition */
			rrDefEscapes(def);
			return false;
		}


		/* Trace the reaching definitions of this local */
		if (!rr0NonEscapingLocal(val, seen, cached, escapes))
		{
			/* This definition escapes - mark it */
			rrDefEscapes(def);
			return false;
		}


		/* This definition doesn't appear to escape */
		rrDefNotEscapes(def);
	});


	/* No defs escape */
	return true;
}


local void
rrReplace(BBlock bb)
{
	Foam		code = bb->code;
	FoamList	newcode = listNil(Foam);


	/*
	 * Walk the statements of the block looking for raw
	 * records to replace.
	 */
	foamIter(code, stmtp,
	{
		Foam	stmt = *stmtp;


		/* Definition to be replaced? */
		if (rrIsDef(stmt))
			stmt = rrReplaceDef(stmt, &newcode);


		/* Try to replace any RRElts */
		stmt = rrReplaceDeeply(stmt);


		/* Add this statement to the code list */
		newcode = listCons(Foam)(stmt, newcode);
	});


	/* Replace the code for this BBlock */
	newcode  = listNReverse(Foam)(newcode);
	bb->code = foamNewOfList(FOAM_Seq, newcode);

	/* FIXME: free newcode and the old bb->code top-level seq node */
}


local Foam
rrReplaceDef(Foam def, FoamList *newcodep)
{
	/* See if we can replace the rhs with an array */
	AInt	rfmt;
	AInt	i, defno, argc;
	Foam	uncast, fmt, elt;
	Bool	isDef = (foamTag(def) == FOAM_Def);
	Foam	lhs = (isDef ? def->foamDef.lhs : def->foamSet.lhs);
	Foam	rhs = (isDef ? def->foamDef.rhs : def->foamSet.rhs);


	/* Safety check (a tad late) */
	assert((foamTag(def) == FOAM_Set) || (foamTag(def) == FOAM_Def));


	/* Which definition is this? */
	defno = rrDefNo(def);


	/* Ignore unknown definitions */
	if (!rrDefIsKnown(defno)) return def;


	/* Ignore definitions that escape */
	if (rrDefHasEscaped(defno)) return def;


	/* Remove the cast from the rhs */
	uncast = rhs;
	while (foamTag(uncast) == FOAM_Cast)
		uncast = uncast->foamCast.expr;


	/* Do we have anything on the rhs to replace? */
	if (foamTag(uncast) != FOAM_RRNew) return def;


	/* The lhs MUST be a local */
	assert(rrIsLocal(lhs));


	/* Get the raw format */
	fmt = uncast->foamRRNew.fmt;


	/* Safety check */
	assert(foamTag(fmt) == FOAM_RRFmt);


	/* Pull out the format value */
	fmt = fmt->foamRRFmt.fmt;


	/* Safety check */
	assert(foamTag(fmt) == FOAM_Values);


	/* How many fields are there? */
	argc = foamArgc(fmt);


	/* We are about to modify this prog */
	rrInfo.changed = true;


	/*
	 * Replace
	 *    (Set (Loc r) (RRNew 3 (RRFmt (Values SInt DFlo SFlo)))) with
	 * with
	 *    (Set (Loc r)  (RNew 9))
	 *    (Set (RElt 9 (Loc r) 0) (RNew 6))
	 *    (Set (RElt 9 (Loc r) 1) (RNew 7))
	 *    (Set (RElt 9 (Loc r) 2) (RNew 8))
	 * and add the declarations:
	 *    (DDecl Records (Decl SInt "data" -1 4)) ; fmt 6
	 *    (DDecl Records (Decl DFlo "data" -1 4)) ; fmt 7
	 *    (DDecl Records (Decl SFlo "data" -1 4)) ; fmt 8
	 *    (DDecl Records                          ; fmt 9
	 *       (Decl Word "p1" -1 4)
	 *       (Decl Word "p2" -1 4)
	 *       (Decl Word "p3" -1 4))
	 *
	 * Actually, to ensure that the reaching definitions are
	 * still valid we keep the original assignment and kill
	 * it by adding the replacement after it. Deadvar ought
	 * to remove the original for us.
	 */
	*newcodep = listCons(Foam)(def, *newcodep);


	/* Get the format number for this RRFmt */
	rfmt = rrTranslateRRFmt(fmt);


	/* Create the new record in this format */
	rhs = foamNewRNew(rfmt);


	/* Create a plain record to replace `def' */
	def = foamCopy(def);
	if (isDef)
		def->foamDef.rhs = rhs;
	else
		def->foamSet.rhs = rhs;


	/* Add to the code stream */
	*newcodep = listCons(Foam)(def, *newcodep);


	/* Update the fields of the new record */
	for (i = 0; i < argc; i++)
	{
		Foam	set;
		FoamTag	type;


		/* Convert the RRFmt element into a type */
		type = rrSizeToType(fmt->foamValues.argv[i]);


		/*
		 * Create a new record of this type. We could invent
		 * a bit of foam to wrap the type in and cache these
		 * formats just like the main format.
		 */
		rhs = foamNewRNew(rrNewRecordFmt(type, 1));


		/* Create a new assignment */
		elt = foamNewRElt(rfmt, foamCopy(lhs), i);
		set = foamNewSet(elt, rhs);


		/* Add it to the statement list */
		*newcodep = listCons(Foam)(set, *newcodep);
	}


	/* Return nothing */
	return foamNewNOp();
}


local Foam
rrReplaceDeeply(Foam foam)
{
	AInt	fmt, relt;
	Foam	rrec, uncast, rfmt;

	/* Recursively replace RRElt expressions */
	foamIter(foam, expr, {*expr = rrReplaceDeeply(*expr);});


	/* Ignore non-RRElt expressions */
	if (foamTag(foam) != FOAM_RRElt) return foam;


	/* Explode the access */
	rrec = foam->foamRRElt.data;
	rfmt = foam->foamRRElt.fmt;
	relt = foam->foamRRElt.field;


	/* Remove any casts */
	uncast = rrec;
	while (foamTag(uncast) == FOAM_Cast)
		uncast = uncast->foamCast.expr;


	/* Ignore it if it isn't a local */
	if (!rrIsLocal(uncast)) return foam;


	/*
	 * Ignore if the raw record escapes. Note that this
	 * call ought not to trace the use-def chains to their
	 * leaves since all reaching definitions ought to be
	 * be cached now.
	 */
	if (!rrNonEscapingLocal(uncast)) return foam;


	/* Get the literal part of the RRFmt */
	assert(foamTag(rfmt) == FOAM_RRFmt);
	rfmt = rfmt->foamRRFmt.fmt;
	assert(foamTag(rfmt) == FOAM_Values);


	/* Get the format number for this RRFmt */
	fmt = rrTranslateRRFmt(rfmt);


	/* Replace the RRElt with a simple RElt */
	rrec = foamNewCast(FOAM_Rec, foamCopy(uncast));
	return foamNewRElt(fmt, rrec, relt);
}


local FoamTag
rrSizeToType(Foam size)
{
	/*
	 * Convert a RawRepSize or SizeOf* call into a FOAM type.
	 * We could come seriously unstuck here if we have a set
	 * of BCalls defining the size but we are unable to work
	 * out what sort of type to associate with it.
	 */
	assert(foamTag(size) == FOAM_BCall);


	/* What is the call? */
	switch (size->foamBCall.op)
	{
		case FOAM_BVal_RawRepSize:
			return rrTypeToType(size->foamBCall.argv[0]);

#if 0
		case FOAM_BVal_SizeOfInt8:	return FOAM_Int8;
		case FOAM_BVal_SizeOfInt16:	return FOAM_Int16;
		case FOAM_BVal_SizeOfInt32:	return FOAM_Int32;
		case FOAM_BVal_SizeOfInt64:	return FOAM_Int64;
		case FOAM_BVal_SizeOfInt128:	return FOAM_Int128;
#endif

		case FOAM_BVal_SizeOfNil:	return FOAM_Nil;
		case FOAM_BVal_SizeOfChar:	return FOAM_Char;
		case FOAM_BVal_SizeOfBool:	return FOAM_Bool;
		case FOAM_BVal_SizeOfByte:	return FOAM_Byte;
		case FOAM_BVal_SizeOfHInt:	return FOAM_HInt;
		case FOAM_BVal_SizeOfSInt:	return FOAM_SInt;
		case FOAM_BVal_SizeOfBInt:	return FOAM_BInt;
		case FOAM_BVal_SizeOfSFlo:	return FOAM_SFlo;
		case FOAM_BVal_SizeOfDFlo:	return FOAM_DFlo;
		case FOAM_BVal_SizeOfWord:	return FOAM_Word;
		case FOAM_BVal_SizeOfClos:	return FOAM_Clos;
		case FOAM_BVal_SizeOfPtr:	return FOAM_Ptr;
		case FOAM_BVal_SizeOfRec:	return FOAM_Rec;
		case FOAM_BVal_SizeOfArr:	return FOAM_Arr;
		case FOAM_BVal_SizeOfTR:	return FOAM_TR;
	}

	bug("rrSizeToType: unknown size");
	return FOAM_Word;
}


local FoamTag
rrTypeToType(Foam size)
{
	/* Convert a Type* call into a FOAM type. */
	assert(foamTag(size) == FOAM_BCall);


	/* What is the call? */
	switch (size->foamBCall.op)
	{
#if 0
		case FOAM_BVal_TypeInt8:	return FOAM_Int8;
		case FOAM_BVal_TypeInt16:	return FOAM_Int16;
		case FOAM_BVal_TypeInt32:	return FOAM_Int32;
		case FOAM_BVal_TypeInt64:	return FOAM_Int64;
		case FOAM_BVal_TypeInt128:	return FOAM_Int128;
#endif

		case FOAM_BVal_TypeNil:		return FOAM_Nil;
		case FOAM_BVal_TypeChar:	return FOAM_Char;
		case FOAM_BVal_TypeBool:	return FOAM_Bool;
		case FOAM_BVal_TypeByte:	return FOAM_Byte;
		case FOAM_BVal_TypeHInt:	return FOAM_HInt;
		case FOAM_BVal_TypeSInt:	return FOAM_SInt;
		case FOAM_BVal_TypeBInt:	return FOAM_BInt;
		case FOAM_BVal_TypeSFlo:	return FOAM_SFlo;
		case FOAM_BVal_TypeDFlo:	return FOAM_DFlo;
		case FOAM_BVal_TypeWord:	return FOAM_Word;
		case FOAM_BVal_TypeClos:	return FOAM_Clos;
		case FOAM_BVal_TypePtr:		return FOAM_Ptr;
		case FOAM_BVal_TypeRec:		return FOAM_Rec;
		case FOAM_BVal_TypeArr:		return FOAM_Arr;
		case FOAM_BVal_TypeTR:		return FOAM_TR;
	}

	bug("rrTypeToType: unknown type");
	return FOAM_Word;
}


local AInt
rrNewRecordFmt(FoamTag type, AInt nelts)
{
	AInt	i, fmt;
	Foam	ddecl;


	/* Create new declaration for this record */
	ddecl = foamNew(FOAM_DDecl, nelts + 1, FOAM_DDecl_Record);


	/* Initialise its fields */
	for (i = 0;i < nelts; i++)
	{
		String	name;
		Foam	decl;


		/* Invent a field name */
		name = strPrintf("p%d", i);


		/* Create a declaration for this field */
		decl = foamNewDecl(type, name, emptyFormatSlot);


		/* Update the record declaration */
		ddecl->foamDDecl.argv[i] = decl;
	}


	/* Convert the declaration into a format number */
	fmt = rrAddFormat(ddecl);


	/* Return the format of this record */
	return fmt;
}


local void
rrInitFormats(Foam unit)
{
	/* Store the formats globally in an extendable form */
	rrInfo.formats = fboxNew(unit->foamUnit.formats);


	/* Create a mapping between RRFmt and foam formats */
	rrInfo.remap = tblNew((TblHashFun)foamHash, (TblEqFun)foamEqual);
}


local AInt
rrAddFormat(Foam ddecl)
{
	return fboxAdd(rrInfo.formats, ddecl);
}


local void
rrMakeNewFormats(Foam unit)
{
	/* Recreate the format declarations for a Unit */
	unit->foamUnit.formats = fboxMake(rrInfo.formats);


	/* Free the RRFmt mapping */
	tblFreeDeeply(rrInfo.remap, (TblFreeKeyFun)foamFree,
		(TblFreeEltFun)NULL);
}


local AInt
rrLookupRRFmt(Foam rrfmt)
{
	/* Safety check */
	assert(foamTag(rrfmt) == FOAM_Values);


	/* Hash table lookup */
	return (AInt)tblElt(rrInfo.remap, (TblKey)rrfmt, (TblElt)0);
}


local void
rrCacheRRFmt(Foam rrfmt, AInt fmt)
{
	/* Safety check */
	assert(foamTag(rrfmt) == FOAM_Values);


	/* Update the hash table */
	tblSetElt(rrInfo.remap, (TblKey)foamCopy(rrfmt), (TblElt)fmt);
}


local AInt
rrTranslateRRFmt(Foam rrfmt)
{
	AInt	fmt = rrLookupRRFmt(rrfmt);


	/* Safety check */
	assert(foamTag(rrfmt) == FOAM_Values);


	/* Create a new format if necessary */
	if (!fmt)
	{
		/* Create the record and format */
		fmt = rrNewRecordFmt(FOAM_Word, foamArgc(rrfmt));


		/* Cache the format */
		rrCacheRRFmt(rrfmt, fmt);
	}


	/* Return the format number */
	return fmt;
}


local Bool
rrFoamHasRRFmt(Foam foam)
{
	/* Check this expression */
	if (foamTag(foam) == FOAM_RRFmt) return true;


	/* Check each sub-term and return if any RRFmt found */
	foamIter(foam, expr, {if (rrFoamHasRRFmt(*expr)) return true;});
	return false;
}


Bool
rrRRFmtIsIndependent(Foam rrfmt)
{
	/*
	 * Independent RRFmt instructions are those whose value
	 * does not depend on any other instruction. This means
	 * that they are entirely composed of literals, or BCalls
	 * with literal or BCall arguments.
	 */
	AInt	i;
	Foam	fmt;


	/* Sanity check */
	assert(foamTag(rrfmt) == FOAM_RRFmt);


	/* Get the format of the RRFmt */
	fmt = rrfmt->foamRRFmt.fmt;


	/* Must be a FOAM_Values to be independent */
	if (foamTag(fmt) != FOAM_Values) return false;


	/* Check each element of the format */
	for (i = 0; i < foamArgc(fmt); i++)
	{
		Foam elt = foamArgv(fmt)[i].code;


		/* Stop checking if not independent */
		if (!rrIndepExpr(elt)) return false;
	}


	/* This is an independent RRFmt */
	return true;
}


local Bool
rrIndepExpr(Foam elt)
{
	AInt	i;

	/* Does it look like a possible candidate? */
	if (foamTag(elt) != FOAM_BCall)
	{
		/* If not a BCall then it must be a SInt literal */
		return (foamTag(elt) == FOAM_SInt);
	}


	/* Check each of the BCall arguments */
	for (i = 1; i < foamArgc(elt); i++)
	{
		/* Each argument must be independent */
		if (!rrIndepExpr(foamArgv(elt)[i].code))
			return false;
	}


	/* Must be okay */
	return true;
}

