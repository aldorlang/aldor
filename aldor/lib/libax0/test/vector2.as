-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"

I ==> Integer;

import from Vector I;

foo0 (x: I) : I ==
	reduce(+, [i for i in 1..x], 0);

foo1 (x: I) : Vector I ==
	[i for i in 1..x];

foo2 (x: I) : I == {
	sum: I := 0;
	for i in 1..x repeat
		sum := sum + x;
	sum;
}
