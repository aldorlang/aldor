/*****************************************************************************
 *
 * of_inlin.c: Inlining Foam functions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file implements Foam procedural integration.
 *
 * The foam unit passed to the inliner is destructively updated by
 * inlining calls to procedures for which we have permission to inline,
 * and which we decide are small enough to inline.  We also integrate
 * non-program constants.
 *
 * Inlining information is communicated though symbol meanings.  During
 * genfoam and constant folding if a symbol meaning has a constant value
 * we put its constant number from its unit into the constNum field
 * of the symbol meaning.  Since shared symbol meanings may be held in
 * separate Syme structures, we use the syme->defNum field and a translation
 * table maintained by genfoam to map to the true constant number.
 * Note that the value of the constant pointed to by the syme has slightly
 * different meanings for program and non-program values.  For programs
 * the the consant is the FOAM_Prog part of a closure; the environment
 * part is determined from the call.  For non-programs the constant is the
 * actual value to be inlined.
 *
 * We maintain two separate bounds for the maximum size of a program to be
 * inlined.  The first is used by all programs except ones with the 
 * INL_InlineMe bit set; for these programs, we use the larger limit.
 * Currently we only set that bit for generator functions.
 *
 * This program also handles inlining of operation that were originally 
 * exported from domains which were parameters to other domains.  
 * For example, when we use the type Complex(DoubleFloat)  we can inline
 * all the operations from the DoubleFloat domain as well as the operation
 * from Complex(DoubleFloat).  This is done by computing substitution lists
 * for the exporters of the operation from paramterized domains.  
 *
 * To Do:
 * 	Inline operations from default packages.
 *		To do this, we need to be able to walk up the category
 *		hierarchy at compile time, and look for default operations.
 *		We will also need to have inlining permission from every
 *		category on the way up, as well as the category with the
 *		default.
 *	Dynamically compute inlining size bounds.
 *		Currently we have hard-coded inliiner size limits which
 *		can cause problems for large programs.  We should instead
 *		walk over a unit, measuring program sizes, and the sizes
 *		of programs we want to inline, and dynamically compute
 *		a bound that won't make the inlined unit to huge.  There
 *		should always be a minmum size of at least 1 so that
 *		operations which are just calls to builtins will always be 
 *		inlined.
 *	Inline based more on whether envirnonment merging will take place.
 *		The biggest payoff from inlining comes when we can merge
 *		the envirnomnet, and data of the function we inlined.
 *		Thus we need to give higher weight to functions which create
 *		heap storage that we can merge.
 *	Inline from Basic
 *		The scope binder should set the inline bit on syme.  
 *		currently the inliner walks the inline declarations to
 *		determine when we have permission to inline, but it doesn't
 *		understand "inline from Basic".
 ***************************************************************************
 *
 *	!!!!!!!!!!!!!!!!!!!!!!!!!!! README !!!!!!!!!!!!!!!!!!!!!!!!
 *
 * 13 June 1994, PI: Part of the previous documentation is out of date.
 *   The inlining strategy has been completely redesigned. For each prog a
 *   priority queue is built to choose the best set of functions to inline.
 *   Elements that are considered assigning the priority are:
 *	- loop depth
 *	- size of the function to inline
 *	- nature of the function (generator, monostatement, ...)
 *	- expected growth of the caller after inlining
 *	- constant parameters in the call (-> good chance for the other opt.)
 *	- other
 *
 *   Other changes regard the way in which function calls are evaluated,
 *   in the sense that all the information about size, time, nature, etc. is
 *   stored in the header of each function. The inliner doesn't need anymore
 *   to traverse a local call and -in the case of external call- to get
 *   the entire function body from the library to decide whether or not to
 *   inline.
 *   Finally, dataflow analysis and flowgraph construction has been introduced
 *   to get a safer and more powerful inlining.
 *
 ***************************************************************************
 *  ToDo:
 *  - Performances of the inliner can be substantially improved using
 *    flowgraph merging and local dataflow analysis. When a call is inlined,
 *    infact, the flowgraph is now converted to a prog, again to a flowgraph
 *    and usedef chains for the entire flog are recomputed. This process can
 *    greatly be simplified with some work.
 *
 ***************************************************************************/

#include "axlphase.h"
#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "gf_rtime.h"
#include "inlutil.h"
#include "loops.h"
#include "of_inlin.h"
#include "optfoam.h"
#include "opttools.h"
#include "store.h"
#include "util.h"


/*****************************************************************************
 *
 * :: Local Structures declarations
 *
 ****************************************************************************/

typedef Foam ProgInfo;  /* ProgInfo is a foamProg without children */

/*
 * Elements of a association list of format numbers with their library of 
 * origin.
 */
typedef struct formatInfo {
	Lib		origin;
	AInt		extfmt;
	AInt		locfmt;
	Hash		hash;
	Foam		ddecl;		/* SHARED */
} *FmtInfo;

DECLARE_LIST(FmtInfo);
CREATE_LIST(FmtInfo);

/*
 * Structure which associates constants and their library of origin
 * with their local contant index, declaration Foam and value Foam
 */
typedef struct constInfo {
	Lib		origin;
	int		extConst;
	int		locConst;
	Foam		def;
	Foam		decl;
} *ConstInfo;

DECLARE_LIST(ConstInfo);
CREATE_LIST(ConstInfo);

/*
 * Structure describing every thing the inliner needs to know about the
 * foamUnit it is inlining.
 */
typedef struct unitInfo {
	Foam		unit;		/* Foam for the unit. */
	FoamBox		globals;	/* Decls for globals in unit */
	FmtInfoList	formatRefList;	/* Assoc list of libs and formats */
	FoamBox		formats;	/* new format DDecls */
	ConstInfoList	constList;	/* Assoc list of inlined constants */
	int		constc;		/* Number of constants in unit */
	Foam		*constv;	/* Array of constant values */
	Bool		changed;	/* True if unit was inlined */
	FoamBox		*formatBoxes;	/* FoamBox for each format */

	ULong		originalSize;	/* Size before inlining 	*/
	ULong		size;		/* Current size for the unit	*/

} *UnitInfo;

/*
 * Structure describing everything about the program that is begin inlined
 * into the current program.
 */
typedef struct inlineeInfo {
	Lib		origin;		/* Library of origin of program */
	Foam		formats;	/* formats from unit of origin */
	Foam		origProg;	/* pointer to the original foamProg */
	Bool		sameParent;	/* true iff inlinee and inliner have
					   the same parent */
	Bool		noLocalEnv;	/* True if no pushenv */
	int		returnCount;	/* how many return statements */
	Foam		denv;		/* prog's foamDEnv */
	Foam		env;		/* prog's environment */
	Foam		parentEnv;	/* Parent of this env (if available) */
	int		returnLabel;	/* label for return stmt */
	FoamList	returnVals;	/* holder for return value(s) */
	Syme		syme;		/* Syme for prog, if known */
	AbSub		sigma;		/* sublist for parameter op inlining */
} *InlineeInfo;

struct InlPriCallStruct {
	Foam 	call;		/* call that must be inlined */
	Foam *	stmtPtr;	/* statem. containing call that must be inl.*/
	BBlock	block;		/* block containing the call */
	AInt	size;		/* extimated growth after inlining */
};

typedef struct InlPriCallStruct *  InlPriCall;

/*****************************************************************************
 *
 * :: Debug Stuff
 *
 ****************************************************************************/

/* To debug priority queues building, use -WD+inlCallInfo.
 * To trace a particolar prog, used the debugger and set
 *	inlConstTrace = (num. prog).
 */
int	inlConstTrace = -1; /* 19; */ /* 18;*/ /* -1; */
int	inlCallInfoSerial = -1;
int	inlRejectInfo;

#define INL_REJ_Unknown			0
#define	INL_REJ_NotConstSyme		1
#define	INL_REJ_LocalFoamFail		2
#define	INL_REJ_ExternalProgHdrFail	3
#define	INL_REJ_LocalConstInfoFail	4
#define	INL_REJ_NotConstClosProg	5
#define	INL_REJ_NoPermission		6
#define	INL_REJ_Getter			7
#define	INL_REJ_Evil			8
#define	INL_REJ_Fluids			9
#define	INL_REJ_OCalls			10
#define	INL_REJ_NoInlineInfo		11
#define	INL_REJ_DontInlineMe		12
#define	INL_REJ_RecursiveCall		13
#define INL_REJ_LocalInConst0		14

extern void	inlPrintRejectCause	(String);
extern void	inlPrintUninlinedCalls	(InlPriCall, PriQKey);

/* 
 * Debug flags. lots. 
 * inline, inlCall and inlCallInfo: General 'is X inlined' questions
 * Others give more detail on prog transforms and construction.
 */

Bool	inlineDebug	= false;
Bool	inlUnitDebug	= false;
Bool	inlProgDebug	= false;
Bool	inlExprDebug	= false;
Bool	inlCallDebug	= false;
Bool	inlCallInfoDebug= false;
Bool	inlTransDebug	= false;
Bool	inlExportDebug	= false;
Bool	inlExtendDebug	= false;

local Bool	inlInlineGenerators = false;

#define inlineDEBUG(s)		DEBUG_IF(inlineDebug, s)
#define	inlUnitDEBUG(s)		DEBUG_IF(inlUnitDebug, s)
#define	inlProgDEBUG(s)		DEBUG_IF(inlProgDebug, s)
#define	inlExprDEBUG(s)		DEBUG_IF(inlExprDebug, s)
#define	inlCallDEBUG(s)		DEBUG_IF(inlCallDebug, s)
#define	inlTransDEBUG(s)	DEBUG_IF(inlTransDebug, s)
#define inlExportDEBUG(s)	DEBUG_IF(inlExportDebug, s)
#define inlExtendDEBUG(s)	DEBUG_IF(inlExtendDebug, s)

#define	inlCallInfoDEBUG(s)	DEBUG_IF(inlCallInfoDebug &&		     \
					 (inlConstTrace == -1 ||	     \
					 inlConstTrace == inlProg->constNum),\
					 s)

/*****************************************************************************
 *
 * :: Local function declarations
 *
 ****************************************************************************/

/*
 * Foam traversal functions
 */
local void	inlDDef			(Foam);
local Foam	inlProgram		(Foam, int);
local void	inlNewLocals		(Foam);
local Foam	inlExpr			(Foam, Bool);
local Foam	inlCall			(Foam, Bool);
local Foam	inlId			(Foam);

local void	inlSimplifyDDef		(Foam);

/*
 * Foam inlining functions
 */
local Foam	inlInlineSymeCall	(Foam, Foam *, Foam, Syme, Bool);
local Foam	inlInlineOpenCall	(Foam, Foam *, Foam, Foam, Bool);
local Foam	inlInlineConstCall	(Foam, Foam *, Foam, Foam, Bool);
local Foam	inlInlineBody		(Foam, Foam, Foam *, Foam, Foam, Bool);
local Foam	inlInlineProg		(Foam, Foam *, Foam *, Bool);
local void	inlAddEnv		(Foam envLoc, Foam env, Bool);
local Foam	inlParentEnv		(Foam envLoc, Foam env);

/*
 * Foam inlinee functions
 */
local void	inlNewInlinee		(void);
local Foam	inlSetInlinee		(Lib, Foam);

/*
 * Foam Globals
 */
local int	inlAddGlobal		(Foam);
local void	inlMakeNewGlobals	(Foam);
local int	inlTransformGlobal	(Foam);
local Bool	inlIsForcer		(Foam);

/*
 * Foam Formats
 */
local FmtInfo	inlNewFormatInfo	(Lib, AInt, AInt, Hash, Foam);
local Bool	inlIsFormatInfo		(FmtInfo, Lib, AInt);
local FoamBox * inlInitFormatBoxes	(Foam formats);
local int	inlAddFormat		(Foam);
local void	inlMakeNewFormats	(Foam);
local AInt	inlGetFormat		(AInt);
local AInt	inlGetExternalFormat	(Lib, AInt);
local Syme	inlGetSyme		(AInt, AInt);
local Syme	inlGetExternalSyme	(Lib, AInt, AInt);

#define		inlInlineeFormat(n)	inlInlinee->formats->foamDFmt.argv[n]
#define		inlInlineeDecl(n,k)	inlInlineeFormat(n)->foamDDecl.argv[k]

/*
 * Foam Constants
 */
local int	inlAddConst		(int);
local void	inlMakeNewConsts	(Foam);
local Foam	inlGetLocalConstInfo	(int);
local Foam	inlGetLocalConst	(AInt);
local Foam	inlGetConstLevels	(AInt);
local Foam	inlGetExternalConst	(Lib, AInt);
local void	inlUpdateConstProg	(Foam);
local void	inlUpdateDDecl		(Foam);
local void	inlUpdateConstBody	(Foam);

/*
 * Foam Code
 */
local Foam	inlGetFoam		(Syme);
local Foam	inlGetLocalFoam		(Syme);
local Foam	inlGetExternalFoam	(Syme);
local ProgInfo	inlGetProgInfoFrSyme	(Syme);
local ProgInfo	inlGetExternalProgHdr	(Syme);

local OptInfo	inlInfoNew0		(Stab, Foam, Syme, Bool, Bool);

/*
 * Foam Inlinability
 */
local Bool	inlInlinable0		(Stab, Syme, Bool);

local Bool	inlSefoIsInlinable	(Stab, Sefo);
local Bool	inlSymeIsInlinable	(Stab, Syme);
local Bool	inlTFormIsInlinable	(Stab, TForm);
local Bool	inlIsConstProgSyme	(Syme syme);

local void	inlGetProgInfo		(Foam, int *, Bool *, Bool *);
local Foam	inlTransformExpr	(Foam, Foam *, Foam *);
local void	inlAddStmt		(Foam);
local Foam	inlAddLocal		(FoamTag, int);
local Foam	inlAddTempLocal		(FoamTag, int);
local int	inlAddLex		(FoamTag, int);
local int	inlAddLabel		(void);
local void	inlAddLocalDecls	(Foam, Foam *);
local void	inlGetTypeFrDecl	(Foam decl, FoamTag *, int *);
local Bool	inlUseParam		(Foam, int);
local Foam	inlFoamEnvElt		(Foam);

local void	inlSimplifyDDef		(Foam);
local void	inlSimplifyFlog		(FlowGraph);
local Foam	inlSimplifySeq		(Foam);

local Foam	inlSet			(Foam set);
local Foam	inlSets			(Foam);
extern Bool	inlUniqueValues		(Foam lhs, Foam rhs);
local Foam	inlReturn		(Foam);
local void	inlComputeEEltSyme	(Foam eelt);
local Foam	inlLex			(Foam);
local Foam	inlEnv			(Foam);
local Bool	inlIsSideEffecting	(Foam);
local FoamTag	inlExprType		(Foam, AInt *);
local Bool	inlSameDEnv		(Foam denv1, Foam denv2);

extern Bool	inlIsEvil		(Foam);
/*
 * value tracking 
 */
