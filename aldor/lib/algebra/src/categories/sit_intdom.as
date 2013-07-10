------------------------------- sit_intdom.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{IntegralDomain}
\History{Manuel Bronstein}{21/11/94}{created}
\History{Manuel Bronstein}{24/09/96}{added order}
\History{Manuel Bronstein}{06/06/2002}{moved quotients from ComRing to here}
\Usage{\this: Category}
\Descr{\this~is the category of commutative integral domains.}
\begin{exports}
\category{\altype{CommutativeRing}}\\
\alexp{divDiffProd}: & (\%, \%, \%, \%, \%) $\to$ \% &
Combined product and quotient\\
\alexp{divSumProd}: & (\%, \%, \%, \%, \%, \%, \%) $\to$ \% &
Combined product and quotient\\
\alexp{exactQuotient}: & (\%, \%) $\to$ \altype{Partial} \% & Exact quotient\\
\alexp{order}: & \% $\to$  \% $\to$ \altype{Integer} & Order of divisibility\\
\alexp{orderquo}: & \% $\to$  \% $\to$ (\altype{Integer}, \%) &
Order of divisibility and quotient\\
\alexp{quotient}: & (\%, \%) $\to$ \% & Exact quotient\\
\alexp{quotient!}: & (\%, \%) $\to$ \% & In-place exact quotient\\
\alexp{quotientBy}: & \% $\to$ (\% $\to$ \%) & Exact quotient procedure\\
\alexp{quotientBy!}:
& \% $\to$ (\% $\to$ \%) & In-place exact quotient procedure\\
\end{exports}
#endif

