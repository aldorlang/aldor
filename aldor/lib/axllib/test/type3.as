-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs -M no-emax
#pile

#include "axllib.as"

ElementaryDoubleFloatFunctions: with
	sin:	F -> F
	cos:	F -> F
	tan:	F -> F

    == add
	-- F ==> DoubleFloat

	ForeignFunctions: with
		X__sin:	F -> F
		X__cos:	F -> F
		X__tan:	F -> F
	    == add
		import
			sin: F -> F
			cos: F -> F
			tan: F -> F
		from Foreign

		X__sin(x: F): F == sin x
		X__cos(x: F): F == cos x
		X__tan(x: F): F == tan x

	import from ForeignFunctions
	sin(x: F): F == X__sin(x)
	cos(x: F): F == X__cos(x)
	tan(x: F): F == X__tan(x)
