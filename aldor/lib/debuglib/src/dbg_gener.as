-----------------------------------------------------------------------------
----
---- gener.as: Extension of Generator(T) with ops for explicit manipulation.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "dbg_defs"

macro GBase == ( ()->Boolean, ()->(), ()->T, ()->SingleInteger);
macro GRep == () -> GBase;


+++ Generator(T) is a type which allows T values to be obtained serially
+++ in a `repeat' or `collect' form.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: generator, step

extend Generator(T: Type): with {

	step!: % -> %;
		++ step!(I) runs the generator function, updating 
		++ its environment.
	empty?:	% -> Boolean;
		++ empty?(I) tests whether there are any values remaining.
		++ This can be tested after calling step!.

	value: % -> T;
		++ value(I) returns the next value in the generator 
		++ and is valid if not empty?(I), and step! has been called
		++ at least once.
	bound: %->SingleInteger;
		++ An upper bound on the number of elements to be generated
	generator: % -> %;
		++ generator(I) is the identity function, allowing 
		++ direct iteration.
	generator: GRep -> %;
		++ generator(F) is a primitive constructor for a generator.
	generator: GBase-> %;
		++ creates a generator from its constituent functions

	export from T;
}
== add {
	Rep  ==> GRep;
	-- Defn of z is to confuse the inliner.

	step!(x: %):% == {
		(d, g!, v, b) := rep(x)(); 
		z :()->() == g!;
		z();
		x;
	}
	empty?(x: %):Boolean == {
		(d, g!, v, b) := rep(x)(); 
		z :()->Boolean == d;
		z();
	}
	value(x: %):T == {
		(d, g!, v, b) := rep(x)(); 
		z :()->T == v;
		z();
	}
	bound(x: %):SingleInteger == {
		(d, g!, v, b) := rep(x)(); 
		z :()->SingleInteger == b;
		z();
	}

	generator(g: %): % == g;

	generator(f: GRep): % == per f;
	generator(d: () -> Boolean, g!: () -> (), v: () -> T,
		  b: () -> SingleInteger): % ==
		generator((): GBase +-> (d,g!,v,b));

}

Gen ==> Generator S;

+++ GeneratorOperationsPackage(S) provides a few convenience
+++ routines for generators.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1996
+++ Keywords: generator, map, combine, reduce

GeneratorOperationsPackage(S: Type): with {
	-- This package could be nullary, but since at least
	-- most operations need 1 type, we provide it early on.

	map: 	(S->S, Gen) -> Gen;
	map:	(T: Type, S->T, Gen) -> Generator T;
		++ Maps a function over a generator 

	reduce: ((S, S) -> S, Gen, S) -> S;
	reduce: (T: Type, (S, T) -> T, Gen, T) -> T;
		++ folds an operation over a generator
		++ eg. factorial(n) == reduce(*, i for i in 1..n)

	combine: ((S, S) -> S, Gen, Gen) -> Gen;
	combine: (T: Type, (S, S) -> T, Gen, Gen) -> Generator T;
		++ Combines 2 generators into a single generator
		++ using f.
} == add {
	default g, g2: Gen;

	map(f: S -> S, g): Gen == f x for x in g;
	map(T: Type, f: S->T, g): Generator T == f x for x in g;

	reduce(f: (S, S) -> S, g, s: S): S == {
		for x in g repeat s:=f(x, s);
		s
	}
	reduce(T: Type, f: (S, T) -> T, g, t: T): T == {
		for x in g repeat t := f(x, t);
		t
	}

	combine(f: (S, S) -> S, g, g2): Gen == generate
		for x in g for y in g2 repeat yield f(x, y);

	
	combine(T: Type, f: (S, S) -> T, g, g2): Generator T == generate
		for x in g for y in g2 repeat yield f(x, y);
	
}

	
