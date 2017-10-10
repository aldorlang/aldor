#include "foam.h"
#include "debug.h"
#include "of_peep.h"
#include "of_retyp.h"
#include "of_util.h"
#include "optfoam.h"
#include "opttools.h"
#include "store.h"
#include "fbox.h"
#include "syme.h"
#include "util.h"

/*
 * Retype pass.
 *
 * This replaces Word valued variables with more specific types.
 * Algorithm is to run over the foam, and any instances of (Cast XX (Loc Y))
 * indicate that Y may be converted to XX, provided that Y is a Word and 
 * other casts do not conflict.
 *
 * A caveat: This will break if the same variable is used for values of
 * two differently represented types (eg. BInt and Double).. genfoam should not
 * generate any cases like this, but optimisations might.
 */

/******************************************************************************
 *
 * :: Debug
 *
 *****************************************************************************/

Bool	retDebug	= false;

#define retDEBUG	DEBUG_IF(ret)	afprintf



local void rtcReplaceDecls(RetContext context);
local void rtcRearrangeMultiAssign(RetContext context);

local FoamTag rtcFoamExprType(RetContext context, Foam foam, AInt *pfmt);
local FoamList rtcMultiAssignVars(RetContext context, FoamBox tempLocals, Foam lhs);
local Foam rtcMultiAssignValues(RetContext context, FoamList extraVars, Foam lhs);
local FoamList rtcMultiAssignFollows(RetContext context, FoamList extraVars, Foam lhs);


local Bool retCanConvert(FoamTag fromType, FoamTag toType);
local Bool retIsCompatible(RetContext context, Foam currentDecl, FoamTag type, AInt fmt);
local void retAddUse(RetContext context, FoamTag type, AInt fmt, Foam foam);
local void retRetypeProg(RetContext context, Foam prog);
local void retMarkCasts(RetContext context, Foam prog);
local void retMarkExpr(RetContext context, Foam foam);
local void retMarkPCallJava(RetContext context, Foam foam);
local void retMarkPCall(RetContext context, Foam foam);
local Bool retRearrangeProg(RetContext context);
local Foam retRearrangeExpr(RetContext context, Foam expr, Bool isLhs);
local Foam retRearrangeSet(RetContext context, Foam set);
local Foam retRearrangeVar(RetContext context, Foam var);
local Foam retCast(RetContext context, FoamTag type, Foam foam);
local Foam retPeepCasts(Foam foam);
local Bool retIsLocal(Foam foam);
local Foam retLocal(Foam foam);


RetContext
rtcInit(Foam unit)
{
	RetContext context = (RetContext) stoAlloc(OB_Other, sizeof(*context));
	context->formats = unit->foamUnit.formats;
	context->globals = unit->foamUnit.formats->foamDFmt.argv[globalsSlot];
	context->locDecls = NULL;
	context->parDecls = NULL;
	context->parLocs = NULL;
	context->nUses = NULL;
	context->nLocals = 0;
	return context;
}

RetContext
rtcNewProg(RetContext global, Foam prog, int nLocals)
{
	int nParams = foamDDeclArgc(prog->foamProg.params);
	int i;

	RetContext context = (RetContext) stoAlloc(OB_Other, sizeof(*context));

	context->locDecls = (Foam *) stoAlloc(OB_Other, nLocals * sizeof(Foam));
	context->parDecls = (Foam *) stoAlloc(OB_Other, nParams * sizeof(Foam));
	context->parLocs = NULL;
	context->nUses = (int *) stoAlloc(OB_Other, nParams * sizeof(int));
	context->nLocals = nLocals;

	context->formats = global->formats;
	context->globals = global->globals;
	context->prog = prog;
	for (i = 0; i < nLocals; i++) {
		context->locDecls[i] = foamCopy(context->prog->foamProg.locals->foamDDecl.argv[i]);
	}
	for (i = 0; i < nParams; i++) {
		context->parDecls[i] = foamCopy(context->prog->foamProg.params->foamDDecl.argv[i]);
		context->nUses[i] = 0;
	}
	return context;
}

