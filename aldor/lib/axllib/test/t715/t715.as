-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O
--> testrun -l axllib

#include "axllib.as"

f(m: SingleInteger)(n: SingleInteger == 3): SingleInteger == m + n;

foo(): () == {
	import from SingleInteger;
	print << f(3)() << newline;
	print << f(3)(1) << newline;
}
