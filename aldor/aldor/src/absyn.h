/*****************************************************************************
 *
 * absyn.h: Abstract syntax.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _ABSYN_H_
#define _ABSYN_H_

#include "axlobs.h"
#include "srcpos.h"
#include "token.h"

/******************************************************************************
 *
 * :: Node kinds
 *
 *****************************************************************************/

enum abSynTag {
    AB_START,
    AB_SYM_START = AB_START,

	AB_Id  = AB_SYM_START,
	AB_IdSy,
	AB_Blank,

    AB_SYM_LIMIT,

    AB_DOC_START = AB_SYM_LIMIT,

	AB_DocText = AB_DOC_START,

    AB_DOC_LIMIT,

    AB_STR_START = AB_DOC_LIMIT,

	AB_LitInteger = AB_STR_START,
	AB_LitFloat,
	AB_LitString,

    AB_STR_LIMIT,
    AB_NODE_START = AB_STR_LIMIT,

	AB_Add = AB_NODE_START,
	AB_And,
	AB_Apply,
	AB_Assert,
	AB_Assign,
	AB_Break,
	AB_Builtin,
	AB_CoerceTo,
	AB_Collect,
	AB_Comma,
	AB_Declare,
	AB_Default,
	AB_Define,
	AB_DDefine,
	AB_Delay,
	AB_Do,
	AB_Documented,
	AB_Except,
	AB_Exit,
	AB_Export,
	AB_Extend,
	AB_Fix,
	AB_Fluid,
	AB_For,
	AB_Foreign,
	AB_Free,
	AB_Generate,
	AB_Goto,
	AB_Has,
	AB_Hide,
	AB_If,
	AB_Import,
	AB_Inline,
	AB_Iterate,
	AB_Label,
	AB_Lambda,
	AB_Let,
	AB_Local,
	AB_Macro,
	AB_MDefine,
	AB_MLambda,
	AB_Never,
	AB_Not,
	AB_Nothing,
	AB_Or,
	AB_Paren,
	AB_PLambda,
	AB_PretendTo,
	AB_Qualify,
	AB_Quote,
	AB_Raise,
	AB_Reference,
	AB_Repeat,
	AB_RestrictTo,
	AB_Return,
	AB_Select,
	AB_Sequence,
	AB_Test,
	AB_Try,
	AB_Unit,
	AB_Where,
	AB_While,
	AB_With,
	AB_Yield,

    AB_NODE_LIMIT,
    AB_LIMIT = AB_NODE_LIMIT
};

typedef Enum(abSynTag)	AbSynTag;


/******************************************************************************
 *
 * :: Node forming macros
 *
 *****************************************************************************/

