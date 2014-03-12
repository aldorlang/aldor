/*****************************************************************************
 *
 * abnorm.c: Produce standard form of abstract syntax tree.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * 1a. Defines f(a:A):B == b  are converted to f: A->B == a +-> label f b.
 *     Curried definitions f(a1)...(an) == c create n lambdas.
 *
 * 1b. Macro defines  f(a) ==> b  are converted to  f ==> [MLambda a b].
 *     Curried definitions f(a1)...(an) ==> c create n macro lambdas.
 *
 * 2a. Lambdas (a)(b) +-> c are converted to a +-> b +-> c.
 *
 * 2b. Likewise for macro lambdas.
 *
 * 3.  Imports are converted to Foreign or Builtin if appropriate.
 *
 * 4.  Change (a(b))$T to (a$T)(b)
 *
 * 5.  Flatten the sequence after a 'with'.
 *     Merge nested 'with's.
 *
 * 6.  Change Enumeration(n1, ..., nk) to Enumeration(n1: (), ..., nk: ())
 *
 * 7.  Change '=>' not in a sequence to be in a singleton sequence.
 *
 * 8.  Remove "abDocumented" nodes and place text into comment fields of abIds.
 *
 * 9.  Change	--!! This should be handled by scobind
 *        [Import w [Sequence o1 ...]] to [Sequence [Import w o1] ...] 
 *        [Import w [Comma o1 ...]]    to [Sequence [Import w o1] ...] 
 *        [Inline w [Sequence o1 ...]] to [Sequence [Inline w o1] ...]
 *        [Inline w [Comma o1 ...]]    to [Sequence [Inline w o1] ...].
 *
 *        [Export x y] to [Export [With 0 x] y] -- if x isn't [With ...]
 *        [Import x y] to [Import [With 0 x] y] -- if x isn't [With ...]
 *        [Inline x y] to [Inline [With 0 x] y] -- if x isn't [With ...]
 *
 * 10. Replace abParen nodes by their contents: (E) becomes E.
 *
 * 11. Change  [Macro [Lambda ...]]   to [MLambda ...]
 *     Change  [Macro [Define ...]]   to [MDefine ...]
 *     Change  [Macro [Sequence ...]] to [Sequence [Macro ..] [Macro ..] ..]
 *     Change  [Macro [Where ...]]    to [Where .. [Macro ..]]
 *
 * 12. Change [Apply f [Comma a1 .. an]] to [Apply f a1 .. an].
 *
 * 13. (Bootstrap!!) Change
 *     [DDefine [Define ...]] to [Define ...]
 *
 * 14. Change `Dom:Cat == X add Y' into `Dom: Cat with == X add Y' except
 *     when Cat is Type (because Type is a domain not a category!). If
 *     the RHS is an add-valued sequence then we also do this.
 *
 * 15. Merge defaults in a with body into a single default: change
 *
 *        [With ... [Default a] [Default b] [Default [Seq c d e ... ]] ...]
 *     to
 *        [With ... [Default [Seq [Default a] [Default b] c d e ...]] ...]
 *
 *     This is so that we can compile older code when defaults seem to
 *     have been handled in a much more relaxed manner.
 *
 * 16. Hack to resolve problems with the analysis of where statements
 *     identified by bug 1206 etc. The real problem might be considered
 *     to be the scope of the LHS of a "where" as defined in axl.y.
 *
 *      (i) remove where statements with an empty context
 *     (ii) after the macex phase, change
 *             [Wher: X [Defi: D [Lamb: T R B]]]
 *          to
 *             [Defi: D [Lamb: T R [Wher: X B]]]
 *
 *     The reason for (i) is that AXIOM users occasionally use where
 *     statements to hold macro definitions. After macex the where is
 *     redundant and needs to be discard to avoid confusing (ii).
 *
 *     If the code generator could handle top-level where statements
 *     then (ii) would not be needed. At present it assumes there are
 *     only definitions at the top-level hence the reversal.
 *
 *     Also note that using the functional-programming "let" statement
 *     as syntactic sugar for "where" might make the scoping obvious
 *     to users.
 *
 * The original tree is modified.
 * Nodes which are no longer pointed to in the resulting tree are freed.
 */

#define UseTypeVariables
#undef	UseTypeVariables

#include "debug.h"
#include "format.h"
#include "macex.h"
#include "spesym.h"
#include "stab.h"
#include "archive.h"
#include "lib.h"
#include "abpretty.h"
#include "doc.h"
#include "comsg.h"
#include "strops.h"

Bool abnormDebug	= false;
Bool abnDefineDebug	= false;
Bool abnWhereDebug	= false;
Bool abnWithMergeDebug	= false;

#define abnormDEBUG		DEBUG_IF(abnorm)	afprintf
#define abnDefineDEBUG		DEBUG_IF(abnDefine)	afprintf
#define abnWhereDEBUG		DEBUG_IF(abnWhere)	afprintf
#define abnWithMergeDEBUG	DEBUG_IF(abnWithMerge)	afprintf

/*
 * Sometimes we really care if we have finished the macro expansion
 * phase. At the moment we only care when processing `where's.
 */
static Bool abnAfterMacex = false;

/* Enabled with -Wno-where (see abnWhere) */
Bool NoWhereHack = false;

/*****************************************************************************
 *
 * :: Forward declarations
 *
 ****************************************************************************/

local AbSyn	abnorm		(AbSyn);

local AbSyn	abnApply	(AbSyn);
local AbSyn	abnBlank	(AbSyn);
local AbSyn	abnDefine	(AbSyn);
local AbSyn	abnDDefine	(AbSyn);
local AbSyn	abnDocumented	(AbSyn);
local AbSyn	abnImport	(AbSyn);
local AbSyn	abnInline	(AbSyn);
local AbSyn	abnExport	(AbSyn);
local AbSyn	abnLambda	(AbSyn);
local AbSyn	abnMacro	(AbSyn);
local AbSyn	abnMDefine	(AbSyn);
local AbSyn	abnMLambda	(AbSyn);
local AbSyn	abnParen	(AbSyn);
local AbSyn	abnQualify	(AbSyn);
local AbSyn	abnWhere	(AbSyn);
local AbSyn	abnWith		(AbSyn);

local AbSyn	abn0Parts	     (AbSyn);
local Bool	abnIsDomValued		(AbSyn);
local AbSyn	abn0DefineTarget     (AbSyn);
local AbSyn	abn0LambdaType	     (AbSyn, AbSyn, Bool);
local void	abn0LambdaBehead     (AbSyn *, AbSyn *, AbSyn *, Bool);
local void	abn0MLambdaBehead    (AbSyn *, AbSyn *);
local AbSyn	abn0Import    (AbSyn, AbSyn, AbSyn);
local AbSyn	abn0BuiltinImport    (AbSyn, AbSyn);
local AbSyn	abn0ImportSeq	     (AbSyn);
local AbSyn	abn0WithFlatten	     (AbSyn);
local AbSyn	abn0JoinFlatten      (AbSyn);
local Bool	ab0IfIsCat	     (AbSyn);
local Bool	ab0SeqIsCat	     (AbSyn);
local AbSyn	abn0SeqConcat	     (AbSyn, AbSyn);
local void	abn0WithRemoveDups   (AbSyn);
local AbSyn	abn0JoinRemoveDups   (AbSyn);
local AbSyn	abn0WithMergeDefaults(AbSyn);


/*****************************************************************************
 *
 * :: Main entry point
 *
 ****************************************************************************/

AbSyn
abNormal(AbSyn ab, Bool afterMacex)
{
	AbSyn	result;
	Bool	oldAfterMacex = abnAfterMacex;

	abnAfterMacex = afterMacex;
	result = abnorm(ab);
	abnAfterMacex = oldAfterMacex;
	return result;
}

static int	level = 0;

local AbSyn
abnorm(AbSyn ab)
{
	assert(ab != 0);

	if (abIsNothing(ab))
		return ab;

	if (DEBUG(abnorm)) {
		fprintf(dbOut, "%*s -> Enter abnorm with ", level++, "");
		abPrint(dbOut, ab);
		fnewline(dbOut);
		fflush(dbOut);
	}


	/*
	 * Recurse into the node first: abnMacro takes
	 * care of its own sub-trees so we skip it here.
	 */
	if (abTag(ab) != AB_Macro)
		ab = abn0Parts(ab);


	/* Now normalise the node itself */
	switch (abTag(ab)) {
	case AB_Apply:
		ab = abnApply(ab);
		break;
	case AB_Blank:
		ab = abnBlank(ab);
		break;
	case AB_Define:
		ab = abnDefine(ab);
		break;
	case AB_DDefine:
		ab = abnDDefine(ab);
		break;
	case AB_Documented:
		ab = abnDocumented(ab);
		break;
	case AB_Import:
		ab = abnImport(ab);
		break;
	case AB_Export:
		ab = abnExport(ab);
		break;
	case AB_Inline:
		ab = abnInline(ab);
		break;
	case AB_Lambda:
	case AB_PLambda:
		ab = abnLambda(ab);
		break;
	case AB_Macro:
		ab = abnMacro(ab);
		break;
	case AB_MDefine:
		ab = abnMDefine(ab);
		break;
	case AB_MLambda:
		ab = abnMLambda(ab);
		break;
	case AB_Paren:
		ab = abnParen(ab);
		break;
	case AB_Qualify:
		ab = abnQualify(ab);
		break;
	case AB_Where:
		ab = abnWhere(ab);
		break;
	case AB_With:
		ab = abnWith(ab);
		break;
	default:
		break;
	}

	if (DEBUG(abnorm)) {
		fprintf(dbOut, "%*s <- Exit  abnorm with ", --level, "");
		abPrint(dbOut, ab);
		fnewline(dbOut);
		fflush(dbOut);
	}

	return ab;
}


