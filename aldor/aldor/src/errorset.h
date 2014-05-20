#ifndef ERROR_H
#define ERROR_H

#include "strops.h"

typedef struct errorSet {
	StringList list;
	StringList alloc;
} *ErrorSet;

extern ErrorSet errorSetNew(void);
extern void 	errorSetFree(ErrorSet);

extern Bool 	  errorSetHasErrors(ErrorSet);
extern StringList errorSetErrors(ErrorSet);

extern void errorSetAdd(ErrorSet, String);
extern Bool errorSetCheck(ErrorSet errors, Bool test, String message);
extern Bool errorSetPrintf(ErrorSet errors, Bool test, String format, ...);

#endif
