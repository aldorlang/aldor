#include "axlobs.h"
#include "foam.h"
#include "fptr.h"
#include "sexpr.h"
#include "testlib.h"

void fptrTest(void)
{
	Foam foam;
	PtrSet ptrSet;
	FoamPtr p0, p1;

	sxiInit();

	ptrSet = psetNew();
	testTrue("t1", psetIsEmpty(ptrSet));
	foam = foamNewSeq(foamNewSInt(100), foamNewSInt(101), foamNewSInt(102));

	p0 = fptrNew(ptrSet, foam, 0);
	p1 = fptrNew(ptrSet, foam, 1);

	testFalse("t2", psetIsEmpty(ptrSet));
	testIntEqual("t3", 100, fptrVal(p0)->foamSInt.SIntData);
	testIntEqual("t4", 101, fptrVal(p1)->foamSInt.SIntData);

	fptrSet(p1, foamNewSInt(201));
	testIntEqual("t5", 201, fptrVal(p1)->foamSInt.SIntData);

	fptrFree(p0);
	fptrFree(p1);

	testTrue("t6", psetIsEmpty(ptrSet));
}

