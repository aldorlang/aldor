-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

U: with
	put: Integer -> %
	get: % -> Integer
  == add
	Rep ==> Record(value: Integer)
	import from Rep

	put(s: Integer): % == per [s]
	get(v: %): Integer == apply(rep v, value)

u: U       := put 3
i: Integer := get u
