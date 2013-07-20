/*****************************************************************************
 *
 * genlisp.c: Foam-to-Lisp translation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "gf_util.h"
#include "util.h"
#include "strops.h"
#include "sexpr.h"

/*
 * The following naming conventions are used in this file:
 *   gli -- function: Foam -> SExpr
 *   gl0 -- utility function
 *   glv -- variable associated with genlisp
 */
/* Output: 
 *  By default Common ie. package  directives are used.
 *
 * Alternative is scheme which puts
 * a call to Foam-Header and Foam-footer at the extremes of the file.
 *
 * Problems:
 *   Non-local returns
 *   labels + Seqs
 */

/*
 * Global variables used for lisp generation
 */
static Foam      glvDGlo;                        /* Unit globals */
static Foam      glvDConst;                      /* Unit constants */
static Foam      glvDFmt;                        /* Unit formats */
static Foam      glvDPar;                        /* Unit/Prog parameters */
static Foam      glvDLoc;                        /* Unit/Prog locals */
static Foam	 glvLexFormats;			 /* Prog lex format stack */
static Foam	 glvProg;			 /* Prog being translated */
static int	 glvIsLeaf;			 /* Prog is a leaf proc */
static int	 glvHasGoto;			 /* true iff prog has goto */
static int	 glvLabelNum;			 /* temp label number */
static SExpr	 glvFunName;			 /* name of function */
static String	 glvFileName;			 /* name of .as file */
static Foam      glvActualLexFormats;		 /* Lex Fomats used */

/*
 * Declaration of local functions.
 * For comments, see the functions themselves.
 */
local  SExpr	 gliUnit                (Foam);
local  SExpr     gliDef                 (Foam);
local  SExpr     gliPushEnv             (Foam);
local  SExpr	 gliEInfo		(Foam);
local  SExpr     gliEEnv                (Foam);
local  SExpr	 gliPRef		(Foam);
local  SExpr     gliGlo                 (Foam);
local  SExpr     gliPlainGlo            (Foam);
local  SExpr     gliConst               (Foam);
local  SExpr     gliPar                 (Foam);
local  SExpr     gliLoc                 (Foam);
local  SExpr     gliLex                 (Foam);
local  SExpr	 gliEnv		  	(Foam);
local  SExpr	 gliPCall		(Foam);
local  SExpr	 gliArr			(Foam);
local  SExpr	 gliANew		(Foam);
local  SExpr	 gliRNew		(Foam);
local  SExpr	 gliRElt		(Foam);
local  SExpr	 gliAElt		(Foam);
local  SExpr	 gliEElt		(Foam);
local  SExpr	 gliSelect		(Foam);

local  SExpr     gliExpr                (Foam);
local  SExpr     gliSet                 (Foam);
local  SExpr	 gl0SetForm             (SExpr, SExpr);
local  SExpr     gliId                  (Foam);
local  SExpr     gliIf                  (Foam);

local  SExpr	 gl0MakeUnitHeader	(void);
local  SExpr	 gl0MakeUnitIface	(Foam, Foam);
local  SExpr     gl0ExprOf              (SExpr, int, Foam);
local  SExpr	 gl0ListOf		(Foam);
local  Foam      gl0GetDecl             (Foam);
local  SExpr	 gl0Prog		(Foam, Foam);
local  SExpr	 gl0MakeDeclare		(Foam);
local  SExpr	 gl0MakeSpecial		(Foam, SExpr);
local  SExpr	 gl0DeclareDefun	(Foam, Foam);
local  SExpr	 gl0DeclareVar		(Foam, int);
local  SExpr	 gl0ProgType		(Foam, Foam);
local  SExpr     gl0typeName		(String);
local  SExpr     gl0IdName		(Foam);
local  SExpr     gl0IdPlainName		(Foam);
local  SExpr     gl0Id                  (FoamTag t, int, String);
local  SExpr	 gl0OpenCall		(Foam);
local  SExpr	 gl0MakeDDecl   	(int);
local  SExpr	 gl0FieldName		(String, int);
local  SExpr	 gl0StructName		(int);
local  SExpr	 gl0MakeKeyword		(String);
local  SExpr	 gl0MakeLevel		(int);
local  SExpr     gl0VarNum		(String, int);
local  SExpr	 gl0EnvInit		(Foam);
/* local  SExpr	 gl0InitCode		(Foam, SExpr); */
local  SExpr	 gl0EnvRef		(int, SExpr);
local  SExpr	 gl0FlattenProgn	(SExpr);
local  SExpr 	 gl0FlattenProg1	(SExpr);
local  SExpr 	 gl0FlattenProgn	(SExpr);
local  String	 gl0ExtractFileName	(void);
local  SExpr     gl0EEnv                (SExpr, int);
local  Foam	 gl0InitFormats		(Foam);
local  void	 gl0UseEnv		(Foam);

/*
 * Macros for Lisp expressions
 */
#define lispId(s)        sxiFrSymbol(symInternConst(s))
#define lisp0(f)         sxiList(1, f)
#define lisp1(f,a)       sxiList(2, f, a)
#define lisp2(f,a,b)     sxiList(3, f, a, b)
#define lisp3(f,a,b,c)   sxiList(4, f, a, b, c)
#define lisp4(f,a,b,c,d) sxiList(5, f, a, b, c, d)

/* 
 * foam types
 */
#define GL_Bool		lispId("Bool")
#define GL_SInt		lispId("SInt")
#define GL_HInt		lispId("HInt")
#define GL_BInt		lispId("BInt")
#define	GL_SFlo		lispId("SFlo")
#define GL_Char		lispId("Char")
#define GL_Byte		lispId("Byte")
#define GL_DFlo		lispId("DFlo")

/*
 * foam instructions
 */
#define GL_CCall        lispId("CCall")
#define GL_Clos		lispId("Clos")
#define GL_ClosFun	lispId("ClosFun")
#define GL_ClosEnv	lispId("ClosEnv")
#define GL_DDecl	lispId("DDecl")
#define GL_Env		lispId("Env")
#define GL_Lex		lispId("Lex")
#define GL_RElt         lispId("RElt")
#define GL_RNew         lispId("RNew")
#define GL_EElt		lispId("EElt")
#define GL_AElt		lispId("AElt")
#define GL_ANew		lispId("ANew")
#define GL_EnvInfo	lispId("EnvInfo")
#define GL_SetEnvInfo	lispId("SetEnvInfo")
#define GL_ProgHash	lispId("ProgHashCode")
#define GL_SetProgHash	lispId("SetProgHashCode")
#define GL_SetClosEnv	lispId("SetClosEnv")
#define GL_SetClosFun	lispId("SetClosFun")
#define GL_FoamFunction lispId("foamfun")
#define GL_FoamFree     lispId("FoamFree")
#define GL_FoamEEnsure  lispId("FoamEnvEnsure")

