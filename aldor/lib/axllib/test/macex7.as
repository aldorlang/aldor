-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile

-- This file tests that errors appearing in macro expanded test indicate that.
-- Also, it test that the pointer is to the real location, not the macro.

#include "axllib.as"

macro f(a,b) == a + b
macro g(a,b) == a +$Integer b
macro b      == 1

f(1$Integer, 1$Integer)   -- bad, after macro expansion

g(1$Integer, 1$Integer)   -- ok
g(1$Integer, 1)           -- bad, after macro expansion

1$Integer + 1$Integer     -- bad, no macro expansion
b +$Integer 1$Integer     -- bad, after macro expansion


macro zero?(zzz) == zzz = 0

Junk:
 with
   _= : (%, %) -> Boolean
   0  : %

  == add
   default x: %

   junk(x):% ==
      zero? x => 0$%
      x
