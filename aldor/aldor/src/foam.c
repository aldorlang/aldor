/*****************************************************************************
 *
 * foam.c: First Order Abstract Machine -- FOAM code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This is the layout of a piece of FOAM
 *
 * (Unit
 *	(DFmt ...)
 *	(DDef
 *		(Def (Glo 0) ...)
 *		(Def (Glo 1) ...)
 *		...
 *		(Def (Glo ng-1) ...)
 *		(Def (Const 0) ...)
 *		...
 *		(Def (Const nc-1) ...)
 *		(Def (Lex 0 0) ...)
 *		(Def (Lex 0 1) ...)
 *		...
 *		(Def (Lex 0 nl-1) ...)
 *	))
 */

# include "axlobs.h"

#define	 FOAM_NARY	(-1)	/* Identifies tags with N-ary data argument. */

/* Used for foam sharing audit */
#define		FOAM_MARKED			0x01
#define		FOAM_UNMARKED			0x00

Bool	foamDebug		= false;
Bool	foamConstDebug		= false;
Bool	foamSposDebug		= false;

#define foamDEBUG(s)		DEBUG_IF(foamDebug, s)
#define foamConstDEBUG(s)	DEBUG_IF(foamConstDebug, s)
#define foamSposDEBUG(s)	DEBUG_IF(foamSposDebug,  s)

/*****************************************************************************
 *
 * :: Basic operations
 *
 ****************************************************************************/

SrcPos		foamDefaultPosition;
static Bool	foamIsInit = false;

/*
 * Note: This implementation shares the foamTagVal field of symCoInfo so foam
 * instructions, builtins and protocols must not have overlapping names.
 */
void
foamInit(void)
{
	int	i;
	Symbol	sym;

	foamDefaultPosition = sposNone;

	for (i = FOAM_START; i < FOAM_LIMIT; i++) {
		sym = symInternConst(foamInfo(i).str);
#if EDIT_1_0_n2_07
		if (symCoInfo(sym) == NULL) symCoInfoInit(sym);
#else
		if (!symCoInfo(sym)) symCoInfoInit(sym);
#endif

		foamInfo(i).sxsym	   = sxiFrSymbol(sym);
		symCoInfo(sym)->foamTagVal = i;
	}
	for (i = FOAM_BVAL_START; i < FOAM_BVAL_LIMIT; i++) {
		sym = symInternConst(foamBValInfo(i).str);
#if EDIT_1_0_n2_07
		if (symCoInfo(sym) == NULL) symCoInfoInit(sym);
#else
		if (!symCoInfo(sym)) symCoInfoInit(sym);
#endif

		foamBValInfo(i).sxsym	   = sxiFrSymbol(sym);
		symCoInfo(sym)->foamTagVal = i;
	}
	for (i = FOAM_PROTO_START; i < FOAM_PROTO_LIMIT; i++) {
		sym = symInternConst(foamProtoInfo(i).str);
#if EDIT_1_0_n2_07
		if (symCoInfo(sym) == NULL) symCoInfoInit(sym);
#else
		if (!symCoInfo(sym)) symCoInfoInit(sym);
#endif

		foamProtoInfo(i).sxsym	    = sxiFrSymbol(sym);
		symCoInfo(sym)->foamTagVal = i;
	}
	for (i = 0; i < FOAM_DDECL_LIMIT; i++) {
		sym = symInternConst(foamDDeclInfo(i).str);
#if EDIT_1_0_n2_07
		if (symCoInfo(sym) == NULL) symCoInfoInit(sym);
#else
		if (!symCoInfo(sym)) symCoInfoInit(sym);
#endif
		
		foamDDeclInfo(i).sxsym	= sxiFrSymbol(sym);
		symCoInfo(sym)->foamTagVal = i;
	}
	foamIsInit = true;
}

Foam
foamNewAlloc(FoamTag tag, Length argsize)
{
	Foam		foam;
	Length		sz;

	if (!foamIsInit) foamInit();

	sz  = sizeof(struct foamGen) + argsize - NARY * sizeof(Foam);
	foam = (Foam) stoAlloc((unsigned) OB_Foam, sz);

	foam->foamGen.hdr.tag		= tag;
	foam->foamGen.hdr.argc		= 0;
	foam->foamGen.hdr.mark		= FOAM_UNMARKED;
	foam->foamGen.hdr.dvMark	= 0;
	foam->foamGen.hdr.pos		= sposNone;
	foam->foamGen.hdr.syme		= NULL;
	foam->foamGen.hdr.defnId	= -1;
	foam->foamGen.hdr.info.opt	= NULL;

	return foam;
}

Foam
foamNewDFlo(DFloat d)
{
	Foam	foam = foamNewAlloc(FOAM_DFlo, sizeof(DFloat));

	foam->foamDFlo.hdr.argc = 1;
	foam->foamDFlo.DFloData = d;

	return foam;
}

Foam
foamNewSFlo(SFloat s)
{
	Foam	foam = foamNewAlloc(FOAM_SFlo, sizeof(SFloat));

	foam->foamSFlo.hdr.argc = 1;
	foam->foamSFlo.SFloData = s;

	return foam;
}

Foam
foamNewEmpty(FoamTag tag, Length argc)
{
	Foam	foam;
	Length	i;

	foam = foamNewAlloc(tag, argc * sizeof(Foam));

	foam->foamGen.hdr.argc = argc;
	for (i = 0; i < argc; i += 1)
		foam->foamGen.argv[i].code = 0;

	return foam;
}

Foam
foamNew(FoamTag tag, Length argc, ...)
{
	Foam	foam;
	va_list argp;
	Length	i;

	foam = foamNewEmpty(tag, argc);

	va_start(argp, argc);
	for (i = 0; i < argc; i++)
		foam->foamGen.argv[i].code = va_arg(argp, Foam);
	va_end(argp);

	return foam;
}


Foam
foamNewOfList(FoamTag tag, FoamList lfoam)
{
	Foam	 s;
	FoamList l;
	int	 i;

	s = foamNewEmpty(tag, listLength(Foam)(lfoam));
	for (i = 0, l = lfoam; l; i++, l = cdr(l))
		foamArgv(s)[i].code = car(l);
	return s;
}

Foam
foamNewOfList1(FoamTag tag, AInt sub, FoamList lfoam)
{
	Foam	 s;
	FoamList l;
	int	 i;

	s = foamNewEmpty(tag, 1 + listLength(Foam)(lfoam));
	i = 0;
	foamArgv(s)[i++].data = sub;

	for (l = lfoam; l; i++, l = cdr(l))
		foamArgv(s)[i].code = car(l);
	return s;
}

Foam
foamCopyNode(Foam foam)
{
	Foam	newFoam;
	Length	i, argc = foamArgc(foam);

	if (foamTag(foam) == FOAM_DFlo)
		newFoam = foamNewDFlo(foam->foamDFlo.DFloData);
	else {
		newFoam = foamNewEmpty(foamTag(foam), argc);
		for (i = 0; i < argc; i++)
			foamArgv(newFoam)[i] = foamArgv(foam)[i];
	}

	foamPos(newFoam)  = foamPos(foam);
	foamSyme(newFoam) = foamSyme(foam);

	/* if (!otIsVar(foam)) */
	if (foamTag(foam) != FOAM_Loc &&
	    foamTag(foam) != FOAM_Par &&
	    foamTag(foam) != FOAM_Lex &&
	    foamTag(foam) != FOAM_Glo)
		foamOptInfo(newFoam) = foamOptInfo(foam);

	return newFoam;
}

Foam
foamCopy(Foam foam)
{
	Foam		newFoam;
	String		argf	 = foamInfo(foamTag(foam)).argf;
	Length		i;

	newFoam = foamCopyNode(foam);

	for (i = 0; i < foamArgc(foam); i++, argf++) {
		if (*argf == '*') argf--;
		switch (*argf) {
		  case 'C': {
			  Foam	 *arg = (Foam *) foamArgv(foam)+i;
			  foamArgv(newFoam)[i].code = foamCopy(*arg);
			  break;
		  }
		  case 's':
			foamArgv(newFoam)[i].str = strCopy(foamArgv(foam)[i].str);
			break;
		  case 'n':
			foamArgv(newFoam)[i].bint =
				bintCopy(foamArgv(foam)[i].bint);
			break;
		}
	}
	return newFoam;
}

void
foamFree(Foam foam)
{
	int	si, fi;
	String	argf;

	if (!foamIsInit) foamInit();
	if (!foam) return;

	argf = foamInfo(foamTag(foam)).argf;

	for (si = fi = 0; si < foamArgc(foam); si++, fi++) {
		if (argf[fi] == '*') fi--;
		switch (argf[fi]) {
		  case 'C': foamFree(foamArgv(foam)[si].code); break;
		  case 's': strFree(foamArgv(foam)[si].str);   break;
		  case 'n': bintFree(foamArgv(foam)[si].bint); break;
		}
	}
	/*!!
	if (foamOptInfo(foam)) stoFree(foamOptInfo(foam));
	*/
	stoFree((Pointer) foam);
}

Length
foamNodeCount(Foam foam)
{
	Length	n, si, fi;
	String	argf;

	n = 1;
	argf = foamInfo(foamTag(foam)).argf;
	for (si = fi = 0; si < foamArgc(foam); si++, fi++) {
		if (argf[fi] == '*') fi--;
		if (argf[fi] == 'C')
			n += foamNodeCount(foamArgv(foam)[si].code);
	}
	return n;
}

int 
foamNaryStart(FoamTag tag)
{
	String argf;
	int n=0;

	argf = foamInfo(tag).argf;
	while (argf[n]!='*')
		n++;

	return n-1;
}

Bool
foamEqual(Foam f1, Foam f2)
{
	int	fi, si;
	String	argf;

	if (foamTag(f1)	 != foamTag(f2))  return false;
	if (foamArgc(f1) != foamArgc(f2)) return false;

	argf = foamInfo(foamTag(f1)).argf;

	for (si = fi = 0; si < foamArgc(f1); si++, fi++) {
		if (argf[fi] == '*') fi--;
		switch (argf[fi]) {
		  case 'C':
			if (!foamEqual(foamArgv(f1)[si].code,
				       foamArgv(f2)[si].code))
				return false;
			break;
		  case 't':
		  case 'o':
		  case 'p':
		  case 'D':
		  case 'b':
		  case 'h':
		  case 'w':
		  case 'i':
		  case 'L':
		  case 'X':
		  case 'F':
		  case 'f':
			if (foamArgv(f1)[si].data != foamArgv(f2)[si].data)
				return false;
			break;
		  case 's':
			if (!strEqual(foamArgv(f1)[si].str,
				      foamArgv(f2)[si].str))
				return false;
			break;
		  case 'n':
			if (!bintEQ(foamArgv(f1)[si].bint,
				    foamArgv(f2)[si].bint))
				return false;
			break;
		  case 'd':
			if (*((DFloat *) foamArgv(f1)) !=
			    *((DFloat *) foamArgv(f2)))
				return false;
			break;
		  default:
			bugBadCase(argf[si]);
			break;
		}
	}
	return true;
}

Hash
foamHash(Foam foam)
{
	Hash	h;
	Length	si, fi;
	String	argf;

	h = 0;

	argf = foamInfo(foamTag(foam)).argf;

	for (si = fi = 0; si < foamArgc(foam); si++, fi++) {
		if (argf[fi] == '*') fi--;
		h ^= (h << 8);
		switch (argf[fi]) {
		  case 'C':
			h += foamHash(foamArgv(foam)[si].code);
			break;
		  case 't':
		  case 'o':
		  case 'p':
		  case 'D':
		  case 'b':
		  case 'h':
		  case 'w':
		  case 'i':
		  case 'L':
		  case 'X':
		  case 'F':
		  case 'f':
			h += foamArgv(foam)[si].data;
			break;
		  case 's':
			h += strHash(foamArgv(foam)[si].str);
			break;
		  case 'd':
		  case 'n':
			/*!! Hash for biginit and double */
			break;
		  default:
			bugBadCase(argf[si]);
			break;
		}
		h += 200041;
		h &= 0x3FFFFFFF;
	}

	h += foamTag(foam);
	h &= 0x3FFFFFFF;
	return h;
}

void
foamFreeNode(Foam foam)
{
	stoFree((Pointer) foam);
}

int
foamPrint(FILE *fout, Foam foam)
{
	return foamWrSExpr(fout, foam, SXRW_Default);
}

int
foamPrintDb(Foam foam)
{
	return foamWrSExpr(dbOut, foam, int0);
}

void
foamDumpToFile(Foam foam, String name)
{
	FILE *out = fileTryOpen(fnameParse(name), osIoWrMode);

	/* Only dump if we managed to open the file */
	if (out)
	{
		(void)foamWrSExpr(out, foam, SXRW_NoSrcPos);
		(void)fclose(out);
	}
	else
		(void)fprintf(dbOut, "Sorry: failed to create `%s'\n", name);
}

int
foamDefPrintDb(Foam foam, int defNo)
{
	Foam defs;

	assert(foamTag(foam) == FOAM_Unit);

	defs = foam->foamUnit.defs;

	return foamPrintDb(defs->foamDDef.argv[defNo]);
}

/* Foam Auditing */

local Bool	foamAuditExpr		(Foam foam);
local void	foamAuditBadRef		(Foam foam);
local void	foamAuditBadSharing	(Foam foam);
local void	foamAuditBadRuntime	(Foam foam);

Foam	faUnit;
Foam	faProg;
Foam	faFormats;
Foam *	faFormatsv;
Foam *	faGlobalsv;
#ifdef NEW_FORMATS
Foam *	faParamsv;
#endif
Foam *	faFluidsv;
int	faNumFormats;
int	faNumConsts;
int	faNumGlobals;
int	faNumFluids;
AInt *	faDEnv;
int	faNumLevels;
int	faConstNum;
int	faNumLocals;
int	faNumParams;

#define faNumLexes(level)	foamArgc(faFormatsv[faDEnv[level]])

#define		FOAM_AUDIT_Records		0x0001
#define		FOAM_AUDIT_Envs			0x0002
#define		FOAM_AUDIT_BCall       		0x0004
#define		FOAM_AUDIT_Values		0x0008
#define		FOAM_AUDIT_If			0x0010
#define		FOAM_AUDIT_Return		0x0020
#define		FOAM_AUDIT_Cast			0x0040


#define		FOAM_AUDIT_All			0xffff

local Bool	faAll			= false;
static Bool	foamAuditTypeChecking 	= false;  /* fluid variable */

local Bool	foamAuditRecords	= false;
local Bool	foamAuditBCall		= false;
local Bool	foamAuditEnvs		= false;
local Bool	foamAuditValues		= false;
local Bool	foamAuditIf		= false;
local Bool	foamAuditReturn		= false;
local Bool	foamAuditCast		= false;


local Bool	foamAudit0		(Foam);
local Bool	foamAuditTypeCheck	(Foam);

local Bool	faTypeCheckingValues	(Foam, Foam, AInt);
local Bool	faTypeCheckingFmtIsEnv	(Foam, AInt);
local Bool	faTypeCheckingFmtIsRec (Foam, AInt);
local Bool	faTypeCheckingBCall	(Foam);

local void	faTypeCheckingFailure	(Foam, String, ...);

local void	foamAuditUnmark		(Foam);

void
foamAuditSetAll()
{
	faAll = true;
}

