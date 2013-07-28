-----------------------------------------------------------------------------
----
---- tuple.as:  Extend Tuple(T) with operations to access the values.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ `Tuple(T)' provides functions for values of type `Tuple T'.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: tuple, homogeneous product

extend Tuple(T: Type): with {
	tuple: (SingleInteger, Pointer) -> %;
		++ tuple(s, p) forms a tuple of length s. 
		++ The tuple values are in the array pointed to by p.

	length: % -> SingleInteger;
		++ length(t) returns the number of components in the tuple t.

	element: (%, SingleInteger) -> T;
		++ element(t, s) extracts the element in position s from 
		++ the tuple t.  The first element is in position 1.

	dispose!: % -> ();
		++ Throw away a tuple.

	export from T;
}
== add {
	Rep ==> Record(size: BSInt, values: BArr);

	import from Rep, Machine;

	tuple(n: SingleInteger, v:Pointer):% ==
		per [n::BSInt, v pretend BArr];

	length(t: %): SingleInteger ==
		rep(t).size::SingleInteger;

	element(t: %, i: SingleInteger): T ==
		get(T)(rep(t).values, prev(i::BSInt));

	dispose!(t: %): () == {
		dispose! rep(t).values;
		dispose! rep(t);
	}
}
