#include "axlobs.h"
#include "cmdline.h"
#include "flog.h"
#include "foam.h"
#include "of_inlin.h"
#include "testlib.h"
#include "syme.h"

local void testBBCopy();

void
flogTest()
{
	TEST(testBBCopy);
}

local void testBBCopy()
{
	Foam body, bodyCopy;
	
	body = foamNewSeq(foamNewSet(foamNewLoc(int0), foamNewSInt(int0)), foamNewReturn(foamNewEmptyValues()), NULL);
	bodyCopy = foamCopy(body);

	testTrue("copy works ok!", foamEqual(body, bodyCopy));

	Foam prog = foamNewProgEmpty();
	Foam locals = foamNewDDecl(FOAM_DDecl_Local, 
				   foamNewDecl(FOAM_SInt, strCopy("control"), emptyFormatSlot), NULL);

	prog->foamProg.locals = locals;
	prog->foamProg.body = body;
	prog->foamProg.params = foamNewEmptyDDecl(int0);
	prog->foamProg.fluids = foamNewEmptyDDecl(int0);
	prog->foamProg.nLabels = 2;
	prog->foamProg.levels = foamNewEmptyDEnv();
	foamOptInfo(prog) = inlInfoNew(NULL, prog, NULL, false);

	FlowGraph flog;
	
	flog = flogFrProg(prog, FLOG_MultipleExits);

	testTrue("Blocks", flogBlockC(flog) > 0);
	BBlock entry = flog->block0;
	testIntEqual("exits", 0, bbExitC(entry));
	testTrue("code preserved..", foamEqual(bodyCopy, entry->code));
	
	cmdDebugReset();
}

