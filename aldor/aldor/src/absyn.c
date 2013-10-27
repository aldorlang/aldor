/*****************************************************************************
 *
 * absyn.c: Abstract syntax construction and manipulation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "opsys.h"
#include "spesym.h"
#include "store.h"
#include "util.h"
#include "syme.h"
#include "tposs.h"
#include "doc.h"
#include "comsg.h"
#include "sexpr.h"

Bool	abDebug		= false;
#define abDEBUG		DEBUG_IF(ab)	afprintf

local int       abFormatter     (OStream stream, Pointer p);
local int       abFormatterList (OStream ostream, Pointer p);
local int       abTagFormatter  (OStream ostream, int p);
local void	abPosNodeSpan0	(AbSyn X, AbSyn *pA, AbSyn *pB);
local SrcPos	abLeafEnd	(AbSyn ab);

local void
abBugPrelude(AbSyn ab)
{
	sposPrint(osStdout, abPos(ab));
	if (abTag(ab) == AB_Id)
		printf(" Identifier %s:\n", symString(ab->abId.sym));
}

static Bool  abIsInit = false;

#undef	Complain

void
abInit(void)
{
	AbSynTag	i;
	Symbol		sym;

	sxiInit();
	for (i = AB_START; i < AB_LIMIT; i++) {
		sym = symInternConst(abInfo(i).str);
#if 0
		printf("Here1 sym->val=%s, sym->info=%x, %p\n", symString(sym), symInfo(sym), symCoInfo(sym));fflush(stdout);
#endif

		if (symCoInfo(sym) == NULL) {
#if 0
			printf("Here1.5\n"); fflush(stdout);
#endif
			symCoInfoInit(sym);
		}
#if 0
		printf("Here2\n"); fflush(stdout);
#endif
		symCoInfo(sym)->abTagVal  = i;
		abInfo(i).sxsym		  = sxiFrSymbol(sym);

		abInfo(i).hash		  = strHash(abInfo(i).str);
	}
	abIsInit = true;

	fmtRegister("AbSyn", abFormatter);
	fmtRegister("AbSynList", abFormatterList);
	fmtRegisterI("AbTag", abTagFormatter);
}

AbSyn
abNewEmpty(AbSynTag abtag, Length argc)
{
	AbSyn	ab;
	Length	i;

	if (!abIsInit) abInit();

	ab = (AbSyn) stoAlloc((int) OB_AbSyn,
			      fullsizeof(struct abGen, argc, AbSyn));

	ab->abGen.hdr.tag	= abtag;
	ab->abGen.hdr.argc	= argc;

	ab->abGen.hdr.pos	= spstackEmpty;
	ab->abGen.hdr.state	= AB_State_AbSyn;

	ab->abGen.hdr.use	= AB_Use_LIMIT;

	ab->abGen.hdr.seman	= 0;
	ab->abGen.hdr.type.poss	= 0;

	for (i = 0; i < argc; i++) ab->abGen.data.argv[i] = 0;

	return ab;
}

AbSyn
abNew(AbSynTag abtag, SrcPos pos, Length argc, ...)
{
	AbSyn	ab;
	Length	i;
	va_list argp;

	ab = abNewEmpty(abtag, argc);

	abSetPos(ab, pos);

	va_start(argp, argc);

	if (abIsSymTag(abtag))
		abLeafSym(ab) = va_arg(argp, Symbol);
	else if (abIsDocTag(abtag))
		abLeafDoc(ab) = va_arg(argp, Doc);
	else if (abIsStrTag(abtag))
		abLeafStr(ab) = strCopy(va_arg(argp, String));
	else {
		for (i = 0; i < argc; i++)
			abArgv(ab)[i] = va_arg(argp, AbSyn);
		if (argc > 0) abSetEnd(ab, abEnd(abArgv(ab)[argc-1]));
	}

	va_end(argp);

	return ab;
}

AbSyn
abNewOfToken(AbSynTag abtag, Token tok)
{
	AbSyn	 ab;

	ab = abNewEmpty(abtag, 1);

	abSetPos(ab, tok->pos);

	if (abIsSymTag(abtag)) {
		if (tokHasString(tok))
			abLeafSym(ab) = symIntern(tok->val.str);
		else
			abLeafSym(ab) = tok->val.sym;
	}
	else if (abIsStrTag(abtag)) {
		if (tokHasString(tok))
			abLeafStr(ab) = strCopy(tok->val.str);
		else
			abLeafStr(ab) = strCopy(symString(tok->val.sym));
	}
	else {
		AbSyn	ab = abNewNothing(tok->pos);
		String	msg = "cannot make this kind of absyn from a token.";
		comsgFatal(ab, ALDOR_F_Bug, msg);
		/*bug("Cannot make this kind of abstract syntax from a token.");*/
	}

	if (!tokHasString(tok)) {
		if ((sposChar(tok->pos) + strlen(symString(tok->val.sym))) < (sposChar(tok->end)))
			abSetEnd(ab, tok->end-1);
	}
	else {
		if ((sposChar(tok->pos) + strlen(tok->val.str)) < (sposChar(tok->end)))
			abSetEnd(ab, tok->end-1);
	}
	return ab;
}

AbSyn
abNewOfList(AbSynTag abtag, SrcPos pos, AbSynList args)
{
	AbSyn	ab;
	Length	i, argc;

	argc = listLength(AbSyn)(args);

	ab = abNewEmpty(abtag, argc);
	abSetPos(ab, pos);

	for (i = 0; i < argc; i++, args = cdr(args))
		abArgv(ab)[i] = car(args);
	if (argc > 0) abSetEnd(ab, abEnd(abArgv(ab)[argc-1]));

	return ab;
}

AbSyn
abNewOfOpAndList(AbSynTag abtag, SrcPos pos, AbSyn op, AbSynList args)
{
	AbSyn	ab;
	Length	i, argc;

	argc = listLength(AbSyn)(args) + 1;

	ab = abNewEmpty(abtag, argc);
	abSetPos(ab, pos);

	abArgv(ab)[0] = op;

	for (i = 1; i < argc; i++, args = cdr(args))
		abArgv(ab)[i] = car(args);
	if (argc > 0) abSetEnd(ab, abEnd(abArgv(ab)[argc-1]));

	return ab;
}

void
abFree(AbSyn ab)
{
	int	i;

	if (!ab) return;

	if (abIsSymTag(abTag(ab)))
		; /* Nothing */
	else if (abIsDocTag(abTag(ab)))
		docFree(abLeafDoc(ab));
	else if (abIsStrTag(abTag(ab)))
		strFree(abLeafStr(ab));
	else
		for (i = 0; i < abArgc(ab); i++) abFree(abArgv(ab)[i]);

	if (abTForm(ab))
		tfReleaseExpr(abTForm(ab), ab);

	if (ab->abHdr.seman)
	    stoFree((Pointer) ab->abHdr.seman);

	stoFree((Pointer) ab);
}

void
abFreeNode(AbSyn ab)
{
	if (!ab) return;

	stoFree((Pointer) ab);
}

Bool
abHasSymbol(AbSyn ab, Symbol sym)
{
	int	i;

	if (!ab) return false;

	if (abTag(ab) == AB_Id) {
		return ab->abId.sym == sym;
	}
	else if (abIsLeaf(ab)) {
		return false;
	}
	else {
		for (i = 0; i < abArgc(ab); i++)
			if (abHasSymbol(abArgv(ab)[i], sym)) return true;
		return false;
	}
}

