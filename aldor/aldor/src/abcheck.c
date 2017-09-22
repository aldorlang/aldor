/*****************************************************************************
 *
 * abcheck.c: Verify tree format after macro expansion
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "format.h"
#include "phase.h"
#include "spesym.h"
#include "util.h"
#include "comsg.h"
#include "comsgdb.h"

/*****************************************************************************
 *
 * Local function declarations
 *
 ****************************************************************************/

local void	abCheckAdd		(AbSyn);
local void	abCheckApply		(AbSyn);
local void	abCheckAssign		(AbSyn);
local void	abCheckBuiltin		(AbSyn);
local void	abCheckDeclare		(AbSyn);
local void	abCheckDefine		(AbSyn);
local void	abCheckDDefine		(AbSyn);
local void	abCheckExport		(AbSyn);
local void	abCheckExtend		(AbSyn);
local void	abCheckFluid		(AbSyn);
local void	abCheckFor		(AbSyn);
local void	abCheckForeignImport	(AbSyn);
local void	abCheckFree		(AbSyn);
local void	abCheckImport		(AbSyn);
local void	abCheckLambda		(AbSyn);
local void	abCheckLocal		(AbSyn);
local void	abCheckReference	(AbSyn);
local void	abCheckParam		(AbSyn);
local void	abCheckSelect		(AbSyn);
local void	abCheckWhere		(AbSyn);

local Bool	abCheckExportCategory	(AbSyn);
local Bool	abCheckExportSource	(AbSyn);
local Bool	abCheckExportTarget	(AbSyn);
local void	abCheckExtendDeclare	(AbSyn, AbSyn);
local void	abCheckFor0		(AbSyn);
local void	abCheckFluidComma	(AbSyn);
local void	abCheckFluidDeclare	(AbSyn, Bool);
local void	abCheckParamDefine	(AbSyn);
local void	abCheckLOF		(AbSyn, String);
local void	abCheckLOFComma		(AbSyn, String);
local void	abCheckLOFDeclare	(AbSyn, Bool, String);

typedef void	(*AbCheckFn)		(AbSyn, String);

local void	abCheckOneOrMoreForms	(AbSyn, String, AbCheckFn);
local void	abCheckOneDefine	(AbSyn, String);
local void	abCheckWithin		(AbSyn, String);
local void      abCheckWithinDeclare(AbSyn ab, String str);

/*****************************************************************************
 *
 * Top-level entry point.
 *
 ****************************************************************************/

void
abCheck(AbSyn absyn)
{
	int	i, argc;
	AbSyn  *argv;

	if (!absyn)
		bug("Encountered zero abstract syntax tree in abCheck.");

	if (DEBUG(phase)) {
		fnewline(dbOut);
		fprintf(dbOut, "abCheck for ");
		abPrint(dbOut, absyn);
		findent += 2;
	}

	if (!abIsLeaf(absyn)) {
		argc = abArgc(absyn);
		argv = abArgv(absyn);

		for (i = 0; i < argc; i++)
			abCheck(argv[i]);
	}

	if (DEBUG(phase)) {
		findent -= 2;
	}

	switch (abTag(absyn)) {
	case AB_Nothing:
		break;

	case AB_Add:
		abCheckAdd(absyn);
		break;

	case AB_Apply:
		abCheckApply(absyn);
		break;

	case AB_Assign:
		abCheckAssign(absyn);
		break;

	case AB_Builtin:
		abCheckBuiltin(absyn);
		break;

	case AB_Declare:
		abCheckDeclare(absyn);
		break;

	case AB_Define:
		abCheckDefine(absyn);
		break;

	case AB_DDefine:
		abCheckDDefine(absyn);
		break;

	case AB_Export:
		abCheckExport(absyn);
		break;

	case AB_Extend:
		abCheckExtend(absyn);
		break;

	case AB_Fluid:
		abCheckFluid(absyn);
		break;

	case AB_For:
		abCheckFor(absyn);
		break;

	case AB_ForeignImport:
		abCheckForeignImport(absyn);
		break;

	case AB_Free:
		abCheckFree(absyn);
		break;

	case AB_Import:
		abCheckImport(absyn);
		break;

	case AB_Label:
		if (!abHasTag(absyn->abLabel.label, AB_Id))
			comsgError(absyn, ALDOR_E_ChkBadLabel);
		break;

	case AB_Goto:
		if (!abHasTag(absyn->abGoto.label, AB_Id))
			comsgError(absyn, ALDOR_E_ChkBadGoto);
		break;

	case AB_Lambda:
	case AB_PLambda:
		abCheckLambda(absyn);
		break;

	case AB_Local:
		abCheckLocal(absyn);
		break;

	case AB_Macro:
		comsgError(absyn, ALDOR_E_ChkBadMacro);
		break;

	case AB_MLambda:
		comsgError(absyn, ALDOR_E_ChkBadMLambda);
		break;

	case AB_With:
		abCheckOneOrMoreForms(absyn->abWith.within, "with",
				      abCheckWithin);
		break;

	case AB_Qualify:
		if (!abHasTag(absyn->abQualify.what, AB_Id))
			comsgError(absyn, ALDOR_E_ChkBadQualification);
		break;

	case AB_Where:
		abCheckWhere(absyn);
		break;
		
	case AB_Select:
		abCheckSelect(absyn);
		break;

	case AB_Reference:
		abCheckReference(absyn);
		break;

	default:
		break;
	}
}

