
/*****************************************************************************
 *
 * genfoam.c: Foam code generation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file implements Foam code generation from fully type-analyzed AbSyn
 * trees for an entire file.
 *
 * To Do:
 *	Use hash function from domains for non-type constructor args.
 *	Treat arg and return types as Multi in gen0TypeHash.
 *	Completely compute side-effects for functions and all sub-expressions
 *		once only.  Get rid of potential exponential process in
 *		dead var elimination.
 *	Ensure type analysis puts embed tags on assignments (it doesn't now).
 *	Compute environment level usage (prog's DEnv) after all foam
 *		generation and optimization is done, not in genfoam.c.
 *	Use Aldor-level code and callbacks for iterators.
 *	Clean up storage leaks.
 *
 */

#include "compcfg.h"
#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "fortran.h"
#include "genfoam.h"
#include "gf_add.h"
#include "gf_excpt.h"
#include "gf_fortran.h"
#include "gf_gener.h"
#include "gf_imps.h"
#include "gf_prog.h"
#include "gf_reference.h"
#include "gf_rtime.h"
#include "gf_syme.h"
#include "gf_util.h"
#include "of_inlin.h"
#include "of_util.h"
#include "optfoam.h"
#include "opttools.h"
#include "scobind.h"
#include "sefo.h"
#include "simpl.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "tform.h"
#include "util.h"
#include "fint.h"
#include "lib.h"
#include "tfsat.h"
#include "ablogic.h"
#include "abpretty.h"

Bool	genfoamDebug		= false;
Bool	genfoamHashDebug	= false;
Bool	genfoamConstDebug	= false;

extern Bool genfExportDebug;	/* (from gf_add.c) */
#define genfExportDEBUG(s)	DEBUG_IF(genfExportDebug, s)

CREATE_LIST (DomainCache);
CREATE_LIST (VarPool);
CREATE_LIST (FoamSig);

/*****************************************************************************
 *
 * :: Standard code generators.
 *
 ****************************************************************************/

local Foam	genFoam		    (AbSyn);
local Foam	genApply	    (AbSyn);
local Foam	genAssert	    (AbSyn);
local Foam	genAssign	    (AbSyn);
local Foam	genAnd		    (AbSyn);
local Foam	genOr		    (AbSyn);
local Foam	genDefine	    (AbSyn);
local Foam	genExcept	    (AbSyn);
local Foam	genId		    (AbSyn);
local Foam	genIf		    (AbSyn);
local Foam	genRepeat	    (AbSyn);
local Foam	genReturn	    (AbSyn);
local Foam	genLabel	    (AbSyn);
local Foam	genGoto		    (AbSyn);
local Foam	genSequence	    (AbSyn);
local Foam	genLit		    (AbSyn);
local Foam	genCollect	    (AbSyn);
local Foam	genPretend	    (AbSyn);
local Foam	genMulti	    (AbSyn);
local Foam	genNever	    (AbSyn);
local Foam	genWhere	    (AbSyn);
local Foam	genExport	    (AbSyn);
local Foam	genSelect	    (AbSyn);
local Foam	genForeign	    (AbSyn);
local Foam	genRestrict	    (AbSyn);

/*****************************************************************************
 *
 * :: Local helper functions.
 *
 ****************************************************************************/

/* !!! No longer used !!! */
#if 0
local AInt         gen0ProtoFrString      (String);
local Bool         gen0IsParamReference   (TForm, int);
local String	   gen0FortranFieldName	  (TForm, int);
local int            gen0GetNumFortranModArgs     (TForm);
local Bool           gen0FortranHasFnParamArg     (TForm);
local AInt           gen0FortranMFmtNumber        (TFormList);
#endif

local Bool	   gen0AbSynHasConstHash  (AbSyn);
local Bool	   gen0AbSynHasConstHash0 (AbSyn);
local AInt	   gen0AddRealFormat	  (Foam);
local Foam	   gen0ApplySyme	  (FoamTag, Syme, SImpl, Length, Foam **);
local Foam	   gen0ApplyBuiltin	  (Syme, Length, Foam **);
local Foam	   gen0ApplyForeign	  (FoamTag, Syme, Length, Foam **);
local Foam	   gen0ApplyImplicitSyme  (FoamTag, Syme, Length, AbSyn *, Foam);
local Foam	   gen0AssignLhs	  (AbSyn, Foam);
local AbSyn *	   gen0MakeImplicitArgs   (Length, AbSyn, AbSynGetter);
local AInt	   gen0BuiltinImport	  (String, String);
local Foam	   gen0BuiltinExporter	  (Foam, Syme);
local Foam	   gen0CCall		  (FoamTag, Syme, Length, Foam **);
local Bool	   gen0CompareFormats	  (Foam, Foam);
local void	   gen0ComputeSideEffects (Foam prog);
local Foam	   gen0CrossToMulti	  (Foam, TForm);
local Foam	   gen0CrossToUnary	  (Foam, TForm);
local Foam	   gen0CrossToTuple	  (Foam, TForm);
local Foam	   gen0Define		  (AbSyn);
local Foam	   gen0DefineRhs	  (AbSyn, AbSyn, AbSyn);
local Foam	   gen0Embed		  (Foam, AbSyn, TForm, AbEmbed);
local Symbol	   gen0ExportingTo	  (AbSyn absyn);
local void	   gen0ExportToBuiltin	  (AbSyn fun);
local void	   gen0ExportToC	  (AbSyn fun);
local Foam	   gen0Extend		  (AbSyn);
local void	   gen0FindDefs		  (AbSyn, AbSyn, Stab, Bool, Bool );
local int	   gen0FoamLevel	  (AInt level);
local FoamTag	   gen0FoamType		  (Foam foam);
local void	   gen0ForIter		  (AbSyn, FoamList *, FoamList *);
local void	   gen0FreeTemp		  (Foam);
local void	   gen0GenFoamInit	  (void);
local void	   gen0GenFoamFini	  (void);
local Syme	   gen0FindImportedSyme   (Syme, AInt, Bool);
local Bool	   gen0GetImportedSyme	  (Syme, AInt, Bool);
local void	   gen0SetImportedSyme	  (Syme, AInt);
local Foam	   gen0GetRealFormat      (AInt);
local Foam	   gen0ImplicitSet	  (AbSyn);
local void	   gen0InitState	  (Stab, int);
local Foam	   gen0InnerSyme	  (Syme, AInt);
local Bool	   gen0IsDomainInit	  (Foam foam);
local Bool	   gen0IsDef		  (AbSyn);
local Bool	   gen0IsImplicitSet	  (AbSyn lhs);
local Bool	   gen0IsInnerVar	  (Syme syme, AInt level);
local Bool	   gen0IsMultiEvaluable	  (Foam);
local Bool	   gen0IsResultCachable	  (AbSyn, TForm);
local void	   gen0Iter		  (AbSyn, FoamList *, FoamList *);
local Foam	   gen0Lambda		  (AbSyn, Syme, AbSyn);
local Symbol	   gen0MachineType	  (TForm);
local Foam *	   gen0MakeApplyArgs	  (AbSyn, Length *);
local Foam	   gen0EmbedApply	  (int, AbSyn *, AbSyn, AbEmbed);
local void	   gen0MakeBuiltinExports (void);
local Foam	   gen0MakeExtend	  (Syme, TForm);
local Foam	   gen0MakeExtendLambda   (Syme, TForm);
local Foam	   gen0MakeExtendBase	  (Syme);
local void	   gen0MakeExtendParents  (Syme, SymeList);
local void	   gen0MakeExtendParent   (Syme, Syme, Length, Foam);
local Foam	   gen0MakeExtendApply    (TForm, Foam);
local AbSyn	   gen0AbTypeArg	  (AbSyn);
local void	   gen0MarkParamsDeep	  (Stab, AbSyn);
local int	   gen0MaxLevel		  (AbSyn);
local Foam	   gen0MultiAssign	  (FoamTag, AbSyn, Foam);
local Foam	   gen0MultiToCross	  (Foam, TForm);
local Foam	   gen0MultiToTuple	  (Foam);
local Foam	   gen0MultiToUnary	  (Foam);
local Foam	   gen0OCall		  (FoamTag, Syme, Length, Foam **);
local void	   gen0PatchEEltFormats   (Foam);
local void	   gen0PatchFormatNums	  (Foam);
local Foam	   gen0RawToUnary	  (Foam, AbSyn);
local String	   gen0RecFieldName	  (TForm, int);
local String	   gen0RecFieldName	  (TForm, int);
local Foam	   gen0RenewConstants	  (FoamList, int);
local Foam	   gen0RenewDefs	  (FoamList, int);
local Foam	   gen0Sequence		  (TForm, AbSyn *, Length, Length);
local void	   gen0SetTemp		  (Foam, Foam);
local Foam	   gen0SetValue		  (Foam, AbSyn);
local Foam	   gen0SpecialOp	  (FoamTag, Syme, Length, AbSyn *, Foam *);
local Syme	   gen0SymeCopyImport	  (Syme);
local Foam	   gen0SymeGeneric	  (Syme);
local Foam	   gen0SymeImport	  (Syme);
local Foam	   gen0TempValue	  (AbSyn);
local Foam	   gen0TempValueMode	  (TForm);
local Foam	   gen0UnaryToMulti	  (Foam);
local Foam	   gen0UnaryToCross	  (Foam, TForm);
local Foam	   gen0UnaryToRaw	  (Foam, AbSyn);
local Foam	   gen0UnaryToTuple	  (Foam);
local AInt 	   gen0CrossFormatNumber  (TForm tf);
local AInt	   gen0TrailingFormatNumber(TForm tf);
local AIntList	   gen0UnusedFormats	  (AIntList);
local void	   gen0UseFormat	  (AInt level, int slot);
local void	   gen0UseStateFormat	  (GenFoamState, AInt);
local void	   gen0VarsList		  (Stab, SymeList);
local void	   gen0VarsParam	  (Syme, Stab);
local void	   gen0VarsLex		  (Syme, Stab);
local void	   gen0VarsFluid	  (Syme);
local void	   gen0VarsImport	  (Syme, Stab);
local void	   gen0VarsExport	  (Syme, Stab);
local void	   gen0VarsForeign	  (Syme);
local Foam	   gen0ForeignValue	  (Syme);
local Foam	   gen0ForeignWrapValue	  (Syme);
local Foam	   gen0ForeignWrapFn	  (Syme);
local void	   gen0FreeFortranActualArgTmps(void);
local Foam	   genFoamArg		  (AbSyn *, Foam *, int);
local void	   gen0GetGlobalDefs	  (void);
local SymeList     gen0GetExporterSymes   (Stab, Sefo, SymeList);
local SymeList	   gen0GetBoundSymes	  (Stab);	
local void 	   gen0AddLexLevels1      (Foam, AInt, int, FoamList);
local int	   gen0StateOffset	  (int, int);
local AInt	   gen0FindFormat	  (AInt);
local AInt	   gen0FindConst	  (AInt);

local AInt	   gen0FindGlobalFluid    (Syme);
local void 	   gen0AddLocalFluid	  (AbSyn);

local void	   gen0DbgAssignment	  (AbSyn);
local void	   gen0DbgFnEntry	  (AbSyn);
local Foam	   gen0DbgFnExit	  (AbSyn, Foam);
local Foam	   gen0DbgFnReturn	  (AbSyn, Foam);

/* Code for the new runtime debugging system */
local Foam	   gen1DbgFnBody	  (AbSyn);
local void	   gen1DbgFnEntry	  (TForm, Syme, AbSyn);
local void	   gen1DbgFnExit	  (AbSyn);
local void	   gen1DbgFnReturn	  (AbSyn, TForm, Foam);
local void	   gen1DbgDoParam	  (String, AInt, Syme, AInt);
local void	   gen1DbgDoAssign	  (String, AInt, Syme, Foam, AInt, AInt);
local void	   gen1DbgFnStep	  (AbSyn);

local Bool	   gen0IsOpenCallable(Syme syme, SImpl impl);

local AInt	   gen0FortranSigFormatNumber   (TForm, Bool);
local String       gen0TypeString               (Sefo);


/*
 * The following are used store information for flattening programs.
 */

AIntList	gen0BuiltinExports;
static int	numLexicals;


String	 gen0FileName;
#ifdef NEW_FORMATS
FoamList gen0GlobalList, gen0FluidList, gen0FormatList, gen0ParamsList;
int	 gen0NumParams;
#else
FoamList gen0GlobalList, gen0FluidList, gen0FormatList;
#endif
FoamList gen0DeclList, gen0ProgList;
AIntList gen0ConstList;
AIntList gen0RealConstList;
int	 gen0NumGlobals;
int	 gen0RealFormatNum;
int	 gen0NumProgs;
int 	 gen0FwdProgNum;
int	 gen0FormatNum;
int	 gloInitIdx;

static AIntList		formatPlaceList, formatRealList;
static int		gen0IterateLabel;
static int		gen0BreakLabel;
static Bool		gen0ValueMode;	/* indicate when in value mode. */

int			gen0GenerFormat;
int			gen0GenerRetFormat;

int 			gen0LazyFunFormat;
FoamSigList 		gen0LazySigList;
AIntList		gen0LazyConstTypeList;
AIntList		gen0LazyConstDefnList;

static FoamSigList	gen0ForeignFnValues;
static SymeList		gen0ForeignFnGlobals;

String			gen0DefName; /* Similar to gen0ProgName */
String			gen0ProgName;
GenFoamState		gen0State;

static AbSyn            gen0FortranFnResult = NULL;
static FoamList         gen0FortranActualArgTmps = listNil(Foam); 


/* Flags for options */
Bool 			gen0InAxiomAx	   = false;
Bool 			gen0DebugWanted    = false; /* Old style */
Bool 			gen0DebuggerWanted = false; /* New style */
Bool 			gen0SmallHashCodes = false;

/* 2 flavours of gen0AbType. The first returns the natural type
 * of an absyn, the second also allows for embeddings. 
 */
TForm
gen0AbType(AbSyn ab)
{
	return tfDefineeType(abGetCategory(ab));
}

TForm 
gen0AbContextType(AbSyn ab)
{
	TForm 	tf = gen0AbType(ab);
	AbEmbed embed = abTContext(ab);

	return tfsEmbedResult(tf, embed);
}

#define gen0FoamNewBreak()	foamNewGoto(gen0BreakLabel)
#define gen0FoamNewIterate()	foamNewGoto(gen0IterateLabel)

void	
genSetAxiomAx(Bool flag)
{
	gen0InAxiomAx = flag;
}
 
void
genSetDebugWanted(Bool flag)
{
	gen0DebugWanted = flag;
}

void
genSetDebuggerWanted(Bool flag)
{
	gen0DebuggerWanted = flag;
}

void
genSetSmallHashCodes(Bool flag)
{
	gen0SmallHashCodes = flag;
}

/******************************************************************************
 *
 * :: Top-level entry point.
 *
 *****************************************************************************/

Foam
generateFoam(Stab stab0, AbSyn absyn, String initName)
{
	Scope("generateFoam");

#ifdef NEW_FORMATS
	Foam		foam, clos, constants, def, params,
			globals, lexicals, formats, defs, decl, fluids;
#else
	Foam		foam, clos, constants, def,
			globals, lexicals, formats, defs, decl, fluids;
#endif
	int		index, gloNOpIdx, gloRuntimeIdx;
	AbLogic		fluid(gfCondKnown); /* COND-DEF */

	/* COND-DEF */
	gfCondKnown = gfCondKnown ? ablogCopy(gfCondKnown) : ablogTrue();

	gen0GenFoamInit();

	gen0FileName		= initName;

	gen0ProgName		= gen0FileName;
	gen0DefName		= strCopy(gen0ProgName);


	/* Walk absyn for open callable funcs and lexically deep references. */
	gen0FindDefs(absyn, NULL, stab0, false, true);

	/* Declare the globals for the top-level prog. */

	/* Once called, never again */
        decl = foamNewGDecl(FOAM_Clos, strCopy(gen0ProgName), emptyFormatSlot,
			    FOAM_GDecl_Export, FOAM_Proto_Init);
	gloInitIdx = gen0AddGlobal(decl);

        decl = foamNewGDecl(FOAM_Clos, strCopy("noOperation"), emptyFormatSlot,
			    FOAM_GDecl_Import,FOAM_Proto_Foam);
	gloNOpIdx = gen0AddGlobal(decl);


	/* Set up this level */

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(gen0ProgName, absyn);

	index = gen0FormatNum--;
	gen0InitState(stab0, index);

	gen0ProgAddParams(int0, NULL);
	gen0Vars(stab0);

	gen0State->program = foam;

	/* Add the inits for the globals for the top-level prog. */

	/* This can go once support for lazy gets of runtime functions is in... */

	if (!genIsRuntime()) {
		decl = foamNewGDecl(FOAM_Clos, gen0InitialiserName("runtime"),
				    emptyFormatSlot,
				    FOAM_GDecl_Import,
				    FOAM_Proto_Init);
		gloRuntimeIdx = gen0AddGlobal(decl);
		gen0AddInit(foamNew(FOAM_CCall, 2, FOAM_NOp,
				    foamNewGlo(gloRuntimeIdx)));
		gen0AddInit(foamNewSet(foamNewGlo(gloInitIdx),
				       foamNewGlo(gloNOpIdx)));
	}

	if (!genIsRuntime())
		gen0StringsInit();

	if (genIsRuntime())
		gen0StdLazyGetsCreate();

	/* !! Should call AddExportedSymes on body */
	gen0FindUncondSymes(absyn, listNil(Syme));


	gen0DefSequence(absyn);

	if (genIsRuntime()) {
		gen0AddStmt(foamNewSet(foamNewGlo(gloInitIdx), 
				       foamNewGlo(gloNOpIdx)), NULL);
	}

	if (!genIsRuntime())
		gen0StringsFini();

	gen0IssueLazyFunctions();
	gen0IssueGVectFns();

	gen0AddStmt(foamNewReturn(foamNew(FOAM_Values, int0)), NULL);

	gen0ProgAddFormat(index);
	gen0IssueDCache();
	gen0ProgFiniEmpty(foam, FOAM_NOp, int0);

	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	/* construct the unit */
	constants   = gen0RenewConstants(gen0DeclList, gen0NumProgs);

	def = foamNewDef(foamNewGlo(gloInitIdx), clos);
	gen0ProgList = listCons(Foam)(def, gen0ProgList);
	gen0NumProgs++;

	gen0GetGlobalDefs();
	gen0MakeBuiltinExports();
	defs	    = gen0RenewDefs(gen0ProgList, gen0NumProgs);

#ifdef NEW_FORMATS
	gen0ParamsList	= listNReverse(Foam)(gen0ParamsList);
	params = foamNewOfList(FOAM_DDecl, gen0ParamsList);
#endif
	/* Fixup defns of globals... */

	gen0GlobalList	= listNReverse(Foam)(gen0GlobalList);
	globals	    = foamNewOfList1(FOAM_DDecl, 
				     (AInt) FOAM_DDecl_Global, 
				     gen0GlobalList);
	lexicals    = foamNewEmptyDDecl(FOAM_DDecl_LocalEnv);
	fluids  = foamNewOfList1(FOAM_DDecl, FOAM_DDecl_Fluid, gen0FluidList);
	gen0FormatList	= listNReverse(Foam)(gen0FormatList);

	gen0FormatList	= listCons(Foam)(foamNewEmptyDDecl(FOAM_DDecl_Local),
					 gen0FormatList);
	gen0FormatList	= listCons(Foam)(fluids, gen0FormatList);
	gen0FormatList	= listCons(Foam)(lexicals, gen0FormatList);
	gen0FormatList	= listCons(Foam)(constants, gen0FormatList);
	gen0FormatList	= listCons(Foam)(globals, gen0FormatList);


	formats = foamNewOfList(FOAM_DFmt, gen0FormatList);
	foam = foamNewUnit(formats, defs);

	if (fintMode == FINT_LOOP)
		gen0KillSymeConstNums();

	/* Tidy up */	    
	gen0GenFoamFini();
	assert(foamAudit(foam));

	/* COND-DEF */
	ablogFree(gfCondKnown);

	Return(foam);
}

/*
 * !! This function does not appear to be used anymore !!
 */
#if 0
local AInt
gen0ProtoFrString(String proto)
{
	if (!proto)				return FOAM_Proto_C;
	else if (!strcasecmp(proto, "C"))	return FOAM_Proto_C;
	else if (!strcasecmp(proto, "Lisp"))	return FOAM_Proto_Lisp;
	else if (!strcasecmp(proto, "Fortran"))	return FOAM_Proto_Fortran;

	return FOAM_Proto_C;
}
#endif

local void
gen0GenFoamInit()
{
	/* initialise globals */
	gen0GenerFormat		= 0;
	gen0GenerRetFormat	= 0;
	gen0TupleFormat		= 0;
	gen0ArrayFormat		= 0;
	gen0UnionFormat		= 0;
	gen0CCheckFormat	= 0;
	gen0LazyFunFormat	= 0;
	gen0State		= 0;

	gen0BuiltinExports = listNil(AInt);
	gen0FormatList		= listNil(Foam);
	gen0ProgList		= listNil(Foam);
	gen0DeclList		= listNil(Foam);
	gen0GlobalList		= listNil(Foam);
	gen0FluidList		= listNil(Foam);
	gen0ConstList		= listNil(AInt);
	gen0RealConstList	= listNil(AInt);
#ifdef NEW_FORMATS
	gen0ParamsList		= listNil(Foam);
	gen0NumParams		= 0;
#endif
	gen0NumGlobals		= 0;
	gen0FwdProgNum		= SYME_NUMBER_UNASSIGNED - 1;
	gen0NumProgs		= 0;
	numLexicals		= 0;
	gen0FormatNum		= -1;

	gen0LazySigList		= listNil(FoamSig);
	gen0LazyConstTypeList   = listNil(AInt);
	gen0LazyConstDefnList	= listNil(AInt);
	gen0ForeignFnValues	= listNil(FoamSig);
	gen0ForeignFnGlobals	= listNil(Syme);
	
	gen0InitConstTable(scobindMaxDef());
	formatPlaceList		= listNil(AInt);
	formatRealList		= listNil(AInt);
	gen0RealFormatNum	= FOAM_FORMAT_START;

	gen0InitGVectTable();
}

local void
gen0GenFoamFini()
{
	listFree(AInt)(gen0BuiltinExports);
	listFree(Foam)(gen0FormatList);
	listFree(Foam)(gen0ProgList);
	listFree(Foam)(gen0DeclList);
#ifdef NEW_FORMATS
	listFree(Foam)(gen0ParamsList);
#endif
	listFree(Foam)(gen0GlobalList);
	listFree(Foam)(gen0FluidList);
	listFree(AInt)(gen0ConstList);
	listFree(AInt)(gen0RealConstList);
	listFree(FoamSig)(gen0ForeignFnValues);
	listFree(Syme)(gen0ForeignFnGlobals);

	listFree(FoamSig)(gen0LazySigList);
	listFree(AInt)(gen0LazyConstTypeList);
	listFree(AInt)(gen0LazyConstDefnList);
	listFree(AInt)(formatPlaceList);
	listFree(AInt)(formatRealList);

	gen0FiniGVectTable();
}

void
genFoamStmt(AbSyn absyn)
{
	Scope("genFoamStmt");
	Foam	 foam;
	Bool	 fluid(gen0ValueMode);
	FoamList fluid(gen0FortranActualArgTmps);

	gen0ValueMode = false;
	gen0FortranActualArgTmps = NULL;


	/* New-style debugging hook */
	if (gen0DebuggerWanted)
		gen1DbgFnStep(absyn);

	foam = genFoam(absyn);

	if (foam) gen0AddStmt(foam, absyn);

	if (gen0FortranActualArgTmps) gen0FreeFortranActualArgTmps();		

	ReturnNothing;
}

Foam
genFoamVal(AbSyn absyn)
{
	Scope("genFoamVal");
	Foam	foam;
	Bool	fluid(gen0ValueMode);

	gen0ValueMode = true;
	foam = genFoam(absyn);
	if (foam && absyn)
		foamPos(foam) = abPos(absyn);
	if (!foam)
		foam = foamNewNil();

	Return(foam);
}

Foam
genFoamType(AbSyn ab)
{
	AbEmbed	tc = abTContext(ab);
	Foam	val;

	abSetTContext(ab, AB_Embed_Identity);
	val = genFoamVal(ab);
	abSetTContext(ab, tc);

	return val;
}

Foam
genFoamCast(Foam foam, AbSyn ab, FoamTag type)
{
	if (type != gen0Type(gen0AbContextType(ab), NULL))
		foam = foamNewCast(type, foam);

	return foam;
}

Foam
genFoamBit(AbSyn ab)
{
	return genFoamCast(genFoamVal(ab), ab, FOAM_Bool);
}

/*****************************************************************************
 *
 * :: Main recursive entry point.
 *
 ****************************************************************************/

local Foam
genFoam(AbSyn absyn)
{
	static int	genDepth = 0;
	Foam		foam = 0;
	AbSynTag	tag;
	String		msg;

	if (!absyn) return NULL;

	tag = abTag(absyn);

	genDepth += 1;

	genfDEBUG({
		fprintf(dbOut, "%*sGenerating %s from ",
			genDepth, "", abInfo(tag).str);
		sposPrint(dbOut, abPos(absyn));
		fprintf(dbOut, "\n");
	});

	switch (tag) {
	case AB_Apply:
		foam = genApply(absyn);
		break;
	case AB_Assert:
		foam = genAssert(absyn);
		break;
	case AB_Assign:
		foam = genAssign(absyn);
		break;
	case AB_Id:
		foam = genId(absyn);
		break;
	case AB_LitInteger:
	case AB_LitFloat:
	case AB_LitString:
		foam = genLit(absyn);
		break;
	case AB_If:
		foam = genIf(absyn);
		break;
	case AB_Try:
		foam = genTry(absyn);
		break;
	case AB_Raise:
		foam = gen0Raise(absyn);
		break;
	case AB_Generate:
		foam = genGenerate(absyn);
		break;
	case AB_Collect:
		foam = genCollect(absyn);
		break;
	case AB_Lambda:
	case AB_PLambda:
		foam = gen0Lambda(absyn, NULL, NULL);
		break;
	case AB_Do:
		foam = genFoam(absyn->abDo.expr);
		break;
	case AB_Sequence:
		foam = genSequence(absyn);
		break;
	case AB_Define:
		foam = genDefine(absyn);
		break;
	case AB_Label:
		foam = genLabel(absyn);
		break;
	case AB_Goto:
		foam = genGoto(absyn);
		break;
	case AB_Not:
		foam = foamNotThis(genFoamBit(absyn->abNot.expr));
		foam = foamNewCast(FOAM_Word, foam);
		break;
	case AB_Or:
		foam = genOr(absyn);
		break;
	case AB_And:
		foam = genAnd(absyn);
		break;
	case AB_Reference:
		foam = genReference(absyn);
		break;
	case AB_Repeat:
		foam = genRepeat(absyn);
		break;
	case AB_Yield:
		foam = genYield(absyn);
		break;
	case AB_Add:
		foam = genAdd(absyn);
		break;
	case AB_Return:
		genReturn(absyn);
		break;
	case AB_Break:
		gen0AddStmt(gen0FoamNewBreak(), absyn);
		break;
	case AB_Iterate:
		gen0AddStmt(gen0FoamNewIterate(), absyn);
		break;
	case AB_Never: 
		genNever(absyn);
		break;
	case AB_CoerceTo:
		foam = genImplicit(absyn, absyn->abCoerceTo.expr,
				   gen0Type(gen0AbType(absyn), NULL));
		break;
	case AB_PretendTo:
		foam = genPretend(absyn);
		break;
	case AB_RestrictTo:
		foam = genRestrict(absyn);
		break;
	case AB_Qualify:
		foam = genFoam(absyn->abQualify.what);
		break;
	case AB_Except:
		foam = genExcept(absyn);
		break;
 	case AB_Test:
		foam = genImplicit(absyn, absyn->abTest.cond, FOAM_Bool);
		break;
	case AB_Local: {
		int i;
		for (i=0; i<abArgc(absyn); i++)
			genFoamStmt(absyn->abLocal.argv[i]);
		break;
		}
	case AB_Fluid: {
		int i;
		for (i=0; i<abArgc(absyn); i++) {
			if (abTag(absyn->abFluid.argv[i]) == AB_Assign)
				gen0AddLocalFluid(
				        absyn->abFluid.argv[i]->abAssign.lhs);
			genFoamStmt(absyn->abFluid.argv[i]);
		}
		break;
		}
	case AB_Free: {
		int i;
		for (i=0; i<abArgc(absyn); i++)
			genFoamStmt(absyn->abFree.argv[i]);
		break;
	}
	case AB_Comma:
		foam = genMulti(absyn);
		break;
	case AB_Where:
		foam = genWhere(absyn);
		break;
	case AB_Declare:
		if (gen0ValueMode)
			foam = genFoamVal(absyn->abDeclare.id);
		break;
	case AB_Builtin:
	case AB_Import:
	case AB_Nothing:
	case AB_Inline:
		break;
	case AB_Foreign:
		genForeign(absyn);
		break;
	case AB_Has:
		foam = genHas(absyn);
		break;
	case AB_With:
		foam = genWith(absyn);
		break;
	case AB_Default:
		genFoamStmt(absyn->abDefault.body);
		break;
	case AB_Export:
		genExport(absyn);
		break;
	case AB_Extend:
		genFoam(absyn->abExtend.body);
		break;
	case AB_Select:
		foam = genSelect(absyn);
		break;
	default:
		msg = strPrintf("unsupported absyn (%s) found by genFoam",
			abInfo(abTag(absyn)).str);
		comsgFatal(absyn, ALDOR_F_Bug, msg);
#if 0
		sposPrint(stdout, abPos(absyn));
		fprintf	 (stdout, " abTag = %s\n", abInfo(abTag(absyn)).str);
		bug("genFoam");
#endif
	}

	if (foam && abEmbedArg(absyn))
		foam = gen0Embed(foam, absyn, gen0AbType(absyn),
				 abEmbedArg(absyn));
	genDepth -= 1;

	return foam;
}

/*****************************************************************************
 *
 * :: Specific generators.
 *
 ****************************************************************************/

/*
 * Generate code for export to Builtin.
*/
local Foam
genExport(AbSyn absyn)
{
	AbSyn	what = absyn->abExport.what;
	Symbol	sym  = gen0ExportingTo(absyn->abExport.destination);
	int	i, argc;
	AbSyn	*argv;

        if (!sym) return 0;

	switch (abTag(what)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Sequence:
		argc = abArgc(what);
		argv = abArgv(what);
		break;
	default:
		argc = 1;
		argv = &what;
		break;
	}

	for (i = 0; i < argc; i += 1) {
		AbSyn ab = argv[i];
		genFoamStmt(ab);
		if (sym == ssymBuiltin)
			gen0ExportToBuiltin(ab);
		else if (sym == ssymC)
			gen0ExportToC(ab);
		else if (sym == ssymFortran)
			gen0ExportToFortran(ab);
	}
	return 0;
}

local Symbol
gen0ExportingTo(AbSyn absyn)
{
	if (abIsApplyOf(absyn, ssymForeign) &&
	    abApplyArgc(absyn) == 1 &&
	    abTag(abApplyArg(absyn, int0)) == AB_Id)
		return abApplyArg(absyn, int0)->abId.sym;
	  else
		return NULL;
}

/*
 * Create a global variable binding for exporting to Builtin.
 */
local void
gen0ExportToBuiltin(AbSyn absyn)
{
	AbSyn	name = abDefineeId(absyn);
	Syme	syme = abSyme(name);
	TForm	tf;
	FoamTag	rtype;
	Foam	decl;
	int	index, progId;

	assert(syme);
	tf = symeType(syme);
	
	rtype = tfIsMap(tf) ? gen0Type(tfMapRet(tf), NULL) : FOAM_Nil;
	decl = foamNewGDecl(gen0Type(tf, NULL), strCopy(symeString(syme)),
			    emptyFormatSlot,
			    FOAM_GDecl_Export,
			    FOAM_Proto_Foam);
	foamGDeclSetRType(decl, rtype);
	
	index = gen0AddGlobal(decl);
	
	if (!tfIsMap(tf))
		progId = 0;
	else if (genIsRuntime()) {
		Foam glo = foamNewGlo(index);
		progId = 0;
		gen0AddStmt(foamNewSet(glo, gen0Syme(syme)), absyn);
	}
	else {
		Foam glo = foamNewGlo(index);
		Foam clos = gen0BuiltinExporter(glo, syme);
		progId = clos->foamClos.prog->foamConst.index;
		gen0AddStmt(foamNewSet(glo, gen0Syme(syme)), absyn);
	}
	gen0BuiltinExports = listCons(AInt)(index, gen0BuiltinExports);
	gen0BuiltinExports = listCons(AInt)(progId, gen0BuiltinExports);
}

