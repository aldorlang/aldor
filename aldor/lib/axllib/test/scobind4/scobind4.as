-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile
#include "axllib"

import from SingleInteger

for x in 1..4 repeat
	j := x

print<<j
j := 10
print<<j
