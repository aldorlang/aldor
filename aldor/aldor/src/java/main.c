#include <stdlib.h>
#include "javacode.h"
#include "genjava.h"
#include "ostream.h"

int main(int argc, char *argv[])
{
	osInit();
	sposInit();
	comsgInit();
	dbInit();

	FILE *file  = fopen(argv[1], "r");
	Foam f      = foamRdSExpr(file, NULL, NULL);
	JavaCode jc = gjGenJavaUnit(f, "foo");
	foamFree(f);

	printf("SExpr...\n");
	jcoPrint(stdout, jc);
	printf("Java...\n");
	OStream o = ostreamNewFrStdout();
	JavaCodePContext ctxt = jcoPContextNew(o, true);
	jcoWrite(ctxt, jc);
	jcoPContextFree(ctxt);
	jcoFree(jc);

	return 0;
}

