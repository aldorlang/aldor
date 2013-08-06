-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
-- This file tests using loop indices freely.
#pile

#include "axllib.as"

import from Integer
export f: Integer -> Integer


f(x : Integer): Integer ==
    import from Segment Integer
    import from TextWriter

    local i : Integer := 0
    for free i in 1..x repeat
        i
    print<<i<<newline
    x

f(10)
