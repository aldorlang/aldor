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
local void testAblogSefo();
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
	testAblogSefo();
	fini();
}

extern int ablogDebug;
extern int sefoEqualDebug;

local void
testAblog()
{
	String Boolean_imp = "import from Boolean";
	String C0_def = "C0: Category == with";
	String C1_def = "C1: Category == C0 with";
	
	String D0_def = "D0: C0 with == add";
	String D1_def = "D1: C1 with == add";

	StringList lines;

	AbSynList code;
	AbSyn absyn;

	Stab file;
	Stab stab;

	Syme C0, C1, D0, D1;
	AbSyn sefo1, sefo0;

	AbLogic cond0, cond1;

	initFile();
	ablogDebug = 0;

	lines = listList(String)(5, Boolean_imp, C0_def, C1_def, D0_def, D1_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	
	abPutUse(absyn, AB_Use_NoValue);
	
	file = stabFile();
	stab = stabPushLevel(file, sposNone, STAB_LEVEL_LARGE);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	
	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());
	
	C0 = uniqueMeaning(stab, "C0");
	C1 = uniqueMeaning(stab, "C1");
	D0 = uniqueMeaning(stab, "D0");
	D1 = uniqueMeaning(stab, "D1");
	sefo1 = has(abFrSyme(D1), abFrSyme(C1));
	sefo0 = has(abFrSyme(D1), abFrSyme(C0));
	tiSefo(stab, sefo0);
	tiSefo(stab, sefo1);

	cond0 = ablogFrSefo(sefo0);
	cond1 = ablogFrSefo(sefo1);
	
	afprintf(dbOut, "Implies: %pAbLogic %pAbLogic %d\n", cond1, cond0, ablogImplies(cond1, cond0));
	afprintf(dbOut, "Implies: %pAbLogic %pAbLogic %d\n", cond0, cond1, ablogImplies(cond0, cond1));

	testTrue("00", ablogImplies(cond0, cond0));
	testTrue("10", ablogImplies(cond1, cond0));
	testFalse("01",ablogImplies(cond0, cond1));
	testTrue("11", ablogImplies(cond1, cond1));
}

local void testAbLogEqual(String text, Stab stab, Sefo sefo1, Sefo sefo2);

local void
testAblogSefo()
{
	String Boolean_imp = "import from Boolean";
	
	String D0_def = "D0: with { prime?: % -> Boolean } == add { prime?(a: %): Boolean == never }";
	String D0_imp = "import from D0";
	String d0_def = "d0: D0 == never";

	StringList lines;

	AbSynList code;
	AbSyn absyn;

	Stab file;
	Stab stab;

	AbSyn prime, d0, D0, sefo1, sefo2;

	initFile();
	ablogDebug = 1;
	sefoEqualDebug = 1;

	lines = listList(String)(4, Boolean_imp, D0_def, D0_imp, d0_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	abPutUse(absyn, AB_Use_NoValue);
	
	file = stabFile();
	stab = stabPushLevel(file, sposNone, STAB_LEVEL_LARGE);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	prime = abFrSyme(uniqueMeaning(stab, "prime?"));
	d0 = abFrSyme(uniqueMeaning(stab, "d0"));
	D0 = abFrSyme(uniqueMeaning(stab, "D0"));
	sefo1 = apply1(prime, d0);
	sefo2 = apply1(qualify(prime, D0), d0);

	testAbLogEqual("",         stab, apply1(prime, d0), apply1(prime, d0));
	testAbLogEqual("qual",     stab, apply1(qualify(prime, D0), d0), apply1(prime, d0));
	testAbLogEqual("test",     stab, test(apply1(prime, d0)), apply1(prime, d0));
	testAbLogEqual("pretend",  stab, apply1(prime, pretend(d0, D0)), apply1(prime, d0));
	testAbLogEqual("restrict", stab, apply1(prime, restrictTo(d0, D0)), apply1(prime, d0));
}

local void
testAbLogEqual(String text, Stab stab, Sefo sefo1, Sefo sefo2)
{
	AbLogic ablog1, ablog2;

	tiSefo(stab, sefo1);
	tiSefo(stab, sefo2);
	ablog1 = ablogFrSefo(sefo1);
	ablog2 = ablogFrSefo(sefo2);
	testTrue(text, dnfEqual((DNF) ablog1, (DNF) ablog2));
}

local void
testDnf()
{
	DNF a = dnfAtom(1);
	DNF b = dnfAtom(2);

	testFalse("true", dnfIsFalse(dnfTrue()));
	testFalse("false", dnfIsTrue(dnfFalse()));

	testTrue("DNF1", dnfIsTrue(dnfOr(dnfAtom(1), dnfNotAtom(1))));
	testTrue("DNF2", dnfIsTrue(dnfOr(dnfAnd(dnfAtom(1), dnfAtom(2)),
					 dnfAnd(dnfNotAtom(1), dnfNotAtom(2)))));
	testTrue("DNF3", dnfIsFalse(dnfAnd(dnfAtom(1), dnfNotAtom(1))));
}
