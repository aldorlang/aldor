/*****************************************************************************
 *
 * genc.c: Foam-to-C translation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "compcfg.h"
#include "debug.h"
#include "fluid.h"
#include "fortran.h"
#include "genc.h"
#include "gf_util.h"
#include "of_killp.h"
#include "of_rrfmt.h"
#include "optfoam.h"
#include "store.h"
#include "util.h"
#include "syme.h"
#include "comsg.h"
#include "bigint.h"


/*
 * The following naming conventions are used in this file:
 *   gcc -- function: Foam -> CCode
 *   gc0 -- utility function
 *   gcv -- variable associated with genC
 */

/*****************************************************************************
 *
 * :: Local C code generation structures.
 *
 ****************************************************************************/

typedef struct {
	int		pos;
	int		argc;
	CCode		*stmt;
} *Cstmts;

struct CList {
	CCode		type;
	int		lsize;
	struct Clocals	*locs;
};

typedef struct {
	int		pos;
	int		argc;
	struct CList	*list;
} *Ldecls;

struct Clocals {
	int		index;
	CCode		loc;
	struct Clocals	*next;
};

struct Cdecls {
	int		index;
	CCode		decl;
	struct Cdecls	*next;
};

struct ccBVal_info {
	FoamBValTag	tag;
	CCodeTag	cfun;
	AInt		special;
	String          str;    /* name of function call */
	String		macro;	/* name of macro which returns a value */
}; 

struct ccListHdrFiles {
	char			*fname;
	struct ccListHdrFiles	*next;
};

static struct ccListHdrFiles *ccHdrFileList = 0;

typedef enum { GC_NoCall, GC_OCall, GC_CCall } GcNesting;
extern struct ccBVal_info  ccBValInfoTable[];

extern Bool ccFortran; /* See ccomp.c */


#define ccBValInfo(tag)   (ccBValInfoTable[(int)(tag)-(int)FOAM_BVAL_START])
#define ccBValCFun(tag)   (ccBValInfo(tag).cfun)
#define ccBValSpec(tag)   (ccBValInfo(tag).special)
#define ccBValStr(tag)    (ccBValInfo(tag).str)
#define ccBValMacro(tag)  (ccBValInfo(tag).macro)

/*****************************************************************************
 *
 * :: Global variables used for C code generation.
 *
 ****************************************************************************/

static Foam	gcvProg;		/* Prog being translated */
static int	gcvLvl;			/* Unit/Prog depth */
static Foam	gcvGlo;			/* Unit globals */
static Foam	gcvConst;		/* Unit constants */
static Foam	gcvFluids;		/* Unit Fluids */
static Foam	gcvFmt;			/* Unit formats */
#ifdef NEW_FORMATS
static Foam	gcvParams;		/* Unit parameters */
#endif
static Foam	gcvPar;			/* Prog parameters */
static Foam	gcvLoc;			/* Prog locals */
static Foam	gcvLocFluids;		/* Prog fluids */
static AIntList gcvFluidList;		/* Fluids used */
static Foam	gcvLFmtStk;		/* Prog lexical format stack */
static Foam	gcvDefs;		/* Unit definitions */
static FoamList gcvLexStk = 0;		/* Unit/Prog lexicals stack */
static Bool	gcvIsLeaf;		/* True iff prog is a leaf proc */
static int	gcvIdChars[CHAR_MAX];	/* Array of special print chars */
static int	gcvIdCharc[CHAR_MAX];	/* Array of special print lengths */
static int	gcvNLocs  = 0;		/* Number of locals */
static int	gcvNStmts = 0;		/* Number of statements */
static int	gcvNBInts = 0;		/* Counter for global big ints */
static int	gcvNRRFmt = 0;		/* Counter for global RRFmts */
static int	gcvisInitConst = 0;	/* True if prog is constant 0 */
static int	gcvisStmtFCall = 0;	/* True if stmt is a function call */
static GcNesting gcvCallNesting;	/* Depth of nesting of foam-level calls*/
static CCodeList gcvNestUsed;		/* Vars used in nesting */
static CCodeList gcvNestFree;		/* Vars used in nesting */
static CCode	 gcvSpec;		/* Prog name specifier */
static Cstmts	 gcvStmts;		/* List of statements in a C prog */
static Ldecls	 gcvNewLocs;		/* Lists of locals by type */
struct Clocals	*gcvLocals = 0;		/* List of locals declared in a prog */
static CCodeList gcvGloCC;		/* List of global C variables */
static CCodeList gcvPreProcCC;		/* List of C preprocessor statements */

/* Used in the inizialization */
static CCodeList gcvExportedGloInitCC;	
static CCodeList gcvImportedGloInitCC;	
static CCodeList gcvInitFunDeclsCC;
static CCodeList gcvInitFunCalls0CC;
static CCodeList gcvInitFunCalls1CC;
static CCodeList gcvInitProgCC;

static CCodeList gcvDefCC;		/* List of defined C variables */
static CCodeList gcvBIntCC;		/* List of bigints for init prog */
static CCodeList gcvRRFmtCC;		/* List of RRFmts for init prog */
static char	gcvFloatBuf[MAX_FLOAT_SIZE]; /* Buffer to hold float data */
/*static int	gcvSMax = 2000;*/	/* Maximum number of C statements */
static int	gcvSMax = 0;		/* Maximum number of C statements */
static int	gcvIdLen = 30;		/* Maximum length of C identifier */
static Bool	gcvIdHash = true;	/* Are global id hash codes used */

static Table	gcvRRFmtTable;		/* Table of globalised RRFmts */

/* Tracking Fortran functional parameters */
static	int	gcvNFtnPar;
static	Table	gcvFtnTable;


/*****************************************************************************
 *
 * :: Functions which convert Foam code into C code.
 *
 ****************************************************************************/

local	CCodeList	gccUnit		(Foam, String); /* Return a C code unit */
local	CCode	gccDef		(Foam); /* Return C code for a definition */
local	CCode	gccCmd		(Foam); /* Generate C code for commands */
local	CCode	gccExpr		(Foam); /* Generate C code for expressions */
local	CCode	gccVal		(Foam); /* Generate C code for values */
local	CCode	gccRef		(Foam); /* Generate C code for references */
local	CCode	gccClos		(Foam); /* Create a closure */
local	CCode	gccId		(Foam); /* Create an identifier */
local 	CCode 	gccProgId	(Foam); /* (see func. definition) */
local	CCode	gccGetVar	(Foam); /* Return a C variable */
local	CCode	gccEnvParam	(void); /* Declare an environment parameter */ 
local	CCode	gccUnhandled	(Foam); /* No implementation for this foam */
local	CCode	gccArr		(Foam); /* Return an array */ 
local	CCode	gccArrNew	(Foam); /* Create a new array */ 
local	CCode	gccRecNew	(Foam); /* Create a new record */ 
local	CCode	gccRRecNew	(Foam);	/* Create a new raw record */
local	CCode	gccRRFmt	(Foam);	/* Create raw record index fr fmt */
local	CCode	gccTRNew	(Foam); /* Create a new record */ 
local	CCode	gccRec		(Foam); /* Return a record */ 
local	CCode	gccRRElt	(Foam); /* Element of a raw record */
local	CCode	gccRElt		(Foam); /* Element of a record */
local	CCode	gccIRElt	(Foam); /* Element of a trailing array */
local	CCode	gccTRElt	(Foam); /* Element of a trailing array */
local	CCode	gccAElt		(Foam); /* Element of an array */
local	CCode	gccReturn	(Foam); /* Create a function return call */
local	CCode	gccSeq		(Foam); /* Return a sequence of statements */
local	CCode	gccMFmt		(Foam); /* Generate multi-value call */
local	CCode	gccReturnValues	(Foam); /* Return multiple values */
local	CCode	gccValues	(Foam); /* Generate multiple values */
local	CCode	gccIf		(Foam); /* Generate an if statement */
local	CCode	gccSelect	(Foam); /* Generate a switch statement */
local	CCode	gccBInt		(Foam); /* Generate a big integer */
local	CCode	gccPushEnv	(Foam); /* Push environment onto the stack */
local	CCode	gccEEnv		(Foam); /* Return an outer environment */
local   CCode	gccEInfo	(Foam); /* Information slot of an environment */
local   CCode	gccPRef 	(Foam); /* Information slot of a prog */
local	CCode	gccPCallId	(Foam); /* Foreign function call */
local   CCode   gccFortranPCall (Foam *, int, Foam, CCodeList *); 
                                        /* Generate code for a Fortran PCall */
local	CCode	gccDef0List	(Foam); /* Return C code for init prog */
local	CCode	gc0Protect	(Foam); 
local	CCode 	gc0Throw	(Foam);	

/*****************************************************************************
 *
 * :: Miscellaneous C code generating functions.
 *
 ****************************************************************************/

local	CCodeList	gc0ExternDecls	(String name);
local	CCode	gc0GloDecl	(int);
local	void	gc0ConstDecl	(int);
local	CCode	gc0FluidDecl	(int);
local	void	gc0LexDecl	(int);
local	void	gc0LFmtDecl	(int, Foam);
local	void	gc0LFmtDef	(int);

local	CCode	gc0GetterDecl	(String);
local	CCode	gc0ArgcDecl	(void);
local	CCode	gc0ArgvDecl	(void);
local	CCode	gc0MainDecl	(void);
local	CCode	gc0MainDef	(String);
local 	void	gc0AddExtraModules (void);
local	CCode	gc0ClosInit	(Foam, Foam);
local	CCode	gc0Set		(Foam, Foam);
local	CCode	gc0SetValues	(Foam, Foam);
local   CCode   gc0FortranSet   (Foam, Foam, FoamTag, FoamTag);
local	CCode	gc0SetCatch	(Foam, Foam);
local	CCode	gc0Prog		(Foam, Foam);
local	CCode	gc0Compound	(Foam, Foam, Foam, int, int);
local	CCode	gc0Compress	(CCode);
local	CCode	gc0Param	(Foam, Foam);
local	CCode	gc0LocRef	(Foam, int);

local	CCode	gc0ExportCDef	(String, Foam, int, int);
local	CCode	gc0ExportToFortran(String, Foam, Foam, FtnFunParam, int);
local	CCode	gc0ExportFtnString(CCode, Bool, CCodeList *, CCodeList *,
				CCodeList *, CCodeList *, int *, int *);
local	CCodeList	gc0ExportInit	(String, Foam, int);
local	CCode	gc0FunFoamCall	(Foam, int);
local	CCode	gc0FunCCall0	(Foam, int);
local	CCode	gc0FunOCall0	(Foam, int);
local	CCode	gc0FunPCall0	(Foam, int);
local	CCode	gc0FiCFun	(FoamTag, int, FoamTag *, CCode, int); 
local   CCode 	gc0UnNestCall	(CCode, CCode);
local   CCode   gc0GetTmp       (CCode);
local	CCode	gc0FunBCall	(Foam, int);

local	CCode	gc0SeqStmt	(Foam, int);
local	CCode	gc0Cop		(FoamBValTag, Foam, CCodeTag);
local	CCode	gc0Builtin	(FoamBValTag, Foam);
local	CCode	gc0FCall	(FoamBValTag, Foam);
local	CCode	gc0SIntMod	(Foam, CCodeTag);

local   CCode	gc0SubExpr	(Foam, CCode);
local	Bool	gc0NeedBothCasts(FoamTag, FoamTag);
local   CCode	gc0TryCast	(FoamTag, Foam);
local   CCode	gc0Cast		(FoamTag, Foam);
local	CCode	gc0TypeId	(AInt, AInt);
local	CCode	gc0IdDecl	(Foam, FoamTag, Foam, int, int);
local	CCode	gc0IdCDecl	(Foam, CCode);
local   void	gc0IdFortranDecl(Foam, CCode *, CCode *);
local 	CCode	gc0GloIdDecl	(Foam, int);
local	CCode 	gc0FluidSet	(Foam, Foam);
local	CCode	gc0FluidRef	(Foam);
local	CCode	gc0PushFluid	(void);
local	CCode	gc0PopFluid	(void);
local	CCode	gc0GetFluid	(AInt);
local	CCode	gc0AddFluid	(AInt);
local	CCode	gc0MultVarId	(String, int, String);
local	CCode	gc0VarId	(String, int);

local	CCode	gc0RRFmt	(CCode, Foam);

local	CCodeList gc0Levels	(int, int, int, int);
local	CCode	gc0LexRef	(int, int);
local	CCode	gc0EnvNext	(int, int);
local	CCode	gc0EnvMake	(int);
local	CCode	gc0EnvPush	(int);
local	CCode	gc0EnvLevel	(int, int);
local	CCode	gc0EnvRef	(int);
local	CCode	gc0EEltNext	(Foam, int);
local	CCode	gc0EEnv		(CCode, int);

local 	CCode	gc0GenModuleInitFun	(String, Bool, int);
local 	CCode	gc0DeclModuleInitFun	(String, int);

local 	CCode	gc0SpecialSFloWord (Foam,AInt,CCode);

/*****************************************************************************
 *
 * :: Other miscellaneous utility functions.
 *
 ****************************************************************************/

local   String  gc0StompOffIncludeFile(String str, FoamProtoTag p);
local	void	gc0CheckBVals		(void);
local   FoamTag	gc0ExprType		(Foam foam);
local	Foam	gc0GetDecl		(Foam);
local	int	gc0IsDecl		(Foam);
local   int	gc0ValidIdInBuf		(Buffer, String);
local	int	gc0IdHashInBuf		(Buffer, String);	
local	void	gc0InitSpecialChars	(void);
local	void	gc0AddDecl		(CCode, int);
local	void	gc0AddLocal		(CCode, int, int);
local	void	gc0CreateGloList	(String);
local	void	gc0CreateLocList	(Foam);
local   void 	gc0AddUnSortedLocal	(CCode);
local	void	gc0NewStmtInit		(void);
local	void	gc0AddTopLevelStmt		(Cstmts, CCode);
local	void	gc0NewLocsInit		(void);
local	void	gc0NewLocals		(CCode);
extern	int	gc0NumVals		(Foam);
local	int	gc0MaxLevel		(int);
local	int	gc0IsNewHeader		(String);
local 	void 	gc0AddHeaderIfNeeded	(String);

local   CCode	gc0ModuleInitFun	(String, int);	
local   CCode	gc0ListOf		(CCodeTag, CCodeList);
local   void    gc0AddLineFun		(CCodeList *, CCode);
local	Bool	gc0IsReturn		(CCode);
local   CCode   gc0Decl			(Foam, CCode);

local void	gc0InitDeclList();
local CCodeList gc0FiniDeclList();

local   Bool   		gc0IsModifiableFortranArg	(Foam);
local   String  	gc0GetFortranArgName    (Foam);
local   int     	gc0GetNumModFortranArgs	(Foam);
local   String  	gc0GenFortranName	(String);
local   FortranType	gc0GetFortranType		(Foam);
local	Foam		gc0GetFortranRetFm		(Foam);
local   FortranType	gc0GetFortranRetType		(Foam);
local	CCode		gccFtnXLstring(Foam, FortranType, Bool, CCode*);
local	CCode		gccFtnStringArray(Foam, CCode *);
local	CCode		gccFtnFnParam(Foam, FtnFunParam, CCode *);

local	void		gc0FtnExFunPar(String, Foam, Foam, AInt, AInt);

local	FtnFunParam	gc0FtnFunParam(String, AInt);
local	CCode		gc0FtnFunClosDeclare(FtnFunParam);
local	CCode		gc0FtnFunClosDefine(FtnFunParam);
local	CCode		gc0FtnFunDeclare(FtnFunParam, CCode);
local	void		ftnFunParamInit(void);
local	void		ftnFunParamFinish(void);
local	Hash		ftnFunParamHash(FtnFunParam);
local	Bool		ftnFunParamEqual(FtnFunParam, FtnFunParam);
local	void		ftnFunParamFree(FtnFunParam);

local	Foam		gc0AddExplicitReturn(Foam);

/*****************************************************************************
 *
 * :: Useful C code macros.
 *
 ****************************************************************************/

#define NOT_SET		(-1)
#define NOT_CHANGED	(-2)
#define NOT_PRINTABLE	(-3)

#define gcFiWord  "FiWord"
#define gcFiArb   "FiArb"
#define gcFiPtr   "FiPtr"
#define gcFiBool  "FiBool"
#define gcFiByte  "FiByte"
#define gcFiHInt  "FiHInt"
#define gcFiSInt  "FiSInt"
#define gcFiChar  "FiChar"
#define gcFiArr   "FiArr"
#define gcFiRec   "FiRec"
#define gcFiRRec  "FiRRec"
#define gcFiProg  "FiProg"
#define gcFiFun   "FiFun"
#define gcFiBInt  "FiBInt"
#define gcFiSFlo  "FiSFlo"
#define gcFiDFlo  "FiDFlo"
#define gcFiEnv   "FiEnv"
#define gcFiClos  "FiClos"
#define gcFiComplexSF "FiComplexSF"
#define gcFiComplexDF "FiComplexDF"
#define gcFiFluid "FiFluid"
#define gcFiFluidStack "FiFluidStack"
#define gcFiFluidStackLVar "localStack"
#define gcFiFluidStackGVar "fiGlobalFluidStack"
#define gcFiNil   "fiNil"	/* The Nil value */
#define gcFmtName "Fmt"
#define gcTFmtName "TFmt"
#define gcFmtType "PFmt"
#define	gcFiInitModulePrefix	"INIT_"
#define	gcFiTRTail	"tail"

#define gc0AddLine(cc, c)   gc0AddLineFun(&(cc), c)

#define ccoStatAsst(ccl, ccr) ccoStat(ccoAsst(ccl, ccr))
#define ccoTypeIdOf(s)	      ccoTypedefId(ccoIdOf(s))

#define	gcFiNARY	"fiNARY"
#define gcFiNew(s,f)    ccoFCall(ccoIdOf("fiNew"), ccoStructRef(gc0VarId(s,f)))
#define gcFi0New(s,f,t)     	ccoFCall(ccoIdOf("fi0New"), \
	ccoMany2(ccoStructRef(gc0VarId(s,f)), ccoIdOf(t)))
#define gcFi0RecNew(s,f,t)     	ccoFCall(ccoIdOf("fi0RecNew"), \
	ccoMany2(ccoStructRef(gc0VarId(s,f)), ccoIdOf(t)))

#define gcFiNARYNew(s1, s2,f,n)      ccoFCall(ccoIdOf("fiNARYNew"), \
					      ccoMany3(ccoStructRef(gc0VarId(s1,f)), \
						       ccoStructRef(gc0VarId(s2,f)), \
						       n))
#define gcFiEnvPush(s,e) ccoFCall(ccoIdOf("fiEnvPush"), ccoMany2(s, e))
#define gcFiEnvNext(c)   ccoFCall(ccoIdOf("fiEnvNext"), c)
#define gcFiEnvLevel(c)  ccoFCall(ccoIdOf("fiEnvLevel"), c)
#define gcFiEnvInfo(e)	 ccoFCall(ccoIdOf("fiEnvInfo"), e)
#define gcFiProgHashCode(e)  ccoFCall(ccoIdOf("fiProgHashCode"), e)
#define gcFiEEnsure(e)	     ccoFCall(ccoIdOf("fiEnvEnsure"), e)
#define gcFiFree(o)	     ccoFCall(ccoIdOf("fiFree"), o)
#define gcFiHalt(hc)	     ccoFCall(ccoIdOf("fiHalt"), ccoCast(ccoTypeIdOf(gcFiSInt), hc))

#define gcFiGetFluid(name)	ccoFCall(ccoIdOf("fiGetFluid"), name)
#define gcFiAddFluid(name)	ccoFCall(ccoIdOf("fiAddFluid"), name)
#define gcFiFluidValue(id)	ccoFCall(ccoIdOf("fiFluidValue"), id)
#define gcFiSetFluid(id, value) ccoFCall(ccoIdOf("fiSetFluid"), ccoMany2(id, value))

#define FOREIGN_INCLUDE_SEPARATOR   '-'

/* These two are no longer used - see gc0FtnFunParam() */
#define gcFortranFnParamSuffix "_fnparam"
#define gcFortranClosSuffix "_Clos"

/* Names of globals/statics for Fortran functional parameters */
#define gcFortranPar	("GFtnPar")
#define gcFortranClo	("GFtnClo")

#define gcFiNMaxEnvLevel   5
#define gcFiNMaxCCall	   5
#define gcFiEnvLevelN(i,c) ccoFCall(gc0VarId("fiEnvLevel", i), c)
#define gcFiCCallN(i,c)    ccoFCall(gc0VarId("fiCCall", i), c)

#define isMacro(f) (((f) == FOAM_BVal || (f) == FOAM_BCall))

#define isStmt(p) (((p) != FOAM_Select) && \
		   ((p) != FOAM_Return) && \
		   ((p) != FOAM_Seq) && \
		   ((p) != FOAM_If) && \
		   ((p) != FOAM_Label) && \
		   ((p) != FOAM_Goto) && \
		   ((p) != FOAM_NOp))

/*
 * These macros should be moved to other files, e.g. foam.h and ccode.h
 */

#define gc0EmptyEnv(x) ((x == emptyFormatSlot) || \
			(foamDDeclArgc(gcvFmt->foamDFmt.argv[x]) == 0))

#define gc0EmptyFormat(fmt) \
	(gc0EmptyEnv(fmt) || (fmt) == envUsedSlot || \
	 (foamDDeclArgc(gcvFmt->foamDFmt.argv[fmt]) < 1))

#define ccIdInfo(tag)   (ccSpecCharIdTable[tag])
#define ccIdChar(tag)   (ccIdInfo(tag).ch)
#define ccIdStr(tag)    (ccIdInfo(tag).str)

#define USE_MACROS

#define	gc0OverSMax()		(gcvSMax > 0 && gcvNStmts > gcvSMax)

void
genCSetSMax(int n)
{
	gcvSMax = n;
	if (gcvSMax < 0) gcvSMax = 1;
}

#define	gc0UnderIdLen(buf,i)	\
	(gcvIdLen == 0 || bufPosition(buf) + gcvIdCharc[i] <= gcvIdLen)

void
genCSetIdLen(int n)
{
	gcvIdLen = n;
	if (gcvIdLen < 0) gcvIdLen = 1;
}

void
genCSetIdHash(Bool flag)
{
	gcvIdHash = flag;
}


/*****************************************************************************
 *
 * :: Developer options (-Wtrace-cfuns)
 *
 ****************************************************************************/

static Bool gc0TraceFuns = false;

void gencSetTraceFuns(Bool flag)
{
	gc0TraceFuns = flag;
}

Bool
gencTraceFuns(void)
{
	return gc0TraceFuns;
}

/*****************************************************************************
 *
 * :: Top level entry point for C generation.
 *    First generate a C code tree, then write it to the file.
 *
 ****************************************************************************/

CCodeList
genC(Foam foam, String name)
{
	CCodeList	ccode;

	gc0CheckBVals();

	ccode = gccUnit(foam, name);

	return ccode;
}

/*****************************************************************************
 *
 * :: Generate a list of C code trees from a given Foam Unit.
 *
 ****************************************************************************/

local CCodeList
gccUnit(Foam foam, String name)
{
	CCodeList	ccode;

	assert(foamTag(foam) == FOAM_Unit);

	gcvLvl	  = 0;
	gcvGlo	  = foamUnitGlobals(foam);
	gcvConst  = foamUnitConstants(foam);
	gcvFluids  = foamUnitFluids(foam);
	gcvFmt	  = foam->foamUnit.formats;
	gcvLexStk = listCons(Foam)(foamUnitLexicals(foam), listNil(Foam));
#ifdef NEW_FORMATS
	gcvParams = foamUnitParams(foam);
#endif

	gc0InitSpecialChars();
	gcvDefs = foam->foamUnit.defs;
	assert(foamTag(gcvDefs) == FOAM_DDef);

	ccode = gc0ExternDecls(name);

	return ccode;
}

/*****************************************************************************
 *
 * :: Create declarations and definitions for each node of the C code tree
 *    and break the C code tree into a list of trees if the C unit becomes
 *    excessively large.
 *
 ****************************************************************************/

local CCodeList
gc0ExternDecls(String name)
{
	int		i, n, stmtCounter, listLen;
	int		nLexs, nLFmts, nDefs = 0, nStmts = 0;
	int		nBrothers = 0;
	CCode		ccExtD, ccExtH, c, ccglo, ccd, ccrest, ccdefs, def0c;
	CCode		ccGloDefs;
	CCode		ccPreProc;
	CCodeList	allcode = listNil(CCode);
	CCodeList	code = listNil(CCode);
	CCodeList	hcode = listNil(CCode);
	CCodeList       decls;
	CCodeList	constDefs = listNil(CCode);
	CCodeList	gloDefs = listNil(CCode);

	gcvGloCC	= listNil(CCode);
	gcvPreProcCC	= listNil(CCode);
	gcvDefCC	= listNil(CCode);
	gcvBIntCC	= listNil(CCode);
	gcvRRFmtCC	= listNil(CCode);
	gcvInitProgCC	= listNil(CCode);

	/* Table of RRFmts which in gcvRRFmtCC */
	gcvRRFmtTable = tblNew((TblHashFun)foamHash, (TblEqFun)foamEqual);

	gcvExportedGloInitCC = listNil(CCode);
	gcvImportedGloInitCC = listNil(CCode);
	gcvInitFunDeclsCC  = listNil(CCode);
	gcvInitFunCalls0CC = listNil(CCode);
	gcvInitFunCalls1CC = listNil(CCode);

	/* Initialise Fortran functional parameter tracking */
	ftnFunParamInit();

	nLexs	= foamDDeclArgc(listElt(Foam)(gcvLexStk, gcvLvl));
	nLFmts	= foamArgc(gcvFmt) - FOAM_FORMAT_START;

	/* Guess num stmts here. */
	for (i = 0; i < foamArgc(gcvDefs); i++) {
		Foam	fdef = gcvDefs->foamDDef.argv[i], prog;
		prog = fdef->foamDef.rhs;
		if (foamTag(prog) == FOAM_Prog) {
			nStmts += foamArgc(prog->foamProg.body);
			nDefs += 1;
		}
		else {
			nStmts += 1;
		}
	}
	gcvNStmts = nStmts;

	/* Define globals */
	gc0CreateGloList(name);	
	gc0AddExtraModules();

	gc0InitDeclList();

	gc0LexDecl(nLexs);

	/* Create typedefs for each C structure we create */
	for (i=FOAM_FORMAT_START; i < FOAM_FORMAT_START+nLFmts; i++) {
		switch ((gcvFmt->foamDFmt.argv[i])->foamDDecl.usage) {
		  /* Some formats must not have a typedef */
		  case FOAM_DDecl_FortranSig:	/*FALLTHROUGH*/
		  case FOAM_DDecl_CSig:		/*FALLTHROUGH*/
		  case FOAM_DDecl_JavaClass:	/*FALLTHROUGH*/
			break;
		  default:
			gc0LFmtDef(i);
		}
	}

	/*
	 * Now generate the structure declarations. Note that this
	 * loop is NOT independent of the previous loop so don't
	 * try merging them! We must generate the typedefs before
	 * the structures that they refer to.
	 */
	for (i=FOAM_FORMAT_START; i < FOAM_FORMAT_START+nLFmts; i++) {
		switch ((gcvFmt->foamDFmt.argv[i])->foamDDecl.usage) {
		  /* Some formats must not have a typedef */
		  case FOAM_DDecl_FortranSig:	/*FALLTHROUGH*/
		  case FOAM_DDecl_CSig:		/*FALLTHROUGH*/
		  case FOAM_DDecl_JavaClass:	/*FALLTHROUGH*/
		  case FOAM_DDecl_JavaSig:	/*FALLTHROUGH*/
			break;
		  default:
			gc0LFmtDecl(i, foamArgv(gcvFmt)[i].code);
		}
	}

	/* Declare all constants */
	for (i = 0; i < foamDDeclArgc(gcvConst); i++) {
		gc0ConstDecl(i);
	}

	decls = gc0FiniDeclList();

	/* Preprocessor lines, e.g. #includes */
	gcvPreProcCC = listNReverse(CCode)(gcvPreProcCC);
	ccPreProc = gc0ListOf(CCO_Many, gcvPreProcCC);
	listLen = listLength(CCode)(gcvPreProcCC);

	for (i = 0; i < listLen; i++)
		gc0AddLine(hcode, ccoArgv(ccPreProc)[i]);

	ccrest = gc0ListOf(CCO_Many, decls);
	listLen = listLength(CCode)(decls);
	for (i = 0; i < listLen; i++)
		gc0AddLine(hcode, ccoArgv(ccrest)[i]);

	ccdefs = ccoNewNode(CCO_Many, nDefs - 1);

	n      = 0;
	stmtCounter = 0;
	/* Do all bar the last stage... */
	while (nStmts > gcvSMax && gcvSMax > 0) {
		for (i = n; i < nDefs - 1 && stmtCounter < gcvSMax; i++) {
			Foam	f = foamArgv(gcvDefs)[i+1].code, prog;

			prog = f->foamDef.rhs;
			if (foamTag(prog) == FOAM_Prog) {
				Foam	body = prog->foamProg.body;
				stmtCounter += foamArgc(body) + 1;
			}
			else
				stmtCounter++;

			ccoArgv(ccdefs)[i] = gccDef(f);
			
			gc0AddLine(code, ccoArgv(ccdefs)[i]);
		}
		n = i;

		nBrothers += 1;
		gc0AddLine(code, gc0GenModuleInitFun(name, false, nBrothers));
		code   = listNReverse(CCode)(code);

		ccExtD = gc0ListOf(CCO_Many, code);

		listFree(CCode)(code);

		code = listNil(CCode);
		gc0AddLine(allcode, ccoUnit(ccExtD));
		stmtCounter = 0;
		nStmts -= gcvSMax;
	}

	
	for (i = n; i < nDefs - 1; i++)  
                ccoArgv(ccdefs)[i] = gccDef(foamArgv(gcvDefs)[i+1].code);

	for (i = nDefs - 1; i < foamArgc(gcvDefs) - 1; i++) {
		c = gccDef(gcvDefs->foamDDef.argv[i+1]);
		if (c)
			gc0AddLine(gloDefs, c);
	}
	
	/* First constant (initialization) */
	def0c = gccDef0List(gcvDefs->foamDDef.argv[0]);

		/* Declarations for globals */
	gcvGloCC = listNReverse(CCode)(gcvGloCC);
	ccglo  = gc0ListOf(CCO_Many, gcvGloCC);
	for (i = 0; i < listLength(CCode)(gcvGloCC); i++)
		gc0AddLine(hcode, ccoArgv(ccglo)[i]);

	hcode  = listNReverse(CCode)(hcode);

	ccExtH = gc0ListOf(CCO_Many, hcode);

	if (!gc0OverSMax())
		gc0AddLine(code, ccoUnit(ccExtH));

	/* Define globals in the same module where const 0 is defined*/

	gcvDefCC = listNReverse(CCode)(gcvDefCC);
	ccd  = gc0ListOf(CCO_Many, gcvDefCC);
	gc0AddLine(code, ccoUnit(ccd)); 

	for (i = 1; i <= nBrothers; i++) {
		gc0AddLine(gcvInitFunDeclsCC, gc0DeclModuleInitFun(name, i));
	}

	gc0AddLine(code, def0c);

	for (i = n; i < nDefs - 1; i++)  
                  gc0AddLine(code, ccoArgv(ccdefs)[i]);

	gc0AddLine(code, gc0GenModuleInitFun(name, true, nBrothers));

	/* doing this ensures that the definition of the
	 *  initialisation proc and the proc itself appear in the same 
	 *  file 
	 * !! bug: if we have more than 1
	 * (def (glo x) (clos a b))
	 * at top level, this won't work.
	 */
	ccGloDefs = gc0ListOf(CCO_Many, gloDefs);

	gc0AddLine(code, ccGloDefs);
	code   = listNReverse(CCode)(code);

	ccExtD = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	gc0AddLine(allcode, ccoUnit(ccExtD));
	allcode = listNReverse(CCode)(allcode);

	if (gc0OverSMax())
		gc0AddLine(allcode, ccoUnit(ccExtH));

	/* Finalise Fortran functional parameter tracking */
	ftnFunParamFinish();

	listFree(CCode)(constDefs);
	listFree(CCode)(gloDefs);
	
	listFree(CCode)(gcvDefCC);
	listFree(CCode)(gcvGloCC);
	listFree(CCode)(gcvPreProcCC);
	listFree(CCode)(gcvBIntCC);
	listFree(CCode)(gcvRRFmtCC);
	listFree(CCode)(gcvExportedGloInitCC);
	listFree(CCode)(gcvImportedGloInitCC);

	tblFreeDeeply(gcvRRFmtTable, (TblFreeKeyFun)foamFree,
		(TblFreeEltFun)ccoFree);

	return allcode;
}

local void
gc0AddExtraModules()
{
	Foam decl;
	CCode cco;
	decl = foamNewGDecl(FOAM_Clos, "rtexns", 
			    emptyFormatSlot, 
			    FOAM_GDecl_Import,
			    FOAM_Proto_Init);
	cco = gc0GloIdDecl(decl, -1);
	ccoFree(cco);
}


/*****************************************************************************
 *
 * :: Top level entry point for axlmain.c code generation.
 *
 ****************************************************************************/

CCode
genAXLmainC(String name)
{
	CCode		ccode;
	CCodeList	code = listNil(CCode);

	gc0InitSpecialChars();


	gc0AddLine(code, ccoDecl(ccoType(ccoExtern(),
					 ccoIdOf("int")),
				 ccoFCall(gc0MultVarId(gcFiInitModulePrefix,
						       int0, name),
					  int0)));

	name = gen0InitialiserName(name);

	gc0AddLine(code, gc0GetterDecl(name));
	gc0AddLine(code, gc0ArgcDecl());
	gc0AddLine(code, gc0ArgvDecl());
	gc0AddLine(code, gc0MainDecl());

	gc0AddLine(code, gc0MainDef(name));

	code  = listNReverse(CCode)(code);
	ccode = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);

	ccode = ccoUnit(ccode);

	return ccode;
}

