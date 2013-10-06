/*****************************************************************************
 *
 * syme.h: Symbol meanings.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SYME_H_
#define _SYME_H_

#include "axlobs.h"
#include "forg.h"
#include "msg.h"
#include "lib.h"

/******************************************************************************
 *
 * :: Syme kind
 *
 *****************************************************************************/

enum symeTag {
    SYME_START,
	SYME_Label = SYME_START,
	SYME_Param,
	SYME_LexVar,
	SYME_LexConst,
	SYME_Import,
	SYME_Export,
	SYME_Extend,
	SYME_Library,
	SYME_Archive,
	SYME_Builtin,
	SYME_Foreign,
	SYME_Fluid,
	SYME_Trigger,
        SYME_Temp,
        SYME_Has,
    SYME_LIMIT
};

typedef Enum(symeTag)	SymeTag;

/******************************************************************************
 *
 * :: Table of information about syme kinds
 *
 *****************************************************************************/

struct symeInfo {
	SymeTag		kind;
	CString		str;
	CString		descr;
	Msg		msgId; 		
};

extern struct symeInfo symeInfo[];

#define			symeTagToStr(kind)	(symeInfo[kind].str)
#define			symeTagToDescr(kind)	(symeInfo[kind].descr)
#define			symeTagToDescrMsgId(kind)	(symeInfo[kind].msgId)

/******************************************************************************
 *
 * :: Syme fields
 *
 *****************************************************************************/

enum symeField {
    SYME_FIELD_START,
	SYFI_Origin = SYME_FIELD_START,	/* Origin as a pointer */
	SYFI_Exporter = SYFI_Origin,	/* Exporter for import symes */
	SYFI_Comment = SYFI_Origin,	/* Documentation for export symes */
	SYFI_Extendee = SYFI_Origin,	/* Extendees for extend symes */
	SYFI_Library = SYFI_Origin,	/* Library for library symes */
	SYFI_Archive = SYFI_Origin,	/* Archive for archive symes */
	SYFI_Builtin = SYFI_Origin,	/* Builtin tag for builtin symes */
	SYFI_Foreign = SYFI_Origin,	/* Foreign origin for foreign symes */
	SYFI_Foam    = SYFI_Origin,	/* foam for temporary symes */
        SYFI_HasTest = SYFI_Origin,	/* Has Expr for `has' symes */
	SYFI_Original,			/* Unsubstituted self */
	SYFI_Extension,			/* Extend syme for extendee symes */
	SYFI_Condition,			/* Condition for conditional symes */
	SYFI_Twins,			/* Equivalent symes */
	SYFI_Depths,			/* sefoFreeVars depth numbers */
	SYFI_Mark,			/* Sefo traversal mark */
	SYFI_DefLevel,			/* Level where syme is bound */
	SYFI_LibNum,			/* Serial number in syme section */
	SYFI_VarIndex,			/* Serial number in stab level */
	SYFI_UsedDepth,			/* Max relative depth used */
	SYFI_IntStepNo,			/* Interactive step where defined */
	SYFI_FoamKind,			/* Par, Loc, Lex, Glo */
	SYFI_Closure,			/* Foam closure */
	SYFI_Inlined,			/* List of inlined symes */
	SYFI_DVMark,			/* Dead variable elimination mark */
	SYFI_SImpl,			/* Implementation information */
	SYFI_ConstLib,			/* Library for foam constant info */
	SYFI_ConstInfo,			/* Constant number and opt. flags*/
	SYFI_DefnNum,			/* Sequence number when defined */
	SYFI_HashNum,			/* Runtime hash code */
	SYFI_ExtraBits,			/* More syme bits */
	SYFI_ConditionContext,		/* Context in which to infer condition */
    SYME_FIELD_LIMIT
};

typedef Enum(symeField)	SymeField;

/******************************************************************************
 *
 * :: Table of information about syme fields
 *
 *****************************************************************************/

struct symeFieldInfo {
	SymeField	tag;
	String		str;
	AInt		def;
};

extern struct symeFieldInfo symeFieldInfo[];

#define			symeFieldToStr(tag)	(symeFieldInfo[tag].str)
#define			symeFieldDefault(tag)	(symeFieldInfo[tag].def)

