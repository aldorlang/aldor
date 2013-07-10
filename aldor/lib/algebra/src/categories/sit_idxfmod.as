----------------------------- sit_idxfmod.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) Marc Moreno Maza 2001
-- Copyright (c) INRIA 2001
-- Copyright (c) LIFL 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{IndexedFreeModule}
\History{Manuel Bronstein/Marc Moreno Maza}{26/4/2001}
{merged from UnivariateFreeFiniteAlgebra (Sumit) and MonoidRing (BasicMath)}
\Usage{\this (R,E): Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em E} & \altype{ExpressionType} & The index domain\\
	& \altype{TotallyOrderedType} &\\
}
\Descr{\this(R, E) is a category for free modules over an
arbitrary arithmetic system {\em R},
with respect to a linearly independent generating set {\em E}.
Its elements are assumed to have finite support.}
\begin{exports}
\category{\altype{FreeModule} R}\\
\category{\altype{IndexedFreeLinearCombinationType}(R, E)}\\
\alexp{degree}: & \% $\to$ E & degree\\
\alexp{generator}:
& \% $\to$ \altype{Generator} \builtin{Cross}(R, E) &
iterate through all the terms\\
\alexp{leadingMonomial}: & \% $\to$ \% & leading monomial\\
\alexp{leadingTerm}: & \% $\to$ (R, E) & leading term\\
\alexp{terms}:
& \% $\to$ \altype{Generator} \builtin{Cross}(R, E) &
iterate through all the terms\\
\alexp{trailingDegree}: & \% $\to$ E & trailing degree\\
\alexp{trailingMonomial}: & \% $\to$ \% & trailing monomial\\
\alexp{trailingTerm}: & \% $\to$ (R, E) & trailing term\\
\end{exports}
\begin{exports}
[if {\em R} has \altype{HashType} and {\em E} has \altype{HashType} then]
\category{\altype{HashType}}\\
\end{exports}
\begin{exports}
[if {\em R} has \altype{SerializableType}
and {\em E} has \altype{SerializableType} then]
\category{\altype{SerializableType}}\\
\end{exports}
#endif

define IndexedFreeModule(R:Join(ArithmeticType, ExpressionType),
			E:Join(TotallyOrderedType, ExpressionType)): Category ==
	Join(FreeModule R, IndexedFreeLinearCombinationType(R, E)) with {
		if R has HashType and E has HashType then HashType;
		if R has SerializableType and E has SerializableType then _
			SerializableType;
		degree: % -> E;
		trailingDegree: % -> E;
#if ALDOC
\alpage{degree,trailingDegree}
\altarget{degree}
\altarget{trailingDegree}
\Usage{degree~p\\ trailingDegree~p}
\Signature{\%}{E}
\Params{ {\em p} & \% & A nonzero element of the module\\ }
\Retval{degree(p) and trailingDegree(p) return respectively the degree
and trailing degree of $p$, \ie $e_n$ and $e_m$ where
$p = \sum_{i=m}^n a_i e_i$ and $a_n \ne 0 \ne a_m$.
When $p = 0$, both functions return an arbitrary value in $E$.}
\alseealso{\alfunc{FreeModule}{leadingCoefficient},\alexp{leadingMonomial},
\alfunc{FreeModule}{trailingCoefficient},\alexp{trailingMonomial}}
#endif
		generator: % -> Generator Cross(R, E);	-- decreasing
		terms: % -> Generator Cross(R, E);	-- increasing
#if ALDOC
\alpage{generator,terms}
\altarget{generator}
\altarget{terms}
\Usage{
for term in p repeat \{ (c, n) := term; \dots \}\\
for term in generator~p repeat \{ (c, n) := term; \dots \}\\
for term in terms~p repeat \{ (c, n) := term; \dots \}\\
}
\Signature{\%}{\altype{Generator} \builtin{Cross}(R, E)}
\Params{ {\em p} & \% & An element of the module\\ }
\Descr{Both functions allow an element of the module
to be iterated independently of its
representation. Both generators yield pairs of the form $(a, e)$,
with $a \ne 0$. The difference between the two is that
generator(p) yields the terms in decreasing exponents, while
terms(p) yields the terms in increasing exponents.}
\begin{asex}
\begin{ttyout}
import from Integer, DenseUnivariatePolynomial Integer;

x := monom;
p := x^3 + 2*x - 1;
for term in p repeat { (c, n) := term; print << c << "," << n << newline }
\end{ttyout}
writes
\begin{asoutput}
\> 1,3\\
\> 2,1\\
\> -1,0\\
\end{asoutput}
to the standard stream print.
\end{asex}
\alseealso{\alfunc{FreeLinearCombinationType}{coefficients}, \alexp{revert}}
#endif
		leadingTerm: % -> (R, E);
		trailingTerm: % -> (R, E);
		leadingMonomial: % -> %;
		trailingMonomial: % -> %;
#if ALDOC
\alpage{leadingMonomial,leadingTerm,trailingMonomial,trailingTerm}
\altarget{leadingMonomial}
\altarget{leadingTerm}
\altarget{trailingMonomial}
\altarget{trailingTerm}
\Usage{leadingMonomial~p\\ (r, e) := leadingTerm~p\\trailingMonomial~p\\
(r, e) := trailingTerm~p}
\Signatures{
leadingMonomial, trailingMonomial: & \% $\to$ \%\\
leadingTerm, trailingTerm: & \% $\to$ (R, E)\\
}
\Params{ {\em p} & \% & An element of the module\\ }
\Retval{
When $p = \sum_{i=m}^n a_i e_i$ and $a_n \ne 0 \ne a_m$,
then leadingMonomial(p) and trailingMonomial(p)
return respectively $e_n$ and $e_m$,
while then leadingTerm(p) and trailingTerm(p)
return respectively $(a_n, e_n)$ and $(a_m, e_m)$.
When $p = 0$, leadingMonomial(0) and trailingMonomial(0)
both return $0$, while leadingTerm(p) and trailingTerm(p)
are undefined.}
\alseealso{\alexp{degree},\alfunc{FreeModule}{leadingCoefficient},
\alfunc{FreeModule}{trailingCoefficient},\alexp{trailingDegree}}
#endif
		default {
			leadingMonomial(p:%):% == {
				zero? p => 0;
				monomial degree p;
			}

			trailingMonomial(p:%):% == {
				zero? p => 0;
				monomial trailingDegree p;
			}
		}
}
