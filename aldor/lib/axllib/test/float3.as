-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile

-- This file tests printing of float values

#include "axllib.as"

import from Float

f(x : Float): Float ==
    print<<x<<newline
    x

f(3.0)
f(3.)
f(10.0)
f(10.)
f(010000.0)          -- There should be no difference between this and next.
f(010000.)
f(.3)
f(0000.3)
f(.3000)
f(0000.300000)
