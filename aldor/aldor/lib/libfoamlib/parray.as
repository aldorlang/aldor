------------------------------------------------------------------------------
----
---- parray.as: Data structures for one-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

#include "foamlib.as"

macro SI == SingleInteger;

+++ `PrimitiveArray' type. 1-based indexing and no size information.
+++ Elements are stored using their natural Aldor representation and they
+++ must fit into a 32-bit word. This means that PrimitiveArray(DFlo)
+++ may not work correctly.
PrimitiveArray(S: Type): with {
        new: SingleInteger -> %;
                ++ `new(n)' creates a new empty array with space for
                ++ n elements.

        new: (SingleInteger, S) -> %;
                ++ `new(n,s)' creates a new array with n elements,
                ++ each initialized to s.

	dispose!: % -> ();
		++ `dispose! a' indicates that `a' will no longer be used.

        set!: (%, SingleInteger, S) -> S;
                ++ `v.n := s' sets the `n'-th element of `v' to `s'.

        apply: (%, SingleInteger) -> S;
                ++ `v.n' extracts the `n'-th element of `v'.

        resize!: (%, osz: SingleInteger, nsz: SingleInteger) -> %;
                ++ `resize!(v, oldsize, newsize)' returns an array with
                ++ the number of elements equal to newsize and containing
                ++ the first m elements of `v', where `m' is the minimum of
                ++ newsize and oldsize.  The value `v' must no longer be used.

	export from S;
} == add {
	Rep ==> BArr;

	import from Machine;

	new(size: SI): % ==
		per array(S)(0$SI pretend S, size::BSInt);
	apply(a: %, i: SI): S ==
		get(S)(rep a, (i-1)::BSInt);
	set! (a: %, i: SI, val: S): S ==
		set!(S)(rep a, (i-1)::BSInt, val);

        new(size:SI, val:S):% == {
                a := new size;
                for i in 1..size repeat a.i := val;
                a
        }

	dispose!(a: %): () ==
		dispose! rep a;

        resize!(a: %, osz: SI, nsz: SI): % == {
                nsz <= osz => a;
                r := new nsz;
                for i in 1..osz repeat r.i := a.i;
		dispose! rep a;
                r
        }
}