# define abNewAdd(p,t,c)	abNew(AB_Add,		p,2, t,c)
# define abNewAnd(p,a,b)	abNew(AB_And,		p,2, a,b)
# define abNewApplyL(p,o,l)	abNewOfOpAndList(AB_Apply,p,o,l)
# define abNewApply0(p,o)	abNew(AB_Apply,		p,1, o)
# define abNewApply1(p,o,a)	abNew(AB_Apply,		p,2, o,a)
# define abNewApply2(p,o,a,b)	abNew(AB_Apply,		p,3, o,a,b)
# define abNewAssert(p,e)	abNew(AB_Assert,	p,1, e)
# define abNewAssign(p,l,r)	abNew(AB_Assign,	p,2, l,r)
# define abNewBlank(p,s)	abNew(AB_Blank,		p,1, s)
# define abNewBreak(p,l)	abNew(AB_Break,		p,1, l)
# define abNewBuiltin(p,b)	abNew(AB_Builtin,	p,1, b)
# define abNewCoerceTo(p,a,t)	abNew(AB_CoerceTo,	p,2, a,t)
# define abNewCollectOL(p,b,l)	abNewOfOpAndList(AB_Collect,p,b,l)
# define abNewDeclare(p,v,t)	abNew(AB_Declare,	p,2, v,t)
# define abNewDefault(p,b)	abNew(AB_Default,	p,1, b)
# define abNewDefine(p,l,r)	abNew(AB_Define,	p,2, l,r)
# define abNewDDefine(p,b)	abNew(AB_DDefine,	p,1, b)
# define abNewDelay(p,e)	abNew(AB_Delay,		p,1, e)
# define abNewDo(p,e)		abNew(AB_Do,		p,1, e)
# define abNewDocumented(p,e,d) abNew(AB_Documented,	p,2, e,d)
# define abNewDocText(p,d)	abNew(AB_DocText,	p,1, d)
# define abNewExcept(p,t,e)	abNew(AB_Except,	p,2, t,e)
# define abNewExit(p,c,v)	abNew(AB_Exit,		p,2, c,v)
# define abNewExport(p,b,f,t)	abNew(AB_Export,	p,3, b,f,t)
# define abNewExtend(p,b)	abNew(AB_Extend,	p,1, b)
# define abNewFluid(p,b)	abNew(AB_Fluid,		p,1, b)
# define abNewFix(p,b)		abNew(AB_Fix,		p,1, b)
# define abNewFor(p,v,i,c)	abNew(AB_For,		p,3, v,i,c)
# define abNewForeign(p,w,o)	abNew(AB_Foreign,	p,2, w,o)
# define abNewFree(p,b)		abNew(AB_Free,		p,1, b)
# define abNewGenerate(p,n,e)	abNew(AB_Generate,	p,2, n,e)
# define abNewGoto(p,l)		abNew(AB_Goto,		p,1, l)
# define abNewHas(p,a,b)	abNew(AB_Has,		p,2, a,b)
# define abNewHide(p,e)		abNew(AB_Hide,		p,1, e)
# define abNewHook(p,e)		abNew(AB_Hook,		p,1, e)
# define abNewId(p,s)		abNew(AB_Id,		p,1, s)
# define abNewIdSy(p,s)		abNew(AB_IdSy,		p,1, s)
# define abNewIf(p,c,t,e)	abNew(AB_If,		p,3, c,t,e)
# define abNewImport(p,b,f)	abNew(AB_Import,	p,2, b,f)
# define abNewInline(p,b,f)	abNew(AB_Inline,	p,2, b,f)
# define abNewIterate(p,l)	abNew(AB_Iterate,	p,1, l)
# define abNewLabel(p,l,s)	abNew(AB_Label,		p,2, l,s)
# define abNewLambda(p,a,r,b)	abNew(AB_Lambda,	p,3, a,r,b)
# define abNewLet(p,a,b)	abNew(AB_Let,		p,2, a,b)
# define abNewLitInteger(p,s)	abNew(AB_LitInteger,	p,1, s)
# define abNewLitFloat(p,s)	abNew(AB_LitFloat,	p,1, s)
# define abNewLitString(p,s)	abNew(AB_LitString,	p,1, s)
# define abNewLocal(p,b)	abNew(AB_Local,		p,1, b)
# define abNewMacro(p,b)	abNew(AB_Macro,		p,1, b)
# define abNewMDefine(p,l,r)    abNew(AB_MDefine,	p,2, l,r)
# define abNewMLambda(p,l,r)    abNew(AB_MLambda,	p,2, l,r)
# define abNewNever(p)		abNew(AB_Never,		p,0  )
# define abNewNot(p,a)		abNew(AB_Not,		p,1, a)
# define abNewNothing(p)	abNew(AB_Nothing,	p,0  )
# define abNewOr(p,a,b)		abNew(AB_Or,		p,2, a,b)
# define abNewParen(p,a)	abNew(AB_Paren,		p,1, a)
# define abNewPLambda(p,a,r,b)	abNew(AB_PLambda,	p,3, a,r,b)
# define abNewPretendTo(p,a,t)	abNew(AB_PretendTo,	p,2, a,t)
# define abNewQualify(p,o,d)	abNew(AB_Qualify,	p,2, o,d)
# define abNewRaise(p,e)	abNew(AB_Raise,		p,1, e)
# define abNewReference(p,e)	abNew(AB_Reference,	p,1, e)
# define abNewRepeatOL(p,b,l)	abNewOfOpAndList(AB_Repeat,p,b,l) /* Reverse */
# define abNewRepeat0(p,b)	abNew(AB_Repeat,	p,1, b)
# define abNewRestrictTo(p,a,t) abNew(AB_RestrictTo,	p,2, a,t)
# define abNewReturn(p,e)	abNew(AB_Return,	p,1, e)
# define abNewSelect(p,e,b)	abNew(AB_Select,	p,2, e,b)
# define abNewSequenceL(p,l)	abNewOfList(AB_Sequence,p,l)
# define abNewSequence0(p)	abNew(AB_Sequence,	p,0  )
# define abNewSequence1(p,a)	abNew(AB_Sequence,	p,1, a)
# define abNewSequence2(p,a,b)	abNew(AB_Sequence,	p,2, a,b)
# define abNewTest(p,a)		abNew(AB_Test,		p,1, a)
# define abNewTry(p,v,id,e,a)	abNew(AB_Try,		p,4, v,id, e,a)
# define abNewCommaL(p,l)	abNewOfList(AB_Comma,	p,l)
# define abNewComma0(p)		abNew(AB_Comma,		p,0  )
# define abNewComma1(p,a)	abNew(AB_Comma,		p,1, a)
# define abNewComma2(p,a,b)	abNew(AB_Comma,		p,2, a,b)
# define abNewWhere(p,d,e)	abNew(AB_Where,		p,2, d,e) /* Reverse */
# define abNewWhile(p,c)	abNew(AB_While,		p,1, c)
# define abNewWith(p,t,s)	abNew(AB_With,		p,2, t,s)
# define abNewYield(p,e)	abNew(AB_Yield,		p,1, e)

extern AbSyn __abNewTest	(SrcPos, AbSyn);

/******************************************************************************
 *
 * :: Structures for nodes
 *
 *****************************************************************************/

/*
 * AbUse
 */

