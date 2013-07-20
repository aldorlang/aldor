/*****************************************************************************
 *
 * macex.c: Macro expansion of parse trees.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
/*
 * Macro expansion is a transformation on normalized parse trees.
 *
 * Statements of the form [MDefine a b] provide definitions,
 * and are replaced with null expressions after processing.
 *
 * The scope of a macro definition is the remainder of the lexical scope
 * (function, where, parameterized macro, ...) in which it was introduced.
 *
 */
 
#include "abnorm.h"
#include "debug.h"
#include "fint.h"
#include "format.h"
#include "phase.h"
#include "store.h"
#include "doc.h"
 
# undef MacDeclArgs

Bool	macDebug		= false;
#define	macDEBUG(s)		DEBUG_IF(macDebug, s)

/*****************************************************************************
 *
 * :: Scoping of macro definitions
 *
 *****************************************************************************/
 
/*
 * Macro values are stored in the symbol structure during the macro expansion
 * phase.  This means that no searching is necessary in handling leaf ids.
 */
 
#define macroCell(sym)	(symCoInfo(sym)->phaseVal.macro)
#define hasMacro(sym)	(symInfo(sym) && symCoInfo(sym) && macroCell(sym))
#define isScopeMarker(def) ((def)->kind == MAC_Scope)
#define atScopeMarker(defs) ((defs) && isScopeMarker(car(defs)))
 
 
/*
 * A stack of macro definitions is used for scoping. To enable us to
 * detect when a macro has been redefined, we insert fake macdefs in
 * the definition stack.
 */
enum macKind {
	MAC_Body,	/* a macro body. parameterized -> AB_MLambda */
	MAC_Param,	/* a macro parameter during body expansion */
	MAC_Scope	/* not a macro, just a scope marker */
};
 
typedef struct macDef {
	enum macKind kind;
	Symbol	name;
	AbSyn	oldMacVal;	/* 0 if no previous value. */
} *MacDef;
 
DECLARE_LIST(MacDef);
CREATE_LIST(MacDef);
 
static MacDefList macDefs = listNil(MacDef);	/* All defined macros. */

/* Initialised by macexInitFile from compFileInit */
static PhaseSymbolDataList macexSavedPhaseSymbolDataList;
static AbSynList 	   macexGlobalMacros;
static AbSynList	   macexLocalMacros;
static Bool		   macexUseMacros = 1;

local void	popMacDef	(void);
/****************************************************************************
 ****************************************************************************/

void
macexInitFile(void)
{
	macDefs = listNil(MacDef); /* 0; */
	macexSavedPhaseSymbolDataList = 0;
	macexGlobalMacros = listNil(AbSyn);
	macexLocalMacros = listNil(AbSyn);
}

void
macexFiniFile()
{
	while (macDefs)
		popMacDef();

	/* Excellent source of memory leaks ... maybe */
	macexSavedPhaseSymbolDataList = 0;
	macexGlobalMacros = listNil(AbSyn);
}
 
/*
 * These operations manage the stack.
 */
local void
initMacDef(void)
{
	if (!fintMode == FINT_LOOP) macDefs = 0;
}
 
 
/*
 * Put a new macro value into the symbol.
 */
local void
pushMacDef0(MacDef md)
{
	AbSyn		oldval;
 
	oldval		    = macroCell(md->name);
	macroCell(md->name) = md->oldMacVal;
	md->oldMacVal	    = oldval;

	/*
	 * Warn if this macro redefines an existing macro in the
	 * same scope level or hides one in an outer scope.
	 */
	if (md->oldMacVal) {
		Bool		found = false;
		MacDefList	mdl = macDefs;

		/* Search for the previous definition in this scope */
		while (mdl && !atScopeMarker(mdl) && !found)
		{
			MacDef	old = car(mdl);

			/* Compare names */
			if (md->name == old->name)
				found = true;


			/* Try the next definition */
			mdl = cdr(mdl);
		}

		/* Are these macros in the same scope? */
		if (found)
		{
			/* Redefine in same scope */
			comsgWarning(macroCell(md->name), ALDOR_W_MacRedefined,
				symString(md->name));
		}
		else
		{
			/* Hides outer meaning */
			comsgWarning(macroCell(md->name), ALDOR_W_MacHides,
				symString(md->name));
		}
	}
 
	macDefs = listCons(MacDef) (md, macDefs);
}
 

