
/*
 * Name:
 *    txt2txt
 *
 * Purpose:
 *    Convert between different text formats.
 *
 * Description:
 *    Reads text from stdin and writes the results to stdout.
 *
 * Syntax:
 *    txt2txt -to <fmt> [-from <fmt>] [-v]
 *
 *       -v:    verbose (displays input file format on stderr)
 *       -to:   specifies the output format to use
 *       -from: force input to be read in the specified format
 *       <fmt>: one of dos, unix or mac
 *
 *    The output format must be specified.
 *
 * Author:
 *    Martin N Dunstan (martin@nag.co.uk)
 *    {list_authors_here}
 *
 * History:
 *    11-Dec-2000 (mnd):
 *       Created.
 *    {list_changes_here}
 *
 * Notes:
 *    <CR> means carriage-return (ASCII 13)
 *    <LF> means line-feed (ASCII 10)
 *
 *    DOS files have lines terminated by the sequence <CR><LF>
 *    UNIX files have lines terminated by <LF>
 *    Mac files have lines terminated by <CR>
 */

#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE		(10240)
#define CR		((int)13)
#define LF		((int)10)

typedef char *(*Reader)(FILE *);
typedef void (*Writer)(FILE *, char *);
typedef enum { false, true } bool;

static Reader guessFileFormat(FILE *, FILE *, Writer);
static char *readDOS(FILE *);
static char *readUNIX(FILE *);
static char *readMac(FILE *);
static void writeDOS(FILE *, char *);
static void writeUNIX(FILE *, char *);
static void writeMac(FILE *, char *);
void setToDos(void);
void setToUnix(void);
void setToMac(void);
void setFromDos(void);
void setFromUnix(void);
void setFromMac(void);
static void conflict(char *, char *, char *);
static void missingArgError(char *);
static void badArgError(char *);
static void syntax(void);
static void missingToArg(void);

static char buffer[BUFSIZE+1];

/* Globals to reduce the number of arguments passed to functions */
static bool gotToDos = false;
static bool gotToUnix = false;
static bool gotToMac = false;
static bool gotFromDos = false;
static bool gotFromUnix = false;
static bool gotFromMac = false;


int main(int argc, char *argv[])
{
	char	*line;
	bool	from, verbose = false;
	Reader	reader = (Reader)NULL;
	Writer	writer = (Writer)NULL;


	/* Check the number of command line arguments */
	switch (argc)
	{
	   case 0:
		(void)fprintf(stderr, "Must specify argv[0]\n");
		return 1;
	   case 1:
		syntax();
		return 1;
	   default:
		/* Skip past the program name */
		argc--, argv++;
		break;
	}


	/* Parse the command line arguments */
	while (argc)
	{
		/* Store a pointer to the next argument */
		char *arg = argv[0];


		/* -to and -from are special */
		if (!strcmp(arg, "-to") || !strcmp(arg, "-from"))
		{
			/* Note the direction */
			from = !strcmp(arg, "-from") ? true : false;


			/* Move onto the next argument */
			argc--, argv++;


			/* Must have an argument ... */
			if (!argc)
			{
				missingArgError(arg);
				return 1;
			}


			/* Update arg */
			arg = argv[0];
		}
		else if (!strcmp(arg, "-v"))
		{
			verbose = true;
			argc--, argv++;
			continue;
		}
		else
		{
			badArgError(arg);
			return 1;
		}


		/* Check which option it is */
		if (!strcmp(arg, "dos"))
		{
			if (from)
				setFromDos();
			else
				setToDos();
		}
		else if (!strcmp(arg, "unix"))
		{
			if (from)
				setFromUnix();
			else
				setToUnix();
		}
		else if (!strcmp(arg, "mac"))
		{
			if (from)
				setFromMac();
			else
				setToMac();
		}
		else
		{
			/* Display the whole option */
			badArgError(argv[0]);
			return 1;
		}


		/* Always move onto the next option */
		argc--, argv++;
	}


	/* Must have -to */
	if (gotToDos)
		writer = writeDOS;
	else if (gotToUnix)
		writer = writeUNIX;
	else if (gotToMac)
		writer = writeMac;
	else
		missingToArg();


	/* Deal with -from */
	if (gotFromDos)
		reader = readDOS;
	else if (gotFromUnix)
		reader = readUNIX;
	else if (gotFromMac)
		reader = readMac;
	else
		reader = guessFileFormat(stdin, stdout, writer);


	/* Display the reader we are using if -v given */
	if (verbose)
	{
		(void)fprintf(stderr, "[%s]\n",
			((long)reader == (long)readDOS) ? "dos" :
			(((long)reader == (long)readMac) ? "mac" : "unix"));
	}


	/* Process the file as requested */
	line = reader(stdin);
	while (line)
	{
		writer(stdout, line);
		line = reader(stdin);
	}


	/* Success */
	return 0;
}