/******************************************************************************
 *
 * :: Syme bits
 *
 *****************************************************************************/

#define			SYME_COPY_BITS		0x33FF

#define			SYME_BIT_LAZY		0x0001	/* Type hash codes */
#define			SYME_BIT_SPEC		0x0002	/* Builtin/record op */
#define			SYME_BIT_DEEP		0x0004	/* Used deeply */
#define			SYME_BIT_IMP		0x0008	/* Import seen */
#define			SYME_BIT_INL		0x0010	/* Inlinable */
#define			SYME_BIT_UNCOND		0x0020	/* def not cond.nal  */
#define			SYME_BIT_USED		0x0040	/* Foam used */
#define			SYME_BIT_DEF		0x0080	/* Has default */
#define			SYME_BIT_MARK		0x0100	/* Traversal mark */
#define			SYME_BIT_TOP		0x0200	/* Library export */
#define			SYME_BIT_PCOND		0x0400	/* Pop'd conds */
#define			SYME_BIT_TWIN		0x0800	/* Implicit twin */
#define			SYME_BIT_LIBS		0x1000	/* lib == constLib */
#define			SYME_BIT_LVL		0x2000	/* defLevel fr lib */
#define			SYME_BIT_TRIG		0x4000	/* 2nd pass trigger */
#define 		SYME_BIT_LAZYCOND	0x8000  /* treat condition as 
							   constraint */

#define			symeIsLazy(s)		symeGetBit(s, SYME_BIT_LAZY)
#define			symeIsSpecial(s)	symeGetBit(s, SYME_BIT_SPEC)
#define			symeUsedDeeply(s)	symeGetBit(s, SYME_BIT_DEEP)
#define			symeImportInit(s)	symeGetBit(s, SYME_BIT_IMP)
#define			symeInlinable(s)	symeGetBit(s, SYME_BIT_INL)
#define			symeUnconditional(s)	symeGetBit(s, SYME_BIT_UNCOND)
#define			symeUsed(s)		symeGetBit(s, SYME_BIT_USED)
#define			symeHasDefault(s)	symeGetBit(s, SYME_BIT_DEF)
#define			symeMarkBit(s)		symeGetBit(s, SYME_BIT_MARK)
#define			symeIsTop(s)		symeGetBit(s, SYME_BIT_TOP)
#define			symePopConds(s)		symeGetBit(s, SYME_BIT_PCOND)
#define			symeFullTwin(s)		symeGetBit(s, SYME_BIT_TWIN)
#define			symeSameLibs(s)		symeGetBit(s, SYME_BIT_LIBS)
#define			symeLibLevel(s)		symeGetBit(s, SYME_BIT_LVL)
#define			symeHasTrigger(s)	symeGetBit(s, SYME_BIT_TRIG)
#define			symeCondIsLazy(s)	symeGetBit(s, SYME_BIT_LAZYCOND)

#define			symeSetLazy(s)		symeSetBit(s, SYME_BIT_LAZY)
#define			symeSetSpecial(s)	symeSetBit(s, SYME_BIT_SPEC)
#define			symeSetUsedDeeply(s)	symeSetBit(s, SYME_BIT_DEEP)
#define			symeSetImportInit(s)	symeSetBit(s, SYME_BIT_IMP)
#define			symeSetInlinable(s)	symeSetBit(s, SYME_BIT_INL)
#define			symeSetUnconditional(s)	symeSetBit(s, SYME_BIT_UNCOND)
#define			symeSetUsed(s)		symeSetBit(s, SYME_BIT_USED)
#define			symeSetDefault(s)	symeSetBit(s, SYME_BIT_DEF)
#define			symeSetMarkBit(s)	symeSetBit(s, SYME_BIT_MARK)
#define			symeSetTop(s)		symeSetBit(s, SYME_BIT_TOP)
#define			symeSetPopConds(s)	symeSetBit(s, SYME_BIT_PCOND)
#define			symeSetFullTwin(s)	symeSetBit(s, SYME_BIT_TWIN)
#define			symeSetSameLibs(s)	symeSetBit(s, SYME_BIT_LIBS)
#define			symeSetLibLevel(s)	symeSetBit(s, SYME_BIT_LVL)
#define			symeSetTrigger(s)	symeSetBit(s, SYME_BIT_TRIG)
#define			symeSetCondIsLazy(s)	symeSetBit(s, SYME_BIT_LAZYCOND)

