----------------------------- sit_sercat.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	V == Vector;
	M == DenseMatrix;
	UPC == UnivariatePolynomialAlgebra;
	PR == DenseUnivariatePolynomial R;
	PC == % pretend UnivariateTaylorSeriesType R;
}

#if ALDOC
\thistype{UnivariateTaylorSeriesType}
\History{Manuel Bronstein}{6/6/2000}{created}
\History{Manuel Bronstein}{22/7/2003}{added dot}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R is the category of univariate Taylor series with coefficients
in R.}
\begin{exports}
\category{\altype{UnivariateFreeLinearArithmeticType} R}\\
\alexp{degree}:
& \% $\to$ \altype{Partial} \altype{Integer} & upper bound on the degree\\
\alfunc{Sequence}{dot}:
& \altype{Vector} R $\to$ \altype{Vector} \% $\to$ \% &
linear combination\\
\alexp{finite?}:
& \% $\to$ \altype{Boolean} & check whether the support is finite\\
\alexp{series}: & \altype{Sequence} R $\to$ \% & creation of a series\\
\end{exports}
\begin{alwhere}
UPC &== \altype{UnivariatePolynomialAlgebra}\\
\end{alwhere}
\begin{exports}[if R has \altype{CommutativeRing} then]
\alexp{differentiate}: & \% $\to$ \% & Differentiation\\
& (\%, \altype{Integer}) $\to$ \% & \\
\alfunc{CommutativeRing}{reciprocal}: & \% $\to$ \altype{Partial} \% & Inverse\\
\end{exports}
\begin{exports}
[if R has \altype{CommutativeRing} and R has \altype{RittRing} then]
\alexp{integrate}: & \% $\to$ \% & Integration\\
 & (\%, \altype{Integer}) $\to$ \% & \\
\end{exports}
\begin{exports}[if R has \altype{FloatType} then]
\alfunc{CommutativeRing}{reciprocal}: & \% $\to$ \altype{Partial} \% & Inverse\\
\end{exports}
#endif

define UnivariateTaylorSeriesType(R:Join(ArithmeticType, ExpressionType)):
	Category == UnivariateFreeLinearArithmeticType R with {
		degree: % -> Partial Z;
		dot: V R -> V % -> %;
		finite?: % -> Boolean;
#if ALDOC
\alpage{degree,finite?}
\altarget{degree}
\altarget{finite?}
\Usage{degree~s\\finite?~s}
\Signatures{
degree: & \% $\to$ \altype{Partial} \altype{Integer} \\
finite?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em s} & \% & a series\\ }
\Retval{finite?(s) returns \true if s is known
to have finite support and \false otherwise, while
degree(s) returns $[n]$ if s is known
to have finite support and $\deg(s) \le n = 0$, \failed otherwise.}
#endif
		if R has CommutativeRing then {
			differentiate: % -> %;
			differentiate: (%, Integer) -> %;
			if R has RittRing then {
				integrate: % -> %;
				integrate: (%, Integer) -> %;
			}
#if ALDOC
\alpage{differentiate,integrate}
\altarget{differentiate}
\altarget{integrate}
\Usage{differentiate~s\\ differentiate(s, n)\\ integrate~s}
\Signatures{
\name: & \% $\to$ \%\\
\name: & (\%, \altype{Integer}) $\to$ \%\\
}
\Params{
{\em s} & \% & a series\\
{\em n} & \altype{Integer} & The order of differentiation or integration\\
}
\Retval{differentiate(s), differentiate(s, n), integrate(s) and
integrate(s, n) return respectively $ds/dx$, $d^n s/dx^n$,
$\int s(x)dx$ and $\int \dots \int s(x) dx^n$.
}
#endif
			reciprocal: % -> Partial %;
		}
		if R has FloatType then {
			reciprocal: % -> Partial %;
		}
		series: Sequence R -> %;
