-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

Pyramid: with
	>>: (%,%) -> Boolean
	<<: (%,%) -> Boolean
	^=: (%,%) -> Boolean

	default 
		(a: %) << (b: %): Boolean == b >> a
		(a: %) ^= (b: %): Boolean == (a << b \/ a >> b)
== add
	Rep ==> Integer
	import from Rep
	(a: %) >> (b: %): Boolean == rep a > rep b

import from Pyramid
import from Integer
import from String

a: Pyramid == 5 pretend Pyramid
b: Pyramid == 53 pretend Pyramid

c1: Boolean == a >> b
c2: Boolean == a << b
c3: Boolean == a ^= b

if c1 then print<<"a >> b"<<newline else print<<"not a >> b"<<newline
if c2 then print<<"a << b"<<newline else print<<"not a << b"<<newline
if c3 then print<<"a ^= b"<<newline else print<<"not a ^= b"<<newline
