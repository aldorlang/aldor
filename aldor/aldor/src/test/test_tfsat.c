#include "testlib.h"

#include "abquick.h"
#include "stab.h"
#include "tfsat.h"
#include "sefo.h"

local void testTfSatEmbed();
local void testTfSatRec();

/* XXX: from test_tinfer.c */
void init(void);
void fini(void);
void initFile(void);
void finiFile(void);

void
tfsatTest()
{
	init();
	TEST(testTfSatEmbed);
	TEST(testTfSatRec);
	fini();
}

local void
testTfSatEmbed()
{
	String Boolean_imp = "import from Boolean";
	String E_def = "E: with == add";
	String f_def = "f(): (E, E) == never";
	String g_def = "g: E == never";

	StringList lines = listList(String)(4, Boolean_imp, E_def, f_def, g_def);
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

