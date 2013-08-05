#include "testlib.h"

#include "abquick.h"
#include "stab.h"
#include "tfsat.h"
#include "absub.h"

local void testTfSatEmbed();

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

