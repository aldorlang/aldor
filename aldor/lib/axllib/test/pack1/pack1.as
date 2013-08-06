-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
----> testgen f -Q3 -W floatrep
--> testgen f -Q3
--> testrun -Q3 -l axllib

#include "axllib"
#include "packdefs"

macro {
	DF == DoubleFloat;
}

times: (DF, DF) ->* DF == (x: DF, y: DF) : DF +->* x * y;

extend DF : Packable with {
	RawType:	% -> BDFlo;
}
== add {
	RawType (x: %) : BDFlo == raw x;

	raw (x: %) : Raw % == x::BDFlo;
	box (x: Raw %) : % == x::%;
}

main () : () == {
	import from DF;
	inline from DF;

	print << times(2.1, 3.2) << newline;
}

main();
