-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"
#pile

z: Boolean == false
z: Integer == 0

left(i: Integer, b: Boolean): Integer == i
left(a: Boolean, b :Boolean): Boolean == b
left(b: Boolean, i: Integer): Boolean == b

g(i: Integer, b: Boolean): Integer ==
	b := left(b, z) -- ambiguous
	b := left(i, i)	-- no meaning
	i
