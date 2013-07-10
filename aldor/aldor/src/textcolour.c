/****************************************************************************
 *
 * textcolour.c: text highlighting
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 ****************************************************************************/

#include "textcolour.h"
#include "strops.h"


/* Flag indicating whether colouring is allowed */
local int no_colour = 1;

/* Store foreground and background colours for ANSI terminals */
local ColourANSI ansi_fg[9], ansi_bg[9];

/* Store colour-pair numbers for HP terminals */
local ColourHP hp_cp[9];

/* Store prefix and postfix strings indexed by message class */
local String pre_text[9];
local String post_text[9];


/* Used by ensure_nag_tags() */
#define ALDOR_BUFSIZ 1024
local char buffer[ALDOR_BUFSIZ];


/* Structure and table to simplify tcolParseColourANSI */
struct acnl_struct
{
	String name;
	int length;
	ColourANSI colour;
};

local struct acnl_struct ansi_colours[] = {
	{"red",		3,	Red},
	{"green",	5,	Green},
	{"blue",	4,	Blue},
	{"cyan",	4,	Cyan},
	{"magenta",	7,	Magenta},
	{"yellow",	6,	Yellow},
	{"black",	5,	Black},
	{"white",	5,	White},
	{"default",	7,	NormalColour}
};


/* Parse a message tag */
local CoMsgTag
tcolParseMessageTag(String tag)
{
	if      (!strcmp(tag,"remark"))  return COMSG_REMARK;
	else if (!strcmp(tag,"warning")) return COMSG_WARNING;
	else if (!strcmp(tag,"error"))   return COMSG_ERROR;
	else if (!strcmp(tag,"fatal"))   return COMSG_FATAL;
	else if (!strcmp(tag,"note"))    return COMSG_NOTE;
	else /* Unrecognised tag */      return -1;
}

/* Convert colour names to ANSI colour numbers */
local ColourANSI
tcolParseColourANSI(String s, String *n)
{
	int i;
	/* Deal with the empty string */
	if (!*s) {
		if (n) *n = s;
		return NormalColour;
	}

	/* Digits [0-8] represent colours [-1..7] */
	if (*s >= '0' && *s <= '8') {
		if (n) *n = s + 1;
		return *s - '0' - 1;
	}

	/* Linear search for colour (called at most 5 times) */
	for (i = 0; i < sizeof(ansi_colours)/sizeof(ansi_colours[0]); i++) {
		if (!strncmp(s,ansi_colours[i].name,ansi_colours[i].length)) {
			if (n) *n = s + ansi_colours[i].length;
			return ansi_colours[i].colour;
		}
	}

	/* Invalid colour name or number */
	if (n) *n = 0;
	return NormalColour;
}

/*
 * Local function to ensure that ansi_fg[] and ansi_bg[] are initialised.
 *
 * If neither $ALDOR_ANSI_COLOURS or $ALDOR_ANSI_COLORS are defined then
 * colouring is disabled: tcolPrefix() and tcolPostfix() will always return
 * the empty string. If either of these variables are set but have no value
 * then a default colouring will be defined. 
 *
 * Otherwise all message classes will be uncoloured except for those defined
 * in $ALDOR_ANSI_COLOURS or $ALDOR_ANSI_COLORS. The format of these variables
 * is strict and any deviation will abort the processing. The format consists
 * of zero or more colon (":") separated elements. Each element consists of a
 * tag, an equals symbol ("=") and a value with no whitespace. The tag is one
 * recognised by tcolParseMessageTag() above while the value specifies zero,
 * one or two colours. The first colour, if present, is the foreground colour
 * while the second, if present, is the background colour. Colours may be
 * specified as ANSI colour numbers (digits 0-8 with 0 for "no change") or as
 * names (see the ansi_colours[] array above).
 *
 * An example setting warning messages to green on the default background,
 * errors to red on a cyan background and fatal errors to black on yellow:
 *
 *    :warning=2:error=red6:fatal=blackyellow:
 *
 * Note that colour names and numbers can be freely mixed and extrea colons
 * are ignored (e.g. the first and last ones above).
 */
