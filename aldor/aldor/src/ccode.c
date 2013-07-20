/*****************************************************************************
 *
 * ccode.c: Structures for manipulating C programs.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * To Do:
 * -- ccoId instances shared.
 * -- ccoTypedefId as leaves.
 * -- ccoString/Char/Integer/Float with appropriate data types in nodes.
 */

#include "axlgen.h"
#include "ccode.h"
#include "debug.h"
#include "store.h"
#include "util.h"

CCode
ccoNewToken(CCodeTag tag, Symbol sym)
{
	CCode	cco;
	cco = (CCode) stoAlloc((int) OB_CCode, sizeof(struct ccoToken));
	cco->ccoToken.hdr.tag  = tag;
	cco->ccoToken.hdr.pos  = sposNone;
	cco->ccoToken.symbol   = sym;
	return cco;
}

CCode
ccoNewNode(CCodeTag tag, int argc)
{
	CCode	cco;
	cco = (CCode) stoAlloc((int) OB_CCode,
				fullsizeof(struct ccoNode, argc, CCode));
	cco->ccoNode.hdr.tag  = tag;
	cco->ccoNode.hdr.pos  = sposNone;
	ccoArgc(cco)	      = argc;
	return cco;
}

CCode
ccoNew(CCodeTag tag, int argc, ...)
{
	CCode	cco;
	va_list argp;
	int	i;

	va_start(argp, argc);

	if (ccoInfo(tag).kind == CCOK_Token) {
		cco = ccoNewToken(tag, va_arg(argp, Symbol));
	}
	else {
		cco = ccoNewNode(tag, argc);
		for (i = 0; i < argc; i++)
			ccoArgv(cco)[i] = va_arg(argp, CCode);
	}
	va_end(argp);

	return cco;
}


CCode
ccoCopy(CCode ccold)
{
	CCode	cco;
	int	i, argc;

	if (!ccold) return 0;

	cco = 0;	/* for lint */

	if (ccoIsToken(ccold)) {
		cco = ccoNewToken(ccoTag(ccold),
				  ccold->ccoToken.symbol);
	}
	else {
		argc = ccoArgc(ccold);
		cco  = ccoNewNode(ccoTag(ccold), argc);
		for (i = 0; i < argc; i++)
			ccoArgv(cco)[i] = ccoCopy(ccoArgv(ccold)[i]);
	}
	ccoPos(cco) = ccoPos(ccold);
	return cco;
}


Bool
ccoTypeEqual(CCode ccoA, CCode ccoB)
{
	int	nA,  nB;
	CCode	ccA, ccB;

	assert(ccoA);
	assert(ccoB);

	nA = ccoArgc(ccoA);
	nB = ccoArgc(ccoB);

	if (nA != nB) return false;

	ccA = ccoArgv(ccoA)[nA-1];
	ccB = ccoArgv(ccoB)[nB-1];

	assert(ccA);
	assert(ccB);

	if (ccoTag(ccA) != ccoTag(ccB)) return false;

	if (ccoIsToken(ccA) && ccoIsToken(ccB)) {
		String	sA, sB;
		sA  = symString(ccA->ccoToken.symbol);
		sB  = symString(ccB->ccoToken.symbol);
		return strEqual(sA, sB);
	}

	return false;
}


void
ccoFree(CCode cco)
{
	if (!cco) return;

	if (!ccoIsToken(cco)) {
		int i, argc = ccoArgc(cco);
		for (i = 0; i < argc; i++) ccoFree(ccoArgv(cco)[i]);
	}
	stoFree((Pointer) cco);
}


int
ccoHowMany(CCode cco)
{
	if (!cco) return 0;
	if (ccoTag(cco) == CCO_Many) return ccoArgc(cco);
	return 1;
}

/*****************************************************************************
 *
 * :: ccoPrint and subordinates
 *
 *****************************************************************************/

local int	ccoPrecMin;		/* Weakest precedence -- constant. */
local int	ccoPrecComma;		/* Comma   precedence -- constant. */

local CCodeMode ccoPrMode;		/* Output style: StandardC or OldC.*/
local FILE     *ccoPrFile;		/* Output stream set by ccoPrint.  */
local int	ccoPrMargin;		/* Current indentation in spaces.  */
local Bool	ccoInFunDecl;		/* Detect function prototypes */

local String	ccoFileName, ccoFileNamePrev;
local Length	ccoFileLine, ccoFileLinePrev;

# define	ccoLEFT_MARGIN	0
# define	ccoINDENT_BY	8
# define	ccoINDENT_LABEL (-ccoINDENT_BY)

local Length	ccoFileChar = ccoINDENT_BY; /* Char pos in current line. */

enum paramContext {
	CCOX_ProtoOrExpr,
	CCOX_HdParam,
	CCOX_HdDecl
};

local char BufferOutput[100000];
local int BufferPos = -1;
local int wrote_fputc = 0;

/*
 * Forward declarations.
 */

/* Printers which understand structure. */
local int	ccoPr	      (CCode);
local int	ccoPrExpr     (CCode, int outerPrec);
local int	ccoPrParam    (CCode, enum paramContext pcon, Bool isLast);
local int	ccoPrToken    (CCode);
local int	ccoPrMany     (CCode,int (*pfn)(CCode), char *sep, char *term);

