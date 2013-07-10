-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

import from Integer
import from String

A: Category == with
	a: % -> %
	b: % -> %
	c: % -> %

	default 
		a(x: %): % == 
			print<<"default 1"<<newline
			c(x)

B: Category == A with
	c: % -> %

	default
		a(x: %): % ==
			print<<"default 2"<<newline
			c(x)
		c(x: %): % ==
			print<<"default 3"<<newline
			3 pretend %

C1: Category == Join(A, B)
C2: Category == Join(B, A)

D1: C1 == add
	Rep ==> Integer
	import from Rep
	export a: % -> %
	b(x: %): % == a(x)

D2: C2 == add
	Rep ==> Integer
	import from Rep
	export a: % -> %
	b(x: %): % == a(x)
	c(x: %): % == per 3


local t1: D1 == 10 pretend D1

b(t1)

local t2: D2 == 10 pretend D2

b(t2)
