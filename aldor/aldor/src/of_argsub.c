/*****************************************************************************
 *
 * of_argsub.c: Perform function-argument substitution.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "of_argsub.h"
#include "store.h"
#include "util.h"
#include "sefo.h"
#include "lib.h"
#include "symbol.h"
#include "strops.h"

/*
 * IMPORTANT: this is code "in progress" so don't link it in yet!
 */

/*
 * argsubUnit() is the exported interface to this optimisation and
 *    is only invoked by clients not by us.
 *
 * agsDo*() are functions to perform argument substitution on a
 *    function or procedure call. They do not recurse.
 *
 * All other ags*() functions are recursive and traverse pieces
 * of FOAM to identify the parts which can be optimised, apply
 * the relevent agsDo* optimising function and clean up afterwards.
 */


/* Local helper functions - these recursively traverse FOAM */
local void agsDDef(Foam);
local void agsProgram(Foam, Length);
local void agsCompound(Foam);

local void agsCast(Foam);
local void agsDef(Foam);
local void agsIf(Foam);
local void agsOCall(Foam);
local void agsReturn(Foam);
local void agsSeq(Foam);
local void agsSet(Foam);
local void agsSelect(Foam);


/* These are the crux of the module */
local void agsTryArgSub(Foam *, String);
local Foam agsSubsProg(Foam, Foam *, Length, String);


/* Local helpers to answer questions about pieces of foam */
local Bool agsParamIsClean(Foam, AInt);
local Bool agsFoamHasParam(Foam, AInt);


/* More local helpers - these perform argument substitutions. */
local Foam agsDoOCall(Foam);
local Foam agsSubsCompound(Foam, Foam *, Length);


/* Last set of helpers which perform a variety of simple tasks */
local void agsAddConstant(Foam, Foam);


/* Globals to keep track of Unit/Prog information */
static struct agsUnitInfoStruct	agsUnitInfo;
static struct agsProgInfoStruct	agsProgInfo;

static AgsUnitInfo agsUnit = &agsUnitInfo;
static AgsProgInfo agsProg = &agsProgInfo;


/* Global lists to keep track of new Prog constants */
static FoamList agsNewConsts  = (FoamList)NULL;
static FoamList agsNewDecls   = (FoamList)NULL;
static AIntList agsNewFormats = (AIntList)NULL;
static Length	agsNumConsts  = 0;


/* Debugging flags */
Bool	agsDebug	= false;
#define agsDEBUG	if (DEBUG(ags))


/*****************************************************************************
 *
 * :: Exported functions
 *
 ****************************************************************************/

/*
 * Perform argument substitutions on the whole unit. For this to work
 * effectively the unit must have been through the optimiser so that
 * arguments to functions are likely to be actual FOAM literals such
 * as (SInt 42). We replace calls of the form:
 *
 *    (Set (Loc 0 x) (OCall SInt (Const 1) (Env 0) (SInt 42)))
 *
 * where
 *
 *    (Def (Const 1 foo)
 *       (Prog 0 0 SInt 4 8195 0 0 0
 *          (DDecl Params (Decl SInt "n" -1 4))
 *          (DDecl Locals)
 *          (DFluid)
 *          (DEnv 4 4)
 *          (Seq (Return (Par 0 n)))))
 *
 * into a call of the form:
 *
 *    (Set (Loc 0 x) (OCall SInt (Const 23) (Env 0)))
 *
 * where (Const 23) is (Const 1) with (Par 0) replaced with (SInt 42):
 *
 *    (Def (Const 23 foo)
 *       (Prog 0 0 SInt 4 8195 0 0 0
 *          (DDecl Params)
 *          (DDecl Locals)
 *          (DFluid)
 *          (DEnv 4 4)
 *          (Seq (Return (SInt 42)))))
 *
 * For such a simple function the next iteration of the optimiser
 * ought to inline this so that we end up with
 *
 *    (Set (Loc 0 x) (SInt 42)))
 *
 * Care needs to be taken in situations where the prog being optimised
 * has statements which treat parameters as local variables. We need
 * to replace any references to parameters used in this way with a
 * local variable instead.
 */
