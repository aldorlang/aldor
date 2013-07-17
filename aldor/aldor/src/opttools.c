/*****************************************************************************
 *
 * opttools.c: Generic Optimization Tools
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
/****************************************************************************
 *            ----- GUIDE to the USE of OPTIMIZATION TOOLS -----
 ************************************
 * Index:
 *   1. Variables Information
 *   2. Side Effects
 * 
 ************************************
 * 1. Variables Information  (24 May 1994)
 *
 * A common problem writing optimizations (and not only) is keeping some kind
 * of information associated to each variable.
 * In example, in the implementation of comm. subexpr. elim. there is the
 * necessity of keeping, for each var, the list of all expr in which this
 * var occurs.
 *
 * This package has been tought for local optimizations; therefore the way of
 * recording this information is done using vectors for locals and params
 * (more common) and lists for lexicals and globals (less common).
 * Fluids are unhandled. No optimization so far works with fluids.
 *
 * The package, for genericity, provide a way to associate to each variable a
 * single information or a list of data.
 *
 * INTERFACE:
 *
 *	otProgInfoInit(..)
 *	otProgInfoFini()
 *
 *	otAddVarInfo(..)
 *	otSetVarInfo(..)
 *	otGetVarInfo(..)
 *	otPrintVarAssociations(..)
 *
 *
 * 1.1 List Associations
 *   Ex: (loc 0) -> VarInfoList
 *
 * First of all the initializator must be called FOR EACH PROG:
 *
 *	otProgInfoInit(OT_ASSOCIATION_LIST, <num locals>, <num parameters>);
 *
 * The first parameter says that we need to keep a list for each var.
 * Note that the number of globals/lexicals is not required, since for them
 * we use lists.
 *
 * REMEMBER to call otProgInfoFini() at the end.
 *
 * The type of info associated with the var is here VarInfo, that is a union
 * of type used in all the optimizations. If you need a different type, add
 * it to the union.
 * Follow an example of use of the other functions (for comex.c):
 *
 *     expInfo = ...;
 *     var     = <loc 0>;
 *     otAddVarInfo(expInfo, var);
 *
 *     expInfo = ...;
 *     var     = <loc 3>;
 *     otAddVarInfo(expInfo, var);
 *
 *     expInfo = ...;
 *     var     = <glo 2>;
 *     otAddVarInfo(expInfo, var);
 *
 *     var     = <loc 0>;
 *     expInfo = otGetVarInfo(var)->expInfo;
 *
 * 1.2 Single Associations
 *
 *  Very similar to 1.1. The only differences are:
 *	- in the initialization OT_ASSOCIATION_SINGLE must be used;
 *	- otSetVarInfo must be used instead of otAddVarInfo
 *
 * 1.3 Vector Associations
 *
 * This means that a vector is associated for each var.
 * UNIMPLEMENTED: write it, if you need.
 * 
 ************************************
 * DEFINITIONS:
 *  These are some definitions that are often used in the
 * comments for the optimization files. If you work on optimizations,
 * LEARN (and add, change, ...) them !
 *
 * NOTE: IF THE FOAM LANGUAGE IS CHANGED, these definitions and all the
 * procedures affected must be changed, otherwise optimizations will not work
 * properly. The idea of keeping many common tests and operations in this file
 * is to get as assumptions as possible about the semantic of FOAM language
 * in a unique file.
 *
 * ----- Variables
 *
 * =) A variable is LOCAL if is or a parameter or a local.
 * =) A NOT LOCAL variable is or a global or a lexical.
 *
 * NOTE: If a variable isn't LOCAL, this =/=> is NOT LOCAL, because it may
 * 	be, in example, a fluid var. or another kind of future variable.
 *	Generally LOCAL and NOT LOCAL variables are the one used for 
 *	optimizations.
 *
 * ----- Definitions
 * 
 * =) A DEFINITION is or a (Def ..) or a (Set ..) statement.
 *
 * ----- Data values
 *
 * =) A value is LITERAL if it's value is indipendent from its position in
 *	the Prog. In other work, the value doesn't change during the execution.
 *	Examples: (SInt 3) is a literal. (Env 0) is a literal.
 * 
 * 
 ****************************************************************************/

