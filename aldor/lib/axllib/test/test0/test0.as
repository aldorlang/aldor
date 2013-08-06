-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  Test0     -- Basic imports and overloads
--> testcomp
--> testerrs
--> testrun -l axllib

#include "axllib.as"
#pile

import from Integer

f(n: Integer): Integer == (n + 1) * n

f 1

#if TestErrorsToo
f f
#endif