local MacDef
newMacDef(Symbol name, enum macKind kind, AbSyn abtree)
{ 
	MacDef md = (MacDef) stoAlloc((unsigned) OB_Other, sizeof(*md));

	md->name = name;
	md->kind = kind;
	md->oldMacVal = abtree ? abCopy(abtree) : 0;
 
	return md;
}

local void
pushMacDef(Symbol name, enum macKind kind, AbSyn abtree)
{
	if (! symInfo(name) || ! symCoInfo(name))
		symCoInfoInit(name);
 
	macDEBUG({
		fprintf(dbOut, "  Pushing %s with definition ",
			symString(name));
		if (! abtree)
			fprintf(dbOut, "(nothing)");
		else
			abPrint(dbOut, abtree);
		fnewline(dbOut);
	});

	pushMacDef0(newMacDef(name, kind, abtree)); 
}
 
/*
 * Put the previous value back in the symbol then
 * return a cell containing the popped definition.
 */
local MacDef
popMacDef0(void)
{
	MacDef	md = car(macDefs);

	/* Ignore scope markers (they don't represent macro definitions) */
	if (!isScopeMarker(md))
	{
		AbSyn	ab = macroCell(md->name);

		macroCell(md->name) = md->oldMacVal;
		md->oldMacVal = ab;
	}

	/* Remove the definition from the list */
	macDefs	= listFreeCons(MacDef)(macDefs);
	return md;
}
 

local void
popMacDef(void)
{
	MacDef md  = popMacDef0();

	/*
	 * Scope markers don't have absyn but we might as well
	 * be careful and check that the macdef had some absyn.
	 */
	if (md->oldMacVal)
		abFree(md->oldMacVal);

	stoFree((Pointer) md);
}
 
/*
 * Use: mdl = pushMacScope(); .... popMacScope(mdl);
 */
 
typedef MacDefList MacDefScope;
 
local	MacDefScope
pushMacScope(void)
{
	MacDef		md;
	MacDefScope	result = macDefs;

	/* Create a scope marker */
	md = newMacDef((Symbol)NULL, MAC_Scope, (AbSyn)NULL);


	/* Push it onto the macro stack */
	macDefs = listCons(MacDef) (md, macDefs);
	
	return result;
}
 
local void
popMacScope(MacDefScope mds)
{
	if (fintMode == FINT_LOOP) return;

	while (macDefs && macDefs != mds)
		popMacDef();
}
 
/*
 * Pop the scope, but return the list of popees.
 */
local MacDefList
popMacScopeDefList(MacDefScope mds)
{
	MacDefList	mdl = 0;
 
	while (macDefs && macDefs != mds)
	{
		MacDef md = popMacDef0();

		if (!isScopeMarker(md))
			mdl = listCons(MacDef)(md, mdl);
	}
 
	return mdl;
}
 
/*
 * Push a list of macdefs.
 */
local void
pushMacDefList(MacDefList mdl)
{
	while (mdl) {
		pushMacDef0(car(mdl));
		mdl = cdr(mdl);
	}
}
 
/******************************************************************************
 *
 * :: 'Global' macros
 *
 ******************************************************************************/

void
macexSetUseGlobalMacros(Bool flg)
{
	/* Never invoked ... */
	macexUseMacros = flg;
}

void
macexAddMacro(AbSyn ab, Bool flg)
{
	macexGlobalMacros = listCons(AbSyn)(abCopy(ab), macexGlobalMacros);
	if (flg)
		macexLocalMacros = listCons(AbSyn)(abCopy(ab), macexLocalMacros);
}

AbSyn
macexGetMacros()
{
	return abNewSequenceL(sposNone, macexLocalMacros);
}

/******************************************************************************
 *
 * :: State of current macro expansion
 *
 ******************************************************************************/
 
