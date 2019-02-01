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

Bool
aintPtrEqual(AIntPtr i1, AIntPtr i2)
{
	return i1 == i2;
}

Hash
aintHash(AInt n)
{
	return n;
}

Hash
aintPtrHash(AIntPtr n)
{
	return ptr2aint(n);
}

AInt
aintAbsorbingSum(AInt max, AInt i1, AInt i2)
{
	assert(i1 >= 0);
	assert(i2 >= 0);
	return max - i2 > i1 ? i1 + i2: max;
}
