-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O -Q inline-all
--> testrun -O -Q inline-all -l axllib 

#include "axllib"
#pile

macro SI == SingleInteger

len(t: Tuple SI): SI == length t

-- Tests inlining and env merging of arrays with constant size and indices.
foo(t: Tuple SI): SI ==
	len t ~= 3 => 10
	element(t,1) + element(t,2) + element(t,3)

bar():() ==
	import from SI
	local n: SI 
	n := foo(1,2,3)
	print<<n<<newline
	n := foo(4)
	print<<n<<newline

	import from List SI
	print<<[1,2,3,4,5]<<newline

bar()
