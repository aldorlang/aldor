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
local void testTiTdnMultiToCrossEmbed();

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
	TEST(testTiTdnMultiToCrossEmbed);
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

	Stab stab;

	TForm D, E;
	Syme g;

	AbSyn collect, collect2, collect3, collect4;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	D = tiGetTForm(stab, id("D"));
	E = tiGetTForm(stab, id("E"));
	g = uniqueMeaning(stab, "g");

	afprintf(dbOut, "D is %pTForm.  E is %pTForm.  g is: %pTForm\n", D, E, symeType(g));
	collect = abqParse("x for x in g()");
	scopeBind(stab, collect);
	
	abPutUse(collect, AB_Use_Value);
	tiBottomUp(stab, collect, tfGenerator(D));
	testIntEqual("one", 1, tpossCount(abTPoss(collect)));

	collect2 = abqParse("x for x in g()");
	scopeBind(stab, collect2);
	
	tiBottomUp(stab, collect2, tfUnknown);
	testIntEqual("two", 1, tpossCount(abTPoss(collect2)));

	collect3 = abqParse("x for x in g()");
	scopeBind(stab, collect3);
	
	tiBottomUp(stab, collect3, E);
	testIntEqual("three", 0, tpossCount(abTPoss(collect3)));

	collect4 = abqParse("x for x in g()");
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

	Stab stab;

	TForm D, E, yTF;
	Syme y;

	AbSyn collect;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	D = tiGetTForm(stab, id("D"));
	E = tiGetTForm(stab, id("E"));

	collect = abqParse("local x: D := never; y := x for free x in g()");
	scopeBind(stab, collect);
	tiBottomUp(stab, collect, tfUnknown);

	testIntEqual("Collect is ok", 1, tpossCount(abTPoss(collect)));

	y = uniqueMeaning(stab, "y");
	yTF = symeType(y);
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

	Stab stab;

	AbSyn pretend;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	pretend = abqParse("(x, x) pretend D");
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

local void
testTiTdnMultiToCrossEmbed()
{
	String Boolean_imp = "import from Boolean";
	String E_def = "E: with == add";
	String f_def = "f(): (E, E) == never";
	String g_def = "g(a: Cross(E,E)): () == never";

	StringList lines = listList(String)(4, Boolean_imp, E_def, f_def, g_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	Stab stab;

	AbSyn fncall;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	fncall = abqParse("g(f())");
	scopeBind(stab, fncall);
	tiBottomUp(stab, fncall, tfUnknown);
	tiTopDown(stab, fncall, tfUnknown);

	testIntEqual("Unique", AB_State_HasUnique, abState(fncall));
	testIntEqual("embed", AB_Embed_MultiToCross, fncall->abApply.argv[0]->abApply.hdr.seman->embed);

	aprintf("Type of g: %pTForm\n", abTUnique(fncall->abApply.op));
	aprintf("Type of f(): %pTForm\n", abTUnique(fncall->abApply.argv[0]));
	aprintf("Embed of f(): %d\n", abEmbedApply(fncall->abApply.argv[0]));

	finiFile();
}
