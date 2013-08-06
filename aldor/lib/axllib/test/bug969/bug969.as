-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Original author: Manuel Bronstein

--> testrun -Q1 -l axllib
--> testrun -Q3 -l axllib


#include "axllib.as"

Bar: Category == with { bar: SingleInteger };

Foo(R:Type): with {
	foo: R -> %;
	if R has Bar then Bar;
} == add {
	macro Rep == R;
	import from Rep;

	foo(a:R):% == per a;
	if R has Bar then bar:SingleInteger == 1;
}		

import from SingleInteger, Foo SingleInteger;

print << ((foo 1) pretend SingleInteger) << newline;


