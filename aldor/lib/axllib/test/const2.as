-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Tests the ordering of statements/definitions at top-level of a unit.

--> testrun -laxllib -Q3
--> testrun -laxllib -Q0

#include "axllib.as"

import from Float;

f(a: Float): Float == {a*a};
local pi: Float == 3.14159;
p : Float == f(pi);

print << "pi**2 = " << p << newline;
