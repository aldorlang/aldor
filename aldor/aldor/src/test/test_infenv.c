#include "abquick.h"
#include "testlib.h"
#include "debug.h"
#include "format.h"
#include "infenv.h"
#include "stab.h"
#include "abuse.h"
#include "scobind.h"
#include "tinfer.h"
#include "sefo.h"
#include "tposs.h"

void testInfEnv_0(void);
void testInfEnv_1(void);
void testInfEnv_2(void);
void testInfEnv_3(void);
void testInfEnv_4(void);

void
infEnvTest()
{
	init();

	TEST(testInfEnv_0);
	TEST(testInfEnv_1);
	TEST(testInfEnv_2);
	TEST(testInfEnv_3);
	TEST(testInfEnv_4);

	fini();
}

extern Bool tfsDebug;

void
testInfEnv_0()
{
	//f(x: ?): X == x
	String X_def = "X: with == add";

	StringList lines = listList(String)(1, X_def);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	TForm tf1 = tfqTypeForm(stab, "X");
	TForm tf2 = tfVarFrId(99);

	TPoss tp1 = tpossSingleton(tf1);
	TPoss tp2 = tpossSingleton(tf2);

	afprintf(dbOut, "Poss1: %pTPoss\n", tp1);
	afprintf(dbOut, "Poss2: %pTPoss\n", tp2);

	TPoss tpI = tpossIntersect(tp1, tp2);

	afprintf(dbOut, "Poss1: %pTPoss\n", tp1);
	afprintf(dbOut, "Poss2: %pTPoss\n", tp2);
	afprintf(dbOut, "Intersection: %pTPoss\n", tpI);

	testIntEqual("", 1, tpossCount(tpI));
	UTFContext utfc = tpossUniqueUTFContext(tpI);

	InferEnv infEnv = uctxtInfEnv(utfc);
	TForm tf = infEnvFollow(infEnv, tf2);
	afprintf(dbOut, "TF: %pTForm\n", tf);

	testTrue("", tfEqual(tf, tf1));

	finiFile();
}

void
testInfEnv_1()
{
	String X_def = "X: with == add";
	String F_def = "F(A: with): with == add";

	StringList lines = listList(String)(2, X_def, F_def);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	TForm tfX = tfqTypeForm(stab, "X");
	TForm tf99 = tfVarFrId(99);
	TForm tf1 = tfCross(2, tf99, tfX);
	TForm tf2 = tfCross(2, tfX, tf99);
	TForm tfR = tfCross(2, tfX, tfX);

	TPoss tp1 = tpossSingleton(tf1);
	TPoss tp2 = tpossSingleton(tf2);

	afprintf(dbOut, "Poss1: %pTPoss\n", tp1);
	afprintf(dbOut, "Poss2: %pTPoss\n", tp2);

	TPoss tpI = tpossIntersect(tp1, tp2);

	afprintf(dbOut, "Poss1: %pTPoss\n", tp1);
	afprintf(dbOut, "Poss2: %pTPoss\n", tp2);
	afprintf(dbOut, "Intersection: %pTPoss\n", tpI);

	testIntEqual("", 1, tpossCount(tpI));
	UTFContext utfc = tpossUniqueUTFContext(tpI);

	InferEnv infEnv = uctxtInfEnv(utfc);
	TForm tf = infEnvFollow(infEnv, tf2);
	afprintf(dbOut, "TF: %pTForm\n", tf);

	//testTrue("", tformEqual(tf, tfR));

	finiFile();
}

void
testInfEnv_2()
{
	String X_def = "X: with == add";
	String F_def = "F(A: with): with == add";

	StringList lines = listList(String)(2, X_def, F_def);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	tfsDebug = 1;

	TForm tfX = tfqTypeForm(stab, "X");
	TForm tf99 = tfVarFrId(99);
	TForm tf98 = tfVarFrId(98);
	TForm tf1 = tfMap(tf99, tf98);
	TForm tf2 = tfMap(tf98, tf99);
	TForm tfR = tfMap(tf99, tf99);

	TPoss tp1 = tpossSingleton(tf1);
	TPoss tp2 = tpossSingleton(tf2);

	afprintf(dbOut, "Poss1: %pTPoss\n", tp1);
	afprintf(dbOut, "Poss2: %pTPoss\n", tp2);

	TPoss tpI = tpossIntersect(tp1, tp2);

	afprintf(dbOut, "Poss1: %pTPoss\n", tp1);
	afprintf(dbOut, "Poss2: %pTPoss\n", tp2);
	afprintf(dbOut, "Intersection: %pTPoss\n", tpI);

	testIntEqual("", 1, tpossCount(tpI));
	UTFContext utfc = tpossUniqueUTFContext(tpI);

	InferEnv infEnv = uctxtInfEnv(utfc);
	TForm tf = infEnvFollow(infEnv, tf2);
	afprintf(dbOut, "TF: %pTForm\n", tf);

	//testTrue("", tformEqual(tf, tfR));

	finiFile();
}

void
testInfEnv_3()
{
	String X_def = "X: with == add";
	String F_def = "F(A: with): with == add";

	StringList lines = listList(String)(2, X_def, F_def);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	tfsDebug = 1;

	TForm tfX = tfqTypeForm(stab, "X");
	TForm tf99 = tfVarFrId(99);

	InferEnv env = infEnvCopy(infEnvEmpty());

	infEnvSet(env, tf99, tfX);
	infEnvSet(env, tf99, tfX);

	TForm tfR = infEnvFollow(env, tfX);

	testTrue("", tformEqual(tfR, tfX));

	finiFile();
}

void
testInfEnv_4()
{
	String Z_def = "Z: with == add";
	String F_def = "F(A: with): with == add";

	StringList lines = listList(String)(1, Z_def);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	tfsDebug = 1;

	TForm tfZ = tfqTypeForm(stab, "Z");
	TForm tf99 = tfVarFrId(99);
	TForm tf98 = tfVarFrId(98);
	TForm tf97 = tfVarFrId(97);

	TForm tfMap99_98 = tfMap(tf99, tf98);
	TForm tfMap98_99 = tfMap(tf98, tf99);
	InferEnv env = infEnvCopy(infEnvEmpty());

	infEnvExtend(env, tf97, tfMap99_98);
	infEnvExtend(env, tf97, tfMap98_99);
	infEnvExtend(env, tf99, tfZ);

	TForm tfX = tfMap(tfZ, tfZ);
	TForm tfR = tformFollowVars(env, tf97);
	afprintf(dbOut, "Expect: %pTForm\n", tfX);
	afprintf(dbOut, "Result: %pInferEnv\n", env);
	afprintf(dbOut, "Result: %pTForm\n", tfR);
	testTrue("", tformEqual(tfR, tfX));

	finiFile();
}

