#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "cmdline.h"
#include "debug.h"
#include "format.h"
#include "macex.h"
#include "opsys.h"
#include "optfoam.h"
#include "phase.h"
#include "scobind.h"
#include "spesym.h"
#include "stab.h"
#include "symbol.h"
#include "testlib.h"
#include "ti_sef.h"
#include "tinfer.h"
#include "sefo.h"
#include "ablogic.h"
#include "comsg.h"
#include "sexpr.h"

void testSelfTInfer();
void testSimpleTInfer();
void testConditionalTInfer();
void testConditionalTInfer2();
void testConditionalTInfer4();
void testConditionalAdd();
void testTinfer3();
void testTinfer5();
void testTinfer9();
void testTinferMutualReference();
void testTinferValueConditional();
void testTinferValueConditionalAliased();
void testTinferImport();

void tinferTest()
{
	init();

	TEST(testSimpleTInfer);

	TEST(testSelfTInfer);
	TEST(testConditionalTInfer);
	TEST(testConditionalTInfer2);
	TEST(testTinfer3);
	TEST(testConditionalTInfer4);

	TEST(testConditionalAdd);
	TEST(testTinfer5);
	TEST(testTinfer9);
	TEST(testTinferMutualReference);

	TEST(testTinferValueConditional);
	TEST(testTinferImport);
	/*TEST(testTinferValueConditionalAliased);*/
	fini();
}

void
testSimpleTInfer()
{
	AbSyn absyn = define(declare(id("x"), emptyWith()), emptyAdd());
	Stab stab;
	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	finiFile();
}


extern int ablogDebug;
extern int tfsDebug;
extern int tfsParentDebug;
extern int tfsExportDebug;
extern int tipBupDebug;
extern int tipAddDebug;
extern int titfOneDebug;
extern int tipTdnDebug;
extern int sefoEqualDebug;
extern int titfDebug;
extern int tfDebug;
extern int tcDebug;
extern int tfImportDebug;

AbSyn defineUnary(String name, AbSyn param, AbSyn retType, AbSyn rhs);

void
testConditionalTInfer()
{
	/*
	 C1: Category == with;
         Foo(X: with): Category == with { if (X has C1) then { f: % -> X } }
	 D1: C1 == add;
	 A: Foo D1 == add { }
	*/
	AbSyn impBoolean = import(nothing(), id("Boolean"));
	AbSyn c1 = define(declare(id("C1"), id("Category")), emptyWith());
	AbSyn condition = _if(has(id("X"), id("C1")),
			      declare(id("f"), apply2(id("->"), id("%"), id("X"))),
			      nothing());

	AbSyn foo = defineUnary("Foo", declare(id("X"), emptyWith()), id("Category"),
				with(nothing(), condition));

	AbSyn d1 = define(declare(id("D1"), with(id("C1"), nothing())), emptyAdd());
	AbSyn theAdd = emptyAdd();
	AbSyn fooD1 = apply1(id("Foo"), id("D1"));
	AbSyn a = define(declare(id("A"), with(fooD1, nothing())), theAdd);
	AbSyn absyn = abNewSequenceL(sposNone, listList(AbSyn)(6, stdtypes(), impBoolean, c1, foo, d1, a));

	TForm tf;
	SymeList sl;
	Stab stab;

	initFile();
	ablogDebug = 0;
	tipBupDebug = 0;
	tfDebug = 0;
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 1, comsgErrorCount());

	tf = abTForm(fooD1);
	sl = tiAddSymes(abStab(theAdd), theAdd->abAdd.capsule,
			abTForm(theAdd->abAdd.base), tf, (SymeList *) NULL);
	testIntEqual("SymeList Length", 1, listLength(Syme)(sl));
	finiFile();
}

