#ifndef _INT_H_
#include "cport.h"

typedef void *AIntPtr;
#define aint2ptr(n) ((AIntPtr) n)
#define ptr2aint(p) ((AInt) p)

extern Bool longIsInt32(long n);

extern Bool aintEqual(AInt i1, AInt i2);
extern Hash aintHash(AInt i1);

extern Bool aintPtrEqual(AIntPtr i1, AIntPtr i2);
extern Hash aintPtrHash(AIntPtr i1);

extern AInt aintAbsorbingSum(AInt, AInt, AInt);

#ifdef CC_long_not_int32
#define IF_LongOver32Bits(x) Statement(x)
#else
#define IF_LongOver32Bits(x) Nothing
#endif

#endif