enum ab_use {
	AB_Use_Declaration,	/* Declaration. */
	AB_Use_Type,		/* Used, e.g., on right-hand side of : . */
	AB_Use_Label,		/* Used in labeled expression. */
	AB_Use_Assign,		/* Used on left-hand side of := . */
	AB_Use_Define,		/* Used on left-hand side of == . */
	AB_Use_Value,		/* Expression whose value is used. */
	AB_Use_RetValue,	/* Expression whose value is returned. */
	AB_Use_NoValue,		/* Expression whose value is not used. */
	AB_Use_Iterator,	/* Iterator on repeat or collect. */
	AB_Use_Default,		/* Default variable declaration. */
	AB_Use_Except,		/* Value in "except" RHS */
	AB_Use_Elided,		/* Used where real syntax was dropped. */
	AB_Use_LIMIT
};

typedef Enum(ab_use)	AbUse;

/*
 * AbState
 */

enum ab_state {
	AB_State_AbSyn,		/* No semantics yet. */
	AB_State_HasPoss,	/* type union has poss list. */
	AB_State_HasUnique,	/* type union has unique type. */
	AB_State_Error,		/* an error has been found for this node */
	AB_State_LIMIT
};

typedef Enum(ab_state)	AbState;

/*
 * AbEmbed
 */

typedef	ULong		AbEmbed;

#define			AB_Embed_Fail			((AbEmbed) 0)

#define			AB_Embed_Identity		(((AbEmbed) 1) << 0)
#define			AB_Embed_CrossToTuple		(((AbEmbed) 1) << 1)
#define			AB_Embed_CrossToMulti		(((AbEmbed) 1) << 2)
#define			AB_Embed_CrossToUnary		(((AbEmbed) 1) << 3)
#define			AB_Embed_MultiToTuple		(((AbEmbed) 1) << 4)
#define			AB_Embed_MultiToCross		(((AbEmbed) 1) << 5)
#define			AB_Embed_MultiToUnary		(((AbEmbed) 1) << 6)
#define			AB_Embed_UnaryToTuple		(((AbEmbed) 1) << 7)
#define			AB_Embed_UnaryToCross		(((AbEmbed) 1) << 8)
#define			AB_Embed_UnaryToMulti		(((AbEmbed) 1) << 9)
#define			AB_Embed_UnaryToRaw		(((AbEmbed) 1) << 10)
#define			AB_Embed_RawToUnary		(((AbEmbed) 1) << 11)
#define			AB_Embed_ApplyMultiToTuple	(((AbEmbed) 1) << 12)
#define			AB_Embed_ApplyMultiToCross	(((AbEmbed) 1) << 13)

#define			AB_Embed_ArgMask		(~AB_Embed_ApplyMask)
#define			AB_Embed_ApplyMask		\
	(AB_Embed_ApplyMultiToTuple | AB_Embed_ApplyMultiToCross)

#define		abEmbedArg(ab)		(abTContext(ab) & AB_Embed_ArgMask)
#define		abEmbedApply(ab)	(abTContext(ab) & AB_Embed_ApplyMask)

/*
 * AbSeman
 */
/* This structure is getting a bit big.  Should attempt to compress it. */
struct abSeman {
	Doc	comment;	/* Comments attached to the absyn. */
	Stab	stab;		/* For lexical level. */
	int	defnIdx;	/* Definition index */
	Syme	syme;		/* Meaning of id or literal. */
	TForm	tform;		/* In type context. */
	AbSyn	implicit;	/* Implicit operator on expression. */
	AbEmbed embed;		/* Implicit embedding for product contexts. */
	SImpl   impl;		/* Syme implementation, if any */
};

typedef struct abSeman *AbSeman;

/*
 * Generic Views
 */

struct abHdr {
	BPack(AbSynTag)		tag;
	BPack(AbUse)		use;
	BPack(AbState)		state;

	Length			argc;
	SrcPosStack		pos;

	AbSeman			seman;

	union {
		TPoss	poss;		 /* during tiBottomUp */
		TForm	unique;		 /* after  tiTopDown  */
	}			type;
};

struct abGen {
	struct abHdr		hdr;
	union {
		Symbol	sym;
		Doc	doc;
		String	str;
		AbSyn	argv[NARY];
	}			data;
};

/*
 * Leaf Nodes
 */

struct abBlank {
	struct abHdr		hdr;
	Symbol			sym;
};

struct abId {
	struct abHdr		hdr;
	Symbol			sym;
};

struct abIdSy {
	struct abHdr		hdr;
	Symbol			sym;
};

struct abDocText {
	struct abHdr		hdr;
	Doc			doc;
};

struct abLitInteger {
	struct abHdr		hdr;
	String			str;
};

struct abLitFloat {
	struct abHdr		hdr;
	String			str;
};

struct abLitString {
	struct abHdr		hdr;
	String			str;
};

/*
 * Interior Nodes
 */

struct abAdd {
	struct abHdr		hdr;
	AbSyn			base;
	AbSyn			capsule;
};

struct abAnd {
	struct abHdr		hdr;
	AbSyn			argv[NARY];
};

struct abApply {
	struct abHdr		hdr;
	AbSyn			op;
	AbSyn			argv[NARY];
};

struct abAssert {
	struct abHdr		hdr;
	AbSyn			test;
};

struct abAssign {
	struct abHdr		hdr;
	AbSyn			lhs;
	AbSyn			rhs;
};

struct abBreak {
	struct abHdr		hdr;
	AbSyn			label;
};

