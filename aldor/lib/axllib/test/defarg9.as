-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun  -l axllib
--> testcomp

#include "axllib.as"

macro Z == SingleInteger;

MyType(v:Z == 17):BasicType with { 
				foo: Z -> %;
				bar: (String, y: String == s) -> ();
				s:   String
		 } == Z add {
	macro Rep == Z;
	import from Rep;

	foo(x:Z):% == per x;
	s: String == "Doobry";
	bar(x: String, y: String == s): () == 
		print << x << " " << y << newline;
}

import from Z;

-- this fails to compile
-- the problem does not occur if a non-optional argument is added to MyType.
q:MyType() == foo 5;

-- make sure we use a default...
print."~a~n" (<< q);
bar("hello");

