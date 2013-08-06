-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib"

Foo(S: AbelianMonoid): with {
	zero: () -> %;
	<<: (TextWriter, %) -> TextWriter;
}
== add {
	Rep ==> Record(x: S);
	import from Rep;

	zero(): % == per [0];

	(p: TextWriter) << (f: %) : TextWriter == p << rep(f).x;
}

import from Foo Integer;
print<<zero()<<newline;
