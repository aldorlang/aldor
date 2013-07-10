/****************************************************************************
 *
 * termtype.c: terminal utility functions
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 ****************************************************************************/

#include "termtype.h"

/* Return the current terminal type: caches the result */
TerminalType
termType(void)
{
	static TerminalType ttype = UnsetTerm;

	/* Make sure that we have the terminal type */
	if (ttype==UnsetTerm && getenv("ALDOR_TERM"))
		ttype = AldorTerm;
	else if (ttype==UnsetTerm)
	{
		char *term = getenv("TERM");
		if (!term) term = "unknown";

		/* Convert terminal name into an integer value */
		if (!strncmp(term,"xterm",5))
			ttype = XTerm;
		else if (!strcmp(term,"linux"))
			ttype = LinuxTerm;
		else if (!strncmp(term,"iris-ansi",9))
			ttype = IrixTerm;
		else if (!strncmp(term,"iris-color",10))
			ttype = IrixTerm;
		else if (!strcmp(term,"hpterm"))
			ttype = HPTerm;
		else
			ttype = UnknownTerm;
	}
	return ttype;
}

/* Returns non-zero if the terminal is ANSI/Tektronix (e.g. xterm) */
int
termIsANSI(void)
{
	return termType()>=FirstANSITerm && termType()<=LastANSITerm;
}

/* Returns non-zero if the terminal is HP-based (e.g. hpterm) */
int
termIsHP(void)
{
	return termType()>=FirstHPTerm && termType()<=LastHPTerm;
}

/* Returns non-zero if the terminal is defined by $ALDOR_TERM */
int
termIsAldor(void)
{
	return termType()==AldorTerm;
}
