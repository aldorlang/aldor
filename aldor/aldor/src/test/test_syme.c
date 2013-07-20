#include "abquick.h"
#include "abuse.h"
#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "scobind.h"
#include "stab.h"
#include "testlib.h"
#include "tinfer.h"
#include "sefo.h"
#include "ablogic.h"
#include "comsg.h"

local void testSymeSExpr();

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);
void finiFile(void);

void
symeTest()
{
	init();
	TEST(testSymeSExpr);
	fini();
}

extern int stabDebug;

local void
testSymeSExpr()
{
	
	String aSimpleDomain = "+++Comment\nDom: Category == with {f: () -> () ++ f\n}";
	StringList lines = listList(String)(1, aSimpleDomain);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	
	AbSyn absyn = abNewSequenceL(sposNone, code);

	initFile();
	Stab stab = stabFile();
	
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	SymeList symes = stabGetMeanings(stab, ablogFalse(), symInternConst("Dom"));
	testIntEqual("unique meaning", 1, listLength(Syme)(symes));

	Syme syme = car(symes);
	SExpr sx = symeSExprAList(syme);
	afprintf(dbOut, "%pSExpr", sx);
	
	finiFile();
}
