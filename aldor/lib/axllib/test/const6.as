-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Tests the ordering of statements/definitions at the top-level

--> testerrs

#include "axllib.as"

import from Float;

-- The use of `f' before definition its is okay because it is
-- a map and therefore lazy. However, the use of `pi' is not
-- okay because it is a non-lazy constant that has not been
-- defined yet and we aren't in an `add'.
p : Float == f(pi);

pi: Float == 3.14159;
f(a: Float): Float == {a*a};

print << "pi**2 = " << p << newline;
