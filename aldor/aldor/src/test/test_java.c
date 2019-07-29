#include "axlobs.h"
#include "foam.h"
#include "testlib.h"
#include "abquick.h"
#include "tform.h"
#include "stab.h"
#include "spesym.h"
#include "abuse.h"
#include "scobind.h"
#include "tinfer.h"
#include "sefo.h"

local void testJavaType1(void);
local void testValidation(void);

void
javaTestSuite()
{
	init();
	TEST(testJavaType1);
	TEST(testValidation);
	fini();
}

local void
testJavaType1()
{
	TForm tf;
	SymeList symes;
	AbSyn imp;

	initFile();
	stdscope(stabFile());

	imp = abqParse("import Runnable: with { apply: (%, 'run') -> () -> (); } from Foreign Java \"java.util\"");
	abPutUse(imp, AB_Use_NoValue);
	scopeBind(stabFile(), imp);
	typeInfer(stabFile(), imp);

	tf = tfqTypeForm(stabFile(), "Runnable");
	symes = symeListSubListById(tfGetDomImports(tf), ssymApply);
	testIntEqual("len", 1, listLength(Syme)(symes));

	testTrue("isApply", symeIsJavaApply(car(symes)));
	testStringEqual("name", "run", symeJavaApplyName(car(symes)));

	imp = abqParse("import RunnableX: with { apply: (%, 'run') -> () -> (); } from Foreign C \"foo\"");
	abPutUse(imp, AB_Use_NoValue);
	scopeBind(stabFile(), imp);
	typeInfer(stabFile(), imp);

	tf = tfqTypeForm(stabFile(), "RunnableX");
	symes = symeListSubListById(tfGetDomImports(tf), ssymApply);
	testFalse("isApply", symeIsJavaApply(car(symes)));

	finiFile();
}

local void
testValidation()
{
	TForm tf;
	SymeList symes;
	AbSyn imp;

	initFile();
	stdscope(stabFile());

	imp = abqParse("import Bad1: with { apply: () -> (); } from Foreign Java \"java.util\"");
	abPutUse(imp, AB_Use_NoValue);
	scopeBind(stabFile(), imp);
	typeInfer(stabFile(), imp);
	
	testFalse("NotSefo", abIsSefo(imp->abImport.what));

	finiFile();
}
