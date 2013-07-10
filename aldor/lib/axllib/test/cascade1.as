-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib"

Foo(S: Type) : with {
	x: Integer;
	export from S;
}
== add {
	x: Integer == 2;
}

import from Foo Integer;
print<<2 + 2<<newline;
