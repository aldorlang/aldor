/*****************************************************************************
 *
 * flatten.c: Take a foam program and flatten it
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "flatten.h"
#include "store.h"
#include "syme.h"
#include "strops.h"

/******************************************************************************
 *
 * :: Macros
 *
 *****************************************************************************/
Bool	flatDebug = true;

#define flatDEBUG	DEBUG_IF(flatDebug)

/******************************************************************************
 *
 * :: Global Data Structures
 *
 *****************************************************************************/

typedef struct _Var {
	Foam decl;
	int  locNo;
} *Var;

DECLARE_LIST(Var);
CREATE_LIST(Var);

/******************************************************************************
 *
 * :: Local Prototypes
 *
 *****************************************************************************/

local void 	flatProg	(Foam);
local void 	flatInit	(Foam);
local void 	flatRebuildProg (Foam);
local Foam 	flatExpr	(Foam, Bool);
local Foam 	flatCall	(Foam);
local Foam 	flatUnNestCall	(Foam);
local Foam 	flatNewLocal	(FoamTag, AInt);
local FoamTag 	flatExprType	(Foam, AInt *);
local void 	flatAddStmt	(Foam);
local Var  	flatNewVar	(Foam, int);
local void 	flatVarFree	(Var);

/******************************************************************************
 *
 * :: Global state
 *
 *****************************************************************************/

static VarList  flatUsed;
static VarList  flatFree;
static int	flatNLocals;
static FoamList flatStmts;

static Foam	flatProgram;
static Foam     flatFormats;
/******************************************************************************
 *
 * :: Main Entry Point
 *
 *****************************************************************************/
/*
 * A _flat_ piece of foam is one which makes no nested 
 * CCalls, OCalls etc.  BCalls can be nested (I think)
 */

void
flattenUnit(Foam foam)
{
	int i;
	Foam def;

	assert(foamTag(foam) == FOAM_Unit);
	
	flatFormats = foam->foamUnit.formats;

	/* !!! Bugged - assumes foam->foamUnit.defs is valid */
	for (i = 0; i < foamArgc(foam->foamUnit.defs); i++) {
		def = foam->foamUnit.defs->foamDDef.argv[i];

		if (foamTag(def->foamDef.lhs) != FOAM_Const) continue;
		if (foamTag(def->foamDef.rhs) != FOAM_Prog) continue;

		flatDEBUG{fprintf(dbOut, "(Start: %d nlocals: %d\n", i,
				  (int) foamDDeclArgc(def->foamDef.rhs->foamProg.locals));}
		flatProg(def->foamDef.rhs);
		flatDEBUG{fprintf(dbOut, "Done)\n");}
	}
}

/******************************************************************************
 *
 * :: Local Function Definitions
 *
 *****************************************************************************/

/*
 * Algorithm used is the same as that in genc.c --- keep a pool of variables
 * to hand, and make sure that we allocate/deallocate correctly.
 */

local void
flatProg(Foam prog)
{
	Foam seq;
	int i;
	flatInit(prog);
	flatProgram = prog;
	seq = prog->foamProg.body;
	for (i=0; i<foamArgc(seq); i++) {
		flatAddStmt(flatExpr(seq->foamSeq.argv[i], false));
	}
	flatProgram = NULL;
	flatRebuildProg(prog);
}

local void
flatInit(Foam prog)
{
	flatStmts = listNil(Foam);
	flatFree  = listNil(Var);
	flatUsed  = listNil(Var);

	flatNLocals = foamDDeclArgc(prog->foamProg.locals);
}

