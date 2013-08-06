-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib.as"

import from Integer

-- Multiple value returns and functional composition
-- This version uses an explicit Cross.
-- Each call to cycle will allocate additional storage.
macro
	I      == Integer
	III    == Cross(I,I,I)
	MapIII == Cross(I,I,I) -> Cross(I,I,I)

id: MapIII ==
	(ijk: III): III +-> ijk

(f: MapIII) *  (g: MapIII): MapIII ==
	(ijk: III): III +-> f g ijk

(f: MapIII) ^ (p: Integer): MapIII ==
	p < 1  => id
	p = 1  => f
	odd? p => f*(f*f)^(p quo 2)
	(f*f)^(p quo 2)

cycle(abc: III): III ==
	(a: I, b: I, c: I) := abc
	(c, a, b)

printIII(a: I, b: I, c: I): () ==
	import from String
	print<<"a = "<<a<<" b = "<<b<<" c = "<<c<<newline

local d: III
d := (1,2,3)
printIII(d)
d := cycle(1,2,3)
printIII(d)
d := cycle cycle  (1,2,3)
printIII(d)
d := (cycle*cycle)(1,2,3)
printIII(d)
d := (cycle^10)  (1,2,3)
printIII(d)
