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
y : U := union( 2.71828 )

print << (x case first)  <<newline
print << (x case second) <<newline
print << (y case first ) <<newline
print << (y case second) <<newline
print << x.first       <<newline
print << y.second      <<newline