static AbSynList abActive;	/* Id-s where the expansions occured. */
static AbSynList macActive;	/* Bodies of active macros. */
 
/*
 * Forward declarations
 */
local AbSyn		macEx			(AbSyn);
local AbSyn		macParts		(AbSyn);
 
local AbSyn		macAdd			(AbSyn);
local AbSyn		macApply		(AbSyn);
local AbSyn		macDefine		(AbSyn);
local AbSyn		macId			(AbSyn);
local AbSyn		macLambda		(AbSyn);
local AbSyn		macMacro		(AbSyn);
local AbSyn		macMDefine		(AbSyn);
local AbSyn		macMLambda		(AbSyn);
local AbSyn		macWhere		(AbSyn);
local AbSyn		macWith			(AbSyn);
 
local String		formatActiveMacroChain	(AbSynList, String);
 
/*
 * Top-level entry point for macro expansion of parse tree.
 */
 
AbSyn
macroExpand(AbSyn ab)
{
	MacDefScope mds;
	AbSynList   ll;
 
	abActive = 0;
	macActive = 0;
 
	initMacDef();

	/* Restore data from previous invocation (if any) */
	if (macexSavedPhaseSymbolDataList) {
		restoreAllPhaseSymbolData(macexSavedPhaseSymbolDataList);
		macexSavedPhaseSymbolDataList = 0;
	}
	
	mds = pushMacScope();

	/* Process global (exported) macro definitions */ 
	ll = macexGlobalMacros;
	while (ll && macexUseMacros) {
		/* The call to macEx frees tmp */
		AbSyn tmp = abCopy(car(ll));
		macEx(tmp);
		ll = cdr(ll);
	}

	/* Macro expand the whole parse tree */
	ab = macEx(ab);
 
	popMacScope(mds);

	/* Store interesting data for subsequent invocations */
	macexSavedPhaseSymbolDataList =
		saveAndEmptyAllPhaseSymbolData();

	return ab;
}
 
local AbSyn
macEx(AbSyn ab)
{
	if (!ab)
		return ab;
 
	macDEBUG({
		findent += 2;
		fprintf(dbOut, "  In Macro expansion for: ");
		abPrint(dbOut, ab);
		fnewline(dbOut);
	});
 
	switch (abTag(ab)) {
	case AB_Id:
		ab = macId(ab);
		break;
	case AB_Add:
		ab = macAdd(ab);
		break;
	case AB_Apply:
		ab = macApply(ab);
		break;
	case AB_Define:
		ab = macDefine(ab);
		break;
	case AB_Lambda:
	case AB_PLambda:
		ab = macLambda(ab);
		break;
	case AB_Macro:
		ab = macMacro(ab);
		break;
	case AB_MDefine:
		ab = macMDefine(ab);
		break;
	case AB_MLambda:
		ab = macMLambda(ab);
		break;
	case AB_Where:
		ab = macWhere(ab);
		break;
	case AB_With:
		ab = macWith(ab);
		break;
	default:
		ab = macParts(ab);
		break;
	}
 
	macDEBUG({
		fprintf(dbOut, "Returning: ");
		abPrint(dbOut, ab);
		findent -= 2;
		fnewline(dbOut);
	});
 
	return ab;
}
 
local AbSyn
macParts(AbSyn ab)
{
	int	i;
 
	if (!abIsLeaf(ab))
		for (i = 0; i < abArgc(ab); i++)
			abArgv(ab)[i] = macEx(abArgv(ab)[i]);
	return ab;
}
 
local AbSyn
macId(AbSyn ab)
{
	Symbol	sy = ab->abId.sym;
 
	if (hasMacro(sy)) {
		Doc     thedoc;
		AbSyn	mac = macroCell(sy);
		Bool	isCircle = listMemq(AbSyn) (macActive, mac);
 
		/* remember to transfer any doc */
		if ((thedoc = abComment(ab)))
			abSetComment(mac,docCopy(thedoc));
		
		abActive = listCons(AbSyn) (ab, abActive);
		macActive = listCons(AbSyn) (mac, macActive);
 
		if (isCircle) {
			String	s = formatActiveMacroChain(abActive, "->");
 
			comsgError(ab, ALDOR_E_MacInfinite, s);
			strFree(s);
		}
		else {
			mac = abCopy(mac);
			abNormal(mac, false);
			if (comsgOkMacText())
				abMarkAsMacroExpanded(mac);
			else
				abReposition(mac,
					     sposMacroExpanded(abPos(ab)),
					     sposMacroExpanded(abEnd(ab)));
			mac = macEx(mac);
			abFree(ab);
			ab = mac;
		}
		abActive = listFreeCons(AbSyn) (abActive);
		macActive = listFreeCons(AbSyn) (macActive);
	}
	return ab;
}
 
