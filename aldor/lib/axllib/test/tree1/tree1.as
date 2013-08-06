-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

#pile
#include "axllib.as"
--> testrun -O  -l axllib
--> testcomp -O

-- Tests mutually recursive domains
XList ==> List

D1: BasicType with { 
	iter: (String->(), SingleInteger -> (), %) -> ();
	make: List D2->%;
	make: SingleInteger->%;
} ==  add {
	Rep==> Union(x: SingleInteger, ol: List D2);
	import from D2, List D2;
	import from Rep;

	make(a: List D2): % == per [a];
	make(a: SingleInteger): % == per [a];
	
	iter(f1: String->(), g1: SingleInteger->(), o: %): () == {
		rep(o) case ol => 
			for d in rep(o).ol repeat
				iter(f1, g1, d);
		g1(rep(o).x);
	}

	(a: %) = (b: %): Boolean == false;
	(p: TextWriter) << (o:%): TextWriter == {
		if rep(o) case x then p<<"#<d2: "<<rep(o).x<<">";
		else {
			p <<"#<d1:";
			for y in rep(o).ol repeat
				p<<" "<<y;
			p <<">";
		}
	}
	sample: % == make[];
}

D2: BasicType with { 
	iter: (String->(), SingleInteger -> (), %)-> ();

	make: List D1->%;
	make: String->%;
} ==  add {
	Rep==> Union(x: String, ol: List D1);
	import from D1, List D1;
	import from Rep;

	make(a: List D1): % == per [a];
	make(a: String): % == per [a];

	iter(f1: String->(), g1: SingleInteger->(), o: %): () == {
		rep(o) case ol => 
			for d in rep(o).ol repeat
				iter(f1, g1, d);
		f1(rep(o).x);
	}
	(a: %) = (b: %): Boolean == false;
	(p: TextWriter) << (o:%): TextWriter == {
		if rep(o) case x then p<<"#<d2: "<<rep(o).x<<">";
		else {
			p << "#<d2:";
			for y in rep(o).ol repeat
				p<< " " << y;
			p << ">";
		}
	}
	sample: % == make [];
}

T1(): () == {
	import from D1, D2;
	import from List D1, List D2;
	import from String, SingleInteger;
	print <<sample$D1<<newline;
	print <<sample$D2<<newline;
	x := make [make "hello", make "there", make [make 1] ];
	ps(x: String): () == print <<x<<newline;
	pi(x: SingleInteger): () == print <<x<<newline;
	iter(ps, pi, x);
	print <<x<<newline;
}

T1();