local Foam	inlGetVarTableEnv	(Foam foam, Foam cenv);
local Foam	inlCanonEElt		(Foam);
local Foam	inlCanonEEnv		(Foam);

local Syme	inlGetSymeFrEnv		(Foam);

/*
 * Parameter inlining
 */
local Syme	inlSubstitutedSyme	(Syme);
local Syme	inlSymeSubstSelf	(Syme, TForm);

static struct unitInfo		inlUnitInfo;

static UnitInfo		inlUnit = &inlUnitInfo;
static OptInfo		inlProg;
static InlineeInfo	inlInlinee;
static Bool		inlInlineAll;
static Bool		inlInlineProgs;

/* Maximum size of a program that we will automatically inline. */
int	inlSizeLimit;	/* default value in optfoam.c */

int 	inlEvilGlobal;

#define inlInlineeIsLocal()	(inlInlinee->origin == NULL)

/*****************************************************************************
 *
 * :: Types definitions
 *
 ****************************************************************************/

local void	inlPriqBuildFrBlock	(BBlock, int loopDepth);

local Foam *	inlPriCallStmtReset(InlPriCall priCall);
local Bool 	inlInlinePriCall(InlPriCall priCall, PriQKey priority);

local InlPriCall inlPriCallNew(Foam foam, Foam * stmtPtr, BBlock, int);
local void	inlPriCallFree(InlPriCall pc);

local Foam	inlInsertSeq(Foam foam);

/*****************************************************************************
 *
 * :: New InlPriCall Staff (TO BE MOVED)
 *
 ****************************************************************************/

local Bool	inlIsUnderLimit(AInt, AInt, int);

local void
inlMakeFlatFlog(FlowGraph flog)
{
	flogIter(flog, bb, {
		bb->code = utilMakeFlatSeq(bb->code);
	});
}

local void
inlBuildPriqFrProg(Foam prog)
{
	OptInfo 	iprog = foamOptInfo(prog);
	int		loopDepth;
	FlowGraph	flog = iprog->flog;

	assert(foamTag(prog) == FOAM_Prog);
	assert(iprog->flog == flog);

	iprog->size = 0;

	flogIter(flog, bb, {
		bb->code = inlSimplifySeq(bb->code);

		iprog->size += foamArgc(bb->code);

		loopDepth = bb->iextra;

		inlPriqBuildFrBlock(bb, loopDepth);

	});
}




local void
inlInlineProgWithPriq(Foam prog)
{
	PriQKey		priority = 0;
	InlPriCall	priCall = NULL;
	OptInfo		optInfo = foamOptInfo(prog);
	Bool		underLimit = true;
extern int optInlineRoof;

	if (!optInfo) return;

	inlBuildPriqFrProg(prog);

	while (priqCount(inlProg->priq)  /* &&
	       priority < inlMinimumGain */) {

		/* We really don't want huge Progs, too expensive optimize and
 		 * compile them.
		 * However, all calls that have priority=0 will be inlined.
		 */
		priCall = (InlPriCall) priqExtractMin(inlProg->priq, &priority);

#if AXL_EDIT_1_1_12p6_04
		if (priority > 0
		    && (inlProg->size > optInlineRoof ||
			flogBlockC(inlProg->flog) > InlFlogCutOff)
		    && !genIsRuntime() && !optIsMaxLevel())
			break;
#else
		if (priority > 0
		    && (inlProg->size > InlProgCutOff ||
			flogBlockC(inlProg->flog) > InlFlogCutOff)
		    && !genIsRuntime() && !optIsMaxLevel())
			break;
#endif

		if (priority > 0 &&
		    inlSizeLimit != -1) {
			underLimit = inlIsUnderLimit(inlProg->originalSize,
			       		prog->foamProg.size + priCall->size,
					 inlSizeLimit);

			if (!underLimit) break;
		}

		if (!inlInlinePriCall(priCall, priority)) break;
   	        
	}

	inlCallInfoDEBUG({inlPrintUninlinedCalls(priCall, priority);});

	flogIter(inlProg->flog, bb, {
		bb->code = inlSets(bb->code);
	});
}


/*****************************************************************************
 *
 * :: Foam traversal functions
 *
 ****************************************************************************/

/*
 * Inline function in a Foam unit, updating the structure in-place.
 */
void
inlineUnit(Foam unit, Bool inlineAll, int inlineLimit, Bool inlineProgs)
{
	assert (foamTag(unit) == FOAM_Unit);

	inlUnitDEBUG({
		fprintf(dbOut, ">>inlUnit:\n");
		foamPrintDb(unit);
		fnewline(dbOut);
	});

	inlInlineAll	       = inlineAll;
	inlSizeLimit           = inlineLimit;
	inlInlineProgs	       = inlineProgs;
	inlUnit->unit	       = unit;
	inlUnit->formatRefList = listNil(FmtInfo);

	inlUnit->globals = fboxNew(foamUnitGlobals(unit));
	inlUnit->formats = fboxNew(unit->foamUnit.formats);
	inlUnit->formatBoxes = inlInitFormatBoxes(unit->foamUnit.formats);

	inlUnit->constList = listNil(ConstInfo);
	inlUnit->constc	   = foamDDeclArgc(foamUnitConstants(unit));
	inlUnit->constv	   = (Foam *) stoAlloc(OB_Other,
					       inlUnit->constc * sizeof(Foam));

	inlEvilGlobal = -1;
	foamConstvFrFoam(unit, inlUnit->constc, inlUnit->constv);

	inuUnitInit(unit);

	if (inlineProgs)
		inlDDef(unit->foamUnit.defs);
	else
		inlSimplifyDDef(unit->foamUnit.defs);

	inlMakeNewFormats(unit);
	inlMakeNewGlobals(unit);
	inlMakeNewConsts(unit);

	inuUnitFini(unit);

	inlUnitDEBUG({
		fprintf(dbOut, "<<inlUnit:\n");
		foamPrintDb(unit);
		fnewline(dbOut);
	});

	assert(foamAudit(unit));
	stoFree(inlUnit->constv);
}

/*
 * Inline the Defines from the unit.
 */


local void
inlDDef(Foam defs)
{
	int	i;
	Foam	def;

	assert(foamTag(defs) == FOAM_DDef);

	for(i=0; i < foamArgc(defs); i++)  {
		def = defs->foamDDef.argv[i];
		assert(foamTag(def) == FOAM_Def);
		if (foamTag(def->foamDef.rhs) == FOAM_Prog)
			def->foamDef.rhs = inlProgram(def->foamDef.rhs, i);
	}
}



/*
 * Inline a foam program.
 */
local Foam
inlProgram(Foam prog, int n)
{
	Scope("inlProgram");
	OptInfo		fluid(inlProg);
	int		count, maxCount = 30;

	assert(foamTag(prog) == FOAM_Prog);

	inlProg = foamOptInfo(prog);
	if (inlProg == 0) Return(prog);

	assert(inlInlineProgs);

	if (inlProg->inlState == INL_Inlined ||
	     inlProg->inlState == INL_BeingInlined)
		Return(prog);

	inlProg->inlState = INL_BeingInlined;
	inlProg->constNum = n;
	inlProg->changed = true;

	if (genIsRuntime()) maxCount = 100;

	inlProg->originalSize	= prog->foamProg.size;

	/* !! This loop probably is no more needed */
	for (count = 0; inlProg->changed && count < maxCount; count += 1) {

		inuProgUpdate(prog);

 		inlProg->prog		= prog;
		inlProg->seq		= prog->foamProg.body;
		inlProg->locals		= vpNew(fboxNew(prog->foamProg.locals));
		inlProg->numLabels	= prog->foamProg.nLabels;
		inlProg->denv		= prog->foamProg.levels;
		inlProg->seqBody	= 0;
		inlProg->changed	= false;

		inlProg->seq = prog->foamProg.body;
		inlProg->seqBody = listNil(Foam);

		inlProg->priq	   = priqNew(30);

		/* the inlProg is in prog.hdr.info.opt */
		inlInlineProgWithPriq(prog); 


		if (inlProg->changed)
			inlMakeFlatFlog(inlProg->flog);

		inlNewLocals(prog);
		prog->foamProg.nLabels = inlProg->numLabels;

		priqFreeDeeply(inlProg->priq, (PriQEltFreeFun) inlPriCallFree);

		inlProgDEBUG({
			if ((inlConstTrace == inlProg->constNum ||
			     inlConstTrace == -1)) {
				if (inlProg->changed) {
			    
					fprintf(dbOut, "<<inlProg[prog:%d][step:%d]:\n", n, count);
					flogPrint(dbOut, inlProg->flog, 1);
					fnewline(dbOut);
				}
				else
					fprintf(dbOut, "<<inlProg [step:%d] (UNCHANGED)\n", count);
			}
		});
	}

	inlProg->inlState = INL_Inlined;
	prog = flogToProg(inlProg->flog);
	inlProg->flog = NULL;

#if 0
	/* Use this code to find inlProg->constNum for a given prog */
	(void)fprintf(dbOut, "******************** %d ***************\n", n);
	(void)foamPrintDb(prog);
	(void)fprintf(dbOut, "***************************************\n\n");
#endif

	Return(prog);
}

/******************************************************************************
 *
 * :: Find Priorities for Calls
 *
 *****************************************************************************/

local void
inlPrintPriqElt(PriQKey priority, PriQElt elt)
{
	InlPriCall info = (InlPriCall) elt;
	Syme syme;
	Foam call;
	String string;
	call = info->call;

	if (foamTag(call) == FOAM_CCall && (syme = foamSyme(call->foamCCall.op)) != NULL)
		string = symeString(syme);
	else
		string = "<unknown>";

	fprintf(dbOut,"--  %f -- %d ----- `%s' ----\n",
		priority, (int)info->size, string);
	foamPrintDb(call);
}

local void 
inlPrintPriq()
{
	fprintf(dbOut,"\n------------------ Start of the queue ---------------------\n");
	priqMap((PriQMapFn) inlPrintPriqElt, inlProg->priq);
	fprintf(dbOut,"------------------ End of the queue   ---------------------\n\n");
}


local Foam	inlGetClosFrVar(Foam op);

/* Return NULL if the prog is the some that we are inlining */
local ProgInfo
inlGetProgInfoFrProg(Foam foam)
{
	Foam prog;
	
	assert(foamTag(foam) == FOAM_Prog);

	if (foam == inlProg->prog) {
		inlRejectInfo = INL_REJ_RecursiveCall;
		return NULL;
	}

	prog = foamCopyNode(foam);

	prog->foamProg.locals = NULL;
	prog->foamProg.params = NULL;
	prog->foamProg.fluids = NULL;
	prog->foamProg.levels = NULL;
	prog->foamProg.body = NULL;

	return prog;
}

local ProgInfo
inlPriqGetSymeCallInfo(Syme syme, Bool * isLocal)
{
	ProgInfo	code;

	if (!inlIsConstProgSyme(syme)) {
		inlRejectInfo = INL_REJ_NotConstSyme;
		return 0;
	}


#if INLINING_OF_DEFAULTS_IS_ALLOWED
	/* If we aren't in runtime then check for default symes */
	if (!genIsRuntime()) {
		Bool	catsyme;
		Syme	osyme;

		/*
		 * Sometimes we fail to recognise a domain definition
		 * and end up with symes from its category rather than
		 * symes from the domain. Normally this means that we
		 * fail to inline the call when we go looking for its
		 * FOAM. Unfortunately if the category has a default
		 * implementation then we inline that (bad news).
		 *
		 * However, the compiler needs to be able to inline
		 * default operations when a domain does not export
		 * an alternative. Otherwise performance plummets
		 * considerably when common operations such as <= and
		 * unary minus are left as defaults.
		 *
		 * For the time being we allow inlining of defaults
		 * even though we may accidentally get hold of the
		 * default instead of its replacement.
		 */
		osyme = symeOriginal(syme);
		catsyme = (symeDefnNum(osyme) == symeDefnNum(syme));
		if (symeHasDefault(osyme) && catsyme) {
			inlRejectInfo = INL_REJ_NotConstSyme;
			return (ProgInfo)0;
		}
	}
#endif


	if (symeIsLocalConst(syme)) {
		code = inlGetLocalFoam(syme);

		if (!code) return NULL;

		inlProgram(code, genGetConstNum(syme));
	       	code = inlGetProgInfoFrProg(code);

		*isLocal = true;
	}
	else {
		code = inlGetExternalProgHdr(syme);
		*isLocal = false;
	}

	return code;
}

local ProgInfo
inlPriqGetOpenCallInfo(Foam op, Bool * isLocal)
{
	Foam		code;

	if (foamTag(op) != FOAM_Const) {
		inlRejectInfo = INL_REJ_NotConstSyme;
		return 0;
	}

	code = inlGetLocalConstInfo(op->foamConst.index);
	*isLocal = true;

	return code;
}

local ProgInfo
inlPriqGetConstCallInfo(Foam op, Bool *isLocal)
{
	Foam		code, cnst;

	assert(foamTag(op) == FOAM_Clos);

	cnst = op->foamClos.prog;

	if (foamTag(cnst) != FOAM_Const) {
		inlRejectInfo = INL_REJ_NotConstClosProg;
		return NULL;
	}

	code = inlGetLocalConstInfo(cnst->foamConst.index);

	*isLocal = true;

	return code;
}

local ProgInfo
inlPriqGetCallInfo(Foam call, Bool * pIsLocal)
{
	Foam 	op, val, progInfo = NULL;
	Syme	syme = NULL;
	static  int serialDebug = 0;

	/*
	 * Why did this code save serialDbg? Perhaps we are
	 * supposed to restore serialDebug to its original
	 * value at the end of this function?
	int	serialDbg = serialDebug;
	 */

	inlRejectInfo = INL_REJ_Unknown;

	assert(foamTag(call) == FOAM_CCall || foamTag(call) == FOAM_OCall);

	if (foamTag(call) == FOAM_CCall) {
		op	= call->foamCCall.op;
		val	= inlGetClosFrVar(op);
	}
	else {
		op	= call->foamOCall.op;
		val	= NULL;
	}

	serialDebug += 1;

	/* Inline if syme is declared inlinable. */
	if (foamTag(op) != FOAM_Clos && (syme = foamSyme(op)) != NULL) {
		Stab	stab = inlProg->stab ? inlProg->stab : stabFile();
		if (inlInlinable(stab, syme))
			progInfo = inlPriqGetSymeCallInfo(syme, pIsLocal);
		else {
			/* Make sure we don't see this again...*/
			foamSyme(op) = NULL;
		}
	}

 	inlCallInfoDEBUG({
		fprintf(dbOut, "CallInfo (prog: %d, serial: %d) - syme: ",
			inlProg->constNum, serialDebug);
		if (syme) {
			/* symePrintDb(syme); */
			fprintf(dbOut,"%s\n",symePretty(syme));
		}
		else
			fprintf(dbOut, "NULL");
		fprintf(dbOut," - Call = ");
		foamPrintDb(call);
		if (syme && inlRejectInfo == INL_REJ_NoPermission)
			fprintf(dbOut, "No permission to inline.\n");
	});

	if (progInfo)
		;

	else if (foamTag(call) == FOAM_OCall)
		progInfo = inlPriqGetOpenCallInfo(op, pIsLocal);

	else if (val && foamTag(val) == FOAM_Clos)
		progInfo = inlPriqGetConstCallInfo(val, pIsLocal);

	else if (foamTag(op) == FOAM_Clos)
		progInfo = inlPriqGetConstCallInfo(op, pIsLocal);

	inlCallInfoSerial = serialDebug;

	if (!progInfo) return NULL;

	if (!foamProgHasInlineInfo(progInfo)) {
		inlRejectInfo = INL_REJ_NoInlineInfo;
		return NULL;
	}

	/* Don't inline getters */
	if (foamProgIsGetter(progInfo))	{
		inlRejectInfo = INL_REJ_Getter;
		return NULL; 
	}

	/* !! This could be improved: only if declare fluids */
	if (foamProgUsesFluids(progInfo)) {
		inlRejectInfo = INL_REJ_Fluids;
		return NULL;
	}
	 
	/* Uncomment this if, for some reason, you don't want to inline
	 * external ocalls */
/* 	if (!foamProgHasNoOCalls(progInfo) && !*pIsLocal) { 	*/
/* 		inlRejectInfo = INL_REJ_OCalls; 		*/
/* 		return NULL; 					*/
/* 	} 							*/



	if (foamProgDontInlineMe(progInfo)) {
		inlRejectInfo = INL_REJ_DontInlineMe;
		return NULL;
	}	

	return progInfo;
}