local AbSyn
macAdd(AbSyn ab)
{
	MacDefScope mds = pushMacScope();
 
	ab->abAdd.base = macEx(ab->abAdd.base);
	ab->abAdd.capsule = macEx(ab->abAdd.capsule);
 
	popMacScope(mds);
	return ab;
}
 
local AbSyn
macApply(AbSyn ab)
{
	int	i, n;
	AbSyn	ab0 = ab, maclam, params, args;
 
	/* Normal macro expansion. */
 
	n = abArgc(ab);
	for (i = 0; i < n; i++)
		abArgv(ab)[i] = macEx(abArgv(ab)[i]);
 
	/* Return if not application of parameterized macro. */
 
	maclam = ab->abApply.op;
	if (!maclam || ! abHasTag(maclam, AB_MLambda))
		return ab;
 
	/* Add parameters as new macros, and expand body. */
 
	params = maclam->abMLambda.param;
	args   = abNewApplyArg(ab);
	n      = abArgc(args);
 
	if (n != abArgc(params))
		comsgError(ab, ALDOR_E_MacBadArgc);
	else {
		MacDefScope mds;
		n = abArgc(params);
 
		for (i = 0; i < n; i++) {
			AbSyn	param = abArgv(params)[i];
			AbSyn	arg   = abArgv(args)  [i];
#ifdef MacDeclArgs
			if (abHasTag(param, AB_Id))
				pushMacDef(param->abId.sym, MAC_Param, arg);
			else if (abHasTag(arg, AB_Declare)) {
				Symbol	id1  = param->abDeclare.id->abId.sym;
				Symbol	id2  = param->abDeclare.type->abId.sym;
				AbSyn	arg1 = arg->abDeclare.id;
				AbSyn	arg2 = arg->abDeclare.type;
 
				pushMacDef(id1, MAC_Param, arg1);
				pushMacDef(id2, MAC_Param, arg2);
			}
			else
				comsgError(arg, ALDOR_E_MacBadArg);
#else
			assert(abHasTag(param, AB_Id));
			pushMacDef(param->abId.sym, MAC_Param, arg);
#endif
		}
 
		mds = pushMacScope();
		ab  = macEx(maclam->abMLambda.body);
		ab  = abNormal(ab, true);
		popMacScope(mds);
 
		for (i = 0; i < n; i++) popMacDef();
 
		abFreeNode(ab0);
		abFreeNode(maclam);
		abFree	  (params);
	}
 
	abFreeNode(args);
	return ab;
}
 
local AbSyn
macDefine(AbSyn ab)
{
	/* !! This is only approximate */
	MacDefScope mds = pushMacScope();
 
	ab = macParts(ab);
	popMacScope(mds);
	return ab;
}
 
local AbSyn
macLambda(AbSyn ab)
{
	/* !! This is only approximate - but probably sufficient! */
	MacDefScope mds = pushMacScope();
 
	ab = macParts(ab);
	popMacScope(mds);
	return ab;
}
 
local AbSyn
macMacro(AbSyn ab)
{
	SrcPos	pos = abPos(ab);
	abFree(ab);
	return abNewNothing(pos);
}
 
