/*****************************************************************************
 *
 * syme.c: Symbol meanings.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "fint.h"
#include "fluid.h"
#include "format.h"
#include "simpl.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "ti_top.h"
#include "util.h"
#include "sefo.h"
#include "lib.h"
#include "tfsat.h"
#include "ablogic.h"
#include "abpretty.h"
#include "doc.h"
#include "comsg.h"
#include "strops.h"
#include "table.h"
#include "sexpr.h"

Bool	symeDebug	= false;
Bool	symeFillDebug	= false;
Bool	symeHasDebug	= false;
Bool	symeLibDebug	= false;
extern Bool tfHashDebug;

#define symeDEBUG	DEBUG_IF(syme)		afprintf
#define symeFillDEBUG	DEBUG_IF(symeFill)	afprintf
#define symeHasDEBUG	DEBUG_IF(symeHas)	afprintf
#define symeLibDEBUG	DEBUG_IF(symeLib)	afprintf
#define tfHashDEBUG	DEBUG_IF(tfHash)	afprintf

/******************************************************************************
 *
 * :: Local function declarations
 *
 *****************************************************************************/

local Syme		symeAlloc		(SymeTag);
local Syme		symeAllocCopy		(Syme);
local void		symeNeed		(Syme, UByte);

local SymeList		symeLocalTwins		(Syme);

local TForm		symeFillType		(Syme);
local void		symeFillFrLibrary	(Syme);
local void		symeFillFrExporter	(Syme, TForm);

local Bool		symeListCheckFindSyme	(SymeList, Syme);
local void		symeListCheckJoinSymes	(Syme, Syme);
local Bool		abIsFullyInstantiated	(Sefo);
local int		symeCheckHas		(SymeCContext, Sefo, Sefo);
local void		symeCheckHasMemo	(Sefo, Sefo, SatMask);
local int		symeCheckHasResult	(Sefo, Sefo, Bool *);
local Bool		 symeCheckIdentifier	(AbSyn, Syme);

extern void		tiTfSefo		(Stab, TForm);
extern TForm		tiGetTForm		(Stab, AbSyn);
extern TForm		abGetCategory		(AbSyn);

/******************************************************************************
 *
 * :: Syme field operations.
 *
 *****************************************************************************/

local Syme
symeAlloc(SymeTag kind)
{
	Syme	syme = (Syme) stoAlloc((unsigned) OB_Syme, sizeof(*syme)); 

	syme->fieldc		= 0;
	syme->kind		= kind;
	syme->bits		= 0;

	syme->locmask		= 0;
	syme->hasmask		= 0;
	syme->full		= NULL;
	syme->fieldv		= NULL;
	syme->lib		= NULL;

	return syme;
}

local Syme
symeAllocCopy(Syme osyme)
{
	Syme	syme = symeAlloc(symeKind(osyme));

	symeBits(syme) = symeBits(osyme) & SYME_COPY_BITS;
	symeHasMask(syme) = symeHasMask(osyme);
	symeFull(syme) = osyme;

	return syme;
}

local void
symeNeed(Syme syme, UByte nfieldc)
{
	UByte	i, ofieldc = symeLocalFieldc(syme);
	AInt	*fieldv;
	ULong	size;

	/* Check the field counts. */
	if (nfieldc <= ofieldc)
		return;

	/* Allocate the new space. */
	size = nfieldc * sizeof(AInt);
	if (symeLocalFieldv(syme))
		fieldv = (AInt *) stoResize(symeLocalFieldv(syme), size);
	else
		fieldv = (AInt *) stoAlloc((unsigned) OB_Other, size);

	/* Initialize the new space. */
	for (i = ofieldc; i < nfieldc; i += 1)
		fieldv[i] = 0;

	/* Update the syme. */
	symeLocalFieldc(syme) = nfieldc;
	symeLocalFieldv(syme) = fieldv;
}

UByte
symeIndex(Syme syme, SymeField field)
{
	SymeField	f;
	UByte		i;

	for (i = 0, f = SYME_FIELD_START; f < field; f += 1)
		if (symeHasLocal(syme, f)) i += 1;

	return i;
}

void
symeSetFieldTrigger(Syme syme, SymeField field)
{
	symeSetTrigger(syme);
	symeSetFieldFn(syme, field, symeFieldDefault(field));
}

AInt
symeGetFieldFn(Syme syme, SymeField field)
{
	while (!symeHasLocal(syme, field)) {
		syme = symeFull(syme);
		if (syme == NULL) return symeFieldDefault(field);
	}

	if (symeHasTrigger(syme) && symeTriggerField(field)) {
		symeClrTrigger(syme);
		libGetAllSymes(symeLib(syme));
	}

	return symeGetLocal(syme, field);
}

AInt
symeSetFieldFn(Syme syme, SymeField field, AInt value)
{
	UByte	fieldc, nfieldc, index, i;
	AInt	*fieldv;

	fieldc = symeIndex(syme, SYME_FIELD_LIMIT);
	index = symeIndex(syme, field);

	nfieldc = fieldc + 1;
	symeNeed(syme, nfieldc);
	symeSetLocalBit(syme, field);
	symeSetFieldBit(syme, field);

	fieldv = symeLocalFieldv(syme);
	for (i = fieldc; i > index; i -= 1)
		fieldv[i] = fieldv[i-1];
	return fieldv[i] = value;
}

AInt symeSetFieldVal;

/******************************************************************************
 *
 * :: Basic operations.
 *
 *****************************************************************************/

Syme
symeNew(SymeTag kind, Symbol id, TForm type, StabLevel level)
{
	Syme	syme = symeAlloc(kind);

	symeSetId(syme, id);
	symeSetLib(syme, NULL);
	symeSetHash(syme, (Hash) 0);
	symeSetType(syme, type);
	symeSetDefLevel(syme, level);

	if (fintMode == FINT_LOOP)
		symeSetIntStepNo(syme, intStepNo);

	return syme;
}

Syme
symeNewLib(SymeTag kind, Symbol id, TForm type, Lib lib)
{
	Syme	syme = symeAlloc(kind);

	symeSetId(syme, id);
	symeSetLib(syme, lib);
	symeSetHash(syme, (Hash) 0);
	syme->type = type;

	if (kind != SYME_Trigger)
		symeSetLibLevel(syme);

	return syme;
}