struct abBuiltin {
	struct abHdr		hdr;
	AbSyn			what;
};

struct abDeclare {
	struct abHdr		hdr;
	AbSyn			id;
	AbSyn			type;
};

struct abDo {
	struct abHdr		hdr;
	AbSyn			expr;
};

struct abDocumented {
	struct abHdr		hdr;
	AbSyn			expr;
	AbSyn			doc;
};

struct abCoerceTo {
	struct abHdr		hdr;
	AbSyn			expr;
	AbSyn			type;
};

struct abCollect {
	struct abHdr		hdr;
	AbSyn			body;
	AbSyn			iterv[NARY];
};

struct abComma {
	struct abHdr		hdr;
	AbSyn			argv[NARY];
};

struct abDefault {
	struct abHdr		hdr;
	AbSyn			body;
};

struct abDefine {
	struct abHdr		hdr;
	AbSyn			lhs;
	AbSyn			rhs;
};

struct abDDefine {
	struct abHdr		hdr;
	AbSyn			body;
};

struct abExcept {
	struct abHdr		hdr;
	AbSyn			type;
	AbSyn			except;
};

struct abExit {
	struct abHdr		hdr;
	AbSyn			test;
	AbSyn			value;
};

struct abExport {
	struct abHdr		hdr;
	AbSyn			what;
	AbSyn			origin;
	AbSyn			destination;
};

struct abExtend {
	struct abHdr		hdr;
	AbSyn			body;
};

struct abFix {
	struct abHdr		hdr;
	AbSyn			function;
};

struct abFluid {
	struct abHdr		hdr;
	AbSyn			argv[NARY];
};

struct abFor {
	struct abHdr		hdr;
	AbSyn			lhs;
	AbSyn			whole;
	AbSyn			test;
};

struct abForeign {
	struct abHdr		hdr;
	AbSyn			what;
	AbSyn			origin;
};

struct abFree {
	struct abHdr		hdr;
	AbSyn			argv[NARY];
};

struct abGenerate {
	struct abHdr		hdr;
	AbSyn			count;
	AbSyn			body;
};

struct abGoto {
	struct abHdr		hdr;
	AbSyn			label;
};

struct abHas {
	struct abHdr		hdr;
	AbSyn			expr;
	AbSyn			property;
};

struct abHide {
	struct abHdr		hdr;
	AbSyn			type;
};

struct abHook {
	struct abHdr		hdr;
	AbSyn			expr;
};

struct abIf {
	struct abHdr		hdr;
	AbSyn			test;
	AbSyn			thenAlt;
	AbSyn			elseAlt;
};

struct abImport {
	struct abHdr		hdr;
	AbSyn			what;
	AbSyn			origin;
};

struct abInline {
	struct abHdr		hdr;
	AbSyn			what;
	AbSyn			origin;
};

struct abIterate {
	struct abHdr		hdr;
	AbSyn			label;
};

struct abLabel {
	struct abHdr		hdr;
	AbSyn			label;
	AbSyn			expr;
};

struct abLambda {
	struct abHdr		hdr;
	AbSyn			param;
	AbSyn			rtype;
	AbSyn			body;
};

struct abLet {
	struct abHdr		hdr;
	AbSyn			context;
	AbSyn			expr;
};

struct abLocal {
	struct abHdr		hdr;
	AbSyn			argv[NARY];
};

struct abMacro {
	struct abHdr		hdr;
	AbSyn			expr;
};

struct abMDefine {
	struct abHdr		hdr;
	AbSyn			lhs;
	AbSyn			rhs;
};

struct abMLambda {
	struct abHdr		hdr;
	AbSyn			param;
	AbSyn			body;
};

struct abNever {
	struct abHdr		hdr;
};

struct abNot {
	struct abHdr		hdr;
	AbSyn			expr;
};

struct abNothing {
	struct abHdr		hdr;
};

struct abOr {
	struct abHdr		hdr;
	AbSyn			argv[NARY];
};

struct abParen {
	struct abHdr		hdr;
	AbSyn			expr;
};

struct abPLambda {
	struct abHdr		hdr;
	AbSyn			param;
	AbSyn			rtype;
	AbSyn			body;
};

struct abPretendTo {
	struct abHdr		hdr;
	AbSyn			expr;
	AbSyn			type;
};

struct abQualify {
	struct abHdr		hdr;
	AbSyn			what;
	AbSyn			origin;
};

struct abRaise {
	struct abHdr		hdr;
	AbSyn			expr;
};

struct abReference {
	struct abHdr		hdr;
	AbSyn			body;
};

struct abRepeat {
	struct abHdr		hdr;
	AbSyn			body;
	AbSyn			iterv[NARY];
};

struct abRestrictTo {
	struct abHdr		hdr;
	AbSyn			expr;
	AbSyn			type;
};

struct abRetractTo {
	struct abHdr		hdr;
	AbSyn			expr;
	AbSyn			type;
};

struct abReturn {
	struct abHdr		hdr;
	AbSyn			value;
};

struct abSelect {
	struct abHdr		hdr;
	AbSyn			testPart;
	AbSyn			alternatives;
};

