#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "comsg.h"
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
local void testTiBupApplyMixed();
local void testTiBupApplyImplicit();
local void testTiBupApplyErrorOnArg();
local void testTiBupGenCross();
local void testTiBupCaseBase();
local void testTiBupCase0();
local void testTiBupCase1();
local void testTiBupCase2();
local void testTiTdnCase1();
local void testTiTdnCase2();
local void testTiTdnSelect();

void
tibupTest()
{
	init();

	TEST(testTiBupCollect1);
	TEST(testTiBupCollect2);
	TEST(testTiTdnPretend);
	TEST(testTiTdnMultiToCrossEmbed);
	TEST(testTiBupApplyMixed);
	TEST(testTiBupApplyImplicit);
	TEST(testTiBupApplyErrorOnArg);
	TEST(testTiBupGenCross);

	TEST(testTiBupCaseBase);
	TEST(testTiBupCase0);
	TEST(testTiTdnSelect);

	TEST(testTiBupCase1);
	TEST(testTiBupCase2);

	TEST(testTiTdnCase1);
	TEST(testTiTdnCase2);

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

extern int tipBupDebug;
extern int tipTdnDebug;
extern int tfsDebug;
extern int tfsMultiDebug;

local void
testTiBupApplyMixed()
{
	String Boolean_imp = "import from Boolean";
	String E_def = "E: with == add";
	String F_def = "F: with { apply: (%, %) -> () } == add { apply(f: %, g: %): () == never }";
	String f1_def = "f: F == never";
	String f2_def = "f(): E == never";

	StringList lines = listList(String)(5, Boolean_imp, E_def, F_def, f1_def, f2_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	AbSyn case1 = abqParse("f(f)");
	Stab stab;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	tfsDebug = tipBupDebug = 1;
	scopeBind(stab, case1);
	tiBottomUp(stab, case1, tfUnknown);

	testIntEqual("fn", 1, tpossCount(abTPoss(case1)));

	tiTopDown(stab, case1, tfNone());
	testIntEqual("Unique", AB_State_HasUnique, abState(case1));

	finiFile();
}



local void
testTiBupApplyImplicit()
{
	String Boolean_imp = "import from Boolean";
	String E_def = "E: with == add";
	String S_def = "S: with { apply: (%, E) -> () } == add { apply(f: %, e: E): () == never }";
	String s_def = "s: S == never";
	String e_def = "e: E == never";

	StringList lines = listList(String)(5, Boolean_imp, E_def, S_def, s_def, e_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	AbSyn case1 = abqParse("s e");
	Stab stab;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	tfsDebug = tipBupDebug = 1;
	scopeBind(stab, case1);
	tiBottomUp(stab, case1, tfUnknown);

	testIntEqual("fn", 1, tpossCount(abTPoss(case1)));

	tiTopDown(stab, case1, tfNone());
	testIntEqual("Unique", AB_State_HasUnique, abState(case1));

	finiFile();
}


local void
testTiBupApplyErrorOnArg()
{
	String Boolean_imp = "import from Boolean";
	String E_def = "E: with == add";
	String S_def = "S: with { apply: (%, E) -> () } == add { apply(f: %, e: E): () == never }";
	String s_def = "s: S == never";
	String e_def = "e: E == never";

	StringList lines = listList(String)(5, Boolean_imp, E_def, S_def, s_def, e_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	AbSyn case1 = abqParse("s x");
	Stab stab;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	tfsDebug = tipBupDebug = 1;
	scopeBind(stab, case1);
	tiBottomUp(stab, case1, tfUnknown);

	testIntEqual("fn", 0, tpossCount(abTPoss(case1)));

	testIntEqual("Unchanged", AB_State_HasPoss, abState(case1));

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


local void
testTiBupGenCross()
{
	AbSyn absyn = stdtypes();

	initFile();
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test1 = abqParse("generate yield ()");
	tiBottomUp(stab, test1, tfUnknown);

	testIntEqual("G0", 1, tpossCount(abTPoss(test1)));
	testTrue("G1", tfIsGenerator(tpossUnique(abTPoss(test1))));
	testTrue("G2", tfIsAnyGenerator(tpossUnique(abTPoss(test1))));

	AbSyn test2 = abqParse("xgenerate yield ()");
	tiBottomUp(stab, test2, tfUnknown);

	testIntEqual("XG0", 1, tpossCount(abTPoss(test2)));
	testTrue("XG1", tfIsXGenerator(tpossUnique(abTPoss(test2))));
	testTrue("XG2", tfIsAnyGenerator(tpossUnique(abTPoss(test2))));

	finiFile();
}

local void
testTiBupCaseBase()
{
	String Boolean_imp = "import from Boolean";
	String t_def = "true: Boolean == never";
	String X_def = "X: with == add";
	String x_def = "x: X == never";

	StringList lines = listList(String)(4, Boolean_imp, t_def, X_def, x_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	tipBupDebug = true;
	tipTdnDebug = true;
	tipPatternDebug = true;
	tipApplyDebug = true;

	AbSyn test1 = abqParse("if x case ? then true");
	abPutUse(test1, AB_Use_NoValue);
	tiBottomUp(stab, test1, tfUnknown);
	AbSyn applyCase1 = abFindNode(test1, AB_Apply);
	testAIntEqual("one pattern", AB_State_HasUnique, abState(applyCase1));

	finiFile();
}

local void
testTiBupCase0()
{
	String Boolean_imp = "import from Boolean";
	String t_def = "true: Boolean == never";
	String X_def = "X: with == add";
	String x_def = "x: X == never";

	StringList lines = listList(String)(4, Boolean_imp, t_def, X_def, x_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	tipBupDebug = true;
	tipTdnDebug = true;
	tipPatternDebug = true;
	tipApplyDebug = true;
	//tfsDebug = true;
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test1 = abqParse("if x case ? then true");
	abPutUse(test1, AB_Use_NoValue);
	tiBottomUp(stab, test1, tfUnknown);
	AbSyn applyCase1 = abFindNode(test1, AB_Apply);
	testAIntEqual("one pattern", AB_State_HasUnique, abState(applyCase1));

	AbSyn test2 = abqParse("if (x, x) case (?,?) then true");
	abPutUse(test2, AB_Use_NoValue);
	tiBottomUp(stab, test2, tfUnknown);
	AbSyn applyCase2 = abFindNode(test2, AB_Apply);
	testAIntEqual("two pattern", AB_State_HasUnique, abState(applyCase2));

	AbSyn test3 = abqParse("if () case () then true");
	abPutUse(test3, AB_Use_NoValue);
	tiBottomUp(stab, test3, tfUnknown);

	finiFile();
}


local void
testTiBupCase1()
{
	String Boolean_imp = "import from Boolean";
	String X_def = "X: with == add";
	String Y_def = "Y: with == add";
	String x1_def = "x1: X == never";
	String x2_def = "x2: X == never";
	String f1_def = "f(x: X): PPartial(Y, X) == never";
	String f2_def = "f(x: X, y: Y): Y == never";
	
	StringList lines = listList(String)(7, Boolean_imp, X_def, Y_def, x1_def, x2_def, f1_def, f2_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	/*
	tipBupDebug = true;
	tipPatternDebug = true;
	tipApplyDebug = true;
	tfsDebug = true;
	tfsMultiDebug = true;
	*/
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test2 = abqParse("x1 case f(?, ?)");
	abPutUse(test2, AB_Use_NoValue);
	scopeBind(stab, absyn);
	tiBottomUp(stab, test2, tfUnknown);

	AbSyn apply = abFindNode(test2, AB_Apply);
	testAIntEqual("one pattern", 1, tpossCount(abTPoss(apply)));
	
	finiFile();
}

local void
testTiTdnCase1()
{
	String Boolean_imp = "import from Boolean";
	String X_def = "X: with == add";
	String Y_def = "Y: with == add";
	String x1_def = "x1: X == never";
	String x2_def = "x2: X == never";
	String f1_def = "f(x: X): PPartial(Y, X) == never";
	String f2_def = "f(x: X, y: Y): Y == never";
	
	StringList lines = listList(String)(7, Boolean_imp, X_def, Y_def, x1_def, x2_def, f1_def, f2_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	tipBupDebug = true;
	tipTdnDebug = true;
	tipPatternDebug = true;
	tipApplyDebug = true;
	tfsDebug = false;
	tfsMultiDebug = true;
	
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test2 = abqParse("if x1 case f(?, ?) then never");
	abPutUse(test2, AB_Use_NoValue);
	scopeBind(stab, absyn);
	tiBottomUp(stab, test2, tfUnknown);
	tiTopDown(stab, test2, tfUnknown);

	AbSyn applyCase = abFindNode(test2, AB_Apply);
	testAIntEqual("uniqueCase", AB_State_HasUnique, abState(applyCase));

	AbSyn applyF = abFindNode(applyCase->abApply.argv[1], AB_Apply);
	testAIntEqual("uniqueApply", AB_State_HasUnique, abState(applyF));

	AbSyn f = abFindNode(applyF, AB_Id);
	testAIntEqual("uniqueF", AB_State_HasUnique, abState(f));
	testAIntEqual("embedF", AB_Embed_ApplyPatCall|AB_Embed_Identity, abTContext(f));
	
	finiFile();
}

local void
testTiBupCase2()
{
	String Boolean_imp = "import from Boolean";
	String X_def = "X: with == add";
	String Y_def = "Y: with == add";
	String x1_def = "x1: X == never";
	String x2_def = "y1: X == never";
	String f1_def = "f(x: X): PPartial(Y) == never";
	
	StringList lines = listList(String)(6, Boolean_imp, X_def, Y_def, x1_def, x2_def, f1_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	/*
	tipBupDebug = true;
	tipPatternDebug = true;
	tipApplyDebug = true;
	tfsDebug = true;
	tfsMultiDebug = true;
	*/
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test2 = abqParse("(x1,y1) case (f(?), ?)");
	abPutUse(test2, AB_Use_Value);
	scopeBind(stab, absyn);
	tiBottomUp(stab, test2, tfUnknown);

	AbSyn applyCase = abFindNode(test2, AB_Apply);
	AbSyn applyF = abFindNode(abApplyArg(applyCase, 1), AB_Apply);
	testAIntEqual("pair lhs", 1, tpossCount(abTPoss(applyCase)));
	testAIntEqual("fn lhs", 1, tpossCount(abTPoss(applyF)));
	
	testIntEqual("Error count", 0, comsgErrorCount());

	finiFile();
}

local void
testTiTdnCase2()
{
	String Boolean_imp = "import from Boolean";
	String X_def = "X: with == add";
	String x1_def = "x1: X == never";
	String g_def = "g: X -> X -> PPartial X";
	
	StringList lines = listList(String)(4, Boolean_imp, X_def, x1_def, g_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	tipBupDebug = true;
	tipTdnDebug = true;
	tipPatternDebug = true;
	tipApplyDebug = true;
	tfsDebug = true;
	tfsMultiDebug = true;
	
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test2 = abqParse("if x1 case g(x1)(?) then never");
	abPutUse(test2, AB_Use_NoValue);
	scopeBind(stab, absyn);
	tiBottomUp(stab, test2, tfUnknown);
	tiTopDown(stab, test2, tfUnknown);

	AbSyn applyCase = abFindNode(test2, AB_Apply);
	testAIntEqual("uniqueCase", AB_State_HasUnique, abState(applyCase));

	testIntEqual("Error count", 0, comsgErrorCount());
	finiFile();
}

local void
testTiTdnSelect()
{
	String Boolean_imp = "import from Boolean";
	String X_def = "X: with == add";
	String x1_def = "local x1: X == never";
	String g_def = "g: X -> X -> PPartial X";
	
	StringList lines = listList(String)(4, Boolean_imp, X_def, x1_def, g_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	tipBupDebug = true;
	tipTdnDebug = true;
	tipPatternDebug = true;
	tipApplyDebug = true;
	tfsDebug = true;
	tfsMultiDebug = true;
	
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test2 = abqParse("select x1 in { ? => never } ");
	abPutUse(test2, AB_Use_NoValue);
	scopeBind(stab, absyn);
	tiBottomUp(stab, test2, tfUnknown);
	tiTopDown(stab, test2, tfUnknown);

	testIntEqual("Error count", 0, comsgErrorCount());
	finiFile();
}
