/*
 * ccode.h
 *
 * Structures for manipulating C programs.
 *
 * The macros ccNewXxxx (e.g. ccNewFor, ccNewInfix) allow a full tree to be
 * created which contains all tokens and white space for a C program.
 *
 * The macros ccXxxx (e.g. ccFor, ccAnd) allow a tree to be created with the
 * redundant parts omitted.  These are represented as zeros in the tree and can
 * be inferred from context for printing.
 *
 * See cparse.y for the details of how each of these constructors is used to
 * represent the corresponding C phrases.
 */

#ifndef _CCODE_H_
#define _CCODE_H_

#include "cscan.h"
#include <stdio.h>

#ifdef __STDC__
#  define	_of(x)	x
#else
#  define	_of(x)	()
#endif

#define NARY	9


typedef enum {
	CC_Token,	  /* a, "hello", 12, struct,... */

	CC_TypeQual,	  /* const, volatile */
	CC_TypeSpec,	  /* int, double, ... */
	CC_SDef,	  /* struct id {..}, union id {..}, enum id {..} */
	CC_SRef,	  /* struct id, union id, enum id */
	CC_StoClass,	  /* extern, typedef, ... */
	CC_Infix,  	  /* a+b, a && b, ... */
	CC_Prefix,  	  /* -a, *a, ++a, sizeof a, ... */
	CC_Postfix,	  /* a++, ... */

	CC_Param,	  /* f(int a, ...) { } */
	CC_Decl,	  /* extern int *a; */
	CC_FDef,	  /* extern int f() { } */
	CC_ARef, 	  /* a[i]  */
	CC_FCall,	  /* a(x)  */
	CC_Cast,          /* (t) a */
	CC_Quest,         /* a ? b : c */
	CC_Type,	  /* T in: sizeof(T), (T) e */
	CC_Init,	  /* {1, 2, 4, ...} */
	CC_PtrStar,	  /* *const**volatile* */
	CC_PtrDecl,       /* int *x */
	CC_BitField,	  /* a: 4 */
	CC_Seq,           /* a b ... */
	CC_Paren,  	  /* (a) */

	CC_Compound,	  /* {d s} */
	CC_Stat,	  /* [E];  */
	CC_Labeled,	  /* id: S */
	CC_Case,	  /* case E: S */
	CC_Default,	  /* default: S */
	CC_If,		  /* if (E) S [else S] */
	CC_Switch,	  /* switch (E) S */
	CC_While,	  /* while (E) S */
	CC_Do,	  	  /* do S while (E); */
	CC_For,		  /* for ([E]; [E]; [E]) S */
	CC_Goto,	  /* goto id; */
	CC_Continue,	  /* continue; */
	CC_Break,	  /* break; */
	CC_Return,	  /* return [E] ; */

	CC_ToDo
} CCodeTag;


typedef union ccode	*CCode;

struct ccodeHdr {
	CCodeTag	nodeKind;
	CTokTag		opKind;
};

struct ccodeNode {
	struct ccodeHdr	hdr;
	unsigned short	argc;
	CCode		argv[NARY];
};

struct ccodeToken {
	struct ccodeHdr	hdr;
	char		*neutral;
	char		*text;
};

union ccode {
	struct ccodeHdr		hdr;
	struct ccodeNode	node;
	struct ccodeToken	token;
};

#define ccNewNode0(C,T)			 ccNewNode(0,C,T, 0,0,0,0,0,0,0,0,0)
#define ccNewNode1(C,T,a)                ccNewNode(1,C,T,a, 0,0,0,0,0,0,0,0)
#define ccNewNode2(C,T,a,b)              ccNewNode(2,C,T,a,b, 0,0,0,0,0,0,0)
#define ccNewNode3(C,T,a,b,c)            ccNewNode(3,C,T,a,b,c, 0,0,0,0,0,0)
#define ccNewNode4(C,T,a,b,c,d)          ccNewNode(4,C,T,a,b,c,d, 0,0,0,0,0)
#define ccNewNode5(C,T,a,b,c,d,e)        ccNewNode(5,C,T,a,b,c,d,e, 0,0,0,0)
#define ccNewNode6(C,T,a,b,c,d,e,f)      ccNewNode(6,C,T,a,b,c,d,e,f, 0,0,0)
#define ccNewNode7(C,T,a,b,c,d,e,f,g)    ccNewNode(7,C,T,a,b,c,d,e,f,g, 0,0)
#define ccNewNode8(C,T,a,b,c,d,e,f,g,h)  ccNewNode(8,C,T,a,b,c,d,e,f,g,h, 0)
#define ccNewNode9(C,T,a,b,c,d,e,f,g,h,i)ccNewNode(9,C,T,a,b,c,d,e,f,g,h,i )

