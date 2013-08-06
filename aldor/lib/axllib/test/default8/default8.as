-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
--> testrun -O -l axllib

#include "axllib.as"
macro Z == SingleInteger;

-- additional notes: The problem is that the creation of the
-- BinaryPowering(%, *, Z) domain occurs before the import of '*'.
-- Fix is to do a topological sort of imports in gf_add (or
-- someplace).

SmallPrimeField(p:Z):FiniteField == SingleIntegerMod p add {
	macro Rep == SingleIntegerMod p;

	import from Rep;

	#:Integer	== p::Integer;
	reduce(a:Z):%	== per(a::Rep);
	inv(a:%):%	== mod_/(1, lift a, p)::%;

	(a:%)^(n:Integer):% == {
#if WorkAround
		local times(x: %, y: %): % == x * y;
#else
		times ==> *;
#endif
		import from BinaryPowering(%, times, Z);
		m:Z := lift(n::%);
		n < 0 => power(1, inv a, m);
		power(1, a, m);
	}
}

import from Z, Integer, SmallPrimeField 11;

x:SmallPrimeField 11 := reduce 2;
print << x^2 << newline;
