/*****************************************************************************
 *
 * fint.h: foam interpreter.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FINT_H_
#define _FINT_H_

extern void	fintInit 		(void);
extern void	fintFini 		(void);
extern Bool     fint			(Foam);
extern void	fintFile		(FileName);
extern void	fintInitFile		(void);
extern void     fintPrintType           (FILE *, AbSyn);


extern void	fintPrintType		(FILE *, AbSyn);
extern Bool	fintYesOrNo		(String);
extern Bool	fintIsCompilerSyntax	(void);

extern void	fintGetInitCompTime	(void);
extern void	fintDisplayTimings	(void);

extern void	fintWhere		(int);
extern void     fintRaiseException(char *, void *);

//extern int      compGDebugger           (int, char **, FILE *, FILE *);
extern int      compCmd                 (int, char**);
extern void     compFini                (void);

extern int	fintMode;
#define			FINT_DONT	0
#define 		FINT_RUN	1
#define 		FINT_LOOP	2
#define 		FINT_DEBUGGER	3

#define                 memory_alignment(n, d) (((n)+sizeof(double)) % (d) ? (n) + (d) - (n) % (d) : (n))
#define                 MAXLINE 256

extern Bool	fintVerbose;
extern Bool	fintHistory;
extern UShort	intStepNo;
extern Bool 	fintConfirm;
extern Bool	fintTimings;
extern long 	fintMsgLimit;
extern long     fintExntraceMode;
extern void     fintDebugProg(FileName fname);
extern void     fintFreeJunk(void);

#endif    /* _FINT_H */