void
rtcFree(RetContext context)
{
	stoFree(context->locDecls);
	stoFree(context->parDecls);
	stoFree(context->parLocs);
	stoFree(context->nUses);
	stoFree(context);
}

FoamTag
rtcFoamExprType(RetContext context, Foam foam, AInt *pfmt)
{
	return foamExprType0(foam, context->prog, context->formats,
			     NULL, NULL, pfmt);
}

Foam
rtcCurrentDecl(RetContext context, Foam foam)
{
	Foam decl;

	decl = rtcNewDecl(context, foam);
	if (decl != NULL)
		return decl;
	
	return rtcOriginalDecl(context, foam);
}

Foam
rtcOriginalDecl(RetContext context, Foam foam)
{
	Foam decl;
	int index;

	switch (foamTag(foam)) {
	case FOAM_Loc:
		index = foam->foamLoc.index;
		decl = context->prog->foamProg.locals->foamDDecl.argv[index];
		break;
	case FOAM_Par:
		index = foam->foamPar.index;
		decl = context->prog->foamProg.params->foamDDecl.argv[index];
		break;
	default:
		decl = NULL;
		bug("bad case");
	}
	return decl;
}

Bool
rtcHasNewDecl(RetContext context, Foam foam)
{
	Foam decl = rtcNewDecl(context, foam);
	if (decl == NULL)
		return false;
	
	return true;
}

Foam
rtcNewDecl(RetContext context, Foam foam)
{
	Foam decl;
	int index;

	switch (foamTag(foam)) {
	case FOAM_Loc:
		index = foam->foamLoc.index;
		if (index < context->nLocals)
			decl = context->locDecls[index];
		else
			decl = NULL;
		break;
	case FOAM_Par:
		index = foam->foamPar.index;
		decl = context->parDecls[index];
		break;
	default:
		decl = NULL;
		bug("bad case");
	}
	return decl;
}

void
rtcSetUnchanged(RetContext context, Foam foam)
{
	AInt index;

	retDEBUG(dbOut, "Not changing %pFoam\n", foam);

	switch (foamTag(foam)) {
	case FOAM_Loc:
		index = foam->foamLoc.index;
		if (index < context->nLocals)
			context->locDecls[index] = NULL;
		break;
	case FOAM_Par:
		index = foam->foamPar.index;
		context->parDecls[index] = NULL;
		break;
	default:
		bug("bad case");
	}
}

void
rtcAddUse(RetContext context, Foam foam)
{
	if (foamTag(foam) == FOAM_Par) {
		context->nUses[foam->foamPar.index]++;
	}
}

void
rtcSetType(RetContext context, Foam foam, FoamTag type, AInt fmt)
{
	Foam decl;
	
	decl = rtcNewDecl(context, foam);
	if (decl == NULL) {
		return;
	}
	retDEBUG(dbOut, "Changing %pFoam from %s(%d) to %s (%d)\n", foam,
		 foamStr(decl->foamDecl.type), decl->foamDecl.format, 
		 foamStr(type), fmt);

	if (type == FOAM_Rec && decl->foamDecl.type == FOAM_Rec) {
		if (fmt == 0 && decl->foamDecl.format != 0)
			bug("Bad conversion");
	}

	decl->foamDecl.type = type;
	decl->foamDecl.format = fmt;
}

local void
retAddUse(RetContext context, FoamTag type, AInt fmt, Foam foam)
{

	Foam currentDecl = rtcCurrentDecl(context, foam);
	Foam originalDecl = rtcOriginalDecl(context, foam);
	FoamTag currentType = currentDecl->foamDecl.type;
	AInt currentFmt = currentDecl->foamDecl.format;

	rtcAddUse(context, foam);

	if (!retIsCompatible(context, currentDecl, type, fmt)) {
		rtcSetUnchanged(context, foam);
		return;
	}

	if (!retCanConvert(originalDecl->foamDecl.type, type)) {
		return;
	}

	if (type == FOAM_Rec && fmt == 0) {
		return;
	}

	if (currentType == type && currentFmt == fmt)
		return;

	rtcSetType(context, foam, type, fmt);
}

