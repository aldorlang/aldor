/****************************** sal_util.c *********************************
 * Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97       *
 * Copyright (c) Manuel Bronstein 1994-2000                                *
 * Copyright (c) INRIA 1998, Version 29-10-98                              *
 * Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998            *
 ***************************************************************************/

#include <stdio.h>
#include <errno.h>

long cerrno() { return((long) errno); }
long lfputc(long c, FILE *stream)  { return((long) fputc((int) c, stream)); }
long lungetc(long c, FILE *stream) { return((long) ungetc((int) c, stream)); }
long fseekset(FILE *s, long n)     { return((long) fseek(s, n, SEEK_SET)); }
long fseekcur(FILE *s, long n)     { return((long) fseek(s, n, SEEK_CUR)); }
long fseekend(FILE *s, long n)     { return((long) fseek(s, n, SEEK_END)); }

#if defined (_MSC_VER) || defined(__MINGW32__)
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

/* returns a time-based seed for the random number generator */
long randomSeed()
{
#if defined(_MSC_VER) || defined(__MINGW32__)
	struct _timeb tv;	/* Microsoft compiler does not offer */
	_ftime(&tv);		/* timeval and gettimeofday */
	return (tv.millitm - tv.time); /* closest match is _ftime */
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);        /* supported by most O/S */
	return(tv.tv_usec - tv.tv_sec); /* some minimal shuffling */
#endif
}

/* gcc defines 'sun' on sunos, that platform is missing powf */
#ifdef sun
#include <math.h>

float powf(x, y)
float x, y;
{
	return((float) pow((double) x, (double) y));
}
#endif

#if defined (_MSC_VER)
#include <math.h>

#endif
