-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile
--> testcomp
--> testrun -l axllib

I==> SingleInteger;

Tree(S: BasicType): BasicType with {
	map!: (f: S->S, t: %)->();
	new: S -> %;
	new: List % -> %;
} == add {
	Rep ==> Record(val: S, kids: List %);
	import from Rep, List %;

	val(x: %): S == rep(x).val;
	kids(x: %): List % == rep(x).kids;

	map!(f: S->S, x: %): () == {
		f(val x);
		for k in kids(x) repeat
			map!(f, k);
	}
	
	new(x: S): % == per [x, []];
	new(x: List %): % == per [val first x, rest x];

	(p: TextWriter) << (x: %): TextWriter == p<<"#<Tree: "<<val x<<" "<<kids x<<">";
	(a: %) = (b: %): Boolean == false; --rep(a) = rep(b);
	sample: % == new(sample$S);

}
				
T2(): () == {
	S==>String;
	import from S, List Tree S;
	t: Tree S == new [ new "2", new "3", new [ new "4", new "5"], new "6"];

	print<<t<<newline;
}

T2();


