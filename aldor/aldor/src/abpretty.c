/*****************************************************************************
 *
 * abpretty.c: Pretty print abstract syntax to produce inputable Aldor code
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "spesym.h"
#include "syme.h"


#define CONTINUED	"..."
#define NCONTINUED	(sizeof("...") - 1)
#define MAX_NEWLINES	2
#define LINE_BLOCK_SIZE 4
#define MAX_PREC	255
#define INDENTATION	"        "
#define HALFINDENTATION "    "


local Bool	abPPClipped0(Buffer, AbSyn, long *pmaxchars);
local Bool	abPPPutc(Buffer, int, long *pmaxchars);
local Bool	abPPPuts(Buffer, String, long *pmaxchars);

local Bool	abPPSingleNewLine(Buffer buf, long *pmaxchars);
local Bool	abPPIndent(Buffer buf, long *pmaxchars);
local Bool	abPPParens(int prec, Buffer buf, AbSyn ab, long *pmaxchars);

local Bool	abPPApply(Buffer, AbSyn, long *pmaxchars);
local Bool	abPPIterators(Buffer, int itc, AbSyn *itv, long *pmaxchars);
local Bool	abPPLambda(Buffer, AbSyn, long *pmaxchars, Bool packed, AbSyn);
local Bool	abPPSequence(Buffer, AbSyn, long *pmaxchars);

local Bool	abPPIsElidedLambda	(AbSyn);
local Bool	abPPElidedLambda	(Buffer, AbSyn, long *);
local Bool 	abPPIsDefine		(AbSyn);

local Bool	symeClipped0(Buffer, Syme, long *pmaxchars);

local int	getPrecedence(AbSyn ab);
local Bool	isLeftAssociative(AbSyn ab);

static int	sequenceLevel;
static Bool	usePiles;


/*****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

/*
 * Abstract syntax functions.
 */
String
abPretty(AbSyn ab)
{
	return abPrettyClippedIn(ab, ABPP_UNCLIPPED, ABPP_NOINDENT);
}

String
abPrettyClippedIn(AbSyn ab, long maxchars, int ind)
{
	/* ind = number of tabs stops of indentation */

	Buffer	buf;
	Bool	hasContinuedRoom = (maxchars >= NCONTINUED);

	sequenceLevel = ind;
	usePiles = true;

	buf = bufNew();

	if (hasContinuedRoom)
		maxchars -= NCONTINUED;

	if (!abPPClipped0(buf, ab, &maxchars) && hasContinuedRoom)
		bufPuts(buf, CONTINUED);

	return bufLiberate(buf);
}

int
abPrettyPrint(FILE * fout, AbSyn ab)
{
	return abPrettyPrintClippedIn(fout, ab, ABPP_UNCLIPPED, ABPP_NOINDENT);
}

int
abPrettyPrintClippedIn(FILE * fout, AbSyn ab, long maxchars, int ind)
{
	String	s  = abPrettyClippedIn(ab, maxchars, ind);
	int	cc = fprintf(fout, "%s", s);
	strFree(s);
	return cc;
}

/*
 * Type form functions.
 */

String
tfPretty(TForm tf)
{
	return tfPrettyClippedIn(tf, ABPP_UNCLIPPED, ABPP_NOINDENT);
}

String
tfPrettyClippedIn(TForm tf, long maxchars, int ind)
{
	String	s;
	assert(tf);

	if (tfIsUnknown(tf))
		s = strCopy("-- Unknown type --");
	else if (tfIsNone(tf))
		s = strCopy("(0 types)");
	else
		s = abPrettyClippedIn(tfToAbSynPretty(tf), maxchars, ind);

	return s;
}

int
tfPrettyPrint(FILE *fout, TForm tf)
{
	return tfPrettyPrintClippedIn(fout, tf, ABPP_UNCLIPPED, ABPP_NOINDENT);
}

int
tfPrettyPrintClippedIn(FILE *fout, TForm tf, long maxchars, int ind)
{
	String	s  = tfPrettyClippedIn(tf, maxchars, ind);
	int	cc = fprintf(fout, "%s", s);
	strFree(s);
	return cc;
}

/*
 * Symbol expression functions.
 */

String
symePretty(Syme syme)
{
	assert(syme);
	return symePrettyClippedIn(syme, ABPP_UNCLIPPED, ABPP_NOINDENT);
}

