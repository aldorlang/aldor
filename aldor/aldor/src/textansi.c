/****************************************************************************
 *
 * textansi.c: text utility functions for ANSI/Tektronix terminals
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 ****************************************************************************/

#include "termtype.h"
#include "textansi.h"

/*
 * Table of escape strings for altering text colour and brightess. The AF
 * and AB escapes must contain precisely one %d. This will be replaced by
 * an integer between 0 and 7. The me and md escapes may not contain a %d.
 * No escape may contain any unescaped fprintf control sequence. All codes
 * ought to be obtained from /etc/termcap or similar and converted into a
 * suitable printf format string.
 *
 * In the ideal world we would use tgetent() but large termcap entries on
 * some systems may over-run internal buffers (of tgetent). In the real
 * world we would scan /etc/termcap for these codes ourselves based on the
 * value of $TERM (yeah, right). Instead we keep a hard-coded database of
 * the terminals that we care about.
 *
 * DO NOT include aixterm in the table below unless you can find an escape
 * sequence that removes the colour attributes. The me capability for these
 * terminals only removes bold, underline etc. Colours can only be restored
 * via AF/AB but since we don't know the original colouring this won't work.
 */
local String ansi_term_escapes[][4] = {
  /* forground	background	normal		extra bright	terminal */
  /* AF		AB		me		md			 */
  {"\033[3%dm",	"\033[4%dm",	"\033[m",	"\033[1m"},	/* xterm */
  {"\033[3%dm",	"\033[4%dm",	"\033[m",	"\033[1m"},	/* linux */
  {"\033[3%dm",	"\033[4%dm",	"\033[0m",	"\033[1m"},	/* iris-ansi */
};

/* Capabilities of interest (index into ansi_term_escapes[]) */
enum ansi_term_capabilities { ANSI_AF_cap, ANSI_AB_cap, ANSI_me_cap, ANSI_md_cap };

/*
 * txtBoldANSI() returns a string of escape codes for enabling the
 * bold terminal text attribute (extra bright). The only way to remove
 * this attribute is to write txtNormalANSI() onto the stream and
 * then set the foreground and background colours again. Note that we
 * assume termType() returns a valid ANSI terminal (not UnknownTerm).
 */
String
txtBoldANSI(void)
{
	return ansi_term_escapes[termType()-FirstANSITerm][ANSI_md_cap];
}


/*
 * txtNormalANSI() returns a string of escape codes for disabling all
 * terminal text attributes currently in force. Note that we assume
 * termType() returns a valid ANSI terminal (not UnknownTerm).
 */
String
txtNormalANSI(void)
{
	return ansi_term_escapes[termType()-FirstANSITerm][ANSI_me_cap];
}

/*
 * txtForegroundANSI(col) returns a string of escape codes for setting
 * the terminal text foreground colour to "col". Note that we assume
 * termType() returns a valid ANSI terminal (not UnknownTerm).
 */
String
txtForegroundANSI(ColourANSI col)
{
	static char result[ANSI_ESCAPE_MAXLEN+1];
	String fmt = ansi_term_escapes[termType()-FirstANSITerm][ANSI_AF_cap];
	(void)sprintf(result, fmt, col);
	return result;
}

/*
 * txtBackgroundANSI(col) returns a string of escape codes for setting
 * the terminal text background colour to "col". Note that we assume
 * termType() returns a valid ANSI terminal (not UnknownTerm).
 */
String
txtBackgroundANSI(ColourANSI col)
{
	static char result[ANSI_ESCAPE_MAXLEN+1];
	String fmt = ansi_term_escapes[termType()-FirstANSITerm][ANSI_AB_cap];
	(void)sprintf(result, fmt, col);
	return result;
}

/*
 * txtColourANSI(fg,bg) returns a string of escape codes for setting the
 * terminal foreground and background colours to "fg" and "bg" respectively.
 * Use NormalColour for "fg" or "bg" values that are to remain unchanged
 * (e.g. to allow just the background colour to be set).
 */
String
txtColourANSI(ColourANSI fg, ColourANSI bg)
{
	static char result[ANSI_ESCAPE_MAXLEN+ANSI_ESCAPE_MAXLEN+1];

	/* We assume that the next two calls return different pointers */
	String set_fg = fg==NormalColour ? "" : txtForegroundANSI(fg);
	String set_bg = bg==NormalColour ? "" : txtBackgroundANSI(bg);
	(void)sprintf(result, "%s%s", set_fg, set_bg);
	return result;
}
