#include "testlib.h"

#include "abquick.h"
#include "absub.h"
#include "stab.h"
#include "tfsat.h"
#include "sefo.h"
#include "abuse.h"
#include "scobind.h"
#include "tinfer.h"
#include "comsg.h"
#include "format.h"

local void testTfSatEmbed();
local void testTfSatEmbedExcept();
local void testTfSatRec();
extern int tfsDebug;

void
tfsatTest()
{
	init();
	TEST(testTfSatEmbed);
	TEST(testTfSatEmbedExcept);
	TEST(testTfSatRec);
	fini();
}

local void
testTfSatEmbed()
{
	String Boolean_imp = "import from Boolean";
	String E_def = "E: with == add";
	String g_def = "g: E == never";

	StringList lines = listList(String)(3, Boolean_imp, E_def, g_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	Syme g = uniqueMeaning(stab, "g");
	TForm E = symeType(g);
	TForm tf1 = tfCross(2, E, E);
	TForm tf2 = tfMulti(2, E, E);
	
	AbSub sigma = absNew(stab);

	int mask;
	SatMask result;
	mask = tfSatTdnMask();
	result = tfSat(mask, tf1, tf2);
	
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_CrossToMulti, tfSatAbEmbed(result));

	mask = tfSatTdnMask();
	result = tfSat(mask, tf2, tf1);
	
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_MultiToCross, tfSatAbEmbed(result));

	result = tfSat(mask, tf2, tfSubst(sigma, tf1));
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_MultiToCross, tfSatAbEmbed(result));

	result = tfSat(mask, tfSubst(sigma, tf1), tf2);
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_CrossToMulti, tfSatAbEmbed(result));

	finiFile();
}

local void
testTfSatEmbedExcept()
{
	String Boolean_imp = "import from Boolean";
	String E_def = "E: Category == with";
	String T_def = "T: with == add";
	String e_def = "e: E == never";
	String t_def = "t: T == never";

	StringList lines = listList(String)(5, Boolean_imp, E_def, e_def, T_def, t_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	int mask;
	SatMask result;

	TForm T = symeType(uniqueMeaning(stab, "t"));
	TForm E = symeType(uniqueMeaning(stab, "e"));
	TForm TExceptE = tfExcept(T, T);
	
	mask = tfSatTdnMask();
	/* T satisfies T */
	result = tfSat(mask, T, T);
	testTrue("", tfSatSucceed(result));
	testIntEqual("", 0, tfSatAbEmbed(result));

	/* T satisfies "T except E" */
	result = tfSat(mask, T, TExceptE);
	testTrue("", tfSatSucceed(result));
	testIntEqual("", 0, tfSatAbEmbed(result));

	/* (T) satisfies "(T) except E" */
	TForm MultiT = tfMulti(1, T);
	TForm MultiTExceptE = tfExcept(tfMulti(1, T), E);
	result = tfSat(mask, MultiT, MultiTExceptE);
	testTrue("", tfSatSucceed(result));
	testIntEqual("", 0, tfSatAbEmbed(result));

	/* (T) satisfies "T except E" */
	result = tfSat(mask, MultiT, TExceptE);
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_MultiToUnary, tfSatAbEmbed(result));

	/* T satisfies "(T)" */
	result = tfSat(mask, MultiT, T);
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_MultiToUnary, tfSatAbEmbed(result));

	finiFile();
}

void
testTfSatRec()
{
	String T_def = "T: with == add";
	String R_def = "R == Record(t: T)";
	String r_def = "local r: R";
	String s_def = "local s: Record(t: T)";

	initFile();
	StringList lines = listList(String)(4, T_def, R_def, r_def, s_def);

	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);
	Stab stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	Syme r = uniqueMeaning(stab, "r");
	TForm rtf = symeType(r);
	Syme s = uniqueMeaning(stab, "s");
	TForm stf = symeType(s);
	aprintf("R: %pTForm S: %pTForm\n", rtf, stf);

	testTrue("def eq", tfSatisfies(rtf, stf));

	finiFile();
}

