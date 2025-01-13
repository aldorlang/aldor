#include "foamlib"
#include "assertlib"
#pile
SInt ==> SingleInteger

#library X "x.ao"
import from X

Gen ==> XGenerator S;

XGeneratorOperationsPackage(S: Type): with {
	-- This package could be nullary, but since at least
	-- most operations need 1 type, we provide it early on.

	map: 	(S->S, Gen) -> Gen;
	map:	(T: Type, S->T, Gen) -> XGenerator T;
		++ Maps a function over a generator 

	reduce: ((S, S) -> S, Gen, S) -> S;
	reduce: (T: Type, (S, T) -> T, Gen, T) -> T;
		++ folds an operation over a generator
		++ eg. factorial(n) == reduce(*, i for i in 1..n)

	combine: ((S, S) -> S, Gen, Gen) -> Gen;
	combine: (T: Type, (S, S) -> T, Gen, Gen) -> XGenerator T;
		++ Combines 2 generators into a single generator
		++ using f.
} == add {
	default g, g2: Gen;

	map(f: S -> S, g): Gen == f x xfor x in g;
	map(T: Type, f: S->T, g): XGenerator T == f x xfor x in g;

	reduce(f: (S, S) -> S, g, s: S): S == {
		xfor x in g repeat s:=f(x, s);
		s
	}
	reduce(T: Type, f: (S, T) -> T, g, t: T): T == {
		xfor x in g repeat t := f(x, t);
		t
	}

	combine(f: (S, S) -> S, g, g2): Gen == xgenerate
		xfor x in g xfor y in g2 repeat yield f(x, y);

	
	combine(T: Type, f: (S, S) -> T, g, g2): XGenerator T == xgenerate
		xfor x in g xfor y in g2 repeat yield f(x, y);
	
}

	
