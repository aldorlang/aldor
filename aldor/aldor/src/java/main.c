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
	PathList pl = pathListFrString(argv[1]);
	String lastElt = car(listLastCons(String)(pl));
	lastElt[strlen(lastElt)-3] = '\0';
	JavaCode jc = gjGenJavaUnit(f, lastElt);
	foamFree(f);

	printf("/*...\n");
	jcoPrint(stdout, jc);
	printf("...*/\n");
	OStream o = ostreamNewFrStdout();
	JavaCodePContext ctxt = jcoPContextNew(o, true);
	jcoWrite(ctxt, jc);
	jcoPContextFree(ctxt);
	jcoFree(jc);

	return 0;
}

