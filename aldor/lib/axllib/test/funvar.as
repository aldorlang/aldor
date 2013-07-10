-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen l
--> testgen c
--> testrun -l axllib
#pile

#include "axllib.as"

local f: Integer -> Integer

f0(n: Integer): Integer == n
f1(n: Integer): Integer == n + 1
f2(n: Integer): Integer == n - 1

-- Test direct call and call through a variable.
g(n: Integer): Integer ==
	free f
	if even? n then f := f1 else f := f2
	f0 n + f n

-- Test that function expression is only evaluated once.
h(n: Integer): Integer ==
	(n := n + 1000; if even? n then f1 else f2)(n)


import from Integer
import from String

print<<g 2<<" = 5"   <<newline
print<<g 3<<" = 5"   <<newline
print<<h 2<<" = 1003"<<newline
print<<h 3<<" = 1002"<<newline
