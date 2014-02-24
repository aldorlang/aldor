#ifndef _INT_H_
#include "cport.h"

extern Bool longIsInt32(long n);

extern Bool aintEqual(AInt i1, AInt i2);

#ifdef CC_long_not_int32
#define IF_LongOver32Bits(x) Statement(x)
#else
#define IF_LongOver32Bits(x) Nothing
#endif

#endif