struct abSequence {
	struct abHdr		hdr;
	AbSyn			argv[NARY];
};

struct abTest {
	struct abHdr		hdr;
	AbSyn			cond;
};

struct abTry {
	struct abHdr		hdr;
	AbSyn			expr;
	AbSyn			id;
	AbSyn			except;
	AbSyn			always;
};

struct abWhere {
	struct abHdr		hdr;
	AbSyn			context;
	AbSyn			expr;
};

struct abWhile {
	struct abHdr		hdr;
	AbSyn			test;
};

struct abWith {
	struct abHdr		hdr;
	AbSyn			base;
	AbSyn			within;
};

struct abYield {
	struct abHdr		hdr;
	AbSyn			value;
};

/*
 * The comprehensive data structure.
 */

union abSyn {
	/*
	 * Generic views.
	 */
	struct abHdr		abHdr;
	struct abGen		abGen;

	/*
	 * All the different kinds of nodes.
	 */

	/* Leaves */
	struct abBlank		abBlank;
	struct abId		abId;
	struct abIdSy		abIdSy;

	struct abDocText	abDocText;
	struct abLitInteger	abLitInteger;
	struct abLitString	abLitString;
	struct abLitFloat	abLitFloat;

	/* Interior */
	struct abAdd		abAdd;
	struct abAnd		abAnd;
	struct abApply		abApply;
	struct abAssert		abAssert;
	struct abAssign		abAssign;
	struct abBreak		abBreak;
	struct abBuiltin	abBuiltin;
	struct abDeclare	abDeclare;
	struct abCoerceTo	abCoerceTo;
	struct abCollect	abCollect;
	struct abComma		abComma;
	struct abDefault	abDefault;
	struct abDefine		abDefine;
	struct abDDefine	abDDefine;
	struct abDo		abDo;
	struct abDocumented	abDocumented;
	struct abExcept		abExcept;
	struct abExit		abExit;
	struct abExport		abExport;
	struct abExtend		abExtend;
	struct abFix		abFix;
	struct abFluid		abFluid;
	struct abFor		abFor;
	struct abForeign	abForeign;
	struct abFree		abFree;
	struct abGenerate	abGenerate;
	struct abGoto		abGoto;
	struct abHas		abHas;
	struct abHide		abHide;
	struct abHook		abHook;
	struct abIf		abIf;
	struct abImport		abImport;
	struct abInline		abInline;
	struct abIterate	abIterate;
	struct abLabel		abLabel;
	struct abLambda		abLambda;
	struct abLet		abLet;
	struct abLocal		abLocal;
	struct abMacro		abMacro;
	struct abMDefine	abMDefine;
	struct abMLambda	abMLambda;
	struct abNever		abNever;
	struct abNot		abNot;
	struct abNothing	abNothing;
	struct abOr		abOr;
	struct abParen		abParen;
	struct abPLambda	abPLambda;
	struct abPretendTo	abPretendTo;
	struct abQualify	abQualify;
	struct abRaise		abRaise;
	struct abReference	abReference;
	struct abRepeat		abRepeat;
	struct abRestrictTo	abRestrictTo;
	struct abRetractTo	abRetractTo;
	struct abReturn		abReturn;
	struct abSelect		abSelect;
	struct abSequence	abSequence;
	struct abTest		abTest;
	struct abTry		abTry;
	struct abWhere		abWhere;
	struct abWhile		abWhile;
	struct abWith		abWith;
	struct abYield		abYield;
};


/******************************************************************************
 *
 * :: Table of information for node types
 *
 *****************************************************************************/

struct ab_info {
	AbSynTag		tag;
	Hash			hash;
	SExpr			sxsym;
	String			str;		    /* absyn name */
	TokenTag		tokenTag;	    /* for pretty printing */
};

extern struct ab_info	abInfoTable[];

#define abInfo(tag)	abInfoTable[(tag) - AB_START]


/******************************************************************************
 *
 * :: Operations on nodes
 *
 *****************************************************************************/

/*
 * Macros for generic treatment of structures.
 */

# define abIsLeaf(a)	  (abTag(a) < AB_NODE_START)
# define abIsSymTag(t)	  (/*AB_SYM_START <= (t) &&*/ (t) < AB_SYM_LIMIT)
# define abIsDocTag(t)	  (  AB_DOC_START <= (t) &&   (t) < AB_DOC_LIMIT)
# define abIsStrTag(t)	  (  AB_STR_START <= (t) &&   (t) < AB_STR_LIMIT)

# define abTag(a)	  ((a)->abHdr.tag)
# define abHasTag(a,t)	  ((a)->abHdr.tag == (t))

# define abPos(a)	  (spstackFirst((a)->abHdr.pos))
# define abSetPos(a,p)	  ((a)->abHdr.pos=spstackSetFirst ((a)->abHdr.pos,(p)))