/*****************************************************************************
 *
 * :: Functions generating the module initialization function.
 *
 ****************************************************************************/

local CCode
gc0DeclModuleInitFun(String name, int n)
{
	return ccoDecl(ccoType(ccoExtern(), ccoTypeIdOf("int")),
		       		ccoFCall(gc0ModuleInitFun(name, n), int0));
}

/* Given the name of the module, it returns a function that provide the
 * module initialization operation, such as the declarations for exported and
 * imported globals.
 * <main> says if it is the main module of a set of split.In this case it must 
 * generate also the call to the module init fun. for each brother.
 * If main is true => <n> is the number of brothers
 * If main is false => <n> is the progressive number of splitted file.
 */
local CCode
gc0GenModuleInitFun(String name, int main, int n)
{
	CCode		ccode;
	CCodeList	code = listNil(CCode);
	int		modNum;
	CCodeList	impGloInitCC;

	impGloInitCC = (main ? listCopy(CCode)(gcvImportedGloInitCC) :
		     	       listCopyDeeply(CCode)(gcvImportedGloInitCC,
						     ccoCopy));

	modNum = (main ? int0 : n);

	if (main) {
		code = impGloInitCC;

		gc0AddLine(code, 
			   ccoIf(ccoIdOf("fiFileInitializer"),
				 ccoCompound(gc0ListOf(CCO_Many,
						       gcvInitFunCalls0CC)),
				 ccoCompound(gc0ListOf(CCO_Many,
						       gcvInitFunCalls1CC))));
				 
		code = listConcat(CCode)(code,
				  listCopy(CCode)(gcvExportedGloInitCC));
		
		listFree(CCode)(gcvInitFunCalls0CC);
		listFree(CCode)(gcvInitFunCalls1CC);

	}
	else {
		code = impGloInitCC;
		gc0AddLine(gcvInitFunCalls0CC,
			   ccoStat(ccoFCall(ccoIdOf("fiFileInitializer"),
					    ccoStringVal(symIntern("<children>")))));
		/* Here I need to know the name of the file */
		
		gc0AddLine(gcvInitFunCalls1CC,
			   ccoStat(ccoFCall(gc0ModuleInitFun(name, modNum),
			   int0)));
	}

	gc0AddLine(code, 
		   ccoComment(ccoStringOf("---------------------------")));

	/* Add prog initializations */
	code = listConcat(CCode)(code, gcvInitProgCC);
	gcvInitProgCC = listNil(CCode);

	gc0AddLine(code, ccoReturn(ccoIntOf(int0)));

	code = listNReverse(CCode)(code);

/***  Generate the following code:
 * static int initialized; = 0
 * if (initialized)
 *           return 0;
 * else
 *           initialized = 1
 */
	gc0AddLine(code, ccoStat(ccoIf(ccoIdOf("initialized"),
			       ccoReturn(ccoIntOf(int0)),
			       ccoAsst(ccoIdOf("initialized"),
				       ccoIdOf("1")))));
	gc0AddLine(code, ccoDecl(ccoType(ccoStatic(), ccoIdOf("int")),
				         ccoAsst(ccoIdOf("initialized"),
				 ccoIdOf("0"))));

	if (main) {
		gc0AddLine(code, gc0ListOf(CCO_Many, gcvInitFunDeclsCC));
		listFree(CCode)(gcvInitFunDeclsCC);
	}

	ccode = gc0ListOf(CCO_Many, code);
	ccode = ccoFDef(ccoType(ccoExtern(), ccoTypeIdOf("int")),
		       		gc0ModuleInitFun(name, modNum),
				NULL, ccoCompound(ccode));

	listFree(CCode)(code);
	
	return ccode;
}

/*****************************************************************************
 *
 * :: Functions generating C code declarations.
 *
 ****************************************************************************/

/*****************************************************************************
 *
 * :: Create a C code tree of type 'tag' from the list of C code l.
 *
 ****************************************************************************/

local CCode
gc0ListOf(CCodeTag tag, CCodeList l)
{
	CCode		new;
	CCodeList	t;
	int 		i, n;

	
	for (n = 0, t = l; t; t = cdr(t)) 
		if (car(t)) n++;

	new = ccoNewNode(tag, n);

	for (i = 0, t = l; t; t = cdr(t))
		if (car(t)) ccoArgv(new)[i++] = car(t);

	return new;
}

/*****************************************************************************
 *
 * :: Create a C declaration for main and global variables for argc + argv;
 *
 ****************************************************************************/

local CCode
gc0GetterDecl(String name)
{
	return ccoDecl(ccoType(ccoStatic(), ccoTypeIdOf(gcFiClos)),
		       ccoPreStar(gc0MultVarId("pG", int0, name)));
}

local CCode
gc0ArgcDecl(void)
{
	return ccoDecl(ccoType(ccoExtern(), ccoTypeIdOf("int")),
		       ccoIdOf("mainArgc"));
}

local CCode
gc0ArgvDecl(void)
{
	return ccoDecl(ccoType(ccoExtern(), ccoTypeIdOf("char")),
		       ccoPreStar(ccoPreStar(ccoIdOf("mainArgv"))));
}

local CCode
gc0MainDecl(void)
{
	return ccoDecl(ccoType(ccoExtern(), ccoTypeIdOf("int")),
		       ccoFCall(ccoIdOf("main"), (CCode) NULL));
}

/*****************************************************************************
 *
 * :: Create a global variable C declaration from the Foam declaration given
 *    at position 'idx' in the Foam globals Decl tree.
 *
 ****************************************************************************/

local CCode
gc0GloDecl(int idx)
{
	Foam	decl;
	CCode	cco;
	String	buf;

	decl = gcvGlo->foamDDecl.argv[idx];
	assert(foamTag(decl) == FOAM_GDecl);
	switch (decl->foamGDecl.protocol) {
	case FOAM_Proto_Foam:
	case FOAM_Proto_C:
	case FOAM_Proto_Fortran:
	case FOAM_Proto_Other:
	case FOAM_Proto_Init:
		cco = gc0GloIdDecl(decl, idx);
		break;
	default:
		buf = strPrintf("Cannot declare %s of protocol %s.",
				decl->foamGDecl.id,
				foamProtoStr(decl->foamGDecl.protocol));
		cco = ccoCppLine(ccoIdOf("warning"), ccoStringOf(buf));
	}
	return cco;
}

/*****************************************************************************
 *
 * :: Create a list of global variable declarations from the global
 *    section of the Foam DDecl, and prototype exported globals.
 *
 ****************************************************************************/

local void
gc0CreateGloList(String name)
{
	int			i, j, n;

	n = foamDDeclArgc(gcvGlo);

	for (i = 0; i < n; i++) {
		Foam gdecl = gcvGlo->foamDDecl.argv[i];

		if (gdecl->foamGDecl.protocol == FOAM_Proto_C) {
			char	*tokInclFile;
			char	*tokDecl = strCopy(gdecl->foamGDecl.id);
			char	sep[2];

			sep[0] = FOREIGN_INCLUDE_SEPARATOR;
			sep[1] = 0;

			tokDecl = strCopy(gdecl->foamGDecl.id);
			tokInclFile = strtok(tokDecl, sep); /* throw away the first part */
			
			tokInclFile = strtok(NULL, sep); /* get the first include file */

			if (tokInclFile) {
			        for (;tokInclFile;) {       /* get the remaining include files */
				  gc0AddHeaderIfNeeded(tokInclFile);
				  tokInclFile = strtok(NULL, sep);
		
				}
			}
			
			else {
				gc0AddLine(gcvGloCC, 
					   gc0GloIdDecl(gdecl, i));
			}
			strFree(tokDecl);
		}
		else if (gdecl->foamGDecl.protocol == FOAM_Proto_Include) {
			char	*tokInclFile;
			char *tokDecl;
			char	sep[2];

			sep[0] = FOREIGN_INCLUDE_SEPARATOR;
			sep[1] = 0;
			tokDecl = strCopy(gdecl->foamGDecl.id);
			tokInclFile = strtok(tokDecl, sep); /* the first header */
			if (tokInclFile) {
			        for (;tokInclFile;) {       /* get the remaining include files */
				  gc0AddHeaderIfNeeded(tokInclFile);
				  tokInclFile = strtok(NULL, sep);
		
				}
			}
		}
		else if (gdecl->foamGDecl.protocol == FOAM_Proto_Fortran) {
			Foam argformat = gcvFmt->foamDFmt.argv[gdecl->foamGDecl.format];
			Foam arg;
			FortranType argtype;
			int fno = 0;

			gc0AddLine(gcvGloCC, gc0GloDecl(i));
			/* Don't check the function result decl */
			for (j = 0; j < ((foamArgc(argformat)-1)-1); j++) {
				arg = argformat->foamDDecl.argv[j];
				
				argtype = gc0GetFortranType(arg);
				if ((argtype == FTN_XLString) ||
				    (argtype == FTN_String))
				{
					gc0AddHeaderIfNeeded("<string.h>");
				}
				else if (argtype == FTN_FnParam) {
					gc0FtnExFunPar(name,arg,gdecl,fno,i);
					fno++;
				}

			} 
		}
		else 
			gc0AddLine(gcvGloCC, gc0GloDecl(i));
		
		/* Handle Exports to C/Fortran */
		
		if (foamGDeclIsExportOf(FOAM_Proto_C, gdecl)
		    || foamGDeclIsExportOf(FOAM_Proto_Fortran, gdecl)) {
			Foam fakedecl;
			CCode cco;

			fakedecl = foamCopy(gdecl);
			fakedecl->foamGDecl.protocol = FOAM_Proto_Foam;
			cco = gc0GloIdDecl(fakedecl, i);
			gc0AddLine(gcvGloCC, cco);
			foamFreeNode(fakedecl);
			if (gdecl->foamGDecl.protocol == FOAM_Proto_C) {
				for (j = 0; j < foamArgc(gcvDefs); j++) {
					Foam fdecl = foamArgv(gcvDefs)[j].code;
					Foam ccdecl = gc0GetDecl(fdecl->foamDef.lhs);
				       
					if (strEqual(gdecl->foamGDecl.id, ccdecl->foamDecl.id))
					{
						if (foamTag(fdecl->foamDef.lhs) == FOAM_Const) {
							cco = gc0ExportCDef(name, gdecl, i, j);
							gc0AddLine(gcvDefCC, cco); 
						}
						break;
					}
				}
			}
			else {
				Foam	ffmt = gcvFmt->foamDFmt.argv[gdecl->foamGDecl.format];

				cco = gc0ExportToFortran(name, gdecl, ffmt, NULL, i);
				gc0AddLine(gcvDefCC, cco);
			}
		}
	}
	if (ccHdrFileList) {
		stoFree(ccHdrFileList);
		ccHdrFileList = NULL;
	}
}

local void
gc0FtnExFunPar(String name, Foam arg, Foam gdecl, AInt n, AInt nglo)
{
	CCode		ccdef;
	String		fn = gdecl->foamGDecl.id;
	FtnFunParam	info  = gc0FtnFunParam(fn, n);
	Foam		pfmt = gcvFmt->foamDFmt.argv[arg->foamDecl.format];

	if (gdecl->foamGDecl.dir == FOAM_GDecl_Export)
		bug("gc0FtnExFunPar: Export to Fortran has function parameter");

	(void)gc0FtnFunClosDeclare(info);
	(void)gc0FtnFunClosDefine(info);
	ccdef = gc0ExportToFortran(name, gdecl, pfmt, info, nglo);
	gc0AddLine(gcvDefCC, ccdef);
	gc0AddHeaderIfNeeded("<string.h>"); /*** Why??? ***/
}

local int
gc0IsNewHeader(char *name)
{
	struct ccListHdrFiles	*l;
	int			isnew = 1;

	l = ccHdrFileList;
	while (l) {
		if (!strcmp(l->fname, name)) {
			isnew = 0;
			break;
		}
		else {
			l = l->next;
		}
	}
	if (isnew) {
		struct ccListHdrFiles	*lhf;

		lhf = (struct ccListHdrFiles *) stoAlloc(OB_Other,
							 sizeof(*lhf));
		lhf->fname = strAlloc(strlen(name));
		strcpy(lhf->fname, name);
		lhf->next  = ccHdrFileList;
		ccHdrFileList = lhf;
	}
	return isnew;
}

local void
gc0AddHeaderIfNeeded(String fname)
{
	CCode	cppline;
	if (!gc0IsNewHeader(fname)) return;
	if (*fname == '<')
		cppline = ccoCppLine(ccoIdOf("include"), ccoIdOf(fname));
	else
		cppline = ccoCppLine(ccoIdOf("include"), ccoStringOf(fname));

	gc0AddLine(gcvPreProcCC, cppline);
}

/*****************************************************************************
 *
 * :: Create a fluid C code declaration from the Foam declaration given
 *    at position 'i' in the Foam fluid variable tree.
 *
 ****************************************************************************/

local CCode
gc0FluidDecl(int i)
{
	Foam decl;

	decl = gcvFluids->foamDDecl.argv[i];

	return ccoDecl(ccoTypeIdOf(gcFiFluid),
		       gc0MultVarId("F", i, decl->foamDecl.id));
}

/*****************************************************************************
 *
 * :: Structure generation
 *
 ****************************************************************************/

local CCodeList gc0DeclList(int n, Foam *argv);

static CCodeList gc0DeclStmts;

/*
 * Create a constant C code declaration from the Foam declaration given
 * at position 'idx' in the Foam constant Decl tree.
 */

local void
gc0ConstDecl(int idx)
{
	Foam	progDef, val, decl;
	CCode	ccType, ccName, ccProgName;
	CCode   ccProto, ccClass;
	String	str;

	progDef = foamArgv(gcvDefs)[idx].code;
	assert(foamTag(progDef) == FOAM_Def);

	val = progDef->foamDef.rhs;
	if (foamTag(val) != FOAM_Prog) return ;

	decl = gcvConst->foamDDecl.argv[idx];
	str = decl->foamDecl.id;
	if (gc0OverSMax()) {
		if (idx) {
			Foam	fn = gcvConst->foamDDecl.argv[0];
			str = strPrintf("%s_%s", fn->foamDecl.id,
					decl->foamDecl.id);
		}
		ccClass = ccoExtern();
		ccType = ccoType(ccoExtern(),ccoIdOf("FiProg"));
	}
	else {
		ccClass = ccoStatic();
		ccType = ccoType(ccoStatic(),ccoIdOf("FiProg"));
	}

	ccName = gc0MultVarId("C", idx, str);
	ccProgName = gc0MultVarId("CF", idx, str);
#ifdef NEW_FORMATS
		
	ccProto = ccoFCall(ccProgName, gc0Param(val,
						gcvParams->foamDDecl.argv[val->foamProg.params-1]));
		
	gc0AddLine(gc0DeclStmts, ccoDecl(ccClass, ccName));
	gc0AddLine(ccoDecl(ccoType(ccClass, 
				   gc0TypeId(val->foamProg.retType,
					     val->foamProg.format)), ccoProto));

#else
	ccProto = ccoFCall(ccProgName, gc0Param(val,
						val->foamProg.params));
	gc0AddLine(gc0DeclStmts, ccoDecl(ccType, ccName));
	gc0AddLine(gc0DeclStmts, ccoDecl(ccoType(ccClass, gc0TypeId(val->foamProg.retType, val->foamProg.format)), ccProto));
#endif
}

local void
gc0InitDeclList()
{
	gc0DeclStmts = listNil(CCode);
}

local CCodeList
gc0FiniDeclList()
{
	CCodeList ret = listNReverse(CCode)(gc0DeclStmts);
	gc0DeclStmts = listNil(CCode);
	return ret;
}

local void
gc0LexDecl(int num)
{
	CCodeList	code = listNil(CCode);
	CCode		ccLexFmt;
	Foam		ddecl;
	
	if (!num) return;

	ddecl = listElt(Foam)(gcvLexStk,gcvLvl);
	code = gc0DeclList(foamDDeclArgc(ddecl), &ddecl->foamDDecl.argv[0]);
	ccLexFmt = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	ccLexFmt = ccoStructDef(gc0VarId(gcFmtName, lexesSlot),
					      ccLexFmt);
	gc0AddLine(gc0DeclStmts, ccoStat(ccLexFmt));
}

local void
gc0LFmtDecl(int idx, Foam ddecl)
{
	CCode 	ccLFmt;
	CCodeList   code = listNil(CCode);
	/* Trailing arrays are just downright odd */
	if (ddecl->foamDDecl.usage == FOAM_DDecl_TrailingArray) {
		CCodeList hdr  = gc0DeclList(foamTRDDeclIDeclN(ddecl)
					     , &foamTRDDeclIDecl(ddecl, int0));
		CCodeList tail = gc0DeclList(foamTRDDeclTDeclN(ddecl), 
					     &foamTRDDeclTDecl(ddecl, int0));

		if (tail) {
			CCode tl  = gc0VarId(gcTFmtName, idx);
			CCode rhs = ccoARef(ccoIdOf(gcFiTRTail), ccoIdOf(gcFiNARY));
			CCode def = ccoStructDef(tl, gc0ListOf(CCO_Many, tail));
			gc0AddLine(gc0DeclStmts, ccoStat(def));
			tail = listSingleton(CCode)(ccoDecl(ccoStructRef(ccoCopy(tl)),
							    rhs));
		}
		code = listNConcat(CCode)(hdr, tail);
	}
	/* Empty structs are tweaked to always have at least one member */
	else if (foamDDeclArgc(ddecl) == 0) {
		CCode ccName, ccDecl;
		ccName = gc0MultVarId("X", int0, "empty");
		ccDecl = ccoDecl(ccoTypeIdOf(gcFiPtr), ccName);
		gc0AddLine(code, ccDecl);
	}
	else {
		code = gc0DeclList(foamDDeclArgc(ddecl), &ddecl->foamDDecl.argv[0]);
	}
	ccLFmt = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	gc0AddLine(gc0DeclStmts, ccoStat(ccoStructDef(gc0VarId(gcFmtName, idx), 
						      ccLFmt)));
}

/*
 * Create a format C code typedef of a structure reference for the format
 *    number 'idx', e.g.  typedef struct Fmt<idx> *PFmt<idx>
 */

local void
gc0LFmtDef(int idx)
{
	gc0AddLine(gc0DeclStmts,
		   ccoDecl(ccoType(ccoTypedef(),
				   ccoStructRef(gc0VarId(gcFmtName, idx))),
			   ccoPreStar(gc0VarId(gcFmtType, idx))));
}



local CCodeList
gc0DeclList(int n, Foam *argv)
{
	Foam 	  decl;
	CCodeList code = listNil(CCode);
	CCode	  ccDecl, ccName;
	int	  fmt, i;
	
	for (i = 0; i < n; i++) {
		decl   = argv[i];
		fmt    = decl->foamDecl.format;
		ccName = gc0MultVarId("X", i, decl->foamDecl.id);
		ccDecl = gc0Decl(decl, ccName);
		gc0AddLine(code, ccDecl);
	}

	return listNReverse(CCode)(code);
}

/*****************************************************************************
 * 
 * :: Create the C definition of an export to C global Foam function, 'gdecl',
 *    with index 'nglo', and the index 'nprog' to a Foam program definition.
 *
 ****************************************************************************/

local CCode
gc0ExportCDef(String name, Foam gdecl, int nglo, int nprog)
{
	CCode	ccType, ccName, ccParams, ccArgs, ccBody, ccLast, tmpParams, ccGlo;
	CCodeList fnbody = listNil(CCode);
	Foam	cprog  = foamArgv(gcvDefs)[nprog].code;
	Foam	ccdecl  = cprog->foamDef.rhs;
#ifdef NEW_FORMATS
	Foam	params = gcvParams->foamDDecl.argv[ccdecl->foamProg.params-1];
#else
	Foam	params = ccdecl->foamProg.params;
#endif
	FoamTag *argTypes;

	int	i, ix, nparams, nargs;

	ccType  = gc0TypeId(ccdecl->foamProg.retType, ccdecl->foamProg.format);
	ccGlo   = gc0MultVarId("G", nglo, gdecl->foamDecl.id);
	tmpParams = gc0Param(ccdecl, params);
	ccParams  = ccoNewNode(CCO_Many, ccoArgc(tmpParams)-1);

	/* Env variable is first in parameter list, but it is omitted here. */
	for (i = 0; i < ccoArgc(tmpParams)-1; i++)
		ccoArgv(ccParams)[i] = ccoArgv(tmpParams)[i+1];
	ccName = ccoIdOf(gdecl->foamDecl.id);
	nparams = foamDDeclArgc(params);
	nargs	= ccoArgc(ccParams);
	ccArgs  = ccoNewNode(CCO_Many, nargs + 2);
	ccoArgv(ccArgs)[0] = gc0TypeId(ccdecl->foamProg.retType, emptyFormatSlot);
	ccoArgv(ccArgs)[1] = ccoCopy(ccGlo);

	argTypes = (FoamTag *) stoAlloc(OB_Other, sizeof(FoamTag)*nparams);

	for (i = 2, ix = 0; ix < nparams; i++, ix++) {
		Foam	decl = params->foamDDecl.argv[ix];
		ccoArgv(ccArgs)[i] = gc0MultVarId("P", ix, decl->foamDecl.id);
		argTypes[ix] = decl->foamDecl.type;
	}

	for (ix = 0; i < nargs + 2; i++, ix++)
		ccoArgv(ccArgs)[i] = gc0MultVarId("R", ix, "");

	ccLast = gc0FiCFun(ccdecl->foamProg.retType, 
			   nparams,
			   argTypes, 
			   ccArgs, 
			   ccdecl->foamProg.format);
	
	stoFree(argTypes);

	fnbody = gc0ExportInit(name, gdecl, nglo);
	if (ccdecl->foamProg.retType == FOAM_NOp)
		/* void or multiple value return */
		gc0AddLine(fnbody, ccoStat(ccLast));
	else
		gc0AddLine(fnbody, ccoReturn(ccoCast(ccoCopy(ccType), ccLast)));
	fnbody = listNReverse(CCode)(fnbody);
	ccBody = ccoCompound(gc0ListOf(CCO_Many, fnbody));
	listFree(CCode)(fnbody);

	return ccoFDef(ccType, ccName, ccParams, ccBody);
}

/* Args are:
 * name:  Name of this function
 * gdecl: decl for export
 * argsformat: Fortran DDecl of arguments
 * fnparam_name: name of closure (if applicable)
 * nglo: Global number
 *
 * Result is a function `name' suitable for calling from fortran,
 * which calls an aldor function.
 * The code below needs a good solid kick.
 */
local CCode
gc0ExportToFortran(String name, Foam gdecl, Foam argsformat, 
		   FtnFunParam fnpar, int nglo)
{
	CCode	  ccClos;
	CCode	  ccType, ccName, ccParams, ccBody, ccCmplxArgName;
	CCode	  ccCmplxType, ccResTmpCast, cctmp, cctmp1;
	CCodeList fndefparams, fndefexecs, fndefbody;
	CCodeList fndefafter = listNil(CCode);
	CCodeList fndefmainparams = listNil(CCode);
	CCodeList fndefchrlenparams = listNil(CCode);
	CCodeList fndefdecls = listNil(CCode);
	CCodeList wordtmps = listNil(CCode);

	CCode	ccResTmp, ccArgs, ccArgName, ccFiCCall, ccResType;
	CCode	ccStringArgName;
	CCode	ccStringLenName, ccStringLenType;
	FoamTag *argTypes;

	int	i;
	int	nxlargs = foamDDeclArgc(argsformat) - 1;
	int	ntmps = 0;
	int	nchrargs = 0;
	char    num[20];
	String	varname, argname;

	FortranType argtype;

	String	cmplxfns = compCfgLookupString("fortran-cmplx-fns");
	Bool	cmplxfirstarg = false;
	Bool	stringfirstarg = false;
	Bool	hasReturn = true;
	FoamTag	fmrestype = (argsformat->foamDDecl.argv[nxlargs])->foamDecl.type; 
	FortranType  ftnrestype = gc0GetFortranRetType(argsformat);

	if (ftnrestype && (ftnrestype != FTN_Machine))
		fmrestype = gen0FtnMachineType(ftnrestype);

	ccType		= NULL;
	ccCmplxType	= NULL;
	ccCmplxArgName	= NULL;
	ccResTmp	= NULL;
	ccResType	= NULL;


	/* Hack */
	if (fmrestype == FOAM_Char)
		ftnrestype = FTN_Character;

	/* Deal with return value. SEE gc0IdFortranDecl() ... */
	ccStringArgName = 0;
	ccStringLenName = 0;
	switch (ftnrestype) {
	  case FTN_Boolean:
		/* Fall through */
	  case FTN_SingleInteger:
		ccType    = ccoTypeIdOf(gcFiSInt);
		ccResType = ccoCopy(ccType);
		break;
	  case FTN_FSingle:
		ccType    = ccoTypeIdOf(gcFiSFlo);
		ccResType = ccoCopy(ccType);
		break;
	  case FTN_FDouble:
		ccType    = ccoTypeIdOf(gcFiDFlo);
		ccResType = ccoCopy(ccType);
		break;
	  case FTN_Character:
		/* Fall through */
	  case FTN_String:
		/*
		 * Unless the user is doing something silly with pretend
		 * then it is safe to assume that the length of the string
		 * will be the same as the length specified by its type.
		 * This in turn ought to correspond to the size used by
		 * the Fortran function otherwise the Fortran RTS will
		 * barf. The end result is that we can pretend that it is
		 * a normal Aldor String.
		 */
		/* Fall through */
	  case FTN_XLString:
		/*
		 * String return values are implemented in Fortran
		 * by passing them as the first argument immediately
		 * followed by the string length. Other parameters
		 * come after this pair.
		 */
		stringfirstarg  = true;
		hasReturn       = false;
		ccStringLenName = ccoIdOf("STRINGRESLEN");
		ccStringArgName = ccoIdOf("STRINGRESULT");
		ccStringLenType = ccoTypeIdOf(gcFiSInt);


		/* Add the two new parameters to the list */
		gc0AddLine(fndefmainparams,
			ccoParam(ccStringArgName,
				ccoChar(),
				ccoPreStar(ccoCopy(ccStringArgName))));
		gc0AddLine(fndefmainparams,
			ccoParam(ccStringLenName,
				ccStringLenType,
				ccoCopy(ccStringLenName)));
		ccResType = gc0TypeId(FOAM_Word, emptyFormatSlot);
		break;
	  case FTN_FDComplex:
		/* Fall through */
	  case FTN_FSComplex:
		ccCmplxType = ccoTypeIdOf(ftnrestype == FTN_FSComplex ? gcFiComplexSF : gcFiComplexDF);
		if (!cmplxfns)
			comsgFatal(NULL, ALDOR_F_NoFCmplxProperty, "fortran-cmplx-fns");
		else if (strEqual(cmplxfns, "return-void")) {
			cmplxfirstarg = true;
			hasReturn = false;
			ccCmplxArgName = ccoIdOf("CMPLXRESULT");
			gc0AddLine(fndefmainparams, 
				   ccoParam(ccCmplxArgName, 
					    ccCmplxType, 
					    ccoPreStar(ccoCopy(ccCmplxArgName)))); 
		}
		else if (strEqual(cmplxfns, "return-struct"))
			ccType = ccCmplxType; 
		else if (strEqual(cmplxfns, "disallowed"))
			bug("gc0ExportToFortran: A function returning a complex result is exported/passed to Fortran");
		else
			comsgFatal(NULL, ALDOR_F_BadFCmplxValue, cmplxfns);
		ccResType = gc0TypeId(FOAM_Word, emptyFormatSlot);
		break;
	  default:
		switch (fmrestype) {
		   case FOAM_NOp:
			hasReturn = false;
			break;
		   case FOAM_Bool : 
			/* Fall through */
		   case FOAM_SInt :
			ccType    = ccoTypeIdOf(gcFiSInt);
			ccResType = ccoCopy(ccType);
			break;
		   default:
			ccType    = gc0TypeId(fmrestype, emptyFormatSlot);
			ccResType = ccoCopy(ccType);
			break;
		}
	}


	/* If no return value then the function type is void */
	if (!hasReturn)
		ccType = gc0TypeId(FOAM_NOp, emptyFormatSlot);


	/* Temporary for return value */
	(void)sprintf(num, "%d", ntmps++);
	varname  = strConcat("T", num);
	ccResTmp = ccoIdOf(varname);
	strFree(varname);


	/* Add initialiser or declaration */
	if (fnpar)
	{
		/* Function being passed as parameter to Fortran */
		ccName = gc0FtnFunDeclare(fnpar, ccType);
		ccClos = gc0FtnFunClosure(fnpar);
		fndefexecs = listNil(CCode);
	}
	else
	{
		ccName = ccoIdOf(gc0GenFortranName(gdecl->foamDecl.id));
		ccClos = gc0MultVarId("G", nglo, gdecl->foamDecl.id);
		fndefexecs = gc0ExportInit(name, gdecl, nglo);
	}

	/* Arguments: first two are fakes (function type and name) */
	ccArgs  = ccoNewNode(CCO_Many, nxlargs + 2);
	ccoArgv(ccArgs)[0] = gc0TypeId(fmrestype, emptyFormatSlot);
	ccoArgv(ccArgs)[1] = ccoCopy(ccClos);

	argTypes = (FoamTag *) stoAlloc(OB_Other, nxlargs * sizeof(FoamTag));

	for (i = 0; i < nxlargs; i++) {
		Foam	decl = argsformat->foamDDecl.argv[i];
		CCode   ccParam;
		Bool modifiablearg = gc0IsModifiableFortranArg(decl);
		argname = gc0GetFortranArgName(decl);
		ccArgName = gc0MultVarId("P", i, argname);
		argtype = gc0GetFortranType(decl);


		/* Construct parameter list */
		/*
		 * How many of these cases are now obsolete - we ought to
		 * only ever find FOAM_Word arguments since our function
		 * ought to have been wrapped up for us during genfoam.
		 * We do get FTN_XLString though ...
		 */
		switch (argtype) {
		  case FTN_Boolean:
		  case FTN_Machine:
		  case FTN_SingleInteger:
		    	argTypes[i] = decl->foamDecl.type;
			cctmp   = gc0TypeId(argTypes[i], emptyFormatSlot);
			ccParam = ccoParam(ccArgName, cctmp, 
					   ccoCopy(ccArgName));
			break;
		  case FTN_FSingle:
		    	argTypes[i] = FOAM_Word;
			cctmp   = gc0TypeId(FOAM_SFlo, emptyFormatSlot);
			ccParam = ccoParam(ccArgName, cctmp,
					   ccoCopy(ccArgName));
			break;
		  case FTN_FDouble:
		    	argTypes[i] = FOAM_Word;
			cctmp   = gc0TypeId(FOAM_DFlo, emptyFormatSlot);
			ccParam = ccoParam(ccArgName, cctmp,
					   ccoCopy(ccArgName));
			break;
		  default:
		    	argTypes[i] = decl->foamDecl.type;
			cctmp   = gc0TypeId(argTypes[i], emptyFormatSlot);
			ccParam = ccoParam(ccArgName, cctmp,
					   ccoCopy(ccArgName));
		}
	        gc0AddLine(fndefmainparams, ccParam);


		/* Fill in the body */
		switch (argtype) {
	  	  case FTN_Character:
			/* Fall through: see gf_fortran.c */
		  case FTN_String:
			/* Fall through */
		  case FTN_XLString:
			/* String parameters are special */
			cctmp = gc0ExportFtnString(
					ccArgName,
					modifiablearg,
					&wordtmps,
					&fndefchrlenparams,
					&fndefexecs,
					&fndefafter,
					&nchrargs,
					&ntmps);
			ccoArgv(ccArgs)[i+2] = cctmp;
			if (fnpar) gc0AddHeaderIfNeeded("<string.h>");
			break;
		  case FTN_FSComplex:
		  case FTN_FDComplex:
		  case FTN_Array:
		  case FTN_Word:
		  case FTN_FSingle:
		  case FTN_FDouble:
		  case FTN_Boolean:
		  case FTN_Machine:
		  case FTN_SingleInteger:
			ccoArgv(ccArgs)[i+2] = gc0MultVarId("P", i, argname);
			break;
		  case FTN_FnParam:
			if (fnpar)
				bug(
   "gc0ExportToFortran: A Fortran routine function parameter itself has a function parameter");
			else
				bug(
   "gc0ExportToFortran: Export to Fortran has a function parameter");
			break;
		  default:
			bug("unknown type in make-arglist");
			break;
		}
		strFree(argname);

	}

	fndefchrlenparams = listNReverse(CCode)(fndefchrlenparams);
	fndefmainparams = listNReverse(CCode)(fndefmainparams);
	fndefparams = listNConcat(CCode)(fndefmainparams, fndefchrlenparams);
	ccParams = gc0ListOf(CCO_Many, fndefparams);
	listFree(CCode)(fndefmainparams);

	if (hasReturn)
	{
		ccFiCCall = gc0FiCFun(fmrestype, nxlargs, argTypes,
			ccArgs, emptyFormatSlot);
	}
	else
	{
		ccFiCCall = gc0FiCFun(FOAM_NOp, nxlargs, argTypes,
			ccArgs, emptyFormatSlot);
	}


	stoFree(argTypes);
	argTypes = NULL;

	switch (ftnrestype) {
	  case FTN_Character:
		/*
		 * Store the result of our Aldor function in a
		 * temporary variable (ccResTmp).
		 */
		cctmp = ccoAsst(ccoCopy(ccResTmp), ccFiCCall);
		gc0AddLine(fndefexecs, ccoStat(cctmp));


		/*
		 * Characters are passed as strings of length 1 so
		 * we generate the following code:
		 *
		 *    T = foo( ... );
		 *    *(char *)STRINGRESULT = (char)T;
		 */
		cctmp  = ccoCopy(ccStringArgName);
		cctmp  = ccoCast(ccoPostStar(ccoChar()), cctmp);
		cctmp  = ccoPreStar(cctmp);
		cctmp1 = ccoCast(ccoChar(), ccoCopy(ccResTmp));
		cctmp  = ccoAsst(cctmp, cctmp1);
		gc0AddLine(fndefexecs, ccoStat(cctmp));
		break;
	  case FTN_String:
		/* Fall through (see earlier explanation) */
	  case FTN_XLString:
		/*
		 * Store the result of our Aldor function in a
		 * temporary variable (ccResTmp).
		 */
		cctmp = ccoAsst(ccoCopy(ccResTmp), ccFiCCall);
		gc0AddLine(fndefexecs, ccoStat(cctmp));


		/*
		 * Copy the result string into the pointer passed to us
		 * by the Fortran RTS. This assumes that they are NUL
		 * terminated (normal for Aldor strings) or that they
		 * are at least as long as Fortran wants them to be.
		 *
		 * We generate the following code:
		 *
		 *    T = foo( ... );
		 *    strncpy((char *)STRINGRESULT,(char *)T,STRINGRESLEN);
		 */
		cctmp  = ccoCopy(ccStringArgName);
		cctmp  = ccoCast(ccoPostStar(ccoChar()), cctmp);
		cctmp1 = ccoCast(ccoPostStar(ccoChar()), ccoCopy(ccResTmp));
		cctmp  = ccoMany3(cctmp, cctmp1, ccoCopy(ccStringLenName));
		cctmp  = ccoStat(ccoFCall(ccoIdOf("strncpy"), cctmp));
		gc0AddLine(fndefexecs, cctmp);


		/* We ought to add a NUL terminator */
		break;
	  case FTN_FSComplex:
	  case FTN_FDComplex:
		cctmp = ccoAsst(ccoCopy(ccResTmp), ccFiCCall);
		gc0AddLine(fndefexecs, ccoStat(cctmp));
		cctmp = ccoPostStar(ccoCopy(ccCmplxType));
		ccResTmpCast = ccoParen(ccoCast(cctmp, ccoCopy(ccResTmp)));
		if (cmplxfirstarg) {
			gc0AddLine(fndefexecs, ccoStat(ccoAsst(ccoPointsTo(ccoCopy(ccCmplxArgName), 
									   ccoIdOf("real")),
							       ccoPointsTo(ccResTmpCast,
									   ccoIdOf("real")))));
			gc0AddLine(fndefexecs, ccoStat(ccoAsst(ccoPointsTo(ccoCopy(ccCmplxArgName), 
									   ccoIdOf("imag")),
							       ccoPointsTo(ccoCopy(ccResTmpCast),
									   ccoIdOf("imag")))));
		}
		else
		{
			gc0AddLine(fndefexecs, ccoStat(ccoAsst(ccoCopy(ccResTmp),
				ccoPreStar(ccResTmpCast))));
		}
		break;
	  default:
		if (hasReturn)
		{
			gc0AddLine(fndefexecs, ccoStat(ccoAsst(ccoCopy(ccResTmp),
				ccoCast(ccoCopy(ccType), ccFiCCall))));
		}
		else
		{
			gc0AddLine(fndefexecs, ccoStat(ccFiCCall));
			ccResTmp  = (CCode)NULL; /* Temp not used */
		}
		break;
	}

	if (ccResTmp)
	{
		/* Declare the temporary */
		cctmp  = ccoDecl(ccoCopy(ccResType), ccoCopy(ccResTmp));
		gc0AddLine(fndefdecls, cctmp);

		/* Add a return statement if required */
		if (hasReturn)
			gc0AddLine(fndefafter, ccoReturn(ccResTmp));
	}

	if (wordtmps) {
		wordtmps = listNReverse(CCode)(wordtmps);
		gc0AddLine(fndefdecls, ccoDecl(gc0TypeId(FOAM_Word, emptyFormatSlot), 
					       gc0ListOf(CCO_Many, wordtmps)));
		listFree(CCode)(wordtmps);
	}

	fndefdecls = listNReverse(CCode)(fndefdecls);
	fndefexecs = listNReverse(CCode)(fndefexecs);
	fndefafter = listNReverse(CCode)(fndefafter);

	fndefbody = listNConcat(CCode)(fndefdecls, fndefexecs);
	fndefbody = listNConcat(CCode)(fndefbody, fndefafter);
	ccBody = ccoCompound(gc0ListOf(CCO_Many, fndefbody));
	listFree(CCode)(fndefbody);

	if (fnpar)
		ccType = ccoType(ccoCopy(gc0FtnFunClass(fnpar)), ccType);

	return ccoFDef(ccType, ccName, ccParams, ccBody);
}