Syme
symeCopy(Syme osyme)
{
	Syme	syme = symeAllocCopy(osyme);

	symeSetId(syme, symeId(osyme));
	symeSetLib(syme, symeLib(osyme));
	symeSetHash(syme, symeHash(osyme));
	syme->type = osyme->type;

	if (fintMode == FINT_LOOP)
		symeSetIntStepNo(syme, intStepNo);

	return syme;
}

void
symeFree(Syme syme)
{
	stoFree((Pointer) syme);
}

Bool
symeEq(Syme syme1, Syme syme2)
{
	return syme1 == syme2;
}

/*
 * symeHash is a macro and we sometimes need a function.
 */
Hash
symeHashFn(Syme syme)
{
	return symeHash(syme);
}


/******************************************************************************
 *
 * :: Field accessors.
 *
 *****************************************************************************/

TForm
symeType(Syme syme)
{
	Syme	ext;
	TForm	tf;

	/* Use the type of the extension if present. */
	ext = symeExtension(syme);
	if (ext) return symeType(ext);

	/* Trigger symes from other libraries. */
	symeTrigger(syme);

	/* Fill types on lazy symbol meanings. */
	if (symeIsLazy(syme))
		return symeFillType(syme);

	/* Follow forward types if present. */
	tf = syme->type;

	/* BDS: tfIsForward(tf) dereferences tf.  Consequently, it will seg
           fault if tf is null.  If everything works properly, we should
           never reach this point without tf pointing to something valid. */
        assert(tf != NULL);

	if (tfIsForward(tf))
		tf = symeSetType(syme, tfFollowOnly(tf));

	return tf;
}

TForm
symeExporter(Syme syme)
{
	assert(symeIsImport(syme));
	return (TForm) symeGetField(symeExportingSyme(syme), SYFI_Exporter);
}

Syme
symeExportingSyme(Syme syme)
{
	Syme	ext;

	assert(symeIsImport(syme));

	/* Use the exporter of the extension if present. */
	ext = symeExtension(syme);
	if (ext && symeIsImport(ext))
		return symeExportingSyme(ext);

	return syme;
}

SymeList
symeExtendee(Syme syme)
{
	return (SymeList) symeGetFieldFn(syme, SYFI_Extendee);
}

Doc
symeComment(Syme syme)
{
	Doc	doc;

	assert(symeIsExport(syme));

	/* Use docNone if the comment is not present. */
	doc = (Doc) symeGetFieldFn(syme, SYFI_Comment);
	if (doc == NULL) doc = docNone;

	return doc;
}

Syme
symeOriginal(Syme syme)
{
	/* Use symeFull(syme) unless an explicit value is present. */
	while (!symeHasLocal(syme, SYFI_Original)) {
		if (!symeFull(syme)) return syme;
		syme = symeFull(syme);
	}

	return (Syme) symeGetField(syme, SYFI_Original);
}

SefoList
symeCondition(Syme syme)
{
	/* Ignore the conditions if needed for syme equality tests. */
	if (symePopConds(syme)) return listNil(Sefo);
	return (SefoList) symeGetField(syme, SYFI_Condition);
}

local SymeList
symeLocalTwins(Syme syme)
{
	return (SymeList) symeGetLocal(syme, SYFI_Twins);
}

SymeList
symeTwins(Syme syme)
{
	static SymeList	symes0 = listNil(Syme);
	SymeList	symes;

	if (symes0 == listNil(Syme))
		symes0 = listCons(Syme)((Syme) NULL, symes0);

	if (symeHasTrigger(syme) && symeHasLocal(syme, SYFI_Twins)) {
		symeClrTrigger(syme);
		libGetAllSymes(symeLib(syme));
	}

	/* Use symeFull(syme) as an implicit twin if present. */
	symes = symeLocalTwins(syme);
	if (symeFullTwin(syme)) {
		Syme	osyme = symeFull(syme);

		if (symes == listNil(Syme)) {
			setcar(symes0, osyme);
			symes = symes0;
		}
		else if (!listMemq(Syme)(symes, osyme)) {
			symes = listCons(Syme)(osyme, symes);
			symeSetTwins(syme, symes);
		}
	}

	return symes;
}

StabLevel
symeDefLevel(Syme syme)
{
	if (symeLibLevel(syme) && symeLib(syme))
		return car(symeLib(syme)->stab);

	return (StabLevel) symeGetField(syme, SYFI_DefLevel);
}

SymeList
symeInlined(Syme syme)
{
	return (SymeList) symeGetFieldFn(syme, SYFI_Inlined);
}

Lib
symeConstLib(Syme syme)
{
	if (symeSameLibs(syme))
		return symeLib(syme);

	return (Lib) symeGetField(syme, SYFI_ConstLib);
}

/* Derived field accessors. */

#define		symeHashArg(h, hi) {	\
	h ^= (h << 8);			\
	h += (hi) + 200041;		\
	h &= 0x3FFFFFFF;		\
}

Bool
symeTop(Syme syme)
{
	return	(symeIsExport(syme) || symeIsExtend(syme)) &&
		symeOriginal(syme) == syme &&
		listMemq(StabLevel)(stabFile(), symeDefLevel(syme));
}

Hash
symeNameCode(Syme syme)
{
	return strHash(symeString(syme));
}

Hash
symeTypeCode(Syme syme)
{
	Hash		h = 0;

	h = symeHash(syme);
	if (h) return h;
	tfHashDEBUG(dbOut, "Hash: %s %pSyme %pTForm\n",
		    symeString(syme), syme, symeType(syme));
	if (symeIsExport(syme) || symeIsParam(syme) || symeIsSelf(syme)) {
		h = tfHash(symeType(syme));
		symeHashArg(h, (Hash) symeKind(syme));
		symeHashArg(h, symeDefLevel(syme)->hash);
	}

	else if (symeIsImport(syme)) {
		h = tfHash(symeType(syme));
		symeHashArg(h, tfHash(symeExporter(syme)));
	}

	else {
		h = tfHash(symeType(syme));
		symeHashArg(h, symeDefLevel(syme)->hash);
	}

	tfHashDEBUG(dbOut, "Hash: %s %pSyme = %d\n",
		    symeString(syme), syme, h);
	return symeSetHash(syme, h);
}

