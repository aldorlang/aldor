#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "scobind.h"
#include "stab.h"
#include "testlib.h"
#include "ti_bup.h"
#include "ti_tdn.h"
#include "tinfer.h"
#include "sefo.h"
#include "tposs.h"

local void testTiBupCollect1();
local void testTiBupCollect2();
local void testTiTdnPretend();

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);
void finiFile(void);

void
tibupTest()
{
	init();
	TEST(testTiBupCollect1);
	TEST(testTiBupCollect2);
	TEST(testTiTdnPretend);
	fini();
}

local void
testTiBupCollect1()
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

local void
testTiBupCollect2()
{
	String Boolean_imp = "import from Boolean";
	String D_def = "D: with == add";
	String E_def = "E: with == add";
	String g1_def = "g(): Generator D == never";
	String g2_def = "g(): Generator E == never";
	String dg_def = "dg(): Generator D == never";

	StringList lines = listList(String)(5, Boolean_imp, D_def, E_def, g1_def, g2_def, dg_def);
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

	AbSyn collect = abqParse("local x: D := never; y := x for free x in g()");
	scopeBind(stab, collect);
	tiBottomUp(stab, collect, tfUnknown);

	testIntEqual("Collect is ok", 1, tpossCount(abTPoss(collect)));

	Syme y = uniqueMeaning(stab, "y");
	TForm yTF = symeType(y);
	testTrue("is a generator", tfIsGenerator(yTF));

	testTrue("Generates D", tformEqual(D, tfGeneratorArg(yTF)));

	finiFile();
}

local void
testTiTdnPretend()
{
	String Boolean_imp = "import from Boolean";
	String D_def = "D: with == add";
	String E_def = "E: with == add";
	String x_def = "x: E == never";

	StringList lines = listList(String)(4, Boolean_imp, D_def, E_def, x_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn pretend = abqParse("(x, x) pretend D");
	scopeBind(stab, pretend);
	tiBottomUp(stab, pretend, tfUnknown);
	tiTopDown(stab, pretend, tfUnknown);

	testTrue("is multi", tfIsMulti(abTUnique(pretend->abPretendTo.expr)));
	testIntIsNotZero("Has a context", abTContext(pretend->abPretendTo.expr));
	testTrue("multi to cross", abTContext(pretend->abPretendTo.expr) & AB_Embed_MultiToCross);

	finiFile();
}

/*
This should really work, but at the moment a4 and a8 give incorrect results.
D: with == add
E: with == add

x: E := never

f(): () ==
  a1 := (x, x) pretend D
  a3 := (x, x) pretend Cross(D, D)
  a4 := (x, x) pretend Tuple(D)

  a5 := (x, x)@Cross(E, E)
  a7 := (x, x)@Cross(E, E)
  a8 := (x, x)@Tuple(E)

*/
