#include "axlobs.h"
#include "fbox.h"
#include "flog.h"
#include "of_cprop.h"
#include "testlib.h"
#include "syme.h"

local void testCPropSimple(void);
local void testCPropLex(void);

void ofCPropTest()
{
	init();
	TEST(testCPropSimple);
	//TEST(testCPropLex);
}

extern Bool cpDebug, cpDfDebug;

local void
testCPropSimple()
{
	FoamList params;
	FoamList locals;
	FoamBox  body;
	Foam prog;

	params = listList(Foam)(1, foamNewDecl(FOAM_SInt, strCopy(""), int0));
	locals = listList(Foam)(1, foamNewDecl(FOAM_SInt, strCopy(""), int0));

	body = fboxNewEmpty(FOAM_Seq);

	fboxAdd(body, foamNewSet(foamNewLoc(int0), foamNewPar(int0)));
	fboxAdd(body, foamNewReturn(foamNewLoc(int0)));

	prog = foamNewProgEmpty();
	prog->foamProg.params = foamNewDDecl(FOAM_DDecl_Param, params);
	prog->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local, locals);
	prog->foamProg.body = fboxMake(body);

	FlowGraph flog = flogFrProg(prog, FLOG_UniqueExit);

	Bool result = cpFlog(flog);
	testTrue("cprop", result);

	int nLabels;
	Foam seq = flogToSeq(flog, &nLabels);

	Foam ret = foamFindFirstTag(FOAM_Return, seq);
	testIsNotNull("found", ret);
	testTrue("eq", foamEqual(foamNewPar(int0), ret->foamReturn.value));
}


local void
testCPropLex()
{
	FoamList params;
	FoamList locals;
	FoamBox  body;
	Foam prog;

	params = listList(Foam)(1, foamNewDecl(FOAM_SInt, strCopy(""), int0));
	locals = listList(Foam)(0);

	body = fboxNewEmpty(FOAM_Seq);

	fboxAdd(body, foamNewSet(foamNewPar(int0), foamNewLex(1, 1)));
	fboxAdd(body, foamNewReturn(foamNewLex(1, 1)));

	prog = foamNewProgEmpty();
	prog->foamProg.params = foamNewDDecl(FOAM_DDecl_Param, params);
	prog->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local, locals);
	prog->foamProg.body = fboxMake(body);

	FlowGraph flog = flogFrProg(prog, FLOG_UniqueExit);

	cpDebug = 1;
	cpDfDebug = 1;

	Bool result = cpFlog(flog);
	testTrue("cprop", result);

	int nLabels;
	Foam seq = flogToSeq(flog, &nLabels);

	Foam ret = foamFindFirstTag(FOAM_Return, seq);
	testIsNotNull("found", ret);
	testTrue("eq", foamEqual(foamNewPar(int0), ret->foamReturn.value));
}