/* Give the space factor on the base of constant parameters.
 * If return value == 0 -> must inline
 */
local PriQKey
inlGetSpaceFactor(Foam call, ProgInfo progInfo)
{
	int	constParams = 0, totParams, i;
	Foam	* parv, prog, cnst;

	if (foamTag(call) == FOAM_CCall) {
		parv = call->foamCCall.argv;
		totParams = foamArgc(call) - 2;
	}
	else {
		assert(foamTag(call) == FOAM_OCall);
		parv = call->foamOCall.argv;
		totParams = foamArgc(call) - 3;
	}
	
	if (totParams == 0) return 1;

	for (i = 0; i < totParams; i++) {
		if (otIsMovableData(parv[i])) constParams += 1;

		/* A generator among parameters -> we win if we inline this
 	 	 * call. See, in example, hilbert1.
		 */
		if (foamTag(parv[i]) == FOAM_Clos) {
			cnst = parv[i]->foamClos.prog;
			if (foamTag(cnst) == FOAM_Const) {
				prog= inlGetLocalConstInfo(cnst->foamConst.index);
				if (prog &&
				    (foamProgInlineMe(prog) ||
				    foamProgIsGenerator(prog))) {
					if (inlInlineGenerators)
						return 0;
					else
				        	return InlParIsGeneratorMagic;
				}
			}
		}
	}

	return InlBaseFactor - ((PriQKey) constParams) / ((PriQKey) totParams);
}

/* Return -1 if the call CANNOT be inlined, its priority otherwise
 */
local PriQKey
inlPriqGetPriority(int depth, Foam call, int * psize, Foam * pinfo)
{
	ProgInfo	progInfo;
	PriQKey		priority, size, spaceFactor;
	ULong		expectedCalls;
	PriQKey		timeFactor;
	Bool		isLocal;

	expectedCalls = 1L << (depth * InlLoopMagicNumber);
	if (expectedCalls <= 0 || expectedCalls > InlInnerLoopMaxIter)
		expectedCalls = InlInnerLoopMaxIter;

	*psize = 0;

	progInfo = inlPriqGetCallInfo(call, &isLocal);

	*pinfo = progInfo;
	
	if (!progInfo)
		return -1;

	if (isLocal &&
	    inlProg->constNum == 0 &&
	    !genIsRuntime()) {
		inlRejectInfo = INL_REJ_LocalInConst0;
		return -1; 
	}

	if (genIsRuntime()) {
		if (!isLocal) return 0;

		if (foamTag(call) == FOAM_CCall 
		    && (foamTag(call->foamCCall.op) == FOAM_Lex 
	     		|| foamTag(call->foamCCall.op) == FOAM_EElt))
				return 0;
	}

	spaceFactor = inlGetSpaceFactor(call, progInfo);

	if (spaceFactor == 0) {
		*psize = 0;
		return 0;
	}

	if (foamProgIsCalledOnce(progInfo)) {
		inlCallInfoDEBUG(fprintf(dbOut,"(^Unique call^)\n"););
		spaceFactor = InlCalledOnceFactor;
	}

	if (!foamProgIsGenerator(progInfo) &&
	    !foamProgInlineMe(progInfo)) {
		timeFactor  = InlStandardTimeFactor;

/* !! The following code has been commented out because is not necessary.
 * I used it in the process of trimming the inliner. I didn't remove because
 * could be useful if we decide to trim again the inliner.
 */
/*		if (foamProgHasConsts(progInfo)) 		*/
/*			spaceFactor += 2; 			*/
/*		if (!foamProgHasNoOCalls(progInfo))		*/
/*			spaceFactor += 2;			*/

	}
	else {
		timeFactor  = InlInlineMeTimeFactor;
		spaceFactor = spaceFactor / InlInlineMeSpaceFactor;

		if (genIsRuntime() ||
		    inlInlineGenerators) {   /* -Wloops */
			*psize = (int) spaceFactor * progInfo->foamProg.size;
			return 0;
		}
	}
	
	/* 3 is a random namber --- this is how much we like
	 * programs that don't touch their environment.
	 * In general, we love them to death.
	 */
	if (foamProgHasNoEnvUse(progInfo))
		spaceFactor = 3* spaceFactor/InlInlineMeSpaceFactor;

	*psize = (int) spaceFactor * progInfo->foamProg.size;

	size = ((PriQKey) inlProg->prog->foamProg.size) +
		spaceFactor * ((PriQKey) progInfo->foamProg.size);

/* !! This code is actually unused. What it says is that in determining the
 * priority of a call we should consider the estimated time-cost of the
 * caller and of the called function. Presently the inliner doesn't perform
 * such kind of analysis because the inaccuracy of estimated time-cost may
 * cause wrong inliner behaviour.
 */

/*	time = ((PriQKey) inlProg->prog->foamProg.time) -		  */
/*		((PriQKey) expectedCalls) *				  */
/*		((1 - timeFactor) * ((PriQKey) progInfo->foamProg.time) + */
/*		 			((PriQKey) InlCallMagicNumber));  */
/*									  */
/*	if (time < 0) time = 0.01;					  */
/*									  */
/*	priority = size * time;						  */
/*									  */

	priority = (size * timeFactor) / expectedCalls;

	if (foamProgHasSingleStmt(progInfo) &&
	    progInfo->foamProg.size < (foamArgc(call) * InlSingleStmtMagic))
		if (isLocal ||
	    	   (foamProgHasNoOCalls(progInfo) &&
		    !foamProgHasConsts(progInfo))) {
			*psize = 0;
			priority = genIsRuntime() ? 0.0 : priority/20.0;
		}

	/* Must minimize:
	 * (inlProg->prog->foamProg.size + spaceFactor * progInfo->size) ^ 2 *
	 * (inlProg->time - expectedCalls(depth) *
	 * 	((1 - timeFactor(progInfo)) * progInfo->time + CALL_COST))
	 *
	 * - To start: spaceFactor is a simple funct. of const params. in
	 *    	call. Idem for timeFactor.
	 */

	foamFreeNode(progInfo);

	return priority;
}

local void
inlAddCallToPriq(Foam call, Foam * stmtp, int depth, BBlock bb)
{
	Syme 		syme;
	InlPriCall 	inlPriCall;
	ProgInfo	progInfo;
	String		string;
	int size;

	PriQKey	priority = inlPriqGetPriority(depth, call, &size, &progInfo);

	if (priority != -1) {

		inlPriCall = inlPriCallNew(call, stmtp, bb, size);
		priqInsert(inlProg->priq, priority, (PriQElt) inlPriCall);
	}

	if (foamTag(call) == FOAM_CCall && (syme = foamSyme(call->foamCCall.op)) != NULL)
		string = symeString(syme);
	else
		string = "<unknown>";

	inlCallInfoDEBUG({
		if (priority!= -1) {
			fprintf(dbOut, "++++ %s (serial: %d, depth: %d, size: %d, mask: %x) ",
				string,
				inlCallInfoSerial, depth, size, 
				(unsigned) progInfo->foamProg.infoBits);
			fprintf(dbOut, "Added to Queue ++++ (priority=%f)\n",
				priority);
		}
		else	
			inlPrintRejectCause(string);
	});
	inlCallInfoDEBUG(inlPrintPriq());
}

local void
inlPriqBuildFrExpr(Foam foam, Foam * stmtp, int depth, BBlock bb)
{
	/* don't recurse into forces */
	if (foamTag(foam) == FOAM_CCall && 
	    inlIsForcer((foam)->foamCCall.op)) 
		return;

	foamIter(foam, arg, {
		inlPriqBuildFrExpr(*arg, stmtp, depth, bb);
     	});

	if (foamTag(foam) == FOAM_CCall ||
	    foamTag(foam) == FOAM_OCall) 
		inlAddCallToPriq(foam, stmtp, depth, bb);
}


local void
inlPriqBuildFrBlock(BBlock bb, int depth)
{
	Foam seq = bb->code;
	int i;

	assert(foamTag(seq) == FOAM_Seq);

	for (i = 0; i < foamArgc(seq); i++)
		inlPriqBuildFrExpr(seq->foamSeq.argv[i],
				    seq->foamSeq.argv + i,
				    depth, bb);

}				   

local void
inlAddNewCallsToPriq0(Foam foam, Foam * parv, int npars, Foam * stmtp,
		      int depth, BBlock bb)
{
	int i;

	/* don't recurse into forces */
	if (foamTag(foam) == FOAM_CCall && 
	    inlIsForcer((foam)->foamCCall.op)) {
		return;
     	}

	foamIter(foam, arg, {
		inlAddNewCallsToPriq0(*arg, parv, npars, stmtp, depth, bb);
     	});

	if (foamTag(foam) != FOAM_CCall &&
	    foamTag(foam) != FOAM_OCall) return;

	for (i = 0; i < npars; i++)
		if (parv[i] == foam) return;

	inlAddCallToPriq(foam, stmtp, depth, bb);
}

/* Called after that a new seq has replaced the stmt containing and inlined
 * call.
 * "parv" is the vector of parameters and "npars" is its size. Since the new
 * seq may contains both new calls and old calls (parameters to the inlined
 * call), this procedure check that a call is really new before inserting it
 * in the priq.
 */
local void
inlAddNewCallsToPriq(Foam * pfoam,Foam * parv, int npars, int depth, BBlock bb)
{
	int	i;
	Foam 	* stmtp;
	Foam	seq;

	*pfoam = inlExpr(*pfoam, false);

	if (foamTag(*pfoam) != FOAM_Seq) {
		inlAddNewCallsToPriq0(*pfoam, parv, npars, pfoam, depth, bb);
		return;
	}
	seq = *pfoam;

	for (i = 0; i < foamArgc(seq); i++) {
		stmtp = seq->foamSeq.argv + i;
		inlAddNewCallsToPriq0(*stmtp, parv, npars, stmtp, depth, bb);
	}

}

local Foam *
inlCallIsInExpr(Foam call, Foam * pfoam)
{
	foamIter(*pfoam, arg, {
		Foam * res;
		res = inlCallIsInExpr(call, arg);
		if (res) return res;
	});

	if (call == *pfoam)
		return pfoam;
	else
		return (Foam *) int0;
}

local Foam *
inlPriCallStmtReset(InlPriCall priCall)
{
	Foam * pstmt = priCall->stmtPtr;
	Foam   call =  priCall->call;
	int i;

	if (foamTag(*pstmt) != FOAM_Seq) {
		return inlCallIsInExpr(call, pstmt);
	}

	for (i = 0; i < foamArgc(*pstmt); i++) {
		Foam * pst = (*pstmt)->foamSeq.argv + i;

		if (inlCallIsInExpr(call, pst)) {
			priCall->stmtPtr = pst;
			return inlPriCallStmtReset(priCall);
		}
	}

	/* bug("inlPriCallStmtReset: call not found..."); */

	return (Foam *) int0;
}

/******************************************************************************
 *
 * :: Inline Priority Calls
 *
 *****************************************************************************/

/*
 * Inline a priority call.
 * Result = false ==> the flog must be rebuilt.
 */
local Bool
inlInlinePriCall(InlPriCall priCall, PriQKey priority)
{
	Foam *	stmtPtr;
	Foam *  callPtr;
	Foam *  parv;
	int	npars;
	Foam	inlinedCall;
	Syme    syme = NULL;

	inlProg->seq	 = priCall->block->code;

	callPtr = inlPriCallStmtReset(priCall);

	if (!callPtr) {
		inlCallInfoDEBUG({
			fprintf(dbOut, "(Already inlined: [%f] ", priority);
			foamPrintDb(priCall->call);
		});
		return true;
	}

 	stmtPtr = priCall->stmtPtr;

	inlProg->seqBody = listNil(Foam);

	assert(stmtPtr && inlCallIsInExpr(*callPtr, stmtPtr));
	assert(*callPtr == priCall->call);

	if (foamTag(*callPtr) == FOAM_CCall) {
		syme = foamSyme((*callPtr)->foamCCall.op);
		parv = (*callPtr)->foamCCall.argv;
		npars = foamArgc(*callPtr) - 2;
	}
	else {
		assert(foamTag(*callPtr) == FOAM_OCall);
		syme = foamSyme((*callPtr)->foamOCall.op);
		parv = (*callPtr)->foamOCall.argv;
		npars = foamArgc(*callPtr) - 3;
	}

	inlCallInfoDEBUG({
			fprintf(dbOut, "(Inlining:");
			if (syme) fprintf(dbOut,"%s ", symePretty(syme));
			fprintf(dbOut, "[%f] ", priority);
			foamPrintDb(*callPtr);
		  });

	inlinedCall = inlCall(*callPtr, stmtPtr != callPtr);
	
	if (inlinedCall == *callPtr) return true;

	inlProg->prog->foamProg.size += priCall->size;
	
	if (callPtr == stmtPtr) {
		if (otIsVar(inlinedCall) ||
	    	    (foamTag(inlinedCall) == FOAM_Values &&
	     	     foamArgc(inlinedCall) == 0))
			*callPtr = foamNewNil();
		else
			*callPtr = inlinedCall;
	}
	else
		*callPtr = inlinedCall;

	*stmtPtr = inlInsertSeq(*stmtPtr);

	

	inlCallInfoDEBUG({
		if (syme) symePrintDb2(syme);
		fprintf(dbOut, " Inlined)\n");
	});

	inlAddNewCallsToPriq(stmtPtr, parv, npars, 
			     priCall->block->iextra, priCall->block);

	return true;
}

