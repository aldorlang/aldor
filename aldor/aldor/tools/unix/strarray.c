/*
 * Usage: strarray infile nm
 *
 * This program is used to generate C code for an array of strings containing
 * the lines from the file "infile".
 * With "nm" given as the second argument, it produces files "nm.h" and "nm.c".
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_PATH	1000

FILE *
mustOpen(name, type, mode)
	char	*name, *type, *mode;
{
	FILE	*f;
	char	fn[MAX_PATH];

	sprintf(fn, "%s%s", name, type);
	f = fopen(fn, mode);
	if (!f) { fprintf(stderr, "Can't open %s.\n", fn); exit(EXIT_FAILURE); }
	return f;
}

int
genLines(fin, fout, sep)
	FILE	*fin, *fout;
	char	*sep;
{
	int	c, n, EOL;

	c = getc(fin);
	for (n = 0; c != EOF; ) {
		if (n > 0) fprintf(fout, "%s", sep);
		n++;

		fputc('"', fout);
		for (EOL = 0; !EOL ; c = getc(fin)) {
			switch (c) {
			case '\n': fprintf(fout, "\\n");
			case EOF : EOL = 1;                  break;
			case '\t': fprintf(fout, "\\t");     break;
			case '\v': fprintf(fout, "\\v");     break;
			case '\b': fprintf(fout, "\\b");     break;
			case '\r': fprintf(fout, "\\r");     break;
			case '\f': fprintf(fout, "\\f");     break;
			case '\\': fprintf(fout, "\\\\");    break;
			case '\"': fprintf(fout, "\\\"");    break;
			default:   fputc(c, fout);           break;
			}
		}
		fputc('"', fout);
	}
	return n;
}

void
genH(infname, basename)
	char	*infname;
	char	*basename;
{
	FILE	*fout = mustOpen(basename, ".h", "w");

	fprintf(fout, "/*\n");
	fprintf(fout, " * %s.h -- generated from file \"%s\".\n",
			basename, infname);
	fprintf(fout, " *\n");
	fprintf(fout, " * This array contains the contents of file \"%s\"\n",
			infname);
	fprintf(fout, " * and is terminated by a 0 pointer.\n");
	fprintf(fout, " */\n");
	fprintf(fout, "\n");
	fprintf(fout, "extern char * %s_array[];\n", basename);

	fclose(fout);
}

void
genC(infname, basename)
	char	*infname;
	char	*basename;
{
	FILE	*fin  = mustOpen(infname,  "",   "r");
	FILE	*fout = mustOpen(basename, ".c", "w");
	int	n;

	fprintf(fout, "/*\n");
	fprintf(fout, " * %s.c -- generated from file \"%s\".\n",
			basename, infname);
	fprintf(fout, " *\n");
	fprintf(fout, " * This array contains the contents of file \"%s\"\n",
			infname);
	fprintf(fout, " * and is terminated by a 0 pointer.\n");
	fprintf(fout, " */\n");
	fprintf(fout, "\n");
	fprintf(fout, "char * %s_array[] = {\n  ", basename);
	n = genLines(fin, fout, ",\n  ");
	fprintf(fout, n == 0 ? "\n" : ",\n");
	fprintf(fout, "  0\n");
	fprintf(fout, "};\n");

	fclose(fin);
	fclose(fout);
}

int
main(argc, argv)
	int	argc;
	char	**argv;
{
	if (argc != 3) {
		fprintf(stderr, "Usage: strarray infile outbase\n");
		fprintf(stderr, "       produces outbase.h and outbase.c\n");
		exit(EXIT_FAILURE);
	}

	genC(argv[1], argv[2]);
	genH(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
