-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen l
--> testgen c
--> testrun -l axllib
#pile

#include "axllib.as"

macro
	I == SingleInteger
	R == Record(first : I, second : I)

import from Boolean
import from I
import from R

x : R := [ 1, 2 ]
y : R := [ 2, 1 ]
z : R := record( 1, 2 )

print << (x  = y) << newline
print << (x  = z) <<newline
print << (x ~= y) <<newline
print << (x ~= z) <<newline
print << x <<newline
print << y <<newline
print << z <<newline
