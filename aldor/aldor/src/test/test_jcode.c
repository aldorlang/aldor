#include "axlobs.h"
#include "buffer.h"
#include "debug.h"
#include "format.h"
#include "formatters.h"
#include "java/javacode.h"
#include "opsys.h"
#include "sexpr.h"
#include "testlib.h"

local void testTry();
local void testStr();
#define ID(name) jcId(strCopy(name))

void
jcodeTest()
{
	osInit();
	dbInit();
	fmttsInit();
	sxiInit();
	TEST(testTry);
	TEST(testStr);
	dbFini();
}

void
testTry()
{
	JavaCode code;
	Buffer buf = bufNew();
	char *txt;
	JavaCodePContext ctxt = jcoPContextNew(ostreamNewFrBuffer(buf), true);
	code = jcTry(jcBlock(jcStatement(jcAssign(ID("r"),
						  jcApplyMethodV(ID("obj"), ID("foo"), 0)))),
		     listSingleton(JavaCode)(jcCatch(jcLocalDecl(0, ID("Exn"), ID("e")),
						     jcBlock(jcStatement(jcReturnVoid())))),
		     0);

	afprintf(dbOut, "%pJavaCode\n", code);
	jcoWrite(ctxt, code);
	txt = bufLiberate(buf);
	afprintf(dbOut, "%s\n", txt);

	testStringEqual("", "try {\n    r = obj.foo();\n}\ncatch (Exn e) {\n    return;\n}", txt);
}

void
testStr()
{
	JavaCode c = jcLiteralChar("\\");
	testStringEqual("eq", "\\\\", c->literal.txt);
}
