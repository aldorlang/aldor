-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

--> testcomp
--> testrun -l axllib

local f: (x: Integer, y: Integer == 2) -> Integer

f(x: Integer, y: Integer == 2): Integer == 10 * x + y

i: Integer == f 1

print<<i<<newline
