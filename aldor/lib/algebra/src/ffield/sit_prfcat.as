---------------------------- sit_prfcat.as --------------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{PrimeFieldCategory}
\History{Manuel Bronstein}{24/7/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category for prime fields,\ie fields of the form
$\ZZ / p \ZZ$ where $p \in \ZZ$ is a prime.}
\begin{exports}
\category{\astype{PrimeFieldCategory0}}\\
\category{\astype{FactorizationRing}}\\
\asexp{roots}:
&(P:POLY \%) $\to$ P $\to$ \astype{List} \builtin{Cross}(\%, \astype{Integer})&
In--field roots\\
\asexp{rootsSqfr}:
& (P:POLY \%) $\to$ P $\to$ \astype{List} \% & In--field roots\\
\end{exports}
\begin{aswhere}
POLY &==& \astype{UnivariatePolynomialAlgebra0}\\
\end{aswhere}
#endif

macro {
	Z == Integer;
	FR == FractionalRoot;
	RR == FractionalRoot Z;
	POLY == UnivariatePolynomialAlgebra0;
}

define PrimeFieldCategory:Category ==
	Join(PrimeFieldCategory0, FactorizationRing) with{
		rootsSqfr: (P:POLY %) -> P -> Generator %;
#if ALDOC
\aspage{rootsSqfr}
\Usage{\name~P\\ \name(P)(p)}
\Signature{(P:\astype{UnivariatePolynomialAlgebra0} \%)}
{P $\to$ \astype{Generator} \%}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & a polynomial type\\
{\em p} & P & a squarefree polynomial\\
}
\Retval{Returns a generator that produces all the roots of $p$, which
must be squarefree, in its coefficient field.}
#endif
	default {
		rationalRoots(P:POLY %):P -> Generator RR == integerRoots P;

		local ratroot(r:FR %):RR ==
			integralRoot(lift integralValue r, multiplicity r);

		roots(P:POLY %):P -> Generator FR % == {
			import from Boolean,PrimeFieldUnivariateFactorizer(%,P);
			(p:P):Generator FR % +-> roots(p, false);
		}

		rootsSqfr(P:POLY %):P -> Generator % == {
			import from Boolean, FR %;
			import from PrimeFieldUnivariateFactorizer(%, P);
			(p:P):Generator % +-> generate {
				for r in roots(p, true) repeat
					yield integralValue r;
			}
		}

		integerRoots(P:POLY %):P -> Generator RR == {
			rootP := roots P;
			(p:P):Generator RR +-> generate {
				for r in rootP p repeat yield ratroot r;
			}
		}

		factor(P:POLY %):P -> (%, Product P) == {
			import from PrimeFieldUnivariateFactorizer(%, P);
			(p:P):(%, Product P) +-> factor p;
		}
	}
}
