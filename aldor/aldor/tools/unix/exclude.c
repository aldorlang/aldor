/*
 *
 * cc -E file.c | exclude [-I dir] > file.E
 *
 * This program replaces included lines with the original "#include" line.
 * SMW Jan/91
 *
 * Only files from the named directory are unincluded.
 * The default uninclude directory is "/usr/include". 
 * Specifying -I "" causes all files to be unincluded.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#if defined (__GNUC__) && defined (__WIN32__)
#  define MAXPATHLEN      (260 - 1 /*NUL*/)
#else
#  include <sys/param.h>
#endif

#define  streq(a,b)	(!strcmp(a,b))

char *cmdName;
char *excludeDir;

enum inclState {Include, Exclude};

main(argc, argv)
	int	argc;
	char	**argv;
{
	cmdName    = argv[0];
	excludeDir = "/usr/include";
	if (argc > 1) {
		if (streq("-I", argv[1]) && argv[2])
			excludeDir = argv[2];
		else 
			error("Usage: uninclude [-I dir] < infile >outfile");
	}
	
	doFile(stdout, stdin, "stdin", "no previous file", Include);
	exit(0);
}



#define	LINESIZE	100000

char *
getLine(inFile)
	FILE		*inFile;
{
	char		*s;
	static char 	line[LINESIZE];

	s = fgets(line, LINESIZE, inFile);

	if (!s) return 0;
	if (s[strlen(s)-1] == '\n') return s;

	/*!! Could realloc and read rest. */
	error("Line too long!!");
}

void
echoLine(outFile, line, cond)
	FILE		*outFile;
	char		*line;
	int		cond;
{
	if (cond) fprintf(outFile, "%s", line);
}

/*
 * Returns EOF as appropriate.
 */

int
doFile(outFile, inFile, inPath, prevPath, state)
	FILE		*outFile, *inFile;
	char		*inPath, *prevPath;
	enum inclState	state;
{
	char	xxPath[MAXPATHLEN];
	int	xxLine;

	char	*line;

	if (state == Include) {
		state = strprefix(excludeDir, inPath);
		if (state != Include)
			fprintf(outFile, "# include \"%s\"\n", inPath);
	}
	
	for (;;) {
		line = getLine(inFile);

		/* End of file */
		if (!line)
			break;

		/* Normal line */
		else if (!isLineDirective(line, xxPath, &xxLine))
			echoLine(outFile, line, state == Include);

		/* Line synchronization within file */
		else if (streq(xxPath, inPath)) 
			echoLine(outFile, line, state == Include);

		/* Start of new file */
		else if (xxLine == 1) {
			if (doFile(outFile,inFile,xxPath,inPath,state) == EOF)
				break;
		}

		/* End of this file */
		else if (streq(xxPath, prevPath))
			return ~ EOF;

		/* Not from # include nesting, e.g. from yacc. */
		else 
			echoLine(outFile, line, state == Include);
	}

	return EOF;
}

int
strprefix(pref, s)
	char	*pref, *s;
{
	int	preflen, slen;

	preflen = strlen(pref);
	slen    = strlen(s);

	return slen >= preflen && !strncmp(pref, s, preflen);
}

/*
 * This function tests whether line is of the form: # nnn "path"
 * If so, pPath and pLine are updated to contain the path and line number.
 */
int
isLineDirective(line, pPath, pLine)
	char	*line, *pPath;
	int	*pLine;
{
	int	n;

	if (*line != '#') return 0;
	line++;

	/*
	 * Accept: # 2...  #2...  # line 2...   #line 2...
	 */
	while (isspace(*line))
		line++;
	if (!strncmp("line", line, 4))
		line += 4;
	while (isspace(*line))
		line++;
	
	if (!isdigit(*line)) return 0;
	n = *line - '0';
	line++;

	while (isdigit(*line)) {
		n *= 10;
		n += *line-'0';
		line++;
	}

	while (isspace(*line))
		line++;

	if (*line != '"') return 0;
	line++;

	while (*line && *line != '"')
		*pPath++ = *line++;

	*pLine = n;
	*pPath = 0;
	return 1;
}

error(s)
	char	*s;
{
	fprintf(stderr, "%s: %s\n", cmdName, s);
	exit(3);
}