Hash
gen0SymeTypeCode(Syme syme)
{
	if (symeExtension(syme))
		return gen0SymeTypeCode(symeExtension(syme));
	else if (symeIsLazy(syme) && symeIsImport(syme))
		return symeHash(syme);
	else
		return tfHash(symeType(symeOriginal(syme)));
}


void
symeAddHash(Syme syme, Hash code)
{
	Hash	h = symeTypeCode(syme);
	symeHashArg(h, code);
	symeSetHash(syme, h);
	symeSetLib(syme, NULL);
}

/*
 * These functions are intended to be used from with a debugger since
 * we can't use the equivalent macros.
 */

AInt
symeGetFieldX(Syme s, AInt f) { return symeGetField(s, f); }

void
symeSetConstNumX(Syme s, AInt n) { symeSetConstNum(s, n); }

/******************************************************************************
 *
 * :: Field modifiers.
 *
 *****************************************************************************/

TForm
symeSetType(Syme syme, TForm tf)
{
	symeClrLazy(syme);
	return syme->type = tf;
}

StabLevel
symeSetDefLevel(Syme syme, StabLevel slev)
{
	symeClrLibLevel(syme);
	return (StabLevel) symeSetField(syme, SYFI_DefLevel, slev);
}

void
symeSetLib(Syme syme, Lib lib)
{
	Lib clib = NULL;

	if (syme->kind != SYME_Trigger 
		 && syme->lib != NULL 
		 && symeSameLibs(syme))
		clib = symeConstLib(syme);

	syme->lib = lib;

	if (clib)
		symeSetConstLib(syme, clib);
}

Lib
symeSetConstLib(Syme syme, Lib lib)
{
	if (lib == symeLib(syme)) {
		symeSetSameLibs(syme);
		return lib;
	}

	symeClrSameLibs(syme);
	return (Lib) symeSetField(syme, SYFI_ConstLib, lib);
}

/* Derived field modifiers. */

void
symeAddExtendee(Syme syme, Syme extendee)
{
	symeSetExtendee(syme, listCons(Syme)(extendee, symeExtendee(syme)));
}

void
symeListSetExtension(SymeList symes, Syme syme)
{
	for (; symes; symes = cdr(symes))
		symeSetExtension(car(symes), syme);
}

Syme
symeAddCondition(Syme syme, Sefo cond, Bool pos)
{
	SefoList	l = symeCondition(syme);

	if (abTag(cond) == AB_Test)
		cond = cond->abTest.cond;

	if (abTag(cond) == AB_And) {
		Length	i = abArgc(cond);
		while (i > 0) {
			symeAddCondition(syme, abArgv(cond)[--i], pos);
		}
	}
	else
		l = listCons(Sefo)(cond, l);

	symeSetCondition(syme, l);
	return syme;
}

SymeList
symeListAddCondition(SymeList symes0, Sefo cond, Bool pos)
{
	SymeList	symes, nsymes = listNil(Syme);

	for (symes = symes0; symes; symes = cdr(symes)) {
		Syme nsyme = symeCopy(car(symes));
		symeAddCondition(nsyme, cond, pos);
		nsymes = listCons(Syme)(nsyme, nsymes);
	}

	return listNReverse(Syme)(nsymes);
}

void
symeAddTwin(Syme final, Syme twin)
{
	if (twin == final)
		return;

	if (twin == symeFull(final))
		symeSetFullTwin(final);
	else {
		SymeList	symes = symeLocalTwins(final);
		symeSetTwins(final, listCons(Syme)(twin, symes));
	}
}

extern SImpl symeMergeImpl(Syme, SImpl);

void 
symeTransferImplInfo(Syme to, Syme from)
{
	symeSetHashNum(to, symeHashNum(from));
	symeSetDefnNum(to, symeDefnNum(from));
	/* If no const info, then why bother? */
	symeSetConstInfo(to, symeConstInfo(from));
	symeSetConstLib(to, symeConstLib(from));

	symeDEBUG(dbOut, "Transfer: %d %d %d [%pSyme --> %pSyme]\n",
		  symeHashNum(from), symeDefnNum(from),
		  symeConstNum(from), from, to);

	symeMergeImpl(to, symeImpl(from));
}

/******************************************************************************
 *
 * :: Implementation handling.
 *
 *****************************************************************************/

/*
 * Eventually, this will be a rehash of the `conditional' code
 * with a few twists (like it's ok to give up if things get 
 * overly nasty.
 */

SImpl
symeMergeImpl(Syme syme, SImpl impl)
{
	SImpl oimpl = symeImpl(syme);
	SImpl newImpl = NULL;

	if (oimpl == NULL && impl == NULL)
		return impl;
	
	if (DEBUG(syme)) {
		fprintf(dbOut, "(Merging:\n");
		implPrintDb(oimpl);
		implPrintDb(impl);
	}
	/* No existing implementation */
	if (oimpl == NULL)
		newImpl = impl;
	/* New implementation doesn't exist */
	else if (impl == NULL) {
		/* might prefer to kill impl here */
		newImpl = oimpl;
	}
	/* Definition is a local */
	else if (implIsLocal(oimpl)) {
		newImpl = oimpl;
	}
	/* Definition is a condition */
	else if (implIsCond(oimpl)) {
		/* This isn't right */
		/* Should attempt to merge sensibly */
		newImpl = implNewBranch(oimpl->implCond.cond, oimpl, impl);
	}
	/* Inherited */
	else if (implIsInherit(oimpl)) {
		if (implIsLocal(impl)) 
			newImpl = impl;
		else if (implIsDefault(impl))
			newImpl = impl;
		else if (implIsCond(impl)) {
			newImpl = implNewBranch(impl->implCond.cond, 
						impl->implCond.impl,
						oimpl);
		}
		else if (implIsInherit(impl)) {
			newImpl = impl;
		}
		else if (implHasDefault(impl)) {
			newImpl = impl;
		}
		else if (implIsBranch(impl)) {
			/* Probably wrong but keeps things running */
			newImpl = impl;
		}
		else
			bug("%s: Unhandled Merge #1", "symeMergeImpl");
	}
	/* Branch condition */
	else if (implIsBranch(oimpl)) {
		/* Probably just plain wrong */
		/* Should be some implies test, since one condition will imply 
		 * or be equal to the other.
		 */
		newImpl = oimpl;
	}
	else if (implIsDefault(oimpl)) {
		if (implIsDefault(impl)) 
			newImpl = impl;
		else if (implIsLocal(impl)) 
			newImpl = impl;
		else if (implIsCond(impl)) {
			newImpl = implNewBranch(impl->implCond.cond, 
						impl->implCond.impl,
						oimpl);
		}
		else if (implIsInherit(impl)) {
			newImpl = impl;
		}
		else if (implIsBranch(impl)) {
			/* Probably wrong but keeps things running */
			newImpl = impl;
		}
		else
			bug("%s: Unhandled Merge #2", "symeMergeImpl");
	}
	else {
		bug("%s: Unexpected merge", "symeMergeImpl");
	}
	
	if (!newImpl)
		bug("%s: Unhandled Merge #3", "symeMergeImpl");
	
	if (DEBUG(syme)) {
		implPrintDb(newImpl); fprintf(dbOut, ")\n");
	}
	if (newImpl != oimpl)
		symeSetImpl(syme, newImpl);
	return impl;
}


