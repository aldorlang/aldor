-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 
--> testgen l

#include "ax0.as"

#pile

testpack: with (f: SingleInteger -> SingleInteger) == add
  f(s:SingleInteger):SingleInteger ==
    sum:SingleInteger:=0
    for i in 1..s repeat sum := sum+i
    sum


