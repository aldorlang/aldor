#include "foam_c.h"

extern Ptr fact();
extern Ptr print();

void bar ()
{
	print(fact(5));
}