/*
 * Normalize all sub-parts of the expression.
 */

local AbSyn
abn0Parts(AbSyn ab)
{
	int	i;

	if (!abIsLeaf(ab))
		for (i = 0; i < abArgc(ab); i++)
			abArgv(ab)[i] = abnorm(abArgv(ab)[i]);

	if (!abIsLeaf(ab) && abTag(ab) != AB_Sequence)
		for (i = 0; i < abArgc(ab); i++) {
			AbSyn abi = abArgv(ab)[i];
			if (abTag(abi) == AB_Exit)
				abArgv(ab)[i] = abNewSequence1(abPos(abi),abi);
		}

	return ab;
}


/*****************************************************************************
 *
 * :: Apply
 *
 ****************************************************************************/

/*
 * abnApply
 * 1. takes
 *	[Apply Enumeration n1 ... nk]
 *    to
 *	[Apply Enumeration n1: () ... nk: ()]
 *
 * 2. takes
 *	[Apply xxx [Comma a1 ... an]]
 *    to
 *	[Apply xxx a1 ... an]
 */

local AbSyn
abnApply(AbSyn ab)
{
	AbSyn	arg, newAb;
	int	i, argc = abApplyArgc(ab);

	if (argc == 1 && abTag(abApplyArg(ab, int0)) == AB_Comma) {
		arg   = abApplyArg(ab, int0);
		argc  = abArgc(arg);
		newAb = abNewEmpty(AB_Apply, 1 + argc);

		abSetPos(newAb, abPos(ab));
		newAb->abApply.op = ab->abApply.op;
		for (i = 0; i < argc; i++)
			abSetApplyArg(newAb, i, abArgv(arg)[i]);

		abFreeNode(arg);
		abFreeNode(ab);
		ab = newAb;
	}

	if (abIsApplyOf(ab, ssymEnum))
		for (i = 0; i < argc; i += 1) {
			AbSyn	abi = abApplyArg(ab, i);

			if (abTag(abi) == AB_Id) {
				newAb = abNewDeclare(abPos(abi), abi,
						     abNewId(abPos(abi),
							     ssymType));
				abApplyArg(ab, i) = newAb;
			}
		}

	return ab;
}


/*****************************************************************************
 *
 * :: Blank
 *
 ****************************************************************************/

/*
 * Experimental Bob stuff (disabled until Bob explains what this is ...).
 * Probably related to the use of "_" blanks in languages such as ML.
 */

local AbSyn
abnBlank(AbSyn ab)
{
#ifdef UseTypeVariables
	if (ab->abBlank.sym == ssymVariable)
		ab->abBlank.sym = symGen();
#endif
	return ab;
}


/*****************************************************************************
 *
 * :: DDefine
 * !! Temporary
 *
 ****************************************************************************/

/*
 * abnDDefine takes
 *   [DDefine A]
 * to
 *   A
 *
 * !!!FIXME!!! This is wrong because [Define lhs rhs] is supposed to hide
 * the value of "rhs" whenever "lhs" is used. [DDefine [Define lhs rhs]]
 * does the opposite: whenever "lhs" is used its value "rhs" is known and
 * can be used in its place (delta-equivalence).
 */

local AbSyn
abnDDefine(AbSyn ab)
{
	AbSyn	body = ab->abDDefine.body;

	abFreeNode(ab);

	return body;
}


/*****************************************************************************
 *
 * :: Define
 *
 ****************************************************************************/

/*
 * abnDefine takes
 *   [Define [Declare [ Apply [ Apply f a ] b ] T ] E]
 * to
 *   [Define [Declare f {A->B->T} ]
 *	[Lambda a {B->T} [Lambda b T [Label f E] ]]]
 *
 * and
 *   [Define [Declare Dom Cat] [Add ...]]
 * to
 *   [Define [Declare Dom [With Cat ()]] [Add ...]]
 *
 * and
 *   [Define [Declare Dom Cat] [Seq ... [Add ...]]]
 * to
 *   [Define [Declare Dom [With Cat ()]] [Seq ... [Add ...]]]
 */

local AbSyn
abnDefine(AbSyn ab)
{
	AbSyn	lhs = ab->abDefine.lhs, rhs = ab->abDefine.rhs;
	AbSyn	targ, type, olhs, otype;
	Bool	packed = false;

	if (abHasTag(lhs, AB_Declare)) {
		olhs = lhs;
		lhs  = olhs->abDeclare.id;
		type = olhs->abDeclare.type;
		abFreeNode(olhs);
	}
	else
		type = 0;	/* Don't abNewNothing yet -- mightn't use it */

/* DON'T ENABLE this edit unless you know what you're doing! */
#if AXL_EDIT_1_1_13_34
	/*
	 * Warn the user if the type of a domain is not an abWith node.
	 * In many cases where the user fails to do this, scobind will not
	 * match the category on the LHS with the domain on the RHS. This
	 * means that imports from the symbol being defined will appear to
	 * come from the category rather than from the RHS. At best this will
	 * prevent inlining of any exports of the RHS; at worst we inline a
	 * default value from the category rather than the replacement value
	 * in the RHS.
	 */
	if (type && abnIsDomValued(rhs)) {
		/* Looking for: [Define [Declare Dom Cat] [Add ...]] */
		if (abHasTag(type, AB_Id)) {
			String	t1	= symString(abIdSym(type));
			String	t2	= symString(ssymType);

			/* Treat Type as being special */
			if (!strEqual(t1, t2))
				comsgWarning(lhs, ALDOR_W_AbnDomainWithout);
		}
		else if (abHasTag(type, AB_Apply))
			comsgWarning(lhs, ALDOR_W_AbnDomainWithout);
	}
#else
	/*
	 * Ensure that the type of a domain is an abWith node. If
	 * we fail then scobind will not match the category on the
	 * LHS with the domain on the RHS. This means that imports
	 * from the symbol being defined will appear to come from
	 * the category rather than from the RHS. At best this will
	 * prevent inlining of any exports of the RHS; at worst we
	 * inline a default value from the category rather than
	 * the replacement value in the RHS.
	 */
	if (type && abnIsDomValued(rhs)) {
		/*
		 * Try to convert
		 *    [Define [Declare Dom Cat] [Add ...]]
		 * into
		 *    [Define [Declare Dom [With Cat ()]] [Add ...]]
		 */
		if (abHasTag(type, AB_Id)) {
			SrcPos	where	= abPos(lhs);
			String	t1	= symString(abIdSym(type));
			String	t2	= symString(ssymType);

			/* Treat Type as being special */
			if (!strEqual(t1, t2)) {
				AbSyn	nowt = abNewNothing(where);

				if (DEBUG(abnDefine)) {
					fprintf(dbOut, "-- abnDefine: ");
					fprintf(dbOut, "changing ");
					fprintf(dbOut, "`%s'", abPretty(type));
				}

				type = abNewWith(where, type, nowt);

				if (DEBUG(abnDefine)) {
					fprintf(dbOut, " into ");
					fprintf(dbOut, "`%s'", abPretty(type));
					fprintf(dbOut, "\n");
				}
			}
		}
		else if (abHasTag(type, AB_Apply)) {
			SrcPos	where	= abPos(lhs);
			AbSyn	nowt = abNewNothing(where);

			if (DEBUG(abnDefine)) {
				fprintf(dbOut, "-- abnDefine: ");
				fprintf(dbOut, "changing ");
				fprintf(dbOut, "`%s'", abPretty(type));
			}

			type = abNewWith(where, type, nowt);

			if (DEBUG(abnDefine)) {
				fprintf(dbOut, " into ");
				fprintf(dbOut, "`%s'", abPretty(type));
				fprintf(dbOut, "\n");
			}
		}
	}
#endif 

	if (abHasTag(lhs, AB_Apply)) {
		targ = abn0DefineTarget(lhs);
		if (!type)
			type = abNewNothing(abPos(lhs));
		/*!! Steal the AB_Hide syntax for now to define packed maps. */
		if (abHasTag(type, AB_Hide)) {
			packed = true;
			otype = type;
			type = otype->abHide.type;
			abFreeNode(otype);
		}
		if (abIsNotNothing(targ))
			rhs = abNewLabel(abPos(rhs), targ, rhs);
		while (abHasTag(lhs, AB_Apply))
			abn0LambdaBehead(&lhs, &rhs, &type, packed);
	}

	if (type && abIsNotNothing(type))
		lhs = abNewDeclare(abPos(lhs), lhs, type);

	ab->abDefine.lhs = lhs;
	ab->abDefine.rhs = rhs;
	return ab;
}


