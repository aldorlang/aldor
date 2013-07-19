/*****************************************************************************
 *
 * linear.c: Bracketing of piles.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Convert newline and indentation information into bracketing.
 * The NewLine tokens are removed and SetTab, BackSet and BackTabs are added.
 *
 * The original list of tokens is modified.
 *
 * Note that most of the function header comments in this file were added
 * post-facto and may be incorrect.
 */

#include "axlphase.h"
#include "debug.h"
#include "format.h"
#include "store.h"

Bool	linDebug	= false;
#define linDEBUG(s)	DEBUG_IF(linDebug, s)

/****************************************************************************
 *
 * :: Forward decls
 *
 ****************************************************************************/

local Token	linKeyword		(Token, TokenTag);

local int	linIndentation		(TokenList);
local TokenList linUseNeededSep		(TokenList);
local TokenList linXTokens		(TokenList, TokenTag);
local TokenList linXBlankLines		(TokenList);
local TokenList linCheckBalance		(TokenList);
local TokenList linISepAfterDontPiles	(TokenList);
local TokenList linXSep			(TokenList);

# define 	linXComments(tl)	linXTokens((tl), TK_Comment)
# define 	linXNewLines(tl)	linXTokens((tl), KW_NewLine)
# define 	tokIsNonStarter(t)	(tokIsFollower(t) || tokIsCloser(t))

# define 	MootIndentation		(-1)

# define 	DoPileStart		KW_StartPile    /* #pile    */
# define 	DoPileEnd		KW_EndPile	/* #endpile */ 

# define 	DontPileStart		KW_OCurly	/* { */
# define 	DontPileSep		KW_Semicolon	/* ; */
# define 	DontPileEnd		KW_CCurly	/* } */


/****************************************************************************
 *
 * :: LNodeTree data structure.
 *
 ****************************************************************************/

/*
 * The LNodeTree structure represents in tree form the information
 * about whether to pile and, if so, line boundaries.
 *
 * The input
 *
 * {|
 *     a
 *     b
 *	  c
 *	  d { e
 *		 h } i
 *	  l {
 *		n
 *		o {| p
 *		   q
 *		   r |}
 *		t
 *	  u }
 *     v
 *     w z
 * |}
 *
 * becomes
 *
 * {| <a> <b> <c> <d { e h } i> <l { n o {| <p> <q> <r> |} t u }> <v> <w z> |}
 *
 * Note that since piling brackets {| and |} have been replaced by #pile
 * and #endpile, the example above is overly complicated and could not be
 * represented in Aldor. Also note that this structure assumes that most
 * programs will be piled whereas now they are normally non-piled. The
 * two forms do not normally appear together in the same source file.
 */


/* Note that LN_1Tok and LN_NTok could be merged */
enum lnodeKind {
	LN_1Tok,	/* a		     */
	LN_NTok,	/* <w z>	     -- Has any NewLine token.	*/
	LN_NNodes,	/* <d { e h } i>     -- Has any NewLine token.	*/
	LN_DoPile	/* {| <p> <q> <r> |} -- Has {| |} tokens.	*/
};

typedef Enum(lnodeKind)	 LNodeKind;
typedef struct lnode	*LNodeTree;

union lnodeArg {
	Token		tok;
	LNodeTree	lnode;
};

struct lnode {
	LNodeKind	kind;
	ULong		has;
	Length		indent;
	Length		argc;
	union lnodeArg	argv[NARY];
};

#define HAS_NonCom	(1<<0)
#define HAS_NonBlank	(1<<1)

#define linIsBlank(lnt) (!((lnt)->has & HAS_NonBlank))
#define linIsCom(lnt)	(!((lnt)->has & HAS_NonCom))


DECLARE_LIST(LNodeTree);
CREATE_LIST (LNodeTree);

local LNodeTree lntNewEmpty	(LNodeKind, Length);
local void	lntFree		(LNodeTree);
local void	lntFreeNode	(LNodeTree);

local ULong	lntTokHas	(TokenTag);
local Token	lntFirstTok	(LNodeTree);
local Token	lntLastTok	(LNodeTree);
local Token	lntLastTokLessNL(LNodeTree);

local LNodeTree lntTok		(Token);
local LNodeTree lntConcat	(LNodeTree, LNodeTree);
local LNodeTree lntSeparate	(LNodeTree, TokenTag,  LNodeTree);
local LNodeTree lntWrap		(TokenTag,  LNodeTree, TokenTag);

local TokenList lntToTokenList	(LNodeTree);
local TokenList lntToTokenList0 (LNodeTree, TokenList);
local TokenList lntConsNL	(TokenList);
local LNodeTree lntFrTokenList	(TokenList);

local LNodeTree lin2DRules	(LNodeTree);



local LNodeTree
lntNewEmpty(LNodeKind kind, Length argc)
{
	LNodeTree	lnt;

	lnt = (LNodeTree) stoAlloc(OB_Other,
				   fullsizeof(struct lnode,
					      argc, union lnodeArg));
	lnt->kind	= kind;
	lnt->has	= 0;
	lnt->indent	= -1;
	lnt->argc	= argc;
	return lnt;
}

local void
lntFree(LNodeTree lnt)
{
	if (!lnt) return;

	if (lnt->kind != LN_1Tok && lnt->kind != LN_NTok) {
		int	i;
		for (i = 0; i < lnt->argc; i++) lntFree(lnt->argv[i].lnode);
	}
	lntFreeNode(lnt);
}

