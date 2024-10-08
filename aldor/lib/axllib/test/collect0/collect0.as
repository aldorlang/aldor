-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint
--> testcomp -O -Q inline-all
--> testrun -O -Q inline-all -l axllib
#pile

#include "axllib.as"

macro SI == SingleInteger

sum(it: Generator SI): SI ==
  s: SI := 0
  for i in it repeat s := s+i
  s

foo(a:SI, b:SI): SI ==
  import from String

  c0  := a
  it := generate
      c := c0
      while c <= b repeat (yield c*c; c := c+1)
  for i in it repeat print<<i<<" "
  print<<newline

  s := sum generator(1..10)
  print<<s<<newline

  c0 := a
  s := sum(generate(c := c0; while c<=b repeat (yield c*c*c; c := c+1)))
  print<<s<<newline

  s := sum(i*i*i for i:SI in 1..10)
  print<<s<<newline

  a

import from SI
foo(4,10)
