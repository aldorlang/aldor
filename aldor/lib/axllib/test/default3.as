-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

Illuminatus!(R: Ring):Category == with
	double: R -> R
	foo: (R, R) -> %

	default double(r: R): R == r + r

Pyramid(S: Ring): Illuminatus!(S) == add
	Rep ==> S
	import from Rep
	foo(a: S, b: S): % == per(a + double(b))

a: Integer == 38
b: Integer == 2
c: Pyramid Integer == foo(a,b)

print<<c pretend Integer<<newline

