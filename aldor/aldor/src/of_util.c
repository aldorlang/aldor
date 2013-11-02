/*****************************************************************************
 *
 * of_util.c: Foam-to-foam optimization utilities.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
#include "debug.h"
#include "of_util.h"
#include "phase.h"
#include "store.h"
#include "util.h"
#include "syme.h"
#include "strops.h"
#include "fbox.h"
 
/*****************************************************************************
 *
 * :: Temporary variable pools
 *
 ****************************************************************************/
 
VarPool
vpNew(FoamBox fbox)
{
        VarPool newVP = (VarPool) stoAlloc(OB_Other, sizeof(struct varPool));
        int             i;
 
        newVP->fbox = fbox;
        for(i=0; i<FOAM_LIMIT; i++)
                newVP->vars[i] = listNil(AInt);
        return newVP;
}
 
void
vpFree(VarPool pool)
{
        int     i;
        for(i=0; i < FOAM_LIMIT; i++)
                listFree(AInt)(pool->vars[i]);
        stoFree(pool);
}
 
int
vpNewVarDecl(VarPool pool, Foam decl)
{
        return fboxAdd(pool->fbox, decl);
}
 
/* NOTE: the macro vpNewVar(pool, type) can be used if fmtSlot not needed */
int
vpNewVar0(VarPool pool, FoamTag type, int fmtSlot)
{
        int     var;
        if (pool->vars[type] != listNil(AInt)) {
                AIntList l = pool->vars[type];
                var        = car(l);
                pool->vars[type] = cdr(l);
                listFreeCons(AInt)(l);
        }
        else {
                String  name = strCopy("");
                Foam decl = foamNewDecl(type, name, fmtSlot);
                var       = fboxAdd(pool->fbox, decl);
        }
        return var;
}
 
void
vpFreeVar(VarPool pool, int var)
{
        int             type;
        Foam            decl;
        decl = fboxNth(pool->fbox, var);
        type = decl->foamDecl.type;
 
        pool->vars[type] = listCons(AInt)(var, pool->vars[type]);
}
 
/*****************************************************************************
 *
 * :: Flags for -W runtime.
 *
 ****************************************************************************/
 
Bool                    gen0IsRuntime = false;
Bool                    inl0AfterInline = false;
Bool                    gen0Hashcheck = false;
 
/*****************************************************************************
 *
 * :: Foam patching.
 *
 ****************************************************************************/
 
/*
 * This is invoked after all for generation and optimization complete.  
 * It marks decls of variables which are accessed only as records
 * with the format number of the record.  It also fixes DEnv sections
 * of progs to reflect the actual levels accessed.
 */
 
local void      fpPatchDefs             (Foam unit);
local void      fpPatchProg             (Foam prog);
local void      fpPatchExpr             (Foam expr, Bool);
local void      fpPatchDefined          (Foam lhs, Foam rhs);
local void      fpMarkFormat            (Foam *, AInt *, FoamTag);
local Foam      fpGetDecl               (Foam foam);
local void      fpClearFormats          (Foam ddecl);
 
Foam            *fpFormats;
AInt            *fpDEnv;
Foam            *fpParams;
Foam            *fpLocals;
Foam            fpProg;
Foam		fpUnit;

void
fpPatchUnit(Foam unit)
{
        int     i;
        assert(foamTag(unit) == FOAM_Unit);
	
	fpUnit = unit;
        fpFormats = unit->foamUnit.formats->foamDFmt.argv;
 
        for(i=0; i < foamArgc(unit->foamUnit.formats); i++)
                fpClearFormats(fpFormats[i]);
 
        fpPatchDefs(unit);
        assert(foamAudit(unit));
}
 
local void
fpPatchDefs(Foam unit)
{
        int     i;
        for(i=0; i < foamArgc(unit->foamUnit.defs); i++) {
                Foam    def = unit->foamUnit.defs->foamDDef.argv[i];
                if (foamTag(def) == FOAM_Def &&
                    foamTag(def->foamDef.rhs) == FOAM_Prog)
                        fpPatchProg(def->foamDef.rhs);
        }
}
 
 
local void
fpPatchProg(Foam prog)
{
	int i;
        assert(foamTag(prog) == FOAM_Prog);

        fpProg   = prog;
	foamProgSetLeaf(fpProg);
	foamProgSetNoEnvUse(fpProg);
        fpDEnv   = prog->foamProg.levels->foamDEnv.argv;
	for (i=1; i<foamArgc(prog->foamProg.levels); i++) {
		if (fpDEnv[i] != emptyFormatSlot)
			foamProgUnsetNoEnvUse(fpProg);
	}

        fpLocals = prog->foamProg.locals->foamDDecl.argv;
#ifdef NEW_FORMATS
	fpParams = fpFormats[paramsSlot]->foamDDecl.argv[prog->foamProg.params-1]->foamDDecl.argv;
        fpClearFormats(fpFormats[paramsSlot]->foamDDecl.argv[prog->foamProg.params-1]);
#else
        fpParams = prog->foamProg.params->foamDDecl.argv;
        fpClearFormats(prog->foamProg.params);
#endif
        fpClearFormats(prog->foamProg.locals);
 
        fpPatchExpr(prog->foamProg.body, false);

	
        return;
}
 
