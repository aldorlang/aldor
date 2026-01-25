#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "scobind.h"
#include "sefo.h"
#include "stab.h"
#include "testlib.h"
#include "tinfer.h"
#include "tinfer.h"
#include "tqual.h"
#include "ablogic.h"
#include "comsg.h"
#include "symbol.h"

Bool	testStabDebug	= true;
#define testDEBUG	DEBUG_IF(testStab)	afprintf

local void testStabIsChild();
local void testStabVar_0();
local void testStabVar_EQ();
local void testStabVar_Sefo();

local void testTFormCascadedImport();

void stabTest()
{
	init();
	TEST(testStabVar_0);
	TEST(testStabVar_EQ);
	TEST(testStabVar_Sefo);
	TEST(testStabIsChild);
	TEST(testTFormCascadedImport);
	fini();
}


local void testStabIsChild()
{
	Stab global = stabNewGlobal();
	Stab root = stabNewFile(global);

	Stab c1 = stabPushLevel(root, sposNone, 0);
	Stab c2 = stabPushLevel(root, sposNone, 0);
	
	Stab c11 = stabPushLevel(c1, sposNone, 0);

	testTrue("selfparent-1", stabIsChild(root, root));
	testTrue("selfparent-2", stabIsChild(c1, c1));
	testTrue("selfparent-3", stabIsChild(c2, c2));
	testTrue("selfparent-3", stabIsChild(c11, c11));

	testTrue("t1", stabIsChild(root, c1));
	testTrue("t1", stabIsChild(root, c11));
	testTrue("t1", stabIsChild(root, c2));

	testFalse("t2", stabIsChild(c1, root));
	testFalse("t2", stabIsChild(c2, root));
	testFalse("t2", stabIsChild(c11, root));

	testTrue("c11", stabIsChild(c1, c11));
	testFalse("c11", stabIsChild(c11, c1));
	testFalse("c11", stabIsChild(c2, c1));
	testFalse("c11", stabIsChild(c2, c11));
}

extern int stabImportDebug;
extern int tipBupDebug;
local void
testTFormCascadedImport()
{
	String Foo_def = "Foo(S: with): with { x: %; export from S } == add { x: % == never }";
	String D_def = "D: with { a: %} == add { a: % == never}";

	StringList lines;
	AbSynList absynList;
	AbSyn absyn;

	Stab subStab;

	TForm Foo_D, D;

	SymeList lSubStab, lStab;

	initFile();

	lines = listList(String)(2, Foo_def, D_def);
	absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, absynList);

	stabImportDebug = 0;
	tipBupDebug = 0;

	subStab = stabPushLevel(stabFile(), sposNone, 0);
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stabFile(), absyn);
	typeInfer(stabFile(), absyn);
	testIntEqual("Error Count", 0, comsgErrorCount());
	
	Foo_D = tiGetTForm(stabFile(), apply1(id("Foo"), id("D")));
	D = tiGetTForm(stabFile(), id("D"));
	testDEBUG(dbOut, "Foo D: %pTForm\n", Foo_D);
	testDEBUG(dbOut, "D: %pTForm\n", D);
	stabImportFrom(subStab, tqNewUnqualified(Foo_D));
	tiTfImportCascades(subStab, tfCascades(Foo_D));

	testTrue("imported Foo D subStab", stabIsImportedTForm(subStab, Foo_D));
	testFalse("imported Foo D stab", stabIsImportedTForm(stabFile(), Foo_D));
	testTrue("imported D subStab", stabIsImportedTForm(subStab, D));
	testFalse("imported D stab", stabIsImportedTForm(stabFile(), D));

	lSubStab = stabGetMeanings(subStab, ablogTrue(), symInternConst("a"));
	lStab =    stabGetMeanings(stabFile(), ablogTrue(), symInternConst("a"));

	testIntEqual("one meaning", 1, listLength(Syme)(lSubStab));
	testIntEqual("no meanings", 0, listLength(Syme)(lStab));

	finiFile();
}

void
testStabVar_0()
{
	initFile();

	Stab stab = stabPushLevel(stabFile(), sposNone, 0);

	TForm tf = stabRegisterVar(stab, abNewBlank(sposNone, "?"));

	testDEBUG(dbOut, "tf created %d %pTForm\n", tfVarId(tf), tf);

	testTrue("", tfIsVar(tf));

	finiFile();
}

void
testStabVar_EQ()
{
	initFile();

	Stab stab = stabPushLevel(stabFile(), sposNone, 0);

	TForm tf1 = stabRegisterVar(stab, abNewBlank(sposNone, "?"));
	TForm tf2 = stabRegisterVar(stab, abNewBlank(sposNone, "?"));

	testDEBUG(dbOut, "tf1 created %d %pTForm\n", tfVarId(tf1), tf1);
	testDEBUG(dbOut, "tf2 created %d %pTForm\n", tfVarId(tf2), tf2);

	testFalse("", tformEqual(tf1, tf2));

	finiFile();
}

void
testStabVar_Sefo()
{
	initFile();

	Stab stab = stabPushLevel(stabFile(), sposNone, 0);
	AbSyn ab1 = abNewBlank(sposNone, symInternConst("?"));
	AbSyn ab2 = abNewBlank(sposNone, symInternConst("?"));

	TForm tf1 = tfPending(stab, ab1);
	TForm tf2 = tfPending(stab, ab2);
	tfMeaning(stab, ab1, tf1);
	tfMeaning(stab, ab2, tf2);

	testTrue("", tfIsMeaning(tf1));
	testFalse("", tfNeedsSefo(tf1));
	
	tiTfSefo(stab, tf1);
	tiTfSefo(stab, tf2);

	testDEBUG(dbOut, "Absyn: %pAbSyn\n", ab1);
	testFalse("", sefoEqual(ab1, ab2));
	finiFile();
}