/*****************************************************************************
 *
 * :: abCheckAdd
 *
 ****************************************************************************/

/*
 * Look for improper exits from the top-level of an add body.
 */
local void
abCheckAdd(AbSyn absyn)
{
	AbSyn	body = absyn->abAdd.capsule;
	Length	i, argc = abArgcAs(AB_Sequence, body);
	AbSyn	*argv = abArgvAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];
		if (abHasTag(arg, AB_Exit)) {
			comsgError(arg, ALDOR_E_ChkBadForm, "add");
			break;
		}
	}
}

/*****************************************************************************
 *
 * :: abCheckApply
 *
 ****************************************************************************/

/*
 * Look for improper forms in Unions and repeated
 * selector/type pairs in Records, RawRecords and Unions.
 */
local void
abCheckApply(AbSyn absyn)
{
	AbSyn	op = absyn->abApply.op;
	Length	i;

	if (abIsRecordOrUnion(op)) {
		AbSynList	al = listNil(AbSyn);

		for (i = 0; i < abApplyArgc(absyn); i += 1) {
			AbSyn	argi = abApplyArg(absyn, i);
			if (listMember(AbSyn)(al, argi, abEqual))
				comsgError(argi, ALDOR_E_ChkBadRecordOrUnion);
			else
				al = listCons(AbSyn)(argi, al);
		}
		listFree(AbSyn)(al);
	}
}

/*****************************************************************************
 *
 * :: abCheckAssign
 *
 ****************************************************************************/

/*
 * Check for improper left-hand sides of assignments.
 */
local void
abCheckAssign(AbSyn absyn)
{
	AbSyn	lhs	= absyn->abAssign.lhs;
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc	= abArgcAs(AB_Comma, lhs);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
		case AB_Apply:
			break;

		case AB_Declare:
			if (abHasTag(lhs, AB_Comma) &&
			    abHasTag(arg->abDeclare.id, AB_Comma))
				comsgError(arg, ALDOR_E_ChkBadAssign);
			break;

		default:
			comsgError(arg, ALDOR_E_ChkBadAssign);
			break;
		}
	}
}

/*****************************************************************************
 *
 * :: abCheckBuiltin
 *
 ****************************************************************************/

/*
 * Check that the operations imported from Builtin are declarations.
 */
local void
abCheckBuiltin(AbSyn absyn)
{
	AbSyn	what	= absyn->abBuiltin.what;
	AbSyn	*argv	= abArgvAs(AB_Sequence, what);
	Length	i, argc = abArgcAs(AB_Sequence, what);

	for (i = 0; i < argc; i += 1)
		if (!abHasTag(argv[i], AB_Declare))
			comsgError(argv[i], ALDOR_E_ChkBadForm, "builtin");
}

/*****************************************************************************
 *
 * :: abCheckDeclare
 *
 ****************************************************************************/

/*
 * Check that the id is an identifier or comma of identifiers.
 */
