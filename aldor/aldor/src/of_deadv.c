/*****************************************************************************
 *
 * of_deadv.c: Dead variable elimination.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/* This file contins code to eliminate dead locals, lexicals and constants.
 *
 * The program walks the unit in top down order, starting with the file
 * initialization program, and mark all defintions and usages.  If
 * a variable exists in a lexical or local environment and is not touched
 * in this process, it is eliminated.  If an environment comes from a
 * foreign unit, we never eliminate from it.
 * 
 * Forcing environment references are ignored for deadvar purposes.
 * Any statement containing an EInfo instruction is assumed to be forcing.
 *
 * To Do:
 *	Remove all empty formats.  It currently leaves empty formats in
 *		the foam.  There should be only one empty format in the empty
 *		format slot.
 *	Compute side-effects all at once after inlining, rather than on
 *		an as-needed basis.
 */

#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "genfoam.h"
#include "gf_rtime.h"
#include "optfoam.h"
#include "store.h"
#include "fint.h"

Bool	dvDebug		= false;

#define	dvDEBUG(s)	DEBUG_IF(dvDebug, s)

typedef DvUsage *DvUsagePtr;

local void	dvSetupUnit		(Foam unit);
local DvUsage	dvMakeUsageVec		(Foam ddecl);
local void	dvMarkUnitUsage		(Foam unit);
local void	dvMarkProgUsage		(int, Foam prog);
local void	dvMarkExprUsage		(Foam expr);
local void	dvMarkDefined		(Foam lhs, Foam rhs);
local void	dvMarkEInfo		(Foam foam);
local Bool	dvHasSideEffect		(Foam expr);
local void	dvHasSideEffect1	(Foam expr);
local Bool	dvOpHasSideEffect	(Foam op);
local Foam	dvIsForcingStmt		(Foam foam);
local void	dvMarkConstProg		(int index);
local void	dvMarkWholeFormat	(int format);
local void	dvMarkEEltFormat	(Foam eelt);
local void	dvMarkDefinedValues	(int format);
local void	dvElimUnused		(Foam unit);
local void	dvComputeRenumbering	(Foam unit);
local void	dvCollapseFormats	(Foam unit);
local void	dvCollapseDefs		(Foam unit);
local Foam	dvCollapseFormat	(DvUsage usage, Foam ddecl);
local void	dvDeleteEmptyFormats	(Foam);
local void	dvRenumberProgs		(Foam unit);
local void	dvRenumberProg		(Foam prog);
local UsageState dvGetUsage		(Foam foam);
local UsageState dvGetForcingUsage	(Foam expr);
local Foam	dvRenumberStmt		(Foam stmt);
local Foam	dvRenumberExpr		(Foam expr);
local Foam	dvReplaceAssignment	(Foam lhs, Foam rhs, Foam ass);
local void	dvElimUnusedFormats	(Foam unit);
local void	dvRemoveNops		(Foam ufoam);
local void	dvRemoveSeqNops		(Foam seq);

Bool dvChanged;

DvUsage		*dvFormatUsage;
DvUsage		dvLocals;
Bool 		*dvKillEnsures;
Foam		*dvDefs;
Foam		*dvFormats;
Foam		dvDEnv;
Foam		dvLocalsDecl;
int		dvGeneration;
int		dvThisConst;

#define dvNewIndex(format, index) (dvFormatUsage[format][index].newIndex)
#define dvUsage(format, index) (dvFormatUsage[format][index].used)

#define dvMarkFormat(format, index, val) 			 \
	Statement(if (dvUsage(format,index)  < (val) )	 \
		  dvUsage(format,index) = (val))

#define DvMaxPasses	10

void
dvInit()
{
	dvGeneration = 0;
}

/*****************************************************************************
 *
 * :: Dead Variable setup.
 *
 ****************************************************************************/

/*
 * Eliminate all dead gloabls, fluids, constants, local, and lexical
 * variables from a foam unit.  It works by succesively removing dead
 * variable until no more can be removed. 
 */
