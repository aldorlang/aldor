/*
 * dosfile file ...
 *
 * Converts dos (unix) files to unix (dos) files and removes the originals.
 */

# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#if 0
# include <sys/param.h>
#endif

extern int	isUnixName(), isDosName(), isBinFile();
extern int	toLower(), toUpper();
extern char	*mkUnixName(), *mkDosName();

int		isQuiet = 0;

main(argc, argv)
	int	argc;
	char	**argv;
{
	int	i;

	if (argc > 1 && !strcmp(argv[1], "-q")) {
		argc--;
		argv++;
		isQuiet = 1;
	}

	for (i = 1; i < argc; i++)
		if (isBinFile(argv[i])) {
			if (!isQuiet)
				printf("Not converting binary file %s\n",
					argv[i]);
		}
		else
			dosconvert(argv[i]);
}

int
isBinFile(fname)
	char	*fname;
{
	FILE	*infile = fopen(fname, "rb");
	int	c, isbin = 0;
	
	while (c = getc(infile), c != EOF)
		if (!isprint(c) && !isspace(c)) { isbin = 1; break; }

	fclose(infile);
	return isbin;
}

dosconvert(fname)
	char	*fname;
{
	if (isDosName(fname))
		mkUnixFile(fname);
	else
		mkDosFile(fname);
}

mkUnixFile(fname)
	char	*fname;
{
	char	*new_fname;
	FILE	*instr, *outstr;
	
	new_fname = mkUnixName(fname);
	instr = fopen(fname, "r");
	if (instr == NULL) {
		fprintf(stderr, "Cannot read from file %s.\n", fname);
		return;
	}

	outstr= fopen(new_fname, "w");
	if (outstr == NULL) {
		fprintf(stderr, "Cannot write to file %s.\n", new_fname);
		return;
	}

	if (!isQuiet)
		printf("Producing Unix file %s from Dos file %s.\n",
			new_fname, fname);
	uncr(instr, outstr);

	fclose(instr);
	fclose(outstr);

	unlink(fname);
}
	
mkDosFile(fname)
	char	*fname;
{
	char	*new_fname;
	FILE	*instr, *outstr;
	
	new_fname = mkDosName(fname);
	instr = fopen(fname, "r");
	if (instr == NULL) {
		fprintf(stderr, "Cannot read from file %s.\n", fname);
		return;
	}

	outstr= fopen(new_fname, "w");
	if (outstr == NULL) {
		fprintf(stderr, "Cannot write to file %s.\n", new_fname);
		return;
	}

	if (!isQuiet)
		printf("Producing Dos file %s from Unix file %s.\n",
			new_fname, fname);
	crlf(instr, outstr);

	fclose(instr);
	fclose(outstr);

	unlink(fname);
}

/*
 * Changes \r\n to \n and removes trailing ^Z
 * for conversion of DOS text files to Unix.
 *
 * For now, assume that all \r and ^Z are to be discarded!
 */

int ctlZ = 'Z' - 0100;

uncr(instr, outstr)
	FILE	*instr, *outstr;
{
	int	c;

	while ((c=getc(instr)) != EOF)
		if (c != '\r' && c != ctlZ) putc(c, outstr);
}

/*
 *
 * Changes \n to \r\n for conversion to dos format.
 */

crlf(instr, outstr)
	FILE	*instr, *outstr;
{
	int	c;
	
	while ((c = getc(instr)) != EOF) {
		if (c == '\n') putc('\r', outstr);
		putc(c, outstr);
	}
}

int
toLower(c)
	int	c;
{
	if (!isupper(c)) return c;
	return c - 'A' + 'a';
}

int
toUpper(c)
	int	c;
{
	if (!islower(c)) return c;
	return c - 'a' + 'A';
}

int
isDosName(fname)
	char *fname;
{
	char	*s;
	int	nlower;
	
	for (nlower = 0, s = fname; *s; s++) {
		if (*s == '/') nlower = 0;
		if (islower(*s)) nlower++;
	}
	return nlower == 0;
}

int
isUnixName(fname)
	char *fname;
{
	char	*s;
	int	nlower;
	
	for (nlower = 0, s = fname; *s; s++)
		if (islower(*s)) nlower++;
	return nlower != 0;
}

char *
mkDosName(fname)
	char	*fname;
{
	char	*s, *new_fname;
	int	i, i0;
	
	new_fname = (char *) malloc(strlen(fname)+1);
	if (!new_fname) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}
	strcpy(new_fname, fname);

	for (i = 0, i0 = 0; fname[i]; i++)
		if (new_fname[i] == '/') i0 = i+1;

	for (s = new_fname + i0; *s; s++)
		*s = toUpper(*s);

	return new_fname;
}

char *
mkUnixName(fname)
	char	*fname;
{
	char	*s, *new_fname;
	int	i, i0;

	new_fname = (char *) malloc(strlen(fname)+1);
	if (!new_fname) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}
	strcpy(new_fname, fname);

	for (i = 0, i0 = 0; fname[i]; i++)
		if (new_fname[i] == '/') i0 = i+1;

	s = new_fname + i0;

	if (!strcmp(s, "MAKEFILE")) {
		strcpy(s, "Makefile");
	}
	else {
		for ( ; *s; s++)
			*s = toLower(*s);
	}

	return new_fname;
}
