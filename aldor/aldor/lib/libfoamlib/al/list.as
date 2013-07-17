-----------------------------------------------------------------------------
----
---- list.as:  Linked lists
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib.as"

define ListCategory(S: Type): Category
== Join(FiniteLinearAggregate S, Conditional) with {

	-- Cascade the exports from the enumeration types 'first' and 'rest'.
	export from 'first';
	export from 'rest';

        nil: () -> %;
                ++ nil is a literal constant that is an empty list.

        cons: (S, %) -> %;
                ++ cons(s,k) appends s to the front of the list k.

        list: Tuple S -> %;
                ++ list(t) generates a list from a tuple.

        list: Generator S -> %;
                ++ list(i) generates a list from a generator.

        first: % -> S;
                ++ first k returns the first element of the list k.

        rest: % -> %;
                ++ rest k returns the list consisting of all elements
                ++ of k after the first.

        setFirst!: (%, S) -> S;
                ++ setFirst!(k, s) destructively modifies the list k
                ++ so that the first element is s.

        setRest!: (%, %) -> %;
                ++ setRest!(k1, k2) destructively modifies the list k1
                ++ so that rest(k1) = k2.

	apply: (%, 'first') -> S;
		++ `l.first' is equivalent to `first(l)'.

	apply: (%, 'rest')  -> %;
		++ `l.rest' is equivalent to `rest(l)'.

	set!:  (%, 'first', S) -> S;
		++ `l.first := v' is equivalent to `setFirst!(l, v)'.

	set!:  (%, 'rest', %) -> %;
		++ `l.rest := t' is equivalent to `setRest!(l, v, t)'.

        copy: % -> %;
                ++ copy k returns a copy of the list k
        last: % -> S;
                ++ last returns the last element of the list k

        reverse: % -> %;
                ++ reverse k non-destructively reverses the elements
                ++ of k.

        reverse!: % -> %;
                ++ reverse! k destructively reverses the elements of k.

        concat!: (%, %) -> %;
                ++ concat!(k1, k2) destructively (to k1) appends k1 to
                ++ the front of k2.

        concat: (%, %) -> %;
                ++ concat(k1, k2) returns a new list that contains the
                ++ elements of k1 appended to the front of k2.

        reduce: ((S, S) -> S, %, S)  -> S;
                ++ reduce(fun, k, s) computes r1 = fun(first k, s), then
                ++ r2 = (fun first rest k, r1) and so, returning the
                ++ final value computed.

        member?: (S, %) -> Boolean;
                ++ member?(s, k) returns true if s is contained in k,
                ++ false otherwise.

        rest: (%, SingleInteger)  -> %;
                ++ rest(k, i) drops the first i elements from k

	tails: % -> Generator %;
		++ tails k returns the successive tails of k.
		++ Clients are allowed to call setFirst! and setRest!
		++ on the generated values.

	dispose!: % -> ();
		++ `dispose l' indicates the list `l' will no longer be used.

	disposeHead!: % -> %;
		++ `disposeHead! l' indicates the head of `l' will no longer
		++ be used, and returns the tail.
}

