/*****************************************************************************
 *
 * format.c: Printf-style formatting with general desination.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This uses sprintf to do the actual formatting into stack allocated buffer.
 */

# include "axlgen.h"
#include "ostream.h"
#include "list.h"
#include "store.h"
/*
 * fnewline(fout) prints a newline and indents next line by amount findent.
 */
int findent = 0;
local int    fmtPPrint(Format format, OStream stream, Pointer ptr);
local int    fmtIPrint(Format format, OStream stream, int n);

int
fnewline(FILE *fout)
{
	int     i;
	fputc('\n', fout);
	for (i = 0; i < findent; i++) fputc(' ', fout);
	return findent + 1; 
}

String
asprintf(const char *fmt, ...)
{
	String str;
	va_list argp;
	va_start(argp, fmt);
	str = asvprintf(fmt, argp);
	va_end(argp);
	return str;
}

String
asvprintf(const char *fmt, va_list argp)
{
	int cc;
	Buffer buf = bufNew();
	OStream ostream = ostreamNewFrBuffer(buf);

	cc = ostreamVPrintf(ostream, fmt, argp);
	ostreamClose(ostream);
	ostreamFree(ostream);

	return bufLiberate(buf);
}

int
afprintf(FILE *fout, const char *fmt, ...)
{
	int cc;
	va_list argp;
	va_start(argp, fmt);
	cc = afvprintf(fout, fmt, argp);
	va_end(argp);
	return cc;
}

int
avprintf(const char *fmt, va_list argp)
{
	afvprintf(stdout, fmt, argp);
}

int
afvprintf(FILE *fout, const char *fmt, va_list argp)
{
	struct _OStream os;
	int cc;

	ostreamInitFrFile(&os, fout);
	cc = ostreamVPrintf(&os, fmt, argp);
	ostreamClose(&os);
	return cc;
}


/*
 * xprintf is like printf but takes a function to actually put the characters.
 *
 * Given a null function pointer, xprintf computes the number of characters
 * without doing output.  
 * The XPutFun consumes a string and returns the count of emitted characters.
 */

int
xprintf(XPutFun putfun, const char *fmt, ...)
{
	int     cc;
	va_list argp;
	va_start(argp, fmt);
	cc = vxprintf(putfun, fmt, argp);
	va_end(argp);
	return cc;
}


struct fbuf {
	int     width, prec;            /* -1 if none */
	char    size,  conv;            /* 0  if none */
	char    fmt[100];	        /* A string containing it. */
	int     len;                    /* Current length of buf. */
};


local void 
xputFunWriteChar(OStream o,char c)
{
	if (o->data)
		((XPutFun) o->data)(&c, 1);
}

local int
xputFunWriteString(OStream o, const char *str, int n)
{
	if (o->data)
		return ((XPutFun) o->data)(str, n);
	else
		return n == -1 ? strlen(str): n;
}

local void 
xputFunClose(OStream o)
{
}

_OStreamOps ostreamXPutFunOps = { xputFunWriteChar, xputFunWriteString, xputFunClose };

int
vxprintf(XPutFun putfun, const char *fmt, va_list argp)
{
	struct _OStream os;
	int cc;

	os.ops  = &ostreamXPutFunOps;
	os.data = (void *) putfun;
	cc = ostreamVPrintf(&os, fmt, argp);

	return cc;
}

int 
ostreamPrintf(OStream ostream, const char *fmt, ...)
{
	va_list argp;
	int	cc;

	va_start(argp, fmt);
	cc = ostreamVPrintf(ostream, fmt, argp);
	va_end(argp);

	return cc;
}

