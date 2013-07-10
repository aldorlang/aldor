-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Original author: Ralf Hemmecke, University of Leipzig

--> testrun -laxllib -Q3

#include "axllib"

Pkg(R:BasicType): with {
  f: R -> R;
  if R has with {+:(%,%)->%} then {g:R->R}
} == add {
  f(x:R):R == x;
  if R has with {+:(%,%)->%} then {g(x:R):R == x+x}
}

MAIN():() == {
  import from Integer,Pkg(Integer);
  print << f(1) << newline;
  print << g(1) << newline;
  
}
MAIN();