void
argsubUnit(Foam unit)
{
	Length	c;

	agsDEBUG{(void)fprintf(dbOut, "-> argsubUnit\n");}


	/* First note which unit we have ... */
	assert(foamTag(unit) == FOAM_Unit);
	agsUnit->unit	= unit;


	/* Get information about our unit */
	agsUnit->fmtc   = foamArgc(unit->foamUnit.formats);
	agsUnit->fmtv   = unit->foamUnit.formats->foamDFmt.argv;
	agsUnit->gloc   = foamDDeclArgc(foamUnitGlobals(unit));
	agsUnit->glov   = foamUnitGlobals(unit)->foamDDecl.argv;
	agsUnit->constc = c = foamDDeclArgc(foamUnitConstants(unit));
	agsUnit->constv = (Foam *) stoAlloc(OB_Other, c*sizeof(Foam));


	/* Fill in the constant table */
	foamConstvFrFoam(unit, agsUnit->constc, agsUnit->constv);


	/* Initially there are no new constants or formats */
	agsNewConsts  = (FoamList)NULL;
	agsNewDecls   = (FoamList)NULL;
	agsNewFormats = (AIntList)NULL;
	agsNumConsts  = 0;


	/* [debug] Initialise the agsProg structure status */
	agsProg->status = Uninitialised;


	/* Optimise all the Defs in this unit */
	agsDDef(unit->foamUnit.defs);


	/* Do we have to add constants to this unit? */
	if (agsNumConsts)
	{
		agsDEBUG{(void)fprintf(dbOut, "(%d new constants)\n",
				(int)agsNumConsts);}
	}


	/* Clear up after ourselves */
	stoFree(agsUnit->constv);
	agsDEBUG{(void)fprintf(dbOut, "<- argsubUnit\n\n");}
}

/*****************************************************************************
 *
 * :: Local helper functions.
 *
 ****************************************************************************/

/* Walk a DDef so that we can optimise each Def */
local void
agsDDef(Foam defs)
{
	Length	i;

	assert(foamTag(defs) == FOAM_DDef);
	for (i = 0; i < foamArgc(defs); i++)
	{
		Foam def = defs->foamDDef.argv[i];
		Foam rhs = def->foamDef.rhs;
		assert(foamTag(def) == FOAM_Def);
		if (foamTag(rhs) == FOAM_Prog)
			agsProgram(rhs, i);
	}
}


/*
 * Now we're getting closer to the point where we can do
 * some optimisations. We note various information about
 * this Prog and then recurse into its body. This works
 * because Progs can't be nested - if they can then we
 * need to start using fluids.
 */
local void
agsProgram(Foam prog, Length n)
{
	/* Check for unexpected recursion */
	agsDEBUG {
		switch (agsProg->status)
		{
		case Uninitialised:
			break;
		case Initialised:
			fprintf(dbOut, "*** agsProg already initialised\n");
			break;
		default:
			fprintf(dbOut, "*** agsProg is garbage\n");
			break;
		}
	}


	/* First note which prog we have ... */
	assert(foamTag(prog) == FOAM_Prog);
	agsProg->prog   = prog;
	agsProg->cnum   = n;


	/* Note the environment */
	agsProg->levels = prog->foamProg.levels;


	/* Note the locals and parameters */
	agsProg->lexc = 0;
	agsProg->lexv = (AInt *)NULL;
	agsProg->locc = foamDDeclArgc(prog->foamProg.locals);
	agsProg->locv = prog->foamProg.locals->foamDDecl.argv;
#ifdef NEW_FORMATS
	agsProg->parc = foamDDeclArgc(faParamsv[prog->foamProg.params-1]);
	agsProg->parv = (faParamsv[prog->foamProg.params-1])->foamDDecl.argv;
#else
	agsProg->parc = foamDDeclArgc(prog->foamProg.params);
	agsProg->parv = prog->foamProg.params->foamDDecl.argv;
#endif


	/* Lexicals only exist if we have formats */
	if (agsUnit->fmtc)
	{
		AInt fmt = agsProg->levels->foamDEnv.argv[0];
		agsProg->lexc = foamDDeclArgc(agsUnit->fmtv[fmt]);
		agsProg->lexv = agsProg->levels->foamDEnv.argv;
	}


	/* Our structure is now initialised */
	agsDEBUG{agsProg->status = Initialised;}


	/* Now walk over the Prog body */
	agsCompound(prog->foamProg.body);


	/* Mark our structure as uninitialised */
	agsDEBUG{agsProg->status = Uninitialised;}
}


