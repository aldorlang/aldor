
#include "ostream.h"
#include "axlgen.h"

local void ostreamFileWriteChar(OStream os, char c);
local int ostreamFileWriteString(OStream os, const char *s, int n);
local void ostreamFileClose(OStream os);

_OStreamOps ostreamFileOps = { ostreamFileWriteChar, ostreamFileWriteString, ostreamFileClose };

OStream 
ostreamNewFrFile(FILE *file)
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamFileOps;
	s->data = file;
}

void 
ostreamInitFrFile(OStream os, FILE *file)
{
	os->ops = &ostreamFileOps;
	os->data = file;
}

local void 
ostreamFileWriteChar(OStream os, char c)
{
	FILE *file = (FILE*) os->data;
	fputc(c, file);
}

local int
ostreamFileWriteString(OStream os, const char *s, int n)
{
	FILE *file = (FILE*) os->data;
	if (n == -1) {
		fputs(s, file);
		n = strlen(s);
	}
	else
		fwrite(s, 1, n, file);

	return n;
}

local void 
ostreamFileClose(OStream os)
{
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
	if (n == -1) {
		return bufPuts(b, s);
	}
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

void
ostreamFree(OStream s)
{
	stoFree(s);
}