AbSyn
abCopy(AbSyn ab)
{
	AbSyn	abnew;
	int	i;

	if (! ab)
		return ab;

	abnew = abNewEmpty(abTag(ab), abArgc(ab));

	if (abIsSymTag(abTag(ab)))
		abLeafSym(abnew) = abLeafSym(ab);
	else if (abIsDocTag(abTag(ab)))
		abLeafDoc(abnew) = docCopy(abLeafDoc(ab));
	else if (abIsStrTag(abTag(ab)))
		abLeafStr(abnew) = strCopy(abLeafStr(ab));
	else
		for (i = 0; i < abArgc(ab); i++)
			abArgv(abnew)[i] = abCopy(abArgv(ab)[i]);

	abnew->abHdr.pos = spstackCopy(ab->abHdr.pos);

	if (abHasTag(ab, AB_Id) && abComment(ab))
		abSetComment(abnew, docCopy(abComment(ab)));

	return abnew;
}

AbSyn
abReposition(AbSyn ab, SrcPos pos, SrcPos end)
{
	if (ab) {
		abSetPos(ab, pos);
		abSetEnd(ab, end);
		if (! abIsLeaf(ab)) {
			int i;
			for (i = 0; i < abArgc(ab); i++)
				abReposition(abArgv(ab)[i], pos, end);
		}
	}
	return ab;
}

AbSyn
abMarkAsMacroExpanded(AbSyn ab)
{
	if (ab) {
		abSetPos(ab, sposMacroExpanded(abPos(ab)));
		abSetEnd(ab, sposMacroExpanded(abEnd(ab)));
		if (!abIsLeaf(ab)) {
			int i;
			for (i = 0; i < abArgc(ab); i++)
				abMarkAsMacroExpanded(abArgv(ab)[i]);
		}
	}
	return ab;
}

Length
abTreeHeight(AbSyn ab)
{
	int	i, n;
	Length	hi, hn;

	if (!ab)  return 0;
	if (abIsLeaf(ab)) return 1;

	n  = abArgc(ab);
	hn = 0;

	for (i = 0; i < n; i++) {
		hi = abTreeHeight(abArgv(ab)[i]);
		if (hi > hn) hn = hi;
	}
	return hn + 1;
}

Bool
abContains(AbSyn ab, AbSyn target)
{
	if (abEqual(ab, target))
		return true;
	else if (abIsLeaf(ab)) {
		return false;
	}
	else {
		int i;
		for (i=0; i<abArgc(ab); i++) {
			if (abContains(abArgv(ab)[i], target))
				return true;
		}
		return false;
	}
}

/*
 * To find a node in a given tree:  path = abPathToNode(r,n, eql, &len, 0);
 * If the equality tester "eql" is 0, then "==" is used.
 * If len >= 0, then the node was found.
 */
AIntList
abPathToNode(AbSyn root, AbSyn node, Bool (*eql)(AbSyn, AbSyn),
	     int *plen, AIntList rpathsofar)
{
	struct AIntListCons	succell;
	int			i;
	AIntList		path;

	if ((!eql && node == root) || (eql && eql(node, root))) {
		if (sposCmp(abPos(root), abPos(node)) == 0) {
			*plen = listLength(AInt)(rpathsofar);
			return listReverse(AInt)(rpathsofar);
		}
	}

	if (!root || abIsLeaf(root)) {
		*plen = -1;
		return 0;
	}

	setcdr(&succell, rpathsofar);

	for (i = 0; i < abArgc(root); i++) {
		setcar(&succell, i);
		path = abPathToNode(abArgv(root)[i],node,eql,plen,&succell);
		if (*plen >= 0) return path;
	}

	*plen = -1;
	return 0;
}

/*
 * Find leftmost smallest subtree with positions spanning pos.
 * The psubtree argument is updated with the smallest containing tree.
 */
#define		AbContToLeft	0
#define		AbContToRight	1
#define		AbContInside	2
#define		AbContCantTell	3

local int
abContainer0(AbSyn root, SrcPos pos, AbSyn *psubtree)
{
	if (abIsLeaf(root)) {
		SrcPos	posS = abPos(root);
		SrcPos	posE = abEnd(root);

		if (psubtree) *psubtree = 0;

		if (sposIsSpecial(posS)) return AbContCantTell;

		if (psubtree) *psubtree = root;

		if (sposCmp(posE,pos) < 0) return AbContToRight;
		if (sposCmp(pos, posS) > 0) return AbContToLeft;

		return AbContInside;
	}
	else {
		int	i, n, rc;
		Bool	anyToLeft, anyToRight;

		n = abArgc(root);

		anyToLeft  = false;
		anyToRight = false;

		for (i = 0; i < n; i++) {
			rc = abContainer0(abArgv(root)[i], pos, psubtree);
			switch (rc) {
			case AbContInside:	return AbContInside;
			case AbContToLeft:	anyToLeft  = true; break;
			case AbContToRight:	anyToRight = true; break;
			}
		}
		if (anyToLeft && anyToRight) {
			if (psubtree) *psubtree = root;
			return AbContInside;
		}
		if (anyToLeft)	return AbContToLeft;
		if (anyToRight) return AbContToRight;
		return AbContCantTell;
	}
}

AbSyn
abContainer(AbSyn root, SrcPos pos)
{
	AbSyn	subtree;
	abContainer0(root, pos, &subtree);
	return subtree;
}

/*
 * Find the leftmost smallest subtree containing both a and b.
 * If eql is 0 then `==' is used as the comparison.
 */

AbSyn
abSupremum(AbSyn root, AbSyn a, AbSyn b, Bool (*eql)(AbSyn, AbSyn))
{
	int	la, lb;
	AIntList pa, pb, ta, tb;

	pa = abPathToNode(root, a, eql, &la, (AIntList) NULL);
	pb = abPathToNode(root, b, eql, &lb, (AIntList) NULL);

	if (la == -1 || lb == -1) return 0;

	for (ta = pa, tb = pb; ta && tb; ta = cdr(ta), tb = cdr(tb)) {
		if (car(ta) != car(tb))	 break;
		if (car(ta)) root = abArgv(root)[car(ta)];
	}
	listFree(AInt)(pa);
	listFree(AInt)(pb);

	return root;
}


/*
 * Print a piece of abstract syntax, limitting the number of nodes shown.
 */

