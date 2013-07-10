/****************************** flags.c ******************************

   Author: Manuel Bronstein
   Date Last Updated: 17 February 1995

   Utilities to treat command line flags.
*/


#include <stdio.h>
#include "flags.h"
#include <stdlib.h>

/* char preceding a flag */
#define FLAG_SIGNAL	'-'

/* flags which are followed by a string argument */
#define	STRINGFLAGS	"lfoIL"

static int member(char c, char *s)
{
	while (*s) if (*(s++) == c) return 1;
	return 0;
}

static int getfl(char *s, char c)
{
	int stop, cont = *s;
	while (cont) {
		stop = member(*s, STRINGFLAGS);
		if (*(s++) == c) return(atoi(s));
		cont = *s && !stop;
	}
	return(-1);
}

static char *getflname(char *s, char c)
{
	int stop, cont = *s;
	while (cont) {
		stop = member(*s, STRINGFLAGS);
		if (*(s++) == c) return s;
		cont = *s && !stop;
	}
	return NULL;
}

/* Returns the numerical value of a flag, or -1 if not there */
int getFlag(int ac, char **ag, char flag)
{
	int i, val;
	for (i = 1; i < ac; i++)
	    if ((ag[i][0] == FLAG_SIGNAL)
		 && ((val = getfl(ag[i], flag))) >= 0)
			return val;
	return(-1);
}

/* Returns the string value of a flag, or NULL if not there */
char *getFlagName(int ac, char **ag, char flag)
{
	int i;
	char *s;
	for (i = 1; i < ac; i++)
	    if ((ag[i][0] == FLAG_SIGNAL)
		&& ((s = getflname(ag[i], flag)) != NULL))
			return((*s || (i == ac)) ? s : ag[++i]);
	return NULL;
}

/* Returns the pos-th non-flag string argument */
char *getNthArgument(int ac, char **ag, int pos)
{ int i, count = 0;

	for (i = 1; i < ac; i++)
	    if ((ag[i][0] != FLAG_SIGNAL) && (++count == pos))
		return(ag[i]);
	return NULL;
}

