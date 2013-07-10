-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
--> testrun -O -l axllib
--> testcomp -O

#include "axllib.as"

macro RZ == Cross(R, Integer);

-- The bug disappears if the following - unused - type is removed!
Bar(R:Monoid, L:ListCategory List R): with { foo: L -> List RZ } == add {
	foo(f:L):List RZ == empty();
}
			
Foo(R:Ring, L:ListCategory R): with { foo: L -> List RZ } == add {
	foo(l:L):List RZ == {
		import from Integer, RZ;
		cons((first l, 1), empty());
	}
}			

main():List Cross(Integer, Integer) == {
	import from Integer, List Integer, Foo(Integer, List Integer);
	print << (x := foo [1]) << newline;
	x
}

main();
