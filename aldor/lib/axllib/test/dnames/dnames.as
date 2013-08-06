-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -O -l axllib
--> testcomp

-- Tests domain and category name handling.  -- Output should be the
-- list of domains, bracketed appropriately.
-- Check for Dunno --- this indicates a bug!
#include "axllib"

S(X: Ring)(Y: Ring): with == add;

X(n: SingleInteger): Category == with;

T1(): () == {
	import from DomainName, SingleInteger;
	l: List Type == [Complex Float, Integer, 
			 SingleIntegerMod 37,
			 Ring, FiniteAggregate Integer,
			 Aggregate List Integer,
			 Record(x: Integer),
			 Record(Integer, DoubleFloat),
			 Enumeration(a), 
			 S(DoubleFloat)(Float),
			 X 32
			];
	for x in l repeat print << typeName x << newline;
}


T1();
