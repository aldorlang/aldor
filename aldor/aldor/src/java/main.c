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

	if (getenv("GJ_DEBUG")) {
		genJavaDebug=1;
	}
	argv++;
	while (true) {
		if (strcmp(argv[0], "-Jmain") == 0) {
			gjGenSetMain(true);
			argv++;
		}
		else
			break;

	}

	FILE *file  = fopen(argv[0], "r");
	Foam f      = foamRdSExpr(file, NULL, NULL);
	PathList pl = pathListFrString(argv[0]);
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