void
symeImplAddConst(Syme syme, AbLogic condition, int defn)
{
	SImpl impl;
	
	if (DEBUG(syme)) {
		fprintf(dbOut, "(Adding implementation (%d) for: %s\n", 
			defn, symeString(syme));
		tfPrintDb(symeType(syme));
		ablogPrint(dbOut, condition);
	}

	impl = implNewLocal(symeHasDefault(syme), defn);

	if (!ablogIsTrue(condition))
		impl = implNewCond(ablogCopy(condition), impl);

	symeMergeImpl(syme, impl);

	implFree(impl);

	symeDEBUG(dbOut, ")\n");
}

void
symeImplAddInherit(Syme syme, TForm tf, Syme parent)
{
	SImpl impl;

	if (symeHasDefault(parent)) {
		impl = implNewDefault(parent);
		symeMergeImpl(syme, impl);
		return;
	}
	/* 
	 * Simple checks to see if tf really does export something
	 * called syme
	 */
	if (tfIsNone(tf)) return;
	
	if (DEBUG(syme)) {
		fprintf(dbOut, "(Adding inherited implementation for: %s", symeString(syme));
		tfPrintDb(symeType(syme));
		tfPrintDb(tf);
	}
	impl = implNewInherit(tf);

	symeMergeImpl(syme, impl);
	symeDEBUG(dbOut, ")\n");
	
}


/******************************************************************************
 *
 * :: Constructors.
 *
 *****************************************************************************/

Syme
symeNewLabel(Symbol id, TForm tf, StabLevel slev)
{
	return symeNew(SYME_Label, id, tf, slev);
}

Syme
symeNewParam(Symbol id, TForm tf, StabLevel slev)
{
	return symeNew(SYME_Param, id, tf, slev);
}

Syme
symeNewLexVar(Symbol id, TForm tf, StabLevel slev)
{
	return symeNew(SYME_LexVar, id, tf, slev);
}

Syme
symeNewLexConst(Symbol id, TForm tf, StabLevel slev)
{
	return symeNew(SYME_LexConst, id, tf, slev);
}

Syme
symeNewExport(Symbol id, TForm tf, StabLevel slev)
{
	return symeNew(SYME_Export, id, tf, slev);
}

Syme
symeNewExtend(Symbol id, TForm tf, StabLevel slev)
{
	return symeNew(SYME_Extend, id, tf, slev);
}

Syme
symeNewFluid(Symbol id, TForm tf, StabLevel slev)
{
	return symeNew(SYME_Fluid, id, tf, slev);
}

Syme
symeNewImport(Symbol id, TForm type, StabLevel level, TForm exporter)
{
	Syme	syme = symeNew(SYME_Import, id, type, level);
	symeSetExporter(syme, exporter);
	return syme;
}

Syme
symeNewLibrary(Symbol id, TForm type, StabLevel level, Lib lib)
{
	Syme	syme = symeNew(SYME_Library, id, type, level);
	symeSetLibrary(syme, lib);
	return syme;
}

Syme
symeNewArchive(Symbol id, TForm type, StabLevel level, Archive ar)
{
	Syme	syme = symeNew(SYME_Archive, id, type, level);
	symeSetArchive(syme, ar);
	return syme;
}

Syme
symeNewBuiltin(Symbol id, TForm type, StabLevel level, FoamBValTag bval)
{
	Syme	syme = symeNew(SYME_Builtin, id, type, level);
	symeSetBuiltin(syme, bval);
	if (bval == FOAM_BVAL_LIMIT) symeSetSpecial(syme);
	return syme;
}

Syme
symeNewForeign(Symbol id, TForm type, StabLevel level, ForeignOrigin forg)
{
	Syme	syme = symeNew(SYME_Foreign, id, type, level);
	symeSetForeign(syme, forg);
	return syme;
}

Syme
symeNewTemp(Symbol id, TForm type, StabLevel level)
{
	Syme	syme = symeNew(SYME_Temp, id, type, level);

	return syme;
}

/*****************************************************************************
 *
 * :: symeFillType
 *
 ****************************************************************************/

local TForm
symeFillType(Syme syme)
{
	assert(symeIsLazy(syme));
	assert(symeIsExport(syme) || symeIsImport(syme) ||
	       symeIsParam(syme) || symeIsSelf(syme));

	symeFillDEBUG(dbOut, ">>[%s]\t name: %25s  hash: %12ld  lib: %s\n",
		      symeTagToStr(symeKind(syme)),
		      symeString(syme),
		      symeTypeCode(syme),
		      libToStringStatic(symeLib(syme)));

	/* Lazy exports know which library they came from. */
	if (symeIsExport(syme) || symeIsParam(syme) || symeIsSelf(syme))
		symeFillFrLibrary(syme);

	if (symeIsImport(syme)) {
		TForm	exporter = symeExporter(syme);

		/* Certain lazy imports know where they came from. */
		if (tfIsLazyExporter(exporter))
			symeFillFrLibrary(syme);
		else
			symeFillFrExporter(syme, exporter);
	}

	symeFillDEBUG(dbOut, "<<[%s]\t name: %25s  hash: %12ld  lib: %s\n",
		      symeTagToStr(symeKind(syme)),
		      symeString(syme),
		      symeTypeCode(syme),
		      libToStringStatic(symeLib(syme)));

	assert(!symeIsLazy(syme));
	return symeType(syme);
}

