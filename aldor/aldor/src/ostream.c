
#include "ostream.h"
#include "axlgen.h"

local void ostreamStdoutWriteChar(OStream os, char c);
local int ostreamStdoutWriteString(OStream os, const char *s, int n);
local void ostreamStdoutClose(OStream os);

_OStreamOps ostreamStdoutOps = { ostreamStdoutWriteChar, ostreamStdoutWriteString, ostreamStdoutClose };

OStream 
ostreamNewFrStdout()
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamStdoutOps;
}

void
ostreamFree(OStream s)
{
	stoFree(s);
}


local void 
ostreamStdoutWriteChar(OStream os, char c)
{
	fputc(c, stdout);
}

local int
ostreamStdoutWriteString(OStream os, const char *s, int n)
{
	if (n == -1)
		fputs(s, stdout);
	else
		fwrite(s, 1, n, stdout);

	return n;
}

local void 
ostreamStdoutClose(OStream os)
{
	fflush(stdout);
}

int
ostreamWrite(OStream s, const char *txt, int n)
{
	return s->ops->writeStringFn(s, txt, n);
}

void
ostreamWriteChar(OStream s, char c)
{
	s->ops->writeCharFn(s, c);
}

void
ostreamClose(OStream s)
{
	s->ops->closeFn(s);
}

local void ostreamNullWriteChar(OStream os, char c);
local int ostreamNullWriteString(OStream os, const char *s, int n);
local void ostreamNullClose(OStream os);

_OStreamOps ostreamDevNullOps = { ostreamNullWriteChar, ostreamNullWriteString, ostreamNullClose };

OStream 
ostreamNewFrDevNull()
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamDevNullOps;
}

local void 
ostreamNullWriteChar(OStream os, char c)
{
}

local int 
ostreamNullWriteString(OStream os, const char *s, int n)
{
	return n == -1 ? strlen(s): n;
}

local void 
ostreamNullClose(OStream os)
{
}

/*
 * :: Buffers
 */

local void ostreamBufferWriteChar(OStream os, char c);
local int ostreamBufferWriteString(OStream os, const char *s, int n);
local void ostreamBufferClose(OStream os);

_OStreamOps ostreamBufferOps = { ostreamBufferWriteChar, ostreamBufferWriteString, ostreamBufferClose };

OStream 
ostreamNewFrBuffer(Buffer b)
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamBufferOps;
	s->data = b;
}

local void 
ostreamBufferWriteChar(OStream os, char c)
{
	Buffer b = (Buffer) os->data;
	bufPutc(b, c);
}

local int
ostreamBufferWriteString(OStream os, const char *s, int n)
{
	Buffer b = (Buffer) os->data;
	if (n == -1) 
		bufPuts(b, s);
	else {
		bufAddn(b, s, n);
		BUF_ADD1(b,char0);
		BUF_BACK1(b);
	}
	return n;
}

local void 
ostreamBufferClose(OStream os)
{
}