local CCode
gc0ExportFtnString(CCode ccArg, Bool mod,
			CCodeList *tmps, CCodeList *chrlens,
			CCodeList *execs, CCodeList *after,
			int *nchrargs, int *ntmps)
{
	char	num[100];
	String	varname;
	CCode	cctmp, cctmp1;
	CCode	ccStringTmp, ccChrLenArg, ccStringLen;


	/* Get the name of the string length argument */
	(void)sprintf(num, "%d", *nchrargs);
	*nchrargs   = *nchrargs + 1;
	varname     = strConcat("CHRLEN", num);
	ccChrLenArg = ccoIdOf(varname);
	strFree(varname);


	/* Declare the the string temporary variable */
	(void)sprintf(num, "%d", *ntmps);
	*ntmps      = *ntmps + 1;
	varname     = strConcat("T", num);
	ccStringTmp = ccoIdOf(varname);
	gc0AddLine(*tmps, ccStringTmp);
	strFree(varname);


	/* Add the declaration for the string length argument */
	cctmp  = ccoTypeIdOf("int");
	cctmp1 = ccoCopy(ccChrLenArg);
	cctmp  = ccoParam(ccChrLenArg, cctmp, cctmp1);
	gc0AddLine(*chrlens, cctmp);


	/* Create a new string: fiARRNEW_Char() */
	ccStringLen = ccoPlus(ccoCopy(ccChrLenArg), ccoIdOf("1"));
	cctmp       = gc0TypeId(FOAM_Word, emptyFormatSlot);
	cctmp       = ccoMany3(ccoCopy(ccStringTmp), cctmp, ccStringLen);
	cctmp       = ccoStat(ccoFCall(ccoIdOf("fiARRNEW_Char"), cctmp));
	gc0AddLine(*execs, cctmp);


	/* Copy the Fortran string into the temporary */
	cctmp  = ccoCast(ccoPostStar(ccoChar()), ccoCopy(ccStringTmp));
	cctmp1 = ccoCast(ccoPostStar(ccoChar()), ccoCopy(ccArg));
	cctmp  = ccoMany3(cctmp, cctmp1, ccoCopy(ccChrLenArg));
	cctmp  = ccoStat(ccoFCall(ccoIdOf("strncpy"), cctmp));
	gc0AddLine(*execs, cctmp);


	/* Append the NUL terminator */
	cctmp  = ccoCast(ccoPostStar(ccoChar()), ccoCopy(ccStringTmp));
	cctmp  = ccoARef(ccoParen(cctmp), ccoCopy(ccChrLenArg));
	cctmp1 = ccoCast(ccoChar(), ccoIdOf("0"));
	cctmp  = ccoStat(ccoAsst(cctmp, cctmp1));
	gc0AddLine(*execs, cctmp);


	/* Create after-call code to write the string back? */
	if (mod)
	{
		cctmp  = ccoCast(ccoPostStar(ccoChar()), ccoCopy(ccArg));
		cctmp1 = ccoCast(ccoPostStar(ccoChar()), ccoCopy(ccStringTmp));
		cctmp  = ccoMany3(cctmp, cctmp1, ccoCopy(ccChrLenArg));
		cctmp  = ccoStat(ccoFCall(ccoIdOf("strncpy"), cctmp));
		gc0AddLine(*after, cctmp);
	}


	/* Return the parameter */
	return ccoCopy(ccStringTmp);
}


local CCodeList
gc0ExportInit(String name, Foam gdecl, int nglo)
{
	AInt	init;
	Foam	ginit, ginit0;
	CCode	ccInit, ccInit0, ccGlo, ccCall;
	CCodeList	stmts = listNil(CCode);

	if (gdecl->foamGDecl.protocol == FOAM_Proto_Fortran)
		init = (AInt) 0;
	else
		init = gdecl->foamDecl.format;
	ginit  = gcvGlo->foamDDecl.argv[init];
	ginit0 = gcvGlo->foamDDecl.argv[0];
	ccInit  = gc0MultVarId("G", nglo, ginit->foamDecl.id);
	ccInit0 = gc0MultVarId("G", nglo, ginit0->foamDecl.id);
	ccGlo   = gc0MultVarId("G", nglo, gdecl->foamDecl.id);
	if (init == 0) {
		ccCall = ccoFCall(gc0MultVarId(gcFiInitModulePrefix,
					       int0, name),
				  int0);
		gc0AddLine(stmts, ccoStat(ccCall));
		ccCall = gcFiCCallN(int0,
				    ccoMany2(ccoTypeIdOf(gcFiClos),
					     ccoCopy(ccInit0)));
	}
	else {
		CCode	n = gc0MultVarId("G", nglo, "domainPrepare!");
		
		static Bool initPrepare = false;
		if (!initPrepare) {
			CCode	type = ccoType(ccoExtern(),
					       ccoTypeIdOf(gcFiClos));
			gc0AddLine(gcvGloCC, ccoDecl(type, ccoCopy(n)));
			initPrepare = true;
		}

		ccCall = ccoFCall(gc0MultVarId(gcFiInitModulePrefix,
					       int0, name),
				  int0);
		gc0AddLine(stmts, ccoStat(ccCall));
		ccCall = gcFiCCallN(int0,
				    ccoMany2(ccoTypeIdOf(gcFiClos),
					     ccoCopy(ccInit0)));
		gc0AddLine(stmts, ccoStat(ccCall));
		ccCall = gcFiCCallN(1,
				    ccoMany3(ccoTypeIdOf("void"), n,
					     ccoCopy(ccInit)));
	}
	gc0AddLine(stmts, ccoIf(ccoLNot(ccGlo),
				 ccoStat(ccCall), NULL));
	return stmts;
}

/*
 * Passing Aldor functions to Fortran is tricky: the functions here
 * are used to create and access an object which takes care of all
 * the nasty details. 
 */
local FtnFunParam
gc0FtnFunParam(String name, AInt i)
{
	FtnFunParam	new, result;


	/* Create a new structure */
	new = (FtnFunParam)stoAlloc(OB_Other, sizeof(*result));


	/* Fill in the basic details */
	gc0FtnFunBase(new)   = strCopy(name);
	gc0FtnFunNumber(new) = i;


	/* Check to see if already known */
	result = (FtnFunParam)tblElt(gcvFtnTable, (TblKey)new, (TblElt)0);
	if (result) return result;


	/* We've never seen this function/parameter before */
	if (gc0OverSMax())
	{
		/* Need to make it global */
		String	base = gc0FtnFunBase(new);
		String	par  = gcFortranPar;
		String	clo  = gcFortranClo;

		gc0FtnFunName(new)    = gc0MultVarId(par, gcvNFtnPar, base);
		gc0FtnFunClosure(new) = gc0MultVarId(clo, gcvNFtnPar, base);
		gc0FtnFunClass(new)   = ccoExtern();
	}
	else
	{
		/* We can make it local */
		String	null = (String)NULL;
		String	base = gc0FtnFunBase(new);
		String	par  = strlConcat(gcFortranPar, "_", base, null);
		String	clo  = strlConcat(gcFortranClo, "_", base, null);

		gc0FtnFunName(new)    = gc0VarId(par, gcvNFtnPar);
		gc0FtnFunClosure(new) = gc0VarId(clo, gcvNFtnPar);
		gc0FtnFunClass(new)   = ccoStatic();
	}


	/* Store this information in our table as key and value */
	tblSetElt(gcvFtnTable, (TblKey)new, (TblElt)new);


	/* Increment the counter */
	gcvNFtnPar++;


	/* Return the information */
	return new;
}

local CCode
gc0FtnFunClosDeclare(FtnFunParam info)
{
	CCode		class = gc0FtnFunClass(info);
	CCode		name = gc0FtnFunClosure(info);
	CCode		ctype = ccoTypeIdOf(gcFiClos);
	CCode		dtype = ccoType(ccoCopy(class), ccoCopy(ctype));

	gc0AddLine(gcvGloCC, ccoDecl(dtype, ccoCopy(name)));
	return ccoCopy(name);
}

local CCode
gc0FtnFunClosDefine(FtnFunParam info)
{
	CCode		name = gc0FtnFunClosure(info);
	CCode		ctype = ccoTypeIdOf(gcFiClos);

	/*
	 * We only need a definition if splitting files. Also we
	 * must NOT include a storage class with the definition.
	 */
	if (gc0OverSMax())
		gc0AddLine(gcvDefCC, ccoDecl(ccoCopy(ctype), ccoCopy(name)));


	/* Return closure name for convenience. */
	return ccoCopy(name);
}

local CCode
gc0FtnFunDeclare(FtnFunParam info, CCode type)
{
	CCode	class = gc0FtnFunClass(info);
	CCode	name  = gc0FtnFunName(info);
	CCode	dtype = ccoType(ccoCopy(class), ccoCopy(type));
	CCode	dname = ccoFCall(ccoCopy(name), int0);

	gc0AddLine(gcvGloCC, ccoDecl(dtype, dname));
	return ccoCopy(name);
}

local void
ftnFunParamInit(void)
{
	/*
	 * Create a new hash table for tracking functions
	 * passed as parameters to Fortran functions.
	 */
	TblHashFun hasher = (TblHashFun)ftnFunParamHash;
	TblEqFun   comper = (TblEqFun)ftnFunParamEqual;

	gcvNFtnPar  = int0;
	gcvFtnTable = tblNew(hasher, comper);
}

local void
ftnFunParamFinish(void)
{
	/* Kill the hash table for tracking functional parameters */
	TblFreeKeyFun	keykiller = (TblFreeKeyFun)NULL;
	TblFreeEltFun	valkiller = (TblFreeEltFun)ftnFunParamFree;

	tblFreeDeeply(gcvFtnTable, keykiller, valkiller);
}

local Hash
ftnFunParamHash(FtnFunParam info)
{
	/* Hash on name and parameter number */
	return strHash(gc0FtnFunBase(info) + gc0FtnFunNumber(info));
}

local Bool
ftnFunParamEqual(FtnFunParam p1, FtnFunParam p2)
{
	/* Quick check on parameter number */
	if (gc0FtnFunNumber(p1) != gc0FtnFunNumber(p2)) return false;


	/* Only equal if base names are equal */
	return strEqual(gc0FtnFunBase(p1), gc0FtnFunBase(p2));
}

local void
ftnFunParamFree(FtnFunParam info)
{
	/* The base name is definitely unaliased */
	strFree(gc0FtnFunBase(info));


	/*
	 * The other fields may be aliased: wipe the
	 * structure clean so that the garbage collector
	 * will not mistake them for live pointers.
	 */
	gc0FtnFunNumber(info)  = int0;
	gc0FtnFunBase(info)    = (String)NULL;
	gc0FtnFunName(info)    = (CCode)NULL;
	gc0FtnFunClosure(info) = (CCode)NULL;
	gc0FtnFunClosure(info) = (CCode)NULL;


	/* Free the whole structure */
	stoFree(info);
}

/*****************************************************************************
 *
 * :: Code to generate programs, closures and definitions.
 *
 ****************************************************************************/

/*****************************************************************************
 * 
 * :: Create a C code program definition for the initialization constant prog
 *    given by 'foam'.
 *
 ****************************************************************************/

local CCode
gccDef0List(Foam foam)
{
	CCode	cc;

	gcvisInitConst = 1;
	cc = gccDef(foam);
	gcvisInitConst = 0;
	return cc;
}

/*****************************************************************************
 * 
 * :: Create a C program definition for constants,
 *    or else a closure initialization or assignment statement.
 *
 ****************************************************************************/

local CCode
gccDef(Foam foam)
{
	Foam	ref, val;
	CCode	cc;

	assert(foamTag(foam) == FOAM_Def);

	ref = foam->foamDef.lhs;
	val = foam->foamDef.rhs;

	switch (foamTag(ref)) {
	  case FOAM_Const:
		if (foamTag(val) != FOAM_Prog) return 0;
		cc = gc0Prog(ref, val);
		ccoPos(cc) = foamPos(val);
		break;
	  default:
		if (gcvLvl == 0) cc = gc0ClosInit(ref, val);
		else cc = gc0Set(ref, val);
		break;
	}

	return cc;
}

/*****************************************************************************
 * 
 * :: Create a C function definition for a Foam program given by 'foam',
 *    and referenced by 'ref' .
 *
 ****************************************************************************/

local Bool  gc0ProgIsC(Foam foam);
local Bool  gc0FoamIsJavaPCall(Foam foam);
local CCode gc0ProgBody(Foam ref, Foam prog);
local CCode gc0ProgBodyC(Foam ref, Foam prog);
local CCode gc0ProgBodyOther(Foam ref, Foam prog);

local CCode
gc0Prog(Foam ref, Foam foam)
{
	Scope("gc0Prog");
	int		type, progFmt;
	Foam		params, locals, lexicals;
	Foam		fluids;
	CCode		ccBody, ccParams, ccLeft, ccRight;
	CCodeList	codeProg = listNil(CCode);
	CCode		ccSpec;
	AInt		format;
	int		fluid(gcvLvl);
	FoamList	fluid(gcvLexStk);
	Foam		fluid(gcvPar), fluid(gcvLoc);
	Foam		fluid(gcvLFmtStk);
	Foam		decl = gc0GetDecl(ref);
	CCode 		retval;

	assert(foamTag(foam) == FOAM_Prog);

	/* We have to leave the pointer crushing until now */
	if (optIsKillPointersWanted())
	{
		foam = foamCopy(foam);
		killProgPointers(foam);
		if (DEBUG(phase)) {
			stoAudit();
		}
	}

	progFmt	 = foamProgIndex(foam);
	type	 = foam->foamProg.retType;
	format	 = foam->foamProg.format;
#ifdef NEW_FORMATS
	params	 = gcvParams->foamDDecl.argv[foam->foamProg.params-1];
#else
	params	 = foam->foamProg.params;
#endif
	locals	 = foam->foamProg.locals;
	lexicals = foamArgv(gcvFmt)[progFmt].code;
	fluids	 = foam->foamProg.fluids;

	gcvProg    = foam;
	gcvLvl	   = gcvLvl+1;
	gcvPar	   = params;
	gcvLoc	   = locals;
	gcvLocFluids = fluids;
	gcvLFmtStk = foam->foamProg.levels;
	gcvLexStk  = listCons(Foam)(lexicals, gcvLexStk);

	if (gc0OverSMax()) {
		gcvSpec	  = gc0TypeId(type, format);
		ccSpec    = ccoTypeIdOf(gcFiProg);
	}
	else {
		gcvSpec	  = ccoType(ccoStatic(), gc0TypeId(type, format));
		ccSpec	  = ccoType(ccoStatic(), ccoTypeIdOf(gcFiProg));
	}
	ccParams  = gc0Param(foam, params);
	gcvIsLeaf = foamProgIsLeaf(foam);

	ccBody = gc0ProgBody(ref, foam);

	listFreeCons(Foam)(gcvLexStk);

	ccLeft = gc0MultVarId("tmp", ref->foamConst.index, decl->foamDecl.id);
	ccRight = ccoInit(ccoMany4(ccoCast(ccoIdOf("FiFun"),
						gccProgId(ref)),
					ccoIntOf(int0),
					ccoIntOf(int0),
					ccoIntOf(int0)));

	gc0AddLine(codeProg, ccoDecl(ccoType(ccoStatic(),
					     ccoStructRef(ccoIdOf("_FiProg"))),
				     ccoAsst(ccLeft, ccRight)));
	if (gc0OverSMax()) 
		gc0AddLine(codeProg, ccoDecl(ccSpec, gccId(ref)));

	gc0AddLine(gcvInitProgCC,  ccoStat(ccoAsst(gccId(ref),
						   ccoPreAnd(ccoCopy(ccLeft)))));

	retval = ccoMany2(ccoFDef(gcvSpec, gccProgId(ref), ccParams, ccBody),
			gc0ListOf(CCO_Many, codeProg));
	Return(retval);
}

local CCode
gc0ProgBody(Foam ref, Foam prog)
{
	if (!gc0ProgIsC(prog)) {
		return gc0ProgBodyOther(ref, prog);
	}
	else {
		return gc0ProgBodyC(ref, prog);
	}
}

local CCode
gc0ProgBodyC(Foam ref, Foam foam)
{
	Foam locals  = foam->foamProg.locals;
	Foam body    = foam->foamProg.body;
	AInt progFmt = foamProgIndex(foam);

	CCode ccBody  = ccoCompound(gc0Compound(locals, body, ref, progFmt,
					       gcvIsLeaf));
	return ccBody;
}

local CCode
gc0ProgBodyOther(Foam ref, Foam prog)
{
	return ccoCompound(ccoStat(gcFiHalt(ccoIdOf("100"))));
}

local Bool
gc0ProgIsC(Foam foam)
{
	int i;
	if (foam->foamProg.retType == FOAM_JavaObj) {
		return false;
	}

	for (int i=0; i<foamDDeclArgc(foam->foamProg.params); i++) {
		Foam param = foam->foamProg.params->foamDDecl.argv[i];
		if (param->foamDecl.type == FOAM_JavaObj) {
			return false;
		}
	}

	for (int i=0; i<foamDDeclArgc(foam->foamProg.locals); i++) {
		Foam param = foam->foamProg.locals->foamDDecl.argv[i];
		if (param->foamDecl.type == FOAM_JavaObj) {
			return false;
		}
	}
	if (foamFindFirst(gc0FoamIsJavaPCall, foam->foamProg.body) != NULL) {
		return false;
	}

	return true;
}

local Bool
gc0FoamIsJavaPCall(Foam foam)
{
	if (foamTag(foam) != FOAM_PCall)
		return false;

	return foam->foamPCall.protocol == FOAM_Proto_Java
		|| foam->foamPCall.protocol == FOAM_Proto_JavaConstructor
		|| foam->foamPCall.protocol == FOAM_Proto_JavaMethod;
}

/*****************************************************************************
 * 
 * :: Create the C program parameters list for the Foam program 'foam',
 *    using the Foam parameters list 'params'.
 *
 ****************************************************************************/

local CCode
gc0Param(Foam foam, Foam params)
{
	CCodeList	code = listNil(CCode);
	CCode		ccParams, ccName, ccDecl, ccSpec;
	int		i;
	Foam		decl;

	assert(foamTag(params) == FOAM_DDecl);
	assert(foamTag(foam) == FOAM_Prog);

	gc0AddLine(code, gccEnvParam());
	for (i = 0; i < foamDDeclArgc(params); i++) {
		int	fmt, typ;
		decl   = params->foamDDecl.argv[i];
		fmt    = decl->foamDecl.format;
		typ = decl->foamDecl.type;

		ccName = gc0MultVarId("P", i, decl->foamDecl.id);
		ccDecl  = gc0Decl(decl, ccoCopy(ccName));
		
		gc0AddLine(code, ccoParam(ccName, ccoArgv(ccDecl)[0], ccoArgv(ccDecl)[1]));
	}
	if (foam->foamProg.retType == FOAM_NOp
	    && foam->foamProg.format != 0 
	    && foam->foamProg.format != emptyFormatSlot) {
		Foam    ddecl;
		int	n;
		DEBUG_DECL(int rn);
		ddecl = gcvFmt->foamDFmt.argv[foam->foamProg.format];
		n = foamDDeclArgc(ddecl);
		assert( (rn = gc0NumVals(foam->foamProg.body)) == 0
		       || n == rn); /* !! paranoid */
		for (i = 0; i < n; i++) {			
			Foam decl = ddecl->foamDDecl.argv[i];
			ccName = gc0MultVarId("R", i, "");
			ccSpec  = ccoPostStar(gc0TypeId(decl->foamDecl.type,
							decl->foamDecl.format));
			ccDecl = ccoCopy(ccName);
			gc0AddLine(code, ccoParam(ccName, ccSpec, ccDecl));
		}
	}
	code = listNReverse(CCode)(code);
	ccParams = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);

	return ccParams;
}

/*****************************************************************************
 * 
 * :: Create the declaration for the parent environment parameter.
 *
 ****************************************************************************/

local CCode
gccEnvParam()
{
	return ccoParam(ccoIdOf("e1"),
			gc0TypeId(FOAM_Env, int0), ccoIdOf("e1"));
}

/*****************************************************************************
 * 
 * :: Return the number of multiple-return values defined in the body of the
 *    Foam program 'body'.
 *
 ****************************************************************************/

int
gc0NumVals(Foam body)
{
	Foam	*argv;
	Length	i, argc;

	if (foamTag(body) == FOAM_Seq) {
		argc = foamArgc(body);
		argv = &body->foamSeq.argv[0];
	}
	else {
		argc = 1;
		argv = &body;
	}

	for (i = 0; i < argc; i++) {
		Foam	foam = argv[i];
		if (foamTag(foam) == FOAM_Return) {
			assert(foamTag(foam->foamReturn.value) == FOAM_Values);
			return foamArgc(foam->foamReturn.value);
		}
	}
	return 0;
}

/*****************************************************************************
 * 
 * :: Create the C code for the Foam program 'body', with local variables
 *    'locals', program reference 'ref', program format 'fmt', and whether
 *    the program is a leaf node, 'leaf'.
 *
 ****************************************************************************/

local CCode
gc0Compound(Foam locals, Foam body, Foam ref, int fmt, int leaf)
{
	CCodeList	code = listNil(CCode);
	CCodeList	cmpd = listNil(CCode);
	CCodeList	ccLevels, tmp;
	CCode		ccCmpd, ccBody;
	struct Clocals	*locList;
	AIntList	fl;	
	int		i, numLexs, maxLevel;
	Foam		nbody;

	gcvNLocs = 0;
	gcvLocals = NULL;
	gcvFluidList = listNil(AInt);
	numLexs	 = foamArgc(gcvLFmtStk);

	gcvNestFree = listNil(CCode);
	gcvNestUsed = listNil(CCode);
	gcvCallNesting = GC_NoCall;

	gc0CreateLocList(locals);
	assert(foamDDeclArgc(locals) == gcvNLocs);

	/*
	 * Some compilers (eg MSVC++ 6.0) need an explicit return
	 * instruction after code that does not return (e.g fiHalt).
	 */
	nbody = gc0AddExplicitReturn(body);

	gc0NewStmtInit();
	gccCmd(nbody);

	maxLevel  = gc0MaxLevel(numLexs);

	while (gcvLocals) {
		locList = gcvLocals->next;
		gc0AddLine(code, gcvLocals->loc);
		stoFree((Pointer) gcvLocals);
		gcvLocals = locList;
	}
	if (gcvFluidList) {
		for (fl = gcvFluidList; fl; fl=cdr(fl)) {
			gc0AddLine(code, gc0FluidDecl(car(fl)));
		}
		gc0AddLine(code, gc0PushFluid());
	}
	ccLevels = gc0Levels(numLexs, maxLevel, leaf, fmt);
	tmp = ccLevels;
	while (tmp) {
		gc0AddLine(code, car(tmp));
		tmp = cdr(tmp);
	}
	listFree(CCode)(ccLevels);

	/*
	 * If bigints or RRFmts exist, create their initialisations
	 * in the init prog.
	 */
	if (gcvisInitConst) {
		if (gcvNBInts) {
			CCode	ccbints;
			gcvBIntCC = listNReverse(CCode)(gcvBIntCC);
			ccbints = gc0ListOf(CCO_Many, gcvBIntCC);
			for (i = 0; i < listLength(CCode)(gcvBIntCC); i++)
				gc0AddLine(cmpd, ccoArgv(ccbints)[i]);
		}

		if (gcvNRRFmt) {
			CCode	ccfmts;
			gcvRRFmtCC = listNReverse(CCode)(gcvRRFmtCC);
			ccfmts = gc0ListOf(CCO_Many, gcvRRFmtCC);
			for (i = 0; i < listLength(CCode)(gcvRRFmtCC); i++)
				gc0AddLine(cmpd, ccoArgv(ccfmts)[i]);
		}
	}		
	if (gcvFluidList) {
		for (fl = gcvFluidList; fl; fl=cdr(fl)) {
			gc0AddLine(cmpd, gc0GetFluid(car(fl)));
		}
		for (i=0; i < foamArgc(gcvLocFluids); i++) {
			gc0AddLine(cmpd, 
				   gc0AddFluid(gcvLocFluids->foamDFluid.argv[i]));
		}
	}

	/* Hack to announce function entry */
	if (gencTraceFuns()) {
		/*
		 * Activated by -Wtrace-cfuns: we generate code to
		 * print out the name of this function immediately
		 * on entry. This is a very low-level debug option
		 * that is sometimes helpful (especially if gets are
		 * being traced in runtime.as).
		 */
		String	fname;
		CCode	cchack = gccProgId(ref);


		/* We assume that gccProgId returns a CCO_Id */
		assert(cchack->ccoHdr.tag == CCO_Id);


		/*
		 * We rely on the fact that ccoId and ccoStringVal
		 * have exactly the same representation. Rather than
		 * do anything clever we flip the tags and add some
		 * extra details around the function name.
		 */
		cchack->ccoHdr.tag = CCO_StringVal;
		fname = symString(cchack->ccoToken.symbol);
		fname = strConcat("\n[* ", strConcat(fname, " *]\n"));
		cchack->ccoToken.symbol = symIntern(fname);
		gc0AddLine(cmpd, ccoStat(ccoFCall(ccoIdOf("puts"), cchack)));
		strFree(fname);
	}

	assert(gcvStmts->pos <= gcvStmts->argc);
	for (i = 0; i < gcvStmts->pos; i++)
		gc0AddLine(cmpd, gcvStmts->stmt[i]);

	if (gcvFluidList && !gc0IsReturn(car(cmpd)))
		gc0AddLine(cmpd, gc0PopFluid());
	cmpd = listNReverse(CCode)(cmpd);
	ccCmpd = gc0ListOf(CCO_Many, cmpd);
	listFree(CCode)(cmpd);

	for (i = 0; i < ccoArgc(ccCmpd); i++)
		gc0AddLine(code, ccoArgv(ccCmpd)[i]);
	code = listNReverse(CCode)(code);
	ccBody = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	listFree(CCode)(gcvNestFree);
	listFree(CCode)(gcvNestUsed);
	stoFree((Pointer) (gcvStmts->stmt));
	stoFree((Pointer) (gcvStmts));

	/* Release the modified body (if modified) */
	if (nbody != body) foamFree(nbody);

	return gc0Compress(ccBody);
}

local Bool
gc0IsReturn(CCode cc)
{
	Bool	changed = true;

	while (changed) {
		switch (ccoTag(cc)) {
		case CCO_Compound:
			cc = ccoArgv(cc)[0];
			break;
		case CCO_Many:
			cc = ccoArgv(cc)[ccoArgc(cc)-1];
			break;
		default:
			changed = false;
			break;
		}
	}

	return ccoTag(cc) == CCO_Return;
}

/*****************************************************************************
 * 
 * :: Compress program labels and local variable declarations from the
 *    C code given by 'cc' and return the new C code body.
 *
 ****************************************************************************/

local CCodeList gc0DirtyStackFrame(CCode, int, CCodeList);

local CCode
gc0Compress(CCode cc)
{
        CCodeList	code = listNil(CCode);
	CCode		*current = NULL, *top = NULL;
	CCode		newCmpd, ccTmp;
	int		i, num;

	gc0NewLocsInit();
	num = ccoArgc(cc);
	for (i = 0; i < num; i++) {
		while (ccoTag(ccoArgv(cc)[i]) == CCO_Decl) {
			ccTmp = ccoCopy(ccoArgv(cc)[i]);
			gc0AddDecl(ccTmp, i);
			i++;
		}
		if ((ccoTag(ccoArgv(cc)[i]) == CCO_Label) && !top) {
			top     = &ccoArgv(cc)[i];
			current = &ccoArgv(*top)[1];
		}
		else {
			if (ccoTag(ccoArgv(cc)[i]) == CCO_Label) {
				*current       = ccoCopy(ccoArgv(cc)[i]);
				current        = &ccoArgv(*current)[1];
				ccoArgv(cc)[i] = NULL;
			}
			else {
				if (current) {
					*current = ccoCopy(ccoArgv(cc)[i]);
					current  = NULL;
					ccoArgv(cc)[i] = NULL;
				}
				top = NULL;
			}
		}
	}
	gc0NewLocals(cc);

	code = gc0DirtyStackFrame(cc, num, code);

	code = listNReverse(CCode)(code);
	newCmpd = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	return newCmpd;
}

local CCodeList
gc0DirtyStackFrame(CCode cc, int num, CCodeList code)
{
	int		i;

	for (i = 0; i < num; i++)
		if (ccoArgv(cc)[i] != 0) gc0AddLine(code, ccoArgv(cc)[i]);

	return code;
}

/*****************************************************************************
 *
 * :: Code to generate program statements.
 *
 ****************************************************************************/

/*****************************************************************************
 * 
 * :: Return the C code of the Foam command, 'foam', identified by Foam tag.
 *
 ****************************************************************************/

local CCode
gccCmd(Foam foam)
{
	CCode	cc;

	switch (foamTag(foam)) {
	  case FOAM_Def:
		cc = gccDef(foam);
		break;
	  case FOAM_Goto:
		cc = ccoGoto(gc0VarId("L", foam->foamGoto.label));
		break;
	  case FOAM_If:
		cc = gccIf(foam);
		break;
	  case FOAM_Select:
		cc = gccSelect(foam);
		break;
	  case FOAM_Return:
		cc = gccReturn(foam->foamReturn.value);
		break;
	  case FOAM_Set:
		cc = gc0Set(foam->foamSet.lhs, foam->foamSet.rhs);
		break;
	  case FOAM_Loose:
		/*cc = ccoAsst(gccRef(foam->foamLoose.loc), ccoIdOf("DEAD"));*/
		cc = ccoAsst(gccRef(foam->foamLoose.loc), ccoIdOf("0"));
		break;
	  case FOAM_EEnsure:
		cc = gcFiEEnsure(gccExpr(foam->foamEEnsure.env));
		break;
	  case FOAM_Free:
		cc = gcFiFree(gc0TryCast(FOAM_Ptr, foam->foamFree.place));
		break;
	  case FOAM_PopEnv:
		cc = ccoIdOf(";");
		break;
	  case FOAM_Seq:
		cc = gccSeq(foam);
		break;
	  case FOAM_NOp:
		cc = ccoStat(ccoIdOf(";"));
		break;
	  case FOAM_Cast:
		/*
		 * If a single result value Set generated for a Fortran
		 * routine which has one modifiable argument (and no
		 * function result) is optimised out a cast is generated
		 * when using -O casting to the type of the result value.
		 * This clashes with the void result type specified in the
		 * extern declaration for the routine.
		 */
		if (foamTag(foam->foamCast.expr) == FOAM_PCall &&
		    foam->foamCast.expr->foamPCall.protocol == FOAM_Proto_Fortran)
			cc = gccExpr(foam->foamCast.expr);
		else
			cc = gccExpr(foam);
		break;
	  default:
		cc = gccExpr(foam);
		break;
	}
	return cc;
}

