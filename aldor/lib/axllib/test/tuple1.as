-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
--> testcomp
--> testrun -l axllib
#pile

#include "axllib.as"

import from SingleInteger

local tt: Tuple SingleInteger
tt := (2,3,4)
print<<length tt<<newline
tt := (2)
print<<length tt<<newline
tt := ()
print<<length tt<<newline

f(t: Tuple SingleInteger): SingleInteger == length t
print<<f(2,3,4)<<newline
print<<f(2)<<newline
print<<f()<<newline

print<<f tt<<newline

#if TestErrorsToo
local y: (3, Integer) -> String
#endif
