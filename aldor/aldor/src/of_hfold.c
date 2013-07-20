/*****************************************************************************
 *
 * hfold.c: Foam syme hash constant folding
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/* 
 * Takes foam, assumed to be decorated with symes and
 * replaces calls to domainGetHash! with the expected result 
 * of the call, and replaces foamPRefs with the value of the 
 * PRef slot which should be held in the syme.
 * 
 * To fix: The hashNum slot could, with a bit of work, be held by a prog.
 * 	   
 */

#include "of_inlin.h"
#include "of_util.h"
#include "gf_rtime.h"
#include "stab.h"

Bool	hfoldDebug	= false;
#define hfoldDEBUG(s)	DEBUG_IF(cfoldDebug, s)

/*****************************************************************************
 *
 * :: Local structures
 *
 ****************************************************************************/

/*****************************************************************************
 *
 * :: Local function declarations
 *
 ****************************************************************************/

local void	hfoldProg	(Foam);
local Foam	hfoldExpr	(Foam);
local Foam	hfoldPRef	(Foam);
local Foam	hfoldCCall	(Foam);
local Foam	hfoldSet	(Foam);
local Bool	hfoldIsDomHash	(Foam);
local void	hfoldInit	(Foam);

/*****************************************************************************
 *
 * :: Local Constants
 *
 ****************************************************************************/

static int 	hfoldDomainHashGlobal;
static OptInfo	hfoldProgInfo;
static Stab	hfoldStab;
static int	hfoldNumLocals;

Foam
hfoldUnit(Foam unit)
{
	Foam defs, formats;
	int i;

	defs    = unit->foamUnit.defs;
	formats = unit->foamUnit.formats;

	hfoldInit(foamUnitGlobals(unit));
	
	for (i = 0; i < foamArgc(defs); i++) {
		Foam def = defs->foamDDef.argv[i];
		if (foamTag(def->foamDef.rhs) == FOAM_Prog)
			hfoldProg(def->foamDef.rhs);
	}
	assert(foamAudit(unit));
	return unit;
}

local void
hfoldInit(Foam globals)
{
	int i;


	/* !! Should put runtime strings into gf_rtime.h */
	for (i=0; i<foamDDeclArgc(globals); i++) {
		Foam decl = globals->foamDDecl.argv[i];
		if (strEqual(decl->foamDecl.id, "domainHash!"))
			break;
	}
	if (i == foamDDeclArgc(globals))
		hfoldDomainHashGlobal = -1;
	else 
		hfoldDomainHashGlobal = i;

}


local void
hfoldProg(Foam prog)
{
	Foam body;
	int i;

	hfoldProgInfo = foamOptInfo(prog);

	if (!hfoldProgInfo) return;   /* saved files have no foamOptInfo */

	hfoldStab     = hfoldProgInfo->stab ? hfoldProgInfo->stab : stabFile();
		
	/* No Stab => nothing to do */
	if (!hfoldStab)
		return;
	hfoldNumLocals = foamDDeclArgc(prog->foamProg.locals);

	body = prog->foamProg.body;
	for (i=0; i<foamArgc(body); i++) {
		body->foamSeq.argv[i] = hfoldExpr(body->foamSeq.argv[i]);
	}
	
	hfoldStab     = NULL;
	hfoldProgInfo = NULL;
}

local Foam 
hfoldExpr(Foam foam)
{
	Foam new, newFoam = foam;

	/* What about FOAM_Def nodes? */
	if (foamTag(foam) == FOAM_Set) {
		newFoam = hfoldSet(foam);
		if (foam != newFoam) {
			foamFree(foam);
			return newFoam;
		}
#if AXL_EDIT_1_1_12p6_24
		/* NEVER hfold PRef target */
		/* ASSUME we never have a PRef in multi-lhs */
		if (foamTag(foam->foamSet.lhs) == FOAM_PRef) {
			/* hfold the rhs of the set just in case */
			Foam	foamRhs = foam->foamSet.rhs;
			newFoam = hfoldExpr(foamRhs);
			if (newFoam != foamRhs) {
				foamFree(foamRhs);
				foam->foamSet.rhs = newFoam;
			}
			return foam;
		}
#endif
	}

	foamIter(foam, arg, { new = hfoldExpr(*arg);
			      *arg = new;  });
	switch(foamTag(foam)) {
	case FOAM_PRef:
		newFoam = hfoldPRef(foam);
		break;
	case FOAM_CCall:
		newFoam = hfoldCCall(foam);
		break;
	default:
		break;
	}
	if (newFoam != foam)
		foamFreeNode(foam);
	
	return newFoam;
}

local Foam
hfoldCCall(Foam foam)
{
	Syme syme;
	Foam ref;

	if (!hfoldIsDomHash(foam->foamCCall.op))
		return foam;

	ref = foam->foamCCall.argv[0];

	syme = foamSyme(ref);

	if (syme && symeHashNum(syme) && inlInlinable(hfoldStab, syme))
		foam = foamNewSInt(symeHashNum(syme));

	return foam;
}

local Foam 
hfoldPRef(Foam foam)
{
	Syme syme;
	Foam prog;

	if (foam->foamPRef.idx != 0) 
		return foam;

	prog = foam->foamPRef.prog;

	if (foamTag(prog) != FOAM_CProg)
		return foam;
	
	syme = foamSyme(prog->foamCProg.prog);

	if (syme && symeHashNum(syme) && inlInlinable(hfoldStab, syme))
		foam = foamNewSInt(symeHashNum(syme));
	
	return foam;
}


local Foam
hfoldSet(Foam foam)
{
	Syme syme;
	Foam rhs = foam->foamSet.rhs;
	Foam ref;

	if (foamTag(rhs) != FOAM_CCall)
		return foam;

	if (!hfoldIsDomHash(rhs->foamCCall.op))
		return foam;

	ref = rhs->foamCCall.argv[0];

	syme = foamSyme(ref);

	if (syme && symeHashNum(syme) && inlInlinable(hfoldStab, syme)) 
		foam->foamSet.rhs = foamNewSInt(symeHashNum(syme));

	return foam;
}

local Bool
hfoldIsDomHash(Foam foam)
{
	if (foamTag(foam) != FOAM_Glo)
		return false;
	
	return (foam->foamGlo.index == hfoldDomainHashGlobal);
}
