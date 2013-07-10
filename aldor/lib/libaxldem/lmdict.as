-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"

NNI ==> SingleInteger;
empty() ==> nil;
^= ==> ~=;

--%  Reference

+++ Author: Stephen M. Watt
+++ Date Created:
+++ Change History:
+++ Basic Operations: deref, elt, ref, setelt, setref, =
+++ Related Constructors:
+++ Keywords:  reference
+++ Description:  `Reference' is for making a changeable instance
+++ of something.

Reference(S: BasicType): with {
        refOf: S -> %;
          ++  refOf(n) creates a pointer (reference) to the object n.
        deref: % -> S;
          ++ deref(n) is equivalent to `apply(n)'.
        setref: (%, S) -> S;
          ++ setref(n,m) same as `set!(n,m)'.
--       =: (%, %) -> Boolean;
--        ++ a=b tests if `a' and `b' are equal.

	export from S;
}
== add {
        Rep ==> Record(value: S);
        import from Rep;

        default p:%;
        default v:S;

--      p = q                 == EQ(p, q)$Lisp;
        refOf(v: S): %          == per [v];
        deref(p: %): S        == rep(p).value;
        setref(p: %, v: S): S == rep(p).value := v;
}

--% List Multi Dictionary
--)abbrev domain LMDICT ListMultiDictionary
-- MBM Nov/87, MB Oct/89
 
 
+++ The ListMultiDictionary domain implements a dictionary with duplicates
+++ allowed.  The representation is a list with duplicates represented
+++ explicitly.  Hence most operations will be relatively innefficient
+++ when the number of entries in the dictionary becomes large.
+++ The operations `pick', `count' and `delete' can be used to iterate
+++ over the objects in the dictionary.  If the objects in the
+++ dictionary belong to an ordered set, the entries are maintained in
+++ ascending order.
 

MultiDictionary(S: BasicType): Category == with {
	dictionary: () -> %;
	insert!: (S,%) -> %;
	insert!: (S,%,NNI) -> %;
	empty?: % -> Boolean;
	inspect: % -> S;
	count: (S, %) -> NNI;
	remove!: (S, %) -> %;
	parts: % -> List S;
}

ListMultiDictionary(S: BasicType): MultiDictionary S with {
	-- finiteAggregate
	duplicates?: % -> Boolean;
		++ duplicates?(d) tests if dictionary d has duplicate entries.
}
== add {
	Rep ==> Reference List S;
	import from Rep, S;

	default s,t: %;
	default l: List S;
	default n: SingleInteger;
 
	parts(s:%): List(S) == deref rep(s);
	#(s:%): NNI         == # parts s;
	count(x:S, s:%):NNI == {
		sum:NNI:=0;
		for y in parts s | y=x repeat sum := sum + 1;
		sum;
	}
	copy(s:%):%         == dictionary copy parts s;
	empty?(s:%):Boolean == empty? parts s;
	bag(l:List(S)):%    == dictionary l;
	dictionary():%      == dictionary empty();
	dictionary(l:List(S)):% == {
		 per refOf l
	}
	insert!(x:S, s:%, n:NNI):%  == {for i in 1..n repeat insert!(x, s); s}
     -- removeDuplicates! s == dictionary removeDuplicates! parts s;
 
	inspect(s:%):S == {
		empty? s => error "empty dictionary";
		first parts s;
	}
 
	extract!(s:%):S == {
		empty? s => error "empty dictionary";
		x := first(p := parts s);
		setref(rep s, rest p);
		x
	}
 
	duplicates?(s:%):Boolean == {
		empty?(p := parts s) => false;
		q := rest p;
		while not empty? q repeat {
			first p = first q => return true;
			p := q;
			q := rest q;
		}
		false
	}
 
#if ConditionalCat
	if S has OrderedSet then {
		s = t == parts s = parts t;
 
		remove!(x, s) == {
			p := deref s;
			while not empty? p and x = first p repeat p := rest p;
			setref(s, p);
			empty? p => s;
			q := rest p;
			while not empty? q and x > first q repeat
				(p,q) := (q,rest q);
			while not empty? q and x = first q repeat q := rest q;
			p.rest := q;
			s
		 }
 
		 insert!(x, s) == {
			p := deref s;
			empty? p or x < first p => {
				setref(s, concat(x, p));
				s
			}
			q := rest p;
			while not empty? q and x > first q repeat
				(p, q) := (q, rest q);
			p.rest := concat(x, q);
			s
		 }
	} 
	else {
#endif
		-- ?? "Getter failed" with this
		-- local remove!:(S, List(S)) -> List(S);
		remove!(v:S, x:List S):List S == {
			while not empty? x and v = first x repeat x := rest x;
			empty? x => x;
			p := x;
			q := rest x;
			while not empty? q repeat {
				if v=first q then
					setRest!(p, rest q);
				else
					p := q;
				q := rest q
			}
			x
		}
		
		remove!(x:S, s:%):% == {setref(rep s, remove!(x, parts s)); s}

		(s:%) = (t:%):Boolean == {
			a := copy s;
			while not empty? a repeat {
				x := inspect a;
				count(x, s) ^= count(x, t) => return false;
				remove!(x, a)
			}
			true
		}
	 
		insert!(x:S, s:%):% == {
			p := deref rep s;
			while not empty? p repeat {
				if x = first p then {
					setRest!(p, cons(x, rest p));
					return s
				}
				p := rest p;
			}
			setref(rep s, cons(x, deref rep s));
			s
		}
	}
