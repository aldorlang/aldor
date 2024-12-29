/****************************** aldoc2html.c ******************************

   Author: Manuel Bronstein
   Date Last Updated: 25 April 2001

   aldoc2html preprocesses aldoc tex sources for latex2html

   Flags recognized:
    -h      (help)
    -x      (input)   expand only the \input statements
    -o name (output)  use name as output file name
    -v      (verbose)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "flags.h"

#define MAXLINE		500
#define	INPUT		"\\input"
#define NINPUT		6
#define	CLASS		"\\documentclass"
#define NCLASS		14
#define	ASPAGE		"\\aspage"
#define	ALPAGE		"\\alpage"
#define NASPAGE		7
#define	ASEXP		"\\asexp"
#define	ALEXP		"\\alexp"
#define	NASEXP		6
#define	ASTARGET	"\\astarget"
#define	ALTARGET	"\\altarget"
#define	NASTARGET	9
#define	ALCONST		"\\alconstant"
#define	NALCONST	11
#define	SIGNATURE	"\\Signature"
#define	NSIGNATURE	10
#define	TTYOUT		"{ttyout}"
#define NTTYOUT		8
#define	EXPORTS		"{exports}"
#define	NEXPORTS	9
#define	ENDEXP		"\\end{exports}"
#define	NENDEXP		13
#define	THISTYPE	"\\thistype"
#define	NTHISTYPE	9
#define	NAME		"\\name"
#define NNAME		5
#define	THIS		"\\this"
#define	NTHIS		5
#define	SHORTTHIS	"\\shortthis"
#define	NSHORTTHIS	10
#define	TEXEXT		".tex"
#define	EXTCHAR		'.'

#define	HELPFLAG	'h'
#define	OUTFLAG		'o'
#define	VERBFLAG	'v'
#define	INPUTFLAG	'x'

static char macroname[MAXLINE], macrothis[MAXLINE], macroshortthis[MAXLINE];
static char macroasexp[MAXLINE], macrosig[MAXLINE], macroastarget[MAXLINE];
static char macroexp[MAXLINE], macroconst[MAXLINE];
static char verbatim[] = "{verbatim}";

static void banner()
{
	fputs("TeX expander 1.1 - (c) Manuel Bronstein 2000,2001\n", stderr);
}

static int help()
{
	banner();
	fputs("Usage: aldoc2html [-opts] file[.tex]\n\n", stderr);
	fputs("  -h       show this help\n", stderr);
	fputs("  -v       verbose (show progress)\n", stderr);
	fputs("  -x       expand only the \\input statements\n", stderr);
	fputs("  -o name  use name for the output tex file\n", stderr);
	return 0;
}

static int macrocheck(char *s, char *macro, int len)
{
	return((!strncmp(s, macro, len)) && (!isalnum(s[len])));
}

/* Writes the macro expansion of s to fp */
static void macroexpand(char *s, FILE *fp)
{
	char t[MAXLINE], *macro;
	int i = 0;
	while (*s) {
		if (macrocheck(s, NAME, NNAME)) {
			macro = &(macroname[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NNAME]);
		}
		else if (macrocheck(s, THIS, NTHIS)) {
			macro = &(macrothis[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NTHIS]);
		}
		else if (macrocheck(s, SHORTTHIS, NSHORTTHIS)) {
			macro = &(macroshortthis[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NSHORTTHIS]);
		}
		else if (macrocheck(s, ASEXP, NASEXP)
			|| macrocheck(s, ALEXP, NASEXP)) {
			macro = &(macroasexp[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NASEXP]);
		}
		else if (macrocheck(s, ALCONST, NALCONST)) {
			macro = &(macroconst[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NALCONST]);
		}
		else if (macrocheck(s, SIGNATURE, NSIGNATURE)) {
			macro = &(macrosig[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NSIGNATURE]);
		}
		else if (macrocheck(s, ASTARGET, NASTARGET)
			|| macrocheck(s, ALTARGET, NASTARGET)) {
			macro = &(macroastarget[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NASTARGET]);
		}
		else if (!strncmp(s, EXPORTS, NEXPORTS)) { /* has {} in macro */
			macro = &(macroexp[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NEXPORTS]);
		}
		else if (!strncmp(s, TTYOUT, NTTYOUT)) {  /* has {} in macro */
			macro = &(verbatim[0]);
			while (*macro) t[i++] = *macro++;
			s = &(s[NTTYOUT]);
		}
		else t[i++] = *s++;
	}
	t[i] = 0;
	fputs(t, fp);
}