#define			symeClrLazy(s)		symeClrBit(s, SYME_BIT_LAZY)
#define			symeClrImportInit(s)	symeClrBit(s, SYME_BIT_IMP)
#define			symeClrInlinable(s)	symeClrBit(s, SYME_BIT_INL)
#define			symeClrDefault(s)	symeClrBit(s, SYME_BIT_DEF)
#define			symeClrMarkBit(s)	symeClrBit(s, SYME_BIT_MARK)
#define			symeClrPopConds(s)	symeClrBit(s, SYME_BIT_PCOND)
#define			symeClrSameLibs(s)	symeClrBit(s, SYME_BIT_LIBS)
#define			symeClrLibLevel(s)	symeClrBit(s, SYME_BIT_LVL)
#define			symeClrTrigger(s)	symeClrBit(s, SYME_BIT_TRIG)

#define			symePutInlinable(s,x)	symePutBit(s, x, SYME_BIT_INL)

/******************************************************************************
 *
 * :: Syme structure
 *
 *****************************************************************************/

struct syme {
	BPack(UByte)		fieldc;		/* Length of fieldv */
	BPack(SymeTag)		kind;		/* How the symbol is used */
	UShort			bits;		/* Bit fields */

	Symbol			id;		/* The symbol being defined */
	Lib			lib;		/* Library for lazy info */
	Hash			hash;		/* Hash code for lazy info */
	TForm			type;		/* The type of the symbol */

	unsigned int		locmask;	/* Fields found in fieldv */
	unsigned int		hasmask;	/* Fields found in fieldv */
	Syme			full;		/* Syme for other fields */
	AInt *			fieldv;		/* Field values */
};

# define SYME_NUMBER_UNASSIGNED (0x7FFF)
# define SYME_DEPTH_UNUSED	(0x7FFF)

/******************************************************************************
 *
 * :: Local macros for implementing syme field operations
 *
 *****************************************************************************/

#define			SYME_LOC_BITS		((1 << SYME_FIELD_LIMIT) - 1)
#define			SYME_MOD_BITS		(~SYME_LOC_BITS)

#define			symeLocMask(syme)	(symeTrigger(syme)->locmask)
#define			symeHasMask(syme)	(symeTrigger(syme)->hasmask)
#define			symeFull(syme)		((syme)->full)

#define			symeLocalFieldc(syme)	((syme)->fieldc)
#define			symeLocalFieldv(syme)	((syme)->fieldv)

#define			symeLocBit(f)		(1 << (f))
#define			symeHasLocal(s,f)	(symeLocMask(s) & symeLocBit(f))
#define			symeSetLocalBit(s,f)	(symeLocMask(s) |= symeLocBit(f))
#define			symeClrLocalBit(s,f)	(symeMask(s) &= ~symeLocBit(f))

#define			symeModBit(f)		(1 << (f))
#define			symeHasField(s,f)	(symeHasMask(s) & symeModBit(f))
#define			symeSetFieldBit(s,f)	(symeHasMask(s) |= symeModBit(f))
#define			symeClrFieldBit(s,f)	(symeHasMask(s) &= ~symeModBit(f))

#define			symeTriggerField(f)	\
	((f) == SYFI_Origin || (f) == SYFI_Twins || (f) == SYFI_Inlined)

extern UByte		symeIndex		(Syme, SymeField);
extern void		symeSetFieldTrigger	(Syme, SymeField);
extern AInt		symeGetFieldFn		(Syme, SymeField);
extern AInt		symeSetFieldFn		(Syme, SymeField, AInt);
extern AInt		symeSetFieldVal;

#define			symeGetLocal(s,f)	\
	(symeHasLocal(s,f) ? symeLocalFieldv(s)[symeIndex(s,f)] : \
	 symeFieldDefault(f))

#define			symeSetLocal(s,f,v)	\
	(symeLocalFieldv(s)[symeIndex(s,f)] = (v))