define IntegralDomain: Category == CommutativeRing with {
	divDiffProd: (%,%,%,%,%) -> %;
#if ALDOC
\alpage{divDiffProd}
\Usage{\name(a1, a2, b1, b2, q)}
\Signature{(\%, \%, \%, \%, \%)}{\%}
\Params{{\em a1, a2, b1, b2, q} & \% & Elements of the ring}
\Retval{\name(a1, a2, b1, b2, q) returns $(a1\,a2 - b1\,b2)/q$.}
#endif
	divSumProd: (%,%,%,%,%,%,%) -> %;
#if ALDOC
\alpage{divSumProd}
\Usage{\name(a1, a2, b1, b2, c1, c2, q)}
\Signature{(\%, \%, \%, \%, \%, \%, \%)}{\%}
\Params{{\em a1, a2, b1, b2, c1, c2, q} & \% & Elements of the ring}
\Retval{
\name(a1, a2, b1, b2, c1, c2, q) returns $(a1\,a2 + b1\,b2 + c1\,c2)/q$.
}
#endif
	exactQuotient: (%, %) -> Partial %;
#if ALDOC
\alpage{exactQuotient}
\Usage{\name(x, y)}
\Signature{(\%, \%)}{\altype{Partial} \%}
\Params{
{\em x} & \% & The numerator\\
{\em y} & \% & The denominator\\
}
\Retval{Returns the unique \emph{q} such that $x = q\, y$ if such a \emph{q}
exists, \failed otherwise.}
\alseealso{\alexp{quotient}}
#endif
	order: % -> % -> Integer;
#if ALDOC
\alpage{order}
\Usage{\name~a\\ \name(a)(b)}
\Signature{\% $\to$ \%}{\altype{Integer}}
\Params{
{\em a} & \% & A nonunit element of the domain\\
{\em b} & \% & A nonzero element of the domain\\
}
\Retval{\name(a)(b) returns the unique nonnegative integer $n = \nu_a(b)$
such that $a^n \mid b$ and $a^{n+1} \nodiv b$, while \name(a) returns the map
$b \to \nu_a(b)$.}
\Remarks{\name(a) makes some precalculations based on a, so if you need to
use \name(a)(b) several times with the same a and different b's, it is
more efficient to compute \name(a) once and assign it, as in the example
below. In addition, if you need to compute $b / a^{\nu_a(b)}$, then it
is more efficient to use the {\tt orderquo} function.}
\begin{asex}
The following function computes the orders at $x^2 + 1$ of
a list of polynomials $l := [p_1,\dots,p_n]$:
\begin{ttyout}
orders(l:List DenseUnivariatePolynomial Integer):List Integer == {
    import from DenseUnivariatePolynomial Integer;
    nu := order(monom * monom + 1);    -- order function at x^2 + 1
    [nu(p) for p in l];
}
\end{ttyout}
\end{asex}
\alseealso{\alexp{orderquo}}
#endif
	orderquo: % -> % -> (Integer, %);
#if ALDOC
\alpage{orderquo}
\Usage{\name~a\\ (n, c) := \name(a)(b);}
\Signature{\% $\to$ \%}{(\altype{Integer}, \%)}
\Params{
{\em a} & \% & A nonunit element of the domain\\
{\em b} & \% & A nonzero element of the domain\\
}
\Retval{\name(a)(b) returns $(n, c)$ such that $b = c a^n$ and $a \nodiv c$,
while \name(a) returns the map $b \to$~\name(a)(b).}
\Remarks{\name(a) makes some precalculations based on a, so if you need to
use \name(a)(b) several times with the same a and different b's, it is
more efficient to compute \name(a) once and assign it.}
\alseealso{\alexp{order}}
#endif
	quotient: (%, %) -> %;
	quotient!: (%, %) -> %;
	quotientBy: % -> (% -> %);
	quotientBy!: % -> (% -> %);
#if ALDOC
\alpage{quotient,quotientBy}
\altarget{quotient}
\altarget{quotient!}
\altarget{quotientBy}
\altarget{quotientBy!}
\Usage{quotient(x, y)\\ quotient!(x, y)\\ quotientBy(y)\\ quotientBy(y)(x)\\
quotientBy!(y)\\ quotientBy!(y)(x)}
\Signatures{
quotient,quotient!: & (\%, \%) $\to$ \%\\
quotientBy, quotientBy!: & \% $\to$ \% $\to$ \%\\
}
\Params{
{\em x} & \% & The numerator\\
{\em y} & \% & The denominator\\
}
\Retval{quotient(x,y) and quotient!(x,y)
return the unique \emph{q} such that $x = q\, y$ if such a \emph{q} exists,
while quotientBy(y) returns the map $x \to \mbox{quotient}(x,y)$
and quotientBy!(y) returns the map $x \to \mbox{quotient!}(x,y)$.
When using quotient! or quotientBy!, the storage used by {\em x}
is allowed to be destroyed or reused, so {\em x} is lost after this call.}
\Remarks{Use those functions only when it is guaranteed that
{\em y} divides {\em x} exactly in the ring.
If there is a nonzero remainder, this function may produce a wrong
quotient instead of an error, so use \alexp{exactQuotient} when
there is the possibility of a nonzero remainder. When however {\em y} is known
to divide {\em x} exactly, then {\tt quotient}
can have better efficiency than the other divisions.}
\alseealso{\alexp{exactQuotient}}
#endif
	default {
		reciprocal(x:%):Partial(%)	== exactQuotient(1, x);
		quotient!(x:%, y:%):%		== quotient(x, y);

		order(a:%)(b:%):Integer == {
			(n, c) := orderquo(a)(b);
			n;
		}

		orderquo(a:%)(b:%):(Integer, %) == {
			import from Boolean, Partial %;
			assert(~zero? a and ~zero? b and ~unit? a);
			n:Integer := 0;
			while ~failed?(u := exactQuotient(b, a)) repeat {
				n := next n; b := retract u;
			}
			(n, b);
		}

		quotientBy(l:%):(%->%) == {
			import from Partial %;
			one? l => (a:%):% +-> a;
			failed?(u := reciprocal l) => (a:%):% +-> quotient(a,l);
			retru := retract u;
			(a:%):% +-> retru * a;
		}

		quotientBy!(l:%):(%->%) == {
			import from Partial %;
			one? l => (a:%):% +-> a;
			failed?(u:= reciprocal l) => (a:%):% +-> quotient!(a,l);
			retru := retract u;
			(a:%):% +-> times!(a, retru);
		}

		quotient(x:%, y:%):% == {
			import from Partial %;
			retract exactQuotient(x, y);
		}

		divDiffProd(a1:%, a2:%, b1:%, b2:%, q:%):% ==
			quotient(a1 * a2 - b1 * b2, q);

		divSumProd(a1:%, a2:%, b1:%, b2:%, c1:%, c2:%, q:%):% ==
			quotient(a1 * a2 + b1 * b2 + c1 * c2, q);
	}
}
