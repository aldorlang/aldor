----------------------------- sit_uffalg.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z == Integer;
}

#if ALDOC
\thistype{UnivariateFreeRing}
\History{Manuel Bronstein}{22/5/95}{created}
\History{Thom Mulders}{27/5/97}{added partial add!}
\History{Manuel Bronstein}{14/4/2000}{separated the common series/poly exports}
\History{Manuel Bronstein}{26/4/2001}
{extracted FreeModule and FreeAlgebra,
 changed name from UnivariateFreeFiniteAlgebra to UnivariateFreeRing}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~is a common category for commutative and noncommutative
univariate polynomials with coefficients in an arbitrary arithmetic
system \emph{R} and with respect to an arbitrary basis $(P_n)_{n \ge 0}$.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{IndexedFreeRRing}(R, \altype{Integer})}\\
\category{\altype{UnivariateFreeLinearArithmeticType} R}\\
\alexp{coerce}: & \altype{Vector} R $\to$ \% & Create a polynomial\\
\alexp{companion}: & \% $\to$ \altype{DenseMatrix} R & Companion matrix\\
\alexp{monomial!}: & (\%, R, Z) $\to$ \% & In-place monomial\\
\alexp{random}: & (\altype{Integer}, () $\to$ R, \altype{Integer}) $\to$ \% &
Creation of a random polynomial\\
\alexp{revert}: & \% $\to$ \% & Left--Right reversion\\
\alexp{revert!}: & \% $\to$ \% & In--place left--right reversion\\
%\alexp{times}: & (\%, \%, Z, Z) $\to$ \% & Partial product\\
\alexp{vectorize!}:
& \altype{Vector} R $\to$ \% $\to$ \altype{Vector} R &
Conversion to a vector\\
\end{exports}
\begin{alwhere}
Z &==& \altype{Integer}\\
\end{alwhere}
\begin{exports}[if \emph{R} has \altype{HashType} then]
\category{\altype{HashType}}\\
\end{exports}
\begin{exports}[if \emph{R} has \altype{Ring} then]
\alexp{random}: & (\altype{Integer}, \altype{Integer}) $\to$ \% &
Creation of a random polynomial\\
\end{exports}
\begin{exports}[if \emph{R} has \altype{SerializableType} then]
\category{\altype{SerializableType}}\\
\end{exports}
#endif

