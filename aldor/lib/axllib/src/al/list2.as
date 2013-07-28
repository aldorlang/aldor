-----------------------------------------------------------------------------
----
---- list2.as:  Linked lists using builtin operations
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"

BuiltinList(S: Type): ListCategory S with
{
}
== FakedConditionalOperations S add
{
	import from Machine;
	import
	{
		ListNil:	() -> Ptr;
		ListEmptyP:	Ptr -> Bool;
		ListHead:	Ptr -> Word;
		ListTail:	Ptr -> Ptr;
		ListCons:	(Word, Ptr) -> Ptr;
	} from Builtin;


        Rep == Ptr;
	import from Rep;


	--!! Remove when cascaded imports can be inferred in the correct order.
	import from S, 'first', 'rest';


	-- Useful conversion
	wrap(s)   ==> ((s@S) pretend Word);
	unwrap(w) ==> ((w@Word) pretend S);


	-- Exports.
	empty?(l:%):Boolean == ListEmptyP(rep l)::Boolean;
	empty():% == per ListNil();
	nil:% == empty();
	sample:% == empty();
	cons(s:S, l:%):% == per ListCons(wrap(s), rep l);


	-- Constructors
        list(its:Generator S):% == [its];
        list(tup:Tuple S):% == [tup];


	-- Accessors
	first(l:%):S ==
	{
		assert(not empty? l);
		unwrap ListHead(rep l);
	}

	rest(l:%):% ==
	{
		assert(not empty? l);
		per ListTail(rep l);
	}


	-- Destructive updates
	setFirst!(l:%, s:S):S == s;
	setRest!(l:%, r:%):% == l;


	-- More accessors
	apply(l:%, x:'first'):S == first l;
	apply(l:%, x:'rest' ):% == rest l;

	set!(l:%, x:'first', s:S):S == setFirst!(l, s);
	set!(l:%, x:'rest', t:%):% == setRest!(l, t);


	-- Fresh copy
        copy(l:%):% == [x for x in l];


        reverse(l:%):% ==
	{
                local revl:% := empty();
                for e in l repeat
			revl := cons(e, revl);
                revl;
        }


        reverse!(l:%):% == l;
        concat!(l1:%, l2:%):% == l1;
        concat(l1:%, l2:%):% == l1;


        reduce(f:(S, S) -> S, l:%, v:S):S ==
	{
                local ans:S := v;
                for x in l repeat
			ans := f(ans, x);
                ans;
        }


	-- Low-level constructors
        [t:Tuple S]:% ==
	{
		import from SingleInteger;
                local l:% := empty();
                for i in length t..1 by -1 repeat
                        l := cons(element(t, i), l);
                l;
        }

        [g: Generator S]: % == {
		local t:%;
                local h:% := empty();
		local l:% := empty();

                for e in g repeat
		{
                        t := l;
                        l := cons(e, empty());

                        empty? t => h := l;
                        t.rest := l;
                }

                h;
        }


        #(l:%):SingleInteger ==
	{
		local n:SingleInteger := 0;
		for i in l repeat
			n := n + 1;
		n
	}

        map(f: S->S,   l: %): %      == [f(a)   for a in l];
        map(f:(S,S)->S,l1:%,l2:%): % == [f(a,b) for a in l1 for b in l2];

        last(l:%):S ==
	{
                assert(not empty? l);
                x := l;
                y := rest l;
                while not empty? y repeat
                   y := rest(x := y);
                first x;
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

	disposeHead!(l:%):% == { rest l; }

	dispose!(l:%):()    == while l repeat l := disposeHead! l;
}

