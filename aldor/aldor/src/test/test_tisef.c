#include "abquick.h"
#include "abuse.h"
#include "testlib.h"
#include "ti_sef.h"
#include "scobind.h"
#include "tinfer.h"
#include "stab.h"

local void testTiSefGenCross(void);
extern int tipSefDebug;

void
tisefTest()
{
	init();
	TEST(testTiSefGenCross);
	fini();
}

local void
testTiSefGenCross()
{
	String R_def = "R: with == add";
	StringList lines = listList(String)(1, R_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = stdtypes();

	initFile();
	Stab stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	AbSyn test1 = abqParse("generate yield ()");
	tiSefo(stab, test1);
	testTrue("G1", tfIsGenerator(abTUnique(test1)));
	testTrue("G2", tfIsAnyGenerator(abTUnique(test1)));

	AbSyn test2 = abqParse("xgenerate yield ()");
	tiSefo(stab, test2);
	testTrue("XG1", tfIsXGenerator(abTUnique(test2)));
	testTrue("XG2", tfIsAnyGenerator(abTUnique(test2)));

	finiFile();
}


