-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--% Test abstract machine code generation for arrays and records.
--> testcomp
--> testgen c
--> testrun -l axllib
#pile

#include "axllib.as"

export
	Vector:  Type->Type

macro
	SI    == SingleInteger
	DF    == BDFlo
	Char  == Character

import 
        ArrNew:    (Boolean,  SI) -> Vector Boolean
        ArrNew:    (Char,     SI) -> Vector Char
        ArrNew:    (DF,       SI) -> Vector DF

	ArrElt:    (Vector Boolean,  SI) -> Boolean
	ArrElt:    (Vector Char,     SI) -> Char
	ArrElt:    (Vector DF,       SI) -> DF

	ArrSet:    (Vector Boolean,  SI, Boolean)  -> Boolean
	ArrSet:    (Vector Char,     SI, Char) -> Char
	ArrSet:    (Vector DF,       SI, DF)   -> DF
from Builtin

import from SingleInteger, DoubleFloat, Machine

f(x: DF): DF ==
	local vd: Vector DF

	vd := ArrNew(x, 3)
	ArrSet(vd, 2, 2.0::DF)
	x + ArrElt(vd, 2)

print<<f(3.0::DF)::DoubleFloat<<newline
