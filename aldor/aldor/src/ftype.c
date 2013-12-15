#include "ftype.h"
#include "sexpr.h"
#include "opsys.h"
#include "util.h"

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
	if (ftypeEqual(ftype, FTYPE_JAVA))	return FTYPENO_JAVA;
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
	case FTYPENO_JAVA:	return FTYPE_JAVA;
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
