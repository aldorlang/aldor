/****************************************************************************
 *
 * ti_decl.c: Type inference -- declaration pass.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ***************************************************************************/

#include "debug.h"
#include "fluid.h"
#include "format.h"
#include "stab.h"
#include "store.h"
#include "terror.h"
#include "ti_bup.h"
#include "ti_decl.h"
#include "ti_tdn.h"
#include "tinfer.h"


struct _pendingDecl
{
	AbSyn	absyn;	/* AB_Declare node to be tibup'd */
	Stab	stab;	/* Symbol table in which to tibup(absyn) */
	TForm	type;	/* Type constraint on absyn */
};
typedef struct _pendingDecl *PendingDecl;

DECLARE_LIST(PendingDecl);
CREATE_LIST(PendingDecl);

/* tiDeclPending: decl nodes waiting for processing */
static PendingDeclList pdPending = NULL;

/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

Bool	tipDeclDebug		= false;
#define tipDeclDEBUG(s)		DEBUG_IF(tipDeclDebug, s)

/*****************************************************************************
 *
 * :: Other fluids and globals
 *
 ****************************************************************************/

static Bool	  tloopBreakCount    = -1;/* Handle loop exits */
static SymbolList terrorIdComplaints = 0; /* Id complaint list for scope. */

/*****************************************************************************
 *
 * :: Declarations for breadth-first declaration pass
 *
 ****************************************************************************/

local PendingDecl pdNew(Stab stab, AbSyn absyn, TForm type);
local void tidecl(Stab stab, AbSyn absyn, TForm type);
local void tideclAssign(Stab stab, AbSyn absyn, TForm type);
local void tideclDeclare(Stab stab, AbSyn absyn, TForm type);
local void tideclDefine(Stab stab, AbSyn absyn, TForm type);
local void tideclGeneric(Stab stab, AbSyn absyn, TForm type);

/*****************************************************************************
 *
 * :: Support functions for breadth-first declaration pass
 *
 ****************************************************************************/

local PendingDecl
pdNew(Stab stab, AbSyn absyn, TForm type)
{
	PendingDecl result = (PendingDecl)stoAlloc(OB_Other, sizeof(*result));
	result->absyn = absyn;
	result->stab = stab;
	result->type = type;
	return result;
}

#define pdAbSyn(pd) ((pd)->absyn)
#define pdStab(pd) ((pd)->stab)
#define pdType(pd) ((pd)->type)


/*****************************************************************************
 *
 * :: Declaration pass
 *
 ****************************************************************************/

void
tiDeclarations(Stab stab, AbSyn absyn, TForm type)
{
	Scope("tiDeclarations");
	int	passNo = 0;

	Bool	   fluid(tloopBreakCount);
	SymbolList fluid(terrorIdComplaints);
	PendingDeclList fluid(pdPending);
	PendingDeclList pds;

	pdPending = NULL;
	tloopBreakCount	   = -1;
	terrorIdComplaints = 0;

	/* Add the top-level node to the pending list */
	pdPending = listCons(PendingDecl)(pdNew(stab, absyn, type), pdPending);

	/* Repeatedly process pending nodes */
	while (pdPending) {
		/* Grab the pending list for ourselves */
		pds = listNReverse(PendingDecl)(pdPending);
		pdPending = NULL;
		passNo++;

		/* Debugging */
		tipDeclDEBUG({
			(void)fprintf(dbOut,"------>Decl: pass %d\n", passNo);
		});

		/* Process each node in the to-do list */
		for (; pds; pds = listFreeCons(PendingDecl)(pds)) {
			PendingDecl pd = car(pds);

			tidecl(pdStab(pd), pdAbSyn(pd), pdType(pd));
			stoFree(pd);
		}

		/* Debugging */
		tipDeclDEBUG({
			(void)fprintf(dbOut,"<------Decl: pass %d\n", passNo);
		});
	}

	/* Clean up and return */
	listFree(Symbol)(terrorIdComplaints);
	ReturnNothing;
}


/*
 * Add all child nodes to pdPending.
 */
local void
tidecl(Stab stab, AbSyn absyn, TForm type)
{
	static int	serialNo = 0, depthNo = 0;
	int		serialThis;
	assert(absyn);

	/* This ought to be impossible but ... */
	if (abState(absyn) >= AB_State_HasPoss) return;

	/* Use the local stab if present */
	if (!abIsLeaf(absyn) && abStab(absyn)) {
		stab = abStab(absyn);
		stabSeeOuterImports(stab);
		stabGetSubstable(stab);
		typeInferTForms(stab);
	}

	/* Stab processing might have done our job for us */
	if (abState(absyn) >= AB_State_HasPoss) return;

	/* Debugging stuff */
	serialNo++, depthNo++;
	serialThis = serialNo;

	/* Ignore leaf nodes */
	if (abIsLeaf(absyn)) return;

	/* Process, ignore or add to the pending queue */
	switch (abTag(absyn)) {
	case AB_Assign:		tideclAssign(stab, absyn, type); break;
	case AB_Declare:	tideclDeclare(stab, absyn, type); break;
	case AB_Define:		tideclDefine(stab, absyn, type); break;
	default:		tideclGeneric(stab, absyn, type); break;
	}

	/* More debugging stuff */
	depthNo--;
}

local void
tideclAssign(Stab stab, AbSyn absyn, TForm type)
{
	PendingDecl pd;
	AbSyn	lhs = absyn->abAssign.lhs;
	AbSyn	rhs = absyn->abAssign.rhs;

	/* We are interested if we have "var:Type := expr" */
	if (abTag(lhs) != AB_Declare) {
		/* Process the lhs on the next pass */
		pd = pdNew(stab, lhs, type);
		pdPending = listCons(PendingDecl)(pd, pdPending);
	}
	else
		tideclDeclare(stab, lhs, type);

	/* The RHS is always processed on the next pass */
	pd = pdNew(stab, rhs, type);
	pdPending = listCons(PendingDecl)(pd, pdPending);
}


local void
tideclDeclare(Stab stab, AbSyn absyn, TForm type)
{
	/* Debugging */
	tipDeclDEBUG({
		(void)fprintf(dbOut,"    -->Decl: ");
		abPrettyPrint(dbOut, absyn);
		fnewline(dbOut);
	});

	/* Tinfer the declaration now */
	typeInferAs(stab, absyn, type);
}


local void
tideclDefine(Stab stab, AbSyn absyn, TForm type)
{
	PendingDecl pd;
	AbSyn	lhs = absyn->abDefine.lhs;
	AbSyn	rhs = absyn->abDefine.rhs;

	/* We are interested if we have "var:Type == expr" */
	if (abTag(lhs) != AB_Declare) {
		/* Process the lhs on the next pass */
		pd = pdNew(stab, lhs, type);
		pdPending = listCons(PendingDecl)(pd, pdPending);
	}
	else
		tideclDeclare(stab, lhs, type);

	/* The RHS is always processed on the next pass */
	pd = pdNew(stab, rhs, type);
	pdPending = listCons(PendingDecl)(pd, pdPending);
}


local void
tideclGeneric(Stab stab, AbSyn absyn, TForm type)
{
	Length i;
	PendingDecl pd;

	/* Add all child nodes to the pending queue */
	if (!abIsLeaf(absyn)) {
		for (i = 0; i < abArgc(absyn); i++) {
			pd = pdNew(stab, abArgv(absyn)[i], tfUnknown);
			pdPending = listCons(PendingDecl)(pd, pdPending);
		}
	}
}