void
dvElim(Foam unit)
{
	int	count = 0;
	assert(foamTag(unit) == FOAM_Unit);

	if (!optIsDeadVarWanted()) return;

	assert(foamAudit(unit));

	do {
		dvGeneration++;
		dvChanged = false;
		dvSetupUnit(unit);
		dvMarkUnitUsage(unit);
		dvElimUnused(unit);
		count++;

		dvDEBUG({
			fprintf(dbOut, "<<dvUnit[%d]:\n", count);
			foamPrint(dbOut, unit);
			fnewline(dbOut);
		});
	}  while (dvChanged && count < DvMaxPasses);

	dvRemoveNops(unit);
	assert(foamAudit(unit));
}

/*
 * Prepare a unit for dead variable elimination.
 */
local void
dvSetupUnit(Foam unit)
{
	int	i, nFormats;
	Foam	*argv;

	/* build dv info structs for each format */

	dvFormats = argv = unit->foamUnit.formats->foamDFmt.argv;
	nFormats = foamArgc(unit->foamUnit.formats);
	dvFormatUsage = (DvUsage *)
		stoAlloc(OB_Other, nFormats*sizeof(DvUsage));
	
	for(i=0; i<nFormats; i++) dvFormatUsage[i] = dvMakeUsageVec(argv[i]);
	
	dvKillEnsures = (Bool*) stoAlloc(OB_Other, 
				 sizeof(Bool)*foamArgc(unit->foamUnit.defs));
	
	for (i=0;  i< foamArgc(unit->foamUnit.defs); i++) 
		dvKillEnsures[i] = true;

	/* mark progs as unchecked */
	dvDefs = argv = unit->foamUnit.defs->foamDDef.argv;
	for(i=0; i< foamArgc(unit->foamUnit.defs); i++) {
		if (foamTag(argv[i]) == FOAM_Def &&
		    foamTag(argv[i]->foamDef.rhs) == FOAM_Prog) {
			Foam	prog = argv[i]->foamDef.rhs;
			assert (foamOptInfo(prog) != 0);
			foamOptInfo(prog)->dvState = DV_NotChecked;
		}
	}

	/* mark exported constants as used */
	for(i=0; i< foamArgc(unit->foamUnit.defs); i++) {
		Foam	def = argv[i];
		if (foamTag(def) == FOAM_Def) {
			Foam	lhs = def->foamDef.lhs;
			Foam	rhs = def->foamDef.rhs;
			if (foamTag(lhs) == FOAM_Const &&
			    foamTag(rhs) != FOAM_Prog)
				dvMarkFormat(constsSlot,
					     lhs->foamConst.index, DV_Used);
		}
	}

	for (i=0; i<foamDDeclArgc(dvFormats[globalsSlot]); i++) {
		Foam gdecl = dvFormats[globalsSlot]->foamDDecl.argv[i];
		/* Mark "include" globals as used */
		if (gdecl->foamGDecl.protocol == FOAM_Proto_Include)
			dvMarkFormat(globalsSlot, i, DV_Used);
		/* Mark "exported" globals as used */
		if (gdecl->foamGDecl.dir == FOAM_GDecl_Export)
			dvMarkFormat(globalsSlot, i, DV_Used);
	}
}


/*
 * Make a variable usage vector for a single DDecl.
 */
local DvUsage
dvMakeUsageVec(Foam ddecl)
{
	int	i, size;
	DvUsage dvu;

	assert(foamTag(ddecl) == FOAM_DDecl);
	size = foamDDeclArgc(ddecl);
	dvu = (DvUsage) stoAlloc(OB_Other, size*sizeof(struct dvUsageStruct));

	for(i=0; i < size; i++ ) {
		dvu[i].used	= DV_Unused;
		dvu[i].newIndex = i;
	}
	return dvu;
}


/*****************************************************************************
 *
 * :: Dead Variable marking.
 *
 ****************************************************************************/

/*
 * Top of recursive descent of the foam tree, starting at the file
 * initialization program.
 */
local void
dvMarkUnitUsage(Foam unit)
{
	int i;

	dvMarkFormat(constsSlot, int0, DV_Used);
	/* Mark the file initialization program */
	dvMarkProgUsage(int0, dvDefs[0]->foamDef.rhs);

	/* And the additional globals */
	for (i=1; i < foamArgc(unit->foamUnit.defs); i++) 
		if (foamTag(dvDefs[i]->foamDef.lhs) == FOAM_Glo)
			dvMarkExprUsage(dvDefs[i]);
}


/*
 * Mark the usages in a program.
 */
