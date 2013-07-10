/*
 * renum infile.o outfile.o logfile.lno
 *
 * Renumber a coff file, using the line number information from "unnum".
 */

/****************************************************************************
 ****************************************************************************
 ***
 ***  WARNING:  WORK IN PROGRESS
 ***
 ****************************************************************************
 ****************************************************************************/
 
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void	doFile();
void	copyFile();
void	fileVitalStats();
void	readLog();

FILE	*fopenOrElse();
char	*mallocOrElse();

struct renum {
	int	fakeno, realno;
	char	*filename;
};
	
int
main(argc, argv)
	int	argc;
	char	**argv;
{
	if (argc != 4) {
		fprintf(stderr,"Usage: renum infile.o outfile.o logfile.lno\n");
		exit(1);
	}
	doFile(argv[1], argv[2], argv[3]);
}


struct renum	*renumInfov;
int		renumInfoc;

void
doFile(infname, outfname, logfname)
	char	*infname, *outfname, *logfname;
{
	FILE	*outfile;

	copyFile(infname, outfname);
	readLog (logfname, &renumInfov, &renumInfoc);

	outfile = fopenOrElse(outfname, "rb");

	fclose(outfile);
}

void
readLog(logfname, pinfov, pinfoc)
	char		* logfname;
	struct renum	**pinfov;
	int		* pinfoc;
{
	int	nlines, lmax;
	char	*lbuf;
	FILE	*fin;
	int	i, ix, n;
	char	*s, *lastFile = 0;

	fileVitalStats(logfname, &nlines, &lmax);
	lmax++;
	lbuf    = (char *) mallocOrElse(lmax);
	*pinfoc = nlines;
	*pinfov = (struct renum *) mallocOrElse(nlines * sizeof(struct renum));
	
	fin = fopenOrElse(logfname, "r");
	for (i = 0, ix = 0; i < *pinfoc; i++) {
		fgets(lbuf, lmax, fin);

		if (lbuf[0] == '"') {
			/* Update lastFile */
			n = strlen(lbuf) + 1;
			lastFile = (char *) mallocOrElse(n);
			strcpy(lastFile, lbuf);

			/* Skip leading '"' and obliterate trailing '"'. */
			lastFile++;
			for (s = lastFile; *s && *s != '"'; s++)
				;
			*s = 0;
		}
		else {
			(*pinfov)[ix].filename = lastFile;
			sscanf(lbuf, "%d %d",
				&(*pinfov)[ix].fakeno,
				&(*pinfov)[ix].realno);
			ix++;
		}
	}
	*pinfoc = ix;
	fclose(fin);

	for (i = 0; i < *pinfoc; i++) {
		printf("%d. '%s'[%d] => '%s' [%d]\n",
			i,
			(*pinfov)[i].filename, (*pinfov)[i].fakeno,
			(*pinfov)[i].filename, (*pinfov)[i].realno);
	}
}

/*****************************************************************************
 *
 * :: Utilities
 *
 ****************************************************************************/

/*
 * Find the number of lines and the length of the longest line in a file.
 */
void
fileVitalStats(fname, pnlines, plmax)
	char	*fname;
	int	*pnlines, *plmax;
{
	FILE	*fin;
	int	c, i, nlines, lmax;

	fin = fopenOrElse(fname, "r");

	nlines = 0;	/* Number of lines in file. */
	lmax   = 0;	/* Max line length so far, including \n. */
	i      = 0;	/* Current pos in line. */

	while ((c = getc(fin)) != EOF) {
		i++;
		if (c == '\n') {
			if (i > lmax) lmax = i;
			nlines++;
			i = 0;
		}
	}
	fclose(fin);
	*pnlines = nlines;
	*plmax   = lmax;
}

