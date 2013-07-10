------------------------------------------------------------------------------
----
---- pkarray.as: Data structures for packed one-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

#include "axllib.as"

macro SI == SingleInteger;

+++ `PackedArray' type. 1-based indexing and no size information. Elements
+++ stored in their natural (aka C) size.
PackedArray(S: DenseStorageCategory): Join(
	PrimitiveArrayCategory(S),
	FortranArray,
	if (S has FortranFString) then FortranFStringArray
) with {
	coerce: BArr -> %;
		++ Used by convert$FortranArray
} == add {
	Rep == BArr;
	import from S, Machine;

	new(size: SI): % ==
		per (PackedArrayNew(size::BSInt)$S);

	apply(a: %, i: SI): S ==
		(PackedArrayGet(rep a, (i-1)::BSInt)$S);

	set!(a: %, i: SI, val: S): S ==
		(PackedArraySet(rep a, (i-1)::BSInt, val)$S);

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


	-- FortranArray export which is applied automatically
	-- to array-like values that are passed to Fortran.
	convert(x:%):BArr == rep x;

	-- This one is applied after the call giving a pointer
	-- to the array modified by Fortran. There is nothing
	-- that we can do except assume that `a' aliases `x'.
	convert(x:%, a:BArr):BSInt == 0;


	-- This export is used by convert$FortranArray and
	-- unflatten$FortranMultiArray.
	coerce(x:BArr):% == per x;


	-- Export of FortranFStringArray
	if (S has FortranFString) then
		#():SInt == (#()$S);
}


