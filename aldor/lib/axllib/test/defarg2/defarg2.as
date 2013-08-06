-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

--> testcomp
--> testrun -l axllib

f(i: Integer, r: Record(a: Integer, b: Integer == 3) == [2]): Integer ==
	ra: Integer == apply(r, a)
	rb: Integer == apply(r, b)

	return 100 * i + 10 * ra + rb

i: Integer == f 1

print<<i<<newline