local void
abCheckDeclare(AbSyn absyn)
{
	AbSyn	id	= absyn->abDeclare.id;
	AbSyn	*argv	= abArgvAs(AB_Comma, id);
	Length	i, argc	= abArgcAs(AB_Comma, id);

	for (i = 0; i < argc; i += 1)
		if (!abHasTag(argv[i], AB_Id))
			comsgError(argv[i], ALDOR_E_ChkBadDeclare);
}

/*****************************************************************************
 *
 * :: abCheckDefine
 *
 ****************************************************************************/

/*
 * Check that the left-hand side is an identifier or
 * a declaration of a single identifier.
 */
local void
abCheckDefine(AbSyn absyn)
{
	AbSyn	lhs	= absyn->abDefine.lhs;
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc	= abArgcAs(AB_Comma, lhs);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			break;

		case AB_Declare:
			if (abHasTag(lhs, AB_Comma) &&
			    abHasTag(arg->abDeclare.id, AB_Comma))
				comsgError(arg, ALDOR_E_ChkBadDefine);
			break;

		default:
			comsgError(arg, ALDOR_E_ChkBadDefine);
			break;
		}
	}
}

/*****************************************************************************
 *
 * :: abCheckDDefine
 *
 ****************************************************************************/

local void
abCheckDDefine(AbSyn absyn)
{
	AbSyn	body	= absyn->abDDefine.body;
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1)
		if (!abHasTag(argv[i], AB_Define))
			comsgError(argv[i], ALDOR_E_ChkBadForm, "define");
}

/*****************************************************************************
 *
 * :: abCheckExport
 *
 ****************************************************************************/

/*
 * Check that what is exported is a valid specification of exports,
 * that there is at most one origin or destination,
 * that the origin is a valid source of exports, and
 * that the destination is a valid destination for exports.
 */
local void
abCheckExport(AbSyn absyn)
{
	AbSyn	what = absyn->abExport.what;
	AbSyn	from = absyn->abExport.origin;
	AbSyn	dest = absyn->abExport.destination;

	if (abIsNotNothing(from) && abIsNotNothing(dest))
		comsgError(absyn, ALDOR_E_ChkBadForm, "export");

	if (abIsNothing(from) && abIsNothing(dest))
		abCheckOneOrMoreForms(what, "export", abCheckOneDefine);
	else {
		if (!abCheckExportCategory(what))
			comsgError(what, ALDOR_E_ChkBadForm, "export");

		if (!abCheckExportSource(from))
			comsgError(from, ALDOR_E_ChkBadForm, "export");

		if (!abCheckExportTarget(dest))
			comsgError(dest, ALDOR_E_ChkBadForm, "export");
	}
}

local Bool
abCheckExportCategory(AbSyn ab)
{
	switch (abTag(ab)) {
	case AB_Nothing:
	case AB_Id:
	case AB_Apply:
	case AB_Qualify:
	case AB_Declare:
	case AB_Sequence:
	case AB_With:
		return true;
	default:
		return false;
	}
}

local Bool
abCheckExportSource(AbSyn ab)
{
	switch (abTag(ab)) {
	case AB_Nothing:
	case AB_Id:
	case AB_Apply:
	case AB_Qualify:
	case AB_Comma:
		return true;
	case AB_PretendTo:
		return abCheckExportSource(ab->abPretendTo.type);
	default:
		return false;
	}
}

local Bool
abCheckExportTarget(AbSyn ab)
{
	return	abIsNothing(ab) ||
		abIsTheId(ab, ssymBuiltin) ||
		abIsApplyOf(ab, ssymForeign);
}

/*****************************************************************************
 *
 * :: abCheckExtend
 *
 ****************************************************************************/

local void
abCheckExtend(AbSyn absyn)
{
	AbSyn	body	= absyn->abExtend.body;
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		if (abHasTag(argv[i], AB_Define)) {
			AbSyn	lhs = argv[i]->abDefine.lhs;
			AbSyn	rhs = argv[i]->abDefine.rhs;

			abCheckExtendDeclare(lhs, rhs);
		}
		else
			abCheckExtendDeclare(argv[i], NULL);
	}
}

local void
abCheckExtendDeclare(AbSyn lhs, AbSyn rhs)
{
	switch (abTag(lhs)) {
	case AB_Id:
		if (!rhs) comsgError(lhs, ALDOR_E_ChkBadForm, "extend");
		break;

	case AB_Declare:
		break;

	default:
		comsgError(lhs, ALDOR_E_ChkBadForm, "extend");
		break;
	}
}