static Reader guessFileFormat(FILE *in, FILE *out, Writer writer)
{
	long	ptr;
	int	done, chr;
	int	last = (int)'\0';
	Reader	result = readDOS;

	for (ptr = done = 0; !done && (ptr < BUFSIZE);)
	{
		/* Read the next character */
		chr = fgetc(in);


		/* If the last character was CR then must be Mac or DOS */
		if (last == CR)
		{
			/* DOS or Mac format */
			if (chr == LF)
			{
				/* DOS format */
				result = readDOS;
				done = 1;
			}
			else
			{
				/* Mac format - replace last character */
				if (chr != -1) (void)ungetc(chr, in);
				result = readMac;
				done = 1;
			}
		}
		else if (chr == LF)
		{
			/* UNIX format */
			result = readUNIX;
			done = 1;
		}
		else if (chr == -1)
		{
			/* Pick any reader - doesn't matter */
			result = readDOS;
			done = 1;
		}
		else if ((chr != CR) && (chr != LF))
			buffer[ptr++] = (char)chr;


		/* Remember this character */
		last = chr;
	}

	/* Terminate the line */
	buffer[ptr] = '\0';


	/* Emit the line if something was read */
	if (!((chr == -1) && !ptr)) writer(out, buffer);


	/* Return the reader */
	return result;
}


static char *readDOS(FILE *handle)
{
	long ptr;
	int done, chr, last = (int)'\0';

	for (ptr = done = 0; !done && (ptr < BUFSIZE);)
	{
		/* Read the next character */
		chr = fgetc(handle);


		/* CR must be followed by LF to mark EOL */
		if (last == CR)
		{
			/* Emit CR if followed by anything other than LF */
			if (chr != LF)
			{
				buffer[ptr++] = (char)last;

				/* Might be the end of the file */
				if (chr != -1)
					buffer[ptr++] = (char)chr;
				else
					done = 1;
			}
			else
				done = 1;
		}
		else if (chr == -1)
			done = 1;
		else if (chr != CR)
			buffer[ptr++] = (char)chr;


		/* Remember this character */
		last = chr;
	}

	/* Terminate the line */
	buffer[ptr] = '\0';


	/* Return NULL on EOF with nothing read */
	return ((chr == -1) && !ptr) ? (char *)NULL : buffer;
}


static char *readUNIX(FILE *handle)
{
	long ptr;
	int chr, done;

	for (ptr = done = 0; !done && (ptr < BUFSIZE);)
	{
		/* Read the next character */
		chr = fgetc(handle);


		/* Stop on LF or EOF */
		if ((chr == LF) || (chr == -1))
			done = 1;
		else
			buffer[ptr++] = (char)chr;
	}

	/* Terminate the line */
	buffer[ptr] = '\0';


	/* Return NULL on EOF with nothing read */
	return ((chr == -1) && !ptr) ? (char *)NULL : buffer;
}


static char *readMac(FILE *handle)
{
	long ptr;
	int chr, done;

	for (ptr = done = 0; !done && (ptr < BUFSIZE);)
	{
		/* Read the next character */
		chr = fgetc(handle);


		/* Stop on CR or EOF */
		if ((chr == CR) || (chr == -1))
			done = 1;
		else
			buffer[ptr++] = (char)chr;
	}

	/* Terminate the line */
	buffer[ptr] = '\0';


	/* Return NULL on EOF with nothing read */
	return ((chr == -1) && !ptr) ? (char *)NULL : buffer;
}


