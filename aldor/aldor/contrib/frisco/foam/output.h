/*
 * Foreign functions for Aldor output functions.
 *
 */

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

# include <stdio.h>

extern FILE * stdinFile(void);
extern FILE * stdoutFile(void);
extern FILE * stderrFile(void);

extern char * formatSInt(FiSInt);
extern char * formatBInt(FiBInt);
extern char * formatSFloat(FiSFlo);
extern char * formatDFloat(FiDFlo);

extern int    fputss	( char *, int, int, FILE * );
extern int    fgetss	( char *, int, int, FILE * );
#endif
