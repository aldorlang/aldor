#include "utype.h"
#include "abquick.h"
#include "testlib.h"
#include "tform.h"
#include "stab.h"
#include "syme.h"
#include "absub.h"
#include "debug.h"
#include "testall.h"
#include "tinfer.h"
#include "tfsat.h"

local void test0();
local void test1();
local void test2();
local void testUtfSat();

extern Bool utypeDebug;

void
utypeTestSuite(void)
{
	init();
	TEST(test0);
	TEST(testUtfSat);
	TEST(test1);
	fini();
}

local void
test0()
{
	Sefo sefo1, sefo2;
	Syme param, listSyme, paramSyme, paramCopy1, paramCopy2 ;
	AbSub sigma;
	UTypeResult res;

	initFile();
	stdscope(stabFile());

	tfqTypeInfer(stabFile(), "Monoid: Category == with { 1: %; *: (%, %) -> % }");
	tfqTypeInfer(stabFile(), "Int: Monoid == add { 1: % == never; (a: %) * (b: %): % == never}");
	tfqTypeInfer(stabFile(), "List(T: with): with { empty: () -> %; cons: (T, %) -> % } == add { empty(): % == never; cons(a: T, l: %): % == never}");

	/* lhs: {List(T): T: with} */
	listSyme  = uniqueMeaning(stabFile(), "List");
	paramSyme = tfDeclareSyme(tfMapArgN(symeType(listSyme), 0));
	paramCopy1 = symeClone(paramSyme);
	paramCopy2 = symeClone(paramSyme);


	sefo1 = tfqTypeInfer(stabFile(), "Int");
	res = utypeUnify(utypeNewConstant(sefo1), utypeNewVar(paramCopy1));

	afprintf(dbOut, "Result: %pUTypeResult\n", res);
	testIntEqual("t1", 1, listLength(Syme)(res->symes));

	sefo1 = tfqTypeInfer(stabFile(), "List Int");
	res = utypeUnify(utypeNewConstant(sefo1), utypeNewVar(paramCopy1));

	afprintf(dbOut, "Result: %pUTypeResult\n", res);
	testIntEqual("t2", 1, listLength(Syme)(res->symes));

	sefo1 = tfqTypeInfer(stabFile(), "List Int");
	sefo2 = tfqTypeInfer(stabFile(), "List List Int");
	res = utypeUnify(utypeNewConstant(sefo1), utypeNewConstant(sefo2));

	afprintf(dbOut, "Result: %pUTypeResult\n", res);
	testTrue("t3", utypeResultIsFail(res));

	sefo1 = sefo(apply1(abFrSyme(listSyme), abFrSyme(paramCopy1)));
	sefo2 = tfqTypeInfer(stabFile(), "List Int");
	res = utypeUnify(utypeNew(listSingleton(Syme)(paramCopy1), sefo1), utypeNewConstant(sefo2));

	afprintf(dbOut, "Result: %pUTypeResult\n", res);
	testIntEqual("t2", 1, listLength(Syme)(res->symes));

#define ListOf(x) apply1(abFrSyme(listSyme), x)

	sefo1 = sefo(ListOf(abFrSyme(paramCopy1)));
	sefo2 = sefo(ListOf(abFrSyme(paramCopy2)));

	res = utypeUnify(utypeNew(listSingleton(Syme)(paramCopy1), sefo1),
			 utypeNew(listSingleton(Syme)(paramCopy2), sefo2));

	afprintf(dbOut, "Result: %pUTypeResult\n", res);
	testIntEqual("t2", 2, listLength(Syme)(res->symes));


	sefo1 = sefo(ListOf(abFrSyme(paramCopy1)));
	sefo2 = sefo(ListOf(ListOf(abFrSyme(paramCopy2))));

	res = utypeUnify(utypeNew(listSingleton(Syme)(paramCopy1), sefo1),
			 utypeNew(listSingleton(Syme)(paramCopy2), sefo2));

	afprintf(dbOut, "Result: %pUTypeResult\n", res);
	testIntEqual("t2", 1, listLength(Syme)(res->symes));

	sefo1 = sefo(ListOf(ListOf(abFrSyme(paramCopy1))));
	sefo2 = sefo(ListOf(abFrSyme(paramCopy2)));

	res = utypeUnify(utypeNew(listSingleton(Syme)(paramCopy1), sefo1),
			 utypeNew(listSingleton(Syme)(paramCopy2), sefo2));

	afprintf(dbOut, "Result: %pUTypeResult\n", res);
	testIntEqual("t2", 1, listLength(Syme)(res->symes));

	finiFile();
}

