-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"

--> testcomp
--> testrun -l axllib
--> testrun -O -l axllib

-- checks that enumeration types can be first,
-- and that empty enumerations are OK.

ZZ==>Enumeration(a,b,c);
YY==>Enumeration();

import from Integer;

Foo(): with {
	aa: () -> ZZ;
} == add {
	aa(): ZZ == a;
}

AA(X: Type, x: X): with {
	z: ()->X
} == add {
	z(): X == x;
}

T1():() == {
	import from Foo();
	import from AA(ZZ, aa());
	import from ZZ;
	print << aa() << newline;
	print << z() << newline;
	z()
}

T1();

T2(): () == {
	import from Foo();
	import from List YY;
	return cons(1 pretend YY, nil);
}

