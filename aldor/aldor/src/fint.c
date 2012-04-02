/*****************************************************************************
 *
 * fint.c: foam interpreter.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/


#include "axlobs.h"
#include "syscmd.h"
#include "foam_c.h"
#include "fint.h"
#include "output.h"
#include "cmdline.h"



/* #define FiBool		FiWord  */

int	fintMode = FINT_DONT;


/* Exception handling: format of the try-block. */
static int fintCurrentFormat = emptyFormatSlot;


/* Data Object Representation */

typedef Length		FiProgPos;     /* position in the buffer */

struct fmt {
	int			type;
	String          	id;
	int			protocol;
	int			format;
	int			offset;    /* used for lex vars */
};

typedef struct fmt 	* Fmt;
typedef struct fintUnit * FintUnit;

struct progInfo {
	FintUnit	unit;			/* unit in which is contained*/
	String		name;			/* name of the const that defines prog */
	FiProgPos	fiProgPos;		/* pos. of Seq command */
	int		size;			/* size */
	FiProgPos	* labels;		/* labels */
	int		nLabels;
	int		labelFmt;
	AInt		retType;		/* return type */
	int		mValFmt;
	UByte		bMask;
	Fmt		fmtLoc;			/* locals */
	int		locsCount;
	Fmt		fmtPar;			/* parameters */
	int		parsCount;
	UByte		* dfluid;		/* DFluid     */
	UByte		dfluidsCount;
	UByte		* denv;			/* DEnv	      */
	UByte		denvsCount;
	
	FiWord		_progInfo;		/* $$ REMOVE when interfaced w/h C */
};

typedef struct progInfo	* ProgInfo;
typedef union  dataObj  * DataObj;
typedef struct mFmt	* MFmt;
typedef struct shDataObj* ShDataObj;

union  dataObj {
	FiNil		_fiNil; /*  (fiNil already defined!) */
	FiWord		fiWord;
	FiArb		fiArb;
	FiPtr		fiPtr;
	FiBool		fiBool;
	FiByte		fiByte;
	FiHInt		fiHInt;
	FiSInt		fiSInt;
	FiChar		fiChar;
	FiArr		fiArr;
	FiRec		fiRec;
	FiTR		fiTR;
	FiProgPos	fiProgPos;
	FiClos		fiClos;
	FiEnv		fiEnv;
	FiBInt		fiBInt;
	FiSFlo		fiSFlo;
	FiDFlo		fiDFlo;
	FiProg		fiProg;
	/* -------- for internal use: -------------- */ 
	FiFluid		fiFluid;
	DataObj		ptr;
	FiProgPos	* labels;
	ProgInfo	progInfo;
	FintUnit	fiUnit;
	MFmt		mFmt;

};

struct mFmt {
	int 	fmt;
	DataObj	values;

};		/* for multiple values */

struct shDataObj {
	int		refCounter;
	union dataObj	dataObj;
};

# define fintDataSize	sizeof(union dataObj)

typedef DataObj		* Ref;
typedef AInt		dataType;
typedef dataType    	* DataType;
typedef AInt		FintLabel;

/****************************************************************************
 *
 ***************************************************************************/


struct lexLevel {
	Fmt		fmtLex;
	int		fmtLexsCount;
	int		fmtSize;

};

typedef struct lexLevel	* LexLevels;
typedef struct lexLevel lexLevel;

struct fintUnit {
	unsigned	unitId;		/* Unique for every loaded unit */
	UByte		* tape;
	Buffer		buf;
	String		name;

	Fmt		fmtGlobs;
	ShDataObj	* globValues;
	int		globsCount;		

	Fmt		fmtConsts;
	DataObj		constValues;
	int		constsCount;

	Fmt		fmtFluids;
	int		fluidsCount;

	LexLevels	lexLevels;
	int		lexLevelsCount;

};

typedef struct fintUnit	fintUnit;

/**************************************************************************
 * 
 * Interface for FintUnit
 *
 *************************************************************************/


# define fintUnitId(u)			((u)->unitId)
# define fintUnitTape(u)		((u)->tape)
# define fintUnitBuffer(u)		((u)->buf)
# define fintUnitName(u)		((u)->name)

# define fintUnitGlobs(u)		((u)->fmtGlobs)
# define fintUnitConsts(u)		((u)->fmtConsts)
# define fintUnitFluids(u)		((u)->fmtFluids)
# define fintUnitLexLevels(u)		((u)->lexLevels)

# define fintUnitGlobsCount(u)		((u)->globsCount)
# define fintUnitConstsCount(u)		((u)->constsCount)
# define fintUnitFluidsCount(u)		((u)->fluidsCount)
# define fintUnitLexLevelsCount(u)	((u)->lexLevelsCount)
# define fintUnitLexsCount(u,lev)	(((u)->lexLevels[(lev)]).fmtLexsCount)
# define fintUnitFmtSize(u,lev)		(((u)->lexLevels[(lev)]).fmtSize)

# define fintUnitGlobValues(u)		((u)->globValues)
# define fintUnitConstValues(u)		((u)->constValues)

/**************************************************************************
 * 
 * Basic operation for Format
 *
 *************************************************************************/

# define fmtType(f)		((f)->type)
# define fmtId(f)		((f)->id)
# define fmtProtocol(f)		((f)->protocol)
# define fmtFormat(f)		((f)->format)
# define fmtOffset(f)		((f)->offset)

/**************************************************************************
 * 
 * Basic operation for globals
 *
 *************************************************************************/

# define globType(n)		((fintUnitGlobs(unit)[(n)]).type)
# define globId(n)		((fintUnitGlobs(unit)[(n)]).id)
# define globProtocol(n)	((fintUnitGlobs(unit)[(n)]).protocol)
# define globValue(n)		(((unit)->globValues[(n)])->dataObj)

/**************************************************************************
 * 
 * Basic operation for constants
 *
 *************************************************************************/

# define constType(n)		((fintUnitConsts(unit)[(n)]).type)
# define constId(n)		((fintUnitConsts(unit)[(n)]).id)
# define constValue(n)		((unit)->constValues[(n)])

/**************************************************************************
 * 
 * Basic operation for locals
 *
 *************************************************************************/

# define locType(n)		(progInfoFmtLoc(prog)[(n)].type)
# define locId(n)		(progInfoFmtLoc(prog)[(n)].id)
# define locValue(n)		(locValues[(n)])

/**************************************************************************
 * 
 * Basic operation for fluids
 *
 *************************************************************************/

# define fluidType(n)		(fintUnitFluids(unit)[(n)].type)
# define fluidId(n)		(fintUnitFluids(unit)[(n)].id)
# define fluidValue(n)		(fluidValues[(n)].fiFluid)

/**************************************************************************
 * 
 * Basic operation for parameters
 *
 *************************************************************************/

# define parType(n)		(progInfoFmtPar(prog)[(n)].type)
# define parId(n)		(progInfoFmtPar(prog)[(n)].id)
# define parValue(n)		(bp[(n)+PAR_OFFSET])

/**************************************************************************
 * 
 * Basic operation for lexicals
 *
 *************************************************************************/

# define lexType(l,n)	      (fintUnitLexLevels(unit)[(prog->denv[(l)])].fmtLex[(n)].type)
# define fmtType0(f,n)	      (fintUnitLexLevels(unit)[(f)].fmtLex[(n)].type)
# define lexFormat(f,n)	      (fintUnitLexLevels(unit)[(f)].fmtLex[(n)].format)

/**************************************************************************
 * 
 * Basic operation for progInfo
 *
 *************************************************************************/

# define progInfoUnit(p)	((p)->unit)
# define progInfoName(p)	((p)->name)
# define progInfoTape(p)	((p)->tape)
# define progInfoSeq(p)		((p)->fiProgPos)
# define progInfoSize(p)	((p)->size)
# define progInfoLabels(p)	((p)->labels)
# define progInfoNLabels(p)	((p)->nLabels)
# define progInfoLabelFmt(p)	((p)->labelFmt)
# define progInfoFmtLoc(p)	((p)->fmtLoc)
# define progInfoRetType(p)	((p)->retType)
# define progInfoMValFmt(p)	((p)->mValFmt)
# define progInfoBMask(p)	((p)->bMask)
# define progInfoLocsCount(p)	((p)->locsCount)
# define progInfoFmtPar(p)	((p)->fmtPar)
# define progInfoParsCount(p)	((p)->parsCount)
# define progInfoDFluid(p)	((p)->dfluid)
# define progInfoDFluidsCount(p) ((p)->dfluidsCount)
# define progInfoDEnv(p)	((p)->denv)
# define progInfoDEnvsCount(p)	((p)->denvsCount)

/* -------------------------------------------------------------- */

/* type is FOAM_Bool, FOAM_Char, etc.
 * ref is a pointer to a dataObj union
 * expr is a dataObj union
 */
# define fintSet(type, ref, expr) { \
   assert(ref != NULL); \
   switch ((int)type) {  \
     case FOAM_Char: *(FiChar *)(ref) = (expr).fiChar; break;   \
     case FOAM_Bool: (ref)->fiBool = (expr).fiBool; break;   \
     case FOAM_Byte: (ref)->fiByte = (expr).fiByte; break;   \
     case FOAM_HInt: (ref)->fiHInt = (expr).fiHInt; break;   \
     case FOAM_SInt: (ref)->fiSInt = (expr).fiSInt; break;   \
     case FOAM_SFlo: (ref)->fiSFlo = (expr).fiSFlo; break;   \
     case FOAM_DFlo: (ref)->fiDFlo = (expr).fiDFlo; break;   \
     case FOAM_Word: (ref)->fiWord = (expr).fiWord; break;   \
     case FOAM_Arb:  (ref)->fiArb  = (expr).fiArb;  break;   \
     case FOAM_Ptr:  (ref)->fiPtr  = (expr).fiPtr;  break;   \
     case FOAM_Rec:  (ref)->fiRec  = (expr).fiRec;  break;   \
     case FOAM_Arr:  (ref)->fiArr  = (expr).fiArr;  break;   \
     case FOAM_TR:   (ref)->fiTR   = (expr).fiTR;  break;   \
     case FOAM_Prog: (ref)->fiProgPos=(expr).fiProgPos; break;\
     case FOAM_Clos: (ref)->fiClos = (expr).fiClos; break;\
     case FOAM_Env: (ref)->fiEnv = (expr).fiEnv; break;\
     case FOAM_NOp: fintSetMFmt((ref), &(expr)); break; \
     case FOAM_Nil:  (ref)->_fiNil = (expr)._fiNil; break;\
     case FOAM_BInt:  (ref)->fiBInt = (Ptr) (bintCopy((BInt) (expr).fiBInt)); break;\
     default: fintWhere(int0);bug("fintSet: type %d unimplemented.", type); \
     }						\
  }	

# define fintASetElem(type, ref, n, expr) { \
   switch ((int)type) { \
    case FOAM_Char: ((FiChar *)((ref)->fiArr))[(n)] = (expr).fiChar; break; \
    case FOAM_Bool: ((FiBool *)((ref)->fiArr))[(n)] = (expr).fiBool; break; \
    case FOAM_Byte: ((FiByte *)((ref)->fiArr))[(n)] = (expr).fiByte; break; \
    case FOAM_HInt: ((FiHInt *)((ref)->fiArr))[(n)] = (expr).fiHInt; break; \
    case FOAM_SInt: ((FiSInt *)((ref)->fiArr))[(n)] = (expr).fiSInt; break; \
    case FOAM_SFlo: ((FiSFlo *)((ref)->fiArr))[(n)] = (expr).fiSFlo; break; \
    case FOAM_DFlo: ((FiDFlo *)((ref)->fiArr))[(n)] = (expr).fiDFlo; break; \
    case FOAM_Word: ((FiWord *)((ref)->fiArr))[(n)] = (expr).fiWord; break; \
    default: fintWhere(int0);bug("fintASetElem: type %d unimplemented.", type); \
    } \
}

# define fintAGetElem(type, pdata, ref, n) {   \
   switch ((int)type) {   \
   case FOAM_Char: (pdata)->fiChar = ((FiChar *)((ref)->fiArr))[(n)]; break;  \
    case FOAM_Bool: (pdata)->fiBool = ((FiBool *)((ref)->fiArr))[(n)]; break; \
    case FOAM_Byte: (pdata)->fiByte = ((FiByte *)((ref)->fiArr))[(n)]; break; \
    case FOAM_HInt: (pdata)->fiHInt = ((FiHInt *)((ref)->fiArr))[(n)]; break; \
    case FOAM_SInt: (pdata)->fiSInt = ((FiSInt *)((ref)->fiArr))[(n)]; break; \
    case FOAM_SFlo: (pdata)->fiSFlo = ((FiSFlo *)((ref)->fiArr))[(n)]; break; \
    case FOAM_DFlo: (pdata)->fiDFlo = ((FiDFlo *)((ref)->fiArr))[(n)]; break; \
    case FOAM_Word: (pdata)->fiWord = ((FiWord *)((ref)->fiArr))[(n)]; break; \
    default: fintWhere(int0);bug("fintAGetElem: type %d unimplemented.", type); \
    } \
}

# define fintAGetElemRef(type, pdata, ref, n) {   \
   switch ((int)type) {   \
    case FOAM_Char:(pdata)=(DataObj)(((FiChar *)((ref)->fiArr)) + (n)); break;\
    case FOAM_Bool:(pdata)=(DataObj)(((FiBool *)((ref)->fiArr)) + (n)); break;\
    case FOAM_Byte:(pdata)=(DataObj)(((FiByte *)((ref)->fiArr)) + (n)); break;\
    case FOAM_HInt:(pdata)=(DataObj)(((FiHInt *)((ref)->fiArr)) + (n)); break;\
    case FOAM_SInt:(pdata)=(DataObj)(((FiSInt *)((ref)->fiArr)) + (n)); break;\
    case FOAM_SFlo:(pdata)=(DataObj)(((FiSFlo *)((ref)->fiArr)) + (n)); break;\
    case FOAM_DFlo:(pdata)=(DataObj)(((FiDFlo *)((ref)->fiArr)) + (n)); break;\
    case FOAM_Word:(pdata)=(DataObj)(((FiWord *)((ref)->fiArr)) + (n)); break;\
    default: fintWhere(int0);bug("fintAGetElemRef: type %d unimplemented.", type); \
    } \
}

# define fintGetTypeSize(x, type)   { \
  switch ((int)type) { \
	 case FOAM_Char: (x) = sizeof(FiChar); break; \
	 case FOAM_Bool: (x) = sizeof(FiBool); break; \
	 case FOAM_Byte: (x) = sizeof(FiByte); break; \
	 case FOAM_HInt: (x) = sizeof(FiHInt); break; \
	 case FOAM_SInt: (x) = sizeof(FiSInt); break; \
	 case FOAM_BInt: (x) = sizeof(FiBInt); break; \
	 case FOAM_SFlo: (x) = sizeof(FiSFlo); break; \
	 case FOAM_DFlo: (x) = sizeof(FiDFlo); break; \
	 case FOAM_Arr: (x) = sizeof(FiArr); break; \
	 case FOAM_Rec: (x) = sizeof(FiRec); break; \
	 case FOAM_TR: (x) = sizeof(FiTR); break; \
	 case FOAM_Env: (x) = sizeof(FiEnv); break; \
	 case FOAM_Prog: (x) = sizeof(FiProg); break; \
	 case FOAM_Clos: (x) = sizeof(FiClos); break; \
	 case FOAM_Ptr: (x) = sizeof(FiPtr); break; \
	 case FOAM_Word: (x) = sizeof(FiWord); break; \
	 case FOAM_Arb: (x) = sizeof(FiArb); break; \
	 case FOAM_Nil: (x) = sizeof(FiNil); break; \
	 default: fintWhere(int0);bug("fintGetTypeSize: type %d unimplemented.", type); \
  }}

/* $$!! ----- From foam.c; This should be moved in foam.h ----- */
#define	 STD_FORMS	2	/* Number of standard formats.
				 * I.e. all 4 or 1 bytes.
				 * Cannot be changed */

#define FFO_ORIGIN	(FOAM_VECTOR_START)
#define FFO_SPAN	(FOAM_LIMIT - FFO_ORIGIN)

/**************************************************************************
 * 
 * Tape management
 *
 *************************************************************************/

#define fintGetByte(b)		((b) = tape[ip++])
#define fintUngetByte()		(ip--)
#define fintGetHInt(i)		{			\
                       String _s = fintGetn(HINT_BYTES); \
                       (i) = UNBYTE2(_s[0],_s[1]);	\
}

#define fintGetSInt(i)		{	\
                       String _s = fintGetn(SINT_BYTES); \
                       (i) = (int) UNBYTE4(_s[0],_s[1],_s[2],_s[3]);	\
}

#define fintGetChars(s, cc)			\
	strncpy(s, fintGetn(cc), cc)

#define FOAM_NARY		(-1)
#define FOAM_FORMAT_GET(tag)       ((tag)<FFO_ORIGIN? 0:FOAM_FORMAT_GET_X(tag))
#define FOAM_FORMAT_GET_X(tag)	    (((tag)-FFO_ORIGIN)/FFO_SPAN)
#define FOAM_FORMAT_REMOVE(tag,fmt) ((tag) - (fmt)*FFO_SPAN)
#define FOAM_FORMAT_FOR(n) \
	((long)(n) <= MAX_BYTE ? 1 : 0)

#define fintGetInt(format, i)  { \
	switch (format) {				\
	case 0:	 fintGetSInt(i); break;		\
	case 1:	 fintGetByte(i); break;		\
	default: (i) = (format) - STD_FORMS; break;	\
	}						\
}

# define fintGetTagFmt(tag, fmt)   {   fintGetByte(tag);  \
				       fmt = FOAM_FORMAT_GET(tag); \
				       tag = FOAM_FORMAT_REMOVE((tag),(fmt)); \
				  	}

# define fintGetTagFmtArgc(tag,fmt0,argc0)     {  \
	  fintGetTagFmt((tag),(fmt0))    	\
          if (foamInfo(tag).argc == FOAM_NARY) \
	  	fintGetInt(fmt0, argc0) \
	  else				       \
	  	(argc0) = foamInfo(tag).argc; }


/**************************************************************************
 * 
 * Stack management
 * 
 * * Main globals for the stack management are:
 *   - bp: points to the base of the current stack frame
 *   - sp: points to the top of the current stack frame
 *   - stack: points to the current stack (see Stack Chaining)
 *
 * * A procedure stack frame consists in:
 *   - header (starting at bp, see below)
 *   - parameters, if any (starting at bp + PAR_OFFSET)
 *   - locals, if any (locValues points to (Loc 0))
 *   - fluids, if any (fluidValues points to (Fluid 0))
 *
 * * Stack Chaining
 * In order to provide a virtually infinite stack, the interpreter stack is
 * organized as a list of stack. Every element has size STACK_SIZE. The
 * starting stack is <headStack>. If a stackFrameAlloc or stackAlloc operation
 * needs X bytes and such amount is not available in the current stack, then
 * a new stack of size STACK_SIZE is dynamically allocated and is chained to
 * the previous stack.
 *************************************************************************/

# define	PAR_OFFSET		9
/* stack[bp + PAR_OFFSET] is the first parameter */


# define 	STACK_SIZE		3000

/* Description of a procedure stack frame header: */

# define	stackFrameBp(b)		((b)[0].ptr)
# define	stackFrameIp(b)		((b)[1].fiProgPos)
# define	stackFrameLabels(b)	((b)[2].labels)
# define	stackFrameLabelFmt(b)	((b)[3].fiChar)
# define	stackFrameLocals(b)	((b)[4].ptr)
# define	stackFrameLexEnv(b)	((b)[5].fiEnv)
# define	stackFrameProg(b)	((b)[6].progInfo)
# define	stackFrameUnit(b)	((b)[7].unit)
# define	stackFrameFluids(b)	((b)[8].ptr)

# define	stackAlloc(ptr0,num)	{ \
	 if (sp + num >= stack + STACK_SIZE -10)  stackChain(num); \
	 (ptr0) = sp; sp += (num); \
         }

# define	stackFrameAlloc(nParam)	{ \
	 if (sp + PAR_OFFSET + nParam >= stack + STACK_SIZE -10)  stackChain(nParam+PAR_OFFSET); \
	 sp->ptr = bp; \
	 bp = sp; \
 	 sp += 2; \
 	 (sp++)->labels = labels;   \
 	 (sp++)->fiChar = (char) labelFmt; \
         (sp++)->ptr = locValues;\
	 (sp++)->fiEnv = lexEnv; \
	 (sp++)->progInfo = prog; \
	 (sp++)->fiUnit = unit; \
	 (sp++)->ptr = fluidValues; \
         sp += nParam + 1; }

# define	stackFrameFree()	{ \
          ip = bp[1].fiProgPos;  \
          labels = bp[2].labels;  \
          labelFmt = (int) bp[3].fiChar;  \
          locValues = bp[4].ptr; \
	  lexEnv = bp[5].fiEnv; \
	  prog = bp[6].progInfo; \
	  unit = bp[7].fiUnit; \
          fluidValues = bp[8].ptr; \
	  tape = fintUnitTape(unit); \
          if (bp < stack || bp >= stack + STACK_SIZE) { sp = stack[1].ptr; stack = stack[0].ptr; }\
	  else sp = bp; \
          bp = bp->ptr; \
	  lev0= (DataObj) lexEnv->level; }

/*
!! NO GOOD with stackChaining
# define	stackPush(type, data)	{fintSet((type),sp,(data)); sp++;}
# define	stackPop()		(sp--, *sp)
*/
/* NOTE: Usage for stackPop(): x = stackPop().fiSInt, etc. */


# define	fintEnvPush(lxEnv, lv, en) { \
	lxEnv = (FiEnv) fintAlloc(struct _FiEnv, 1); \
	lxEnv->level = (Ptr) lv; lxEnv->next = (en); \
	lxEnv->info = (FiWord) NULL; }

