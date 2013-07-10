-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun  -l axllib
#pile

#include "axllib"

Pyramid(n: Integer): with
	>>: (%,%) -> Boolean
	<<: (%,%) -> Boolean
	^=: (%,%) -> Boolean

	default 
		(a: %) << (b: %): Boolean == b >> a
		(a: %) ^= (b: %): Boolean == 
			local d1: Boolean == a << b
			local d2: Boolean == a >> b
			d1 \/ d2
== add
	Rep ==> Integer
	import from Rep
	(a: %) >> (b: %): Boolean == rep a > rep b

import from Integer
import from String

a: Pyramid(3) == 5 pretend Pyramid(3)
b: Pyramid(3) == 53 pretend Pyramid(3)

c1: Boolean == a >> b
c2: Boolean == a << b
c3: Boolean == a ^= b

if c1 then print<<"a >> b"<<newline else print<<"not a >> b"<<newline
if c2 then print<<"a << b"<<newline else print<<"not a << b"<<newline
if c3 then print<<"a ^= b"<<newline else print<<"not a ^= b"<<newline
