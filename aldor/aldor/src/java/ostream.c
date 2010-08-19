#include "ostream.h"

local void ostreamStdoutWriteChar(OStream os, char c);
local void ostreamStdoutWriteString(OStream os, String s, int n);
local void ostreamStdoutClose(OStream os);

OStream 
ostreamNewFrStdout()
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->writeCharFn   = ostreamStdoutWriteChar;
	s->writeStringFn = ostreamStdoutWriteString;
	s->closeFn       = ostreamStdoutClose;
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
	fflush(stdout);
}

local void 
ostreamStdoutWriteString(OStream os, String s, int n)
{
	if (n == -1)
		fputs(s, stdout);
	else
		fwrite(s, 1, n, stdout);
	fflush(stdout);
}

local void 
ostreamStdoutClose(OStream os)
{
}

void
ostreamWrite(OStream s, char *txt, int n)
{
	s->writeStringFn(s, txt, n);
}

void
ostreamWriteChar(OStream s, char c)
{
	s->writeCharFn(s, c);
}

void
ostreamClose(OStream s)
{
	s->closeFn(s);
}
