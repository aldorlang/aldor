-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
#pile

#include "axllib.as"

Foo: with
	a: Integer -> Integer
   == add
	a(i:Integer):Integer == i

Bar: with
	a: Integer -> Integer
	b: Integer -> Integer
    == add
	a(i:Integer):Integer == 0
	b(i:Integer):Integer == 1

g(i: Integer): Integer ==
	import from Foo
	a i

h(i: Integer): Integer ==
	import from Bar
	a i

#if TestErrorsToo

k(i: Integer): Integer ==
	import from Foo
	import from Bar
	a i
#endif
