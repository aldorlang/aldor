-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib"

-- This tests a category declaration without a definition.
export C0: Category

-- This tests an empty category definition.
C1: Category == with ()

Set: Category == with
	=: (%, %) -> Boolean

Mon: Category == with
	1: %
	*: (%,%) -> %

Rng: Category == Join(Set, Mon)

Operator(T: Type): Category == with
	*: (T, %) -> %

Algebra(R: Rng): Category == Join(Rng, Operator R)

A(R: Rng): Algebra R == add
	import from R
        (x: %) = (y: %): Boolean == true
	r(x: %): R == x pretend R
	a(x: R): % == x pretend %
        1: %       == a 1
	(x: %) * (y: %): % == a(r x * r y)
	(x: R) * (y: %): % == a(x   * r y)

B(R: Rng): Algebra R == A R

Left: Rng == add
	Rep ==> Integer
	import from Rep
        (x: %) = (y: %): Boolean == true
        1: % == per 1
	(a: %) * (b: %): % == a

Right: Rng == add
	Rep ==> Integer
	import from Rep
        (x: %) = (y: %): Boolean == true
        1: % == per 1
	(a: %) * (b: %): % == b

-- Simple cat inheritance
C2(): Category == with 
	f:%->%
NewCat(): Category == C2()

f(): () == 
	lt: List Type         == [Integer, Category, Rng]
	lc: List Category     == [Set, Mon, Rng, Join(Set, Mon)]
	lm: List Mon          == [Left, Right, A Left, A Right]
	ll: List Algebra Left == [A Left, B Left]
