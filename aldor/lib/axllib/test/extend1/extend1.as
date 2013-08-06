-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile

#include "axllib"

PyramidCat: Category == with
	>>: (%, %) -> Boolean

Pyramid: PyramidCat
== add
	Rep ==> Integer
	import from Rep
	(a: %) >> (b: %): Boolean == rep a > rep b

f() : () ==
	extend Pyramid: with
		>>: (%, %) -> Boolean
		<<: (%, %) -> Boolean
	== Pyramid add
		Rep ==> Integer
		import from Rep
		(a: %) << (b: %): Boolean == b >> a

	import from Integer
	import from Pyramid

	a: Pyramid == 5 pretend Pyramid
	b: Pyramid == 7 pretend Pyramid

	c1: Boolean == a >> b
	c2: Boolean == a << b

	if c1 then print<<"a >> b"<<newline else print<<"not a >> b"<<newline
	if c2 then print<<"a << b"<<newline else print<<"not a << b"<<newline

f()