# define	fintClosMake(cl,en,pr)   {  \
           (cl) = (FiClos) fintAlloc(struct _FiClos,1); \
           (cl)->prog = (FiProg) (pr).fiProgPos;  \
           (cl)->env =  (en).fiEnv;}	

/*
 * This value is the number of stack frames printed before executing an
 * halt instr.
 */
#define FINT_BACKTRACE_CUTOFF		23

/**************************************************************************
 * 
 * Heap management
 *
 *************************************************************************/
# define fintAlloc(type,n)	((DataObj) stoAlloc(OB_Other, sizeof(type) * (n)))
# define fintFree(p)		stoFree((p))
# define fintFree0(p)		if (p) stoFree(p)


/**************************************************************************
 *************************************************************************/


#define		POS_LEX_FMT		4

#define		DECL_INIT_SIZE		15

#define fiTrue				((FiBool) 1)
#define fiFalse				((FiBool) 0)

/* This macro is used to cast FOAM_Bool to FOAM_Word. This is usefull because
 * in a foam program sometime an expr of type Word/SInt is used as an expr. of
 * type Bool.
 */
#define fintForceBoolToWord(expr, t) if (t == FOAM_Bool) expr.fiWord = (FiWord) expr.fiBool

/******************************************************************************
 *
 * :: Jmp Buffer (Halt Recovery)
 *
 *****************************************************************************/

static JmpBuf	fintJmpBuf; 

/*
 * Whenever a block of statements are protected by
 * fiBlock() or fiVoidBlock(), the fintCurrentFormat
 * global MUST be saved and restored. It must also
 * be saved when using fiProtect() but that never
 * seems to be used in the interpreter.
 */
#define fintBlock(ok, val, exn, expr)	\
{					\
	int __fmt = fintCurrentFormat;	\
	fiBlock(ok, val, exn, expr);	\
	fintCurrentFormat = __fmt;	\
}

#define fintVoidBlock(ok, exn, expr)	\
{					\
	int __fmt = fintCurrentFormat;	\
	fiVoidBlock(ok, exn, expr);	\
	fintCurrentFormat = __fmt;	\
}


/******************************************************************************
 *
 * :: Debug Stuff
 *
 *****************************************************************************/
#ifdef NDEBUG

#define fintDEBUG(x)
#define fintLinkDEBUG(x)		
#undef  assert
#define assert(x)
#define softAssert(x)
#define hardAssert(x)
#define fintStoDEBUG(x)

#define		fintTypedEval(pExpr,t)		fintEval(pExpr)
#define		fintGetTypedReference(pExpr,t)	fintGetReference(pExpr)
#else /* !NDEBUG */

local Bool	fintSoftAssertIsOn = false;
local long	instrBreak = -1;

#define fintDEBUG(x)			if (fintDebug) x
#define fintLinkDEBUG(x)		if (fintLinkDebug) x
#define fintStoDEBUG(x)			if (fintStoDebug ) x 
#define softAssert(x)			if (!fintSoftAssertIsOn || x) ; else fintSoftAssert(Enstring(x), __FILE__, __LINE__)
#define hardAssert(x)			if (x) ; else fintHardAssert(Enstring(x), __FILE__, __LINE__)

#define		fintTypedEval(pExpr,t) { \
	dataType type = fintEval(pExpr); \
	assert(type == t || type == FOAM_Word || type == FOAM_Nil); \
}

#define		fintGetTypedReference(pExpr,t) { \
	dataType type = fintGetReference(pExpr); \
	assert(type == t); \
}
#endif /* NDEBUG */

/**************************************************************************
 * Foreign function data structures
 *************************************************************************/


enum fintForeignTag {
	FINT_FOREIGN_fputs,
	FINT_FOREIGN_fputss,
	FINT_FOREIGN_fgetss,
	FINT_FOREIGN_isatty,
	FINT_FOREIGN_fileno,
	FINT_FOREIGN_stdinFile,
	FINT_FOREIGN_stdoutFile,
	FINT_FOREIGN_stderrFile,
	FINT_FOREIGN_formatSInt,
	FINT_FOREIGN_formatBInt,
	FINT_FOREIGN_formatSFloat,
	FINT_FOREIGN_formatDFloat,
	FINT_FOREIGN_fiSetDebugVar,
	FINT_FOREIGN_fiGetDebugVar,
	FINT_FOREIGN_fiSetDebugger,
	FINT_FOREIGN_fiGetDebugger,
	FINT_FOREIGN_fputc,
	FINT_FOREIGN_sqrt,
	FINT_FOREIGN_pow,
	FINT_FOREIGN_log,
	FINT_FOREIGN_log10,
	FINT_FOREIGN_exp,
	FINT_FOREIGN_sin,
	FINT_FOREIGN_cos,
	FINT_FOREIGN_tan,
	FINT_FOREIGN_sinh,
	FINT_FOREIGN_cosh,
	FINT_FOREIGN_tanh,
	FINT_FOREIGN_asin,
	FINT_FOREIGN_acos,
	FINT_FOREIGN_atan,
	FINT_FOREIGN_atan2,
	FINT_FOREIGN_fopen,
	FINT_FOREIGN_fclose,
	FINT_FOREIGN_fflush,
	FINT_FOREIGN_fgetc,
	FINT_FOREIGN_fseek,
	FINT_FOREIGN_ftell,
	FINT_FOREIGN_mainArgc,
	FINT_FOREIGN_mainArgv,
	FINT_FOREIGN_strLength,
	FINT_FOREIGN_fiStrHash,
	/* recent stuff 1.1.9d */
	FINT_FOREIGN_fiDoubleHexPrintToString,
	FINT_FOREIGN_fiInitialiseFpu,
	FINT_FOREIGN_fiIeeeGetRoundingMode,
	FINT_FOREIGN_fiIeeeSetRoundingMode,
	FINT_FOREIGN_fiIeeeGetEnabledExceptions,
	FINT_FOREIGN_fiIeeeSetEnabledExceptions,
	FINT_FOREIGN_fiIeeeGetExceptionStatus,
	FINT_FOREIGN_fiIeeeSetExceptionStatus,

	FINT_FOREIGN_fiDFloMantissa,
	FINT_FOREIGN_fiDFloExponent,
	FINT_FOREIGN_fiSFloMantissa,
	FINT_FOREIGN_fiSFloExponent,
	
	/* Operating System Interface */
	FINT_FOREIGN_osRun,
	FINT_FOREIGN_osRunConcurrent,
	FINT_FOREIGN_osRunQuoteArg,
	FINT_FOREIGN_osCpuTime,
	FINT_FOREIGN_osDate,
	FINT_FOREIGN_osGetEnv,
	FINT_FOREIGN_osPutEnv,
	FINT_FOREIGN_osPutEnvIsKept,
	FINT_FOREIGN_osIoRdMode,
	FINT_FOREIGN_osIoWrMode,
	FINT_FOREIGN_osIoApMode,
	FINT_FOREIGN_osIoRbMode,
	FINT_FOREIGN_osIoWbMode,
	FINT_FOREIGN_osIoAbMode,
	FINT_FOREIGN_osIoRubMode,
	FINT_FOREIGN_osIoWubMode,
	FINT_FOREIGN_osIoAubMode,
	FINT_FOREIGN_osObjectFileType,
	FINT_FOREIGN_osExecFileType,
	FINT_FOREIGN_osCurDirName,
	FINT_FOREIGN_osTmpDirName,
	FINT_FOREIGN_osFnameDirEqual,
	FINT_FOREIGN_osSubdir,
	FINT_FOREIGN_osSubdirLength,
	FINT_FOREIGN_osFnameNParts,
	FINT_FOREIGN_osFnameParse,
	FINT_FOREIGN_osFnameParseSize,
	FINT_FOREIGN_osFnameUnparse,
	FINT_FOREIGN_osFnameUnparseSize,
	FINT_FOREIGN_osFnameTempSeed,
	FINT_FOREIGN_osFnameTempDir,
	FINT_FOREIGN_osIsInteractive,
	FINT_FOREIGN_osFileRemove,
	FINT_FOREIGN_osFileRename,
	FINT_FOREIGN_osFileIsThere,
	FINT_FOREIGN_osFileHash,
	FINT_FOREIGN_osFileSize,
	FINT_FOREIGN_osDirIsThere,
	FINT_FOREIGN_osDirSwap,
	FINT_FOREIGN_osIncludePath,
	FINT_FOREIGN_osLibraryPath,
	FINT_FOREIGN_osExecutePath,
	FINT_FOREIGN_osPathLength,
	FINT_FOREIGN_osPathParse,

	FINT_FOREIGN_gcTimer,
	FINT_FOREIGN_fiRaiseException,
	FINT_FOREIGN_osAllocShow,
	FINT_FOREIGN_osAlloc,
	FINT_FOREIGN_osFree,
	FINT_FOREIGN_osMemMap,

#if EDIT_1_0_n1_06
	FINT_FOREIGN_fiNewExportTable,
	FINT_FOREIGN_fiAddToExportTable,
	FINT_FOREIGN_fiFreeExportTable,
#endif

	FINT_FOREIGN_END
};

typedef struct {
	String			string;
	enum fintForeignTag 	funct;
	Bool			isConst;
} fintForeign;

#define		DECL_FOREIGN(x)		{ Enstring(x), Abut(FINT_FOREIGN_,x), false }
#define		DECL_FOREIGN_CONST(x)	{ Enstring(x), Abut(FINT_FOREIGN_,x), true }


DECLARE_LIST(FintUnit);
CREATE_LIST(FintUnit);

extern TForm	typeInferAs(Stab, AbSyn, TForm);

/**************************************************************************
 * 
 * Globals
 *
 *************************************************************************/

local FintUnitList fintUnitList;
local Bool	fintInitialized = false;
local unsigned	unitId;		/* id number, unique for each unit. */
local FintUnit	unit;		/* current unit */
local FintUnit	mainUnit;

struct mainInfo {
	int globsSize;
	int constsSize;
	int fluidsSize;
	int lexLevelsSize;

} mainInfo;

local Buffer	evalBuf;

local UByte 	* tape;		/* interpreted string */
local FiProgPos ip;
local DataObj	headStack;	/* first stack allocated */
local DataObj	stack;		/* current stack */
local DataObj	sp;		/* First free cell on the top of the stack. */
local DataObj	bp;		/* Bottom of the current frame; refers to a
				 * dataObj containing the old bp. 
				 */
local DataObj	locValues; 	/* local values in the current stack frame */
local DataObj	fluidValues; 	/* fluid values in the current stack frame */
local FiEnv	lexEnv;		/* current lexical environment */
local DataObj	lev0;		/* lexEnv->level, used to speed up lex(0,n) */
local ProgInfo	prog;		/* progInfo for the current program */
local FiProgPos	* labels;
local DataObj   stackBase;
local int 	labelFmt;

fintForeign	fintForeignTable [] = {
	DECL_FOREIGN(fputs),
	DECL_FOREIGN(fputss),
	DECL_FOREIGN(fgetss),
	DECL_FOREIGN(isatty),
	DECL_FOREIGN(fileno),
	DECL_FOREIGN(stdinFile),
	DECL_FOREIGN(stdoutFile),
	DECL_FOREIGN(stderrFile),
	DECL_FOREIGN(formatSInt),
	DECL_FOREIGN(formatBInt),
	DECL_FOREIGN(formatSFloat),
	DECL_FOREIGN(formatDFloat),
	DECL_FOREIGN(fiGetDebugVar),
	DECL_FOREIGN(fiSetDebugVar),
	DECL_FOREIGN(fiGetDebugger),
	DECL_FOREIGN(fiSetDebugger),

	DECL_FOREIGN(fputc),
	DECL_FOREIGN(sqrt),
	DECL_FOREIGN(pow),
	DECL_FOREIGN(log),
	DECL_FOREIGN(log10),
	DECL_FOREIGN(exp),
	DECL_FOREIGN(sin),
	DECL_FOREIGN(cos),
	DECL_FOREIGN(tan),
	DECL_FOREIGN(sinh),
	DECL_FOREIGN(cosh),
	DECL_FOREIGN(tanh),
	DECL_FOREIGN(asin),
	DECL_FOREIGN(acos),
	DECL_FOREIGN(atan),
	DECL_FOREIGN(atan2),
	DECL_FOREIGN(fopen),
	DECL_FOREIGN(fclose),
	DECL_FOREIGN(fflush),
	DECL_FOREIGN(fgetc),
	DECL_FOREIGN(fseek),
	DECL_FOREIGN(ftell),
	DECL_FOREIGN(mainArgc),
	DECL_FOREIGN(mainArgv),
	DECL_FOREIGN(strLength),
	DECL_FOREIGN(fiStrHash),
	/* recent stuff 1.1.9d */
	DECL_FOREIGN(fiDoubleHexPrintToString),
	DECL_FOREIGN(fiInitialiseFpu),
	DECL_FOREIGN(fiIeeeGetRoundingMode),
	DECL_FOREIGN(fiIeeeSetRoundingMode),
	DECL_FOREIGN(fiIeeeGetEnabledExceptions),
	DECL_FOREIGN(fiIeeeSetEnabledExceptions),
	DECL_FOREIGN(fiIeeeGetExceptionStatus),
	DECL_FOREIGN(fiIeeeSetExceptionStatus),

	DECL_FOREIGN(fiDFloMantissa),
	DECL_FOREIGN(fiDFloExponent),
	DECL_FOREIGN(fiSFloMantissa),
	DECL_FOREIGN(fiSFloExponent),
	/* Operating System Interface */
	DECL_FOREIGN(osRun),
	DECL_FOREIGN(osRunConcurrent),
	DECL_FOREIGN(osRunQuoteArg),
	DECL_FOREIGN(osCpuTime),
	DECL_FOREIGN(osDate),
	DECL_FOREIGN(osGetEnv),
	DECL_FOREIGN(osPutEnv),
	DECL_FOREIGN(osPutEnvIsKept),
	DECL_FOREIGN_CONST(osIoRdMode),
	DECL_FOREIGN_CONST(osIoWrMode),
	DECL_FOREIGN_CONST(osIoApMode),
	DECL_FOREIGN_CONST(osIoRbMode),
	DECL_FOREIGN_CONST(osIoWbMode),
	DECL_FOREIGN_CONST(osIoAbMode),
	DECL_FOREIGN_CONST(osIoRubMode),
	DECL_FOREIGN_CONST(osIoWubMode),
	DECL_FOREIGN_CONST(osIoAubMode),
	DECL_FOREIGN_CONST(osObjectFileType),
	DECL_FOREIGN_CONST(osExecFileType),
	DECL_FOREIGN(osCurDirName),
	DECL_FOREIGN(osTmpDirName),
	DECL_FOREIGN(osFnameDirEqual),
	DECL_FOREIGN(osSubdir),
	DECL_FOREIGN(osSubdirLength),
	DECL_FOREIGN_CONST(osFnameNParts),
	DECL_FOREIGN(osFnameParse),
	DECL_FOREIGN(osFnameParseSize),
	DECL_FOREIGN(osFnameUnparse),
	DECL_FOREIGN(osFnameUnparseSize),
	DECL_FOREIGN(osFnameTempSeed),
	DECL_FOREIGN(osFnameTempDir),
	DECL_FOREIGN(osIsInteractive),
	DECL_FOREIGN(osFileRemove),
	DECL_FOREIGN(osFileRename),
	DECL_FOREIGN(osFileIsThere),
	DECL_FOREIGN(osFileHash),
	DECL_FOREIGN(osFileSize),
	DECL_FOREIGN(osDirIsThere),
	DECL_FOREIGN(osDirSwap),
	DECL_FOREIGN(osIncludePath),
	DECL_FOREIGN(osLibraryPath),
	DECL_FOREIGN(osExecutePath),
	DECL_FOREIGN(osPathLength),
	DECL_FOREIGN(osPathParse),
	/* Gc timer */
	DECL_FOREIGN(gcTimer),
	DECL_FOREIGN(fiRaiseException),
	DECL_FOREIGN(osAllocShow),
	DECL_FOREIGN(osAlloc),
	DECL_FOREIGN(osFree),
	DECL_FOREIGN(osMemMap),

#if EDIT_1_0_n1_06
	/* Runtime hashcode checks */
	DECL_FOREIGN(fiNewExportTable),
	DECL_FOREIGN(fiAddToExportTable),
	DECL_FOREIGN(fiFreeExportTable),
#endif

	{NULL, FINT_FOREIGN_END}	/* TERMINATE TABLE */
};

/**************************************************************************
 * Debugging globals
 *************************************************************************/

Bool	fintDebug = false;
Bool	fintLinkDebug = false;
Bool	fintStoDebug = false;
Bool	fintExceptDebug = false;

local long	instrCounter = 0;

/**************************************************************************
 * Globals shared with Aldor 
 *************************************************************************/

Bool		fintVerbose = true; /* Valid only with interactive Aldor */
Bool		fintHistory = false; /* Valid only with interactive Aldor */

UShort	intStepNo = 0;	   /* current step in the interpreter */

/**************************************************************************
 * 
 *************************************************************************/

Bool 	fintConfirm = true;
Bool	fintTimings = true; /* Display timings in loop mode? */
long 	fintMsgLimit = ABPP_UNCLIPPED;    /* Default no msg limit */
long      fintExntraceMode = 0;    /* Default: no trace */

/**************************************************************************
 * 
 * Local functions prototype
 *
 *************************************************************************/

local void 	loadUnitFrLib		(Lib);
local Bool	loadOtherUnits		(void);
local void	unloadOtherUnits	(void);
local Bool 	loadMainUnit 		(Foam);
local void	loadUnit		(String, Buffer);

local void	readDefs		(FintUnit);
local void	readDef			(FintUnit);

local void	unitFree 		(FintUnit);
local Fmt	fmtAlloc		(int);
local void 	fmtFree			(Fmt);
local void	fmtGlobalsFree		(FintUnit);
local void	fmtConstantsFree	(FintUnit);
local void	fmtFluidsFree		(FintUnit);
local void	lexLevelsFree		(FintUnit);
local void	fintLoadLexLevels	(FintUnit, int);
local ShDataObj shDataObjAdd		(AInt, String, int, int, FintUnit);
local ShDataObj shDataObjFind		(AInt, String, int);
local void	shDataObjFree		(ShDataObj);
local void	stackChain		(int);


local dataType	fintStmt		(DataObj);
local dataType 	fintEval		(DataObj);
local dataType	fintGetReference	(Ref);
local void	fintSetMFmt		(DataObj, DataObj);

local int	fintReadFmt		(Fmt *);
local void	fintLoadGlobalsFmt	(FintUnit);
local void	fintLoadConstantsFmt	(FintUnit);
local void	fintLoadFluidsFmt	(FintUnit);
local void	fintLoadLexLevels	(FintUnit, int);

local void	skipProg		(FiProgPos *, int *);
local SFloat    fintRdSFloat		(void);
local DFloat	fintRdDFloat		(void);
local String	fintRdChars		(int cc);
local String	fintGetn		(Length n);
local void	fintPushFluids		(int);

extern void	fintSoftAssert		(char *, char *, int);
extern void	fintHardAssert		(char *, char *, int);

local int	fintExecMainUnit	(void);

local void	fintGetInitInterpTime	(void);
local void	fintGetEndInterpTime	(void);

local dataType	fintDoCall0	(DataObj clos, DataObj);
local dataType	fintDoCall1	(DataObj clos, DataObj, DataObj);
local dataType	fintDoCall	(DataObj clos, DataObj, int, ...);
local dataType	fintDoCallN	(DataObj clos, DataObj, int, DataObj *);

      void      fintWhere(int level);
      void      fintCheckCallStack(void);
local void *	fintSaveState(void);
local void	fintRestoreState(void *);
/**************************************************************************
 * :: fintInit()
 * :: fintFini()
 *************************************************************************/

/* Initializes data structures.
 * Must be called before using any other fint function
 */
void
fintInit(void)
{
	if (!fintInitialized) {
		extern void (*fiExceptionHandler)(char *,void *);
		fiRegisterStateFns(fintSaveState, fintRestoreState);
		fiExceptionHandler = &fintRaiseException;
		fiInitialiseFpu();
	}
	fintInitialized = true;
	fintUnitList = listNil(FintUnit);

	headStack = fintAlloc(union dataObj, STACK_SIZE + 1);
	headStack[STACK_SIZE].ptr = 0;

	stack = headStack;
	bp = headStack;
	sp = headStack + 1;
	ip = 0;
	headStack[0].ptr = (DataObj) NULL;

	/* create a progInfo so that it can process (Clos (Env 0) ...) when
 	 * reads defs
	 */
	prog = (ProgInfo) fintAlloc(struct progInfo, 1);
	progInfoDEnv(prog) = (UByte *) fintAlloc(UByte, 1);
	*progInfoDEnv(prog) = POS_LEX_FMT;   /* null format */

	evalBuf = bufNew();

	/* *********** mainUnit *********** */

	mainUnit = (FintUnit) fintAlloc(fintUnit,1);

	fintUnitId(mainUnit) = unitId++;	
	fintUnitTape(mainUnit) = evalBuf->argv;
	fintUnitName(mainUnit) = "main";
	fintUnitBuffer(mainUnit) = evalBuf;
}

/* Frees all the allocated data structures.
 * Must be called after the last fint() call
 */
void
fintFini(void)
{
	FintUnitList	ul = fintUnitList;
	FintUnit	u;

	/* fintFree(progInfoDEnv(prog));
	   fintFree(prog);
	 */ 

	for (; ul; ul = cdr(ul)) {
		u = car(ul);
		unitFree(u);
	}

	bufFree(evalBuf);
	unloadOtherUnits();
}

