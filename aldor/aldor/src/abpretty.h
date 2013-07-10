/*****************************************************************************
 *
 * abpretty.h: Pretty print abstract syntax to produce inputable Aldor code
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _ABPRETTY_H_
#define _ABPRETTY_H_

# include "axlobs.h"

#define ABPP_UNCLIPPED	(200000L)
#define ABPP_NOINDENT	(-1)

extern String abPretty       		(AbSyn);
extern String abPrettyClippedIn	  	(AbSyn, long clip, int indent);
extern int    abPrettyPrint  		(FILE *, AbSyn);
extern int    abPrettyPrintClippedIn	(FILE *, AbSyn, long clip, int indent);

extern String tfPretty       		(TForm);
extern String tfPrettyClippedIn	  	(TForm, long clip, int indent);
extern int    tfPrettyPrint  		(FILE *, TForm);
extern int    tfPrettyPrintClippedIn	(FILE *, TForm, long clip, int indent);

extern String symePretty       		(Syme);
extern String symePrettyClippedIn  	(Syme, long clip, int indent);

#endif