#include "genfoam.h"
#include "gf_rtime.h"
#include "opttools.h"
#include "store.h"

CREATE_LIST(VarInfo);


struct _NotLocalVarAssoc {

	Foam		var;
	AssociationType	info;
};

typedef struct _NotLocalVarAssoc * NotLocalVarAssoc;

DECLARE_LIST(NotLocalVarAssoc);
CREATE_LIST(NotLocalVarAssoc);

/*****************************************************************************
 *
 * :: Local Prototypes
 *
 ****************************************************************************/

local AssociationType	otGetAssociationFrNotLocalVar	(Foam);
local void		otAddVarInfoToNotLocalVar	(VarInfo, Foam);
local void		otSetVarInfoToExternVar		(VarInfo, Foam);

/*****************************************************************************
 *
 * :: otGetVarInfo and otSetVarInfo
 *
 * SEE THE TOP OF THE FILE
 *
 ****************************************************************************/

/* otGetVarInfo and otSetVarInfo provide a mechanism useful in many 
 * optimizations, i.e. mapping from variables to some kind of information
 * associated to the variable.
 *
 * Example: in comm. subexpr. elim. there is the necessity of keeping track,
 *	for each var V, of all the expressions in which V appears.
 *
 * Current Implementation: These procedures are usefull for local,
 *	optimizations, where generally
 *		(numLocals + numParams >> numLex + numGlo)
 *	Therefore we use vectors for locals and params and lists for globals
 *	and lexicals.
 */

static Bool	otProgInfoInitialized = false;	/* try to catch bugs */

CREATE_LIST(AssociationType);

static struct  {

	UShort			mode; /* OT_ASSOCIATION_...(see include file)*/

	Foam			unit;
	Foam			* globv; /* globals declarations */

	int			numLocs;
	int			numPars;

	AssociationType *	locv;
	AssociationType	*	parv;

	NotLocalVarAssocList	glol;
	NotLocalVarAssocList	lexl;

} otProgInfo;



/* Return NULL if there is no VarInfo for VAR */
VarInfoList
otGetVarInfoList(Foam var)
{
	assert(otProgInfoInitialized);

	if (foamTag(var) == FOAM_Loc) {
		assert(var->foamLoc.index < otProgInfo.numLocs);
		return (VarInfoList) otProgInfo.locv[var->foamLoc.index];
	}
	else if (foamTag(var) == FOAM_Par) {
		assert(var->foamPar.index < otProgInfo.numPars);
		return (VarInfoList) otProgInfo.parv[var->foamPar.index];
	}
	else if (foamTag(var) == FOAM_Glo || foamTag(var) == FOAM_Lex)
		return (VarInfoList) otGetAssociationFrNotLocalVar(var);

	return listNil(VarInfo);

}


/* NOTE: The client can use the macro otAddVarInfo, which perform the cast.
 * Must be used if the info is a list */
void
otAddVarInfo0(VarInfo varInfo, Foam var)
{
	VarInfoList * p;

	assert(otProgInfoInitialized);

	if (foamTag(var) == FOAM_Loc) {
		assert(var->foamLoc.index < otProgInfo.numLocs);
		p = (VarInfoList *) otProgInfo.locv + var->foamLoc.index;
	}
	else if (foamTag(var) == FOAM_Par) {
		assert(var->foamPar.index < otProgInfo.numPars);
		p = (VarInfoList *) otProgInfo.parv + var->foamPar.index;
	}
	else if (foamTag(var) == FOAM_Glo || foamTag(var) == FOAM_Lex) {
		otAddVarInfoToNotLocalVar(varInfo, var);
		return;
	}
	else {
		bug("otAddVarInfo: VAR is not an expected variable...");
		NotReached(p = NULL);
	}

	if (!listMemq(VarInfo)(*p, varInfo))
		listPush(VarInfo, varInfo, *p);
}



