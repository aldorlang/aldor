/*****************************************************************************
 *
 * gf_add.c: Foam code generation for "add", "with", and default packages.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "debug.h"
#include "fluid.h"
#include "genfoam.h"
#include "gf_util.h"
#include "gf_prog.h"
#include "gf_add.h"
#include "gf_imps.h"
#include "gf_rtime.h"
#include "gf_seq.h"
#include "of_util.h"
#include "of_inlin.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "util.h"
#include "sefo.h"
#include "lib.h"
#include "tfsat.h"
#include "ablogic.h"
#include "abpretty.h"
#include "comsg.h"
#include "strops.h"
#include "table.h"

local Foam         gen0AddBody1           (AbSyn, Stab, AbSyn);
local void         gen0AddImportedDomain  (TForm, Foam, AInt);
local Foam         gen0CombineHash        (Foam hash1, Foam hash2);
local AbSyn        gen0FindDefaults       (AbSyn with);
local Foam         gen0GetDomainDomain    (TForm);
local Foam	   gen0MakeDefaultHash	  (void);
local Foam	   gen0RtDomainHash	  	(Foam);
local void	   gen0MakeDomainSelf	  (void);
local void	   gen0TypeAddDefaultSelfSlot	(void);
local void	   gen0MakeDomainDefaults (Foam);
local void	   gen0MakeCategoryParents(AbSyn);
local int	   gen0MakeCatParents0	  (AbSyn, Foam, int);
local int	   gen0MakeCatParentsIf	  (AbSyn, Foam, int);
local void         gen0MakeDomainParents  (AbSyn);
local void         gen0MakeTypeParents	  (Length, AbSyn *, String, AbSyn);
local void	   gen0MakeTypeParent	  (AbSyn, Length, Foam, AbSyn);

local void	   gen0TypeInitDomain	  (AbSyn, AbSyn);
local void	   gen0TypeInitDefaults	  (void);
local ExportState  gen0TypeInit		  (void);
local void	   gen0TypeOpen		  (Foam, String);
local void	   gen0TypeFini 	  (void);
local Foam	   gen0MakeTypeExportMap  (SymeList);

local Foam 	   gen0BuildExporterName  (AbSyn exporter);
local Foam         gen0SeenImportedDomain (TForm, AInt);
local void         gen0SetInitUsage       (Foam, AInt);

local AbSynList    gen0CollectAux	  (AbSyn);
local AbSyn	   gen0CollectWithImports (AbSyn);

local Foam	   gen0Join			(AbSyn);
local Foam 	   gen0Map		     	(AbSyn);
local Foam 	   gen0Enum		     	(AbSyn);
local Foam	   gen0ApplySpecialOthers	(AbSyn);
local Bool	   gen0AllSymesAllocated	(AbSyn);

local Foam 	   gen0RtTypeHash		(TForm, TForm);
local Foam 	   gen0RtTypeHashWith		(TForm, TForm, String);
local Foam	   gen0RtSefoHashExporter 	(Sefo);
local Foam	   gen0RtSefoHash		(Sefo, Sefo);

local void 	   gen0InitExports	     (void);
local void 	   gen0InitConditionalExport (Syme);
local SymeList 	   gen0AddExportedSymes	     (void);
local Foam 	   gen0HasJoin		     (Foam, int, AbSyn *);
local Foam 	   gen0HasCat		     (Foam, AbSyn);
local Foam 	   gen0HasImports	     (Foam, SymeList, Foam);
local Foam 	   gen0HasImport	     (Foam, Syme);

local void	   gen0StrRegister	     (int, String);

static ExportState 	gen0ExportState;
static Foam		gen0HasSelf;

Bool	genfExportDebug = false;
Bool	gfaddDebug	= false;

#define genfExportDEBUG		DEBUG_IF(genfExport)	afprintf
#define gfaddDEBUG		DEBUG_IF(gfadd)		afprintf

local void
gen0ClashCheck(AbSyn ab)
{
	Foam stmt, rhs;
	Foam seentab, arg1, arg2, arg3, arg4;
	TForm dom = abTForm(ab);
	/* Check ALL exports of this domain ... */
	SymeList symes = dom ? tfGetDomExports(dom) : 0;
	AbSyn exporter = gen0ProgGetExporter();
	String exp = exporter ? abPretty(exporter) : "(unknown)";
	long nelts = listLength(Syme)(symes);

	/* seentab = fiNewExportTable() */
	seentab = gen0TempLocal(FOAM_Word);
	rhs = foamNew(FOAM_BCall, 3, FOAM_BVal_NewExportTable,
			gen0CharArray(exp), foamNewSInt(nelts));
	stmt = foamNewSet(seentab, rhs);
	gen0AddStmt(stmt, ab);

	/* Add each export to the table (may generate runtime warning) */
	for (; symes; symes = cdr(symes)) {
		Syme syme = car(symes);
		TForm tf = symeType(syme);
		String str = symString(symeId(syme));
		String tfp = tfPretty(tf);

		arg1 = foamNewSInt(gen0StrHash(str));
		arg2 = gen0TypeHash(tf, tf, str);
		arg3 = gen0CharArray(str);
		arg4 = gen0CharArray(tfp);
		strFree(tfp);
		stmt = foamNew(FOAM_BCall, 6, FOAM_BVal_AddToExportTable,
				foamCopy(seentab), arg1, arg2, arg3, arg4);
		gen0AddStmt(stmt, ab);
	}


	/* fiFreeExportTable() */
	stmt = foamNew(FOAM_BCall, 2, FOAM_BVal_FreeExportTable,
			foamCopy(seentab));
	gen0AddStmt(stmt, ab);
}


/*
 * Create a getter function for a package.
 */

Foam
genAdd(AbSyn absyn)
{
        foamProgSetGetter(gen0State->program);
        return gen0AddBody0(absyn, abStab(absyn), tfExpr(gen0AbType(absyn)));
}

/*
 * This function generates code for add bodies.  
 *
 * A domain is represented by an Aldor record.  See axllib/runtime.as
 * for the detail of domain representation.  When first created, domains
 * contain only a single function, that, when called fills in the hash code
 * for the domain, and another function.  When the second function is called,
 * the domain get fully instantiated, and all the export slots are filled.
 * This is all done in a lazy way, so that requestors of information from a
 * domain need not worry about it.  Gen0AddBody0 creates this first function,
 * and gen0AddBody1 creates the second.
 *
 * The arguments:
 *      base: The left hand side of the add or with being processed, this
 *              is the parent of the domain or category.
 *      body: This is the code to be run when instantiating the type.
 *      stab: The symbol table for the body.
 *      defaultsAb: AbSyn for the defaults package.
 *
 */

Foam
gen0AddBody0(AbSyn ab, Stab stab, AbSyn defaultsAb)
{
	AbSyn		body = ab->abAdd.capsule;
	AInt		index;
        Foam            foam, clos;
	Length		argc = 1;
	String		argv[1];

	argv[0] = "domain";

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty("addLevel0", NULL);

	index = gen0FormatNum;
        gen0ProgPushState(stab, GF_Add0);

	if (genIsRuntime())
		gen0Vars(stab);
	else
		gen0ProgAddParams(argc, argv);

        gen0PushFormat(index);

        gen0State->program = foam;
        gen0State->program->foamProg.infoBits = IB_SIDE;

        if (genIsRuntime()) {
                genFoamStmt(body);
                gen0AddStmt(foamNewReturn(foamNewNil()), NULL);
        }
        else {
		AbSyn	exporter = gen0ProgGetExporter();
		Foam	rtHashCode = gen0SefoHashExporter(exporter);
		Foam	stmt;

		stmt = gen0BuiltinCCall(FOAM_Word, "domainAddNameFn!",
					"runtime",
				        2, foamNewPar(int0),
					gen0BuildExporterName(exporter));
		gen0AddStmt(stmt, NULL);
		stmt = gen0BuiltinCCall(FOAM_Word, "domainAddHash!", "runtime",
					2, foamNewPar(int0), rtHashCode);
                gen0AddStmt(stmt, NULL);
		stmt = foamNewReturn(gen0AddBody1(ab,stab,defaultsAb));
                gen0AddStmt(stmt, NULL);
        }

	gen0IssueDCache();
	gen0ProgAddFormat(index);
	gen0ProgFiniEmpty(foam, FOAM_Clos, int0);

        foamOptInfo(foam) = inlInfoNew(gen0State->stab, foam, NULL, false);
        foamProgSetGetter(foam);

	gen0ProgPopState();

        if (!genIsRuntime()) {
		clos = gen0BuiltinCCall(FOAM_Word, "domainMake", "runtime",
					1, clos);
		foamPure(clos) = true;
	}
        return clos;
}

Foam
gen0AddBody1(AbSyn ab, Stab stab, AbSyn defaultsAb)
{
	Scope("gen0AddBody1");
	AbSyn		base = ab->abAdd.base;
	AbSyn		body = ab->abAdd.capsule;
	ExportState	fluid(gen0ExportState);
	SymeList	symes = listNil(Syme);
	Foam		clos, foam;
	int		index;
	Length		argc = 2;
	String		argv[2];

	argv[0] = "domain";
	argv[1] = "hashcode";

	clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty("addLevel1", body);

        index = gen0FormatNum;
        gen0ProgPushState(stab, GF_Add1);

	gen0ProgAddParams(argc, argv);

	gen0Vars(stab);

        gen0PushFormat(index);

        gen0State->program = foam;
        gen0State->program->foamProg.infoBits = IB_SIDE;

	gen0FindUncondSymes(body, symes);
	/* Add parents and defaults */
	gen0ExportState = gen0TypeInit();
	gen0TypeInitDomain(base, defaultsAb);

        /* generate code for add body. */
        gen0DefTypeSequence(body, gen0ExportState->domExportList);

	/* Generate code to check for export clashes */
	if (genHashcheck()) gen0ClashCheck(ab);

	gen0TypeFini();

        gen0AddStmt(foamNewReturn(foamNewPar(int0)), NULL);

        gen0State->hasTemps = true;

	gen0ProgAddFormat(index);
	gen0ProgFiniEmpty(foam, FOAM_Word, int0);

        foamOptInfo(foam) = inlInfoNew(gen0State->stab, foam, NULL, false);
        foamProgSetGetter(foam);

	gen0ProgPopState();

        Return(clos);
}

Foam
gen0MakeDefaultPackage(AbSyn base, Stab stab, Bool inCatForm, Syme syme)
{
	Scope("gen0MakeDefaultPackage");
	ExportState 	fluid(gen0ExportState);
	AbSyn		defs = NULL;
	Syme		self = stabGetSelf(stab);
	SymeList	symes = listNil(Syme);
	Foam		foam, clos;
	int		index, j;
	Foam 		hasher;

	Length		argc = 2;
	String		argv[2];
	argv[0] 	= "self";
	argv[1] 	= "dom";

	if (base && abTag(base) == AB_With) {
		defs = gen0FindDefaults(base->abWith.within);
		base = gen0CollectWithImports(base);
	}

	if (base && abIsJoin(base)) {
		Length	jargc = abApplyArgc(base);
		if (jargc == 0)
			base = abNewNothing(sposNone);
		if (jargc == 1)
			base = abApplyArg(base, int0);
	}

	if ((base == NULL || abIsNothing(base)) &&
	    (defs == NULL || abIsNothing(defs)) &&
	    !inCatForm)
		Return(foamNewNil());

	if (!abIsJoin(base) &&
	    (defs == NULL || abIsNothing(defs)) &&
	    (abIsId(base) || abIsApply(base)) &&
#if 0
	    (self == NULL || symeUnused(self)) &&
#endif
	    !inCatForm)
		Return(genFoamType(base));

	hasher = gen0MakeDefaultHash();

        clos = gen0ProgClosEmpty();
	foam = gen0ProgInitEmpty(gen0ProgName, base);

	index = gen0FormatNum;
	gen0ProgPushState(stab, inCatForm ? GF_DefaultCat : GF_Default);

	gen0State->type = tfCategory;
	gen0State->stab = stab;

	gen0State->program	    = foam;
	gen0State->program->foamProg.infoBits = IB_SIDE;

	gen0ProgAddParams(argc, argv);
	if (self) symeSetUsedDeeply(self);
	gen0Vars(stab);
	gen0PushFormat(index);

	if (defs) 
		gen0FindUncondSymes(defs, symes);

	gen0ExportState = gen0TypeInit();

	if (self) {
		gen0ExportState->self     = gen0Syme(self);
		gen0ExportState->selfSyme = self;
	}
	else {
		j = gen0AddLex(foamNewDecl(FOAM_Word, strCopy("self"),
					   emptyFormatSlot));
		gen0ExportState->self = foamNewLex(int0, j);
	}
	gen0AddInit(foamNewDef(foamCopy(gen0ExportState->self),
			       foamNewPar(1)));
	gen0ExportState->selfHash = gen0TempLex(FOAM_SInt);
	gen0AddInit(foamNewSet(foamCopy(gen0ExportState->selfHash),
			       gen0RtDomainHash(gen0ExportState->self)));

	gen0TypeInitDefaults();
	gen0MakeCategoryParents(base);

	if (defs) 
		gen0DefTypeSequence(defs, gen0ExportState->domExportList);
	
	gen0TypeFini();

	gen0AddStmt(foamNewReturn(foamNew(FOAM_Values, int0)), NULL);

	gen0ProgAddFormat(index);
	gen0ProgFiniEmpty(foam, FOAM_NOp, int0);

	foamOptInfo(foam) = inlInfoNew(gen0State->stab, foam, syme, false);
        foamProgSetGetter(foam);
	if (foam->foamProg.levels->foamDEnv.argv[0] != emptyFormatSlot)
		foamProgUnsetLeaf(foam);
	/*gen0ComputeSideEffects(foam);*/
	gen0ProgPopState();
	
	
	foam = gen0BuiltinCCall(FOAM_Word,
				"categoryMake", "runtime", 3, clos,
				hasher,
				gen0BuildExporterName(gen0ProgGetExporter()));
	Return(foam);
}

