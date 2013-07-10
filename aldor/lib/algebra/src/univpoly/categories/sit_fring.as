----------------------------- sit_fring.as --------------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z == Integer;
	FR == FractionalRoot;
	POL == UnivariatePolynomialAlgebra0;
}

#if ALDOC
\thistype{FactorizationRing}
\History{Manuel Bronstein}{27/4/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of rings that export an algorithm for
factoring univariate polynomials over themselves into irreducibles.}
\begin{exports}
\category{\astype{GcdDomain}}\\
\category{\astype{RationalRootRing}}\\
\asexp{factor}:
& (P:POL \%) $\to$ P $\to$ (\%, Product P) & Factorization into irreducibles\\
\asexp{fractionalRoots}:
& (P:POL \%) $\to$ P $\to$ \astype{Generator} FR \% &
Roots in the fraction field\\
\asexp{roots}:
& (P:POL \%) $\to$ P $\to$ \astype{Generator} FR \% &
Roots in the coefficient ring\\
\end{exports}
\begin{aswhere}
FR &==& \astype{FractionalRoot}\\
POL &==& \astype{UnivariatePolynomialAlgebra0}\\
\end{aswhere}
#endif

define FactorizationRing:Category== Join(GcdDomain, RationalRootRing) with {
	factor: (P:POL %) -> P -> (%, Product P);
#if ALDOC
\aspage{factor}
\Usage{ \name(P)(p) }
\Signature{(P: \astype{UnivariatePolynomialAlgebra0} \%)}
{P $\to$ (\%, \astype{Product} P)}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & A polynomial type\\
{\em p} & P & A polynomial\\
}
\Retval{Returns $(c, p_1^{e_1} \cdots p_n^{e_n})$ such that
each $p_i$ is irreducible, the $p_i$'s have no common factors, and
$$
p = c\;\prod_{i=1}^n p_i^{e_i}\,.
$$
}
#endif
	fractionalRoots: (P:POL %) -> P -> Generator FR %;
	roots: (P:POL %) -> P -> Generator FR %;
#if ALDOC
\aspage{fractionalRoots,roots}
\astarget{fractionalRoots}
\astarget{roots}
\Usage{ fractionalRoots(P)(p)\\ roots(P)(p) }
\Signature{(P: POL \%)}{P $\to$ \astype{Generator} \builtin{FractionalRoot} \%}
\begin{aswhere}
POL &==& \astype{UnivariatePolynomialAlgebra0}\\
\end{aswhere}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & A polynomial type\\
{\em p} & P & A polynomial\\
}
\Retval{Returns a generator that produces all the roots $p$
either in the ring or in its fraction field.}
#endif
	default {
		roots(P:POL %):P -> Generator FR % == {
			froots := fractionalRoots P;
			(p:P):Generator FR % +-> generate {
				import from FR %;
				for z in froots p | integral? z repeat yield z;
			}
		}

		fractionalRoots(P:POL %):P -> Generator FR % == {
			fact := factor P;
			(p:P):Generator FR % +-> generate {
				import from Z, FR %, Product P;
				(c, fp) := fact p;
				for term in fp repeat {
					(q, n) := term;
					if one? degree q then {
						num := - coefficient(q, 0);
						den := leadingCoefficient q;
						yield fractionalRoot(num,den,n);
					}
				}
			}
		}
	}
}