local void
dvMarkProgUsage(int idx, Foam prog)
{
	Scope("dvMarkProgUsage");
	DvUsage		fluid(dvLocals);
	Foam		fluid(dvDEnv);
	int		fluid(dvThisConst);
	dvThisConst = idx;

	assert(foamTag(prog) == FOAM_Prog);
	if (foamOptInfo(prog)->dvState == DV_Checked) ReturnNothing;

	foamOptInfo(prog)->dvState = DV_Checked;
	if (foamOptInfo(prog)->localUsage)
		stoFree(foamOptInfo(prog)->localUsage);

	dvLocals = dvMakeUsageVec(prog->foamProg.locals);
	foamOptInfo(prog)->localUsage = dvLocals;
	dvDEnv = prog->foamProg.levels;

	/* Mark the whole return format if a prog return multiple values */
	if (prog->foamProg.retType == FOAM_NOp && prog->foamProg.format!=0)
		dvMarkWholeFormat(prog->foamProg.format);
#ifdef NEW_FORMATS
	dvMarkWholeFormat(paramsSlot);
#endif

	/* Walk the program's body. */
	dvMarkExprUsage(prog->foamProg.body);

	ReturnNothing;
}


/*
 * Mark the usages in a general Foam expression.
 */
local void
dvMarkExprUsage(Foam expr)
{
	Syme	syme;

	switch (foamTag(expr)) {
	  case FOAM_Def:
		dvMarkDefined(expr->foamDef.lhs, expr->foamDef.rhs);
		break;
	  case FOAM_Set:
		dvMarkDefined(expr->foamSet.lhs, expr->foamSet.rhs);
		break;
	  case FOAM_EInfo: /* XXX! Don't need */
		dvMarkEInfo(expr->foamEInfo.env);
		break;
	  case FOAM_EEnsure:
		dvMarkEInfo(expr->foamEInfo.env);
		break;
	  default:
		foamIter(expr, arg, dvMarkExprUsage(*arg));
		break;
	}
	switch (foamTag(expr)) {
	  case FOAM_Loc:
		dvLocals[expr->foamLoc.index].used = DV_Used;
		break;
	  case FOAM_Return: {
		Foam	env, clos = expr->foamReturn.value;
		int	format;
		if (foamTag(clos) == FOAM_Clos) {
			env = clos->foamClos.env;
			if (foamTag(env) == FOAM_Env) {
				format = dvDEnv->foamDEnv.
					argv[env->foamEnv.level];
				dvMarkDefinedValues(format);
			}
		}
		break; }
	  case FOAM_Const:
		dvMarkFormat(constsSlot, expr->foamConst.index, DV_Used);
		dvMarkConstProg(expr->foamConst.index);
		break;
	  case FOAM_Fluid:
		dvMarkFormat(fluidsSlot, expr->foamFluid.index, DV_Used);
		break;
	  case FOAM_Lex: {
		int	format = dvDEnv->foamDEnv.argv[expr->foamLex.level];
		if (foamFixed(dvFormats[format]))
			dvMarkWholeFormat(format);
		dvMarkFormat(format, expr->foamLex.index, DV_Used);
		dvKillEnsures[dvThisConst] = false;
		break;
	  }
	  case FOAM_EElt:
		dvMarkEEltFormat(expr);
		dvKillEnsures[dvThisConst] = false;
		break;
	  case FOAM_EEnv:
		dvKillEnsures[dvThisConst] = false;
		break;
	  case FOAM_RElt:
		dvMarkWholeFormat(expr->foamRElt.format);
		break;
	  case FOAM_RNew:
		dvMarkWholeFormat(expr->foamRNew.format);
		break;
	  case FOAM_Rec:
		dvMarkWholeFormat(expr->foamRec.format);
		break;
	  case FOAM_MFmt:
		dvMarkWholeFormat(expr->foamMFmt.format);
		break;
          case FOAM_TRNew:
                dvMarkWholeFormat(expr->foamTRNew.format);
                break;
          case FOAM_TRElt:
                dvMarkWholeFormat(expr->foamTRElt.format);
                break;
          case FOAM_IRElt:
                dvMarkWholeFormat(expr->foamIRElt.format);
                break;
	  case FOAM_Glo: {
	        Foam gdecl, gdecls;
		dvMarkFormat(globalsSlot, expr->foamGlo.index, DV_Used);
		gdecls = dvFormats[globalsSlot];
		gdecl  = gdecls->foamDDecl.argv[expr->foamGlo.index];
		switch (gdecl->foamGDecl.protocol) {
		  case FOAM_Proto_Fortran:	/*FALLTHROUGH*/
		  case FOAM_Proto_C:		/*FALLTHROUGH*/
		  	dvMarkWholeFormat(gdecl->foamGDecl.format);
			break;
		  default:
			break;
		}
		break;
	      }
	  default:
		break;
	}

	syme = foamSyme(expr);
	if (!foamIsDecl(expr) &&
	    genIsLocalConst(syme) && genHasConstNum(syme) &&
	    (fintMode != FINT_LOOP || symeIntStepNo(syme) == intStepNo)) {
		int index = genGetConstNum(syme);
		dvMarkFormat(constsSlot, index, DV_Used);
		dvMarkConstProg(index);
	}
}