/*****************************************************************************
 * 
 * :: Return the C code if statement from the Foam code if statement.
 *
 ****************************************************************************/

local CCode
gccIf(Foam foam)
{
	CCode	ccTest, ccThen;

	ccTest = gccExpr(foam->foamIf.test);
	ccThen = ccoGoto(gc0VarId("L", foam->foamIf.label));
	return ccoIf(ccTest, ccThen, NULL);
}

/*****************************************************************************
 * 
 * :: Return 'switch..case' C code from Foam 'select' code 'foam'.
 *
 ****************************************************************************/

local CCode
gccSelect(Foam foam)
{
	int		i;
	CCode		ccSel, ccLabel;
	CCodeList	code = listNil(CCode);

	for (i = 0; i < foamArgc(foam)-1; i++) {
		ccLabel = gc0VarId("L", foam->foamSelect.argv[i]);
		gc0AddLine(code, ccoCase(ccoIntOf(i), ccoGoto(ccLabel)));
	}
	code = listNReverse(CCode)(code);
	ccSel = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	ccSel = ccoCompound(ccSel);
	return ccoSwitch(gccExpr(foam->foamSelect.op), ccSel);
}

/*****************************************************************************
 * 
 * :: Return a C code 'return' statement for the given Foam node, 'foam'.
 *
 ****************************************************************************/

local CCode
gccReturn(Foam foam)
{
	FoamTag		fTag = foamTag(foam);
	int		retFmt;
	CCode ret;

	retFmt = gcvProg->foamProg.format;

	if (fTag == FOAM_Cast && foamTag(foam->foamCast.expr) != FOAM_BVal
	    && foamTag(foam->foamCast.expr) != FOAM_BCall
	    && foam->foamCast.type != FOAM_Rec)
		ret = ccoReturn(gccExpr(foam));
	else if (fTag == FOAM_Values)
		ret = gccReturnValues(foam);
	else if (gcvProg->foamProg.retType==FOAM_NOp)
		ret = ccoNewNode(CCO_Return, int0);
	else if (ccoArgc(gcvSpec) > 1)
		ret = ccoReturn(gc0SubExpr(foam,
					   ccoCopy(ccoArgv(gcvSpec)[1])));
	else
		ret = ccoReturn(gc0SubExpr(foam, ccoCopy(gcvSpec)));

	if (foamProgUsesFluids(gcvProg)) {
		return ccoNew(CCO_Compound, 1, ccoMany2(gc0PopFluid(), ret));
	}
	else return ret;
}

/*****************************************************************************
 * 
 * :: Return a C code 'return' statement for a multiple-value return
 *    Foam node, 'foam'.  Create the C statement with the return variables.
 *
 ****************************************************************************/

local CCode
gccReturnValues(Foam foam)
{
	int		i;
	
	for (i = 0; i < foamArgc(foam); i++) {
		CCode lhs = ccoPreStar(gc0MultVarId("R",i,""));
#if 0 /* Using `word *' as return object */
		CCode rhs = gc0SubExpr(foam->foamValues.argv[i],
				       ccoTypeIdOf(gcFiWord));
#endif
		CCode rhs = gccExpr(foam->foamValues.argv[i]);
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccoAsst(lhs, rhs)));
	}
	return ccoNewNode(CCO_Return, int0);
}

/*****************************************************************************
 * 
 * :: Return C code function which pushes the environment format given
 *    from 'foam' onto the parent environment stack.
 *
 ****************************************************************************/

local CCode
gccPushEnv(Foam foam)
{
	CCode	cc, cc1;
	int	format = foam->foamPushEnv.format;

	cc1 = gccExpr(foam->foamPushEnv.parent);
	if (gc0EmptyFormat(format))
		cc = gcFiEnvPush(ccoIdOf(gcFiNil), cc1);
	else
		cc = gcFiEnvPush(gcFi0New(gcFmtName, format, "CENSUS_EnvLevel"), cc1);
	return cc;
}

/*****************************************************************************
 * 
 * :: Return the C code statements created from the sequence of Foam nodes
 *    given by 'foam'.
 *
 ****************************************************************************/

local CCode
gccSeq(Foam foam)
{
	int		i, nSeq;
	Foam		seqFoam;
	CCode		ccSeq;

	nSeq  = foamArgc(foam);
	ccSeq = ccoNewNode(CCO_Many, nSeq);
	for (i = 0; i < nSeq; i++) {
		gcvisStmtFCall = 0;
		seqFoam = foamArgv(foam)[i].code;
		/* These automatically generate a statement. */
		if (!isStmt(foamTag(seqFoam))) {
			ccoArgv(ccSeq)[i] = gc0SeqStmt(foam, i);
			if (foamTag(seqFoam) != FOAM_Seq)
				gc0AddTopLevelStmt(gcvStmts, ccoArgv(ccSeq)[i]);
		}
		else {
			ccoArgv(ccSeq)[i] = ccoStat(gccCmd(seqFoam));
			gc0AddTopLevelStmt(gcvStmts, ccoArgv(ccSeq)[i]);
		}
		ccoPos(ccoArgv(ccSeq)[i]) = foamPos(seqFoam);
		while (gcvNestUsed) {
			gcvNestFree = listCons(CCode)(car(gcvNestUsed),
						      gcvNestFree);
			gcvNestUsed = cdr(gcvNestUsed);
		}
		
	}
	return ccSeq;
}

/*****************************************************************************
 * 
 * :: Return the C code for the Foam expression 'foam'.
 *
 ****************************************************************************/

local CCode
gccExpr(Foam foam)
{
	CCode	cc;
 
	switch (foamTag(foam)) {
	  case FOAM_BVal:
		cc = gc0Builtin(foam->foamBVal.builtinTag, foam);
		break;
	  case FOAM_Label:
		cc = ccoLabel(gc0VarId("L", foam->foamLabel.label),
			      ccoStat(NULL));
		break;
	  case FOAM_Cast:
		cc = gc0Cast(foam->foamCast.type, foam->foamCast.expr);
		break;
	  case FOAM_ANew:
		cc = gccArrNew(foam);
		break;
	  case FOAM_TRNew:
		cc = gccTRNew(foam);
		break;
	  case FOAM_RRFmt:
		cc = gccRRFmt(foam);
		break;
	  case FOAM_RRNew:
		cc = gccRRecNew(foam);
		break;
	  case FOAM_RNew:
		cc = gccRecNew(foam);
		break;
	  case FOAM_BCall:
		cc = gc0FunBCall(foam, emptyFormatSlot);
		break;
	  case FOAM_CCall:
		cc = gc0FunFoamCall(foam, emptyFormatSlot);
		break;
	  case FOAM_OCall:
		cc = gc0FunFoamCall(foam, emptyFormatSlot);
		break;
	  case FOAM_PCall:
		cc = gc0FunFoamCall(foam, emptyFormatSlot);
		break;
	  case FOAM_MFmt:
		cc = gccMFmt(foam);
		break;
	  case FOAM_Values:
		cc = gccValues(foam);
		break;
	  case FOAM_Throw:
		cc = gc0Throw(foam);
		break;
	  default:
		cc = gccVal(foam);
		break;
	}
	return cc;
}

/*****************************************************************************
 * 
 * :: Return the C function to create a new array of the size and type
 *    specified by the Foam array, 'foam'.
 *
 ****************************************************************************/

local CCode
gccArrNew(Foam foam)
{
	CCode	ccArrIndex, ccFunName, ccCall;

	ccArrIndex = gccExpr(foam->foamANew.size);
	ccFunName  = ccoIdOf(strConcat("fiArrNew_",
				       foamStr(foam->foamANew.eltType)));
	ccCall = ccoFCall(ccFunName, ccArrIndex);
	return ccoCast(gc0TypeId(FOAM_Ptr, emptyFormatSlot), ccCall);
}

/*****************************************************************************
 * 
 * :: Return the C function to create a new structure from the format specified
 *    by the Foam record, 'foam'.
 *
 ****************************************************************************/

local CCode
gccRecNew(Foam foam)
{
	int	fmt;

	fmt = foam->foamRNew.format;
	return gcFi0RecNew(gcFmtName, fmt, "CENSUS_Rec");
}

/*****************************************************************************
 * 
 * :: Return the C function to create a new raw record.
 *
 ****************************************************************************/

local CCode
gccRRecNew(Foam foam)
{
	AInt		argc;
	CCode		ccFmt, ccArgs;


	/* Get the format of this record */
	ccFmt = gccExpr(foam->foamRRNew.fmt);


	/* How many fields in the record? */
	argc = foam->foamRRNew.argc;


	/* Create the arguments for fiRawRecordNew */
	ccArgs = ccoMany2(gccExpr(foamNewSInt(argc)), ccFmt);
	return ccoFCall(ccoIdOf("fiRawRecordNew"), ccArgs);
}


local CCode
gccRRFmt(Foam foam)
{
	Foam		format;
	AInt		argc, i;
	CCode		ccArgs, ccFmt;
	CCodeList	arglist;


	/* Get the format of this record */
	format = foam->foamRRFmt.fmt;


	/* Must be a FOAM_Values */
	assert(foamTag(format) == FOAM_Values);


	/* How many fields in the record? */
	argc = foamArgc(format);


	/* Convert the values into a list */
	arglist = listNil(CCode);
	for (i = argc - 1;i >= 0; i--)
	{
		Foam arg = (format->foamValues.argv)[i];
		listPush(CCode, gccExpr(arg), arglist);
	}
	listPush(CCode, gccExpr(foamNewSInt(argc)), arglist);
	ccArgs = gc0ListOf(CCO_Many, arglist);


	/* Generate the call to fiRawRecordValues */
	ccFmt = ccoFCall(ccoIdOf("fiRawRecordValues"), ccArgs);


	/* Can we globalise this format creation? */
	if (rrRRFmtIsIndependent(foam))
		ccFmt = gc0RRFmt(ccFmt, foam);


	/* Return the format */
	return ccFmt;
}


/* Store RRFmt in a C global for this module */
local CCode
gc0RRFmt(CCode ccFmt, Foam rrfmt)
{
	CCode	cctype, ccdecl, ccset, ccname;


	/* Can we reuse a global? */
	ccname = (CCode)tblElt(gcvRRFmtTable, (TblKey)rrfmt, (TblElt)0);
	if (ccname) return ccoCopy(ccname);


	/* Create a new global */
	if (gc0OverSMax())
	{
		String id = gcvConst->foamDDecl.argv[0]->foamDecl.id;
		ccname = gc0MultVarId("GRRFmt", gcvNRRFmt, id);


		/* extern FiWord GRRFmt0; */
		cctype = ccoType(ccoExtern(), ccoTypeIdOf(gcFiWord));
		ccdecl = ccoDecl(cctype, ccoCopy(ccname));


		/* Add to the globals declaration list */
		gc0AddLine(gcvGloCC, ccdecl);


		/* FiWord GRRFmt0 = fiRawRecordValues(...); */
		cctype = ccoTypeIdOf(gcFiWord);
	}
	else
	{
		/* static FiWord GRRFmt0 = fiRawRecordValues(...); */
		ccname = gc0VarId("GRRFmt", gcvNRRFmt);
		cctype = ccoType(ccoStatic(), ccoTypeIdOf(gcFiWord));
	}


	/* Create a simple declaration for global */
	ccdecl = ccoDecl(cctype, ccoCopy(ccname));
	gc0AddLine(gcvDefCC, ccdecl);


	/* Add its initialisation to the module init */
	ccset = ccoStatAsst(ccoCopy(ccname), ccFmt);
	gc0AddLine(gcvRRFmtCC, ccset);


	/* Record the global in our table */
	tblSetElt(gcvRRFmtTable, (TblKey)foamCopy(rrfmt), (TblElt)ccname);


	/* We have added another RRFmt global */
	gcvNRRFmt++;


	/* Return the reference to the global */
	return ccoCopy(ccname);
}


/*****************************************************************************
 * 
 * :: Return the function call of the Foam PCall, 'foam'.
 *
 ****************************************************************************/

local CCode
gccPCallId(Foam foam)
{
#ifdef NEED_FUN_CAST
	CCode	ccType;

	ccType = ccoFCall(ccoFCall(gc0TypeId(foam->foamPCall.type, emptyFormatSlot),
				   ccoIdOf("*")), int0);
	return ccoParen(gc0SubExpr(foam->foamPCall.op, ccType));
#else
	return gccExpr(foam->foamPCall.op);
#endif
}


/*
 * TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO
 * TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO
 * TODO
 * TODO  This should be restructured so that the cases for the types are
 * TODO  handled completely in single blocks.  As is, the logic to prove
 * TODO  that all variables have proper values as we separate and join
 * TODO  is to complicated for most compilers and people.
 * TODO  See the "Just to silence the compiler..." comment.  SMW Sept 01.
 * TODO
 * TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO
 * TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO  TODO
 */


/*
 * This code doesn't worry about the final type for XL Word
 * pointer arguments i.e. whether they are FiWord, PFmt or FiChar
 * In addition we don't convert Fortran strings into Aldor strings
 * after a Fortran PCall. Thus if Fortran modifies a string
 * argument then it will *not* be NULL terminated unless the
 * Fortran routine does something special. We ought to do the
 * same thing as in FOAM whereby we store the function result
 * in a temporary, perform any special actions like patching
 * Fortran strings and then returning the temporary.
 */
local CCode
gccFortranPCall(Foam *resultvar, int numresultvars, Foam foam, CCodeList *closnulls)
{
	Foam gdecl, argformat, fargfmt, farg, fargnocast; 
	CCode ccTmp, ccArg, ccType, ccName, ccCast, ccChrTmp;
	CCode ccLenArg;
	CCode ccChrlen = NULL, ccArgs;
	CCodeList argslist;
	CCodeList mainargs = listNil(CCode);
	CCodeList chrlenargs = listNil(CCode);
	CCodeList closnullifys = listNil(CCode);
	int fnparamno = 0;
	int extraArg = 0;
	int i;
	String cmplxfns = compCfgLookupString("fortran-cmplx-fns");
	FtnFunParam	fnparam;
	String		tmpstr;
	FortranType restype, argtype;
	Length argc = foamArgc(foam)-3;
	FoamTag resvartype;
	FoamTag fmtype;
	AInt resvarfmt;
	Bool modifiablearg;
	Foam fnresultdecl;


	/* Just to silence the compiler... */
	ccType = NULL;
	ccName = NULL;
	ccChrTmp = NULL;
	ccArg = NULL;
	ccLenArg = NULL;
	resvartype = FOAM_Nil;
	resvarfmt  = 0;

	/* deal with result type... */
	gdecl = gc0GetDecl(foam->foamPCall.op);
	argformat = gcvFmt->foamDFmt.argv[gdecl->foamGDecl.format]; 
	fnresultdecl = gc0GetFortranRetFm(argformat);

	restype = gc0GetFortranRetType(argformat);
	if (restype && (restype != FTN_Machine))
		fmtype = gen0FtnMachineType(restype);
	else
		fmtype = fnresultdecl->foamDecl.type;


	/* Char and Character are equivalent */
	if (fmtype == FOAM_Char)
		restype = FTN_Character;


	/* Only allow 0 or 1 return value now ... */
	assert(!numresultvars || (numresultvars == 1));

	switch (restype) {
	case FTN_Character:
		/*
		 * Our hack in gen0MakeApplyArgs() has provided
		 * us with the string length as the first argument
		 * to this function. We just need to add the buffer.
		 */
		ccType = ccoPostStar(ccoChar());
		ccChrTmp = gc0GetTmp(ccType); 
		if (numresultvars)
			ccName   = gccRef(resultvar[numresultvars-1]);
		else
			ccName = gc0GetTmp(ccType); 


		/* Add the string pointer */
		ccTmp = ccoCast(ccoCopy(ccType), ccoCopy(ccChrTmp));
		mainargs = listCons(CCode)(ccTmp, mainargs);


		/* How long is the return value? */
		ccLenArg = gc0GetTmp(ccoTypeIdOf(gcFiSInt));
		farg = foam->foamPCall.argv[0];	
		ccTmp = ccoAsst(ccoCopy(ccLenArg), gccExpr(farg));
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


		/*
		 * We need to allocate a buffer for the result
		 * irrespective of whether or not the caller
		 * wants to use it.
		 */
		ccTmp = ccoPlus(ccoCopy(ccLenArg), ccoIdOf("1"));
		ccArg = ccoCast(ccoCopy(ccType), ccoCopy(ccChrTmp));
		ccTmp = ccoMany3(ccArg, ccoCopy(ccType), ccTmp);
		ccTmp = ccoFCall(ccoIdOf("fiARRNEW_Char"), ccTmp);
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


		/* We have an extra dummy argument */
		extraArg = 1;
		break;
	case FTN_String:
	case FTN_XLString:
		/*
		 * Our hack in gen0MakeApplyArgs() has provided
		 * us with the string length as the first argument
		 * to this function. We just need to add the buffer.
		 */
		ccType = ccoPostStar(ccoChar());
		if (numresultvars)
			ccName = gccRef(resultvar[numresultvars - 1]);
		else
			ccName = gc0GetTmp(ccType); 


		/* Add the string pointer */
		ccTmp = ccoCast(ccoCopy(ccType), ccoCopy(ccName));
		mainargs = listCons(CCode)(ccTmp, mainargs);


		/* How long is the return value? */
		ccLenArg = gc0GetTmp(ccoTypeIdOf(gcFiSInt));
		farg = foam->foamPCall.argv[0];
		ccTmp = ccoAsst(ccoCopy(ccLenArg), gccExpr(farg));
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


		/*
		 * We need to allocate a buffer for the result
		 * irrespective of whether or not the caller
		 * wants to use it.
		 */
		ccTmp = ccoPlus(ccoCopy(ccLenArg), ccoIdOf("1"));
		ccArg = ccoCast(ccoCopy(ccType), ccoCopy(ccName));
		ccTmp = ccoMany3(ccArg, ccoCopy(ccType), ccTmp);
		ccTmp = ccoFCall(ccoIdOf("fiARRNEW_Char"), ccTmp);
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


		/* We have an extra dummy argument */
		extraArg = 1;
		break;
	case FTN_FSComplex:
		/* Fall through */
	case FTN_FDComplex:
		if (!cmplxfns)
			comsgFatal(NULL, ALDOR_F_NoFCmplxProperty, "fortran-cmplx-fns");
		else if (strEqual(cmplxfns, "return-void")) { 
			ccType = ccoTypeIdOf(restype == FTN_FSComplex ?
					     gcFiComplexSF : gcFiComplexDF);
			if (numresultvars)
				mainargs = listCons(CCode)(ccoCast(ccoPostStar(ccType), 
								   gccRef(resultvar[numresultvars-1])), 
							   mainargs);  
			else {
				ccName = gc0GetTmp(ccType); 
				mainargs = listCons(CCode)(ccoPreAnd(ccName), mainargs);
			}
		} 
		else if (strEqual(cmplxfns, "disallowed"))
			bug("gccFortranPCall : Fortran function returning a complex result detected");
		else if (!strEqual(cmplxfns, "return-struct"))
			comsgFatal(NULL, ALDOR_F_BadFCmplxValue, cmplxfns);
		break;
	default:
		break;
	}


	/* We need to add some extra statements after the PCall */
	switch (restype) {
	case FTN_Character:
		/* Pull out the char result */
		ccTmp  = ccoCast(ccoCopy(ccType), ccoCopy(ccChrTmp));
		ccCast = ccoARef(ccoParen(ccTmp), ccoIdOf("0"));
		ccTmp  = ccoCast(ccoChar(), ccoCopy(ccCast));
		ccTmp  = ccoAsst(ccoCopy(ccName), ccTmp);

		closnullifys = listCons(CCode)(ccTmp, closnullifys);  
		break;
	case FTN_XLString:
		/*
		 * Kill the string return value because we
		 * don't know how long it is. They need to
		 * use FixedString() instead.
		 *
		 * Generate the code:
		 *
		 *    (char *)ccName[ccLen] = (char)0
		 */
		ccTmp  = ccoCast(ccoCopy(ccType), ccoCopy(ccName));
		ccCast = ccoARef(ccoParen(ccTmp), ccoCopy(ccLenArg));
		ccTmp  = ccoCast(ccoChar(), ccoIdOf("0"));
		ccTmp  = ccoAsst(ccCast, ccTmp);

		closnullifys = listCons(CCode)(ccTmp, closnullifys);  
		break;
	default:
		break;
	}


	/*
	 * The FOAM generation deals with passing values by reference.
	 * Those arguments which need to be passed to Fortran as a
	 * pointer will have been stored in a record for us already.
	 * Thus all we have to do here is take care of any special
	 * parameters such as String and function values.
	 */
	/* for (i = 0; i < argc; i++) { */
	for (i = 0; i < argc; i++) {
		if (extraArg && !i)
		{
			/* Here's one I prepared earlier ... */
			assert(ccLenArg);
			ccArg = ccoCopy(ccLenArg);
			mainargs = listCons(CCode)(ccArg, mainargs);
			continue;
		}


		fargfmt = argformat->foamDDecl.argv[i-extraArg];
		modifiablearg = gc0IsModifiableFortranArg(fargfmt);
		argtype       = gc0GetFortranType(fargfmt);
		farg          = foam->foamPCall.argv[i];	
		fargnocast    = (foamTag(farg) == FOAM_Cast) ?
			farg->foamCast.expr : farg;
		fmtype        = fargfmt->foamDecl.type;


		/* Char and Character are treated in the same way */
		if (fmtype == FOAM_Char)
			argtype = FTN_Character;


		switch (argtype) {
		case FTN_Character:
			/* Fall through */
		case FTN_String:
			/* Fall through */
		case FTN_XLString:
			ccArg = gccFtnXLstring(farg, argtype, modifiablearg, &ccChrlen);
			chrlenargs = listCons(CCode)(ccChrlen, chrlenargs);
			break;
		case FTN_StringArray:
			ccArg = gccFtnStringArray(farg, &ccChrlen);
			chrlenargs = listCons(CCode)(ccChrlen, chrlenargs);
			break;
		case FTN_FnParam:
			tmpstr = gdecl->foamGDecl.id;
			fnparam = gc0FtnFunParam(tmpstr, fnparamno++);
			ccArg = gccFtnFnParam(farg, fnparam, &ccTmp);
			closnullifys = listCons(CCode)(ccTmp, closnullifys);  
			break;
		case FTN_Array:
		case FTN_Word:
		case FTN_Machine:
		case FTN_Boolean:
		case FTN_SingleInteger:
		case FTN_FDouble:
		case FTN_FSingle:
		case FTN_FSComplex:
		case FTN_FDComplex:
			ccArg = gccExpr(farg);
			break;
		default: bug("gccFortranPCall: bad case");
		}


		/*
		 * Fortran doesn't use NUL terminated strings but
		 * Aldor usually does (and definitely does at this
		 * low-level). We need to add an extra statement for
		 * each String/Character/FixedString argument to be
		 * executed after the Fortran function returns.
		 * The statement simply terminates the string so
		 * that other code can work safely. We generate:
		 *
		 *    (char *)ccArg[ccLen] = (char)0
		 */
		switch (argtype) {
		case FTN_String:
			/* Fall through */
		case FTN_XLString:
			/*
			 * Kill the string return value because we
			 * don't know how long it is. They need to
			 * use FixedString() instead.
			 *
			 * Generate the code:
			 *
			 *    (char *)ccName[ccLen] = (char)0
			 */
			ccTmp  = ccoPostStar(ccoChar());
			ccTmp  = ccoCast(ccoCopy(ccTmp), ccoCopy(ccArg));
			ccCast = ccoARef(ccoParen(ccTmp), ccoCopy(ccChrlen));
			ccTmp  = ccoCast(ccoChar(), ccoIdOf("0"));
			ccTmp  = ccoAsst(ccCast, ccTmp);

			closnullifys = listCons(CCode)(ccTmp, closnullifys);  
			break;
		default:
			break;
		}
		mainargs = listCons(CCode)(ccArg, mainargs);  
	}
	mainargs = listNReverse(CCode)(mainargs);
	chrlenargs = listNReverse(CCode)(chrlenargs);
	argslist = listNConcat(CCode)(mainargs, chrlenargs);
	ccArgs = gc0ListOf(CCO_Many, argslist);
	listFree(CCode)(argslist);
	if (closnullifys) {
		closnullifys = listNReverse(CCode)(closnullifys);
		*closnulls = closnullifys;
	} 
	return ccoFCall(gccExpr(foam->foamPCall.op), ccArgs);
}

local CCode
gccFtnXLstring(Foam farg, FortranType type, Bool modifiablearg, CCode *ccChrlen)
{
	Foam	fargnocast;
	CCode	ccArgStr;
	CCode	ccType, ccCast1, ccCast2;
	CCode	ccTmp, ccTmpLen, ccTmpStr, ccTmpLenPlus;
	FoamTag	fargcast = (FoamTag)0;


	/* Obtain a version of the argument without a cast */
	if (foamTag(farg) == FOAM_Cast)
	{
		fargcast   = farg->foamCast.type;
		fargnocast = farg->foamCast.expr;
	}
	else
		fargnocast = farg;


	/* Safety checks */
	assert(ccChrlen);
	assert(genIsVar(fargnocast));


	/*
	 * If the argument is an expression rather than an
	 * identifier then we store it in a temporary. This
	 * code assumes that all modifiable arguments reach
	 * us as an lvalue (eg a temporary). Hopefully this
	 * is a reasonable assumption since the FOAM creator
	 * needs to know which lvalue is going to be modified
	 * so that it can update the reference after the call.
	 *
	 *    ccArgStr = ccArg;
	 */
	if (modifiablearg)
	{
		/* Copy the argument */
		ccArgStr = gccRef(fargnocast);


		/* We may need to cast this to something else */
		if (fargcast)
		{
			ccTmp    = gc0TypeId(fargcast, emptyFormatSlot);
			ccArgStr = ccoCast(ccTmp, ccArgStr);
		}
	}
	else
	{
		ccType     = gc0TypeId(FOAM_Word, emptyFormatSlot);
		ccArgStr   = gc0GetTmp(ccType);
		ccTmp      = ccoCopy(gccExpr(farg));
		ccTmp      = ccoAsst(ccoCopy(ccArgStr), ccTmp);
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));
	}


	/*
	 * Always pass a copy of the string: string literals
	 * are usually stored in a read-only segment of the
	 * process and we will cause a seg-fault if Fortran
	 * tries to modify the value passed to it. Note that
	 * Fortran will hopefully give us an unaliased copy
	 * of the string on return.
	 *
	 * For character we generate the code:
	 *
	 *    ccTmpLen = 1;
	 *    fiARRNEW_Char(ccTmpStr, FOAM_Word, ccTmpLen+1);
	 *    strncpy((char *)ccTmpStr, (char *)ccArgStr, ccTmpLen);
	 *    ((char *)ccTmpStr)[ccTmpLen] = (char)0;
	 *
	 * while for real strings we generate:
	 *
	 *    ccTmpLen = strlen((char *)ccArgStr);
	 *    fiARRNEW_Char(ccTmpStr, FOAM_Word, ccTmpLen+1);
	 *    strncpy((char *)ccTmpStr, (char *)ccArgStr, ccTmpLen);
	 *    ((char *)ccTmpStr)[ccTmpLen] = (char)0;
	 */
	ccType     = gc0TypeId(FOAM_SInt, emptyFormatSlot);
	ccTmpLen   = gc0GetTmp(ccType);

	if (type != FTN_Character)
	{
		ccTmp      = ccoPostStar(ccoChar());
		ccTmp      = ccoCast(ccTmp, ccoCopy(ccArgStr));
		ccTmp      = ccoFCall(ccoIdOf("strlen"), ccTmp);
	}
	else
		ccTmp = ccoIdOf("1");

	ccTmp      = ccoAsst(ccoCopy(ccTmpLen), ccTmp);
	gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


	/* Compute ccTmpLen + 1 */
	ccTmpLenPlus = ccoPlus(ccoCopy(ccTmpLen), ccoIdOf("1"));


	/* Allocate a buffer for the string */
	ccType     = gc0TypeId(FOAM_Word, emptyFormatSlot);
	ccTmpStr   = gc0GetTmp(ccType);
	ccTmp      = ccoCopy(ccTmpLenPlus);
	ccTmp      = ccoMany3(ccoCopy(ccTmpStr), ccoCopy(ccType), ccTmp);
	ccTmp      = ccoFCall(ccoIdOf("fiARRNEW_Char"), ccTmp);
	gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


	/* Copy our argument across */
	ccTmp      = ccoPostStar(ccoChar());
	ccCast1    = ccoCast(ccoCopy(ccTmp), ccoCopy(ccTmpStr));
	ccCast2    = ccoCast(ccoCopy(ccTmp), ccoCopy(ccArgStr));
	ccTmp      = ccoMany3(ccCast1, ccCast2, ccoCopy(ccTmpLen));
	ccTmp      = ccoFCall(ccoIdOf("strncpy"), ccTmp);
	gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


	if (modifiablearg)
	{
		/*
		 * Update the temporary variable used for the
		 * reference with the pointer to the new string
		 * value. This may be modified by Fortran and
		 * can then be written back to the reference.
		 * We can only do this here before the call is
		 * actually made if different temporaries were
		 * used for each string ...
		 */
		ccTmp = ccoAsst(ccoCopy(ccArgStr), ccoCopy(ccTmpStr));
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));
	}


	*ccChrlen = ccoCopy(ccTmpLen);
	return ccoCopy(ccTmpStr);
}

local CCode
gccFtnStringArray(Foam foam, CCode *ccChrlen)
{
	Foam	arr, len;
	CCode	ccarr, cclen;


	/* Must be a FOAM_Values with two elements */
	assert(foamTag(foam) == FOAM_Values);
	assert(foamArgc(foam) == 2);
	assert(ccChrlen);


	/* Split into two components: data and length */
	arr = foam->foamValues.argv[0];
	len = foam->foamValues.argv[1];


	/*  Convert both parts into C */
	ccarr = gccExpr(arr);
	cclen = gccExpr(len);


	/* Tell our caller about the length */
	*ccChrlen = ccoCopy(cclen);


	/* Return the data component */
	return ccoCopy(ccarr);
}


local CCode
gccFtnFnParam(Foam farg, FtnFunParam info, CCode *pClos)
{
	char	num[20];
	CCode	ccTmp;
	CCode	ccClos = gc0FtnFunClosure(info);


	/* Make sure we don't recurse */
	(void)sprintf(num, "%d", FOAM_Halt_BadFortranRecursion);
	ccTmp = gcFiHalt(ccoIdOf(num));
	ccTmp = ccoIf(ccoCopy(ccClos), ccTmp, NULL);
	gc0AddTopLevelStmt(gcvStmts, ccoStat(ccTmp));


	/* Store the function pointer in a temporary at the top level */
	ccTmp = ccoStat(ccoAsst(ccoCopy(ccClos), gccExpr(farg)));
	gc0AddTopLevelStmt(gcvStmts, ccTmp);


	/* Construct the code to nullify this function pointer */
	if (pClos) *pClos = ccoAsst(ccoCopy(ccClos), ccoIdOf("NULL"));


	/* Return the name of the wrapper function (to be created) */
	return ccoCopy(gc0FtnFunName(info));
}


/*****************************************************************************
 * 
 * :: Return the C function call of the multiple format Foam, 'foam'.
 *
 ****************************************************************************/

local CCode
gccMFmt(Foam foam)
{
	CCode	cc;
	int     fmt  = foam->foamMFmt.format;
	Foam	expr = foam->foamMFmt.value;

	if (fmt == emptyFormatSlot)
		bug("MFmt used with empty format!");

	switch (foamTag(expr)) {
	  case FOAM_BCall:
		cc = gc0FunBCall(expr, fmt);
		break;
	  case FOAM_CCall:
		cc = gc0FunFoamCall(expr, fmt);
		break;
	  case FOAM_OCall:
		cc = gc0FunFoamCall(expr, fmt);
		break;
	  case FOAM_PCall:
		cc = gc0FunFoamCall(expr, fmt);
		break;
	  case FOAM_Catch:
		/* Only allowed (Set (Values ...) (MFmt f (Catch ...))) */
		bug("gccMFmt: Catch in MFmt missed by gc0Set");
		NotReached(cc = 0);
		break;
	  default:
		bugBadCase(foamTag(expr));
		NotReached(cc = 0);
		break;
	}
	return cc;
}

local CCode
gccValues(Foam foam)
{
	return gccUnhandled(foam);
}

/*****************************************************************************
 * 
 * :: Return the C code of the given Foam value, 'foam'.
 *
 ****************************************************************************/