/*****************************************************************************
 *
 * :: abCheckFluid
 *
 ****************************************************************************/

local void
abCheckFluid(AbSyn absyn)
{
	AbSyn	body	= absyn->abFluid.argv[0];
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		if (abHasTag(argv[i], AB_Assign))
			abCheckFluidComma(argv[i]->abAssign.lhs);
		else
			abCheckFluidComma(argv[i]);
	}
}

local void
abCheckFluidComma(AbSyn lhs)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc = abArgcAs(AB_Comma, lhs);
	Bool	isComma	= abHasTag(lhs, AB_Comma);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			break;

		case AB_Declare:
			abCheckFluidDeclare(arg, isComma);
			break;

		default:
			comsgError(arg, ALDOR_E_ChkBadForm, "fluid");
			break;
		}
	}
}

local void
abCheckFluidDeclare(AbSyn decl, Bool isComma)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	if (isComma && abHasTag(name, AB_Comma))
		comsgError(name, ALDOR_E_ChkBadForm, "fluid");

	for (i = 0; i < argc; i += 1)
		if (!abHasTag(argv[i], AB_Id))
			comsgError(argv[i], ALDOR_E_ChkBadForm, "fluid");
}

/*****************************************************************************
 *
 * :: abCheckFor
 *
 ****************************************************************************/

/*
 * Check that the expression following 'for' is an identifier or
 * a declaration of a single identifier, optionally preceeded
 * by the keyword 'free'.
 */
local void
abCheckFor(AbSyn absyn)
{
	AbSyn	lhs = absyn->abFor.lhs;

	if (abHasTag(lhs, AB_Free) || abHasTag(lhs, AB_Local))
		lhs = abArgv(lhs)[0];

	abCheckFor0(lhs);
}

local void
abCheckFor0(AbSyn lhs)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc	= abArgcAs(AB_Comma, lhs);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			break;

		case AB_Apply:
                  comsgError(arg, ALDOR_E_ChkBadFor);
                  break;
                  
		case AB_Declare:
			if (abHasTag(lhs, AB_Comma) &&
			    abHasTag(arg->abDeclare.id, AB_Comma))
				comsgError(arg, ALDOR_E_ChkBadFor);
			break;

		default:
			comsgError(arg, ALDOR_E_ChkBadFor);
			break;
		}
	}
}

local void
abCheckFor0_old(AbSyn var)
{
	switch (abTag(var)) {
	case AB_Id:
		break;

	case AB_Declare:
		if (!abHasTag(var->abDeclare.id, AB_Id))
			comsgError(var, ALDOR_E_ChkBadFor);
		break;

	default:
		comsgError(var, ALDOR_E_ChkBadFor);
		break;
	}
}

/*****************************************************************************
 *
 * :: abCheckForeignImport
 *
 ****************************************************************************/

local void
abCheckForeignImport(AbSyn absyn)
{
	AbSyn	what	= absyn->abForeignImport.what;
	AbSyn	*argv	= abArgvAs(AB_Sequence, what);
	Length	i, argc = abArgcAs(AB_Sequence, what);


	/* Are we checking `import Cat from Foreign X' ? */
	if (abTag(what) == AB_With) return;


	/* Standard check of foreign import declarations */
	for (i = 0; i < argc; i += 1) {
		switch (abTag(argv[i])) {
		case AB_Nothing:
		case AB_Declare:
		case AB_Define:
		case AB_DDefine:
			break;

		default:
			comsgError(argv[i], ALDOR_E_ChkBadForm, "import");
		}
	}
}

/*****************************************************************************
 *
 * :: abCheckImport
 *
 ****************************************************************************/

/*
 * Check that what is imported is a valid specification of exports,
 * and that the origin of the import is a valid non-empty source of exports.
 */
