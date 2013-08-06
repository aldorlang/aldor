-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

-- This file tests that imports and types on ids do not wander
-- into other branches of the tree.

#include "axllib.as"

SetCategory ==>
	=:  (%, %) -> Boolean
	~=: (%, %) -> Boolean
	<<: (TextWriter, %) -> TextWriter

LstCategory S ==>
	SetCategory

	nil:	   %
	trivial:   () -> %

Lst(S: with SetCategory): with LstCategory S == add

	macro Rep0== P
	macro Rep == P
	macro R	  == Record(first: S, rest: Rep0)

	-- This local domain gives an untagged union of Records and Nil.
	P: with
		nil?:	 % -> Boolean
		nilptr:	 %
		recptr:	 R -> %
		value:	 % -> R
	    == add
		macro Rep == Pointer
		import from Rep
		nil? (p: %): Boolean == nil?(rep p)$Rep
		nilptr: %	 == per nil
		recptr(r: R): %	 == r pretend %
		value(p: %): R	 == p pretend R

	import from R, S, Rep

	(l1: %) = (l2: %): Boolean	 == false		-- dummy
	(l1: %) ~= (l2: %): Boolean	 == ~(l1 = l2)
	(p: TextWriter) << (b: %): TextWriter == print	-- dummy

	nil: %			 == per nilptr
	trivial(): %		 == nil		-- has unique type
