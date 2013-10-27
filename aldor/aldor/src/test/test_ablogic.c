#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "scobind.h"
#include "stab.h"
#include "testlib.h"
#include "ti_sef.h"
#include "tinfer.h"
#include "sefo.h"
#include "ablogic.h"
#include "comsg.h"
#include "dnf.h"

local void testAblog();
local void testDnf();

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);

void ablogTest()
{
	init();
	testDnf();
	testAblog();
	fini();
}

extern int ablogDebug;

local void
testAblog()
{
	initFile();
	ablogDebug = 0;

	String Boolean_imp = "import from Boolean";
	String C0_def = "C0: Category == with";
	String C1_def = "C1: Category == C0 with";
	
	String D0_def = "D0: C0 with == add";
	String D1_def = "D1: C1 with == add";

	StringList lines = listList(String)(5, Boolean_imp, C0_def, C1_def, D0_def, D1_def);

	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);
	
	abPutUse(absyn, AB_Use_NoValue);
	
	Stab file = stabFile();
	Stab stab = stabPushLevel(file, sposNone, STAB_LEVEL_LARGE);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	
	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());
	
	Syme C0 = uniqueMeaning(stab, "C0");
	Syme C1 = uniqueMeaning(stab, "C1");
	Syme D0 = uniqueMeaning(stab, "D0");
	Syme D1 = uniqueMeaning(stab, "D1");
	AbSyn sefo1 = has(abFrSyme(D1), abFrSyme(C1));
	AbSyn sefo0 = has(abFrSyme(D1), abFrSyme(C0));
	tiSefo(stab, sefo0);
	tiSefo(stab, sefo1);

	AbLogic cond0 = ablogFrSefo(sefo0);
	AbLogic cond1 = ablogFrSefo(sefo1);
	
	afprintf(dbOut, "Implies: %pAbLogic %pAbLogic %d\n", cond1, cond0, ablogImplies(cond1, cond0));
	afprintf(dbOut, "Implies: %pAbLogic %pAbLogic %d\n", cond0, cond1, ablogImplies(cond0, cond1));

	testTrue("00", ablogImplies(cond0, cond0));
	testTrue("10", ablogImplies(cond1, cond0));
	testFalse("01",ablogImplies(cond0, cond1));
	testTrue("11", ablogImplies(cond1, cond1));
}


local void
testDnf()
{
	testFalse("true", dnfIsFalse(dnfTrue()));
	testFalse("false", dnfIsTrue(dnfFalse()));

	DNF a = dnfAtom(1);
	DNF b = dnfAtom(2);

	afprintf(dbOut, "%pDNF\n", dnfOr(dnfAtom(1), dnfNotAtom(1)));
}