/*
 * other foam support
 */
#define GL_e1		lispId("e1")
#define GL_EnvLevel     lispId("EnvLevel")
#define GL_EnvNext      lispId("EnvNext")
#define GL_MakeEnv	lispId("MakeEnv")
#define GL_MakeLevel	lispId("MakeLevel")
#define GL_MakeLit	lispId("MakeLit")
#define GL_SetLex	lispId("SetLex")
#define GL_SetAElt      lispId("SetAElt")
#define GL_SetRElt      lispId("SetRElt")
#define GL_SetEElt	lispId("SetEElt")

#define GL_FileExports   lispId("FILE-EXPORTS")
#define GL_FileImports   lispId("FILE-IMPORTS")
#define GL_IgnoreVar    lispId("IGNORE-VAR")
#define GL_DefProg	lispId("DEFPROG")
#define GL_Seq		lispId("TAGBODY")
#define GL_DefSpecials   lispId("DEFSPECIALS")
#define GL_DeclareType  lispId("DECLARE-TYPE")
#define GL_DeclareFunction lispId("DECLARE-PROG")
#define GL_UnitHead	lispId("UNIT-HEADER")
#define GL_ReturnBlock  lispId("BLOCK-RETURN")

#define GL_NUL          lispId("CharCode0")
/*
 * Common Lisp/Scheme words
 */
#define GL_And		lispId("AND")
#define GL_Block	lispId("BLOCK")
#define GL_Case		lispId("CASES")
#define GL_Compile	lispId("COMPILE")
#define GL_Declare      lispId("DECLARE")
#define GL_Declaim	lispId("DECLAIM")
#define GL_Defconstant  lispId("DEFCONSTANT")
#define GL_Defstruct	lispId("DEFSTRUCT")
#define GL_Defun        lispId("DEFUN")
#define GL_Eval		lispId("EVAL")
#define GL_EvalWhen	lispId("EVAL-WHEN")
#define GL_Ftype        lispId("FTYPE")
#define GL_Funcall      lispId("FUNCALL")
#define GL_Function     lispId("FUNCTION")
#define GL_Go		lispId("GO")
#define GL_If           lispId("IF")
#define GL_When           lispId("WHEN")
#define GL_Ignore	lispId("IGNORE")
#define GL_InPackage	lispId("IN-PACKAGE")
#define GL_Integer      lispId("INTEGER")
#define GL_Lambda       lispId("LAMBDA")
#define GL_Load		lispId("LOAD")
#define GL_Loop		lispId("LOOP")
#define GL_Let          lispId("LET")
#define GL_MVSetq	lispId("MULTIPLE-VALUE-SETQ")
#define GL_Nil          lispId("NIL")
#define GL_Or		lispId("OR")
#define GL_Proclaim     lispId("PROCLAIM")
#define GL_Progn        lispId("PROGN")
#define GL_Quote        lispId("QUOTE")
#define GL_Return       lispId("RETURN")
#define GL_ReturnFrom   lispId("RETURN-FROM")
#define GL_Setq         lispId("SETQ")
#define GL_TLSetq         lispId("top-level-define")
#define GL_Setf         lispId("SETF")
#define GL_Special      lispId("SPECIAL")
#define GL_T            lispId("T")
#define GL_The          lispId("THE")
#define GL_Type         lispId("TYPE")
#define GL_Values	lispId("VALUES")

/* Make a new label for goto */
#define gl0GenLabel() lispId(strPrintf("G_%d", glvLabelNum++))

#define gl0GlobalInfo(f,x,y,z) genGlobalInfo(f,x,y,z)

#define SOPT_COMMON	"common"
#define SOPT_SCHEME     "scheme"
#define	SOPT_FTYPE	"ftype="
#define SOPT_CASE	"mixedcase"

enum gllangOpt { GLLANG_COMMON, GLLANG_SCHEME };
static enum gllangOpt langOpt = GLLANG_COMMON; /* by default */

ULong	glWriteMode	= 0;
static int glimixedCase;

int
genLispOption(String opt)
{
	String	s;
	
	if      (strEqual(opt, SOPT_COMMON)) langOpt = GLLANG_COMMON;
	else if (strEqual(opt, SOPT_SCHEME)) langOpt = GLLANG_SCHEME;
	else if (strEqual(opt, SOPT_CASE))   glimixedCase = true;
	else if (s = strIsPrefix(SOPT_FTYPE, opt), s)	sxiLispFileType = s;
	else return -1;

	return 0;
}

/*
 * Top level entry point for lisp generation.
 */

SExpr
genLisp(Foam foam)
{
	glWriteMode  = glimixedCase ? SXRW_MixedCase : SXRW_FoldCase;
	glWriteMode |= (langOpt == GLLANG_COMMON) ? SXRW_Packages : 0;
	return gliUnit(foam);
}

/*
 * Generate lisp for a single file.
 */
