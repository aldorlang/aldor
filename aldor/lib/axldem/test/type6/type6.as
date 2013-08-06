-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axldem -l axllib

#include "axllib.as"
#library DemoLib "axldem"
#pile

import from DemoLib
import from SingleInteger
import from Integer

Z(n: SingleInteger): Ring == SmallPrimeField n

l: List Ring :=
  cons(Integer, cons(DoubleFloat, [Z n for n: SingleInteger in 2..11]))

f(R: Ring): Ring ==
     import from R
     import from String
     four:    R := 1+1+1+1
     sixteen: R := four^2 
     print<<"four    = "<<four<<newline
     print<<"sixteen = "<<sixteen<<newline
     R

map(f, l)