void
copyFile(infname, outfname)
	char	*infname, *outfname;
{
	char	buf[4096];
	int	n;
	FILE	*inf, *outf;

	inf  = fopenOrElse(infname,  "rb");
	outf = fopenOrElse(outfname, "wb");

	do {
		n = fread(buf, 1, sizeof(buf), inf);
		fwrite(buf, 1, n, outf);
	} while (n == sizeof(buf));

	fclose(inf);
	fclose(outf);
}

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

/*****************************************************************************
 *
 * :: Coff
 *
 ****************************************************************************/

/*
 * Set line number information in an object file.
 */
#define local static
typedef unsigned long	ULong;
typedef unsigned	Length;
typedef char		*String;

struct osObjectRenumber {
	char *	functionName;
	int	oldStart, newStart;
	int	lnoc;
	struct	osLineRenumber {int old, new;} *lnov;
};

#include <xcoff.h>
#include <stdio.h>
#include <ctype.h>

typedef unsigned long	FilePos;
typedef long		FileOffset;

typedef struct renumber {
	int	old;
	int	new;
} RENUM;

local RENUM *	renoGetLine	  (RENUM*);
local char *	renoGetFn	  (RENUM*);

local int	coffGetSectionLnos(FILE*,FILHDR*,long mask, FilePos*,ULong*);
local char *	coffGetString	  (FILE*,FILHDR*,FileOffset,char *,  Length);
local char *	coffGetFnString	  (FILE*,FILHDR*,int symndx,char *,  Length);
local int	coffSetFnStart	  (FILE*,FILHDR*,int symndx,int newStart);
local int	coffSetFnLine	  (FILE*,FilePos,int lnndx, int newLnno);

/****
 *
 * Main program
 *
 ****/

/* #define SYNCDEBUG */
#define MAX_FUN_NAME	1024

local void	renumFatal	     (char *, char *);
local FILE	*renumMustOpen	  (char *, char *);
local int	syncFn	     (FILE *, FILHDR *, FilePos, int symndx,
			      RENUM *pstart, RENUM *pexe);
local int	syncLine     (FILE *, FILHDR *, FilePos, int lnndx,  int lnno,
			      RENUM *pstart, RENUM *pexe);

struct osObjectRenumber *currentFun, *objectRenum;
local int	currentLine;

extern void
osObjectRenumber(String fname, struct osObjectRenumber *rn)
{
	FILE		*coff;
	FILHDR		filhdr;
	LINENO		lineno;
	RENUM		start, exe;
	FilePos		lnnoptr, wmi;
	unsigned long	nlnno;
	int		i;

	currentFun = 0;
	objectRenum = rn;
	coff = renumMustOpen(fname, "r+b");

	fread(&filhdr, FILHSZ, 1, coff);

	coffGetSectionLnos(coff, &filhdr, STYP_TEXT, &lnnoptr, &nlnno);

	wmi = lnnoptr;

	for (i = 0; i < nlnno; i++) {

#ifdef DEBUGIT
		printf("Would read from position %d\n", ftell(coff));
#endif
		fseek(coff, wmi, SEEK_SET);
#ifdef DEBUGIT
		printf("Reading line number from position %d\n", ftell(coff));
#endif
		fread(&lineno, LINESZ, 1, coff);
		wmi = ftell(coff);

		if (lineno.l_lnno == 0)
			syncFn	(coff,&filhdr,lnnoptr, lineno.l_addr.l_symndx,
				 &start,&exe);
		else
			syncLine(coff,&filhdr,lnnoptr, i, lineno.l_lnno,
				 &start, &exe);
	}
	fclose(coff);
}

local void
renumFatal(char *fmt, char *arg)
{
	fprintf(stderr, fmt, arg);
	exit(1);
}


local FILE *
renumMustOpen(char *fname, char *mode)
{
	FILE	*file;

	file = fopen(fname, mode);
	if (!file) renumFatal("Cannot open file %s.\n", fname);
	return file;
}

