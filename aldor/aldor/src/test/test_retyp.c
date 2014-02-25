#include "axlobs.h"
#include "foam.h"
#include "fbox.h"
#include "testlib.h"
#include "syme.h"
#include "sexpr.h"
#include "util.h"
#include "of_retyp.h"

local void rtcTest0();

void
retypeTest()
{
	sxiInit();

	TEST(rtcTest0);
}

local void
rtcTest0()
{
	init();

	Foam unit = foamNewUnit(foamNewEmpty(FOAM_DFmt, 0), foamNewEmpty(FOAM_DDef, 1));
	Foam prog = foamNewProg(/*x*/0,/*m*/0,/*t*/0,/*f*/0,/*ib*/0,/*p*/NULL,/*l*/NULL,
				/*fl*/NULL,/*le*/NULL,/*b*/NULL);
	FoamBox paramBox = fboxNew(foamNewEmptyDDecl(FOAM_DDecl_Local));
	FoamBox localBox = fboxNew(foamNewEmptyDDecl(FOAM_DDecl_Local));

	FoamBox seqBox = fboxNewEmpty(FOAM_Seq);

	fboxAdd(localBox, foamNewDecl(FOAM_Word, strCopy(""), 0));
	fboxAdd(localBox, foamNewDecl(FOAM_Word, strCopy(""), 0));

	fboxAdd(seqBox, foamNewSet(foamNewLoc(0), foamNewCast(FOAM_BInt, foamNewLoc(int0))));

	prog->foamProg.locals = fboxMake(localBox);
	prog->foamProg.params = fboxMake(paramBox);
	prog->foamProg.retType = FOAM_Word;
	prog->foamProg.format = emptyFormatSlot;
	prog->foamProg.levels = foamNewEmpty(FOAM_DEnv, 0);
	prog->foamProg.body = fboxMake(seqBox);

	unit->foamUnit.defs->foamDDef.argv[0] = foamNewDef(foamNewConst(int0),
							   prog);

	RetContext unitContext = rtcInit(unit);
	RetContext progContext = rtcNewProg(unitContext, prog, 2);
	
	rtcAddUse(progContext, foamNewLoc(int0));
	
	rtcSetType(progContext, foamNewLoc(int0), FOAM_BInt);

	rtcRearrangeProg(progContext);

	rtcFree(progContext);
	rtcFree(unitContext);

	testIntEqual("stmts", 1, foamArgc(prog->foamProg.body));
	testAIntEqual("type", FOAM_BInt, prog->foamProg.locals->foamDDecl.argv[0]->foamDecl.type);
	testAIntEqual("type", FOAM_Word, prog->foamProg.locals->foamDDecl.argv[1]->foamDecl.type);

	foamFree(prog);

	fini();
}
