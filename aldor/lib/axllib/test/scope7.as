-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase scobind
--> testcomp
#pile

-- Inner 'test' is an implicit export.

#include "axllib.as"

test(t: Boolean): Boolean == t

Lst(S: with ( =: (S,S)->Boolean; ~=: (S,S)->Boolean; print: S -> ();)): with
	nil:	   %
	cons:	   (S, %) -> %
	empty?:	   % -> Boolean
	first:	   % -> S
	rest:	   % -> %
	test:	   % -> Boolean

    == add
	macro Rep == P
	macro R	  == Record(first: S, rest: P)

        -- This local domain gives an untagged union of
        -- Records and Nil.
	P: with
		nil?:	  % -> Boolean
		nilptr:	  %
		recptr:	  R -> %
		value:	  % -> R
	    == add
                macro  Rep == Pointer
                import from Rep
		nil? (p: %): Boolean == nil? rep p
		nilptr: %	 == per nil
		recptr(r: R): %	 == r pretend %
		value(p: %): R	 == p pretend R

	import from R, Rep, Boolean

	rec(x: %): R		 == value rep x

	empty?(l: %): Boolean	 == nil? rep l
	nil: %			 == per nilptr

	cons(a: S, l: %): %	 == per recptr [a, rep l]
	first (l: %): S		 == rec(l).first
	rest  (l: %): %		 == per rec(l).rest

	test(l: %): Boolean      == not empty? l
