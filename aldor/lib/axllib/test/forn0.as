-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen c

#include "axllib.as"
#pile

import
	getenv: String -> String
	puts:   String -> SingleInteger
from Foreign C

import
	CONS: (Pointer, Pointer) -> Pointer
	CAR:  Pointer -> Pointer
	CDR:  Pointer -> Pointer
from Foreign Lisp

showEnv(s: String): String ==
	puts getenv s
	s

CONS
CAR
CDR