/* stops the string s at the first occurence of c */
static void killChar(char *s, char c)
{
	while (*s && (*s != c)) s++;
	if (*s == c) *s = '\0';
}

/* stops the string s at the last occurence of c */
static void killLastChar(char *s, char c)
{
	char *pos;
	pos = strrchr(s, c);
	if (pos != NULL) *pos = '\0';
}

/* returns 1 if s has an extension, 0 otherwise */
static int hasExtension(char *s)
{
	while (*s && (*s != EXTCHAR)) s++;
	return(*s == EXTCHAR);
}

static char *getpath(char *path, char *s, char *home)
{
	if (path != NULL) {
		if (*path == '\0') return NULL;
		while ((*path != '\0') && (*path != ':')) {
			if ((*path == '~') && (home != NULL)) {
				while (*home != '\0') *s++ = *home++;
				path++;
			}
			else *s++ = *path++;
		}
		*s = '\0';
		if (*path == ':') path++;
	}
	return path;
}

/* Looks in . and HOME and TEXINPUTS for the file name */
static FILE *envopen(char *name)
{
	FILE *fp;
	char s[MAXLINE], *path, *home;
	while (isspace(*name)) name++;
	killChar(name, ' ');
	fp = fopen(name, "r");
	if (fp != NULL) return fp;
	path = getenv("TEXINPUTS");
	home = getenv("HOME");
	while ((path = getpath(path, s, home)) != NULL) {
		strcat(s, "/");
		strcat(s, name);
		fp = fopen(s, "r");
		if (fp != NULL) return fp;
	}
	return NULL;
}

/* returns 0 if no arg, -1 if arg is between [], +1 if arg is between {} */
static int getarg(char *name, char *s)
{
	int ans;
	while ((*s != '\0') && (*s != '{') && (*s != '[')) s++;
	if (*s == '\0')		{ ans =  0; strcpy(name, ""); }
	else if (*s == '{')	{ ans =  1; strcpy(name, ++s); }
	else			{ ans = -1; strcpy(name, ++s); }
	return ans;
}

/* Forward declation */
static int doexpand(FILE *, FILE *, int, int);

/* returns 0 if file found and inlined, <> 0 otherwise */
static int doinput(char *name, FILE *fpout, int verbose, int exmacro)
{
	int err = 0;
	FILE *fpin = envopen(name);
	if (verbose) fprintf(stderr, "Looking for %s...\t", name);
	if (fpin != NULL) {
		if (verbose) fprintf(stderr, "...opened\n");
		err = doexpand(fpin, fpout, verbose, exmacro);
		if (verbose) {
			if (err) fputc('\n', stderr);
			else fprintf(stderr, "%s completed\n", name);
		}
	}
	else if (verbose) fprintf(stderr, "...not found!\n");
	return err;
}