local AbSyn
macMDefine(AbSyn ab)
{
	AbSyn	definee	= ab->abMDefine.lhs;
	AbSyn	body	= ab->abMDefine.rhs;
	SrcPos	pos	= abPos(ab);

	if (!definee || abTag(definee) != AB_Id)
		comsgError(ab, ALDOR_E_MacBadDefn);
	else {
		int n0 = comsgErrorCount();
		body   = macEx(abCopy(body));

		/* Don't add bad macros to the list. */
		if (comsgErrorCount() == n0)
			pushMacDef(definee->abId.sym, MAC_Body, body);
		else
			abFree(body);
	}
 
	abFree(ab);
	return abNewNothing(pos);
}
 
local AbSyn
macMLambda(AbSyn ab)
{
	int	i, n;
	AbSyn	params, body;
	Bool	okparams;
 
	params	 = ab->abMLambda.param;
	n	 = abArgc(params);
	body	 = ab->abMLambda.body;
 
	okparams = params && (abTag(params) == AB_Comma);
	assert(okparams);
 
	/* Alpha convert bound variables and capture environment. */
 
	for (i = 0; i < n; i++) {
		AbSyn  par = abArgv(params)[i];
 
		/* Handle a parameter that is an id */
		if (abHasTag(par, AB_Id)) {
			Symbol sym = symGen();
			abSubSymbol(body, par->abId.sym, sym);
			par->abId.sym = sym;
		}
#ifdef MacDeclArgs
		/* Handle a parameter that is of the form id1 : id2 */
		else if (abHasTag(par, AB_Declare) &&
			 abHasTag(par->abDeclare.id, AB_Id) &&
			 abHasTag(par->abDeclare.type, AB_Id))
		{
			Symbol sym = symGen();
			AbSyn  x   = par->abDeclare.id;
			abSubSymbol(body, x->abId.sym, sym);
			x->abId.sym = sym;
 
			sym = symGen();
			x   = par->abDeclare.type;
			abSubSymbol(body, x->abId.sym, sym);
			x->abId.sym = sym;
		}
#endif
		/* Otherwise: ERROR */
		else {
			okparams = false;

			if (abHasTag(par, AB_Declare))
				comsgError(par, ALDOR_E_MacBadParamDecl);
			else
				comsgError(par, ALDOR_E_MacBadParam);
		}
	}
 
	if (okparams) {
		MacDefScope mds;
		mds = pushMacScope();
		ab->abMLambda.body = macEx(body);
		popMacScope(mds);
	}

	return ab;
}
 
local AbSyn
macWhere(AbSyn ab)
{
	MacDefScope	mds0, mdsW;
	MacDefList	mdsE;
 
	/* Save original scope and add items from where clause. */
	mds0 = pushMacScope();
	ab->abWhere.context = macEx(ab->abWhere.context);
 
	/* Expand expr in an inner scope. */
	mdsW = pushMacScope();
	ab->abWhere.expr = macEx(ab->abWhere.expr);
 
	/* Pop inner scope and keep any macros it defines. */
	mdsE = popMacScopeDefList(mdsW);
 
	/* Pop scope from where clause. */
	popMacScope(mds0);
 
	/* Add new macros from expr to current environment. */
	pushMacDefList(mdsE);
	listFree(MacDef)(mdsE);
 
	return ab;
}
 
local AbSyn
macWith(AbSyn ab)
{
	MacDefScope mds = pushMacScope();
 
	ab->abWith.base = macEx(ab->abWith.base);
	ab->abWith.within = macEx(ab->abWith.within);
 
	popMacScope(mds);
	return ab;
}
 
/*
 * Given the list of active macro names [an, ..., a2, a1], allocate the
 * string "a1->a1->...->an", where "->" is given by the "punct" parameter.
 */
local	String
formatActiveMacroChain(AbSynList abIdList, String punct)
{
	AbSyn	ab;
	AbSynList abl0, abl;
	Buffer	buf;
	String	str;
	int	i;
 
	buf = bufNew();
	abl0 = listReverse(AbSyn) (abIdList);
 
	for (abl = abl0, i = 0; abl; abl = cdr(abl), i++) {
		if (i > 0)
			bufPuts(buf, punct);
		ab = car(abl);
		assert(ab && abHasTag(ab, AB_Id));
		bufPuts(buf, symString(ab->abId.sym));
	}
	str = bufLiberate(buf);
	listFree(AbSyn) (abl0);
 
	return str;
}
