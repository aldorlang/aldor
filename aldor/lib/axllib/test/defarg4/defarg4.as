-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

--> testcomp
--> testrun -l axllib

local f: (Integer, __:Integer == 2, x: Integer, y: Integer == 4) -> Integer

f(a: Integer, b: Integer == 2, x: Integer, y: Integer == 4): Integer ==
	1000 * a + 100 * b + 10 * x + y

i: Integer == f(1, x == 3)

print<<i<<newline