# define abUse(a)	  ((a)->abHdr.use)
# define abState(a)	  ((a)->abHdr.state)
# define abArgc(a)	  ((a)->abHdr.argc)
# define abArgv(a)	  ((a)->abGen.data.argv)
# define abLeafSym(a)	  ((a)->abGen.data.sym)
# define abLeafDoc(a)	  ((a)->abGen.data.doc)
# define abLeafStr(a)	  ((a)->abGen.data.str)
# define abTPoss(a)	  ((a)->abHdr.type.poss)
# define abGoodTPoss(a)	  (abState(a) == AB_State_Error ? NULL :abTPoss(a))
# define abTUnique(a)	  ((a)->abHdr.type.unique)
# define abComment(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->comment	 : 0)
# define abStab(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->stab	 : 0)
# define abSyme(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->syme	 : 0)
# define abTForm(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->tform	 : 0)
# define abImplicit(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->implicit : 0)
# define abTContext(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->embed	 : 0)
# define abDefineIdx(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->defnIdx  : -1)
# define abSymeImpl(a)	  ((a)->abHdr.seman ? (a)->abHdr.seman->impl	 : 0)

# define abRepeatIterc(a) (abArgc(a)-1) /* -1 for body */
# define abCollectIterc(a)(abArgc(a)-1) /* -1 for body */
# define abImplicitSyme(a)(abImplicit(a) ? abSyme(abImplicit(a)) : 0)

# define abCopyPos(a)	  abNewNothing(abPos(a))

#define			abIsUnknown(a)		abHasTag(a, AB_Blank)

#define			abIsNothing(a)		abHasTag(a, AB_Nothing)
#define			abIsNotNothing(a)	!abIsNothing(a)

#define			abIsId(a)		abHasTag(a, AB_Id)
#define			abIdSym(a)		((a)->abId.sym)
#define			abIsTheId(a,s)		(abIsId(a) && abIdSym(a)==(s))

#define			abIsIf(a)		abHasTag(a, AB_If)

#define			abIsExit(a)		abHasTag(a, AB_Exit)

#define			abIsDefine(a)		abHasTag(a, AB_Define)
#define			abDefineDecl(a)		((a)->abDefine.lhs)
#define			abDefineVal(a)		((a)->abDefine.rhs)

#define			abIsApply(a)		abHasTag(a, AB_Apply)
#define			abApplyOp(a)		((a)->abApply.op)
#define			abApplyArg(a,i)		((a)->abApply.argv[i])
#define			abSetApplyArg(a,i,e)	((a)->abApply.argv[i] = (e))
#define			abApplyArgc(a)		(abArgc(a)-1) /* -1 for op */
#define			abApplyArgv(a)		((a)->abApply.argv)
#define			abIsApplyOf(a,s)	\
	(abIsApply(a) && abIsTheId(abApplyOp(a), s))

#define			abIsJoin(a)		abIsApplyOf(a, ssymJoin)

#define			abIsAnyMap(a)		\
	(abIsGenericMap(a) || abIsPackedMap(a))
#define			abIsGenericMap(a)	\
	(abIsApplyOf(a, ssymArrow) && abApplyArgc(a) == 2)
#define			abIsPackedMap(a)	\
	(abIsApplyOf(a, ssymPackedArrow) && abApplyArgc(a) == 2)

#define			abMapArg(a)		abApplyArg(a, 0)
#define			abMapRet(a)		abApplyArg(a, 1)

#define			abIsAnyLambda(a)	\
	(abHasTag(a, AB_Lambda) || abHasTag(a, AB_PLambda))

#define			abIsEmptySequence(a)	\
	((a) && abHasTag(a, AB_Sequence) && abArgc(a) == 0)

#define		abArgcAs(tag,ab)	(abHasTag(ab,tag) ? abArgc(ab) : 1)
#define		abArgvAs(tag,ab)	(abHasTag(ab,tag) ? abArgv(ab) : &(ab))

#define		abIsRecordOrUnion(op)				\
		(						\
			abIsTheId((op), ssymUnion)	||	\
			abIsTheId((op), ssymRecord)	||	\
			abIsTheId((op), ssymRawRecord)		\
		)

/*
 * :: General operations
 */
extern AbSyn	abNewEmpty		(AbSynTag t, Length argc);
extern void	abFree			(AbSyn);
extern void	abFreeNode		(AbSyn);

extern AbSyn	abNew			(AbSynTag t, SrcPos, Length argc, ...);
extern AbSyn	abNewOfList		(AbSynTag t, SrcPos, AbSynList);
extern AbSyn	abNewOfOpAndList	(AbSynTag t, SrcPos,AbSyn op,AbSynList);
extern AbSyn	abNewOfToken		(AbSynTag t, Token);

extern AbSyn	abCopy			(AbSyn);
extern AbSyn	abReposition		(AbSyn, SrcPos pos, SrcPos end);
extern AbSyn	abMarkAsMacroExpanded	(AbSyn);
extern Bool	abContains		(AbSyn, AbSyn);
extern Bool	abEqual			(AbSyn, AbSyn);
extern Bool	abEqualModDeclares	(AbSyn, AbSyn);
extern Hash	abHash			(AbSyn);
extern Hash	abHashSefo		(AbSyn);
extern Hash	abHashList		(AbSynList);
extern Hash	abHashModDeclares	(AbSyn);
extern void	abSubSymbol		(AbSyn, Symbol, Symbol);
extern AbSyn	abFrSyme		(Syme);