local Foam
gen0MakeDefaultHash()
{
	AbSyn exp = gen0ProgGetExporter();
	Foam hashCode;
	
	if (exp == NULL || abTag(exp) == AB_Id) {
		hashCode = gen0SefoHashExporter(exp);
		return gen0BuiltinCCall(FOAM_Clos, "rtConstSIntFn", 
					"runtime", 1, hashCode);
	}
	else {
		GenFoamState saved;
		Foam 	     foam, clos;
		
		clos = gen0ProgClosEmpty();
		foam = gen0ProgInitEmpty("defhash0", NULL);
		
		saved = gen0ProgSaveState(PT_ExFn);
		
		hashCode = gen0SefoHashExporter(exp);
		
		gen0AddStmt(foamNewReturn(hashCode), NULL);
		
		gen0ProgPushFormat(emptyFormatSlot);
		gen0ProgFiniEmpty(foam, FOAM_SInt, int0);
		gen0AddLexLevels(foam, 1);
		foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);
		
		gen0ProgRestoreState(saved);
		
		return clos;
	}
}
	
	
	
local void
gen0TypeInitDomain(AbSyn base, AbSyn defaultsAb)
{
	Stab nstab;
	Foam defaults;
	
	gen0TypeOpen(foamNewPar(int0), "domainAddExports!");

	gen0MakeDomainSelf();
	
	nstab = defaultsAb ? abStab(defaultsAb) : NULL;
	defaults = gen0MakeDefaultPackage(defaultsAb, nstab, false, NULL);

	gen0MakeDomainDefaults(defaults);
	
	gen0MakeDomainParents(base);
}

local void
gen0TypeInitDefaults()
{
	gen0TypeOpen(foamNewPar(int0), "categoryAddExports!");
	
	if (gen0IsCatDefForm(gen0State))
		gen0TypeAddDefaultSelfSlot();

}

/* COND-DEF */
local Hash
gen0CondHash(GfCond cond)
{
	return (Hash)ptrCanon(cond->syme);
}

local Bool
gen0CondEq(GfCond a, GfCond b)
{
	if ((a->syme) != (b->syme)) return false;
	return ablogEqual(a->condition, b->condition);
}

local ExportState
gen0TypeInit()
{
	ExportState exportState;
	SymeList 	symes;
	Foam 		szVar = gen0TempLocal(FOAM_SInt);
	Foam 		names, types, vals;
	FoamList	place;	

        /* collect the exports. */
        if (gen0State->stab)
                symes = gen0AddExportedSymes();
        else
                symes = 0;
	/* set for array sizes */
	gen0AddInit(foamNewNOp());
	place = gen0State->inits;

	/* Create the foam arrays for the domain vectors. */
	names = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	types = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	vals  = gen0TempLocal0(FOAM_Arr, FOAM_Word);

	gen0AddInit(foamNewSet(names, foamNewANew(FOAM_Word, 
						  foamCopy(szVar))));
	gen0AddInit(foamNewSet(types, foamNewANew(FOAM_Word, 
						  foamCopy(szVar))));
	gen0AddInit(foamNewSet(vals,  foamNewANew(FOAM_Word, 
						  foamCopy(szVar))));

	exportState = (ExportState) stoAlloc(OB_Other, sizeof(*exportState));

	exportState->foamLevel = gen0State->foamLevel;
	exportState->domExportList = symes;
	exportState->domInittedTbl = tblNew(NULL, NULL);
	/* COND-DEF */
	exportState->domCondTbl = tblNew((TblHashFun)gen0CondHash,
					(TblEqFun)gen0CondEq);
	exportState->defMap   = gen0MakeTypeExportMap(symes);
	exportState->setPlace = place;
	exportState->szVar    = szVar;
	exportState->size     = 0;
	exportState->names    = names;
	exportState->types    = types;
	exportState->vals     = vals;
	exportState->selfSyme = NULL;
	return exportState;
}

local void
gen0TypeOpen(Foam type, String exporterName)
{
	Foam size = gen0ExportState->szVar;
	Foam names, types, vals;

	/* Create the Aldor arrays for the domain vectors. */
	names = gen0MakeArray(foamCopy(size), gen0ExportState->names, NULL);
	types = gen0MakeArray(foamCopy(size), gen0ExportState->types, NULL);
	vals  = gen0MakeArray(foamCopy(size), gen0ExportState->vals,  NULL);

	gen0AddStmt(gen0BuiltinCCall(FOAM_NOp, exporterName, "runtime", 4, 
				     type, names, types, vals), NULL);
}

local void
gen0TypeFini()
{
	gen0InitExports();

	gen0IssueDCache();

	setcar(gen0ExportState->setPlace,
	       foamNewSet(foamCopy(gen0ExportState->szVar), 
			  foamNewSInt(gen0ExportState->size)));

	listFree(Syme)(gen0ExportState->domExportList);
	tblFree(gen0ExportState->domInittedTbl);

	stoFree(gen0ExportState);
}

void 
gen0TypeAddExportSlot(Syme syme)
{
	TForm	tf = symeType(syme);
	Foam 	name, type, val, lhs;
	int 	posn;
	String  str  = symString(symeId(syme));
	
	if (!gen0IsDomLevel(gen0State->tag) || gen0State->tag == GF_File)
	    return;

	/* Get the location which holds the value of the export */
	lhs = gen0SymeInit(syme);
	if (!lhs)
	{
		AbSyn	ab = (AbSyn)NULL;
		String	etype = tfPretty(tf);
		comsgFatal(ab, ALDOR_F_BugExportSymeNotInit, str, etype);
	}


	/*
	 * Conditional exports get a flag so that runtime checks can be
	 * made to determine whether or not they have been initialised.
	 */
	if (!symeUnconditional(syme)) {
		gen0AddInit(foamNewSet(gen0ExpMapRef(syme), 
				       foamNewBool(false)));
		gen0AddStmt(foamNewSet(gen0ExpMapRef(syme), 
				       foamNewBool(true)), NULL);
	}
	posn = gen0ExportState->size++;
	name = foamNewSInt(gen0StrHash(str));
	type = gen0TypeHash(tf, tf, str);


	/* COND-DEF */
	if (!symeUnconditional(syme)) {
		Foam def, rhs;


		/* Get the local holding the export value */
		rhs = gen0SymeCond(syme);


		/* Safety check */
		assert(rhs);


		/* Create definition for this export */
		def = foamNewDef(foamCopy(lhs), foamCopy(rhs));


		/* Retrieve the DefnId stashed by gen0Define */
		def->foamDef.hdr.defnId = rhs->foamGen.hdr.defnId;


#if 0
		/* Debugging output */
		(void)fprintf(dbOut, "--- [%d] ", def->foamDef.hdr.defnId);
		foamPrintDb(rhs);
		(void)fprintf(dbOut, "   ");
		symePrintDb(syme);
		(void)fprintf(dbOut, "   ");
		ablogPrintDb(gfCondKnown);
#endif


		/* Define the export */
		gen0AddStmt(def, NULL);

	}

        val  = foamCopy(lhs);


	/* Wrap non-word exports */
        if (gen0Type(tf, NULL) != FOAM_Word)
                val = foamNewCast(FOAM_Word, val);

        gen0AddStmt(gen0ASet(gen0ExportState->names, 
			     posn, FOAM_Word, foamNewCast(FOAM_Word, name)),
		    NULL);
        gen0AddStmt(gen0ASet(gen0ExportState->types, 
			     posn, FOAM_Word, foamNewCast(FOAM_Word, type)),
		    NULL);
        gen0AddStmt(gen0ASet(gen0ExportState->vals,  
			     posn, FOAM_Word, val),  NULL);

	gen0StrRegister(name->foamSInt.SIntData, str);

	return;
}

/*
 * Walk the tree looking for symes at top level. We ought to
 * do this in scobind and we ought to deal with implicit
 * exports as well. For now we assume that implicit exports
 * are unconditional and mark them in gen0DefTypeSequence().
 */
void
gen0FindUncondSymes(AbSyn absyn, SymeList symes)
{
	int i, argc;
	AbSyn lhs, *argv;
	Syme syme;

	switch(abTag(absyn)) {
	  case AB_Define:
		lhs = absyn->abDefine.lhs;
		if (abTag(lhs) == AB_Comma) {
			argc = abArgc(lhs);
			argv = lhs->abComma.argv;
		} 
		else {
			argc = 1;
			argv = &lhs;
		}
		for (i=0; i< argc; i++) {
			if (abTag(argv[i])==AB_Declare)
				syme = abSyme(argv[i]->abDeclare.id);
			else {			
				assert(abTag(argv[i])==AB_Id);
				syme = abSyme(argv[i]);
			}
			if (symeIsExport(syme)||symeIsExtend(syme))
				symeSetUnconditional(syme);
		}
		break;
	  case AB_Where:
		gen0FindUncondSymes(absyn->abWhere.expr, symes);
		break;
	  case AB_Default:
	  case AB_Sequence:
		for (i=0; i<abArgc(absyn); i++) {
			if (abTag(abArgv(absyn)[i]) == AB_Exit) break;
			gen0FindUncondSymes(abArgv(absyn)[i], symes);
		}
		break;
	  case AB_Local:
		for (i=0; i<abArgc(absyn); i++) {
			gen0FindUncondSymes(abArgv(absyn)[i], symes);
		}
		break;
	  default:
		break;
	}
}

/*
 * Create a local for self for a domain if needed.
 */
local void
gen0MakeDomainSelf()
{
	SymeList sl;
	Foam	result, set;

	gen0ExportState->self = gen0TempLocal(FOAM_Word);
	result = gen0BuiltinCCall(FOAM_Word, "domainMakeDispatch",
					  "runtime", 1, foamNewPar(int0));
	foamPure(result) = true;
	gen0AddInit(foamNewSet(foamCopy(gen0ExportState->self), result));
	/* Tacky --- stabGetSelf gives an inner version, 
	   so we go look for outer */
	sl = gen0ExportState->domExportList;
	while (sl && symeId(car(sl)) != ssymSelf)
		sl = cdr(sl);

	/* This was conditional on (sl && !symeUnused(car(sl))) 
	 * ie. used deeply.  The optimisations for self in tfFloat 
	 * break this.
	 */
	if (sl) {
		set = gen0Syme(car(sl));
		gen0AddInit(foamNewSet(set,
				       foamCopy(gen0ExportState->self)));
		gen0SymeSetInit(car(sl), set);
		symeSetUnconditional(car(sl));
	}

	gen0ExportState->selfSyme = sl ? car(sl) : NULL;
	gen0ExportState->selfHash = gen0TempLex(FOAM_SInt);
	gen0AddInit(foamNewDef(gen0ExportState->selfHash, foamNewCast(FOAM_SInt,
								      foamNewPar(1))));
}

Bool
gen0HasDefaults(AbSyn absyn)
{
	int	i;
	AbSyn	within = absyn->abWith.within;

	for(i=0; i<abArgc(within); i++) {
		if (abTag(abArgv(within)[i]) == AB_Default)
			return true;
		/* Test should be improved.. */
		if (abTag(abArgv(within)[i]) == AB_If)
			return true;
	}
	return false;
}

local void
gen0TypeAddDefaultSelfSlot()
{
	AbSyn abType = gen0ProgGetExporter();
	Foam  rtHash;
	int i;

	if (DEBUG(phase)) {
		fprintf(dbOut, "Make slot: exporter is:\n");
		abWrSExpr(dbOut, abType,int0);
	}

	assert(gen0ExportState->self);
	rtHash = gen0SefoHashExporter(abType);

	i = gen0ExportState->size++;
	gen0AddStmt(gen0ASet(gen0ExportState->names, i, FOAM_Word, 
			     foamNewCast(FOAM_Word, 
					 foamNewSInt(gen0StrHash("%%")))), 
		    NULL);
	gen0AddStmt(gen0ASet(gen0ExportState->types, i, FOAM_Word,
			     foamNewCast(FOAM_Word, rtHash)),
		    NULL);
	gen0AddStmt(gen0ASet(gen0ExportState->vals, i, FOAM_Word, 
			     foamCopy(gen0ExportState->self)), 
		    NULL);
	assert(foamTag(gen0ExportState->self) == FOAM_Lex);
	gen0UseStackedFormat(gen0ExportState->self->foamLex.level);
}

/*
 * Create the defaults for a domain.
 */
local void
gen0MakeDomainDefaults(Foam defaults)
{
	Foam result;

	if (!defaults || foamTag(defaults) == FOAM_Nil) return;

	result = gen0BuiltinCCall(FOAM_Word, "domainAddDefaults!",
				  "runtime", 3, foamNewPar(int0), defaults, 
				  foamCopy(gen0ExportState->self));
	gen0AddStmt(result, NULL);

	gen0SetInitUsage(defaults, int0);
}

/*
 * Evaluate the parents of a domain.
 */
