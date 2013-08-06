-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O
--> testrun -O -l axllib 

#include "axllib.as"
#pile

macro
	C == ComplexDoubleFloat
	F == DoubleFloat

inline from DoubleFloat

ComplexDoubleFloat: with
	complex: (F,F) -> %
	real:	 % -> F
	imag:	 % -> F
	0: 	 %
	1: 	 %
	+:	 (%, %) -> %
	-:	 (%, %) -> %
	*:	 (%, %) -> %
	/:	 (%, %) -> %
	<<:      (TextWriter, %) -> TextWriter

    == add
	macro R == Record(real: F, imag: F)
	import from F
	import from R

	complex(r: F, i: F): % == [r, i] pretend %
	real(a: %): F          == apply(a pretend R, real)
	imag(a: %): F          == apply(a pretend R, imag)

	0: % == complex(0, 0)
	1: % == complex(1, 0)
	
	(a: %) + (b: %):% ==
		complex(real a + real b,  imag a + imag b)
	(a: %) - (b: %):% ==
		complex(real a - real b,  imag a - imag b)
	(a: %) * (b: %):% ==
		complex(real a * real b - imag a * imag b,
	                real a * imag b + imag a * real b)
	(a: %) / (b: %):% ==
		d: F == real b * real b  + imag b * imag b
		complex((real a *real b  + imag a * imag b)/d,
			(imag a *real b  - real a * imag b)/d)

	(p: TextWriter) << (z: %): TextWriter ==
		import from String
		p << real z << " + " << imag z << " %i"

foo(): () ==
  import from ComplexDoubleFloat
  import from DoubleFloat

  print << complex(1.0, 2.0)                     << newline
  print << complex(2.0, 3.0) / complex(1.0, 4.0) << newline

foo()
