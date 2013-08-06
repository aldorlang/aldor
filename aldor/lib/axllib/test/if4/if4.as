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
		aaa(n: Integer): Integer == n + 1
		aa(n: Integer): Integer == n + 1
	else
		bbb(n: Integer): Integer == n - 1
		bb(n: Integer): Integer == n - 1
	ccc(n: Integer): Integer == n

Bar(n: Integer): Integer ==
	import from Foo(n)

	if (n > 0) then
		aaa n + ccc n
	else
		bbb n + ccc n

import from Integer
print<<Bar 7<<newline