local void
fintChainedStackFree(DataObj st)
{
	if (!st) return;

	fintChainedStackFree(st[STACK_SIZE].ptr);
	fintFree(st);
}

/* Called before running the garbage collector.
 * Cleans the stack to improve the effectiveness of the garbage coll.
 * Also deletes all the added supplementary stacks 
 */
void
fintFreeJunk(void)
{
	DataObj		p = headStack;

	while (p < headStack + STACK_SIZE) {
		p->fiWord = (FiWord) 0;
		p = (DataObj) (((FiWord *) p) + 1);
	}

	fintChainedStackFree(headStack[STACK_SIZE].ptr);
	headStack[STACK_SIZE].ptr = 0;   /* unchain added stacks */

	return;
}

/*****************************************************************************
 *
 * :: Units loading
 *
 ****************************************************************************/


local LibList		libUsedList = 0;
local LibList		libDoneList = 0;

/* Check if a the library corresponding to "name" has been already loaded.
 * If not, push this library in libUsedList, i.e. the list of libraries that
 * must be loaded.
 * NOTE that "name" come from globals with protocol `FOAM_Proto_Init'.
 * NOTE that a library must be loaded exactly once.
 */
local void
lazyLibGet(String name)
{
	Lib		lib;
	char		aoFile[80];

	fintLinkDEBUG((void)fprintf(dbOut,"LazyGet of %s...\n", name));

	if (name[0] == '-' && name[1] == 0) return;

	if (strEqual(name, "runtime")) {
		Archive	ar = arFrString("libfoam.al");
		lib = ar ? symeLibrary(car(arGetLibrarySymes(ar))) : NULL;
	}
	else {
		(void)sprintf(aoFile, "%s.ao", name);
		lib = libFrString(aoFile);
	}

	if (lib == NULL)
		LongJmp(fintJmpBuf, 1);

	if (!lib->intLoaded) {
		libUsedList = listCons(Lib)(lib, libUsedList);
		lib->intLoaded = true;
	}
}

/*****************************************************************************
 *
 ****************************************************************************/


/* Read recursively all the library used */
local Bool
loadOtherUnits(void)
{
	Lib	lib0;

	while (libUsedList) {
		lib0 = car(libUsedList);
		libUsedList = listFreeCons(Lib)(libUsedList);
		libDoneList = listCons(Lib)(lib0, libDoneList);
		loadUnitFrLib(lib0);
	}

	return false;
}

local void
unloadOtherUnits(void)
{
	Lib	lib0;

	while (libDoneList) {
		lib0 = car(libDoneList);
		libDoneList = listFreeCons(Lib)(libDoneList);
		libClose(lib0);
	}
}

/* 
   Called for each call of fint.
 */
local Bool
loadMainUnit(Foam foam)
{
	int 		tag, fmt, argc, i, j, n;
	Buffer		buf;
	FintUnit	progUnit;
	FiProgPos	constPos;

        progUnit = (FintUnit) fintAlloc(fintUnit, 1);
	fintUnitBuffer(progUnit) = bufNew();
	fintUnitId(progUnit) = unitId++;
	fintUnitName(progUnit) = "top-level";

	unit = progUnit;

	fintUnitList = listCons(FintUnit)(progUnit, fintUnitList);

	bufPosition(evalBuf) = 0;

	(void)foamToBuffer(evalBuf, foamUnitFormats(foam)); /* Reads ddecl */

	tape = evalBuf->argv;
	ip = 0;

	fintGetTagFmtArgc(tag, fmt, argc);
	hardAssert(tag == FOAM_DFmt);

	fintLoadGlobalsFmt(mainUnit);
	fintLoadConstantsFmt(mainUnit);
	fintLoadFluidsFmt(mainUnit);
	fintLoadLexLevels(mainUnit, argc);

	/* Copy shared structures in progUnit */

	fintUnitGlobs(progUnit) = fintUnitGlobs(mainUnit);
	fintUnitConsts(progUnit) = fintUnitConsts(mainUnit);
	fintUnitFluids(progUnit) = fintUnitFluids(mainUnit);
	fintUnitLexLevels(progUnit) = fintUnitLexLevels(mainUnit);

	fintUnitGlobsCount(progUnit) = fintUnitGlobsCount(mainUnit);
	fintUnitConstsCount(progUnit) = fintUnitConstsCount(mainUnit);
	fintUnitFluidsCount(progUnit) = fintUnitFluidsCount(mainUnit);
	fintUnitLexLevelsCount(progUnit) = fintUnitLexLevelsCount(mainUnit);	

	fintUnitGlobValues(progUnit) = fintUnitGlobValues(mainUnit);
	fintUnitConstValues(progUnit) = fintUnitConstValues(mainUnit);

	/******** Reads (Const 0) in mainUnit *********/
	
	bufPosition(evalBuf) = 0;
	
	(void)foamToBuffer(evalBuf, foamArgv(foam->foamUnit.defs)[0].code);

	tape = evalBuf->argv;
	ip = 0;

	readDef(mainUnit);

	/******** Reads (Const 1..n) in progUnit *********/

	n = fintUnitConstsCount(mainUnit);
	buf = fintUnitBuffer(progUnit);
	constPos = 0;
	j = 1;
	
	for (i = 1; j < n; i++) {
		Foam  foam0 = foamArgv(foam->foamUnit.defs)[i].code;
		if (foamTag(foam0->foamGen.argv[0].code) != FOAM_Const)
			continue;
		(void)foamToBuffer(buf, foam0);
		j++;

		tape = buf->argv;  /* here, because may change */
		ip = constPos;
		constPos = bufPosition(buf);
		readDef(progUnit);
	}

	fintUnitTape(mainUnit) = evalBuf->argv;
	fintUnitTape(progUnit) = fintUnitBuffer(progUnit)->argv;

	return false;
}


local void
loadUnitFrLib(Lib lib)
{
	String		name;
	Buffer		buf;

	name = libToStringShort(lib);

	fintLinkDEBUG((void)fprintf(dbOut, "----------------- Loading unit %s -----------------\n", name););

	hardAssert(lib);

	libGetUnitBuffer(lib);
	buf = lib->unitb;
	
	tape = buf->argv;
	ip = 0;
	
	loadUnit(name, buf);
}


/* Load a new unit from tape
 */
local void
loadUnit(String name, Buffer buf)
{
	int 		tag, fmt, argc;

	fintGetTagFmtArgc(tag, fmt, argc);
	
	hardAssert(tag == FOAM_Unit && argc == 2);

	/* allocates fintUnit */
	unit = (FintUnit) fintAlloc(fintUnit,1);
	fintUnitList = listCons(FintUnit)(unit, fintUnitList);

	fintUnitId(unit) = unitId++;	
	fintUnitTape(unit) = tape;
	fintUnitBuffer(unit) = buf;
	fintUnitName(unit) = strCopy(name);

	fintGetTagFmtArgc(tag, fmt, argc);
	hardAssert(tag == FOAM_DFmt);

	fintLoadGlobalsFmt(unit);
	fintLoadConstantsFmt(unit);
	fintLoadFluidsFmt(unit);
	fintLoadLexLevels(unit, argc);
	
	readDefs(unit);
}


local void
readDefs(FintUnit unit)
{
	int tag, fmt, argc;
	int j;

	fintGetTagFmtArgc(tag, fmt, argc);
	hardAssert(tag == FOAM_DDef);

	for (j = 0; j < argc; j++)
		readDef(unit);
}

local void
readDef(FintUnit unit)
{
	int 		tag, fmt, argc, num = 0;
	DataObj		ref;
	union dataObj	expr;
	dataType	type;
	FiProgPos	oldIp;

	fintGetTagFmtArgc(tag, fmt, argc);
	hardAssert(tag == FOAM_Def && argc == 2);

	oldIp = ip;

	fintGetTagFmt(tag, fmt);

	if (tag == FOAM_Const) {
		fintGetInt(fmt, num);
		hardAssert(num < fintUnitConstsCount(unit));
	}

	if (tag == FOAM_Const && constType(num) == FOAM_Prog) {
		int 		nLabels, labelsCount, n;
		ProgInfo	p;
		FiProgPos       * pLabels;

		p = (ProgInfo) stoAlloc(OB_Other, sizeof(struct progInfo));
		progInfoName(p) = constId(num);
		constValue(num).progInfo = p;
		progInfoUnit(p) = unit;

		fintGetTagFmtArgc(tag, fmt, argc);
		hardAssert(tag == FOAM_Prog);

		fintGetInt(int0, progInfoSize(p));		/* prog size */
		fintGetInt(int0	, progInfoNLabels(p));		/* max label */
		nLabels = progInfoNLabels(p);

		labelFmt = FOAM_FORMAT_FOR(nLabels);
		progInfoLabelFmt(p) = labelFmt;
		p->_progInfo = 0;

		if (nLabels)
		   	progInfoLabels(p) = (FiProgPos *)
			      stoAlloc(OB_Other,sizeof(FiProgPos) * nLabels);
		else
			progInfoLabels(p) = (FiProgPos *) NULL;

		fintGetByte(progInfoRetType(p));	/* return type */
		fintGetInt(fmt, progInfoMValFmt(p));

		fintGetSInt(progInfoBMask(p));		/* bit mask */
		fintGetSInt(n);				/* skip size */
		fintGetSInt(n);				/* skip time */
		fintGetSInt(n);				/* skip auxbits */

		/* reads par. fmt */
		progInfoParsCount(p) = fintReadFmt(&progInfoFmtPar(p));

		/* reads locs. fmt */
		progInfoLocsCount(p) = fintReadFmt(&progInfoFmtLoc(p));
		
		/* DFluid */
	
		fintGetTagFmtArgc(tag, fmt, argc);
		hardAssert(tag == FOAM_DFluid);

		if (argc) {
			UByte * b;
			
			b = (UByte *) fintAlloc(UByte, argc);
			for (n = 0; n < argc; n++)
				fintGetInt(fmt, b[n]);

			progInfoDFluid(p) = b;
		}
		else
			progInfoDFluid(p) = NULL;

		progInfoDFluidsCount(p) = (UByte) argc;

		/* DEnv */
	
		fintGetTagFmtArgc(tag, fmt, argc);
		hardAssert(tag == FOAM_DEnv);

		if (argc) {
			UByte * b;
			
			b = (UByte *) fintAlloc(UByte, argc);
			for (n = 0; n < argc; n++)
				fintGetInt(fmt, b[n]);

			progInfoDEnv(p) = b;
		}
		else
			progInfoDEnv(p) = NULL;

		progInfoDEnvsCount(p) = (UByte) argc;

		pLabels = progInfoLabels(p);
		labelsCount = 0;

		progInfoSeq(p) = ip;
		skipProg(pLabels, &labelsCount);

		hardAssert(labelsCount <= nLabels);
	}
	else {
		ip = oldIp;
		type = fintGetReference(&ref);
		fintTypedEval(&expr, type);
		fintSet(type, ref, expr);
	}
}

/*****************************************************************************
 *
 * :: Foam Interpretation procedure
 *
 ****************************************************************************/

/* Execute a statement. Returns true iff a return stmt has been found */

local dataType
fintStmt(DataObj retDataObj)
{
	int 		fmt, tag, argc;
	int 		n;
	union dataObj 	expr;
	FiProgPos	stmtPos;
	dataType 	myType;
 readEvalLoop:

	fintStoDEBUG(stoAudit(););
#ifndef NDEBUG
	if (instrCounter++ == instrBreak) {
		/* stoAudit()*/;	/* SET BREAKPOINT HERE */
		fintDebug = true;
	}
#endif
	

	stmtPos = ip;
	
	fintGetTagFmt(tag, fmt);

	fintDEBUG((void)fprintf(dbOut, ">> %s (<%s> in [%s])\n", foamInfo(tag).str, prog->name, prog->unit->name););

	switch (tag) {
	case FOAM_Def:
	case FOAM_Set: {
		DataObj 	ref;
		dataType	type;
		
		
		type = fintGetReference(&ref);
		(void)fintEval(&expr);
		
		fintSet(type, ref, expr);
		break;
	}
	case FOAM_Loose: {
		/* Neutralise a pointer */
		DataObj		loc;
		dataType	type;

		type = fintGetReference(&loc);
		switch ((int)type) {
		   case FOAM_SFlo : expr.fiSFlo = 0.0;break;
		   case FOAM_DFlo : expr.fiDFlo = 0.0;break;
		   default        : expr.fiSInt = 0;break;
		}

		fintSet(type, loc, expr);
	}
	case FOAM_Free:
                (void)fintEval(&expr);
                fintFree0(expr.fiPtr);
                break;

	case FOAM_Goto:
		fintGetInt(labelFmt, n);
		ip = labels[n];
		fintDEBUG((void)fprintf(dbOut, "(Goto %d)\n", n););
		break;

	case FOAM_If: {
		dataType	type;
		type = fintEval(&expr);
		hardAssert(type == FOAM_Word || type == FOAM_Bool);
		fintGetInt(labelFmt, n);

		if ((type == FOAM_Word ? expr.fiWord : expr.fiBool)) {
			ip = labels[n];
			fintDEBUG((void)fprintf(dbOut, "(if causes jump to label %d)\n", n));
		}
		break;
	}
	case FOAM_Return:
		myType = fintEval(retDataObj);
		fintDEBUG((void)fprintf(dbOut, "returning from %s in %s\n", prog->name, prog->unit->name));
		return myType;   /* Unique exit for a stmt sequence */

	case FOAM_Seq:   /* Ignore... */
		fintGetInt(fmt,argc);
		break;

	case FOAM_Select: {
		int i;

		fintGetInt(fmt,argc);   /* Get the arity */
		fintTypedEval(&expr, FOAM_SInt);

		fintGetInt(labelFmt, n);

		/*
		 * Bug? What happens if expr.fiSInt >= argc? It looks
		 * as though we keep reading the tape interpreting the
		 * bytes as labels even if we run out of labels.
		 */
		for (i = 0; i < expr.fiSInt; i++)
			fintGetInt(labelFmt, n);

		fintDEBUG({
			if (i >= argc)
				(void)fprintf(dbOut, "Select: read too many labels\n");
		});
		ip = labels[n];
		
		break;
	}
	case FOAM_Throw: {
		union dataObj expr;
		(void)fintEval(&expr); /* Tag: Ignore */
		fintTypedEval(&expr, FOAM_Word);

		if (fintExntraceMode == 2) {
		  /* Stack trace sent to stderr */
		  FILE *oldDbOut = dbOut;
		  dbOut = osStderr; 
		  fprintf(dbOut, "Aldor interpreter: exception raised (may be caught), backtrace:\n");
		  fintWhere(FINT_BACKTRACE_CUTOFF);
		  fprintf(dbOut, "\n");
		  dbOut = oldDbOut;
		};

		fiUnwind(int0, expr.fiWord);
		bug("unwind returned");
		break;
	}
	case FOAM_CCall:
	case FOAM_BCall:
	case FOAM_OCall:
	case FOAM_PCall:
	case FOAM_BVal:
	case FOAM_Cast:
	case FOAM_ANew:
	case FOAM_RNew:
	case FOAM_RRNew:
	case FOAM_TRNew:
	case FOAM_MFmt:
	case FOAM_Values:
	case FOAM_Catch:
	case FOAM_EEnsure:
		ip = stmtPos;
		(void)fintEval(&expr); /* we ignore the ret value */
		break;
	case FOAM_Label:
		fintGetInt(fmt, n);
	        fintDEBUG((void)printf("(Label %d)\n", n));
		break;
	case FOAM_Nil:
	case FOAM_Lex: /* we get things like that when we -q0 (deadvar 
			is effective in killing them */
	case FOAM_NOp:
		break;
	default:
	        fintWhere(int0);
		bug("fintStmt: %s (<%s> in [%s]) unimplemented...\n",
		    foamInfo(tag).str, prog->name, prog->unit->name);
	}

	goto readEvalLoop;
	return -1;
}