/******************************************************************************
 *
 * Generic formers corresponding to C phrases.
 *
 *****************************************************************************/

#define ccNewStoClass(t,c) 		ccNewNode1(CC_StoClass,t, c)
#define ccNewTypeQual(t,q) 		ccNewNode1(CC_TypeQual,t, q)
#define ccNewTypeSpec(t,s)   		ccNewNode1(CC_TypeSpec,t, s)
#define ccNewSRef(t,k,i)        	ccNewNode2(CC_SRef,    t, k,i)
#define ccNewSDef(t,k,i,l,d,r)		ccNewNode5(CC_SDef,    t, k,i,l,d,r)
#define ccNewInfix(t,a,o,b)  		ccNewNode3(CC_Infix,   t, a,o,b)   
#define ccNewPrefix(t,o,a)   		ccNewNode2(CC_Prefix,  t, o,a)	   
#define ccNewPostfix(t,a,o)  		ccNewNode2(CC_Postfix, t, a,o)	  
#define ccNewParam(t,l,c,d)		ccNewNode3(CC_Param,   t, l,c,d)
#define ccNewDecl(x,s,d,z)              ccNewNode3(CC_Decl,    0, s,d,z)
#define ccNewFDef(x,s,h,d,b)		ccNewNode4(CC_FDef,    0, s,h,d,b)
#define ccNewARef(x,a,l,b,r) 		ccNewNode4(CC_ARef,    0, a,l,b,r)
#define ccNewFCall(x,a,l,b,r)		ccNewNode4(CC_FCall,   0, a,l,b,r) 
#define ccNewCast(x,l,a,r,b) 		ccNewNode4(CC_Cast,    0, l,a,r,b)
#define ccNewQuest(x,a,q,b,k,c)		ccNewNode5(CC_Quest,   0, a,q,b,k,c)
#define ccNewType(x,s,d)		ccNewNode2(CC_Type,    0, s,d)
#define ccNewInit(x,l,i,c,r)            ccNewNode4(CC_Init,    0, l,i,c,r)
#define ccNewPtrStar(x,s,q,p)		ccNewNode3(CC_PtrStar, 0, s,q,p)
#define ccNewPtrDecl(x,p,d)		ccNewNode2(CC_PtrDecl, 0, p,d)
#define ccNewBitField(x,a,c,b)          ccNewNode3(CC_BitField,0, a,c,b)
#define ccNewSeq(x,a,b)			ccNewNode2(CC_Seq,     0, a,b)
#define ccNewParen(x,l,a,r)		ccNewNode3(CC_Paren,   0, l,a,r) 
#define ccNewCompound(x,l,d,s,r)	ccNewNode4(CC_Compound,0, l,d,s,r)       
#define ccNewStat(x,e,z)      		ccNewNode2(CC_Stat,    0, e,z)       
#define ccNewLabeled(x,i,k,s) 		ccNewNode3(CC_Labeled, 0, i,k,s)    
#define ccNewCase(x,c,e,k,s)  		ccNewNode4(CC_Case,    0, c,e,k,s) 
#define ccNewDefault(x,d,k,s)   	ccNewNode3(CC_Default, 0, d,k,s)  
#define ccNewGoto(x,g,i,z)  		ccNewNode3(CC_Goto,    0, g,i,z) 
#define ccNewContinue(x,c,z)		ccNewNode2(CC_Continue,0, c,z)	 
#define ccNewBreak(x,b,z)   		ccNewNode2(CC_Break,   0, b,z)	
#define ccNewReturn(x,r,e,z)		ccNewNode3(CC_Return,  0, r,e,z)
#define ccNewIf(x,i,l,b,r,t,e,s)	ccNewNode7(CC_If,      0, i,l,b,r,t,e,s)   
#define ccNewSwitch(x,s,l,e,r,b)	ccNewNode5(CC_Switch,  0, s,l,e,r,b) 
#define ccNewWhile(x,w,l,b,r,s) 	ccNewNode5(CC_While,   0, w,l,b,r,s)
#define ccNewDo(x,d,s,w,l,e,r,z)	ccNewNode7(CC_Do,      0, d,s,w,l,e,r,z)
#define ccNewFor(x,f,l,a,h,b,k,c,r,s)	ccNewNode9(CC_For,     0,f,l,a,h,b,k,c,r,s)

