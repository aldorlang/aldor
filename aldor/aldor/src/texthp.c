/****************************************************************************
 *
 * texthp.c: text utility functions for HP terminals
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 ****************************************************************************/

#include "termtype.h"
#include "texthp.h"

/*
 * Table of escape strings for altering text colour and brightess. The sp
 * escape must contain precisely one %d. This will be replaced by an integer
 * between 0 and 7. The me and md escapes may not contain a %d. No escape
 * may contain any unescaped fprintf control sequence. All codes ought to
 * be obtained from /etc/termcap or similar and converted into a suitable
 * printf format string.
 *
 * In the ideal world we would use tgetent() but large termcap entries on
 * some systems may over-run internal buffers (of tgetent). In the real
 * world we would scan /etc/termcap for these codes ourselves based on the
 * value of $TERM (yeah, right). Instead we keep a hard-coded database of
 * the terminals that we care about.
 *
 * NOTE that the me capability for HP terminals does not (normally) alter
 * the current colour-pair selection but we do here.
 */
local String hp_term_escapes[][4] = {
  /* set-pair	normal			extra bright	terminal */
  /* sp		me			md			 */
  {"\033&v%dS",	"\033&d@\033&v0S",	"\033&dB"},	/* hpterm */
  {"\033[3%dm",	"\033[m",		"\033[1m"},	/* xterm (debug) */
};

/* Capabilities of interest (index into hp_term_escapes[]) */
enum hp_term_capabilities { HP_sp_cap, HP_me_cap, HP_md_cap };

/*
 * txtBoldHP() returns a string of escape codes for enabling the bold
 * terminal text attribute (extra bright). The only way to remove this
 * attribute is to write txtNormalHP() onto the stream. Note that we
 * assume termType() returns a valid HP terminal (not UnknownTerm).
 */
String
txtBoldHP(void)
{
	return hp_term_escapes[termType()-FirstHPTerm][HP_md_cap];
}

/*
 * txtNormalHP() returns a string of escape codes for disabling
 * all terminal text attributes currently in force. It also resets
 * the current colour pair to the default colour pair. Note that we
 * assume termType() returns a valid HP terminal (not UnknownTerm).
 */
String
txtNormalHP(void)
{
	return hp_term_escapes[termType()-FirstHPTerm][HP_me_cap];
}

/*
 * txtColourHP(n) returns a string of escape codes for setting the terminal
 * foreground and background colours to colour-pair "n". Colour-pair 0
 * corresponds to the default foreground/background.
 */
String
txtColourHP(ColourHP cp)
{
	static char result[HP_ESCAPE_MAXLEN+1];
	String fmt = hp_term_escapes[termType()-FirstHPTerm][HP_sp_cap];
	(void)sprintf(result, fmt, cp);
	return result;
}
