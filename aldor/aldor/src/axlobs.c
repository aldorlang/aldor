/*****************************************************************************
 *
 * axlobs.c: General operations for compiler-specific types.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "btree.h"
#include "opsys.h"
#include "store.h"
#include "util.h"

/*
 * Initialize specific list parameterizations.
 */
CREATE_LIST(Hash);
CREATE_LIST(Symbol);
CREATE_LIST(SExpr);

CREATE_LIST(CoMsg);
CREATE_LIST(SrcLine);
CREATE_LIST(Token);
CREATE_LIST(AbSyn);
CREATE_LIST(AbBind);
CREATE_LIST(TForm);
CREATE_LIST(TConst);
CREATE_LIST(TQual);
CREATE_LIST(Stab);
CREATE_LIST(StabLevel);
CREATE_LIST(Syme);
CREATE_LIST(Sefo);
CREATE_LIST(Table);
CREATE_LIST(Foam);
CREATE_LIST(AInt);
CREATE_LIST(FileName);
CREATE_LIST(CCode);
CREATE_LIST(DepDag);

CREATE_LIST(SymeList);



/*
 * These must be in the same order as the OB_ values..
 */
struct ob_info	obInfo[] = {
	{OB_Other,	"OB_Other", 	1},
	{OB_Bogus,	"OB_Bogus",	1},

	{OB_BInt,	"OB_BInt",	0},
	{OB_BTree,	"OB_BTree",	1},
	{OB_Bitv,	"OB_Bitv",	0},
	{OB_Buffer,	"OB_Buffer",	1},
	{OB_List,	"OB_List",	1},
	{OB_String,	"OB_String",	1},
	{OB_Symbol,	"OB_Symbol",	1},
	{OB_Table,	"OB_Table",	1},
	{OB_DNF,	"OB_DNF",	1},
	{OB_CCode,	"OB_CCode",	1},
	{OB_JCode,	"OB_JCode",	1},
	{OB_SExpr,	"OB_SExpr",	1},

	{OB_CoMsg,	"OB_CoMsg",	1},
	{OB_SrcLine,	"OB_SrcLine",	1},
	{OB_Token,	"OB_Token",	1},
	{OB_Doc,	"OB_Doc",	1},
	{OB_AbSyn,	"OB_AbSyn",	1},
	{OB_AbBind,	"OB_AbBind",	1},
	{OB_Stab,	"OB_Stab",	1},
	{OB_Syme,	"OB_Syme",	1},
	{OB_TForm,	"OB_TForm",	1},
	{OB_TPoss,	"OB_TPoss",	1},
	{OB_TConst,	"OB_TConst",	1},
	{OB_TQual,	"OB_TQual",	1},
	{OB_Foam,	"OB_Foam",	1},
	{OB_Lib,	"OB_Lib",	1},
	{OB_Archive,	"OB_Archive",	1},
	{ -1, NULL, 1}
};

# define   obCode(ob)		stoCode(ob)

int
obPrint(FILE *fout, Pointer ob)
{
	if (!ob) return fprintf(fout, "OB_0");

	switch (obCode(ob)) {
	case OB_Bogus:	 bug("Bad object.");
	case OB_BTree:	 return btreePrint(fout, (BTree) ob);
	case OB_BInt:	 return bintPrint(fout,	 (BInt) ob);
	case OB_Bitv:	 return bitvPrint(fout, (BitvClass) NULL, (Bitv) ob);
	case OB_String:	 return strPrint(fout,	 (String) ob,
				'"', '"', '\\', "\\%#x");
	case OB_Buffer:	 return bufPrint(fout,	 (Buffer) ob);
	case OB_List:	 return listPrint(Pointer)(fout, (PointerList) ob,
				obPrint);
	case OB_Table:	 return tblPrint(fout, (Table) ob,
				(TblPrKeyFun) obPrint, (TblPrEltFun) obPrint);
	case OB_Symbol:	 return symPrint(fout, (Symbol) ob);
	case OB_SExpr:	 return sxiWrite(fout, (SExpr) ob, SXRW_Default);
	case OB_CCode:	 return ccoPrint(fout, (CCode) ob, CCOM_StandardC);
	case OB_JCode:	 return fprintf(fout, "java");

	case OB_SrcLine: return slinePrint(fout, (SrcLine) ob);
	case OB_Token:	 return tokPrint(fout, (Token) ob);
	case OB_Doc:	 return docPrint(fout, (Doc) ob);
	case OB_AbSyn:	 return abPrint(fout, (AbSyn) ob);
	case OB_Syme:	 return symePrint(fout, (Syme) ob);
	case OB_TForm:	 return tfPrint(fout, (TForm) ob);
	case OB_TPoss:	 return tpossPrint(fout, (TPoss) ob);
	case OB_TConst:	 return tcPrint(fout, (TConst) ob);
	case OB_TQual:	 return tqPrint(fout, (TQual) ob);
	case OB_Foam:	 return foamPrint(fout, (Foam) ob);

	case OB_Lib:
	case OB_Archive:
#if EDIT_1_0_n1_07
	default:	 return fprintf(fout, "%p->OB_%d", ob, obCode(ob));
#else
	default:	 return fprintf(fout, "%#x->OB_%d",
					(unsigned int)ob, obCode(ob));
#endif
	}
}

