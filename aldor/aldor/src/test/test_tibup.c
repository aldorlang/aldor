#include "axlobs.h"
#include "testlib.h"
#include "abquick.h"
#include "tinfer.h"

local void testTiBupCollect();

void
tibupTest()
{
	init();
	TEST(testTiBupCollect);
	fini();
}

local void
testTiBupCollect()
{
	String Boolean_imp = "import from Boolean";
	String D_def = "D: with == add";
	String E_def = "E: with == add";
	String g_def = "g(): Generator D == never";
	
	StringList lines = listList(String)(4, Boolean_imp, D_def, E_def, g_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	

	TForm D = tiGetTForm(stab, id("D"));
	TForm E = tiGetTForm(stab, id("E"));
	Syme g = uniqueMeaning(stab, "g");

	afprintf(dbOut, "D is %pTForm.  E is %pTForm.  g is: %pTForm\n", D, E, symeType(g));
	AbSyn collect = abqParse("x for x in g()");
	scopeBind(stab, collect);
	
	abPutUse(collect, AB_Use_Value);
	tiBottomUp(stab, collect, tfGenerator(D));
	testIntEqual("one", 1, tpossCount(abTPoss(collect)));

	AbSyn collect2 = abqParse("x for x in g()");
	scopeBind(stab, collect2);
	
	tiBottomUp(stab, collect2, tfUnknown);
	testIntEqual("two", 1, tpossCount(abTPoss(collect2)));

	AbSyn collect3 = abqParse("x for x in g()");
	scopeBind(stab, collect3);
	
	tiBottomUp(stab, collect3, E);
	testIntEqual("three", 0, tpossCount(abTPoss(collect3)));

	AbSyn collect4 = abqParse("x for x in g()");
	scopeBind(stab, collect4);
	
	tiBottomUp(stab, collect4, tfGenerator(E));
	testIntEqual("four", 0, tpossCount(abTPoss(collect4)));

	finiFile();
}

