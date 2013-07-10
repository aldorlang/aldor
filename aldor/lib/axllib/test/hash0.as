-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen l
--> testgen c
--> testrun -l axllib

#include "axllib"
#pile

x : Tuple SingleInteger := (4, 3, 2, 1)

y : SingleInteger := length x

z : SingleInteger := element(x,3)

print<<y<<newline
print<<z<<newline