void
obInit()
{
	int i;
	StoInfoObj obj;
	
	i = 0;
	while (obInfo[i].code < OB_LIMIT) {
		obj.code    = obInfo[i].code;
		obj.hasPtrs = obInfo[i].hasPtrs;
		i++;
		stoRegister(&obj);
	}
}


/******************************************************************************
 *
 * :: Phases
 *
 ****************************************************************************/

union symCoInfoU *
symCoInfoNew(void)
{
	union symCoInfoU * info;

	info = (union symCoInfoU *) stoAlloc(OB_Other, sizeof(*info));

	info->val.phaseVal.generic = 0;
	info->val.abTagVal	   = AB_LIMIT;
	info->val.foamTagVal	   = FOAM_BVAL_LIMIT;

	return info;
}

/******************************************************************************
 *
 * :: File Types
 *
 ****************************************************************************/

FTypeNo
ftypeNo(String ftype)
{
	if (!ftype)				return FTYPENO_NONE;
	if (ftypeEqual(ftype, FTYPE_SRC))	return FTYPENO_SRC;
	if (ftypeEqual(ftype, FTYPE_INCLUDED))	return FTYPENO_INCLUDED;
	if (ftypeEqual(ftype, FTYPE_ABSYN))	return FTYPENO_ABSYN;
	if (ftypeEqual(ftype, FTYPE_OLDABSYN))	return FTYPENO_OLDABSYN;
	if (ftypeEqual(ftype, FTYPE_INTERMED))	return FTYPENO_INTERMED;
	if (ftypeEqual(ftype, FTYPE_FOAMEXPR))	return FTYPENO_FOAMEXPR;
	if (ftypeEqual(ftype, FTYPE_SYMEEXPR))	return FTYPENO_SYMEEXPR;
	if (ftypeEqual(ftype, FTYPE_LOCK))	return FTYPENO_LOCK;
	if (ftypeEqual(ftype, FTYPE_LISP))	return FTYPENO_LISP;
	if (ftypeEqual(ftype, FTYPE_C))		return FTYPENO_C;
	if (ftypeEqual(ftype, FTYPE_CPP))	return FTYPENO_CPP;
	if (ftypeEqual(ftype, FTYPE_H))		return FTYPENO_H;
	if (ftypeEqual(ftype, FTYPE_OBJECT))	return FTYPENO_OBJECT;
	if (ftypeEqual(ftype, FTYPE_AR_OBJ))	return FTYPENO_AR_OBJ;
	if (ftypeEqual(ftype, FTYPE_AR_INT))	return FTYPENO_AR_INT;
	if (ftypeEqual(ftype, FTYPE_MSG))	return FTYPENO_MSG;
	if (ftypeEqual(ftype, FTYPE_EXEC))	return FTYPENO_EXEC;
	if (ftypeEqual(ftype, ""))              return FTYPENO_NONE;

	return FTYPENO_OTHER;
}

String
ftypeString(FTypeNo ftype)
{
	switch (ftype) {
	case FTYPENO_SRC:	return FTYPE_SRC;
	case FTYPENO_INCLUDED:	return FTYPE_INCLUDED;
	case FTYPENO_ABSYN:	return FTYPE_ABSYN;
	case FTYPENO_OLDABSYN:	return FTYPE_OLDABSYN;
	case FTYPENO_INTERMED:	return FTYPE_INTERMED;
	case FTYPENO_FOAMEXPR:	return FTYPE_FOAMEXPR;
	case FTYPENO_SYMEEXPR:	return FTYPE_SYMEEXPR;
	case FTYPENO_LOCK:	return FTYPE_LOCK;
	case FTYPENO_LISP:	return FTYPE_LISP;
	case FTYPENO_C:		return FTYPE_C;
	case FTYPENO_CPP:	return FTYPE_CPP;
	case FTYPENO_H:		return FTYPE_H;
	case FTYPENO_OBJECT:	return FTYPE_OBJECT;
	case FTYPENO_AR_OBJ:	return FTYPE_AR_OBJ;
	case FTYPENO_AR_INT:	return FTYPE_AR_INT;
	case FTYPENO_MSG:	return FTYPE_MSG;
	case FTYPENO_EXEC:	return FTYPE_EXEC;
	case FTYPENO_AXLMAINC:	return FTYPE_AXLMAINC;
	case FTYPENO_NONE:	return "";
	default:
		bugBadCase(ftype);
		NotReached(return "");
	}
}

/*
 * Allow file types to be equal independent of case.
 */
Bool
ftypeEqual(String ft1, String ft2)
{
	return strAEqual(ft1, ft2);
}

Bool
ftypeIs(String ft, FTypeNo fno)
{
	return ftypeEqual(ft, ftypeString(fno));
}

Bool
ftypeHas(FileName fn, FTypeNo fno)
{
	return ftypeEqual(fnameType(fn), ftypeString(fno));
}