local InlPriCall
inlPriCallNew(Foam call, Foam * stmtPtr, BBlock bb, int size)
{
	InlPriCall i = (InlPriCall) stoAlloc(OB_Other, sizeof(*i));

	i->call    = call;
	i->stmtPtr = stmtPtr;
	i->block   = bb;
	i->size    = size;

	return i;
}

local void
inlPriCallFree(InlPriCall pc)
{
	stoFree(pc);
}

/* Return a (Seq S1..Sn), where S1..Sn-1 are the stmts in inl->seqBody, and
 * Sn is FOAM.
 * Side effects: inlProg->seqBody is released.
 */
local Foam
inlInsertSeq(Foam foam)
{
	int	numStmts = listLength(Foam)(inlProg->seqBody) + 1;
	Foam newSeq = foamNewEmpty(FOAM_Seq, numStmts);
	int	i = 0;

	if (!inlProg->seqBody) return foam;

	inlProg->seqBody = listNReverse(Foam)(inlProg->seqBody);

	listIter(Foam, stmt, inlProg->seqBody, {
		newSeq->foamSeq.argv[i++] = stmt;
		});	

	listFree(Foam)(inlProg->seqBody);

	inlProg->seqBody = listNil(Foam);

	newSeq->foamSeq.argv[i] = foam;

	inlProg->changed = true;
	inlProg->size += (numStmts - 1);

	return newSeq;
}

/*
 * Create a new locals section for a program if new locals were generated.
 */
local void
inlNewLocals(Foam prog)
{
	prog->foamProg.locals = fboxMake(inlProg->locals->fbox);
}

local Foam
inlSets(Foam foam)
{
	foamIter(foam, arg, *arg = inlSets(*arg); );

	if (otIsDef(foam))
		return inlSet(foam);
	else
		return foam;

}

/******************************************************************************
 *
 * :: Simplify Foam
 *
 *****************************************************************************/

/*
 * Simplify Defines from the unit w/out inlining calls.
 */
local void
inlSimplifyDDef(Foam defs)
{
	Scope("inlSimplifyDDef");
	int	i;
	Foam	def;
	OptInfo	fluid(inlProg);

	assert(foamTag(defs) == FOAM_DDef);

	for(i=0; i < foamArgc(defs); i++)  {
		def = defs->foamDDef.argv[i];

		assert(foamTag(def) == FOAM_Def);

		if (foamTag(def->foamDef.rhs) == FOAM_Prog) {
			inlProg = foamOptInfo(def->foamDef.rhs);
			assert(inlProg);

			inlSimplifyFlog(inlProg->flog);
		}
	}

	ReturnNothing;
}

local void
inlSimplifyFlog(FlowGraph flog)
{
	flogIter(flog, bb, { bb->code = inlSimplifySeq(bb->code); });
}

local Foam
inlSimplifySeq(Foam seq)
{
	int i;

	assert(foamTag(seq) == FOAM_Seq);

	for (i = 0; i < foamArgc(seq); i++)
		seq->foamSeq.argv[i] =
		    inlExpr(seq->foamSeq.argv[i], false);

	return seq;
}

/*
 * Simplify a general Foam expression, without inlining calls.
 */
local Foam
inlExpr(Foam foam, Bool isDefLhs)
{
	FoamTag		tag	 = foamTag (foam);
	Foam		newArg;
	int		numArg;
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;

	serialNo += 1;
	depthNo  += 1;
	serialThis = serialNo;

	inlExprDEBUG({
		fprintf(dbOut, ">>(inlExpr %d.%d :\n",
			depthNo-serialThis, serialThis);
		foamPrint(dbOut, foam);
		fnewline(dbOut);
	});

	/* don't recurse into forces */
	if (tag == FOAM_CCall && inlIsForcer(foam->foamCCall.op))
		return inlId(foam);

	numArg = 0;
	foamIter(foam, arg,  {
		newArg = inlExpr(*arg, numArg == 0 && otIsDef(foam));
		if (*arg != newArg) foamFreeNode(*arg);
		*arg = newArg;
		numArg++;
	});

	switch (tag) {
	  case FOAM_Set:
	  case FOAM_Def:
		foam = inlSet(foam);
		break;
	  case FOAM_CEnv:
		foam = inlGetVarTableEnv(foam->foamCEnv.env, foam);
		break;
	  case FOAM_EEnv:
		foam = inlCanonEEnv(foam);
		break;
	  case FOAM_EElt:
		foam = inlCanonEElt(foam);
		/* Fall through */
	  case FOAM_Lex:
		if (!isDefLhs)  foam = inlId(foam);
		break;
	  default:
		break;
	}
	/*!! Free stuff ! */

	inlExprDEBUG({
		fprintf(dbOut, "<<inlExpr: (%d.%d) :\n", depthNo-serialThis, serialThis);
		foamPrint(dbOut, foam);
		fprintf(dbOut, "expr)\n");
	});

	return foam;
}

/*
 * Inline a non-program constant.  e.g. small integer 0 and 1.
 */
local Foam
inlId(Foam var)
{
	Syme	syme;
	Foam	foam, id;
	ProgInfo progInfo;

	if (foamTag(var) == FOAM_CCall)
		id = var->foamCCall.argv[0];
	else
		id = var;
	syme = foamSyme(id);

	if (!syme) return var;

	/* programs get inlined at call point. */
	if (tfIsAnyMap(symeType(syme))) return var;

	progInfo = inlGetProgInfoFrSyme(syme);

	if (progInfo) {
		foamFreeNode(progInfo);
		return var;
	}

	foam = inlGetFoam(syme);
	if (!foam) return var;

	assert(foamTag(foam) != FOAM_Prog);

	return foamCopy(foam);
}

local Foam
inlGetClosFrVar(Foam op)
{
	if (inlProg->converged)
		return inuGetClosFrVar(op);
	else
		return (Foam) NULL;
}

/******************************************************************************
 *
 * :: Inline Call
 *
 *****************************************************************************/

/*
 * Inline a closed or open call.
 */
local Foam
inlCall(Foam call, Bool valueMode)
{
	Foam	op, *argv, env, val, ncall = call;
	Syme	syme;

	if (!inlInlineProgs) return call;

	assert(foamTag(call) == FOAM_CCall || foamTag(call) == FOAM_OCall);

	if (foamTag(call) == FOAM_CCall) {
		op	= call->foamCCall.op;
		argv	= call->foamCCall.argv;
		env	= inlGetVarTableEnv(op, NULL);
		val	= inlGetClosFrVar(op);
	}
	else {
		op	= call->foamOCall.op;
		argv	= call->foamOCall.argv;
		env	= call->foamOCall.env;
		val	= NULL;
	}

	/* Inline if syme is declared inlinable. */
	if (foamTag(op) != FOAM_Clos && (syme = foamSyme(op)) != NULL)
		ncall = inlInlineSymeCall(call, argv, env, syme, valueMode);

	if (ncall != call) 
		;

	/* Inline direct open calls. */
	else if (foamTag(call) == FOAM_OCall)
		ncall = inlInlineOpenCall(call, argv, env, op, valueMode);

	/* Inline direct calls to constant closure values
	 * whose values are tracked through varTable.
	 */
	else if (val && foamTag(val) == FOAM_Clos)
		ncall = inlInlineConstCall(call, argv, env, val, valueMode);

	/* Inline direct calls to constant closure values. */
	else if (foamTag(op) == FOAM_Clos)
		ncall = inlInlineConstCall(call, argv, env, op, valueMode);

	inlCallDEBUG({
		  if (inlConstTrace == -1 ||
		      inlConstTrace == inlProg->constNum) {
				fprintf(dbOut, "<<inlCall:  ");
				foamPrint(dbOut, call); 	
				fprintf(dbOut, "\n  ->\n");	
				foamPrint(dbOut, ncall);	
				fnewline(dbOut);
		}
	});
	
	if (!valueMode && !foamHasSideEffect(ncall)) {
		foamFree(ncall);
		ncall = foamNewNOp();
	}
	if (ncall != call)  ncall = inuPeepExpr(ncall);

	return ncall;
}

/*****************************************************************************
 *
 * :: Foam inlining functions
 *
 ****************************************************************************/

/*
 * inline a call to a function for which we have a symbol meaning.
 * 'call' is the original call, returned if we decide not to inline it.
 * 'argv' holds the arguments to the call, 'env' is the environment for
 * the function, 'syme' is its symbol meaning.
 */
local Foam
inlInlineSymeCall(Foam call, Foam *argv, Foam env, Syme syme, Bool valueMode)
{
	Scope("inlInlineSymeCall");
	Foam		code, val, envLoc;
	InlineeInfo	fluid(inlInlinee);

	inlNewInlinee();
	inlInlinee->syme = syme;

	code = inlGetFoam(syme);
	if (!code) Return(call); 

	if (symeIsLocalConst(syme) && foamOptInfo(code) &&
	    foamOptInfo(code)->inlState != INL_Inlined)
		Return(call);

	assert(!foamProgIsGetter(code));
	assert(!inlIsEvil(code));

	code = inlSetInlinee(symeConstLib(syme), code);
	envLoc = inlAddLocal(FOAM_Env, emptyFormatSlot);
	val = inlInlineBody(code, call, argv, env, envLoc, valueMode);

	assert(val != call);
	if (inlProg->syme) {
		SymeList	symes = symeInlined(inlProg->syme);

		symes = listCons(Syme)(syme, symes);
		symes = listConcat(Syme)(symes, symeInlined(syme));

		symeSetInlined(inlProg->syme, symes);
	}

	if (inlInlinee->sigma)
		absFree(inlInlinee->sigma);
	stoFree(inlInlinee);
	Return(val);
}

/*
 * inline an open call to a function.
 * 'call' is the original call, returned if we decide not to inline it.
 * 'argv' holds the arguments to the call, 'env' is the environment for
 * the function, op is the program part of the operator of the call.
 */
local Foam
inlInlineOpenCall(Foam call, Foam *argv, Foam env, Foam op, Bool valueMode)
{
	Scope("inlInlineOpenCall");
	Foam		code, val, cnst, envLoc;
	InlineeInfo	fluid(inlInlinee);

	assert(foamTag(op) == FOAM_Const);
	cnst = op;
	assert(foamTag(cnst) == FOAM_Const);

	inlNewInlinee();

	code = inlGetLocalConst(cnst->foamConst.index);

	if (!code) Return(call);

	envLoc = inlAddLocal(FOAM_Env, emptyFormatSlot);
	val = inlInlineBody(code, call, argv, env, envLoc, valueMode);

	assert(val != call);

	stoFree(inlInlinee);
	Return(val);
}

/*
 * inline an call to a constant closure. e.g. (CCall (Clos op env) ...)
 * 'call' is the original call, returned if we decide not to inline it.
 * 'argv' holds the arguments to the call, 'env' is the environment for
 * the function, op is the program part of the operator of the call.
 */
local Foam
inlInlineConstCall(Foam call, Foam *argv, Foam env, Foam op,
		   Bool valueMode)
{
	Scope("inlInlineConstCall");
	Foam		code, val, cnst, envLoc;
	InlineeInfo	fluid(inlInlinee);

	assert(foamTag(op) == FOAM_Clos);
	cnst = op->foamClos.prog;
	assert(foamTag(cnst) == FOAM_Const);

	inlNewInlinee();
	inlInlinee->syme = inlGetSymeFrEnv(env);

	code = inlGetLocalConst(cnst->foamConst.index);

	if (!code) Return(call);

	envLoc = inlAddLocal(FOAM_Env, emptyFormatSlot);
	val = inlInlineBody(code, call, argv, env, envLoc, valueMode);

	assert(val != call);

	if (inlInlinee->sigma)
		absFree(inlInlinee->sigma);
	stoFree(inlInlinee);
	Return(val);
}

/*
 * Inline the Foam code for the body of a function.  This is the general entry
 * point for all inlining.  'code' is the body of the function to inline.
 * 'call' is the original call returned if we decide not to inline.  'argv'
 * holds the arguments to the call.  'env' is the envirnment of the call.
 * envLoc is a variable pointing to the envirnment.
 */
local Foam
inlInlineBody(Foam code, Foam call, Foam *argv, Foam env,
	      Foam envLoc, Bool valueMode)
{
	int	i, paramArgc, localArgc, format;
	Bool	hasOCall = false;
	Bool	noLocalEnv, usesInnerEnvironment = false;
	int	*paramCount = 0;
	Foam	*paramArgv, *localArgv, foam;
	Foam    parent;
	/* don't inline yourself */
	assert(inlInlinee->origProg != inlProg->prog);
	assert(!foamProgIsGetter(code));
	assert(foamArgc(code->foamProg.fluids) == 0);

	/* Initialize the paramter replacement vector. */
#ifdef NEW_FORMATS
	paramArgc  = foamDDeclArgc(foamUnitParams(inlUnit->unit)->foamDDecl.argv[code->foamProg.params-1]);
#else
	paramArgc  = foamDDeclArgc(code->foamProg.params);
#endif
	paramCount = (int *) stoAlloc(OB_Other, paramArgc * sizeof(int));
	paramArgv  = (Foam *) stoAlloc(OB_Other, paramArgc * sizeof(Foam));
	for(i=0; i<paramArgc; i++) paramCount[i] = 0;
	inlGetProgInfo(code->foamProg.body, paramCount, &hasOCall, 
		       &usesInnerEnvironment);

	if (usesInnerEnvironment) {
		Foam denv = code->foamProg.levels;
		usesInnerEnvironment = false;
		for (i=1; i<foamArgc(denv); i++) {
			if (denv->foamDEnv.argv[i] != emptyFormatSlot)
				usesInnerEnvironment = true;
		}
	}

#if 0		
	if (!usesInnerEnvironment)
		env = foamNewCast(FOAM_Env, foamNewNil());
#endif
	/* Determine whether inliner and inlinee have the same parent. */
	inlInlinee->sameParent = 
		foamOptInfo(code) &&
		foamOptInfo(code)->stab && inlProg->stab &&
		!foamOptInfo(code)->isGener &&
		!inlProg->isGener &&
		cdr(foamOptInfo(code)->stab) == cdr(inlProg->stab);

	/* Push a new env if progs don't exist in the same env. */
	inlInlinee->denv    = code->foamProg.levels;

	noLocalEnv = (inlInlinee->denv->foamDEnv.argv[0] == emptyFormatSlot);
	inlInlinee->noLocalEnv = noLocalEnv;

	if (inlInlineeIsLocal() 
	    && !inlProg->isGener
	    && inlSameDEnv(inlInlinee->denv,inlProg->denv)) {
		env = noLocalEnv ? foamNewEnv(1) : foamNewEnv(int0);
	} else {
		format = inlGetFormat(inlInlinee->denv->foamDEnv.argv[0]);
		fboxNth(inlUnit->formats, format)->foamDDecl.usage = FOAM_DDecl_LocalEnv;
		env    = noLocalEnv ? foamCopy(env)
			 : foamNewPushEnv(format, foamCopy(env));
	}
	foamSyme(env) = inlInlinee->syme;

	/* Initialize the local variable vector. */
	localArgc = foamDDeclArgc(code->foamProg.locals);
	localArgv = (Foam *) stoAlloc(OB_Other, localArgc * sizeof(Foam));

	/* Add the decls of any new locals in the inlinee. */
	if (foamDDeclArgc(code->foamProg.locals) > 0)
		inlAddLocalDecls(code->foamProg.locals, localArgv);

	/* Set up local var for environment. */
	inlInlinee->env = envLoc;
	parent          = inlParentEnv(envLoc, env);
	if (parent) foamSyme(parent) = inlInlinee->syme;
	inlInlinee->parentEnv = parent;
	/* Compute a label offset for the labels on the labels in the prog. */
	inlProg->newLabel = inlProg->numLabels + code->foamProg.nLabels;

	inlAddEnv(envLoc, env, usesInnerEnvironment);
	
	/* Initialize the variables used to hold the paramters of the call. */
	for(i=0; i< paramArgc; i++) {
		if (inlUseParam(argv[i], paramCount[i]))
			paramArgv[i] = argv[i];
		else {
			FoamTag type;
			Foam	var;
			AInt     fmt;
			type = inlExprType(argv[i], &fmt);
			var = inlAddTempLocal(type, fmt);
			paramArgv[i] = var;
			inlAddStmt(foamNewSet(foamCopy(paramArgv[i]),argv[i]));
			inlProg->prog->foamProg.size += 3;
		}
	}
		
	/* Integrate the inlinee into the inliner. */
	foam = inlInlineProg(code, paramArgv, localArgv, valueMode);

	inlProg->numLabels = inlProg->newLabel;

	inlProg->changed = true;

	if (!foamProgHasNoOCalls(code))
		foamProgUnsetHasNoOCalls(inlProg->prog);

	if (inlProg->seqBody)
		foamProgUnsetHasSingleStmt(inlProg->prog);

	if (paramCount) {
		stoFree(paramCount);
		stoFree(paramArgv);
	}
	
	if (inlInlinee->parentEnv) foamFree(inlInlinee->parentEnv);

	return foam;
}