String
symePrettyClippedIn(Syme syme, long maxchars, int ind)
{
	/* ind = number of tabs stops of indentation */

	Buffer	buf;
	Bool	hasContinuedRoom = (maxchars >= NCONTINUED);

	assert(syme);
	sequenceLevel = ind;
	usePiles = true;

	buf = bufNew();

	if (hasContinuedRoom)
		maxchars -= NCONTINUED;

	if (!symeClipped0(buf, syme, &maxchars) && hasContinuedRoom)
		bufPuts(buf, CONTINUED);

	return bufLiberate(buf);
}

/*****************************************************************************
 *
 * :: The Guts
 *
 ****************************************************************************/

/*
 * Return whether the expression fit.
 */

local Bool
abPPClipped0(Buffer buf, AbSyn ab, long *pmaxchars)
{
	Bool	f;		/* Fits? */
	int	i;
	AbSynTag tag;
	String	str;

	assert(ab != 0);

	f = *pmaxchars > 0;

	if (! f)
		return f;

	tag = abTag(ab);

	switch (tag) {
	case AB_Nothing:
		f = f && abPPPuts(buf, "()", pmaxchars);
		break;
	case AB_Label:
		f = f && abPPClipped0(buf, ab->abLabel.expr , pmaxchars);
		break;
	case AB_Paren:
		f = f && abPPClipped0(buf, ab->abParen.expr , pmaxchars);
		break;

	case AB_Blank:
	case AB_IdSy:
	case AB_Id:
		f = f && abPPPuts(buf, symString(abLeafSym(ab)), pmaxchars);
		break;

	case AB_LitInteger:
	case AB_LitFloat:
		f = f && abPPPuts(buf, abLeafStr(ab), pmaxchars);
		break;

	case AB_LitString:
		{
		    f = f && abPPPutc(buf, '"', pmaxchars)
			  && abPPPuts(buf, ab->abLitString.str, pmaxchars)
			  && abPPPutc(buf, '"', pmaxchars);
		    break;
		}

	case AB_Apply:
		f = f && abPPApply(buf, ab, pmaxchars);
		break;

	case AB_Lambda:
	case AB_PLambda:
		f = f && abPPLambda(buf, ab, pmaxchars, 
				    tag == AB_PLambda, NULL);
		break;

	case AB_Sequence:
		f = f && abPPSequence(buf, ab, pmaxchars);
		break;

	case AB_Documented:
		f = f && abPPClipped0(buf, ab->abDocumented.doc , pmaxchars)
		      && abPPClipped0(buf, ab->abDocumented.expr, pmaxchars);
		break;

	case AB_DocText:
		{
		    char *d = docString(ab->abDocText.doc);

		    f = f && abPPIndent(buf, pmaxchars)
			  && abPPPuts(buf, "++", pmaxchars);

		    while (d && *d) {
			if (*d == '\n') {
			    if (*(++d))
				f = f && abPPIndent(buf, pmaxchars)
				      && abPPPuts(buf, "++", pmaxchars);
			}
			else {
			    f = f && abPPPutc(buf, *d, pmaxchars);
			    ++d;
			}
		}

		f = f && abPPIndent(buf, pmaxchars);
		}
		break;

	case AB_Collect:
		f = f && abPPClipped0(buf, ab->abCollect.body, pmaxchars)
		      && abPPPutc(buf, ' ', pmaxchars)
		      && abPPIterators(buf, abCollectIterc(ab), ab->abCollect.iterv,
			pmaxchars);
		break;

	case AB_Repeat:
		{
		    Bool    ind;
		    String  repeat;
		    int	    itc = abRepeatIterc(ab);

		    repeat = (itc > 0) ? " repeat" : "repeat";

		    f = f && abPPIterators(buf, itc, ab->abRepeat.iterv, pmaxchars)
			  && abPPPuts(buf, repeat, pmaxchars);

		    ind = (abTag(ab->abRepeat.body) != AB_Sequence);

		    if (ind) {
			    sequenceLevel++;
			    f = f && abPPIndent(buf, pmaxchars);
		    }

		    f = f && abPPClipped0(buf, ab->abRepeat.body, pmaxchars);

		    if (ind)
			    sequenceLevel--;

		    break;
		}

	case AB_For:
		f = f && abPPPuts(buf, "for ", pmaxchars)
		      && abPPClipped0(buf, ab->abFor.lhs, pmaxchars)
		      && abPPPuts(buf, " in ", pmaxchars)
		      && abPPClipped0(buf, ab->abFor.whole, pmaxchars);
		if (!abIsNothing(ab->abFor.test)) {
			Bool	saveUsePiles = usePiles;
			AbSyn	test = ab->abFor.test;

			usePiles = false;
			f = f && abPPPuts(buf, " | ", pmaxchars)
			      && abPPClipped0(buf, test, pmaxchars);
			usePiles = saveUsePiles;
		}
		break;

	case AB_Test:
		f = f && abPPClipped0(buf, ab->abTest.cond, pmaxchars);
		break;

	case AB_While:
		{
		    Bool    saveUsePiles = usePiles;

		    usePiles = false;
		    f = f && abPPPuts(buf, "while ", pmaxchars)
			  && abPPClipped0(buf, ab->abWhile.test, pmaxchars);
		    usePiles = saveUsePiles;
		    break;
		}

	case AB_If:
		f = f && abPPIndent(buf, pmaxchars)
		      && abPPPuts(buf, "if (", pmaxchars)
		      && abPPClipped0(buf, abArgv(ab)[0], pmaxchars)
		      && abPPPuts(buf, ") then ", pmaxchars)
		      && abPPClipped0(buf, abArgv(ab)[1], pmaxchars);
		if (abIsNotNothing(abArgv(ab)[2]))
			f = f && abPPIndent(buf, pmaxchars)
			      && abPPPuts(buf, "else ", pmaxchars)
			      && abPPClipped0(buf, abArgv(ab)[2], pmaxchars);
		break;

	case AB_Define:
		if (abPPIsDefine(ab->abDefine.lhs))
			f = f && abPPClipped0(buf, ab->abDefine.lhs, pmaxchars);
		else if (abPPIsElidedLambda(ab)) 
			f = f && abPPElidedLambda(buf, ab, pmaxchars);
		else {
			if (! abIsAnyLambda(ab->abDefine.rhs)) {
				/*
				 * '==' is right associative.
				 */	
				
				AbSyn lhs   =  ab->abDefine.lhs;
				Bool isWith =
					abHasTag(lhs, AB_Declare) &&
						abHasTag(lhs->abDeclare.type, AB_With);

				if (isWith)
					f = f && abPPIndent(buf, pmaxchars);

				if (!isLeftAssociative(lhs))
					f = f && abPPParens(MAX_PREC, buf, 
							    lhs, pmaxchars);
				else	
					f = f && abPPClipped0(buf, lhs,
							      pmaxchars);

				if (isWith)
					f = f && abPPIndent(buf, pmaxchars)
						&& abPPPuts(buf, HALFINDENTATION, 
							    pmaxchars);
				else
					f = f && abPPPutc(buf, ' ', pmaxchars);

				f = f && abPPPuts(buf, "== ", pmaxchars);
			}
			f = f && abPPClipped0(buf, ab->abDefine.rhs, pmaxchars);
		}
		break;

	case AB_Assign:
	case AB_Qualify:
		{
		    /*
		     * These fellows are right associative.
		     */
		    String  op;

		    op = (tag == AB_Assign) ? " := " : "$";

		    if (!isLeftAssociative(abArgv(ab)[0]))
			    f = f && abPPParens(MAX_PREC, buf, abArgv(ab)[0], pmaxchars);
		    else
			    f = f && abPPClipped0(buf, abArgv(ab)[0], pmaxchars);
		    f = f && abPPPuts(buf, op, pmaxchars)
			  && abPPClipped0(buf, abArgv(ab)[1], pmaxchars);
		    break;
		}

	case AB_Declare:
		{
		    if (abIsNotNothing(abArgv(ab)[0])) {
			    if (abIsTheId(abArgv(ab)[0], ssymBracket))
				    f = f && abPPPuts(buf, symString(ssymBracket), pmaxchars);
			    else if (abIsTheId(abArgv(ab)[0], ssymBrace))
				    f = f && abPPPuts(buf, symString(ssymBrace), pmaxchars);
			    else
				    f = f && abPPClipped0(buf, abArgv(ab)[0], pmaxchars);
			    f = f && abPPPuts(buf, ": ", pmaxchars);
		    }
		    f = f && abPPClipped0(buf, abArgv(ab)[1], pmaxchars);
		    break;
		}

	case AB_Comma:
		for (i = 0; f && i < ab->abHdr.argc; i++)
			if (abIsNotNothing(abArgv(ab)[i])) {
				f = f && abPPClipped0(buf, abArgv(ab)[i], pmaxchars);
				if (i < ab->abHdr.argc - 1)
					f = f && abPPPuts(buf, ", ", pmaxchars);
			}
		break;

	case AB_Export:
		{
		    Bool     seq   = abHasTag(abArgv(ab)[0], AB_Sequence);
		    TokenTag tTag  = abInfo(tag).tokenTag;

		    str	    = keyString(tTag);

		    f = f && abPPPuts(buf, str, pmaxchars);

		    if (abIsNotNothing(abArgv(ab)[0])) {
			    if (! seq)
				    f = f && abPPPutc(buf, ' ', pmaxchars);
			    f = f && abPPClipped0(buf, abArgv(ab)[0], pmaxchars);
		    }

		    if (abIsNotNothing(abArgv(ab)[1])) {
			    if (seq)
				    f = f && abPPIndent(buf, pmaxchars)
					  && abPPPuts(buf, HALFINDENTATION, pmaxchars);
			    else
				    f = f && abPPPutc(buf, ' ', pmaxchars);
			    f = f && abPPPuts(buf, "to ", pmaxchars)
				  && abPPClipped0(buf, abArgv(ab)[1], pmaxchars);
		    }
		}
		break;

	case AB_Extend:
		{
		    Bool     seq   = abHasTag(abArgv(ab)[0], AB_Sequence);
		    TokenTag tTag  = abInfo(tag).tokenTag;

		    str	    = keyString(tTag);

		    f = f && abPPPuts(buf, str, pmaxchars);

		    if (abIsNotNothing(abArgv(ab)[0])) {
			    if (! seq)
				    f = f && abPPPutc(buf, ' ', pmaxchars);
			    f = f && abPPClipped0(buf, abArgv(ab)[0], pmaxchars);
		    }
		}
		break;

	case AB_Import:
	case AB_Inline:
		{
		    AbSyn what = abArgv(ab)[0], origin = abArgv(ab)[1];

		    TokenTag tTag  = abInfo(tag).tokenTag;

		    f = f && abPPPuts(buf, keyString(tTag), pmaxchars);
		    f = f && abPPPuts(buf, " ", pmaxchars);

		    if (abIsNotNothing(what)) {
			    if (abHasTag(what, AB_With)) {
				    if (! abHasTag(what->abWith.within, AB_Sequence))
					    f = f && abPPIndent(buf, pmaxchars)
						  && abPPPuts(buf, INDENTATION, pmaxchars);
				    f = f && abPPClipped0(buf, what->abWith.within, pmaxchars);
			    }
			    else {
				    if (! abHasTag(what, AB_Sequence))
					    f = f && abPPIndent(buf, pmaxchars)
						  && abPPPuts(buf, INDENTATION, pmaxchars);
				    f = f && abPPClipped0(buf, what, pmaxchars);
			    }
			    f = f && abPPIndent(buf, pmaxchars);
		    };
		    if (abIsNotNothing(origin))
			    f = f && abPPPuts(buf, "from ", pmaxchars)
				  && abPPClipped0(buf, origin, pmaxchars);

		}
		break;

	case AB_Builtin:
		{
		    f = f && abPPPuts(buf, "import ", pmaxchars);

		    if (! abHasTag(ab->abBuiltin.what, AB_Sequence))
			    f = f && abPPIndent(buf, pmaxchars)
				  && abPPPuts(buf, INDENTATION, pmaxchars);
		    f = f && abPPClipped0(buf, ab->abBuiltin.what, pmaxchars);
		    f = f && abPPIndent(buf, pmaxchars);
		    f = f && abPPPuts(buf, "from Builtin", pmaxchars);
		}
		break;

	case AB_Foreign:
		{
		    f = f && abPPPuts(buf, "import ", pmaxchars);

		    if (! abHasTag(ab->abForeign.what, AB_Sequence))
			    f = f && abPPIndent(buf, pmaxchars)
				  && abPPPuts(buf, INDENTATION, pmaxchars);

		    f = f && abPPClipped0(buf, ab->abForeign.what, pmaxchars);

		    f = f && abPPIndent(buf, pmaxchars);
		    f = f && abPPPuts(buf, "from Foreign(", pmaxchars);
		    if (abIsNotNothing(ab->abForeign.origin))
			    f = f && abPPClipped0(buf, ab->abForeign.origin, pmaxchars);
		    f = f && abPPPuts(buf, ")", pmaxchars);
		}
		break;

	case AB_Where:
	    {
		AbSyn context = ab->abWhere.context, expr = ab->abWhere.expr;

		f = f && abPPClipped0(buf, expr, pmaxchars);
		if (context && abIsNotNothing(context)) {
			f = f && abPPPuts(buf, " where", pmaxchars);
			sequenceLevel++;
			if (! abHasTag(context, AB_Sequence))
				f = f && abPPIndent(buf, pmaxchars);
			f = f && abPPClipped0(buf, context, pmaxchars);
			sequenceLevel--;
		}

		break;
	    }

	case AB_Add:
	    {
		AbSyn	base	= ab->abAdd.base;
		AbSyn	capsule = ab->abAdd.capsule;

		if (abIsNotNothing(base))
			f = f && abPPClipped0(buf, base, pmaxchars);
		f = f && abPPPuts(buf, " add ", pmaxchars);
		if (abIsNotNothing(capsule)) {
			sequenceLevel++;
			if (! abHasTag(capsule, AB_Sequence))
				f = f && abPPIndent(buf, pmaxchars);
		}
		f = f && abPPClipped0(buf, capsule, pmaxchars);
		if (abIsNotNothing(capsule))
			sequenceLevel--;
		f = f && abPPIndent(buf, pmaxchars);
		break;
	    }

	case AB_With:
	    {
		AbSyn	base   = ab->abWith.base;
		AbSyn	within = ab->abWith.within;
		if (abIsNotNothing(base))
			f = f && abPPClipped0(buf, base, pmaxchars);
		f = f && abPPPuts(buf, " with ", pmaxchars);
		if (abIsNotNothing(within))
			f = f && abPPClipped0(buf, within, pmaxchars);
		break;
	    }
	case AB_Never:
		if (abUse(ab) == AB_Use_Elided)
			f = f && abPPPuts(buf, "..", pmaxchars);
		else
			f = f && abPPPuts(buf, "never", pmaxchars);
		break;
	/*
	 * Remaining cases are supposed to fall through to default.
	 * Skip a line before these forms if they are followed by a
	 * sequence.
	 */

	case AB_Default:
		if (abIsNothing(abArgv(ab)[0]))
			break;
		
	default:
	    {
		TokenTag tTag = abInfo(abTag(ab)).tokenTag;
		UByte	mInf = (tTag == TK_LIMIT) ? 0 : tokInfo(tTag).isMaybeInfix;

		if (tTag == TK_LIMIT)
			str = abInfo(tag).str;	/* !! Can do better */
		else
			str = keyString(tTag);

#if AXL_EDIT_1_1_13_11
		if (mInf && (abArgc(ab) == 2)) {
			if (abHasTag(abArgv(ab)[0], AB_Comma))
				f = f && abPPPutc(buf, '(', pmaxchars)
				      && abPPClipped0(buf, abArgv(ab)[0], pmaxchars)
				      && abPPPuts(buf, ") ", pmaxchars);
			else if (abIsNotNothing(abArgv(ab)[0]))
				f = f && abPPClipped0(buf, abArgv(ab)[0], pmaxchars)
				      && abPPPuts(buf, " ", pmaxchars);

			f = f && abPPPuts(buf, str, pmaxchars);

			if (abHasTag(abArgv(ab)[1], AB_Comma))
				f = f && abPPPuts(buf, " (", pmaxchars)
				      && abPPClipped0(buf, abArgv(ab)[1], pmaxchars)
				      && abPPPutc(buf, ')', pmaxchars);
			else if (abIsNotNothing(abArgv(ab)[1]))
				f = f && abPPPutc(buf, ' ', pmaxchars)
				      && abPPClipped0(buf, abArgv(ab)[1], pmaxchars);
		}
#else
		if (mInf && (abArgc(ab) == 2)) {
			if (abIsNotNothing(abArgv(ab)[0]))
				f = f && abPPClipped0(buf, abArgv(ab)[0], pmaxchars)
				      && abPPPuts(buf, " ", pmaxchars);

			f = f && abPPPuts(buf, str, pmaxchars);

			if (abIsNotNothing(abArgv(ab)[1]))
				f = f && abPPPutc(buf, ' ', pmaxchars)
				      && abPPClipped0(buf, abArgv(ab)[1], pmaxchars);
		}
#endif
		else {
			f = f && abPPPuts(buf, str, pmaxchars)
			      && abPPPutc(buf, ' ', pmaxchars);

			for (i = 0; f && i < ab->abHdr.argc; i++) {
				if (abIsNothing(abArgv(ab)[i]))
					continue;

				f = f && abPPClipped0(buf, abArgv(ab)[i], pmaxchars);

				if (tag == AB_For)
					if (i < ab->abHdr.argc - 1)
						f = f && abPPPutc(buf, ' ', pmaxchars);
			}
		}
	    }
	    break;
	}

	return f;
}

