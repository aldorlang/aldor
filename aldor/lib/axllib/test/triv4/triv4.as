-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  Triv4      -- A minimal exporting program, independent of any other code.
#pile

--> testcomp
--> testgen c
--> testgen l
--> testrun -l axllib

export
	Type:	      	Type
	Tuple:		Type -> Type
	->:  	      	(Tuple Type, Tuple Type) -> Type
	Literal:	Type
	String:		Type
	Int:		Type

import
	printf:     	(String, Int) -> Int
	puts:		(String) -> Int
from Foreign

import
	ArrToSInt:    	String -> Int
from Builtin


integer(s: Literal): Int    == ArrToSInt (s pretend String)
string (s: Literal): String == s pretend String

-- FIXME: This generates a couple of warnings from the C compiler,
-- but it's probably better to just let them pass than introduce 
-- additional code.
printTriv(): () ==
	printf("%d ", 42)
	puts "Skidoo"

printTriv()
