#ifndef UTYPE_H
#define UTYPE_H
#include "axlobs.h"

/**
 * Universally quantified types
 */

typedef struct utform {
	TForm tf;
	SymeList vars;
	AbLogic typeInfo;
} *UTForm;

typedef struct utype {
	Sefo sefo;
	SymeList vars;
	AbLogic typeInfo;
} *UType;

typedef struct utypeResult {
	SymeList symes;
	SefoList sefos;
} *UTypeResult;

DECLARE_LIST(UTForm);

UTForm utformNew	(SymeList freevars, TForm tform);
UTForm utformNewConstant(TForm tform);
void   utformFree	(UTForm utform);
TForm  utformConstOrFail (UTForm);

TForm utformTForm(UTForm utf);
SymeList utformVars(UTForm utf);

Bool utfSatisfies(UTForm S, UTForm t);
Bool utfIsUnknown(UTForm S);
Bool utfIsConstant(UTForm S);
UTForm utformFollowOnly(UTForm);
Bool utformEqual(UTForm, UTForm);
int utformPrint(FILE *, UTForm);

UType utypeNew(SymeList freevars, Sefo sefo);
UType utypeNewConstant(Sefo sefo);
UType utypeNewVar(Syme syme);
Bool  utypeIsConstant(UType type);


UTypeResult utypeUnify(UType ut1, UType ut2);

SymeList utypeVars(UType utype);
Sefo utypeSefo(UType utype);

Bool utypeResultIsFail(UTypeResult res);
Bool utypeResultIsEmpty(UTypeResult res);
int utypeResultOStreamPrint(OStream ostream, UTypeResult utypeResult);

#endif