local void
gen0MakeCategoryParents(AbSyn base)
{
	Foam 	 pars, result;
	FoamList place;
	int 	 sz;

	assert(base);
	if (abIsNothing(base)) return;
	
	pars = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	gen0AddInit(foamNewNOp());
	place = gen0State->inits;
	sz = gen0MakeCatParents0(base, pars, int0);
	/* fixup */
	if (sz == 0)
		return;
	setcar(place, 
	       foamNewSet(foamCopy(pars),
			  foamNewANew(FOAM_Word, foamNewSInt(sz))));

	/* Create the Aldor array for the parents. */
	pars = gen0MakeArray(foamNewSInt(sz), foamCopy(pars), base);

	result = gen0BuiltinCCall(FOAM_Word, "categoryAddParents!", 
				  "runtime", 3,
				  foamNewPar(int0), pars, 
				  foamCopy(gen0ExportState->self));
	gen0AddStmt(result, NULL);
}

local int
gen0MakeCatParents0(AbSyn ab, Foam pars, int idx)
{
	int i;
        if (abIsJoin(ab)) {
		for (i=0; i<abApplyArgc(ab) ; i++) {
			idx = gen0MakeCatParents0(abApplyArg(ab, i), 
						  pars, idx);
		}
	}
	else switch (abTag(ab)) {
	  case AB_Sequence:
		for (i=0; i<abArgc(ab) ; i++)
			idx = gen0MakeCatParents0(abArgv(ab)[i], 
						  pars, idx);
		break;
	  case AB_If:
		idx = gen0MakeCatParentsIf(ab, pars, idx);
		break;
	  case AB_Default:
		gen0DefTypeSequence(ab,
				    gen0ExportState->domExportList);
		break;
	  case AB_Declare:
	  case AB_Export:
	  case AB_Nothing:
		break;
	  case AB_Comma:
		assert(abArgc(ab) == 0);
		break;
	  default:
		gen0MakeTypeParent(ab, idx, foamCopy(pars), NULL);
		idx ++;
	}
	return idx;
}


local int
gen0MakeCatParentsIf(AbSyn ab, Foam pars, int idx)
{
	FoamList topLines;
	Foam	 low;
	int   l1 = gen0State->labelNo++, l2 = gen0State->labelNo++;
	int   l3 = gen0State->labelNo++, l4 = gen0State->labelNo++;
	int   idx1, idx2;
	Bool  flag;

 	/* COND-DEF */
	AbLogic	saveCond;
	AbSyn	nTest;
	Stab	stab = abStab(ab) ? abStab(ab) : stabFile();
	extern AbSyn abExpandDefs(Stab, AbSyn);
	
	flag = gen0AddImportPlace(&topLines);
	
	nTest = abExpandDefs(stab, (ab->abIf.test)); /* COND-DEF */

	/* 
	 * Plan is:
	 *   (if cond L1)
	 *   idx1 := Fill <else> part from idx
	 *   low  := idx1
	 *   goto L2
	 * L1:
	 *   idx2 := Fill <then> part from idx
	 *   low := idx2
	 * L2:
	 *   high := max(idx1, idx2)
	 * L3:
	 *   if (high = low) L4;
	 *   arr.low := NIL;
	 *   low++;
	 * L4
	 */
	low  = gen0TempLocal(FOAM_SInt);
	 
	gen0AddStmt(foamNewIf(genFoamBit(ab->abIf.test), l1), ab);
	ablogAndPush(&gfCondKnown, &saveCond, nTest, false); /* COND-DEF */
	idx1 = gen0MakeCatParents0(ab->abIf.elseAlt, pars, idx);
	ablogAndPop (&gfCondKnown, &saveCond); /* COND-DEF */
	gen0AddStmt(foamNewSet(foamCopy(low), foamNewSInt(idx1)), ab);
	
	gen0AddStmt(foamNewGoto(l2), ab);
	gen0AddStmt(foamNewLabel(l1), ab);

	ablogAndPush(&gfCondKnown, &saveCond, nTest, true); /* COND-DEF */
	idx2 = gen0MakeCatParents0(ab->abIf.thenAlt, pars, idx);
	ablogAndPop (&gfCondKnown, &saveCond); /* COND-DEF */
	gen0AddStmt(foamNewSet(foamCopy(low), foamNewSInt(idx2)), ab);
	gen0AddStmt(foamNewLabel(l2), ab);
	
	idx = idx1>idx2 ? idx1 : idx2;

	gen0AddStmt(foamNewLabel(l3), ab);
	gen0AddStmt(foamNewIf(foamNew(FOAM_BCall, 3, FOAM_BVal_SIntEQ,
				      foamNewSInt(idx),
				      foamCopy(low)),
			      l4), ab);
	gen0AddStmt(foamNewSet(foamNewAElt(FOAM_Word, 
					   foamCopy(low),
					   foamCopy(pars)),
			       foamNewCast(FOAM_Word, foamNewNil())),
		    ab);
	gen0AddStmt(foamNewSet(foamCopy(low),
			       foamNew(FOAM_BCall, 2,
				       FOAM_BVal_SIntNext, 
				       foamCopy(low))),
		    ab);
	gen0AddStmt(foamNewGoto(l3), ab);
	gen0AddStmt(foamNewLabel(l4), ab);

	foamFree(low);
	if (flag) gen0ResetImportPlace(topLines);
			       
	return idx;
}

local void 
gen0MakeDomainParents(AbSyn base)
{
	if (abIsNothing(base)) return;

	gen0MakeTypeParents(1, &base, "domainAddParents!", base);
}


local void
gen0MakeTypeParents(Length argc, AbSyn *argv, String adderName, AbSyn base)
{
	Foam	pars, result;
	Length	i;

	/* Create the foam array for the parent vector. */
	pars = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	gen0AddStmt(gen0ANew(pars, FOAM_Word, argc), base);

	/* Fill the slots in the foam array. */
	for (i = 0; i < argc; i += 1)
		gen0MakeTypeParent(argv[i], i, pars, base);

	/* Create the Aldor array for the domain parents. */
	pars = gen0MakeArray(foamNewSInt(argc), pars, base);

	result = gen0BuiltinCCall(FOAM_Word, adderName, "runtime", 3,
				  foamNewPar(int0), pars, 
				  foamCopy(gen0ExportState->self));
	gen0AddStmt(result, NULL);
}

/*
 * Stuff the parent vector slot for a single parent in the add/with chain.
 */
local void
gen0MakeTypeParent(AbSyn elt, Length i, Foam pars, AbSyn absyn)
{
	FoamTag		type = gen0Type(gen0AbType(elt), NULL);
	Foam		par;

	par = genFoamVal(elt);
	if (type != FOAM_Word)
		par = foamNewCast(FOAM_Word, par);

	gen0AddStmt(gen0ASet(pars, i, FOAM_Word, par), absyn);
}

local Foam
gen0MakeTypeExportMap(SymeList symes)
{
	Foam mapVar;

	mapVar = gen0Temp0(FOAM_Arr, FOAM_Bool);
	gen0AddInit(gen0ANew(mapVar, FOAM_Bool, listLength(Syme)(symes)));
	return mapVar;
}

int
gen0ExpMapPos(Syme syme)
{
	return listPosq(Syme)(gen0ExportState->domExportList, syme);
}

Foam
gen0ExpMapRef(Syme syme)
{	/* !! Assumes syme came from gen0ExportState->domExports */
	return foamNewAElt(FOAM_Bool, foamNewSInt(gen0ExpMapPos(syme)),
			   foamCopy(gen0ExportState->defMap));
}

local void
gen0InitExports()
{
	SymeList sl;

	for (sl = gen0ExportState->domExportList; sl ; sl=cdr(sl)) {
		Syme syme = car(sl);
		/* !! Eek */
		if (symeIsExtend(syme))
			continue;
		/* 
		 * The game we play with defaults is to assign them to
		 * locals (see gen0Define), export them and then use
		 * InitExports to pull in then binding.
		 * !!Unfortunately, this fails if the export is
		 * something that isn't lazy.
		 */
		if (symeHasDefault(syme) && gen0SymeInit(syme))
			gen0InitExport(syme);
		else if (symeUsed(syme) && !symeUnconditional(syme) && 
			 gen0SymeInit(syme) != NULL)
			gen0InitConditionalExport(syme);
		else if (symeUsed(syme) && gen0SymeInit(syme) == NULL)
			gen0InitExport(syme);
	}
}

local void
gen0InitConditionalExport(Syme syme)
{
	int label;
	
	if (!gen0IsDomLevel(gen0State->tag) || gen0State->tag == GF_File)
		return;

	label = gen0State->labelNo++;

	gen0AddStmt(foamNewIf(gen0ExpMapRef(syme), label), NULL);
	gen0InitExport(syme);
	gen0AddStmt(foamNewLabel(label), NULL);
}

/*
 * This code generator tries to avoid domain lookup failures
 * by testing for an export before attempting to forcing the
 * lazy version. If the export is not found then no forcing
 * is performed and (SInt 0) returned. Hopefully by the time
 * that the export really is needed we will have initialised
 * it by some other route.
 *
 * We want to generate the following code:
 *
 *     if (self has foo) L0;
 *     tmpvar = (SInt 0);
 *     goto L1;
 * L0: tmpvar = lazyForceExport! val;
 * L1: ...
 *
 * This trick helps to avoid bug #1221: while building % we
 * try to initialise another domain parameterised by %. This
 * domain is actually a domain-valued function and eagerly
 * asks us a question. Unfortunately this causes us to force
 * a simple (non-lambda) export that we haven't been able to
 * create yet. Luckily the forced export isn't needed yet:
 * if it was then we are in trouble.
 */
local Foam
gen0TryForceLazy(Foam self, Syme syme, Foam lazy)
{
	Bool		flag;
	Foam		test, tmpvar, val;
	FoamList	topLines;
	int		l1, l2;
	AbSyn		absyn = (AbSyn)NULL;


	/* Get some labels */
	l1 = gen0State->labelNo++;
	l2 = gen0State->labelNo++;


	/* Standard guff */
	flag = gen0AddImportPlace(&topLines);


	/* Export values must fit into a word */
	tmpvar = gen0TempLocal(FOAM_Word);


	/* Create the "has" test */
	test = gen0HasImport(foamCopy(self), syme);
	test = foamNewCast(FOAM_Bool, test);


	/* Test for the export that we want to force */
	gen0AddStmt(foamNewIf(test, l1), absyn);


	/* Export not found: use a neutral value/bad pointer */
	val = foamNewCast(FOAM_Word, foamNewSInt(int0));
	gen0AddStmt(foamNewSet(foamCopy(tmpvar), val), absyn);


	/* Jump to the place where we update the export */
	gen0AddStmt(foamNewGoto(l2), absyn);


	/* Target for the successful if-test */
	gen0AddStmt(foamNewLabel(l1), absyn);


	/* Force the lazy export */
	val = gen0LazyValue(lazy, syme);
	gen0AddStmt(foamNewSet(foamCopy(tmpvar), val), absyn);


	/* Target for the export update */
	gen0AddStmt(foamNewLabel(l2), absyn);


	/* Standard guff */
	if (flag) gen0ResetImportPlace(topLines);


	/* Return the value of the export */
	return tmpvar;
}

void
gen0InitExport(Syme syme)
{
	Foam	self, val;

	assert(symeIsExport(syme)||symeIsExtend(syme));
	assert(gen0ExportState->self);

	self = foamCopy(gen0ExportState->self);
	val  = gen0GetDomImport(syme, self);

	foamSyme(val) = syme;

	if (gen0IsLazyConst(symeType(syme)))
		val = gen0TryForceLazy(self, syme, val);

	gen0AddStmt(foamNewSet(gen0Syme(syme), val), NULL);
	if (foamTag(self) == FOAM_Lex)
		gen0UseStackedFormat(self->foamLex.level);
}

local SymeList
gen0AddExportedSymes()
{
	SymeList symes;

	if (gen0State->parent &&
	    gen0State->parent->parent &&
	    gen0State->parent->parent->tag == GF_Lambda &&
	    gen0State->parent->parent->stab) {
		symes = stabGetExportedSymes(gen0State->parent->parent->stab);
		symes = listConcat(Syme)(stabGetExportedSymes(gen0State->stab),
				         symes);
	}
	else symes = stabGetExportedSymes(gen0State->stab);
	
	return symes;
}


void
gen0SymeSetInit(Syme syme, Foam foam)
{
	tblSetElt(gen0ExportState->domInittedTbl, syme, foam);
}

Foam 
gen0SymeInit(Syme syme)
{
	return (Foam) tblElt(gen0ExportState->domInittedTbl, syme, NULL);
}


/* Conditional export */
/* COND-DEF */
Foam
gen0SymeCond(Syme syme)
{
	Foam	result;
	_GfCond	cond;

	cond.syme = syme;
	cond.condition = gfCondKnown;

	result = (Foam) tblElt(gen0ExportState->domCondTbl, &cond, NULL);
	return result;
}

/* COND-DEF */
void
gen0SymeSetCond(Syme syme, Foam foam)
{
	GfCond	cond = (GfCond)stoAlloc(OB_Other, sizeof(*cond));

	cond->syme = syme;
	cond->condition = ablogCopy(gfCondKnown);

	tblSetElt(gen0ExportState->domCondTbl, cond, foam);
}


/* Return a simpler sefo with the same type */

