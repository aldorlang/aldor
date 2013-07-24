/****************************************************************************
 *
 * termtype.h: terminal utility functions
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 ****************************************************************************/

#ifndef _TERMTYPE_H_
#define _TERMTYPE_H_

#include "cport.h"

/*
 * This is a list of all the terminal types that we recognise. If you
 * extend this enumeration you MUST update textansi.c and texthp.c.
 *
 * NOTE that both these files use the enumeration values as indices
 * into their internal tables so keep them in contiguous blocks.
 *
 * Note that the delimiters FirstANSITerm etc must be kept up-to-date.
 *
 * Always use the suffix Term.
 */
enum term_types {
  /* Special values for our own use */
  UnsetTerm=-1,
  UnknownTerm=0,

  /* Used if $ALDOR_TERM is set */
  AldorTerm,

  /* ANSI/Tektronix terminals */
  XTerm,	/* includes derivatives such as kvt, rxvt */
  LinuxTerm,	/* aka console */
  IrixTerm,	/* xwsh (possibly wsh and winterm too) */

  /* HP terminals */
  HPTerm,	/* hpterm */

  /* Delimiters */
  FirstANSITerm=XTerm, LastANSITerm=IrixTerm,
  FirstHPTerm=HPTerm,  LastHPTerm=HPTerm
};
typedef Enum(term_types) TerminalType;

/* Exported functions */
extern TerminalType termType(void);
	/* Return the current terminal type: caches the result */

extern int termIsANSI(void);
	/* Returns non-zero if the terminal is ANSI/Tektronix (e.g. xterm) */

extern int termIsHP(void);
	/* Returns non-zero if the terminal is HP-based (e.g. hpterm) */

extern int termIsAldor(void);
	/* Returns non-zero if the terminal is defined by $ALDOR_TERM */

#endif /* TERMTYPE_H_INCLUDED */