/*
 * This function could always be improved ...
 */
local Bool
abnIsDomValued(AbSyn ab)
{
	AInt	i, argc;


	/* Deal with NULL absyn */
	if (!ab) return false;


	/* Does the absyn look like an add body? */
	switch (abTag(ab)) {
	case AB_Add:
		return true;

	case AB_Exit:
		/* Recurse on exit statements */
		return abnIsDomValued(ab->abExit.value);

	case AB_Return:
		/* Recurse on return statements */
		return abnIsDomValued(ab->abReturn.value);

	case AB_Where:
		/* Can't do much really without checking context */
		return abnIsDomValued(ab->abWhere.expr);

	case AB_If:
		/* Check if-then-else statements */
		if (abnIsDomValued(ab->abIf.thenAlt)) return true;
		if (abnIsDomValued(ab->abIf.elseAlt)) return true;
		return false;

	case AB_Sequence:
		/* How many nodes in the sequence? */
		argc = abArgc(ab);


		/* Empty sequences can't be adds */
		if (!argc) return false;


		/* Recurse on singleton sequences */
		if (argc == 1) return abnIsDomValued(abArgv(ab)[0]);


		/* Check for an obvious return value */
		if (abnIsDomValued(abArgv(ab)[argc-1])) return true;


		/* Search each node in the sequence looking for exits */
		for (i = 0; i < argc - 1;i++) {
			AbSyn	abi = abArgv(ab)[i];

			switch (abTag(abi)) {
			case AB_Return:	/* Fall through */
			case AB_Exit:	/* Fall through */
				if (abnIsDomValued(abi)) return true;
			default: break;
			}
		}


		/* Doesn't look like an add */
		return false;
	default: break;
	}


	/* Doesn't look like an add */
	return false;
}


/*
 * abn0DefineTarget takes f(a:A)(b:B):C and, if f is an Id, returns a copy of f.
 */

local AbSyn
abn0DefineTarget(AbSyn lhs)
{
	if (abHasTag(lhs, AB_Declare))
		lhs = lhs->abDeclare.id;
	while (abHasTag(lhs, AB_Apply))
		lhs = lhs->abApply.op;

	return abHasTag(lhs, AB_Id) ? abCopy(lhs) : abNewNothing(abPos(lhs));
}


/*****************************************************************************
 *
 * :: Documented
 *
 ****************************************************************************/

local AbSyn	abnDocumentedForm(AbSyn ab);

/*
 * abnDocumented takes
 *   [Documented e pre]
 *   with abComment(e) == post
 * to
 *   e
 * with abComment = pre post
 */

local AbSyn
abnDocumented(AbSyn ab)
{
	AbSyn	oab, odoc, abid, abinner;
	Doc	pre = 0, post = 0, doc;
	SrcPos  opos;
	/* Extract the pre-doc. */

	oab  = ab;
	odoc = ab->abDocumented.doc;
	assert(abTag(odoc) == AB_DocText);

	post = odoc->abDocText.doc;
	ab  = ab->abDocumented.expr;
	opos = abPos(ab);
	abFreeNode(odoc);
	abFreeNode(oab);

	abinner = abnDocumentedForm(ab);
	abid = abDefineeIdOrElse(abinner, NULL);

	if (abid) {
		pre = abComment(abid);
		abSetComment(abid, NULL);
	}

	/* Merge the pre-doc and the post-doc. */

	if (pre == 0) {
		abSetComment(ab, post);
		doc = post;
	}
	else {
		doc = docMerge(pre, post);
		docFree(post);
		docFree(pre);
		abSetComment(ab, doc);
	}

	/* See if we can find an identifier to stick this on. */

	if (abid) {
		abSetComment(abid,  doc);
		if (abid != ab)
			abSetComment(ab, NULL);
	}
	else {
		oab = abNewNothing(opos);
		comsgWarning(oab, ALDOR_W_NormNoId);
	}
	return ab;
}

local AbSyn
abnDocumentedForm(AbSyn ab)
{
	if (abTag(ab) == AB_Where)
		ab = ab->abWhere.expr;

	if (abTag(ab) == AB_Extend)
		ab = ab->abExtend.body;

	return ab;
}

/*****************************************************************************
 *
 * :: Import
 *
 ****************************************************************************/

/*
 * abnImport takes
 *   [Import w o]
 * to
 *   [Builtin w]    if o == Builtin
 *   [Foreign w o]  if o == Foreign or Foreign xx  or Foreign xx(yy)
 *   [Import  w o]  otherwise
 */

local AbSyn
abnImport(AbSyn ab)
{
	AbSyn	what = ab->abImport.what, origin = ab->abImport.origin;

	assert(origin != 0);

	if (!abHasTag(what, AB_With) && !abHasTag(what, AB_Nothing)) {
		what = abNewWith(abPos(what),
				 abNewNothing(abPos(what)),
				 what);
		ab->abImport.what = what;
	}

	if (abHasTag(origin, AB_Sequence) || abHasTag(origin, AB_Comma))
		return abnorm(abn0ImportSeq(ab));

#if 0
	/*!! This seems to be necessary, for now, with yacc on AIX 3.2 */
	if (abHasTag(origin, AB_Declare)) {
		AbSyn oorigin = origin;
		abFree(what);
		ab->abImport.what = what = origin->abDeclare.type;
		ab->abImport.origin = origin = origin->abDeclare.id;
		abFreeNode(oorigin);
	}
#endif


	/*
	 * Deal with imports from magic domains Foreign and Builtin:
	 *
	 *    import from Builtin;
	 *    import from Foreign;
	 *    import from Foreign X;
	 *    import from Foreign X("<header1>-<header2>");
	 *
	 * where X is one of C, Lisp, Fortran, Foreign or Builtin. We
	 * don't encourage use of Foreign as an argument to Foreign and
	 * warn the user if they try. Foreign(Builtin) is accepted because
	 * users are not allowed to `export to Builtin': they have to export
	 * to Foreign(Builtin) instead.
	 *
	 * We also allow (with a warning) `import from Foreign()'
	 */
	if (abIsTheId(origin, ssymForeign) || abIsTheId(origin, ssymBuiltin))
		ab = abn0Import(ab, what, origin);
	else if (abIsApplyOf(origin, ssymForeign))
	{
		AbSyn	oorigin = origin;


		/* Extract the foreign origin */
		if (abApplyArgc(origin) == 1)
			origin = abApplyArg(origin, int0);
		else 
			origin = abNewNothing(abPos(origin));


		/* Foreign(Foreign) and Foreign() are discouraged. */
		if (abTag(origin) == AB_Nothing)
		{
			comsgWarning(origin, ALDOR_W_NormNullForeign);
			origin = abNewId(abPos(origin), ssymForeign);
		}
		else if (abIsTheId(origin, ssymForeign))
			comsgWarning(origin, ALDOR_W_NormFornForeign);


		/* Normalise this foreign import */
		ab = abn0Import(ab, what, origin);
		abFreeNode(oorigin);
	}

	return ab;
}

/*
 * Normalise `import WHAT from WHERE' where WHAT is either a category
 * a sequence of declarations, and where WHERE is either Foreign or an
 * application of Foreign (eg. `Foreign Lisp' or `Foreign C("yo")'). If
 * WHERE is Builtin we pass the buck to abn0BuiltinExport.
 */
local AbSyn
abn0Import(AbSyn ab, AbSyn what, AbSyn where)
{
	AbSyn	oab = ab;


	/* Need to be a bit smarter than this ... */
	if (abHasTag(what, AB_With))
	{
		AbSyn owhat = what;
		what = what->abWith.within;
		abFreeNode(owhat);
	}


	/*
	 * If `import Cat from ...' then put the `with' back. Otherwise
	 * if the origin is `Foreign' and we have `import { ... } from'
	 * then we use nothing for the origin. Finally if we have the
	 * code `import { ... } from Builtin' then we pass the task on
	 * to abn0BuiltinImport().
	 */
	if (abHasTag(what, AB_Id))
	{
		SrcPos	pos = abPos(what);
		what = abNewWith(pos, what, abNewNothing(pos));
	}
	else if (abIsTheId(where, ssymForeign))
	{
		AbSyn	owhere = where;
		where = abNewNothing(abPos(where));
		abFreeNode(owhere);
	}
	else if (abIsTheId(where, ssymBuiltin))
	{
		/* Explicit exports from Builtin are special */
		abFreeNode(where);
		return abn0BuiltinImport(ab, what);
	}


	/* Replace the import with a foreign */
	ab = abNewForeign(abPos(oab), what, where);


	/* Release storage associated with the original node */
	abFreeNode(oab);


	/* Return the normalised version */
	return ab;
}