local void
lntFreeNode(LNodeTree lnt)
{
	stoFree((Pointer) lnt);
}


local int
lntPrint(FILE *fout, LNodeTree lnt)
{
	int	cc, i, n;
	static int	d = 0;

	if (!lnt) return fprintf(fout, "|0|");

	n = lnt->argc;
	cc = 0;

	d++;
	switch (lnt->kind) {
	case LN_1Tok:
		cc  = fprintf(fout, "(: ");
		cc += tokPrint(fout, lnt->argv[0].tok);
		cc += fprintf(fout, " :)");
		break;
	case LN_NTok:
		cc = fprintf(fout, "<: ");
		for (i = 0; i < n; i++) {
			cc += fprintf(fout, " ");
			cc += tokPrint(fout, lnt->argv[i].tok);
		}
		cc += fprintf(fout, " :>");
		break;
	case LN_NNodes:
		cc = fprintf(fout, "< %d: ", d);
		for (i = 0; i < n; i++)
			cc += lntPrint(fout, lnt->argv[i].lnode);
		cc += fprintf(fout, " :%d >", d);
		break;
	case LN_DoPile:
		cc = fprintf(fout, "{ %d: ", d);
		for (i = 0; i < n; i++)
			cc += lntPrint(fout, lnt->argv[i].lnode);
		cc += fprintf(fout, " :%d }", d);
		break;
	}
	d--;
	cc += fprintf(fout, "%lx", lnt->has);

	return cc;
}

/*****************************************************************************
 *
 * :: LNodeTree utilities
 *
 ****************************************************************************/

local LNodeTree
lntTok(Token tok)
{
	LNodeTree lnt	= lntNewEmpty(LN_1Tok, 1);
	lnt->argv[0].tok= tok;
	lnt->has	= lntTokHas(tokTag(tok));
	lnt->indent	= sposChar(tok->pos);
	return lnt;
}

local LNodeTree
lntConcat(LNodeTree lnt, LNodeTree rnt)
{
	LNodeTree tnt;

	if (! lnt)
		tnt = rnt;
	else if (! rnt)
		tnt = lnt;
	else {
		tnt = lntNewEmpty(LN_NNodes, 2);
		tnt->indent	   = lnt->indent;
		tnt->has	   = lnt->has | rnt->has;
		tnt->argv[0].lnode = lnt;
		tnt->argv[1].lnode = rnt;
	}
	return tnt;
}

local LNodeTree
lntSeparate(LNodeTree lnt, TokenTag sep, LNodeTree rnt)
{
	LNodeTree tnt;
	Token	  tok  = lntLastTok(lnt);

	tnt = lntNewEmpty(LN_NNodes, 3);
	tnt->indent	   = lnt->indent;
	tnt->has	   = lnt->has | lntTokHas(sep) | rnt->has;
	tnt->argv[0].lnode = lnt;
	tnt->argv[1].lnode = lntTok(linKeyword(tok, sep));
	tnt->argv[2].lnode = rnt;

	return tnt;
}

local LNodeTree
lntWrap(TokenTag open, LNodeTree lnt, TokenTag close)
{
	LNodeTree tnt;

	Token	  otok	= lntFirstTok(lnt);
	Token	  ctok	= lntLastTok(lnt);

	tnt		   = lntNewEmpty(LN_NNodes, 3);
	tnt->indent	   = lnt->indent;
	tnt->has	   = lntTokHas(open) | lnt->has | lntTokHas(close);
	tnt->argv[0].lnode = lntTok(linKeyword(otok, open));
	tnt->argv[1].lnode = lnt;
	tnt->argv[2].lnode = lntTok(linKeyword(ctok, close));

	return tnt;
}

local ULong
lntTokHas(TokenTag tt)
{
	switch (tt) {
	case KW_NewLine: case TK_Comment: return 0;
	case TK_PostDoc: case TK_PreDoc:  return HAS_NonBlank;
	default:			  return HAS_NonBlank | HAS_NonCom;
	}
}

local Token
lntFirstTok(LNodeTree lnt)
{
	if (!lnt) return 0;

	for (;;) {
		if (lnt->argc == 0) return 0;

		switch (lnt->kind) {
		case LN_1Tok:
		case LN_NTok:
			return lnt->argv[0].tok;
		case LN_NNodes:
		case LN_DoPile:
			lnt = lnt->argv[0].lnode;
		}
	}
}

local Token
lntLastTok(LNodeTree lnt)
{
	if (!lnt) return 0;

	for (;;) {
		int n = lnt->argc;

		if (n == 0) return 0;

		switch (lnt->kind) {
		case LN_1Tok:
			return lnt->argv[0].tok;
		case LN_NTok:
			return lnt->argv[n-1].tok;
		case LN_NNodes:
		case LN_DoPile:
			lnt = lnt->argv[n-1].lnode;
		}
	}
}

local Token
lntLastTokLessNL(LNodeTree lnt)
{
	int	n;
	Token	tok;

	if (!lnt) return 0;

	switch (lnt->kind) {
	case LN_1Tok:
	case LN_NTok:
		for (n = lnt->argc; n > 0; n--) {
			tok = lnt->argv[n-1].tok;
			if (!tokIs(tok, KW_NewLine)) return tok;
		}
		return 0;
	case LN_NNodes:
	case LN_DoPile:
		for (n = lnt->argc; n > 0; n--) {
			tok = lntLastTokLessNL(lnt->argv[n-1].lnode);
			if (tok && !tokIs(tok, KW_NewLine)) return tok;
		}
		return 0;
	default:
		NotReached(return 0);
	}
}

