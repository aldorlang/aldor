#ifndef UTYPERES_H
#define UTYPERES_H
#include "axlobs.h"

typedef struct utypeSubst {
	Syme syme;
	Sefo sefo;
} *UTypeSubst;

DECLARE_LIST(UTypeSubst);

struct utypeResult {
	UTypeSubstList substs;
	SymeList news;
	SefoList conditions;
};

// UTYPERES

UTypeResult utypeResultFailed(void);

UTypeResult	utypeResultEmpty(void);
Bool		utypeResultIsFail(UTypeResult res);
Bool		utypeResultIsEmpty(UTypeResult res);
UTypeSubst	utypeSubstNew(Syme syme, Sefo sefo);
UTypeSubst	utypeSubstCopy(UTypeSubst subst);
void		utypeSubstFree(UTypeSubst subst);
Sefo		utypeSubstApply(UTypeSubst subst, Sefo sefo);
UTypeSubst 	utypeResultSubst(UTypeResult result, Syme syme);
SymeList 	utypeResultSymes(UTypeResult result);
UTypeResult 	utypeResultNew(void);
UTypeResult 	utypeResultCopy(UTypeResult res);
void		utypeResultAdd(UTypeResult res, Syme syme, Sefo sefo);
void		utypeResultSetCondition(UTypeResult res, SefoList conditions);
UTypeResult	utypeResultOne(Syme syme, Sefo sefo);
UTypeResult	utypeResultTwo(Syme syme1, Sefo sefo1, Syme syme2, Sefo sefo2, Syme new);
void 		utypeResultFree(UTypeResult result);
int 		utypeResultPrint(FILE *fout, UTypeResult utypeResult);
int		utypeResultOStreamWrite(OStream ostream, UTypeResult utypeResult);

AbSub		utypeResultSigma(UTypeResult result);

#endif
