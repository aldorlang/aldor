-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib

#include "axllib.as"
#pile

import from Integer
import from String

u: Union(s: String, i: Integer) := union "hello"
print<<u.s<<newline
print<<u.i<<newline

