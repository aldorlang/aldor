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
#include "ccode.h"
#include "comsgdb.h"
#include "list.h"
#include "sexpr.h"

/*
 * Forward declarations of the types
 */
typedef struct srcLine *           SrcLine;
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
DECLARE_LIST(FileName);
DECLARE_LIST(UdInfo);
DECLARE_LIST(DepDag);

DECLARE_LIST(SymeList);

/*
 * Include files which give meaning to the above declarations.
 */
# include "comsg.h"
# include "srcline.h"
# include "token.h"
# include "doc.h"
# include "absyn.h"
# include "abpretty.h"
# include "ablogic.h"
# include "absub.h"
# include "freevar.h"
# include "tform.h"
# include "tfsat.h"
# include "tposs.h"
# include "foam.h"
# include "comsg.h"
# include "fbox.h"

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


/*
 * Symbol information for fast S-Expression IO of compiler types.
 */
union symCoInfoU {
	struct {
		union {
			Pointer	generic;
			AbSyn	macro;
		}     phaseVal;		/* phase varying info */

		AbSynTag abTagVal;
		FoamTag foamTagVal;
	} val;
	MostAlignedType 	align;	/* Force alignment. */
};

extern  union symCoInfoU * symCoInfoNew  (void);

#define symCoInfo(sym)     (&(((union symCoInfoU *) symInfo(sym))->val))
#define symCoInfoInit(sym) (symInfo(sym) = &(symCoInfoNew()->align))


/*****************************************************************************
 *
 * :: File types
 *
 ****************************************************************************/

#define FTYPE_SRC          "as"
#define FTYPE_INCLUDED     "ai"
#define FTYPE_ABSYN        "ap"
#define FTYPE_OLDABSYN     "ax"
#define FTYPE_INTERMED     "ao"
#define FTYPE_FOAMEXPR     "fm"
#define FTYPE_SYMEEXPR	   "asy"
#define FTYPE_LOCK	   "ask"
#define FTYPE_LISP         sxiLispFileType
#define FTYPE_C		   "c"
#define FTYPE_CPP	   "c++"
#define FTYPE_H		   "h"
#define FTYPE_OBJECT	   osObjectFileType
#define FTYPE_AR_OBJ	   "a"
#define FTYPE_AR_INT	   "al"
#define FTYPE_MSG	   "cat"
#define FTYPE_EXEC	   osExecFileType
#define FTYPE_AXLMAINC      "c"

enum ftypeNo {
    FTYPENO_START,
	FTYPENO_OTHER = FTYPENO_START,
	FTYPENO_NONE,

	FTYPENO_SRC,
	FTYPENO_INCLUDED,
	FTYPENO_ABSYN,
	FTYPENO_OLDABSYN,
	FTYPENO_INTERMED,
	FTYPENO_FOAMEXPR,
	FTYPENO_SYMEEXPR,
        FTYPENO_LOCK,
	FTYPENO_LISP,
	FTYPENO_C,
	FTYPENO_CPP,
	FTYPENO_H,
	FTYPENO_OBJECT,
	FTYPENO_AR_OBJ,
	FTYPENO_AR_INT,
	FTYPENO_MSG,
	FTYPENO_EXEC,
	FTYPENO_AXLMAINC,
    FTYPENO_LIMIT
};

typedef Enum(ftypeNo)	FTypeNo;

extern FTypeNo	ftypeNo		(String);
extern String	ftypeString	(FTypeNo);

extern Bool	ftypeEqual	(String,   String);
extern Bool	ftypeIs		(String,   FTypeNo);
extern Bool	ftypeHas	(FileName, FTypeNo);

#endif /* !_AXLOBS_H_ */

