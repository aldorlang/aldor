-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs

#include "axllib.as"
#pile

import from SingleInteger
import from DoubleFloat

#if TestErrorsToo
y := 1
#endif

x := 3
x := 1

for i in 1..10 repeat print <<i + x<<newline