/*
 * This is a generic routine to walk across a piece of FOAM
 * that we might expect to find inside a Prog and optimise
 * function applications by argument substitution. At the
 * moment we only deal with OCalls but the others can be
 * added later.
 */
local void
agsCompound(Foam foam)
{
	/*
	 * We don't need to check every possible foam tag
	 * but it's probably worth it during development.
	 */
	switch (foamTag(foam))
	{
	   /* FOAM_CONTROL nodes that we examine */
	   case FOAM_Return:	agsReturn(foam); break;
	   case FOAM_Cast:	agsCast(foam); break;
	   case FOAM_Set:	agsSet(foam); break;
	   case FOAM_Def:	agsDef(foam); break;
	   case FOAM_If:	agsIf(foam); break;


	   /* FOAM_NARY nodes that we examine */
	   case FOAM_Select:	agsSelect(foam); break;
	   case FOAM_OCall:	agsOCall(foam); break;
	   case FOAM_Seq:	agsSeq(foam); break;


	   /* FOAM_CONTROL nodes that we ignore */
	   case FOAM_NOp:	/* Fall through */
	   case FOAM_BVal:	/* Fall through */
	   case FOAM_Ptr:	/* Fall through */
	   case FOAM_CProg:	/* Fall through */
	   case FOAM_CEnv:	/* Fall through */
	   case FOAM_Loose:	/* Fall through */
	   case FOAM_EEnsure:	/* Fall through */
	   case FOAM_EInfo:	/* Fall through */
	   case FOAM_Kill:	/* Fall through */
	   case FOAM_Free:	/* Fall through */
	   case FOAM_ANew:	/* Fall through */
	   case FOAM_RRNew:	/* Fall through */
	   case FOAM_RRec:	/* Fall through */
	   case FOAM_Clos:	/* Fall through */
	   case FOAM_AElt:	/* Fall through */
	   case FOAM_Goto:	/* Fall through */
	   case FOAM_Throw:	/* Fall through */
	   case FOAM_Catch:	/* Fall through */
	   case FOAM_Protect:	/* Fall through */
	   case FOAM_Unit:	/* Fall through */
	   case FOAM_PushEnv:	/* Fall through */
	   case FOAM_PopEnv:	/* Fall through */
	   case FOAM_MFmt:	/* Fall through */
	   case FOAM_RRFmt:	/* Fall through */


	   /* FOAM_VECTOR nodes that we ignore */
	   case FOAM_Unimp:	/* Fall through */
	   case FOAM_GDecl:	/* Fall through */
	   case FOAM_Decl:	/* Fall through */
	   case FOAM_BInt:	/* Fall through */


	   /* FOAM_INDEX nodes that we ignore */
	   case FOAM_Par:	/* Fall through */
	   case FOAM_Loc:	/* Fall through */
	   case FOAM_Glo:	/* Fall through */
	   case FOAM_Fluid:	/* Fall through */
	   case FOAM_Const:	/* Fall through */
	   case FOAM_Env:	/* Fall through */
	   case FOAM_EEnv:	/* Fall through */
	   case FOAM_RNew:	/* Fall through */
	   case FOAM_PRef:	/* Fall through */
	   case FOAM_TRNew:	/* Fall through */
	   case FOAM_RRElt:	/* Fall through */
	   case FOAM_Label:	/* Fall through */


	   /* FOAM_MULTINT nodes that we ignore */
	   case FOAM_Lex:	/* Fall through */
	   case FOAM_RElt:	/* Fall through */
	   case FOAM_IRElt:	/* Fall through */
	   case FOAM_TRElt:	/* Fall through */
	   case FOAM_EElt:	/* Fall through */
	   case FOAM_CFCall:	/* Fall through */
	   case FOAM_OFCall:	/* Fall through */


	   /* FOAM_NARY nodes that we ignore */
	   case FOAM_DDecl:	/* Fall through */
	   case FOAM_DFluid:	/* Fall through */
	   case FOAM_DEnv:	/* Fall through */
	   case FOAM_DDef:	/* Fall through */
	   case FOAM_DFmt:	/* Fall through */
	   case FOAM_Rec:	/* Fall through */
	   case FOAM_Arr:	/* Fall through */
	   case FOAM_TR:	/* Fall through */
	   case FOAM_PCall:	/* Fall through */
	   case FOAM_BCall:	/* Fall through */
	   case FOAM_CCall:	/* Fall through */
	   case FOAM_Values:	/* Fall through */
	   case FOAM_Prog:	/* Fall through */


	   	/* Just ignore this case */
		break;
	   default :
		/* Missing anything other than FOAM_DATA is a bug */
		if (foamTag(foam) >= FOAM_DATA_LIMIT)
			bug("agsCompound: unrecognised foam");
		break;
	}
}

