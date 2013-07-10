/*
 * collide [n] <infile >outfile
 *
 * Selects input lines which do not differ from their neighbors within
 * the first n characters.  The default value for n is 7.
 *
 * E.g.  cids *.c | collide 12 > badnames
 *
 * SMW Jan/91
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE	1024

int	sig = 7;
char	prevWord[MAXLINE], currWord[MAXLINE];
int	prevPrinted,       currPrinted;

main(argc, argv)
	int	argc;
	char	**argv;
{
	if (argc > 1) sig = atoi(argv[1]);

	strcpy(currWord, " You won't ever see this !?! ");
	currPrinted = 0;

	for (;;) {
		strcpy(prevWord, currWord);
		prevPrinted = currPrinted;

		if (!fgets(currWord, MAXLINE, stdin)) break;
		currPrinted = 0;

		if (!strncmp(currWord, prevWord, sig)) {
			if (!prevPrinted)
				printf( "%s", prevWord);
			printf("%s", currWord);
			currPrinted = 1;
		}
	}
	exit(0);
}
