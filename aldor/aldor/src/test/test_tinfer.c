#include "axlobs.h"
#include "abquick.h"
#include "testlib.h"
#include "tinfer.h"

void testSimpleTInfer();
void testConditionalTInfer();
AbSyn stdtypes();

void init(void);
void fini(void);

void initFile();
void finiFile();

void tinferTest()
{
	init();
	TEST(testSimpleTInfer);
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
extern int tipBupDebug;

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

void fini()
{
	comsgFini();
}

AbSyn tupleOfType() 
{
	return apply1(id("Tuple"), id("Type"));
}

AbSyn stdtypes() 
{
	/* Type: with == add; */
	AbSyn type = define(declare(id("Type"), emptyWith()), emptyAdd());
	/* Tuple(T: Type): with == add */
	AbSyn tuple = define(declare(id("Tuple"), apply2(id("->"), declare(id("T"), id("Type")), emptyWith())),
			     lambda(comma1(declare(id("T"), id("Type"))),
				    emptyWith(),
				    label(id("Tuple"), emptyAdd())));
	/* Map(A: Tuple Type, R: Tuple Type): with == add; */
	AbSyn map = define(declare(id("->"), 
				   apply2(id("->"), 
					  comma2(declare(id("A"), tupleOfType()),
						 declare(id("R"), tupleOfType())),
					  emptyWith())),
			   lambda(comma2(declare(id("A"), tupleOfType()),
					 declare(id("R"), tupleOfType())),
				  emptyWith(),
				  label(id("->"), emptyAdd())));
	/* Boolean: with == add; */
	AbSyn boolean = define(declare(id("Boolean"), emptyWith()), emptyAdd());

	return abNewSequenceL(sposNone, listList(AbSyn)(4, type, tuple, map, boolean));
}
