-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen f -Q3 -W floatrep

#include "axllib"

macro {
	DF == DoubleFloat;
}

import {
	dbltimes:		(DF, DF) ->* DF;
} from Foreign C;

extend DF : with {
	RawType:	% -> BDFlo;

	raw: % -> BDFlo;
	box: BDFlo -> %;
}
== add {
	RawType (x: %) : BDFlo == raw x;

	raw (y: %) : BDFlo == {
	  y::BDFlo;
	}

	box (z: BDFlo) : % == {
	  z::%;
	}
}

main () : () == {
	import from DF;
	inline from DF;

	dbltimes(2.0, 4.0) = 8.0 => return;
	never
}

main();
