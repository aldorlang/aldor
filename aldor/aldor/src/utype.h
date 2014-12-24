#ifndef UTYPE_H
#define UTYPE_H
#include "axlobs.h"

/**
 * Universally quantified types
 */
typedef struct utype {
	Sefo sefo;
	SymeList vars;
	AbLogic typeInfo;
} *UType;

typedef struct _UTypeResult {
	SymeList symes;
	SefoList sefos;
} *UTypeResult;

UType utypeNew(SymeList freevars, Sefo sefo);
UType utypeNewConstant(Sefo sefo);
UType utypeNewVar(Syme syme);
Bool  utypeIsConstant(UType type);

UTypeResult utypeUnify(UType ut1, UType ut2);

SymeList utypeVars(UType utype);
Sefo utypeSefo(UType utype);

Bool utypeResultIsFail(UTypeResult res);
Bool utypeResultIsEmpty(UTypeResult res);

#endif
