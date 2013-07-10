/****************************** extract.c ******************************

   Author: Manuel Bronstein
   Date Last Updated: 7 December 2000

   extract simply extracts from any file all the lines contained between
   #if MARKER and #endif, where MARKER is the specified marker.

   Flags recognized:
    -h      (help)
    -m name (marker)  use name as start marker (default AUTODOC)
    -o name (output)  use name as output file name
    -r      (reverse) removes all the lines between the markers
    -t      (test)    make a stand-alone LaTeX file for testing, by adding
                      the appropriate preamble and conclusion
    -v      (verbose)
*/

#include <stdio.h>
#include <string.h>
#include "flags.h"
#include <stdlib.h>

#define MAXLINE 500
#define	STARTDOC "#if "
#define	ENDDOC   "#endif"
#define	AUTODOC  "AUTODOC"
#define	ASEXT    ".as"
#define	TEXEXT   ".tex"
#define	EXTCHAR	 '.'

#define	HELPFLAG 'h'
#define	MARKFLAG 'm'
#define	OUTFLAG	 'o'
#define	REVFLAG	 'r'
#define	TESTFLAG 't'
#define	VERBFLAG 'v'

static void banner()
{
	fputs("Documentation extractor 1.2 - (c) Manuel Bronstein 1995-2001\n",
		stderr);
}

static int help()
{
	banner();
	fputs("Usage: extract [-opts] file[.as]\n\n", stderr);
	fputs("  -h      show this help\n", stderr);
	fputs("  -v      verbose (show progress)\n", stderr);
	fputs("  -t      test (produce stand-alone latex file)\n", stderr);
	fputs("  -o name  use name for the output tex file\n", stderr);
	fputs("  -m name  use name for the marker to select\n", stderr);
	fputs("  -r      reverse (keep the code lines, strip the doc)\n\n",
									stderr);
	return 0;
}

/* Returns the new value of doc */
static int stripdoc(FILE *fp, char *s, char *start, int doc, int non, int noff,
			int verbose, int reverse, int *page)
{
	if (doc) {
		if (!strncmp(s, ENDDOC, noff)) {
			if (verbose) fputc(']', stderr);
			doc = 0;
		}
		else if (!reverse) fputs(s, fp);
	}
	else {
		if (!strncmp(s, start, non)) {
			if (verbose) fprintf(stderr, "[%d", ++*page);
			++doc;
		}
		else if (reverse) fputs(s, fp);
	}
	return doc;
}

/* removes an extension if any */
static void killExtension(char *s)
{
	while (*s && (*s != EXTCHAR)) s++;
	if (*s == EXTCHAR) *s = '\0';
}

/* returns 1 if s has an extension, 0 otherwise */
static int hasExtension(char *s)
{
	while (*s && (*s != EXTCHAR)) s++;
	return(*s == EXTCHAR);
}

/* returns 0 if ok, > 0 if error */
static int getFiles(int argc, char **argv, FILE **fpin, FILE **fpout, int strip)
{
	int out, err = 0;
	char *name, s[MAXLINE], t[MAXLINE];

	name = getFlagName(argc, argv, OUTFLAG);
	out = name != NULL;
	if (out) strcpy(t, name);
	name = (char *) argv[argc - 1];
	if ((argc == 0) || (*name == '-') || (out && !strcmp(t, name))) {
		fputs("extract: no file name given\n", stderr);
		err = 1;
	}
	else {
		strcpy(s, name);
		if (!hasExtension(s)) strcat(s, ASEXT);
		if (!out) {
			strcpy(t, s);
			killExtension(t);
			strcat(t, TEXEXT);
		}
		*fpin = fopen(s, "r");
		if (*fpin == NULL) {
			fputs("extract: cannot open ", stderr);
			fputs(s, stderr);
			fputs("\n", stderr);
			err = 2;
		}
		else {
			*fpout = (strip && !out) ? stdout : fopen(t, "w");
			if (*fpout == NULL) {
				fclose(*fpin);
				fputs("extract: cannot create ", stderr);
				fputs(t, stderr);
				fputs("\n", stderr);
				err = 3;
			}
		}
	}
	return err;
}

static void preamble(FILE *fp)
{
	fputs("\\documentclass[12pt,hyperref]{aldoc}\n", fp);
	fputs("\\pagestyle{fancyplain}\n", fp);
	fputs("\\begin{document}\n", fp);
}

static void closing(FILE *fp)
{
	fputs("\\end{document}\n", fp);
}

/* returns 0 if ok, error code otherwise */
static int extract(int argc, char **argv)
{
	int strip, verbose, test, err, non, noff, doc = 0, page = 0;
	char *marker, s[MAXLINE], start[MAXLINE];
	FILE *fpin, *fpout;
	/* unused variables
	   int cont = 1;
	   char *t[MAXLINE];
	*/

	verbose = existFlag(argc, argv, VERBFLAG);
	if (verbose) banner();
	err = getFiles(argc, argv, &fpin, &fpout,
			strip = existFlag(argc, argv, REVFLAG));
	if (err) return err;
	strcpy(start, STARTDOC);
	marker = getFlagName(argc, argv, MARKFLAG);
	strcat(start, marker == NULL ? AUTODOC : marker);
	non = strlen(start);
	noff = strlen(ENDDOC);
	test = (!strip) && existFlag(argc, argv, TESTFLAG);
	if (test) preamble(fpout);
	while (fgets(s, MAXLINE, fpin) != NULL)
		doc = stripdoc(fpout,s,start,doc,non,noff,verbose,strip,&page);
	if (test) closing(fpout);
	if (verbose) fputc('\n', stderr);
	fclose(fpin);
	fclose(fpout);
	return 0;
}

int main(int argc, char **argv)
{
	exit(existFlag(argc, argv, HELPFLAG) ? help() : extract(argc, argv));
}