/*
 * Create a global variable binding for exporting to C.
 */
local void
gen0ExportToC(AbSyn absyn)
{
	AbSyn	name = abDefineeId(absyn);
	Syme	syme = abSyme(name);
	TForm	tf;
	FoamTag	rtype;
	AInt	init = gloInitIdx;
	Foam	decl;
	AInt	index;

	assert(syme);
	tf = symeType(syme);
	rtype = tfIsMap(tf) ? gen0Type(tfMapRet(tf), NULL) : FOAM_Nil;

	if (!(gen0State->tag == GF_File &&
	      stabLevelNo(gen0State->stab) == 1)) {
		AbSyn	exporter = gen0ProgGetExporter();
		if (exporter && abHasTag(exporter, AB_Id)) {
			Syme	xsyme = abSyme(exporter);
			if (xsyme && gen0FoamKind(xsyme) == FOAM_Glo)
				init = gen0VarIndex(abSyme(exporter));
		}
	}

	/*!! Assumes export to C is exporting a function! */
	decl = foamNewGDecl(FOAM_Clos, strCopy(symeString(syme)),
			    init, FOAM_GDecl_Export, FOAM_Proto_C);
	foamGDeclSetRType(decl, rtype);

	index = gen0AddGlobal(decl);
	gen0BuiltinExports = listCons(AInt)(index, gen0BuiltinExports);
	gen0BuiltinExports = listCons(AInt)(int0, gen0BuiltinExports);

	gen0AddStmt(foamNewSet(foamNewGlo(index), gen0Syme(syme)), absyn);
}


/*
 * Generate Foreign inclusion hints.
 */
local Foam
genForeign(AbSyn absyn)
{
	AbSyn		origin = absyn->abForeign.origin;
	ForeignOrigin	forg;
	Foam		decl;

	/*
	 * Skip Foreign(Builtin): it was invented by the compiler
	 * in abnImport() as a useful way of getting qualified
	 * imports. However, we don't want forgFrAbSyn complaining
	 * that it isn't a valid protocol (it isn't).
	 */
	if (abIsTheId(origin, ssymBuiltin)) return (Foam)NULL;


	/* Where are these imports coming from? */
	forg = forgFrAbSyn(origin);


	/* If they don't specify include files then do nothing */
	if (!forg->file) return (Foam)NULL;


	/* Global declaration */
	decl = foamNewGDecl(FOAM_Word, strCopy(forg->file), emptyFormatSlot,
			    FOAM_GDecl_Import, FOAM_Proto_Include);
	foamGDeclSetRType(decl, FOAM_Nil);

	gen0AddGlobal(decl);
	return (Foam)NULL;
}

local Foam
genNever(AbSyn absyn)
{	
	assert(abUse(absyn) != AB_Use_Elided);

	gen0AddStmt(foamNew(FOAM_BCall, 2,
			    FOAM_BVal_Halt,
			    foamNewSInt(FOAM_Halt_NeverReached)),
		    absyn);
	return (Foam)NULL;
}

local Foam
genAssert(AbSyn absyn)
{
	AbSyn test = absyn->abAssert.test;
	Foam  call;
	String file, text;
	int l1, line;

	if (optIsIgnoreAssertsWanted())
		return NULL;

	/* !!! We should try and record where this assertion is */
	l1 = gen0State->labelNo++;
	gen0AddStmt(foamNewIf(genFoamBit(absyn->abAssert.test), l1), absyn);

	file = fnameName(sposFile(abPos(test)));
	line = sposLine(abPos(test));
	text = abPretty(test);
	call = gen0BuiltinCCall(FOAM_NOp, "rtAssertMessage", "runtime", 
				3, gen0CharArray(file), 
				foamNewSInt(line), 
				gen0CharArray(text));
	gen0AddStmt(call, absyn);
	gen0AddStmt(foamNew(FOAM_BCall, 2,
			    FOAM_BVal_Halt,
			    foamNewSInt(FOAM_Halt_AssertFailed)),
		    absyn);
	gen0AddStmt(foamNewLabel(l1), absyn);

	return NULL;
}

/*
 * Generate code for multiple values.
 */
local Foam
genMulti(AbSyn absyn)
{
	Foam 	values, var, val;
	int	i;

	if (gen0ValueMode) {
		values = foamNewEmpty(FOAM_Values, abArgc(absyn));
		for (i = 0; i < abArgc(absyn); i++) {
			val = genFoamVal(abArgv(absyn)[i]);
#ifdef MultiUseLocals
			/*!! We would rather generate locals here, but
			 *!! gen0TempLocal isn't working as expected. */
			var = gen0TempLocal(gen0FoamType(val));
#else
			var = gen0TempLex(gen0FoamType(val));
#endif

			values->foamValues.argv[i] = var;
			gen0AddStmt(foamNewSet(foamCopy(var), val), NULL);   
		}
		return values;
	}	
	else {
		for (i = 0; i < abArgc(absyn); i++) 
			genFoamStmt(absyn->abComma.argv[i]);
		values = NULL;
	}
	return values;
}

/*
 * Create a tuple.
 */
Foam
gen0MakeTuple(Length argc, AbSyn *argv, AbSyn absyn)
{
	Length	i;
	AbSyn	abi;
	Foam	vars[2], tupl, elts, elt;

	gen0MakeEmptyTuple(foamNewSInt(argc), vars, absyn);
	tupl = vars[0];
	elts = vars[1];

	for (i = 0; i < argc; i += 1) {
		abi = gen0AbTypeArg(argv[i]);
		elt = genFoamCast(genFoamVal(abi), abi, FOAM_Word);
		gen0AddStmt(gen0ASet(elts, (AInt) i, FOAM_Word, elt), absyn);
	}

	/*!! This should be a callout to tuple$Tuple(S) */
	return tupl;
}

local AbSyn
gen0AbTypeArg(AbSyn ab)
{
	Bool	changed = true;

	while (changed)
		switch (abTag(ab)) {
		case AB_Declare:
			ab = ab->abDeclare.type;
			break;
		case AB_Define:
			ab = ab->abDefine.lhs;
			break;
		default:
			changed = false;
			break;
		}

	return ab;
}


/*
 * Create a tuple.
 */
Foam
gen0MakeCross(Length argc, AbSyn *argv, AbSyn op)
{
	TForm   tf = tfMapArgN(gen0AbContextType(op), int0);
	AbSyn	elt;
	AInt    cfmt, ftype;
	Foam	t;
	Length	i;


	tf    = tfDefineeType(tf);
	assert(tfIsCross(tf));

	ftype = gen0Type(tf, &cfmt);
	cfmt  = gen0CrossFormatNumber(tf);
	t     = gen0TempLocal0(FOAM_Rec, cfmt);

	gen0SetTemp(t, foamNewRNew(cfmt));

	for (i = 0; i < argc; i += 1) {
		elt = gen0AbTypeArg(argv[i]);
		gen0AddStmt(foamNewSet(foamNewRElt(cfmt, foamCopy(t), i),
				       genFoamVal(elt)), op);
	}

	/*!! This should be a callout to tuple$Tuple(S) */
	return foamNewCast(FOAM_Word, t);
}

/*
 * Generate code for labelled expressions.
 */
local Foam
genLabel(AbSyn absyn)
{
	Syme	syme = abSyme(absyn->abLabel.label);

	assert(syme);
	if (symeDVMark(syme) > 0)
		gen0AddStmt(foamNewLabel(gen0VarIndex(syme)), absyn);

	return genFoam(absyn->abLabel.expr);
}

/*
 * Generate code for a gotos.
 */
local Foam
genGoto(AbSyn absyn)
{
	Syme	syme = abSyme(absyn->abGoto.label);

	assert(syme);
	gen0AddStmt(foamNewGoto(gen0VarIndex(syme)), absyn);

	return 0;
}

/*
 * Generate code for where clauses.
 */
local Foam
genWhere(AbSyn absyn)
{
	Foam		foam, lexicals, push, envVar;
	FoamList	pushEnvPlace;
	FoamList 	lowerSavedLines;
	int		index, nindex;
	Bool		flag;

	index = gen0FormatNum--;

	flag = gen0AddImportPlace(&lowerSavedLines);

	/* Save a place for a PushEnv, if needed. */
	gen0AddStmt(foamNewNOp(), absyn);
	pushEnvPlace = gen0State->lines;
	envVar	     = gen0Temp(FOAM_Env);

	gen0State->envVarStack =
		listCons(Foam)(envVar, gen0State->envVarStack);
	gen0State->envFormatStack =
		listCons(AInt)(index, gen0State->envFormatStack);
	gen0State->envLexPools =
		listCons(VarPool)(gen0State->lexPool, 
				  gen0State->envLexPools);
	gen0State->lexPool   = vpNew(fboxNew(foamNewEmptyDDecl(FOAM_DDecl_LocalEnv)));

	gen0State->importPlacePrev = 
		listCons(AInt)((AInt) gen0State->importPlace, 
			       gen0State->importPlacePrev);

	gen0State->importPlace = NULL;

	gen0State->whereNest++;
	gen0Vars(abStab(absyn));

	genFoamStmt(absyn->abWhere.context);
	foam = genFoamVal(absyn->abWhere.expr);
	gen0State->whereNest--;

	/* Create the lexicals. */
	lexicals = fboxMake(gen0State->lexPool->fbox);
	/* always push an env, as we don't track usage of where envs */
	nindex	   = gen0RealFormatNum++;
	gen0FormatList = listCons(Foam)(lexicals, gen0FormatList);
	push	   = foamNewSet(foamCopy(envVar),
				foamNewPushEnv(nindex,
		       foamCopy(car(cdr(gen0State->envVarStack)))));
	foamFree(car(pushEnvPlace));
	car(pushEnvPlace) = push;
	foamProgUnsetLeaf(gen0State->program);

	/* Restore environment state. */
	gen0State->lexPool = car(gen0State->envLexPools);
	gen0State->importPlace     = (FoamList*) 
				     car(gen0State->importPlacePrev);
	gen0State->importPlacePrev = cdr(gen0State->importPlacePrev);
	
	if (flag) gen0ResetImportPlace(lowerSavedLines);

	gen0AddFormat(index, nindex);

	gen0State->envVarStack	  = cdr(gen0State->envVarStack);
	gen0State->envFormatStack = cdr(gen0State->envFormatStack);
	gen0State->envLexPools = cdr(gen0State->envLexPools);

	return foam;
}

local Foam
genPretend(AbSyn absyn)
{
	AbSyn	expr = absyn->abPretendTo.expr;
	FoamTag	type = gen0Type(gen0AbType(absyn), NULL);

	return genFoamCast(genFoamVal(expr), expr, type);
}

local Foam
genRestrict(AbSyn absyn)
{
	AbSyn	expr = absyn->abRestrictTo.expr;

	if (abTag(expr) == AB_Add) {
#if EDIT_1_0_n1_06
		return gen0AddBody0(expr, abStab(expr),
				    absyn->abRestrictTo.type);
#else
		return gen0AddBody0(expr->abAdd.base, expr->abAdd.capsule, 
				    abStab(expr), absyn->abRestrictTo.type);
#endif
	}
	else
		return genFoamVal(expr);
}

local Foam
genExcept(AbSyn absyn)
{
	/* 
	 * This is maybe a bit too simple, esp. if hashcodes
	 * change under excepts.
	 */
	return genFoamVal(absyn->abExcept.type);
}


Foam
genImplicit(AbSyn absyn, AbSyn val, FoamTag type)
{
	Syme	syme = abImplicitSyme(absyn);
	AbSyn  *argv = NULL;
	Foam 	foam;      
	if (!syme) return genFoamVal(val);

	argv = gen0MakeImplicitArgs(1, val, abThisArgf);
	foam = gen0ApplyImplicitSyme(type, syme, 1, argv, NULL);
	return gen0ApplyReturn(absyn, syme, gen0AbType(absyn), foam);
}

local Foam
genApply(AbSyn absyn)
{
	AbSyn	op = abApplyOp(absyn), *argv = abApplyArgv(absyn);
	TForm	tf = gen0AbType(absyn);
	Foam	foam, *vals, *argloc;
	FoamTag type;
	Length	i, valc;
	Bool	packed = tfIsPackedMap(gen0AbType(op));

	if (abTag(op) == AB_Qualify && abTag(op->abQualify.what) == AB_Id)
		op = op->abQualify.what;

/*	printf("BDS: entered genApply\n"); */
	if (packed && !tfIsMulti(tf)) tf = tfRawType(tf);
/*	printf("BDS: done in genApply\n"); */

	type = gen0Type(tf, NULL);

	if (gen0IsSpecialType(absyn))
		return gen0ApplySpecialType(absyn);
		
	if (abImplicitSyme(absyn)) {
		Syme	syme = abImplicitSyme(absyn);
		Length	argc = abApplyArgc(absyn) + 1;
		argv = gen0MakeImplicitArgs(argc, absyn, abArgf);
		foam = gen0ApplyImplicitSyme(type, syme, argc, argv, NULL);
		return gen0ApplyReturn(absyn, syme, tf, foam);
	}
	else {
		/* Generate code for the arguments. */
		vals = gen0MakeApplyArgs(absyn, &valc);

		/* Fill in the operator and leave room for the arguments. */
		if (abTag(op) == AB_Id) {
			Syme	syme = abSyme(op);

			if (symeIsSpecial(syme)) {
				foam = gen0SpecialOp(type, syme, valc, argv, vals);
				valc   = 0;	/* args have been filled in */
				argloc = NULL;
			}
			else {
				foam = gen0ApplySyme(type, syme, abSymeImpl(op), 
						     valc, &argloc);
			}
		}
		else {	
			Foam	opFoam = genFoamVal(op);
			if (!gen0IsMultiEvaluable(opFoam)) {
				Foam	loc = gen0TempLocal(FOAM_Clos);
				gen0AddStmt(foamNewSet(foamCopy(loc), opFoam), op);
				opFoam = loc;
			}
			foam = gen0CCallFrFoam(type, opFoam, valc, &argloc);
		}

		/* Fill in the arguments. */
		for (i = 0; i < valc; i += 1) argloc[i] = vals[i];
		stoFree(vals);

		foam = gen0ApplyReturn(absyn, abSyme(op), tf, foam);
		return foam;
	}
}

/*
 * Args are:
 * 	ab: For source code posn
 *	syme: possibly null symbol meaning
 *      tf:   return type
 *	foam: the generated call
 */
Foam
gen0ApplyReturn(AbSyn ab, Syme syme, TForm tf, Foam foam)
{
	/* Explode multiple return values. */
	if (tfIsMulti(tf) && !tfIsNone(tf) && foamTag(foam) != FOAM_Values) {
		Foam stmt;

		foam = gen1ApplyReturn(ab, syme, tf, foam, &stmt);
		gen0AddStmt(stmt, ab);
	}
	/* Fixup fortran calls. */
	/* We should do the argument mangling when generating the args,
	 * not at this late stage
	 */
	if (syme && symeIsForeign(syme) && 
	    symeForeign(syme)->protocol == FOAM_Proto_Fortran) {
		/*
		 * Handle a call to a Fortran routine which has modifiable
		 * dummy arguments or which has a complex result/fn parameter
		 * and the value of the expression is required
		 */
		foam = gen0ModifyFortranCall(syme, foam, gen0FortranFnResult,
			gen0ValueMode);
	}

	return(foam);
}

Foam
gen1ApplyReturn(AbSyn ab, Syme syme, TForm tf, Foam foam, Foam *f)
{
	/*
	 * Use this routine if you don't want to add statements
	 * to the current closure. Make sure that you pass a
	 * valid pointer for `f' otherwise you're in trouble.
	 * Note that we don't do any argument mangling so you
	 * can't use this if your function has been imported
	 * from a language like Fortran.
	 */
	int	i;
	Foam	locs = foamNewEmpty(FOAM_Values, tfMultiArgc(tf));
	Foam	*argv = locs->foamValues.argv;

	/* Explode multiple return values. */
	for (i = 0; i < foamArgc(locs); i += 1) {
		TForm	tfi = tfMultiArgN(tf, i);
		FoamTag tag;
		AInt    fmt;
		tag = gen0Type(tfi, &fmt);
		argv[i] = gen0TempLocal0(tag, fmt);
	}
	foam = foamNewMFmt(gen0MultiFormatNumber(tf), foam);
	*f   = foamNewSet(foamCopy(locs), foam);
	return locs;
}


Bool
gen0IsFortranCall(AbSyn ab)
{
	if (abIsApply(ab) && abTag(abApplyOp(ab)) == AB_Id && 
	    symeIsForeign(abSyme(abApplyOp(ab))) && 
	    symeForeign(abSyme(abApplyOp(ab)))->protocol == FOAM_Proto_Fortran)
		return true;
	else
		return false;
}


local Foam *
gen0MakeApplyArgs(AbSyn absyn, Length *valc)
{
	AbSyn		op, *argv, abi;
	Length		i, argc;
	TForm		opTf, tfret;
	Foam		*vals;
	Bool		ftnfixedret = false;
	int		extraArg = 0;
	FortranType	ftnType = 0;

	op      = abApplyOp(absyn);
	opTf    = gen0AbContextType(op);

	/*
	 * We need to communicate the length of string values returned
	 * by Fortran functions which have return type FixedString.
	 * Once we have created the foam it may be too late so we create
	 * an extra first argument to the function call here into which
	 * we pass the foam representing the string length. Later when
	 * the gen0ModifyFortranCall() code is moved further upstream
	 * this problem will go away.
	 */
	if (gen0IsFortranCall(absyn))
	{
		/* Extract the return type */
		tfret   = tfMapRet(opTf);
		ftnType = ftnTypeFrDomTForm(tfret);


		/* Treat Char and Character in the same way */
		if (!ftnType && (gen0Type(tfret, NULL) == FOAM_Char))
			ftnType = FTN_Character;


		/* Do we need an extra argument for string length? */
		switch (ftnType)
		{
		   case FTN_XLString:
			/*
			 * Warn the user about returning String
			 * values from a Fortran function. This
			 * ought to be done much earlier when the
			 * original import statement is parsed.
			 */
			comsgWarning(absyn, ALDOR_W_FtnVarStringRet);
			/* Fall through */
		   case FTN_Character:
			/* Fall through */
		   case FTN_String:
			extraArg = 1;
			break;
		   default:
			break;
		}


		/* Is the return value a fixed-length string? */
		ftnfixedret = (ftnType == FTN_String);
	}

	argc = abApplyArgc(absyn);
	argv = abApplyArgv(absyn);

	*valc = tfMapArgc(opTf) + extraArg;
	vals = (Foam *) stoAlloc(OB_Other, (*valc) * sizeof(Foam));

	if (abEmbedApply(op)) {
		assert(*valc == 1);
		assert(!ftnfixedret);
		vals[0] = gen0EmbedApply(argc, argv, op, abEmbedApply(op));
	}
	else if (argc == 1 && tfIsMulti(gen0AbContextType(argv[0]))) {
		assert(!extraArg);
		if (*valc == 0)
			genFoamStmt(argv[0]);
		else {
			Foam	val = genFoamVal(argv[0]);
			assert(foamTag(val) == FOAM_Values);
			assert(foamArgc(val) == *valc);
			for (i = 0; i < *valc; i += 1)
				vals[i] = val->foamValues.argv[i];
		}
	}
	else if (ftnfixedret)
	{
		/*
		 * We have a call to a function imported from
		 * Fortran which returns a FixedString value.
		 */
		AbSyn	ab;
		TForm	tfret = tfMapRet(opTf);

		/* <paranoia> */
		assert(tfIsAbSyn(tfret)); /* TF_Syntax || TF_General */
		ab = tfGetExpr(tfret);
		assert(abTag(ab) == AB_Apply);
		assert(abArgc(ab) == 2);
		ab = abArgv(ab)[1];
		assert(ab);
		/* </paranoia> */

		vals[0] = genFoamVal(ab);
		for (i = 1; i < *valc; i += 1) {
			abi = tfMapSelectArg(opTf, absyn, i-1);
			assert(abi);
			vals[i] = genFoamVal(abi);
		}
	}
	else if (extraArg)
	{
		/*
		 * We have a call to a function imported from
		 * Fortran which returns a Character or String
		 * result. For Character result then the Fortran
		 * function really returns a String of length 1
		 * otherwise it returns a String with length
		 * that we cannot compute. Use length 0.
		 */
		int len = (ftnType == FTN_Character) ? 1 : 0;
		vals[0] = foamNewSInt(len);
		for (i = 1; i < *valc; i += 1) {
			abi = tfMapSelectArg(opTf, absyn, i-1);
			assert(abi);
			vals[i] = genFoamVal(abi);
		}
	}
	else {
		for (i = 0; i < *valc; i += 1) {
			abi = tfMapSelectArg(opTf, absyn, i);
			assert(abi);
			vals[i] = genFoamVal(abi);
		}
	}

	return vals;
}

local Foam
gen0EmbedApply(int argc, AbSyn *argv, AbSyn op, AbEmbed embed)
{
	Foam val;
	switch (embed) {
	  case AB_Embed_ApplyMultiToTuple:
		val = gen0MakeTuple(argc, argv, op);
		break;
	  case AB_Embed_ApplyMultiToCross:
		val = gen0MakeCross(argc, argv, op);
		break;
	  default:
		bugBadCase(embed);
		val = NULL;
		break;
	}
	return val;
}

/*
 * Build a call to a function given by its symbol meaning.
 * The args are filled in later.
 */
local Foam
gen0ApplySyme(FoamTag type, Syme syme, SImpl impl, 
	      Length argc, Foam **pargv)
{
	Foam	foam;
	Foam    *args;
	AInt    otype = type;

	if (symeIsBuiltin(syme))
		foam = gen0ApplyBuiltin(syme, argc, &args);

	else if (symeIsForeign(syme))
		foam = gen0ApplyForeign(type, syme, argc, &args);
	
	else if (symeIsImport(syme)) {
		otype = type;
		type = gen0Type(tfMapRet(symeType(symeOriginal(syme))), NULL);
		foam = gen0CCall(type, syme, argc, &args);
	}
	else if (gen0IsOpenCallable(syme, impl) &&
		 listIsSingleton(gen0State->envFormatStack))
		foam = gen0OCall(type, syme, argc, &args);
	else
		/* BDS -- syme->id->str gives the name of the function being 
                          called. */
		foam = gen0CCall(type, syme, argc, &args);

	if (type != otype)
		foam = foamNewCast(type, foam);

	*pargv = args;
	return foam;
}

local Bool
gen0IsOpenCallable(Syme syme, SImpl impl)
{

/*
 * #define isOpenCallable(syme)	\
 * 	(fintMode != FINT_LOOP && !gen0IsCatInner() && symeClosure(syme) && \
 *	 (symeIsLexConst(syme) || symeIsExtend(syme) || symeIsExport(syme)))
 */
	if (fintMode == FINT_LOOP)
		return false;
	if (gen0IsCatInner())
		return false;
	if (!symeClosure(syme))
		return false;

	if (!impl || !implIsLocal(impl))
		return false;

	if (symeIsLexConst(syme))
		return true;
	if (symeIsExtend(syme))
		return true;
	if (symeIsExport(syme))
		return true;
	
	return false;
}


/*
 * Build a call to a builtin function.  The args are filled in later.
 */
local Foam
gen0ApplyBuiltin(Syme syme, Length argc, Foam **pargv)
{
	Foam	foam;

	foam = foamNewEmpty(FOAM_BCall, OpSlot + argc);
	foam->foamBCall.op = symeBuiltin(syme);

	*pargv = foam->foamBCall.argv;

	return foam;
}

/*
 * Build a call to a foreign function.  The args are filled in later.
 */
local Foam
gen0ApplyForeign(FoamTag type, Syme syme, Length argc, Foam **pargv)
{
	Foam	foam;

	/* printf("BDS: Inside gen0ApplyForeign\n"); */

	foam = foamNewEmpty(FOAM_PCall, TypeSlot + OpSlot + ProtoSlot + argc);
	foam->foamPCall.type = type;
	foam->foamPCall.op = gen0ForeignValue(syme);
	foam->foamPCall.protocol = symeForeign(syme)->protocol;

	*pargv = foam->foamPCall.argv;

	return foam;
}

/*
 * Build an open call.  The args are filled in later.
 */
local Foam
gen0OCall(FoamTag type, Syme syme, Length argc, Foam **pargv)
{
	Foam clos     = foamCopy(symeClosure(syme)), foam;
	int opLevel   = symeDefLevelNo(syme);
	int closLevel = clos->foamClos.env->foamEnv.level;
	int envLevel  = gen0FoamLevel(opLevel + closLevel);

	foam = foamNewEmpty(FOAM_OCall, TypeSlot + OpSlot + EnvSlot + argc);
	foam->foamOCall.type = type;
	foam->foamOCall.op = clos->foamClos.prog;
	foam->foamOCall.env = foamNewEnv(envLevel);

	if (envLevel == 0) foamProgUnsetLeaf(gen0State->program);
	if (listElt(AInt)(gen0State->formatUsage, envLevel) == emptyFormatSlot)
		gen0UseFormat(envLevel, envUsedSlot);

	*pargv = foam->foamOCall.argv;
	return foam;
}

/*
 * Build a closed call.  The args are filled in later.
 */
local Foam
gen0CCall(FoamTag type, Syme syme, Length argc, Foam **pargv)
{
	Foam   *argloc;
	Foam	foam;

	foam = gen0CCallFrFoam(type, gen0ExtendSyme(syme), argc, &argloc);
	if (tfIsDomainMap(symeType(syme)))
		foamPure(foam) = true;

	*pargv = argloc;

/*
	printf("BDS: inside gen0CCall\n");
	foamPrint(stdout,foam);
*/

	return foam;
}

/*
 * This is an export so we can get to it from export.c. It is
 * after all quite a useful function.
 */
Foam
gen0CCallFrFoam(FoamTag type, Foam op, Length argc, Foam **pargv)
{
	Foam	foam;

	foam = foamNewEmpty(FOAM_CCall, TypeSlot + OpSlot + argc);
	foam->foamCCall.type = type;
	foam->foamCCall.op = op;

	*pargv = foam->foamCCall.argv;
	return foam;
}

local Foam
gen0ApplyImplicitSyme(FoamTag type, Syme syme, Length argc, 
		      AbSyn *argv, Foam vals)
{
	AbSyn   abtmp;
	TForm	tf;
	Foam	foam, *argloc;
	Length	i, tfargc;
	
	if (symeIsSpecial(syme)) {
		foam = gen0SpecialOp(type, syme, argc, argv, NULL);
		stoFree(argv);
		return foam;
	}
	/* Gross! */
	tf = symeType(syme);
	assert(tfIsAnyMap(tf));
	
	tfargc = tfMapArgc(tf);
	foam = gen0ApplySyme(type, syme, NULL, tfargc, &argloc);

	abtmp = abNewEmpty(AB_Apply, argc + 1);
	abtmp->abApply.op = NULL;
	
	for (i = 0; i < argc; i += 1) {
		abtmp->abApply.argv[i] = argv[i];
	}
	for (i = 0; i < tfargc; i += 1) {
		AbSyn argi = tfMapSelectArg(tf, abtmp, i);
		argloc[i] = genFoamVal(argi);
	}
	/* Should check vals here for additional arguments ... */
	abFreeNode(abtmp);
	stoFree(argv);
	return foam;
}

local AbSyn *
gen0MakeImplicitArgs(Length argc, AbSyn ab, AbSynGetter argf)
{
	AbSyn *argv = (AbSyn*) stoAlloc(OB_Other, argc*sizeof(AbSyn));
	int i;

	for (i=0; i<argc; i++)
		argv[i] = (*argf)(ab, i);

	return argv;
}

/*****************************************************************************
 *
 * :: Generate code for application of special operations.
 *
 ****************************************************************************/

local TForm	gen0SpecialKeyType	(TForm);
local TForm	gen0SpecialArgType	(Syme, Length);
local TForm	gen0SpecialRetType	(Syme);
local AInt	gen0RawRecordIndex	(TForm, TForm);
local AInt	gen0RecordIndex		(TForm, TForm);
local AInt	gen0UnionIndex		(TForm, TForm);
local AInt	gen0TrailingIndex	(TForm, TForm);
local AInt	gen0UnionCaseIndex	(TForm, AbSyn);
local AInt	gen0EnumIndex		(TForm, Symbol);
local Bool	gen0IsEnumLit		(Syme);

local Foam	gen0SpecialUnhandled	(Syme);

local Foam	gen0ArrayNew		(Syme,        Length, AbSyn *, Foam *);
local Foam	gen0ArrayElt		(FoamTag,     Length, AbSyn *, Foam *);
local Foam	gen0ArraySet		(FoamTag,     Length, AbSyn *, Foam *);
local Foam	gen0ArrayDispose	(Length, AbSyn *, Foam *);

