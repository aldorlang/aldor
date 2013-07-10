/*
 * cwords < infile > outfile
 *
 * This program extracts the identifiers and keywords from a C program.
 * Each output line contains a single word.
 *
 * SMW Jan/91
 */

#include <stdio.h>
#include <stdlib.h>
#include "cscan.h"

char	word[10000];

main(argc, argv)
	int		argc;
	char		**argv;
{
	struct ctok	*tk;
	int		done = 0;

	cscanInit(word, sizeof(word), "stdin", stdin, 0);

	for ( ; !done; ) {
		tk = cscan();
		switch (tk->kind) {
		  case CTOK_EOF:
			done = 1;
			break;
		  case CTOK_Identifier:
		  case CTOK_TypedefName:
		  case CTOK_EnumerationConstant:
			fprintf(stdout, "%s\n", tk->string);
		}
	}
	exit(0);
}