local void
tcolEnsureColoursANSI(void)
{
	int i;
	char *envstr, *ptr;
	char tag[21], esc[21];
	static int initialised = 0;
	if (initialised) return;
	initialised = 1;

	/* Check for $ALDOR_ANSI_COLOURS and then $ALDOR_ANSI_COLORS */
	envstr = getenv("ALDOR_ANSI_COLOURS");
	if (!envstr) envstr = getenv("ALDOR_ANSI_COLORS");

	/* No colouring if neither environment variable are defined */
	if (!envstr) return;

	/* Enable colouring */
	no_colour = 0;

	/* Ensure foreground and background colours have sensible defaults */
	for (i = 0; i < sizeof(ansi_fg)/sizeof(ansi_fg[0]); i++)
		ansi_fg[i] = ansi_bg[i] = NormalColour;

	/* If the environment variable is empty, use default colouring */
	if (!*envstr) {
		/*
		 * These colours seem to work okay on normal
		 * and reverse video displays.
		 */
		ansi_fg[COMSG_NOTE]    = Cyan;
		ansi_fg[COMSG_REMARK]  = Blue;
		ansi_fg[COMSG_WARNING] = Green;
		ansi_fg[COMSG_ERROR]   = Red;

		/* Highlight serious errors */
		ansi_fg[COMSG_FATAL] = Cyan, ansi_bg[COMSG_FATAL] = Red;
		return;
	}

	/* Parse the value: colon-separated tag=value elements */
	while (*envstr) {
		int class;
		ColourANSI fg, bg = NormalColour;


		/* Skip element separators (:) if there */
		while (*envstr == ':') envstr++;


		/* Read the tag */
		for (i=0, ptr=tag; *envstr && *envstr!='=' && i<20; i++)
			*ptr++ = *envstr++;
		*ptr = 0;


		/* Abort if we reached EOS or tag is too long */
		if (!*envstr || i >= 20) break;


		/* Skip the tag/value separator (=) */
		envstr++;


		/* Read the value */
		for (i=0, ptr=esc; *envstr && *envstr!=':' && i<20; i++)
			*ptr++ = *envstr++;
		*ptr = 0;


		/* Abort if value is too long */
		if (i > 20) break;


		/* Ignore unrecognised tags */
		class = tcolParseMessageTag(tag);
		if (class == -1) continue;


		/* Parse the foreground colour */
		fg = tcolParseColourANSI(esc, &ptr);
		if (!ptr) continue; /* Invalid colour */


		/* Parse the background colour, if given */
		if (ptr != esc) {
			bg = tcolParseColourANSI(ptr, &ptr);
			/* Skip invalid entries or those with garbage after */
			if (!ptr || *ptr) continue;
		}


		/* Supplied colours are valid so use them */
		ansi_fg[class] = fg;
		ansi_bg[class] = bg;
	}
}

/*
 * Local function to ensure that hp_cp[] is initialised.
 *
 * If neither $ALDOR_HP_COLOURS or $ALDOR_HP_COLORS are defined then colouring
 * is disabled: tcolPrefix() and tcolPostfix() will always return the empty
 * string. If either of these variables are set but have no value then a
 * default colouring will be defined. 
 *
 * Otherwise all message classes will be uncoloured except for those defined
 * in $ALDOR_HP_COLOURS or $ALDOR_HP_COLORS. The format of these variables is
 * strict and any deviation will abort the processing. The format consists
 * of zero or more colon (":") separated elements. Each element consists of
 * a tag, an equals symbol ("=") and a value with no whitespace. The tag is
 * one recognised by tcolParseMessageTag() above while the value specifies zero
 * or one colour-pair numbers (0-7) with 0 representing the default pair.
 *
 * An example setting warning messages to green on the default background,
 * errors to red on a default background and fatal errors to colour-pair 7:
 *
 *    :warning=2:error=1:fatal=7:
 *
 * Note that the RGB colours associated with a given colour-pair may vary
 * by terminal type. Some HP terminals allow the RGB values associated with
 * a given colour-pair to be modified via the Ip capability.
 */