local void
flatRebuildProg(Foam prog)
{
	FoamList stmts;
	Foam ddecl;
	int  i;

	assert(flatUsed == listNil(Var));
	ddecl = foamNewEmpty(FOAM_DDecl, 1+flatNLocals);
	ddecl->foamDDecl.usage = FOAM_DDecl_Local;
	for (i=0; i < foamDDeclArgc(prog->foamProg.locals); i++) 
		ddecl->foamDDecl.argv[i] = prog->foamProg.locals->foamDDecl.argv[i];

	/* Free list should contain all new variables */
	while (flatFree != listNil(Var)) {
		ddecl->foamDDecl.argv[i++] = car(flatFree)->decl;
		flatVarFree(car(flatFree));
		flatFree = listFreeCons(Var)(flatFree);
	}
	foamFreeNode(prog->foamProg.locals);
	prog->foamProg.locals = ddecl;

	stmts = listNReverse(Foam)(flatStmts);
	prog->foamProg.body = foamNewOfList(FOAM_Seq, stmts);
	listFree(Foam)(stmts);
}


local Foam
flatExpr(Foam expr, Bool nest)
{
	Foam newExpr = expr;

	switch(foamTag(expr)) {
	  case FOAM_Set:
	  case FOAM_Def: 
	  case FOAM_MFmt:
		foamIter(expr, arg, *arg = flatExpr(*arg, nest));
		break;
	  default:
		newExpr = flatCall(expr);
		break;
	}
	switch (foamTag(expr)) {
	  case FOAM_CCall:
	  case FOAM_OCall:
		if (nest) newExpr = flatUnNestCall(expr);
		break;
	  default:	
		break;
	}

	return newExpr;
}

local Foam
flatCall(Foam call)
{
	VarList prevUsed;
	
	prevUsed = flatUsed;

	foamIter(call, arg, *arg = flatExpr(*arg, true));
	
	while (flatUsed != prevUsed) {
		flatDEBUG{fprintf(dbOut, "Freeing: %d\n", car(flatUsed)->locNo);}
		flatFree = listCons(Var)(car(flatUsed), flatFree);
		flatUsed = cdr(flatUsed);
	}
	return call;
}

local Foam
flatUnNestCall(Foam call)
{
	Foam loc;
	FoamTag tag;
	AInt fmt;
	
	tag = flatExprType(call, &fmt);
	
	if (fmt == emptyFormatSlot && (tag == FOAM_Rec || tag == FOAM_TR)) 
		tag = FOAM_Word;

	loc = flatNewLocal(tag, fmt);
	
	flatAddStmt(foamNewSet(loc, call));

	return foamCopy(loc);
}


local void
flatAddStmt(Foam foam)
{
	flatStmts = listCons(Foam)(foam, flatStmts);
}

local Foam
flatNewLocal(FoamTag tag, AInt fmt)
{
	VarList *plst;
	Foam	decl;
	Var	var = NULL;

	/* Check the free list */
	plst = &flatFree;
	while (*plst != listNil(Var)) {
		if (car(*plst)->decl->foamDecl.type == tag 
		    && car(*plst)->decl->foamDecl.format == fmt) {
			VarList tmp = *plst;
			var   = car(*plst);
			*plst = cdr(*plst);
			listFreeCons(Var)(tmp);
			flatDEBUG{fprintf(dbOut, "Re-using: %d\n", var->locNo);}
			break;
		}
		plst = &cdr(*plst);
	}

	/* O/wise, get a _really_ new variable */
	if (var == NULL) {
		int idx = flatNLocals++;
		decl = foamNewDecl(tag, strCopy("tmp"), fmt);
		var = flatNewVar(decl, idx);
		flatDEBUG{fprintf(dbOut, "New Var: %d\n", var->locNo);}
	}

	flatUsed = listCons(Var)(var, flatUsed);
	return foamNewLoc(var->locNo);
}

local FoamTag
flatExprType(Foam foam, AInt *pfmt)
{
	return foamExprType(foam, flatProgram, flatFormats, NULL, NULL, pfmt);
}

local Var
flatNewVar(Foam decl, int idx)
{
	Var var = (Var) stoAlloc(OB_Other, sizeof(*var));
	var->decl  = decl;
	var->locNo = idx;

	return var;
}

local void
flatVarFree(Var var)
{
	stoFree(var);
}
