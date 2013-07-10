/*****************************************************************************
 *
 * strops.c: String manipulation functions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axlgen.h"

String
strAlloc(Length n)
{
	String  s;
	s = (String) stoAlloc((unsigned) OB_String, n + 1);
	s[0] = 0;
	s[n] = 0;
	return s;
}

void
strFree(String s)
{
	stoFree((Pointer) s);
}

String
strResize(String s, Length n)
{
	s = (String) stoResize(s, n+1);
	s[n] = 0;
	return s;
}

Length
strLength(String s)
{
	return strlen(s);
}

Length
strUntabLength(String s, Length tabstop)
{
        int     l, r;

        for (l = 0; *s; s++) {
                if (*s == '\t') {
			r = tabstop - l % tabstop;
			while (r != 0)  r--, l++;
		}
		else
			l++;
        }
	return l;
}

String
strCopy(String s)
{
	Length  n;
	String  t;

	n = strlen(s);
	t = strAlloc(n);
	strcpy(t, s);
	return t;
}

String
strCopyIf(String s)
{
	return s ? strCopy(s) : s;
}

String
strnCopy(String s, Length n)
{
	String  t;

	t = strAlloc(n);
	strncpy(t, s, n); t[n] = 0;
	return t;
}

String
strConcat(String s1, String s2)
{
	Length  n1, n2;
	String  r;
	n1 = strlen(s1);
	n2 = strlen(s2);
	r = strAlloc(n1 + n2);
	strcpy(r, s1);
	strcpy(r+n1, s2);
	return r;
}

String
strlConcat(String s1, ...)
{
	va_list argp;
	Length  rlen;
	String  r, ri, si;

	if (!s1) return "";

	rlen = strlen(s1);

	va_start(argp, s1);
	si = va_arg(argp, String);
	while (si) {
		rlen += strlen(si);
		si = va_arg(argp, String);
	}
	va_end(argp);

	r = ri = strAlloc(rlen);

	strcpy(r, s1);
	ri += strlen(s1);

	va_start(argp, s1);
	si = va_arg(argp, String);
	while (si) {
		strcpy(ri, si);
		ri += strlen(si);
		si = va_arg(argp, String);
	}
	va_end(argp);

	return r;
}

String
strVPrintf(const char *fmt, va_list argp)
{
	Buffer b = bufNew();
	bufVPrintf(b, fmt, argp);
	return bufLiberate(b);
}

String
strPrintf(const char *fmt, ...)
{
	va_list argp;
	String  s;
	int     cc;

	va_start(argp, fmt);
	cc = vxprintf((XPutFun) 0, fmt, argp);
	va_end(argp);

	s = strAlloc(cc);
	va_start(argp, fmt);
	vsprintf(s, fmt, argp);
	va_end(argp);

	return s;
}

/*
 * Determine the largest n such that s1[0..n-1] == s2[0..n-1].
 */
Length
strMatch(String s1, String s2)
{
	Length   n;
	for (n = 0; *s1 && *s2; s1++, s2++, n++)
		if (*s1 != *s2) return n;
	return n;
}

Length
strAMatch(String s1, String s2)
{
	Length   n;
	for (n = 0; *s1 && *s2; s1++, s2++, n++)
		if (tolower(*s1) != tolower(*s2)) return n;
	return n;
}


/*
 * If pre is a prefix of s then return the rest of s. Otherwise 0.
 */
String 
strIsPrefix(String pre, String s)
{
	for (; *pre && *s; pre++, s++)
		if (*pre != *s) break;
	if (*pre == '\0') return s;
	return 0;
}
	
String 
strAIsPrefix(String pre, String s)
{
	for (; *pre && *s; pre++, s++)
		if (tolower(*pre) != tolower(*s)) break;
	if (*pre == '\0') return s;
	return 0;
}
	
/*
 * If suf is a suffix of s then return the suffix ptr of s. Otherwise 0.
 */
String
strIsSuffix(String suf, String s)
{
	String   suf0;
	Length   ns = strlen(s), nsuf = strlen(suf);
	if (ns < nsuf) return 0;
	suf0 = s + (ns - nsuf);
	for (s = suf0; *suf && *s; suf++, s++)
		if (*suf != *s) return 0;
	return suf0;
}

String
strAIsSuffix(String suf, String s)
{
	String   suf0;
	Length   ns = strlen(s), nsuf = strlen(suf);
	if (ns < nsuf) return 0;
	suf0 = s + (ns - nsuf);
	for (s = suf0; *suf && *s; suf++, s++)
		if (tolower(*suf) != tolower(*s)) return 0;
	return suf0;
}
	
Bool
strAEqual(String s1, String s2)
{
	for ( ; *s1 && *s2; s1++, s2++)
		if (tolower(*s1) != tolower(*s2)) return false;

	return *s1 == *s2;	/* True iff both '\0' */
}

Bool
strEqual(String s1, String s2)
{
	return !strcmp(s1, s2);
}


Hash
strHash(register String s)
{
	register Hash   h = 0;
	register int    c;

	while ((c = *s++) != 0) {
		h ^= (h << 8);
		h += (charToAscii(c) + 200041);
		h &= 0x3FFFFFFF;
	}
	return h;
}

Hash
strSmallHash(register String s)
{
	register Hash   h = 0;
	register int    c;

	while ((c = *s++) != 0) {
		h ^= (h << 8);
		h += (charToAscii(c) + 200041);
		h &= 0x5FFFFCB;
	}
	return h;
}

Hash
strAHash(register String s)
{
	register Hash   h = 0;
	register int    c;

	while ((c = *s++) != 0) {
		h ^= (h << 8);
		h += (toupper(charToAscii(c)) + 200041);
		h &= 0x3FFFFFFF;
	}
	return h;
}

String
strUpper(String s)
{
        String t;

	for (t = s; *t; t++) *t = toupper(*t);
	return s;
}

String
strLower(String s)
{
        String t;

	for (t = s; *t; t++) *t = tolower(*t);
	return s;
}

/*
 * Convert a string to Ascii from the native character set.
 *
 * The result may share memory with either the first argument,
 * or with a static buffer internal to this function.
 * As a result, the result should be copied to some more stable
 * storage area by the caller, if necessary.
 */

String
strnToAsciiStatic(String s, Length sz)
{
	static Buffer buf = 0;

	if (!buf) buf = bufNew(); bufNeed(buf, sz);
	return strToAscii(s, bufChars(buf), sz);
}

/*
 * Convert a string from Ascii to the native character set.
 *
 * The result may share memory with either the first argument,
 * or with a static buffer internal to this function.
 * As a result, the result should be copied to some more stable
 * storage area by the caller, if necessary.
 */

String
strnFrAsciiStatic(String s, Length sz)
{
	static Buffer buf = 0;

	if (!buf) buf = bufNew(); bufNeed(buf, sz);
	return strFrAscii(s, bufChars(buf), sz);
}

int
strPrint(FILE *fout, String s, int oq, int cq, int esc, const char *fmt)
{
	int	c, cc = 0;
	
	putc(oq, fout); cc++;

	while ((c = *s++) != 0) {
		if (c == oq || c == cq || c == esc) {
			putc(esc, fout); cc++;
			putc(c,   fout); cc++;
		}
		else if (isprint(c)) {
			putc(c,   fout); cc++;
		}
		else {
			cc += fprintf(fout, fmt, c);
		}
	}

	putc(cq, fout); cc++;

	return cc;
}