Bool
foamAuditAll(Foam foam, UShort tests)
{
	Scope("foamAuditAll");
	Bool	   fluid(foamAuditTypeChecking);
	Bool		result;

	if (!faAll) Return(true);

	foamAuditTypeChecking 	= true;

	foamAuditRecords 	= (tests & FOAM_AUDIT_Records);
	foamAuditEnvs 		= (tests & FOAM_AUDIT_Envs);
	foamAuditBCall	 	= (tests & FOAM_AUDIT_BCall);
	foamAuditValues		= (tests & FOAM_AUDIT_Values);
	foamAuditIf 		= (tests & FOAM_AUDIT_If);
	foamAuditReturn		= (tests & FOAM_AUDIT_Return);
	foamAuditCast		= (tests & FOAM_AUDIT_Cast);

	result = foamAudit0(foam);

	Return(result);
}

Bool
foamAudit(Foam foam)
{
	foamAuditTypeChecking = false;

	return foamAudit0(foam);
}

/* Check variable references and formats for consistency. */
local Bool
foamAudit0(Foam foam)
{
	Bool	ok;
	assert(foamTag(foam) == FOAM_Unit);
	faUnit	     = foam;
	faFormats    = foamUnitFormats(foam); /* used for type checking */
	faFormatsv   = foamUnitFormats(foam)->foamDFmt.argv;
	faGlobalsv   = foamUnitGlobals(foam)->foamDDecl.argv;
	faFluidsv    = foamUnitGlobals(foam)->foamDDecl.argv;
#ifdef NEW_FORMATS
	faParamsv     = foamUnitParams(foam)->foamDDecl.argv;
#endif
	faNumFormats = foamArgc(foamUnitFormats(foam));
	faNumConsts  = foamDDeclArgc(foamUnitConstants(foam));
	faNumGlobals = foamDDeclArgc(foamUnitGlobals(foam));
	faNumFluids  = foamDDeclArgc(foamUnitFluids(foam));
	ok = foamAuditExpr(foam->foamUnit.defs);
	DEBUG(if (ok) fprintf(dbOut, "Foam OK\n"));
	foamAuditUnmark(foam);
	return ok;
}

local Bool
foamAuditExpr(Foam foam)
{
	Bool	result = true;

	assert(foam);
	assert(foamTag(foam) <= FOAM_LIMIT);
	if (foamMark(foam) == FOAM_MARKED)
		foamAuditBadSharing(foam);
	foamMark(foam) = FOAM_MARKED;

	switch (foamTag(foam)) {
	  case FOAM_Prog:
		faProg	    = foam;
		faDEnv	    = foam->foamProg.levels->foamDEnv.argv;
		faNumLevels = foamArgc(foam->foamProg.levels);
		faNumLocals = foamDDeclArgc(foam->foamProg.locals);
#ifdef NEW_FORMATS
		faNumParams = foamDDeclArgc(faParamsv[foam->foamProg.params-1]);
		assert(foam->foamProg.params < foamDDeclArgc(foamUnitParams(faUnit)));
#else
		faNumParams = foamDDeclArgc(foam->foamProg.params);
#endif
		break;
	  case FOAM_Def:
		if (foamTag(foam->foamDef.lhs) == FOAM_Const)
			faConstNum = foam->foamDef.lhs->foamConst.index;
		break;
	  default:
		break;
	}

	foamIter(foam, arg, foamAuditExpr(*arg));

	switch (foamTag(foam)) {
	  case FOAM_Loc:
		if (foam->foamLoc.index >= faNumLocals)
			foamAuditBadRef(foam);
		break;
	  case FOAM_Par:
		if (foam->foamPar.index >= faNumParams)
			foamAuditBadRef(foam);
		break;
	  case FOAM_Lex:
		if (foam->foamLex.level >= faNumLevels ||
		    faDEnv[foam->foamLex.level] >= faNumFormats ||
		    foam->foamLex.index >= faNumLexes(foam->foamLex.level))
			foamAuditBadRef(foam);
		break;
	  case FOAM_Const:
		if (foam->foamConst.index >= faNumConsts)
			foamAuditBadRef(foam);
		break;
	  case FOAM_Glo:
		if (foam->foamGlo.index >= faNumGlobals)
			foamAuditBadRef(foam);
		break;
	  case FOAM_Fluid:
		if (foam->foamFluid.index >= faNumFluids)
			foamAuditBadRef(foam);
		break;
	  case FOAM_EElt:
		if (foam->foamEElt.env >= faNumFormats ||
		    foam->foamEElt.lex >=
		    foamArgc(faFormatsv[foam->foamEElt.env]))
			foamAuditBadRef(foam);
		break;
	  case FOAM_RElt:
		if (foam->foamRElt.format >= faNumFormats)
			foamAuditBadRef(foam);
		break;
	  case FOAM_RRElt:
		if (foam->foamRRElt.field < 0)
			foamAuditBadRef(foam);
		break;
	  case FOAM_RRNew:
		break;
	  case FOAM_RRFmt:
		if (foamTag(foam->foamRRFmt.fmt) != FOAM_Values)
			foamAuditBadRef(foam);
		break;
	  case FOAM_RNew:
		if (foam->foamRNew.format >= faNumFormats)
			foamAuditBadRef(foam);
		break;
	  case FOAM_PushEnv:
		if (foam->foamPushEnv.format >= faNumFormats)
			foamAuditBadRef(foam);
		break;
	  case FOAM_CCall:
		if (genIsRuntime() && inlAfterInline() &&
		    foamTag(foam->foamCCall.op) == FOAM_Glo) {
			int	j = foam->foamCCall.op->foamGlo.index;
			String	id = faGlobalsv[j]->foamDecl.id;
			if (strEqual(id, "domainGetExport!"))
				foamAuditBadRuntime(foam);
		}
		break;
	  default:
		break;
	}

	if (foamAuditTypeChecking)
		result = foamAuditTypeCheck(foam);

	return result;
}

/**************************************************************************
 * NOTE: This procedure doesn't perform type checking on subtrees,
 *       except in the case of (Values ...).
 *
 * PLEASE, update this documentation if other controls are added.
 *
 * WHAT is checked?
 *
 *	- (Set typeA typeB)      typeA == typeB ?
 *	- (Set (Values X1..Xn) (MFmt F ..)) -> has F n slots ?
 *					    -> type Xi correspond to type slot?
 * 	- (If (test is Boolean) ..)
 *	- (Return expr)  		-> does expr match type of Prog ?
 *	- (Return (Values ...))		-> as (Set (Values ...
 *	- (Cast T (expr))		-> expr already of type T ?
 *	- (PushEnv FMT ...)	        -> Is FMT an env format ?
 *	- (EElt FMT ...)		->   "        "     "
 *	- (BCall ...)			-> arguments type checking
 *	- (  FMT ), appearing in a record context -> is a record format ?
 *
 **************************************************************************/
local Bool
foamAuditTypeCheck(Foam foam)
{
	AInt		type, fmt;

	switch (foamTag(foam)) {
	case FOAM_Set:
	case FOAM_Def: {
		Foam	lhs, rhs;
		AInt	typeLhs, typeRhs, fmtLhs, fmtRhs;

		lhs = foam->foamSet.lhs;
		rhs = foam->foamSet.rhs;

		if (!foamIsRef(lhs) && !foamTag(lhs) == FOAM_Values) {
			faTypeCheckingFailure(foam, "lhs is not an l-value");
			return false;
		}

		if (foamTag(lhs) == FOAM_Values) {

			if (foamTag(rhs) != FOAM_MFmt) {
				faTypeCheckingFailure(foam,
				"lhs is Values, but no MFmt on the rhs");
			return false;
			}

			return faTypeCheckingValues(foam, lhs, 
						    rhs->foamMFmt.format);
		}
		else {
			typeLhs = foamExprType(lhs, faProg, faFormats,
					    NULL, NULL, &fmtLhs);
			typeRhs = foamExprType(rhs, faProg, faFormats,
					    NULL, NULL, &fmtRhs);

			if (typeLhs != typeRhs) {
				faTypeCheckingFailure(foam,
					"The type of lhs (%s) doesn't match type of rhs (%s).", foamInfo(typeLhs).str, foamInfo(typeRhs).str);
				return false;
			}

			if (typeLhs == FOAM_Rec && fmtLhs != fmtRhs) {
				faTypeCheckingFailure(foam,
					"assignment between records with different formats");
				return false;
			}
			if (typeLhs == FOAM_Arr && fmtLhs != fmtRhs) {
				faTypeCheckingFailure(foam,
					"assignment between array with different base type (%s - %s)", foamInfo(fmtLhs).str, foamInfo(fmtRhs).str);
				return false;
			}
		}

		return true;
	}
	case FOAM_If:

		if (!foamAuditIf) return true;

		type = foamExprType(foam->foamIf.test, faProg, faFormats,
				    NULL, NULL, NULL);

		if (type != FOAM_Bool) {
				faTypeCheckingFailure(foam,
					"test of 'If' is not FOAM_Bool");
				return false;
		}
		return true;

	case FOAM_Return:

		if (!foamAuditReturn) return true;

		if (faProg->foamProg.retType == FOAM_NOp) {

			if (foamTag(foam->foamReturn.value) != FOAM_Values) {
		      		faTypeCheckingFailure(foam,
					"Prog should return Values expr and a return without Values has been found");
			return false;
			}

			return faTypeCheckingValues(foam,
						    foam->foamReturn.value,
						    faProg->foamProg.format);
		}

		type = foamExprType(foam->foamReturn.value, faProg, faFormats,
				    NULL, NULL, &fmt);

		if (type != faProg->foamProg.retType) {
		      faTypeCheckingFailure(foam,
				"Return value type doesn't match Prog return type");
			return false;
		}

		if (type == FOAM_Rec || type == FOAM_Arr) {
		      faTypeCheckingFailure(foam,
				"returning a Record or an Array");
			return false;
		}

		return true;

	case FOAM_Cast:

		if (!foamAuditCast) return true;

		type = foamExprType(foam->foamCast.expr, faProg, faFormats,
				    NULL, NULL, NULL);

		if (type == foam->foamCast.type) {
		      	faTypeCheckingFailure(foam,
				"cast to the same type");
			return false;
		}
		return true;

      /* ---------------- Envs ------------------------------- */

	case FOAM_PushEnv:
		fmt = foam->foamPushEnv.format;
		return faTypeCheckingFmtIsEnv(foam, fmt);

	case FOAM_EElt:
		fmt = foam->foamEElt.env;
		return faTypeCheckingFmtIsEnv(foam, fmt);

	case FOAM_BCall:
		return faTypeCheckingBCall(foam);
	      
	default:
		return true;
	}
}

/* Given (Values X1..Xn) and a format, verify arity and type
 * FOAM is used to print the error msg
 */
local Bool
faTypeCheckingValues(Foam foam, Foam values, AInt formatNo)
{
	int	numFmtSlots, i;
	Foam	decl;
	AInt	type, fmt;
	Bool	result = true;

	assert(foamTag(values) == FOAM_Values);

	if (!foamAuditValues) return true;

	/* Progs with formatNo = 0 are nullary */
	numFmtSlots = (formatNo ? foamDDeclArgc(faFormatsv[formatNo]) : 0);

	if (formatNo == 0 && foamArgc(values) == 0) 
			return true;

	if (foamArgc(values) != numFmtSlots) {
		faTypeCheckingFailure(foam,
		  	"Values arity and fmt slots different");
		return false;
	}
	
	for (i = 0; i < numFmtSlots; i++) {
		type = foamExprType(values->foamValues.argv[i], faProg,
				    faFormats, NULL, NULL, &fmt);
		decl = faFormatsv[formatNo]->foamDDecl.argv[i];

		if (type != decl->foamDecl.type) {
			faTypeCheckingFailure(foam,
				"type of arg %d (%s) of Values doesn't match the type of corresponding slot (%s)",
				i, foamInfo(type).str,
				foamInfo(decl->foamDecl.type).str);
			result = false;
		}

		if ((type == FOAM_Rec || type == FOAM_Arr) &&
		    fmt != decl->foamDecl.format) {
			faTypeCheckingFailure(foam,
				"format of arg %d of Values doesn't match the format of corresponding slot", i);
			result = false;
		}

		if (type == FOAM_Rec &&
		    !faTypeCheckingFmtIsRec(foam, fmt))
			result = false;
	}

	return result;
}

local Bool
faTypeCheckingFmtIsEnv(Foam foam, AInt format)
{
	if (!foamAuditEnvs) return true;

	if (faFormatsv[format]->foamDDecl.usage != FOAM_DDecl_LocalEnv &&
	    faFormatsv[format]->foamDDecl.usage != FOAM_DDecl_NonLocalEnv) {
		faTypeCheckingFailure(foam,
		  	"NOT environment format used in environment context");
		return false;
		
	}

	return true;
}

local Bool
faTypeCheckingFmtIsRec(Foam foam, AInt format)
{
	if (!foamAuditRecords) return true;

	if (faFormatsv[format]->foamDDecl.usage != FOAM_DDecl_Record) {
		faTypeCheckingFailure(foam,
		  	"NOT record format (%d) used in record context",
				      format);
		return false;
		
	}

	return true;
}

local Bool
faTypeCheckingBCall(Foam foam)
{
	int 		i, nargs;
	FoamBValTag	op;
	AInt		argType, parType, fmt;
	Bool		result = true;

	assert(foamTag(foam) == FOAM_BCall);

	if (!foamAuditBCall) return true;

	op = foam->foamBCall.op;
	nargs = foamBValInfo(op).argCount;

	for (i = 0; i < nargs; i++) {

		argType = foamExprType(foam->foamBCall.argv[i],
				       faProg, faFormats, NULL, NULL, &fmt);

		parType = foamBValInfo(op).argTypes[i];

		if (argType != parType) {
			faTypeCheckingFailure(foam,
		  		"Bad arg type (%s) to BCall: expected %s.",
					      foamInfo(argType).str,
					      foamInfo(parType).str);
			result = false;
		}

		if (argType == FOAM_Rec &&
		    !faTypeCheckingFmtIsRec(foam, fmt))
			result = false;
	}

	return result;
}

local void
faTypeCheckingFailure(Foam foam, String msg, ...)
{
	va_list	argp;
	va_start(argp, msg);

	fprintf(dbOut, "\n------ FoamAudit Type Checking failure in const %d: ------\n>> ",
		 faConstNum);
	vfprintf(dbOut, msg, argp);
	fprintf(dbOut, "\nThe foam expression that caused the failure is:\n");

	foamWrSExpr(dbOut, foam, SXRW_AsIs);

	va_end(argp);
}

/* Reset the foam sharing mark. */
local void
foamAuditUnmark(Foam foam)
{
	foamIter(foam, arg, foamAuditUnmark(*arg));

	foamMark(foam) = FOAM_UNMARKED;
}

local void
foamAuditBadRef(Foam foam)
{
	foamPrint(stderr, foam);
	foamDEBUG(foamPrint(dbOut, faUnit));
	bug("\nBad foam reference in const %d:\n", faConstNum);
}

local void
foamAuditBadSharing(Foam foam)
{
	foamPrint(stderr, foam);
	foamDEBUG(foamPrint(dbOut, faUnit));
	bug("\nBad foam sharing in const %d:\n", faConstNum);
}