local CCode
gccVal(Foam foam)
{
	CCode	cc;

	switch (foamTag(foam)) {
	  case FOAM_Nil:
		cc = ccoIdOf(gcFiNil);
		break;
	  case FOAM_Char:
	  case FOAM_Bool:
	  case FOAM_Byte:
	  case FOAM_HInt:
	  case FOAM_SInt:
		cc = ccoIntOf(foamArgv(foam)[0].data);
		break;
	  case FOAM_BInt:
		cc = gccBInt(foam);
		break;
	  case FOAM_SFlo:
		cc = ccoFloatOf(gcvFloatBuf, foamToSFlo(foam));
		break;
	  case FOAM_DFlo:
		cc = ccoFloatOf(gcvFloatBuf, foamToDFlo(foam));
		break;
	  case FOAM_Arr:
		cc = gccArr(foam);
		break;
	  case FOAM_Rec:
		cc = gccRec(foam);
		break;
	  case FOAM_Prog:
		cc = gc0Prog(foamArgv(foam)[0].code, foamArgv(foam)[1].code);
		break;
	  case FOAM_Clos:
		cc = gccClos(foam);
		break;
	  case FOAM_Ptr:
		cc = gccVal(foam->foamPtr.val);
		break;
	  case FOAM_Protect:
		cc = gc0Protect(foam);
		break;
	  case FOAM_PushEnv:
		cc = gccPushEnv(foam);
		break;
	  default:
		cc = gccRef(foam);
		break;
	}
	return cc;
}

/*****************************************************************************
 * 
 * :: Return the C code to create big integers from the big integer data
 *    given in 'foam'.
 *
 ****************************************************************************/

/*
 * !! Since bintIsSmall may be different for different machines, 
 *    the code generated is not strictly portable
 */

local CCode
gccBInt(Foam foam)
{
	CCode	globint, globdata, globval, globstmt,
		globAName, globBName;
	BInt    bint;
	Length	i, size, isNeg;
	int	isize;
	String	px;

	/*
	 * FIXME: keep a table indexed by BInt with a value that will
	 * allow globAName and globBName to be recreated. We will only
	 * create unique names for each distinct value rather than for
	 * all values irrespective of their uniqueness. The idea is to
	 * reduce the number of globals we create.
	 */

	bint = foam->foamBInt.BIntData ;
	if (gc0OverSMax()) {
		Foam	decl = gcvConst->foamDDecl.argv[0];
		globAName = gc0MultVarId("GA", gcvNBInts, decl->foamDecl.id);
		globBName = gc0MultVarId("GB", gcvNBInts, decl->foamDecl.id);
	}
	else {
		globAName = gc0VarId("GA", gcvNBInts);
		globBName = gc0VarId("GB", gcvNBInts);
	}
	/*
	 * Append the bigint variable to global variable list.
	 */
	if (gc0OverSMax()) {
		CCode	defbint;
		defbint = ccoDecl(ccoTypeIdOf(gcFiBInt), ccoCopy(globBName));
		gc0AddLine(gcvDefCC, defbint);
		globint = ccoDecl(ccoType(ccoExtern(), ccoTypeIdOf(gcFiBInt)),
				  ccoCopy(globBName));
	}
	else
		globint = ccoDecl(ccoType(ccoStatic(), ccoTypeIdOf(gcFiBInt)),
				  ccoCopy(globBName));
	gc0AddLine(gcvGloCC, globint);

	if (bintIsNeg(bint))
		isNeg = 1;
	else
		isNeg = 0;
	
	if (bintIsSmall(bint)) {
		size = 1;
		px = strPrintf("0x%0lX", bintSmall(bint));
		globval = ccoIdOf(px);
		if (gc0OverSMax())
			globdata = ccoDecl(ccoTypeIdOf("long"),
					   ccoAsst(ccoCopy(globAName),
						   globval));
		else
			globdata = ccoDecl(ccoType(ccoStatic(),
						   ccoTypeIdOf("long")),
					   ccoAsst(ccoCopy(globAName),
						   globval));
	}
	else {
		U16 *data;
		bintToPlacevS(bint, &isize, &data);
		globval = ccoNewNode(CCO_Many, isize);
		for (i = 0; i < isize; i++) {
			px = strPrintf("0x%0X", data[i]);
			ccoArgv(globval)[i] = ccoIdOf(px);
		}
		bintReleasePlacevS(data);

		if (gc0OverSMax())
			globdata = ccoDecl(ccoTypeIdOf("FiBIntS"),
					   ccoAsst(ccoARef(ccoCopy(globAName), 
							   NULL),
						   ccoInit(globval)));
		else
			globdata = ccoDecl(ccoType(ccoStatic(),
						   ccoTypeIdOf("FiBIntS")),
					   ccoAsst(ccoARef(ccoCopy(globAName),
							   NULL),
						   ccoInit(globval)));
	}
	/*
	 * Append the bigint data initialization to global variable list.
	 */
	gc0AddLine(gcvDefCC, globdata);

	/*
	 * Create assignment statement for the initialization prog C0.
	 */
	if (bintIsSmall(bint))
#ifdef USE_MACROS
		globstmt =  ccoStat(ccoFCall(ccoIdOf("fiBINT_FR_INT"),
					     ccoMany3(ccoCopy(globBName),
						  ccoIdOf(gcFiBInt),
						  ccoCopy(globAName))));
#else
		globstmt =  ccoStatAsst(ccoCopy(globBName),
					ccoFCall(ccoIdOf("fiBIntFrInt"),
						 ccoCopy(globAName)));
#endif
	else
		globstmt =  ccoStatAsst(ccoCopy(globBName),
					ccoFCall(ccoIdOf("fiBIntFrPlacev"),
						 ccoMany3(ccoIntOf(isNeg),
							  ccoIntOf(isize),
							  ccoCopy(globAName))));
	gc0AddLine(gcvBIntCC, globstmt);
	gcvNBInts++;
	return ccoCopy(globBName);
}

/*****************************************************************************
 * 
 * :: Return the C code to create a string from a Foam array, given in 'foam'.
 *
 ****************************************************************************/

local CCode
gccArr(Foam foam)
{
	String	str = foamArrToString(foam);

	return ccoStringOf(str);
}

/*****************************************************************************
 * 
 * :: Return the C code function which creates a new structure whose format
 *    and fields are given by the Foam record, 'foam'.
 *
 ****************************************************************************/

local CCode
gccRRElt(Foam foam)
{
	CCode	ccRec, ccFmt, ccElt, ccArgs;

	/* Now generate the record access */
	ccRec  = gccExpr(foam->foamRRElt.data);
	ccFmt  = gccExpr(foam->foamRRElt.fmt);
	ccElt  = gccExpr(foamNewSInt(foam->foamRRElt.field));
	ccArgs = ccoMany3(ccRec, ccElt, ccFmt);

	return ccoFCall(ccoIdOf("fiRawRecordElt"), ccArgs);
}

local CCode
gccRec(Foam foam)
{
	CCodeList	code = listNil(CCode);
	int		fmt, nfields, i;
	CCode		ccRecFill;
	String		newFmtName;

	fmt = foam->foamRec.format;
	nfields = foamArgc(foam) - 1;
	for (i = 0; i < nfields; i++)
		gc0AddLine(code, gccVal(foam->foamRec.eltv[i]));
	code = listNReverse(CCode)(code);
	ccRecFill = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	newFmtName = strPrintf("fiRecNew%s", gcFmtName);
	return ccoFCall(gc0VarId(newFmtName, fmt), ccRecFill);
}

/*****************************************************************************
 * 
 * :: Return the C code function which creates a new closure of the
 *    environment and program given by the Foam closure 'foam'.
 *
 ****************************************************************************/

local CCode
gccClos(Foam foam)
{
	Foam	env, prog;

	assert(foamTag(foam) == FOAM_Clos);
	env  = foam->foamClos.env;
	prog = foam->foamClos.prog;
	return ccoFCall(ccoIdOf("fiClosMake"), ccoMany2(gccVal(env),
							gccExpr(prog)));
}

/*****************************************************************************
 * 
 * :: Return the C code for the Foam reference, 'foam'.
 *
 ****************************************************************************/

local CCode
gccRef(Foam foam)
{
	CCode	cc;

	switch (foamTag(foam)) {
	  case FOAM_AElt:
		cc = gccAElt(foam);
		break;
	  case FOAM_RElt:
		cc = gccRElt(foam);
		break;
	  case FOAM_RRElt:
		cc = gccRRElt(foam);
		break;
	  case FOAM_IRElt:
		cc = gccIRElt(foam);
		break;
	  case FOAM_TRElt:
		cc = gccTRElt(foam);
		break;
	  case FOAM_Fluid:
		cc = gc0FluidRef(foam);
		break;
	  case FOAM_EElt:
	  case FOAM_Const:
	  case FOAM_Glo:
	  case FOAM_Par:
	  case FOAM_Loc:
	  case FOAM_Lex:
	  case FOAM_Env:
		cc = gccId(foam);
		break;
	  case FOAM_EEnv:
		cc = gccEEnv(foam);
		break;
	  case FOAM_EInfo:
		cc = gccEInfo(foam);
		break;
	  case FOAM_PRef:
		cc = gccPRef(foam);
		break;
	  case FOAM_CEnv:
		cc = ccoPointsTo(gc0SubExpr(foam->foamCEnv.env,
					    ccoTypeIdOf(gcFiClos)),
				 ccoIdOf("env"));
		break;
	  case FOAM_CProg:
		cc = ccoPointsTo(gc0SubExpr(foam->foamCProg.prog,
					    ccoTypeIdOf(gcFiClos)),
				 ccoIdOf("prog"));
		break;
          case FOAM_Catch:
		cc = gccId(foam->foamCatch.ref);
		break;
	  default:
		bugBadCase(foamTag(foam));
		NotReached(return 0);
	}
	return cc;
}

/*****************************************************************************
 * 
 * :: Return the C code to reference an array element indicated in 'foam'.
 *
 ****************************************************************************/

local CCode
gccAElt(Foam foam)
{
	CCode	arrExpr;
	Foam	lhs;
	Bool    wasCast = false;

	lhs = foam->foamAElt.expr;
	if (foamTag(lhs) == FOAM_Cast) {
		assert(lhs->foamCast.type == FOAM_Arr);
		lhs = lhs->foamCast.expr;
		wasCast = true;
	}
	arrExpr = gccExpr(lhs);
	if (gc0IsDecl(lhs)) {
		Foam	decl = gc0GetDecl(lhs);
		int	fmt  = decl->foamDecl.format;
		if (fmt != foam->foamAElt.baseType || decl->foamDecl.type != FOAM_Arr)
			arrExpr = gc0SubExpr(lhs, 
				   ccoPostStar(gc0TypeId(
					    foam->foamAElt.baseType, emptyFormatSlot)));
	}
	else if (foamTag(lhs) == FOAM_AElt)
		arrExpr = gc0SubExpr(lhs, ccoPostStar(gc0TypeId(
					    foam->foamAElt.baseType, emptyFormatSlot)));
	else if (wasCast)
		arrExpr = gc0SubExpr(lhs, ccoPostStar(gc0TypeId(
					    foam->foamAElt.baseType, emptyFormatSlot)));

	return ccoARef(arrExpr, gccExpr(foam->foamAElt.index));
}

/*****************************************************************************
 * 
 * :: Return the C code to reference an structure field, using the Foam
 *    record element, 'foam'.
 *
 ****************************************************************************/

local CCode
gccRElt(Foam foam)
{
	CCode	ccName, ccField;
	Foam	decl, lhs;

	decl  = gc0GetDecl(foam);
	lhs   = foam->foamRElt.expr;
	if (foamTag(lhs) == FOAM_Cast)
		lhs = lhs->foamCast.expr;
	ccField = gc0MultVarId("X", foam->foamRElt.field, decl->foamDecl.id);
	ccName = gccExpr(lhs);
	if (gc0IsDecl(lhs)) {
		Foam ldecl = gc0GetDecl(lhs);
		int	fmt = ldecl->foamDecl.format;
		if (fmt && fmt != foam->foamRElt.format)
			ccName = ccoCast(gc0VarId(gcFmtType,
						  foam->foamRElt.format),
					 ccName);
	}
	else
		ccName = ccoCast(gc0VarId(gcFmtType,
					  foam->foamRElt.format), ccName);
	return ccoPointsTo(ccName, ccField);
}

local CCode
gccIRElt(Foam foam)
{
	CCode	ccName, ccField;
	Foam	decl, lhs;

	decl  = gc0GetDecl(foam);
	lhs   = foam->foamIRElt.expr;
	if (foamTag(lhs) == FOAM_Cast)
		lhs = lhs->foamCast.expr;
	ccField = gc0MultVarId("X", foam->foamIRElt.field, decl->foamDecl.id);
	ccName = gccExpr(lhs);
	if (gc0IsDecl(lhs)) {
		Foam ldecl = gc0GetDecl(lhs);
		int	fmt = ldecl->foamDecl.format;
		if (fmt && fmt != foam->foamIRElt.format)
			ccName = ccoCast(gc0VarId(gcFmtType,
						  foam->foamIRElt.format),
					 ccName);
	}
	else
		ccName = ccoCast(gc0VarId(gcFmtType,
					  foam->foamIRElt.format), ccName);

	return ccoPointsTo(ccName, ccField);
}

local CCode
gccTRElt(Foam foam)
{
	CCode	ccName, ccField, ccIdx;
	Foam decl, lhs;

	decl  = gc0GetDecl(foam);
	lhs   = foam->foamTRElt.expr;
	if (foamTag(lhs) == FOAM_Cast)
		lhs = lhs->foamCast.expr;
	ccField = gc0MultVarId("X", foam->foamTRElt.field, decl->foamDecl.id);
	ccName = gccExpr(lhs);
	if (gc0IsDecl(lhs)) {
		Foam ldecl = gc0GetDecl(lhs);
		int	fmt = ldecl->foamDecl.format;
		if (fmt && fmt != foam->foamTRElt.format)
			ccName = ccoCast(gc0VarId(gcFmtType,
						  foam->foamTRElt.format),
					 ccName);
	}
	else
		ccName = ccoCast(gc0VarId(gcFmtType,
					  foam->foamTRElt.format), ccName);
	ccIdx = gccExpr(foam->foamTRElt.index);
	return ccoDot(ccoARef(ccoPointsTo(ccName, ccoIdOf(gcFiTRTail)), ccIdx),
		      ccField);

}

local CCode
gccTRNew(Foam foam)
{
	int    fmt = foam->foamTRNew.format;
	CCode  sz  = gccExpr(foam->foamTRNew.size);
	CCode  cco;

	cco = gcFiNARYNew(gcFmtName, gcTFmtName, fmt, sz);

	return cco;
}

local CCode
gccEEnv(Foam foam)
{
	return gc0EEnv(gccExpr(foam->foamEEnv.env), foam->foamEEnv.level);
}
    
local CCode
gccEInfo(Foam foam)
{
	return gcFiEnvInfo(gccExpr(foam->foamEInfo.env)); 
}
 
local CCode
gccPRef(Foam foam)
{
  assert (foam->foamPRef.idx == 0);
  return gcFiProgHashCode(gccExpr(foam->foamPRef.prog)); 
}
 
/*
 * Return an identifier.
 */

local CCode
gccId(Foam foam)
{
	CCode	cc;

	switch (foamTag(foam)) {
	  case FOAM_Glo:
	  case FOAM_Const:
	  case FOAM_Par:
	  case FOAM_Loc:
		cc = gccGetVar(foam);
		break;
	  case FOAM_Fluid:
		cc = gccGetVar(foam);
		break;
	  case FOAM_Lex:
		cc = ccoPointsTo(gc0VarId("l", foam->foamLex.level),
				 gccGetVar(foam));
		break;
	  case FOAM_EElt:
		{
		  CCode	id, next;
		  id   = gc0VarId(gcFmtType, foam->foamEElt.env);
		  next = gc0EEltNext(foam->foamEElt.ref, foam->foamEElt.level);
		  cc   = ccoPointsTo(ccoCast(ccoTypedefId(id), next),
				     gccGetVar(foam));
		  break;
		}
	  case FOAM_Env:
		{
		int level = foam->foamEnv.level;

		if (gcvIsLeaf && level == 0 && 
		    (gcvLFmtStk->foamDEnv.argv[level] == emptyFormatSlot
		     || gcvLFmtStk->foamDEnv.argv[level] == envUsedSlot))
			cc = gcFiEnvPush(ccoIntOf(int0),
					 gc0VarId("e", level+1));
		else
			cc = gc0VarId("e", level);
		/* !! Because sometimes usage info is lost */
		if (gc0EmptyEnv(gcvLFmtStk->foamDEnv.argv[level]) && level!=0) {
			gcvLFmtStk = foamCopy(gcvLFmtStk);
			gcvLFmtStk->foamDEnv.argv[level] = envUsedSlot;
		}
		break;
	      }
	  default:
		cc = gccExpr(foam);
		break;
	}
	return cc;
}


/* Given a constant, it returns the name of the corresponding function.
 * In example, for constant 2 it returns CF2_....
 * This identifier should be used only:
 *     - to declare/define the function
 *     - to initialize the corresponding prog  
 */
local CCode
gccProgId(Foam foam)
{
	int 		idx = foam->foamConst.index;
	String 		tag = "CF";
	Foam		decl = gc0GetDecl(foam);

	assert(foamTag(foam) == FOAM_Const);

	if (gc0OverSMax()) {
		if (idx) {
			String	s;
			Foam	fn = gcvConst->foamDDecl.argv[0];
		
			s = strPrintf("%s_%s", fn->foamDecl.id,
					decl->foamDecl.id);
			return gc0MultVarId(tag, idx, s);
			}
			else
				return gc0MultVarId(tag, idx, decl->foamDecl.id);
	}
	else
		return gc0MultVarId(tag, idx, decl->foamDecl.id);
			

}

/*
 * Get a variable identifier.
 */
local CCode
gccGetVar(Foam foam)
{
	Foam	decl = gc0GetDecl(foam);
	CCode 	ccode;
	String	s, s0;
	int	idx;

	s = s0 = decl->foamDecl.id;

	switch (foamTag(foam)) {
	case FOAM_Glo:
		idx = foam->foamGlo.index;
		switch (foamProtoBase(decl->foamGDecl.protocol)) {
		case FOAM_Proto_Foam:
		case FOAM_Proto_Init:

			if (!foamGDeclIsExport(decl)) {
				ccode = gc0MultVarId("pG", idx, s);
				ccode = ccoParen(ccoPreStar(ccode));
			}
			else 
				ccode = gc0MultVarId("G", idx, s);
			break;
		case FOAM_Proto_C:	
			if (strchr(s, FOREIGN_INCLUDE_SEPARATOR)) {
				s = strCopy(s);
				s = gc0StompOffIncludeFile(s, FOAM_Proto_C);
			}
			if (foamGDeclIsExport(decl))
			  ccode = gc0MultVarId("G", idx, s);
			else
			  ccode = ccoIdOf(s);
			break;
		case FOAM_Proto_Fortran:
			if (foamGDeclIsExport(decl))
				ccode = gc0MultVarId("G", idx, s);
			else {
				s = gc0GenFortranName(s);
				ccode = ccoIdOf(s);
			}
			break;
		case FOAM_Proto_Java:
			ccode = gc0MultVarId("J", idx, "java");
			break;
		case FOAM_Proto_Other:
			ccode = ccoIdOf(s);
			break;
		default:
			comsgFatal(NULL, ALDOR_F_ProtoNotSupported, "C",
			     foamProtoInfo(decl->foamGDecl.protocol).str);
			NotReached(return 0);
		}
		break;
	  case FOAM_Const:
		idx = foam->foamConst.index;
		if (gc0OverSMax() && idx)
			s = strPrintf("%s_%s", 
			              gcvConst->foamDDecl.argv[0]->foamDecl.id,
				      decl->foamDecl.id
			);
		ccode = gc0MultVarId("C", foam->foamConst.index, s);
		break;
	  case FOAM_Par:
		ccode = gc0MultVarId("P", foam->foamPar.index, s);
		break;
	  case FOAM_Loc:
		ccode = gc0MultVarId("T", foam->foamLoc.index, s);
		break;
	  case FOAM_Lex:
		ccode = gc0MultVarId("X", foam->foamLex.index, s);
		break;
	  case FOAM_EElt:
		ccode = gc0MultVarId("X", foam->foamEElt.lex, s);
		break;
	  case FOAM_Fluid:
		ccode = gc0MultVarId("F", foam->foamFluid.index, s);
		break;
	  default:
		bugBadCase(foamTag(foam));
		NotReached(return 0);
	}
	if (s0 != s) strFree(s);

	return ccode;
}

local CCode
gccUnhandled(Foam foam)
{
	String	buf;

	bugWarning("foamTag: %s is unhandled.\n", foamStr(foamTag(foam)));
	buf = strPrintf("foamTag: %s is unhandled.", foamStr(foamTag(foam)));
	return ccoComment(ccoIdOf(buf));
}

/*****************************************************************************
 *
 * :: Functions for determining environment levels
 *
 ****************************************************************************/

local CCodeList
gc0Levels(int nLexs, int maxLev, int isLeaf, int fmt)
{
	CCodeList	code = listNil(CCode);
	int		i, level;

	for (i = 0; i < nLexs; i++) {
		level = gcvLFmtStk->foamDEnv.argv[i];
		if (!gc0EmptyEnv(level) && level != envUsedSlot)
			if (foamArgc(gcvFmt->foamDFmt.argv[level]) > 0)
				gc0AddLine(code, gc0LexRef(level, i));
		if (!gc0EmptyEnv(level) && i != 1)
			gc0AddLine(code, gc0EnvRef(i));
		if (gc0EmptyEnv(level) && i > 1 && i <= maxLev)
			gc0AddLine(code, gc0EnvRef(i));
	}
	if (!isLeaf) {
		if (gc0EmptyEnv(fmt))
			gc0AddLine(code, gc0EnvRef(int0));
		if (!gc0EmptyEnv(fmt) && fmt != envUsedSlot)
			if (foamArgc(foamArgv(gcvFmt)[fmt].code) > 0)
				gc0AddLine(code, gc0EnvMake(fmt));
		gc0AddLine(code, gc0EnvPush(fmt));
	}
	for (i = 1; i <= maxLev; i++) {
		level = gcvLFmtStk->foamDEnv.argv[i];
		if (i != 1)
			gc0AddLine(code, gc0EnvNext(i, i-1));
		if (!gc0EmptyEnv(level) && level != envUsedSlot)
			gc0AddLine(code, gc0EnvLevel(i, level));
	}
	code = listNReverse(CCode)(code);
	return code;
}

local CCode
gc0LexRef(int fmt, int i)
{
	return ccoDecl(ccoTypedefId(gc0VarId(gcFmtType, fmt)),
		       gc0VarId("l", i));
}

local CCode
gc0EnvRef(int i)
{
	return ccoDecl(ccoTypeIdOf(gcFiEnv), gc0VarId("e", i));
}

local CCode
gc0LocRef(Foam locals, int i)
{
	Foam	decl;

	decl = locals->foamDDecl.argv[i];
	return gc0IdDecl(locals, FOAM_Loc, decl, int0, i);
}

local CCode
gc0EnvMake(int fmt)
{
	if (gc0EmptyFormat(fmt)) return ccoIdOf(gcFiNil);
	return ccoStatAsst(gc0VarId("l", int0),
			   gcFi0New(gcFmtName, fmt, "CENSUS_EnvLevel"));
}

local CCode
gc0EnvPush(int fmt)
{
	CCode	ccEnvPush;

	if (gcvIsLeaf)
		ccEnvPush = ccoCast(ccoTypeIdOf(gcFiEnv),
				    ccoInit(ccoMany2(ccoIdOf(gcFiNil),
						     gc0VarId("e", 1))));
	else if (gc0EmptyFormat(fmt))
		ccEnvPush = gcFiEnvPush(ccoIdOf(gcFiNil), gc0VarId("e", 1));
	else
		ccEnvPush = gcFiEnvPush(gc0VarId("l", int0), gc0VarId("e", 1));
	return ccoStatAsst(gc0VarId("e", int0), ccEnvPush);
}

local CCode
gc0EnvNext(int id, int count)
{
	CCode	cc = 0;
	int	i;

	if (count == 1 && id > 2) {
		for (i = 1; i < id; i++)
			cc = gcFiEnvNext(gc0VarId("e", id-2));
	}
	else
		cc = gcFiEnvNext(gc0VarId("e", id-1));
	return ccoStatAsst(gc0VarId("e", id), cc);
}

local CCode
gc0EnvLevel(int id, int level)
{
	return ccoStatAsst(gc0VarId("l", id),
			   ccoCast(ccoTypedefId(gc0VarId(gcFmtType, level)),
				   gcFiEnvLevel(gc0VarId("e", id))));
}

/*****************************************************************************
 *
 * :: C code generating utility functions.
 *
 ****************************************************************************/

/*
 * Create a main function definition and global variables for the arguments.
 */

local CCode
gc0MainDef(String name)
{
	CCode  		ccParm, ccBody, ccInitProg;
	CCode 		flag, var, stmt, call;
	CCodeList 	stmts;
	String		ftnInitFn = (String)NULL;


	/* Check if we need any Fortran I/O initialising code */
	if (ccFortran)
	{
		/*
		 * We are compiling with a Fortran RTS so check if
		 * we need to invoke an I/O initialisation function
		 */
		ftnInitFn = compCfgLookupString("fortran-io-init-fun");
		if (ftnInitFn) { if (!*ftnInitFn) ftnInitFn = (String)NULL; }
	}


	ccInitProg = gc0MultVarId("pG", int0, name);

	ccParm   = ccoMany2(
		         ccoParam(ccoIdOf("argc"), ccoInt(),
				ccoIdOf("argc")),
			 ccoParam(ccoIdOf("argv"), ccoChar(),
				ccoPreStar(ccoPreStar(ccoIdOf("argv")))));

        /* 
	 * FiBool flag;
	 * FiWord var;
	 * mainArgc = argc;
	 * mainArgv = argv;
	 * fiInitialiseFpu();
	 * <fortran-io-init-fn>();
	 * INIT__0_YYY();
	 * fiImportGlobal("G_XXXXX_try", pG_XXXXX_YYY);
	 * fiBlock(T1, T2, T3, fiCCall0(FiClos, *pG_XXXXX_YYY));
	 * if (!T1) fiUnhandledException(T3)
	 * return 0;
	 */
	stmts = listNil(CCode);
	flag = ccoIdOf("flag");
	var  = ccoIdOf("var");
	stmt  = ccoDecl(ccoTypeIdOf(gcFiBool), flag);
	stmts = listCons(CCode)(stmt, stmts);
	stmt  = ccoDecl(ccoTypeIdOf(gcFiWord), var);
	stmts = listCons(CCode)(stmt, stmts);
	stmt  = ccoStatAsst(ccoIdOf("mainArgc"), ccoIdOf("argc"));
	stmts = listCons(CCode)(stmt, stmts);
	stmt  = ccoStatAsst(ccoIdOf("mainArgv"), ccoIdOf("argv"));
	stmts = listCons(CCode)(stmt, stmts);
	stmt  = ccoStat(ccoFCall(ccoIdOf("fiInitialiseFpu"), ccoMany0()));
	stmts = listCons(CCode)(stmt, stmts);

	if (ftnInitFn)
	{
		stmt  = ccoStat(ccoFCall(ccoIdOf(ftnInitFn), ccoMany0()));
		stmts = listCons(CCode)(stmt, stmts);
	}

       	stmt  = ccoStat(ccoFCall(gc0MultVarId(gcFiInitModulePrefix,
					     int0, name), int0));
	stmts = listCons(CCode)(stmt, stmts);
	stmt  = ccoStat(ccoFCall(ccoIdOf("fiImportGlobal"),
				ccoMany2(ccoStringVal(gc0MultVarId("G", int0, name)
						      ->ccoToken.symbol),
					 ccoCopy(ccInitProg))));
	stmts = listCons(CCode)(stmt, stmts);
	call  = gcFiCCallN(int0, ccoMany2(ccoIdOf(gcFiWord),
					 ccoPreStar(ccInitProg)));

	stmt  = ccoStat(ccoFCall(ccoIdOf("fiBlock"),
				ccoMany4(ccoCopy(flag), 
					 ccoCopy(var),
					 ccoCopy(var),
					 call)));
	stmts = listCons(CCode)(stmt, stmts);
	stmt  = ccoStat(ccoIf(ccoLNot(ccoCopy(flag)),
			      ccoFCall(ccoIdOf("fiUnhandledException"),
				       ccoMany1(ccoCopy(var))),
			      NULL));
				      
	stmts = listCons(CCode)(stmt, stmts);
	stmt  = ccoReturn(ccoIntOf(int0));
	stmts = listCons(CCode)(stmt, stmts);
	stmts = listNReverse(CCode)(stmts);
	
	ccBody = gc0ListOf(CCO_Many, stmts);

	listFree(CCode)(stmts);

	return ccoFDef(ccoType(ccoExtern(), ccoTypeIdOf("int")),
		       ccoIdOf("main"), 
		       ccParm,
		       ccoCompound(ccBody));
}

local CCode
gc0ClosInit(Foam ref, Foam val)
{
	Foam		prog, decl, tmp;
	CCode		ccClos, ccLeft, ccRight, type;

	decl    = gc0GetDecl(ref);

	if (foamTag(val) != FOAM_Clos) {
		int fmt = decl->foamDecl.format;

		type = gc0TypeId(decl->foamDecl.type, fmt);
		tmp = val;
		while (foamTag(tmp) == FOAM_Cast)
			tmp = tmp->foamCast.expr;
		return ccoDecl(type,foamTag(tmp)==FOAM_Nil 
				    ? gccId(ref)
			            : ccoAsst(gccId(ref), gccExpr(val)));
	}
	prog    = val->foamClos.prog;
	ccClos	= ccoNewNode(CCO_Many, 2);

	ccLeft = gc0MultVarId("tmpClos", int0, decl->foamDecl.id);
	ccRight = ccoInit(ccoMany2(ccoIntOf(int0),
		   ccoCast(ccoIdOf("FiProg"),
			   ccoPreAnd(gc0MultVarId("tmp", 
						  prog->foamConst.index,
						  gc0GetDecl(prog)->foamDecl.id)))));

	ccoArgv(ccClos)[0] = ccoDecl(ccoType(ccoStatic(),
					     ccoStructRef(ccoIdOf("_FiClos"))),
				     ccoAsst(ccLeft, ccRight));

	ccoArgv(ccClos)[1] = ccoDecl(ccoTypeIdOf(gcFiClos),
				     ccoAsst(gccId(ref),
					     ccoPreAnd(gc0MultVarId("tmpClos", int0, 
								    decl->foamDecl.id))));
	return ccClos;
}

local CCode
gc0Set(Foam foamLHS, Foam foamRHS)
{
	AInt  typeCastExpr = -1;

	FoamBValTag tag = 0;

	gcvisStmtFCall = 0;
	if (foamTag(foamRHS) == FOAM_BVal)
		tag = foamRHS->foamBVal.builtinTag;
	if (foamTag(foamRHS) == FOAM_BCall)
		tag = foamBValInfo(foamRHS->foamBCall.op).tag;
	if (foamTag(foamRHS) == FOAM_Cast) {
		Foam	fc = foamRHS->foamCast.expr; /* what's being cast */

		typeCastExpr = gc0ExprType(fc); /* deduce its FOAM type */

		if (foamTag(fc) == FOAM_BVal) {
			tag = fc->foamBVal.builtinTag;
			if (ccBValMacro(tag) != 0)
				foamRHS = fc;
		}
		if (foamTag(fc) == FOAM_BCall) {
			tag = foamBValInfo(fc->foamBCall.op).tag;
/* 		        fprintf(stderr,"%s\tcast to  %s\n",foamInfoTable[typeCastExpr].str,foamInfoTable[gc0ExprType(foamLHS)].str); */
			if ((ccBValMacro(tag) != 0) && (typeCastExpr != FOAM_SFlo))
				foamRHS = fc;
		}
		/* A modifiable argument variable on the lhs can cause a cast. */
		if (foamTag(fc) == FOAM_PCall &&
		    fc->foamPCall.protocol == FOAM_Proto_Fortran)
			return gc0FortranSet(foamLHS, fc, (FoamTag)foamRHS->foamCast.type, (FoamTag)typeCastExpr);
	}
	if (tag && ccBValMacro(tag) != 0)
		gcvisStmtFCall = 1;

	if (foamTag(foamRHS) == FOAM_Catch) {
		/* The old way: (Set (Values ...) (Catch ...)) */
		bug("gc0Set: old-style set-catch");
	  	return gc0SetCatch(foamLHS, foamRHS);
	}
	else if (foamTag(foamRHS) == FOAM_MFmt &&
		foamTag(foamRHS->foamMFmt.value) == FOAM_Catch) {
		/* The new way: (Set (Values ...) (MFmt f (Catch ...))) */
	  	return gc0SetCatch(foamLHS, foamRHS->foamMFmt.value);
	}
	else if (foamTag(foamLHS) == FOAM_Fluid) {
		return gc0FluidSet(foamLHS, foamRHS);
	} 
	else if (foamTag(foamLHS) == FOAM_Values)
		return gc0SetValues(foamLHS, foamRHS);
	else if (foamTag(foamRHS) == FOAM_PCall &&
		 foamRHS->foamPCall.protocol == FOAM_Proto_Fortran)
		return gc0FortranSet(foamLHS, foamRHS, FOAM_NOp, FOAM_NOp);
	else {
		CCode	ccArg, ccType, ccSpecial;
		AInt    typeLhs = gc0ExprType(foamLHS);

		ccArg = gccRef(foamLHS);
		if (ccoTag(ccArg) == CCO_Cast) 
			ccArg = ccoArgv(ccArg)[1];
		ccType = gc0TypeId(typeLhs, emptyFormatSlot);
		if (gc0IsDecl(foamLHS)) {
			Foam	decl = gc0GetDecl(foamLHS);
			int	fmt = decl->foamDecl.format;
			int	typ = decl->foamDecl.type;

			if (typ == FOAM_Rec) {
				if (fmt && fmt != emptyFormatSlot)
					ccType = ccoTypedefId(gc0VarId(gcFmtType, 
								       fmt));
			}
			else if (typ == FOAM_Arr)
				ccType = ccoPostStar(gc0TypeId(fmt, emptyFormatSlot));
		}
		else if (foamTag(foamLHS) == FOAM_AElt)
			ccType = gc0TypeId(foamLHS->foamAElt.baseType, emptyFormatSlot);
#ifdef USE_MACROS
		if (tag && gcvisStmtFCall) {
			CCode	ccArgs,ccSpecial;
			int	argc, i, ix;

			ccSpecial = gc0SpecialSFloWord(foamRHS,typeCastExpr,ccArg);
			if (ccSpecial) return ccSpecial;

			argc = foamBValInfo(tag).argCount;
			ccArgs = ccoNewNode(CCO_Many, argc+2);
			ccoArgv(ccArgs)[0] = ccArg;
			ccoArgv(ccArgs)[1] = ccType;
			for (i = 2, ix = 0; ix < argc; i++, ix++)
				ccoArgv(ccArgs)[i] = gccExpr(foamRHS->foamBCall.argv[ix]);
			return ccoFCall(ccoIdOf(ccBValMacro(tag)), ccArgs);
		}
#endif
		if (foamTag(foamRHS) == FOAM_RNew)
			return ccoAsst(ccArg, gccExpr(foamRHS));
		else if (foamTag(foamRHS) == FOAM_ANew) {
			CCode	ccArrIndex, ccFunName;

			ccArrIndex = gccExpr(foamRHS->foamANew.size);
			ccFunName  = ccoIdOf(strConcat("fiARRNEW_",
				       foamStr(foamRHS->foamANew.eltType)));
			return ccoFCall(ccFunName, ccoMany3(ccArg,
							    ccType,
							    ccArrIndex));
		}
		else if ((ccSpecial = gc0SpecialSFloWord(foamRHS,typeCastExpr,ccArg)) != NULL )  return ccSpecial;
			 
		else
			return ccoAsst(ccArg, gc0SubExpr(foamRHS, ccType));
	}
}