int
ostreamVPrintf(OStream ostream, const char *fmt, va_list argp)
{
	int             c, r, w, cc;
	struct fbuf     fb;
	char    	*f, arg_buf[256];

	for (cc = 0; *fmt; ) {

		if (*fmt != '%') {
			int	i;
			for (i = 0; fmt[i] && fmt[i] != '%'; i++) ;
			if (ostream) i = ostreamWrite(ostream, fmt, i);
			cc  += i;
			fmt += i;
			continue;
		}
		/************************************************************
		 * Parse according to ANSI Standard X3.159-1989:
		 *   %[flags][width][.prec][szmod]conv
		 ************************************************************/
		fb.len = 0;

		/* % */
		fb.fmt[fb.len++] = *fmt++;

		/* Flags */
		while ((c = *fmt)=='-'||c=='+'||c==' '||c=='#'||c=='0')
			fb.fmt[fb.len++] = *fmt++;

		/* Width */
		r = -1;
		if (*fmt == '*') {
			r = va_arg(argp, int);
			if (r < 0) { r = -r; fb.fmt[fb.len++] = '-'; }
			fmt++;
			fb.len += sprintf(fb.fmt + fb.len, "%d", r);
		}
		else if (isdigit(*fmt))
			for (r = 0; isdigit(c = *fmt); fmt++) {
				r = 10*r + c - '0';
				fb.fmt[fb.len++] = c;
			}
		fb.width = r;

		/* Precision */
		if (*fmt == '.')  fb.fmt[fb.len++] = *fmt++;

		r = -1;
		if (*fmt == '*') {
			r = va_arg(argp, int);
			if (r < 0) { r = -r; fb.fmt[fb.len++] = '-'; }
			fmt++;
			fb.len += sprintf(fb.fmt + fb.len, "%d", r);
		}
		else if (isdigit(*fmt))
			for (r = 0; isdigit(c = *fmt); fmt++) {
				r = 10*r + c - '0';
				fb.fmt[fb.len++] = c;
			}
		fb.prec  = r;

		/* Size modifier */
		if ((c = *fmt) == 'h' || c == 'l' || c == 'L')
			fb.fmt[fb.len++] = fb.size = *fmt++;
		else
			fb.size = 0;

		/* Conversion character */
		fb.fmt[fb.len++] = fb.conv = *fmt++;
		fb.fmt[fb.len]   = 0;

		/************************************************************
		 * Perform the formatting on the indicated parameter
		 * and increment the character count.
		 ************************************************************/
		/* n format requires the parameter to be assigned cc */
		if (fb.conv == 'n') {
			switch (fb.size) {
			case 'h': *va_arg(argp, short *) = cc; break;
			case 'l': *va_arg(argp, long  *) = cc; break;
			default:  *va_arg(argp, int   *) = cc; break;
			}
			continue;
		}
		/* s format could point to arbitrarily long string */
		if (fb.conv == 's') {
			char *s = va_arg(argp, char *);
			if (ostream) {
				int l = strlen(s), w = fb.width;
				int pad = w-l > 0 ? w-l : 0;
				while (pad > 0) {
					ostreamWriteChar(ostream, ' ');
					cc++;
					pad--;
				}
				cc += ostreamWrite(ostream, s, -1);
			}
			else {
				int l = strlen(s), w = fb.width;
				cc += (w != -1 && w < l) ? w  : l;
			}
			continue;
		}
		if (fb.conv == 'o'){
			Format format = fmtMatch(fmt);
			if (format == NULL) {
				w  = fb.width + fb.prec + 30;       /* over estimate */
				f = (w < sizeof(arg_buf)) ? fb.fmt : "<too wide to format>";
				sprintf(arg_buf, f, va_arg(argp, int));
				cc += ostreamWrite(ostream, arg_buf, -1);
			}
			else {
				cc += fmtIPrint(format, ostream, va_arg(argp, int));
				fmt += strlen(format->name);
			}
			continue;
		}
		if (fb.conv == 'p') {
			Format format = fmtMatch(fmt);
			if (format == NULL) {
				w  = fb.width + fb.prec + 30;       /* over estimate */
				f = (w < sizeof(arg_buf)) ? fb.fmt : "<too wide to format>";
				sprintf(arg_buf, f, va_arg(argp, Pointer));
				cc += ostreamWrite(ostream, arg_buf, -1);
			}
			else {
				cc += fmtPPrint(format, ostream, va_arg(argp, Pointer));
				fmt += strlen(format->name);
			}
			continue;
		}
		/* Remaining formats are short unless width or prec is given */

		w  = fb.width + fb.prec + 30;       /* over estimate */
		f = (w < sizeof(arg_buf)) ? fb.fmt : "<too wide to format>";

		switch (fb.conv) {
		case '%':
			sprintf(arg_buf, "%s", f);
			break;
		case 'c':
		case 'd': case 'i': case 'u':
		case 'o': case 'x': case 'X':
			if (fb.size == 'l')
				sprintf(arg_buf,f,va_arg(argp, long));
			else 
				sprintf(arg_buf,f,va_arg(argp, int));
			break;
		case 'e': case 'E': case 'f': case 'g': case 'G':
			if (fb.size == 'L') 
				sprintf(arg_buf,f,va_arg(argp, LongDouble));
			else
				sprintf(arg_buf,f,va_arg(argp, double));
			break;
		}
		if (ostream)
			cc += ostreamWrite(ostream, arg_buf, -1);
		else
			cc += strlen(arg_buf);
	}
	return cc;
}

/*
 * :: User defined formats
 */

DECLARE_LIST(Format);
CREATE_LIST(Format);

static FormatList fmtRegisteredFormats = listNil(Format);

void 
fmtRegister(const char *name, PFormatFn fn)
{
	fmtRegisterFull(name, fn, true);
}

void 
fmtRegisterFull(const char *name, PFormatFn fn, Bool nullOk)
{
	Format format = (Format) stoAlloc(OB_Other, sizeof(*format));
	assert(name[0] != '\0');
	format->name = strCopy(name);
	format->pfn = fn;
	format->nullOk = nullOk;
	fmtRegisteredFormats = listCons(Format)(format, fmtRegisteredFormats);
}

void fmtRegisterI(const char *name, IFormatFn ifn)
{
	Format format = (Format) stoAlloc(OB_Other, sizeof(*format));
	assert(name[0] != '\0');
	format->name = strCopy(name);
	format->ifn = ifn;
	format->nullOk = false;
	fmtRegisteredFormats = listCons(Format)(format, fmtRegisteredFormats);
}


Format
fmtMatch(const char *fmtTxt)
{
	size_t longestMatch = 0;
	Format match = NULL;
	FormatList list;

	if (fmtTxt[0] == '\0')
		return NULL;
	
	list = fmtRegisteredFormats;
	while (list != listNil(Format)) {
		Format format = car(list);
		list = cdr(list);
		if (strIsPrefix(format->name, fmtTxt) != NULL && strlen(format->name) > longestMatch) {
			match = format;
			longestMatch = strlen(match->name);
		}
	}
	return match;
	
}

static int 
fmtPPrint(Format format, OStream stream, Pointer ptr)
{
	assert(format != NULL);
	if (!format->nullOk && ptr == NULL) {
		return ostreamPrintf(stream, "(nil)");
	}
	return format->pfn(stream, ptr);
}

static int 
fmtIPrint(Format format, OStream stream, int n)
{
	assert(format != NULL);
	return format->ifn(stream, n);
}
