#ifndef UTYPE_H
#define UTYPE_H
#include "axlobs.h"

/**
 * Universally quantified types
 */

struct utform {
	TForm tf;
	SymeList vars;
	AbLogic typeInfo;
};

struct utype {
	Sefo sefo;
	SymeList vars;
	AbLogic typeInfo;
};

typedef struct utypeResult {
	SymeList symes;
	SefoList sefos;
} *UTypeResult;

UTForm utformNew	(SymeList freevars, TForm tform);
UTForm utformNewConstant(TForm tform);
void   utformFree	(UTForm utform);
TForm  utformConstOrFail (UTForm);
Bool   utformIsAnyMap(UTForm utf);
Bool   utfIsPending(UTForm utf);
UTForm utfMapRet(UTForm tf);
UTForm utfMapArg(UTForm tf);
Bool   utfIsMulti(UTForm tf);

UTForm utfDefineeType(UTForm utf);
Syme   utfDefineeSyme		(UTForm);
Length utfAsMultiArgc		(UTForm);
UTForm utfAsMultiArgN		(UTForm, Length argc, Length n);
AbSyn  utfAsMultiSelectArg	(AbSyn, Length argc, Length n,
				 AbSynGetter, UTForm, Bool *,
				 Length *);

TForm utformTForm(UTForm utf);
SymeList utformVars(UTForm utf);

UTForm utformSubst(AbSub sigma, UTForm utf);
UTypeResult utformUnify(UTForm ut1, UTForm ut2);
Bool utformCanUnify(UTForm ut1, UTForm ut2);

Bool utfIsUnknown(UTForm S);
Bool utfIsConstant(UTForm S);

Bool   utfSatisfies(UTForm S, UTForm T);
UTForm utformFollowOnly(UTForm);
Bool   utformEqual(UTForm, UTForm);
int    utformPrint(FILE *, UTForm);

UType utypeNew(SymeList freevars, Sefo sefo);
UType utypeNewConstant(Sefo sefo);
UType utypeNewVar(Syme syme);
Bool  utypeIsConstant(UType type);

UTypeResult utypeUnify(UType utype1, UType utype2);

SymeList utypeVars(UType utype);
Sefo     utypeSefo(UType utype);

Bool utypeResultIsFail(UTypeResult res);
Bool utypeResultIsEmpty(UTypeResult res);
UTForm utypeResultApplyTForm(UTypeResult res, UTForm tf);
int utypeResultOStreamPrint(OStream ostream, UTypeResult utypeResult);
Bool utypeHasVar(UType, Syme);

UTypeResult utypeResultEmpty(void);
UTypeResult utypeResultFailed(void);
void utypeResultFree(UTypeResult result);
UTypeResult utypeResultMerge(UTypeResult res1, UTypeResult res2);
AbSub utypeResultSigma(UTypeResult result);

int utfPrintDb(UTForm);
int utypePrintDb(UType);
int utypeResultPrintDb(UTypeResult);

#endif
