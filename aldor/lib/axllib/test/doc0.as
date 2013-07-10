-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase parse
--> testgen y
#pile

-- This file tests placement of ++ documentation and that it gets
-- properly transferred to the correct identifier.

#include "axllib.as"


ListCat(S: Type): Category == Join(BasicType,Aggregate S,Conditional) with

    +++ nil is a literal constant
    nil:       %	    ++ that is an empty list

    +++ cons(s,k) appends
    cons:      (S, %) -> %
	++ s to the front of the list k.

    +++ list(t) generates a list from a tuple.
    list:      Tuple S -> %

    +++ list(i) generates a list from a generator.
    list:      Generator S -> %

    +++ first k returns the first element of the list k.
    first:     % -> S

    +++ rest k returns the list consisting of all elements
    +++	    of k after the first.
    rest:      % -> %

    setFirst!: (%, S) -> S
	++ setFirst!(k, s) destructively modifies the list k
	++	so that the first element is s.

    setRest!:  (%, %) -> %	++ setRest!(k1, k2) destructively
				++ modifies the list k1
				++ so that rest(k1) = k2.

    +++ reverse! k destructively
    +++ reverses the elements of k.
    reverse!:  % -> %

    +++ concat!(k1, k2) destructively (to k1) appends k1 to the
    +++	    front of k2.
    concat!:   (%, %) -> %

    +++ concat(k1, k2) returns a new list that contains the
    +++	    elements of k1 appended to the front of k2.
    concat:    (%, %) -> %

    +++ reduce(fun, k, s) computes r1 = fun(first k, s), then
    reduce:    ((S, S) -> S, %, S)  -> S   ++ r2 = (fun first rest k, r1)
					   ++ and so, returning the
					   ++ final value computed.


    member?:   (S, %) -> Boolean	++ member?(s, k) returns
				++ true if s is contained in k,
		++ false otherwise.

    +++ apply(k, i) returns the i-th element of k
    apply:     (%, SingleInteger) -> S

+++ List(S) provides an implementation of linked lists.

List(S: BasicType): ListCat S == add

    macro Rep0== P
    macro Rep == P
    macro R   == Record(first: S, rest: Rep0)

pretend ListCat S


+++ TestSet is a very basic category exporting only one operation.

TestSet: Category == with
	=: (%, %) -> Boolean  ++ a = b tests for equality of a and b.


+++ TestMonoid is slightly more complicated.

TestMonoid: Category == with
	1: %		  ++ 1 is an exported constant.
	*: (%,%) -> %	  ++ a * b is the monoid operation.

+++ TestOperator is now reporting its operations.

TestOperator(T: Type): Category == with
	*: (T, %) -> %	  ++ t * o is the operation that we want to see.
