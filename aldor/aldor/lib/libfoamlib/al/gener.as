-----------------------------------------------------------------------------
----
---- gener.as: Extension of Generator(T) with ops for explicit manipulation.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib"

macro GBase == ( ()->Boolean, ()->(), ()->T, ()->SingleInteger);
macro GRep == () -> GBase;

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

	
