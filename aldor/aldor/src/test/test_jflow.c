#include "axlobs.h"
#include "cmdline.h"
#include "foam.h"
#include "of_inlin.h"
#include "of_jflow.h"
#include "testlib.h"

local void testJFlow1();
local void testJFlow2();

local Foam fmTestSideEffectingStmt(FoamTag type, ...);
local Foam fmTestProgFrCode(Foam locals, Foam seq);
local int fmTestNLabels(Foam seq);

void
jflowTest()
{
	TEST(testJFlow1);
	TEST(testJFlow2);
}

extern int jflowCatDebug, jflowDfDebug, jflowDfiDebug, jflowGoDebug, jflowDmDebug;

local void
testJFlow1()
{
	Foam body;

	body = foamNewSeq(foamNewSet(foamNewLoc(int0), foamNewSInt(int0)),
			  foamNewLabel(int0),
			  foamNewIf(foamNewPCall(FOAM_Proto_C, FOAM_NOp, foamNewGlo(int0), NULL), 1),
			  foamNewLabel(1),
			  foamNewIf(foamNewLoc(int0), int0),
			  foamNewReturn(foamNewSInt(1)),
			  NULL
		);

	Foam locals = foamNewDDecl(FOAM_DDecl_Local,
				   foamNewDecl(FOAM_SInt, strCopy("control"), emptyFormatSlot), NULL);

	Foam prog = fmTestProgFrCode(locals, body);

	jflowProg(prog);

	foamPrintDb(prog);
	cmdDebugReset();
}

local void
testJFlow2()
{
	Foam body;

	jflowCatDebug = 1;
	jflowDfDebug = 1;
	jflowDfiDebug = 1;
	jflowGoDebug = 1;
	jflowDmDebug = 1;

	body = foamNewSeq(foamNewSet(foamNewLoc(int0), foamNewBool(true)),
			  foamNewLabel(1),
			  foamNewSet(foamNewLoc(1), foamNewBool(true)),
			  foamNewIf(foamNewBCall1(FOAM_BVal_BoolNot, foamNewLoc(int0)), 3),
			  foamNewIf(fmTestSideEffectingStmt(FOAM_Bool, NULL), 3),
			  foamNewLabel(4),
			  foamNewIf(foamNewBCall1(FOAM_BVal_BoolNot, foamNewLoc(1)), 2),
			  foamNewIf(fmTestSideEffectingStmt(FOAM_Bool, NULL), 5),
			  fmTestSideEffectingStmt(FOAM_Word, NULL),
			  foamNewLabel(6),
			  foamNewGoto(1),
			  foamNewLabel(5),
			  foamNewSet(foamNewLoc(int0), foamNewBool(false)),
			  foamNewGoto(6),
			  foamNewLabel(2),
			  foamNewIf(foamNewLoc(int0), 7),
			  fmTestSideEffectingStmt(FOAM_Word, NULL),
			  foamNewLabel(8),
			  foamNewReturn(fmTestSideEffectingStmt(FOAM_Word, NULL)),
			  foamNewLabel(7),
			  fmTestSideEffectingStmt(FOAM_Word, NULL),
			  foamNewGoto(8),
			  foamNewLabel(3),
			  foamNewSet(foamNewLoc(1), foamNewBool(false)),
			  foamNewGoto(4),
			  NULL
		);

	Foam locals = foamNewDDecl(FOAM_DDecl_Local,
				   foamNewDecl(FOAM_Bool, strCopy("good"), emptyFormatSlot),
				   foamNewDecl(FOAM_Bool, strCopy("control"), emptyFormatSlot),
				   NULL);


	Foam prog = fmTestProgFrCode(locals, body);
	foamPrintDb(prog);

	jflowProg(prog);

	foamPrintDb(prog);
	cmdDebugReset();
}

local Foam
fmTestProgFrCode(Foam locals, Foam body)
{
	Foam prog = foamNewProgEmpty();
	int nLabels = fmTestNLabels(body);

	prog->foamProg.locals = locals;
	prog->foamProg.body = body;
	prog->foamProg.params = foamNewEmptyDDecl(int0);
	prog->foamProg.fluids = foamNewEmptyDDecl(int0);
	prog->foamProg.nLabels = nLabels;
	prog->foamProg.levels = foamNewEmptyDEnv();
	foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);

	return prog;
}

local Foam
fmTestSideEffectingStmt(FoamTag type, ...)
{
	va_list argp;
	Foam foam;
	int i;

	va_start(argp, type);
	FoamList args = listListv(Foam)(argp);
	va_end(argp);

	foam = foamNewEmpty(FOAM_PCall, foamPCallSlotc + listLength(Foam)(args));
	foam->foamPCall.protocol = FOAM_Proto_C;
	foam->foamPCall.type = type;
	foam->foamPCall.op = foamNewGlo(int0);

	i=0;
	while (args != listNil(Foam)) {
		foam->foamPCall.argv[i++] = car(args);
		args = listFreeCons(Foam)(args);
	}

	return foam;
}

int
fmTestNLabels(Foam seq)
{
	int maxLabel = -1;
	int i=0;
	assert(foamTag(seq) == FOAM_Seq);

	for (i=0; i<foamArgc(seq); i++) {
		Foam stmt = seq->foamSeq.argv[i];
		if (foamTag(stmt) == FOAM_Label) {
			AInt lno = stmt->foamLabel.label;
			if (lno > maxLabel)
				maxLabel = lno;
		}
	}
	return maxLabel+1;
}