local Syme
inlGetSymeFrEnv(Foam env)
{
	if (inlProg->converged)
		return inuGetSymeFrEnv(env);
	else
		return NULL;
}

local Bool
inlIsLocalEnv(Foam env)
{
	if (inlProg->converged)
		return inuIsLocalEnv(env);
	else
		return false;
}

local void
inlAddEnv(Foam envLoc, Foam env, Bool usesInner)
{
	Foam parentEnv = NULL;

	if (foamTag(env) == FOAM_PushEnv)
		parentEnv = env->foamPushEnv.parent;
	else if (inlInlinee->noLocalEnv)
		parentEnv = env;

	if (parentEnv
	    && usesInner
	    && !inlInlinee->sameParent 
	    && foamTag(parentEnv) != FOAM_Env 
	    && !inlIsLocalEnv(parentEnv)) {

		inlAddStmt(foamNewEEnsure(foamCopy(parentEnv)));
		inlProg->prog->foamProg.size += 1;
	}

	inlAddStmt(foamNewDef(foamCopy(envLoc), env));
}

local Foam
inlParentEnv(Foam envLoc, Foam env)
{
	Foam parent;

	if (foamTag(env) == FOAM_Env) 
		parent = foamNewEnv(env->foamEnv.level + 1);
	else if (foamTag(env) == FOAM_PushEnv)
		parent = foamCopy(env->foamPushEnv.parent);
	else
		parent = NULL;
	return parent;
}

/******************************************************************************
 *
 * :: Get Prog Info
 *
 *****************************************************************************/

/*
 * Get parameter and lexical usage information from a foam prog.
 */
local void
inlGetProgInfo(Foam foam, int *paramCount, Bool *hasOCall, 
	       Bool *hasInnerEnv)
{
	FoamTag		tag	 = foamTag (foam);
	int		i;

	foamIter(foam, arg, 
		 inlGetProgInfo(*arg, paramCount, hasOCall, hasInnerEnv));
	switch (tag) {
	  case FOAM_Par:
		i = foam->foamPar.index;
		if (paramCount[i] >= 0)
			paramCount[i]++;
		break;
	  case FOAM_Set:
		if (foamTag(foam->foamSet.lhs) == FOAM_Par)
			paramCount[foam->foamSet.lhs->foamPar.index] = -1;
		break;
	  case FOAM_OCall:
		*hasOCall = true;
		break;
	  case FOAM_Env:
		/* Have to be over-conservative as (Env 0) can be 
		 * aliased and referenced deeply 
		 */
		*hasInnerEnv = true;
		break;
	  case FOAM_Lex:
		if (foam->foamLex.level > 0)
			*hasInnerEnv = true;
		break;
	  default:
		break;
	}
}

/******************************************************************************
 *
 * :: Inline Prog
 *
 *****************************************************************************/
/*
 * Inline a program body into a prog.
 */
local Foam
inlInlineProg(Foam code, Foam *paramArgv, Foam *localArgv, Bool valueMode)
{
	Foam		body = code->foamProg.body, expr, retVal;
	int		argc = foamArgc(body), i, rc;
	FoamList	lv;

	assert(foamTag(body) == FOAM_Seq);

	/* Count the returns. */
	for (rc = 0, i = 0; i < argc; i++)
		if (foamTag(body->foamSeq.argv[i])==FOAM_Return) rc++;

	if (argc == 1 && rc == 1) {
		expr   = body->foamSeq.argv[0]->foamReturn.value;
		retVal = inlTransformExpr(expr, paramArgv, localArgv);
	}
	else if (rc == 0) {
		for (i = 0; i < argc; i++)
			inlAddStmt(inlTransformExpr(body->foamSeq.argv[i],
						    paramArgv, localArgv));
		if (code->foamProg.retType != FOAM_NOp)
			retVal = foamNewNil();
		else
			retVal = foamNew(FOAM_Values, int0);
	}
	else {
		/* Set up communication with inlReturn. */
		inlInlinee->returnCount = rc;
		inlInlinee->returnVals  = 0;
		inlInlinee->returnLabel = inlAddLabel();

		/* Transform and add statements. */
		for (i = 0; i < argc; i++)
			inlAddStmt(inlTransformExpr(body->foamSeq.argv[i],
						    paramArgv, localArgv));
		/* Common return point. */
		inlAddStmt(foamNewLabel(inlInlinee->returnLabel));

		/* Return value: either orignal expr (if rc == 1) or temps. */
		lv = inlInlinee->returnVals;
		if (rc > 1) lv = listNMap(Foam)(foamCopy, lv);
		if (lv && !cdr(lv))
			retVal = car(lv);
		else 
			retVal = foamNewOfList(FOAM_Values, lv);
		listFree(Foam)(lv);
	}

	inlProgDEBUG(
	   if (inlConstTrace == inlProg->constNum ||
	       inlConstTrace == -1) {
		   fprintf(dbOut, "<== Producing prog value:\n");
			foamPrint(dbOut, retVal);
	});
	return retVal;
}

/*****************************************************************************
 *
 * :: Foam inlinee functions
 *
 ****************************************************************************/

/*
 * Create a fresh inlinee structure.
 */
local void
inlNewInlinee()
{
	inlInlinee = (InlineeInfo) stoAlloc(OB_Other,
					    sizeof(struct inlineeInfo));
	inlInlinee->syme  = NULL;
	inlInlinee->sigma = NULL;
	return;
}

local Foam
inlSetInlinee(Lib origin, Foam code)
{
	Foam	fmts;

	code = foamCopy(code);
	fmts = origin ? libGetFoamFormats(origin) :
		inlUnit->unit->foamUnit.formats;

	inlInlinee->origin	= origin;
	inlInlinee->formats	= fmts;
	inlInlinee->returnCount	= 0;
	inlInlinee->origProg	= code;

	return code;
}

/*****************************************************************************
 *
 * :: Foam Globals
 *
 ****************************************************************************/

/*
 * Add a global variable to a function being inlined.
 */
local int
inlAddGlobal(Foam decl)
{
	int n = fboxAdd(inlUnit->globals, decl);
	if (strEqual(decl->foamGDecl.id, "dummyDefault"))
		inlEvilGlobal = n;
	return n;
}

/*
 * create the decls for the new globals.
 */
local void
inlMakeNewGlobals(Foam unit)
{
	foamUnitGlobals(unit) = fboxMake(inlUnit->globals);
}

/*
 * Reindex a global variable from a foreign unit, and add it to the
 * global assoc list. 
 */
local int
inlTransformGlobal(Foam glo)
{
	int		size, i;
	AInt		protocol, index;
	String		name;
	Foam		decl, decl0;

	if (inlInlineeIsLocal()) return glo->foamGlo.index;

	decl0 = inlInlineeDecl(globalsSlot, glo->foamGlo.index);
	assert(foamTag(decl0) == FOAM_GDecl);

	name = decl0->foamGDecl.id;
	protocol = decl0->foamGDecl.protocol;

	size = fboxSize(inlUnit->globals);

	for (i = 0; i < size; i++) {
		decl = fboxNth(inlUnit->globals, i);
		if (strEqual(name, decl->foamGDecl.id) &&
		    protocol == decl->foamGDecl.protocol)
			return i;
	}

	decl = foamCopy(decl0);
	decl->foamGDecl.dir = FOAM_GDecl_Import;

	/* Fortran and C PCalls use formats */
	switch (decl->foamGDecl.protocol) {
	  case FOAM_Proto_Fortran:	/*FALLTHROUGH*/
	  case FOAM_Proto_C:		/*FALLTHROUGH*/
		if (decl->foamGDecl.type == FOAM_Clos) {
			AInt fmt = inlGetFormat(decl->foamGDecl.format);
			decl->foamGDecl.format = fmt;
		}
		break;
	  default:
		break;
	}

	index = inlAddGlobal(decl);

	assert(index == i);

	return index;
}

/*****************************************************************************
 *
 * :: Foam Formats
 *
 ****************************************************************************/

/*
 * Create a new format info structure.
 */
local FmtInfo
inlNewFormatInfo(Lib origin, AInt extfmt, AInt locfmt, Hash hash, Foam ddecl)
{
	FmtInfo new = (FmtInfo) stoAlloc(OB_Other, sizeof(struct formatInfo));
	new->origin = origin;
	new->extfmt = extfmt;
	new->locfmt = locfmt;
	new->hash   = hash;
	new->ddecl  = ddecl;
	return new;
}

/*
 * Equality between format infos.
 */
local Bool
inlIsFormatInfo(FmtInfo info, Lib origin, AInt extfmt)
{
	return info->extfmt == extfmt && libEqual(origin, info->origin);
}

/*
 * Create FoamBoxes for every format in a unit.
 */
local FoamBox *
inlInitFormatBoxes(Foam formats)
{
	int	i, size = foamArgc(formats);
	FoamBox *boxes;

	boxes = (FoamBox *) stoAlloc(OB_Other, size*sizeof(FoamBox *));
	for(i=0; i< size; i++)
		boxes[i] = fboxNew(formats->foamDFmt.argv[i]);
	return boxes;
}

/*
 * Add a format to a function being inlined.
 */
local int
inlAddFormat(Foam ddecl)
{
	return fboxAdd(inlUnit->formats, ddecl);
}

/*
 * Create decls for all the formats added by inlining.
 */
local void
inlMakeNewFormats(Foam unit)
{
	int	i;
	for(i=0; i < foamArgc(inlUnit->formats->initial); i++)
		inlUnit->formats->initial->foamDFmt.argv[i] =
			fboxMake(inlUnit->formatBoxes[i]);
	unit->foamUnit.formats = fboxMake(inlUnit->formats);
}

/*
 * return a format number for a format from a prog being inlined.
 */
local AInt
inlGetFormat(AInt format)
{
	if (inlInlineeIsLocal())
		return format;
	else if (format == emptyFormatSlot || format == envUsedSlot)
		return format;
	else
		return inlGetExternalFormat(inlInlinee->origin, format);
}

/*
 * Compute a new format number for an external format.  If it isn't
 * already in the new unit, add it.
 */
local AInt
inlGetExternalFormat(Lib origin, AInt extfmt)
{
	FmtInfoList	l;
	Foam 		ddecl, ddecl0;
	Hash		hash, hash0;
	AInt		locfmt;
	FmtInfo		info;

	ddecl = inlInlineeFormat(extfmt);
	hash  = foamHash(ddecl);

	for (l = inlUnit->formatRefList; l; l = cdr(l)) {
		if (inlIsFormatInfo(car(l), origin, extfmt))
			return car(l)->locfmt;

		ddecl0 = car(l)->ddecl;
		hash0  = foamHash(ddecl0);

		if (hash == hash0 && foamEqual(ddecl, ddecl0))
			return car(l)->locfmt;
	}

 	/* copy because source is a library, therefore it's shared */
        ddecl = foamCopy(ddecl);
	if (ddecl->foamDDecl.usage == FOAM_DDecl_LocalEnv)
		ddecl->foamDDecl.usage = FOAM_DDecl_NonLocalEnv;

	assert(foamTag(ddecl) == FOAM_DDecl);
	foamFixed(ddecl) = true;
	locfmt = inlAddFormat(ddecl);
	info   = inlNewFormatInfo(origin, extfmt, locfmt, hash, ddecl);
	listPush(FmtInfo, info, inlUnit->formatRefList);

	/* A format can contain ref. to other formats in Rec decl.
	 * Placed here to avoid infinite recursion if there is a loop
	 * in the format definitions
	 */
	inlUpdateDDecl(ddecl);
	return locfmt;
}

/*
 * Get the syme from an index into a format.
 */
local Syme
inlGetSyme(AInt format, AInt index)
{
	Syme	syme = NULL;

	if (inlInlineeIsLocal()) {
		Foam	ddecl;
		assert(format < inlUnit->formats->argc);
		ddecl = fboxNth(inlUnit->formats, format);
		assert(ddecl);
		if (index < foamDDeclArgc(ddecl))
			syme = foamSyme(ddecl->foamDDecl.argv[index]);
#if 0
		if (syme)
			fprintf(dbOut, "inlGetSyme:  local syme from decl.\n");
#endif
		/*!! assert(syme == NULL); */
	}
	else
		syme = inlGetExternalSyme(inlInlinee->origin, format, index);

	return syme;
}

local Syme
inlGetExternalSyme(Lib origin, AInt format, AInt index)
{
	return libGetFormatSyme(origin, format, index);
}

/******************************************************************************
 *
 * :: inlIsForcer
 *
 *****************************************************************************/

local Bool
inlIsForcer(Foam foam)
{
	RuntimeCallInfo	info;
	if (foamTag(foam) != FOAM_Glo)
		return false;
	info = gen0GetRuntimeCallInfo(fboxNth(inlUnit->globals, 
					      foam->foamGlo.index));
	return rtCallIsForce(info);
}

/*****************************************************************************
 *
 * :: Foam Constants
 *
 ****************************************************************************/