local Foam	gen0RawRecordNew	(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RawRecordExplode	(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RawRecordElt	(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0RawRecordSet	(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen1RawRecordSet	(TForm, Foam, Foam, Foam, AInt);
local Foam	gen0RawRecordDispose	(Length, AbSyn *, Foam *);
local Foam	gen1RawRecordFormat	(TForm);

local Foam	gen0RecordNew		(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RecordExplode	(TForm,       Length, AbSyn *, Foam *);
local Foam	gen0RecordElt		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0RecordSet		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0RecordDispose	(Length, AbSyn *, Foam *);

local Foam	gen0BIntDispose		(Length, AbSyn *, Foam *);

local Foam	gen0UnionNew		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0UnionCase		(TForm,       Length, AbSyn *, Foam *);
local Foam      gen0UnionCaseBool       (TForm, Length, AbSyn *, Foam *);
local Foam	gen0UnionElt		(FoamTag,TForm,Length,AbSyn *, Foam *);
local Foam	gen0UnionSet		(FoamTag,TForm,Length,AbSyn *, Foam *);
local Foam	gen0UnionDispose	(Length, AbSyn *, Foam *);

local Foam	gen0EnumEqual		(AbSyn *, Foam *);
local Foam	gen0EnumNotEqual	(AbSyn *, Foam *);

local Foam	gen0TrailingNew		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0TrailingDispose	(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0TrailingElt		(Syme, TForm, Length, AbSyn *, Foam *);
local Foam	gen0TrailingSet	(Syme, TForm, Length, AbSyn *, Foam *);

local Foam
gen0SpecialOp(FoamTag type, Syme syme, Length argc, AbSyn *argv, Foam *vals)
{
	Symbol		sym = symeId(syme);
	Foam		foam = NULL;

	/* Arrays. */

	if (sym == ssymArrNew)
		foam = gen0ArrayNew(syme, argc, argv, vals);

	else if (sym == ssymArrElt)
		foam = gen0ArrayElt(type, argc, argv, vals);

	else if (sym == ssymArrSet)
		foam = gen0ArraySet(type, argc, argv, vals);

	else if (sym == ssymArrDispose)
		foam = gen0ArrayDispose(argc, argv, vals);

	/* Big Integers. */

	else if (sym == ssymBIntDispose)
		foam = gen0BIntDispose(argc, argv, vals);

	/* Raw Records. */
	else if (sym == ssymRawRecNew || sym == ssymTheRawRecord) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordNew(key, argc, argv, vals);
	}

	else if (sym == ssymRawRecElt) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordElt(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRawRecSet) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordSet(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRawRecDispose) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRawRecord(key))
			foam = gen0RawRecordDispose(argc, argv, vals);
	}

	/* Records. */
	else if (sym == ssymRecNew || sym == ssymTheRecord) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsRecord(key))
			foam = gen0RecordNew(key, argc, argv, vals);
	}

	else if (sym == ssymRecElt) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordElt(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRecSet) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordSet(syme, key, argc, argv, vals);
	}

	else if (sym == ssymRecDispose) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordDispose(argc, argv, vals);
	}

	/* Unions. */

	else if (sym == ssymTheUnion) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsUnion(key))
			foam = gen0UnionNew(syme, key, argc, argv, vals);
	}

	else if (sym == ssymTheCase) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsUnion(key))
			foam = gen0UnionCase(key, argc, argv, vals);
	}

	/* TrailingArrays. */
	else if (sym == ssymTheTrailingArray) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsTrailingArray(key))
			foam = gen0TrailingNew(syme, key, argc, argv, vals);
	}
	/* Enumerations. */

	else if (sym == ssymEquals) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsEnum(key))
			foam = gen0EnumEqual(argv, vals);
	}

	else if (sym == ssymNotEquals) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsEnum(key))
			foam = gen0EnumNotEqual(argv, vals);
	}

	/* Generators. */

	else if (sym == ssymTheGenerator)
		foam = genFoamArg(argv, vals, int0);

	/* bracket, explode, apply, set!, dispose!. */

	else if (sym == ssymBracket) {
		TForm	key = gen0SpecialRetType(syme);
		if (tfIsRecord(key))
			foam = gen0RecordNew(key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordNew(key, argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionNew(syme, key, argc, argv, vals);
		if (tfIsTrailingArray(key))
			foam = gen0TrailingNew(syme, key, argc, argv, vals);
	}

	else if (sym == ssymTheExplode) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordExplode(key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordExplode(key, argc, argv, vals);
	}

	else if (sym == ssymApply) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordElt(syme, key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordElt(syme, key, argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionElt(type, key, argc, argv, vals);
		else if (tfIsTrailingArray(key))
			foam = gen0TrailingElt(syme, key, argc, argv, vals);
	}

	else if (sym == ssymSetBang) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordSet(syme, key, argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordSet(syme, key, argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionSet(type, key, argc, argv, vals);
		else if (tfIsTrailingArray(key))
			foam = gen0TrailingSet(syme, key, argc, argv, vals);
	}

	else if (sym == ssymTheDispose) {
		TForm	key = gen0SpecialArgType(syme, argc);
		if (tfIsRecord(key))
			foam = gen0RecordDispose(argc, argv, vals);
		else if (tfIsRawRecord(key))
			foam = gen0RawRecordDispose(argc, argv, vals);
		else if (tfIsUnion(key))
			foam = gen0UnionDispose(argc, argv, vals);
		else if (tfIsTrailingArray(key))
			foam = gen0TrailingDispose(syme, key, argc, argv, vals);
	}

	if (foam == NULL)
		foam = gen0SpecialUnhandled(syme);

	return gen0SetValue(foam, argv[0]);
}


/*****************************************************************************
 *
 * :: Specific generators for special operations.
 *
 ****************************************************************************/

/* Helper functions. */

local Foam
genFoamArg(AbSyn *argv, Foam *vals, int index)
{
	if (vals && vals[index])
		return vals[index];

	return genFoamVal(argv[index]);
}

local TForm
gen0SpecialKeyType(TForm tf)
{
	SymeList	xsymes;
	TFormTag	tag = TF_Unknown;
	TFormList	args = listNil(TForm);
	TForm		ntf;
	Length		i, argc;

	tfFollow(tf);
	if (tfIsRecord(tf) || tfIsRawRecord(tf) || tfIsUnion(tf) ||
		tfIsEnum(tf) || tfIsTrailingArray(tf))
		return tf;

	for (xsymes = tfGetDomExports(tf); xsymes; xsymes = cdr(xsymes)) {
		Syme		xsyme = car(xsymes);
		Symbol		sym = symeId(xsyme);
		TFormTag	ntag;
		TForm		arg = NULL;

		if (!symeIsSpecial(xsyme)) continue;

		if (sym == ssymTheRecord)
			ntag = TF_Record;
		else if (sym == ssymTheRawRecord)
			ntag = TF_RawRecord;
		else if (sym == ssymTheUnion)
			ntag = TF_Union;
		else if (sym == ssymTheTrailingArray)
			ntag = TF_TrailingArray;
		else if (tfIsSelf(symeType(xsyme)))
			ntag = TF_Enumerate;
		else
			ntag = TF_Unknown;
			
		if (ntag == TF_Unknown)
			;
		else if (tag == TF_Unknown)
			tag = ntag;
		else if (tag != ntag)
			/* Seen two of record/union/enum!? */
			return tfUnknown;

		/* We assume the special symes stay in order. */
		if (tag == TF_Record && sym == ssymApply)
			arg = tfMapRet(symeType(xsyme));
		if (tag == TF_RawRecord && sym == ssymApply)
			arg = tfMapRet(symeType(xsyme));
		if (tag == TF_Union && sym == ssymTheUnion)
			arg = tfMapArgN(symeType(xsyme), int0);
		if (tag == TF_Enumerate && tfIsSelf(symeType(xsyme)))
			arg = tfDeclare(abFrSyme(xsyme), tfType);

		if (arg) args = listCons(TForm)(arg, args);
	}

	args = listNReverse(TForm)(args);
	argc = listLength(TForm)(args);

	ntf = tfNewEmpty(tag, argc);
	for (i = 0; i < argc; i += 1, args = cdr(args))
		tfArgv(ntf)[i] = car(args);

	return ntf;
}

local TForm
gen0SpecialArgType(Syme syme, Length argc)
{
	TForm	tf = tfDefineeType(symeType(syme));
	TForm	key = tfMapMultiArgN(tf, argc, int0);
	return gen0SpecialKeyType(key);
}

local TForm
gen0SpecialRetType(Syme syme)
{
	TForm	tf = tfDefineeType(symeType(syme));
	TForm	key = tfMapRet(tf);
	return gen0SpecialKeyType(key);
}

local AInt
gen0RawRecordIndex(TForm whole, TForm part)
{
	Symbol	sym = tfEnumId(part, int0);
	AInt	i;

	for (i = 0; i < tfRawRecordArgc(whole); i += 1)
		if (sym == tfDefineeSymbol(tfRawRecordArgN(whole, i)))
			return i;

	bug("gen0RawRecordIndex: accessor not in record");
	return -1;
}

local AInt
gen0RecordIndex(TForm whole, TForm part)
{
	Symbol	sym = tfEnumId(part, int0);
	AInt	i;

	for (i = 0; i < tfRecordArgc(whole); i += 1)
		if (sym == tfDefineeSymbol(tfRecordArgN(whole, i)))
			return i;

	bug("gen0RecordIndex: accessor not in record");
	return -1;
}

local AInt
gen0UnionIndex(TForm whole, TForm part)
{
	AInt	i;
	Symbol  sym = tfDefineeSymbol(part);
	for (i = 0; i < tfUnionArgc(whole); i += 1)
		if ((!sym || tfCompoundId(whole,i) == sym)
		    && tfEqual(part, tfUnionArgN(whole, i)))
			return i;

	bug("gen0UnionIndex: branch not in union");
	return -1;
}

local AInt
gen0UnionCaseIndex(TForm whole, AbSyn part)
{
	Symbol	sym = part->abId.sym;
	AInt	i;

	for (i = 0; i < tfUnionArgc(whole); i += 1)
		if (sym == tfDefineeSymbol(tfUnionArgN(whole, i)))
			return i;

	bug("gen0UnionCaseIndex: branch not in union");
	return -1;
}

local AInt
gen0EnumIndex(TForm whole, Symbol sym)
{
	AInt	i;

	for (i = 0; i < tfEnumArgc(whole); i += 1)
		if (sym == tfEnumId(whole, i))
			return i;

	bug("gen0EnumIndex: case not in enumeration");
	return -1;
}

local AInt
gen0TrailingIndex(TForm whole, TForm tf)
{
	AInt	i;
	int 	argc = tfAsMultiArgc(whole);
	Symbol  sym = tfEnumId(tf, int0);
	for (i = 0; i < argc; i += 1)
		if (sym == tfDefineeSymbol(tfAsMultiArgN(whole, argc, i)))
			return i;

	bug("gen0TrailingIndex: label not in record");
	return -1;
}

local Bool
gen0IsEnumLit(Syme syme)
{
	/* SpecialKeyType may be costly, so avoid if poss */
	return	symeIsImport(syme) && 
		symeIsSpecial(syme) &&
		!tfIsMap(symeType(syme)) &&
		tfIsEnum(gen0SpecialKeyType(symeType(syme)));
}

local Foam
gen0SpecialUnhandled(Syme syme)
{
	fprintf(stderr, 
	       "Implementation restriction: unhandled special symbol meaning");
	fnewline(stderr);
	fprintf(stderr, "  %s : ", symeString(syme));
	tfPrettyPrint(stderr, symeType(syme));
	fnewline(stderr);

	bugUnimpl(symeString(syme));
	return NULL;
}

/* Arrays. */

local Foam 
gen0ArrayNew(Syme syme, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf = tfDefineeType(symeType(syme));
	TForm	tfi = tfMapMultiArgN(tf, argc, int0);
	FoamTag	type = gen0Type(tfi, NULL);
	Foam	value = genFoamArg(argv, vals, 1);

	return foamNewANew(type, value);
}

local Foam
gen0ArrayElt(FoamTag type, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	whole = genFoamArg(argv, vals, int0);
	Foam	index = genFoamArg(argv, vals, 1);

	return foamNewAElt(type, index, whole);
}

local Foam
gen0ArraySet(FoamTag type, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	whole = genFoamArg(argv, vals, int0);
	Foam	index = genFoamArg(argv, vals, 1);
	Foam	value = genFoamArg(argv, vals, 2);

	return foamNewSet(foamNewAElt(type, index, whole), value);
}

local Foam
gen0ArrayDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}


/* Big Integers. */

local Foam
gen0BIntDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}


/* Raw Records. */

/*
 * PackedRepSize: () -> SInt;
 */
local Bool
tfIsPackedRepSize(TForm tf)
{
	/* Follow substitutions */
	tfFollow(tf);


	/*
	 * Type checking - must be nullary map returning a
	 * SInt$Machine. We probably ought to be using tfSat():
	 * since we don't we don't check the return type.
	 */
	if (!tfIsAnyMap(tf)) return false;
	if (tfMapArgc(tf) != 0) return false;


	/* Looks like the correct type */
	return true;
}


/*
 * PackedRecordGet: Ptr -> %
 */
local Bool
tfIsPackedRecordGet(TForm tf)
{
	/* Follow substitutions */
	tfFollow(tf);


	/*
	 * Type checking - must be unary map to self. We probably
	 * ought to be using tfSat() but since we aren't we ignore
	 * the argument type.
	 */
	if (!tfIsAnyMap(tf)) return false;
	if (tfMapArgc(tf) != 1) return false;
	if (!tfIsSelf(tfMapRet(tf))) return false;


	/* Looks like the correct type */
	return true;
}


/*
 * PackedRecordSet: (Ptr, %) -> %
 */
local Bool
tfIsPackedRecordSet(TForm tf)
{
	/* Follow substitutions */
	tfFollow(tf);


	/*
	 * Type checking - must be a binary map from a Ptr
	 * and self to self. We probably ought to be using
	 * tfSat() but since we aren't we ignore the type
	 * of the first argument.
	 */
	if (!tfIsAnyMap(tf)) return false;
	if (tfMapArgc(tf) != 2) return false;
	if (!tfIsSelf(tfMapArgN(tf, 1))) return false;
	if (!tfIsSelf(tfMapRet(tf))) return false;


	/* Looks like the correct type */
	return true;
}


local Foam
gen0RawRecordNew(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	i;
	Foam	format, whole, fmt, foam;


	/* Create the raw record format/index */
	format = gen1RawRecordFormat(key);


	/* Create a temporary for the index */
	fmt = gen0Temp(FOAM_Word);


	/* Store the index in a temporary */
	gen0AddStmt(foamNewSet(foamCopy(fmt), format), (AbSyn)NULL);


	/* Create a local for the record pointer */
	whole = gen0Temp(FOAM_RRec);


	/* Create the uninitialised raw record */
	foam = foamNewRRNew(foamCopy(fmt), argc);
	foam = foamNewSet(foamCopy(whole), foam);
	gen0AddStmt(foam, (AbSyn)NULL);


	/* Fill in each of the fields */
	for (i = 0; i < argc; i++)
	{
		Foam	rrec  = foamCopy(whole);
		Foam	rfmt  = foamCopy(fmt);
		Foam	value = genFoamArg(argv, vals, i);
		Foam	set   = gen1RawRecordSet(key, rrec, rfmt, value, i);
		gen0AddStmt(set, NULL);
	}

	return foamCopy(whole);
}


local Foam
gen1RawRecordFormat(TForm key)
{
	AInt		i, fmtc;
	FoamList	fmts;


	/* Create the dynamic format */
	fmtc = tfArgc(key);
	fmts = listNil(Foam);


	/* Build the format list in reverse order */
	for (i = (fmtc - 1); i >= 0 ; i--)
	{
		Syme	op;
		Foam	call, *ignored;
		TForm	tf = tfArgv(key)[i];


		/* Get the actual type */
		tf = tfDefineeType(tf);


		/* Look for PackedRepSize: () -> SInt */
		op = tfGetDomImport(tf, "PackedRepSize", tfIsPackedRepSize);


		/* Did we find it? */
		if (!op)
		{
			AbSyn	ab = abNewNothing(sposNone);
			String	msg = strPrintf("%s: %s is missing export %s",
					"gen1RawRecordFormat",
					abPretty(tfToAbSyn(tf)),
					"PackedRepSize");
			comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
			String	dom = abPretty(tfToAbSyn(tf));
			bug("gen1RawRecordFormat: %s is missing export %s",
				dom, "PackedRepSize");
			strFree(dom);
#endif
		}


		/* Apply this export to get the type */
		call = gen0ExtendSyme(op);
		call = gen0CCallFrFoam(FOAM_SInt, call, (Length)0, &ignored);


		/* Prepend onto the list of formats */
		listPush(Foam, call, fmts);
	}


	/* The dynamic format must be a multi */
	return foamNewRRFmt(foamNewOfList(FOAM_Values, fmts));
}


local Foam
gen0RawRecordElt(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt		field;
	TForm		tf, elt;
	Foam		rrec, call, format, *fp;
	Syme		op;
	SymeList	symes;


	/* Create the raw record format */
	format = gen1RawRecordFormat(key);


	/* Get the type of the map application */
	tf = tfDefineeType(symeType(syme));


	/* Extract the type of the argument */
	elt = tfMapMultiArgN(tf, argc, 1);


	/* Find the field for this type */
	field = gen0RawRecordIndex(key, elt);


	/* Get the raw record value being dereferenced */
	rrec  = genFoamArg(argv, vals, int0);


	/* Get the field type from the key */
	tf = tfDefineeType(tfArgv(key)[field]);


	/* Look for PackedRecordGet: Ptr -> % */
	op = tfGetDomImport(tf, "PackedRecordGet", tfIsPackedRecordGet);


	/* Did we find the export? */
	if (!op)
	{
		AbSyn	ab = abNewNothing(sposNone);
		String	msg = strPrintf("%s: %s is missing export %s",
				"gen0RawRecordElt",
				abPretty(tfToAbSyn(tf)),
				"PackedRecordGet");
		comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
		String	dom = abPretty(tfToAbSyn(tf));

		bug("gen0RawRecordElt: %s is missing export %s",
			dom, "PackedRecordGet");
		strFree(dom);
#endif
	}


	/* Convert the export into an import */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Apply this export to get the domain value */
	call = gen0ExtendSyme(op);
	call = gen0CCallFrFoam(FOAM_Word, call, 1, &fp);
	*fp  = foamNewRRElt(rrec, field, format);


	/* Return the call */
	return call;
}


local Foam
gen0RawRecordSet(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt		field;
	TForm		tf, elt;
	Foam		rrec, value, format;


	/* Create the raw record format */
	format = gen1RawRecordFormat(key);


	/* Get the type of the map application */
	tf = tfDefineeType(symeType(syme));


	/* Extract the type of the argument */
	elt = tfMapMultiArgN(tf, argc, 1);


	/* Find the field for this type */
	field = gen0RawRecordIndex(key, elt);


	/* Get the raw record value being dereferenced */
	rrec  = genFoamArg(argv, vals, int0);


	/* Get the value being stored */
	value = genFoamArg(argv, vals, 2);


	/* Generate the update */
	return gen1RawRecordSet(key, rrec, format, value, field);
}


local Foam
gen1RawRecordSet(TForm key, Foam rrec, Foam fmt, Foam value, AInt field)
{
	Foam		call, *fp;
	TForm		tf;
	Syme		op;
	SymeList	symes;


	/* Get the field type from the key */
	tf = tfDefineeType(tfArgv(key)[field]);


	/* Look for PackedRecordSet: (Ptr, %) -> % */
	op = tfGetDomImport(tf, "PackedRecordSet", tfIsPackedRecordSet);


	/* Did we find the export? */
	if (!op)
	{
		AbSyn	ab = abNewNothing(sposNone);
		String	msg = strPrintf("%s: %s is missing export %s",
				"gen1RawRecordSet",
				abPretty(tfToAbSyn(tf)),
				"PackedRecordSet");
		comsgFatal(ab, ALDOR_F_Bug, msg);
#if 0
		String	dom = abPretty(tfToAbSyn(tf));

		bug("gen1RawRecordSet: %s is missing export %s",
			dom, "PackedRecordSet");
		strFree(dom);
#endif
	}


	/* Convert the export into an import */
	symes = listSingleton(Syme)(op);
	symes = symeListSubstSelf(stabFile(), tf, symes);
	op    = car(symes);


	/* Apply this export to get the domain value */
	call = gen0ExtendSyme(op);
	call = gen0CCallFrFoam(FOAM_Word, call, 2, &fp);


	/* Fill in the argument slots */
	fp[0] = foamNewRRElt(rrec, field, fmt);
	fp[1] = value;


	/* Return the call to do the update */
	return call;
}


local Foam
gen0RawRecordExplode(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	tvals  = gen0TempFrDDecl(int0, true);

	/*
	 * Not implemented yet!
	 */
	(void)fprintf(dbOut, "*** gen0RawRecordExplode unimplemented\n");
	return tvals;
}


local Foam
gen0RawRecordDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}


/* Records. */

local Foam
gen0RecordNew(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0RecordFormatNumber(key);
	AInt	index;
	Foam	whole = gen0Temp0(FOAM_Rec, format);

	gen0AddStmt(gen0RNew(whole, format), NULL);
	for (index = 0; index < argc; index += 1) {
		Foam	value = genFoamArg(argv, vals, index);
		gen0AddStmt(gen0RSet(whole, format, index, value), NULL);
	}

	return foamCopy(whole);
}

local Foam
gen0RecordElt(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf  = tfDefineeType(symeType(syme));
	TForm	elt = tfMapMultiArgN(tf, argc, 1);

	AInt	format = gen0RecordFormatNumber(key);
	AInt	index  = gen0RecordIndex(key, elt);
	Foam	whole  = genFoamArg(argv, vals, int0);

	return foamNewRElt(format, whole, index);
}

local Foam
gen0RecordSet(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf  = tfDefineeType(symeType(syme));
	TForm	elt = tfMapMultiArgN(tf, argc, 1);

	AInt	format = gen0RecordFormatNumber(key);
	AInt	index  = gen0RecordIndex(key, elt);
	Foam	whole  = genFoamArg(argv, vals, int0);
	Foam	value  = genFoamArg(argv, vals, 2);

	return foamNewSet(foamNewRElt(format, whole, index), value);
}

local Foam
gen0RecordExplode(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0RecordFormatNumber(key);
	Foam	whole  = gen0TempLocal0(FOAM_Rec, format);
	Foam	tvals  = gen0TempFrDDecl(format, true);
	int	i;

	gen0AddStmt(foamNewSet(whole, genFoamArg(argv, vals, int0)), NULL);

	whole = gen0MakeMultiEvaluable(FOAM_Rec, format, whole);

	for (i = 0; i < foamArgc(tvals); i++) {
		Foam lhs = foamCopy(tvals->foamValues.argv[i]);
		Foam rhs = foamNewRElt(format, foamCopy(whole), (AInt) i); 
		gen0AddStmt(foamNewSet(lhs, rhs), NULL);
	}

	return tvals;
}


local Foam
gen0RecordDispose(Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}

/* Unions. */

local Foam
gen0UnionNew(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm	tf  = tfDefineeType(symeType(syme));
	TForm	elt = tfMapMultiArgN(tf, argc, int0);

	AInt	format = gen0MakeUnionFormat();
	Foam	index  = foamNewSInt(gen0UnionIndex(key, elt));
	Foam	value  = genFoamArg(argv, vals, int0);
	Foam	whole  = gen0Temp0(FOAM_Rec, format);

	if (gen0Type(elt, NULL) != FOAM_Word)
		value = foamNewCast(FOAM_Word, value);

	gen0AddStmt(gen0RNew(whole, format), NULL);
	gen0AddStmt(gen0RSet(whole, format, (AInt) 0, index), NULL);
	gen0AddStmt(gen0RSet(whole, format, (AInt) 1, value), NULL);

	return foamNewCast(FOAM_Word, foamCopy(whole));
}

local Foam
gen0UnionElt(FoamTag type, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	l = gen0State->labelNo++;
	AInt	format = gen0MakeUnionFormat();
	Foam	whole = genFoamArg(argv, vals, int0);
	Foam	stmt, foam;
	Foam 	myVals[2];
	
	whole = gen0MakeMultiEvaluable(FOAM_Rec, format, foamNewCast(FOAM_Rec, whole));
	myVals[0] = foamCopy(whole);
	myVals[1] = NULL;

	stmt = foamNewIf(gen0UnionCaseBool(key, argc, argv, myVals), l);
	gen0AddStmt(stmt, NULL);
	stmt = foamNew(FOAM_BCall, 2, FOAM_BVal_Halt,
		       foamNewSInt(FOAM_Halt_BadUnionCase));
	gen0AddStmt(stmt, NULL);
	gen0AddStmt(foamNewLabel(l), NULL);

	foam = foamNewRElt(format, foamCopy(whole), (AInt) 1);

	if (type != FOAM_Word)
		foam = foamNewCast(type, foam);

	return foam;
}

local Foam
gen0UnionCase(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	Foam foam = gen0UnionCaseBool(key, argc, argv, vals);
	return foamNewCast(FOAM_Word, foam);
}

local Foam
gen0UnionCaseBool(TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0MakeUnionFormat();
	AInt	index  = gen0UnionCaseIndex(key, argv[1]);
	Foam	whole  = genFoamArg(argv, vals, int0);
	Foam	foam   = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntEQ;
	foam->foamBCall.argv[0] = foamNewRElt(format, 
					      foamNewCast(FOAM_Rec, whole), (AInt) 0);
	foam->foamBCall.argv[1] = foamNewSInt(index);

	return foam;
}

local Foam
gen0UnionSet(FoamTag type, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	AInt	format = gen0MakeUnionFormat();
	Foam	whole  = genFoamArg(argv, vals, int0);
	Foam	index  = foamNewSInt(gen0UnionCaseIndex(key, argv[1]));
	Foam	value  = genFoamArg(argv, vals, 2);

	whole = gen0MakeMultiEvaluable(FOAM_Rec, format, foamNewCast(FOAM_Rec, whole));

	gen0AddStmt(gen0RSet(whole, format, (AInt) 0, index), NULL);
	gen0AddStmt(gen0RSet(whole, format, (AInt) 1, value), NULL);

	return foamNewRElt(format, foamCopy(whole), (AInt) 1);
}

local Foam
gen0UnionDispose(Length argc,AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}

/* Enumerations. */

local Foam
gen0EnumEqual(AbSyn *argv, Foam *vals)
{
	Foam	arg0 = genFoamArg(argv, vals, int0);
	Foam	arg1 = genFoamArg(argv, vals, 1);
	Foam	foam = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntEQ;
	foam->foamBCall.argv[0] = arg0;
	foam->foamBCall.argv[1] = arg1;

	return foamNewCast(FOAM_Word, foam);
}

local Foam
gen0EnumNotEqual(AbSyn *argv, Foam *vals)
{
	Foam	arg0 = genFoamArg(argv, vals, int0);
	Foam	arg1 = genFoamArg(argv, vals, 1);
	Foam	foam = foamNewEmpty(FOAM_BCall, 3);

	foam->foamBCall.op = FOAM_BVal_SIntNE;
	foam->foamBCall.argv[0] = arg0;
	foam->foamBCall.argv[1] = arg1;

	return foamNewCast(FOAM_Word, foam);
}

/* TrailingArrays. */

local Foam
gen0TrailingNew(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm tf = symeType(syme);
	AInt format = gen0TrailingFormatNumber(key);
	Foam arg0   = genFoamArg(argv, vals, int0); /* size  */
	Foam arg1   = genFoamArg(argv, vals, 1);    /* hdr   */
	Foam arg2   = genFoamArg(argv, vals, 2);    /* proto */
	Foam whole, hdr, proto, sz;
	int  i, iargc, aargc;
	iargc = tfAsMultiArgc(tfTrailingIArg(key));
	aargc = tfAsMultiArgc(tfTrailingAArg(key));

	whole = gen0Temp0(FOAM_TR, format);
	sz    = foamNewCast(FOAM_SInt, arg0);
	hdr   = gen0CrossToMulti(arg1, tfMapArgN(tf, 1));
	proto = gen0CrossToMulti(arg2, tfMapArgN(tf, 2));

	/* Idea is to generate:
	 * tr   := TRNew(fmt, sz)
	 * tr.x := argx
	 * -- ... and maybe initialize the trailing part as well...
	 */
	gen0AddStmt(foamNewSet(foamCopy(whole), foamNewTRNew(format, sz)), NULL);
	for (i=0; i < iargc; i++) {
		gen0AddStmt(foamNewSet(foamNewIRElt(format, foamCopy(whole), i),
				       hdr->foamValues.argv[i]), NULL);
	}
	return foamNewCast(FOAM_Word, whole);
}

local Foam
gen0TrailingDispose(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	Foam	value = genFoamArg(argv, vals, int0);

	return foamNewFree(value);
}

local Foam
gen0TrailingElt(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm tf  = tfDefineeType(symeType(syme));
	AInt format = gen0TrailingFormatNumber(key);
	Foam whole  = genFoamArg(argv, vals, int0);
	Foam foam = NULL;
	AInt idx;

	whole = foamNewCast(FOAM_TR, whole);
	if (argc == 2) {
		TForm elt = tfMapMultiArgN(tf, argc, 1);
		TForm xtf  = tfTrailingIArg(key);
		idx  = gen0TrailingIndex(xtf, elt);
		foam = foamNewIRElt(format, whole, idx);
	}
	else if (argc == 3) {
		TForm elt = tfMapMultiArgN(tf, argc, 2);
		TForm xtf  = tfTrailingAArg(key);
		Foam  arg1 = genFoamArg(argv, vals, 1);
		idx  = gen0TrailingIndex(xtf, elt);
		arg1 = foamNewCast(FOAM_SInt, arg1);
		arg1 = foamNew(FOAM_BCall, 2, FOAM_BVal_SIntPrev, arg1);
		foam = foamNewTRElt(format, whole, arg1, idx);
	}
	else {
		tf = NULL;
		bug("badly formed special operation");
	}

	return foam;
}

local Foam
gen0TrailingSet(Syme syme, TForm key, Length argc, AbSyn *argv, Foam *vals)
{
	TForm tf  = tfDefineeType(symeType(syme));
	AInt format = gen0TrailingFormatNumber(key);
	Foam whole  = genFoamArg(argv, vals, int0);
	Foam ref, val;
	AInt idx;
	
	whole = foamNewCast(FOAM_TR, whole);
	if (argc == 3) {
		TForm elt  = tfMapMultiArgN(tf, argc, 1);
		TForm xtf  = tfTrailingIArg(key);
		Foam  arg1 = genFoamArg(argv, vals, 2);
		idx  = gen0TrailingIndex(xtf, elt);
		val = gen0MakeMultiEvaluable(FOAM_TR, format, arg1);
		ref = foamNewIRElt(format, whole, idx);
	}
	else if (argc == 4) {
		TForm elt  = tfMapMultiArgN(tf, argc, 2);
		TForm xtf  = tfTrailingAArg(key);
		Foam  arg1 = genFoamArg(argv, vals, 1);
		Foam  arg2 = genFoamArg(argv, vals, 3);
		idx  = gen0TrailingIndex(xtf, elt);
		arg1 = foamNewCast(FOAM_SInt, arg1);
		arg1 = foamNew(FOAM_BCall, 2, FOAM_BVal_SIntPrev, arg1);
		val  = gen0MakeMultiEvaluable(FOAM_TR, format, arg2);
		ref  = foamNewTRElt(format, whole, arg1, idx);
	}
	else {
		tf = NULL;
		val = ref = NULL; 
		bug("badly formed special operation");
	}
	
	gen0AddStmt(foamNewSet(ref, val), NULL);
	return foamCopy(val);
}

/*****************************************************************************
 *
 * :: End of specific generators for special operations.
 *
 ****************************************************************************/

/*
 * Return the format number of a given record type.
 */
AInt
gen0RecordFormatNumber(TForm tf)
{
	Foam		ddecl;
	Length		i, argc;
	AInt		fmt0 = emptyFormatSlot;

	assert (tfIsRecord(tf));
	argc = tfRecordArgc(tf);

	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 1);
	ddecl->foamDDecl.usage = FOAM_DDecl_Record;
	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfRecordArgN(tf, i);
		String	s   = gen0RecFieldName(tf, i);
		FoamTag tag = gen0Type(tfi, &fmt0);
		ddecl->foamDDecl.argv[i] =
			foamNewDecl(tag, strCopy(s), fmt0);
	}

	return gen0AddRealFormat(ddecl);
}

/*
 * Return the format number of a given multi type.
 */
AInt
gen0MultiFormatNumber(TForm tf)
{
	Foam		ddecl;
	Length		i, argc;
	AInt		fmt0 = emptyFormatSlot;

	assert (tfIsMulti(tf));
	argc = tfMultiArgc(tf);

	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 1);
	ddecl->foamDDecl.usage = FOAM_DDecl_Multi;
	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfMultiArgN(tf, i);
		String	s = "";
		FoamTag tag = gen0Type(tfi, &fmt0);
		ddecl->foamDDecl.argv[i] =
			foamNewDecl(tag, strCopy(s), fmt0);
	}

	return gen0AddRealFormat(ddecl);
}

/*
 * Return the format number of a non-void catch block.
 */
AInt
gen0CatchFormatNumber(TForm try, TForm exn)
{
	Foam	ddecl;
	FoamTag	tag;
	AInt	fmt = emptyFormatSlot;
	AInt	empty = emptyFormatSlot;


	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, 3 + 1);
	ddecl->foamDDecl.usage = FOAM_DDecl_Multi;


	/* The first argument is a Bool */
	ddecl->foamDDecl.argv[0] =
		foamNewDecl(FOAM_Bool, strCopy(""), empty);


	/* The second argument is the try-block */
	if (tfIsMulti(try)) {
		tag = FOAM_Word;
		fmt = gen0MultiFormatNumber(try);
	}
	else
		tag = gen0Type(try, &fmt);
	ddecl->foamDDecl.argv[1] = foamNewDecl(tag, strCopy(""), fmt);


	/* The final argument is the handler */
	if (tfIsMulti(exn)) {
		tag = FOAM_Word;
		fmt = gen0MultiFormatNumber(exn);
	}
	else
		tag = gen0Type(exn, &fmt);
	ddecl->foamDDecl.argv[2] = foamNewDecl(tag, strCopy(""), fmt);

	return gen0AddRealFormat(ddecl);
}

/*
 * Return the format number of a void catch block.
 */
AInt
gen0VoidCatchFormatNumber(TForm exn)
{
	Foam	ddecl;
	FoamTag	tag;
	AInt	fmt = emptyFormatSlot;
	AInt	empty = emptyFormatSlot;


	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, 2 + 1);
	ddecl->foamDDecl.usage = FOAM_DDecl_Multi;


	/* The first argument is a Bool */
	ddecl->foamDDecl.argv[0] =
		foamNewDecl(FOAM_Bool, strCopy(""), empty);


	/* The final argument is the handler */
	if (tfIsMulti(exn)) {
		tag = FOAM_Word;
		fmt = gen0MultiFormatNumber(exn);
	}
	else
		tag = gen0Type(exn, &fmt);
	ddecl->foamDDecl.argv[1] = foamNewDecl(tag, strCopy(""), fmt);

	return gen0AddRealFormat(ddecl);
}

