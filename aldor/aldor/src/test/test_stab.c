#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "scobind.h"
#include "stab.h"
#include "testlib.h"
#include "tinfer.h"
#include "tinfer.h"
#include "tqual.h"

local void testStabIsChild();

local void testTFormCascadedImport();

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile();
void finiFile();

void stabTest()
{
	init();
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
	initFile();
	String Foo_def = "Foo(S: with): with { x: %; export from S } == add { x: % == never }";
	String D_def = "D: with { a: %} == add { a: % == never}";

	StringList lines = listList(String)(2, Foo_def, D_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	stabImportDebug = 1;
	tipBupDebug = 1;

	Stab subStab = stabPushLevel(stabFile(), sposNone, 0);
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stabFile(), absyn);
	typeInfer(stabFile(), absyn);
	testIntEqual("Error Count", 0, comsgErrorCount());
	
	TForm Foo_D = tiGetTForm(stabFile(), apply1(id("Foo"), id("D")));
	TForm D = tiGetTForm(stabFile(), id("D"));
	afprintf(dbOut, "Foo D: %pTForm\n", Foo_D);
	afprintf(dbOut, "D: %pTForm\n", D);
	stabImportFrom(subStab, tqNewUnqualified(Foo_D));
	tiTfImportCascades(subStab, tfCascades(Foo_D));

	testTrue("imported Foo D subStab", stabIsImportedTForm(subStab, Foo_D));
	testFalse("imported Foo D stab", stabIsImportedTForm(stabFile(), Foo_D));
	testTrue("imported D subStab", stabIsImportedTForm(subStab, D));
	testFalse("imported D stab", stabIsImportedTForm(stabFile(), D));

	SymeList lSubStab = stabGetMeanings(subStab, ablogTrue(), symInternConst("a"));
	SymeList lStab =    stabGetMeanings(stabFile(), ablogTrue(), symInternConst("a"));

	testIntEqual("one meaning", 1, listLength(Syme)(lSubStab));
	testIntEqual("no meanings", 0, listLength(Syme)(lStab));

	finiFile();
}