local void
foamAuditBadRuntime(Foam foam)
{
	foamPrint(stderr, foam);
	foamDEBUG(foamPrint(dbOut, faUnit));
	fprintf(dbOut, "\nBad runtime call to domainGetExport in const %d:\n",
		faConstNum);
}

int
foamStdPrint(Foam foam)
{
	return foamPrint(dbOut, foam);
}

/* Return true if foam is a lhs - except Values */
Bool
foamIsRef(Foam foam)
{
	switch(foamTag(foam)) {

	case FOAM_Loc:
	case FOAM_Par:
	case FOAM_Lex:
	case FOAM_Glo:
	case FOAM_Const:
	case FOAM_RElt:
	case FOAM_RRElt:
	case FOAM_IRElt:
	case FOAM_TRElt:
	case FOAM_EElt:
	case FOAM_EInfo:
	case FOAM_PRef:
	case FOAM_CEnv:
	case FOAM_CProg:
	case FOAM_AElt:
		return true;
	default:
		return false;
	}
}

/*
 * Determine when a foam expression is immediate data.
 */
Bool
foamIsData(Foam foam)
{
	switch(foamTag(foam)) {
	  case FOAM_Nil:
	  case FOAM_Char:
	  case FOAM_Bool:
	  case FOAM_Byte:
	  case FOAM_HInt:
	  case FOAM_SInt:
	  case FOAM_BInt:
	  case FOAM_SFlo:
	  case FOAM_DFlo:
	  case FOAM_Word:
	  case FOAM_Arb:
	  case FOAM_Arr:
		return true;
	  case FOAM_Cast:
		return foamIsData(foam->foamCast.expr);
	  default:
		return false;
	}
}

/*
 * Returns a list of symes refered to in the foam.
 */
SymeList
foamSymeList(Foam foam)
{
	Foam		*fmtv, *declv, dfmt, decl;
	Length		fmtc, declc, i, j;
	Syme		syme;
	SymeList	l = listNil(Syme);

	assert(foamTag(foam) == FOAM_Unit);
	dfmt = foam->foamUnit.formats;

	assert(foamTag(dfmt) == FOAM_DFmt);
	fmtv = dfmt->foamDFmt.argv;
	fmtc = foamArgc(dfmt);

	for (i = 1; i < fmtc; i += 1) {
		assert(foamTag(fmtv[i]) == FOAM_DDecl);
		declv = fmtv[i]->foamDDecl.argv;
		declc = foamDDeclArgc(fmtv[i]);
#ifdef NEW_FORMATS
		if (declc > 0 && foamTag(declv[0]) == FOAM_DDecl) break;
#endif
		for (j = 0; j < declc; j += 1) {
			decl = declv[j];
			assert(foamIsDecl(decl));
			syme = foamSyme(decl);
			if (syme) l = listCons(Syme)(syme, l);
		}
	}

	return listNReverse(Syme)(l);
}

/*****************************************************************************
 *
 * :: General Utilities
 *
 ****************************************************************************/

/*
 * This can modify its argument, or even free parts of it.
 */
Foam
foamNotThis(Foam foam)
{
	return foamNew(FOAM_BCall, 2, FOAM_BVal_BoolNot, foam);
}

int
foamCountSubtreesOfKind(Foam foam, FoamTag kind)
{
	int	i, count;

	assert(foam && foamTag(foam) == FOAM_Seq);

	for (count = 0, i = 0; i < foamArgc(foam); i++)
		if (foamTag(foam->foamSeq.argv[i]) == kind) count++;

	return count;
}

/*****************************************************************************
 *
 * :: Foam SExpr I/O
 *
 ****************************************************************************/

int
foamWrSExpr(FILE *file, Foam foam, ULong sxioMode)
{
	SExpr	sx;
	int	cc;

	sx = foamToSExpr(foam);
	cc = sxiWrite(file, sx, SXRW_MixedCase | sxioMode);
	sxiFree(sx);

	return cc;
}

Foam
foamRdSExpr(FILE *file, FileName *pfn, int *plno)
{
	SExpr	sx;
	Foam	foam;

	sx   = sxiRead(file, pfn, plno, sxNil, SXRW_MixedCase | SXRW_SrcPos);
	foam = foamFrSExpr(sx);
	sxiFree(sx);

	return foam;
}

/*****************************************************************************
 *
 * :: Foam-SExpr conversion
 *
 ****************************************************************************/

static	Foam	*fexFmtv;	/* DDecls for formats  */
static	Length	fexFmtc;
static	Foam	*fexGlov;	/* Decls for globals   */
static	Length	fexGloc;
static	Foam	*fexConstv;	/* Decls for constants */
static	Length	fexConstc;
static	Foam	*fexParv;	/* Decls for params    */
static	Length	fexParc;
static	Foam	*fexLocv;	/* Decls for locals    */
static	Length	fexLocc;
static	AInt	*fexLexv;	/* Format numbers in DEnv */
static	Length	fexLexc;

#define fexSymbol(id)		sxiFrSymbol(symIntern(id))

/* Define StripHash to remove the hash code from the end of global names. */
#undef	StripHash

/* Define StripName to remove the file name from the front of global names. */
#undef	StripName

/* Define StripIndex to remove the loc/lex/glo index when we have an idstr. */
#undef	StripIndex

#ifdef StripHash
String
foamStripHash(String str)
{
	int	i, j = 0;
	for (i = strLength(str) - 1; i > 0; i -= 1) {
		if (str[i] == '_')
			break;
		if (!isdigit(str[i]))
			return str;
	}
	if (i <= 0 || i == strLength(str) - 1)
		return str;

#ifdef StripName
	for (j = 0; j < i; j += 1)
		if (str[j] == '_') break;

	if (j == 0 || j == i)
		return str;
#endif

	return strnCopy(str + j, i - j);
}
#endif

local SExpr
foamToSExpr0(Foam foam)
{
	int	si, fi;
	String	argf, idstr;
	SExpr	sx, sxi;
	int	i, j;

	Bool	isDecl;
	long	li;

	assert (foam);

	if (!foam) 
		return sxiFrString("Bad-Foam-0");
	/*
	 * Code for determining identifers.
	 */
	idstr = 0;

	switch (foamTag(foam)) {
	case FOAM_Unit:
		fexFmtv	  = foam->foamUnit.formats->foamDFmt.argv;
		fexFmtc	  = foamArgc(foam->foamUnit.formats);
		fexGlov	  = foamUnitGlobals(foam)->foamDDecl.argv;
		fexGloc	  = foamDDeclArgc(foamUnitGlobals(foam));
#ifdef NEW_FORMATS
		faParamsv  = foamUnitParams(foam)->foamDDecl.argv;
#endif
		fexConstv = foamUnitConstants(foam)->foamDDecl.argv;
		fexConstc = foamDDeclArgc(foamUnitConstants(foam));
		break;
	case FOAM_Prog:
#ifdef NEW_FORMATS
		fexParv	  = (faParamsv[foam->foamProg.params-1])->foamDDecl.argv;
		fexParc	  = foamDDeclArgc(faParamsv[foam->foamProg.params-1]);
#else
		fexParv	  = foam->foamProg.params->foamDDecl.argv;
		fexParc	  = foamDDeclArgc(foam->foamProg.params);
#endif
		fexLocv	  = foam->foamProg.locals->foamDDecl.argv;
		fexLocc	  = foamDDeclArgc(foam->foamProg.locals);
		if (fexFmtc != 0) {
			fexLexv	  = foam->foamProg.levels->foamDEnv.argv;
			fexLexc	  = foamDDeclArgc(fexFmtv[foam->foamProg.levels->
						     foamDEnv.argv[0]]);
		}
		break;

#ifdef NEW_FORMATS
	case FOAM_DDecl:
		if (foam->foamDDecl.usage == paramsSlot)
			return sxNil;
#endif
	case FOAM_Par:
		j = foam->foamPar.index;
		if (j < fexParc)
			idstr = fexParv[j]->foamDecl.id;
		break;
	case FOAM_Loc:
		j = foam->foamLoc.index;
		if (j < fexLocc)
			idstr = fexLocv[j]->foamDecl.id;
		break;
	case FOAM_Glo:
		j = foam->foamGlo.index;
		if (j < fexGloc)
			idstr = fexGlov[j]->foamGDecl.id;
#ifdef StripHash
		if (idstr)
			idstr = foamStripHash(idstr);
#endif
		break;
	case FOAM_Const:
		j = foam->foamConst.index;
		if (j < fexConstc)
			idstr = fexConstv[j]->foamDecl.id;
		break;
	case FOAM_Lex:
		i = foam->foamLex.level;
		j = foam->foamLex.index;
		if (fexFmtc != 0) {
			i = fexLexv[i];
			if (j < foamArgc(fexFmtv[i]))
			    idstr = fexFmtv[i]->foamDDecl.argv[j]->foamDecl.id;
		}
		break;

	case FOAM_EElt:
		i = foam->foamEElt.env;
		j = foam->foamEElt.lex;
		if (fexFmtc != 0)
			idstr = fexFmtv[i]->foamDDecl.argv[j]->foamDecl.id;
		break;

	default:
		break;
	}
	if (idstr && !*idstr) idstr = 0;

	/*
	 * Cons the subexpressions into a list.
	 */
	sx    = sxNil;
	foamSposDEBUG(
		if (foamPos(foam) != sposNone)
			sx = sxCons(sxiFrInteger(sposLine(foamPos(foam))), sx);
	);
	argf  = foamInfo(foamTag(foam)).argf;
	sx    = sxCons(foamSExpr(foamTag(foam)), sx);

	isDecl = foamTag(foam) == FOAM_Decl || foamTag(foam) == FOAM_GDecl;

	for (si = fi = 0; si < foamArgc(foam); si++, fi++) {
		if (argf[fi] == '*') fi--;

		switch (argf[fi]) {
		case 'X':
		case 'F':
		case 'L':
		case 'b':
		case 'h':
		case 'w':
		case 'i':
			li = isDecl && argf[fi] == 'w' ? -1 :
				(long) foamArgv(foam)[si].data;
			DEBUG({
				li  = (long) foamArgv(foam)[si].data;
			});
			sxi = sxiFrInteger(li);
#ifdef NEW_FORMATS
			DEBUG({
				if (foamTag(foam) == FOAM_Prog
				    && si > 4) {
					assert(li < fexFmtc);
					sxi = foamToSExpr0(fexFmtv[li]);
				}
			});
#endif
			break;
		case 't':
			sxi = foamSExpr(foamArgv(foam)[si].data);
			break;
		case 'o':
			sxi = foamBValSExpr(foamArgv(foam)[si].data);
			break;
		case 'p':
			sxi = foamProtoSExpr(foamArgv(foam)[si].data);
			break;
		case 'D':
			sxi = foamDDeclSExpr(foamArgv(foam)[si].data);
			break;
		case 's':
#ifdef StripHash
			if (foamTag(foam) == FOAM_Decl) {
				String	str = foamArgv(foam)[si].str;
				str = foamStripHash(str);
				sxi = sxiFrString(str);
			}
			else
#endif
			sxi = sxiFrString(foamArgv(foam)[si].str);
			break;
		case 'f':
			sxi = sxiFrSFloat(foamToSFlo(foam));
			si = foamArgc(foam);
			break;
		case 'd':
			sxi = sxiFrDFloat(foamToDFlo(foam));
			si = foamArgc(foam);
			break;
		case 'n':
			sxi = sxiFrBigInteger(foamArgv(foam)[si].bint);
			break;
		case 'C':
			sxi = foamToSExpr0(foamArgv(foam)[si].code);
			break;
		default:
			bugBadCase(argf[fi]);
			NotReached(sxi = 0);
		}
		sx = sxCons(sxi, sx);
	}
#ifdef StripIndex
	if (idstr) sx = sxCons(sxiFrSymbol(symIntern(idstr)), sxCdr(sx));
#else
	if (idstr) sx = sxCons(sxiFrSymbol(symIntern(idstr)), sx);
#endif

	sx = sxNReverse(sx);
	sx = sxiRepos(foamPos(foam), sx);

	return sx;
}

SExpr
foamToSExpr(Foam foam)
{
	if (!foamIsInit) foamInit();
	fexFmtc = fexGloc = fexConstc = fexParc = fexLocc = fexLexc = 0;
	return foamToSExpr0(foam);
}

#define croak(sx, msg)	comsgFatal(abNewNothing(sxiPos(sx)), msg)

Foam
foamFrSExpr(SExpr sx)
{
	Foam	foam;
	Symbol	op, sym;
	SExpr	sxi;
	String	argf;
	int	fi, si, tag, argc;

	if (!foamIsInit) foamInit();

	
	if (!sxiConsP(sx))  croak(sx, ALDOR_F_LoadNotList);
	if (!sxiSymbolP(sxi = sxCar(sx))) croak(sxi, ALDOR_F_LoadNotSymbol);

	op   = sxiToSymbol(sxi);
	if (!symCoInfo(op) || symCoInfo(op)->foamTagVal == FOAM_BVAL_LIMIT)
		croak(sxi, ALDOR_F_LoadNotFoam);

	tag  = symCoInfo(op)->foamTagVal;
	argf = foamInfo(tag).argf;
	argc = foamInfo(tag).argc;
	if (argc == FOAM_NARY) argc = sxiLength(sx) - 1; /* -1 For tag */

	if (tag == FOAM_DFlo)
		foam   = foamNewDFlo(0.0);
	else
		foam   = foamNewEmpty(tag, argc);

	for (fi = si = 0, sx=sxCdr(sx); si < argc; fi++, si++, sx=sxCdr(sx)) {
		if (argf[fi] == '*') fi--;

		if (!sxiConsP(sx)) croak(sx, ALDOR_F_LoadNotList);

		sxi = sxCar(sx);

		switch (argf[fi]) {
		case 'X':
		case 'F':
		case 'L':
		case 'b':
		case 'h':
		case 'w':
		case 'i':
			if (!sxiIntegerP(sxi)) croak(sxi, ALDOR_F_LoadNotInteger);
			foamArgv(foam)[si].data = sxiToInteger(sxi);
			break;
		case 't':
		case 'o':
		case 'p':
		case 'D':
			if (!sxiSymbolP(sxi)) croak(sxi, ALDOR_F_LoadNotSymbol);
			sym = sxiToSymbol(sxi);

			if (!symCoInfo(sym) || symCoInfo(sym)->foamTagVal==-1)
				croak(sxi, ALDOR_F_LoadNotFoam);
			foamArgv(foam)[si].data = symCoInfo(sym)->foamTagVal;
			break;
		case 's':
			if (!sxiStringP(sxi)) croak(sxi, ALDOR_F_LoadNotString);
			foamArgv(foam)[si].str = sxiToString(sxi);
			break;
		case 'f':
			if (!sxiSFloatP(sxi)) croak(sxi, ALDOR_F_LoadNotFloat);
			foamToSFlo(foam) = sxiToSFloat(sxi);
			break;
		case 'd':
			if (!sxiDFloatP(sxi)) croak(sxi, ALDOR_F_LoadNotFloat);
			foamToDFlo(foam) = sxiToDFloat(sxi);
			break;
		case 'n':
			if (!sxiIntegerP(sxi)) croak(sxi, ALDOR_F_LoadNotInteger);
			foamArgv(foam)[si].bint = sxiToBigInteger(sxi);
			break;
		case 'C':
			if (!sxiConsP(sxi))  croak(sxi, ALDOR_F_LoadNotList);
			foamArgv(foam)[si].code = foamFrSExpr(sxi);
			break;
		default:
			bugBadCase(argf[fi]);
		}
	}
	foamPos(foam) = sxiPos(sx);
	return foam;
}

