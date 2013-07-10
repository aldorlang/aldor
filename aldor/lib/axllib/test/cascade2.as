-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib"

Foo: with {
	zero: () -> %;
	<<: (TextWriter, %) -> TextWriter;
}
== add {
	Rep ==> Record(x: Integer);
	import from Rep;

	zero(): % == per [0];

	(p: TextWriter) << (f: %) : TextWriter == p << rep(f).x;
}

import from Foo;
print<<zero()<<newline;