local void
symeFillFrLibrary(Syme syme)
{
	ULong		num;
	TForm		type;
	Lib		lib = symeLib(syme);
	Symbol		sym = symeId(syme);
	Hash		h = symeHash(syme);

	assert(lib && h);
	libGetAllSymes(lib);
	num = libSymeTypeNo(lib, sym, h);
	type = libGetSymeType(lib, num);
	symeSetType(syme, type);
}

local void
symeFillFrExporter(Syme isyme, TForm exporter)
{
	Stab		stab;
	SymeList	symes;

	assert(symeLib(isyme));
	stab = symeLib(isyme)->stab;

	tiTopFns()->tiTfSefo(stab, exporter);

	for (symes = tfGetDomImports(exporter); symes; symes = cdr(symes)) {
		Syme	syme = car(symes);
		if (symeId(syme) == symeId(isyme) &&
		    symeTypeCode(syme) == symeTypeCode(isyme)) {
			/* Lazy domain imports know where they came from. */
			if (symeIsLazy(syme)) symeFillFrLibrary(syme);
			symeSetType(isyme, symeType(syme));
			symeSetOriginal(isyme, symeOriginal(syme));
			return;
		}
	}

        comsgFatal(NULL, ALDOR_F_LibExportNotFound,
                   symeString(isyme), symeTypeCode(isyme),
                   libToStringStatic(symeLib(isyme)), tfPretty(exporter));
}

/******************************************************************************
 *
 * :: Syme conditions.
 *
 *****************************************************************************/

SymeList
symeListCheckAddConditions(SymeList symes0)
{
	SymeList	symes, next, nsymes = listNil(Syme);

	for (symes = symes0; symes; symes = listFreeCons(Syme)(symes)) {
		Syme	syme = car(symes);

		if (!symeCheckCondition(syme)) continue;

		if (!symeListCheckFindSyme(nsymes, syme)) {
			next = listCons(Syme)(syme, listNil(Syme));
			nsymes = listNConcat(Syme)(nsymes, next);
		}
	}

	return nsymes;
}


SymeList
symeListCheckWithCondition(SymeList symes0)
{
	SymeList	symes, next, nsymes = listNil(Syme);

	for (symes = symes0; symes; symes = listFreeCons(Syme)(symes)) {
		Syme	syme = car(symes);

		if (symeIsSelfSelf(syme)) continue;
		if (!symeCheckCondition(syme)) continue;
		
		/* Don't merge, as we may get garbage!
		 * eg % == X
		 * [remove next 3 if unsure]*/
		if (1/*!symeListCheckFindSyme(nsymes, syme)*/) {
			next = listCons(Syme)(syme, listNil(Syme));
			nsymes = listNConcat(Syme)(nsymes, next);
		}

	}

	return nsymes;
}


SymeList
symeListCheckCondition(SymeList symes0)
{
	SymeList	symes, next, nsymes = listNil(Syme);

	for (symes = symes0; symes; symes = listFreeCons(Syme)(symes)) {
		Syme	syme = car(symes);

		if (symeIsSelfSelf(syme)) continue;
		if (!symeCheckCondition(syme)) continue;

		if (!symeListCheckFindSyme(nsymes, syme)) {
			next = listCons(Syme)(syme, listNil(Syme));
			nsymes = listNConcat(Syme)(nsymes, next);
		}
	}

	return nsymes;
}

local Bool
symeListCheckFindSyme(SymeList symes, Syme syme2)
{
	Bool		merge = false;

	for (; !merge && symes; symes = cdr(symes)) {
		Syme	syme1 = car(symes);

		if (!symeEqualModConditions(NULL, syme1, syme2))
			merge = false;

		else if (!symeCondition(syme1) || !symeCondition(syme2))
			merge = true;

		else
			merge = sefoListEqualMod(NULL, symeCondition(syme1),
						 symeCondition(syme2));

		if (merge) {
			symeListCheckJoinSymes(syme1, syme2);
			symeAddTwin(syme1, syme2);
		}
	}

	return merge;
}

local void
symeListCheckJoinSymes(Syme syme1, Syme syme2)
{
	if (!symeCondition(syme1))
		symeSetCondition(syme2, NULL);

	if (!symeCondition(syme2)) {
		symeSetCondition(syme1, NULL);
	}

	if (!symeHasDefault(syme1) && symeHasDefault(syme2)) {
		symeSetDefault(syme1);
		symeTransferImplInfo(syme1, syme2);
	}
}

Bool
symeCheckCondition(Syme syme)
{
	symeSetCondChecked(syme);
	while (symeCondition(syme)) {
		Sefo	cond = car(symeCondition(syme));
		Sefo	dom, cat;
		int     result;

		/* If the condition can be checked now, check it.
		 * Otherwise just leave it alone and accept the syme.
		 */
		if (abTag(cond) != AB_Has)
			return true;

		dom = cond->abHas.expr;
		cat = cond->abHas.property;
		symeHasDEBUG(dbOut, "(symeCheckCondition: %pSymeC...", syme);
		result = symeCheckHas(symeConditionContext(syme), dom, cat);
		symeHasDEBUG(dbOut, " ... %d)\n", result);

		if (result == 1) {
			symeSetCheckCondIncomplete(syme);
		}
		else if (result == 2) {
			symeSetCondition(syme,
					 listFreeCons(Sefo)(symeCondition(syme)));

			continue;
		}

		if (!abIsFullyInstantiated(dom)) {
			return true;
		}
		if (abTForm(dom) && tfEqual(abTForm(dom), symeExporter(syme)))
			return true;

		symeClrCheckCondIncomplete(syme);
		return false;
	}

	symeClrCheckCondIncomplete(syme);
	return true;
}

local Bool
abIsFullyInstantiated(Sefo ab)
{
	Bool	result = true;

	if (abTag(ab) == AB_Id) {
		Syme	syme = abSyme(ab);
		if (syme == NULL || symeIsParam(syme) ||
		    symeIsLexConst(syme) || symeIsLexVar(syme))
			result = false;
	}

	else if (!abIsLeaf(ab)) {
		Length	i;
		for (i = 0; result && i < abArgc(ab); i += 1)
			result = abIsFullyInstantiated(abArgv(ab)[i]);
	}

	return result;
}

