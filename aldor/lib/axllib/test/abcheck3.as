-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
--> testphase abcheck
#pile

-- This file test syntax errors that are caught by the abcheck phase
-- for the left-hand-side of assignments and definitions.

#include "axllib.as"
import from
    Integer
    DoubleFloat
    String


#if TestErrorsToo
2 := 3
2 == 3

2.4 := 6
2.4 == 6

"abc" := "xyz"
"abc" == "xyz"
#endif
