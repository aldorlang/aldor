-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

macro toRep(dval) == dval @ % pretend Rep
macro frRep(rval) == rval @ Rep pretend %

+++ Lst(S) provides an implementation of linked lists.

Lst( S: with (=:(S,S)->Boolean; <<: (TextWriter, S) -> TextWriter) ): with

  	nil:       %
	cons:	   (S, %) -> %
	empty?:    % -> Boolean
	first:	   % -> S
	rest:	   % -> %
	=:         (%, %) -> Boolean
	map:       (S->S, %) -> %
	<<:	   (TextWriter, %) -> TextWriter

	setFirst!: (%, S)   -> S
		++ setFirst!(l,s) replaces the first element of l with s.
	setRest!:  (%, %) -> %
		++ setRest!(l,t) replaces the tail of l with the list t.
	reverse!:  % -> %
		++ reverse! l gives the head of an in-place reversal of l.

	test:      % -> Boolean
	generator: % -> Generator S
		

        bracket:       Generator S -> %

        bracket:       () -> %
        bracket:       (S) -> %
        bracket:       (S,S) -> %
        bracket:       (S,S,S) -> %
        bracket:       (S,S,S,S) -> %
        bracket:       (S,S,S,S,S) -> %

    == add
	macro Rep == U
	macro R   == Record(first: S, rest: Rep)

	U: with
		nil:      U
		unil:     R -> U
		nil?:     U -> Boolean
		value:    U -> R
    	    == add
		import from Pointer
		nil: U           == nil @ Pointer pretend U
		unil (r: R): U   == r pretend U
		nil? (u: U): Boolean == nil?(u pretend Pointer)
		value(u: U): R   == u pretend R

	import from R
	import from Rep
	import from Boolean

	rec(x: %): R             == value toRep x

	nil: %                   == frRep nil
	cons(a: S, l: %): %      == frRep unil [a, toRep l]
	empty?(l: %): Boolean    == nil?  toRep l
	first (l: %): S          == apply(rec l, first)   --!! apply will be .
	rest  (l: %): %          == frRep apply(rec l, rest) 

	setFirst!(l: %, a: S): S == set!(rec l, first, a) --!! set! will be :=
	setRest! (l: %, t: %): % == frRep set!(rec l, rest,  toRep t)

	reverse! (l: %): % ==
		local t, r: %
		r := nil
		while l repeat
			t := rest l
			setRest!(l, r)
			r := l
			l := t
		r

	map(f: S->S, l: %): %    ==
		r: % := nil
		for a: S in l repeat r := cons(f a, r)
		reverse! r

--	map(f: S->S, l: %): %  ==  [f a for a: S in l]
						

  	(l1: %) = (l2: %): Boolean ==
  		import from S
  		while l1 and l2 repeat
  			if not(first l1 = first l2) then return false
  		return empty? l1 and empty? l2

  	(p: TextWriter) << (l: %): TextWriter == 
 		import from String
		import from S
  
  		p << "["
  		if l then
  			p << first l
  			l := rest l
  		for a: S in l repeat
  			p << ", " << a
  		p << "]"

	test(l: %): Boolean == not empty? l

	generator(l: %): Generator S == generate
		while l repeat
			yield first l
			l := rest l

  	[ita: Generator S]: % ==
  		local h, l, t: %
  		h := l := nil
  		for a: S in ita repeat
  			t := l
  			l := cons(a, nil)
  			if empty? t then
  				h := l
  			else
  				setRest!(t, l)
  		h

	[]: %                        == nil
	[a:S]: %                     == cons(a, [])
	[a:S, b:S]: %                == cons(a, [b])
	[a:S, b:S, c:S]: %           == cons(a, [b, c])
	[a:S, b:S, c:S, d:S]: %      == cons(a, [b, c, d])
	[a:S, b:S, c:S, d:S, e:S]: % == cons(a, [b, c, d, e])