static void writeDOS(FILE *handle, char *line)
{
	(void)fputs(line, handle);
	(void)fputc((char)CR, handle);
	(void)fputc((char)LF, handle);
}


static void writeUNIX(FILE *handle, char *line)
{
	(void)fputs(line, handle);
	(void)fputc((char)LF, handle);
}


static void writeMac(FILE *handle, char *line)
{
	(void)fputs(line, handle);
	(void)fputc((char)CR, handle);
}


static void conflict(char *arg, char *got, char *had)
{
	char *fmt = "Error: `-%s %s' conflicts with -%s %s\n";
	(void)fprintf(stderr, fmt, arg, got, arg, had);
	exit(1);
}


void setToDos(void)
{
	/* Check for repeated/clashing options */
	if (gotToUnix || gotToMac)
		conflict("-to", "dos", gotToUnix ? "unix" : "mac");
	else if (gotToDos)
		(void)fprintf(stderr, "Warning: `-to dos' repeated\n");


	/* Update the flags */
	gotToDos  = true;
	gotToUnix = gotToMac = false;
}


void setToUnix(void)
{
	/* Check for repeated/clashing options */
	if (gotToDos || gotToMac)
		conflict("-to", "unix", gotToDos ? "dos" : "mac");
	else if (gotToUnix)
		(void)fprintf(stderr, "Warning: `-to unix' repeated\n");


	/* Update the flags */
	gotToUnix = true;
	gotToDos  = gotToMac = false;
}


void setToMac(void)
{
	/* Check for repeated/clashing options */
	if (gotToDos || gotToUnix)
		conflict("-to", "mac", gotToDos ? "dos" : "unix");
	else if (gotToMac)
		(void)fprintf(stderr, "Warning: `-to mac' repeated\n");


	/* Update the flags */
	gotToMac = true;
	gotToDos = gotToUnix = false;
}


void setFromDos(void)
{
	/* Check for repeated/clashing options */
	if (gotFromUnix || gotFromMac)
		conflict("-from", "dos", gotFromUnix ? "unix" : "mac");
	else if (gotFromDos)
		(void)fprintf(stderr, "Warning: `-from dos' repeated\n");


	/* Update the flags */
	gotFromDos  = true;
	gotFromUnix = gotFromMac = false;
}


void setFromUnix(void)
{
	/* Check for repeated/clashing options */
	if (gotFromDos || gotFromMac)
		conflict("-from", "unix", gotFromDos ? "dos" : "mac");
	else if (gotFromUnix)
		(void)fprintf(stderr, "Warning: `-from unix' repeated\n");


	/* Update the flags */
	gotFromUnix = true;
	gotFromDos  = gotFromMac = false;
}


void setFromMac(void)
{
	/* Check for repeated/clashing options */
	if (gotFromDos || gotFromUnix)
		conflict("-from", "mac", gotFromDos ? "dos" : "unix");
	else if (gotFromMac)
		(void)fprintf(stderr, "Warning: `-from mac' repeated\n");


	/* Update the flags */
	gotFromMac = true;
	gotFromDos = gotFromUnix = false;
}


static void badArgError(char *arg)
{
	(void)fprintf(stderr, "Unrecognised option `%s'\n", arg);
}


static void missingArgError(char *arg)
{
	(void)fprintf(stderr, "Missing argument to option `%s'\n", arg);
}


static void missingToArg(void)
{
	(void)fprintf(stderr, "Option -to <fmt> MUST be specified\n");
}


static void syntax(void)
{
#define XOUT(f,s)	(void)fprintf(stderr, f, s)

	XOUT("%s\n", "txt2txt -to <fmt> [-from <fmt>]");
	XOUT("\n", "");
	XOUT("%8s: ", "-to");
	XOUT("%s\n", "specifies the output format");
	XOUT("%8s: ", "-from");
	XOUT("%s\n", "force input to be read in the specified format");
	XOUT("%8s: ", "<fmt>");
	XOUT("%s\n", "one of dos, unix or mac");
}

