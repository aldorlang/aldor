#include "axlobs.h"
#include "foam.h"
#include "testlib.h"

void
foamTest()
{
	sxiInit();

	Foam foam;

	foam = foamNew(FOAM_OCall, 3, FOAM_Clos, foamNewNil(), foamNewNil());
	testIntEqual("foamOCallArgc", 0, foamOCallArgc(foam));

	foam = foamNew(FOAM_OCall, 4, FOAM_Clos, foamNewNil(), foamNewNil(), foamNewSInt(1));
	testIntEqual("foamOCallArgc", 1, foamOCallArgc(foam));
}