/*
 * Mark a defintion of a variable.
 */
local void
dvMarkDefined(Foam lhs, Foam rhs)
{
	int		side  = dvHasSideEffect(rhs);
	UsageState	tag;

	tag = side ? DV_DefinedSdEfx : DV_DefinedNoSdEfx;
	switch(foamTag(lhs)) {
	  case FOAM_Loc: {
		int index = lhs->foamLoc.index;
		if (dvLocals[index].used != DV_Used &&
		    dvLocals[index].used != DV_DefinedSdEfx)
			dvLocals[index].used = tag;
		break; }
	  case FOAM_Const:
		if (dvUsage(constsSlot, lhs->foamConst.index) !=
		    DV_DefinedSdEfx)
			dvMarkFormat(constsSlot, lhs->foamConst.index, tag);
		break;
	  case FOAM_Lex: {
		int format = dvDEnv->foamDEnv.argv[lhs->foamLex.level];
		assert(format!=0);
		if (dvUsage(format, lhs->foamLex.index) !=
		    DV_DefinedSdEfx)
			dvMarkFormat(format, lhs->foamLex.index, tag);
		break; }
	  case FOAM_EElt: {
		  AInt fmt = lhs->foamEElt.env;
		  if (dvUsage(fmt, lhs->foamEElt.lex) !=
		      DV_DefinedSdEfx)
			  dvMarkFormat(fmt, lhs->foamEElt.lex, tag);
		  if (dvFormats[fmt]->foamDDecl.usage == FOAM_DDecl_NonLocalEnv)
			  dvMarkWholeFormat(fmt);
		  dvMarkExprUsage(lhs->foamEElt.ref);
		  break;
	  }
	  default: /* Globals, fluids fall through */
		dvMarkExprUsage(lhs);
		break;
	}
	dvMarkExprUsage(rhs);
}

/*
 * EInfo => Skip one level
 */
local void
dvMarkEInfo(Foam foam)
{
	switch (foamTag(foam)) {
	  case FOAM_CEnv:
		dvMarkEInfo(foam->foamCEnv.env);
		break;
	  case FOAM_EElt:
		dvMarkExprUsage(foam->foamEElt.ref);
		break;
	  case FOAM_EEnv:
		dvMarkEInfo(foam->foamEEnv.env);
		break;
	  default:	
		break;
	}
	return;
}


Bool	dvHasSdEfx;


/*
 * Compute whether a function call has side effects.
 */
local Bool
dvHasSideEffect(Foam expr)
{
	dvHasSdEfx = false;

	dvHasSideEffect1(expr);
	return dvHasSdEfx;
}


/*
 * Inner recursive entry point for side-effect detection function.
 */
local void
dvHasSideEffect1(Foam expr)
{
	foamIter(expr, arg, dvHasSideEffect1(*arg));
	switch(foamTag(expr)) {
	  case FOAM_Set:
	  case FOAM_Def:
		dvHasSdEfx = true;
		return;
	  case FOAM_CCall:
		if (!foamPure(expr)) {
			dvHasSdEfx = true;
			return;
		}
		if (dvOpHasSideEffect(expr->foamCCall.op))
			dvHasSdEfx = true;
		break;
	  case FOAM_OCall:
	  case FOAM_PCall:
		if(!foamPure(expr)) {
			dvHasSdEfx = true;
			return;
		}
		break;
	  case FOAM_BCall:
		if (foamBValInfo(expr->foamBCall.op).hasSideFx) {
			dvHasSdEfx = true;
			return;
		}
		break;
	  default:
		break;
	}
	return;
}