local SExpr
gliUnit(Foam foam)
{
	SExpr	defl, sx, exports;
	Foam	defs, initDef, initProg, initSeq;
	int	i;

	assert(foamTag(foam) == FOAM_Unit);

	glvDGlo      = foamUnitGlobals(foam);
	glvDConst    = foamUnitConstants(foam);
	glvDFmt      = foam->foamUnit.formats;


	glvHasGoto   = 0;
	glvLabelNum  = 0;
	glvFunName   = sxNil;
	glvFileName  = gl0ExtractFileName();
	glvLexFormats= 0;

	defs = foam->foamUnit.defs;
	assert(foamTag(defs) == FOAM_DDef);

	defl  = sxCons(gl0MakeUnitHeader(), sxNil);
	exports = sxNil;

	/* collect defun names in a list and make proclaims. */
	for (i = 0; i < foamArgc(defs); i++) {
		sx = gl0MakeDeclare(foamArgv(defs)[i].code);
		if (!sxiNull(sx)) defl = sxCons(sx, defl);
	}

	initDef     = foamArgv(defs)[0].code;
 	assert(foamTag(initDef) == FOAM_Def);
	initProg    = initDef->foamDef.rhs;
	assert(foamTag(initProg) == FOAM_Prog);
	initSeq	    = initProg->foamProg.body;
	/*!! this may not always be a valid assumption. */
	assert(foamTag(initSeq) == FOAM_Seq);

	/* proclaim defs in initialization program. */
	for (i = 0; i < foamArgc(initSeq); i++) {
		sx = gl0MakeDeclare(foamArgv(initSeq)[i].code);
		if (!sxiNull(sx)) defl = sxCons(sx, defl);
	}


	/* proclaim specials for globals. */
	sx = sxNil;
	for (i = 0; i < foamArgc(initSeq); i++)
		sx = gl0MakeSpecial(foamArgv(initSeq)[i].code, sx);
	if (!sxiNull(sx)) { /* Warning: Untested! */
		sx = sxCons(GL_DefSpecials, sx);
		defl = sxCons(sx, defl);
	}

	/* make defstructs for formats */
	for(i=lexesSlot; i < foamArgc(glvDFmt); i++) {
		sx = gl0MakeDDecl(i);
		if (!sxiNull(sx)) defl = sxCons(sx, defl);
	}

	/* first, make program definitions. */
	for (i = 1; i < foamArgc(defs); i++)
		if (foamTag(foamArgv(defs)[i].code) == FOAM_Prog)
			defl = sxCons(gliDef(foamArgv(defs)[i].code), defl);
	defl = sxCons(gliDef(initDef), defl);

	/* next, make other definitions. */
	for (i = 1; i < foamArgc(defs); i++)
		if (foamTag(foamArgv(defs)[i].code) != FOAM_Prog)
			defl = sxCons(gliDef(foamArgv(defs)[i].code), defl);

	defl = sxNConc(gl0MakeUnitIface(defs, glvDFmt->foamDFmt.argv[globalsSlot]), defl);

	defl = sxNReverse(defl);
	
	return defl;
}


/*
 * Form an "in-package" expression for the generated code.
 */

local SExpr
gl0MakeUnitHeader(void)
{
	if (langOpt==GLLANG_COMMON)
		return lisp3(GL_InPackage,
			sxiFrString("FOAM-USER"),
			gl0MakeKeyword("USE"),
			lisp1(GL_Quote,
				sxiList(2,sxiFrString("FOAM"),
					sxiFrString("LISP")) ));
	else
		return lisp0(GL_UnitHead);
}


/*
 * Generate lisp for an arbitrary Foam expression.
 */

local SExpr
gliExpr(Foam foam)
{
	SExpr     sx = sxNil;

	switch (foamTag(foam)) {
	  case FOAM_Prog:
		bugBadCase(FOAM_Prog);
		break;
		
	  case FOAM_NOp:
	  case FOAM_Nil:
		sx = GL_Nil;
		break;
	  case FOAM_CCall: {
		  sx = gl0ExprOf(GL_CCall, 1, foam);
		  break;
	  }
	  case FOAM_OCall: {
		  sx = gl0OpenCall(foam);
		  break;
	  }
	  case FOAM_BVal: {
		  FoamTag op = foam->foamBVal.builtinTag;
		  sx = gl0ExprOf(foamBValSExpr(op), 1, foam);
		  break;
	  }
	  case FOAM_BCall: {
		  FoamTag op = foam->foamBCall.op;
		  sx = gl0ExprOf(foamBValSExpr(op), 1, foam);
		  break;
	  }
	  case FOAM_PCall:
		sx = gliPCall(foam);
		break;
	  case FOAM_If:
		sx = gliIf(foam);
		break;
	  case FOAM_Set: 
		sx = gliSet(foam);
		break;
	  case FOAM_Def: 
		sx = gliDef(foam);
		break;
	  case FOAM_PushEnv: 
		sx = gliPushEnv(foam);
		break;
	  case FOAM_PopEnv: 
		sx = GL_Nil;
		break;
	  case FOAM_EEnv: 
		sx = gliEEnv(foam);
		break;
	  case FOAM_Seq:
		sx = gl0ExprOf(GL_Progn, int0, foam);
		break;
	  case FOAM_Par:
	  case FOAM_Loc:
	  case FOAM_Glo:
	  case FOAM_Const:
		sx = gliId(foam);
		break;
	  case FOAM_Lex:
		sx = gliLex(foam);
		break;
	  case FOAM_Return:
		sx = lisp2(GL_ReturnBlock, glvFunName,
			   gliExpr(foam->foamReturn.value));
		break;
	  case FOAM_Cast:
		sx = gliExpr(foam->foamCast.expr);
		break;
	  case FOAM_Clos:
		sx = gl0ExprOf(GL_Clos, int0, foam);
		break;
	  case FOAM_Env:
		sx = gliEnv(foam);
		break;
	  case FOAM_EInfo:
		sx = gliEInfo(foam);
		break;
	  case FOAM_PRef:
	  	sx = gliPRef(foam);
		break;
	  case FOAM_Arr:
		sx = gliArr(foam);
		break;
	  case FOAM_CEnv:
		sx = gl0ExprOf(GL_ClosEnv, int0, foam);
		break;
	  case FOAM_CProg:
		sx = gl0ExprOf(GL_ClosFun, int0, foam);
		break;
	  case FOAM_Values:
		if (foamArgc(foam) == 0)
			sx = sxNil;
		else 
			sx = gl0ExprOf(GL_Values, int0, foam);
		break;
	  case FOAM_MFmt:
		sx = gliExpr(foam->foamMFmt.value);
		break;
	  case FOAM_EElt:
		sx = gliEElt(foam);
		break;
	  case FOAM_Char: /* XXX: Should be fixed elsewhere */
		sx = lisp2(GL_The, GL_Char,
			   (foam->foamChar.CharData=='\0')
			   ? GL_NUL	
                           : sxiFrChar(foam->foamChar.CharData));
		break;
	  case FOAM_Bool:
		sx = lisp2(GL_The, GL_Bool, (foam->foamBool.BoolData ?
					    GL_T : GL_Nil));
		break;
	  case FOAM_Byte:
		sx = lisp2(GL_The, GL_Byte,
			   sxiFrInteger(foam->foamByte.ByteData));
		break;
	  case FOAM_SInt:
		sx = lisp2(GL_The, GL_SInt,
			   sxiFrInteger(foam->foamSInt.SIntData));
		break;
	  case FOAM_HInt:
		sx = lisp2(GL_The, GL_HInt,
			   sxiFrInteger(foam->foamHInt.HIntData));
		break;
	  case FOAM_BInt:
		sx = lisp2(GL_The, GL_BInt,
			   sxiFrBigInteger(foam->foamBInt.BIntData));
		break;
	  case FOAM_SFlo:
		sx = lisp2(GL_The, GL_SFlo, sxiFrSFloat(foamToSFlo(foam)));
		break;
	  case FOAM_DFlo:
		sx = lisp2(GL_The, GL_DFlo, sxiFrDFloat(foamToDFlo(foam)));
		break;
	  case FOAM_Goto:
		sx = lisp1(GL_Go,
			   lispId(strPrintf("Lab%d", foam->foamGoto.label)));
		glvHasGoto = 1;
		break;
	  case FOAM_Label:
		sx = lispId(strPrintf("Lab%d", foam->foamGoto.label));
		break;
	  case FOAM_Select:
		sx = gliSelect(foam);
		break;
	  case FOAM_ANew:
		sx = gliANew(foam);
		break;
	  case FOAM_RNew:
		sx = gliRNew(foam);
		break;
	  case FOAM_AElt:
		sx = gliAElt(foam);
		break;
	  case FOAM_RElt:
		sx = gliRElt(foam);
		break;
	  case FOAM_Free:
		sx = lisp1(GL_FoamFree, gliExpr(foam->foamFree.place));
		break;
	  case FOAM_EEnsure:
		sx = lisp1(GL_FoamEEnsure, gliExpr(foam->foamEEnsure.env));
		break;
	  default:
		printf("unhandled foamTag = %s\n",
		       foamInfo(foamTag(foam)).str);
		sx = lispId("unhandled!");
		break;
	}
	return sx;
}

