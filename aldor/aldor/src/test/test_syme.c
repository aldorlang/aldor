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
#include "symbol.h"
#include "tform.h"

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
	
	finiFile();
}

local void
testSymeAddCondition()
{
	String C_txt = "C: Category == with";
	String D1_txt = "D1: with == add";
	String D2_txt = "D2: with == add";
	StringList lines = listList(String)(3, C_txt, D1_txt, D2_txt);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	
	AbSyn absyn = abNewSequenceL(sposNone, code);

	initFile();
	Stab stab = stabFile();
	
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);
	
	AbSyn D1 = abFrSyme(uniqueMeaning(stabFile(), "D1"));
	AbSyn D2 = abFrSyme(uniqueMeaning(stabFile(), "D2"));
	AbSyn C = abFrSyme(uniqueMeaning(stabFile(), "C"));
	Syme syme1 = symeNewExport(symInternConst("syme2"), tfNewAbSyn(TF_General, id("D")), car(stab));
	symeAddCondition(syme1, sefo(has(D1, C)), true);
	testIntEqual("test1", 1, listLength(Sefo)(symeCondition(syme1)));

	Syme syme2 = symeNewExport(symInternConst("syme1"),tfNewAbSyn(TF_General, id("D")), car(stab));
	symeAddCondition(syme2, sefo(and(has(D1, C),
					 has(D2, C))), true);
	
	testIntEqual("test2", 2, listLength(Sefo)(symeCondition(syme2)));

	finiFile();
}
