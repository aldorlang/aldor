#include "foam_c.h"

extern Ptr quotient();
extern Ptr print();

void bar ()
{
	FiWord	quo;
	FiWord	rem;

	quotient(37, 5, &quo, &rem);

	print(quo);
	print(rem);
}

