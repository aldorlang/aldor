#include "utype.h"
#include "abquick.h"
#include "testlib.h"
#include "tform.h"
#include "stab.h"
#include "syme.h"
#include "absub.h"
#include "debug.h"
#include "testall.h"

local void test0();
local void test1();
extern Bool utypeDebug;

void
utypeTestSuite(void)
{
	init();
	TEST(test0);
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