/*****************************************************************************
 *
 * :: Convert  LNodeTree -> TokenList
 *
 ****************************************************************************/

local TokenList
lntToTokenList(LNodeTree lnt)
{
	return listNReverse(Token)(lntToTokenList0(lnt, NULL));
}

local TokenList
lntToTokenList0(LNodeTree lnt, TokenList rsofar)
{
	int	i, n;

	if (!lnt) return rsofar;

	n = lnt->argc;

	switch (lnt->kind) {
	case LN_1Tok:
		rsofar = listCons(Token)(lnt->argv[0].tok, rsofar);
		break;
	case LN_NTok:
		for (i = 0; i < n; i++)
			rsofar = listCons(Token)(lnt->argv[i].tok, rsofar);
		break;
	case LN_NNodes:
		for (i = 0; i < n; i++)
			rsofar = lntToTokenList0(lnt->argv[i].lnode, rsofar);
		break;
	case LN_DoPile:
		/* Insert newlines between each pair of args. */
		rsofar = lntToTokenList0(lnt->argv[0].lnode, rsofar);
		rsofar = lntConsNL(rsofar);
		for (i = 1; i < n-1; i++)  {
			rsofar = lntToTokenList0(lnt->argv[i].lnode, rsofar);
			rsofar = lntConsNL(rsofar);
		}
		rsofar = lntToTokenList0(lnt->argv[n-1].lnode, rsofar);
		break;
	}

	return rsofar;
}

local TokenList
lntConsNL(TokenList tl)
{
	Token	nl = linKeyword(tl ? car(tl) : 0, KW_NewLine);

	return listCons(Token)(nl, tl);
}

/*****************************************************************************
 *
 * :: Convert  TokenList -> LNodeTree
 *
 ****************************************************************************/

/*
 * Use a recursive descent parser to convert from lists to trees.
 */
local LNodeTree lntFrTL_DoPile		(TokenList *);
local LNodeTree lntFrTL_DoLine		(TokenList *);
local LNodeTree lntFrTL_DontPile	(TokenList *);
local LNodeTree lntFrTL_DontLine	(TokenList *, Bool isStacking);
local LNodeTree lntFrTL_1Tok		(TokenList *);
local LNodeTree lntFrTL_MakeLine	(LNodeTreeList,int,Length,Length);

local int depthDoPileNo;
local int depthDontPileNo;

/* Top-level entry-point for converting token lists into lnode trees */
local LNodeTree
lntFrTokenList(TokenList tl)
{
	depthDoPileNo = 0;  
	depthDontPileNo = 0; 

	return lntFrTL_DontLine(&tl, false);
}

/*
 * Convert a list of tokens for a piled section of code into a tree. If
 * the end of the stream is reached without finding a closing #endpile
 * then one is automatically added. The first child of the result tree
 * is the #pile token and the last is a #endpile token. The other nodes
 * correspond to complete lines from source code or embedded piled or
 * non-piled sections blocks.
 */
local LNodeTree
lntFrTL_DoPile(TokenList *ptl)
{
	TokenList	tl0 = *ptl;
	LNodeTreeList	ll, l;
	LNodeTree	lnt;
	Token		tk;
	int		i, n, in0;
	Bool		has;

	assert(tl0);
	assert(tokTag(car(tl0)) == DoPileStart);

	depthDoPileNo += 1;  
	in0 = linIndentation(tl0);
	ll  = listCons(LNodeTree)(lntFrTL_1Tok(&tl0), NULL);
	n   = 1;

	while (tl0 && tokTag(car(tl0)) != DoPileEnd) {
		lnt = lntFrTL_DoLine(&tl0);
		ll  = listCons(LNodeTree)(lnt, ll);
		n++;
	}
	if (tl0 && tokTag(car(tl0)) == DoPileEnd) {
		ll = listCons(LNodeTree)(lntFrTL_1Tok(&tl0), ll);
		n++;
	}
	/* Insert extra #endpile if needed. */
	else if (!tl0) {
		tk = tokKeyword(sposNone, sposNone, DoPileEnd);
		ll = listCons(LNodeTree)(lntTok(tk), ll);
		n++;
	}

	lnt = lntNewEmpty(LN_DoPile, n);
	has = 0;
	for (i = n-1, l = ll; i >= 0 ; i--, l = cdr(l)) {
		lnt->argv[i].lnode = car(l);
		has |= car(l)->has;
	}
	lnt->indent = in0;
	lnt->has    = has;

	*ptl = tl0;
	listFree(LNodeTree)(ll);
	depthDoPileNo -= 1;  
	return lnt;
}


/*
 * Convert a list of tokens for a single line of code into a tree. If a
 * #pile token is found then the embedded piled section is converted into
 * a single tree node; likewise for embedded non-piled sections. All
 * other tokens are added to the result tree as leaves. Processing stops
 * when a newline token is found, the end of the token stream is reached,
 * or when a #endpile or } token is found without a matching #pile or {.
 */
