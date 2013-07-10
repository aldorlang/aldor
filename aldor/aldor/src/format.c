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

# include "axlgen0.h"

/*
 * fnewline(fout) prints a newline and indents next line by amount findent.
 */
int findent = 0;

int
fnewline(FILE *fout)
{
	int     i;
	fputc('\n', fout);
	for (i = 0; i < findent; i++) fputc(' ', fout);
	return findent + 1; 
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

int
vxprintf(XPutFun putfun, const char *fmt, va_list argp)
{
	int             c, r, w, cc;
	struct fbuf     fb;
	char    	*f, arg_buf[256];

	for (cc = 0; *fmt; ) {

		if (*fmt != '%') {
			int	i;
			for (i = 0; fmt[i] && fmt[i] != '%'; i++) ;
			if (putfun) i = putfun(fmt, i);
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
			if (putfun)
				cc += putfun(s, fb.width);
			else {
				int l = strlen(s), w = fb.width;
				cc += (w != -1 && w < l) ? w  : l;
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
		case 'p': 
			sprintf(arg_buf, f, va_arg(argp, Pointer));
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
		if (putfun)
			cc += putfun(arg_buf, -1);
		else
			cc += strlen(arg_buf);
	}
	return cc;
}
