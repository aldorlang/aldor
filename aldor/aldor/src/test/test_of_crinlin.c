#include "axlobs.h"
#include "of_crinlin.h"
#include "optinfo.h"
#include "strops.h"
#include "testlib.h"

void testCrinInlineOne(void);

extern int crinDebug;

void
ofCrinTest()
{
	crinDebug = 1;
	init();
	TEST(testCrinInlineOne);
}

void
testCrinInlineOne()
{
	String progText = /* indicative */
		"(Seq "
		"  (Set (Loc 0) (GenIter (Gener 0 (Env 0) (Const 2 cr))))"
		"  (Label 0)"
		"  (GenerStep 1 (Loc 0))"
		"  (CCall (Par 0) (GenerValue (Loc 0)))"
		"  (Goto 0)"
		"  (Label 1)"
	        "  (CCall (Par 0) (Nil))"
		"  (Return))";
	String coroutineText = /* indicative */
		"(Seq "
		"  (Yield (SInt 22))"
		"  (Return))";

	Foam prog = foamNewProgEmpty();
	foamOptInfo(prog) =  optInfoNew(NULL, prog, NULL, false);
	prog->foamProg.nLabels = 2;
	prog->foamProg.params = foamNewDDecl(FOAM_DDecl_Param,
					     foamNewDecl(FOAM_Clos, strCopy("p0"), emptyFormatSlot),
					     NULL);
	prog->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
					     foamNewDecl(FOAM_Gener, strCopy("g"), emptyFormatSlot),
					     foamNewDecl(FOAM_GenIter, strCopy("i"), emptyFormatSlot),
					     NULL);
	prog->foamProg.levels = foamNew(FOAM_DEnv, 1, emptyFormatSlot);
	prog->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	prog->foamProg.body = // foamFrString(callerText)
		foamNewSeq(foamNewSet(foamNewLoc(1),
				      foamNewGenIter(foamNewGener(0, foamNewEnv(0), foamNewConst(1)))),
			   foamNewLabel(0),
			   foamNewGenerStep(1, foamNewLoc(1)),
			   foamNewCCall(FOAM_NOp, foamNewPar(0), foamNewGenerValue(foamNewLoc(1)), NULL),
			   foamNewGoto(0),
			   foamNewLabel(1),
			   foamNewReturn(foamNewNil()),
			   NULL);

	Foam coroutine = foamNewProgEmpty();
	coroutine->foamProg.nLabels = 0;
	coroutine->foamProg.params = foamNewEmptyDDecl(FOAM_DDecl_Param);
	coroutine->foamProg.locals = foamNewDDecl(FOAM_DDecl_Local,
					     NULL);
	coroutine->foamProg.levels = foamNew(FOAM_DEnv, 2, emptyFormatSlot, emptyFormatSlot);
	coroutine->foamProg.fluids = foamNew(FOAM_DFluid, 0);
	coroutine->foamProg.body = //foamFrCString(coroutineText);
		foamNewSeq(foamNewYield(foamNewSInt(22)),
			   foamNewReturn(foamNewNil()),
			   NULL);
	
	Foam unit = foamNew(FOAM_Unit, 2,
			    foamNewDFmt(foamNewDDecl(FOAM_DDecl_Global, NULL),
					foamNewDDecl(FOAM_DDecl_Consts,
						     foamNewDecl(FOAM_Prog, strCopy("caller"),
								 emptyFormatSlot),
						     foamNewDecl(FOAM_Prog, strCopy("callee"),
								 emptyFormatSlot),
						     NULL),
					foamNewDDecl(FOAM_DDecl_LocalEnv, NULL),
					foamNewDDecl(FOAM_DDecl_Fluid, NULL),
					foamNewDDecl(FOAM_DDecl_LocalEnv, NULL),
					NULL),
			    foamNew(FOAM_DDef, 2,
				    foamNewDef(foamNewConst(0), prog),
				    foamNewDef(foamNewConst(1), coroutine)));
	crinUnit(unit);

	foamPrintDb(unit);
}
