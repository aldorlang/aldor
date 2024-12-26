-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  Triv1      -- A minimal program, independent of any other code.
#pile

--> testcomp
--> testgen c
--> testgen l
--> testrun -l axllib

Machine: with
    Ptr: Type
== add
    Ptr: Type == add

export
	Type:	      	Type
	Tuple:		Type -> Type
	->:  	      	(Tuple Type, Tuple Type) -> Type
	Literal:	Type
	String:		Type

import
	puts:     	(Ptr$Machine) -> ()
from Foreign C "<stdio.h>"

string (l: Literal): String          == l pretend String
ptr(s: String): Ptr$Machine == s pretend Ptr$Machine
puts(ptr("42 Skidoo"))