local int
abPrintClipped0(FILE *fout, AbSyn ab, int *pMaxNodes)
{
	int	cc;
	Length	i;

	if (!ab) return 0;

	if (*pMaxNodes == 0) return fprintf(fout, "...");

	(*pMaxNodes)--;

	switch (abTag(ab)) {
	case AB_Nothing:
		cc = fprintf(fout, "_");
		break;

	case AB_Blank:
	case AB_IdSy:
	case AB_Id:
		cc = fprintf(fout, "%s",  symString(abLeafSym(ab)));
		break;
	case AB_DocText:
		findent += 2;
		cc = fprintf(fout, "++%s", docString(ab->abDocText.doc));
		findent -= 2;
		break;
	case AB_LitInteger:
	case AB_LitString:
	case AB_LitFloat:
		cc = fprintf(fout, "%.3s: %s",
			    abInfo(abTag(ab)).str,
			    abLeafStr(ab));
		break;
	case AB_Sequence:
		findent += 2;
		cc = fprintf(fout, "[%.4s: ", abInfo(abTag(ab)).str);
		cc += fnewline(fout);
		for (i = 0; i < ab->abHdr.argc; i++) {
			cc += abPrintClipped0(fout,
				ab->abSequence.argv[i], pMaxNodes);
			cc += fnewline(fout);
			if (*pMaxNodes == 0) {
				cc += fprintf(fout, "...");
				break;
			}
		}
		cc += fprintf(fout, "]");
		cc += fnewline(fout);
		findent -= 2;
		break;
	default:
		cc = fprintf(fout, "[%s%.4s: ",
		  ((AB_Apply == abTag(ab)) && abStab(ab)) ? "*" : "",
		  abInfo(abTag(ab)).str);
		for (i = 0; i < ab->abHdr.argc; i++) {
			cc += abPrintClipped0(fout,
				abArgv(ab)[i], pMaxNodes);
			if (i < ab->abHdr.argc-1) cc += fprintf(fout, " ");
			if (*pMaxNodes == 0) {
				cc += fprintf(fout, "...");
				break;
			}
		}
		cc += fprintf(fout, "]");
	}
	return cc;
}

int
abPrintClipped(FILE *fout, AbSyn ab, int maxNodes)
{
	return abPrintClipped0(fout, ab, &maxNodes);
}

int
abPrintClippedDb(AbSyn ab, int maxNodes)
{
	int rc = abPrintClipped0(dbOut, ab, &maxNodes);
	fnewline(dbOut);
	return rc;
}

int
abPrint(FILE *fout, AbSyn ab)
{
	return abPrintClipped(fout, ab, -1);
}

int
abPrintDb(AbSyn ab)
{
	int rc = abPrintClipped(dbOut, ab, -1);
	fnewline(dbOut);
	return rc;
}

/*
 * Syntactic comparison.
 */

Bool
abEqual(AbSyn a, AbSyn b)
{
	Length i;

	if (!a || !b) return a == b;

	if (abTag(a) != abTag(b) || abArgc(a) != abArgc(b))
		return false;

	if (abIsLeaf(a)) {
		switch (abTag(a)) {
		case AB_Id:
		case AB_IdSy:
		case AB_Blank:
			return abLeafSym(a) == abLeafSym(b);
		case AB_LitInteger:
		case AB_LitString:
		case AB_LitFloat:
			return strEqual(abLeafStr(a), abLeafStr(b));
		default:
			for (i = 0; i < abArgc(a); i++)
				if (!strEqual((String) abArgv(a)[i],
					      (String) abArgv(b)[i]))
				{
					return false;
				}
		}
	}
	else {
		for (i = 0; i < abArgc(a); i++)
		    if (!abEqual(abArgv(a)[i], abArgv(b)[i]))
				return false;
	}
	return true;
}

local AbSyn
abEqualDeclMods(AbSyn ab, Bool decls)
{
	Bool	changed = (ab != NULL);

	while (changed)
		switch (abTag(ab)) {
		case AB_Declare:
			if (decls)
				ab = ab->abDeclare.type;
			else
				changed = false;
			break;
		case AB_Qualify:
			ab = ab->abQualify.what;
			break;
		case AB_PretendTo:
			ab = ab->abPretendTo.expr;
			break;
		default:
			changed = false;
			break;
		}

	return ab;
}

local Bool
abEqualModDeclares0(AbSyn ab1, AbSyn ab2, Bool decls)
{
	if (ab1 == ab2)
		return true;

	ab1 = abEqualDeclMods(ab1, decls);
	ab2 = abEqualDeclMods(ab2, decls);

	if (abTag(ab1) != abTag(ab2) || abArgc(ab1) != abArgc(ab2))
		return false;

	else if (abIsLeaf(ab1))
		return abEqual(ab1, ab2);

	else if (abHasTag(ab1, AB_Define))
        	return abEqualModDeclares0(ab1->abDefine.lhs,
					   ab2->abDefine.lhs, decls) &&
		       abEqualModDeclares0(ab1->abDefine.rhs,
					   ab2->abDefine.rhs, false);

	else {
		Length	i;
		decls &= abHasTag(ab1, AB_Comma) || abIsAnyMap(ab1);
		for (i = 0; i < abArgc(ab1); i += 1)
			if (!abEqualModDeclares0(abArgv(ab1)[i],
						 abArgv(ab2)[i], decls))
				return false;
		return true;
	}
}

Bool
abEqualModDeclares(AbSyn ab1, AbSyn ab2)
{
	return abEqualModDeclares0(ab1, ab2, true);
}

/*
 * Hash code.
 */

#define		abHashArg(h, hi) {	\
	h ^= (h << 8);			\
	h += (hi) + 200041;		\
	h &= 0x3FFFFFFF;		\
}

Hash
abHashSefo(AbSyn ab)
{
	Hash	h = 0;
	Length	i;

	if (abHasTag(ab, AB_Declare))
		return abHashSefo(ab->abDeclare.type);
	if (abHasTag(ab, AB_Qualify))
		return abHashSefo(ab->abQualify.what);
	if (abHasTag(ab, AB_PretendTo))
		return abHashSefo(ab->abPretendTo.expr);
	if (abHasTag(ab, AB_RestrictTo))
		return abHashSefo(ab->abRestrictTo.expr);
	if (abHasTag(ab, AB_Test))
		return abHashSefo(ab->abTest.cond);

	if (abIsSymTag(abTag(ab)))
		h = strHash(symString(abLeafSym(ab)));
	else if (abIsDocTag(abTag(ab)))
		h = strHash(docString(abLeafDoc(ab)));
	else if (abIsStrTag(abTag(ab)))
		h = strHash(abLeafStr(ab));
	else if (abHasTag(ab, AB_Define)) {
		abHashArg(h, abHashSefo(ab->abDefine.lhs));
		abHashArg(h, abHashSefo(ab->abDefine.rhs));
	}
	else if (abTag(ab) == AB_Lambda) {
		abHashArg(h, abHashSefo(ab->abLambda.param));
		abHashArg(h, abHashSefo(ab->abLambda.rtype));
	}
	else
		for (i = 0; i < abArgc(ab); i++)
			abHashArg(h, abHashSefo(abArgv(ab)[i]));

	h += abInfo(abTag(ab)).hash;
	h &= 0x3FFFFFFF;
	return h;
}

Hash
abHash(AbSyn ab)
{
	Hash	h = 0;
	Length	i;

	if (abIsSymTag(abTag(ab)))
		h = strHash(symString(abLeafSym(ab)));
	else if (abIsDocTag(abTag(ab)))
		h = strHash(docString(abLeafDoc(ab)));
	else if (abIsStrTag(abTag(ab)))
		h = strHash(abLeafStr(ab));
	else if (abTag(ab) == AB_Lambda) {
		abHashArg(h, abHash(ab->abLambda.param));
		abHashArg(h, abHash(ab->abLambda.rtype));
	}
	else
		for (i = 0; i < abArgc(ab); i++)
			abHashArg(h, abHash(abArgv(ab)[i]));

	h += abInfo(abTag(ab)).hash;
	h &= 0x3FFFFFFF;
	return h;
}

