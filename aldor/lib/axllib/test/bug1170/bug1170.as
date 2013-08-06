-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Original author: Manuel Bronstein

--> testrun -Q1 -l axllib
--> testrun -Q3 -l axllib

#include "axllib"

Wrap(R:BasicType):BasicType with {
	wrap: R -> %;
	if R has Monoid then Monoid;
} == add {
	macro Rep == R;
	import from Rep;

	wrap(r:R):% == per r;
	sample:% == wrap sample;
	(r:%) = (s:%):Boolean == rep(r) = rep(s);
	(p:TextWriter) << (r:%):TextWriter == p << rep r;

	if R has Monoid then {
		1:% == per 1;
		(r:%) * (s:%):% == wrap(rep(r) * rep(s));
		(r:%) ^ (n:Integer):% == wrap(rep(r) ^ n);
	}
}

import from String, Wrap String;
print << (wrap "bad bug!") << newline;

