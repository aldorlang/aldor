/*****************************************************************************
 *
 * parseby.h: Various support functions for the parser.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * The main job is done by axl.y.
 */
#ifndef _PARSEBY_H_
#define _PARSEBY_H_

# include "axlobs.h"

extern AbSyn parse             (TokenList *prest);

extern AbSyn yypval;
extern int   yylex             (void);
extern int   yyerrorfn         (String);

/*
 * Node forming operations which can cause messages go here -- not in absyn.*.
 */
extern AbSyn parseMakeJuxtapose(AbSyn, AbSyn);
extern AbSyn parseDeprecated(TokenTag, AbSyn);

#endif /* !_PARSEBY_H_ */
