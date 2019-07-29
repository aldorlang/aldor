#include "abquick.h"
#include "abuse.h"
#include "comsg.h"
#include "debug.h"
#include "foam.h"
#include "genfoam.h"
#include "scobind.h"
#include "stab.h"
#include "testlib.h"
#include "tinfer.h"

local void testForeign(void);

void
genfoamTestSuite()
{
	init();
	TEST(testForeign);
	fini();
}

local void
testForeign()
{
	String Boolean_imp = "import from Boolean";
	String P_def = "Pointer: with == add";
	String C_def = "C: with == add";
	String F_def = "Foreign(T: Type): with == add";
	String R_def = "R == Record(x: Pointer)";
	String fn_imp = "import { fn: R -> () } from Foreign C";
	String tst_def = "test(p: R): () == fn(p)";

	StringList lines = listList(String)(7, Boolean_imp,
					    P_def, C_def, F_def, R_def,
					    fn_imp, tst_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	Stab stab;
	Foam foam;

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	testIntEqual("Error Count", 0, comsgErrorCount());

	foam = generateFoam(stab, absyn, "test");

	finiFile();
	/* At this point, we should check that the 'test' function
	 * calls 'fn' with a type of FOAM_Rec.  In order to do this
	 * nicely, there should be a decent way of searching a blob of
	 * foam for certain properties. */
#if 0
	FoamList pcall = foamFind(fmfAnd(fmfFoamTag(FOAM_PCall), fmfPCallProto(FOAM_Proto_C)),
				  foam);
	testAIntEqual(FOAM_Rec, foamExprType(foam, pcall->first->foamPCall.argv[0]))
#endif

}