static int doexpand0(FILE *fpout, char *s, int verbose, int exmacro)
{
	char name[MAXLINE-50], name2[MAXLINE-50];
	/*FILE *fpin = NULL;*//*unused*/
	int i, err = 0, shortthis;
	if (macrocheck(s, INPUT, NINPUT)) {	/* \input something */
		if (getarg(name, &(s[NINPUT])) == 0) strcpy(name, &(s[NINPUT]));
		killChar(name, '\n');
		killChar(name, '}');
		if (!hasExtension(name)) strcat(name, TEXEXT);
		if (doinput(name, fpout, verbose, exmacro)) fputs(s, fpout);
	}
	else if (!strncmp(s, CLASS, NCLASS)) {	/* \documentclass something */
		fputs("\\documentclass{aldoc}\n\n", fpout);
		if (doinput("aldoc2html.tex", fpout, 0, 0))
			fputs("\\input aldoc2html\n\n", fpout);
	}
	else {
		if (exmacro) {
			if (macrocheck(s, ASPAGE, NASPAGE)
				|| macrocheck(s,ALPAGE,NASPAGE)) {/* \aspage */
				macroexpand(s, fpout);
				getarg(name, &(s[NASPAGE]));
				killLastChar(name, '}');
				strcpy(macroname, name);
				sprintf(macrosig, "\\nsignature{%s}", name);
				sprintf(macroconst, "\\alsignature{%s}", name);
				if (verbose)
					fprintf(stderr,"\\name --> %s\n", name);
				fprintf(fpout, "\\alhtmltarget{%s}{%s}\n",
					macrothis, name);
			}
			else if (!strncmp(s,THISTYPE,NTHISTYPE)) {/*\thistype*/
				strcpy(macroname, "");
				strcpy(macroexp, "{exports}");
				shortthis = (getarg(name, &(s[NTHISTYPE])) < 0);
				if (shortthis) {
					strcpy(macroshortthis, name);
					killChar(macroshortthis, ']');
					getarg(name2, name);
					/* change \thistype[foo]{bar}
					       to \thistypE{foo}{bar} */
					s[NTHISTYPE-1] = 'E';
					i = NTHISTYPE;
					while (s[i] && (s[i] != '[')) i++;
					if (s[i] == '[') {
						s[i++] = '{';
						while (s[i] && (s[i]!=']')) i++;
						if (s[i] == ']') s[i] = '}';
					}
				}
				else strcpy(name2, name);
				macroexpand(s, fpout);
				killLastChar(name2, '}');
				strcpy(macrothis, name2);
				if (!shortthis) strcpy(macroshortthis, name2);
				snprintf(macroasexp, MAXLINE, "\\alfunc{%s}", name2);
				snprintf(macroastarget, MAXLINE,
						"\\alhtmltarget{%s}", name2);
				if (verbose) {
					fprintf(stderr, "\\shortthis --> %s\n",
								macroshortthis);
					fprintf(stderr, "\\this --> %s\n",
								macrothis);
					fprintf(stderr, "\\name --> %s\n",
								macroname);
				}
			}	/* end of \thistype */
			else if (!strncmp(s,ENDEXP,NENDEXP)) { /*\end{exports}*/
				macroexpand(s, fpout);
				strcpy(macroexp, "{exports0}");
			}
			else macroexpand(s, fpout);
		}	/* end of if (exmacro) */
		else fputs(s, fpout);
	}
	return err;
}

static int doexpand(FILE *fpin, FILE *fpout, int verbose, int exmacro)
{
	char s[MAXLINE];
	int err = 0;
	while ((!err) && (fgets(s, MAXLINE, fpin) != NULL))
		err = doexpand0(fpout, s, verbose, exmacro);
	fclose(fpin);
	return err;
}

#if 0
/*unused*/
static void killExtension(char *s)
{
	killLastChar(s, EXTCHAR);
}
#endif

/* returns 0 if ok, > 0 if error */
static int getFiles(int argc, char **argv, FILE **fpin, FILE **fpout)
{
	int out, err = 0;
	char *name, s[MAXLINE], t[MAXLINE];

	name = getFlagName(argc, argv, OUTFLAG);
	out = name != NULL;
	strcpy(t, out ? name : "stdout");
	name = (char *) argv[argc - 1];
	if ((argc == 0) || (*name == '-') || (out && !strcmp(t, name))) {
		fputs("aldoc2html: no file name given\n", stderr);
		err = 1;
	}
	else {
		strcpy(s, name);
		if (!hasExtension(s)) strcat(s, TEXEXT);
		*fpin = fopen(s, "r");
		if (*fpin == NULL) {
			fputs("aldoc2html: cannot open ", stderr);
			fputs(s, stderr);
			fputs("\n", stderr);
			err = 2;
		}
		if (!err) {
			*fpout = out ? fopen(t, "w") : stdout;
			if (*fpout == NULL) {
				fclose(*fpin);
				fputs("aldoc2html: cannot create ", stderr);
				fputs(t, stderr);
				fputs("\n", stderr);
				err = 3;
			}
		}
	}
	return err;
}

/* returns 0 if ok, error code otherwise */
static int expand(int argc, char **argv)
{
	int verbose, exmacro, err;
	FILE *fpin, *fpout;

	exmacro = !existFlag(argc, argv, INPUTFLAG);
	verbose = existFlag(argc, argv, VERBFLAG);
	if (verbose) banner();
	err = getFiles(argc, argv, &fpin, &fpout);
	if (err) return err;
	strcpy(macroname, "");
	strcpy(macrothis, "");
	strcpy(macroshortthis, "");
	strcpy(macroexp, "{exports}");
	strcpy(macroasexp, "\\alfunc{}");
	strcpy(macrosig, "\\nsignature{}");
	strcpy(macroconst, "\\alsignature{}");
	strcpy(macroastarget, "\\alhtmltarget{}");
	err = doexpand(fpin, fpout, verbose, exmacro);
	if (verbose) fputc('\n', stderr);
	fclose(fpout);
	return err;
}

int main(int argc, char **argv)
{
	exit(existFlag(argc, argv, HELPFLAG) ? help() : expand(argc, argv));
}