Hash
abHashList(AbSynList abl)
{
	AbSyn	ab = car(abl);
	Hash	h = 0;

	if (!abHasTag(ab, AB_Apply)) abl = cdr(abl);

	for (; abl; abl = cdr(abl))
		abHashArg(h, abHash(car(abl)));

	h += abInfo(abTag(ab)).hash;
	h &= 0x3FFFFFFF;
	return h;
}

Hash
abHashModDeclares(AbSyn ab)
{
	Hash	h = 0;
	Length	i;

	if (abHasTag(ab, AB_Declare))
		return abHashModDeclares(ab->abDeclare.type);
	if (abHasTag(ab, AB_Qualify))
		return abHashModDeclares(ab->abQualify.what);

	if (abIsSymTag(abTag(ab)))
		h = strHash(symString(abLeafSym(ab)));
	else if (abIsDocTag(abTag(ab)))
		h = strHash(docString(abLeafDoc(ab)));
	else if (abIsStrTag(abTag(ab)))
		h = strHash(abLeafStr(ab));
	else if (abHasTag(ab, AB_Define)) {
		abHashArg(h, abHashModDeclares(ab->abDefine.lhs));
		abHashArg(h, abHash(ab->abDefine.rhs));
	}
	else if (abHasTag(ab, AB_Comma)) {
		for (i = 0; i < abArgc(ab); i++)
			abHashArg(h, abHashModDeclares(abArgv(ab)[i]));
	}
	else
		for (i = 0; i < abArgc(ab); i++)
			abHashArg(h, abHash(abArgv(ab)[i]));

	h += abInfo(abTag(ab)).hash;
	h &= 0x3FFFFFFF;
	return h;
}

/*
 * Destructively replace oldSym by newSym in all abIds in ab
 */

void
abSubSymbol(AbSyn ab, Symbol oldSym, Symbol newSym)
{
	if (! ab)
		;
	else if (abHasTag(ab, AB_Id)) {
	    if (ab->abId.sym == oldSym)
		    ab->abId.sym = newSym;
	}
	else if (! abIsLeaf(ab)) {
		int i;
		for (i = 0; i < abArgc(ab); i++)
			abSubSymbol(abArgv(ab)[i], oldSym, newSym);
	}
}

/*
 * Construct a new identifier from a symbol meaning.
 */
AbSyn
abFrSyme(Syme syme)
{
	AbSyn	ab = abNewId(sposNone, symeId(syme));

	abSetSyme(ab, syme);
	abState(ab) = AB_State_HasUnique;
	abTUnique(ab) = symeType(syme);

	return ab;
}

/*
 * abNew{No,In,Pre,Post,Match}fix   form [Apply f, a, b, c]
 */

AbSyn
abNewNofix(SrcPos pos, AbSyn op)
{
	return abNew(AB_Apply, pos, 1, op);
}

AbSyn
abNewInfix(SrcPos pos, AbSyn op, AbSyn a, AbSyn b)
{
	return abNew(AB_Apply, pos, 3, op, a, b);
}

AbSyn
abNewPrefix(SrcPos pos, AbSyn op, AbSyn a)
{
	AbSyn	ab;
	int	i;

	if (abTag(a) != AB_Comma) return abNew(AB_Apply, pos, 2, op, a);

	ab = abNewEmpty(AB_Apply, 1 + abArgc(a));
	abArgv(ab)[0] = op;
	for (i = 0; i < abArgc(a); i++) abArgv(ab)[i+1] = abArgv(a)[i];
	abFreeNode(a);

	abSetPos(ab, pos);
	return ab;
}

AbSyn
abNewPostfix(SrcPos pos, AbSyn op, AbSyn a)
{
	return abNewPrefix(pos, op, a);		/* All the same processing */
}

AbSyn
abNewMatchfix(SrcPos pos, AbSyn op, AbSyn a)
{
	return a ? abNewPrefix(pos, op, a) : abNewNofix(pos, op);
}

/*
 * Used as a functional argument.
 */
AbSyn
abArgf(AbSyn ab, Length i)
{
	return abArgv(ab)[i];
}

/*
 * Used as a functional argument.
 */
AbSyn
abThisArgf(AbSyn ab, Length i)
{
	assert(i == 0);
	return ab;
}

/*
 * Used as a functional argument.
 */
AbSyn
abApplyArgf(AbSyn ab, Length i)
{
	return abApplyArg(ab, i);
}

/*
 * Used as a functional argument.
 */
AbSyn
abForIterArgf(AbSyn ab, Length i)
{
	assert(i == 0);
	return ab->abFor.whole;
}

/*
 * Used as a functional argument.
 */
AbSyn
abSetArgf(AbSyn ab, Length i)
{
	AbSyn	lhs, rhs;
	Length	n;

	assert(abTag(ab) == AB_Assign);

	lhs = ab->abAssign.lhs;
	rhs = ab->abAssign.rhs;

	assert(abTag(lhs) == AB_Apply);
	n = abApplyArgc(lhs);

	/* 0 is the lhs operator */
	if (i == 0) return abApplyOp(lhs);

	/* 1..n are the lhs arg[0]...arg[n-1] */
	if (i-1 < n) return abApplyArg(lhs, i-1);

	/* n+1 is the rhs */
	if (i == n+1) return rhs;

	bugBadCase(i);
	NotReached(return 0);
}

/*
 * Allocate a new application for the arguments of the comma.
 */
AbSyn
abNewApplyOfComma(AbSyn op, AbSyn arg)
{
	Length	i, argc;
	AbSyn	*argv;
	AbSyn	ab;

	switch (abTag(arg)) {
	case AB_Nothing:
		argc = 0;
		argv = 0;
		break;
	case AB_Comma:
		argc = abArgc(arg);
		argv = abArgv(arg);
		break;
	default:
		argc = 1;
		argv = &arg;
		break;
	}

	ab = abNewEmpty(AB_Apply, argc + 1);

	if (argc > 0) {
		abSetPos(ab, abPos(argv[0]));
		abSetEnd(ab, abEnd(argv[argc-1]));
	}

	abApplyOp(ab) = op;
	for (i = 0; i < argc; i += 1)
		abApplyArg(ab, i) = abDefineeId(argv[i]);

	return ab;
}

/*
 * Allocate a new comma for the original arguments of an application.
 */
AbSyn
abNewApplyArg(AbSyn app)
{
	AbSyn	a, *v;
	int	i, n;

	assert(app && abTag(app) == AB_Apply);

	n = abApplyArgc(app);
	v = abApplyArgv(app);

	if (n == 1 && v[0] && abTag(v[0]) == AB_Comma) {
		n = abArgc(v[0]);
		v = abArgv(v[0]);
	}

	a	 = abNewEmpty(AB_Comma, n);
	abSetPos(a, ((n == 0) ? abPos(app) : abPos(v[0])));

	for (i = 0; i < n; i++) abArgv(a)[i] = v[i];
	abSetEnd(a, ((n == 0) ? abEnd(app) : abEnd(v[n-1])));

	return a;
}

/*
 * Allocate a new comma for the original arguments of an application.
 * Args that are ids have declarations created.
 */