/* Just check for builtins at the moment */
local Bool
dvOpHasSideEffect(Foam op)
{
	RuntimeCallInfo info;

	if (foamTag(op) != FOAM_Glo)
		return false;

	info = gen0GetRuntimeCallInfo(dvFormats[globalsSlot]->foamDDecl.argv[op->foamGlo.index]);
	
	return info && info->hasSideFx;
}

/*
 * Testing if a statement is a forcing one
 * !!if details of forcing is changed, this must be updated.
 */

local Foam
dvIsForcingStmt(Foam foam)
{
	Foam tmp;

	switch(foamTag(foam)) {
	  case FOAM_EEnsure: 
		return foam->foamEEnsure.env;
	  case FOAM_EInfo:
		if (foamLazy(foam))
			return NULL;
		else return foam->foamEInfo.env;

	  default:
		foamIter(foam, sub, if ((tmp = dvIsForcingStmt(*sub)) != NULL) return tmp;);
		return NULL;
	}
}

/*
 * Mark a prog pointed to by a constant.
 */
local void
dvMarkConstProg(int index)
{
	Foam	rhs = dvDefs[index]->foamDef.rhs;
	if (foamTag(rhs) == FOAM_Prog) dvMarkProgUsage(index, rhs);
}

/*
 * Mark an eelt's format for usage.  Only mark local formats.
 */
local void
dvMarkEEltFormat(Foam eelt)
{
	int    format = eelt->foamEElt.env;
	assert(format!=0);
#if 0
	/* check to see if format is in prog's existing env */
	for(i=0; i < foamArgc(dvDEnv); i++)
		if (format == dvDEnv->foamDEnv.argv[i]) break;
	if (i == foamArgc(dvDEnv)) /* foreign format */
		dvMarkWholeFormat(format);
#endif
	if (dvFormats[format]->foamDDecl.usage == FOAM_DDecl_NonLocalEnv)
		dvMarkWholeFormat(format);

	dvMarkFormat(format, eelt->foamEElt.lex, DV_Used);
}

/*
 * Mark records as completely used
 */
local void
dvMarkWholeFormat(int format)
{
	Foam    ddecl;
	int	i;

	ddecl = dvFormats[format];
	/* FOAM_DDecl_CSig has no references to other formats (yet) */
	if (ddecl->foamDDecl.usage == FOAM_DDecl_FortranSig) {
		for (i=0; i < foamDDeclArgc(ddecl); i++) {
			if (ddecl->foamDDecl.argv[i]->foamDecl.type == FOAM_Clos)
				dvMarkWholeFormat(ddecl->foamDDecl.argv[i]->foamDecl.format);
		}
	} 
	for(i=0; i<foamDDeclArgc(ddecl); i++)
		dvMarkFormat(format, i, DV_Keep);
}

/*
 * Mark formats as used which are declared as fixed.
 */
local void
dvMarkDefinedValues(int format)
{
	int	i;
	for(i=0; i<foamDDeclArgc(dvFormats[format]); i++)
		if (foamFixed(dvFormats[format]->foamDDecl.argv[i]))
			dvMarkFormat(format, i, DV_Keep);
}

/*****************************************************************************
 *
 * :: Elimination of dead variables in decls.
 *
 ****************************************************************************/

/*
 * Eliminate unused variables in a unit once usage info has been
 * computed.
 */
local void
dvElimUnused(Foam unit)
{
	dvComputeRenumbering(unit);
	if (!dvChanged) return;

	dvRenumberProgs(unit);
	dvCollapseFormats(unit);
	dvDeleteEmptyFormats(unit);
	dvElimUnusedFormats(unit);
	dvCollapseDefs(unit);
	genKillOldSymeConstNums(dvGeneration);
}


/*
 * Compute the renumbering of variables in a unit.
 */
