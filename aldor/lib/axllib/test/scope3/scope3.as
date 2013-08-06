-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen l
--> testgen c
--> testrun -l axllib
#pile

-- Tests FOAM program denesting of multiple levels of Lambdas

#include "axllib.as"

export f: Integer -> Integer
local  g: Integer -> Integer

local h: Integer
h := 1

g(i: Integer): Integer ==
	i + 1 + 1

f(i: Integer): Integer ==
	free h
	z: Integer := 0
	y: Integer := 1+1+1

	left(j: Integer, d: Integer): Integer ==
	  free i
	  right(l: Integer, r: Integer): Integer ==
	    r + z + y + h + i -- + g(h)
	  i := right(j, d)

	h := h +1+1
	z := left(z, z+1) + g(h)
	z+y+i

import from Integer

print <<f(1)<<newline
