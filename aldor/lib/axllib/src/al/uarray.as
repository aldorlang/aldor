------------------------------------------------------------------------------
----
---- uarray.as: Data structures for one-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

#include "axllib.as"

macro SI == SingleInteger;

+++ `UniformPrimitiveArray' type. 1-based indexing and no size information.
+++ Elements are stored using their natural Aldor representation and they
+++ must fit into a 32-bit word. This means that UniformPrimitiveArray(DFlo)
+++ may not work correctly.
UniformPrimitiveArray(S: Type): PrimitiveArrayCategory(S) with {
	coerce: BArr -> %;
		++ Used by convert$FortranArray
} == add
{
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

	-- This export is used by convert$FortranArray and
	-- unflatten$FortranMultiArray.
	coerce(x:BArr):% == per x;
}

