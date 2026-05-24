#include "abquick.h"
#include "testlib.h"
#include "debug.h"
#include "orenv.h"
#include "stab.h"
#include "abuse.h"
#include "scobind.h"
#include "tform.h"
#include "tinfer.h"

extern int tipSolveDebug;
extern int infEnvDebug;

void testOrEnv_0(void);
void testOrEnv_Big(void);

void
orEnvTest()
{
	init();

	TEST(testOrEnv_0);
	TEST(testOrEnv_Big);

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

	infEnvExtend(infEnv, tfV99, tfX);

	OrEnv orEnv1  = orEnvOne(infEnvEmpty());
	OrEnv orEnv2  = orEnvOne(infEnv);
	OrEnv orEnvR = orEnvAnd(orEnv1, orEnv2);

	afprintf(dbOut, "1: %pOrEnv\n", orEnv1);
	afprintf(dbOut, "2: %pOrEnv\n", orEnv2);
	afprintf(dbOut, "R: %pOrEnv\n", orEnvR);

	finiFile();
}

void
testOrEnv_Big()
{
/*
  (InfEnv
	(<* Variable(100) *> -> <* General[W] (* % *) *>)
	(<* Variable(101) *> -> <* General[X] (* SExpression2 *) *>)
	(<* Variable(200) *> == <* Variable(500) *>)
	(<* Variable(201) *> == <* Variable(501) *>)
	(<* Variable(300) *> == <* Variable(600) *>)
	(<* Variable(301) *> == <* Variable(601) *>)
	(<* Variable(400) *> == <* Variable(100) *>)
	(<* Variable(401) *> -> <* General[X] (* SExpression2 *) *>)
        (<* Variable(500) *>)
	(<* Variable(501) *> -> <* General[X] (* SExpression2 *) *>)
	(<* Variable(600) *>)
	(<* Variable(601) *>)

)])
	
 OrEnvAnd[296]: 2:

(Or[745](4) [

(InfEnv (<* Variable(300) *> == <* Variable(101) *>)
        (<* Variable(301) *> == <* Variable(101) *>)
	(<* Variable(101) *>)),

(InfEnv (<* Variable(101) *> -> <* General[X] (* SExpression2 *) *>)),
(InfEnv (<* Variable(101) *> -> <* Enumeration[Y] <* Declare <* Type *> ( {* (LexConst) car *} ) *> ( {* (Export) car *} ) *>)),
(InfEnv (<* Variable(101) *> -> <* Enumeration[Z] <* Declare <* Type *> ( {* (LexConst) cdr *} ) *> ( {* (Export) cdr *} ) *>))])
*/

	String W_def = "W: with == add";
	String X_def = "X: with == add";
	String Y_def = "Y: with == add";
	String Z_def = "Z: with == add";

	StringList lines = listList(String)(4, W_def, X_def, Y_def, Z_def);
	AbSynList code = listCons(AbSyn)(stdtypes(), abqParseLines(lines));
	AbSyn absyn = abNewSequenceL(sposNone, code);

	Stab stab;

	initFile();
	stab = stabFile();
	abPutUse(absyn, AB_Use_NoValue);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	TForm tfW   = tfqTypeForm(stab, "W");
	TForm tfX   = tfqTypeForm(stab, "X");
	TForm tfY   = tfqTypeForm(stab, "Y");
	TForm tfZ   = tfqTypeForm(stab, "Z");
	TForm tfV100 = tfVarFrId(100);
	TForm tfV101 = tfVarFrId(101);
	TForm tfV200 = tfVarFrId(200);
	TForm tfV201 = tfVarFrId(201);
	TForm tfV300 = tfVarFrId(300);
	TForm tfV301 = tfVarFrId(301);
	TForm tfV400 = tfVarFrId(400);
	TForm tfV401 = tfVarFrId(401);
	TForm tfV500 = tfVarFrId(500);
	TForm tfV501 = tfVarFrId(501);
	TForm tfV600 = tfVarFrId(600);
	TForm tfV601 = tfVarFrId(601);

	InferEnv infEnv = infEnvNew();

	infEnvExtend(infEnv, tfV100, tfW);
	infEnvExtend(infEnv, tfV101, tfX);
	infEnvExtend(infEnv, tfV200, tfV500);
	infEnvExtend(infEnv, tfV201, tfV501);
	infEnvExtend(infEnv, tfV300, tfV600);
	infEnvExtend(infEnv, tfV301, tfV601);
	infEnvExtend(infEnv, tfV400, tfV100);
	infEnvExtend(infEnv, tfV401, tfX);
	infEnvExtend(infEnv, tfV501, tfX);

	OrEnv orEnvA  = orEnvOne(infEnv);

	InferEnv infEnv1 = infEnvNew();
	InferEnv infEnv2 = infEnvNew();
	InferEnv infEnv3 = infEnvNew();
	InferEnv infEnv4 = infEnvNew();

	infEnvExtend(infEnv1, tfV300, tfV101);
	infEnvExtend(infEnv1, tfV301, tfV101);

	infEnvExtend(infEnv2, tfV101, tfX);
	infEnvExtend(infEnv3, tfV101, tfY);
	infEnvExtend(infEnv4, tfV101, tfZ);

	ENABLE_DEBUG(infEnvDebug);
	ENABLE_DEBUG(tipSolveDebug);

	InferEnv firstEnv = infEnvMerge(infEnv, infEnv1);
	
	afprintf(dbOut, "TestMerge\nEnv: \n%pInferEnv\n%pInferEnv\nResult:\n%pInferEnvn", infEnv, infEnv1, firstEnv);
/*	
	OrEnv orEnvB  = orEnvOne(infEnv1);
	orEnvAdd(orEnvB, infEnv2);
	//orEnvAdd(orEnvB, infEnv3);
	//orEnvAdd(orEnvB, infEnv4);

	afprintf(dbOut, "*** Or 1 2...\n", orEnvA);
	OrEnv orEnvR = orEnvAnd(orEnvA, orEnvB);
	
	afprintf(dbOut, "1: %pOrEnv\n", orEnvA);
	afprintf(dbOut, "2: %pOrEnv\n", orEnvB);
	afprintf(dbOut, "R: %pOrEnv\n", orEnvR);
*/
	finiFile();

}