local CCode
gc0SpecialSFloWord(Foam foamRHS, AInt typeCastExpr, CCode ccArg)
{
	if (foamTag(foamRHS) == FOAM_Cast &&
		foamRHS->foamCast.type == FOAM_SFlo &&
		typeCastExpr == FOAM_Word) {

		CCode cc = ccoMany2(ccArg,
			gc0SubExpr(foamRHS->foamCast.expr, NULL));

		return ccoFCall(ccoIdOf("fiSFLO_FR_WORD"), cc);
	}
 	else if (foamTag(foamRHS) == FOAM_Cast &&
		foamRHS->foamCast.type == FOAM_Word &&
		typeCastExpr == FOAM_SFlo) {

		CCode cc = ccoMany2(ccArg,
		    gc0SubExpr(foamRHS->foamCast.expr, NULL));

		return ccoFCall(ccoIdOf("fiWORD_FR_SFLO"), cc);
	}
	else return NULL;
}

local CCode
gc0SetValues(Foam foamLHS, Foam foamRHS)
{
	int		i, num, args;
	CCode		ccRhs, ccArgs, cc;
	CCodeList	code = listNil(CCode);

	/* This is legal (if nasty) so deal with it */
	if (foamTag(foamRHS) == FOAM_MFmt &&
	    foamTag(foamRHS->foamMFmt.value) == FOAM_Values)
		foamRHS = foamRHS->foamMFmt.value;

	if (foamTag(foamRHS) == FOAM_Values) {
		/* Assume lhs + rhs are independent */
		for (i=0; i<foamArgc(foamLHS); i++) {
			cc = gc0Set(foamLHS->foamValues.argv[i],
				    foamRHS->foamValues.argv[i]);
			gc0AddTopLevelStmt(gcvStmts, ccoStat(cc));
		}
		return NULL;
	}

	if (foamTag(foamRHS) == FOAM_MFmt && 
	    foamTag(foamRHS->foamMFmt.value) == FOAM_PCall &&
	    foamRHS->foamMFmt.value->foamPCall.protocol == FOAM_Proto_Fortran)
		bug("gc0SetValues: Fortran call returning MFmt (not supported)");
		/* return gc0FortranSet(foamLHS, foamRHS);  */

	ccRhs  = gccExpr(foamRHS);
	args   = ccoArgc(ccoArgv(ccRhs)[1]);
	ccArgs = ccoArgv(ccRhs)[1];
	for (i = 0; i < args; i++)
		gc0AddLine(code, ccoArgv(ccArgs)[i]);
	num = foamArgc(foamLHS);
	for (i = 0; i < num; i++) {
		if (foamTag(foamLHS->foamValues.argv[i]) == FOAM_Fluid)
			bug("Fluid multi: Not supported.");
		gc0AddLine(code, ccoPreAnd(gccRef(foamLHS->foamValues.argv[i])));
	}
	code   = listNReverse(CCode)(code);
	ccoArgv(ccRhs)[1] = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	return ccRhs;
}

local CCode
gc0FortranSet(Foam foamLHS, Foam foamRHS, FoamTag lhsType, FoamTag rhsType)
{
	Foam gdecl, argformat, pcall, lastresultvar, *resultvars;
	int i, numresultvars, listl, modargs;
	String cmplxfns = compCfgLookupString("fortran-cmplx-fns");
	FortranType restype;	
	CCode ccType, ccCastType, ccFortranSet;
	FoamTag resvartype, fmtype;
	AInt resvarfmt;
	CCodeList l, closnullifys = listNil(CCode);
	Foam fnresultdecl;

	ccFortranSet = NULL;
	
	if (foamTag(foamLHS) == FOAM_Values) {
                bug("Fortran function call with multiple return values");
		numresultvars = foamArgc(foamLHS);
		pcall = foamRHS->foamMFmt.value;
		resultvars = foamLHS->foamValues.argv;
		lastresultvar = foamLHS->foamValues.argv[foamArgc(foamLHS)-1]; 
	} 
	else {
		numresultvars = 1;
		pcall = foamRHS;
		resultvars = &foamLHS;
		lastresultvar = foamLHS;
	}
	gdecl = gc0GetDecl(pcall->foamPCall.op);
        argformat = gcvFmt->foamDFmt.argv[gdecl->foamGDecl.format];
	fnresultdecl = gc0GetFortranRetFm(argformat);
	modargs = gc0GetNumModFortranArgs(argformat);
	restype = gc0GetFortranRetType(argformat);

	if (foamTag(foamLHS) == FOAM_Values && numresultvars < modargs)
		bug("Fortran Set Values has the wrong number of lhs variables");


	/* Char and Character are equivalent */
	if (restype && (restype != FTN_Machine))
		fmtype = gen0FtnMachineType(restype);
	else
		fmtype = fnresultdecl->foamDecl.type;


	/* Hack */
	if (fmtype == FOAM_Char)
		restype = FTN_Character;


	/* Strings, characters and complex numbers are special ... */
	switch (restype)
	{
	  case FTN_Character:
		/* Fall through */
	  case FTN_String:
		/* Fall through */
	  case FTN_XLString:
		ccFortranSet = gccFortranPCall(resultvars, numresultvars, pcall, &closnullifys);
		break;
	  case FTN_FSComplex:
		/* Fall through */
	  case FTN_FDComplex:
		resvartype = (gc0GetDecl(lastresultvar))->foamDecl.type;
		resvarfmt = (gc0GetDecl(lastresultvar))->foamDecl.format;
		ccType = ccoTypeIdOf(restype == FTN_FSComplex ? gcFiComplexSF : gcFiComplexDF); 
		ccCastType = gc0TypeId(resvartype, resvarfmt);

		gc0AddTopLevelStmt(gcvStmts, 
				   ccoStat(ccoFCall(ccoIdOf("fi_ALLOC"), 
						    ccoMany3(gccRef(lastresultvar),
							     ccCastType,
							     ccoSizeof(ccType)))));
		if (!cmplxfns)
			comsgFatal(NULL, ALDOR_F_NoFCmplxProperty, "fortran-cmplx-fns");
		else if (strEqual(cmplxfns, "return-struct"))
			ccFortranSet = ccoAsst(ccoPreStar(ccoParen(ccoCast(ccoPostStar(ccoCopy(ccType)), 
									   gccRef(lastresultvar)))),
					       gccFortranPCall(resultvars, numresultvars, pcall, &closnullifys));    
		else if (strEqual(cmplxfns, "return-void"))
			ccFortranSet = gccFortranPCall(resultvars, numresultvars, pcall, &closnullifys);
		else if (strEqual(cmplxfns, "disallowed"))
			bug("gc0FortranSet: Fortran function returning a complex result detected");
		else 
			comsgFatal(NULL, ALDOR_F_BadFCmplxValue, cmplxfns);
		break;
	  default:
		{
			CCode cc = gccFortranPCall(resultvars, numresultvars, pcall, &closnullifys);
			CCode ccR = gccRef(lastresultvar);

			if ((lhsType == FOAM_Word) && (rhsType == FOAM_SFlo))
			{
				cc = ccoMany2(ccR, cc);
				ccFortranSet = ccoFCall(ccoIdOf("fiWORD_FR_SFLO"), cc);
			}
			else
				ccFortranSet = ccoAsst(ccR, cc);
		}
	}

	if (closnullifys) {
		gc0AddTopLevelStmt(gcvStmts, ccoStat(ccFortranSet));
		listl = listLength(CCode)(closnullifys);
		l = closnullifys;
		for (i = 0; i < listl-1; closnullifys = cdr(closnullifys), i++) 
			gc0AddTopLevelStmt(gcvStmts, ccoStat(car(closnullifys)));
		ccFortranSet = car(closnullifys);
		listFree(CCode)(l);
	}
	return ccFortranSet;
}

local CCode
gc0SetCatch(Foam lhs, Foam rhs)
{
	Foam	flg, val, exn;
	Foam	call = rhs->foamCatch.ref;
	CCode	args, res, fcall;

	assert(foamTag(call) == FOAM_Clos);
	args  = ccoMany1(gccExpr(call->foamClos.env));
	fcall = ccoFCall(gccProgId(call->foamClos.prog), args);


	/* Does this catch-block return a value? */
	if (foamArgc(lhs) == 3) {
  		flg = lhs->foamValues.argv[0];
	  	val = lhs->foamValues.argv[1];
  		exn = lhs->foamValues.argv[2];

		res = ccoFCall(ccoIdOf("fiBlock"),
			ccoMany4(gccExpr(flg), gccExpr(val), gccExpr(exn),
			fcall));
	}
	else {
  		flg = lhs->foamValues.argv[0];
  		exn = lhs->foamValues.argv[1];

		res = ccoFCall(ccoIdOf("fiVoidBlock"),
			ccoMany3(gccExpr(flg), gccExpr(exn), fcall));
	}

	return res;
}


local CCode
gc0SeqStmt(Foam foam, int n)
{
	Foam	seqFoam;

	seqFoam = foamArgv(foam)[n].code;

	if (foamTag(seqFoam) == FOAM_Seq) {
		gcvNStmts     += (foamArgc(seqFoam) - 1);
		gcvStmts->stmt = (CCode *) stoResize(gcvStmts->stmt,
					     fullsizeof(struct ccoNode, 1,
						CCode) * (gcvNStmts));
		gcvStmts->argc = gcvNStmts;
	}

	return gccCmd(seqFoam);
}

local CCode
gc0FunBCall(Foam foam, int returnKind)
{
	return gc0Builtin(foamBValInfo(foam->foamBCall.op).tag, foam);
}

local CCode
gc0Builtin(FoamBValTag tag, Foam foam)
{
	CCode	cc;
	int	ccTag;

	ccTag = ccBValCFun(tag);
	switch (ccTag) {
	  case CCO_Id:
	  case CCO_FloatVal:
	  case CCO_IntVal:
	  case CCO_CharVal:
		cc = ccoIdOf(ccBValStr(tag));
		break;
	  case CCO_FCall:
		cc = gc0FCall(tag, foam);
		break;
	  case CCO_Cast:
		cc = ccoCast(ccoTypeIdOf(ccBValStr(tag)),
			     gccExpr(foam->foamBCall.argv[0]));
		break;
	  default:
		cc = gc0Cop(tag, foam, ccTag);
		break;
	}

	return cc;
}

local CCode
gc0FCall(FoamBValTag tag, Foam foam)
{
	CCode	ccArgs;

	if (ccBValSpec(tag) == 0) {
		int	i, argc;
		argc   = foamBValInfo(tag).argCount;
#ifdef USE_MACROS
		if (ccBValMacro(tag) && !gcvisStmtFCall) {
			CCode		cc, ccName;
			int		ix;

			/* Create a temporary assignment variable. */
			ccName = gc0MultVarId("T", gcvNLocs, "");
			ccArgs = ccoNewNode(CCO_Many, argc+2);
			ccoArgv(ccArgs)[0] = ccoCopy(ccName);
			ccoArgv(ccArgs)[1] = gc0TypeId(gc0ExprType(foam), emptyFormatSlot);

			/* Update local variable list. */
			gc0AddUnSortedLocal(ccoDecl(gc0TypeId(gc0ExprType(foam), emptyFormatSlot), 
						    ccName));

        		for (i = 2, ix = 0; ix < argc; i++, ix++)
				ccoArgv(ccArgs)[i] = gccExpr(foam->foamBCall.argv[ix]);
			cc = ccoFCall(ccoIdOf(ccBValMacro(tag)), ccArgs);
			gc0AddTopLevelStmt(gcvStmts, ccoStat(cc));

			return ccName;
		}
#else
		if (false) ;
#endif
		else {
			ccArgs = ccoNewNode(CCO_Many, argc);
			for (i = 0; i < argc; i++) {
				FoamTag	type;
				Foam	val;

				type = foamBValInfoTable[tag].argTypes[i];
				val = foam->foamBCall.argv[i];

				ccoArgv(ccArgs)[i] = gc0TryCast(type, val);
			}
		}
	}
	else {
		switch(tag) {
		  case FOAM_BVal_BIntIsEven:
		  case FOAM_BVal_BIntIsOdd:
			ccArgs = ccoMany2(
				ccoFCall(ccoIdOf("fiBIntMod"),
				 ccoMany2(gccExpr(foam->foamBCall.argv[0]),
					  ccoFCall(ccoIdOf("fiBIntNew"),
						   ccoIntOf(2)))),
				ccoFCall(ccoIdOf("fiBInt0"), int0));
			break;
		  case FOAM_BVal_BIntPrev:
		  case FOAM_BVal_BIntNext:
			ccArgs = ccoMany2(gccExpr(foam->foamBCall.argv[0]),
					  ccoFCall(ccoIdOf("fiBInt1"), int0));
			break;
		  default:
			bugBadCase(tag);
			NotReached(return 0);
		}
	}
	return ccoFCall(ccoIdOf(ccBValStr(tag)), ccArgs);
}

local CCode
gc0Cop(FoamBValTag tag, Foam foam, CCodeTag ctag)
{
	CCode	ccArgs;
	int	argc, i;

	if (ccBValSpec(tag) == 0) {
		argc   = foamBValInfo(tag).argCount;
		ccArgs = ccoNewNode(ctag, argc);
		for (i = 0; i < argc; i++)
			ccoArgv(ccArgs)[i] = gccExpr(foam->foamBCall.argv[i]);
	}
	else {
		if (ccBValSpec(tag) == 1) /* !!! cast words to ptrs */
			ccArgs = ccoNew(ctag, 2,
					gccExpr(foam->foamBCall.argv[0]),
					ccoIdOf(ccBValStr(tag)));
		else {
			if ((tag == FOAM_BVal_SIntIsEven) ||
			    (tag == FOAM_BVal_SIntIsOdd))
				ccArgs = ccoNew(ctag, 2,
					ccoMod(gccExpr(foam->foamBCall.argv[0]),
					       ccoIntOf(2)), ccoIntOf(int0));
			else
				if ((tag == FOAM_BVal_SIntPlusMod) ||
				 (tag == FOAM_BVal_SIntMinusMod) ||
				 (tag == FOAM_BVal_SIntTimesMod))
					ccArgs = gc0SIntMod(foam, ctag);
				else
					ccArgs = gccUnhandled(foam);
		}
	}

	return ccArgs;
}

local CCode
gc0FunFoamCall(Foam foam, int returnKind)
{
	FoamTag		type;
	AInt		oformat = emptyFormatSlot;
	int		idx;
	CCode		call;
	GcNesting	oldNest = gcvCallNesting;
	CCodeList	prevUsed = gcvNestUsed;
	CCode		retval;

	switch (foamTag(foam)) {
	  case FOAM_CCall:
		type = foam->foamCCall.type;
		gcvCallNesting = GC_CCall;
		call = gc0FunCCall0(foam, returnKind);
		break;
	  case FOAM_OCall:
		type = foam->foamOCall.type;
		idx = foam->foamOCall.op->foamConst.index;
		/* !!!FIXME (use proper macros ... ) */
		oformat =  gcvDefs->foamDDef.argv[idx]->foamDef.rhs->foamProg.format;
		if (gcvCallNesting < GC_OCall)
			gcvCallNesting = GC_OCall;
		call = gc0FunOCall0(foam, returnKind);
		break;
	  case FOAM_PCall:
		type = foam->foamPCall.type;
		if (gcvCallNesting < GC_OCall)
			gcvCallNesting = GC_OCall;
		call = gc0FunPCall0(foam, returnKind);
		break;
	  default:
		call = NULL;
		type = FOAM_Word;
		bug("Unlikely call for gc0FunFoamCall");
		break;
	}
	gcvCallNesting = oldNest;

	if (gcvCallNesting == GC_CCall) {
		while (gcvNestUsed != prevUsed) {
			gcvNestFree = listCons(CCode)(car(gcvNestUsed), 
						      gcvNestFree);
			gcvNestUsed = cdr(gcvNestUsed);
		}
		retval = gc0UnNestCall(gc0TypeId(type, oformat), call);
		return retval;
	}
	else
	    return call;
}

local CCode
gc0FunCCall0(Foam foam, int returnFmt)
{
	CCodeList	code = listNil(CCode);
	CCode		ccArgs, ccCall, ccId;
	FoamTag	       *argTypes;
	Foam		cfoam;
	int		i, argc;

	assert(foamTag(foam) == FOAM_CCall);

	argc = foamCCallArgc(foam);
	gc0AddLine(code, gc0TypeId(foam->foamCCall.type, emptyFormatSlot));
	
	ccId = gccId(foam->foamCCall.op);

	if (gc0ExprType(foam->foamCCall.op) != FOAM_Clos)
		gc0AddLine(code, ccoCast(ccoTypeIdOf(gcFiClos),
				   ccId));
	else
		gc0AddLine(code, ccId);

	argTypes = (FoamTag *) stoAlloc(OB_Other, argc * sizeof(FoamTag));
	for (i = 0; i < argc; i++) {
		cfoam = foam->foamCCall.argv[i];
		argTypes[i] = gc0ExprType(cfoam);
		gc0AddLine(code, gccExpr(cfoam));
	}
	code = listNReverse(CCode)(code);
	ccArgs = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	
	ccCall = gc0FiCFun(foam->foamCCall.type, argc, argTypes, ccArgs, returnFmt);

	stoFree(argTypes);

	return ccCall;
}

local CCode
gc0FunOCall0(Foam foam, int returnKind)
{
	CCodeList	code = listNil(CCode);
	CCode		ccArgs;
	Foam		decl, pdecls;
	int		i, idx, argc;

	/* Call to function in the same unit */
	assert(foamTag(foam) == FOAM_OCall);
	assert(foamTag(foam->foamOCall.op) == FOAM_Const);

	argc = foamOCallArgc(foam);
	/* Which one? !!! FIXME use proper macros for gcvDefs ... */
	idx = foam->foamOCall.op->foamConst.index;
	assert(foamTag(gcvDefs->foamDDef.argv[idx]->foamDef.rhs) == FOAM_Prog);


	/* Extract the parameter declarations for this call !!! FIXME gcvDefs */
	pdecls = gcvDefs->foamDDef.argv[idx]->foamDef.rhs->foamProg.params;

	/* Generation code for each declaration */
	decl = foam->foamOCall.env;
	gc0AddLine(code, gccRef(decl));
	for (i = 0; i < argc; i++) {
		/* These are what the function expects */
		FoamTag	expect = pdecls->foamDDecl.argv[i]->foamDecl.type;
		Foam	farg  = foam->foamOCall.argv[i];

		gc0AddLine(code, gc0TryCast(expect, farg));
	}

	code = listNReverse(CCode)(code);
	ccArgs = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	return ccoFCall(gccProgId(foam->foamOCall.op), ccArgs);
}

local CCode
gc0FunPCall0(Foam foam, int returnKind)
{
	CCodeList	l, code = listNil(CCode);
	CCode		ccArgs, ccPCall;
	int		i, listl, argc;

	assert(foamTag(foam) == FOAM_PCall);
	argc = foamPCallArgc(foam);

	if (foam->foamPCall.protocol == FOAM_Proto_Fortran) {
		ccPCall = gccFortranPCall(NULL,(int) 0, foam, &code);
		if (code) {
			/* Calls to Fortran routines that have fn parameters
			   are always top level stmts */
			gc0AddTopLevelStmt(gcvStmts, ccoStat(ccPCall));
			listl = listLength(CCode)(code);
			l = code;
			for (i = 0; i < listl-1; code = cdr(code), i++) 
				gc0AddTopLevelStmt(gcvStmts, ccoStat(car(code)));
			ccPCall = car(code);
			listFree(CCode)(l);
		}
		return ccPCall;
	}
	else {
		for (i = 0; i < argc; i++) {
			Foam farg = foam->foamPCall.argv[i];
			gc0AddLine(code, gccExpr(farg)); 
		}
		code = listNReverse(CCode)(code);
	} 
	ccArgs = gc0ListOf(CCO_Many, code);
	listFree(CCode)(code);
	return ccoFCall(gccPCallId(foam), ccArgs);
}


/* This is a bit of a hack, since it assumes that the declaration
 * 	foo(FiWord, FiWord, etc);
 * generates the same calling convention as
 *      foo();
 * Still, keeps code length down...
 */
local Bool
gc0TypeRequiresDecl(FoamTag tag, AInt fmt)
{
	switch (tag) {
	  case FOAM_SInt:
	  case FOAM_Word:
	  case FOAM_Ptr:
	  case FOAM_Arr:
	  case FOAM_Bool:
	  case FOAM_NOp:
	  case FOAM_Clos:
	  case FOAM_BInt:
	  case FOAM_Env:
		return false;
	  case FOAM_Rec:
	  case FOAM_RRec:
		return false;

	  case FOAM_Byte:
	  case FOAM_SFlo:
	  case FOAM_DFlo:
	  case FOAM_HInt:
	  case FOAM_Char:
	  case FOAM_Arb:
		return true;
	  default:
		printf("%s\n", foamStr(tag));
		return true;
	}
}

/* This function makes a function call from ccArgs, where 
 * ccArgs[0] = closure
 * ccArgs[1..n] = arguments.
 *
 * The arg types are given in typev, and the return format 
 * is in retFmt.  This should be reworked to take an 
 * input and output ddecl, I guess.
 * 
 * The return value references are dumped into the function
 * by gen0Set (or thereabouts), so this fn has to create
 * something of the form fn(args).
 *
 * The major complexity is that ANSI requires that if a function
 * is declared fully --- ie. not `int foo()', then any calls to foo
 * must ensure that the type of foo is known.  Hence the creative use
 * of casting below.
 *
 * NB: ccArgc(ccArgs) may be more than argc --- if so, these values
 *     are copied.
 */
local CCode
gc0FiCFun(FoamTag ret, int argc, FoamTag *typev, CCode ccArgs, int retFmt)
{
	int   i;
	CCode ccNary;
	CCode ccFunction;
	Foam ddecl = gcvFmt->foamDFmt.argv[retFmt];
	int  nRets;
	Bool useProto = false;
	CCode retval;

	nRets = foamDDeclArgc(ddecl);
	
	/* First decide how lazy to be */
	if (retFmt != emptyFormatSlot) useProto = true;
	if (gc0TypeRequiresDecl(ret, retFmt))  useProto = true;
		
	for (i = 0 ; typev && i < argc; i++) {
		if (gc0TypeRequiresDecl(typev[i], emptyFormatSlot)) useProto = true;
	}

	for (i = 0 ; i < nRets; i++) {
		if (gc0TypeRequiresDecl(ddecl->foamDDecl.argv[i]->foamDecl.type,
					ddecl->foamDDecl.argv[i]->foamDecl.format))
			useProto = true;
	}

	/* Now be lazy */
	if (!useProto) {
		if (argc <= gcFiNMaxCCall) {
			ccFunction = gcFiCCallN(argc, ccArgs);
			return ccFunction;
		}
		else {
			ccFunction = ccoFCall(ccoIdOf("fiCFun"),
					      ccoMany2(ccoArgv(ccArgs)[0],
						       ccoArgv(ccArgs)[1]));
		}
	}
	else {
		ccNary = ccoNewNode(CCO_Many, argc+ 1 + nRets);
		ccoArgv(ccNary)[0] = ccoParam(NULL, ccoTypeIdOf(gcFiEnv), NULL);

		for (i = 0; i < argc; i++) {
			CCode ctype;
			/*
			 * Lie like crazy because we've lost format
			 * information and can't recover it. We get lucky
			 * because if we need fmt info, then we have a
			 * pointer type.
			 */
			if (gc0TypeRequiresDecl(typev[i], emptyFormatSlot))
				ctype = gc0TypeId(typev[i], emptyFormatSlot);
			else {
				ctype = ccoTypeIdOf(gcFiWord);
				ccoArgv(ccArgs)[i+2] = ccoCast(ccoTypeIdOf(gcFiWord),
							       ccoArgv(ccArgs)[i+2]);
			}
			ccoArgv(ccNary)[i + 1] = ccoParam(NULL,ctype,NULL);
		}
		for (i = 0; i < nRets ; i++) {
			Foam decl = ddecl->foamDDecl.argv[i];
			
			ccoArgv(ccNary)[argc + i + 1] = 
				ccoParam(NULL, 
					 ccoPostStar(gc0TypeId(decl->foamDecl.type,
							       decl->foamDecl.format)),
					 NULL);
		}
		ccFunction = ccoCast(ccoType(ccoArgv(ccArgs)[0], 
					     ccoFCall(ccoPreStar(NULL), ccNary)),
				     ccoFCall(ccoIdOf("fiRawCProg"), ccoArgv(ccArgs)[1]));
	}

	i = 0;
	ccNary = ccoNewNode(CCO_Many, ccoArgc(ccArgs) - 2 + 1);
	/* Env argument */
	ccoArgv(ccNary)[i++] = ccoPointsTo(ccoParen(ccoCopy(ccoArgv(ccArgs)[1])),
					   ccoIdOf("env"));
	/* All the others... */
	for ( ; i < ccoArgc(ccNary); i++)
		ccoArgv(ccNary)[i] = ccoCopy(ccoArgv(ccArgs)[i+1]);

	retval = ccoFCall(ccFunction, ccNary);
	return retval;
}

local CCode
gc0UnNestCall(CCode type, CCode call)
{
	CCode cc, ccName;

	ccName = gc0GetTmp(type);
	cc = ccoAsst(ccName, call);
	gc0AddTopLevelStmt(gcvStmts, ccoStat(cc));
	return ccoCopy(ccName);
}

local CCode
gc0GetTmp(CCode type)
{
	CCodeList vars, prev = NULL;
	CCode ccVar = NULL, ccName;

	for (vars = gcvNestFree; vars ; vars = cdr(vars)) {
		assert(ccoTag(car(vars)) == CCO_Decl);
		if (ccoTypeEqual(type, ccoArgv(car(vars))[0])) {
			ccVar = ccoCopy(car(vars)); 
			if (!prev) gcvNestFree = cdr(gcvNestFree);
			else
				setcdr(prev, cdr(vars));
			break;
		}
		prev = vars;
	}
	if (!ccVar) {
		ccVar = ccoDecl(type, gc0MultVarId("T", gcvNLocs, ""));
		gc0AddUnSortedLocal(ccVar);
	}

	ccName = ccoArgv(ccVar)[1];
	gcvNestUsed = listCons(CCode)(ccVar, gcvNestUsed);
	return ccoCopy(ccName);
}

local CCode 
gc0Protect(Foam foam)
{
	CCode res;

	res = ccoFCall(ccoIdOf("fiProtect"), 
		       ccoMany3(gccRef(foam->foamProtect.val),
				gccExpr(foam->foamProtect.expr),
				gccExpr(foam->foamProtect.after)));
	
	return res;
}


local CCode 
gc0Throw(Foam foam)
{
	return ccoFCall(ccoIdOf("fiUnwind"),
			ccoMany2(gccExpr(foam->foamThrow.tag),
				 gccExpr(foam->foamThrow.val)));
}


local CCode
gc0EEnv(CCode env, int level)
{     
	CCode	cc = env;

	while (level-- > 0)
		cc = gcFiEnvNext(cc);
	return cc;
}

/* "cc" == NULL -> No cast wanted. */
local CCode
gc0SubExpr(Foam foam, CCode cc)
{
	CCode	ccNew, ccExpr;

	if (foamTag(foam) == FOAM_Arr || foamTag(foam) == FOAM_ANew)
		ccNew = ccoTypeIdOf(gcFiPtr);
	else
		ccNew = gc0TypeId(gc0ExprType(foam), emptyFormatSlot);
	if (gc0IsDecl(foam)) {
		Foam	decl = gc0GetDecl(foam);
		int	fmt = decl->foamDecl.format;
		int	typ = decl->foamDecl.type;
		if (foamTag(foam) == FOAM_Glo &&
		    decl->foamGDecl.rtype == FOAM_Nil)
			ccNew = ccoTypeIdOf(gcFiPtr);
		if (typ == FOAM_Rec) {
			if (fmt && fmt != emptyFormatSlot)
				ccNew = ccoTypedefId(gc0VarId(gcFmtType, fmt));
		}
		else if (typ == FOAM_Arr && fmt)
			ccNew = ccoTypeIdOf(gcFiPtr);
	}
	ccExpr = gccCmd(foam);

	if (!cc)
		return ccExpr;
	if (!ccoTypeEqual(cc, ccNew) && (ccoTag(ccExpr) != CCO_Cast))
		return ccoCast(cc, ccExpr);
	else if (!ccoTypeEqual(cc, ccNew) && (ccoTag(ccExpr) == CCO_Cast)) {
		ccoArgv(ccExpr)[0] = cc;
		return ccExpr;
	}
	else
		return ccExpr;
}

local CCode
gc0EEltNext(Foam env, int count)
{
	CCode	ccRef;
	
	ccRef = gccExpr(env);
	while (count > gcFiNMaxEnvLevel) {
		ccRef = gcFiEnvNext(ccRef);
		count--;
	}
	return gcFiEnvLevelN(count, ccRef);
}

/*
 * Create the symbol for an identifier.
 */

local CCode
gc0IdDecl(Foam foam, FoamTag tag, Foam decl, int lv, int idx)
{
	CCode	ccDecl, ccName;
	int	t   = decl->foamDecl.type;
	String	str = decl->foamDecl.id;

	assert(strLength(str) < 180);

	switch (tag) {
	  case FOAM_Const:
		if (gc0OverSMax() && idx) {
			Foam	fn = gcvConst->foamDDecl.argv[0];
			String	s  = strPrintf("%s_%s", fn->foamDecl.id, str);
			ccName = gc0MultVarId("C", idx, s);
			strFree(s);
		}
		else {
			ccName = gc0MultVarId("C", idx, str);
		}
		ccName = gc0MultVarId("C", idx, str);
		ccDecl = ccoDecl(gc0TypeId(t, emptyFormatSlot), ccName);
		break;
	  case FOAM_Par:
		ccName = gc0MultVarId("P", idx, str);
		ccDecl = gc0Decl(decl, ccName);
		break;
	  case FOAM_Loc:
		ccName = gc0MultVarId("T", idx, str);
		ccDecl = gc0Decl(decl, ccName);
		break;
	  case FOAM_Lex:
	  case FOAM_EElt:
		bug("attempt to declare non-local");
	  default:
		bugBadCase(tag);
		NotReached(return 0);
	}
	return ccDecl;

}

local CCode
gc0GloIdDecl(Foam decl, int idx)
{
	CCode	ccDecl, ccName, ccType;
	int	t, p, fmt, dir;
	String	str, s0;
	Bool	imported, initialiser;
	CCode	ccStorageClass, ccExternalName;

	t   = decl->foamGDecl.type;
	str = s0 = decl->foamGDecl.id;
	p   = decl->foamGDecl.protocol;
	fmt = decl->foamGDecl.format;
	dir = decl->foamGDecl.dir;

	if (p == FOAM_Proto_C && strchr(str, FOREIGN_INCLUDE_SEPARATOR))
		return 0;

	imported = (dir == FOAM_GDecl_Import);
	initialiser = (p == FOAM_Proto_Init);
	ccExternalName = NULL;

	if (imported && idx != -1 && 
		 (p == FOAM_Proto_Foam || p == FOAM_Proto_Init)) {
			ccName = gc0MultVarId("pG", idx, str);
			ccExternalName = gc0MultVarId("G", idx, str);
	}
	else if (p == FOAM_Proto_Foam || p == FOAM_Proto_Init)
		ccName = gc0MultVarId("G", idx, str);
	else
		ccName = ccoIdOf(str);
	
	if (p == FOAM_Proto_Foam || p == FOAM_Proto_Init) 
		ccType = gc0TypeId(t, fmt);
	else if (p == FOAM_Proto_Fortran)
		gc0IdFortranDecl(decl, &ccName, &ccType);
	else if (t != FOAM_Prog && t != FOAM_Clos)
		ccType = gc0TypeId(t, fmt);
	else if (imported && (p == FOAM_Proto_C)) {
		ccName = gc0IdCDecl(decl, ccName);
		ccType = gc0TypeId(decl->foamGDecl.rtype, emptyFormatSlot);
	}
	else {
		/*
		 * We need to be able to do better than this, especially
		 * when exporting functions involving machine types such
		 * as SInt/HInt. Perhaps when we create this global we
		 * ought to store the signature somewhere safe so that it
		 * can be used here?
		 */
		ccType = gc0TypeId(decl->foamGDecl.rtype, emptyFormatSlot);
		ccName = ccoFCall(ccName, int0);
	}

	if (idx != -1 && (p == FOAM_Proto_Foam || p == FOAM_Proto_Init)) {
		if (imported) {
			gc0AddLine(gcvImportedGloInitCC,
				   ccoStat(ccoFCall(ccoIdOf("fiImportGlobal"),
						    ccoMany2(ccoStringVal(ccExternalName->ccoToken.symbol),
							     ccoCopy(ccName)))));
			ccName = ccoPreStar(ccName);
		}
		else
			gc0AddLine(gcvExportedGloInitCC,
				   ccoStat(ccoFCall(ccoIdOf("fiExportGlobal"),
						    ccoMany2(ccoStringVal(ccName->ccoToken.symbol),
							     ccoCopy(ccName)))));
	}

	if (initialiser && imported) {
		gc0AddLine(gcvInitFunCalls0CC,
			   ccoStat(ccoFCall(ccoIdOf("fiFileInitializer"),
					    ccoStringOf(str))));
		
		gc0AddLine(gcvInitFunCalls1CC,
			   ccoStat(ccoFCall(gc0MultVarId(gcFiInitModulePrefix, int0, str),
					    int0)));
		gc0AddLine(gcvInitFunDeclsCC,
			   ccoDecl(ccoType(ccoExtern(),
					   ccoIdOf("int")),
				   ccoFCall(gc0MultVarId(gcFiInitModulePrefix, int0, str), int0)));
	}

	ccStorageClass = (imported &&
			  (p == FOAM_Proto_Foam || p == FOAM_Proto_Init) ?
			  ccoStatic() : ccoExtern());
	ccDecl = ccoDecl(ccoType(ccStorageClass, ccType), ccName);
	if (s0 != str) strFree(str);

	return ccDecl;
}