void
testSelfTInfer()
{
	String Boolean_imp = "import from Boolean";
	String XAlgebra_def = "XAlgebra(T: with): Category == with";
	String XIntegralDomain_def = "XIntegralDomain: Category == XAlgebra(%) with";
	String XLocalAlgebra_def = "XLocalAlgebra(R: with, S: XAlgebra R): with == add";
	String D_def = "D: with == add; F(U: with): XIntegralDomain with == add";
	String E_def = "export a: XLocalAlgebra(F D, F D)";
	String F_def = "F(X: XIntegralDomain): with { 1: %} == XLocalAlgebra(X, X) add {1: % == never}";
	StringList lines = listList(String)(5, Boolean_imp, XAlgebra_def, XIntegralDomain_def,
					    XLocalAlgebra_def, D_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	Stab stab;
	AbSyn F, selfRef, sefo, dSefo;

	Syme xalgebra, d;
	TForm tf, dTf;

	SymeList dCatSelfList;

	initFile();

	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	xalgebra = uniqueMeaning(stab, "XAlgebra");
	d = uniqueMeaning(stab, "D");
	sefo = abNewApply1(sposNone, abFrSyme(xalgebra), abFrSyme(d));
	tf = tfFullFrAbSyn(stab, sefo);

	symePrintDb(d);
	tipBupDebug = tipTdnDebug = tfsDebug = tfsParentDebug = tfsExportDebug = 0;
	sefoEqualDebug = 0;

	dSefo = abFrSyme(d);
	tiSefo(stab, dSefo);
	dTf = abTUnique(dSefo);
	afprintf(dbOut, "Type of D is %pTForm\n", dTf);
	dCatSelfList = tfGetCatSelf(dTf);
	afprintf(dbOut, "Self for Type of D is %pSymeList\n", dCatSelfList);
	listIter(Syme, dCatSelf, dCatSelfList,
		 afprintf(dbOut, "Self: %s Type: %pTForm\n", symeString(dCatSelf), symeType(dCatSelf)));


	selfRef = abqParse(E_def);

	abPutUse(selfRef, AB_Use_NoValue);
	scopeBind(stab, selfRef);
	typeInfer(stab, selfRef);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	F = abqParse(F_def);
	abPutUse(selfRef, AB_Use_NoValue);
	scopeBind(stab, selfRef);
	typeInfer(stab, selfRef);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	finiFile();
}

void
testConditionalTInfer2()
{
	/*
	 AdditiveType: Category == with;
         Evalable(T: AdditiveType): Category == with { eval: % -> T; }
	 Obj(R: with): Category == with { x: %; if R has AdditiveType then if R has Evalable(R) then Evalable(R) }
	*/
	String Boolean_imp      = "import from Boolean";
	String AdditiveType_txt = "AdditiveType: Category == with";
	String Evalable_txt     = "Evalable(T: AdditiveType): Category == with { eval: % -> T; }";
	String Obj_txt          =
		"Obj(R: with): Category == with { "
		"   x: %; "
		"   if R has AdditiveType then if R has Evalable(R) then Evalable(R) }";



	StringList lines = listList(String)(4, Boolean_imp, AdditiveType_txt, Evalable_txt, Obj_txt);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	ablogDebug = 0;
	tipBupDebug = 0;
	titfDebug = 0;
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());
	finiFile();
}

void
testTinfer3()
{
	String Boolean_imp = "import from Boolean";
	String AdditiveType_txt = "AdditiveType: Category == with;";
	String Evalable_txt = "Evalable(T: AdditiveType): Category == with { eval: % -> T; }";
	String Obj_txt =
		"Obj(R: with): Category == AdditiveType with { x: %;"
		"	if R has AdditiveType then {"
		"            Evalable(R);"
		"            foo: () -> Evalable(R)}}";
	String AnAdditive_txt = "AnAdditive: AdditiveType == add;";

	StringList lines = listList(String)(5, Boolean_imp, AdditiveType_txt,
					    Evalable_txt, Obj_txt, AnAdditive_txt);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	AbSyn ab, ab2;
	SymeList symes, l;
	Syme syme;
	TForm tf;
	Stab stab;

	initFile();
	ablogDebug = 0;
	tipBupDebug = 0;
	titfDebug = 0;
	tfDebug = 0;
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	ab = abqParse("Obj(AnAdditive)");
	abPutUse(absyn, AB_Use_NoValue);
	typeInfer(stab, ab);
	testIntEqual("Error Count", 0, comsgErrorCount());
	tf = abTUnique(ab);
	l = tfGetThdExports(tf);
	testIntEqual("Export count", 6, listLength(Syme)(l));

	ab2 = abqParse("X: Obj(AnAdditive)");
	abPutUse(ab2, AB_Use_NoValue);
	scopeBind(stab, ab2);
	typeInfer(stab, ab2);
	testIntEqual("Error Count", 0, comsgErrorCount());
	symes = stabGetMeanings(stab, ablogTrue(), symInternConst("X"));
	testIntEqual("Inferred X", 1, listLength(Syme)(symes));
	syme = car(symes);
	tf = symeType(syme);
	symes = tfGetDomImports(tf);
	symeListPrintDb(symes);
	finiFile();

}

void
testConditionalTInfer4()
{
	/*
	 AdditiveType: Category == with;
	 AdditiveType1: Category == AdditiveType with;
	 AdditiveType2: Category == AdditiveType with;
         Evalable(T: AdditiveType): Category == with { eval: % -> T; }
	 Obj(R: with): Category == with { x: %; if R has AdditiveType then if R has Evalable(R) then Evalable(R) }
	*/
	String Boolean_imp      = "import from Boolean";
	String AdditiveType_txt = "AdditiveType: Category == with";
	String AdditiveType1_txt = "AdditiveType1: Category == AdditiveType with";
	String AdditiveType2_txt = "AdditiveType2: Category == AdditiveType with";
	String Evalable_txt     = "Evalable(T: AdditiveType): Category == with { eval: % -> T; }";
	String Obj_txt          =
		"Obj(R: with): Category == with { "
		"   x: %; "
		"   if R has AdditiveType1 then if R has Evalable(R) then Evalable(R);"
		"   if R has AdditiveType2 then if R has Evalable(R) then Evalable(R); }";

	StringList lines = listList(String)(6, Boolean_imp,
					    AdditiveType_txt,
					    AdditiveType1_txt,
					    AdditiveType2_txt,
					    Evalable_txt, Obj_txt);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	tfImportDebug = 0;
	ablogDebug = 0;
	tipBupDebug = 0;
	titfDebug = 0;
	tfDebug = 0;

	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	finiFile();
}

