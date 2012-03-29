#include "axlobs.h"
#include "abquick.h"
#include "testlib.h"
#include "tinfer.h"
#include "ti_sef.h"

void testSimpleTInfer();
void testConditionalTInfer();
void testSelfTInfer();
AbSyn stdtypes();

void init(void);
void fini(void);

void initFile();
void finiFile();

void tinferTest()
{
	init();
	TEST(testSimpleTInfer);
	TEST(testSelfTInfer);
	TEST(testConditionalTInfer);
	fini();
}

void
testSimpleTInfer()
{
	AbSyn absyn = define(declare(id("x"), emptyWith()), emptyAdd());
	initFile();
	Stab stab = stabFile();

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
extern int tipTdnDebug;
extern int sefoEqualDebug;

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

	initFile();
	ablogDebug = 0;
	tipBupDebug = 0;
	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 1, comsgErrorCount());

	TForm tf = abTForm(fooD1);
	SymeList sl = tiAddSymes(abStab(theAdd), theAdd->abAdd.capsule,
				 abTForm(theAdd->abAdd.base), tf, (SymeList *) NULL);
	testIntEqual("SymeList Length", 1, listLength(Syme)(sl));
	finiFile();
}

local Syme uniqueMeaning(Stab stab, String s);

void
testSelfTInfer()
{
	String Boolean_imp = "import from Boolean";
	String XAlgebra_def = "XAlgebra(T: with): Category == with";
	String XIntegralDomain_def = "XIntegralDomain: Category == XAlgebra(%) with";
	String XLocalAlgebra_def = "XLocalAlgebra(R: with, S: XAlgebra R): with == add";
	String D_def = "D: with == add; F(U: with): XIntegralDomain with == add";
	String E_def = "export a: XLocalAlgebra(F D, F D)";

	StringList lines = listList(String)(5, Boolean_imp, XAlgebra_def, XIntegralDomain_def,
					    XLocalAlgebra_def, D_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);
	initFile();

	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	Syme xalgebra = uniqueMeaning(stab, "XAlgebra");
	Syme d = uniqueMeaning(stab, "D");
	AbSyn sefo = abNewApply1(sposNone, abFrSyme(xalgebra), abFrSyme(d));
	TForm tf = tfFullFrAbSyn(stab, sefo);

	symePrintDb(d);
	tipBupDebug = tipTdnDebug = tfsDebug = tfsParentDebug = tfsExportDebug = 1;
	sefoEqualDebug = 1;

	afprintf(dbOut, "Testing %pAbSyn satisfies %pTForm\n", abFrSyme(d),  tf);
	tfSatArg(tfSatBupMask(), abFrSyme(d), tf);

	Sefo dSefo = abFrSyme(d);
	tiSefo(stab, dSefo);
	TForm dTf = abTUnique(dSefo);
	afprintf(dbOut, "Type of D is %pTForm\n", dTf);
	SymeList dCatSelfList = tfGetCatSelf(dTf);
	afprintf(dbOut, "Self for Type of D is %pSymeList\n", dCatSelfList);
	listIter(Syme, dCatSelf, dCatSelfList,
		 afprintf(dbOut, "Self: %s Type: %pTForm\n", symeString(dCatSelf), symeType(dCatSelf)));


	AbSyn selfRef = abqParse(E_def);

	abPutUse(selfRef, AB_Use_NoValue);
	scopeBind(stab, selfRef);
	typeInfer(stab, selfRef);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	finiFile();
}

local Syme
uniqueMeaning(Stab stab, String s)
{
	SymeList symesForString = stabGetMeanings(stab, ablogTrue(), symIntern(s));
	testNull("", cdr(symesForString));
	Syme d = car(symesForString);

	return d;
}


void initFile()
{
	scobindInitFile();
	stabInitFile();
}

void finiFile()
{
	scobindFiniFile();
	stabFiniFile();
}

void init()
{
	osInit();
	sxiInit();
	keyInit();
	ssymInit();
	dbInit();
	stabInitGlobal();
	tfInit();
	foamInit();
	optSetInit();
	tinferInit();

	sposInit();
	ablogInit();
	comsgInit();
}

void 
fini()
{
	saveAndEmptyAllPhaseSymbolData();
}
