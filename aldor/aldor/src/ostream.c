
#include "axlgen.h"
#include "ostream.h"
#include "store.h"

local void ostreamFileWriteChar(OStream os, char c);
local int ostreamFileWriteString(OStream os, const char *s, int n);
local void ostreamFileClose(OStream os);

struct ostreamOps ostreamFileOps = {
	ostreamFileWriteChar,
	ostreamFileWriteString,
	ostreamFileClose,
};

OStream 
ostreamNewFrFile(FILE *file)
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamFileOps;
	s->data.obj = file;
	return s;
}

void 
ostreamInitFrFile(OStream os, FILE *file)
{
	os->ops = &ostreamFileOps;
	os->data.obj = file;
}

local void 
ostreamFileWriteChar(OStream os, char c)
{
	FILE *file = (FILE*) os->data.obj;
	fputc(c, file);
}

local int
ostreamFileWriteString(OStream os, const char *s, int n)
{
	FILE *file = (FILE*) os->data.obj;
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

struct ostreamOps ostreamDevNullOps = {
	ostreamNullWriteChar,
	ostreamNullWriteString,
	ostreamNullClose,
};

local OStream ostreamTheDevNull = NULL;

OStream
oStreamDevNull()
{
	if (ostreamTheDevNull == NULL) {
		ostreamTheDevNull = ostreamNewFrDevNull();
	}
	return ostreamTheDevNull;
}

OStream 
ostreamNewFrDevNull(void)
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamDevNullOps;
	return s;
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

struct ostreamOps ostreamBufferOps = {
	ostreamBufferWriteChar,
	ostreamBufferWriteString,
	ostreamBufferClose,
};

OStream 
ostreamNewFrBuffer(Buffer b)
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamBufferOps;
	s->data.obj = b;
	return s;
}

local void 
ostreamBufferWriteChar(OStream os, char c)
{
	Buffer b = (Buffer) os->data.obj;
	bufPutc(b, c);
}

local int
ostreamBufferWriteString(OStream os, const char *s, int n)
{
	Buffer b = (Buffer) os->data.obj;
	if (n == -1) {
		return bufPuts(b, s);
	}
	else {
		bufAddn(b, s, n);
		bufAdd1(b,char0);
		bufBack1(b);
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


/*
 * :: Sizing
 */

TextSizing
textSizingNew(void)
{
	TextSizing sz = (TextSizing) stoAlloc(OB_Other, sizeof(*sz));
	sz->nChars   = 0;
	sz->nLines   = 0;
	sz->cpos     = 0;
	sz->maxWidth = 0;
	return sz;
}

void
textSizingFree(TextSizing txtSz)
{
	stoFree(txtSz);
}


local void ostreamSizingWriteChar(OStream os, char c);
local int ostreamSizingWriteString(OStream os, const char *s, int n);
local void ostreamSizingClose(OStream os);


struct ostreamOps ostreamSizingOps = {
	ostreamSizingWriteChar,
	ostreamSizingWriteString,
	ostreamSizingClose,
};

OStream
ostreamNewFrTextSizing(TextSizing txtSizing)
{
	OStream s = (OStream) stoAlloc(OB_Other, sizeof(*s));
	s->ops = &ostreamSizingOps;
	s->data.obj = txtSizing;
	return s;
}

local void
ostreamSizingWriteChar(OStream os, char c)
{
	TextSizing sz = (TextSizing) os->data.obj;
	sz->nChars++;
	if (c == '\n') {
		sz->nLines++;
		sz->cpos = 0;
	}
	else {
		sz->cpos++;
		if (sz->maxWidth < sz->cpos) {
			sz->maxWidth = sz->cpos;
		}
	}
}

local int
ostreamSizingWriteString(OStream os, const char *s, int n)
{
	int i;
	if (n == -1) {
		i = 0;
		while (s[i] != '\0') {
			ostreamSizingWriteChar(os, s[i]);
			i++;
		}
		return i;
	}
	else {
		for (i=0; i<n; i++) {
			ostreamSizingWriteChar(os, s[i]);
		}
	}
	return n;
}

local void
ostreamSizingClose(OStream os)
{
	TextSizing sz = (TextSizing) os->data.obj;

	if (sz->cpos != 0) {
		sz->nLines++;
	}
}