/******************************************************************************
 *
 * Specific formers corresponding to particular C statements or expressions.
 *
 *****************************************************************************/

#define ccAuto()	 ccNewStoClass(CTOK_Auto,     0)     /* auto         */
#define ccRegister()	 ccNewStoClass(CTOK_Register, 0)     /* register     */
#define ccStatic()	 ccNewStoClass(CTOK_Static,   0)     /* static       */
#define ccExtern()	 ccNewStoClass(CTOK_Extern,   0)     /* extern       */
#define ccTypedef()	 ccNewStoClass(CTOK_Typedef,  0)     /* typedef      */
#define ccConst()	 ccNewTypeQual(CTOK_Const,    0)     /* const        */
#define ccVolatile()	 ccNewTypeQual(CTOK_Volatile, 0)     /* volatile     */
#define ccVoid()	 ccNewTypeSpec(CTOK_Void,     0)     /* void         */
#define ccChar()	 ccNewTypeSpec(CTOK_Char,     0)     /* char         */
#define ccShort()	 ccNewTypeSpec(CTOK_Short,    0)     /* short        */
#define ccInt() 	 ccNewTypeSpec(CTOK_Int,      0)     /* int          */
#define ccLong()	 ccNewTypeSpec(CTOK_Long,     0)     /* long         */
#define ccFloat()	 ccNewTypeSpec(CTOK_Float,    0)     /* float        */
#define ccDouble()	 ccNewTypeSpec(CTOK_Double,   0)     /* double       */
#define ccSigned()	 ccNewTypeSpec(CTOK_Signed,   0)     /* signed       */
#define ccUnsigned()	 ccNewTypeSpec(CTOK_Unsigned, 0)     /* unsigned     */
#define ccStructRef(i)	 ccNewSRef    (CTOK_Struct,0,i)      /* struct a     */
#define ccStructDef(i,b) ccNewSDef    (CTOK_Struct,0,i,0,b,0)/* struct {}    */
#define ccUnionRef(i)	 ccNewSRef    (CTOK_Union, 0,i)      /* union  a     */
#define ccUnionDef(i,b)	 ccNewSDef    (CTOK_Union, 0,i,0,b,0)/* union  {}    */
#define ccEnumRef(i)	 ccNewSRef    (CTOK_Enum,  0,i)      /* enum   a     */
#define ccEnumDef(i,b)	 ccNewSDef    (CTOK_Enum,  0,i,0,b,0)/* enum   {}    */
#define ccVaParam(d)     ccNewParam   (CTOK_DDDot, d,0,0)    /* f(d,...) { } */
#define ccParam(d)	 ccNewParam   (0,          d,0,0)    /* f(int a) { } */
#define ccDecl(s,d)	 ccNewDecl    (0, s,d,0)             /* extern int a */
#define ccFDef(s,h,d,b)	 ccNewFDef    (0, s,h,d,b)           /* void f() {}  */
#define ccSeq(a,b)	 ccNewSeq     (0, a, 0, b)           /* a b          */
#define ccCompound(d,s)	 ccNewCompound(0, 0,d,s,0)           /* {d s}        */
#define ccStat(e)        ccNewStat    (0, e, 0)              /* e;           */
#define ccLabeled(i,s)   ccNewLabeled (0, i,0,s)             /* i: s         */
#define ccCase(e,s)      ccNewCase    (0, 0,e,0,s)           /* case e: s    */
#define ccDefault(s)     ccNewDefault (0, 0,0,s)             /* default:s    */
#define ccGoto(i)        ccNewGoto    (0, 0,i,0)             /* goto id;     */
#define ccContinue()     ccNewContinue(0, 0,0)	             /* continue;    */
#define ccBreak()        ccNewBreak   (0, 0,0)	             /* break;       */
#define ccReturn(e)      ccNewReturn  (0, 0,e,0)             /* return e;    */
#define ccIf(b,t,e)      ccNewIf      (0, 0,0,b,0,t,0,e)     /* if(b) t else */
#define ccSwitch(e,b)    ccNewSwitch  (0, 0,0,e,0,b)         /* switch(e) s  */
#define ccWhile(b,s)     ccNewWhile   (0, 0,0,b,0,s)         /* while (b) s  */
#define ccDo(s,e)        ccNewDo      (0, 0,s,0,0,e,0,0)     /* do s while   */
#define ccFor(a,b,c,s)   ccNewFor     (0, 0,0,a,0,b,0,c,0,s) /* for(a;b;c) s */
#define ccType(r,d)	 ccNewType    (0, r,d)		     /* int (*)()    */	
#define ccBitField(a,b)  ccNewBitField(0, a,0,b)	     /* a : 4        */
#define ccInit(i)        ccNewInit    (0, 0,i,0,0)           /* {i}          */
#define ccPtrStar(q,p)   ccNewPtrStar (0, 0,q,p)             /* * const **   */
#define ccPtrDecl(p,d)   ccNewPtrDecl (0, p,d)		     /* int *a	     */
#define ccARef(a,b)	 ccNewARef    (0, a,0,x,0)           /* a[b]         */
#define ccFCall(a,b)	 ccNewFCall   (0, f,0,x,0)           /* a(b)         */
#define ccCast(a,b)	 ccNewCast    (0, 0,a,0,b)           /* (a) b        */
#define ccQuest(a,b,c)   ccNewQuest   (0, a,0,b,0,c)         /* a ? b : c    */
#define ccParen(a)	 ccNewParen   (0, 0,a,0)             /* (a)          */
#define ccComma(a,b)     ccNewInfix(CTOK_Comma,    a,0,b)    /* a, b         */
#define ccAsst(a,b)      ccNewInfix(CTOK_Asst,     a,0,b)    /* a  =  b      */
#define ccStarAsst(a,b)  ccNewInfix(CTOK_StarAsst, a,0,b)    /* a *=  b      */
#define ccDivAsst(a,b)   ccNewInfix(CTOK_DivAsst,  a,0,b)    /* a /=  b      */
#define ccModAsst(a,b)   ccNewInfix(CTOK_ModAsst,  a,0,b)    /* a %=  b      */
#define ccPlusAsst(a,b)  ccNewInfix(CTOK_PlusAsst, a,0,b)    /* a +=  b      */
#define ccMinusAsst(a,b) ccNewInfix(CTOK_MinusAsst,a,0,b)    /* a -=  b      */
#define ccUShAsst(a,b)	 ccNewInfix(CTOK_UShAsst,  a,0,b)    /* a <<= b      */
#define ccDShAsst(a,b)	 ccNewInfix(CTOK_DShAsst,  a,0,b)    /* a >>= b      */
#define ccAndAsst(a,b)   ccNewInfix(CTOK_AndAsst,  a,0,b)    /* a &=  b      */
#define ccXorAsst(a,b)   ccNewInfix(CTOK_XorAsst,  a,0,b)    /* a ^=  b      */
#define ccOrAsst(a,b)    ccNewInfix(CTOK_OrAsst,   a,0,b)    /* a |=  b      */
#define ccLOr(a,b)   	 ccNewInfix(CTOK_LOr,      a,0,b)    /* a ||  b      */
#define ccLAnd(a,b)  	 ccNewInfix(CTOK_LAnd,     a,0,b)    /* a &&  b      */
#define ccOr(a,b)    	 ccNewInfix(CTOK_Or,       a,0,b)    /* a | b        */
#define ccXor(a,b)    	 ccNewInfix(CTOK_Xor,      a,0,b)    /* a ^ b        */
#define ccAnd(a,b)    	 ccNewInfix(CTOK_And,      a,0,b)    /* a & b        */
#define ccEQ(a,b)    	 ccNewInfix(CTOK_EQ,       a,0,b)    /* a == b       */
#define ccNE(a,b)    	 ccNewInfix(CTOK_NE,       a,0,b)    /* a != b       */
#define ccLT(a,b)    	 ccNewInfix(CTOK_LT,       a,0,b)    /* a <  b       */
#define ccLE(a,b)    	 ccNewInfix(CTOK_LE,       a,0,b)    /* a <= b       */
#define ccGT(a,b)    	 ccNewInfix(CTOK_GT,       a,0,b)    /* a >  b       */
#define ccGE(a,b)    	 ccNewInfix(CTOK_GE,       a,0,b)    /* a >= b       */
#define ccUSh(a,b)	 ccNewInfix(CTOK_USh,      a,0,b)    /* a << b       */
#define ccDSh(a,b)	 ccNewInfix(CTOK_DSh,      a,0,b)    /* a >> b       */
#define ccPlus(a,b)  	 ccNewInfix(CTOK_Plus,     a,0,b)    /* a + b        */
#define ccMinus(a,b) 	 ccNewInfix(CTOK_Minus,    a,0,b)    /* a - b        */
#define ccStar(a,b)      ccNewInfix(CTOK_Star,     a,0,b)    /* a * b        */
#define ccDiv(a,b)	 ccNewInfix(CTOK_Div,      a,0,b)    /* a / b        */
#define ccMod(a,b)	 ccNewInfix(CTOK_Mod,      a,0,b)    /* a % b        */
#define ccDot(a,b)	 ccNewInfix(CTOK_Dot,      a,0,b)    /* a . b        */
#define ccPointsTo(a,b)	 ccNewInfix(CTOK_PointsTo, a,0,b)    /* a->b         */
#define ccSizeof(a)  	 ccNewPrefix(CTOK_Sizeof,  0,a)      /* sizeof a     */
#define ccNot(a)	 ccNewPrefix(CTOK_Not,     0,a)      /* ~a           */
#define ccLNot(a)	 ccNewPrefix(CTOK_LNot,    0,a)      /* !a           */
#define ccPreAnd(a)  	 ccNewPrefix(CTOK_And,     0,a)      /* &a           */
#define ccPreStar(a) 	 ccNewPrefix(CTOK_Star,    0,a)      /* *a           */
#define ccPrePlus(a) 	 ccNewPrefix(CTOK_Plus,    0,a)      /* +a           */
#define ccPreMinus(a) 	 ccNewPrefix(CTOK_Minus,   0,a)      /* -a           */
#define ccPreInc(a)	 ccNewPrefix(CTOK_Inc,     0,a)      /* ++a          */
#define ccPreDec(a)	 ccNewPrefix(CTOK_Dec,     0,a)      /* --a          */
#define ccPostInc(a)	 ccNewPostFix(CTOK_Inc,    a,0)      /* a++          */
#define ccPostDec(a)	 ccNewPostFix(CTOK_Dec,    a,0)      /* a--          */

/******************************************************************************
 *
 * Operations
 *
 *****************************************************************************/

extern CCode	ccNewToken      _of((CTokTag,char *, char *));
extern CCode	ccNewNode	_of((int n,   CCodeTag, CTokTag,
				     	CCode, CCode, CCode, CCode, CCode, 
					CCode, CCode, CCode, CCode));

extern int	ccPrint		_of((FILE *,  CCode));
extern CCode	ccDoTypedefs	_of((CCode));

#endif