local void
tcolEnsureColoursHP(void)
{
	int i;
	char *envstr, *ptr;
	char tag[21];
	static int initialised = 0;
	if (initialised) return;
	initialised = 1;

	/* Check for $ALDOR_HP_COLOURS and then $ALDOR_HP_COLORS */
	envstr = getenv("ALDOR_HP_COLOURS");
	if (!envstr) envstr = getenv("ALDOR_HP_COLORS");

	/* No colouring if neither environment variable are defined */
	if (!envstr) return;

	/* Enable colouring */
	no_colour = 0;

	/* Ensure all colour-pairs have sensible defaults */
	for (i=0; i<sizeof(hp_cp)/sizeof(hp_cp[0]); i++) hp_cp[i] = 7;

	/* If the environment variable is empty, use default colouring */
	if (!*envstr) {
		/* We're pretty limited in our choice of colours */
		hp_cp[COMSG_NOTE]    = CyanOnDefault;
		hp_cp[COMSG_REMARK]  = BlueOnDefault;
		hp_cp[COMSG_WARNING] = GreenOnDefault;
		hp_cp[COMSG_ERROR]   = RedOnDefault;
		hp_cp[COMSG_FATAL]   = DefaultOnYellow;
		return;
	}

	/* Parse the value: colon-separated tag=value elements */
	while (*envstr) {
		int class;
		char cp;

		/* Skip element separators (:) if there */
		while (*envstr == ':') envstr++;

		/* Read the tag */
		for (i=0, ptr=tag; *envstr && *envstr!='=' && i<20; i++)
			*ptr++ = *envstr++;
		*ptr = 0;

		/* Abort if we reached EOS or tag is too long */
		if (!*envstr || i >= 20) break;

		/* Skip the tag/value separator (=) */
		envstr++;

		/* Abort if we reached EOS */
		if (!*envstr) break;

		/* Ignore empty-values */
		if (*envstr == ':') continue;

		/* Read the value (single digit) */
		cp = *envstr++;

		/* Abort if garbage after value */
		if (*envstr && *envstr != ':') break;

		/* Ignore unrecognised tags */
		class = tcolParseMessageTag(tag);
		if (class == -1) continue;

		/* Parse the colour-pair */
		if (cp >= '0' && cp <= '7') hp_cp[class] = cp - '0';
	}
}

/*
 * tcolParseTermValue(s,&e,c) locates the end-of-sequence mark c in the
 * buffer pointed to by s and replaces it with NUL. Escape sequences \\,
 * \E, \n and \t are replaced with \, \033, \n and \t characters respectively.
 * All subsequent characters in the sequence will be shifted left accordingly.
 * In addition, if the end-of-sequence mark c is preceded by \ then it is
 * treated as a normal character. After processing, e will point to the
 * position of last character in s that was scanned (either 0 or c). If the
 * sequence did not contain any escaped characters then *e will be NUL. If
 * the sequence had been terminated by NUL then the return value of this
 * function will be zero. If this function returns non-zero then the text
 * following the end-of-sequence mark c will begin at e+1.
 */
local int
tcolParseTermValue(String src, String *endp, char c)
{
	int result;
	char *dst;
	char *end;

	/* Locate the end of the sequence */
	for (end=dst=src; *end && *end!=c; end++, dst++) {
		/* Process escape sequences */
		if (*end == '\\') {
			if (end[1] != c) {
				switch (end[1]) {
				case '\\':	end++; break;
				case 'E':	*(++end) = '\033'; break;
				case 'n':	*(++end) = '\n'; break;
				case 't':	*(++end) = '\t'; break;
				}
			}
			else
				end++;
		}

		/* Copy the character if we've seen an escape at some point */
		if (end != dst) *dst = *end;
	}

	/* Detect incomplete lines */
	result = *end;

	/* Terminate the sequence */
	*dst = 0;

	/* Tell the caller where we reached and return */
	if (endp) *endp = end;
	return result;
}

/*
 * tcolReadTermTags(pre, post, n) initialises the pre and post arrays of
 * n elements each with the tags to be emitted before and after errors of
 * each class. We use $ALDOR_TERM as an index into the file $ALDOR_TERMINFO
 * and parse the section found. If $ALDOR_TERMINFO is unset or $ALDOR_TERM
 * cannot be found in $ALDOR_TERMINFO then the pre and post arrays will be
 * left unchanged. Any error in the terminal section for $ALDOR_TERM in
 * $ALDOR_TERMINFO will cause the current line to be silently ignored.
 *
 * The $ALDOR_TERMINFO file consists of zero or more sections. Each sections
 * begins with a terminal name enclosed in [] where every character inside
 * [] is significant. The opening [ must be the first character on the line
 * and anything after the closing ] is ignored. The section body consists of
 * one or more elements, one per line up to the next line beginning with [.
 *
 * Each element begins with a tag name which must be recognised by
 * tcolParseMessageTag(), a colon (":"), a sequence of characters, a second
 * colon and a third sequence of characters which may be terminated by a
 * colon or the end of the line. In the character sequences the following
 * character pairs are treated as escapes:
 *
 *       Pair      Meaning
 *       \t        TAB
 *       \n        NEWLINE
 *       \\        \
 *       \:        :
 *       \E        ESC (ASCII 27)
 *
 * The first character sequence is placed in the pre array while the second
 * character sequence is placed in post. The tag name is used to determine
 * which slot in pre/post is updated.
 */
