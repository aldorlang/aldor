#ifndef UTFORM_H
#define UTFORM_H
#include "axlobs.h"

struct utform {
	TForm tf;
	UVar  uvar;
};

extern UTForm utfUnknown;

// UTFORM

void   utformInit(void);

UTForm utformNew	(SymeList freevars, TForm tform);
UTForm utformNewUVar    (UVar uvar, TForm tform);
UTForm utformNewWithCond(SymeList freevars, TForm tform, SefoList cond);
UTForm utformNewConstant(TForm tform);
void   utformFree	(UTForm utform);
TForm  utformConstOrFail (UTForm);
Bool   utformIsAnyMap(UTForm utf);
Bool   utfIsPending(UTForm utf);
UTForm utfMapRet(UTForm tf);
UTForm utfMapArg(UTForm tf);
Bool   utfIsMulti(UTForm tf);
Bool   utfIsDeclare(UTForm tf);
Bool   utfIsTuple(UTForm tf);
UTForm utfTupleArg(UTForm tf);

UTForm utfDefineVal(UTForm tf);

Length utfMultiArgc(UTForm T);
UTForm utfMultiArgN(UTForm T, Length n);
Bool   utfMultiHasDefaults(UTForm T);
Bool   utfIsEmptyMulti(UTForm tf);
UTForm utfMultiFrList(UTFormList utfl);

Bool   utfIsCross(UTForm T);
Length utfCrossArgc(UTForm T);
UTForm utfCrossArgN(UTForm T, Length n);

Bool   utfIsDefine(UTForm T);

Bool   utfIsAnyMap(UTForm tf);
Bool   utfIsPackedMap(UTForm tf);

Bool   utfIsRaw(UTForm tf);
UTForm utfRawType(UTForm tf);
UTForm utfRawArg(UTForm tf);

Bool   utfIsExit(UTForm tf);

Length utfArgc(UTForm utf);
UTForm utfArgN(UTForm utf, Length i);

Stab utfGetStab(UTForm utf);

UTForm utfDefineeType		(UTForm utf);
Syme   utfDefineeSyme		(UTForm);
AbEmbed utfAsMultiEmbed		(UTForm, Length argc);
Length utfAsMultiArgc		(UTForm);
UTForm utfAsMultiArgN		(UTForm, Length argc, Length n);
AbSyn  utfAsMultiSelectArg	(AbSyn, Length argc, Length n,
				 AbSynGetter, UTForm, Bool *,
				 Length *);

UVar  utformUVar(UTForm utf); // Adds a ref, remember to free or swallow
TForm utformTForm(UTForm utf);
SymeList utformVars(UTForm utf);
SefoList utformCondition(UTForm utf);

Bool utfIsUnknown(UTForm S);
Bool utfIsConstant(UTForm S);
Bool utfIsAny(UTForm S);
Bool utfIsNone(UTForm S);

Bool utfHasVar(UTForm utf, Syme syme);

UTForm utformFollowOnly(UTForm);
UTForm utformFollow(UTForm);
Bool   utformEqual(UTForm, UTForm);

UTForm utformSubst(AbSub sigma, UTForm utf);

int utfPrint(FILE *fout, UTForm utf);
void utfPrintDb(UTForm utf);

int utfOStreamWrite(OStream ostream, Bool deep, UTForm utf);

#endif
