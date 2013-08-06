-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase macex
#pile

#include "axllib.as"

macro s == a + b
macro z == s * c
macro 
	c == 1 + 2
	d == 3 + 4
	A == 6

local a, b, h: Integer
a := 3
b := 5 
h := z -- This should be macro expanded
