-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

-- Tests FOAM program denesting of multiple levels of Lambdas

#include "axllib.as"

export f: Integer -> Integer

f(i: Integer): Integer ==
	z: Integer := 0
	y: Integer := 0

	left(j: Integer, d: Integer): Integer ==
	  free z
	  right(l: Integer, r: Integer): Integer ==
	    r
	  z := right(0, 0)
	  j

	z := left(z, z)
	z

import from Integer
f(0)