local Bool
abPPSingleNewLine(Buffer buf, long *pmaxchars)
{
	return abPPPutc(buf, '\n', pmaxchars);
}

local Bool
abPPPutc(Buffer buf, int c, long *pmaxchars)
{
	if (*pmaxchars >= 1) {
		/* Character fits */
		BUF_ADD1(buf, (char) c);
		BUF_ADD1(buf, char0);
		BUF_BACK1(buf);
		*pmaxchars -= 1;
		return true;
	}

	/* Doesn't fit at all */
	return false;
}

local Bool
abPPPuts(Buffer buf, String s, long *pmaxchars)
{
	int	n = strlen(s);

	if (*pmaxchars >= n) {
		/* String fits completely */

		bufPuts(buf, s);
		*pmaxchars -= n;
		return true;
	}
	else if (*pmaxchars > 0) {
		/* Fits partially */
		bufAddn(buf, s, *pmaxchars);
		bufPuts(buf, "");
		*pmaxchars = 0;
		return false;
	}
	else {
		/* Doesn't fit at all */
		return false;
	}
}

local Bool
abPPIndent(Buffer buf, long *pmaxchars)
{
	Bool	f = (*pmaxchars > 0);
	int	i = sequenceLevel;

	if (!f)
		return f;

	f = f && abPPSingleNewLine(buf, pmaxchars);

	while (f && (i > 0)) {
		f = f && abPPPuts(buf, INDENTATION, pmaxchars);
		i--;
	}

	return f;
}

