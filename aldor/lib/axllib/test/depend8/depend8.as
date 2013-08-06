-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

--> testrun -l axllib

#include "axllib"


-- The compiler is unable to determine whether or not the domain
-- returned by this function has a constant hash code. Thus if
-- we use it in a dependent type we get a runtime halt. The fix
-- is to define the return value as `R add'.
Foo(R:Ring, r:R):Ring == R;


tester(R:Ring):with
{
   convert: (r:R) -> Foo(R, r);
}
== add
{
   convert(r:R):Foo(R, r) == r pretend Foo(R, r);
}


import from SingleInteger;
import from tester(SingleInteger);

local x:Foo(SingleInteger, 2) := convert(2);


-- Current halts at runtime
print << (x pretend SingleInteger) << newline;

