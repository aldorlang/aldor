#ifndef OSTREAM_H_
#define OSTREAM_H_

#include "buffer.h"
#include "cport.h"

typedef int (*OStreamPutFun) (CString s, int n);

typedef struct ostream *OStream;

typedef void	OstWriteCharFn		(OStream o,char c);
typedef int	OstWriteStringFn	(OStream o, const char *str, int n);
typedef void	OstCloseFn		(OStream o);

typedef struct ostreamOps {
	OstWriteCharFn	 *writeCharFn;
	OstWriteStringFn *writeStringFn;
	OstCloseFn	 *closeFn;
} *OStreamOps;

struct ostream {
	OStreamOps ops;
	union {
		Pointer obj;
		OStreamPutFun fun;
	} data;
};

extern OStream	ostreamNewFrBuffer	(Buffer b);
extern OStream	ostreamNewFrFile	(FILE *);
extern void	ostreamInitFrFile	(OStream, FILE *);

extern OStream	ostreamNewFrDevNull	(void);
extern void	ostreamFree		(OStream);

extern int	ostreamWrite		(OStream o, const char *s, int n);
extern void	ostreamWriteChar	(OStream o, char c);
extern void	ostreamClose		(OStream o);

#endif /* OSTREAM_H_ */
