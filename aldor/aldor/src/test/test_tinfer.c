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
#include "symbol.h"
#include "testlib.h"
#include "tinfer.h"
#include "ti_sef.h"

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);
void finiFile(void);

void testSelfTInfer();
void testSimpleTInfer();
void testConditionalTInfer();
void testConditionalTInfer2();
void testConditionalTInfer4();
void testConditionalAdd();
void testTinfer3();
void testTinfer5();
void testTinfer9();

local AbSynList parseLines(StringList lines);

AbSyn stdtypes();

void init(void);
void fini(void);

void initFile();
void finiFile();

void tinferTest()
{
	init();
	//TEST(testSimpleTInfer);

	TEST(testSelfTInfer);
	TEST(testConditionalTInfer);
	TEST(testConditionalTInfer2);
	TEST(testTinfer3);
	TEST(testConditionalTInfer4);

	TEST(testConditionalAdd);
	TEST(testTinfer5);
	TEST(testTinfer9);
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

	initFile();
	ablogDebug = 0;
	tipBupDebug = 0;
	tfDebug = 0;
	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
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
	tipBupDebug = tipTdnDebug = tfsDebug = tfsParentDebug = tfsExportDebug = 0;
	sefoEqualDebug = 0;

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

	AbSyn F = abqParse(F_def);
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
	AbSynList code = listCons(AbSyn)(stdtypes(), parseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	initFile();
	ablogDebug = 0;
	tipBupDebug = 0;
	titfDebug = 0;
	Stab stab = stabFile();
	
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
	AbSynList code = listCons(AbSyn)(stdtypes(), parseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	initFile();
	ablogDebug = 0;
	tipBupDebug = 0;
	titfDebug = 0;
	tfDebug = 0;
	Stab stab = stabFile();
	
	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	
	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	AbSyn ab = abqParse("Obj(AnAdditive)");
	abPutUse(absyn, AB_Use_NoValue);
	typeInfer(stab, ab);
	testIntEqual("Error Count", 0, comsgErrorCount());
	TForm tf = abTUnique(ab);
	SymeList l = tfGetThdExports(tf);
	testIntEqual("Export count", 6, listLength(Syme)(l));

	AbSyn ab2 = abqParse("X: Obj(AnAdditive)");
	abPutUse(ab2, AB_Use_NoValue);
	scopeBind(stab, ab2);
	typeInfer(stab, ab2);
	testIntEqual("Error Count", 0, comsgErrorCount());
	SymeList symes = stabGetMeanings(stab, ablogTrue(), symInternConst("X"));
	testIntEqual("Inferred X", 1, listLength(Syme)(symes));
	Syme syme = car(symes);
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
	AbSynList code = listCons(AbSyn)(stdtypes(), parseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);
	initFile();
	tfImportDebug = 0;
	ablogDebug = 0;
	tipBupDebug = 0;
	titfDebug = 0;
	tfDebug = 0;

	Stab stab = stabFile();
	
	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	
	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());


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

					    
	AbSynList code = listCons(AbSyn)(stdtypes(), parseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

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

	Stab stab = stabFile();
	
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
	initFile();
	String Boolean_imp         = "import from Boolean";
	String R_def   = "R: Category == with";
	String M_def   = "M(T: R): with == add";
	String V_def   = "V(T: Type): Category == with { if T has R then o: % -> M T }";

	StringList lines = listList(String)(4, Boolean_imp, R_def, M_def, V_def);

	AbSynList code = listCons(AbSyn)(stdtypes(), parseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);
	Stab stab = stabFile();
	
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

	initFile();
	StringList lines = listList(String)(1, Bar_def);

	AbSynList code = listCons(AbSyn)(stdtypes(), parseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);
	Stab stab = stabFile();
	tipLitDebug = 1;

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	
	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 2, comsgErrorCount());
	
	finiFile();
}


local 
AbSynList parseLines(StringList lines) 
{
	AbSynList result = listNil(AbSyn);
	while (lines != listNil(String)) {
		result = listCons(AbSyn)(abqParse(car(lines)), result);
		lines = listFreeCons(String)(lines);
	}
	return listNReverse(AbSyn)(result);
}

void initFile()
{
	macexInitFile();
	comsgInit();
	scobindInitFile();
	stabInitFile();
}

void finiFile()
{
	scobindFiniFile();
	stabFiniFile();
	comsgFini();
	macexFiniFile();

	cmdDebugReset();
}

void 
init()
{
	osInit();
	sxiInit();
	keyInit();
	ssymInit();
	dbInit();
	stabInitGlobal();
	tfInit();
	foamInit();
	optInit();
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
