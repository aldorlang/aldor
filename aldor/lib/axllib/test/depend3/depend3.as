-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

V(S: Type): with
	put: S -> %
	get: % -> S
  == add
	Rep ==> Record(value: S)
	import from Rep

	put(s: S): % == per [s]
	get(v: %): S == apply(rep v, value)

f(): () ==
	v: V Integer := put 3
	i: Integer   := get v