/* NOTE: The client can use the macro otSetVarInfo, which perform the cast.
 * Must be used if the info is not a list */
void
otSetVarInfo0(VarInfo varInfo, Foam var)
{
	assert(otProgInfoInitialized);

	if (foamTag(var) == FOAM_Loc) {
		assert(var->foamLoc.index < otProgInfo.numLocs);
		otProgInfo.locv[var->foamLoc.index] = (Pointer) varInfo;
	}
	else if (foamTag(var) == FOAM_Par) {
		assert(var->foamPar.index < otProgInfo.numPars);
		otProgInfo.parv[var->foamPar.index] = (Pointer) varInfo;
	}
	else if (foamTag(var) == FOAM_Glo || foamTag(var) == FOAM_Lex)
		otSetVarInfoToExternVar(varInfo, var);
	else
		bug("otSetVarInfo: VAR is not an expected variable...");

	return;

}

/*****************************************************************************
 *
 * :: Init / Fini
 *
 * SEE THE TOP OF THE FILE
 *
 ****************************************************************************/


/* MODE:
 * OT_ASSOCIATION_SINGLE -> each var is associated to a single element
 * OT_ASSOCIATION_LIST   -> each var is associated to a list
 * (Eventually add:
 * OT_ASSOCIATION_VECTOR -> each var is associated to a vector
 *
 * NOTE: "unit" may be NULL; in this case some info (ex: globals declarations)
 *   aren't available (you get a bug if you try to use procedures that need
 *     this info; each proc. that needs this info must declare this in its
 *     header).
 */

void
otProgInfoInit(UShort mode, int numLocs, int numPars, Foam unit)
{
	int i;

	if (mode & OT_ASSOCIATION_SINGLE)
		otProgInfo.mode = OT_ASSOCIATION_SINGLE;
	else if (mode & OT_ASSOCIATION_LIST)
		otProgInfo.mode = OT_ASSOCIATION_LIST;
	else if (mode & OT_ASSOCIATION_VECTOR) {
		otProgInfo.mode = OT_ASSOCIATION_VECTOR;
		bug("otProgInfoInit: the ASSOCIATION_VECTOR has been never used so far, so is unimplemented. You are free to add it...");
	}
	else
		bug("otProgInfoInit: unknown mode request...");

	otProgInfoInitialized = true;

	otProgInfo.numLocs = numLocs;
	otProgInfo.numPars = numPars;

	if (numLocs)
		otProgInfo.locv = (AssociationType *) 
			stoAlloc(OB_Other, numLocs * sizeof(AssociationType));
	else
		otProgInfo.locv = (AssociationType *) int0;

	if (numPars)
		otProgInfo.parv = (AssociationType *) 
			stoAlloc(OB_Other, numPars * sizeof(AssociationType));
	else
		otProgInfo.parv = (AssociationType *) int0;

	for (i = 0; i < numLocs; i++)
		otProgInfo.locv[i] = (AssociationType) int0;

	for (i = 0; i < numPars; i++)
		otProgInfo.parv[i] = (AssociationType) int0;

	otProgInfo.glol = listNil(NotLocalVarAssoc);
	otProgInfo.lexl = listNil(NotLocalVarAssoc);

	if (unit) {
		otProgInfo.unit = unit;
		otProgInfo.globv = foamUnitGlobals(unit)->foamDDecl.argv;
	}
	else {
		otProgInfo.unit = NULL;
		otProgInfo.globv = NULL;
	}
}


void
otProgInfoFini()
{
	int i;

	assert(otProgInfoInitialized);

	otProgInfoInitialized = false;

	if (otProgInfo.mode == OT_ASSOCIATION_LIST) {
		for (i = 0; i < otProgInfo.numLocs; i++)
			listFree(VarInfo)((VarInfoList) otProgInfo.locv[i]);
		for (i = 0; i < otProgInfo.numPars; i++)
			listFree(VarInfo)((VarInfoList) otProgInfo.parv[i]);
	}

	if (otProgInfo.locv) stoFree(otProgInfo.locv);
	if (otProgInfo.parv) stoFree(otProgInfo.parv);

	listFree(NotLocalVarAssoc)(otProgInfo.glol);
	listFree(NotLocalVarAssoc)(otProgInfo.lexl);
}

