-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib"
#library ReplLib "repl"
import from ReplLib;

Int ==> SingleInteger;

B: with {
	a: Int;
	b: Int;
	export from A;
}
== add {
	import from A;
	a: Int == x + y;
	b: Int == x - y;
}

import from Int, B;
print<<"a := "<<a<<newline;
print<<"b := "<<b<<newline;
