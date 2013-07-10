------------------------------------------------------------------------------
----
---- fmdarray.as: Data structures for Fortran multi-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

#include "axllib"

FortranMultiDimensionalArrayData: with {
} == add {
}

FortranArrayCategory(T) ==> with {
        new: SingleInteger -> %;
        new:   (SingleInteger, T) -> %;
        apply: (%, SingleInteger) -> T;
        set!:  (%, SingleInteger, T) -> T;
        dispose!: % -> ();
                ++ `dispose! a' indicates that `a' will no longer be used.
}
        
FMultiDimensionalArray(T: BasicType, A: FortranArrayCategory T): with {
	FortranArray;
        new: Array SingleInteger -> %;
        apply: (%, Array SingleInteger) -> T;
        data: % -> FortranMultiDimensionalArrayData;
        set!: (%, Array SingleInteger, T) -> T;
        export from Array SingleInteger;
} == add {
        Rep ==> Record(sz: Array SingleInteger, data: A);
        import from Rep;
        import from SingleInteger;
        import from Array SingleInteger;

        data(arr: %): FortranMultiDimensionalArrayData == 
                rep(arr).data pretend FortranMultiDimensionalArrayData;

	convert(x:%):BArr == data(x) pretend BArr;

        tidx(idx: Array SingleInteger, sz: Array SingleInteger): SingleInteger == {
                assert(# idx = # sz);
                off := idx.1;
                for i in 2..# idx repeat {
                        off := off + (idx.i - 1) * sz.(i - 1);
                }
                off
        }

        new(t: Array SingleInteger): % == {
                len := 1;
                for i in 1..# t repeat 
                        len := len * t.i;
		per [ [d for d in t], new len]
        }
        
        apply(arr: %, t: Array SingleInteger): T == rep(arr).data.(tidx(t, rep(arr).sz));
        
        set!(arr: %, t: Array SingleInteger, f: T): T == {
                (rep(arr).data).(tidx(t, rep(arr).sz)) := f;
                f
        }
        
        -- BasicType stuff
        (=)(a: %, b: %): Boolean == error "Not Implemented";

        (<<)(o: TextWriter, a: %): TextWriter == o << "[array]";

        sample: % == new [1,1];
        
        generator(x: %): Generator T == error "Not implemented";
        
        map(f: T -> T, arr: %): % == error "Not implemented";

        #(x: %): SingleInteger == {
                arr := rep(x).sz;
                n: SingleInteger := 1;
                for v in arr repeat n := n*v;
                n
        }
}
