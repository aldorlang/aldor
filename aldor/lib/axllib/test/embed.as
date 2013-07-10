-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib"
import from Integer;
--> testcomp

f(): () == {
	(3,2);
	0
}

#if 0
h(): Cross(Integer, Integer) == {
	args := (1,2);
	divide args;
}
#endif
-- these are ok
g(): (Integer, Integer) == {
	args := (1,2);
	divide args;
}

x(): (Integer, Integer) == {
	args: Cross(Integer, Integer) := (1,2);
	divide args;
}

import from Segment Integer;

--foo(): Generator Cross(Integer, Integer) == 
--	(x, x+1) for x in 1..;

bar(): Generator Cross(Integer, Integer) == 
	generate for x in 1.. repeat yield (x, x-1);

factish(n: Integer, m: Integer): (Integer, Integer) == {
	if n < 1 then (n, m) else return factish(n-1, n*m);
}
