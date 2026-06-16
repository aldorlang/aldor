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
local void testTfSatEnum();
local void testTfSatExitPattern();
local void testTfSatEmbedPattern();

extern int tfsDebug;

void
tfsatTest()
{
	init();
	TEST(testTfSatEmbed);
	TEST(testTfSatEmbedExcept);
	TEST(testTfSatRec);
	TEST(testTfSatEnum);
	TEST(testTfSatExitPattern);
	TEST(testTfSatEmbedPattern);
	fini();
}

local void
testTfSatEmbed()
{
	Stab stab;

	int mask;
	SatMask result;

	Syme g;
	TForm E, tf0, tfCrossEE, tfMultEE;

	AbSub sigma;

	String Boolean_imp = "import from Boolean";
	String E_def = "E: with == add";
	String g_def = "g: E == never";

	StringList lines = listList(String)(3, Boolean_imp, E_def, g_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	g = uniqueMeaning(stab, "g");
	E = symeType(g);
	tf0 = tfMulti(0);
	tfCrossEE = tfCross(2, E, E);
	tfMultEE = tfMulti(2, E, E);

	sigma = absNew(stab);

	mask = tfSatTdnMask();
	result = tfSat(mask, tfCrossEE, tfMultEE);

	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_CrossToMulti, tfSatAbEmbed(result));

	mask = tfSatTdnMask();
	result = tfSat(mask, tfMultEE, tfCrossEE);

	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_MultiToCross, tfSatAbEmbed(result));

	result = tfSat(mask, tfMultEE, tfSubst(sigma, tfCrossEE));
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_MultiToCross, tfSatAbEmbed(result));

	result = tfSat(mask, tfSubst(sigma, tfCrossEE), tfMultEE);
	testTrue("", tfSatSucceed(result));
	testIntEqual("", AB_Embed_CrossToMulti, tfSatAbEmbed(result));

	result = tfSat(mask, E, tf0);
	testTrue("satAnyToNone", tfSatSucceed(result));
	testIntEqual("", AB_Embed_AnyToNone, tfSatAbEmbed(result));

	result = tfSat(mask, tf0, tf0);
	afprintf(stdout, "s1 %s", tfSatMaskToString(result));
	testTrue("satAnyToNone", tfSatSucceed(result));
	testIntEqual("s1", 0, tfSatAbEmbed(result));

	result = tfSat(mask, tfMultEE, tf0);
	afprintf(stdout, "s2 %s", tfSatMaskToString(result));
	testTrue("satAnyToNoneMult", tfSatSucceed(result));
	testIntEqual("s2", AB_Embed_AnyToNone, tfSatAbEmbed(result));

	testIntEqual("E->tf0", AB_Embed_AnyToNone, tfSatEmbedType(E, tf0));
	testIntEqual("C->tf0", AB_Embed_AnyToNone, tfSatEmbedType(tfCrossEE, tf0));
	testIntEqual("M->tf0", AB_Embed_Identity, tfSatEmbedType(tfMultEE, tf0));

	finiFile();
}

local void
testTfSatEmbedExcept()
{
	Stab stab;
	int mask;
	SatMask result;

	TForm T;
	TForm E;
	TForm TExceptE;
	TForm MultiT;
	TForm MultiTExceptE;

	String Boolean_imp = "import from Boolean";
	String E_def = "E: Category == with";
	String T_def = "T: with == add";
	String e_def = "e: E == never";
	String t_def = "t: T == never";

	StringList lines = listList(String)(5, Boolean_imp, E_def, e_def, T_def, t_def);
	AbSynList absynList = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, absynList);

	initFile();
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	T = symeType(uniqueMeaning(stab, "t"));
	E = symeType(uniqueMeaning(stab, "e"));
	TExceptE = tfExcept(T, T);
	
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
	MultiT = tfMulti(1, T);
	MultiTExceptE = tfExcept(tfMulti(1, T), E);
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

	StringList lines;

	AbSynList code;
	AbSyn absyn;
	Stab stab;

	Syme r, s;
	TForm rtf, stf;

	initFile();
	lines = listList(String)(4, T_def, R_def, r_def, s_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();

	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	r = uniqueMeaning(stab, "r");
	rtf = symeType(r);
	s = uniqueMeaning(stab, "s");
	stf = symeType(s);

	testTrue("def eq", tfSatisfies(rtf, stf));

	finiFile();
}

local void
testTfSatEnum()
{
	String T_def = "T: with == add";
	StringList lines;
	AbSynList code;
	AbSyn absyn;
	TForm tf1, tf2;
	Stab stab;
	initFile();
	stdscope(stabFile());

	lines = listList(String)(1, T_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();
	tf1 = tfqTypeForm(stabFile(), "'x'");
	tf2 = tfqTypeForm(stabFile(), "'y'");

	testFalse("enum0", tfSatisfies(tf1, tf2));
	finiFile();
}

local void
testTfSatExitPattern()
{
	String T_def = "T: with == add";
	StringList lines;
	AbSynList code;
	AbSyn absyn;
	TForm tf1, tf2;
	Stab stab;
	initFile();
	stdscope(stabFile());

	lines = listList(String)(1, T_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();

	tf1 = tfExit;
	tf2 = tfPattern(tfqTypeForm(stab, "Integer"));

	testTrue("exitPattern", tfSatisfies(tf1, tf2));
	
	finiFile();
}

local void
testTfSatEmbedPattern()
{
	String T_def = "T: with == add";
	StringList lines;
	AbSynList code;
	AbSyn absyn;
	TForm tf1, tf2;
	Stab stab;
	SatMask satMask;

	initFile();
	stdscope(stabFile());
	tfsDebug = 1;
	
	lines = listList(String)(1, T_def);

	code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	absyn = abNewSequenceL(sposNone, code);
	stab = stabFile();

	tf1 = tfqTypeForm(stab, "Integer");
	tf2 = tfPattern(tfqTypeForm(stab, "Integer"));
	satMask = tfSat(tfSatWithPatContext(tfSatBupMask()), tf1, tf2);
	testTrue("embedPattern", tfSatSucceed(satMask));
	
	finiFile();
}
