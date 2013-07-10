-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

--> testcomp
--> testrun -l axllib

Foo: with
	f: (x: Integer, y: Integer == 2) -> Integer
	foo: () -> Integer
== add
	f(x: Integer, y: Integer == 2): Integer == 10 * x + y
	foo(): Integer == f(1, y == 2)

import from Foo

i: Integer == foo()

print<<i<<newline
