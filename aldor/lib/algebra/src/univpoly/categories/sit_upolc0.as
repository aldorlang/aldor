----------------------------- sit_upolc0.as --------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	I == MachineInteger;
	Z == Integer;
	V == Vector;
	M == DenseMatrix;
}

-- The documentation for this type is meant to be included in sit_upolc.tex,
-- since UnivariatePolynomialAlgebra0 is not documented separately
define UnivariatePolynomialAlgebra0(R:Join(ArithmeticType, ExpressionType)):
	Category == Join(UnivariatePolynomialRing R,
				IndexedFreeAlgebra(R, Z)) with {
	apply: (%, R) -> R;
	apply: (%, %) -> %;
#if ALDOC
\alpage{apply}
\Usage{ \name(p, a)\\ \name(p, q)\\ p~a\\ p~q }
\Signatures{
\name: & (\%, R) $\to$ R\\
\name: & (\%, \%) $\to$ \%\\
}
\Params{
{\em p} & \% & A polynomial\\
{\em q} & \% & A polynomial\\
{\em a} & R & A scalar\\
}
\Retval{
Returns
$$
p(a) = \sum_{i=0}^n a_i a^i
$$
or
$$
p(q) = \sum_{i=0}^n a_i q^i
$$
where $p = \sum_{i=0}^n a_i x^i$.}
#endif
	equal?: (%, %, %, Z) -> Boolean;
#if ALDOC
\alpage{equal?}
\Usage{\name(a, b, c, n)}
\Signature{(\%, \%, \%, \altype{Integer})}{\altype{Boolean}}
\Params{
{\em a, b, c} & \% & Polynomials \\
{\em n} & \altype{Integer} & The order of truncation\\
}
\Retval{Returns \true~if $a = b c \pmod{x^n}$, \false~otherwise.}
#endif
	if R has OrderedArithmeticType then height: % -> R;
#if ALDOC
\alpage{height}
\Usage{\name~p}
\Signature{\%}{R}
\Params{ {\em p} & \% & A polynomial\\ }
\Retval{Returns
$$
\vert\vert p \vert\vert_\infty = \max_{i=0}^n\left(\vert a_i \vert\right)
$$
where $p = \sum_{i=0}^n a_i x^i$.}
#endif
	Horner: (%, R) -> (%, R);
#if ALDOC
\alpage{Horner}
\Usage{\name(p, a)}
\Signature{(\%, R)}{(\%, R)}
\Params{
{\em p} & \% & A polynomial\\
{\em a} & R & A point\\
}
\Retval{Returns $(q, p(a))$ such that $p = q (x - a) + p(a)$.}
#endif
	if R has CommutativeRing then {
	    ModularComputation;
	    DifferentialRing;
	    if R has RittRing then {
		integrate: % -> %;
		integrate: (%, Integer) -> %;
	    }
#if ALDOC
\alpage{integrate}
\Usage{\name~p\\ \name(p, n)}
\Signatures{
\name: & \% $\to$ \%\\
\name: & (\%, \altype{Integer}) $\to$ \%\\
}
\Params{
{\em p} & \% & A polynomial\\
{\em n} & \altype{Integer} & The order of integration\\
}
\Retval{\name(p) returns $\int p(x) dx$,
while integrate(s, n) returns $\int \dots \int s(x) dx^n$.}
#endif
		lift: (Derivation R, %) -> Derivation %;
#if ALDOC
\alpage{lift}
\Usage{\name(D, x')}
\Signature{(\altype{Derivation} R, \%)}{\altype{Derivation} \%}
\Params{
{\em D} & \altype{Derivation} R & A derivation on R\\
{\em x'} & \% & The desired derivative of x\\
}
\Retval{Returns the unique extension of the derivation $D$ such that
$D x = x'$.}
#endif
		monicDivide: (%, %) -> (%, %);
		monicDivide!: (%, %) -> (%, %);
		monicDivideBy: % -> % -> (%, %);
		monicDivideBy!: % -> % -> (%, %);
		monicQuotient: (%, %) -> %;
		monicQuotient!: (%, %) -> %;
		monicQuotientBy: % -> % -> %;
		monicQuotientBy!: % -> % -> %;
		monicRemainder: (%, %) -> %;
		monicRemainder!: (%, %) -> %;
		monicRemainderBy: % -> % -> %;
		monicRemainderBy!: % -> % -> %;
#if ALDOC
\alpage{monicDivide,monicQuotient, monicRemainder}
\altarget{monicDivide}
\altarget{monicDivide!}
\altarget{monicDivideBy}
\altarget{monicDivideBy!}
\altarget{monicQuotient}
\altarget{monicQuotient!}
\altarget{monicQuotientBy}
\altarget{monicQuotientBy!}
\altarget{monicRemainder}
\altarget{monicRemainder!}
\altarget{monicRemainderBy}
\altarget{monicRemainderBy!}
\Usage{monicXXX(a, b)\\ monicXXX!(a, b)\\ monicXXXBy(b)(a)\\ monicXXXBy!(b)(a)}
\Signatures{
monicDivide, monicDivide!: & (\%, \%) $\to$ (\%, \%)\\
monicDivideBy, monicDivideBy!: & \% $\to$ \% $\to$ (\%, \%)\\
monicQuotient, monicQuotient!: & (\%, \%) $\to$ \%\\
monicRemainder, monicRemainder!: & (\%, \%) $\to$ \%\\
monicQuotientBy, monicQuotientBy!: & \% $\to$ \% $\to$ \%\\
monicRemainderBy, monicRemainderBy!: & \% $\to$ \% $\to$ \%\\
}
\Params{
{\em a} & \% & A polynomial\\
{\em b} & \% & A polynomial whose leading coefficient is a unit in {\em R}\\
}
\Retval{monicRemainder(a, b) returns {\em r} such that
either $r = 0$ or $\deg(r) < \deg(b)$ or $a \equiv r \pmod b$,
monicQuotient(a, b) returns {\em q} such that $a - bq = 0$ or
$\deg(a - bq) < \deg(b)$, and monicDivide(a, b) returns
$(q, r)$ such that $a = b q + r$ and either $r = 0$ or $\deg(r) < \deg(b)$.
The functions monicDivide!, monicQuotient! and monicRemainder! return the
same results but allow the storage used by {\em a} to be destroyed or reused.
Finally, monicXXXBy(b) returns the map $a \to \mbox{monicXXX}(a, b)$, while
monicXXXBy!(b) returns the map $a \to \mbox{monicXXX!}(a, b)$.}
\Remarks{When using monicXXX!(a,b) or monicXXXBy!(b)(a),
the storage used by a is allowed to be
destroyed or reused, so a is lost after this call.
This may cause a to be destroyed, so do not use this unless a has been locally
allocated, and is thus guaranteed not to share space with other polynomials.}
\alseealso{\alexp{pseudoRemainder}}
#endif
		monicLinearGenerator: % -> V R -> Generator R;
	}
	if R has CharacteristicZero then {
		ordinaryPoint: % -> Z;
#if ALDOC
\alpage{ordinaryPoint}
\Usage{\name~p}
\Signature{\%}{\altype{Integer}}
\Params{ {\em p} & \% & A nonzero polynomial\\ }
\Retval{Returns an integer $n$ such that $p(n) \ne 0$.}
#endif
	}
	if R has IntegralDomain then {
		IntegralDomain;
		pseudoDivide: (%, %) -> (%, %);
#if ALDOC
\alpage{pseudoDivide}
\Usage{\name(a, b)}
\Signature{(\%, \%)}{(\%, \%)}
\Params{
{\em a} & \% & A polynomial\\
{\em b} & \% & A nonzero polynomial\\
}
\Retval{Returns $(q, r)$ such that $c^n a = b q + r$ and either $r = 0$
or $\deg(r) < \deg(b)$, where $c$ is the leading coefficient of $b$ and
$n = \deg(a) - \deg(b) + 1$.}
\alseealso{\alexp{pseudoRemainder}}
#endif
		pseudoRemainder: (%, %) -> %;
		pseudoRemainder!: (%, %) -> %;
#if ALDOC
\alpage{pseudoRemainder}
\altarget{\name!}
\Usage{\name(a, b)\\ \name!(a, b)}
\Signature{(\%, \%)}{\%}
\Params{
{\em a} & \% & A polynomial\\
{\em b} & \% & A nonzero polynomial\\
}
\Retval{Returns $r$ such that $c^n a = b q + r$ and either $r = 0$
or $\deg(r) < \deg(b)$, where $c$ is the leading coefficient of $b$ and
$n = \deg(a) - \deg(b) + 1$.}
\Remarks{When using \name!($a,b$), the storage used by a is allowed to be
destroyed or reused, so a is lost after this call.
This may cause a to be destroyed, so do not use this unless a has been locally
allocated, and is thus guaranteed not to share space with other polynomials.}
\alseealso{\alexp{pseudoDivide}}
#endif
	}
	if R has Field then {
		rationalReconstruction: % -> % -> Partial Cross(%, %);
#if ALDOC
\alpage{rationalReconstruction}
\Usage{\name~m\\ \name(m)(u)}
\Signature{\%}{\% $\to$ \altype{Partial} \builtin{Cross}(\%, \%)}
\Params{
\emph{m} & \% & A modulus of positive degree\\
\emph{u} & \% & A polynomial\\
}
\Retval{\name(m)(u) returns either $(a,b)$ such that $a/b = u \pmod m$,
$\deg(a) \le (\deg(m)-1)/2$ and $\deg(b) \le (\deg(m)-1)/2$,
or \failed~if no such $a,b$ exist.}
\Remarks{The resulting $a$ and $b$ are guaranteed to be unique.}
\alseealso{\alfunc{EuclideanDomain}{rationalReconstruction}}
#endif
	}
	if R has IntegralDomain then {
		resultant: (%, %) -> R;
#if ALDOC
\alpage{resultant}
\Usage{\name(p, q)}
\Signature{(\%, \%)}{R}
\Params{
{\em p} & \% & A polynomial\\
{\em q} & \% & A polynomial\\
}
\Retval{Returns the resultant of p and q.}
#endif
	}
	if R has Field then {
		sparseMultiple: (%, Z) -> %;
#if ALDOC
\alpage{sparseMultiple}
\Usage{\name(p, n)}
\Signature{(\%, \altype{Integer})}{\%}
\Params{
{\em p} & \% & A polynomial\\
{\em n} & \altype{Integer} & A positive integer\\
}
\Retval{Returns a nonzero polynomial $q = \sum_{i=0}^m a_i x^i$
of minimal degree such that $q(x^n)$ is a multiple of $p(x)$.}
#endif
	}
	if R has GcdDomain then {
		DecomposableRing;
		GcdDomain;
		squareFree: % -> (R, Product %);
		squareFreePart: % -> %;
#if ALDOC
\alpage{squareFree,squareFreePart}
\altarget{squareFree}
\altarget{squareFreePart}
\Usage{squareFree~p\\ squareFreePart~p}
\Signatures{
squareFree: & \% $\to$ (R, \altype{Product} \%)\\
squareFreePart: & \% $\to$ \%\\
}
\Params{ {\em p} & \% & A polynomial\\ }
\Descr{squareFree(p) returns $(c, p_1^{e_1} \cdots p_n^{e_n})$ such that
each $p_i$ is squarefree, the $p_i$'s have no common factors, and
$$
p = c\;\prod_{i=1}^n p_i^{e_i}\,,
$$
while squareFreePart(p) returns $p^\ast$ such that $p^\ast$ is squarefree,
$p^\ast \mid p$ and every irreducible factor of $p$ divides $p^\ast$.
}
#endif
	}
	if R has Ring then {
		values: (%, R) -> Generator R;
#if ALDOC
\alpage{values}
\Usage{\name(p, a)}
\Signature{(\%,R)}{\altype{Generator} R}
\Params{
{\em p} & \% & A polynomial\\
{\em a} & R & A scalar\\
}
\Retval{Returns a generator generating the
sequence $p(a), p(a+1), p(a+2),\ldots$}
\Remarks{\name~uses arrays of differences and can be more efficient
that repeated Horner evaluation.}
#endif
	}
	if R has Field then EuclideanDomain;
	if R has Specializable then Specializable;
	default {
		commutative?:Boolean == commutative?$R;

		equal?(a:%, b:%, c:%, n:Z):Boolean == {
			n <= 0 or zero?(p := a - b * c) => true;
			trailingDegree(p) >= n;
		}

		-- Horner's division of p by x - a,
		-- returns c q and p(a) s.t.  p = q (x - a) + p(a)
		local horner(p:%, c:R, a:R):(%, R) == {
			import from Z;
			assert(~zero? c);
			one? c => Horner(p, a);
			zero? p => (0, 0);
			zero? a => (times!(c, shift(p, -1)), coefficient(p, 0));
			v := leadingCoefficient p;
			q:% := 0;
			for i in prev degree p .. 0 by -1 repeat {
				q := add!(q, c * v, i);
				v := a * v + coefficient(p, i);
			}
			(q, v);
		}

		Horner(p:%, a:R):(%, R) == {
			import from Z;
			zero? p => (0, 0);
			zero? a => (shift(p, -1), coefficient(p, 0));
			v := leadingCoefficient p;
			q:% := 0;
			for i in prev degree p .. 0 by -1 repeat {
				q := add!(q, v, i);
				v := a * v + coefficient(p, i);
			}
			(q, v);
		}

		-- uses Horner's rule
		apply(p:%, a:R):R == {
			import from Z;
			zero? p => 0;
			zero? a => coefficient(p, 0);
			v := leadingCoefficient p;
			for i in prev degree p .. 0 by -1 repeat
				v := a * v + coefficient(p, i);
			v;
		}

		-- uses Horner's rule
		apply(p:%, q:%):% == {
			import from Z;
			zero? p => 0;
			zero? q => coefficient(p, 0)::%;
			v := leadingCoefficient(p)::%;
			for i in degree p - 1 .. 0 by -1 repeat
				v := add!(times!(v, q), coefficient(p, i)::%);
			v;
		}

		-- returns c p x^n, always makes a copy, also for n = 0
		local shift(p:%, c:R, n:Z):% == {
			zero? c => 0;
			one? c => {
				zero? n => copy p;
				shift(p, n);
			}
			q:% := 0;
			pos?:Boolean := n >= 0;
			for term in p repeat {
				(a, d) := term;
				e := d + n;
				if pos? or e >= 0 then q := add!(q, c * a, e);
			}
			q;
		}

		if R has CharacteristicZero then {
			-- tries 0,1,-1,2,-2,... until a good point is found
			ordinaryPoint(p:%):Z == {
				import from R;
				~zero? p(0@R) => 0;
				n:Z := 1;
				repeat {
					a := n::R;
					~zero? p a => return n;
					~zero? p(-a) => return(-n);
					n := next n;
				}
			}
		}

		if R has Ring then {
			values(p:%, a:R):Generator R == {
				import from Array R, I, Z;
				zero? p or zero?(dp := degree p) => generate {
					repeat yield leadingCoefficient p;
				}
				d := machine dp;
				v:Array R := new next d;
				generate {
					for i in 0..d repeat {
						v.i := p(a + i::Z::R);
						yield(v.i);
					}
					for i in prev(d)..0 by -1 repeat {
						for j in 0..i repeat
							v.j := v(j+1)-v.j;
					}
					repeat {
						for i in 1..d repeat
							v.i := v.i+v(i-1);
						yield v.d;
					}
				}
			}
		}

		if R has OrderedArithmeticType then {
			height(p:%):R == {
				h:R := 0;
				for term in p repeat {
					(c, n) := term;
					a := abs c;
					if a > h then h := a;
				}
				h;
			}
		}

		if R has CommutativeRing then {
			if R has RittRing then {
				integrate(p:%):% == {
					import from Z;
					integrate(p, 1);
				}

				integrate(p:%, k:Z):% == {
					import from I, R;
					zero?(kk := machine k) => p;
					assert(kk > 0);
					q:% := 0;
					for term in p repeat {
						(c, n) := term;
						m := factorial(next n, 1, kk);
						q := add!(q, inv(m) * c, n + k);
					}
					q;
				}
			}

			canonicalUnitNormal?:Boolean == canonicalUnitNormal?$R;

			cutoff(t:I):I == {
				(kx := cutoff(t)$R) < 0 => kx;
				t = CUTOFF__KARAMULT => {
					kx < 5 => kx;
					max(5, kx quo 2);
				}
				kx;
			}

			-- returns true if a = u x^n and u is a unit,
			-- false otherwise
			local umonom?(a:%):Boolean == {
				import from R;
				assert(~zero? a);
				term? a and unit? leadingCoefficient a;
			}

			-- MORE EFFICIENT THAN THE COMRING DEFAULT
			unit?(a:%):Boolean == {
				import from Z, R;
				zero? a => false;
				zero? degree a and unit? leadingCoefficient a;
			}

			-- returns u^{-1} if a = u x^n and u is a unit,
			-- failed otherwise
			local umonom(a:%):Partial R == {
				import from R;
				assert(~zero? a);
				term? a => reciprocal leadingCoefficient a;
				failed;
			}

			-- returns (q, u, u^{-1}) s.t. p = u q
			unitNormal(p:%):(%,%,%) == {
				import from R;
				(y, u, uinv) := unitNormal leadingCoefficient p;
				(uinv * p, u::%, uinv::%);
			}

			differentiate(p:%):% == {
				import from Z;
				differentiate(p, 1);
			}

			differentiate(p:%, k:Z):% == {
				import from I, R;
				zero?(kk := machine k) => p;
				assert(kk > 0);
				h:% := 0;
				for term in p repeat {
					(c, n) := term;
					if (e := n - k) >= 0 then {
						m := factorial(next e, 1, kk);
						h := add!(h, m::R * c, e);
					}
				}
				h;
			}

			lift(D:Derivation R,xp:%):Derivation % =={
				derivation((p:%):% +-> diff(p, D, xp));
			}

			local diff(p:%,D:Derivation(R),xp:%):% == {
				import from Z, R;
				h:% := 0;
				for term in p repeat {
					(c, n) := term;
					h := add!(h, D c, n);
					if n > 0 then
						h := add!(h, n::R * c, n-1, xp);
				}
				h;
			}

			-- destroys p
			monicRemainder!(p:%, q:%):% == monicRemainderBy!(q)(p);
			monicRemainderBy!(q:%):% -> % == {
				import from Z, R, Partial R;
				assert(~zero? q);
				zero?(dq := degree q) => { (p:%):% +-> 0 };
				lq := leadingCoefficient q;
				assert(unit? lq);
				-- the remainder depends only on q modulo a unit
				if ~one?(lq) then
					q := retract(reciprocal lq) * q;
				assert(one? leadingCoefficient q);
				term? q => { (p:%):% +-> truncate!(p, dq); }
				one? dq => {
					mq0 := - coefficient(q, 0);
					(p:%):% +-> p(mq0)::%;
				}
				(p:%):% +-> monicRemainder1!(p, q, dq);
			}

			monicRemainder(p:%, q:%):% == monicRemainderBy(q)(p);
			monicRemainderBy(q:%):% -> % == {
				import from Z, R, Partial R;
				assert(~zero? q);
				zero?(dq := degree q) => { (p:%):% +-> 0 };
				lq := leadingCoefficient q;
				assert(unit? lq);
				-- the remainder depends only on q modulo a unit
				if ~one?(lq) then
					q := retract(reciprocal lq) * q;
				assert(one? leadingCoefficient q);
				term? q => { (p:%):% +-> truncate(p, dq); }
				one? dq => {
					mq0 := - coefficient(q, 0);
					(p:%):% +-> p(mq0)::%;
				}
				(p:%):% +-> monicRemainder1!(copy p, q, dq);
			}

			-- destroys p
			-- case when q = X^m + h(X), with m > 1 and h <> 0
			local monicRemainder1!(p:%, q:%, dq:Z):% == {
				import from Z, R;
				assert(~zero? q); assert(~term? q);
				assert(dq = degree q); assert(dq > 1);
				assert(one? leadingCoefficient q);
				while ~zero? p repeat {
					dp := degree p;
					(d := dp - dq) < 0 => return p;
					p := add!(p,-leadingCoefficient p,d,q);
				}
				0;
			}

			monicQuotient(p:%, q:%):% == monicQuotientBy(q)(p);
			monicQuotientBy(q:%):% -> % == {
				div := monicDivideBy q;
				(p:%):% +-> { (qt, r) := div p; qt }
			}

			monicQuotient!(p:%, q:%):% == monicQuotientBy!(q)(p);
			monicQuotientBy!(q:%):% -> % == {
				div := monicDivideBy! q;
				(p:%):% +-> { (qt, r) := div p; qt }
			}

			monicDivide(p:%, q:%):(%, %) == monicDivideBy(q)(p);
			monicDivideBy(q:%):% -> (%, %) == {
				import from Z, R, Partial R;
				assert(~zero? q);
				lq := leadingCoefficient q;
				assert(unit? lq);
				ilq := retract reciprocal lq;
				zero?(dq := degree q) => {
					(p:%):(%, %) +-> (ilq * p, 0);
				}
				one? dq => {
					c := - coefficient(q, 0) * ilq;
					(p:%):(%, %) +-> {
						(qt, v) := horner(p, ilq, c);
						(qt, v::%);
					}
				}
				(p:%):(%,%) +-> monicDivide!(copy p,q,dq,ilq,0);
			}

			-- destroys p
			monicDivide!(p:%, q:%):(%, %) == monicDivideBy!(q)(p);
			monicDivideBy!(q:%):% -> (%, %) == {
				import from Z, R, Partial R;
				assert(~zero? q);
				lq := leadingCoefficient q;
				assert(unit? lq);
				ilq := retract reciprocal lq;
				zero?(dq := degree q) => {
					(p:%):(%, %) +-> (times!(ilq, p), 0);
				}
				one? dq => {
					c := - coefficient(q, 0) * ilq;
					(p:%):(%, %) +-> {
						(qt, v) := horner(p, ilq, c);
						(qt, v::%);
					}
				}
				(p:%):(%,%) +-> monicDivide!(p, q, dq, ilq, 0);
			}

			-- destroys p and quot, ilq = inverse of lc(q)
			local monicDivide!(p:%,q:%,dq:Z,ilq:R,quot:%):(%,%) == {
				import from Z, R;
				assert(~zero? q); assert(dq > 0);
				assert(dq = degree q);
				assert(one?(ilq * leadingCoefficient q));
				zero? p => (0, 0);
				one? p => (0, p);
				first?:Boolean := true;
				while ~zero? p repeat {
					d := degree p - dq;
					d < 0 => return (quot, p);
					c := leadingCoefficient p * ilq;
					quot := {
						first? => monomial!(quot, c, d);
						add!(quot, c, d);
					}
					p := add!(p, - c, d, q);
					if first? then first? := false;
				}
				(quot, 0);
			}

			local gener0():Generator R == generate {
				repeat yield(0$R);
			}

			local shiftLeft!(v:V R, r:R):() == {
				import from I;
				n := prev(#v);
				assert(n >= 0);
				for i in 1..n repeat v.i := v(next i);
				v.n := r;
			}

			monicLinearGenerator(p:%):V R -> Generator R == {
				import from I, Z, R, Partial R;
				assert(~zero? p);
				zero?(d := degree p) => {
					(v:V R):Generator R +-> {
						assert(empty? v);
						gener0();
					}
				}
				assert(unit? leadingCoefficient p);
				u := - retract reciprocal leadingCoefficient p;
				pp:V R :=
					[u*coefficient(p,i) for i in 0..prev d];
				(v:V R):Generator R +-> generate {
					assert(machine(d) = #v);
					buf := copy v;
					for c in v repeat yield c;
					repeat {
						yield(c := dot(pp, buf));
						shiftLeft!(buf, c);
					}
				}
			}
			(a:%) mod (b:%): % == {
			    u?: Partial(R) := reciprocal(leadingCoefficient(b));
			    assert(not failed? u?);
			    u: R := retract(u?);
			    monicRemainder(a, u*b);
			}
			macro field? == R has Field;

			local recipMod__Field(a: %, b: %):  Partial(%) == {
			        assert(field?);
                                (g: %, u:%, v:% ) := extendedEuclidean(a, b)$(% pretend EuclideanDomain);
                                import from Integer; 
                                degree(g) > 0 => failed;
                                lcg: R := leadingCoefficient(g);
				u := if field? then inv(lcg) * u else never;
				assert(one?((u * a) mod b)); -- COMMENT ME LATER
                                [u];
			}
			local recipMod__NonField(a: %, b: %):  Partial(%) == {
			    assert (not ground? b);
			    a := a mod b;
			    zero? a => failed;
			    ground? a => {
				lc?: Partial(R) := reciprocal(leadingCoefficient(a));
				failed? lc? => return failed;
				lc: R := retract(lc?);
				return [lc :: %];
			    }
			    local c0, d0, c1, d1: %;
			    (c0, d0) := (a, b);
			    (c1, d1) := (1, 0);
			    assert(not zero? d0);
			    local d0__n: %;
			    repeat {
				v : R := leadingCoefficient(d0);
				u?: Partial(R) := reciprocal(v);
				failed? u? => return failed;
				u: R := retract(u?);
				d0__n: % := u * d0;
				(q, r) := monicDivide(c0, d0__n);
				q := v * q;
				(c0, d0) := (d0, r);
				zero? d0 => break;
				(c1, d1) := (d1, c1 - q*d1)
			    }
			    not one? d0__n => failed;
			    assert(one?((c1 * a) mod b)); -- COMMENT ME LATER
			    [c1];
			}    
			recipMod(a: %, b: %):  Partial(%) == {
			    field? => recipMod__Field(a,b);
			    recipMod__NonField(a,b);
			}
		}

		if R has EuclideanDomain then {
		    symmetricMod(a: %, b: %): % == a mod b;
		}

		if R has IntegralDomain then {
			pseudoDivide(a:%,b:%):(%,%) == pseudoDivide!(copy a, b);
			pseudoRemainder(a:%,b:%):%==pseudoRemainder!(copy a,b);

			pseudoRemainder!(a:%,b:%): % == {
				import from Z, R;
				assert(~zero? b);
				zero? a => 0;
				da := degree a; db := degree b;
				lb := leadingCoefficient b;
				N := da-db+1;
				b := reductum b;
				while ~zero?(a) repeat {
					da := degree a;
					(d := da - db) < 0 => break;

					la := leadingCoefficient a;
					a:= add!(times!(lb,reductum a),-la,d,b);
					N := prev N;
				}
				zero? N => a;
				times!(lb^N, a);
			}

			local pseudoDivide!(a:%,b:%): (%,%) == {
				import from Z, R;
				assert(~zero? b);
				zero? a => (0,0);
				da := degree a; db := degree b;
				lb := leadingCoefficient b;
				N := da-db+1;
				b := reductum b;
				q:% := 0;
				while ~zero?(a) repeat {
					da := degree a;
					(d := da - db) < 0 => break;

					la := leadingCoefficient a;
					a:= add!(times!(lb,reductum a),-la,d,b);
					q := add!(times!(lb, q), la, d);
					N := prev N;
				}
				zero? N => (q, a);
				f := lb^N;
				(times!(f, q), times!(f, a));
			}

			quotient!(p:%, q:%):% == {
				import from Z, R, Partial R;
				assert(~zero? q);
				zero? p or one? q => p;
				(a, n) := trailingTerm q;
				quot!(p, q, quotientBy leadingCoefficient q,
					quotientBy a, n);
			}

			quotient(p:%, q:%):% == {
				import from Z, R, Partial R;
				assert(~zero? q);
				zero? p or one? q => p;
				(a, n) := trailingTerm q;
				quot!(copy p,q, quotientBy leadingCoefficient q,
					quotientBy a, n);
			}

			-- computes p / q, destroying p but not q
			local quot!(p:%,q:%,divByLq:R->R,divByTq:R->R,tdeg:Z):%_
				== {
				import from R;
				assert(~zero? p);
				assert(~zero? q); assert(~one? q);
				qt:% := 0;
				zero?(dq := degree q) => {
					for term in p repeat {
						(a, n) := term;
						qt := add!(qt, divByLq a, n);
					}
					qt;
				}
				dp := degree p;
				d := dp - dq;
				dr := (next d) quo 2;
				low := dq + dr;
				while d >= dr repeat {
					c := divByLq(leadingCoefficient p);
					qt := add!(qt, c, d);
					p := add!(p, -c, d, q, low, dp);
					if zero? p then d := prev dr;	-- stop
					else {
						dp := degree p;
						d := dp - dq;
					}
				}
				high := prev(tdeg + dr);
				(a, n) := trailingTerm p;
				d := n - tdeg;
				while d < dr and a ~= 0 repeat {
					c := divByTq a;
					qt := add!(qt, c, d);
					p := add!(p, -c, d, q, n, high);
					(a, n) := trailingTerm p;
					d := n-tdeg;
				}
				qt;
			}

			-- returns p x^n / c, always makes a copy
			local shift(c:R, p:%, n:Z):Partial % == {
				import from Partial R;
				assert(~zero? c);
				assert(~unit? c);
				q:% := 0;
				pos?:Boolean := n >= 0;
				for term in p repeat {
					(a, d) := term;
					u := exactQuotient(a, c);
					failed? u => return failed;
					e := d + n;
					if pos? or e >= 0 then
						q := add!(q, retract u, e);
				}
				[q];
			}

			exactQuotient(p:%, q:%):Partial % == {
				import from Z, R, Partial R;
				assert(~zero? q);
				zero? p => [0];
				one? q => [p];
				-- check first whether trailing degrees divide
				(cp, dp) := trailingTerm p;
				(cq, dq) := trailingTerm q;
				dp < dq => failed;
				u := reciprocal(lq := leadingCoefficient q);
				inv? := ~failed? u;
				-- special case where q = lq x^dq
				(dgq := degree q) = dq => {
					inv? => [shift(p, retract u, -dq)];
					shift(lq, p, -dq);
				}
				-- special case where q = a x + b
				inv? and one? dgq => {
					ilq := retract u;
					(qt, v) := horner(p, ilq,
							- ilq*coefficient(q,0));
					zero? v => [qt];
					failed;
				}
				-- general case
				a := copy p;
				uqt:Partial % := {
					inv? => [exquo!(retract u, a, q)];
					-- R cannot be a field at this point
					failed? exactQuotient(cp, cq) =>
						return failed;
					exquo!(a, q);
				}
				failed? uqt => failed;
				equal?(p, retract uqt, q, dgq) => uqt;
				failed;
			}

			-- ilq = inverse of leading coeff of q
			-- does not necessarily return an exact quotient
			-- computes a candidate p / q, destroying p but not q
			local exquo!(ilq:R, p:%, q:%):% == {
				import from Z, R;
				assert(~zero? ilq);
				assert(~zero? p); assert(~zero? q);
				dq := degree q;
				quot:% := 0;
				while (~zero? p) repeat {
					dp := degree p;
					(d := dp-dq) < 0 => return quot;

					c := ilq * leadingCoefficient p;
					quot := add!(quot, c, d);
					p := add!(p, - c, d, q, dq, dp);
				}
				quot;
			}

			-- should not be used when lq is a unit
			-- does not necessarily return an exact quotient
			-- computes a candidate p / q, destroying p but not q
			local exquo!(p:%, q:%):Partial % == {
				assert(~zero? p); assert(~zero? q);
				import from Z, R, Partial R;
				dq := degree q;
				lq := leadingCoefficient q;
				assert(~unit? lq);
				quot:% := 0;
				while (~zero? p) repeat {
					dp := degree p;
					d := dp-dq;
					d < 0 => return [quot];
					c:=exactQuotient(leadingCoefficient p,
	 							lq);
					failed? c => return failed;
					cc := retract c;
					quot := add!(quot, cc, d);
					p := add!(p, - cc, d, q, dq, dp);
				}
				[quot];
			}

			-- order at x^n of q
			local umonomorder(n:Z)(q:%):Z == {
				assert(~zero? q);
				assert(n > 1);
				trailingDegree(q) quo n;
			}

			-- return the multiplicity n of a as a root of q
			-- as well as c^n q / (x - a)^n
			local computeRootOrder(c:R, a:R)(q:%):(Z, %) == {
				import from Z;
				assert(~zero? q);
				assert(~zero? a);
				n:Z := 0;
				lastq := q;
				(q, v) := horner(lastq, c, a);
				while zero? v repeat {
					n := next n;
					lastq := q;
					(q, v) := horner(lastq, c, a);
				}
				assert(~zero? v);
				(n, lastq);
			}

			-- return the multiplicity of a as a root of q
			local rootOrder(a:R)(q:%):Z == {
				(n, q) := computeRootOrder(1, a)(q);
				n;
			}

			-- redefined: special cases p = u x^n or p = a x + b
			order(p:%):% -> Z == {
				import from Z, R, Partial R;
				assert(~zero? p);
				d := degree p;
				assert(~zero? d);
				-- special case where p = u x^n
				umonom? p => {
					one? d => trailingDegree;
					umonomorder d;
				}
				-- special case where p = a x + b, a unit
				one? d and ~failed?(_
					u := reciprocal leadingCoefficient p) =>
						rootOrder(- coefficient(p,0) *_
								retract u);
				-- general case
				generalOrder p;
			}

			local generalOrder(p:%)(q:%):Z == {
				(n, q) := computeOrder(p)(q);
				n;
			}

			-- order at u^{-1} x^n of q
			local umonomorderquo(u:R, n:Z)(q:%):(Z, %) == {
				assert(~zero? q);
				zero?(m := trailingDegree(q) quo n) => (m, q);
				(m, shift(q, u^m, -m * n));
			}

			-- redefined: special cases p = u x^n or p = a x + b
			orderquo(p:%):% -> (Z, %) == {
				import from Z, R, Partial R;
				assert(~zero? p);
				d := degree p;
				assert(~zero? d);
				-- special case where p = u x^n
				~failed?(u := umonom p) =>
						umonomorderquo(retract u, d);
				-- special case where p = a x + b, a unit
				one? d and ~failed?(_
					u:=reciprocal leadingCoefficient p) => {
						a := retract u;
						computeRootOrder(a,
							- coefficient(p,0) * a);
				}
				-- general case
				computeOrder p;
			}

			-- order of q at p
			local computeOrder(p:%)(q:%):(Z, %) == {
				import from Partial %;
				assert(~zero? p and ~zero? q and ~zero? degree p
							and ~umonom? p);
				n:Z := 0;
				while ~failed?(u := exactQuotient(q,p)) repeat {
					n := next n;
					q := retract u;
				}
				(n, q);
			}
		}

		if R has GcdDomain then {
			provablyIrreducible?(a:%):Boolean == {
				import from Z;
				assert(~zero? a);
				one? degree a;
			}

			someFactors(a:%):List % == {
				import from Product %;
				l:List % := empty;
				(c, p) := squareFree a;
				for term in p repeat {
					(b, e) := term; l := cons(b, l);
				}
				l;
			}

			squareFreePart(a:%):% == {
				(g, aa, dummy) :=
					gcdquo(a,differentiate a)$(%@GcdDomain);
				aa;
			}
		}

		if R has Field then {
			-- TEMPORARY: THOSE DEFAULTS CANNOT BE IN sit_euclid.as
			-- AS LONG AS THE COMPILER DOES EARLY-BINDING
			divide!(a:%, b:%, q:%):(%, %)	== divide(a, b);
			remainder!(a:%, b:%):%		== a rem b;

			divide(a:%, b:%):(%, %) == monicDivide(a, b);
			(a:%) rem (b:%):%	== monicRemainder(a, b);

			(a:%) quo (b:%):% == {
				(q, r) := divide(a, b);
				q;
			}

			euclideanSize(a:%):Z == {
				assert(~zero? a);
				degree a;
			}

			sparseMultiple(p:%, N:Z):% == {
				import from I, LinearAlgebra(R, M R);
				assert(N > 0);
				zero? p or zero?(d := degree p) or one? N => p;
				m := machine d;
				firstDependence(remgen(N, p, m), m)::%;
			}

			-- generates x^{in} mod p for i = 0,1,...
			local remgen(n:Z,p:%,dim:I):Generator V R == generate {
				assert(n > 1);
				v:V R := zero dim;
				vec! := vectorize! v;
				y:% := 1;
				yield vec! y;
				repeat {
					y := remainder!(shift(y, n), p);
					yield vec! y;
				}
			}

			rationalReconstruction(m:%):% -> Partial Cross(%,%) == {
				import from Z;
				assert(~zero? m); assert(degree(m) > 0);
				b := prev(degree m) quo 2;
				(r:%):Partial Cross(%, %) +->
					rationalReconstruction(r, m, b, b);

			}
		}

		if R has Specializable then {
			specialization(Image:CommutativeRing):_
				PartialFunction(%,Image) == {
				import from Z, R, Image, Partial Image;
				import from PartialFunction(R, Image);
				f := partialMapping(specialization(Image)$R);
				x := random()$Image;
				-- uses Horner's evaluation specializing the
				-- coefficients as we go along, which is more
				-- efficient than computing f p x
				partialFunction((p:%):Partial(Image) +-> {
					zero? p => [0];
					zero? x => f coefficient(p, 0);
					failed?(vv := f leadingCoefficient p) =>
						failed;
					v := retract vv;
					for i in prev degree p..0 by -1 repeat {
						failed?(u:=f coefficient(p,i))=>
							return failed;
						v := x * v + retract u;
					}
					[v];
				})
			}
		}
	}
}