local void 
test1() {
	Sefo sefo1, sefo2;
	Syme param, listSyme, paramSyme, paramCopy1, paramCopy2 ;
	AbSub sigma;
	UTypeResult res;

	initFile();
	stdscope(stabFile());

	tfqTypeInfer(stabFile(), "Monoid: Category == with { 1: %; *: (%, %) -> % }");
	tfqTypeInfer(stabFile(), "Int: Monoid == add { 1: % == never; (a: %) * (b: %): % == never}");
	tfqTypeInfer(stabFile(), "List(T: with): with { empty: () -> %; } == add { empty(): % == never}");
	tfqTypeInfer(stabFile(), "Alg(T: Monoid): with { lift: T -> %; } == add { lift(a: T): % == never}");

	finiFile();
}

local void
test2()
{
	Sefo sefo1, sefo2;
	Syme param, listSyme, paramSyme, paramCopy1, paramCopy2 ;
	AbSub sigma;
	UTypeResult res;

	initFile();
	stdscope(stabFile());

	tfqTypeInfer(stabFile(), "List(T: Type): with { one: T -> %; cons: (T, %) -> %} == add { one(t: T): % == never; cons(x: T, l: %): % == never}");
	tfqTypeInfer(stabFile(), "Monoid: Category == with { 1: %; *: (%, %) -> % }");
	tfqTypeInfer(stabFile(), "Int: Monoid == add { 1: % == never; (a: %) * (b: %): % == never}");

	tfqTypeInfer(stabFile(), "f(x: Int): () == { import from List(X: with); one(x)}");
/*	tfqTypeInfer(stabFile(), "f(x: Int): List Int == one(x)"); TOFIX: Needs tpossUnify */

	tfqTypeInfer(stabFile(), "f(x: Int): () == { import from List(X: with); one(one(x))}");
	tfqTypeInfer(stabFile(), "f(x: Int): () == { import from List(X: with); cons(x, one(x))}");

	finiFile();

}

local void
testUtfSat()
{
	UTForm arrayOfX;
	Sefo sefo1, sefo2;
	SymeList varList;
	Syme param, arraySyme, intSyme, paramSyme, paramCopy1, paramCopy2;
	AbSub sigma;
	UTypeResult res;
	USatMask mask;

	initFile();
	stdscope(stabFile());

	tfqTypeInfer(stabFile(), "Monoid: Category == with { 1: %; *: (%, %) -> % }");
	tfqTypeInfer(stabFile(), "Int: Monoid == add { 1: % == never; (a: %) * (b: %): % == never}");
	tfqTypeInfer(stabFile(), "Array(T: with): with { empty: () -> %; } == add { empty(): % == never}");

	/* lhs: {Array(T): T: with} */
	arraySyme  = uniqueMeaning(stabFile(), "Array");
	intSyme  = uniqueMeaning(stabFile(), "Int");
	paramSyme = tfDeclareSyme(tfMapArgN(symeType(arraySyme), 0));
	paramCopy1 = symeClone(paramSyme);
	paramCopy2 = symeClone(paramSyme);

#define ArrayOf(x) apply1(abFrSyme(arraySyme), x)
	sefo1 = sefo(ArrayOf(abFrSyme(paramCopy1)));
	varList = listSingleton(Syme)(paramCopy1);
	arrayOfX = utformNew(varList, tiGetTForm(stabFile(), sefo1));
	mask = utfSat(tfSatBupMask(),
		      arrayOfX,
		      utformNewConstant(tfqTypeForm(stabFile(), "Int")));
	testFalse("", utfSatSucceed(mask));

	mask = utfSat(tfSatBupMask(), arrayOfX,
		     utformNewConstant(tfqTypeForm(stabFile(), "Array Int")));
	testTrue("", utfSatSucceed(mask));
	mask = utfSat(tfSatBupMask(), arrayOfX,
		     utformNewConstant(tfqTypeForm(stabFile(), "Array Array Int")));
	testTrue("", utfSatSucceed(mask));

	mask = utfSat(tfSatBupMask(), arrayOfX, utformNewConstant(tfUnknown));
	testTrue("", utfSatSucceed(mask));

	mask = utfSat(tfSatBupMask(),  utformNew(listSingleton(Syme)(paramCopy1),
						tfMap(tiGetTForm(stabFile(), abFrSyme(paramCopy1)),
						      tiGetTForm(stabFile(), abFrSyme(paramCopy1)))),
		      utformNewConstant(tfqTypeForm(stabFile(), "Int -> Array Int")));
	testFalse("X -> X", utfSatSucceed(mask));

	mask = utfSat(tfSatBupMask(), utformNew(listList(Syme)(2, paramCopy1, paramCopy2),
						tfMap(tiGetTForm(stabFile(), abFrSyme(paramCopy1)),
						      tiGetTForm(stabFile(), abFrSyme(paramCopy2)))),
		      utformNewConstant(tfqTypeForm(stabFile(), "Int -> Array Int")));
	testTrue("X->Y", utfSatSucceed(mask));
}

