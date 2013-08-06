-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs -M no-emax
#pile

-- This file tests when 'for' variables are later assigned or defined

#include "axllib.as"

import from
    Integer
    Segment Integer
    TextWriter

#if TestErrorsToo

for i : Integer in 1..10 for j in 2..8 repeat
    print<<i<<newline
    i := i + 1
    j == 9
    print<<i<<newline

[(z := 12) for z in 1..10]
[(z == 12) for z in 1..10]

#endif