local int
symeCheckHas(SymeCContext conditionContext, Sefo dom, Sefo cat)
{
	TForm	tfdom, tfcat;
	SatMask	result;
	int	flg = 0;
	int     cache;

	cache = symeCheckHasResult(dom, cat, &flg);
	if (cache == 1)
		return flg;
	
	tfdom = abGetCategory(dom);
	tfcat = abTForm(cat) ? abTForm(cat) : tiTopFns()->tiGetTopLevelTForm(NULL, cat);

	/* D has C iff typeof(D) satisfies C. */
	result = tfSat(tfSatBupMask(), tfdom, tfcat);

#if 0
	symeCheckHasMemo(dom, cat, result);
#endif

	return tfSatPending(result) ? 1 : (tfSatSucceed(result) ? 2 : 0);
}


/* We use the following encoding for the hashtable:
 * symeHasCache is a table consisting of (S, SymeSatTblVal)
 * pairs.  We have to treat pending comparisons specially, 
 * as we can't rely on the return value --- it better to
 * wait until the type is fully instatiated.  This also
 * assumes that the value given by tfSatGetPendingFail  
 * must no longer be pending before the result of the
 * tfSat test changes.
 */

typedef struct _SymeSatTblVal {
	Sefo   sefo;
	Bool   success;
	TForm  pend;
} SymeSatTblVal;

typedef struct _SymeSatTblEnt {
	int   sz;
	SymeSatTblVal argv[NARY];
} SymeSatTblEnt;

Table symeHasTestCache;

local void
symeCheckHasMemo(Sefo dom, Sefo cat, SatMask result)
{
	SymeSatTblEnt *ent, *ent0;
	int idx;

	if (symeHasTestCache == NULL)
		symeHasTestCache = tblNew((TblHashFun) abHash, 
					  (TblEqFun) sefoEqual);

	ent0 = (SymeSatTblEnt*) tblElt(symeHasTestCache, (TblKey) dom, NULL);
	ent = ent0;
	if (ent == NULL) {
		ent = (SymeSatTblEnt *)stoAlloc(OB_Other, 
						fullsizeof(SymeSatTblEnt, 2, 
							   SymeSatTblVal));
		ent->sz = 2;
		ent->argv[1].sefo = NULL;
		idx = 0;
	} 
	else if (ent->argv[ent->sz-1].sefo != NULL) {
		ent = (SymeSatTblEnt *)stoAlloc(OB_Other, 
						fullsizeof(SymeSatTblEnt, 
							   (ent0->sz + 2), 
							   SymeSatTblVal));
		ent->sz = ent0->sz + 2;
		for (idx = 0; idx < ent0->sz; idx++) {
			ent->argv[idx].sefo    = ent0->argv[idx].sefo;
			ent->argv[idx].success = ent0->argv[idx].success;
			ent->argv[idx].pend    = ent0->argv[idx].pend;
		}
		/* Set the 2 end chaps */
		ent->argv[idx  ].sefo = NULL;
		ent->argv[idx+1].sefo = NULL;
	}
	else {	
		for (idx=0; ent->argv[idx].sefo != NULL; idx++) /* nuffin */;
	}
	
	ent->argv[idx].sefo    = cat;
	ent->argv[idx].success = tfSatSucceed(result);
	ent->argv[idx].pend    = tfSatPending(result) ? tfSatGetPendingFail() : NULL;

	if (ent != ent0) {
		tblSetElt(symeHasTestCache, (TblKey)dom, (TblElt) ent);
		if (ent0) stoFree(ent0);
	}
}


/* Result:
 * <undef> => no result
 * 0 => fail
 * 1 => Pending
 * 2 => Success
 * Return value:
 * 2 => pending value changed
 * 1 => found
 * 0 => not found
 */

local int
symeCheckHasResult(Sefo dom, Sefo cat, Bool *result)
{
	SymeSatTblEnt *ent;
	int 	      idx;

	if (symeHasTestCache == NULL) return 0;

	ent =  (SymeSatTblEnt*) tblElt(symeHasTestCache, (TblKey) dom, NULL);
	
	if (ent == NULL) return 0;
	
	for (idx = 0; idx < ent->sz; idx++) {
		if (ent->argv[idx].sefo 
		    && sefoEqual(ent->argv[idx].sefo, cat))
			break;
	}
	if (idx == ent->sz) return 0;

	*result = (ent->argv[idx].pend != NULL) ? 1 : (ent->argv[idx].success ? 2 : 0);

	if (ent->argv[idx].pend) {
		if (!tfIsPending(ent->argv[idx].pend)) {	
			ent->argv[idx].sefo = NULL;
			return 2;
		}
	}
	return 1;
}

/*
 * Lazy conditionals 
 */
AbSyn symeLazyCheckData;

SymeList
symeListMakeLazyConditions(SymeList symes)
{
	SymeList tsymes;
	for (tsymes = symes; tsymes ; tsymes = cdr(tsymes)) {
		if (symeCondition(car(tsymes)))
			symeSetCondIsLazy(car(tsymes));
	}
	return symes;
}


Bool
symeUseIdentifier(AbSyn ab, Syme syme)
{
	return symeCheckIdentifier(ab, syme);
}

local Bool
symeCheckIdentifier(AbSyn ab, Syme syme)
{
	Scope("SymeUseIdentifier");
	AbSyn 	 fluid(symeLazyCheckData);
	SefoList conds, tmp;
	Bool 	 ok = true;
	
	symeLazyCheckData = ab;

	if (!symeCondIsLazy(syme))
		Return(true);

	tmp = listCons(Sefo)(ab, listNil(Sefo));


	/* First check conditions imposed by categories */
	for (conds = symeCondition(syme); conds && ok  ; conds = cdr(conds)) {
		AbSyn 	cond = car(conds);
		AbSyn   dom, cat;
		TForm	tfdom, tfcat;
		SatMask	result;

		car(tmp) = cond;
		if (ablogIsListKnown(tmp))
			continue;

		assert(abTag(cond) == AB_Has);
		dom = cond->abHas.expr;
		cat = cond->abHas.property;

		tfdom = abGetCategory(dom);
		tfcat = abTForm(cat) ? abTForm(cat) : tiTopFns()->tiGetTopLevelTForm(NULL, cat);

		/* D has C iff typeof(D) satisfies C. */
		result = tfSat(tfSatTdnInfoMask(), tfdom, tfcat);
		
		ok = tfSatPending(result) || tfSatSucceed(result);
	}

	Return(ok);
}

