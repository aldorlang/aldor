----------------------------- sit_zring.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z  == Integer;
	RR == FractionalRoot Z;
	GEN== Generator;
}

#if ALDOC
\thistype{RationalRootRing}
\History{Manuel Bronstein}{10/12/96}{created}
\Usage{\this: Category}
\Descr{\this~is the category of gcd domains that export an algorithm for
finding the rational roots of univariate polynomials over themselves.}
\begin{exports}
\category{\astype{Ring}}\\
\asexp{integerRoots}:
& (P:POL \%) $\to$ P $\to$ \astype{Generator} FR \astype{Integer} &
Integer roots\\
\asexp{rationalRoots}:
& (P:POL \%) $\to$ P $\to$ \astype{Generator} FR \astype{Integer}&
Rational roots\\
\end{exports}
\begin{aswhere}
FR &==& \astype{FractionalRoot}\\
POL &==& \astype{UnivariatePolynomialAlgebra0}\\
\end{aswhere}
#endif

define RationalRootRing: Category == Ring with {
	integer: (P:UnivariatePolynomialAlgebra0 %) -> % -> Partial Z;
	integer?: (P:UnivariatePolynomialAlgebra0 %) -> % -> Boolean;
	rational: (P:UnivariatePolynomialAlgebra0 %) -> % ->Partial Cross(Z,Z);
	rational?: (P:UnivariatePolynomialAlgebra0 %) -> % -> Boolean;
	integerRoots: (P:UnivariatePolynomialAlgebra0 %) -> P -> GEN RR;
	rationalRoots: (P:UnivariatePolynomialAlgebra0 %) -> P -> GEN RR;
#if ALDOC
\aspage{integerRoots,rationalRoots}
\astarget{integerRoots}
\astarget{rationalRoots}
\Usage{ integerRoots(P)(p)\\ rationalRoots(P)(p) }
\Signature{(P: POL \%)}{P $\to$ \astype{Generator} \astype{RationalRoot}}
\begin{aswhere}
POL &==& \astype{UnivariatePolynomialAlgebra0}\\
\end{aswhere}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & A polynomial type\\
{\em p} & P & A polynomial\\
}
\Retval{Return $[(r_1,e_1),\dots,(r_n,e_n)]$ where the $r_i$'s are
the integer or rational roots of $p$ and have multiplicity $e_i$.}
#endif
	default {
		integer?(P:UnivariatePolynomialAlgebra0 %):% -> Boolean == {
			import from Boolean, Partial Z;
			f := integer P;
			(r:%):Boolean +-> ~failed? f r;
		}

		integer(P:UnivariatePolynomialAlgebra0 %):% -> Partial Z == {
			import from RR;
			zRoots := integerRoots P;
			x:P := monom;
			(r:%):Partial Z +-> {
				for z in zRoots(x - r::P) repeat
					return [integralValue z];
				failed;
			}
		}

		rational?(P:UnivariatePolynomialAlgebra0 %):% -> Boolean == {
			import from Boolean, Partial Cross(Z, Z);
			f := rational P;
			(r:%):Boolean +-> ~failed? f r;
		}

		rational(P:UnivariatePolynomialAlgebra0 %):
			% -> Partial Cross(Z, Z) == {
			import from RR;
			rRoots := rationalRoots P;
			x:P := monom;
			(r:%):Partial Cross(Z, Z) +-> {
				for z in rRoots(x-r::P) repeat return [value z];
				failed;
			}
		}

		integerRoot(P:UnivariatePolynomialAlgebra0 %):P -> GEN RR == {
			rRoots := rationalRoots P;
			(p:P):GEN RR +-> generate {
				import from RR;
				for z in rRoots p | integral? z repeat yield z;
			}
		}
	}
}