#define			symeGetField(s,f)	\
	(symeModBit(f) && !symeHasField(s,f) ? symeFieldDefault(f) : \
	 symeHasLocal(s,f) ? symeGetLocal(s,f) : symeGetFieldFn(s,f))

#define			symeSetField(s,f,v)	\
	(symeSetFieldVal = ((AInt) (v)),	\
	 symeHasLocal(s,f) ? symeSetLocal(s,f,symeSetFieldVal) : \
	 !symeFull(s) && symeSetFieldVal == symeFieldDefault(f) ? \
	 symeSetFieldVal : symeSetFieldFn(s,f,symeSetFieldVal))

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

/*
 * Basic operations.
 */

typedef Bool		(*SymeEqFun)		(Syme, Syme);

extern Syme		symeNew			(SymeTag, Symbol, TForm,
						 StabLevel);
extern Syme		symeNewLib		(SymeTag, Symbol, TForm, Lib);
extern Syme		symeCopy		(Syme);
extern void		symeFree		(Syme);
extern Bool		symeEq			(Syme, Syme);
extern Hash		symeHashFn		(Syme);

/*
 * Field accessors.
 */

#define			symeTrigger(s)		\
	(((s)->kind == SYME_Trigger ? libGetAllSymes((s)->lib) : NULL), (s))

#define			symeKind(syme)		(symeTrigger(syme)->kind)
#define			symeBits(syme)		(symeTrigger(syme)->bits)

#define			symeId(syme)		((syme)->id)
#define			symeLib(syme)		((syme)->lib)
#define			symeHash(syme)		((syme)->hash)

extern TForm		symeType		(Syme);
extern Syme		symeExportingSyme	(Syme);
extern TForm		symeExporter		(Syme);
extern SymeList		symeExtendee		(Syme);
extern Doc		symeComment		(Syme);
extern Syme		symeOriginal		(Syme);
extern SefoList		symeCondition		(Syme);
extern SymeList		symeTwins		(Syme);
extern StabLevel	symeDefLevel		(Syme);
extern SymeList		symeInlined		(Syme);
extern Lib		symeConstLib		(Syme);

#define	symeOrigin(s)		((Pointer)  symeGetField(s, SYFI_Origin))
#define	symeLibrary(s)		((Lib)      symeGetField(s, SYFI_Library))
#define	symeArchive(s)		((Archive)  symeGetField(s, SYFI_Archive))
#define	symeBuiltin(s)		((FoamBValTag) symeGetField(s, SYFI_Builtin))
#define	symeForeign(s)		((ForeignOrigin) symeGetField(s, SYFI_Foreign))
#define	symeFoam(s)		((Foam)     symeGetField(s, SYFI_Foam))
#define	symeExtension(s)	((Syme)     symeGetField(s, SYFI_Extension))
#define	symeDepths(s)		((AIntList) symeGetField(s, SYFI_Depths))
#define	symeMark(s)		((SefoMark) symeGetLocal(s, SYFI_Mark))
#define	symeLibNum(s)		((UShort)   symeGetField(s, SYFI_LibNum))
#define	symeVarIndex(s)		((UShort)   symeGetLocal(s, SYFI_VarIndex))
#define	symeUsedDepth(s)	((UShort)   symeGetLocal(s, SYFI_UsedDepth))
#define	symeIntStepNo(s)	((UShort)   symeGetField(s, SYFI_IntStepNo))
#define	symeFoamKind(s)		((FoamTag)  symeGetField(s, SYFI_FoamKind))
#define	symeClosure(s)		((Foam)     symeGetField(s, SYFI_Closure))
#define	symeDVMark(s)		((UShort)   symeGetField(s, SYFI_DVMark))
#define	symeConstInfo(s)	((AInt)     symeGetField(s, SYFI_ConstInfo))
#define	symeDefnNum(s)		((int)      symeGetField(s, SYFI_DefnNum))
#define	symeHashNum(s)		((int)      symeGetField(s, SYFI_HashNum))
#define symeImpl(s)		((SImpl)    symeGetField(s, SYFI_SImpl))
#define symeExtraBits(s)	((AInt)     symeGetField(s, SYFI_ExtraBits))
#define symeConditionContext(s)	((SymeCContext)     symeGetField(s, SYFI_ConditionContext))