local int
syncFn(FILE *coff, FILHDR *phdr, FilePos lnnoptr, int symndx,
       RENUM *pstart, RENUM *pexe)
{
	unsigned long offset0 = ftell(coff);
	char	cn[MAX_FUN_NAME];
	char	*cs, *rs;

	cs = coffGetFnString(coff, phdr, symndx, cn,sizeof(cn));


	/* E.g. 83 */
	pexe->old = coffSetFnStart(coff,phdr,symndx, -1);
	/* E.g. 60 => 1 */
	rs = renoGetFn(pstart);
	if (strcmp(cs + 1, rs)) return -1;

	pexe->new = pstart->new; /* Assume code can start right away. */
	coffSetFnStart(coff,phdr,symndx, pexe->new);


	fseek(coff, offset0, SEEK_SET);

#ifdef DEBUGIT
	printf("Function: %s\n", cs);
	printf("Old: Src= %d, Exe= %d\n", pstart->old, pexe->old);
	printf("New: Src= %d, Exe= %d\n", pstart->new, pexe->new);
#endif

	return 0;
}

local int
syncLine(FILE *coff, FILHDR *phdr, FilePos lnnoptr, int lnndx, int lnno,
	 RENUM *pstart, RENUM *pexe)
{
	RENUM	renumber, *rn;
	int	lnnoNew;

	do {
		rn = renoGetLine(&renumber);
		if (!rn) return -1;
#ifdef SYNCDEBUG
		printf("(%d->%d)\n", rn->old, rn->new);
#endif
	}
	while (lnno > rn->old - pexe->old + 1);

	lnnoNew = rn->new - pexe->new + 1;

	if (lnnoNew < 1) lnnoNew = 1;

#ifdef DEBUGIT
	printf("	line: %d + %d = %d // %d + %d = %d.\n",
	       pexe->old, lnno	  - 1, pexe->old + lnno	   - 1,
	       pexe->new, lnnoNew - 1, pexe->new + lnnoNew - 1);
#endif

	coffSetFnLine(coff, lnnoptr, lnndx, lnnoNew);

	return 0;
}

/****
 *
 * Renumbering-file functions
 *
 ****/

/*
 * Scan a renumbering line of the form "43 -> 102\n"
 * 0 indicates error.
 */
local RENUM *
renoGetLine(RENUM *rn)
{
	rn->old = currentFun->lnov[currentLine].old;
	rn->new = currentFun->lnov[currentLine++].new;
	if (currentLine > currentFun->lnoc) return 0;
	return rn;
}

/*
 * Scan forward in file to text of the form "Function: xxxxx\nNNN => MMM\n"
 * and extract the name "xxxxx" and starting line numbers "NNN", "MMM".
 * "NNN" is the starting line of the function in the .c file and
 * "MMM" is the starting line of the function in the true source.
 * Return the function name or 0 if not possible.
 */
local char *
renoGetFn(RENUM *pstart)
{
	if (currentFun == 0) currentFun = objectRenum;
	else {
		currentFun++;
		currentLine = 0;
	}
	pstart->old = currentFun->oldStart;
	pstart->new  = currentFun->newStart;
	return (currentFun)->functionName;
}

/****
 *
 * COFF functions
 *
 ****/

/*
 * Get the offset and count of line number entries for a given section.
 * Returns 0 on success, -1 on failure.
 */
local int
coffGetSectionLnos(FILE *coff, FILHDR *phdr, long scnmask,
		   FilePos *plnoptr, unsigned long *pnlnno)
{
	int	i;
	SCNHDR	scnhdr;

	fseek(coff, FILHSZ + phdr->f_opthdr, SEEK_SET);

	for (i = 0; i < phdr->f_nscns; i++) {
		fread(&scnhdr, SCNHSZ, 1, coff);

		if (scnhdr.s_flags & scnmask) {
			if (plnoptr) *plnoptr = scnhdr.s_lnnoptr;
			if (pnlnno)  *pnlnno  = scnhdr.s_nlnno;
			return 0;
		}
	}
	return -1;
}

/*
 * Get a string from an object file, given the starting offset
 * within the strings section.
 */
