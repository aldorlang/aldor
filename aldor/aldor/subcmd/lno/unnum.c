/*
 * unnum infile.c outfile.c logfile.lno
 *
 * This program copies infile.c to outfile.c, moving the line number 
 * information into the file logfile.lno.
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define	MaxPathLen	2000

int	doFile();
char	*getLine();
int	isLineDirective();
FILE	*fopenOrElse();
char	*mallocOrElse();

int	logFile();
int	logLine();
int	outLine();


int
main(argc, argv)
	int	argc;
	char	**argv;
{
	if (argc != 4) {
		fprintf(stderr,"Usage: unnum infile.c outfile.c logfile.lno\n");
		exit(1);
	}
	doFile(argv[1], argv[2], argv[3]);
}


/*
 * Copy infile to outfile, stripping line number directives
 * and recording line number mapping in logfile.
 *
 * #line directives changing file names are kept.
 */
int
doFile(infname, outfname, logfname)
	char	*infname, *outfname, *logfname;
{
	FILE	*infile,  *outfile,  *logfile;
	char	*text;

	char	logicalFile[MaxPathLen], lastFile[MaxPathLen];
	int	logicalLine, actualLine;
	int	isAnyLogged = 0;

	infile  = fopenOrElse(infname,  "r");
	outfile = fopenOrElse(outfname, "w");
	logfile = fopenOrElse(logfname, "w");

	actualLine  = 1;
	logicalLine = 1;
	strcpy(logicalFile, infname);
	strcpy(lastFile,    infname);


	while (!feof(infile)) {
		text = getLine(infile);
		if (isLineDirective(text, logicalFile, &logicalLine)) {
			if (!isAnyLogged || strcmp(logicalFile, lastFile)) {
				strcpy (lastFile, logicalFile);
				logFile(logfile, logicalFile);
				outLine(outfile, actualLine, logicalFile);
				isAnyLogged = 1;
			}
			logLine(logfile, actualLine, logicalLine);
		}
		else {
			if (!isAnyLogged) {
				logFile(logfile, logicalFile);
				outLine(outfile, actualLine, logicalFile);
				logLine(logfile, actualLine, logicalLine);
				isAnyLogged = 1;
			}
			fputs(text, outfile);
			actualLine++;
			logicalLine++;
		}
	}

	fclose(infile);
	fclose(outfile);
	fclose(logfile);

	return 0;
}

/*
 * :: Functions which generated new text in files.
 */
int
logFile(outf, fname)
	FILE	*outf;
	char	*fname;
{
	return fprintf(outf, "\"%s\"\n", fname);
}

int
logLine(outf, newNo, oldNo)
	FILE	*outf;
	int	newNo, oldNo;
{
	return fprintf(outf, "%d %d\n", newNo, oldNo);
}

int
outLine(outf, newNo, oldFile)
	FILE	*outf;
	int	newNo;
	char	*oldFile;
{
	return	fprintf(outf, "#line %d \"%s\"\n", newNo, oldFile);
}

/*
 * :: getLine -- Get a whole line, no matter how long it is.
 */
#define BUFINC	20

char *
getLine(infile)
	FILE	*infile;
{
	static char 	*buf   = 0;
	static unsigned buflen = 0;

	char	*s;
	int	slen;

	if (!buf) { buflen += BUFINC; buf = mallocOrElse(buflen); }

	s    = buf;
	slen = buflen;

	s[0] = 0;
	for (;;) {
		char	*t;
		int	n;

		fgets(s, slen, infile);
		if (feof(infile)) break;

		n = strlen(s);
		if (n != slen-1 || s[n-1] == '\n') break;

		t       = buf;
		buflen += BUFINC;
		buf     = mallocOrElse(buflen);
		strcpy(buf, t);
		free(t);

		s       = buf + buflen - BUFINC - 1;
		slen    = BUFINC + 1;

	}
	return buf;
}

/*
 * :: isLineDirective
 *
 * Test whether `line' is a line number C preprocessor directive.
 * If so, `pPath' and `pLine' are updated to contain the path and line number.
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
	*pLine = n;

	while (isspace(*line))
		line++;

	if (*line == '\n' || *line == 0) return 1;

	if (*line != '"') return 0;
	line++;

	while (*line && *line != '"')
		*pPath++ = *line++;

	*pPath = 0;
	return 1;
}

/*
 * :: Error handling versions of stdc functions.
 */
FILE *
fopenOrElse(fname, mode)
	char	*fname;
	char	*mode;
{
	FILE	*f;

	f = fopen(fname, mode);
	if (!f) {
		fprintf(stderr, "Cannot open file \"%s\" with mode \"%s\".\n",
			fname, mode);
		exit(1);
	}
	return f;
}

char *
mallocOrElse(unsigned n)
{
	char *p;
	p = (char *) malloc(n);
	if (!p) {
		fprintf(stderr, "Cannot allocate %u bytes.\n", n);
		exit(1);
	}
	return p;
}
