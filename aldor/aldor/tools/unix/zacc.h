/*
 * zacc.h
 */

#ifndef _ZACC_H_
#define _ZACC_H_

/* This program uses "of" and "stdarg" simulation. */
# include "zcport.h"

#define YYSTYPE yystype
typedef union yystype {
	char	*str;
} yystype;

extern void	token   	of((int, char *));

extern void	startProlog	of((void));
extern void	endProlog	of((void));

extern void	wholeEpilog	of((void));

extern void	startCommand	of((void));
extern void	endCommand	of((void));

extern void	startRule	of((void));
extern void	midRule		of((void));
extern void	endRule		of((void));

extern void	startPhraseName of((void));
extern void	endPhaseName  	of((void));

extern void	startAction	of((void));
extern void	endAction	of((void));

extern void	seeName		of((char *));
extern void	seeArg		of((char *));
extern void	seeType		of((char *));

extern void	seeTokenType	of((char *));
extern void	seeRuleType	of((char *));

extern void     incLineNo	of((void));
#endif