/*
 * Add a new constant to the unit.  If it isn't already in the unit, add it.
 */
local int
inlAddConst(int index)
{
	Lib		origin = inlInlinee->origin;
	ConstInfoList	l = inlUnit->constList;
	ConstInfo	info;
	Foam		code;
	int		num;

	for (; l; l = cdr(l))
		if (index == car(l)->extConst &&
		    libEqual(origin, car(l)->origin))
			return car(l)->locConst;

	code = inlGetExternalConst(origin, index);
	info = (ConstInfo) stoAlloc(OB_Other, sizeof(struct constInfo));
	num  = inlUnit->constc++;

	info->decl     = foamCopy(inlInlineeDecl(constsSlot, index));
	info->def      = foamNewDef(foamNewConst(num), code);
	info->origin   = origin;
	info->extConst = index;
	info->locConst = num;
	inlUnit->constList = listCons(ConstInfo)(info, inlUnit->constList);
	inlUpdateConstProg(code);

	return num;
}

/*
 * Create a new DDef section with all the new constants added by inlining.
 */
local void
inlMakeNewConsts(Foam unit)
{
	int		i, j, numNew;
	Foam		new;
	ConstInfoList	l;
	Foam		constDDecl, defs;

	if (inlUnit->constList == 0) return;

	constDDecl = foamUnitConstants(unit);
	defs = unit->foamUnit.defs;

	inlUnit->constList = listNReverse(ConstInfo)(inlUnit->constList);

	new = foamNewEmpty(FOAM_DDecl, inlUnit->constc + 1);
	new->foamDDecl.usage = FOAM_DDecl_Consts;

	numNew = inlUnit->constc - foamDDeclArgc(constDDecl);

	for(i=0; i<foamDDeclArgc(constDDecl); i++)
		new->foamDDecl.argv[i] = constDDecl->foamDDecl.argv[i];
	for(l = inlUnit->constList; l; l = cdr(l), i++)
		new->foamDDecl.argv[i] = car(l)->decl;
	foamUnitConstants(unit) = new;
	foamFreeNode(constDDecl);

	/* ensure non-const outer defs are last */
	new = foamNewEmpty(FOAM_DDef, foamArgc(defs) + numNew);

	for (i=0;foamTag(defs->foamDDef.argv[i]->foamDef.lhs)==FOAM_Const;i++)
		new->foamDDef.argv[i] = defs->foamDDef.argv[i];
	j=i;
	for(l = inlUnit->constList; l; l = cdr(l), i++)
		new->foamDDef.argv[i] = car(l)->def;
	for ( ; j<foamArgc(defs); i++, j++)
		new->foamDDef.argv[i] = defs->foamDDef.argv[j];

	unit->foamUnit.defs = new;
	foamFreeNode(defs);

	listFree(ConstInfo)(inlUnit->constList);
}

local Foam
inlGetLocalConstInfo(int n)
{
	Foam	code = NULL;
	Foam	prog;
	OptInfo	info;

	if (n < foamDDeclArgc(foamUnitConstants(inlUnit->unit)))
		code = inlUnit->unit->foamUnit.defs->foamDDef.argv[n];
	else {
		ConstInfoList	l = inlUnit->constList;
		for(; l && (code == 0); l = cdr(l))
			if (n == car(l)->locConst)
				code = car(l)->def;
		if (code == 0) {
			inlRejectInfo = INL_REJ_LocalConstInfoFail;
			return 0;
		}
	}

	assert(foamTag(code) == FOAM_Def);
	prog = code->foamDef.rhs;
	info = foamOptInfo(prog);

	/* don't return the code if we are in the middle of inlining it. */
	assert(foamTag(prog) == FOAM_Prog);
	if (info && info->inlState == INL_BeingInlined) return NULL;
	prog = inlProgram(prog, n);

	prog = inlGetProgInfoFrProg(prog);

	return prog;
}

/*
 * Get the foam code for constant number n from the file we are inlining.
 */
local Foam
inlGetLocalConst(AInt n)
{
	Foam	code = NULL;
	Foam	prog;
	OptInfo	info;

	if (n < foamDDeclArgc(foamUnitConstants(inlUnit->unit)))
		code = inlUnit->unit->foamUnit.defs->foamDDef.argv[n];
	else {
		ConstInfoList	l = inlUnit->constList;
		for(; l && (code == 0); l = cdr(l))
			if (n == car(l)->locConst)
				code = car(l)->def;
		if (code == 0) return 0;
	}

	assert(foamTag(code) == FOAM_Def);
	prog = code->foamDef.rhs;
	info = foamOptInfo(prog);

	assert(foamTag(prog) == FOAM_Prog);

	/* don't return the code if we are in the middle of inlining it. */
	if (info->inlState != INL_Inlined) 
		return NULL;

	return inlSetInlinee(NULL, prog);
}

/*
 * Get the environment levels of a local constant
 */
local Foam
inlGetConstLevels(AInt n)
{
#if EDIT_1_0_n1_07
	Foam	code = (Foam) NULL, prog;
#else
	Foam	code, prog;
#endif

	/*
	 * External constants that have been recently localised
	 * are stored separately from true local constants.
	 */
	if (n >= foamDDeclArgc(foamUnitConstants(inlUnit->unit)))
	{
		ConstInfoList	l = inlUnit->constList;
		for(; l; l = cdr(l))
		{
			ConstInfo	info = car(l);

			if (n == info->locConst)
			{
				code = info->def;
				break;
			}
		}

		/* Hope that we never end up with NULL code */
		if (!code) return 0;
	}
	else
		code = inlUnit->unit->foamUnit.defs->foamDDef.argv[n];


	/* Extract the definition of the prog */
	assert(foamTag(code) == FOAM_Def);
	prog = code->foamDef.rhs;


	/* Return the environment usage */
	assert(foamTag(prog) == FOAM_Prog);
	return prog->foamProg.levels;
}


/*
 * Get the foam code for constant number n from an external library.
 */
local Foam
inlGetExternalConst(Lib lib, AInt n)
{
	return inlSetInlinee(lib, libGetFoamConstant(lib, n));
}

/*
 * update the body of a constant we are adding to the unit.
 */
local void
inlUpdateConstProg(Foam prog)
{
	int	i;
	AInt	*levels;
	assert(foamTag(prog) == FOAM_Prog);
	levels = prog->foamProg.levels->foamDEnv.argv;
	inlUpdateConstBody(prog->foamProg.body);
	foamOptInfo(prog) = inlInfoNew0(NULL, prog, NULL, false, true);
	
#ifdef NEW_FORMATS
	inlUpdateDDecl(inlInlineeDecl(paramsSlot, prog->foamProg.params-1));
#else
	inlUpdateDDecl(prog->foamProg.params);
#endif
	inlUpdateDDecl(prog->foamProg.locals);

	for(i=0; i< foamArgc(prog->foamProg.levels); i++)
		levels[i] = inlGetFormat(levels[i]);
	if (prog->foamProg.retType == FOAM_NOp)
		prog->foamProg.format = inlGetFormat(prog->foamProg.format);
}

/* Given a DDecl, remap all the format number for records */
local void
inlUpdateDDecl(Foam ddecl)
{
	int   i;
	Foam  decl;
	Bool  sigDecl = false;

	/* Function signatures are special */
	switch (ddecl->foamDDecl.usage) {
	  case FOAM_DDecl_FortranSig:	/*FALLTHROUGH*/
	  case FOAM_DDecl_CSig:		/*FALLTHROUGH*/
		sigDecl = true;
	  default:
		break;
	}

	for(i=0; i< foamDDeclArgc(ddecl); i++) {
		decl = ddecl->foamDDecl.argv[i];
		if (decl->foamDecl.type == FOAM_Rec
		    || decl->foamDecl.type == FOAM_TR)
			decl->foamDecl.format = 
				inlGetFormat(decl->foamDecl.format);
		if (sigDecl && decl->foamDecl.type == FOAM_Clos)
			decl->foamDecl.format = inlGetFormat(decl->foamDecl.format);
	}
}

/*
 * transform all the formats in the body of a constant we are adding to
 * the unit.
 */
local void
inlUpdateConstBody(Foam body)
{
	FoamTag		tag   = foamTag (body);

	foamIter(body, arg, inlUpdateConstBody(*arg));
	switch(tag) {
	  case FOAM_Const:
		body->foamConst.index = inlAddConst(body->foamConst.index);
		break;
	  case FOAM_Glo:
		body->foamGlo.index = inlTransformGlobal(body);
		break;
	  case FOAM_EElt:
		body->foamEElt.env = inlGetFormat(body->foamEElt.env);
		break;
	  case FOAM_MFmt:	
		body->foamMFmt.format =  inlGetFormat(body->foamEElt.env);
		break;
	  case FOAM_Rec:
		body->foamRec.format = inlGetFormat(body->foamRec.format);
		break;
	  case FOAM_RElt:
		body->foamRElt.format = inlGetFormat(body->foamRElt.format);
		break;
	  case FOAM_RNew:
		body->foamRNew.format = inlGetFormat(body->foamRNew.format);
		break;
	  case FOAM_TRElt:
		body->foamTRElt.format = inlGetFormat(body->foamTRElt.format);
		break;
	  case FOAM_IRElt:
		body->foamIRElt.format = inlGetFormat(body->foamIRElt.format);
		break;
	  case FOAM_TRNew:
		body->foamTRNew.format = inlGetFormat(body->foamTRNew.format);
		break;
	  case FOAM_PushEnv:
		body->foamPushEnv.format =
			inlGetFormat(body->foamPushEnv.format);
		break;
	  default:
		break;
	}
}

/*****************************************************************************
 *
 * :: Foam Code
 *
 ****************************************************************************/

/*
 * Get the foam code to inline for a given syme.
 */
local Foam
inlGetFoam(Syme syme)
{
	Foam	code;

	if (!inlIsConstProgSyme(syme))
		return NULL;

	if (symeIsLocalConst(syme))
		code = inlGetLocalFoam(syme);
	else
		code = inlGetExternalFoam(syme);

	return code;
}

/*
 * Get the foam code for a syme from the file we are inlining.
 */
local Foam
inlGetLocalFoam(Syme syme)
{
	inlineDEBUG({
		fprintf(dbOut, "inlGetLocalFoam:  ");
		symePrint(dbOut, syme);
		fnewline(dbOut);
	});

	if (genHasConstNum(syme)) {
		int	constNum = genGetConstNum(syme);

		assert(constNum < inlUnit->constc);
		return inlUnit->constv[constNum];
	}
	else {
		inlRejectInfo = INL_REJ_LocalFoamFail;
		return NULL;
	}
}

/*
 * Get the foam code for a syme from an external file.
 */
local Foam
inlGetExternalFoam(Syme syme)
{
	Foam ret;
	Lib  origin = symeConstLib(syme);

	inlineDEBUG({
		fprintf(dbOut, "(inlGetExternalFoam: %s (%s.%ld)",
			symeString(syme), libGetFileId(origin),
			symeConstNum(syme));
	});

	if (genHasConstNum(syme))
		ret = libGetFoamConstant(origin, genGetConstNum(syme));
	else
		ret = NULL;

	inlineDEBUG(fprintf(dbOut, "%s)\n", ret ? "OK" : "Fail"));

	return ret;
}

local ProgInfo
inlGetProgInfoFrSyme(Syme syme)
{
	Foam	code;

	if (!syme)
		return NULL;

	if (!genHasConstNum(syme))
		return NULL;
	
	if (!inlIsConstProgSyme(syme))
		return NULL;

	if (symeIsLocalConst(syme))
		code = inlGetLocalFoam(syme);
	else
		code = inlGetExternalProgHdr(syme);

	if (code && foamTag(code) == FOAM_Prog)
		code = inlGetProgInfoFrProg(code);
	else
		code = NULL;

	return code;
}

local ProgInfo
inlGetExternalProgHdr(Syme syme)
{
	Lib origin = symeConstLib(syme);

	inlineDEBUG({
		fprintf(dbOut, "(inlGetExternalProgHdr: %s (%s.%ld)",
			symeString(syme), libGetFileId(origin),
			symeConstNum(syme));
	});

	/* !!Speedo fix to aviod inlining categories in libaxiom */
	if (strEqual(libGetFileId(origin), "axiom"))
		return NULL;

	if (genHasConstNum(syme)) {
		ProgInfo res = libGetProgHdr(origin, genGetConstNum(syme));
		if (!res) inlRejectInfo = INL_REJ_ExternalProgHdrFail;
		inlineDEBUG(fprintf(dbOut, "%s)\n", res ? "OK" : "Fail"));
		return res;
	}
	else {
		inlineDEBUG(fprintf(dbOut, "Not const: Fail)\n"));
		inlRejectInfo = INL_REJ_ExternalProgHdrFail;
		return NULL;
	}
}

/*****************************************************************************
 *
 * :: Inlining Permission
 *
 ****************************************************************************/

/* "limit" is the percentage limit; ex. 150 means 150%
 * "base" is the original value;
 * "value" is the new value; this function check if "value" is under 
 * "(limit" * InlOverGrowthFactor)%
 * with respect to "base"
 */
local Bool
inlIsUnderLimit(AInt base, AInt value, int limit)
{
	int overLimit = (int) ((double) limit * InlOverGrowthFactor);
	return  ((value * 100) / base)  < overLimit;

}

/* $$ Think the possibility of using a new bit: DontInlineMe, that can be
 * generally used by #pragma DONT_INLINE 
 */
Bool 
inlIsEvil(Foam foam)
{
	Foam body;
	int i, idx;

	body = foam->foamProg.body;
	for (i=0; i<foamArgc(body); i++) {
		Foam stmt = body->foamSeq.argv[i];
		if (foamTag(stmt) == FOAM_Return) 
			stmt = stmt->foamReturn.value;
		if (foamTag(stmt) == FOAM_Glo) {
			idx = inlTransformGlobal(stmt);
			if (idx == inlEvilGlobal)
				return true;
		}
	}
	return false;
}

/*
 * See if Syme is declared inlinable.
 */
Bool 
inlInlinable(Stab stab, Syme syme)
{
	Bool result = inlInlinable0(stab, syme, true);

	if (!result) inlRejectInfo = INL_REJ_NoPermission;

	return result;
}

/*
 * Inner inlinable checking function.
 */
local Bool
inlInlinable0(Stab stab, Syme syme, Bool top)
{
	/* Anything goes with -Qinline-all */
	if (inlInlineAll) return true;


	/* Okay, now check the syme a little more carefully */
	if ( top  && !inlIsConstProgSyme(syme)) return false;
	if (!top && !otIsConstSyme(syme)) return false;
	if (symeIsLocalConst(syme)) return true;


	/* Looks okay but check outer scope levels */
	for (; stab; stab = cdr(stab))
		if (inlSymeIsInlinable(stab, syme)) {
			Bool		result = true;
			SymeList	l;

			/* Make sure all functions inlined are inlinable. */
			l = top ? symeInlined(syme) : listNil(Syme);
			for (; l && result; l = cdr(l))
				result = inlInlinable0(stab, car(l), false);
						
			return result;
		}

	return false;
}