AbSyn
abNewApplyDeclaredArg(AbSyn app)
{
      AbSyn   a, *v;
      int     i, n;

      assert(app && abTag(app) == AB_Apply);

      n = abApplyArgc(app);
      v = abApplyArgv(app);

      if (n == 1 && v[0] && abTag(v[0]) == AB_Comma) {
	      n = abArgc(v[0]);
	      v = abArgv(v[0]);
      }

      a	       = abNewEmpty(AB_Comma, n);
      abSetPos(a, ((n == 0) ? abPos(app) : abPos(v[0])));

      for (i = 0; i < n; i++)
	   abArgv(a)[i] = abHasTag(v[i], AB_Id)
		  ? abNewDeclare(abPos(v[i]),v[i], abNewNothing(abPos(v[i])))
		  : v[i];
      abSetEnd(a, ((n == 0) ? abEnd(app) : abEnd(v[n-1])));

      return a;
}

/*
 * Allocate a new comma for copied arguments of an application.
 */
AbSyn
abCopyApplyArg(AbSyn ab)
{
	int	i, n;
	ab = abNewApplyArg(ab);
	n  = abApplyArgc(ab);
	for (i = 0; i < n; i++) abArgv(ab)[i] = abCopy(abArgv(ab)[i]);
	return ab;
}

/*
 * Return a singleton, otherwise alloc node and fill.
 */
AbSyn
abOneOrNewOfList(AbSynTag tag, AbSynList l)
{
	AbSyn	ab;
	Length	n, n0;

	if (l && !cdr(l)) return car(l);

	n0 = n	= listLength(AbSyn)(l);
	ab = abNewEmpty(tag, n);

	for ( ; l; l = cdr(l) )
		abArgv(ab)[--n] = car(l);
	if (n0 > 0) {
		abSetPos(ab, abPos(abArgv(ab)[0]));
		abSetEnd(ab, abEnd(abArgv(ab)[n0-1]));
	}

	return ab;
}

/*
 * Allocate a new abDocText node from a list of doc tokens.
 */
AbSyn
abNewDocTextOfList(TokenList tl)
{
	return (tl ? abNewDocText(car(tl)->pos, docNewFrList(tl)) : 0);
}

/*
 * Find the nodes with the minimum and maximum positions.
 * In case of ties, the node closer to the frontier is selected
 */
local void
abPosNodeSpan0(AbSyn X, AbSyn *pA, AbSyn *pB)
{
	Length	i, n;
	SrcPos	sposA, sposB, sposX, sposE;

	sposX = abPos(X);
	sposE = abEnd(X);
	sposA = abPos(*pA);
	sposB = abEnd(*pB);

	if (!sposIsSpecial(sposX))
		if (sposIsSpecial(sposA) || sposCmp(sposX,sposA)<=0) *pA = X;
	if (!sposIsSpecial(sposE))
		if (sposIsSpecial(sposB) || sposCmp(sposE,sposB)>=0) *pB = X;

	if (!abIsLeaf(X)) {
		n = abArgc(X);
		for (i = 0; i < n; i++) abPosNodeSpan0(abArgv(X)[i], pA, pB);
	}
}

SrcPos
abEnd(AbSyn ab)
{
	SrcPos	sp;

	if (abInfo(abTag(ab)).tokenTag < TK_GEN_LIMIT)
		sp = abLeafEnd(ab);
	else
		sp = spstackFirst(spstackRest(ab->abHdr.pos));
	return sp;
}

void
abSetEnd(AbSyn ab, SrcPos sp)
{
	ab->abHdr.pos = spstackSetSecond(ab->abHdr.pos, sp);
}

/*
 * Find the end position of a leaf node.
 *!! This is approximate, assuming no escapes.
 */
local SrcPos
abLeafEnd(AbSyn ab)
{
	int	off;

	if (sposChar(spstackFirst(spstackRest(ab->abHdr.pos))) > 0)
		return spstackFirst(spstackRest(ab->abHdr.pos));

	switch (abTag(ab)) {
	case AB_Blank:
		off = strlen(symString(ab->abBlank.sym)); break;
	case AB_Id:
		off = strlen(symString(ab->abId.sym)); break;
	case AB_IdSy:
		off = strlen(symString(ab->abIdSy.sym)); break;
	case AB_DocText:
		/*!! Assuming single line ++ */
		off = docLength(ab->abDocText.doc); break;
	case AB_LitInteger:
		off = strlen(ab->abLitInteger.str); break;
	case AB_LitFloat:
		off = strlen(ab->abLitFloat.str); break;
	case AB_LitString:
		/* Add 2 for quotes. */
		off = strlen(ab->abLitString.str) + 2; break;
	default:
		off = 0;
	}
	return sposOffset(abPos(ab), off-1);
}

void
abPosNodeSpan(AbSyn ab, AbSyn *pMinNode, AbSyn *pMaxNode)
{
	AbSyn	A, B;

	A = ab;
	B = ab;

	abPosNodeSpan0(ab, &A, &B);

	if (pMinNode) *pMinNode = A;
	if (pMaxNode) *pMaxNode = B;
}

void
abPosSpan(AbSyn ab, SrcPos *pmin, SrcPos *pmax)
{
	AbSyn	A, B;

	if (!ab) return;

	A = ab;
	B = ab;

	abPosNodeSpan0(ab, &A, &B);

	if (pmin) *pmin = abPos(A);
	if (pmax) *pmax = abEnd(B);
}

/******************************************************************************
 *
 * :: AbSyn/SExpr conversion
 *
 *****************************************************************************/

SExpr
abToSExpr(AbSyn ab)
{
	Length	ai;
	SExpr	sx;

	if (!abIsInit) abInit();

	if (!ab) return sxNil;

	switch (abTag(ab)) {
	  case AB_Nothing:
		sx = sxNil;
		break;
	  case AB_Blank:
	  case AB_IdSy:
		sx  = sxiList(2,
			abInfo(abTag(ab)).sxsym,
			sxiFrSymbol(ab->abId.sym));
		break;
	  case AB_Id:
		sx  = sxiFrSymbol(ab->abId.sym);
		break;
	  case AB_DocText:
		sx  = sxiList(2,
			abInfo(abTag(ab)).sxsym,
			sxiFrString(docString(ab->abDocText.doc))
		);
		break;
	  case AB_LitInteger:
	  case AB_LitString:
	  case AB_LitFloat:
		sx  = sxiList(2,
			abInfo(abTag(ab)).sxsym,
			sxiFrString(ab->abLitString.str)
		);
		break;
	  case AB_Declare: {
		Syme	syme = abSyme(ab->abDeclare.id);

		sx  = sxCons(abInfo(abTag(ab)).sxsym, sxNil);
		for (ai = 0; ai < abArgc(ab); ai++)
			sx = sxCons(abToSExpr(abArgv(ab)[ai]), sx);
		if (syme)
		  sx = sxCons(symeSExprAList(syme), sx);

		sx = sxNReverse(sx);
		break;
		}
	  default:
		sx  = sxCons(abInfo(abTag(ab)).sxsym, sxNil);
		for (ai = 0; ai < abArgc(ab); ai++)
			sx = sxCons(abToSExpr(abArgv(ab)[ai]), sx);
		sx = sxNReverse(sx);
	}

	sx = sxiRepos(abPos(ab), sx);
	return sx;
}