local LNodeTree
lntFrTL_DoLine(TokenList *ptl)
{
	TokenList	tl0 = *ptl;
	TokenTag	tag;
	LNodeTreeList	ll;
	LNodeTree	lnt;
	int		n, ntoks, in0;

	if (!tl0) {
		lnt = lntNewEmpty(LN_NTok, int0);
		lnt->indent = 0;
		lnt->has    = 0;
		return lnt;
	}

	in0   = linIndentation(tl0);
	ll    = 0;
	n     = 0;
	ntoks = 0;

	/* This test always succeeds and is therefore redundant */
	if (tl0) do {
		tag = tokTag(car(tl0));

		switch (tag) {
		case DoPileStart:
			lnt = lntFrTL_DoPile(&tl0);
			ll  = listCons(LNodeTree)(lnt, ll);
			n++;
			break;
		case DontPileStart:
			lnt = lntFrTL_DontPile(&tl0);
			ll  = listCons(LNodeTree)(lnt, ll);
			n++;
			break;
		case DoPileEnd:
		case DontPileEnd:
			if (tag == DoPileEnd)
#if 1
				break;	/*XXXXXXX*/
#else
				if (depthDoPileNo)
					break;
				else if (depthDontPileNo)
					break;
#endif
		default:
			lnt = lntFrTL_1Tok(&tl0);
			ll  = listCons(LNodeTree)(lnt, ll);
			n++;
			ntoks++;
			break;
		}
	} while (tag != KW_NewLine &&
		 (tag != DoPileEnd || depthDoPileNo == 0) &&  
		 (tag != DontPileEnd || depthDontPileNo == 0) && 
		 tl0);

	lnt  = lntFrTL_MakeLine(ll, in0, n, ntoks);
	*ptl = tl0;
	listFree(LNodeTree)(ll);
	return lnt;
}

/*
 * Convert a list of tokens for a non-piled section into an lnode tree.
 * For correctly balanced programs the result tree will have three child
 * nodes: the first will be a { leaf and the third will be a } leaf. The
 * second node will contain all tokens within the non-piled section with
 * the nesting of non-piled sections removed. Piled sections will have
 * their own tree nodes as normal.
 */
local LNodeTree
lntFrTL_DontPile(TokenList *ptl)
{
	TokenList	tl0 = *ptl;
	LNodeTreeList	ll, l;
	LNodeTree	lnt;
	int		i, n, in0;
	Bool		has;

	assert(tl0);
	assert(tokTag(car(tl0)) == DontPileStart);

	depthDontPileNo += 1;  
	in0   = linIndentation(tl0);
	ll    = listCons(LNodeTree)(lntFrTL_1Tok(&tl0), NULL);
	ll    = listCons(LNodeTree)(lntFrTL_DontLine(&tl0, true), ll);
	n     = 2;
	if (tl0 && tokTag(car(tl0)) == DontPileEnd) {
		ll = listCons(LNodeTree)(lntFrTL_1Tok(&tl0), ll);
		n++;
	}

	lnt = lntNewEmpty(LN_NNodes, n);
	has = 0;
	for (i = n-1, l = ll; i >= 0 ; i--, l = cdr(l)) {
		lnt->argv[i].lnode = car(l);
		has |= car(l)->has;
	}
	lnt->indent = in0;
	lnt->has    = has;

	*ptl = tl0;
	listFree(LNodeTree)(ll);
	depthDontPileNo -= 1;  
	return lnt;
}

/*
 * Convert a list of tokens in a non-piled section into a tree. If the
 * isStacking flag is false then every token is placed in the resulting
 * tree even if it is not correctly balanced. Otherwise only the tokens
 * up to the end of the non-piled section are consumed taking into account
 * the balancing of { } around non-piled sections.
 */
local LNodeTree
lntFrTL_DontLine(TokenList *ptl, Bool isStacking)
{
	TokenList	tl0 = *ptl;
	TokenTag	tag;
	LNodeTreeList	ll;
	LNodeTree	lnt;
	int		n, ntoks, in0, depth;

	if (!tl0) {
		lnt = lntNewEmpty(LN_NTok, int0);
		lnt->indent = 0;
		lnt->has    = 0;
		return lnt;
	}

	in0   = linIndentation(tl0);
	depth = 0;
	n     = 0;
	ntoks = 0;
	ll    = 0;

	while (tl0) {
		tag = tokTag(car(tl0));

		if (tag == DoPileStart) {
			lnt = lntFrTL_DoPile(&tl0);
			ll  = listCons(LNodeTree)(lnt, ll);
			n++;
		}
		else {
			if (isStacking && tag == DontPileStart) {
				depth++;
			}
			if (isStacking && tag == DontPileEnd) {
				depth--;
			}
			if (depth < 0) break;

			lnt = lntFrTL_1Tok(&tl0);
			ll  = listCons(LNodeTree)(lnt, ll);
			n++;
			ntoks++;
		}
	}

	lnt  = lntFrTL_MakeLine(ll, in0, n, ntoks);
	*ptl = tl0;
	listFree(LNodeTree)(ll);
	return lnt;
}


/* Remove a token from the token list and convert into an lnode tree leaf */
local LNodeTree
lntFrTL_1Tok(TokenList *ptl)
{
	LNodeTree	lnt;

	assert(*ptl);

	lnt  = lntTok(car(*ptl));
	*ptl = cdr(*ptl);

	return lnt;
}

