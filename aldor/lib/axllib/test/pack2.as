-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
----> testgen f -Q3 -W floatrep
--> testgen f -Q3
--> testrun -Q3 -l axllib

#include "axllib"
#include "../pack1/packdefs"

macro {
	DF == DoubleFloat;
}

extend DF : Packable with {
	RawType:	% -> BDFlo;

	times:		(%, %) ->* %;
}
== add {
	RawType (x: %) : BDFlo == raw x;

	raw (x: %) : Raw % == x::BDFlo;
	box (x: Raw %) : % == x::%;

	times (x: %, y: %) :* % == x * y;
}

main () : () == {
	import from DF;
	inline from DF;

	print << times(2.1, 3.2) << newline;
}

main();