/*****************************************************************************
 *
 * :: Recursive FOAM traversal functions in alphabetical order
 *
 ****************************************************************************/

local void
agsCast(Foam foam)
{
}


local void
agsDef(Foam foam)
{
}


local void
agsIf(Foam foam)
{
}


local void
agsOCall(Foam foam)
{
}


local void
agsReturn(Foam foam)
{
}


local void
agsSeq(Foam foam)
{
	Length	i;
	Length	n = foamArgc(foam);

	for (i = 0;i < n; i++)
		agsTryArgSub(&(foamArgv(foam)[i].code), "agsSeq");
}


local void
agsSet(Foam foam)
{
	/* Sanity check (a little late perhaps!) */
	assert(foamTag(foam) == FOAM_Set);


	/* Only optimise the RHS for the time being */
	agsTryArgSub(&(foam->foamSet.rhs), "agsSet");
}


local void
agsSelect(Foam foam)
{
}


/*****************************************************************************
 *
 * :: The following calls actually perform the argument substitutions
 *
 ****************************************************************************/

/*
 * Given a pointer to a piece of FOAM, see if we can replace
 * it with a similar piece that has been optimised.
 */
local void
agsTryArgSub(Foam *ptr, String fun)
{
	Foam elt = *ptr;
	Foam new = elt;

	/*
	 * Recurse into the node. This may modify deeper
	 * nodes or leaves but we don't mind.
	 */
	agsCompound(elt);


	/*
	 * Check to see if we have something that
	 * can have its arguments substituted.
	 */
	switch (foamTag(elt))
	{
	   case FOAM_OCall:
		new = agsDoOCall(elt);
		break;
	   default:
		break;
	}


	/* Did a substitution take place? */
	if (new != elt)
	{
		/* Yes - modify the FOAM */
		/* Show what happened */
		agsDEBUG {
			(void)fprintf(dbOut, "** %s substitution\n", fun);
			(void)fprintf(dbOut, ">>\n");
			foamPrintDb(elt);
			(void)fprintf(dbOut, "<<\n");
			foamPrintDb(new);
			(void)fprintf(dbOut, "\n");
		}

		foamFreeNode(elt);
		*ptr = new;
	}
}


/*
 * Given an OCall, perform any argument substitutions that are
 * possible and allowed. We return a new OCall to replace the
 * original one. Note that we don't recurse into the OCall as
 * this has already been done at a higher level.
 */