local void
dvComputeRenumbering(Foam unit)
{
	int	i,j, newIndex;

	/* first renumber formats */
	for (i=0; i < foamArgc(unit->foamUnit.formats); i++) {
		newIndex = 0;
		for(j=0; j < foamDDeclArgc(dvFormats[i]); j++) {
			if (dvFormatUsage[i][j].used >= DV_Keep)
				dvFormatUsage[i][j].newIndex = newIndex++;
			else {
				dvChanged = true;
				dvFormatUsage[i][j].newIndex = SYME_NUMBER_UNASSIGNED;
			}
		}
	}

	/* next renumber locals */
	for(i=0; i<foamArgc(unit->foamUnit.defs); i++) {
		Foam prog = dvDefs[i]->foamDef.rhs;
		Foam locals;

		if (foamTag(prog) != FOAM_Prog) continue;
		if (foamOptInfo(prog)->dvState != DV_Checked) continue;

		locals = prog->foamProg.locals;
		newIndex = 0;
		assert(foamDDeclArgc(locals)==0 || foamOptInfo(prog)->localUsage);
		for(j=0; j < foamDDeclArgc(locals); j++) {
			if (foamOptInfo(prog)->localUsage[j].used >= DV_Keep)
				foamOptInfo(prog)->localUsage[j].newIndex =
					newIndex++;
			else 
				dvChanged = true;
		}
	}
}


/*
 * Rebuild the format declarations for a foam unit, removing decls for
 * unused variables.
 */
local void
dvCollapseFormats(Foam unit)
{
	int	i;

	for (i=0; i< foamArgc(unit->foamUnit.formats); i++)
	     dvFormats[i] = dvCollapseFormat(dvFormatUsage[i], dvFormats[i]);

	/* collapse locals */
	for(i=0; i<foamArgc(unit->foamUnit.defs); i++) {
		Foam prog = dvDefs[i]->foamDef.rhs;
		if (foamTag(prog) != FOAM_Prog) continue;
		if (foamOptInfo(prog)->dvState != DV_Checked) continue;
		prog->foamProg.locals =
			dvCollapseFormat(foamOptInfo(prog)->localUsage,
					 prog->foamProg.locals);
	}
}


/*
 * Recompute the DDecl for a format removing unused slots.
 */
local Foam
dvCollapseFormat(DvUsage usage, Foam ddecl)
{
	int	i, size;
	Foam	newFoam;
	assert(foamTag(ddecl) == FOAM_DDecl);

	/* compute size of new ddecl */
	for(i=0, size=0; i<foamDDeclArgc(ddecl); i++)
		if (usage[i].used >= DV_Keep) size++;

	if (size == foamDDeclArgc(ddecl)) return ddecl;

	/* Build the new DDecl */
	newFoam = foamNewEmpty(FOAM_DDecl, size + 1);
	newFoam->foamDDecl.usage = ddecl->foamDDecl.usage;

	for(i=0, size=0; i<foamDDeclArgc(ddecl); i++) {
		if (usage[i].used >= DV_Keep) {
			newFoam->foamDDecl.argv[size++] =
				ddecl->foamDDecl.argv[i];
		}
		else
			foamFree(ddecl->foamDDecl.argv[i]);
	}
	foamFreeNode(ddecl);

	return newFoam;
}

local void
dvDeleteEmptyFormats(Foam unit)
{
	/* !! Fill in after per-prog format info is calculated. */
}
/*
 * See if a definition has been used.  Never removes defs of constants.
 */
local Bool
dvDefUsed(Foam def)
{
	Foam lhs = def->foamDef.lhs;
	return foamTag(lhs) != FOAM_Const || foamDvMark(lhs) == dvGeneration;
}

/*
 * Remove the defintions of constants that were not used.
 */
local void
dvCollapseDefs(Foam unit)
{
	int	i, size = 0;
	Foam	defs = unit->foamUnit.defs, newFoam;

	/* Compute the size of the new definition section. */
	for(i=0; i<foamArgc(defs); i++) {
		Foam def = defs->foamDDef.argv[i];
		if (dvDefUsed(def)) size++;
	}

	if (size == foamArgc(defs)) return;

	/* Build the new definition section. */
	newFoam = foamNewEmpty(FOAM_DDef, size);
	for(i=0, size=0; i<foamArgc(defs); i++) {
		Foam def = defs->foamDDef.argv[i];
		if (dvDefUsed(def))
			newFoam->foamDDef.argv[size++] = defs->foamDDef.argv[i];
		else
			foamFree(defs->foamDDef.argv[i]);
	}
	foamFreeNode(defs);
	unit->foamUnit.defs = newFoam;
}

