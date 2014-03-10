/*
 * Foreign functions for Aldor output functions.
 */

#include "axlgen.h"
#include "bigint.h"
#include "foam_c.h"
#include "opsys.h"
#include "store.h"

local String
outputStrAlloc(Length n)
{
	String  s;
	s = (String) stoAlloc((unsigned) OB_String, n + 1);
	s[0] = 0;
	s[n] = 0;
	return s;
}

local void
outputStrFree(String s)
{
	stoFree((Pointer) s);
}


/*
 * Like fgets, but works on the substring s[start..limit-1].
 * Returns the number of characters read.
 */
int
fgetss(s, start, limit, fin)
	char	*s;
	int	start, limit;
	FILE	*fin;
{
	int	i, c;

	for (i = start; i < limit; i++) {
		c = fgetc(fin);
		if (c == EOF)  break;
		if (c == '\n') { s[i++] = c; break; }
		s[i] = c;
	}
	s[i] = 0;
	return i - start;
}

/*
 * Like fputs, but works on the substring s[start..limit-1].
 * Returns the number of characters put.
 */
int
fputss(s, start, limit, fout)
	char	*s;
	int	start, limit;
	FILE	*fout;
{
	int	i;

	if (!s) return 0;
	if (limit == -1)
	{
		/*
		 * This is wrong because fputs does not have to
		 * return the number of characters written. It
		 * only has to return a non-negative integer on
		 * success and EOF on failure.
		 */
		return fputs(s+start, fout);
	}

	for (i = start;  i < limit && s[i]; i++) fputc(s[i], fout);
	return i - start;
}

FILE *
stdinFile()
{
	return  osStdin;
}

FILE *
stdoutFile()
{
	return osStdout;
}

FILE *
stderrFile()
{
	return osStderr;
}

static char	inibuf[200];

static char	*buf       = &inibuf[0];
static int	bufferSize = sizeof(inibuf);
static int	bufIsAlloc = 0;

char *
formatSInt(FiSInt i)
{
	sprintf(buf, "%ld", i);
	return buf;
}

char *
formatBInt(FiBInt i)
{
#if FOAM_RTS
	return fiBIntToString(i);
#else
	extern int	bintStringSize();
	extern char *	bintIntoString();

	int len = bintStringSize((BInt) i);

	if (len > bufferSize) {
		if (bufIsAlloc) outputStrFree(buf);

		/* Ensure buffer grows by at least 50 %. */
		if (len < (3 * bufferSize)/2) len = (3*bufferSize)/2;
		
		buf        = outputStrAlloc(len);
		bufferSize    = len;
		bufIsAlloc = 1;
	}
	bintIntoString(buf, (BInt) i);	
#endif
	return buf;
}

/*fixes the differences in the exponent output between windows and linux*/
local void
outputFixFloat()
{
	int i, j;
	int len = strlen(buf);
	for (i = j = 0; i < len+1; i++, j++) {
		if ((buf[i] == 'e' || buf[i] == 'E')
		    && buf[i+2] == '0' 
		    && buf[i+3] != '\0'
		    && buf[i+4] != '\0'
		    && buf[i+5] == '\0') {
		        i = i + 2;
			j = j + 3;
		}
		if (i != j) buf[i] = buf[j];
	}
}

char *
formatSFloat(FiSFlo sf)
{
	int prec = 9;
	sprintf(buf, "%.*g", prec, sf);
	outputFixFloat();
	return buf;
}

char *
formatDFloat(FiDFlo d)
{
	int prec = 17;
	sprintf(buf, "%.*g", prec, d);
	outputFixFloat();
	return buf;
}
