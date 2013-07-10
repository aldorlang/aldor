----------------------------- sit_ugring.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{UnivariateGcdRing}
\History{Manuel Bronstein}{20/9/95}{created}
\Usage{\this: Category}
\Descr{\this~is the category of rings which export a gcd algorithm for
univariate polynomials over themselves.}
\begin{exports}
\category{\astype{GcdDomain}}\\
\asexp{gcdUP}:
& (P:\astype{UnivariatePolynomialAlgebra0}) $\to$ (P, P) $\to$ P & Gcd\\
\asexp{gcdUP!}:
& (P:\astype{UnivariatePolynomialAlgebra0}) $\to$ (P, P) $\to$ P & Gcd\\
\asexp{gcdquoUP}:
& (P:\astype{UnivariatePolynomialAlgebra0}) $\to$ (P, P) $\to$ (P,P,P) & Gcd\\
\end{exports}
#endif

define UnivariateGcdRing: Category == GcdDomain with {
	gcdUP: (P: UnivariatePolynomialAlgebra0 %) -> (P, P) -> P;
	gcdUP!: (P: UnivariatePolynomialAlgebra0 %) -> (P, P) -> P;
#if ALDOC
\aspage{gcdUP}
\astarget{\name!}
\Usage{ \name(P)(p, q) \\ \name!(P)(p, q) }
\Signature{(P: \astype{UnivariatePolynomialAlgebra0} \%) $\to$ (P, P)}{P}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & A polynomial type\\
{\em p,q} & P & Polynomials\\
}
\Retval{Both function return $\gcd(p, q)$. When \name! is used, the
storage used by $x_1$ and $x_2$ is allowed to be destroyed or reused,
so $p$ and $q$ are lost after this call.}
#endif
	gcdquoUP: (P: UnivariatePolynomialAlgebra0 %) -> (P, P) -> (P, P, P);
#if ALDOC
\aspage{gcdquoUP}
\Usage{ \name(P)(p, q) }
\Signature{(P: \astype{UnivariatePolynomialAlgebra0} \%) $\to$ (P,P)}{(P,P,P)}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & A polynomial type\\
{\em p,q} & P & Polynomials\\
}
\Retval{Returns $(g, y, z)$ such that $g = \gcd(p, q)$, $p = g y$ and
$q = g z$.}
#endif
	default {
		gcdUP!(P: UnivariatePolynomialAlgebra0 %):(P,P) -> P == {
			(p:P, q:P):P +-> gcdUP(P)(p, q);
		}

		gcdquoUP(P: UnivariatePolynomialAlgebra0 %):(P,P)->(P,P,P) == {
			gcdP := gcdUP P;
			(p:P, q:P):(P, P, P) +-> {
				g := gcdP(p, q);
				(g, quotient(p, g), quotient(q, g));
			}
		}
	}
}