local SExpr
gliSet(Foam foam)
{
       SExpr lhs,rhs;

	if (foamTag(foam->foamSet.lhs) == FOAM_Values)
	  return lisp2(GL_MVSetq, gl0ListOf(foam->foamSet.lhs), 
		       gliExpr(foam->foamSet.rhs));

	lhs=gliExpr(foam->foamSet.lhs);
	rhs=gliExpr(foam->foamSet.rhs);
	return gl0SetForm(lhs,rhs);
}

/* 
  * convert reader form to a writer form 
  * Could make this into a switch..
  * Currently - a -> (setq a ..)
  *             (aref x) -> (setaref x)
  *             (RElt x) -> (setrelt x)
  * destructive on rdr form in last 2 cases
  */
local SExpr
gl0SetForm(SExpr rdr, SExpr value)
{
	if (sxSymbolP(rdr)==sxT)
		return lisp2(GL_Setq, rdr, value);

	assert(sxSymbolP(sxCar(rdr))==sxT);

	if (sxCar(rdr)==GL_AElt) {
		sxCar(rdr)=GL_SetAElt;
		sxNConc(rdr,lisp0(value));
		return rdr;
	}
	if (sxCar(rdr)==GL_RElt) {
		sxCar(rdr)=GL_SetRElt;
		sxNConc(rdr,lisp0(value));
		return rdr;
	}
	if (sxCar(rdr)==GL_EElt) {
		sxCar(rdr)=GL_SetEElt;
		sxNConc(rdr,lisp0(value));
		return rdr;
	}
	if (sxCar(rdr)==GL_Lex) {
		sxCar(rdr)=GL_SetLex;
		sxNConc(rdr,lisp0(value));
		return rdr;
	}
	if (sxCar(rdr)==GL_EnvInfo) {
		sxCar(rdr)=GL_SetEnvInfo;
		sxNConc(rdr,lisp0(value));
		return rdr;
	}
	if (sxCar(rdr)==GL_ClosEnv) {
		sxCar(rdr)=GL_SetClosEnv;
		sxNConc(rdr,lisp0(value));
		return rdr;
	}
	if (sxCar(rdr)==GL_ClosFun) {
		sxCar(rdr)=GL_SetClosFun;
		sxNConc(rdr,lisp0(value));
		return rdr;
	}
	if (sxCar(rdr) == GL_ProgHash) {
	      	sxCar(rdr) = GL_SetProgHash;
		sxNConc(rdr, lisp0(value));
		return rdr;
	}
	NotReached(return 0);
}

/* generate lisp for a name, appropriately quoting functions */
local SExpr
gliId(Foam foam)
{
	SExpr	sx = gl0IdName(foam);

	return sx;
}

/* generate lisp for a name. */
local SExpr
gl0IdName(Foam foam)
{
	switch (foamTag(foam)) {
	  default:      bugBadCase(foamTag(foam));
	  case FOAM_Glo:   return gliGlo(foam);
	  case FOAM_Const: return gliConst(foam);
	  case FOAM_Par:   return gliPar(foam);
	  case FOAM_Loc:   return gliLoc(foam);
	  case FOAM_Lex:   return gliLex(foam);
	}
}

local SExpr
gl0IdInfo(Foam decl, int ix)
{
	String name, exporter;
	int hash;
	SExpr sxTypeId;

	gl0GlobalInfo(decl, &name, &exporter, &hash);

	if (decl->foamGDecl.protocol == FOAM_Proto_Init) 
		sxTypeId = lispId("initializer");
	else
		sxTypeId = sxiFrInteger(hash);

	return sxCons(gl0Id(FOAM_Glo, ix, decl->foamGDecl.id),
		      sxCons(sxiFrString(name), 
			     sxCons(sxTypeId,
				    sxCons(sxiFrString(exporter), 
					   sxNil))));
}

local SExpr
gl0IdPlainName(Foam foam)
{
	if (foamTag(foam) != FOAM_Glo) bugBadCase(foamTag(foam));
	return gliPlainGlo(foam);
}

local SExpr
gliConst(Foam foam)
{
	Foam    decl = gl0GetDecl(foam);
	int     ix   = foam->foamConst.index;
	SExpr   val;

	val =  gl0Id(FOAM_Const, ix, strPrintf("%s-%s", glvFileName,
						 decl->foamDecl.id));
	return val;		

}

local SExpr
gliGlo(Foam foam)
{
	Foam    decl = gl0GetDecl(foam);
	int     ix   = foam->foamGlo.index;

	if (decl->foamGDecl.protocol == FOAM_Proto_Lisp)
		return gl0IdPlainName(foam);

	return  gl0Id(FOAM_Glo, ix, decl->foamGDecl.id);
}

local SExpr
gliPlainGlo(Foam foam)
{
	Foam    decl = gl0GetDecl(foam);
	String	str  = decl->foamGDecl.id;

	return  sxiFrSymbol(symIntern(str));
}

local SExpr
gliPar(Foam foam)
{
	Foam    decl = gl0GetDecl(foam);
	int     ix   = foam->foamPar.index;
	
	return  gl0Id(FOAM_Par, ix, decl->foamDecl.id);
}

