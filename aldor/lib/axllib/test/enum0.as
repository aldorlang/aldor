-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen l
--> testgen c
--> testrun -l axllib
#pile

#include "axllib.as"

macro E == Enumeration(first, second, third, fourth, fifth)

import from Boolean
import from E

x : E := third

print<<(x  = third )<<newline
print<<(x  = second)<<newline
print<<(x ~= fourth)<<newline
--print<<x<<newline
