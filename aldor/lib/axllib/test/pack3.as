-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen f -Q3
--> testrun -Q3 -l axllib

#include "axllib"
#include "packdefs"

macro {
	SI == SingleInteger;
	DF == DoubleFloat;
}

extend SI : GenericType == add;

extend DF : Packable with {
	RawType:	% -> BDFlo;
	double:		% ->* %;
}
== add {
	RawType (x: %) : BDFlo == raw x;

	raw (x: %) : Raw % == x::BDFlo;
	box (x: Raw %) : % == x::%;

	double (x: %) :* % == x + x;
}

--!! This passes type inference just fine, but we cannot generate
--!! the correct foam type for raw values from S.
Array_*(S: Packable) : Join(ArrayCategory S, GenericType) with {
	empty:		SI -> %;
	extend!:	(%, S) -> ();
	find:		(%, S) ->* SI;
	map:		(S ->* S, %) -> %;
}
== Array S add {
	import from S, SI;

	find (v: %, s: S) :* SI == {
		for i in 1..#v repeat
			v.i = s => return i;
		0;
	}

	map (f: S ->* S, v: %) : % == {
		n := #v;
		w := empty n;
		for i in 1..n repeat extend!(w, f apply(v,i));
		w;
	}
}

main () : () == {
	import from SI, DF;
	inline from SI, DF;

	v: Array_* DF == [1.2, 2.3, 3.4];
	print << find(v, apply(v,2)) << newline;
	print << map(double, v) << newline;
}

main();