#define croak(sx,msg)	comsgFatal(abNewNothing(sxiPos(sx)), msg)

AbSyn
abFrSExpr(SExpr sx)
{
	AbSyn	ab;
	Symbol	op;
	int	tag;
	Length	argc, ai;
	SExpr	ss;

	if (!abIsInit)	  abInit();

	/*
	 * Special cases
	 */
	if (sxiNull(sx)) {
		ab = abNewNothing(sxiPos(sx));
		return ab;
	}
	if (sxiSymbolP(sx)) {
		ab = abNewId(sxiPos(sx), sxiToSymbol(sx));
		return ab;
	}

	/*
	 * General case
	 */
	if (!sxiConsP(sx)) 
		croak(sx, ALDOR_F_LoadNotList);
	if (!sxiSymbolP(ss = sxCar(sx)) || !symCoInfo(sxiToSymbol(ss))) 
		croak(ss, ALDOR_F_LoadNotAbSyn);

	op   = sxiToSymbol(ss);
	argc = sxiLength(sx) - 1;		/* -1 for tag */
	tag  = symCoInfo(op)->abTagVal;

	switch (tag) {
	  case AB_Blank:
	  case AB_IdSy:
		if (argc != 1)	croak(sx, ALDOR_F_LoadNotUnary);
		if (!sxiSymbolP(ss = sxSecond(sx))) croak(ss, ALDOR_F_LoadNotSymbol);
		ab = abNew(tag, sposNone, 1, sxiToSymbol(ss));
		break;
	  case AB_DocText:
		ab = abNewDocText(sposNone,
				  docNewFrString(sxiToString(sxSecond(sx))));
		break;
	  case AB_LitInteger:
	  case AB_LitString:
	  case AB_LitFloat:
		if (argc != 1)	croak(sx, ALDOR_F_LoadNotUnary);
		if (!sxiStringP(ss = sxSecond(sx))) croak(ss, ALDOR_F_LoadNotString);
		ab = abNew(tag, sposNone, 1, sxiToString(ss));
		break;
	  default:
		ab = abNewEmpty(tag, argc);
		for (ai = 0, sx = sxCdr(sx); ai < argc; ai++, sx = sxCdr(sx))
			abArgv(ab)[ai] = abFrSExpr(sxCar(sx));
		break;
	}

	abSetPos(ab, sxiPos(sx));
	return ab;
}


int
abWrSExpr(FILE *file, AbSyn ab, ULong sxioMode)
{
	SExpr	sx;
	int	cc;

	sx = abToSExpr(ab);
	cc = sxiWrite(file, sx, SXRW_MixedCase | sxioMode);
	sxiFree(sx);

	return cc;
}

AbSyn
abRdSExpr(FILE *file, FileName *pfn, int *plno)
{
	SExpr	sx;
	AbSyn	ab;

	sx = sxiRead(file, pfn, plno, sxNil, SXRW_MixedCase | SXRW_SrcPos);
	ab = abFrSExpr(sx);
	sxiFree(sx);

	return ab;
}

int 
abOStreamPrint(OStream ostream, AbSyn absyn)
{
	/* Ideally, we'd avoid going through the buffer, but sxiWrite is
	 * written vs a buffer, and there's no point changing it for debug functionality
	 */
	SExpr sx = abToSExpr(absyn);
	Buffer b = bufNew();
	int c;

	sxiToBufferFormatted(b, sx, SXRW_MixedCase);
	c = ostreamWrite(ostream, bufLiberate(b), -1);
	sxiFree(sx);

	return c;

}


/*****************************************************************************
 *
 * :: Writing uninterpreted forms
 *
 ****************************************************************************/

int
abToBuffer(Buffer buf, AbSyn ab)
{
	Length		start = bufPosition(buf);
	AbSynTag	tag = abTag(ab);
	UShort		i, argc;

	bufPutByte(buf, tag);

	switch (tag) {
	case AB_Nothing:
	case AB_Blank:
		break;

	case AB_IdSy:
	case AB_Id:
		bufWrString(buf, symString(abIdSym(ab)));
		break;

	case AB_LitInteger:
	case AB_LitString:
	case AB_LitFloat:
		bufWrString(buf, abLeafStr(ab));
		break;

	default:
		argc = abArgc(ab);
		bufPutHInt(buf, argc);
		for (i = 0; i < argc; i += 1)
			abToBuffer(buf, abArgv(ab)[i]);
	}

	return bufPosition(buf) - start;
}

AbSyn
abFrBuffer(Buffer buf)
{	
	AbSyn	 ab;
	AbSynTag tag;
	int	 i, argc;
	tag = bufGetByte(buf);
	switch (tag) {
	case AB_Nothing:
		ab = abNewNothing(sposNone);
		break;
	case AB_Blank:
		ab = abNewBlank(sposNone, ssymVariable);
		break;
	case AB_IdSy:
	case AB_Id:
		ab = abNewId(sposNone, symIntern(bufRdString(buf)));
		break;
	case AB_LitInteger:
		ab = abNewLitInteger(sposNone, bufRdString(buf));
		break;
	case AB_LitString:
		ab = abNewLitString(sposNone, bufRdString(buf));
		break;
	case AB_LitFloat:
		ab = abNewLitFloat(sposNone, bufRdString(buf));
		break;
	default:
		argc = bufGetHInt(buf);
		ab = abNewEmpty(tag, argc);
		for (i = 0; i < argc; i += 1)
			abArgv(ab)[i] = abFrBuffer(buf);
		break;
	}
	return ab;
}

/*****************************************************************************
 *
 * :: Information associated with each abstract syntax tag
 *
 ****************************************************************************/

/*
 * These must have the same order as the elements of the enumeration.
 */

