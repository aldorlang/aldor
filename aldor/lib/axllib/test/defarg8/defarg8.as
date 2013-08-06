-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

--> testcomp
--> testrun -l axllib

import from SingleInteger
import from String
import from Character

s: String == new(3)
t: String == new(3, fill == char ".")
u: String == new(3, char "?")

print<<"Hello"<<s<<"World"<<t<<"!"<<u<<"!"<<newline

