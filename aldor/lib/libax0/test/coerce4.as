-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"
import from AxiomLib;

MySet : SetCategory with {
	coerce: Integer -> %;
}
== add {
	Rep ==> Integer;
	import from Rep;

	sample: % == per 0;
	(x: %) = (y: %): Boolean == rep x = rep y;

	coerce(i: Integer):% == per i;
	coerce(x: %): OutputForm == coerce rep x;
}
