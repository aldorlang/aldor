/*****************************************************************************
 *
 * strops.c: String manipulation functions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "format.h"
#include "store.h"
#include "strops.h"

CREATE_LIST(String);

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
strLength(CString s)
{
	return strlen(s);
}

Length
strUntabLength(CString s, Length tabstop)
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
strCopy(CString s)
{
	Length  n;
	String  t;

	n = strlen(s);
	t = strAlloc(n);
	strcpy(t, s);
	return t;
}

String
strCopyIf(CString s)
{
	return s ? strCopy(s) : (String) s;
}

String
strnCopy(CString s, Length n)
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
strNConcat(String s1, String s2)
{
	String s3 = strConcat(s1, s2);
	strFree(s1);
	return s3;
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
	Buffer  buf = bufNew();

	va_start(argp, fmt);
	bufVPrintf(buf, fmt, argp);
	va_end(argp);

	return bufLiberate(buf);
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
strIsPrefix(CString pre, CString s)
{
	for (; *pre && *s; pre++, s++)
		if (*pre != *s) break;
	if (*pre == '\0') return (String) s;
	return 0;
}
	
String 
strAIsPrefix(CString pre, CString s)
{
	for (; *pre && *s; pre++, s++)
		if (tolower(*pre) != tolower(*s)) break;
	if (*pre == '\0') return (String) s;
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
		h += (c + 200041);
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
		h += (c + 200041);
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
		h += (toupper(c) + 200041);
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

String
strNReplace(String txt, String orig, String repl)
{
	String s = strReplace(txt, orig, repl);
	strFree(txt);
	return s;
}

String
strReplace(String txt, String orig, String repl)
{
	Buffer buf = bufNew();
	String s;
	int replLen;

	bufNeed(buf, strlen(orig));
	replLen = strlen(repl);
	while (true) {
		char *nxt = strstr(txt, orig);
		if (nxt == NULL)
			break;
		bufAddn(buf, txt, nxt-txt);
		bufAddn(buf, repl, replLen);
		txt = nxt;
		txt += strlen(orig);
	}
	bufAddn(buf, txt, strlen(txt));
	bufAdd1(buf, '\0');
	s = bufLiberate(buf);
	return s;
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

char *
strLastIndexOf(String s, char c)
{
	char *p;
	char *lastP = 0;

	p = s;
	while (*p) {
		if (*p == c)
			lastP = p;
		p++;
	}
	return lastP;
}

void
strSplitLast(String s0, char c, String *plhs, String *prhs)
{
	String s = s0; /* Copy ptr in case of aliasing */
	char *lastC = strLastIndexOf(s, c);
	if (lastC == NULL) {
		*plhs = 0;
		*prhs = s;
		return;
	}
	*plhs = strnCopy(s, lastC - s);
	*prhs = strCopy(lastC+1);
	strFree(s);
}
