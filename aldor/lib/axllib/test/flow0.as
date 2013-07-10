-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen l
--> testgen c
--> testrun -l axllib
#pile

#include "axllib.as"

macro SI == SingleInteger

export pile1: SI -> SI

pile1(i: SI): SI ==
  i < 1 => i
  i+1

export pile2: SI -> SI

pile2(i: SI): SI ==
  1+i
  i < 1 => i
  i+1
  i > 1 => i
  i+1
  i+1+1

export f: SI -> SI

f(i: SI): SI ==

	left(j: SI, d: SI): SI ==
	  free i
	  right(l: SI, r: SI): SI ==
	    i+r+l
	  i := right(j, d)

	i > 1 => i+1
	i := i+1
	i := left(1, 1)
	i > 1 => i
	i+1

g(i: SI): SI ==
  i + (if i < i then 1 else i+i)

k(i:SI): SI ==
  i + (
    i<1 => i+i
    i+2
    i<4 =>
      i+3
      i+4
    i+6
    i+7
  )

import from SI

print << f(1) + g(2) + k(3) << newline