List(S: Type): ListCategory S with == FakedConditionalOperations S add {

        macro Rep == P;
        macro R   == Record(first: S, rest: Rep);

	--!! Remove when cascaded imports can be inferred in the correct order.
	import from S, 'first', 'rest';

        -- This local domain gives an untagged union of
        -- Records and Nil.
        P: with {
                nil?:    % -> Boolean;
                nilptr:  %;
                recptr:  R -> %;
                value:   % -> R;
        } == add {
                macro  Rep == Pointer;
                import from Rep;
                nil? (p: %): Boolean == nil? rep p;
                nilptr: %        == per (nil()$Pointer);
                recptr(r: R): %  == r pretend %;
                value(p: %): R   == p pretend R;
        }

        import from R;

        default n: SingleInteger;

        -- non-empty % as a Record
        rec(x: %): R == value rep x;

        empty?(l: %): Boolean  == nil? rep l;
        #(l: %): SingleInteger == { n:=0; for i in l repeat n:=n+1; n }
        nil(): %     == per nilptr;
        empty():%  == nil();
        cons(a: S, l: %): % == per recptr [a, rep l];

	sample: %  == nil(); --!! Should get from Aggregate(S)

        list(its: Generator S): %    == [its];
        list(tup: Tuple S): %        == [tup];

        map(f: S->S,   l: %): %      == [f(a)   for a in l];
        map(f:(S,S)->S,l1:%,l2:%): % == [f(a,b) for a in l1 for b in l2];

        first(l: %): S == {
		empty? l => error "Cannot select `first' of empty list.";
		rec(l).first;
	}
        rest(l: %): % == {
		empty? l => l;
		per rec(l).rest;
	}
        setFirst!(l: %, a: S): S == {
		empty? l => error "Cannot set `first' of emtpy list.";
		rec(l).first := a
	}
        setRest! (l: %, t: %): % == {
		empty? l => error "Cannot set `rest' of emtpy list.";
		per (rec(l).rest := rep t)
	}
	apply(l: %, x: 'first'): S == first l;
	apply(l: %, x: 'rest' ): % == rest l;
	set! (l: %, x: 'first', a: S): S == setFirst!(l, a);
	set! (l: %, x: 'rest',  t: %): % == setRest!(l, t);

        tail (l: %): % == {
                empty? l => error "empty list";
                x := l;
                y := rest l;
                while not empty? y repeat
                   y := rest(x := y);
                x;
        }

        last (l: %): S == first tail l;

        copy(l: %): % == [x for x in l];

        reverse(l: %): % ==  {
                revl := nil();
                for e in l repeat revl := cons(e, revl);
                revl;
        }
        reverse!(l: %): % == {
                r := nil();
                while l repeat {
			-- (l.rest, r, l) := (r, l, l.rest);
                        t := l.rest;
                        l.rest := r;
                        r := l;
                        l := t;
                }
                r;
        }
        concat!(l1: %, l2: %): % == {
                empty? l1 => l2;
                l := l1;
                while (t := rest l) repeat l := t;
                l.rest := l2;
                l1;
        }
        concat(l1: %, l2: %): % == {
                empty? l1 => l2;
		e := empty();
		ini := cons(first l1, e);
		new := ini;
		for x in rest l1 repeat {
			new.rest := cons(x, e);
			new      := rest new;
		}
		new.rest := l2;
		ini
        }
        reduce(f: (S,S)->S, l: %, v: S): S == {
                ans := v;
                for x in l repeat ans := f(ans, x);
                ans;
        }
        apply(l: %, i: SingleInteger): S == {
                for j in 1..(i-1) while l repeat l := rest l;
                l => first l;
                error "apply: too few elements in list";
        }
        rest(l: %, i: SingleInteger): % == {
                for j in 1..i while l repeat l := rest l;
                l;
        }

	if S has BasicType then {
	        member?(v:S, l:%): Boolean == {
	                for x in l repeat
	                        if v = x then return true;
	                false;
	        }

	        (l1: %) = (l2: %): Boolean == {
	                while l1 and l2 repeat {
	                        first l1 ~= first l2 => return false;
	                        l1 := rest l1;
	                        l2 := rest l2;
	                }
	                empty? l1 and empty? l2;
	        }

	        (l1: %) ~= (l2: %): Boolean == ~(l1 = l2);

	        (p: TextWriter) << (l: %): TextWriter == {
	                empty? l => p << "list()";
	                p << "list(";
	                p << first l;
	                for a in rest l repeat p << ", " << a;
	                p << ")";
	        }
	}

        test(l: %): Boolean == not empty? l;

        generator(l: %): Generator S == generate {
                while l repeat {
                        yield first l;
                        l := rest l;
                }
        }

	tails(l: %): Generator % == generate {
		while l repeat {
			-- Save the tail first to allow clients to update it.
			tl := rest l;
			yield l;
			l := tl;
		}
	}

        [t: Tuple S]: % == {
                l := nil();
                for i in length t..1 by -1 repeat
                        l := cons(element(t, i), l);
                l;
        }

        [g: Generator S]: % == {
                h := l := nil();
                for a in g repeat {
                        t := l;
                        l := cons(a, nil());
                        empty? t => h := l;
                        t.rest := l;
                }
                h;
        }

	disposeHead!(l: %): % == { t := rest l; dispose! rec l; t }

	dispose!(l: %): ()    == while l repeat l := disposeHead! l;


	-- Originally these were defaults from BasicType etc
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}


-- !! This will go when we can get exact usage information
--    on conditionals
FakedConditionalOperations(S: Type): with {
	=: (%, %) -> Boolean;
	<<: (TextWriter, %) -> TextWriter;
	member?: (S, %) -> Boolean;
}  == add {
	(a: %) = (b: %): Boolean == {
		error "no equality on this object";
	}
	member?(a: S, b: %): Boolean == {
		error "no member? on this object";
	}
	(p: TextWriter) << (a: %): TextWriter ==  {
		p << "<unprintable>";
	}
}

