#include "abquick.h"
#include "testlib.h"
#include "debug.h"
#include "orenv.h"
#include "stab.h"
#include "abuse.h"
#include "scobind.h"
#include "tform.h"
#include "tinfer.h"

void testOrEnv_0(void);

void
orEnvTest()
{
	init();

	TEST(testOrEnv_0);

	fini();
}

void
testOrEnv_0()
{
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

	TForm tfX   = tfqTypeForm(stab, "X");
	TForm tfV99 = tfVarFrId(99);

	InferEnv infEnv = infEnvNew();

	infEnvSet(infEnv, tfV99, tfX);

	OrEnv orEnv1  = orEnvOne(infEnvEmpty());
	OrEnv orEnv2  = orEnvOne(infEnv);
	OrEnv orEnvR = orEnvAnd(orEnv1, orEnv2);

	afprintf(dbOut, "1: %pOrEnv\n", orEnv1);
	afprintf(dbOut, "2: %pOrEnv\n", orEnv2);
	afprintf(dbOut, "R: %pOrEnv\n", orEnvR);

	finiFile();
}