/* Convert a list of tree nodes into a single n-ary tree node */
local LNodeTree
lntFrTL_MakeLine(LNodeTreeList ll, int in0, Length n, Length ntoks)
{
	int		i;
	LNodeTreeList	l;
	LNodeTree	lnt;
	Bool		has;

	has = 0;
	for (l = ll; l; l = cdr(l))
		has |= car(l)->has;

	if (n == 1) {
		lnt = car(ll);
	}
	else if (n == ntoks) {
		/*
		 * Compactify the case when all subtrees are tokens.
		 */
		lnt = lntNewEmpty(LN_NTok, n);
		lnt->indent = in0;
		lnt->has    = has;

		for (i = n-1, l = ll; i >= 0; i--, l = cdr(l)) {
			lnt->argv[i].tok = car(l)->argv[0].tok;
			lntFree(car(l));
		}
	}
	else {
		lnt = lntNewEmpty(LN_NNodes, n);
		lnt->indent = in0;
		lnt->has    = has;

		for (i = n-1, l = ll; i >= 0; i--, l = cdr(l))
			lnt->argv[i].lnode = car(l);
	}

	return lnt;
}


/****************************************************************************
 *
 * :: Verify balance of piling braces
 *
 ***************************************************************************/

# define LIN_NOT_OK	0
# define LIN_OK		1

local void
serrorUnbalanced(Token tok,Bool errorIfTrue)
{
	SrcPos   pos  = tok->pos;
#if EDIT_1_0_n2_02
        TokenTag tag  = tokTag(tok), mate = tokTag(tok);
#else
        TokenTag tag  = tokTag(tok), mate;
#endif

	switch (tag) {
	case DoPileStart:	mate = DoPileEnd; 	break;
	case DoPileEnd:		mate = DoPileStart; 	break;
	case DontPileStart:	mate = DontPileEnd; 	break;
	case DontPileEnd:	mate = DontPileStart; 	break;
	default:		comsgFatal(abNewNothing(pos), ALDOR_F_Bug,
					"unexpected unbalanced token");
	}

	if (errorIfTrue) {
		comsgError(abNewNothing(pos), ALDOR_E_LinUnbalanced,
			keyString(tag), keyString(mate));
	}
	else {
		 comsgWarning(abNewNothing(pos), ALDOR_E_LinUnbalanced,
			keyString(tag), keyString(mate));
	}
}

/*
 * Look for unmatched {,},{|,|} and give error msg.
 * It also sets tok->extra field for each token in the token list.	  
 */
local void
linCheckBalance0(TokenList *ptl, Token lastOpener, int depth)
{
	TokenTag	tag;
	Token		tok;

	linDEBUG(
	     fprintf(dbOut,"->> linCheckPile0: %d--\n",tokTag(lastOpener)););

	lastOpener->extra = LIN_OK;  

	while (*ptl) {
		tok  = car(*ptl);
		tag  = tokTag(tok);
		*ptl = cdr(*ptl);
		
		switch(tag) {
		case DoPileStart:
		case DontPileStart:
			linCheckBalance0(ptl, tok, depth+1);
			break;
		case DoPileEnd:
			if (tokTag(lastOpener) == DoPileStart) {
				tok->extra = LIN_OK;
				linDEBUG(
				  fprintf(dbOut,"-<< linCheckPile0- |} --\n"));
				return;
			}
			else {
				tok->extra = LIN_NOT_OK;
				serrorUnbalanced(tok,true);
			}
			break;
		case DontPileEnd:
			if (tokTag(lastOpener) == DontPileStart) {
				tok->extra = LIN_NOT_OK;
				linDEBUG(
				    fprintf(dbOut,"-<< linCheckPile0- } --\n"));
				return;
			}
			else {
				tok->extra = LIN_NOT_OK;
				serrorUnbalanced(tok,true);
			}
			break;
		default:
			break;
		}
	}

	/* Detect unmatched opener, but allow missing #endpile at top level.*/
	tag = tokTag(lastOpener);
	if (tag != TK_Blank && tag != DoPileStart ) { 
		lastOpener->extra = LIN_NOT_OK;
		serrorUnbalanced(lastOpener,true);
	}
	if (tag != TK_Blank && depth > 1 ) { 
		lastOpener->extra = LIN_NOT_OK;
		serrorUnbalanced(lastOpener,false);
	}
	linDEBUG(fprintf(dbOut,"-<< linCheckPile0-NULL--\n"););
}


/*
 * This function returns the original list but we should suppose that
 * it could be different -- we may later insert correction tokens.
 */
local TokenList
linCheckBalance(TokenList tl0)
{
	TokenList 	tl = tl0;
	Token		tok;

	/* Mark all nodes OK. */
	for (tl = tl0; tl; tl = cdr(tl))
		car(tl)->extra = LIN_OK;

	/* Do the balancing act. */
	tl  = tl0;
	tok = tokNew(sposNone, sposNone, TK_Blank, NULL);
	linCheckBalance0(&tl, tok, int0);
	tokFree(tok);

	return tl0;
}

/*****************************************************************************
 *
 * :: Main entry point of linearizer
 *
 ****************************************************************************/

