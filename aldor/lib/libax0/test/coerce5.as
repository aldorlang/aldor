-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"
import from AxiomLib;

NegatedIntegers() : BasicType with {
	coerce: Integer -> %;
}
== add {
	Rep ==> Integer;
	import from Rep;

	(a : %) = (b : %): Boolean ==
		((a pretend Rep) =$Rep (b pretend Rep));

	coerce(i : Integer): % == (-i) pretend %;
}