local Bool
abPPParens(int prec, Buffer buf, AbSyn ab, long *pmaxchars)
{
	/*
	 * prec is the precedence if ab is an argument to something if prec =
	 * MAX_PREC, always add parens.
	 */

	Bool	f = true, useParens;

	useParens = ((prec == MAX_PREC) || (getPrecedence(ab) < prec)) ? true : false;

	if (useParens)
		f = f && abPPPutc(buf, '(', pmaxchars);

	f = f && abPPClipped0(buf, ab, pmaxchars);

	if (useParens)
		f = f && abPPPutc(buf, ')', pmaxchars);

	return f;
}

local Bool
abPPApply(Buffer buf, AbSyn ab, long *pmaxchars)
{
	/*
	 * Handle applications, taking into account precedence and
	 * associativity.
	 */

	Bool	f = true;
	String	str;
	AbSyn	op = abApplyOp(ab);
	TokenTag tTag = TK_LIMIT;
	UByte	mInf = 0;
	int	i, p;
	Bool	bracket = abIsTheId(op, ssymBracket);
	Bool	brace	= abIsTheId(op, ssymBrace  );


	if (abHasTag(op,AB_Id)) {
		str = symString(op->abId.sym);
		tTag = keyTag(str);
		mInf = (tTag == TK_LIMIT) ? (UByte) 0 : tokInfo(tTag).isMaybeInfix;
	}

	if (bracket || brace) {
		f = f && abPPPutc(buf, (bracket ? '[' : '{'), pmaxchars);

		for (i = 1; f && i < ab->abHdr.argc; i++) {
			f = f && abPPClipped0(buf,
				abArgv(ab)[i], pmaxchars);
			if (i < ab->abHdr.argc - 1)
				f = f && abPPPuts(buf, ", ", pmaxchars);
		}

		f = f && abPPPutc(buf, (bracket ? ']' : '}'), pmaxchars);
	}
	else if ((tTag == KW_Minus) && (abApplyArgc(ab) == 1)) {
		/* prefix '-': has higher precedence than binary '-' */

		p = 1 + getPrecedence(op);
		f = f && abPPClipped0(buf, op, pmaxchars)
		      && abPPPutc(buf, ' ', pmaxchars)
		      && abPPParens(p, buf, abArgv(ab)[1], pmaxchars);
	}
	else if (mInf && (abApplyArgc(ab) == 2)) {
		int q;
		p = getPrecedence(op);

		q = (abIsAnyMap(ab) && abHasTag(abArgv(ab)[1], AB_Declare))
			? MAX_PREC
			: p;

		f = f && abPPParens(q, buf, abArgv(ab)[1], pmaxchars)
		      && abPPPutc(buf, ' ', pmaxchars)
		      && abPPClipped0(buf, op, pmaxchars)
		      && abPPPutc(buf, ' ', pmaxchars)
		      && abPPParens(p, buf, abArgv(ab)[2], pmaxchars);
	}
	else {
		f = f && abPPClipped0(buf, abArgv(ab)[0], pmaxchars);
		f = f && abPPPutc(buf, '(', pmaxchars);

		for (i = 1; f && i < ab->abHdr.argc; i++) {
			f = f && abPPClipped0(buf,
				abArgv(ab)[i], pmaxchars);
			if (i < ab->abHdr.argc - 1)
				f = f && abPPPuts(buf, ", ", pmaxchars);
		}
		f = f && abPPPutc(buf, ')', pmaxchars);
	}

	return f;
}