local SExpr
gliLoc(Foam foam)
{
	Foam    decl = gl0GetDecl(foam);
	int     ix   = foam->foamLoc.index;
	
	return  gl0Id(FOAM_Loc, ix, decl->foamDecl.id);
}

local SExpr
gliLex(Foam foam)
{
	int     lv    = foam->foamLex.level;
	int     ix    = foam->foamLex.index;
	int     fi    = glvLexFormats->foamDEnv.argv[lv];
	Foam	ddecl = glvDFmt->foamDFmt.argv[fi];
	Foam    decl  = ddecl->foamDDecl.argv[ix];
	String  buf;
	SExpr   access;

	assert (foamTag(ddecl) == FOAM_DDecl);
	assert (foamTag(decl)  == FOAM_Decl);
	assert (strlen(decl->foamDecl.id) < 180);

	gl0UseEnv(foam);
	buf = strPrintf("Struct-%s-%d-%s-%d", glvFileName,
			fi, decl->foamDecl.id, ix);
	access = lispId(buf);
	return lisp3(GL_Lex, access, sxiFrInteger(ix), gl0VarNum("l", lv));
}


/*
 * Construct bindings+types for all locals
 * 
 */

local SExpr
gl0MakeLocals(Foam locals, SExpr inits0)
{
	SExpr     typedVars, liId, inits = inits0;
	int	  i, argc = foamDDeclArgc(locals);
	Foam	  *argv = locals->foamDDecl.argv;
	String	  id, typeId;

	assert(foamTag(locals) == FOAM_DDecl);

	typedVars = sxNil;

	for (i=0; i<argc; i++) {
		id = argv[i]->foamDecl.id;
		liId = gl0Id(FOAM_Loc, i, id);
		typeId = foamInfo(argv[i]->foamDecl.type).str;
		typedVars = sxCons(lisp1(liId, gl0typeName(typeId)),
				   typedVars);
	}

	if (sxiNull(inits0)) 
		return sxNReverse(typedVars);

	for(; inits != sxNil; inits = sxCdr(inits)) {
		liId = sxSecond(sxCar(inits)); /* Nasty ! */
		typeId = symString(sxiToSymbol(liId))[0]=='l' 
			 ? "Level" : "Env";
		typedVars = sxCons(lisp1(liId, gl0typeName(typeId)),
				   typedVars);
	}

	return sxNReverse(typedVars);
}


local SExpr
gl0typeName(String str)
{
	if (strcmp(str,"Rec")==0)
		return lispId("Record");
	else
		return lispId(str);
}

/*
 * Form an expression with op and foam[start]..foam[argc-1] as args.
 */
local SExpr
gl0ExprOf(SExpr op, int start, Foam foam)
{
	SExpr   sx;
	int     foami, sxi;

	sx = sxCons(op, sxNil);
	for (foami = start, sxi = 1; foami < foamArgc(foam); foami++, sxi++)
		sx = sxCons(gliExpr(foamArgv(foam)[foami].code), sx);
	sx = sxNReverse(sx);

	return sx;
}

local SExpr
gl0ListOf(Foam foam)
{
	SExpr   sx;
	int     i;

	sx =  sxNil;
	for (i = 0; i < foamArgc(foam); i++)
		sx = sxCons(gliExpr(foamArgv(foam)[i].code), sx);
	sx = sxNReverse(sx);

	return sx;
}

/*
 * Get the declaration object for a global, parameter or local.
 */
local Foam
gl0GetDecl(Foam foam)
{
	int     ix;
	Foam     decl;

	switch (foamTag(foam)) {
	  case FOAM_Glo:
		ix = foam->foamGlo.index;
		decl = glvDGlo->foamDDecl.argv[ix];
		break;
	  case FOAM_Const:
		ix = foam->foamConst.index;
		decl = glvDConst->foamDDecl.argv[ix];
		break;
	  case FOAM_Par:
		ix = foam->foamPar.index;
		decl = glvDPar->foamDDecl.argv[ix];
		break;
	  case FOAM_Loc:
		ix = foam->foamLoc.index;
		decl = glvDLoc->foamDDecl.argv[ix];
		break;
	  default:
		bugBadCase(foamTag(foam));
		NotReached(decl = 0);
	}
	assert(foamTag(decl) == FOAM_Decl || foamTag(decl) == FOAM_GDecl);
	return decl;
}

/*
 * Create the symbol for an identifier, given the level, index and string
 */     
local SExpr
gl0Id(FoamTag tag, int idx, String str)
{
	String    buf;
	
	if (*str) 
		switch (tag) {
		  case FOAM_Glo:   buf = strPrintf("G-%s",        str); break;
		  case FOAM_Const: buf = strPrintf("C%d-%s", idx, str); break;
		  case FOAM_Par:   buf = strPrintf("P%d-%s", idx, str); break;
		  case FOAM_Loc:   buf = strPrintf("T%d-%s", idx, str); break;
		  default:         bugBadCase(tag); NotReached(buf = 0);
		}
	else
		switch (tag) {
		  case FOAM_Glo:   bugBadCase(tag); NotReached(buf = 0); break;
		  case FOAM_Const: buf = strPrintf("C%d", idx); break;
		  case FOAM_Par:   buf = strPrintf("P%d", idx); break;
		  case FOAM_Loc:   buf = strPrintf("T%d", idx); break;
		  default:         bugBadCase(tag); NotReached(buf = 0);
		}
	return sxiFrSymbol(symIntern(buf));
}

#define gl0Level(i) 	gl0VarNum("l", i)
#define gl0Env(i)	gl0VarNum("e", i)
#define gl0MakeEnv(e,l)	lisp2(GL_MakeEnv, e, l)
#define gl0EnvLevel(i)	lisp1(GL_EnvLevel, gl0Env(i))
#define gl0EnvNext(i)	lisp1(GL_EnvNext, gl0Env(i))

#define emptyEnv(x) ((x == emptyFormatSlot) || \
		     (foamArgc(glvDFmt->foamDFmt.argv[x]) == 0))

local SExpr
gliEnv(Foam foam)
{
	int level = foam->foamEnv.level;
	gl0UseEnv(foam);
	if (!glvLexFormats) return GL_Nil; 	 /* if at top level */
	if (glvIsLeaf && level == 0 && 
	    (glvLexFormats->foamDEnv.argv[level] == emptyFormatSlot
	     || glvLexFormats->foamDEnv.argv[level] == envUsedSlot))
		return gl0MakeEnv(gl0VarNum("e", level+1), GL_Nil);
	else
		return gl0VarNum("e", level);
}

