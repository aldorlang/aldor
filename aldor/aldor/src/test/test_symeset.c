#include "testall.h"
#include "testlib.h"

#include "stab.h"
#include "symeset.h"
#include "syme.h"

local void testSymeSet(void);

void symeSetTestSuite()
{
	init();
	TEST(testSymeSet);
	fini();
}

void
testSymeSet()
{
	Stab stab;
	SymeSet symeSet;
	Syme syme1, syme1a, syme2, syme2a;
	
	initFile();
	stab = stabFile();
	syme1 = symeNewParam(symInternConst("syme1"), tfCross(0), car(stab));
	syme1a = symeNewParam(symInternConst("syme1"), tfTuple(tfCross(0)), car(stab));
	syme2 = symeNewParam(symInternConst("syme2"), tfCross(0), car(stab));
	syme2a = symeNewParam(symInternConst("syme2"), tfTuple(tfCross(0)), car(stab));

	symeSet = symeSetFrSymes(listList(Syme)(3, syme1, syme1a, syme2));
	testTrue("", symeSetMember(symeSet, syme1));
	testTrue("", symeSetMember(symeSet, syme1a));
	testTrue("", symeSetMember(symeSet, syme2));
	testFalse("", symeSetMember(symeSet, syme2a));

	testIntEqual("", 2, listLength(Syme)(symeSetSymesForSymbol(symeSet, symInternConst("syme1"))));
	testIntEqual("", 1, listLength(Syme)(symeSetSymesForSymbol(symeSet, symInternConst("syme2"))));
	testIntEqual("", 0, listLength(Syme)(symeSetSymesForSymbol(symeSet, symInternConst("syme3"))));

	finiFile();
}
