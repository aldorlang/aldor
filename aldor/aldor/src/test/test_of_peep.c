#include "axlobs.h"
#include "cmdline.h"
#include "flog.h"
#include "foam.h"
#include "of_inlin.h"
#include "of_peep.h"
#include "optfoam.h"
#include "strops.h"
#include "syme.h"
#include "testlib.h"

local void testAElt();

void ofPeepTest()
{
	TEST(testAElt);
}

void testAElt()
{
	Foam expr, body, prog, locals;

	expr = foamNewDef(foamNewLoc(0),
			  foamNewAElt(FOAM_Char, foamNewSInt(0), foamNew(FOAM_Arr, 2, FOAM_Char, 46)));
	body = foamNewSeq(expr, NULL);

	prog = foamNewProgEmpty();
	locals = foamNewDDecl(FOAM_DDecl_Local,
			      foamNewDecl(FOAM_Char, strCopy("0"), emptyFormatSlot), NULL);

	prog->foamProg.locals = locals;
	prog->foamProg.body = body;
	prog->foamProg.params = foamNewEmptyDDecl(int0);
	prog->foamProg.fluids = foamNewEmptyDDecl(int0);
	prog->foamProg.nLabels = 2;
	prog->foamProg.levels = foamNewEmptyDEnv();
	foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);
	
	peepProg(prog, false);

	testTrue("eq", foamEqual(foamNewDef(foamNewLoc(int0), foamNewChar(46)), prog->foamProg.body->foamSeq.argv[0]));
}