local Foam
agsDoOCall(Foam foam)
{
	Foam	op, body, new;
	Length	i, argc, rargc, nargc;
	Syme	opSyme;
	UShort	cnum;
	Bool	isLocal;
	Foam	*sigma;


	/* Safety check */
	assert(foamTag(foam) == FOAM_OCall);


	/* What do we have here? */
	argc = foamArgc(foam);
	op   = foam->foamOCall.op;


	/* Get the symbol meaning associated with op */
	opSyme = foamSyme(op);
	if (!opSyme)
		return foam;


	/* We can only handle constants */
	if (!genHasConstNum(opSyme))
		return foam;


	/* Get the constant number for this operator */
	cnum = genGetConstNum(opSyme);


	/* Is this a local or a library operator? */
	isLocal = symeIsLocalConst(opSyme);
	if (isLocal)
	{
		assert(cnum < agsUnit->constc);
		body = agsUnit->constv[cnum];
	}
	else
	{
		Lib origin = symeConstLib(opSyme);
		body =  libGetFoamConstant(origin, cnum);
	}


	/* More sanity checks */
	assert(body);
	assert(foamTag(body) == FOAM_Prog);


	/* Display what we've got here */
	agsDEBUG {
		(void)fprintf(dbOut,"** agsDoOCall: ");
		(void)symePrintDb(opSyme);

		for (i = 3;i < argc;i++)
		{
			Foam arg = foamArgv(foam)[i].code;
			Syme syme = foamSyme(arg);

			(void)fprintf(dbOut,"     Arg: ");
			if (syme)
				(void)symePrintDb(syme);
			else
				(void)foamPrintDb(arg);
		}
		(void)fprintf(dbOut,"\n");
	}


	/* How many arguments are we giving this function? */
	rargc = argc - 3;
	assert(rargc == agsProg->parc);


	/*
	 * Decide which arguments need changing. We do this
	 * by creating an array of substitutions, with one
	 * slot for each parameter in order. Parameters which
	 * are not substituted are initialised to their actual
	 * foam par number.
	 */
	sigma = (Foam *)stoAlloc(OB_Other, rargc*sizeof(Foam));
	nargc = 0;
	for (i = 3; i < argc; i++)
	{
		Foam nocast;
		Foam arg = foamArgv(foam)[i].code;

		/* Strip off any cast */
		if (foamTag(arg) == FOAM_Cast)
			nocast = arg->foamCast.expr;
		else
			nocast = arg;


		/* Can we do any substitutions? */
		if (foamTag(nocast) == FOAM_SInt)
		{
			/* Only if this param is clean */
			if (agsParamIsClean(body, i-3))
			{
				sigma[i-3] = arg;
				continue;
			}
		}


		/* Remap the parameter */
		sigma[i-3] = foamNewPar(nargc);
		nargc++;
	}

	agsDEBUG {
		for (i = 0; i < rargc; i++)
		{
			(void)fprintf(dbOut, "     [%d] = ", (int)i);
			foamPrintDb(sigma[i]);
		}
	}


	/*
	 * Do the substitution - this gives us a new function
	 * to call which we need to add to the unit and then
	 * call correctly using the new signature.
	 */
	new = agsSubsProg(body, sigma, rargc, symString(symeId(opSyme)));


	/* Clean up after ourselves */
	stoFree(sigma);
	return foam;
}


/*
 * Return `true' if the specified parameter never appears on the LHS
 * of a set/assignment otherwise return `false'. Clean parameters
 * are those which aren't used as local variables.
 */
local Bool
agsParamIsClean(Foam body, AInt param)
{
	assert(body);


	/* Base case - we are looking for a foamSet node */
	if (foamTag(body) == FOAM_Set)
	{
		/*
		 * Does this foamPar appear on the LHS of the
		 * foamSet. If it does then it is probably being
		 * used as a local variable which is naughty.
		 */
		if (agsFoamHasParam(body->foamSet.lhs, param))
			return false;
	}


	/* Check each sub-node */
	foamIter(body, arg, 
	{
		if (!agsParamIsClean(*arg, param))
			return false;
	});

	return true;
}