local void
abCheckImport(AbSyn absyn)
{
	AbSyn	what = absyn->abImport.what;
	AbSyn	from = absyn->abImport.origin;
	AbSyn 	where;

	if (!abCheckExportCategory(what))
		comsgError(what, ALDOR_E_ChkBadForm, "import");

	if (abIsNothing(from) || !abCheckExportSource(from)) {
		Buffer	obuf = bufNew();

		bufPrintf(obuf, comsgString(ALDOR_E_ChkBadForm), "import");
	
		if (comsgOkDetails()) {
			bufPutc(obuf, '\n');
			bufPrintf(obuf, comsgString(ALDOR_D_ChkUseFromHint));
		}
		where = abIsNothing(what) ? from : what;
		comsgError(where, ALDOR_E_ExplicitMsg, bufChars(obuf));
		bufFree(obuf);
	}
}

/*****************************************************************************
 *
 * :: abCheckLambda
 *
 ****************************************************************************/

/*
 * Check that the lambda expression has parameter types and return types,
 * and ensure that the parameter names are unique.
 */
local void
abCheckLambda(AbSyn absyn)
{
	AbSyn		param = absyn->abLambda.param;
	AbSyn		rtype = absyn->abLambda.rtype;
	SymbolList	parameters = listNil(Symbol);
	Length		i;

	if (!rtype)
		comsgError(absyn, ALDOR_E_ChkMissingRetType);

	abCheckParam(param);

	for (i = 0; i < abArgc(param); i += 1) {
		AbSyn	pari = abDefineeIdOrElse(abArgv(param)[i], NULL);
		Symbol	sym;

		if (!pari) continue;
		sym = pari->abId.sym;

		if (listMemq(Symbol)(parameters, sym))
			comsgError(abArgv(param)[i], ALDOR_E_ChkBadParamsDups);
		else
			parameters = listCons(Symbol)(sym, parameters);
	}
	listFree(Symbol)(parameters);
}

/*****************************************************************************
 *
 * :: abCheckReference
 *
 ****************************************************************************/

local void
abCheckReference(AbSyn absyn)
{
	AbSyn body	= absyn -> abReference.body;

	if (abHasTag(body, AB_Id))
		return;
	else if (abHasTag(body, AB_Apply))
	{
		/* Can applications have non-ID operators? */
		body = body -> abApply.op;
		if (abHasTag(body, AB_Id))
        	        return;
	}


	/* We don't like this form */
	comsgError(absyn, ALDOR_E_ChkBadForm, "ref");
}

/*****************************************************************************
 *
 * :: abCheckParam
 *
 ****************************************************************************/

local void
abCheckParam(AbSyn absyn)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, absyn);
	Length	i, argc	= abArgcAs(AB_Comma, absyn);

	if (!abHasTag(absyn, AB_Comma))
		comsgError(absyn, ALDOR_E_ChkBadParams);

	for (i = 0; i < argc; i += 1) {
		if (abHasTag(argv[i], AB_Define))
			abCheckParamDefine(argv[i]->abDefine.lhs);
		else
			abCheckParamDefine(argv[i]);
	}
}

local void
abCheckParamDefine(AbSyn lhs)
{
	if (!abHasTag(lhs, AB_Declare))
		comsgError(lhs, ALDOR_E_ChkBadParams);
}

/*****************************************************************************
 *
 * :: abCheckSelect
 *
 ****************************************************************************/

local void
abCheckSelect(AbSyn absyn)
{
	AbSyn seq;
	int i;

	seq = absyn->abSelect.alternatives;
	if (!abHasTag(seq, AB_Sequence))
		comsgError(absyn, ALDOR_E_ChkSelectSeq);

	for (i = 0; i<abArgc(seq); i++) {
		if (!abHasTag(seq->abSequence.argv[i], AB_Exit))
			break;
	}
	for ( ; i<abArgc(seq); i++) {
		if (abHasTag(seq->abSequence.argv[i], AB_Exit))
			comsgError(absyn, ALDOR_E_ChkSelectExits);
	}
}

/*****************************************************************************
 *
 * :: abCheckWhere
 *
 ****************************************************************************/

/*
 * Look for improper exits from the context of a where clause.
 */
local void
abCheckWhere(AbSyn absyn)
{
	AbSyn	ctxt = absyn->abWhere.context;
	Length	i, argc = abArgcAs(AB_Sequence, ctxt);
	AbSyn	*argv = abArgvAs(AB_Sequence, ctxt);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];
		if (abHasTag(arg, AB_Exit)) {
			comsgError(arg, ALDOR_E_ChkBadForm, "where");
			break;
		}
	}
}