TokenList
linearize(TokenList tl)
{
	LNodeTree lnt;

	linDEBUG({
		fprintf(dbOut,"-------------- Starting with -------------\n");
		toklistPrint(dbOut, tl);
		fnewline(dbOut);
	});

	tl  = linXComments(tl);
	tl  = linXBlankLines(tl);

	/* Add a `#pile' if in interactive mode, so that the user can use
         * TAB for an easier input.
	 */

	if (fintMode == FINT_LOOP) {
		Token tok = tokNew(sposNone, sposNone, KW_StartPile, NULL);
		listPush(Token, tok, tl);
	}

	tl  = linCheckBalance(tl);
	lnt = lntFrTokenList(tl);
	listFree(Token)(tl);
	linDEBUG({
		fprintf(dbOut,"-------------- Converted to --------------\n");
		lntPrint(dbOut, lnt);
		fnewline(dbOut);
	});

	lnt = lin2DRules(lnt);
	linDEBUG({
		fprintf(dbOut,"-------------- Converted to -------------\n");
		lntPrint(dbOut, lnt);
		fnewline(dbOut);
	});

	tl = lntToTokenList(lnt);
	tl = linXNewLines(tl);
	lntFree(lnt);
	linDEBUG({
		fprintf(dbOut,"-------------- Ending with -------------\n");
		toklistPrint(dbOut, tl);
		fnewline(dbOut);
	});

	tl = linUseNeededSep(tl);

	linDEBUG({
		fprintf(dbOut,"-------------- Leaving with ------------\n");
		toklistPrint(dbOut, tl);
		fnewline(dbOut);
	});

	return tl;
}

/* This function inserts or deletes statement separators (;) as required */
local TokenList
linUseNeededSep(TokenList tl)
{
	tl = linISepAfterDontPiles(tl);

	linDEBUG({
		fprintf(dbOut,"------- linUseNeededSep (mid) ----------\n");
		toklistPrint(dbOut, tl);
		fnewline(dbOut);
	});

	tl = linXSep(tl);

	linDEBUG({
		fprintf(dbOut,"------- linUseNeededSep (xit) ----------\n");
		toklistPrint(dbOut, tl);
		fnewline(dbOut);
	});
	return tl;
}

/*****************************************************************************
 *
 * :: TokenList Utilities
 *
 ****************************************************************************/

local Token
linKeyword(Token org, TokenTag key)
{
	SrcPos	  spos = org ? org->pos : sposNone;
	SrcPos	  epos = org ? org->end : sposNone;

	return tokKeyword(spos, epos, key);
}

/*
 * Free the specified tokens.  (Modifies the original list.)
 */
local TokenList
linXTokens(TokenList tl, TokenTag tag)
{
	struct TokenListCons	head;
	TokenList		prev;

	setcar(&head, NULL);
	setcdr(&head, tl);

	for (prev = &head; cdr(prev); ) {
		TokenList curr = cdr(prev);
		Token	  tok  = car(curr);

		if (tokTag(tok) == tag) {
			setcdr(prev, cdr(curr));
			listFreeCons(Token)(curr);
			tokFree(tok);
		}
		else
			prev = curr;
	}
	return cdr(&head);
}

/*
 * Free leading newlines and all but the first of consecutive newlines.
 * (Modifies the original list.)
 */
local TokenList
linXBlankLines0(TokenList tl)
{
	while (tl && tokTag(car(tl)) == KW_NewLine) {
		Token	  tk = car(tl);
		TokenList rl = cdr(tl);

		tokFree(tk);
		listFreeCons(Token)(tl);
		tl = rl;
	}
	return tl;
}

local TokenList
linXBlankLines(TokenList tl0)
{
	TokenList tl;

	tl0 = linXBlankLines0(tl0);

	for (tl = tl0; tl; tl = cdr(tl)) {
		Token	  tok  = car(tl);
		TokenList rest = cdr(tl);

		if (tokTag(tok) == KW_NewLine || tokTag(tok) == DoPileStart) {
			/* !!!FIXME!!! Loop executes just once? */
			while (rest && tokTag(car(rest)) == KW_NewLine)
				rest = linXBlankLines0(rest);
			setcdr(tl, rest);
		}
	}
	return tl0;
}


/*
 * Insert ";" after all "}".  (Modifies the original list.) This function
 * ought to be modified to use a two-token look-ahead. If the next token
 * is not DontPileSep and the one after is not a follower or a closer
 * then the insertion can take place. If this change is made then linXSep
 * is redundant and the compiler will make fewer strange parsing decisions.
 */
local TokenList
linISepAfterDontPiles(TokenList tl)
{
	TokenList t;
	for (t = tl; t; t = cdr(t)) {
		if (tokIs(car(t), DontPileEnd)) {
			TokenList trest = cdr(t);

			if (trest && !tokIs(car(trest), DontPileSep)) {
				Token tok = linKeyword(car(t),DontPileSep);
				trest = listCons(Token)(tok, trest);
				setcdr(t, trest);
			}
		}
	}

	return tl;
}

/*
 * Delete ";"  before non-starters. This function would be redundant if
 * linISepAfterDontPiles() used a two-token look-ahead.
 */
local TokenList
linXSep(TokenList tl)
{
	TokenList t;

	/* Delete leading string ";". */
	while (tl && tokIs(car(tl), DontPileSep))
		tl = listFreeDeeplyTo(Token)(tl,cdr(tl),tokFree);

	/* Delete ";" before non-starters. */
	for (t = tl; t; t = cdr(t)) {
		TokenList tcdr, tcddr;
		tcdr = cdr(t);
		if (!tcdr) break;
		if (!tokIs(car(tcdr), DontPileSep)) continue;
		tcddr = cdr(tcdr);
		if (!tcddr || tokIsNonStarter(car(tcddr)))
			setcdr(t, listFreeDeeplyTo(Token)(tcdr,tcddr,tokFree));
	}

	return tl;
}

