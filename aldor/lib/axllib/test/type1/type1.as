-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

Utilities: with
	T1: IntegerNumberSystem
	T2: IntegerNumberSystem
	show: (String, T1) -> ()
	show: (String, T2) -> ()
  == add
	T1: IntegerNumberSystem == Integer
	T2: IntegerNumberSystem == SingleInteger

	show(s: String, t: T1): () ==
		print<<s<<t<<newline
	show(s: String, t: T2): () ==
		print<<s<<t<<newline


f(): () ==
	import from Utilities
	x1: T1 := 1 + 1 + 1
	x2: T2 := 1 + 1 + 1

	show("The 3 of type T1 is ", x1)
	show("The 3 of type T2 is ", x2)

f()
