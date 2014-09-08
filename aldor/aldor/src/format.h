/*****************************************************************************
 *
 * format.h: Code for prettyprinting.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FORMAT_H_
#define _FORMAT_H_

#include "cport.h"
#include "ostream.h"

extern int  findent;
extern int  fnewline    (FILE *f);

/*
 * xprintf takes a function to put the characters.
 *
 * The XPutFun consumes at most n characters of s and returns the count.
 * If n == -1 no limit is imposed.
 * A null function pointer computes the count without doing output.
 */
typedef int (*XPutFun)  (const char *s, int n);

extern  int  xprintf	(XPutFun f, const char *fmt, ...);
extern  int  vxprintf	(XPutFun f, const char *fmt, va_list argp);

/*
 * a-prefixed printf functions are the same as printf and so on, but
 * go via ostreamVPrintf. This allows fun stuff like '%p' formatting
 * methods.
 */
extern String vaStrPrintf(const char *fmt, va_list argp);
extern String aStrPrintf(const char *fmt, ...);

extern int afprintf(FILE *fout, const char *fmt, ...);
extern int afvprintf(FILE *fout, const char *fmt, va_list argp);
extern int aprintf(const char *fmt, ...);
extern int avprintf(const char *fmt, va_list argp);

extern int ostreamPrintf(OStream ostream, const char *fmt, ...);
extern int ostreamVPrintf(OStream ostream, const char *fmt, va_list argp);

typedef int (*PFormatFn)(OStream stream, Pointer p);
typedef int (*IFormatFn)(OStream stream, int np);

typedef struct format {
	const char *name;
	PFormatFn pfn;
	IFormatFn ifn;
	Bool nullOk;
} *Format;


extern void   fmtRegister(const char *name, PFormatFn fn);
extern void   fmtRegisterI(const char *name, IFormatFn fn);
extern void   fmtRegisterFull(const char *name, PFormatFn fn, Bool nullOk);
extern Format fmtMatch(const char *fmtTxt);
extern void   fmtUnregister(Format format);
extern void   fmtUnregisterAll(void);
#endif
