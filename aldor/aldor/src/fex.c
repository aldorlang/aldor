#include "fex.h"
#include "sexpr.h"
#include "comsg.h"
#include "debug.h"
#include "util.h"
#include "symcoinfo.h"

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
			fexLexc	  = foamDEnvArgc(foam->foamProg.levels);
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
		if (fexFmtc != 0 && i < fexLexc) {
			i = fexLexv[i];
			if (j < foamDDeclArgc(fexFmtv[i]))
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
#if 0 /* This breaks the format of FOAM sexprs. */
	if (DEBUG(foamSpos)) {
		if (foamPos(foam) != sposNone)
			sx = sxCons(sxiFrInteger(sposLine(foamPos(foam))), sx);
	}
#endif
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
			if (isDecl && argf[fi] == 'w') {
				li = -1;
			} else {
				li = (long) foamArgv(foam)[si].data;
			}
			if (DEBUG(phase)) {
				li = (long) foamArgv(foam)[si].data;
			}
			sxi = sxiFrInteger(li);
#ifdef NEW_FORMATS
			if (DEBUG(phase)) {
				if (foamTag(foam) == FOAM_Prog
				    && si > 4) {
					assert(li < fexFmtc);
					sxi = foamToSExpr0(fexFmtv[li]);
				}
			}
#endif
			break;
		case 't': {
			AInt tag = foamArgv(foam)[si].data;
			if (tag < FOAM_LIMIT)
				sxi = foamSExpr(foamArgv(foam)[si].data);
			else
				sxi = sxiFrInteger(tag);
			break;
		}
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
	foamEnsureInit();
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

	foamEnsureInit();

	if (!sxiConsP(sx))  croak(sx, ALDOR_F_LoadNotList);
	if (!sxiSymbolP(sxi = sxCar(sx))) croak(sxi, ALDOR_F_LoadNotSymbol);

	op   = sxiToSymbol(sxi);
	if (!symCoInfo(op) || (FoamBValTag)symCoInfo(op)->foamTagVal == FOAM_BVAL_LIMIT)
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

			if (!symCoInfo(sym) || (int)symCoInfo(sym)->foamTagVal == -1)
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

