-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen l
--> testgen c
--> testrun -l axllib
#pile

#include "axllib.as"

macro
	I == SingleInteger
	F == SingleFloat
	U == Union(first : I, second : F)

import from Boolean
import from I
import from F
import from U

x : U := [ 3 ]
y : U := union( 2.0 )

print << (x  = y) << newline
print << (x  = x) << newline
print << (x ~= y) << newline
print << (x ~= x) << newline
print << x << newline
print << y << newline
