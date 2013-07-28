------------------------------------------------------------------------------
----
---- farray.as: Data structures for Fortran one-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

-- This file is obsolete: used PackedArray(T) instead

#include "axllib"

ImportArrOps(T) ==> { 
	import {
                ArrNew: (T, BSInt) -> BArr;
                ArrElt: (BArr,  BSInt) -> T;
                ArrSet: (BArr,  BSInt, T) -> T;
        } from Builtin;
}

FSingleArray: with {
	FortranArray;
        new:   SingleInteger -> %;
        new:   (SingleInteger, SingleFloat) -> %;
        apply: (%, SingleInteger) -> SingleFloat;
        set!:  (%, SingleInteger, SingleFloat) -> SingleFloat;
        dispose!: % -> ();
                ++ `dispose! a' indicates that `a' will no longer be used.
} == add {
        Rep ==> BArr;
        import from Machine;
        import from SingleFloat;
	ImportArrOps(BSFlo);

        new(n: SingleInteger): % == per ArrNew(0.0::BSFlo, n::BSInt);

        new(n: SingleInteger, s: SingleFloat): % == {
		a := new n;
		for i in 1..n repeat a.i := s;
		a
	}

        apply(a: %, i: SingleInteger): SingleFloat == coerce ArrElt(rep(a), (i-1)::BSInt);
        set!(a: %, i: SingleInteger, f: SingleFloat): SingleFloat == {
                ArrSet(rep a, (i-1)::BSInt, f::BSFlo);
                f
        }

        dispose!(a: %): () == dispose! rep a;
}

FDoubleArray: with {
	FortranArray;
        new:   SingleInteger -> %;
        new:   (SingleInteger, DoubleFloat) -> %;
        apply: (%, SingleInteger) -> DoubleFloat;
        set!:  (%, SingleInteger, DoubleFloat) -> DoubleFloat;
        dispose!: % -> ();
                ++ `dispose! a' indicates that `a' will no longer be used.
} == add {
        Rep ==> BArr;

        import from Machine;
        import from DoubleFloat;
	ImportArrOps(BDFlo);
	
        new(n: SingleInteger): % == per ArrNew(0.0::BDFlo, n::BSInt);

        new(n: SingleInteger, d: DoubleFloat): % == {
		a := new n;
		for i in 1..n repeat a.i := d;
		a
	}

        apply(a: %, i: SingleInteger): DoubleFloat == coerce ArrElt(rep(a), (i-1)::BSInt);
        set!(a: %, i: SingleInteger, f: DoubleFloat): DoubleFloat == {
                ArrSet(rep a, (i-1)::BSInt, f::BDFlo);
                f
        }

        dispose!(a: %): () == dispose! rep a;
}

FSComplexArray: with {
	FortranArray;
        new:   SingleInteger -> %;
        new:   (SingleInteger, FSComplex) -> %;
        apply: (%, SingleInteger) -> FSComplex;
        set!:  (%, SingleInteger, FSComplex) -> FSComplex;
	dispose!: % -> ();
} == add {
        Rep ==> FSingleArray;
        import from Rep;
        import from FSComplex;

        local realIdx(n: SingleInteger): SingleInteger == 2*n - 1;
        local imagIdx(n: SingleInteger): SingleInteger == 2*n;
        
        new(n: SingleInteger): % == per new(imagIdx n);
        new(n: SingleInteger, c: FSComplex): % == {
                arr: % := per new(imagIdx n);
                for i in 1..n repeat arr.i := c;
                arr;
        }

        apply(arr: %, n: SingleInteger): FSComplex == {
                r := rep(arr).(realIdx n);
                i := rep(arr).(imagIdx n);
                complex(r, i);
        }

        set!(arr: %, n: SingleInteger, c: FSComplex): FSComplex == {
                rep(arr).(realIdx n) := real c;
                rep(arr).(imagIdx n) := imag c;
                c
        }

	dispose!(a: %): () == dispose! rep a;
}

