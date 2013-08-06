-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen l
--> testgen c
#pile

#include "axllib.as"

macro
	C == ComplexDoubleFloat
	F == DoubleFloat

ComplexDoubleFloat: with
	complex: (F,F) -> C
	real:	 C -> F
	imag:	 C -> F
	0: 	 C
	1: 	 C
	+:	 (C, C) -> C
	-:	 (C, C) -> C
	*:	 (C, C) -> C
	/:	 (C, C) -> C
    == add
	macro R == Record(real: F, imag: F)

	import from R, F

	complex(r: F, i: F): C == [r, i] pretend C
	real(a: C): F          == apply(a pretend R, real)
	imag(a: C): F          == apply(a pretend R, imag)

	0: C == complex(0, 0)
	1: C == complex(1, 0)
	
	(a: C) + (b: C):C ==
		complex(real a + real b,  imag a + imag b)
	(a: C) - (b: C):C ==
		complex(real a - real b,  imag a - imag b)
	(a: C) * (b: C):C ==
		complex(real a * real b - imag a * imag b,
	                real a * imag b + imag a * real b)
	(a: C) / (b: C):C ==
		d: F == real b * real b  + imag b * imag b
		complex((real a *real b  + imag a * imag b)/d,
			(imag a *real b  - real a * imag b)/d)