/*
 * Return the format number of a C arguments format.
 *
 * The dummy arguments come first with a decl at the 
 * end for the function result.
 *
 * Hacked from gen0FortranSigFormatNumber()
 */ 
AInt
gen0CSigFormatNumber(TForm tf)
{
	Foam		ddecl;
#ifdef AXL_EDIT_1_1_13_27
	Length		i, argc, retc;
#else
	Length		i, argc;
#endif
	char		buffer[120];

	assert (tfIsMap(tf));
	argc = tfMapArgc(tf);

#ifdef AXL_EDIT_1_1_13_27
	/* Generate the format. */
	retc = tfMapRetc(tf);
	if (retc == 1) retc = 0; /* Only want retc for multi-valued imports */
	ddecl = foamNewEmpty(FOAM_DDecl, 1 + argc + retc);
	ddecl->foamDDecl.usage = FOAM_DDecl_CSig;


	/* Process the arguments */
	for (i = 0; i < argc; i++) {
		AInt	fmt;
		char	*str;
		FoamTag	type;
		TForm	tfi = tfMapArgN(tf, i);

		/* Skip any declaration */
		if (tfIsDeclare(tfi)) tfi = tfDeclareType(tfi);


		/* Get the foam type */
		type = gen0Type(tfi, &fmt);


		/* Create a suitable declaration */
#if EDIT_1_0_n1_07
		(void)sprintf(buffer, "P%d", (int) i);
#else
		(void)sprintf(buffer, "P%d", i);
#endif
		str = strCopy(buffer);
		ddecl->foamDDecl.argv[i] = foamNewDecl(type, str, fmt);
	}

	/* Process any multiple return values */
	for (i = 0; i < retc; i++) {
		char *str;
		FoamTag rtype = FOAM_Ptr; /* Always a pointer */
		AInt fmt = emptyFormatSlot;
#if EDIT_1_0_n1_07
		(void)sprintf(buffer, "R%d", (int) i);
#else
		(void)sprintf(buffer, "R%d", i);
#endif
		str = strCopy(buffer);
		ddecl->foamDDecl.argv[argc + i] = foamNewDecl(rtype, str, fmt);
	}
#else
	/* Generate the format */
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 2); /* Include return type */
	ddecl->foamDDecl.usage = FOAM_DDecl_CSig;

	for (i = 0; i < argc + 1; i++) {
		AInt	fmt;
		TForm	tfi;
		FoamTag	type;

		/* Generate a decl for the parameter or return type? */
		tfi = (i == argc) ? tfMapRet(tf) : tfMapArgN(tf, i);


		/* Skip any declaration */
		if (tfIsDeclare(tfi)) tfi = tfDeclareType(tfi);


		/* Get the foam type */
		type = gen0Type(tfi, &fmt);


		/* Create a suitable declaration */
		ddecl->foamDDecl.argv[i] = foamNewDecl(type, strCopy(""), fmt);
	}
#endif

	return gen0AddRealFormat(ddecl);
}

/*
 * Return the format number of a C arguments format.
 *
 * The dummy arguments come first with a decl at the 
 * end for the function result.
 *
 * Hacked from gen0FortranSigFormatNumber()
 */ 
AInt
gen0CPackedSigFormatNumber(TForm tf)
{
	Foam		ddecl;
#ifdef AXL_EDIT_1_1_13_27
	Length		i, argc, retc;
#else
	Length		i, argc;
#endif
	char		buffer[120];

	assert (tfIsPackedMap(tf));
	argc = tfMapArgc(tf);

#ifdef AXL_EDIT_1_1_13_27
	/* Generate the format. */
	retc = tfMapRetc(tf);
	if (retc == 1) retc = 0; /* Only want retc for multi-valued imports */
	ddecl = foamNewEmpty(FOAM_DDecl, 1 + argc + retc);
	ddecl->foamDDecl.usage = FOAM_DDecl_CSig;


	/* Process the arguments */
	for (i = 0; i < argc; i++) {
		AInt	fmt;
		char	*str;
		FoamTag	type;
		TForm	tfi = tfMapArgN(tf, i);

		/* Skip any declaration */
		if (tfIsDeclare(tfi)) tfi = tfDeclareType(tfi);

/*		printf("BDS: Getting Raw Type in gen0CPackedSigFormatNumber\n"); */
		tfi = tfRawType(tfi);

		/* Get the foam type */
		type = gen0Type(tfi, &fmt);


		/* Create a suitable declaration */
#if EDIT_1_0_n1_07
		(void)sprintf(buffer, "P%d", (int) i);
#else
		(void)sprintf(buffer, "P%d", i);
#endif
		str = strCopy(buffer);
		ddecl->foamDDecl.argv[i] = foamNewDecl(type, str, fmt);
	}

	/* Process any multiple return values */
	for (i = 0; i < retc; i++) {
		char *str;
		FoamTag rtype = FOAM_Ptr; /* Always a pointer */
		AInt fmt = emptyFormatSlot;
#if EDIT_1_0_n1_07
		(void)sprintf(buffer, "R%d", (int) i);
#else
		(void)sprintf(buffer, "R%d", i);
#endif
		str = strCopy(buffer);
		ddecl->foamDDecl.argv[argc + i] = foamNewDecl(rtype, str, fmt);
	}
#else
	/* Generate the format */
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 2); /* Include return type */
	ddecl->foamDDecl.usage = FOAM_DDecl_CSig;

	for (i = 0; i < argc + 1; i++) {
		AInt	fmt;
		TForm	tfi;
		FoamTag	type;

		/* Generate a decl for the parameter or return type? */
		tfi = (i == argc) ? tfMapRet(tf) : tfMapArgN(tf, i);


		/* Skip any declaration */
		if (tfIsDeclare(tfi)) tfi = tfDeclareType(tfi);

/*		printf("BDS: Getting Raw Type in gen0CPackedSigFormatNumber\n"); */
		tfi = tfRawType(tfi);

		/* Get the foam type */
		type = gen0Type(tfi, &fmt);


		/* Create a suitable declaration */
		ddecl->foamDDecl.argv[i] = foamNewDecl(type, strCopy(""), fmt);
	}
#endif

	return gen0AddRealFormat(ddecl);
}

/*
 * Return the format number of a Fortran arguments format.
 *
 * The dummy arguments come first with a decl at the 
 * end for the function result.
 *
 * Note that this and gen0FortranSigExportNumber() are
 * mutually recursive.
 */ 
AInt
gen0FortranSigFormatNumber(TForm tf, Bool modargs)
{
	Foam		ddecl;
	Length		i, argc;
	AInt		fmt0 = emptyFormatSlot;

	assert (tfIsMap(tf));
	argc = tfMapArgc(tf);

	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 2);
	ddecl->foamDDecl.usage = FOAM_DDecl_FortranSig;
	for (i = 0; i < argc + 1; i++) {
		TForm	tfi;
		String	namefield, tstr;
		FoamTag	type;
		Bool	isRefArg;
		FortranType ftype;

		if (i == (argc + 1) - 1) {
			/* Generate a decl for the return type */
			tfi = tfMapRet(tf);
			isRefArg = false;
		} else {
			tfi = tfMapArgN(tf, i); 

			/* Skip any declaration */
			if (tfIsDeclare(tfi))
				tfi = tfDeclareType(tfi);

			isRefArg = tfIsReferenceFn(tfi);
			if (isRefArg) tfi = tfReferenceArg(tfi);
		}
		type = gen0Type(tfi, NULL);
		if (tfIsDefine(tfi)) tfi = tfDefineDecl(tfi);
		if (tfIsDeclare(tfi)) tfi = tfDeclareType(tfi);
		ftype = ftnTypeFrDomTForm(tfi);
		if (ftype) {
			/*
			 * We store the Fortran category attribute
			 * which this domain has since that doesn't
			 * change with macros etc. Really we ought
			 * to be able to pick up the original TForm
			 * rather than trying to pick it out of a
			 * piece of text in the FOAM.
			 */
			tstr = ftnNameFrType(ftype);
			if (isRefArg) {
				if (modargs)
					namefield = strlConcat(MODIFIABLEARG, ":", tstr, NULL);
				else
					namefield = strlConcat(":", tstr, NULL);
				strFree(tstr);
			}
			else
				namefield = tstr;
			ddecl->foamDDecl.argv[i] = foamNewDecl(type, namefield, fmt0);
		}
		else if (type == FOAM_Word) {
			tstr = gen0TypeString(tfGetExpr(tfi));	
			if (isRefArg) {
				if (modargs)
					namefield = strlConcat(MODIFIABLEARG, ":", tstr, NULL);
				else
					namefield = strlConcat(":", tstr, NULL);
				strFree(tstr);
			}
			else
				namefield = tstr;
			ddecl->foamDDecl.argv[i] = foamNewDecl(type, namefield, fmt0);
		} 
		else if (type == FOAM_Clos) 
			/* Generate a separate format for procedure parameters */			
			ddecl->foamDDecl.argv[i] = foamNewDecl(FOAM_Clos, strCopy(""), gen0FortranSigExportNumber(tfi)); 
		else {
			if (isRefArg) {
				if (modargs) {
					namefield = strCopy(MODIFIABLEARG);
				}
				else
					namefield = strCopy("");
			}
			else
				namefield = strCopy("");
			ddecl->foamDDecl.argv[i] = foamNewDecl(type, namefield, fmt0);
		}
	}

	return gen0AddRealFormat(ddecl);
}

/*
 * Similar to gen0FortranSigFormatNumber() except that
 * we only generate FOAM_Word argument types unless it
 * is a function argument. This is because all the code
 * to do the Fortran argument unpacking for an exported
 * Aldor function is created at the genFoam() stage
 * instead of during genC(). We do, however, need to take
 * extra care with String arguments and return types.
 *
 * Note that this and gen0FortranSigFormatNumber() are
 * mutually recursive.
 */
AInt
gen0FortranSigExportNumber(TForm tf)
{
	Foam		ddecl;
	Length		i, argc;
	AInt		argfmt, fmt0 = emptyFormatSlot;

	assert (tfIsMap(tf));
	argc = tfMapArgc(tf);

	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 2);
	ddecl->foamDDecl.usage = FOAM_DDecl_FortranSig;

	for (i = 0; i < argc + 1; i++)
	{
		TForm		tfi;
		FoamTag		type;
		Foam		decl;
		FortranType	ftype;
		Bool		isRefArg;

		if (i == (argc + 1) - 1)
		{
			/* Generate a decl for the return type */
			tfi = tfMapRet(tf);
			isRefArg = false;
		}
		else
		{
			tfi = tfMapArgN(tf, i); 

			/* Skip any declaration */
			if (tfIsDeclare(tfi))
				tfi = tfDeclareType(tfi);


			/* Unpack references */
			isRefArg = tfIsReferenceFn(tfi);
			if (isRefArg)
				tfi = tfReferenceArg(tfi);
		}

		type = gen0Type(tfi, NULL);
		if (tfIsDefine(tfi)) tfi = tfDefineDecl(tfi);
		if (tfIsDeclare(tfi)) tfi = tfDeclareType(tfi);
		ftype = ftnTypeFrDomTForm(tfi);


		/* Generate a separate format for procedure parameters */
		if ((ftype == FTN_XLString) || (ftype == FTN_String))
		{
			String tstr = ftnNameFrType(ftype);
			if (isRefArg)
				tstr = strlConcat(MODIFIABLEARG, ":", tstr, NULL);
			decl = foamNewDecl(type, tstr, fmt0);
		}
		else if (type == FOAM_Clos)
		{
			argfmt = gen0FortranSigFormatNumber(tfi, false);
			decl   = foamNewDecl(FOAM_Clos, strCopy(""), argfmt);
		}
		else if (i == (argc + 1) - 1)
		{
			/* Return type */
			String tstr = ftnNameFrType(ftype);
			decl = foamNewDecl(type, tstr, fmt0);
		}
		else
			decl = foamNewDecl(FOAM_Word, strCopy(""), fmt0);

		ddecl->foamDDecl.argv[i] = decl;
	}

	return gen0AddRealFormat(ddecl);
}

String
gen0TypeString(Sefo sefo)
{
	Length i;
	String s1, s2, s3 = NULL;

	if (abIsLeaf(sefo)) 
	{
		AbSynTag tag = abTag(sefo);

		if (abIsSymTag(tag))
			s1 = symString(abLeafSym(sefo));
		else if (abIsStrTag(tag))
			s1 = abLeafStr(sefo);
		else
			s1 = "?";

		return strCopy(s1);
	}
	else if (abIsApply(sefo))
	{
		/*
		 * Treat Ref(T) differently. This code is disgusting
		 * and will fail if the domain Ref has been renamed
		 * by use of definitions (A == Ref). How about checking
		 * tfIsReferenceFn() ...
		 */
		AbSyn op = abApplyOp(sefo);
		if (abIsId(op))
		{
			if (!strcmp(symString(abIdSym(op)),
				 symString(ssymReference)))
			{
				/* It's a reference */
				for (i = 1; i < abArgc(sefo); i += 1) {
					s1 = gen0TypeString(abArgv(sefo)[i]);
					if (s3) {
						s2 = strConcat(s3, ".");
						s2 = strConcat(s2, s1);
						strFree(s1);
						strFree(s3);
						s3 = s2;
					}
					else
						s3 = s1;
				}
				return s3;
			}
			/* Fall through ... */
		}
		/* Fall through ... */
	}


	/* Any other type ... */
	for (i = 0; i < abArgc(sefo); i += 1) {
		s1 = gen0TypeString(abArgv(sefo)[i]);
		if (s3) {
			s2 = strConcat(s3, ".");
			s2 = strConcat(s2, s1);
			strFree(s1);
			strFree(s3);
			s3 = s2;
		}
		else
			s3 = s1;
	}
	return s3;
}

/*
 * !! This function does not appear to be used anymore !!
 */
#if 0
AInt
gen0FortranMFmtNumber(TFormList returntypes)
{
	Foam ddecl;
	TForm tf;
	Length i, argc;
	AInt fmt0 = emptyFormatSlot;

	argc = listLength(TForm)(returntypes);
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 1);
	ddecl->foamDDecl.usage = FOAM_DDecl_Multi;
	for (i = 0; i < argc; i++, returntypes = listFreeCons(TForm)(returntypes)) {
		tf = car(returntypes);
		ddecl->foamDDecl.argv[i] =
			foamNewDecl(gen0Type(tf, NULL), strCopy(""), fmt0);
	}

	return gen0AddRealFormat(ddecl);
}
#endif

/* s/b extern */
local AInt
gen0CrossFormatNumber(TForm tf)
{
	Foam		ddecl;
	Length		i, argc;
	AInt		fmt0 = emptyFormatSlot;

	assert (tfIsCross(tf));
	argc = tfCrossArgc(tf);

	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, argc + 1);
	/* DDecl_Record for now, should be DDecl_Cross */
	ddecl->foamDDecl.usage = FOAM_DDecl_Record; 
	for (i = 0; i < argc; i += 1) {
		TForm	tfi = tfCrossArgN(tf, i);
		FoamTag tag = gen0Type(tfi, &fmt0);
		ddecl->foamDDecl.argv[i] =
			foamNewDecl(tag, strCopy(""), fmt0);
	}

	return gen0AddRealFormat(ddecl);
}

local AInt
gen0TrailingFormatNumber(TForm tf)
{
	TForm 		atf, itf;
	Foam		ddecl;
	Length		i, aargc, iargc;
	AInt		fmt0 = emptyFormatSlot;

	assert (tfIsTrailingArray(tf));

	itf = tfTrailingIArg(tf);
	atf = tfTrailingAArg(tf);
	aargc = tfAsMultiArgc(atf);
	iargc = tfAsMultiArgc(itf);

	/* Generate the format. */
	ddecl = foamNewEmpty(FOAM_DDecl, iargc + aargc + 2);

	ddecl->foamDDecl.usage = FOAM_DDecl_TrailingArray; 
	ddecl->foamDDecl.argv[0] = foamNewDecl(FOAM_NOp, strCopy(""), iargc);

	for (i = 0; i < iargc; i += 1) {
		TForm	tfi = tfAsMultiArgN(itf, iargc, i);
		Syme 	id  = tfDefineeSyme(tfi);
		String  s = id ? symeString(id) : "";
		FoamTag tag = gen0Type(tfi, &fmt0);
		ddecl->foamDDecl.argv[i + 1] =
			foamNewDecl(tag, strCopy(s), fmt0);
	}
	
	for (i = 0; i < aargc; i += 1) {
		TForm	tfi = tfAsMultiArgN(atf, aargc, i);
		Syme 	id  = tfDefineeSyme(tfi);
		String  s = id ? symeString(id) : "";
		FoamTag tag = gen0Type(tfi, &fmt0);
		ddecl->foamDDecl.argv[iargc + 1 + i] =
			foamNewDecl(tag, strCopy(s), fmt0);
	}
	
	return gen0AddRealFormat(ddecl);
}

AInt
gen0MFmtNumberForSig(int sz, FoamTag *types)
{
	Foam 	ddecl;
	String  s  = "";
	int i;
	
	ddecl = foamNewEmpty(FOAM_DDecl, sz+1);
	ddecl->foamDDecl.usage = FOAM_DDecl_Multi;

	for (i=0; i<sz ; i++) {
		ddecl->foamDDecl.argv[i] = 
			foamNewDecl(types? types[i]: FOAM_Word, 
				    strCopy(s), emptyFormatSlot);
	}
	return gen0AddRealFormat(ddecl);
}

/*
 * Create a field name for a Foam record.
 */
local String
gen0RecFieldName(TForm tf, int i)
{
	TForm	f = tfRecordArgN(tf, i);
	Syme	syme = tfDefineeSyme(f);
	return syme ? strCopy(symeString(syme)) : strPrintf("rf%d", i);
}

/*
 * Create a field name for a Fortran format.
 */
/*
 * !! This function does not appear to be used anymore !!
 */
#if 0
local String
gen0FortranFieldName(TForm tf, int i)
{
	TForm	f = tfMapArgN(tf, i);
	Syme	syme = tfDefineeSyme(f);
	return syme ? strCopy(symeString(syme)) : strCopy("");
}
#endif


/*
 * Detection of modifiable (reference) parameters
 */
/*
 * !! This function does not appear to be used anymore !!
 */
#if 0
local Bool
gen0IsParamReference(TForm tf, int i)
{
	TForm tfa = tfMapArgN(tf, i);

	/* Skip any declaration */
	if (tfIsDeclare(tfa))
		tfa = tfDeclareType(tfa);


	return tfIsReferenceFn(tfa);
}
#endif

/*
 * Generate Foam for an assignment.
 */
local Foam
genAssign(AbSyn absyn)
{
	AbSyn lhs = absyn->abAssign.lhs;
	AbSyn rhs = absyn->abAssign.rhs;
	Foam	rhsFoam, foam;

	if (gen0IsFortranCall(rhs))
		gen0FortranFnResult = lhs;

	if (gen0IsImplicitSet(lhs))
		return gen0ImplicitSet(absyn);

	/*
	 * This next line must never appear before code which
	 * generates foam for the RHS, eg. gen0ImplicitSet().
	 * Otherwise we end up with two identical foam blocks
	 * for the RHS. This is not only inefficient but if
	 * the RHS has side-effects then it is plain wrong.
	 */
	rhsFoam = genFoamVal(rhs);


	/* This will go once we have the embed tags right */
#if 0
	if (abTContext(rhs) == AB_Embed_Fail) {
		embed = tfSatEmbedType(abTUnique(rhs), abTUnique(lhs));
		rhsFoam = gen0Embed(rhsFoam, rhs, gen0AbType(rhs), embed);
	}
#endif
	if (abTag(lhs) == AB_Declare)
		lhs = lhs->abDeclare.id;
	if (abTag(lhs) == AB_Comma)
		return gen0MultiAssign(FOAM_Set, lhs, rhsFoam);
	if (gen0IsFortranCall(rhs))
		gen0FortranFnResult = 0;

	foam = gen0AssignLhs(lhs, rhsFoam);

	if (gen0DebugWanted) gen0DbgAssignment(lhs);

	return foam;
}

local Foam
gen0AssignLhs(AbSyn lhs, Foam rhsFoam)
{
	Foam foam;
#if 0
	if (gen0IsImplicitSet(lhs)) {
		gen0ImplicitSet(gen0Type(gen0AbType(lhs), NULL), lhs, rhsFoam);
		/* !!! Fixme ??? */
		foam = NULL;
		return;
	}
#endif
	foam = foamNewSet(genId(abDefineeId(lhs)),
			  rhsFoam);
	foam = gen0SetValue(foam, lhs);

	return foam;
}


local Bool
gen0IsImplicitSet(AbSyn lhs)
{
	if (abTag(lhs) != AB_Apply)
		return false;
	return (abImplicitSyme(lhs) != 0);
}


local Foam
gen0ImplicitSet(AbSyn absyn)
{
	AbSyn   lhs  = absyn->abAssign.lhs;
	AbSyn   rhs  = absyn->abAssign.rhs;
	AbSyn  *argv;
	FoamTag	type = gen0Type(gen0AbContextType(rhs), NULL);
	Syme	syme = abImplicitSyme(lhs);
	Length	argc = abApplyArgc(lhs) + 2;
	Foam	foam;

	argv = gen0MakeImplicitArgs(argc, absyn, abSetArgf);
	foam = gen0ApplyImplicitSyme(type, syme, argc, argv, NULL);
	foam = gen0ApplyReturn(absyn, syme, gen0AbType(absyn), foam);
	return gen0SetValue(foam, lhs);
}

/*
 * Generate code for tuple assignemnts.
 *
 * !!!FIXME!!! If invoked from gen0Define then we ought to ensure
 * that gen0SymeSetInit() is invoked on each lhs target.
 */
local Foam
gen0MultiAssign(FoamTag set, AbSyn lhs, Foam rhsFoam)
{
	Foam	temps;
	int	i, argc = abArgc(lhs);

	assert(foamTag(rhsFoam) == FOAM_Values);
	assert(argc == foamArgc(rhsFoam));

	/* Create temporaries so that (a,b) := (b,a) will work */
	temps = foamNewEmpty(FOAM_Values, argc);
	for(i = 0; i < argc; i++) {
		FoamTag tag;
		AInt    fmt;
		tag = gen0Type(gen0AbType(abArgv(lhs)[i]), &fmt);
		temps->foamValues.argv[i] = gen0TempLocal0(tag, fmt);

		gen0AddStmt(foamNew(set, 2, 
				    foamCopy(temps->foamValues.argv[i]),
				    rhsFoam->foamValues.argv[i]), lhs);
	}
#if AXL_EDIT_1_1_13_33
	/* Copy the temporary results into the targets */
	for(i=0; i < argc; i++) {
		AbSyn	lhsi = abArgv(lhs)[i];
		AbSyn	id = abDefineeId(lhsi);
		Syme	syme = abSyme(id);
		Foam	lhsFoam = genId(id);
		Foam	rhsFoam = foamCopy(temps->foamValues.argv[i]);
		Foam	def = foamNew(set, 2, lhsFoam, rhsFoam);

		/* Note the export slot to be initialised if exporting */
		if (gen0IsDomLevel(gen0State->tag) && gen0State->tag != GF_File)
			gen0SymeSetInit(syme, lhsFoam);

		/* Create the actual definition */
		gen0AddStmt(def, lhs);

		/* Link between definitions and const values */
		if (set==FOAM_Def) def->foamDef.hdr.defnId = abDefineIdx(id);

		/* FIXME: deal with domains defs here (see gen0Define) */
	}
#else
	for(i=0; i< argc; i++) {
		AbSyn	lhsi = abArgv(lhs)[i];

		gen0AddStmt(foamNew(set, 2, genId(abDefineeId(lhsi)),
				    foamCopy(temps->foamValues.argv[i])), lhs);
	}
#endif
	if (!gen0ValueMode) {
		for(i = 0; i < argc; i++)
			gen0FreeTemp(temps->foamValues.argv[i]);
		foamFree(temps);

		return 0;
	}
	else
		return temps;
}


/* 
 * Utility function for getting a safe
 * return value from a foam assignment
 */
local Foam
gen0SetValue(Foam set, AbSyn absyn)
{
	Foam	lhs;
	if (!set || (foamTag(set) != FOAM_Set && foamTag(set) != FOAM_Def))
		return set;
	lhs = set->foamSet.lhs;
	if (!gen0ValueMode) {
		gen0AddStmt(set, absyn);
		return 0;
	}
	if (!foamHasSideEffect(lhs)) {
		gen0AddStmt(set, absyn);
		return foamCopy(lhs);
	}
	/**!! Need code to factor out side-effecting part and set to a local */
	if (!gen0ValueMode) {
		gen0AddStmt(set, absyn);
		return 0;
	}
	gen0AddStmt(set, absyn);
	return foamCopy(lhs);
}

/*
 * Generate Foam for a constant definition.
 */
local Foam
genDefine(AbSyn absyn)
{
	AbSyn	lhs = abDefineeId(absyn);
	Syme	syme = abSyme(lhs);
	Foam	result;

	assert(syme);
	if (symeExtension(syme))
		result = gen0Extend(absyn);
	else
		result = gen0Define(absyn);

	return result;
}

/*
 * Generate Foam for a constant definition.
 */
local Foam
gen0Define(AbSyn absyn)
{
	AbSyn	lhs = absyn->abDefine.lhs;
	AbSyn	rhs = absyn->abDefine.rhs;
	AbSyn   id = NULL, type = NULL;
	AbEmbed embed;
	Foam	rhsFoam, def, res;
	Foam    lhsFoam = (Foam)NULL;
	Foam    condFoam = (Foam)NULL;
	Syme	syme;

	if (abTag(lhs) != AB_Comma) {
		id = abDefineeId(lhs);
		type = abDefineeTypeOrElse(lhs, NULL);
	}

	rhsFoam = gen0DefineRhs(id, type, rhs);

	/* This will go once we have the embed tags right */
	if (abTContext(rhs) == AB_Embed_Fail) {
		embed = tfSatEmbedType(abTUnique(rhs), abTUnique(lhs));
		rhsFoam = gen0Embed(rhsFoam, rhs, gen0AbType(rhs), embed);
	}
	
	if (abTag(lhs) == AB_Comma)
		return gen0MultiAssign(FOAM_Def, lhs, rhsFoam);

	syme = abSyme(id);


	if (symeIsExport(syme) || symeIsExtend(syme)) {
		/*
		 * If conditional exports weren't bad enough, some
		 * people write code with conditional defaults. We
		 * have to deal with conditional exports first even
		 * if they are defaults: we may need to fix this so
		 * that conditional defaults are more robust.
		 */
		if (!symeUnconditional(syme) &&
				gen0IsDomLevel(gen0State->tag) &&
				(gen0State->tag != GF_File))
		{
			/* Check for previously computed value */
			condFoam = gen0SymeCond(syme);


			/* Cache export value if not found */
			if (!condFoam) {
				/* Type of the export */
				AInt type = gen0Type(symeType(syme), NULL);


				/* Create a temporary for the export value */
				condFoam = gen0Temp(type);


				/* Stash DefnId for gen0TypeAddExportSlot */
				condFoam->foamGen.hdr.defnId = abDefineIdx(id);


				/* Associate foam with syme/condition */
				gen0SymeSetCond(syme, condFoam);
			}


#if 0
			/* Debugging output */
			(void)fprintf(dbOut, "+++ [%d] ",
				condFoam->foamGen.hdr.defnId);
			foamPrintDb(condFoam);
			(void)fprintf(dbOut, "   ");
			symePrintDb(syme);
			(void)fprintf(dbOut, "   ");
			ablogPrintDb(gfCondKnown);
#endif
		}
		else if (symeHasDefault(syme)) {
			/*
			 * Check that the default bit was not
		   	 * mis-inherited.
			 */
			assert(gen0State->tag == GF_Default ||
		       	gen0State->tag == GF_DefaultCat);
			lhsFoam = gen0SymeInit(syme);
		
			if (!lhsFoam) {
				AInt	type = gen0Type(symeType(syme), NULL);
				lhsFoam = gen0Temp(type);
			}
		}
	}


	/* Did we get any foam for the LHS? */
	if (!lhsFoam) lhsFoam = genFoamVal(lhs);


	/* Note the export slot to be initialised if exporting */
	if (gen0IsDomLevel(gen0State->tag) && gen0State->tag != GF_File)
		gen0SymeSetInit(syme, lhsFoam);


	/* Use local instead of export slot if conditional */
	if (condFoam) lhsFoam = foamCopy(condFoam);


	/* Create the export or conditional definition */
	def = foamNewDef(lhsFoam, rhsFoam);


	/* Link between definitions and const values */
	def->foamDef.hdr.defnId = abDefineIdx(id);


	/* Decide where to put the definition */
        if (gen0IsDomainInit(rhsFoam)) {
                gen0AddInit(def);
		res = NULL;
#if AXL_EDIT_1_1_12p6_24
		/* Stamp the domain with its hash code (if appropriate) */
		if (symeHashNum(syme) && (foamTag(rhsFoam) == FOAM_Clos)) {
			AInt	hash = symeHashNum(syme);
			Foam	dom = foamCopy(lhsFoam);

			/* Don't use gen0AddStmt() ... */
			gen0AddInit(gen0RtSetProgHash(dom, hash));
		}
#endif
	}
	else 
		res = gen0SetValue(def, absyn);

	return res;
}

local Foam
gen0DefineRhs(AbSyn id, AbSyn type, AbSyn rhs)
{
	Scope("gen0Define");

	String	fluid(gen0ProgName);
	String	fluid(gen0DefName);
	Syme    syme = NULL;
	AbSyn 	oldex;
	Stab	stab = abStab(rhs);
	Foam 	rhsFoam;
	
	if (id) syme = abSyme(id);

	if (abIsAnyLambda(rhs)) {
		if (type) type = abMapRet(type);
		gen0ProgName = strCopy(symeString(syme));
		oldex = gen0ProgPushExporter(id);

		/* patch up ConstNum(syme)  */
		gen0AddConst(symeConstNum(syme), gen0NumProgs);
		genSetConstNum(syme, abDefineIdx(id), (UShort) gen0NumProgs, true);

		rhsFoam = gen0Lambda(rhs, syme, type);
		gen0ProgPopExporter(oldex);
	}
	else if (abTag(rhs) == AB_Add) {
		gen0ProgName = gen0FileName;
		gen0DefName = strCopy(syme ? symeString(syme) : gen0ProgName);
		if (genIsRuntime()) {
			gen0Vars(stab);
			rhsFoam = genFoamVal(rhs->abAdd.capsule);
		}
		else {
			oldex = gen0ProgPushExporter(id);
#if EDIT_1_0_n1_06
			rhsFoam = gen0AddBody0(rhs, stab, type);
#else
			rhsFoam = gen0AddBody0(rhs->abAdd.base,
					       rhs->abAdd.capsule,
					       stab, type);
#endif
			gen0ProgPopExporter(oldex);
		}
	}
	else if (syme && tfIsCategoryType(symeType(syme))) {
		gen0ProgName = strCopy(symeString(syme));
		gen0DefName = strCopy(gen0ProgName);
		oldex = gen0ProgPushExporter(id);
		rhsFoam = gen0MakeDefaultPackage(rhs, stab, true, syme);
		gen0ProgPopExporter(oldex);
	}
	else {
		gen0ProgName = gen0FileName;
		rhsFoam = genFoamVal(rhs);
	}

	if (syme && gen0AbSynHasConstHash(rhs))
		symeSetHashNum(syme, strHash(symeString(syme)));

	Return(rhsFoam);
}


local Bool
gen0IsDomainInit(Foam foam)
{
	Foam	arg;
	if (foamTag(foam) == FOAM_Clos &&
	    foamTag(foam->foamClos.prog) == FOAM_Const &&
	    foamTag(foam->foamClos.env) == FOAM_Env)
		return true;
	if (foamTag(foam) != FOAM_CCall ||
	    foamArgc(foam) != 3 ||
	    foamTag(foam->foamCCall.op) != FOAM_Glo)
		return false;
	arg = foam->foamCCall.argv[0];
	return (foamTag(arg) == FOAM_Clos &&
		foamTag(arg->foamClos.prog) == FOAM_Const &&
		foamTag(arg->foamClos.env) == FOAM_Env);
}