local dataType
fintEvalBCall(DataObj retDataObj)
{
	int		call;
	union dataObj	expr1, expr2, expr3, expr4, expr5, expr6;
	dataType	type, myType;

#if SMALL_BVAL_TAGS
	fintGetByte(call);
#else
	fintGetHInt(call);
#endif
	call += FOAM_BVAL_START;

	fintDEBUG((void)fprintf(dbOut,
		  "fintBCall: %s\n", foamBValInfo(call).str););

	switch (call) {

	/* -------------------- Operations on BOOLEAN ------------- */

	case FOAM_BVal_BoolFalse:
		retDataObj->fiBool = fiFalse;
		myType = FOAM_Bool;
		break;
	case FOAM_BVal_BoolTrue:
		retDataObj->fiBool = fiTrue;
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BoolNot:
		type = fintEval(&expr1);

		fintForceBoolToWord(expr1, type);

		retDataObj->fiBool = ((expr1.fiWord) ? (fiFalse) : (fiTrue));

		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BoolAnd:

		type = fintEval(&expr1);
		fintForceBoolToWord(expr1, type);

		type = fintEval(&expr2);
		fintForceBoolToWord(expr2, type);

		retDataObj->fiBool = ((expr1.fiWord && expr2.fiWord) ?
				fiTrue :
				fiFalse);

		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BoolOr:
		type = fintEval(&expr1);
		fintForceBoolToWord(expr1, type);

		type = fintEval(&expr2);
		fintForceBoolToWord(expr2, type);


		retDataObj->fiBool = ((expr1.fiWord || expr2.fiWord) ?
			       fiTrue :
			       fiFalse);

		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BoolEQ:
		type = fintEval(&expr1);
		fintForceBoolToWord(expr1, type);

		type = fintEval(&expr2);
		fintForceBoolToWord(expr2, type);


		retDataObj->fiBool = ((expr1.fiWord == expr2.fiWord) ?
			       fiTrue :
			       fiFalse);

		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BoolNE:
		type = fintEval(&expr1);
		fintForceBoolToWord(expr1, type);

		type = fintEval(&expr2);
		fintForceBoolToWord(expr2, type);


		retDataObj->fiBool = ((expr1.fiWord != expr2.fiWord) ?
			       fiTrue :
			       fiFalse);

		myType = FOAM_Bool;
		break;

	/* -------------------- Operations on CHAR ------------- */
	case FOAM_BVal_CharSpace:
		retDataObj->fiChar = ' ';
		myType = FOAM_Char;
		break;

	case FOAM_BVal_CharNewline:
		retDataObj->fiChar = '\n';
		myType = FOAM_Char;
		break;


	case FOAM_BVal_CharTab:
		retDataObj->fiChar = '\t';
		myType = FOAM_Char;
		break;

	case FOAM_BVal_CharMax:
		retDataObj->fiChar = fiCharMax();
		myType = FOAM_Char;
		break;

	case FOAM_BVal_CharMin:
		retDataObj->fiChar = fiCharMin();
		myType = FOAM_Char;
		break;

	case FOAM_BVal_CharIsDigit:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (FiBool) isdigit(expr1.fiChar);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_CharIsLetter:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (FiBool) isalpha(expr1.fiChar);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_CharEQ:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiChar == expr2.fiChar);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_CharNE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiChar != expr2.fiChar);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_CharLT:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiChar < expr2.fiChar);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_CharLE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiChar <= expr2.fiChar);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_CharLower:
		(void)fintEval(&expr1);
		retDataObj->fiChar = fiCharLower(expr1.fiChar);
		myType = FOAM_Char;
		break;

	case FOAM_BVal_CharUpper:
		(void)fintEval(&expr1);
		retDataObj->fiChar = fiCharUpper(expr1.fiChar);
		myType = FOAM_Char;
		break;

	case FOAM_BVal_CharOrd:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = (FiSInt) (expr1.fiChar);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_CharNum:
		(void)fintEval(&expr1);
		retDataObj->fiChar = (FiChar) (expr1.fiSInt);
		myType = FOAM_Char;
		break;

	/* -------------------- Rounding Values ------------- */

	case FOAM_BVal_RoundNearest:
		retDataObj->fiSInt = fiRoundNearest();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_RoundZero:
		retDataObj->fiSInt = fiRoundZero();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_RoundUp:
		retDataObj->fiSInt = fiRoundUp();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_RoundDown:
		retDataObj->fiSInt = fiRoundDown();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_RoundDontCare:
		retDataObj->fiSInt = fiRoundDontCare();
		myType = FOAM_SInt;
		break;

	/* -------------------- Operations on SFLO ------------- */

	case FOAM_BVal_SFlo0:
		retDataObj->fiSFlo = (FiSFlo) 0;
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFlo1:
		retDataObj->fiSFlo = (FiSFlo) 1;
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloMax:
		retDataObj->fiSFlo = fiSFloMax();
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloMin:
		retDataObj->fiSFlo = fiSFloMin();
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloEpsilon:
		retDataObj->fiSFlo = fiSFloEpsilon();
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloIsZero:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSFlo ? fiFalse : fiTrue);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SFloIsNeg:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSFlo < 0 ? fiTrue : fiFalse);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SFloIsPos:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSFlo > 0 ? fiTrue : fiFalse);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SFloEQ:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSFlo == expr2.fiSFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SFloNE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSFlo != expr2.fiSFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SFloLT:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSFlo < expr2.fiSFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SFloLE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSFlo <= expr2.fiSFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SFloNegate:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = -expr1.fiSFlo;
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloPrev:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = fiSFloPrev(expr1.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloNext:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = fiSFloNext(expr1.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSFlo = (expr1.fiSFlo + expr2.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloMinus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSFlo = (expr1.fiSFlo - expr2.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloTimes:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSFlo = (expr1.fiSFlo * expr2.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloTimesPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiSFlo = fiSFloTimesPlus(expr1.fiSFlo,
						     expr2.fiSFlo,
						     expr3.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloDivide:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSFlo = (expr1.fiSFlo / expr2.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloTruncate:
		(void)fintEval(&expr1);
		retDataObj->fiBInt = fiSFloTruncate(expr1.fiSFlo);
		myType = FOAM_BInt;
		break;
		
	case FOAM_BVal_SFloFraction:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = fiSFloFraction(expr1.fiSFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloRound:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiSFloRound(expr1.fiSFlo, expr2.fiSInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_SFloRPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiSFlo = fiSFloRPlus(expr1.fiSFlo,expr2.fiSFlo,
						 expr3.fiSInt);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloRMinus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiSFlo = fiSFloRMinus(expr1.fiSFlo,expr2.fiSFlo,
						 expr3.fiSInt);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloRTimes:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiSFlo = fiSFloRTimes(expr1.fiSFlo,expr2.fiSFlo,
						  expr3.fiSInt);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloRTimesPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		(void)fintEval(&expr4);

		retDataObj->fiSFlo = fiSFloRTimesPlus(expr1.fiSFlo,
						      expr2.fiSFlo,
						      expr3.fiSFlo,
						      expr4.fiSInt);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloRDivide:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiSFlo = fiSFloRDivide(expr1.fiSFlo,expr2.fiSFlo,
						   expr3.fiSInt);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SFloDissemble:

		retDataObj->ptr = fintAlloc(union dataObj, 3);
		
		(void)fintEval(&expr1);

		fiSFloDissemble((FiSFlo)expr1.fiSFlo,
				(FiBool *) & (retDataObj->ptr[0].fiBool),
				(FiSInt *) & (retDataObj->ptr[1].fiSInt),
				(FiWord *) & (retDataObj->ptr[2].fiWord));
		
		myType = FOAM_NOp;
		break;

	case FOAM_BVal_SFloAssemble:

		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);

		retDataObj->fiSFlo = fiSFloAssemble((FiBool)expr1.fiBool,
						    expr2.fiSInt,
			       			    expr3.fiWord);
		
		myType = FOAM_SFlo;
		break;

	/* -------------------- Operations on DFLO ------------- */

	case FOAM_BVal_DFlo0:
		retDataObj->fiDFlo = (FiDFlo) 0;
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFlo1:
		retDataObj->fiDFlo = (FiDFlo) 1;
		myType = FOAM_DFlo;
		break;


	case FOAM_BVal_DFloMax:
		retDataObj->fiDFlo = fiDFloMax();
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloMin:
		retDataObj->fiDFlo = fiDFloMin();
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloEpsilon:
		retDataObj->fiDFlo = fiDFloEpsilon();
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloIsZero:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiDFlo ? fiFalse : fiTrue);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_DFloIsNeg:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiDFlo < 0 ? fiTrue : fiFalse);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_DFloIsPos:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiDFlo > 0 ? fiTrue : fiFalse);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_DFloEQ:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiDFlo == expr2.fiDFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_DFloNE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiDFlo != expr2.fiDFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_DFloLT:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiDFlo < expr2.fiDFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_DFloLE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiDFlo <= expr2.fiDFlo);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_DFloNegate:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = -expr1.fiDFlo;
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloPrev:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = fiDFloPrev(expr1.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloNext:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = fiDFloNext(expr1.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiDFlo = (expr1.fiDFlo + expr2.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloMinus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiDFlo = (expr1.fiDFlo - expr2.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloTimes:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiDFlo = (expr1.fiDFlo * expr2.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloTimesPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiDFlo = fiDFloTimesPlus(expr1.fiDFlo,
						     expr2.fiDFlo,
						     expr3.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloDivide:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiDFlo = (expr1.fiDFlo / expr2.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloTruncate:
		(void)fintEval(&expr1);
		retDataObj->fiBInt = fiDFloTruncate(expr1.fiDFlo);
		myType = FOAM_BInt;
		break;
		
	case FOAM_BVal_DFloFraction:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = fiDFloFraction(expr1.fiDFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloRound:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiDFloRound(expr1.fiDFlo, expr2.fiSInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_DFloRPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiDFlo = fiDFloRPlus(expr1.fiDFlo,expr2.fiDFlo,
						 expr3.fiSInt);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloRMinus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiDFlo = fiDFloRMinus(expr1.fiDFlo,expr2.fiDFlo,
						 expr3.fiSInt);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloRTimes:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiDFlo = fiDFloRTimes(expr1.fiDFlo,expr2.fiDFlo,
						  expr3.fiSInt);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloRDivide:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		
		retDataObj->fiDFlo = fiDFloRDivide(expr1.fiDFlo,expr2.fiDFlo,
						   expr3.fiSInt);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloRTimesPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		(void)fintEval(&expr4);

		retDataObj->fiDFlo = fiDFloRTimesPlus(expr1.fiDFlo,
						      expr2.fiDFlo,
						      expr3.fiDFlo,
						      expr4.fiSInt);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloDissemble:

		retDataObj->ptr = fintAlloc(union dataObj, 4);
		
		(void)fintEval(&expr1);

		fiDFloDissemble((FiDFlo)expr1.fiDFlo,
				(FiBool *) & (retDataObj->ptr[0].fiBool),
				(FiSInt *) & (retDataObj->ptr[1].fiSInt),
				(FiWord *) & (retDataObj->ptr[2].fiWord),
				(FiWord *) & (retDataObj->ptr[3].fiWord));
		
		myType = FOAM_NOp;
		break;

	case FOAM_BVal_DFloAssemble:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		(void)fintEval(&expr4);

		retDataObj->fiDFlo = fiDFloAssemble((FiBool)expr1.fiBool,
						    expr2.fiSInt,
			       			    expr3.fiWord,
						    expr4.fiWord);
		myType = FOAM_DFlo;
		break;

	/* -------------------- Operations on BYTE ------------- */

	case FOAM_BVal_Byte0:
		retDataObj->fiByte = (FiByte) 0;
		myType = FOAM_Byte;
		break;

	case FOAM_BVal_Byte1:
		retDataObj->fiByte = (FiByte) 1;
		myType = FOAM_Byte;
		break;

	case FOAM_BVal_ByteMax:
		retDataObj->fiByte = fiByteMax();
		myType = FOAM_Byte;
		break;

	case FOAM_BVal_ByteMin:
		retDataObj->fiByte = fiByteMin();
		myType = FOAM_Byte;
		break;

	/* -------------------- Operations on WORD ------------- */

	case FOAM_BVal_WordTimesDouble:
		retDataObj->ptr = fintAlloc(union dataObj, 2);

		(void)fintEval(&expr1);
		(void)fintEval(&expr2);

		fiWordTimesDouble(expr1.fiWord, expr2.fiWord,
				 &expr3.fiWord,&expr4.fiWord);

		retDataObj->ptr[0].fiWord = expr3.fiWord;
		retDataObj->ptr[1].fiWord = expr4.fiWord;

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_WordDivideDouble:
		retDataObj->ptr = fintAlloc(union dataObj, 3);

		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);

		fiWordDivideDouble(expr1.fiWord, expr2.fiWord, expr3.fiWord,
				  &expr4.fiWord,&expr5.fiWord,&expr6.fiWord);

		retDataObj->ptr[0].fiWord = expr4.fiWord;
		retDataObj->ptr[1].fiWord = expr5.fiWord;
		retDataObj->ptr[2].fiWord = expr6.fiWord;

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_WordPlusStep:
		retDataObj->ptr = fintAlloc(union dataObj, 2);

		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);

		fiWordPlusStep(expr1.fiWord, expr2.fiWord, expr3.fiWord,
			      &expr4.fiWord,&expr5.fiWord);

		retDataObj->ptr[0].fiWord = expr4.fiWord;
		retDataObj->ptr[1].fiWord = expr5.fiWord;

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_WordTimesStep:
		retDataObj->ptr = fintAlloc(union dataObj, 2);

		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		(void)fintEval(&expr4);

		fiWordTimesStep(expr1.fiWord, expr2.fiWord, expr3.fiWord,
				expr4.fiWord,&expr5.fiWord,&expr6.fiWord);

		retDataObj->ptr[0].fiWord = expr5.fiWord;
		retDataObj->ptr[1].fiWord = expr6.fiWord;

		myType = FOAM_NOp;
		break;

	/* -------------------- Operations on HINT ------------- */

	case FOAM_BVal_HInt0:
		retDataObj->fiHInt = (FiHInt) 0;
		myType = FOAM_HInt;
		break;

	case FOAM_BVal_HInt1:
		retDataObj->fiHInt = (FiHInt) 1;
		myType = FOAM_HInt;
		break;

	case FOAM_BVal_HIntMax:
		retDataObj->fiHInt = fiHIntMax();
		myType = FOAM_HInt;
		break;

	case FOAM_BVal_HIntMin:
		retDataObj->fiHInt = fiHIntMin();
		myType = FOAM_HInt;
		break;

	/* -------------------- Operations on SINT ------------- */

	case FOAM_BVal_SInt0:
		retDataObj->fiSInt = (FiSInt) 0;
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SInt1:
		retDataObj->fiSInt = (FiSInt) 1;
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntMax:
		retDataObj->fiSInt = fiSIntMax();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntMin:
		retDataObj->fiSInt = fiSIntMin();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntIsZero:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSInt ? fiFalse : fiTrue);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntIsNeg:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSInt < 0 ? fiTrue : fiFalse);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntIsPos:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSInt > 0 ? fiTrue : fiFalse);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntIsEven:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSInt & 1 ? fiFalse : fiTrue);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntIsOdd:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiSInt & 1 ? fiTrue : fiFalse);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntEQ:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSInt == expr2.fiSInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntNE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSInt != expr2.fiSInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntLT:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSInt < expr2.fiSInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntLE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiSInt <= expr2.fiSInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntNegate:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = -(expr1.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntPrev:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = (expr1.fiSInt - 1);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntNext:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = (expr1.fiSInt + 1);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt + expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntMinus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt - expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntTimes:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt * expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntTimesPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiSInt = (expr1.fiSInt * expr2.fiSInt + expr3.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntMod:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt % expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntQuo:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = fiSIntQuo(expr1.fiSInt, expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntRem:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = fiSIntRem(expr1.fiSInt, expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntDivide:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);

		retDataObj->ptr = fintAlloc(union dataObj, 2);
		fiSIntDivide(expr1.fiSInt, expr2.fiSInt,
			   (FiSInt *) & (retDataObj->ptr[0].fiSInt) ,
			   (FiSInt *) & (retDataObj->ptr[1].fiSInt));

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_SIntGcd:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = fiSIntGcd(expr1.fiSInt, expr2.fiSInt);
		myType = FOAM_SInt;
		break;

/*	case FOAM_BVal_SIntPlusMod:
 *	case FOAM_BVal_SIntMinusMod:
 *	case FOAM_BVal_SIntTimesMod:
 *
 *	SEE FOAM_BVal_SInt[Plus|Minus|Times]
 */

	case FOAM_BVal_SIntPlusMod:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiSInt = (expr1.fiSInt + expr2.fiSInt) % expr3.fiSInt;
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntMinusMod:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiSInt = (expr1.fiSInt - expr2.fiSInt) % expr3.fiSInt;
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntTimesMod:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiSInt = (expr1.fiSInt * expr2.fiSInt) % expr3.fiSInt;
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntLength:
		(void)fintEval(&expr1);

		retDataObj->fiSInt = fiSIntLength(expr1.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntShiftUp:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt << expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntShiftDn:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt >> expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntBit:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = fiSIntBit(expr1.fiSInt, expr2.fiSInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_SIntNot:
		(void)fintEval(&expr1);

		retDataObj->fiSInt = ~(expr1.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntAnd:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt & expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntOr:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt | expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntXOr:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiSInt = (expr1.fiSInt ^ expr2.fiSInt);
		myType = FOAM_SInt;
		break;

	/* -------------------- Operations on BInt ------------- */

	case FOAM_BVal_BInt0:
		retDataObj->fiBInt = fiBInt0();
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BInt1:
		retDataObj->fiBInt = fiBInt1();
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntIsZero:
		(void)fintEval(&expr1);
		retDataObj->fiBool = fiBIntIsZero(expr1.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntIsNeg:
		(void)fintEval(&expr1);
		retDataObj->fiBool = fiBIntIsNeg(expr1.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntIsEven:
		(void)fintEval(&expr1);
		retDataObj->fiBool = fiBIntEQ(fiBIntMod(expr1.fiBInt, fiBIntNew(2)), fiBInt0());
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntIsOdd:
		(void)fintEval(&expr1);
		retDataObj->fiBool = fiBIntNE(fiBIntMod(expr1.fiBInt, fiBIntNew(2)), fiBInt0());
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntIsSingle:
		(void)fintEval(&expr1);
		retDataObj->fiBool = fiBIntIsSingle(expr1.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntIsPos:
		(void)fintEval(&expr1);
		retDataObj->fiBool = fiBIntIsPos(expr1.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntEQ:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = fiBIntEQ(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntNE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = fiBIntNE(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntLT:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = fiBIntLT(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntLE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = fiBIntLE(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_BIntNegate:
		(void)fintEval(&expr1);
		retDataObj->fiBInt = fiBIntNegate(expr1.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntPrev:
		(void)fintEval(&expr1);
		retDataObj->fiBInt = fiBIntMinus(expr1.fiBInt, fiBInt1());
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntNext:
		(void)fintEval(&expr1);
		retDataObj->fiBInt = fiBIntPlus(expr1.fiBInt, fiBInt1());
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntPlus(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntMinus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntMinus(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntTimes:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntTimes(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntTimesPlus:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiBInt = fiBIntTimesPlus(expr1.fiBInt,
						     expr2.fiBInt,
						     expr3.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntMod:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntMod(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntQuo:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntQuo(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntRem:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntRem(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntDivide:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);

		retDataObj->ptr = fintAlloc(union dataObj, 2);
		fiBIntDivide(expr1.fiBInt, expr2.fiBInt,
			   (FiBInt *) & (retDataObj->ptr[0].fiBInt),
			   (FiBInt *) & (retDataObj->ptr[1].fiBInt));

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_BIntGcd:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntGcd(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntSIPower:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntSIPower(expr1.fiBInt, expr2.fiSInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntBIPower:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntBIPower(expr1.fiBInt, expr2.fiBInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntPowerMod:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);
		retDataObj->fiBInt = fiBIntPowerMod(expr1.fiBInt,
						     expr2.fiBInt,
						     expr3.fiBInt);
		myType = FOAM_BInt;
		break;
		
	case FOAM_BVal_BIntLength:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = fiBIntLength(expr1.fiBInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_BIntShiftUp:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntShiftUp(expr1.fiBInt, (int)expr2.fiSInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntShiftDn:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntShiftDn(expr1.fiBInt, (int)expr2.fiSInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntShiftRem:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBInt = fiBIntShiftRem(expr1.fiBInt, (int)expr2.fiSInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntBit:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = fiBIntBit(expr1.fiBInt, expr2.fiSInt);
		myType = FOAM_Bool;
		break;

	/* -------------------- Operations on Ptr ------------- */

	case FOAM_BVal_PtrNil:
		retDataObj->fiPtr = 0;
		myType = FOAM_Ptr;
		break;

	case FOAM_BVal_PtrIsNil:
		(void)fintEval(&expr1);
		retDataObj->fiBool = (expr1.fiPtr == 0);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_PtrMagicEQ:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiPtr == expr2.fiPtr);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_PtrEQ:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiPtr == expr2.fiPtr);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_PtrNE:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		retDataObj->fiBool = (expr1.fiPtr != expr2.fiPtr);
		myType = FOAM_Bool;
		break;

	/* -------------------- Text Operations  ----------------- */
	case FOAM_BVal_FormatSFlo:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);

		retDataObj->fiSInt = fiFormatSFlo(expr1.fiSFlo,
						  expr2.fiArr, expr3.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_FormatDFlo:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);

		retDataObj->fiSInt = fiFormatDFlo(expr1.fiDFlo,
						  expr2.fiArr, expr3.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_FormatSInt:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);

		retDataObj->fiSInt = fiFormatSInt(expr1.fiSInt,
						  expr2.fiArr, expr3.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_FormatBInt:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);
		(void)fintEval(&expr3);

		retDataObj->fiSInt = fiFormatBInt(expr1.fiBInt,
						  expr2.fiArr, expr3.fiSInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_ScanSFlo:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);

		retDataObj->ptr = fintAlloc(union dataObj, 2);
		fiScanSFlo( expr1.fiArr, 
			   (FiSInt) expr2.fiSInt,
			   (FiSFlo *) & (retDataObj->ptr[0].fiSFlo),
			   (FiSInt *) & (retDataObj->ptr[1].fiSInt));

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_ScanDFlo:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);

		retDataObj->ptr = fintAlloc(union dataObj, 2);
		fiScanDFlo( expr1.fiArr, 
                           (FiSInt) expr2.fiSInt,
			   (FiDFlo *) & (retDataObj->ptr[0].fiDFlo),
			   (FiSInt *) & (retDataObj->ptr[1].fiSInt));

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_ScanSInt:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);

		retDataObj->ptr = fintAlloc(union dataObj, 2);
		fiScanSInt( expr1.fiArr, 
                           (FiSInt) expr2.fiSInt,
			   (FiSInt *) & (retDataObj->ptr[0].fiSInt),
			   (FiSInt *) & (retDataObj->ptr[1].fiSInt));

		myType = FOAM_NOp;
		break;

	case FOAM_BVal_ScanBInt:
		(void)fintEval(&expr1);
		(void)fintEval(&expr2);

		retDataObj->ptr = fintAlloc(union dataObj, 2);
		fiScanBInt( expr1.fiArr, 
		           (FiSInt) expr2.fiSInt,
			   (FiBInt *) & (retDataObj->ptr[0].fiBInt),
			   (FiSInt *) & (retDataObj->ptr[1].fiSInt));
		myType = FOAM_NOp;
		break;

	/* -------------------- Conversion Operations  ------------- */

	case FOAM_BVal_SFloToDFlo:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = fiSFloToDFlo(expr1.fiSFlo);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_DFloToSFlo:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = fiDFloToSFlo(expr1.fiDFlo);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_ByteToSInt:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = fiByteToSInt(expr1.fiByte);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntToByte:
		(void)fintEval(&expr1);
		retDataObj->fiByte = fiSIntToByte(expr1.fiSInt);
		myType = FOAM_Byte;
		break;

	case FOAM_BVal_HIntToSInt:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = fiHIntToSInt(expr1.fiHInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntToHInt:
		(void)fintEval(&expr1);
		retDataObj->fiHInt = fiSIntToHInt(expr1.fiSInt);
		myType = FOAM_HInt;
		break;

	case FOAM_BVal_SIntToBInt:
		(void)fintEval(&expr1);
		retDataObj->fiBInt = fiSIntToBInt(expr1.fiSInt);
		myType = FOAM_BInt;
		break;

	case FOAM_BVal_BIntToSInt:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = fiBIntToSInt(expr1.fiBInt);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntToSFlo:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = (FiSFlo) expr1.fiSInt;
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_SIntToDFlo:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = (FiDFlo) expr1.fiSInt;
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_BIntToSFlo:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = fiBIntToSFlo(expr1.fiBInt);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_BIntToDFlo:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = fiBIntToDFlo(expr1.fiBInt);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_PtrToSInt:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = fiPtrToSInt(expr1.fiPtr);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SIntToPtr:
		(void)fintEval(&expr1);
		retDataObj->fiPtr = fiSIntToPtr(expr1.fiSInt);
		myType = FOAM_Ptr;
		break;

	case FOAM_BVal_ArrToSFlo:
		(void)fintEval(&expr1);
		retDataObj->fiSFlo = fiArrToSFlo((FiArr) expr1.fiArr);
		myType = FOAM_SFlo;
		break;

	case FOAM_BVal_ArrToDFlo:
		(void)fintEval(&expr1);
		retDataObj->fiDFlo = fiArrToDFlo((FiArr) expr1.fiArr);
		myType = FOAM_DFlo;
		break;

	case FOAM_BVal_ArrToSInt:
		(void)fintEval(&expr1);
		retDataObj->fiSInt = fiArrToSInt(expr1.fiArr);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_ArrToBInt:
		(void)fintEval(&expr1);
		retDataObj->fiBInt = fiArrToBInt(expr1.fiArr);
		myType = FOAM_BInt;
		break;

	/* ---------------- Platform specific operations ------------- */

	case FOAM_BVal_PlatformRTE:
		retDataObj->fiSInt = fiPlatformRTE();
		myType = FOAM_SInt;
		break;
		
	case FOAM_BVal_PlatformOS:
		retDataObj->fiSInt = fiPlatformOS();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_Halt:

		(void)fintEval(&expr1);
		switch ((int)expr1.fiSInt) {
		case FOAM_Halt_BadDependentType:
		  fiRaiseException((FiWord)"(Aldor error) Bad use of a dependent type"); 
		  break;
		case FOAM_Halt_NeverReached:
		  fiRaiseException((FiWord)"(Aldor error) Reached a \"never\"");
		  break;
		case FOAM_Halt_BadUnionCase:
		  fiRaiseException((FiWord)"(Aldor error) Bad union branch.");
		  break;
		case FOAM_Halt_AssertFailed:
		  fiRaiseException((FiWord)"(Aldor error) Assertion failed.");
		  break;
		case FOAM_Halt_BadFortranRecursion:
		  fiRaiseException((FiWord)"(Aldor error) Bad Fortran recursion."); 
		  break;
		case FOAM_Halt_BadPointerWrite:
		  fiRaiseException((FiWord)"(Aldor error) Write to invalid pointer (read-only?)."); 
		  break;
		default:
		  fiRaiseException((FiWord)"(Aldor error) Halt"); 
		  break;
		}
		LongJmp(fintJmpBuf, 1);

		myType = FOAM_Nil;
		break;

	/* ----------------- Store manager operations ------------- */

	case FOAM_BVal_StoForceGC:
		fiStoForceGC();
		retDataObj->fiPtr = 0; /* Hopefully never used */
		myType = FOAM_NOp;
		break;

	case FOAM_BVal_StoInHeap:
		fintTypedEval(&expr1, FOAM_Ptr);
		retDataObj->fiBool = fiStoInHeap(expr1.fiPtr);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_StoIsWritable:
		fintTypedEval(&expr1, FOAM_Ptr);
		retDataObj->fiSInt = fiStoIsWritable(expr1.fiPtr);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_StoMarkObject:
		fintTypedEval(&expr1, FOAM_Ptr);
		retDataObj->fiSInt = fiStoMarkObject(expr1.fiPtr);
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_StoRecode:
		fintTypedEval(&expr1, FOAM_Ptr);
		fintTypedEval(&expr2, FOAM_SInt);
		retDataObj->fiWord = fiStoRecode(expr1.fiPtr, expr2.fiSInt);
		myType = FOAM_Word;
		break;

	case FOAM_BVal_StoNewObject:
		fintTypedEval(&expr1, FOAM_SInt);
		fintTypedEval(&expr2, FOAM_Bool);
		fiStoNewObject(expr1.fiSInt, expr2.fiBool);
		retDataObj->fiPtr = 0; /* Hopefully never used */
		myType = FOAM_NOp;
		break;

	case FOAM_BVal_StoATracer:
		fintTypedEval(&expr1, FOAM_SInt);
		fintTypedEval(&expr2, FOAM_Clos);
		fiStoATracer(expr1.fiSInt, expr2.fiClos);
		retDataObj->fiPtr = 0; /* Hopefully never used */
		myType = FOAM_NOp;
		break;

	case FOAM_BVal_StoCTracer:
		fintTypedEval(&expr1, FOAM_SInt);
		fintTypedEval(&expr2, FOAM_Word);
		fiStoATracer(expr1.fiSInt, (FiClos) expr2.fiWord);
		retDataObj->fiPtr = 0; /* Hopefully never used */
		myType = FOAM_NOp;
		break;

	case FOAM_BVal_StoShow:
		fintTypedEval(&expr1, FOAM_SInt);
		fiStoShow(expr1.fiSInt);
		retDataObj->fiPtr = 0; /* Hopefully never used */
		myType = FOAM_NOp;
		break;

	case FOAM_BVal_StoShowArgs:
		fintTypedEval(&expr1, FOAM_Ptr);
		retDataObj->fiSInt = fiStoShowArgs(expr1.fiPtr);
		myType = FOAM_SInt;
		break;

	/* --------------------- Raw type tags ------------------ */

	case FOAM_BVal_TypeInt8:
		retDataObj->fiSInt = fiTypeInt8();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeInt16:
		retDataObj->fiSInt = fiTypeInt16();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeInt32:
		retDataObj->fiSInt = fiTypeInt32();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeInt64:
		retDataObj->fiSInt = fiTypeInt64();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeInt128:
		retDataObj->fiSInt = fiTypeInt128();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeNil:
		retDataObj->fiSInt = fiTypeNil();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeChar:
		retDataObj->fiSInt = fiTypeChar();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeBool:
		retDataObj->fiSInt = fiTypeBool();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeByte:
		retDataObj->fiSInt = fiTypeByte();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeHInt:
		retDataObj->fiSInt = fiTypeHInt();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeSInt:
		retDataObj->fiSInt = fiTypeSInt();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeBInt:
		retDataObj->fiSInt = fiTypeBInt();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeSFlo:
		retDataObj->fiSInt = fiTypeSFlo();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeDFlo:
		retDataObj->fiSInt = fiTypeDFlo();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeWord:
		retDataObj->fiSInt = fiTypeWord();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeClos:
		retDataObj->fiSInt = fiTypeClos();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypePtr:
		retDataObj->fiSInt = fiTypePtr();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeRec:
		retDataObj->fiSInt = fiTypeRec();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeArr:
		retDataObj->fiSInt = fiTypeArr();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_TypeTR:
		retDataObj->fiSInt = fiTypeTR();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfInt8:
		retDataObj->fiSInt = fiSizeOfInt8();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfInt16:
		retDataObj->fiSInt = fiSizeOfInt16();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfInt32:
		retDataObj->fiSInt = fiSizeOfInt32();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfInt64:
		retDataObj->fiSInt = fiSizeOfInt64();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfInt128:
		retDataObj->fiSInt = fiSizeOfInt128();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfNil:
		retDataObj->fiSInt = fiSizeOfNil();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfChar:
		retDataObj->fiSInt = fiSizeOfChar();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfBool:
		retDataObj->fiSInt = fiSizeOfBool();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfByte:
		retDataObj->fiSInt = fiSizeOfByte();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfHInt:
		retDataObj->fiSInt = fiSizeOfHInt();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfSInt:
		retDataObj->fiSInt = fiSizeOfSInt();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfBInt:
		retDataObj->fiSInt = fiSizeOfBInt();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfSFlo:
		retDataObj->fiSInt = fiSizeOfSFlo();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfDFlo:
		retDataObj->fiSInt = fiSizeOfDFlo();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfWord:
		retDataObj->fiSInt = fiSizeOfWord();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfClos:
		retDataObj->fiSInt = fiSizeOfClos();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfPtr:
		retDataObj->fiSInt = fiSizeOfPtr();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfRec:
		retDataObj->fiSInt = fiSizeOfRec();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfArr:
		retDataObj->fiSInt = fiSizeOfArr();
		myType = FOAM_SInt;
		break;

	case FOAM_BVal_SizeOfTR:
		retDataObj->fiSInt = fiSizeOfTR();
		myType = FOAM_SInt;
		break;

	/* ----------------- Linked list operations ------------- */

	case FOAM_BVal_ListNil:
		retDataObj->fiPtr = fiListNil();
		myType = FOAM_Ptr;
		break;

	case FOAM_BVal_ListEmptyP:
		fintTypedEval(&expr1, FOAM_Ptr);
		retDataObj->fiBool = fiListEmptyP(expr1.fiPtr);
		myType = FOAM_Bool;
		break;

	case FOAM_BVal_ListHead:
		fintTypedEval(&expr1, FOAM_Ptr);
		retDataObj->fiWord = fiListHead(expr1.fiPtr);
		myType = FOAM_Word;
		break;

	case FOAM_BVal_ListTail:
		fintTypedEval(&expr1, FOAM_Ptr);
		retDataObj->fiPtr = fiListTail(expr1.fiPtr);
		myType = FOAM_Ptr;
		break;

	case FOAM_BVal_ListCons:
		fintTypedEval(&expr1, FOAM_Word);
		fintTypedEval(&expr2, FOAM_Ptr);
		retDataObj->fiPtr = fiListCons(expr1.fiWord, expr2.fiPtr);
		myType = FOAM_Ptr;
		break;

	default:
		bug("BCall: %s (<%s> in [%s]) unimplemented...\n",
		    foamBValInfo(call).str, prog->name, prog->unit->name);
		NotReached(myType = FOAM_Nil);
	}

	return myType;
	
      /**************************** END OF BCALL ***************************/
}

/* Execute the program inside the current tape in the environment related to
 * unit.
 * the return value is the type of the evaluated expression,
 * retDataObj is a pointer to the value 
 */

local dataType fintEval_(DataObj retDataObj);
local dataType
fintEval(DataObj retDataObj)
{
	fintDEBUG(fprintf(dbOut, "(fintEval:\n"));
	dataType d = fintEval_(retDataObj);
	fintDEBUG(fprintf(dbOut, " fintEval)"));

	return d;
}

local dataType
fintEval_(DataObj retDataObj)
{
        extern int isatty(int);
	int 		fmt, tag, argc;
	long 		n;
	union dataObj 	expr;
	dataType	type, myType = FOAM_NOp;
	
	fintGetTagFmtArgc(tag, fmt, argc);

	fintDEBUG(fprintf(dbOut, "fintEval: %s\n", foamInfo(tag).str);); 

	switch (tag) {

	case FOAM_EEnsure:
        case FOAM_OCall:
	case FOAM_CCall: {
		union dataObj	par;
		dataType	retType;
		UByte		denv;
		ProgInfo	prog0;
		FiEnv		env;

		DataObj		sp0;
		DataObj		oldStack;
		int		nFluids;
		
		/* This is a bad place for this, but f. call seems to
		 * be a horrendous operation 
		 */
		if (tag == FOAM_EEnsure) {
			retType = FOAM_NOp;
			fintTypedEval(&expr, FOAM_Env);
			if ((FiEnv)(expr.fiEnv->info) == (FiEnv) NULL) {
				myType = FOAM_NOp;
				return myType;
			}
			prog0 = (ProgInfo)((FiClos) expr.fiEnv->info)->prog;
			env   = ((FiClos) expr.fiEnv->info)->env;
			argc  = 0;
		}
		else if (tag == FOAM_CCall) {
			fintGetByte(retType); 	/* return type */
			type = fintEval(&expr);
			hardAssert((type == FOAM_Clos ||
			       type == FOAM_Word ) &&
			       (expr.fiClos != (FiClos) 0));
			prog0 = (ProgInfo) expr.fiClos->prog;
			env = expr.fiClos->env;
			argc -= 2;
		}
		else {
			fintGetByte(retType); 	/* return type */
			fintTypedEval(&expr, FOAM_Prog);
			prog0 = expr.progInfo;
			fintTypedEval(&expr, FOAM_Env);
			env = expr.fiEnv;
			argc -= 3;
		}

		hardAssert(argc >= 0);

		
		stackAlloc(sp0, argc + PAR_OFFSET);
		oldStack = stack;
		
		fintDEBUG(fintCheckCallStack());

		for (n = 0; n < argc; n++) {
			type = fintEval(&par);
			fintSet(type, sp0 + n + PAR_OFFSET,  par);
		}

		sp = sp0;
		stack = oldStack;

		stackFrameAlloc(argc);    /* creates a frame for argc param. */
		
		hardAssert(sp < stack + STACK_SIZE + 1);

		fintDEBUG({int k;
			  if (tag == FOAM_CCall)
			  	(void)fprintf(dbOut, "((CCall ");
			  else
			  	(void)fprintf(dbOut, "((OCall ");
			   (void)fprintf(dbOut, "to %s in %s with par: ",
				   prog0->name,
				   prog0->unit->name);
			   for (k = 0; k < argc; k++)
			   	(void)fprintf(dbOut, "%p ", parValue(k).fiPtr);
			   (void)fprintf(dbOut, ")\n");
		   })

		stackFrameIp(bp) = ip;

		/* set globals and env for the call */
		prog = prog0;
		unit = prog->unit;
		tape = fintUnitTape(unit);
		labels = progInfoLabels(prog);
		labelFmt = progInfoLabelFmt(prog);
		ip = progInfoSeq(prog);

		if (progInfoLocsCount(prog))
			stackAlloc(locValues, progInfoLocsCount(prog));
		
		nFluids = progInfoDFluidsCount(prog);
		
		if (nFluids)
			fintPushFluids(nFluids);

		denv = progInfoDEnv(prog)[0];

		if (fintUnitLexsCount(unit, denv)) {
		      lev0 = fintAlloc(union dataObj, 
				       fintUnitLexsCount(unit, denv));
		}
		else
		      lev0 = NULL;

        	fintEnvPush(lexEnv, lev0, env);

		(void)fintStmt(retDataObj);
		softAssert(retType == progInfoRetType(prog));

		fintDEBUG(fintCheckCallStack());
		
		if (nFluids) 
			fiGlobalFluidStack = 
				(FiFluidStack) fluidValue(nFluids);

		stackFrameFree();
		fintDEBUG(fintCheckCallStack());
		fintDEBUG(printf(" call returns %p)", (char*) retDataObj->fiSInt));

		return retType;
	}

	case FOAM_Glo: 
		fintGetInt(fmt, n);

		hardAssert(n < fintUnitGlobsCount(unit));
		
		fintSet(globType(n), retDataObj, globValue(n));

		myType = globType(n);
		
		return myType;

	case FOAM_Loc:
		fintGetInt(fmt, n);

		hardAssert(progInfoFmtLoc(prog) && 
		       n < progInfoLocsCount(prog));

		fintSet(locType(n), retDataObj, locValue(n));

		myType = locType(n);

		return myType;

	case FOAM_Par:
		fintGetInt(fmt, n);

		hardAssert(progInfoFmtPar(prog) && 
		       n < progInfoParsCount(prog));

		fintSet(parType(n), retDataObj, parValue(n));

		myType = parType(n);

		return myType;

	case FOAM_Lex: {
		int lev, type;
		DataObj  pLev;

		fintGetInt(fmt, lev);
		fintGetInt(fmt, n);
		fintDEBUG(printf("(Lex %d %d)", (int) lev, (int) n));
		switch (lev) {
			case 0: type = lexType(lev, n);
				fintSet(type, retDataObj, lev0[n]);
				myType = type;
				return myType;

			case 1: pLev = ((DataObj) lexEnv->next->level);
				break;
			case 2: pLev = ((DataObj) lexEnv->next->next->level);
				break;
			case 3: pLev = ((DataObj) lexEnv->next->next->next->level);
				break;
			case 4: pLev = ((DataObj) lexEnv->next->next->next->next->level);
				break;
			default: {
				FiEnv e = lexEnv;
				int j;

				for (j = 0; j < lev; j++)
					e = e->next;
				pLev = (DataObj) e->level;
				break;
			}
			}

		hardAssert(lev < progInfoDEnvsCount(prog));

		type = lexType(lev, n);
		fintSet(type, retDataObj, pLev[n]);

		myType = type;
			
		return myType;
	}

	case FOAM_Fluid: {
		FiFluid		afluid;
		fintGetInt(fmt, n);

		hardAssert(n < fintUnitFluidsCount(unit));

		afluid = fiGetFluid(fluidId(n));
		expr.fiWord = fiFluidValue(afluid);

		fintSet(fluidType(n), retDataObj, expr);

		myType = fluidType(n);

		return myType;
	}
/* !! */
	case FOAM_Env: {
		int	lev;

		fintGetInt(fmt, lev);

		switch (lev) {
			case 0: retDataObj->fiEnv = lexEnv;
				break;
			case 1: retDataObj->fiEnv = lexEnv->next;
				break;
			case 2: retDataObj->fiEnv = lexEnv->next->next;
				break;
			case 3: retDataObj->fiEnv = lexEnv->next->next->next;
				break;
			case 4: retDataObj->fiEnv = lexEnv->next->next->next->next;
				break;
			default: {
				FiEnv e = lexEnv;
				int j;

				for (j = 0; j < lev; j++)
					e = e->next;
				retDataObj->fiEnv = e;
				break;
			}
			}

		myType = FOAM_Env;

		return myType;
	}

	case FOAM_CEnv: {
		union dataObj		clos;

		fintTypedEval(&clos, FOAM_Clos);
		retDataObj->fiEnv = clos.fiClos->env;
		myType = FOAM_Env;
		break;
	}

	case FOAM_CProg: {
		union dataObj		clos;

		fintTypedEval(&clos, FOAM_Clos);
		retDataObj->fiProgPos = (FiProgPos) clos.fiClos->prog;
		myType = FOAM_Prog;
		break;
	}

	case FOAM_EInfo: {
		union dataObj		env;

		fintTypedEval(&env, FOAM_Env);
		retDataObj->fiWord = (FiWord) (env.fiEnv->info);
		myType = FOAM_Word;
		break;
	}


	case FOAM_EEnv: {
		int	n;
		union dataObj	env;
		FiEnv	e;

		fintGetInt(fmt, n);
		fintTypedEval(&env, FOAM_Env);
		for (e = env.fiEnv; n; n--, e = e->next)
			hardAssert(e);
		retDataObj->fiEnv = e;
		myType = FOAM_Env;
		break;
	}

	case FOAM_PRef: {
		int 		n;
		union dataObj		expr;
		/* Luckily, fiProgGetInfo does not look too carefully at its arg */
		fintGetInt(fmt, n);
		assert(n==0);
		fintTypedEval(&expr, FOAM_Prog);
		retDataObj->fiWord = (FiWord) expr.progInfo->_progInfo;
		myType = FOAM_SInt;
		break;
	}
/* !! */
	case FOAM_PushEnv:
		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_Env);
		retDataObj->fiEnv = (FiEnv) fintAlloc(struct _FiEnv, 1);
		retDataObj->fiEnv->level = (Ptr)
			fintAlloc(union dataObj, fintUnitLexsCount(unit, n));
		retDataObj->fiEnv->next = expr.fiEnv;
		retDataObj->fiEnv->info = (FiWord) NULL;

		myType = FOAM_Env;
		break;

	case FOAM_Clos: {
		union dataObj		env;
		union dataObj		prog0;

		fintTypedEval(&env, FOAM_Env);
		fintTypedEval(&prog0, FOAM_Prog);

		fintClosMake(retDataObj->fiClos, env, prog0); 

		myType = FOAM_Clos;
		return myType;
	}
	case FOAM_Const: 
		fintGetInt(fmt, n);

		hardAssert(n < fintUnitConstsCount(unit));

		fintSet(constType(n), retDataObj, constValue(n));

		myType = constType(n);

		return myType;

	case FOAM_Catch: {
		union dataObj	clos;
		union dataObj	nuffin;
		union dataObj  *rets;
		FiBool 		ok;
		FiWord		val, exn = 0;


		/* Evaluate the catch expression */
		fintTypedEval(&clos, FOAM_Clos);
		(void)fintEval(&nuffin); /* ignore */


		/* Check that old-style catches are gone */
		hardAssert(fintCurrentFormat != emptyFormatSlot);


		/* Decide which type of try-block this is. */
		if (fintUnitLexsCount(unit,fintCurrentFormat) == 3) {
			/* catch-with-value */
			fintBlock(ok, val, exn, fintDoCall0(&clos, retDataObj));
			rets = fintAlloc(union dataObj, 3);
			rets[0].fiSInt  = ok;
			rets[1].fiWord  = retDataObj->fiWord;
			rets[2].fiWord  = exn;
			retDataObj->ptr = rets;
		}
		else {
			/* catch-with-no-value */
			fintVoidBlock(ok, exn, fintDoCall0(&clos, retDataObj));
			rets = fintAlloc(union dataObj, 2);
			rets[0].fiSInt  = ok;
			rets[1].fiWord  = exn;
			retDataObj->ptr = rets;
		}


		/* Tell the caller where we've put our value */
		myType = FOAM_Ptr;
		break;
	}
	case FOAM_Cast: {
		/*
		 * The job of the FOAM cast instruction is to change the
		 * type under which a value is viewed. It is important
		 * that this operation DOES NOT alter the information
		 * associated with the value unless it cannot be avoided.
		 * Some loss of data is inevitable if we view DFlos as
		 * SFlos or SInts as Bools.
		 *
		 * For C-like coercions with information loss, such as
		 *
		 *     anInt  = (int)aDFlo;
		 *     anSFlo = (float)anInt;
		 *
		 * we use the built-in convert() operations instead of
		 * pretend. The FOAM cast instruction is not used for
		 * this kind of value manipulation.
		 */
		dataType	frType, toType;

		fintGetByte(toType);
		
		frType = fintEval(&expr);

		switch ((int)frType) {
		case FOAM_SFlo:
			switch ((int)toType) {
			case FOAM_SFlo:
				retDataObj->fiSFlo = expr.fiSFlo;
				break;
			case FOAM_DFlo:
				retDataObj->fiDFlo = expr.fiSFlo;
				break;
			case FOAM_SInt:
			case FOAM_Word:
				retDataObj->fiSInt = fiSFloToWord(expr.fiSFlo);
				break;
			case FOAM_HInt:
				retDataObj->fiHInt = (FiHInt) expr.fiSFlo;
				break;
			default:
				bug("FintEval: Cast from %d to %d unimplemented.", frType, toType);
			}
			break;
		case FOAM_DFlo:
			switch ((int)toType) {
			case FOAM_DFlo:
				retDataObj->fiDFlo = expr.fiDFlo;
				break;
			case FOAM_SFlo:
				retDataObj->fiSFlo = (FiSFlo) expr.fiDFlo;
				break;
			case FOAM_SInt:
			case FOAM_Word:
				retDataObj->fiSInt = (FiWord)
					fiBoxDFlo(expr.fiDFlo);
				break;
			case FOAM_HInt:
				retDataObj->fiHInt = (FiHInt) expr.fiDFlo;
				break;
			default:
				bug("FintEval: Cast from %d to %d unimplemented.", frType, toType);
			}
			break;
		case FOAM_SInt:
			switch ((int)toType) {
			case FOAM_Word:
				retDataObj->fiWord = expr.fiSInt;
#if AXL_EDIT_1_1_12p6_18
				goto castDone;
#endif
			case FOAM_SFlo:
				retDataObj->fiSFlo = fiWordToSFlo(expr.fiSInt);
				goto castDone;
			case FOAM_DFlo:
				retDataObj->fiDFlo = fiUnBoxDFlo(expr.fiSInt);
				goto castDone;
			}
			/* NO BREAK !*/
		case FOAM_Word:
			switch ((int)toType) {
			case FOAM_SInt:
				retDataObj->fiSInt = expr.fiWord;
#if AXL_EDIT_1_1_12p6_18
				goto castDone;
#endif
			case FOAM_SFlo:
				retDataObj->fiSFlo = fiWordToSFlo(expr.fiSInt);
				goto castDone;
			case FOAM_DFlo:
				retDataObj->fiDFlo = fiUnBoxDFlo(expr.fiSInt);
				goto castDone;
			}
			/* NO BREAK !*/
		default:{
			int frSize = 0, toSize = 0;

			hardAssert(toType != FOAM_SFlo &&
			       toType != FOAM_DFlo);
			if (frType == FOAM_NOp && toType== FOAM_NOp) break;
			fintGetTypeSize(frSize, frType);
			fintGetTypeSize(toSize, toType);

			/* $$ !! not portable */
			if (frSize == sizeof(FiChar)) {
				if (toSize == sizeof(FiChar)) 
					retDataObj->fiChar = expr.fiChar;
				
				else if (toSize == sizeof(FiHInt))
					retDataObj->fiHInt = expr.fiChar;
				else if (toSize == sizeof(FiWord))
					retDataObj->fiWord = expr.fiChar;
				else
					bug("FintEval: Cast from %d to %d unimplemented.", frType, toType);
				
			}
			else if (frSize == sizeof(FiHInt)) {
				if (toSize == sizeof(FiChar))
					retDataObj->fiChar = expr.fiHInt;
				else if (toSize == sizeof(FiHInt)) 
					retDataObj->fiHInt = expr.fiHInt;

				else if (toSize == sizeof(FiWord))
					retDataObj->fiWord = expr.fiHInt;
				else
					bug("FintEval: Cast from %d to %d unimplemented.", frType, toType);
			}
			else if (frSize == sizeof(FiWord)) {
				if (toSize == sizeof(FiChar))
					retDataObj->fiChar = (FiChar) expr.fiWord;
				else if (toSize == sizeof(FiHInt))
					retDataObj->fiHInt = (FiHInt) expr.fiWord;
				else if (toSize == sizeof(FiWord)) 
					retDataObj->fiWord = expr.fiWord;
				
				else
					bug("FintEval: Cast from %d to %d unimplemented.", frType, toType);
			}
			else
				bug("FintEval: Cast from %d to %d unimplemented.", frType, toType);

			break;
			}
		}
	castDone:
		myType = toType;

		return myType;
	}
	      
	case FOAM_Nil: {
		retDataObj->_fiNil = (FiNil) 0;
		myType = FOAM_Nil;
		return myType;
	}

	case FOAM_Char:
	case FOAM_Byte:
		fintGetByte(n);
		retDataObj->fiByte = (FiByte) n;
		myType = tag;
		return myType;

	case FOAM_Bool:
		fintGetByte(n);
		retDataObj->fiBool = (FiByte) n;
		myType = tag;
		return myType;

	case FOAM_HInt: 
		fintGetHInt(n);
		retDataObj->fiHInt = (FiHInt)n;
		/* fintGetHInt(retDataObj->fiHInt); */
		myType = FOAM_HInt;
		return myType;

	case FOAM_SInt:
		fintGetSInt(retDataObj->fiSInt);
		myType = FOAM_SInt;
		return myType;

	case FOAM_BInt: {
		Bool	neg;
		U16     static_data[4];
		U16     *data;
		int	slen, bi;
		long n;

		fintGetByte(neg);
		fintGetInt(fmt, slen);
		data = (slen <= 4) ? static_data :
			(U16*) stoAlloc(OB_Other, slen*sizeof(int));

		for (bi = 0; bi < slen; bi++) {
			fintGetHInt(n);
			data[bi] = (U16)n;
		}
		retDataObj->fiBInt = (FiBInt) bintFrPlacevS((Bool) neg, 
							    slen, data);
		if (slen > 4)
			stoFree(data);
		myType = FOAM_BInt;
		return myType;
	}

	case FOAM_SFlo:
		retDataObj->fiSFlo = (FiSFlo) fintRdSFloat();
		myType = FOAM_SFlo;
		return myType;

	case FOAM_DFlo:
		retDataObj->fiDFlo = (FiDFlo) fintRdDFloat();
		myType = FOAM_DFlo;
		return myType;

	case FOAM_BCall:
		myType = fintEvalBCall(retDataObj);
		return myType;
	
       case FOAM_Arr: {
		int i;

		fintGetByte(type);
		argc--;
		
		switch ((int)type) {
		
		case FOAM_Char:
			retDataObj->fiArr = (Ptr) fiArrNew_Char(argc+1);
			break;

/* Now we create only array of chars */
#ifdef FALSE
		case FOAM_Bool:
			retDataObj->fiArr = (Ptr) fiArrNew_Bool(argc+1);
			break;
		case FOAM_Byte:
			retDataObj->fiArr = (Ptr) fiArrNew_Byte(argc+1);
			break;
		case FOAM_HInt:
			retDataObj->fiArr = (Ptr) fiArrNew_HInt(argc+1);
			break;
		case FOAM_SInt:
			retDataObj->fiArr = (Ptr) fiArrNew_SInt(argc+1);
		case FOAM_SFlo:
			retDataObj->fiArr = (Ptr) fiArrNew_SFlo(argc+1);
			break;
		case FOAM_DFlo:
			retDataObj->fiArr = (Ptr) fiArrNew_DFlo(argc+1);
			break;
		case FOAM_Word:
			retDataObj->fiArr = (Ptr) fiArrNew_Word(argc+1);
			break;
#endif
		default:
			bug("fintEval: array of type %d unimplemented", type);
		}

		for (n = 0; n < argc; n++) {
			fintGetSInt(i);
			/* fintASetElem(type, retDataObj, n, expr); !!*/
			((char *)(retDataObj->fiArr))[n] = charFrAscii(i);
		}
		((char *)(retDataObj->fiArr))[argc] = '\0';     
		
		myType = FOAM_Arr;
		break;
	}

	case FOAM_ANew:
		fintGetByte(type);
		fintTypedEval(&expr, FOAM_SInt);

		switch ((int)type) {
		case FOAM_Char:
			retDataObj->fiArr = (Ptr) fiArrNew_Char(expr.fiSInt);
			break;
		case FOAM_Bool:
			retDataObj->fiArr = (Ptr) fiArrNew_Word(expr.fiSInt);   /* $$ !! WORD */
			break;
		case FOAM_Byte:
			retDataObj->fiArr = (Ptr) fiArrNew_Byte(expr.fiSInt);
			break;
		case FOAM_HInt:
			retDataObj->fiArr = (Ptr) fiArrNew_HInt(expr.fiSInt);
			break;
		case FOAM_SInt:
			retDataObj->fiArr = (Ptr) fiArrNew_SInt(expr.fiSInt);
			break;
		case FOAM_SFlo:
			retDataObj->fiArr = (Ptr) fiArrNew_SFlo(expr.fiSInt);
			break;
		case FOAM_DFlo:
			retDataObj->fiArr = (Ptr) fiArrNew_DFlo(expr.fiSInt);
			break;
		case FOAM_Word:
			retDataObj->fiArr = (Ptr) fiArrNew_Word(expr.fiSInt);
			break;
		default:
			bug("fintEval: array of type %d unimplemented", type);
		}

		myType = FOAM_Arr;
		break;

	case FOAM_AElt: {
		union dataObj		elem;

		fintGetByte(type);
		fintTypedEval(&elem, FOAM_SInt);
		fintTypedEval(&expr, FOAM_Arr);

		fintAGetElem(type, retDataObj, &expr, elem.fiSInt);

		myType = type;

		break;
        }

	case FOAM_RRElt: {
		bug("fintEval: FOAM_RRElt not implemented");
		break;
	}

	case FOAM_RRNew: {
		bug("fintEval: FOAM_RRNew not implemented");
		break;
	}

	case FOAM_RNew: {
		long i, bSize;

		fintGetInt(fmt, n);

		hardAssert(fintUnitLexsCount(unit, n));
		
		retDataObj->fiRec = (FiRec)
			fintAlloc(union dataObj, fintUnitLexsCount(unit,n));

		bSize = sizeof(union dataObj) * fintUnitLexsCount(unit, n);
		for (i = 0; i < bSize; i++)
			 ((UByte *) retDataObj->fiRec)[i] = 0;

		myType = FOAM_Rec;
		break;
	}
	case FOAM_RElt: {
		int slot;

		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_Rec);
		fintGetInt(fmt, slot);

		type = fmtType0(n, slot);

		assert((DataObj)(expr.fiRec) != NULL);

		fintSet(type, retDataObj, ((DataObj)(expr.fiRec))[slot]);

		myType = type;

		break;
	}

	case FOAM_TRNew: {
		int isz, asz, sz;
		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_SInt);
		hardAssert(fintUnitLexsCount(unit, n));
		
		sz  = fintUnitLexsCount(unit,n) - 1;
		isz = lexFormat(n, int0);
		asz = sz - isz;
		retDataObj->fiTR = (FiTR)
			fintAlloc(union dataObj, isz + asz * expr.fiSInt);
		
		myType = FOAM_TR;
		break;
	}
	case FOAM_IRElt: {
		int slot;

		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_TR);
		fintGetInt(fmt, slot);

		type = fmtType0(n, slot + 1);

		assert((DataObj)(expr.fiTR) != NULL);

		fintSet(type, retDataObj, ((DataObj)(expr.fiTR))[slot]);

		myType = type;

		break;
	}

	case FOAM_TRElt: {
		union dataObj idx;
		long slot;
		int isz, asz, sz;
		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_TR);
		fintTypedEval(&idx, FOAM_SInt);
		fintGetInt(fmt, slot);
		sz  = fintUnitLexsCount(unit,n) - 1;
		isz = lexFormat(n, int0);
		asz = sz - isz;

		type = fmtType0(n, slot+asz+1);
		slot = isz + asz*idx.fiSInt + slot;
		fintSet(type, retDataObj, ((DataObj)(expr.fiTR))[slot]);

		myType = type;

		break;
	}

	case FOAM_EElt: {
		int format, lev, n, i;
		FiEnv		env;

		fintGetInt(fmt, format);
		fintTypedEval(&expr, FOAM_Env);
		fintGetInt(fmt, lev);
		fintGetInt(fmt, n);

		env = expr.fiEnv;
		for (i = 0; i < lev; i++) {
			env = env->next;
			hardAssert(env);
		}

		type = fmtType0(format, n);
		/* !! now the offset is ignored */
		fintSet(type, retDataObj, ((DataObj)(env->level))[n]);

		myType = type;
		
		break;
	}

	case FOAM_MFmt: {
		/* Save the existing format */
		int format = fintCurrentFormat;

		fintGetInt(fmt, fintCurrentFormat);
		fintTypedEval(&expr, FOAM_NOp);
		retDataObj->mFmt = (MFmt) stoAlloc(OB_Other, sizeof(struct mFmt));

		retDataObj->mFmt->fmt = fintCurrentFormat;
		retDataObj->mFmt->values = expr.ptr;
		myType = FOAM_NOp;

		/* Restore the old format */
		fintCurrentFormat = format;
		break;
	}
	case FOAM_Values:
		if (argc) {
			retDataObj->ptr = fintAlloc(union dataObj, argc);

			for (n = 0; n < argc; n++)
				(void)fintEval(retDataObj->ptr + n);
		}
		else
			retDataObj->ptr = 0;

		myType = FOAM_NOp;

		break;

	case FOAM_PCall: {
		union dataObj	expr1, expr2, expr3, expr4;
		fintGetInt(fmt, n);  /* protocol */
		fintGetByte(type);
		(void)fintEval(&expr);

		switch ((int)expr.fiSInt) {
		case FINT_FOREIGN_fputs:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Word);
			retDataObj->fiWord = (FiWord)
			    fputs((char *) expr1.fiArr, (FILE *) expr2.fiWord);
			break;
		case FINT_FOREIGN_fputss:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_SInt);
			fintTypedEval(&expr3, FOAM_SInt);
			fintTypedEval(&expr4, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
			    fputss((char *) expr1.fiArr,
			    (int)expr2.fiSInt,(int) expr3.fiSInt, (FILE *) expr4.fiWord);
			break;
		case FINT_FOREIGN_fgetss:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_SInt);
			fintTypedEval(&expr3, FOAM_SInt);
			fintTypedEval(&expr4, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
			    fgetss((char *) expr1.fiArr,
			    (int)expr2.fiSInt, (int)expr3.fiSInt, (FILE *) expr4.fiWord);
			break;
		case FINT_FOREIGN_isatty:
			fintTypedEval(&expr1, FOAM_SInt);
			retDataObj->fiSInt = (FiSInt) isatty((int)expr1.fiSInt);
			break;
		case FINT_FOREIGN_fileno:
			fintTypedEval(&expr1, FOAM_Ptr);
			retDataObj->fiSInt = (FiSInt) fileno((FILE *) expr1.fiPtr);
			break;
		case FINT_FOREIGN_stdinFile:
			retDataObj->fiWord = (FiWord) stdinFile();
			break;
		case FINT_FOREIGN_stdoutFile:
			retDataObj->fiWord = (FiWord) stdoutFile();
			break;
		case FINT_FOREIGN_stderrFile:
			retDataObj->fiWord = (FiWord) stderrFile();
			break;
		case FINT_FOREIGN_formatSInt:
			fintTypedEval(&expr1, FOAM_SInt);
 			retDataObj->fiWord =(FiWord)formatSInt(expr1.fiSInt);
			break;
		case FINT_FOREIGN_formatBInt:
			fintTypedEval(&expr1, FOAM_BInt);
 			retDataObj->fiWord =(FiWord)formatBInt(expr1.fiBInt);
			break;
		case FINT_FOREIGN_formatSFloat:
			fintTypedEval(&expr1, FOAM_SFlo);
 			retDataObj->fiWord =(FiWord)formatSFloat(expr1.fiSFlo);
			break;
		case FINT_FOREIGN_formatDFloat:
			fintTypedEval(&expr1, FOAM_DFlo);
 			retDataObj->fiWord =(FiWord)formatDFloat(expr1.fiDFlo);
			break;

		/* Old debugging system */
		case FINT_FOREIGN_fiGetDebugVar:
			retDataObj->fiWord = fiGetDebugVar();
			break;
		case FINT_FOREIGN_fiSetDebugVar:
			fintTypedEval(&expr1, FOAM_Word);
			fiSetDebugVar(expr1.fiWord);
			retDataObj->fiWord = (FiWord) 0;
			break;

		/* New debugging system */
		case FINT_FOREIGN_fiGetDebugger:
			fintTypedEval(&expr1, FOAM_SInt);
			retDataObj->fiClos =
				(FiClos)fiGetDebugger(expr1.fiSInt);
			break;
		case FINT_FOREIGN_fiSetDebugger:
			fintTypedEval(&expr1, FOAM_SInt);
			fintTypedEval(&expr2, FOAM_Clos);
			fiSetDebugger(expr1.fiSInt, (FiWord)expr2.fiClos);
			retDataObj->fiWord = (FiWord) 0;
			break;

		case FINT_FOREIGN_fputc:
			fintTypedEval(&expr1, FOAM_SInt);
			fintTypedEval(&expr2, FOAM_Word);
			retDataObj->fiWord = (FiWord)
			    fputc((int)expr1.fiSInt, (FILE *) expr2.fiWord);
			break;

		case FINT_FOREIGN_sqrt:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    sqrt(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_pow:
			fintTypedEval(&expr1, FOAM_DFlo);
			fintTypedEval(&expr2, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    pow(expr1.fiDFlo, expr2.fiDFlo);
			break;

		case FINT_FOREIGN_log:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    log(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_log10:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    log10(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_exp:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    exp(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_sin:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    sin(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_cos:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    cos(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_tan:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    tan(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_sinh:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    sinh(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_cosh:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    cosh(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_tanh:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    tanh(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_asin:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    asin(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_acos:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    acos(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_atan:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    atan(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_atan2:
			fintTypedEval(&expr1, FOAM_DFlo);
			fintTypedEval(&expr2, FOAM_DFlo);
			retDataObj->fiDFlo = (FiDFlo)
			    atan2(expr1.fiDFlo, expr2.fiDFlo);
			break;

		case FINT_FOREIGN_fopen:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Arr);
			retDataObj->fiWord = (FiWord)
			    fopen((char *)expr1.fiArr, (char *)expr2.fiArr);
			break;

		case FINT_FOREIGN_fclose:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
			    fclose((FILE*)expr1.fiWord);
			break;

		case FINT_FOREIGN_fflush:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
			    fflush((FILE*)expr1.fiWord);
			break;

		case FINT_FOREIGN_fgetc:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
			    fgetc((FILE *)expr1.fiWord);
			break;

		case FINT_FOREIGN_fseek:
			fintTypedEval(&expr1, FOAM_Word);
			fintTypedEval(&expr2, FOAM_Word);
			fintTypedEval(&expr3, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
				fseek((FILE *)expr1.fiWord,
					expr2.fiWord,
					expr3.fiWord);
			break;

		case FINT_FOREIGN_ftell:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)ftell((FILE *)expr1.fiWord);
			break;

		case FINT_FOREIGN_strLength:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    strLength((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_fiStrHash:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    strHash((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_fiDoubleHexPrintToString:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiWord = (FiWord)
			    fiDoubleHexPrintToString(expr1.fiWord);
			break;

		case FINT_FOREIGN_fiInitialiseFpu:
		        fiInitialiseFpu();
			/* we lie ... */
			retDataObj->fiSInt = 0;
			break;

		case FINT_FOREIGN_fiIeeeGetRoundingMode:
			retDataObj->fiWord = (FiWord)
			    fiIeeeGetRoundingMode();
			break;

		case FINT_FOREIGN_fiIeeeSetRoundingMode:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiWord = (FiWord)
			    fiIeeeSetRoundingMode(expr1.fiWord);
			break;

		case FINT_FOREIGN_fiIeeeGetExceptionStatus:
			retDataObj->fiWord = (FiWord)
			    fiIeeeGetExceptionStatus();
			break;

		case FINT_FOREIGN_fiIeeeSetExceptionStatus:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiWord = (FiWord)
			    fiIeeeSetExceptionStatus(expr1.fiWord);
			break;

		case FINT_FOREIGN_fiIeeeGetEnabledExceptions:
			retDataObj->fiWord = (FiWord)
			    fiIeeeGetEnabledExceptions();
			break;

		case FINT_FOREIGN_fiIeeeSetEnabledExceptions:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiWord = (FiWord)
			    fiIeeeSetEnabledExceptions(expr1.fiWord);
			break;

		case FINT_FOREIGN_fiDFloMantissa:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiBInt = (FiBInt)
			    fiDFloMantissa(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_fiDFloExponent:
			fintTypedEval(&expr1, FOAM_DFlo);
			retDataObj->fiBInt = (FiBInt)
			    fiDFloExponent(expr1.fiDFlo);
			break;

		case FINT_FOREIGN_fiSFloMantissa:
			fintTypedEval(&expr1, FOAM_SFlo);
			retDataObj->fiBInt = (FiBInt)
			    fiSFloMantissa(expr1.fiSFlo);
			break;

		case FINT_FOREIGN_fiSFloExponent:
			fintTypedEval(&expr1, FOAM_SFlo);
			retDataObj->fiBInt = (FiBInt)
			    fiSFloExponent(expr1.fiSFlo);
			break;


		/* ------ Operating System Interface ------- */

		case FINT_FOREIGN_osRun:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osRun((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osRunConcurrent:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Word);
			fintTypedEval(&expr3, FOAM_Word);
			fintTypedEval(&expr4, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
			    osRunConcurrent((String) expr1.fiArr,
					    (FILE **) expr2.fiWord,
					    (FILE **) expr3.fiWord,
					    (FILE **) expr4.fiWord);
			break;

		case FINT_FOREIGN_osRunQuoteArg:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Word);
			retDataObj->fiSInt = (FiSInt)
			    osRunQuoteArg((String) expr1.fiArr,
					    (int (*)(int)) expr2.fiWord);
			break;

		case FINT_FOREIGN_osCpuTime:
			retDataObj->fiSInt = (FiSInt) osCpuTime();
			break;

		case FINT_FOREIGN_osDate:
			retDataObj->fiArr = (FiArr) osDate();
			break;

		case FINT_FOREIGN_osGetEnv:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiArr = (FiArr)
			    osGetEnv((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osPutEnv:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osPutEnv((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osPutEnvIsKept:
			retDataObj->fiBool = (FiBool)
				osPutEnvIsKept();
			break;

		case FINT_FOREIGN_osCurDirName:
			retDataObj->fiArr = (FiArr)
			    osCurDirName();
			break;
		case FINT_FOREIGN_osTmpDirName:
			retDataObj->fiArr = (FiArr)
			    osTmpDirName();
			break;

		case FINT_FOREIGN_osFnameDirEqual:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Arr);
			retDataObj->fiBool = (FiBool)
			    osFnameDirEqual((String) expr1.fiArr,
					    (String) expr2.fiArr);
			break;
		case FINT_FOREIGN_osSubdir:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Arr);
			fintTypedEval(&expr3, FOAM_Arr);
		    	osSubdir((String) expr1.fiArr,
				 (String) expr2.fiArr,
				 (String) expr3.fiArr);
			break;

		case FINT_FOREIGN_osSubdirLength:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osSubdirLength((String) expr1.fiArr,
					    (String) expr2.fiArr);
			break;

		case FINT_FOREIGN_osFnameParse:
			fintTypedEval(&expr1, FOAM_Word);
			fintTypedEval(&expr2, FOAM_Arr);
			fintTypedEval(&expr3, FOAM_Arr);
			fintTypedEval(&expr4, FOAM_Arr);
		    	osFnameParse((String *) expr1.fiArr,
				     (String) expr2.fiArr,
				     (String) expr3.fiArr,
				     (String) expr4.fiArr);
			break;

		case FINT_FOREIGN_osFnameParseSize:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osFnameParseSize((String) expr1.fiArr,
					     (String) expr2.fiArr);
			break;

		case FINT_FOREIGN_osFnameUnparse:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Word);
			fintTypedEval(&expr3, FOAM_Bool);
			retDataObj->fiSInt = (FiSInt)
			    osFnameUnparse((String) expr1.fiArr,
					    (String *) expr2.fiWord,
					   (Bool)expr3.fiBool);
			break;

		case FINT_FOREIGN_osFnameUnparseSize:
			fintTypedEval(&expr1, FOAM_Word);
			fintTypedEval(&expr2, FOAM_Bool);
			retDataObj->fiSInt = (FiSInt)
			    osFnameUnparseSize((String *) expr1.fiWord,
					       (Bool)expr2.fiBool);
			break;

		case FINT_FOREIGN_osFnameTempSeed:
			retDataObj->fiSInt = (FiSInt)
				osFnameTempSeed();
			break;
		case FINT_FOREIGN_osFnameTempDir:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiArr = (FiArr)
			    osFnameTempDir((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osIsInteractive:
			fintTypedEval(&expr1, FOAM_Word);
			retDataObj->fiBool = (FiBool)
			    osIsInteractive((FILE *) expr1.fiWord);
			break;

		case FINT_FOREIGN_osFileRemove:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osFileRemove((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osFileRename:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osFileRename((String) expr1.fiArr,
					    (String) expr2.fiArr);
			break;

		case FINT_FOREIGN_osFileIsThere:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiBool = (FiBool)
			    osFileIsThere((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osFileHash:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osFileHash((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osFileSize:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osFileSize((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osDirIsThere:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiBool = (FiBool)
			    osDirIsThere((String) expr1.fiArr);
			break;

		case FINT_FOREIGN_osDirSwap:
			fintTypedEval(&expr1, FOAM_Arr);
			fintTypedEval(&expr2, FOAM_Arr);
			fintTypedEval(&expr3, FOAM_SInt);

			retDataObj->fiSInt = (FiSInt)
			    osDirSwap((String) expr1.fiArr,
				      (String) expr2.fiArr,
				      (FiSInt) expr3.fiSInt);
			break;

		case FINT_FOREIGN_osIncludePath:
			retDataObj->fiArr = (FiArr)
			    osIncludePath();
			break;
		case FINT_FOREIGN_osLibraryPath:
			retDataObj->fiArr = (FiArr)
			    osLibraryPath();
			break;
		case FINT_FOREIGN_osExecutePath:
			retDataObj->fiArr = (FiArr)
			    osExecutePath();
			break;

		case FINT_FOREIGN_osPathLength:
			fintTypedEval(&expr1, FOAM_Arr);
			retDataObj->fiSInt = (FiSInt)
			    osPathLength((String) expr1.fiArr);
			break;
		case FINT_FOREIGN_osPathParse:
			fintTypedEval(&expr1, FOAM_Word);
			fintTypedEval(&expr2, FOAM_Arr);
			fintTypedEval(&expr3, FOAM_Arr);
			osPathParse((String *) expr1.fiWord,
				    (String) expr2.fiArr,
				    (String) expr3.fiArr);
			break;
		case FINT_FOREIGN_gcTimer: {
		  	extern void *gcTimer(void);
		  	retDataObj->fiWord = (FiWord) gcTimer();
			break;
			}
		case FINT_FOREIGN_fiRaiseException: {
			fintTypedEval(&expr1, FOAM_Word);
			fiRaiseException(expr1.fiWord);
			break;
			}
		case FINT_FOREIGN_osAllocShow: {
			osAllocShow();
			break;
		        }
		case FINT_FOREIGN_osAlloc: {
			fintTypedEval(&expr1, FOAM_Ptr);
			retDataObj->fiPtr = osAlloc((unsigned long *)expr1.fiPtr);
			break;
		        }
		case FINT_FOREIGN_osFree: {
			fintTypedEval(&expr1, FOAM_Ptr);
			osFree((FiPtr)expr1.fiPtr);
			break;
		        }
		case FINT_FOREIGN_osMemMap: {
			fintTypedEval(&expr1, FOAM_SInt);
		        retDataObj->fiArr = (FiArr)osMemMap((int)expr1.fiSInt);
			break;
		        }
		default: {
			AInt pcallId = expr.fiSInt;
			bug("fintEval: %s PCall %d %s, (called from <%s> in [%s])\n",
			    pcallId == -1 ?
			    "undeclared" : "unimplemented", pcallId,
			    (pcallId > 0 && pcallId < FINT_FOREIGN_END) ? fintForeignTable[pcallId].string : "??",
			    prog->name, prog->unit->name);
			    }
		}

		myType = type;
		break;
	}

	default:
		bug("fintEval: %s (<%s> in [%s]) unimplemented... (or Seq without Return)\n",
		    foamInfo(tag).str, prog->name, prog->unit->name);
		NotReached(myType = FOAM_Nil);
	}

	return myType;
}

/* Store in retDataObj a reference to a global, a local, a lexVar, ... 
 * NOTE: here retDataType != NULL
 */
local dataType
fintGetReference(Ref pDataObj)
{
	int fmt, tag, n, argc;
	dataType myType = 0;

	fintGetTagFmt(tag, fmt);

	switch (tag) {
	case FOAM_Glo:
		fintGetInt(fmt, n);

		hardAssert(n < fintUnitGlobsCount(unit));
		
		*pDataObj = &(globValue(n));
		myType = globType(n);
		break;

	case FOAM_Loc: 

		fintGetInt(fmt, n);

		hardAssert(progInfoFmtLoc(prog) && 
		       n < progInfoLocsCount(prog));

		*pDataObj = &(locValue(n));
		myType = locType(n);
		break;

	case FOAM_Par: 

		fintGetInt(fmt, n);

		hardAssert(progInfoFmtPar(prog) && 
		       n < progInfoParsCount(prog));

		*pDataObj = &(parValue(n));
		myType = parType(n);
		break;

	case FOAM_Lex: {
		int lev;
		DataObj  pLev;

		fintGetInt(fmt, lev);
		fintGetInt(fmt, n);

		switch (lev) {
			case 0: *pDataObj = lev0 + n;
				myType = lexType(int0, n);
				return myType;

			case 1: pLev = ((DataObj) lexEnv->next->level);
				break;
			case 2: pLev = ((DataObj) lexEnv->next->next->level);
				break;
			case 3: pLev = ((DataObj) lexEnv->next->next->next->level);
				break;
			case 4: pLev = ((DataObj) lexEnv->next->next->next->next->level);
				break;
			default: {
				FiEnv e = lexEnv;
				int j;

				for (j = 0; j < lev; j++)
					e = e->next;
				pLev = (DataObj) e->level;
				break;
			}
			}

		*pDataObj = pLev + n;

		myType = lexType(lev, n);
			
		return myType;
	}

	case FOAM_Fluid: {
		FiFluid		afluid;
		fintGetInt(fmt, n);

		hardAssert(n < fintUnitFluidsCount(unit));

		afluid = fiGetFluid(fluidId(n));
		
		*pDataObj = (DataObj) &(afluid->value);
		myType = fluidType(n);
		break;
	}

	case FOAM_Const: 

		fintGetInt(fmt, n);

		hardAssert(n < fintUnitConstsCount(unit));

		*pDataObj = &(constValue(n));
		myType = constType(n);

		break;

	case FOAM_AElt: {
		union dataObj		elem;
		union dataObj		expr;
		int		type;

		fintGetByte(type);
		fintTypedEval(&elem, FOAM_SInt);
		fintTypedEval(&expr, FOAM_Arr);

		fintAGetElemRef(type, *pDataObj, &expr, elem.fiSInt);

		myType = type;

		break;
	      }

	case FOAM_RRElt: {
		bug("fintGetReference: FOAM_RRElt not implemented");
		break;
	}

	case FOAM_RElt: {
		int 		slot;
		union dataObj		expr;

		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_Rec);
		fintGetInt(fmt, slot);

		assert((DataObj)(expr.fiRec)!=NULL);
		*pDataObj = ((DataObj)(expr.fiRec)) + slot;

		myType = fmtType0(n, slot);

		break;
	}

	case FOAM_IRElt: {
		int 		slot;
		union dataObj		expr;

		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_TR);
		fintGetInt(fmt, slot);
		

		assert((DataObj)(expr.fiTR) != NULL);
		*pDataObj = ((DataObj)(expr.fiTR)) + slot;

		myType = fmtType0(n, slot + 1);

		break;
	}
	case FOAM_TRElt: {
		long 		slot;
		union dataObj	expr, idx;
		int 		isz, asz, sz;

		fintGetInt(fmt, n);
		fintTypedEval(&expr, FOAM_TR);
		fintTypedEval(&idx, FOAM_SInt);
		fintGetInt(fmt, slot);
		sz  = fintUnitLexsCount(unit,n) - 1;
		isz = lexFormat(n, int0);
		asz = sz - isz;

		myType = fmtType0(n, slot + isz + 1);
		slot = isz + asz*idx.fiSInt + slot;

		assert((DataObj)(expr.fiTR) != NULL);
		*pDataObj = ((DataObj)(expr.fiTR)) + slot;


		break;
	}

	case FOAM_CEnv: {
		union dataObj		clos;

		fintTypedEval(&clos, FOAM_Clos);
		*pDataObj = (DataObj) &clos.fiClos->env;
		myType = FOAM_Env;
		break;
	}

	case FOAM_CProg: {
		union dataObj		clos;

		fintTypedEval(&clos, FOAM_Clos);
		*pDataObj = (DataObj) &clos.fiClos->prog;
		myType = FOAM_Prog;
		break;
	}

	case FOAM_EInfo: {
		union dataObj		env;

		fintTypedEval(&env, FOAM_Env);
		*pDataObj = (DataObj) &(env.fiEnv->info);
		myType = FOAM_Word;
		break;
	}

	case FOAM_EElt: {
		int format, lev, n, i;
		FiEnv		env;
		union dataObj		expr;
		dataType	type;

		fintGetInt(fmt, format);
		fintTypedEval(&expr, FOAM_Env);
		fintGetInt(fmt, lev);
		fintGetInt(fmt, n);

		env = expr.fiEnv;
		for (i = 0; i < lev; i++) {
			env = env->next;
			hardAssert(env);
		}

		type = fmtType0(format, n);
		/* !! now the offset is ignored */
		*pDataObj = ((DataObj)(env->level)) + n;

		myType = type;
		
		break;
	}
	case FOAM_PRef: {
		union dataObj		expr;
		int 		n;
		
		fintGetInt(fmt, n);
		assert(n==0);
		fintTypedEval(&expr, FOAM_Prog);
		/* $$!! NOW WILL NOT WORK */
		*pDataObj = (DataObj) &(expr.progInfo->_progInfo);
		myType = FOAM_SInt;
		break;
	}

	case FOAM_Values: {
		dataType	type;

		fintGetInt(fmt, argc);
		*pDataObj = fintAlloc(DataObj, argc);

		for (n = 0; n < argc; n++)
			type = fintGetReference((((Ref) *pDataObj) + n));

		myType = FOAM_NOp;
		break;
	}
	default:
		bug("reference to %d unimplemented...", tag);
		NotReached(myType = FOAM_Nil);
	}

	return myType;
}

local void
fintSetMFmt(DataObj ref, DataObj expr)
{
	int	i, n;
	int	fmt = expr->mFmt->fmt;
	DataObj	values = expr->mFmt->values;


	/* Hack, due to bad foam generation */
	if (!values) return;


	/* Check that old-style catch expressions are dead */
	hardAssert(fmt >= 0);


	/* How many fields in this multi? */
	n = fintUnitLexsCount(unit,fmt);


	/* Copy each value into its destination reference */
	for (i = 0; i < n; i++)
		fintSet(fmtType0(fmt, i), ((Ref) ref)[i], values[i]);


	/* Clean up */
	fintFree(values);
	fintFree(expr->mFmt);
}

local dataType
fintDoCall0(DataObj clos, DataObj retDataObj)
{
	return fintDoCall(clos, retDataObj, int0);
}

local dataType
fintDoCall1(DataObj clos, DataObj retDataObj, DataObj arg1)
{
	return fintDoCall(clos, retDataObj, 1, arg1);
}

local dataType
fintDoCall(DataObj clos, DataObj retDataObj, int argc, ...)
{
	DataObj *argv;
	dataType type;
	va_list argp;
	int i;

	argv = (DataObj *) stoAlloc(OB_Other, argc * sizeof(DataObj));

	va_start(argp, argc);
	for (i = 0; i<argc; i++) argv[i] = va_arg(argp, DataObj);
	va_end(argp);

	type = fintDoCallN(clos, retDataObj, argc, argv);
	stoFree(argv);

	return type;
}

local dataType
fintDoCallN(DataObj clos, DataObj retDataObj, int argc, DataObj *argv)
{
	ProgInfo prog0 = (ProgInfo) clos->fiClos->prog;
	FiEnv    env   = clos->fiClos->env;
	DataObj		sp0;
	DataObj		oldStack;
	int		nFluids, i;
	UByte		denv;

	stackAlloc(sp0, 0 + PAR_OFFSET);
	oldStack = stack;

	/* NB all arguments are words, right? */
	for (i = 0; i < argc; i++) 
		fintSet(FOAM_Word, sp0 + i + PAR_OFFSET, *argv[i]);

	sp = sp0;

	stackFrameAlloc(argc);    /* creates a frame for argc param. */
	hardAssert(sp < stack + STACK_SIZE + 1);

	stackFrameIp(bp) = ip;

	/* set globals and env for the call */
	prog 	= prog0;
	unit 	= prog->unit;
	tape 	= fintUnitTape(unit);
	labels 	= progInfoLabels(prog);
	labelFmt = progInfoLabelFmt(prog);
	ip 	= progInfoSeq(prog);

	if (progInfoLocsCount(prog))
		stackAlloc(locValues, progInfoLocsCount(prog));
		
	nFluids = progInfoDFluidsCount(prog);
		
	if (nFluids)
		fintPushFluids(nFluids);

	denv = progInfoDEnv(prog)[0];

	if (fintUnitLexsCount(unit, denv)) {
		lev0 = fintAlloc(union dataObj, 
				 fintUnitLexsCount(unit, denv));
	}
	else
		lev0 = NULL;

	fintEnvPush(lexEnv, lev0, env);

	(void)fintStmt(retDataObj);
		
	if (nFluids) 
		fiGlobalFluidStack = 
			(FiFluidStack) fluidValue(nFluids);

	stackFrameFree();

	return progInfoRetType(prog);
	
}

/*****************************************************************************
 *
 * :: Storage management
 *
 ****************************************************************************/

local void
unitFree(FintUnit unit)
{
	fmtGlobalsFree(unit);
	fmtConstantsFree(unit);
	fmtFluidsFree(unit);
	lexLevelsFree(unit);
}

local Fmt
fmtAlloc(int nDecls)
{
	if (nDecls)
		return (Fmt) fintAlloc(struct fmt, nDecls);
	return NULL;
}

local void
fmtFree(Fmt fmt)
{	
	if (fmt) {
		strFree(fmtId(fmt));
		stoFree(fmt);
	}
}

local void
fmtGlobalsFree(FintUnit unit)
{
	ShDataObj	* globValues = fintUnitGlobValues(unit);
	int		globCount = fintUnitGlobsCount(unit);
	int 		n;

	for (n = 0; n < globCount; n++)
		shDataObjFree(globValues[n]);

	fmtFree(fintUnitGlobs(unit));
	fintFree0(globValues);
}

local void
fmtConstantsFree(FintUnit unit)
{
	int n;

	for (n = 0; n < fintUnitConstsCount(unit); n++)
		if (constType(n) == FOAM_Prog) {
			ProgInfo p = constValue(n).progInfo;
			
			fintFree0(progInfoLabels(p));
			fintFree0(progInfoFmtLoc(p));
			fintFree0(progInfoFmtPar(p));
			fintFree0(progInfoDFluid(p));
			fintFree0(progInfoDEnv(p));

			fintFree(p);
		}
			

	fmtFree(fintUnitConsts(unit));
}

local void
fmtFluidsFree(FintUnit unit)
{
	fmtFree(fintUnitFluids(unit));
}

local void
lexLevelsFree(FintUnit unit)
{
	LexLevels	l = fintUnitLexLevels(unit);
	int 		levCount = fintUnitLexLevelsCount(unit);
	int		n;

	for (n = 0; n < levCount ; n++)
		fmtFree((Fmt) l->fmtLex);

	fintFree0(l);

}

local void
fintInitForeignGlobValue(DataObj retDataObj, int n)
{
	if (!fintForeignTable[n].isConst)
		retDataObj->fiSInt =(FiSInt) fintForeignTable[n].funct;
	else {
		switch(fintForeignTable[n].funct) {

		case FINT_FOREIGN_osIoRdMode:
			retDataObj->fiArr = (FiArr) osIoRdMode;
			break;
		case FINT_FOREIGN_osIoWrMode:
			retDataObj->fiArr = (FiArr) osIoWrMode;
			break;
		case FINT_FOREIGN_osIoApMode:
			retDataObj->fiArr = (FiArr) osIoApMode;
			break;
		case FINT_FOREIGN_osIoRbMode:
			retDataObj->fiArr = (FiArr) osIoRbMode;
			break;
		case FINT_FOREIGN_osIoWbMode:
			retDataObj->fiArr = (FiArr) osIoWbMode;
			break;
		case FINT_FOREIGN_osIoAbMode:
			retDataObj->fiArr = (FiArr) osIoAbMode;
			break;
		case FINT_FOREIGN_osIoRubMode:
			retDataObj->fiArr = (FiArr) osIoRubMode;
			break;
		case FINT_FOREIGN_osIoWubMode:
			retDataObj->fiArr = (FiArr) osIoWubMode;
			break;
		case FINT_FOREIGN_osIoAubMode:
			retDataObj->fiArr = (FiArr) osIoAubMode;
			break;

		case FINT_FOREIGN_osObjectFileType:
			retDataObj->fiArr = (FiArr) osObjectFileType;
			break;
		case FINT_FOREIGN_osExecFileType:
			retDataObj->fiArr = (FiArr) osExecFileType;
			break;

		case FINT_FOREIGN_osFnameNParts:
			retDataObj->fiSInt = (FiSInt) osFnameNParts;
			break;

		default:
			bug("fintForeignInitGlobalValue: bad foreign const.");
		}
	}
}

local ShDataObj
shDataObjAdd(AInt type, String id, int protocol, int globNum, FintUnit curUnit)
{
	int 		n = 0;
	ShDataObj 	new;
	Bool 		found = false;
	FintUnit	u = NULL;
	String 		id_orig = NULL;
	new = shDataObjFind(type, id, protocol);

	/* !! Hack: resolving globals we look also in the current unit.
	 * This because a bug in genfoam generate 2 globals with the
	 * same name in the same unit.
	 * Once the bug has been fixed, the globNum par. can be removed
	 * hack 03450
	 */

        if (new == NULL) {
		u = curUnit;
		for (n = 0; n < globNum; n++)
			if (!strcmp(id, fintUnitGlobs(u)[n].id)) {
				new = fintUnitGlobValues(u)[n];
				new->refCounter++;
				break;
			}
	}

	if (new != NULL) {
		fintLinkDEBUG((void)fprintf(dbOut, "(linked glob %s)\n", id););
		return new;
	}


	/*
	 * Create an new shared object initialised with the value
	 * -1 (see next paragraph). We expect that the object will
	 * be initialised with its correct value later on.
	 *
	 * The main exceptions to this are foreign imports (usually
	 * the target of a PCall). If we recognise the name of the
	 * identifer as a foreign import the shared object will be
	 * initialised with its index in fintForeignTable. Since 0
	 * is a valid index, we use -1 to indicate failure.
	 *
	 * Future work: keep the unresolved identifier names around
	 * somewhere handy so that fintEval can give a better bug
	 * report when it hits an unrecognised PCall.
	 */
	new = (ShDataObj) stoAlloc(OB_Other, sizeof(struct shDataObj));
	new->dataObj.fiWord = -1; /* Don't use 0 here!!! */
	new->refCounter = 1;

	/* Handle mangled names if necessary */
	if (protocol == FOAM_Proto_C) {
	  	String tmp = strchr(id, '-');
		if (tmp) {
		  	id_orig = id;
			id = strCopy(id);
			id[tmp - id_orig] = '\0';
		}
        }
	if (protocol == FOAM_Proto_Init)
		lazyLibGet(id);
	else {
		found = false;
		for (n=0; fintForeignTable[n].string; n++)
			if (id[0] == fintForeignTable[n].string[0] &&
			    !strcmp(id, fintForeignTable[n].string)) {
				found = true;
				break;
			}
			    
		if (found) {
		    fintInitForeignGlobValue(&(new->dataObj), n);
		    fintLinkDEBUG((void)fprintf(dbOut, "(Resolved foreign %s)\n",
			                fintForeignTable[n].string););
		} else {
		  	fintLinkDEBUG({printf("Could not resolve: %s\n", id);});
		}
	}

        if (id_orig) 
		strFree(id);	  
	
	return new;
}

local ShDataObj
shDataObjFind(AInt type, String id, int protocol)
{
	ShDataObj obj = NULL;
	FintUnitList	ul;
	FintUnit 	u;
	int m, n;

	/* NB: Should worry about protocol and check type */
 	for (ul = cdr(fintUnitList); ul; ul = cdr(ul)) { 
		u = car(ul);
		m = fintUnitGlobsCount(u);
		for (n = 0; n < m ; n++) {
			if (strcmp(id, fintUnitGlobs(u)[n].id) == 0) {
				obj = fintUnitGlobValues(u)[n];
				obj->refCounter++;
			}
		}
		if (obj) break;
	}
	return obj;
}

local ShDataObj
shDataObjFindBis(AInt type, String id, int protocol)
{
	ShDataObj obj = NULL;
	FintUnitList	ul;
	FintUnit 	u;
	int m, n;

	/* NB: Should worry about protocol and check type */
	/* this one looks in the current one too 
	 * see hack 03450
	 */
 	for (ul = fintUnitList; ul; ul = cdr(ul)) { 
		u = car(ul);
		m = fintUnitGlobsCount(u);
		for (n = 0; n < m ; n++) {
			if (strcmp(id, fintUnitGlobs(u)[n].id) == 0) {
				obj = fintUnitGlobValues(u)[n];
				obj->refCounter++;
			}
		}
		if (obj) break;
	}
	return obj;
}


local void
shDataObjFree(ShDataObj obj)
{
	if (obj->refCounter-- == 0) fintFree(obj);
}


local void
stackChain(int num)
{
	if (num > STACK_SIZE) bug("Stack Growth Excessive!");
	if (!stack[STACK_SIZE].ptr) {
		DataObj newStack = fintAlloc(union dataObj, STACK_SIZE + 1);

		fintLinkDEBUG(
		  (void)fprintf(dbOut,"Allocating a new stack of size %d\n",
			  STACK_SIZE);
		  );
		
		newStack[STACK_SIZE].ptr = 0;
		stack[STACK_SIZE].ptr = newStack;
		newStack[0].ptr = stack;
		newStack[1].ptr = sp;
		sp = newStack + 2;
		stack = newStack;
        }
        else {
		stack = stack[STACK_SIZE].ptr;
		stack[1].ptr = sp;
	    	sp = stack + 2;
	}

	return;
}

/****************************************************************************
 *
 * Unit loading procedures
 *
 ***************************************************************************/


/* Assumes that the next tag in tape is DDecl. Reads all the decls and put them
 * in the fmt passed. Returns the number of decls read.
 */
/* "tswi" or "tswibp" */
local int
fintReadFmt(Fmt * pFmt)
{
	int tag, fm, argc, n, i, slen;
	Fmt	fmt;

	fintGetTagFmtArgc(tag, fm, argc);
	hardAssert(tag == FOAM_DDecl);
	argc -= 1;
	fmt = fmtAlloc(argc);
	/* Usage field */
	fintGetByte(n);
	*pFmt = fmt;

	for (i = 0; i < argc; i++, fmt++) {
		/* Tag */
		fintGetTagFmt(tag, fm);
		hardAssert(tag == FOAM_Decl || tag == FOAM_GDecl);
		/* Type */
		fintGetByte(n);
		fmtType(fmt) = FOAM_START + n;		
		/* id */
		fintGetInt(fm, slen);
		fmtId(fmt) = fintRdChars(slen);
		/* symeIndex -- ignore */
		fintGetSInt(n);
		/* Format */
		fintGetInt(fm, n);
		fmtFormat(fmt) = n;
		if (tag == FOAM_Decl) {
			/* Default protocol */
			fmtProtocol(fmt) = FOAM_Proto_Foam;
		} else {
			/* Direction -- ignore */
			fintGetByte(n);
			/* Protocol */
			fintGetByte(n);
			fmtProtocol(fmt) = FOAM_PROTO_START + n;
		}
	}

	return argc;
}

local void
fintLoadGlobalsFmt(FintUnit unit)
{
	int n, i;

	n = fintReadFmt(&fintUnitGlobs(unit));
	fintUnitGlobsCount(unit) = n;

	fintLinkDEBUG((void)fprintf(dbOut, "Read globals: %d\n", n););

	fintUnitGlobValues(unit) = (ShDataObj *)
		fintAlloc(ShDataObj, n);

	for (i = 0; i < n; i++)
		fintUnitGlobValues(unit)[i] = 
			shDataObjAdd(globType(i), globId(i),
				     globProtocol(i), i, unit);
}

local void
fintLoadConstantsFmt(FintUnit unit)
{
	int n;

	n = fintReadFmt(&fintUnitConsts(unit));
	fintUnitConstsCount(unit) = n;
	fintUnitConstValues(unit) = (DataObj)
		stoAlloc(OB_Other, sizeof(*(unit->constValues)) * n);

	fintLinkDEBUG((void)fprintf(dbOut, "Read constants: %d\n", n););

}

local void
fintLoadFluidsFmt(FintUnit unit)
{
	int n;

	skipProg(NULL, NULL); /* skip the unused format */
	n = fintReadFmt(&fintUnitFluids(unit));
	fintUnitFluidsCount(unit) = n;

	fintLinkDEBUG((void)fprintf(dbOut, "Read fluids: %d\n", n););
}

local void
fintLoadLexLevels(FintUnit unit, int nLexLevels)
{
	int 		n, j;
	LexLevels	lexLevels;
	Fmt		* pFmt;

	fintUnitLexLevelsCount(unit) = nLexLevels;
	lexLevels = (LexLevels) fintAlloc(lexLevel, nLexLevels);

	fintUnitLexLevels(unit) = lexLevels;

	for (j = 0; j < POS_LEX_FMT ; j++) {
		lexLevels[j].fmtLexsCount = 0;
		lexLevels[j].fmtLex = 0;
	}

	for (; j < nLexLevels ; j++) {
		pFmt = &(lexLevels[j].fmtLex);
		n = fintReadFmt(pFmt);
		lexLevels[j].fmtLexsCount = n;
	
		fintLinkDEBUG((void)fprintf(dbOut, "Level: %d, read %d lexicals\n", j, n););
	}
}

/****************************************************************************
 *
 * SkipProg
 *
 ***************************************************************************/

/* If pLabels == NULL, then skipProg'll ignore labels processing
 */
local void
skipProg(FiProgPos * pLabels, int * pLabelsCount)
{
	int	fi, si, tag, argc, format, bi;
	String	argf;
	Bool	neg;
	Bool 	isNary;

	fintGetByte(tag);
	format = FOAM_FORMAT_GET(tag);
	tag    = FOAM_FORMAT_REMOVE(tag, format);

	isNary = (foamInfo(tag).argc == FOAM_NARY);

	if (!isNary)
		argc = foamInfo(tag).argc;
	else
		fintGetInt(format, argc);

	if (tag == FOAM_Label && pLabels) {
		long n;
		fintGetInt(format, n);
		pLabels[n] = ip; /* next instruction */
		*pLabelsCount += 1;
		return;
	}

	argf  = foamInfo(tag).argf;

	for (fi = si = 0; si < argc; fi++, si++) {
		int	af = argf[fi], slen;
		long n;
		if (af == '*') af = argf[--fi];
		switch (argf[fi]) {
		case 't':
			fintGetByte(n);
			break;
		case 'o':
#if SMALL_BVAL_TAGS
			fintGetByte(n);
#else
			fintGetHInt(n);
#endif
			break;
		case 'p':
			fintGetByte(n);
			break;
		case 'b':
			fintGetByte(n);
			break;
		case 'D':
			fintGetByte(n);
			break;
		case 'h':
			fintGetHInt(n);
			break;
		case 'w':
			fintGetSInt(n);
			break;
		case 'X':
			fintGetInt(int0, n);
			break;
		case 'F':
			fintGetInt(int0, n);
			labelFmt = FOAM_FORMAT_FOR(n);
			break;
		case 'L':
			fintGetInt(labelFmt, n);
			break;
		case 'i':
			fintGetInt(format, n);
			break;
		case 's':
			fintGetInt(format, slen);
			ip += slen;
			break;
		case 'f':
			/* foamToSFlo(foam) = fintRdSFloat();*/
			ip += XSFLOAT_BYTES;
			break;
		case 'd':
			/* foamToDFlo(foam) = fintRdDFloat();*/
			ip += XDFLOAT_BYTES;
			break;
		case 'n': {
			fintGetByte(neg);
			fintGetInt(format, slen);
			for (bi = 0; bi < slen; bi++)
				fintGetHInt(n);
			
			break;
		}
		case 'C':
			skipProg(pLabels, pLabelsCount);
			break;
		default:
			bugBadCase(argf[fi]);
		}
	}
	return;
}

/****************************************************************************
 *
 * General utility
 *
 ***************************************************************************/

local SFloat
fintRdSFloat(void)
{
	XSFloat *	pxs;
	SFloat		s;

	pxs = (XSFloat *) fintGetn(XSFLOAT_BYTES);
	xsfToNative(pxs, &s);

	return s;
}

local DFloat
fintRdDFloat(void)
{
	XDFloat *	pxd;
	DFloat		d;

	pxd = (XDFloat *) fintGetn(XDFLOAT_BYTES);
	xdfToNative(pxd, &d);

	return d;
}

local String
fintRdChars(int cc)
{
	String	s;

	s = strAlloc(cc);
	(void)fintGetChars(s, cc);
	s = strnFrAscii(s,cc);

	return s;
}

local String
fintGetn(Length n)
{
	UByte	*s = tape + ip;
	ip += n;
	return (String) s;
}

local void
fintPushFluids(int nFluids)
{
	int i, fluidNo;

	stackAlloc(fluidValues, nFluids + 1);
	fluidValues[nFluids].ptr = (DataObj) fiGlobalFluidStack;

	for (i = 0; i < nFluids; i++) {
		fluidNo = progInfoDFluid(prog)[i];
		fluidValue(i) = fiAddFluid(fluidId(fluidNo));
	}
}
/****************************************************************************
 *
 * Aldor interface
 *
 ***************************************************************************/


/* Ask confirmation if confirm is on
 * Return: true if (the answer is 'y' || confimation is off)
 * $$ TODO: change the parameter: must be a compiler message
 */
Bool
fintYesOrNo(String t)
{
	char c0;

	if (!fintConfirm) return true;

	while (true) {
		(void)fprintf(osStdout, "%s", t);
		c0 = getchar();
		while(getchar() != '\n')
			;

		if (c0 == 'y' || c0 == 'Y')
			return true;
		else if (c0 == 'n' || c0 == 'N')
			return false;
		else
			(void)comsgFPrintf(osStdout, ALDOR_M_FintYesOrNo);
	}
	return false;
}

/****************************************************************************
 *
 * Internal debugging
 *
 ***************************************************************************/

/* Print the backtrace of all stack frames (similar to gdb) 
 * With a 0 argument prints all the stack 
 */
void
fintWhere(int level)
{
	int	n = 0,
		l = 1;
	DataObj	bp0 = bp;

	if (level < 0) return;
	if (level == 0) level = -1;

	if (prog)
		(void)fprintf(dbOut, "#%d %8p in <%s> at unit [%s]\n", int0,
		       	 bp, prog->name, prog->unit->name);
	else
		(void)fprintf(dbOut, "(Unknown current prog)\n");

	while (n != level && bp0 != stackBase) {
		(void)fprintf(dbOut, "#%d %lx in <%s> at unit [%s]\n",
			l++,
			(ULong)stackFrameIp(bp0),
			stackFrameProg(bp0)->name,
			stackFrameProg(bp0)->unit->name);
		bp0 = stackFrameBp(bp0);
		n += 1;
	}

	if (bp != stack)
		(void)fprintf(dbOut, "...\n");

	return;
}


void
fintCheckCallStack(void)
{
	DataObj bp0;
	bp0 = bp;
	while (bp0 != stackBase) {
		DataObj bp1;
		bp1 = stackFrameBp(bp0);
		while (bp1 != stackBase) {
			if (bp1 == bp0) bug("Stack is a mess");
			bp1 = stackFrameBp(bp1);
		}
		bp0 = stackFrameBp(bp0);
	}
}


void
fintSoftAssert(char * ass, char * filename, int line_num)
{
	(void)fprintf(dbOut, "Warning: soft assertion failed, file %s line %d: %s\n",
		filename, line_num, ass);
	return;
}

void
fintHardAssert(char * ass, char * filename, int line_num)
{
	(void)fprintf(dbOut, "Warning: hard assertion failed, file %s line %d: %s\n",
		filename, line_num, ass);
	return;
}

/****************************************************************************
 *
 * fintExecMainUnit
 *
 ***************************************************************************/

void (*defaultBreakHandler)(int) = 0;
void (*defaultFaultHandler)(int) = 0;

local Bool
fintExecMainUnit(void)
{
	union dataObj	expr;
	dataType	type;
	UByte		denv;
	int		nFluids;
	FiBool ok;

	OsSignalHandler oldCompFintBreakHandler = NULL;
	OsSignalHandler oldCompFintFaultHandler = NULL;
	extern void compFintBreakHandler(int);
	extern void compFintFaultHandler(int);

	if (fintMode == FINT_LOOP) {
    		oldCompFintBreakHandler = defaultBreakHandler;
		oldCompFintFaultHandler = defaultFaultHandler;
	}

	/* This group of statements is needed because might be the case
	 * that the previous execution has been interrupted from the user
	 * (Ctrl-C, in example), therefore stack, bp and sp have the values
 	 * that they had when the interrupt occurred.
	 */
	
	stack = headStack;  
	bp = headStack;
	sp = headStack + 1;
	ip = 0;
	stackBase = sp;

	fintCurrentFormat = emptyFormatSlot;

	fintDEBUG((void)fprintf(dbOut, "Starting with bp = %p, sp = %p\n", bp,  sp););

	unit = mainUnit;

	fintEnvPush(lexEnv, NULL, NULL);
	stackFrameAlloc(int0);

   	tape = fintUnitTape(unit);
	prog = constValue(int0).progInfo;
	ip = progInfoSeq(prog);
	labels = progInfoLabels(prog);
	labelFmt = progInfoLabelFmt(prog);

	denv = progInfoDEnv(prog)[0];

	if (fintUnitLexsCount(unit, denv)) {
	      lev0 = fintAlloc(union dataObj, fintUnitLexsCount(unit, denv));
	}
	else
	      lev0 = NULL;

       	fintEnvPush(lexEnv, lev0, NULL);

	if (progInfoLocsCount(prog))
		stackAlloc(locValues, progInfoLocsCount(prog));


	nFluids = progInfoDFluidsCount(prog);
		
	if (nFluids)
		fintPushFluids(nFluids);

	/* **************************** */

 	{
		FiWord	exn = 0;

		fintBlock(ok, type, exn, fintStmt(&expr));
	
		if (!ok) {
			ShDataObj handler;
			union dataObj ret, dexn;
			lazyLibGet("rtexns");
			(void)loadOtherUnits();
			/*	 * hack 03450 */
			handler = shDataObjFindBis((AInt) FOAM_Clos,
						"aldorUnhandledException", 
						FOAM_Proto_Foam);
			if (handler) {
			  if (fintExntraceMode == 1 ) {
			    FILE *oldDbOut = dbOut;
			    dbOut = osStderr; 
			    fprintf(dbOut, "Aldor runtime (interpreter): backtrace:\n");
			    fintWhere(FINT_BACKTRACE_CUTOFF);
			    fprintf(dbOut, "\n");
			    dbOut = oldDbOut;
			  };
			  
			  dexn.fiWord = exn;
			  (void)fintDoCall1(&handler->dataObj, &ret, &dexn);
			  ok = true;
			}
		}
	}
	/* **************************** */

	if (nFluids) 
		fiGlobalFluidStack = (FiFluidStack) fluidValue(nFluids);

	stackFrameFree(); /* This used to cause grief on suns... */

	fintDEBUG((void)fprintf(dbOut, "Finished with bp = %p, sp = %p\n", bp, sp));

	fintDEBUG((void)fprintf(dbOut, "Program returned with value %ld of type %ld\n\n", expr.fiSInt, type));

	if (fintMode == FINT_LOOP) {
		(void)osSetBreakHandler(oldCompFintBreakHandler);
		(void)osSetBreakHandler(oldCompFintFaultHandler);
	}
	fflush(dbOut);
	fflush(osStderr);
	fflush(osStdout);
	return (Bool) ok;
}

/****************************************************************************
 *
 * Main external entry point
 *
 * Takes a foam tree from Aldor and evaluates it.
 * fintInit() must be called before any call to fint()
 * All used structures must be released with fintFini()
 *
 ***************************************************************************/

Bool
fint(Foam foam)
{
	FiBool ok;

	hardAssert(fintInitialized);

	fintGetInitInterpTime();

	instrCounter = 0;

	(void)loadMainUnit(foam);

	(void)loadOtherUnits();

	/* Need a _much_ better handler than this... */
	ok = fintExecMainUnit();

	/* !! We should close the archive files */

	fintStoDEBUG(stoAudit(););

	fintGetEndInterpTime();
	return (Bool) ok;
}

void 
fintRaiseException(char *reason, void *stuff)
{
	ShDataObj exceptionThrower;
	union dataObj ret, arg1, arg2;



	lazyLibGet("rtexns");
	(void)loadOtherUnits();
	/*	 * hack 03450 */
	exceptionThrower = shDataObjFindBis(FOAM_Clos,
				"aldorRuntimeException",
				FOAM_Proto_Foam);
	/* Could munge into better format */
	if (!exceptionThrower) {
		(void)fprintf(stdout,
"Aldor runtime (interpreter): An Aldor runtime error occurred : %s\n\
Note: there seems to be no aldorRuntimeException function defined\n\
so it is not possible to throw an exception.\n",
			reason);

		  /* The interpreter will exit due to an error, give backtrace if requested */

		if (fintExntraceMode == 1 ) {
		  FILE *oldDbOut = dbOut;
		  dbOut = osStderr;
		  fprintf(dbOut, "Aldor runtime (interpreter): backtrace:\n");
		  fintWhere(FINT_BACKTRACE_CUTOFF);
		  fprintf(dbOut, "\n");
		  dbOut = oldDbOut;
		};
		exit(1);
	}
	else {
	  arg1.fiWord = (FiWord) reason;
	  arg2.fiWord = (FiWord) stuff;
	  (void) fintDoCall(&exceptionThrower->dataObj, &ret, 2, &arg1, &arg2);
	  exit(int0);
	  /* Won't get past here */
	}
}

/* Assumes that the file exists */
void
fintFile(FileName fname)
{
	FintUnit	u;
	Lib		lib;

	fintInit();

	lib = libGetHeader(libNew(fname, false, fileRbOpen(fname),
				  (Offset) 0));
	loadUnitFrLib(lib);

	u = car(fintUnitList);

	/* Copy shared structures in mainUnit */

	fintUnitGlobs(mainUnit) = fintUnitGlobs(u);
	fintUnitConsts(mainUnit) = fintUnitConsts(u);
	fintUnitFluids(mainUnit) = fintUnitFluids(u);
	fintUnitLexLevels(mainUnit) = fintUnitLexLevels(u);

	fintUnitGlobsCount(mainUnit) = fintUnitGlobsCount(u);
	fintUnitConstsCount(mainUnit) = fintUnitConstsCount(u);
	fintUnitFluidsCount(mainUnit) = fintUnitFluidsCount(u);
	fintUnitLexLevelsCount(mainUnit) = fintUnitLexLevelsCount(u);	

	fintUnitGlobValues(mainUnit) = fintUnitGlobValues(u);
	fintUnitConstValues(mainUnit) = fintUnitConstValues(u);

	fintUnitId(mainUnit) = fintUnitId(u);
	fintUnitTape(mainUnit) = fintUnitTape(u);
	fintUnitBuffer(mainUnit) = fintUnitBuffer(u);

	(void)loadOtherUnits();

 	(void)fintExecMainUnit();

	/* !! We should close the archive files */
	libClose(lib);

	fintFini();

	fintStoDEBUG(stoAudit(););

}


/****************************************************************************
 *
 * :: Dynamic State
 *
 ***************************************************************************/

typedef struct {
	Buffer	evalBuf;

	UByte 	* tape;		/* interpreted string */
	FiProgPos ip;
	DataObj	stack;		/* current stack */
	DataObj	sp;		/* First free cell on the top of the stack. */
	DataObj	bp;		/* Bottom of the current frame; refers to a
				 * dataObj containing the old bp. 
				 */
	DataObj	locValues; 	/* local values in the current stack frame */
	DataObj	fluidValues; 	/* fluid values in the current stack frame */
	FiEnv	lexEnv;		/* current lexical environment */
	DataObj	lev0;		/* lexEnv->level, used to speed up lex(0,n) */
	ProgInfo	prog;		/* progInfo for the current program */
	FiProgPos	* labels;

	int 	labelFmt;
	FintUnit unit;
} fintState;

local void *
fintSaveState(void)
{
	fintState *state 	= (fintState*) stoAlloc(OB_Other, sizeof(*state));
	state->evalBuf 		= evalBuf;
	state->tape    		= tape;
	state->ip      		= ip;
	state->stack   		= stack;
	state->sp      		= sp;
	state->bp      		= bp;
	state->locValues 	= locValues;
	state->fluidValues 	= fluidValues;
	state->lexEnv		= lexEnv;
	state->lev0		= lev0;
	state->prog     	= prog;
	state->labels   	= labels;
	state->labelFmt 	= labelFmt;
	state->unit     	= unit;

	return state;
}

local void
fintRestoreState(void *s0)
{
	fintState *state = (fintState *) s0;

	evalBuf 	= state-> evalBuf;
	tape    	= state-> tape;
	ip      	= state-> ip;
	stack   	= state-> stack;
	sp      	= state-> sp;
	bp      	= state-> bp;
	locValues 	= state-> locValues;
	fluidValues 	= state-> fluidValues;
	lexEnv		= state-> lexEnv;
	lev0		= state-> lev0;
	prog     	= state-> prog;
	labels   	= state-> labels;
	labelFmt 	= state-> labelFmt;
	unit     	= state-> unit;

	stoFree(state);
}


/****************************************************************************
 *
 * :: Timings
 *
 ***************************************************************************/


local Millisec fintCompTime, fintInterpTime;

void
fintDisplayTimings(void)
{
	if (!fintTimings) return;

	(void)comsgFPrintf(osStdout, ALDOR_M_FintTimings, fintCompTime, fintInterpTime);
}


void
fintGetInitCompTime(void)
{
	if (!fintTimings) return;

	fintCompTime = osCpuTime();
}

local void
fintGetInitInterpTime(void)
{
	Millisec curCpuTime;

	if (!fintTimings) return;

	curCpuTime = osCpuTime();

	fintCompTime = curCpuTime - fintCompTime;
	fintInterpTime = curCpuTime;
}


local void
fintGetEndInterpTime(void)
{
	if (!fintTimings) return;

	fintInterpTime = osCpuTime() - fintInterpTime;
}