/*****************************************************************************
 *
 * :: Foam-String conversion
 *
 ****************************************************************************/

String
foamToString(Foam foam)
{
	String	s;
	Buffer	buf;

	buf = bufNew();
	foamToBuffer(buf, foam);
	s = bufLiberate(buf);

	return s;
}

Foam
foamFrString(String s)
{
	Foam	foam;
	Buffer	buf;

	buf = bufCapture(s, stoSize(s));
	foam = foamFrBuffer(buf);
	bufLiberate(buf);

	return foam;
}

/*****************************************************************************
 *
 * :: Byte code conversion to/from Buffer
 *
 *****************************************************************************/

#define	 STD_FORMS	2	/* Number of standard formats.
				 * I.e. all 4 or 2 or 1 bytes.
				 * Cannot be changed */
#define	 IMMED_FORMS	3	/* Number of immediate, implicit formats.
				 * I.e. byte = 0, 1,...
				 * Can be changed. */
#define	 NUM_FORMS	(STD_FORMS + IMMED_FORMS)


#define FFO_ORIGIN	(FOAM_VECTOR_START)
#define FFO_SPAN	(FOAM_LIMIT - FFO_ORIGIN)

#define FOAM_FORMAT_GET(tag)	    ((tag)<FFO_ORIGIN? 0:FOAM_FORMAT_GET_X(tag))
#define FOAM_FORMAT_GET_X(tag)	    (((tag)-FFO_ORIGIN)/FFO_SPAN)
#define FOAM_FORMAT_PUT(tag,   fmt) ((tag) + (fmt)*FFO_SPAN)
#define FOAM_FORMAT_REMOVE(tag,fmt) ((tag) - (fmt)*FFO_SPAN)

#define FOAM_FORMAT_FOR(n) \
	((long)(n) <= MAX_BYTE ? 1 : 0)

#define FOAM_PUT_INT(format, buf, i) { \
	switch (format) {				\
	case 0:	 BUF_PUT_SINT(buf, i); break;		\
	case 1:	 BUF_PUT_BYTE(buf, i); break;		\
	default: break; /* Included in tag. */		\
	}						\
}

#define FOAM_GET_INT(format, buf, i) {			\
	switch (format) {				\
	case 0:	 BUF_GET_SINT(buf, i); break;		\
	case 1:	 BUF_GET_BYTE(buf, i); break;		\
	default: (i) = (format) - STD_FORMS; break;	\
	}						\
}

int
foamTagLimit(void)
{
	return 1 * FFO_ORIGIN + NUM_FORMS * (FFO_SPAN);
}


/* For debugging */
int
foamTagSpanLength(void)
{
	return FFO_SPAN;
}


local int	foamTagFormat	(Foam);
local int	labelFmt;

/*
 * External entry point for reading foam byte codes from a buffer.
 */