/*
 * Generate Foam for a domain extension.
 */
local Foam
gen0Extend(AbSyn absyn)
{
	AbSyn		lhs = abDefineeId(absyn);
	Syme		syme = abSyme(lhs), extension;
	SymeList	extendee;
	Foam		result;

	assert(syme);
	extension = symeExtension(syme);
	assert(extension && symeIsExtend(extension));
	extendee  = symeExtendee(extension);
	assert(listMemq(Syme)(extendee, syme));

	symeListSetExtension(extendee, NULL);

	result = gen0Define(absyn);
	if (symeHashNum(syme)) symeSetHashNum(extension, symeHashNum(syme));
	symeListSetExtension(extendee, extension);

	if (syme == car(listLastCons(Syme)(extendee))) {
		Foam	val = gen0MakeExtend(extension, symeType(extension));
		Foam	dom = gen0Syme(extension);
		Foam	def = foamNewDef(dom, val);
		if (gen0IsDomainInit(val)) {
			gen0AddInit(def);
			result = NULL;
#if AXL_EDIT_1_1_12p6_25
			/* Stamp the extension with its hash code */
			if (symeHashNum(extension) &&
				(foamTag(val) == FOAM_Clos)) {
				AInt	hash = symeHashNum(extension);
				Foam	ext = foamCopy(dom);

				gen0AddInit(gen0RtSetProgHash(ext, hash));
			}
#endif
		}
		else
			result = gen0SetValue(def, absyn);
	}

	return result;
}

/*
 * Generate code for the function to create a (possibly parameterized)
 * extension domain.
 */
local Foam
gen0MakeExtend(Syme syme, TForm tf)
{
	if (tfIsMap(tf))
		return gen0MakeExtendLambda(syme, tf);
	else
		return gen0MakeExtendBase(syme);
}

/*
 * Generate code for the function to create a parameterized extension domain.
 */
local Foam
gen0MakeExtendLambda(Syme syme, TForm tf)
{
	Foam		*paramv;
	Foam		foam, clos, var;
	FoamTag		retType;
	AInt		index;
	TForm		tfret;
	Stab		stab;
	RTCacheInfo	cache;
#if AXL_EDIT_1_1_12p6_25
	int		i;
#else
	int		hashCode, i;
#endif
	
	assert(tfIsMap(tf));

	tfret = tfMapRet(tf);
	retType = gen0Type(tfret, NULL);

	stab = gen0State->stab;
	if (tfMapArgc(tf) > 0) {
		/* This is just a really cheezy way to get a stab. */
		assert(tfSymes(tf));
		stab = listCons(StabLevel)(symeDefLevel(car(tfSymes(tf))),
					   stab);
	}
	
	cache = gen0CacheMakeEmpty(NULL);
	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(gen0ProgName, NULL);

	index = gen0FormatNum;
	gen0ProgPushState(stab, GF_Lambda);

	if (tfIsMulti(tfret))
		foam->foamProg.format = gen0MultiFormatNumber(tfret);

	gen0State->type = tf;
	gen0State->program = foam;

	gen0PushFormat(index);

	gen0Vars(stab);
	var = gen0Temp(FOAM_Word);

	paramv = (Foam*) stoAlloc(OB_Other, sizeof(Foam)*tfMapArgc(tf));
	for (i=0; i<tfMapArgc(tf); i++) {
		paramv[i] = genFoamVal(abDefineeId(tfExpr(tfMapArgN(tf, i))));
	}
	gen0CacheCheck(cache, tfMapArgc(symeType(syme)), paramv);

	gen0AddStmt(foamNewSet(var, gen0MakeExtend(syme, tfret)), NULL);
	var = gen0CacheReturn(cache, foamCopy(var));
	gen0AddStmt(foamNewReturn(var), NULL);
	gen0ProgAddFormat(index);
	gen0IssueDCache();
	gen0ProgFiniEmpty(foam, retType, int0);

        foamOptInfo(foam) = inlInfoNew(gen0State->stab, foam, NULL, false);
        foamProgSetGetter(foam);

	gen0ProgPopState();
	if (gen0IsDomainInit(clos))
	    	gen0AddInit(cache->init);
	else
		gen0AddStmt(cache->init, cache->ab);
	gen0CacheKill(cache);
#if AXL_EDIT_1_1_12p6_25
#else
	hashCode = symeHashNum(syme);
	if (hashCode)
		gen0AddStmt(gen0RtSetProgHash(foamCopy(clos), hashCode), NULL);
#endif

	return clos;
}

/*
 * Generate code for the function to create an extension domain.
 */
local Foam
gen0MakeExtendBase(Syme syme)
{
	Foam		foam, clos;
	AInt		index;
	Length		argc;
	String		argv[1];

	argc    = 1;
	argv[0] = "domain";

	assert(!genIsRuntime());

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty("extend0", NULL);

	index = gen0FormatNum;
        gen0ProgPushState(gen0State->stab, GF_Add0);

	gen0PushFormat(index);

	gen0ProgAddParams(argc, argv);

        gen0State->program = foam;
        gen0State->program->foamProg.infoBits = IB_SIDE;

	gen0MakeExtendParents(syme, symeExtendee(syme));
	//gen0AddStmt(foamNewReturn(foamNewCast(FOAM_Clos, foamNewPar(int0))), NULL);
	gen0AddStmt(foamNewReturn(foamNewCast(FOAM_Clos, foamNewNil())), NULL);

	gen0ProgAddFormat(index);
	gen0IssueDCache();
	gen0ProgFiniEmpty(foam, FOAM_Clos, int0);

        foamOptInfo(foam) = inlInfoNew(gen0State->stab, foam, NULL, false);
        foamProgSetGetter(foam);

	gen0ProgPopState();

	foam = gen0BuiltinCCall(FOAM_Word, "extendMake", "runtime", 1, clos);
	return foam;
}

/*
 * Helper functions for gen0MakeExtend.
 */
local void
gen0MakeExtendParents(Syme syme, SymeList symes)
{
	Foam	pars, result;
	Length	i, argc = listLength(Syme)(symes);

	/* Create the foam array for the parent vector. */
	pars = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	gen0AddStmt(gen0ANew(pars, FOAM_Word, argc), NULL);

	/* Fill the slots in the foam array. */
	for (i = argc - 1; symes; i -= 1, symes = cdr(symes))
		gen0MakeExtendParent(syme, car(symes), i, pars);

	/* Create the Aldor array for the domain parents. */
	pars = gen0MakeArray(foamNewSInt(argc), pars, NULL);

	result = gen0BuiltinCCall(FOAM_Word, "extendFill!", "runtime", 2,
				  foamNewPar(int0), pars);
	gen0AddStmt(result, NULL);
}

local void
gen0MakeExtendParent(Syme syme, Syme parent, Length i, Foam pars)
{
	Foam	par = gen0MakeExtendApply(symeType(syme), gen0Syme(parent));

	gen0AddStmt(gen0ASet(pars, i, FOAM_Word, par), NULL);
}

local Foam
gen0MakeExtendApply(TForm tf, Foam foam)
{
	Foam	call;
	Length	i, j, k, argc;

	if (!tfIsMap(tf)) {
		if (gen0Type(tf, NULL) != FOAM_Word)
			foam = foamNewCast(FOAM_Word, foam);
		return foam;
	}

	argc = tfMapArgc(tf);

	call = foamNewEmpty(FOAM_CCall, TypeSlot+OpSlot + argc);
	call->foamCCall.type = gen0Type(tfMapRet(tf), NULL);
	call->foamCCall.op = foam;

	for (i = TypeSlot+OpSlot, j = 0; j < argc; i += 1, j += 1) {
		TForm	tfj = tfMapArgN(tf, j);
		Syme	syme = NULL;
		if (tfIsMeet(tfj))
			for (k = 0; !syme && k < tfMeetArgc(tfj); k += 1)
				syme = tfDefineeSyme(tfMeetArgv(tfj)[k]);
		else
			syme = tfDefineeSyme(tfj);
		assert(syme);
		call->foamGen.argv[i].code = gen0Syme(syme);
	}

	return gen0MakeExtendApply(tfMapRet(tf), call);
}

/*
 * Generate Foam for an identifier.
 */
local Foam
genId(AbSyn absyn)
{
	return gen0ExtendSyme(abSyme(absyn));
}

/*
 * Generate Foam for a literal.
 */
local Foam
genLit(AbSyn absyn)
{
	Syme	syme = abSyme(absyn);
	FoamTag type = gen0Type(gen0AbType(absyn), NULL);
	String	str  = abLeafStr(absyn);
	Foam   *argloc;
	Foam	foam = gen0ApplySyme(type, syme, 
				     abSymeImpl(absyn), 1, &argloc);

	argloc[0] = foamNewCast(FOAM_Word,gen0CharArray(str));

	return foam;
}

local Bool
gen0IsInnerVar(Syme syme, AInt level)
{
	GenFoamState	s;
	if (level != 0)
		return false;
	s = gen0NthState(level);
	return symeDefLevelNo(syme) > stabLevelNo(s->stab);
}

/*!!
 * This is only needed because symes aren't shared between add bodys and
 * decls for domain return values.
 */
local int
gen0ParamIndex(Syme param)
{
	StabLevel	stab = symeDefLevel(param);
        int             i = 0, j=0, me=-1;
	Syme		syme;
	SymeList	l;

	assert (stab);
	for(i = 0, l = stab->boundSymes; l; l = cdr(l), i++) {
		syme = car(l);
                if (symeIsParam(syme))
                         j++;
		if (symeId(param)==symeId(syme)) 
			me = j;
	}
	assert(me>=0);

        return j-me;
}

/*
 * Generate Foam for the Syme of an identifier.
 */

Foam
gen0ExtendSyme(Syme syme)
{
	while (symeExtension(syme)) syme = symeExtension(syme);
	return gen0Syme(syme);
}

Foam
gen0Syme(Syme syme)
{
	Foam		foam;

	switch (symeKind(syme)) {
	case SYME_Param:
	case SYME_LexConst:
	case SYME_LexVar:
	case SYME_Export:
	case SYME_Extend: 
		foam = gen0SymeGeneric(syme);
		break;

	case SYME_Foreign: {
		assert(gen0FoamKind(syme) == FOAM_Glo);
		if (tfIsMap(symeType(syme))) {
			foam = gen0ForeignWrapValue(syme);
		} else
			foam = foamNew(gen0FoamKind(syme), 1,
				       (AInt) gen0VarIndex(syme));
		break;
	}
	case SYME_Fluid:
		foam = foamNewFluid(gen0VarIndex(syme));
		foamProgSetUsesFluid(gen0State->program);
		break;
	case SYME_Library:
		bug("Invalid use of library");
		NotReached(foam = 0);
		break;

	case SYME_Archive:
		bug("Invalid use of archive");
		NotReached(foam = 0);
		break;

	case SYME_Import:
		foam = gen0SymeImport(syme);
		break;

	case SYME_Builtin:
		foam = foamNewBVal(symeBuiltin(syme));
		break;

	case SYME_Temp:
		foam = foamCopy(symeFoam(syme));
		break;

	default:
		bugUnimpl("syme kind in genFoam");
		NotReached(foam = 0);
	}

	if (symeKind(syme) != SYME_Temp && !foamSyme(foam)) 
		foamSyme(foam) = syme;

	symeSetUsed(syme);
	return foam;
}

local Syme
gen0SymeCopyImport(Syme syme)
{
	TForm	tf;

	assert(symeIsExport(syme) || symeIsExtend(syme));
	assert(symeLib(syme));

	tf = tfLibrary(libLibrarySyme(symeLib(syme)));
	syme = symeCopy(syme);
	symeSetKind(syme, SYME_Import);
	symeSetExporter(syme, tf);
	return syme;
}

local Foam
gen0SymeGeneric(Syme syme)
{
	FoamTag		kind = gen0FoamKind(syme);
	AInt		level;

	if (kind == FOAM_LIMIT) {
		if ((symeIsExport(syme) || symeIsExtend(syme)) && symeLib(syme))
			return gen0SymeImport(gen0SymeCopyImport(syme));

		/*!! Scobind needs to share param symes in cat forms */
		else if (symeIsParam(syme)) {
			kind = FOAM_Lex;
			gen0SetFoamKind(syme, kind);
			/*!! Assumes params are in order in symTab. */
			gen0SetVarIndex(syme, gen0ParamIndex(syme));
		}

		else if (symeIsLexVar(syme) && symeIsSelf(syme) &&
			 gen0IsDomLevel(gen0State->tag))
			return gen0LocalSelf();

		else {
			bug("gen0Syme:  syme unallocated by gen0Vars");
			NotReached(return NULL);
		}
	}

	if (kind != FOAM_Lex) {
		assert(kind != FOAM_Nil);
		return foamNew(kind, 1, (AInt) gen0VarIndex(syme));
	}

	genfDEBUG({
		printf("GenSyme: %s \t\tstablev: %lu stabLamLev:%lu symeDefLev: %lu symeDefLamLev: %lu ",
		symeString(syme),
		stabLevelNo(gen0State->stab), 
		stabLambdaLevelNo(gen0State->stab), 
		symeDefLevelNo(syme),
		symeDefLambdaLevelNo(syme));});

	if (!gen0State->stab)
		level = 0;
	else {
		level = stabLambdaLevelNo(gen0State->stab) -
			symeDefLambdaLevelNo(syme);
		if (gen0IsInnerVar(syme, level)) {
			genfDEBUG({printf("Inner\n");});
			return gen0InnerSyme(syme, level);
		}
	}

	level = gen0FoamLevel(symeDefLevelNo(syme));
	genfDEBUG({printf("std: Lev:%d\n", (int)level);});
	gen0UseStackedFormat(level);
	return foamNewLex(level, gen0VarIndex(syme));
}

local Foam
gen0SymeImport(Syme syme)
{
	AInt	symeLexLevel, level, env;
	int	idx;
	Foam	ref, foam;

	if (gen0IsEnumLit(syme)) {
		TForm	key = gen0SpecialKeyType(symeType(syme));
		return foamNewSInt(gen0EnumIndex(key, symeId(syme)));
	}

	symeLexLevel = gen0GetImportLexLevel(syme);
	level = gen0FoamImportLevel(symeLexLevel);
	gen0UseStackedFormat(level);
	if (!gen0GetImportedSyme(syme, level, false)) {
		if (symeUnused(syme)) stabUseMeaning(gen0State->stab, syme);
		gen0VarsImport(syme, gen0State->stab);
	}
	if (!symeImportInit(syme)) {
		gen0SetImportedSyme(syme, level);
		gen0InitImport(syme);
	}

	idx = (gen0State->whereNest) ? 
		gen0State->whereNest - 
			(symeLexLevel - gen0State->stabLevel) : 0;

	if (symeLexLevel < gen0State->stabLevel)
		idx = gen0State->whereNest;

	if (gen0NthState(level) == gen0State &&
	    gen0State->whereNest &&
	    idx != gen0State->whereNest) {
		env = listElt(AInt)(gen0State->envFormatStack, idx);
		ref = foamCopy(listElt(Foam)(gen0State->envVarStack, idx));
		foam = foamNewEElt(env, ref, int0, gen0VarIndex(syme));
	}
	else {
		assert(level >= 0);
		foam = foamNewLex(level, gen0VarIndex(syme));
	}

	foamSyme(foam) = syme;

	if (gen0IsLazyConst(symeType(syme)))
		foam = gen0LazyValue(foam, syme);

	return foam;
}

local Foam
gen0InnerSyme(Syme syme, AInt funLevel)
{
	GenFoamState	s	 = gen0NthState(funLevel);
	int		envLevel = symeDefLevelNo(syme) - stabLevelNo(s->stab);
	int		format = listElt(AInt)(s->envFormatStack,
					      s->whereNest-envLevel);
	Foam		env;

	gen0UseStackedFormat(funLevel);
	env = foamCopy(listElt(Foam)(s->envVarStack, s->whereNest-envLevel));
	if (foamTag(env)==FOAM_Env) {
		   env->foamLex.level = funLevel;
		   return foamNewEElt(format, env, int0, gen0VarIndex(syme));
	} else
		return foamNewEElt(format,env,int0, gen0VarIndex(syme));
}

/*
 * Generate a Foam character array from a string.
 */
Foam
gen0CharArray(String s)
{
	Length	len, i;
	Foam	foam;

	len  = strLength(s);
	foam = foamNewEmpty(FOAM_Arr, TypeSlot + len);

	foam->foamArr.baseType = FOAM_Char;

	for (i = 0; i < len; i++) foam->foamArr.eltv[i] = s[i];

	return foam;
}

/*
 * Generate Foam for a balanced if.
 */
local Foam
genIf(AbSyn absyn)
{
	Bool flag;
	FoamList topLines;
	int	l1 = gen0State->labelNo++, l2 = gen0State->labelNo++;
	/*
	 * We need to take embeddings into account so we use
	 * gen0AbContextType() instead of gen0AbType().
	 */
	Foam	t = gen0TempValueMode(gen0AbContextType(absyn));

	flag = gen0AddImportPlace(&topLines);
	
	gen0AddStmt(foamNewIf(genFoamBit(absyn->abIf.test), l1), absyn);
	gen0SetTemp(t, gen0TempValue(absyn->abIf.elseAlt));
	gen0AddStmt(foamNewGoto(l2), absyn);
	gen0AddStmt(foamNewLabel(l1), absyn);
	gen0SetTemp(t, gen0TempValue(absyn->abIf.thenAlt));
	gen0AddStmt(foamNewLabel(l2), absyn);

	if (flag) gen0ResetImportPlace(topLines);
	return t;

}

/*
 * Generate Foam for a Select.
 * The idea is to generate massively dull code, and
 * then use the optimiser to insert the relevant
 * clevernesses.
 */

local Foam gen0SelectCase(AbSyn test, AbSyn id);

local Foam
genSelect(AbSyn absyn)
{
	FoamList topLines;
	AbSyn seq;
	Syme  tmpSym;
	Foam key;
	Foam t;
	AInt kfmt, kt, argc;
	Bool flag;
	int  exitLabel, nextLabel;
	int  i;   

	seq = absyn->abSelect.alternatives;
	kt = gen0Type(gen0AbType(absyn->abSelect.testPart), &kfmt);
	key = gen0Temp0(kt, kfmt);
	gen0SetTemp(key, genFoamVal(absyn->abSelect.testPart));
	t   = gen0TempValueMode(gen0AbType(absyn));

	tmpSym = symeNewTemp(ssymTheCase,
			     gen0AbType(absyn->abSelect.testPart),
			     car(gen0State->stab));

	symeSetFoam(tmpSym, key);

	/* sb. temporary */
	exitLabel = gen0State->labelNo++;

	flag = gen0AddImportPlace(&topLines);

	argc = abArgc(seq);

	/* Generate code for each of the cases */
	for (i = 0; (i < argc) && abIsExit(seq->abSequence.argv[i]); i++) {
		AbSyn item = seq->abSequence.argv[i];
		AbSyn tmp;
		Foam  call;
		
		/* Allocate a label */
		nextLabel = gen0State->labelNo++;
		
		tmp = abFrSyme(tmpSym);
		call = gen0SelectCase(item->abExit.test, tmp);

		gen0AddStmt(foamNewIf(foamNotThis(foamNewCast(FOAM_Bool, call)),
				      nextLabel),
			    item);
		gen0SetTemp(t, gen0TempValue(item->abExit.value));
		gen0AddStmt(foamNewGoto(exitLabel), absyn);
		gen0AddStmt(foamNewLabel(nextLabel), absyn);
	}
	
	foamFree(key);


	/*
	 * Emit code for statements between the last case and the
	 * statement representing the default value of the select.
	 */
	for ( ; i < abArgc(seq) - 1; i++) 
		genFoamStmt(seq->abSequence.argv[i]);


	/* Emit code for the default value of the select if needed */
	if (i == abArgc(seq) - 1)
		gen0SetTemp(t, gen0TempValue(seq->abSequence.argv[i]));
	
	if (flag) gen0ResetImportPlace(topLines);

	gen0AddStmt(foamNewLabel(exitLabel), absyn);
	symeFree(tmpSym);
	return t;
}

local Foam
gen0SelectCase(AbSyn test, AbSyn id)
{
	AbSyn app;
	Foam  foam;

	assert(abTag(test) == AB_Test);
	app  = abNewApply2(sposNone, abImplicit(test), id, test->abTest.cond);
	
	foam =  genFoamVal(app);

	return foam;
}

/*
 * Generate Foam for a Sequence.
 */
local Foam
genSequence(AbSyn absyn)
{
	Length	i = 0, argc = abArgc(absyn);
	AbSyn	*argv = abArgv(absyn);

	assert(abTag(absyn) == AB_Sequence);
	return gen0Sequence(gen0AbType(absyn), argv, argc, i);
}

/*
 * Generate code for a sequence of definitions at the top-level of
 * a library (.ao file).
 */

void
gen0DefSequence(AbSyn absyn)
{
	Length	argc, i = 0;
	AbSyn	*argv;
	Bool	save = gen0ValueMode;

	if (abTag(absyn) == AB_Sequence) {
		argc = abArgc(absyn);
		argv = abArgv(absyn);
	}
	else {
		argc = 1;
		argv = &absyn;
	}

	gen0ValueMode = false;
	gen0Sequence(gen0AbType(absyn), argv, argc, i);
	gen0ValueMode = save;
}


/*
 * Generate sequence code, where there may be exits in the sequence.
 */

local Foam
gen0Sequence(TForm tf, AbSyn *argv, Length argc, Length i)
{
	FoamList	topLines; /* Initialised when flag == true */
	Length		j;
	Bool		flag = false;

	for (j = i; j < argc; j += 1) {
		AbSyn	s = argv[j];

		if (abTag(s) == AB_Exit) {
			AbSyn	test = s->abExit.test;
			AbSyn	val = s->abExit.value;
			Foam	t = gen0TempValueMode(tf);
			int	l1 = gen0State->labelNo++,
				l2 = gen0State->labelNo++;

			/* New-style debugging hook */
			if (gen0DebuggerWanted) gen1DbgFnStep(s);

			if (flag) gen0ResetImportPlace(topLines);
			flag = gen0AddImportPlace(&topLines);
			gen0AddStmt(foamNewIf(genFoamBit(test), l1), s);
			gen0SetTemp(t, gen0Sequence(tf, argv, argc, j + 1));
			gen0AddStmt(foamNewGoto(l2), s);
			gen0AddStmt(foamNewLabel(l1), s);
			gen0SetTemp(t, gen0TempValue(val));
			gen0AddStmt(foamNewLabel(l2), s);
			if (flag) gen0ResetImportPlace(topLines);
			return t;
		}

		if (j == argc - 1) {
			Foam	result = gen0TempValue(s);
			if (flag) gen0ResetImportPlace(topLines);
			return result;
		}

		genFoamStmt(s);
		if (gen0IsDef(s)) {
			if (flag) gen0ResetImportPlace(topLines);
			flag = gen0AddImportPlace(&topLines);
		}
	}

	assert(gen0ValueMode == false);
	return NULL;
}

/* Find forms to generate at the top of add bodies. */
local Bool
gen0IsDef(AbSyn absyn)
{
	return (abTag(absyn) == AB_Define);
}


/*
 * Generate Foam for a program.
 */
local Foam
gen0Lambda(AbSyn absyn, Syme syme, AbSyn defaults)
{
	AbSyn		fbody, params, ret;
	TForm		tf, tfret, retStmtType;
	Foam		foam, clos, val = NULL;
	RTCacheInfo	cache = NULL;
	FoamTag		retType;
	AInt		index, retfmt;
	Bool		isconst, packed = abHasTag(absyn, AB_PLambda);

	fbody  = absyn->abLambda.body;
	params = absyn->abLambda.param; 
	while (abTag(fbody) == AB_Label)
		fbody = fbody->abLabel.expr;

	tf = gen0AbType(absyn);
	assert(tfIsAnyMap(tf));

	tfret = tfMapRet(tf);
/*	printf("BDS: entered gen0Lambda\n"); */
	if (packed && !tfIsMulti(tfret)) tfret = tfRawType(tfret);
/*	printf("BDS: done in gen0Lambda\n"); */
	retType = gen0Type(tfret, &retfmt);

	if (gen0IsResultCachable(fbody, tf))
		cache = gen0CacheMakeEmpty(fbody);

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(gen0ProgName, absyn);

	index = gen0FormatNum;
	gen0ProgPushState(abStab(absyn), GF_Lambda);

	if (tfIsMulti(tfret))
		retfmt = gen0MultiFormatNumber(tfret);

	if (tfIsGenerator(tfret))
		foamProgSetGenerator(foam);

	gen0State->type = tf;
	gen0State->param = params;
	gen0State->program = foam;

	gen0PushFormat(index);

	gen0Vars(abStab(absyn));

	if (gen0ProgGetExporter())
		gen0ProgAddExporterArgs(params);


	/* Debugger hooks enabled with -Wdebug  */
	if (gen0DebugWanted) gen0DbgFnEntry(absyn);


	/* Debugger hooks enabled with -Wdebugger */
	if (gen0DebuggerWanted) gen1DbgFnEntry(tf, syme, absyn);


	/* Domain caching */
	if (cache) {
		int  i;
		Foam *paramv = (Foam*)stoAlloc(OB_Other, abArgc(params)*sizeof(Foam));
		for (i=0; i<abArgc(params); i++) 
			paramv[i] = genFoamVal(abDefineeId(params->abComma.argv[i]));
		gen0CacheCheck(cache, abArgc(params), paramv);
		stoFree(paramv);
	}
	
	if (abIsAnyLambda(fbody)) {
		if (defaults) defaults = abMapRet(defaults);
		/*
		 * We used to pass the syme down to gen0Lambda() but
		 * this causes problems. The trouble is that given the
		 * curried domain Dom(A)(B), we store the hash code for
		 * Dom in the closure for Dom(A). Also, when we compute
		 * the hash code, we can't see A, only A -> B -> ???
		 * and B -> ???.
		 *
		 * Note that this temporary fix forces us to construct
		 * Dom(A)(B) and apply domainHash!() at runtime when we
		 * could have computed it from Dom(A).
		 *
		 * See bugs 1072 and 892 for more details.
		 */
#if AXL_EDIT_1_1_12p6_17
		val = gen0Lambda(fbody, (Syme)NULL, defaults);
#else
		val = gen0Lambda(fbody, syme, defaults);
#endif
		assert(foamTag(val) != FOAM_Nil);
	}

	else if (abTag(fbody) == AB_Add) {
		if (genIsRuntime() && syme) {
			gen0Vars(abStab(fbody));
			genFoamStmt(fbody->abAdd.capsule);
		}
		else {
#if EDIT_1_0_n1_06
			val = gen0AddBody0(fbody, abStab(fbody), defaults);
#else
			val = gen0AddBody0(fbody->abAdd.base,
					   fbody->abAdd.capsule,
					   abStab(fbody), defaults);
#endif
			assert(foamTag(val) != FOAM_Nil);
		}
	}
	else if (tfIsCategoryMap(tf)) {
		foamProgSetGetter(gen0State->program);
		val = gen0MakeDefaultPackage(fbody, abStab(fbody), true, syme);
		assert(foamTag(val) != FOAM_Nil);
	}
	else if (tfMapRetc(tf) == 0) {
		genFoamStmt(fbody);
	}
	else {
		if (gen0DebuggerWanted)
			val = gen1DbgFnBody(fbody);
		else
			val = genFoamVal(fbody);
		if (foamTag(val) == FOAM_Nil && gen0ProgHasReturn())
			val = NULL;
	}

	if (tfSatDom(tfret) && gen0AbSynHasConstHash(fbody)) 
		foamProgSetGetter(gen0State->program);

	if (val && cache) val = gen0CacheReturn(cache, val);

	if (gen0ProgGetExporter())
		gen0ProgPopExporterArgs();

	ret = fbody;
	while (abTag(ret) == AB_Sequence && abArgc(ret) > 0)
		ret = ret->abSequence.argv[abArgc(ret) - 1];
	retStmtType = gen0AbContextType(ret);

	if (val && !tfEqual(retStmtType, tfret)) {
		AbEmbed embed = tfSatEmbedType(retStmtType, tfret);
		val = gen0Embed(val, ret, retStmtType, embed);
	}

	/* Debugger hooks enabled with -Wdebug */
	if (gen0DebugWanted) val = gen0DbgFnExit(absyn, val);


	/* Debugger hooks enabled with -Wdebugger. */
	if (gen0DebuggerWanted)
	{
		if (val)
			gen1DbgFnReturn(fbody, retStmtType, val);
		else
			gen1DbgFnExit(fbody);
	}


	if (!val && !gen0ProgHasReturn()) {
		if (tfMapRetc(tf) == 0)
			val = foamNewEmpty(FOAM_Values, int0);
		else
			val = foamNewNil();
	}
	if (val) gen0AddStmt(foamNewReturn(val), absyn);

	gen0ProgAddFormat(index);
	gen0IssueDCache();
	gen0ProgFiniEmpty(foam, retType, retfmt);

	/* foam->foamProg.infoBits = IB_SIDE | IB_LEAF; */
	foamOptInfo(foam) = inlInfoNew(gen0State->stab, foam,
				       syme, false);
	if (foam->foamProg.levels->foamDEnv.argv[0] != emptyFormatSlot)
		foamProgUnsetLeaf(foam);
	gen0ComputeSideEffects(foam);
	gen0ProgPopState();
	
	if (cache) {
		if (gen0IsDomainInit(clos))
			gen0AddInit(cache->init);
		else
			gen0AddStmt(cache->init, cache->ab);
		gen0CacheKill(cache);
	}
	
	isconst = gen0AbSynHasConstHash(fbody);
	if (syme && isconst) {
#if AXL_EDIT_1_1_12p6_24
		AInt hashCode = strHash(symeString(syme));
		symeSetHashNum(syme, hashCode);
		/* DO NOT use gen0RtSetProgHash() here */
#else
		AInt hashCode = strHash(symeString(syme));
		symeSetHashNum(syme, hashCode);
		gen0AddStmt(gen0RtSetProgHash(foamCopy(clos), hashCode), NULL);
#endif
	}

	/*
	 * Functions that return types need to have a constant
	 * hash code if they are going to be used safely. Warn
	 * the user about non-const type-returning functions.
	 */
	if (!isconst) {
		TForm tf = gen0AbType(fbody);

		if (tfSatDom(tf))
			comsgWarning(absyn, ALDOR_W_GenDomFunNotConst);
		else if (tfSatCat(tf))
			comsgWarning(absyn, ALDOR_W_GenCatFunNotConst);
	}
	return clos;
}

/*
 * Ensure that single-statement function bodies get step events.
 */
local Foam
gen1DbgFnBody(AbSyn fbody)
{
	/* Sequences get step events already. */
	if (abTag(fbody) != AB_Sequence)
	{
		gen1DbgFnStep(fbody);
		return genFoamVal(fbody);
	}
	else
		return genFoamVal(fbody);
}


local Foam
genReturn(AbSyn ab)
{
	AbSyn	val = ab->abReturn.value;
	Foam	ret;

	if (tfMapRetc(gen0State->type) == 0) {
		genFoamStmt(val);
		ret = foamNew(FOAM_Values, int0);
	}
	else {
		ret = genFoamVal(val);


		/* Old style debugger hook */
		if (gen0DebugWanted) 
			ret = gen0DbgFnReturn(ab, ret);


		/* New style debugger hook */
		if (gen0DebuggerWanted) 
			gen1DbgFnReturn(ab, tfMapRet(gen0State->type), ret);
	}

	gen0AddStmt(foamNewReturn(ret), ab);
	return NULL;
}

local Bool
gen0AbSynHasConstHash(AbSyn ab)
{
	TForm tf = gen0AbType(ab);

	if (tfSatDom(tf) || tfSatCat(tf))
		return gen0AbSynHasConstHash0(ab);
	return false;
}