local CCode
gc0IdCDecl(Foam decl, CCode ccName)
{
	AInt	i, argc;
	Foam	fndecl;
	CCode	ccArgs;
	AInt	fmt = decl->foamGDecl.format;


	/* Ignore things with no format */
	if (fmt == emptyFormatSlot) return ccoFCall(ccoCopy(ccName), int0);


	/* Get the true declaration */
	fndecl	= gcvFmt->foamDFmt.argv[fmt];
	argc	= foamArgc(fndecl) - 1; /* skip CSig */
	ccArgs	= ccoNewNode(CCO_Many, argc);


	/* Process each argument (includes arguments for return values) */
	for (i = 0; i < argc; i++)
	{
		Foam	arg = fndecl->foamDDecl.argv[i];
		FoamTag type = arg->foamDecl.type;
		AInt	fmt = arg->foamDecl.format;
		String	str = arg->foamDecl.id;
		CCode	ccName = ccoIdOf(str);
		CCode	ccDecl = gc0TypeId(type, fmt);
		ccoArgv(ccArgs)[i] = ccoParam(ccName, ccDecl, ccoCopy(ccName));
	}

	/* Return the full declaration */
	return ccoFCall(ccoCopy(ccName), ccArgs);
}

local void
gc0IdFortranDecl(Foam decl, CCode *pName, CCode *pType)
{
	Foam fndecl = gcvFmt->foamDFmt.argv[decl->foamGDecl.format];
	Foam fnresultdecl = gc0GetFortranRetFm(fndecl);
	FortranType restype = gc0GetFortranType(fnresultdecl);
	FoamTag fmtype, rtag;
	String cmplxfns, str;
	CCode ccType, ccName;

	if (restype && (restype != FTN_Machine))
		fmtype = gen0FtnMachineType(restype);
	else
		fmtype = fnresultdecl->foamDecl.type;

	/* Char and Character are the same */
	if (fmtype == FOAM_Char)
		restype = FTN_Character;


	str = gc0GenFortranName(decl->foamGDecl.id);
	ccName = ccoIdOf(str);

	switch (restype) {
	  case FTN_Boolean:
		/* Fall through */
	  case FTN_SingleInteger:
		ccType    = ccoTypeIdOf(gcFiSInt);
		break;
	  case FTN_FSingle:
		ccType    = ccoTypeIdOf(gcFiSFlo);
		break;
	  case FTN_FDouble:
		ccType    = ccoTypeIdOf(gcFiDFlo);
		break;
	  case FTN_Character:
		/* Fall through */
	  case FTN_String:
		/* Fall through */
	  case FTN_XLString:
		/* String return values are passed as first argument */
		ccType = gc0TypeId(FOAM_NOp, emptyFormatSlot);
		break;
	  case FTN_FSComplex:
	  case FTN_FDComplex:
		cmplxfns = compCfgLookupString("fortran-cmplx-fns");
		ccType = NULL;
		if (!cmplxfns)
			comsgFatal(NULL, ALDOR_F_NoFCmplxProperty, "fortran-cmplx-fns");
		else if (strEqual(cmplxfns, "return-void"))
			ccType = gc0TypeId(FOAM_NOp, emptyFormatSlot);
		else if (strEqual(cmplxfns, "return-struct")) 
			ccType = ccoTypeIdOf(restype == FTN_FSComplex ? gcFiComplexSF : gcFiComplexDF); 
		else if (strEqual(cmplxfns, "disallowed"))
			bug("gc0GloIdDecl: Fortran function returning a complex result detected");
		else
			comsgFatal(NULL, ALDOR_F_BadFCmplxValue, cmplxfns);
		break;
	  default:
		/* Fortran passes LOGICAL values as INTEGERs. */
		rtag = (fmtype == FOAM_Bool) ? FOAM_SInt : decl->foamGDecl.rtype;
		ccType = gc0TypeId(rtag, emptyFormatSlot);
		break;
	}
	ccName = ccoFCall(ccName, int0);
	
	*pName = ccName;
	*pType = ccType;
}

local String
gc0StompOffIncludeFile(String str, FoamProtoTag p)
{
	int i;
	if (p != FOAM_Proto_C) return str;
	for (i = 0; str[i]; i++)
		if (str[i] == FOREIGN_INCLUDE_SEPARATOR) {
				str[i] = 0;
				break;
		}
	return str;
}


/* Decide whether the application of two casts has any useful effect */
/*
 *  Pointers are *not* the same as words because the gcc compiler will
 *  generate a warning if you attempt to pass a word in a context where
 *  a pointer was expected. 
 */
local Bool
gc0NeedBothCasts(FoamTag first, FoamTag second)
{
	/* (FooType)(FooType)E is the same as (FooType)E */
	if (first == second) return false;


	/* {Word,Ptr,Rec,Arr} and {Word,Ptr,Rec,Arr} are equivelent */
	switch (first)
	{
	  case FOAM_Word:	/* FALLTHROUGH */
/* See Comment Above	  case FOAM_Ptr:	FALLTHROUGH */
	  case FOAM_Rec:	/* FALLTHROUGH */
	  case FOAM_Arr:	/* FALLTHROUGH */
		switch (second)
		{
		  case FOAM_Word:	/*FALLTHROUGH*/
/* See Comment Above		  case FOAM_Ptr:	FALLTHROUGH*/
		  case FOAM_Rec:	/*FALLTHROUGH*/
		  case FOAM_Arr:	/*FALLTHROUGH*/
			return false;

		  default:
			return true;
		}
		break;

	  default:
		return true;
	}


	/* Assume that we need both of the casts */
	return true;
}


/*
 * gc0TryCast(type, foam) behaves exactly like gc0Cast(type, foam)
 * unless (foamTag(foam) == FOAM_Cast) && (foam->foamCast.type == type).
 * when it simply returns "foam".
 */
local CCode
gc0TryCast(FoamTag type, Foam foam)
{
	/* We don't want to generate "(FiFoo)(FiFoo)someExpr" */
	if (foamTag(foam) == FOAM_Cast)
	{
		if (!gc0NeedBothCasts(type, foam->foamCast.type))
			return gccExpr(foam);
	}
	return gc0Cast(type, foam);
}


local CCode
gc0Cast(FoamTag toType, Foam foam)
{
	CCode	cc;
	FoamTag fromType = gc0ExprType(foam);

	cc = gccExpr(foam);

	if (toType == FOAM_Rec) toType = FOAM_Word;

	if (fromType != FOAM_DFlo && toType == FOAM_DFlo) {
		if (fromType != FOAM_Word) 
			cc = ccoCast(gc0TypeId(FOAM_Word, emptyFormatSlot), cc);
		cc = ccoCast(gc0TypeId(toType, emptyFormatSlot),
			     ccoFCall(ccoIdOf("fiUnBoxDFlo"), cc));
	}
	else if (toType != FOAM_DFlo && fromType == FOAM_DFlo) {
		cc = ccoCast(gc0TypeId(toType, emptyFormatSlot),
			     ccoFCall(ccoIdOf("fiBoxDFlo"), cc));
		if (toType != FOAM_Word)
			cc = ccoCast(gc0TypeId(toType, emptyFormatSlot), cc);
	}
	else if (fromType != FOAM_SFlo && toType == FOAM_SFlo) {
		if (fromType != FOAM_Word) 
			cc = ccoCast(gc0TypeId(FOAM_Word, emptyFormatSlot), cc);
		cc = ccoFCall(ccoIdOf("fiWordToSFlo"), cc);
	}
	else if (toType != FOAM_SFlo && fromType == FOAM_SFlo) {
		cc = ccoFCall(ccoIdOf("fiSFloToWord"), cc);
		if (toType != FOAM_Word)
			cc = ccoCast(gc0TypeId(toType, emptyFormatSlot), cc);
	}
	else {
		cc = ccoCast(gc0TypeId(toType, emptyFormatSlot), cc);
	}

	return cc;
}

local CCode
gc0TypeId(AInt t, AInt fmt)
{
	CCode	cc;

	switch (t) {
	  case FOAM_NOp:
		cc = ccoTypeIdOf("void");
		break;
	  case FOAM_Nil:
		cc = ccoTypeIdOf(gcFiPtr);
		break;
	  case FOAM_Char:
		cc = ccoTypeIdOf(gcFiChar);
		break;
	  case FOAM_HInt:
		cc = ccoTypeIdOf(gcFiHInt);
		break;
	  case FOAM_Bool:
		cc = ccoTypeIdOf(gcFiBool);
		break;
	  case FOAM_Byte:
		cc = ccoTypeIdOf(gcFiByte);
		break;
	  case FOAM_SInt:
		cc = ccoTypeIdOf(gcFiSInt);
		break;
	  case FOAM_SFlo:
		cc = ccoTypeIdOf(gcFiSFlo);
		break;
	  case FOAM_DFlo:
		cc = ccoTypeIdOf(gcFiDFlo);
		break;
	  case FOAM_BInt:
		cc = ccoTypeIdOf(gcFiBInt);
		break;
	  case FOAM_RRec:
		cc = ccoTypeIdOf(gcFiRRec);
		break;
	  case FOAM_Rec:
		if (fmt == emptyFormatSlot)
			cc = ccoTypeIdOf(gcFiWord);
		else
			cc = ccoTypedefId(gc0VarId(gcFmtType, fmt));
		break;
	  case FOAM_TR:
		if (fmt == emptyFormatSlot)
			cc = ccoTypeIdOf(gcFiWord);
		else
			cc = ccoTypedefId(gc0VarId(gcFmtType, fmt));
		break;
	  case FOAM_Arr:
	        if (fmt == emptyFormatSlot || fmt == FOAM_Nil)
		        cc =  ccoTypeIdOf(gcFiPtr);
		else 
		        cc = ccoPostStar(gc0TypeId(fmt, emptyFormatSlot));
		break;
	  case FOAM_Env:
		cc = ccoTypeIdOf(gcFiEnv);
		break;
	  case FOAM_Prog:
		cc = ccoTypeIdOf(gcFiProg);
		break;
	  case FOAM_Clos:
		cc = ccoTypeIdOf(gcFiClos);
		break;
	  case FOAM_Ptr:
		cc = ccoTypeIdOf(gcFiPtr);
		break;
	  case FOAM_PRef:
		/* For now */
		cc = ccoTypeIdOf(gcFiSInt);
		break;
	  case FOAM_Word:
		cc = ccoTypeIdOf(gcFiWord);
		break;
	  case FOAM_Arb:
		cc = ccoTypeIdOf(gcFiArb);
		break;
	  case FOAM_JavaObj:
		cc = ccoTypeIdOf(gcFiWord);
		break;
	  default:
		bugBadCase(t);
		NotReached(return 0);
	}
	return cc;
}

local CCode
gc0SIntMod(Foam foam, CCodeTag ctag)
{
	CCode	cc, cc0, cc1;

	/* Order of the temporaries relies on order of evaluation here. */
	cc0 = gccExpr(foam->foamBCall.argv[0]);
	cc1 = gccExpr(foam->foamBCall.argv[1]);
	cc = ccoNew(ctag, 2, cc0, cc1);

	return ccoMod(cc, gccExpr(foam->foamBCall.argv[2]));
}

static Buffer gcvVarIdBuf = 0;

local CCode
gc0VarId(String str, int id)
{
	Buffer	buf;
 
	if (!gcvVarIdBuf) gcvVarIdBuf = bufNew();
	buf = gcvVarIdBuf;
 
	bufStart(buf);
	gc0ValidIdInBuf(buf, str);
	bufPuti(buf, id);
 
	return ccoIdOf(bufChars(buf));
}

local CCode
gc0MultVarId(String strA, int id, String strB)
{
	Buffer	buf;
 
	if (!gcvVarIdBuf) gcvVarIdBuf = bufNew();
	buf = gcvVarIdBuf;
 
	bufStart(buf);
 
	if ((strA[0] == 'G' && strA[1] == 0) ||
	    (strA[0] == 'p' && strA[1] == 'G' && strA[2] == 0)) {
		bufAddn(buf, strA, strLength(strA));
		bufAdd1(buf, '_');
		if (gcvIdHash) {
			gc0IdHashInBuf(buf, strB);
			bufAdd1(buf,'_');
		}
		gc0ValidIdInBuf(buf, strB);
	}
	else {
		if (isalpha(strA[0]) && strA[1] == 0)
			bufAdd1(buf, strA[0]);
		else {
			if (isdigit(strA[0]))
				bufAdd1(buf, '_');
			gc0ValidIdInBuf(buf, strA);
		}
		bufPuti(buf, id);
		if (strcmp(strB, "")) {
			bufAdd1(buf, '_');
			gc0ValidIdInBuf(buf, strB);
		}
	}
	return ccoIdOf(bufChars(buf));
}

local CCode
gc0FluidRef(Foam foam)
{
	int i = foam->foamFluid.index;
	Foam decl = gcvFluids->foamDDecl.argv[i];
	CCode ref;

	if (!listMemq(AInt)(gcvFluidList, (AInt)i))
		gcvFluidList=listCons(AInt)((AInt)i, gcvFluidList);

	ref = gcFiFluidValue(gc0MultVarId("F", i, decl->foamDecl.id));
	return ccoCast(gc0TypeId(decl->foamDecl.type, emptyFormatSlot), ref);
}

local CCode 
gc0FluidSet(Foam foamLHS, Foam foamRHS)
{
	int i = foamLHS->foamFluid.index;
	Foam decl = gcvFluids->foamDDecl.argv[i];
	CCode rhs;

	if (!listMemq(AInt)(gcvFluidList, (AInt)i))
		gcvFluidList=listCons(AInt)((AInt)i, gcvFluidList);

	rhs = gc0SubExpr(foamRHS, ccoTypeIdOf(gcFiWord));
	return gcFiSetFluid(gc0MultVarId("F", i, decl->foamDecl.id), rhs);
}

local CCode
gc0PushFluid()
{
	return ccoDecl(ccoTypeIdOf(gcFiFluidStack), 
		       ccoAsst(ccoIdOf(gcFiFluidStackLVar),
 			       ccoIdOf(gcFiFluidStackGVar)));
}

local CCode
gc0PopFluid()
{
	return ccoStat(ccoAsst(ccoIdOf(gcFiFluidStackGVar),
		       	       ccoIdOf(gcFiFluidStackLVar)));

}

local CCode
gc0GetFluid(AInt i)
{
	Foam decl = gcvFluids->foamDDecl.argv[i];
	
	return ccoStat(ccoAsst(gc0MultVarId("F", i, decl->foamDecl.id),
			       gcFiGetFluid(ccoStringOf(decl->foamDecl.id))));
}

local CCode
gc0AddFluid(AInt i)
{
	Foam decl = gcvFluids->foamDDecl.argv[i];
	
	return ccoStat(ccoAsst(gc0MultVarId("F", i, decl->foamDecl.id),
			       gcFiAddFluid(ccoStringOf(decl->foamDecl.id))));
}

/*****************************************************************************
 *
 * :: Miscellaneous utility functions.
 *
 ****************************************************************************/

local void
gc0AddLineFun(CCodeList *pccl, CCode cco)
{
	if (!cco) return;

	*pccl = listCons(CCode)(cco, *pccl);
}

/*
 * Given a piece of Foam code, return its Foam type.
 */

local FoamTag
gc0ExprType(Foam foam)
{
	if (foamTag(foam) == FOAM_Fluid)
		return FOAM_Word;

	return foamExprType(foam, gcvProg, gcvFmt, NULL, NULL, NULL);
}

local int
gc0IsDecl(Foam foam)
{
	int	isDecl = 0;

	switch (foamTag(foam)) {
	  case FOAM_Glo:
	  case FOAM_Const:
	  case FOAM_Par:
	  case FOAM_Loc:
	  case FOAM_Lex:
	  case FOAM_EElt:
	  case FOAM_RElt:
	  case FOAM_RRElt:
	  case FOAM_IRElt:
	  case FOAM_TRElt:
	  case FOAM_Fluid:
		isDecl = 1;
		break;
	  default:
		isDecl = 0;
		break;
	}
	return isDecl;
}


local CCode
gc0Decl(Foam decl, CCode name)
{
	CCode ccType, ccDecl;
	int fmt = decl->foamDecl.format;
	switch (decl->foamDecl.type) {
	  case FOAM_Rec:
		/* This should never happen, but it does (see bug1142),
		 * so handle it here. The correct way to fix it is to
		 * put format information into OCall statements, so the
		 * foamExprType can predict the type properly.
		 */
		/*assert(fmt && fmt != emptyFormatSlot);*/
		if (!fmt || fmt == emptyFormatSlot) {
			ccType = gc0TypeId(FOAM_Word, emptyFormatSlot);
			ccDecl = ccoDecl(ccType, name);
		}
		else {
			ccType = ccoTypedefId(gc0VarId(gcFmtType, fmt));
			ccDecl = ccoDecl(ccType, name);
		}
		break;
	  case FOAM_Arr:
		ccType = gc0TypeId(fmt, emptyFormatSlot);
		ccDecl = ccoDecl(ccType, ccoPreStar(name));
		break;
	  case FOAM_TR:
		ccType = ccoTypedefId(gc0VarId(gcFmtType, fmt));
		ccDecl = ccoDecl(ccType, name);
		break;
	  default:
		ccType = gc0TypeId(decl->foamDecl.type, emptyFormatSlot);
		ccDecl = ccoDecl(ccType, name);
		break;
	}
	return ccDecl;
}

/*
 * Get the declaration object for a global, constant, parameter,
 * local, lexical, or environment.
 */

local Foam
gc0GetDecl(Foam foam)
{
	int	ix;
	Foam	decl;

	switch (foamTag(foam)) {
	  case FOAM_Glo:
		ix   = foam->foamGlo.index;
		decl = gcvGlo->foamDDecl.argv[ix];
		break;
	  case FOAM_Const:
		ix   = foam->foamConst.index;
		decl = gcvConst->foamDDecl.argv[ix];
		break;
	  case FOAM_Par:
		ix   = foam->foamPar.index;
		decl = gcvPar->foamDDecl.argv[ix];
		break;
	  case FOAM_Loc:
		ix   = foam->foamLoc.index;
		decl = gcvLoc->foamDDecl.argv[ix];
		break;
	  case FOAM_Fluid:
		ix = foam->foamFluid.index;
		decl = gcvFluids->foamDDecl.argv[ix];
		break;
	  case FOAM_Lex: {
		int	ind;
		Foam	ddecl;
		ind   = gcvLFmtStk->foamDEnv.argv[foam->foamLex.level];
		ddecl = gcvFmt->foamDFmt.argv[ind];
		assert(foamTag(ddecl) == FOAM_DDecl);
		ix    = foam->foamLex.index;
		assert(ix < foamDDeclArgc(ddecl));
		decl  = ddecl->foamDDecl.argv[ix];
		break;
	  }
	  case FOAM_EElt: {
		Foam	ddecl;
		ddecl = gcvFmt->foamDFmt.argv[foam->foamEElt.env];
		assert(foamTag(ddecl) == FOAM_DDecl);
		ix    = foam->foamEElt.lex;
		decl  = ddecl->foamDDecl.argv[ix];
		break;
	  }
	  case FOAM_RRElt: {
		decl = foamNewDecl(FOAM_Word, "_rawdata", FOAM_Word);
		break;
	  }
	  case FOAM_RElt: {
		Foam	ddecl;
		ddecl = gcvFmt->foamDFmt.argv[foam->foamRElt.format];
		assert(foamTag(ddecl) == FOAM_DDecl);
		ix    = foam->foamRElt.field;
		decl  = ddecl->foamDDecl.argv[ix];
		break;
	  }
	  case FOAM_IRElt: {
		Foam	ddecl;
		ddecl = gcvFmt->foamDFmt.argv[foam->foamIRElt.format];
		assert(foamTag(ddecl) == FOAM_DDecl);
		ix    = foam->foamIRElt.field;
		decl  = foamTRDDeclIDecl(ddecl, ix);
		break;
	  }
	  case FOAM_TRElt: {
		Foam	ddecl;
		ddecl = gcvFmt->foamDFmt.argv[foam->foamTRElt.format];
		assert(foamTag(ddecl) == FOAM_DDecl);
		ix    = foam->foamTRElt.field;
		decl  = foamTRDDeclTDecl(ddecl, ix);
		break;
	  }
	  default:
		bugBadCase(foamTag(foam));
		NotReached(return 0);
	}
	assert(foamTag(decl) == FOAM_Decl || foamTag(decl) == FOAM_GDecl);
	return decl;
}

local int
gc0ValidIdInBuf(Buffer buf, String s)
{
	int	pos0;
	pos0 = bufPosition(buf);
	for ( ; *s && gc0UnderIdLen(buf, (int)*s); s++) {
		int k = gcvIdChars[(int)*s];
		if (k == NOT_CHANGED)
			bufAdd1(buf, *s);
		else if (k != NOT_PRINTABLE)
			bufPuts(buf, ccIdStr(k));
	}
	bufAdd1(buf, char0);
	bufBack1(buf);
	return bufPosition(buf) - pos0;
}
 
#define VAR_HASH_MAX	5
#define VAR_HASH	0x39AA3F9	/* prevPrime(36**VAR_HASH_MAX) */

local int
gc0IdHashInBuf(Buffer buf, String s)
{
	Hash	hashNum;
	int	ndig, alphnum[4 * bitsizeof(hashNum)];
	int	pos0;

	pos0 = bufPosition(buf);
	hashNum = strHash(s) % VAR_HASH;
	for (ndig = 0; hashNum; hashNum /= 36, ndig++)
		alphnum[ndig] = hashNum % 36;

	while (ndig--) {
		char	c;

		if (alphnum[ndig] < 10)
			c = '0' + alphnum[ndig];
		else
			c = 'A' + (alphnum[ndig] - 10);
		bufAdd1(buf, c);
	}
	bufAdd1(buf, char0);
	bufBack1(buf);
	return bufPosition(buf) - pos0;
}
 
local void
gc0InitSpecialChars(void)
{
	int	i;
 
	for (i = 0; i < CHAR_MAX; i++) {
		if (isalnum(i)) {
			gcvIdChars[i] = NOT_CHANGED;
			gcvIdCharc[i] = 1;
		}
		else {
			gcvIdChars[i] = NOT_PRINTABLE;
			gcvIdCharc[i] = 0;
		}
	}
	for (i = 0; ccIdChar(i) != 0; i++) {
		gcvIdChars[ccIdChar(i)] = i;
		gcvIdCharc[ccIdChar(i)] = strLength(ccIdStr(i));
	}
	return;
}

local void
gc0CreateLocList(Foam foam)
{
	int		i;

	for (i = 0; i < foamDDeclArgc(foam); i++) {
		gc0AddUnSortedLocal(gc0LocRef(foam, i));
	}
	return;
}

local void 
gc0AddUnSortedLocal(CCode ref)
{
	struct Clocals *lst;
	
	lst = gcvLocals;
	gcvLocals =  (struct Clocals *)stoAlloc(OB_Other,
					        sizeof(*gcvLocals));
	gcvLocals->index = gcvNLocs++;
	gcvLocals->loc   = ref;
	gcvLocals->next  = lst;
}


local void
gc0NewLocsInit()
{
	gcvNewLocs       = (Ldecls) stoAlloc((int) OB_Other, 
					     sizeof(*gcvNewLocs));
	gcvNewLocs->list = (struct CList *) stoAlloc((int) OB_Other,
						     sizeof(struct CList));
	gcvNewLocs->pos  = 0;
	gcvNewLocs->list[gcvNewLocs->pos].type = 
	                    (CCode) stoAlloc((int) OB_CCode,
					fullsizeof(struct ccoNode, 1, CCode));
	gcvNewLocs->list[gcvNewLocs->pos].lsize = 0;
	gcvNewLocs->list[gcvNewLocs->pos].locs  = 0;
	gcvNewLocs->argc = 0;
	return;
}

/*
 * The 'Cstmts' type is really just a total waste, but
 * then so is the rest of this file.
 */
local void
gc0NewStmtInit()
{
	gcvStmts       = (Cstmts)  stoAlloc((int) OB_Other, sizeof(*gcvStmts));

	gcvStmts->stmt = (CCode *) stoAlloc((int) OB_Other,
					    sizeof(CCode) * gcvNStmts);
	gcvStmts->argc = gcvNStmts;
	gcvStmts->pos  = 0;
	return;
}

local void
gc0AddTopLevelStmt(Cstmts stmts, CCode stmt)
{
	if (stmts->pos >= stmts->argc) {
		int newsz   = stmts->argc+20;
		stmts->stmt = (CCode*)stoResize(stmts->stmt, 
					sizeof(CCode) * newsz);
		stmts->argc = newsz;
	}
	stmts->stmt[stmts->pos++] = stmt;
}

local void
gc0AddDecl(CCode cc, int indx)
{
	int	i, args, done;

	args = gcvNewLocs->argc;
	if (!args) {
		gcvNewLocs->list[gcvNewLocs->pos].type = ccoArgv(cc)[0];
		gc0AddLocal(ccoArgv(cc)[1], gcvNewLocs->pos, indx);
		gcvNewLocs->argc++;
		gcvNewLocs->pos++;
	} else {
		done = 0;
		for (i = 0; i < args && !done; i++) {
			if (ccoTypeEqual(gcvNewLocs->list[i].type,
				   ccoArgv(cc)[0])) {
				gc0AddLocal(ccoArgv(cc)[1], i, indx);
				done = 1;
			}
		}
		if (!done) {
			gcvNewLocs->argc++;
			gcvNewLocs->list = (struct CList *) stoResize(gcvNewLocs->list, sizeof(struct CList)*(gcvNewLocs->argc));
			gcvNewLocs->list[gcvNewLocs->pos].type =
				(CCode) stoAlloc((int) OB_CCode,
					fullsizeof(struct ccoNode, 1,
						CCode));
			gcvNewLocs->list[gcvNewLocs->pos].type  = ccoArgv(cc)[0];
			gcvNewLocs->list[gcvNewLocs->pos].lsize = 0;
			gcvNewLocs->list[gcvNewLocs->pos].locs  = 0;
			gc0AddLocal(ccoArgv(cc)[1], gcvNewLocs->pos, indx);
			gcvNewLocs->pos++;
		}
	}
	return;
}

local void
gc0AddLocal(CCode cc, int indx, int loc)
{
	struct Clocals	*nlocs;

	nlocs = gcvNewLocs->list[indx].locs;
	gcvNewLocs->list[indx].locs = (struct Clocals *) 
		stoAlloc((int) OB_Other, sizeof(*gcvNewLocs->list[indx].locs));
	gcvNewLocs->list[indx].lsize++;
	gcvNewLocs->list[indx].locs->index = loc;
	gcvNewLocs->list[indx].locs->loc   = cc;
	gcvNewLocs->list[indx].locs->next  = nlocs;
	return;
}

local void
gc0NewLocals(CCode cc)
{
	int		i, n, size = 0;
	struct Clocals	*nlocs;
	CCode		ccNew;

	for (i = 0; i < gcvNewLocs->argc; i++) {
		ccNew = ccoNewNode(CCO_Many, gcvNewLocs->list[i].lsize);
		size += gcvNewLocs->list[i].lsize;
		for (n = 0; n < gcvNewLocs->list[i].lsize; n++) {
			nlocs = gcvNewLocs->list[i].locs->next;
			ccoArgv(cc)[gcvNewLocs->list[i].locs->index] = 0;
			ccoArgv(ccNew)[n] = gcvNewLocs->list[i].locs->loc;
			stoFree((Pointer) gcvNewLocs->list[i].locs);
			gcvNewLocs->list[i].locs = nlocs;
		}
		ccoArgv(cc)[i] = ccoDecl(ccoCopy(gcvNewLocs->list[i].type),
					 ccNew);
		stoFree((Pointer) gcvNewLocs->list[i].type);
	}
	stoFree((Pointer) (gcvNewLocs->list));
	stoFree((Pointer) (gcvNewLocs));
	return;
}

local int
gc0MaxLevel(int numLexs)
{
	int	i, level, maxLevel = -1;

	for (i = 1; i < numLexs; i++) {
		level = gcvLFmtStk->foamDEnv.argv[i];
		if (!gc0EmptyEnv(level) || level == envUsedSlot) maxLevel = i;
	}
	return maxLevel;
}

local CCode
gc0ModuleInitFun(String modName, int n)
{
	return gc0MultVarId(gcFiInitModulePrefix, n, modName);
}

void
ccodeListPrintDb(CCodeList cl)
{
	for (; cl; cl = cdr(cl))
		ccoPrintDb(car(cl));
}

local Bool
gc0IsModifiableFortranArg(Foam decl)
{
	if (strncmp(decl->foamDecl.id, MODIFIABLEARG, 
		    strlen(MODIFIABLEARG)) == 0)
		return true;
	else 
		return false;
}

local String
gc0GetFortranArgName(Foam decl)
{
	String sep, s;

	/* Export/fn parameter arguments don't have MODIFIABLEARGs */
	if (decl->foamDecl.type == FOAM_Word) {
		if ((sep = strchr(decl->foamDecl.id,':'))) {
			*sep = '\0';
			s = strCopy(decl->foamDecl.id);
			*sep = ':';
		}
		else
			s = strCopy("");
	}
	else
		s = strCopy(decl->foamDecl.id);
	return s;
}

local FortranType
gc0GetFortranType(Foam decl)
{
	String		s, s2;
	FortranType	ftype;

	if (decl->foamDecl.type == FOAM_Clos)
		return FTN_FnParam;
	else if (decl->foamDecl.type != FOAM_Word)
		return FTN_Machine;
	else {
		if (strncmp(decl->foamDecl.id, MODIFIABLEARG, 
			    strlen(MODIFIABLEARG)) == 0)
			s = decl->foamDecl.id + strlen(MODIFIABLEARG);
		else
			s = decl->foamDecl.id;
		s2 = strchr(s,':');
		if (s2)	s = s2 + 1;

		/* Check the category attribute */
		ftype = ftnTypeFrString(s);


		/* Did we recognise it? */
		if (ftype)
			return ftype;
		else
			return FTN_Word;
	}
}

local int
gc0GetNumModFortranArgs(Foam argddecl)
{
	Length i, argc = foamArgc(argddecl)-1;
	Foam *argv = argddecl->foamDDecl.argv;
	int numvarargs = 0;

	/* argc-1 to avoid the function result decl */
	for (i = 0; i < argc-1; i++) {
		if (gc0IsModifiableFortranArg(argv[i]))
			numvarargs++;
	}
	return numvarargs;
}

local Foam
gc0GetFortranRetFm(Foam argddecl)
{
	return argddecl->foamDDecl.argv[(foamArgc(argddecl)-1)-1];
}

local FortranType
gc0GetFortranRetType(Foam argddecl)
{
	Foam fnresultdecl = gc0GetFortranRetFm(argddecl);
	return gc0GetFortranType(fnresultdecl);
}

local String 
gc0GenFortranName(String str)
{
	String naming = compCfgLookupString("fortran-name-scheme"); 

	if (!naming)
		comsgFatal(NULL, ALDOR_F_NoFNameProperty, "fortran-name-scheme");
	if (strEqual(naming, "underscore"))
		return strConcat(str, "_");
	else if (strEqual(naming, "no-underscore"))
		return strCopy(str);
	else if (strEqual(naming, "underscore-bug")) {
		if (strchr(str, '_'))
			return strConcat(str, "__");
		else
			return strConcat(str, "_");
	}
	else  
		comsgFatal(NULL, ALDOR_F_BadFNameValue, naming);

	return NULL;
}

/*
 * This function appends a redundant FOAM return instruction
 * after every fiHalt call. This is to get around a bug in
 * the MSVC++ 6.0 compiler which enters an infinite loop when
 * it tries to optimise a C function containing non-obvious
 * exits (e.g. calls to fiHalt or even exit).
 */