/*****************************************************************************
 *
 * :: inlSefoIsInlinable
 * :: inlSymeIsInlinable
 * :: inlTFormIsInlinable
 *
 ****************************************************************************/

local Bool
inlSefoIsInlinable(Stab stab, Sefo sefo)
{
	if (abIsLeaf(sefo)) {
		if (abSyme(sefo))
			return inlSymeIsInlinable(stab,abSyme(sefo));
	}
	else if (abTag(sefo) == AB_Qualify)
		return inlSefoIsInlinable(stab, sefo->abQualify.origin);

	else if (abTag(sefo) == AB_Apply)
		return inlSefoIsInlinable(stab, abApplyOp(sefo));

	return false;
}

local Bool
inlSymeIsInlinable(Stab stab, Syme syme)
{
	TFormUsesList	tful;

	if (symeExtension(syme))
		return inlSymeIsInlinable(stab, symeExtension(syme)); 

	if ( (symeIsExport(syme) || symeIsExtend(syme)) && 
	     symeLib(syme)) {
		Syme	isyme = symeCopy(syme);
		TForm	tf = tfLibrary(libLibrarySyme(symeLib(syme)));
		symeSetKind(isyme, SYME_Import);
		symeSetExporter(isyme, tf);
		syme = isyme;
	}


	if (symeIsLocalConst(syme) && (symeIsExport(syme) || symeIsExtend(syme)))
		return true;
	if (!symeIsImport(syme)) return false;
	for (tful = car(stab)->tformsUsed.list; tful; tful = cdr(tful)) {
		TFormUses	tfu = car(tful);

		if (tqHasImport(tfu->inlines, syme))
			return true;
	}

	if (inlTFormIsInlinable(stab, symeExporter(syme)))
		return true;

	return false;
}

local Bool
inlTFormIsInlinable(Stab stab, TForm tf)
{
	tf = tfFollowSubst(tf);

	if (tfIsArchive(tf))
		return false;
	else if (tfIsGeneral(tf))
		return inlSefoIsInlinable(stab, tfGetExpr(tf));
	else if (tfIsTuple(tf)
		 || tfIsRecord(tf)
		 || tfIsRawRecord(tf)
		 || tfIsUnion(tf)
		 || tfIsEnum(tf)
		 || tfIsTrailingArray(tf)
		 || tfIsGenerator(tf))
		return inlSefoIsInlinable(stab, tfGetExpr(tf));

	return false;
}

/*
 * Determine when we need temporary variables for parameters
 */
local Bool
inlUseParam(Foam param, int count)
{
        /* true means don't make a local */
	if (count < 0)	/* side effect on arg */
		return false;
	if (otIsLocalVar(param))
		return true;
	if (otIsNonLocalVar(param))
	        return false;
	if (count > 1)
		return false;

	/*
	 * Need to check what happens when we have a Cast for param.
	 * If we strip the cast with foamDereferenceCast() then we
	 * end up with bad foam sharing.
	 */
	if (inlIsSideEffecting(param))
		return false;
	else
		return true;
}

/*
 * Check whether a param is side-effecting.
 */
local Bool
inlIsSideEffecting(Foam param)
{
	FoamTag		tag;
	Foam		op;
	ProgInfo	prog;
	
	tag = foamTag(param);
	switch (tag) {
	  case FOAM_Set:
		return true;
	  case FOAM_PCall:
		return true;
	  case FOAM_OCall:
		op = param->foamOCall.op;
		break;
	  case FOAM_CCall:
		op = param->foamCCall.op;
		break;
	  case FOAM_BCall:
		return foamBValInfo(param->foamBCall.op).hasSideFx;
	  default:
		return false;
	}
	prog = inlGetProgInfoFrSyme(foamSyme(op));
	if (!prog) return true;
	return prog->foamProg.infoBits & IB_SIDE;
}

local Bool
inlIsConstProgSyme(Syme syme)
{
	if (otIsConstSyme(syme))
		return true;

	return otSymeIsFoamConst(syme);
}

/*****************************************************************************
 *
 * :: Foam Inlining Transformations
 *
 ****************************************************************************/

/*
 * Transform an expression for use in an inlined program.  paramArg and 
 * localArgv are vectors of values to use for paramters and locals.
 */
local Foam
inlTransformExpr(Foam expr, Foam *paramArgv, Foam *localArgv)
{
	FoamTag		tag   = foamTag (expr);
	Foam		nexpr = expr;
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;

	serialNo += 1;
	depthNo  += 1;
	serialThis = serialNo;
	inlTransDEBUG({
		fprintf(dbOut, "{inlTrans[%d][%d]\n",
			depthNo, serialThis);
	});

	/*
	 * We miss out on some environment merging and inlining with
	 * closures whose environment is (Env 0) and which are stored
	 * in (Env 0). The inliner will always transform (Env 0) into
	 * a locally created environment, creating cycles that prevent
	 * subsequent emerging. This is particularly unfortunate if the
	 * code part of the closure doesn't use its environment.
	 *
	 * Here we avoid the problem by leaving (Env 0) alone if the
	 * closure doesn't access any outer environment levels.
	 */
	if (tag == FOAM_Clos)
	{
		Foam	env = expr->foamClos.env;
		Foam	prog = expr->foamClos.prog;
		Bool	isEnv = (foamTag(env) == FOAM_Env);
		Bool	isLocalEnv = (isEnv && !(env->foamEnv.level));
		Bool	isConst = (foamTag(prog) == FOAM_Const);


		/* Want environment to be (Env 0) and prog to be const */
		if (isLocalEnv && isConst)
		{
			AInt	i;
			Bool	used;
			Foam	denv;
			AInt	cnum = prog->foamConst.index;


			/* Non-local constants need to be localised */
			if (!inlInlineeIsLocal())
			{
				/* Suck across external constants */
				cnum = inlAddConst(cnum);
				prog->foamConst.index = cnum;
			}


			/* Get the environment levels of this constant */
			denv = inlGetConstLevels(cnum);
			assert(denv);


			/* See if it might use its environment */
			used = false;
			for (i = 1; i < foamArgc(denv); i++)
			{
				AInt	fmt = denv->foamDEnv.argv[i];

				/* Non-empty format means env used */
				if (fmt != emptyFormatSlot)
				{
					used = true;
					break;
				}
			}


			/*
			 * If any of the outer environment levels are used
			 * then we must transform the environment into the
			 * local environment. Otherwise we leave it alone
			 * because our environment will never be touched by
			 * the code in this closure.
			 */
			if (used) expr->foamClos.env = inlEnv(env);


			/* Transformation complete */
			return expr;
		}

		/* Fall through and transform as before */
	}


	/* Recursive depth-first transformation of the expression */
	foamIter(expr, arg, *arg = inlTransformExpr(*arg,paramArgv,localArgv));

	switch(tag) {
	  case FOAM_Par:
		nexpr = paramArgv[expr->foamPar.index];
		if (otIsLocalVar(nexpr))
			nexpr = foamCopy(nexpr);
		break;
	  case FOAM_Loc:
		nexpr->foamLoc.index =
			localArgv[expr->foamLoc.index]->foamLoc.index;
		break;
	  case FOAM_Lex:
		nexpr = inlLex(nexpr);
		break;
	  case FOAM_Env:
		nexpr = inlEnv(nexpr);
		break;
	  case FOAM_Glo:
		expr->foamGlo.index = inlTransformGlobal(expr);
		break;
	  case FOAM_Label:
		nexpr->foamLabel.label += inlProg->numLabels;
		break;
	  case FOAM_If:
		nexpr->foamIf.label += inlProg->numLabels;
		break;
	  case FOAM_Const:
		if (!inlInlineeIsLocal())
			expr->foamConst.index =
				inlAddConst(expr->foamConst.index);
		break;
	  case FOAM_Select: {
		int	i;
		for(i=0; i< foamArgc(nexpr) - 1 /* for op */; i++)
			nexpr->foamSelect.argv[i] += inlProg->numLabels;
		break; }
	  case FOAM_Goto:
		nexpr->foamGoto.label += inlProg->numLabels;
		break;
	  case FOAM_EElt:
		inlComputeEEltSyme(expr);
		expr->foamEElt.env = inlGetFormat(expr->foamEElt.env);
		break;
	  case FOAM_Rec:
		expr->foamRec.format = inlGetFormat(expr->foamRec.format);
		break;
	  case FOAM_RElt:
		expr->foamRElt.format = inlGetFormat(expr->foamRElt.format);
		break;
	  case FOAM_IRElt:
		expr->foamIRElt.format = inlGetFormat(expr->foamIRElt.format);
		break;
	  case FOAM_TRElt:
		expr->foamTRElt.format =
			inlGetFormat(expr->foamTRElt.format);
		break;
	  case FOAM_RNew:
		expr->foamRNew.format = inlGetFormat(expr->foamRNew.format);
		break;
	  case FOAM_TRNew:
		expr->foamTRNew.format =
			inlGetFormat(expr->foamTRNew.format);
		break;
	  case FOAM_MFmt:
		expr->foamMFmt.format = inlGetFormat(expr->foamMFmt.format);
		break;
	  case FOAM_PushEnv:
		expr->foamPushEnv.format =
			inlGetFormat(expr->foamPushEnv.format);
		break;
	  case FOAM_Return:
		nexpr = inlReturn(expr);
		break;
	  default:
		break;
	}

	inlTransDEBUG({
		fprintf(dbOut, "inlTrans: %d%d= ", depthNo, serialThis);
		foamPrint(dbOut, expr);
		fprintf(dbOut, "-->");
		foamPrint(dbOut, nexpr);
		fprintf(dbOut, "[%d][%d]Done}\n", depthNo, serialThis);
	});

	return nexpr;
}

/*
 * Transform sets of multiple values to a pile of sets to single values.
 */
local Foam
inlSet(Foam set)
{
	Foam	lhs = set->foamSet.lhs, rhs = set->foamSet.rhs;
	Foam 	iLhs, iRhs, newSet;
	int	i;
	FoamTag	tag;

	if (foamTag(rhs) ==  FOAM_MFmt)
		rhs = rhs->foamMFmt.value;

	if (foamTag(rhs) != FOAM_Values)
		return set;

	assert(foamTag(lhs) == FOAM_Values);
	assert(foamArgc(lhs) == foamArgc(rhs));
	assert(inlUniqueValues(lhs, rhs));

	if (foamArgc(lhs) == 0)
		return foamNewNOp();
	if (foamArgc(rhs) == 0)	{
		/* In the case where there are no RHS values, we
		   probably had a function inlined which ended with a 'never'..
		   in this case, just gob out a halt, and give up */
		inlAddStmt(foamNew(FOAM_BCall, 2,
				   FOAM_BVal_Halt,
				   foamNewSInt(FOAM_Halt_NeverReached)));
		return foamNewNOp();
	}
	tag = foamTag(set);
	
	for(i=0; i<foamArgc(lhs)-1; i++) {
		iLhs = lhs->foamValues.argv[i];
		iRhs = rhs->foamValues.argv[i];
		inlAddStmt(foamNew(tag, 2, iLhs, iRhs));
	}
	iLhs = lhs->foamValues.argv[i];
	iRhs = rhs->foamValues.argv[i];
	newSet = foamNew(tag, 2, lhs->foamValues.argv[i],
			  rhs->foamValues.argv[i]);

	return inlInsertSeq(newSet);
}

/*
 * Ensures that multi value sets have unique values on the left and right.
 */
Bool
inlUniqueValues(Foam lhs, Foam rhs)
{
	int	i, j;
	for(i=0; i<foamArgc(lhs); i++)
		for(j=0; j<foamArgc(rhs); j++)
			if (i != j && foamEqual(lhs->foamValues.argv[i],
						rhs->foamValues.argv[j]))
				return false;
	return true;
}

/*
 * Set information for generating code for returns.
 */
local Foam
inlReturn(Foam ret)
{
	Foam	 rv = ret->foamReturn.value;
	Foam	 tv;
	FoamList lv;
	FoamTag  type;
	int	 i;
	AInt	 fmt = emptyFormatSlot;

	/* If there is only one return statement, return the expression. */
	if (inlInlinee->returnCount == 1)
		inlInlinee->returnVals = listCons(Foam)(rv, listNil(Foam));

	else {
		/* 1st time: Generate temporaries to hold the results. */
		if (!inlInlinee->returnVals) {
			if (foamTag(rv) != FOAM_Values) {
				type = inlExprType(rv, &fmt);
				tv   = inlAddLocal(type, fmt);
				lv   = listCons(Foam)(tv, listNil(Foam));
			}
			else {
				lv = listNil(Foam);
				for (i = 0; i < foamArgc(rv); i++) {
					type = inlExprType(
						rv->foamValues.argv[i],
						&fmt);
					tv   = inlAddLocal(type,
							   fmt);
					lv   = listCons(Foam)(tv, lv);
				}
				lv = listNReverse(Foam)(lv);
			}
			inlInlinee->returnVals = lv;
		}

		/* Each time: generate assignments to temporaries. */
		if (foamTag(rv) != FOAM_Values) {
			lv = inlInlinee->returnVals;
			inlAddStmt(foamNewDef(foamCopy(car(lv)), rv));
		}
		else {
			lv = inlInlinee->returnVals;
			assert(listLength(Foam)(lv) == foamArgc(rv));

			for (i = 0; i < foamArgc(rv); i++, lv = cdr(lv)) {
				inlAddStmt(foamNewDef(foamCopy(car(lv)),
						      rv->foamValues.argv[i]));
			}
		
		}
		if (foamTag(rv) == FOAM_Clos &&
		    foamTag(rv->foamClos.prog) == FOAM_Const)
		{
			foamSyme(rv) = inlInlinee->syme;
		} 
	}

	inlProg->prog->foamProg.size += 1;
	return foamNewGoto(inlInlinee->returnLabel);
}

local void
inlComputeEEltSyme(Foam eelt)
{
	int	index	= eelt->foamEElt.lex;
	int	format	= eelt->foamEElt.env;
	Syme	syme, nsyme;

	if (foamSyme(eelt))
		syme = foamSyme(eelt);
	else
		syme = inlGetSyme(format, index);

	if (!syme) return;

	nsyme = inlSubstitutedSyme(syme);
	if (nsyme) foamSyme(eelt) = nsyme;
}

local Foam
inlLex(Foam lex)
{
	int	level	= lex->foamLex.level;
	int	index	= lex->foamLex.index;
	int	format	= inlInlinee->denv->foamDEnv.argv[level];
	Syme	syme, nsyme;

	if (foamSyme(lex))
		syme = foamSyme(lex);
	else
		syme = inlGetSyme(format, index);

	nsyme = syme ? inlSubstitutedSyme(syme) : NULL;

	if (!inlInlinee->sameParent || level == 0)
		lex = inlFoamEnvElt(lex);
	else
		inlProg->denv->foamDEnv.argv[level] =
			inlInlinee->denv->foamDEnv.argv[level];

	if (nsyme) foamSyme(lex) = nsyme;

	return lex;
}