local SExpr
gliDef(Foam foam)
{
       SExpr lhs,rhs;

	/* see if we have a function defintion */
	if (foamTag(foam->foamDef.rhs) != FOAM_Prog) {
	        if (foamTag(foam->foamDef.lhs) == FOAM_Values)
			return lisp2(GL_MVSetq, gl0ListOf(foam->foamDef.lhs),
				     gliExpr(foam->foamDef.rhs));
		lhs=gliExpr(foam->foamDef.lhs);
		rhs=gliExpr(foam->foamDef.rhs);
		return gl0SetForm(lhs,rhs);
	}
	else
		return gl0Prog(foam->foamDef.lhs, foam->foamDef.rhs);
}

/* generate lisp for a Foam program */		
local SExpr
gl0Prog(Foam lhs, Foam prog)
{
	Foam            params, locals, body;
	SExpr           sx, name = gl0IdName(lhs), ei, hdr;

	assert(foamTag(prog) == FOAM_Prog);

#ifdef NEW_FORMATS
	params  = glvDFmt->foamDFmt.argv[prog->foamProg.params];
#else
	params  = prog->foamProg.params;
#endif
	locals  = prog->foamProg.locals;
	body    = prog->foamProg.body;

	glvLabelNum  = 0;
	glvHasGoto   = 0;
	glvDPar      = params;
	glvDLoc      = locals;
	glvProg	     = prog;
	glvIsLeaf    = foamProgIsLeaf(prog);
	glvLexFormats= prog->foamProg.levels;
	glvActualLexFormats = gl0InitFormats(glvLexFormats);
	glvFunName   = name;

	sx = gliExpr(body);
	ei = gl0EnvInit(prog);

	if (glvHasGoto) {
		sx = gl0FlattenProgn(sx);
	}

	hdr= sxCons(GL_DefProg, 
		    sxCons(gl0ProgType(lhs,prog),
		           lisp0(gl0MakeLocals(locals,ei))));
		   
	if (sxCar(sx) == GL_Progn) sx = sxCdr(sx);
	else sx = lisp0(sx);
	
	glvLexFormats = 0;
	foamFree(glvActualLexFormats);
	return(sxNConc(hdr,sxNConc(ei,sx)));
}

local Foam
gl0InitFormats(Foam foam)
{
	Foam new;
	int  i;
	assert(foamTag(foam) == FOAM_DEnv);
	new = foamCopy(foam);
	for (i=0; i<foamArgc(foam); i++)
		new->foamDEnv.argv[i] = (AInt) emptyFormatSlot;

	return new;
}

/* make lexical environment initializations */
local SExpr
gl0EnvInit(Foam prog)
{
	Foam	levels = glvActualLexFormats;
	SExpr 	sx = sxNil;
	SExpr   chain;
	int 	index = foamProgIndex(prog);

	int 	i, level, maxLevel = -1;
	
	if (!foamProgIsLeaf(prog)) {
		sx = sxCons(lisp2(GL_Setq, gl0Level(int0),
				  gl0MakeLevel(index)), sx);
		sx = sxCons(lisp2(GL_Setq, gl0Env(int0),
				  gl0MakeEnv(gl0Env(1), gl0Level(int0))), sx);
	}
	/* compute highest lex level used */

	for (i=1; i< foamArgc(levels); i++) {
		level = levels->foamDEnv.argv[i];
		if (!emptyEnv(level) || level == envUsedSlot) maxLevel = i;
	}

	/* Construct initialiser block */
	chain = sxNil;
	for (i=1; i <= maxLevel; i++) {
		level = levels->foamDEnv.argv[i];

		if (i == 1)
			chain = gl0EnvNext(i);
		else if (level == envUsedSlot || !emptyEnv(level)) {
			sx = sxCons(lisp2(GL_Setq, gl0Env(i), chain), 
				    sx);
			chain = gl0EnvNext(i);
		}
		else
			chain = lisp1(GL_EnvNext, chain);

		if (!emptyEnv(level) && level != envUsedSlot)
			sx = sxCons(lisp2(GL_Setq, gl0Level(i),
					     gl0EnvLevel(i)), sx);
	}
	if (chain != sxNil) sxiFree(chain);
	return sxNReverse(sx);
}

local SExpr 
gl0MakeLevel(int i)
{
	String 	buf;

	if (emptyEnv(i) || i == envUsedSlot) return GL_Nil;

	buf = strPrintf("MAKE-Struct-%s-%d", glvFileName, i);
	return lisp2(GL_MakeLevel,lispId(buf), lispId(buf+strlen("MAKE-")));
}

local void
gl0UseEnv(Foam foam)
{
	AInt val;
	int  level;

	switch(foamTag(foam)) {
	  case FOAM_Lex:
		level = foam->foamLex.level;
		val   = glvLexFormats->foamDEnv.argv[level];
		break;
	  case FOAM_Env:
		level = foam->foamEnv.level;
		val   = 0;
		break;
	  default:
		level = 0;
		val   = 0;
		bug("Use Env called with non-lex instr");
		break;
	}
	
	if (glvActualLexFormats->foamDEnv.argv[level] == emptyFormatSlot
	    || glvActualLexFormats->foamDEnv.argv[level] == 0)
		glvActualLexFormats->foamDEnv.argv[level] = val;
		
}

local SExpr
gl0VarNum(String s, int i)
{
	String 	buf;
	buf = strPrintf("%s%d", s, i);
	return lispId(buf);
}
	
/* make proclaim for each define and add name to defun list. */
local SExpr
gl0MakeDeclare(Foam foam)
{
	int 	tag;
	Foam	rhs, lhs;
	
	if (foamTag(foam) != FOAM_Def) return sxNil;

	rhs = foam->foamDef.rhs;
	lhs = foam->foamDef.lhs;
	tag = foamTag(rhs);

	if (foamTag(lhs) != FOAM_Glo && foamTag(lhs) != FOAM_Const)
		return sxNil;
	
	switch (tag) {
	  case FOAM_OCall:
		return gl0DeclareVar(lhs,rhs->foamOCall.type);
	  case FOAM_CCall:
		return gl0DeclareVar(lhs,rhs->foamCCall.type);
	  case FOAM_BCall:
		return gl0DeclareVar(lhs,foamBValRetType(rhs->foamBCall.op));
	  case FOAM_Clos:
		return gl0DeclareVar(lhs, FOAM_Clos);
	  case FOAM_Prog:
		return gl0DeclareDefun(lhs, rhs);
	  default:
		return sxNil;
	}
}	