/*
 * Normalise `import WHAT from Builtin' where WHAT is either a
 * category or a sequence of declarations.
 */
local AbSyn
abn0BuiltinImport(AbSyn ab, AbSyn what)
{
	AbSyn	oab = ab;

	/* Need to be a bit smarter than this ... */
	if (abHasTag(what, AB_With))
	{
		AbSyn owhat = what;
		what = what->abWith.within;
		abFreeNode(owhat);
	}


	/* If `import Cat from Builtin' then put the `with' back. */
	if (abHasTag(what, AB_Id))
	{
		SrcPos	pos = abPos(what);
		what = abNewWith(pos, what, abNewNothing(pos));
	}


	/* Replace the import with a builtin */
	ab = abNewBuiltin(abPos(oab), what);


	/* Release storage associated with the original node */
	abFreeNode(oab);


	/* Return the normalised version */
	return ab;
}

/*
 * abn0ImportSeq takes
 *   [Import w [Sequence o1 o2 ...]]
 * and
 *   [Import w [Comma    o1 o2 ...]]
 * to
 *   [Sequence [Import w o1] [Import w o2] ...]
 *
 * If abIsNothing(w) and, say, o1 = [Declare id [With ...]] then the
 * generated statement looks like [Import [With ...] id].
 */

local AbSyn
abn0ImportSeq(AbSyn ab)
{
	AbSyn what   = ab->abImport.what;
	AbSyn origin = ab->abImport.origin, *o = abArgv(ab->abImport.origin);
	int   i, n = abArgc(ab->abImport.origin);

	
	assert(abHasTag(origin, AB_Sequence) || abHasTag(origin, AB_Comma));

	for (i = 0; i < n; i++) {
		/* First check for qualified imports. */

		if (abIsNothing(what) && abHasTag(o[i], AB_Declare) &&
		    abHasTag(o[i]->abDeclare.type, AB_With)) {
			AbSyn wh = o[i]->abDeclare.type,
			      or = o[i]->abDeclare.id;
			abFreeNode(o[i]);
			o[i] = abnImport(abNewImport(abPos(or), wh, or));
		}
		else
			o[i] = abNewImport(abPos(o[i]), abCopy(what),o[i]);
	}
	abFree(ab->abImport.what);
	abFreeNode(ab);

	abTag(origin) = AB_Sequence;
	return origin;
}


/*****************************************************************************
 *
 * :: Export
 *
 ****************************************************************************/

/*
 * abnExport takes
 *   [Export w [Sequence o1 o2 ...]]
 * and
 *   [Export w [Comma    o1 o2 ...]]
 * to
 *   [Sequence [Export w o1] [Export w o2] ...]
 *
 * If abIsNothing(w) and, say, o1 = [Declare id [With ...]] then the
 * generated statement looks like [Export [With ...] id].
 */

local AbSyn
abnExport(AbSyn ab)
{
	AbSyn what   = ab->abExport.what;
	AbSyn dest   = ab->abExport.destination;
	AbSyn origin = ab->abExport.origin;
        AbSyn *o = abArgv(ab->abExport.origin);
	int   i, n = abArgc(ab->abExport.origin);

	if (!abHasTag(dest, AB_Nothing))
	  	return ab;

	if (abHasTag(dest, AB_Nothing) && abHasTag(origin, AB_Nothing))
		return ab;

	if (!abHasTag(what, AB_With) && !abHasTag(what, AB_Nothing)) {
		what = abNewWith(abPos(what),
				 abNewNothing(abPos(what)),
				 what);
		ab->abExport.what = what;
	}

	if (! abHasTag(origin, AB_Sequence) && !abHasTag(origin, AB_Comma))
		return ab;

	for (i = 0; i < n; i++)
		/* First check for qualified Exports. */

		if (abIsNothing(what) && abHasTag(o[i], AB_Declare) &&
		    abHasTag(o[i]->abDeclare.type, AB_With)) {
			AbSyn wh = o[i]->abDeclare.type,
			      or = o[i]->abDeclare.id;
			abFreeNode(o[i]);
			o[i] = abnExport(abNewExport(abPos(or), wh, or, 
						     abNewNothing(abPos(what))));
		}
		else
			o[i] = abNewExport(abPos(o[i]), abCopy(what),o[i],
					   abNewNothing(abPos(what)));
	abFree(ab->abExport.what);
	abFreeNode(ab);

	abTag(origin) = AB_Sequence;
	return origin;
}

/*****************************************************************************
 *
 * :: Inline
 *
 ****************************************************************************/

/*
 * abnInline takes
 *   [Inline w [Sequence o1 o2 ...]]
 * and
 *   [Inline w [Comma    o1 o2 ...]]
 * to
 *   [Sequence [Inline w o1] [Inline w o2] ...]
 *
 * If abIsNothing(w) and, say, o1 = [Declare id [With ...]] then the
 * generated statement looks like [Inline [With ...] id].
 */

local AbSyn
abnInline(AbSyn ab)
{
	AbSyn what   = ab->abInline.what;
	AbSyn origin = ab->abInline.origin, *o = abArgv(ab->abInline.origin);
	int   i, n = abArgc(ab->abInline.origin);

	if (!abHasTag(what, AB_With) && !abHasTag(what, AB_Nothing)) {
		what = abNewWith(abPos(what),
				 abNewNothing(abPos(what)),
				 what);
		ab->abInline.what = what;
	}

	if (! abHasTag(origin, AB_Sequence) && !abHasTag(origin, AB_Comma))
		return ab;

	for (i = 0; i < n; i++)
		/* First check for qualified inlines. */

		if (abIsNothing(what) && abHasTag(o[i], AB_Declare) &&
		    abHasTag(o[i]->abDeclare.type, AB_With)) {
			AbSyn wh = o[i]->abDeclare.type,
			      or = o[i]->abDeclare.id;
			abFreeNode(o[i]);
			o[i] = abnInline(abNewInline(abPos(or), wh, or));
		}
		else
			o[i] = abNewInline(abPos(o[i]), abCopy(what),o[i]);
	abFree(ab->abInline.what);
	abFreeNode(ab);

	abTag(origin) = AB_Sequence;
	return origin;
}


/*****************************************************************************
 *
 * :: Lambda
 *
 ****************************************************************************/

/*
 * abnLambda takes
 *   [Lambda [Declare [ Apply [ Apply a b ] c ] T ]  0	E]
 * to
 *   [Lambda a {B->C->T} [Lambda b {C->T} [Lambda c {T} E ]]]
 */

local AbSyn
abnLambda(AbSyn ab)
{
	AbSyn	lhs   = ab->abLambda.param;
	AbSyn	rhs   = ab->abLambda.body;
	AbSyn	rtype = ab->abLambda.rtype;
	AbSyn	ortype, olhs;
	Bool	packed = false;

	if (abHasTag(lhs, AB_Declare)) {
		assert(abHasTag(rtype, AB_Nothing));

		ortype= rtype;
		olhs  = lhs;
		lhs   = olhs->abDeclare.id;
		rtype = olhs->abDeclare.type;

		abFree(ortype);
		abFreeNode(olhs);
	}

	if (abHasTag(ab, AB_PLambda)) {
		packed = true;
	}
	/*!! Steal the AB_Hide syntax for now for packed lambda exprs. */
	if (abHasTag(rtype, AB_Hide)) {
		packed = true;
		ortype = rtype;
		rtype = ortype->abHide.type;
		abFreeNode(ortype);
	}

	while (abHasTag(lhs, AB_Apply))
		abn0LambdaBehead(&lhs, &rhs, &rtype, packed);

	if (! abHasTag(lhs, AB_Comma))
		lhs = abNewComma1(abPos(lhs), lhs);

	ab->abLambda.param = lhs;
	ab->abLambda.rtype = rtype;
	ab->abLambda.body = rhs;

	return ab;
}


/*
 * abn0LambdaBehead takes
 *   ([Apply H [Declare b B]],	E,   R)
 * to
 *   (H, [Lambda [Declare b B] R E], B->R)
 */

local void
abn0LambdaBehead(AbSyn *plhs, AbSyn *prhs, AbSyn *prtype, Bool packed)
{
	AbSyn	lhs = *plhs, rhs = *prhs, rtype = *prtype;
	AbSyn	args;

	assert(abHasTag(lhs, AB_Apply));

	args = abNewApplyDeclaredArg(lhs);
	*plhs = lhs->abApply.op;
	if (packed)
		*prhs = abNewPLambda(abPos(args), args, rtype, rhs);
	else
		*prhs = abNewLambda(abPos(args), args, rtype, rhs);
	*prtype = abn0LambdaType(args, rtype, packed);
	abFreeNode(lhs);
}

