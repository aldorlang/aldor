-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen l
--> testrun -l axllib
#pile

#include "axllib.as"

import from SingleInteger

foo(): () ==
	print<<x*x<<newline where
		x:SingleInteger == 2


foo()

print<<x*x<<newline where
	x:SingleInteger == 5


bar(): () ==
	local
		f1: () -> SingleInteger
		f2: () -> SingleInteger
	for i in 1..2 repeat
		local x: () -> SingleInteger
		x := fun where
			local a: SingleInteger := i
			fun():SingleInteger == a+1
			baz():SingleInteger == fun()+1
			print<<fun()<<newline
			print<<baz()<<newline
		if (i = 1) then f1 := x
		if (i = 2) then f2 := x
		print<<x()<<newline
	print<<f2()<<newline
	print<<f1()<<newline

bar()
