-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen c
--> testrun -l axllib

#include "axllib.as"
#pile

import from Integer
import from Ratio Integer
import from List Ratio Integer
import from Array List Ratio Integer
import from Segment Integer

Hilbert(n: Integer): Array List Ratio Integer ==
   array(list(1/(i+j-1) for i in 1..n) for j in 1..n)

p := Hilbert 3

print<<p<<newline
