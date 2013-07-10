-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

macro Agg E == with
	bracket:   Generator E -> %
	generator: % -> Generator E

Links(S: Type): Agg S == add pretend Agg S
Vect(T: Type): Agg T == add pretend Agg T

f(): () ==
	export lv: Links Vect Integer
	export f: Vect Integer -> Links Integer

	vl: Vect Links Integer := [f v for v: Vect Integer in lv]