local Bool
gen0AbSynHasConstHash0(AbSyn ab)
{
	TForm tf;
	AbSyn abi;
	int i;
	
	switch (abTag(ab)) {
	  case AB_Sequence:
		for (i = 0; i<abArgc(ab); i++) {
			abi = ab->abSequence.argv[i];
			if (abTag(abi) == AB_Exit) {
				tf = gen0AbType(abi);
				/* !! Multiple? */
				if (!tfIsExit(tf) && !tfIsMulti(tf))
					return false;
			}
		}
		return gen0AbSynHasConstHash(ab->abSequence.argv[abArgc(ab)-1]);
		break;
	  case AB_If:
		assert(ab->abIf.elseAlt);
		return gen0AbSynHasConstHash(ab->abIf.thenAlt)
			&& gen0AbSynHasConstHash(ab->abIf.elseAlt);
		break;
	  case AB_Add:
	  case AB_With:
		return true;
		break;
		/* AB_Repeat is a possibility, but not a very useful one */
	  case AB_Apply:
		return abIsJoin(ab);
		break;
	  case AB_Where:
		return gen0AbSynHasConstHash(ab->abWhere.expr);
	  case AB_Define:
		/*
		 * We ought to check that the RHS is a singleton and
		 * then pass the result to gen0AbSynHasConstHash().
		 */
		return false;
	  case AB_Id:
		/*
		 * We ought to check to see if the identifier is a
		 * parameter that is not used as a local and treat
		 * it as having a constant hash. Not only that but
		 * we ought to trace identifiers back through local
		 * definitions to check that their value has a const
		 * hash. I've no idea how to do this so users will
		 * just have to use with, add or Join to get domains
		 * and categories with constant hash.
		 */
		return false;
	  default:
		return false;
	}
}


/*
 * Create an empty format usage stack of the same length as l.
 */
local AIntList
gen0UnusedFormats(AIntList l)
{
	AIntList r = listNil(AInt);
	for(; l != listNil(AInt); l = cdr(l))
		r = listCons(AInt)(emptyFormatSlot, r);
	return r;
}

/*
 * Return the Foam type given a type form.
 */

FoamTag
gen0Type(TForm tf, AInt *pfmt)
{
	Symbol	sym;
	FoamTag	tag;
	int	pass;
	Bool	done = false;
	AInt	fmt = emptyFormatSlot;

#if EDIT_1_0_n1_07
	tag = FOAM_Word; /* Default */
#endif

	for (pass = 0;(pass < 2) && !done; pass++)
	{
		tf = pass ? tfDefineeBaseType(tf) : tfDefineeType(tf);
		if (tfIsRaw(tf)) tf = tfRawType(tfRawArg(tf));

		done = true;

		if	(tfIsWith(tf))		tag = FOAM_Word;
		else if (tfIsAnyMap(tf))	tag = FOAM_Clos;
		else if (tfIsGenerator(tf))	tag = FOAM_Clos;
		else if (tfIsMulti(tf))		tag = FOAM_NOp;
		else if (tfIsRecord(tf)) {
			tag = FOAM_Rec;
			fmt = gen0RecordFormatNumber(tf);
		}
		else if (tfIsTuple(tf)) {
			tag = FOAM_Word;
		}
		else if (tfIsCross(tf)) {
			tag = FOAM_Word;
		}
		else if (tfIsEnum(tf)) {
			tag = FOAM_SInt;
 		}
		else if (tfIsUnknown(tf))	tag = FOAM_Word;
 		else if ((sym = gen0MachineType(tf)) == NULL)
			tag = FOAM_Word;
	
		else if (sym == ssymBool)	tag = FOAM_Bool;
		else if (sym == ssymByte)	tag = FOAM_Byte;
		else if (sym == ssymHInt)	tag = FOAM_HInt;
		else if (sym == ssymSInt)	tag = FOAM_SInt;
		else if (sym == ssymBInt)	tag = FOAM_BInt;
		else if (sym == ssymChar)	tag = FOAM_Char;
		else if (sym == ssymSFlo)	tag = FOAM_SFlo;
		else if (sym == ssymDFlo)	tag = FOAM_DFlo;
		else if (sym == ssymNil)	tag = FOAM_Nil;
		else if (sym == ssymPtr)	tag = FOAM_Ptr;
		else if (sym == ssymArr) {
			tag = FOAM_Arr;
			/* Something of a bug here as we don't know
			   the element type for the array. Use 0
			   rather than emptyFormatSlot as that is
			   FOAM_HInt */
			fmt = 0;
		}
		else {
			/* Try a second time using the normalised type */
			done = false;
		}

	/*!! assert(tag == tfFoamType(tf)); */
#if 0
	if (tag != tfFoamType(tf)) {
		if (tag == FOAM_Word && tfFoamType(tf) == FOAM_LIMIT)
			/*!! Probably OK. */;
		else if (tfFoamType(tf) == FOAM_LIMIT) {
			fprintf(dbOut, "tf w/o foam type\n");
			tformPrintDb(tf);
			fnewline(dbOut);
		}
		else {
			fprintf(dbOut, "tf w/ different foam type\n");
			fprintf(dbOut, "tag = %d, foamType = %d\n",
				tag, tfFoamType(tf));
			tformPrintDb(tf);
			fnewline(dbOut);
		}
	}
#endif
	}

#if EDIT_1_0_n1_07
#else
	if (!done) tag = FOAM_Word;
#endif
	if (pfmt) *pfmt = fmt;

	return tag;
}

local Symbol
gen0MachineType(TForm tf)
{
	AbSyn 	absyn;
	Syme	syme;

	if (tf == NULL) return NULL;
	if (!tfHasExpr(tf)) return NULL;

	absyn = gen0EqualMods(tfGetExpr(tf));
	
	syme = abSyme(absyn);
	if (syme == NULL) return NULL;

	switch (symeKind(syme)) {
	case SYME_Export:
		tf = tfDefineeType(symeType(syme));
		if (tfIsType(tf))
			return symeId(syme);
		break;
	case SYME_Import:
		tf = tfDefineeType(symeExporter(syme));
		if (tfIsTheId(tf, ssymMachine))
			return symeId(syme);
		if (tfIsTheId(tf, ssymBasic))
			return symeId(syme);
		break;
	default:
		break;
	}
	return NULL;
}


local Bool
gen0IsResultCachable(AbSyn body, TForm tf)
{
	if (gen0AbSynHasConstHash(body))
		return true;
	
	return false;
}

/*
 * Extract the declarations for lexicals, locals, globals and parameters
 * from the list of symes in a function body.
 */

void
gen0Vars(Stab stab)
{
	SymeList	symes;
	AbSynList	labels, la;
	
	if (!stab)
		return;

	labels = stabGetLevelLabels(stab);

	for (la = labels; la; la = cdr(la)) {
		Syme	syme = abSyme(car(la));
		if (symeDVMark(syme) > 0)
			gen0SetVarIndex(syme, gen0State->labelNo++);
	}

	symes = gen0GetBoundSymes(stab);
	gen0VarsList(stab, symes);
	listFree(Syme)(symes);
}

local void
gen0VarsList(Stab stab, SymeList symes)
{
	for (; symes; symes = cdr(symes)) {
		Syme	syme = car(symes);

		switch (symeKind(syme)) {
		case SYME_Param:
			gen0VarsParam(syme, stab);
			break;
		case SYME_LexVar:
		case SYME_LexConst:
			gen0VarsLex(syme, stab);
			break;
		case SYME_Fluid:
			gen0VarsFluid(syme);
			break;
		case SYME_Import:
			gen0VarsImport(syme, stab);
			break;
		case SYME_Extend:
		case SYME_Export:
			gen0VarsExport(syme, stab);
			break;
		case SYME_Foreign:
			gen0VarsForeign(syme);
			break;
		case SYME_Builtin:
		case SYME_Library:
		case SYME_Archive:
			break;
		default:
			bugBadCase(symeKind(syme));
			break;
		}
	}
}

local AbSyn
gen0PLambdaParam(Syme syme)
{
	AbSyn	param = gen0State->param;
	AbSyn	*argv;
	Length	i, argc;

	/* printf("BDS: Entered gen0PLambdaParam\n"); */

	switch (abTag(param)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Comma:
		argc = abArgc(param);
		argv = abArgv(param);
		break;
	default:
		argc = 1;
		argv = &param;
		break;
	}

	for (i = 0; i < argc; i += 1) {
		AbSyn	argi = abDefineeId(argv[i]);
		if (syme == abSyme(argi))
			return argi;
	}

	assert(false);
	return NULL;
}

local void
gen0VarsParam(Syme syme, Stab stab)
{
	TForm		tf = symeType(syme);
	String		name = strCopy(symeString(syme));
	FoamTag		type;
	Foam		decl;
	AInt		index;
	FoamTag		kind;
	AInt		fmtSlot = emptyFormatSlot;
	Bool		packed = tfIsPackedMap(gen0State->type);

	assert(symeIsParam(syme));

/*
	printf("BDS: gen0VarsParam: name is '%s'\n",name);
	printf("BDS: gen0VarsParam: packed is '%d'\n",packed);
*/

/*	printf("BDS: entered gen0VarsParam\n"); */
	type = packed ? gen0Type(tfRawType(tf), NULL) : gen0Type(tf, &fmtSlot);
/*	printf("BDS: done in gen0VarsParam\n"); */

	decl = foamNewDecl(type, name, fmtSlot);
	index = gen0AddParam(decl);
	kind = FOAM_Par;

	if (packed) {
		FoamTag	ntype = gen0Type(tf, NULL);
		Foam	ndecl = foamNewDecl(ntype, strCopy(name), fmtSlot);
		Foam	par = foamNewPar(index);
		AbSyn	id = gen0PLambdaParam(syme);

		/* printf("BDS: gen0VarsParam: packed: name is '%s'\n",name); */

		index = gen0AddLex(ndecl);
		kind = FOAM_Lex;

		par = gen0RawToUnary(par, id);
		gen0AddStmt(foamNewSet(foamNewLex(int0, index), par), NULL);
		if (gen0State->program)
			foamProgUnsetLeaf(gen0State->program);
		gen0State->hasTemps = true;
	}

	else if (symeUsedDeeply(syme)) {
		Foam	par = foamNewPar(index);

		index = gen0AddLex(foamCopy(decl));
		kind = FOAM_Lex;

		gen0AddInit(foamNewSet(foamNewLex(int0, index), par));
		if (gen0State->program)
			foamProgUnsetLeaf(gen0State->program);
		gen0State->hasTemps = true;
	}

	if (gen0InGener(gen0State->progType))
		foamFixed(decl) = true;

	gen0SetVarIndex(syme, index);
	gen0SetFoamKind(syme, kind);
}

local void
gen0VarsLex(Syme syme, Stab stab)
{
	FoamTag		type;
	AInt 		fmt;
	String		name = strCopy(symeString(syme));
	Foam		decl;
	AInt		index;
	FoamTag		kind;
	int		fmtSlot = emptyFormatSlot;
	Bool 		isGlobal = false;

	type = gen0Type(symeType(syme), &fmt);
	assert(symeIsLexVar(syme) || symeIsLexConst(syme));

	if (type == FOAM_Rec) fmtSlot = fmt;

	decl = foamNewDecl(type, name, fmtSlot);

	if (fintMode == FINT_LOOP &&
	    gen0State->tag == GF_File && stabLevelNo(stab) == 1) {
		decl = foamNewGDecl(type, name, fmtSlot,
				    FOAM_GDecl_Export, FOAM_Proto_Foam);
		decl->foamDecl.id = gen0GlobalName(gen0FileName, syme);
		isGlobal = true;
	}
	else 
		decl = foamNewDecl(type, name, fmtSlot);

	if (isGlobal) {
		index = gen0AddGlobal(decl);
		kind = FOAM_Glo;
	}
	else if (symeUsedDeeply(syme)) {
		index = gen0AddLex(decl);
		kind = FOAM_Lex;
	}
	else {
		index = gen0AddLocal(decl);
		kind = FOAM_Loc;
	}

	if (gen0InGener(gen0State->progType))
		foamFixed(decl) = true;

	gen0SetVarIndex(syme, index);
	gen0SetFoamKind(syme, kind);
}

local void
gen0VarsFluid(Syme syme)
{
	FoamTag		type = gen0Type(symeType(syme), NULL);
	String		name = strCopy(symeString(syme));
	Foam		decl;
	AInt		index;
	int		fmtSlot = emptyFormatSlot;

	assert(symeIsFluid(syme));

	if (type == FOAM_Rec)	
		fmtSlot = gen0RecordFormatNumber(symeType(syme));

	decl = foamNewDecl(type, name, fmtSlot);
	index = gen0FindGlobalFluid(syme);

	gen0SetVarIndex(syme, index);
	gen0SetFoamKind(syme, FOAM_Fluid);
}

local void
gen0VarsImport(Syme syme, Stab stab)
{
	String		name;
	TForm		tf;
	FoamTag 	type;
	Foam		decl;
	AInt		index, sl, l;
	int		fmtSlot = emptyFormatSlot;
	Sefo		sefo;
	SymeList	symes;

	assert(symeIsImport(syme));
	if (symeUnused(syme) || gen0IsEnumLit(syme)) return;

	sl = gen0GetImportLexLevel(syme);
	l = gen0FoamImportLevel(sl);
	if (gen0GetImportedSyme(syme, l, true)) return;

	name = strCopy(symeString(syme));
	tf = symeType(syme);

	type = gen0IsLazyConst(tf) ? FOAM_Word : gen0Type(tf, NULL);
	if (type == FOAM_Rec) fmtSlot = gen0RecordFormatNumber(tf);

	decl = foamNewDecl(type, name, fmtSlot);
	if (!otSymeIsFoamConst(syme))
		foamSyme(decl) = syme;

	index = gen0AddLexNth(decl, l, gen0StateOffset(sl, l));

	gen0SetVarIndex(syme, index);
	gen0SetFoamKind(syme, FOAM_Lex);

	sefo = tfExpr(symeExporter(syme));
	symes = gen0GetExporterSymes(stab, sefo, listNil(Syme));
	gen0VarsList(stab, symes);
	listFree(Syme)(symes);
}

local void
gen0VarsExport(Syme syme, Stab stab)
{
	TForm		tf = symeType(syme);
	FoamTag		type = gen0Type(tf, NULL);
	String		name = strCopy(symeString(syme));
	Foam		decl;
	AInt		index;
	FoamTag		kind;
	int		fmtSlot = emptyFormatSlot;

	assert(symeIsExport(syme) || symeIsExtend(syme));

	if (symeDefLevel(syme) != car(stab))
		return;

	if (symeIsSelf(syme) && gen0DebugWanted &&
	    symeUnused(syme)) {
		stabUseMeaning(stab, syme);
		symeSetUsed(syme);
	}

	if (type == FOAM_Rec)	
		fmtSlot = gen0RecordFormatNumber(symeType(syme));

	if (gen0State->tag == GF_File && stabLevelNo(stab) == 1) {
		decl = foamNewGDecl(type, NULL, fmtSlot,
				    FOAM_GDecl_Export, FOAM_Proto_Foam);
		decl->foamGDecl.id = gen0GlobalName(gen0FileName, syme);
		index = gen0AddGlobal(decl);
		kind = FOAM_Glo;
	}
	else {
		decl = foamNewDecl(type, name, fmtSlot);
		index = gen0AddLex(decl);
		kind = FOAM_Lex;
		if (gen0State->tag == GF_Default ||
		    gen0State->tag == GF_DefaultCat)
			foamSyme(decl) = syme;
	}

	if (gen0InAxiomAx &&
	    (tfSatType(tf) || (tfIsMap(tf) && tfSatType(tfMapRet(tf)))))
		symeSetHashNum(syme, strHash(symeString(syme)));

	gen0SetVarIndex(syme, index);
	gen0SetFoamKind(syme, kind);
}

local void
gen0VarsForeign(Syme syme)
{
	TForm		tf      = symeType(syme);
	FoamTag		type    = gen0Type(tf, NULL), rtype;
	ForeignOrigin	forg    = symeForeign(syme);
	int		fmtSlot;
	Foam		decl;
	AInt		index;
	String		name;

 	if (symeUnused(syme)) return;

	if (tfIsMap(tf))
		rtype = gen0Type(tfMapRet(tf), NULL);
	else if (tfIsPackedMap(tf))
	{
/*		printf("BDS: Entered gen0VarsForeign\n"); */
		rtype = gen0Type(tfRawType(tfMapRet(tf)), NULL);
/*		printf("BDS: done in gen0VarsForeign\n"); */
	}
	else
		rtype = FOAM_Nil;

	assert(symeIsForeign(syme));

	if (forg->file)
		name = strPrintf("%s-%s", symeString(syme), forg->file);
	else
		name = strCopy(symeString(syme));

	if (type == FOAM_Rec)	
		fmtSlot = gen0RecordFormatNumber(tf);
#if AXL_EDIT_1_1_12p6_22
	else if (tfIsMap(tf) && (forg->protocol == FOAM_Proto_C))
	        fmtSlot = gen0CSigFormatNumber(tf);
#endif
	else if (forg->protocol == FOAM_Proto_Fortran)
	        fmtSlot = gen0FortranSigFormatNumber(tf, true);
	else if (tfIsPackedMap(tf))
	{
/*		printf("BDS: gen0VarsForeign landed in new else if case for Packed Map\n"); */
		if (forg->protocol == FOAM_Proto_C)
		{
/*			printf("BDS:     Landed in FOAM_Proto_C\n"); */
	        	fmtSlot = gen0CPackedSigFormatNumber(tf);
		}
		else
		{
/*			printf("BDS:     Landed in some other protocol\n"); */
			fmtSlot = emptyFormatSlot;
		}
	}
	else
		fmtSlot = emptyFormatSlot;

	decl = foamNewGDecl(type, name, fmtSlot,
			    FOAM_GDecl_Import, forg->protocol);
	foamGDeclSetRType(decl, rtype);

	index = gen0AddGlobal(decl);

	gen0SetVarIndex(syme, index);
	gen0SetFoamKind(syme, FOAM_Glo);
}

/* gen0Vars could build a list..*/
local void
gen0GetGlobalDefs()
{
	Stab		stab = gen0State->stab;
	SymeList	ls, symes;
	Foam		foam;

	symes = gen0GetBoundSymes(stab);

	for (ls = symes; ls ; ls = cdr(ls)) {
		Syme	syme = car(ls);

		if (symeIsExport(syme) || symeIsExtend(syme)) {
			FoamTag type = gen0Type(symeType(syme), NULL);
			assert(gen0FoamKind(syme) == FOAM_Glo);
			foam = foamNewCast(type, foamNewNil());

			foam = foamNewDef(foamNewGlo(gen0VarIndex(syme)),foam);
			gen0ProgList = listCons(Foam)(foam, gen0ProgList);
			gen0NumProgs++;	
		}
	}

	listFree(Syme)(symes);
}

local SymeList
gen0GetExporterSymes(Stab stab, Sefo sefo, SymeList symes)
{
	Syme	syme = abSyme(sefo);

	if (syme && gen0FoamKind(syme) == FOAM_LIMIT) {
		if ((symeIsExtend(syme) || symeIsExport(syme)) &&
		    symeLib(syme))
			syme = gen0SymeCopyImport(syme);

		if (symeIsImport(syme) && !gen0IsEnumLit(syme)) {
			stabUseMeaning(stab, syme);
			if (!listMemq(Syme)(symes, syme))
				symes = listCons(Syme)(syme, symes);
		}
	}

	else if (!abIsLeaf(sefo)) {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1) {
			Sefo	argi = abArgv(sefo)[i];
			symes = gen0GetExporterSymes(stab, argi, symes);
		}
	}

	return symes;
}

local SymeList
gen0GetBoundSymes(Stab stab)
{
	SymeList	ls, symes;

	symes = listReverse(Syme)(stabGetBoundSymes(stab));

	for (ls = symes; ls; ls = cdr(ls)) {
		Syme	syme = car(ls);

		if (symeIsExtend(syme) && !symeLib(syme)) {
			SymeList	xsymes = symeExtendee(syme);
			for (; xsymes; xsymes = cdr(xsymes)) {
				Syme	xsyme = car(xsymes);
				if (!listMemq(Syme)(symes, xsyme))
					symes = listCons(Syme)(xsyme, symes);
			}
		}
	}

	return symes;
}

/*
 * Return the inner most lexical level used by the type of syme.
 */

static SefoList *gen0MaxLevList;
static int	gen0MaxLevLevel;

int
gen0GetImportLexLevel(Syme syme)
{
	assert (symeIsImport(syme));
	return gen0MaxLevel(tfExpr(symeExporter(syme)));
}

int 
gen0GetSefoLexLevel(Sefo sf)
{
	return gen0MaxLevel(sf);
}

SefoList
gen0GetSefoInnerSefos(Sefo sf)
{
	SefoList sl = listNil(Sefo);

	gen0MaxLevList  = &sl;
	gen0MaxLevLevel = gen0State->stabLevel;
	gen0MaxLevel(sf);
	gen0MaxLevList  = NULL;
	return sl;
}

/* Initializing max = 1 prevents imports from floating out too far. */
local int
gen0MaxLevel(AbSyn ab)
{
	int	i, level, max = 1;

	switch (abTag(ab)) {
	case AB_Id:
	case AB_LitInteger:
	case AB_LitFloat:
	case AB_LitString:
	{
		Syme	syme = abSyme(ab);
		if (!syme) break;
		while (symeExtension(syme)) syme = symeExtension(syme);
		if (symeLib(syme) && (symeIsExport(syme) ||
				      symeIsExtend(syme)))
			level = 1;
		else if (symeIsImport(syme)
			 && tfIsLibrary(symeExporter(syme)))
			level = 1;
		else {
			level = symeDefLevelNo(syme);
			if (symeIsExtend(syme) && !symeIsSelf(syme)) 
				level += 1;
		}
		if (level > max) max = level;
		if (gen0MaxLevList && level >= gen0MaxLevLevel)
			*gen0MaxLevList = listCons(Sefo)(ab, *gen0MaxLevList);
		break;
	}
	case AB_Comma:
	case AB_Apply:
		for (i = 0; i < abArgc(ab); i += 1) {
			level = gen0MaxLevel(abArgv(ab)[i]);
			if (level > max) max = level;
		}
		break;
	case AB_Declare:
		level = gen0MaxLevel(ab->abDeclare.type);
		if (level > max) max = level;
		break;
	case AB_PretendTo:
		level = gen0MaxLevel(ab->abPretendTo.expr);
		if (level > max) max = level;
		break;
	default:
		/* If we see something unrecognized, use the current level. */
		/*!! gen0State->stab may not be quite right here. */
		level = stabLevelNo(gen0State->stab);
		if (level > max) max = level;
		if (gen0MaxLevList && level >= gen0MaxLevLevel)
			*gen0MaxLevList = listCons(Sefo)(ab, *gen0MaxLevList);
		break;
	}
	return max;
}

/*
 * Turn the list of constant decls into a DDecl.
 */
local Foam
gen0RenewConstants(FoamList decls, int numProgs)
{
	int	i;
	Foam	 newConstants;

	newConstants  = foamNewEmpty(FOAM_DDecl, numProgs + 1);
	newConstants->foamDDecl.usage = FOAM_DDecl_Consts;

	for (i = numProgs-1; i >= 0; i--, decls = cdr(decls))
		newConstants->foamDDecl.argv[i] = car(decls);

	return newConstants;
}

/*
 * Turn list of programs into a DDef.
 */
local Foam
gen0RenewDefs(FoamList progs, int numProgs)
{
	int	i;
	Foam	 newDefs;

	newDefs	 = foamNewEmpty(FOAM_DDef, numProgs);

	for (i = numProgs-1; i >= 0; i--, progs = cdr(progs)) {
		if (foamTag(car(progs)) == FOAM_Prog) {
			newDefs->foamDDef.argv[i] =
				foamNewDef(foamNewConst(i), car(progs));
			gen0PatchFormatNums(car(progs));
		} else
			newDefs->foamDDef.argv[i] = car(progs);
	}
	return newDefs;
}

local Foam
genOr(AbSyn absyn)
{
	int	i, l1 = gen0State->labelNo++, l2 = gen0State->labelNo++;
	Foam	t;

	t = gen0Temp(FOAM_Bool);
	gen0AddStmt(foamNewSet(foamCopy(t), foamNewBool(false)), absyn);
	for (i = 0; i < abArgc(absyn); i += 1)
		gen0AddStmt(foamNewIf(genFoamBit(abArgv(absyn)[i]),l1),absyn);
	gen0AddStmt(foamNewGoto(l2), absyn);
	gen0AddStmt(foamNewLabel(l1), absyn);
	gen0AddStmt(foamNewSet(foamCopy(t), foamNewBool(true)), absyn);
	gen0AddStmt(foamNewLabel(l2), absyn);
	return foamNewCast(FOAM_Word, t);
}

local Foam
genAnd(AbSyn absyn)
{
	int	i, l1 = gen0State->labelNo++, l2 = gen0State->labelNo++;
	Foam	t;

	t = gen0Temp(FOAM_Bool);
	gen0AddStmt(foamNewSet(foamCopy(t), foamNewBool(true)), absyn);
	for (i = 0; i < abArgc(absyn); i += 1) {
		Foam	test = genFoamBit(abArgv(absyn)[i]);
		gen0AddStmt(foamNewIf(foamNotThis(test),l1), absyn);
	}
	gen0AddStmt(foamNewGoto(l2), absyn);
	gen0AddStmt(foamNewLabel(l1), absyn);
	gen0AddStmt(foamNewSet(foamCopy(t), foamNewBool(false)), absyn);
	gen0AddStmt(foamNewLabel(l2), absyn);
	return foamNewCast(FOAM_Word, t);
}

/*
 * Generate code for repeat loops.
 */
local Foam
genRepeat(AbSyn absyn)
{
	Scope("genRepeat");
	FoamList 	topLines;
	FoamList	itl = listNil(Foam), forl = listNil(Foam), l;
	int		gen0BodyLabel;
	int		fluid(gen0IterateLabel);
	int		fluid(gen0BreakLabel);
	int		iterSize, i;
	Bool flag;

	if (abStab(absyn))
		gen0Vars(abStab(absyn)); /* put vars in outer level */

	gen0BodyLabel    = gen0State->labelNo++;
	gen0IterateLabel = gen0State->labelNo++;
	gen0BreakLabel	 = gen0State->labelNo++;

	flag = gen0AddImportPlace(&topLines);
	
	/* Generate the code for the iterators, but do not add it yet. */
	iterSize = abRepeatIterc(absyn);
	for (i = 0; i < iterSize; i++)
		gen0Iter(absyn->abRepeat.iterv[i], &forl, &itl);

	l = forl = listNReverse(Foam)(forl);

	/* Add for-iterator initializations. */
	iterSize = listLength(Foam)(forl);
	for(i = 0; forl != listNil(Foam); i++, forl = cdr(forl))
		gen0AddStmt(car(forl), absyn);
	listFree(Foam)(l);

	/* Add goto the iterator steppers at the end of the loop. */
	gen0AddStmt(foamNewGoto(gen0IterateLabel), absyn);

	/* Add the body. */
	gen0AddStmt(foamNewLabel(gen0BodyLabel), absyn);
	genFoamStmt(absyn->abRepeat.body);

	/* Add the iterator steppers. */
	gen0AddStmt(foamNewLabel(gen0IterateLabel), absyn);

	l = itl = listNReverse(Foam)(itl);
	iterSize = listLength(Foam)(itl);

	for(i = 0; i<iterSize; i++, itl = cdr(itl)) {
		gen0AddStmt(car(itl), absyn);
	}
	listFree(Foam)(l);

	/* Add goto the body. */
	gen0AddStmt(foamNewGoto(gen0BodyLabel), absyn);

	/* Add the break label. */
	gen0AddStmt(foamNewLabel(gen0BreakLabel), absyn);


	if (flag) gen0ResetImportPlace(topLines);
	
	Return(NULL);
}

/*
 * Generate an exit test and for-loop initialization for a single iterator.
 */
local void
gen0Iter(AbSyn absyn, FoamList *forl, FoamList *itl)
{
	FoamList	olines = gen0State->lines;
	Foam		test;

	switch (abTag(absyn)) {
	case AB_While:
		gen0State->lines = *itl;
		test = foamNotThis(genFoamBit(absyn->abWhile.test));
		test = foamNewIf(test, gen0BreakLabel);
		*itl = listCons(Foam)(test, gen0State->lines);
		break;
	case AB_For:
		gen0ForIter(absyn, forl, itl);
		*itl = gen0State->lines;
		break;
	default:
		bugUnimpl("Unimplemented iterator");
		break;
	}

	gen0State->lines = olines;
	return;
}

/*
 * Generate an exit test and initialization code for a single for-iterator.
 */
local void
gen0ForIter(AbSyn absyn, FoamList *forl, FoamList *itl)
{
	AbSyn id;
	Foam iterVars, doneFun, stepFun, valueFun, boundFun;
	Foam call;

	gen0State->lines = *forl;
	iterVars = gen0GenLiftedGener(absyn, absyn->abFor.whole);
	*forl = gen0State->lines;
	gen0State->lines = *itl;

	assert(foamTag(iterVars) == FOAM_Values);
	doneFun = iterVars->foamValues.argv[0];
	stepFun = iterVars->foamValues.argv[1];
        valueFun = iterVars->foamValues.argv[2];
	boundFun = iterVars->foamValues.argv[3];

	/* step the generator */
	call = foamNewEmpty(FOAM_CCall, 2);
	call->foamCCall.type = FOAM_NOp;
	call->foamCCall.op   = foamCopy(stepFun);
	gen0AddStmt(call, absyn);

	/* Have we finished */	
	call = foamNewEmpty(FOAM_CCall, 2);
	call->foamCCall.type = FOAM_Word;
	call->foamCCall.op   = foamCopy(doneFun);
	gen0AddStmt(foamNewIf(foamNewCast(FOAM_Bool, call), gen0BreakLabel),
		    absyn);

	/* Snarf the value...*/
        /*         -- PAB 
                   id = abDefineeId(absyn);
                   call = foamNewEmpty(FOAM_CCall, 2);
                   call->foamCCall.type = gen0Type(gen0AbContextType(id), NULL);
                   call->foamCCall.op   = foamCopy(valueFun);
                   gen0AddStmt(foamNewSet(genFoamVal(id), call), absyn);
	*/
        if (abTag(absyn->abFor.lhs) == AB_Comma) {
                call = foamNewEmpty(FOAM_CCall, 2);
                call->foamCCall.type = FOAM_Rec;
                call->foamCCall.op   = foamCopy(valueFun);
                call = gen0CrossToMulti(call, abTUnique(absyn->abFor.lhs));
                gen0MultiAssign(FOAM_Set, absyn->abFor.lhs, call);
        }
        else {
                id = abDefineeId(absyn);
                call = foamNewEmpty(FOAM_CCall, 2);
                call->foamCCall.type = gen0Type(gen0AbContextType(id), NULL);
                call->foamCCall.op   = foamCopy(valueFun);
                gen0AddStmt(foamNewSet(genFoamVal(id), call), absyn);
        }

        if (!abIsNothing(absyn->abFor.test)) {
		int  l1     = gen0State->labelNo++;
		Foam test   = genFoamBit(absyn->abFor.test);
		gen0AddStmt(foamNewIf(test, l1), absyn);
		gen0AddStmt(foamNewGoto(gen0IterateLabel), absyn);
		gen0AddStmt(foamNewLabel(l1), absyn);
	}

	return;
}

/*****************************************************************************
 *
 * :: gen0FindDefs
 *
 ****************************************************************************/

/*
 * Find all the definitions in the absyn, and create foam closures
 * for defines of programs, also marks symes when used deeply.
 * This function can traverse the absyn in any order.
 */

/* 
 * !!should replace lhs with exporter
 */
local void gen0FindDefsSyme(Stab stab, Syme syme, Bool inHighLev);