local SExpr
gl0MakeUnitIface(Foam defs, Foam globals)
{
	SExpr imports = sxNil;
	SExpr exports = sxNil;
	Foam  gdecl;
	int i;

	assert(foamTag(defs) == FOAM_DDef);
        assert(foamTag(globals) == FOAM_DDecl);

	for (i=0; i<foamDDeclArgc(globals); i++) {
		gdecl = globals->foamDDecl.argv[i];
		if (gdecl->foamGDecl.protocol != FOAM_Proto_Foam
		    && gdecl->foamGDecl.protocol != FOAM_Proto_Init)
			continue;
		else if (gdecl->foamGDecl.dir == FOAM_GDecl_Import)
			imports = sxCons(gl0IdInfo(gdecl, i),
					 imports);
		else if (gdecl->foamGDecl.dir == FOAM_GDecl_Export)
			exports = sxCons(gl0IdInfo(gdecl, i),
			                 exports);
		else
			bug("odd value for gdecl direction");
	}
	exports = lisp1(GL_FileExports, lisp1(GL_Quote, exports));
	imports = lisp1(GL_FileImports, lisp1(GL_Quote, imports));

	exports = lisp1(exports, imports);
	return exports;
}


/* make proclaim for each define and add name to defun list. */
local SExpr
gl0MakeSpecial(Foam foam, SExpr specList)
{
	Foam	lhs;
	
	if (foamTag(foam) != FOAM_Def) return specList;
	lhs = foam->foamDef.lhs;
	if (foamTag(lhs) != FOAM_Glo) return specList;
	return sxCons(gl0IdName(lhs), specList);

}	

local SExpr
gliPushEnv(Foam foam)
{
	SExpr	sx;
	int	fmt = foam->foamPushEnv.format;

	if (fmt == emptyFormatSlot || fmt == envUsedSlot)
		sx = gl0MakeEnv(gliExpr(foam->foamPushEnv.parent), GL_Nil);
	else
		sx = gl0MakeEnv(gliExpr(foam->foamPushEnv.parent),
				gl0MakeLevel(fmt));
	return sx;
}

local SExpr
gliEInfo(Foam foam)
{
	return lisp1(GL_EnvInfo, gliExpr(foam->foamEInfo.env));
}

local SExpr
gliPRef(Foam foam)
{
	return lisp1(GL_ProgHash, gliExpr(foam->foamPRef.prog));
}

local SExpr
gliEEnv(Foam foam)
{
	return gl0EEnv(gliExpr(foam->foamEEnv.env), foam->foamEEnv.level);
}

local SExpr
gl0EEnv(SExpr env, int level)
{
	SExpr sx = env;

	while (level-- > 0)
		sx = lisp1(GL_EnvNext, sx);
	return sx;
}

#define typeIdentifier(type) (gl0typeName(foamInfo(type).str))

local SExpr
gl0DeclareVar(Foam lhs, int type)
{
	SExpr  name = gl0IdName(lhs);
	SExpr  ltype = typeIdentifier(type);
	return lisp2(GL_DeclareType, name, ltype);
}

local SExpr
gl0DeclareDefun(Foam lhs, Foam rhs)
{
	return sxCons(GL_DeclareFunction,gl0ProgType(lhs,rhs));
}


local SExpr
gl0ProgType(Foam lhs, Foam rhs)
{
	SExpr name, argTypes, retTypes, sx;
	Foam  params, *argv, format, decl;
	AInt type;
	int i;

	name    = gl0IdName(lhs);
#ifdef NEW_FORMATS
	params  = glvDFmt->foamDFmt.argv[rhs->foamProg.params];
#else
	params  = rhs->foamProg.params;
#endif
	argv    = params->foamDDecl.argv;
	argTypes = sxNil;

	/* parameters..*/
	for (i = 0; i < foamDDeclArgc(params); i++)
		argTypes = sxCons(lisp1(gl0Id(FOAM_Par,i,argv[i]->foamDecl.id),
					typeIdentifier(argv[i]->foamDecl.type)),
				  argTypes);

	argTypes = sxCons(lisp1( GL_e1, GL_Env), argTypes);
	argTypes = sxNReverse(argTypes);

	/* Return type */
	type = rhs->foamProg.retType;
        retTypes = sxNil;
	if (type == FOAM_NOp){
		/* multiple value return case */
		int fmt=rhs->foamProg.format;
		if (fmt==0)
			retTypes=sxNil;
		else {
			format = glvDFmt->foamDFmt.argv[fmt];
			for (i = 0; i < foamDDeclArgc(format); i++){
				decl = format->foamDDecl.argv[i];
				retTypes = 
				  sxCons(typeIdentifier(decl->foamDecl.type),
				         retTypes);
			}
		}
		retTypes = sxNReverse(retTypes);
	}
	/* single value return case */
	else retTypes = sxCons(typeIdentifier(type), sxNil);
	
	sx = lisp1(sxCons(name, retTypes), argTypes);
	return sx;
}

local SExpr
gliPCall(Foam foam)
{
	SExpr sx;
	int   foami;

	/*!! check for lisp protocol */
	sx = sxCons(gl0IdPlainName(foam->foamPCall.op), sxNil);
	/* slot 3 is the first function argument. */
	for (foami = 3; foami < foamArgc(foam); foami++)
		sx = sxCons(gliExpr(foamArgv(foam)[foami].code), sx);
	sx = sxNReverse(sx);

	return sx;
}

local SExpr
gl0OpenCall(Foam foam)
{
	SExpr   sx;
	int     foami;

	sx = sxCons(gl0IdName(foam->foamOCall.op), sxNil);
	/* slot 3 is the first function argument. */
	for (foami = 3; foami < foamArgc(foam); foami++)
		sx = sxCons(gliExpr(foamArgv(foam)[foami].code), sx);
	/* slot 2 is the environment. */
	sx = sxCons(gliExpr(foamArgv(foam)[2].code), sx);
	sx = sxNReverse(sx);

	return sx;
}