local Bool
retIsCompatible(RetContext context, Foam currentDecl, FoamTag type, AInt fmt)
{
	FoamTag currentType = currentDecl->foamDecl.type;
	FoamTag currentFmt = currentDecl->foamDecl.format;
	if (currentType == FOAM_Word)
		return true;

	switch (type) {
	case FOAM_Rec:
		if (currentType == FOAM_Rec)
			return currentFmt == 0 || currentFmt == fmt || fmt == 0;
		return false;
	case FOAM_Arr:
		if (currentType == FOAM_Arr)
			return currentFmt == 0 || currentFmt == fmt || fmt == 0;
		return false;
	default:
		if (currentType == type)
			return true;
		break;
	}

	return false;
}


/*
 * T --> T       no
 * X --> Word    no
 * Word --> Any  yes
 * Any --> Any   no
 */
local Bool
retCanConvert(FoamTag fromType, FoamTag toType)
{
	if (fromType == toType) {
		return false;
	}
	if (toType == FOAM_Word) {
		return false;
	}
	if (toType == FOAM_Clos) {
		return false;
	}
	if (toType == FOAM_Ptr) {
		return false;
	}
	if (fromType == FOAM_Word) {
		return true;
	}
	return false;
}


void
retypeUnit(Foam foam)
{
	RetContext globals;
	int i;

	assert(foamTag(foam) == FOAM_Unit);
	globals = rtcInit(foam);

	for (i = 0; i < foamArgc(foam->foamUnit.defs); i++) {
		Foam decl, def;

		def = foam->foamUnit.defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.lhs) != FOAM_Const) continue;
		if (foamTag(def->foamDef.rhs) != FOAM_Prog) continue;

		decl = foamUnitConstants(foam)->foamDDecl.argv[i];

		retDEBUG(dbOut, "(Retype begins.. %d - %s\n", i, decl->foamDecl.id);

		retRetypeProg(globals, def->foamDef.rhs);
		
		retDEBUG(dbOut, " Retype ends.. %d - %s)\n", i, decl->foamDecl.id);
	}

	rtcFree(globals);
}

local void 
retRetypeProg(RetContext globals, Foam prog)
{
	Bool changed = true;
	int nLocals = foamDDeclArgc(prog->foamProg.locals);
	while (changed) {
		RetContext context = rtcNewProg(globals, prog, nLocals);

		retDEBUG(dbOut, "Initial:\n%pFoam\n", prog);
		retPeepCasts(prog);

		retMarkCasts(context, prog);

		changed = rtcRearrangeProg(context);

		retDEBUG(dbOut, "Final:\n%pFoam\n", prog);
		rtcFree(context);
	}
}

local void
retMarkCasts(RetContext context, Foam prog)
{
	retMarkExpr(context, prog->foamProg.body);
}

local Foam 
retLocal(Foam foam)
{
	switch (foamTag(foam)) {
	case FOAM_Loc:
	case FOAM_Par:
		return foam;
	case FOAM_Cast:
		return retLocal(foam->foamCast.expr);
	case FOAM_RElt:
		return retLocal(foam->foamRElt.expr);
		/* FOAM_Arr, FOAM_TRElt, etc */
	default:
		return NULL;
	}

	return foam;
}

local Bool
retIsLocal(Foam foam)
{
	return retLocal(foam) != NULL;
}


local void
retMarkExpr(RetContext context, Foam foam)
{
	foamIter(foam, arg, retMarkExpr(context, *arg););

	switch (foamTag(foam)) {
	case FOAM_Cast:
		if (retIsLocal(foam)) {
			retAddUse(context, foam->foamCast.type, 0, retLocal(foam));
		}
		break;
	case FOAM_RElt:
		if (retIsLocal(foam)) {
			retAddUse(context, FOAM_Rec, foam->foamRElt.format, retLocal(foam));
		}
		break;
	case FOAM_PCall:
		retMarkPCall(context, foam);
		break;
	case FOAM_Set:
	case FOAM_Def:
		break;

	default:
		break;
	}
}

