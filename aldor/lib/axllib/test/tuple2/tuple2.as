-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen c
--> testgen l
--> testrun -l axllib
--> testrun -O -l axllib
#pile

#include "axllib.as"

import from Integer

-- Multiple value returns and functional composition
-- This version uses real multiple values.
-- Only the closures returned by * should allocate additional storage.
macro
	I      == Integer
	III    == (I,I,I)
	MapIII == (I,I,I) -> (I,I,I)

id: MapIII ==
	(i:I, j:I, k: I): III +-> (i,j,k)

(f: MapIII) * (g: MapIII): MapIII ==
	(i:I, j:I, k: I): III +-> f g (i,j,k)

(f: MapIII) ^ (p: Integer): MapIII ==
	p < 1  => id
	p = 1  => f
	odd? p => f*(f*f)^(p quo 2)
	(f*f)^(p quo 2)

cycle(a: I, b: I, c: I): III == (c, a, b)

printIII(a: I, b: I, c: I): () ==
	import from String
	print<<"a = "<<a<<" b = "<<b<<" c = "<<c<<newline

foo(): () ==
	printIII (cycle(1,2,3))
	printIII (cycle cycle  (1,2,3))
	printIII ((cycle*cycle)(1,2,3))
	printIII ((cycle^10)   (1,2,3))

-- Tests mutuple values across exits and ifs
bar(a: I, b: I, c: I): III ==
	if (a < 1) then (21,52,31)
	else (a,b,c)

baz(n: I): III ==
	n > 10 => bar(n,n,n)
	n < 5  => (2,1,n)
	baz(n-1)



foo()
printIII (bar(1,2,3))
printIII (bar(0,2,3))

printIII(baz(7))
printIII(baz(20))
