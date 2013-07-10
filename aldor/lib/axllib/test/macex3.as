-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase macex
#pile

-- This file tests macro name scoping within function definitions.

macro
        x  == u
        a == a1 - a2
        c == c1
        a + b == c(a,b)

f(x: Integer): Integer ==
  macro a == a2 - a1
  g(y : Integer): Integer ==
    macro i + j == concat(i,j)
    "Number" + stringify(y)             -- concat("Number",stringify(y))
  g(x + a)                              -- g(c1(u,a2-a1))

a                                       -- a1 - a2
