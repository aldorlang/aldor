-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"

I ==> Integer;

f (v: Vector I, i: I) : Vector I == concat(v, i);