void
testConditionalAdd()
{
	String Boolean_imp         = "import from Boolean";
	String AdditiveGroup_def   = "AdditiveGroup: Category == with { 0: % }";
	String IndexedCategory_def = "IndexedCategory(X: with): Category == with {if X has AdditiveGroup then AdditiveGroup}";
	String IndexedObject_def   = "IndexedObject(S: with): with IndexedCategory(S) == add { if S has AdditiveGroup then 0: % == 0$S pretend %}";
	String Obj_def             = "Obj: AdditiveGroup with == add { 0: % == never }";

	StringList lines = listList(String)(5, Boolean_imp,
					    AdditiveGroup_def,
					    IndexedCategory_def,
					    IndexedObject_def,
					    Obj_def);


	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	tfImportDebug = 0;
	ablogDebug = 0;
	tipBupDebug = 1;
	titfDebug = 0;
	titfOneDebug = 0;
	tfDebug = 0;
	tcDebug = 0;
	tipAddDebug = 0;
	tfImportDebug = 0;

	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);


	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());
#if 0
	String B_def = "B: IndexedCategory(Obj) with { foo: () -> % } == IndexedObject(Obj) add { foo(x: %): % == 0 }";
	AbSyn B_ab = abqParse(B_def);

	abPutUse(B_ab, AB_Use_NoValue);
	scopeBind(stab, B_ab);
	typeInfer(stab, B_ab);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());
#endif
	finiFile();
}

void
testTinfer5()
{
	String Boolean_imp         = "import from Boolean";
	String R_def   = "R: Category == with";
	String M_def   = "M(T: R): with == add";
	String V_def   = "V(T: Type): Category == with { if T has R then o: % -> M T }";
	StringList lines;

	AbSynList code;
	AbSyn absyn;
	Stab stab;

	initFile();
	lines = listList(String)(4, Boolean_imp, R_def, M_def, V_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	finiFile();
}

void
testTinfer9()
{
	String Bar_def = "Bar: with { f: () -> %; a: % } == add { f():  % == a$% }";
	StringList lines;

	AbSynList code;
	AbSyn absyn;
	Stab stab;

	initFile();
	lines = listList(String)(1, Bar_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();
	tipLitDebug = 1;

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 2, comsgErrorCount());

	finiFile();
}


void
testTinferMutualReference()
{
	String Foo_def = "Foo(F: with): with { f: () -> %;} == add { f(): % == (f()$Bar(F)) pretend %; }";
	String Bar_def = "Bar(B: with): with { f: () -> %;} == add { f(): % == (f()$Foo(B)) pretend % }";
	StringList lines;

	AbSynList code;
	AbSyn absyn;
	Stab stab;

	initFile();
	lines = listList(String)(2, Foo_def, Bar_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	finiFile();
}

void
testTinferValueConditionalAliased()
{
	String I_def = "I: with { zero?: % -> Boolean } == add { zero?(t: %): Boolean == never }";
	String C_def =
		"C(i: I): with { if zero? i then { foo: % -> () } } == "
		"    add { if zero? i then foo(i: %): () == never };";
	StringList lines;

	AbSynList code;
	AbSyn absyn;
	Stab stab;

	initFile();
	lines = listList(String)(2, I_def, C_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	finiFile();
}

void
testTinferValueConditional()
{
	String I_def = "I: with { zero?: % -> Boolean } == add { zero?(t: %): Boolean == never }";
	String C_def =
		"C(i: I): with { if zero? i then { foo: % -> () } } == "
		"    add { import from I; if zero? i then foo(n: %): () == never };";
	StringList lines;
	AbSynList code;
	AbSyn absyn;
	Stab stab;

	initFile();

	lines = listList(String)(2, I_def, C_def);
	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();
	tfImportDebug = 1;
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	finiFile();
}

void
testTinferImport()
{
	initFile();
	stdscope(stabFile());

	tfqTypeInfer(stabFile(), "T: with == add");
	tfqTypeInferFails(stabFile(), "f(): T == { import from 'a' }");
	tfqTypeInferFails(stabFile(), "f(): T == { import from 'a', 'b' }");
	tfqTypeInferFails(stabFile(), "f(): T == { inline from 'a', 'b' }");
	tfqTypeInfer(stabFile(), "f(): 'a' == { import from 'a'; a }");
	tfqTypeInfer(stabFile(), "X: Category == with { import from 'a' }");

	finiFile();
}
