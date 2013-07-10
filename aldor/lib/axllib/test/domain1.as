-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen l
--> testrun -l axllib
#pile

#include "axllib.as"

macro C == Complex DoubleFloat

foo(a: C, b: C): C ==
	(a + 1)/(b - 1)

bar(a: C, b: C): C ==
	(a + 1)/(b + 1)

test(a:DoubleFloat, b:DoubleFloat): () ==
  local z: C
  z := foo(complex(a,b), complex(b,a))
  print<<z<<newline

import from DoubleFloat

test(1.0, -2.3)
