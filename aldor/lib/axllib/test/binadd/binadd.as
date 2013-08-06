-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile

#include "axllib"

SI ==> SingleInteger

Foo: with 
	foo: (SI, SI) -> %
	a:   % -> SI
	b:   % -> SI
    == add
	Rep ==> Record(a: SI, b: SI)
	import from Rep
	foo(a:SI, b: SI): % == per [a,b]
	a(x: %): SI == rep(x).a
	b(x: %): SI == rep(x).b

Bar: with
	foo: (SI, SI) -> %
	bar: % -> SI
	a:   % -> SI
	b:   % -> SI
    == Foo add
	Rep ==> Foo
	import from Rep
	bar(x: %):SI == a(rep x) + b(rep x)

import from SI
local x: Bar == foo(2,3)
print<<bar(x)<<newline