local void
gen0FindDefs(AbSyn absyn, AbSyn lhs, Stab stab, Bool inHighLev, Bool topLev)
{
	Length		i, argc = abArgc(absyn);

	switch (abTag(absyn)) {
	case AB_Add: {
		AbSyn	lhs = absyn->abAdd.base;
		AbSyn	rhs = absyn->abAdd.capsule;

		stab = abStab(absyn);

		gen0FindDefs(lhs, NULL, stab, inHighLev, false);
		gen0FindDefs(rhs, NULL, stab, inHighLev, true);
		break;
	}
	case AB_Define: {
		AbSyn	lhs = absyn->abDefine.lhs;
		AbSyn	rhs = absyn->abDefine.rhs;
		AbSyn	id = abDefineeId(lhs);
		AbSyn	type = abDefineeTypeOrElse(lhs, NULL);

		gen0FindDefs(rhs, id, stab, inHighLev, false);

		if (!type) break;

		if (abIsAnyMap(type)) type = abMapRet(type);

		if (abTag(type) == AB_With &&
		    (abIsNotNothing(type->abWith.base) ||
		     gen0HasDefaults(type)))
			gen0FindDefs(type, NULL, stab, true, false);
		else if (tfIsCategoryType(gen0AbType(type)))
			gen0FindDefs(type, NULL, stab, true, false);
		else 
			gen0FindDefs(type, NULL, stab, inHighLev, topLev);

		break;
	}
	case AB_RestrictTo: {
		AbSyn	expr = absyn->abRestrictTo.expr;
		AbSyn	type = absyn->abRestrictTo.type;

		gen0FindDefs(expr, NULL, stab, inHighLev, topLev);
		if (abHasTag(expr, AB_Add) && tfIsCategory(gen0AbType(type)))
			gen0FindDefs(type, NULL, stab, true, topLev);
		break;
	}
	case AB_Generate: {
		AbSyn	count = absyn->abGenerate.count;
		AbSyn	body  = absyn->abGenerate.body;

		gen0FindDefs(count, NULL, stab, true, false);
		gen0FindDefs(body,  NULL, stab, true, false);
		break;
	}
	case AB_Reference: {
		AbSyn	body  = absyn->abReference.body;

		/* Mark our parameter as being used deeply */
		gen0FindDefs(body,  NULL, stab, true, false);
		break;
	}
	case AB_Collect: {
		AbSyn	*argv = absyn->abCollect.iterv;
		AbSyn	body  = absyn->abCollect.body;

		for (i = 0; i < argc - 1; i += 1)
			gen0FindDefs(argv[i], NULL, stab, true, false);
		gen0FindDefs(body, NULL, stab, true, false);
		break;
	}
	case AB_Id: {
		Syme	syme = abSyme(absyn);
		gen0FindDefsSyme(stab, syme, inHighLev);
		break;
	}
	case AB_Where: {
		AbSyn	ctxt = absyn->abWhere.context;
		AbSyn	expr = absyn->abWhere.expr;

		stab = abStab(absyn);

                gen0FindDefs(ctxt, NULL, abStab(absyn), inHighLev, false);
                gen0FindDefs(expr, NULL, abStab(absyn), inHighLev, false);
		break;
	}
	case AB_With: {
		AbSyn	lhs = absyn->abWith.base;
		AbSyn	rhs = absyn->abWith.within;

		gen0FindDefs(lhs, NULL, stab, inHighLev, false);
		gen0FindDefs(rhs, NULL, stab, inHighLev, false);
		break;
	}
	case AB_Lambda:
	case AB_PLambda: {
		AbSyn	fbody;
		Bool	markParams = false;

		fbody = absyn->abLambda.body;
		while (abTag(fbody) == AB_Label)
			fbody = fbody->abLabel.expr;

		if (abTag(fbody) == AB_Add || abTag(fbody) == AB_With ||
		    abIsAnyLambda(fbody))
			markParams = true;

		if (lhs) {
			Syme	syme = abSyme(lhs);
			symeSetConstNum(syme, gen0FwdProgNum);
			if (symeDefLevelNo(syme) == stabLevelNo(stab)) {
				Foam	foam = foamNewConst(gen0FwdProgNum);
				foamSyme(foam) = syme;
				foam = foamNewClos(foamNewEnv(int0), foam);
				symeSetClosure(syme, foam);
			}
			if (!markParams) {
				TForm	ret = tfMapRet(symeType(syme));
				markParams = tfSatCat(ret) || tfSatDom(ret);
			}
		}

		gen0FwdProgNum -= 1;

		if (markParams) gen0MarkParamsDeep(stab,absyn->abLambda.param);
		gen0FindDefs(fbody, NULL, abStab(absyn), inHighLev, false);

		break;
	}
	case AB_Apply: {
		AbSyn	*argv = abArgv(absyn);
		AbSyn	impl = abImplicit(absyn);
		Syme	syme = abSyme(argv[0]);
		Bool	isForeign = false;

		if (syme && symeIsForeign(syme) &&
			symeForeign(syme)->protocol == FOAM_Proto_Fortran)
			isForeign = true;

		if (abStab(absyn))
			stab = abStab(absyn);

		if (impl) gen0FindDefs(impl, NULL, stab, inHighLev, topLev);

		
		for (i = 0; i < argc; i += 1)
		{
			/*
			 * We want to check to see if this argument is a
			 * function which is being passed to a foreign import.
			 * If so then it will be lexically deep in the wrapper
			 * that surrounds it (see gf_fortran.c) and we need
			 * to note this here.
			 */
			Bool	isDeep = false;
			TForm	tfi = abTUnique(argv[i]);


			/*
			 * At the moment we don't type check the origin
			 * field of imports so tfi may be NULL.
			 */
			if (tfi)
			{
				Bool isFunArg = tfIsAnyMap(tfi);
				isDeep = i && isForeign && isFunArg;
			}

			gen0FindDefs(argv[i], NULL, stab, isDeep || inHighLev, false);
		}
		break;
	}
	case AB_Try: {
		AbSyn	id     = absyn->abTry.id;
		AbSyn	expr   = absyn->abTry.expr;
		AbSyn	except = absyn->abTry.except;
		AbSyn	always = absyn->abTry.always;

		gen0FindDefs(id,     lhs, stab, inHighLev, false);
		gen0FindDefs(expr,   lhs, stab, true, false);
		gen0FindDefs(except, lhs, stab, inHighLev, false);
		gen0FindDefs(always, lhs, stab, inHighLev, false);
		break;
	}
  	case AB_Export: {
		AbSyn	what = absyn->abExport.what;
		AbSyn	from = absyn->abExport.origin;
		AbSyn	dest = absyn->abExport.destination;

		if (abTag(dest) == AB_Apply) 
			gen0FindDefs(what, lhs, stab, true, false);
		else 
			gen0FindDefs(what, lhs, stab, inHighLev, false);
		gen0FindDefs(dest, lhs, stab, inHighLev, false);
		gen0FindDefs(from, lhs, stab, inHighLev, false);
		break;
	}
	case AB_Label: {
		AbSyn	expr = absyn->abLabel.expr;
		gen0FindDefs(expr, lhs, stab, inHighLev, topLev);
		break;
	}
	default:
		if (abImplicitSyme(absyn) != NULL) {
			gen0FindDefs(abImplicit(absyn), NULL, stab, inHighLev, false);

		}
		if (!abIsLeaf(absyn))
			for (i = 0; i < argc; i += 1)
				gen0FindDefs(abArgv(absyn)[i], NULL,
					     stab, inHighLev, false);
		break;
	}
}

local void
gen0FindDefsSyme(Stab stab, Syme syme, Bool inHighLev)
{
	if (!syme) return;
	if (!symeUsed(syme)) {
		stabUseMeaning(stab, syme);
		symeSetUsed(syme);
	}
	if (inHighLev ||
	    stabLambdaLevelNo(stab) != symeDefLambdaLevelNo(syme)) {
		stabUseMeaning(stab, syme);
		symeSetUsed(syme);
		symeSetUsedDeeply(syme);
	}
}
local void
gen0MarkParamsDeep(Stab stab, AbSyn param)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, param);
	Length	i, argc	= abArgcAs(AB_Comma, param);

	for (i = 0; i < argc; i += 1) {
		Syme	syme = abSyme(abDefineeId(argv[i]));
		if (syme) {
			symeSetUsedDeeply(syme);
			stabUseMeaning(stab, syme);
		}
	}
}

/*****************************************************************************
 *
 * :: Temporaries
 *
 ****************************************************************************/

/*
 * Temp to make re-evaluation side-effect free 
 */
Foam
gen0MakeMultiEvaluable(int type, int fmt, Foam foam)
{
	Foam new;

	if (gen0IsMultiEvaluable(foam))
		return foam;
	else {
		new = gen0Temp0(type, fmt);
		gen0AddStmt(foamNewSet(new, foam), NULL);
		return foamCopy(new);
	}
}


/*
 * Create a temporary variable if needed.
 */

local Foam
gen0TempValueMode(TForm tf)
{
	int		i;
	Foam		vals;

	if (!gen0ValueMode)
		return NULL;

	if (!tfIsMulti(tf)) {
		AInt fmt;
		FoamTag type;
		type = gen0Type(tf, &fmt);
#if AXL_EDIT_1_1_12p6_23
		if (type == FOAM_Arr) {
		  return gen0TempLocal0(FOAM_Ptr,emptyFormatSlot) ;
		    }
#endif
		return gen0TempLocal0(type, fmt);
	}
	vals = foamNewEmpty(FOAM_Values, tfMultiArgc(tf));
	for (i = 0; i < tfMultiArgc(tf); i += 1) {
		AInt fmt, tag;
		tag = gen0Type(tfMultiArgN(tf, i), &fmt);
		vals->foamValues.argv[i] =
			gen0TempLocal0(tag, fmt);
	}
	return vals;
}

local Foam
gen0TempValue(AbSyn absyn)
{
	if (gen0ValueMode && gen0AbType(absyn) != tfExit)
		return genFoamVal(absyn);
	else {
		genFoamStmt(absyn);
		return NULL;
	}
}

local void
gen0SetTemp(Foam t, Foam foam)
{
	Length	i;

	if (t == NULL || foam == NULL)
		return;

	if (foamTag(t) != FOAM_Values) {
		gen0AddStmt(foamNewSet(foamCopyNode(t), foam), NULL);
		return;
	}
	if (foamTag(foam) == FOAM_Nil) return;

	assert(foamTag(foam) == FOAM_Values);
	for (i = 0; i < foamArgc(t); i += 1)
		gen0AddStmt(foamNewSet(foamCopyNode(t->foamValues.argv[i]),
				       foam->foamValues.argv[i]), NULL);
}

/*
 * Create a temporary variable given foam_type and fmt number
 * The fmt number is used creating the decl.
 * If the format isn't meaningfull, you can use the gen0Temp macro.
 */
Foam
gen0Temp0(int type, int fmt)
{
	int	i;

	gen0State->hasTemps = true;
	if (!gen0InDeep(gen0State->progType)) {
		i = vpNewVar0(gen0State->localPool, type, fmt);
		return foamNewLoc(i);
	}
	else {
		return gen0TempLex0(type, fmt);
	}
}

/*
 * Free up a temporary variable for possible re-use
 */
local void
gen0FreeTemp(Foam var)
{
	if (!var)
		return;
	if (foamTag(var) == FOAM_Loc)
		vpFreeVar(gen0State->localPool, (int) var->foamLoc.index);
	else
		vpFreeVar(gen0State->lexPool,	(int) var->foamLex.index);
}

/*
 * Create a local temporary variable.
 */

Foam
gen0TempFrTForm(TForm tf, Bool isLocal)
{
/*	FoamTag type = gen0Type(tf, NULL);*/

	return NULL;
}

Foam
gen0TempLocal0(int type, int fmt)
{
	int	i;
	gen0State->hasTemps = true;
	i = vpNewVar0(gen0State->localPool, type, fmt);
	return foamNewLoc(i);
}

Foam
gen0TempFrDDecl(AInt id, Bool isLocal)
{
	Foam	ddecl, vals;
	Length	i;

	ddecl = gen0GetRealFormat(gen0FindFormat(id));
	vals = foamNewEmpty(FOAM_Values, foamDDeclArgc(ddecl));
	for (i = 0; i < foamDDeclArgc(ddecl); i += 1) {
		FoamTag	type = ddecl->foamDDecl.argv[i]->foamDecl.type;
		vals->foamValues.argv[i] =
			isLocal ? gen0TempLocal(type) : gen0Temp(type);
	}
	return vals;
}

/*
 * Create a local temporary variable.
 */
Foam
gen0TempLex0(int type, int fmt)
{
	int	i;

	gen0State->hasTemps = true;
	i = vpNewVar0(gen0State->lexPool, type, fmt);
	/* Over-defensive, but saves hassle */
	gen0UseStackedFormat(int0);
	/* since env vars are created this way, we should be OK */
	if ( listIsSingleton(gen0State->envFormatStack))
		return foamNewLex(int0, i);
	else
		return foamNewEElt(car(gen0State->envFormatStack),
				   foamCopy(car(gen0State->envVarStack)),
				   int0, i);
}

Foam
gen0TempLexNth(int type, int wn)
{
	VarPool	vp;
	int 	i;

	if (wn == 0)
		vp = gen0State->lexPool;
	else
		vp = listElt(VarPool)(gen0State->envLexPools, wn - 1);

	i = vpNewVar(vp, type);

	return gen0NewLex(wn, i);
}

/* Called when potentially in where clause, and level is 0 */
Foam
gen0NewLex(int idx, int offset)
{
	Foam    foam;

	if (gen0State->whereNest && idx != gen0State->whereNest) {
		AInt	fmt = listElt(AInt)(gen0State->envFormatStack, idx);
		Foam	var = listElt(Foam)(gen0State->envVarStack, idx);
		foam = foamNewEElt(fmt, foamCopy(var), int0, offset);
	}
	else 
		foam = foamNewLex(int0, offset);

	return foam;
}

/*****************************************************************************
 *
 * :: Collect statements
 *
 ****************************************************************************/
/*
 * Generate a generator for a collect form.
 * E for a in b...  ==> generate for a in b... repeat yield E
 */
local Foam
genCollect(AbSyn absyn)
{
	AbSyn	iter, body, repeat;
	int	i;

	body   = abNewYield(abPos(absyn), absyn->abCollect.body);

	repeat = abNewEmpty(AB_Repeat, abArgc(absyn));
	abSetStab(repeat, abStab(absyn));
	repeat->abRepeat.body = body;
	for(i=1; i< abArgc(absyn); i++)
		abArgv(repeat)[i] = abArgv(absyn)[i];

	iter   = abNewGenerate(abPos(absyn), abNewNothing(sposNone), repeat);

	abTUnique(body)	  = tfExit;
	abTUnique(repeat) = tfNone();
	abTUnique(iter)	  = gen0AbType(absyn);

	abState(body)	= AB_State_HasUnique;
	abState(repeat)	= AB_State_HasUnique;
	abState(iter)	= AB_State_HasUnique;

	return genFoam(iter);
}

/*
 * Bump the lexical level numbers for a body of a generator function.
 */

void
gen0IncLexLevels(Foam foam, AInt inc)
{
	FoamTag		tag = foamTag (foam);

	assert(foam);

	foamIter(foam, arg, gen0IncLexLevels(*arg, inc));

	switch (tag) {
	case FOAM_Lex:  
		foam->foamLex.level += inc; 
		break;
	case FOAM_Env:  
		foam->foamEnv.level += inc; 
		break;
	default: 
		break;
	}

}

void
gen0AddLexLevels(Foam foam, int dist)
{
	if (gen0State->whereNest==0)
		gen0IncLexLevels(foam, dist);
	else {
		gen0AddLexLevels1(foam, dist, gen0State->whereNest, 
				  gen0State->envVarStack);
	}
}

local void
gen0AddLexLevels1(Foam foam, AInt dist, int wN, FoamList envStack)
{
	switch (foamTag(foam)) {
	  case FOAM_Lex:
		if (foam->foamLex.level < 0)
			foam->foamLex.level+=dist;
		else
			foam->foamLex.level+=dist+wN;
		break;
	  case FOAM_Env:
		if (foam->foamEnv.level < 0)
			foam->foamEnv.level+=dist;
		else
			foam->foamEnv.level+=dist+wN;
		break;
	  /* !Warning: Changes the type of a node... */
	  case FOAM_EElt: { int pos;
		listFind(Foam)(envStack, foam->foamEElt.ref, foamEqual, &pos);
		if (pos == -1)
			gen0AddLexLevels1(foam->foamEElt.ref, 
					 dist, wN, envStack);
		else {
			int off = foam->foamEElt.lex;
	
			foam->hdr.tag = FOAM_Lex;
			foam->hdr.argc = 2;
			foam->foamLex.level = pos+dist;
			foam->foamLex.index = off;
		}
		break;
		    }
	  default:
		foamIter(foam, arg,
			 gen0AddLexLevels1(*arg, dist, wN, envStack));
	}
}

/*
 * Replace format number place holders with the real format numbers.
 */
local void
gen0PatchFormatNums(Foam prog)
{
	Foam	levels = prog->foamProg.levels;
	Length	i;

	if (foamTag(prog) != FOAM_Prog)
		return;

	for (i = 0; i < foamArgc(levels); i += 1) {
		AInt	nindex = gen0FindFormat(levels->foamDEnv.argv[i]);
		if (nindex >= 0) levels->foamDEnv.argv[i] = nindex;
	}

	gen0PatchEEltFormats(prog);
}

local void
gen0PatchEEltFormats(Foam foam)
{
	assert(foam);

	foamIter(foam, arg, gen0PatchEEltFormats(*arg));

	if (foamTag(foam) == FOAM_EElt) {
		AInt	nindex = gen0FindFormat(foam->foamEElt.env);
		if (nindex >= 0) foam->foamEElt.env = nindex;
	}
	if (foamTag(foam) == FOAM_Const && foam->foamConst.index > gen0NumProgs)
		foam->foamConst.index = gen0FindConst(foam->foamConst.index);
}

void
gen0AddFormat(AInt index, AInt nindex)
{
	formatPlaceList = listCons(AInt)(index, formatPlaceList);
	formatRealList  = listCons(AInt)(nindex, formatRealList);
}

local AInt
gen0FindFormat(AInt index)
{
	Length	pos;

	if (index >= 0)
		return index;

	pos = listPosq(AInt)(formatPlaceList, index);
	if (pos == -1)
		return index;

	return listElt(AInt)(formatRealList, pos);
}

void
gen0AddConst(AInt old, AInt new)
{
	gen0ConstList = listCons(AInt)(old, gen0ConstList);
	gen0RealConstList = listCons(AInt)(new, gen0RealConstList);
}

local AInt
gen0FindConst(AInt old)
{
	Length	pos;

	pos = listPosq(AInt)(gen0ConstList, old);
	assert(pos != -1);

	return listElt(AInt)(gen0RealConstList, pos);
}

/*
 * Functions manipulating the state of code generation for a single lambda.
 */
GenFoamState
gen0NewState(Stab stab, int format, GenFoamTag tag)
{
	GenFoamState	s = (GenFoamState) stoAlloc(OB_Other,
						    sizeof(struct gfs));

	s->tag		 = tag;
	s->stabLevel	 = (stab ? stabLevelNo(stab) : -1);
	s->foamLevel	 = (gen0State ? gen0State->foamLevel + 1 : 0);
	s->stab		 = stab;
	s->localPool	 = vpNew(fboxNew(foamNewEmptyDDecl(FOAM_DDecl_Local)));
	s->lexPool	 = vpNew(fboxNew(foamNewEmptyDDecl(FOAM_DDecl_LocalEnv)));
	s->envLexPools	 = listNil(VarPool);
	s->params	 = fboxNew(foamNewEmptyDDecl(FOAM_DDecl_Param));
	s->formatStack	 = listNil(AInt);
	s->formatUsage	 = listNil(AInt);
	s->fluidsUsed	 = listNil(AInt);
	s->program	 = NULL;
	s->yieldCount	 = 0;
	s->progType	 = PT_Std;
	s->labelNo	 = 0;
	s->yieldLabels	 = listNil(AInt);
	s->yieldPlace	 = 0;
	s->yieldValueVar = NULL;
	s->lines	 = listNil(Foam);
	s->inits	 = listNil(Foam);
	s->importPlace	 = NULL;
	s->importPlacePrev = NULL;
	s->funImportList = listNil(Syme);
	s->domImportList = listNil(TForm);
	s->domList	 = listNil(Foam);
	s->hasTemps	 = false;
	s->envVarStack	 = listCons(Foam)(foamNewEnv(int0), listNil(Foam));
	s->envFormatStack= listCons(AInt)(format, listNil(AInt));
	s->whereNest	 = 0;
	s->type		 = NULL;
	s->param	 = NULL;
	s->exporter	 = NULL;
	s->parent	 = gen0State;
	s->base		 = NULL;
	s->domCache	 = listNil(DomainCache);
	s->dbgContext    = (Foam)NULL;
	return s;
}

local void
gen0InitState(Stab stab, int index)
{
	GenFoamState	s = gen0NewState(stab, index, GF_File);

	s->formatUsage = listCons(AInt)(emptyFormatSlot, listNil(AInt));
	s->formatStack = listCons(AInt)(index, listNil(AInt));

	gen0State = s;
}

GenFoamState
gen0NthState(AInt n)
{
	GenFoamState	s = gen0State, prev=0;
	
	n+=s->whereNest;
	while(n > 0) {
		assert(s != 0);
		n -= s->whereNest;
		prev=s;
		s = s->parent;
		n -= 1;
	}
	if (n < 0 && prev->whereNest) return prev;
	assert(s != 0);
	return s;
}

local int 
gen0StateOffset(int symeLevel, int foamLevel)
{
	GenFoamState s = gen0NthState(foamLevel);
	int idx;

	idx = s->whereNest ? s->whereNest - (symeLevel - s->stabLevel) : 0;
	if (idx > s->whereNest) idx = s->whereNest;
	return idx;
}

int
gen0AddGlobal(Foam decl)
{
	FoamList glst;
	AInt idx = 0;
	/* Merge identical globals */
	/* If we import something already exported, or export something 
	   already imported then make it an export */
	glst = gen0GlobalList;
	while (glst) {
		Foam odecl = car(glst);
		if (odecl->foamGDecl.type == decl->foamGDecl.type 
		    && odecl->foamGDecl.format == decl->foamGDecl.format
		    && odecl->foamGDecl.protocol == decl->foamGDecl.protocol
		    && strEqual(odecl->foamGDecl.id, decl->foamGDecl.id)) {
			if (decl->foamGDecl.dir == FOAM_GDecl_Export)
				odecl->foamGDecl.dir = FOAM_GDecl_Export;
			foamFree(decl);
			return gen0NumGlobals - idx - 1;
		} else {
			glst = cdr(glst);
			idx++;
		}
	}

	gen0GlobalList = listCons(Foam)(decl, gen0GlobalList);
	return gen0NumGlobals++;
}

Foam
gen0GetGlobal(AInt n)
{
	assert (n < gen0NumGlobals);
	return listElt(Foam)(gen0GlobalList, gen0NumGlobals - (n+1));
}

void
gen0PushFormat(int index)
{
	GenFoamState	s = gen0State;

	s->formatStack = listCons(AInt)(index, s->formatStack);
	s->formatUsage = gen0UnusedFormats(s->formatStack);
}

int 
gen0RootEnv()
{
	return gen0FoamLevel(1);
}

/* genXXLevel fns wrong currently, but work in most common cases */
local int
gen0FoamLevel(AInt level)
{
	GenFoamState	s = gen0State;
	int		i = 0, whereNest = -s->whereNest;

	while (s) {
		/* Return value should only be fudged if we have a where, 
		   o/w runtime gets clobbered */
		whereNest+=s->whereNest;
		if (s->stab && gen0IsDomLevel(s->tag) &&
		    s->stabLevel <= level)
			return i + (s->whereNest 
				    ? /*whereNest*/ - (level - s->stabLevel) : 0);
		if ((!s->parent || (s->stab && s->parent->stab && s->stabLevel
				    != s->parent->stabLevel)) &&
		    s->stabLevel <= level)
			return i + (s->whereNest 
				    ? /*whereNest*/ - (level - s->stabLevel) : 0);

		s = s->parent;
		i++;
		i += s->whereNest;
	}
	bug("level %d not found\n", level);
	return 0;
}

local Bool
gen0IsLambdaLevel(GenFoamState s, AInt level)
{
	return  s && s->tag == GF_Lambda &&
		s->stab && s->stabLevel <= level;
}

int
gen0FoamImportLevel(AInt level)
{
	GenFoamState s     = gen0State;
	int 	     i     = 0, whereNest = -s->whereNest;
	
	while (s) {
		whereNest += s->whereNest;
		if (s->stab && gen0IsDomLevel(s->tag) &&
		    s->stabLevel <= level)
			return i - (s->whereNest ? (level - s->stabLevel) : 0);
		
		if ((! s->parent || (s->stab && s->parent->stab && 	
				     s->stabLevel != s->parent->stabLevel)) &&
		    s->stabLevel <= level)
			return i - (s->whereNest ? (level - s->stabLevel) : 0);
		
		if ((s->tag == GF_Default || s->tag == GF_DefaultCat)
		    && gen0IsLambdaLevel(s->parent, level)) {
			return i;
		}
		if (s->tag == GF_Add1 &&
		    gen0IsLambdaLevel(s->parent->parent, level)) {
			return i;
		}
		
		s = s->parent;
		i++;
		i+= s->whereNest;
	}
	bug("level not found!");
	return 0;
}

local void
gen0UseStateFormat(GenFoamState s, AInt level)
{
	AIntList	ls = s->formatStack;
	AIntList	lu = s->formatUsage;

	while(level > 0) {
		assert(ls != 0);
		assert(lu != 0);
		ls = cdr(ls);
		lu = cdr(lu);
		level--;
	}
	assert(ls != 0);
	assert(lu != 0);
	car(lu) = car(ls);
}

void
gen0UseStackedFormat(AInt level)
{
	gen0UseStateFormat(gen0State, level);
}

local void
gen0UseFormat(AInt level, int slot)
{
	AIntList		l = gen0State->formatUsage;
	while(level > 0) {
		assert(l != 0);
		l = cdr(l);
		level -= 1;
	}
	assert(l != 0);
	car(l) = slot;
}

local Syme
gen0FindImportedSyme(Syme syme, AInt level, Bool add)
{
	GenFoamState	s = gen0NthState(level);
	SymeList	l = s->funImportList;
	Syme		osyme = NULL;

	for (; !osyme && l; l = cdr(l))
		if (symeEqual(syme, car(l)))
			osyme = car(l);

	if (!osyme && add) {
		symeClrImportInit(syme);
		s->funImportList = listCons(Syme)(syme, s->funImportList);
	}

	return osyme;
}

local Bool
gen0GetImportedSyme(Syme syme, AInt level, Bool add)
{
	Syme	osyme = gen0FindImportedSyme(syme, level, add);

	if (osyme && osyme != syme) {
		gen0SetVarIndex(syme, gen0VarIndex(osyme));
		gen0SetFoamKind(syme, gen0FoamKind(osyme));
		if (symeImportInit(osyme))
			symeSetImportInit(syme);
		else
			symeClrImportInit(syme);
	}

	return osyme != NULL;
}

local void
gen0SetImportedSyme(Syme syme, AInt level)
{
	Syme	osyme = gen0FindImportedSyme(syme, level, false);

	assert(osyme);
	symeSetImportInit(syme);
	symeSetImportInit(osyme);
}


Bool
genIsConst(Syme syme)
{
	return syme && !symeHasDefault(syme);
}

Bool
genIsLocalConst(Syme syme)
{
	return syme && !symeHasDefault(syme) && symeIsLocalConst(syme);
}

Bool
genIsVar(Foam foam)
{
	switch (foamTag(foam)) {
	  case FOAM_Par:
	  case FOAM_Loc: 
	  case FOAM_Glo:
	  case FOAM_Lex:
	  case FOAM_RElt:
	  case FOAM_RRElt:
	  case FOAM_IRElt:
	  case FOAM_TRElt:
	  case FOAM_EElt:
		return 1;
	  default:
		return 0;
	}
}


String 
gen0GlobalName(String libname, Syme syme)
{
	Syme	ext0 = symeExtension(syme);
	String	g;

	symeSetExtension(syme, NULL);
	if (genIsRuntime() && !symeIsImport(syme))
		g = strCopy(symeString(syme));
	else {
		String l = strLower(strCopy(libname));
		Hash   h = gen0SymeTypeCode(syme);
		g = strPrintf("%s_%s_%09d", l, symeString(syme), h);
		strFree(l);
	}
	symeSetExtension(syme, ext0);

	return g;
}

/* Reverse of above */
void
genGlobalInfo(Foam glo, String *pname, String *porig, int *phash)
{
	String name = glo->foamDecl.id;
	String nameStart, nameEnd;

	*porig = strCopy("");
	*pname = name;
	*phash = 0;

	if (glo->foamGDecl.protocol != FOAM_Proto_Foam) 
		return;

	nameStart = strchr(name, '_');

	if (!nameStart) 
		return;

	nameStart++;
	nameEnd = strrchr(nameStart, '_');
	assert(nameEnd);

	*porig = strnCopy(name, nameStart - name - 1);
	*pname = strnCopy(nameStart, nameEnd - nameStart);
	*phash = atoi(nameEnd+1);
}


/* Try to compute the side effect bit correctly . */
local void
gen0ComputeSideEffects(Foam prog)
{
	/*!! Needs beefing up */
	Foam	body = prog->foamProg.body;
	Foam	last;
	if (foamArgc(body) == 0) {
		foamProgUnsetSide(prog);
		return;
	}
	if (foamArgc(body) > 1) { /*!!*/
		foamProgSetSide(prog);
		return;
	}
	last = body->foamSeq.argv[foamArgc(body)-1];
	if (foamTag(last) != FOAM_Return) {
		foamProgSetSide(prog);
		return;
	}
	last = last->foamReturn.value;
	if (foamTag(last) != FOAM_BCall) {
		foamProgSetSide(prog);
		return;
	}
	if (foamBValInfo(last->foamBCall.op).hasSideFx)
		foamProgSetSide(prog);
	else
		foamProgUnsetSide(prog);
	return;
}

/*
 * Returns true if foam can evaluated multiple times w/o a call.
 */
local Bool
gen0IsMultiEvaluable(Foam foam)
{
	if (foamTag(foam) == FOAM_Cast)
		return gen0IsMultiEvaluable(foam->foamCast.expr);
	return foamIsRef(foam) || foamTag(foam) < FOAM_DATA_LIMIT;
}

local Foam
gen0Embed(Foam val, AbSyn ab, TForm tf, AbEmbed embed)
{
	/* Deal with delta-equality of cross/multis */
	tf = tfDefineeMaybeType(tf);

	switch (embed) {
	case AB_Embed_Identity:
		return val;
	case AB_Embed_CrossToTuple:
		return gen0CrossToTuple(val, tf);
	case AB_Embed_CrossToMulti:
		return gen0CrossToMulti(val, tf);
	case AB_Embed_CrossToUnary:
		return gen0CrossToUnary(val, tf);
	case AB_Embed_MultiToTuple:
		return gen0MultiToTuple(val);
	case AB_Embed_MultiToCross:
		return gen0MultiToCross(val, tf);
	case AB_Embed_MultiToUnary:
		return gen0MultiToUnary(val);
	case AB_Embed_UnaryToTuple:
		return gen0UnaryToTuple(val);
	case AB_Embed_UnaryToCross:
		return gen0UnaryToCross(val, tf);
	case AB_Embed_UnaryToMulti:
		return gen0UnaryToMulti(val);
	case AB_Embed_UnaryToRaw:
		return gen0UnaryToRaw(val, ab);
	case AB_Embed_RawToUnary:
		return gen0RawToUnary(val, ab);
	default:
		bugBadCase(embed);
		NotReached(return val);
	}
}

local Foam
gen0CrossToMulti(Foam val, TForm tf)
{
	Foam	values;
	Foam	t; 
	int	i, size = tfCrossArgc(tf);
	AInt    cfmt, ftype;

	ftype = gen0Type(tf, &cfmt);
	cfmt  = gen0CrossFormatNumber(tf);
	t   = gen0TempLocal0(FOAM_Rec, cfmt);
	gen0SetTemp(t, foamNewCast(FOAM_Rec, val));
	values = foamNewEmpty(FOAM_Values, size);
	for (i = 0; i < size ; i++)
		values->foamValues.argv[i] = foamNewRElt(cfmt, foamCopy(t), i);

	foamFree(t);
	return values;
}