/* Printers which determine layout. */
local void	ccoNoteSrcPos (CCode);
local int	ccoPrLn	      (CCode);
local int	ccoPrLnTab    (CCode);

local int	ccoPrOBrace   (void),
		ccoPrCBrace   (void);

local int	ccoPrBFDef    (CCode),
		ccoPrOFDef    (void),
		ccoPrMFDef    (void),
		ccoPrCFDef    (void),
		ccoPrCFDef    (void),
		ccoPrEFDef    (void);

local int	ccoPrMidStat  (void);
local int	ccoPrMidBefore(CCode);
local int	ccoPrNewline  (void);
local int	ccoPrNewlineDo(Bool);

/* Printers for actually doing output. */
local int	ccoPrintf     (const char *fmt, ...);
local int	ccoPuts	      (char *s);
local int	ccoPutc	      (int  c);

local int       ccoPutsFileOnly(char *s);

/* Misc functions. */
local Bool	ccoIsWantingElse(CCode);
local Bool	ccoIsWantingLabel(CCode);

#define	ccoMAX_LINE_LEN		78
#define ccoUseStandardC(mode)	(!((mode) & CCOM_OldC  ))
#define ccoUseLineNo(mode)	( ((mode) & CCOM_LineNo))

/*
 * External entry point to print CCode.
 */

int
ccoPrint(FILE *fout, CCode cco, CCodeMode mode)
{
	int	cc = 0;

	ccoPrFile	  = fout;
	ccoPrMode	  = mode;
	ccoPrMargin	  = ccoLEFT_MARGIN;
	ccoInFunDecl	  = false;

	ccoFileName	  = 0;
	ccoFileNamePrev	  = 0;
	ccoFileLine	  = 1;
	ccoFileLinePrev	  = 1;

	ccoPrecMin	  = ccoInfo(CCO_Stat).precedence;
	ccoPrecComma	  = ccoInfo(CCO_Comma).precedence;

	cc += ccoPr(cco);
	cc += ccoPrNewline();

	return cc;
}

int
ccoPrintDb(CCode cc)
{
	return ccoPrint(dbOut, cc, CCOM_StandardC);
}

