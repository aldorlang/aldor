-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

Ag ==> (S: BasicType) -> LinearAggregate S

MyPackage: with 
	SwapPackage: (X: Ag, Y: Ag) -> (S: BasicType) -> with (swap: X Y S -> Y X S)
 == add 
	SwapPackage(X: Ag, Y: Ag)(S: BasicType): with (swap: X Y S -> Y X S)
	    == add
		swap(xys: X Y S): Y X S == 
			[[s for s: S in y]$X(S) for y: Y S in xys]$Y(X(S))


f(): () == 
	import from List  Integer
	import from Array Integer
	import from Integer
	import from Segment Integer
	import from String

	al: Array List Integer := array(list(i+j-1 for i in 1..3) for j in 1..3)
	print<<"This is an array of lists: "<<newline<<al<<newline<<newline

	import from MyPackage
	import from SwapPackage(Array, List)(Integer)

	la: List Array Integer := swap(al)
	print<<"This is a list of arrays: "<<newline<<la<<newline


f()