local Foam
gen0CrossToUnary(Foam val, TForm tf)
{
	AInt    cfmt = gen0CrossFormatNumber(tf);

	return foamNewRElt(cfmt, val, int0);
}

local Foam
gen0CrossToTuple(Foam val, TForm tf)
{
	Foam vars[2], tupl, elts, relt;
	AInt cfmt, ftype;
	Foam t;
	int  i;
	
	ftype = gen0Type(tf, &cfmt);
	cfmt  = gen0CrossFormatNumber(tf);
	t     = gen0TempLocal0(FOAM_Rec, cfmt);

	gen0SetTemp(t, foamNewCast(FOAM_Rec, val));
	gen0MakeEmptyTuple(foamNewSInt(tfCrossArgc(tf)), vars, NULL);
	tupl = vars[0];
	elts = vars[1];
	
	for (i=0; i < tfCrossArgc(tf); i++) {
		relt = foamNewRElt(cfmt, foamCopy(t), i);
		gen0AddStmt(gen0ASet(elts, i, FOAM_Word, relt), NULL);
	}
	
	return tupl;
}

local Foam
gen0MultiToTuple(Foam val)
{
	Length	i, argc = foamArgc(val);
	Foam	vars[2], tupl, elts, elt;

	assert(foamTag(val) == FOAM_Values);
	gen0MakeEmptyTuple(foamNewSInt(argc), vars, NULL);
	tupl = vars[0];
	elts = vars[1];

	for (i = 0; i < argc; i += 1) {
		elt = val->foamValues.argv[i];
		gen0AddStmt(gen0ASet(elts, (AInt) i, FOAM_Word, elt), NULL);
	}

	return tupl;
}

local Foam
gen0MultiToCross(Foam val, TForm tf)
{
	TForm ctf = tfCrossFrMulti(tf);
	Length	i, argc = foamArgc(val);
	AInt    cfmt;
	Foam	t;
	Foam    elt;

	assert(foamTag(val) == FOAM_Values);

	cfmt = gen0CrossFormatNumber(ctf);
	t = gen0TempLocal0(FOAM_Rec, cfmt);

	gen0SetTemp(t, foamNewRNew(cfmt));

	for (i = 0; i < argc; i += 1) {
		elt = val->foamValues.argv[i];
		gen0AddStmt(foamNewSet(foamNewRElt(cfmt, foamCopy(t), i),
				       elt), NULL);
	}

	return foamNewCast(FOAM_Word, t);
}

local Foam
gen0MultiToUnary(Foam val)
{
	assert (foamTag(val) == FOAM_Values);
	assert (foamArgc(val) == 1);
	return val->foamValues.argv[0];
}

local Foam
gen0UnaryToTuple(Foam val)
{
	Foam	vars[2], tupl, elts;
	
	gen0MakeEmptyTuple(foamNewSInt(1), vars, NULL);
	tupl = vars[0];
	elts = vars[1];

	gen0AddStmt(gen0ASet(elts, (AInt) 0, FOAM_Word, val), NULL);
	return tupl;
}

local Foam
gen0UnaryToMulti(Foam val)
{
	Foam	values;

	values = foamNewEmpty(FOAM_Values, 1);
	values->foamValues.argv[0] = val;
	return values;
}

local Foam
gen0UnaryToCross(Foam val, TForm tf)
{
	TForm   ctf;
	AInt    cfmt, ftype;
	Foam	t;
	
	ctf   = tfCross(1, tf);
	ftype = gen0Type(tf, &cfmt);
	cfmt  = gen0CrossFormatNumber(ctf);
	t = gen0TempLocal0(FOAM_Rec, cfmt);

	gen0AddStmt(foamNewSet(foamCopy(t), foamNewRNew(cfmt)), NULL);
	gen0AddStmt(foamNewSet(foamNewRElt(cfmt, foamCopy(t), int0), val), NULL);
	
	return foamNewCast(FOAM_Rec, t);
}


local Foam
gen0UnaryToRaw(Foam val, AbSyn ab)
{
	AbSyn	imp = abImplicit(ab);
/* BDS: */
/* BDS:  This is the bug for pack0.sh */
/* BDS: */
/* Orig	FoamTag	raw = gen0Type(gen0AbType(imp), NULL); */
	FoamTag	raw = gen0Type(gen0AbType(imp), NULL);
	Syme	syme = abSyme(abApplyOp(imp));
	Foam	foam, *argloc;

/*	printf("BDS: Inside gen0UnaryToRaw\n");  */

	foam = gen0ApplySyme(raw, syme, abSymeImpl(abApplyOp(imp)), 1, &argloc);
	/* BDS This foamPrint may cause a crash because argloc isn't initialized */
/*	foamPrint(stdout,foam); */


/* BDS: */
/* BDS:  This is the bug for pack0.sh */
/* BDS: */
/* 	argloc[0] = genFoamCast(val, ab, FOAM_Word); */
/*	argloc[0] = genFoamCast(val, ab, raw);  */
/*	argloc[0] = genFoamCast(val, ab, raw);  */
	/*
	 *  In its original form, this code was casting the value to a word
         *  without considering its type.  However, no cast should be
         *  performed because it is raw's job to perform the conversion 
         *  to the raw data type.
         */
	argloc[0] = val; 

/*
	printf("BDS: About to finish in gen0UnaryToRaw\n"); 
	foamPrint(stdout,foam);
*/

	return foam;
}

local Foam
gen0RawToUnary(Foam val, AbSyn ab)
{
	AbSyn	imp = abImplicit(ab);
	FoamTag	raw = gen0Type(gen0AbType(abApplyArg(imp, int0)), NULL);
	Syme	syme = abSyme(abApplyOp(imp));
	Foam	foam, *argloc;

/*	printf("BDS: Inside gen0RawToUnary\n"); */

	foam = gen0ApplySyme(FOAM_Word, syme, abSymeImpl(abApplyOp(imp)), 
							 1, &argloc);
	argloc[0] = genFoamCast(val, ab, raw);

/*	printf("BDS: Done in gen0RawToUnary\n"); */

	return foam;
}

local FoamTag
gen0FoamType(Foam foam)
{
	Foam	decl;
	switch(foamTag(foam)) {
	case FOAM_Loc:
		decl = fboxNth(gen0State->localPool->fbox,
			       (AInt) foam->foamLoc.index);
		break;
	case FOAM_Par:
		decl = fboxNth(gen0State->params, (int) foam->foamPar.index);
		break;
	case FOAM_Lex: {
		GenFoamState s = gen0NthState(foam->foamLex.level);
		decl = fboxNth(s->lexPool->fbox, (int) foam->foamLex.index);
		break; }
	case FOAM_Glo:
		decl = gen0GetGlobal(foam->foamGlo.index);
		break;
		
	case FOAM_RRElt:
		return FOAM_Word;

        case FOAM_RElt: {
		AInt	fmt = foam->foamRElt.format;
		AInt	slot = foam->foamRElt.field;
		Foam	ddecl = gen0GetRealFormat(fmt);

		assert(slot < foamArgc(ddecl));		
		return ddecl->foamDDecl.argv[slot]->foamDecl.type;
	}

        case FOAM_IRElt: {
		AInt	fmt = foam->foamIRElt.format;
		AInt	slot = foam->foamIRElt.field;
		Foam	ddecl = gen0GetRealFormat(fmt);

		return foamTRDDeclIDecl(ddecl, slot)->foamDecl.type;
	}

        case FOAM_TRElt: {
		AInt	fmt = foam->foamTRElt.format;
		AInt	slot = foam->foamTRElt.field;
		Foam	ddecl = gen0GetRealFormat(fmt);

		return foamTRDDeclTDecl(ddecl,slot)->foamDecl.type;
	}

	default:
		return foamExprType(foam, NULL, NULL, NULL, NULL, NULL);
	}
	return decl->foamDecl.type;
}

Foam
gen0BuiltinCCall(FoamTag type, String funName, String libName, Length argc, ...)
{
	va_list argp;
	Foam   fn, call;
	AInt   glNo;
	int    i;

	if (1/*strEqual(libName, "boot")*/) {
		va_list argp;
		Foam	ccall;
		int	i;

		ccall = foamNewEmpty(FOAM_CCall, argc + 2);
		ccall->foamCCall.type = type;
		ccall->foamCCall.op = foamNewGlo(gen0BuiltinImport(funName, libName));
		
		va_start(argp, argc);
		for(i=0; i< argc; i++)
			ccall->foamCCall.argv[i] = va_arg(argp, Foam);
		va_end(argp);
		return ccall;
	}
	glNo = gen0BuiltinImport(funName, libName);
	fn = gen0GetLazyBuiltin(libName, glNo, argc, 1);
	call = foamNew(FOAM_CCall, argc + 2);
	call->foamCCall.type = FOAM_Word;
	call->foamCCall.op   = fn;

	va_start(argp, argc);
	for (i=0; i<argc ; i++) {
		call->foamCCall.argv[i] = va_arg(argp, Foam);
	}

	return foamNewCast(type, call);
}

Foam 
gen0LazyBuiltinCCall(String funName, String libName,
		     Length nOutArgs, Length argc, ...)
{
	va_list argp;
	Foam   fn, call;
	AInt   glNo;
	int    i;
	
	glNo = gen0BuiltinImport(funName, libName);
	fn = gen0GetLazyBuiltin(libName, glNo, argc, nOutArgs);
	call = foamNew(FOAM_CCall, argc + 2);
	call->foamCCall.type = (nOutArgs == 1) ? FOAM_Word : FOAM_NOp;
	call->foamCCall.op   = fn;

	va_start(argp, argc);
	for (i=0; i<argc ; i++) {
		call->foamCCall.argv[i] = va_arg(argp, Foam);
	}

	return call;
}

local AInt
gen0BuiltinImport(String fun, String lib)
{
	int		i;
	FoamList	l;
	Foam		decl;

	/*!! Possible conflicts with imported foreign function names */
	for(i=0, l = gen0GlobalList; l; i++, l = cdr(l))
		if (strEqual(fun, car(l)->foamGDecl.id))
			return (AInt) (gen0NumGlobals - i - 1);
	decl = foamNewGDecl(FOAM_Clos, strCopy(fun), emptyFormatSlot,
			    FOAM_GDecl_Import, FOAM_Proto_Foam);
	assert(gen0GetRuntimeCallInfo(decl));
	return (AInt) gen0AddGlobal(decl);
}

local void
gen0MakeBuiltinExports()
{
	AIntList l = listNReverse(AInt)(gen0BuiltinExports);

	while (l) {
		Foam def, val;
		int glId, progId;
		glId   = car(l);
		progId = car(cdr(l));
		l = cdr(cdr(l));
		
		if (progId == 0)
			val = foamNewCast(FOAM_Clos, foamNewNil());
		else 
			val = foamNewClos(foamNewEnv(int0), foamNewConst(progId));
		def = foamNewDef(foamNewGlo(glId), val);
		gen0ProgList = listCons(Foam)(def, gen0ProgList);
		genfConstDEBUG(genfNumProg(gen0NumProgs, "builtin"));
		gen0NumProgs++;
	}
}

Bool 
gen0IsCatDefForm(GenFoamState s)
{
	return (s->tag == GF_DefaultCat);
}

Bool 
gen0IsCatInner()
{
	GenFoamState s = gen0State;

	while (s->tag != GF_File) {
		if (s->tag == GF_DefaultCat || s->tag == GF_Default) 
			return true;
		s = s->parent;
	}

	return false;
}

local AInt
gen0FindGlobalFluid(Syme syme)
{
	FoamList l = gen0FluidList;
	Foam new;
	int i=0;
	/* Should use syme equality... */
	while (l) {
		if (strEqual(car(l)->foamDecl.id, symeString(syme)))
			return i;
		l = cdr(l);
		i++;
	}
	new = foamNewDecl(gen0Type(symeType(syme), NULL), strCopy(symeString(syme)),
			  emptyFormatSlot);
	gen0FluidList = listNConcat(Foam)(gen0FluidList, 
					listCons(Foam)(new, listNil(Foam)));
	return i;
}

local void
gen0AddLocalFluid(AbSyn id)
{
	AIntList l = gen0State->fluidsUsed;
	Syme	syme;
	String	msg;

	switch (abTag(id)) {
	  case AB_Declare:
		id = id->abDeclare.id;
		break;
	  case AB_Id:
		break;
	  default:
		msg = "unexpected absyn in gen0AddLocalFluid";
		comsgFatal(id, ALDOR_F_Bug, msg);
#if 0
		abWrSExpr(dbOut, id, SXRW_Default);
		bug("unexpected absyn in addFluid\n");
#endif
	}
	syme = abSyme(id);
	if (listMemq(AInt)(l, gen0VarIndex(syme))) return;

	gen0State->fluidsUsed = 
			listCons(AInt)(gen0VarIndex(syme), gen0State->fluidsUsed);
	return;
}

local Foam
gen0BuiltinExporter(Foam glo, Syme syme)
{
	GenFoamState saved;
	TForm 	tf = symeType(syme);
	Foam	foam, clos, call, result, tmp;
	FoamTag retType;
	int 	retFmt, i;

	if (!tfIsMap(tf) || genIsRuntime())
		return gen0Syme(syme);

	retType = gen0Type(tfMapRet(tf), NULL);
	retFmt = retType == FOAM_NOp ? gen0MultiFormatNumber(tfMapRet(tf)) : 0;

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(strCopy("builtinWrapper"), NULL);

	saved = gen0ProgSaveState(PT_ExFn);

	for (i=0; i < tfMapArgc(tf); i++)
		gen0AddParam(foamNewDecl(gen0Type(tfMapArgN(tf, i), NULL),
					 strCopy(""),
					 emptyFormatSlot));

	gen0AddStmt(foamNew(FOAM_CCall, 2, FOAM_NOp, 
			    foamNewGlo(gloInitIdx)), NULL);
	call = foamNewEmpty(FOAM_CCall, 2 + tfMapArgc(tf));
	call->foamCCall.type = gen0Type(tfMapRet(tf), NULL);
	call->foamCCall.op   = foamCopy(glo);

	for (i=0; i < tfMapArgc(tf); i++)
		call->foamCCall.argv[i] = foamNewPar(i);

	if (retFmt == 0)
		result = call;
	else {
		tmp = gen0TempFrDDecl(retFmt, true);
		gen0AddStmt(foamNewSet(tmp, foamNewMFmt(retFmt, call)), NULL);
		result = foamCopy(tmp);
	}

	gen0AddStmt(foamNewReturn(result), NULL);
	
	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, retType, int0);
	gen0AddLexLevels(foam, 1);
	foam->foamProg.format = retFmt;
	foam->foamProg.levels = foamNew(FOAM_DEnv, 1, int0);
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);
	gen0ProgRestoreState(saved);

	return clos;
}


local Foam
gen0ForeignValue(Syme syme)
{
	/* printf("BDS: Inside gen0ForeignValue\n"); */

	return foamNew(gen0FoamKind(syme), 1, 
		       (AInt) gen0VarIndex(syme));
}

local Foam
gen0ForeignWrapValue(Syme syme)
{
	/* printf("BDS: Inside gen0ForeignWrapValue\n"); */

	if (symeClosure(syme)) 
		return foamCopy(symeClosure(syme)); 
	else
		return gen0ForeignWrapFn(syme);
}

local Foam
gen0ForeignWrapFn(Syme syme)
{
	Scope("gen0ForeignWrapFn");
	GenFoamState	saved;
	TForm		tf = symeType(syme);
	Foam		foam, clos, call;
	FoamTag		retType;
        int		retFmt = 0;
	int		i;
	Bool		fluid(gen0ValueMode);
	FoamList	fluid(gen0FortranActualArgTmps);

	retType = gen0Type(tfMapRet(tf), NULL);

	gen0ValueMode = !tfIsNone(tfMapRet(tf));

	if (gen0ValueMode && (retType == FOAM_NOp))
		retFmt = gen0MultiFormatNumber(tfMapRet(tf));
	
	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(strCopy("foreignWrapper"), NULL);

	saved = gen0ProgSaveState(PT_ExFn);

	for (i=0; i < tfMapArgc(tf); i++)
		gen0AddParam(foamNewDecl(gen0Type(tfMapArgN(tf, i), NULL),
					 strCopy(""),
					 emptyFormatSlot));

	call = foamNewEmpty(FOAM_PCall, 
			    tfMapArgc(tf) + TypeSlot + OpSlot + ProtoSlot);
	call->foamPCall.protocol = symeForeign(syme)->protocol;
	call->foamPCall.type     = retType;
	call->foamPCall.op  	 = gen0ForeignValue(syme);

	for (i=0; i < tfMapArgc(tf); i++) 
		call->foamPCall.argv[i] = foamNewPar(i);

	if (symeForeign(syme)->protocol == FOAM_Proto_Fortran) {
		call = gen0ModifyFortranCall(syme, call, gen0FortranFnResult,
			gen0ValueMode);
	}			    
	
	if (gen0FortranActualArgTmps)
		gen0FreeFortranActualArgTmps();

	gen0FortranActualArgTmps = NULL;

	if (gen0ValueMode && (retType == FOAM_NOp))
	{
		/* Special care with multi return values */
		Foam tmp = gen0TempFrDDecl(retFmt, true);

		call = foamNewSet(tmp, foamNewMFmt(retFmt, call));
		gen0AddStmt(call, NULL);

		call = gen0TempFrDDecl(retFmt, true);
		for (i=0; i < foamArgc(tmp); i++)
		{
			Foam lhs = foamCopy(call->foamValues.argv[i]);
			Foam rhs = foamCopy(tmp->foamValues.argv[i]);
			gen0AddStmt(foamNewSet(lhs, rhs), NULL);
		}
	}

	if (!gen0ValueMode) /* Void functions also need attention */
		call = foamNew(FOAM_Values, (Length)0);

	gen0AddStmt(foamNewReturn(call), NULL);

	gen0ProgFiniEmpty(foam, retType, int0);

	gen0ProgRestoreState(saved);
	foam->foamProg.format = retFmt;
	foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	symeSetClosure(syme, clos);
	Return(foamCopy(clos));
}


local void
gen0FreeFortranActualArgTmps(void)
{
	FoamList l;

	l = gen0FortranActualArgTmps;
	for (; gen0FortranActualArgTmps; gen0FortranActualArgTmps = cdr(gen0FortranActualArgTmps)) 
		gen0FreeTemp(car(gen0FortranActualArgTmps));
	listFree(Foam)(l);
}


AInt
gen0StdDeclFormat(Length size, String *names, FoamTag *types, AInt *fmts)
{
	Foam	decl, ddecl;
	Length	i;
	
	ddecl = foamNewEmpty(FOAM_DDecl, size + 1);
	ddecl->foamDDecl.usage = FOAM_DDecl_Record;

	for (i = 0; i < size; i += 1) {
		decl = foamNewDecl(types[i], strCopy(names[i]), fmts[i]);
		foamFixed(decl) = true;
		ddecl->foamDDecl.argv[i] = decl;
	}

	return gen0AddRealFormat(ddecl);
}

void 
gen0SetDDeclUsage(AInt fmtNo, FoamDDeclTag usage)
{
	Foam ddecl;

	ddecl = gen0GetRealFormat(gen0FindFormat(fmtNo));
	ddecl->foamDDecl.usage = (AInt) usage;
}

/*
 * Get the foam format for a real format number.
 */
local Foam
gen0GetRealFormat(AInt fmt0)
{
	FoamList	fl;
	AInt		fmt;

	fmt = gen0RealFormatNum - 1;
	for (fl = gen0FormatList; fl; fmt -= 1, fl = cdr(fl))
		if (fmt == fmt0)
			return car(fl);
	assert(false);
	return NULL;
}

/*
 * Add a real format to the list.
 * Use an existing format if available.
 */
local AInt
gen0AddRealFormat(Foam ddecl)
{
	FoamList	fl;
	AInt		fmt;

	/* See if the format is already on the list. */
	fmt = gen0RealFormatNum - 1;
	for (fl = gen0FormatList; fl; fmt -= 1, fl = cdr(fl))
		if (gen0CompareFormats(ddecl, car(fl))) {
			foamFree(ddecl);
			return fmt;
		}

	/* Otherwise just use the one we constructed. */
	gen0FormatList = listCons(Foam)(ddecl, gen0FormatList);
	return gen0RealFormatNum++;
}

/*
 * Compare Foam formats.
 */
local Bool
gen0CompareFormats(Foam dd1, Foam dd2)
{
	Length	i, argc;

	assert(foamTag(dd1) == FOAM_DDecl);
	assert(foamTag(dd2) == FOAM_DDecl);

	if (dd1->foamDDecl.usage != dd2->foamDDecl.usage)
		return false;

	argc = foamDDeclArgc(dd1);
	if (foamDDeclArgc(dd2) != argc)
		return false;

	for (i = 0; i < argc; i += 1) {
		Foam	d1 = dd1->foamDDecl.argv[i];
		Foam	d2 = dd2->foamDDecl.argv[i];
		FoamTag	t1 = d1->foamDecl.type;

		if (t1 != d2->foamDecl.type)
			return false;

		/*
		 * We MUST check signatures of functions
		 * otherwise we get problems with the
		 * foreign Fortran interface.
		 */
		if (t1 == FOAM_Clos)
		{ /* Can't merge if different signatures ... */
			AInt f1 = d1->foamDecl.format;
			AInt f2 = d2->foamDecl.format;
			if (f1 != f2)
				return false;
		}

		if (!strEqual(d1->foamDecl.id, d2->foamDecl.id))
			return false;
	}

	return true;
}

/*****************************************************************************
 *
 * :: Debugging (old style)
 *
 ****************************************************************************/

local Foam	gen0DbgFnExit0	(int, Foam);
local void	gen0DbgAssign0	(int, Syme, Bool);

local void
gen0DbgAssignment(AbSyn lhs)
{
	Syme   id;
	int    lineNo;
	Foam   type;

	if (abTag(lhs) != AB_Id) 
		return;
	assert(gen0DebugWanted);
	id     = abSyme(lhs);
	lineNo = sposLine(abPos(lhs));
	type   = gen0GetDomainLex(symeType(id));

	gen0DbgAssign0(lineNo, id, false);
}

local void
gen0DbgFnEntry(AbSyn fn)
{
	AbSyn  body, params, *argv;
	Syme   self;
	Foam   type;
	String name;
	Bool   inDom;
	int    lineNo, argc, i;

	assert(abIsAnyLambda(fn));
	assert(gen0DebugWanted);

	self   = gen0LocalSelfSyme();
	body   = fn->abLambda.body;
	params = fn->abLambda.param;

	if (self != NULL) {
		type  = gen0Syme(self);
		inDom = true;
	} 
	else {
		type  = foamNewSInt(23);
		inDom = false;
	}

	if (abTag(body) == AB_Label) 
		name = symString(abIdSym(body->abLabel.label));
	else 
		name = "<unknown>";
	
	lineNo = sposLine(abPos(fn));
	gen0DebugIssueStmt(GenDebugFnEntry, name,
			   lineNo, type, foamNewSInt(inDom), 
			   foamNewBool(true));
	
	switch (abTag(params)) {
	  case AB_Nothing:
		argc = 0;
		argv = NULL;
		break;
	  case AB_Comma:
		argc = abArgc(params);
		argv = abArgv(params);
		break;
	  default:
		argc = 1;
		argv = &params;
		break;
	}

	for (i=0; i<argc; i++) {
		Syme syme = abSyme(abDefineeId(argv[i]));
		gen0DbgAssign0(lineNo, syme, true);
	}	
}

local Foam
gen0DbgFnReturn(AbSyn ab, Foam value)
{
	return gen0DbgFnExit0(sposLine(abPos(ab)), value);
}

local Foam
gen0DbgFnExit(AbSyn fn, Foam value)
{
	return gen0DbgFnExit0(sposLine(abEnd(fn)), value);
}

local void
gen0DbgAssign0(int line, Syme syme, Bool isParam)
{
	TForm	tf = symeType(syme);

	/* Ignore types we don't like */
	if (tfSatCat(tf) || tfSatDom(tf) || gen0Type(tf, NULL) != FOAM_Word)
		return;
#if 0
	gen0DebugIssueStmt(GenDebugAssign, 
			   symeString(syme),
			   line,
			   gen0GetDomainLex(tf), 
			   gen0Syme(syme),
			   foamNewBool(isParam));
#endif
}

local Foam
gen0DbgFnExit0(int line, Foam value)
{
	TForm	tf = gen0State->type;
	Foam	type, flag, v;

	/* Ignore types we don't like */
	if (tfIsAnyMap(tf)) tf = tfMapRet(tf);
	if (tfSatCat(tf) || tfSatDom(tf) || gen0Type(tf, NULL) != FOAM_Word ||
	    tfAsMultiArgc(tf) != 1) {
		type  = foamNewSInt(int0);
		flag  = foamNewBool(false);
		gen0DebugIssueStmt(GenDebugFnExit, "",
				   line, type, foamNewSInt(int0), flag);
	} 
	else {
		type  = gen0GetDomainLex(tf);
		flag  = foamNewBool(true);
		v     = gen0TempLocal(FOAM_Word);
		gen0AddStmt(foamNewSet(foamCopy(v), value), NULL);
		gen0DebugIssueStmt(GenDebugFnExit, "",
				   line, type, foamCopy(v), flag);
		value = v;
	}

	return value;
}

/*****************************************************************************
 *
 * :: Debugging (new style)
 *
 ****************************************************************************/

local void
gen1DbgFnEntry(TForm tf, Syme syme, AbSyn fn)
{
	AbSyn	*argv;
	Foam	call, result, exporter, type;
	String	file, name;
	SrcPos	pos;
	int	line, argc, i;
	Bool	inDom = false;
	Syme	self = (Syme)0;
	AbSyn	body = fn->abLambda.body;
	AbSyn	params = fn->abLambda.param;


	/* Safety checks */
	assert(abIsAnyLambda(fn));
	assert(gen0DebuggerWanted);


	/* What is the name of this function? */
	if (abTag(body) == AB_Label)
	{
		AbSyn	label = body->abLabel.label;
		Symbol	sym = abIdSym(label);
		name = symString(sym);
	}
	else 
		name = "<unknown>";


	/* Find % (works if this is a domain constructor) */
	if ((self = gen0LocalSelfSyme()) != NULL)
		inDom = true;


	/*
	 * Cobble together a domain value (or nothing). If we can't
	 * find a domain then we ought to use the library instead.
	 */
	exporter = inDom ? gen0Syme(self) : foamNewSInt(0L);


	/* We cannot reliably compute a type value yet */
	type = foamNewCast(FOAM_Word, foamNewSInt(0L));


	/* Jump to the function body */
	while (abTag(body) == AB_Label)
		body = body->abLabel.expr;


	/*
	 * Find the start of the function. We assume that there are no
	 * nested sequences in the body.
	 */
	if ((abTag(body) == AB_Sequence) && abArgc(body))
		body = abArgv(body)[0];


	/* Get the source code position */
	pos  = abPos(body);
	line = sposLine(pos);
	file = fnameUnparse(sposFile(pos));


	/* Obtain a consistent view of the parameters */
	switch (abTag(params))
	{
	  case AB_Nothing:
		argc = 0;
		argv = NULL;
		break;
	  case AB_Comma:
		argc = abArgc(params);
		argv = abArgv(params);
		break;
	  default:
		argc = 1;
		argv = &params;
		break;
	}


	/* Generate a call to the debugger function-entry hook */
	result = gen0TempLocal(FOAM_Word);
	call = gen0DbgEnter(file, line, exporter, name, type, argc);
	gen0AddStmt(foamNewSet(foamCopy(result), call), body);


	/* Store the function context */
	gen0State->dbgContext = foamCopy(result);


	/* Declare and assign the parameters to this function */
	for (i = 0; i < argc; i++)
	{
		AbSyn	param = abDefineeId(argv[i]);
		Syme	syme = abSyme(param);

		gen1DbgDoParam(file, line, syme, i);
	}


	/* Finally tell the debugger that we are "in" */
	gen0AddStmt(gen0DbgInside(foamCopy(result)), body);
}

local void
gen1DbgFnExit(AbSyn body)
{
	String		file;
	SrcPos		pos;
	int		line;
	Foam		decl;


	/* Do nothing if there is no function context */
	if (!(gen0State->dbgContext)) return;


	/* Obtain a local copy of the function context */
	decl = foamCopy(gen0State->dbgContext);


	/*
	 * Find the end of the function. We assume that there are no
	 * nested sequences in the body.
	 */
	if ((abTag(body) == AB_Sequence) && abArgc(body))
		body = abArgv(body)[abArgc(body) - 1];


	/* Get the source code position. */
	pos  = abPos(body);
	line = sposLine(pos);
	file = fnameUnparse(sposFile(pos));


	/* Generate the debug call statement */
	gen0AddStmt(gen0DbgExit(file, line, decl), body);
}

local void
gen1DbgFnReturn(AbSyn body, TForm tf, Foam value)
{
	String		file;
	SrcPos		pos;
	int		line;
	Foam		decl, type;


	/* Do nothing if there is no function context */
	if (!(gen0State->dbgContext)) return;


	/* Obtain a local copy of the function context */
	decl = foamCopy(gen0State->dbgContext);


	/*
	 * Find the end of the function. We assume that there are no
	 * nested sequences in the body. Note that if we are called
	 * from gen0Return() body is the return statement itself.
	 */
	if ((abTag(body) == AB_Sequence) && abArgc(body))
		body = abArgv(body)[abArgc(body) - 1];


	/* Get the source code position. */
	pos  = abPos(body);
	line = sposLine(pos);
	file = fnameUnparse(sposFile(pos));


	/* Try and get a handle on the type of this value */
	if (tfSatCat(tf) || tfSatDom(tf))
		type = foamNewCast(FOAM_Word, foamNewSInt(0L));
	else if (gen0Type(tf, NULL) != FOAM_Word)
		type = foamNewCast(FOAM_Word, foamNewSInt(0L));
	else if (tfAsMultiArgc(tf) != 1)
		type = foamNewCast(FOAM_Word, foamNewSInt(0L));
	else
		type = gen0GetDomainLex(tf);


	/* Generate the debug call statement */
	gen0AddStmt(gen0DbgReturn(file, line, decl, type, value), body);
}

local void
gen1DbgDoParam(String file, AInt line, Syme syme, AInt pno)
{
	Foam	par;


	/* Do nothing if there is no function context */
	if (!(gen0State->dbgContext)) return;


	/* Which parameter is this? */
	par = foamNewPar(pno);


	/* Generate the debug call */
	gen1DbgDoAssign(file, line, syme, par, (AInt)DbgDepthParam, pno);
}

local void
gen1DbgDoAssign(String file, AInt line, Syme syme, Foam value,
	AInt depth, AInt vno)
{
	Foam		decl, type, call;
	TForm		tf = symeType(syme);
	String		name = symString(symeId(syme));


	/* Do nothing if there is no function context */
	if (!(gen0State->dbgContext)) return;


	/* Obtain a local copy of the function context */
	decl = foamCopy(gen0State->dbgContext);


	/* Try and get a handle on the type of this value */
	if (tfSatCat(tf) || tfSatDom(tf))
		type = foamNewCast(FOAM_Word, foamNewSInt(0L));
	else if (gen0Type(tf, NULL) != FOAM_Word)
		type = foamNewCast(FOAM_Word, foamNewSInt(0L));
	else if (tfAsMultiArgc(tf) != 1)
		type = foamNewCast(FOAM_Word, foamNewSInt(0L));
	else
		type = gen0GetDomainLex(tf);


	/* Create the debug call */
	call = gen0DbgAssign(file, line, decl, name, type, value, depth, vno);
	gen0AddStmt(call, (AbSyn)NULL);
}

local void
gen1DbgFnStep(AbSyn stmt)
{
	String		file;
	SrcPos		pos;
	int		line;
	Foam		decl;


	/* Obtain a local copy of the function context (if any) */
	decl = gen0State->dbgContext;
	decl = decl ? foamCopy(decl) : foamNewSInt(0L);


	/* Get the source code position. */
	pos  = abPos(stmt);
	line = sposLine(pos);
	file = fnameUnparse(sposFile(pos));


	/* Generate the debug call statement */
	gen0AddStmt(gen0DbgStep(file, line, decl), stmt);
}

