-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"

NNI ==> NonNegativeInteger;
I ==> Integer;

import from I, NNI;

max(v: Vector I): I == {
	(#v)@NNI = 0 => 0;
	x := v.(1@I);
	for y in v repeat if y > x then x := y;
	x
}

import from Vector I;

r1 := max [1, 2, 3, 4, 5];
r2 := max [5, 4, 3, 2, 1];
r3 := max [];
r4 := max [2, 3, 1, 5, 4];
