-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- tests some pitfalls in bigint code.
--> testint
--> testrun -O -l axllib 

#pile
#include "axllib.as"
import from Integer
print << 144115188075855872- 3002399751580330 << newline
