-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen f -Q3 -W floatrep

#include "foamlib"
#pile
DF ==> XDoubleFloat

import 
	dbltimes:		(DF, DF) ->* DF
from Foreign C

XDoubleFloat: with 
    float: Literal -> %
    coerce: % -> BDFlo
    coerce: BDFlo -> %
    =: (%, %) -> Boolean
== add
    Rep ==> Record(d: BDFlo)
    box(b) ==> per [b];
    import from Rep

    float(l: Literal): % == per convert l pretend BArr
    coerce(x: %): BDFlo == rep(x).d
    coerce(d: BDFlo): % == per [d]
    (=)(a: %, b: %): Boolean == false
    

extend DF : with 
	RawType:	% -> BDFlo

	raw: % -> BDFlo
	box: BDFlo -> %
== add 
	RawType (x: %) : BDFlo == raw x
	import from Machie
	raw (y: %) : BDFlo == 
	  y::BDFlo

	box (z: BDFlo) : % ==
	  z::%

main () : () ==
        import from Assert DF
	import from DF
	inline from DF
	not(4.0 = dbltimes(2.0, 2.0)) => never
	return

main();