/*!! uncomment this to get array decls to be marked with base type. */
#define MarkArrayDecls
/**/
 
local void
fpPatchExpr(Foam expr, Bool envOK)
{
	int i;
        switch (foamTag(expr)) {
          case FOAM_Clos:
		if (foamTag(expr->foamClos.env) == FOAM_Env) {
			fpPatchExpr(expr->foamClos.env, true);
			fpPatchExpr(expr->foamClos.prog, false);
		}
                break;
          case FOAM_OCall:
		if (foamTag(expr->foamOCall.env) == FOAM_Env) {
			fpPatchExpr(expr->foamOCall.env, true);
			fpPatchExpr(expr->foamClos.prog, false);
			for (i=0; i<foamArgc(expr) - 3; i++)
				fpPatchExpr(expr->foamClos.prog, false);
		}
                break;
	  default:
		foamIter(expr, arg, fpPatchExpr(*arg, false));
	}

        switch (foamTag(expr)) {
          case FOAM_Def:
                fpPatchDefined(expr->foamDef.lhs, expr->foamDef.rhs);
                break;
          case FOAM_Set:
                fpPatchDefined(expr->foamSet.lhs, expr->foamSet.rhs);
                break;
          case FOAM_Lex:
                /*!! Need code for computing DEnv */
		if (expr->foamLex.level == 0)
			if (fpProg) foamProgUnsetLeaf(fpProg);
                break;
          case FOAM_Env:
		if (expr->foamEnv.level == 0)
			if (fpProg) foamProgUnsetLeaf(fpProg);
                break;
          case FOAM_RElt:
                fpMarkFormat(&expr->foamRElt.expr, &expr->foamRElt.format,
                             FOAM_Rec);
                break;
#if 0
#ifdef MarkArrayDecls
          case FOAM_AElt:
                fpMarkFormat(&expr->foamAElt.expr, &expr->foamAElt.baseType,
                             FOAM_Arr);
                break;
#endif
#endif
          default:
                break;
	}	
#if 0
	  /* 
	   * There are arguments for putting 'FOAM_Glo' in here
	   * fortunately, the only time we use globals are for 
	   *  a) things like 'import from Foreign', and therefore are
	   *    initialized already
	   *  b) forcing of global initializers.  In this case, we don't
	   *     care, because we won't be looking at our environment
	   *
	   * FOAM_Clos technically needn't be here, but unfortunately
	   * the prog's DEnv could be wrong, and we would lose.
	   */
        switch (foamTag(expr)) {
          case FOAM_Env:
		if (envOK)
			break;
          case FOAM_Lex:
		if (expr->foamLex.level == 0)
			break;
	  case FOAM_CEnv:
#if 0
/**/	  case FOAM_Clos:
#endif
	  case FOAM_EElt:
		if (expr->foamEElt.level < 0) bug("Really bad eelt");
	  case FOAM_Fluid:
	  case FOAM_PRef:
	  case FOAM_EInfo:
		foamProgUnsetNoEnvUse(fpProg);
		break;
	  default:
		break;
        }
#endif
}
 
 
local void
fpPatchDefined(Foam lhs, Foam rhs)
{
        if (foamTag(rhs) == FOAM_RNew)
                fpMarkFormat(&lhs, &rhs->foamRNew.format, FOAM_Rec);
#ifdef MarkArrayDecls
        if (foamTag(rhs) == FOAM_ANew)
                fpMarkFormat(&lhs, &rhs->foamANew.eltType, FOAM_Arr);
#endif
}
 
