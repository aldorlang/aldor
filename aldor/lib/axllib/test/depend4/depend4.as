-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

Links(S: Type): with
        bracket:    Generator S -> %
	generator:  % -> Generator S
  == add
pretend with
        bracket:    Generator S -> %
	generator:  % -> Generator S

f(): () ==
	export l: Links Integer
	ll: Links Links Integer := [l for v: Integer in l]