local void
retMarkPCall(RetContext context, Foam foam)
{
	switch (foam->foamPCall.protocol) {
	case FOAM_Proto_Java:
	case FOAM_Proto_JavaMethod:
	case FOAM_Proto_JavaConstructor:
		retMarkPCallJava(context, foam);
	default:
		break;
	}
}

local void
retMarkPCallJava(RetContext context, Foam foam)
{
	Foam op = foam->foamPCall.op;

	if (foamTag(op) != FOAM_Glo) {
		return;
	}

	Foam gdecl = context->globals->foamDDecl.argv[op->foamGlo.index];
	Foam ddecl = context->formats->foamDFmt.argv[gdecl->foamGDecl.format];

	for (int i=0; i<foamPCallArgc(foam); i++) {
		Foam arg = foam->foamPCall.argv[i];
		if (retIsLocal(arg)) {
			retAddUse(context,
				  ddecl->foamDDecl.argv[i+1]->foamDecl.type,
				  ddecl->foamDDecl.argv[i+1]->foamDecl.format,
				  retLocal(arg));
		}
	}
}



Bool
rtcRearrangeProg(RetContext context)
{
	FoamBox newLocals;
	FoamList newAssignments;
	Foam prog = context->prog;
	int i;
	int paramCount = foamDDeclArgc(prog->foamProg.params);
	int changeCount = 0;
	/* Mark locals that should be converted */
	for (i = 0; i < context->nLocals; i++) {
		Foam origDecl = prog->foamProg.locals->foamDDecl.argv[i];
		if (context->locDecls[i] == NULL) {
		}
		else if (foamDeclEqual(origDecl,
				  context->locDecls[i])) {
			foamFree(context->locDecls[i]);
			context->locDecls[i] = NULL;
		}
		else if (origDecl->foamDecl.symeIndex != SYME_NUMBER_UNASSIGNED) {
			foamFree(context->locDecls[i]);
			context->locDecls[i] = NULL;
		}
		else {
			changeCount++;
		}
	}
	
	/* Mark params that should be converted */
	for (i = 0; i < paramCount; i++) {
		Foam origDecl = prog->foamProg.params->foamDDecl.argv[i];
		if (context->parDecls[i] == NULL) {
		}
		else if (foamDeclEqual(origDecl,
				      context->parDecls[i])) {
			foamFree(context->parDecls[i]);
			context->parDecls[i] = NULL;
		}
		else if (context->nUses[i] <= 1) {
			foamFree(context->parDecls[i]);
			context->parDecls[i] = NULL;
		}
		else if (origDecl->foamDecl.symeIndex != -1) {
			foamFree(context->parDecls[i]);
			context->parDecls[i] = NULL;
		}
		else {
			changeCount++;
		}

	}
	
	if (changeCount == 0)
		return false;

	/* Deal with values in set statements */
	rtcRearrangeMultiAssign(context);

	/* Parameters */
	newLocals = fboxNew(prog->foamProg.locals);
	newAssignments = listNil(Foam);

	context->parLocs = (int *) stoAlloc(OB_Other,
					    sizeof(Foam) * foamDDeclArgc(prog->foamProg.params));
	for (i = 0; i < foamDDeclArgc(prog->foamProg.params); i++) {
		Foam newDecl = context->parDecls[i];
		if (context->parDecls[i] != NULL) {
			int id = fboxAdd(newLocals, newDecl);
			Foam newAssignment = foamNewSet(foamNewLoc(id),
							foamNewCast(newDecl->foamDecl.type,
								    foamNewPar(i)));
			newAssignments = listCons(Foam)(newAssignment, newAssignments);
			context->parLocs[i] = id;
		}
		else {
			context->parLocs[i] = -1;
		}
	}
	prog->foamProg.locals = fboxMake(newLocals);

	if (newAssignments == listNil(Foam)) 
		retRearrangeExpr(context, context->prog->foamProg.body, false);
	else {
		int bodyArgc = foamArgc(prog->foamProg.body);
		Foam newSeq = foamNewEmpty(FOAM_Seq,
					   bodyArgc + listLength(Foam)(newAssignments));
		int i = 0, j = 0;
		while (newAssignments != listNil(Foam)) {
			newSeq->foamSeq.argv[i] = car(newAssignments);
			newAssignments = listFreeCons(Foam)(newAssignments);
			i++;
		}
		
		for (j = 0; j < bodyArgc; j++) {
			newSeq->foamSeq.argv[i] = retRearrangeExpr(context,
								   prog->foamProg.body->foamSeq.argv[j],
								   false);
			i++;
		}
		foamFreeNode(prog->foamProg.body);
		prog->foamProg.body = newSeq;
	}

	for (i = 0; i < context->nLocals; i++) {
		if (context->locDecls[i] != NULL) {
			foamFreeNode(prog->foamProg.locals->foamDDecl.argv[i]);
			prog->foamProg.locals->foamDDecl.argv[i] = context->locDecls[i];
		}
	}

	return true;
}