/*****************************************************************************
 *
 * :: Print
 *
 ****************************************************************************/

void
otPrintVarAssociations(VarInfoPrintFn vprint)
{
	int i, mode = otProgInfo.mode;

	assert(otProgInfoInitialized);

	fprintf(dbOut, "Variables associations:\n(type is ");

	if (mode == OT_ASSOCIATION_LIST)
		fprintf(dbOut, "LIST)\n");
	else if (mode == OT_ASSOCIATION_SINGLE)
		fprintf(dbOut, "SINGLE)\n");

	for (i = 0; i < otProgInfo.numLocs; i++) {
		if (mode == OT_ASSOCIATION_LIST &&
		    otProgInfo.locv[i]) {

			fprintf(dbOut, "** Loc %d :", i);

			listIter(VarInfo, vi,(VarInfoList)otProgInfo.locv[i], {
				vprint(vi);
			});
		}
		else if (mode == OT_ASSOCIATION_SINGLE) {
			fprintf(dbOut, "** Loc %d :", i);
			vprint((VarInfo) otProgInfo.locv[i]);
		}

	}

	for (i = 0; i < otProgInfo.numPars; i++) {
		if (mode == OT_ASSOCIATION_LIST &&
   		    otProgInfo.parv[i]) {

			fprintf(dbOut, "** Par %d :", i);

			listIter(VarInfo, vi,(VarInfoList)otProgInfo.parv[i], {
				vprint(vi);
			});

			fprintf(dbOut, "\n");
		}
		else if (mode == OT_ASSOCIATION_SINGLE &&
			 otProgInfo.parv[i]) {

			fprintf(dbOut, "** Par %d :", i);

			vprint((VarInfo) otProgInfo.parv[i]);

			fprintf(dbOut, "\n");

		}
	}

	listIter(NotLocalVarAssoc, ass, otProgInfo.glol, {

		fprintf(dbOut, "** Glo %d :", (int)ass->var->foamGlo.index);

		if (mode == OT_ASSOCIATION_LIST) {

			listIter(VarInfo, vi, (VarInfoList) ass->info, {
				vprint(vi);
			});
		}
		else if (mode == OT_ASSOCIATION_SINGLE)
			vprint((VarInfo) ass->info);

		fprintf(dbOut, "\n");
	});

	listIter(NotLocalVarAssoc, ass, otProgInfo.lexl, {

		fprintf(dbOut, "** Lex %d %d :",
			(int)ass->var->foamLex.level,
			(int)ass->var->foamLex.index);

		if (mode == OT_ASSOCIATION_LIST) {

			listIter(VarInfo, vi, (VarInfoList) ass->info, {
				vprint(vi);
			});
		}
		else if (mode == OT_ASSOCIATION_SINGLE)
			vprint((VarInfo) ass->info);

		fprintf(dbOut, "\n");

	});
}

/*****************************************************************************
 *
 * :: otIsMovableData
 *
 ****************************************************************************/

Bool
otIsMovableData(Foam foam)
{
	Bool	result;

	otDereferenceCast(foam);

	result = (foamTag(foam) == FOAM_SInt ||
		  foamTag(foam) == FOAM_Char ||
		  foamTag(foam) == FOAM_Bool ||
		  foamTag(foam) == FOAM_Byte ||
		  foamTag(foam) == FOAM_HInt ||
		  foamTag(foam) == FOAM_SFlo ||
		  foamTag(foam) == FOAM_DFlo ||
		  foamTag(foam) == FOAM_Env  ||
		  foamTag(foam) == FOAM_Const);

	return result;
}

/*****************************************************************************
 *
 * :: Utility
 *
 ****************************************************************************/

