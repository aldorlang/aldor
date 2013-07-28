-----------------------------------------------------------------------------
----
---- sort.as: Sorting of linked lists.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"

SI ==> SingleInteger;
LS ==> List S;
B  ==> Boolean;
 
ListSort(S: BasicType): with {
	sort!:((S,S) ->B, LS) -> LS;
	sort: ((S,S) ->B, LS) -> LS;
}
== add {
	sort (<:(S,S)->B, l:LS):LS == sort!(<, list(a for a in l));
	sort!(<:(S,S)->B, l:LS):LS == mergeSort!(<, l, #l);

	split!(p:LS, n:SI):LS == {
		n < 1 => error "index out of range";
		p := rest(p, n - 1);
		q := rest p;
		setRest!(p, nil);
		q
	}
	merge!(<:(S,S)->B, p:LS, q:LS):LS == {
		not p => q;
		not q => p;
		-- eq(p, q) => error "cannot merge a list into itself"
		if first p < first q
			then { r := t := p; p := rest p }
			else { r := t := q; q := rest q }
		while p and q repeat {
			if first p < first q
				then { setRest!(t, p); t := p; p := rest p }
				else { setRest!(t, q); t := q; q := rest q }
		}
		setRest!(t, if p then p else q);
		r
	}
	mergeSort!(<:(S,S)->B, p:LS, n:SI):LS == {
		if n = 2 and first rest p < first p then p := reverse! p;
		n < 3 => p;
		l := (n quo 2);
		q := split!(p, l);
		p := mergeSort!(<, p, l);
		q := mergeSort!(<, q, n - l);
		merge!(<, p, q)
	}
}