Sefo
gen0EqualMods(Sefo sefo)
{
	Bool	changed = (sefo != NULL);

	while (changed)
		switch (abTag(sefo)) {
		case AB_PretendTo:
			sefo = sefo->abPretendTo.expr;
			break;
		case AB_RestrictTo:
			sefo = sefo->abRestrictTo.expr;
			break;
		case AB_Qualify:
			sefo = sefo->abQualify.what;
			break;
		case AB_Assign:
			sefo = sefo->abAssign.lhs;
			break;
		case AB_Define:
			sefo = sefo->abDefine.lhs;
			break;
		case AB_Documented:
			sefo = sefo->abDocumented.expr;
			break;
		case AB_Declare:
			sefo = sefo->abDeclare.id;
			break;
		case AB_For:
			sefo = sefo->abFor.lhs;
			break;
		case AB_Local:
		case AB_Free:
		case AB_Sequence:
		case AB_Comma:
			if (abArgc(sefo) == 1)
				sefo = abArgv(sefo)[0];
			else
				changed = false;
			break;
		default:
			changed = false;
			break;
		}

	/*
	 * AB_Except is a bit of an oddity - in this case, the lhs won't be
	 * simplified, which may mean this function doesn't quite work.
	 */
	assert(sefo == NULL ||
	       abTag(sefo) == AB_Id ||
	       abTag(sefo) == AB_Apply ||
	       abTag(sefo) == AB_Except ||
	       (abIsLeaf(sefo) && abSyme(sefo)));
	return sefo;
}


Foam 
gen0LocalSelf()
{
	Foam foam = foamCopy(gen0ExportState->self);

	if (gen0HasSelf)
		return foamCopy(gen0HasSelf);

	gen0AddLexLevels(foam,
			 gen0State->foamLevel
			 - gen0ExportState->foamLevel);
	return foam;
	
}

Syme
gen0LocalSelfSyme()
{
	if (gen0ExportState == NULL)
		return NULL;
	return gen0ExportState->selfSyme;
}

/*****************************************************************************
 *
 * :: Selection of an appropriate hashing scheme, and delayed 
 *    export hashcode finding
 *
 ****************************************************************************/

local Bool 	gen0SefoListIsCachable	(SefoList);
local Foam	gen0DCacheAddItem	(HashType, Pointer, Pointer);
local Bool	gen0DCacheMatch		(DomainCache dc, 	
					 HashType type, 
					 Pointer arg0, Pointer arg1);
local void	gen0IssueDCache1	(void);
static Bool gen0DCacheInIssue = false;

Foam
gen0TypeHash(TForm tf, TForm otf, String name)
{
	SefoList sfl = gen0GetSefoInnerSefos(tfExpr(tf));
	
	if (tfTag(otf) == TF_With)
		gen0RtTypeHashWith(tf, otf, name);

	if (!gen0DCacheInIssue && gen0SefoListIsCachable(sfl)) {
		listFree(Sefo)(sfl);
		return gen0DCacheAddItem(HT_TFormHash, tf, otf);
	}
	listFree(Sefo)(sfl);
	return gen0RtTypeHash(tf, otf);
}

Foam 
gen0SefoHashExporter(Sefo sf)
{
	return gen0RtSefoHashExporter(sf);
}

Foam 
gen0SefoHash(Sefo sf, Sefo osf)
{
	SefoList sfl = gen0GetSefoInnerSefos(sf);

	if (!gen0DCacheInIssue && gen0SefoListIsCachable(sfl)) {
		listFree(Sefo)(sfl);
		return gen0DCacheAddItem(HT_SefoHash, sf, osf);
	}
	listFree(Sefo)(sfl);
	return gen0RtSefoHash(sf, osf);
}

/* Can we make a cache at the start of a function... */
local Bool 
gen0SefoListIsCachable(SefoList sfl)
{
	Sefo 	 sf;
	Syme	 syme;

	while (sfl != listNil(Sefo)) {
		sf = car(sfl);
		if (abTag(sf) != AB_Id)
			return false;
		syme = abSyme(sf);
		if (!syme)
			return false;
		if (!symeIsSelf(syme)
		    && !symeIsParam(syme))
			return false;
		/* In 'has' expression => Weird self */
		if (symeIsSelf(syme) && gen0HasSelf) return false;
		sfl = cdr(sfl);
	}
	return true;
}

local Foam
gen0DCacheAddItem(HashType type, Pointer arg0, Pointer arg1)
{
	DomainCacheList dcl;
	DomainCache 	cache, dc;
	Foam 	    	tmp;

	dcl = gen0State->domCache;

	while (dcl != listNil(DomainCache)) {
		dc = car(dcl);
		if (gen0DCacheMatch(dc, type, arg0, arg1))
			return foamCopy(dc->hashVar);

		dcl = cdr(dcl);
	}

	tmp = gen0Temp(FOAM_SInt);
	
	cache = (DomainCache) stoAlloc(OB_Other, sizeof(*cache));
	cache->hashVar  = tmp;
	cache->type	= type;
	cache->args[0]  = arg0;
	cache->args[1]  = arg1;

	if (type == HT_Id)
		gen0State->domCache = 
			listNConcat(DomainCache)
				(gen0State->domCache,
				 listCons(DomainCache)(cache, 
						       listNil(DomainCache)));
	else
		gen0State->domCache = 
			listCons(DomainCache)(cache, gen0State->domCache);

	return foamCopy(tmp);
}

local Bool
gen0DCacheMatch(DomainCache dc, HashType type, Pointer arg0, Pointer arg1)
{
	if (dc->type != type)
		return false;

	if (dc->args[0] == arg0 &&
	    dc->args[1] == arg1) 
		return true;
	if (type == HT_SefoHash)
		return sefoEqual((Sefo)(dc->args[0]), (Sefo) arg0);
	if (type == HT_TFormHash)
		return tfEqual((TForm)(dc->args[0]), (TForm) arg0);

	return false;
}

/* Flushing the cache */
void
gen0IssueDCache()
{
	gen0IssueDCache1();
	/* HT_Id's may appear */
	if (gen0State->domCache)
		gen0IssueDCache1();
	
	assert(gen0State->domCache == NULL);
}

local void
gen0IssueDCache1()
{
	DomainCacheList dcl = listNReverse(DomainCache)(gen0State->domCache);
	DomainCache     dc;
	FoamList	stmts;
	Foam 		set, rhs = NULL;
	
	gen0DCacheInIssue = true;
	stmts = gen0State->lines;
	gen0State->lines = NULL;
	gen0State->domCache = listNil(DomainCache);

	while (dcl != listNil(DomainCache)) {
		dc = car(dcl);
		switch (dc->type) {
		  case HT_TFormHash:
			rhs = gen0RtTypeHash((TForm) (dc->args[0]), 
					     (TForm) (dc->args[1]));
			break;
		  case HT_SefoHash:
			rhs = gen0RtSefoHash((Sefo) (dc->args[0]), 
					     (Sefo) (dc->args[1]));
			break;
		  case HT_SefoHashExporter:
			rhs = gen0RtSefoHashExporter((Sefo) (dc->args[0]));
			break;
		  case HT_Id:
			rhs = gen0RtDomainHash(gen0Syme((Syme)(dc->args[0])));
			break;
		  default:
			bug("Unexpected hash-kind");
			break;
		}
		set = foamNewDef(dc->hashVar, rhs);
		gen0AddStmt(set, NULL);
		dcl = cdr(dcl);
	}
	
	gen0State->lines = listNConcat(Foam)(stmts, gen0State->lines);
	listFree(DomainCache)(dcl);
	gen0DCacheInIssue = false;
}


/*****************************************************************************
 *
 * :: Foam code generation for (runtime) hash values
 *
 ****************************************************************************/

#define foamNewProgInfo(x)	foamNewPRef(0, foamNewCProg(x))

local Foam	gen0RtTypeHashAsGeneral		(TForm);
local Bool	gen0RtSefoIsSpecialOp		(AbSyn);
local Foam	gen0RtSefoHashSpecialExporter	(Sefo, Sefo);
local Foam	gen0RtSefoHashId		(Sefo, Sefo);
local Foam	gen0RtSefoHashApply		(Sefo, Sefo);
local Foam	gen0RtSefoHashStdApply		(Sefo, Sefo);
local Foam	gen0RtSefoHashSpecialApply	(Sefo);
local Foam	gen0RtSefoHashEnum		(Sefo, SefoList);
local Foam	gen0RtSefoHashList		(SefoList, SefoList, Foam);
local SefoList	gen0RtSefoMakeArgList		(Sefo);
local Foam	gen0RtTypeHashAsGeneral		(TForm);
local void	gen0RtUseDeclares		(SefoList);
local SefoList  gen0RtSefoListUnComma		(SefoList);
local Foam	gen0RtIsProgInfoNull		(Foam);

Foam 
gen0RtSetProgHash(Foam clos, AInt hash)
{
	return foamNewSet(foamNewProgInfo(clos),
			  foamNewSInt(hash));
}

static unsigned long gen0RtArgHashSeed = 74755L;
static unsigned long *gen0RtArgHashMask = (unsigned long *)NULL;

/*
 * A trivial random number generator: it doesn't have to be good
 * (it isn't), it just has to generate exactly the same sequence
 * of pseudo-random numbers on every platform the compiler runs on.
 * It returns a 15-bit unsigned number but must be able to perform
 * unsigned arithmetic on 27-bit numbers without overflow.
 */
unsigned long gen0RtRand(void)
{
	gen0RtArgHashSeed = (gen0RtArgHashSeed*1309L + 13849L) & 65535L;
	return gen0RtArgHashSeed;
}

local void
gen0RtInitRand(void)
{
	gen0RtArgHashSeed = 74755L;
}


/* Allocate and populate gen0RtArgHashMask */
local void
gen0RtInitHashMask(void)
{
	int i;
	unsigned long required;

	/* Do nothing if already initialised */
	if (gen0RtArgHashMask) return;

	/* How many bytes of store are needed? */
	required = ((int)TF_LIMIT)*sizeof(unsigned long);
	gen0RtArgHashMask = (unsigned long *)stoAlloc(int0, required);

	/* Reset the random number generator */
	gen0RtInitRand();

	/* Generate a hash mask for each tform type */
	for (i = 0; i < TF_LIMIT; i++) gen0RtArgHashMask[i] = gen0RtRand();
}

local Foam
gen0RtTypeHash(TForm tf, TForm otf)
{
        TFormList       tfl = 0, otfl = 0, l, ol;
        Foam            hash = 0;
        int             code, i;
	int		hashPoint = -1;
	int		hashMask = (int)TF_START;
	Foam		twist = (Foam)NULL;

	if (genIsRuntime())
		return foamNewSInt(int0);
        /* first fill tfl with the tforms we want to combine */
        tf  = tfDefineeType(tf);
        otf = tfDefineeType(otf);

        code = gen0StrHash(tformSyntax(tfTag(tf)));

	if (tfTag(otf) == TF_General && tfTag(tf) != TF_General)
		return gen0RtTypeHashAsGeneral(tf);

	if (tfArgc(otf) != tfArgc(tf))
		otf = tf;

	switch (tfTag(otf)) {
          case TF_Map:
          case TF_PackedMap:
                assert(tfTag(tf) == tfTag(otf));
                for(i=0; i<tfMapArgc(tf); i++) {
                        tfl  = listCons(TForm)(tfMapArgN(tf, i), tfl);
                        otfl = listCons(TForm)(tfMapArgN(otf, i), otfl);
                }
                for(i=0; i<tfMapRetc(tf); i++) {
                        tfl  = listCons(TForm)(tfMapRetN(tf, i), tfl);
                        otfl = listCons(TForm)(tfMapRetN(otf, i), otfl);
                }
		hashMask = (int)tfTag(otf);
		/* Between argument types and return types */
		hashPoint = tfMapArgc(tf);
                break;
          case TF_RawRecord:
                assert(tfTag(tf) == tfTag(otf));
                for(i=0; i<tfRawRecordArgc(tf); i++) {
                        tfl  = listCons(TForm)(tfRawRecordArgN(tf, i), tfl);
                        otfl = listCons(TForm)(tfRawRecordArgN(otf, i), otfl);
                }
                break;
          case TF_Record:
                assert(tfTag(tf) == tfTag(otf));
                for(i=0; i<tfRecordArgc(tf); i++) {
                        tfl  = listCons(TForm)(tfRecordArgN(tf, i), tfl);
                        otfl = listCons(TForm)(tfRecordArgN(otf, i), otfl);
                }
                break;
          case TF_Union:
                assert(tfTag(tf) == tfTag(otf));
                for(i=0; i<tfUnionArgc(tf); i++) {
                        tfl  = listCons(TForm)(tfUnionArgN(tf, i), tfl);
                        otfl = listCons(TForm)(tfUnionArgN(otf, i), otfl);
                }
                break;
          case TF_Cross:
                assert(tfTag(tf) == tfTag(otf));
                for(i=0; i<tfCrossArgc(tf); i++) {
                        tfl  = listCons(TForm)(tfCrossArgN(tf, i), tfl);
                        otfl = listCons(TForm)(tfCrossArgN(otf, i), otfl);
                }
                break;
          case TF_Multiple:
                assert(tfTag(tf) == tfTag(otf));
                for(i=0; i<tfMultiArgc(tf); i++) {
                        tfl  = listCons(TForm)(tfMultiArgN(tf, i), tfl);
                        otfl = listCons(TForm)(tfMultiArgN(otf, i), otfl);
                }
                break;
	  case TF_Tuple:
		assert(tfTag(tf) == tfTag(otf));
		tfl  = listCons(TForm)(tfTupleArg( tf), listNil(TForm));
		otfl = listCons(TForm)(tfTupleArg(otf), listNil(TForm));
		break;
	  case TF_Generator:
		assert(tfTag(tf) == tfTag(otf));
		tfl  = listCons(TForm)(tfGeneratorArg( tf), listNil(TForm));
		otfl = listCons(TForm)(tfGeneratorArg(otf), listNil(TForm));
		break;
	  case TF_Reference:
		assert(tfTag(tf) == tfTag(otf));
		tfl  = listCons(TForm)(tfReferenceArg( tf), listNil(TForm));
		otfl = listCons(TForm)(tfReferenceArg(otf), listNil(TForm));
		break;
          case TF_Enumerate:
                assert(tfTag(tf) == tfTag(otf));
                hash = foamNewSInt(code);
                for(i=0; i<tfEnumArgc(tf); i++) {
                        String  lit;
                        AbSyn   tfi  = abDefineeId(tfExpr(tfEnumArgN(tf, i)));
                        assert(abTag(tfi) == AB_Id);
                        lit = tfi->abId.sym->str;
                        hash = gen0CombineHash(foamNewSInt(gen0StrHash(lit)),
                                               hash);
                }
                break;
          case TF_Literal:
                break;
		       
          case TF_With:
		code = 0;
                break;
          case TF_General:
                hash = gen0RtSefoHash(tfExpr(tf), tfExpr(otf));
          default:
                break;
        }
        if (!hash)
                hash = foamNewSInt(code);
        tfl  = listNReverse(TForm)(tfl);
        otfl = listNReverse(TForm)(otfl);
	/* Ensure that we have hash masks */
	if (!gen0RtArgHashMask) gen0RtInitHashMask();

	/* Original hash combine with a twist of lime for maps */
	for(i = 0, l = tfl, ol = otfl; l; i++, l = cdr(l), ol = cdr(ol)) {
		/* Extra hash code merged in at the correct moment */
		if (i == hashPoint) {
			twist = foamNewSInt(gen0RtArgHashMask[hashMask]);
			foamPure(twist) = true;
			hash = gen0CombineHash(twist, hash);
			foamPure(hash) = true;
		}
		hash = gen0CombineHash(gen0RtTypeHash(car(l), car(ol)), hash);
		foamPure(hash) = true;
	}
	/* Add the lime if not done so already */
	if (!twist && (hashPoint >= 0)) {
		twist = foamNewSInt(gen0RtArgHashMask[hashMask]);
		foamPure(twist) = true;
		hash = gen0CombineHash(twist, hash);
		foamPure(hash) = true;
	}

        return hash;
}

