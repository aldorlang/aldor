-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O
--> testrun -O -l axllib 

#include "axllib.as"
#pile

SInt ==> SingleInteger

import from SInt

export test1: SInt -> SInt
export test2: SInt -> SInt
export test3: SInt -> SInt
export test4: SInt -> SInt
export test5: SInt -> SInt
export test6: SInt -> SInt
export test7: SInt -> SInt
export test8: SInt -> SInt

export fact: SInt -> SInt

test1(n: SInt): SInt ==
	3+4*2+n

test2(n: SInt): SInt ==
	test1(n)

test3(n: SInt): SInt ==
	4

fact(n: SInt): SInt ==
	if n < 1 then 1
	else n * fact(n - 1)

test4(n: SInt): SInt ==
	n := n*n
	n := n+n
	n

test5(n: SInt): SInt ==
	n := test4(n)
	n := n*n*n
	n


test7(n: SInt): SInt ==
	local q: SInt
	q := test6(n)
	n+q
	
test6(n: SInt): SInt ==
	local	z: SInt
	z := n+1
	n := z+z*n
	z

print << test1 3 <<newline
print << test2 3 <<newline
print << test3 3 <<newline
print << test4 3 <<newline
print << test5 3 <<newline
print << test6 3 <<newline
print << test7 3 <<newline