/*
 * abn0LambdaType((a:A,b:B), R) gives (a:A,b:B)->R where a:A, b:B and R
 * are copied.
 */

#define abIsFormalArg(a) (abTag(a) == AB_Declare || abTag(a) == AB_Define)

# define abDeclType(a) \
	(abIsFormalArg(a) ? abCopy(a) : abNewNothing(abPos(a)))

local AbSyn
abn0LambdaType(AbSyn arg, AbSyn rtype, Bool packed)
{
	AbSyn	argType;
	AbSyn	op;
	int	i;

	if (abIsNothing(rtype))
		return abNewNothing(abPos(rtype));

	if (abHasTag(arg, AB_Comma) && abArgc(arg) == 1) {
		if (!abIsFormalArg(abArgv(arg)[0]))
			return abNewNothing(abPos(abArgv(arg)[0]));
		argType = abDeclType(abArgv(arg)[0]);
	}
	else if (abHasTag(arg, AB_Comma)) {
		for (i = 0; i < abArgc(arg); i++)
			if (!abIsFormalArg(abArgv(arg)[i]))
				return abNewNothing(abPos(abArgv(arg)[i]));

		argType = abNewEmpty(AB_Comma, abArgc(arg));
		abSetPos(argType, abPos(arg));

		for (i = 0; i < abArgc(arg); i++)
			abArgv(argType)[i] = abDeclType(abArgv(arg)[i]);
	}
	else if (abIsFormalArg(arg)) {
		argType = abDeclType(arg);
	}
	else
		return abNewNothing(abPos(arg));

	op = abNewId(abPos(arg), packed ? ssymPackedArrow : ssymArrow);
	return abNewApply2(abPos(arg), op, argType, abCopy(rtype));
}


/*****************************************************************************
 *
 * :: Macro
 *
 ****************************************************************************/

local AbSyn abnMacImport		(AbSyn);
local AbSyn abnMacGetImport		(AbSyn, AbSyn);
local AbSyn abnMacExport		(AbSyn);

/*
 * abnMacro
 *
 * 1. takes  macro {e0; e1; ...} to   {macro e0; macro e1; ...}
 * 2. takes  macro a ==	 b	 to   [MDefine a b]
 * 3. takes  macro a +-> b	 to   [MLambda a b]
 * 4. takes  macro (e where d)   to   (macro e) where d
 * 5. takes  macro { import { X, Y, Z } from ZZZ } 
 *	 to macro X == ??? [??? is the definition of X 
 *				   from the library ZZZ]
 * 6. takes  macro { export { A, B, C} } 
 *       to  _, and as a side-effect exports the macros.
 */

local AbSyn
abnMacro(AbSyn ab)
{
	AbSyn	expr = ab->abMacro.expr;
	assert(expr);

	while (abTag(expr) == AB_Paren) {
		AbSyn	oexpr = expr;
		expr = expr->abParen.expr;
		abFreeNode(oexpr);
		assert(expr);
	}

	switch (abTag(expr)) {
	case AB_Lambda: {
		AbSyn	param = expr->abLambda.param;
		AbSyn	body  = expr->abLambda.body;
		SrcPos	spos  = abPos(expr);

		abFree	  (expr->abLambda.rtype);
		abFreeNode(expr);
		abFreeNode(ab);
		ab = abNewMLambda(spos, param, body);
		ab = abnorm(ab);
		break;
	    }
	case AB_Define: {
		AbSyn	lhs  = expr->abDefine.lhs;
		AbSyn	rhs  = expr->abDefine.rhs;
		SrcPos	spos = abPos(expr);

		abFreeNode(expr);
		abFreeNode(ab);
		ab = abNewMDefine(spos, lhs, rhs);
		ab = abnorm(ab);
		break;
	    }
	case AB_Sequence: {
		int	i;

		abFreeNode(ab);
		for (i = 0; i < abArgc(expr); i++) {
			AbSyn	abi = abArgv(expr)[i];
			abArgv(expr)[i] = abNewMacro(abPos(abi), abi);
		}
		ab = abnorm(expr);
		break;
	    }
	case AB_Where: {
		AbSyn	wexpr = expr->abWhere.expr;
		AbSyn	wcon  = expr->abWhere.context;
		SrcPos	spos  = abPos(expr);

		abFreeNode(expr);
		abFreeNode(ab);
		ab = abNewWhere(spos, wcon, abNewMacro(spos, wexpr));
		ab = abnorm(ab);
		break;
	    }
	case AB_Import: {
		ab = abnMacImport(expr);
		break;
	    }
	case AB_Export: {
		ab = abnMacExport(expr);
		break;
	    }
	default:
		comsgError(expr, ALDOR_E_NormMacBadBody);
		ab = abn0Parts(ab);
		break;
	}

	return ab;
}

local AbSyn
abnMacImport(AbSyn expr)
{
	Syme  syme;
	AbSyn macs;
	AbSyn what = expr->abImport.what;
	AbSyn orig = expr->abImport.origin;
	AbSyn new;
	AbSyn *argv;
	Symbol sym;
	int   argc, i;

	if (!abIsId(orig)) {
		comsgError(orig, ALDOR_E_NormMacDecl);
		return abNewNothing(abPos(expr));
	}
	switch (abTag(what)) {
	  case AB_Sequence:
		argv = what->abSequence.argv;
		argc = abArgc(what);
		break;
	  case AB_Id:
		argv = &what;
		argc = 1;
		break;
	  default:
		comsgError(orig, ALDOR_E_NormMacDecl);
		argv = NULL;
		argc = 0;
	}

	sym  = orig->abId.sym;
	syme = stabGetLibrary(sym);

	if (syme) {
		macs = libGetMacros(symeLibrary(syme));
	} else if ( (syme = stabGetArchive(sym)) != NULL) {
		macs = arGetGlobalMacros(symeArchive(syme));
	} else {
		comsgError(orig, ALDOR_E_NormMacDecl);
		return abNewNothing(abPos(expr));
	}

	new = abNewEmpty(AB_Sequence, argc);
	for (i=0; i<argc; i++) {
		AbSyn mac = abnMacGetImport(argv[i], macs);
		if (!mac) {
			comsgError(argv[i], ALDOR_E_NormMacDecl);
			return abNewNothing(abPos(expr));
		}
		
		new->abSequence.argv[i] = mac;
	}
	
	return new;
}

local AbSyn
abnMacGetImport(AbSyn id, AbSyn macs)
{
	Symbol sym;
	int i;
	if (!abIsId(id))
		return NULL;

	assert(abTag(macs) == AB_Sequence);
	sym = id->abId.sym;
	for (i=0; i<abArgc(macs); i++) {
		AbSyn ab = macs->abSequence.argv[i];
		AbSyn r;

		if (abTag(ab) == AB_Sequence)
			if ( (r = abnMacGetImport(id, ab)) != NULL)
				return r;

		if (abTag(ab) != AB_MDefine) continue;

		if (ab->abMDefine.lhs->abId.sym == sym)
			return abCopy(ab);

	}
	
	return NULL;
}

local AbSyn
abnMacExport(AbSyn expr)
{
	AbSyn *argv;
	AbSyn lst;
	int argc, i;	
	
	lst = expr->abExport.what;
	switch (abTag(lst)) {
	  case AB_Define:
		argv = &lst;
		argc = 1;
		break;
	  case AB_Sequence:
		argc = abArgc(lst);
		argv = lst->abSequence.argv;
		break;
	  default:
		comsgError(lst, ALDOR_E_NormMacDecl);
		argc = 0;
		argv = NULL;
	}

	for (i=0; i<argc; i++) {
		if (abTag(argv[i]) == AB_Define) {
			AbSyn new;
			new = abNewMDefine(abPos(argv[i]), 
					   abCopy(argv[i]->abDefine.lhs),
					   abCopy(argv[i]->abDefine.rhs)),
			macexAddMacro(new, true);
			abFree(new);
		}
	}
	return abNewNothing(abPos(lst));
}

/*
 * This allows us to write:
 * macro {
 *	export {
 *		NNI == NonNegativeInteger;
 *		SI  == SingleInteger;
 *		Foo == Bar;
 *	} 
 * }
 *
 * but doesn't allow us to write:
 * macro {
 *      export {
 *              f(x) == x;
 *      }
 * }
 */

/*****************************************************************************
 *
 * :: MDefine
 *
 ****************************************************************************/

/*
 * abnMDefine takes
 *    [MDefine [Apply [Apply A B] C]] E]
 * to
 *    [MDefine A [MLambda B [MLambda C E]]]
 */