#if ALDOC
\alpage{series}
\Usage{\name~s}
\Signature{\altype{Sequence} R}{\%}
\Params{ {\em s} & \altype{Sequence} R & a coefficient sequence\\ }
\Retval{Returns $s$ viewed as a series.}
#endif
		default {
			if R has CommutativeRing then {
				if R has RittRing then {
					integrate(s:%):% == {
						import from Z;
						integrate(s, 1);
					}
				}

				differentiate(s:%):% == {
					import from Z;
					differentiate(s, 1);
				}

				local recipComring(s:%):Partial % == {
					import from Z, R, Partial R;
					s0 := coefficient(s, 0);
					failed?(u := reciprocal s0) => failed;
					[recip(s, retract u)];
				}
			}

			if R has FloatType then {
				local recipFloat(s:%):Partial % == {
					import from Z, R;
					zero?(s0 := coefficient(s,0)) => failed;
					[recip(s, 1 / s0)];
				}
			}

			-- REQUIRED BECAUSE COMPILER DOES NOT ACCEPT
			-- 2 SEPARATE CONDITIONAL DEFS FOR reciprocal
			-- local comring?:Boolean == R has CommutativeRing;
			-- local float?:Boolean	== R has FloatType;
			reciprocal(s:%):Partial % == {
				-- TEMPORARY: WOULD LIKE TO CACHE THE TESTS
				-- comring? => recipComring s;
				R has CommutativeRing => recipComring s;
				-- float? => recipFloat s;
				R has FloatType => recipFloat s;
				never;
			}

			-- computes s^{-1} given that s0 t0 = 1
			local recip(s:%, t0:R):% == {
				import from Stream R, Sequence R;
				series sequence [recipGen(s, t0)];
			}

			-- PROBABLY CAUSES TROUBLE IN CHARACTERISTIC 2
			-- computes s^{-1} via Newton given that s0 t0 = 1
			-- the usual iteration is t_{n+1} = 2t_n - s t_n^2
			-- to use + rather than -, we store g_n = -t_n, and
			-- the iteration becomes g_{n+1} = s g_n^2 + 2 g_n
			-- we must then yield the coefficients of -g
			local recipGen(s:%, t0:R):Generator R == generate {
				import from Z, PR,
				   UnivariateTaylorSeriesType2Poly(R,PC,PR);
				yield t0;
				g := (-t0)::PR;
				n:Z := 1;	-- s g = -1 mod x^n
				two:R := 1+1;
				repeat {
					m := 2 * n;
					sg2 := truncate!(truncate(s,m) * g^2,m);
					g := add!(sg2, two, g);
					for i in n..prev m repeat
						yield(- coefficient(g, i));
					n := m;
				}
			}

			(s:%)^(n:I):% == {
				import from BinaryPowering(%, I);
				binaryExponentiation(s, n);
			}
		}
}

#if ALDOC
\thistype{UnivariateTaylorSeriesType2Poly}
\History{Manuel Bronstein}{30/11/2001}{created}
\History{Manuel Bronstein}{30/6/2003}{added tryExpandFraction,monicNewtonSeries}
\History{Manuel Bronstein}{22/7/2003}{added polynomials}
\Usage{import from \this(R, RXX, RX)}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em RXX} & \altype{UnivariateTaylorSeriesType} R & A series type over R\\
{\em RX} & \altype{UnivariatePolynomialAlgebra} R & A polynomial type over R\\
}
\Descr{\this(R, RXX, RX) provides conversion tools between polynomials
and series.}
\begin{exports}
\alexp{expand}: & R $\to$ RX $\to$ RXX & series expansion at a point\\
\alexp{truncate}: & (RXX, \altype{Integer}) $\to$ RX & truncation of a series\\
\end{exports}
\begin{exports}[if R has \altype{Field} then]
\alexp{expandFraction}:
& R $\to$ \altype{Fraction} RX $\to$ (\altype{Integer}, RXX) &
series expansion at a point\\
\end{exports}
\begin{exports}[if R has \altype{FloatType} then]
\alexp{expandFraction}:
& R $\to$ \altype{Fraction} RX $\to$ (\altype{Integer}, RXX) &
series expansion at a point\\
\end{exports}
\begin{exports}[if R has \altype{CommutativeRing} then]
\alexp{monicNewtonSeries}: & RX $\to$ RXX & Newton series\\
\alexp{tryExpandFraction}:
& R $\to$ (RX, RX) $\to$ (\altype{Integer}, \altype{Partial} RXX) &
try series expansion\\
\end{exports}
\begin{exports}[if R has \altype{IntegralDomain} then]
\alexp{polynomials}:
& (V RXX, \altype{Integer}) $\to$ (V RX, M R) & interpolation\\
& (V RXX, \altype{Integer}, \altype{Integer}) $\to$ (V RX, M R) &\\
\end{exports}
\begin{alwhere}
V &==& \altype{Vector}\\
M &==& \altype{DenseMatrix}\\
\end{alwhere}
#endif

