-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

Pyramid(R: Ring): with
	double: R -> R
	foo: (R, R) -> %

	default double(r: R): R == r + r
== add
	Rep ==> R
	import from Rep
	export double: R -> R
	foo(a: R, b: R): % == per(a + double(b))

a: Integer == 38
b: Integer == 2
c: Pyramid Integer == foo(a,b)

print<<c pretend Integer<<newline
