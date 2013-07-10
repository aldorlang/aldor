----------------------------- sit_ufalg.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

macro Z == Integer;

#if ALDOC
\thistype{UnivariateFreeLinearArithmeticType}
\History{Manuel Bronstein}{22/5/95}{created}
\History{Thom Mulders}{27/5/97}{added partial add!}
\History{Manuel Bronstein}{14/4/2000}{allowed infinite dimensional algebras}
\History{Manuel Bronstein}{26/4/2001}
{extracted FreeLinearCombinationType and FreeLinearArithmeticType,
 changed name from UnivariateFreeAlgebra to UnivariateFreeLinearArithmeticType}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~is a common category for commutative and noncommutative
univariate polynomials and power series with coefficients in an arbitrary
arithmetic system R and with respect to an arbitrary basis $(P_n)_{n \ge 0}$.
Its elements are not assumed to have finite support, so this type
cannot be asserted to be an \altype{RRing}~R even when R is a \altype{Ring}.}
\begin{exports}
\category{\altype{IndexedFreeLinearArithmeticType}(R, Z)}\\
\alexp{add!}: & (\%, R, Z) $\to$ \% & In-place addition of a term\\
\alexp{add!}: & (\%, R, Z, \%) $\to$ \% & In-place product and sum\\
\alexp{apply}: & (\%, TREE) $\to$ TREE & Conversion to an expression tree\\
\alexp{apply}:
& (OUT, \%, \altype{Symbol}) $\to$ OUT & Write an element to a port\\
\alexp{coefficients}:
& \% $\to$ \altype{Generator} R & Iterate over all the coefficients\\
\alexp{monom}: & $\to$ \% & Term with degree 1 and coefficient 1\\
\alexp{setCoefficient!}: & (\%, Z, R) $\to$ \% &
In-place replacement of a coefficient\\
\alexp{shift}: & (\%, Z) $\to$ \% & Exponent translation\\
\alexp{shift!}: & (\%, Z) $\to$ \% & In-place exponent translation\\
\alexp{truncate}: & (\%, Z) $\to$ \% & Truncation\\
\alexp{truncate!}: & (\%, Z) $\to$ \% & In-place truncation\\
\end{exports}
\begin{alwhere}
OUT &==& \altype{TextWriter}\\
TREE &==& \altype{ExpressionTree}\\
Z &==& \altype{Integer}\\
\end{alwhere}
#endif

define UnivariateFreeLinearArithmeticType(R:Join(ArithmeticType, ExpressionType)):
	Category == IndexedFreeLinearArithmeticType(R, Z) with {
	apply: (%, ExpressionTree) -> ExpressionTree;
	apply: (TextWriter, %, Symbol) -> TextWriter;
#if ALDOC
\alpage{apply}
\Usage{\name(p, t)\\p~t\\ \name(port, p, x)\\ port(p, x)}
\Signatures{
\name: & (\%, \altype{ExpressionTree}) $\to$ \altype{ExpressionTree}\\
\name: & (\altype{TextWriter}, \%, \altype{Symbol}) $\to$ \altype{TextWriter}\\
}
\Params{
{\em p} & \% & A polynomial or series\\
{\em t} & \altype{ExpressionTree} & An expression tree\\
{\em x} & \altype{Symbol} & A name for the variables\\
{\em port} & \altype{TextWriter} & An output port\\
}
\Descr{\name(p, t) returns p as an expression tree,
using t as root variable name, while
\name(port, p, x) sends p to port using x as root variable name,
and returns the output port afterwards.}
\begin{alex}
\begin{ttyout}
import from Integer, DenseUnivariatePolynomial Integer;

p := term(1, 3) + term(2, 1) - term(1, 0);  -- p = x^3 + 2 x - 1
stdout(map((n:Integer):Integer +-> n + n)(p), -"x");
\end{ttyout}
writes
\begin{ttyout}
       2*x^3+4*x-2
\end{ttyout}
to the standard stream {\tt stdout}.
\end{alex}
#endif
	coefficients: % -> Generator R;
#if ALDOC
\alpage{coefficients}
\Usage{for c in \name~m repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} R}
\Params{ {\em m} & \% & An element of the module\\ }
\Retval{Returns a generator that produces all the coefficients of $m$,
including the ones which are 0.}
\alseealso{\alfunc{UnivariateFreeRing}{generator},
\alfunc{UnivariateFreeRing}{terms}}
#endif
	monom: %;
#if ALDOC
\alpage{monom}
\Usage{\name}
\alconstant{\%}
\Retval{Returns $P_1$, \ie~the term with degree 1 and coefficient 1.}
\alseealso{\alfunc{IndexedFreeLinearCombinationType}{monomial}}
#endif
	shift: (%, Z) -> %;
	shift!: (%, Z) -> %;
#if ALDOC
\alpage{shift}
\altarget{\name!}
\Usage{\name(p, m)\\ \name!(p, m)}
\Signature{(\%,\altype{Integer})}{\%}
\Params{
{\em p} & \% & A polynomial or series\\
{\em m} & \altype{Integer} & The amount to shift\\
}
\Retval{Returns
$$
\sum_{i \ge \max(0,-m)} a_i P_{i+m}
$$
where $p = \sum_{i \ge 0} a_i P_i$.}
\Remarks{When using \name!, the storage used by {\em p} is allowed to be
destroyed or reused, so {\em p} is lost after this call.
This may cause {\em p} to be destroyed, so do not use this unless
{\em p} has been locally allocated, and is thus guaranteed not to share space
with other series or polynomials.}
#endif
	truncate: (%, Z) -> %;
	truncate!: (%, Z) -> %;
#if ALDOC
\alpage{truncate}
\altarget{\name!}
\Usage{\name(p, m)\\ \name!(p, m)}
\Signature{(\%,\altype{Integer})}{\%}
\Params{
{\em p} & \% & A polynomial or series\\
{\em m} & \altype{Integer} & The truncation order\\
}
\Retval{Returns the truncation of {\em p} at order {\em m}, \ie
$$
\sum_{i=0}^{m-1} a_i P_i
$$
where $p = \sum_{i \ge 0} a_i P_i$.}
\Remarks{When using \name!, the storage used by {\em p} is allowed to be
destroyed or reused, so {\em p} is lost after this call.
This may cause {\em p} to be destroyed, so do not use this unless
{\em p} has been locally allocated, and is thus guaranteed not to share space
with other series or polynomials.}
#endif
	default {
		coerce(c:R):%		== { import from Z; term(c, 0); }
		monom:%			== { import from Z; monomial 1; }
		shift!(p:%, n:Z):%	== shift(p, n);
		truncate!(p:%, n:Z):%	== truncate(p, n);

		truncate(p:%, n:Z):% == {
			assert(n >= 0);
			q:% := 0;
			for i in prev(n)..0 by -1 repeat
				q := add!(q, coefficient(p, i), i);
			q;
		}

		apply(port:TextWriter, p:%, v:Symbol):TextWriter == {
			import from ExpressionTree;
			-- tex(port, p extree v);
			infix(port, p extree v);
		}
	}
}
