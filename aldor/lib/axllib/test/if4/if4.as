-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib.as"
#pile

Foo(n: Integer): with
	import from Integer
	if n > 0 then
		aaa: Integer -> Integer
		aa: Integer -> Integer
	else
		bbb: Integer -> Integer
		bb: Integer -> Integer
	ccc: Integer -> Integer

== add
	if n > 0 then
		aaa(m: Integer): Integer == m + 1
		aa(m: Integer): Integer == m + 1
	else
		bbb(m: Integer): Integer == m - 1
		bb(m: Integer): Integer == m - 1
	ccc(m: Integer): Integer == m

Bar(n: Integer): Integer ==
	import from Foo(n)

	if (n > 0) then
		aaa n + ccc n
	else
		bbb n + ccc n

import from Integer
print<<Bar 7<<newline