#define			symeIsLabel(s)		(symeKind(s) == SYME_Label)
#define			symeIsParam(s)		(symeKind(s) == SYME_Param)
#define			symeIsLexVar(s)		(symeKind(s) == SYME_LexVar)
#define			symeIsLexConst(s)	(symeKind(s) == SYME_LexConst)
#define			symeIsImport(s)		(symeKind(s) == SYME_Import)
#define			symeIsExport(s)		(symeKind(s) == SYME_Export)
#define			symeIsExtend(s)		(symeKind(s) == SYME_Extend)
#define			symeIsLibrary(s)	(symeKind(s) == SYME_Library)
#define			symeIsArchive(s)	(symeKind(s) == SYME_Archive)
#define			symeIsBuiltin(s)	(symeKind(s) == SYME_Builtin)
#define			symeIsForeign(s)	(symeKind(s) == SYME_Foreign)
#define			symeIsFluid(s)		(symeKind(s) == SYME_Fluid)
#define			symeIsImportOfExtend(s)	\
	(symeIsImport(s) && symeIsExtend(symeOriginal(s)))

#define			symeGetBit(s,b)		(symeBits(s) & (b))

#define			symeIsSelf(s)		(symeId(s) == ssymSelf)
#define			symeIsSelfSelf(s)	(symeId(s) == ssymSelfSelf)
#define			symeIsLocalConst(s)	(symeConstLib(s) == NULL)

#define	symeHasVarIndex(s)	(symeVarIndex(s) != SYME_NUMBER_UNASSIGNED)
#define	symeHasConstNum(s)	(symeConstNum(s) != SYME_NUMBER_UNASSIGNED)
#define	symeHasFoamKind(s)	(symeFoamKind(s) != FOAM_LIMIT)		
#define	symeUnused(s)		(symeUsedDepth(s) == SYME_DEPTH_UNUSED)

#define			symeString(s)		symString(symeId(s))

extern Bool		symeTop			(Syme);
extern Hash		symeNameCode		(Syme);
extern Hash		symeTypeCode		(Syme);
extern void		symeAddHash		(Syme, Hash);

#define	symeDefLevelNo(syme)		(symeDefLevel(syme)->lexicalLevel)
#define	symeDefLambdaLevelNo(syme)	(symeDefLevel(syme)->lambdaLevel)
#define	symeDefLevelIsSubstable(syme)	(symeDefLevel(syme)->isSubstable)
#define	symeIsSubstable(syme)		\
	(symeIsParam(syme) || symeIsLexConst(syme) || symeIsSelf(syme) || \
	 (symeIsExport(syme) && !symeTop(syme)))

/*
 * Field modifiers.
 */

#define			symeSetKind(s,tag)	(symeKind(s) = (tag))
#define			symeSetBits(s,bits)	(symeBits(s) = (bits))

#define			symeSetId(s,id)		(symeId(s) = (id))
#define			symeSetHash(s,h)	(symeHash(s) = (h))

extern TForm		symeSetType		(Syme, TForm);
extern StabLevel	symeSetDefLevel		(Syme, StabLevel);
extern void		symeSetLib		(Syme, Lib);
extern Lib		symeSetConstLib		(Syme, Lib);

/*
 * For debugging purposes
 */
extern AInt		symeGetFieldX		(Syme, AInt);
extern void		symeSetConstNumX	(Syme, AInt);

#define	symeSetOrigin(s,v)		symeSetField(s, SYFI_Origin, v)
#define	symeSetExporter(s,v)		symeSetField(s, SYFI_Exporter, v)
#define	symeSetComment(s,v)		symeSetField(s, SYFI_Comment, v)
#define	symeSetExtendee(s,v)		symeSetField(s, SYFI_Extendee, v)
#define	symeSetLibrary(s,v)		symeSetField(s, SYFI_Library, v)
#define	symeSetArchive(s,v)		symeSetField(s, SYFI_Archive, v)
#define	symeSetBuiltin(s,v)		symeSetField(s, SYFI_Builtin, v)
#define	symeSetForeign(s,v)		symeSetField(s, SYFI_Foreign, v)
#define	symeSetFoam(s,v)		symeSetField(s, SYFI_Foam, v)
#define	symeSetOriginal(s,v)		symeSetField(s, SYFI_Original, v)