local AbSyn
abnMDefine(AbSyn ab)
{
	AbSyn	lhs = ab->abMDefine.lhs, rhs = ab->abMDefine.rhs;
	AbSyn	olhs, rtype;


	if (abHasTag(lhs, AB_Declare)) {
		comsgError(lhs, ALDOR_E_NormMacDecl);
		olhs  = lhs;
		lhs   = olhs->abDeclare.id;
		rtype = olhs->abDeclare.type;
		abFreeNode(olhs);
		abFree(rtype);
	}

	while (abHasTag(lhs, AB_Apply))
		abn0MLambdaBehead(&lhs, &rhs);

	ab->abDefine.lhs = lhs;
	ab->abDefine.rhs = rhs;
	return ab;
}


/*****************************************************************************
 *
 * :: MLambda
 *
 ****************************************************************************/

/*
 * abnMLambda takes
 *   [MLambda [Apply [Apply A B] C] E]
 * to
 *   [MLambda A [MLambda B [MLambda C E]]]
 */

local AbSyn
abnMLambda(AbSyn ab)
{
	AbSyn	lhs = ab->abMLambda.param, rhs = ab->abMLambda.body;
	AbSyn	olhs, rtype;

	if (abHasTag(lhs, AB_Declare)) {
		comsgError(lhs, ALDOR_E_NormMacDecl);
		olhs  = lhs;
		lhs   = olhs->abDeclare.id;
		rtype = olhs->abDeclare.type;
		abFreeNode(olhs);
		abFree(rtype);
	}

	while (abHasTag(lhs, AB_Apply))
		abn0MLambdaBehead(&lhs, &rhs);

	if (! abHasTag(lhs, AB_Comma))
		lhs = abNewComma1(abPos(lhs), lhs);

	ab->abMLambda.param = lhs;
	ab->abMLambda.body = rhs;

	return ab;
}

/*
 * abn0MLambdaBehead takes
 *   ([Apply A B], E)
 * to
 *   (A, [MLambda B E])
 */

local void
abn0MLambdaBehead(AbSyn *plhs, AbSyn *prhs)
{
	AbSyn	lhs = *plhs, rhs = *prhs;
	AbSyn	args;

	assert(abHasTag(lhs, AB_Apply));

	args  = abNewApplyArg(lhs);
	*plhs = lhs->abApply.op;
	*prhs = abNewMLambda(abPos(args), args, rhs);
	abFreeNode(lhs);
}

/*****************************************************************************
 *
 * :: Paren
 *
 ****************************************************************************/

/*
 * abnParen takes
 *   [Paren E]
 * to
 *   E
 */

local AbSyn
abnParen(AbSyn ab)
{
	AbSyn	expr = ab->abParen.expr;
	abFreeNode(ab);
	return expr;
}


/*****************************************************************************
 *
 * :: Qualify
 *
 ****************************************************************************/

/*
 * abnQualify: [Qualify [Apply f [...]] T] +-> [Apply [Qualify f T] [...]]
 */

local AbSyn
abnQualify(AbSyn ab)
{
	AbSyn	what = ab->abQualify.what, origin = ab->abQualify.origin;

	if (! abHasTag(what, AB_Apply))
		return ab;

	what->abApply.op = abNewQualify(abPos(ab), what->abApply.op, origin);
	abFreeNode(ab);
	return what;
}


/*****************************************************************************
 *
 * :: Where
 *
 ****************************************************************************/

/*
 * abnWhere:
 *  (i) remove where's with an empty context
 * (ii) change [Wher: X [Defi: D [Lamb: T R B]]]
 *      to [Defi: D [Lamb: T R [Wher: X B]]]
 */

local AbSyn
abnWhere(AbSyn ab)
{
	AbSyn context = ab->abWhere.context;
	AbSyn expr    = ab->abWhere.expr;
	Bool freeExpr = false;


	/* First remove any where's with an empty context */
	if (! context || abIsNothing(context))
		freeExpr = true;
	else if (abHasTag(context, AB_Sequence)) {
		int i;
		freeExpr = true;
		for (i = 0; i < abArgc(context); i++)
			if (abIsNotNothing(abArgv(context)[i])) {
				freeExpr = false;
				break;
			}
	}


	/* Anything to remove? */
	if (freeExpr)
	{
		if (DEBUG(abnWhere)) {
			fprintf(dbOut, ">> abnWhere():");
			fnewline(dbOut);
			abPrint(dbOut, ab);
			fnewline(dbOut);
			fprintf(dbOut, "<< abnWhere():");
			fnewline(dbOut);
			abPrint(dbOut, expr);
			fnewline(dbOut);
		}
		abFree(context);
		abFreeNode(ab);
		return expr;
	}


	/* -Wno-where */
	if (NoWhereHack) return ab;


	/*
	 * Change [Wher: C [Defi: D [Lamb: T R B]]]
         * to [Defi: D [Lamb: T R [Wher: C B]]] after
	 * macro expansion has been completed.
	 */
	if (abnAfterMacex && abHasTag(expr, AB_Define))
	{
		AbSyn defrhs = expr->abDefine.rhs;

		/* Debugging output: before */
		if (DEBUG(abnWhere)) {
			fprintf(dbOut, ">> abnWhere():");
			fnewline(dbOut);
			abPrint(dbOut, ab);
			fnewline(dbOut);
		}


		/* Are we defining a lambda? */
		if (abHasTag(defrhs, AB_Lambda))
		{
			/*
			 * Create [Wher: C B]: if we were being nice we
			 * would recurse into expr->abDefine.rhs first.
			 */
			ab->abWhere.expr = defrhs->abLambda.body;


			/* Create [Lamb: T R [Wher: C B]] */
			defrhs->abLambda.body = ab;


			/* Now move the define to the top */
			ab = expr;
		}


		/* Debugging output: after */
		if (DEBUG(abnWhere)) {
			fprintf(dbOut, "<< abnWhere():");
			fnewline(dbOut);
			abPrint(dbOut, ab);
			fnewline(dbOut);
		}
	}

	return ab;
}

/*****************************************************************************
 *
 * :: With
 *
 * !! This will need to be cleaned up once the category stuff is finished.
 * !! We need more abFreeNode() calls in here ...
 *
 ****************************************************************************/

/*
 * abnWith: flatten and merge rhs.
 */

