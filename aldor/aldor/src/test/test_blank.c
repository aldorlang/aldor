#include "abquick.h"
#include "testlib.h"
#include "stab.h"
#include "abuse.h"
#include "scobind.h"
#include "tinfer.h"
#include "sefo.h"
#include "comsg.h"

void testBlank_0(void);

void
blankTest()
{
	init();

	//TEST(testBlank_0);
	fini();
}


void
testBlank_0()
{
	//f(x: ?): X == x
	String X_def = "X: with == add";
	String f_def = "f(x: ?): X == x";

	StringList lines = listList(String)(2, X_def, f_def);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());
	finiFile();
}