/*
 * Left as a function, as the hashcode for a TF_With is odd.
 */

local Foam
gen0RtTypeHashWith(TForm tf, TForm otf, String name)
{
	if (name)
		return foamNewSInt(gen0StrHash(name));
	else
		return foamNewSInt(int0);
}

local Foam
gen0RtTypeHashAsGeneral(TForm tf)
{
	TFormList	tfl = listNil(TForm);
	Foam		hash = NULL;
	int		code, i;
	int		hashPoint = -1;
	int		hashMask = (int)TF_START;
	Foam		twist = (Foam)NULL;

        code = gen0StrHash(tformSyntax(tfTag(tf)));

	if (tfIsSym(tf) || tfIsThird(tf))
		return gen0RtSefoHash(tfExpr(tf), tfExpr(tf));

	switch(tfTag(tf)) {
	case TF_Map:
	case TF_PackedMap:
                for(i = 0; i < tfMapArgc(tf); i += 1)
                        tfl  = listCons(TForm)(tfMapArgN(tf, i), tfl);
                for(i = 0; i < tfMapRetc(tf); i += 1)
                        tfl  = listCons(TForm)(tfMapRetN(tf, i), tfl);
		hashMask = (int)tfTag(tf);
		/* Between argument types and return types */
		hashPoint = tfMapArgc(tf);
                break;
	case TF_RawRecord:
		for (i = 0; i < tfRawRecordArgc(tf); i += 1) {
			TForm	tfi = tfRawRecordArgN(tf, i);
			tfl = listCons(TForm)(tfDefineeType(tfi), tfl);
		}
		break;
	case TF_Record:
		for (i = 0; i < tfRecordArgc(tf); i += 1) {
			TForm	tfi = tfRecordArgN(tf, i);
			tfl = listCons(TForm)(tfDefineeType(tfi), tfl);
		}
		break;
	case TF_Union:
		for (i = 0; i < tfUnionArgc(tf); i += 1) {
			TForm	tfi = tfUnionArgN(tf, i);
			tfl = listCons(TForm)(tfDefineeType(tfi), tfl);
		}
		break;
	case TF_Cross:
		for (i = 0; i < tfCrossArgc(tf); i += 1)
			tfl = listCons(TForm)(tfCrossArgN(tf, i), tfl);
		break;
	case TF_Tuple:
		tfl = listCons(TForm)(tfTupleArg(tf), listNil(TForm));
		break;
	case TF_Reference:
		tfl = listCons(TForm)(tfReferenceArg(tf), listNil(TForm));
		break;
	case TF_Generator:
		tfl = listCons(TForm)(tfGeneratorArg(tf), listNil(TForm));
		break;
        case TF_Enumerate:
		hash = foamNewSInt(code);
		for (i = 0; i < tfEnumArgc(tf); i++) {
			String lit;
			AbSyn  tfi = abDefineeId(tfExpr(tfEnumArgN(tf, i)));
			lit = tfi->abId.sym->str;
			hash = gen0CombineHash(foamNewSInt(gen0StrHash(lit)),
					       hash);
		}
		break;
	default:
		tfPrintDb(tf);
		bug("unhandled special type used in value context");
	}
	
	tfl = listNReverse(TForm)(tfl);
	if (hash == NULL)
		hash = foamNewSInt(code);
	/* Ensure that we have hash masks */
	if (!gen0RtArgHashMask) gen0RtInitHashMask();
	
	/* Original hash combine plus a twist of lime */
	for(i = 0; tfl; i++, tfl = cdr(tfl)) {
		/* Extra hash code merged in at the correct moment */
		if (i == hashPoint) {
			twist = foamNewSInt(gen0RtArgHashMask[hashMask]);
			foamPure(twist) = true;
			hash = gen0CombineHash(twist, hash);
			foamPure(hash) = true;
		}
		hash = gen0CombineHash(gen0RtTypeHash(car(tfl),car(tfl)),hash);
		foamPure(hash) = true;
	}
	
	/* Add the lime if not done so already */
	if (!twist && (hashPoint >= 0)) {
		twist = foamNewSInt(gen0RtArgHashMask[hashMask]);
		foamPure(twist) = true;
		hash = gen0CombineHash(twist, hash);
		foamPure(hash) = true;
	}

	return hash;
}

local Foam
gen0RtSefoHashExporter(Sefo sf)
{
	Foam	hash, hi;

	if (genIsRuntime() || sf == NULL || abHasTag(sf, AB_With))
		return foamNewSInt(int0);

	sf = gen0EqualMods(sf);

	switch(abTag(sf)) {
	  case AB_Id:
		hash = foamNewSInt(gen0StrHash(symString(abIdSym(sf))));
		break;
	  case AB_Apply:
		if (gen0RtSefoIsSpecialOp(sf))
			hash = gen0RtSefoHashSpecialExporter(sf, NULL);
		else {
			Length	i;
			hash = gen0RtSefoHashExporter(abApplyOp(sf));

			for (i = 0; i < abApplyArgc(sf); i += 1) {
				hi = gen0RtSefoHash(abApplyArg(sf, i), NULL);
				hash = gen0CombineHash(hi, hash);
			}
		}
		break;
	  case AB_LitInteger:
	  case AB_LitFloat:
	  case AB_LitString:
	  default:
		comsgFatal((AbSyn)sf, ALDOR_F_Bug, "unexpected exporter");
		NotReached(hash = 0);
	}

	return hash;
}

/*
 * This nonsense is to get the definition of     Record(T:Tuple) == add
 * to generate the same hash code as the use of  Record(A,B,C,D)
 */
local Bool
gen0RtSefoIsSpecialOp(AbSyn ab)
{
	AbSyn	op = abApplyOp(ab);
	Symbol	sym = abIsId(op) ? abIdSym(op) : NULL;

	return	sym == ssymArrow	||
		sym == ssymPackedArrow	||
		sym == ssymCross	||
		sym == ssymRawRecord	||
		sym == ssymRecord	||
		sym == ssymUnion	||
		sym == ssymEnum;
}

/* Convert special operation into a tform tag */
local int
gen0RtSymSpecialTag(Symbol sym)
{
	/* We aren't interested in every special */
	if (sym == ssymArrow)		return (int)TF_Map;
	if (sym == ssymPackedArrow)	return (int)TF_PackedMap;
	if (sym == ssymCross)		return (int)TF_Cross;
	if (sym == ssymRawRecord)	return (int)TF_RawRecord;
	if (sym == ssymRecord)		return (int)TF_Record;
	if (sym == ssymUnion)		return (int)TF_Union;
	if (sym == ssymEnum)		return (int)TF_START;

	/* All other specials are ignored */
	return (int)TF_START;
}

/*
 * Generate this code:
 *
 *     Th := hash(operatorString);
 *  ** for each arg **
 *     Tt := arg.i;
 *     Tn := #Tt;
 *     Ti := 0;
 * TS: if (Ti = Tn) goto TE;
 *     Th := gen0CombineHash(getDomainHash! tuple.i, Th);
 *     Ti := Ti + 1;
 *     goto TS
 * TE: nop
 *  ** end for each arg **
 *  ** genFoamVal is Th **
 */       

#define GSTAT(x)	gen0AddStmt(x, NULL)
#define GSET(l,r)	foamNewSet(foamCopy(l),r)

local Foam
gen0RtSefoHashSpecialExporter(Sefo sf, Sefo osf)
{
	Foam	Tt = gen0TempLocal0(FOAM_Rec, gen0MakeTupleFormat());
	Foam 	Tn = gen0TempLocal(FOAM_SInt);
	Foam 	Ti = gen0TempLocal(FOAM_SInt);
	Foam 	Th = gen0TempLocal(FOAM_SInt);
	int	i;
	AbSyn	op;
	Bool	inEnum;

	int	hashPoint = 0;
	int	hashMask;
	Symbol	opsym;
	Foam	twist = (Foam)NULL;

	op = abApplyOp(sf);
	opsym = op->abId.sym;
	inEnum = (opsym == ssymEnum);
	hashMask = gen0RtSymSpecialTag(opsym);

	assert(abTag(sf) == AB_Apply && abTag(abApplyOp(sf)) == AB_Id);
	/* I don't think we ought to ever see this ... */
	if ((opsym == ssymArrow) || (opsym == ssymPackedArrow))
		hashPoint = 1; /* Only insert hash mask for maps */

	/* Ensure that we have hash masks */
	if (!gen0RtArgHashMask) gen0RtInitHashMask();

	GSTAT(GSET(Th,foamNewSInt(gen0StrHash(symString(op->abId.sym)))));

	for (i = 0; i < abApplyArgc(sf); i++) {
		int	TS  = gen0State->labelNo++;	
		int  	TE  = gen0State->labelNo++;
		AbSyn	arg = abApplyArg(sf, i);
		Foam 	val;

		assert(abTag(arg) == AB_Declare);
		arg = arg->abDeclare.id;

		GSTAT(GSET(Tt, foamNewCast(FOAM_Rec, genFoamVal(arg))));
		GSTAT(GSET(Tn, gen0NewTupleSizeRef(foamCopy(Tt))));
		GSTAT(GSET(Ti, foamNewSInt(int0)));
		GSTAT(foamNewLabel(TS));
		GSTAT(foamNewIf(foamNew(FOAM_BCall, 3, FOAM_BVal_SIntEQ, 
					foamCopy(Ti), foamCopy(Tn)), TE));
		/* Extra hash code merged in at the correct moment */
		if (hashPoint && (i == hashPoint)) {
			twist = foamNewSInt(gen0RtArgHashMask[hashMask]);
			GSTAT(GSET(Th, gen0CombineHash(twist, foamCopy(Th))));
		}
		val = foamNewAElt(FOAM_Word,
				  foamCopy(Ti),
				  gen0NewTupleValsRef(foamCopy(Tt)));
		if (inEnum) 
			val = gen0BuiltinCCall(FOAM_SInt, "stringHash", "runtime", 1,
					       val);
		else 
			val = gen0RtDomainHash(val);
		GSTAT(GSET(Th, gen0CombineHash(val, foamCopy(Th))));
		GSTAT(GSET(Ti, foamNew(FOAM_BCall, 3, FOAM_BVal_SIntPlus, 
				       foamCopy(Ti), foamNewSInt(1))));
		GSTAT(foamNewGoto(TS));
		GSTAT(foamNewLabel(TE));
	}

	/* Add a dash of lime if not done so already */
	if (!twist && hashPoint) {
		twist = foamNewSInt(gen0RtArgHashMask[hashMask]);
		GSTAT(GSET(Th, gen0CombineHash(twist, foamCopy(Th))));
	}

	foamFree(Tt);
	foamFree(Ti);
	foamFree(Tn);

	return Th;
}

/* extract the hash code from a domain. */

local Foam
gen0RtDomainHash(Foam dom)
{
	Foam foam;

        foam = gen0BuiltinCCall(FOAM_SInt, "domainHash!", "runtime", 1, dom);
	foamPure(foam) = true;
	return foam;
}