#define	symeSetExtension(s,v)		symeSetField(s, SYFI_Extension, v)
/*#define	symeSetExtension(s,v)		symeXSetExtension(s, v)*/

#define	symeSetCondition(s,v)		symeSetField(s, SYFI_Condition, v)
#define	symeSetTwins(s,v)		symeSetField(s, SYFI_Twins, v)
#define	symeSetDepths(s,v)		symeSetField(s, SYFI_Depths, v)
#define	symeSetMark(s,v)		symeSetField(s, SYFI_Mark, v)
#define	symeSetLibNum(s,v)		symeSetField(s, SYFI_LibNum, v)
#define	symeSetVarIndex(s,v)		symeSetField(s, SYFI_VarIndex, v)
#define	symeSetUsedDepth(s,v)		symeSetField(s, SYFI_UsedDepth, v)
#define	symeSetIntStepNo(s,v)		symeSetField(s, SYFI_IntStepNo, v)
#define	symeSetFoamKind(s,v)		symeSetField(s, SYFI_FoamKind, v)
#define	symeSetClosure(s,v)		symeSetField(s, SYFI_Closure, v)
#define	symeSetInlined(s,v)		symeSetField(s, SYFI_Inlined, v)
#define	symeSetDVMark(s,v)		symeSetField(s, SYFI_DVMark, v)
#define	symeSetConstInfo(s,v)		symeSetField(s, SYFI_ConstInfo, v)
#define	symeSetDefnNum(s,v)		symeSetField(s, SYFI_DefnNum, v)
#define	symeSetHashNum(s,v)		symeSetField(s, SYFI_HashNum, v)
#define	symeSetImpl(s,v)		symeSetField(s, SYFI_SImpl, v)
#define symeSetExtraBits(s,v)		symeSetField(s, SYFI_ExtraBits, v)
#define symeSetConditionContext(s,v)	symeSetField(s, SYFI_ConditionContext, v)

#define			symeSetBit(s,b)		(symeBits(s) |= (b))
#define			symeClrBit(s,b)		(symeBits(s) &= ~(b))
#define			symePutBit(s,x,b)	\
	((x) ? symeSetBit(s,b) : symeClrBit(s,b))

#define			symeClrConstNum(s)	\
	symeSetConstNum(s, SYME_NUMBER_UNASSIGNED)

#define symeConstNum(s)		(symeConstInfo(s) & 0xFFFF)
#define symeSetConstNum(s,n)	(symeSetConstInfo(s, (symeConstInfo(s) & 0xFFFF0000) | n))

#define symeConstFlags(s)	(symeConstInfo(s) >> 16)
#define symeConstFlag(s, n)  	(symeConstInfo(s) & (1<< (n+16)))
#define symeSetConstFlag(s, n)  (symeSetConstInfo(s, symeConstInfo(s) | (1<< (n+16))))
#define symeClrConstFlag(s, n)  (symeSetConstInfo(s, symeConstInfo(s) & ~(1<< (n+16))))


extern void		symeAddExtendee		(Syme, Syme);
extern void		symeListSetExtension	(SymeList, Syme);
extern Syme		symeAddCondition	(Syme, Sefo, Bool);
extern SymeList		symeListAddCondition	(SymeList, Sefo, Bool);
extern void		symeAddTwin		(Syme, Syme);
extern void		symeAddImplementation	(Syme, AbLogic, int);
extern void		symeTransferImplInfo    (Syme, Syme);
extern void		symeImplAddConst	(Syme, AbLogic, int);
extern void 		symeImplAddInherit	(Syme, TForm, Syme);

/*
 * Constructors.
 */