local Bool 
abPPIsDefine(AbSyn ab)
{
	if (abTag(ab) != AB_Declare)
		return false;
	if (abTag(ab->abDeclare.type) != AB_Define)
		return false;
	return true;
}

local Bool
abPPIsElidedLambda(AbSyn ab)
{
	if (!abIsAnyLambda(ab->abDefine.rhs))
		return false;
	if (abUse(ab->abDefine.rhs->abLambda.body) != AB_Use_Elided)
		return false;
	if (abTag(ab->abDefine.lhs) != AB_Declare)
		return false;

	return true;
}

local Bool
abPPElidedLambda(Buffer buf, AbSyn ab, long *pmaxchars)
{
	AbSyn lambda = ab->abDefine.rhs;
	AbSyn decl = ab->abDefine.lhs;
	Bool  f = true;
	
	assert(abTag(ab) == AB_Define);
	assert(abTag(decl) == AB_Declare);
	assert(abTag(decl->abDeclare.id) == AB_Id);
	f = f && abPPLambda(buf, ab->abDefine.rhs, pmaxchars, 
			    abTag(lambda) == AB_PLambda, 
			    decl->abDeclare.id);
	return f;
	       
}

local Bool
abPPLambda(Buffer buf, AbSyn ab, long *pmaxchars, Bool packed, 
	   AbSyn label)
{
	/*
	 * Handle lambda bodies.
	 */

	AbSyn	body = ab->abLambda.body, param = ab->abLambda.param;
	TokenTag tTag;
	String	op, str;
	UByte	mInf = 0;
	Bool	f = true;

	if (!label && abHasTag(body,AB_Label)) {
		label = body->abLabel.label;

		assert(abHasTag(label, AB_Id));
		body = body->abLabel.expr;
	}
	if (label) {
		str = symString(label->abId.sym);

		tTag = keyTag(str);

		mInf = (tTag == TK_LIMIT) ? (UByte) 0 : tokInfo(tTag).isMaybeInfix;

		op = " == ";
	}
	else if (packed) {
		str = "";
		op = " +->* ";
	}
	else {
		str = "";
		op = " +-> ";
	}

	if (mInf && abIsNotNothing(ab->abLambda.rtype))
		f = f && abPPPutc(buf, '(', pmaxchars);

	if (! abHasTag(param, AB_Comma))
		comsgError(param, ALDOR_E_ChkBadParams);

	if (mInf && (abArgc(param) == 2)) {
		int	p = getPrecedence(label);

		f = f && abPPParens(p, buf, abArgv(param)[0], pmaxchars)
		      && abPPPutc(buf, ' ', pmaxchars)
		      && abPPPuts(buf, str, pmaxchars)
		      && abPPPutc(buf, ' ', pmaxchars)
		      && abPPParens(p, buf, abArgv(param)[1], pmaxchars);
	}
	else {
		Bool	saveUsePiles = usePiles;
		Bool	brace	     = label && abIsTheId(label,ssymBrace);
		Bool	bracket	     = label && abIsTheId(label,ssymBracket);

		usePiles = false;

		if (bracket || brace)
			f = f && abPPPutc(buf, bracket ? '[' : '{', pmaxchars)
			      && abPPClipped0(buf, param, pmaxchars)
			      && abPPPutc(buf, bracket ? ']' : '}', pmaxchars);
		else
			f = f && abPPPuts(buf, str, pmaxchars)
			      && abPPParens(MAX_PREC, buf, param, pmaxchars);

		usePiles = saveUsePiles;
	}

	if (abIsNotNothing(ab->abLambda.rtype)) {
		Bool isWith = abHasTag(ab->abLambda.rtype, AB_With);

		if (mInf)
			f = f && abPPPuts(buf, ")", pmaxchars);
		f = f && abPPPuts(buf, ": ", pmaxchars)
		      && abPPClipped0(buf, ab->abLambda.rtype, pmaxchars);
		if (isWith) {
			    f = f && abPPIndent(buf, pmaxchars)
				  && abPPPuts(buf, HALFINDENTATION, pmaxchars);
		}
	}

	f = f && abPPPuts(buf, op, pmaxchars)
	      && abPPClipped0(buf, body, pmaxchars);

	return f;
}

