#include "axlobs.h"
#include "foam.h"
#include "testlib.h"
#include "syme.h"
#include "sexpr.h"

local void testCall();
local void testDDecl();
local void testConstructors();

void
foamTest()
{
	sxiInit();

	TEST(testCall);
	TEST(testDDecl);
	TEST(testConstructors);
}

local void
testCall()
{
	Foam foam;

	foam = foamNew(FOAM_OCall, 3, FOAM_Clos, foamNewNil(), foamNewNil());
	testIntEqual("foamOCallArgc", 0, foamOCallArgc(foam));

	foam = foamNew(FOAM_OCall, 4, FOAM_Clos, foamNewNil(), foamNewNil(), foamNewSInt(1));
	testIntEqual("foamOCallArgc", 1, foamOCallArgc(foam));

	foam = foamNewPCall(FOAM_Proto_C, FOAM_NOp, foamNewGlo(int0), NULL);
	testIntEqual("argc", 0, foamPCallArgc(foam));
	testIntEqual("protocol", FOAM_Proto_C, foam->foamPCall.protocol);
}

local void
testDDecl()
{
	Foam ddecl = foamNewDDecl(FOAM_DDecl_Local,
				  foamNewDecl(FOAM_SInt, strCopy("fred"), emptyFormatSlot),
				  NULL);
	testIntEqual("tag", FOAM_DDecl, foamTag(ddecl));
	testIntEqual("argc", 1, foamDDeclArgc(ddecl));
}

local void
testConstructors()
{
	Foam foam;
	foam = foamNewBCall0(FOAM_BVal_BoolNot);
	testIntEqual("argc", foamBCallSlotc, foamArgc(foam));
	testIntEqual("tag", FOAM_BVal_BoolNot, foam->foamBCall.op);

	Foam arg1 = foamNewLoc(int0);
	Foam arg2 = foamNewLoc(int0);
	foam = foamNewBCall1(FOAM_BVal_BoolNot, arg1);
	testIntEqual("argc", 1, foamBCallArgc(foam));
	testIntEqual("tag", FOAM_BVal_BoolNot, foam->foamBCall.op);
	testPointerEqual("arg1", arg1, foam->foamBCall.argv[0]);

	foam = foamNewBCall2(FOAM_BVal_BoolNot, arg1, arg2);
	testIntEqual("argc", 2, foamBCallArgc(foam));
	testIntEqual("tag", FOAM_BVal_BoolNot, foam->foamBCall.op);
	testPointerEqual("arg1", arg1, foam->foamBCall.argv[0]);
	testPointerEqual("arg1", arg2, foam->foamBCall.argv[1]);
}