extern int	abPrint			(FILE *, AbSyn);
extern int	abPrintDb		(AbSyn);
extern int	abPrintClipped		(FILE *, AbSyn, int maxNodes);
extern int	abPrintClippedDb	(AbSyn, int maxNodes);
extern int      abOStreamPrint          (OStream, AbSyn ab);

extern SExpr	abToSExpr		(AbSyn);
extern AbSyn	abFrSExpr		(SExpr);

extern AbSyn	abRdSExpr		(FILE *, FileName *, int *lno);
extern int	abWrSExpr		(FILE *, AbSyn, ULong sxioMode);

extern int      abToBuffer		(Buffer buf, AbSyn ab);
extern AbSyn    abFrBuffer		(Buffer buf);

extern void	abTransferSemantics	(AbSyn from, AbSyn to);
extern AbSeman	abNewSemantics		(void);

extern Doc	abSetComment		(AbSyn, Doc);
extern Stab	abSetStab		(AbSyn, Stab);
extern Syme	abSetSyme		(AbSyn, Syme);
extern TForm	abSetTForm		(AbSyn, TForm);
extern AbSyn	abSetImplicit		(AbSyn, AbSyn);
extern AbEmbed	abSetTContext		(AbSyn, AbEmbed);
extern AbEmbed  abAddTContext		(AbSyn, AbEmbed);
extern void     abSetDefineIdx		(AbSyn, int);
extern void     abSetImpl		(AbSyn, SImpl);

extern TPoss	abResetTPoss		(AbSyn, TPoss);
extern TPoss	abReferTPoss		(AbSyn);
		/*
		 * Refer to (possibly creating) the type possibility set.
		 */

/*
 * :: Tree shape
 */

extern Length	abTreeHeight		(AbSyn);
extern SrcPos	abEnd			(AbSyn);
extern void	abSetEnd		(AbSyn, SrcPos);
extern void	abPosNodeSpan		(AbSyn, AbSyn  *pmin, AbSyn  *pmax);
extern void	abPosSpan		(AbSyn, SrcPos *pmin, SrcPos *pmax);

/*
 *:: Tree searching
 */

extern Bool	abHasSymbol		(AbSyn, Symbol);
		/*
		 * Return true iff the tree has an abId with the given symbol.
		 */

extern AbSyn	abContainer		(AbSyn root, SrcPos pos);
		/*
		 * Find leftmost smallest subtree with positions spanning pos.
		 * 0 indicates failure.
		 */

extern AbSyn	abSupremum		(AbSyn root, AbSyn a, AbSyn b,
					 Bool (*eql)(AbSyn, AbSyn));
		/*
		 * Find the leftmost smallest subtree containing both a and b.
		 * If eql is 0 then `==' is used as the comparison.
		 */

extern AIntList	abPathToNode		(AbSyn root, AbSyn node,
					 Bool (*eql)(AbSyn, AbSyn),
					 int *plen, AIntList revPathSoFar);
		/*
		 * path = abPathToNode(r, n, eql, &len, 0);
		 * finds node n in the given tree r.
		 * If len >= 0, then the node was found.
		 * If eql is 0, then `==' is used as the comparison.
		 */

/*
 * :: Operations for to make applications
 */

extern AbSyn abNewNofix	     (SrcPos pos, AbSyn	 op);
extern AbSyn abNewInfix	     (SrcPos pos, AbSyn	 op, AbSyn a, AbSyn b);
extern AbSyn abNewPrefix     (SrcPos pos, AbSyn	 op, AbSyn a);
extern AbSyn abNewPostfix    (SrcPos pos, AbSyn	 op, AbSyn a);
extern AbSyn abNewMatchfix   (SrcPos pos, AbSyn	 op, AbSyn a);

/*
 * :: Operations for take apart nodes
 */

typedef AbSyn	(*AbSynGetter)		(AbSyn, Length);
		/*
		 * The type of a function which returns a part of a tree,
		 * indexed in some private way.
		 */

extern AbSyn	abArgf			(AbSyn ab, Length i);
		/*
		 * Return the i'th argument of ab.
		 * This is useful as an AbSynGetter.
		 */

extern AbSyn	abThisArgf		(AbSyn ab, Length i);
		/*
		 * Returns ab.	'i' must be 0.
		 * This is useful as an AbSynGetter.
		 */

extern AbSyn	abForIterArgf		(AbSyn ab, Length i);
		/*
		 * Returns ab->abFor.whole.  'i' must be 0.
		 */

extern AbSyn	abSetArgf		(AbSyn ab, Length i);
		/*
		 * Returns l, i, j, r  in 'l(i,j) := r'.
		 */

extern AbSyn	abDefineeId		(AbSyn ab);
extern AbSyn	abDefineeIdOrElse	(AbSyn ab, AbSyn failure);
extern AbSyn	abDefineeType		(AbSyn ab);
extern AbSyn	abDefineeTypeOrElse	(AbSyn ab, AbSyn failed);

		/*
		 * Operations for taking apart definitions.
		 */

extern AbSyn	abApplyArgf		(AbSyn app, Length i);
		/*
		 * Returns the ith operand of an application.
		 * abApplyArgf is a function corresponding to the macro.
		 */

