-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

local f: (Integer, Boolean) -> Integer

f(i: Integer, b:  Boolean): Integer ==
	local z:  Boolean
	local z: Integer

	local left: (Integer, Boolean) -> Integer
	local left: (Boolean, Integer) -> Boolean
	local left: (Boolean, Boolean) -> Boolean

	z: Boolean == false
	z: Integer == 0

	left(j: Integer, d: Boolean): Integer == j
	left(d: Boolean, j: Integer): Boolean == d
	left(d1:Boolean, d2:Boolean): Boolean == d1

	b := left(z, b)
	i := left(z, b)
	b := left(left(z, b), b)
	i := left(left(z, b), b)