local Foam
gen0RtSefoHash(Sefo sf, Sefo osf)
{
	TForm	tf;
	Foam	hash;
	String	msg;

	if (genIsRuntime() || sf == NULL || abHasTag(sf, AB_With))
		return foamNewSInt(int0);
	
	sf  = gen0EqualMods(sf);
	osf = gen0EqualMods(osf);

	tf = gen0AbType(sf);

	if (tf && !tfSatDom(tf) && !tfSatCat(tf))
		return foamNewSInt(7);

	switch(abTag(sf)) {
	  case AB_Id:
		hash = gen0RtSefoHashId(sf, osf);
		break;
	  case AB_Apply:
		hash = gen0RtSefoHashApply(sf, osf);
		break;
	  case AB_Lambda:
	default:
		msg = "gen0RtSefoHash got wierd type";
		comsgFatal((AbSyn)sf, ALDOR_F_Bug, msg);
		NotReached(hash = 0);
	}

	return hash;
}

local Foam
gen0RtSefoHashId(Sefo sf, Sefo osf)
{
	Syme    syme = abSyme(sf);
	Foam    hash;
	FoamTag kind = symeFoamKind(syme);

	/* % --> my hashcode */
	if (gen0HasSelf && symeIsSelf(syme)) 
		hash = gen0RtDomainHash(foamCopy(gen0HasSelf));
	else if (gen0ExportState && symeIsSelf(syme)) {
		hash = foamCopy(gen0ExportState->selfHash);
		gen0AddLexLevels(hash,
			gen0State->foamLevel - gen0ExportState->foamLevel);
	}
	else if (symeIsSelf(syme)) {
		/*
		 * This seems to only occur during -gloop. We
		 * don't actually achieve anything by this since
		 * gen0Syme will segfault anyway ... The problem
		 * is that cascaded exports don't get added to
		 * gen0ExportState properly leaving it NULL.
		 */
		hash = gen0RtDomainHash(foamCopy(gen0Syme(syme)));
		if (DEBUG(genfHash)) {
			(void)fprintf(dbOut, "!!! Warning: inventing hash for %%: ");
			symePrintDb(syme);
		}
	}


	else if (symeIsImport(syme)) {
		if (kind == FOAM_LIMIT) gen0Syme(syme);
		hash = gen0RtDomainHash(genFoamType(sf));
	}

	else if (symeIsExport(syme) || symeIsExtend(syme)) {
		if (kind == FOAM_LIMIT && !symeLib(syme)) {
			if (DEBUG(genfHash)) {
				fprintf(dbOut, "Ugh: Found unhackable syme: ");
				symePrintDb(syme);
			}
			return foamNewSInt(gen0StrHash(symeString(syme)));
		}
		hash = gen0RtDomainHash(genFoamType(sf));
	}

	else if (kind == FOAM_LIMIT) {
		if (DEBUG(genfHash)) {
			fprintf(dbOut, "Ugh: Found weird syme: ");
			symePrintDb(syme);
		}
		hash = foamNewSInt(int0);
	}

	else if (symeIsParam(syme) &&
		 !stabHasMeaning(gen0State->stab, symeOriginal(syme)))
		hash = foamNewSInt(int0);
	/* We can cache non-local parameters and lexicals used in types,
	   providing we are in an add or with clause */
	else if (symeIsParam(syme) &&
		 gen0State->tag >= GF_START_TYPE &&
		 gen0State->tag <= GF_END_TYPE)
		hash = gen0DCacheAddItem(HT_Id, syme, NULL);
	else
		hash = gen0RtDomainHash(genFoamType(sf));

	return hash;
}

local Foam
gen0RtSefoHashApply(Sefo sf, Sefo osf)
{
	if (gen0RtSefoIsSpecialOp(sf))
		return gen0RtSefoHashSpecialApply(sf);
	else
		return gen0RtSefoHashStdApply(sf, osf);
}

local Foam
gen0RtSefoHashStdApply(Sefo sf, Sefo osf)
{
	SefoList sfl = listNil(Sefo), osfl = listNil(Sefo);
	AbSyn 	 op   = abApplyOp(sf);
	FoamList topLines;
	Foam	 opFoam, dom, hash;
	Foam	 result = gen0TempLocal(FOAM_SInt);
	int	 askLabel = gen0State->labelNo++;
	int	 outLabel = gen0State->labelNo++;
	Bool	 flag, buildable;

	sfl = gen0RtSefoMakeArgList(sf);

	if (osf && abIsApply(osf))
		osfl = gen0RtSefoMakeArgList(osf);

	flag = gen0AddImportPlace(&topLines);

	/*
	 * Check to see if all the symes have been classified (their
	 * kind is believable. Unfortunately this isn't a good test
	 * of whether a domain can be constructed. Some domains can
	 * be constructed without having all their symes allocated
	 * while others can't.
	 */
	buildable = gen0AllSymesAllocated(sf);

	opFoam = genFoamType(op);
	if (abTag(op) != AB_Id) {
		Foam	loc = gen0TempLocal(FOAM_Word);
		gen0AddStmt(foamNewSet(foamCopy(loc), opFoam), NULL);
		opFoam = loc;
	}

	/*
	 * Force the type to be unlazied: We have to do this
	 * to avoid creating at least one domain per imported
	 * constructor. See gen0MakeLazyGlo{Dom,Cat}
	 */
	gen0AddStmt(foamNewEEnsure(foamNewCEnv(foamCopy(opFoam))),
		    NULL);


#if AXL_EDIT_1_1_12p6_14  /* DO NOT ENABLE THIS (see editlevels.h) */
	/*
	 * Always ask unless we can't build: this is because a bug in
	 * this code means that we peek the hash code from the wrong
	 * object when given a curried domain. Until this bug is fixed
	 * we MUST create the domain and ask for its hash code.
	 */
	if (!buildable) {
		gen0AddStmt(foamNewIf(gen0RtIsProgInfoNull(opFoam), askLabel),
		    NULL);
		hash = foamNewProgInfo(foamCopy(opFoam));
		hash = gen0RtSefoHashList(sfl, osfl, hash);
		gen0AddStmt(foamNewSet(foamCopy(result), hash), NULL);
		gen0AddStmt(foamNewGoto(outLabel), NULL);

		/* When all else fails, try asking... */
		gen0AddStmt(foamNewLabel(askLabel), NULL);
	}
#else
	/*
	 * If we can read a pre-computed hash code then do so. If we
	 * cannot then we build the domain and ask it for the hash.
	 */
	gen0AddStmt(foamNewIf(gen0RtIsProgInfoNull(opFoam), askLabel),
		    NULL);
	hash = foamNewProgInfo(foamCopy(opFoam));
	hash = gen0RtSefoHashList(sfl, osfl, hash);
	gen0AddStmt(foamNewSet(foamCopy(result), hash), NULL);
	gen0AddStmt(foamNewGoto(outLabel), NULL);


	/* When all else fails, try asking... */
	gen0AddStmt(foamNewLabel(askLabel), NULL);
#endif

	if (buildable) {
		/*
		 * We assume that this will give us the hash code. Luckily
		 * if we don't get the hash code then we didn't actually need
		 * it! This begs the question, why did we ask for it in the
		 * first place?
		 */
		dom = gen0RtDomainHash(genFoamType(sf));
		gen0AddStmt(foamNewSet(foamCopy(result), dom), NULL);
	}
	else {
/*
 * We ought to generate this compiler warning but it happens
 * too often in situations where it may not actually cause
 * any problems.
 */
#if 0
		/*
		 * Issue a compiler warning if we are able to link
		 * this bit of sefo with some source code.
		 */
		if (abPos((AbSyn)sf))
			comsgWarning((AbSyn)sf, ALDOR_W_ChkBadDependent);
#endif

		gen0AddStmt(foamNew(FOAM_BCall, 2,
				    FOAM_BVal_Halt,
				    foamNewSInt(FOAM_Halt_BadDependentType)), NULL);
		gen0AddStmt(foamNewSet(foamCopy(result), 
				       foamNewSInt(int0)), NULL);
	}

	gen0AddStmt(foamNewLabel(outLabel), NULL);
	if (flag) gen0ResetImportPlace(topLines);
	hash = result;

	listFree(Sefo)(sfl);
	listFree(Sefo)(osfl);
	return hash;
}

local Foam
gen0RtSefoHashSpecialMap(Sefo sf)
{
	Sefo	 arg;
	Sefo	*argv;
	AbSyn	 op  = abApplyOp(sf);
	Symbol	 sym = abIdSym(op);
	Foam	 hash, twist, hi;
	int 	 argc;
	int 	 i;
	int	hashMask = gen0RtSymSpecialTag(sym);

	/* Ensure that we have hash masks */
	if (!gen0RtArgHashMask) gen0RtInitHashMask();

	/* Hash the operator ... */
	hash = foamNewSInt(gen0StrHash(symString(sym)));

	/* Process the map argument types */
	arg  = abApplyArg(sf, int0);
	argv = abArgvAs(AB_Comma, arg);
	argc = abArgcAs(AB_Comma, arg);
	for (i = 0; i < argc; i++) {
		Sefo argi = argv[i];

		if (abTag(argi) != AB_Id)
			argi = abDefineeTypeOrElse(argi, argi);
		hi = gen0RtSefoHash(argi, (Sefo)NULL);
		foamPure(hi) = true;
		hash = gen0CombineHash(hi, hash);
		foamPure(hash) = true;
	}

	/* Fold in the map hash mask */
	twist = foamNewSInt(gen0RtArgHashMask[hashMask]);
	foamPure(twist) = true;
	hash = gen0CombineHash(twist, hash);
	foamPure(hash) = true;

	/* Process the map return types */
	arg  = abApplyArg(sf, 1);
	argv = abArgvAs(AB_Comma, arg);
	argc = abArgcAs(AB_Comma, arg);
	for (i = 0; i < argc; i++) {
		Sefo argi = argv[i];

		if (abTag(argi) != AB_Id)
			argi = abDefineeTypeOrElse(argi, argi);
		hi = gen0RtSefoHash(argi, (Sefo)NULL);
		foamPure(hi) = true;
		hash = gen0CombineHash(hi, hash);
		foamPure(hash) = true;
	}

	return hash;
}

local Foam
gen0RtSefoHashSpecialApply(Sefo sf)
{
	SefoList sfl = listNil(Sefo);
	AbSyn	 op  = abApplyOp(sf);
	Symbol	 sym = abIdSym(op);
	Foam	 hash;
	int 	 argc = abApplyArgc(sf);
	int 	 i;

	if (abIsAnyMap(sf))
		return gen0RtSefoHashSpecialMap(sf);

	for (i = 0; i < argc; i += 1)
		sfl = listCons(Sefo)(abApplyArg(sf, i), sfl);
	sfl = listNReverse(Sefo)(sfl);

	if (sym == ssymEnum)
		return gen0RtSefoHashEnum(sf, sfl);

	gen0RtUseDeclares(sfl);

	hash = foamNewSInt(gen0StrHash(symString(sym)));
	hash = gen0RtSefoHashList(sfl, listNil(Sefo), hash);
	listFree(Sefo)(sfl);
	return hash;
}

local Foam
gen0RtSefoHashEnum(Sefo sf, SefoList sfl)
{
	AbSyn	op = abApplyOp(sf);
	Symbol	sym = abIdSym(op);
	Foam	hash, hi;

	hash = foamNewSInt(gen0StrHash(symString(sym)));
	for (; sfl; sfl = cdr(sfl)) {
		sym = abIdSym(abDefineeId(car(sfl)));
		hi = foamNewSInt(gen0StrHash(symString(sym)));
		hash = gen0CombineHash(hi, hash);
	}

	return hash;
}

local Foam
gen0RtSefoHashList(SefoList sfl, SefoList osfl, Foam hash)
{
	Sefo	sf, osf;

	assert(!osfl || listLength(Sefo)(sfl) == listLength(Sefo)(osfl));
	while (sfl) {
		sf = car(sfl);
		osf = osfl ? car(osfl) : NULL;

		hash = gen0CombineHash(gen0RtSefoHash(sf, osf), hash);
		foamPure(hash) = true;

		sfl = cdr(sfl);
		osfl = osfl ? cdr(osfl) : NULL;
	}
	return hash;
}

local SefoList
gen0RtSefoListUnComma(SefoList sfl)
{
	SefoList lst = listNil(Sefo);

	while (sfl) {
		Sefo	sf = car(sfl);
		Sefo	*argv = abArgvAs(AB_Comma, sf);
		Length	i, argc = abArgcAs(AB_Comma, sf);

		for (i = 0; i < argc; i += 1)
			lst = listCons(Sefo)(argv[i], lst);
		sfl = cdr(sfl);
	}
	return listNReverse(Sefo)(lst);
}

local SefoList
gen0RtSefoMakeArgList(Sefo sf)
{
	SefoList sfl = listNil(Sefo);
	TForm    opTf = gen0AbType(abApplyOp(sf));
	int      argc = tfMapArgc(opTf);
	int      i;

	for (i = 0; i < argc; i += 1) {
		AbSyn sfi = tfMapSelectArg(opTf, sf, i);
		sfl = listCons(Sefo)(sfi, sfl);
	}
	sfl = listNReverse(Sefo)(sfl);
	
	return sfl;
}

local void
gen0RtUseDeclares(SefoList sfl)
{
	while (sfl) {
		Sefo sf = car(sfl);
		if (abTag(sf) != AB_Id)
			car(sfl) = abDefineeTypeOrElse(sf, sf);
		sfl = cdr(sfl);
	}
}

