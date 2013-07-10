--------------------------------------------------------------------------------
--
-- sm_delist.as: A domain constructor wrapping list so that appending c
--               an be done in O(1) time.
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- cat DoubleEndedList

#include "algebra"

+++ `DoubleEndedList' is a domain constructor wrapping
+++ list so that appending can be done in O(1) time.
+++ The typical use is to avoid code like:
+++    l: List S := empty()
+++    while foo repeat 
+++         x := calc()
+++         l := cons(x, l)
+++    ret := concat!(reverse! l, bar)
+++    ret
+++  which builds a list in reverse order, and then
+++ appends to it.  Examples where this happens include
+++ sparse polynomial addition, amongst other things.
+++
+++ Using DoubleEndedList the above will be:
+++     l: DoubleEndedList S := empty()
+++     while foo repeat
+++         x := calc()
+++         concat!(l, x)
+++     lastCell(l).rest := bar
+++     ret := l.firstCell
+++     ret
+++  The compiler can optimise the additional storage associated
+++  with this object.  The above will also avoid 2 extra traversals 
+++  of the list.
+++
+++ Overview: DataStructure


DoubleEndedList(T: Type): with {
        empty: () -> %;
                ++ `empty()' returns the empty list.
        empty?: % -> Boolean;
                ++ emptyness test.
	cons!: (T, %) -> %;
		++ puts `t' at the start of the list.
	concat!: (%, T) -> %;
		++ put t at the end of the list
	concat!: (%, %) -> %;
		++ concat!(e,f) puts f at the end of e
	firstCell: % -> List T;
		++ Return the whole list
	lastCell: % -> List T;
		++ return the  last element of the list
        bracket: List T -> %;
                ++ construction from a list
        if T has CopyableType then CopyableType;
        if T has ExpressionType then ExpressionType;

} == add {

        import from T, List T;

	Rep == Record(whole: List T, end: List T);

	import from Rep;

	firstCell(x: %): List T == rep(x).whole;

	lastCell(x: %): List T == rep(x).end;

        bracket (l: List T): % == {
		local p,t: List T;
		t := p := l;
		while not empty? t repeat {
			p := t;
			t := rest t;
		}
		per [l, p];		
	}
	empty(): % == per [empty, empty];

        empty? (x: %): Boolean == empty? rep(x).whole;

        map (f : T -> T) (x: %): % == {
                empty? x => x;
                local tout, fin : List T;
                l := firstCell x;
                fin := [f first l];
                tout := fin;
                while not empty? (l := rest l) repeat {
                      setRest!(fin,[f first l]);
                      fin := rest fin;
                }
                per [tout, fin];
        }
	cons!(t: T, x: %): % == {
                local tout, fin : List T;
		(tout, fin) := explode rep x;
                cell: List T := cons(t, tout);
		if empty? fin then {
                        assert(empty? tout);
			rep(x).end := cell;
		}
		rep(x).whole := cell;
                x;
	}
	concat!(x: %, t: T): % == {
                local tout, fin : List T;
		(tout, fin) := explode rep x;
		cell: List T := cons(t, empty);
		if empty? fin then {
                        assert(empty? tout);
			rep(x).whole := cell;
			rep(x).end := cell;
		} else {
                        setRest!(fin,cell);
		        rep(x).end := cell;
                }
                x;
	}
        concat!(x: %, y: %): % == {
                local tout, fin : List T;
                (tout, fin) := explode rep x;
		if empty? fin then {
                        assert(empty? tout);
			rep(x).whole := firstCell(y);
                        rep(x).end := lastCell(y);
                } else {
                        setRest!(fin,firstCell(y));
                        rep(x).end := lastCell(y);
                }
                x;
        }
        if T has CopyableType then {
           import from T pretend CopyableType;
           copy(x: %): % == {
              per [copy(rep(x).whole), copy(rep(x).end)];
           }
        }
        if T has ExpressionType then {
           import from T pretend ExpressionType;
           (x: %) = (y: %): Boolean == {
              firstCell(x) = lastCell(x);
           }
           extree(x: %): ExpressionTree == {
              le: List ExpressionTree := [
                  extree(t)$T for t in generator(firstCell(x))
              ];
              ExpressionTreeList le;
           }
        }
}








