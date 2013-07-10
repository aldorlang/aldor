/*
 * shorten [fmt] < infile >outfile
 *
 * Produce a file of #defines for the names in the input stream.
 * Stdin contains a list of identifiers, one per line.
 * If no format is given, then "_Ug_%d" is used.
 *
 * E.g.  shorten "T_%d" < scomp.ids > ugly.h
 *
 * SMW Jan/91
 */

#include <stdio.h>
#include <stdlib.h>

#define	MAXLINE	1024
#define MAXFMT	200

char	*idfmt = "_Ug_%d", allfmt[MAXFMT];
char	line[MAXLINE];

char *fgetsNoNL(line, size, fin)
    char *line;
    unsigned size;
    FILE *fin;
{
    char *s, *s0;

    s0 = s = fgets(line, MAXLINE, stdin);
    if (!s0) return s0;

    for ( ; *s ; s++) if (*s == '\n') { *s = 0 ; break; }
    return s0;
}

main(argc, argv)
	int	argc;
	char	**argv;
{
	int	i;

	if (argc > 1) idfmt = argv[1];

	sprintf(allfmt, "#define %%s \t%s\n", idfmt);

	for (i = 1; fgetsNoNL(line, MAXLINE, stdin); i++)
		printf(allfmt, line, i);
	exit(0);
}