extern AbSyn	abNewApplyOfComma	(AbSyn op, AbSyn arg);
		/*
		 * Make an apply of the operator and the arguments.
		 * The arguments may be a comma or a singleton.
		 */

extern AbSyn	abNewApplyArg		(AbSyn app);
		/*
		 * Make a comma of the arguments of an application.
		 * Just the comma node is new; the arguments are the originals.
		 */

extern AbSyn	abNewApplyDeclaredArg	(AbSyn app);
		/*
		 * Make a comma of the arguments of an application.
		 * If an argument is an id, a declaration node is created.
		 */

extern AbSyn	abCopyApplyArg		(AbSyn app);
		/*
		 * Make a comma of copies of the arguments of an application.
		 * The entire tree is new.
		 */

extern AbSyn	abOneOrNewOfList	(AbSynTag t, AbSynList args);
		/*
		 * Return a singleton or make a node from a list of many parts.
		 */

extern AbSyn	abNewDocTextOfList	(TokenList);
		/*
		 * Construct a document text node from a list of doc tokens.
		 */


#define AB_SWITCH(ab, fun, args)				\
	switch (abTag(ab)) {					\
	case AB_Id:		fun##Id		args; break;	\
	case AB_IdSy:		fun##IdSy	args; break;	\
	case AB_Blank:		fun##Blank	args; break;	\
	case AB_LitInteger:	fun##LitInteger args; break;	\
	case AB_LitFloat:	fun##LitFloat	args; break;	\
	case AB_LitString:	fun##LitString	args; break;	\
	case AB_Add:		fun##Add	args; break;	\
	case AB_And:		fun##And	args; break;	\
	case AB_Apply:		fun##Apply	args; break;	\
	case AB_Assert:		fun##Assert	args; break;	\
	case AB_Assign:		fun##Assign	args; break;	\
	case AB_Break:		fun##Break	args; break;	\
	case AB_Builtin:	fun##Builtin	args; break;	\
	case AB_CoerceTo:	fun##CoerceTo	args; break;	\
	case AB_Collect:	fun##Collect	args; break;	\
	case AB_Comma:		fun##Comma	args; break;	\
	case AB_Declare:	fun##Declare	args; break;	\
	case AB_Default:	fun##Default	args; break;	\
	case AB_Define:		fun##Define	args; break;	\
	case AB_Delay:		fun##Delay	args; break;	\
	case AB_Do:		fun##Do		args; break;	\
	case AB_Except:		fun##Except	args; break;	\
	case AB_Raise:		fun##Raise	args; break;	\
	case AB_Exit:		fun##Exit	args; break;	\
	case AB_Export:		fun##Export	args; break;	\
	case AB_Extend:		fun##Extend	args; break;	\
	case AB_Fix:		fun##Fix	args; break;	\
	case AB_Fluid:		fun##Fluid	args; break;	\
	case AB_For:		fun##For	args; break;	\
	case AB_Foreign:	fun##Foreign	args; break;	\
	case AB_Free:		fun##Free	args; break;	\
	case AB_Generate:	fun##Generate	args; break;	\
	case AB_Goto:		fun##Goto	args; break;	\
	case AB_Has:		fun##Has	args; break;	\
	case AB_Hide:		fun##Hide	args; break;	\
	case AB_If:		fun##If		args; break;	\
	case AB_Import:		fun##Import	args; break;	\
	case AB_Inline:		fun##Inline	args; break;	\
	case AB_Iterate:	fun##Iterate	args; break;	\
	case AB_Label:		fun##Label	args; break;	\
	case AB_Lambda:		fun##Lambda	args; break;	\
	case AB_Let:		fun##Let	args; break;	\
	case AB_Local:		fun##Local	args; break;	\
	case AB_Macro:		fun##Macro	args; break;	\
	case AB_MLambda:	fun##MLambda    args; break;	\
	case AB_Never:		fun##Never	args; break;	\
	case AB_Not:		fun##Not	args; break;	\
	case AB_Nothing:	fun##Nothing	args; break;	\
	case AB_Or:		fun##Or		args; break;	\
	case AB_PLambda:	fun##Lambda	args; break;	\
	case AB_PretendTo:	fun##PretendTo	args; break;	\
	case AB_Qualify:	fun##Qualify	args; break;	\
	case AB_Quote:		fun##Quote	args; break;	\
	case AB_Reference:	fun##Reference	args; break;	\
	case AB_Repeat:		fun##Repeat	args; break;	\
	case AB_RestrictTo:	fun##RestrictTo args; break;	\
	case AB_Return:		fun##Return	args; break;	\
	case AB_Select:		fun##Select	args; break;	\
	case AB_Sequence:	fun##Sequence	args; break;	\
	case AB_Test:		fun##Test	args; break;	\
	case AB_Try:		fun##Try	args; break;	\
	case AB_Where:		fun##Where	args; break;	\
	case AB_While:		fun##While	args; break;	\
	case AB_With:		fun##With	args; break;	\
	case AB_Yield:		fun##Yield	args; break;	\
	default:		bugBadCase	(abTag(ab));	\
	}


#endif /* !_ABSYN_H_ */