extern Syme		symeNewLabel		(Symbol,TForm,StabLevel);
extern Syme		symeNewParam		(Symbol,TForm,StabLevel);
extern Syme		symeNewLexVar		(Symbol,TForm,StabLevel);
extern Syme		symeNewLexConst		(Symbol,TForm,StabLevel);
extern Syme		symeNewExport		(Symbol,TForm,StabLevel);
extern Syme		symeNewExtend		(Symbol,TForm,StabLevel);
extern Syme		symeNewFluid		(Symbol,TForm,StabLevel);
extern Syme		symeNewTemp		(Symbol,TForm,StabLevel);
extern Syme		symeNewHas		(Symbol,TForm,StabLevel);

extern Syme		symeNewImport	(Symbol,TForm,StabLevel,TForm);
extern Syme		symeNewLibrary	(Symbol,TForm,StabLevel,Lib);
extern Syme		symeNewArchive	(Symbol,TForm,StabLevel,Archive);
extern Syme		symeNewBuiltin	(Symbol,TForm,StabLevel,FoamBValTag);
extern Syme		symeNewForeign	(Symbol,TForm,StabLevel,ForeignOrigin);

/*
 * Syme conditions.
 */

extern Bool		symeCheckCondition	(Syme);
extern SymeList		symeListCheckCondition	(SymeList);

extern SymeList 	symeListCheckWithCondition	(SymeList);
extern SymeList 	symeListCheckAddConditions	(SymeList);

extern SymeList	        symeListMakeLazyConditions	(SymeList);

extern Bool 		symeUseIdentifier	(AbSyn, Syme);

/*
 * Syme SExpr I/O.
 */

extern SExpr		symeToSExpr		(Syme);
extern SExpr		symeSExprAList		(Syme);
extern SExpr		symeListToSExpr		(SymeList, Bool);
extern int		symeListWrSExpr		(FILE *, String, SymeList, ULong);

/* TODO: does this belong here? */
extern Hash		gen0SymeTypeCode	(Syme);

/******************************************************************************
 *
 * :: Extra bits - we've run out of room in syme->bits
 *
 *****************************************************************************/

#define		SYME_XBIT_IMPLICIT	0x0001	/* Implicit */
#define		SYME_XBIT_MULTICOND	0x0002	/* Multi-value cond */
#define		SYME_XBIT_CONDINCOMPLETE 0x0004	/* we gave up in symeCheckCondition */
#define		SYME_XBIT_CONDCHECKED	0x0004	/* symeCheckCondition has been called */

#define	symeGetXBit(s,b)	(symeExtraBits(s) & (b))
#define	symeSetXBit(s,b) 	symeSetExtraBits(s, symeExtraBits(s) | (b));
#define	symeClrXBit(s,b) 	symeSetExtraBits(s, symeExtraBits(s) & ~(b));
#define symePutXBit(s,x,b)	((x) ? symeSetXBit(s,b) : symeClrXBit(s,b))


/*
 * Symes that are implicit, i.e. implemented by the compiler
 */
#define symeIsImplicit(s)	symeGetXBit(s, SYME_XBIT_IMPLICIT)
#define symeSetImplicit(s)	symeSetXBit(s, SYME_XBIT_IMPLICIT)
#define symeClrImplicit(s)	symeClrXBit(s, SYME_XBIT_IMPLICIT)


/*
 * Symes that are conditional with more than one possible value.
 */
#define symeIsMultiCond(s)	symeGetXBit(s, SYME_XBIT_MULTICOND)
#define symeSetMultiCond(s)	symeSetXBit(s, SYME_XBIT_MULTICOND)
#define symeClrMultiCond(s)	symeClrXBit(s, SYME_XBIT_MULTICOND)

/*
 * pending stuff
 */
#define symeIsCheckCondIncomplete(s)	symeGetXBit(s, SYME_XBIT_CONDINCOMPLETE)
#define symeSetCheckCondIncomplete(s)	symeSetXBit(s, SYME_XBIT_CONDINCOMPLETE)
#define symeClrCheckCondIncomplete(s)	symeClrXBit(s, SYME_XBIT_CONDINCOMPLETE)

#define symeIsCondChecked(s)	symeGetXBit(s, SYME_XBIT_CONDCHECKED)
#define symeSetCondChecked(s)	symeSetXBit(s, SYME_XBIT_CONDCHECKED)
#define symeClrCondChecked(s)	symeClrXBit(s, SYME_XBIT_CONDCHECKED)

#endif	/* !_SYME_H_ */
