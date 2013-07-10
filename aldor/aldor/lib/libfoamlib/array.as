------------------------------------------------------------------------------
----
---- array.as: Data sturctures for one-dimensional arrays.
----
------------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
------------------------------------------------------------------------------

#include "foamlib.as"

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


Array(S: BasicType): ExtensibleArrayCategory(S) with == add {
        Rep ==> Record(size: SI, nvalues: SI, values: PrimitiveArray S);

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


	-- Originally these were defaults from BasicType etc
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}
