-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib"

Foo : with {
	export from Integer;
}
== add {
}

import from Foo;
print<<2 + 2<<newline;
