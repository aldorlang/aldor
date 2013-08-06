-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  Triv1      -- A minimal program, independent of any other code.
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

import
	puts:     	(String) -> ()
from Foreign C "<stdlib.h>"

string (s: Literal): String          == s pretend String

puts "42 Skidoo"
