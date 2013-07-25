#ifndef _OSTREAM_H
#define _OSTREAM_H

#include "cport.h"
#include "buffer.h"

typedef struct _OStream *OStream;
typedef void OstWriteCharFn(OStream o,char c);
typedef int OstWriteStringFn(OStream o, const char *str, int n);
typedef void OstCloseFn(OStream o);

typedef struct ostream_ops {
	OstWriteCharFn   *writeCharFn;
	OstWriteStringFn *writeStringFn;
	OstCloseFn       *closeFn;
} *OStreamOps, _OStreamOps;

struct _OStream {
	OStreamOps ops;
	void      *data;
};

extern OStream ostreamNewFrBuffer(Buffer b);
extern OStream ostreamNewFrFile(FILE *);
extern void    ostreamInitFrFile(OStream, FILE *);

extern OStream ostreamNewFrDevNull();
extern void ostreamFree(OStream);

extern int ostreamWrite(OStream o, const char *s, int n);
extern void ostreamWriteChar(OStream o, char c);
extern void ostreamClose(OStream o);

#endif