/*
 * Returns `true' if the foam contains a reference to the
 * parameter `param'. This is probably too restrictive since
 * it may be possible for a (Par n) to appear on the LHS of
 * a foamSet and not be modified. However, the optimiser is
 * probably going to flatten the LHS anyway and if the node
 * is read-only then we will get to optimise it next time.
 */
local Bool
agsFoamHasParam(Foam foam, AInt param)
{
	assert(foam);


	/* Base case - we want a foamPar */
	if (foamTag(foam) == FOAM_Par)
		return (foam->foamPar.index == param);


	/* Check each sub-node */
	foamIter(foam, arg,
	{
		if (agsFoamHasParam(*arg, param))
			return true;
	});

	return false;
}


local Foam
agsSubsProg(Foam foam, Foam *sigma, Length argc, String name)
{
	Foam	prog, decl;
	Foam	*parv;
	Length	parc;


	/* Pull out the list of function parameters */
#ifdef NEW_FORMATS
	parc = foamDDeclArgc(faParamsv[foam->foamProg.params-1]);
	parv = (faParamsv[foam->foamProg.params-1])->foamDDecl.argv;
#else
	parc = foamDDeclArgc(foam->foamProg.params);
	parv = foam->foamProg.params->foamDDecl.argv;
#endif


	/* Create a new prog */
#ifdef NEW_FORMATS
	prog = foamNewProg
		(
			/* Leading fields accessed anonymously */
			foam->foamGen.argv[0].data,	/* endOffset */
			foam->foamGen.argv[1].data,	/* nLabels */
			foam->foamGen.argv[2].data,	/* retType */
			foam->foamGen.argv[3].data,	/* format */
			foam->foamGen.argv[4].data,	/* infoBits */
			emptyFormatSlot,		/* (AInt)params */
			foamCopy(foam->foamProg.locals),
			foamCopy(foam->foamProg.fluids),
			foamCopy(foam->foamProg.levels),
			NULL				/* body */
		);
#else
	prog = foamNewProg
		(
			/* Leading fields accessed anonymously */
			foam->foamGen.argv[0].data,	/* endOffset */
			foam->foamGen.argv[1].data,	/* nLabels */
			foam->foamGen.argv[2].data,	/* retType */
			foam->foamGen.argv[3].data,	/* format */
			foam->foamGen.argv[4].data,	/* infoBits */
			NULL,				/* (Foam)params */
			foamCopy(foam->foamProg.locals),
			foamCopy(foam->foamProg.fluids),
			foamCopy(foam->foamProg.levels),
			NULL				/* body */
		);
#endif


	/* Create a declaration for this prog */
	decl = foamNewDecl(FOAM_Prog, strCopy(name), emptyFormatSlot);


	/* Add this prog to the list of new constants */
	agsAddConstant(prog, decl);


	/* Perform the substitutions on the function body. */
	prog->foamProg.body = agsSubsCompound(foam->foamProg.body, sigma, argc);


	/* Patch up the parameter list */
	/* prog->params = ... */
	prog->foamProg.params = foam->foamProg.params; /* WRONG!!!! */


	/* Perhaps we ought to sort out the size and execution times? */


	/* Return the new const */
	return prog;
}


/*
 * Copy a piece of foam substituting each (Par i) with sigma[i]
 */
local Foam
agsSubsCompound(Foam foam, Foam *sigma, Length argc)
{
	Foam	new;

	assert(foam);


	/* Create a new node sharing original storage */
	new = foamCopyNode(foam);


	/* Base case - deal with foamPar nodes */
	if (foamTag(foam) == FOAM_Par)
	{
		assert(foam->foamPar.index < argc);
		return foamCopy(sigma[foam->foamPar.index]);
	}


	/* Recurse into nodes overwriting shared storage */
	foamIter(foam, arg,
	{
		*arg = agsSubsCompound(*arg, sigma, argc);
	});


	/* Return the modified foam */
	return new;
}


local void
agsAddConstant(Foam foam, Foam decl)
{
	agsNewConsts  = listCons(Foam)(foam, agsNewConsts);
	agsNewDecls   = listCons(Foam)(decl, agsNewConsts);
	agsNumConsts++;
}


