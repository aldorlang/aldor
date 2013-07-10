/*
 * badzeros < infile > outfile
 * badzeros infile   > outfile
 *
 * This program detects the use of a naked 0 as an argument to a function
 * or macro.
 *
 * SMW July 93
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cscan.h"
/*extern char * malloc();*/

char	word[64000];


#define tkIs(tk,tg)	((tk) && (tk)->kind == (tg))
#define tkIsZero(tk)	((tk) && (tk)->kind == CTOK_IntegerConstant && \
			 !strcmp((tk)->string, "0"))

void
tkFree(tk)
	struct ctok	*tk;
{
	if (tk) free(tk);
}

struct ctok *
tkCopy(otk)
	struct ctok *	otk;
{
	struct ctok *	ntk;
	int		sz;

	sz  = sizeof(*otk) + strlen(otk->string) + strlen(otk->neutral) + 2;
	ntk = (struct ctok *) malloc(sz);
	if (ntk == 0) {
		fprintf(stderr, "Could not allocate %d bytes\n", sz);
		exit(1);
	}
	ntk->kind    = otk->kind;
	ntk->string  = (char *) ntk + sizeof(*ntk);
	ntk->neutral = (char *) ntk + sizeof(*ntk) + strlen(otk->string) + 1;

	strcpy(ntk->string,  otk->string);
	strcpy(ntk->neutral, otk->neutral);
	return ntk;
}


void
badzeros(inname, infile)
	char	*inname;
	FILE	*infile;
{
	struct ctok	*tk0, *tk1, *tk2;
	int		braceDepth;

	cscanInit(word, sizeof(word), inname, infile, 0);

	tk0 = tk1 = tk2 = 0;
	braceDepth = 0;

	do {
		tkFree(tk0);

		tk0 = tk1;
		tk1 = tk2;
		tk2 = tkCopy(cscan());

		/*
		 * Complain about 
		 *	(    0 [,)]
		 *	[(,] 0 )
		 *	,    0 ,	-- not in  {...}
		 */

		if (tkIs(tk0, CTOK_OCurly))
			braceDepth++;
		if (tkIs(tk0, CTOK_CCurly))
			if (braceDepth > 0) braceDepth--;
		if (tkIs(tk0, CTOK_OParen))
			braceDepth = 0;


		if (!tkIsZero(tk1)) continue;

		/* ( 0 [,)] */
		if (tkIs(tk0, CTOK_OParen)
		&&   (tkIs(tk2, CTOK_Comma) || tkIs(tk2, CTOK_CParen)) ) {
			fprintf(stdout,"\"%s\", line %d: Suspicious 0\n",
				cscanFileName, cscanLineNumber);
			continue;
		}

		/* [(,] 0 ) */
		if (tkIs(tk2, CTOK_CParen)
		&&   (tkIs(tk0, CTOK_Comma) || tkIs(tk2, CTOK_OParen)) ) {
			fprintf(stdout,"\"%s\", line %d: Suspicious 0\n",
				cscanFileName, cscanLineNumber);
			continue;
		}

		/* , 0 ,  -- not in {...} */
		if ( braceDepth == 0
		&&   tkIs(tk0, CTOK_Comma) && tkIs(tk2, CTOK_Comma) ) {
			fprintf(stdout,"\"%s\", line %d: Suspicious 0\n",
				cscanFileName, cscanLineNumber);
			continue;
		}
	} while (tk2 && tk2->kind != CTOK_EOF);
}

int
main(argc, argv)
	int		argc;
	char		**argv;
{
	if (argc < 2) 
		badzeros("stdin", stdin);
	else {
		int	i;
		for (i = 1; i < argc; i++) {
			char	*inname = argv[i];
			FILE	*infile = fopen(inname, "r");

			if (infile == 0) {
				fprintf(stderr,
					"Could not open file '%s'\n", inname);
				exit(1);
			}
			badzeros(inname, infile);
			fclose(infile);
		}
	}
	return 0;
}