local Foam
gen0RtIsProgInfoNull(Foam foam)
{
	return foamNew(FOAM_BCall, 2, FOAM_BVal_SIntIsZero, 
		       foamNewProgInfo(foam));
}


/* A 28 or 30 bit prime */

#define HashModulus (gen0SmallHashCodes ? 0x07FFFFD9 : 0x3FFFFFDD)
#define ShiftMask   0x00FFFFFF

local Foam
gen0CombineHash(Foam hash1, Foam hash0)
{
        Foam    bcall;
#if 0
/*!! use when inlining from callbacks works */
        return gen0BuiltinCCall(FOAM_SInt, "combineHash", "runtime",
                               hash1, hash0);
#endif
        bcall = foamNew(FOAM_BCall, 3, FOAM_BVal_SIntHashCombine, hash1, hash0);

        return bcall;
}

int
gen0StrHash(String s)
{
        return strHash(s) % HashModulus;
}

Foam
genWith(AbSyn absyn)
{
	return gen0MakeDefaultPackage(absyn, abStab(absyn), false, NULL);
}

/*
 * Construct calls to Join/Map/Enumeration/Cross/Record/Union.
 */
Bool
gen0IsSpecialType(AbSyn ab)
{
	AbSyn	op = abApplyOp(ab);
	Symbol	sym = abIsId(op) ? abIdSym(op) : NULL;

	return	sym == ssymJoin		||
	 	sym == ssymArrow	||
		sym == ssymPackedArrow	||
		sym == ssymCross	||
		sym == ssymRawRecord	||
		sym == ssymRecord	||
		sym == ssymUnion	||
		sym == ssymEnum;
}

Foam
gen0ApplySpecialType(AbSyn absyn)
{
	if (abIsJoin(absyn))
		return gen0Join(absyn);
	else if (abIsAnyMap(absyn))
		return gen0Map(absyn);
	else if (abIsApplyOf(absyn, ssymEnum))
		return gen0Enum(absyn);
	else 	
		return gen0ApplySpecialOthers(absyn);
}

local Foam
gen0Join(AbSyn absyn)
{
	return gen0MakeDefaultPackage(absyn, abStab(absyn), false, NULL);
}

local Foam 
gen0Map(AbSyn absyn)
{
	Foam	opFoam, argFoam, retFoam, foam;
	AbSyn	arg = abMapArg(absyn);
	AbSyn	ret = abMapRet(absyn);
	AbSyn	*argv;
	Length	argc;

	argv = abArgvAs(AB_Comma, arg);
	argc = abArgcAs(AB_Comma, arg);
	argFoam = gen0MakeTuple(argc, argv, arg);

	argv = abArgvAs(AB_Comma, ret);
	argc = abArgcAs(AB_Comma, ret);
	retFoam = gen0MakeTuple(argc, argv, ret);

	opFoam = genFoamVal(abApplyOp(absyn));

	foam = foamNew(FOAM_CCall, 4, FOAM_Word, opFoam, argFoam, retFoam);
	foamPure(foam) = true;
	return foam;
}

local Foam
gen0Enum(AbSyn absyn)
{
	Foam	vars[2], tupl, elts, elt, opFoam, foam;
	Length	i, argc = abApplyArgc(absyn);
	AbSyn	*argv = abApplyArgv(absyn);

	gen0MakeEmptyTuple(foamNewSInt(argc), vars, absyn);
	tupl = vars[0];
	elts = vars[1];

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = abDefineeId(argv[i]);
		elt = gen0CharArray(arg->abId.sym->str);
		gen0AddStmt(gen0ASet(elts, (AInt) i, FOAM_Word, elt), absyn);
	}

	opFoam = genFoamVal(abApplyOp(absyn));
	foam = foamNew(FOAM_CCall, 3, FOAM_Word, opFoam, tupl);
	foamPure(foam) = true;
	return foam;
}

/*
 * This function is only called when Record() etc are used as values,
 * eg on the RHS of a definition. When used as a type constructor the
 * functor never actually gets applied.
 */
local Foam
gen0ApplySpecialOthers(AbSyn absyn)
{
	Foam	vars[2], tupl, elts, elt, foam;
	Length	i, argc = abApplyArgc(absyn);
	AbSyn	*argv = abApplyArgv(absyn);
	AbSyn	op = abApplyOp(absyn);

	assert(abIdSym(op) == ssymCross ||
	       abIdSym(op) == ssymRawRecord ||
	       abIdSym(op) == ssymRecord ||
	       abIdSym(op) == ssymUnion);

	gen0MakeEmptyTuple(foamNewSInt(argc), vars, absyn);
	tupl = vars[0];
	elts = vars[1];

	for (i = 0; i < argc; i += 1) {
		AbSyn	arg = abDefineeTypeOrElse(argv[i], argv[i]);

		if (gen0AllSymesAllocated(arg))
			elt = genFoamType(arg);
		else
			elt = foamNewNil();

		gen0AddStmt(gen0ASet(elts, (AInt) i, FOAM_Word, elt), absyn);
	}

	foam = foamNew(FOAM_CCall, 3, FOAM_Word, genFoamVal(op), tupl);
	foamPure(foam) = true;
	return foam;
}

/* Return true if we can generate complete foam for 'ab'. 
 * This should check for formal arguments, symes from 
 * dependant types, and so on.
 * As lazy symbol meanings are handled later (see gen0SymeGeneric),
 * we test for and and ignore them here.
 */
local Bool
gen0AllSymesAllocated(AbSyn ab)
{
	Bool	result = true;

	if (abTag(ab) == AB_Declare)
		result = gen0AllSymesAllocated(ab->abDeclare.type);
	else if (abIsLeaf(ab)) {
		Syme	syme = abSyme(ab);

		/* Unallocated symes normally cause false return value. */
		result = !(syme && gen0FoamKind(syme) == FOAM_LIMIT);

		if (DEBUG(gfadd)) {
			fprintf(dbOut, "syme [%c]: ", result ? ' ' :
				symeLib(syme) &&
				(symeIsExport(syme) || symeIsExtend(syme)) ?
				'-' : '?');
			symePrintDb(syme);
		}

		if (syme && symeLib(syme) &&
		    (symeIsExport(syme) || symeIsExtend(syme)))
		    result = true;
	}
	else {
		Length	i;
		for (i = 0; result && i < abArgc(ab); i += 1)
			result = gen0AllSymesAllocated(abArgv(ab)[i]);
	}
	return result;
}

/*
 * Collect the defaults from the body of a with.
 */
local AbSyn
gen0FindDefaults(AbSyn with)
{
	Length	i;
	Length	defc = 0;
	AbSyn	defs = NULL, *argv;

	if (abTag(with) == AB_Default)
		return with;
	if (abTag(with) != AB_Sequence)
		return abNewNothing(sposNone);

	argv = with->abSequence.argv;
	for (i = 0; i < abArgc(with); i += 1)
		if (abTag(argv[i]) == AB_Default) {
			defs = argv[i];
			defc += 1;
		}

	if (defc == 0)
		return abNewNothing(sposNone);
	if (defc == 1)
		return defs;

        defs = abNewEmpty(AB_Sequence, defc);
        for (i = 0, defc = 0; i < abArgc(with); i += 1)
                if (abTag(argv[i]) == AB_Default)
			abArgv(defs)[defc++] = abArgv(argv[i])[0];

        return defs;
}

/*
 * Return the code for the getter function from a domain.
 */

Foam
gen0GetDomain(TForm exporter, int levOffset)
{
        int             i;
        String          name;
        Foam            getter, var, decl, ini;

        /* if already imported return variable where stored. */
        var = gen0SeenImportedDomain(exporter, int0);
        if (var)
                return var;

        name   = strPrintf("dom");
        getter = gen0GetDomainDomain(exporter);

        decl = foamNewDecl(FOAM_Word, name, emptyFormatSlot);
        i = gen0AddLexNth(decl, int0, levOffset);

	var = gen0NewLex(levOffset, i);

	/* if offsets are the same, then we should not be importing twice */
        gen0AddImportedDomain(exporter, foamCopy(var), int0);
        ini = foamNewDef(foamCopy(var), getter);
        gen0SetInitUsage(ini, int0);
        gen0AddStmt(ini, NULL);
        return foamCopy(var);
}

local void
gen0AddImportedDomain(TForm exporter, Foam var, AInt level)
{
        GenFoamState    s = gen0NthState(level);
        s->domImportList = listCons(TForm)(exporter, s->domImportList);
        s->domList = listCons(Foam)(var, s->domList);
}

local Foam
gen0SeenImportedDomain(TForm dom, AInt level)
{
        GenFoamState    s = gen0NthState(level);
        TFormList       dl = s->domImportList;
        FoamList        vl = s->domList;

        for(; dl; dl = cdr(dl), vl = cdr(vl))
                if (tfEqual(dom, car(dl)))
                        return foamCopy(car(vl));
        return 0;
}

local Foam
gen0GetDomainDomain(TForm exporter)
{
        return genFoamType(tfExpr(exporter));
}

local void
gen0SetInitUsage(Foam getter, AInt level)
{
        AIntList         lu;
        AIntList         ls;

        lu = gen0NthState(level)->formatUsage;
        ls = gen0NthState(level)->formatStack;
        gen0SetUsage(getter, lu, ls);
}

void
gen0SetUsage(Foam foam, AIntList lu, AIntList ls)
{
        foamIter(foam, arg, gen0SetUsage(*arg, lu, ls));
        if (foamTag(foam) == FOAM_Lex) {
                AInt level = foam->foamLex.level;
                while(level > 0) {
                        assert(ls != 0);
                        assert(lu != 0);
                        ls = cdr(ls);
                        lu = cdr(lu);
                        level -= 1;
                }
                assert(ls != 0);
                assert(lu != 0);
                car(lu) = car(ls);
        }
        else if (foamTag(foam) == FOAM_Env) {
                AInt level = foam->foamEnv.level;
                while(level > 0) {
                        assert(lu != 0);
                        lu = cdr(lu);
                        level -= 1;
                }
                assert(lu != 0);
                if (car(lu) == emptyFormatSlot)
                        car(lu) = envUsedSlot;
        }
}

local AbSyn
gen0CollectWithImports(AbSyn with)
{
	AbSynList lst;
	AbSyn base = with->abWith.base;
	int i;

	lst = gen0CollectAux(with->abWith.within);

	if (!lst)
		return with->abWith.base;

        if (abIsJoin(base)) {
		for (i=abApplyArgc(base)-1; i >= 0; i--)
			lst = listCons(AbSyn)(abApplyArg(base,i), lst);
	}
	else if (!abIsNothing(base))
		lst = listCons(AbSyn)(base, lst);

	return abNewApplyL(abPos(with), abNewId(sposNone, ssymJoin), lst);
}

local AbSynList
gen0CollectAux(AbSyn absyn)
{
	AbSynList lst = listNil(AbSyn);
	int i;

	switch (abTag(absyn)) {
	  case AB_Sequence:
		for (i=0; i<abArgc(absyn); i++)
			lst = listNConcat(AbSyn)
					 (gen0CollectAux(abArgv(absyn)[i]),
					  lst);
		return lst;
	  case AB_Default:
	  case AB_Declare:
		return listNil(AbSyn);
	  case AB_Export:
	  case AB_Nothing:
	  case AB_Comma:
		return listNil(AbSyn);
	  default:
		return listCons(AbSyn)(absyn, lst);
	}
}

/* Has questions */

Foam
genHas(AbSyn absyn)
{
	AbSyn dom, cat, lhs, *argv;
	SymeList symes = listNil(Syme);
	Foam foam, self;
	int argc;

	assert(abTag(absyn) == AB_Has);

	dom = absyn->abHas.expr;
	cat = absyn->abHas.property;

	/* should replace with tfSubstHasSelf */
	self  = gen0Temp(FOAM_Word);
	gen0HasSelf = self;

	gen0AddStmt(foamNewSet(self, genFoamVal(dom)), absyn);

	if (abTag(cat) == AB_With) {
		symes = stabGetExportedSymes(abStab(cat));
		lhs = gen0CollectWithImports(cat);
	}
	else 
		lhs = cat;

	if (abIsJoin(lhs)) {
		argc = abApplyArgc(lhs);
		argv = abApplyArgv(lhs);
	}
	else if (abIsNothing(lhs)) {
		argc = 0;
		argv = NULL;
	}
	else {
		argc = 1;
		argv = &lhs;
	}
	if (argc == 0)
		foam = foamNewBool(true);
	else if (argc == 1) 
		foam = gen0HasCat(self, argv[0]);
	else 
		foam = gen0HasJoin(self, argc, argv);
	
	if (symes) {
		foam = gen0HasImports(self, symes, foam);
	}

	gen0HasSelf = NULL;
	return(foam);
}

local Foam 
gen0HasJoin(Foam dom, int argc, AbSyn *argv)
{
	Foam resultVar = gen0Temp(FOAM_Bool);
	int falseLabel = gen0State->labelNo++;
	int endLabel = gen0State->labelNo++;
	int i;

	for (i = 0; i < argc ; i++) {
		int nextLabel = gen0State->labelNo++;
		gen0AddStmt(foamNewIf(gen0HasCat(dom, argv[i]), nextLabel), NULL);
		gen0AddStmt(foamNewGoto(falseLabel), NULL);
		gen0AddStmt(foamNewLabel(nextLabel), NULL);
	}
	gen0AddStmt(foamNewSet(foamCopy(resultVar), foamNewBool(true)), NULL);
	gen0AddStmt(foamNewGoto(endLabel), NULL);
	gen0AddStmt(foamNewLabel(falseLabel), NULL);
	gen0AddStmt(foamNewSet(foamCopy(resultVar), foamNewBool(false)), NULL);
	gen0AddStmt(foamNewLabel(endLabel), NULL);
	return resultVar;
}