local AssociationType
otGetAssociationFrNotLocalVar(Foam var)
{
	NotLocalVarAssocList	assocl;

	assocl = (foamTag(var) == FOAM_Lex ?
			 otProgInfo.lexl : otProgInfo.glol);

	listIter(NotLocalVarAssoc, assoc, assocl, {

		if (foamEqual(var, assoc->var))
			return assoc->info;

	});

	return (AssociationType) int0;
}


local void
otAddVarInfoToNotLocalVar(VarInfo varInfo, Foam var)
{
	NotLocalVarAssocList	assocl;
	AssociationType		ass = (AssociationType) NULL;
#if EDIT_1_0_n1_07
	NotLocalVarAssoc	pair = (NotLocalVarAssoc) NULL;
#else
	NotLocalVarAssoc	pair;
#endif

	assocl = (foamTag(var) == FOAM_Lex ?
			 otProgInfo.lexl : otProgInfo.glol);

	listIter(NotLocalVarAssoc, assoc, assocl, {

		 if (foamEqual(var, assoc->var)) {
		 	ass  = assoc->info;
			pair = assoc;
		     	break;
		}

	 });

	if (!ass) {
		NotLocalVarAssoc assoc =
			(NotLocalVarAssoc) stoAlloc(OB_Other, sizeof(*assoc));

		NotLocalVarAssocList * pl =(foamTag(var) == FOAM_Lex ?
			  		&(otProgInfo.lexl) :
					&(otProgInfo.glol));

		assoc->var = var;
		assoc->info = (Pointer) listNil(VarInfo);

		listPush(NotLocalVarAssoc, assoc, *pl);
		assoc->info = (AssociationType) 
			listCons(VarInfo)(varInfo, (VarInfoList) assoc->info);
	}
	else if (!listMemq(VarInfo)((VarInfoList) ass, varInfo)) {

		assert(ass);

		ass = (AssociationType)
			listCons(VarInfo)(varInfo, (VarInfoList) ass);
		/*
		 * This next statement is really quite vital although
		 * commenting it out probably won't affect the way
		 * that the majority of Aldor programs will be built.
		 * If you find that it causes problems when building
		 * the compiler then find a work-around which achieves
		 * the same effect or get yourself better C compiler ;)
		 *
		 * Here's the original comment ...
		 *
		 *Logically necessary, but makes rutime barf
		 */
		 pair->info = ass;

		/*
		 * Just for reference, if the statement above has been
		 * commented out then the following program will not
		 * work correctly after inlining. Compare the output of
		 * -Q1 to -Q2 for an example.
		 *
		 * ----------------------- CUT HERE ----------------------
		 * -- A function that ought to always return false
		 * foo():Boolean == {
		 *    import from SingleInteger;
		 *    for i in 1..10 repeat {
		 *       if (i = 5) then return false;
		 *    }
		 *    return true;
		 * }
		 *
		 * -- A function that ought to always return an empty list
		 * bar():List(SingleInteger) == [i for i in 1..10 | foo()];
		 * ----------------------- CUT HERE ----------------------
		 *
		 * If you try this you'll see that bar() returns the
		 * empty list with -Q1 (correct) and a non-empty list
		 * with -Q2 (incorrect). Forcing foo() to have a single
		 * exit by breaking from its for loop fixes the bug.
		 */
	}
		
}

local void
otSetVarInfoToExternVar(VarInfo varInfo, Foam var)
{}


/******************************************************************************
 *
 * :: otIsForcer
 *
 *****************************************************************************/

/* Called on the op of a CCall. Return true if op is a lazyGetExport call.
 * NOTE: unit info MUST be available.
 */
Bool
otIsForcer(Foam op)
{
	RuntimeCallInfo	info;
	Foam		decl;

	assert(otProgInfo.unit);
	
	if (foamTag(op) != FOAM_Glo)
		return false;

	assert(otProgInfo.globv);

	decl = otProgInfo.globv[op->foamGlo.index];

	info = gen0GetRuntimeCallInfo(decl);
	return rtCallIsForce(info);
}