struct ab_info abInfoTable[] = {
	{AB_Id,		 0, 0,	"Id",           TK_Id       },
	{AB_IdSy,	 0, 0,	"IdSy",         TK_Id       },
	{AB_Blank,	 0, 0,	"Blank",        TK_LIMIT    },
	{AB_DocText,	 0, 0,	"DocText",      TK_PostDoc  },
	{AB_LitInteger,	 0, 0,	"LitInteger",   TK_Int      },
	{AB_LitFloat,	 0, 0,	"LitFloat",     TK_Float    },
	{AB_LitString,	 0, 0,	"LitString",    TK_String   },
	{AB_Add,	 0, 0,	"Add",          KW_Add      },
	{AB_And,	 0, 0,	"And",          KW_And      },
	{AB_Apply,	 0, 0,	"Apply",        KW_Juxtapose},
	{AB_Assert,	 0, 0,	"Assert",       KW_Assert   },
	{AB_Assign,	 0, 0,	"Assign",       KW_Assign   },
	{AB_Break,	 0, 0,	"Break",        KW_Break    },
	{AB_Builtin,	 0, 0,	"Builtin",      TK_LIMIT    },
	{AB_CoerceTo,	 0, 0,	"CoerceTo",     KW_2Colon   },
	{AB_Collect,	 0, 0,	"Collect",      TK_LIMIT    },
	{AB_Comma,	 0, 0,	"Comma",        TK_LIMIT    },
	{AB_Declare,	 0, 0,	"Declare",      KW_Colon    },
	{AB_Default,	 0, 0,	"Default",      KW_Default  },
	{AB_Define,	 0, 0,	"Define",       KW_2EQ      },
	{AB_DDefine,	 0, 0,	"DDefine",      KW_Define   },
	{AB_Delay,	 0, 0,	"Delay",        KW_Delay    },
	{AB_Do,		 0, 0,	"Do",           KW_Do       },
	{AB_Documented,	 0, 0,	"Documented",   TK_LIMIT    },
	{AB_Except,	 0, 0,	"Except",       KW_Throw    },
	{AB_Exit,	 0, 0,	"Exit",         KW_Implies  },
	{AB_Export,	 0, 0,	"Export",       KW_Export   },
	{AB_Extend,	 0, 0,	"Extend",       KW_Extend   },
	{AB_Fix,	 0, 0,	"Fix",          KW_Fix      },
	{AB_Fluid,	 0, 0,	"Fluid",        KW_Fluid    },
	{AB_For,	 0, 0,	"For",          KW_For      },
	{AB_Foreign,	 0, 0,	"Foreign",      TK_LIMIT    },
	{AB_Free,	 0, 0,	"Free",         KW_Free     },
	{AB_Generate,	 0, 0,	"Generate",     KW_Generate },
	{AB_Goto,	 0, 0,	"Goto",         KW_Goto     },
	{AB_Has,	 0, 0,	"Has",          KW_Has      },
	{AB_Hide,	 0, 0,	"Hide",         KW_ColonStar},
	{AB_If,		 0, 0,	"If",           KW_If       },
	{AB_Import,	 0, 0,	"Import",       KW_Import   },
	{AB_Inline,	 0, 0,	"Inline",       KW_Inline   },
	{AB_Iterate,	 0, 0,	"Iterate",      KW_Iterate  },
	{AB_Label,	 0, 0,	"Label",        KW_At       },
	{AB_Lambda,	 0, 0,	"Lambda",       TK_LIMIT    },
	{AB_Let,	 0, 0,	"Let",          KW_Let      },
	{AB_Local,	 0, 0,	"Local",        KW_Local    },
	{AB_Macro,	 0, 0,	"Macro",        KW_Macro    },
	{AB_MDefine,     0, 0,	"MDefine",      KW_MArrow   },
	{AB_MLambda,     0, 0,	"MLambda",      KW_Macro    },
	{AB_Never,	 0, 0,	"Never",        KW_Never    },
	{AB_Not,	 0, 0,	"Not",          KW_Not      },
	{AB_Nothing,	 0, 0,	"Nothing",      TK_LIMIT    },
	{AB_Or,		 0, 0,	"Or",           KW_Or       },
	{AB_Paren,	 0, 0,	"Paren",        KW_OParen   },
	{AB_PLambda,     0, 0,	"PLambda",      TK_LIMIT    },
	{AB_PretendTo,	 0, 0,	"PretendTo",    KW_Pretend  },
	{AB_Qualify,	 0, 0,	"Qualify",      KW_Dollar   },
	{AB_Quote,	 0, 0,	"Quote",        KW_Quote    },
	{AB_Raise,	 0, 0,	"Raise",        KW_Throw    },
	{AB_Reference,	 0, 0,	"Reference",    KW_Reference },
	{AB_Repeat,	 0, 0,	"Repeat",       KW_Repeat   },
	{AB_RestrictTo,	 0, 0,	"RestrictTo",   KW_At       },
	{AB_Return,	 0, 0,	"Return",       KW_Return   },
	{AB_Select,	 0, 0,	"Select",       KW_Select   },
	{AB_Sequence,	 0, 0,	"Sequence",     TK_LIMIT    },
	{AB_Test,	 0, 0,	"Test",         TK_LIMIT    },
	{AB_Try,	 0, 0,	"Try",          KW_Try      },
	{AB_Unit,	 0, 0,	"Unit",         TK_LIMIT    },
	{AB_Where,	 0, 0,	"Where",        KW_Where    },
	{AB_While,	 0, 0,	"While",        KW_While    },
	{AB_With,	 0, 0,	"With",         KW_With     },
	{AB_Yield,	 0, 0,	"Yield",        KW_Yield    },

	{AB_LIMIT,	 0, 0,	"LIMIT",        TK_LIMIT    }
};

local int
abTagFormatter(OStream ostream, int p)
{
	int tag = (int) p;
	if (tag < 0 || tag >= AB_LIMIT)	{
		return ostreamPrintf(ostream, "AbTag[%d]", tag);
	}
	else {
		return ostreamPrintf(ostream, "AbTag[%s]", abInfo(tag).str);
	}
}


/*
 * Equality preserving functions for abTransferSemantics.
 */

local Sefo
abEqualMods(Sefo sefo)
{
	Bool	changed = (sefo != NULL);

	while (changed)
		switch (abTag(sefo)) {
		case AB_Hide:
			sefo = sefo->abHide.type;
			break;
		case AB_Define:
			sefo = sefo->abDefine.lhs;
			break;
		case AB_PretendTo:
			sefo = sefo->abPretendTo.expr;
			break;
		case AB_RestrictTo:
			sefo = sefo->abRestrictTo.expr;
			break;
		case AB_Qualify:
			sefo = sefo->abQualify.what;
			break;
		case AB_Declare:
			if (abIsNothing(sefo->abDeclare.id) 
			    || abTag(sefo->abDeclare.id) == AB_Label)
				sefo = sefo->abDeclare.type;
			else
				changed = false;
			break;
		case AB_Comma:
			if (abArgc(sefo) == 1)
				sefo = abArgv(sefo)[0];
			else
				changed = false;
			break;
		  case AB_Label:
			sefo = sefo->abLabel.expr;
			break;
		default:
			changed = false;
			break;
		}

	return sefo;
}

/*
 * from and to should be two absyns that have exactly the same
 * structure. We transfer the sematics information from 'from'
 * to 'to'.
 */
void
abTransferSemantics(AbSyn from, AbSyn to)
{
	if (abState(to) > abState(from))
		return;

	if (from == to)
		return;

	if (abHasTag(to, AB_Blank))
		return;

	if (abTag(from) != abTag(to)) {
		from = abEqualMods(from);
		to   = abEqualMods(to);
	}

	if (DEBUG(ab)) {
		if (abTag(from) != abTag(to)) {
			fprintf(dbOut,"'from' absyn = ");
			abPrint(dbOut, from);
			fnewline(dbOut);
			fprintf(dbOut,"'to' absyn = ");
			abPrint(dbOut, to);
			fnewline(dbOut);
		}
	}

	abUse(to) = abUse(from);

	if (from->abHdr.seman) {
		abSetComment(to, abComment(from));
		abSetStab(to, abStab(from));
		abSetSyme(to, abSyme(from));
		abSetTForm(to, abTForm(from));
		abSetImplicit(to, abImplicit(from));
		abSetTContext(to, abTContext(from));
		abSetDefineIdx(to, abDefineIdx(from));
	}

	switch (abState(from)) {
	case AB_State_HasPoss:
		abState(to) = abState(from);
		abTPoss(to) = tpossRefer(abTPoss(from));
		break;

	case AB_State_HasUnique:
		abState(to) = abState(from);
		abTUnique(to) = abTUnique(from);
		break;

	default:
		break;
	}

	if (abIsNothing(from) || abIsNothing(to))
		return;

	assert(abTag(from) == abTag(to));

	if (!abIsLeaf(from)) {
		int i, n = abArgc(from);
		assert(n == abArgc(to));

		for (i = 0; i < n; i += 1)
			abTransferSemantics(abArgv(from)[i], abArgv(to)[i]);
	}
}

