-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
--> testint
--> testcomp
--> testrun -l axllib
#pile

-- Treating type values and non-type values uniformly.
higher(T: Type, f: T->T, t: T): T == f f t

inc(n: Integer): Integer == n + 1

-- inc inc 2
n: Integer   == higher(Integer, inc,   2)       

-- Ratio Ratio Integer
--!! Ratio does not quite satisfy (Ring -> Ring)
R: Ring      == higher(Ring,    Ratio pretend (Ring -> Ring), Integer) 

show(): () ==
	import from String
	two: R := 1 + 1
	print<<"4 @ I = "<<n<<newline
	print<<"2 @ R = "<<two<<newline

show()