local Bool
abPPIterators(Buffer buf, int iterc, AbSyn *iterv, long *pmaxchars)
{
	int	i;
	Bool	f = true;
	for (i = 0; i < iterc; i++) {
		if (i > 0)
			f = f && abPPPutc(buf, ' ', pmaxchars);
		f = f && abPPClipped0(buf, iterv[i], pmaxchars);
	}
	return f;
}

local Bool
abPPSequence(Buffer buf, AbSyn ab, long *pmaxchars)
{
	/*
	 * Handle sequences, only producing piles is usePiles == true.
	 */

	int	i;
	Bool	f = true;

	if (!usePiles)
		f = f && abPPPutc(buf, '(', pmaxchars);

	sequenceLevel++;

	for (i = 0; f && i < ab->abHdr.argc; i++) {
		Bool seqInSeq = false;

		if (abIsNothing(abArgv(ab)[i]))
			continue;

		if (usePiles) {
			f = f && abPPIndent(buf, pmaxchars);

			/* handle sequences within sequences */

			if (abHasTag(abArgv(ab)[i], AB_Sequence)) {
				seqInSeq = true;
				f = f && abPPPutc(buf, '{', pmaxchars);
			}
		}

		f = f && abPPClipped0(buf, abArgv(ab)[i], pmaxchars);

		if (usePiles && seqInSeq)
			f = f && abPPIndent(buf, pmaxchars)
			      && abPPPuts(buf, HALFINDENTATION, pmaxchars)
			      && abPPPutc(buf, '}', pmaxchars);
		if (!usePiles && (i != ab->abHdr.argc - 1))
			f = f && abPPPuts(buf, "; ", pmaxchars);
	}

	sequenceLevel--;

	if (!usePiles)
		f = f && abPPPutc(buf, ')', pmaxchars);

	return f;
}