local AbSyn
abnWith(AbSyn ab)
{
	AbSyn	base, within;

	if (DEBUG(abnorm)) {
		fprintf(dbOut, ">> Entering abnWith with ");
		abPrint(dbOut, ab);
		fprintf(dbOut, "\n");
	}


	/* Get the flattened components of `base with within' */
	base   = abn0WithFlatten(ab->abWith.base);
	within = abn0WithFlatten(ab->abWith.within);


	/*
	 * Remove nested withs from with-body (this ought to have
	 * been done by abn0WithFlatten):
	 *
	 * [within = with { with W }] --> [within = with W]
	 */
	while ((abTag(within) == AB_With) && abIsNothing(within->abWith.base))
	{
		AbSyn botWithin = within->abWith.within;
		abFreeNode(within->abWith.base);
		abFreeNode(within);
		within = botWithin;
	}


	/*
	 * Remove redundant withs (again this ought to have been
	 * done by abn0WithFlatten):
	 *
	 * [base = with W] --> [base = W]
	 */
	while ((abTag(base) == AB_With) && abIsNothing(base->abWith.base))
	{
		AbSyn botBase = base->abWith.base;
		within = abn0SeqConcat(base->abWith.within, within);
		abFreeNode(base);
		base = botBase;
	}


	/*
	 * Not sure how this case occurs but with-base expressions
	 * represented as a sequence are moved into the with-body:
	 *
	 * [base = {B1; ...; Bn}, within = {W1; ...; Wn}]
	 * --> [base = {}, within = {B1; ...; Bn; W1; ...; Wn}]
	 *
	 * Actually - this probably occurs as a result of the previous
	 * normalisations on the base.
	 */
	if (abTag(base) == AB_Sequence)
	{
		AbSyn botBase = abNewNothing(abPos(base));
		within = abn0SeqConcat(base, within);
		base = botBase;
	}


	/*
	 * Category valued-expressions (identifiers, if-tests and
	 * applications) are moved into a Join in the with-base:
	 *
	 * [base = Join(B1, ..., Bn), within = { C1; ...; Cn; X}]
	 * --> [base = Join(B1, ..., Bn, C1, ..., Cn), within = { X }]
	 *
	 * or
	 *
	 * [base = Cat, within = { C1; ...; Cn; X}] 
         * --> [base = Join(Cat, C1, ..., Cn), within = { X }]
	 *
	 * for categories Ci. Assumes that all identifiers and
	 * applications are category valued. If statements are
	 * category-valued if all components of their branches
	 * are category-valued.
	 */
	if (abnAfterMacex)
	{
		AInt		i;
		AbSynList	copied = listNil(AbSyn);
		AbSynList	others = listNil(AbSyn);


		/* Extract the categories from the base */
		if (abIsJoin(base))
		{
			/* Extract the Join elements */
			for (i = 1;i < abArgc(base); i++)
			{
				AbSyn	arg = abArgv(base)[i];
				copied = listCons(AbSyn)(arg, copied);
			}
		}
		else if (!abIsNothing(base))
			copied = listCons(AbSyn)(base, copied);


		/* Extract categories from the with body */
		if (abTag(within) == AB_Sequence)
		{

			/*
			 * Check each element of the sequence. Identifiers
			 * and applications are considered to be category
			 * values and will be moved into a Join on the LHS.
			 * If expressions that only contain category values
			 * will also be moved.
			 *
			 * Improvement: split if statements into two so
			 * that one only contains category values and the
			 * other contains non-category values (decls etc).
			 * Then we can move the category-valued if into a
			 * Join on the lhs.
			 */
			for (i = 0 ; i < abArgc(within) ; i++ )
			{
				AbSyn arg = abArgv(within)[i];

				if (abIsId(arg) || abIsApply(arg))
					copied = listCons(AbSyn)(arg, copied);
				else if (abIsIf(arg) && ab0IfIsCat(arg))
					copied = listCons(AbSyn)(arg, copied);
				else if (!abIsNothing(arg))
					others = listCons(AbSyn)(arg, others);
			}
		}
		else if (abIsId(within) || abIsApply(within))
			copied = listCons(AbSyn)(within, copied);
		else if (abIsIf(within) && ab0IfIsCat(within))
			copied = listCons(AbSyn)(within, copied);
		else if (!abIsNothing(within))
			others = listCons(AbSyn)(within, others);


		/* Reverse the lists */
		copied = listNReverse(AbSyn)(copied);
		others = listNReverse(AbSyn)(others);


		/* Decide if we need a new with */
		if (listLength(AbSyn)(copied) != 0)
		{
			AbSyn		op = abNewId(abPos(ab), ssymJoin);
			AbSynTag	tag = AB_Apply;
			AbSynList	lis = copied;


			/* New base is a Join */
			base = abNewOfOpAndList(tag, abPos(ab), op, lis);


			/* With-body might be empty */
			if (listLength(AbSyn)(others) != 0)
				within = abNewSequenceL(abPos(ab), others);
			else
				within = abNewNothing(abPos(ab));
		}
	}


	/* Process Joins */
	if (abIsJoin(base))
	{
		/* Flatten into a single Join() */
		base  = abn0JoinFlatten(base);


		/* Remove duplicates */
		base = abn0JoinRemoveDups(base);


		/* Replace nullary and unary Joins */
		switch (abArgc(base))
		{
		   case 1:  base = abNewNothing(abPos(base)); break;
		   case 2:  base = base->abApply.argv[0]; break;
		   default: break;
		}
	}


	/* No point in having duplicates confusing matters */
	abn0WithRemoveDups(within);


	/* Merge default bodies together into a single sequence. */
	within = abn0WithMergeDefaults(within);


	/* Update the with-expression */
	ab->abWith.base	  = base;
	ab->abWith.within = within;

	if (DEBUG(abnorm)) {
		fprintf(dbOut, "<< Leaving abnWith with ");
		abPrint(dbOut, ab);
		fprintf(dbOut, "\n");
	}
	return ab;
}

/*
 * Flatten a sequence of declarations in a with.
 */
local AbSyn
abn0WithFlatten(AbSyn ab)
{
	int i, j, slots;
	Bool mustFlatten = false;
	AbSyn newSeq;

	if (abTag(ab) != AB_Sequence) return ab;

	if (DEBUG(abnorm)) {
		fprintf(dbOut, ">> Entering abn0WithFlatten with ");
		abPrint(dbOut, ab);
		fprintf(dbOut, "\n");
	}

	for (i = 0; i < abArgc(ab); i++) {
		AbSyn arg = abArgv(ab)[i];
		if (abHasTag(arg, AB_Sequence)) {
			abArgv(ab)[i] = abn0WithFlatten(arg);
			mustFlatten = true;
		}
		else if (abHasTag(arg, AB_With) &&
			 abHasTag(arg->abWith.base, AB_Nothing))
		{
			abArgv(ab)[i] = abn0WithFlatten(arg->abWith.within);
			mustFlatten = true;
		}
	}
	if (! mustFlatten) { newSeq = ab; goto done; }

	slots = 0;
	for (i = 0; i < abArgc(ab); i++) {
		AbSyn arg = abArgv(ab)[i];
		if (abHasTag(arg, AB_Sequence))
			slots += abArgc(arg);
		else
			slots++;
	}

	newSeq = abNewEmpty(AB_Sequence, slots);
	slots  = 0;
	for (i = 0; i < abArgc(ab); i++) {
		AbSyn arg = abArgv(ab)[i];
		if (abHasTag(arg, AB_Sequence)) {
			for (j = 0; j < abArgc(arg); j++)
				abArgv(newSeq)[slots++] = abArgv(arg)[j];
			abFreeNode(arg);
		}
		else
			abArgv(newSeq)[slots++] = arg;
	}
	abSetPos(newSeq, abPos(ab));
	abFreeNode(ab);

done:
	if (DEBUG(abnorm)) {
		fprintf(dbOut, "<< Leaving abn0WithFlatten with ");
		abPrint(dbOut, newSeq);
		fprintf(dbOut, "\n");
	}
	return newSeq;
}


/*
 * Test for a category-valued `if' statement. This function
 * will only return sensible results if the absyn has been
 * fully macro expanded: non category-valued expressions may
 * be disguised as macro applications.
 */
local Bool
ab0IfIsCat(AbSyn abif)
{
	AbSyn	abthen, abelse;


	/* Sanity check */
	assert(abIsIf(abif));
	assert(abnAfterMacex);


	/* Split into two branches */
	abthen = abif->abIf.thenAlt;
	abelse = abif->abIf.elseAlt;


	/* Does the `then' branch contain non-category values? */
	switch (abTag(abthen))
	{
	   case AB_Id:		/* Fall through */
	   case AB_Apply:	/* Fall through */
	   case AB_Nothing:	/* Fall through */
		break;
	   case AB_If:
		if (!ab0IfIsCat(abthen)) return false;
		break;
	   case AB_Sequence:
		if (!ab0SeqIsCat(abthen)) return false;
		break;
	   default:
		return false;
	}


	/* Does the `else' branch contain non-category values? */
	switch (abTag(abelse))
	{
	   case AB_Id:		/* Fall through */
	   case AB_Apply:	/* Fall through */
	   case AB_Nothing:	/* Fall through */
		break;
	   case AB_If:
		if (!ab0IfIsCat(abelse)) return false;
		break;
	   case AB_Sequence:
		if (!ab0SeqIsCat(abelse)) return false;
		break;
	   default:
		return false;
	}


	/* Looks good */
	return true;
}


/*
 * Test for a sequence that only contains category values. This
 * function will only return sensible results if the absyn has
 * been fully macro expanded: non category-valued expressions
 * may be disguised as macro applications.
 */
local Bool
ab0SeqIsCat(AbSyn ab)
{
	AInt	i;

	/* Sanity check */
	assert(abTag(ab) == AB_Sequence);
	assert(abnAfterMacex);


	/* Check each component of the sequence */
	for (i = 0; i < abArgc(ab); i++)
	{
		AbSyn	elt = abArgv(ab)[i];


		/* Check to see if category valued */
		switch (abTag(elt))
		{
		   case AB_Id:		/* Fall through */
		   case AB_Apply:	/* Fall through */
		   case AB_Nothing:	/* Fall through */
			break;
		   case AB_If:
			if (!ab0IfIsCat(elt)) return false;
			break;
		   case AB_Sequence:
			if (!ab0SeqIsCat(elt)) return false;
			break;
		   default:
			return false;
		}
	}


	/* Looks good */
	return true;
}


/*
 * Convert Joins of Joins into a single Join.
 */
local AbSyn
abn0JoinFlatten(AbSyn ab)
{
	AInt		i;
	AbSyn		op;
	AbSynList	result = listNil(AbSyn);


	/* Sanity check */
	assert(abIsJoin(ab));


	/* Loop over each element of the Join */
	for (i = 1;i < abArgc(ab); i++)
	{
		AbSyn	args;
		AbSyn	arg = abArgv(ab)[i];

		/* Recursively flatten Joins */
		if (abIsJoin(arg))
		{
			AInt	i;

			/* Flatten it */
			args = abn0JoinFlatten(arg);


			/* Add each argument to our list */
			for (i = 1;i < abArgc(args); i++)
			{
				AbSyn arg = abArgv(args)[i];
				result = listCons(AbSyn)(arg, result);
			}
		}
		else
			result = listCons(AbSyn)(arg, result);
	}


	/* Restore the original order */
	result = listNReverse(AbSyn)(result);


	/* Create a new Join */
	op = abNewId(abPos(ab), ssymJoin);


	/* Return it */
	return abNewOfOpAndList(AB_Apply, abPos(ab), op, result);
}


