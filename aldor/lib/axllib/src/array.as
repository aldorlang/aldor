------------------------------------------------------------------------------
----
---- array.as: Data sturctures for one-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

#include "axllib.as"

macro SI == SingleInteger;

define ArrayCategory(S: Type): Category == FiniteLinearAggregate S with {
        set!: (%, SI, S) -> S;
                ++ `set!(a, n, s)' sets the n-th element of a to s.

	dispose!: % -> ();
		++ `dispose! a' indicates that `a' will no longer be used.
}

ExtensibleArrayCategory(S) ==> ArrayCategory S with {
        array:		Tuple S -> %;
                ++ array(t) creates an array from a tuple.

        array:		Generator S -> %;
                ++ array(it) creates an array from an generator.

	empty!:		% -> ();
		++ empty! v resets v to have no elements.

        extend!:	(%, S) -> ();
                ++ extend!(v, s) adds the element s to the end of v.

        new:		(SI, S) -> %;
                ++ new(n,s) creates a new array with n elements, each
                ++ initialized to s.

	empty:		SI -> %;
		++ empty(n) creates a new array with n elements.
}


Array(S: BasicType): Join(
	ExtensibleArrayCategory S,
	FortranMultiArray,
	if (S has FortranFString) then FortranFStringArray
) with == add {
        Rep == Record(size: SI, nvalues: SI, values: PrimitiveArray S);

        import from Rep;

        new(size: SI, val: S): % == per [size, size, new(size, val)];
        empty(size: SI): %       == per [size, 0,    new size];

        apply(v: %, i: SI): S         == rep(v).values.i;
        set! (v: %, i: SI, val: S): S == rep(v).values.i := val;

        extend!(v: %, val: S): ()  == {
                r := rep v;
                if r.nvalues = r.size then {
                        nsize     := max(4, r.size + r.size quo 2);
                        r.values  := resize!(r.values, r.size, nsize);
                        r.size    := nsize
                }
                r.nvalues := r.nvalues+1;
                r.values(r.nvalues) := val
        }
	empty!(v: %): () == {
		rep(v).nvalues := 0;
	}
	dispose!(v: %): () == {
		dispose! rep(v).values;
		dispose! rep(v)
	}

        #(v: %): SI            == rep(v).nvalues;
        empty(): %             == empty 0;
        empty?(v: %): Boolean  == #v = 0;

	sample: %              == empty();

        array(g: Generator S): % == [g];
        array(t: Tuple S): %     == [t];

        map(f: S -> S, v: %): % == {
                n := #v;
                w := empty n;
                for i in 1..n repeat extend!(w, f v.i);
                w
        }
        map(f: (S,S) -> S, u: %, v: %): % == {
                n := min(#u, #v);
                w := empty n;
                for i in 1..n repeat extend!(w, f(u.i, v.i));
                w
        }
        (u: %) = (v: %): Boolean == {
                (n := #u) ~= #v => false;
                for i in 1..n repeat u.i ~= v.i => return false;
                true
        }
        (u: %) ~= (v: %): Boolean == ~(u = v);

        generator(v: %): Generator S == generate
                for i in 1..#v repeat yield v.i;

        [g: Generator S]: % == {
                w := empty();
                for t in g repeat extend!(w, t);
                w
        }

        [t: Tuple S]: % == {
                w := empty length t;
                for i in 1..length t repeat extend!(w, element(t,i));
                w
        }

        (p: TextWriter) << (v: %): TextWriter == {
                empty? v => p << "array()";
                p << "array(" << v.1;
                for i in 2..#v repeat p << ", " << v.i;
                p << ")"
        }

	-- FortranArray export which is automatically applied
	-- whenever a value from an array-like domain such as
	-- this is passed to Fortran.
	convert(x:%):BArr == {
		-- Deal with empty arrays first
		(empty? x) => ((0@SInt) pretend BArr);


		-- Function to return a row
		getter(arr:%, row:SI):S == arr.row;


		-- Arrays of arrays need flattening
                if (S has FortranMultiArray) then
			flatten(%, x, #x, getter)$S;
		else
			convert((rep x).values);
	}

	-- FortranArray export which is automatically applied
	-- after a value from an array-like domain such as this
	-- is passed to Fortran. The BArr passed to Fortran for
	-- the array value `x' is `flat'.
	convert(x:%, flat:BArr):BSInt == {
		import from Machine;
		local gap:BSInt;
		local arr:PrimitiveArray S;


		-- Deal with empty arrays first
		(empty? x) => 0;


		-- Arrays of arrays need special care
                if (S has FortranMultiArray) then {
			-- The elements of the sub-arrays are
			-- separated by `gap' elements.
			gap := (#x)::BSInt;

			for i in 1..(#x) repeat
				unflatten(x.i, flat, (i-1)::BSInt, gap)$S;
		}
		else {
			-- Need to use a PrimitiveArray so that the
			-- packed array can be accessed correctly.
			arr := flat::PrimitiveArray S;

			for i in 1..(#x) repeat
				x.i := arr(i);
		}


		-- Return value ignored
		0;
	}

	-- FortranMultiArray export. It will probably be invoked
	-- by a convert() function that was applied automatically
	-- when an array-like value was passed to a Fortran function.
	flatten(A:Type, arr:A, rows:SI, getter: (A, SI) -> %):BArr == {
		-- Requires getter(arr, 1) to exist otherwise
		-- we are in serious trouble.
		local slice:%;


		-- Allocate an array big enough for the return value.
		-- Use Array S instead of PrimitiveArray S so that it
		-- multi-dimensional array objects can be recursively
		-- flattened into a single BArr.
		local cols:SI := #(getter(arr, 1));
		local res:%   := empty(rows*cols);


		-- Flatten our (2D) array.
		for col in 1..cols repeat {
			for row in 1..rows repeat {
				slice    := getter(arr, row);
				extend!(res, slice(col));
			}
		}


		-- Recurse to flatten the rest.
		convert(res);
	}

	-- FortranMultiArray export used to unpack a multi-
	-- dimensional array which has been flattened into
	-- a machine array and passed to Fortran. The BArr
	-- that was passed to Fortran for the array value
	-- `x' is stored in `flat'. The elements begin at
	-- offset `off' (counting from 0) in `flat' and are
	-- separated by `stp' slots.
	unflatten(x:%, flat:BArr, off:BSInt, stp:BSInt):() == {
		import from Machine;
		local idx, gap:BSInt;
		local arr:PrimitiveArray S;


		-- Deal with empty arrays first
		(empty? x) => ();


		-- Arrays of arrays are tricky
                if (S has FortranMultiArray) then {
			-- The elements of the sub-arrays are
			-- separated by `gap' elements starting
			-- at index `off'.
			gap := ((#x)::BSInt)*stp;
			idx := off;

			for i in 1..(#x) repeat {
				unflatten(x.i, flat, idx, gap)$S;
				idx := idx + stp;
			}
		}
		else {
			-- Need to use a PrimitiveArray so that the
			-- packed array can be accessed correctly.
			-- 1-based indexing is also being used now.
			arr := flat::PrimitiveArray S;
			idx := off + 1;

			for i in 1..(#x) repeat {
				x.i := arr(idx::SingleInteger);
				idx := idx + stp;
			}
		}
	}


	-- Export of FortranFStringArray
	if (S has FortranFString) then
	{
		import from Machine;
		#():SInt == (#()$S);
	}
}