/*
 * Determine the true indentation a line, taking into account labels.
 */
local int
linIndentation(TokenList tl)
{
	int	ind;

	linDEBUG({if (tl) tokPrint(dbOut, car(tl));});

	if (tl && tokIs(car(tl), KW_At)) {
		/* Skip '@' and 'id' if there. */
		tl = cdr(tl);
		if (tl) tl = cdr(tl);
	}
	ind = tl && !tokIs(car(tl), KW_NewLine)
		? sposChar(car(tl)->pos)
		: MootIndentation;

	DEBUG(fprintf(dbOut, " line indented to: %d\n", ind));

	return ind;
}

/****************************************************************************
 *
 * :: Apply piling rules.
 *
 ****************************************************************************/

local LNodeTree lin2DRulesPile	  (LNodeTree);
local LNodeTree lin2DRulesPile0	  (LNodeTree context, LNodeTree, int *, int *);

local LNodeTree joinUp		  (LNodeTree context, LNodeTreeList);
local Bool	isBackSetRequired (LNodeTree context, LNodeTree, LNodeTree);
local Bool	isPileRequired	  (LNodeTree context, LNodeTree);

local LNodeTree
lin2DRules(LNodeTree lnt)
{
	int	i, n;
	n = lnt->argc;

	switch (lnt->kind) {
	case LN_1Tok:
	case LN_NTok:
		break;
	case LN_NNodes:
		for (i = 0; i < n; i++)
			lnt->argv[i].lnode = lin2DRules(lnt->argv[i].lnode);
		break;
	case LN_DoPile:
		for (i = 0; i < n; i++)
			lnt->argv[i].lnode = lin2DRules(lnt->argv[i].lnode);
		lnt = lin2DRulesPile(lnt);
		break;
	}

	return lnt;
}

#define linIsArgKW(lnt,n,k) \
	((lnt)->argc > (n) && (lnt)->argv[n].lnode->kind == LN_1Tok && \
	 tokIs((lnt)->argv[n].lnode->argv[0].tok, k))

local LNodeTree
lin2DRulesPile(LNodeTree lnt)
{
	Bool		hasStarter, hasEnder;
	int		iS, iE;
	LNodeTree	rnt;

	assert(lnt->kind == LN_DoPile);

	hasStarter = linIsArgKW(lnt, int0,	  DoPileStart);
	hasEnder   = linIsArgKW(lnt, lnt->argc-1, DoPileEnd);

	iS  = hasStarter ? 1 : 0;
	iE  = hasStarter && hasEnder ? lnt->argc - 2 : lnt->argc - 1;

	/*
	 * Consider	xxx1
	 *		   xxx2
	 *		   xxx3
	 *		xxx4
	 *	     xxx5
	 */

	/* Handle pile starting with first line: xxx1..xxx4 */
	rnt = lin2DRulesPile0(NULL, lnt, &iS, &iE);

	/* Handle subsequent outdented piles: e.g., xxx5 */
	while (iS <= iE)
		rnt = lin2DRulesPile0(rnt, lnt, &iS, &iE);

	lntFreeNode(lnt);
	return rnt;
}


/*
 * Piles a range of descendents. Note that since the final argument is
 * never updated it ought to be passed by value.
 */
local LNodeTree
lin2DRulesPile0(LNodeTree context, LNodeTree lnt, int *piS, int *piE)
{
	int		iS = *piS, iE = *piE;
	int		indentS, indent0;
	LNodeTree	lnt0;
	LNodeTreeList	sofar;

	DEBUG(fprintf(dbOut," ==== Linearizing parts %d to %d \n", *piS, *piE));

	if (iS > iE) return lntNewEmpty(LN_NNodes, int0);

	indentS = lnt->argv[iS].lnode->indent;
	sofar	= 0;

	DEBUG(fprintf(dbOut,"indentS = %d\n", indentS));
	while (iS <= iE) {
		lnt0	= lnt->argv[iS].lnode;
		indent0 = lnt0->indent;

		DEBUG({
			fprintf(dbOut,"%d/%d/%d. indent0 = %d (argc = %d)\n",
				iS, iE, (int) lnt->argc,
				indent0, (int) lnt0->argc);
			lntPrint(dbOut, lnt0);
			fnewline(dbOut);
		});

		if (linIsBlank(lnt0) || indent0 == MootIndentation) {
			iS++;
			DEBUG(fprintf(dbOut,"Ah.... a blank line _________\n"));
			sofar  = listCons(LNodeTree)(lnt0, sofar);
			continue;
		}

		DEBUG(fprintf(dbOut,"%d :: %d\n", indent0, indentS));
		if (indent0 < indentS)
			break;

		if (indent0 == indentS) {
			iS++;
			sofar  = listCons(LNodeTree)(lnt0, sofar);
			DEBUG({
				fprintf(dbOut,"Adding one more +++++++++++++ ");
				listPrint(LNodeTree)(dbOut, sofar, lntPrint);
				fnewline(dbOut);
			});
		}
		/* (indent0 > indentS) */
		else {
			setcar(sofar, lin2DRulesPile0(car(sofar),lnt,&iS,&iE));
			DEBUG({
				fprintf(dbOut,"Joining indentee >>>>>>>>>>>> ");
				listPrint(LNodeTree)(dbOut, sofar, lntPrint);
				fnewline(dbOut);
			});
		}
	}
	sofar = listNReverse(LNodeTree)(sofar);
	lnt0  = lntConcat(context, joinUp(context, sofar));

	DEBUG({
		fprintf(dbOut,"Result of joinUp is !!!!!!!!!!!!! ");
		lntPrint(dbOut, lnt0);
		fnewline(dbOut);
	});

	/* Free the list we just consed. */
	listFree(LNodeTree)(sofar);
	*piS = iS;
	*piE = iE;
	return lnt0;
}


