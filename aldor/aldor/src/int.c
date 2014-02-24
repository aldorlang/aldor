#include "cport.h"
#include "int.h"


int
longIsInt32(long n)
{
	if (sizeof(n) * BYTE_BITS <= 32) {
		return true;
	}

	IF_LongOver32Bits(
		if (n < -(1L<<31) || n >= (1L<<31))
			return false;
		);
	return true;
}

Bool
aintEqual(AInt i1, AInt i2)
{
	return i1 == i2;
}

