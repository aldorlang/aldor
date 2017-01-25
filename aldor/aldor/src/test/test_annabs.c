#include "testall.h"
#include "testlib.h"

#include "abquick.h"
#include "annabs.h"
#include "abuse.h"
#include "absyn.h"
#include "scobind.h"
#include "sexpr.h"
#include "stab.h"
#include "tinfer.h"

local void testAnnotateSimple(void);

void annotateAbSynTest()
{
	init();
	TEST(testAnnotateSimple);
	fini();
}

local void
testAnnotateSimple()
{
	String fDef = "f(): () == never";
	String fCall = "f()";
	
	StringList lines = listList(String)(2, fDef, fCall);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);
	Stab stab;
	
	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	SExpr sx = abToAnnotatedSExpr(absyn);

	sxiWrite(stdout, sx, SXRW_Default);

	finiFile();
}