/*****************************************************************************
 *
 * :: Dead Variable renumbering of foam expressions.
 *
 ****************************************************************************/

/*
 * Renumber the variable references.
 */
local void
dvRenumberProgs(Foam unit)
{
	int	i;

	/* Renumber all programs */
	for(i=0; i<foamArgc(unit->foamUnit.defs); i++) {
		Foam prog = dvDefs[i]->foamDef.rhs;
		Foam lhs  = dvDefs[i]->foamDef.lhs;
		dvThisConst = i;
		if (foamTag(lhs) == FOAM_Const) lhs = dvRenumberExpr(lhs);
		if (foamTag(lhs) == FOAM_Glo) {
			dvDefs[i] = dvRenumberStmt(dvDefs[i]);
		}
		if (foamTag(prog) != FOAM_Prog) continue;
		dvRenumberProg(prog);
	}
}


/*
 * Renumber a single program.
 */
local void
dvRenumberProg(Foam prog)
{
	Scope("dvReunmberProg");
	DvUsage		fluid(dvLocals);
	Foam		fluid(dvDEnv);
	Foam 		seq;
	Bool		changed = false;
	int 		i;

	if (foamOptInfo(prog)->dvState != DV_Checked) return;
	dvLocals = foamOptInfo(prog)->localUsage;
	dvLocalsDecl = prog->foamProg.locals;
	dvDEnv = prog->foamProg.levels;

	optResetDeadvPending(prog);

	seq = prog->foamProg.body;

	for (i=0; i<foamArgc(seq); i++)	{
		Foam newStmt, stmt = seq->foamSeq.argv[i];

		newStmt = dvRenumberStmt(seq->foamSeq.argv[i]);
		seq->foamSeq.argv[i] = newStmt;

		if (newStmt != stmt) changed = true;
	}

	if (changed) {
		optSetCsePending(prog);
		optSetJFlowPending(prog);
		optSetCPropPending(prog);
	}

	ReturnNothing;
}

local Foam
dvRenumberStmt(Foam stmt)
{
	Foam 	   force, nstmt;
	UsageState usage;

	if (foamTag(stmt) == FOAM_EEnsure && dvKillEnsures[dvThisConst])
		return foamNewNOp();

	force = dvIsForcingStmt(stmt);
	
	if (force) {
		usage = dvGetForcingUsage(force);
		if (usage != DV_Used)
			return foamNewNOp();
	}
	switch(foamTag(stmt)) {
	  case FOAM_Def:
		nstmt = dvReplaceAssignment(stmt->foamDef.lhs,
					    stmt->foamDef.rhs, stmt);
		break;
	  case FOAM_Set:
		nstmt = dvReplaceAssignment(stmt->foamSet.lhs,
					    stmt->foamSet.rhs, stmt);
		break;
	  default:
		nstmt = dvRenumberExpr(stmt);
	}		
	return nstmt;
}

local UsageState
dvGetForcingUsage(Foam expr)
{
	UsageState usage;

	if (foamTag(expr) == FOAM_CEnv)
		return dvGetForcingUsage(expr->foamCEnv.env);
	if (foamTag(expr) == FOAM_EEnv)
		return dvGetForcingUsage(expr->foamEEnv.env);

	usage = dvGetUsage(expr);

	if (usage != DV_Used)
		return usage;

	if (foamTag(expr) == FOAM_EElt)
		return dvGetForcingUsage(expr->foamEElt.ref);

	return usage;
}

/* 
 * Renumber a general foam expression.
 */
