-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O
--> testrun -O -l axllib 

#include "axllib.as"
#pile

export test1: SingleInteger -> SingleInteger
export test2: SingleInteger -> SingleInteger
export test3: Integer -> Integer
export test4: SingleInteger -> SingleInteger
export test5: SingleInteger -> SingleInteger
export test6: SingleInteger -> SingleInteger
export test7: SingleInteger -> SingleInteger
export test8: SingleInteger -> SingleInteger

export fact: SingleInteger -> SingleInteger

import from SingleInteger
import from Integer
import from Character

inline from SingleInteger
inline from Integer

test1(n: SingleInteger): SingleInteger ==
	2+n

test2(n: SingleInteger): SingleInteger ==
	test1(n)

test3(n: Integer): Integer ==
	4

fact(n: SingleInteger): SingleInteger ==
	if n < 1 then 1
	else n * fact(n - 1)

test4(n: SingleInteger): SingleInteger ==
	n := n*n
	n := n+n
	n

test5(n: SingleInteger): SingleInteger ==
	n := test4(n)
	n := n*n*n
	n


test6(n: SingleInteger): SingleInteger ==
	local	z: SingleInteger
	z := n+1
	n := z+z*n
	z

test7(n: SingleInteger): SingleInteger ==
	local q: SingleInteger
	q := test6(n)
	n+q
	

print << test1 3 <<newline
print << test2 3 <<newline
print << test3 3 <<newline
print << test4 3 <<newline
print << test5 3 <<newline
print << test6 3 <<newline
print << test7 3 <<newline