Foam
foamFrBuffer(Buffer buf)
{
	Foam	foam;
	int	fi, si, tag, argc, format, bi;
	Bool	isArr, isNary;
	String	argf;
	Bool	neg;
	BInt	bint;

	BUF_GET_BYTE(buf, tag);
	format = FOAM_FORMAT_GET(tag);
	tag    = FOAM_FORMAT_REMOVE(tag, format);

	isArr  = (tag == FOAM_Arr);
	isNary = (foamInfo(tag).argc == FOAM_NARY);

	if (!isNary)
		argc = foamInfo(tag).argc;
	else
		FOAM_GET_INT(format, buf, argc);

	argf  = foamInfo(tag).argf;
	if (tag == FOAM_DFlo)
		foam   = foamNewDFlo(0.0);
	else
		foam   = foamNewEmpty(tag, argc);

	for (fi = si = 0; si < argc; fi++, si++) {
		int	af = argf[fi], n, slen;
		if (af == '*') af = argf[--fi];
		switch (argf[fi]) {
		case 't':
			BUF_GET_BYTE(buf, n);
			foamArgv(foam)[si].data = FOAM_START + n;
			break;
		case 'o':
#if SMALL_BVAL_TAGS
			BUF_GET_BYTE(buf, n);
#else
			BUF_GET_HINT(buf, n);
#endif
			foamArgv(foam)[si].data = FOAM_BVAL_START + n;
			break;
		case 'p':
			BUF_GET_BYTE(buf, n);
			foamArgv(foam)[si].data = FOAM_PROTO_START + n;
			break;
		case 'D':
			BUF_GET_BYTE(buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 'b':
			BUF_GET_BYTE(buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 'h':
			BUF_GET_HINT(buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 'w':
			BUF_GET_SINT(buf, n);
			if (isArr) {
				if (foam->foamArr.baseType == FOAM_Char)
					n = charFrAscii(n);
			}
			foamArgv(foam)[si].data = n;
			break;
		case 'X':
			/* Throw away length/offset information in tree form. */
			/* This makes .fm the same whether from .as or .ao. */
			FOAM_GET_INT(int0, buf, n);
			foamArgv(foam)[si].data = 0;
			break;
		case 'F':
			FOAM_GET_INT(int0, buf, n);
			foamArgv(foam)[si].data = n;
			labelFmt = FOAM_FORMAT_FOR(n);
			break;
		case 'L':
			FOAM_GET_INT(labelFmt, buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 'i':
			FOAM_GET_INT(format, buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 's':
			FOAM_GET_INT(format, buf, slen);
			foamArgv(foam)[si].str = bufRdChars(buf, slen);
			break;
		case 'f':
			foamToSFlo(foam) = bufRdSFloat(buf);
			si = argc;
			break;
		case 'd':
			foamToDFlo(foam) = bufRdDFloat(buf);
			si = argc;
			break;
		case 'n': {
			BInt b;
			BUF_GET_BYTE(buf, neg);
			FOAM_GET_INT(format, buf, slen);
			bint = bintAllocPlaces(slen);
			bint->isNeg = neg;
			for (bi = 0; bi < slen; bi++) {
				BUF_GET_HINT(buf, n);
				bint->placev[bi] = n;
			}
			b = xintImmedIfCan(bint);
			if (b != bint) bintFree(bint);
			foamArgv(foam)[si].bint = b;
			break;
		}
		case 'C':
			foamArgv(foam)[si].code = foamFrBuffer(buf);
			break;
		default:
			bugBadCase(argf[fi]);
		}
	}
	return foam;
}

/* Get the header of a Prog skipping the body. It stops when find the first
 * piece of foam, so parameters, locals, dfluis/denv and body are not
 * returned.
 * Return NULL if the constant does not refer to a Prog.
 * NOTE: this procedure could be more general, but less efficient. This
 * 	implementations rely on the fact that if a foam subtree is found, then
 *	all the remaining subtrees are foam (as in Prog).
 *	This implementation needs to be fast because is used by the inliner.
 */

local Foam
foamProgHdrFrBuffer(Buffer buf)
{
	Foam	foam;
	int	fi, si, tag, argc, format, bi;
	Bool	isArr, isNary;
	String	argf;
	Bool	neg;

	BUF_GET_BYTE(buf, tag);
	format = FOAM_FORMAT_GET(tag);
	tag    = FOAM_FORMAT_REMOVE(tag, format);

	isArr  = (tag == FOAM_Arr);
	isNary = (foamInfo(tag).argc == FOAM_NARY);

	if (!isNary)
		argc = foamInfo(tag).argc;
	else
		FOAM_GET_INT(format, buf, argc);

	if (tag != FOAM_Prog) return NULL;

	argf  = foamInfo(tag).argf;

	foam   = foamNewEmpty(tag, argc);

	for (fi = si = 0; si < argc; fi++, si++) {
		int	af = argf[fi], n, slen;
		if (af == '*') af = argf[--fi];
		switch (argf[fi]) {
		case 't':
			BUF_GET_BYTE(buf, n);
			foamArgv(foam)[si].data = FOAM_START + n;
			break;
		case 'o':
#if SMALL_BVAL_TAGS
			BUF_GET_BYTE(buf, n);
#else
			BUF_GET_HINT(buf, n);
#endif
			foamArgv(foam)[si].data = FOAM_BVAL_START + n;
			break;
		case 'p':
			BUF_GET_BYTE(buf, n);
			foamArgv(foam)[si].data = FOAM_PROTO_START + n;
			break;
		case 'b':
			BUF_GET_BYTE(buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 'h':
			BUF_GET_HINT(buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 'w':
			BUF_GET_SINT(buf, n);
			if (isArr) {
				if (foam->foamArr.baseType == FOAM_Char)
					n = charFrAscii(n);
			}
			foamArgv(foam)[si].data = n;
			break;
		case 'X':
			/* Throw away length/offset information in tree form. */
			/* This makes .fm the same whether from .as or .ao. */
			FOAM_GET_INT(int0, buf, n);
			foamArgv(foam)[si].data = 0;
			break;
		case 'F':
			FOAM_GET_INT(int0, buf, n);
			foamArgv(foam)[si].data = n;
			labelFmt = FOAM_FORMAT_FOR(n);
			break;
		case 'L':
			FOAM_GET_INT(labelFmt, buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 'i':
			FOAM_GET_INT(format, buf, n);
			foamArgv(foam)[si].data = n;
			break;
		case 's':
			FOAM_GET_INT(format, buf, slen);
			foamArgv(foam)[si].str = bufRdChars(buf, slen);
			break;
		case 'f':
			foamToSFlo(foam) = bufRdSFloat(buf);
			si = argc;
			break;
		case 'd':
			foamToDFlo(foam) = bufRdDFloat(buf);
			si = argc;
			break;
		case 'n': {
			BInt b;
			U16 *data;
			BUF_GET_BYTE(buf, neg);
			FOAM_GET_INT(format, buf, slen);
			data = (U16*) stoAlloc(OB_Other, slen*sizeof(U16));
			for (bi = 0; bi < slen; bi++) {
				BUF_GET_HINT(buf, n);
				data[bi] = n;
			}
			b = bintFrPlacevS(neg, slen, data);
			stoFree(data);
			foamArgv(foam)[si].bint = b;
			break;
		}
		case 'C':
			break;
		default:
			bugBadCase(argf[fi]);
		}
	}
	return foam;
}


void
foamPosFrBuffer(Buffer buf, Foam foam)
{
	int	fi, si, tag, argc;
	String	argf;

	tag    = foamTag(foam);
	argc   = foamArgc(foam);
	argf   = foamInfo(tag).argf;

	if (tag == FOAM_Seq) {
		for (si = 0; si < argc; si++)
			foamPos(foamArgv(foam)[si].code) = bufRdULong(buf);
	}
	else {
		if (tag == FOAM_Prog)
			foamPos(foam) = bufRdULong(buf);
		for (fi = si = 0; si < argc; fi++, si++) {
			int	af = argf[fi];
			if (af == '*') af = argf[--fi];

			switch (af) {
			  case 'C':
				foamPosFrBuffer(buf, foamArgv(foam)[si].code);
				break;
			  default:
				break;
			}
		}
	}
}

Foam
foamSIntReduce(Foam foam)
{
	/*
	 * Convert arbitrarily large integer literals into an equivalent
	 * expression involving only unsigned 31 bit arithmetic. This is
	 * to allow >32-bit constants on 64-bit platforms to be stored in
	 * flat FOAM buffers/files and be retrieved correctly.
	 */
	int	negative = (foam->foamSInt.SIntData < 0);
	long	bignum = ((-1*negative)*foam->foamSInt.SIntData) >> 31;
	assert(foamTag(foam) == FOAM_SInt);
	if (bignum) {
		/* Must split into unsigned 31-bit chunks */
		int	i, bits = sizeof(foam->foamSInt.SIntData)*8;
		int	hunks = bits/31 + ((bits%31) ? 1 : 0);
		long	*parts = (long *)stoAlloc(OB_Other, hunks*sizeof(long));

		/* Kill the sign */
		long	number = (-1*negative)*foam->foamSInt.SIntData;

		/* Split ... */
		for (i = 0; i < hunks; i++)
			parts[i] = number & 0x7fffffff, number >>= 31;

		/* Find most significant chunk */
		for (i = hunks - 1; i >= 0 && !parts[i]; i--) {}

		/* Reconstruct ... */
		foam = foamNewSInt(parts[i--]); /* Don't foamFree(foam) ! */
		while (i >= 0) {
			foam = foamNew(FOAM_BCall, 3, FOAM_BVal_SIntShiftUp,
					foam, foamNewSInt(31));
			foam = foamNew(FOAM_BCall, 3, FOAM_BVal_SIntOr,
					foam, foamNewSInt(parts[i--]));
		}

		/* Deal with the sign */
		if (negative)
			foam = foamNew(FOAM_BCall,2,FOAM_BVal_SIntNegate,foam);
		stoFree(parts);
	}
	return foam;
}

/*
 * External entry point for writing foam byte codes to a buffer.
 */

int
foamToBuffer(Buffer buf, Foam foam)
{
	int	fi, si, tag, argc, format, bi;
	Bool	isArr, isNary;
	BInt	bint;
	String	argf;
	Offset	tmpPos, offPos = 0;
	Offset	start = bufPosition(buf);

#if AXL_EDIT_1_1_13_30
	if (foamTag(foam) == FOAM_SInt) foam = foamSIntReduce(foam);
#else
	/* The '!=0' is wrong, but this should be fixed better anyways. */
	if (foamTag(foam) == FOAM_SInt 
	    && sizeof(foam->foamSInt.SIntData) > 4
	    && (labs(foam->foamSInt.SIntData) >> 32) != 0) 
	  	foam = foamNew(FOAM_BCall, 3,
			       FOAM_BVal_SIntOr,
			       foamNew(FOAM_BCall, 3,
				       FOAM_BVal_SIntShiftUp, 
				       foamNewSInt(foam->foamSInt.SIntData >> 32L),
				       foamNewSInt(32)),
			       foamNewSInt(foam->foamSInt.SIntData 
					   & ((1L <<32L) - 1L)));
#endif

	tag    = foamTag(foam);

	isArr  = (tag == FOAM_Arr);
	isNary = (foamInfo(tag).argc == FOAM_NARY);

	argc   = foamArgc(foam);
	argf   = foamInfo(tag).argf;
	format = foamTagFormat(foam);
	tag    = FOAM_FORMAT_PUT(tag, format);

	BUF_PUT_BYTE(buf, tag);
	if (isNary) FOAM_PUT_INT(format, buf, argc);
	for (fi = si = 0; si < argc; fi++, si++) {
		int	n, af = argf[fi];
		if (af == '*') af = argf[--fi];

		switch (af) {
		case 't':
			n = foamArgv(foam)[si].data - FOAM_START;
			BUF_PUT_BYTE(buf, n);
			break;
		case 'o':
			n = foamArgv(foam)[si].data - FOAM_BVAL_START;
#if SMALL_BVAL_TAGS
			BUF_PUT_BYTE(buf, n);
#else
			BUF_PUT_HINT(buf, n);
#endif
			break;
		case 'p':
			n = foamArgv(foam)[si].data - FOAM_PROTO_START;
			BUF_PUT_BYTE(buf, n);
			break;
		case 'D':
		        n = foamArgv(foam)[si].data;
			BUF_PUT_BYTE(buf, n);
			break;
		case 'b':
			BUF_PUT_BYTE(buf, foamArgv(foam)[si].data);
			break;
		case 'h':
			BUF_PUT_HINT(buf, foamArgv(foam)[si].data);
			break;
		case 'w':
			n = foamArgv(foam)[si].data;
			if (isArr) {
				if (foam->foamArr.baseType == FOAM_Char)
					n = charToAscii(n);
			}
			BUF_PUT_SINT(buf, n);
			break;
		case 'X':
			offPos = bufPosition(buf);
			FOAM_PUT_INT(int0, buf, offPos);
			break;
		case 'F':
			n = foamArgv(foam)[si].data;
			FOAM_PUT_INT(int0, buf, n);
			labelFmt = FOAM_FORMAT_FOR(n);
			break;
		case 'L':
			FOAM_PUT_INT(labelFmt, buf, foamArgv(foam)[si].data);
			break;
		case 'i':
			FOAM_PUT_INT(format, buf, foamArgv(foam)[si].data);
			break;
		case 's': {
			String	s    = foamArgv(foam)[si].str;
			int	slen = strlen(s);
			FOAM_PUT_INT(format,buf,slen);
			bufWrChars(buf, slen, s);
			break;
		}
		case 'f':
			bufWrSFloat(buf, foamToSFlo(foam));
			si = argc;
			break;
		case 'd':
			bufWrDFloat(buf, foamToDFlo(foam));
			si = argc;
			break;
		case 'n': {
			int	slen;
			U16    *data;
			/*!! Should not store here. */
			bint=xintStore(bintCopy(foamArgv(foam)[si].bint));
			BUF_PUT_BYTE(buf,bint->isNeg);
			bintToPlacevS(bint, &slen, &data);
			FOAM_PUT_INT(format,buf,slen);
			for (bi = 0; bi < slen; bi++)
				BUF_PUT_HINT(buf,data[bi]);
			bintFree(bint);
			bintReleasePlacevS(data);
			break;
		}
		case 'C':
			foamToBuffer(buf, foamArgv(foam)[si].code);
			break;
		default:
			bugBadCase(af);
		}
	}

	if (foamTag(foam) == FOAM_Prog) {
		tmpPos = bufPosition(buf);
		bufPosition(buf) = offPos;
		FOAM_PUT_INT(int0, buf, tmpPos - offPos);
		if (tmpPos != 0) bufPosition(buf) = tmpPos;
	}

	return bufPosition(buf) - start;
}

void
foamPosToBuffer(Buffer buf, Foam foam)
{
	int	fi, si, tag, argc;
	String	argf;

	tag    = foamTag(foam);
	argc   = foamArgc(foam);
	argf   = foamInfo(tag).argf;

	if (tag == FOAM_Seq) {
		for (fi = si = 0; si < argc; fi++, si++)
			bufWrULong(buf, foamPos(foamArgv(foam)[si].code));
	}
	else {
		if (tag == FOAM_Prog)
			bufWrULong(buf, foamPos(foam));
		for (fi = si = 0; si < argc; fi++, si++) {
			int	af = argf[fi];
			if (af == '*') af = argf[--fi];

			switch (af) {
			  case 'C':
				foamPosToBuffer(buf, foamArgv(foam)[si].code);
				break;
			  default:
				break;
			}
		}
	}
}

void
foamPosBufPrint(FILE *file, Buffer buf)
{
	int	i, size, step;
	int	sposFlag = 0;
	SrcPos	sp;

	size = sizeof(SrcPos);
	step = buf->argc / size;

#if EDIT_1_0_n1_07
	fprintf(file, "Buffer length: %d, SrcPos size: %d\n", (int) buf->argc, size);
#else
	fprintf(file, "Buffer length: %d, SrcPos size: %d\n", buf->argc, size);
#endif
	fprintf(file, "Number of steps: %d\n", step);
	for (i = 0, buf->pos = 0; buf->pos < buf->argc; i++) {
		sp = bufRdULong(buf);
		if (sposLine(sp) != 0) {
			fprintf(file, "[%d]. ", i);
			if (sposFlag)
				sposPrint(file,	sp);
			else
				fprintf(file, "%lu", (ULong)sposLine(sp));
			fprintf(file, "\n");
		}
	}
}

/*
 * Skip over the foam tag in a buffer.
 */
local int
foamTagFrBuffer0(Buffer buf)
{
	FoamTag tag;
	BUF_GET_BYTE(buf, tag);
	return FOAM_FORMAT_GET(tag);
}

/*
 * Skip over a piece of foam in a buffer.
 */
local void
foamFrBuffer0(Buffer buf)
{
	int	fi, si, tag, argc, format;
	Bool	isNary;
	String	argf;

	BUF_GET_BYTE(buf, tag);
	format = FOAM_FORMAT_GET(tag);
	tag    = FOAM_FORMAT_REMOVE(tag, format);

	isNary = (foamInfo(tag).argc == FOAM_NARY);
	if (!isNary)
		argc = foamInfo(tag).argc;
	else
		FOAM_GET_INT(format, buf, argc);

	argf  = foamInfo(tag).argf;

	for (fi = si = 0; si < argc; fi++, si++) {
		int	af = argf[fi], n;
		if (af == '*') af = argf[--fi];
		switch (argf[fi]) {
		case 't':
#ifdef SMALL_BVAL_TAGS
		case 'o':
#endif
		case 'p':
		case 'D':
		case 'b':
			bufGetn(buf, BYTE_BYTES);
			break;
#ifndef SMALL_BVAL_TAGS
		case 'o':
#endif
		case 'h':
			bufGetn(buf, HINT_BYTES);
			break;
		case 'w':
			bufGetn(buf, SINT_BYTES);
			break;
		case 'X':
			FOAM_GET_INT(int0, buf, n);
			break;
		case 'F':
			FOAM_GET_INT(int0, buf, n);
			labelFmt = FOAM_FORMAT_FOR(n);
			break;
		case 'L':
			FOAM_GET_INT(labelFmt, buf, n);
			break;
		case 'i':
			FOAM_GET_INT(format, buf, n);
			break;
		case 's':
			FOAM_GET_INT(format, buf, n);
			bufGetn(buf, n);
			break;
		case 'f':
			bufGetn(buf, XSFLOAT_BYTES);
			return;
		case 'd':
			bufGetn(buf, XDFLOAT_BYTES);
			return;
		case 'n':
			bufGetn(buf, BYTE_BYTES);
			FOAM_GET_INT(format, buf, n);
			bufGetn(buf, n * HINT_BYTES);
			break;
		case 'C':
			foamFrBuffer0(buf);
			break;
		default:
			bugBadCase(argf[fi]);
		}
	}
	return;
}

/*
 * Read the foam formats from a foam unit in a buffer.
 */
Foam
foamFormatsFrBuffer(Buffer buf)
{
	BUF_START(buf);

	/* Read the foam unit tag. */
	foamTagFrBuffer0(buf);

	/* Read the formats. */
	return foamFrBuffer(buf);
}

/*
 * Read the constant number at position pos from a foam unit in a buffer.
 */
Foam
foamConstFrBuffer(Buffer buf, int pos)
{
	bufPosition(buf) = pos;
	return foamFrBuffer(buf);
}

/*
 * Read the number of constants from a foam unit in a buffer.
 */
Length
foamConstcFrBuffer(Buffer buf)
{
	Length	i, argc, format;

	BUF_START(buf);

	/* Read the foam unit tag. */
	foamTagFrBuffer0(buf);

	/* Read the foam dfmt tag. */
	format = foamTagFrBuffer0(buf);

	/* Read the number of ddecls. */
	FOAM_GET_INT(format, buf, argc);

	/* Skip to the predefined constsSlot. */
	for (i = 0; i < constsSlot; i += 1)
		foamFrBuffer0(buf);

	/* Read the foam ddecl tag. */
	format = foamTagFrBuffer0(buf);

	/* Read the type field */
	
	/* Read the number of foam constants. */
	FOAM_GET_INT(format, buf, argc);
	
	return argc-foamNaryStart(FOAM_DDecl);
}

/*
 * Compute the positions of the constants from a foam unit in a buffer.
 */
void
foamConstvFrBuffer(Buffer buf, Length posc, int *posv)
{
	Length	i, j, c, argc, format;

	for (i = 0; i < posc; i += 1)
		posv[i] = 0;

	BUF_START(buf);

	/* Read the foam unit tag. */
	foamTagFrBuffer0(buf);

	/* Skip over the formats. */
	foamFrBuffer0(buf);

	/* Read the foam ddef tag. */
	format = foamTagFrBuffer0(buf);

	/* Read the number of foam definitions. */
	FOAM_GET_INT(format, buf, argc);

	/* Compute the positions of the foam constants. */
	for (i = 0, c = 0; i < argc; i += 1) {
		FoamTag tag;

		/* Read the foam def tag. */
		foamTagFrBuffer0(buf);

		/* Read the foam const/global tag. */
		BUF_GET_BYTE(buf, tag);
		format = FOAM_FORMAT_GET(tag);
		tag    = FOAM_FORMAT_REMOVE(tag, format);

		/* Read the constant/global number. */
		FOAM_GET_INT(format, buf, j);

		/* If we have a constant, store its position. */
		if (tag == FOAM_Const) {
			assert(posv[j] == 0);
			posv[j] = bufPosition(buf);
			c += 1;
		}

		/* Skip over the rest of the foam definition. */
		foamFrBuffer0(buf);
	}
	assert(c == posc);

	return;
}

/*
 * Fill the vector with the constants from the foam unit.
 */
void
foamConstvFrFoam(Foam unit, Length argc, Foam *argv)
{
	Length	defc = foamArgc(unit->foamUnit.defs);
	Length	i, j, c;

	for (i = 0; i < argc; i += 1)
		argv[i] = 0;

	for (i = 0, c = 0; i < defc; i += 1) {
		Foam	def = foamArgv(unit->foamUnit.defs)[i].code;
		Foam	lhs = def->foamDef.lhs;
		if (foamTag(lhs) == FOAM_Const) {
			j = lhs->foamConst.index;
			assert(argv[j] == 0);
			argv[j] = def->foamDef.rhs;
			c += 1;
		}
	}
	assert(c == argc);
}


/* Return NULL if is not a Prog */
Foam
foamGetProgHdrFrBuffer(Buffer buf, int pos)
{
	Foam prog;
	bufPosition(buf) = pos;
	prog = foamProgHdrFrBuffer(buf);

	if (prog) {
		prog->foamProg.locals = NULL;
		prog->foamProg.params = NULL;
		prog->foamProg.fluids = NULL;
		prog->foamProg.levels = NULL;
		prog->foamProg.body = NULL;
	}

	return prog;
}

/*
 * Determine the tag format for compact linear output.
 */
local int
foamTagFormat(Foam foam)
{
	int	tag = foamTag(foam), format, isNary, argc;
	int	ix[3], x1, x2, i, si, di, ng1, ng2;

	isNary = (foamInfo(tag).argc == FOAM_NARY);
	argc   = foamArgc(foam);
	if (tag < FOAM_INDEX_START) {
		if (tag < FFO_ORIGIN)
			format = 0;
		else {
			if	(tag == FOAM_Unimp)
				si = strlen(foamArgv(foam)[0].str);
			else if (tag == FOAM_Decl || tag == FOAM_GDecl)
			{
				/*
				 * We are storing a compressible integer
				 * and a string in this chunk so we need
				 * to be careful about our format number.
				 */
				si = strlen(foamArgv(foam)[1].str);
				di = foamArgv(foam)[3].data;
				if (di > si) si = di;
			}
			else if (tag == FOAM_BInt) {
				/* !! Should not store here. */
				BInt	bint;
				bint= xintStore(bintCopy(foamArgv(foam)[0].bint));
				si  = bint->placec;
				bintFree(bint);
			}
			else {
				NotReached(si = 0);
			}
			format = FOAM_FORMAT_FOR(si);
		}
	}
	else if (tag == FOAM_Rec ||
		 tag == FOAM_DEnv ||
		 tag == FOAM_DFluid) {
		si = argc;
		format = FOAM_FORMAT_FOR(si);
		for (i = 0; i<argc ; i++) {
			int fi;
			si = foamArgv(foam)[i].data;
			fi = FOAM_FORMAT_FOR(si);
			if (fi < format) format = fi;
			if (format == 0) break;
		}
	}
	else if (tag < FOAM_INDEX_LIMIT || isNary) {
		si = isNary ? argc : foamArgv(foam)[0].data;

		/*
		 * !! HACK. The first test is here due to a bug discovered
		 * after the freeze of v. 0.37 (EInfo is in the wrong position
		 * in foamTag enumeration). Without this hack we should change
		 * the format of .ao files.
		 *
		 * During development of 1.1.13, new instructions were added
		 * and EInfo put in the correct place. This check is only
		 * here temporarily until we are sure that the problem has
		 * gone away.
		 */
		if (tag == FOAM_EInfo)
		{
			bug("Arrgghh! The EInfo bug is back!");
			format = 0;
		}
		else if (si < IMMED_FORMS)
			format = STD_FORMS + si;
		else
			format = FOAM_FORMAT_FOR(si);
	}
	else {
		switch (tag) {
		case FOAM_Lex:		x1 = 1; x2 = -1; break;
		case FOAM_RElt:		x1 = 2; x2 = -1; break;
		case FOAM_RRElt:	x1 = 0; x2 = -1; break;
		case FOAM_EElt:		x1 = 2; x2 =  3; break;
		case FOAM_IRElt:	x1 = 2; x2 = -1; break;
		case FOAM_TRElt:	x1 = 3; x2 = -1; break;
		default:
			bugBadCase(tag);
			NotReached(x1 = x2 = 0);
		}
		ix[0] = foamArgv(foam)[0].data;
		ix[1] =			    foamArgv(foam)[x1].data;
		ix[2] = (x2 == -1) ? 0	  : foamArgv(foam)[x2].data;

		for (ng1 = ng2 = 0, i = 0;  i < 3; i++) {
			if ((long) ix[i] > MAX_HINT) { ng2++; }
			if ((long) ix[i] > MAX_BYTE) { ng1++; }
		}
		/* We only care if any are > MAX_BYTE ... */
		if	(ng1 > 0)  format = 0;
		else		   format = 1;
	}
	return format;
}

local Foam
foamGetDecl(int index, Foam ddecl, FoamBox fbox)
{
	assert(ddecl == NULL || foamTag(ddecl) == FOAM_DDecl);
	if (ddecl && index < foamDDeclArgc(ddecl))
		return ddecl->foamDDecl.argv[index];
	else {
		assert (fbox);
		return fboxNth(fbox, index);
	}
}

local Foam
foamGetDDecl(int index, Foam fmts, FoamBox fbox)
{
	assert(fmts == NULL || foamTag(fmts) == FOAM_DFmt);
	if (fmts && index < foamArgc(fmts))
		return fmts->foamDFmt.argv[index];
	else {
		assert (fbox);
		return fboxNth(fbox, index);
	}
}

/*
 * Given an expression occuring in a foam prog and given the DFmt section for
 * the unit where it is defined, return the foam type of the expression
 *
 * If the EXTRA parameter is not NULL, then extra type information is
 * eventually stored in it. In example, if the type is FOAM_Rec, in EXTRA is
 * returned format number.
 * Extra type information:
 *      Record 		->	format number
 *	Array		->	type of elements
 */
FoamTag
foamExprType0(Foam expr, Foam prog, Foam formats, FoamBox locals,
	     FoamBox formatBox, AInt * extra)
{
	FoamTag		type;
	Foam		decl;

	if (extra) *extra = emptyFormatSlot;
	switch (foamTag(expr)) {
	  case FOAM_Nil:
	  case FOAM_Char:
	  case FOAM_Bool:
	  case FOAM_Byte:
	  case FOAM_HInt:
	  case FOAM_SInt:
	  case FOAM_SFlo:
	  case FOAM_DFlo:
	  case FOAM_Word:
	  case FOAM_Arb:
	  case FOAM_Ptr:
	  case FOAM_Clos:
	  case FOAM_BInt:
	  case FOAM_Env:
	  case FOAM_RRec:
	  case FOAM_Prog:
		return foamTag(expr);

	  case FOAM_Rec:
		if (extra) *extra = expr->foamRec.format;
		return foamTag(expr);

	  case FOAM_Arr:
		if (extra) *extra = expr->foamArr.baseType;
		return foamTag(expr);

	  case FOAM_BVal:
		return foamBValInfo(expr->foamBVal.builtinTag).retType;
	  case FOAM_CProg:
		return FOAM_Prog;
	  case FOAM_CEnv:
		return FOAM_Env;
	  case FOAM_Cast:
		return expr->foamCast.type;
	  case FOAM_ANew:
		if (extra) *extra = expr->foamANew.eltType;
		return FOAM_Arr;

	  case FOAM_Set:
		return foamExprType(expr->foamSet.rhs, prog, formats, locals,
				    formatBox, extra);
	  case FOAM_Def:
		return foamExprType(expr->foamDef.rhs, prog, formats, locals,
				    formatBox, extra);
	  case FOAM_AElt:
		return expr->foamAElt.baseType;
	  case FOAM_Par:
		assert (prog != 0);
		assert (foamTag(prog) == FOAM_Prog);
#ifdef NEW_FORMATS
		decl = formats->foamDFmt.argv[paramsSlot]->foamDDecl.argv[prog->foamProg.params-1];
#else

		decl = prog->foamProg.params->
			foamDDecl.argv[expr->foamPar.index];
#endif
		type = decl->foamDecl.type;

		if (extra && 
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;
		return type;
	  case FOAM_Loc: {
		int	index = expr->foamLoc.index;

		assert (prog != 0);
		assert (foamTag(prog) == FOAM_Prog);

		decl = foamGetDecl(index, prog->foamProg.locals, locals);
		type = (decl->foamDecl.type) & 0xFF;

		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;

		return type;
		}
	  case FOAM_Glo:
		decl = formats->foamDFmt.argv[globalsSlot]->
			foamDDecl.argv[expr->foamGlo.index];
		type = decl->foamGDecl.type;

		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamGDecl.format;

		return type;

	  case FOAM_Fluid:
		decl = formats->foamDFmt.argv[fluidsSlot]->
			foamDDecl.argv[expr->foamFluid.index];
		type = decl->foamDecl.type;

		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;

		return type;

	  case FOAM_Const:
		return formats->foamDFmt.argv[constsSlot]->
			foamDDecl.argv[expr->foamGlo.index]->foamDecl.type;
	  case FOAM_RNew:
		if (extra) *extra = expr->foamRNew.format;
		return FOAM_Rec;

	  case FOAM_RRNew:
		return FOAM_RRec;

	  case FOAM_RRFmt:
		return FOAM_Word;

	  case FOAM_TRNew:
		if (extra) *extra = expr->foamTRNew.format;
		return FOAM_TR;

	  case FOAM_RRElt:
		return FOAM_Word;

	  case FOAM_Lex: {
		Foam 	ddecl;
		int 	index;
		assert (prog != 0);
		assert (foamTag(prog) == FOAM_Prog);
		
		index = prog->foamProg.levels->foamDEnv.argv[expr->foamLex.level];
		ddecl = foamGetDDecl(index, formats, formatBox);
		decl = ddecl->foamDDecl.argv[expr->foamLex.index];
		type = decl->foamDecl.type;

		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;
	
		return type;
		}
	  case FOAM_RElt: {
		int	index = expr->foamRElt.format;
		Foam	ddecl = foamGetDDecl(index, formats, formatBox);
		decl = ddecl->foamDDecl.argv[expr->foamRElt.field];
		type = decl->foamDecl.type;

		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;
	
		return type;
	  }
	  case FOAM_IRElt: {
		int	index = expr->foamIRElt.format;
		Foam	ddecl = foamGetDDecl(index, formats, formatBox);
		decl = foamTRDDeclIDecl(ddecl, expr->foamIRElt.field); /* use foamIRDeclIdx() */
		type = decl->foamDecl.type;

		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;
	
		return type;
	  }

	  case FOAM_TRElt: {
		int	index = expr->foamIRElt.format;
		Foam	ddecl = foamGetDDecl(index, formats, formatBox);
		decl = foamTRDDeclTDecl(ddecl, expr->foamTRElt.field); /* use foamTRDeclIdx() */
		type = decl->foamDecl.type;

		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;
	
		return type;
	  }
	  case FOAM_EElt: {
		int	index = expr->foamEElt.env;
		Foam	ddecl = foamGetDDecl(index, formats, formatBox);
		decl = ddecl->foamDDecl.argv[expr->foamEElt.lex];
		type = decl->foamDecl.type;
		
		if (extra &&
		    (type == FOAM_Rec || type == FOAM_Arr || type == FOAM_TR))
			*extra = decl->foamDecl.format;
	
		return type;
	  }
	case FOAM_PCall:
		return expr->foamPCall.type;
	case FOAM_BCall:
		return foamBValInfo(expr->foamBCall.op).retType;
	case FOAM_CCall:
		return expr->foamCCall.type;
	case FOAM_OCall:
		return expr->foamOCall.type;
	case FOAM_PRef:
		return FOAM_SInt;
	case FOAM_MFmt:
		if (extra) *extra = expr->foamMFmt.format;
		return FOAM_Rec;
	case FOAM_EInfo:
		return FOAM_Word;
	case FOAM_PushEnv:
		return FOAM_Env;
	default:
		bugWarning("foamExprType0: type %s unhandled. Returning 0", foamInfo(foamTag(expr)).str);
	}

	bug("foamExprType0: reached end of code.");

	return 0;   /* quit warnings */

}

FoamTag
foamExprType(Foam expr, Foam prog, Foam formats, FoamBox locals,
	     FoamBox formatBox, AInt * extra)
{
	FoamTag tag;
	AInt    foo;
	tag = foamExprType0(expr, prog, formats, locals, formatBox, &foo);	
	
#if 0  /* Need to check against FOAM_CCall as well as FOAM_Cast, plus others */
	assert( foamTag(expr) == FOAM_Cast || tag != FOAM_Rec || foo != emptyFormatSlot);
#endif
	if (extra) *extra = foo;
	return tag;
}	

Bool
foamHasSideEffect(Foam foam)

{
	Bool	t;

	switch(foamTag(foam)) {
	  case FOAM_Set:
	  case FOAM_Def:
	  case FOAM_PCall:
	  case FOAM_OCall:
	  case FOAM_CCall:
		if (!foamPure(foam))
			return true;
		break;
	  case FOAM_BCall:
		if (foamBValInfo(foam->foamBCall.op).hasSideFx)
			return true;
		break;
	  case FOAM_Catch:
		return true;
	  case FOAM_Prog:
		return false;
	  case FOAM_EEnsure:
		return true;
	  default:
		break;
	}
	if (foamTag(foam) == FOAM_Prog) return false;

	foamIter(foam, arg, {t = foamHasSideEffect(*arg);
			     if (t) return true;});
	return false;
}

Bool
foamIsControlFlow(Foam foam)
{
	switch(foamTag(foam)) {
	  case FOAM_Return:
	  case FOAM_Label:
	  case FOAM_Goto:
	  case FOAM_If:
	  case FOAM_Select:
	  case FOAM_Loose:
	  case FOAM_Kill:
	  case FOAM_Free:
	  case FOAM_Throw:
	  case FOAM_Catch:
	  case FOAM_Seq:
		return true;
	  default:
		return false;
	}
}

/*****************************************************************************
 *
 * :: Table of FOAM instruction codes
 *
 ****************************************************************************/

/*
 * Meanings of the bytes in the argf field:
 *
 * t = AInt as a foam type tag.
 * o = AInt as a foam builtin tag.
 * p = AInt as a foam protocol tag.
 * D = AInt as a foam DDecl tag.
 * b = AInt as a byte.
 * h = AInt as a half-int.
 * w = AInt as a single-int.
 * i = AInt as a usually small index (to be compressed).
 * L = AInt as a usually small label (to be compressed).
 * X = AInt length of byte-coded tree rooted at this node (to be compressed).
 * F = AInt as a byte, indicating format of all labels in prog.
 * s = String.
 * f = Single float.
 * d = Double float.
 * n = Big integer.
 * C = foam code.
 * ! = Arbitrary value (cannot be written to a file).
 */

struct foam_info foamInfoTable[] = {
/* tag        sxsym    str         argc       argf */
 {FOAM_Nil,	    0,"Nil",          0,        ""},
 {FOAM_Char,	    0,"Char",         1,        "b"},
 {FOAM_Bool,	    0,"Bool",         1,        "b"},
 {FOAM_Byte,	    0,"Byte",         1,        "b"},
 {FOAM_HInt,	    0,"HInt",         1,        "h"},
 {FOAM_SInt,	    0,"SInt",         1,        "w"},
 {FOAM_SFlo,	    0,"SFlo",         1,        "f"},
 {FOAM_DFlo,	    0,"DFlo",         1,        "d"},
 {FOAM_Word,	    0,"Word",         1,        "w"},
 {FOAM_Arb,	    0,"Arb",          1,        "!"},

 {FOAM_Int8,	    0,"Int8",         1,        "b"},
 {FOAM_Int16,	    0,"Int16",        1,        "bb"},
 {FOAM_Int32,	    0,"Int32",        1,        "bbbb"},
 {FOAM_Int64,	    0,"Int64",        1,        "bbbbbbbb"},
 {FOAM_Int128,	    0,"Int128",       1,        "bbbbbbbbbbbbbbbb"},

 {FOAM_NOp,	    0,"NOp",          0,        ""},
 {FOAM_BVal,	    0,"BVal",         1,        "o"},
 {FOAM_Ptr,	    0,"Ptr",          1,        "C"},
 {FOAM_CProg,	    0,"CProg",        1,        "C"},
 {FOAM_CEnv,	    0,"CEnv",         1,        "C"},
 {FOAM_Loose,	    0,"Loose",        1,        "C"},
 {FOAM_EEnsure,	    0,"EEnsure",      1,        "C"},
 {FOAM_EInfo,	    0,"EInfo",	      1,	"C"},
 {FOAM_Kill,	    0,"Kill",         1,        "C"},
 {FOAM_Free,	    0,"Free",         1,        "C"},
 {FOAM_Return,	    0,"Return",       1,        "C"},
 {FOAM_Cast,	    0,"Cast",         2,        "tC"},
 {FOAM_ANew,	    0,"ANew",         2,        "tC"},
 {FOAM_RRNew,	    0,"RRNew",        2,        "iC"},
 {FOAM_RRec,	    0,"RRec",         2,        "CC"},
 {FOAM_Clos,	    0,"Clos",         2,        "CC"},
 {FOAM_Set,	    0,"Set",          2,        "CC"},
 {FOAM_Def,	    0,"Def",          2,        "CC"},
 {FOAM_AElt,	    0,"AElt",         3,        "tCC"},
 {FOAM_If,	    0,"If",           2,        "CL"},
 {FOAM_Goto,	    0,"Goto",         1,        "L"},
 {FOAM_Throw,	    0,"Throw",        2,        "CC"},
 {FOAM_Catch,	    0,"Catch",        2,        "CC"},
 {FOAM_Protect,     0,"Protect",      2,        "CCC"},
 {FOAM_Unit,	    0,"Unit",         2,        "CC"},
 {FOAM_PushEnv,	    0,"PushEnv",      2,        "iC"},
 {FOAM_PopEnv,	    0,"PopEnv",       0,        ""},
 {FOAM_MFmt,	    0,"MFmt",         2,        "iC"},
 {FOAM_RRFmt,	    0,"RRFmt",        1,        "C"},

/* ========> FFO_ORIGIN (start of multi-format instructions) <======== */

 {FOAM_Unimp,	    0,"Unimp",        1,        "s"},
 {FOAM_GDecl,	    0,"GDecl",        6,        "tswibp"},
 {FOAM_Decl,	    0,"Decl",         4,        "tswi"},
 {FOAM_BInt,	    0,"BInt",         1,        "n"},

 {FOAM_Par,	    0,"Par",          1,        "i"},
 {FOAM_Loc,	    0,"Loc",          1,        "i"},
 {FOAM_Glo,	    0,"Glo",          1,        "i"},
 {FOAM_Fluid,	    0,"Fluid",        1,        "i"},
 {FOAM_Const,	    0,"Const",        1,        "i"},
 {FOAM_Env,	    0,"Env",          1,        "i"},
 {FOAM_EEnv,	    0,"EEnv",         2,        "iC"},
 {FOAM_RNew,	    0,"RNew",         1,        "i"},
 {FOAM_PRef,	    0,"PRef",	      2,	"iC"},
 {FOAM_TRNew,	    0,"TRNew",        2,        "iC"},
 {FOAM_RRElt,       0,"RRElt",        3,        "iCC"},
 {FOAM_Label,	    0,"Label",        1,        "i"},

 {FOAM_Lex,	    0,"Lex",          2,        "ii"},
 {FOAM_RElt,	    0,"RElt",         3,        "iCi"},
 {FOAM_IRElt,	    0,"IRElt",        3,        "iCi"},
 {FOAM_TRElt,	    0,"TRElt",        4,        "iCCi"},
 {FOAM_EElt,	    0,"EElt",         4,        "iCii"},
 {FOAM_CFCall,	    0,"CFCall",       4,        "iiCC"},
 {FOAM_OFCall,	    0,"OFCall",       4,        "iiCCC"},

 {FOAM_DDecl,	    0,"DDecl",        FOAM_NARY, "DC*"},
 {FOAM_DFluid,	    0,"DFluid",       FOAM_NARY, "i*"},
 {FOAM_DEnv,	    0,"DEnv",         FOAM_NARY, "i*"},
 {FOAM_DDef,	    0,"DDef",         FOAM_NARY, "C*"},
 {FOAM_DFmt,	    0,"DFmt",         FOAM_NARY, "C*"},
 {FOAM_Rec,	    0,"Rec",          FOAM_NARY, "iC*"},
 {FOAM_Arr,	    0,"Arr",          FOAM_NARY, "tw*"},
 {FOAM_TR,	    0,"TR",           FOAM_NARY, "iC*"},
 {FOAM_Select,	    0,"Select",       FOAM_NARY, "CL*"},
 {FOAM_PCall,	    0,"PCall",        FOAM_NARY, "ptC*"},
 {FOAM_BCall,	    0,"BCall",        FOAM_NARY, "oC*"},
 {FOAM_CCall,	    0,"CCall",        FOAM_NARY, "tCC*"},
 {FOAM_OCall,	    0,"OCall",        FOAM_NARY, "tCCC*"},
 {FOAM_Seq,	    0,"Seq",          FOAM_NARY, "C*"},
 {FOAM_Values,	    0,"Values",       FOAM_NARY, "C*"},
#ifdef NEW_FORMATS
 {FOAM_Prog,	    0,"Prog",         FOAM_NARY, "XFtiwwwwC*"}
#else
 {FOAM_Prog,	    0,"Prog",         FOAM_NARY, "XFtiwwwwC*"}
#endif
};

/*****************************************************************************
 *
 * :: Table of FOAM protcols
 *
 ****************************************************************************/

struct foamProto_info foamProtoInfoTable[] = {
 {FOAM_Proto_Foam,   0,"Foam"},
 {FOAM_Proto_Fortran,0,"Fortran"},
 {FOAM_Proto_C,	     0,"C"},
 {FOAM_Proto_Lisp,   0,"Lisp"},
 {FOAM_Proto_Init,   0,"Init"},
 {FOAM_Proto_Include,0,"Include"},
 {FOAM_Proto_Other,  0,"Other"}
};

/*****************************************************************************
 *
 * :: Table of FOAM decl information
 *
 ****************************************************************************/

struct foamDDecl_info	foamDDeclInfoTable[] = {
 { FOAM_DDecl_LocalEnv, 	0, "LocalEnv" },
 { FOAM_DDecl_NonLocalEnv, 	0, "NonLocalEnv" },
 { FOAM_DDecl_Param, 		0, "Params" },
 { FOAM_DDecl_Local,  		0, "Locals" },  
 { FOAM_DDecl_Fluid,  		0, "Fluids" },
 { FOAM_DDecl_Multi,  		0, "Multis" },  
 { FOAM_DDecl_Union,  		0, "Unions" },  
 { FOAM_DDecl_Record, 		0, "Records" }, 
 { FOAM_DDecl_TrailingArray,	0, "TrailingArray" }, 
 { FOAM_DDecl_Consts, 		0, "Consts" }, 
 { FOAM_DDecl_Global, 		0, "Globals" },  
 { FOAM_DDecl_FortranSig,       0, "FortranSig" },
 { FOAM_DDecl_CSig,             0, "CSig" }
};

/*****************************************************************************
 *
 * :: Table of FOAM builtin operations
 *
 ****************************************************************************/

struct foamBVal_info foamBValInfoTable[] = {
 {FOAM_BVal_BoolFalse,	 0,"BoolFalse",
			 0,0,{0},			     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BoolTrue,	 0,"BoolTrue",
			 0,0,{0},			     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BoolNot,	 0,"BoolNot",
			 0,1,{FOAM_Bool},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BoolAnd,	 0,"BoolAnd",
			 0,2,{FOAM_Bool, FOAM_Bool},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BoolOr,	 0,"BoolOr",
			 0,2,{FOAM_Bool, FOAM_Bool},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BoolEQ,	 0,"BoolEQ",
			 0,2,{FOAM_Bool, FOAM_Bool},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BoolNE,	 0,"BoolNE",
			 0,2,{FOAM_Bool, FOAM_Bool},	     FOAM_Bool, 1, {0}},

 {FOAM_BVal_CharSpace,	 0,"CharSpace",
			 0,0,{0},			     FOAM_Char, 1, {0}},
 {FOAM_BVal_CharNewline, 0,"CharNewline",
			 0,0,{0},			     FOAM_Char, 1, {0}},
 {FOAM_BVal_CharTab,     0,"CharTab",
			 0,0,{0},			     FOAM_Char, 1, {0}},
 {FOAM_BVal_CharMin,	 0,"CharMin",
			 0,0,{0},			     FOAM_Char, 1, {0}},
 {FOAM_BVal_CharMax,	 0,"CharMax",
			 0,0,{0},			     FOAM_Char, 1, {0}},
 {FOAM_BVal_CharIsDigit, 0,"CharIsDigit",
			 0,1,{FOAM_Char},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_CharIsLetter,0,"CharIsLetter",
			 0,1,{FOAM_Char},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_CharEQ,	 0,"CharEQ",
			 0,2,{FOAM_Char,FOAM_Char},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_CharNE,	 0,"CharNE",
			 0,2,{FOAM_Char,FOAM_Char},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_CharLT,	 0,"CharLT",
			 0,2,{FOAM_Char,FOAM_Char},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_CharLE,	 0,"CharLE",
			 0,2,{FOAM_Char,FOAM_Char},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_CharLower,	 0,"CharLower",
			 0,1,{FOAM_Char},		     FOAM_Char, 1, {0}},
 {FOAM_BVal_CharUpper,	 0,"CharUpper",
			 0,1,{FOAM_Char},		     FOAM_Char, 1, {0}},
 {FOAM_BVal_CharOrd,	 0,"CharOrd",
			 0,1,{FOAM_Char},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_CharNum,	 0,"CharNum",
			 0,1,{FOAM_SInt},		     FOAM_Char, 1, {0}},

  /* Floating point system properties are omitted, but need to be returned. */

 {FOAM_BVal_SFlo0,	 0,"SFlo0",
			 0,0,{0},			     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFlo1,	 0,"SFlo1",
			 0,0,{0},			     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloMin,	 0,"SFloMin",
			 0,0,{0},			     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloMax,	 0,"SFloMax",
			 0,0,{0},			     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloEpsilon, 0,"SFloEpsilon",
			 0,0,{0},			     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloIsZero,	 0,"SFloIsZero",
			 0,1,{FOAM_SFlo},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SFloIsNeg,	 0,"SFloIsNeg",
			 0,1,{FOAM_SFlo},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SFloIsPos,	 0,"SFloIsPos",
			 0,1,{FOAM_SFlo},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SFloEQ,	 0,"SFloEQ",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SFloNE,	 0,"SFloNE",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SFloLT,	 0,"SFloLT",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SFloLE,	 0,"SFloLE",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SFloNegate,	 0,"SFloNegate",
			 0,1,{FOAM_SFlo},		     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloPrev,	 0,"SFloPrev",
			 0,1,{FOAM_SFlo},		     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloNext,	 0,"SFloNext",
			 0,1,{FOAM_SFlo},		     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloPlus,	 0,"SFloPlus",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloMinus,	 0,"SFloMinus",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloTimes,	 0,"SFloTimes",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloTimesPlus, 0,"SFloTimesPlus",
			 0,3,{FOAM_SFlo,FOAM_SFlo,FOAM_SFlo},FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloDivide,	 0,"SFloDivide",
			 0,2,{FOAM_SFlo,FOAM_SFlo},	     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloRPlus,         0,"SFloRPlus",
                       0,3,{FOAM_SFlo,FOAM_SFlo,FOAM_SInt},  FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloRMinus,        0,"SFloRMinus",
                       0,3,{FOAM_SFlo,FOAM_SFlo,FOAM_SInt},  FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloRTimes,        0,"SFloRTimes",
                       0,3,{FOAM_SFlo,FOAM_SFlo,FOAM_SInt},  FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloRTimesPlus, 0,"SFloRTimesPlus",
                       0,4,{FOAM_SFlo,FOAM_SFlo,FOAM_SFlo,FOAM_SInt},
                                                             FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloRDivide, 0,"SFloRDivide",
                       0,3,{FOAM_SFlo,FOAM_SFlo,FOAM_SInt},  FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloDissemble,0,"SFloDissemble",
                       0,1,{FOAM_SFlo},                      FOAM_NOp,  3, 
                           {FOAM_Bool,FOAM_SInt,FOAM_Word}},
 {FOAM_BVal_SFloAssemble,0,"SFloAssemble",
                       0,3,  {FOAM_Bool,FOAM_SInt,FOAM_Word},FOAM_SFlo, 1, {0}},


 {FOAM_BVal_DFlo0,	 0,"DFlo0",
			 0,0,{0},			     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFlo1,	 0,"DFlo1",
			 0,0,{0},			     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloMin,	 0,"DFloMin",
			 0,0,{0},			     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloMax,	 0,"DFloMax",
			 0,0,{0},			     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloEpsilon, 0,"DFloEpsilon",
			 0,0,{0},			     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloIsZero,	 0,"DFloIsZero",
			 0,1,{FOAM_DFlo},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_DFloIsNeg,	 0,"DFloIsNeg",
			 0,1,{FOAM_DFlo},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_DFloIsPos,	 0,"DFloIsPos",
			 0,1,{FOAM_DFlo},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_DFloEQ,	 0,"DFloEQ",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_DFloNE,	 0,"DFloNE",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_DFloLT,	 0,"DFloLT",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_DFloLE,	 0,"DFloLE",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_DFloNegate,	 0,"DFloNegate",
			 0,1,{FOAM_DFlo},		     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloPrev,	 0,"DFloPrev",
			 0,1,{FOAM_DFlo},		     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloNext,	 0,"DFloNext",
			 0,1,{FOAM_DFlo},		     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloPlus,	 0,"DFloPlus",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloMinus,	 0,"DFloMinus",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloTimes,	 0,"DFloTimes",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloTimesPlus, 0,"DFloTimesPlus",
			 0,3,{FOAM_DFlo,FOAM_DFlo,FOAM_DFlo},FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloDivide,	 0,"DFloDivide",
			 0,2,{FOAM_DFlo,FOAM_DFlo},	     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloRPlus,   0,"DFloRPlus",
                         0,3,{FOAM_DFlo,FOAM_DFlo,FOAM_SInt},FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloRMinus,  0,"DFloRMinus",
                         0,3,{FOAM_DFlo,FOAM_DFlo,FOAM_SInt},FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloRTimes,  0,"DFloRTimes",
                         0,3,{FOAM_DFlo,FOAM_DFlo,FOAM_SInt},FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloRTimesPlus, 0,"DFloRTimesPlus",
                         0,4,{FOAM_DFlo,FOAM_DFlo,FOAM_DFlo,FOAM_SInt},
                                                             FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloRDivide, 0,"DFloRDivide",
                       0,3,{FOAM_DFlo,FOAM_DFlo,FOAM_SInt},  FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloDissemble,0,"DFloDissemble",
                       0,1,{FOAM_DFlo},                      FOAM_NOp,  4, 
                           {FOAM_Bool,FOAM_SInt,FOAM_Word,FOAM_Word}},
 {FOAM_BVal_DFloAssemble,0,"DFloAssemble",
                       0,4,{FOAM_Bool,FOAM_SInt,FOAM_Word,FOAM_Word},
#if EDIT_1_0_n2_04 
                                                             FOAM_DFlo, 1, {0}},
#else
                                                             FOAM_SFlo, 1, {0}},
#endif


 {FOAM_BVal_Byte0,	 0,"Byte0",
			 0,0,{0},			     FOAM_Byte, 1, {0}},
 {FOAM_BVal_Byte1,	 0,"Byte1",
			 0,0,{0},			     FOAM_Byte, 1, {0}},
 {FOAM_BVal_ByteMin,	 0,"ByteMin",
			 0,0,{0},			     FOAM_Byte, 1, {0}},
 {FOAM_BVal_ByteMax,	 0,"ByteMax",
			 0,0,{0},			     FOAM_Byte, 1, {0}},

 {FOAM_BVal_HInt0,	 0,"HInt0",
			 0,0,{0},			     FOAM_HInt, 1, {0}},
 {FOAM_BVal_HInt1,	 0,"HInt1",
			 0,0,{0},			     FOAM_HInt, 1, {0}},
 {FOAM_BVal_HIntMin,	 0,"HIntMin",
			 0,0,{0},			     FOAM_HInt, 1, {0}},
 {FOAM_BVal_HIntMax,	 0,"HIntMax",
			 0,0,{0},			     FOAM_HInt, 1, {0}},

 {FOAM_BVal_SInt0,	 0,"SInt0",
			 0,0,{0},			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SInt1,	 0,"SInt1",
			 0,0,{0},			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntMin,	 0,"SIntMin",
			 0,0,{0},			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntMax,	 0,"SIntMax",
			 0,0,{0},			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntIsZero,	 0,"SIntIsZero",
			 0,1,{FOAM_SInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntIsNeg,	 0,"SIntIsNeg",
			 0,1,{FOAM_SInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntIsPos,	 0,"SIntIsPos",
			 0,1,{FOAM_SInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntIsEven,	 0,"SIntIsEven",
			 0,1,{FOAM_SInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntIsOdd,	 0,"SIntIsOdd",
			 0,1,{FOAM_SInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntEQ,	 0,"SIntEQ",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntNE,	 0,"SIntNE",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntLT,	 0,"SIntLT",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntLE,	 0,"SIntLE",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntNegate,	 0,"SIntNegate",
			 0,1,{FOAM_SInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntPrev,	 0,"SIntPrev",
			 0,1,{FOAM_SInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntNext,	 0,"SIntNext",
			 0,1,{FOAM_SInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntPlus,	 0,"SIntPlus",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntMinus,	 0,"SIntMinus",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntTimes,	 0,"SIntTimes",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntTimesPlus, 0,"SIntTimesPlus",
			 0,3,{FOAM_SInt,FOAM_SInt,FOAM_SInt},FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntMod,	 0,"SIntMod",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntQuo,	 0,"SIntQuo",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntRem,	 0,"SIntRem",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntDivide,	 0,"SIntDivide",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_NOp,
			 2, {FOAM_SInt, FOAM_SInt}},
 {FOAM_BVal_SIntGcd,	 0,"SIntGcd",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntPlusMod, 0,"SIntPlusMod",
			 0,3,{FOAM_SInt,FOAM_SInt,FOAM_SInt},FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntMinusMod,0,"SIntMinusMod",
			 0,3,{FOAM_SInt,FOAM_SInt,FOAM_SInt},FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntTimesMod,0,"SIntTimesMod",
			 0,3,{FOAM_SInt,FOAM_SInt,FOAM_SInt},FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntTimesModInv,0,"SIntTimesModInv",
			 0,4,{FOAM_SInt,FOAM_SInt,FOAM_SInt,FOAM_DFlo},
                                                             FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntLength,	 0,"SIntLength",
			 0,1,{FOAM_SInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntShiftUp, 0,"SIntShiftUp",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntShiftDn, 0,"SIntShiftDn",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntBit,	 0,"SIntBit",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_SIntNot,	 0,"SIntNot",
			 0,1,{FOAM_SInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntAnd,	 0,"SIntAnd",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntOr,	 0,"SIntOr",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntXOr,	 0,"SIntXOr",
			 0,2,{FOAM_SInt,FOAM_SInt},	     FOAM_SInt, 1, {0}},

 {FOAM_BVal_WordTimesDouble,0,"WordTimesDouble",
                         0,2,{FOAM_Word,FOAM_Word},          FOAM_NOp,  2,
                             {FOAM_Word,FOAM_Word}},
 {FOAM_BVal_WordDivideDouble,0,"WordDivideDouble",
                         0,3,{FOAM_Word,FOAM_Word,FOAM_Word},FOAM_NOp,  3,
                             {FOAM_Word,FOAM_Word,FOAM_Word}},
 {FOAM_BVal_WordPlusStep,0,"WordPlusStep",
                         0,3,{FOAM_Word,FOAM_Word,FOAM_Word},FOAM_NOp,  2,
                             {FOAM_Word,FOAM_Word}},
 {FOAM_BVal_WordTimesStep,0,"WordTimesStep",
                         0,4,{FOAM_Word,FOAM_Word,FOAM_Word,FOAM_Word},
	  						     FOAM_NOp,  2,
                             {FOAM_Word,FOAM_Word}},

 {FOAM_BVal_BInt0,	 0,"BInt0",
			 0,0,{0},			     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BInt1,	 0,"BInt1",
			 0,0,{0},			     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntIsZero,	 0,"BIntIsZero",
			 0,1,{FOAM_BInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntIsNeg,	 0,"BIntIsNeg",
			 0,1,{FOAM_BInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntIsPos,	 0,"BIntIsPos",
			 0,1,{FOAM_BInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntIsEven,	 0,"BIntIsEven",
			 0,1,{FOAM_BInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntIsOdd,	 0,"BIntIsOdd",
			 0,1,{FOAM_BInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntIsSingle,0,"BIntIsSingle",
			 0,1,{FOAM_BInt},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntEQ,	 0,"BIntEQ",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntNE,	 0,"BIntNE",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntLT,	 0,"BIntLT",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntLE,	 0,"BIntLE",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_BIntNegate,	 0,"BIntNegate",
			 0,1,{FOAM_BInt},		     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntPrev,	 0,"BIntPrev",
			 0,1,{FOAM_BInt},		     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntNext,	 0,"BIntNext",
			 0,1,{FOAM_BInt},		     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntPlus,	 0,"BIntPlus",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntMinus,	 0,"BIntMinus",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntTimes,	 0,"BIntTimes",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntTimesPlus, 0,"BIntTimesPlus",
			 0,3,{FOAM_BInt,FOAM_BInt,FOAM_BInt},FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntMod,	 0,"BIntMod",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntQuo,	 0,"BIntQuo",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntRem,	 0,"BIntRem",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntDivide,	 0,"BIntDivide",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_NOp,
			 2, {FOAM_BInt, FOAM_BInt}},
 {FOAM_BVal_BIntGcd,	 0,"BIntGcd",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntSIPower, 0,"BIntSIPower",
			 0,2,{FOAM_BInt, FOAM_SInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntBIPower, 0,"BIntBIPower",
			 0,2,{FOAM_BInt, FOAM_BInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntPowerMod,0,"BIntPowerMod",
 			 0,3,{FOAM_BInt, FOAM_BInt, FOAM_BInt},FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntLength,	 0,"BIntLength",
			 0,1,{FOAM_BInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_BIntShiftUp, 0,"BIntShiftUp",
			 0,2,{FOAM_BInt, FOAM_SInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntShiftDn, 0,"BIntShiftDn",
			 0,2,{FOAM_BInt, FOAM_SInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntShiftRem, 0,"BIntShiftRem",
			 0,2,{FOAM_BInt, FOAM_SInt},	     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntBit,	 0,"BIntBit",
			 0,2,{FOAM_BInt, FOAM_SInt},	     FOAM_Bool, 1, {0}},

 {FOAM_BVal_PtrNil,	 0,"PtrNil",
			 0,0,{0},			     FOAM_Ptr, 1, {0}},
 {FOAM_BVal_PtrIsNil,	 0,"PtrIsNil",
			 0,1,{FOAM_Ptr},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_PtrMagicEQ,  0,"PtrMagicEQ",
                         0,2,{FOAM_Ptr, FOAM_Ptr},           FOAM_Bool, 1, {0}},
 {FOAM_BVal_PtrEQ,	 0,"PtrEQ",
			 0,2,{FOAM_Ptr, FOAM_Ptr},	     FOAM_Bool, 1, {0}},
 {FOAM_BVal_PtrNE,	 0,"PtrNE",
			 0,2,{FOAM_Ptr, FOAM_Ptr},	     FOAM_Bool, 1, {0}},

 {FOAM_BVal_FormatSFlo,	 0,"FormatSFlo",
			 1,3,{FOAM_SFlo,FOAM_Arr, FOAM_Rec}, FOAM_SInt, 1, {0}},
 {FOAM_BVal_FormatDFlo,	 0,"FormatDFlo",
			 1,3,{FOAM_DFlo,FOAM_Arr, FOAM_Rec}, FOAM_SInt, 1, {0}},
 {FOAM_BVal_FormatSInt,	 0,"FormatSInt",
			 1,3,{FOAM_SInt,FOAM_Arr, FOAM_Rec}, FOAM_SInt, 1, {0}},
 {FOAM_BVal_FormatBInt,	 0,"FormatBInt",
			 1,3,{FOAM_BInt, FOAM_Arr, FOAM_Rec},FOAM_SInt, 1, {0}},

 {FOAM_BVal_ScanSFlo,	 0,"ScanSFlo",
			 0,2,{FOAM_Arr, FOAM_SInt},	      FOAM_NOp,
			 2, {FOAM_SFlo, FOAM_SInt}},
 {FOAM_BVal_ScanDFlo,	 0,"ScanDFlo",
			 0,2,{FOAM_Arr, FOAM_SInt},	      FOAM_NOp,
			 2, {FOAM_DFlo, FOAM_SInt}},
 {FOAM_BVal_ScanSInt,	 0,"ScanSInt",
			 0,2,{FOAM_Arr, FOAM_SInt},	      FOAM_NOp,
			 2, {FOAM_SInt, FOAM_SInt}},
 {FOAM_BVal_ScanBInt,	 0,"ScanBInt",
			 0,2,{FOAM_Arr, FOAM_SInt},	      FOAM_NOp,
			 2, {FOAM_BInt, FOAM_SInt}},

 {FOAM_BVal_SFloToDFlo,	 0,"SFloToDFlo",
			 0,1,{FOAM_SFlo},		     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloToSFlo,	 0,"DFloToSFlo",
			 0,1,{FOAM_DFlo},		     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_ByteToSInt,	 0,"ByteToSInt",
			 0,1,{FOAM_Byte},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntToByte,	 0,"SIntToByte",
			 0,1,{FOAM_SInt},		     FOAM_Byte, 1, {0}},
 {FOAM_BVal_HIntToSInt,	 0,"HIntToSInt",
			 0,1,{FOAM_HInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntToHInt,	 0,"SIntToHInt",
			 0,1,{FOAM_SInt},		     FOAM_HInt, 1, {0}},
 {FOAM_BVal_SIntToBInt,	 0,"SIntToBInt",
			 0,1,{FOAM_SInt},		     FOAM_BInt, 1, {0}},
 {FOAM_BVal_BIntToSInt,	 0,"BIntToSInt",
			 0,1,{FOAM_BInt},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntToSFlo,	 0,"SIntToSFlo",
			 0,1,{FOAM_SInt},		     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SIntToDFlo,	 0,"SIntToDFlo",
			 0,1,{FOAM_SInt},		     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_BIntToSFlo,	 0,"BIntToSFlo",
			 0,1,{FOAM_BInt},		     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_BIntToDFlo,	 0,"BIntToDFlo",
			 0,1,{FOAM_BInt},		     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_PtrToSInt,	 0,"PtrToSInt",
			 0,1,{FOAM_Ptr},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_SIntToPtr,	 0,"SIntToPtr",
			 0,1,{FOAM_SInt},		     FOAM_Ptr, 1, {0}},

 {FOAM_BVal_ArrToSFlo,	 0,"ArrToSFlo",
			 0,1,{FOAM_Arr},		     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_ArrToDFlo,	 0,"ArrToDFlo",
			 0,1,{FOAM_Arr},		     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_ArrToSInt,	 0,"ArrToSInt",
			 0,1,{FOAM_Arr},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_ArrToBInt,	 0,"ArrToBInt",
			 0,1,{FOAM_Arr},		     FOAM_BInt, 1, {0}},

 {FOAM_BVal_PlatformRTE, 0,"PlatformRTE",
			 0,0,{0},			     FOAM_SInt, 1, {0}},

 {FOAM_BVal_PlatformOS, 0,"PlatformOS",
			 0,0,{0},			     FOAM_SInt, 1, {0}},

 {FOAM_BVal_Halt,	 0,"Halt",
			 1,1,{FOAM_SInt},		     FOAM_Word, 1, {0}},

 {FOAM_BVal_RoundZero, 0,"RoundZero",
  		       0, 0, {0},			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_RoundNearest, 0,"RoundNearest",		     
  		       0, 0, {0},			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_RoundUp, 0,"RoundUp",			     
  		       0, 0, {0}, 			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_RoundDown, 0,"RoundDown",			     
  		       0, 0, {0}, 			     FOAM_SInt, 1, {0}},
 {FOAM_BVal_RoundDontCare, 0,"RoundDontCare",		     
  		       0, 0, {0}, 			     FOAM_SInt, 1, {0}},

 {FOAM_BVal_SFloTruncate, 0, "SFloTruncate",
  		       0,1,{FOAM_SFlo},			     FOAM_BInt, 1, {0}},
 {FOAM_BVal_SFloFraction, 0, "SFloFraction",
  		       0,1,{FOAM_SFlo},			     FOAM_SFlo, 1, {0}},
 {FOAM_BVal_SFloRound, 0, "SFloRound",
  		       0,2,{FOAM_SFlo, FOAM_SInt},	     FOAM_BInt, 1, {0}},

 {FOAM_BVal_DFloTruncate, 0, "DFloTruncate",
  		       0,1,{FOAM_DFlo},			     FOAM_BInt, 1, {0}},
 {FOAM_BVal_DFloFraction, 0, "DFloFraction",
  		       0,1,{FOAM_DFlo},			     FOAM_DFlo, 1, {0}},
 {FOAM_BVal_DFloRound, 0, "DFloRound",
  		       0,2,{FOAM_DFlo, FOAM_SInt},	     FOAM_BInt, 1, {0}},

 {FOAM_BVal_StoForceGC, 0, "StoForceGC",
			1, 0, {0},			     FOAM_NOp, 0, {0}},
 {FOAM_BVal_StoInHeap,  0, "StoInHeap",
			1, 1, {FOAM_Ptr},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_StoIsWritable,  0, "StoIsWritable",
			1, 1, {FOAM_Ptr},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_StoMarkObject,  0, "StoMarkObject",
			1, 1, {FOAM_Ptr},		     FOAM_SInt, 1, {0}},
 {FOAM_BVal_StoRecode,  0, "StoRecode",
			1, 2, {FOAM_Ptr, FOAM_SInt},	     FOAM_Word, 1, {0}},
 {FOAM_BVal_StoNewObject,  0, "StoNewObject",
			1, 2, {FOAM_SInt, FOAM_Bool},	     FOAM_NOp, 0, {0}},
 {FOAM_BVal_StoATracer,  0, "StoATracer",
			1, 2, {FOAM_SInt, FOAM_Clos},	     FOAM_NOp, 0, {0}},
 {FOAM_BVal_StoCTracer,  0, "StoCTracer",
			1, 2, {FOAM_SInt, FOAM_Word},	     FOAM_NOp, 0, {0}},
 {FOAM_BVal_StoShow,  0, "StoShow",
			1, 1, {FOAM_SInt},		     FOAM_NOp, 0, {0}},
 {FOAM_BVal_StoShowArgs, 0, "StoShowArgs",
			1, 1, {FOAM_Ptr},		     FOAM_SInt, 1, {0}},

 {FOAM_BVal_TypeInt8,   0, "TypeInt8",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeInt16,  0, "TypeInt16",         0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeInt32,  0, "TypeInt32",         0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeInt64,  0, "TypeInt64",         0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeInt128, 0, "TypeInt128",        0, 0, {0},   FOAM_SInt, 1, {0}},

 {FOAM_BVal_TypeNil,    0, "TypeNil",           0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeChar,   0, "TypeChar",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeBool,   0, "TypeBool",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeByte,   0, "TypeByte",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeHInt,   0, "TypeHInt",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeSInt,   0, "TypeSInt",          0, 0, {0},   FOAM_SInt, 1, {0}}, {FOAM_BVal_TypeBInt,   0, "TypeBInt",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeSFlo,   0, "TypeSFlo",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeDFlo,   0, "TypeDFlo",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeWord,   0, "TypeWord",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeClos,   0, "TypeClos",          0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypePtr,    0, "TypePtr",           0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeRec,    0, "TypeRec",           0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeArr,    0, "TypeArr",           0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_TypeTR,     0, "TypeTR",            0, 0, {0},   FOAM_SInt, 1, {0}},

 {FOAM_BVal_RawRepSize, 0, "RawRepSize",        0, 1, {FOAM_SInt},
                                                           FOAM_SInt, 1, {0}},

 {FOAM_BVal_SizeOfInt8, 0, "SizeOfInt8",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfInt16,0, "SizeOfInt16",       0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfInt32,0, "SizeOfInt32",       0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfInt64,0, "SizeOfInt64",       0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfInt128,0, "SizeOfInt128",     0, 0, {0},   FOAM_SInt, 1, {0}},

 {FOAM_BVal_SizeOfNil,  0, "SizeOfNil",         0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfChar, 0, "SizeOfChar",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfBool, 0, "SizeOfBool",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfByte, 0, "SizeOfByte",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfHInt, 0, "SizeOfHInt",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfSInt, 0, "SizeOfSInt",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfBInt, 0, "SizeOfBInt",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfSFlo, 0, "SizeOfSFlo",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfDFlo, 0, "SizeOfDFlo",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfWord, 0, "SizeOfWord",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfClos, 0, "SizeOfClos",        0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfPtr,  0, "SizeOfPtr",         0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfRec,  0, "SizeOfRec",         0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfArr,  0, "SizeOfArr",         0, 0, {0},   FOAM_SInt, 1, {0}},
 {FOAM_BVal_SizeOfTR,   0, "SizeOfTR",          0, 0, {0},   FOAM_SInt, 1, {0}},

 {FOAM_BVal_ListNil, 0, "ListNil",
			0, 0, {0},			     FOAM_Ptr, 1, {0}},
 {FOAM_BVal_ListEmptyP, 0, "ListEmptyP",
			0, 1, {FOAM_Ptr},		     FOAM_Bool, 1, {0}},
 {FOAM_BVal_ListHead, 0, "ListHead",
			0, 1, {FOAM_Ptr},		     FOAM_Word, 1, {0}},
 {FOAM_BVal_ListTail, 0, "ListTail",
			0, 1, {FOAM_Ptr},		     FOAM_Ptr, 1, {0}},
 {FOAM_BVal_ListCons, 0, "ListCons",
			0, 2, {FOAM_Word, FOAM_Ptr},	     FOAM_Ptr, 1, {0}},

#if EDIT_1_0_n1_06
  {FOAM_BVal_NewExportTable, 0, "NewExportTable",
                        0, 2, {FOAM_Word, FOAM_SInt}, FOAM_Word, 1, {0}},
  {FOAM_BVal_AddToExportTable, 0, "AddToExportTable",
                        1, 5, {FOAM_Word, FOAM_SInt, FOAM_SInt, FOAM_Arr,
                                FOAM_Arr}, FOAM_Values, 0, {0}},
  {FOAM_BVal_FreeExportTable, 0, "FreeExportTable",
                        1, 1, {FOAM_Word}, FOAM_Values, 0, {0}},
#endif
#if EDIT_1_0_n1_AB
  /*
   * Note that ssaPhi actually takes variable number of arguments
   * but since it must never reach genc this doesn't matter.
   */
  {FOAM_BVal_ssaPhi, 0, "ssaPhi", 0, 0, {0}, FOAM_Values, 0, {0}},
#endif

};
