-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun  -l axllib
--> testrun -O -l axllib
--> testcomp

#include "axllib.as"
#pile
SI ==> SingleInteger

extend Enumeration(T: Tuple Type): with
    card: SI
    ord: % -> SI
    val: SI -> %
  == add
    Rep ==> BSInt
    card: SI == length T
    ord(e: %): SI == (rep e)::SI + 1
    val(i: SI): % == per((i - 1)::BSInt)

import from SI

test(): () ==
  E == Enumeration(a,b,c)
  import from E
  i: SI == card$E
  e: E == val 1
  j: SI == ord e
  print . "i = ~a, e = ~a, j = ~a~n" . (<< i, << e, << j)

test()