local Foam
dvRenumberExpr(Foam expr)
{
	Foam	nexpr = expr;
	Syme	syme;

	assert(foamTag(expr) != FOAM_Def && foamTag(expr) != FOAM_Set);

	foamIter(expr, arg, *arg = dvRenumberExpr(*arg));

	switch (foamTag(expr)) {
	  case FOAM_Loc:
		expr->foamLoc.index =
			dvLocals[expr->foamLoc.index].newIndex;
		break;
	  case FOAM_Const:
		if (dvUsage(constsSlot, expr->foamConst.index) >= DV_Keep)
			foamDvMark(expr) = dvGeneration;
		expr->foamConst.index = dvNewIndex(constsSlot,
						   expr->foamConst.index);
		break;
	  case FOAM_Lex:
		expr->foamLex.index =
			dvNewIndex(dvDEnv->foamDEnv.argv[expr->foamLex.level],
				   expr->foamLex.index);
		break;
	  case FOAM_EElt:
		expr->foamEElt.lex =
			dvNewIndex(expr->foamEElt.env, expr->foamEElt.lex);
		break;
	  case FOAM_Glo:
		expr->foamGlo.index = dvNewIndex(globalsSlot, expr->foamGlo.index);
		break;
	  default:
		break;
	}

	/* renumber the constants in syme hanging off the foam tree. */
	syme = foamSyme(nexpr);
	if (syme && !foamIsDecl(nexpr) &&
	    genIsLocalConst(syme) && genHasConstNum(syme) &&
	    symeDVMark(syme) < dvGeneration &&
	    (fintMode != FINT_LOOP || symeIntStepNo(syme) == intStepNo)) {
		UShort index    = genGetConstNum(syme);
		UShort newIndex = dvNewIndex(constsSlot, index);
		symeSetDVMark(syme, dvGeneration);
		/* FIXME */
		genSetConstNum(syme, -index, newIndex, false);
	}
	return nexpr;
}


/*
 * Replace assignments to variables.  If the variable is unused, it eliminates
 * the initialization code unless it may be side-effecting.
 */
local Foam
dvReplaceAssignment(Foam lhs, Foam rhs, Foam ass)
{
	UsageState	tag;


	tag = dvGetUsage(lhs);

	if (tag >= DV_Keep) {
		ass->foamDef.lhs = dvRenumberExpr(lhs);
		ass->foamDef.rhs = dvRenumberExpr(rhs);
		return ass;
	}
	else if (tag == DV_DefinedNoSdEfx || !dvHasSideEffect(rhs)) {
		foamFree(lhs);
		return foamNewNOp();
	}
	else
		return dvRenumberExpr(rhs);
}

local UsageState
dvGetUsage(Foam foam)
{
	DvUsage	usage;
	int	index, format;

	switch(foamTag(foam)) {
	  case FOAM_Loc:
		index = foam->foamLoc.index;
		usage = dvLocals;
		break;
	  case FOAM_Const:
		index = foam->foamConst.index;
		format = constsSlot;
		usage = dvFormatUsage[format];
		break;
	  case FOAM_Lex:
		index = foam->foamLex.index;
		format = dvDEnv->foamDEnv.argv[foam->foamLex.level];
		usage = dvFormatUsage[format];
		break;
	  case FOAM_EElt:
		index = foam->foamEElt.lex;
		format = foam->foamEElt.env;
		usage = dvFormatUsage[format];
		break;
	  default:
		return DV_Used;
	}

	return usage[index].used;
}


local void
dvElimUnusedFormats(Foam unit)
{
	/*!! Do it for real later */
}

/*
 * Remove FOAM_Nops from the top-level of all function defs in a unit.
 */
local void
dvRemoveNops(Foam unit)
{
	int	i;
	Foam	*argv;
	assert (foamTag(unit) == FOAM_Unit);
	argv = unit->foamUnit.defs->foamDDef.argv;
	for(i=0; i< foamArgc(unit->foamUnit.defs); i++) {
		if (foamTag(argv[i]) == FOAM_Def &&
		    foamTag(argv[i]->foamDef.rhs) == FOAM_Prog) {
			Foam	prog = argv[i]->foamDef.rhs;
			dvRemoveSeqNops(prog->foamProg.body);
		}
	}
}

/* 
 * Remove the FOAM_NOps from a body of a function.
 */
local void
dvRemoveSeqNops(Foam seq)
{
	int	i, j;

	assert (foamTag(seq) == FOAM_Seq);

	for(j = i = 0; i < foamArgc(seq); i++) {
		Foam	foam = seq->foamSeq.argv[i];
		if (foamHasSideEffect(foam) || foamIsControlFlow(foam) ||
		    foamPure(foam)) {
			if (i != j)
				seq->foamSeq.argv[j] = seq->foamSeq.argv[i];
			j++;
		}
	}
	foamArgc(seq) = j;
}
