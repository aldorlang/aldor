------------------------------- sit_ring.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

macro Z == Integer;

#if ALDOC
\thistype{Ring}
\History{Manuel Bronstein}{21/11/94}{created}
\Usage{\this: Category}
\Descr{ \this~is the category of rings, not necessarily commutative.}
\begin{exports}
\category{\altype{AbelianGroup}}\\
\category{\altype{ArithmeticType}}\\
\category{\altype{Monoid}}\\
\alexp{characteristic}: & \altype{Integer} & characteristic\\
\alexp{coerce}: & \altype{Integer} $\to$ \% & embedding of the integers\\
\alexp{factorial}:
& (\%, \%, \altype{MachineInteger}) $\to$ \% & Generalized factorial\\
\alexp{random}: & () $\to$ \% & Get a random element\\
\end{exports}
#endif

define Ring: Category == Join(AbelianGroup, ArithmeticType, Monoid) with {
	characteristic: Z;
#if ALDOC
\alpage{characteristic}
\Usage{\name}
\alconstant{\altype{Integer}}
\Retval{Returns the characteristic of the ring.}
#endif
	coerce: Z -> %;
#if ALDOC
\alpage{coerce}
\Usage{\name~n\\n::\%}
\Signature{\altype{Integer}}{\%}
\Params{ {\em n} & \altype{Integer} & an integer }
\Retval{Returns $n$ seen as an element of the ring.}
#endif
	factorial: (%, %, MachineInteger) -> %;
#if ALDOC
\alpage{factorial}
\Usage{\name(a, s, n)}
\Signature{(\%, \%, \altype{MachineInteger})}{\%}
\Params{
{\em a, s} & \% & Elements of the ring\\
{\em n} & \altype{MachineInteger} & A nonnegative integer\\
}
\Retval{Returns the generalized factorial $\prod_{i=0}^{n-1} (a + i s)$.}
#endif
	random: () -> %;
#if ALDOC
\alpage{random}
\Usage{\name()}
\Signature{()}{\%}
\Retval{Returns a random element.}
#endif
	default {
		(x:%)^(n:MachineInteger):%	== x^(n::Z);
		(n:Z) * (x:%):%			== n::% * x;
		random():%			== random()$Z :: %;
		(x:%)^(n:Z):% == binaryExponentiation(x, n)$BinaryPowering(%,Z);

		factorial(a:%, s:%, n:MachineInteger):% == {
			assert(n >= 0);
			zero? n => 1;
			one? n => a;
			zero? a => 0;
			zero? s => a^n;
			b := a;
			for i in 2..n repeat {
				a := a + s;
				b := times!(b, a);
			}
			b;
		}
	}
}
