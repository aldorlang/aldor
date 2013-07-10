-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib.as"
import from Segment Integer

f(): Integer ==
   for i: Integer in 1..3 repeat
      for j: Integer in 1..3 repeat
	 print<<i + j<<newline
   2

f()