UnivariateTaylorSeriesType2Poly(R:Join(ArithmeticType, ExpressionType),
	RXX: UnivariateTaylorSeriesType R,
	RX: UnivariatePolynomialAlgebra0 R): with {
		expand: R -> RX -> RXX;
#if ALDOC
\alpage{expand}
\Usage{\name~a\\ \name(a)(p)}
\Signature{R}{RX $\to$ RXX}
\Params{
{\em a} & R & the expansion point\\
{\em p} & RX & a polynomial\\
}
\Retval{\name(a)(p) returns the series expansion of {\em p} around {\em a}.}
\alseealso{\alexp{expandFraction}}
#endif
		if R has Field then {
			expandFraction:R -> Fraction RX -> (Z,RXX);
		}
		if R has FloatType then {
			expandFraction: R -> Fraction RX -> (Z, RXX);
		}
#if ALDOC
\alpage{expandFraction}
\Usage{\name~a\\ \name(a)(f)}
\Signature{R}{\altype{Fraction} RX $\to$ (\altype{Integer}, RXX)}
\Params{
{\em a} & R & the expansion point\\
{\em f} & \altype{Fraction} RX & a rational function\\
}
\Retval{\name(a)(f) returns $(n, s)$ where $n \le 0$ and the series expansion
of $f$ around {\em a} is $(x-a)^{n} s(x-a)$.
In addition, $s(a) \ne 0$ whenever $n < 0$.}
\alseealso{\alexp{expand},\alexp{tryExpandFraction}}
#endif
		if R has IntegralDomain then {
			polynomials: (V RXX, Z) -> (V RX, M R);
			polynomials: (V RXX, Z, Z) -> (V RX, M R);
#if ALDOC
\alpage{polynomials}
\Usage{\name([$s_1,\dots,s_n$], N)\\ \name([$s_1,\dots,s_n$], N, M)}
\Signatures{
\name: & (\altype{Vector} RXX, \altype{Integer})
$\to$ (\altype{Vector} RX, \altype{DenseMatrix} R)\\
\name: & (\altype{Vector} RXX, \altype{Integer}, \altype{Integer})
$\to$ (\altype{Vector} RX, \altype{DenseMatrix} R)\\
}
\Params{
$s_i$ & RXX & series\\
\emph{N} & \altype{Integer} & a degree bound\\
\emph{M} & \altype{Integer} & an upper bound\\
}
\Descr{\name($[s_1,\dots,s_n], N, M$) returns $([p_1,\dots,p_s], A)$
such that the series $[s_1,\dots,s_n] A$ all have coefficients $0$
from $x^{N+1}$ to $x^M$, and $[p_1,\dots,p_s]$ are the truncations
to order $N$ of $[s_1,\dots,s_n] A$. If the upper bound $M$ is not given,
then the series returned have coefficients $0$ from $x^{N+1}$ up to
an order that is determined heuristically.}
#endif
		}
		if R has CommutativeRing then {
			monicNewtonSeries: RX -> RXX;
#if ALDOC
\alpage{monicNewtonSeries}
\Usage{\name~p}
\Signature{RX}{RXX}
\Params{{\em p} & RX & A monic polynomial\\}
\Retval{Returns the series
$$
\sum_{n\ge 0} (y_1 + \dots + y_d)^n x^n
$$
where $y_1,\dots,y_d$ are all the roots of \emph{p}.}
#endif
			tryExpandFraction: R -> (RX, RX) -> (Z, Partial RXX);
#if ALDOC
\alpage{tryExpandFraction}
\Usage{\name~a\\ \name(a)(p,q)}
\Signature{R}{(RX, RX) $\to$ (\altype{Integer}, \altype{Partial} RXX)}
\Params{
{\em a} & R & the expansion point\\
{\em p,q} & RX & numerator and denominator of a rational function\\
}
\Retval{If $q(a)$ is a unit in \emph{R},
then \name(a)(p,q) returns $(n, s)$ where $n \le 0$ and the series expansion
of $p/q$ around {\em a} is $(x-a)^{n} s(x-a)$.
In addition, $s(a) \ne 0$ whenever $n < 0$.
Otherwise, it returns $(n,\failed)$ where \emph{n} is the order of \emph{p/q}
at $x=a$.}
\alseealso{\alexp{expand},\alexp{expandFraction}}
#endif
		}
		truncate: (RXX, Z) -> RX;
#if ALDOC
\alpage{truncate}
\Usage{\name(s, m)}
\Signature{(RXX,\altype{Integer})}{RX}
\Params{
{\em s} & \% & a series\\
{\em m} & \altype{Integer} & the truncation order\\
}
\Retval{Returns the truncation of {\em s} at order {\em m}, \ie
$$
\sum_{n=0}^{m-1} a_n x^n
$$
where $s = \sum_{n \ge 0} a_n x^n$.}
#endif
} == add {
	expand(a:R):RX -> RXX == {
		f := coeffs a;
		(p:RX):RXX +-> {
			import from I, Z, R, Sequence R, Stream R;
			zero? p => 0;
			series sequence stream(f p, next machine degree p, 0);
		}
	}

	if R has CommutativeRing then {
		monicNewtonSeries(p:RX):RXX == {
			import from Z, R, RX, Partial RXX;
			assert(~zero? p);
			assert(unit? leadingCoefficient p);
			p := shift(p, -trailingDegree p);
			zero?(pp := differentiate p) => 0;
			d := prev(degree p - degree pp);
			assert(d >= 0);         -- always 0 in characteristic 0
			(nu, s) := tryExpandFraction(0)(shift!(revert! pp, d),
							revert p);
			assert(zero? nu);
			retract s;
		}

		tryExpandFraction(a:R):(RX, RX) -> (Z, Partial RXX) == {
			import from RX, RXX, Partial RXX;
			phi := expand a;
			nu := orderquo(monom - a::RX);
			(p:RX, q:RX):(Z, Partial RXX) +-> {
				assert(~zero? q);
				zero? p => (0, [0]);
				(d, den) := nu q;
				failed?(u := reciprocal phi den) => (-d,failed);
				(-d, [phi(p) * retract u]);
			}
		}
	}

	if R has Field then {
		expandFraction(a:R):Fraction RX -> (Z, RXX) == {
			import from RX, RXX, Partial RXX;
			phi := expand a;
			nu := orderquo(monom - a::RX);
			(f:Fraction RX):(Z, RXX) +-> {
				zero? f => (0, 0);
				(d, den) := nu denominator f;
				s := retract reciprocal phi den;
				(-d, phi(numerator f) * s);
			}
		}
	}

	if R has FloatType then {
		expandFraction(a:R):Fraction RX -> (Z, RXX) == {
			import from RX, RXX, Partial RXX;
			phi := expand a;
			nu := orderquo(monom - a::RX);
			(f:Fraction RX):(Z, RXX) +-> {
				zero? f => (0, 0);
				(d, den) := nu denominator f;
				s := retract reciprocal phi den;
				(-d, phi(numerator f) * s);
			}
		}
	}

	local coeffs(a:R):RX -> Generator R == {
		zero? a => {
			(p:RX):Generator R +-> coefficients p;
		 }
		(p:RX):Generator R +-> generate {
			import from Boolean, I, Z;
			assert(~zero? p);
			for i in 0..machine degree p repeat {
				(p, c) := Horner(p, a);
				yield c;
			}
		}
	}

	truncate(s:RXX, m:Z):RX == {
		assert(m >= 0);
		p:RX := 0;
		for i in prev(m)..0 by -1 repeat
			p := add!(p, coefficient(s, i), i);
		p;
	}

	if R has IntegralDomain then {

		dot(v:V RXX, m:M R):V RXX == {
			import from RXX;
			[dot(w)(v) for w in columns m];
		}

		polynomials(s:V RXX, degbound:Z):(V RX, M R) ==
			polynomials(s, degbound, -1);

		-- limit < 0 means no limit known, use heuristic
		-- otherwise make the series 0 from x^degbound+1 to x^limit
		polynomials(s:V RXX, degbound:Z, limit:Z):(V RX, M R) == {
			import from I, R, RXX;
			import from LinearAlgebra(R, M(R));
			assert(degbound >= -1);
			assert(limit < 0 or limit >= degbound);
			c := #s;
			r := { limit < 0 => c; machine(limit - degbound); }
			m:M R := zero(r, c);
			for i in r..1 by -1 repeat for j in 1..c repeat
				m(i, j) := coefficient(s.j, degbound + i::Z);
			dim := numberOfColumns(kern := kernel m);
			s := dot(s, kern);
			b := degbound + r::Z;	-- s = 0 from x^{bound+1} to x^b
			limit >= 0 =>
				([truncate(f, next degbound) for f in s], kern);
			-- Use heuristic refinement until dim=0 or dim unchanged
			non0? := true;
			while dim > 0 and non0? repeat {
				non0? := false;
				m := zero(dim, dim);
				for i in dim..1 by -1 repeat
					for j in 1..dim repeat {
						mij := coefficient(s.j, b+i::Z);
						m(i, j) := mij;
						if ~(non0? or zero? mij) then
							non0? := true;
				}
				if non0? then {
					dim:= numberOfColumns(newk := kernel m);
					kern := {
						dim > 0 => kern * newk;
						zero(0, 0);
					}
					s := dot(s, newk);
				}
				b := b + dim::Z;
			}
			([truncate(f, next degbound) for f in s], kern);
		}
	}
}

