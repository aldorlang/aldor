#ifndef _OSTREAM_H
#define _OSTREAM_H
#include "axlobs.h"

typedef struct ostream *OStream;
typedef void OstWriteCharFn(OStream o,char c);
typedef void OstWriteStringFn(OStream o, char *str, int n);
typedef void OstCloseFn(OStream o);

struct ostream {
	OstWriteCharFn   *writeCharFn;
	OstWriteStringFn *writeStringFn;
	OstCloseFn       *closeFn;
};

extern OStream ostreamNewFrBuffer(Buffer b);
extern OStream ostreamNewFrFile(FILE *, Bool mustClose);
extern OStream ostreamNewFrStdout();
extern void ostreamWrite(OStream o, char *s, int n);
extern void ostreamWriteChar(OStream o, char c);
extern void ostreamClose(OStream o);

#endif