local int
getPrecedence(AbSyn ab)
{
	/*
	 * Get operator precedence. See token.c for table.
	 */

	if (!ab)
		return 0;

	if (abHasTag(ab, AB_Id)) {
		TokenTag tTag = keyTag(symString(ab->abId.sym));

		if (tTag != TK_LIMIT)
			return tokInfo(tTag).precedence;
	}
	else if (abHasTag(ab, AB_Apply))
		return getPrecedence(abApplyOp(ab));

	return tokInfo(abInfo(abTag(ab)).tokenTag).precedence;
}

local Bool
isLeftAssociative(AbSyn ab)
{
	/*
	 * Return true if operator ab is left associative, false otherwise.
	 */

	if (!ab)
		return true;

	if (abHasTag(ab, AB_Id)) {
		TokenTag tTag = keyTag(symString(ab->abId.sym));

		if (tTag != TK_LIMIT)
			return tokInfo(tTag).isLeftAssoc;
	}
	else if (abHasTag(ab, AB_Apply))
		return isLeftAssociative(abApplyOp(ab));

	return tokInfo(abInfo(abTag(ab)).tokenTag).isLeftAssoc;
}

local Bool
symeClipped0(Buffer buf, Syme syme, long *pmaxchars)
{
	Bool	f;		/* Fits? */
	String	str;

	assert(syme != 0);

	f = *pmaxchars > 0;

	if (! f)
		return f;

	if (!syme)
		return (f = f && abPPPuts(buf, "{* NULL *}", pmaxchars));

	str = symeInfo[symeKind(syme)].str + sizeof("SYME_") - 1;
	f = f && abPPPuts(buf, str, pmaxchars);
	f = f && abPPPuts(buf, " ", pmaxchars);
	f = f && abPPPuts(buf, symeString(syme), pmaxchars);

#ifdef PrintCircle
	if (symeMark(syme) == sstSerial)
		return f = f && abPPPuts(buf, " *}", pmaxchars);
	symeSetMark(syme, sstSerial);
#endif

	f = f && abPPPuts(buf, " : ", pmaxchars);
	f = f && abPPPuts(buf, tfPretty(symeType(syme)), pmaxchars);

	if (symeIsImport(syme)) {
		f = f && abPPPuts(buf, " from ", pmaxchars);
		f = f && abPPPuts(buf, tfPretty(symeExporter(syme)),
				  pmaxchars);
	}

	return f;
}