local char *
coffGetString(FILE *coff, FILHDR *phdr, FileOffset offset,
	      char *buf, Length buflen)
{
	int		c;
	char		*buf0 = buf;
	FileOffset	str_ptr = phdr->f_symptr + phdr->f_nsyms * SYMESZ;

	fseek(coff, str_ptr + offset, SEEK_SET);
	while ((c = fgetc(coff)) != 0 && buflen > 1) {
			*buf++ = c;
			buflen--;
	}
	*buf++ = 0;
	return buf0;
}

/*
 * Get the name for a function symbol table entry.
 */
local char *
coffGetFnString(FILE *coff, FILHDR *phdr, int symndx,
		char *buf, Length buflen)
{
	SYMENT	syment;

	fseek(coff, phdr->f_symptr + symndx*SYMESZ, SEEK_SET);
	fread(&syment, SYMESZ, 1, coff);

	if (!syment.n_zeroes)
		coffGetString(coff, phdr, syment.n_offset, buf, buflen);
	else  {
		strncpy(buf, syment.n_name, buflen);
		if (sizeof(syment.n_name) >= buflen) buf[buflen-1] = 0;
	}

	return buf;
}

local int
dofwrite(void *p, unsigned sz, unsigned ct, FILE *fout)
{
#ifdef DEBUGIT
	unsigned long pos = ftell(fout);
	printf("Writing at %d items of size %d at position %d\n", ct, sz, pos);
#endif
	return fwrite(p, sz, ct, fout);
}

local int
dofseek(FILE *f, long off, int how)
{
	int	rc;
#ifdef DEBUGIT
	printf("Seeking to %d in mode %d from %d\n", off, how, ftell(f));
#endif
	rc = fseek(f, off, how);
#ifdef DEBUGIT
	printf("Sought to %d\n", ftell(f));
#endif
	return rc;
}

/*
 * Set the number for the first executable line in a function.
 * Return the old value, or -1 if it is not possible.
 */
local int
coffSetFnStart(FILE *coff, FILHDR *phdr, int symndx, int newStart)
{
	SYMENT	syment;
	AUXENT	auxent;
	int	j, k, oldStart;

#ifdef DEBUGIT
	printf("***************** The symprt is %d\n", phdr->f_symptr);
#endif
	dofseek(coff, phdr->f_symptr + symndx*SYMESZ, SEEK_SET);

	/* Find syment for .bf and get line from its first auxent. */
	for (j = 0; ; j++) {
		fread(&syment, SYMESZ, 1, coff);
		for (k = 0; k < syment.n_numaux; k++) {
			fread(&auxent, AUXESZ, 1, coff);
			if (j > 0 && syment.n_sclass == C_FCN) {
				oldStart = auxent.x_sym.x_misc.x_lnsz.x_lnno;
				if (newStart != -1) {
					auxent.x_sym.x_misc.x_lnsz.x_lnno =
						newStart;
					dofseek(coff, -AUXESZ, SEEK_CUR);
					dofwrite(&auxent, AUXESZ, 1, coff);
				}
				return oldStart;
			}
		}
	}
	return -1;
}

local int
coffSetFnLine(FILE *coff, FilePos lnnoptr, int lnndx, int newLnno)
{
	LINENO	lineno;
	int	oldLnno;

#ifdef DEBUGIT
	printf("The lnnoptr is %d\n", lnnoptr);
	printf("Setting to new line number %d\n", newLnno);
#endif
	dofseek(coff, lnnoptr + lnndx * LINESZ, SEEK_SET);
	fread(&lineno, LINESZ, 1, coff);
	oldLnno = lineno.l_lnno;
	lineno.l_lnno = newLnno;
	dofseek(coff, lnnoptr + lnndx * LINESZ, SEEK_SET);
#ifdef DEBUGIT
	printf("Writing line number to position %d\n", ftell(coff));
#endif
	dofwrite(&lineno, LINESZ, 1, coff);
	return oldLnno;
}
