-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib.as"

Tree(X: BasicType): BasicType with {
	addTree: (%, %) -> %;
	addValue:(%, X) -> %;
	node:	     () -> %;
			
	export from X;
} == add {
	U == Union(x: X, r: Rep);
	Rep == List(U);

	import from Rep;
	
	node(): % == per [];
	addTree (l: %, r: %): % == per(cons([rep r], rep l));
	addValue(l: %, v: X): % == per(cons([v], rep l));

	-- basicType operations
	sample: % == per [];
	(p: TextWriter) << (l: %): TextWriter == {
		p << "Thing(";
		for e in rep l repeat 
			if e case x then p << e.x
			else p << (e.r);
		p << ")";
	}

	(a: %) = (b: %): Boolean == false;
}


T1(): () == {
	import from Tree SingleInteger;
	print << sample@Tree(SingleInteger) << newline;
	x := node();
	print << x << newline;
	x := addValue(x, 2);
	print << x << newline;
	y := addValue(node(), 2);
	print << y << newline;
	print << addTree(x, y) << newline;
}

T1();