/******************************************************************************
 *
 * :: abCheckFree
 * :: abCheckLocal
 *
 *****************************************************************************/

local void
abCheckFree(AbSyn absyn)
{
	abCheckLOF(absyn->abFree.argv[0], "free");
}

local void
abCheckLocal(AbSyn absyn)
{
	abCheckLOF(absyn->abLocal.argv[0], "local");
}

local void
abCheckLOF(AbSyn body, String context)
{
	AbSyn	*argv	= abArgvAs(AB_Sequence, body);
	Length	i, argc = abArgcAs(AB_Sequence, body);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
		case AB_Declare:
		case AB_Comma:
			abCheckLOFComma(arg, context);
			break;

		case AB_Assign:
			abCheckLOFComma(arg->abAssign.lhs, context);
			abCheckAssign(arg);
			break;

		case AB_Define:
			abCheckLOFComma(arg->abDefine.lhs, context);
			abCheckDefine(arg);
			break;

		default:
			comsgError(arg, ALDOR_E_ChkBadForm, context);
			break;
		}
	}
}

local void
abCheckLOFComma(AbSyn lhs, String context)
{
	AbSyn	*argv	= abArgvAs(AB_Comma, lhs);
	Length	i, argc = abArgcAs(AB_Comma, lhs);
	Bool	isComma	= abHasTag(lhs, AB_Comma);

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = argv[i];

		switch (abTag(arg)) {
		case AB_Id:
			break;

		case AB_Declare:
			abCheckLOFDeclare(arg, isComma, context);
			break;

		default:
			comsgError(arg, ALDOR_E_ChkBadForm, context);
			break;
		}
	}
}

local void
abCheckLOFDeclare(AbSyn decl, Bool isComma, String context)
{
	AbSyn	name	= decl->abDeclare.id;
	AbSyn	*argv	= abArgvAs(AB_Comma, name);
	Length	i, argc = abArgcAs(AB_Comma, name);

	if (isComma && abHasTag(name, AB_Comma))
		comsgError(name, ALDOR_E_ChkBadForm, context);

	for (i = 0; i < argc; i += 1)
		if (!abHasTag(argv[i], AB_Id))
			comsgError(argv[i], ALDOR_E_ChkBadForm, context);
}

/*****************************************************************************
 *
 * abCheck helper functions.
 *
 ****************************************************************************/

local void
abCheckOneOrMoreForms(AbSyn ab, String str, AbCheckFn check)
{
	int	i;

	switch (abTag(ab)) {
	case AB_Nothing:
		break;

	case AB_Comma:
	case AB_Sequence:
		for (i = 0; i < abArgc(ab); i += 1)
			abCheckOneOrMoreForms(abArgv(ab)[i], str, check);
		break;

	default:
		check(ab, str);
	}
}

local void
abCheckOneDefine(AbSyn ab, String str)
{
	if (abHasTag(ab, AB_Define))
		ab = ab->abDefine.lhs;

	if (!abHasTag(ab, AB_Declare))
		comsgError(ab, ALDOR_E_ChkBadForm, str);
}

local void
abCheckWithin(AbSyn ab, String str)
{
	int i;

	switch (abTag(ab)) {
	case AB_Id:
	case AB_Declare:
		abCheckWithinDeclare(ab, str);
	case AB_Default:
	case AB_Import:
	case AB_Export:
	case AB_Apply:
	case AB_RestrictTo:
		break;

	case AB_If:
		abCheckOneOrMoreForms(ab->abIf.thenAlt, str, abCheckWithin);
		abCheckOneOrMoreForms(ab->abIf.elseAlt, str, abCheckWithin);
		break;

	case AB_Sequence:
		for (i=0; i<abArgc(ab); i++)
			abCheckOneOrMoreForms(ab->abSequence.argv[i], str, abCheckWithin);
		break;

	default:
		comsgError(ab, ALDOR_E_ChkBadForm, str); 
		break;
	}
}

local void
abCheckWithinDeclare(AbSyn ab, String str)
{
	AbSyn id = ab->abDeclare.id;

	if (abTag(id) != AB_Id) {
		comsgError(ab, ALDOR_E_ChkBadForm, str);
	}
}