/* create a lisp DEFSTRUCT for a given Foam format */
local SExpr
gl0MakeDDecl(int formatIndex)
{
	Foam 	decl, format = glvDFmt->foamDFmt.argv[formatIndex];
	int 	i;
	SExpr	def, field, name, type;
	String	typeId;

	assert(foamTag(format) == FOAM_DDecl);

	if (formatIndex == envUsedSlot) return sxNil;
	if (foamDDeclArgc(format) == 0) return sxNil;

	def = sxNil;

	for (i=foamDDeclArgc(format)-1; i>=0; i--) {
		decl = format->foamDDecl.argv[i];
		assert(foamTag(decl) == FOAM_Decl);
		field = gl0FieldName(decl->foamDecl.id, i);
		typeId = foamStr(decl->foamDecl.type);
		type  = gl0typeName(typeId);
		def = sxCons(lisp1(field, type), def);
	}

	name = gl0StructName(formatIndex);
	return sxCons(GL_DDecl, sxCons(name, def));
}

local SExpr
gl0FieldName(String field, int i)
{
	String		buf;
	buf = strPrintf("%s-%d", field, i);
	return lispId(buf);
}

/*!! give it a unique name */
local SExpr
gl0StructName(int i)
{
	String 	buf;
	buf = strPrintf("Struct-%s-%d", glvFileName, i);
	return lispId(buf);
}

local SExpr
gl0MakeKeyword(String s)
{
	return sx0InternInFrString(s, sx0KeywordPackage);
}

#if 0
/* generate file initialization code */
local SExpr
gl0InitCode(Foam foam, SExpr defl)
{
	SExpr 	code, forms = sxNil, initFn, initEnv;
	String	initName;
	
	assert (foamTag(foam) == FOAM_Unit);

 	initFn = sxSecond(sxCar(defl));
	initName = sxiToString(sxSymbolName(initFn));
	initName[0] = 'E';
	initEnv = lispId(initName);

	/* set up call to initialization program. */
 	code   = lisp1(GL_CCall, initFn);
 	forms  = sxNReverse(sxCons(code, forms));
 	code   = sxCons(GL_EvalWhen, sxCons(lisp1(GL_Load, GL_Eval), forms));
	return sxCons(code, defl);
}
#endif

/* generate lisp strings for arrays. */
local SExpr
gliArr(Foam foam)
{
	String		s;
	int		i;
	assert(foamTag(foam) == FOAM_Arr);
	assert(foam->foamArr.baseType == FOAM_Char);
	
	s = strAlloc(foamArgc(foam));
	for(i=0; i < foamArgc(foam)-1; i++) s[i] = foam->foamArr.eltv[i];
	s[i] = 0;
	return lisp1(GL_MakeLit,sxiFrString(s));
}

local SExpr
gliEElt(Foam foam)
{
	int     lv    = foam->foamEElt.level;
	int     ix    = foam->foamEElt.lex;
	int     fi    = foam->foamEElt.env;
	Foam	ddecl = glvDFmt->foamDFmt.argv[fi];
	Foam    decl  = ddecl->foamDDecl.argv[ix];
	SExpr	envRef = gl0EnvRef(lv, gliExpr(foam->foamEElt.ref));
	String  buf;
	SExpr   access;

	assert (foamTag(ddecl) == FOAM_DDecl);
	assert (foamTag(decl)  == FOAM_Decl);
	assert (strlen(decl->foamDecl.id) < 180);

	buf = strPrintf("Struct-%s-%d-%s-%d", glvFileName,
			fi, decl->foamDecl.id, ix);
	access = lispId(buf);
	return lisp3(GL_EElt,access,sxiFrInteger(ix),envRef);
}

local SExpr
gl0EnvRef(int level, SExpr env)
{
	int i; 
	for(i=0; i< level; i++)
		env = lisp1(GL_EnvNext, env);
	return lisp1(GL_EnvLevel, env);
}

local SExpr
gliIf(Foam foam)
{
	glvHasGoto = 1;
	return lisp2(GL_When, gliExpr(foam->foamIf.test),
		     	lisp1(GL_Go,
			      lispId(strPrintf("Lab%d", foam->foamIf.label))));
}

local SExpr
gl0FlattenProgn(SExpr sx)
{
	SExpr r;
	if (sxCar(sx) != GL_Progn) return sx;
	r = sxCons(GL_Seq, sxNReverse(gl0FlattenProg1(sxCdr(sx))));
	return r;
}

local SExpr
gl0FlattenProg1(SExpr sx)
{
	SExpr	stmts = sxNil;
	if (sxiNull(sx)) return stmts;
	for(; !sxiNull(sx); sx = sxCdr(sx)) {
		if (sxiConsP(sxCar(sx)) && sxCar(sxCar(sx)) == GL_Progn) {
			stmts = sxNConc(gl0FlattenProg1(sxCdr(sxCar(sx))),
							stmts);
		}
		else
			stmts = sxCons(sxCar(sx), stmts);
	}
	return stmts;
}

local SExpr
gliSelect(Foam foam)
{
	int i, n = foamArgc(foam) - 1;
	SExpr	branches = sxNil, key;

	glvHasGoto = 1;
	key = gliExpr(foam->foamSelect.op);
	for(i=0; i < n; i++)
		branches = sxCons(lisp1(sxiFrInteger(i),
					lisp1(GL_Go,
		      lispId(strPrintf("Lab%d", foam->foamSelect.argv[i])))),
				  branches);
	return sxCons(GL_Case, sxCons(key, branches));
}

local SExpr
gliANew(Foam foam)
{
	return sxiList(3,
		       GL_ANew,
		       gl0typeName(foamInfo(foam->foamANew.eltType).str),
		       gliExpr(foam->foamANew.size));
}

local SExpr
gliRNew(Foam foam)
{
	SExpr name = gl0StructName(foam->foamRNew.format);
	return lisp1(GL_RNew, name);
}

local SExpr
gliAElt(Foam foam)
{
	return lisp2(GL_AElt, gliExpr(foam->foamAElt.expr),
		     gliExpr(foam->foamAElt.index));
}

local SExpr
gliRElt(Foam foam)
{
	int     ix    = foam->foamRElt.field;
	int     fi    = foam->foamRElt.format;
	Foam	ddecl = glvDFmt->foamDFmt.argv[fi];
	Foam    decl  = ddecl->foamDDecl.argv[ix];

	assert (foamTag(ddecl) == FOAM_DDecl);
	assert (foamTag(decl)  == FOAM_Decl);
	assert (strlen(decl->foamDecl.id) < 180);

	return lisp4(GL_RElt,
		     gl0StructName(fi),
		     lispId(decl->foamDecl.id),
		     sxiFrInteger(ix),
		     gliExpr(foam->foamRElt.expr));
}

local String
gl0ExtractFileName()
{	
	Foam decl = glvDConst->foamDDecl.argv[0];
	return strCopy(decl->foamDecl.id);
}