local void
fpMarkFormat(Foam * pRef, AInt *pFormat, FoamTag tag)
{
        /*!! Could be a problem if used as different types of records */
        Foam    decl, ref = *pRef;
        int     oldFormat;

	while (foamTag(ref) == FOAM_Cast)
		ref = ref->foamCast.expr;

	decl = fpGetDecl(ref);

        if (!decl) return;

	/* Kill redundant casts (ex: (RElt 4 (Cast Rec (Par 0)) 0)) */

	/* Unsafe if typeof(ref) == FOAM_Word */
	if (ref != *pRef && decl->foamDecl.type != FOAM_Word) {
		ref = foamCopy(ref);
		foamFree(*pRef);
		*pRef = ref;
	}

        oldFormat = decl->foamDecl.format;

        if (oldFormat == *pFormat)
                return;

	/* Never, ever, modify the type of a parameter! */
	if (foamTag(ref) == FOAM_Par)
		return;

        if (oldFormat == emptyFormatSlot) {
		/* !! Hack: we modify record formats too because we are still
		 * weak in unifying formats
		 */

		if (DEBUG(phase)) {
			if (decl->foamDecl.type == FOAM_Rec)
				fprintf(dbOut,
					"Record with wrong format number found...");
		}

#ifdef NOWAY
                decl->foamDecl.format = *pFormat;
                decl->foamDecl.type   = tag;
#endif
		return;
        }


	/* This point is reached if old and new format are different
	 * and the old one is not the empty format 
  	 */

	if (decl->foamDecl.type != FOAM_Rec)
		return;

	if (foamArgc(fpFormats[*pFormat]) !=  foamArgc(fpFormats[oldFormat]) ||
	    !foamEqual(fpFormats[*pFormat], fpFormats[oldFormat]))
	 	bugWarning("record var. used in 2 different format contexts.");
        else
                /* Formats are equals: use the first one */
		*pFormat = oldFormat;

	/* !! Somewhere we should compress equal formats and remove unused
	 * ones
	 */
}
 
local Foam
fpGetDecl(Foam foam)
{
        switch(foamTag(foam)) {
          case FOAM_Par:
                return fpParams[foam->foamPar.index];
          case FOAM_Loc:
                return fpLocals[foam->foamLoc.index];
          case FOAM_Lex:
                return fpFormats[fpDEnv[foam->foamLex.level]]->
                        foamDDecl.argv[foam->foamLex.index];
          default:
                return 0;
        }
}
 
local void
fpClearFormats(Foam ddecl)
{
	Length	i;

	/* Never clear non-record formats */
	switch  (ddecl->foamDDecl.usage) {
	  case FOAM_DDecl_FortranSig:	/*FALLTHROUGH*/
	  case FOAM_DDecl_CSig:		/*FALLTHROUGH*/
		return; 
	  default:
		break;
	}

	for (i = 0; i < foamDDeclArgc(ddecl); i += 1) {
		Foam	decl = ddecl->foamDDecl.argv[i];
#ifdef NEW_FORMATS
		if (foamTag(decl) == FOAM_DDecl) break;
#endif
		if (foamTag(decl) == FOAM_GDecl) {
			if (decl->foamGDecl.protocol == FOAM_Proto_Foam &&
			    decl->foamGDecl.type != FOAM_Rec)
				decl->foamGDecl.format = emptyFormatSlot;
		} 
		else if (decl->foamDecl.type != FOAM_Rec
			 && decl->foamDecl.type != FOAM_Arr
			 && decl->foamDecl.type != FOAM_TR
			 && decl->foamDecl.type != FOAM_NOp)
			decl->foamDecl.format = emptyFormatSlot;
	}
}

/*****************************************************************************
 *
 * :: Make Flat Sequences
 *
 ****************************************************************************/

local int	utilStatementsCount	(Foam foam);
local void	utilSequencesExpand	(Foam foam, Foam ** p);

/* During the inlining some (Seq ..) stmts are inserted into the code.
 * For convenience, we want that a Prog contains exactly a unique Seq as
 * its first instruction.
 * So this utility build a new Seq without Seqs inside.
 * NOTE: the procedure is optimized, because seq may appear only at the first
 * 	level of an expansion tree. Don't use foamIter!
 */

Foam
utilMakeFlatSeq(Foam foam)
{
	int 	numStmts = utilStatementsCount(foam);
	Foam 	newSeq;
	Foam *  stmtPtr;

	assert(foamTag(foam) == FOAM_Seq);

	/* No sequences inside ? */
	if (numStmts == foamArgc(foam)) return foam;

	newSeq = foamNewEmpty(FOAM_Seq, numStmts);

	stmtPtr = newSeq->foamSeq.argv;

	utilSequencesExpand(foam, &stmtPtr);

	return newSeq;
}

local int
utilStatementsCount(Foam foam)
{
	int i, count = foamArgc(foam);

	assert(foamTag(foam) == FOAM_Seq);

	for (i = 0; i < foamArgc(foam); i++)
		if (foamTag(foam->foamSeq.argv[i]) == FOAM_Seq)
			count += utilStatementsCount(foam->foamSeq.argv[i])-1;
	
	return count;
}

local void
utilSequencesExpand(Foam foam, Foam ** p)
{
	int i;

	assert(foamTag(foam) == FOAM_Seq);

	for (i = 0; i < foamArgc(foam); i++) {

		Foam stmt = foam->foamSeq.argv[i];

		if (foamTag(stmt) == FOAM_Seq)
			utilSequencesExpand(stmt, p);
		else {
			**p = stmt;
			(*p)++;
		}
	}

	foamFreeNode(foam);	/* Release the expanded Seq */
}
