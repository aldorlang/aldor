#ifndef _ABQUICK_H
#define _ABQUICK_H

#include "axlobs.h"
#include "absyn.h"
#include "path.h"

#define ABQK_DECLARE0(name, abname) extern AbSyn name(void)
#define ABQK_DEFINE0(name, abName) \
	AbSyn name() { return abName(sposNone); } \
	ABQK_DECLARE0(name, abName)

#define ABQK_DECLARE1(name, abname) extern AbSyn name(AbSyn x)
#define ABQK_DEFINE1(name, abName) \
	AbSyn name(AbSyn x) { return abName(sposNone, x); } \
	ABQK_DECLARE1(name, abName)

#define ABQK_DECLARE2(name, abname) extern AbSyn name(AbSyn x, AbSyn y)
#define ABQK_DEFINE2(name, abName) \
	AbSyn name(AbSyn x, AbSyn y) { return abName(sposNone, x, y); } \
	ABQK_DECLARE2(name, abName)

#define ABQK_DECLARE3(name, abname) extern AbSyn name(AbSyn x, AbSyn y, AbSyn z)
#define ABQK_DEFINE3(name, abName) \
	AbSyn name(AbSyn x, AbSyn y, AbSyn z) { return abName(sposNone, x, y, z); } \
	ABQK_DECLARE3(name, abName)

#define ABQK_DECLARE1_Symbol(name, abname) extern AbSyn name(String y)
#define ABQK_DEFINE1_Symbol(name, abName) \
	AbSyn name(String txt) { return abName(sposNone, symIntern(txt)); } \
	ABQK_DECLARE1_Symbol(name, abName)

ABQK_DECLARE0(sequence0, abNewSequence0);
ABQK_DECLARE1(sequence1, abNewSequence1);
ABQK_DECLARE2(sequence2, abNewSequence2);
ABQK_DECLARE0(comma0, abNewComma0);
ABQK_DECLARE1(comma1, abNewComma1);
ABQK_DECLARE2(comma2, abNewComma2);
ABQK_DECLARE2(define, abNewDefine);
ABQK_DECLARE2(declare, abNewDeclare);
ABQK_DECLARE0(nothing, abNewNothing);
ABQK_DECLARE2(with, abNewWith);
ABQK_DECLARE2(add, abNewAdd);
ABQK_DECLARE2(label, abNewLabel);
ABQK_DECLARE2(has, abNewHas);
ABQK_DECLARE2(apply1, abNewApply1);
ABQK_DECLARE3(apply2, abNewApply2);
ABQK_DECLARE3(lambda, abNewLambda);
ABQK_DECLARE3(_if0, abNewIf);
ABQK_DECLARE2(import, abNewImport);
ABQK_DECLARE1(test, abNewTest);
ABQK_DECLARE2(qualify, abNewQualify);
ABQK_DECLARE2(pretend, abNewPretendTo);
ABQK_DECLARE2(restrictTo, abNewRestrictTo);

ABQK_DECLARE1_Symbol(id, abNewId);


AbSyn emptyWith();
AbSyn emptyAdd();
AbSyn defineUnary(String name, AbSyn param, AbSyn retType, AbSyn rhs);
AbSyn _if(AbSyn testPart, AbSyn thenPart, AbSyn elsePart);

AbSyn abqParse(String txt);
AbSynList abqParseLines(StringList lines);
AbSyn abqParseLinesAsSeq(StringList lines);

AbSyn stdtypes();

Syme uniqueMeaning(Stab stab, String s);

#endif