/*
 * Transform a foamEnv.
 */
local Foam
inlEnv(Foam env)
{
	int	level = env->foamEnv.level;

	if (inlInlinee->noLocalEnv)
		level--;
	assert(level >= 0);

	if (level == 0) {
		env = foamCopy(inlInlinee->env);
		foamSyme(env) = inlInlinee->syme;
	}
	else if (!inlInlinee->sameParent)  {
		assert((int)level >= 0); /* bug 1168 */
		env = foamNewEEnv(level, foamCopy(inlInlinee->env));
	}

	return env;
}

/*
 * Transform a lexical reference into an environment reference
 */
local Foam
inlFoamEnvElt(Foam lex)
{
	int	format = 0;
	int	level = lex->foamLex.level;
	int	index = lex->foamLex.index;
	Foam	newFoam;

	format = inlGetFormat(inlInlinee->denv->foamDEnv.argv[level]);
	
	if (inlInlinee->noLocalEnv)
		level--;
	assert(level >= 0);

	if (foamTag(inlInlinee->env) == FOAM_Env) 
		newFoam = foamNewLex(level + inlInlinee->env->foamEnv.level,
		       index);
	else if (inlInlinee->parentEnv && level > 0) 
		newFoam = foamNewEElt(format, 
				      foamCopy(inlInlinee->parentEnv),
				      level - 1,
				      index);
	else
		newFoam = foamNewEElt(format, 
				      foamCopy(inlInlinee->env),
				      level,
				      index);
	return newFoam;
}

/*
 * Add a statement to the body of an inlined function.
 */
local void
inlAddStmt(Foam stmt)
{
	inlProgDEBUG(
	   if (inlConstTrace == inlProg->constNum ||
	       inlConstTrace == -1) {
		 	fprintf(dbOut, "... adding statement ");
			foamPrint(dbOut, stmt);
	});
	inlProg->seqBody = listCons(Foam)(stmt, inlProg->seqBody);
}

/*
 * Add a label to the inlined function.
 */
local int
inlAddLabel()
{
	return inlProg->newLabel++;
}

/*
 * Add a local variable to a function being inlined.  Will be a lex if
 * it needs to be.
 */
local Foam
inlAddLocal(FoamTag type, int format)
{
	if (!inlProg->isGener) {
		return inlAddTempLocal(type, format);
	}
	else {
		int	level  = 2;	/* level of iterator parent */
		int	format = inlProg->denv->foamDEnv.argv[level];
		assert(format != emptyFormatSlot);
		assert(format != envUsedSlot);
		return foamNewLex(level, inlAddLex(type, format));
	}
}

/*
 * Add a local variable to a function being inlined.
 */
local Foam
inlAddTempLocal(FoamTag type, int format)
{
	int	index = vpNewVar0(inlProg->locals, type, format);
	return	foamNewLoc(index);
}

/*
 * Add a lexical variable to a function being inlined.
 */
local int
inlAddLex(FoamTag type, int format)
{
	String	name = strPrintf("%d", inlUnit->formatBoxes[format]->argc);
	Foam	decl = foamNewDecl(type, name, emptyFormatSlot);

	assert(format < inlUnit->formats->argc);
	assert(format > emptyFormatSlot);
	return fboxAdd(inlUnit->formatBoxes[format], decl);
}

/*
 * make decls for all the locals in a prog being inlined.
 */
local void
inlAddLocalDecls(Foam locals, Foam *localArgv)
{
	int	i, format;
	Foam	var;
	FoamTag type;

	for(i= 0; i< foamDDeclArgc(locals); i++) {
		Foam decl = locals->foamDDecl.argv[i];
		inlGetTypeFrDecl(decl, &type, &format);
		var = inlAddTempLocal(type, format);
		localArgv[i] = var;
	}
}

local void
inlGetTypeFrDecl(Foam decl, FoamTag *ptype, int *pformat)
{
	FoamTag type   = decl->foamDecl.type;
	int     format = decl->foamDecl.format;

	*ptype = type;
	if (type == FOAM_Rec || type == FOAM_Env || type == FOAM_TR)
		*pformat = inlGetFormat(format);
	else 
		*pformat = format;
}

/*
 * Create a new OptInfo structure.
 */
local OptInfo
inlInfoNew0(Stab stab, Foam prog, Syme lhs, Bool isGener, Bool external)
{
	OptInfo		new;

	new = (OptInfo) stoAlloc(OB_Other, sizeof(struct optInfo));

	new->inlState	 = (external ? INL_Inlined : INL_NotInlined);
	new->stab	 = stab;
	new->syme	 = lhs;
	new->prog	 = prog;
	new->seq	 = prog->foamProg.body;
	new->locals	 = vpNew(fboxNew(prog->foamProg.locals));
	new->numLabels	 = prog->foamProg.nLabels;
	new->denv	 = prog->foamProg.levels;
	new->seqBody	 = 0;

	new->localUsage	 = 0;
	new->constNum	 = -1;
	new->isGener	 = isGener;

	new->flog	 = 0;
	new->priq	 = 0;
	new->converged   = false;
	new->numRefs	 = 0;
	new->originalSize = 0;
	new->size	  = 0;
	new->optMask	  = 0xffff;

	return new;
}

OptInfo
inlInfoNew(Stab stab, Foam prog, Syme lhs, Bool isGener)
{
	return inlInfoNew0(stab, prog, lhs, isGener, false);
}


/*
 * return the Foam type of an arbitrary Foam expression.
 */
local FoamTag
inlExprType(Foam expr, AInt *fmt)
{
	return foamExprTypeG0(expr, inlProg->prog,
			     inlUnit->unit->foamUnit.formats,
			     inlProg->locals->fbox,
			     inlUnit->formats, inlUnit->globals, 
			     fmt);
}

local Foam
inlGetPushEnvFrVar(Foam env)
{

	if (inlProg->converged)
		return inuGetPushEnvFrVar(env);
	else
		return NULL;
}

local Foam
inlCanonEElt(Foam foam)
{	/* Strips EElts s.t. the index part is 0 */
	Foam env, next;
	int depth;

	env = foam->foamEElt.ref;
	depth = foam->foamEElt.level;

        while (depth > 0) {		
		next = inlGetPushEnvFrVar(env);
		if (!next) break;
		assert(foamTag(next) == FOAM_PushEnv);

		env = next->foamPushEnv.parent;
		depth--;
	}

	if (foamTag(env) == FOAM_Env) {
		depth = env->foamEnv.level+depth;
		assert(inlProg->denv->foamDEnv.argv[depth] == 0 ||
		       inlProg->denv->foamDEnv.argv[depth] == emptyFormatSlot ||
		       inlProg->denv->foamDEnv.argv[depth] == foam->foamEElt.env);
		inlProg->denv->foamDEnv.argv[depth] = foam->foamEElt.env;
		next = foamNewLex(depth,
				  foam->foamEElt.lex);
	}
	else if (env == foam->foamEElt.ref) 
		return foam;
	else
		next = foamNewEElt(foam->foamEElt.env, foamCopy(env), depth, 
				   foam->foamEElt.lex);

	foamSyme(next) = foamSyme(foam);
	foam = next;
	inlProg->changed = true;
	return foam;
}

local Foam
inlCanonEEnv(Foam foam)
{
	Foam env, next;
	int depth;

	env = foam->foamEEnv.env;
	depth = foam->foamEEnv.level;

	while (depth>0) {

		next = inlGetPushEnvFrVar(env);
		if (!next) break;
		assert(foamTag(next) == FOAM_PushEnv);

		env = next->foamPushEnv.parent;
		depth--;
	}

	if (foamTag(env) == FOAM_Env) {
		depth = env->foamEnv.level+depth;
		next = foamNewEnv(depth);
	}
	else if (foamTag(env) == FOAM_EEnv) {
		int level = depth + env->foamEEnv.level;
		assert(level >= 0); /* bug 1168 */
		next = foamNewEEnv(level, foamCopy(env->foamEEnv.env));
	}
	else if (env == foam->foamEEnv.env) 
		return foam;
	else if (depth == 0)
		next = foamCopy(env);
	else {
		assert(depth >= 0); /* bug 1168 */
		next = foamNewEEnv(depth, foamCopy(env));
	}

	foamSyme(next) = foamSyme(foam);
	foam = next;
	inlProg->changed = true;

	return foam;
}

local Foam
inlGetConstEnvFrClosVar(Foam op)
{
	if (inlProg->converged)
		return 	inuGetConstEnvFrClosVar(op);
	else
		return NULL;
}

/*
 * Get the environment of a variable in the value tracking table.
 */
local Foam
inlGetVarTableEnv(Foam op, Foam cenv)
{
	Foam	val, env;

	val = inlGetConstEnvFrClosVar(op);

	if (foamTag(op) == FOAM_Clos)
		env = op->foamClos.env;

	else if (val && (foamTag(val) == FOAM_Loc || foamTag(val) == FOAM_Lex))
		env = foamCopy(val);

	else if (cenv)
		env = cenv;

	else
		env = foamNewCEnv(foamCopy(op));

	if (val) foamSyme(env) = foamSyme(val);

	return env;
}

/*****************************************************************************
 *
 * :: Syme substitution for paramter function inlining.
 *
 ****************************************************************************/

/*
 * Substitute the type of a syme based on the exporter of the syme.
 */
local Syme
inlSubstitutedSyme(Syme syme)
{
	TForm	oldType, newType;
	TForm	oldExporter, newExporter;
	Syme	isyme, nsyme;

	isyme = inlInlinee->syme;

	if (isyme == NULL)
		return NULL;

	if (!symeIsImport(isyme))
		return syme;

	if (symeIsExport(syme)) {
	  	syme = inlSymeSubstSelf(syme, symeExporter(isyme));
		if (!syme)
			return NULL;
	}

	if (inlInlinee->sigma == NULL)
		inlInlinee->sigma =
			tfSatSubList(tfGetExpr(symeExporter(isyme)));

	if (inlInlinee->sigma == absFail())
		return NULL;

	if (absIsEmpty(inlInlinee->sigma))
		return syme;

	if (!symeIsImport(syme))
		return NULL;

	inlExportDEBUG({
		fprintf(dbOut, ">>inlSubstitutedSyme:\n");
		fprintf(dbOut, "    syme:\n");
		symeType(syme);
		symePrint(dbOut, syme);
		fnewline(dbOut);
		fprintf(dbOut, "    sigma:\n");
		absPrint(dbOut, inlInlinee->sigma);
		fnewline(dbOut);
	});

	oldExporter = symeExporter(syme);
	/* Bail if we don't know where this came from */
	if (!oldExporter) return NULL; 

	newExporter = tformSubst(inlInlinee->sigma, oldExporter);
	tfFollow(newExporter);
	tfHasSelf(newExporter) = false;

	oldType = symeType(syme);
	newType = tformSubst(inlInlinee->sigma, oldType);

	nsyme = tfHasDomImport(newExporter, symeId(syme), newType);

	inlExportDEBUG({
		fprintf(dbOut, "<<inlSubstitutedSyme:\n");
		fprintf(dbOut, "    syme:\n");
		symePrint(dbOut, nsyme);
		fnewline(dbOut);
	});

	return nsyme;
}

local Syme
inlSymeSubstSelf(Syme syme, TForm tfex)
{
	SymeList	symes;
	Syme		nsyme;

	symes = symeListSubstSelf(stabFile(), tfex,
				  listCons(Syme)(syme, listNil(Syme)));
	nsyme = symes->first;
	nsyme = tfHasDomImport(tfex, symeId(nsyme), symeType(nsyme));
	
	return nsyme;
}

/*
 * returns true when two denvs are exactly the same.
 * 0 => any format, hence not same.
 */
local Bool
inlSameDEnv(Foam denv1, Foam denv2)
{
	int i;
	if (foamArgc(denv1) != foamArgc(denv2)) return false;
	for(i=0; i<foamArgc(denv1); i++) {
		if (denv1->foamDEnv.argv[i] !=
		    denv2->foamDEnv.argv[i])
			return false;
		if (denv1->foamDEnv.argv[i] == 0)
			return false;
	}
	return true;
}

/******************************************************************************
 *
 * :: Always Inline Generators (-Wloops)
 *
 *****************************************************************************/

void
inlSetGenerators()
{
	inlInlineGenerators = true;
}

/*****************************************************************************
 *
 * :: Debug Staff
 *
 ****************************************************************************/

void
inlPrintRejectCause(String call)
{
	String str = ""; /* Quit warnings */

	if (!inlCallInfoDebug) return;

	if (inlConstTrace != -1 &&
	    inlConstTrace != inlProg->constNum) return;
        
	switch (inlRejectInfo) {
	case INL_REJ_Unknown: str = "Unknown"; break;
	case INL_REJ_NotConstSyme: str = "NotConstSyme"; break;
	case INL_REJ_LocalFoamFail: str = "LocalFoamFail"; break;
	case INL_REJ_ExternalProgHdrFail: str = "ExternalProgHdrFail"; break;
	case INL_REJ_LocalConstInfoFail: str = "LocalConstInfoFail"; break;
	case INL_REJ_NotConstClosProg: str = "NotConstClosProg"; break;
	case INL_REJ_NoPermission: str = "NoPermission"; break;
	case INL_REJ_Getter: str = "Getter"; break;
	case INL_REJ_Evil: str = "Evil"; break;
	case INL_REJ_Fluids: str = "Fluids"; break;
	case INL_REJ_OCalls: str = "OCalls"; break;
	case INL_REJ_NoInlineInfo: str = "NoInlineInfo"; break;
	case INL_REJ_DontInlineMe: str = "DontInlineMe"; break;
	case INL_REJ_RecursiveCall: str = "RecursiveCall"; break;
	case INL_REJ_LocalInConst0: str = "LocalCallInConst(0)"; break;
	default:
		bugWarning("Unknown rejection cause...");
	}

	fprintf(dbOut, "****(uninlined: %s [%d]) Reason: %s ****\n",
			call, inlCallInfoSerial, str);
}


void
inlPrintUninlinedCalls(InlPriCall pric, PriQKey pri)
{
	InlPriCall 	priCall;
	PriQKey	       	priority;

	if (pric == NULL) return;

	fprintf(dbOut, "(Over limit in prog: %d, originalSize = %d, limit = %d\n", inlProg->constNum, inlProg->originalSize, inlSizeLimit);

       foamPrintDb(pric->call);
       fprintf(dbOut, "(priority = %f, size = %d)\n", 
	       pri, (int)pric->size);

       inlPrintPriq();
       while (priqCount(inlProg->priq)) {
	       priCall =(InlPriCall) priqExtractMin(inlProg->priq, &priority);

	       foamPrintDb(priCall->call);
	       fprintf(dbOut, "(priority = %f, size = %d)\n", 
		       priority, (int)priCall->size);
       }

	fprintf(dbOut, "limit end)\n");
}
