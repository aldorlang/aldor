-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

--> testrun -O -l axllib
--> testcomp

-- tests generators used as first-class values

GEN ==> Generator X;

GeneratorOps(X: Type): with {
	map: (X -> X, GEN)-> GEN;
	filter: ( X->Boolean, GEN) -> GEN;
	concat: (GEN, GEN) -> GEN;
	combine: ( (X, X) -> X, GEN, GEN) -> GEN
}
== add {
	map(f: X->X, g: GEN): GEN ==
		generate for x in g repeat yield f x;

	filter(f: X->Boolean, g: GEN): GEN == {
		generate
			for x in g repeat
				if f x then yield x;
	}

	concat(g1: GEN, g2: GEN): GEN == {
		generate {
			for x in g1 repeat yield x;
			for x in g2 repeat yield x;
		}
	}
	
	combine(f: (X, X) -> X, g1: GEN, g2: GEN): GEN == {
		generate for x in g1
			 for y in g2 repeat 
				yield f(x, y);
	}

}

--- candidate for Daftest Program for Printing Prime Numbers Award

I    ==> SingleInteger
IGEN ==> Generator I
import from GeneratorOps I
import from Generator I

numFilter(n: I, G: IGEN): IGEN == filter( (m: I): Boolean +-> not zero? (m mod n), G);

sieve(G: IGEN): IGEN == {
	step! G;
	empty? G => { generate { } };
	next := value G;	
	concat (generate yield next,
		i for i in sieve numFilter(next, G))
}

T1(): () == for n in sieve generator(2..200) repeat
		print<<n<<newline;

T1();