/*****************************************************************************
 *
 * :: Syme SExpr I/O
 *
 ****************************************************************************/

local String symeSExprDocumentation	(Syme, Bool);

#define	sxiACons(s,v,a)	sxCons(sxCons(sxiFrSymbol(symIntern(s)),v),a)
static String ssxName = "";

SExpr
sefoListToSExpr(SefoList sefos)
{
	SExpr		sx = sxNil;

	for (; sefos; sefos = cdr(sefos))
		sx = sxCons(abToSExpr(car(sefos)), sx);

	sx = sxNReverse(sx);
	return sx;
}

SExpr
symeSExprAList(Syme syme)
{
	SExpr		sxi, al = sxNil;
	TForm		type = symeType(syme);
	String 		str;
	/* 1. Documentation */

	/* Should replace this with something that combines 
	 * the comments in a meaningful way
	 */
	
	str = symeSExprDocumentation(syme, false);
	if (str) {
		sxi = sxiFrString(str);
		strFree(str);
		al = sxiACons("documentation", sxi, al);
	}
#if 0
	/* pre-1.1.11 method */
	if (symeIsExport(syme)) dsyme = syme;
	else if (symeIsExtend(syme))
		dsyme = car(listLastCons(Syme)(symeExtendee(syme)));
	else dsyme = NULL;
	
	if (dsyme && symeIsExport(dsyme) 
	    && symeComment(dsyme) != docNone
	    && (symeLib(dsyme) == NULL
		|| strEqual(libToStringShort(symeLib(dsyme)), ssxName))
	    ) {
		sxi = sxiFrString(docString(symeComment(dsyme)));
		al = sxiACons("documentation", sxi, al);
	}
#endif
	
	/* 2. Defaulted?  */
	if (symeHasDefault(syme))
		al = sxiACons("default", sxiFrInteger(1), al);

	/* 3. Name hash code */
	sxi = sxiFrInteger(symeNameCode(syme));
	al = sxiACons("symeNameCode", sxi, al);

	/* 4. Type hash code */
	sxi = sxiFrInteger(gen0SymeTypeCode(syme));
	al = sxiACons("symeTypeCode", sxi, al);
	
	if (tfIsAnyMap(type))
		type = tfMapRet(type);

	/* 5. Category exports */
	if (!symeIsSelfSelf(syme) && tfSatCat(type)) {
		sxi = symeListToSExpr(tfGetThdExports(type), false);
		al = sxiACons("catExports", sxi, al);
	}

	/* 6. Domain exports */
	else if (!symeIsSelfSelf(syme) && tfSatDom(type)) {
		sxi = symeListToSExpr(tfGetCatExports(type), false);
		al = sxiACons("domExports", sxi, al);
	}

	/* 7. Condition */
	else if (symeCondition(syme)) {
		sxi = sxCons(abInfo(AB_And).sxsym,
			     sefoListToSExpr(symeCondition(syme)));
		al = sxiACons("condition", sxi, al);
	}

	return sxNReverse(al);
}

local String
symeSExprDocumentation(Syme syme, Bool localp)
{
	if (symeIsExport(syme)) {
		if (symeComment(syme) == docNone)
			return NULL;
		if (!localp 
		    || symeLib(syme) == NULL 
		    || strEqual(libToStringShort(symeLib(syme)), 
				ssxName))
			return strCopy(docString(symeComment(syme)));
		return NULL;
	}
	else if (!symeIsExtend(syme))
		return NULL;
	else {
		SymeList exts;
		String str = strCopy("");
		exts = symeExtendee(syme);
		while (exts) {
			Syme orig = symeOriginal(car(exts));
			String ds = symeSExprDocumentation(orig, false);
			if (ds) {
				String ostr = str;
				str = strConcat(str, ds);
				strFree(ostr);
				strFree(ds);
			}
			exts = cdr(exts);
		}
		return str;
	}
}

SExpr
symeToSExpr(Syme syme)
{
	SExpr		sx = sxNil;
	SExpr		sxi;
	TForm		type = symeType(syme);

	/* |Declare| */
	sxi = abInfo(AB_Declare).sxsym;
	sx  = sxCons(sxi, sx);

	/* Symbol name */
	sxi = sxiFrSymbol(symeId(syme));
	sx  = sxCons(sxi, sx);

	/* Symbol type */
	sxi = abToSExpr(tfExpr(type));
	sx  = sxCons(sxi, sx);

	/* Create an Alist for remaining data */
	sxi = symeSExprAList(syme);
	sx  = sxCons(sxi, sx);

	sx = sxNReverse(sx);
	return sx;
}

SExpr
symeListToSExpr(SymeList symes, Bool top)
{
	SExpr		sx = sxNil;

	for (; symes; symes = cdr(symes)) {
		Syme		syme = car(symes);
		Bool		this;

		this = symeTop(syme);
		if (!top)
			this = true;
		if (DEBUG(symeLib)) {
			this = true;
		}
		if (this) sx = sxCons(symeToSExpr(syme), sx);
	}

	sx = sxNReverse(sx);
	return sx;
}

int
symeListWrSExpr(FILE * file, String libName, SymeList symes, ULong sxioMode)
{
	Scope("symeListWrSExpr");
	SExpr		sx;
	int		cc;
	String	fluid(ssxName);
	ssxName = libName;
	sx = symeListToSExpr(symes, true);
	cc = sxiWrite(file, sx, sxioMode);
	sxiFree(sx);

	Return(cc);
}


void gen0SetFoamKind(Syme syme, FoamTag kind)
{
	symeSetFoamKind(syme, kind);
}