/*
 * Form a new seq consisting of the elements of s1 followed by those of s2.
 * The nodes s1 and s2 are freed as appropriate.
 */
local AbSyn
abn0SeqConcat(AbSyn s1, AbSyn s2)
{
	AbSyn	*vs1, *vs2, s3;
	int	ns1, ns2, i;

	if (DEBUG(abnorm)) {
		fprintf(dbOut, ">> Entering abn0SeqConcat with ");
		abPrint(dbOut, s1);
		fprintf(dbOut, " and ");
		abPrint(dbOut, s2);
		fprintf(dbOut, "\n");
	}

	if (abTag(s1) == AB_Sequence) {
		vs1 = abArgv(s1);
		ns1 = abArgc(s1);
	}
	else {
		vs1 = &s1;
		ns1 = 1;
	}
	if (abTag(s2) == AB_Sequence) {
		vs2 = abArgv(s2);
		ns2 = abArgc(s2);
	}
	else {
		vs2 = &s2;
		ns2 = 1;
	}
	s3 = abNewEmpty(AB_Sequence, ns1 + ns2);
	for (i = 0; i < ns1; i++) abArgv(s3)[i]	    = vs1[i];
	for (i = 0; i < ns2; i++) abArgv(s3)[ns1+i] = vs2[i];
	if (abTag(s1) == AB_Sequence) abFreeNode(s1);
	if (abTag(s2) == AB_Sequence) abFreeNode(s2);

	if (DEBUG(abnorm)) {
		fprintf(dbOut, "<< Leaving abn0SeqConcat with ");
		abPrint(dbOut, s3);
		fprintf(dbOut, "\n");
	}
	return s3;
}

/*
 * Removes duplicates in a sequence, replacing the duplicates with abNothings.
 */
local void
abn0WithRemoveDups(AbSyn ab)
{
	int i, n;
	AbSyn *v;

	if (! abHasTag(ab, AB_Sequence)) return;

	if (DEBUG(abnorm)) {
		fprintf(dbOut, ">> Entering abn0WithRemoveDups with ");
		abPrint(dbOut, ab);
		fprintf(dbOut, "\n");
	}

	n = abArgc(ab);
	v = abArgv(ab);

	for (i = 0; i < n - 1; i++) {
		AbSyn a = v[i];
		int j;

		if (abIsNothing(a)) continue;

		for (j = i+1; j < n; j++) {
			AbSyn b = v[j];

			if (abIsNothing(b)) continue;

			if (abEqual(a,b)) {
				v[j] = abNewNothing(abPos(b));
				abFree(b);
			}
		}
	}
	if (DEBUG(abnorm)) {
		fprintf(dbOut, "<< Leaving abn0WithRemoveDups with ");
		abPrint(dbOut, ab);
		fprintf(dbOut, "\n");
	}
}


/*
 * Remove duplicates from Joins(): assumes flat Join.
 */
local AbSyn
abn0JoinRemoveDups(AbSyn ab)
{
	AInt		i, j;
	AbSyn		op;
	AbSynList	result = listNil(AbSyn);


	/* Sanity check */
	assert(abIsJoin(ab));


	/* Loop over each element of the Join */
	for (i = 1; i < abArgc(ab); i++)
	{
		AbSyn	arg1 = abArgv(ab)[i];


		/* Blanks are inserted by the inner loop */
		if (abIsNothing(arg1)) continue;


		/* Add this expression to our list */
		result = listCons(AbSyn)(arg1, result);


		/* Compare this element with the later ones */
		for (j = i + 1;j < abArgc(ab);j++)
		{
			AbSyn	arg2 = abArgv(ab)[j];


			/* Blank inserted by earlier pass? */
			if (abIsNothing(arg2)) continue;


			/* Clobber this element if equal */
			if (abEqual(arg1, arg2))
				abArgv(ab)[j] = abNewNothing(abPos(arg2));
		}
	}


	/* Restore the original order */
	result = listNReverse(AbSyn)(result);


	/* Create a new Join */
	op = abNewId(abPos(ab), ssymJoin);


	/* Return it */
	return abNewOfOpAndList(AB_Apply, abPos(ab), op, result);
}


local AbSyn
abn0WithMergeDefaults(AbSyn ab)
{
	/*
	 * Merge default bodies together into a single sequence.
	 * Existing defaults with AB_Sequence bodies are simply
	 * concatenated onto what we have already. Type declaration
	 * defaults are added to the sequence untouched while all
	 * others (defines) are pulled out of their default node:
	 *
	 *   (With
	 *     (Default (Declare a A))
	 *     (Default (Declare b B))
	 *     (Default (Seq (a b c))))
	 *
	 * would become:
	 *
	 *   (With
	 *     (Default
	 *        (Seq
	 *           (Default (Declare a A))
	 *           (Default (Declare b B))
	 *           a b c)))
	 *
	 * Note that if the with body does not contain a sequence
	 * then we have nothing to do. This is because the node
	 * is either a declaration or it is a default block. In
	 * either case we return the with body unchanged.
	 *
	 * !!! We assume that the with body has been flattened.
	 */
	int		i, j, defc, normc;
	AbSyn		result, abnorms, abdefs;
	AbSynList	normals, defs;


	/* We only deal with sequences */
	if (!abHasTag(ab, AB_Sequence))
		return ab;


	/* A little debugging won't hurt us */
	if (DEBUG(abnWithMerge)) {
		fprintf(dbOut, ">> Entering abn0WithMergeDefaults with ");
		abPrint(dbOut, ab);
		fprintf(dbOut, "\n");
	}


	/* Start with two empty lists */
	normals  = listNil(AbSyn);
	defs     = listNil(AbSyn);


	/* Split the sequence into defaults and non-defaults */
	for (i = 0; i < abArgc(ab); i++)
	{
		AbSyn arg = abArgv(ab)[i];

		if (abHasTag(arg, AB_Default))
		{
			AbSyn body = arg->abDefault.body;

			/* What do we do with the body? */
			switch (abTag(body))
			{
			   case AB_Sequence:
				/* Sequences must be pulled apart */
				for (j = 0; j < abArgc(body); j++)
				{
					AbSyn	a = abArgv(body)[j];
					defs = listCons(AbSyn)(a, defs);
				}
				break;
			   case AB_Declare:
				/* Declares keep their default tag */
				defs = listCons(AbSyn)(arg, defs);
				break;
			   default:
				/* All others lose their default tag */
				defs = listCons(AbSyn)(body, defs);
				break;
			}
		}
		else
			normals  = listCons(AbSyn)(arg, normals);
	}


	/*
	 * Normalise the non-defaults: we either want nothing, a
	 * single node or a sequence.
	 */
	normc = listLength(AbSyn)(normals);
	switch (normc)
	{
	   case 0:
		abnorms = abNewNothing(abPos(ab));
		break;
	   case 1:
		abnorms = car(normals);
		break;
	   default:
		normals = listNReverse(AbSyn)(normals);
		abnorms = abNewSequenceL(abPos(ab), normals);
		break;
	}


	/*
	 * Normalise the defaults: we either want nothing or a
	 * single default. If we have a singleton list with a
	 * default then we can leave it as it is.
	 */
	defc = listLength(AbSyn)(defs);
	switch (defc)
	{
	   case 0:
		abdefs = abNewNothing(abPos(ab));
		break;
	   case 1:
		abdefs = car(defs);
		if (!abHasTag(abdefs, AB_Default))
			abdefs = abNewDefault(abPos(abdefs), abdefs);
		break;
	   default:
		defs = listNReverse(AbSyn)(defs);
		abdefs   = abNewSequenceL(abPos(ab), defs);
		abdefs   = abNewDefault(abPos(abdefs), abdefs);
		break;
	}


	/* Work out what the new with body is */
	if (normc)		/* We have at least one non-default... */
	{
		if (defc)	/* ... and at least one default */
			result = abn0SeqConcat(abnorms, abdefs);
		else		/* ... but no defaults */
			result = abnorms;
	}
	else			/* We don't have any non-defaults ... */
	{
		if (defc)	/* ... but at least one default */
			result = abdefs;
		else		/* ... or anything else */
			result = ab;
	}


	/* Throw out the garbage */
	listFree(AbSyn)(normals);
	listFree(AbSyn)(defs);


	/* I've started so I'll finish ... */
	if (DEBUG(abnWithMerge)) {
		fprintf(dbOut, "<< Leaving abn0WithMergeDefaults with ");
		abPrint(dbOut, result);
		fprintf(dbOut, "\n");
	}
	return result;
}