local void
rtcRearrangeMultiAssign(RetContext context)
{
	Foam prog = context->prog;
	Bool hasMultiAssign = foamProgHasMultiAssign(prog);
	int i;

	if (hasMultiAssign) {
		FoamBox tmpLocals = fboxNew(prog->foamProg.locals);
		FoamBox stmts = fboxNewEmpty(FOAM_Seq);
		Foam seq = prog->foamProg.body;
		int bodyArgc = foamArgc(seq);
		for (i=0; i<bodyArgc; i++) {
			Foam stmt = seq->foamSeq.argv[i];
			if (!foamIsMultiAssign(stmt)) 
				fboxAdd(stmts, stmt);
			else {
				Foam lhs = stmt->foamSet.lhs;
				Foam rhs = stmt->foamSet.rhs;
				FoamList extraVars = rtcMultiAssignVars(context, tmpLocals, lhs);
				Foam modifiedValues = rtcMultiAssignValues(context, extraVars, lhs);
				FoamList extraStmts = rtcMultiAssignFollows(context, extraVars, lhs);
				
				fboxAdd(stmts, foamNewSet(modifiedValues, rhs));
				while (extraStmts != listNil(Foam)) {
					fboxAdd(stmts, car(extraStmts));
					extraStmts = listFreeCons(Foam)(extraStmts);
				}
				while (extraVars != listNil(Foam)) {
					foamFreeNode(car(extraVars));
					extraVars = listFreeCons(Foam)(extraVars);
				}
			}
		}
		prog->foamProg.locals = fboxMake(tmpLocals);
		prog->foamProg.body = fboxMake(stmts);
	}
}


local FoamList
rtcMultiAssignVars(RetContext context, FoamBox tempLocals, Foam lhs)
{
	FoamList extraVars = listNil(Foam);
	int i;

	for (i=0; i<foamArgc(lhs); i++) {
		Foam loc = lhs->foamValues.argv[i];
		if (!rtcHasNewDecl(context, loc)) 
			extraVars = listCons(Foam)(NULL, extraVars);
		else {
			int id = fboxAdd(tempLocals, foamCopy(rtcOriginalDecl(context, loc)));
			extraVars = listCons(Foam)(foamNewLoc(id), extraVars);
		}	
	}
	return listNReverse(Foam)(extraVars);
}

local Foam
rtcMultiAssignValues(RetContext context, FoamList extraVars, Foam lhs)
{
	FoamBox box = fboxNewEmpty(FOAM_Values);
	int i;
	for (i=0; i<foamArgc(lhs); i++) {
		if (car(extraVars) == NULL)
			fboxAdd(box, lhs->foamValues.argv[i]);
		else
			fboxAdd(box, foamCopy(car(extraVars)));
		extraVars = cdr(extraVars);
	}
	
	return fboxMake(box);
}

