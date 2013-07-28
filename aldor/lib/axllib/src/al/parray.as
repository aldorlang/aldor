------------------------------------------------------------------------------
----
---- parray.as: Data sturctures for one-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

#include "axllib.as"

SI ==> SingleInteger;

-- PrimitiveArray(S: Type):Join(PrimitiveArrayCategory(S), FortranArray) with {
PrimitiveArray(S: DenseStorageCategory):Join(
	PrimitiveArrayCategory(S),
	DenseStorageCategory,
	FortranArray) with
{
	if (S has FortranFString) then FortranFStringArray;

	coerce: BArr -> %;
		++ Used by convert$FortranArray
} == add
{
	Rep == BArr;

	import from Machine;

	new(size: SI): % ==
		per (PackedArrayNew(size::BSInt)$S);

	apply(a: %, i: SI): S ==
		(PackedArrayGet(rep a, (i-1)::BSInt)$S);

	set!(a: %, i: SI, val: S): S ==
		(PackedArraySet(rep a, (i-1)::BSInt, val)$S);

	-- FortranArray export which is applied automatically
	-- to array-like values that are passed to Fortran.
	convert(x:%):BArr == rep x;

	-- This one is applied after the call giving a pointer
	-- to the array modified by Fortran. There is nothing
	-- that we can do except assume that `a' aliases `x'.
	convert(x:%, a:BArr):BSInt == 0;

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


	-- Export of FortranFStringArray
	if (S has FortranFString) then
		#():SInt == (#()$S);

	-- Explicit exports of DenseStorageCategory to improve inlining.
	import
	{
		ArrNew: (Word, BSInt) -> BArr;
		ArrElt: (BArr, BSInt) -> Rep;
		ArrSet: (BArr, BSInt, Rep) -> Rep;
	} from Builtin;

	PackedArrayNew(n:BSInt):BArr ==
		ArrNew((0@BSInt) pretend Word, n);

	PackedArrayGet(a:BArr, i:BSInt):% ==
		per ArrElt(a, i);

	PackedArraySet(a:BArr, i:BSInt, e:%):% ==
		per ArrSet(a, i, rep e);

	PackedRecordGet(p:BPtr):% ==
		per ArrElt(p pretend BArr, 0);

	PackedRecordSet(p:BPtr, e:%):% ==
		per ArrSet(p pretend BArr, 0, rep e);

	PackedRepSize():BSInt == {
		import {
			TypeWord: () -> BSInt;
			RawRepSize: BSInt -> BSInt;
		} from Builtin;

		RawRepSize(TypeWord());
	}
}