local Foam
gc0AddExplicitReturn(Foam foam)
{
	FoamBValTag	tag;
	Foam		retval;
	FoamList	newcode = listNil(Foam);
	int		nhalts = 0;


	/* Must be a sequence ... */
	assert(foamTag(foam) == FOAM_Seq);


	/* Must be in a gc0Prog */
	assert(gcvProg);


	/* Count the number of halt instructions */
	foamIter(foam, pstmt,
	{
		/* Must be a BCall */
		if (foamTag(*pstmt) != FOAM_BCall) continue;


		/* Get the tag of the call */
		tag = foamBValInfo((*pstmt)->foamBCall.op).tag;


		/* Must be FOAM_BVal_Halt */
		if (tag == FOAM_BVal_Halt) nhalts++;
	});


	/* If no halts, return unchanged */
	if (!nhalts) return foam;


	/* Compute a suitable return value */
	switch (gcvProg->foamProg.retType)
	{
	   case FOAM_NOp:
		/* No return value or multiple return values */
		retval = foamNew(FOAM_Values, int0);
		break;
	   case FOAM_Bool:
		/* Return false */
		retval = foamNew(FOAM_BCall, 1, FOAM_BVal_BoolFalse);
		break;
	   case FOAM_Char:
		/* Return space */
		retval = foamNew(FOAM_BCall, 1, FOAM_BVal_CharSpace);
		break;
	   case FOAM_SFlo:
		/* Return 0.0 */
		retval = foamNew(FOAM_BCall, 1, FOAM_BVal_SFlo0);
		break;
	   case FOAM_DFlo:
		/* Return 0.0 */
		retval = foamNew(FOAM_BCall, 1, FOAM_BVal_DFlo0);
		break;
	   default:
		/* Return (0 pretend retType) */
		retval = foamNewCast(gcvProg->foamProg.retType,
			foamNew(FOAM_BCall, 1, FOAM_BVal_SInt0));
		break;
	}


	/* Convert the sequence into a statement list */
	foamIter(foam, pstmt,
	{
		Foam	ret;


		/* Always add the statement to the list */
		newcode = listCons(Foam)(foamCopy(*pstmt), newcode);


		/* Must be a BCall */
		if (foamTag(*pstmt) != FOAM_BCall) continue;


		/* Get the tag of the call */
		tag = foamBValInfo((*pstmt)->foamBCall.op).tag;


		/* Must be FOAM_BVal_Halt */
		if (tag != FOAM_BVal_Halt) continue;


		/* Create a new statement return */
		ret = foamNewReturn(foamCopy(retval));


		/* Add the explicit return */
		newcode = listCons(Foam)(ret, newcode);
	});


	/* Reverse the instruction list */
	newcode = listNReverse(Foam)(newcode);


	/* Return the modified code */
	return foamNewOfList(FOAM_Seq, newcode);
}


/***********************************************************************
 *
 * :: Table of builtin FOAM to C operations
 *
 ***********************************************************************/

local void
gc0CheckBVals(void)
{
	int	i;
	for (i = FOAM_BVAL_START; i < FOAM_BVAL_LIMIT; i++)
		if (ccBValInfo(i).tag != i)
			bug("ccBValInfo is badly initialized at %s = %d.",
			    ccBValStr(i), i);
}

struct ccBVal_info ccBValInfoTable[] = {
 {FOAM_BVal_BoolFalse,    CCO_IntVal,  1,"0",              0},
 {FOAM_BVal_BoolTrue,     CCO_IntVal,  1,"1",              0},
 {FOAM_BVal_BoolNot,      CCO_LNot,    0,0,                0},
 {FOAM_BVal_BoolAnd,      CCO_And,     0,0,                0},
 {FOAM_BVal_BoolOr,       CCO_Or,      0,0,                0},
 {FOAM_BVal_BoolEQ,       CCO_EQ,      0,0,                0},
 {FOAM_BVal_BoolNE,       CCO_NE,      0,0,                0},

 {FOAM_BVal_CharSpace,    CCO_CharVal, 1,"' '",            0},
 {FOAM_BVal_CharNewline,  CCO_CharVal, 1,"'\\n'",          0},
 {FOAM_BVal_CharTab,      CCO_CharVal, 1,"'\\t'",          0},
 {FOAM_BVal_CharMin,      CCO_FCall,   0,"fiCharMin",      "fiCHAR_MIN"},
 {FOAM_BVal_CharMax,      CCO_FCall,   0,"fiCharMax",      "fiCHAR_MAX"},
 {FOAM_BVal_CharIsDigit,  CCO_FCall,   0,"fiCharIsDigit",  "fiCHAR_IS_DIGIT"},
 {FOAM_BVal_CharIsLetter, CCO_FCall,   0,"fiCharIsLetter", "fiCHAR_IS_LETTER"},
 {FOAM_BVal_CharEQ,       CCO_EQ,      0,0,                0},
 {FOAM_BVal_CharNE,       CCO_NE,      0,0,                0},
 {FOAM_BVal_CharLT,       CCO_LT,      0,0,                0},
 {FOAM_BVal_CharLE,       CCO_LE,      0,0,                0},
 {FOAM_BVal_CharLower,    CCO_FCall,   0,"fiCharLower",    "fiCHAR_LOWER"},
 {FOAM_BVal_CharUpper,    CCO_FCall,   0,"fiCharUpper",    "fiCHAR_UPPER"},
 {FOAM_BVal_CharOrd,      CCO_Cast,    0,gcFiSInt,         0},
 {FOAM_BVal_CharNum,      CCO_Cast,    0,"char",           0},

 {FOAM_BVal_SFlo0,        CCO_FloatVal,1,"0.0",            0},
 {FOAM_BVal_SFlo1,        CCO_FloatVal,1,"1.0",            0},
 {FOAM_BVal_SFloMin,      CCO_FCall,   0,"fiSFloMin",      "fiSFLO_MIN"},
 {FOAM_BVal_SFloMax,      CCO_FCall,   0,"fiSFloMax",      "fiSFLO_MAX"},
 {FOAM_BVal_SFloEpsilon,  CCO_FCall,   0,"fiSFloEpsilon",  "fiSFLO_EPSILON"},
 {FOAM_BVal_SFloIsZero,   CCO_EQ,      1,"0.0",            0},
 {FOAM_BVal_SFloIsNeg,    CCO_LT,      1,"0.0",            0},
 {FOAM_BVal_SFloIsPos,    CCO_GT,      1,"0.0",            0},
 {FOAM_BVal_SFloEQ,       CCO_EQ,      0,0,                0},
 {FOAM_BVal_SFloNE,       CCO_NE,      0,0,                0},
 {FOAM_BVal_SFloLT,       CCO_LT,      0,0,                0},
 {FOAM_BVal_SFloLE,       CCO_LE,      0,0,                0},
 {FOAM_BVal_SFloNegate,   CCO_PreMinus,0,0,                0},
 {FOAM_BVal_SFloPrev,     CCO_FCall,   0,"fiSFloPrev",     "fiSFLO_PREV"},
 {FOAM_BVal_SFloNext,     CCO_FCall,   0,"fiSFloNext",     "fiSFLO_NEXT"},
 {FOAM_BVal_SFloPlus,     CCO_Plus,    0,0,                0},
 {FOAM_BVal_SFloMinus,    CCO_Minus,   0,0,                0},
 {FOAM_BVal_SFloTimes,    CCO_Star,    0,0,                0},
 {FOAM_BVal_SFloTimesPlus, CCO_FCall,  0,"fiSFloTimesPlus","fiSFLO_TIMES_PLUS"},
 {FOAM_BVal_SFloDivide,    CCO_Div,    0,0,                0},
 {FOAM_BVal_SFloRPlus,     CCO_FCall,  0,"fiSFloRPlus",     "fiSFLO_R_PLUS"},
 {FOAM_BVal_SFloRMinus,    CCO_FCall,  0,"fiSFloRMinus",    "fiSFLO_R_MINUS"},
 {FOAM_BVal_SFloRTimes,    CCO_FCall,  0,"fiSFloRTimes",    "fiSFLO_R_TIMES"},
 {FOAM_BVal_SFloRTimesPlus,CCO_FCall,  0,"fiSFloRTimesPlus","fiSFLO_R_TIMES_PLUS"},
 {FOAM_BVal_SFloRDivide,   CCO_FCall,  0,"fiSFloRDivide",   "fiSFLO_R_DIVIDE"},
 {FOAM_BVal_SFloDissemble, CCO_FCall,  0,"fiSFloDissemble", 0},
 {FOAM_BVal_SFloAssemble,  CCO_FCall,  0,"fiSFloAssemble",  0},

 {FOAM_BVal_DFlo0,         CCO_FloatVal,1,"0.0",            0},
 {FOAM_BVal_DFlo1,         CCO_FloatVal,1,"1.0",            0},
 {FOAM_BVal_DFloMin,       CCO_FCall,  0,"fiDFloMin",       "fiDFLO_MIN"},
 {FOAM_BVal_DFloMax,       CCO_FCall,  0,"fiDFloMax",       "fiDFLO_MAX"},
 {FOAM_BVal_DFloEpsilon,   CCO_FCall,  0,"fiDFloEpsilon",   "fiDFLO_EPSILON"},
 {FOAM_BVal_DFloIsZero,    CCO_EQ,     1,"0.0",             0},
 {FOAM_BVal_DFloIsNeg,     CCO_LT,     1,"0.0",             0},
 {FOAM_BVal_DFloIsPos,     CCO_GT,     1,"0.0",             0},
 {FOAM_BVal_DFloEQ,        CCO_EQ,     0,0,                 0},
 {FOAM_BVal_DFloNE,        CCO_NE,     0,0,                 0},
 {FOAM_BVal_DFloLT,        CCO_LT,     0,0,                 0},
 {FOAM_BVal_DFloLE,        CCO_LE,     0,0,                 0},
 {FOAM_BVal_DFloNegate,    CCO_PreMinus,0,0,                0},
 {FOAM_BVal_DFloPrev,      CCO_FCall,  0,"fiDFloPrev",      "fiDFLO_PREV"},
 {FOAM_BVal_DFloNext,      CCO_FCall,  0,"fiDFloNext",      "fiDFLO_NEXT"},
 {FOAM_BVal_DFloPlus,      CCO_Plus,   0,0,                 0},
 {FOAM_BVal_DFloMinus,     CCO_Minus,  0,0,                 0},
 {FOAM_BVal_DFloTimes,     CCO_Star,   0,0,                 0},
 {FOAM_BVal_DFloTimesPlus, CCO_FCall,  0,"fiDFloTimesPlus","fiDFLO_TIMES_PLUS"},
 {FOAM_BVal_DFloDivide,    CCO_Div,    0,0,                 0},
 {FOAM_BVal_DFloRPlus,     CCO_Plus,   0,"fiSFloRPlus",     "fiDFLO_R_PLUS"},
 {FOAM_BVal_DFloRMinus,    CCO_FCall,  0,"fiSFloRMinus",    "fiDFLO_R_MINUS"},
 {FOAM_BVal_DFloRTimes,    CCO_FCall,  0,"fiSFloRTimes",    "fiDFLO_R_TIMES"},
 {FOAM_BVal_DFloRTimesPlus,CCO_FCall,  0,"fiDFloRTimesPlus","fiDFLO_R_TIMES_PLUS"},
 {FOAM_BVal_DFloRDivide,   CCO_FCall,  0,"fiSFloRDivide",   "fiDFLO_R_DIVIDE"},
 {FOAM_BVal_DFloDissemble, CCO_FCall,  0,"fiDFloDissemble", 0},
 {FOAM_BVal_DFloAssemble,  CCO_FCall,  0,"fiDFloAssemble",  0},

 {FOAM_BVal_Byte0,        CCO_IntVal,  1,"0",               0},
 {FOAM_BVal_Byte1,        CCO_IntVal,  1,"1",               0},
 {FOAM_BVal_ByteMin,      CCO_Id,      1,"0",               0},
 {FOAM_BVal_ByteMax,      CCO_FCall,   0,"fiByteMax",       "fiBYTE_MAX"},

 {FOAM_BVal_HInt0,        CCO_IntVal,  1,"0",              0},
 {FOAM_BVal_HInt1,        CCO_IntVal,  1,"1",              0},
 {FOAM_BVal_HIntMin,      CCO_FCall,   0,"fiHIntMin",      "fiHINT_MIN"},
 {FOAM_BVal_HIntMax,      CCO_FCall,   0,"fiHIntMax",      "fiHINT_MAX"},

 {FOAM_BVal_SInt0,        CCO_IntVal,  1,"0L",              0},
 {FOAM_BVal_SInt1,        CCO_IntVal,  1,"1L",              0},
 {FOAM_BVal_SIntMin,      CCO_FCall,   0,"fiSIntMin",      "fiSINT_MIN"},
 {FOAM_BVal_SIntMax,      CCO_FCall,   0,"fiSIntMax",      "fiSINT_MAX"},
 {FOAM_BVal_SIntIsZero,   CCO_EQ,      1,"0",              0},
 {FOAM_BVal_SIntIsNeg,    CCO_LT,      1,"0",              0},
 {FOAM_BVal_SIntIsPos,    CCO_GT,      1,"0",              0},
 {FOAM_BVal_SIntIsEven,   CCO_EQ,      2,0,                0},
 {FOAM_BVal_SIntIsOdd,    CCO_NE,      2,0,                0},
 {FOAM_BVal_SIntEQ,       CCO_EQ,      0,0,                0},
 {FOAM_BVal_SIntNE,       CCO_NE,      0,0,                0},
 {FOAM_BVal_SIntLT,       CCO_LT,      0,0,                0},
 {FOAM_BVal_SIntLE,       CCO_LE,      0,0,                0},
 {FOAM_BVal_SIntNegate,   CCO_PreMinus,0,0,                0},
 {FOAM_BVal_SIntPrev,     CCO_Minus,   1,"1L",              0},
 {FOAM_BVal_SIntNext,     CCO_Plus,    1,"1L",              0},
 {FOAM_BVal_SIntPlus,     CCO_Plus,    0,0,                0},
 {FOAM_BVal_SIntMinus,    CCO_Minus,   0,0,                0},
 {FOAM_BVal_SIntTimes,    CCO_Star,    0,0,                0},
 {FOAM_BVal_SIntTimesPlus,CCO_FCall,   0,"fiSIntTimesPlus","fiSINT_TIMES_PLUS"},
 {FOAM_BVal_SIntMod,      CCO_Mod,     0,0,                0},
 {FOAM_BVal_SIntQuo,      CCO_FCall,   0,"fiSIntQuo",      "fiSINT_QUO"},
 {FOAM_BVal_SIntRem,      CCO_FCall,   0,"fiSIntRem",      "fiSINT_REM"},
 {FOAM_BVal_SIntDivide,   CCO_FCall,   0,"fiSIntDivide",   0},
 {FOAM_BVal_SIntGcd,      CCO_FCall,   0,"fiSIntGcd",      "fiSINT_GCD"},
 {FOAM_BVal_SIntPlusMod,  CCO_Plus,    2,0,                0},
 {FOAM_BVal_SIntMinusMod, CCO_Minus,   2,0,                0},
 {FOAM_BVal_SIntTimesMod,   CCO_FCall, 0,"fiSIntTimesMod", "fiSINT_TIMES_MOD"},
 {FOAM_BVal_SIntTimesModInv,CCO_FCall, 0,"fiSIntTimesModInv","fiSINT_TIMES_MOD_INV"},
 {FOAM_BVal_SIntLength,   CCO_FCall,   0,"fiSIntLength",   "fiSINT_LENGTH"},
 {FOAM_BVal_SIntShiftUp,  CCO_USh,     0,0,                0},
 {FOAM_BVal_SIntShiftDn,  CCO_DSh,     0,0,                0},
 {FOAM_BVal_SIntBit,      CCO_FCall,   0,"fiSIntBit",      "fiSINT_BIT"},
 {FOAM_BVal_SIntNot,      CCO_Not,     0,0,                0},
 {FOAM_BVal_SIntAnd,      CCO_And,     0,0,                0},
 {FOAM_BVal_SIntOr,       CCO_Or,      0,0,                0},
 {FOAM_BVal_SIntXOr,      CCO_Xor,     0,0,                0},
 {FOAM_BVal_SIntHashCombine, CCO_FCall,0,"fiSIntHashCombine",  "fiSINT_HASHCOMBINE"},

 {FOAM_BVal_WordTimesDouble, CCO_FCall,0,"fiWordTimesDouble",  0},
 {FOAM_BVal_WordDivideDouble,CCO_FCall,0,"fiWordDivideDouble", 0},
 {FOAM_BVal_WordPlusStep,    CCO_FCall,0,"fiWordPlusStep",     0},
 {FOAM_BVal_WordTimesStep,   CCO_FCall,0,"fiWordTimesStep",    0},

 {FOAM_BVal_BInt0,        CCO_FCall,   0,"fiBInt0",        "fiBINT_0"},
 {FOAM_BVal_BInt1,        CCO_FCall,   0,"fiBInt1",        "fiBINT_1"},
 {FOAM_BVal_BIntIsZero,   CCO_FCall,   0,"fiBIntIsZero",   "fiBINT_IS_ZERO"},
 {FOAM_BVal_BIntIsNeg,    CCO_FCall,   0,"fiBIntIsNeg",    "fiBINT_IS_NEG"},
 {FOAM_BVal_BIntIsPos,    CCO_FCall,   0,"fiBIntIsPos",    "fiBINT_IS_POS"},
 {FOAM_BVal_BIntIsEven,   CCO_FCall,   1,"fiBIntEQ",       "fiBINT_IS_EVEN"},
 {FOAM_BVal_BIntIsOdd,    CCO_FCall,   1,"fiBIntNE",       "fiBINT_IS_ODD"},
 {FOAM_BVal_BIntIsSingle, CCO_FCall,   0,"fiBIntIsSingle", "fiBINT_IS_SINGLE"},
 {FOAM_BVal_BIntEQ,       CCO_FCall,   0,"fiBIntEQ",       "fiBINT_EQ"},
 {FOAM_BVal_BIntNE,       CCO_FCall,   0,"fiBIntNE",       "fiBINT_NE"},
 {FOAM_BVal_BIntLT,       CCO_FCall,   0,"fiBIntLT",       "fiBINT_LT"},
 {FOAM_BVal_BIntLE,       CCO_FCall,   0,"fiBIntLE",       "fiBINT_LE"},
 {FOAM_BVal_BIntNegate,   CCO_FCall,   0,"fiBIntNegate",   "fiBINT_NEGATE"},
 {FOAM_BVal_BIntPrev,     CCO_FCall,   1,"fiBIntMinus",    "fiBINT_MINUS1"},
 {FOAM_BVal_BIntNext,     CCO_FCall,   1,"fiBIntPlus",     "fiBINT_PLUS1"},
 {FOAM_BVal_BIntPlus,     CCO_FCall,   0,"fiBIntPlus",     "fiBINT_PLUS"},
 {FOAM_BVal_BIntMinus,    CCO_FCall,   0,"fiBIntMinus",    "fiBINT_MINUS"},
 {FOAM_BVal_BIntTimes,    CCO_FCall,   0,"fiBIntTimes",    "fiBINT_TIMES"},
 {FOAM_BVal_BIntTimesPlus,CCO_FCall,   0,"fiBIntTimesPlus","fiBINT_TIMES_PLUS"},
 {FOAM_BVal_BIntMod,      CCO_FCall,   0,"fiBIntMod",      "fiBINT_MOD"},
 {FOAM_BVal_BIntQuo,      CCO_FCall,   0,"fiBIntQuo",      "fiBINT_QUO"},
 {FOAM_BVal_BIntRem,      CCO_FCall,   0,"fiBIntRem",      "fiBINT_REM"},
 {FOAM_BVal_BIntDivide,   CCO_FCall,   0,"fiBIntDivide",   0},
 {FOAM_BVal_BIntGcd,      CCO_FCall,   0,"fiBIntGcd",      "fiBINT_GCD"},
 {FOAM_BVal_BIntSIPower,  CCO_FCall,   0,"fiBIntSIPower",  "fiBINT_SI_POWER"},
 {FOAM_BVal_BIntBIPower,  CCO_FCall,   0,"fiBIntBIPower",  "fiBINT_BI_POWER"},
 {FOAM_BVal_BIntPowerMod, CCO_FCall,   0,"fiBIntPowerMod", "fiBINT_POWER_MOD"},
 {FOAM_BVal_BIntLength,   CCO_FCall,   0,"fiBIntLength",   "fiBINT_LENGTH"},
 {FOAM_BVal_BIntShiftUp,  CCO_FCall,   0,"fiBIntShiftUp",  "fiBINT_SHIFT_UP"},
 {FOAM_BVal_BIntShiftDn,  CCO_FCall,   0,"fiBIntShiftDn",  "fiBINT_SHIFT_DN"},
 {FOAM_BVal_BIntShiftRem, CCO_FCall,   0,"fiBIntShiftRem", 0},
 {FOAM_BVal_BIntBit,      CCO_FCall,   0,"fiBIntBit",      "fiBINT_BIT"},

 {FOAM_BVal_PtrNil,       CCO_Id,      1,"fiPtrNil",        0},
 {FOAM_BVal_PtrIsNil,     CCO_FCall,   0,"fiPtrIsNil",      "fiPTR_IS_NIL"},
 {FOAM_BVal_PtrMagicEQ,   CCO_FCall,   0,"fiPtrMagicEQ",    "fiPTR_MAGIC_EQ"},
 {FOAM_BVal_PtrEQ,        CCO_EQ,      0,0,                 0},
 {FOAM_BVal_PtrNE,        CCO_NE,      0,0,                 0},

 {FOAM_BVal_FormatSFlo,   CCO_FCall,   0,"fiFormatSFlo",    "fiFORMAT_SFLO"},
 {FOAM_BVal_FormatDFlo,   CCO_FCall,   0,"fiFormatDFlo",    "fiFORMAT_DFLO"},
 {FOAM_BVal_FormatSInt,   CCO_FCall,   0,"fiFormatSInt",    "fiFORMAT_SINT"},
 {FOAM_BVal_FormatBInt,   CCO_FCall,   0,"fiFormatBInt",    "fiFORMAT_BINT"},

 {FOAM_BVal_ScanSFlo,     CCO_FCall,   0,"fiScanSFlo",      0},
 {FOAM_BVal_ScanDFlo,     CCO_FCall,   0,"fiScanDFlo",      0},
 {FOAM_BVal_ScanSInt,     CCO_FCall,   0,"fiScanSInt",      0},
 {FOAM_BVal_ScanBInt,     CCO_FCall,   0,"fiScanBInt",      0},

 {FOAM_BVal_SFloToDFlo,   CCO_FCall,   0,"fiSFloToDFlo",    "fiSFLO_TO_DFLO"},
 {FOAM_BVal_DFloToSFlo,   CCO_FCall,   0,"fiDFloToSFlo",    "fiDFLO_TO_SFLO"},
 {FOAM_BVal_ByteToSInt,   CCO_FCall,   0,"fiByteToSInt",    "fiBYTE_TO_SINT"},
 {FOAM_BVal_SIntToByte,   CCO_FCall,   0,"fiSIntToByte",    "fiSINT_TO_BYTE"},
 {FOAM_BVal_HIntToSInt,   CCO_FCall,   0,"fiHIntToSInt",    "fiHINT_TO_SINT"},
 {FOAM_BVal_SIntToHInt,   CCO_FCall,   0,"fiSIntToHInt",    "fiSINT_TO_HINT"},
 {FOAM_BVal_SIntToBInt,   CCO_FCall,   0,"fiSIntToBInt",    "fiSINT_TO_BINT"},
 {FOAM_BVal_BIntToSInt,   CCO_FCall,   0,"fiBIntToSInt",    "fiBINT_TO_SINT"},
 {FOAM_BVal_SIntToSFlo,   CCO_Cast,    0,gcFiSFlo,          "fiSINT_TO_SFLO"},
 {FOAM_BVal_SIntToDFlo,   CCO_Cast,    0,gcFiDFlo,          "fiSINT_TO_DFLO"},
 {FOAM_BVal_BIntToSFlo,   CCO_FCall,   0,"fiBIntToSFlo",    "fiBINT_TO_SFLO"},
 {FOAM_BVal_BIntToDFlo,   CCO_FCall,   0,"fiBIntToDFlo",    "fiBINT_TO_DFLO"},
 {FOAM_BVal_PtrToSInt,    CCO_FCall,   0,"fiPtrToSInt",     "fiPTR_TO_SINT"},
 {FOAM_BVal_SIntToPtr,    CCO_FCall,   0,"fiSIntToPtr",     "fiSINT_TO_PTR"},

 {FOAM_BVal_ArrToSFlo,    CCO_FCall,   0,"fiArrToSFlo",     "fiARR_TO_SFLO"},
 {FOAM_BVal_ArrToDFlo,    CCO_FCall,   0,"fiArrToDFlo",     "fiARR_TO_DFLO"},
 {FOAM_BVal_ArrToSInt,    CCO_FCall,   0,"fiArrToSInt",     "fiARR_TO_SINT"},
 {FOAM_BVal_ArrToBInt,    CCO_FCall,   0,"fiArrToBInt",     "fiARR_TO_BINT"},

 {FOAM_BVal_PlatformRTE,  CCO_FCall,   0,"fiPlatformRTE",   0},
 {FOAM_BVal_PlatformOS,   CCO_FCall,   0,"fiPlatformOS",    0},

 {FOAM_BVal_Halt,         CCO_FCall,   0,"fiHalt",          0},

 {FOAM_BVal_RoundZero,    CCO_FCall,   0,"fiRoundZero",    "FI_ROUND_ZERO"},
 {FOAM_BVal_RoundNearest, CCO_FCall,   0,"fiRoundNearest", "FI_ROUND_NEAREST"},
 {FOAM_BVal_RoundUp,      CCO_FCall,   0,"fiRoundUp",      "FI_ROUND_UP"},
 {FOAM_BVal_RoundDown,    CCO_FCall,   0,"fiRoundDown",    "FI_ROUND_DOWN"},
 {FOAM_BVal_RoundDontCare,CCO_FCall,   0,"fiRoundDontCare","FI_ROUND_DONT_CARE"},

 {FOAM_BVal_SFloTruncate,  CCO_FCall,  0,"fiSFloTruncate",  0},
 {FOAM_BVal_SFloFraction,  CCO_FCall,  0,"fiSFloFraction",  0},
 {FOAM_BVal_SFloRound,     CCO_FCall,  0,"fiSFloRound",     0},

 {FOAM_BVal_DFloTruncate,  CCO_FCall,  0,"fiDFloTruncate",  0},
 {FOAM_BVal_DFloFraction,  CCO_FCall,  0,"fiDFloFraction",  0},
 {FOAM_BVal_DFloRound,     CCO_FCall,  0,"fiDFloRound",     0},

 {FOAM_BVal_StoForceGC,    CCO_FCall,  0,"fiStoForceGC",    0},	
 {FOAM_BVal_StoInHeap,     CCO_FCall,  0,"fiStoInHeap",     0},
 {FOAM_BVal_StoIsWritable, CCO_FCall,  0,"fiStoIsWritable", 0},
 {FOAM_BVal_StoMarkObject, CCO_FCall,  0,"fiStoMarkObject", 0},
 {FOAM_BVal_StoRecode,     CCO_FCall,  0,"fiStoRecode",     0},
 {FOAM_BVal_StoNewObject,  CCO_FCall,  0,"fiStoNewObject",  0},
 {FOAM_BVal_StoATracer,    CCO_FCall,  0,"fiStoATracer",    0},
 {FOAM_BVal_StoCTracer,    CCO_FCall,  0,"fiStoCTracer",    0},
 {FOAM_BVal_StoShow,       CCO_FCall,  0,"fiStoShow",       0},
 {FOAM_BVal_StoShowArgs,   CCO_FCall,  0,"fiStoShowArgs",   0},

 {FOAM_BVal_TypeInt8,      CCO_FCall,  0,"fiTypeInt8",      0},
 {FOAM_BVal_TypeInt16,     CCO_FCall,  0,"fiTypeInt16",     0},
 {FOAM_BVal_TypeInt32,     CCO_FCall,  0,"fiTypeInt32",     0},
 {FOAM_BVal_TypeInt64,     CCO_FCall,  0,"fiTypeInt64",     0},
 {FOAM_BVal_TypeInt128,    CCO_FCall,  0,"fiTypeInt128",    0},

 {FOAM_BVal_TypeNil,       CCO_FCall,  0,"fiTypeNil",       0},
 {FOAM_BVal_TypeChar,      CCO_FCall,  0,"fiTypeChar",      0},
 {FOAM_BVal_TypeBool,      CCO_FCall,  0,"fiTypeBool",      0},
 {FOAM_BVal_TypeByte,      CCO_FCall,  0,"fiTypeByte",      0},
 {FOAM_BVal_TypeHInt,      CCO_FCall,  0,"fiTypeHInt",      0},
 {FOAM_BVal_TypeSInt,      CCO_FCall,  0,"fiTypeSInt",      0},
 {FOAM_BVal_TypeBInt,      CCO_FCall,  0,"fiTypeBInt",      0},
 {FOAM_BVal_TypeSFlo,      CCO_FCall,  0,"fiTypeSFlo",      0},
 {FOAM_BVal_TypeDFlo,      CCO_FCall,  0,"fiTypeDFlo",      0},
 {FOAM_BVal_TypeWord,      CCO_FCall,  0,"fiTypeWord",      0},
 {FOAM_BVal_TypeClos,      CCO_FCall,  0,"fiTypeClos",      0},
 {FOAM_BVal_TypePtr,       CCO_FCall,  0,"fiTypePtr",       0},
 {FOAM_BVal_TypeRec,       CCO_FCall,  0,"fiTypeRec",       0},
 {FOAM_BVal_TypeArr,       CCO_FCall,  0,"fiTypeArr",       0},
 {FOAM_BVal_TypeTR,        CCO_FCall,  0,"fiTypeTR",        0},
 {FOAM_BVal_RawRepSize,    CCO_FCall,  0,"fiRawRepSize",    0},
 {FOAM_BVal_SizeOfInt8,    CCO_FCall,  0,"fiSizeOfInt8",    0},
 {FOAM_BVal_SizeOfInt16,   CCO_FCall,  0,"fiSizeOfInt16",   0},
 {FOAM_BVal_SizeOfInt32,   CCO_FCall,  0,"fiSizeOfInt32",   0},
 {FOAM_BVal_SizeOfInt64,   CCO_FCall,  0,"fiSizeOfInt64",   0},
 {FOAM_BVal_SizeOfInt128,  CCO_FCall,  0,"fiSizeOfInt128",  0},

 {FOAM_BVal_SizeOfNil,     CCO_FCall,  0,"fiSizeOfNil",     0},
 {FOAM_BVal_SizeOfChar,    CCO_FCall,  0,"fiSizeOfChar",    0},
 {FOAM_BVal_SizeOfBool,    CCO_FCall,  0,"fiSizeOfBool",    0},
 {FOAM_BVal_SizeOfByte,    CCO_FCall,  0,"fiSizeOfByte",    0},
 {FOAM_BVal_SizeOfHInt,    CCO_FCall,  0,"fiSizeOfHInt",    0},
 {FOAM_BVal_SizeOfSInt,    CCO_FCall,  0,"fiSizeOfSInt",    0},
 {FOAM_BVal_SizeOfBInt,    CCO_FCall,  0,"fiSizeOfBInt",    0},
 {FOAM_BVal_SizeOfSFlo,    CCO_FCall,  0,"fiSizeOfSFlo",    0},
 {FOAM_BVal_SizeOfDFlo,    CCO_FCall,  0,"fiSizeOfDFlo",    0},
 {FOAM_BVal_SizeOfWord,    CCO_FCall,  0,"fiSizeOfWord",    0},
 {FOAM_BVal_SizeOfClos,    CCO_FCall,  0,"fiSizeOfClos",    0},
 {FOAM_BVal_SizeOfPtr,     CCO_FCall,  0,"fiSizeOfPtr",     0},
 {FOAM_BVal_SizeOfRec,     CCO_FCall,  0,"fiSizeOfRec",     0},
 {FOAM_BVal_SizeOfArr,     CCO_FCall,  0,"fiSizeOfArr",     0},
 {FOAM_BVal_SizeOfTR,      CCO_FCall,  0,"fiSizeOfTR",      0},

 {FOAM_BVal_ListNil,       CCO_FCall,  0,"fiListNil",       "fiLIST_NIL"},
 {FOAM_BVal_ListEmptyP,    CCO_FCall,  0,"fiListEmptyP",    "fiLIST_EMPTYP"},
 {FOAM_BVal_ListHead,      CCO_FCall,  0,"fiListHead",      "fiLIST_HEAD"},
 {FOAM_BVal_ListTail,      CCO_FCall,  0,"fiListTail",      "fiLIST_TAIL"},
 {FOAM_BVal_ListCons,      CCO_FCall,  0,"fiListCons",      0},
 {FOAM_BVal_NewExportTable, CCO_FCall, 0, "fiNewExportTable", 0},
 {FOAM_BVal_AddToExportTable, CCO_FCall, 0, "fiAddToExportTable", 0},
 {FOAM_BVal_FreeExportTable, CCO_FCall, 0, "fiFreeExportTable", 0},
#if EDIT_1_0_n1_AB
 /* This BVal must NEVER be seen by genc ... */
 {FOAM_BVal_ssaPhi, CCO_FCall, 0, "fiNonExistentFunction", 0},
#endif
};


/*****************************************************************************
 *
 * :: Table of valid identifiers for special characters
 *
 ****************************************************************************/

struct ccSpecCharId_info ccSpecCharIdTable[] = {
 {'!',	"_BANG_"},
 {'\"',	"_QUOTE_"},
 {'#',	"_SHARP_"},
 {'$',	"_DOLLR_"},
 {'%',	"_PCENT_"},
 {'&',	"_AMPER_"},
 {'\'',	"_APOS_"},
 {'(',	"_OPAREN_"},
 {')',	"_CPAREN_"},
 {'*',	"_STAR_"},
 {'+',	"_PLUS_"},
 {',',	"_COMMA_"},
 {'-',	"_MINUS_"},
 {'.',	"_DOT_"},
 {'/',	"_SLASH_"},
 {':',	"_COLON_"},
 {';',	"_SEMI_"},
 {'<',	"_LT_"},
 {'=',	"_EQ_"},
 {'>',	"_GT_"},
 {'?',	"_QMARK_"},
 {'@',	"_AT_"},
 {'[',	"_OBRACK_"},
 {'\\',	"_BSLSH_"},
 {']',	"_CBRACK_"},
 {'^',	"_HAT_"},
 {'_',	"__"},
 {'`',	"_GRAVE_"},
 {'{',	"_OBRACE_"},
 {'|',	"_BAR_"},
 {'}',	"_CBRACE_"},
 {'~',	"_TILDE_"},
 {0,	0}
};