local FoamList
rtcMultiAssignFollows(RetContext context, FoamList extraVars, Foam lhs)
{
	FoamList follows = listNil(Foam);
	int i;

	for (i=0; i<foamArgc(lhs); i++) {
		if (car(extraVars) != NULL) {
			follows = listCons(Foam)(foamNewSet(lhs->foamValues.argv[i],
							    foamCopy(car(extraVars))),
						 follows);
		}

		extraVars = cdr(extraVars);
	}
	return listNReverse(Foam)(follows);
}

local Foam
retRearrangeExpr(RetContext context, Foam expr, Bool isLhs)
{
	Foam result;
	Foam orig = expr;
	
	switch (foamTag(expr)) {
	case FOAM_Set:
	case FOAM_Def:
		orig = foamCopy(expr);
		result = retRearrangeSet(context, expr);
		return result;
	}
	foamIter(expr, arg, *arg = retRearrangeExpr(context, *arg, false));

	switch (foamTag(expr)) {
	case FOAM_Loc:
	case FOAM_Par:
		orig = foamCopy(expr);
		result = retRearrangeVar(context, expr);
		break;
	case FOAM_Cast:
		if (foamTag(expr->foamCast.expr) == FOAM_Cast) {
			Foam value = expr->foamCast.expr->foamCast.expr;
			foamFreeNode(expr->foamCast.expr);
			expr->foamCast.expr = value;
		}
		result = expr;
		break;
	default:
		result = expr;
		break;
	}

	if (expr != result) {
		retDEBUG(dbOut, " Rearranged: %pFoam to %pFoam\n", orig, result);
	}
	return result;
}

local Bool 
retIsVar(Foam foam)
{
	return foamTag(foam) == FOAM_Loc || foamTag(foam) == FOAM_Par;
}

local Foam
retRearrangeSet(RetContext context, Foam set)
{
	Foam decl;
	Foam lhs = set->foamSet.lhs;
	Foam rhs = set->foamSet.rhs;

	set->foamSet.rhs = retRearrangeExpr(context, rhs, false);

	if (!retIsVar(lhs)) {
		set->foamSet.lhs = retRearrangeExpr(context, lhs, true);
		return set;
	}
	if (!rtcHasNewDecl(context, lhs)) {
		return set;
	}
	decl = rtcCurrentDecl(context, lhs);

	foamFreeNode(set);

	return foamNewSet(lhs, foamNewCast(decl->foamDecl.type, rhs));
}

local Foam
retRearrangeVar(RetContext context, Foam var)
{
	Foam originalDecl;
	int index;

	if (!rtcHasNewDecl(context, var))
		return var;

	originalDecl = rtcOriginalDecl(context, var);

	index = var->foamPar.index;
	if (foamTag(var) == FOAM_Par
	    && context->parLocs[index] != -1)  {
		foamFree(var);
		var = foamNewLoc(context->parLocs[index]);
	}
	return foamNewCast(originalDecl->foamDecl.type, var);
}

local Foam
retCast(RetContext context, FoamTag type, Foam foam)
{
	FoamTag current;

	while (foamTag(foam) == FOAM_Cast)
		foam = foam->foamCast.expr;

	current = rtcFoamExprType(context, foam, NULL);

	if (type != current)
		return foamNewCast(type, foam);
	else
		return foam;
}


local Foam
retPeepCasts(Foam foam)
{
	Foam value;
	AInt type;

	foamIter(foam, arg, *arg = retPeepCasts(*arg););

	if (foamTag(foam) != FOAM_Cast) return foam;

	value = foam->foamCast.expr;

	/* Dereference Casts releasing unused nodes */
	while (foamTag(value) == FOAM_Cast) {
		Foam expr = value->foamCast.expr;
		foamFreeNode(value);
		value = expr;
	}

	if (foamTag(value) == FOAM_BCall)
		type =  foamBValInfo(value->foamBCall.op).retType;

	else if (foamTag(value) == FOAM_CCall)
	    	type = value->foamCCall.type;
	else
		type = FOAM_NOp;

	if (foam->foamCast.type == type) {
		foamFreeNode(foam);
		return value;
	}
	else {
		foam->foamCast.expr = value;
		return foam;
	}
}