local Foam 
gen0HasCat(Foam dom, AbSyn cat)
{
	Foam foam;
	if (DEBUG(genf)) {
		fprintf(dbOut, "Hash:\n");
		sefoPrintDb(cat);
		tfPrintDb(abTForm(cat));
	}
	foam = gen0BuiltinCCall(FOAM_Bool,"domainTestExport!",
				"runtime", 3,
				foamCopy(dom),
				 foamNewSInt(gen0StrHash("%%")), 
				 gen0SefoHash(cat, cat));
	return foamNewCast(FOAM_Word, foam);
}

local Foam
gen0HasImports(Foam dom, SymeList symes, Foam startTest)
{
	Foam resultVar = gen0Temp(FOAM_Bool);
	int falseLabel = gen0State->labelNo++;
	int startLabel = gen0State->labelNo++;
	int endLabel = gen0State->labelNo++;

	gen0AddStmt(foamNewIf(startTest, startLabel), NULL);
	gen0AddStmt(foamNewGoto(falseLabel), NULL);
	gen0AddStmt(foamNewLabel(startLabel), NULL);

	for ( ; symes ; symes = cdr(symes)) {
		int	nextLabel;
		assert(symeIsExport(car(symes)));

		nextLabel = gen0State->labelNo++;
		gen0AddStmt(foamNewIf(gen0HasImport(dom, car(symes)),
				      nextLabel), NULL);
		gen0AddStmt(foamNewGoto(falseLabel), NULL);
		gen0AddStmt(foamNewLabel(nextLabel), NULL);
	}
	gen0AddStmt(foamNewSet(foamCopy(resultVar), foamNewBool(true)), NULL);
	gen0AddStmt(foamNewGoto(endLabel), NULL);
	gen0AddStmt(foamNewLabel(falseLabel), NULL);
	gen0AddStmt(foamNewSet(foamCopy(resultVar), foamNewBool(false)), NULL);
	gen0AddStmt(foamNewLabel(endLabel), NULL);
	return resultVar;
}

local Foam 
gen0HasImport(Foam dom, Syme syme) 
{
	Foam 	foam;
	TForm 	tf;
	String str = symeString(syme);

	tf = symeType(syme);

	foam = gen0BuiltinCCall(FOAM_Bool, "domainTestExport!",
				"runtime", 3,
				foamCopy(dom),
				foamNewSInt(gen0StrHash(str)),
				gen0TypeHash(tf, tf, str));

	return foamNewCast(FOAM_Word, foam);
}	

/*****************************************************************************
 *
 * :: gen0NameType: Given an exporter, return a name-constuctor
 *
 ****************************************************************************/

local Foam 	gen0BuildExporterNameFn	(AbSyn);

local Foam	gen0NameType	(AbSyn, Bool, Bool);
local Foam	gen0NameApply   (AbSyn, Bool);
local Foam	gen0NameId	(AbSyn, Bool, Bool);
local Foam	gen0NameTuple	(Syme,  TForm, Bool);

local Foam	gen0NameConst		(String);
local Bool	gen0NameIsSingleParam	(AbSyn);
local Foam 	gen0NameSingleParam	(String, Foam);
local Foam	gen0NameCombineParts	(int argc, Foam *argv);
local Foam	gen0NamePartFrString	(String s);


local Foam
gen0BuildExporterName(AbSyn exporter)
{
	if (!exporter)
		return gen0NameConst("Anonymous");

	else if (abTag(exporter) == AB_Id)
		return gen0NameConst(symString(exporter->abId.sym));

	else if (gen0NameIsSingleParam(exporter)) {
		AbSyn	op  = abApplyOp(exporter);
		AbSyn	arg = abApplyArg(exporter, int0);
		return gen0NameSingleParam(symString(op->abId.sym),
					   genFoamVal(arg));
	}
	else
		return gen0BuildExporterNameFn(exporter);
}

local Bool
gen0NameIsSingleParam(AbSyn exporter)
{
	TForm	tf;

	if (!(abIsApply(exporter) && abApplyArgc(exporter) == 1))
		return false;
	if (gen0RtSefoIsSpecialOp(exporter))
		return false;

	if (!abIsId(abApplyOp(exporter)))
		return false;

	tf = gen0AbType(gen0EqualMods(abApplyArg(exporter, int0)));
	return tfSatCat(tf) || tfSatDom(tf);
}

local Foam 
gen0BuildExporterNameFn(AbSyn exporter)
{
	GenFoamState	saved;
	Foam		foam, clos;

	clos = foamNewClos(foamNewEnv(int0), foamNewConst(gen0NumProgs));
	foam = gen0ProgInitEmpty(strCopy("addNameFn"), NULL);

	saved = gen0ProgSaveState(PT_Gener);

	gen0AddStmt(foamNewReturn(gen0NameType(exporter, true, false)), NULL);

	gen0ProgPushFormat(emptyFormatSlot);
	gen0ProgFiniEmpty(foam, FOAM_Word, int0);

	gen0AddLexLevels(foam, 1);

        foamOptInfo(foam) = inlInfoNew(NULL, foam, NULL, false);

	gen0ProgRestoreState(saved);

	return clos;
}


local Foam
gen0NameType(AbSyn ab, Bool atExporter, Bool isEnum)
{
	Foam	foam;
	String	msg;

	ab = gen0EqualMods(ab);

	switch(abTag(ab)) {
	  case AB_Apply:
		foam = gen0NameApply(ab, atExporter);
		break;
	  case AB_Id:
		foam = gen0NameId(ab, atExporter, isEnum);
		break;
	  default:
		msg = "gen0NameType found unhandled exporter type";
		comsgFatal(ab, ALDOR_F_Bug, msg);
		NotReached(foam = 0);
#if 0
		printf("unhandled exporter type");
		abWrSExpr(dbOut, ab,int0);
		bug("unhandled exporter type");
		foam = gen0NamePartFrString(abPretty(ab));
#endif
	}
	return foam;
}

local Foam
gen0NameApply(AbSyn app, Bool atExporter)
{
	AbSyn op = app->abApply.op;
	Foam foam;
	Foam *fmArgv;
	Bool isEnum;
	int argc = abApplyArgc(app);
	int i;

	isEnum = abTag(op) == AB_Id && op->abId.sym == ssymEnum;

	fmArgv = (Foam *) stoAlloc(OB_Other, (1 + argc)*sizeof(Foam));
	
	fmArgv[0] = gen0NameType(op, atExporter, false);

	for (i=0; i<argc; i++) {
		fmArgv[i+1] = gen0NameType(abApplyArg(app, i), false, isEnum);
	}

	foam = gen0NameCombineParts(1 + argc, fmArgv);
	stoFree(fmArgv);
	return foam;
}

local Foam
gen0NameId(AbSyn id, Bool atExporter, Bool isEnum)
{	
	Syme   syme;
	TForm  tf;

	Foam foam;
	if (atExporter)
		return gen0NamePartFrString(symString(id->abId.sym));

	syme = abSyme(id);
	tf = symeType(syme);
	if (tfSatDom(tf)) 
		foam = gen0BuiltinCCall(FOAM_Word, "domainName", "runtime",
					1, genFoamVal(id));
	else if (tfIsTuple(tf)) 
		foam = gen0NameTuple(syme, tfTupleArg(tf), isEnum);
	else if (tfSatCat(tf)) 
		foam = gen0BuiltinCCall(FOAM_Word, "categoryName", "runtime", 
					1, genFoamVal(id));
	else
		foam = gen0BuiltinCCall(FOAM_Word, "namePartFrOther",
					"runtime", 1, genFoamVal(id));
	return foam;
}

local Foam
gen0NameTuple(Syme id, TForm tf, Bool isEnum)
{
	Foam tuple, n, newarr, foam, refexpr;
	Foam t2[2];
	int l1 = gen0State->labelNo++;
	int l2 = gen0State->labelNo++;

	tuple = foamNewCast(FOAM_Rec, gen0Syme(id));
	n     = gen0TempLocal(FOAM_SInt);

	/*
	 * Generate:
	 *   n := size(tuple);
	 *   t2 := Tuple(n);
	 * l1:
	 *   n := n - 1;
	 *   if negative? n goto l2;
	 *   t2.n := domainName tuple.n;
	 *   goto l1;
	 * l2:
	 *   buildName(t2);
	 */
	gen0AddStmt(foamNewSet(foamCopy(n), 
			       gen0NewTupleSizeRef(tuple)), NULL);
	gen0MakeEmptyTuple(foamCopy(n), t2, NULL);
	newarr = t2[1];
	gen0AddStmt(foamNewLabel(l1), NULL);
	gen0AddStmt(foamNewSet(foamCopy(n), 
			       foamNew(FOAM_BCall, 2, 
				       FOAM_BVal_SIntPrev,
				       foamCopy(n))), 
		    NULL);
	gen0AddStmt(foamNewIf(foamNew(FOAM_BCall, 2, FOAM_BVal_SIntIsNeg, 
				      foamCopy(n)), l2), NULL);

	refexpr = foamNewAElt(FOAM_Word, foamCopy(n),
			      gen0NewTupleValsRef(tuple));

	if (isEnum) 
		refexpr = gen0BuiltinCCall(FOAM_Word, "namePartFrString",
					   "runtime",
					   1, refexpr);
	else if (tfSatDom(tf) || tfSatCat(tf))
		refexpr = gen0BuiltinCCall(FOAM_Word, "domainName", "runtime",
					1, refexpr);
	else
		refexpr = gen0BuiltinCCall(FOAM_Word, "namePartFrOther",
					   "runtime", 1, refexpr);
	
	gen0AddStmt(foamNewSet(foamNewAElt(FOAM_Word, foamCopy(n),
					   foamCopy(newarr)), refexpr),
		    NULL);
	gen0AddStmt(foamNewGoto(l1), NULL);
	gen0AddStmt(foamNewLabel(l2), NULL);

	foam = gen0BuiltinCCall(FOAM_Word, "namePartConcat", "runtime", 2,
				foamNewBool(1), foamCopy(t2[0]));
	foamFree(n);

	return foam;
}

local Foam
gen0NameConst(String s)
{
	return gen0BuiltinCCall(FOAM_Word, "rtConstNameFn", "runtime", 1,
				gen0CharArray(s));
}

local Foam
gen0NameSingleParam(String s, Foam foam)
{
	return gen0BuiltinCCall(FOAM_Word, 
				"rtSingleParamNameFn", "runtime", 2,
				gen0CharArray(s),
				foam);
}

local Foam
gen0NameCombineParts(int argc, Foam *argv)
{
	return gen0BuiltinCCall(FOAM_Word, "namePartConcat", "runtime", 2,
				foamNewBool(false),
				gen0MakeTupleFromFoam(argc, argv));
}

local Foam
gen0NamePartFrString(String s)
{
	return gen0BuiltinCCall(FOAM_Word, "namePartFrString",
				"runtime", 1, gen0CharArray(s));
}

/*****************************************************************************
 *
 * :: Hash<->String value management
 *
 ****************************************************************************/

/* hash values are placed in a table, and it is assumed that collisions never occur 
 * [get assumes this too]
 */
static Table gen0StringTable;

void
gen0StringsInit()
{
	gen0StringTable = tblNew((TblHashFun) NULL, (TblEqFun) NULL);
}

void
gen0StringsFini()
{
	FoamList body;
	Foam 	 str1, str2;
	Foam	 int1, int2;
	TableIterator iter;
	int size = tblSize(gen0StringTable);
	int	i=0;

	if (size == 0) {
		tblFree(gen0StringTable);
		return;
	}
	body = gen0State->lines;
	gen0State->lines = listNil(Foam);

	int1 = gen0TempLocal0(FOAM_Arr, FOAM_Word);
	str1 = gen0TempLocal0(FOAM_Arr, FOAM_Word);

	gen0AddStmt(gen0ANew(int1, FOAM_Word, size), NULL);
	gen0AddStmt(gen0ANew(str1, FOAM_Word, size), NULL);

	for (tblITER(iter, gen0StringTable); tblMORE(iter); tblSTEP(iter)) {
		gen0AddStmt(gen0ASet(int1, i, FOAM_Word,
				     foamNewCast(FOAM_Word,
						 foamNewSInt((AInt) tblKEY(iter)))), NULL);
		gen0AddStmt(gen0ASet(str1, i, FOAM_Word, 
				     foamNewCast(FOAM_Word,
						 gen0CharArray((String) tblELT(iter)))), 
			    NULL);
		i++;
	}
	int2 = gen0MakeArray(foamNewSInt(size), int1, NULL);
	str2 = gen0MakeArray(foamNewSInt(size), str1, NULL);

	gen0AddStmt(gen0BuiltinCCall(FOAM_NOp, "rtAddStrings", "runtime",
				     2, 
				     foamNewCast(FOAM_Word, int2), 
				     foamNewCast(FOAM_Word, str2)), NULL);
	
	gen0State->lines = listNConcat(Foam)(body, gen0State->lines);

	tblFree(gen0StringTable);
}

local void
gen0StrRegister(int hash, String s)
{
	tblSetElt(gen0StringTable, (TblKey) (long) hash, (TblElt) s);
}

