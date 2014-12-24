/*****************************************************************************
 *
 * axlobs.h: Compiler-specific types.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file provides forward declarations for the various compiler types.
 *
 * Without this, it would be impossible to use typedefs -- the compiler
 * types are mutually recursive. 
 */

#ifndef _AXLOBS_H_
#define _AXLOBS_H_

#include "axlgen.h"

/*
 * Forward declarations of the types
 */
typedef struct srcLine *           SrcLine;
typedef struct symbol *            Symbol;
typedef struct comsg *		   CoMsg;
typedef struct token *             Token;
typedef struct doc *		   Doc;
typedef union  abSyn *             AbSyn;
typedef union  abSyn *             Sefo;
typedef struct abBind *		   AbBind;
typedef	struct abSub *		   AbSub;
typedef struct abLogic *	   AbLogic;
typedef struct fvar *		   FreeVar;
typedef struct syme *              Syme;
typedef struct tform *		   TForm;
typedef struct tposs *		   TPoss;
typedef struct tconst *		   TConst;
typedef struct tqual *		   TQual;
typedef union  foam *              Foam;
typedef struct foamBox *      	   FoamBox;
typedef struct lib *		   Lib;
typedef struct archive *	   Archive;
typedef struct ar_entry *	   ArEntry;
typedef struct stabEntry *         StabEntry;
typedef struct stabLevel *         StabLevel;
typedef struct StabLevelListCons * Stab;
typedef struct optInfo *           OptInfo;
typedef struct flowGraph *	   FlowGraph;
typedef struct basicBlock *	   BBlock;
typedef struct dflowInfo *         DFlowInfo;
typedef struct depDag *		   DepDag;
typedef ULong			   SefoMark;

typedef struct _UdInfo *	   UdInfo;
typedef struct _ExpInfo * 	   ExpInfo;
typedef struct _InvInfo *	   InvInfo;
typedef union _SImpl *		   SImpl;

typedef struct utform * UTForm;
typedef struct utype *  UType;

#if EDIT_1_0_n1_AB
typedef struct foamuses_struct *   FoamUses;
typedef struct ssa_struct *        SSA;
typedef struct domtree_struct *    DominatorTree;
#endif


/*
 * Declare necessary lists
 */
DECLARE_LIST(Hash);
DECLARE_LIST(Symbol);
DECLARE_LIST(SExpr);

DECLARE_LIST(CoMsg);
DECLARE_LIST(SrcLine);
DECLARE_LIST(Token);
DECLARE_LIST(AbSyn);
DECLARE_LIST(AbBind);
DECLARE_LIST(Doc);
DECLARE_LIST(TForm);
DECLARE_LIST(TConst);
DECLARE_LIST(TQual);
DECLARE_LIST(Stab);
DECLARE_LIST(StabLevel);
DECLARE_LIST(Syme);
DECLARE_LIST(Sefo);
DECLARE_LIST(Table);
DECLARE_LIST(Foam);
DECLARE_LIST(AInt);
DECLARE_LIST(CCode);
DECLARE_LIST(UdInfo);
DECLARE_LIST(DepDag);
DECLARE_LIST(UTForm);

DECLARE_LIST(SymeList);

/*
 * Include files which give meaning to the above declarations.
 */
#include "absyn.h"
#include "foam.h"

/*
 * Tags for labelled storage.  These continue where "axlgen.h" ends.
 */
# define OB_CoMsg		(OB_AXLGEN_LIMIT +  0)
# define OB_SrcLine		(OB_AXLGEN_LIMIT +  1)
# define OB_Token		(OB_AXLGEN_LIMIT +  2)
# define OB_Doc			(OB_AXLGEN_LIMIT +  3)
# define OB_AbSyn		(OB_AXLGEN_LIMIT +  4)
# define OB_AbBind		(OB_AXLGEN_LIMIT +  5)
# define OB_Stab		(OB_AXLGEN_LIMIT +  6)
# define OB_Syme		(OB_AXLGEN_LIMIT +  7)
# define OB_TForm		(OB_AXLGEN_LIMIT +  8)
# define OB_TPoss		(OB_AXLGEN_LIMIT +  9)
# define OB_TConst		(OB_AXLGEN_LIMIT + 10)
# define OB_TQual		(OB_AXLGEN_LIMIT + 11)
# define OB_Foam		(OB_AXLGEN_LIMIT + 12)
# define OB_Lib			(OB_AXLGEN_LIMIT + 13)
# define OB_Archive		(OB_AXLGEN_LIMIT + 14)
# define OB_LIMIT		(OB_AXLGEN_LIMIT + 15)

/*
 * Structure containing information about each type.
 */
struct ob_info {
	UByte	      code;
	String        str;
	Bool 	      hasPtrs;
};

extern struct ob_info   obInfo[];

extern void obInit  (void);
extern int  obPrint (FILE *, Pointer);


#endif

