#include "testall.h"
#include "testlib.h"
#include "buffer.h"
#include "forg.h"
#include "strops.h"

void forgTest()
{
	Buffer buf = bufNew();

	ForeignOrigin forg1 = forgNew(FOAM_Proto_Java, strCopy("hello"));
	ForeignOrigin forg2 = forgNew(FOAM_Proto_C, strCopy("world"));
	ForeignOrigin forg3 = forgNew(FOAM_Proto_C, NULL);
	ForeignOrigin forg1_r, forg2_r, forg3_r;
	testTrue("1", forgEqual(forg1, forg1));
	testTrue("2", forgEqual(forg2, forg2));
	testFalse("3", forgEqual(forg1, forg2));
	testFalse("4", forgEqual(forg1, forg3));

	forgToBuffer(buf, forg1);
	forgToBuffer(buf, forg2);
	bufSetPosition(buf, 0);

	forg1_r = forgFrBuffer(buf);
	forg2_r = forgFrBuffer(buf);

	testTrue("5", forgEqual(forg1_r, forg1));
	testTrue("6", forgEqual(forg2_r, forg2));

	int pos = bufPosition(buf);
	bufSetPosition(buf, 0);
	forgBufferSkip(buf);
	forgBufferSkip(buf);
	testIntEqual("7", pos, bufPosition(buf));

	bufSetPosition(buf, 0);
	forgToBuffer(buf, forg3);
	forgToBuffer(buf, forg1);
	pos = bufPosition(buf);

	bufSetPosition(buf, 0);
	forg3_r = forgFrBuffer(buf);
	forg1_r = forgFrBuffer(buf);

	testTrue("8", forgEqual(forg3_r, forg3));
	testTrue("8", forgEqual(forg1_r, forg1));
	testIntEqual("10", pos, bufPosition(buf));
}
