
/*
 * includes [-make | -prj] file1 file2 ...
 *
 * Produce a lists of the .h files that the argument files depend on.
 *
 * Could be extended to use -I arguments.
 * SMW Sept 89
 */

# include <stdio.h>
# define  TABSTOP       12
# define  MAX_INCLUDE_LINE	256


# define  FMT_DEFAULT	2
# define  FMT_MAKE	1
# define  FMT_PRJ	2

int	format;

main(argc, argv)
        int     argc;
        char    *argv[];
{
	if (argc < 2) 
		format = FMT_DEFAULT;
	else if (!strcmp(argv[1], "-make")) {
		format = FMT_MAKE;
		--argc, ++argv;
	}
	else if (!strcmp(argv[1], "-prj")) {
		format = FMT_PRJ;
		--argc, ++argv;
	}
	else
		format = FMT_DEFAULT;

        while (--argc)
                princludes(*++argv);
}

struct flist {
        char    *fname;
        struct flist *next;
} *seen;

void
clearSeen()
{
        seen = 0;  /* Could de-allocate */
}

void
see(fname)
        char    *fname;
{
        struct flist    *head;

        head = (struct flist *) malloc(sizeof (struct flist));
        if (!head) goto memerr;
        head->fname = (char *) malloc(strlen(fname)+1);
        if (!head->fname) goto memerr;
        strcpy(head->fname, fname);
        head->next = seen;
        seen = head;
        return;
memerr:
        fprintf(stderr, "\nCannot allocate memory.\n");
        exit(1);

}

int
hasSeen(fname)
        char    *fname;
{
        struct flist    *head;

        for (head = seen; head; head = head->next)
                if (!strcmp(head->fname, fname)) return 1;
        return 0;
}

int
numSeen()
{
        struct flist    *head;
        int n;

        for (n = 0, head = seen; head; head = head->next)
                n++;
        return n;
}

princludes(fname)
        char    *fname;

{
        int     n;
	char	fbuf[100], *s;

        clearSeen();

	strcpy(fbuf, fname);
	for (s = fbuf; *s && *s != '.'; s++)
		;
#ifdef __MSDOS__
	strcpy(s, ".obj");
#else
	strcpy(s, ".o");
#endif

	if (format == FMT_MAKE) printf("%s:", fbuf); else printf("%s", fbuf);

        for (n = TABSTOP-strlen(fbuf); n > 0; n--) putchar(' ');

        if (format != FMT_MAKE) printf("(");
        inrec(fname);
	if (format == FMT_MAKE) printf("\n"); else printf(")\n");
}


inrec(fname)
        char    *fname;
{
        FILE    *instream;
        char    line[MAX_INCLUDE_LINE], 
		cmd[MAX_INCLUDE_LINE], 
		fnew[MAX_INCLUDE_LINE];

        instream = fopen(fname, "r");
        if (NULL == instream) {
                printf(" [Can't open %s]", fname);
                return;
        }

        while (!feof(instream)) {
                fgets(line, sizeof line, instream);
                if (line[0] == '#') {
			cmd[0] = 0; fnew[0] = 0;
			sscanf(line, "# %s %s\n", cmd, fnew);
			if (!strcmp(cmd, "include") && *fnew == '"') {
				char    *s;
				s = fnew + 1;
				s[strlen(s)-1] = 0;
				if (hasSeen(s))
					continue;
				if (numSeen() == 0 || format == FMT_MAKE)
					printf(" %s", s);
				else
					printf(", %s", s);
				see(s);
				inrec(s);
			}
                }
        }
	fclose(instream);
}