/*
 * Insert SetTab .. BackSet .. BackSet ... BackTab
 */
local LNodeTree
joinUp(LNodeTree context, LNodeTreeList tll)
{
	LNodeTreeList	l;
	LNodeTree	lnt, t0, t1;
	Bool		hadBackSet;

	assert(tll != 0);

	/* Insert BackSets between the lines, if necessary. */
	lnt	   = car(tll);
	hadBackSet = false;

	for (l = tll, t0 = lnt; cdr(l); l = cdr(l), t0 = t1) {
		t1 = car(cdr(l));

		if (isBackSetRequired(context, t0, t1)) {
			hadBackSet = true;
			lnt = lntSeparate(lnt, KW_BackSet, t1);
		}
		else
			lnt = lntConcat(lnt, t1);
	}

	/* Surround with SetTab and BackTab, if necessary. */
	if (hadBackSet || isPileRequired(context, lnt))
		lnt = lntWrap(KW_SetTab, lnt, KW_BackTab);

	return lnt;
}

/*
 * "isPileRequired" decides whether a SetTab/BackTab empiling is needed
 *  for the line which is to be joined to *pcontext.
 *
 *  A single line pile is formed whenever the previous word is an alphabetic
 *  language keyword, e.g.  "return", "then", "else", etc.
 *  (Note, this does not include user-definable operators such as "quo".)
 */
local Bool
isPileRequired(LNodeTree context, LNodeTree lnt)
{
	/*ARGSUSED*/
	Token	tok = lntLastTokLessNL(context);

	DEBUG(fprintf(dbOut,"In isPileRequired %s",
		      tok? "with token: " : "NO TOK\n"));

	if (!tok) return false;

	DEBUG(tokPrint(dbOut, tok));

	return	tokIs(tok, KW_Then)    || tokIs(tok, KW_Else)    ||
		tokIs(tok, KW_With)    || tokIs(tok, KW_Add)     ||
		tokIs(tok, KW_Try)     || tokIs(tok, KW_But)     ||
		tokIs(tok, KW_Catch)   || tokIs(tok, KW_Finally) ||
		tokIs(tok, KW_Always);
}


/*
 * "isBackSetRequired" decides whether a BackSet is needed between tl1 and tl2.
 *  Normally a BackSet is needed.   The exceptions are:
 *
 * 1. tl1 contains only ++ comments (-- comments already deleted)
 *    This allows
 *
 *	++ xxx
 *	f: T -> Y
 *
 * 2. tl1 ends with "," or an opener ("(" "[" etc).
 *    This allows
 *
 *	f(x,		  a := [
 *	  y,		     1, 2, 3,
 *	  z)		     4, 5, 6 ]
 *
 * 3. tl2 begins with "in", "then", "else" or a closer (")" "]" "}" etc)
 *    (i.e. words which CANNOT start an expression).
 *    This allows
 *
 *	if aaa		  let		     f(x,	  a := [1, 2, 3,
 *	then bbb	     f == 1	       y		4, 5, 6
 *	else ccc	  in x := f+f	     )		       ]
 *
 * 4. The lines occur within "{ }"
 *
 * Note: case 4 appears to be ignored and the context is not used.
 */
local Bool
isBackSetRequired(LNodeTree context, LNodeTree lnt1, LNodeTree lnt2)
{
	/*ARGSUSED*/
	Token	tok1 = lntLastTokLessNL (lnt1);
	Token	tok2 = lntFirstTok	(lnt2);

	DEBUG({
		fprintf(dbOut,"In isBackSetRequrired ");
		fprintf(dbOut,"First tree ");
		lntPrint(dbOut, lnt1);
		fnewline(dbOut);
		fprintf(dbOut,"Second tree ");
		lntPrint(dbOut, lnt2);
		fnewline(dbOut);

		if (tok1){ fprintf(dbOut," with tok1 "); tokPrint(dbOut,tok1); }
		else {fprintf(dbOut,"No first token\n"); }
		if (tok2){ fprintf(dbOut," with tok2 "); tokPrint(dbOut,tok2); }
		else {fprintf(dbOut,"No second token\n"); }
		fnewline(dbOut);
	});

	/* Rule 1 */
	if (linIsCom(lnt1) || linIsBlank(lnt1) || linIsBlank(lnt2))
		return false;

	if (!tok1 || !tok2)
		return true;

	/* Rule 2 */
	if (tokIs(tok1, KW_Comma) || tokIsOpener(tok1))
		return false;

	/* Rule 3 */
	if (tokIsFollower(tok2) || tokIsCloser(tok2))
		return false;

	return true;
}