AbSeman
abNewSemantics(void)
{
	AbSeman as;
	as		= (AbSeman) stoAlloc((int) OB_Other, sizeof(*as));
	as->comment	= 0;
	as->stab	= 0;
	as->syme	= 0;
	as->tform	= 0;
	as->implicit	= 0;
	as->embed	= 0;
	as->defnIdx     = -1;
	as->impl	= NULL;

	return as;
}

Doc
abSetComment(AbSyn ab, Doc comment)
{
	Doc new = NULL;
	assert(ab);
	if (! ab->abHdr.seman)
		ab->abHdr.seman = abNewSemantics();
	if (comment) {
		new = comment;
	}
	ab->abHdr.seman->comment = new;
	return new;
}

Stab
abSetStab(AbSyn ab, Stab stab)
{
	assert(ab);
	if (! ab->abHdr.seman)
		ab->abHdr.seman = abNewSemantics();
	ab->abHdr.seman->stab = stab;
	return stab;
}

Syme
abSetSyme(AbSyn ab, Syme syme)
{
	/* scobind may hand this a 0 ab */
	if (ab) {
		if (! ab->abHdr.seman)
			ab->abHdr.seman = abNewSemantics();
		ab->abHdr.seman->syme = syme;
	}
	return syme;
}

void
abSetDefineIdx(AbSyn ab, int idx)
{
	/* scobind may hand this a 0 ab */
	if (ab) {
		if (! ab->abHdr.seman)
			ab->abHdr.seman = abNewSemantics();
		ab->abHdr.seman->defnIdx = idx;
	}
}
void
abSetImpl(AbSyn ab, SImpl impl)
{
	if (impl == NULL && !ab->abHdr.seman)
		return;

	if (! ab->abHdr.seman)
		ab->abHdr.seman = abNewSemantics();

	ab->abHdr.seman->impl = impl;
}

AbSyn
abSetImplicit(AbSyn ab, AbSyn imp)
{
	assert(ab);
	if (! ab->abHdr.seman)
		ab->abHdr.seman = abNewSemantics();
	ab->abHdr.seman->implicit = imp;
	return imp;
}

TForm
abSetTForm(AbSyn ab, TForm tform)
{
	if (ab) {
		if (! ab->abHdr.seman)
			ab->abHdr.seman = abNewSemantics();
		ab->abHdr.seman->tform = tform;
	}
	return tform;
}

AbEmbed
abSetTContext(AbSyn ab, AbEmbed tag)
{
	if (ab) {
		if (! ab->abHdr.seman)
			ab->abHdr.seman = abNewSemantics();
		ab->abHdr.seman->embed = tag;
	}
	return tag;
}

AbEmbed
abAddTContext(AbSyn ab, AbEmbed tag)
{
	if (ab) {
		if (! ab->abHdr.seman)
			ab->abHdr.seman = abNewSemantics();
		ab->abHdr.seman->embed |= tag;
	}
	return tag;
}

TPoss
abResetTPoss(AbSyn ab, TPoss tp)
{
	switch (abState(ab)) {
	case AB_State_HasPoss:
		tpossFree(abTPoss(ab));
		break;
	case AB_State_HasUnique:
		abState(ab) = AB_State_HasPoss;
		break;
	default:
		break;
	}

	abTPoss(ab) = tp;
	return tp;
}

/*
 * Important: this function may increase the reference count
 * on the returned tposs. If you want to use this function for
 * debugging purposes you must call tpossFree when you have
 * finished with it otherwise it will hang around forever.
 */
TPoss
abReferTPoss(AbSyn ab)
{
	TPoss	result;

	switch (abState(ab)) {
	case AB_State_HasPoss:
		result = tpossRefer(abTPoss(ab));
		break;
	case AB_State_HasUnique:
		result = tpossSingleton(abTUnique(ab));
		break;
	default:
		result = tpossEmpty();
		break;
	}

	return result;
}

/*
 * Find the identifier in a Declare/Define/Assign. Should not be
 * invoked with a multi ...
 */

AbSyn
abDefineeId(AbSyn ab)
{
	AbSyn	r;

	r = abDefineeIdOrElse(ab, NULL);
	if (!r) {
		abBugPrelude(ab);
		bugBadCase(abTag(ab));
	}
	return r;
}


AbSyn
abDefineeIdOrElse(AbSyn ab, AbSyn failed)
{
	if (DEBUG(ab)) {
		fprintf(dbOut, "abDefineeIdOrElse: ");
		abPrint(dbOut, ab);
		fnewline(dbOut);
	}
	while (abTag(ab) != AB_Id)
		switch (abTag(ab)) {
		case AB_Assign:
			ab = ab->abAssign.lhs;
			break;
		case AB_Define:
			ab = ab->abDefine.lhs;
			break;
		case AB_Documented:
			ab = ab->abDocumented.expr;
			break;
		case AB_Declare:
			ab = ab->abDeclare.id;
			break;
		case AB_For:
			ab = ab->abFor.lhs;
			break;
		case AB_Local:
		case AB_Free:
		case AB_Sequence:
		case AB_Comma:
			if (abArgc(ab) < 1)
				return failed;
			if (abArgc(ab) > 1)
				bugWarning("abDefineeId comma bug");
			ab = abArgv(ab)[0];
			break;
		case AB_Id:
			break;
		default:
			return failed;
		}

	return ab;
}

/*
 * Find the type in a Declare/Define/Assign.
 */

AbSyn
abDefineeType(AbSyn ab)
{
	AbSyn	r;

	r = abDefineeTypeOrElse(ab, NULL);
	if (!r) {
		abBugPrelude(ab);
		bugBadCase(abTag(ab));
	}
	return r;
}

AbSyn
abDefineeTypeOrElse(AbSyn ab, AbSyn failed)
{
	if (DEBUG(ab)) {
		fprintf(dbOut, "abDefineeTypeOrElse: ");
		abPrint(dbOut, ab);
		fnewline(dbOut);
	}
	while (abTag(ab) != AB_Declare)
		switch (abTag(ab)) {
		case AB_Assign:
			ab = ab->abAssign.lhs;
			break;
		case AB_Define:
			ab = ab->abDefine.lhs;
			break;
		case AB_Documented:
			ab = ab->abDocumented.expr;
			break;
		default:
			return failed;
		}

	return ab->abDeclare.type;
}

/*****************************************************************************
 *
 * :: Debugging facilities
 *
 ****************************************************************************/

void
abDumpPosition(AbSyn ab)
{
	/* Display the source code where `ab' came from */
	if (ab)
	{
		spstackPrintDb(ab->abHdr.pos);
		fnewline(dbOut);
	}
}

local int
abFormatter(OStream ostream, Pointer p)
{
	AbSyn absyn = (AbSyn) p;
	return abOStreamPrint(ostream, absyn);
}

local int
abFormatterList(OStream ostream, Pointer p)
{
	AbSynList list = (AbSynList) p;
	return listFormat(AbSyn)(ostream, "AbSyn", list);
}
