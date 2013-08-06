-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib.as"

macro Z == SingleInteger;

MyType(v:Z == 1):BasicType with { foo: Z -> % } == Z add {
	macro Rep == Z;
	import from Rep;

	foo(x:Z):% == per x;
}

import from Z;

-- this fails to compile
-- the problem does not occur if a non-optional argument is added to MyType.
q:MyType() == foo 5;