define UnivariateFreeRing(R:Join(ArithmeticType, ExpressionType)): Category ==
	Join(CopyableType, IndexedFreeRRing(R, Z),
		UnivariateFreeLinearArithmeticType R) with {
	coerce: Vector R -> %;
	vectorize!: Vector R -> % -> Vector R;
#if ALDOC
\alpage{coerce,vectorize!}
\altarget{coerce}
\altarget{vectorize!}
\Usage{v::\%\\ vectorize! v\\ vectorize!(v)(p)}
\Signatures{
coerce: & \altype{Vector} R $\to$ \%\\
vectorize!: & \altype{Vector} R $\to$ \% $\to$ \altype{Vector} R\\
}
\Params{
{\em p} & \% & A polynomial\\
{\em v} & \altype{Vector} R & A coefficient vector\\
}
\Descr{$[v_1,\dots,v_n]$::\% returns the polynomial
$\sum_{i=0}^{n-1} v_{i+1} P_i$, while
vectorize!$(v)(\sum_{j=0}^d a_j P_j)$ fills $v$ with $[a_0,\dots,a_d,0,\dots]$
and returns $v$.}
\Remarks{If $d > \#v - 1$, then the high coefficients of $p$ are
simply ignored.}
#endif
	companion: % -> DenseMatrix R;
#if ALDOC
\alpage{companion}
\Usage{\name~p}
\Signature{\%}{\altype{DenseMatrix} R}
\Params{ {\em p} & \% & A polynomial\\ }
\Retval{Returns the companion matrix
$$
\pmatrix{
0   &        &   &     &  -a_0    \cr
a_n & \ddots &   &     &  -a_1    \cr
    & \ddots &   &     &  -a_2    \cr
    &        &   &     & \vdots   \cr
    &        &   & a_n &  -a_{n-1}\cr
}
$$
where $p = \sum_{i=0}^n a_i P_i$.}
#endif
	monomial!: (%, R, Z) -> %;
#if ALDOC
\alpage{monomial!}
\Usage{\name(p, c, n)}
\Signature{(\%, R, \altype{Integer})}{\%}
\Params{
{\em p} & \% & A polynomial (to be destroyed)\\
{\em c} & R & A scalar\\
{\em n} & \altype{Integer} & An exponent\\
}
\Retval{Returns the monomial $c P_n$.}
\Remarks{The storage used by $p$ is allowed to be destroyed or reused
so $p$ is lost after this call.
This may cause $p$ to be destroyed, so do not use this unless
$p$ has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
	random: (Z, () -> R, m:Z == -1) -> %;
	if R has Ring then {
		random: (Z, m:Z == -1) -> %;
#if ALDOC
\alpage{random}
\Usage{\name(n[, m])\\ \name(n, f[, m])}
\Signatures{
\name:& (\altype{Integer}, \altype{Integer}) $\to$ \%\\
\name:& (\altype{Integer}, () $\to$ R, \altype{Integer}) $\to$ \%\\
}
\Params{
{\em n} & \altype{Integer} & The desired degree\\
{\em f} & () $\to$ R & A random generator for R\\
{\em m} & \altype{Integer} & The desired number of terms (optional)\\
}
\Retval{Returns a monic random polynomial of degree
{\em n} with at most {\em m} terms,
($n+1$ terms if {\em m} is not present). Uses $f()$ to generate
the coefficients if the parameter {\em f} is present, the \alfunc{Ring}{random}
function otherwise.}
#endif
	}
	revert: % -> %;
	revert!: % -> %;
#if ALDOC
\alpage{revert}
\altarget{\name!}
\Usage{\name~p\\ \name!~p}
\Signature{\%}{\%}
\Params{ {\em p} & \% & A polynomial\\ }
\Retval{Returns
$\sum_{i=0}^n a_i P_{n-i}$
where
$p = \sum_{i=0}^n a_i P_i$ and $a_n \ne 0$. Returns 0 if $p = 0$.}
\Remarks{The storage used by {\em p} is allowed to be destroyed or reused
when \name! is used,
so {\em p} is lost after those calls.
This may cause {\em p} to be destroyed, so do not use this unless
{\em p} has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
\alseealso{\alexp{terms}}
#endif
--	times: (%, %, Z, Z) -> %;
#if ALDOC_COMMENTED_OUT
\alpage{times}
\Usage{\name(p, q, l, h)}
\Signature{(\%, \%, \altype{Integer}, \altype{Integer})}{\%}
\Params{
{\em p, q} & \% & Polynomials\\
{\em l, h} & \altype{Integer} &
Lower and upper bound of coefficients to be computed\\
}
\Retval{\name(p, q, l, h) returns a polynomial $s$ containing some coefficients
of the product $pq$. The $i$th coefficient of $s$ equals the $l+i$th
coefficient of $pq$. Here $l\le h$ should hold.}
#endif
	default {
		ground?(p:%):Boolean	== { import from Z; zero? degree p; }
		monomial!(p:%,c:R,n:Z):%== term(c, n);
		revert!(p:%):%		== revert p;

		support(p:%):Generator Cross(R, %) == generate {
			import from R, Z;
			for trm in p repeat {
				(c, n) := trm;
				yield (c, monomial n);
			}
		}

		coefficients(p:%):Generator R == generate {
			import from Z;
			for i in 0..degree p repeat yield coefficient(p, i);
		}

		relativeSize(p:%):MachineInteger == {
			import from Z;
			zero? p => 0;
			machine degree p;
		}

		leadingCoefficient(p:%):R == {
			(c, n) := leadingTerm p;
			c;
		}

		degree(p:%):Z == {
			(c, n) := leadingTerm p;
			n;
		}

		trailingCoefficient(p:%):R == {
			(c, n) := trailingTerm p;
			c;
		}

		trailingDegree(p:%):Z == {
			(c, n) := trailingTerm p;
			n;
		}

		shift(p:%, n:Z):% == {
			zero? n => p;
			q:% := 0;
			pos? := n > 0;
			for term in p repeat {
				(a, d) := term;
				e := d + n;
				if pos? or e >= 0 then q := add!(q, a, e);
			}
			q;
		}

		companion(p:%):DenseMatrix R == {
			import from MachineInteger, Z, R;
			zero? p or zero?(d := degree p) => zero(0, 0);
			n := machine d;
			v:Vector R := zero(n := machine d);
			for term in p repeat {
				(c, e) := term;
				ee := machine e;
				if ee < n then v(next ee) := -c;
			}
			companion(v, leadingCoefficient p);
		}

		copy(p:%):% == {
			q:% := 0;
			for term in p repeat {
				(c, n) := term;
				q := add!(q, c, n);
			}
			q;
		}

		map(f:R -> R)(p:%):% == {
			q:% := 0;
			for term in p repeat {
				(c, n) := term;
				q := add!(q, f c, n);
			}
			q;
		}

--		times(a:%, b:%, l:Z, h:Z):% == {
--			p := a * b;
--			s:% := 0;
--			for i in l..min(degree p, h) repeat
--				s := add!(s, coefficient(p, i), i - l);
--			s
--		}

		random(n:Z, f:() -> R, m:Z):% == {
			import from R;
			assert(n >= 0);
			if m < 0 or m > n+1 then m := n+1;
			p := monomial(n) + term(f(), 0);
			for i in 3..m repeat
				p := add!(p, f(), random()@Z rem n);
			p;
		}

		if R has Ring then {
			coerce(n:Z):% == { import from R; term(n::R, 0); }
			random(n:Z, m:Z):% == random(n, random$R, m);

			-- TEMPORARY: 1.1.12p4 DOES NOT SEE THE Algebra DEFAULT
			characteristic:Z == characteristic$R;

			random():% == {
				import from Z;
				d := random()$Z rem 101;
				random(d, (3 * d) quo 4);
			}
		}

		revert(p:%):% == {
			zero? p => p;
			d := degree p;
			q:% := 0;
			-- terms is important so that the high degree terms
			-- are created first (space-efficiency)
			for term in terms p repeat {
				(c, e) := term;		-- low to high
				q := add!(q, c, d - e);
			}
			q;
		}

		vectorize!(v:Vector R): % -> Vector R == {
			import from MachineInteger, Z;
			n := #v;
			(p:%):Vector R +-> {
				for i in 1..n repeat
					v.i := coefficient(p, prev(i)::Z);
				v;
			}
		}

		coerce(v:Vector R):% == {
			import from MachineInteger, Z;
			zero?(n := #v) => 0;
			n1 := prev n;
			p := term(v.n, n1::Z);
			for i in n1..1 by -1 repeat p := add!(p,v.i,prev(i)::Z);
			p;
		}

		-- default is sparse dumping, terminated by 0 P_{-1}
		if R has SerializableType then {
			(port:BinaryWriter) << (p:%):BinaryWriter == {
				import from R, Z;
				for term in p repeat {
					(c, n) := term;
					port := port << c << n;
				}
				port << 0@R << (-1)@Z;
			}

			<< (port:BinaryReader):% == {
				import from R, Z;
				p:% := 0;
				n:Z := 0;
				local c:R;
				while n >= 0 repeat {
					c := << port;
					n := << port;
					if n >= 0 then p := add!(p, c, n);
				}
				p;
			}
		}

		if R has HashType then {
			hash(p:%):MachineInteger == {
				import from R;
				h:MachineInteger := 0;
				for term in p repeat {
					(c, n) := term;
					h := h + hash(c) + machine n;
				}
				h;
			}
		}
	}
}

#if ALDOC
\thistype{UnivariateFreeRing2}
\History{Manuel Bronstein}{27/7/2000}{created}
\Usage{import from \this(R,Rx,S,Sy)}
\Params{
{\em R, S} & \altype{ExpressionType} & Coefficient domains\\
           & \altype{ArithmeticType} &\\
{\em Rx}
& \altype{UnivariateFreeRing} R & A monogenic algebra over $R$\\
{\em Sy}
& \altype{UnivariateFreeRing} S & A monogenic algebra over $R$\\
}
\Descr{\this(R,Rx,S,Sy) provides tools for lifting maps $R \to S$ to
maps $Rx \to Sy$.}
\begin{exports}
\alexp{map}: & (R $\to$ S) $\to$ Rx $\to$ Sy & Lift a mapping\\
\end{exports}
#endif

UnivariateFreeRing2(R:Join(ArithmeticType, ExpressionType),
				RX:UnivariateFreeRing R,
				S:Join(ArithmeticType, ExpressionType),
				SX:UnivariateFreeRing S): with {
	map: (R -> S) -> RX -> SX;
#if ALDOC
\alpage{map}
\Usage{\name~f\\\name(f)(p)}
\Signature{(R $\to$ S) $\to$ Rx}{Sy}
\Params{
{\em f} & R $\to$ S & A map\\
{\em p} & \% & A polynomial with coefficient in $R$\\
}
\Descr{\name(f)(p) returns
$$
f(p) = \sum_i f(a_i) y^i
$$
where $p = \sum_i a_i x^i$, while \name(f) returns the mapping $p \to f(p)$.}
#endif
} == add {
	map(f:R -> S)(p:RX):SX == {
		q:SX := 0;
		for term in p repeat {
			(c, n) := term;
			q := add!(q, f c, n);
		}
		q;
	}
}