local int
ccoPr(CCode cco)
{
	int	 cc;
	CCode	 *argv;
	CCodeTag tag;

	if (!cco) return 0;
	if (ccoIsExpr(cco)) return ccoPrExpr(cco, ccoPrecMin);
	ccoNoteSrcPos(cco);

	tag = ccoTag(cco);

	if (ccoIsKeywd(cco)) {
		if (!ccoUseStandardC(ccoPrMode) && tag == CCO_Volatile)
			return 0;
		if (!ccoUseStandardC(ccoPrMode) && tag == CCO_Const)
			return 0;
		return ccoPuts(ccoInfo(ccoTag(cco)).str);
	}

	cc   = 0;
	argv = ccoArgv(cco);

	switch (tag) {
	case CCO_Unit:
		cc += ccoPrMany(argv[0], ccoPrLn, "", "");
		break;
	case CCO_TypedefId:
		cc += ccoPr  (argv[0]);
		break;
	case CCO_StructRef:
		cc += ccoPuts("struct ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(" ");
		break;
	case CCO_StructDef:
		cc += ccoPuts("struct ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(" ");
		cc += ccoPrOBrace();
		cc += ccoPrMany(argv[1], ccoPrLn, "","");
		cc += ccoPrCBrace();
		break;
	case CCO_UnionRef:
		cc += ccoPuts("union ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(" ");
		break;
	case CCO_UnionDef:
		cc += ccoPuts("union ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(" ");
		cc += ccoPrOBrace();
		cc += ccoPrMany(argv[1], ccoPrLn, "","");
		cc += ccoPrCBrace();
		break;
	case CCO_EnumRef:
		cc += ccoPuts("enum ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(" ");
		break;
	case CCO_EnumDef:
		cc += ccoPuts("enum ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(" ");
		cc += ccoPrOBrace();
		cc += ccoPrMany(argv[1], ccoPrLn, ",","");
		cc += ccoPrCBrace();
		break;
	case CCO_Decl:
		BufferPos = 0; // start buffering the output

		cc += ccoPrMany(argv[0], ccoPr, "", "");
		cc += ccoPuts(" ");
		if (argv[1]) {
			/* Hack to identifier FCalls used as prototypes */
			Bool old = ccoInFunDecl;
			ccoInFunDecl = true;
			cc += ccoPrMany(argv[1], ccoPr, ", ","");
			cc += ccoPuts  (";");
			ccoInFunDecl = old;
		}

                // dump the buffered output and stop the buffering

//                cc += ccoPutsFileOnly("\n#if 0\n");
//                cc += ccoPutsFileOnly(BufferOutput);
//                cc += ccoPutsFileOnly("\n#endif\n");
#if 0
		if (strcmp(BufferOutput,"extern FiWord fputc();\n") == 0)
		{
	         	if (wrote_fputc == 0)
			{
			       cc += ccoPutsFileOnly("extern int fputc(); /* Signature patched in ccode.c:ccoPr */ \n");
				wrote_fputc = 1;
			}
		}
		else 
#endif
		
		if (strcmp(BufferOutput,"extern FiWord fputc();") == 0)
		{
	         	if (wrote_fputc == 0)
			{
	                	cc += ccoPutsFileOnly("extern int fputc(); /* Signature patched in ccode.c:ccoPr */");
				wrote_fputc = 1;
			}
		}
		else if (strcmp(BufferOutput,"extern FiWord fputc(FiWord P0, FiWord P1);") == 0)
		{
	         	if (wrote_fputc == 0)
			{
	                	cc += ccoPutsFileOnly("extern int fputc(FiWord P0, FiWord P1); /* Signature patched in ccode.c:ccoPr */");
				wrote_fputc = 1;
			}
		}
                else if (strcmp(BufferOutput,"extern FiWord fputs();") == 0)
                {
                        cc += ccoPutsFileOnly("extern int fputs(); /* Signature patched in ccode.c:ccoPr */");
                }
		else
		{
                	cc += ccoPutsFileOnly(BufferOutput);
		}
                BufferPos = -1;


		break;
	case CCO_FDef:
		cc += ccoPrBFDef(cco);

		cc += ccoPrMany(argv[0], ccoPr, "", "");
		cc += ccoPrOFDef();
		cc += ccoPr(argv[1]);

		cc += ccoPuts("(");
		cc += ccoPrParam(argv[2], CCOX_HdParam, true);
		cc += ccoPuts(")");

		cc += ccoPrParam(argv[2], CCOX_HdDecl,	true);
		cc += ccoPrCFDef();

		cc += ccoPrLnTab(argv[3]);
		cc += ccoPrEFDef();
		break;
	case CCO_Type:
		cc += ccoPr(argv[0]);
		if (argv[1]) cc += ccoPr(argv[1]);
		break;
	case CCO_BitField:
		cc += ccoPr(argv[0]);
		cc += ccoPuts(" : ");
		cc += ccoPr(argv[1]);
		break;
	case CCO_Init:
		cc += ccoPrOBrace();
		cc += ccoPrMany(argv[0], ccoPrLn, ",", "");
		cc += ccoPrCBrace();
		break;
	case CCO_Qual:
		cc += ccoPr(argv[0]);
		cc += ccoPr(argv[1]);
		break;
	case CCO_Label:
		cc += ccoPr  (argv[0]);
		ccoFileChar++;
		cc += ccoPuts(":\t");
		if (ccoIsWantingLabel(argv[1]))
			cc += ccoPrLn(argv[1]);
		else
			cc += ccoPr  (argv[1]);
		break;
	case CCO_Case:
		cc += ccoPuts("case ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(": ");
		if (ccoIsWantingLabel(argv[1]))
			cc += ccoPrLn(argv[1]);
		else
			cc += ccoPr  (argv[1]);
		break;
	case CCO_Default:
		cc += ccoPuts("default: ");
		cc += ccoPrLn(argv[0]);
		break;
	case CCO_Compound:
		cc += ccoPrOBrace();
		cc += ccoPrMany(argv[0], ccoPrLn, "", "");
		cc += ccoPrCBrace();
		break;
	case CCO_Stat:
		cc += ccoPrNewlineDo(true);

		ccoFileChar++;
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(";");
		break;
	case CCO_Goto:
		cc += ccoPuts("goto ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(";");
		break;
	case CCO_Continue:
		cc += ccoPuts("continue;");
		break;
	case CCO_Break:
		cc += ccoPuts("break;");
		break;
	case CCO_Return:
		cc += ccoPrNewlineDo(true);
		cc += ccoPuts("return ");
		if (ccoArgc(cco)) cc += ccoPr  (argv[0]);
		cc += ccoPuts(";");
		break;
	case CCO_If:
		cc += ccoPrNewlineDo(true);

		cc += ccoPuts("if (");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(") ");
		if (!argv[2])
			cc += ccoPr(argv[1]);
		else {
			if (ccoIsWantingElse(argv[1])) {
				cc += ccoPrOBrace();
				cc += ccoPrLn	 (argv[1]);
				cc += ccoPrCBrace();
			}
			else
				cc += ccoPrLnTab(argv[1]);
			cc += ccoPrMidStat();
			cc += ccoPuts("else ");
			cc += ccoPrLnTab(argv[2]);
		}
		break;
	case CCO_Switch:
		cc += ccoPrNewlineDo(true);

		cc += ccoPuts("switch (");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(") ");
		cc += ccoPrLnTab(argv[1]);
		break;
	case CCO_While:
		cc += ccoPrNewlineDo(true);

		cc += ccoPuts("while (");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(") ");
		cc += ccoPrLnTab(argv[1]);
		break;
	case CCO_Do:
		cc += ccoPrNewlineDo(true);

		cc += ccoPuts("do ");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts(" while (");
		cc += ccoPr  (argv[1]);
		cc += ccoPuts(");");
		break;
	case CCO_For:
		cc += ccoPrNewlineDo(true);

		cc += ccoPuts("for (");
		cc += ccoPr  (argv[0]);
		cc += ccoPuts("; ");
		cc += ccoPr  (argv[1]);
		cc += ccoPuts("; ");
		cc += ccoPr  (argv[2]);
		cc += ccoPuts(") ");
		cc += ccoPrLnTab(argv[3]);
		break;
	case CCO_Many:
		if (ccoArgc(cco))
			cc += ccoPrMany(cco, ccoPrLn, "", "");
		break;
	case CCO_Param:
		cc += ccoPrMany(argv[1], ccoPr, "", "");
		cc += ccoPuts(" ");
		ccoFileChar += 2;
		cc += ccoPr(argv[2]);
		cc += ccoPuts(", ");
		break;
	default:
		bugBadCase(tag);
	}

	return cc;
}

/*
 * Print an expression.
 *
 * Parentheses are inserted as needed based on the precedence of the
 *   containing node, oPrec, and the precedence of current node, iPrec.
 * The left- or right- associativity of an operator is used to bump up the
 *   precedence to force parentheses.
 */
local int
ccoPrExpr(CCode cco, int oPrec)
{
	int	  cc, iPrec;
	String	  str;
	Bool	  isLtoR;
	CCodeTag  tag;
	CCodeKind kind;
	CCode	  a;

	if (!cco) return 0;
	if (!ccoIsExpr(cco)) return ccoPr(cco);
	ccoNoteSrcPos(cco);

	tag    = ccoTag(cco);
	kind   = ccoInfo(tag).kind;
	iPrec  = ccoInfo(tag).precedence;
	isLtoR = ccoInfo(tag).isLeftToRight;
	str    = ccoInfo(tag).str;

	cc = 0;

	if (iPrec < oPrec) cc += ccoPuts("(");

	switch (kind) {
	case CCOK_Token:
		cc += ccoPrToken(cco);
		break;
	case CCOK_Prefix:
		cc += ccoPuts  (str);
		cc += ccoPrExpr(ccoArgv(cco)[0], iPrec);
		break;
	case CCOK_Postfix:
		cc += ccoPrExpr(ccoArgv(cco)[0], iPrec);
		cc += ccoPuts  (str);
		break;
	case CCOK_Infix:
		ccoFileChar += strlen(str);
		cc += ccoPrExpr(ccoArgv(cco)[0], iPrec +!isLtoR);
		cc += ccoPuts  (str);
		cc += ccoPrExpr(ccoArgv(cco)[1], iPrec + isLtoR);
		break;
	case CCOK_Misc:
		switch (tag) {
		case CCO_Paren:
			cc += ccoPuts  ("(");
			cc += ccoPrExpr(ccoArgv(cco)[0], ccoPrecMin);
			cc += ccoPuts  (")");
			break;
		case CCO_Quest:
			cc += ccoPrExpr(ccoArgv(cco)[0],iPrec);
			cc += ccoPuts  (" ? ");
			cc += ccoPrExpr(ccoArgv(cco)[1],iPrec);
			cc += ccoPuts  (" : ");
			cc += ccoPrExpr(ccoArgv(cco)[2],iPrec);
			break;
		case CCO_Cast:
			cc += ccoPuts  ("(");
			cc += ccoPr    (ccoArgv(cco)[0]);
			cc += ccoPuts  (") ");
			cc += ccoPrExpr(ccoArgv(cco)[1], iPrec);
			break;
		case CCO_Sizeof:
			a   = ccoArgv(cco)[0];
			cc += ccoPuts  ("sizeof");
			if (a && (ccoTag(a) == CCO_Type || ccoTag(a) == CCO_TypedefId)) {
				cc += ccoPuts  ("(");
				cc += ccoPr    (a);
				cc += ccoPuts  (")");
			}
			else {
				cc += ccoPuts  (" ");
				cc += ccoPrExpr(a, iPrec);
			}
			break;
		case CCO_ARef:
			cc += ccoPrExpr(ccoArgv(cco)[0], iPrec);
			cc += ccoPuts  ("[");
			cc += ccoPrExpr(ccoArgv(cco)[1], ccoPrecMin);
			cc += ccoPuts  ("]");
			break;
		case CCO_FCall:
			cc += ccoPrExpr (ccoArgv(cco)[0], iPrec);
			cc += ccoPuts	("(");
			cc += ccoPrParam(ccoArgv(cco)[1],CCOX_ProtoOrExpr,true);
			cc += ccoPuts	(")");
			break;
		case CCO_Comment:
			cc += ccoPuts  ("/* ");
			cc += ccoPrExpr(ccoArgv(cco)[0], iPrec);
			cc += ccoPuts  (" */");
			break;
		case CCO_CppLine:
			cc += ccoPuts("#");
			cc += ccoPr  (ccoArgv(cco)[0]);
			cc += ccoPuts(" ");
			cc += ccoPr  (ccoArgv(cco)[1]);
			cc += ccoPrLn(NULL);
			break;
		  default:
			break;
		}
		break;
	default:
		bugBadCase(kind);
	}

	if (iPrec < oPrec) cc += ccoPuts(")");

	return cc;
}


/*
 * Print the arguments to a function head, prototype or function call.
 */
local int
ccoPrParam(CCode cco, enum paramContext pcon, Bool isLast)
{
	int		cc, i, argc;
	CCode		*argv;
	CCodeTag	tag;

	if (!cco) return 0;

	tag  = ccoTag (cco);
	argc = ccoArgc(cco);
	argv = ccoArgv(cco);

	cc = 0;

	switch (tag) {
	case CCO_Many:
		for (i = 0; i < argc; i++)
			cc += ccoPrParam(argv[i], pcon, i == argc-1 && isLast);
		break;
	case CCO_Param:
		if (ccoUseStandardC(ccoPrMode)) {
			switch (pcon) {
			case CCOX_ProtoOrExpr:
			case CCOX_HdParam:
				cc += ccoPrMany(argv[1], ccoPr, "", "");
				cc += ccoPuts(" ");
				if (!isLast) ccoFileChar += 2;
				cc += ccoPr(argv[2]);
				if (!isLast) cc += ccoPuts(", ");
				break;
			case CCOX_HdDecl:
				break;
			}
		}
		else {
			switch (pcon) {
			case CCOX_ProtoOrExpr:
				break;
			case CCOX_HdParam:
				cc += ccoPr(argv[0]);
				if (!isLast) cc += ccoPuts(", ");
				break;
			case CCOX_HdDecl:
				if (ccoHowMany(argv[1]) > 0) {
					cc += ccoPrMFDef();
					cc += ccoPrMany(argv[1], ccoPr, "", "");
					cc += ccoPuts(" ");
					ccoFileChar++;
					cc += ccoPr(argv[2]);
					cc += ccoPuts(";");
				}
				break;
			}
		}
		break;
	case CCO_VAParam:
		if (ccoUseStandardC(ccoPrMode) && pcon != CCOX_HdDecl)
			cc += ccoPuts("...");
		break;
	default:
		cc += ccoPrExpr(cco, ccoPrecComma + 1);
		if (!isLast) cc += ccoPuts(", ");
		break;
	}
	return cc;
}

local int
ccoPrToken(CCode cco)
{
	int	 q, c, cc = 0;
	char	 *s  = symString(cco->ccoToken.symbol);
	CCodeTag tag = ccoTag(cco);

	if (tag != CCO_StringVal && tag != CCO_CharVal) {
		if (ccoFileChar + strlen(s) > ccoMAX_LINE_LEN) {
			ccoPrNewlineDo(false);
			ccoPuts("\t");
			ccoFileChar += ccoPrMargin;
		}
		cc += ccoPuts(s);
	}
	else {
		q = (tag == CCO_StringVal) ? '\"' : '\'';

		ccoPutc(q), cc++;
		for (c = *s++; c; c = *s++)
			switch (c) {
			case '\n':
				ccoPutc('\\'); ccoPutc('n');  cc += 2; break;
			case '\t':
				ccoPutc('\\'); ccoPutc('t');  cc += 2; break;
			case '\v':
				ccoPutc('\\'); ccoPutc('v');  cc += 2; break;
			case '\b':
				ccoPutc('\\'); ccoPutc('b');  cc += 2; break;
			case '\r':
				ccoPutc('\\'); ccoPutc('r');  cc += 2; break;
			case '\f':
				ccoPutc('\\'); ccoPutc('f');  cc += 2; break;
			case '\"':
				ccoPutc('\\'); ccoPutc('\"'); cc += 2; break;
			case '\'':
				ccoPutc('\\'); ccoPutc('\''); cc += 2; break;
			case '\\':
				ccoPutc('\\'); ccoPutc('\\'); cc += 2; break;
			case '?':
				if (ccoUseStandardC(ccoPrMode)) ccoPutc('\\'), cc++;
				ccoPutc('?'), cc++;
				break;
			/*
			 * Give \a in octal in default, to allow non-ANSI
			 * compilers to handle this program.  Otherwise
			 * case '\a' may be equivalent to case 'a'.
			 */
			default:
				if (isprint(c))
					ccoPutc(c), cc++;
				else
					cc += ccoPrintf("\\%#o", c);
				break;
			}
		ccoPutc(q), cc++;
	}
	return cc;
}

/*
 * Functions for printing groups.
 */

local int
ccoPrMany(CCode cco, int (*prfn)(CCode), char *sep, char *term)
{
	int	argc, argf, cc = 0, i;
	CCode	*argv;

	if (!cco)
		;
	else if (ccoTag(cco) != CCO_Many)  {
		cc += (*prfn)(cco);
		cc += ccoPuts(term);
	}
	else {
		argc = ccoArgc(cco);
		argv = ccoArgv(cco);
		argf = argc-1;

		if (argc == 0) return 0;

		for (i = 0; i < argf; i++) {
			ccoFileChar++;
			cc += ccoPrMidBefore(argv[i]);
			cc += (*prfn)(argv[i]);
			cc += ccoPuts(sep);
		}
		cc += ccoPrMidBefore(argv[i]);
		cc += (*prfn)(argv[i]);
		cc += ccoPuts(term);
	}
	return cc;
}

/*
 * Layout Functions:
 *    ccoNoteSrcPos,
 *    ccoPrLn, ccoPrLnTab,
 *
 *    ccoPrNewline,
 *    ccoPrOBrace, ccoPrCBrace,
 *    ccoPrBFDef,  ccoPrOFDef,  ccoPrMFDef, ccoPrCFDef, ccoPrEFDef,
 *    ccoPrMidStat,ccoPrMidBefore.
 *
 * Functions to break the current line and print on the next line,
 * handle brace style, label outdenting etc.
 *
 * ALL newlines and margin modifications from ccoPrint come from
 * these functions.
 */

local void
ccoNoteSrcPos(CCode cco)
{
	SrcPos	spos = ccoPos(cco);

	if (ccoUseLineNo(ccoPrMode) && !sposIsSpecial(spos)) {
		FileName fn    = sposFile(spos);
		String	 fname = fn ? fnameUnparseStatic(fn) : NULL;
		Length	 lno   = sposLine(spos);

		if (fname && fname[0]) {
			if (!(ccoFileName && strEqual(ccoFileName, fname)))
				ccoFileName = strCopy(fname);
			ccoFileLine = lno;
		}
	}
}

local int
ccoPrLn(CCode cco)
{
	int	 cc;
	CCodeTag tag;

	if (!cco) return ccoPrNewline();

	tag = ccoTag(cco);

	if (tag == CCO_Compound) {
		ccoPrMargin -= ccoINDENT_BY;
		cc	     = ccoPr(cco);
		ccoPrMargin += ccoINDENT_BY;
	}
	else if (tag == CCO_Label || tag == CCO_Case || tag == CCO_Default) {
		ccoPrMargin += ccoINDENT_LABEL;
		cc	     = ccoPrNewline();
		ccoPrMargin -= ccoINDENT_LABEL;
		cc	    += ccoPr(cco);
	}
	else {
		cc  = ccoPrNewline();
		cc += ccoPr(cco);
	}
	return cc;
}

local int
ccoPrLnTab(CCode cco)
{
	int	cc;
	ccoPrMargin += ccoINDENT_BY;
	cc	     = ccoPrLn(cco);
	ccoPrMargin -= ccoINDENT_BY;
	return cc;
}

/* Open brace for compound statement, struct, init, etc. */
local int
ccoPrOBrace(void)
{
	int	cc;
	ccoPrMargin += ccoINDENT_BY;
	cc	     = ccoPuts("{");
	return cc;
}

/* Close brace for compound statement, struct, init, etc. */
local int
ccoPrCBrace(void)
{
	int cc;
	ccoPrMargin -= ccoINDENT_BY;
	cc	     = ccoPrNewline();
	cc	    += ccoPuts("}");
	return cc;
}

/*
 * <BFDef> char * <OFDef> foo (a,b,c) <MFDef>int a;<MFDef>int b;<CFDef>
 * {...} <EFDef>
 */
local int
ccoPrBFDef(CCode ccode)
{
	/* Could keep track of line renumber information here. */
	return ccoPrNewlineDo(true);
}

local int
ccoPrOFDef(void)
{
	int cc;
	cc	     = ccoPrNewline();
	ccoPrMargin += ccoINDENT_BY;
	return cc;
}

local int
ccoPrMFDef(void)
{
	return ccoUseStandardC(ccoPrMode) ? 0 : ccoPrNewline();
}

local int
ccoPrCFDef(void)
{
	ccoPrMargin -= ccoINDENT_BY;
	return ccoPrNewline();
}

local int
ccoPrEFDef(void)
{
	return 0;
}

/* if ( ) a; <MidStat> else b;	   do a; <MidStat> while ( ); */
local int
ccoPrMidStat(void)
{
	return ccoPrNewline();
}


/* <MidBefore> a; <MidBefore> b; <MidBefore> c; */

local int
ccoPrMidBefore(CCode cco)
{
	if (!cco) return 0;

	if (ccoTag(cco) == CCO_FDef || ccoTag(cco) == CCO_StructDef)
		return ccoPrNewline();
	else
		return 0;
}

local int
ccoPrNewline(void)
{
	return ccoPrNewlineDo(false);
}

/* This is the ONLY call which puts out \n. */
local int
ccoPrNewlineDo(Bool infile)
{
	int	cc = 0, line;

	if (!ccoUseLineNo(ccoPrMode)) {
		if (!infile) {
			cc += ccoPrintf("\n%*s", ccoPrMargin, "");
			ccoFileChar = ccoPrMargin;
		}
		return cc;
	}

	line = ccoFileLine;

	if (infile) {
		cc += ccoPrintf("\n#line %d", line);
		if (ccoFileName && !strEqual(ccoFileName, "")) {
			if (!ccoFileNamePrev ||
			    !strEqual(ccoFileNamePrev, ccoFileName))
				cc += ccoPrintf(" \"%s\"", ccoFileName);
		}
		ccoFileLinePrev = ccoFileLine;
		ccoFileNamePrev = ccoFileName;
	}
	cc += ccoPrintf("\n%*s", ccoPrMargin, "");

	ccoFileChar     = ccoPrMargin;

	return cc;
}

/*
 * Actual output functions:
 *    ccoPrintf, ccoPuts
 */
/*
local int
ccoPrintf(const char *fmt, ...)
{
	va_list argp;
	int	cc;

	va_start(argp, fmt);
	cc = vfprintf(ccoPrFile, fmt, argp);
	va_end(argp);

	ccoFileChar += cc;
	return cc;
}

local int
ccoPuts(char *s)
{
	ccoFileChar += strlen(s);
	return fputs(s, ccoPrFile);
}

local int
ccoPutc(int c)
{
	ccoFileChar++;
	return fputc(c, ccoPrFile);
}
*/
local int
ccoPrintf(const char *fmt, ...)
{
        va_list argp;
        int     cc;

        if (BufferPos >= 0)
        {
                va_start(argp, fmt);
                BufferPos += vsprintf(&BufferOutput[BufferPos], fmt, argp);
                va_end(argp);

                return 0;
        }
        else
        {
                va_start(argp, fmt);
                cc = vfprintf(ccoPrFile, fmt, argp);
                va_end(argp);

                ccoFileChar += cc;
                return cc;
        }
}

local int
ccoPuts(char *s)
{
        if (BufferPos >= 0)
        {
                strcpy(&BufferOutput[BufferPos], s);
                BufferPos += strlen(s);
                return 0;
        }
        else
        {
                ccoFileChar += strlen(s);
                return fputs(s, ccoPrFile);
        }
}

local int
ccoPutsFileOnly(char *s)
{
                ccoFileChar += strlen(s);
                return fputs(s, ccoPrFile);
}

local int
ccoPutc(int c)
{
        if (BufferPos >= 0)
        {
                BufferOutput[BufferPos++] = c;
                return 0;
        }
        else
        {
                ccoFileChar++;
                return fputc(c, ccoPrFile);
        }
}





/*
 * Determine whether this statement will eat a dangling else.
 */
local Bool
ccoIsWantingElse(CCode cco)
{
	for (;;) {
		if (!cco) return false;

		switch (ccoTag(cco)) {
		case CCO_If:
			if (!ccoArgv(cco)[2]) return true;
			/* Seep through. */
		case CCO_Switch:
		case CCO_While:
		case CCO_For:
			cco = ccoArgv(cco)[ccoArgc(cco)-1];
			break;
		default:
			return false;
		}
	}
}

local Bool
ccoIsWantingLabel(CCode cco)
{
	for (;;) {
		if (!cco) return false;

		switch (ccoTag(cco)) {
		case CCO_Label:
		case CCO_Case:
		case CCO_Default:
			return true;
		default:
			return false;
		}
	}
}

/******************************************************************************
 *
 * :: Table of information about each CCode node type.
 *
 *****************************************************************************/

/*
 * See the comments in the enumeration definition for CCodeTag.
 */
struct cco_info ccoInfoTable[] = {
	{ CCO_Unit,	CCOK_Misc,    0, 0, 0	       },

	{ CCO_Auto,	CCOK_Keywd,   0, 0, "auto "    },
	{ CCO_Register, CCOK_Keywd,   0, 0, "register "},
	{ CCO_Static,	CCOK_Keywd,   0, 0, "static "  },
	{ CCO_Extern,	CCOK_Keywd,   0, 0, "extern "  },
	{ CCO_Typedef,	CCOK_Keywd,   0, 0, "typedef " },

	{ CCO_Const,	CCOK_Keywd,   0, 0, "const "   },
	{ CCO_Volatile, CCOK_Keywd,   0, 0, "volatile "},

	{ CCO_Void,	CCOK_Keywd,   0, 0, "void "    },
	{ CCO_Char,	CCOK_Keywd,   0, 0, "char "    },
	{ CCO_Short,	CCOK_Keywd,   0, 0, "short "   },
	{ CCO_Int,	CCOK_Keywd,   0, 0, "int "     },
	{ CCO_Long,	CCOK_Keywd,   0, 0, "long "    },
	{ CCO_Float,	CCOK_Keywd,   0, 0, "float "   },
	{ CCO_Double,	CCOK_Keywd,   0, 0, "double "  },
	{ CCO_Signed,	CCOK_Keywd,   0, 0, "signed "  },
	{ CCO_Unsigned, CCOK_Keywd,   0, 0, "unsigned "},
	{ CCO_TypedefId,CCOK_Misc,    0, 0, 0	       },

	{ CCO_StructRef,CCOK_Misc,    0, 0, "struct"   },
	{ CCO_StructDef,CCOK_Misc,    0, 0, "struct"   },
	{ CCO_UnionRef, CCOK_Misc,    0, 0, "union"    },
	{ CCO_UnionDef, CCOK_Misc,    0, 0, "union"    },
	{ CCO_EnumRef,	CCOK_Misc,    0, 0, "enum"     },
	{ CCO_EnumDef,	CCOK_Misc,    0, 0, "enum"     },

	{ CCO_Param,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_VAParam,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Decl,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_FDef,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Type,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_BitField, CCOK_Misc,    0, 0, 0	       },
	{ CCO_Init,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Qual,	CCOK_Misc,    0, 0, 0	       },

	{ CCO_Label,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Case,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Default,	CCOK_Misc,    0, 0, 0	       },

	{ CCO_Compound, CCOK_Misc,    0, 0, 0	       },
	{ CCO_Stat,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Goto,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Continue, CCOK_Misc,    0, 0, 0	       },
	{ CCO_Break,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Return,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_If,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Switch,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_While,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_Do,	CCOK_Misc,    0, 0, 0	       },
	{ CCO_For,	CCOK_Misc,    0, 0, 0	       },

	{ CCO_Comma,	CCOK_Infix,   1, 1, ", "       },
	{ CCO_Asst,	CCOK_Infix,   2, 0, " = "      },
	{ CCO_StarAsst, CCOK_Infix,   2, 0, " *= "     },
	{ CCO_DivAsst,	CCOK_Infix,   2, 0, " /= "     },
	{ CCO_ModAsst,	CCOK_Infix,   2, 0, " %= "     },
	{ CCO_PlusAsst, CCOK_Infix,   2, 0, " += "     },
	{ CCO_MinusAsst,CCOK_Infix,   2, 0, " -= "     },
	{ CCO_UShAsst,	CCOK_Infix,   2, 0, " <<= "    },
	{ CCO_DShAsst,	CCOK_Infix,   2, 0, " >>= "    },
	{ CCO_AndAsst,	CCOK_Infix,   2, 0, " &= "     },
	{ CCO_XorAsst,	CCOK_Infix,   2, 0, " ^= "     },
	{ CCO_OrAsst,	CCOK_Infix,   2, 0, " |= "     },
	{ CCO_Quest,	CCOK_Misc,    3, 0, 0	       },
	{ CCO_LOr,	CCOK_Infix,   4, 1, " || "     },
	{ CCO_LAnd,	CCOK_Infix,   5, 1, " && "     },
	{ CCO_Or,	CCOK_Infix,   6, 1, " | "      },
	{ CCO_Xor,	CCOK_Infix,   7, 1, " ^ "      },
	{ CCO_And,	CCOK_Infix,   8, 1, " & "      },
	{ CCO_EQ,	CCOK_Infix,   9, 1, " == "     },
	{ CCO_NE,	CCOK_Infix,   9, 1, " != "     },
	{ CCO_LT,	CCOK_Infix,  10, 1, " < "      },
	{ CCO_LE,	CCOK_Infix,  10, 1, " <= "     },
	{ CCO_GT,	CCOK_Infix,  10, 1, " > "      },
	{ CCO_GE,	CCOK_Infix,  10, 1, " >= "     },
	{ CCO_USh,	CCOK_Infix,  11, 1, " << "     },
	{ CCO_DSh,	CCOK_Infix,  11, 1, " >> "     },
	{ CCO_Plus,	CCOK_Infix,  12, 1, " + "      },
	{ CCO_Minus,	CCOK_Infix,  12, 1, " - "      },
	{ CCO_Star,	CCOK_Infix,  13, 1, " * "      },
	{ CCO_Div,	CCOK_Infix,  13, 1, " / "      },
	{ CCO_Mod,	CCOK_Infix,  13, 1, " % "      },
	{ CCO_Cast,	CCOK_Misc,   14, 0, 0	       },
	{ CCO_Sizeof,	CCOK_Misc,   14, 0, 0	       },
	{ CCO_Not,	CCOK_Prefix, 14, 0, "~"	       },
	{ CCO_LNot,	CCOK_Prefix, 14, 0, "!"	       },
	{ CCO_PreAnd,	CCOK_Prefix, 14, 0, "&"	       },
	{ CCO_PreStar,	CCOK_Prefix, 14, 0, "*"	       },
	{ CCO_PrePlus,	CCOK_Prefix, 14, 0, "+"	       },
	{ CCO_PreMinus, CCOK_Prefix, 14, 0, "-"	       },
	{ CCO_PreInc,	CCOK_Prefix, 14, 0, "++"       },
	{ CCO_PreDec,	CCOK_Prefix, 14, 0, "--"       },
	{ CCO_PostInc,	CCOK_Postfix,14, 0, "++"       },
	{ CCO_PostDec,	CCOK_Postfix,14, 0, "--"       },
	{ CCO_PostStar,	CCOK_Postfix,14, 0, "*"       },
	{ CCO_ARef,	CCOK_Misc,   15, 1, 0	       },
	{ CCO_FCall,	CCOK_Misc,   15, 1, 0	       },
	{ CCO_Dot,	CCOK_Infix,  15, 1, "."	       },
	{ CCO_PointsTo, CCOK_Infix,  15, 1, "->"       },
	{ CCO_Paren,	CCOK_Misc,   20, 0, 0	       },

	{ CCO_StringVal,CCOK_Token,  20, 0, 0	       },
	{ CCO_CharVal,	CCOK_Token,  20, 0, 0	       },
	{ CCO_IntVal,	CCOK_Token,  20, 0, 0	       },
	{ CCO_FloatVal, CCOK_Token,  20, 0, 0	       },
	{ CCO_EnumId,	CCOK_Token,  20, 0, 0	       },
	{ CCO_Id,	CCOK_Token,  20, 0, 0	       },

	{ CCO_CppLine,	CCOK_Misc,   20, 0, 0	       },
	{ CCO_Comment,	CCOK_Misc,   20, 0, 0	       },
	{ CCO_Many,	CCOK_Misc,    0, 0, 0	       },

	{ CCO_LIMIT,	CCOK_Misc,    0, 0, 0	       }
};





