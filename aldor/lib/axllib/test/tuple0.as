-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
--> testcomp
--> testrun -l axllib
#pile

#include "axllib.as"

import from Integer

-- Multiple value assignment and parameters
foo():() ==
	local nm: Cross(Integer, Integer) := (4,5)
	local n, m: Integer
	(n, m) := (3,4)
	print<<n<<newline<<m<<newline
	(n, m) := (m,n)
	print<<n<<newline<<m<<newline
	(n, m) := nm
	print<<n<<newline<<m<<newline

--+ nm

-- Overload resolution
import from SingleInteger, Boolean, SingleFloat, DoubleFloat

x:  Cross(SingleFloat, Boolean, Integer) := (0, true, 1)

#if TestErrorsToo
(3,4,5,6)
#endif

foo()

import from Tuple SingleInteger
print<<element((42,2), 1)<<newline

f(t: Tuple SingleInteger, i: SingleInteger): SingleInteger ==
	element(t, i)

print<<f((71, 12), 2)<<newline

	


