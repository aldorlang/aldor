#include <stdlib.h>
#include "javacode.h"
#include "genjava.h"
#include "ostream.h"

local void generate(String name);

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

	generate(argv[0]);

	return 0;
}

local void
generate(String name)
{
	FILE *file  = fopen(name, "r");
	Foam f      = foamRdSExpr(file, NULL, NULL);
	PathList pl = pathListFrString(name);
	String lastElt = car(listLastCons(String)(pl));
	JavaCode jc;
	OStream o;
	JavaCodePContext ctxt;

	lastElt[strlen(lastElt)-3] = '\0';
	jc = gjGenJavaUnit(f, lastElt);
	foamFree(f);

	printf("/*...\n");
	jcoPrint(stdout, jc);
	printf("...*/\n");
	o = ostreamNewFrFile(stdout);
	ctxt = jcoPContextNew(o, true);
	jcoWrite(ctxt, jc);
	jcoPContextFree(ctxt);
	jcoFree(jc);
}