void
symeShowFully(Syme s)
{
	symePrintDb(s);
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"lazy ...............", symeIsLazy(s) ? "yes" : "no",
		"special ............", symeIsSpecial(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"used ...............", symeUsed(s) ? "yes" : "no",
		"deeply .............", symeUsedDeeply(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"init ...............", symeImportInit(s) ? "yes" : "no",
		"trigger ............", symeHasTrigger(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"unconditional ......", symeUnconditional(s) ? "yes" : "no",
		"lazy cond ..........", symeCondIsLazy(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"inlinable ..........", symeInlinable(s) ? "yes" : "no",
		"has defaults .......", symeHasDefault(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"marked .............", symeMarkBit(s) ? "yes" : "no",
		"const lib ..........", symeConstLib(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"top-level ..........", symeIsTop(s) ? "yes" : "no",
		"pop-conds ..........", symePopConds(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-3s         %s %-3s\n",
		"full twins .........", symeFullTwin(s) ? "yes" : "no",
		"same libs ..........", symeSameLibs(s) ? "yes" : "no");
	fprintf(dbOut, "   %s %-5d       %s %-5ld\n",
		"var index ..........", symeVarIndex(s),
		"const num ..........", symeConstNum(s));
	fprintf(dbOut, "   %s %-9d   %s %-9d\n",
		"defn num ...........", symeDefnNum(s),
		"hash num ...........", symeHashNum(s));
	fprintf(dbOut, "   %s %-5d       %s %-5d\n",
		"lib level ..........", symeLibLevel(s),
		"def level ..........", ((UShort)symeGetLocal(s, SYFI_DefLevel)));
	fprintf(dbOut, "   %s %-9lx   %s %-9ld\n",
		"depths .............", (unsigned long) symeDepths(s),
		"use depth ..........", (long) symeUsedDepth(s));

	fprintf(dbOut, "   %s %-9s   %s %s\n",
		"foam kind ..........", (symeFoamKind(s) < FOAM_LIMIT) ?
			foamInfo(symeFoamKind(s)).str : "(unset)",
		"extension ..........", symeExtension(s) ? "yes" : "no");
}


/******************************************************************************
 *
 * :: Debugging versions of the syme* getter macros.
 *
 *****************************************************************************/

SImpl
symeXSImpl(Syme s)
{
	return (SImpl)symeImpl(s);
}

/******************************************************************************
 *
 * :: Debugging versions of the symeSet* macros.
 *
 *****************************************************************************/

int
symeXSetExtension(Syme s, AInt v)
{
	symeSetField(s, SYFI_Extension, v);
	return 0;
}


/******************************************************************************
 *
 * :: Table of information about syme tags
 *
 *****************************************************************************/

struct symeInfo symeInfo[] = {
	{SYME_Label,   "SYME_Label",	"label",	    ALDOR_S_Syme_Label},
	{SYME_Param,   "SYME_Param",	"parameter",        ALDOR_S_Syme_Param},
	{SYME_LexVar,  "SYME_LexVar",	"lexical variable", ALDOR_S_Syme_LexVar },      
	{SYME_LexConst,"SYME_LexConst", "lexical constant", ALDOR_S_Syme_LexConst},
	{SYME_Import,  "SYME_Import",	"import",	    ALDOR_S_Syme_Import},
	{SYME_Export,  "SYME_Export",	"export",	    ALDOR_S_Syme_Export},
	{SYME_Extend,  "SYME_Extend",	"extend",	    ALDOR_S_Syme_Extend},
	{SYME_Library, "SYME_Library",	"library",	    ALDOR_S_Syme_Library},
	{SYME_Archive, "SYME_Archive",	"archive",	    ALDOR_S_Syme_Archive},
	{SYME_Builtin, "SYME_Builtin",	"builtin",	    ALDOR_S_Syme_Builtin},
	{SYME_Foreign, "SYME_Foreign",	"foreign",	    ALDOR_S_Syme_Foreign},
	{SYME_Fluid,   "SYME_Fluid", 	"fluid variable",   ALDOR_S_Syme_Fluid},
	{SYME_Trigger, "SYME_Trigger", 	"trigger",	    ALDOR_S_Syme_Trigger},
	{SYME_Temp,    "SYME_Temp", 	"temp",	            ALDOR_S_Syme_Temp},
	{SYME_Has,     "SYME_Has", 	"has",	            ALDOR_S_Syme_Temp},
};

/******************************************************************************
 *
 * :: Table of information about syme fields
 *
 *****************************************************************************/

struct symeFieldInfo symeFieldInfo[] = {
	{ SYFI_Origin,		"origin",	(AInt) (Pointer)   NULL },
	{ SYFI_Original,	"original",	(AInt) (Syme)      NULL },
	{ SYFI_Extension,	"extension",	(AInt) (Syme)      NULL },
	{ SYFI_Condition,	"condition",	(AInt) listNil(Sefo) },
	{ SYFI_Twins,		"twins",	(AInt) listNil(Syme) },
	{ SYFI_Depths,		"depths",	(AInt) listNil(AInt) },
	{ SYFI_Mark,		"mark",		(AInt) (SefoMark)  0 },
	{ SYFI_DefLevel,	"defLevel",	(AInt) (StabLevel) NULL },
	{ SYFI_LibNum,		"libNum",	(AInt) SYME_NUMBER_UNASSIGNED},
	{ SYFI_VarIndex,	"varIndex",	(AInt) SYME_NUMBER_UNASSIGNED},
	{ SYFI_UsedDepth,	"usedDepth",	(AInt) SYME_DEPTH_UNUSED },
	{ SYFI_IntStepNo,	"intStepNo",	(AInt) (UShort)    0 },
	{ SYFI_FoamKind,	"foamKind",	(AInt) FOAM_LIMIT },
	{ SYFI_Closure,		"closure",	(AInt) (Foam)      NULL },
	{ SYFI_Inlined,		"inlined",	(AInt) listNil(Syme) },
	{ SYFI_DVMark,		"dvMark",	(AInt) (UShort)    0 },
	{ SYFI_SImpl,		"impl",		(AInt) (Pointer)  0 },
	{ SYFI_ConstLib,	"constLib",	(AInt) (Lib)       NULL },
	{ SYFI_ConstInfo,	"constInfo",	(AInt) SYME_NUMBER_UNASSIGNED},
	{ SYFI_DefnNum,		"defnNum",	(AInt) (int)       0 },
	{ SYFI_HashNum,		"hashNum",	(AInt) (int)       0 },
	{ SYFI_ExtraBits,	"extraBits",	(AInt) (int)       0 },
	{ SYFI_ConditionContext,"conditionContext",(AInt) (AbSyn) NULL },
};