local void
tcolReadTermTags(String *pre, String *post, int n)
{
	FILE *handle;
	int i, patlen, class, cmp = 1;
	char *src, *dst, *end, *nagterm, *naginfo;
	char tag[21], pattern[103];

	/* Check $ALDOR_TERM */
	nagterm = getenv("ALDOR_TERM");
	if (!nagterm) return; /* Set but no value */
	(void)sprintf(pattern,"[%*s]",(int)strlen(nagterm),nagterm);
	patlen = strlen(pattern);

	/* Check $ALDOR_TERMINFO */
	naginfo = getenv("ALDOR_TERMINFO");
	if (!naginfo) return; /* Set but no value */
	handle = fileRdOpen(fnameParseStatic(naginfo));
	if (!handle) return; /* Unreadable */

	/* Scan for the text "[$ALDOR_TERM]" in file $ALDOR_TERMINFO */
	while (fgets(buffer,ALDOR_BUFSIZ,handle) &&
		(cmp = strncmp(buffer,pattern,patlen)))
		/*EMPTY*/ ;
	if (cmp) return; /* EOF and terminal not found */

	/* Enable colouring */
	no_colour = 0;

	/* Parse the terminal section */
	while (fgets(buffer,ALDOR_BUFSIZ,handle) && *buffer != '[') {
		/* Kill any trailing newlines */
		int buflen = strlen(buffer);
		if (buffer[buflen-1] == '\n') buffer[buflen-1] = 0;

		/* Skip leading whitespace */
		for (src=buffer; *src && isspace(*src); src++) /*EMPTY*/ ;
		if (!*src || *src=='#') continue; /* Blank or comment line */

		/* Read the tag */
		for (i=0, dst=tag; *src && *src!=':' && i <20; i++)
			*dst++ = *src++;
		*dst = 0;

		/* Ignore lines if we reached EOS or tag is too long */
		if (!*src || i >= 20) continue;

		/* Skip the separator (:) */
		src++;

		/* Abort if we reached EOS */
		if (!*src) continue;

		/* Ignore unrecognised tags */
		class = tcolParseMessageTag(tag);
		if (class == -1) continue;

		/*
		 * Locate the end of the prefix sequence skipping
		 * unterminated ones
		 */
		if (!tcolParseTermValue(src, &end, ':')) continue;
		pre[class] = strCopy(src);

		/* Locate the end of the postfix sequence */
		(void)tcolParseTermValue(src = ++end,(char **)NULL,':');
		post[class] = strCopy(src);
	}

	/* Close the terminfo file and return */
	(void)fclose(handle);
}

/* Local function to ensure text tags have been initialised */
local void
tcolEnsureColours(void)
{
	int i;
	int nelts = sizeof(pre_text)/sizeof(pre_text[0]);
	String normal_text;
	static int initialised = 0;
	if (initialised) return;
	initialised = 1;

	/*
 	 * Initialise all table entries to the empty string so that later we
 	 * can update only the entries of interest. Note that require pre_text
 	 * and post_text to contain the same number of entries.
 	 */
	for (i=0; i<nelts; i++) pre_text[i] = post_text[i] = "";

	/* Handle specific terminal types */
	if (termIsAldor()) {
		/* Get the user's choice of prefix and postfix strings */
		tcolReadTermTags(pre_text,post_text,nelts);
	}
	else if (termIsANSI()) {
		/* Get the user's choice of colours */
		tcolEnsureColoursANSI();
		if (no_colour) return;

		/* It matters if this strCopy() call fails */
		normal_text = strCopy(txtNormalANSI());

		/* Doesn't matter if any of these strCopy() calls fail */
		for (i=0; i<nelts; i++) {
			String s = txtColourANSI(ansi_fg[i], ansi_bg[i]);
			pre_text[i] = strCopy(s);
		}

		/* Removing colour is easy on most ANSI terminals */
		for (i=0; i<nelts; i++) post_text[i] = normal_text;
	}
	else if (termIsHP()) {
		/* Get the user's choice of colours */
		tcolEnsureColoursHP();
		if (no_colour) return;

		/* It matters if this strCopy() call fails */
		normal_text = strCopy(txtNormalHP());

		/* Doesn't matter if any of these strCopy() calls fail */
		for (i=0; i<nelts; i++)
			pre_text[i] = strCopy(txtColourHP(hp_cp[i]));

		/* Removing colour is also easy on HP terminals */
		for (i=0; i<nelts; i++) post_text[i] = normal_text;
	}
}

/*
 * tcolPrefix(c) returns the text to be emitted before a message
 * of class c.
 */
String
tcolPrefix(CoMsgTag c)
{
	tcolEnsureColours();
	return pre_text[c];
}

/*
 * tcolPostfix(c) returns the text to be emitted after a message
 * of class c.
 */
String
tcolPostfix(CoMsgTag c)
{
	tcolEnsureColours();
	return post_text[c];
}

