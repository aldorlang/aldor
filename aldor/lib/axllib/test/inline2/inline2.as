-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O
--> testrun -O -l axllib 

#include "axllib.as"
#pile

test8(n: SingleInteger): SingleInteger ==
	z: SingleInteger := n
	
	f1(q: SingleInteger): SingleInteger ==
		t: SingleInteger := q

		f2(r: SingleInteger): SingleInteger ==
			t * f1(r-1)

		q = 0 => q+1
		f2(q)
	f1(z)

import from SingleInteger

print << test8 0 <<newline
print << test8 4 <<newline

pow3(i: SingleInteger, j: SingleInteger): SingleInteger ==
	import from String
	prod: SingleInteger := 1
	for k: SingleInteger in 1..j for m: SingleInteger in 1..j repeat
		for l: SingleInteger in m..j repeat
			print << l << " "
		print << newline
		prod := i * prod
	prod

print << pow3(3,4) << newline