FDComplexArray: with {
	FortranArray;
        new:   SingleInteger -> %;
        new:   (SingleInteger, FDComplex) -> %;
        apply: (%, SingleInteger) -> FDComplex;
        set!:  (%, SingleInteger, FDComplex) -> FDComplex;
	dispose!: % -> ();
} == add {
        Rep ==> FDoubleArray;
        import from Rep;
        import from FDComplex;

        local realIdx(n: SingleInteger): SingleInteger == 2*n - 1;
        local imagIdx(n: SingleInteger): SingleInteger == 2*n;
        
        new(n: SingleInteger): % == per new(imagIdx n);
        new(n: SingleInteger, c: FDComplex): % == {
                arr: % := per new(imagIdx n);
                for i in 1..n repeat arr.i := c;
                arr;
        }

        apply(arr: %, n: SingleInteger): FDComplex == {
                r := rep(arr).(realIdx n);
                i := rep(arr).(imagIdx n);
                complex(r, i);
        }

        set!(arr: %, n: SingleInteger, c: FDComplex): FDComplex == {
                rep(arr).(realIdx n) := real c;
                rep(arr).(imagIdx n) := imag c;
                c
        }

	dispose!(a: %): () == dispose! rep a;
}

FSingleIntegerArray: with {
	FortranArray;
        new:   SingleInteger -> %;
        new:   (SingleInteger, SingleInteger) -> %;
        apply: (%, SingleInteger) -> SingleInteger;
        set!:  (%, SingleInteger, SingleInteger) -> SingleInteger;
        dispose!: % -> ();
                ++ `dispose! a' indicates that `a' will no longer be used.
} == add {
        Rep ==> BArr;
        import from Machine;
	ImportArrOps(BSInt);

        new(n: SingleInteger): % == per ArrNew(0::BSInt, n::BSInt);

        new(n: SingleInteger, v: SingleInteger): % == {
		a := new n;
		for i in 1..n repeat a.i := v;
		a
	}

        apply(a: %, i: SingleInteger): SingleInteger == coerce ArrElt(rep(a), (i-1)::BSInt);
        set!(a: %, i: SingleInteger, v: SingleInteger): SingleInteger == {
                ArrSet(rep a, (i-1)::BSInt, v::BSInt);
                v
        }

        dispose!(a: %): () == dispose! rep a;
}

FHalfIntegerArray: with {
	FortranArray;
        new:   SingleInteger -> %;
        new:   (SingleInteger, HalfInteger) -> %;
        apply: (%, SingleInteger) -> HalfInteger;
        set!:  (%, SingleInteger, HalfInteger) -> HalfInteger;
        dispose!: % -> ();
                ++ `dispose! a' indicates that `a' will no longer be used.
} == add {
        Rep ==> BArr;
        import from Machine;
	import from HalfInteger;
	ImportArrOps(BHInt);

        new(n: SingleInteger): % == per ArrNew(0::BHInt, n::BSInt);

        new(n: SingleInteger, v: HalfInteger): % == {
		a := new n;
		for i in 1..n repeat a.i := v;
		a
	}

        apply(a: %, i: SingleInteger): HalfInteger == coerce ArrElt(rep(a), (i-1)::BSInt);

        set!(a: %, i: SingleInteger, v: HalfInteger): HalfInteger == {
                ArrSet(rep a, (i-1)::BSInt, v::BHInt);
                v
        }

        dispose!(a: %): () == dispose! rep a;
}

FByteIntegerArray: with {
	FortranArray;
        new:   SingleInteger -> %;
        new:   (SingleInteger, Byte) -> %;
        apply: (%, SingleInteger) -> Byte;
        set!:  (%, SingleInteger, Byte) -> Byte;
        dispose!: % -> ();
                ++ `dispose! a' indicates that `a' will no longer be used.
} == add {
        Rep ==> BArr;
        import from Machine;
	import from Byte;
	ImportArrOps(BByte);

        new(n: SingleInteger): % == per ArrNew(0::BByte, n::BSInt);

        new(n: SingleInteger, v: Byte): % == {
		a := new n;
		for i in 1..n repeat a.i := v;
		a
	}

        apply(a: %, i: SingleInteger): Byte == coerce ArrElt(rep(a), (i-1)::BSInt);
        set!(a: %, i: SingleInteger, v: Byte): Byte == {
                ArrSet(rep a, (i-1)::BSInt, v::BByte);
                v
        }

        dispose!(a: %): () == dispose! rep a;
}
